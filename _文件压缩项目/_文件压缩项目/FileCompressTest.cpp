#define _CRT_SECURE_NO_WARNINGS 1
#include "Huffman.hpp"
#include "FileCompressHuffman.h"

int main()
{
	FileCompressHuffman fc;
	fc.CompressFile("test.txt");
	fc.UnCompressFile("2.txt");
	return 0;
}