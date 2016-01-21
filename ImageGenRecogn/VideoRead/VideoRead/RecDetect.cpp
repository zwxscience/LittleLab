// RecDetect.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/highgui/highgui.hpp"
#include"CRCOP.h"
#include <vector>
#include "RecDetect.h"
#include <iostream>
#include <math.h>
#include <string.h>
using namespace std;
using namespace cv;


RecDect::RecDect(){
	thresh=800;
	storage = cvCreateMemStorage(0);
}


/*************************************************
功能:        图像增强
描述:     因为摄像头图像质量差，需要根据直方图进行图像增强，
将图像灰度的域值拉伸到0-255    
参数说明:    
src： 单通道灰度图像 
dst：返回处理后的图像
返回值:          同样大小的单通道灰度图像 
*************************************************/
int RecDect::ImageStretchByHistogram(IplImage *src,IplImage *dst)

{
	//p[]存放图像各个灰度级的出现概率；
	//p1[]存放各个灰度级之前的概率和，用于直方图变换；
	//num[]存放图象各个灰度级出现的次数;

	assert(src->width==dst->width);//保证一致性
	float p[GrayScale],p1[GrayScale],num[GrayScale];
	//清空三个数组
	memset(p,0,sizeof(p));
	memset(p1,0,sizeof(p1));
	memset(num,0,sizeof(num));

	int height=src->height;
	int width=src->width;
	long wMulh = height * width;

	//求存放图象各个灰度级出现的次数
	// to do use openmp
	for(int x=0;x<width;x++)
	{
		for(int y=0;y<height;y++)
		{
			uchar v=((uchar*)(src->imageData + src->widthStep*y))[x];
			num[v]++;
		}
	}

	//求存放图像各个灰度级的出现概率
	for(int i=0;i<GrayScale;i++)
	{
		p[i]=num[i]/wMulh;
	}

	//求存放各个灰度级之前的概率和
	for(int i=0;i<GrayScale;i++)
	{
		for(int k=0;k<=i;k++)
			p1[i]+=p[k];
	}

	//直方图变换
	// to do use openmp
	for(int x=0;x<width;x++)
	{
		for(int y=0;y<height;y++)
		{
			uchar v=((uchar*)(src->imageData + src->widthStep*y))[x];
			((uchar*)(dst->imageData + dst->widthStep*y))[x]= p1[v]*255+0.5;            
		}
	}

	return 0;

}
/*************************************************
功能:        图像透视变换
描述:    因为处理后的图片会发生扭曲，旋转等，需要将其校正   
参数说明:    
grey： 需要处理的图像 
返回值:       0
*************************************************/
int RecDect::WarpPerspective(IplImage *grey)
{
	IplImage* Img_old = cvCloneImage( grey );

	float fix = 1; 
	float w = (float)Img_old->width + fix;
	float h = (float)Img_old->height + fix;

	CvPoint2D32f src_point[4];
	CvPoint2D32f dst_point[4];
	int minx,miny;
	CvRect crt;
	crt.x=(pt[1].x>=pt[2].x?pt[2].x:pt[1].x);
	//expand
	if((crt.x-margin)>0){
		minx=crt.x-margin;
	}else{
		minx=0;
	}
	crt.y=(pt[0].y>=pt[1].y?pt[1].y:pt[0].y);
	if(crt.y-margin>0){
		miny=crt.y-margin;
	}
	else{
		miny=0;
	}

	//设定源观察面，即指定四边形的四个顶点
	//左下角
	src_point[0].x=(float)pt[2].x-minx;             
	src_point[0].y=(float)pt[2].y-miny;
	//右下角
	src_point[1].x=(float)pt[3].x-minx;                    
	src_point[1].y=(float)pt[3].y-miny;
	//左上角
	src_point[2].x=(float)pt[1].x-minx; 
	src_point[2].y=(float)pt[1].y-miny;
	//右上角
	src_point[3].x=(float)pt[0].x-minx; 
	src_point[3].y=(float)pt[0].y-miny;


	//设定目标观察面，即指定四边形的四个顶点
	dst_point[0].x=-fix;               
	dst_point[0].y=h;

	dst_point[1].x=w;              
	dst_point[1].y=h;

	dst_point[2].x=-fix;
	dst_point[2].y=-fix;

	dst_point[3].x=w;
	dst_point[3].y=-fix;


	float newm[9];            
	CvMat newM = cvMat( 3, 3, CV_32F, newm );
	//获得透视转换矩阵
	//cvWarpPerspectiveQMatrix(src_point,dst_point,&newM);
	cvGetPerspectiveTransform(src_point,dst_point,&newM);
	//透视转换
	cvWarpPerspective(Img_old,grey,&newM,CV_INTER_LINEAR+CV_WARP_FILL_OUTLIERS, cvScalarAll(0) );

	//cvNamedWindow("ImgWarp",1);
	//cvShowImage("ImgWarp",grey);
	//cvWaitKey(0);

	cvReleaseImage(&Img_old);
	return 0;
}
/*************************************************
功能:  图像OTSU二值化，大津法求阈值
描述:    图像最终目的是黑白图，需要利用合理的方法进行二值化
参数说明:    
frame： 需要处理的单通道灰度图像 
返回值:      阈值分割点
*************************************************/
int RecDect::otsu(const IplImage *frame) 
{
	//frame灰度级
	int pixelCount[GrayScale]={0};
	float pixelPro[GrayScale]={0};
	int width=frame->width;
	int height=frame->height;
	int i, j, pixelSum = width * height, threshold = 0;
	uchar* data = (uchar*)frame->imageData;
	float w0, w1, u0tmp, u1tmp, u0, u1, deltaTmp, deltaMax = 0;
	//统计每个灰度级中像素的个数
	for(i = 0; i < height; i++)
	{
		for(j = 0;j < width;j++)
		{
			pixelCount[(int)data[i * width + j]]++;
		}
	}

	//计算每个灰度级的像素数目占整幅图像的比例
	for(i = 0; i < GrayScale; i++)
	{
		pixelPro[i] = (float)pixelCount[i] / pixelSum;
	}

	//遍历灰度级[0,255],寻找合适的threshold

	for(i = 0; i < GrayScale; i++)
	{
		w0 = w1 = u0tmp = u1tmp = u0 = u1 = deltaTmp = 0;
		for(j = 0; j < GrayScale; j++)
		{
			if(j <= i)   //背景部分
			{
				w0 += pixelPro[j];
				u0tmp += j * pixelPro[j];
			}
			else   //前景部分
			{
				w1 += pixelPro[j];
				u1tmp += j * pixelPro[j];
			}
		}
		u0 = u0tmp / w0;
		u1 = u1tmp / w1;
		deltaTmp = (float)(w0 *w1* pow((u0 - u1), 2)) ;
		if(deltaTmp > deltaMax)
		{
			deltaMax = deltaTmp;
			threshold = i;
		}
	}
	return threshold;
}

