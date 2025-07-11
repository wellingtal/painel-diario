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
        // ������ �ø��� �� Ŀ���� �� ���׿���.. ���� ���� �� / ä�� 2byteü���� ����.

        SERVER_ID serverID;
        serverID.dwID = dwServerID;

        m_dwItemSerial = ((static_cast<unsigned __int64>(serverID.GetZone()) << 56) & 0xFF00000000000000LL) | 
            ((static_cast<unsigned __int64>(serverID.GetChannel()) << 48) & 0x00FF000000000000LL);


        // ��ϵ��� ���� ����
        if (!GameDB::InsertItemUID(DBComponent, dwServerID, m_dwItemSerial)) 
        {
            ERRLOG1(g_Log, "ServerID:0x%08X / ������ �ø��� ��� ����", m_dwServerID);
            return false;
        }
    }

    // ������ �ø��� �ߺ��� ���� ����, ���Ƿ� �ణ �����ش�
    m_dwItemSerial += 10LL;
    return true;
}

bool CItemSerialMgr::SaveItemSerial(CDBComponent& DBComponent, unsigned long dwServerID)
{
    if(m_dwServerID != dwServerID)
    {
        ERRLOG2(g_Log, "OldServerID:0x%08X / NewServerID:0x%08X / ������ �ø��� ���� ���� : ���� ID�� �ٸ��ϴ�", 
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
            ERRLOG2(g_Log, "ServerID:0x%08X / ItemSerial : 0x016I64X / ������ �ø��� ���� ���� : DB�� �ø��� ���� ����", 
                m_dwServerID, m_dwItemSerial);
        }
    }

    return false;
}


}