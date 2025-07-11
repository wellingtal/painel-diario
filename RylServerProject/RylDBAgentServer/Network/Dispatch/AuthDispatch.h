#ifndef _DBAGENT_SERVER_AUTH_DISPATCH_H_
#define _DBAGENT_SERVER_AUTH_DISPATCH_H_

#include <Network/Dispatch/RylServerDispatch.h>
#include <Network/Dispatch/SingleDispatchStorage.h>

#include <Item/ItemSerialMgr.h>

// forward decl.
struct PktSL;
struct PktUK;
struct PktDD;

namespace DBAgent
{
    class CAuthDispatch : public CRylServerDispatch
    {
    public:

        static CSingleDispatch& GetDispatchTable();

        enum Const
        {
            MAX_PACKET_DISPATCH_PER_PULSE   = 100,
            MAX_STREAM_BUFFER_SIZE          = 16000
        };

        CAuthDispatch(CSession& Session);
        virtual ~CAuthDispatch();

        virtual void Connected();
        virtual void Disconnected();
        virtual bool DispatchPacket(PktBase* lpPktBase);
        
        void IncUserNum() { ++m_nUserNum; }
        void DecUserNum() { --m_nUserNum; }

        int GetUserNum() const { return (m_nUserNum < 1) ? 1 : m_nUserNum; }

    private:

        bool ParseServerLogin(PktSL* lpPktSL);
        bool ParseServerLogout(PktBase* lpPktBase);
        bool ParseUserKill(PktUK* lpPktUK);

        unsigned long               m_dwServerID;
        int                         m_nUserNum;
        Item::CItemSerialMgr        m_AuthItemSerialMgr;
    };
}

#endif