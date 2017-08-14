#ifdef _LOGGER_
#ifndef __NetLogger_h__
#define __NetLogger_h__
 
#include "Thread.h"
#include "Strings.h"
#include "ReEntrantMutex.h"

#include "../../ConfigurationManager/H/ConfigureOperation.h"

#ifdef _LINUX_
#include <stdarg.h>
#endif

PROJECT_NS_BEGIN

#define NET_LOGGER_CLIENT_PORT	901	// TCP

class CNetLogger : private CThread {
public:
	static PCSTR s_cpszNetLoggerName;
public:
	CNetLogger();
	virtual ~CNetLogger();
	bool Launch(PCSTR cpszServer, unsigned short ushPort=DEF_CFG_LOGGER_PORT, bool bTcp=true, unsigned char nTTL=144);
	// reject NULL as "localhsot"
	// it will call IsLogging() to avoid re-connection
	void Stop();
	inline bool IsRunning() { return !m_bStop; }
	bool FormatLog(PCSTR cpszFormat, ...); // the largest string to log is up to 4095 characters.
	bool Log(PCSTR cpszMsg);
	bool Log(PCSTR cpszFmt, va_list& argList);	// the largest string to log is up to 4095 characters.
	/*
		for performance issue, the FormatLog() and Log(cpszFmt, argList) MUST be called
		sequentially by external mutex due to they use the same format buffer pointed to by m_pFmtBuffer
	 */
	bool IsLogging(PCSTR cpszServer, unsigned short ushPort, bool bTcp);
	// checking whether it is currently logging to the specific server, port and protocol
	inline String GetServer() { return m_szServer; }
	inline unsigned short GetPort() { return m_ushPort; }
	inline bool IsTcpLogger() { return m_bTcp; }

protected:
	virtual	unsigned int WorkerFunc();
private:
	PVOID Connect();
	void GetTime(char* szBuf);
	CReEntrantMutex	m_mtxBuffer;
	char m_szServer[128];
	unsigned short m_ushPort;
	int m_nBufferLength;
	PBYTE m_pLogBuffer;
	char* m_pFmtBuffer;
	int m_nBegin, m_nEnd;
	bool m_bStop;
	bool m_bTcp;
	int m_nTTL;
};

PROJECT_NS_END

#endif // __NetLogger_h__
#endif //#ifdef _LOGGER_
