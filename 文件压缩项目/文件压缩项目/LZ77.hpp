#pragma once
#include<string>
#include"HashTable.hpp"

class LZ77 {
public:
	LZ77();
	~LZ77();
	void CompressionFile(const std::string& fileName);
	void UnCompressionFile(const std::string& fileName);
	
private:
	USH  LongestMatch(USH matchHead, USH &curMatchDist, USH start);
	void WriteFlag(FILE* file, UCH& chNum, UCH& bitCount, bool isLen);
	UCH* pWin_;
	HashTable ht_;
};