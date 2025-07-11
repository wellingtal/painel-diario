#include "stdafx.h"

#include <Item/Item.h>
#include <Item/ItemFactory.h>
#include <Item/Container/ItemContainer.h>
#include <Item/Container/EquipmentsContainer.h>

#include <Log/GameLog.h>
#include <Log/LogStruct.h>
#include <Log/ItemLog.h>

#include <Skill/SkillTable.h>
#include <Skill/Spell/SpellKind.h>

#include <Creature/Siege/SiegeObject.h>
#include <Creature/Siege/SiegeObjectMgr.h>

#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/CharStatusPacket.h>
#include <Network/Dispatch/GameClient/GameClientDispatch.h>
#include <Network/Dispatch/GameClient/SendCharSkill.h>

#include <Community/Guild/Guild.h>
#include <Community/Guild/GuildMgr.h>

#include "Character.h"


inline unsigned char GetLimitStatus(SKILLSLOT SkillSlot, char LimitValue) 
{
	return (SkillSlot.SKILLINFO.cLockCount * 6 + SkillSlot.SKILLINFO.cSkillLevel + 1) * LimitValue;
}


void LogSkillSlot(const CharacterDBData& DBData)
{
	int nMaxSkill = DBData.m_Skill.wSlotNum < SKILL::MAX_SLOT_NUM ? 
		DBData.m_Skill.wSlotNum : SKILL::MAX_SLOT_NUM;
	
	for (int nCount = 0; nCount < nMaxSkill; ++nCount)
	{
		ERRLOG5(g_Log, "CID:0x%08x, 캐릭터 스킬 정보를 출력합니다. 슬롯 %2d, 스킬 종류 0x%04x, 락 카운트 %d, 스킬 레벨 %d",
			DBData.m_Info.CID, nCount, DBData.m_Skill.SSlot[nCount].SKILLINFO.wSkill,
			DBData.m_Skill.SSlot[nCount].SKILLINFO.cLockCount, DBData.m_Skill.SSlot[nCount].SKILLINFO.cSkillLevel);
	}
}


unsigned short CCharacter::ReadSkill(SKILLSLOT SkillSlot, unsigned short wSkillID, unsigned short wSkillLockCount)
{
	const Skill::ProtoType* pSkillProtoType = CSkillMgr::GetInstance().GetSkillProtoType(wSkillID);

    if (NULL == pSkillProtoType)
    {
        return PktSk::FAIL_NOT_SKILL;
    }

	// By Minbobo
	// 임시 주석 처리. 다른 방식으로 클래스 체크.

	// 하위 클래스 스킬은 해당 종족이면 누구나 익힐 수 있고, 상위 클래스 스킬의 경우 해당 클래스만이 그 스킬을 익힐 수 있다.
	/*unsigned char SkillClass = static_cast<unsigned char>((((wSkillID - Skill::SKILL_MASK) & 0xFF00) >> 8) & 0x00FF);
	if (CClass::GetJobLevel(SkillClass) == CClass::JobLevel::DEFAULT_CLASS)
	{
		//// 하위 클래스 스킬
		if (GetRace() != CClass::GetRace(SkillClass))
		{
			return PktSk::FAIL_NOT_CURRENT_CLASS;
		}
	}
	else if (CClass::GetJobLevel(SkillClass) == CClass::JobLevel::JOB_CHANGE_1ST)
	{
		// 상위 클래스 스킬
		if (GetClass() != SkillClass)
		{
			return PktSk::FAIL_NOT_CURRENT_CLASS;
		}
	}*/

	if (wSkillLockCount == SkillSlot.SKILLINFO.cLockCount)	
	{
		if (SkillSlot.SKILLINFO.cSkillLevel < CSkillMgr::MAX_SKILL_LEVEL) 
		{
			for (int Slot = 0; Slot < Skill::ProtoType::MAX_LIMIT_NUM; ++Slot)
			{
				unsigned char cLimitType	= pSkillProtoType[wSkillLockCount].m_StatusLimitType[Slot];
				unsigned char cLimitValue	= pSkillProtoType[wSkillLockCount].m_StatusLimitValue[Slot];

				unsigned char cLimitSutatus = (SkillSlot.SKILLINFO.cLockCount * CSkillMgr::MAX_SKILL_LEVEL + 
					SkillSlot.SKILLINFO.cSkillLevel) * cLimitValue;

				switch (cLimitType)
				{
					case Skill::StatusLimit::NONE:	return PktBase::NO_SERVER_ERR;
					case Skill::StatusLimit::STR:	if (cLimitSutatus > m_CharacterStatus.m_nSTR - 20) { return PktSk::FAIL_NOT_ENOUGH_STATUS; }	break;
					case Skill::StatusLimit::DEX:	if (cLimitSutatus > m_CharacterStatus.m_nDEX - 20) { return PktSk::FAIL_NOT_ENOUGH_STATUS; }	break;
					case Skill::StatusLimit::CON:	if (cLimitSutatus > m_CharacterStatus.m_nCON - 20) { return PktSk::FAIL_NOT_ENOUGH_STATUS; }	break;
					case Skill::StatusLimit::INT:	if (cLimitSutatus > m_CharacterStatus.m_nINT - 20) { return PktSk::FAIL_NOT_ENOUGH_STATUS; }	break;
					case Skill::StatusLimit::WIS:	if (cLimitSutatus > m_CharacterStatus.m_nWIS - 20) { return PktSk::FAIL_NOT_ENOUGH_STATUS; }	break;
				}
			}
		}
		else 
		{	
			return PktSk::FAIL_MAX_LEVEL;	
		}
	}
	else 
	{	
		return PktSk::FAIL_NOT_CURRENT_LOCK_COUNT;		
	}

	return PktBase::NO_SERVER_ERR;
}


bool CCharacter::RedistributionSkill(void)
{
	if (false == CalculateStatusData(false)) 
	{ 
		return false; 
	}

	SKILL &Skill = m_DBData.m_Skill;
	int nSlotIndex = 0;

	for (; nSlotIndex < Skill.wSlotNum; ++nSlotIndex)
	{
		const Skill::ProtoType* pSkillProtoType = 
			CSkillMgr::GetInstance().GetSkillProtoType(Skill.SSlot[nSlotIndex].SKILLINFO.wSkill);
		if (NULL == pSkillProtoType)
		{
			ERRLOG2(g_Log, "CID:0x%08x 존재하지않는 스킬을 가지고 있습니다. SkillID:0x%04x", 
				m_dwCID, Skill.SSlot[nSlotIndex].SKILLINFO.wSkill);
			return false;
		}		

		while (true)
		{
			bool bLevelDown = false;

			for (int nTypeIndex = 0; nTypeIndex < Skill::ProtoType::MAX_LIMIT_NUM; nTypeIndex++)
			{
				unsigned char cLimitType = pSkillProtoType->m_StatusLimitType[nTypeIndex];
				char cLimitValue = pSkillProtoType->m_StatusLimitValue[nTypeIndex];

				unsigned char cLimitStatus = (Skill.SSlot[nSlotIndex].SKILLINFO.cLockCount * CSkillMgr::MAX_SKILL_LEVEL + 
					static_cast<int>(Skill.SSlot[nSlotIndex].SKILLINFO.cSkillLevel) - 1) * cLimitValue;

				switch (cLimitType)
				{
					case Skill::StatusLimit::STR:	if (cLimitStatus > m_CharacterStatus.m_nSTR - 20) { bLevelDown = true; } break;
					case Skill::StatusLimit::DEX:	if (cLimitStatus > m_CharacterStatus.m_nDEX - 20) { bLevelDown = true; } break;
					case Skill::StatusLimit::CON:	if (cLimitStatus > m_CharacterStatus.m_nCON - 20) { bLevelDown = true; } break;
					case Skill::StatusLimit::INT:	if (cLimitStatus > m_CharacterStatus.m_nINT - 20) { bLevelDown = true; } break;
					case Skill::StatusLimit::WIS:	if (cLimitStatus > m_CharacterStatus.m_nWIS - 20) { bLevelDown = true; } break;
				}
			}

			if (true == bLevelDown)
			{
				if (0 == Skill.SSlot[nSlotIndex].SKILLINFO.cSkillLevel)
				{
					--Skill.SSlot[nSlotIndex].SKILLINFO.cLockCount;
					Skill.SSlot[nSlotIndex].SKILLINFO.cSkillLevel = CSkillMgr::MAX_SKILL_LEVEL;
				}

				--Skill.SSlot[nSlotIndex].SKILLINFO.cSkillLevel;
				--Skill.wSkillNum;

				if (0 == Skill.SSlot[nSlotIndex].SKILLINFO.cSkillLevel && 0 == Skill.SSlot[nSlotIndex].SKILLINFO.cLockCount)
				{
					// 스킬 슬롯을 도는 루프 안이므로 슬롯 자체를 삭제하는 건 이후에 따로 처리한다.
					Skill.SSlot[nSlotIndex] = SKILLSLOT::SKILLSLOT();
					break;
				}
			}
			else
			{
				break;
			}
		}
	}

	// 스킬 삭제로 비워진 슬롯을 삭제한다.
	for (nSlotIndex = 0; nSlotIndex < Skill.wSlotNum; )
	{
		if (0 == Skill.SSlot[nSlotIndex].dwSkillSlot)
		{
			unsigned char Count = nSlotIndex;
			for (; Count < Skill.wSlotNum - 1; ++Count) 
			{
				Skill.SSlot[Count] = Skill.SSlot[Count + 1];
			}

			Skill.SSlot[Count] = SKILLSLOT::SKILLSLOT();
			--Skill.wSlotNum;

			continue;
		}

		++nSlotIndex;
	}

	return CalculateMaxSkillSlot();
}


