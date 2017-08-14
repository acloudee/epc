#ifndef __NtpTime_h__
#define __NtpTime_h__

#include  "Common.h"

PROJECT_NS_BEGIN

class NtpTime
{
public:
	NtpTime(u_int sec, u_int fraction ): seconds(sec), fractional(fraction) {}
    NtpTime();	//{ *this = _GetNtpTime(); };
	~NtpTime() {}

	inline u_int getSeconds() { return seconds; };
	inline u_int getFractional() { return fractional; }
	inline void Reset(u_int sec, u_int fraction ) { seconds =sec, fractional = fraction; }
	void Now();	// { *this = _GetNtpTime(); }

private:
    u_int seconds;
    u_int fractional;

    friend NtpTime operator+( const NtpTime& rhs , u_int delayMs );
    friend NtpTime operator-( const NtpTime& rhs , u_int ms );
    friend int operator-( const NtpTime& rhs , const NtpTime& lhs );
    friend bool operator==( const NtpTime& rhs , const NtpTime& lhs );
    friend bool operator<( const NtpTime& rhs , const NtpTime& lhs );
    friend bool operator>( const NtpTime& rhs , const NtpTime& lhs );
    friend NtpTime _GetNtpTime();
};

NtpTime _GetNtpTime();

PROJECT_NS_END

#endif // __NtpTime_h__
