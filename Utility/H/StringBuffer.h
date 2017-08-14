#ifndef __StringBuffer_H__
#define __StringBuffer_H__
#include "ByteBuffer.h"
 
PROJECT_NS_BEGIN

class CStringBuffer : public CByteBuffer
{
public:
	CStringBuffer();
	CStringBuffer(PCSTR cpszSrc);
	CStringBuffer(int nInitCapacity);
	virtual ~CStringBuffer();

	CStringBuffer& Append(PCSTR cpszSrc);
	inline CStringBuffer& Append(CStringBuffer& sb) { return Append(sb.ToString());}
	CStringBuffer& Append(CStringBuffer* psb);
	CStringBuffer& Append(char c);
	CStringBuffer& Append(int i);	
	CStringBuffer& Append(long l);
	inline CStringBuffer& Append(bool b) { return Append(b? "true": "false");}

	CStringBuffer& Delete(int nStart, int nEnd);
	// delete character(s) and move the following characters forward.
	// nStart is included but nEnd is excluded
	CStringBuffer& DeleteCharAt(int nIndex);
	// delete character(s) and move the following characters forward.

	inline char GetCharAt(int nIndex) { return (char)GetAt(nIndex);}
	bool SetCharAt(int nIndex, char ch) { return SetAt(nIndex, (BYTE)ch); }
	PSTR GetBuffer(); // !! Risky
	// export m_lpszBuf and reset it to NULL
	// after this call, its user's responsibility to reclaim this buffer
	void Clear();

	// convenient functions	
	inline CStringBuffer& operator = (CStringBuffer& buf) { SetData(buf.GetLength(), buf.GetData()); return *this; }
	inline CStringBuffer& operator = (PSTR pbData) { SetData((int)strlen((PCSTR)pbData), (PBYTE)pbData); return *this;}
	inline CStringBuffer& operator+=(PCSTR cpszSrc) { return Append(cpszSrc);}
	inline CStringBuffer& operator+= (CStringBuffer& buf) { return Append(buf); }
	inline CStringBuffer& operator+=(CStringBuffer* psb) { return Append(psb);} 
	inline CStringBuffer& operator+=(char c) { return Append(c);}
	inline CStringBuffer& operator+=(int i) { return Append(i);}
	inline CStringBuffer& operator+=(long l) { return Append(l);}
	inline CStringBuffer& operator+=(bool b) { return Append(b);}

	static CStringBuffer* ToHex(int nLen, PBYTE pbBuffer, bool bHexPrefix = false );// prefix with "0x" 
	static int ToHex(int nLen, PBYTE pbBuffer, CStringBuffer& sb, bool bHexPrefix = false );// prefix with "0x" 

private:
	static PCSTR s_cpszHex;	
};

PROJECT_NS_END

#endif
