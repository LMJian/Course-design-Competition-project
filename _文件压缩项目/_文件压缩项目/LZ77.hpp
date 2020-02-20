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
	UCH LongestMatch(USH matchHead, USH& curMatchDist, USH start);   //���ƥ��
	UCH* pWin_;                                           //���������ѹ�����ݵĻ�����
	LZHashTable ht_;
};