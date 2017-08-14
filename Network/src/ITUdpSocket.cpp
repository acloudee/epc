#include "ITUdpSocket.h"
#include "SockUtil.h"
#include <stdlib.h>
#include <errno.h>

PROJECT_NS_BEGIN

CITUdpSocket::CITUdpSocket()
{
	m_nListenPort = 0;
	m_socket = INVALID_SOCKET;
	m_socket = socket(AF_INET, SOCK_DGRAM,0);
//	assert( m_socket != INVALID_SOCKET );
	memset(m_strDestIp,0,100);
	m_nDestIp = 0;
	m_mtxSend.Create(false);
}

CITUdpSocket::~CITUdpSocket()
{
	TRACE(LogDetail);
	Close();
	m_mtxSend.Delete();
}

bool CITUdpSocket::Init(PCSTR cpszListenPort, bool bBroadcast)
{
	if ( cpszListenPort == NULL )
		return Init((u_short)0, bBroadcast);
	else
		return Init( (u_short)atoi(cpszListenPort), bBroadcast);
}

bool CITUdpSocket::Init(unsigned short ushListenPort, bool bBroadcast)
{
	if (m_socket == INVALID_SOCKET)	return false;

	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	if (ushListenPort == 0) { // bind to any port
		sin.sin_port = htons(0);
		int result = bind(m_socket,(sockaddr*)&sin,sizeof(sin));
		if (result != 0) return false;
		sockaddr_in sin_temp;
		int len = sizeof(sin_temp);
#ifdef _LINUX_
		result = getsockname(m_socket, (sockaddr *)&sin_temp, (socklen_t *)&len);
#else		
		result = getsockname(m_socket,(sockaddr*)&sin_temp,&len);
#endif
		if (result != 0) 
		{
			MKLOG( FORMAT_LOG(LOGSRC,LogError,"getsockname failed,errno=0x%x", GetErrorNum()));	
			return false;
		}
		m_nListenPort = sin_temp.sin_port;
		ushListenPort = ntohs(m_nListenPort);
	}
	else {
		m_nListenPort = htons(ushListenPort);
		sin.sin_port = htons(ushListenPort);
		int result = bind(m_socket,(sockaddr*)&sin,sizeof(sin));
		if (result != 0)
		{
			MKLOG( FORMAT_LOG(LOGSRC,LogError,"bind UDP-%d failed, errno=0x%x", ushListenPort, GetErrorNum()));
			return false;
		}
	}

	int nOpt = bBroadcast ? 1 : 0;
	if (setsockopt(m_socket, SOL_SOCKET, SO_BROADCAST, (char*)&nOpt, sizeof(int)) == SOCKET_ERROR) {
		int nErrorNo = GetErrorNum();
		MKLOG(FORMAT_LOG(LOGSRC, LogError, "Error! UDP-%d failed to turn SO_BROADCAST option %s! Error-No = 0x%x (%d)", ushListenPort, bBroadcast ? "on" : "off", nErrorNo, nErrorNo));
		return false;
	}
	else if (bBroadcast) 
		MKLOG(FORMAT_LOG(LOGSRC, LogInformative, "UDP-%d turn SO_BROADCAST option on", ushListenPort));

	return true;
}

