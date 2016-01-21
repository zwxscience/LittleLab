// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 XMLLIB_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// XMLLIB_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
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
// 此类是从 XMLLib.dll 导出的
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