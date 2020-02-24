#define _CRT_SECURE_NO_WARNINGS 1
#include "Huffman.hpp"
#include"FileCompressHuffman.h"
#include"LZ77.hpp"

int main()
{
	//FileCompressHuffman fc;
	//fc.CompressFile("test.txt");
	//fc.UnCompressFile("2.txt");
	LZ77 lz;
	lz.CompressFile("LMJian.png");
	//lz.UNCompressFile("2.txt");
	system("pause");
	return 0;
}