/*************************************************
功能: 检查四边形角度
描述:    轮廓检索的图像不一定是矩形，需要利用规则排除
参数说明:    
pt1：  pt2：pt0：检测到的图形三个顶点的坐标
返回值:   他们之间的夹角的余弦
*************************************************/
double RecDect::angle( CvPoint* pt1, CvPoint* pt2, CvPoint* pt0 )
{
	double dx1 = pt1->x - pt0->x;
	double dy1 = pt1->y - pt0->y;
	double dx2 = pt2->x - pt0->x;
	double dy2 = pt2->y - pt0->y;
	return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}
/*************************************************
功能: 返回一系列检测到的四边形
描述:    原始图像利用轮廓检索的图像四边形
参数说明:    
img：原始图像
int low:设定检索矩形的最短变长，下限。默认值：25单位：像素
,int high：:设定检索矩形的最长变长，上限。默认值：220单位：像素
上下限定位的越精确，识别速度越快
返回值:   一系列四边形顶点坐标
*************************************************/
CvSeq* RecDect::findSquares4( IplImage* img,int low=25,int high=220 )
{
	CvSeq* contours;
	int i, c, l, N = 1;
	CvSize sz = cvSize( img->width & -2, img->height & -2 );
	IplImage* timg = cvCloneImage( img ); //复制
	IplImage* gray = cvCreateImage( sz, 8, 1 ); 
	IplImage* pyr = cvCreateImage( cvSize(sz.width/2, sz.height/2), 8, 3 );
	IplImage* tgray;
	CvSeq* result;
	double s, t;
	// 创建将要包含四边形定点的序列
	// 每四个点是一个四边图形
	CvSeq* squares = cvCreateSeq( 0, sizeof(CvSeq), sizeof(CvPoint), storage );

	// 选择图像最大区域
	// 其中宽高都除以2
	cvSetImageROI( timg, cvRect( 0, 0, sz.width, sz.height ));

	// 消除噪声
	cvPyrDown( timg, pyr, 7 );
	cvPyrUp( pyr, timg, 7 );
	tgray = cvCreateImage( sz, 8, 1 );

	//寻找每一个通道的轮廓
	for( c = 0; c < 1; c++ )
	{
		// 扩招C通道
		cvSetImageCOI( timg, 2 );
		cvCopy( timg, tgray, 0 );
	//cvCvtColor(timg,tgray,CV_BGR2GRAY);
		// 尝试多种阈值
		for( l = 0; l < N; l++ )
		{
			// 利用Canny 
			// Canny利用gradient shading  来捕捉方块
			if( l == 0 )
			{
				// 应用 Canny算法. 上限800
				// 下限 0 
				cvCanny( tgray, gray, 0, 800, 5 );
				// 膨胀：消除边之间潜在的洞
			//	cvDilate( gray, gray, 0, 1 );
				//cvErode(gray,gray,0,4);
			}
			else
			{
				// apply threshold if l!=0:
				//     tgray(x,y) = gray(x,y) < (l+1)*255/N ? 255 : 0
				cvThreshold( tgray, gray, (l+1)*255/N,255, CV_THRESH_BINARY );
			}
	      // int  thread= otsu(tgray);
		//   	cvThreshold( tgray, gray,thread, 255,CV_THRESH_BINARY );
			cvShowImage("d",gray);
			// find contours and store them all as a list
			cvFindContours( gray, storage, &contours, sizeof(CvContour),
				CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0) );
		//	CvPoint pt[4], *rect = pt;
		//	int count = 4;
			// test each contour
			while( contours )
			{
				// approximate contour with accuracy proportional
				// to the contour perimeter
				result = cvApproxPoly( contours, sizeof(CvContour), storage,
					CV_POLY_APPROX_DP, cvContourPerimeter(contours)*0.02, 0 );
				// 区块要有四个边
				// 周长在100到850
				//检测是否是凸型
				//也可以利用面积求解if( result->total == 4 &&
				//	fabs(cvContourArea(result,CV_WHOLE_SEQ)) > 1000 &&cvContourPerimeter(contours) > low*4 &&cvContourPerimeter(contours) <high*4
				//	cvCheckContourConvexity(result) )
				if( result->total == 4 &&
				cvContourPerimeter(contours) > low*4 &&cvContourPerimeter(contours) <high*4&&
					cvCheckContourConvexity(result) )

				{
					s = 0;

					for( i = 0; i < 5; i++ )
					{
						// find minimum angle between joint
						// edges (maximum of cosine)
						if( i >= 2 )
						{
							t = fabs(angle(
								(CvPoint*)cvGetSeqElem( result, i ),
								(CvPoint*)cvGetSeqElem( result, i-2 ),
								(CvPoint*)cvGetSeqElem( result, i-1 )));
							s = s > t ? s : t;
						}
					}

					// 角度在允许区域内，即允许角度在arccos0.3到90度之间
					if( s < 0.3 )
						for( i = 0; i < 4; i++ ){
							cvSeqPush( squares,
								(CvPoint*)cvGetSeqElem( result, i ));
						}
				}
				//采用下一个轮廓
				contours = contours->h_next;
			}
		}
	}
	  //  cvShowImage( "Test", timg );
	// 释放临时图像
	cvReleaseImage( &gray );
	cvReleaseImage( &pyr );
	cvReleaseImage( &tgray );
	cvReleaseImage( &timg );

	return squares;
}
//CvSeq* RecDect::findSquares( const Mat& image, vector<vector<Point> >& squares )
//{
//	squares.clear();
//	int thresh = 50, N = 11;
//	Mat pyr, timg, gray0(image.size(), CV_8U), gray;
//
//	// down-scale and upscale the image to filter out the noise
//	pyrDown(image, pyr, Size(image.cols/2, image.rows/2));
//	pyrUp(pyr, timg, image.size());
//	vector<vector<Point> > contours;
//
//	// find squares in every color plane of the image
//	for( int c = 0; c < 3; c++ )
//	{
//		int ch[] = {c, 0};
//		mixChannels(&timg, 1, &gray0, 1, ch, 1);
//
//		// try several threshold levels
//		for( int l = 0; l < N; l++ )
//		{
//			// hack: use Canny instead of zero threshold level.
//			// Canny helps to catch squares with gradient shading
//			if( l == 0 )
//			{
//				// apply Canny. Take the upper threshold from slider
//				// and set the lower to 0 (which forces edges merging)
//				Canny(gray0, gray, 0, thresh, 5);
//				// dilate canny output to remove potential
//				// holes between edge segments
//				dilate(gray, gray, Mat(), Point(-1,-1));
//			}
//			else
//			{
//				// apply threshold if l!=0:
//				//     tgray(x,y) = gray(x,y) < (l+1)*255/N ? 255 : 0
//				gray = gray0 >= (l+1)*255/N;
//			}
//
//			// find contours and store them all as a list
//			findContours(gray, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
//
//			vector<Point> approx;
//
//			// test each contour
//			for( size_t i = 0; i < contours.size(); i++ )
//			{
//				// approximate contour with accuracy proportional
//				// to the contour perimeter
//				approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.02, true);
//
//				// square contours should have 4 vertices after approximation
//				// relatively large area (to filter out noisy contours)
//				// and be convex.
//				// Note: absolute value of an area is used because
//				// area may be positive or negative - in accordance with the
//				// contour orientation
//				if( approx.size() == 4 &&
//					fabs(contourArea(Mat(approx))) > 1000 &&
//					isContourConvex(Mat(approx)) )
//				{
//					double maxCosine = 0;
//
//					for( int j = 2; j < 5; j++ )
//					{
//						// find the maximum cosine of the angle between joint edges
//						double cosine = fabs(angle(approx[j%4], approx[j-2], approx[j-1]));
//						maxCosine = MAX(maxCosine, cosine);
//					}
//
//					// if cosines of all angles are small
//					// (all angles are ~90 degree) then write quandrange
//					// vertices to resultant sequence
//					if( maxCosine < 0.3 )
//						squares.push_back(approx);
//				}
//			}
//		}
//	}
//}
// the function draws all the squares in the image
//void RecDect::drawSquares( Mat& image, const vector<vector<Point> >& squares )
//{
//	for( size_t i = 0; i < squares.size(); i++ )
//	{
//		const Point* p = &squares[i][0];
//		int n = (int)squares[i].size();
//		polylines(image, &p, &n, 1, true, Scalar(0,255,0), 3, CV_AA);
//	}
//
//	imshow("Test", image);
//}


