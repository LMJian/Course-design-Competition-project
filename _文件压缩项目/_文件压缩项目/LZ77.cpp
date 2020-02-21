#define _CRT_SECURE_NO_WARNINGS 1
#include"LZ77.hpp"
#include<iostream>
#include<assert.h>

const USH MIN_LOOKAHEAD = MAX_MATCH + MIN_MATCH + 1; //要保证最后一次匹配,最大匹配长度258 
const USH MAX_DIST = WSIZE - MIN_LOOKAHEAD;       // 待定******

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

	//1. 如果源文件的大小小于最小匹配长度 MIN_MATCH，则不进行处理
	if (fileSize <= MIN_MATCH) {          //此处是小于3个字符
		std::cout << "文件太小，不压缩" << std::endl;
		return;
	}
	//从压缩文件中读取一个缓冲区的数据到窗口中
	fseek(fIn, 0, SEEK_SET);
	size_t lookAhead = fread(pWin_, 1, 2 * WSIZE, fIn);
	USH hashAddr = 0;
	
	//设置起始hashAddr (前两个字符的hash地址)
	for (USH i = 0; i < MIN_MATCH - 1; ++i) {
		ht_.HashFunc(hashAddr,pWin_[i]);
	}

	//压缩:
	FILE* fOUT = fopen("2.txt", "wb");
	assert(fOUT);
	USH start = 0;           //查找字符串在缓冲区的地址
	
	//与查找最长匹配相关的变量
	USH matchHead = 0;       //匹配字符串的头
	USH curMatchLength = 0;  //最长匹配字符串的长度
	USH curMatchDist = 0;    //最长匹配字符串的距离

	//与写标记相关的变量
	UCH chFlag = 0;
	UCH bitCount = 0;
	bool IsLen = false;
	
	//写标记的文件
	FILE* fOutF = fopen("3.txt", "wb");
	assert(fOutF);

	//lookAhead表示先行缓冲区中剩余字节的个数
	while (lookAhead) {
		//1.将当前三个字符插入到哈希表中，并获取匹配链的头
		ht_.Insert(matchHead, pWin_[start + 2], start, hashAddr);
		
		curMatchLength = 0;
		curMatchDist = 0;
		//2.验证在查找缓冲区中是否找到匹配，如果有匹配，找最长匹配
		if (matchHead) {
			//顺着匹配链找最长匹配,最终带出<长度，距离>对
			curMatchLength = LongestMatch(matchHead, curMatchDist, start);
		}

		//3.验证是否找到匹配
		if (curMatchLength < MIN_MATCH) {
			//在查找缓冲区中未找到重复字符串
			//将start位置的字符写入到压缩文件中
			fputc(pWin_[start], fOUT); //写字符
			
			//写当前原字符对应的标记
			WriteFlag(fOutF, chFlag, bitCount, false);//写标记
			++ start;
			lookAhead--;
		}
		else {
			//找到匹配
			//将《长度，距离》对写入压缩文件中
			//写长度
			UCH chLen = curMatchLength - 3;
			fputc(chLen, fOUT);

			//写距离
			fwrite(&curMatchDist, sizeof(curMatchDist), 1, fOUT);

			//写当前原字符对应的标记
			WriteFlag(fOutF, chFlag, bitCount, true);
			
			//更新先行缓冲区中剩余的字节数
			lookAhead -= curMatchLength;

			//将已经匹配的字符串按照三个一组将其插入到哈希表中
			--curMatchLength;  //当前字符串已经插入
			++start;
			while (curMatchLength) {
				ht_.Insert(matchHead, pWin_[start + 2], start, hashAddr);
				--curMatchLength;
				++start;
			}
		}
	}
	//标记位数如果不够八位
	if (bitCount > 0 && bitCount < 8) {
		chFlag <<= (8 - bitCount);
		fputc(chFlag, fOutF);
	}
	//将压缩数据文件和标记信息文件合并
	fclose(fIn);
	fclose(fOUT);
	fclose(fOutF);
}

