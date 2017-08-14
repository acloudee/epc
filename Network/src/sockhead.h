/*
sock head interface:
Note:if you want include sockhead.h  
in you file,you MUST include it prior to other head file,
otherwise there will be conflict with other files.
for exampel:
#include "sockhead.h"

#include "MyOtherHead.h"

*/ 

#ifndef _SOCKHEAD_H_
#define _SOCKHEAD_H_

#ifdef _WIN32
	#include <winsock2.h>
	#include "winerrno.h"
	#include "../../Utility/H/Common.h"
#else
	#include "NonWinType.h"
#endif

// Definitions for IP type of service (ip_tos)
#define	IPTOS_LOWDELAY		0x10 //(D)
#define	IPTOS_THROUGHPUT	0x08 //(T)
#define	IPTOS_RELIABILITY	0x04 //(R)
#define	IPTOS_MINCOST		0x02 //(C)

// Definitions for IP precedence (also in ip_tos) (hopefully unused)
#define	IPTOS_PREC_NETCONTROL		0xe0 // interior router controls, such as OSPF or RIP
#define	IPTOS_PREC_INTERNETCONTROL	0xc0 // exterior router controls, such as BGP
#define	IPTOS_PREC_CRITIC_ECP		0xa0 // voice
#define	IPTOS_PREC_FLASHOVERRIDE	0x80
#define	IPTOS_PREC_FLASH			0x60
#define	IPTOS_PREC_IMMEDIATE		0x40
#define	IPTOS_PREC_PRIORITY			0x20
#define	IPTOS_PREC_ROUTINE			0x00 // best effort

PROJECT_NS_BEGIN

bool InitSocket();
void CleanupSocket();

int IP2String(u_long ulIP, PSTR pszBuf, int nBufLen);
// ulIP:	passed in parameter in NETWORK byte order. 
// return	# of bytes written into pszBuf, including the terminiated '\0'.
bool DottedString2IP(PCSTR cpszDottedIp, u_long& ulIp);
// Note: output in Network byte order.
bool GetLocalhostName(char* pszBuf, int nBufLen);	// get localhost name
u_long GetLocalhostIP(); // get localhost IP in network byte order,
//return 127.0.0.1 if no valid IP available
bool IsPublicIP(u_long ulIP); 
// Private IP:	192.168.xxx.xxx/10.xxx.xxx.xxx/172.[16-31].xxx.xxx
//				127.0.0.1  0.0.0.0
// All others: public
bool IsLocalhostIPValid(); // localhost is not 127.0.0.1, 255.255.255.255, 0.0.0.0
bool Hostname2IP(PCSTR cpszDnsName, u_long& ulIp);
// resolve domain name to NETWORK byte order IP,
// cpszDnsName could be a DNS record or a dotted IP.
int Hostname2IP(PCSTR cpszDnsName,PSTR pszBuf, int nBufLen);
// resolve domain name to dotted IP string, a.b.c.d
// return 0 for failed! otherwise return the lenghth of bytes writtern into pszBuf
// cpszDnsName could be a DNS record or a dotted IP.
u_long GetNetMask(); // 
//test if the remote host is active;If no replies are received 
//within a 5-second timeout period, the routine exits,return false
//only available on Vxworks currently
// pHost: host to ping 
// nNumPackets: number of packets to receive 
bool IsNetMaskValid(u_long ulNetMask);
// check whether the ulNetMask(in network byte order) is a valid netmask
// netmask has to be contiguous (0.0.0.0, 255.255.255.255 is invalid)
bool Ping(PCSTR cpszHost, int nNumPackets = 1);

bool IsOnSameLanSegment(u_long ulIP);
//  return true if (ulIP & GetNetMask()) == GetLocalhostName() & GetNetMask())

#ifdef _LINUX_
	bool GetMACAddress(char *pszBuf, int nBufLen);	// get MAC address
#endif

