#define _CRT_SECURE_NO_WARNINGS 1
#include"FileCompressionHuffManTree.hpp"
#include"FileExtractionHuffmanTree.hpp"

void TestFileCompression() {       //压缩文件
	FileCompressionHuffManTree tree;  //指定需要压缩的文件位置
	tree.InitQue();                //打开文件，统计每个字符的个数，然后创建结点，压入优先级队列
	tree.InitTree();               //初始化二叉树
	tree.WriteBin();               //开始压缩
}

void TestFileExtraction() {        //解压文件
	FileExtractionHuffManTree tree;
	tree.InitTree();          //根据头部信息，重建二叉树
}

int main() {
	TestFileCompression();

	TestFileExtraction();

	system("pause");
	return 0;
}