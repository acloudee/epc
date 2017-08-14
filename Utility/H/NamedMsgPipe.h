#ifndef __NamedMsgPipe_h__
#define __NamedMsgPipe_h__

#include "ShareMemory.h"
#include "Semaphore.h"

PROJECT_NS_BEGIN

class CNamedMsgPipe // Unidirectional
{
public:
	CNamedMsgPipe(); // Creator must be called before reader
	virtual ~CNamedMsgPipe();

	bool Open(PCSTR cpszName, bool bCreator, int nMaxQueuedMsg, int nAvgMsgSize);
	// cpszName		must not be NULL nor empty string
	// bCreator		true to create named pipe and any necessary objects.
	//				if creator called Close(), every objects will be destroyed
	//				false for to open (connect) to an existing named pipe
	//				Creator must be called before Opener
	// nMaxQueuedMsg an estimation of how many message will be written into the pipe before
	//				 pipe is full and cause Write() to block thereafter.
	// nAvgMsgSize	 an average size in bytes of each message. 
	//				This is just an estimation. 
	//				In pratice, you may write messages of variable lengths;
	//				e.g.,  message of 16-bytes, then followed by a message of 256-bytes.
	//				but you should never write a message of size exceeding (nMaxQueuedMsg * nAvgMsgSize)
	//				Besides, if byte stream is needed, use NamedStreamPipe() instead.
	void Close();
	// close of Creator() will destroy this pipe.

	int Write(PVOID pMsg, int nMsgSize);
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
	// if nBufSize is not large enough to accommodate the whole message queued, the remaining bytes is lost
	// and only nBufSize is returned.
	// pBuf	allow NULL pointer to discard message
	// nBufSize allow 0 to discard message
	// nTimeoutMs	wait new message for nTimeoutMs 
	// return # of bytes read into pBuf
private:
	void WriteMsg(PBYTE pData, int nOffset, PBYTE pMsg, int nMsgSize);
	int ReadMsg(PBYTE pData, int& nOffset, PBYTE pMsgBuf, int nMsgBufSize);

private:
	CShareMemory m_shm;
	int m_nDataBufSize;
	CSemaphore m_semaWriteSlot;
	CSemaphore m_semaReadSlot;
};

PROJECT_NS_END

#endif		// __NamedMsgPipe_h__
