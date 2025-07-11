#include "stdafx.h"
#include "ClientEventHandler.h"

#include <Network/Packet/PacketBase.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/ServerPacket.h>
#include <Network/XORCrypt/XORCrypt.h>
#include <Network/ClientNetwork/Session.h>
#include <Utility/Compress/MiniLZO/miniLZOWrapper.h>
#include <Thread/ThreadMgr.h>

#include <mmsystem.h>
#include "GMMemory.h"

namespace ClientNet
{

CClientEventHandler::CClientEventHandler()
:   m_lpEventHandlerMgr(0),
    m_dwHandlerID(0), m_dwSessionID(0), 
    m_bCloseCalled(false)
{

}

CClientEventHandler::~CClientEventHandler()
{

}

bool CClientEventHandler::SendPacket(CNetworkMsgBlock* lpMsgBlock)
{
    return (0 != m_lpEventHandlerMgr) ? 
        m_lpEventHandlerMgr->SendPacket(this, lpMsgBlock) : false;
}

CClientEventHandlerMgr::CClientEventHandlerMgr()
:   m_dwMaxEventHandlerID(0), m_bExit(FALSE)
{
    m_SessionMgr.SetDispatchQueue(m_LockedMsgQueue);

    CThreadMgr::Run(this);
}

CClientEventHandlerMgr::~CClientEventHandlerMgr()
{
    // �������� ��� Handler�� ���ؼ�, Close�� ȣ���Ѵ�.
    ClientEventHanderMap::iterator pos = m_ClientEventHandlerMap.begin();
    ClientEventHanderMap::iterator end = m_ClientEventHandlerMap.end();

    for(; pos != end; ++pos)
    {
        Close(pos->second);
    }
    
    // 2�ʵ��� ���ؼ��� ���� ����⸦ ��ٸ���.
    DWORD dwLastTime = timeGetTime();

    do
    {        
        HandleEvents(0xFFFFFFFF);
        Sleep(100);
    }
    while(timeGetTime() - dwLastTime < 5000 && !m_ClientEventHandlerMap.empty());

    // ������ ����.
    End();
    CThreadMgr::Stop(this, 2000);

    // ������ ���� ����
    pos = m_ClientEventHandlerMap.begin();
    end = m_ClientEventHandlerMap.end();

    for(; pos != end; ++pos)
    {
        delete pos->second;
    }    
}


void CClientEventHandlerMgr::HandleEvents(unsigned long dwProcessPerCall)
{
    m_LockedMsgQueue.GetMessages(m_LocalMsgQueue, ULONG_MAX);

    // ���� �޽����� ������ ó���Ѵ�.

    CNetworkMsgBlock* lpMsgBlock = 0;

    ClientEventHanderMap::iterator pos;
    ClientEventHanderMap::iterator end = m_ClientEventHandlerMap.end();

    for (DWORD dwCount = 0; dwCount < dwProcessPerCall; ++dwCount)
    {
        lpMsgBlock = m_LocalMsgQueue.Dequeue();

        if(0 == lpMsgBlock)
        {
            break;
        }

        pos = m_ClientEventHandlerMap.find(lpMsgBlock->GetID());

        if(pos != end)
        {
            // �̹� ������ Handler�� ���� �����̴�.
            CClientEventHandler* lpHandler = pos->second;

            switch(lpMsgBlock->GetMsgType())
            {
            case NET_MSG_ACK_DISPATCH:

                if (-1 == lpHandler->OnDispatch(
                    reinterpret_cast<PktBase*>(lpMsgBlock->rd_ptr()), lpMsgBlock->GetAddress()))
                {
                    Close(lpHandler);
                }
                break;

            case NET_MSG_ACK_OPEN:

                {
                    // �ɵ� ����ÿ� Handler�� �̹� �����Ѵ�.
                    int     nErrorCode  = 0;
                    DWORD   dwSessionID = 0;

                    lpMsgBlock->pop(nErrorCode);
                    lpMsgBlock->pop(dwSessionID);

                    lpHandler->SetSessionID(dwSessionID);
                    if(-1 == lpHandler->OnOpen(nErrorCode))
                    {
                        // ���� ���� ����. ���� �ݱ⸦ ȣ���Ѵ�.                        
                        if(0 == nErrorCode)
                        {
                            Close(lpHandler);
                        }
                    }

                    if (0 != nErrorCode)
                    {
                        lpHandler->OnClose();
                    }
                }
                break;

            case NET_MSG_ACK_CLOSE:

                lpHandler->OnClose();

                // �ڵ鷯�� �����մϴ�.
                delete lpHandler;
                
                m_ClientEventHandlerMap.erase(pos);
                end = m_ClientEventHandlerMap.end();
                break;
            }
        }
        else
        {
            // TODO : Accept�� ���� ����ÿ���, Handler�� �������� �ʴ´�. ���� ó���� �̰����� ���ְ�, 
            // �޽����� �ѹ� �� ������� �Ѵ�.(CThreadEventHandler�� EventID�� ������ �� �ֵ���..)


        }

        lpMsgBlock->SetMsgType(NET_MSG_FREE);
        CNetworkMsgPool::GetInstance().FreeNetworkMsgBlock(lpMsgBlock);
    }
}

bool CClientEventHandlerMgr::Connect(INET_Addr& connAddr, CClientEventHandler* lpEventHandler)
{
    if (0 == lpEventHandler)
    {
        fprintf(stderr, "CClientEventHandlerMgr::Connect - EventHandler is NULL\n");
        return false;
    }

    CNetworkMsgBlock* lpMsgBlock = 
        CNetworkMsgPool::GetInstance().GetNetworkMsgBlock(1, connAddr);

    if(0 == lpMsgBlock || !lpMsgBlock->is_valid())
    {
        fprintf(stderr, "CClientEventHandlerMgr::Connect - NetMsgBlock Create Failed\n");
        return false;
    }

    DWORD dwHandlerID = 0;

    ClientEventHanderMap::iterator pos;
    ClientEventHanderMap::iterator end = m_ClientEventHandlerMap.end();
    
    do
    {
        // �� ����ID�� ã�´�.
        dwHandlerID = ++m_dwMaxEventHandlerID;
        pos = m_ClientEventHandlerMap.find(dwHandlerID);
    } 
    while(pos != end);

    m_ClientEventHandlerMap.insert(
        ClientEventHanderMap::value_type(dwHandlerID, lpEventHandler));

    lpEventHandler->SetHandlerID(dwHandlerID);
    lpEventHandler->SetPeerAddress(connAddr);
    lpEventHandler->SetEventHandlerMgr(this);

    lpMsgBlock->SetAddress(connAddr);
    lpMsgBlock->SetMsgType(NET_MSG_REQ_TCP_OPEN);
    lpMsgBlock->SetID(dwHandlerID);
    m_SessionMgr.PutMessage(lpMsgBlock);
    return true;
}

bool CClientEventHandlerMgr::Close(CClientEventHandler* lpEventHandler)
{
    CNetworkMsgBlock* lpMsgBlock = 
        CNetworkMsgPool::GetInstance().GetNetworkMsgBlock(1, INET_Addr());

    if(0 == lpMsgBlock || !lpMsgBlock->is_valid())
    {
        fprintf(stderr, "CClientEventHandlerMgr::Close - NetMsgBlock Create Failed\n");
        return false;
    }

    lpEventHandler->SetCloseCalled();

    lpMsgBlock->SetMsgType(NET_MSG_REQ_CLOSE);
    lpMsgBlock->SetID(lpEventHandler->GetSessionID());
    m_SessionMgr.PutMessage(lpMsgBlock);
    return true;
}

bool CClientEventHandlerMgr::SendPacket(CClientEventHandler* lpEventHandler, CNetworkMsgBlock* lpMsgBlock)
{
    if (0 != lpEventHandler && 0 != lpMsgBlock)
    {
        lpMsgBlock->SetMsgType(NET_MSG_REQ_SEND);
        lpMsgBlock->SetID(lpEventHandler->GetSessionID());

        m_SessionMgr.PutMessage(lpMsgBlock);
        return true;
    }

    return false;
}


unsigned int CClientEventHandlerMgr::Run()
{
    while(!InterlockedCompareExchange(&m_bExit, TRUE, TRUE))
    {
        m_SessionMgr.HandleEvents(1000);
    }

    return true;
}

BOOL CClientEventHandlerMgr::End()
{
    InterlockedExchange(&m_bExit, TRUE);
    return TRUE;
}


CThreadEventHandler::CThreadEventHandler(CLockedMsgQueue& LockedMsgQueue, DWORD dwClientHandlerID)
:   m_LockedMsgQueue(LockedMsgQueue),
    m_Crypt(CXORCrypt::GetInstance()), 
    m_dwClientHandlerID(dwClientHandlerID),
    CEventHandler(60)   // 1�и��� ȣ���Ѵ�.
{

}

// ��Ŷ�� �޾��� �� �Ҹ���. ��ü������ �޽��� ť���� �����Ѵ�.
int CThreadEventHandler::OnReceived(CNetworkMsgBlock& msg_block)
{
    const int MAX_PACKET_PER_RECV = 32;

    char*       rd_ptr          = 0;
    PktBase*	lpPktBase       = 0;
    DWORD		length          = static_cast<unsigned long>(msg_block.length());
    DWORD       decompress_len  = 0;

    CNetworkMsgBlock* lpNewMsgBlock = 0;
    CNetworkMsgPool& netMessagePool = CNetworkMsgPool::GetInstance();

    CNetworkMsgQueue netMsgQueue;

    while(sizeof(PktBase) <= length)
    {
        rd_ptr    = msg_block.rd_ptr();
        lpPktBase = reinterpret_cast<PktBase*>(rd_ptr);

        // ��Ŷ ����� vaild���� Ȯ���Ѵ�. invalid�� ���� �׳� ���������.
        if(StartBit != lpPktBase->GetStartBit())
        {
            fprintf(stderr, "CClientEventHandler::OnReceived - Packet header is invalid. Not equal startbit\n");
            return -1;
        }

        // ��Ŷ ��� ���ڵ�
        m_Crypt.DecodeHeader(rd_ptr + 1, sizeof(PktBase) - 1, 0, 0);

        // ��� ���� ��� ����.
        const PktBase::CMDType		nPacketCMD		= lpPktBase->GetCmd();
        const PktBase::LengthType	nPacketLength	= lpPktBase->GetLen();

        if(length < nPacketLength)
        {
            // ��ü ��Ʈ�� ����� �Ľ��� �ϱ⿡�� ���ڶ�. �ٽ� ���ڵ� �� �� ���� ���� ��ٸ�
            m_Crypt.EncodeHeader(rd_ptr + 1, sizeof(PktBase) - 1, 0, 0);
            break;
        }
        else
        {
            // ��Ŷ ���ڵ�.
            if(lpPktBase->IsCrypt())
            {
                m_Crypt.DecodePacket(rd_ptr + sizeof(PktBase), 
                    nPacketLength - sizeof(PktBase), lpPktBase->GetCodePage());
            }

            // ��Ŷ ���� ����.
            if(lpPktBase->IsCompress())
            {
                decompress_len = PktMaxLen - sizeof(PktBase);

                lpNewMsgBlock = netMessagePool.GetNetworkMsgBlock(PktMaxLen, msg_block.GetAddress());
                if(0 == lpNewMsgBlock)
                {
                    fprintf(stderr, "CClientEventHandler::OnReceived - Create NetworkMsgBlock Failed for Decompress\n");
                    return -1;
                }

                char* szDecompressedPacket = lpNewMsgBlock->wr_ptr();
                memcpy(szDecompressedPacket, lpPktBase, sizeof(PktBase));

                // ���� ���ۿ��� ���� ����.
                if(!CMiniLZO::Decompress(rd_ptr + sizeof(PktBase), nPacketLength - sizeof(PktBase), 
                    szDecompressedPacket + sizeof(PktBase), &decompress_len))
                {
                    netMessagePool.FreeNetworkMsgBlock(lpNewMsgBlock);
                    fprintf(stderr, "CClientEventHandler::OnReceived - Decompress Packet Failed\n");
                    return -1;
                }

                lpNewMsgBlock->wr_ptr(decompress_len + sizeof(PktBase));
                lpPktBase = reinterpret_cast<PktBase*>(lpNewMsgBlock->rd_ptr());

                lpPktBase->SetLen(static_cast<PktBase::LengthType>(decompress_len + sizeof(PktBase)));
            }
            else
            {
                lpNewMsgBlock = netMessagePool.GetNetworkMsgBlock(nPacketLength, msg_block.GetAddress());
                if(0 == lpNewMsgBlock)
                {
                    fprintf(stderr, "CClientEventHandler::OnReceived - Create NetworkMsgBlock Failed for Enqueue\n");
                    return -1;
                }

                memcpy(lpNewMsgBlock->wr_ptr(), rd_ptr, nPacketLength);
                lpNewMsgBlock->wr_ptr(nPacketLength);
            }

            lpNewMsgBlock->SetMsgType(NET_MSG_ACK_DISPATCH);
            lpNewMsgBlock->SetID(m_dwClientHandlerID);
            netMsgQueue.Enqueue(lpNewMsgBlock);

            length -= nPacketLength;
            msg_block.rd_ptr(nPacketLength);
        }
    }

    m_LockedMsgQueue.PutMessages(netMsgQueue);
    return 0;
}

int CThreadEventHandler::OnOpen(CSession& ConnSession, int iErrorCode)
{
    CNetworkMsgBlock* lpNewMsgBlock = 
        CNetworkMsgPool::GetInstance().GetNetworkMsgBlock(
        sizeof(int) + sizeof(unsigned long), INET_Addr());

    if(0 == lpNewMsgBlock)
    {
        fprintf(stderr, "CThreadEventHandler::OnOpen - NetMsgBlock Create Failed\n");
        return -1;
    }

    lpNewMsgBlock->SetMsgType(NET_MSG_ACK_OPEN);
    lpNewMsgBlock->SetID(m_dwClientHandlerID);
    lpNewMsgBlock->push(iErrorCode);
    lpNewMsgBlock->push(ConnSession.GetSessionID());
    
    m_LockedMsgQueue.PutMessage(lpNewMsgBlock);
    return 0;
}

int CThreadEventHandler::OnClose(CSession& CloseSession)
{
    CNetworkMsgBlock* lpNewMsgBlock = 
        CNetworkMsgPool::GetInstance().GetNetworkMsgBlock(1, INET_Addr());

    if(0 == lpNewMsgBlock)
    {
        fprintf(stderr, "CThreadEventHandler::OnClose - NetMsgBlock Create Failed\n");
        return -1;
    }

    lpNewMsgBlock->SetMsgType(NET_MSG_ACK_CLOSE);
    lpNewMsgBlock->SetID(m_dwClientHandlerID);
    m_LockedMsgQueue.PutMessage(lpNewMsgBlock);
    return 0;
}

int CThreadEventHandler::OnTimeout(CSession& Session)
{
    // Ping�� ������.
    ClientNet::CNetworkMsgBlock* lpMsgBlock = 
        CNetworkMsgPool::GetInstance().GetNetworkMsgBlock(sizeof(PktSyP), INET_Addr());

    if(0 != lpMsgBlock)
    {
        PktSyP* lpSyPAckPt = reinterpret_cast<PktSyP*>(lpMsgBlock->wr_ptr());
        lpSyPAckPt->m_dwTickTime = timeGetTime();

        if (lpMsgBlock->WrapCrypt(sizeof(PktSyP), CmdSysPing, 0))
        {
            Session.SendPending(lpMsgBlock);
        }
		else
		{
			CNetworkMsgPool::GetInstance().FreeNetworkMsgBlock(lpMsgBlock);
		}
    }

    return 0;
}



}