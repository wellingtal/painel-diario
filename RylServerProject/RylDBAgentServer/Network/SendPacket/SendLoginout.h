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
        // 세션 시작
        bool StartSession(CSendStream& SendStream, unsigned long dwRequestKey, 
            unsigned long dwUID, unsigned long dwSessionID, 
            unsigned char cFirstLogin, unsigned short usError);

        // 캐릭터 로그인
        bool CharLogin(CSendStream& SendStream, 
            DataStorage::CSessionData& SessionData, unsigned long dwRequestKey);

        // 캐릭터 로그아웃
        bool CharLoginError(CSendStream& SendStream, unsigned long dwRequestKey,
            DBUpdateData::UpdateType eUpdateType,  
            unsigned long dwUID, unsigned long dwCID, unsigned short usError);
    }
}

#endif