bool CCharacter::CalculateMaxSkillSlot(void)
{
	SKILL &Skill = m_DBData.m_Skill;

	// 스탯이 바껴 최대 슬롯수가 줄어들면 그 차만큼 삭제한다.
	short wDeleteSkillNum = Skill.GetSkillNum() - m_CreatureStatus.m_StatusInfo.m_wSkillPoint; 

	while (wDeleteSkillNum > 0)
	{
		if (1 > Skill.wSlotNum)
		{
			ERRLOG1(g_Log, "CID:0x%08x 지울 스킬 슬롯이 없습니다.", m_dwCID);
			return false;
		}

		int nSlotIndex = Skill.wSlotNum - 1;
		const Skill::ProtoType* pSkillProtoType = 
			CSkillMgr::GetInstance().GetSkillProtoType(Skill.SSlot[nSlotIndex].SKILLINFO.wSkill);
		if (NULL == pSkillProtoType)
		{
			ERRLOG2(g_Log, "CID:0x%08x 존재하지않는 스킬을 가지고 있습니다. SkillID:0x%04x", 
				m_dwCID, Skill.SSlot[nSlotIndex].SKILLINFO.wSkill);
			return false;
		}		

		if (0 == Skill.SSlot[nSlotIndex].SKILLINFO.cSkillLevel)
		{
			--Skill.SSlot[nSlotIndex].SKILLINFO.cLockCount;
			Skill.SSlot[nSlotIndex].SKILLINFO.cSkillLevel = CSkillMgr::MAX_SKILL_LEVEL;
		}

		--Skill.SSlot[nSlotIndex].SKILLINFO.cSkillLevel;
		--Skill.wSkillNum;

		--wDeleteSkillNum;

		if (0 == Skill.SSlot[nSlotIndex].SKILLINFO.cSkillLevel && 0 == Skill.SSlot[nSlotIndex].SKILLINFO.cLockCount)
		{
			Skill.SSlot[nSlotIndex] = SKILLSLOT::SKILLSLOT();
			--Skill.wSlotNum;
		}

		UpdateQuickSlotSkill(Skill.SSlot[nSlotIndex]);
	}

	return true;
}

int CCharacter::GetAbilityPoint()
{
	if(GetLevel() > 40)
	{
		int level=(GetLevel() - 40);
		return level;
	}
	else
		return 0;
}

void CCharacter::UpdateUseAbilityPoint()
{
	m_iUseAbilityPoint = 0;

	SKILL &Skill = m_DBData.m_Skill;

	// 어빌리티 포인트를 검사한다.
	for(int i = 0; i <= Skill.wSlotNum; ++i)
	{
		unsigned short SkillID = Skill.SSlot[i].SKILLINFO.wSkill;
		if(0x1000 <= SkillID && SkillID < 0x2000) // hz change this range when i want to add new abilities
		{
			const Skill::ProtoType* lpProtoType = CSkillMgr::GetInstance().GetSkillProtoType(SkillID);
			if (NULL == lpProtoType)
			{
				ERRLOG2(g_Log, "CID:0x%08x 어빌리티 아이디가 이상합니다. 스킬 아이디:0x%04x", m_dwCID, SkillID);
				continue;
			}	

			short SkillLV = Skill.SSlot[i].SKILLINFO.cLockCount;
			if(SkillLV >= CSkillMgr::MAX_SKILL_LOCKCOUNT)
				SkillLV = CSkillMgr::MAX_SKILL_LOCKCOUNT-1;
			
			for(int j = 0; j <= SkillLV; ++j)
				m_iUseAbilityPoint += (int)lpProtoType[j].m_fMinRange;
		}
	}

	return;
}

