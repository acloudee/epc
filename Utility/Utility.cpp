#include <stdio.h>

#ifdef WIN32
#include <winsock2.h>
#include <Ws2tcpip.h>
#else
#include "NonWinType.h"
#endif 

#include "./H/Integer.h"

bool CInteger::_IsNumber(PCSTR cpszValue, int nRadix)
{
	int a = 0;
	return sscanf((char *)cpszValue, "%d", &a);
}

int CInteger::_DecodeInt(PCSTR cpszValue)
{
	int a = 0;
	sscanf((char *)cpszValue, "%d", &a);
	return a;
}