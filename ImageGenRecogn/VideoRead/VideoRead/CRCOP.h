#ifndef CRCOP_H     
#include<iostream>
#include "stdafx.h"
#define CRCOP_H   
#define Poly 11//�������ʽ����x3+x+1��1011����ʮ���Ʊ�ʾ
//ʵ��˼�룺
//        1���������ɱ��ʽ��G(x)=...(r��)
//        2��Ҫ�õ�У�����֡��M��x��=...
//        3����֡����rλ��
//        4������������㣻
//ǿ�ƣ��趨����λǰ��λ��У��λ����λ
class CRCOP{
public:

	unsigned int GetBits(unsigned int nData);//��ȡ��Ч�Ķ����� λ����
	unsigned int GetCRC(unsigned int nData,unsigned int nPoly);//����ָ������ʽ�õ�ָ�����ݵ�CRCУ����
	int chg2To10(char a[]);//������ת��Ϊ10����
	unsigned int Validation(char * rs);//��֤crc�������Ҫ����ʶ��˿�
	char *GetBitsData(int data);//���crcУ���Ķ����ƽ��
unsigned	int GetIndexBits(char * rs,int index);//��ȡĳһλ�õĶ���������
};
#endif