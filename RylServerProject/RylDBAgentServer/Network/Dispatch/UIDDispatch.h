#ifndef _DBAGENT_SERVER_UID_DISPATCH_H_
#define _DBAGENT_SERVER_UID_DISPATCH_H_

#include <Network/Packet/PacketStruct/ServerPacket.h>
#include <Network/Dispatch/RylServerDispatch.h>
#include <Network/Dispatch/SingleDispatchStorage.h>

// forward decl.
struct PktBase;
struct PktUUTAck;
struct PktBTN;
struct PktHanBTN;
struct PktHanUserKill;
struct PktUK;
struct PktSLAck;

namespace DBAgent
{
    extern const char* g_szPktUUTString[PktUUT::MAX_UPDATE_TYPE];

    class CUIDDispatch : public CRylServerDispatch
    {
    public:

        static CSingleDispatch& GetDispatchTable();

        enum Const
        {
            MAX_PACKET_DISPATCH_PER_PULSE   = 100,
            MAX_STREAM_BUFFER_SIZE          = 16000
        };

        CUIDDispatch(CSession& Session);
        virtual ~CUIDDispatch();

        virtual void Connected();
        virtual void Disconnected();
        virtual bool DispatchPacket(PktBase* lpPktBase);
        
    private:

        bool ParseServerLoginAck(PktSLAck* lpPktSLAck);
        bool ParseServerLogoutAck(PktBase* lpPktBase);

        bool ParseUpdateUIDTable(PktUUTAck* lpPktUUTAck);

        bool ParseBillingTimeNotify(PktBTN* lpPktBTN);              // �ѱ� ���� ��� �޽���
        bool ParseBillingTimeCheckNotify(PktBTN* lpPktBTN);         // �븸 ���� ��� �޽���

        bool ParseHanBTNWarning(PktHanBTN* lpPktHanBTN);            // �ѱ� ���պ��� ���� ��� �޽���
        bool ParseHanBTNUserKill(PktHanUserKill* lpPktHanUserKill); // �ѱ� ���պ��� ���� ���� ����

        bool ParseUserKill(PktUK* lpPktUK);                         // ���� ����
        
    };
}

#endif