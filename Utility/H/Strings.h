#ifndef __String_hpp__
#define __String_hpp__

#include "Common.h" 
#include <string.h> 
#include <stdlib.h>
#include <ctype.h> 

#define ISSPACE(x)	((x)==0x20 || (x)>=0x09 && (x)<=0x0d) 

#ifdef _DEBUG
	#define isalpha(x)	((x)>='a' && (x)<='z' || (x)>='A' && (x)<='Z') 
#endif


PROJECT_NS_BEGIN

class String
{
public:
	static int _CompareNoCase(PCSTR cpsz1, PCSTR cpsz2); 
	// return > 0 if cpsz1 > cpsz2
	//		 = 0 if cpsz1 and cpsz2 are the same
	//		 < 0 if cpsz1 < cpsz2
	// Note: only alphabets are case-less, 
	
	static void _MakeUpper(char* pszBuf, int nStartOffset=0, int nCount=-1); 
	static void _MakeLower(char* pszBuf, int nStartOffset=0, int nCount=-1); 
	static void _MakeReverse(char* pszBuf, int nStartOffset=0, int nCount=-1); 
	// pszBuf[IN,OUT] pointer to a user-specified character buffer, NULL-terminated if necessary
	// nStartOffset zero-based index
	// nCount how many characters to transform. 
	//			-1 to transform 'till the end of string (must be NULL terminated)
	// return # of changes

	static char* _FindNonSpace(char* pszBuf, bool bFromTail);
	// pszBuf null terminated '\0' string
	// bFromTail false to search from the beginning (left to right) of the string pointed to by pszBuf
	//			 true to search from the end (right to left) of the sring pointed to by pszBuf
	// return '\0' (end of string pointed to by pszBuf) if not found
	//		  NULL if pszBuf is NULL
	//		  pointer to 1st non-space character in pszBuf

	static void _Trim(char* pszBuf);
	static void _TrimLeft(char* pszBuf);
	static void _TrimRight(char* pszBuf);
	// pszBuf[IN,OUT] null terminated '\0' string
	// return pszBuf with spaces trimmed.

public:
	// constructs empty String
	String();
	// copy constructor
	String(const String& stringSrc);
	// substring from an ANSI string (converts to char)
	String(PCSTR cpsz, int nLength=-1); // pass -1(<0) to copy the entire cpsz, and zero as empty string
	// Note if you pass nLength=-1, cpsz MUST be NULl terminated; 
	//	    if nLength > 0, it is app's responsibility to guarantee cpsz is at least of nLength long.
	//		it does not check for end-of-string whenever nLength > 0.
	String(int i);
	~String();	// don't declare as "virtual destructor"... - printf() -

	// get data length
	inline int		GetLength() const { return m_nLen;}
	// TRUE if zero length
	inline bool	IsEmpty() const { return m_nLen == 0; }

	// clear contents to empty
	void	Empty();

	// return single character at zero-based index
	inline  char	operator[](int nIndex) { return GetAt(nIndex); }
	char	GetAt(int nIndex) const;
	void	SetAt(int nIndex, char ch);
	// return pointer to const string
	inline operator PCSTR() const { 	return (PCSTR) m_pszData; }

	inline bool operator==(const String& s1) const {	return (Compare((String&)s1) == 0); }
	inline bool operator==(PCSTR s1) const {	return (Compare(s1) == 0); }
	inline bool operator!=(const String& s1) const { return (Compare((String&)s1) != 0);}
	inline bool operator!=(PCSTR s1) const { return (Compare(s1) != 0); }
	inline bool operator<(const String& s1) const { return (Compare((String&)s1) < 0); }
	inline bool operator<(PCSTR s1) { return (Compare(s1) < 0); }
	inline bool operator>(const String& s1) const { return (Compare((String&)s1) > 0);}
	inline bool operator>(PCSTR s1) const { return (Compare(s1) > 0); }
	inline bool operator<=(const String& s1) const { return (Compare((String&)s1) <= 0);}
	inline bool operator<=(PCSTR s1) const { return (Compare(s1) <= 0); }
	inline bool operator>=(const String& s1) { return (Compare((String&)s1) >= 0);}
	inline bool operator>=(PCSTR s1) const { return (Compare(s1) >= 0); }

	// overloaded assignment
	// ref-counted copy from another String
	inline String& operator=(const String& stringSrc) { return (*this = (PCSTR)(String&)stringSrc); }

	// copy string content from ANSI string (converts to char)
	String& operator=(PCSTR cpsz);

	// string concatenation
	// concatenate from another character
	String& operator+=(char ch);
	// concatenate from another String
	String& operator+=(const String& string);
	// concatenate a UNICODE character after converting it to char
	String& operator+=(PCSTR cpsz);