bool CITUdpSocket::JoinGroup(PCSTR cpszMulticastIP)
{
	if (m_socket == INVALID_SOCKET || !cpszMulticastIP || *cpszMulticastIP == 0) return false;

	struct ip_mreq  ipMreq;  
	/* fill in the argument structure to join the multicast group */ 
	/* initialize the multicast address to join */ 
	if ((ipMreq.imr_multiaddr.s_addr = inet_addr((PSTR)cpszMulticastIP)) == INADDR_NONE) {
		MKLOG(FORMAT_LOG(LOGSRC, LogError, "Error! UDP-%d failed to IP_ADD_MEMBERSHIP into invalid multicast group %s", GetListenPort(), cpszMulticastIP));
		return false;
	}
	 /* unicast interface addr from which to receive the multicast packets */ 
	ipMreq.imr_interface.s_addr = INADDR_ANY; // inet_addr (ifAddr); 
	/* set the socket option to join the MULTICAST group */ 

	if (m_szlstGroup.Find(cpszMulticastIP)) {
		MKLOG(FORMAT_LOG(LOGSRC, LogWarning, "Warning! UDP-%d has been IP_ADD_MEMBERSHIP into multicast group %s", GetListenPort(), cpszMulticastIP));
		return true;	// existed already
	}
	else if (setsockopt(m_socket, IPPROTO_IP, IP_ADD_MEMBERSHIP,  (char*)&ipMreq, sizeof(ip_mreq)) == SOCKET_ERROR) {
		int nErrorNo = GetErrorNum();
		MKLOG(FORMAT_LOG(LOGSRC, LogError, "Error! UDP-%d failed to IP_ADD_MEMBERSHIP into multicast group %s! Error-No = 0x%x (%d)", GetListenPort(), cpszMulticastIP, nErrorNo, nErrorNo));
		return false;
	}
	else {
		m_szlstGroup.AddTail(cpszMulticastIP);
		MKLOG(FORMAT_LOG(LOGSRC, LogInformative, "UDP-%d joins into multicast group %s", GetListenPort(), cpszMulticastIP)); 
		return true;
	}
}

int CITUdpSocket::LeaveGroup(PCSTR cpszMulticastIP)
{
	if (m_socket == INVALID_SOCKET) return 0;

	int nRc = 0;
	struct ip_mreq  ipMreq;  
	 /* unicast interface addr from which to receive the multicast packets */ 
	ipMreq.imr_interface.s_addr = INADDR_ANY; // inet_addr (ifAddr); 

	POSITION pos;	
	if (cpszMulticastIP) pos = m_szlstGroup.Find(cpszMulticastIP);
	else pos = m_szlstGroup.GetHeadPosition();
	while (pos) {
		String szGroup;
		 if (cpszMulticastIP) szGroup = m_szlstGroup.RemoveAt(pos);
		 else szGroup = m_szlstGroup.GetNext(pos); 
		/* set the socket option to leave the MULTICAST group */ 
		if ((ipMreq.imr_multiaddr.s_addr = inet_addr((PSTR)(PCSTR)szGroup)) != INADDR_NONE) {
			/* set the socket option to join the MULTICAST group */ 
			if (setsockopt(m_socket, IPPROTO_IP, IP_DROP_MEMBERSHIP,  (char*)&ipMreq, sizeof(ip_mreq)) != SOCKET_ERROR) {
				nRc++;
				MKLOG(FORMAT_LOG(LOGSRC, LogInformative, "UDP-%d leaves multicast group %s", GetListenPort(), (PCSTR)szGroup)); 
			}
			else {
				int nErrorNo = GetErrorNum();
				MKLOG(FORMAT_LOG(LOGSRC, LogError, "Error! UDP-%d failed to IP_DROP_MEMBERSHIP from multicast group %s! ErrorNo=0x%x (%d)", GetListenPort(), (PCSTR)szGroup, nErrorNo, nErrorNo));
			}
		} 
		else 
			MKLOG(FORMAT_LOG(LOGSRC, LogWarning, "Warning! UDP-%d failed to IP_DROP_MEMBERSHIP from invalid multicast group %s", GetListenPort(), (PCSTR)szGroup));
	
		if (cpszMulticastIP) break; // done
		else continue;	// remove all
	}

	if (!cpszMulticastIP) m_szlstGroup.RemoveAll();
	
	return nRc;
}

void CITUdpSocket::Close()
{
	if (m_socket != INVALID_SOCKET) {
		LeaveGroup(NULL);
#ifdef WIN32
			closesocket(m_socket);
#elif defined(_LINUX_)
			close(m_socket);
#else
			close(m_socket);
		#endif
		m_socket = INVALID_SOCKET;	
	}
}

