#ifndef _GAMA_TEST_GAME_HANDLER_H_
#define _GAMA_TEST_GAME_HANDLER_H_

#include <vector>
#include <Network/ClientNetwork/ClientEventHandler.h>

// forward decl.
namespace ClientNet
{
    class CSession;
    class CSessionMgr;
}

#define BIT(value) (1 << value)
#define GAME_EVENT_FUNC(funcName) bool funcName(ClientNet::CClientEventHandler* lpHandler, PktBase* lpPktBase)

// 게임서버 이벤트 핸들러
class CGameEventHandler : public ClientNet::CClientEventHandler
{
public:

    typedef bool (*EventFunc)(ClientNet::CClientEventHandler* lpHandler, PktBase* lpPktBase);

    // Factory Function
    static CGameEventHandler* Create();
    virtual ~CGameEventHandler();

    // 이벤트 추가 및 삭제.
    bool AddHandler(unsigned char cCmd, EventFunc fnEvent, unsigned long dwIgnoreStatus = 0);
    bool RemoveHandler(unsigned char cCmd);
    void ClearHandler();

protected:

    // Create함수로 생성할 것.
    CGameEventHandler();

    inline void COPY_AND_ADVANCE_SRC(void* dst, const char*& src, const size_t size)
    { memcpy(dst, src, size); src += size; }

    inline void COPY_AND_ADVANCE_DST(char*& dst, const void* src, const size_t size)
    { memcpy(dst, src, size); dst += size; }

    virtual int OnDispatch(PktBase* lpPktBase, INET_Addr& peerAddr);
    virtual int OnOpen(int iErrorCode);
    virtual int OnClose();
    
    struct EventFunctions
    {
        unsigned long   m_dwCommand;
        EventFunc       m_fnEvent;
        unsigned long   m_dwIgnoreStatus;

        EventFunctions() : m_dwCommand(0), m_fnEvent(0), m_dwIgnoreStatus(0) { }
        EventFunctions(unsigned long dwCommand, EventFunc fnEvent, unsigned long dwIgnoreStatus)
            :   m_dwCommand(dwCommand), m_fnEvent(fnEvent), m_dwIgnoreStatus(dwIgnoreStatus)
        {
        
        }

        friend bool operator < (const EventFunctions& lhs, const EventFunctions& rhs) { return lhs.m_dwCommand < rhs.m_dwCommand; }
        friend bool operator < (const EventFunctions& lhs, const unsigned long rhs) { return lhs.m_dwCommand < rhs; }
        friend bool operator < (const unsigned long lhs, const EventFunctions& rhs) { return lhs < rhs.m_dwCommand; }
    };

    typedef std::vector<EventFunctions> EventSet;

    EventSet    m_EventSet;
};


#endif