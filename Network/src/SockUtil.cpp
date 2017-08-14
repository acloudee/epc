#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <time.h>
#include "SockUtil.h"
#include "../../Utility/H/Common.h"	// for LOG_NEW_MEMORY and LOG_DELETE_MEMORY

PROJECT_NS_BEGIN


const char* _GetErrorMsg(int nErrorCode)
{
	PCSTR cpszError = strerror(nErrorCode);
	if (cpszError == NULL) return "Unknown";
	else return cpszError;
}

bool GetSocketAddress(SOCKET s, bool bPeer, unsigned long& ulIP, unsigned short& ushPort)
// IP is in network byte order
// PORT is is HOST byte order
{
	if (s == INVALID_SOCKET) return false;

	sockaddr sa;
	memset(&sa, 0, sizeof(sockaddr));

#ifdef _LINUX_
	socklen_t slen = sizeof(sockaddr);
#else
	int slen = sizeof(sockaddr);
#endif
	
	int nRc = SOCKET_ERROR;

	if (bPeer) nRc = getpeername(s, &sa, &slen);
	else nRc = getsockname(s, &sa, &slen);

	if (nRc == 0) {
		sockaddr_in *psin = (sockaddr_in *)&sa; // // pAddrIn->sin_addr.S_un.S_addr;
		ulIP = psin->sin_addr.s_addr; // *(unsigned long*)&pAddrIn->sin_addr;
		ushPort = ntohs(psin->sin_port);
		return true;
	}
	else return false;	
}

void GetSocketAddress(SOCKET s, bool bPeer, char* pszAddress)
{
	strcpy(pszAddress, "Unknown Address");
	unsigned long ulIP;
	unsigned short ushPort;

	if (GetSocketAddress(s, bPeer, ulIP, ushPort)) {
#ifdef _WIN32
		sprintf(pszAddress,"%s:%d",inet_ntoa(*(struct in_addr*)&ulIP), ushPort);
#elif defined(_LINUX_)		
		char addr[INET_ADDRSTRLEN];
		in_addr ipaddress;
		ipaddress.s_addr = ulIP;
		sprintf(pszAddress, "%s:%d", inet_ntop( AF_INET, &ipaddress, addr, sizeof(addr) ), ushPort);
#else // VxWorks
		char szBuf[64];
		inet_ntoa_b (*(struct in_addr*)&ulIP, szBuf);
		sprintf(pszAddress,"%s:%d",szBuf, ushPort);
#endif
	}
}

// alber_20010502 rewrites all
bool	TcpSend(SOCKET s, DWORD dwLen, LPBYTE pbData, 
				struct sockaddr* pSock, int nSockAddrSize, 
				 bool& bTimedout, bool bRaw)
{
	int dwOut = htonl(dwLen);	
	bool bSent = true;

	if (!bRaw) bSent = send(s, (char *)&dwOut, sizeof(int), 0) != SOCKET_ERROR;
	if (bSent)	{ 
		if (dwLen < 4096) // takes care of zero length message to shutdown socket
			bSent = send(s, (char*)pbData, dwLen, 0) != SOCKET_ERROR;
		else { // for WSAENOBUFS No buffer space is available. 
			char* pcbData = (char*)pbData;
			for (DWORD dwSent=0; bSent && dwLen>0; pcbData+=dwSent) {
				dwSent = dwLen < 4096 ? dwLen : 4096; // 
				dwLen -= dwSent;
				bSent = send(s, pcbData, dwSent, 0) != SOCKET_ERROR;
			}
		}
	}

	DWORD dwError = 0;
	if (!bSent) {
		dwError = WSAGetLastError();
		if (dwError == WSAETIMEDOUT) 
			bTimedout = true;
	}

	return bSent;
}
// albert_20010503
// while returning, it either collects data of nBufLen size or
// peer reset socket/or encounters an error
int RxNBytes(SOCKET s, int nBufLen, char* szBuf)
{
	int nOffset = 0;		
	while (nOffset < nBufLen) {
		int nLen = recv(s, szBuf+nOffset, nBufLen-nOffset, 0);
		if (nLen == SOCKET_ERROR) return SOCKET_ERROR;
		else if (nLen == 0)  // peer closed gracefully
			return nOffset;// forced skipped					
		else nOffset += nLen;			
	}		
	return nOffset;
}


