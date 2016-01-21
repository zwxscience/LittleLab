#include "stdafx.h"
#include "CRCOP.h"
#include<iostream>
using namespace std;
/*
���ܣ���ȡ��Ч�Ķ����� λ����
������nData[in]:    Ҫ��ȡ������λ�������ݣ�
���أ��ɹ��������ݵĶ�����λ����
˵�����������Ϊ0���򷵻صĶ�����λ��Ϊ0��
�޸ļ�¼����
------------------------------------------------------------------*/

unsigned int CRCOP::GetBits(unsigned int nData)
{
	unsigned int nNum=0;
	while (nData!=0)
	{
		nData=nData>>1;
		nNum++;
	}
	return nNum;
}

/*
���ܣ�����ָ������ʽ�õ�ָ�����ݵ�CRCУ����
������nData[in]:Ҫ�õ�У��������ݣ�
nPoly[in]:Ԥ���趨�����ɶ���ʽ��
����ֵ���ɹ����صõ���CRCУ���룬���������ĩβ��У����;
-----------------------------------------------------------------*/
unsigned int  CRCOP::GetCRC(unsigned int nData,unsigned int nPoly)
{
	unsigned int nRank=GetBits(nPoly)-1;//�õ�����ʽ�Ľ�.�̶�ֵ��
	unsigned int nbits;
	unsigned int nFront;
	unsigned int nBehind;

	unsigned int nMulData = nData<<nRank;//�õ��ƶ�֮������ݣ�
	nbits =GetBits(nMulData);//�õ����ݵ�λ������������ݱ䶯���仯�� 
	/*��Ҫ���������ݶ�����λ��С�ڣ�nRank+1��˵���õ����յ�У����*/
	while(nbits>nRank)
	{
		nFront = nMulData>>(nbits-nRank-1); //��ʼ�����������ݡ��˺�ñ������Ǵ���������

		nBehind =nMulData^(nFront<<(nbits-nRank-1));//�õ���벿�ִ����������ݣ�

		//    printf("nMulData=%d,nFront=%d,nBehind=%d,nbits=%d ",nMulData,nFront,nBehind,nbits);
		nFront = nFront^nPoly;//�õ�������

		nMulData =(nFront<<(nbits-nRank-1))^nBehind; 
		//  printf("����=%d,��nMulData=%d ",nFront,nMulData);
		nbits = GetBits(nMulData);
	}
	return nMulData;
}

/*
���ܣ�������ת��Ϊ10����
������a[]:����Ķ��������ݣ�
����ֵ������10��������
-----------------------------------------------------------------*/
int  CRCOP::chg2To10(char a[])
{
	int bitv;
	int value = 0;
	int w = 1;
	for(int i=strlen(a)-1; i>=0; i--)
	{
		bitv = a[i];
		if(bitv == '1')
		{
			value += w;
		}
		else if(bitv != '0')
		{
		//	cout << "error��" << endl;
			// exit(0);
		}
		w *= 2;
	}
	//cout << a << "(2) = " << value << "(10)" << endl;
	return value;
}
/*
���ܣ����crcУ���Ķ���������
������data:�������ʮ�������ݣ�
����ֵ������������
-----------------------------------------------------------------*/
char * CRCOP::GetBitsData(int data){
	unsigned int crc=GetCRC(data,Poly);//���crc���
	unsigned int nbit=3;//�õ�����ʽ�Ľ׹̶�ֵ��GetBits(Poly)-1
	unsigned int resultData = (data<<nbit)^crc;//�õ��ƶ�֮������ݣ�
	char *rs=new char[9];
	memset(rs,'0',sizeof(char)*9);
	itoa(resultData, rs, 2);  //3������,��һ���Ǵ�ת������,�ڶ����Ǵ��ת������ַ���,����������Ҫת���Ľ���

	/*�õ��Ľ�����ܲ���8λ������Ҫ����*/
	int head=8-strlen(rs);//����ʣ��λ��
	int length=strlen(rs);//������Чλ��
	for(int i=0;i<length;i++){
		char r=rs[length-i-1];//�ƶ���Ч����λ��
		rs[7-i]=r;
	}
	for( int i=0;i<head;i++)
	{
		rs[i]='0';
	}
	rs[8]='\0';
	//cout<<"����ת��Ϊ��������"<<rs<<endl;
	return rs;
}
/*
���ܣ���֤crc�������Ҫ����ʶ��˿�
������rs[]:��ʶ��Ķ��������ݣ�
����ֵ���ɹ�����0����ʾʶ��ɹ�;����1��ʶ��ʧ��;
-----------------------------------------------------------------*/
unsigned int CRCOP::Validation(char * rs)
{
	int nData= chg2To10(rs);
	if((nData>>3)==4)
	{
		cout<<"��Ϊ4��9ͼ���Ƿ�ת��ϵ��������4"<<endl;
		return 1;
	}
	if((nData>>3)==0)
	{
		cout<<"���Ƽ�ʹ��0"<<endl;
		return 1;
	}
	unsigned int tag=GetCRC(nData,Poly);//11����֤����ʽ
	if(tag==0)return 0;
	else return 1;
}
/*
���ܣ���ȡĳһλ�õĶ���������
������rs[]:��ʶ��Ķ��������ݣ�
����ֵ���ɹ�����indexλ�õ�����
-----------------------------------------------------------------*/
unsigned int CRCOP::GetIndexBits(char * rs,int index)//
{
	int length=strlen(rs);
	if(index>=length) cout<<"��������ֵ���������ƴ�����"<<endl;
	return rs[index]-'0';
}