bool CCharacter::AbilityCreate(Item::CUseItem* lpUseItem)
{
    if(0 == lpUseItem)
    {
        ERRLOG1(g_Log, "CID:%10u / 스킬 생성 실패 : 사용 아이템이 0입니다", m_dwCID);
        return false;
    }

	unsigned short SkillID = lpUseItem->GetItemInfo().m_UseItemInfo.m_usSkill_ID;
	char SkillLockCount = (char)lpUseItem->GetItemInfo().m_UseItemInfo.m_usSkill_LockCount;
	unsigned short wError = PktBase::NO_SERVER_ERR;

	unsigned char Index = 0;
	bool bLockFlag = false;		// 락이 될 때는 SkillCreate 의 Ack 패킷을 보내지 않는다.

	SKILL &Skill = m_DBData.m_Skill;
	if (SKILL::MAX_SLOT_NUM < Skill.wSlotNum) 
	{
        ERRLOG2(g_Log, "CID:%10u / 어빌리티 생성 실패 : 스킬 개수가 이상합니다. 슬롯 수:%d", m_dwCID, Skill.wSlotNum);
		return false;
	}	

	const Skill::ProtoType* lpProtoType = CSkillMgr::GetInstance().GetSkillProtoType(SkillID);
	if (NULL == lpProtoType)
	{
		ERRLOG2(g_Log, "CID:0x%08x 어빌리티 아이디가 이상합니다. 스킬 아이디:0x%04x", m_dwCID, SkillID);
		return false;
	}	

	int UsePoint = (int)lpProtoType[SkillLockCount].m_fMinRange;
	/*unsigned char MaxLearnAbility;
	

	if(this->GetLevel() > 40)
	
		MaxLearnAbility=this->GetLevel()- 40;
	else
		MaxLearnAbility=0;*/

/*	if (GetUseAbilityPoint()+UsePoint > GetAbilityPoint() )//((MaxLearnAbility - Skill.GetAbilityNum()) <= 0) //hz was if (GetUseAbilityPoint()+UsePoint > GetAbilityPoint() )
	{
		wError = PktSk::FAIL_NOT_ENOUGH_SKILL_POINT;	
	}
	if((MaxLearnAbility>0)&&(wError!=7))
	{
		for (Index = 0; Index < Skill.wSlotNum; ++Index) 
		{
			if (SkillID == Skill.SSlot[Index].SKILLINFO.wSkill)
			{
				if(Skill.SSlot[Index].SKILLINFO.cLockCount < SkillLockCount)
				{*/

	if (GetUseAbilityPoint()+UsePoint > GetAbilityPoint() ) //blackdog closed auto 60 ability points, rewrote that bitch
	{
		wError = PktSk::FAIL_NOT_ENOUGH_SKILL_POINT;	
	}
	else
	{
		for (Index = 0; Index < Skill.wSlotNum; ++Index) 
		{
			if (SkillID == Skill.SSlot[Index].SKILLINFO.wSkill)
			{
				if(Skill.SSlot[Index].SKILLINFO.cLockCount < SkillLockCount)
				{
					
					bLockFlag = true;
					Skill.SSlot[Index].SKILLINFO.cLockCount++;
					Skill.SSlot[Index].SKILLINFO.cSkillLevel	= 1;

					
					if (NULL != m_lpGameClientDispatch)
					{
						GameClientSendPacket::SendCharSkillCommand(m_lpGameClientDispatch->GetSendStream(), m_dwCID, 
							CmdCharSkillLock, Index, SkillID, wError);
					}
				}
				break;
			}
		}

		// 없는 스킬이면.
		if (Index == Skill.wSlotNum) 
		{
			if (Skill.wSlotNum == SKILL::MAX_SLOT_NUM) 
			{
				wError = PktSk::FAIL_NOT_ENOUGH_SLOT;	
			}
			else
			{
				// 어빌리티는 제한이 없다. 그냥 배우면 된다.
				Skill.SSlot[Index].SKILLINFO.wSkill			= SkillID;
				Skill.SSlot[Index].SKILLINFO.cLockCount		= SkillLockCount;
				Skill.SSlot[Index].SKILLINFO.cSkillLevel	= 1;
				++Skill.wSlotNum; 
			}
		}

		CalculateStatusData(false);

		// 퀵슬롯 업데이트
		UpdateQuickSlotSkill(Skill.SSlot[Index]);
		UpdateUseAbilityPoint();
	}

	

	// 패킷 보내기
    if (false == bLockFlag && NULL != m_lpGameClientDispatch)
    {
        GameClientSendPacket::SendCharSkillCommand(m_lpGameClientDispatch->GetSendStream(), m_dwCID, 
            CmdCharSkillCreate, Index, SkillID, wError);
    }
	
	if (PktBase::NO_SERVER_ERR != wError) 
	{ 
		ERRLOG7(g_Log, "CID:%10u 어빌리티 생성에 실패하였습니다. 스킬수:%d/%d"
			" Create Ability Index:%d, Level:%d, Int:%d, Error:%d",
			m_dwCID, Skill.GetSkillNum(), m_CreatureStatus.m_StatusInfo.m_wSkillPoint, 
			Index, m_DBData.m_Info.Level, m_CharacterStatus.m_nINT, wError);
		LogSkillSlot(m_DBData);
		return false; 
	}	

	return true;	
}

bool CCharacter::AbilityErase(unsigned char Index_In, Item::ItemPos InvenPos)
{
	SKILL &Skill = m_DBData.m_Skill;
	unsigned short SkillID	= 0;
	unsigned short wError	= 0;
	bool bUnlockFlag = false;	// Unlock 될때는 SkillErase 의 Ack 패킷을 보내지 않는다.

	if (SKILL::MAX_SLOT_NUM < Skill.wSlotNum)
	{
		ERRLOG2(g_Log, "CID:0x%08x 어빌리티 개수가 이상합니다. 스킬 슬롯 수:%d", m_dwCID, Skill.wSlotNum);
		return false;
	}	

    if (Skill.wSlotNum <= Index_In)
    {
        ERRLOG3(g_Log, "CID:0x%08x 어빌리티 제거 오류 : 잘못된 인덱스입니다. 스킬 개수:%d, 인덱스:%d", m_dwCID, Skill.wSlotNum, Index_In);
        return false;
    }

	SkillID	= Skill.SSlot[Index_In].SKILLINFO.wSkill;	
	
	const Skill::ProtoType* lpProtoType = CSkillMgr::GetInstance().GetSkillProtoType(SkillID);
	if (NULL == lpProtoType)
	{
		ERRLOG2(g_Log, "CID:0x%08x 어빌리티 아이디가 이상합니다. 스킬 아이디:0x%04x", m_dwCID, SkillID);
		return false;
	}	

	/*Item::CItem* lpItem = NULL; //hz delete ability doesnt require stone of oblivion
	if(InvenPos.m_cPos != TakeType::TS_ADMIN)
	{
		// 망각의돌 확인
		lpItem = m_Inventory.GetItem(InvenPos);
		if (NULL == lpItem)
		{
			ERRLOG4(g_Log, "CID:0x%08x 어빌리티삭제 오류 : 요청한 위치에 아이템이 없습니다. SkillID:%d, Pos:%d, Index:%d", m_dwCID, SkillID, InvenPos.m_cPos, InvenPos.m_cIndex);

			GameClientSendPacket::SendCharSkillCommand(m_lpGameClientDispatch->GetSendStream(), m_dwCID, 
				CmdCharSkillErase, Index_In, SkillID, PktSk::FAIL_NO_ITEM);

			return true;
		}
		if (Item::EtcItemID::OBLIVION_STONE != lpItem->GetPrototypeID())
		{
			ERRLOG3(g_Log, "CID:0x%08x 어빌리티삭제 오류 : 사용하려는 아이템이 망각의 돌이 아닙니다. SkillID:%d, ItemID:%d", m_dwCID, SkillID, lpItem->GetPrototypeID());

			GameClientSendPacket::SendCharSkillCommand(m_lpGameClientDispatch->GetSendStream(), m_dwCID, 
				CmdCharSkillErase, Index_In, SkillID, PktSk::FAIL_INVALID_ITEM);

			return true;
		}
		// 망각의돌 개수가 이상한지 확인
		if(lpItem->GetNumOrDurability() <= 0)
		{
			ERRLOG4(g_Log, "CID:0x%08x 어빌리티삭제 오류 : 망각의 돌의 갯수가 이상합니다. SkillID:%d, Pos:%d, Index:%d", m_dwCID, SkillID, InvenPos.m_cPos, InvenPos.m_cIndex);

			GameClientSendPacket::SendCharSkillCommand(m_lpGameClientDispatch->GetSendStream(), m_dwCID, 
				CmdCharSkillErase, Index_In, SkillID, PktSk::FAIL_NO_ITEM);

			return true;
		}
	}*/

	if (Index_In >= Skill.wSlotNum)
	{
		wError = PktSk::FAIL_NOT_SKILL;				// 해당 인덱스에 스킬 없음
	}
	else
	{
		SKILLSLOT SkillSlot = Skill.SSlot[Index_In];
		if (SkillSlot.SKILLINFO.cLockCount >= 1)
		{
			bUnlockFlag = true;

			Skill.SSlot[Index_In].SKILLINFO.cLockCount--;
			//Skill.SSlot[Index_In].SKILLINFO.cSkillLevel = 1;
			// 패킷 보내기
			if (NULL != m_lpGameClientDispatch)
			{
				GameClientSendPacket::SendCharSkillCommand(m_lpGameClientDispatch->GetSendStream(), m_dwCID, 
					CmdCharSkillUnLock, Index_In, SkillID, wError, static_cast<unsigned char>(Skill.SSlot[Index_In].SKILLINFO.cSkillLevel), &InvenPos);
			}
		}
		else
		{
			unsigned char Count = Index_In;
			for (; Count < Skill.wSlotNum - 1; ++Count) 
			{
				Skill.SSlot[Count] = Skill.SSlot[Count + 1];
			}

			Skill.SSlot[Count].SKILLINFO.wSkill			= 0;
			Skill.SSlot[Count].SKILLINFO.cLockCount		= 0;
			Skill.SSlot[Count].SKILLINFO.cSkillLevel	= 0;
			--Skill.wSlotNum; 
		}
		UpdateUseAbilityPoint();
	}

	

	CalculateStatusData(false);

	if (PktBase::NO_SERVER_ERR == wError)
	{
		// 에러없이 성공했으면 망각의돌 감소
		/*if(InvenPos.m_cPos != TakeType::TS_ADMIN) //hz delete ability doesnt require stone of oblivion
		{
			// 망각의돌 개수 1개 감소
			lpItem->SetNumOrDurability(lpItem->GetNumOrDurability() - 1);

			if (0 == lpItem->GetNumOrDurability())
			{
				if (RemoveItem(InvenPos))
				{
					DELETE_ITEM(lpItem);
				}
				else
				{
					ERRLOG4(g_Log, "CID:0x%08x 어빌리티삭제 오류 : 망각의 돌을 제거할 수 없습니다. SkillID:%d, Pos:%d, Index:%d", m_dwCID, SkillID, InvenPos.m_cPos, InvenPos.m_cIndex);

					m_Inventory.DumpItemInfo();

					GameClientSendPacket::SendCharSkillCommand(m_lpGameClientDispatch->GetSendStream(), m_dwCID, 
						CmdCharSkillErase, Index_In, SkillID, PktSk::FAIL_NO_ITEM);

					return true;
				}
			}
		}*/

		// 스킬락 확인
		UpdateQuickSlotSkill(Skill.SSlot[Index_In]);
	}

	// 패킷 보내기
    if (false == bUnlockFlag && NULL != m_lpGameClientDispatch)
    {
        GameClientSendPacket::SendCharSkillCommand(m_lpGameClientDispatch->GetSendStream(), m_dwCID, 
            CmdCharSkillErase, Index_In, SkillID, wError, 0, &InvenPos);
    }

	if (PktBase::NO_SERVER_ERR != wError) 
	{
		ERRLOG5(g_Log, "CID:0x%08x 스킬 삭제에 실패하였습니다. Erase Skill Index:%d, SkillID:%d, 스킬 개수:%d, Error:%d", m_dwCID, Index_In, SkillID, Skill.wSlotNum, wError);

		LogSkillSlot(m_DBData);		
	}	

	return true;
}


