#include "ITUdpSocket.h"


PROJECT_NS_BEGIN

CUdpSocket* CUdpSocket::_NewInstance()
{
	return (CUdpSocket*)new CITUdpSocket();
}

CUdpSocket::CUdpSocket()
{
}

CUdpSocket::~CUdpSocket()
{
}


PROJECT_NS_END

