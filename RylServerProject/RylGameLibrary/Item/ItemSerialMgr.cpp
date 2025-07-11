#include "stdafx.h"
#include "ItemSerialMgr.h"

#include <Network/Packet/PacketStruct/ServerInfo.h>

#include <DB/DBComponent.h>
#include <DB/GameDBComponent.h>

namespace Item
{

CItemSerialMgr::CItemSerialMgr()
:   m_dwItemSerial(0LL), m_dwServerID(0)
{

}

CItemSerialMgr::~CItemSerialMgr()
{

}

bool CItemSerialMgr::SetItemSerial(unsigned __int64 dwItemSerial)
{
    if(m_dwItemSerial < dwItemSerial)
    {
        m_dwItemSerial = dwItemSerial;
        return true;
    }

    return false;
}

bool CItemSerialMgr::LoadItemSerial(CDBComponent& DBComponent, unsigned long dwServerID)
{
    m_dwItemSerial = 0LL;
    m_dwServerID   = dwServerID;

    using namespace DBComponent;

    if (!GameDB::GetItemUID(DBComponent, dwServerID, &m_dwItemSerial, true)) 
    {
        // 아이템 시리얼 막 커지는 건 버그였음.. 전과 같이 존 / 채널 2byte체제로 간다.

        SERVER_ID serverID;
        serverID.dwID = dwServerID;

        m_dwItemSerial = ((static_cast<unsigned __int64>(serverID.GetZone()) << 56) & 0xFF00000000000000LL) | 
            ((static_cast<unsigned __int64>(serverID.GetChannel()) << 48) & 0x00FF000000000000LL);


        // 등록되지 않은 서버
        if (!GameDB::InsertItemUID(DBComponent, dwServerID, m_dwItemSerial)) 
        {
            ERRLOG1(g_Log, "ServerID:0x%08X / 아이템 시리얼 얻기 실패", m_dwServerID);
            return false;
        }
    }

    // 아이템 시리얼 중복을 막기 위해, 임의로 약간 더해준다
    m_dwItemSerial += 10LL;
    return true;
}

bool CItemSerialMgr::SaveItemSerial(CDBComponent& DBComponent, unsigned long dwServerID)
{
    if(m_dwServerID != dwServerID)
    {
        ERRLOG2(g_Log, "OldServerID:0x%08X / NewServerID:0x%08X / 아이템 시리얼 저장 실패 : 서버 ID가 다릅니다", 
            m_dwServerID, dwServerID);
    }
    else
    {
        using namespace DBComponent;

        if(GameDB::SetItemUID(DBComponent, dwServerID, m_dwItemSerial))
        {
            return true;
        }
        else
        {
            ERRLOG2(g_Log, "ServerID:0x%08X / ItemSerial : 0x016I64X / 아이템 시리얼 저장 실패 : DB에 시리얼 저장 실패", 
                m_dwServerID, m_dwItemSerial);
        }
    }

    return false;
}


}