#pragma once

typedef unsigned char UCH;       //ƥ���ַ�������
typedef unsigned short USH;      //��ƥ���ַ�������
typedef unsigned long long ULL;

const USH MIN_MATCH = 3;     //����ƥ�������ַ�
const USH MAX_MATCH = 258;   //һ���ֽڷ�Χ[0,255]   ƥ�䳤�ȷ�Χ[3,258]
const USH WSIZE = 32 * 1024;  //32k