bool CCharacter::SkillCreate(Item::CUseItem* lpUseItem)
{
    if(0 == lpUseItem)
    {
        ERRLOG1(g_Log, "CID:%10u / 스킬 생성 실패 : 사용 아이템이 0입니다", m_dwCID);
        return false;
    }

	unsigned short SkillID = lpUseItem->GetItemInfo().m_UseItemInfo.m_usSkill_ID;
	unsigned short SkillLockCount = lpUseItem->GetItemInfo().m_UseItemInfo.m_usSkill_LockCount;
	unsigned short wError = PktBase::NO_SERVER_ERR;

	unsigned char Index = 0;
	bool bLockFlag = false;		// 락이 될 때는 SkillCreate 의 Ack 패킷을 보내지 않는다.
	
	SKILL &Skill = m_DBData.m_Skill;
	if (SKILL::MAX_SLOT_NUM < Skill.wSlotNum) 
	{
        ERRLOG2(g_Log, "CID:%10u / 스킬 생성 실패 : 스킬 개수가 이상합니다. 슬롯 수:%d", m_dwCID, Skill.wSlotNum);
		return false;
	}	

/*	if(GetUseAbilityPoint() > 0)//hz cant learn skill after learning an ability.  //blackdog closed , thats bullshit
	{
		wError=PktSk::FAIL_NOT_ENOUGH_SLOT;
	}
	else*/ if (Skill.GetSkillNum() >= m_CreatureStatus.m_StatusInfo.m_wSkillPoint) 
	{
		wError = PktSk::FAIL_NOT_ENOUGH_SKILL_POINT;	
	}
	else
	{
        // 인덱스 구하기
		for (Index = 0; Index < Skill.wSlotNum; ++Index) 
		{
			if (SkillID == Skill.SSlot[Index].SKILLINFO.wSkill)
			{
				// 스킬 증가
				wError = ReadSkill(Skill.SSlot[Index], SkillID, SkillLockCount);
				if (PktBase::NO_SERVER_ERR == wError)
				{
					++Skill.SSlot[Index].SKILLINFO.cSkillLevel;	
					++Skill.wSkillNum;

					if (Skill.SSlot[Index].SKILLINFO.cLockCount < CSkillMgr::MAX_SKILL_LOCKCOUNT - 2 && 
							Skill.SSlot[Index].SKILLINFO.cSkillLevel == CSkillMgr::MAX_SKILL_LEVEL)
					{
							bLockFlag = SkillLock(Index);
					}
				}

				break;
			}
		}

		if (Index == Skill.wSlotNum) 
		{
			if (Skill.wSlotNum == SKILL::MAX_SLOT_NUM) 
			{
				wError = PktSk::FAIL_NOT_ENOUGH_SLOT;	
			}
			else
			{
				// 슬롯 추가
				wError = ReadSkill(Skill.SSlot[Index], SkillID, SkillLockCount);
				if (PktBase::NO_SERVER_ERR == wError)
				{
					Skill.SSlot[Index].SKILLINFO.wSkill			= SkillID;
					Skill.SSlot[Index].SKILLINFO.cLockCount		= 0;
					Skill.SSlot[Index].SKILLINFO.cSkillLevel	= 1;
					++Skill.wSkillNum;
					++Skill.wSlotNum;
				}
			}
		}

		CalculateStatusData(false);

		// 퀵슬롯 업데이트
		UpdateQuickSlotSkill(Skill.SSlot[Index]);
	}

	// 패킷 보내기
    if (false == bLockFlag && NULL != m_lpGameClientDispatch)
    {
        GameClientSendPacket::SendCharSkillCommand(m_lpGameClientDispatch->GetSendStream(), m_dwCID, 
            CmdCharSkillCreate, Index, SkillID, wError);
    }
	
	if (PktBase::NO_SERVER_ERR != wError) 
	{ 
		ERRLOG7(g_Log, "CID:%10u 스킬 생성에 실패하였습니다. 스킬수:%d/%d"
			" Create Skill Index:%d, Level:%d, Int:%d, Error:%d",
			m_dwCID, Skill.GetSkillNum(), m_CreatureStatus.m_StatusInfo.m_wSkillPoint, 
			Index, m_DBData.m_Info.Level, m_CharacterStatus.m_nINT, wError);
		LogSkillSlot(m_DBData);
		return false; 
	}	

	return true;	
}


