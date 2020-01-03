#pragma once
#define _CRT_SECURE_NO_WARNINGS 1
#include<iostream>
#include<fstream>
#include<queue>
using namespace std;

struct HuffManTreeNode {
	HuffManTreeNode(int weight, char ch = 0)
		:pLeft_(nullptr)
		,pRight_(nullptr)
		,Weight_(weight)
		,ch_(ch)
	{}
	HuffManTreeNode *pLeft_;
	HuffManTreeNode *pRight_;
	int Weight_;
	char ch_;
};

class Less {
public:
	bool operator()(const HuffManTreeNode* pLeft, const HuffManTreeNode* pRight) {
		return pLeft->Weight_ > pRight->Weight_;
	}
};

class HuffManTree {
	typedef HuffManTreeNode Node;
public:	
	HuffManTree(const string fileName=string())
		:fileName_(fileName)
		,root_(nullptr)
	{}
	~HuffManTree() {
		//释放二叉树节点
	}
	void InitQue() {
		FILE *pFile = fopen(fileName_.c_str(), "r");
		if (pFile == NULL) {
			perror("fopen error!");
			return;
		}
		char count[256] = { 0 };
		char buf[1024] = { 0 };
		while (1) {
			int n = fread(buf, 1, 1024, pFile);
			for (int i = 0; i < n; ++i) {
				++count[buf[i]];
			}
			if (n < 1024)
				break;
		}
		for (int i = 0; i < 256; ++i) {
			if (count[i] > 0) {
				Node * ptr = new Node(count[i], i);
				que_.push(ptr);
			}
		}
		fclose(pFile);
	}
	void InitTree() {
		while (que_.size() != 1) {
			Node* left = que_.top();
			que_.pop();
			Node* right = que_.top();
			que_.pop();
			Node *newNode = new Node(left->Weight_ + right->Weight_);
			newNode->pLeft_ = left;
			newNode->pRight_ = right;
			que_.push(newNode);
		}
		root_ = que_.top();
	}
	void WriteBind() {
		FILE *pRead = fopen(fileName_.c_str(), "r");
		FILE *pWrite = fopen("test.bin", "wb");
		if (pRead == NULL || pWrite == NULL) {
			perror("fopen error!");
			return;
		}
		char count[256] = { 0 };
		char buf[1024] = { 0 };
		while (1) {
			int n = fread(buf, 1, 1024, pRead);
			for (int i = 0; i < n; ++i) {
				//
			}
			if (n < 1024)
				break;
		}
	
		fclose(pRead);
		fclose(pWrite);
	}
private:
	Node* root_;
	string fileName_;
	priority_queue<Node*,vector<Node*>,Less> que_;
};
