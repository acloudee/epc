#include "SocketRendezvous.h"
 
PROJECT_NS_BEGIN
 
CSocketRendezvous::CSocketRendezvous()
{
	TRACE(LogDetail);
	m_mutex.Create(false);
	m_mutex2.Create(false);
	m_ulReference = 0;
}

CSocketRendezvous::~CSocketRendezvous()
{
	TRACE(LogDetail);	
	Clear();

	m_mutex.Delete();
	m_mutex2.Delete();
}

LONG CSocketRendezvous::AddReference()
{ 
	m_mutex2.Take(INFINITE);
	m_ulReference++;
	m_mutex2.Give();
	return m_ulReference;
}

LONG CSocketRendezvous::ReleaseReference() 
{ 
	m_mutex2.Take(INFINITE);
	m_ulReference--;
	int nRef = m_ulReference;
	m_mutex2.Give();
	if (nRef <= 0) delete this;
	return nRef;
}

void CSocketRendezvous::Clear()
{
	m_mutex.Take(INFINITE);

	POSITION po;
	PVOID key,value;
	po = m_mapSocket.GetStartPosition();
	while (po != NULL) {
		m_mapSocket.GetNextAssoc(po,key,value);
		if ((SOCKET)value != INVALID_SOCKET ) 
#ifdef WIN32
			closesocket((SOCKET)value);
#elif defined(_LINUX_)
			close((SOCKET)value);
#else
			close((SOCKET)value);
#endif	
	}
	
	m_mapSocket.Clear();

	m_mutex.Give();
}

bool CSocketRendezvous::Remove(SOCKET s)
{
	if (s == INVALID_SOCKET) return false;

	bool bRc = false;

	m_mutex.Take(INFINITE);
	SOCKET sReturn;
	sReturn = (SOCKET)m_mapSocket.Get((PVOID)s);
	if (sReturn != NULL) {
		if (s != INVALID_SOCKET) 
#ifdef WIN32
			closesocket(s);
#elif defined(_LINUX_)
			close(s);
#else
			close(s);
#endif	
		m_mapSocket.Remove((PVOID)s);		

		bRc = true;
	}
	m_mutex.Give();

	return bRc;
}

bool CSocketRendezvous::Add(SOCKET s)
{
	if (s == INVALID_SOCKET) return false;


	m_mutex.Take(INFINITE);

	//m_mapSocket.insert(CMapDW2DW::value_type(s, s));
	m_mapSocket.Put((PVOID)s,(PVOID)s);

	m_mutex.Give();


	return true;
}

PROJECT_NS_END
