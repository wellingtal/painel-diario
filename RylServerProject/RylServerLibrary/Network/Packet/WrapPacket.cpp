#include "stdafx.h"
#include "WrapPacket.h"
#include "PacketStatistics.h"

#include <Log/ServerLog.h>
#include <Stream/Buffer/Buffer.h>
#include <Network/XORCrypt/XORCrypt.h>
#include <Utility/Compress/MiniLZO/MiniLZOWrapper.h>


bool PacketWrap::WrapHeader(CBuffer* lpBuffer_In, unsigned short usUsed_In, unsigned char cCMD_In,
                                   unsigned short usState_In, unsigned short usError_In)
{
    if (WrapHeader(lpBuffer_In->wr_ptr(), usUsed_In, cCMD_In, usState_In, usError_In))
    {
        lpBuffer_In->wr_ptr(usUsed_In);
        return true;
    }

	return false;
}

bool PacketWrap::WrapCryptOld(CBuffer* lpBuffer_In, unsigned short usUsed_In, unsigned char cCMD_In,
                                  unsigned short usState_In, unsigned short usError_In)
{
    if (WrapCryptOld(lpBuffer_In->wr_ptr(), usUsed_In, cCMD_In, usState_In, usError_In))
    {
        lpBuffer_In->wr_ptr(usUsed_In);
        return true;
    }

    return false;
}


bool PacketWrap::WrapCrypt(CBuffer* lpBuffer_In, unsigned short usUsed_In, unsigned char cCMD_In,
                                  unsigned short usState_In, unsigned short usError_In)
{
    if (WrapCrypt(lpBuffer_In->wr_ptr(), usUsed_In, cCMD_In, usState_In, usError_In))
    {
        lpBuffer_In->wr_ptr(usUsed_In);
        return true;
    }

    return false;
}

bool PacketWrap::WrapCompress(CBuffer* lpDstBuffer, const char* lpSourceData, unsigned short usSourceLength, 
                              unsigned char cCMD_In, unsigned short usState_In, unsigned short usError_In)
{
    char* lpBuffer = lpDstBuffer->wr_ptr();
	unsigned long dwDstBufferSize_InOut = static_cast<unsigned long>(lpDstBuffer->remain());

    if (WrapCompress(lpBuffer, dwDstBufferSize_InOut, 
        lpSourceData, usSourceLength, cCMD_In, usState_In, usError_In))
    {
        // 압축된 길이를 알아야 하기 때문.
        lpDstBuffer->wr_ptr(dwDstBufferSize_InOut);
        return true;
    }

    return false;
}


bool PacketWrap::WrapHeader(char* lpBuffer_In, unsigned short usUsed_In, 
                            unsigned char cCMD_In, unsigned short usState_In, unsigned short usError_In)
{
    reinterpret_cast<PktBase*>(lpBuffer_In)->InitPtHead(usUsed_In, cCMD_In, usState_In, usError_In);
    CXORCrypt::GetInstance().EncodeHeader(lpBuffer_In + 1, sizeof(PktBase) - 1, 0, 0);
	return true;
}


bool PacketWrap::WrapCryptOld(char* lpBuffer_In, unsigned short usUsed_In, 
                           unsigned char cCMD_In, unsigned short usState_In, unsigned short usError_In)
{    
    CXORCrypt& Crypt = CXORCrypt::GetInstance();
	unsigned long dwCodePage = Crypt.GetCodePage(1);

	if(Crypt.EncodePacket(lpBuffer_In + sizeof(PktBase), usUsed_In - sizeof(PktBase), dwCodePage))
    {
        PktBase* lpPktBase = reinterpret_cast<PktBase*>(lpBuffer_In);

        lpPktBase->InitPtHead(usUsed_In, cCMD_In, usState_In, usError_In);
        lpPktBase->SetCodePage(dwCodePage);
	    lpPktBase->SetCrypt();

        // 헤더 인코딩
	    Crypt.EncodeHeader(lpBuffer_In + 1, sizeof(PktBase) - 1, 0, 0);
        return true;
    }

	return false;
}

bool PacketWrap::WrapCryptOld(char* lpBuffer_In, unsigned short usUsed_In, 
                           unsigned char cCMD_In, unsigned long dwTick)
{    
    CXORCrypt& Crypt = CXORCrypt::GetInstance();
	unsigned long dwCodePage = Crypt.GetCodePage(1);

	if(Crypt.EncodePacket(lpBuffer_In + sizeof(PktBase), usUsed_In - sizeof(PktBase), dwCodePage))
    {
        PktBase* lpPktBase = reinterpret_cast<PktBase*>(lpBuffer_In);

        lpPktBase->InitPtHead(usUsed_In, cCMD_In, dwTick);
        lpPktBase->SetCodePage(dwCodePage);
	    lpPktBase->SetCrypt();

        // 헤더 인코딩
	    Crypt.EncodeHeader(lpBuffer_In + 1, sizeof(PktBase) - 1, 0, 0);
        return true;
    }

	return false;
}

