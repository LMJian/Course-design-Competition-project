#pragma once
#include<string>
#include<iostream>
#include"Huffman.hpp"

class FileCompressHuffman {
public:
	FileCompressHuffman();
	void CompressFile(const std::string& fileName);
	void UnCompressFile(const std::string& fileName);
private:
	void ReadHead(FILE* pRead, std::vector<int>& count);
	void WriteHead(FILE* pWrite, const std::vector<int>& count);
	void GetHuffmanCode(HuffManTreeNode* ptr, std::vector<std::string>& charPassWord);
};
