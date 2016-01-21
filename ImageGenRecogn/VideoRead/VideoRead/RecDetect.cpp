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
����:        ͼ����ǿ
����:     ��Ϊ����ͷͼ���������Ҫ����ֱ��ͼ����ͼ����ǿ��
��ͼ��Ҷȵ���ֵ���쵽0-255    
����˵��:    
src�� ��ͨ���Ҷ�ͼ�� 
dst�����ش�����ͼ��
����ֵ:          ͬ����С�ĵ�ͨ���Ҷ�ͼ�� 
*************************************************/
int RecDect::ImageStretchByHistogram(IplImage *src,IplImage *dst)

{
	//p[]���ͼ������Ҷȼ��ĳ��ָ��ʣ�
	//p1[]��Ÿ����Ҷȼ�֮ǰ�ĸ��ʺͣ�����ֱ��ͼ�任��
	//num[]���ͼ������Ҷȼ����ֵĴ���;

	assert(src->width==dst->width);//��֤һ����
	float p[GrayScale],p1[GrayScale],num[GrayScale];
	//�����������
	memset(p,0,sizeof(p));
	memset(p1,0,sizeof(p1));
	memset(num,0,sizeof(num));

	int height=src->height;
	int width=src->width;
	long wMulh = height * width;

	//����ͼ������Ҷȼ����ֵĴ���
	// to do use openmp
	for(int x=0;x<width;x++)
	{
		for(int y=0;y<height;y++)
		{
			uchar v=((uchar*)(src->imageData + src->widthStep*y))[x];
			num[v]++;
		}
	}

	//����ͼ������Ҷȼ��ĳ��ָ���
	for(int i=0;i<GrayScale;i++)
	{
		p[i]=num[i]/wMulh;
	}

	//���Ÿ����Ҷȼ�֮ǰ�ĸ��ʺ�
	for(int i=0;i<GrayScale;i++)
	{
		for(int k=0;k<=i;k++)
			p1[i]+=p[k];
	}

	//ֱ��ͼ�任
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
����:        ͼ��͸�ӱ任
����:    ��Ϊ������ͼƬ�ᷢ��Ť������ת�ȣ���Ҫ����У��   
����˵��:    
grey�� ��Ҫ�����ͼ�� 
����ֵ:       0
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

	//�趨Դ�۲��棬��ָ���ı��ε��ĸ�����
	//���½�
	src_point[0].x=(float)pt[2].x-minx;             
	src_point[0].y=(float)pt[2].y-miny;
	//���½�
	src_point[1].x=(float)pt[3].x-minx;                    
	src_point[1].y=(float)pt[3].y-miny;
	//���Ͻ�
	src_point[2].x=(float)pt[1].x-minx; 
	src_point[2].y=(float)pt[1].y-miny;
	//���Ͻ�
	src_point[3].x=(float)pt[0].x-minx; 
	src_point[3].y=(float)pt[0].y-miny;


	//�趨Ŀ��۲��棬��ָ���ı��ε��ĸ�����
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
	//���͸��ת������
	//cvWarpPerspectiveQMatrix(src_point,dst_point,&newM);
	cvGetPerspectiveTransform(src_point,dst_point,&newM);
	//͸��ת��
	cvWarpPerspective(Img_old,grey,&newM,CV_INTER_LINEAR+CV_WARP_FILL_OUTLIERS, cvScalarAll(0) );

	//cvNamedWindow("ImgWarp",1);
	//cvShowImage("ImgWarp",grey);
	//cvWaitKey(0);

	cvReleaseImage(&Img_old);
	return 0;
}
/*************************************************
����:  ͼ��OTSU��ֵ�����������ֵ
����:    ͼ������Ŀ���Ǻڰ�ͼ����Ҫ���ú���ķ������ж�ֵ��
����˵��:    
frame�� ��Ҫ����ĵ�ͨ���Ҷ�ͼ�� 
����ֵ:      ��ֵ�ָ��
*************************************************/
int RecDect::otsu(const IplImage *frame) 
{
	//frame�Ҷȼ�
	int pixelCount[GrayScale]={0};
	float pixelPro[GrayScale]={0};
	int width=frame->width;
	int height=frame->height;
	int i, j, pixelSum = width * height, threshold = 0;
	uchar* data = (uchar*)frame->imageData;
	float w0, w1, u0tmp, u1tmp, u0, u1, deltaTmp, deltaMax = 0;
	//ͳ��ÿ���Ҷȼ������صĸ���
	for(i = 0; i < height; i++)
	{
		for(j = 0;j < width;j++)
		{
			pixelCount[(int)data[i * width + j]]++;
		}
	}

	//����ÿ���Ҷȼ���������Ŀռ����ͼ��ı���
	for(i = 0; i < GrayScale; i++)
	{
		pixelPro[i] = (float)pixelCount[i] / pixelSum;
	}

	//�����Ҷȼ�[0,255],Ѱ�Һ��ʵ�threshold

	for(i = 0; i < GrayScale; i++)
	{
		w0 = w1 = u0tmp = u1tmp = u0 = u1 = deltaTmp = 0;
		for(j = 0; j < GrayScale; j++)
		{
			if(j <= i)   //��������
			{
				w0 += pixelPro[j];
				u0tmp += j * pixelPro[j];
			}
			else   //ǰ������
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
����: ����ı��νǶ�
����:    ����������ͼ��һ���Ǿ��Σ���Ҫ���ù����ų�
����˵��:    
pt1��  pt2��pt0����⵽��ͼ���������������
����ֵ:   ����֮��ļнǵ�����
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
����: ����һϵ�м�⵽���ı���
����:    ԭʼͼ����������������ͼ���ı���
����˵��:    
img��ԭʼͼ��
int low:�趨�������ε���̱䳤�����ޡ�Ĭ��ֵ��25��λ������
,int high��:�趨�������ε���䳤�����ޡ�Ĭ��ֵ��220��λ������
�����޶�λ��Խ��ȷ��ʶ���ٶ�Խ��
����ֵ:   һϵ���ı��ζ�������
*************************************************/
CvSeq* RecDect::findSquares4( IplImage* img,int low=25,int high=220 )
{
	CvSeq* contours;
	int i, c, l, N = 1;
	CvSize sz = cvSize( img->width & -2, img->height & -2 );
	IplImage* timg = cvCloneImage( img ); //����
	IplImage* gray = cvCreateImage( sz, 8, 1 ); 
	IplImage* pyr = cvCreateImage( cvSize(sz.width/2, sz.height/2), 8, 3 );
	IplImage* tgray;
	CvSeq* result;
	double s, t;
	// ������Ҫ�����ı��ζ��������
	// ÿ�ĸ�����һ���ı�ͼ��
	CvSeq* squares = cvCreateSeq( 0, sizeof(CvSeq), sizeof(CvPoint), storage );

	// ѡ��ͼ���������
	// ���п�߶�����2
	cvSetImageROI( timg, cvRect( 0, 0, sz.width, sz.height ));

	// ��������
	cvPyrDown( timg, pyr, 7 );
	cvPyrUp( pyr, timg, 7 );
	tgray = cvCreateImage( sz, 8, 1 );

	//Ѱ��ÿһ��ͨ��������
	for( c = 0; c < 1; c++ )
	{
		// ����Cͨ��
		cvSetImageCOI( timg, 2 );
		cvCopy( timg, tgray, 0 );
	//cvCvtColor(timg,tgray,CV_BGR2GRAY);
		// ���Զ�����ֵ
		for( l = 0; l < N; l++ )
		{
			// ����Canny 
			// Canny����gradient shading  ����׽����
			if( l == 0 )
			{
				// Ӧ�� Canny�㷨. ����800
				// ���� 0 
				cvCanny( tgray, gray, 0, 800, 5 );
				// ���ͣ�������֮��Ǳ�ڵĶ�
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
				// ����Ҫ���ĸ���
				// �ܳ���100��850
				//����Ƿ���͹��
				//Ҳ��������������if( result->total == 4 &&
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

					// �Ƕ������������ڣ�������Ƕ���arccos0.3��90��֮��
					if( s < 0.3 )
						for( i = 0; i < 4; i++ ){
							cvSeqPush( squares,
								(CvPoint*)cvGetSeqElem( result, i ));
						}
				}
				//������һ������
				contours = contours->h_next;
			}
		}
	}
	  //  cvShowImage( "Test", timg );
	// �ͷ���ʱͼ��
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
���ܣ����ʶ���������ͼ�ε����ݡ�: ��ͼ�����ʶ������
������vecResult���ͼ�������ŵ�
����ֵ��ʶ�����ͼ�θ�����
arr����Ŵ��ھ������꣨�������ĸ�Ϊһ�����Σ�
answer�����arr��Ӧ���δ���ֵ
vecResult����ž�����Ļ����λ�úͱ��
����˵�������������������vecResult��֮��ʶ���ͼ����������������vector������ֵΪ0��ʾû���ҵ���ʶ��ͼ�Ρ�
*************************************************/
int  RecDect::GetResult( vector<_result>& vecResult )
{
	vecResult.clear();//�������
	for(unsigned int i=0;i<arr.size();i+=4){
		int edge=(int)sqrt((float)(arr[i+1].x-arr[i].x)*(arr[i+1].x-arr[i].x)+(float)(arr[i+1].y-arr[i].y)*(arr[i+1].y-arr[i].y));//�߳�
		CvPoint ct=cvPoint((arr[i].x+arr[i+2].x)/2,(arr[i].y+arr[i+2].y)/2);//ͼ������
		_result temprs;//�����ʱ�������Ϣ
		temprs.nType=i/4;//ʶ��ͼ�α�ţ���0��ʼ
		temprs.nX=(arr[i].x+arr[i+2].x)/2;
		temprs.nY=(arr[i].y+arr[i+2].y)/2;
		temprs.data=answer.at(i/4);
		vecResult.push_back(temprs);
		//����Ϊ��ͼ�����粻��Ҫ��ɾ������
		cvCircle( img0, ct ,edge , CV_RGB(255,255,0),1, 8, 3 );
		CvFont font;
		double hscale = 0.5f;
		double vscale = 0.5f;
		cvInitFont(&font,CV_FONT_HERSHEY_SIMPLEX  ,hscale,vscale,0);
		int data=i/4;
		char *number1=new char[2];
		itoa(data,number1,10);
		string filename="ID:";//ͼ���ʶ��
		filename.append(number1);
		filename.append(",number:");//ʶ����δ���λ��
		char *number=new char[2];
		CRCOP cp;
		int resultData10=cp.chg2To10(result0)>>nbit;//10���ƽ�� 
		data=answer.at(i/4);
		itoa(data,number,10);
		filename.append(number);
		filename.append(".");
		const char * file=filename.c_str();
		CvPoint ct2=cvPoint(ct.x-edge/2,ct.y);//ͼ������-1/2�߳���Ϊ�������λ��
		cvPutText(img0,file,ct2,&font,cvScalar(255,255,0));
		int count = 4;
		CvPoint* rect=new CvPoint[4];
		rect[0]=arr[i];
		rect[1]=arr[i+1];
		rect[2]=arr[i+2];
		rect[3]=arr[i+3];
		cvPolyLine( img0, &rect, &count, 1, 1, CV_RGB(0,255,255), 1, CV_AA, 0 );
		//����Ϊ��ͼ�����粻��Ҫ��ɾ������

	}
	cvShowImage( "������", img0 );
	return arr.size()/4;//����ͼ�����
	//����ֵΪ0��ʾû���ҵ���ʶ��ͼ��

}
/*************************************************
����: ����ȡ��Ϣ����ȡ�������λ
����:  ͨ��01ʶ���ַ������������ʶ��У������ȡ����λ��������ת
����˵��:    
temp��ʶ����ַ���
result0����Ž������λ
����ֵ:   true��ȡ�ɹ�����֮��ʧ��
*************************************************/
bool RecDect::readFromTemp(char *temp,char * result0,int count){//��ʶ�������ȡ�����ֽ�
	int a=0,a1=0;//�ֱ���Ϊ��������ת180�ı�־
	for(int i=8;i<12;i++){//�������
		if (temp[i]=='0')//�����ɫɫդ������
		{
			a++;
		}
	}
	for(int i=12;i<16;i++){//�������
		if (temp[i]=='1')//�����ɫդ������
		{
			a++;
		}
	}
	if (a==8)
	{
		for(int i=0;i<8;i++) result0[i]=temp[i];
		return true;
	}else{//�ֱ���ת90,180,270
		if(temp[0]=='1'&&temp[1]=='0'&&temp[4]=='1'&&temp[5]=='0'&&temp[8]=='1'&&temp[9]=='0'&&temp[12]=='1'&&temp[13]=='0')//��ת90
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
		}else{//�ֱ���ת180,270
			for(int i=0;i<4;i++){//�������
				if (temp[i]=='1')//�����ɫդ������
				{
					a1++;
				}
			}
			for(int i=4;i<8;i++){//�������
				if (temp[i]=='0')//�����ɫդ������
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
			}else{//��ת270
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
				}else{//��ʶ��
				//	cout<<"��ͼ���"<<count<<"�����β�����."<<endl;
					return false;
				}
			}
		}
	}

}

// 
/*************************************************
����: ʶ�������ı���
����:  ͨ��������ȡ����ȡ�����Ϲ����ı��κ���Ҫ����У������ֵ��
ʶ��ͼ����ȡ����λ������У�飬��Ž��
����˵��:    
img��ʶ���ͼ��
squares���Ѿ�������������
*************************************************/
void RecDect::recognSquares( IplImage* img, CvSeq* squares )
{
	CvSeqReader reader;
	IplImage* cpy = cvCloneImage( img );
	//IplImage* cpy1 = cvCloneImage( img );
	int i;
	// ��ʼ��ȡ������
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

		cout<<"�����ڸ�ͼƬ�������ĵ� "<<numbercount<<" �����Σ����ڼ���Ƿ����ǰ���غϣ�"<<endl;
	  		// cvPolyLine( cpy1, &rect, &count, 1, 1, CV_RGB(0,255,0), 3, CV_AA, 0 );
		bool tag1=false;
		float edge=sqrt((float)(pt[1].x-pt[0].x)*(pt[1].x-pt[0].x)+(float)(pt[1].y-pt[0].y)*(pt[1].y-pt[0].y));//�߳�
		for(unsigned  int m=0; m<arr.size(); m+=4)
		{
			int x=arr.at(m).x;
			int y=arr.at(m).y;
			float dis=sqrt((float)(x-pt[0].x)*(x-pt[0].x)+(float)(y-pt[0].y)*(y-pt[0].y));//���Ͻ�����ľ���

			if(5*dis<edge){
				//�����������ͼ�����Ͻ�����ľ���С�ڱ߳�����Ϊ�����غ���һ��ͼ�Ρ�
				cout<<"����ǰ�غϣ��Թ���"<<endl;
				tag1=true;
			}

		}
		if(tag1)//����ͼ���غϣ�����
			continue;
		
		/*
		4 vertices�ṹ��
		P2***P1
		*     *
		P3***P4
		ȷ�����ĸ��Ƕ�֮����Ҫ��ÿһ��ͼ�ν��н�������У������ʶ��
		*/
		cout<<"���غ�"<<endl;

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
		//ͼ�β��ظ������ͼ��任

		IplImage *recImage=cvCreateImage(cvSize(crt.width,crt.height),cpy->depth,cpy->nChannels);
		//cvGetSubRect()
		cvCopy(cpy,recImage);
		WarpPerspective(recImage);//ͼ��͸��У��
		IplImage *recImageG=cvCreateImage(cvSize(recImage->width,recImage->height),recImage->depth,1);
		cvCvtColor(recImage,recImageG,CV_BGR2GRAY);
		cvReleaseImage( &recImage );
		int threshold=otsu(recImageG);//��ֵ��
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
		//01�ַ���ʶ��
		memset(result0,'0',sizeof(char)*8);
		char *temp=new char[16];//�洢����ʶ����
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
					temp[index]='0';//��ɫ��1����ɫ��0
					CvPoint centerpoint=cvPoint(recImageG->nChannels*x+recImageG->width/9,(y+recImageG->height/9));
					cvCircle( recImageG, centerpoint ,3 , CV_RGB(0,0,0),1, 8, 3 );

				}
				else{
					temp[index]='1';//��ɫ��1����ɫ��0
					CvPoint centerpoint=cvPoint(recImageG->nChannels*(x+recImageG->width/9),(y+recImageG->height/9));
					cvCircle( recImageG, centerpoint ,3 , CV_RGB(255,255,255),1, 8, 3 );

				}
				index++;
			}
		}		
		temp[17]='\0';
		//ͨ�������ַ���У��ͼ��֮��λ����֤��λ������
		//�Ѿ�������洢��temp����Ҫ������λͼ������ȡ�ַ�
		//cvShowImage("�����ͼת��",recImageG);
		bool tag2=false;
		if(!readFromTemp(temp,result0,numbercount)){//�Ҳ�����λͼ����ʧ��
			tag2=true;
		}
		if(tag2)continue;
		//cout<<"ʶ������ַ������Ϊ "<<temp<<" ."<<endl;
		result0[8]='\0';
		//cout<<"ʶ��01�ַ������Ϊ "<<result0<<" ."<<endl;
		CRCOP cp;
		//��֤����

		int resultData10=0;
		bool tag3=false;
		if(cp.Validation(result0)==0){
			cout<<"��֤�ɹ�"<<endl;
			resultData10=cp.chg2To10(result0)>>nbit;//10���ƽ�� 	
			cout<<"��"<<numbercount<<"�����ε�ʶ������"<<resultData10<<endl;
			answer.push_back(resultData10);
			//װ��ʵ��ͼ��
			arr.push_back(cvPoint(pt[0].x,pt[0].y));
			arr.push_back(cvPoint(pt[1].x,pt[1].y));
			arr.push_back(cvPoint(pt[2].x,pt[2].y));
			arr.push_back(cvPoint(pt[3].x,pt[3].y));
		}
		else 
		{
			cout<<"��һ��ʶ��ʧ�ܣ�ͼ����ܷ�ת�����Է�תͼ�񡣡�"<<endl;
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
			result0[8]='\0';//ǿ�ƽ���ĩβ
			if(cp.Validation(result0)==0){
				cout<<"��֤�ɹ�"<<endl;
				resultData10=cp.chg2To10(result0)>>nbit;//10���ƽ�� 	
				cout<<"��"<<numbercount<<"�����ε�ʶ������"<<resultData10<<endl;
				//װ��ʵ��ͼ��
				arr.push_back(cvPoint(pt[0].x,pt[0].y));
				arr.push_back(cvPoint(pt[1].x,pt[1].y));
				arr.push_back(cvPoint(pt[2].x,pt[2].y));
				arr.push_back(cvPoint(pt[3].x,pt[3].y));
				answer.push_back(resultData10);
			}
			else
			{
				cout<<"��ת����Ȼ����ʶ�𣬵�"<<numbercount<<"��������֤ʧ�ܣ�������Ҫ�Ľ��."<<endl;
				tag3=false;
			}
		}
		if(tag3)continue;
		//cvNamedWindow("�����ͼת��");
		//cvShowImage("�����ͼת��",recImageG);

		cvReleaseImage(&recImageG);
		cvResetImageROI(cpy);
	}

	cout<<"����ͼƬ���ѵ�"<<squares->total/4<<"�����Σ�����ͼ�����㣬ʵ�ʱ���ͼ��"<<arr.size()/4<<"��"<<endl;
	// ��ʾͼ��
	///cvShowImage("Test",cpy1);
	//	cvReleaseImage( &cpy1 );
	cvReleaseImage( &cpy );
}
/*************************************************
����: ���ͼ����ε�������С��
����:  ���ͼ����ε�������С��
����˵��:    
unsigned int a,unsigned int b,unsigned int c,unsigned int d�������ĸ�����x/y����
����ֵ����С��x/yֵ
*************************************************/
int RecDect::minPoint(unsigned int a,unsigned int b,unsigned int c,unsigned int d){
	unsigned int tem1=(b>a? a:b);
	unsigned int tem2=(c>d? d:c);
	return (tem1>tem2? tem2:tem1);
}
/*************************************************
����: ���ͼ����ε���������
����:  ���ͼ����ε���������
����˵��:    
unsigned int a,unsigned int b,unsigned int c,unsigned int d�������ĸ�����x/y����
����ֵ������x/yֵ
*************************************************/
int RecDect::maxPoint(unsigned int a,unsigned int b,unsigned int c,unsigned int d){
	unsigned int tem1=(b>a? b:a);
	unsigned int tem2=(c>d? c:d);
	return (tem1>tem2? tem1:tem2);
}
/*************************************************
����:  ����ͼ�񲢽���ͼ���ۺϴ���ʶ��
����˵��:    
������
image:��������ͷ��׽����
int low:�趨�������ε���̱䳤�����ޡ�Ĭ��ֵ��25��λ������
,int high��:�趨�������ε���䳤�����ޡ�Ĭ��ֵ��220��λ������
�����޶�λ��Խ��ȷ��ʶ���ٶ�Խ��
mode:��ͨͼ����ģʽ��mode=0����ͨģʽ��mode=1��ͼ����ǿģʽ�������˿������㣩
*************************************************/
void  RecDect::SetInputImage(IplImage* image,int low=25,int high=220,int mode=0){
	img0 = cvCloneImage(image);
	img = cvCloneImage( image);
	if(mode==0)
	recognSquares( img0,findSquares4(img0,low,high) );//�������ʶ��ͼ��
	else if(mode==1){
	/*���ͺ�ʴ����������*/
	IplImage *src=cvCreateImage(cvGetSize(img0),img0->depth,1);//�Ҷ�
	cvCvtColor(img0,src,CV_BGR2GRAY);
	ImageStretchByHistogram(src,src);
	cvCvtColor(src,img,CV_GRAY2BGR);		
	cvReleaseImage( &src );
	cvDilate( img, img, 0, 1 );
	cvErode( img, img, 0, 1 );
	recognSquares(img0, findSquares4( img,low,high ));//ͼ����ǿ���ڶ�ȡʶ��ͼ��
	}
	else{
		assert(mode==0||mode==1);
		cerr<<"�������ģʽ����"<<endl;
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
	//		//ת���Ҷ�
	//		src=cvCreateImage(cvGetSize(rd->img0),rd->img0->depth,1);//�Ҷ�

	//		cvCvtColor(rd->img0,src,CV_BGR2GRAY);
	//		cout<<"picture"<<i<<":"<<endl;
	//		dst = cvCloneImage( src );
	//		cvShowImage("w",dst);
	//		//rd->ImageStretchByHistogram(src,dst);
	//		cvCvtColor(dst,rd->img0,CV_GRAY2BGR);
	//		rd->recognSquares( rd->img0, rd->findSquares4( rd->img) );//�������ʶ��ͼ��
	//		//cout<<"����ͼ��ʶ����ϣ����Խ���ͼ����ǿģʽʶ��."<<endl;
	//		//rd->recognSquares( rd->img0, rd->findSquares4( rd->img ) );//ͼ����ǿ���ڶ�ȡʶ��ͼ��
	//		////rd->drawSquares(rd->img0);�˺����Ѿ�����
	//			vector<_result>vecResult;//��ž�����Ļ����λ�úͱ�����ֵȣ���Ϊ��Ķ�������ӿ�
 //        rd->GetResult(vecResult);
	//		//// �ȴ�.
	//		 cvWaitKey(0);
	//		//�ͷ�ͼ��
	//		cvReleaseImage( &src );
	//		cvReleaseImage( &dst );
	//		// ����ڴ�
	//		//cvClearMemStorage( storage );
	//		//arr.clear();//ÿ��ȡһ��ͼ��Ҫ�������
	//		//answer.clear();
	//	
	//	}

	//	return 0;

	//}
