#include <stdio.h>
//#include <unistd.h>
#include "../../../Utility/H/Thread.h"
#include "../../../Utility/H/Integer.h"
#include "../../H/sockhead.h"

void _OnConnect(CCTSocketContext *pSockCtx)
{
	CCTSocketConnection *pHttpConn = (CCTSocketConnection *)pSockCtx->GetConnection();

	u_long ulIP;
	u_short ushPort;
	char szIP[32];
	pHttpConn->GetAddress(true, ulIP, ushPort);
	IP2String(ulIP, szIP, 32);
	printf("-----[%s:%d] Connected------\n", szIP, ushPort);

	char szMsg[128];
	int nSent = 0;
	while (true) {
		sprintf(szMsg, "I am %d TCP message\n", nSent++);
		pHttpConn->Send(sizeof(szMsg), (PBYTE)szMsg);
		CThread::Delay(10 * 1000);
	}

	printf("OnConnect done\n");
}

int main(int argc, char **argv)
{
	int nPort = 56447;	// default
	if (argc >= 2 && CInteger::_IsNumber(argv[1])) {
		nPort = CInteger::_DecodeInt(argv[1]);
	}

	CCTSocketServer *pTcpSvr = CCTSocketServer::_NewInstance("TcpSvr");
	if (pTcpSvr) {
		if (pTcpSvr->Init(nPort, 5) && pTcpSvr->Service(_OnConnect, NULL)) {
			printf("Tcp Server Run On %d\n", nPort);
#ifdef _LINUX_
			int nSleep = 0;
			while (true) {
				nSleep++;
				CThread::Delay(1000);
				//printf("Sleep %d seconds\n", nSleep);
			}
#endif
		}		
	}

	printf("Start TCP Server Error\n");
	return 0;
}