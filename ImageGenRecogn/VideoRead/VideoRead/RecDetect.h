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
#define  nbit	 3//j校验位
#define  margin	 50//预留边大小
struct  _result
{
	int  nX; 		//识别图形的中心X坐标（屏幕坐标系）
	int  nY;		//识别图形的中心Y坐标（屏幕坐标系）
	int  nType;	//识别图形的类型编号
	int data;//识别数字
};
 class RecDect{
public:
	RecDect();
	~RecDect(){
		cvReleaseImage( &img );
		cvReleaseImage( &img0 );
		cvClearMemStorage( storage );
		arr.clear();//每读取一张图需要清空数据
		answer.clear();
	//	vecResult.clear();
	}

	int WarpPerspective(IplImage *grey);//图像透视变换
	int ImageStretchByHistogram(IplImage *src,IplImage *dst);//图像对比度增强
	int otsu(const IplImage *frame);//阈值化
	double angle( CvPoint* pt1, CvPoint* pt2, CvPoint* pt0 );//检测四边形角度
	CvSeq* findSquares4( IplImage* img,int low,int high );//寻找四边形
//	CvSeq* findSquares(const cvMat& image);
	bool readFromTemp(char *temp,char * result0,int count);//检测读取信息并提取结果数据位
	void recognSquares( IplImage* img, CvSeq* squares );	// 识别所有四边形
	int minPoint(unsigned int a,unsigned int b,unsigned int c,unsigned int d);//寻找顶点最小点（x或y）
	int maxPoint(unsigned int a,unsigned int b,unsigned int c,unsigned int d);//寻找顶点最大点（x或y）
	int  GetResult(vector<_result>& vecResult);//输出识别出的所有图形的数据。
//	void  drawSquares( cvMat & image );
	void SetInputImage(IplImage* image,int low,/*检索矩形边长下限*/int high,/*检索矩形边长上限*/int mode);//功能说明： 输入图像并进行图像综合处理，识别。
public://存放变量
	IplImage* img;//用于参与计算
	IplImage* img0;//用于显示效果输出
	//vector<vector<CvPoint> > squares;
private:	

	CvMemStorage* storage;//存放轮廓顶点
	CvPoint pt[4];//临时存放矩形四个顶点
	vector<CvPoint> arr;//存放实际矩形坐标
	vector<int > answer;//存放实际矩形坐标对应值
	// read 4 sequence elements at a time (all vertices of a square)
	char result0[9];//存储识别字符串
	int thresh;//cany阈值

};
#endif