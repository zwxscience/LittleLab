#ifndef _STDIO_H_
#include "stdafx.h"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/highgui/highgui.hpp"
#include <vector>
using namespace std;
#define _STDIO_H_

#define GrayScale 256
#define  nbit	 3//jУ��λ
#define  margin	 50//Ԥ���ߴ�С
struct  _result
{
	int  nX; 		//ʶ��ͼ�ε�����X���꣨��Ļ����ϵ��
	int  nY;		//ʶ��ͼ�ε�����Y���꣨��Ļ����ϵ��
	int  nType;	//ʶ��ͼ�ε����ͱ��
	int data;//ʶ������
};
 class RecDect{
public:
	RecDect();
	~RecDect(){
		cvReleaseImage( &img );
		cvReleaseImage( &img0 );
		cvClearMemStorage( storage );
		arr.clear();//ÿ��ȡһ��ͼ��Ҫ�������
		answer.clear();
	//	vecResult.clear();
	}

	int WarpPerspective(IplImage *grey);//ͼ��͸�ӱ任
	int ImageStretchByHistogram(IplImage *src,IplImage *dst);//ͼ��Աȶ���ǿ
	int otsu(const IplImage *frame);//��ֵ��
	double angle( CvPoint* pt1, CvPoint* pt2, CvPoint* pt0 );//����ı��νǶ�
	CvSeq* findSquares4( IplImage* img,int low,int high );//Ѱ���ı���
//	CvSeq* findSquares(const cvMat& image);
	bool readFromTemp(char *temp,char * result0,int count);//����ȡ��Ϣ����ȡ�������λ
	void recognSquares( IplImage* img, CvSeq* squares );	// ʶ�������ı���
	int minPoint(unsigned int a,unsigned int b,unsigned int c,unsigned int d);//Ѱ�Ҷ�����С�㣨x��y��
	int maxPoint(unsigned int a,unsigned int b,unsigned int c,unsigned int d);//Ѱ�Ҷ������㣨x��y��
	int  GetResult(vector<_result>& vecResult);//���ʶ���������ͼ�ε����ݡ�
//	void  drawSquares( cvMat & image );
	void SetInputImage(IplImage* image,int low,/*�������α߳�����*/int high,/*�������α߳�����*/int mode);//����˵���� ����ͼ�񲢽���ͼ���ۺϴ���ʶ��
public://��ű���
	IplImage* img;//���ڲ������
	IplImage* img0;//������ʾЧ�����
	//vector<vector<CvPoint> > squares;
private:	

	CvMemStorage* storage;//�����������
	CvPoint pt[4];//��ʱ��ž����ĸ�����
	vector<CvPoint> arr;//���ʵ�ʾ�������
	vector<int > answer;//���ʵ�ʾ��������Ӧֵ
	// read 4 sequence elements at a time (all vertices of a square)
	char result0[9];//�洢ʶ���ַ���
	int thresh;//cany��ֵ

};
#endif