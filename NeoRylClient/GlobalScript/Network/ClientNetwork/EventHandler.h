#ifndef _GAMA_CLIENT_EVENT_HANDLER_H_
#define _GAMA_CLIENT_EVENT_HANDLER_H_

namespace ClientNet
{
    // forward decl.
    class CSession;
    class CNetworkMsgBlock;
    
    class CEventHandler
    {
    public:

        CEventHandler(DWORD dwTimeoutSec = 1) 
			:	m_dwTimeoutSec(dwTimeoutSec), m_dwLastCalled(0) 
		{ 
		
		}

        virtual ~CEventHandler() { }
        
        virtual int OnReceived(CNetworkMsgBlock& msg_block) = 0;        // ��Ŷ�� �޾��� �� �Ҹ���.

        virtual int OnOpen(CSession& ConnSession, int iErrorCode) = 0;  // ������ �Ϸ�Ǿ��� �� �Ҹ���. (����/����. UDP�� �ش� ����)
        virtual int OnClose(CSession& CloseSession) = 0;                // ������ ������ �� �Ҹ���.  (UDP�� �ش� ����)
        virtual int OnDispatch(CSession& ReceivedSession) = 0;          // �޽����� ó���� �� �Ҹ���.

        virtual int OnTimeout(CSession& Session) = 0;                   // ���� �ð��� ������ ȣ��ȴ�. �⺻�� ���ʸ��� �Ҹ���.

        bool IsTimeout(DWORD dwCurrentTime) const 
		{
			return (0 == m_dwLastCalled) || 
				(m_dwLastCalled + (m_dwTimeoutSec * 1000) < dwCurrentTime);
		}

        void UpdateTimeoutCalled(DWORD dwCurrentTime) 
		{
			m_dwLastCalled = dwCurrentTime; 		
			if (0 == m_dwLastCalled) { ++m_dwLastCalled; }
		}

    protected:

        DWORD   m_dwTimeoutSec;
        DWORD   m_dwLastCalled;
    };
};

#endif