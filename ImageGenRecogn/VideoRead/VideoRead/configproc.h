//�����ļ���д��
//Created by Chun, HUABO 2009-02-06
//���������ʱ����ʼ���ļ�������û�������ļ���������Ĭ��ֵ���������ļ�
#pragma once

#ifdef CONFIGPROC_EXPORTS
#define CONFIGPROC_API __declspec(dllexport)
#else
#define CONFIGPROC_API __declspec(dllimport)
#pragma comment(lib, "ConfigProc.lib")
#endif

// �����Ǵ� ConfigProc.dll ������
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