NET_CODE CITUdpSocket::Send(u_long ulDestIP, u_short ushDestPort, 
							DWORD dwLen, LPBYTE pData, 
							DWORD dwTimeoutsMs/*=30*1000*/)
{
	NET_CODE nRc = NET_OK; // init
	m_mtxSend.Take(INFINITE);

	if (m_socket == INVALID_SOCKET) nRc = NET_ERROR; // return NET_ERROR;
	else if (dwLen > 65535) nRc = NET_TIMEOUT; 
	else {
		sockaddr_in sin;
		sin.sin_family = AF_INET;
		sin.sin_addr.s_addr = ulDestIP;
		sin.sin_port = htons(ushDestPort);
		int len = sizeof(sin);

		fd_set  set;
		timeval tv;
		tv.tv_sec = dwTimeoutsMs / 1000;  
		tv.tv_usec = (dwTimeoutsMs % 1000)*1000;
		FD_ZERO(&set);
		FD_SET(m_socket, &set);
		int result ;
		if (dwTimeoutsMs == 0) result = select(m_socket+1,NULL,&set,NULL,NULL);
		else if (dwTimeoutsMs == 1)	{
			tv.tv_sec = 0;
			tv.tv_usec = 0;
			result = select(m_socket+1,NULL,&set,NULL,&tv);
		}
		else result = select(m_socket+1,NULL,&set,NULL,&tv);

		if (result == 0 || !FD_ISSET(m_socket,&set))//time out
			nRc = NET_TIMEOUT; // return NET_TIMEOUT;		
		
	#ifdef WIN32
		if (sendto(m_socket, (const char *)pData, dwLen, 0, (sockaddr*)&sin, len) == SOCKET_ERROR) 
	#elif defined(_LINUX_)
		if (sendto(m_socket, (const char *)pData, dwLen, 0, (sockaddr *)&sin, len) == SOCKET_ERROR) 
	#else
		if (sendto(m_socket, (caddr_t)pData, dwLen, 0, (sockaddr*)&sin, len) == SOCKET_ERROR) 
	#endif
		{
#define SOCK_ERR_NO_BUF_SPACE_AVAILABLE	0x37	// 55
			int nSockErr = WSAGetLastError();
			PCSTR cpszError;
			if (nSockErr == SOCK_ERR_NO_BUF_SPACE_AVAILABLE) // 55
				cpszError = "No buffer space available";
			else cpszError = _GetErrorMsg(nSockErr);
			if (nSockErr == EINTR || nSockErr == SOCK_ERR_NO_BUF_SPACE_AVAILABLE) {// 
				MKLOG(FORMAT_LOG(LOGSRC, LogWarning, "!!!sendto(socket=0x%x, IP=0x%08x, port=%d, dwLen=%d) failed: WSAGetLastError()=0x%x(%d, %s) => Ignored!!!", m_socket, ulDestIP, ushDestPort, dwLen, nSockErr, nSockErr, cpszError));
				nRc = NET_TIMEOUT;
			}
			else {
	            MKLOG(FORMAT_LOG(LOGSRC, LogError, "!!!sendto(socket=0x%x,IP=0x%08x, port=%d, dwLen=%d) failed: WSAGetLastError()=0x%x(%d, %s)!!!", m_socket, ulDestIP, ushDestPort, dwLen, nSockErr, nSockErr, cpszError));
				nRc = NET_ERROR;// return NET_ERROR;
			}
		}
		else nRc = NET_OK; // return NET_OK;
	}

	m_mtxSend.Give();

	return nRc;
}

