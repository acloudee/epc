/*
*/

#ifndef __EPCTOFCLIENT_H__
#define __EPCTOFCLIENT_H__

int InitPlatform(void* pContext);	
void ShutdownPlatform();

class EpcTofClient 
{
public:
	EpcTofClient();
	virtual ~EpcTofClient();

public:
	// socket raw operations
	bool init(char *cpszHost, int port);
	bool init(char *cpszHost, char *cpszPort);
	void close();	

	bool send(unsigned long ulLen, unsigned char *pData, unsigned long ulTimeoutMs=30*1000);
	bool receive(unsigned long &ulLen, unsigned char *pData, unsigned long ulTimeoutMs=30*1000);

	bool isBroken();

public: 
	// epc tof operations
	unsigned short getVersion();
	unsigned short getIcVersion();
	unsigned short getPartVersion();
	// getChipInfo();
	unsigned short getTemperature(char *pData, int nBufLen);
	unsigned short getAveragedTemperature();
	// 0-false, 1-true
	bool isCalibrationAvailable();
	unsigned short getChipInfo(char *pData, int nBufLen);

	unsigned short setOffset(int value);
	unsigned short getOffset();
	unsigned short enableDefaultOffset(int value);

	int getCalibrationList(char *pData, int nBufLen);

	//int getBadPixels();

	// readRegister/read/r
	// writeRegister/write/w


	unsigned short loadConfig(int index);
	unsigned short enableIllumination(bool enable);
	unsigned short setIntegrationTime3D(int timeInUs);
	unsigned short setIntegrationTime3DHDR(int timeInUs);
	unsigned short setIntegrationTime2D(int timeInUs);

	unsigned short setMinAmplitude(int value);
	unsigned short getMinAmplitude();

	/*
		0 - 24.00 MHZ
		1 - 12.00 MHZ
		2 - 6.00 MHZ
		3 - 3.00 MHZ
		4 - 1.50 MHZ
		5 - 0.75 MHZ
	*/
	unsigned short setModulationFrequency(int index);
	unsigned short getModulationFrequencies();
	
	unsigned short setABS(bool value);
	unsigned short enableVerticalBinning(bool value);
	unsigned short enableHorizontalBinning(bool value);
	unsigned short setRowReduction(int value);
	unsigned short enableDualMGX(bool value);
	unsigned short enableHDR(bool value);
	unsigned short enablePiDelay(bool value);

	unsigned short setROI(int left_x, int left_y, int right_x, int right_y);
	unsigned short calibratePixel(int value1, int value2);

	unsigned short enableCalibration(bool value);
	unsigned short enableCorrectionBG(bool value);
	unsigned short setCorrectionBG(int value1, int value2, int value3);

	unsigned short restoreCalibration();

	unsigned short setHysteresis(int value);
	unsigned short selectMode(int value);
	unsigned short selectPolynomial(int value);
	unsigned short factoryCalibration(int value1, int value2);
	unsigned short enableImageCorrection(bool value);
	unsigned short setImageAveraging(int value);
	unsigned short setImageProcessing(int value);
	unsigned short setImageDifferenceThreshold(int value);
	unsigned short enableSaturation(bool value);
	unsigned short enableAdcOverflow(bool value);

	unsigned short isFLIM();
	unsigned short FLIMSetT1(int value);
	unsigned short FLIMSetT2(int value);
	unsigned short FLIMSetT3(int value);
	unsigned short FLIMSetT4(int value);
	unsigned short FLIMSetTREP(int value);
	unsigned short FLIMSetRepetitions(int value);
	unsigned short FLIMSetFlashDelay(int value);
	unsigned short FLIMSetFlashWidth(int value);
	unsigned short FLIMGetStep();

	// return -1 or 0
	unsigned short searchBadPixels();

	int dumpAllRegisters(char *pData, int nBufLen);

	unsigned short createBGTable(int value);
	unsigned short dcsCorrection(char *cpszCorrection);
	unsigned short calibrateGrayscale(int value);
	unsigned short correctGrayscaleOffset(int value);
	unsigned short correctGrayscaleGain(int value);
	unsigned short calibrateDRNU();
	unsigned short correctDRNU(int value);
	bool isDRNUEnabled();
	bool isGrayscaleCorrectionEnabled();
	unsigned long getEpcBuffer(char *cpszType, char *szBuf, unsigned long ulLen);

private:
	bool _init(char *cpszHost, int port, int *pNetCode = NULL);
	unsigned short executeCmdForShort(char *cpszCmd);
	int amplifyimage(char *indepth, char *outdepth, unsigned int size);
	unsigned short amplifyimage_mixtogether(char *indepth, unsigned int x, unsigned int y, unsigned int w, unsigned int h, int type);
	int paserCmd(char *cmdsrc, char *cmddst);

private:
	char mHostIP[256];
	int mHostPort;
	void *mTcpClient;

	char mCmd[256];
	int mImage480Enable;
};

#endif //__EPCTOFCLIENT_H__

