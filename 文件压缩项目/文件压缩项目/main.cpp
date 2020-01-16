#define _CRT_SECURE_NO_WARNINGS 1
#include"FileCompressionHuffManTree.hpp"
#include"FileExtractionHuffmanTree.hpp"

void TestFileCompression() {       //����ѹ��
	FileCompressionHuffManTree tree("IMG_5413.MP4");   //ָ��Ҫѹ�����ļ�             
	tree.Compress();              
}

void TestFileExtraction() {        //���Խ�ѹ
	FileExtractionHuffManTree tree("IMG_5413.bin");    //ָ��Ҫ��ѹ���ļ�
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