/*************************************************
功能：输出识别出的所有图形的数据。: 绘图，标记识别物体
参数：vecResult存放图像坐标编号等
返回值：识别出的图形个数。
arr：存放窗口矩形坐标（连续的四个为一个矩形）
answer：存放arr对应矩形代表值
vecResult：存放矩形屏幕坐标位置和编号
功能说明：函数调用需先清空vecResult，之后将识别的图形相关数据填入这个vector。返回值为0表示没有找到可识别图形。
*************************************************/
int  RecDect::GetResult( vector<_result>& vecResult )
{
	vecResult.clear();//清空数据
	for(unsigned int i=0;i<arr.size();i+=4){
		int edge=(int)sqrt((float)(arr[i+1].x-arr[i].x)*(arr[i+1].x-arr[i].x)+(float)(arr[i+1].y-arr[i].y)*(arr[i+1].y-arr[i].y));//边长
		CvPoint ct=cvPoint((arr[i].x+arr[i+2].x)/2,(arr[i].y+arr[i+2].y)/2);//图形中心
		_result temprs;//存放临时坐标等信息
		temprs.nType=i/4;//识别图形编号，从0开始
		temprs.nX=(arr[i].x+arr[i+2].x)/2;
		temprs.nY=(arr[i].y+arr[i+2].y)/2;
		temprs.data=answer.at(i/4);
		vecResult.push_back(temprs);
		//下面为绘图功能如不想要，删掉即可
		cvCircle( img0, ct ,edge , CV_RGB(255,255,0),1, 8, 3 );
		CvFont font;
		double hscale = 0.5f;
		double vscale = 0.5f;
		cvInitFont(&font,CV_FONT_HERSHEY_SIMPLEX  ,hscale,vscale,0);
		int data=i/4;
		char *number1=new char[2];
		itoa(data,number1,10);
		string filename="ID:";//图像标识号
		filename.append(number1);
		filename.append(",number:");//识别矩形代表位置
		char *number=new char[2];
		CRCOP cp;
		int resultData10=cp.chg2To10(result0)>>nbit;//10进制结果 
		data=answer.at(i/4);
		itoa(data,number,10);
		filename.append(number);
		filename.append(".");
		const char * file=filename.c_str();
		CvPoint ct2=cvPoint(ct.x-edge/2,ct.y);//图形中心-1/2边长作为文字输出位置
		cvPutText(img0,file,ct2,&font,cvScalar(255,255,0));
		int count = 4;
		CvPoint* rect=new CvPoint[4];
		rect[0]=arr[i];
		rect[1]=arr[i+1];
		rect[2]=arr[i+2];
		rect[3]=arr[i+3];
		cvPolyLine( img0, &rect, &count, 1, 1, CV_RGB(0,255,255), 1, CV_AA, 0 );
		//上面为绘图功能如不想要，删掉即可

	}
	cvShowImage( "结果输出", img0 );
	return arr.size()/4;//返回图像个数
	//返回值为0表示没有找到可识别图形

}
/*************************************************
功能: 检测读取信息并提取结果数据位
描述:  通过01识别字符串，将其进行识别校正并提取数据位，允许旋转
参数说明:    
temp：识别的字符串
result0：存放结果数据位
返回值:   true提取成功，反之，失败
*************************************************/
bool RecDect::readFromTemp(char *temp,char * result0,int count){//从识别矩阵提取所需字节
	int a=0,a1=0;//分别作为正常，旋转180的标志
	for(int i=8;i<12;i++){//正常情况
		if (temp[i]=='0')//计算白色色栅格总数
		{
			a++;
		}
	}
	for(int i=12;i<16;i++){//正常情况
		if (temp[i]=='1')//计算黑色栅格总数
		{
			a++;
		}
	}
	if (a==8)
	{
		for(int i=0;i<8;i++) result0[i]=temp[i];
		return true;
	}else{//分别旋转90,180,270
		if(temp[0]=='1'&&temp[1]=='0'&&temp[4]=='1'&&temp[5]=='0'&&temp[8]=='1'&&temp[9]=='0'&&temp[12]=='1'&&temp[13]=='0')//旋转90
		{
			result0[0]=temp[3];
			result0[1]=temp[7];
			result0[2]=temp[11];
			result0[3]=temp[15];
			result0[4]=temp[2];
			result0[5]=temp[6];
			result0[6]=temp[10];
			result0[7]=temp[14];
			return true;
		}else{//分别旋转180,270
			for(int i=0;i<4;i++){//正常情况
				if (temp[i]=='1')//计算黑色栅格总数
				{
					a1++;
				}
			}
			for(int i=4;i<8;i++){//正常情况
				if (temp[i]=='0')//计算黑色栅格总数
				{
					a1++;
				}
			}
			if (a1==8)
			{
				result0[0]=temp[15];
				result0[1]=temp[14];
				result0[2]=temp[13];
				result0[3]=temp[12];
				result0[4]=temp[11];
				result0[5]=temp[10];
				result0[6]=temp[9];
				result0[7]=temp[8];
				return true;
			}else{//旋转270
				if(temp[2]=='0'&&temp[3]=='1'&&temp[6]=='0'&&temp[7]=='1'&&temp[10]=='0'&&temp[11]=='1'&&temp[14]=='0'&&temp[15]=='1'){
					result0[0]=temp[12];
					result0[1]=temp[8];
					result0[2]=temp[4];
					result0[3]=temp[0];
					result0[4]=temp[13];
					result0[5]=temp[9];
					result0[6]=temp[5];
					result0[7]=temp[1];
					return true;
				}else{//不识别
				//	cout<<"该图像第"<<count<<"个矩形不符合."<<endl;
					return false;
				}
			}
		}
	}

}

