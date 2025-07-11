#include "stdafx.h"
#include "DBItemSerialMgr.h"

#include <DB/DBComponent.h>
#include <DB/GameDBComponent.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>

#include <Log/ServerLog.h>

namespace DBAgent
{
namespace DataStorage
{


CDBAgentItemSerialMgr& CDBAgentItemSerialMgr::GetInstance()
{
    static CDBAgentItemSerialMgr dbAgentItemSerialMgr;
    return dbAgentItemSerialMgr;
}


void CDBAgentItemSerialMgr::SaveChangedSerial(CDBComponent& DBComponent)
{
    if(m_dwLastChangedItemSerial != GetItemSerial())
    {
        SaveItemSerial(DBComponent, GetServerID());
        m_dwLastChangedItemSerial = GetItemSerial();
    }
}


}
}