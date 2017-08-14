#ifndef __MODULEREG_H__
#define __MODULEREG_H__
 
#include "List.h"
#include "Semaphore.h"

PROJECT_NS_BEGIN

#ifndef _NO_CFGMGR_
	class CConfigureOperation;
#endif

#define MAX_NOTIFY_MODE	2
class CModuleReg {
public:
	typedef enum {
		ON_CFG_CHANGE = 0x1,
		ON_IP_CHANGE = 0x2
	} NOTIFY_MODE; 	// OR-able NOTIFY_MODE 

public:
	CModuleReg();
	virtual ~CModuleReg();

	bool RegModule(PCFGCALLBACKFUNC pCallBackFunc, PVOID pContext, int nMode=ON_CFG_CHANGE);
	// you can register the same call back function for different modes by 
	// registering them in a whole (OR-able them together) or by registering 
	// the call back function several times.
	// If you register the same call back function for different notification modes
	// in 2 or more calls, you could pass different pContext for each mode and 
	// it will pass the specified pContext on each notification mode.

	bool UnregModule(PCFGCALLBACKFUNC pCallBackFunc);
	#ifndef _NO_CFGMGR_
		DWORD OnCfgChanged(CConfigureOperation* pCfgMgr, NOTIFY_MODE nMode=ON_CFG_CHANGE);
	#endif
private:
	typedef struct {
		PCFGCALLBACKFUNC pFunc;
		PVOID pPara[MAX_NOTIFY_MODE];
		int nMode;	// OR-able NOTIFY_MODE 
	} MOD_REG_ITEM, *PMOD_REG_ITEM;

	PMOD_REG_ITEM Find(PCFGCALLBACKFUNC pCallBackFunc);

	CSemaphore m_semExeCtrl;
	List m_lstModule;
};

PROJECT_NS_END

#endif	//__MODULEREG_H__
