#include "stdafx.h"
#include "GiveItemMgr.h"

#include <DB/DBComponent.h>
#include <Log/ServerLog.h>

#include <Item/ItemMgr.h>
#include <Item/ItemFactory.h>

#include <DataStorage/SessionDataMgr.h>
#include <DataStorage/CharacterDataMgr.h>
#include <DataStorage/DBItemSerialMgr.h>

#include <Network/Dispatch/GameDispatch.h>

#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/WrapPacket.h>
#include <Network/Packet/PacketStruct/CharItemPacket.h>

CGiveItemMgr& CGiveItemMgr::GetInstance()
{
    static CGiveItemMgr giveItemMgr;
    return giveItemMgr;
}

CGiveItemMgr::CGiveItemMgr()
:   m_lpDBComponent(0)
{
    memset(&m_UpdateParamInfo, 0, sizeof(OleDB::PARAM_INFO));

    m_UpdateParamInfo.ColNum = 2;

    m_UpdateParamInfo.eParamIO[0] = DBPARAMIO_INPUT;
    m_UpdateParamInfo.ColType[0]  = DBTYPE_I8;
    m_UpdateParamInfo.ColSize[0]  = sizeof(unsigned __int64);

    m_UpdateParamInfo.eParamIO[1] = DBPARAMIO_INPUT;
    m_UpdateParamInfo.ColType[1]  = DBTYPE_I4;
    m_UpdateParamInfo.ColSize[1]  = sizeof(unsigned long);
}

CGiveItemMgr::~CGiveItemMgr()
{


}


void CGiveItemMgr::SetDBComponent(CDBComponent& DBComponent)
{
    m_lpDBComponent = &DBComponent;    
}

