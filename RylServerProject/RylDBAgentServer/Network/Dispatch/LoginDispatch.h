#ifndef _DBAGENT_SERVER_LOGIN_DISPATCH_H_
#define _DBAGENT_SERVER_LOGIN_DISPATCH_H_

#include <Network/Dispatch/RylServerDispatch.h>
#include <Network/Dispatch/SingleDispatchStorage.h>

namespace DBAgent
{
    class CLoginDispatch : public CRylServerDispatch
    {
    public:

        static CSingleDispatch& GetDispatchTable();

        enum Const
        {
            MAX_PACKET_DISPATCH_PER_PULSE   = 10,
            MAX_STREAM_BUFFER_SIZE          = 16000
        };

        CLoginDispatch(CSession& Session);
        virtual ~CLoginDispatch();

        virtual void Connected();
        virtual void Disconnected();
        virtual bool DispatchPacket(PktBase* lpPktBase);
        
    private:

        
    };
}

#endif