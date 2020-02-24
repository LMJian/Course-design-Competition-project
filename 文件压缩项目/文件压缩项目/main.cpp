#define _CRT_SECURE_NO_WARNINGS 1
#include"FileCompressHuffman.hpp"
#include"FileCompressHuffman.cpp"
#include"LZ77.hpp"

void test() {
	LZ77 lz;
	FileCompressHuffman tree;
	lz.CompressionFile("IMG_5725.PNG");

	tree.CompressFile("LZ77.bin");   //ָ��Ҫѹ�����ļ�             

	tree.UnCompressFile("Huffman.bin");    //ָ��Ҫ��ѹ���ļ�

	lz.UnCompressionFile("h2.bin");
}
int main() {
	test();
	_CrtDumpMemoryLeaks();
	system("pause");
	return 0;
}