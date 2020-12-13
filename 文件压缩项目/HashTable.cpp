#include<string.h>
#include"HashTable.hpp"

const USH HASH_BITS = 15;                  //��ϣ��ַ15λ
const USH HASH_SIZE = (1 << HASH_BITS);    //��ϣ��ַ���� 32K
const USH HASH_MASK = HASH_SIZE - 1;       //��ֹ���  ��15λȫ1

HashTable::HashTable(USH size)
	:prev_(new USH[2*size])
	,head_(prev_+size)
{
	memset(prev_, 0, 2 * size * sizeof(USH));
}
HashTable::~HashTable() {
	delete[] prev_;
	prev_ = nullptr;
}
void HashTable::Insert(USH& matchhead, UCH ch, USH pos, USH& hashAddr) {
	hashFunc(hashAddr, ch);//��ȡ���β���Ĺ�ϣ��ַ

	matchhead = head_[hashAddr];//��ȡ��һ��ƥ����ַ���ͷ
	
	//���µĹ�ϣ��ַ��������
	prev_[pos&HASH_MASK] = head_[hashAddr];
	head_[hashAddr] = pos;
}
USH HashTable::GetNext(USH matchHead) {
	return prev_[matchHead&HASH_MASK];
}
void HashTable::Update() {
	for (size_t i = 0; i < WSIZE; ++i) {
		//����head
		if (head_[i] > WSIZE)
			head_[i] -= WSIZE;
		else
			head_[i] = 0;
		//����prev
		if (prev_[i] > WSIZE)
			prev_[i] -= WSIZE;
		else
			prev_[i] = 0;
	}
}
void HashTable::hashFunc(USH& hashAddr, UCH ch) {
	hashAddr = (((hashAddr) << H_SHIFT()) ^ (ch))&HASH_MASK;
}
USH HashTable::H_SHIFT() {
	return (HASH_BITS + MIN_MATCH - 1) / MIN_MATCH;
}