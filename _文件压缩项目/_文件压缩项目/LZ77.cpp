#define _CRT_SECURE_NO_WARNINGS 1
#include"LZ77.hpp"
#include<iostream>
#include<assert.h>

const USH MIN_LOOKAHEAD = MAX_MATCH + MIN_MATCH + 1; //Ҫ��֤���һ��ƥ��,���ƥ�䳤��258 
const USH MAX_DIST = WSIZE - MIN_LOOKAHEAD;       // ����******

LZ77::LZ77()
	:pWin_(new UCH[WSIZE*2])
	,ht_(WSIZE)         //��ʼ��hash���С
{}
LZ77::~LZ77() {
	delete[] pWin_;
	pWin_ = nullptr;
}
void LZ77 :: CompressFile(const std::string& strFilePath) {
	FILE* fIn = fopen(strFilePath.c_str(), "rb");
	if (nullptr == fIn) {
		std::cout << "���ļ�ʧ��" << std::endl;
		return;
	}
	//��ȡ�ļ���С
	fseek(fIn, 0, SEEK_END);
	ULL fileSize = ftell(fIn);

	//1. ���Դ�ļ��Ĵ�СС����Сƥ�䳤�� MIN_MATCH���򲻽��д���
	if (fileSize <= MIN_MATCH) {          //�˴���С��3���ַ�
		std::cout << "�ļ�̫С����ѹ��" << std::endl;
		return;
	}
	//��ѹ���ļ��ж�ȡһ�������������ݵ�������
	fseek(fIn, 0, SEEK_SET);
	size_t lookAhead = fread(pWin_, 1, 2 * WSIZE, fIn);
	USH hashAddr = 0;
	
	//������ʼhashAddr (ǰ�����ַ���hash��ַ)
	for (USH i = 0; i < MIN_MATCH - 1; ++i) {
		ht_.HashFunc(hashAddr,pWin_[i]);
	}

	//ѹ��:
	FILE* fOUT = fopen("2.txt", "wb");
	assert(fOUT);
	USH start = 0;           //�����ַ����ڻ������ĵ�ַ
	
	//������ƥ����صı���
	USH matchHead = 0;       //ƥ���ַ�����ͷ
	USH curMatchLength = 0;  //�ƥ���ַ����ĳ���
	USH curMatchDist = 0;    //�ƥ���ַ����ľ���

	//��д�����صı���
	UCH chFlag = 0;
	UCH bitCount = 0;
	bool IsLen = false;
	
	//д��ǵ��ļ�
	FILE* fOutF = fopen("3.txt", "wb");
	assert(fOutF);

	//lookAhead��ʾ���л�������ʣ���ֽڵĸ���
	while (lookAhead) {
		//1.����ǰ�����ַ����뵽��ϣ���У�����ȡƥ������ͷ
		ht_.Insert(matchHead, pWin_[start + 2], start, hashAddr);
		
		curMatchLength = 0;
		curMatchDist = 0;
		//2.��֤�ڲ��һ��������Ƿ��ҵ�ƥ�䣬�����ƥ�䣬���ƥ��
		if (matchHead) {
			//˳��ƥ�������ƥ��,���մ���<���ȣ�����>��
			curMatchLength = LongestMatch(matchHead, curMatchDist, start);
		}

		//3.��֤�Ƿ��ҵ�ƥ��
		if (curMatchLength < MIN_MATCH) {
			//�ڲ��һ�������δ�ҵ��ظ��ַ���
			//��startλ�õ��ַ�д�뵽ѹ���ļ���
			fputc(pWin_[start], fOUT); //д�ַ�
			
			//д��ǰԭ�ַ���Ӧ�ı��
			WriteFlag(fOutF, chFlag, bitCount, false);//д���
			++ start;
			lookAhead--;
		}
		else {
			//�ҵ�ƥ��
			//�������ȣ����롷��д��ѹ���ļ���
			//д����
			UCH chLen = curMatchLength - 3;
			fputc(chLen, fOUT);

			//д����
			fwrite(&curMatchDist, sizeof(curMatchDist), 1, fOUT);

			//д��ǰԭ�ַ���Ӧ�ı��
			WriteFlag(fOutF, chFlag, bitCount, true);
			
			//�������л�������ʣ����ֽ���
			lookAhead -= curMatchLength;

			//���Ѿ�ƥ����ַ�����������һ�齫����뵽��ϣ����
			--curMatchLength;  //��ǰ�ַ����Ѿ�����
			++start;
			while (curMatchLength) {
				ht_.Insert(matchHead, pWin_[start + 2], start, hashAddr);
				--curMatchLength;
				++start;
			}
		}
		//������л�������ʣ���ַ�����
		if (lookAhead <= MIN_LOOKAHEAD)
			FillWindow(fIn, lookAhead, start);
	}
	
	//���λ�����������λ
	if (bitCount > 0 && bitCount < 8) {
		chFlag <<= (8 - bitCount);
		fputc(chFlag, fOutF);
	}
	fclose(fOutF);

	MergeFile(fOUT, fileSize);
	fclose(fIn);
	fclose(fOUT);

	//��������������Ϣ����ʱ�ļ�ɾ����
}

