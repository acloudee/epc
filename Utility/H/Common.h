#ifndef __Common_h__
#define __Common_h__
/***************************************************************************
*  Change Log:
*
*  Date        By      Description
*  ========    ===     ====================================================
*  09/25/2004 Albert  Add the enum definition of LogComponent for the CODEC and DeviceManager of video and whiteboard.
***************************************************************************/
#ifndef __LITTLE_ENDIAN
	#define __LITTLE_ENDIAN 1234
#endif
#ifndef __BIG_ENDIAN
	#define __BIG_ENDIAN 4321
#endif

#ifdef _WIN32
	#include <windows.h>
	#ifndef _CRT_SECURE_NO_WARNINGS
		#define _CRT_SECURE_NO_WARNINGS
	#endif
#elif defined(_LINUX_)
	#include <unistd.h>
	#include <semaphore.h>
#else // vxWorks
	#include "vxWorks.h" 
	#include <cplusLib.h>
#endif   // endian

#ifndef __BYTE_ORDER
	#ifdef _WIN32
		#define __BYTE_ORDER	__LITTLE_ENDIAN
	#elif defined(_LINUX_)
		#define __BYTE_ORDER	__LITTLE_ENDIAN
	#else // vxWorks
		#if (CPU == MIPS32)
			#define __BYTE_ORDER	__BIG_ENDIAN
		#elif (CPU == ARMARCH4)
			#define __BYTE_ORDER	__LITTLE_ENDIAN
		#else 
			#error __BYTE_ORDER
		#endif
	#endif   // endian
#endif

#include <time.h>

#ifdef _USE_NS_
	#define PROJECT_NS_BEGIN	namespace IPPhone {
	#define PROJECT_NS_END		}
	#define USE_PROJECT_NS		using namespace IPPhone;
#else
	#define PROJECT_NS_BEGIN
	#define PROJECT_NS_END
	#define USE_PROJECT_NS		
#endif	// _USE_NS_

#ifdef _WIN32
	#define THREAD_ID		HANDLE
	#define SEMAPHORE_ID	HANDLE
	#define MUTEX_ID		HANDLE
	#define	PART_ID			HANDLE	// heap handle

	// cross-platfrom time-related functions
	#define	TIME_T			__time64_t
	#define	_TIME			_time64
	#define	MKTIME			_mktime64
	#define DIFFTIME(x, y)		((x)-(y))

#elif defined(_LINUX_)
	#define THREAD_ID		pthread_t
	#define SEMAPHORE_ID	sem_t*
	#define MUTEX_ID		pthread_mutex_t*
	#define INFINITE		-1
	#define PART_ID			void*
	#define IPC_TAKE_TIME_STEP_MS	35
	#ifndef max
		#define max(a,b)    (((a) > (b)) ? (a) : (b))
	#endif	// max
	#ifndef min
		#define min(a,b)    (((a) < (b)) ? (a) : (b))
	#endif	// min
		typedef int		BOOL;
	#if !defined(TRUE)
		#define TRUE	1
	#endif
	#if !defined(FALSE)
		#define FALSE	0
	#endif

	// cross-platfrom time-related functions
	#define	TIME_T			time_t
//	#define	_TIME			time	// linux will implement this funciton
	#define	MKTIME			mktime
	#define DIFFTIME(x, y)	difftime(x, y)
#else
	#define THREAD_ID		int
	#define SEMAPHORE_ID	SEM_ID
	#define MUTEX_ID		SEM_ID
	#define INFINITE    	WAIT_FOREVER

	// cross-platfrom time-related functions
	#define	TIME_T			time_t
	#define	_TIME			time
	#define	MKTIME			mktime
	#define DIFFTIME(x, y)	difftime(x, y)
#endif

#define INVALID_THREAD_ID	0

#ifndef _WIN32
	typedef unsigned int	u_int;
	#ifdef _I18N_ 
		typedef wchar_t	TCHAR;
		#define _T(str)	L#str
	#else
		typedef char TCHAR;
		#define _T(str)	str
	#endif
#endif
typedef unsigned short	u_short;
typedef unsigned long	u_long;

typedef char int8;
typedef short int16;
typedef int int32;
typedef unsigned char u_int8;
typedef unsigned short u_int16;
typedef unsigned int u_int32;

#if !(defined(_WIN32) && defined(_GUI_))
//	typedef TCHAR* PSTR;
//	typedef const TCHAR* PCSTR;
	typedef unsigned  char  BYTE;
	typedef BYTE*	 PBYTE;
	typedef const BYTE*	PCBYTE;
	typedef	void*	PVOID;
	typedef u_short	WORD;
	typedef	WORD*	PWORD;
	typedef u_long	DWORD;
	typedef	DWORD*	PDWORD;
