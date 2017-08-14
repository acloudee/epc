#ifndef	__SEMAPHORE_H__
#define	__SEMAPHORE_H__

#include "Common.h" 

#ifdef _LINUX_
	#include <semaphore.h> // sem_t
#else
	#ifndef _WIN32
		#include <sysLib.h>
	#endif
#endif

PROJECT_NS_BEGIN

class CSemaphore {
public:
	CSemaphore();

	virtual ~CSemaphore();

	bool Create(						//allocate a Semaphore object in memory
		long nInitCount, /* initial count */
		bool bPrioritized = false); /*if Tasks be queued on a priority basis or not */
#if (defined(_WIN32) || defined(_LINUX_))
	bool Create(PCSTR cpszName, long nInitCount); // create or open a named semaphore for IPC
	// true if the named semaphore has not been created or it has been existed.
	bool Open(PCSTR cpszName); // open a named semaphore for IPC
	// false if the named semaphore does not already exist
#endif
	TAKE_STATUS_CODE	Take(int nTimeoutMs); //lock the Semaphore oject
	
	bool	Give(long nCount=1);             //unlock the Semaphore object
    
	bool    Delete();						 //release the Semaphore object from memory
	// if a named semaphore is referrenced by other processes by 
	// either Open / Create (for a already existed named semaphore), this Delete has no effect on them
	// the real deletion of named semaphore will be postponed 'till all others finished.
	inline SEMAPHORE_ID GetSemaphore() { return m_nSemaphore; }

protected:

	SEMAPHORE_ID m_nSemaphore;
#if (defined(_WIN32) || defined(_LINUX_))
	int m_nNamedCreator;	// 0: not named, 1: creator, 2: opener
	char* m_pszName;
#endif
};

PROJECT_NS_END

#endif	/* __SEMAPHORE_H__ */
