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
				vector<_result>vecResult;//��ž�����Ļ����λ�úͱ�����ֵȣ���Ϊ��Ķ�������ӿ�
				card.GetCaptureImage(&img);
				IplImage *src=0; 
				RecDect *rd=new RecDect();		
				rd->SetInputImage(img[0],20,100,0);//��ͨģʽ
				rd->SetInputImage(img[0],20,100,1);//ͼ����ǿģʽ
				
				int RecCount=rd->GetResult(vecResult);
				if(RecCount==0)cout<<"��֡û���ҵ���ʶ��ͼ��"<<endl;
				else cout<<"��֡�ҵ���ʶ��ͼ��"<<RecCount<<"��"<<endl;
				//�������
				for(int i=0;i<vecResult.size();i++){
					cout<<"��ţ�"<<vecResult[i].nType<<"ֵ��"<<vecResult[i].data<<endl;
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
