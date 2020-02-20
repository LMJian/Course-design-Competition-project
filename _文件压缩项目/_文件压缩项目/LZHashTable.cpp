#include"LZHashTable.hpp"
#include<string.h>

const USH HASH_BITS = 15;
const USH HASH_SIZE = (1 << HASH_BITS);    //哈希地址个数32K个
const USH HASH_MASK = HASH_SIZE - 1;       //低15位全1

LZHashTable::LZHashTable(USH size)
	:prev_(new USH[size * 2])     //哈希表中存放的是索引字符串的首地址(下标)
	, head_(prev_ + size)
{
	memset(prev_, 0, size * 2 * sizeof(USH));
}
LZHashTable::~LZHashTable() {
	delete[] prev_;
	prev_ = nullptr;
}
                         //matchHead：匹配链的头  
						 //ch：所查找字符串的最后一个字符（第3个） 
						 //pos：查找字符串首字符在缓冲区的下标 
						 //hashAddr：上一次插入的哈希地址       
void LZHashTable::Insert(USH& matchHead, UCH ch, USH pos, USH& hashAddr) {
	HashFunc(hashAddr, ch);   //获取本次插入的哈希地址
	
	matchHead = head_[hashAddr];//找当前三个字符在查找缓冲区中找到的最近的一个，即匹配链的头

	prev_[pos&HASH_MASK] = head_[hashAddr];//pos可能会超过32K，&MASK的目的就是为了不越界
	head_[hashAddr] = pos;
}

//abcdefgh字符串
//hashaddr1:abc
//hashaddr2:bcd

//hashAddr:前一次计算出的哈希地址  abc
//本次需要计算bcd哈希地址
//ch:本次匹配三个字符中的最后一个
//本次哈希地址是在上一次哈希地址的基础上计算出来的
void LZHashTable::HashFunc(USH& hashAddr, UCH ch) {      //hashAddr是输入，输出型参数，ch是所查找字符串中第一个字符
	hashAddr = (((hashAddr) << H_SHIFT()) ^ (ch))&HASH_MASK;
}
USH LZHashTable::H_SHIFT() {
	return (HASH_BITS + MIN_MATCH - 1) / MIN_MATCH;     //5
}
