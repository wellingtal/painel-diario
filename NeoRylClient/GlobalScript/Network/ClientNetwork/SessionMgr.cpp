#include "stdafx.h"
#include "SessionMgr.h"
#include "ClientEventHandler.h"
#include "TCPSession.h"
#include "UDPSession.h"

#include <algorithm>
#include <ctime>

#include <mmsystem.h>
#include "GMMemory.h"

const int MAX_DEFAULT_BLOCK_NUM = 100;
const int DEFAULT_BLOCK_SIZE    = 16384;
 

namespace ClientNet
{

CSessionMgr::CSessionMgr()
:   m_dwMaxSessionID(0), m_lpLockedMsgQueue(0)
{
    memset(m_hHandleSet, 0, sizeof(HANDLE) * MAXIMUM_WAIT_OBJECTS);

    WSADATA wsaData;
    WSAStartup(0x0202, &wsaData);
    
    RearrangeHandleSet();
}

CSessionMgr::~CSessionMgr()
{
    SessionMap::iterator end = m_SessionMap.end();
    SessionMap::iterator pos = m_SessionMap.begin();

    for(;pos != end; ++pos)
    {        
        delete pos->second;
    }

    m_SessionMap.clear();

    WSACleanup();
}

int CSessionMgr::Connect(INET_Addr& address, NetMsgType eNetMsgType, DWORD dwHandlerID)
{
    if (MAXIMUM_WAIT_OBJECTS + 1 <= m_SessionMap.size())
    {
        // ������ �ʹ� �����ϴ�.
        return -1;
    }

    if (0 == m_lpLockedMsgQueue)
    {
        // �޽��� ť�� ���õ��� �ʾҽ��ϴ�.
        return -1;
    }

    DWORD dwSessionID = 0;

    SessionMap::iterator pos;
    SessionMap::iterator end = m_SessionMap.end();
    
    do
    {
        // �� ����ID�� ã�´�.
        dwSessionID = ++m_dwMaxSessionID;
        pos = m_SessionMap.find(dwSessionID);
    } 
    while(pos != end);

    // ����, Ÿ�Կ� ���� UDP������ ����� �� �ְ� �Ѵ�.
    CSession*       lpSession = 0;
#include "GMUndefNew.h"
    CEventHandler*  lpEventHandler = 
        new (std::nothrow) CThreadEventHandler(*m_lpLockedMsgQueue, dwHandlerID);
#include "GMRedefNew.h"

    if(0 == lpEventHandler)
    {
        // �̺�Ʈ �ڵ鷯 ���� ����
        return -1;
    }

    switch(eNetMsgType)
    {
    case NET_MSG_REQ_TCP_OPEN:
#include "GMUndefNew.h"
        lpSession = new (std::nothrow) CTCPSession(dwSessionID, lpEventHandler);
#include "GMRedefNew.h"
        break;

    case NET_MSG_REQ_UDP_OPEN:
#include "GMUndefNew.h"
        lpSession = new (std::nothrow) CUDPSession(dwSessionID, lpEventHandler);
#include "GMRedefNew.h"
        break;
    }

    if(0 == lpSession || -1 == lpSession->Open(address))
    {
        delete lpSession;
        return -1;
    }

    m_SessionMap.insert(SessionMap::value_type(dwSessionID, lpSession));
    return 0;
}

void CSessionMgr::RearrangeHandleSet()
{
    SessionMap::iterator pos = m_SessionMap.begin();
    SessionMap::iterator end = m_SessionMap.end();

    m_hHandleSet[0] = GetQueueHandle();

    for (DWORD dwHandleIndex = 1; pos != end; ++pos, ++dwHandleIndex)
    {
        m_hHandleSet[dwHandleIndex] = pos->second->GetEvent();
    }
}


int CSessionMgr::HandleEvents(DWORD dwMilliseconds)
{
    SessionMap::iterator pos = m_SessionMap.begin();
    SessionMap::iterator end = m_SessionMap.end();
        
    CEventHandler*  lpEventHandler = 0;

    DWORD dwResult = WaitForMultipleObjects(
        m_SessionMap.size() + 1, m_hHandleSet, FALSE, dwMilliseconds);

    if (WAIT_FAILED != dwResult)
    {
        // Ÿ�Ӿƿ� �ɷȴ� �ϴ���, ���и� �ƴϸ� ��� ����� �� ���Ƽ� üũ�� ��.
        unsigned long dwRemoveCount = 0;

        // ť ������ ó���� �Ѵ�.
        if (-1 == HandleMsgQueue())
        {
            ++dwRemoveCount;
        }

        // ��Ŷ ó���� �Ѵ�.
        for (; pos != end;)
        {
            bool bRemove = false;
            CSession* lpSession = pos->second;

            lpEventHandler = lpSession->GetEventHandler();
            if(0 == lpEventHandler)
            {
                fprintf(stderr, "CSessionMgr::HandleEvents - Session event handler is null\n");
                bRemove = true;            
            }
            else
            {
                int rc = WaitForSingleObject(lpSession->GetEvent(), 0);

                if(WAIT_FAILED == rc)
                {
                    fprintf(stderr, "CSessionMgr::HandleEvents - WaitForSingleObject failed : %d\n",
                        WSAGetLastError());            
                }
                else if(WAIT_TIMEOUT != rc && -1 == HandleIO(lpSession))
                {
                    bRemove = true;
                }
            }

            if (-1 == lpEventHandler->OnDispatch(*lpSession))
            {
                bRemove = true;
            }

            DWORD dwCurrentTime = timeGetTime();

            if (lpEventHandler->IsTimeout(dwCurrentTime))
            {
                if(-1 == lpEventHandler->OnTimeout(*lpSession))
                {
                    bRemove = true;
                }

                lpEventHandler->UpdateTimeoutCalled(dwCurrentTime);
            }

            if (bRemove)
            {
                lpEventHandler->OnDispatch(*lpSession);
                lpEventHandler->OnClose(*lpSession);

                lpSession->Send();
                lpSession->Close();

                delete lpSession;

			    pos = m_SessionMap.erase(pos);
                ++dwRemoveCount;
            }
            else
            {
                ++pos;
            }
        }

        if (0 < dwRemoveCount)
        {
            RearrangeHandleSet();
        }
    }

    return 0;
}


int CSessionMgr::HandleIO(CSession* lpSession)
{
    WSANETWORKEVENTS netEvents;
    memset(&netEvents, 0, sizeof(WSANETWORKEVENTS));

    CEventHandler* lpEventHandler = lpSession->GetEventHandler();

    // Enumerate the events
    int rc = WSAEnumNetworkEvents(lpSession->GetSocket(), lpSession->GetEvent(), &netEvents);

    if (SOCKET_ERROR == rc)
    {
        fprintf(stderr, "CSessionMgr::HandleIO - WSAEnumNetworkEvents failed: %d\n", WSAGetLastError());
        return -1;
    }
    
    if (netEvents.lNetworkEvents & FD_CONNECT)
    {
        fprintf(stderr, "CSessionMgr::HandleIO - Connect : %d\n", netEvents.iErrorCode[FD_CONNECT_BIT]);

        lpEventHandler->OnOpen(*lpSession, netEvents.iErrorCode[FD_CONNECT_BIT]);
    }
    
    if (netEvents.lNetworkEvents & FD_READ)
    {
        // Check for read error
        if (netEvents.iErrorCode[FD_READ_BIT] == 0)
        {            
            if (-1 == lpSession->Recv())
            {
                return -1;
            }
        }
        else
        {
            fprintf(stderr, "CSessionMgr::HandleIO - FD_READ error %d\n", 
                netEvents.iErrorCode[FD_READ_BIT]);
            
            return -1;
        }
    }

    if (netEvents.lNetworkEvents & FD_WRITE)
    {
        // Check for write error
        if (netEvents.iErrorCode[FD_WRITE_BIT] == 0)
        {            
            if (-1 == lpSession->Send())
            {
                return -1;
            }
        }
        else
        {
            fprintf(stderr, "CSessionMgr::HandleIO - FD_WRITE error %d\n",
                    netEvents.iErrorCode[FD_WRITE_BIT]);
            return -1;
        }
    }
    if (netEvents.lNetworkEvents & FD_CLOSE)
    {
        if (netEvents.iErrorCode[FD_CLOSE_BIT] == 0)
        {
            while(1)
            {
                // ���� �� ������ ������ �д´�.
                rc = lpSession->Recv();

                if(-1 == rc)
                {
                    lpSession->Close();
                    return -1;
                }
                else if (rc != 0)
                {
                    continue;
                }
                else
                {
                    break;
                }
            }

            // ���� �� ������ �� ������.
            if(-1 == lpSession->Send())
            {
                return -1;
            }            
        }
        else
        {
            fprintf(stderr, "CSessionMgr::HandleIO - FD_CLOSE error %d\n",
                    netEvents.iErrorCode[FD_CLOSE_BIT]);

            return -1;
        }
    }

    return 0;
}

int CSessionMgr::HandleMsgQueue()
{
    DWORD dwHandleSetChange = 0;

    CNetworkMsgQueue netMsgQueue;
	GetMessages(netMsgQueue, 0xFFFFFFFF);

	CNetworkMsgBlock* lpMsgBlock = 0;
    DWORD dwEventHandlerID = 0;
    
    SessionMap::iterator pos;
    
    while (0 != (lpMsgBlock = netMsgQueue.Dequeue()))
    {
        bool bDeleteMsg = true;

        switch(lpMsgBlock->GetMsgType())
        {
        case NET_MSG_REQ_TCP_OPEN:
        case NET_MSG_REQ_UDP_OPEN:
            
            dwEventHandlerID = lpMsgBlock->GetID();

            if(0 == Connect(lpMsgBlock->GetAddress(), 
                lpMsgBlock->GetMsgType(), dwEventHandlerID))
            {
                // ���� ����. �ڵ� set�� ����Ǿ���. (�ݹ��� ���߿� OnOpenȣ��� �߻��Ѵ�)
                ++dwHandleSetChange;
            }
            else
            {
                // ���� �õ����� ���� ���� ����̴�. ������ Callback���� �����Ѵ�.
                int nError = WSAGetLastError();    
                if (0 == nError) { nError = INT_MAX; }

                if(0 != m_lpLockedMsgQueue)
                {
                    CNetworkMsgBlock* lpAckMsgBlock = 
                        CNetworkMsgPool::GetInstance().GetNetworkMsgBlock(sizeof(int), INET_Addr());

                    if(0 != lpAckMsgBlock)
                    {                        
                        lpAckMsgBlock->SetID(dwEventHandlerID);
                        lpAckMsgBlock->SetMsgType(NET_MSG_ACK_OPEN);

                        lpAckMsgBlock->push(nError);
                        m_lpLockedMsgQueue->PutMessage(lpAckMsgBlock);
                    }
                }
            }
            break;

        case NET_MSG_REQ_CLOSE:

            // ������ ã�� Shutdown/Close�� ȣ���Ѵ�. (�ݹ��� ���߿� OnCloseȣ��� �߻��Ѵ�)
            pos = m_SessionMap.find(lpMsgBlock->GetID());
            if (pos != m_SessionMap.end()) 
            { 
                pos->second->Close();            
            }
            break;

        case NET_MSG_REQ_SEND:

            // ������ ã�� ��Ŷ�� ���� ť�� �ִ´�. (�ݹ��� ����.)
            pos = m_SessionMap.find(lpMsgBlock->GetID());
            if (pos != m_SessionMap.end()) 
            { 
                pos->second->SendPending(lpMsgBlock); 
                bDeleteMsg = false;
            }
            break;
        }

        if (bDeleteMsg)
        {
            lpMsgBlock->SetMsgType(NET_MSG_FREE);
            CNetworkMsgPool::GetInstance().FreeNetworkMsgBlock(lpMsgBlock);
        }
    }

    return (0 < dwHandleSetChange) ? -1 : 0;
}


}

