// VideoRead.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "VideoRead.h"
#include "CaptureCard.h"
#include"opencv2\imgproc\imgproc_c.h"
#include<highgui\highgui.hpp>
#include"opencv2/core/core.hpp"
#include"opencv2/core/core_c.h"
#include<vector>
#include"RecDetect.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

CWinApp theApp;
CCaptureCard card;
using namespace std;
using namespace cv;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(NULL);

	if (hModule != NULL)
	{
		// initialize MFC and print and error on failure
		if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
		{
			// TODO: change error code to suit your needs
			_tprintf(_T("Fatal Error: MFC initialization failed\n"));
			nRetCode = 1;
		}
		else
		{
			// TODO: code your application's behavior here.
			card.InitCard();
			card.Capture();
			Sleep(500);
			vector<IplImage*> img;
			while(true)
			{
				vector<_result>vecResult;//存放矩形屏幕坐标位置和编号数字等，作为类的对外输出接口
				card.GetCaptureImage(&img);
				IplImage *src=0; 
				RecDect *rd=new RecDect();		
				rd->SetInputImage(img[0],20,100,0);//普通模式
				rd->SetInputImage(img[0],20,100,1);//图像增强模式
				
				int RecCount=rd->GetResult(vecResult);
				if(RecCount==0)cout<<"该帧没有找到可识别图形"<<endl;
				else cout<<"该帧找到可识别图形"<<RecCount<<"个"<<endl;
				//结果测试
				for(int i=0;i<vecResult.size();i++){
					cout<<"编号："<<vecResult[i].nType<<"值："<<vecResult[i].data<<endl;
				}

				if(VK_ESCAPE == cvWaitKey(25))
				{
					//	cvSaveImage("hard.bmp", rd->img0);
				}
			}
			//cvDestroyAllWindows();
		}
	}

	else
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: GetModuleHandle failed\n"));
		nRetCode = 1;
	}

	return nRetCode;
}
