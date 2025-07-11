#ifndef _DBAGENT_SERVER_CHAT_DISPATCH_H_
#define _DBAGENT_SERVER_CHAT_DISPATCH_H_

#include <Network/Dispatch/RylServerDispatch.h>
#include <Network/Dispatch/SingleDispatchStorage.h>

// forward decl.
struct PktSL;

namespace DBAgent
{
    class CChatDispatch : public CRylServerDispatch
    {
    public:

        static CSingleDispatch& GetDispatchTable();

        enum Const
        {
            MAX_PACKET_DISPATCH_PER_PULSE   = 100,
            MAX_STREAM_BUFFER_SIZE          = 16000
        };

        CChatDispatch(CSession& Session);
        virtual ~CChatDispatch();

        virtual void Connected();
        virtual void Disconnected();
        virtual bool DispatchPacket(PktBase* lpPktBase);
        
    private:

        bool ParseServerLogin(PktSL* lpPktSL);
        bool ParseServerLogout(PktBase* lpPktBase);

        unsigned long  m_dwServerID;
    };


}

#endif