void CGiveItemMgr::CheckGiveItem()
{
    using namespace DBAgent::DataStorage;

    // ��Ʈ��ũ�� ������ ��, 30�� �Ŀ� ������ �õ��� �Ѵ�.
    const int MAX_WAIT_MSEC = 30 * 1000;

    if (!m_GiveItemMap.empty())
    {
        unsigned long dwCurrentTime = timeGetTime();
        if (0 == dwCurrentTime) { ++dwCurrentTime; }

        // ���� ������ ������ �������� �������� ���� �Ŀ� �����Ѵ�.        
        CSessionDataMgr&    sessionDataMgr = CSessionDataMgr::GetInstance();
        CCharacterDataMgr&  characterDataMgr = CCharacterDataMgr::GetInstance();

        CSessionData*       lpSessionData	= NULL;
        CCharacterData*     lpCharacterData = NULL;

        PktGiveItemToTempInven giveItemToTempInven;
        memset(&giveItemToTempInven, 0, sizeof(PktGiveItemToTempInven));

        GiveItemMap::iterator pos = m_GiveItemMap.begin();
        GiveItemMap::iterator end = m_GiveItemMap.end();
        GiveItemMap::iterator nextPos;

        unsigned __int64 dwItemUID = 0LL;

        unsigned char cGiveItemCount = 0;
        unsigned char cNetworkGiveItemCount = 0;

        TEMPINVEN tempInven;
        memset(&tempInven, 0, sizeof(TEMPINVEN));

        for (; cGiveItemCount < PktGiveItemToTempInven::MAX_GIVE_ITEM && pos != end; ++cGiveItemCount)
        {                
            GiveItem& giveItem = pos->second;

            if (0 == giveItem.second)
            {
				lpCharacterData = characterDataMgr.GetLogoutData(giveItem.first.m_dwCID);
                if (NULL != lpCharacterData)
                {
                    // LogoutDB���� ������ ã����. �׳� ������ �����ؼ� ������Ʈ�Ѵ�.
                    tempInven = lpCharacterData->GetTempInven();

                    if (true == SetItem(tempInven.Data, tempInven.dwSize, TEMPINVEN::MAX_TEMPINVEN_SIZE, 
						giveItem.first, Item::MAX_TEMP_INVEN_ITEM_NUM, dwItemUID))
					{
						if (true == UpdateItemSerial(pos, nextPos, dwItemUID))
						{
							lpCharacterData->SetTempInven(tempInven.Data, tempInven.dwSize);
						}
					}
					else
					{
						UpdateItemSerial(pos, nextPos, Item::GiveItem::PENDING_ITEM_UID);
					}

					pos = nextPos;
					continue;
                }
				else
				{
					if (NULL != (lpSessionData = sessionDataMgr.GetCharLoadedSession(giveItem.first.m_dwCID)) &&
						NULL != (lpCharacterData = lpSessionData->GetCharacterData()) &&
						giveItem.first.m_dwCID == lpCharacterData->GetCID())
					{
						// LogoutDB���� ������ ��ã����. �α��� �� �����Ƿ� ã�ƺ���.
						if (CSessionData::SE_CHAR_ENABLED == lpSessionData->GetSessionState())
						{
							// ��Ʈ��ũ�� ������ �����Ѵ�.
							giveItemToTempInven.m_GiveItemInfo[cNetworkGiveItemCount] = giveItem.first;
							giveItem.second = dwCurrentTime;

							++cNetworkGiveItemCount;

							INFLOG3(g_Log, "UID:%10u / CID:%10u / CreateID:%10u / ������ ���� ����� ���Ӽ����� �����ϴ�",
								giveItem.first.m_dwUID, giveItem.first.m_dwCID, giveItem.first.m_dwCreateID);
						}
						else
						{
							// �ƿ� �α����� ���� ���� ���, �α׾ƿ��� ���¿� ������ ó���� �Ѵ�.
							tempInven = lpCharacterData->GetTempInven();

							if (true == SetItem(tempInven.Data, tempInven.dwSize, TEMPINVEN::MAX_TEMPINVEN_SIZE, 
								giveItem.first, Item::MAX_TEMP_INVEN_ITEM_NUM, dwItemUID))
							{
								if (true == UpdateItemSerial(pos, nextPos, dwItemUID))
								{
									lpCharacterData->SetTempInven(tempInven.Data, tempInven.dwSize);
								}
							}
							else
							{
								UpdateItemSerial(pos, nextPos, Item::GiveItem::PENDING_ITEM_UID);
							}

							pos = nextPos;
							continue;
						}
					}
				}
            }
            else if (MAX_WAIT_MSEC < dwCurrentTime - giveItem.second)
            {
                // �ִ� MAX_WAIT_MSEC ��ŭ ��ٷȴٰ�, ������ �غ� �Ѵ�.
                giveItem.second = 0;
            }

			++pos;
        }

        if (0 < cNetworkGiveItemCount)
        {   
            giveItemToTempInven.m_cGiveItemNum = cNetworkGiveItemCount;

            // �� �������� ���� �����Ѵ�.
            if (PacketWrap::WrapHeader(reinterpret_cast<char*>(&giveItemToTempInven), 
                sizeof(PktGiveItemToTempInven), CmdGiveItemToTempInven, 0, 0))
            {
                DBAgent::CGameDispatch::GetDispatchTable().Process(
                    CSendPacketAllServer(reinterpret_cast<char*>(&giveItemToTempInven), 
                    sizeof(PktGiveItemToTempInven), CmdGiveItemToTempInven));
            }
        }
    }
}

