#define _CRT_SECURE_NO_WARNINGS 1
#include"LZ77.hpp"
#include<iostream>

const USH MIN_LOOKAHEAD = MAX_MATCH + MIN_MATCH + 1; //Ҫ��֤���һ��ƥ��,���ƥ�䳤��258 
const USH MAX_DIST = WSIZE - MIN_LOOKAHEAD;    

LZ77::LZ77()
	:pWin_(new UCH[WSIZE * 2])
	,ht_(WSIZE)
{}
LZ77::~LZ77() {
	delete[] pWin_;
	pWin_ = nullptr;
}
void LZ77::CompressionFile(const std::string& fileName) {
	FILE* fR = fopen(fileName.c_str(), "rb");
	if (!fR) {
		std::cout << "��ѹ���ļ���ʧ��!" << std::endl;
		return;
	}
	//�����ļ���С
	fseek(fR, 0, SEEK_END);
	ULL fileSize = ftell(fR);
	if (fileSize <= MIN_MATCH) {
		std::cout << "�ļ�̫С������������ѹ������" << std::endl;
		return;
	}
	//���ļ�ָ���û���ʼλ��
	fseek(fR, 0, SEEK_SET);
	//��ʼѹ��
	//��ȡ�ļ���������
	size_t readSize = fread(pWin_, sizeof(UCH), 2 * WSIZE, fR);

	//����ǰ�����ַ��Ĺ�ϣ��ַ
	USH hashAddr = 0;
	for (UCH i = 0; i < MIN_MATCH - 1; ++i) {
		ht_.hashFunc(hashAddr, pWin_[i]);
	}

	USH matchHead = 0;//ƥ������ͷ
	USH curMatchLen = 0; //�ƥ�����ĳ���
	USH curMatchDist = 0; //�ƥ�����ľ���
	USH start = 0;

	FILE* fW = fopen("2.txt", "wb");//дѹ������
	FILE* fWT = fopen("3.txt", "wb");//д���ݵı��
	if (!fW || !fWT) {
		std::cout << "2.txt/3.txt �ļ���ʧ��" << std::endl;
		return;
	}

	UCH chNum = 0;   //��Ҫд��ı��
	UCH bitCount = 0; //��¼ ���д�˶���λ
	while (readSize) {
		//�����ַ��������ϣ��
		ht_.Insert(matchHead, pWin_[start + 2], start, hashAddr);
		curMatchLen = 0;
		curMatchDist = 0;

		if (matchHead > 0) { //�ҵ���ƥ����
			//���ƥ����
			curMatchLen = LongestMatch(matchHead, curMatchDist, start);
		}
		if (curMatchLen < MIN_MATCH) {//δ�ҵ�ƥ����
			//дԭ�ַ�
			fputc(pWin_[start], fW);
			//д���
			WriteFlag(fWT, chNum, bitCount, false);
			++start;
			--readSize;
		}
		else {
			//д����
			UCH chlen = curMatchLen - 3;
			fputc(chlen, fW);
			//д����
			fwrite(&curMatchDist, sizeof(curMatchDist), 1, fW);
			//д���
			WriteFlag(fWT, chNum, bitCount, true);
			//��ƥ����ַ�������һ������ϣ��
			readSize -= curMatchLen;
			++start;
			--curMatchLen;//��һ���ַ��Ѿ�����
			while (curMatchLen) {
				ht_.Insert(matchHead, pWin_[start + 2], start, hashAddr);
				++start;
				--curMatchLen;
			}
		}
		if (readSize <= MIN_LOOKAHEAD)
			fillWindow(start, fR, readSize);
	}
	if (bitCount > 0 && bitCount < 8) {
		chNum <<= (8 - bitCount);
		fputc(chNum, fWT);
	}
	fclose(fWT);
	fclose(fR);
	//�ѱ���ļ�д�������ļ��ĺ���
	MergeFile(fW, fileSize);
	fclose(fW);
}
void LZ77::MergeFile(FILE* fW, ULL fileSize) {
	FILE* fR = fopen("3.txt", "rb");
	UCH *buff = new UCH[1024];
	size_t rSize = 0;
	while (1) {
		size_t readSize = fread(buff, sizeof(UCH), 1024, fR);
		if (readSize == 0)
			break;
		rSize += readSize;
		fwrite(buff, sizeof(UCH), readSize, fW);
	}
	fwrite(&rSize, sizeof(rSize), 1, fW);
	fwrite(&fileSize, sizeof(fileSize), 1, fW);
	delete[] buff;
	fclose(fR);
}
void LZ77::fillWindow(USH& start, FILE* fR, size_t& readSize) {
	if (start >= WSIZE) {
		memcpy(pWin_, pWin_ + WSIZE, WSIZE);
		memset(pWin_ + WSIZE, 0, WSIZE);
		
		start -= WSIZE;
		ht_.Update();

		if (!feof(fR))
			readSize += fread(pWin_ + WSIZE, sizeof(UCH), WSIZE, fR);
	}
}
USH LZ77::LongestMatch(USH matchHead, USH& MatchDist, USH start) {     //���ƥ��
	USH curMatchLen = 0;
	USH maxMatchLen = 0;
	USH maxMatchHead = 0;
	UCH matchCount = 255;

	//�����л������в���ƥ��ʱ������̫Զ�����ܳ���MAX_DIST
	USH limit = start > MAX_DIST ? start - MAX_DIST : 0;
	do {
		UCH* pStart = pWin_ + start;
		UCH* pEnd = pStart + MAX_MATCH;

		UCH* ptr = pWin_ + matchHead;
		curMatchLen = 0;

		while (pStart < pEnd&&*pStart == *ptr) {
			++curMatchLen;
			++pStart;
			++ptr;
		}
		if (maxMatchLen < curMatchLen) {
			maxMatchLen = curMatchLen;
			maxMatchHead = matchHead;
		}
	} while ((matchHead = ht_.GetNext(matchHead)) > limit&&matchCount--);
	MatchDist = start - maxMatchHead;
	return maxMatchLen;
}
void LZ77::WriteFlag(FILE* file, UCH& chNum, UCH& bitCount, bool isLen) {
	chNum <<= 1;
	if (isLen) 
		chNum |= 1;
	++bitCount;
	if (bitCount == 8) {
		fputc(chNum, file);
		bitCount = 0;
		chNum = 0;
	}
}
void LZ77::UnCompressionFile(const std::string& fileName) {

}