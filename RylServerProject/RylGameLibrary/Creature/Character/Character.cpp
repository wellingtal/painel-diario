
#include "stdafx.h"

#include <Item/Container/ItemContainer.h>
#include <Item/Container/ContainerConstant.h>
#include <Item/Container/ExchangeContainer.h>
#include <Item/Container/EquipmentsContainer.h>

#include <Log/GameLog.h>
#include <Log/CharacterLog.h>
#include <Log/LogStruct.h>

#include <Utility/Setup/ServerSetup.h>

#include <Map/FieldMap/CellManager.h>
#include <Map/FieldMap//VirtualArea/VirtualArea.h>
#include <Map/FieldMap//VirtualArea/VirtualAreaMgr.h>
#include <Map/DuelMap/DuelCellManager.h>

#include <Skill/SkillMgr.h>
#include <Skill/SkillTable.h>

#include <Network/Dispatch/GameClient/GameClientDispatch.h>
#include <Network/Dispatch/GameClient/SendCharEtc.h>
#include <Network/Dispatch/Chat/ChatDispatch.h>

#include <Creature/CreatureManager.h>
#include <Creature/NPC/NPC.h>
#include <Creature/Siege/SiegeObject.h>
#include <Creature/Siege/SiegeObjectMgr.h>
#include <Creature/Siege/CampShop.h>

#include <Community/Guild/GuildMgr.h>
#include <Community/Guild/Guild.h>

#include "Character.h"

CCharacter::CCharacter(unsigned long dwCID, unsigned long dwSessionID)
:	CAggresiveCreature(dwCID), 
	m_dwUID(0),	m_dwSessionID(dwSessionID),
    m_nLogoutCount(LOGOUT_COUNT), m_nDBUpdateCount(DBUPDATE_COUNT), m_cConsumeMPCount(0),
	m_lpGameClientDispatch(NULL), m_lpSummonee(NULL), 
    m_cOperationFlags(0), m_cHandPos(0), m_cRidePos(0), m_friendList(dwCID, &CXRefFriends::GetInstance()), m_banList(dwCID, &CXRefBans::GetInstance()),
	m_dwLastUpdateExTime(0), m_cMoveUpdateExCount(0), m_dwRideArmsCID(0), m_dwRespawnSpeed(RESPAWN_PULSE),	m_dwLastKillTime(0),m_dwKilingCount(0),m_dwLastKilledID(0),
	m_dwProtectGateCID(0), m_dwLastShoutTime(0), RightNow(0),
    m_dwLastSendPartyAttackInfoTime(0), m_bChatBan(false), 
    m_cAccountNation(0), // WORK_LIST 2.4 계정 국적을 게임서버의 캐릭터가 가지도록 구현
    m_cNameChangeCount(0),
	m_cGuildWarFlag(Creature::WAR_OFF),
	m_cRealmWarFlag(Creature::WAR_OFF),
	m_dwExchangeID(0),
	m_cGMModelFlag(0),
	m_cRealmPoint(0),
	m_dwPlayTime(0),
	m_lPremiumTime(0),
	m_cGuildSafe(-1),
	m_iAbilityPoint(0),
	m_iUseAbilityPoint(0),
	m_iAdminAbilityPoint(0),
	m_bRealmWarBuffer(FALSE),
	m_bDead(FALSE),
    m_eLastDeadType(DEAD_BY_NONE),
	m_dwRespawnTownID(0)
{	
	m_StartTime = CTime::GetCurrentTime();
	m_StartPremiumTime = CTime::GetCurrentTime();

	/*edited*/
	m_dwLastKillTime=0;
	m_dwKilingCount=0;
	m_dwLastKilledID=0;
	RightNow=0;
	unsigned long	LastUsePotionTime[12] = {0};//hz added this for potion cooldown check
	m_KillSpreeLevel = 0;
	/**/
	memset(&m_FightInfo, 0, sizeof(CharacterFightInfo));
    memset(&m_DBData, 0, sizeof(m_DBData));
	memset(&m_CharacterStatus, 0, sizeof(m_CharacterStatus));
    memset(m_szAccountName, 0, sizeof(char) * CHAR_INFOST::MAX_ACCOUNT_LEN);

	memset(m_AbilityValue, 0, sizeof(m_AbilityValue));

	std::fill_n(m_wHistoryQuest, int(Quest::MAX_HISTORY_QUEST), 0);
	std::fill_n(m_cUsingMastery, int(MAX_USING_MASTERY), char(Skill::NONE_MASTERY));
	std::fill_n(m_bPadding, int(PAD_BYTE), 0x65);
}


