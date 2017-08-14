#ifndef __SocketClient_h__
#define __SocketClient_h__

#include "SocketConnection.h"
#include "sockhead.h"

PROJECT_NS_BEGIN

class CSocketClient : public CSocketConnection, public CCTSocketClient
{
public: 
	CSocketClient();
	virtual	~CSocketClient();
	virtual	bool Init(LPCTSTR cpszHost, LPCTSTR cpszSrvPort, NET_CODE* pNetCode=NULL);
	virtual	bool Init(LPCTSTR cpszHost, unsigned short ushSvrPort, NET_CODE* pNetCode=NULL);
	virtual void Close();
	virtual inline void SetStream(bool bRaw=false) { CSocketConnection::SetStream(bRaw);}
	virtual inline bool IsBroken() { return CSocketConnection::IsBroken(); }
	virtual inline	bool Send(DWORD dwLen, LPBYTE pData, DWORD dwTimeoutsMs=30*1000) { return CSocketConnection::Send(dwLen, pData, dwTimeoutsMs); }
	virtual inline	bool Receive(DWORD& dwLen, LPBYTE pData, DWORD dwTimeoutsMs=30*1000){ return CSocketConnection::Receive(dwLen, pData, dwTimeoutsMs); }
	virtual inline	bool RawReceive(DWORD& dwLen, LPBYTE pData, DWORD dwTimeoutsMs=30*1000)
	{
		return CSocketConnection::RawReceive(dwLen,pData,dwTimeoutsMs);
	}

	virtual inline int GetSocketInfo(char* pszBuf, int nBufLen) { return ToString(pszBuf, nBufLen);}	
	virtual inline int GetErrorNum(){ return CSocketConnection::GetErrorNum();}
	virtual inline SOCKET GetSocketId(){return CSocketConnection::GetSocketId();}
	virtual inline bool IsReadable(DWORD dwTimeout){return CSocketConnection::IsReadable(dwTimeout);}
	
	//set and get the value of TOS,return true if succeed otherwise return false;
	virtual inline bool SetTos(unsigned char chTos/*in*/){return CSocketConnection::SetTos(chTos);}
	virtual inline bool GetTos(unsigned char& chTos/*out*/){return CSocketConnection::GetTos(chTos);}
	
	//set and get the value of TTL,return true if succeed otherwise return false;
	virtual inline bool SetTtl(unsigned char chTtl/*in*/){return CSocketConnection::SetTtl(chTtl);}
	virtual inline bool GetTtl(unsigned char& chTtl) {return CSocketConnection::GetTtl(chTtl);}

	virtual bool SetTcpNoDelay(bool bNoDelay=false) { return CSocketConnection::SetTcpNoDelay(bNoDelay); } // getsockopt(level = IPPROTO_TCP, TCP_NODELAY)
	virtual bool GetTcpNoDelay(bool& bNoDelay/*IN/OUT*/) { return CSocketConnection::GetTcpNoDelay(bNoDelay); } // getsockopt(level = IPPROTO_TCP, TCP_NODELAY)
	// enable/Disables the Nagle algorithm for send coalescing, suitable for telnet/rlogin/ssh (virtual terminal) appliations
	// default is false: congestion-avoidance algorithm works 

	virtual inline bool GetAddress(bool bPeer, unsigned long& ulIP, unsigned short& ushPort)  
	{
		return CSocketConnection::GetAddress(bPeer, ulIP, ushPort);
	}
	// Get peer or local address of the current binding socket
	// both output IP/Port are in network byte order

private:
	char	m_szHost[ MAX_PATH ];
	char	m_szService[ MAX_PATH ];
};

PROJECT_NS_END

#endif // __SocketClient_h__
