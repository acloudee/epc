#ifndef __URLCODEC_H__
#define __URLCODEC_H__

#include "StringBuffer.h"

PROJECT_NS_BEGIN

class  CURLCodec {
 
public:
	CURLCodec();
	virtual ~CURLCodec();
	static void Encode(const char* cpszSrc, CStringBuffer& sbDest, bool bEncodeSlash=true);
	static int Encode(const char* cpszSrc, char* pszDest, int nSize, bool bEncodeSlash=true);
	// 1. The ASCII characters 'a' through 'z', 'A' through 'Z', 
	//		'0' through '9', 	and ".", "-", "*", "_" remain the same. 
	//	2. The space character ' ' is converted into a plus sign '+'. 
	//	3. All other characters are converted into the 3-character string "%xy", 
	//		where xy is the two-digit hexadecimal representation of the lower 
	//		8-bits of the character. 	
	// 4. bEncodeSlash: Normally you should encode '/' also, 
	//		in case of a full URL, you may set bEncodeSlash to false
	//		to avoid encode path separator, '/'.

	static void Decode(const char* cpszSrc, CStringBuffer& sbDest);
	static int Decode(const char* cpszSrc, char* pszDest, int nSize);
	//	1. The ASCII characters 'a' through 'z', 'A' through 'Z', 
	//		and '0' through '9' remain the same. 
	//	2.	The plus sign '+' is converted into a space character ' '. 
	// 3.	The remaining characters are represented by 3-character strings 
	//		which begin with the percent sign, "%xy", where xy is the two-digit 
	//		hexadecimal representation of the lower 8-bits of the character. 	

private:
	static char* ToHex(const char chSrc, char* pszDest);
	static char DeHex(const char* cpszSrc);

	static const bool* s_bDontNeedEncodings;
	static bool s_bChars[256];	
	static bool* SetExemptChars();
	static const char* s_cpszHex;
	static const char* s_cpszDeHex;
	static char s_chDeHex[256];
	static char* SetDeHex();
};

PROJECT_NS_END

#endif		// __URLCODEC_H__
