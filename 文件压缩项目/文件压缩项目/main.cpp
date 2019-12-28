#include"HuffManTree.hpp"

int main() {
	//获取所压缩文件的内容

	//建立哈夫曼树，并获取每个字符对应的密码
	HuffManTree tree("test.txt");
	tree.InitQue();
	tree.InitTree();
	//开始压缩文件并保存

	system("pause");
	return 0;
}