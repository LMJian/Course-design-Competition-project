#pragma once
#include<string>
#include<iostream>
#include"Huffman.hpp"

struct ParHuffNode {
	ParHuffNode(unsigned char ch, unsigned char len)
		:Ch_(ch)
		,bitLen_(len)
	{}
	unsigned char Ch_;
	unsigned char bitLen_;
	std::string str_;
};
struct DecTable {
	DecTable(unsigned char len, std::string str, unsigned char index)
		:bitLen_(len)
		, headStr_(str)
		, chCount_(0)
		, index_(index)
	{}
	unsigned char bitLen_;  //编码位长
	std::string headStr_;   //首编码
	unsigned char chCount_; //符号数量
	unsigned char index_;   //符号索引
};
class FileCompressHuffman {
public:
	FileCompressHuffman();
	void CompressFile(const std::string& fileName);
	void UnCompressFile(const std::string& fileName);
private:
	void ReadHead(FILE* pRead, std::vector<ParHuffNode*>& ChBitLen);
	void WriteHead(FILE* pWrite, const std::vector<ParHuffNode*>& ChBitLen);
	void GetBitLength(HuffManTreeNode* ptr, std::vector<ParHuffNode*>& ChBitLen, unsigned char bitLen);
	void MySort(std::vector<ParHuffNode*>& ChBitLen);
	bool Compare(const ParHuffNode* left, const ParHuffNode* right);
	void CalCharCode(std::vector<ParHuffNode*>& ChBitLen, std::vector<std::string>& strCode);
	std::string strPlusOne(const std::string& str, unsigned char count);
	void CalDecTable(const std::vector<ParHuffNode*>& ChBitLen, std::vector<DecTable*>& decTable);
	bool IsNext(int index, const std::vector<DecTable*>& decTable, const unsigned char* readRuf);
	void bitToStr(unsigned char* readBuf, const int n, unsigned char* strBuf);
};
