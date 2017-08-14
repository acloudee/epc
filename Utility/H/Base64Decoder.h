#ifndef	__Base64Decoder_h__
#define __Base64Decoder_h__

#include "ByteBuffer.h"
#include "ctype.h"

PROJECT_NS_BEGIN

/** 
 * A class to decode Base64 streams and strings.  
 * See RFC 1521 section 5.2 for details of the Base64 algorithm.
 * <p>
 * This class can be used for decoding strings:
 * <blockquote><pre>
 * String encoded = "d2VibWFzdGVyOnRyeTJndWVTUw";
 * String decoded = Base64Decoder.decode(encoded);
 */
class CBase64Decoder  
{
public:
  /**
   * Returns the decoded form of the given encoded string.
   *
   * @param cpszEncoded the string to decode
   * @return the decoded form of the encoded string
   */
	static bool _Decode(PCSTR cpszEncoded, PBYTE lpbBuffer, DWORD& dwLen);
	// dwLen [IN] buffer lenggh in bytes pointed to by lpbBuffer, mim must be (0.67*strlen(cpszEncoded) + 32)
	//		 [OUT]	decoded bytes written into lpbBuffer
	static CByteBuffer* _Decode(PCSTR cpszEncoded);
	static bool _Decode(PCSTR cpszEncodedSrcFile, PCSTR cpszDecodedDestFile);

public:	
  /**
   * Constructs a new Base64 decoder that reads input from the given
   * InputStream.
   */
	CBase64Decoder(PCSTR cpszEncoded, bool bViaFile);
	~CBase64Decoder();
  /**
   * Returns the next decoded character from the stream, or -1 if
   * end of stream was reached.
   *
   * @return  the decoded character, or -1 if the end of the
   *      input stream is reached
   */
	bool IsClosed();

	int Read();
  /**
   * Reads decoded data into an array of bytes and returns the actual 
   * number of bytes read, or -1 if end of stream was reached.
   *
   * @param buf the buffer into which the data is read
   * @param off the start offset of the data
   * @param len the maximum number of bytes to read
   * @return  the actual number of bytes read, or -1 if the end of the
   *      input stream is reached
   */
  int Read(PBYTE lpbDecoded, int nOffset, int nLen);

private:
	static const char s_cpszChars[];
	static	const int s_nCharsCount;
	// A mapping between char values and six-bit integers
	static	int s_nDecode2Bits[128];	
	static int _SetMappings();
private:
	int		DoRead();
	int		m_nCharCount;
	int		m_nCarryOver;
	const char*	m_cpchNextEncoded;	
	FILE* m_fpIn;
};

PROJECT_NS_END

#endif

