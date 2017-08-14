#ifndef __ShareMemory_h__
#define __ShareMemory_h__

#include "Common.h"

#ifdef _LINUX_
	#include "Semaphore.h"
#endif

PROJECT_NS_BEGIN

class CShareMemory
{
public:
	CShareMemory();
	virtual ~CShareMemory();

	bool Create(PCSTR cpszName, int nSize, bool bClearMappedMem);
	// cpszName	could be empty or NULL string if this memory is not shared accross processes.
	//			it must not contains any slashes '/' in linux, 
	//			it must not contains backslashes, '\', nor colon, ':', in Win32
	// bClearMappedMem the returned share memory may or may not be init to all zeros, you must do it by yourself
	//		  because caller may use Create() to open an existed share memory block without any problem.
	//		 i.e. 2 processes may call Create() simultaneously and ends with both calls are successful.
	//			one of them just returns what has been created by the other.

	bool Open(PCSTR cpszName, int nSize);
	// cpszName	must NOT be empty or NULL string
	// nSize 0 to map the whole share memory in win32 
	//		but not in Linux you must not pass 0
	// the returned share memory may or may not be init to all zeros, you must do it by yourself.
	void Close();

	PVOID Lock();	
	// acquire a cross-process safty lock on shared memory for read/wrie
	// call Unlock() to unlock inter-processes lock
	// return pointer to shared memory buffer
	//			NULL if not locked or not init yet!
	void Unlock();	
	// relinquish a cross-process safty lock on shared memory
	// do NOT Unlock() if you failed to lock it out by Lock()
	inline int GetSize() { return m_nSize; }
	inline void* GetBuffer() { return m_pMappedView; } 
	// unsafe to get shared memoy pointer for read/write without mutex lock between processes synchronzation
	// return pointer to shared memory buffer

	bool WaitDirty(int nWaitTimeoutMs);
	void SetDirty(bool bDirty);
	inline PCSTR GetName() { return m_szName; }

private:
	bool Init(PCSTR cpszName, int nSize, bool bCreate, bool bClearMappedMem);

private:
	char m_szName[256];
#ifdef _WIN32
	HANDLE m_hMtxLock;
	HANDLE m_hEvtDirty;
	HANDLE m_hMappedFile;
#elif defined(_LINUX_)
	int m_nMappedFile;
	bool m_bCreator;
	CSemaphore m_semaLock;
	CSemaphore m_semaDirty;
#else
#endif
	int m_nSize;
	PVOID m_pMappedView;
};

PROJECT_NS_END

#endif //__ShareMemory_h__
