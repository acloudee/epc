#ifndef __CircularBuffer_h__
#define __CircularBuffer_h__
 
#include "ReEntrantMutex.h"
#include "Semaphore.h"

#define DEF_STREAM_BUFFER_SIZE 4096	// 4K

PROJECT_NS_BEGIN

/****************************************************************************************
 * Thread safe circular buffer, 
 * on write it will overwrite the oldest buffer as necessary,
 * on read it will read the oldest buffer until no newer buffer could be read			
****************************************************************************************/

class CircularBuffer 
{
public:
	CircularBuffer(int nSize=DEF_STREAM_BUFFER_SIZE, bool bOverwritten=false);
	// nSize specify the size of the circular buffer
	// bOverwritten true to overwite the oldest data whenever needed.
	// 
	virtual ~CircularBuffer();

	int Empty();
	// return the # of bytes discarded
	bool IsEmpty();
	// return true if no data available, false otherwise
	int GetReadableDataLength();
	// return # of bytes currently in buffer

	inline int GetBufferSize() { return m_nSize > 0 ? m_nSize-1 : 0; }
	int Write(PBYTE pBuffer, int nBufSize, bool bAllOrNone=true);
	// bAllOrNone  
	//			true:  to write the whole pBuffer of nBufSize if the available 
	//				   empty buffer can accommodate them; otherwise, dont write any data
	//			false: wirte as many bytes as possible.
	// return # of bytes written into StreamBuffer, if bOverwritten is true
	//		it always returns nBufSize

	int Read(PBYTE pBuffer, int nBufSize, int nWaitMs=0);
	// nWaitMs ms to wait for data available.
	// return # of bytes read

private:
	bool m_bOverWritten;
	PBYTE m_pbBuffer;
	int m_nHead, m_nTail, m_nSize;
	CReEntrantMutex m_mtxBuffer;
	CSemaphore m_semaData;
	bool m_bSingaled;
};


PROJECT_NS_END

#endif //__CircularBuffer_h__