void CGiveItemMgr::ReadGiveInfo()
{
    const int MAX_READ_INFO = 1024;

    GiveItemInfo giveItemInfo[MAX_READ_INFO];
    
    if (true == m_GiveItemMap.empty())
    {
		if (NULL == m_lpDBComponent)
		{
			ERRLOG0(g_Log, "DBComponent is NULL");
			return;
		}

		if (false == m_lpDBComponent->ExecuteQuery(
            "SELECT TOP 1024 CreateID, UID, CID, ItemPrototypeID, Amount, "
            "MIN_DAMAGE, MAX_DAMAGE, ARMOR, HIT_RATE, EVADE, MAX_HP, HP_REGEN, MAX_MP, "
            "MP_REGEN, CRITICAL, BLOCK, SPEED, MAGIC_POWER, MAGIC_RESIST, ItemUID "
            "FROM TblWebItemInfo WHERE ItemUID = 0"))
        {
            ERRLOG1(g_Log, "DB������ ������ �ֱ� ������ �о�� �� �����ϴ�. : %s", 
				m_lpDBComponent->GetErrorString());
        }
        else
        {   
            int nGetRows = 0;

            while (m_lpDBComponent->GetData((void**)giveItemInfo, sizeof(GiveItemInfo), MAX_READ_INFO, &nGetRows))
            {
                if (0 == nGetRows)
                {
                    break;
                }

                GiveItemInfo* lpPos = giveItemInfo;
                GiveItemInfo* lpEnd = giveItemInfo + nGetRows;
#ifdef AUTH_MY
				NFITEMINFO info;
#endif

                for (; lpPos != lpEnd; ++lpPos)
                {
                    m_GiveItemMap.insert(GiveItemMap::value_type(lpPos->m_dwCreateID, GiveItem(*lpPos, 0)));

#ifdef AUTH_MY
					// edith 2009.09.14 ������ ���ų����� �����ߴٰ� NFAuthServer�� �����ϱ�
					info.CreateID	= lpPos->m_dwCreateID;
					info.UID		= lpPos->m_dwUID;
					info.CID		= lpPos->m_dwCID;
					info.ItemPrototypeID = lpPos->m_usPrototypeID;
					info.Amount		= lpPos->m_cAmount;	
					m_NFItemInfo.push_back(info);
#endif
                }
            }

			// �� �̻� �о���� ���� ������, ����ص� �͵��� Ǯ���ش�.
			if (true == m_GiveItemMap.empty())
			{
				char szQuery[1024] = { 0, };
				sprintf(szQuery, "UPDATE TblWebItemInfo SET ItemUID = 0 WHERE ItemUID = %d", (unsigned __int64)Item::GiveItem::PENDING_ITEM_UID);
				if (false == m_lpDBComponent->ExecuteQuery(szQuery))
				{
					ERRLOG1(g_Log, "DB������ ��� �����͸� ������ų �� �����ϴ�. : %s", 
						m_lpDBComponent->GetErrorString());
				}
			}
        }
    }
}

bool CGiveItemMgr::UpdateItemSerial(GiveItemMap::iterator pos, 
                                    GiveItemMap::iterator& NextPos, 
                                    unsigned __int64 dwItemSerial)
{
    GiveItem&       giveItem = pos->second;
    GiveItemInfo&   giveItemInfo = giveItem.first;

    UpdateItemUID   updateItemUID;

    giveItemInfo.m_dwItemUID      = dwItemSerial;
    updateItemUID.m_dwItemUID     = dwItemSerial;
    updateItemUID.m_dwCreateID    = giveItemInfo.m_dwCreateID;

    if (0 == m_lpDBComponent ||
        !m_lpDBComponent->ExecuteQueryWithParams(
        "UPDATE TblWebItemInfo SET ItemUID=? WHERE CreateID=?", 
        reinterpret_cast<char*>(&updateItemUID), m_UpdateParamInfo))
    {
        ERRLOG5(g_Log, "UID:%10u / CID:%10u / CreateID:%d/ ItemSerial:0x%016I64X / "
            "������ �ø��� ������Ʈ�� �����߽��ϴ�. ���� ��ȸ�� ������Ʈ�մϴ�. : %s",
            giveItemInfo.m_dwUID, giveItemInfo.m_dwCID, giveItemInfo.m_dwCreateID, dwItemSerial, 
            0 == m_lpDBComponent ? "DBComponent is NULL" : m_lpDBComponent->GetErrorString());
    }
    else
    {
        // ������Ʈ�� ���������� �����Ѵ�.
        INFLOG4(g_Log, "UID:%10u / CID:%10u / CreateID:%10u / ItemSerial:%016I64X / �ӽ��κ����� ������ ���޿� �����߽��ϴ�",
            giveItemInfo.m_dwUID, giveItemInfo.m_dwCID, giveItemInfo.m_dwCreateID, dwItemSerial);

        NextPos = m_GiveItemMap.erase(pos);
        return true;
    }

    return false;
}

