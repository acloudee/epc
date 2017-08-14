#ifndef __URI_h__
#define __URI_h__

#include "Strings.h"
#include "StringBuffer.h"

PROJECT_NS_BEGIN

class ParamList
{
public:
	typedef struct _NAME_VALUE_PAIR {
		String strName;
		String strValue;
		_NAME_VALUE_PAIR* pNext;
	} NAME_VALUE_PAIR;

public:
	ParamList();
	ParamList(ParamList& paramLst);
	ParamList(PCSTR cpszNameValueList, char chParamSeparator=';');
	ParamList(PCSTR cpszName, PCSTR cpszValue);
	virtual ~ParamList();

	int Parse(PCSTR cpszNameValueList, char chParamSeparator=';');
	// chParamSeparator	'&' or ';'
	// reutrn # of name=value pairs.

	// parameter
	bool HasParam(PCSTR cpszName=NULL, bool bCaseSensitive=true);
	// cpszName	Pass NULL to query for any parameter
	String GetParam(PCSTR cpszName, bool bCaseSensitive=true);
	void SetParam(PCSTR cpszName, PCSTR cpszValue, bool bCaseSensitive=true);
	// override existed parametere; otherwise add a new one
	// bCaseSensitive	whether cpszName is case sensitive or not
	void Clear();
	void Remove(PCSTR cpszName, bool bCaseSensitive=true);

	bool Encode(CStringBuffer& sb, char chParamSeparator);
	String ToString(char szParamSeparator);

	// iterator
	POSITION GetHeadPosition();
	NAME_VALUE_PAIR* GetNextPos(POSITION& pos);

	// operator
	ParamList& operator=(ParamList& paramList);
	ParamList& operator+=(ParamList& paramList);

protected:
	NAME_VALUE_PAIR* FindParam(PCSTR cpszName, bool bCaseSensitive);

	NAME_VALUE_PAIR* m_pParamPair; // point to uri parameters.
};

class URI 
{
public:
	URI() { }
	//cpszURI:IN param, follow 
	/*
	3104<sip:3104@sh.fci.com>
	"3104"<sip:3104@sh.fci.com>
	3104<sip:3104@sh.fci.com:5060>
	sip:3104@sh.fci.com
	sip:3104@sh.fci.com:5060
	sip:192.168.3.104
	sip:192.168.3.104:5060
	*/
	
	URI(PCSTR cpszURI, char chParamSeparator=';') { Parse(cpszURI, chParamSeparator);}
	virtual ~URI() {}
	String ToString(bool bSchema, PCSTR cpszSchema="sip"); // add schema or not
	String GetAddr(bool bSchema=false);
	// bSchema	Specify to put schema or not
	// return E-Mail like address without protocol field, such as "xyz@isp.com:5060".
	void Parse(PCSTR cpszURI, char chParamSeparator=';'); // '&' or ';'

	inline bool HasDisplay() { return !m_szDisplay.IsEmpty(); }
	inline bool HasSchema() { return !m_szSchema.IsEmpty(); }
	inline bool HasUserPart() { return !m_szUserPart.IsEmpty(); }
	inline bool HasDomain() { return !m_szDomain.IsEmpty(); }
	inline bool HasPort() { return !m_szPort.IsEmpty(); }	

	String m_szDisplay;
	String m_szSchema;
	String m_szUserPart;
	String m_szDomain;
	String m_szPort;

	ParamList m_paramList;

private:
	void Clear();
};


PROJECT_NS_END

#endif
