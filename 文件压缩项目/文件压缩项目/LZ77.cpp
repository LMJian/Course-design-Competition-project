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
		std::cout << "�ļ�̫С������" << std::endl;
		return;
	}
	//���ļ�ָ���û���ʼλ��
	fseek(fR, 0, SEEK_SET);
	//��ȡ�ļ���������
	ULL readSize = fread(pWin_, sizeof(UCH), 2 * WSIZE, fR);
	
	USH hashAddr = 0;//����ǰ�����ַ��Ĺ�ϣ��ַ
	for (UCH i = 0; i < MIN_MATCH - 1; ++i) {
		ht_.hashFunc(hashAddr, pWin_[i]);
	}
	
	USH matchHead = 0;//ƥ������ͷ
	USH curMatchLen = 0; //�ƥ�����ĳ���
	USH curMatchDist = 0; //�ƥ�����ľ���
	USH start = 0;

	FILE* fW = fopen("2.txt", "wb");//дѹ������
	FILE* fWT = fopen("3.txt", "wb");//д���ݵı��
	if (!fW||!fWT) {
		std::cout << "2.txt/3.txt �ļ���ʧ��" << std::endl;
		return;
	}

	UCH chNum = 0;   //��Ҫд��ı��
	UCH bitCount = 0; //��¼ ���д�˶���λ
	//while(1){
		ULL readCount = readSize;
		while (readCount) {
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
				--readCount;
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
				readCount -= curMatchLen;
				++start;
				--curMatchLen;//��һ���ַ��Ѿ�����
				while (curMatchLen) {
					ht_.Insert(matchHead, pWin_[start + 2], start, hashAddr);
					++start;
					--curMatchLen;
				}
			}
		}
		if (bitCount > 0 && bitCount < 8) {
			chNum <<= (8 - bitCount);
			fputc(chNum, fWT);
		}
		//if (readSize < 2 * WSIZE)
		//	break;
		//readSize = fread(pWin_, sizeof(UCH), 2 * WSIZE, fR);
	//}
		fclose(fR);
		fclose(fW);
		fclose(fWT);
}
USH LZ77::LongestMatch(USH matchHead, USH& MatchDist, USH start) {     //���ƥ��
	USH curMatchLen = 0;  //һ��ƥ��ĳ���
	USH maxMatchLen = 0;
	UCH maxMatchCount = 255;   //����ƥ������������״��
	USH curMatchStart = 0;     //��ǰƥ���ڲ��һ������е���ʼλ��

	//�����л������в���ƥ��ʱ������̫Զ�����ܳ���MAX_DIST
	USH limit = start > MAX_DIST ? start - MAX_DIST : 0;

	do {
		//ƥ�䷶Χ
		//���л�����
		UCH* pstart = pWin_ + start;
		UCH* pend = pstart + MAX_MATCH;

		//���һ�����ƥ�䴮����ʼ
		UCH* pMatchStart = pWin_ + matchHead;
		curMatchLen = 0;

		//���Խ��б���ƥ��
		while (pstart < pend&&*pstart == *pMatchStart) {
			++curMatchLen;
			++pstart;
			++pMatchStart;
		}
		//һ��ƥ�����
		if (curMatchLen > maxMatchLen) {
			maxMatchLen = curMatchLen;
			curMatchStart = matchHead;
		}
	} while ((matchHead = ht_.GetNext(matchHead)) > limit&&maxMatchCount--);

	MatchDist = start - curMatchStart;
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