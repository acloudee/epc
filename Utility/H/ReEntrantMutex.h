#ifndef	__ReEntrantMutex_H__
#define	__ReEntrantMutex_H__

#include "Common.h"
#ifdef _WIN32
#elif defined(_LINUX_)
#include "Mutex.h"
#else
#include <sysLib.h>
#endif

PROJECT_NS_BEGIN

/*******************************************************************************
	The mutex could be taken by the same thread several times to prevent dead-locked
	But it must be given exactly the same times (more given has no effect).
	Note: the mutex could only be given by those who took it previously.
	
	If you want to take a mutex by thread-A while given (waken) it by Thread-B,
	use CMutex instead. CMutex is effectively a binary semaphore, thus it can 
	be taken by thread-A and given by threade-B. 
	Note, however it cannot be taken for more than once in a succession, even it
	is the sampe thread which taken it previously. This logic is the nature of
	binary semaphore.
 *******************************************************************************/
class CReEntrantMutex 
{
public:
	CReEntrantMutex();
	virtual ~CReEntrantMutex();

	bool    Create							//allocate a Mutex object in memory
		(
		bool bTake, /*if the Mutex is initialized to Take status or not */
		bool bPrioritized=false /*if Tasks be queued on a priority basis or not */
		); 
	
	TAKE_STATUS_CODE	Take(int nTimeoutMs); //lock the Mutex oject

	bool	Give();							 //unlock the Mutex object
	bool    Delete();						 //release the Mutex object from memory

	inline MUTEX_ID GetMutex() { return m_nMutex; }

protected:
	MUTEX_ID m_nMutex;
};

PROJECT_NS_END

#endif // __ReEntrantMutex_H__
