//配置文件读写类
//Created by Chun, HUABO 2009-02-06
//定义类对象时，初始化文件名。如没有配置文件，则依据默认值生成配置文件
#pragma once

#ifdef CONFIGPROC_EXPORTS
#define CONFIGPROC_API __declspec(dllexport)
#else
#define CONFIGPROC_API __declspec(dllimport)
#pragma comment(lib, "ConfigProc.lib")
#endif

// 此类是从 ConfigProc.dll 导出的
class CONFIGPROC_API CConfigProc 
{
public:
	CConfigProc(TCHAR * fileName);
	~CConfigProc(void);

	BOOL WriteInt(LPCTSTR lpSection,LPCTSTR lpKeyName,int value);
	BOOL WriteFloat(LPCTSTR lpSection,LPCTSTR lpKeyName,float value);
	BOOL WriteString(LPCTSTR lpSection,LPCTSTR lpKeyName,LPCTSTR lpString);

	bool GetInt(LPCTSTR lpSection,LPCTSTR lpKeyName,int & value);
	bool GetFloat(LPCTSTR lpSection,LPCTSTR lpKeyName,float & value);
	bool GetString(LPCTSTR lpSection,LPCTSTR lpKeyName,LPTSTR lpReturnedString,DWORD nSize);
	bool m_bNewFile;

private:
	HANDLE m_hFile;
	TCHAR m_FileName[256];
};