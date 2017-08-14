#ifdef _LOGGER_

#ifndef __SysLog_h__
#define __SysLog_h__
#include "../../Network/H/sockhead.h"
#include "Common.h"

PROJECT_NS_BEGIN

#define DEF_FACILITY_CODE	(CSysLog::LOCAL_USE7)

class CSysLog 
{
public:
	typedef enum {
		KERNEL_MSG = 0, // 0 kernel messages
		USER_LEVEL_MSG, // 1 user-level messages
		MAIL_SYSTEM, // 2 mail system
		SYSTEM_DAEMONS, // 3 system daemons
		SECURITY_AUTHORIZATION_MSG, // 4 security/authorization messages (note 1)
		MSG_GNERATED_INTERNALLY, // 5 messages generated internally
		LINE_PRINTER_SUBSYSTEM, // 6 line printer subsystem
		NETWORK_NEWS_SUBSYSTEM, // 7 network news subsystem
		UUCP_SUSYSTEM, // 8 UUCP subsystem
		CLOCK_DAEMON, // 9 clock daemon (note 2)
		SECURITY_AUTHORIZATION_MSG2,// 10 security/authorization messages
		FTP_DAEMON, // 11 FTP daemon
		NTP_SUBSYSTEM, // 12 NTP subsystem
		LOG_AUDIT, // 13 log audit (note 1)
		LOG_ALERT, // 14 log alert (note 1)
		CLOCK_DAEMON2, // 15 clock daemon (note 2)
		LOCAL_USE0, // 16 local use 0 (local0)
		LOCAL_USE1, // 17 local use 0 (local1)
		LOCAL_USE2, // 18 local use 0 (local2)
		LOCAL_USE3, // 19 local use 0 (local3)
		LOCAL_USE4, // 20 local use 0 (local4)
		LOCAL_USE5, // 21 local use 0 (local4)
		LOCAL_USE6, // 22 local use 0 (local5)
		LOCAL_USE7, // 23 local use 0 (local6)
	} MSG_FACILITY_CODE;

public:
	CSysLog(PCSTR cpszSvr, u_short ushPort, int nTTL=144, MSG_FACILITY_CODE nFacilityCode=DEF_FACILITY_CODE);
	~CSysLog();
	inline MSG_FACILITY_CODE GetFacilityCode() { return (MSG_FACILITY_CODE)(m_nFacilityCode >> 3); } // divide by 8
	void SetServer(PCSTR cpszSvr, u_short ushPort, int nTTL);
	void Log(PCSTR cpszSrcFile, int nLine, int nModuleID, int nSeverity, PCSTR cpszMsg);
	inline bool IsRunning() { return (m_pUdpSock != NULL && m_szServer[0] != 0); }

private:
	int m_nFacilityCode;
	int m_nTTL;
	CUdpSocket* m_pUdpSock;
	char m_szServer[128];
	unsigned short m_ushPort;
};

PROJECT_NS_END

#endif //__SysLog_h__

#endif //#ifdef _LOGGER_