bool	TcpReceive(SOCKET s, DWORD& dwLen, LPBYTE pbData,
				   struct sockaddr* pSock, int* pnSockAddrSize, 
				 bool& bTimedout, bool bRaw)
{
	long	lLength = 0;
	int		nReceive = 0;
	DWORD	dwError = 0;

//	char szAddress[MAX_PATH];
//	GetAddress(s, true, szAddress);

	//if pbData is NULL,just wodering if there is data to receive;
	if (pbData == NULL) {
		while (true) {	// make sure that we have 4 bytes at least
			nReceive = recv(s,(char*)&lLength, sizeof(int), MSG_PEEK);
			if (nReceive == SOCKET_ERROR || nReceive == 0)	goto RECEIVE_ERROR;
			else if (bRaw) {
				dwLen = nReceive;	// bytes pending				
				return true;
			}
			else if (nReceive >= (int)sizeof(int)) break;
		}

		dwLen = ntohl(lLength);
		return true;
	}
	else if (bRaw) {
//		nReceive = recv(s, (char*)pbData, dwLen, 0);
		nReceive = RxNBytes(s, dwLen, (char*)pbData);
		if (nReceive == SOCKET_ERROR || 
			nReceive == 0)	// peer closed gracefully
			goto RECEIVE_ERROR;
		dwLen = nReceive;
		return true;
	}
	else {
		nReceive = RxNBytes(s, sizeof(int), (char*)&lLength);
		if (nReceive == SOCKET_ERROR || nReceive < (int)sizeof(int))
			goto RECEIVE_ERROR;

		lLength = ntohl(lLength);
		if (lLength < (int)dwLen) dwLen = lLength;	
		nReceive = RxNBytes(s, dwLen, (char*)pbData);
		if (nReceive == SOCKET_ERROR) goto RECEIVE_ERROR;
		else if (nReceive < (int)dwLen) { // peer closed gracefully
			dwLen = nReceive; // in case of a broken socket
			lLength = dwLen;	// since peer reset, leaves no need to skip
		}		
		// skip all others in case pData were not large enough to accommodate
		if (lLength > (int)dwLen) {
			lLength = lLength - dwLen;
			char* lpBuf = new char[lLength];
			RxNBytes(s, lLength, lpBuf);
			delete lpBuf;
		}

		return true;
	}

RECEIVE_ERROR:
	dwError = WSAGetLastError();
	if (dwError == WSAETIMEDOUT) bTimedout = true;

	return false;
}

bool InitSocket()
{
#ifndef WIN32
	return true;
#else
	
	WSADATA wsaData;
 
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) { // albert_20010719
		DWORD	dwError = WSAGetLastError();
//gwf_		char szErrorMessage[MAX_PATH];
//gwf_		FormatError(dwError, szErrorMessage );
		WSACleanup();
		return false; 
	}

	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
		DWORD	dwError = WSAGetLastError();
//gwf_		char szErrorMessage[MAX_PATH];
//gwf_		FormatError(dwError, szErrorMessage);
		WSACleanup();
		return false; 
	}
	return true;
#endif
}

void CleanupSocket()
{
	TRACE(LogDetail);

#ifdef WIN32
	WSACleanup();
#endif
}

bool TcpReceive_1(SOCKET s, DWORD& dwLen, LPBYTE pbData,
				  struct sockaddr* pSock, int* pnSockAddrSize,
				 bool& bTimedout, bool bRaw)
{
	int recvLen;
	if ((recvLen=recv(s,(char*)pbData, dwLen, 0)) == SOCKET_ERROR || recvLen == 0)	{
		if (WSAGetLastError() == WSAETIMEDOUT) bTimedout = true;
		dwLen = 0;
		return false;
	}
	else  {
		dwLen = (DWORD)recvLen;
		return true;
	}
}

SOCKET PassiveSocket(LPCSTR pszService, int qLen, sockaddr_in& sin)
{
#ifdef WIN32
	servent		*pse = NULL;
	protoent	*ppe = NULL;
#endif
	//sockaddr_in	sin;
	SOCKET		s = INVALID_SOCKET;

	memset( &sin,  0, sizeof( sin ));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = htonl(INADDR_ANY);

	/* Map service name to port name */
	if (!pszService ) return INVALID_SOCKET;
#ifdef WIN32	
	if (pse = getservbyname(pszService, "tcp"))	// should not be else if => otherwise it will not execute else
		sin.sin_port = htons( ntohs( (u_short) pse->s_port) );
	else if ( (sin.sin_port = htons((u_short) atoi( pszService) ) )== 0 ) {
		return INVALID_SOCKET;
	}
	/* Map protocol name to protocol number */
	if ((ppe = getprotobyname("tcp")) == 0)	return INVALID_SOCKET;
#else
	if ((sin.sin_port = htons((u_short) atoi(pszService)))== 0) return INVALID_SOCKET;
#endif

	/* Allocate a socket */
#ifdef WIN32
	s = WSASocket (AF_INET, SOCK_STREAM, ppe->p_proto, NULL, 0, WSA_FLAG_OVERLAPPED);
#else
	s = socket (AF_INET, SOCK_STREAM, 0);
#endif
	if (s == INVALID_SOCKET) {
		return INVALID_SOCKET;
	}

	/* Bind the socket */
	if (bind(s,(struct sockaddr*)&sin, sizeof(sin)) == SOCKET_ERROR)	{
		#ifdef WIN32
			closesocket(s);
		#else
			close(s);
		#endif
		return INVALID_SOCKET;
	}

	/* Listen to the socket */
	if (listen(s,qLen) == SOCKET_ERROR) {
		#ifdef WIN32
		closesocket(s);
		#else
		close(s);
		#endif
		return INVALID_SOCKET;
	}

	return s;
}

