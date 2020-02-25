#define _CRT_SECURE_NO_WARNINGS 1
#include"FileCompressHuffman.h"
#include<iostream>

FileCompressHuffman::FileCompressHuffman() {}

void FileCompressHuffman::CompressFile(const std::string& fileName) {
	FILE *pFile = fopen(fileName.c_str(), "rb");//因为这里压缩的不一定是文本文件
												 //用二进制方式打开更为合理
	if (!pFile) {
		std::cout << "open file " << fileName << " error!" << std::endl;
		return;
	}
	//1、统计源文件中每个字符出现的次数
	unsigned char buf[1024] = { 0 };   //一次读取文件的1024个字节
									   //这里不能出现负数，因为无法作为count数组的下标，所以要用unsigned char 
	int rdSize = 0;
	std::vector<int> count;
	count.resize(256);
	while (1) {
		rdSize = fread(buf, 1, 1024, pFile);
		for (int i = 0; i < rdSize; ++i) {
			++count[buf[i]];
		}
		if (rdSize < 1024)
			break;
	}
	//2、以字符出现的次数为权值创建huffman树
	HuffmanTree tree(count);
	//3、获取每个字符的深度
	std::vector<ParHuffNode*> ChBitLen;
	GetBitLength(tree.GetRoot(), ChBitLen, 0);

	//按照长度为第一优先，字符值为第二优先排序ChBitLen
	MySort(ChBitLen);

	//4.计算每个字符的编码
	std::vector<std::string> strCode;
	strCode.resize(256);
	CalCharCode(ChBitLen, strCode);

	//5、用获取到的编码重新改写源文件
	FILE *pWrite = fopen("Huffman.bin", "wb");
	if (!pWrite) {
		std::cout << "open Huffman.bin error!" << std::endl;
		return;
	}
	//写入头部信息
	WriteHead(pWrite, ChBitLen);
	fseek(pFile, 0, SEEK_SET);

	unsigned char ch = 0;
	int bitCount = 0;
	while (1) {
		rdSize = fread(buf, 1, 1024, pFile);
		for (int i = 0; i < rdSize; ++i) {
			std::string passWord = strCode[buf[i]];//读取buf[i]对应的密码
			for (size_t j = 0; j < passWord.size(); ++j) {
				ch <<= 1;
				++bitCount;
				if (passWord[j] == '1') {
					ch |= 1;
				}
				if (bitCount == 8) {
					fputc(ch, pWrite);
					bitCount = 0;
					ch = 0;
				}
			}
		}
		if (rdSize < 1024)
			break;
	}
	if (bitCount > 0) {
		ch = ch << (8 - bitCount);
		fputc(ch, pWrite);
	}
	fclose(pFile);
	fclose(pWrite);
}
void FileCompressHuffman::CalCharCode(std::vector<ParHuffNode*>& ChBitLen, std::vector<std::string>& strCode) {
	std::string str = "";
	int count = ChBitLen[0]->bitLen_;
	while (count--) {
		str += "0";
	}
	ChBitLen[0]->str_ = str;
	for (int i = 1; i < ChBitLen.size(); ++i) {
		if (ChBitLen[i]->bitLen_ == ChBitLen[i - 1]->bitLen_) {//等长，属于同一组，加一操作
			ChBitLen[i]->str_ = strPlusOne(ChBitLen[i - 1]->str_, 1);
		}
		else {//不等长，加一 ，左移
			std::string t = strPlusOne(ChBitLen[i - 1]->str_, 1);
			int n = ChBitLen[i]->bitLen_ - ChBitLen[i - 1]->bitLen_;
			while (n--) {
				t = t + "0";
			}
			ChBitLen[i]->str_ = t;
		}
	}
	for (int i = 0; i < ChBitLen.size(); ++i) {
		strCode[ChBitLen[i]->Ch_] = ChBitLen[i]->str_;
	}
}
std::string FileCompressHuffman::strPlusOne(const std::string& str, unsigned char count) {
	std::string ret = str;
	while (count--) {
		auto it = ret.rbegin();
		bool flag = true;
		while (it != ret.rend()) {
			if (flag) {
				++(*it);
				flag = false;
			}
			if (*it == '2') {
				*it = 0;
				flag = true;
			}
			else
				break;
			++it;
		}
		if (flag)
			ret = "1" + ret;
	}
	return ret;
}
void FileCompressHuffman::MySort(std::vector<ParHuffNode*>& ChBitLen) {
	for (int i = 1; i < ChBitLen.size(); ++i) {
		int j = i;
		while (j >0 && Compare(ChBitLen[i - 1], ChBitLen[i])) {
			ParHuffNode* tmp = ChBitLen[i - 1];
			ChBitLen[i - 1] = ChBitLen[i];
			ChBitLen[i] = tmp;
			--j;
		}
	}
}
bool FileCompressHuffman::Compare(const ParHuffNode* left, const ParHuffNode* right) {//升序
	if (left->bitLen_ > right->bitLen_)
		return true;
	else if (left->bitLen_ == right->bitLen_&&left->Ch_ > right->Ch_)
		return true;
	else
		return false;
}
void FileCompressHuffman::UnCompressFile(const std::string& fileName) {
	FILE *pRead = fopen(fileName.c_str(), "rb");
	if (!pRead) {
		std::cout << "open file " << fileName << " error!" << std::endl;
		return;
	}
	//读取头部信息
	std::vector<ParHuffNode*> ChBitLen;
	ReadHead(pRead, ChBitLen);

	//计算解码表
	std::vector<DecTable*> decTable;
	CalDecTable(ChBitLen, decTable);

	FILE *pWrite = fopen("h2.bin", "wb");
	if (pWrite == NULL) {
		std::cout << "open h2.bin error!" << std::endl;
		return;
	}

	//开始解压缩
	int strIndex = 0;  //缓冲区剩余多少字符
	unsigned char readBuf[128] = { 0 };
	unsigned char strBuf[1024] = { 0 };
	while (1) {
		int n = fread(readBuf, 1, 128 - (strIndex / 8) - 1, pRead);
		if (!n)
			break; 
		//将读取的二进制转为字符串
		bitToStr(readBuf, n, strBuf + strIndex);
		//根据解码表，解压缩
		while (ProStrBuf(decTable, strBuf, strIndex)) {}
	}
	fclose(pRead);
	fclose(pWrite);
}
void FileCompressHuffman::bitToStr(unsigned char* readBuf, unsigned char* strBuf, const int n) {
	int strIndex = 0;
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < 8; ++j) {
			if (readBuf[i] & 0x80) {//转为字符1
				strBuf[strIndex++] = '1';
			}
			else {//转为字符0
				strBuf[strIndex++] = '0';
			}
			readBuf[i] <<= 1;
		}
	}
}
bool FileCompressHuffman::ProStrBuf(const std::vector<DecTable*>& decTable, const unsigned char* readRuf) {

}
void FileCompressHuffman::CalDecTable(const std::vector<ParHuffNode*>& ChBitLen, std::vector<DecTable*>& decTable) {
	unsigned char bitLen = ChBitLen[0]->bitLen_;
	std::string headStr = "";
	int count = bitLen;
	while (count--) {
		headStr += "0";
	}
	decTable.push_back(new DecTable(bitLen, headStr, 0));
	int tableIndex = 0;
	for (int i = 1; i < ChBitLen.size(); ++i) {
		if (ChBitLen[i]->bitLen_ == ChBitLen[i - 1]->bitLen_) {
			++(decTable[tableIndex]->chCount_);
		}
		else {
			bitLen = ChBitLen[i]->bitLen_;
			std::string headStr = strPlusOne(decTable[tableIndex]->headStr_, decTable[tableIndex]->chCount_);
			int moveLeft = bitLen - decTable[tableIndex]->bitLen_;
			while (moveLeft--) {
				headStr = headStr + "0";
			}
			decTable.push_back(new DecTable(bitLen, headStr, i));
			++tableIndex;
		}
	}
}