bool CCharacter::SkillErase(unsigned char Index_In, Item::ItemPos InvenPos)
{
	SKILL &Skill = m_DBData.m_Skill;
	unsigned short SkillID	= 0;
	unsigned short wError	= 0;
	bool bUnlockFlag = false;	// Unlock 될때는 SkillErase 의 Ack 패킷을 보내지 않는다.

	if (SKILL::MAX_SLOT_NUM < Skill.wSlotNum)
	{
		ERRLOG2(g_Log, "CID:0x%08x 스킬 개수가 이상합니다. 슬롯 수:%d", m_dwCID, Skill.wSlotNum);
		return false;
	}	

    if (Skill.wSlotNum <= Index_In)
    {
        ERRLOG3(g_Log, "CID:0x%08x 스킬 제거 오류 : 잘못된 인덱스입니다. 스킬 개수:%d, 인덱스:%d", m_dwCID, Skill.wSlotNum, Index_In);
        return false;
    }

	SkillID	= Skill.SSlot[Index_In].SKILLINFO.wSkill;	
	
	const Skill::ProtoType* lpProtoType = CSkillMgr::GetInstance().GetSkillProtoType(SkillID);
	if (NULL == lpProtoType)
	{
		ERRLOG2(g_Log, "CID:0x%08x 스킬 아이디가 이상합니다. 스킬 아이디:0x%04x", m_dwCID, SkillID);
		return false;
	}	

	if (Skill::Type::CHANT == lpProtoType->m_eSkillType) 
	{
		AtType attackType;
		attackType.m_wType = SkillID;
		attackType.m_cSkillLevel = 0;
		attackType.m_cSkillLockCount = 0;
		attackType.m_cAtCount = 0;

		unsigned char cOffencerJudge = 0, cDefenserJudge = 0;
		unsigned short wOffencerMPHeal = 0, wDefenserMPHeal = 0;

		Skill::CProcessTable::GetInstance().UseSkill(attackType, this, this, cOffencerJudge, cDefenserJudge, wOffencerMPHeal, wDefenserMPHeal, wError);
	}

	Item::CItem* lpItem = NULL;
	if(InvenPos.m_cPos != TakeType::TS_ADMIN)
	{
		// 망각의돌 확인
		lpItem = m_Inventory.GetItem(InvenPos);
		if (NULL == lpItem)
		{
			ERRLOG4(g_Log, "CID:0x%08x 스킬삭제 오류 : 요청한 위치에 아이템이 없습니다. SkillID:%d, Pos:%d, Index:%d", m_dwCID, SkillID, InvenPos.m_cPos, InvenPos.m_cIndex);

			GameClientSendPacket::SendCharSkillCommand(m_lpGameClientDispatch->GetSendStream(), m_dwCID, 
				CmdCharSkillErase, Index_In, SkillID, PktSk::FAIL_NO_ITEM);

			return true;
		}
		if (Item::EtcItemID::OBLIVION_STONE != lpItem->GetPrototypeID())
		{
			ERRLOG3(g_Log, "CID:0x%08x 스킬삭제 오류 : 사용하려는 아이템이 망각의 돌이 아닙니다. SkillID:%d, ItemID:%d", m_dwCID, SkillID, lpItem->GetPrototypeID());

			GameClientSendPacket::SendCharSkillCommand(m_lpGameClientDispatch->GetSendStream(), m_dwCID, 
				CmdCharSkillErase, Index_In, SkillID, PktSk::FAIL_INVALID_ITEM);

			return true;
		}
		// 망각의돌 개수가 이상한지 확인
		if(lpItem->GetNumOrDurability() <= 0)
		{
			ERRLOG4(g_Log, "CID:0x%08x 스킬삭제 오류 : 망각의 돌의 갯수가 이상합니다. SkillID:%d, Pos:%d, Index:%d", m_dwCID, SkillID, InvenPos.m_cPos, InvenPos.m_cIndex);

			GameClientSendPacket::SendCharSkillCommand(m_lpGameClientDispatch->GetSendStream(), m_dwCID, 
				CmdCharSkillErase, Index_In, SkillID, PktSk::FAIL_NO_ITEM);

			return true;
		}
	}


	if (Index_In >= Skill.wSlotNum)
	{
		wError = PktSk::FAIL_NOT_SKILL;				// 해당 인덱스에 스킬 없음
	}
	else
	{
		SKILLSLOT SkillSlot = Skill.SSlot[Index_In];
		if (SkillSlot.SKILLINFO.cLockCount > 0)
		{
			if (0 == SkillSlot.SKILLINFO.cSkillLevel)
			{
				if (4 <= SkillSlot.SKILLINFO.cLockCount)
				{
					wError = PktSk::FAIL_FIFTHSKILL_UNLOCK;
				}
				else
				{
					// 상위 단계 0 레벨 스킬을 지우는 경우 하위 단계 5 레벨로 만들어 주어야 한다.
					// 여기서 지우지 말고 변수를 TRUE 해서 아래에서 지워주게한다.

					// SkillUnLock 함수안에서bUnlockFlag가 true일때 패킷을 보내기 때문에 
					// bUnlockFlag가 true이면 아래에서 패킷을 보내지 않는다.
					// 이함수 내에서 이미 패킷을 날리기 때문에..
					bUnlockFlag = SkillUnLock(Index_In, &InvenPos);
				}
			}
			else
			{
				--Skill.SSlot[Index_In].SKILLINFO.cSkillLevel;                
				--Skill.wSkillNum;
			}
		}
		else
		{
			if (0 == SkillSlot.SKILLINFO.cSkillLevel)
			{
				wError = PktSk::FAIL_NON_LEVEL;		// 스킬 지우기 실패
			}
			else if (1 == SkillSlot.SKILLINFO.cSkillLevel) 
			{
				// 클래스 스킬을 지우려는지 체크
				if (lpProtoType->m_bIsClassSkill)
				{
					wError = PktSk::FAIL_ERASE_CLASS_SKILL;
				}
				else
				{
					unsigned char Count = Index_In;
					for (; Count < Skill.wSlotNum - 1; ++Count) 
					{
						Skill.SSlot[Count] = Skill.SSlot[Count + 1];
					}

					Skill.SSlot[Count].SKILLINFO.wSkill			= 0;
					Skill.SSlot[Count].SKILLINFO.cLockCount		= 0;
					Skill.SSlot[Count].SKILLINFO.cSkillLevel	= 0;
					--Skill.wSkillNum;
					--Skill.wSlotNum;
				}
			}
			else
			{
				--Skill.SSlot[Index_In].SKILLINFO.cSkillLevel;
				--Skill.wSkillNum;
			}
		}
	}

	CalculateStatusData(false);

	if (PktBase::NO_SERVER_ERR == wError)
	{
		// 에러없이 성공했으면 망각의돌 감소
		if(InvenPos.m_cPos != TakeType::TS_ADMIN)
		{
			// 망각의돌 개수 1개 감소
			lpItem->SetNumOrDurability(lpItem->GetNumOrDurability() - 1);

			if (0 == lpItem->GetNumOrDurability())
			{
				if (RemoveItem(InvenPos))
				{
					DELETE_ITEM(lpItem);
				}
				else
				{
					ERRLOG4(g_Log, "CID:0x%08x 스킬삭제 오류 : 망각의 돌을 제거할 수 없습니다. SkillID:%d, Pos:%d, Index:%d", m_dwCID, SkillID, InvenPos.m_cPos, InvenPos.m_cIndex);

					m_Inventory.DumpItemInfo();

					GameClientSendPacket::SendCharSkillCommand(m_lpGameClientDispatch->GetSendStream(), m_dwCID, 
						CmdCharSkillErase, Index_In, SkillID, PktSk::FAIL_NO_ITEM);

					return true;
				}
			}
		}

		// 스킬락 확인
		UpdateQuickSlotSkill(Skill.SSlot[Index_In]);
	}

	// 패킷 보내기
    if (false == bUnlockFlag && NULL != m_lpGameClientDispatch)
    {
        GameClientSendPacket::SendCharSkillCommand(m_lpGameClientDispatch->GetSendStream(), m_dwCID, 
            CmdCharSkillErase, Index_In, SkillID, wError, 0, &InvenPos);
    }

	if (PktBase::NO_SERVER_ERR != wError) 
	{
		ERRLOG5(g_Log, "CID:0x%08x 스킬 삭제에 실패하였습니다. Erase Skill Index:%d, SkillID:%d, 스킬 개수:%d, Error:%d", m_dwCID, Index_In, SkillID, Skill.wSlotNum, wError);

		LogSkillSlot(m_DBData);		
	}	

	return true;
}

