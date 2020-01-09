#define _CRT_SECURE_NO_WARNINGS 1
#include"FileCompressionHuffManTree.hpp"
#include"FileExtractionHuffmanTree.hpp"

void TestFileCompression() {       //ѹ���ļ�
	FileCompressionHuffManTree tree;  //ָ����Ҫѹ�����ļ�λ��
	tree.InitQue();                //���ļ���ͳ��ÿ���ַ��ĸ�����Ȼ�󴴽���㣬ѹ�����ȼ�����
	tree.InitTree();               //��ʼ��������
	tree.WriteBin();               //��ʼѹ��
}

void TestFileExtraction() {        //��ѹ�ļ�
	FileExtractionHuffManTree tree;
	tree.InitTree();          //����ͷ����Ϣ���ؽ�������
}

int main() {
	TestFileCompression();

	TestFileExtraction();

	system("pause");
	return 0;
}