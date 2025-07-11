#include "stdafx.h"
#include "RylServerDispatch.h"

#include "../Packet/PacketBase.h"
#include "../Packet/PacketCommand.h"
#include "../XORCrypt/XORCrypt.h"

#include <Log/ServerLog.h>
#include <Stream/Buffer/Buffer.h>
#include <Stream/Buffer/BufferFactory.h>

#include <Network/Session/Session.h>
#include <Network/Stream/SendStream.h>
#include <Network/Packet/PacketStatistics.h>
#include <Utility/Compress/MiniLZO/MiniLZOWrapper.h>

#include <algorithm>

enum RylServerDispatchFlags
{
	PROCESS_PACKET_NOW	=	(1 << 0),
    SUSPEND_RECV        =   (1 << 1)
};


CRylServerDispatch::CRylServerDispatch(CSession& Session, 
                                       unsigned long dwMaxProcessPacketPerPulse)
:   CPacketDispatch(Session), m_SendStream(Session),
    m_dwMaxProcessPacketPerPulse(dwMaxProcessPacketPerPulse),
	m_dwFlags(0)
{
    
}


CRylServerDispatch::~CRylServerDispatch()
{
    BufferQueueLock::Syncronize sync(m_BufferQueueLock);
    m_ProcessQueue.clear();
}


CBufferFactory& CRylServerDispatch::GetBufferFactory()
{
	return m_Session.GetPolicy().GetBufferFactory();
}

INET_Addr& CRylServerDispatch::GetRemoteAddr()
{
    return m_Session.GetRemoteAddr(); 
}

INET_Addr& CRylServerDispatch::GetLocalAddr()
{
    return m_Session.GetLocalAddr();
}

bool CRylServerDispatch::Shutdown()
{
    return m_Session.Shutdown();
}

void CRylServerDispatch::CloseSession()
{
    m_Session.CloseSession();
}

void CRylServerDispatch::LogErrorPacket(const char* szDetailText,
                                        const unsigned char cCmd)
{
    ERRLOG5(g_SessionLog, "SP:0x%p/DP:0x%p/IP:%15s/PktCmd:0x%02x/%s",
        &m_Session, this, m_Session.GetRemoteAddr().get_addr_string(), cCmd, szDetailText);
}


CRylServerDispatch::CreationResult CRylServerDispatch::CreatePacket(
    CBufferFactory& bufferFactory, CBufferQueue& bufferQueue, 
    char* const lpStream_In, unsigned long* dwStreamSize_InOut)
{
    CXORCrypt& Crypt = CXORCrypt::GetInstance();
    
    CBuffer*        lpBuffer            = 0;
    PktBase*	    lpPktBase	        = 0;
    char*		    lpBufferPos         = lpStream_In;
    
    unsigned long	dwStreamSize        = *dwStreamSize_InOut;
    unsigned long   dwDecompressedSize  = 0;
    unsigned long   dwPacketNum         = 0;
   
    while(sizeof(PktBase) <= dwStreamSize)
    {
	    lpPktBase = reinterpret_cast<PktBase*>(lpBufferPos);

	    // ��Ŷ ��� ���ڵ�
	    Crypt.DecodeHeader(lpBufferPos + 1, sizeof(PktBase) - 1, 0, 0);

	    // ��� ���� ��� ����.
	    const PktBase::CMDType		nPacketCMD		= lpPktBase->GetCmd();
	    const PktBase::LengthType	nPacketLength	= lpPktBase->GetLen();

	    // ��Ŷ ����� vaild���� Ȯ���Ѵ�. invalid�� ���� �׳� ���������.
	    if(StartBit != lpPktBase->GetStartBit())
	    {
            return E_INVALID_STARTBIT;
	    }

	    if(dwStreamSize < nPacketLength)
        {
		    // ��ü ��Ʈ�� ����� �Ľ��� �ϱ⿡�� ���ڶ�. �ٽ� ���ڵ� �� �� ���� ���� ��ٸ�
		    Crypt.EncodeHeader(lpBufferPos + 1, sizeof(PktBase) - 1, 0, 0);
            break;
        }
        else
        {
		    // ��Ŷ ���ڵ�.
		    if(lpPktBase->IsCrypt())
		    {
			    Crypt.DecodePacket(lpBufferPos + sizeof(PktBase), 
				    nPacketLength - sizeof(PktBase), lpPktBase->GetCodePage());
		    }

		    // ��Ŷ ���� ����.
		    if(lpPktBase->IsCompress())
		    {
			    dwDecompressedSize = PktMaxLen - sizeof(PktBase);

                lpBuffer = CREATE_BUFFER(bufferFactory, PktMaxLen);
                if(0 == lpBuffer)
                {
                    return E_ALLOCATE_BUFFER_FAILED;
                }

                char* szDecompressedPacket = lpBuffer->wr_ptr();
			    memcpy(szDecompressedPacket, lpPktBase, sizeof(PktBase));

			    // ���� ����.
			    if(!CMiniLZO::Decompress(
					lpBufferPos + sizeof(PktBase),          // src Pos
                    nPacketLength - sizeof(PktBase),        // src Size
                    szDecompressedPacket + sizeof(PktBase), // dst Pos
                    &dwDecompressedSize))                   // dst Size
			    {                    				    
                    SAFE_RELEASE_BUFFER(lpBuffer);
				    return E_DECOMPRESS_FAILED;
			    }

                lpBuffer->wr_ptr(dwDecompressedSize + sizeof(PktBase));
		    
			    lpPktBase = reinterpret_cast<PktBase*>(szDecompressedPacket);
			    lpPktBase->SetLen(static_cast<PktBase::LengthType>(dwDecompressedSize + sizeof(PktBase)));
		    }
            else
            {
                lpBuffer = CREATE_BUFFER(bufferFactory, nPacketLength);

                if(NULL == lpBuffer)
                {
                    return E_ALLOCATE_BUFFER_FAILED;
                }

                lpBuffer->push(lpBufferPos, nPacketLength);
            }

            // ��Ŷ�� �迭�� ����.
            bufferQueue.enqueue(lpBuffer);

            dwStreamSize -= nPacketLength;
            lpBufferPos += nPacketLength;
	    }
    }

    *dwStreamSize_InOut -= dwStreamSize;
    return S_CREATE_SUCCESS;    
}


