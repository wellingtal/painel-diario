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

    // 네트워크로 전송한 뒤, 30초 후에 재전송 시도를 한다.
    const int MAX_WAIT_MSEC = 30 * 1000;

    if (!m_GiveItemMap.empty())
    {
        unsigned long dwCurrentTime = timeGetTime();
        if (0 == dwCurrentTime) { ++dwCurrentTime; }

        // 읽은 아이템 정보를 바탕으로 아이템을 만든 후에 전송한다.        
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
                    // LogoutDB에서 데이터 찾았음. 그냥 아이템 생성해서 업데이트한다.
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
						// LogoutDB에서 데이터 못찾았음. 로그인 해 있으므로 찾아본다.
						if (CSessionData::SE_CHAR_ENABLED == lpSessionData->GetSessionState())
						{
							// 네트워크로 데이터 전송한다.
							giveItemToTempInven.m_GiveItemInfo[cNetworkGiveItemCount] = giveItem.first;
							giveItem.second = dwCurrentTime;

							++cNetworkGiveItemCount;

							INFLOG3(g_Log, "UID:%10u / CID:%10u / CreateID:%10u / 아이템 지급 명령을 게임서버로 보냅니다",
								giveItem.first.m_dwUID, giveItem.first.m_dwCID, giveItem.first.m_dwCreateID);
						}
						else
						{
							// 아예 로그인을 하지 않은 경우, 로그아웃한 상태와 동일한 처리를 한다.
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
                // 최대 MAX_WAIT_MSEC 만큼 기다렸다가, 재전송 준비를 한다.
                giveItem.second = 0;
            }

			++pos;
        }

        if (0 < cNetworkGiveItemCount)
        {   
            giveItemToTempInven.m_cGiveItemNum = cNetworkGiveItemCount;

            // 전 서버군에 전부 전송한다.
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
            ERRLOG1(g_Log, "DB에러로 아이템 주기 정보를 읽어올 수 없습니다. : %s", 
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
					// edith 2009.09.14 아이템 구매내역을 저장했다가 NFAuthServer로 전달하기
					info.CreateID	= lpPos->m_dwCreateID;
					info.UID		= lpPos->m_dwUID;
					info.CID		= lpPos->m_dwCID;
					info.ItemPrototypeID = lpPos->m_usPrototypeID;
					info.Amount		= lpPos->m_cAmount;	
					m_NFItemInfo.push_back(info);
#endif
                }
            }

			// 더 이상 읽어들일 것이 없으면, 펜딩해둔 것들을 풀어준다.
			if (true == m_GiveItemMap.empty())
			{
				char szQuery[1024] = { 0, };
				sprintf(szQuery, "UPDATE TblWebItemInfo SET ItemUID = 0 WHERE ItemUID = %d", (unsigned __int64)Item::GiveItem::PENDING_ITEM_UID);
				if (false == m_lpDBComponent->ExecuteQuery(szQuery))
				{
					ERRLOG1(g_Log, "DB에러로 펜딩 데이터를 복구시킬 수 없습니다. : %s", 
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
            "아이템 시리얼 업데이트에 실패했습니다. 다음 기회에 업데이트합니다. : %s",
            giveItemInfo.m_dwUID, giveItemInfo.m_dwCID, giveItemInfo.m_dwCreateID, dwItemSerial, 
            0 == m_lpDBComponent ? "DBComponent is NULL" : m_lpDBComponent->GetErrorString());
    }
    else
    {
        // 업데이트에 성공했으면 제거한다.
        INFLOG4(g_Log, "UID:%10u / CID:%10u / CreateID:%10u / ItemSerial:%016I64X / 임시인벤으로 아이템 지급에 성공했습니다",
            giveItemInfo.m_dwUID, giveItemInfo.m_dwCID, giveItemInfo.m_dwCreateID, dwItemSerial);

        NextPos = m_GiveItemMap.erase(pos);
        return true;
    }

    return false;
}

bool CGiveItemMgr::UpdateItemSerial(unsigned long dwCreateID, unsigned __int64 dwItemSerial)
{
    // 일단 Ack가 왔으면 숫자는 까고 본다.

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
        ERRLOG2(g_Log, "CreateID:%d/ ItemSerial:0x%016I64X / 존재하지 않는 CreateID나, 이상한 ItemSerial입니다.", 
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
	
	// 한번에 50개씩 전달
	if(Count >= 50)
		Count = 50;

	SHORT Len = 0;
	char szBuff[1000];

	LPNFITEMINFO item;
	
	for(SHORT i = 0; i < Count; ++i)
	{
		obj = m_NFItemInfo.begin();

		// NFITEMINFO 만큼 복사
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
    // 아이템 갯수를 세면서, 개수제한에 걸린 건 아닌지를 살피고,
    // 인덱스를 재조정한다.    
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

