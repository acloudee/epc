#ifndef __SocketServer_h__
#define __SocketServer_h__

#include "sockhead.h"
#include "SocketConnection.h"
#include "../../Utility/H/Runnable.h"
#include "../../Utility/H/Queue.h"
 
PROJECT_NS_BEGIN

class CSocketContext : public CCTSocketContext
{
public:
	CSocketContext(CSocketConnection* pSockConn, LPVOID pContext = NULL);
	virtual ~CSocketContext();
	virtual void Close();
	virtual inline CCTSocketConnection* GetConnection() { return m_pSockConn;}
	virtual inline LPVOID GetContext() { return m_pContext; }

	void SetParent(CQueue* pDeadClientQueue, CRunnable* pRunnable);

private:
	CCTSocketConnection* m_pSockConn;
	LPVOID	m_pContext;
	CQueue* m_pDeadClientQueue;
	CRunnable* m_pRunnable;
};


class CSocketServer : public CSocketConnection, public CCTSocketServer
{
public:
	CSocketServer(PCSTR cpszServiceName=NULL); // default is TCPSvc-Port
	virtual ~CSocketServer();
public:	
	virtual bool Init(PCSTR cpszServicePort, int qLen=5);
	virtual bool Init(unsigned short ushServicePort, int qLen=5);
	virtual bool Service(LPFN_SOCKSVR_WORKER lpfnOnConnect, LPVOID pContext=NULL, 
		int nOnConnectStackSize=0,//use default
		int nServicePollingTimeMs=6*1000); // how long the svr thread checking for exit
	virtual inline unsigned short GetServicePort() { return ntohs(m_addrInTo.sin_port); }
	
	virtual inline int GetSocketInfo(char* pszBuf, int nBufLen) { return ToString(pszBuf, nBufLen);}
//	void	SetActive(SOCKET s, bool bActive);
	void Close();
	virtual inline  int GetErrorNum(){ return CSocketConnection::GetErrorNum();}
	virtual inline SOCKET GetSocketId(){return CSocketConnection::GetSocketId();}
	virtual inline bool IsReadable(DWORD dwTimeout){return CSocketConnection::IsReadable(dwTimeout);}

	//set and get the value of TOS,return true if succeed otherwise return false;
	virtual inline bool SetTos(unsigned char chTos/*in*/){return CSocketConnection::SetTos(chTos);}
	virtual inline bool GetTos(unsigned char& chTos/*out*/){return CSocketConnection::GetTos(chTos);}
	
	//set and get the value of TTL,return true if succeed otherwise return false;
	virtual inline bool SetTtl(unsigned char chTtl/*in*/){return CSocketConnection::SetTtl(chTtl);}
	virtual inline bool GetTtl(unsigned char& chTtl){return CSocketConnection::GetTtl(chTtl);}
	
	virtual inline bool GetAddress(unsigned long& ulIP, unsigned short& ushPort) { return  CSocketConnection::GetAddress(false, ulIP, ushPort); }

protected:
	static 	void	_Service(LPVOID lpParam);

private:
	LPVOID	m_pSockContext;
	DWORD	m_dwTimeout; 
	fd_set	m_fdActive;
	CRunnable* m_pRunnable;
	bool	m_bStop;
	LPFN_SOCKSVR_WORKER m_lpfnOnConnect;
	char	m_szHost[MAX_PATH];
	char	m_szService[MAX_PATH];
	char	m_szServiceName[36]; // 
	CQueue	m_pDeadClientQueue;
	unsigned short m_ushPort;
	int		m_nOnConnectStackSize;
};

PROJECT_NS_END

#endif // __SocketServer_h__
