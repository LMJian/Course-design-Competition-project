#define _CRT_SECURE_NO_WARNINGS 1
#include"FileCompressionHuffManTree.hpp"
#include"FileExtractionHuffmanTree.hpp"

void TestFileCompression() {       //����ѹ��
	FileCompressionHuffManTree tree("test.txt");   //ָ��Ҫѹ�����ļ�
	tree.InitQue();                
	tree.InitTree();             
	tree.WriteBin();              
}

void TestFileExtraction() {        //���Խ�ѹ
	FileExtractionHuffManTree tree("test.bin");    //ָ��Ҫ��ѹ���ļ�
	tree.InitQue();
	tree.InitTree();
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