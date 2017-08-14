
#ifndef	__MUTEX_H__
#define	__MUTEX_H__
 
#include "Common.h"
#if defined(_WIN32)
#elif defined(_LINUX_)
#include <pthread.h>
#else
#include <sysLib.h>
#endif

PROJECT_NS_BEGIN
/*******************************************************************************
	This kind of mutex is effectively a binary semaphore, 
	thus it can be taken by thread-A and given by threade-B. 
	Note, however it cannot be taken for more than once in a succession, even it
	is the sampe thread which taken it previously. This logic is the nature of
	binary semaphore.

	If a recursive mutex (a mutex which can be taken by the same thread several 
	times to prevent dead-lock itself, but this mutex must be given by its owner)
	is what you want, use CReEntrantMutext instead.
 *******************************************************************************/
class CMutex 
{
public:
	CMutex();

	virtual ~CMutex();

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

#ifdef _WIN32
	bool m_bTaken;
	CRITICAL_SECTION m_cs;
#endif
};

PROJECT_NS_END

#endif
