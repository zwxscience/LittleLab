// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� XMLLIB_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// XMLLIB_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#pragma once

#ifdef XMLLIB_EXPORTS
#define XMLLIB_API __declspec(dllexport)
#else
#define XMLLIB_API __declspec(dllimport)
#pragma comment(lib, "XMLLib.lib")
#endif

#include <objbase.h>
#include <msxml2.h>

#pragma comment(lib, "comsuppw.lib")

#define SAFE_DELETE(p)     {delete (p);     (p)=NULL;}
#define SAFE_DELETE_ARRAY(p)    {delete[] (p);     (p)=NULL;}
// �����Ǵ� XMLLib.dll ������
class XMLLIB_API CXMLLib 
{
public:
	CXMLLib(void);
	~CXMLLib(void);

	bool Create(int others);
	void Destroy();
	bool Load(BSTR file);
	bool Save(BSTR file);
	DWORD Read(DWORD index);
	bool Append();
	void Begin(DWORD index);
	DWORD Update();

	int A1,A2,A3;
	wchar_t * szA4;
	int iOthers;
	DWORD m_AllNums;
	DWORD m_CurNum;
	int m_nSample;
	bool m_bIsCreated;

protected:
	bool AppendChildToParent(IXMLDOMNode *pChild, IXMLDOMNode *pParent);
	void AddWhiteSpaceToNode(IXMLDOMDocument* pDom,BSTR bstrWs,IXMLDOMNode *pNode);
	VARIANT VariantString(BSTR str);
	void ShowError(TCHAR * title);
	void GetTime1();
	void GetTime2();
	void SetSample();
	
	IXMLDOMDocument *pXMLDom;
	IXMLDOMElement *pRoot;
	BSTR bstr_wsn;
	BSTR bstr_wsnt;
	BSTR bstr_wsntt;
	DWORD st0,st1,st2;
	DWORD minutes,seconds,milliseconds;
	wchar_t szTime[10];//xx:xx:xxx
	wchar_t szTemp[11];
};