// 
/*************************************************
功能: 识别所有四边形
描述:  通过轮廓提取并提取出符合规则四边形后，需要进行校正，二值化
识别图像，提取数据位，翻译校验，存放结果
参数说明:    
img：识别的图像
squares：已经检测出来的序列
*************************************************/
void RecDect::recognSquares( IplImage* img, CvSeq* squares )
{
	CvSeqReader reader;
	IplImage* cpy = cvCloneImage( img );
	//IplImage* cpy1 = cvCloneImage( img );
	int i;
	// 初始读取的序列
	cvStartReadSeq( squares, &reader, 0 );

	for( i = 0; i < squares->total; i += 4 )
	{
		CvPoint* rect = pt;
		int count = 4;

		// read 4 vertices
		memcpy( pt, reader.ptr, squares->elem_size );
		CV_NEXT_SEQ_ELEM( squares->elem_size, reader );
		memcpy( pt + 1, reader.ptr, squares->elem_size );
		CV_NEXT_SEQ_ELEM( squares->elem_size, reader );
		memcpy( pt + 2, reader.ptr, squares->elem_size );
		CV_NEXT_SEQ_ELEM( squares->elem_size, reader );
		memcpy( pt + 3, reader.ptr, squares->elem_size );
		CV_NEXT_SEQ_ELEM( squares->elem_size, reader );
		int numbercount=(i/4)+1;

		cout<<"这是在该图片搜索到的第 "<<numbercount<<" 个矩形，现在检测是否和以前的重合："<<endl;
	  		// cvPolyLine( cpy1, &rect, &count, 1, 1, CV_RGB(0,255,0), 3, CV_AA, 0 );
		bool tag1=false;
		float edge=sqrt((float)(pt[1].x-pt[0].x)*(pt[1].x-pt[0].x)+(float)(pt[1].y-pt[0].y)*(pt[1].y-pt[0].y));//边长
		for(unsigned  int m=0; m<arr.size(); m+=4)
		{
			int x=arr.at(m).x;
			int y=arr.at(m).y;
			float dis=sqrt((float)(x-pt[0].x)*(x-pt[0].x)+(float)(y-pt[0].y)*(y-pt[0].y));//右上角坐标的距离

			if(5*dis<edge){
				//如果存在两个图形右上角坐标的距离小于边长，认为他们重合是一个图形。
				cout<<"和以前重合，略过。"<<endl;
				tag1=true;
			}

		}
		if(tag1)//本次图形重合，跳出
			continue;
		
		/*
		4 vertices结构：
		P2***P1
		*     *
		P3***P4
		确定了四个角度之后需要对每一个图形进行进行区域校正采样识别
		*/
		cout<<"不重合"<<endl;

		CvRect crt;
		crt.x=minPoint(pt[0].x,pt[1].x,pt[2].x,pt[3].x);
		//expand
		crt.x=(crt.x-margin)>0?(crt.x-margin):0;
		crt.y=minPoint(pt[0].y,pt[1].y,pt[2].y,pt[3].y);
		crt.y=(crt.y-margin)>0?(crt.y-margin):0;
		crt.height=maxPoint(pt[0].y,pt[1].y,pt[2].y,pt[3].y)-crt.y+2*margin;
		crt.height=(crt.height+crt.y)>(cpy->height)?(cpy->height-crt.y):crt.height;
		crt.width=maxPoint(pt[0].x,pt[1].x,pt[2].x,pt[3].x)-crt.x+2*margin;
		crt.width=(crt.width+crt.x)>(cpy->width)?(cpy->width-crt.x):crt.width;


		cvSetImageROI(cpy,crt);
		//图形不重复则进行图像变换

		IplImage *recImage=cvCreateImage(cvSize(crt.width,crt.height),cpy->depth,cpy->nChannels);
		//cvGetSubRect()
		cvCopy(cpy,recImage);
		WarpPerspective(recImage);//图像透视校正
		IplImage *recImageG=cvCreateImage(cvSize(recImage->width,recImage->height),recImage->depth,1);
		cvCvtColor(recImage,recImageG,CV_BGR2GRAY);
		cvReleaseImage( &recImage );
		int threshold=otsu(recImageG);//二值化
		for(int i=0;i<recImageG->height;i++)
		{
			for(int j=0;j<recImageG->width;j++)
			{
				CvScalar 	s=cvGet2D(recImageG,i,j);
				int m=(int)s.val[0];
				if(m>=threshold)
					m=255;
				else
					m=0;
				s.val[0]=m;
				cvSet2D(recImageG,i,j,s);  
			}
		}
		//01字符串识别
		memset(result0,'0',sizeof(char)*8);
		char *temp=new char[16];//存储矩阵识别结果
		memset(result0,'0',sizeof(char)*16);

		int index=0;
		for(int y=recImageG->height/18,m=0;y<recImageG->height*17/18,m<4;y+=recImageG->height*2/9,m++)
		{
			uchar *ptr=(uchar *)(recImageG->imageData+(y+recImageG->height/9)*recImageG->widthStep);
			for(int x=recImageG->width/18,n=0;x<recImageG->width*17/18,n<4;x+=recImageG->width*2/9,n++)
			{
				int getPixValue=ptr[recImageG->nChannels*(x+recImageG->width/9)];
				// CvScalar  getPixValue=cvGet2D(img,recImageG->nChannels*(x+recImageG->width/8),(y+recImageG->height/8)); // get   pixel value
				if (getPixValue>0)
				{
					temp[index]='0';//黑色是1，白色是0
					CvPoint centerpoint=cvPoint(recImageG->nChannels*x+recImageG->width/9,(y+recImageG->height/9));
					cvCircle( recImageG, centerpoint ,3 , CV_RGB(0,0,0),1, 8, 3 );

				}
				else{
					temp[index]='1';//黑色是1，白色是0
					CvPoint centerpoint=cvPoint(recImageG->nChannels*(x+recImageG->width/9),(y+recImageG->height/9));
					cvCircle( recImageG, centerpoint ,3 , CV_RGB(255,255,255),1, 8, 3 );

				}
				index++;
			}
		}		
		temp[17]='\0';
		//通过分析字符串校正图像之定位，保证定位角在下
		//已经将矩阵存储如temp，需要分析定位图形来提取字符
		//cvShowImage("区域截图转化",recImageG);
		bool tag2=false;
		if(!readFromTemp(temp,result0,numbercount)){//找不到定位图形则失败
			tag2=true;
		}
		if(tag2)continue;
		//cout<<"识别矩阵字符串结果为 "<<temp<<" ."<<endl;
		result0[8]='\0';
		//cout<<"识别01字符串结果为 "<<result0<<" ."<<endl;
		CRCOP cp;
		//验证程序

		int resultData10=0;
		bool tag3=false;
		if(cp.Validation(result0)==0){
			cout<<"验证成功"<<endl;
			resultData10=cp.chg2To10(result0)>>nbit;//10进制结果 	
			cout<<"第"<<numbercount<<"个矩形的识别结果是"<<resultData10<<endl;
			answer.push_back(resultData10);
			//装载实际图形
			arr.push_back(cvPoint(pt[0].x,pt[0].y));
			arr.push_back(cvPoint(pt[1].x,pt[1].y));
			arr.push_back(cvPoint(pt[2].x,pt[2].y));
			arr.push_back(cvPoint(pt[3].x,pt[3].y));
		}
		else 
		{
			cout<<"第一次识别失败，图像可能反转，尝试反转图像。。"<<endl;
			char *result1=new char[9];
			memset(result1,'0',sizeof(char)*8);
			strcpy(result1,result0);
			//cout<<"result1:"<<result1<<endl;
			result0[0]=result1[3];
			result0[1]=result1[2];
			result0[2]=result1[1];
			result0[3]=result1[0];
			result0[4]=result1[7];
			result0[5]=result1[6];
			result0[6]=result1[5];
			result0[7]=result1[4];
			result0[8]='\0';//强制结束末尾
			if(cp.Validation(result0)==0){
				cout<<"验证成功"<<endl;
				resultData10=cp.chg2To10(result0)>>nbit;//10进制结果 	
				cout<<"第"<<numbercount<<"个矩形的识别结果是"<<resultData10<<endl;
				//装载实际图形
				arr.push_back(cvPoint(pt[0].x,pt[0].y));
				arr.push_back(cvPoint(pt[1].x,pt[1].y));
				arr.push_back(cvPoint(pt[2].x,pt[2].y));
				arr.push_back(cvPoint(pt[3].x,pt[3].y));
				answer.push_back(resultData10);
			}
			else
			{
				cout<<"翻转后仍然不能识别，第"<<numbercount<<"个矩形验证失败，不是需要的结果."<<endl;
				tag3=false;
			}
		}
		if(tag3)continue;
		//cvNamedWindow("区域截图转化");
		//cvShowImage("区域截图转化",recImageG);

		cvReleaseImage(&recImageG);
		cvResetImageROI(cpy);
	}

	cout<<"本张图片共搜到"<<squares->total/4<<"个矩形，经过图像运算，实际保留图形"<<arr.size()/4<<"个"<<endl;
	// 显示图像
	///cvShowImage("Test",cpy1);
	//	cvReleaseImage( &cpy1 );
	cvReleaseImage( &cpy );
}
/*************************************************
功能: 检测图像矩形的坐标最小点
描述:  检测图像矩形的坐标最小点
参数说明:    
unsigned int a,unsigned int b,unsigned int c,unsigned int d：矩形四个顶点x/y坐标
返回值：最小的x/y值
*************************************************/
int RecDect::minPoint(unsigned int a,unsigned int b,unsigned int c,unsigned int d){
	unsigned int tem1=(b>a? a:b);
	unsigned int tem2=(c>d? d:c);
	return (tem1>tem2? tem2:tem1);
}
/*************************************************
功能: 检测图像矩形的坐标最大点
描述:  检测图像矩形的坐标最大点
参数说明:    
unsigned int a,unsigned int b,unsigned int c,unsigned int d：矩形四个顶点x/y坐标
返回值：最大的x/y值
*************************************************/
int RecDect::maxPoint(unsigned int a,unsigned int b,unsigned int c,unsigned int d){
	unsigned int tem1=(b>a? b:a);
	unsigned int tem2=(c>d? c:d);
	return (tem1>tem2? tem1:tem2);
}
/*************************************************
描述:  输入图像并进行图像综合处理，识别。
参数说明:    
参数：
image:输入摄像头捕捉画面
int low:设定检索矩形的最短变长，下限。默认值：25单位：像素
,int high：:设定检索矩形的最长变长，上限。默认值：220单位：像素
上下限定位的越精确，识别速度越快
mode:普通图像处理模式，mode=0，普通模式；mode=1，图像增强模式（增加了开闭运算）
*************************************************/
void  RecDect::SetInputImage(IplImage* image,int low=25,int high=220,int mode=0){
	img0 = cvCloneImage(image);
	img = cvCloneImage( image);
	if(mode==0)
	recognSquares( img0,findSquares4(img0,low,high) );//正常情况识别图形
	else if(mode==1){
	/*膨胀后腐蚀，消除噪声*/
	IplImage *src=cvCreateImage(cvGetSize(img0),img0->depth,1);//灰度
	cvCvtColor(img0,src,CV_BGR2GRAY);
	ImageStretchByHistogram(src,src);
	cvCvtColor(src,img,CV_GRAY2BGR);		
	cvReleaseImage( &src );
	cvDilate( img, img, 0, 1 );
	cvErode( img, img, 0, 1 );
	recognSquares(img0, findSquares4( img,low,high ));//图像增强后在读取识别图形
	}
	else{
		assert(mode==0||mode==1);
		cerr<<"您输入的模式错误"<<endl;
	}
}