#endif

#ifndef _BYTE_ALIGN_
	#ifdef  _ATTR_PACK_
		#define _BYTE_ALIGN_	__attribute__((packed))	
	#else
		#define _BYTE_ALIGN_
	#endif
#endif	

PROJECT_NS_BEGIN

typedef void*	POSITION;

#ifndef  _ATTR_PACK_
	#pragma pack(1)
#endif
 
#ifdef _WIN32
	typedef SYSTEMTIME SYSTIME;
	typedef LPSYSTEMTIME PSYSTIME;
#else  
	typedef struct { // st 
		WORD wYear;			// Specifies the current year in 4-digits;
		WORD wMonth;		// Specifies the current month; January = 1, February = 2, and so on;
		WORD wDayOfWeek;	// Specifies the current day of the week; Sunday = 0, Monday = 1, and so on;
		WORD wDay;			// Specifies the current day of the month(1~31);
		WORD wHour;			// Specifies the current hour(0~23);	
		WORD wMinute;		// Specifies the current minute(0~59);	
		WORD wSecond;		// Specifies the current second(0~59);
		WORD wMilliseconds; // Not in use, always 0;
	} SYSTIME, *PSYSTIME;	
#endif	//#ifndef _WIN32

typedef struct {
	bool bIPv6; // true for IPv6 false for IPv4
	bool bDnsName; // true for NULL terminated DNS name; false for IP address
	union {
		char szDnsName[256];
		union {
			BYTE ipBuf[4];
			u_long ulIP;
		} ip4;
		union {
			BYTE ipBuf[16];
			u_short ushIP[8];
		} ip6;
	} ip;
} NET_ADDR;

typedef struct {
	u_long ulPackets[2] _BYTE_ALIGN_ ;	// sent packet count, received packet count
	u_long ulBytes[2] _BYTE_ALIGN_ ;	// sent bytes count, received bytes count
	u_long ulLostPackets[2] _BYTE_ALIGN_ ;	// lost packet count (sent), lost packet count (received)
	u_long ulDelay[2] _BYTE_ALIGN_ ;	// in ms	one-way, round-trip
	char szSessionInfo[2][80] _BYTE_ALIGN_ ; // local/remote RTP session. hostname:RTPport/RTCP = 64 + 1 + 5 + +1 + 5 + 1 = 71
}  _BYTE_ALIGN_  MEDIA_TRAFFIC;	// only RTP matters, not including RTCP;

#define	LOCAL_PORT	0
#define	MAPPED_PORT	1

typedef struct {
	u_long ulIP;
	u_short ushRtp;
	u_short ushRtcp;
} RTP_PORT;

#ifndef  _ATTR_PACK_
	#pragma pack()
#endif

typedef enum 
{
	TSC_OK,
	TSC_TIMEOUT,
	TSC_FAILED
} TAKE_STATUS_CODE; // return code of Take() member function

/********************************************************************
 * Note: 
 * 1. define _LOGGER_ in project settings to enable Logger
 * 2. define CURRENT_MOUDLE ito be one of the following 
 *		macros to specify which module is under compilation
 * 3. To compile Utility module, both DEBUG_LEVEL and DEBUG_MODULES
 * 		must be defined, where DEBUG_LEVEL is of type LOG_LEVEL.
 * 		To disable certain moudle's logging capability, define
 *		DEBUG_MODULES to be (ALL_MODULES & ~xxxx_MODULE_ID)
 * 4. For all modules, logs more emergent than LogWarning (excluded) 
 *		cannot be disabled even if DEBUG_MODUES doesn't set certain 
 *		modules. To filter out LogError, set DEBUG_MODULES to LogFatal.
 *******************************************************************/
enum {
	UTILITY_MODULE_ID = 0,
	NETWORK_MODULE_ID,
	RTP_MODULE_ID,
	SDP_MODULE_ID,
	SIP_MODULE_ID,
	NEGOTIATOR_MODULE_ID,
	DM_MODULE_ID,
	CM_MODULE_ID,
	ATTENDANT_MODULE_ID,
	DNS_MODULE_ID,
	STUN_MODULE_ID,
	CFG_MODULE_ID,
	UI_MODULE_ID,
	AUTOPROV_MODULE_ID,
	SNTP_MODULE_ID,
	XML_MODULE_ID,
	TELNET_MODULE_ID,
	HTTP_MODULE_ID,
	SNMP_MODULE_ID,
	RESV_MODULE_ID,
	DSP_MODULE_ID,
	UPNP_MODULE_ID,
	PRESENCE_MODULE_ID,
	LDAP_MODULE_ID,
	// VIDEO_CODEC_MODULE_ID,
	// WB_CODEC_MODULE_ID,
	VIDEO_DM_MODULE_ID,
	WB_DM_MODULE_ID,
	CRYPTO_MODULE_ID,
	CTI_MODULE_ID,
	SYSTEM_MODULE_ID,
	IPKT_NEGOTIATOR_MODULE_ID,

