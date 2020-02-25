#pragma once
#include<string>
#include"LZHashTable.hpp"

class LZ77 {
public:
	LZ77();
	~LZ77();
	void CompressionFile(const std::string& fileName);
	void UnCompressionFile(const std::string& fileName);
private:
	void MergeFile(FILE* fW, ULL fileSize);
	void fillWindow(USH& start, FILE* fR, size_t& readSize);
	USH  LongestMatch(USH matchHead, USH &curMatchDist, USH start);
	void WriteFlag(FILE* file, UCH& chNum, UCH& bitCount, bool isLen);
private:
	UCH* pWin_;
	HashTable ht_;
	std::string fileName_;
};