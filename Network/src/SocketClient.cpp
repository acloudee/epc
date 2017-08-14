// SocketServer.cpp: implementation of the CSocketServer class.
//
//////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#ifdef WIN32
#elif defined(_LINUX_)

#else
#include <ioLib.h>//for close function
#endif
#include "SocketClient.h"
#include "SockUtil.h"

PROJECT_NS_BEGIN

CSocketClient::CSocketClient()
{
//	m_pSockSvr = NULL;
//	TRACE(LogDetail);			// commented for bug-142:netlogger restart deadlock
	m_sk = INVALID_SOCKET;
	m_szHost[0] = m_szService[0] = 0;
}

CSocketClient::~CSocketClient()
{
//	TRACE(LogDetail);			// commented for bug-142:netlogger restart deadlock
	Close();
}

void CSocketClient::Close()
{
	if (m_sk != INVALID_SOCKET) {
		shutdown(m_sk, SD_SEND);
#ifdef WIN32
		closesocket(m_sk);
#elif defined(_LINUX_)
		close(m_sk);
#else
		close(m_sk);
#endif
		m_sk = INVALID_SOCKET;
	}
	CSocketConnection::Close();
}

bool CSocketClient::Init(LPCTSTR cpszHost, 
						 unsigned short ushSvrPort,
						 NET_CODE* pNetCode)
{
	char szSvrPort[16];
	sprintf(szSvrPort,"%d",ushSvrPort);
	return Init(cpszHost, szSvrPort, pNetCode);
}


bool CSocketClient::Init(LPCTSTR cpszHost, 
						 LPCTSTR cpszService,
						 NET_CODE* pNetCode)
{
	if (cpszHost) {
		int nLen = strlen(cpszHost);
		if (nLen >= sizeof(m_szHost)) {
			//MKLOG(FORMAT_LOG(LOGSRC, "Hostname too long(%d, max=%d):%s!", nLen, sizeof(m_szHost), cpszHost));
			return false;
		}
		strcpy(m_szHost, cpszHost);
	}
	if (cpszService) strcpy(m_szService, cpszService);

	//	if ( !InitSocket())	return FALSE;
	int nErroCode = 0;
	if ((m_sk=ClientConnectSocket(cpszHost, cpszService, m_addrInTo, &nErroCode)) == INVALID_SOCKET) {
		if (pNetCode) {
			if (nErroCode == NET_ERROR_UNKNOWN_HOST) *pNetCode = NET_UNKNOWN_HOST;
			else if (nErroCode == NET_ERROR_UNKNOWN_PORT) *pNetCode = NET_UNKNOWN_PORT;
			else if (nErroCode == NET_ERROR_UNKNOWN_PROTOCOL) *pNetCode = NET_UNKNOWN_PROTOCOL;
			else *pNetCode = NET_ERROR;	// NET_ERROR_UNKNOWN
		}
		return false;
	}
	else if (pNetCode) *pNetCode = NET_OK;
	CSocketConnection::Init(m_sk, m_addrInTo);
	return true;
}

PROJECT_NS_END
