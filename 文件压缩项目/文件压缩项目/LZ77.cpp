#define _CRT_SECURE_NO_WARNINGS 1
#include"LZ77.hpp"
#include<iostream>

const USH MIN_LOOKAHEAD = MAX_MATCH + MIN_MATCH + 1; //要保证最后一次匹配,最大匹配长度258 
const USH MAX_DIST = WSIZE - MIN_LOOKAHEAD;    

LZ77::LZ77()
	:pWin_(new UCH[WSIZE * 2])
	,ht_(WSIZE)
{}
LZ77::~LZ77() {
	delete[] pWin_;
	pWin_ = nullptr;
}
void LZ77::CompressionFile(const std::string& fileName) {
	FILE* fR = fopen(fileName.c_str(), "rb");
	if (!fR) {
		std::cout << "待压缩文件打开失败!" << std::endl;
		return;
	}
	//计算文件大小
	fseek(fR, 0, SEEK_END);
	ULL fileSize = ftell(fR);
	if (fileSize <= MIN_MATCH) {
		std::cout << "文件太小！！！不进行压缩！！" << std::endl;
		return;
	}
	//将文件指针置回起始位置
	fseek(fR, 0, SEEK_SET);
	//开始压缩
	//读取文件到缓冲区
	size_t readSize = fread(pWin_, sizeof(UCH), 2 * WSIZE, fR);

	//计算前两个字符的哈希地址
	USH hashAddr = 0;
	for (UCH i = 0; i < MIN_MATCH - 1; ++i) {
		ht_.hashFunc(hashAddr, pWin_[i]);
	}

	USH matchHead = 0;//匹配链的头
	USH curMatchLen = 0; //最长匹配链的长度
	USH curMatchDist = 0; //最长匹配链的距离
	USH start = 0;

	FILE* fW = fopen("2.txt", "wb");//写压缩数据
	FILE* fWT = fopen("3.txt", "wb");//写数据的标记
	if (!fW || !fWT) {
		std::cout << "2.txt/3.txt 文件打开失败" << std::endl;
		return;
	}

	UCH chNum = 0;   //将要写入的标记
	UCH bitCount = 0; //记录 标记写了多少位
	while (readSize) {
		//将首字符串插入哈希表
		ht_.Insert(matchHead, pWin_[start + 2], start, hashAddr);
		curMatchLen = 0;
		curMatchDist = 0;

		if (matchHead > 0) { //找到了匹配链
			//找最长匹配链
			curMatchLen = LongestMatch(matchHead, curMatchDist, start);
		}
		if (curMatchLen < MIN_MATCH) {//未找到匹配链
			//写原字符
			fputc(pWin_[start], fW);
			//写标记
			WriteFlag(fWT, chNum, bitCount, false);
			++start;
			--readSize;
		}
		else {
			//写长度
			UCH chlen = curMatchLen - 3;
			fputc(chlen, fW);
			//写距离
			fwrite(&curMatchDist, sizeof(curMatchDist), 1, fW);
			//写标记
			WriteFlag(fWT, chNum, bitCount, true);
			//将匹配的字符串三个一组插入哈希表
			readSize -= curMatchLen;
			++start;
			--curMatchLen;//第一个字符已经插入
			while (curMatchLen) {
				ht_.Insert(matchHead, pWin_[start + 2], start, hashAddr);
				++start;
				--curMatchLen;
			}
		}
		if (readSize <= MIN_LOOKAHEAD)
			fillWindow(start, fR, readSize);
	}
	if (bitCount > 0 && bitCount < 8) {
		chNum <<= (8 - bitCount);
		fputc(chNum, fWT);
	}
	fclose(fWT);
	fclose(fR);
	//把标记文件写在数据文件的后面
	MergeFile(fW, fileSize);
	fclose(fW);
}
void LZ77::MergeFile(FILE* fW, ULL fileSize) {
	FILE* fR = fopen("3.txt", "rb");
	UCH *buff = new UCH[1024];
	size_t rSize = 0;
	while (1) {
		size_t readSize = fread(buff, sizeof(UCH), 1024, fR);
		if (readSize == 0)
			break;
		rSize += readSize;
		fwrite(buff, sizeof(UCH), readSize, fW);
	}
	fwrite(&rSize, sizeof(rSize), 1, fW);
	fwrite(&fileSize, sizeof(fileSize), 1, fW);
	delete[] buff;
	fclose(fR);
}
void LZ77::fillWindow(USH& start, FILE* fR, size_t& readSize) {
	if (start >= WSIZE) {
		memcpy(pWin_, pWin_ + WSIZE, WSIZE);
		memset(pWin_ + WSIZE, 0, WSIZE);
		
		start -= WSIZE;
		ht_.Update();

		if (!feof(fR))
			readSize += fread(pWin_ + WSIZE, sizeof(UCH), WSIZE, fR);
	}
}
USH LZ77::LongestMatch(USH matchHead, USH& MatchDist, USH start) {     //找最长匹配
	USH curMatchLen = 0;
	USH maxMatchLen = 0;
	USH maxMatchHead = 0;
	UCH matchCount = 255;

	//在先行缓冲区中查找匹配时，不能太远即不能超过MAX_DIST
	USH limit = start > MAX_DIST ? start - MAX_DIST : 0;
	do {
		UCH* pStart = pWin_ + start;
		UCH* pEnd = pStart + MAX_MATCH;

		UCH* ptr = pWin_ + matchHead;
		curMatchLen = 0;

		while (pStart < pEnd&&*pStart == *ptr) {
			++curMatchLen;
			++pStart;
			++ptr;
		}
		if (maxMatchLen < curMatchLen) {
			maxMatchLen = curMatchLen;
			maxMatchHead = matchHead;
		}
	} while ((matchHead = ht_.GetNext(matchHead)) > limit&&matchCount--);
	MatchDist = start - maxMatchHead;
	return maxMatchLen;
}
void LZ77::WriteFlag(FILE* file, UCH& chNum, UCH& bitCount, bool isLen) {
	chNum <<= 1;
	if (isLen) 
		chNum |= 1;
	++bitCount;
	if (bitCount == 8) {
		fputc(chNum, file);
		bitCount = 0;
		chNum = 0;
	}
}
void LZ77::UnCompressionFile(const std::string& fileName) {

}