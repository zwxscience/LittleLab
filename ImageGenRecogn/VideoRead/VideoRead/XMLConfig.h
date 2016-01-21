//CXMLConfig��XML��ʽ�����ļ���дģ��
//Created by Light, HUABO 2010-08-12
//������ͷ�ļ���ģ�顢���
#pragma once

#ifdef XMLCONFIG_EXPORTS
#define XMLCONFIG_API __declspec(dllexport)
#else
#define XMLCONFIG_API __declspec(dllimport)
#ifdef _DEBUG
#pragma comment(lib, "XMLConfigD.lib")
#else
#pragma comment(lib, "XMLConfig.lib")
#endif
#endif

#pragma warning(disable: 4251)
#include <string>
#include <libxml/parser.h>
#include <libxml/xpath.h>

#pragma comment(lib, "libxml2.lib")
#pragma comment(lib, "iconv.lib")
#pragma comment(lib, "odbc32.lib")
#pragma comment(lib, "odbccp32.lib")
using std::string;
//using namespace System;

class XMLCONFIG_API CXMLConfig 
{
public:
	CXMLConfig(void);
	CXMLConfig(const char* szXmlFilename);
	~CXMLConfig(void);

	bool OpenFile(const char* szXmlFilename);
	//XPath��ʽ��/Dir/sub/sub"
	string getXmlString(const char *szXpath);
	//XPath��ʽ��/Dir/sub/sub"
	int getXmlInt(const char* szXpath);	
	//XPath��ʽ��/Dir/sub/sub"
	double getXmlFloat(const char* szXpath);
	//XPath��ʽ��/Dir/sub/sub"
	string GetAttrString(const char* szXpath, const char* szAttrName);
	//XPath��ʽ��/Dir/sub/sub"
	int GetAttrInt(const char* szXpath, const char* szAttrName);
	//XPath��ʽ��/Dir/sub/sub"
	double GetAttrFloat(const char* szXpath, const char* szAttrName);
	
	//XPath��ʽ��/Dir/sub/sub"
	string getXmlString(const char *szXpath, int nIndex);
	//XPath��ʽ��/Dir/sub/sub"
	int getXmlInt(const char* szXpath, int nIndex);	
	//XPath��ʽ��/Dir/sub/sub"
	double getXmlFloat(const char* szXpath, int nIndex);
	//XPath��ʽ��/Dir/sub/sub"
	string GetAttrString(const char* szXpath, const char* szAttrName, int nIndex);
	//XPath��ʽ��/Dir/sub/sub"
	int GetAttrInt(const char* szXpath, const char* szAttrName, int nIndex);
	//XPath��ʽ��/Dir/sub/sub"
	double GetAttrFloat(const char* szXpath, const char* szAttrName, int nIndex);

	int GetNodeCount(const char* szXpath);

	//XPath��ʽ��/Dir/sub/sub"
	bool writeXmlString(const string strValue, const char* szXpath);
	//XPath��ʽ��/Dir/sub/sub"
	bool writeXmlInt(const int iValue, const char* szXpath);
	//XPath��ʽ��/Dir/sub/sub"
	bool writeXmlFloat(const double dValue, const char* szXpath);
	//XPath��ʽ��/Dir/sub/sub"
	bool writeAttrString(const string strValue, const char* szAttrName, const char* szXpath);
	//XPath��ʽ��/Dir/sub/sub"
	bool writeAttrInt(const int iValue, const char* szAttrName, const char* szXpath);
	//XPath��ʽ��/Dir/sub/sub"
	bool writeAttrFloat(const double dValue, const char* szAttrName, const char* szXpath);

	bool AddXmlString(const string strValue, const char* szParentXpath, const char* szSubXpath);
	bool AddXmlInt(const int nValue, const char* szParentXpath, const char* szSubXpath);
	bool AddXmlFloat(const float fValue, const char* szParentXpath, const char* szSubXpath);

	bool AddAttrString(const string strValue, const char* szParentXpath, const char* szSubXpath, const char* szAttrName);
	bool AddAttrInt(const int nValue, const char* szParentXpath, const char* szSubXpath, const char* szAttrName);
	bool AddAttrFloat(const float fValue, const char* szParentXpath, const char* szSubXpath, const char* szAttrName);

	bool RemoveXmlNode(const char* szXpathNode);
	bool RemoveAttr(const char* szXpathNode, const char* szAttrName);

	bool saveConfigFile();
	bool saveasConfigFile(const char* szAttrName);
	bool saveBakConfigFile();
	bool loadBakConfigFile();

private:
	//����ת��:��һ�ֱ���תΪ��һ�ֱ���   
	int code_convert(char* from_charset, char* to_charset, char* inbuf,
		int inlen, char* outbuf, int outlen);

	//UNICODE��תΪGB2312��   
	char* u2g(char *inbuf);

	//GB2312��תΪUNICODE��   
	char* g2u(char *inbuf);

	//����xpath��ѯ�ڵ㼯�ϣ��ɹ��򷵻�xpath�Ķ���ָ�룬ʧ�ܷ���NULL
	xmlXPathObjectPtr get_nodeset(const xmlChar *xpath);

	xmlNodePtr CheckNode(const char* szXpath);
	xmlNodeSetPtr CheckMultiNode(const char* szXpath);

	xmlNodePtr CreateNewNode(const char* szXpath);

	// ��ֹ�������캯����"="����
	CXMLConfig(const CXMLConfig&);
	CXMLConfig& operator=(const CXMLConfig&);
private:
	string m_strFilename;
	xmlDocPtr m_doc;
	const int NUM_OF_DIGITS;
};
