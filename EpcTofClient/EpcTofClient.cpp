#include <stdio.h>
#include "../Network/H/sockhead.h"
#include "../Utility/H/Integer.h"

#include "EpcTofClient.h"

/*
#ifdef _DEBUG
#pragma comment(lib, "../Library/Win32/Debug_Static/Utility.lib")
#pragma comment(lib, "../Library/Win32/Debug_Static/Network.lib")
#else
#pragma comment(lib, "../Library/Win32/Release_Static/Utility.lib")
#pragma comment(lib, "../Library/Win32/Release_Static/Network.lib")
#endif
*/

#define __EPC_FUNC_NAME__ (__FUNCTION__+strlen("EpcTofClient::"))

int InitPlatform(void* pContext)
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	return 0;
}

void ShutdownPlatform()
{
	WSACleanup();
}

EpcTofClient::EpcTofClient() 
{
	strcpy_s(mHostIP, "192.168.7.2");
	mHostPort = 50660;
	mTcpClient = NULL;
}

EpcTofClient::~EpcTofClient()
{
	close();
}

bool EpcTofClient::init(char *cpszHost, int port)
{
	mTcpClient = (void *)CCTSocketClient::_NewInstance();
	if (mTcpClient == NULL) return false;

	int nNetCode;
//	printf("init1: %s:%d\n", cpszHost,  port);
	
	if (_init(cpszHost, port, &nNetCode)) {
		//printf("init OK\n");
		// store host and port
		strcpy_s(mHostIP, cpszHost);
		mHostPort = port;
		return true;
	}
	printf("init fail\n");
	
	return false;
}

bool EpcTofClient::init(char *cpszHost, char *cpszPort)
{
//	printf("init2: %s:%s\n", cpszHost,  cpszPort);
	
	if (CInteger::_IsNumber(cpszPort)) {
		printf("init2\n");
		return init(cpszHost, CInteger::_DecodeInt(cpszPort));
	}
	return false;
}


bool EpcTofClient::_init(char *cpszHost, int port, int *pNetCode /* = NULL */) 
{
	if (mTcpClient == NULL) return false;


	CCTSocketClient *pTcpClient = (CCTSocketClient *)mTcpClient;
	NET_CODE netCode;
	if (pTcpClient->Init(cpszHost, port, &netCode)) {
		pTcpClient->SetStream(true);
		return true;
	}
	return false;
}

void EpcTofClient::close()
{
	if (mTcpClient) {
		delete (CCTSocketClient *)mTcpClient;
		mTcpClient = NULL;
	}	
}

bool EpcTofClient::isBroken()
{
	if (mTcpClient) {
		return ((CCTSocketClient *)mTcpClient)->IsBroken();
	}
	return false;
}

bool EpcTofClient::send(unsigned long ulLen, unsigned char *pData, unsigned long ulTimeoutMs/* =30*1000 */)
{
	close();
	init(mHostIP, mHostPort);
	if (mTcpClient == NULL) return false;

	CCTSocketClient *pTcpClient = (CCTSocketClient *)mTcpClient;	
	return pTcpClient->Send(ulLen, pData, ulTimeoutMs);
}

bool EpcTofClient::receive(unsigned long &ulLen, unsigned char *pData, unsigned long ulTimeoutMs/* =30*1000 */)
{
	if (mTcpClient == NULL) return false;

	CCTSocketClient *pTcpClient = (CCTSocketClient *)mTcpClient;
	return pTcpClient->Receive(ulLen, pData, ulTimeoutMs);
}

unsigned short EpcTofClient::getVersion()
{
	return executeCmdForShort("version\n");
}

unsigned short EpcTofClient::getPartVersion()
{	
	sprintf(mCmd, "%s\n", __EPC_FUNC_NAME__);
	return executeCmdForShort(mCmd);
}

unsigned short EpcTofClient::getOffset()
{
	sprintf(mCmd, "%s\n", __EPC_FUNC_NAME__);
	return executeCmdForShort(mCmd);
}

unsigned short EpcTofClient::getIcVersion()
{	
	sprintf(mCmd, "%s\n", __EPC_FUNC_NAME__);
	return executeCmdForShort(mCmd);
}

unsigned short EpcTofClient::getAveragedTemperature()
{	
	sprintf(mCmd, "%s\n", __EPC_FUNC_NAME__);
	return executeCmdForShort(mCmd);
}

unsigned short EpcTofClient::loadConfig(int index) 
{
	sprintf(mCmd, "%s %d\n", __EPC_FUNC_NAME__, index);
	return executeCmdForShort(mCmd);
}

unsigned short EpcTofClient::setIntegrationTime3D(int timeInUs)
{
	sprintf(mCmd, "%s %d\n", __EPC_FUNC_NAME__, timeInUs);
	return executeCmdForShort(mCmd);
}

