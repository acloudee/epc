/*
History:
	01-14-2005	Adam Yao	Implement Ping function on Linux Version
*/

#include "sockhead.h"
#include "SocketClient.h"
#include "SocketServer.h"
 
#ifdef _WIN32
#elif defined(_LINUX_)
#include <stdio.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip_icmp.h>
#include <netinet/ip.h>
#else
#include <pingLib.h>
#endif

PROJECT_NS_BEGIN

#ifdef _WIN32
#elif defined(_LINUX_)
	static PCSTR s_cpszNetInterfaceName = "eth0";
#else // vxWorks
	#if (CPU == MIPS32)
		static PCSTR s_cpszNetInterfaceName = "esw0";
	#elif (CPU==ARMARCH4)
		static PCSTR s_cpszNetInterfaceName = "IPPhone0";
	#else 
		#error s_cpszNetInterfaceName is uncognized!
	#endif
#endif

int IP2String(unsigned long ulIP, PSTR pszBuf, int nBufLen)
{
	if (!pszBuf || nBufLen < 1) return 0;

	in_addr ipaddress;
	ipaddress.s_addr = ulIP;
	char* pszAddr = NULL;

#ifdef WIN32
	pszAddr = inet_ntoa(ipaddress);
#elif defined(_LINUX_)
	char addr[INET_ADDRSTRLEN];
	pszAddr = (PSTR)inet_ntop( AF_INET, &ipaddress, addr, sizeof(addr) );
#else
	char szAddress[32] = {0};
	pszAddr = szAddress;
	inet_ntoa_b(ipaddress, szAddress);
#endif
	if (pszAddr == NULL) return 0;
	int nLen = strlen(pszAddr);
	if (nBufLen > nLen) {
		memcpy(pszBuf, pszAddr, nLen+1);
		return nLen + 1;
	}
	else {
		*pszBuf = 0;
		return 0;
	}
}