class CUdpSocket 
{
public:
	static CUdpSocket* _NewInstance();
/*	static int _IP2String(u_long ulIP, PSTR pszBuf, int nBufLen); 
	// ulIP:	passed in parameter in network byte order. 
	// convert unsinge long IP to xxx.xxx.xxx.xxx string form
	// If pszBuf is NULL or nBufLen is 0, _IP2String will return 
	// return the buffer length needed for conversion, including '\0'.
	//if the return value is 0,it indiates the error occurred during the conversion
	static bool _Name2IP(PCSTR cpszDnsName, u_long& ulIp);
	// resolve domain name to NETWORK byte order IP
	static int _Name2IP(PCSTR cpszDnsName, PSTR pszBuf, int nBufLen);
	// resolve domain name to dotted IP string, a.b.c.d
	static bool _GetHostName(char* pszBuf, int nBufLen); // get localhost name
	static bool _String2Ip(PCSTR cpszDottedIp, u_long& ulIp);
	// output in Network byte order.
	static u_long _GetHostIP();	// get localhost IP in network byte order
	static bool _IsPublicIP(u_long ulIP); 
	// Private IP:	192.168.xxx.xxx/10.xxx.xxx.xxx/172.[16-31].xxx.xxx
	//				127.0.0.1 0.0.0.0
	// All others: public
	static bool _IsHostIPValid(); // localhost is not 127.0.0.1, 255.255.255.255, 0.0.0.0
*/
public:
	CUdpSocket();
	virtual	~CUdpSocket();
	virtual	bool Init(PCSTR cpszListenPort, bool bBroadcast=false) = 0;	
	// cpszListenPort: NULL for auto-assignment
	// bBroadcast: to Tx and Rx broadcast packets for not, default is false;
	virtual	bool Init(unsigned short ushListenPort, bool bBroadcast=false) = 0; 
	// ushListenPort: zero for auto-assignment, in host byte order
	// bBroadcast: to Tx and Rx broadcast packets for not, default is false;

