#ifndef __RUNNABLE_H__
#define __RUNNABLE_H__

#include "Thread.h" 

PROJECT_NS_BEGIN

class CRunnable : public CThread {
public: 
	static CRunnable* _Run(void (*pFunc)(PVOID), PVOID pPara, int nRunTimes = 1, PCSTR cpszName = NULL,
							int nPriority=0, int nStackSize=0, int nOptions=0);
public:
	CRunnable();
	virtual ~CRunnable();

	bool	Start(PCSTR cpszName, int nRunTimes = 1, int nPriority=0, int nStackSize=0, int nOptions=0);
	void	Stop();	// wait until thread finished.
	void	SetFunc(void (*pFunc)(PVOID), PVOID pPara) {m_pFunc = pFunc; m_pPara = pPara;}
	int		GetCurState() {return m_nState;}
private:
	unsigned int WorkerFunc() ;

private:
	void (*m_pFunc)(PVOID);
	PVOID m_pPara;
	int m_nState;	// WorkerFunc has completed : 1; Otherwise : 0;
	bool m_bStopped;
	int m_nRunTimes;
};


PROJECT_NS_END

#endif //__RUNNABLE_H__
