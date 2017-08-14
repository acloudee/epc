// SocketServer.cpp: implementation of the CSocketServer class.
//
//////////////////////////////////////////////////////////////////////
#include <stdlib.h> 
#include <stdio.h>
#include <ctype.h>
#include "SocketServer.h"
#include "SockUtil.h"
#ifdef WIN32
#include <process.h>    /* _beginthread, _endthread */
#else
#ifndef _LINUX_
#include <taskLib.h>
#include <sysLib.h>
#endif
#endif

//CSocketRegister g_srRegister;
PROJECT_NS_BEGIN

CSocketContext::CSocketContext(CSocketConnection* pSockConn, LPVOID pContext) 
{
	m_pDeadClientQueue = NULL;
	m_pRunnable = NULL;
	m_pSockConn = pSockConn; 
	m_pContext = pContext;
}

CSocketContext::~CSocketContext() 
{
	Close();
}


void CSocketContext::SetParent(CQueue* pDeadClientQueue, CRunnable* pRunnable) 
{ 
	m_pDeadClientQueue = pDeadClientQueue;
	m_pRunnable = pRunnable; 
}

void CSocketContext::Close() 
{
	// note! the order matters!
	if (m_pDeadClientQueue) {	// must put itself into the dead queue first
		if (m_pRunnable) {
			m_pDeadClientQueue->PushData(m_pRunnable);
			m_pRunnable = NULL;
		}
		m_pDeadClientQueue = NULL;
	}

	if (m_pSockConn) {
		delete m_pSockConn;
		m_pSockConn = NULL;
	}
	m_pContext = NULL;	
}

CSocketServer::CSocketServer(PCSTR cpszServiceName) 
{
	TRACE(LogDetail);
	m_lpfnOnConnect = NULL;
	m_bStop = false;
	m_pRunnable = NULL;
	m_sk= INVALID_SOCKET;
	FD_ZERO(&m_fdActive);
	m_szHost[0] = m_szService[0] = 0;
	m_pSockContext = NULL;
	m_pSocketRendezvous = NULL;
	m_nOnConnectStackSize = 0;
	m_szServiceName[0] = 0; // init	
	if (cpszServiceName && *cpszServiceName != 0) {
		strncpy(m_szServiceName, cpszServiceName, sizeof(m_szServiceName)-1);
		m_szServiceName[sizeof(m_szServiceName)-1] = 0;
	}
}

CSocketServer::~CSocketServer()
{
	TRACE(LogDetail);
	Close();
}

void CSocketServer::Close()
{
	if (!m_pRunnable) return;

	m_bStop = true;		
	if (m_sk != INVALID_SOCKET) {
#ifdef WIN32
		closesocket(m_sk);
#elif defined(_LINUX_)
		close(m_sk);
#else
		close(m_sk);
#endif
		m_sk= INVALID_SOCKET;
	}

	FD_ZERO(&m_fdActive);
	m_pRunnable->Stop();
	delete m_pRunnable;
	m_pRunnable = NULL;
	m_bStop = false;	// reset

	if (m_pSocketRendezvous) {
		m_pSocketRendezvous->Clear();
		for (int i=0; i<10; i++) 
			if (m_pSocketRendezvous->GetReference() > 1) // self has one reference
				CThread::Delay(1000);
			else break; 
		m_pSocketRendezvous->ReleaseReference();
		m_pSocketRendezvous = NULL;
	}
	// cleanup
	while (true) {
		CRunnable* pRunnable = (CRunnable*)m_pDeadClientQueue.PopData(0);
		if (pRunnable) delete pRunnable;
		else break;		
	}

	m_szHost[0] = m_szService[0] = 0;
	m_pSockContext = NULL;
}

//gwf_ ushServicePort for port,pszTransport for protocol
bool CSocketServer::Init(unsigned short ushServicePort, int qLen)
{
	char szService[16];	
	sprintf(szService,"%d",ushServicePort);
	return Init(szService, qLen);
}

//gwf_ cpszService for port number, cpszTransport for protocol
bool CSocketServer::Init(LPCSTR cpszService, int qLen)
{
//	if ( !InitSocket())	return false;	// masked due to no WSACleanup??
	m_sk = PassiveSocket(cpszService, qLen, m_addrInTo);
	if (m_sk == INVALID_SOCKET) return false;
	else CSocketConnection::Init(m_sk, m_addrInTo);
	strcpy(m_szService, cpszService);
	FD_ZERO(&m_fdActive);
	FD_SET(m_sk, &m_fdActive);
	return true;
}

bool CSocketServer::Service(LPFN_SOCKSVR_WORKER lpfnOnConnect, LPVOID pContext,
		int nOnConnectStackSize, //use default
		int nServicePollingTimeMs)
{
	m_pSocketRendezvous = new CSocketRendezvous;
	m_pSocketRendezvous->AddReference();
	m_lpfnOnConnect = lpfnOnConnect;
	if (nServicePollingTimeMs < 1) nServicePollingTimeMs = 6*1000; // default
	m_dwTimeout = nServicePollingTimeMs;//dwTimeout;
	m_nOnConnectStackSize = nOnConnectStackSize;
	
	char szTaskName[64] = {0};
	if (m_szServiceName[0] == 0) sprintf(szTaskName, "TCPSvc-%s", m_szService);

	m_pRunnable = CRunnable::_Run(_Service, (PVOID)this, 1, m_szServiceName[0] == 0 ? szTaskName : m_szServiceName);

	if (m_pRunnable) {
		m_pSockContext = pContext;
		return true;
	}
	else return false;
} 

