#ifndef __UFT8_H__
#define __UFT8_H__

#include "Common.h"

PROJECT_NS_BEGIN

class CUTF8  
{
public:
	virtual ~CUTF8();
	
	/**
	* @brief Decode a UTF8 string to unicode string
	* @param szUTF8 A UTF8 string to be decoded.
	* @param pbUnicode Unicode buffer
	* @param nSize  Size of unicode buffer.
	* @return Number of Unicode.
	*/
	static int _Decode(PCSTR cpszUTF8, PDWORD pbUnicode, int &nSize);

	/**
	* @brief Encode an Unicode
	* @param dwUnicode Unicode number to be encoded.
	* @param szUTF8 A UTF8 buffer.	
	* @param nSize  Size of UTF8 buffer.
	* @return Number of bytes in UTF8 buffer
	*/
	static int _Encode(DWORD dwUnicode, char *pszUTF8, int nSize);

	/**
	* @brief Encode an Unicode string with null ending to UTF8
	* @param pdwUnicode Unicode array to be encoded.
	* @param szUTF8 A UTF8 buffer.	
	* @param nSize  Size of UTF8 buffer.
	* @return Number of bytes in UTF8 buffer
	*/
	static int _Encode(PDWORD pdwUnicode, char *szUTF8, int nSize);
    
private:
	CUTF8();

	static DWORD _DecodeFirstUnicode(char *&szSource);
};

PROJECT_NS_END

#endif


