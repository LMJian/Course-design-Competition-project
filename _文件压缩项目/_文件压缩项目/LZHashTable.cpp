#include"LZHashTable.hpp"
#include<string.h>

const USH HASH_BITS = 15;
const USH HASH_SIZE = (1 << HASH_BITS);    //��ϣ��ַ����32K��
const USH HASH_MASK = HASH_SIZE - 1;       //��15λȫ1

LZHashTable::LZHashTable(USH size)
	:prev_(new USH[size * 2])     //��ϣ���д�ŵ��������ַ������׵�ַ(�±�)
	, head_(prev_ + size)
{
	memset(prev_, 0, size * 2 * sizeof(USH));
}
LZHashTable::~LZHashTable() {
	delete[] prev_;
	prev_ = nullptr;
}
                         //matchHead��ƥ������ͷ  
						 //ch���������ַ��������һ���ַ�����3���� 
						 //pos�������ַ������ַ��ڻ��������±� 
						 //hashAddr����һ�β���Ĺ�ϣ��ַ       
void LZHashTable::Insert(USH& matchHead, UCH ch, USH pos, USH& hashAddr) {
	HashFunc(hashAddr, ch);   //��ȡ���β���Ĺ�ϣ��ַ
	
	matchHead = head_[hashAddr];//�ҵ�ǰ�����ַ��ڲ��һ��������ҵ��������һ������ƥ������ͷ

	prev_[pos&HASH_MASK] = head_[hashAddr];//pos���ܻᳬ��32K��&MASK��Ŀ�ľ���Ϊ�˲�Խ��
	head_[hashAddr] = pos;
}

//abcdefgh�ַ���
//hashaddr1:abc
//hashaddr2:bcd

//hashAddr:ǰһ�μ�����Ĺ�ϣ��ַ  abc
//������Ҫ����bcd��ϣ��ַ
//ch:����ƥ�������ַ��е����һ��
//���ι�ϣ��ַ������һ�ι�ϣ��ַ�Ļ����ϼ��������
void LZHashTable::HashFunc(USH& hashAddr, UCH ch) {      //hashAddr�����룬����Ͳ�����ch���������ַ����е�һ���ַ�
	hashAddr = (((hashAddr) << H_SHIFT()) ^ (ch))&HASH_MASK;
}
USH LZHashTable::H_SHIFT() {
	return (HASH_BITS + MIN_MATCH - 1) / MIN_MATCH;     //5
}
