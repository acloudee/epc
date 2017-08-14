#ifndef __SocketConnection_h__
#define __SocketConnection_h__

#include "sockhead.h"

#ifdef WIN32
#include <winsock2.h>
#include <Ws2tcpip.h>
#elif defined(_LINUX_)
#include "NonWinType.h"
#else
#include <socklib.h>
#include <inetlib.h>
#include <types.h>
#include "NonWinType.h"
#endif 

#include "SocketRendezvous.h"

PROJECT_NS_BEGIN

typedef bool (*LPFN_SEND)(SOCKET s, DWORD dwLen, LPBYTE pData, struct sockaddr* pSock, int nSockAddrSize, bool& bTimeouted, bool bRaw);	// only for 
typedef bool (*LPFN_RECEIVE)(SOCKET s, DWORD& dwLen, LPBYTE pData, struct sockaddr* pSock, int* pnSockAddrSize, bool& bTimeouted, bool bRaw);

class CSocketConnection : public CCTSocketConnection
{
public:
	CSocketConnection();
	CSocketConnection(SOCKET s, struct sockaddr_in	addrIn);
	virtual ~CSocketConnection();
	virtual inline void SetStream(bool bRaw=false) { m_bRaw = bRaw;}
	virtual	inline bool IsBroken() { return m_bBroken;}
	virtual	bool Send( DWORD dwLen, LPBYTE pData, DWORD dwTimeoutsMs=30*1000);
	virtual	bool Receive( DWORD& dwLen, LPBYTE pData, DWORD dwTimeoutsMs=30*1000);
	virtual bool RawReceive(DWORD& dwLen, LPBYTE pData, DWORD dwTimeoutsMs=30*1000);
	virtual bool GetAddress(bool bPeer, unsigned long& ulIP, unsigned short& ushPort);
	// Get peer or local address of the current socket
	// both output IP/Port are in network byte order
	virtual int ToString(char* szBuf, int nBufLen);
	// Output format: xxx.xxx.xxx.xxx:pppp
	virtual int GetErrorNum();
	virtual bool IsReadable(DWORD dwTimeout);
	virtual inline void GetAddrinFrom(struct sockaddr_in * Infrom) { *Infrom = m_addrInTo;}
	
public:
	inline SOCKET GetSocketId() {return m_sk;};
	virtual void Close();
	virtual void Init(SOCKET s, struct sockaddr_in	addrIn);
	CSocketRendezvous* m_pSocketRendezvous;

public:
	//set and get the value of TOS,return true if succeed otherwise return false;
	virtual bool SetTos(unsigned char chTos/*in*/);
	virtual bool GetTos(unsigned char& chTos/*out*/);
	
	//set and get the value of TTL,return true if succeed otherwise return false;
	virtual bool SetTtl(unsigned char chTtl/*in*/);
	virtual bool GetTtl(unsigned char& chTtl);

	virtual bool SetTcpNoDelay(bool bNoDelay=false); // setsockopt(level = IPPROTO_TCP, TCP_NODELAY)
	virtual bool GetTcpNoDelay(bool& bNoDelay/*IN/OUT*/); // setsockopt(level = IPPROTO_TCP, TCP_NODELAY)
	// enable/Disables the Nagle algorithm for send coalescing, suitable for telnet/rlogin/ssh (virtual terminal) appliations
	// default is false: congestion-avoidance algorithm works 
	
protected:
	SOCKET	m_sk;
	struct sockaddr_in	m_addrInTo, m_addrInFrom;
	LPFN_SEND		m_lpfnSend;
	LPFN_RECEIVE	m_lpfnReceive;
	bool	m_bBroken;
	DWORD	m_dwSendTimeoutsMs;
	DWORD	m_dwReceiveTimeoutsMs;	
	bool	m_bRaw;
};

PROJECT_NS_END

#endif // __SocketConnection_h__