void CRylServerDispatch::LogErrorPacketCreation(CRylServerDispatch::CreationResult eResult)
{
    const char* lpErrString = 0;

    switch(eResult)
    {        
    case E_INVALID_STARTBIT:        lpErrString = "Invalid packet startbit";        break;
    case E_ALLOCATE_BUFFER_FAILED:  lpErrString = "Allocate packetbuffer failed";   break;
    case E_DECOMPRESS_FAILED:       lpErrString = "Decompress packet failed";       break;
    default:                        lpErrString = "Unknown error occured";          break;
    }

    if(0 != lpErrString)
    {
        LogErrorPacket(lpErrString, 0);        
    }
}


void CRylServerDispatch::InternalCheckSuspendRecv(CBufferQueue& bufferQueue)
{
    const unsigned long MAX_PACKET = m_dwMaxProcessPacketPerPulse * 5;
    const unsigned long CUR_PACKET = bufferQueue.getBufferNum();

    if (!(m_dwFlags & SUSPEND_RECV) && MAX_PACKET < CUR_PACKET)
    {
        // Recv������ �� �ɷ� �ְ�, ��Ŷ ���� �ʹ� ������ Recv�� �����.
        GetSession().SuspendRecv();
        m_dwFlags |= SUSPEND_RECV;
    }
    else if ((m_dwFlags & SUSPEND_RECV) && CUR_PACKET < MAX_PACKET)
    {
        // Recv������ �ɷ� �ְ�, ��Ŷ ���� �ٽ� ���� ȸ���Ǿ� ������ Recv�� �ٽ� �Ǵ�.
        GetSession().ResumeRecv();
        m_dwFlags &= ~ SUSPEND_RECV;        
    }
}

bool CRylServerDispatch::ParsePacket(char *const lpStream_In, unsigned long* dwStreamSize_InOut)
{
    CBufferFactory& bufferFactory = m_Session.GetPolicy().GetBufferFactory();
    CBufferQueue    bufferQueue;

    CreationResult eCreationResult = CreatePacket(
        bufferFactory, bufferQueue, lpStream_In, dwStreamSize_InOut);

    if(S_CREATE_SUCCESS != eCreationResult)
    {
        LogErrorPacketCreation(eCreationResult);
        return false;
    }

    BufferQueueLock::Syncronize sync(m_BufferQueueLock);
    m_ProcessQueue.splice(bufferQueue);
    InternalCheckSuspendRecv(m_ProcessQueue);
    return true;
}

