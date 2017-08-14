#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
//#include <assert.h>
#ifdef WIN32
#elif defined(_LINUX_)
	#include <netinet/tcp.h> // for TCP_NODELAY in setsockopt()
#else
	#include <ioLib.h>//for close function;
	#include <netinet/tcp.h> // for TCP_NODELAY in setsockopt()
#endif
 
#include "SocketConnection.h"
#include "SockUtil.h"

PROJECT_NS_BEGIN

bool CSocketConnection::IsReadable(DWORD dwTimeout)
{	
	if ( m_bBroken )	return false;
	if( m_sk == INVALID_SOCKET )
	{	
		m_bBroken = true;
		MKLOG(FORMAT_LOG(LOGSRC,LogInformative,"INVALID_SOCKET,broken:%d",m_bBroken));
		return false;
	}
	
	fd_set  set;
	timeval tv;
	tv.tv_sec = dwTimeout /1000;  
	tv.tv_usec = (dwTimeout % 1000) * 1000;
	FD_ZERO(&set);
	FD_SET(m_sk, &set);
	int result ;
	if ( dwTimeout == 0)
		result = select(m_sk+1,&set,NULL,NULL,NULL);
	else
		if ( dwTimeout == 1 )
		{
			tv.tv_sec = 0;
			tv.tv_usec = 0;
			result = select(m_sk+1,&set,NULL,NULL,&tv);
		}
		else
			result = select(m_sk+1,&set,NULL,NULL,&tv);

//	result = select(m_sk+1,&set,NULL,NULL,&tv);
	if ( result == 0 )
	{	
		MKLOG(FORMAT_LOG(LOGSRC,LogTrace,"select time out,broken:%d",m_bBroken));
		return false;
	}
	
	if( result == SOCKET_ERROR )
	{
		m_bBroken = true;
		MKLOG(FORMAT_LOG(LOGSRC,LogTrace,"SOCKET_ERROR,broken:%d",m_bBroken));
		return false;
	}		

	MKLOG(FORMAT_LOG(LOGSRC,LogTrace,"select ok,broken:%d",m_bBroken));
	return true;	
}

CSocketConnection::CSocketConnection()
{
//	TRACE(LogDetail);	// commented for bug-142:netlogger restart deadlock	
	m_pSocketRendezvous = NULL;
	m_sk = INVALID_SOCKET;
	memset(&m_addrInTo, 0, sizeof(sockaddr_in));
	memset(&m_addrInFrom, 0, sizeof(sockaddr_in));
	m_lpfnSend = NULL;
	m_lpfnReceive = NULL;
	m_bBroken = true;
	m_bRaw = false;
	m_dwSendTimeoutsMs = 0; // blocking call
	m_dwReceiveTimeoutsMs = 0;	// blocking call
}

CSocketConnection::CSocketConnection(SOCKET s, struct sockaddr_in	addrIn)
{
//	TRACE(LogDetail);	// commented for bug-142:netlogger restart deadlock	
	m_pSocketRendezvous = NULL;
	m_bRaw = false;
	memset(&m_addrInFrom, sizeof(sockaddr_in), 0);
	Init(s, addrIn);
}

CSocketConnection::~CSocketConnection()
{
//	TRACE(LogDetail);	// commented for bug-142:netlogger restart deadlock
	Close();
}

void CSocketConnection::Init(SOCKET s, struct sockaddr_in	addrIn)
{
	m_pSocketRendezvous = NULL;
	m_sk = s;
	m_addrInTo = addrIn;
	
	
	m_lpfnSend = TcpSend;
	m_lpfnReceive = TcpReceive;
 
	m_bBroken = false;
	m_dwSendTimeoutsMs = 0;	// blocking call
	m_dwReceiveTimeoutsMs = 0;		// blocking call	
}

