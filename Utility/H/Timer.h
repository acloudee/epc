#ifndef __TIMER_H__
#define __TIMER_H__

#include "Thread.h"
#include "List.h"
#include "Semaphore.h"
#include "ReEntrantMutex.h" // a

PROJECT_NS_BEGIN

#define TIMER_PAUSE_PRIORITY	2 
#define TIMER_ACTIVE_PRIORITY	-2 

class CTimer : public CThread {
public:
	CTimer(u_int nDelayMs, u_int nIntervalMs, int nRepetition = 1, bool bFixed = false);
	// nRepetition	INFINITE for forever,  at least 1 even it set it to less than 1.
    ~CTimer();
	bool Start(void (*pFunc)(PVOID), PVOID pPara, PCSTR pszName = NULL, 
		int nActivePriority=TIMER_ACTIVE_PRIORITY, int nStatckSize=0);
/*			@param nStackSize in bytes:
						0 in Win32 means default as calling thread, usually 2 MB
						0 in VxWorks means 8 KB in text mode and 12 kb in graphics mode
*/
	void Stop();
	void Pause(bool bLowerPriority=true);	// lower priority to TIMER_PAUSE_PRIORITY (2)
	void Resume(bool bRestorePriority=true); // restore priority to TIMER_ACTIVE_PRIORITY (-3)

	inline u_int GetDelayMs() { return m_nDelayMs; }  // return delay time in MS
	inline void SetDelay(u_int nDelayMs) { m_nDelayMs = nDelayMs; }  // return delay time in MS
	inline u_int GetIntervalMs() { return m_nIntervalMs; }  // return delay time in MS
	inline void SetIntervalMs(u_int nIntervalMs) { m_nIntervalMs = nIntervalMs; }  // return delay time in MS
	inline int GetRepetition() { return m_nRepetition;} // INFINITE for forever
	inline void SetRepetition(int nRepetition) { m_nRepetition = nRepetition; }
	inline int ExeTimes() { return m_nRunTimes;}

private:
	unsigned int WorkerFunc();

private: 
	CReEntrantMutex	m_mtxRunning;
	bool m_bStopped;
	bool m_bPause;
	void (*m_pFunc)(PVOID);
	PVOID m_pPara;
	u_int m_nDelayMs;
	u_int m_nIntervalMs;
	u_int m_nRepetition;
	u_int m_nRunTimes;
	bool m_bFixed;
	List m_lstTmrFuncThrd;
	CSemaphore m_semThrdList;
	CSemaphore m_semTimer;
	int m_nActivePriority;

private:
	class CTmrThrd : public CThread {
	public:
		CTmrThrd(void (*pFunc)(PVOID), PVOID pPara) ;
		virtual ~CTmrThrd()	{Stop();}

		bool Start(char* cpszName, int nPriority=0, int nStackSize=0, int nOptions=0);
		void Stop();
		bool SetThrdFunc(void (*pFunc)(PVOID), PVOID pPara);
		int GetStatus() { return m_nReturnVal;}
	
	protected:
		unsigned int WorkerFunc();

	private:
		u_int m_nReturnVal;
		void (*m_pFunc)(PVOID);
		PVOID m_pPara;
	};
};

PROJECT_NS_END
#endif /* __TIMER_H__ */