	virtual	void Close() = 0;
	virtual bool JoinGroup(PCSTR cpszMulticastIP) = 0;
	// cpszMulticastIP = dotted multicast IP in the rane of E0.x.x.x and EF.x.x.x
	//		that is from 224.x.x.x to 239.x.x.x
	//		To join to multiple groups simultaneously, call JoinGroup multiple times.
	virtual int LeaveGroup(PCSTR cpszMulticastIP=NULL) = 0;	// NULL for all
	// cpszMulticastIP = dotted multicast IP in the rane of E0.x.x.x and EF.x.x.x
	//		that is from 224.x.x.x to 239.x.x.x
	//		you could pass NULL to remove it from all multicast groups
	virtual	NET_CODE Send(PCSTR cpszDestAddr,// could be IP, xxx.xxx.xxx.xxx or Domain name
				u_short ushDestPort, // in host byte order
				DWORD dwLen, LPBYTE pData, 
				DWORD dwTimeoutsMs=3*1000) = 0;
	// Note,	if you are sending to the same destionation (such as peer to peer RTP), 
	//			use of either Send() will works identically because it will cache the DNS
	//			query for cpszDestAddr in the first Send(PCSTR cpszDestAddr, ...) to accelerate later Send().
	//			However, if you are sending to multiple destinations, 
	//			such as alternativly send one packet to peer and the other for RTP recording, 
	//			you should use the later Send(u_long ulDestIP, ...)	such that it may not affect
	//			the cached DNS result
	virtual	NET_CODE Send(u_long ulDestIP,  // dest IP in NETWORK byte order
				u_short ushDestPort, // in HOST byte order
				DWORD dwLen, LPBYTE pData,
				DWORD dwTimeoutsMs=3*1000) = 0;
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
				u_short& ushSrcPort,	// in HOST byte order	
				DWORD& dwLen, LPBYTE pData, 
				DWORD dwTimeoutsMs=3*1000) = 0;				
	// ulIP: return the source IP in NETWORK byte order
	// ushSrcPort: return the source port in HOST byte order.
	// dwTimeoutsMs: 0 for blocking call (INFINITE), 1 for nonblocking calls; 
	//				others to wait 'till timeout
	virtual unsigned short GetListenPort() = 0;

	//set and get the value of TOS,return true if succeed otherwise return false;
	/*
	  SetTos does not take effect on Windows 2000 by default;
	  if user wants to use it correctly on Windows 2000,please add a new key named DisableUserTOSSetting with value 0 
	  under following location:		
	  HKEY_LOCAL_ MACHINE on Local Machine\System\CurrentControlSet\Services\Tcpip\Parameters\;
	  
	  for more details,please refer to:	
	  http://support.microsoft.com:80/support/kb/articles/Q248/6/11.ASP&NoWebContent=1

	  SetTos works well on VxWorks.
	*/

	/*
	7(MSB)  6     5      4     3     2     1    0(LSB)
	+-----+-----+-----+-----+-----+-----+-----+-----+
	|   PRECEDENCE    |  D  |  T  |  R  |  0  |  0  |
	+-----+-----+-----+-----+-----+-----+-----+-----+

	Bits 5-7: Precedence.
	Bit 4: 0 = Normal Delay, 1 = Low Delay. (RTP)  // IPTOS_LOWDELAY		0x10
	Bits 3: 0 = Normal Throughput, 1 = High Throughput. (RTP) // IPTOS_THROUGHPUT	0x08
	Bits 2: 0 = Normal Relibility, 1 = High Relibility. (RTCP) //IPTOS_RELIABILITY	0x04
	Bit 0-1: Reserved for Future Use. //IPTOS_MINCOST		0x02

	Precedence
	111 - Network Control  // 	IPTOS_PREC_NETCONTROL		0xe0
	110 - Internetwork Control	// IPTOS_PREC_INTERNETCONTROL	0xc0
	101 - CRITIC/ECP (RTP)	// IPTOS_PREC_CRITIC_ECP		0xa0
	100 - Flash Override (Video RTP)	// IPTOS_PREC_FLASHOVERRIDE	0x80
	011 - Flash (RTCP)	// IPTOS_PREC_FLASH		0x60
	010 - Immediate	// IPTOS_PREC_IMMEDIATE		0x40
	001 - Priority	// IPTOS_PREC_PRIORITY		0x20
	000 - Routine (normal)	// IPTOS_PREC_ROUTINE		0x00
	,default value:0
	*/
	virtual bool SetTos(unsigned char chTos = 0/*in*/) = 0; // default is not set
	virtual bool GetTos(unsigned char& chTos/*out*/) = 0;
	
	//set and get the value of TTL,return true if succeed otherwise return false;
	//default is 128;
	virtual bool SetTtl(unsigned char chTtl/*in*/) = 0; // default is the system default.
	virtual bool GetTtl(unsigned char& chTtl) = 0;
	//SOL_SOCKET, SO_REUSEADD	
	virtual bool SetReuseAddr(bool bReuseAddr=false/*in*/) = 0; // default is false
	virtual bool GetReuseAddr(bool& bReuseAddr/*out*/) = 0;	
	virtual int GetErrorNum() = 0;
	virtual SOCKET GetSocketId() = 0;
};

class  CCTSocketConnection
{
public:
	CCTSocketConnection();
	virtual ~CCTSocketConnection();

	virtual void SetStream(bool bRaw=false)=0;
	// take effects only if TxProtocol is TCP.
	// bRaw: default is false, set to true only when you want to re-assemble
	//		the packets sent through TCP stream on your own instead of
	//		replying on the underlying socket library to do the on your
	//		behalf.
	// Note: call SetStream() before any Send() / Receive() is called.
	virtual	bool IsBroken() = 0;
	virtual	bool Send(DWORD dwLen, LPBYTE pData, DWORD dwTimeoutsMs=30*1000) = 0;
	// dwTimeoutsMs: 0 for blocking calls
	// Note: If Send() returns false, you should call IsBroken() to
	//			see if Send() failed due to connection broken or timeout.
	virtual	bool Receive( DWORD& dwLen, LPBYTE pData, DWORD dwTimeoutsMs=30*1000) = 0;	
	//if pData is NULL,just wonder if there is data to receive;!!!
	// dwTimeoutsMs: 0 for blocking calls
	// Note: If Send() returns false, you should call IsBroken() to
	//			see if Send() failed due to connection broken or timeout.
	virtual bool RawReceive(DWORD& dwLen, LPBYTE pData, DWORD dwTimeoutsMs=30*1000) = 0;

	virtual bool IsReadable(DWORD dwTimeout) = 0;//in millisecond time out
	//can tell if there is data to recevie;if false;to invoke IsBroken to can tell if the sock failed
	//dwTimeout:0:blocking call;1:unblocking call;

