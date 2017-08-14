#ifndef __Queue_h__
#define __Queue_h__

#include "ReEntrantMutex.h"
#include "Semaphore.h"
#include "List.h"

PROJECT_NS_BEGIN

class CQueue 
{ 
public:
	CQueue(bool bPrioritized=false);
	virtual ~CQueue();

	bool PushData(PVOID pData, bool bTail=true);
	bool PushData(List& lstData, bool bTail=true);

	// pData must not NULL
	PVOID PopData(int nTimeoutMs=0, bool bHead=true); // INFINITE for blocking call
	// NULL for no message or timeouted
	int	CountData();
	inline bool	HasData() { return CountData() > 0; }
	int Empty();
	inline SEMAPHORE_ID GetSemaphore() { return m_semaData.GetSemaphore(); }
	PVOID GetData(bool bHead=true); // skipping waiting in semaphore, No Wait At all.

protected:
	List m_lstData;
	
#ifdef _WIN32
	CRITICAL_SECTION m_mutex;
#else
	CReEntrantMutex m_mutex;
#endif 
	CSemaphore m_semaData;
};

PROJECT_NS_END
#endif//__Queue_h__
