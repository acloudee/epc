#ifndef __SockUtil_h__
#define __SockUtil_h__

#ifdef WIN32
#include <winsock2.h>
#elif defined(_LINUX_)
#include <string.h>
#include <netdb.h>
#include "NonWinType.h"
#else
#include <socklib.h>
#include <inetlib.h>
#include <stdioLib.h>
#include <errno.h>
#include <errnolib.h>
#include <ioLib.h>
#include <types.h>
#include <resolvLib.h>
#include "NonWinType.h"
#endif 

#ifdef _USE_NS_
namespace IPPhone {
#endif

#define NET_ERROR_UNKNOWN			-1
#define NET_ERROR_UNKNOWN_HOST		-2
#define NET_ERROR_UNKNOWN_PORT		-3
#define NET_ERROR_UNKNOWN_PROTOCOL	-4

SOCKET	PassiveSocket(LPCSTR pszService, int qLen, sockaddr_in& sin);
SOCKET	ClientConnectSocket(LPCSTR pszHost, LPCSTR pszService, sockaddr_in& sin, int* pnErrorCode);

// 1: Tx/Rx success
// 0: Tx/Rx failed due to time-outed
// -1: Tx/Rx failed
bool TcpSend(SOCKET s, DWORD dwLen, LPBYTE pbData, struct sockaddr* pSock, int nSockAddrSize, bool& bTimedout, bool bRaw=false);
bool TcpReceive(SOCKET s, DWORD& dwLen, LPBYTE pbData, struct sockaddr* pSock, int* pnSockAddrSize, bool& bTimedout, bool bRaw=false);
bool TcpReceive_1(SOCKET s, DWORD& dwLen, LPBYTE pbData, struct sockaddr* pSock, int* pnSockAddrSize, bool& bTimedout, bool bRaw=false);

void GetSocketAddress(SOCKET s, bool bPeer, char* pszAddress); // output format: xxx.xxx.xxx.xxx:pppp
bool GetSocketAddress(SOCKET s, bool bPeer, unsigned long& ulIP, unsigned short& ulPort);
	// Note: IP returned in NETWORK byte order, but port in host byte order

const char* _GetErrorMsg(int nErrorCode);

#ifdef _USE_NS_
} // namespace IPPhone
#endif

#endif