bool CCharacter::SkillLock(unsigned char Index_In)
{
	SKILL &Skill = m_DBData.m_Skill;

	if (SKILL::MAX_SLOT_NUM < Skill.wSlotNum) 
	{
		ERRLOG2(g_Log, "CID:0x%08x 스킬 개수가 이상합니다. 슬롯 수:%d", m_dwCID, Skill.wSlotNum);
		return false;
	}	

    if (Skill.wSlotNum <= Index_In)
    {
        ERRLOG3(g_Log, "CID:0x%08x 락 오류 : 잘못된 인덱스입니다. 스킬 개수:%d, 인덱스:%d",
            m_dwCID, Skill.wSlotNum, Index_In);
        return false;
    }

	unsigned short SkillID	= Skill.SSlot[Index_In].SKILLINFO.wSkill;
	unsigned short wError	= 0;

	if (0 != SkillID)
	{
		if (Skill.SSlot[Index_In].SKILLINFO.cSkillLevel == CSkillMgr::MAX_SKILL_LEVEL)
		{
			if (Skill.SSlot[Index_In].SKILLINFO.cLockCount != CSkillMgr::MAX_SKILL_LOCKCOUNT - 1)
			{
				++Skill.SSlot[Index_In].SKILLINFO.cLockCount;
				Skill.SSlot[Index_In].SKILLINFO.cSkillLevel	= 0;
			}
			else 
			{	
				wError = PktSk::FAIL_FULL_LOCK;			// 스킬당 락 수 초과
			}	
		}
		else
		{
			wError = PktSk::FAIL_NOT_ENOUGH_LEVEL;		// 락 할수 없는 스킬 레벨
		}
	}
	else
	{
		wError = PktSk::FAIL_NOT_SKILL;					// 해당 인덱스에 스킬 없음
	}

	if (PktBase::NO_SERVER_ERR == wError)
	{
		UpdateQuickSlotSkill(Skill.SSlot[Index_In]);

		// 스킬 시전중일때 단계를 바꾸면 스킬이 끝나면서 쿨타임이 돌도록 변경
		switch (SkillID)
		{
			case 0x8304:	// 마나셀		(법사)
				GetEnchantInfo().ResetFlag(Skill::SpellID::ManaShell);
				break;

			case 0x9204:	// 플랙서빌리티	(오피)
				GetEnchantInfo().ResetFlag(Skill::SpellID::Flexibility);
				break;

			case 0x8704:	// 스텔스		(어쌔)
			case 0x8805:	// 캐모플라쥐	(아처)
			case 0x9504:	// 캐모플라쥐	(거너)
			case 0x9804:	// 스텔스		(쉐옵)
				GetEnchantInfo().ResetFlag(Skill::SpellID::Stealth);
				break;
		}
	}

	// 패킷 보내기
    if (NULL != m_lpGameClientDispatch)
    {
        GameClientSendPacket::SendCharSkillCommand(m_lpGameClientDispatch->GetSendStream(), m_dwCID, 
            CmdCharSkillLock, Index_In, SkillID, wError);
    }

	if (wError) 
	{ 
		ERRLOG3(g_Log, "CID:0x%08x 스킬 락에 실패하였습니다. Lock Skill Index:%d, Error:%d",
			m_dwCID, Index_In, wError);
		LogSkillSlot(m_DBData);		 
	}	

	return true;
}


bool CCharacter::SkillUnLock(unsigned char Index_In, Item::ItemPos* InvenPos, bool bSkillFifthUnlock)
{
	SKILL &Skill = m_DBData.m_Skill;

	if (SKILL::MAX_SLOT_NUM < Skill.wSlotNum) 
	{
		ERRLOG2(g_Log, "CID:0x%08x 스킬 개수가 이상합니다. 슬롯 수:%d", m_dwCID, Skill.wSlotNum);
		return false;
	}	

    if (Skill.wSlotNum <= Index_In)
    {
        ERRLOG3(g_Log, "CID:0x%08x 락 해제 오류 : 잘못된 인덱스입니다. 스킬 개수:%d, 인덱스:%d",
            m_dwCID, Skill.wSlotNum, Index_In);
        return false;
    }

	unsigned short SkillID = Skill.SSlot[Index_In].SKILLINFO.wSkill;
	unsigned short wError = PktBase::NO_SERVER_ERR;

	if (0 != SkillID)
	{
		if (0 < Skill.SSlot[Index_In].SKILLINFO.cLockCount)
		{
			--Skill.SSlot[Index_In].SKILLINFO.cLockCount;
			--Skill.wSkillNum;
			if (bSkillFifthUnlock)
			{
				Skill.SSlot[Index_In].SKILLINFO.cSkillLevel	= CSkillMgr::MAX_SKILL_LEVEL;
			}
			else
			{
				Skill.SSlot[Index_In].SKILLINFO.cSkillLevel	= CSkillMgr::MAX_SKILL_LEVEL - 1;
			}
		}
		else 
		{	
			wError = PktSk::FAIL_NON_LOCK;			// 한번도 락하지 않은 스킬
		}	
	}
	else
	{
		wError = PktSk::FAIL_NOT_SKILL;				// 해당 인덱스에 스킬 없음
	}

	if (0 == wError) 
	{
		UpdateQuickSlotSkill(Skill.SSlot[Index_In]);

		// 스킬 시전중일때 단계를 바꾸면 스킬이 끝나면서 쿨타임이 돌도록 변경
		switch (SkillID)
		{
			case 0x8304:	// 마나셀		(법사)
				GetEnchantInfo().ResetFlag(Skill::SpellID::ManaShell);
				break;

			case 0x9204:	// 플랙서빌리티	(오피)
				GetEnchantInfo().ResetFlag(Skill::SpellID::Flexibility);
				break;

			case 0x8704:	// 스텔스		(어쌔)
			case 0x8805:	// 캐모플라쥐	(아처)
			case 0x9504:	// 캐모플라쥐	(거너)
			case 0x9804:	// 스텔스		(쉐옵)
				GetEnchantInfo().ResetFlag(Skill::SpellID::Stealth);
				break;
		}
	}
    else
    {
		ERRLOG3(g_Log, "CID:0x%08x 스킬 락 해제에 실패하였습니다. UnLock Skill Index:%d, Error:%d",
			m_dwCID, Index_In, wError);
		LogSkillSlot(m_DBData);
    }

	// 패킷 보내기
    if (NULL != m_lpGameClientDispatch)
    {
		GameClientSendPacket::SendCharSkillCommand(m_lpGameClientDispatch->GetSendStream(), m_dwCID, 
			CmdCharSkillUnLock, Index_In, SkillID, wError, static_cast<unsigned char>(Skill.SSlot[Index_In].SKILLINFO.cSkillLevel), InvenPos);
    }

	return true;
}

bool CCharacter::SkillFifthLock(unsigned short wSkillID)
{
	unsigned short SkillLockCount = GetSkillLockCount(wSkillID);
	unsigned short wError = PktBase::NO_SERVER_ERR;

	unsigned char Index = 0;
	bool bLockFlag = false;		// 락이 될 때는 SkillCreate 의 Ack 패킷을 보내지 않는다.

	SKILL &Skill = m_DBData.m_Skill;
	if (SKILL::MAX_SLOT_NUM < Skill.wSlotNum) 
	{
		ERRLOG2(g_Log, "CID:%10u / 스킬 생성 실패 : 스킬 개수가 이상합니다. 슬롯 수:%d", m_dwCID, Skill.wSlotNum);
		return false;
	}	

	// 인덱스 구하기
	for (Index = 0; Index < Skill.wSlotNum; ++Index) 
	{
		if (wSkillID == Skill.SSlot[Index].SKILLINFO.wSkill)
		{
			Skill.SSlot[Index].SKILLINFO.cSkillLevel = CSkillMgr::MAX_SKILL_LEVEL - 1;

			wError = ReadSkill(Skill.SSlot[Index], wSkillID, SkillLockCount);
            if (PktBase::NO_SERVER_ERR == wError)
			{
				Skill.SSlot[Index].SKILLINFO.cSkillLevel = CSkillMgr::MAX_SKILL_LEVEL;

				bLockFlag = SkillLock(Index);

				CalculateStatusData(false);

				// 퀵슬롯 업데이트
				UpdateQuickSlotSkill(Skill.SSlot[Index]);
			}

			break;
		}
	}
	
	if (PktBase::NO_SERVER_ERR != wError) 
	{ 
		ERRLOG7(g_Log, "CID:%10u 5단계 스킬 락에 실패하였습니다. 스킬수:%d/%d"
			" Create Skill Index:%d, Level:%d, Int:%d, Error:%d",
			m_dwCID, Skill.GetSkillNum(), m_CreatureStatus.m_StatusInfo.m_wSkillPoint, 
			Index, m_DBData.m_Info.Level, m_CharacterStatus.m_nINT, wError);
		LogSkillSlot(m_DBData);
		return false; 
	}	

	return true;	
}

