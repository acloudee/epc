#ifndef __Logger_h__
#define __Logger_h__

#include "SysLog.h"
#include "StringMap.h"  
#include "ReEntrantMutex.h"
#include "NetLogger.h"

#ifndef _WIN32 
	#include <stdarg.h>	// va_list
#endif

PROJECT_NS_BEGIN

/*****************************************************************************************
	Please take a look at LoggerSampleCfg.txt first 
	This logger should be singleton 
	Prepossor Definitons
	1. _LOGGER_		Turn on/off log framework
			Let it open "Logger.cfg" as the configuration file on the same folder 
			as current directory, (Not Available on VxWorks)
	2. DEBUG_LEVEL,		default is LogError (VxWorks only)
	3. DEBUG_MODULES,	default is ALL_MODULES (VxWorks only)
	4. _LOG_MEMORY_USAGE_, monitor new/delete functions calls
		4.1. SAFETY_MEMORY_MARGIN,	default is 256*1024 (256 KB)
				Memory size to set aside that it could dump memory logs on exhausting system memory.
		4.2. MEMORY_ERROR_THRESHOLD,	default is 1500
				Threshold to alart user as error whenever the delta amount of new() function calls exceeds this threshold.
		4.3. MEMORY_WARNING_THRESHOLD, default is 800
				Threshold to alart user as warning whenever the delta amount of new() function calls exceeds this threshold.
		4.4. MEMORY_INFOMATIVE_THRESHOLD, default is 500
				Threshold alart user as informative whenever the delta amount of new() function calls exceeds this threshold.
		4.5. MEMORY_TRACE_THRESHOLD, default is 200
				Threshold to alart user as trace whenever the delta amount of new() function calls exceeds this threshold.
	Note:
		String.cpp, HashMap.cpp and StringMap.cpp should not turn _LOG_MEMROY_USAGE_ on
		Utility classes should not use Logger utility as well. That is, dont call CLogger::Logxxx() to log
		to improve efficiency.
 *****************************************************************************************/
#include "../../ConfigurationManager/H/ConfigureOperation.h"

extern "C" {
	int _ConfigureLogger();
}

int ConfigureLogger(CConfigureOperation* pCfgMgr, PVOID pCtx);
int RestartNetLoggerLogger(CConfigureOperation* pCfgMgr, PVOID pCtx);

class CLogger {
public:
	static void _GetTime(char* szBuf);	// 33 chars at least
public:
	CLogger(bool bLogMemory=false);
	virtual ~CLogger();
	bool Open();
	void Close();

	void SysLog(PCSTR cpszSrcFile, int nLine, int nModuleID, int nSeverity, PCSTR cpszMsg);
	// cpszSrcFile _SRC_FILE_
	// nLine __LINE__

	void Log(LOG_FUNC_SIG_HEADER, int nModule, LOG_LEVEL nLogLevel, const char* cpszMsg=NULL, int nLen=-1);
	// cpszSrcFile _SRC_FILE_
	// nLine __LINE__
	// cpszFuncSig _FUNC_SIG_
	// -1, log unitl end of string, nLen the length of string not including '\0';
	void RawFormatLog(int nModule, LOG_LEVEL nLogLevel, PCSTR cpszFormat, ...);
	// printf-like logger without _SRC_FILE_ & __LINE__ prefixed.
	void FormatLog(LOG_FUNC_SIG_HEADER, int nModule, LOG_LEVEL nLogLevel, PCSTR cpszFormat, ...);
	// cpszSrcFile _SRC_FILE_
	// nLine __LINE__
	// printf-like logger without _SRC_FILE_ & __LINE__ prefixed.
	void HexLog(LOG_FUNC_SIG_HEADER, int nModule, LOG_LEVEL nLogLevel, void* pbData, int nLen);
	// cpszSrcFile _SRC_FILE_
	// nLine __LINE__
	// cpszFuncSig _FUNC_SIG_
	// log unitl end of string, nLen the length of string not including '\0';
	void FormatArg(LOG_FUNC_SIG_HEADER, int nModule, LOG_LEVEL nLogLevel, PCSTR cpszFormat, va_list& argList);
	// cpszSrcFile _SRC_FILE_
	// nLine __LINE__
	// cpszFuncSig _FUNC_SIG_
	// printf-like logger without _SRC_FILE_ & __LINE__ prefixed.

