#define _CRT_SECURE_NO_WARNINGS 1 
#include "FileCompressHuffman.h"

FileCompressHuffman::FileCompressHuffman()
{
	_fileInfo.resize(256);
	for (int i = 0; i < 256; i++)
	{
		_fileInfo[i]._ch = i;
		_fileInfo[i]._count = 0;
	}
}

void FileCompressHuffman::CompressFile(const std::string& path)
{
	_fileName = path;
	FILE* fIn = fopen(path.c_str(), "rb");
	if (nullptr == fIn)
	{
		assert(false);
		return;
	}
	//1��ͳ��Դ�ļ���ÿ���ַ����ֵĴ���
	unsigned char* pReadBuff = new unsigned char[1024];
	size_t readSize = 0;
	while (true)
	{
		readSize = fread(pReadBuff, 1, 1024, fIn);
		if (0 == readSize)
		{
			break;
		}
		for (size_t i = 0; i < readSize; i++)
		{
			_fileInfo[pReadBuff[i]]._count++;
		}
	}

	//2�����ַ����ֵĴ���ΪȨֵ����huffman��
	HuffmanTree<CharInfo> tree(_fileInfo, CharInfo());  //����0�ε���Ч���ַ����������huffman���Ĺ���

	//3����ȡÿ���ַ��ı���
	GenerateHuffmanCode(tree.GetRoot());

	//4���û�ȡ���ı������¸�дԴ�ļ�
	FILE* fOut = fopen("2.txt", "wb");
	if (nullptr == fOut)
	{
		assert(false);
		return;
	}
	WriteHead(fOut, path);           //1111111111
	
	fseek(fIn, 0, SEEK_SET);
	int ch = 0;
	int bitCount = 0;
	while (true)
	{
		readSize = fread(pReadBuff, 1, 1024, fIn);
		if (0 == readSize)
		{
			break;
		}
		//�����ֽڵı���Զ�ȡ�������ݽ�����д
		for (size_t i = 0; i < readSize; i++)
		{
			std::string strCode = _fileInfo[pReadBuff[i]]._strCode;
			for (size_t j = 0; j < strCode.size(); j++)
			{
				ch <<= 1;
				if ('1' == strCode[j])
				{
					ch |= 1;
				}
				bitCount++;
				if (8 == bitCount)
				{
					fputc(ch, fOut);  //���ļ���һ��д��һ���ֽ�
					bitCount = 0;
					ch = 0;
				}
			}
		}
	}
	//���һ��ch�п��ܲ���8��bitλ
	if (bitCount < 8)
	{
		ch <<= (8 - bitCount);
		fputc(ch, fOut);
	}

	delete[] pReadBuff;
	fclose(fIn);
	fclose(fOut);
}

void FileCompressHuffman::UnCompressFile(const std::string& path) {
	FILE* fIn = fopen(path.c_str(), "rb");
	if (nullptr == fIn) {
		assert(false);
		return;
	}
	std::string strFilePostFix;
	ReadLine(fIn, strFilePostFix);

	std::string strCount;
	ReadLine(fIn, strCount);
	int lineCount = atoi(strCount.c_str());
	for (int i = 0; i < lineCount; ++i) {
		std::string strchCount;
		ReadLine(fIn, strchCount);
		if (strchCount.empty()) {
			strchCount += '\n';
			ReadLine(fIn,strchCount);
		}
		_fileInfo[(unsigned char)strchCount[0]]._count = atoi(strchCount.c_str() + 2);
	}
	HuffmanTree<CharInfo> t;
	t.CreateHuffmanTree(_fileInfo, CharInfo(0));
	
	std::string newFileName = "new" + _fileName;
	FILE* fOut = fopen(newFileName.c_str(),"wb");
	char *pReadBuff = new char[1024];
	char ch = 0;
	HuffmanTreeNode<CharInfo>* pCur = t.GetRoot();
	size_t fileSize = pCur->_Weight._count;
	size_t unCount = 0;
	while (1) {
		size_t rdsize = fread(pReadBuff, 1, 1024, fIn);
		if (rdsize == 0) {
			break;
		}
		for (size_t i = 0; i < rdsize; ++i) {
			ch = pReadBuff[i];
			for (int pos = 0; pos < 8; ++pos) {
				if (ch & 0x80) {
					pCur = pCur->_pRight;
				}
				else {
					pCur = pCur->_pLeft;
				}
				ch <<= 1;
				if (nullptr == pCur->_pLeft&&nullptr == pCur->_pRight) {
					++unCount;
					fputc(pCur->_Weight._ch,fOut);
					pCur = t.GetRoot();
					if (unCount == fileSize)
						break;

				}
			}
		}
		
	}
	fclose(fIn);
	fclose(fOut);
	delete[] pReadBuff;
}
void FileCompressHuffman::ReadLine(FILE* fIn, std::string& strInfo) {
	assert(fIn);
	while (!feof(fIn))
	{
		char ch = fgetc(fIn);
		if (ch == '\n')
			break;
		strInfo += ch;
	}
}
void FileCompressHuffman::WriteHead(FILE* fOut, const std::string& fileName) {
	assert(fOut);
	std::string strHead;
	strHead += GetFilePostFix(fileName);
	strHead += '\n';

	size_t lineCount = 0;
	std::string strChCount;
	char szValue[32] = { 0 };
	for (int i = 0; i < 256; ++i) {
		CharInfo& charInfo = _fileInfo[i];
		if (charInfo._count) {
			lineCount++;
			strChCount += _fileInfo[i]._ch;
			strChCount += ':';
			_itoa(charInfo._count, szValue, 10);
			strChCount += szValue;
			strChCount += '\n';
		}
	}
	_itoa(lineCount, szValue, 10);
	strHead += szValue;
	strHead += '\n';
	strHead += strChCount;

	fwrite(strHead.c_str(), 1, strHead.size(), fOut);
}

std::string  FileCompressHuffman::GetFilePostFix(const std::string& fileName) {
	return fileName.substr(fileName.rfind('.'));
}

void FileCompressHuffman::GenerateHuffmanCode(HuffmanTreeNode<CharInfo>* pRoot)  //����huffman����
{
	if (nullptr == pRoot)
	{
		return;
	}
	GenerateHuffmanCode(pRoot->_pLeft);
	GenerateHuffmanCode(pRoot->_pRight);

	if (nullptr == pRoot->_pLeft && nullptr == pRoot->_pRight)
	{//Ҷ�ӽڵ㣬Ҫ������ַ�
		std::string strCode;
		HuffmanTreeNode<CharInfo>* pCur = pRoot;
		HuffmanTreeNode<CharInfo>* pParent = pCur->_pParent;

		while (pParent)
		{
			if (pCur == pParent->_pLeft)
			{
				strCode += '0';
			}
			else
			{
				strCode += '1';
			}
			pCur = pParent;
			pParent = pCur->_pParent;
		}
		reverse(strCode.begin(), strCode.end());

		_fileInfo[pRoot->_Weight._ch]._strCode = strCode;
	}
}