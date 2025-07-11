#include "stdafx.h"
#include "SendStream.h"

#include <cassert>

#include <Log/ServerLog.h>
#include <Stream/Buffer/Buffer.h>
#include <Stream/Buffer/BufferFactory.h>

#include <Network/Session/Session.h>
#include <Network/Packet/WrapPacket.h>
#include <Network/Packet/PacketStatistics.h>

#include <Utility/Compress/MiniLZO/MiniLZOWrapper.h>

enum SendStreamConst
{
    DEFAULT_BUFFER_SIZE	= 16384
};

CSendStream::CSendStream(CSession& Session)
:   m_Session(Session), m_lpBuffer(0)
{
    
}


CSendStream::~CSendStream()
{
    SAFE_RELEASE_BUFFER(m_lpBuffer);
}


bool CSendStream::ForceSend()
{    
    CBuffer* lpBuffer = 0;
    
    if(0 != m_lpBuffer)
    {                
        if(0 == m_lpBuffer->length())
        {
            return true;
        }
        else
        {
			std::swap(m_lpBuffer, lpBuffer);
			if(m_Session.SendPending(lpBuffer))
			{
				return m_Session.Send();
			}
			else
			{
				SAFE_RELEASE_BUFFER(lpBuffer);
			}
        }
    }

    return false;
}


char* CSendStream::GetBuffer(unsigned long dwReserve_In)
{
	unsigned long dwAllocateSize = DEFAULT_BUFFER_SIZE < dwReserve_In ? dwReserve_In : DEFAULT_BUFFER_SIZE;

    if(0 == m_lpBuffer)
    {
		m_lpBuffer = CREATE_BUFFER(m_Session.GetPolicy().GetBufferFactory(), dwReserve_In);
    }
	else if(m_lpBuffer->remain() < dwReserve_In)
	{
		if(0 < m_lpBuffer->length())
		{
            m_Session.SendPending(m_lpBuffer);
		}
		else
		{
			SAFE_RELEASE_BUFFER(m_lpBuffer);			
		}

		m_lpBuffer = CREATE_BUFFER(m_Session.GetPolicy().GetBufferFactory(), dwReserve_In);
	}

	return (0 != m_lpBuffer) ? m_lpBuffer->wr_ptr() : 0;
}



bool CSendStream::PutBuffer(const char* szBuffer, const unsigned long dwBufferSize, unsigned char cCMD_In)
{
    // TODO : 패킷 통계 내기.

    char* lpBuffer = GetBuffer(dwBufferSize);
    if(0 != lpBuffer)
    {
        const char* lpBufferPastEnd = szBuffer + dwBufferSize; 
        std::copy(szBuffer, lpBufferPastEnd, lpBuffer);        
        m_lpBuffer->wr_ptr(dwBufferSize);
	
        // 패킷 통계.
        CPacketStatistics::GetInstance().Send(cCMD_In, dwBufferSize);                
        return ForceSend();
    }

    return false;
}


bool CSendStream::WrapHeader(unsigned short usUsed_In, unsigned char cCMD_In, 
                             unsigned short usState_In, unsigned short usError_In)
{
    // TODO : 패킷 통계 내기.

    char* lpBuffer = GetBuffer(usUsed_In);
    if(0 != lpBuffer)
    {
        if (PacketWrap::WrapHeader(lpBuffer, usUsed_In, cCMD_In, usState_In, usError_In))
        {
            m_lpBuffer->wr_ptr(usUsed_In);

            CPacketStatistics::GetInstance().Send(cCMD_In, usUsed_In);
            return ForceSend();
        }
    }

    return false;
}


bool CSendStream::WrapCrypt(unsigned short usUsed_In, unsigned char cCMD_In, 
                            unsigned short usState_In, unsigned short usError_In)
{
    // TODO : 패킷 통계 내기.

    char* lpBuffer = GetBuffer(usUsed_In);
    if(0 != lpBuffer)
    {
        if (PacketWrap::WrapCrypt(lpBuffer, usUsed_In, cCMD_In, usState_In, usError_In))
        {
            m_lpBuffer->wr_ptr(usUsed_In);

            CPacketStatistics::GetInstance().Send(cCMD_In, usUsed_In);
            return ForceSend();
        }
    }

    return false;
}


bool CSendStream::WrapCompress(char* lpSourceData, unsigned short usSourceLength, 
                               unsigned char cCMD_In, unsigned short usState_In, unsigned short usError_In)
{
    // TODO : 패킷 통계 내기.

	unsigned long dwDstBufferSize_InOut = CMiniLZO::GetLeastCompressBuffer(usSourceLength);
    char* lpBuffer = GetBuffer(dwDstBufferSize_InOut);

    if(0 != lpBuffer)
    {	    
        if (PacketWrap::WrapCompress(lpBuffer, dwDstBufferSize_InOut, 
		    lpSourceData, usSourceLength, cCMD_In, usState_In, usError_In))
        {
            m_lpBuffer->wr_ptr(dwDstBufferSize_InOut);

            CPacketStatistics::GetInstance().Send(cCMD_In, dwDstBufferSize_InOut);            
            return ForceSend();
        }
    }

    return false;
}