//gwf,Create a socket,according the pszTrnasprot which is udp or tcp,
//pszHost indicates the destination host,pszService indicates the port;
SOCKET ClientConnectSocket(LPCSTR pszHost, LPCSTR pszService,
						sockaddr_in& sin, int* pnErrorCode)
{
	hostent		*phe = NULL;
#ifdef WIN32
	servent		*pse = NULL;	
	protoent	*ppe = NULL;
#endif
	SOCKET		s = INVALID_SOCKET;

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;

	if (!pszService) {
		if (pnErrorCode) *pnErrorCode  = NET_UNKNOWN_HOST;
		return INVALID_SOCKET;
	}
	else if (pnErrorCode) *pnErrorCode  = NET_ERROR_UNKNOWN;


#ifdef WIN32
	/* Map service name to port name */
	if (pse = getservbyname( pszService, "tcp")) // should not be else if => otherwise it will not execute else
		sin.sin_port = pse->s_port;
	else if ((sin.sin_port = htons((u_short) atoi( pszService) ) ) == 0 )	{
		if (pnErrorCode) *pnErrorCode  = NET_UNKNOWN_PORT;
		return INVALID_SOCKET;
	}
#else
	if ((sin.sin_port = htons((u_short) atoi( pszService) ) ) == 0 ) {
		if (pnErrorCode) *pnErrorCode  = NET_UNKNOWN_PORT;
		return INVALID_SOCKET;	
	}
#endif

	/* Map host name to IP address, allowing for dotted decimal */
	if (!pszHost || *pszHost == 0) pszHost = "localhost";	// default

#if defined(WIN32) || defined(_LINUX_)
	if (phe = gethostbyname(pszHost))
		memcpy(&sin.sin_addr, phe->h_addr, phe->h_length);
#else
	hostent hostname;
	char szHost[100];
	strcpy(szHost,pszHost);
	
	int DestIp = hostGetByName(szHost);
	//int DestIp = ERROR;
	//CDnsRunner* pDnsRunner = CDnsRunner::_GetInstance();
	//if (pDnsRunner) DestIp = pDnsRunner->HostGetByName(szHost);
	if (DestIp != ERROR) sin.sin_addr.s_addr = (unsigned long)DestIp;

/*	if ( (phe = resolvGetHostByName(szHost,(char*)&hostname,sizeof(hostname))) != NULL )
		memcpy(&sin.sin_addr,phe->h_addr_list[0],phe->h_length);
*/
#endif
	else if (( sin.sin_addr.s_addr = inet_addr((PSTR)pszHost) ) == INADDR_NONE) {
		if (pnErrorCode) *pnErrorCode  = NET_UNKNOWN_HOST;
		return INVALID_SOCKET;
	}
#ifdef WIN32
	/* Map protocol name to protocol number */
	if ((ppe = getprotobyname("tcp")) == 0)	{
		if (pnErrorCode) *pnErrorCode  = NET_UNKNOWN_PROTOCOL;
		return INVALID_SOCKET;
	}
#endif

	/* Allocate a socket */
	#ifdef WIN32
		s = WSASocket (AF_INET, SOCK_STREAM, ppe->p_proto, NULL, 0, WSA_FLAG_OVERLAPPED);
	#else
		s = socket (AF_INET, SOCK_STREAM, 0);
	#endif

	if (s == INVALID_SOCKET) return INVALID_SOCKET;

	/* connect the socket */
	MKLOG(FORMAT_LOG(LOGSRC,LogInformative,"start connect"));
	if (connect( s,(sockaddr*) &sin, sizeof( sin)) == SOCKET_ERROR)	{
		#ifdef WIN32
		closesocket(s);
		#else
		close(s);
		#endif
		MKLOG(FORMAT_LOG(LOGSRC,LogInformative,"end connect"));
		return INVALID_SOCKET;
	}
	else {	// keep alive
		#ifdef WIN32
			BOOL nOptVal = 1;
		#else
			int nOptVal = 1;
		#endif
		MKLOG(FORMAT_LOG(LOGSRC,LogInformative,"end connect"));
		setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, (char*)&nOptVal, sizeof(nOptVal));
	}

	return s;
}

PROJECT_NS_END
