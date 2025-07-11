#include "stdafx.h"
#include "LoginDispatch.h"

#include <Log/ServerLog.h>
#include <Network/Address/INET_Addr.h>
#include <Network/SendPacket/SendServerInfo.h>

#include <Utility/Setup/ServerSetup.h>

#include "MapRewardDayToItem.h"
#include <string>
#include <windows.h>
#include <sql.h>
#include <sqlext.h>

namespace DBAgent
{

CSingleDispatch& CLoginDispatch::GetDispatchTable()
{
    static CSingleDispatch loginDispatch;
    return loginDispatch;
}

CLoginDispatch::CLoginDispatch(CSession& Session)
    :   CRylServerDispatch(Session, MAX_PACKET_DISPATCH_PER_PULSE)
{


}

CLoginDispatch::~CLoginDispatch()
{


}

void CLoginDispatch::Connected()
{
    DETLOG3(g_Log, "SS:0x%08x/DP:0x%08x/IP:%15s/LoginServer Connected",
        &GetSession(), this, GetRemoteAddr().get_addr_string());

    GetDispatchTable().SetDispatch(this);

    CSendStream& SendStream = GetSendStream();
    unsigned long dwServerID = CServerSetup::GetInstance().GetServerID();

    SendPacket::ServerLogin(SendStream, dwServerID);
    SendPacket::UpdateServerVersion(SendStream);
    SendPacket::UpdateChannel(SendStream, dwServerID);
}

void ExecuteSQL(const std::string& query); // Supondo que esta função esteja declarada em outro lugar

void HandleLoginReward(int accountId, int charId, int day) {
    int itemId = MapRewardDayToItem(day);
    if (itemId == 0) return;

    std::string sql = "INSERT INTO WebRewardQueue (AccountID, CharID, ItemID, Amount, Reason, Delivered) VALUES ('" +
                      std::to_string(accountId) + "', " +
                      std::to_string(charId) + ", " +
                      std::to_string(itemId) + ", 1, 'DailyLoginReward', 0)";
    ExecuteSQL(sql);
}

void CLoginDispatch::Disconnected()
{
    DETLOG3(g_Log, "SS:0x%08x/DP:0x%08x/IP:%15s/LoginServer Disconnected",
        &GetSession(), this, GetRemoteAddr().get_addr_string());

    GetDispatchTable().RemoveDispatch(this);
}

bool CLoginDispatch::DispatchPacket(PktBase* lpPktBase)
{


    return true;
}

}