CCharacter::~CCharacter()
{
	for(int nCount = 0; nCount < PAD_BYTE; ++nCount)
	{
		if (0x65 != (unsigned char)m_bPadding[nCount])
		{
			SERLOG1(g_Log, "CID:0x%08x 캐릭터의 패딩 바이트가 바뀌었습니다. "
				"버퍼 오버플로우가 일어났을 수 있습니다.", m_dwCID);
			break;	
		}
	}

	std::fill_n(m_bPadding, int(PAD_BYTE), 0x67);

    SetDispatcher(NULL);

	m_SpellMgr.GetAffectedInfo().ClearAll();
}


bool CCharacter::Initialize(CGameClientDispatch* lpGameClientDispatch)
{
    SetDispatcher(lpGameClientDispatch);

    if (!m_Inventory.Initialize(m_dwCID, 
        ContainerConstant::INVENTORY_WIDTH, 
        ContainerConstant::INVENTORY_HEIGHT, 
        ContainerConstant::MAX_INVENTORY_TAB)) 
    {
        return false; 
    }

    if (!m_Equipments.Initialize(this, 
        Item::EquipmentPos::MAX_EQUPMENT_POS))				
    { 
        return false; 
    }

    if (!m_ExtraSpace.Initialize(m_dwCID,
        Item::ExtraSpacePos::MAX_EXTRA_SPACE_NUM))            
    {
        return false; 
    }
    
    if (!m_Exchange.Initialize(this, 
        ContainerConstant::EXCHANGE_WIDTH, 
        ContainerConstant::EXCHANGE_HEIGHT))
    { 
        return false; 
    }

	if (!m_Deposit.Initialize(this, 
        ContainerConstant::DEPOSIT_WIDTH, 
        ContainerConstant::DEPOSIT_HEIGHT, 
        ContainerConstant::MAX_DEPOSIT_TAB))
    {
        return false; 
    }

	if (!m_Stall.Initialize(m_dwCID, 
        ContainerConstant::STALL_WIDTH, 
        ContainerConstant::STALL_HEIGHT))	
    {
        return false; 
    }

	if (!m_TempInven.Initialize(this, Item::MAX_TEMP_INVEN_ITEM_NUM))							
    { 
        return false;	
    }

    return true;
}


void CCharacter::SetDispatcher(CGameClientDispatch* lpGameClientDispatch)
{
    DETLOG5(g_Log, "UID:%d/CID:0x%08x(0x%p)/DispatchPointer:0x%p에서 -> 0x%p로 바꿉니다.",
        m_dwUID, m_dwCID, this, m_lpGameClientDispatch, lpGameClientDispatch);

    if (NULL != m_lpGameClientDispatch)
    {
        m_lpGameClientDispatch->SetCharacter(NULL);

        if (false == IsOperationFlagSet(CHAR_ZONE_MOVED))
        {
            m_lpGameClientDispatch->Disconnect();
        }
    }

    m_lpGameClientDispatch = lpGameClientDispatch;

	if (NULL != m_lpGameClientDispatch)
	{
		m_bLogout = false;
		m_dwUID = m_lpGameClientDispatch->GetUID();
		m_lpGameClientDispatch->SetCharacter(this);
	}
	else 
	{
		if (false == m_bLogout)
		{
			CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(m_Stall.GetOtherOwner());

			// 노점상 마무리
			if (0 != strcmp(m_Stall.GetStallName(), ""))
			{
				m_Stall.Close();
				m_Stall.SendCharStallOpen("");
			}

			if (0 != m_Stall.GetOtherOwner())
			{
				if (Creature::CT_PC == Creature::GetCreatureType(m_Stall.GetOtherOwner()))
				{
					if (NULL != lpCharacter)
					{
						lpCharacter->GetStall().Leave(this);
						lpCharacter->GetStall().SendCharStallEnter(m_dwCID, 0);
					}
				}
				else
				{
					CCampShop* lpShop = 
						reinterpret_cast<CCampShop*>(CCreatureManager::GetInstance().GetSiegeObject(m_Stall.GetOtherOwner()));
					if (NULL != lpShop)
					{
						lpShop->GetContainer().Leave(this);
						lpShop->GetContainer().SendCharStallEnter(m_dwCID, 0);
					}
				}
			}

			m_bLogout = true;
			CCreatureManager::GetInstance().EnqueueLogout(this);
		}
	}
}


