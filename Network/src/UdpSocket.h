/*
UDP socket interface:
Note:if you want include UdpSocket.h  
in you file,you MUST include it prior to other head file,
otherwise there will be conflict with other files.
for exampel:
#include "UdpSocket.h"

#include "MyOtherHead.h"

*/

#ifndef _UDPSOCKET_H
#define _UDPSOCKET_H

#ifdef WIN32
	#include <winsock2.h>
	#include "winerrno.h"
	#include  "../../utility/h/Common.h"
#else
	#include "NonWinType.h"
#endif

// Definitions for IP type of service (ip_tos)
#define	IPTOS_LOWDELAY		0x10
#define	IPTOS_THROUGHPUT	0x08
#define	IPTOS_RELIABILITY	0x04
#define	IPTOS_MINCOST		0x02

// Definitions for IP precedence (also in ip_tos) (hopefully unused)
#define	IPTOS_PREC_NETCONTROL		0xe0
#define	IPTOS_PREC_INTERNETCONTROL	0xc0
#define	IPTOS_PREC_CRITIC_ECP		0xa0
#define	IPTOS_PREC_FLASHOVERRIDE	0x80
#define	IPTOS_PREC_FLASH		0x60
#define	IPTOS_PREC_IMMEDIATE		0x40
#define	IPTOS_PREC_PRIORITY		0x20
#define	IPTOS_PREC_ROUTINE		0x00

#ifdef _WIN32
namespace IPPhone
{
#endif

bool SockInit();
void SockClear();

int IP2String(u_long ulIP, PSTR pszBuf, int nBufLen);
// ulIP:	passed in parameter in NETWORK byte order. 
bool GetHostName(char* pszBuf, int nBufLen);	// get localhost name
u_long GetHostIP();	// get localhost IP in network byte order, return 127.0.0.1 if no valid IP available
bool IsPublicIP(u_long ulIP); 
// Private IP:	192.168.xxx.xxx/10.xxx.xxx.xxx/172.[16-31].xxx.xxx
//				127.0.0.1  0.0.0.0
// All others: public
bool IsHostIPValid(); // localhost is not 127.0.0.1, 255.255.255.255, 0.0.0.0
bool Name2IP(PCSTR cpszDnsName, u_long& ulIp);
// resolve domain name to NETWORK byte order IP
int Name2IP(PCSTR cpszDnsName,PSTR pszBuf, int nBufLen);
// resolve domain name to dotted IP string, a.b.c.d
// return 0 for failed! otherwise return the lenghth of bytes writtern into pszBuf
bool String2Ip(PCSTR cpszDottedIp, u_long& ulIp);
// Note: output in Network byte order.
u_long GetNetMask(); // 


//test if the remote host is active;If no replies are received 
//within a 5-second timeout period, the routine exits,return false
//only available on Vxworks currently
// pHost: host to ping 
// nNumPackets: number of packets to receive 
bool Ping(PCSTR cpszHost, int nNumPackets = 1);

class CUdpSocket 
{
public:
	static CUdpSocket* _NewInstance();
	static int _IP2String(u_long ulIP, PSTR pszBuf, int nBufLen); 
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
public:
	CUdpSocket();
	virtual	~CUdpSocket();
	virtual	bool Init(PCSTR cpszListenPort, bool bBroadcast=false) = 0;	
	// cpszListenPort: NULL for auto-assignment
	// bBroadcast: to Tx and Rx broadcast packets for not, default is false;
	virtual	bool Init(unsigned short ushListenPort, bool bBroadcast=false) = 0; 
	// ushListenPort: zero for auto-assignment
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
	virtual	NET_CODE Send(PCSTR cpszDestAddr, 
				unsigned short ushDestPort,
				DWORD dwLen, LPBYTE pData, 
				 char chTos=0,				
				DWORD dwTimeoutsMs=3*1000) = 0;
	virtual	NET_CODE Send(unsigned long ulDestIP,  // dest IP in NETWORK byte order
				unsigned short ushDestPort,
				DWORD dwLen, LPBYTE pData,
				 char chTos=0,				
				DWORD dwTimeoutsMs=3*1000) = 0;
	// cpszDestAddr:could be IP, xxx.xxx.xxx.xxx or Domain name
	// char chTos=0;	// type of service, highest, 0-7 lowest
	// dwTimeoutsMs: 0 for blocking calls,1 for unblocking calls;
	// Note: the resolution from domain name to IP should be cached inside udp 

	// char chTos=0;	// type of service, highest, 0-7 lowest
	// dwTimeoutsMs: 0 for blocking calls,1 for unblocking calls;
	// Note: the resolution from domain name to IP should be cached inside udp 
	//return value:0 stands for success,
	//				-1 stands for invalid socket,
	//				-2 for unkown host
	//				1 stands for timeout or other reasons
	//dwTimeoutsMs:time out value in millisecond,1 for return immediately
	virtual	NET_CODE Receive(unsigned long& ulIP, 
				unsigned short& ushSrcPort,		
				DWORD& dwLen, LPBYTE pData, 
				DWORD dwTimeoutsMs=3*1000) = 0;				
	// ulIP: return the source IP in NETWORK byte order
	// ushSrcPort: return the source port in HOST byte order.
	// dwTimeoutsMs: 0 for blocking calls
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
	100 - Flash Override	// IPTOS_PREC_FLASHOVERRIDE	0x80
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
};

#ifdef _WIN32
}//namespace
#endif

#endif//_UDPSOCKET_H
