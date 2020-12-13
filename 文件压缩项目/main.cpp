#define _CRT_SECURE_NO_WARNINGS 1
#include"FileCompressHuffman.hpp"
#include"FileCompressHuffman.cpp"
#include"LZ77.hpp"

void test() {
	LZ77 lz;
	FileCompressHuffman tree;
	lz.CompressionFile("IMG_5725.PNG");

	tree.CompressFile("LZ77.bin");   //指定要压缩的文件             

	tree.UnCompressFile("Huffman.bin");    //指定要解压的文件

	lz.UnCompressionFile("h2.bin");
}
int main() {
	test();
	_CrtDumpMemoryLeaks();
	system("pause");
	return 0;
}