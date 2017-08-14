#ifndef __Base64Encoder_h__
#define __Base64Encoder_h__

#include "StringBuffer.h"
#include <stdio.h>
 
PROJECT_NS_BEGIN

/** 
 * A class to encode Base64 streams and strings.  
 * See RFC 1521 section 5.2 for details of the Base64 algorithm.
 * This class can be used for encoding strings:
 * String unencoded = "webmaster:try2gueSS";
 * String encoded = Base64Encoder.encode(unencoded);
 * or for encoding file:
 */
class CBase64Encoder {
public:
  /**
   * Returns the encoded form of the given unencoded string.
   *
   * @param cpszUnencoded the string to encode
   * @return the encoded form of the unencoded string
   */
   static CStringBuffer* _Encode(PCSTR cpszUnencoded);
   static CStringBuffer* _Encode(PCBYTE cpbUnencoded, DWORD dwLen);
   static bool _Encode(PCSTR cpszUnencodedSrcFile, PCSTR cpszEncodedDestFile);

public:
  /**
   * Constructs a new Base64 encoder that writes output to the given
   * OutputStream.
   *
   * @param out the output stream
   */
	CBase64Encoder(CStringBuffer* psbOut);
	CBase64Encoder(PCSTR cpszOutFilename);
	~CBase64Encoder();

  /**
   * Writes the given byte to the output stream in an encoded form.
   */
	void Write(BYTE byte);
  /**
   * Writes the given byte array to the output stream in an 
   * encoded form.
   *
   * @param lpb the data to be written
   * @param nOffset the start offset of the data
   * @param nLen the length of the data
   * @exception IOException if an I/O error occurs
   */
	void Write(PBYTE lpb, int nOffset, int nLen);
	void Close();
	bool IsClosed();

private:
	static const char s_cpszChars[];
	static	const int s_nCharsCount;
private:
	void	DoWrite(const char ch);
	int		m_nCharCount;
	int		m_nCarryOver;
	CStringBuffer*	m_psbOut;
	FILE*	m_fpOut;
};

PROJECT_NS_END

#endif
