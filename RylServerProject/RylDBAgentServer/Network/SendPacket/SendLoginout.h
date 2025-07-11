#ifndef _DBAGENT_SERVER_SEND_LOGIN_OUT_H_
#define _DBAGENT_SERVER_SEND_LOGIN_OUT_H_

#include <Network/Packet/PacketStruct/CharLoginOutPacketStruct.h>

// forward decl.
class CSendStream;

namespace DBAgent
{
    namespace DataStorage
    {
        // forward delc.
        class CSessionData;
    }

    namespace SendPacket
    {
        // ���� ����
        bool StartSession(CSendStream& SendStream, unsigned long dwRequestKey, 
            unsigned long dwUID, unsigned long dwSessionID, 
            unsigned char cFirstLogin, unsigned short usError);

        // ĳ���� �α���
        bool CharLogin(CSendStream& SendStream, 
            DataStorage::CSessionData& SessionData, unsigned long dwRequestKey);

        // ĳ���� �α׾ƿ�
        bool CharLoginError(CSendStream& SendStream, unsigned long dwRequestKey,
            DBUpdateData::UpdateType eUpdateType,  
            unsigned long dwUID, unsigned long dwCID, unsigned short usError);
    }
}

#endif