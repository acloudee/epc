#ifndef __DateTime_h__
#define __DateTime_h__

#include "../../Utility/H/StringList.h"

PROJECT_NS_BEGIN

class CDateTime //Date: Sat, 13 Nov 2010 23:29:00 GMT
	//	XML: datetime: yyyy-mm-ddTmm:dd:ss(.sss)?ZZZZ
{
public:
	typedef struct {
		PCSTR cpszName;
		short nOffset;	// in minutes
	} TIMEZONE;

	static PCSTR s_cpszDayofWeek[7]; // "Mon" / "Tue" / "Wed"/ "Thu" / "Fri" / "Sat" / "Sun"
	static PCSTR s_cpszMonth[12]; //month = "Jan" / "Feb" / "Mar" / "Apr"/ "May" / "Jun" / "Jul" / "Aug"/ "Sep" / "Oct" / "Nov" / "Dec"
	static void _TmToSystime(const struct tm& tmTime, SYSTIME& systime);
	static void _SystimeToTm(const SYSTIME& systime, struct tm& tmTime);
	static bool _IsSystimeEqual(const SYSTIME& systime1, const SYSTIME& systime2, bool bMs);
	static bool _ValidateSystime(const SYSTIME& systime);
	// return true if all members of systime are in range

	static bool _IsLeapYear(int nYear);
	
	static const TIMEZONE* _FindTimeZone(PCSTR cpszName);

	static int _GetTimeZone(PCSTR cpszName); 
	// cpszName	time zone name to search for, case IN-sensitive.
	// return	-1 if not found, 
	//			otherwise return offset, in minutes, from GMT time; 
	//			such as -480 for GMT-8:00, and 210 for GMT+3:30
	static PCSTR _GetTimeZone(int nOffsetMin);
	// nOffsetMin	offset, in minutes, from GMT time to search for
	//			such as -480 for GMT-8:00, and 210 for GMT+3:30
	// return  defined time zone name, NULL if not found
	static CStringList _GetAllTimeZone(int nOffset);
	// nOffsetMin	offset, in minutes, from GMT time to search for
	//			such as -480 for GMT-8:00, and 210 for GMT+3:30
	// return  defined time zone name list, empty string list if not found

	static TIME_T _ToGMT(const SYSTIME& systime, int nOffsetFromGMTMin);
	static TIME_T _ToGMT(const SYSTIME& systime, PCSTR cpszTimeZone);
	// return GMT sec by converting systime from cpszTimeZone to GMT
	static void _AdjustTimeZone(SYSTIME& systime, PCSTR cpszFromTZ, PCSTR cpszToTZ);
	// systime[IN/OUT]	change time from cpszFromTZ to cpszToTZ
	// cpszFromTZ		pass NULL as GMT
	// cpszToTZ		pass NULL as GMT
	// return cpszToTZ sec by converting systime from cpszTimeZone to cpszToTZ
	static void _CalculateTime(SYSTIME& systime, int nOffsetMin);
	// converting systime by add nOffsetMin
	
public:
    /// Create one with default values
    CDateTime(bool bCurrent, int nTimeZoneOffsetFromGMTMin);
    CDateTime(bool bCurrent, PCSTR cpszTimeZone);
    CDateTime(const SYSTIME& systime, int nTimeZoneOffsetFromGMTMin);
    CDateTime(const SYSTIME& systime, PCSTR cpszTimeZone);
    CDateTime(PCSTR pszDate);
    CDateTime(const CDateTime& srcDate);
	virtual ~CDateTime() {} // has virtual function should declare virtual destructor.

	void Empty();
	bool AdjustToTimeZone(PCSTR cpszTimezone);
	bool AdjustToTimeZone(int nTimeZoneOffsetFromGMTMin);

	inline String GetTimeZone() { return m_strTimeZone; }
	inline void SetTimeZone(PCSTR cpszTimeZone) { m_strTimeZone = cpszTimeZone; m_strTimeZone.Trim(); }
	inline bool IsValid() { return m_bValid; }
	inline SYSTIME	GetDateTime() { return m_systime;}

    ///
    virtual bool operator ==(const CDateTime& srcDate) const;
    virtual bool operator <(const CDateTime& srcDate) const;
	virtual bool operator > (const CDateTime& srcDate) const;
	virtual bool operator != (const CDateTime& srcDate) const;
    virtual const CDateTime& operator=(const CDateTime& srcDate);

    virtual bool operator ==(const SYSTIME& srcTime) const;
    virtual bool operator <(const SYSTIME& srcTime) const;
	virtual bool operator > (const SYSTIME& srcTime) const;
	virtual bool operator != (const SYSTIME& srcTime) const;
    virtual const CDateTime& operator=(const SYSTIME& srcTime);

	/*** return the encoded string version of this. This call
	 should only be used inside the stack and is not part of
	 the API */
    virtual String ToString(PCSTR cpszTimeZone) const;
	//Mon, 10 Jul 2000 03:55:06 GMT 

protected:
	static TIMEZONE s_tzTimeZones[];
	static int s_nDayOfMonths[];

	bool Parse(PCSTR pszVal);
	virtual inline bool Validate() { return true;}
	// call after Parse

    bool m_bValid;
	SYSTIME	m_systime;
	String m_strTimeZone;
};


PROJECT_NS_END

#endif // __DateTime_h__
