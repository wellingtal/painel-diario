#ifndef _CRC8STATIC_H_
#define _CRC8STATIC_H_

#include "Common.h"

class CCrc8Static
{
public:

    static unsigned char StringCrc8(const char *szString) { return BufferCrc8(szString, strlen(szString)); }
	static unsigned char BufferCrc8(const char *szBuffer, size_t size);

protected:
    
	static unsigned char s_arrcCrc8Table[256];
};

#endif
