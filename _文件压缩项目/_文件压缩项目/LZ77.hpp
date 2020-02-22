#pragma once
#include<string>
#include"LZHashTable.hpp"

class LZ77 {
public:
	LZ77();
	~LZ77();
	void CompressFile(const std::string& strFilePath);
	void UNCompressFile(const std::string& strFilePath);
private:
	USH LongestMatch(USH matchHead, USH& curMatchDist, USH start);      //���ƥ��
	void WriteFlag(FILE* fOUT, UCH& chFlag, UCH& bitCount, bool isLen); //д����ļ�
	void MergeFile(FILE* fOut, ULL fileSize);
	void FillWindow(FILE* fIn, size_t& lookAhead, USH& start);
private:
	UCH* pWin_;                                                         //���������ѹ�����ݵĻ�����
	LZHashTable ht_;
};