bool CCharacter::GetCharacterView(CHAR_VIEW& charView)
{
	charView.CID = m_dwCID;

    memcpy(charView.Name, m_DBData.m_Info.Name, CHAR_INFOST::MAX_NAME_LEN);		// 캐릭터 이름

    charView.Sex			= m_DBData.m_Info.Sex;			// 캐릭터 성
	charView.Hair			= m_DBData.m_Info.Hair;			// 캐릭터 머리 모양
	charView.Face			= m_DBData.m_Info.Face;			// 캐릭터 얼굴 모양
	charView.Race			= m_DBData.m_Info.Race;			// 캐릭터 종족
	charView.Class			= m_DBData.m_Info.Class;		// 캐릭터 클래스
	charView.Fame			= m_DBData.m_Info.Fame;			// 명성
	charView.Mileage		= m_DBData.m_Info.Mileage;		// 마일리지
	charView.GID			= m_DBData.m_Info.GID;		    // 캐릭터 길드
	charView.PID			= m_DBData.m_Info.PID;		    // 캐릭터 파티
	charView.Level			= m_DBData.m_Info.Level;		// 캐릭터 레벨
	
	m_Equipments.GetEquipmentView(charView.Equip);	        // 장비(겉보기)
	return true;
}


bool CCharacter::BindPositionToNPC(const unsigned long dwNPCID) 
{
/*
	GAMELOG::ERRType eReturnCode = 0;

	CNPC* pNPC = CCreatureManager::GetInstance().GetNPC(dwNPCID);
	if (NULL != pNPC&& true == pNPC->IsBindable() &&
		pNPC->GetCurrentPos().GetDistance(m_CurrentPos) < CNPC::MAX_CONTACT_RANGE) 
	{
		int nZone = CServerSetup::GetInstance().GetServerZone();
		if (GetMapIndex() != 0)
		{
			VirtualArea::CVirtualArea* lpVirtualArea = VirtualArea::CVirtualAreaMgr::GetInstance().GetVirtualArea(GetMapIndex());
			if (NULL != lpVirtualArea)
			{
				nZone = static_cast<int>(lpVirtualArea->GetVirtualZone());
			}
		}

		if (pNPC->GetZone() == nZone)
		{
			m_DBData.m_Pos.SavePoint.fPointX = m_CurrentPos.m_fPointX;
			m_DBData.m_Pos.SavePoint.fPointY = m_CurrentPos.m_fPointY;
			m_DBData.m_Pos.SavePoint.fPointZ = m_CurrentPos.m_fPointZ;
		}
	}
	else
	{
		eReturnCode = 1;
	}

	// BindPosition로그를 남긴다.
	GAMELOG::LogCharBindPos(*this, dwNPCID, eReturnCode);

	return (0 == eReturnCode);
*/

	return true;
}

bool CCharacter::ControlOption(const RejectOption Reject, bool bLogin)
{
	m_RejectOption = Reject;

	// TODO : 키 세팅 저장
	return true;
}

unsigned char CCharacter::GetNation(void) const
{
	CGuild* lpGuild = CGuildMgr::GetInstance().GetGuild(GetGID());
	if (NULL != lpGuild)
	{
		if (Guild::JOIN_WAIT != lpGuild->GetTitle(m_dwCID))
		{
			return lpGuild->GetNation();
		}
	}

	// WORK_LIST 2.4 계정 국적을 게임서버의 캐릭터가 가지도록 구현
	return m_cAccountNation;
/*
	switch (m_DBData.m_Info.Race)
	{
		case CClass::HUMAN:		return Creature::KARTERANT;
		case CClass::AKHAN:		return Creature::MERKADIA;
	}

	ERRLOG2(g_Log, "CID:0x%08x 캐릭터의 종족 설정이 이상하여 제3국가로 사용합니다. 종족:%d", 
		m_dwCID, m_DBData.m_Info.Race);
	return Creature::ALMIGHTY_PIRATE;
*/
}

void CCharacter::SetPID(unsigned long dwPID)		
{ 
	m_DBData.m_Info.PID = dwPID;	
}

void CCharacter::SetGID(unsigned long dwGID)
{ 
	m_DBData.m_Info.GID = dwGID;	
}

ChState CCharacter::GetState(void)
{
	ChState state;

	state.m_wIP = m_DBData.m_Info.IP;
	state.m_wSTR = m_DBData.m_Info.STR;
	state.m_wDEX = m_DBData.m_Info.DEX;
	state.m_wCON = m_DBData.m_Info.CON;
	state.m_wINT = m_DBData.m_Info.INT;
	state.m_wWIS = m_DBData.m_Info.WIS;

	return state;
}
 