	friend String operator+(char ch, String& string);
	friend String operator+(const String& string, char ch);
	friend String operator+(const String& string1, const String& string2);
	friend String operator+(const String& string, PCSTR cpsz);
	friend String operator+(PCSTR cpsz, const String& string);

	bool StartsWith(PCSTR cpszPrefix, int nOffset=0) const; 
	bool EndsWith(PCSTR cpszPostfix) const;
	// string comparison
	// straight character comparison
	int Compare(PCSTR cpsz) const;
	// compare ignoring case
	inline int CompareNoCase(PCSTR cpsz) const { return _CompareNoCase(m_pszData, cpsz); }
	// return > 0 if cpsz1 > cpsz2
	//		 = 0 if cpsz1 and cpsz2 are the same
	//		 < 0 if cpsz1 < cpsz2

	// simple sub-string extraction
	// return nCount characters starting at zero-based nFirst
	String Mid(int nFirst, int nCount) const;
	// return all characters starting at zero-based nFirst
	inline String Mid(int nFirst) const {	return Mid(nFirst, GetLength()-nFirst); }

	// return first nCount characters in string
	String Left(int nCount) const;
	// return nCount characters from end of string
	String Right(int nCount) const;
	//String GetLine(int nFirst = 0);
	//String GetLineNext();

	// return the length of the changed string.
	int Insert(int nIndex, char ch);
	int Insert(int nIndex, PCSTR cpsz);

	// upper/lower/reverse conversion
	String& MakeUpper(int nOffset = 0, int nLen = -1);
	// nOffset	the offset of string to make upper. 
	//			< 0 or > GetLength() returns immediately.
	// nLen		how many characterts to convert from nOffset
	//			-1 (< 0) to convert 'till the end of string.
	String& MakeLower(int nOffset = 0, int nLen = -1);
	// nOffset	the offset of string to make lower. 
	//			< 0 or > GetLength() returns immediately.
	// nLen		how many characterts to convert from nOffset
	//			-1 (< 0) to convert 'till the end of string.

	String& MakeReverse(int nOffset = 0, int nLen = -1);
	// reverse string right-to-left
	// nOffset	the offset of string to reverse
	//			< 0 or > GetLength() returns immediately.
	// nLen		how many characterts to convert from nOffset
	//			-1 (< 0) to reverse 'till the end of string.

	// trimming whitespace (both side)
	String& Trim();
	// remove whitespace starting from right edge
	String& TrimRight();
	// remove whitespace starting from left side
	String& TrimLeft();
	int Replace(char chOld, char chNew); // replace ALL occurences 
	// the number of replaced instances of the character.
	// zero if the string isn't changed
	int Replace(PCSTR cpszOld, PCSTR cpszNew); // replace ALL occurences 
	// the length of the changed string.
	int Delete(int nIndex, int nCount=1); // delete nCount characters starting from nIndex.

	// searching
	// find first instance of substring starting at zero-based index
	int Find(PCSTR cpszSub, int nStart=0) const;
	int Find(char cCH, int nStart=0) const;
	// reverse find first instance of substring starting at zero-based index
	int ReverseFind(PCSTR cpszSub) const;

protected:
	char*	m_pszData;
	int m_nLen;

private:
	static const char s_cpszNullString[2];// ""
	int		ValidateRange(int nOffset, int nLen);
	// nLen		-1 (< 0) as the end of string.
	//	return -1 for invalid, otherwise the nLen after adjustment.
};

/*extern  bool operator == (PCSTR s1, const String& s2);
extern 	bool operator != (PCSTR s1, const String& s2);
extern 	bool operator < (PCSTR s1, const String& s2);
extern 	bool operator > (PCSTR s1, const String& s2);
extern 	bool operator <= (PCSTR s1, const String& s2);
extern 	bool operator >= (PCSTR s1, const String& s2);
*/
inline bool operator==(PCSTR s1, const String& s2)
{
	return (0 == ((String&)s2).Compare(s1));
}

inline bool operator!=(PCSTR s1, const String& s2)
{
	return (0 != ((String&)s2).Compare(s1));
}

inline bool operator<(PCSTR s1, const String& s2)
{
	return (0 < ((String&)s2).Compare(s1));
}

inline bool operator>(PCSTR s1, const String& s2)
{
	return (0 > ((String&)s2).Compare(s1));
}

inline bool operator<=(PCSTR s1, const String& s2)
{
	return (0 <= ((String&)s2).Compare(s1));
}

inline bool operator>=(PCSTR s1, const String& s2)
{
	return (0 >= ((String&)s2).Compare(s1));
}


PROJECT_NS_END

#endif // __String_h__
