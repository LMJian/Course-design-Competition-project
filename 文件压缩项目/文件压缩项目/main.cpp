#include"HuffManTree.hpp"

int main() {
	//��ȡ��ѹ���ļ�������

	//������������������ȡÿ���ַ���Ӧ������
	HuffManTree tree("test.txt");
	tree.InitQue();
	tree.InitTree();
	//��ʼѹ���ļ�������

	system("pause");
	return 0;
}