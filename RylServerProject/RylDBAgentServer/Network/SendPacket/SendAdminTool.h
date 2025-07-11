#ifndef _DBAGENT_SERVER_SEND_ADMIN_TOOL_H_
#define _DBAGENT_SERVER_SEND_ADMIN_TOOL_H_

#include <ctime>
#include <Network/Packet/PacketStruct/CharAdminPacket.h>

// forward decl.
class CSendStream;


namespace DBAgent
{

namespace DataStorage
{
    // forward decl.
    class CStoreData;
    class CCharacterData;
}

namespace SendPacket
{

    bool ItemQtyCheck(unsigned long dwItemTypeID, unsigned long dwItemQty, 
        unsigned long dwCurrentItemQty, time_t tStartTime, time_t tEndTime, 
        unsigned char cType, unsigned short usError = 0);

    bool AdminToolGetDataAck(CSendStream& SendStream, PktAdminToolGetData::GetDataType dataType, 
        unsigned long dwRequestKey, unsigned long dwUID, unsigned long dwCID,
        const char* lpData, unsigned long dwDataSize, unsigned short usError,
		unsigned char cOldServerGroupID = 0);

    bool AdminToolSetDataAck(CSendStream& SendStream, unsigned long dwRequestKey, 
        unsigned long dwUID, unsigned long dwCID, unsigned char cType, unsigned short eErrorCode);

    bool TotalDataToAdminTool(CSendStream& SendStream, unsigned long dwRequestKey, IN_ADDR peerAddr,
        DataStorage::CStoreData& storeData, DataStorage::CCharacterData& characterData);
    
}
}

#endif