void CSocketConnection::Close()
{
	if (m_sk != INVALID_SOCKET) {
		/*
		if (m_pSocketRendezvous) {
			m_pSocketRendezvous->Remove(m_sk); // has been closed by CSocketRendezvous::Remove
			m_pSocketRendezvous->ReleaseReference();
			m_pSocketRendezvous = NULL;	
		}
		else 
		*/
#ifdef WIN32
			closesocket(m_sk);
#elif defined(_LINUX_)
			close(m_sk);
#else
			close(m_sk);
#endif
		m_sk = INVALID_SOCKET;
	}
	m_bRaw = false;
	m_bBroken = true;
}

bool CSocketConnection::GetAddress(bool bPeer, unsigned long& ulIP, unsigned short& ushPort)
{
	return GetSocketAddress(m_sk, bPeer, ulIP, ushPort);
}

//gwf,get local address and peer address  ,and then copy it into the pszBuf,
//the return value indicates the total length of the string ;
int	CSocketConnection::ToString(char* pszBuf, int nBufLen)
{
	char szPeer[64], szLocal[64];
	if (pszBuf && nBufLen > 0) *pszBuf = 0; // init
	GetSocketAddress(m_sk, true, szPeer);
	GetSocketAddress(m_sk, false, szLocal);
	int nLen = strlen(szPeer) + strlen(szLocal) + 24;
	if (nBufLen < nLen) return nLen;
	else sprintf(pszBuf, "Local = %s, Remote = %s", szLocal, szPeer);
	return strlen(pszBuf);
}

bool CSocketConnection::Send(DWORD dwLen, LPBYTE pData, DWORD dwTimeoutsMs)
{
#ifdef _WIN32
	if (dwTimeoutsMs != m_dwReceiveTimeoutsMs &&
		setsockopt(m_sk, SOL_SOCKET, SO_RCVTIMEO, (char*)&dwTimeoutsMs, sizeof(DWORD)) == 0)
		m_dwReceiveTimeoutsMs = dwTimeoutsMs;
#elif defined(_LINUX_)
	if (m_bBroken) return false;

	fd_set set;
	timeval tv;
	tv.tv_sec = dwTimeoutsMs / 1000;
	tv.tv_usec = (dwTimeoutsMs % 1000) * 1000;
	FD_ZERO(&set);
	FD_SET(m_sk, &set);
	int result;
	if (dwTimeoutsMs == 0) result = select(m_sk + 1, NULL, &set, NULL, NULL);
	else result = select(m_sk + 1, NULL, &set, NULL, &tv);
	// select: error
	if (result == ERROR) {
		m_bBroken = true;
		return false;
	}
	// select: time out
	if (result == 0 || !FD_ISSET(m_sk, &set)) return false;
	
#else	// vxworks
	if ( m_bBroken ) return false;//= false;
	fd_set  set;
	timeval tv;
	tv.tv_sec = dwTimeoutsMs / 1000;  
	tv.tv_usec = (dwTimeoutsMs % 1000)*1000;
	FD_ZERO(&set);
	FD_SET(m_sk, &set);
	int result ;
	if (dwTimeoutsMs == 0) result = select(m_sk+1,NULL,&set,NULL,NULL);
	else result = select(m_sk+1,NULL,&set,NULL,&tv);
	if ( result == ERROR )//select returns error
	{
		m_bBroken = true;
		return false;
	}
	if ( result == 0 || !FD_ISSET(m_sk,&set) )//time out
		return false;		
#endif

	bool bTimedout =  false;
	if (m_lpfnSend)	m_bBroken = !(*m_lpfnSend)(m_sk, dwLen, pData,(struct sockaddr*)&m_addrInTo, sizeof(struct sockaddr), bTimedout, m_bRaw);
	else m_bBroken = true;
	if (m_bBroken && bTimedout) { // for timeouted but not broken
		m_bBroken = false;
		return  false;
	}
	return !m_bBroken;
}

