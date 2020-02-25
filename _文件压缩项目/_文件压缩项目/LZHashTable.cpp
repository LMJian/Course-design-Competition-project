#include<string.h>
#include"LZHashTable.hpp"

const USH HASH_BITS = 15;                  //哈希地址15位
const USH HASH_SIZE = (1 << HASH_BITS);    //哈希地址个数 32K
const USH HASH_MASK = HASH_SIZE - 1;       //防止溢出  低15位全1

HashTable::HashTable(USH size)
	:prev_(new USH[2 * size])
	, head_(prev_ + size)
{
	memset(prev_, 0, 2 * size * sizeof(USH));
}
HashTable::~HashTable() {
	delete[] prev_;
	prev_ = nullptr;
}
void HashTable::Insert(USH& matchhead, UCH ch, USH pos, USH& hashAddr) {
	hashFunc(hashAddr, ch);//获取本次插入的哈希地址

	matchhead = head_[hashAddr];//获取上一次匹配的字符串头

	//将新的哈希地址插入链表
	prev_[pos&HASH_MASK] = head_[hashAddr];
	head_[hashAddr] = pos;
}
USH HashTable::GetNext(USH matchHead) {
	return prev_[matchHead&HASH_MASK];
}
void HashTable::Update() {
	for (size_t i = 0; i < WSIZE; ++i) {
		//更新head
		if (head_[i] > WSIZE)
			head_[i] -= WSIZE;
		else
			head_[i] = 0;
		//更新prev
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