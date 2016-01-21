#ifndef CRCOP_H     
#include<iostream>
#include "stdafx.h"
#define CRCOP_H   
#define Poly 11//定义多项式函数x3+x+1（1011）的十进制表示
//实现思想：
//        1：定义生成表达式：G(x)=...(r阶)
//        2：要得到校验码的帧：M（x）=...
//        3：将帧左移r位；
//        4：进行异或运算；
//强制：设定数据位前五位，校验位后三位
class CRCOP{
public:

	unsigned int GetBits(unsigned int nData);//获取有效的二进制 位数；
	unsigned int GetCRC(unsigned int nData,unsigned int nPoly);//根据指定多项式得到指定数据的CRC校验码
	int chg2To10(char a[]);//二进制转化为10进制
	unsigned int Validation(char * rs);//验证crc结果，主要用于识别端口
	char *GetBitsData(int data);//获得crc校验后的二进制结果
unsigned	int GetIndexBits(char * rs,int index);//获取某一位置的二进制数据
};
#endif