bool CSocketConnection::Receive( DWORD& dwLen, LPBYTE pData, DWORD dwTimeoutsMs)
{
#ifdef _WIN32
	if (dwTimeoutsMs != m_dwReceiveTimeoutsMs &&
		setsockopt(m_sk, SOL_SOCKET, SO_RCVTIMEO, (char*)&dwTimeoutsMs, sizeof(DWORD)) == 0)
		m_dwReceiveTimeoutsMs = dwTimeoutsMs;
#elif defined(_LINUX_)
	if (m_bBroken) return false;

	fd_set set;
	timeval tv;
	tv.tv_sec = dwTimeoutsMs / 1000;  
	tv.tv_usec = (dwTimeoutsMs % 1000)*1000;
	FD_ZERO(&set);
	FD_SET(m_sk, &set);
	int result;
	if (dwTimeoutsMs == 0) result = select(m_sk + 1, &set, NULL, NULL, NULL);
	else result = select(m_sk + 1, &set, NULL, NULL, &tv);
	// select: error
	if (result == ERROR) {
		m_bBroken = true;
		return false;
	}
	// select: time out
	if (result == 0 || !FD_ISSET(m_sk , &set))	return false;
#else	// vxworks
	if ( m_bBroken ) return false;//= false;
	fd_set  set;
	timeval tv;
	tv.tv_sec = dwTimeoutsMs / 1000;  
	tv.tv_usec = (dwTimeoutsMs % 1000)*1000;
	FD_ZERO(&set);
	FD_SET(m_sk, &set);
	int result ;
	if (dwTimeoutsMs == 0) result = select(m_sk+1,&set,NULL,NULL,NULL);
	else result = select(m_sk+1,&set,NULL,NULL,&tv);
	if ( result == ERROR )//select returns error
	{
		m_bBroken = true;
		return false;
	}
	if ( result == 0 || !FD_ISSET(m_sk,&set) )//time out
		return false;		
#endif
	
	int nFromAddrLen = sizeof(struct sockaddr);

	bool bTimedout = false;
	if (m_lpfnReceive) m_bBroken = !(*m_lpfnReceive)(m_sk, dwLen, pData,(struct sockaddr*)&m_addrInFrom, &nFromAddrLen, bTimedout, m_bRaw);
	else m_bBroken = true;
	if (m_bBroken && bTimedout) { // for timeouted but not broken
		m_bBroken = false;
		return  false;
	}
	return !m_bBroken;
}

bool CSocketConnection::RawReceive(DWORD& dwLen, LPBYTE pData, DWORD dwTimeoutsMs)
{
#ifdef _WIN32
	if (dwTimeoutsMs != m_dwReceiveTimeoutsMs &&
		setsockopt(m_sk, SOL_SOCKET, SO_RCVTIMEO, (char*)&dwTimeoutsMs, sizeof(DWORD)) == 0)
		m_dwReceiveTimeoutsMs = dwTimeoutsMs;
#elif defined(_LINUX_)
	if (m_bBroken) return false;

	fd_set  set;
	timeval tv;
	tv.tv_sec = dwTimeoutsMs / 1000;  
	tv.tv_usec = (dwTimeoutsMs % 1000)*1000;
	FD_ZERO(&set);
	FD_SET(m_sk, &set);
	int result ;
	if (dwTimeoutsMs == 0) result = select(m_sk + 1, &set, NULL, NULL, NULL);
	else result = select(m_sk + 1, &set, NULL, NULL, &tv);
	// select: error
	if (result == ERROR) {
		m_bBroken = true;
		return false;
	}
	// select: time out
	if (result == 0 || !FD_ISSET(m_sk, &set)) return false;
#else
	if ( m_bBroken )	return false;//= false;
	fd_set  set;
	timeval tv;
	tv.tv_sec = dwTimeoutsMs / 1000;  
	tv.tv_usec = (dwTimeoutsMs % 1000)*1000;
	FD_ZERO(&set);
	FD_SET(m_sk, &set);
	int result ;
	if (dwTimeoutsMs == 0) result = select(m_sk+1,&set,NULL,NULL,NULL);
	else result = select(m_sk+1,&set,NULL,NULL,&tv);
	if ( result == ERROR )//select returns error
	{
		m_bBroken = true;
		return false;
	}
	if ( result == 0 || !FD_ISSET(m_sk,&set) )//time out
		return false;
#endif

	int nFromAddrLen = sizeof(struct sockaddr);

	bool bTimedout = false;
	
	m_bBroken = !TcpReceive_1(m_sk,dwLen,pData,(struct sockaddr*)&m_addrInFrom,
		&nFromAddrLen, bTimedout, m_bRaw);

	if (m_bBroken && bTimedout) { // for timeouted but not broken
		m_bBroken = false;
		return  false;
	}
	return !m_bBroken;
}


