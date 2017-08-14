#ifndef __SocketRendezvous_h__
#define __SocketRendezvous_h__

#ifdef WIN32
#pragma warning (disable : 4786)
#include <winsock2.h>
#elif defined(_LINUX_)
#include "NonWinType.h"
#else
#include <socket.h>
#include <socklib.h>
#include <inetlib.h>
#include <types.h>
#include <ioLib.h>
#include <semLib.h>
#include "NonWinType.h"
#endif 

#include "../../Utility/H/Mutex.h"
#include "../../Utility/H/HashMap.h"

PROJECT_NS_BEGIN

class CSocketRendezvous
{
public:
	CSocketRendezvous();
	virtual ~CSocketRendezvous();
	void Clear();
	bool Remove(SOCKET s);
	bool Add(SOCKET s);
	LONG AddReference(); 
	LONG ReleaseReference();
	inline LONG GetReference() { return m_ulReference;}

private:
	CMutex m_mutex;
	CMutex m_mutex2;
	LONG m_ulReference;
	CHashMap  m_mapSocket;	
};

PROJECT_NS_END

#endif // __SocketRendezvouse_h__