	// memory logger
	void SetupMemoryExhaustedHandler(LOG_FUNC_SIG_HEADER, int nModuleID, PART_ID nPartID);
	// cpszSrcFile _SRC_FILE_
	// nLine __LINE__
	// cpszFuncSig _FUNC_SIG_
	// this could be called globally once on main()
	void RemoveMemoryExhaustedHandler(LOG_FUNC_SIG_HEADER, int nModuleID);
	// cpszSrcFile _SRC_FILE_
	// nLine __LINE__
	// cpszFuncSig _FUNC_SIG_
	// this could be called globally once before exiting main()
	PVOID LogNewMemory(LOG_FUNC_SIG_HEADER, int nModuleID, PVOID pMem, int nSize, PCSTR cpszDesc=NULL);
	// cpszSrcFile _SRC_FILE_
	// nLine __LINE__
	// cpszFuncSig _FUNC_SIG_
	// e.g.: Object* pObj = (Object*)LogNewMemory(new Object(), "Object*");
	void LogDeleteMemory(LOG_FUNC_SIG_HEADER,  int nModuleID, PVOID pMem, int nSize, PCSTR cpszDesc=NULL);
	// cpszSrcFile _SRC_FILE_
	// nLine __LINE__
	// cpszFuncSig _FUNC_SIG_
	// e.g.: LogDeleteMemory(pObj, "Object*"), delete (Object*)pObj;
	void DumpMemoryUsage(int nModuleID);
	int CountMemoryUsage();

	bool CheckLogLevel(int nModuleId, LOG_LEVEL nLogLevel);// ? should this public
	void Reset(PVOID pCfg, bool bAll);
	void RestartNetLogger(bool bTcp=true, unsigned char nTTL=144);
	void ConfigureLogger(IPRANGER_CFG_LOGGER_TYPE nLoggerType,  
						PCSTR cpszServer,	// the port is embeded as "IP:port"
				 		unsigned short ushPort,
						unsigned char nTTL); // ignored 
	void ConfigureLogger(PBYTE pbLogLevels);
	void ConfigureLogger(bool bCall, bool bDebug, bool bError, PBYTE pbLogLevels, unsigned char nTTL);
	void SetMask(int nMask);	// OR-able mask
	// File-Logger-Mask = 0x01 = 0x01 << FILE_LOGGER
	// TCP-Logger-Mask = 0x02 = 0x01<< TCP_LOGGER 
	// UDP-Logger-Mask = 0x04 = 0x01 << UDP_LOGGER
	// to block out the logger stream temporarily, set the corresponding bit to 1.
	// Note: it cannot block those logs of LogFatal.
	inline int GetMask() { return m_nMask; }	// OR-able mask
protected:	
	CHashMap m_mapModuleLogger;

private:
	#ifdef _WIN32
		static int _MemoryAllocationFailHandler(u_int nSize);
	#else 
		static void _MemoryAllocationFailHandler();
	#endif 
	static void _TerminateHandler(void);

	typedef struct {
		LOG_LEVEL	nLogLevel;
	} MODULE_CFG;
 
	static const char* s_cpszHex;
	
	static void _RenderLogInfo(char* szLogInfoBuf);	// 70 chars at least, in the format of "time taskname(taskID)"

	void ClearMemoryLog();
	bool IsMasked(bool bNetLogger);
	int OnMemoryAllocationFailure();
	CNetLogger	m_netLogger;
	CSysLog*	m_pSysLogger;
	CReEntrantMutex m_mutexLog;
	CReEntrantMutex m_mutexLogMemory;
	bool m_bSysLogger;

	class MEMORY_LOG {
	public:
		MEMORY_LOG(PCSTR cpszDesc=NULL, int nMemSize=0) { strDesc = cpszDesc, nSize = nMemSize; }
		String strDesc;
		int nSize;
	};
	CHashMap m_mapMemoryUsage;
	PVOID m_pSystemNewMemoryHandler;
	bool m_bLogMemory;
	int m_nMask;
};

PROJECT_NS_END

#endif // __Logger_h__