NET_CODE CITUdpSocket::Send(PCSTR cpszDestAddr, u_short ushDestPort,
							DWORD dwLen, LPBYTE pData, 
                            DWORD dwTimeoutsMs/*=30*1000*/)
{
	if (cpszDestAddr == NULL || *cpszDestAddr == 0) return NET_UNKNOWN_HOST;
	else if (ushDestPort == 0) return NET_UNKNOWN_PORT;
	else {
		int nLen = strlen(cpszDestAddr);
		if (nLen >= sizeof(m_strDestIp)) return NET_UNKNOWN_HOST;
	}

	u_long ulIP;
	m_mtxSend.Take(INFINITE);
	if (strcmp(m_strDestIp, cpszDestAddr) == 0) // skip DNS query
		ulIP = m_nDestIp;
	else {
		#ifdef WIN32
			hostent* phe = gethostbyname(cpszDestAddr);	
			if (phe) ulIP = ((in_addr*)phe->h_addr)->s_addr;
		#elif defined(_LINUX_)
			hostent* phe = gethostbyname(cpszDestAddr);
			if (phe) ulIP = ((in_addr*)phe->h_addr)->s_addr;
		#else
			ulIP = hostGetByName((char*)cpszDestAddr);
		//	CDnsRunner* pDnsRunner = CDnsRunner::_GetInstance();
		//	if (pDnsRunner) ulIP = pDnsRunner->HostGetByName((char*)cpszDestAddr);
		//	else ulIP = (unsigned long)ERROR;
			if (ulIP != (unsigned long)ERROR) ;	// 
		#endif

		else if ((ulIP=inet_addr((PSTR)cpszDestAddr)) == INADDR_NONE && 
			strcmp(cpszDestAddr, "255.255.255.255") !=0) {
			m_mtxSend.Give();
			return NET_UNKNOWN_HOST;	// unkown host
		}
				
		m_nDestIp = ulIP; // cache it for later skip DNS reQuery
		strcpy(m_strDestIp, cpszDestAddr);
	}
	m_mtxSend.Give();
	
	return Send(ulIP, ushDestPort, dwLen, pData, dwTimeoutsMs);
}

NET_CODE CITUdpSocket::Receive(u_long& ulIP, u_short& ushSrcPort,
							   DWORD& dwLen, PBYTE pData,
							   DWORD dwTimeoutsMs/*=30*1000*/)
{
	if (m_socket == INVALID_SOCKET) return NET_ERROR;
	
	fd_set  set;
	timeval tv;
	tv.tv_sec = dwTimeoutsMs / 1000;  
	tv.tv_usec = (dwTimeoutsMs % 1000)*1000;
	FD_ZERO(&set);
	FD_SET(m_socket, &set);
	int result ;
	if ( dwTimeoutsMs == 0)
		result = select(m_socket+1,&set,NULL,NULL,NULL);
	else
		if (dwTimeoutsMs == 1)	{
			tv.tv_sec = 0;
			tv.tv_usec = 0;
			result = select(m_socket+1,&set,NULL,NULL,&tv);
		}
		else
			result = select(m_socket+1,&set,NULL,NULL,&tv);

	if ( result == 0 || !FD_ISSET(m_socket,&set) )//time out
		return NET_TIMEOUT;	
	
	sockaddr_in sin;
	int len = sizeof(sin);
	char szBuffer[256];
	int nBufSize = sizeof(szBuffer);
	char* pszBuffer = szBuffer;
	if (dwLen > 0 && pData) {
		pszBuffer = (char*)pData;
		nBufSize = (int)dwLen;
	}

#ifdef _LINUX_
	int nRxLen = recvfrom(m_socket, pszBuffer, nBufSize, 0, (struct sockaddr *)&sin, (socklen_t *)&len);
#else
	int nRxLen = recvfrom(m_socket, pszBuffer, nBufSize, 0 ,(struct sockaddr*) &sin, &len);
#endif
	if (nRxLen == SOCKET_ERROR) {
		int nSockErr = WSAGetLastError();
		PCSTR cpszError = _GetErrorMsg(nSockErr);
		if (nSockErr ==  ECONNRESET) {
			ulIP = sin.sin_addr.s_addr;
			ushSrcPort = ntohs(sin.sin_port);
			MKLOG(FORMAT_LOG(LOGSRC, LogError, "!!!recvfrom(socket=0x%x, BufSize=%d) failed: nRxLen=0x%x(%d), WSAGetLastError()=0x%x(%d, %s) => set RxIP=0x%08x, RxPort=%d!!!", 
				m_socket, nBufSize, nRxLen, nRxLen, nSockErr, nSockErr, cpszError, ulIP, ushSrcPort));
			return NET_ERROR;
		}
		else if (nSockErr == EINTR) {
			MKLOG(FORMAT_LOG(LOGSRC, LogWarning, "!!!recvfrom(socket=0x%x, BufSize=%d) failed: nRxLen=0x%x(%d), WSAGetLastError()=0x%x(%d, %s) => Ignored!!!", m_socket, nBufSize, nRxLen, nRxLen, nSockErr, nSockErr, cpszError));
			dwLen = 0;
			return NET_TIMEOUT;
		}
		else {
            MKLOG(FORMAT_LOG(LOGSRC, LogError, "!!!recvfrom(socket=0x%x, BufSize=%d) failed: nRxLen=0x%x(%d), WSAGetLastError()=0x%x(%d, %s)!!!", m_socket, nBufSize,  nRxLen, nRxLen, nSockErr, nSockErr, cpszError));
			return NET_ERROR;
		}
	}

	if (nRxLen <= (int)dwLen && pData) {
		//memcpy(pData, szBuffer, nRxLen);
		ulIP = sin.sin_addr.s_addr;
		ushSrcPort = ntohs(sin.sin_port);
		dwLen = nRxLen;
		return NET_OK;
	} 
	else {
		dwLen = nRxLen;	// set RxLen
		return NET_TIMEOUT;			//	else if (nRxLen == 0) bRc = true;// closed gracefully.
	}
}