	LAST_MODULE_ID
};

extern PCSTR g_cpszModuleName[LAST_MODULE_ID];

#define	IPRANGER_CFG_MODULE_UTILITY		UTILITY_MODULE_ID
#define IPRANGER_CFG_MODULE_NETWORK		NETWORK_MODULE_ID
#define IPRANGER_CFG_MODULE_RTP			RTP_MODULE_ID
#define IPRANGER_CFG_MODULE_SDP			SDP_MODULE_ID
#define IPRANGER_CFG_MODULE_SIP			SIP_MODULE_ID
#define IPRANGER_CFG_MODULE_NEGOTIATER	NEGOTIATOR_MODULE_ID
#define IPRANGER_CFG_MODULE_DEVMGR		DM_MODULE_ID
#define IPRANGER_CFG_MODULE_CALLMGR		CM_MODULE_ID
#define IPRANGER_CFG_MODULE_ATTENDANT	ATTENDANT_MODULE_ID
#define IPRANGER_CFG_MODULE_DNS			DNS_MODULE_ID
#define IPRANGER_CFG_MODULE_STUN		STUN_MODULE_ID
#define IPRANGER_CFG_MODULE_CFGMGR		CFG_MODULE_ID
#define IPRANGER_CFG_MODULE_UI			UI_MODULE_ID
#define IPRANGER_CFG_MODULE_AUTOPROV	AUTOPROV_MODULE_ID
#define IPRANGER_CFG_MODULE_SNTP		SNTP_MODULE_ID
#define IPRANGER_CFG_MODULE_XML			XML_MODULE_ID
#define IPRANGER_CFG_MODULE_TELNET		TELNET_MODULE_ID
#define IPRANGER_CFG_MODULE_HTTP		HTTP_MODULE_ID
#define IPRANGER_CFG_MODULE_SNMP		SNMP_MODULE_ID
#define IPRANGER_CFG_MODULE_RESV		RESV_MODULE_ID
#define IPRANGER_CFG_MODULE_DSP			DSP_MODULE_ID
#define IPRANGER_CFG_MODULE_UPNP		UPNP_MODULE_ID
#define IPRANGER_CFG_MODULE_PRESENCE		IPRANGER_CFG_MODULE_TELNET
#define IPRANGER_CFG_MODULE_LDAP		LDAP_MODULE_ID
#define IPRANGER_CFG_MODULE_VIDEO_DM	VIDEO_DM_MODULE_ID
#define IPRANGER_CFG_MODULE_WB_DM		WB_DM_MODULE_ID
#define IPRANGER_CFG_MODULE_CRYPTO		CRYPTO_MODULE_ID
#define IPRANGER_CFG_MODULE_SYSTEM		SYSTEM_MODULE_ID

typedef enum {
	LogFatal = 0,	// non-recoverable 
	LogError,	// recoverable
	LogWarning,	// 
	LogEvent,
	LogInformative,
	LogTrace,	// medium volumn
	LogTrace2,
	LogTrace3,
	LogDetail,	// everything
	LogDebug // = ~ALL_MODULES
} LOG_LEVEL;

extern PCSTR g_cpszLogLevel[LogDebug+1];

#ifndef CURRENT_MODULE
	#define CURRENT_MODULE	RESV_MODULE_ID
#endif
 
#ifdef _WIN32
	#define	_FUNC_SIG_	__FUNCSIG__
	#define	_SRC_FILE_	__FILE__
	#define LOG_FUNC_SIG_HEADER PCSTR cpszSrcFile, int nLine, PCSTR cpszFuncSig
#elif defined(_LINUX_)
	#define	_FUNC_SIG_	__FUNCTION__
	#define	_SRC_FILE_	__FILE__
	#define LOG_FUNC_SIG_HEADER PCSTR cpszSrcFile, int nLine, PCSTR cpszFuncSig
#else
	#define	_FUNC_SIG_	__FUNCTION__
	#define	_SRC_FILE_	__FILE__
	#define LOG_FUNC_SIG_HEADER PCSTR cpszSrcFile, int nLine
	//#define LOG_FUNC_SIG_HEADER PCSTR cpszSrcFile, int nLine, PCSTR cpszFuncSig
