#ifndef __Misc_h__
#define __Misc_h__

#include "Common.h"

PROJECT_NS_BEGIN

class CMisc
{
public:
	~CMisc();

	static BYTE _ToHex(char chHigh, char chLow);
	static bool _HexString2MAC(PCSTR cpszMacInHex /*IN*/, BYTE mac[6] /*OUT*/);

private:
	CMisc();
};

PROJECT_NS_END
#endif

