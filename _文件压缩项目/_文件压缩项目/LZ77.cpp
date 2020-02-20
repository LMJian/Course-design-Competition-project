#define _CRT_SECURE_NO_WARNINGS 1
#include"LZ77.hpp"
#include<iostream>
#include<assert.h>

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

	//1. ���Դ�ļ��Ĵ�СС��MIN_MATCH���򲻽��д���
	if (fileSize <= MIN_MATCH) {          //�˴���С��3���ַ�
		std::cout << "�ļ�̫С����ѹ��" << std::endl;
		return;
	}
	//��ѹ���ļ��ж�ȡһ�������������ݵ�������
	fseek(fIn, 0, SEEK_SET);
	size_t lookAhead = fread(pWin_, 1, 2 * WSIZE, fIn);
	USH hashAddr = 0;
	
	//abcdefg...������ʼhashAddr (ǰ�����ַ���hash��ַ)
	for (USH i = 0; i < MIN_MATCH - 1; ++i) {
		ht_.HashFunc(hashAddr,pWin_[i]);
	}

	//ѹ��:
	FILE* fOUT = fopen("2.txt", "wb");
	assert(fOUT);
	USH start = 0;           //�����ַ�����ַ
	USH matchHead = 0;       //��һ���ҵ�ƥ���ַ�������ʼλ��
	UCH curMatchLength = 0;  //�ƥ���ַ����ĳ���
	USH curMatchDist = 0;    //�ƥ���ַ����ľ���

	//lookAhead��ʾ���л�������ʣ���ֽڵĸ���
	while (lookAhead) {
		//1.����ǰ�����ַ����뵽��ϣ���У�����ȡƥ������ͷ
		ht_.Insert(matchHead, pWin_[start + 2], start, hashAddr);
		//2.��֤�ڲ��һ��������Ƿ��ҵ�ƥ�䣬�����ƥ�䣬���ƥ��
		if (matchHead) {
			//˳��ƥ�������ƥ��,���մ���<���ȣ�����>��
			curMatchLength = LongestMatch(matchHead, curMatchDist, start);
		}

		//3.��֤�Ƿ��ҵ�ƥ��
		if (curMatchLength < MIN_MATCH) {
			//�ڲ��һ�������δ�ҵ��ظ��ַ���
			//��startλ�õ��ַ�д�뵽ѹ���ļ���
			fputc(pWin_[start], fOUT);   //**//
			++ start;
			lookAhead--;
		}
		else {
			//�ҵ�ƥ��
			//�������ȣ����롷��д��ѹ���ļ���
			//д����
			fputc(curMatchLength, fOUT);

			//д����
			fwrite(&curMatchDist, sizeof(curMatchDist), 1, fOUT);

			//�������л�������ʣ����ֽ���
			lookAhead -= curMatchLength;

			//���Ѿ�ƥ����ַ�����������һ�齫����뵽��ϣ����
			--curMatchLength;  //��ǰ�ַ����Ѿ�����
			while (curMatchLength) {
				++ start;
				ht_.Insert(matchHead, pWin_[start], start, hashAddr);
				--curMatchLength;
			}
		}
	}
}
UCH LZ77::LongestMatch(USH hashHead, USH& MatchStart, USH start) {     //���ƥ��
	// ��ϣ�������������ȣ���ֹ�����ѭ�� 
	int chain_length = 256; 
	// ʼ�ձ��ֻ�������ΪWSIZE����Ϊ��С�ĳ�ǰ�鿴��������MIN_LOOKAHEAD������ 
	// ���ֻ����_start���MAX_DIST��Χ�ڵĴ� 
	USH limit = start > MAX_DIST ? start - MAX_DIST : 0; 
	// ��ƥ���ַ��������λ��

	// [pScan, strend] 
	UCH* pScan = pWin_ + start; 
	UCH* strend = pScan + MAX_MATCH - 1; 
	
	// �������е����ƥ�� 
	int bestLen = 0; 
	UCH* pCurMatchStart; 
	USH curMatchLen = 0; 
	
	// ��ʼƥ�� 
	do{ 
		// ��������hashHead��λ�ÿ�ʼƥ�� 
		pCurMatchStart = pWin_ + hashHead; 
		while (pScan < strend && *pScan == *pCurMatchStart) { 
			pScan++; 
			pCurMatchStart++; 
		}
		
		// ����ƥ��ĳ��Ⱥ�ƥ�����ʼλ�� 
		curMatchLen = (MAX_MATCH - 1) - (int)(strend - pScan); 
		pScan = strend - (MAX_MATCH - 1); 
		
		/*�������ƥ��ļ�¼*/ 
		if (curMatchLen > bestLen) { 
			MatchStart = hashHead; 
			bestLen = curMatchLen; 
		} 
	} while ((hashHead = hash_.prev_[hashHead & WMASK]) > limit && --chain_length != 0); 
	return curMatchLen;
}

void UNCompressFile(const std::string& strFilePath);
