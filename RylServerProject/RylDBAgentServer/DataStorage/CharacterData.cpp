#include "stdafx.h"
#include "CharacterData.h"
#include "SessionData.h"
#include "DBDataMgr.h"
#include "DBItemSerialMgr.h"

#include <DB/DBComponent.h>
#include <DB/GameDBComponent.h>
#include <Log/ServerLog.h>

#include <Item/ItemStructure.h>

#include <Item/ItemStructure.h>
#include <Item/ItemConstants.h>
#include <Item/Container/ContainerConstant.h>
#include <Item/RebalanceConvert/ContainerChecker.h>
#include <Item/RebalanceConvert/RebalanceItem.h>
#include <Utility/Debug/PerformanceCheck.h>

#include <Network/Packet/PacketStruct/CharLoginOutPacketStruct.h>
#include <mmsystem.h>

#include <Network/Dispatch/GameDispatch.h>
#include <Creature/Character/CharacterCreate.h>
#include <Creature/Character/ExpTable.h>

#include <Community/FriendList.h>

#include <DataStorage/SessionDataMgr.h>

#include <GameTime/GameTimeDBMgr.h>

namespace DBAgent
{
namespace DataStorage
{

CCharacterData::CCharacterData()
:   m_FriendList(0), m_BanList(0), m_dwUpdateData(0), m_dwLastDBUpdateTime(0)
{
    ClearData();
}

CCharacterData::~CCharacterData()
{

}

void CCharacterData::SetUID(unsigned long dwUID)
{
	m_dwUID = dwUID;
}

void CCharacterData::SetPID(unsigned long dwPID)
{
    m_CharInfo.PID = dwPID;
    m_dwUpdateData |= CHANGED_CHAR_INFOST;
}	


void CCharacterData::SetGID(unsigned long dwGID)
{
    m_CharInfo.GID = dwGID;
    m_dwUpdateData |= CHANGED_CHAR_INFOST;
}

void CCharacterData::SetServerID(unsigned long dwServerID)
{
    m_CharInfoEx.ServerID = dwServerID;
    m_dwUpdateData |= CHANGED_CHAR_INFOEX;
}

bool CCharacterData::SetInfo(const CHAR_INFOST& charInfoST, bool bChangeName)
{   
    CHAR_INFOST local_InfoST = charInfoST;

    if(!bChangeName)
    {
        // 이름을 변경하지 않는 경우는, 이름을 지금 이름으로 덮어씌운다.
        memcpy(local_InfoST.Name, m_CharInfo.Name, 
            sizeof(char) * CHAR_INFOST::MAX_NAME_LEN);
    }

    if(m_CharInfo.CID != local_InfoST.CID || 0 == local_InfoST.Level)
    {
        SERLOG3(g_Log, "CID:%10u / 캐릭터 데이터 업데이트 에러 : UpdateCID : %10u, Level : %d", 
            m_CharInfo.CID, local_InfoST.CID, local_InfoST.Level);

        return false;
    }
    else if(0 != memcmp(&m_CharInfo, &local_InfoST, sizeof(CHAR_INFOST)))
    {
        m_CharInfo = local_InfoST;
        m_dwUpdateData |= CHANGED_CHAR_INFOST;
    }

    return true;
}

void CCharacterData::SetPos(const CHAR_POS& charPos)
{
    if(0 != memcmp(&m_CharPos, &charPos, sizeof(CHAR_POS)))
    {
        m_CharPos = charPos;
        m_dwUpdateData |= CHANGED_CHAR_POS;
    }
}

void CCharacterData::SetSkill(const SKILL& skill)
{
    if(0 != memcmp(&m_Skill, &skill, sizeof(SKILL)))
    {
        m_Skill = skill;
        m_dwUpdateData |= CHANGED_SKILL;
    }

}

void CCharacterData::SetQuick(const QUICK& quick)
{
    if(0 != memcmp(&m_Quick, &quick, sizeof(QUICK)))
    {
        m_Quick = quick;
        m_dwUpdateData |= CHANGED_QUICK;
    }
}

void CCharacterData::SetSpell(const SPELL& spell)
{
	if(0 != memcmp(&m_Spell, &spell, sizeof(SPELL)))
	{
		m_Spell = spell;
		m_dwUpdateData |= CHANGED_SPELL;
	}
}

void CCharacterData::SetInfoEx(const CHAR_INFOEX& charInfoEx)
{
    if(0 != memcmp(&m_CharInfoEx, &charInfoEx, sizeof(CHAR_INFOEX)))
    {
        m_CharInfoEx = charInfoEx;
        m_dwUpdateData |= CHANGED_CHAR_INFOEX;
    }
}


void CCharacterData::SetQuest(const QUEST& quest)
{
    if(0 != memcmp(&m_Quest, &quest, sizeof(QUEST)))
    {
        m_Quest = quest;
        m_dwUpdateData |= CHANGED_QUEST;
    }
}


void CCharacterData::SetHistory(const HISTORY& history)
{
    if(0 != memcmp(&m_History, &history, sizeof(HISTORY)))
    {
        m_History = history;
        m_dwUpdateData |= CHANGED_HISTORY;
    }
}


void CCharacterData::SetConfig(const CONFIG& config)
{
    if(0 != memcmp(&m_Config, &config, sizeof(CONFIG)))
    {
        m_Config = config;
        m_dwUpdateData |= CHANGED_CONFIG;
    }
}


bool CCharacterData::SetEquip(const char* szData, unsigned long dwDataLen)
{
    if(EQUIP::MAX_EQUIP_SIZE < dwDataLen)
    {
        SERLOG3(g_Log, "CID:%10u / 캐릭터 데이터 업데이트 에러 : 장비정보(%u)의 크기(%u) 에러", 
            GetCID(), EQUIP::MAX_EQUIP_SIZE, dwDataLen);

        return false;
    }
    else if(m_Equip.dwSize != dwDataLen || 0 != memcmp(m_Equip.Data, szData, dwDataLen))
    {
        memcpy(m_Equip.Data, szData, dwDataLen);
        m_Equip.dwSize = dwDataLen;

        m_dwUpdateData |= CHANGED_EQUIP;
    }

    return true;
}


bool CCharacterData::SetInven(const char* szData, unsigned long dwDataLen)
{
    if(INVEN::MAX_INVEN_SIZE < dwDataLen)
    {
        SERLOG3(g_Log, "CID:%10u / 캐릭터 데이터 업데이트 에러 : 인벤정보(%u)의 크기(%u) 에러",
            GetCID(), INVEN::MAX_INVEN_SIZE, dwDataLen);

        return false;
    }
    else if(m_Inven.dwSize != dwDataLen || 0 != memcmp(m_Inven.Data, szData, dwDataLen))
    {
        memcpy(m_Inven.Data, szData, dwDataLen);
        m_Inven.dwSize = dwDataLen;

        m_dwUpdateData |= CHANGED_INVEN;
    }

    return true;
}


bool CCharacterData::SetExtra(const char* szData, unsigned long dwDataLen)
{
    if(EXTRA::MAX_EXTRA_SIZE < dwDataLen)
    {
        SERLOG3(g_Log, "CID:%10u / 캐릭터 데이터 업데이트 에러 : EXTRA정보(%u)의 크기(%u) 에러",
            GetCID(), EXTRA::MAX_EXTRA_SIZE, dwDataLen);

        return false;
    }
    else if(m_Extra.dwSize != dwDataLen || 0 != memcmp(m_Extra.Data, szData, dwDataLen))
    {
        memcpy(m_Extra.Data, szData, dwDataLen);
        m_Extra.dwSize = dwDataLen;

        m_dwUpdateData |= CHANGED_EXTRA;
    }

    return true;
}

bool CCharacterData::SetExchange(const char* szData, unsigned long dwDataLen)
{
    if(EXCHANGE::MAX_EXCHANGE_SIZE < dwDataLen)
    {
        SERLOG3(g_Log, "CID:%10u / 캐릭터 데이터 업데이트 에러 : EXCHANGE정보(%u)의 크기(%u) 에러",
            GetCID(), EXCHANGE::MAX_EXCHANGE_SIZE, dwDataLen);

        return false;
    }
    else if(m_Exchange.dwSize != dwDataLen || 0 != memcmp(m_Exchange.Data, szData, dwDataLen))
    {
        memcpy(m_Exchange.Data, szData, dwDataLen);
        m_Exchange.dwSize = dwDataLen;

        m_dwUpdateData |= CHANGED_EXCHANGE;
    }

    return true;
}

bool CCharacterData::SetTempInven(const char* szData, unsigned long dwDataLen)
{
    if(TEMPINVEN::MAX_TEMPINVEN_SIZE < dwDataLen)
    {
        SERLOG3(g_Log, "CID:%10u / 캐릭터 데이터 업데이트 에러 : 임시인벤정보(%u)의 크기(%u) 에러",
            GetCID(), TEMPINVEN::MAX_TEMPINVEN_SIZE, dwDataLen);

        return false;
    }
    else if(m_TempInven.dwSize != dwDataLen || 0 != memcmp(m_TempInven.Data, szData, dwDataLen))
    {
        memcpy(m_TempInven.Data, szData, dwDataLen);
        m_TempInven.dwSize = dwDataLen;

        m_dwUpdateData |= CHANGED_TEMPINVEN;
    }

    return true;
}

bool CCharacterData::SetGuildWarFlag(unsigned char cFlag)
{
	if (cFlag >= Creature::WAR_OFF && cFlag <= Creature::WAR_INSTANCE)
	{
		m_CharInfoEx.GuildWarFlag = cFlag;
		return DBComponent::GameDB::UpdateGuildWarFlag(CDBSingleObject::GetInstance(), GetCID(), cFlag);
	}

	return false;
}

bool CCharacterData::SetRealmWarFlag(unsigned char cFlag)
{
	if (cFlag >= Creature::WAR_OFF && cFlag <= Creature::WAR_INSTANCE)
	{
		m_CharInfoEx.RealmWarFlag = cFlag;
		return DBComponent::GameDB::UpdateRealmWarFlag(CDBSingleObject::GetInstance(), GetCID(), cFlag);
	}

	return false;
}

void CCharacterData::ClearData()
{
    memset(&m_CharInfo, 0, sizeof(CHAR_INFOST));    // 기본 정보
    memset(&m_CharPos, 0, sizeof(CHAR_POS));		// 위치 정보
    memset(&m_Skill, 0, sizeof(SKILL));		        // 스킬 정보
    memset(&m_Quick, 0, sizeof(QUICK));		        // 퀵슬롯

    memset(&m_CharInfoEx, 0, sizeof(CHAR_INFOEX));	// 추가 정보
    memset(&m_Quest, 0, sizeof(QUEST));		        // 퀘스트
    memset(&m_History, 0, sizeof(HISTORY));		    // 히스토리	
    memset(&m_Config, 0, sizeof(CONFIG));		    // 설정

    memset(&m_Equip, 0, sizeof(EQUIP));     		// 장비
    memset(&m_Inven, 0, sizeof(INVEN));     		// 인벤
    memset(&m_Extra, 0, sizeof(EXTRA));     		// 여분
    memset(&m_Exchange, 0, sizeof(EXCHANGE));		// 교환
    memset(&m_TempInven, 0, sizeof(TEMPINVEN));	    // 임시 인벤토리

    m_BanList.Clear();		// 거부
    m_FriendList.Clear();	// 친구

    m_dwUID = 0;

    memset(&m_EquipView, 0, sizeof(unsigned short) * MAX_EQUIP_VIEW); // 장비 뷰
    m_dwUpdateData = 0;     // 데이터가 업데이트되었는지를 나타냄.
}

void CCharacterData::ReloadEquipView()
{    
    const char* lpDataPos = m_Equip.Data;
    const char* lpDataEnd = m_Equip.Data + m_Equip.dwSize;

    memset(m_EquipView, 0, sizeof(unsigned short) * MAX_EQUIP_VIEW);

    for(; lpDataPos < lpDataEnd; )
    {
        const Item::ItemData* lpItemData =
            reinterpret_cast<const Item::ItemData*>(lpDataPos);

        unsigned short usIndex = lpItemData->m_ItemPos.m_cIndex;

        if(usIndex < MAX_EQUIP_VIEW)
        {
            m_EquipView[usIndex] = lpItemData->m_usProtoTypeID;
        }
        else
        {
            ERRLOG3(g_Log, "CID:%10u / 장비 뷰 세팅중에 잘못된 장비(PrototypeID:%6d/Pos:%4d) 발견.",
                GetCID(), lpItemData->m_usProtoTypeID, usIndex);
        }

        lpDataPos += lpItemData->m_cItemSize;

        if(0 == lpItemData->m_cItemSize)
        {
            ERRLOG4(g_Log, "CID:%10u / 장비 뷰 세팅중에 잘못된 장비 정보 발견. "
                "(PrototypeID:%6d/Pos:%4d/itemSize:%d)",
                GetCID(), lpItemData->m_usProtoTypeID, usIndex, lpItemData->m_cItemSize);
            
            break;
        }
    }
}

bool CCharacterData::GetFromDB(CDBComponent& DBComponent, unsigned long dwCID)
{
    PERFORMANCE_CHECK(FunctionTimingCheck);

    ClearData();

    using namespace DBComponent;

	bool bResult = false;

    FRIEND  Friend  = {0,};
    BAN     Ban     = {0,};

    // CharInfo 업데이트
    if (!GameDB::GetCharInfo(DBComponent, dwCID, &m_CharInfo, m_EquipView, MAX_EQUIP_VIEW))
    {
        SERLOG1(g_Log, "CID:%10u / 캐릭터 데이터 로드 실패 : GetCharInfo 호출 실패", dwCID);        
    }
    // CharPos 업데이트
    else if (!GameDB::GetCharPos(DBComponent, dwCID, &m_CharPos)) 
    {
        SERLOG1(g_Log, "CID:%10u / 캐릭터 데이터 로드 실패 : GetCharPos 호출 실패", dwCID);        
    }
    // Quick 업데이트
    else if (!GameDB::GetQuick(DBComponent, dwCID, &m_Quick)) 
    {
        SERLOG1(g_Log, "CID:%10u / 캐릭터 데이터 로드 실패 : GetQuick 호출 실패", dwCID);        
    }
	// Spell 업데이트
	else if (!GameDB::GetSpell(DBComponent, dwCID, &m_Spell)) 
	{
		SERLOG1(g_Log, "CID:%10u / 캐릭터 데이터 로드 실패 : GetSpell 호출 실패", dwCID);        
	}
    // Skill 업데이트
    else if (!GameDB::GetCharSkill(DBComponent, dwCID, &m_Skill)) 
    {
        SERLOG1(g_Log, "CID:%10u / 캐릭터 데이터 로드 실패 : GetCharSkill 호출 실패", dwCID);        
    }
    // Equip 업데이트
    else if (!GameDB::GetEquip(DBComponent, dwCID, &m_Equip)) 
    {
        SERLOG1(g_Log, "CID:%10u / 캐릭터 데이터 로드 실패 : GetEquip 호출 실패", dwCID);        
    }
    // Inven 업데이트
    else if (!GameDB::GetInven(DBComponent, dwCID, &m_Inven)) 
    {
        SERLOG1(g_Log, "CID:%10u / 캐릭터 데이터 로드 실패 : GetInven 호출 실패", dwCID);        
    }
    // Extra 업데이트
    else if (!GameDB::GetExtra(DBComponent, dwCID, &m_Extra)) 
    {
        SERLOG1(g_Log, "CID:%10u / 캐릭터 데이터 로드 실패 : GetExtra 호출 실패", dwCID);        
    }
    // Exchange 업데이트
    else if (!GameDB::GetExchange(DBComponent, dwCID, &m_Exchange)) 
    {
        SERLOG1(g_Log, "CID:%10u / 캐릭터 데이터 로드 실패 : GetExchange 호출 실패", dwCID);        
    }
    // TempInven 업데이트
    else if (!GameDB::GetTempInven(DBComponent, dwCID, &m_TempInven)) 
    {
        SERLOG1(g_Log, "CID:%10u / 캐릭터 데이터 로드 실패 : GetTempInven 호출 실패", dwCID);        
    }
    // CharInfoEx
    else if (!GameDB::GetCharInfoEx(DBComponent, dwCID, &m_CharInfoEx)) 
    {
        SERLOG1(g_Log, "CID:%10u / 캐릭터 데이터 로드 실패 : GetCharInfoEx 호출 실패", dwCID);        
    }
    // Friend
    else if (!GameDB::GetFriend(DBComponent, dwCID, &Friend)) 
    {
        SERLOG1(g_Log, "CID:%10u / 캐릭터 데이터 로드 실패 : GetFriend 호출 실패", dwCID);        
    } 
    else if(!m_FriendList.SerializeIn(Friend.Data, Friend.Info, Friend.dwSize, Friend.dwInfoSize))
    {
        SERLOG1(g_Log, "CID:%10u / 캐릭터 데이터 로드 실패 : CFriend::SerializeIn 호출 실패", dwCID);        
    }	
    // Ban
    else if(!GameDB::GetBan(DBComponent, dwCID, &Ban)) 
    {
        SERLOG1(g_Log, "CID:%10u / 캐릭터 데이터 로드 실패 : GetBan 호출 실패", dwCID);        
    } 
    else if(!m_BanList.SerializeIn(Ban.Data, Ban.Info, Ban.dwSize, Ban.dwInfoSize))
    {
        SERLOG1(g_Log, "CID:%10u / 캐릭터 데이터 로드 실패 : CBan::SerializeIn 호출 실패", dwCID);        
    }
    // Quest
    else if (!GameDB::GetQuest(DBComponent, dwCID, &m_Quest)) 
    {
        SERLOG1(g_Log, "CID:%10u / 캐릭터 데이터 로드 실패 : GetQuest 호출 실패", dwCID);
    }
    // History
    else if (!GameDB::GetHistory(DBComponent, dwCID, &m_History)) 
    {
        SERLOG1(g_Log, "CID:%10u / 캐릭터 데이터 로드 실패 : GetHistory 호출 실패", dwCID);
    }
    // Config
    else if (!GameDB::GetConfig(DBComponent, dwCID, &m_Config)) 
    {
        SERLOG1(g_Log, "CID:%10u / 캐릭터 데이터 로드 실패 : GetConfig 호출 실패", dwCID);
    }
    else
    {			
		// Friend 정보 설정 (시간이 생기면 수정하겠음) //
		unsigned long dwListCID[CFriendList::MAX_FRIENDS_NUM];

		// 친구리스트에 CID 만 가지고 온다. //
		m_FriendList.GetCIDList(dwListCID);

		for(unsigned char cIndex = 0; cIndex < m_FriendList.GetFriendNum(); cIndex++)
		{
			CSessionData* lpSessionData = CSessionDataMgr::GetInstance().GetCharLoadedSession(dwListCID[cIndex]);

			CFriendList::Rebind* lpRebind = m_FriendList.GetFriend(dwListCID[cIndex]);

			if(lpRebind)			
			{				
				if(lpSessionData)
				{
					CCharacterData*	lpCharacterData = lpSessionData->GetCharacterData();				

					if(lpCharacterData)
					{
						// Frind 초기화 //
						lpRebind->InitializeFriendInfo(lpCharacterData->GetServerID(), lpCharacterData->GetGID(),
							lpCharacterData->GetClass(), lpCharacterData->GetLevel());
					}
					else
					{
						lpRebind->UpdateServerID(0);
					}
				}
				else
				{
					lpRebind->UpdateServerID(0);
				}
			}
		}

		// Ban 정보 설정 (시간이 생기면 여기도 수정하겠음) //

		// 거부리스트에 CID 만 가지고 온다. //
		m_BanList.GetCIDList(dwListCID);

		for(unsigned char cIndex = 0; cIndex < m_BanList.GetBanNum(); cIndex++)
		{
			CSessionData* lpSessionData = CSessionDataMgr::GetInstance().GetCharLoadedSession(dwListCID[cIndex]);

			CBanList::Rebind* lpRebind = m_BanList.GetBan(dwListCID[cIndex]);

			if(lpRebind)			
			{								
				if(lpSessionData)
				{
					CCharacterData*	lpCharacterData = lpSessionData->GetCharacterData();

					if(lpCharacterData)
					{
						// Frind 초기화 //
						lpRebind->InitializeBanInfo(lpCharacterData->GetServerID(), lpCharacterData->GetGID(),
							lpCharacterData->GetClass(), lpCharacterData->GetLevel());
					}
					else
					{
						lpRebind->UpdateServerID(0);
					}
				}
				else
				{
					lpRebind->UpdateServerID(0);
				}
			}
		}

        INFLOG1(g_Log, "CID:%10u / 캐릭터 데이터 로드 성공 : CCharacterData::GetFromDB", dwCID);
        bResult = true;
    }

    return bResult;
}

bool CCharacterData::AddGold(unsigned long dwGold)
{
    m_CharInfo.Gold += dwGold;
	m_dwUpdateData |= CHANGED_CHAR_INFOST;
	return true;
}

bool CCharacterData::DeductGold(unsigned long dwGold)
{
    if (dwGold <= m_CharInfo.Gold)
    { 
        m_CharInfo.Gold -= dwGold;
		m_dwUpdateData |= CHANGED_CHAR_INFOST;
		return true;
    }
	return false;
}

bool CCharacterData::UpdateDBData(CDBComponent& DBComponent)
{
    PERFORMANCE_CHECK(FunctionTimingCheck);

    unsigned long dwCID = GetCID();

    // 무결성 검사
    if (!IsValidData())
    {
        SERLOG1(g_Log, "CID:%10u / UpdateToDB 에서 무결성 검사 실패.", dwCID);
        return false;
    }
    else if(IsDataChanged())
    {
        using namespace DBComponent;

//		INFLOG2(g_Log, "CID:%10u / 캐릭터 데이터 업데이트 준비 : CCharacterData::UpdateToDB, %d 부분 업데이트 시작", dwCID, m_dwUpdateData);

        FRIEND  Friend  = {0,};
        BAN     Ban     = {0,};

        Friend.dwSize		= FRIEND::MAX_FRIEND_SIZE;
		Friend.dwInfoSize	= FRIEND::MAX_FRIENDINFO_SIZE;
        Ban.dwSize			= BAN::MAX_BAN_SIZE;
		Ban.dwInfoSize		= BAN::MAX_BANINFO_SIZE;

        // 장비 뷰 다시 로드
        ReloadEquipView();

/*
		unsigned long       dwUpdateData = 0;

        // CharInfo 업데이트
        if((m_dwUpdateData & CHANGED_CHAR_INFOST) && 
            !GameDB::UpdateCharInfo(DBComponent, dwCID, &m_CharInfo, m_EquipView, MAX_EQUIP_VIEW))
        {
			dwUpdateData |= CHANGED_CHAR_INFOST;
            SERLOG1(g_Log, "CID:%10u / 캐릭터 데이터 업데이트 실패 : UpdateCharInfo 호출 실패", dwCID);                
        }
        // CharPos 업데이트
        if((m_dwUpdateData & CHANGED_CHAR_POS) && 
            !GameDB::UpdateCharPos(DBComponent, dwCID, &m_CharPos)) 
        {
			dwUpdateData |= CHANGED_CHAR_POS;
            SERLOG1(g_Log, "CID:%10u / 캐릭터 데이터 업데이트 실패 : UpdateCharPos 호출 실패", dwCID);
        }        
        // Quick 업데이트
        if((m_dwUpdateData & CHANGED_QUICK) &&
            !GameDB::UpdateQuick(DBComponent, dwCID, &m_Quick))
        {
			dwUpdateData |= CHANGED_QUICK;
            SERLOG1(g_Log, "CID:%10u / 캐릭터 데이터 업데이트 실패 : UpdateCharInfo 호출 실패", dwCID);
        }
		// Spell 업데이트
		if((m_dwUpdateData & CHANGED_SPELL) &&
			!GameDB::UpdateSpell(DBComponent, dwCID, &m_Spell))
		{
			dwUpdateData |= CHANGED_SPELL;
			SERLOG1(g_Log, "CID:%10u / 캐릭터 데이터 업데이트 실패 : UpdateSpellInfo 호출 실패", dwCID);
		}
        // Skill 업데이트
        if((m_dwUpdateData & CHANGED_SKILL) && 
            !GameDB::UpdateCharSkill(DBComponent, dwCID, &m_Skill)) 
        {
			dwUpdateData |= CHANGED_SKILL;
            SERLOG1(g_Log, "CID:%10u / 캐릭터 데이터 업데이트 실패 : UpdateCharSkill 호출 실패", dwCID);            
        }
        // Equip 업데이트
        if((m_dwUpdateData & CHANGED_EQUIP) && 
            !GameDB::UpdateEquip(DBComponent, dwCID, &m_Equip)) 
        {
			dwUpdateData |= CHANGED_EQUIP;
            SERLOG1(g_Log, "CID:%10u / 캐릭터 데이터 업데이트 실패 : UpdateEquip 호출 실패", dwCID);
        }
        // Inven 업데이트
        if((m_dwUpdateData & CHANGED_INVEN) && 
            !GameDB::UpdateInven(DBComponent, dwCID, &m_Inven)) 
        {
			dwUpdateData |= CHANGED_INVEN;
            SERLOG1(g_Log, "CID:%10u / 캐릭터 데이터 업데이트 실패 : UpdateInven 호출 실패", dwCID);            
        }
        // Extra 업데이트
        if((m_dwUpdateData & CHANGED_EXTRA) && 
            !GameDB::UpdateExtra(DBComponent, dwCID, &m_Extra))
        {
			dwUpdateData |= CHANGED_EXTRA;
            SERLOG1(g_Log, "CID:%10u / 캐릭터 데이터 업데이트 실패 : UpdateExtra 호출 실패", dwCID);            
        }                
        // Exchange 업데이트
        if((m_dwUpdateData & CHANGED_EXCHANGE) && 
            !GameDB::UpdateExchange(DBComponent, dwCID, &m_Exchange)) 
        {
			dwUpdateData |= CHANGED_EXCHANGE;
            SERLOG1(g_Log, "CID:%10u / 캐릭터 데이터 업데이트 실패 : UpdateExchange 호출 실패", dwCID);            
        }
		// TempInven 업데이트
		if((m_dwUpdateData & CHANGED_TEMPINVEN) && 
			!GameDB::UpdateTempInven(DBComponent, dwCID, &m_TempInven)) 
		{
			dwUpdateData |= CHANGED_TEMPINVEN;
			SERLOG1(g_Log, "CID:%10u / 캐릭터 데이터 업데이트 실패 : UpdateTempInven 호출 실패", dwCID);            
		}
        // CharInfoEx
        if((m_dwUpdateData & CHANGED_CHAR_INFOEX) &&
            !GameDB::UpdateCharInfoEx(DBComponent, dwCID, &m_CharInfoEx)) 
        {
			dwUpdateData |= CHANGED_CHAR_INFOEX;
            SERLOG1(g_Log, "CID:%10u / 캐릭터 데이터 업데이트 실패 : UpdateCharInfoEx 호출 실패", dwCID);
        }               
        // Quest
        if((m_dwUpdateData & CHANGED_QUEST) &&
            !GameDB::UpdateQuest(DBComponent, dwCID, &m_Quest)) 
        {
			dwUpdateData |= CHANGED_QUEST;
            SERLOG1(g_Log, "CID:%10u / 캐릭터 데이터 업데이트 실패 : UpdateQuest 호출 실패", dwCID);            
        }
        // History
        if((m_dwUpdateData & CHANGED_HISTORY) && 
            !GameDB::UpdateHistory(DBComponent, dwCID, &m_History)) 
        {
			dwUpdateData |= CHANGED_HISTORY;
            SERLOG1(g_Log, "CID:%10u / 캐릭터 데이터 업데이트 실패 : UpdateHistory 호출 실패", dwCID);            
        }
        // Config
        if((m_dwUpdateData & CHANGED_CONFIG) &&
            !GameDB::UpdateConfig(DBComponent, dwCID, &m_Config)) 
        {
			dwUpdateData |= CHANGED_CONFIG;
            SERLOG1(g_Log, "CID:%10u / 캐릭터 데이터 업데이트 실패 : UpdateConfig 호출 실패", dwCID);
        }
        // Friend
        if(m_dwUpdateData & CHANGED_FRIENDLIST)
        {
            if(!m_FriendList.SerializeOut(Friend.Data, Friend.Info, Friend.dwSize, Friend.dwInfoSize))
            {
				dwUpdateData |= CHANGED_FRIENDLIST;
                SERLOG1(g_Log, "CID:%10u / 캐릭터 데이터 업데이트 실패 : CFriend::SerializeOut 호출 실패", dwCID);
            }        
            else if(!GameDB::UpdateFriend(DBComponent, dwCID, &Friend)) 
            {
				dwUpdateData |= CHANGED_FRIENDLIST;
                SERLOG1(g_Log, "CID:%10u / 캐릭터 데이터 업데이트 실패 : UpdateFriend 호출 실패", dwCID);
            }
        }        
        
        if(m_dwUpdateData & CHANGED_BANLIST)
        {
            // Ban
            if(!m_BanList.SerializeOut(Ban.Data, Ban.Info, Ban.dwSize, Ban.dwInfoSize))
            {
				dwUpdateData |= CHANGED_BANLIST;
                SERLOG1(g_Log, "CID:%10u / 캐릭터 데이터 업데이트 실패 : CBan::SerializeOut 호출 실패", dwCID);
                return false;
            }
            else if(!GameDB::UpdateBan(DBComponent, dwCID, &Ban)) 
            {
				dwUpdateData |= CHANGED_BANLIST;
                SERLOG1(g_Log, "CID:%10u / 캐릭터 데이터 업데이트 실패 : UpdateBan 호출 실패", dwCID);
                return false;
            }
        }

        // 데이터 업데이트 플래그 리셋.
		if(dwUpdateData == 0)
		{
			INFLOG2(g_Log, "CID:%10u / 캐릭터 데이터 업데이트 성공 : CCharacterData::UpdateToDB, %d 부분 업데이트 성공", dwCID, m_dwUpdateData);
			m_dwUpdateData = 0;
		}
		else
		{
//			SERLOG2(g_Log, "CID:%10u / 캐릭터 데이터 업데이트 실패 : %d 부분 실패", dwCID, m_dwUpdateData);
//			m_dwUpdateData = 0;
			m_dwUpdateData = dwUpdateData;
			SERLOG2(g_Log, "CID:%10u / 캐릭터 데이터 업데이트 실패 : %d 부분 재도전", dwCID, m_dwUpdateData);
		}
*/
        // CharInfo 업데이트
        if((m_dwUpdateData & CHANGED_CHAR_INFOST) && 
            !GameDB::UpdateCharInfo(DBComponent, dwCID, &m_CharInfo, m_EquipView, MAX_EQUIP_VIEW))
        {
            SERLOG1(g_Log, "CID:%10u / 캐릭터 데이터 업데이트 실패 : UpdateCharInfo 호출 실패", dwCID);                
        }
        // CharPos 업데이트
        else if((m_dwUpdateData & CHANGED_CHAR_POS) && 
            !GameDB::UpdateCharPos(DBComponent, dwCID, &m_CharPos)) 
        {
            SERLOG1(g_Log, "CID:%10u / 캐릭터 데이터 업데이트 실패 : UpdateCharPos 호출 실패", dwCID);
        }        
        // Quick 업데이트
        else if((m_dwUpdateData & CHANGED_QUICK) &&
            !GameDB::UpdateQuick(DBComponent, dwCID, &m_Quick))
        {
            SERLOG1(g_Log, "CID:%10u / 캐릭터 데이터 업데이트 실패 : UpdateCharInfo 호출 실패", dwCID);
        }
		// Spell 업데이트
		else if((m_dwUpdateData & CHANGED_SPELL) &&
			!GameDB::UpdateSpell(DBComponent, dwCID, &m_Spell))
		{
			SERLOG1(g_Log, "CID:%10u / 캐릭터 데이터 업데이트 실패 : UpdateSpellInfo 호출 실패", dwCID);
		}
        // Skill 업데이트
        else if((m_dwUpdateData & CHANGED_SKILL) && 
            !GameDB::UpdateCharSkill(DBComponent, dwCID, &m_Skill)) 
        {
            SERLOG1(g_Log, "CID:%10u / 캐릭터 데이터 업데이트 실패 : UpdateCharSkill 호출 실패", dwCID);            
        }
        // Equip 업데이트
        else if((m_dwUpdateData & CHANGED_EQUIP) && 
            !GameDB::UpdateEquip(DBComponent, dwCID, &m_Equip)) 
        {
            SERLOG1(g_Log, "CID:%10u / 캐릭터 데이터 업데이트 실패 : UpdateEquip 호출 실패", dwCID);
        }
        // Inven 업데이트
        else if((m_dwUpdateData & CHANGED_INVEN) && 
            !GameDB::UpdateInven(DBComponent, dwCID, &m_Inven)) 
        {
            SERLOG1(g_Log, "CID:%10u / 캐릭터 데이터 업데이트 실패 : UpdateInven 호출 실패", dwCID);            
        }
        // Extra 업데이트
        else if((m_dwUpdateData & CHANGED_EXTRA) && 
            !GameDB::UpdateExtra(DBComponent, dwCID, &m_Extra))
        {
            SERLOG1(g_Log, "CID:%10u / 캐릭터 데이터 업데이트 실패 : UpdateExtra 호출 실패", dwCID);            
        }                
        // Exchange 업데이트
        else if((m_dwUpdateData & CHANGED_EXCHANGE) && 
            !GameDB::UpdateExchange(DBComponent, dwCID, &m_Exchange)) 
        {
            SERLOG1(g_Log, "CID:%10u / 캐릭터 데이터 업데이트 실패 : UpdateExchange 호출 실패", dwCID);            
        }
		// TempInven 업데이트
		else if((m_dwUpdateData & CHANGED_TEMPINVEN) && 
			!GameDB::UpdateTempInven(DBComponent, dwCID, &m_TempInven)) 
		{
			SERLOG1(g_Log, "CID:%10u / 캐릭터 데이터 업데이트 실패 : UpdateTempInven 호출 실패", dwCID);            
		}
        // CharInfoEx
        else if((m_dwUpdateData & CHANGED_CHAR_INFOEX) &&
            !GameDB::UpdateCharInfoEx(DBComponent, dwCID, &m_CharInfoEx)) 
        {
            SERLOG1(g_Log, "CID:%10u / 캐릭터 데이터 업데이트 실패 : UpdateCharInfoEx 호출 실패", dwCID);
        }               
        else 
        {
            // Friend
            if(m_dwUpdateData & CHANGED_FRIENDLIST)
            {
                if(!m_FriendList.SerializeOut(Friend.Data, Friend.Info, Friend.dwSize, Friend.dwInfoSize))
                {
                    SERLOG1(g_Log, "CID:%10u / 캐릭터 데이터 업데이트 실패 : CFriend::SerializeOut 호출 실패", dwCID);
                    return false;
                }        
                else if(!GameDB::UpdateFriend(DBComponent, dwCID, &Friend)) 
                {
                    SERLOG1(g_Log, "CID:%10u / 캐릭터 데이터 업데이트 실패 : UpdateFriend 호출 실패", dwCID);
                    return false;
                }
            }        
            
            if(m_dwUpdateData & CHANGED_BANLIST)
            {
                // Ban
                if(!m_BanList.SerializeOut(Ban.Data, Ban.Info, Ban.dwSize, Ban.dwInfoSize))
                {
                    SERLOG1(g_Log, "CID:%10u / 캐릭터 데이터 업데이트 실패 : CBan::SerializeOut 호출 실패", dwCID);
                    return false;
                }
                else if(!GameDB::UpdateBan(DBComponent, dwCID, &Ban)) 
                {
                    SERLOG1(g_Log, "CID:%10u / 캐릭터 데이터 업데이트 실패 : UpdateBan 호출 실패", dwCID);
                    return false;
                }
            }
        
            // Quest
            if((m_dwUpdateData & CHANGED_QUEST) &&
                !GameDB::UpdateQuest(DBComponent, dwCID, &m_Quest)) 
            {
                SERLOG1(g_Log, "CID:%10u / 캐릭터 데이터 업데이트 실패 : UpdateQuest 호출 실패", dwCID);            
            }
            // History
            else if((m_dwUpdateData & CHANGED_HISTORY) && 
                !GameDB::UpdateHistory(DBComponent, dwCID, &m_History)) 
            {
                SERLOG1(g_Log, "CID:%10u / 캐릭터 데이터 업데이트 실패 : UpdateHistory 호출 실패", dwCID);            
            }
            // Config
            else if((m_dwUpdateData & CHANGED_CONFIG) &&
                !GameDB::UpdateConfig(DBComponent, dwCID, &m_Config)) 
            {
                SERLOG1(g_Log, "CID:%10u / 캐릭터 데이터 업데이트 실패 : UpdateConfig 호출 실패", dwCID);
            }
            else
            {
                // 데이터 업데이트 플래그 리셋.
                m_dwUpdateData = 0;
                INFLOG1(g_Log, "CID:%10u / 캐릭터 데이터 업데이트 성공 : CCharacterData::UpdateToDB", dwCID);
                return true;
            }
        }

        // 데이터 업데이트 실패
        return false;
    }
    else
    {
        INFLOG1(g_Log, "CID:%10u / 캐릭터 데이터 업데이트 성공 : 변경된 데이터가 없어서 업데이트하지 않았습니다.", dwCID);
    }

    m_dwLastDBUpdateTime = timeGetTime();
    return true;
}


inline void CopyAndAdvanceDst(char*& lpWritePos, const void* lpData, unsigned long dwSize, 
                              unsigned short* lpSizeArray_Out, DBUpdateData::UpdateList eUpdatePos)
{
    memcpy(lpWritePos, lpData, dwSize);
    lpWritePos += dwSize;
    lpSizeArray_Out[eUpdatePos] = static_cast<unsigned short>(dwSize);
}

bool CCharacterData::SerializeOut(char* lpBuffer_Out, unsigned short* lpSizeArray_Out, 
                                unsigned long &dwBufferSize_InOut, unsigned long dwSizeArrayNum)
{
    // 버퍼 바운딩 체크 및 SizeArray체크
    
    if(0 == lpBuffer_Out || 0 == lpSizeArray_Out)
    {
        SERLOG3(g_Log, "CID:%10u / 캐릭터 데이터 버퍼에 쓰기 실패 : 입력 데이터가 이상합니다."
            "lpBuffer_Out:%p / lpSizeArray_Out:%p", GetCID(), lpBuffer_Out, lpSizeArray_Out);

        return false;
    }

    unsigned long dwEstimateSize = 
        sizeof(CHAR_INFOST) + sizeof(CHAR_POS) + sizeof(SKILL) + sizeof(QUICK) + sizeof(SPELL) +
        m_Equip.dwSize + m_Inven.dwSize + m_Extra.dwSize + m_Exchange.dwSize + m_TempInven.dwSize;

    if(dwBufferSize_InOut < dwEstimateSize)
    {
        SERLOG3(g_Log, "CID:%10u / 캐릭터 데이터 버퍼에 쓰기 실패 : 버퍼 크기가 부족합니다."
            "필요 크기:%10u / 받은 크기:%10u", GetCID(), dwEstimateSize, dwBufferSize_InOut);

        return false;
    }

    if(dwSizeArrayNum < DBUpdateData::MAX_UPDATE_DB)
    {
        SERLOG3(g_Log, "CID:%10u / 캐릭터 데이터 버퍼에 쓰기 실패 : SizeArray 크기가 부족합니다."
            "필요 개수:%10u / 받은 개수:%10u", GetCID(), DBUpdateData::MAX_UPDATE_DB, dwSizeArrayNum);

        return false;
    }

    char* lpWritePos = lpBuffer_Out;

    CopyAndAdvanceDst(lpWritePos, &m_CharInfo, sizeof(CHAR_INFOST), lpSizeArray_Out, DBUpdateData::STATUS_UPDATE);
    CopyAndAdvanceDst(lpWritePos, &m_CharPos, sizeof(CHAR_POS), lpSizeArray_Out, DBUpdateData::POSITION_UPDATE);
    CopyAndAdvanceDst(lpWritePos, &m_Skill, sizeof(SKILL), lpSizeArray_Out, DBUpdateData::SKILL_UPDATE);
    CopyAndAdvanceDst(lpWritePos, &m_Quick, sizeof(QUICK), lpSizeArray_Out, DBUpdateData::QUICKSLOT_UPDATE);
	CopyAndAdvanceDst(lpWritePos, &m_Spell, sizeof(SPELL), lpSizeArray_Out, DBUpdateData::SPELL_UPDATE);
    CopyAndAdvanceDst(lpWritePos, m_Equip.Data, m_Equip.dwSize, lpSizeArray_Out, DBUpdateData::ITEM_EQUIP_UPDATE);
    CopyAndAdvanceDst(lpWritePos, m_Inven.Data, m_Inven.dwSize, lpSizeArray_Out, DBUpdateData::ITEM_INVEN_UPDATE);
    CopyAndAdvanceDst(lpWritePos, m_Extra.Data, m_Extra.dwSize, lpSizeArray_Out, DBUpdateData::ITEM_EXTRA_UPDATE);
    CopyAndAdvanceDst(lpWritePos, m_Exchange.Data, m_Exchange.dwSize, lpSizeArray_Out, DBUpdateData::ITEM_EXCHANGE_UPDATE);
    CopyAndAdvanceDst(lpWritePos, m_TempInven.Data, m_TempInven.dwSize, lpSizeArray_Out, DBUpdateData::ITEM_TEMPINVEN_UPDATE);


    dwBufferSize_InOut = static_cast<unsigned long>(lpWritePos - lpBuffer_Out);
    return true;
}




bool CCharacterData::SerializeIn(const char* lpBuffer_In, const unsigned short* lpSizeArray_In, 
                                 unsigned long dwBufferSize, unsigned long dwSizeArrayNum)
{
    unsigned long dwCID = GetCID();

    // 데이터길이 체크 및 버퍼길이 체크
    if(0 == lpBuffer_In || 0 == lpSizeArray_In || dwSizeArrayNum < DBUpdateData::MAX_UPDATE_DB)
    {
        SERLOG4(g_Log, "CID:%10u / 버퍼 데이터를 캐릭터에 쓰기 실패 : 입력 데이터가 이상합니다."
            "lpBuffer_Out:%p / lpSizeArray_Out:%p / dwSizeArrayNum : %d",
            dwCID, lpBuffer_In, lpSizeArray_In, dwSizeArrayNum);

        return false;
    }

    if (sizeof(CHAR_INFOST) != lpSizeArray_In[DBUpdateData::STATUS_UPDATE] || 
        sizeof(CHAR_POS) != lpSizeArray_In[DBUpdateData::POSITION_UPDATE] || 
        sizeof(SKILL) != lpSizeArray_In[DBUpdateData::SKILL_UPDATE] || 
        sizeof(QUICK) != lpSizeArray_In[DBUpdateData::QUICKSLOT_UPDATE] ||
		sizeof(SPELL) != lpSizeArray_In[DBUpdateData::SPELL_UPDATE])
    {
//        SERLOG10(g_Log, "CID:%10u / 버퍼 데이터를 캐릭터에 쓰기 실패 : 입력 데이터가 이상합니다. sizeof(CHAR_INFOST) : %d/%d / sizeof(CHAR_POS) : %d/%d / sizeof(SKILL) : %d/%d / sizeof(QUICK) : %d/%d / sizeof(SPELL) : %d/%d",
        SERLOG10(g_Log, "CID:%10u / 버퍼 데이터를 캐릭터에 쓰기 실패 : 입력 데이터가 이상합니다. sizeof(CHAR_INFOST) : %d/%d / sizeof(CHAR_POS) : %d/%d / sizeof(SKILL) : %d/%d / sizeof(QUICK) : %d/%d / sizeof(SPELL) : %d",
            dwCID, 
			sizeof(CHAR_INFOST), lpSizeArray_In[DBUpdateData::STATUS_UPDATE],
            sizeof(CHAR_POS), lpSizeArray_In[DBUpdateData::POSITION_UPDATE],
            sizeof(SKILL), lpSizeArray_In[DBUpdateData::SKILL_UPDATE],
            sizeof(QUICK), lpSizeArray_In[DBUpdateData::QUICKSLOT_UPDATE],
			lpSizeArray_In[DBUpdateData::SPELL_UPDATE]);
//			sizeof(SPELL), lpSizeArray_In[DBUpdateData::SPELL_UPDATE]);

        return false;
    }

    const char* lpReadPos = lpBuffer_In; 

    // 업데이트시 CID다르면 즐!
    if(!SetInfo(*reinterpret_cast<const CHAR_INFOST*>(lpReadPos)))
    {
        return false;
    }

    lpReadPos += sizeof(CHAR_INFOST);

    SetPos(*reinterpret_cast<const CHAR_POS*>(lpReadPos));
    lpReadPos += sizeof(CHAR_POS);

    SetSkill(*reinterpret_cast<const SKILL*>(lpReadPos));
    lpReadPos += sizeof(SKILL);

    SetQuick(*reinterpret_cast<const QUICK*>(lpReadPos));
    lpReadPos += sizeof(QUICK);

	//! 스펠 데이터 세팅
	SetSpell(*reinterpret_cast<const SPELL*>(lpReadPos));
	lpReadPos += sizeof(SPELL);


    //! 장비 데이터 세팅
    if(!SetEquip(lpReadPos, lpSizeArray_In[DBUpdateData::ITEM_EQUIP_UPDATE]))
    {
        SERLOG1(g_Log, "CID:%10u / 버퍼 데이터를 캐릭터에 쓰기 실패 : 장비 세팅 실패.", dwCID);
        return false;
    }
    lpReadPos += lpSizeArray_In[DBUpdateData::ITEM_EQUIP_UPDATE];
    
    //! 인벤토리 데이터 세팅
    if(!SetInven(lpReadPos, lpSizeArray_In[DBUpdateData::ITEM_INVEN_UPDATE]))
    {
        SERLOG1(g_Log, "CID:%10u / 버퍼 데이터를 캐릭터에 쓰기 실패 : 인벤 세팅 실패.", dwCID);
        return false;
    }
    lpReadPos += lpSizeArray_In[DBUpdateData::ITEM_INVEN_UPDATE];

    //! 추가 데이터 세팅
    if(!SetExtra(lpReadPos, lpSizeArray_In[DBUpdateData::ITEM_EXTRA_UPDATE]))
    {
        SERLOG1(g_Log, "CID:%10u / 버퍼 데이터를 캐릭터에 쓰기 실패 : Extra 세팅 실패.", dwCID);
        return false;
    }
    lpReadPos += lpSizeArray_In[DBUpdateData::ITEM_EXTRA_UPDATE];

    //! 교환창 데이터 세팅
    if(!SetExchange(lpReadPos, lpSizeArray_In[DBUpdateData::ITEM_EXCHANGE_UPDATE]))
    {
        SERLOG1(g_Log, "CID:%10u / 버퍼 데이터를 캐릭터에 쓰기 실패 : Exchange 세팅 실패.", dwCID);
        return false;
    }
    lpReadPos += lpSizeArray_In[DBUpdateData::ITEM_EXCHANGE_UPDATE];

    //! 임시 인벤토리 데이터 세팅
    if(!SetTempInven(lpReadPos, lpSizeArray_In[DBUpdateData::ITEM_TEMPINVEN_UPDATE]))
    {
        SERLOG1(g_Log, "CID:%10u / 버퍼 데이터를 캐릭터에 쓰기 실패 : 임시인벤 세팅 실패.", dwCID);
        return false;
    }
    lpReadPos += lpSizeArray_In[DBUpdateData::ITEM_TEMPINVEN_UPDATE];


    return true;
}

bool CCharacterData::AddFriend(unsigned long dwFriendCID, const char* szCharacterName, unsigned long dwGID, unsigned short wClass, char cLevel, unsigned long dwServerID)
{
    if(m_FriendList.Add(dwFriendCID, szCharacterName, dwGID, wClass, cLevel, dwServerID))
    {
        m_dwUpdateData |= CHANGED_FRIENDLIST;
        return true;
    }

    return false;
}

bool CCharacterData::RemoveFriend(unsigned long dwFriendCID)
{
    if(m_FriendList.Remove(dwFriendCID))
    {
        m_dwUpdateData |= CHANGED_FRIENDLIST;
        return true;
    }

    return false;
}

bool CCharacterData::AddBan(unsigned long dwBanCID, const char* szBanName, unsigned long dwGID, unsigned short wClass, char cLevel, unsigned long dwServerID)
{
    if(m_BanList.Add(dwBanCID, szBanName, dwGID, wClass, cLevel, dwServerID))
    {
        m_dwUpdateData |= CHANGED_BANLIST;
        return true;
    }

    return false;
}

bool CCharacterData::RemoveBan(unsigned long dwBanCID)
{
    if(m_BanList.Remove(dwBanCID))
    {
        m_dwUpdateData |= CHANGED_BANLIST;
        return true;
    }

    return false;
}


bool CCharacterData::SetFriendGroup(unsigned long dwCID, unsigned long dwGroup)
{
    CFriendList::Rebind* lpRebind = m_FriendList.GetFriend(dwCID);
    if (0 != lpRebind && lpRebind->SetGroup(dwGroup))
    {
        m_dwUpdateData |= CHANGED_FRIENDLIST;
        return true;
    }

    return false;
}

bool CCharacterData::FriendSerializeIn(const char* lpData, unsigned long dwDataLen)
{
    if(m_FriendList.SerializeIn(lpData, dwDataLen))
    {
        m_dwUpdateData |= CHANGED_FRIENDLIST;
        return true;
    }
    
    return false;
}

bool CCharacterData::BanSerializeIn(const char* lpData, unsigned long dwDataLen)
{
    if(m_BanList.SerializeIn(lpData, dwDataLen))
    {
        m_dwUpdateData |= CHANGED_BANLIST;
        return true;
    }

    return false;
}


//! 가지고 있는 데이터를 파일로 덤프를 남기거나, 파일에서 데이터를 불러 온다.
bool CCharacterData::WriteDataToDumpFile(const char* szFileName)
{

    return false;
}

bool CCharacterData::ReadDataFromDumpFile(const char* szFileName)
{

    return false;
}


bool CCharacterData::ConvertToPart2Item(bool bConvertSkillToTicket)
{
    RebalanceLib::CItemArrayChecker<
        ContainerConstant::INVENTORY_WIDTH,
        ContainerConstant::INVENTORY_HEIGHT,
        ContainerConstant::MAX_INVENTORY_TAB>   invenChecker(TakeType::TS_INVEN);

    RebalanceLib::CItemListChecker<Item::MAX_TEMP_INVEN_ITEM_NUM>   
        tempInvenChecker(TakeType::TS_TEMPINVEN);

    RebalanceLib::SkillBookInfo skillBookInfo;

    EQUIP		Equip;		// 장비
    INVEN		Inven;		// 인벤
    EXTRA		Extra;		// 여분
    EXCHANGE	Exchange;	// 교환창
    TEMPINVEN	TempInven;	// 임시 인벤토리

    unsigned long dwGold = m_CharInfo.Gold;

	// 캐릭터 이전.
	m_CharInfo.Chance = 3;

    CDBAgentItemSerialMgr& dbItemSerialMgr = 
        CDBAgentItemSerialMgr::GetInstance();

    unsigned __int64 dwItemSerial = dbItemSerialMgr.GetItemSerial();

    memset(&skillBookInfo, 0, sizeof(RebalanceLib::SkillBookInfo));
    memset(&Equip, 0, sizeof(EQUIP));
    memset(&Inven, 0, sizeof(INVEN));
    memset(&Extra, 0, sizeof(EXTRA));
    memset(&Exchange, 0, sizeof(EXCHANGE));
    memset(&TempInven, 0, sizeof(TEMPINVEN));

    Equip.dwSize        = EQUIP::MAX_EQUIP_SIZE;
    Inven.dwSize        = INVEN::MAX_INVEN_SIZE;
    Extra.dwSize        = EXTRA::MAX_EXTRA_SIZE;
    Exchange.dwSize     = EXCHANGE::MAX_EXCHANGE_SIZE;
    TempInven.dwSize    = TEMPINVEN::MAX_TEMPINVEN_SIZE;

    // 스킬 더하기
    int nMaxSlotNum = std::min(m_Skill.wSlotNum, unsigned short(SKILL::MAX_SLOT_NUM));
    for(int nCount = 0; nCount < nMaxSlotNum; ++nCount)
    {
        SKILLSLOT& skillSlot = m_Skill.SSlot[nCount];

        if(skillSlot.SKILLINFO.cLockCount < CSkillMgr::MAX_SKILL_LOCKCOUNT)
        {
            for(int nLockCount = 0; nLockCount < skillSlot.SKILLINFO.cLockCount; ++nLockCount)
            {
                skillBookInfo.m_dwBookNum[nLockCount] += CSkillMgr::MAX_SKILL_LEVEL;
            }

            skillBookInfo.m_dwBookNum[skillSlot.SKILLINFO.cLockCount] += 
                skillSlot.SKILLINFO.cSkillLevel;
        }
    }

    // 아이템 컨버팅
    if(!RebalanceLib::ConvertContainer(0, m_CharInfo.CID, 0, 0, 
        m_Equip.Data, m_Equip.dwSize, Equip.Data, Equip.dwSize))
    {
        ERRLOG1(g_Log, "CID:%10u / EQUIP 컨버팅에 실패했습니다", m_CharInfo.CID);
    }
    else if(!RebalanceLib::ConvertContainer(0, m_CharInfo.CID, &invenChecker, 
        bConvertSkillToTicket ? &skillBookInfo : 0, 
        m_Inven.Data, m_Inven.dwSize, Inven.Data, Inven.dwSize))
    {
        ERRLOG1(g_Log, "CID:%10u / INVEN 컨버팅에 실패했습니다", m_CharInfo.CID);        
    }
    else if(!RebalanceLib::ConvertContainer(0, m_CharInfo.CID, 0, 
        bConvertSkillToTicket ? &skillBookInfo : 0,  
        m_Extra.Data, m_Extra.dwSize, Extra.Data, Extra.dwSize))
    {
        ERRLOG1(g_Log, "CID:%10u / EXTRA 컨버팅에 실패했습니다", m_CharInfo.CID);        
    }
    else if(!RebalanceLib::ConvertContainer(0, m_CharInfo.CID, 0, 
        bConvertSkillToTicket ? &skillBookInfo : 0, 
        m_Exchange.Data, m_Exchange.dwSize, Exchange.Data, Exchange.dwSize))
    {
        ERRLOG1(g_Log, "CID:%10u / EXCHANGE 컨버팅에 실패했습니다", m_CharInfo.CID);        
    }
    else if(!RebalanceLib::ConvertContainer(0, m_CharInfo.CID, &tempInvenChecker, 
        bConvertSkillToTicket ? &skillBookInfo : 0, 
        m_TempInven.Data, m_TempInven.dwSize, TempInven.Data, TempInven.dwSize))
    {
        ERRLOG1(g_Log, "CID:%10u / TEMPINVEN 컨버팅에 실패했습니다", m_CharInfo.CID);        
    }
    else 
    {
        for(int nCount = 0; nCount < CSkillMgr::MAX_SKILL_LOCKCOUNT; ++nCount)
        {
            // 3배로 보상한다.
            skillBookInfo.m_dwBookNum[nCount] *= 3;
        }

        if (bConvertSkillToTicket && !RebalanceLib::AddSkillBook(
            0, m_CharInfo.CID, "Inventory", dbItemSerialMgr, dwGold,
            Inven.Data, Inven.dwSize, INVEN::MAX_INVEN_SIZE, invenChecker, skillBookInfo))
        {
            ERRLOG1(g_Log, "CID:%10u / 인벤토리 스킬북 넣어주기에 실패했습니다", m_CharInfo.CID);        
        }
        else if (bConvertSkillToTicket && !skillBookInfo.IsEmpty() && 
            !RebalanceLib::AddSkillBook(0, m_CharInfo.CID, "TempInven", dbItemSerialMgr, dwGold, 
            TempInven.Data, TempInven.dwSize, TEMPINVEN::MAX_TEMPINVEN_SIZE, tempInvenChecker, skillBookInfo))
        {
            ERRLOG1(g_Log, "CID:%10u / 임시인벤 스킬북 넣어주기에 실패했습니다", m_CharInfo.CID);        
        }
        else
        {
            if(dwItemSerial < dbItemSerialMgr.GetItemSerial())
            {
                dbItemSerialMgr.SaveItemSerial(
                    CDBSingleObject::GetInstance(), dbItemSerialMgr.GetServerID());
            }

            m_CharInfo.Gold = dwGold;

            unsigned char cLevelIndex = (0 < m_CharInfo.Level) ? m_CharInfo.Level - 1 : 0;

            m_CharInfo.Exp = static_cast<unsigned __int64>(
                m_CharInfo.Exp * EXP::ExpConvertTable[cLevelIndex]);

            memset(&m_Skill, 0, sizeof(SKILL));
            memset(&m_Quick, 0, sizeof(QUICK));
            memset(&m_Quest, 0, sizeof(QUEST));         // 컨버팅할때 퀘스트/히스토리 전부 날린다.
            memset(&m_History, 0, sizeof(HISTORY));     // 

            unsigned short usHumanCount = 0;
            unsigned short usAkhanCount = 0;
            unsigned char  cChannelCount = 0;
            
            CGameDispatch::GetDispatchTable().Process(
                CGetTotalCount(usHumanCount, usAkhanCount, cChannelCount));

            // 캐릭터 좌표를 세팅한다.
            m_CharPos.LastPoint = CharCreate::GetDefaultCharacterPos(m_CharInfo.Race, 
                0 == m_CharInfo.Race ? usHumanCount : usAkhanCount);

            memset(&m_CharPos.SavePoint, 0, sizeof(POS));

            // 선택 서버군을 0으로 놓는다. 자동으로 신의 대지로 이동할것임.
            m_CharInfoEx.ServerID = 0;

            m_Equip     = Equip;		// 장비
            m_Inven     = Inven;		// 인벤
            m_Extra     = Extra;		// 여분
            m_Exchange  = Exchange;	    // 교환창
            m_TempInven = TempInven;	// 임시 인벤토리
        }

        if(!skillBookInfo.IsEmpty())
        {
            for(int nCount = 0; nCount < CSkillMgr::MAX_SKILL_LOCKCOUNT; ++nCount)
            {
                if(0 != skillBookInfo.m_dwBookNum[nCount])
                {
                    ERRLOG3(g_Log, "CID:%10u / LockCount:%d / Num:%d / 스킬북을 넣어주지 못했습니다",
                        m_CharInfo.CID, nCount + 1, skillBookInfo.m_dwBookNum[nCount]);
                }
            }
        }

        return true;
    }

    return false;
}



}
}