//chFlag:该字节中的每个比特位是用来区分当前字节是原字符还是长度?
//0:原字符
//1:长度

//bitCount:该字节中的多少个比特位已经被设置
//isCharOrLen:代表该字节是源字符还是长度
void LZ77::WriteFlag(FILE* fOUT, UCH& chFlag, UCH& bitCount, bool isLen){
	chFlag <<= 1;
	if (isLen)
		chFlag |= 1;

	bitCount++;
	if (bitCount == 8) {
		//将该标记写入到压缩文件中
		fputc(chFlag, fOUT);
		chFlag = 0;
		bitCount = 0;
	}
}
//匹配：是在查找缓冲区中进行的，查找缓冲区中可能会找到多个匹配
//输出：需要最长匹配
//注意：可能会遇到环状链    解决：设置最大匹配次数
//       匹配是在MAX_DIST范围内进行匹配的，太远的距离则不进行匹配

//在找的过程中，需要将每次找到的匹配结果进行比较，保持最长匹配
USH LZ77::LongestMatch(USH matchHead, USH& MatchDist, USH start) {     //找最长匹配
	USH curMatchLen = 0;  //一次匹配的长度
	USH maxMatchLen = 0;
	UCH maxMatchCount = 255;   //最大的匹配次数，解决环状链
	USH curMatchStart = 0;     //当前匹配在查找缓冲区中的起始位置

	//在先行缓冲区中查找匹配时，不能太远即不能超过MAX_DIST
	USH limit = start > MAX_DIST ? start - MAX_DIST : 0;

	do {
		//匹配范围
		//先行缓冲区
		UCH* pstart = pWin_ + start;
		UCH* pend = pstart + MAX_MATCH;

		//查找缓冲区匹配串的起始
		UCH* pMatchStart = pWin_ + matchHead;
		curMatchLen = 0;

		//可以进行本次匹配
		while (pstart < pend&&*pstart == *pMatchStart) {
			++curMatchLen;
			++pstart;
			++pMatchStart;
		}
		//一次匹配结束
		if (curMatchLen > maxMatchLen) {
			maxMatchLen = curMatchLen;
			curMatchStart = matchHead;
		}
	} while ((matchHead = ht_.GetNext(matchHead)) > limit&&maxMatchCount--);
	
	MatchDist = start - curMatchStart;
	return maxMatchLen;
}

void LZ77::UNCompressFile(const std::string& strFilePath) {
	//打开压缩文件和标记文件
	FILE* fInD = fopen("2.txt", "rb");
	if (nullptr == fInD) {
		std::cout << "压缩文件打开失败" << std::endl;
		return;
	}
	FILE* fInF = fopen("3.txt", "rb");
	if (nullptr == fInF) {
		fclose(fInD);
		std::cout << "标记文件打开失败" << std::endl;
		return;
	}

	//写解压缩数据
	FILE* fOut = fopen("4.txt", "wb");
	assert(fOut);

	FILE* fR = fopen("4.txt", "rb");
	UCH bitCount = 0;
	UCH chFlag = 0;
	while (!feof(fInD)) {
		//读取标记
		if (0 == bitCount) {
			chFlag=fgetc(fInF);
			bitCount = 8;
		}
		if (chFlag & 0x80) {
			//距离长度对
			USH matchLen = fgetc(fInD) + 3;
			USH matchDist = 0;
			fread(&matchDist, sizeof(matchDist), 1, fInD);

			//清空缓冲区
			fflush(fOut);
			//fR：读取前文匹配串中的内容
			UCH ch;
			fseek(fR, 0 - matchDist, SEEK_END);
			while (matchLen) {
				ch = fgetc(fR);
				fputc(ch, fOut);
				--matchLen;
			}
		}
		else {
			//原字符
			UCH ch = fgetc(fInD);
			fputc(ch, fOut);
		}
		chFlag <<= 1;
		bitCount--;
	}
	fclose(fInD);
	fclose(fInF);
	fclose(fOut);
	fclose(fR);
}