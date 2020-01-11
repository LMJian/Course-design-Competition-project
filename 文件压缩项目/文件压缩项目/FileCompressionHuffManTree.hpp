#pragma once
#include<iostream>
#include<fstream>
#include<queue>
#include<string>
#include<stack>
using namespace std;

struct HuffManTreeNode {
	HuffManTreeNode(int weight, unsigned char ch = 0)
		:pLeft_(nullptr)
		,pRight_(nullptr)
		,Weight_(weight)
		,Ch_(ch)
	{}
	HuffManTreeNode *pLeft_;
	HuffManTreeNode *pRight_;
	int Weight_;                    //Ȩֵ
	unsigned char Ch_;              //��ѹ���ַ�
};

class Less {            //ʹ��С��
public:
	bool operator()(const HuffManTreeNode* pLeft, const HuffManTreeNode* pRight) {
		return pLeft->Weight_ > pRight->Weight_;
	}
};

class FileCompressionHuffManTree {
	typedef HuffManTreeNode Node;
	typedef HuffManTreeNode* PNode;
public:
	FileCompressionHuffManTree(const string fileName)
		:fileName_(fileName)
		, root_(nullptr)
	{}
	~FileCompressionHuffManTree() {
		stack<PNode> sa;
		PNode ptr = root_;
		while (!sa.empty() || ptr) {
			while (ptr) {
				sa.push(ptr);
				ptr = ptr->pLeft_;
			}
			PNode del = sa.top();
			sa.pop();
			ptr = del->pRight_;
			delete del;
		}
	}
	
	void Compress() {     //��ʼѹ��
		
		InitQue();//ͳ���ַ�Ƶ�ʣ�ѹ�����ȶ���

		InitTree();//��������Ľ�㹹���Huffman��

		if (root_)
			InitChPW(root_);    //��ȡÿ���ַ���Ӧ�����룬          ���˴��ǵݹ���Ҫ�Ż���
		
		string BinFileName = fileName_.substr(0, fileName_.rfind('.'));
		BinFileName += ".bin";
		FILE *pWrite = fopen(BinFileName.c_str(), "wb");
		if (pWrite == NULL) {
			perror("fopen file error!");
			return;
		}
		//д��ͷ����Ϣ
		string headstr = "";
		string EndFormat= fileName_.substr(fileName_.rfind('.'));//�ļ�β����ʽ
		int rows = 0;
		for (int i = 0; i < 256; ++i) {
			if (count_[i] > 0) {
				unsigned char ch = i;
				int ret = count_[i];
				char str[1024] = { 0 };
				sprintf(str, "%c:%d\n", ch, ret);
				headstr += str;
				++rows;
			}
		}
		char str[1024] = { 0 };
		sprintf(str, "%d\n", rows);
		headstr = str + headstr;
		headstr = EndFormat + '\n' + headstr;
		fwrite(headstr.c_str(), 1, headstr.size(), pWrite);
		
		FILE *pRead = fopen(fileName_.c_str(), "rb");
		if (pRead == NULL) {
			perror("fopen file error!");
			return;
		}
		unsigned char buf[1024] = { 0 };                  //һ��ѹ��1024���ֽڵ��ļ�����
		unsigned char ch = 0;
		int index = 0;
		while (1) {
			int n = fread(buf, 1, 1024, pRead);
			for (int i = 0; i < n; ++i) {
				string passWord = charPassWord_[buf[i]];        //��ȡbuf[i]��Ӧ������
				for (size_t j = 0; j < passWord.size(); ++j) {
					ch <<= 1;
					++index;
					if (passWord[j] == '1') {
						ch |= 1;
					}
					if (index == 8) {
						fputc(ch, pWrite);
						index = 0;
						ch = 0;
					}
				}
			}
			if (n < 1024)
				break;
		}
		if(index > 0){
			ch = ch << (8 - index);
			fputc(ch, pWrite);
		}
		fclose(pRead);
		fclose(pWrite);
	}

private:
	void InitQue() {
		FILE *pFile = fopen(fileName_.c_str(), "rb");//��Ϊ����ѹ���Ĳ�һ�����ı��ļ�
													 //�ö����Ʒ�ʽ�򿪸�Ϊ����
		if (pFile == NULL) {
			perror("fopen file error!");
			return;
		}
		unsigned char buf[1024] = { 0 };   //һ�ζ�ȡ�ļ���1024���ֽ�
										   //���ﲻ�ܳ��ָ�������Ϊ�޷���Ϊcount������±꣬����Ҫ��unsigned char 
		while (1) {
			int n = fread(buf, 1, 1024, pFile);
			for (int i = 0; i < n; ++i) {
				++count_[buf[i]];
			}
			if (n < 1024)
				break;
		}
		for (int i = 0; i < 256; ++i) {
			if (count_[i] > 0) {              //ɸѡ���ֹ����ַ���ѹ�����
				PNode ptr = new Node(count_[i], i);
				que_.push(ptr);
			}
		}
		fclose(pFile);
	}

	void InitTree() {               //��������Ľ���ʼ��Ϊ������
		while (que_.size() > 1) {
			PNode left = que_.top();
			que_.pop();
			PNode right = que_.top();
			que_.pop();
			PNode newNode = new Node(left->Weight_ + right->Weight_);
			newNode->pLeft_ = left;
			newNode->pRight_ = right;
			que_.push(newNode);
		}
		root_ = que_.top();
		que_.pop();
	}
	void InitChPW(PNode ptr) {
		if (!ptr->pLeft_ && !ptr->pRight_) {    //����ΪҶ�ӽڵ�ʱ
			charPassWord_[ptr->Ch_] = passWord;
			return;
		}
		if (ptr->pLeft_) {
			passWord += '0';
			InitChPW(ptr->pLeft_);
			passWord.erase(passWord.end() - 1);
		}
		if (ptr->pRight_) {
			passWord += '1';
			InitChPW(ptr->pRight_);
			passWord.erase(passWord.end() - 1);
		}
	}
	string passWord = "";
	PNode root_;                                       //���ĸ��ڵ�
	string fileName_;                                  //Ҫѹ�����ļ���
	priority_queue<PNode,vector<PNode>,Less> que_;     //���ȶ��У�С����
	string charPassWord_[256];                          //����ַ���Ӧ������
	int count_[256] = { 0 };                            //ͳ���ַ����ֵĴ���	
};