unsigned short CITUdpSocket::GetListenPort()
{
	return ntohs(m_nListenPort);
}

bool CITUdpSocket::SetTos(unsigned char chTos/*in*/) 
{
	if (m_socket == INVALID_SOCKET) return false;
	
	int tos = chTos;
	if (setsockopt(m_socket, IPPROTO_IP, IP_TOS, (char *)&tos, sizeof(int)) != SOCKET_ERROR)
		return true;
	else
		return false;
}

bool CITUdpSocket::GetTos(unsigned char& chTos/*out*/)
{
	if (m_socket == INVALID_SOCKET)
		return false;
	
	int tos, tos_len;
	tos_len = sizeof(tos);

#ifdef _LINUX_
	if (getsockopt(m_socket, IPPROTO_IP, IP_TOS, (char *)&tos, (socklen_t *)&tos_len) != SOCKET_ERROR) {
		chTos = tos;
		return true;
	}
	else return false;
#else
	if (getsockopt(m_socket, IPPROTO_IP, IP_TOS, (char *)&tos,
                  &tos_len) != SOCKET_ERROR) {
		chTos = tos;
		return true;
	}
	else
		return false;
#endif
}
	
bool CITUdpSocket::SetTtl(unsigned char chTtl/*in*/)
{
	if (m_socket == INVALID_SOCKET) return false;
	
	int ttl = chTtl;
	
	bool bRc = false;
	if (setsockopt(m_socket, IPPROTO_IP, IP_TTL, (char *)&ttl, sizeof(int)) == SOCKET_ERROR) {
		int nErrorNo = GetErrorNum();
		MKLOG(FORMAT_LOG(LOGSRC, LogError, "Error! UDP-%d failed to set IP_TTL! Error-No = 0x%x (%d, %s)!", GetListenPort(), nErrorNo, nErrorNo, _GetErrorMsg(nErrorNo)));
	}
	else bRc = true;

	if (m_szlstGroup.GetCount() > 0 &&
		setsockopt(m_socket, IPPROTO_IP, IP_MULTICAST_TTL, (char *)&ttl, sizeof(int)) == SOCKET_ERROR) {
		int nErrorNo = GetErrorNum();
		MKLOG(FORMAT_LOG(LOGSRC, LogError, "Error! UDP-%d failed to set IP_MULTICAST_TTL! Error-No = 0x%x (%d, %s)!", GetListenPort(), nErrorNo, nErrorNo, _GetErrorMsg(nErrorNo)));
		bRc = false;
	}

	return bRc;
}