bool DottedString2IP(const char* cpszDottedIp, unsigned long& ulIp)
{
	if (!cpszDottedIp || *cpszDottedIp == 0) return false;
#ifdef _WIN32
	if ( (ulIp = inet_addr((PSTR)cpszDottedIp)) == INADDR_NONE) {
		if (strcmp(cpszDottedIp, "255.255.255.255") ==0) ulIp = 0xffffffff;
		else return false;
	}
	// in network byte order already
	return true;
#else
	struct in_addr host;
	int nRc = inet_aton((char*)cpszDottedIp, &host);
	#ifdef _LINUX_
		if (nRc != 0) {
	#else
		if (nRc == OK) {
	#endif
			ulIp = host.s_addr;
			return true;
		}
		else return false;
#endif
}

bool GetLocalhostName(char* pName, int nNameLen)
{
	TRACE(LogDetail);
	if (!pName || nNameLen < 1) return false;

	return (gethostname(pName, nNameLen) == 0);
}

bool Hostname2IP(const char* cpszDnsName, unsigned long& ulIP)
{
	TRACE(LogDetail);
	
	if (cpszDnsName == NULL || *cpszDnsName == 0) return false;

	hostent *phe = NULL;
#ifdef _WIN32
	if (phe = gethostbyname(cpszDnsName))	{
		memcpy(&ulIP, phe->h_addr, phe->h_length);
		return true;
	}
#elif defined(_LINUX_)
	if (phe = gethostbyname(cpszDnsName)) {
		memcpy(&ulIP, phe->h_addr, phe->h_length);
		return true;
	}
#else
	int DestIp = hostGetByName((char*)cpszDnsName);
	//int DestIp = ERROR;
	//CDnsRunner* pDnsRunner = CDnsRunner::_GetInstance();
	//if (pDnsRunner) DestIp = pDnsRunner->HostGetByName((char*)cpszDnsName);
	if (DestIp != ERROR) {
		ulIP = (unsigned long)DestIp;
		return true;
	}

/*	if ( (phe = resolvGetHostByName(szHost,(char*)&hostname,sizeof(hostname))) != NULL  )
	{
		memcpy(&sin.sin_addr,phe->h_addr_list[0],phe->h_length);
		return IP2String(sin.sin_addr.s_addr,pszBuf,nBufLen);
	}
*/
#endif

	else if ((ulIP=inet_addr((PSTR)cpszDnsName)) != INADDR_NONE) return true;
	else return false;
}

int Hostname2IP(const char* cpszDnsName, PSTR pszBuf, int nBufLen)
{
	unsigned long ulIP;

	if (Hostname2IP(cpszDnsName, ulIP)) 
		return IP2String(ulIP, pszBuf, nBufLen);
	else return 0;
}

u_long GetNetMask()
{
	u_long ulMask = 0xffffffff; // 255.255.255.255
#ifdef _WIN32
	ulMask = htonl(0xffffff00);	// 255.255.255.0
#elif defined(_LINUX_)
	int skfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (skfd < 0) {
		MKLOG(FORMAT_LOG(LOGSRC, LogError, "Failed to create sock for retrieving netmask! errno (%d) -%s", errno, strerror(errno)));
	}
	else {
		struct ifreq ifr;
		strcpy(ifr.ifr_name, s_cpszNetInterfaceName); 
		if (ioctl(skfd, SIOCGIFNETMASK, &ifr) < 0) {
			MKLOG(FORMAT_LOG(LOGSRC, LogError, "Failed to get net mask from interface %s! errno (%d) - %s", s_cpszNetInterfaceName, errno, strerror(errno)));			
		}
		else {
			struct sockaddr_in *psin = (struct sockaddr_in *)&ifr.ifr_netmask;
			ulMask = psin->sin_addr.s_addr;
		}
		close(skfd);
	}
#else
	if (ifMaskGet((char*)s_cpszNetInterfaceName, (int*)&ulMask) == ERROR) {
		MKLOG(FORMAT_LOG(LOGSRC, LogError, "Failed to get net mask from interface %s", s_cpszNetInterfaceName));
	}
	else ulMask = htonl(ulMask);
#endif

	return ulMask;
}

u_long GetLocalhostIP()	// get localhost IP in network byte order
{
	u_long ulIP = 0;
	char szBuf[65];
#ifdef _LINUX_
	int skfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (skfd < 0) {
		MKLOG(FORMAT_LOG(LOGSRC, LogError, "Failed to create socket! errno (%d) - %s", errno, strerror(errno)));
		return 0;		
	}
	
	struct ifreq ifr;
	strcpy(ifr.ifr_name, s_cpszNetInterfaceName);
	if (ioctl(skfd, SIOCGIFADDR, &ifr) < 0) {
		MKLOG(FORMAT_LOG(LOGSRC, LogError, "Failed to get host IP! errno (%d) - %s", errno, strerror(errno)));
		unsigned char* cptr = (unsigned char*)&ulIP;
		*cptr = 127;
		*(cptr+3) = 1;
	}
	else {
		struct sockaddr_in *psin = (struct sockaddr_in *)&ifr.ifr_addr;
		ulIP = psin->sin_addr.s_addr;	
	}
	close(skfd);
	return ulIP;
#else
	if (GetLocalhostName(szBuf, 65) && Hostname2IP(szBuf, ulIP) ) return ulIP;
	else {
		unsigned char* cptr = (unsigned char*)&ulIP;
		*cptr = 127;
		*(cptr+3) = 1;
		return ulIP;
	}
#endif
}

bool IsNetMaskValid(u_long ulNetMask)
// ulNetMask is network byte order (BigEndian)
// network mask must be contiguous
// network mask must not be 0.0.0.0 or 255.255.255.255
{
	ulNetMask = ntohl(ulNetMask);
	return ((ulNetMask != 0) && (ulNetMask != ~0) && ((((ulNetMask ^ 0xFFFFFFFF) + 1) & (ulNetMask ^ 0xFFFFFFFF)) == 0));
}


bool IsPublicIP(u_long ulIP)
// Private IP:	192.168.xxx.xxx/10.xxx.xxx.xxx/172.[16-31].xxx.xxx
//				127.0.0.1, 0.0.0.0
// All others: public
{
	if (ulIP == 0) return false;

	unsigned char localhost[] = {127, 0, 0, 1};
	u_long* pulLocalhost = (u_long*)localhost;
	if (*pulLocalhost == ulIP) return false;

	unsigned char* cptr = (unsigned char*)&ulIP;
	if (*cptr == 10) return false;
	else if (*cptr == 192) {
		if (*(cptr+1) == 168) return false;
	}
	else if  (*cptr == 172) {
		cptr++;
		if (*cptr > 15 && *cptr < 32) return false;
	}

	return true;
}

bool IsLocalhostIPValid() // localhost is not 127.0.0.1, 255.255.255.255, 0.0.0.0
{
	u_long ulIP = GetLocalhostIP();
	if (ulIP == 0 || ulIP == 0xffffffff) return false;
	else {
		unsigned char localhost[] = {127, 0, 0, 1};
		u_long* pulLocalhost = (u_long*)localhost;
		if (*pulLocalhost == ulIP) return false;
		else return true;
	}
}

bool IsOnSameLanSegment(u_long ulIP)
//  return true if (ulIP & GetNetMask()) == GetLocalhostName() & GetNetMask())
{ 
	u_long ulNetMask = GetNetMask();
	return ((ulNetMask & GetLocalhostIP()) == (ulNetMask & ulIP));
}

#ifdef _LINUX_

#ifndef DEFDATALEN 
#define DEFDATALEN	56
#endif

#ifndef MAXIPLEN	
#define MAXIPLEN	60
#endif

#ifndef MAXICMPLEN
#define MAXICMPLEN	76
#endif

#ifndef ICMP_RECVFROM_INTERVAL
#define ICMP_RECVFROM_INTERVAL	1600			// 1.6 seconds
#endif

const char *const ping_msg_perm_denied_are_you_root = "Permission denied. (are you root?)";
const char *const ping_msg_cannot_create_raw_socket = "Cannot create a raw socket";
const char *const ping_msg_send_icmp_packet_error = "Failed to send a ICMP packet";
const char *const ping_msg_recvfrom_error = "Error on recvfrom function";

static int icmp_cksum(unsigned short *buf, int sz)
{
	int nLeft = sz;
	int nSum = 0;
	unsigned short *w = buf;
	unsigned short ans = 0;
	
	while (nLeft > 1)  {
		nSum += *w++;
		nLeft -= 2;
	}

	if (nLeft == 1) {
		*(unsigned short *)(&ans) = *(unsigned char *)w;
		nSum += ans;
	}

	nSum = (nSum >> 16) + (nSum & 0xFFFF);
	nSum += (nSum >> 16);
	ans = ~nSum;
	
	return ans;
}

bool _ping(const char * cpszHost, int nPackets)
{
	bool bAlive = false;
	int pingsock = -1;
	struct sockaddr_in 	pingaddr;
	struct hostent *host = NULL;
	struct icmp *pkt = NULL;
	char packet[DEFDATALEN + MAXIPLEN + MAXIPLEN];

	// create icmp socket
	struct protoent *proto = getprotobyname("icmp");
	if ((pingsock = socket(AF_INET, SOCK_RAW, (proto ? proto->p_proto : 1))) < 0) {
		if (errno == EPERM)
			perror(ping_msg_perm_denied_are_you_root);
		else 
			perror(ping_msg_cannot_create_raw_socket);
		goto _err;
	}

	// init addr
	memset(&pingaddr, 0, sizeof(struct sockaddr_in));
	pingaddr.sin_family = AF_INET;
	host = gethostbyname(cpszHost);
	if (!host) goto _err;
	memcpy(&pingaddr.sin_addr, host->h_addr, sizeof(pingaddr.sin_addr));
	
	// icmp packet
	pkt = (struct icmp *)packet;
	memset(pkt, 0, sizeof(packet));
	pkt->icmp_type = ICMP_ECHO;
	pkt->icmp_cksum = icmp_cksum((unsigned short *)pkt, sizeof(packet));
	
	int nRc;
	int i;
	for (i = 0; i < nPackets; i++) {
		nRc = sendto(pingsock, packet, sizeof(packet), 0, (struct sockaddr *)&pingaddr, sizeof(struct sockaddr_in));
		if (nRc < 0 || nRc != sizeof(packet)) {
			perror(ping_msg_send_icmp_packet_error);
			goto _err;
		}
	}

	// listen for replies for 5 seconds	
	for (i = 0; i < nPackets; i++) {
		struct sockaddr_in from;
		socklen_t fromlen = sizeof(socklen_t);

		fd_set  set;
		timeval tv;		
		tv.tv_sec = ICMP_RECVFROM_INTERVAL / 1000;  
		tv.tv_usec = (ICMP_RECVFROM_INTERVAL % 1000) * 1000;
		FD_ZERO(&set);
		FD_SET(pingsock, &set);
		int result = select(pingsock + 1, &set, NULL, NULL, &tv);

		//time out
		if (result == 0 || !FD_ISSET(pingsock, &set)) continue;	

		// listen for replies for 5 seconds	
		if ((nRc = recvfrom(pingsock, packet, sizeof(packet), 0, (struct sockaddr *)&from, &fromlen)) < 0) {
			perror(ping_msg_recvfrom_error);
			goto _err;
		}

		// ip + icmp
		if (nRc >= 76) {
			struct iphdr *iph = (struct iphdr *)packet;
			// skip IP header
			pkt = (struct icmp *)(packet + (iph->ihl << 2));
			if (pkt->icmp_type == ICMP_ECHOREPLY) {
				bAlive = true;
				break;
			}
		}
	}

_err:
	if (pingsock)
		close(pingsock);
	return bAlive;	
}

bool GetMACAddress(char *pszBuf, int nBufLen)
{
	if (!pszBuf || nBufLen < 1) return false;

	int skfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (skfd < 0) {
		MKLOG(FORMAT_LOG(LOGSRC, LogError, "Failed to create socket! errno (%d) - %s", errno, strerror(errno)));
		return false;		
	}

	struct ifreq ifr;
	strcpy(ifr.ifr_name, s_cpszNetInterfaceName);
	if (ioctl(skfd, SIOCGIFHWADDR, &ifr) < 0) {
		MKLOG(FORMAT_LOG(LOGSRC, LogError, "Failed to get host IP! errno (%d) - %s", errno, strerror(errno)));
		close(skfd);
		return false;
	}
	else {
		memset(pszBuf, 0, nBufLen);
		snprintf(pszBuf, nBufLen, "%02X%02X%02X%02X%02X%02X",
			ifr.ifr_hwaddr.sa_data[0] & 0xff, ifr.ifr_hwaddr.sa_data[1] & 0xff, ifr.ifr_hwaddr.sa_data[2] & 0xff,
			ifr.ifr_hwaddr.sa_data[3] & 0xff, ifr.ifr_hwaddr.sa_data[4] & 0xff, ifr.ifr_hwaddr.sa_data[5] & 0xff);
		close(skfd);
		return true;
	}

	return true;
}

#endif	// _LINUX_

bool Ping(PCSTR  cpszHost, int nNumPackets /*= 1*/)
{
	if (cpszHost == NULL || *cpszHost == 0) return false;
#ifdef _WIN32
	return true;
#elif defined(_LINUX_)
	return (cpszHost && nNumPackets && _ping(cpszHost, nNumPackets));
#else//vxWorks
	STATUS nRv = ping((PSTR)cpszHost, nNumPackets,PING_OPT_SILENT);
	return nRv == OK;
#endif
}

CCTSocketConnection::CCTSocketConnection()
{
}

CCTSocketConnection::~CCTSocketConnection()
{
}

CCTSocketClient::CCTSocketClient()
{
}

CCTSocketClient::~CCTSocketClient()
{
}

CCTSocketServer::CCTSocketServer()
{
}
CCTSocketServer::~CCTSocketServer()
{
}

CCTSocketContext::CCTSocketContext() 
{
}

CCTSocketContext::~CCTSocketContext()
{
};

CCTSocketServer* CCTSocketServer::_NewInstance(LPCSTR cpszServiceName)
{
	return (CCTSocketServer*)new CSocketServer(cpszServiceName);
}

CCTSocketClient* CCTSocketClient::_NewInstance()
{
	return (CCTSocketClient*)new CSocketClient();
}

PROJECT_NS_END

