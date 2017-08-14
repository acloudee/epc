#ifndef __Property_h__
#define __Property_h__

#include "StringMap.h"
#include "StringBuffer.h"
 
PROJECT_NS_BEGIN

/************************************************************************************
 * syntax:
 * 1. anything  after double slash, "//", are ignored 'till end of line, '\n'.
 * 2. each line forms a property
 * 3. key and value separator is any of the following characters encounterd:
 *	[':' | '\t' | '=' | or ' ']
 * 4. anything after the 1st key-value separator encountered is the perperty value on this line
 * e.g.:
 *	// this is a comment
 *  TNCE: Telephony Network Communication Equipments
 *  FCI = First Communication Inc. // this is inline comment
 *  MS	Microsoft
 *	www.tnce.com 210.59.188.1
 ***********************************************************************************/

class CProperty : public CString2StringMap
{
public:
	static PCSTR s_cpszSeparator;
	static bool _ParseBoolean(PCSTR cpszValue, bool bDefault=false);
	// case-insensitive [true|false|yes|no|0|non-zero-int |enable(d)|disable(d) | on | off]
	// return bDefault if either cpszPropName cannot be found or the value is not valid integer(empty)
	// note: [-][0b | 0B | 0x | 0X][0-9a-ZA-Z] are valid integer
	static int _ParseInteger(PCSTR cpszValue, int nDefault=0);
	// return nDefault if cpszPropName cannot be found or the valie is not a parsable integer(empty)
	// note: [-][0b | 0B | 0x | 0X][0-9a-ZA-Z] are valid integer

public:
	CProperty(bool bPropCaseSensitive=true, PCSTR cpszComment="//");
	// bPropCaseSensitive : whether the property name is case sensitive or not on search
	//			defaulut is case-sensitive, and all propertye name will be kept in lower case
	//		thus, we strongly suggest the following bPropCaseSensitive useage only:
	//			constructor,LoadProperties/ReadProperties = (true, true)/(true, false)/(false, false)
	//		; be very casefully when you try to use 	constructor,LoadProperties/ReadProperties = (false, true)
	// cpszComment	 comment string such as "//" or '#'
	CProperty(PCSTR cpszProperties, bool bFromFile=true, bool bPropCaseSensitive=true, PCSTR cpszComment="//");
	// bPropCaseSensitive : whether the property name is case sensitive or not on search
	//			defaulut is case-sensitive, and all propertye name will be kept in lower case
	//		thus, we strongly suggest the following bPropCaseSensitive useage only:
	//			constructor,LoadProperties/ReadProperties = (true, true)/(true, false)/(false, false)
	//		; be very casefully when you try to use 	constructor,LoadProperties/ReadProperties = (false, true)
	// cpszComment	 comment string such as "//" or '#'
	virtual ~CProperty();
	int GetInteger(PCSTR cpszPropName, int nDefault=0); 
	// return nDefault if cpszPropName cannot be found or not parsable integer
	// note: [-][0b | 0B | 0x | 0X][0-9a-ZA-Z] are valid integer
	int GetInteger(PCSTR cpszPropName, int nDefault, int nMin, int nMax); 
	// return nDefault if cpszPropName cannot be found or the valie is not a parsable integer(empty)
	// note: [-][0b | 0B | 0x | 0X][0-9a-ZA-Z] are valid integer
	void SetInteger(PCSTR cpszPropName, int nValue);
	bool GetBoolean(PCSTR cpszPropName, bool bDefault=false);
	// case-insensitive 
	//	true:  non-zeor-int | true | yes | enable(d) | on | up
	//	false: 0 | false | no | disable(d) | off | down | 
	// return bDefault if either cpszPropName cannot be found or the value is not valid integer(empty)
	// note: [-][0b | 0B | 0x | 0X][0-9a-ZA-Z] are valid integer
	void SetBoolean(PCSTR cpszPropName, bool bValue);
	String GetString(PCSTR cpszPropName, PCSTR cpszDefault=NULL);
	// return cpszDefault only if cpszPropName cannot be found
	void SetString(PCSTR cpszPropName, PCSTR cpszValue);

	int LoadProperties(PCSTR cpszFilename, bool bPropCaseSensitive=true);
	// bPropCaseSensitive : whether the read property name should be stored in case sensitive way or not
	//			defaulut is case-sensitive, and those read propertye name will be kept in lower case.
	//		besides, the bPropCaseSensitive is a one time flag, which would not keep in member variable
	//		the member variable could only be passed by consuctor inherently.
	//		thus, we strongly suggest the following bPropCaseSensitive useage only:
	//			constructor,LoadProperties/ReadProperties = (true, true)/(true, false)/(false, false)
	//		; be very casefully when you try to use 	constructor,LoadProperties/ReadProperties = (false, true)
	// return # of properties read, -1 for error reading from file
	// Note: this is an additive operation, which will not clear up the original properties 
	//			before loading perperties from file. 
	//		If you want to clear all properties first, call the inherited Clear() function explicitly.
	int ReadProperties(PCSTR cpszInMemoryProperties, bool bPropCaseSensitive=true);
	// bPropCaseSensitive : whether the read property name should be stored in case sensitive way or not
	//			defaulut is case-sensitive, and those read propertye name will be kept in lower case.
	//		besides, the bPropCaseSensitive is a one time flag, which would not keep in member variable
	//		the member variable could only be passed by consuctor inherently.
	//		thus, we strongly suggest the following bPropCaseSensitive useage only:
	//			constructor,LoadProperties/ReadProperties = (true, true)/(true, false)/(false, false)
	//		; be very casefully when you try to use 	constructor,LoadProperties/ReadProperties = (false, true)
	// return # of properties read
	// cpszInMemoryProperties = "key1 value1\nkey2 value2\n";
	// Note: this is an additive operation, which will not clear up the original properties 
	//			before reading perperties from memory. 
	//		If you want to clear all properties first, call the inherited Clear() function explicitly.
	PCSTR Dump(CStringBuffer& sb);

protected:
	String MakePropertyName(PCSTR cpszPropName);
	bool Parse(String& cpszLine, bool bPropCaseSensitive=true);
	bool m_bPropCaseSensitive;	// whether the property name is case sensitive or not on search
	String m_strComment;
};

PROJECT_NS_END

#endif // __Property_h__
