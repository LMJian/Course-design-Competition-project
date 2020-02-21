#pragma once

typedef unsigned char UCH;       //匹配字符串长度
typedef unsigned short USH;      //所匹配字符串距离
typedef unsigned long long ULL;

const USH MIN_MATCH = 3;     //最少匹配三个字符
const USH MAX_MATCH = 258;   //一个字节范围[0,255]   匹配长度范围[3,258]
const USH WSIZE = 32 * 1024;  //32k