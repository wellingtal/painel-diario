#ifndef _WRAP_PACKET_
#define _WRAP_PACKET_

#include "PacketBase.h"
#include <Stream/Buffer/Buffer.h>

namespace PacketWrap
{
    // WrapHeader
    bool WrapHeader(CBuffer* lpBuffer_In, unsigned short usUsed_In, 
        unsigned char cCMD_In, unsigned short usState_In, unsigned short usError_In);

    bool WrapHeader(char* lpBuffer_In, unsigned short usUsed_In, 
        unsigned char cCMD_In, unsigned short usState_In, unsigned short usError_In);

    // WrapCrypt Old
    bool WrapCryptOld(CBuffer* lpBuffer_In, unsigned short usUsed_In, 
        unsigned char cCMD_In, unsigned short usState_In, unsigned short usError_In);

    bool WrapCryptOld(char* lpBuffer_In, unsigned short usUsed_In, 
        unsigned char cCMD_In, unsigned short usState_In, unsigned short usError_In);

    bool WrapCryptOld(char* lpBuffer_In, unsigned short usUsed_In, 
        unsigned char cCMD_In, unsigned long dwTick);

    // WrapCrypt
    bool WrapCrypt(CBuffer* lpBuffer_In, unsigned short usUsed_In, 
        unsigned char cCMD_In, unsigned short usState_In, unsigned short usError_In);

    bool WrapCrypt(char* lpBuffer_In, unsigned short usUsed_In, 
        unsigned char cCMD_In, unsigned short usState_In, unsigned short usError_In);

    bool WrapCrypt(char* lpBuffer_In, unsigned short usUsed_In, 
        unsigned char cCMD_In, unsigned long dwTick);

    // WrapCompress
    bool WrapCompress(CBuffer* lpDstBuffer, const char* lpSourceData, unsigned short usSourceLength, 
        unsigned char cCMD_In, unsigned short usState_In, unsigned short usError_In);

    bool WrapCompress(char* lpDstBuffer, unsigned long& dwDstBufferSize_InOut, 
        const char* lpSourceData, unsigned short usSourceLength, 
        unsigned char cCMD_In, unsigned short usState_In, unsigned short usError_In);
};

#endif