//CXMLConfig，XML格式配置文件读写模块
//Created by Light, HUABO 2010-08-12
//依赖的头文件、模块、组件
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
	//XPath格式“/Dir/sub/sub"
	string getXmlString(const char *szXpath);
	//XPath格式“/Dir/sub/sub"
	int getXmlInt(const char* szXpath);	
	//XPath格式“/Dir/sub/sub"
	double getXmlFloat(const char* szXpath);
	//XPath格式“/Dir/sub/sub"
	string GetAttrString(const char* szXpath, const char* szAttrName);
	//XPath格式“/Dir/sub/sub"
	int GetAttrInt(const char* szXpath, const char* szAttrName);
	//XPath格式“/Dir/sub/sub"
	double GetAttrFloat(const char* szXpath, const char* szAttrName);
	
	//XPath格式“/Dir/sub/sub"
	string getXmlString(const char *szXpath, int nIndex);
	//XPath格式“/Dir/sub/sub"
	int getXmlInt(const char* szXpath, int nIndex);	
	//XPath格式“/Dir/sub/sub"
	double getXmlFloat(const char* szXpath, int nIndex);
	//XPath格式“/Dir/sub/sub"
	string GetAttrString(const char* szXpath, const char* szAttrName, int nIndex);
	//XPath格式“/Dir/sub/sub"
	int GetAttrInt(const char* szXpath, const char* szAttrName, int nIndex);
	//XPath格式“/Dir/sub/sub"
	double GetAttrFloat(const char* szXpath, const char* szAttrName, int nIndex);

	int GetNodeCount(const char* szXpath);

	//XPath格式“/Dir/sub/sub"
	bool writeXmlString(const string strValue, const char* szXpath);
	//XPath格式“/Dir/sub/sub"
	bool writeXmlInt(const int iValue, const char* szXpath);
	//XPath格式“/Dir/sub/sub"
	bool writeXmlFloat(const double dValue, const char* szXpath);
	//XPath格式“/Dir/sub/sub"
	bool writeAttrString(const string strValue, const char* szAttrName, const char* szXpath);
	//XPath格式“/Dir/sub/sub"
	bool writeAttrInt(const int iValue, const char* szAttrName, const char* szXpath);
	//XPath格式“/Dir/sub/sub"
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
	//代码转换:从一种编码转为另一种编码   
	int code_convert(char* from_charset, char* to_charset, char* inbuf,
		int inlen, char* outbuf, int outlen);

	//UNICODE码转为GB2312码   
	char* u2g(char *inbuf);

	//GB2312码转为UNICODE码   
	char* g2u(char *inbuf);

	//调用xpath查询节点集合，成功则返回xpath的对象指针，失败返回NULL
	xmlXPathObjectPtr get_nodeset(const xmlChar *xpath);

	xmlNodePtr CheckNode(const char* szXpath);
	xmlNodeSetPtr CheckMultiNode(const char* szXpath);

	xmlNodePtr CreateNewNode(const char* szXpath);

	// 禁止拷贝构造函数和"="操作
	CXMLConfig(const CXMLConfig&);
	CXMLConfig& operator=(const CXMLConfig&);
private:
	string m_strFilename;
	xmlDocPtr m_doc;
	const int NUM_OF_DIGITS;
};
