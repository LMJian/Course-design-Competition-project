#define _CRT_SECURE_NO_WARNINGS 1
#include"FileCompressionHuffManTree.hpp"
#include"FileExtractionHuffmanTree.hpp"
#include<time.h>
#include<Windows.h>
#include"LZ77.hpp"

void TestFileCompression() {       //测试压缩
	FileCompressionHuffManTree tree("test.txt");   //指定要压缩的文件             
	tree.Compress();              
}

void TestFileExtraction() {        //测试解压
	FileExtractionHuffManTree tree("test.bin");    //指定要解压的文件
	tree.UnCompress();
}

int main() {
	/*clock_t start, end;
	double sec = 0;
	start = clock();
	TestFileCompression();
	end = clock();
	sec = (double)(end - start) / CLOCKS_PER_SEC;
	printf("压缩耗时%f秒\n", sec);
	_CrtDumpMemoryLeaks();

	TestFileExtraction();
	_CrtDumpMemoryLeaks();*/
	LZ77 lz;
	lz.CompressionFile("test.txt");

	system("pause");
	return 0;
}