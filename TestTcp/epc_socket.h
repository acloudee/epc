class EpcSocket
{
public:
	EpcSocket();
	virtual ~EpcSocket();

public:
	// socket raw operations
	bool init(char *cpszHost, int port);
	bool init(char *cpszHost, char *cpszPort);
	void close();

	bool send(unsigned long ulLen, unsigned char *pData, unsigned long ulTimeoutMs = 30 * 1000);
	bool receive(unsigned long &ulLen, unsigned char *pData, unsigned long ulTimeoutMs = 30 * 1000);

	bool isBroken();


}