bool CRylServerDispatch::Dispatch()
{
    CBufferQueue    ProcessQueue;
    CBufferQueue    Processed;
    PktBase*        lpPktBase = 0;
     
    {
		BufferQueueLock::Syncronize sync(m_BufferQueueLock);
		
		if((m_dwFlags & PROCESS_PACKET_NOW))
		{
			// �̹� ��Ŷ�� ó�����ε� �����Ͽ���.
			return true;
		}

		// ���� ��Ŷ�� ó���Ѵ�.
		m_dwFlags |= PROCESS_PACKET_NOW;

        InternalCheckSuspendRecv(m_ProcessQueue);
		ProcessQueue.splice(m_ProcessQueue);
    }
    
    for(unsigned long dwCount = 0; dwCount < GetMaxProcessPacketPerPulse(); ++dwCount)
    {        
		CBuffer* lpBuffer = ProcessQueue.dequeue();
		if(0 == lpBuffer)
		{
			break;
		}
		else 
		{
            lpPktBase = reinterpret_cast<PktBase*>(lpBuffer->rd_ptr());

            // ��Ŷ ���.
            CPacketStatistics::GetInstance().Recv(lpPktBase->GetCmd(), lpPktBase->GetLen());
            
            // ��Ŷ ó��.
			bool bResult = DispatchPacket(lpPktBase);
            Processed.enqueue(lpBuffer);
            
			if(!bResult)
			{
				return false;
			}
		}
    }
	
    if (!ProcessQueue.empty())
    {
        ProcessTooManyPacket(ProcessQueue);
    }
	
	{
		BufferQueueLock::Syncronize sync(m_BufferQueueLock);

		if(!ProcessQueue.empty())
		{
			BufferQueueLock::Syncronize sync(m_BufferQueueLock);
			m_ProcessQueue.splice(ProcessQueue, true);
		}

		m_dwFlags &= ~PROCESS_PACKET_NOW;
	}

    return true;
}


template<typename T>
bool greator_second(const T& lhspair, const T& rhspair)
{
    return rhspair.second < lhspair.second;
}


void CRylServerDispatch::ProcessTooManyPacket(CBufferQueue& bufferQueue)
{
    typedef std::pair<unsigned long, unsigned long> PacketInfoPair;
    typedef std::vector<PacketInfoPair>  PacketStatistics;

    const int MAX_BUFFER = 3072;
    char      szBuffer[MAX_BUFFER];
            
    int nTotalLength = _snprintf(szBuffer, MAX_BUFFER, "Processed:%d/Remain:%d/Remain packets(cmd:num)/",
        m_dwMaxProcessPacketPerPulse, bufferQueue.getBufferNum());
    
    PacketStatistics statistics;
    PacketStatistics::iterator spos;
    PacketStatistics::iterator send;

    statistics.reserve(bufferQueue.getBufferNum());

    for(CBuffer* lpBuffer = bufferQueue.getHead();
        0 != lpBuffer; lpBuffer = lpBuffer->next())
    {
        PktBase::CMDType cmd = reinterpret_cast<PktBase*>(lpBuffer->rd_ptr())->GetCmd();
        
        spos = statistics.begin();
        send = statistics.end();

        for(;spos != send; ++spos)
        {
            if(spos->first == cmd)
            {
                ++spos->second;				
                break;
            }
        }

        if(spos == send)
        {
            statistics.push_back(PacketInfoPair(cmd, 1));
        }
    }

    std::sort(statistics.begin(), statistics.end(), &greator_second<PacketInfoPair>);

    spos = statistics.begin();
    send = statistics.end();

    for(; spos != send; ++spos)
    {
        int nLength = _snprintf(szBuffer + nTotalLength, MAX_BUFFER - nTotalLength, 
            "0x%02x:%5d/", spos->first, spos->second);

        if(0 < nLength)
        {
            nTotalLength += nLength;
        }
    }

    LogErrorPacket(szBuffer, 0);
}


CSendPacketAllServer::CSendPacketAllServer(const char* szData, unsigned long dwDataLen, 
                                           unsigned char cPacketCmd)
                                           :    m_szData(szData), 
                                                m_dwDataLen(dwDataLen), 
                                                m_cPacketCmd(cPacketCmd)
{

}

bool CSendPacketAllServer::operator () (unsigned long dwServerID, CPacketDispatch& packetDispatch)
{    
    return static_cast<CRylServerDispatch&>(packetDispatch).GetSendStream().PutBuffer(
        m_szData, m_dwDataLen, m_cPacketCmd);
}