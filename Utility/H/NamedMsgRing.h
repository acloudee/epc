#ifndef __NamedMsgRing_h__
#define __NamedMsgRing_h__

#include "ShareMemory.h"
#include "Semaphore.h"

PROJECT_NS_BEGIN

class CNamedMsgRing // Unidirectional
{
public:
	CNamedMsgRing(); // Creator must be called before reader
	virtual ~CNamedMsgRing();

	bool Open(PCSTR cpszName, bool bCreator, int nMaxQueuedMsg, int nMsgSize);
	// cpszName		must not be NULL nor empty string
	// bCreator		true to create named pipe and any necessary objects.
	//				if creator called Close(), every objects will be destroyed
	//				false for to open (connect) to an existing named pipe
	//				Creator must be called before Opener
	// nMaxQueuedMsg how many messages will be written into the pipe before
	//				 pipe is full and cause Write() to start overwrite oldest one thereafter.
	// nMsgSize	 size in bytes of each message. 

	void Close();
	// close of Creator() will destroy this pipe.

	int Write(PVOID pMsg, int nMsgSize);
	// this is a non-blocking call even on queue full; 
	// when queue full happens, it just overwrite the oldest message
	// pMsg will be treated as a single msg,
	// nMsgSize must not exceed nMsgSize specified in Open();
	// return # of bytes written

	int Read(PVOID pBuf, int nBufSize, int nTimeoutMs);
	// read one message inf pBuf, 
	// if nBufSize is not large enough to accommodate the whole message queued, the remaining bytes are lost
	// and only nBufSize is returned.
	// pBuf	allow NULL pointer to discard message
	// nBufSize allow 0 to discard message
	// nTimeoutMs	wait new message for nTimeoutMs 
	// return # of bytes read into pBuf, and 
	//		it should be nMsgSize as specified in Open() in most cases,
	//		unless nBufSize is less than nMsgSize.

	int Clear();
	// return # of msg discarded

private:

private:
	CShareMemory m_shm;
	int m_nDataBufSize;
	int m_nMsgSize;
	CSemaphore m_semaQueuedMsg;
};

PROJECT_NS_END

#endif		// __NamedMsgPipe_h__