bool CCharacter::SkillFifthUnlock(unsigned short wSkillID)
{
	unsigned short SkillLockCount = GetSkillLockCount(wSkillID);
	unsigned short wError = PktBase::NO_SERVER_ERR;

	unsigned char Index = 0;
	bool bUnlockFlag = false;		// 락이 될 때는 SkillCreate 의 Ack 패킷을 보내지 않는다.

	SKILL &Skill = m_DBData.m_Skill;

	// 인덱스 구하기
	for (Index = 0; Index < Skill.wSlotNum; ++Index) 
	{
		if (wSkillID == Skill.SSlot[Index].SKILLINFO.wSkill)
		{

			const Skill::ProtoType* lpProtoType = CSkillMgr::GetInstance().GetSkillProtoType(wSkillID);
			if (NULL == lpProtoType)
			{
				ERRLOG2(g_Log, "CID:0x%08x 스킬 아이디가 이상합니다. 스킬 아이디:0x%04x", m_dwCID, wSkillID);
				return false;
			}	

			if (Skill::Type::CHANT == lpProtoType->m_eSkillType) 
			{
				AtType attackType;
				attackType.m_wType = wSkillID;
				attackType.m_cSkillLevel = 0;
				attackType.m_cSkillLockCount = 0;
				attackType.m_cAtCount = 0;

				unsigned char cOffencerJudge = 0, cDefenserJudge = 0;
				unsigned short wOffencerMPHeal = 0, wDefenserMPHeal = 0;

				Skill::CProcessTable::GetInstance().UseSkill(attackType, this, this, cOffencerJudge, cDefenserJudge, wOffencerMPHeal, wDefenserMPHeal, wError);
			}

			bUnlockFlag = SkillUnLock(Index, NULL, true);

			CalculateStatusData(false);

			if (PktBase::NO_SERVER_ERR == wError)
			{
				UpdateQuickSlotSkill(Skill.SSlot[Index]);
			}

			//// 패킷 보내기
			//if (false == bUnlockFlag && NULL != m_lpGameClientDispatch)
			//{
			//	GameClientSendPacket::SendCharSkillCommand(m_lpGameClientDispatch->GetSendStream(), m_dwCID, 
			//		CmdCharSkillErase, Index, wSkillID, wError);
			//}

			break;
		}
	}

	if (PktBase::NO_SERVER_ERR != wError) 
	{
		ERRLOG4(g_Log, "CID:0x%08x 5단계 스킬 락 해제에 실패하였습니다. Erase Skill Index:%d, 스킬 개수:%d, Error:%d",
			m_dwCID, Index, Skill.wSlotNum, wError);
		LogSkillSlot(m_DBData);		
	}	

	return true;
}



bool CCharacter::HasSkill(unsigned short usSkillType, unsigned char cLockCount, unsigned char cLevel)
{
	const Skill::ProtoType* pSkillProtoType = CSkillMgr::GetInstance().GetSkillProtoType(usSkillType);
	if (NULL == pSkillProtoType) 
	{
		ERRLOG2(g_Log, "CID:0x%08x 존재하지 않는 스킬 아이디입니다. Skill ID:0x%04x", m_dwCID, usSkillType);
		return false;
	}

	// 자식 스킬을 사용한 경우 부모 스킬이 있는지 체크한다.
	if (0 != pSkillProtoType->m_usParentSkill)
	{
		pSkillProtoType = CSkillMgr::GetInstance().GetSkillProtoType(pSkillProtoType->m_usParentSkill);
		usSkillType = pSkillProtoType->m_usSkill_ID;
	}

	if (Skill::Type::ITEM == pSkillProtoType->m_eSkillType || 
		Skill::Type::SET == pSkillProtoType->m_eSkillType || 
		Skill::Type::ACTION == pSkillProtoType->m_eSkillType)
	{
		// 누구나 사용할 수 있는 스킬 (포션류, 인챈트의 추가 효과 스킬)
		return true;
	}

	if (true == pSkillProtoType->m_bIsClassSkill)
	{
		unsigned char cSkillClass = static_cast<unsigned char>((((pSkillProtoType->m_usSkill_ID - Skill::SKILL_MASK) & 0xFF00) >> 8) & 0x00FF);
		if (m_DBData.m_Info.Class != cSkillClass && 
			CClass::GetPreviousJob(static_cast<unsigned char>(m_DBData.m_Info.Class)) != cSkillClass)
		{
			ERRLOG3(g_Log, "CID:0x%08x 알맞지 않은 클래스 스킬을 사용하려합니다. Skill ID:0x%04x, Class:%d", 
				m_dwCID, pSkillProtoType->m_usSkill_ID, m_DBData.m_Info.Class);
			return false;
		}

		return true;
	}

	int nMaxSlotNum = m_DBData.m_Skill.wSlotNum;
	if (SKILL::MAX_SLOT_NUM < nMaxSlotNum)
	{
		ERRLOG2(g_Log, "CID:0x%08x 스킬 개수가 이상합니다. 슬롯 수 = %d",
			m_dwCID, m_DBData.m_Skill.wSlotNum);
		return false;
	}

	for (int nSSlot = 0; nSSlot < nMaxSlotNum; ++nSSlot) 
	{
		const SKILLSLOT& SkillSlot = m_DBData.m_Skill.SSlot[nSSlot];

		// 스킬 찾아서 있는 스킬인지, 스킬 레벨은 되는지 확인.
		if (usSkillType == SkillSlot.SKILLINFO.wSkill)
		{
			if ((cLockCount == SkillSlot.SKILLINFO.cLockCount && cLevel <= SkillSlot.SKILLINFO.cSkillLevel) || 
				cLockCount < SkillSlot.SKILLINFO.cLockCount)
			{ 
				return true; 
			}
		}
	}

	return false;
}



short CCharacter::GetSkillLockCount(unsigned short usSkillType)
{
	const Skill::ProtoType* lpProtoType = CSkillMgr::GetInstance().GetSkillProtoType(usSkillType);
	if (NULL == lpProtoType) { return -1; }

	// 클래스 스킬, SET 계열 스킬은 스킬을 가지고 있지 않아도 사용할 수 있다.
	if (true == lpProtoType->m_bIsClassSkill || 
		Skill::Type::SET == lpProtoType->m_eSkillType ||
		Skill::Type::ACTION == lpProtoType->m_eSkillType)
	{
		return 0;
	}

	// 부모 스킬이 있는 경우 부모 스킬의 락카운트를 이용한다.
	if (0 != lpProtoType->m_usParentSkill)
	{
		usSkillType = lpProtoType->m_usParentSkill;
	}

	int nMaxSlotNum = m_DBData.m_Skill.wSlotNum;
	if (SKILL::MAX_SLOT_NUM < nMaxSlotNum)
	{
		ERRLOG2(g_Log, "CID:0x%08x 스킬 개수가 이상합니다. 슬롯 수 = %d",
			m_dwCID, m_DBData.m_Skill.wSlotNum);
		return -1;
	}

	for (int nSSlot = 0; nSSlot < nMaxSlotNum; ++nSSlot) 
	{
		const SKILLSLOT& SkillSlot = m_DBData.m_Skill.SSlot[nSSlot];
   
		if (usSkillType == SkillSlot.SKILLINFO.wSkill)
		{
			return SkillSlot.SKILLINFO.cLockCount;
		}
	}

	return -1;
}

short CCharacter::GetSkillLevel(unsigned short usSkillType)
{
	const Skill::ProtoType* lpProtoType = CSkillMgr::GetInstance().GetSkillProtoType(usSkillType);
	if (NULL == lpProtoType) { return -1; }

	// 부모 스킬이 있는 경우 부모 스킬의 락카운트를 이용한다.
	if (0 != lpProtoType->m_usParentSkill)
	{
		usSkillType = lpProtoType->m_usParentSkill;
	}

	int nMaxSlotNum = m_DBData.m_Skill.wSlotNum;
	if (SKILL::MAX_SLOT_NUM < nMaxSlotNum)
	{
		ERRLOG2(g_Log, "CID:0x%08x 스킬 개수가 이상합니다. 슬롯 수 = %d",
			m_dwCID, m_DBData.m_Skill.wSlotNum);
		return -1;
	}

	for (int nSSlot = 0; nSSlot < nMaxSlotNum; ++nSSlot) 
	{
		const SKILLSLOT& SkillSlot = m_DBData.m_Skill.SSlot[nSSlot];

		if (usSkillType == SkillSlot.SKILLINFO.wSkill)
		{
			return SkillSlot.SKILLINFO.cSkillLevel;
		}
	}

	return -1;
}

