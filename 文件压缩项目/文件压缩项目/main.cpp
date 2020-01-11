#define _CRT_SECURE_NO_WARNINGS 1
#include"FileCompressionHuffManTree.hpp"
#include"FileExtractionHuffmanTree.hpp"

void TestFileCompression() {       //����ѹ��
	FileCompressionHuffManTree tree("IMG_5392.MP4");   //ָ��Ҫѹ�����ļ�
	tree.InitQue();                
	tree.InitTree();             
	tree.WriteBin();              
}

void TestFileExtraction() {        //���Խ�ѹ
	FileExtractionHuffManTree tree("IMG_5392.bin");    //ָ��Ҫ��ѹ���ļ�
	tree.InitTree();       
}

int main() {
	TestFileCompression();
	_CrtDumpMemoryLeaks();
	TestFileExtraction();
	_CrtDumpMemoryLeaks();
	system("pause");
	return 0;
}