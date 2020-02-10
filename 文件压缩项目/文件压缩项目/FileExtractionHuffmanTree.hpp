#pragma once
#include"FileCompressionHuffManTree.hpp"

class FileExtractionHuffManTree {
	typedef HuffManTreeNode Node;
	typedef HuffManTreeNode* PNode;
public:
	FileExtractionHuffManTree(const std::string fileName)
		:fileName_(fileName)
		, root_(nullptr)
	{}
	~FileExtractionHuffManTree() {
		std::stack<PNode> sa;
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
	void UnCompress(){     //��ʼ��ѹ��
		FILE *pRead = fopen(fileName_.c_str(), "rb");
		if (NULL == pRead) {
			perror("fopen file error!\n");
			return;
		}
		
		InitQue(pRead);  //��ȡͷ����Ϣ��ѹ�����ȶ���

		InitTree();      //��������Ľ���ʼ��ΪHuffman��

		std::string newFileName = "new" + fileName_.substr(0, fileName_.rfind('.')) + endName_;
		FILE *pWrite = fopen(newFileName.c_str(), "wb");
		if (pWrite == NULL) {
			perror("fopen file error!");
			return;
		}
		unsigned char ch = 0;
		PNode ptr = root_;
		int count = root_->Weight_;
		char readBuf[1024] = { 0 };
		while (1) {
			int n = fread(readBuf, 1, 1024, pRead);
			for (int i = 0; i < n; ++i) {
				ch = readBuf[i];
				for (int j = 0; j < 8; ++j) {
					if (ch & 0x80) {//���λΪ1
						ptr = ptr->pRight_;
					}
					else {
						ptr = ptr->pLeft_;
					}
					if (!ptr->pLeft_ && !ptr->pRight_) {//�Ѿ��ҵ�Ҷ�ӽڵ�
						unsigned char bufC = ptr->Ch_;
						fputc(bufC, pWrite);
						--count;
						if (!count)
							goto LOOP;
						ptr = root_;
					}
					ch = ch << 1;
				}
			}
			
		}
		LOOP:
		fclose(pRead);
		fclose(pWrite);
	}
private:
	void InitTree() {     //��������Ľ���ʼ��Ϊ������
		while (que_.size() > 1) {
			PNode left = que_.top();
			que_.pop();
			PNode right = que_.top();
			que_.pop();
			PNode ptr = new Node(left->Weight_ + right->Weight_);
			ptr->pLeft_ = left;
			ptr->pRight_ = right;
			que_.push(ptr);
		}
		root_ = que_.top();
		que_.pop();
	}
	void InitQue(FILE* pRead) {
		//��ȡ�ļ���׺��Ϣ
		char buf[1024] = { 0 };
		fgets(buf, 1024, pRead);
		endName_ = buf;
		endName_.erase(endName_.end() - 1);
		//��ȡ������Ϣ
		fgets(buf, 1024, pRead);
		int rows = atoi(buf);
		for (int i = 0; i < rows; ++i) {
			fgets(buf, 1024, pRead);
			if (buf[0] == '\n') {
				fgets(buf + 1, 1023, pRead);
			}
			unsigned char ch = buf[0];
			int ret = atoi(buf + 2);
			PNode ptr = new Node(ret, ch);
			que_.push(ptr);
		}
	}
	PNode root_;                                       //���ĸ��ڵ�
	std::string fileName_;                                  //Ҫѹ�����ļ���
	std::priority_queue<PNode, std::vector<PNode>, Less> que_;     //���ȶ��У�С����
	std::string endName_ = "";
};
