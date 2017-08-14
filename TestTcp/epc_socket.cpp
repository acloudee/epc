bool EpcTofClient::init(char *cpszHost, int port)
{
	mTcpClient = (void *)CCTSocketClient::_NewInstance();
	if (mTcpClient == NULL) return false;

	int nNetCode;
	//	printf("init1: %s:%d\n", cpszHost,  port);

	if (_init(cpszHost, port, &nNetCode)) {
		//printf("init OK\n");
		// store host and port
		strcpy(mHostIP, cpszHost);
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

void main()
{
	WSADATA wsaData;
	SOCKET sockClient;//�ͻ���Socket
	SOCKADDR_IN addrServer;//����˵�ַ
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	//�½��ͻ���socket
	sockClient = socket(AF_INET, SOCK_STREAM, 0);
	//����Ҫ���ӵķ���˵�ַ
	addrServer.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");//Ŀ��IP(127.0.0.1�ǻ��͵�ַ)
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(6000);//���Ӷ˿�6000
	//���ӵ������
	connect(sockClient, (SOCKADDR*)&addrServer, sizeof(SOCKADDR));
	//��������
	char message[20] = "HelloSocket!";
	send(sockClient, message, strlen(message) + 1, 0);
	//�ر�socket
	closesocket(sockClient);
	WSACleanup();
}