	virtual void GetAddrinFrom(struct sockaddr_in * Infrom) = 0;
	// in Network byte order	
	virtual SOCKET GetSocketId() = 0;
	virtual int GetErrorNum() = 0;
	
	//set and get the value of TOS,return true if succeed otherwise return false;
	//for details,see ReleastNote.txt
	virtual bool SetTos(unsigned char chTos/*in*/) = 0;
	virtual bool GetTos(unsigned char& chTos/*out*/) = 0;
	
	//set and get the value of TTL,return true if succeed otherwise return false;
	virtual bool SetTtl(unsigned char chTtl/*in*/) = 0;
	virtual bool GetTtl(unsigned char& chTtl) = 0;

	virtual bool SetTcpNoDelay(bool bNoDelay=false) = 0; // setsockopt(level = IPPROTO_TCP, TCP_NODELAY)
	// enable/Disables the Nagle algorithm for send coalescing, suitable for telnet/rlogin/ssh (virtual terminal) appliations
	// default is false: congestion-avoidance algorithm works  by having no more than one outstanding unacknowledged 
	//					segment in the transmission channel while queueing up the rest of the smaller segments for later transmission.
	//					Another segment is sent only if enough new data is available to make up a maximum sized segment, 
	//					or if the outstanding data is acknowledged.
	// use fflush(GetSocketId()) to write all cached data immediately on demand if SetTcpNoDelay(false);

	virtual bool GetAddress(bool bPeer, unsigned long& ulIP, unsigned short& ushPort) = 0;
	// Get peer or local address of the current binding socket
	// Note: IP returned in NETWORK byte order, but port in host byte order
	virtual int ToString(char* szBuf, int nBufLen) = 0;
	// Output format: xxx.xxx.xxx.xxx:pppp
};

class  CCTSocketContext 
{
public:
	CCTSocketContext() ;
	virtual ~CCTSocketContext();
	virtual CCTSocketConnection* GetConnection() = 0;
	virtual LPVOID GetContext() = 0;
};

typedef void (*LPFN_SOCKSVR_WORKER)(CCTSocketContext* pCTSocketContext);

class CCTSocketServer 
{
public:
	static CCTSocketServer* _NewInstance(LPCSTR cpszServiceName=NULL);  // default is TCPSvc-Port

public:
	CCTSocketServer();
	virtual ~CCTSocketServer();
	virtual bool Init(LPCSTR cpszServicePort, int qLen=5) = 0;
	// pszTxProtocol: "tcp" or "udp"
	// Note: the support of "udp" has been moved to UdpSocket.
	virtual bool Init(unsigned short ushServicePort, int qLen=5) = 0;
	// Note: the support of "udp" has been moved to UdpSocket.
	virtual bool Service(LPFN_SOCKSVR_WORKER lpfnOnConnect, LPVOID pContext=NULL,
		int nOnConnectStackSize=0,//
		int nServicePollingTimeMs=6*1000) = 0; // how long the svr thread checking for exit
	virtual int GetSocketInfo(char* pszBuf, int nBufLen) = 0;
	virtual unsigned short GetServicePort() = 0;
	// return 0 on error
	// return port iff service is running.

	virtual int GetErrorNum() = 0;
	virtual SOCKET GetSocketId() = 0;
	virtual bool IsReadable(DWORD dwTimeout) = 0;//in millisecond time out
	//can tell if there is data to recevie;if false;to invoke IsBroken to can tell if the sock failed
	//dwTimeout:0:blocking call;1:unblocking call;

	//set and get the value of TOS,return true if succeed otherwise return false,default value 0;
	//for more details,see ReleaseNote.txt
	virtual bool SetTos(unsigned char chTos/*in*/) = 0;
	virtual bool GetTos(unsigned char& chTos/*out*/) = 0;
	
	//set and get the value of TTL,return true if succeed otherwise return false,default value 128;
	virtual bool SetTtl(unsigned char chTtl/*in*/) = 0;
	virtual bool GetTtl(unsigned char& chTtl) = 0;

