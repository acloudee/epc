#ifndef __Platform_h__
#define __Platform_h__

#include "Common.h"


PROJECT_NS_BEGIN

#ifdef _MMPHONE_
	int InitPlatform(PVOID pContext);	// create singletons of logger, cfgmgr and modulereg (in that order)
#else
	int InitPlatform(PART_ID pContext);	// create singletons of logger, cfgmgr and modulereg (in that order)
#endif
// return non-zero on success; otherwise an zero is returned.
void ShutdownPlatform();// destroy singletons of logger, cfgmgr and modulereg (in that order)

void GetSysMicroTime(long& lSec,long& lUsec);
//	int	gettimeofday (struct timeval *tv);
// return current time in micro-seconds since 1970/1/1

// Get local calendar and time
void GetCalendarTime(PSYSTIME pSystemTime); 
// Set local calendar and time
// wDayOfWeek is ignored; 
bool SetCalendarTime(const PSYSTIME pSystemTime); 

u_long GetPlatformStartupDuration(); 
// return the system start up duration in seconds;

char* ASCTIME(const struct tm * cpTm,    /* broken-down time */
		char* pszAscTimeBuf, /* buffer to contain string */
		int nBufLen);      /* size of buffer */
// return The size of the created string.
// This routine converts the broken-down time pointed to 
// by cpTm into a string of the form: 
// SUN SEP 16 01:03:52 1973\n\0
// return pszAscTimeBuf

struct tm* GMTIME(const TIME_T* cpTimer,
			struct tm* pTmBuffer);
// cross platfor gmtime()
// This routine converts the calendar time pointed to by cpTimer into 
// broken-down time, expressed as Coordinated Universal Time (UTC).
// return pTmBuffer;

struct tm* LOCALTIME(const TIME_T* cpTimer,     /* calendar time in seconds */
				struct tm*   pTmBuffer);/* buffer for the broken-down time */

// cross platfor localtime()
// This routine converts the calendar time pointed to by cpTimer 
// into broken-down time, expressed as local time. 
// The broken-down time is stored in pTmBuffer. 
// return	pTmBuffer

char* CTIME(const TIME_T* cpTimer,      /* calendar time in seconds */
			char*  pszAscTimeBuf, /* buffer to contain the string */
			int nBufLen);      /* size of the buffer */
// cross platfor ctime()
// This routine converts the calendar time pointed to by timer into 
// local time in the form of a string. It is equivalent to: 
// asctime (localtime (timer));
// return  The pointer returned by pszAscTimeBuf
//	with local broken-down time as the argument. 

#ifdef _LINUX_
	TIME_T	_TIME(TIME_T* t);
    // cross platfor time()
#endif

void REBOOT();	
// revoke os's reboot routines directly

PROJECT_NS_END 

#endif // #ifndef __Platform_h__
