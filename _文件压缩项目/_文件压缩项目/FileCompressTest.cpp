#define _CRT_SECURE_NO_WARNINGS 1
#include"FileCompressHuffman.h"
#include"LZ77.hpp"
#include<time.h>
using namespace std;

void test() {
	LZ77 lz;
	FileCompressHuffman tree;
	double left=clock();
	lz.CompressionFile("test.txt");

	tree.CompressFile("LZ77.bin");   //ָ��Ҫѹ�����ļ�             

	tree.UnCompressFile("Huffman.bin");    //ָ��Ҫ��ѹ���ļ�

	lz.UnCompressionFile("h2.bin");
	
	double right = clock();
	cout << (right - left) / 1000 << endl;
}
int main() {
	test();
	_CrtDumpMemoryLeaks();
	system("pause");
	return 0;
}