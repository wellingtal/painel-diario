#ifndef _DBAGENT_SERVER_ADMIN_TOOL_DISPATCH_H_
#define _DBAGENT_SERVER_ADMIN_TOOL_DISPATCH_H_

#include <Network/Dispatch/RylServerDispatch.h>
#include <Network/Dispatch/MultiDispatchStorage.h>

#include <Item/ItemSerialMgr.h>

// forward decl.
struct PktSL;
struct PktUK;
struct PktItemQtyControl;
struct PktAdminToolGetData;
struct PktAdminToolSetData;
struct PktGuildPosition;

namespace DBAgent
{
    class CAdminToolDispatch : public CRylServerDispatch
    {
    public:

        static CMultiDispatch& GetDispatchTable();

        enum Const
        {
            MAX_PACKET_DISPATCH_PER_PULSE   = 100,
            MAX_STREAM_BUFFER_SIZE          = 16000
        };

        CAdminToolDispatch(CSession& Session);
        virtual ~CAdminToolDispatch();

        virtual void Connected();
        virtual void Disconnected();
        virtual bool DispatchPacket(PktBase* lpPktBase);
        
    private:

        bool ParseServerLogin(PktSL* lpPktSL);
        bool ParseServerLogout(PktBase* lpPktBase);
        bool ParseUserKill(PktUK* lpPktUK);

        bool ParseGetData(PktBase* lpPktBase);
        bool ParseSetData(PktBase* lpPktBase);
        bool ParseItemQtyCheck(PktItemQtyControl* lpPktItemQtyControl);				// �̺�Ʈ ������ ���� ���� ��Ŷ

        bool ParseAdminToolGetData(PktAdminToolGetData* lpPktAdminToolGetData);		// ���� ���(AdminTool)
        bool ParseAdminToolSetData(PktAdminToolSetData* lpPktAdminToolSetData);		// ���� ���(AdminTool)

        bool ParseAdminToolChangeName(PktAdminToolGetData* lpPktAdminToolGetData);  // ����� ĳ���� �̸� �ٲٱ�

		bool ParseGuildPosition(PktGuildPosition* lpPktGuildPosition);				// ��� ������ ����.

        Item::CItemSerialMgr m_AdminItemSerialMgr;

        unsigned long m_dwServerID;
    };

}

#endif