	virtual bool GetAddress(unsigned long& ulIP, unsigned short& ushPort) = 0;
	// Get IP & port of the current binding socket
	// Note: IP returned in NETWORK byte order, but port in host byte order
};

class CCTSocketClient 
{
public:
	static CCTSocketClient* _NewInstance();

public:
	CCTSocketClient();
	virtual	~CCTSocketClient();
	virtual	bool Init(LPCTSTR cpszHost, LPCTSTR cpszSrvPort, NET_CODE* pNetCode=NULL) = 0;
	// Note: the support of "udp" has been moved to UdpSocket.
	// pszTxProtocol: "tcp" or "udp"
	// return true or false and detail error code NET_CODE* pNetCode
	virtual	bool Init(LPCTSTR cpszHost, unsigned short ushSrvPort, NET_CODE* pNetCode=NULL) = 0;
	// Note: the support of "udp" has been moved to UdpSocket.
	// pszTxProtocol: "tcp" or "udp"
	// return true or false and detail error code NET_CODE* pNetCode
	virtual	void Close() = 0;
	virtual	bool IsBroken() = 0;
	virtual void SetStream(bool bRaw=false) = 0;
	// take effects only if TxProtocol is TCP.
	// bRaw: default is false, set to true only when you want to re-assemble
	//		the packets sent through TCP stream on your own instead of
	//		replying on the underlying socket library to do the on your
	//		behalf.
	// Note: call SetStream() before any Send() / Receive() is called.	
	virtual	bool Send(DWORD dwLen, LPBYTE pData, DWORD dwTimeoutsMs=30*1000) = 0;
	// dwTimeoutsMs: 0 for blocking calls
	// Note: If Send() returns false, you should call IsBroken() to
	//			see if Send() failed due to connection broken or timeout.
	virtual	bool Receive(DWORD& dwLen, LPBYTE pData, DWORD dwTimeoutsMs=30*1000) = 0;
	//if pData is NULL,just wonder if there is data to receive;!!!
	// dwTimeoutsMs: 0 for blocking calls
	// Note: If Receive() returns false, you should call IsBroken() to
	//			see if Receive() failed due to connection broken or timeout.
	virtual bool RawReceive(DWORD& dwLen, LPBYTE pData, DWORD dwTimeoutsMs=30*1000) = 0;
	virtual int GetErrorNum() = 0;

	virtual int GetSocketInfo(char* pszBuf, int nbufLen) = 0;	
	virtual SOCKET GetSocketId() = 0;
	virtual bool IsReadable(DWORD dwTimeout) = 0;//in millisecond time out
	//can tell if there is data to recevie;if false;to invoke IsBroken to can tell if the sock failed
	//dwTimeout:0:blocking call;1:unblocking call;

	//set and get the value of TOS,return true if succeed otherwise return false;
	//for details,see ReleastNote.txt
	virtual bool SetTos(unsigned char chTos/*in*/) = 0;
	virtual bool GetTos(unsigned char& chTos/*out*/) = 0;
	
	//set and get the value of TTL,return true if succeed otherwise return false;
	virtual bool SetTtl(unsigned char chTtl/*in*/) = 0;
	virtual bool GetTtl(unsigned char& chTtl) = 0;

	virtual bool SetTcpNoDelay(bool bNoDelay=false) = 0; // setsockopt(level = IPPROTO_TCP, TCP_NODELAY)
	virtual bool GetTcpNoDelay(bool& bNoDelay/*IN/OUT*/) = 0; // setsockopt(level = IPPROTO_TCP, TCP_NODELAY)
	// enable/Disables the Nagle algorithm for send coalescing, suitable for telnet/rlogin/ssh (virtual terminal) appliations
	// default is false: congestion-avoidance algorithm works  by having no more than one outstanding unacknowledged 
	//					segment in the transmission channel while queueing up the rest of the smaller segments for later transmission.
	//					Another segment is sent only if enough new data is available to make up a maximum sized segment, 
	//					or if the outstanding data is acknowledged.
	virtual bool GetAddress(bool bPeer, unsigned long& ulIP, unsigned short& ushPort) = 0;
	// Get peer or local address of the current binding socket
	// Note: IP returned in NETWORK byte order, but port in host byte order
};

PROJECT_NS_END

#endif //_SOCKHEAD_H_