unsigned short EpcTofClient::setIntegrationTime3DHDR(int timeInUs)
{
	sprintf(mCmd, "%s %d\n", __EPC_FUNC_NAME__, timeInUs);
	return executeCmdForShort(mCmd);
}

unsigned short EpcTofClient::setIntegrationTime2D(int timeInUs)
{
	sprintf(mCmd, "%s %d\n", __EPC_FUNC_NAME__, timeInUs);
	return executeCmdForShort(mCmd);
}

unsigned short EpcTofClient::setModulationFrequency(int index) 
{
	sprintf(mCmd, "%s %d\n", __EPC_FUNC_NAME__, index);
	return executeCmdForShort(mCmd);
}

unsigned short EpcTofClient::getModulationFrequencies()
{
	// TODO
	return 0;
}

unsigned short EpcTofClient::setMinAmplitude(int value)
{
	sprintf(mCmd, "%s %d\n", __EPC_FUNC_NAME__, value);
	return executeCmdForShort(mCmd);
}

unsigned short EpcTofClient::getMinAmplitude()
{
	sprintf(mCmd, "%s\n", __EPC_FUNC_NAME__);
	return executeCmdForShort(mCmd);
}

unsigned short EpcTofClient::setABS(bool value){
	sprintf(mCmd, "%s %d\n", __EPC_FUNC_NAME__, value ? 1 : 0);
	return executeCmdForShort(mCmd);
}

unsigned short EpcTofClient::enableVerticalBinning(bool value){
	sprintf(mCmd, "%s %d\n", __EPC_FUNC_NAME__, value ? 1 : 0);
	return executeCmdForShort(mCmd);
}

unsigned short EpcTofClient::enableHorizontalBinning(bool value){
	sprintf(mCmd, "%s %d\n", __EPC_FUNC_NAME__, value ? 1 : 0);
	return executeCmdForShort(mCmd);
}

unsigned short EpcTofClient::setRowReduction(int value)
{
	sprintf(mCmd, "%s %d\n", __EPC_FUNC_NAME__, value);
	return executeCmdForShort(mCmd);
}

unsigned short EpcTofClient::enableDualMGX(bool value)
{
	sprintf(mCmd, "%s %d\n", __EPC_FUNC_NAME__, value ? 1 : 0);
	return executeCmdForShort(mCmd);
}

unsigned short EpcTofClient::enableHDR(bool value)
{
	sprintf(mCmd, "%s %d\n", __EPC_FUNC_NAME__, value ? 1 : 0);
	return executeCmdForShort(mCmd);
}

unsigned short EpcTofClient::enablePiDelay(bool value)
{
	sprintf(mCmd, "%s %d\n", __EPC_FUNC_NAME__, value ? 1 : 0);
	return executeCmdForShort(mCmd);
}

unsigned short EpcTofClient::setROI(int left_x, int left_y, int right_x, int right_y)
{
	sprintf(mCmd, "%s %d %d %d %d\n", __EPC_FUNC_NAME__, left_x, left_y, right_x, right_y);
	return executeCmdForShort(mCmd);
}

unsigned short EpcTofClient::calibratePixel(int value1, int value2)
{
	sprintf(mCmd, "%s %d %d\n", __EPC_FUNC_NAME__, value1, value2);
	return executeCmdForShort(mCmd);
}

unsigned short EpcTofClient::enableCalibration(bool value)
{
	sprintf(mCmd, "%s %d\n", __EPC_FUNC_NAME__, value ? 1 : 0);
	return executeCmdForShort(mCmd);
}

unsigned short EpcTofClient::enableCorrectionBG(bool value)
{
	sprintf(mCmd, "%s %d\n", __EPC_FUNC_NAME__, value ? 1 : 0);
	return executeCmdForShort(mCmd);
}

unsigned short EpcTofClient::restoreCalibration()
{
	sprintf(mCmd, "%s\n", __EPC_FUNC_NAME__);
	return executeCmdForShort(mCmd);
}

unsigned short EpcTofClient::setHysteresis(int value)
{
	sprintf(mCmd, "%s %d\n", __EPC_FUNC_NAME__, value);
	return executeCmdForShort(mCmd);
}

unsigned short EpcTofClient::selectMode(int value)
{
	sprintf(mCmd, "%s %d\n", __EPC_FUNC_NAME__, value);
	return executeCmdForShort(mCmd);
}

unsigned short EpcTofClient::setImageAveraging(int value)
{
	sprintf(mCmd, "%s %d\n", __EPC_FUNC_NAME__, value);
	return executeCmdForShort(mCmd);
}

unsigned short EpcTofClient::selectPolynomial(int value)
{
	sprintf(mCmd, "%s %d\n", __EPC_FUNC_NAME__, value);
	return executeCmdForShort(mCmd);
}

