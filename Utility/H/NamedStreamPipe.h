#ifndef __NamedStreamPipe_h__
#define __NamedStreamPipe_h__

#include "ShareMemory.h"
#include "Semaphore.h"

PROJECT_NS_BEGIN

class CNamedStreamPipe // Unidirectional
{
public:
	CNamedStreamPipe(); // Creator must be called before reader
	virtual ~CNamedStreamPipe();

	bool Open(PCSTR cpszName, bool bCreator, int nMaxSize);
	// cpszName		must not be NULL nor empty string
	// bCreator		true to create named pipe and any necessary objects.
	//				if creator called Close(), every objects will be destroyed
	//				false for to open (connect) to an existing named pipe
	//				Creator must be called before Opener
	// nMaxSize		an estimation of how many bytes will be written into the pipe before
	//				 pipe is full and cause Write() to block thereafter.
	void Close();
	// close of Creator() will destroy this pipe.

	int Write(PVOID pBuf, int nBufSize);
	// this is a blocking call if queue full
	// pMsg will be treated as a single msg,
	// nMsgSize must not exceed nMaxQueuedMsg * nMsgSize specified in Open();
	// return # of bytes written
	//		In pratice, you may write messages of variable lengths;
	//		e.g.,  message of 16-bytes, then followed by a message of 256-bytes.
	//		but you should never write a message of size exceeding (nMaxQueuedMsg * nAvgMsgSize)
	//		Besides, if byte stream is needed, use NamedStreamPipe() instead.

	int Read(PVOID pBuf, int nBufSize, int nTimeoutMs);
	// read one message inf pBuf, 
	// nTimeoutMs	wait new message for nTimeoutMs 
	// return # of bytes read into pBuf
private:
	void WriteData(PBYTE pData, int nOffset, PBYTE pBuf, int nBufSize);
	void ReadData(PBYTE pData, int nOffset, PBYTE pBuf, int nBufSize);

private:
	CShareMemory m_shm;
	int m_nDataBufSize;
	CSemaphore m_semaWritable;
	CSemaphore m_semaReadable;
};

PROJECT_NS_END

#endif		// __NamedStreamPipe_h__
