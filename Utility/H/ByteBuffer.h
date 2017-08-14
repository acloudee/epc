#ifndef __ByteBuffer_h__
#define __ByteBuffer_h__
 
#include "Common.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
  
PROJECT_NS_BEGIN

class CByteBuffer {

public:
	static int _RegularExprFind(const PBYTE pbSrc, int nSrcLen, const PBYTE pbTarget, int nTargetLen);
	// return the index of the 1st matched instance, -1 otherwise
	CByteBuffer(int nCapacity=16);	
	CByteBuffer(int nLen, PCBYTE lpbData);
	CByteBuffer(PCSTR cpszFilename);
	virtual ~CByteBuffer();

	virtual inline int	GetLength() { return m_nLen; }
	virtual bool SetLength(int nLen); // nLen must less than m_nLen

	inline PBYTE GetData() { return m_lpbData; }
	bool SetData(int nLen, PCBYTE lpbData); // reset data, may cause buffer to shink
	virtual inline PCSTR ToString() { return (PCSTR)m_lpbData; }
	int ToHex(int nLen,  PSTR pszBuf, bool bHexPrefix = false /* prefix with "0x" */);
	/************************************************************
	 *	nBufLen should be 2*GetLength()+1 if bHexPrefix is false,
	 *  and be 2*GetLength()+3 if bHexPrefix is true.
	 *  return: hex characters written into pszBuf
	 ************************************************************/
	BYTE GetAt(int nIndex) { return IsValid(nIndex) ? m_lpbData[nIndex] : 0; }
	bool SetAt(int nIndex, BYTE b);// return false if nIndex is out of boundary

	virtual bool AppendFile(PCSTR cpszFilename);
	CByteBuffer& Append(int nLen, PCBYTE lpbData);
	virtual inline CByteBuffer& Append(const CByteBuffer& buf) { return Append(((CByteBuffer&)buf).GetLength(), ((CByteBuffer&)buf).GetData()); }
	CByteBuffer& AppendZero(int nLen); // 	append nLen bytes and set them to zero.
	inline CByteBuffer& operator += (const CByteBuffer& buf) { return Append(buf); }
	inline CByteBuffer& operator = (const CByteBuffer& buf) { SetData(((CByteBuffer&)buf).GetLength(), ((CByteBuffer&)buf).GetData()); return *this; }
	inline CByteBuffer& operator = (PCBYTE pbData) { SetData((int)strlen((PCSTR)pbData), pbData); return *this;}
	inline bool operator == (const CByteBuffer& buf) const { return Compare(buf) == 0; }
	inline bool operator != (const CByteBuffer& buf) const { return Compare(buf) != 0; }
	inline bool operator < (const CByteBuffer& buf) const { return Compare(buf) < 0; }
	inline bool operator > (const CByteBuffer& buf) const { return Compare(buf) > 0; }
	inline bool operator >= (const CByteBuffer& buf) const { return Compare(buf) >= 0; }
	inline bool operator <= (const CByteBuffer& buf) const { return Compare(buf) <= 0; }
	inline BYTE operator[](int nIndex) { return GetAt(nIndex); }
	bool IsValid(int nIndex); 

	virtual int Compare(const CByteBuffer& buf) const;	// 0:==, 1:>, -1:<
	PBYTE GetBuffer();	// !! Risky
	// export m_lpszBuf and reset it to NULL
	// after this call, its user's responsibility to reclaim this buffer
	inline int RegularExprFind(const PBYTE pbTarget, int nTargetLen) { return _RegularExprFind(m_lpbData, m_nLen, pbTarget, nTargetLen); }
	// return the index of the 1st matched instance, -1 otherwise
	
protected:
	int	m_nLen;
	int m_nCapacity;
	PBYTE  m_lpbData;
	virtual void Init(int nCapacity = 16);
	virtual bool EnsureCapacity(int nAddedLen);	

private:
	static PCSTR s_cpszHex;

};

PROJECT_NS_END

#endif // __ByteBuffer_h__
