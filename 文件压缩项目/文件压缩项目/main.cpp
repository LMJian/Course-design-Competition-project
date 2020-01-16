#define _CRT_SECURE_NO_WARNINGS 1
#include"FileCompressionHuffManTree.hpp"
#include"FileExtractionHuffmanTree.hpp"

void TestFileCompression() {       //测试压缩
	FileCompressionHuffManTree tree("IMG_5413.MP4");   //指定要压缩的文件             
	tree.Compress();              
}

void TestFileExtraction() {        //测试解压
	FileExtractionHuffManTree tree("IMG_5413.bin");    //指定要解压的文件
	tree.UnCompress();
}

int main() {
	TestFileCompression();
	_CrtDumpMemoryLeaks();
	TestFileExtraction();
	_CrtDumpMemoryLeaks();
	system("pause");
	return 0;
}