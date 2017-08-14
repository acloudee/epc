#ifndef	__THREAD_H__
#define	__THREAD_H__

#include "Common.h"
#include "Mutex.h"
#ifdef _WIN32
	#include "StringMap.h"
#elif defined(_LINUX_)
	#include <pthread.h>
	#include "StringMap.h"
#else
	#include <taskLib.h>
	#include <sysLib.h>
#endif 

PROJECT_NS_BEGIN

#ifdef _LINUX_
bool _BoostProcessPriority(); // must called by main() thread
/******************************************************
 * FunctionName:  _BoostProcessPriority
 *
 * Synopsis: Maximises the priority of the process for timing accuracy
 *
 * Input :  None
 *
 * Output :  N/A
 *
 * Returns:  N/A
 *
 * Description : Sets the scheduling policy to be SCHED_RR
 *               and sets the priority to the max possible
 *
 * Cautions : None
 *
 * Published : Yes
 *
 ******************************************************/
#endif

class CThread {
public:
#if (defined( _WIN32) || defined(_LINUX_))
	static bool _Initialize();
	static void _Uninitialize();
#endif
	static void _DumpAllThread();
public:
	// current Thread functions
	static bool _Suspend(); // suspend yourself
	static bool _SetPriority(int nPriority);	// local priority level -10 ~ 10, -10 indicates the highest priority level and 10 as the lowest.
	static bool _AdjustPriority(int nDiff);
	// it is relatvie to the current priority.
	// < 0 => increase priority
	// > 0 => decrease priority
	static int _GetPriority();
	static THREAD_ID _GetThreadID();	// get self id of the calling thread
	static bool _GetName(THREAD_ID nId, char* pszBuf, int nBufLen); // return false on failure
	// get thread name, if nId is zero, it will get the thread name of the calling thread.

public:
	CThread();
	virtual ~CThread();
	bool	Start(PCSTR cpszName, int m_nPriority = 0, int nStackSize=0, int nOptions=0);
	/* 
		@param cpszName	Thread name used for debugging but will be ignored under win32 
		@param nPriorityLevel:	-10 ~ 10, which is mapped to OS priority range. 
								-10 indicates the highest priority level and 10 as the lowest.
					  VxWorks:	-10		: 0 which indicates the highest priority.
								  0		: 127	
								 10		: 255 which indicates the lowest priority.
					    Win32:	-10		: THREAD_PRIORITY_TIME_CRITICAL 
								-9		: THREAD_PRIORITY_HIGHEST Indicates 2 points above normal priority for the priority class. 
								-8 ~ -5	: THREAD_PRIORITY_ABOVE_NORMAL Indicates 1 point above normal priority for the priority class. 
								-4 ~  4	: THREAD_PRIORITY_NORMAL Indicates normal priority for the priority class. 
								 5 ~  8	: THREAD_PRIORITY_BELOW_NORMAL Indicates 1 point below normal priority for the priority class. 
								 9		: THREAD_PRIORITY_LOWEST Indicates 2 points below normal priority for the priority class. 
								 10		: THREAD_PRIORITY_IDLE Indicates a base priority level of 1 for IDLE_PRIORITY_CLASS, BELOW_NORMAL_PRIORITY_CLASS, NORMAL_PRIORITY_CLASS, ABOVE_NORMAL_PRIORITY_CLASS, or HIGH_PRIORITY_CLASS processes, and a base priority level of 16 for REALTIME_PRIORITY_CLASS processes. 
						Linux:	-10		: 1 which indicates the highest priority
								  0		: 50
								 10		: 99 which indicates the lowest prioriyt
		@param nStackSize in bytes:
						0 in Win32 means default as calling thread, usually 2 MB
						0 in VxWorks means 8 KB 
						0 in Linux means default, about 2MB
		@param nOptions:	
					  VxWorks: 
							VX_FP_TASK (0x0008)  execute with floating-point coprocessor support. 
							VX_PRIVATE_ENV (0x0080) include private environment support (see envLib). 
							VX_NO_STACK_FILL (0x0100) do not fill the stack for use by checkStack( ). 
							VX_UNBREAKABLE (0x0002) do not allow breakpoint debugging.
						Win32: CREATE_SUSPENDED 
						Linux: Unsupport
	*/
	void	Stop();	// wait until thread finished.
	THREAD_ID GetThreadID();	// get thread id of this thread object
	bool SetPriority(int m_nPriority);	// local priority level -10 ~ 10, -10 indicates the highest priority level and 10 as the lowest.
	inline bool AdjustPriority(int nDiff) { return SetPriority(m_nPriority+nDiff); }
	// it is relatvie to the current priority.
	// < 0 => increase priority
	// > 0 => decrease priority

	inline bool GetName(char* pszBuf, int nBufLen) // return false on failure
				{ return _GetName(GetThreadID(), pszBuf, nBufLen); }
	// coonvenient funciton to get name of current thread, same as CThread::_GetName(CThread::GetThreadID())

	int GetPriority();
	bool Suspend(); // suspend the created thread, this usually paired with Resume
	bool Resume();	// resume the created thred, this usually paired with suspend
	// Note: Suspend & Resume must be paried & appears exactly the same # of times
	
	// calling Thread specific functions
	static bool Delay(int nMilliSeconds); 
	/*
		put the calling thread into suspended state, relinguishing CPU to others.		
		@param nMilliSeconds	VxWorks: delay in ticks of nMilliSeconds*sysClkRateGet( )/1000;
						Win32: sleep in miniseconds
	 */
	bool Lock();	
	/* 
		Disable task scheduling, making the calling thread to occupy CPU.
		VxWorks only, alwayse return false in Win32
	 */
	bool Unlock();	// Enable task scheduling, VxWorks only, alwayse return true in Win32
#ifdef _LINUX_
	static PVOID ThreadFunc(PVOID param);
#elif defined(_WIN32)
	static unsigned	__stdcall /* WINAPI */ ThreadFunc(PVOID param);
#else
	static unsigned long ThreadFunc(PVOID param);
#endif

private:
	virtual	inline unsigned int WorkerFunc() { return 0; }
	static int _ToOsPriority(int nPriority);	// set local priority level to -10~10, and return OS priority that is mapped to 
	static int _ToSystemIndepPriority(int nPriority);

	THREAD_ID	m_nThread;
	int m_nPriority;

#ifdef _WIN32 
	DWORD m_dwThreadID;
#else
	CMutex m_mtxThreadCtrl;
	CMutex m_mtxData; // for m_nThread, SetPrioriy()/GetPriority and Lock() to be safe before Stop() been called.
#endif

#if (defined( _WIN32) || defined(_LINUX_))
	static bool _AddThreadName(THREAD_ID nTID, PCSTR cpszName);
	static bool _RemoveThreadInfo(THREAD_ID nTID);
	static THREAD_ID s_tidInitializer;
	static CPtr2StringMap*	s_pMapThreadID2Name;

	#ifdef _WIN32
		static CRITICAL_SECTION s_csThreadID2Name;
	#elif defined(_LINUX_)
		static pthread_mutex_t s_mtxThreadID2Name;
	#endif
#endif
};

PROJECT_NS_END

#endif	//endof __THREAD_H__
