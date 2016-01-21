#include "stdafx.h"
#include "CRCOP.h"
#include<iostream>
using namespace std;
/*
功能：获取有效的二进制 位数；
参数：nData[in]:    要获取二进制位数的数据；
返回：成功返回数据的二进制位数。
说明：如果该数为0，则返回的二进制位数为0；
修改记录：无
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
功能：根据指定多项式得到指定数据的CRC校验码
参数：nData[in]:要得到校验码的数据；
nPoly[in]:预先设定的生成多项式；
返回值：成功返回得到的CRC校验码，既是添加在末尾的校验码;
-----------------------------------------------------------------*/
unsigned int  CRCOP::GetCRC(unsigned int nData,unsigned int nPoly)
{
	unsigned int nRank=GetBits(nPoly)-1;//得到多项式的阶.固定值。
	unsigned int nbits;
	unsigned int nFront;
	unsigned int nBehind;

	unsigned int nMulData = nData<<nRank;//得到移动之后的数据；
	nbits =GetBits(nMulData);//得到数据的位数，会根据数据变动而变化。 
	/*当要分析的数据二进制位数小于（nRank+1）说明得到最终的校验码*/
	while(nbits>nRank)
	{
		nFront = nMulData>>(nbits-nRank-1); //初始参与计算的数据。此后该变量都是代表余数；

		nBehind =nMulData^(nFront<<(nbits-nRank-1));//得到后半部分待分析的数据；

		//    printf("nMulData=%d,nFront=%d,nBehind=%d,nbits=%d ",nMulData,nFront,nBehind,nbits);
		nFront = nFront^nPoly;//得到余数；

		nMulData =(nFront<<(nbits-nRank-1))^nBehind; 
		//  printf("余数=%d,新nMulData=%d ",nFront,nMulData);
		nbits = GetBits(nMulData);
	}
	return nMulData;
}

/*
功能：二进制转化为10进制
参数：a[]:输入的二进制数据；
返回值：返回10进制数据
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
		//	cout << "error！" << endl;
			// exit(0);
		}
		w *= 2;
	}
	//cout << a << "(2) = " << value << "(10)" << endl;
	return value;
}
/*
功能：获得crc校验后的二进制数据
参数：data:待输入的十进制数据；
返回值：二进制数据
-----------------------------------------------------------------*/
char * CRCOP::GetBitsData(int data){
	unsigned int crc=GetCRC(data,Poly);//获得crc结果
	unsigned int nbit=3;//得到多项式的阶固定值。GetBits(Poly)-1
	unsigned int resultData = (data<<nbit)^crc;//得到移动之后的数据；
	char *rs=new char[9];
	memset(rs,'0',sizeof(char)*9);
	itoa(resultData, rs, 2);  //3个参数,第一个是待转换的数,第二个是存放转换后的字符串,第三个即需要转换的进制

	/*得到的结果可能不是8位数，需要补足*/
	int head=8-strlen(rs);//计算剩余位数
	int length=strlen(rs);//计算有效位数
	for(int i=0;i<length;i++){
		char r=rs[length-i-1];//移动有效数据位置
		rs[7-i]=r;
	}
	for( int i=0;i<head;i++)
	{
		rs[i]='0';
	}
	rs[8]='\0';
	//cout<<"该数转换为二进制是"<<rs<<endl;
	return rs;
}
/*
功能：验证crc结果，主要用于识别端口
参数：rs[]:待识别的二进制数据；
返回值：成功返回0，表示识别成功;返回1则识别失败;
-----------------------------------------------------------------*/
unsigned int CRCOP::Validation(char * rs)
{
	int nData= chg2To10(rs);
	if((nData>>3)==4)
	{
		cout<<"因为4和9图像是反转关系，不采用4"<<endl;
		return 1;
	}
	if((nData>>3)==0)
	{
		cout<<"不推荐使用0"<<endl;
		return 1;
	}
	unsigned int tag=GetCRC(nData,Poly);//11是验证多项式
	if(tag==0)return 0;
	else return 1;
}
/*
功能：获取某一位置的二进制数据
参数：rs[]:待识别的二进制数据；
返回值：成功返回index位置的数据
-----------------------------------------------------------------*/
unsigned int CRCOP::GetIndexBits(char * rs,int index)//
{
	int length=strlen(rs);
	if(index>=length) cout<<"错误：索引值超过二进制串长度"<<endl;
	return rs[index]-'0';
}