int CSocketConnection::GetErrorNum()
{
	return WSAGetLastError();
}

bool CSocketConnection::SetTos(unsigned char chTos/*in*/) 
{
	if (m_sk == INVALID_SOCKET) return false;
	
	int tos = chTos;
	return (setsockopt(m_sk, IPPROTO_IP, IP_TOS, (char *)&tos, sizeof(int)) != SOCKET_ERROR);
}

bool CSocketConnection::GetTos(unsigned char& chTos/*out*/)
{
	if (m_sk == INVALID_SOCKET) return false;
	
	int tos, tos_len;
    tos_len = sizeof(tos);

#ifdef _LINUX_
	if (getsockopt(m_sk, IPPROTO_IP, IP_TOS, (char *)&tos, (socklen_t *)&tos_len) != SOCKET_ERROR)
#else
	if (getsockopt(m_sk, IPPROTO_IP, IP_TOS, (char *)&tos, &tos_len) != SOCKET_ERROR)
#endif
	{
		chTos = tos;
		return true;
	}
	else
		return false;
}
	
bool CSocketConnection::SetTtl(unsigned char chTtl/*in*/)
{
	if (m_sk == INVALID_SOCKET) return false;
	
	int ttl = chTtl;
	return (setsockopt(m_sk, IPPROTO_IP, IP_TTL, (char*)&ttl, sizeof(int)) != SOCKET_ERROR);
}

bool CSocketConnection::GetTtl(unsigned char& chTtl)
{
	if (m_sk == INVALID_SOCKET) return false;

	int ttl, ttl_len;
    ttl_len = sizeof(ttl);

#ifdef _LINUX_
	if (getsockopt(m_sk, IPPROTO_IP, IP_TTL, (char*)&ttl, (socklen_t *)&ttl_len) != SOCKET_ERROR)
#else
	if (getsockopt(m_sk, IPPROTO_IP, IP_TTL, (char*)&ttl, &ttl_len) != SOCKET_ERROR)
#endif
	{
		chTtl = ttl;
		return true;
	}
	else
		return false;
}

bool CSocketConnection::SetTcpNoDelay(bool bNoTcpDelay/* IN*/)
{
	if (m_sk == INVALID_SOCKET) return false;
	
	int nNoTcpDelay = bNoTcpDelay ? 1 : 0; // false
	return (setsockopt(m_sk, IPPROTO_TCP, TCP_NODELAY, (char*)&nNoTcpDelay, sizeof(int)) != SOCKET_ERROR);
}

bool CSocketConnection::GetTcpNoDelay(bool& bNoTcpDelay/* IN/OUT*/)
{
	if (m_sk == INVALID_SOCKET) return false;

	int nNoTcpDelay, nSize;
    nSize = sizeof(nSize);

#ifdef _LINUX_
	if (getsockopt(m_sk, IPPROTO_TCP, TCP_NODELAY, (char*)&nNoTcpDelay, (socklen_t *)&nSize) != SOCKET_ERROR)
#else
	if (getsockopt(m_sk, IPPROTO_TCP, TCP_NODELAY, (char*)&nNoTcpDelay, &nSize) != SOCKET_ERROR)
#endif
	{
		bNoTcpDelay = nNoTcpDelay != 0;
		return true;
	}
	else
		return false;
}

PROJECT_NS_END

