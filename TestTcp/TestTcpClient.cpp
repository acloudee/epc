#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#include "../EpcTofClient/EpcTofClient.h"

#pragma comment(lib, "../Library/Win32/Debug_Static/Utility.lib")
#pragma comment(lib, "../Library/Win32/Debug_Static/Network.lib")
#pragma comment(lib, "../Library/Win32/Debug_Static/EpcTofClient.lib")

void getEpcData(EpcTofClient *epcClient,char *cpszType, char *filename)
{
	FILE* fp = NULL;

	// 0. store data
	if (filename != NULL) {
		fp = fopen(filename, "wb");
		if (fp == NULL) {
			printf("failed to create file %s\n", filename);
			return;
		}
	}

	// b1. send command
	char szCmd[256];
	sprintf(szCmd, "%s\n", cpszType);	
	epcClient->send(strlen(szCmd), (unsigned char *)szCmd, 30*1000);
	// 2. receive response
	char szBuf[153600];			
	int nTotalLen = 0;
	do {
		unsigned long ulLen = 153600;
		if (epcClient->receive(ulLen, (unsigned char *)szBuf, 30*1000)) {
			if(fp != NULL)
				fwrite(szBuf, sizeof(char), ulLen, fp);

			nTotalLen += ulLen;
			//printf("ecpData: len=%d\n", ulLen);
		}				
	} while (!epcClient->isBroken());
	printf("epc %s=%d\n", cpszType, nTotalLen);

	if (fp != NULL) {
		fclose(fp);
		fp = NULL;
	}
}

void getEpcData2(EpcTofClient *epcClient, char *cpszType, char *filename)
{
	unsigned long ulLen = 640 * 480 * 2 * 2;
	char *szBuf = new char[ulLen];
	unsigned long nTotalLen = epcClient->getEpcBuffer(cpszType, (char *)szBuf, ulLen);
	printf("epc %s=%d\n", cpszType, nTotalLen);

	if (filename != NULL) {
		FILE* fp = NULL;
		fp = fopen(filename, "wb");
		if (fp == NULL) {
			printf("failed to create file %s\n", filename);
			return;
		}
		fwrite(szBuf, sizeof(char), nTotalLen, fp);
		fclose(fp);
		fp = NULL;
	}

	delete szBuf;

	
	return;
}

void testEpcClient(const char *cpszHost, char* port)
{
	EpcTofClient *epcClient = new EpcTofClient();
	if (epcClient == NULL) return ;
	
	if (epcClient->init((char*)cpszHost, port)) {
		int nVersion = epcClient->getVersion();
		printf("epc version = %d\n", nVersion);

		nVersion = epcClient->getPartVersion();
		printf("epc getPartVersion = %d\n", nVersion);

		nVersion = epcClient->getIcVersion();
		printf("epc getIcVersion = %d\n", nVersion);

		int nTemp = epcClient->getAveragedTemperature();
		printf("epc getAveragedTemperature = %d\n", nTemp);

		int nOffset = epcClient->getOffset();
		printf("epc getOffset = %d\n", nOffset);

		int rc = epcClient->loadConfig(0);
		printf("epc loadConfig(0) = %d\n", rc);

		rc = epcClient->loadConfig(1);
		printf("epc loadConfig(1) = %d\n", rc);

		int timeInUs = 1500;
		epcClient->setIntegrationTime3D(timeInUs);

		int minAmp = 100;
		epcClient->setMinAmplitude(minAmp);
		minAmp = epcClient->getMinAmplitude();
		printf("epc minAmp = %d\n", minAmp);

		bool value = 1;
		epcClient->enablePiDelay(value);
		value = epcClient->isCalibrationAvailable();
		printf("iscali = %d\n", value);

		getEpcData2(epcClient, "getDistanceSorted", "epc_getDistanceSorted_new.bin");
		getEpcData2(epcClient, "getAmplitudeSorted", "epc_getAmplitudeSorted_new.bin");

		getEpcData2(epcClient, "getDistanceSorted480", "epc_getDistanceSorted480_new.bin");
		getEpcData2(epcClient, "getAmplitudeSorted480", "epc_getAmplitudeSorted480_new.bin");

		printf("exit\n");
		epcClient->close();

	}

	delete epcClient;	
}

int main(int argc, char **argv)
{
	/*
	if (argc < 3) {
		printf("Please input target server and port\n");
		return -1;
	}
	*/

	InitPlatform(0);

	char* strServer = "192.168.7.2";
	char* strPort = "50660";
	
	//printf("Server = %s\n", strServer);
	//printf("Port   = %s\n", strPort);

	testEpcClient(strServer, strPort);	

	ShutdownPlatform();
	getchar();

	return 0;
}


