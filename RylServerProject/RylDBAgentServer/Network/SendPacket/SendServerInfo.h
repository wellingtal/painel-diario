#ifndef _DBAGENT_SERVER_SEND_SERVER_INFO_H_
#define _DBAGENT_SERVER_SEND_SERVER_INFO_H_

#include <Network/Packet/PacketStruct/ServerPacket.h>

// forward decl.
class CSendStream;
class CPacketDispatch;

namespace DBAgent
{
namespace DataStorage
{
    // forward delc.
    class CSessionData;
}

namespace SendPacket
{
    bool ServerLogin(CSendStream& SendStream, unsigned long dwServerID);
    bool ServerLoginAck(CSendStream& SendStream, unsigned long dwServerID,
        unsigned __int64 dwItemSerial); 

    bool UpdateChannel(CSendStream& SendStream, unsigned long dwServerID);
    bool UpdateServerVersion(CSendStream& SendStream);

    void UpdateChannelAllServer();

    bool UpdateUIDTable(PktUUT::UpdateType eUpdateType, const char* szAccountName, const char* szPassword, 
        unsigned long dwUID, unsigned long dwCID, unsigned long dwSessionID, 
        unsigned long dwServerID, IN_ADDR peerAddr, unsigned short usError = 0);

    bool UserKill(const DataStorage::CSessionData& SessionData);

    bool BillingNotify(const DataStorage::CSessionData& SessionData,
        unsigned char cRemainMinute, char cBillingType);

    // 대만쪽 빌링 체크 Notify
    bool BillingCheckNotify(const DataStorage::CSessionData& SessionData,
        unsigned char cRemainMinute, char cBillingType);
}

}


#endif