#ifndef __Integer_h__
#define __Integer_h__

#include "Strings.h" 

PROJECT_NS_BEGIN

class CInteger 
{
public: 
	static const int s_nMinValue;	// = 0x80000000;
	static const int s_nMaxValue;	// 0x7fffffff;
	static u_long _ParseULong(PCSTR cpszValue, int nRadix=10); //
	//			It will pase in a best effort fashion. That is, it parses untill the first
	//			non-digit, and returns the parsing result so far. 
	//			E.g. _ParseULong("500]", 10) would return 500
	static String _ToULongString(u_long ulValue, int nRadix);

	static bool _IsNumber(PCSTR cpszValue, int nRadix=0); 
	// cpszValue	[-][0b | 0B | 0x | 0X][0-9a-ZA-Z] 
	//				integer value to parse, no space is allowed, case-insensitive,
	//				the optional minus sign and [0x | 0X | 0b | 0B] could prefix the numerical value.
	//				where [0b | 0B] denotes binary value, and [0x | 0X] denotes hexidecimal.
	//				The munus sign, if present, must be the first character.
	// nRadix	The base which cpszValue presents, must be within [2-36]
	//			pass 0 for the function to determine the radix based on the prefix [0b | 0B | 0x | 0X]
	//			if nRadix is zero and no prefix presents, it will treat cpszValue as decimal.

	static int _ParseInt(PCSTR cpszValue, int nRadix=10); 
	// cpszValue	[-][0-9a-ZA-Z] 
	//				integer value to parse, no space is allowed, case-insensitive,
	//				only the optional minus sign could prefix the numerical value.
	// nRadix 2-36 [0-9a-zA-Z],
	// return	0 on error, to determine if it is a valid number, use _IsNumber()
	//			It will pase in a best effort fashion. That is, it parses untill the first
	//			non-digit, and returns the parsing result so far. 
	//			E.g. _ParseInt("500]", 10) would return 500
	static int _DecodeInt(PCSTR cpszValue); // [-]0b in binary, [-]0x in hex, [-] decimal, case-insensitive,no space is allowed
	// cpszValue	[-][0b | 0B | 0x | 0X][0-9a-ZA-Z] 
	//				integer value to parse, no space is allowed, case-insensitive,
	//				the optional minus sign and [0x | 0X | 0b | 0B] could prefix the numerical value.
	//				where [0b | 0B] denotes binary value, and [0x | 0X] denotes hexidecimal.
	//				The munus sign, if present, must be the first character.
	// return	0 on error; to determine if it is a valid number, use _IsNumber()
	//			It will pase in a best effort fashion. That is, it parses untill the first
	//			non-digit, and returns the parsing result so far. 
	//			E.g. _DecodeInt("3-500") would return 3
	static String _ToString(int nValue, int nRadix=10);  
	// no leading symbol for radix,
	// it will return value in [0-9a-z] based on nRadix 

	static int _DecodeHex(PCSTR cpszNumber, PBYTE pBuf, int nBufLen);
	// cpszNumber	number in string format
	//	   			[0x | 0X][0-9a-fA-F] 
	//				integer value to parse, no space is allowed, case-insensitive,
	// pBuf			pointer to user supplied buffer storing decodec number
	// nBufLen		length of buffer in bytes pointed to by pBuf.
	// return		# of cpszNumber decoded, such as 4
	//				0 when cpszNumber is NULL or emtpy string, or pBuf is NULL or nBufLen is 0
	//				if any digits in cpszNumber is not valid hex digits, it stop decode and 
	//				return what has been decoded.
	//				Note, if cpszNumber is not a string of even size, a prefix 0x0 will be
	//				written into pBuf on the MSB. For example, if "0x12345" is passed as cpszNumber,
	//				the 0x012345 will be written int pBuf and 5 will be returned.

public:
	CInteger() { m_nValue = 0; }
	CInteger(PCSTR cpszValue, int nRadix=10) { m_nValue = _ParseInt(cpszValue, nRadix); }
	CInteger(int nValue) { m_nValue = nValue; }
	virtual ~CInteger(){}

	inline String ToString(int nRadix=10) { return _ToString(m_nValue, nRadix); } // no leading symbol for radix
	inline CInteger& operator=(int nValue) { SetValue(nValue); return *this; }
	inline bool operator==(int nValue) { return m_nValue == nValue; }
	inline operator int() { return GetValue(); }
	inline operator long() { return GetValue(); }
	inline int GetValue() { return m_nValue; }
	inline void SetValue(int nValue) { m_nValue = nValue; }	
protected:
	static const char s_cpszDigits[];	
	static const char s_cpszRadixTenTenths[];
	static const char s_cpszRadixTenUnits[];
	static int _ToDigit(char ch, int nRadix);
	static String _ToDecimalString(int nValue);  // no leading symbol for radix
	
	int m_nValue;
};

PROJECT_NS_END
#endif // __Integer_h__
