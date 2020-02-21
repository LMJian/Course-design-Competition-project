#define _CRT_SECURE_NO_WARNINGS 1
#include"FileCompressionHuffManTree.hpp"
#include"FileExtractionHuffmanTree.hpp"
#include<time.h>
#include<Windows.h>
#include"LZ77.hpp"

void TestFileCompression() {       //����ѹ��
	FileCompressionHuffManTree tree("test.txt");   //ָ��Ҫѹ�����ļ�             
	tree.Compress();              
}

void TestFileExtraction() {        //���Խ�ѹ
	FileExtractionHuffManTree tree("test.bin");    //ָ��Ҫ��ѹ���ļ�
	tree.UnCompress();
}

int main() {
	/*clock_t start, end;
	double sec = 0;
	start = clock();
	TestFileCompression();
	end = clock();
	sec = (double)(end - start) / CLOCKS_PER_SEC;
	printf("ѹ����ʱ%f��\n", sec);
	_CrtDumpMemoryLeaks();

	TestFileExtraction();
	_CrtDumpMemoryLeaks();*/
	LZ77 lz;
	lz.CompressionFile("test.txt");

	system("pause");
	return 0;
}