#ifndef _GAMA_CLIENT_SESSION_MGR_H_
#define _GAMA_CLIENT_SESSION_MGR_H_

#include "NetworkMsgBlock.h"
#include <map>

#pragma comment(lib, "ws2_32")

namespace ClientNet
{
    class CSession;

    class CSessionMgr : public CUnboundedWaitableMsgQueue
    {
    public:

        CSessionMgr();
        ~CSessionMgr();

        void SetDispatchQueue(CLockedMsgQueue& LockedMsgQueue) { m_lpLockedMsgQueue = &LockedMsgQueue; }
        CLockedMsgQueue* GetDispatchQueue() { return m_lpLockedMsgQueue; }

        int HandleEvents(DWORD dwMilliseconds = INFINITE);

    private:

        void RearrangeHandleSet();

        // �ش� ���ǿ� ������ ������ -1, ������ 0�� �����Ѵ�.
        int Connect(INET_Addr& address, NetMsgType eNetMsgType, DWORD dwHandlerID);
        int HandleIO(CSession* lpSession);

        // �޽��� ť�� ��Ŷ�� ó���Ѵ�. HandleSet�� ��ȭ�� ������ -1�� �����Ѵ�.
        int HandleMsgQueue();

        typedef std::map<DWORD, CSession*>  SessionMap;

        DWORD               m_dwMaxSessionID;
        DWORD               m_dwLastTimeoutCall;
        CLockedMsgQueue*    m_lpLockedMsgQueue;

        SessionMap          m_SessionMap;
        HANDLE              m_hHandleSet[MAXIMUM_WAIT_OBJECTS];
    };
}


#endif