short CCharacter::GetSkillSlotIndex(unsigned short usSkillType)
{
	const Skill::ProtoType* lpProtoType = CSkillMgr::GetInstance().GetSkillProtoType(usSkillType);
	if (NULL == lpProtoType) { return -1; }

	// 부모 스킬이 있는 경우 부모 스킬의 락카운트를 이용한다.
	if (0 != lpProtoType->m_usParentSkill)
	{
		usSkillType = lpProtoType->m_usParentSkill;
	}

	int nMaxSlotNum = m_DBData.m_Skill.wSlotNum;
	if (SKILL::MAX_SLOT_NUM < nMaxSlotNum)
	{
		ERRLOG2(g_Log, "CID:0x%08x 스킬 개수가 이상합니다. 슬롯 수 = %d",
			m_dwCID, m_DBData.m_Skill.wSlotNum);
		return -1;
	}

	for (int nSSlot = 0; nSSlot < nMaxSlotNum; ++nSSlot) 
	{
		const SKILLSLOT& SkillSlot = m_DBData.m_Skill.SSlot[nSSlot];

		if (usSkillType == SkillSlot.SKILLINFO.wSkill)
		{
			return nSSlot;
		}
	}

	return -1;
}

bool CCharacter::AddWorldWeaponEnchant(CAggresiveCreature* lpWeapon, unsigned char cNation)
{
	if (Creature::KARTERANT != cNation && Creature::MERKADIA != cNation) return false;
	if (NULL == lpWeapon) return false;

	switch (cNation)
	{
		case Creature::KARTERANT:
		{
			if (GetNation() == cNation)
			{
				Guild::CGuild* lpGuild = Guild::CGuildMgr::GetInstance().GetGuild(GetGID());
				if (lpGuild && lpGuild->IsEnemyGuild(lpWeapon->GetGID()))
				{
					// 국가는 같으나 적대 길드라면 DeBuff
					Skill::CAddSpell<CKarterantWorldDeBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWeapon, 
						Skill::SpellType::WORLDWEAPON_SPELL, Skill::SpellID::KarterantWorldDeBuff, 1, CSpell::INFINITE_DURATION))(this);
				}
				else
				{
					Skill::CAddSpell<CKarterantWorldBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWeapon, 
						Skill::SpellType::WORLDWEAPON_SPELL, Skill::SpellID::KarterantWorldBuff, 1, CSpell::INFINITE_DURATION))(this);
				}
			}
			else
			{
				Skill::CAddSpell<CKarterantWorldDeBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWeapon, 
					Skill::SpellType::WORLDWEAPON_SPELL, Skill::SpellID::KarterantWorldDeBuff, 1, CSpell::INFINITE_DURATION))(this);
			}
		}
		break;

		case Creature::MERKADIA:
		{
			if (GetNation() == cNation)
			{
				Guild::CGuild* lpGuild = Guild::CGuildMgr::GetInstance().GetGuild(GetGID());
				if (lpGuild && lpGuild->IsEnemyGuild(lpWeapon->GetGID()))
				{
					// 국가는 같으나 적대 길드라면 DeBuff
					Skill::CAddSpell<CMerkadiaWorldDeBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWeapon, 
						Skill::SpellType::WORLDWEAPON_SPELL, Skill::SpellID::MerkadiaWorldDeBuff, 1, CSpell::INFINITE_DURATION))(this);
				}
				else
				{
					Skill::CAddSpell<CMerkadiaWorldBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWeapon, 
						Skill::SpellType::WORLDWEAPON_SPELL, Skill::SpellID::MerkadiaWorldBuff, 1, CSpell::INFINITE_DURATION))(this);
				}
			}
			else
			{
				Skill::CAddSpell<CMerkadiaWorldDeBuffSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpWeapon, 
					Skill::SpellType::WORLDWEAPON_SPELL, Skill::SpellID::MerkadiaWorldDeBuff, 1, CSpell::INFINITE_DURATION))(this);
			}
		}
		break;
	}

	return true;
}

bool CCharacter::ClearWorldWeaponEnchant(void)
{
	GetSpellMgr().GetAffectedInfo().Disenchant(Skill::SpellType::WORLDWEAPON_SPELL,
		Skill::SpellTarget::ALL_ENCHANT, Skill::Disenchant::NONE, 1, Skill::Disenchant::INFINITE_NUM);

	return true;
}

void CCharacter::CheckSkillVaild(void)
{
	SKILL &Skill = m_DBData.m_Skill;

	for (int nSlotIndex = 0; nSlotIndex < Skill.wSlotNum; )
	{
		bool bEraseSkill = false;

		const Skill::ProtoType* pSkillProtoType = 
			CSkillMgr::GetInstance().GetSkillProtoType(Skill.SSlot[nSlotIndex].SKILLINFO.wSkill);
		if (NULL == pSkillProtoType)
		{
			ERRLOG2(g_Log, "CID:0x%08x 스킬 무결성 검사 : 존재하지 않는 스킬입니다. SkillID:0x%04x", 
				m_dwCID, Skill.SSlot[nSlotIndex].SKILLINFO.wSkill);

			bEraseSkill = true;
		}		

		if (Skill.SSlot[nSlotIndex].SKILLINFO.cLockCount < 0 || 
			Skill.SSlot[nSlotIndex].SKILLINFO.cLockCount >= CSkillMgr::MAX_SKILL_LOCKCOUNT)
		{
			ERRLOG3(g_Log, "CID:0x%08x 스킬 무결성 검사 : 락카운트가 비정상적입니다. SkillID:0x%04x, SkillLockCount:%d", 
				m_dwCID, Skill.SSlot[nSlotIndex].SKILLINFO.wSkill, Skill.SSlot[nSlotIndex].SKILLINFO.cLockCount);

			bEraseSkill = true;
		}

		if (Skill.SSlot[nSlotIndex].SKILLINFO.cSkillLevel < 0 || 
			Skill.SSlot[nSlotIndex].SKILLINFO.cSkillLevel > CSkillMgr::MAX_SKILL_LEVEL)
		{
			ERRLOG3(g_Log, "CID:0x%08x 스킬 무결성 검사 : 레벨이 비정상적입니다. SkillID:0x%04x, SkillLevel:%d", 
				m_dwCID, Skill.SSlot[nSlotIndex].SKILLINFO.wSkill, Skill.SSlot[nSlotIndex].SKILLINFO.cSkillLevel);

			bEraseSkill = true;
		}

		if (Skill.SSlot[nSlotIndex].SKILLINFO.cLockCount == 0 && 
			Skill.SSlot[nSlotIndex].SKILLINFO.cSkillLevel == 0)
		{
			ERRLOG2(g_Log, "CID:0x%08x 스킬 무결성 검사 : 락카운트와 레벨이 모두 0입니다. SkillID:0x%04x", 
				m_dwCID, Skill.SSlot[nSlotIndex].SKILLINFO.wSkill);

			bEraseSkill = true;
		}

		if (true == bEraseSkill)
		{
			unsigned char Count = nSlotIndex;
			for (; Count < Skill.wSlotNum - 1; ++Count) 
			{
				Skill.SSlot[Count] = Skill.SSlot[Count + 1];
			}

			Skill.SSlot[Count] = SKILLSLOT::SKILLSLOT();
			--Skill.wSlotNum;
		}
		else
		{
			++nSlotIndex;
		}
	}
}