#endif

#if (defined(_WIN32) || defined(_LINUX_))
	#define LOGSRC	_SRC_FILE_,__LINE__,_FUNC_SIG_,CURRENT_MODULE
#else
	#define LOGSRC	_SRC_FILE_,__LINE__,CURRENT_MODULE
	//#define LOGSRC	_SRC_FILE_,__LINE__,_FUNC_SIG_,CURRENT_MODULE
#endif

#define SYS_LOG_PORT	514	// udp
enum {
	SYSLOG_SEVERITY_EMERGENCY = 0, // system is unusable
	SYSLOG_SEVERITY_ALERT, // action must be taken immediately
	SYSLOG_SEVERITY_CRITICAL, // critical conditons
	SYSLOG_SEVERITY_ERROR,
	SYSLOG_SEVERITY_WARNING,
	SYSLOG_SEVERITY_NOTICE, // normal but significant conditions.
	SYSLOG_SEVERITY_INFORMATIONAL,
	SYSLOG_SEVERITY_DEBUG,

	SYSLOG_SEVERITY_MAX
};

#ifdef _LOGGER_
	#define SET_LOGGER_MASK(nBitMask)	_SetLoggerMask(nBitMask)// OR-able mask
	// File-Logger-Mask = 0x01 = 0x01 << FILE_LOGGER
	// TCP-Logger-Mask = 0x02 = 0x01<< TCP_LOGGER 
	// UDP-Logger-Mask = 0x04 = 0x01 << UDP_LOGGER
	// to block out the logger stream temporarily, set the corresponding bit to 1.
	// Note: it cannot block those logs of LogError and LogFatal.
	#define GET_LOGGER_MASK()			_GeLoggerMask()

	#define MKLOG(formatLog)	formatLog
	#define FORMAT_LOG	_FormatLogger
	// FORMAT_LOG(LOGSRC, nLogLevel, cpszFormat, ...)
	
	#define TRACE(nLogLevel) _Logger(LOGSRC, nLogLevel)
	#define LOG(nLogLevel, cpszMsg)	_Logger(LOGSRC, nLogLevel, cpszMsg, -1)
	#define LOG_N(nLogLevel, cpszMsg, nLen) _Logger(LOGSRC, nLogLevel, cpszMsg, nLen)
	#define LOG_HEX(nLogLevel, pbData, nLen) _HexLogger(LOGSRC, nLogLevel, pbData, nLen)
	#define SYSLOG(nSeverity, cpszMsg) _SysLog(__FILE__, __LINE__, CURRENT_MODULE, nSeverity, cpszMsg)
	extern "C" {
		void _FormatLogger(LOG_FUNC_SIG_HEADER, int nModule, LOG_LEVEL nLogLevel, PCSTR cpszFormat, ...);
		// printf-like logger without _SRC_FILE_, __LINE__ and _FUNC_SIG_ prefixed.
		void _Logger(LOG_FUNC_SIG_HEADER, int nModule, LOG_LEVEL nLogLevel, PCSTR cpszMsg=NULL, int nLen=-1);	
		// -1, log unitl end of string, nLen the length of string not including '\0';
		void _HexLogger(LOG_FUNC_SIG_HEADER, int nModule, LOG_LEVEL nLogLevel, PVOID pbData, int nLen); 
		// log unitl end of string, nLen the length of string not including '\0';
		void _SetLoggerMask(int nLoggerType);	// OR-able mask
		// File-Logger-Mask = 0x01 = 0x01 << FILE_LOGGER
		// TCP-Logger-Mask = 0x02 = 0x01<< TCP_LOGGER 
		// UDP-Logger-Mask = 0x04 = 0x01 << UDP_LOGGER
		int _GetLoggerMask();	// OR-able mask
		void _SysLog(PCSTR cpszSrcFile, int nLine, int nModule, int nSeverity, PCSTR cpszMsg);
	}
#else
#if !(defined(_WIN32) && defined(_GUI_))
		#define TRACE(nLogLevel)	
#endif
		#define MKLOG(formatLog)	
		#define LOG(nLogLevel, cpszMsg)		
		#define LOG_N(nLogLevel, cpszMsg, nLen)		
		#define LOG_HEX(nLogLevel, pbData, nLen)	
		#define SET_LOGGER_MASK(nLoggerType)		
		#define GET_LOGGER_MASK()	0xffffffff	// this is a return value
		#define SYSLOG(nSeverity, cpszMsg)
