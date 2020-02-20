#define _CRT_SECURE_NO_WARNINGS 1
#include"LZ77.hpp"
#include<iostream>
#include<assert.h>

LZ77::LZ77()
	:pWin_(new UCH[WSIZE*2])
	,ht_(WSIZE)         //初始化hash表大小
{}
LZ77::~LZ77() {
	delete[] pWin_;
	pWin_ = nullptr;
}
void LZ77 :: CompressFile(const std::string& strFilePath) {
	FILE* fIn = fopen(strFilePath.c_str(), "rb");
	if (nullptr == fIn) {
		std::cout << "打开文件失败" << std::endl;
		return;
	}
	//获取文件大小
	fseek(fIn, 0, SEEK_END);
	ULL fileSize = ftell(fIn);

	//1. 如果源文件的大小小于MIN_MATCH，则不进行处理
	if (fileSize <= MIN_MATCH) {          //此处是小于3个字符
		std::cout << "文件太小，不压缩" << std::endl;
		return;
	}
	//从压缩文件中读取一个缓冲区的数据到窗口中
	fseek(fIn, 0, SEEK_SET);
	size_t lookAhead = fread(pWin_, 1, 2 * WSIZE, fIn);
	USH hashAddr = 0;
	
	//abcdefg...设置起始hashAddr (前两个字符的hash地址)
	for (USH i = 0; i < MIN_MATCH - 1; ++i) {
		ht_.HashFunc(hashAddr,pWin_[i]);
	}

	//压缩:
	FILE* fOUT = fopen("2.txt", "wb");
	assert(fOUT);
	USH start = 0;           //查找字符串地址
	USH matchHead = 0;       //第一次找到匹配字符串的起始位置
	UCH curMatchLength = 0;  //最长匹配字符串的长度
	USH curMatchDist = 0;    //最长匹配字符串的距离

	//lookAhead表示先行缓冲区中剩余字节的个数
	while (lookAhead) {
		//1.将当前三个字符插入到哈希表中，并获取匹配链的头
		ht_.Insert(matchHead, pWin_[start + 2], start, hashAddr);
		//2.验证在查找缓冲区中是否找到匹配，如果有匹配，找最长匹配
		if (matchHead) {
			//顺着匹配链找最长匹配,最终带出<长度，距离>对
			curMatchLength = LongestMatch(matchHead, curMatchDist, start);
		}

		//3.验证是否找到匹配
		if (curMatchLength < MIN_MATCH) {
			//在查找缓冲区中未找到重复字符串
			//将start位置的字符写入到压缩文件中
			fputc(pWin_[start], fOUT);   //**//
			++ start;
			lookAhead--;
		}
		else {
			//找到匹配
			//将《长度，距离》对写入压缩文件中
			//写长度
			fputc(curMatchLength, fOUT);

			//写距离
			fwrite(&curMatchDist, sizeof(curMatchDist), 1, fOUT);

			//更新先行缓冲区中剩余的字节数
			lookAhead -= curMatchLength;

			//将已经匹配的字符串按照三个一组将其插入到哈希表中
			--curMatchLength;  //当前字符串已经插入
			while (curMatchLength) {
				++ start;
				ht_.Insert(matchHead, pWin_[start], start, hashAddr);
				--curMatchLength;
			}
		}
	}
}
UCH LZ77::LongestMatch(USH hashHead, USH& MatchStart, USH start) {     //找最长匹配
	// 哈希链的最大遍历长度，防止造成死循环 
	int chain_length = 256; 
	// 始终保持滑动窗口为WSIZE，因为最小的超前查看窗口中有MIN_LOOKAHEAD的数据 
	// 因此只搜索_start左边MAX_DIST范围内的串 
	USH limit = start > MAX_DIST ? start - MAX_DIST : 0; 
	// 待匹配字符串的最大位置

	// [pScan, strend] 
	UCH* pScan = pWin_ + start; 
	UCH* strend = pScan + MAX_MATCH - 1; 
	
	// 本次链中的最佳匹配 
	int bestLen = 0; 
	UCH* pCurMatchStart; 
	USH curMatchLen = 0; 
	
	// 开始匹配 
	do{ 
		// 从搜索区hashHead的位置开始匹配 
		pCurMatchStart = pWin_ + hashHead; 
		while (pScan < strend && *pScan == *pCurMatchStart) { 
			pScan++; 
			pCurMatchStart++; 
		}
		
		// 本次匹配的长度和匹配的起始位置 
		curMatchLen = (MAX_MATCH - 1) - (int)(strend - pScan); 
		pScan = strend - (MAX_MATCH - 1); 
		
		/*更新最佳匹配的记录*/ 
		if (curMatchLen > bestLen) { 
			MatchStart = hashHead; 
			bestLen = curMatchLen; 
		} 
	} while ((hashHead = hash_.prev_[hashHead & WMASK]) > limit && --chain_length != 0); 
	return curMatchLen;
}

void UNCompressFile(const std::string& strFilePath);