//char* names[] = { 
//	"pic1.png","pic2.png","pic3.png","pic4.png" , 0 };
//

	 // the function draws all the squares in the image

	//int _tmain(int argc, _TCHAR* argv[])
	//{
	//	int i ;
	//	// create memory storage that will contain all the dynamic data

	//
	//	IplImage *src=0, *dst=0; 
	//	for( i = 0; names[i] != 0; i++ )
	//	{
	//		// load i-th image
	//		RecDect *rd=new RecDect();		
	//		rd->img = cvLoadImage( names[i], 1);
	//		if( !rd->img )
	//		{
	//			printf("Couldn't load %s\n", names[i] );
	//			continue;
	//		}
	//		rd->img0 = cvCloneImage( rd->img );
	//		//转化灰度
	//		src=cvCreateImage(cvGetSize(rd->img0),rd->img0->depth,1);//灰度

	//		cvCvtColor(rd->img0,src,CV_BGR2GRAY);
	//		cout<<"picture"<<i<<":"<<endl;
	//		dst = cvCloneImage( src );
	//		cvShowImage("w",dst);
	//		//rd->ImageStretchByHistogram(src,dst);
	//		cvCvtColor(dst,rd->img0,CV_GRAY2BGR);
	//		rd->recognSquares( rd->img0, rd->findSquares4( rd->img) );//正常情况识别图形
	//		//cout<<"正常图像识别完毕，尝试进行图像增强模式识别."<<endl;
	//		//rd->recognSquares( rd->img0, rd->findSquares4( rd->img ) );//图像增强后在读取识别图形
	//		////rd->drawSquares(rd->img0);此函数已经更改
	//			vector<_result>vecResult;//存放矩形屏幕坐标位置和编号数字等，作为类的对外输出接口
 //        rd->GetResult(vecResult);
	//		//// 等待.
	//		 cvWaitKey(0);
	//		//释放图像
	//		cvReleaseImage( &src );
	//		cvReleaseImage( &dst );
	//		// 清除内存
	//		//cvClearMemStorage( storage );
	//		//arr.clear();//每读取一张图需要清空数据
	//		//answer.clear();
	//	
	//	}

	//	return 0;

	//}