bool PacketWrap::WrapCrypt(char* lpBuffer_In, unsigned short usUsed_In, 
                           unsigned char cCMD_In, unsigned short usState_In, unsigned short usError_In)
{    
    CXORCrypt& Crypt = CXORCrypt::GetInstance();
	unsigned long dwCodePage = Crypt.GetCodePage();

	if(Crypt.EncodePacket(lpBuffer_In + sizeof(PktBase), usUsed_In - sizeof(PktBase), dwCodePage))
    {
        PktBase* lpPktBase = reinterpret_cast<PktBase*>(lpBuffer_In);

        lpPktBase->InitPtHead(usUsed_In, cCMD_In, usState_In, usError_In);
        lpPktBase->SetCodePage(dwCodePage);
	    lpPktBase->SetCrypt();

        // 헤더 인코딩
	    Crypt.EncodeHeader(lpBuffer_In + 1, sizeof(PktBase) - 1, 0, 0);
        return true;
    }

	return false;
}

bool PacketWrap::WrapCrypt(char* lpBuffer_In, unsigned short usUsed_In, 
                           unsigned char cCMD_In, unsigned long dwTick)
{    
    CXORCrypt& Crypt = CXORCrypt::GetInstance();
	unsigned long dwCodePage = Crypt.GetCodePage();

	if(Crypt.EncodePacket(lpBuffer_In + sizeof(PktBase), usUsed_In - sizeof(PktBase), dwCodePage))
    {
        PktBase* lpPktBase = reinterpret_cast<PktBase*>(lpBuffer_In);

        lpPktBase->InitPtHead(usUsed_In, cCMD_In, dwTick);
        lpPktBase->SetCodePage(dwCodePage);
	    lpPktBase->SetCrypt();

        // 헤더 인코딩
	    Crypt.EncodeHeader(lpBuffer_In + 1, sizeof(PktBase) - 1, 0, 0);
        return true;
    }

	return false;
}

bool PacketWrap::WrapCompress(char* lpDstBuffer, unsigned long& dwDstBufferSize_InOut, 
                              const char* lpSourceData, unsigned short usSourceLength, 
                              unsigned char cCMD_In, unsigned short usState_In, unsigned short usError_In)
{
	if(usSourceLength < sizeof(PktBase) || PktMaxLen < usSourceLength)
	{
        // 패킷 크기가 헤더 크기보다 작거나, 소스 크기가 최대 패킷 길이보다 큰 경우
		ERRLOG2(g_Log, "Cmd:0x%02x Invalid packet size, Can't compress now.",
			reinterpret_cast<const PktBase*>(lpSourceData)->GetCmd(), usSourceLength);		
	}
    else if(dwDstBufferSize_InOut < usSourceLength)
    {
        // 압축할 버퍼 크기가 소스 크기보다 작은 경우
        SERLOG1(g_Log, "Cmd:0x%02x Insufficient buffer size, Can't compress now.",
            reinterpret_cast<const PktBase*>(lpSourceData)->GetCmd());
    }
    else
    {            
        unsigned long dwArchiveSrcLen = usSourceLength - sizeof(PktBase);
        unsigned long dwArchiveDstLen = dwDstBufferSize_InOut - sizeof(PktBase);

        bool bCompressResult = 
            CMiniLZO::Compress(lpSourceData + sizeof(PktBase), dwArchiveSrcLen, 
            lpDstBuffer + sizeof(PktBase), &dwArchiveDstLen);

        // 전체 버퍼 크기를 맞춘다.
        dwArchiveDstLen += sizeof(PktBase);

        if (!bCompressResult)
        {
            // 압축 실패 로그를 남긴다.
		    SERLOG2(g_Log, "Cmd:0x%02x Packet compress failed. Packet length is %d.",
			    reinterpret_cast<const PktBase*>(lpSourceData)->GetCmd(), usSourceLength);
	    }
        else
        {
            // 압축후 패킷 통계를 낸다. 패킷 커맨드, 원본 크기, 압축본 크기를 넘긴다.
            CPacketStatistics::GetInstance().CheckCompression(cCMD_In, 
                usSourceLength, dwArchiveDstLen);
        }

        if (!bCompressResult || usSourceLength <= dwArchiveDstLen)
        {
            // 압축에 실패했거나, 압축한 후 크기가 같거나, 오히려 더 커진 경우이다
            // 데이터를 복사한 후에, WrapCrypt로 다시 작업한다.
            memcpy(lpDstBuffer, lpSourceData, usSourceLength);
            
            dwDstBufferSize_InOut = usSourceLength;

            return PacketWrap::WrapCrypt(lpDstBuffer, usSourceLength, 
                cCMD_In, usState_In, usError_In);
        }
        else
        {   
            PktBase* lpPktDst = reinterpret_cast<PktBase*>(lpDstBuffer);

            lpPktDst->InitPtHead(static_cast<PktBase::LengthType>(dwArchiveDstLen), 
                cCMD_In, usState_In, usError_In);

	        lpPktDst->SetCompress();

	        // 헤더 인코딩
	        CXORCrypt::GetInstance().EncodeHeader(lpDstBuffer + 1, sizeof(PktBase) - 1, 0, 0);	        

            dwDstBufferSize_InOut = dwArchiveDstLen;
            return true;
        }
    }

    return false;
}