unsigned short EpcTofClient::setImageProcessing(int value)
{
	sprintf(mCmd, "%s %d\n", __EPC_FUNC_NAME__, value);
	return executeCmdForShort(mCmd);
}

unsigned short EpcTofClient::setImageDifferenceThreshold(int value)
{
	sprintf(mCmd, "%s %d\n", __EPC_FUNC_NAME__, value);
	return executeCmdForShort(mCmd);
}

unsigned short EpcTofClient::factoryCalibration(int value1, int value2)
{
	sprintf(mCmd, "%s %d %d\n", __EPC_FUNC_NAME__, value1, value2);
	return executeCmdForShort(mCmd);
}

unsigned short EpcTofClient::enableImageCorrection(bool value)
{
	sprintf(mCmd, "%s %d\n", __EPC_FUNC_NAME__, value ? 1 : 0);
	return executeCmdForShort(mCmd);
}

unsigned short EpcTofClient::enableSaturation(bool value)
{
	sprintf(mCmd, "%s %d\n", __EPC_FUNC_NAME__, value ? 1 : 0);
	return executeCmdForShort(mCmd);
}

unsigned short EpcTofClient::enableAdcOverflow(bool value)
{
	sprintf(mCmd, "%s %d\n", __EPC_FUNC_NAME__, value ? 1 : 0);
	return executeCmdForShort(mCmd);
}

unsigned short EpcTofClient::isFLIM()
{
	sprintf(mCmd, "%s\n", __EPC_FUNC_NAME__);
	return executeCmdForShort(mCmd);
}

unsigned short EpcTofClient::FLIMGetStep()
{
	sprintf(mCmd, "%s\n", __EPC_FUNC_NAME__);
	return executeCmdForShort(mCmd);
}

unsigned short EpcTofClient::FLIMSetT1(int value)
{
	sprintf(mCmd, "%s %d\n", __EPC_FUNC_NAME__, value);
	return executeCmdForShort(mCmd);
}

unsigned short EpcTofClient::FLIMSetT2(int value)
{
	sprintf(mCmd, "%s %d\n", __EPC_FUNC_NAME__, value);
	return executeCmdForShort(mCmd);
}

unsigned short EpcTofClient::FLIMSetT3(int value)
{
	sprintf(mCmd, "%s %d\n", __EPC_FUNC_NAME__, value);
	return executeCmdForShort(mCmd);
}

unsigned short EpcTofClient::FLIMSetT4(int value)
{
	sprintf(mCmd, "%s %d\n", __EPC_FUNC_NAME__, value);
	return executeCmdForShort(mCmd);
}

unsigned short EpcTofClient::FLIMSetTREP(int value)
{
	sprintf(mCmd, "%s %d\n", __EPC_FUNC_NAME__, value);
	return executeCmdForShort(mCmd);
}

unsigned short EpcTofClient::FLIMSetRepetitions(int value)
{
	sprintf(mCmd, "%s %d\n", __EPC_FUNC_NAME__, value);
	return executeCmdForShort(mCmd);
}

unsigned short EpcTofClient::FLIMSetFlashDelay(int value)
{
	sprintf(mCmd, "%s %d\n", __EPC_FUNC_NAME__, value);
	return executeCmdForShort(mCmd);
}

unsigned short EpcTofClient::FLIMSetFlashWidth(int value)
{
	sprintf(mCmd, "%s %d\n", __EPC_FUNC_NAME__, value);
	return executeCmdForShort(mCmd);
}

unsigned short EpcTofClient::searchBadPixels()
{
	sprintf(mCmd, "%s\n", __EPC_FUNC_NAME__);
	return executeCmdForShort(mCmd);
}

int EpcTofClient::dumpAllRegisters(char *pData, int nBufLen)
{
	// TODO	
	return 0;
}

unsigned short EpcTofClient::createBGTable(int value)
{
	sprintf(mCmd, "%s %d\n", __EPC_FUNC_NAME__, value);
	return executeCmdForShort(mCmd);
}

unsigned short EpcTofClient::dcsCorrection(char *cpszCorrection)
{
	sprintf(mCmd, "%s %s\n", __EPC_FUNC_NAME__, cpszCorrection);
	return executeCmdForShort(mCmd);
}

unsigned short EpcTofClient::calibrateGrayscale(int value)
{
	sprintf(mCmd, "%s %d\n", __EPC_FUNC_NAME__, value);
	return executeCmdForShort(mCmd);
}

unsigned short EpcTofClient::correctGrayscaleOffset(int value)
{
	sprintf(mCmd, "%s %d\n", __EPC_FUNC_NAME__, value);
	return executeCmdForShort(mCmd);
}

unsigned short EpcTofClient::correctGrayscaleGain(int value)
{
	sprintf(mCmd, "%s %d\n", __EPC_FUNC_NAME__, value);
	return executeCmdForShort(mCmd);
}