void CCharacter::SetFame(unsigned long dwFame)
{ 
	char cOldEliteBonus = GetEliteBonus();
	m_DBData.m_Info.Fame = dwFame;		
	char cNewEliteBonus = GetEliteBonus();

	if (cNewEliteBonus != cOldEliteBonus)
	{
		if (NULL != m_lpGameClientDispatch)
		{
			GameClientSendPacket::SendCharEliteBonus(
				m_lpGameClientDispatch->GetSendStream(), cNewEliteBonus);
		}
	}

	// 길드원 정보 업데이트
	CGuild* lpGuild = Guild::CGuildMgr::GetInstance().GetGuild(GetGID());
	if (NULL != lpGuild)
	{
		lpGuild->UpdateMemberInfo(m_dwCID, m_DBData.m_Info.Fame, Guild::TYPE_FAME);
	}

	// 퀘스트 트리거 체크
	CheckTrigger(Quest::TRIGGER_FAME, dwFame, GetCurrentPos(), 1);
}


bool CCharacter::SetDuelOpponent(CCharacter* lpCharacter)
{ 
	const POS DuelPos1 = { 3099, 1137, 3215 };	// 듀얼 스타트 위치
	const POS DuelPos2 = { 3156, 1137, 3209 };

	if (CServerSetup::GetInstance().GetDuelModeCheck())
	{
		CCell* lpDuelCell = NULL;
		unsigned long Cell_ID = 0;

		if (NULL == lpCharacter)
		{
			// 듀열 셀
			Cell_ID = m_FightInfo.m_dwCellID;

			lpDuelCell = CDuelCellManager::GetInstance().GetCell(Cell_ID);
			if (NULL == lpDuelCell)
			{
				ERRLOG1(g_Log, "(CID:0x%08x) 듀얼 셀 얻기에 실패했습니다.", GetCID());
				return false; 
			}

			// 듀얼 셀에서 제거
			lpDuelCell->DeleteCreature(m_dwCID);
			if (0 == lpDuelCell->GetCharacterNum())
			{
				// 셀 제거
				CDuelCellManager::GetInstance().DestroyCell(Cell_ID);
			}

			// 정보 설정
			m_FightInfo.m_pDuelOpponent = lpCharacter;	
			m_FightInfo.m_dwCellID		= 0;

			// 이전 셀로 복귀
			m_CellPos.MoveTo(m_CurrentPos);
			if (NULL == m_CellPos.m_lpCell)
			{
                ERRLOG1(g_Log, "(CID:0x%08x) 현재 셀 없음.", GetCID());
                return false; 
			}

			m_CellPos.m_lpCell->SetCreature(m_dwCID, this);

			GameClientSendPacket::SendCharBindPosition(*this, 0, 1, 
                m_DBData.m_Pos.LastPoint, CServerSetup::GetInstance().GetServerZone(), 0);
		}
		else
		{
			POS DuelPos = {0,};
			Cell_ID = lpCharacter->m_FightInfo.m_dwCellID;
			if (0 == Cell_ID)
			{
				Cell_ID = GetCID();

				lpDuelCell = CDuelCellManager::GetInstance().CreateCell(Cell_ID);
				if (NULL == lpDuelCell)
				{
					ERRLOG0(g_Log, "듀얼 셀 생성에 실패했습니다.");
					return false; 
				}

				DuelPos = DuelPos1;
            }
			else
			{
				lpDuelCell = CDuelCellManager::GetInstance().GetCell(Cell_ID);
				if (NULL == lpDuelCell)
				{
					ERRLOG0(g_Log, "듀얼 셀 생성에 실패했습니다.");
					return false; 
				}

				DuelPos = DuelPos2;
			}

			// 이전 셀에서 제거
			if (NULL == m_CellPos.m_lpCell)
			{
				ERRLOG1(g_Log, "(CID:0x%08x) 현재 셀 없음.", GetCID());
				return false; 
			}

			m_CellPos.m_lpCell->DeleteCreature(GetCID());
			m_CellPos.m_lpCell = NULL;

			// 듀얼 셀로 추가
			lpDuelCell->SetCreature(m_dwCID, this);
    		GameClientSendPacket::SendCharBindPosition(*this, 0, 1, DuelPos, 100, 0);

			// 정보 설정
			m_FightInfo.m_pDuelOpponent = lpCharacter;	
			m_FightInfo.m_dwCellID		= Cell_ID;
		}
	}
	else
	{
		m_FightInfo.m_pDuelOpponent = lpCharacter;	
	}

	return true;
}


void	CCharacter::SetGuildWarFlag(unsigned char cFlag)
{
	if (cFlag >= Creature::WAR_OFF && cFlag <= Creature::WAR_INSTANCE)
	{
		m_cGuildWarFlag = cFlag;

		// 길드원 정보 업데이트
		CGuild* lpGuild = Guild::CGuildMgr::GetInstance().GetGuild(GetGID());
		if (NULL != lpGuild)
		{
			lpGuild->UpdateMemberInfo(m_dwCID, m_cGuildWarFlag, Guild::TYPE_WARFLAG);
		}
	}
}