bool CITUdpSocket::GetTtl(unsigned char& chTtl)
{
	if ( m_socket == INVALID_SOCKET )
		return false;

	int ttl, ttl_len = sizeof(int);

	bool bRc = false;
	if (m_szlstGroup.GetCount() > 0) {
#ifdef _LINUX_
		bRc = getsockopt(m_socket, IPPROTO_IP, IP_MULTICAST_TTL, (char *)&ttl, (socklen_t *)&ttl_len) != SOCKET_ERROR;
#else
		bRc = getsockopt(m_socket, IPPROTO_IP, IP_MULTICAST_TTL, (char*)&ttl, &ttl_len) != SOCKET_ERROR;
#endif
		if (!bRc) {
			int nErrorNo = GetErrorNum();
			MKLOG(FORMAT_LOG(LOGSRC, LogError, "Error! UDP-%d failed to get IP_MULTICAST_TTL! Error-No = 0x%x (%d, %s). Trying IP_TTL instead!", GetListenPort(), nErrorNo, nErrorNo, _GetErrorMsg(nErrorNo)));
		}
	}
	// dont use else to let multicast fall through
#ifdef _LINUX_
	if (!bRc) bRc = getsockopt(m_socket, IPPROTO_IP, IP_TTL, (char *)&ttl, (socklen_t *)&ttl_len) != SOCKET_ERROR;
#else
	if (!bRc) bRc = getsockopt(m_socket, IPPROTO_IP, IP_TTL, (char*)&ttl, &ttl_len) != SOCKET_ERROR;
#endif
	
	if (bRc) {
		chTtl = ttl;
		return true;
	}
	else {
		int nErrorNo = GetErrorNum();
		MKLOG(FORMAT_LOG(LOGSRC, LogError, "Error! UDP-%d failed to get IP_TTL! Error-No = 0x%x (%d, %s)", GetListenPort(), nErrorNo, nErrorNo, _GetErrorMsg(nErrorNo)));
		return false;
	}
}

bool CITUdpSocket::SetReuseAddr(bool bReuseAddr/*in*/) 
{
	if (m_socket == INVALID_SOCKET)	return false;
	
	int nOptval = bReuseAddr ? 1 : 0;
	if (setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&nOptval, sizeof(int)) == SOCKET_ERROR) {
		int nErrorNo = GetErrorNum();
		MKLOG(FORMAT_LOG(LOGSRC, LogError, "Error! Failed to turn SO_REUSEADDR option %s! Error-No = 0x%x (%d, %s)", bReuseAddr ? "on" : "off", nErrorNo, nErrorNo, _GetErrorMsg(nErrorNo)));
		return false;
	}
	else return true;
}

bool CITUdpSocket::GetReuseAddr(bool& bReuseAddr/*out*/)
{
	if (m_socket == INVALID_SOCKET) return false;
	
	int nOptval, nOptLen = sizeof(int);
#ifdef _LINUX_
	 if (getsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&nOptval, (socklen_t *)&nOptLen) == SOCKET_ERROR)
#else
	if (getsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&nOptval, &nOptLen) == SOCKET_ERROR) 
#endif
	{
		int nErrorNo = GetErrorNum();
		MKLOG(FORMAT_LOG(LOGSRC, LogError, "Error! Failed to get SO_REUSEADDR option! Error-No = 0x%x (%d, %s)", nErrorNo, nErrorNo, _GetErrorMsg(nErrorNo)));
		return false;
	}
	else {
		bReuseAddr = nOptval == 0 ? false : true;
		return true;
	}
}

int CITUdpSocket::GetErrorNum()
{
	return WSAGetLastError();
}

SOCKET CITUdpSocket::GetSocketId()
{
	return m_socket;
}

PROJECT_NS_END