unsigned short EpcTofClient::calibrateDRNU()
{
	sprintf(mCmd, "%s\n", __EPC_FUNC_NAME__);
	return executeCmdForShort(mCmd);
}

unsigned short EpcTofClient::correctDRNU(int value)
{
	sprintf(mCmd, "%s %d\n", __EPC_FUNC_NAME__, value);
	return executeCmdForShort(mCmd);
}

bool EpcTofClient::isDRNUEnabled()
{
	sprintf(mCmd, "%s\n", __EPC_FUNC_NAME__);
	return executeCmdForShort(mCmd) == 1 ? true : false;
}

bool EpcTofClient::isGrayscaleCorrectionEnabled()
{
	sprintf(mCmd, "%s\n", __EPC_FUNC_NAME__);
	return executeCmdForShort(mCmd) == 1 ? true : false;
}

bool EpcTofClient::isCalibrationAvailable()
{
	sprintf(mCmd, "%s\n", __EPC_FUNC_NAME__);
	return executeCmdForShort(mCmd) == 1 ? true : false;
}

unsigned short EpcTofClient::executeCmdForShort(char *cpszCmd)
{
	send(strlen(cpszCmd), (unsigned char *)cpszCmd, 30*1000);

	unsigned short usResult = 0;
	unsigned long ulLen = sizeof(unsigned short);
	receive(ulLen, (unsigned char *)&usResult, 30*1000);
	return usResult;
}

unsigned short EpcTofClient::amplifyimage_mixtogether(char *indepth, unsigned int x, unsigned int y, unsigned int w, unsigned int h, int type)
{
	unsigned short *in = (unsigned short *)indepth;
	int val = 0;

	if (x == w)
		x -= 1;
	if (y == h)
		y -= 1;

	if (type == 1) {//x1+x2
		val = (int)(in[y*w + x] + in[y*w + (x + 1)]) / 2;
	}
	else if (type == 2) {//y1+y2
		val = (int)(in[y*w + x] + in[(y + 1)*w + x]) / 2;
	}
	else if (type == 3) {//x1+x2 y1+y2
		val = (int)(in[y*w + x] + in[y*w + (x + 1)] + in[(y + 1)*w + x] + in[(y + 1)*w + (x + 1)]) / 4;
	}

	return (unsigned short)val;
}

int EpcTofClient::amplifyimage(char *indepth, char *outdepth, unsigned int size)
{
	int x, y, w, h;
	int x1, y1, w1, h1;
	int xr, yr;
	unsigned short *in = (unsigned short *)indepth;
	unsigned short *out = (unsigned short *)outdepth;

	w1 = 320;
	h1 = 240 * (size / (320 * 240 * 2));
	w = w1 * 2;
	h = h1 * 2;

	for (y = 0; y < h; y++) {
//		if (y< 10)
//			printf("\n");
		y1 = y / 2;
		yr = y % 2;
		for (x = 0; x < w; x++) {
			x1 = x / 2;
			xr = x % 2;



			if (yr == 0 && xr == 0)
				*out = in[y1*w1 + x1];
			else
				*out = amplifyimage_mixtogether(indepth, x1, y1, w1, h1, (yr << 1) | xr);

			out++;
		}
	}
	return w * h * 2;
}

int EpcTofClient::paserCmd(char *cmdsrc, char *cmddst)
{
	char *ptr = cmdsrc;
	if (strlen(cmdsrc) > 3)
		ptr += (strlen(cmdsrc) - 3);

	mImage480Enable = 0;
	if (strcmp(ptr, "480") == 0) {
		mImage480Enable = 1;
		memcpy(cmddst, cmdsrc, strlen(cmdsrc)-3);
		cmddst[strlen(cmdsrc) - 3] = '\0';
	}
	else {
		strcpy(cmddst, cmdsrc);
	}
	return 0;
}

unsigned long EpcTofClient::getEpcBuffer(char *cpszType, char *szBuf, unsigned long ulLen)
{
	char *pData = szBuf;
	unsigned long size = ulLen;
	// b1. send command
	char szCmd[256];
	paserCmd(cpszType, (char *)szCmd);
	send(strlen(szCmd), (unsigned char *)szCmd, 30 * 1000);

	if (mImage480Enable) {
		size = 320 * 240 * 2;
		pData = new char[size];
	}

	// 2. receive response
	unsigned long nTotalLen = 0;
	do {
		if (receive(size, (unsigned char *)pData, 30 * 1000)) {
			nTotalLen += size;
		}
	} while (!isBroken());
	printf("epc %s=%d, ulLen = %d\n", szCmd, nTotalLen, size);

	if (mImage480Enable) {
		nTotalLen = amplifyimage(pData, szBuf, nTotalLen);
		delete pData;
	}
	return nTotalLen;
}