void FileCompressHuffman::ReadHead(FILE* pRead, std::vector<ParHuffNode*>& ChBitLen){
	unsigned char count = fgetc(pRead);
	for (int i = 0; i < count; ++i) {
		unsigned char ch = fgetc(pRead);
		unsigned char len = fgetc(pRead);
		ChBitLen.push_back(new ParHuffNode(ch, len));
	}
}
void FileCompressHuffman::WriteHead(FILE* pWrite, const std::vector<ParHuffNode*>& ChBitLen) {
	//写入字符个数
	unsigned char count = ChBitLen.size();
	fputc(count, pWrite);
	//写入每个字符和码字长度
	for (int i = 0; i < ChBitLen.size(); ++i) {
		fputc(ChBitLen[i]->Ch_, pWrite);
		fputc(ChBitLen[i]->bitLen_, pWrite);
	}
}
void FileCompressHuffman::GetBitLength(HuffManTreeNode* ptr, std::vector<ParHuffNode*>& ChBitLen, unsigned char bitLen) {
	if (!ptr->pLeft_ && !ptr->pRight_) {//找到了叶子节点
		ChBitLen.push_back(new ParHuffNode(ptr->Ch_, bitLen));
		return;
	}
	if (ptr->pLeft_)
		GetBitLength(ptr->pLeft_, ChBitLen, bitLen + 1);
	if (ptr->pRight_)
		GetBitLength(ptr->pRight_, ChBitLen, bitLen + 1);
}