void LZ77::FillWindow(FILE* fIn, size_t& lookAhead, USH& start) {
	//startѹ���Ѿ����е��Ҵ������л�����ʣ�����ݲ���MIN_LOOKAHEAD
	if (start >= WSIZE) {
		//1.���Ҵ��е����ݰ��Ƶ���
		memcpy(pWin_, pWin_ + WSIZE, WSIZE);
		memset(pWin_ + WSIZE, 0, WSIZE);
		start -= WSIZE;

		//2.���¹�ϣ��
		ht_.Update();

		//3.���Ҵ��в���WSIZE���Ĵ�ѹ������
		if (!feof(fIn))
			lookAhead += fread(pWin_ + WSIZE, 1, WSIZE, fIn);
	}
}
void LZ77::MergeFile(FILE* fOut, ULL fileSize) {
	//��ѹ�������ļ��ͱ����Ϣ�ļ��ϲ�
	//1.��ȡ�����Ϣ�ļ������ݣ�Ȼ�󽫽��д�뵽ѹ���ļ���
	FILE* fInf = fopen("3.txt", "rb");
	size_t flagSize = 0;
	UCH* pReadbuff = new UCH[1024];
	while (true) {
		size_t rdSize = fread(pReadbuff, 1, 1024, fInf);
		if (rdSize == 0)
			break;

		fwrite(pReadbuff, 1, rdSize, fOut);
		flagSize += rdSize;
	}
	fwrite(&flagSize, sizeof(flagSize), 1, fOut);
	fwrite(&fileSize, sizeof(fileSize), 1, fOut);
	delete[] pReadbuff;
	fclose(fInf);
}

//chFlag:���ֽ��е�ÿ������λ���������ֵ�ǰ�ֽ���ԭ�ַ����ǳ���?
//0:ԭ�ַ�
//1:����

//bitCount:���ֽ��еĶ��ٸ�����λ�Ѿ�������
//isCharOrLen:������ֽ���Դ�ַ����ǳ���
void LZ77::WriteFlag(FILE* fOUT, UCH& chFlag, UCH& bitCount, bool isLen){
	chFlag <<= 1;
	if (isLen)
		chFlag |= 1;

	bitCount++;
	if (bitCount == 8) {
		//���ñ��д�뵽ѹ���ļ���
		fputc(chFlag, fOUT);
		chFlag = 0;
		bitCount = 0;
	}
}
//ƥ�䣺���ڲ��һ������н��еģ����һ������п��ܻ��ҵ����ƥ��
//�������Ҫ�ƥ��
//ע�⣺���ܻ�������״��    ������������ƥ�����
//       ƥ������MAX_DIST��Χ�ڽ���ƥ��ģ�̫Զ�ľ����򲻽���ƥ��

//���ҵĹ����У���Ҫ��ÿ���ҵ���ƥ�������бȽϣ������ƥ��
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

void LZ77::UNCompressFile(const std::string& strFilePath) {
	//��ѹ���ļ��ͱ���ļ�
	FILE* fInD = fopen(strFilePath.c_str(), "rb");
	if (nullptr == fInD) {
		std::cout << "ѹ���ļ���ʧ��" << std::endl;
		return;
	}

	//����������ݵ��ļ�ָ��
	FILE* fInF = fopen(strFilePath.c_str(), "rb");
	if (nullptr == fInF) {
		std::cout << "ѹ���ļ���ʧ��" << std::endl;
		return;
	}
	//��ȡԴ�ļ���С
	ULL fileSize = 0;
	fseek(fInF, 0 - sizeof(fileSize), SEEK_END);
	fread(&fileSize, sizeof(fileSize), 1, fInF);

	//��ȡ�����Ϣ�Ĵ�С
	size_t flagSize = 0;
	fseek(fInF, 0 - sizeof(fileSize) - sizeof(flagSize), SEEK_END);
	fread(&flagSize, sizeof(flagSize), 1, fInF);

	//����ȡ�����Ϣ���ļ�ָ���ƶ������������ݵ���ʼλ��
	fseek(fInF, 0 - sizeof(flagSize) - sizeof(fileSize) - flagSize, SEEK_END);


	//��ʼ��ѹ��

	//д��ѹ������
	FILE* fOut = fopen("4.txt", "wb");
	assert(fOut);

	FILE* fR = fopen("4.txt", "rb");
	UCH bitCount = 0;
	UCH chFlag = 0;
	ULL encodeCount = 0;
	while (encodeCount < fileSize) {
		//��ȡ���
		if (0 == bitCount) {
			chFlag=fgetc(fInF);
			bitCount = 8;
		}
		if (chFlag & 0x80) {
			//���볤�ȶ�
			USH matchLen = fgetc(fInD) + 3;
			USH matchDist = 0;
			fread(&matchDist, sizeof(matchDist), 1, fInD);

			//��ջ�����
			fflush(fOut);
			
			//�����Ѿ�������ֽ�����С
			encodeCount += matchLen;

			//fR����ȡǰ��ƥ�䴮�е�����
			UCH ch;
			fseek(fR, 0 - matchDist, SEEK_END);
			while (matchLen) {
				ch = fgetc(fR);
				fputc(ch, fOut);
				--matchLen;

				//�ڻ�ԭ���Ⱦ����ʱ��һ��Ҫ��ջ�������������ܻỹԭ����

				fflush(fOut);
			}
		}
		else {
			//ԭ�ַ�
			UCH ch = fgetc(fInD);
			fputc(ch, fOut);
			encodeCount += 1;
		}
		chFlag <<= 1;
		bitCount--;
	}
	fclose(fInD);
	fclose(fInF);
	fclose(fOut);
	fclose(fR);
}