#endif 

// those are used by machine generated code 
#define xdbgTRACE(nLogLevel)
#define xdbgMKLOG(formatLog)	
#define xdbgLOG(nLogLevel, cpszMsg)	
#define xdbgLOG_N(nLogLevel, cpszMsg, nLen)
#define xdbgLOG_HEX(nLogLevel, pbData, nLen)

#ifdef _LOGGER_
	#define SETUP_MEMORY_EXHAUSTED_HANDLER(nPartID)	_SetupMemoryExhaustedHandler(LOGSRC, nPartID)
	#define REMOVE_MEMORY_EXHAUSTED_HANDLER	_RemoveMemoryExhaustedHandler(LOGSRC)
	void _SetupMemoryExhaustedHandler(LOG_FUNC_SIG_HEADER, int nModuleID, PART_ID nPartID);
	void _RemoveMemoryExhaustedHandler(LOG_FUNC_SIG_HEADER, int nModuleID);
	#if (defined(_LOG_MEMORY_USAGE_) || defined(_LOG_MEMROY_USAGE_))
		#define DUMP_MEMORY_LOG  _DumpMem(CURRENT_MODULE)
		#define COUNT_MEMORY_LOG  _CountMem()
		#define LOG_NEW_MEMORY(pMem, nSize, cpszDesc)	_LogNewMemory(LOGSRC, pMem, nSize, cpszDesc)
		#define LOG_DELETE_MEMORY(pMem, nSize, cpszDesc)	_LogDeleteMemory(LOGSRC, pMem, nSize, cpszDesc)
		extern "C" {
			void _DumpMem(int nModuleId);
			int _CountMem();
		}
		PVOID _LogNewMemory(LOG_FUNC_SIG_HEADER, int nModuleID, PVOID pMem, int nSize, PCSTR cpszDesc=NULL);
		void _LogDeleteMemory(LOG_FUNC_SIG_HEADER, int nModuleID, PVOID pMem, int nSize, PCSTR cpszDesc=NULL);
	#else
		#define DUMP_MEMORY_LOG 
		#define COUNT_MEMORY_LOG  
		#define LOG_NEW_MEMORY(pMem, nSize, cpszDesc)	pMem
		#define LOG_DELETE_MEMORY(pMem, nSize, cpszDesc)	pMem
		//#define SETUP_MEMORY_EXHAUSTED_HANDLER(nPartID)	
		//#define REMOVE_MEMORY_EXHAUSTED_HANDLER	
	#endif // _LOG_MEMROY_USAGE_
#else
	#define DUMP_MEMORY_LOG 
	#define COUNT_MEMORY_LOG  
	#define LOG_NEW_MEMORY(pMem, nSize, cpszDesc)	pMem
	#define LOG_DELETE_MEMORY(pMem, nSize, cpszDesc)	pMem
	#define SETUP_MEMORY_EXHAUSTED_HANDLER(nPartID)	
	#define REMOVE_MEMORY_EXHAUSTED_HANDLER	
#endif

class CConfigureOperation;	
typedef int (*PCFGCALLBACKFUNC)(CConfigureOperation* pCfgMgr, PVOID pCtx);

bool _SubscribeCfgEvent(PCFGCALLBACKFUNC pFunc, PVOID pCtx);
bool _SubscribeIPChangeEvent(PCFGCALLBACKFUNC pFunc, PVOID pCtx);
bool _UnsubscribeCfgEvent(PCFGCALLBACKFUNC pFunc);

#define CFG_CHANGE_NOT_CARE			0x0
#define CFG_CHANGE_SIPSRV_REBOOT	0xffffffff
#define CFG_CHANGE_SIP_REBOOT		(1<<1)
#define CFG_CHANGE_SIP_UNREGIST		(1<<2)	
#define CFG_CHANGE_SIP_REFRESH		(1<<3)
#define CFG_CHANGE_REFRESH_IP		(1<<4)

typedef enum {
	NET_UNKNOWN_PROTOCOL = -4,
	NET_UNKNOWN_PORT = -3,
	NET_UNKNOWN_HOST = -2,
	NET_ERROR = -1,
	NET_OK = 0,
	NET_TIMEOUT = 1
} NET_CODE;

#ifdef _LOADTEST_
	#define NOTIFY_AA(nEvt, nCtx) NotifyAutoAttendant(nEvt, nCtx);
	void NotifyAutoAttendant(int nEvt, int nCtx);
#else 
	#define NOTIFY_AA(nEvt, nCtx)	// NULL it
#endif

PROJECT_NS_END

#endif // __Common_h__