bool CGiveItemMgr::UpdateItemSerial(unsigned long dwCreateID, unsigned __int64 dwItemSerial)
{
    // �ϴ� Ack�� ������ ���ڴ� ��� ����.

    GiveItemMap::iterator pos;
    GiveItemMap::iterator nextPos;
    GiveItemMap::iterator end = m_GiveItemMap.end();

    if (0 != dwCreateID && 0 != dwItemSerial && 
        end != (pos = m_GiveItemMap.find(dwCreateID)))
    {
        if (UpdateItemSerial(pos, nextPos, dwItemSerial))
        {
            return true;
        }
    }
    else
    {        
        ERRLOG2(g_Log, "CreateID:%d/ ItemSerial:0x%016I64X / �������� �ʴ� CreateID��, �̻��� ItemSerial�Դϴ�.", 
            dwCreateID, dwItemSerial);
    }

    return false;
}

#ifdef AUTH_MY
bool CGiveItemMgr::UpdateBuyItemInfo()
{
	if(m_NFItemInfo.empty())
		return false;

	SHORT Count = (SHORT)m_NFItemInfo.size();
	std::list<NFITEMINFO>::iterator obj;
	
	// �ѹ��� 50���� ����
	if(Count >= 50)
		Count = 50;

	SHORT Len = 0;
	char szBuff[1000];

	LPNFITEMINFO item;
	
	for(SHORT i = 0; i < Count; ++i)
	{
		obj = m_NFItemInfo.begin();

		// NFITEMINFO ��ŭ ����
		item = &(*obj);

		memcpy(&szBuff[Len], item, sizeof(NFITEMINFO) );
		Len += sizeof(NFITEMINFO);

		m_NFItemInfo.pop_front();
	}

	g_NetAuth.Send_BUYITEMINFO(Count, szBuff, Len);
	return true;
}
#endif

bool CGiveItemMgr::SetItem(char* szData, unsigned long& dwDataSize_InOut, 
                           unsigned long dwMaxDataSize, GiveItemInfo& giveItemInfo, 
                           unsigned long dwMaxItemNum, unsigned __int64& dwItemUID)
{
    // ������ ������ ���鼭, �������ѿ� �ɸ� �� �ƴ����� ���ǰ�,
    // �ε����� �������Ѵ�.    
    char* szDataPos = szData;
    char* szDataEnd = szData + dwDataSize_InOut;
    
    unsigned char cIndex = 0;

    for (; szDataPos < szDataEnd; )
    {
        Item::ItemData* lpItemData = reinterpret_cast<Item::ItemData*>(szDataPos);
        lpItemData->m_ItemPos.m_cIndex = cIndex;

        szDataPos += lpItemData->m_cItemSize;
        ++cIndex;
    }

    if (cIndex < Item::MAX_TEMP_INVEN_ITEM_NUM)
    {
        Item::ItemPos itemPos(TakeType::TS_TEMPINVEN, cIndex);
        
        unsigned long dwRemainSize = dwMaxDataSize - dwDataSize_InOut;
        dwItemUID = DBAgent::DataStorage::CDBAgentItemSerialMgr::GetInstance().GetNewItemSerial();

        if (Item::CreateItemDataFromGiveInfo(Item::CItemMgr::GetInstance(), giveItemInfo, 
            szData + dwDataSize_InOut, dwRemainSize, dwItemUID, itemPos))
        {
            dwDataSize_InOut += dwRemainSize;
            return true;
        }
    }
    
    return false;
}

