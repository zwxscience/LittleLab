//������ͷ��Ƶ��׽��
//���ƴ�ӻ��棬��������ת��ƴ���ص����
//Created by Light, HUABO 2011-03-10

//���������ڳ�������·���£��ļ���:CardConfig.xml
//�����Ը�����Ҫ�޸��ļ���Ĳ���
//��DLL����OpenCV2.2
//��ӿ�Ϊ:
//bool InitCard(void);     ���ʼ�����������������ļ���Ĳ�����ʼ���ɼ���
//BOOL Capture(void);     ��׽����/�رպ���������ʹ�ñ������淵��ֵ�����жϲ�׽״̬��TRUEΪ������׽��FALSEΪ�رղ�׽
//void GetCaptureImage(vector<IplImage*>* captureArray);    �ڳ�����ѭ�����ô˺����ɻ��ͼ�����飬�û�ʹ�ú���Ҫ�ͷŸ�����;
//void UpdateSetting(void);  �����κ�״̬�¸��²ɼ�������ע����ɼ��߳�ͬ����

#pragma once

#ifdef CAPTURECARD_EXPORTS
#define CAPTURECARD_API __declspec(dllexport)
#else
#define CAPTURECARD_API __declspec(dllimport)
#ifdef _DEBUG
#pragma comment(lib, "CaptureCardD.lib")
#else
#pragma comment(lib, "CaptureCard.lib")
#endif
#endif

#include "CGVideo.h"
#include "CGVidex.h"
#include  <algorithm>
#include "cvconfig.h"
#include "core\core_c.h"
#include "imgproc\imgproc_c.h"
#include "highgui\highgui_c.h"
#include <vector>
#include "XMLConfig.h"


#pragma comment(lib, "CGVideo.lib")
#pragma comment(lib, "CGVidex.lib")
#ifdef _DEBUG
#pragma comment(lib, "opencv_core220d.lib")
#pragma comment(lib, "opencv_imgproc220d.lib")
#pragma comment(lib, "opencv_highgui220d.lib")
#else
#pragma comment(lib, "opencv_core220.lib")
#pragma comment(lib, "opencv_imgproc220.lib")
#pragma comment(lib, "opencv_highgui220.lib")
#endif



using namespace std;
//using namespace cv;

//#pragma comment(lib, "cxcore.lib")
//#pragma comment(lib, "cv.lib")
//#pragma comment(lib, "highgui.lib")

typedef struct tagDISP_DESC {
	HCG hcg;

	VIDEO_SCAN VideoScan;
	VIDEO_FORMAT VideoFormat;
	VIDEO_STANDARD VideoStandard;
	VIDEO_SOURCE VideoSource;

	CRY_OSC CryOsc;
	CRITICAL_SECTION* cs;

	BYTE byHue;
	BYTE byBrightness;
	BYTE byContrast;
	BYTE bySaturation;
	BYTE byPLLFrequency;

	int nMaxWidth;
	int nMaxHeight;
	int nCardSerial;

	CRect rcInputWindow;
	CRect rcOutputWindow;

	CRect rcClientWindow;

	BITMAPINFO *pBmpInfo;  
	BYTE *pImageBuffer;
	char chBmpBuf[2048];

	BOOL bHorizon;
	BOOL bVertical;

	BOOL bChroma;

	BOOL bInterline;

	DWORD dwOffset;
} DISP_DESC;

typedef enum tagRotateType {
	CW, CCW, _180, NOT_ROTATE
} ROTATE_TYPE;

typedef struct tagCaptureParam{
	int width, height, brightness, contrast, hue, saturation, left, right, top, bottom;
	ROTATE_TYPE rt;
}CaptureParam;

typedef enum tagAssemblyType {

	_1X1		= 0,
	_1X2 		= 1,
	_2X1		= 2,
	_1X3		= 3,
	_3X1		= 4,
	_1X4		= 5,
	_4X1		= 6,
	_2X2		= 7
} ASSEMBLY_TYPE;

enum CardStatus{OPD_NONE,OPD_SNAP_EX = 3, CARD_MAX_NUMBER = 4};
#define VideoFormatToChannel(i)		(VideoFormatToRgb(m_DispDesc[(i)].VideoFormat) + 7) / 8

class CAPTURECARD_API CCaptureCard {
public:
	CCaptureCard(void);
	~CCaptureCard(void);

	bool InitCard(void);
	BOOL Capture(void);
	CardStatus QueryCardStatus(){return m_nOperation;}
	void UpdateSetting(void);

	void GetCaptureImage(vector<IplImage*>* captureArray);
private:
	static int CALLBACK SnapThreadCallbackEx(SNAP_INFO *pInfo);	
	void SetDispDesc(void);
	void RotateImage(IplImage* srcImage, ROTATE_TYPE rt);
	void CropImage(IplImage* srcImage, CvRect rect);
	DISP_DESC m_DispDesc[CARD_MAX_NUMBER];
	CardStatus m_nOperation;
};

int VideoFormatToRgb(VIDEO_FORMAT format);
//extern CAPTURECARD_API int nCaptureCard;

//CAPTURECARD_API int fnCaptureCard(void);
class CParam : public CXMLConfig
{
public:
	CParam(void){};
	~CParam(void){};
	void ReadParam(void);

	int m_nNumOfCapture;
	CRY_OSC m_nCryOsc;
	int m_nVideoFormat;
	int m_nVideoSource;
	int m_nSourceIndex;
	int m_nAssemlyType;

	vector<CaptureParam> m_vecCaptureParam;
};