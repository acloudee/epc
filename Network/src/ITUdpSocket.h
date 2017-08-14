#ifndef _ITUDPSOCKET_H
#define _ITUDPSOCKET_H

#include "sockhead.h"

#ifdef WIN32
#include <winsock2.h>
#include <Ws2tcpip.h>
#else
#include "NonWinType.h"
#endif 

#include "../../Utility/H/StringList.h"
#include "../../Utility/H/Mutex.h"

PROJECT_NS_BEGIN

class CITUdpSocket :public CUdpSocket
{
private:
	unsigned short	m_nListenPort;
	SOCKET			m_socket;
	char			m_strDestIp[256];
	unsigned long	m_nDestIp;
	CStringList		m_szlstGroup;
	CMutex	m_mtxSend;
public:
	CITUdpSocket();
	virtual	~CITUdpSocket();
	virtual	bool Init(PCSTR cpszListenPort, bool bBroadcast=false);	
	// cpszListenPort: NULL for auto-assignment
	// bBroadcast: to Tx and Rx broadcast packets for not, default is false;
	virtual	bool Init(unsigned short ushListenPort, bool bBroadcast=false); 
	// ushListenPort: zero for auto-assignment
	// bBroadcast: to Tx and Rx broadcast packets for not, default is false;
	virtual	void Close();
	
	virtual bool JoinGroup(PCSTR cpszMulticastIP);
	// cpszMulticastIP = dotted multicast IP in the rane of E0.x.x.x and EF.x.x.x
	//		that is from 224.x.x.x to 239.x.x.x
	virtual int LeaveGroup(PCSTR cpszMulticastIP=NULL);	// NULL for all
	// cpszMulticastIP = dotted multicast IP in the rane of E0.x.x.x and EF.x.x.x
	//		that is from 224.x.x.x to 239.x.x.x
	//		you could pass NULL to remove it from all multicast groups
	// return # of groups have been dropped.
	virtual	NET_CODE Send(u_long ulDestIP,  // dest IP in NETWORK byte order
				u_short ushDestPort,
				DWORD dwLen, LPBYTE pData,
				DWORD dwTimeoutsMs=3*1000);
	// Note,	if you are sending to the same destionation (such as peer to peer RTP), 
	//			use of either Send() will works identically because it will cache the DNS
	//			query for cpszDestAddr in the first Send(PCSTR cpszDestAddr, ...) to accelerate later Send().
	//			However, if you are sending to multiple destinations, 
	//			such as alternativly send one packet to peer and the other for RTP recording, 
	//			you should use the later Send(u_long ulDestIP, ...)	such that it may not affect
	//			the cached DNS result
	virtual	NET_CODE Send(PCSTR cpszDestAddr, 
				u_short ushDestPort,
				DWORD dwLen, LPBYTE pData,
				DWORD dwTimeoutsMs=3*1000);
	// cpszDestAddr:could be IP, xxx.xxx.xxx.xxx or Domain name
	// dwLen	specify how many bytes pointed to by pData to transmit.
	//			pass zero to transmit a UDP packet without payload.
	// pData	must NOT be NULL; otherwise an error code 0x16 (22): invalid argument will be returned.
	// dwTimeoutsMs: 0 for blocking call (INFINITE), 1 for nonblocking calls; 
	//				others to wait 'till timeout, measured in milliseconds
	// Note,	if you are sending to the same destionation (such as peer to peer RTP), 
	//			use of either Send() will works identically because it will cache the DNS
	//			query for cpszDestAddr in the first Send(PCSTR cpszDestAddr, ...) to accelerate later Send().
	//			However, if you are sending to multiple destinations, 
	//			such as alternativly send one packet to peer and the other for RTP recording, 
	//			you should use the later Send(u_long ulDestIP, ...)	such that it may not affect
	//			the cached DNS result
	// Note: the resolution from domain name to IP should be cached inside udp 
	//return value:0 stands for success,
	//				-1 stands for invalid socket,
	//				-2 for unkown host
	//				1 stands for timeout or other reasons
	// to send a UDP header only, pass dwLen = 0 &&  pData is "" 
	// If pData is NULL, it will cause 0x16(Invalid argument) error.
	 
	virtual	NET_CODE Receive(u_long& ulIP, 
				u_short& ushSrcPort,		
				DWORD& dwLen, LPBYTE pData, 
				DWORD dwTimeoutsMs=3*1000);				
	// ulIP: source IP
	// dwTimeoutsMs: 0 for blocking call (INFINITE), 1 for nonblocking calls; 
	//				others to wait 'till timeout
	virtual unsigned short GetListenPort();
	
	//set and get the value of TOS,return true if succeed otherwise return false;
	virtual bool SetTos(unsigned char chTos=0/*in*/) ;//default is not set
	virtual bool GetTos(unsigned char& chTos/*out*/);
	
	//set and get the value of TTL,return true if succeed otherwise return false;
	virtual bool SetTtl(unsigned char chTtl/*in*/); // default is equal to system wide 
	virtual bool GetTtl(unsigned char& chTtl);
	//SOL_SOCKET, SO_REUSEADD	
	virtual bool SetReuseAddr(bool bReuseAddr=false/*in*/); // default is false
	virtual bool GetReuseAddr(bool& bReuseAddr/*out*/);

	virtual SOCKET GetSocketId();
	virtual int GetErrorNum();
};

PROJECT_NS_END

#endif
