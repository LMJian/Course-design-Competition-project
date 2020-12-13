#define _CRT_SECURE_NO_WARNINGS 1
#include"FileCompressHuffman.hpp"
#include<iostream>

FileCompressHuffman::FileCompressHuffman(){}

void FileCompressHuffman::CompressFile(const std::string& fileName) {
	FILE *pFile = fopen(fileName.c_str(), "rb");//��Ϊ����ѹ���Ĳ�һ�����ı��ļ�
												 //�ö����Ʒ�ʽ�򿪸�Ϊ����
	if (!pFile) {
		std::cout << "open file " << fileName << " error!" << std::endl;
		return;
	}
	//1��ͳ��Դ�ļ���ÿ���ַ����ֵĴ���
	unsigned char buf[1024] = { 0 };   //һ�ζ�ȡ�ļ���1024���ֽ�
									   //���ﲻ�ܳ��ָ�������Ϊ�޷���Ϊcount������±꣬����Ҫ��unsigned char 
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
	//2�����ַ����ֵĴ���ΪȨֵ����huffman��
	HuffmanTree tree(count);
	//3����ȡÿ���ַ��ı���
	std::vector<std::string> strCode;
	strCode.resize(256);
	GetHuffmanCode(tree.GetRoot(), strCode);
	
	//4���û�ȡ���ı������¸�дԴ�ļ�
	FILE *pWrite = fopen("Huffman.bin", "wb");
	if (!pWrite) {
		std::cout << "open Huffman.bin error!" << std::endl;
		return;
	}
	//д��ͷ����Ϣ
	WriteHead(pWrite, count);
	fseek(pFile, 0, SEEK_SET);
		
	unsigned char ch = 0;
	int bitCount = 0;
	while (1) {
		rdSize = fread(buf, 1, 1024, pFile);
		for (int i = 0; i < rdSize; ++i) {
			std::string passWord = strCode[buf[i]];//��ȡbuf[i]��Ӧ������
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
	if(bitCount > 0){
		ch = ch << (8 - bitCount);
		fputc(ch, pWrite);
	}
	fclose(pFile);
	fclose(pWrite);
}
void FileCompressHuffman::UnCompressFile(const std::string& fileName) {
	FILE *pRead = fopen(fileName.c_str(), "rb");
	if (!pRead) {
		std::cout << "open file " << fileName << " error!" << std::endl;
		return;
	}
	//��ȡͷ����Ϣ
	std::vector<int> count;
	count.resize(256);
	ReadHead(pRead, count);
	
	//����Haffman��
	HuffmanTree tree(count);
		
	FILE *pWrite = fopen("h2.bin", "wb");
	if (pWrite == NULL) {
		std::cout << "open h2.bin error!" << std::endl;
		return;
	}
	unsigned char ch = 0;
	HuffManTreeNode* ptr = tree.GetRoot();
	unsigned long long fileSize = ptr->Weight_;
	char readBuf[1024] = { 0 };
	while (1) {
		int n = fread(readBuf, 1, 1024, pRead);
		if (!n)
			break;
		for (int i = 0; i < n; ++i) {
			ch = readBuf[i];
			for (int j = 0; j < 8; ++j) {
				if (ch & 0x80) {//���λΪ1
					ptr = ptr->pRight_;
				}
				else {
					ptr = ptr->pLeft_;
				}
				ch = ch << 1;
				if (!ptr->pLeft_ && !ptr->pRight_) {//�Ѿ��ҵ�Ҷ�ӽڵ�
					fputc(ptr->Ch_, pWrite);
					--fileSize;
					ptr = tree.GetRoot();
					if (!fileSize)
						goto LOOP;
				}
			}
		}
	}	
LOOP:
	fclose(pRead);
	fclose(pWrite);
}

void FileCompressHuffman::ReadHead(FILE* pRead, std::vector<int>& count){
	char buf[1024] = { 0 };
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
		count[ch] = ret;
	}
}
void FileCompressHuffman::WriteHead(FILE* pWrite, const std::vector<int>& count) {
	std::string headstr = "";
	int rows = 0;
	char str[1024] = { 0 };
	for (int i = 0; i < 256; ++i) {
		if (count[i] > 0) {
			unsigned char ch = i;
			int ret = count[i];
			//sprintf(str, "%c:%d\n", ch, ret);
			//headstr += str;
			headstr += ch;
			headstr += ':';
			_itoa(ret, str, 10);
			headstr += str;
			headstr += '\n';
			++rows;
		}
	}
	sprintf(str, "%d\n", rows);
	headstr = str + headstr;
	fwrite(headstr.c_str(), 1, headstr.size(), pWrite);
}
void FileCompressHuffman::GetHuffmanCode(HuffManTreeNode* ptr, std::vector<std::string>& charPassWord) {
	static std::string passWord = "";
	if (!ptr->pLeft_ && !ptr->pRight_) {//����ΪҶ�ӽڵ�ʱ
		charPassWord[ptr->Ch_] = passWord;
		return;
	}
	if (ptr->pLeft_) {
		passWord += '0';
		GetHuffmanCode(ptr->pLeft_, charPassWord);
		passWord.erase(passWord.end() - 1);
	}
	if (ptr->pRight_) {
		passWord += '1';
		GetHuffmanCode(ptr->pRight_, charPassWord);
		passWord.erase(passWord.end() - 1);
	}
}