typedef void (*LPFN_THREAD_WORKER)(LPVOID pSocketCtx); //CCTSocketConnection* lpCtSockConn;

void CSocketServer::_Service(LPVOID lpParam)
{
	CSocketServer* pThis = (CSocketServer*) lpParam;
	CSocketConnection* pNewSockConnect = NULL;
	
//	fd_set	fdReady;	
	SOCKET	skMaster = pThis->m_sk;

//	memcpy( &fdReady, &(pThis->m_fdActive), sizeof( fd_set ));
	while(!(pThis->m_bStop)) {
		struct timeval Time;
		Time.tv_sec = pThis->m_dwTimeout / 1000;
		Time.tv_usec = (pThis->m_dwTimeout % 1000) * 1000 ;
		FD_ZERO(&(pThis->m_fdActive));
		FD_SET( pThis->m_sk, &(pThis->m_fdActive));
		int nSelect = select(pThis->m_sk+1, &(pThis->m_fdActive), NULL, NULL, &Time);
		if (nSelect == 0) continue; // timer expires
		else if (nSelect == SOCKET_ERROR ) {
			MKLOG(FORMAT_LOG(LOGSRC, LogError, "TCP service port: %d error!", ntohs(pThis->m_addrInTo.sin_port)));
			break;	// error
		}

		struct sockaddr_in	AddrIn;
		int nLen = sizeof(AddrIn);

		// Check if new Connection
		if (FD_ISSET( skMaster, &(pThis->m_fdActive))) {
			if (pThis->m_bStop) break;
			// garbage collection => !!! may cause problem on bad behaviour client !!!
			// it would be safer to do it on another thread
			while (true) {
				CRunnable* pRunnable = (CRunnable*)pThis->m_pDeadClientQueue.PopData(0);
				if (pRunnable) delete pRunnable;
				else break;		
			}

#ifdef _LINUX_
			SOCKET skAccept = accept(skMaster, (struct sockaddr *)&AddrIn, (socklen_t *)&nLen);
#else
			SOCKET skAccept = accept(skMaster, (struct sockaddr*)&AddrIn, &nLen);
#endif
			if (skAccept == INVALID_SOCKET) continue;
			else {
				char szAddress[MAX_PATH];
				GetSocketAddress(skAccept, true, szAddress);
//				pSockSvr->SetActive(skAccept, true);
				pNewSockConnect = new CSocketConnection(skAccept, AddrIn);
				if (pNewSockConnect) {
					pThis->m_pSocketRendezvous->Add(skAccept);
					pThis->m_pSocketRendezvous->AddReference();
					pNewSockConnect->m_pSocketRendezvous = pThis->m_pSocketRendezvous;
					//CSocketContext* pSockCtx = new CSocketContext(pNewSockConnect, pThis->m_pSockContext);
					CCTSocketContext* pSockCtx = (CCTSocketContext*)new CSocketContext(pNewSockConnect, pThis->m_pSockContext); // for log memory
					if (pSockCtx) {
						#ifdef WIN32
							BOOL nOptVal = 1;
						#else
							int nOptVal = 1;
						#endif
	
						setsockopt(skAccept, SOL_SOCKET, SO_KEEPALIVE, (char*)&nOptVal, sizeof(nOptVal));

						CRunnable* pRunnable = new CRunnable;
						((CSocketContext*)pSockCtx)->SetParent(&pThis->m_pDeadClientQueue, pRunnable);
						pRunnable->SetFunc((LPFN_THREAD_WORKER)pThis->m_lpfnOnConnect, pSockCtx);
						char szClntName[64], szPeerIP[32];
						u_long ulPeerIP; 
						u_short ushPeerPort;
						pNewSockConnect->GetAddress(true, ulPeerIP, ushPeerPort);
						IP2String(ulPeerIP, szPeerIP, 32);
						sprintf(szClntName, "%s-%s:%d", pThis->m_szServiceName[0] == 0 ? "TCPConn2" : pThis->m_szServiceName, szPeerIP, ushPeerPort);
						if (!pRunnable->Start(szClntName, 1, 0, pThis->m_nOnConnectStackSize)) delete pSockCtx;	// this will automatically clear pRunnable and pNewSockConnect
					}
					else delete pNewSockConnect;
				}
				else 
#ifdef WIN32
					closesocket(skAccept);
#elif defined(_LINUX_)
					close(skAccept);
#else				
					close(skAccept);
#endif
			}
		}
	}

	MKLOG(FORMAT_LOG(LOGSRC, LogInformative, "TCP service port: %d stopped!", ntohs(pThis->m_addrInTo.sin_port)));
	memset(&pThis->m_addrInTo, sizeof(sockaddr_in), 0); // reset
}

PROJECT_NS_END
