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
		ERRLOG5(g_Log, "CID:0x%08x, ĳ���� ��ų ������ ����մϴ�. ���� %2d, ��ų ���� 0x%04x, �� ī��Ʈ %d, ��ų ���� %d",
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
	// �ӽ� �ּ� ó��. �ٸ� ������� Ŭ���� üũ.

	// ���� Ŭ���� ��ų�� �ش� �����̸� ������ ���� �� �ְ�, ���� Ŭ���� ��ų�� ��� �ش� Ŭ�������� �� ��ų�� ���� �� �ִ�.
	/*unsigned char SkillClass = static_cast<unsigned char>((((wSkillID - Skill::SKILL_MASK) & 0xFF00) >> 8) & 0x00FF);
	if (CClass::GetJobLevel(SkillClass) == CClass::JobLevel::DEFAULT_CLASS)
	{
		//// ���� Ŭ���� ��ų
		if (GetRace() != CClass::GetRace(SkillClass))
		{
			return PktSk::FAIL_NOT_CURRENT_CLASS;
		}
	}
	else if (CClass::GetJobLevel(SkillClass) == CClass::JobLevel::JOB_CHANGE_1ST)
	{
		// ���� Ŭ���� ��ų
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
			ERRLOG2(g_Log, "CID:0x%08x ���������ʴ� ��ų�� ������ �ֽ��ϴ�. SkillID:0x%04x", 
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
					// ��ų ������ ���� ���� ���̹Ƿ� ���� ��ü�� �����ϴ� �� ���Ŀ� ���� ó���Ѵ�.
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

	// ��ų ������ ����� ������ �����Ѵ�.
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

	// ������ �ٲ� �ִ� ���Լ��� �پ��� �� ����ŭ �����Ѵ�.
	short wDeleteSkillNum = Skill.GetSkillNum() - m_CreatureStatus.m_StatusInfo.m_wSkillPoint; 

	while (wDeleteSkillNum > 0)
	{
		if (1 > Skill.wSlotNum)
		{
			ERRLOG1(g_Log, "CID:0x%08x ���� ��ų ������ �����ϴ�.", m_dwCID);
			return false;
		}

		int nSlotIndex = Skill.wSlotNum - 1;
		const Skill::ProtoType* pSkillProtoType = 
			CSkillMgr::GetInstance().GetSkillProtoType(Skill.SSlot[nSlotIndex].SKILLINFO.wSkill);
		if (NULL == pSkillProtoType)
		{
			ERRLOG2(g_Log, "CID:0x%08x ���������ʴ� ��ų�� ������ �ֽ��ϴ�. SkillID:0x%04x", 
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

	// �����Ƽ ����Ʈ�� �˻��Ѵ�.
	for(int i = 0; i <= Skill.wSlotNum; ++i)
	{
		unsigned short SkillID = Skill.SSlot[i].SKILLINFO.wSkill;
		if(0x1000 <= SkillID && SkillID < 0x2000) // hz change this range when i want to add new abilities
		{
			const Skill::ProtoType* lpProtoType = CSkillMgr::GetInstance().GetSkillProtoType(SkillID);
			if (NULL == lpProtoType)
			{
				ERRLOG2(g_Log, "CID:0x%08x �����Ƽ ���̵� �̻��մϴ�. ��ų ���̵�:0x%04x", m_dwCID, SkillID);
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
        ERRLOG1(g_Log, "CID:%10u / ��ų ���� ���� : ��� �������� 0�Դϴ�", m_dwCID);
        return false;
    }

	unsigned short SkillID = lpUseItem->GetItemInfo().m_UseItemInfo.m_usSkill_ID;
	char SkillLockCount = (char)lpUseItem->GetItemInfo().m_UseItemInfo.m_usSkill_LockCount;
	unsigned short wError = PktBase::NO_SERVER_ERR;

	unsigned char Index = 0;
	bool bLockFlag = false;		// ���� �� ���� SkillCreate �� Ack ��Ŷ�� ������ �ʴ´�.

	SKILL &Skill = m_DBData.m_Skill;
	if (SKILL::MAX_SLOT_NUM < Skill.wSlotNum) 
	{
        ERRLOG2(g_Log, "CID:%10u / �����Ƽ ���� ���� : ��ų ������ �̻��մϴ�. ���� ��:%d", m_dwCID, Skill.wSlotNum);
		return false;
	}	

	const Skill::ProtoType* lpProtoType = CSkillMgr::GetInstance().GetSkillProtoType(SkillID);
	if (NULL == lpProtoType)
	{
		ERRLOG2(g_Log, "CID:0x%08x �����Ƽ ���̵� �̻��մϴ�. ��ų ���̵�:0x%04x", m_dwCID, SkillID);
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

		// ���� ��ų�̸�.
		if (Index == Skill.wSlotNum) 
		{
			if (Skill.wSlotNum == SKILL::MAX_SLOT_NUM) 
			{
				wError = PktSk::FAIL_NOT_ENOUGH_SLOT;	
			}
			else
			{
				// �����Ƽ�� ������ ����. �׳� ���� �ȴ�.
				Skill.SSlot[Index].SKILLINFO.wSkill			= SkillID;
				Skill.SSlot[Index].SKILLINFO.cLockCount		= SkillLockCount;
				Skill.SSlot[Index].SKILLINFO.cSkillLevel	= 1;
				++Skill.wSlotNum; 
			}
		}

		CalculateStatusData(false);

		// ������ ������Ʈ
		UpdateQuickSlotSkill(Skill.SSlot[Index]);
		UpdateUseAbilityPoint();
	}

	

	// ��Ŷ ������
    if (false == bLockFlag && NULL != m_lpGameClientDispatch)
    {
        GameClientSendPacket::SendCharSkillCommand(m_lpGameClientDispatch->GetSendStream(), m_dwCID, 
            CmdCharSkillCreate, Index, SkillID, wError);
    }
	
	if (PktBase::NO_SERVER_ERR != wError) 
	{ 
		ERRLOG7(g_Log, "CID:%10u �����Ƽ ������ �����Ͽ����ϴ�. ��ų��:%d/%d"
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
	bool bUnlockFlag = false;	// Unlock �ɶ��� SkillErase �� Ack ��Ŷ�� ������ �ʴ´�.

	if (SKILL::MAX_SLOT_NUM < Skill.wSlotNum)
	{
		ERRLOG2(g_Log, "CID:0x%08x �����Ƽ ������ �̻��մϴ�. ��ų ���� ��:%d", m_dwCID, Skill.wSlotNum);
		return false;
	}	

    if (Skill.wSlotNum <= Index_In)
    {
        ERRLOG3(g_Log, "CID:0x%08x �����Ƽ ���� ���� : �߸��� �ε����Դϴ�. ��ų ����:%d, �ε���:%d", m_dwCID, Skill.wSlotNum, Index_In);
        return false;
    }

	SkillID	= Skill.SSlot[Index_In].SKILLINFO.wSkill;	
	
	const Skill::ProtoType* lpProtoType = CSkillMgr::GetInstance().GetSkillProtoType(SkillID);
	if (NULL == lpProtoType)
	{
		ERRLOG2(g_Log, "CID:0x%08x �����Ƽ ���̵� �̻��մϴ�. ��ų ���̵�:0x%04x", m_dwCID, SkillID);
		return false;
	}	

	/*Item::CItem* lpItem = NULL; //hz delete ability doesnt require stone of oblivion
	if(InvenPos.m_cPos != TakeType::TS_ADMIN)
	{
		// �����ǵ� Ȯ��
		lpItem = m_Inventory.GetItem(InvenPos);
		if (NULL == lpItem)
		{
			ERRLOG4(g_Log, "CID:0x%08x �����Ƽ���� ���� : ��û�� ��ġ�� �������� �����ϴ�. SkillID:%d, Pos:%d, Index:%d", m_dwCID, SkillID, InvenPos.m_cPos, InvenPos.m_cIndex);

			GameClientSendPacket::SendCharSkillCommand(m_lpGameClientDispatch->GetSendStream(), m_dwCID, 
				CmdCharSkillErase, Index_In, SkillID, PktSk::FAIL_NO_ITEM);

			return true;
		}
		if (Item::EtcItemID::OBLIVION_STONE != lpItem->GetPrototypeID())
		{
			ERRLOG3(g_Log, "CID:0x%08x �����Ƽ���� ���� : ����Ϸ��� �������� ������ ���� �ƴմϴ�. SkillID:%d, ItemID:%d", m_dwCID, SkillID, lpItem->GetPrototypeID());

			GameClientSendPacket::SendCharSkillCommand(m_lpGameClientDispatch->GetSendStream(), m_dwCID, 
				CmdCharSkillErase, Index_In, SkillID, PktSk::FAIL_INVALID_ITEM);

			return true;
		}
		// �����ǵ� ������ �̻����� Ȯ��
		if(lpItem->GetNumOrDurability() <= 0)
		{
			ERRLOG4(g_Log, "CID:0x%08x �����Ƽ���� ���� : ������ ���� ������ �̻��մϴ�. SkillID:%d, Pos:%d, Index:%d", m_dwCID, SkillID, InvenPos.m_cPos, InvenPos.m_cIndex);

			GameClientSendPacket::SendCharSkillCommand(m_lpGameClientDispatch->GetSendStream(), m_dwCID, 
				CmdCharSkillErase, Index_In, SkillID, PktSk::FAIL_NO_ITEM);

			return true;
		}
	}*/

	if (Index_In >= Skill.wSlotNum)
	{
		wError = PktSk::FAIL_NOT_SKILL;				// �ش� �ε����� ��ų ����
	}
	else
	{
		SKILLSLOT SkillSlot = Skill.SSlot[Index_In];
		if (SkillSlot.SKILLINFO.cLockCount >= 1)
		{
			bUnlockFlag = true;

			Skill.SSlot[Index_In].SKILLINFO.cLockCount--;
			//Skill.SSlot[Index_In].SKILLINFO.cSkillLevel = 1;
			// ��Ŷ ������
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
		// �������� ���������� �����ǵ� ����
		/*if(InvenPos.m_cPos != TakeType::TS_ADMIN) //hz delete ability doesnt require stone of oblivion
		{
			// �����ǵ� ���� 1�� ����
			lpItem->SetNumOrDurability(lpItem->GetNumOrDurability() - 1);

			if (0 == lpItem->GetNumOrDurability())
			{
				if (RemoveItem(InvenPos))
				{
					DELETE_ITEM(lpItem);
				}
				else
				{
					ERRLOG4(g_Log, "CID:0x%08x �����Ƽ���� ���� : ������ ���� ������ �� �����ϴ�. SkillID:%d, Pos:%d, Index:%d", m_dwCID, SkillID, InvenPos.m_cPos, InvenPos.m_cIndex);

					m_Inventory.DumpItemInfo();

					GameClientSendPacket::SendCharSkillCommand(m_lpGameClientDispatch->GetSendStream(), m_dwCID, 
						CmdCharSkillErase, Index_In, SkillID, PktSk::FAIL_NO_ITEM);

					return true;
				}
			}
		}*/

		// ��ų�� Ȯ��
		UpdateQuickSlotSkill(Skill.SSlot[Index_In]);
	}

	// ��Ŷ ������
    if (false == bUnlockFlag && NULL != m_lpGameClientDispatch)
    {
        GameClientSendPacket::SendCharSkillCommand(m_lpGameClientDispatch->GetSendStream(), m_dwCID, 
            CmdCharSkillErase, Index_In, SkillID, wError, 0, &InvenPos);
    }

	if (PktBase::NO_SERVER_ERR != wError) 
	{
		ERRLOG5(g_Log, "CID:0x%08x ��ų ������ �����Ͽ����ϴ�. Erase Skill Index:%d, SkillID:%d, ��ų ����:%d, Error:%d", m_dwCID, Index_In, SkillID, Skill.wSlotNum, wError);

		LogSkillSlot(m_DBData);		
	}	

	return true;
}


bool CCharacter::SkillCreate(Item::CUseItem* lpUseItem)
{
    if(0 == lpUseItem)
    {
        ERRLOG1(g_Log, "CID:%10u / ��ų ���� ���� : ��� �������� 0�Դϴ�", m_dwCID);
        return false;
    }

	unsigned short SkillID = lpUseItem->GetItemInfo().m_UseItemInfo.m_usSkill_ID;
	unsigned short SkillLockCount = lpUseItem->GetItemInfo().m_UseItemInfo.m_usSkill_LockCount;
	unsigned short wError = PktBase::NO_SERVER_ERR;

	unsigned char Index = 0;
	bool bLockFlag = false;		// ���� �� ���� SkillCreate �� Ack ��Ŷ�� ������ �ʴ´�.
	
	SKILL &Skill = m_DBData.m_Skill;
	if (SKILL::MAX_SLOT_NUM < Skill.wSlotNum) 
	{
        ERRLOG2(g_Log, "CID:%10u / ��ų ���� ���� : ��ų ������ �̻��մϴ�. ���� ��:%d", m_dwCID, Skill.wSlotNum);
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
        // �ε��� ���ϱ�
		for (Index = 0; Index < Skill.wSlotNum; ++Index) 
		{
			if (SkillID == Skill.SSlot[Index].SKILLINFO.wSkill)
			{
				// ��ų ����
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
				// ���� �߰�
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

		// ������ ������Ʈ
		UpdateQuickSlotSkill(Skill.SSlot[Index]);
	}

	// ��Ŷ ������
    if (false == bLockFlag && NULL != m_lpGameClientDispatch)
    {
        GameClientSendPacket::SendCharSkillCommand(m_lpGameClientDispatch->GetSendStream(), m_dwCID, 
            CmdCharSkillCreate, Index, SkillID, wError);
    }
	
	if (PktBase::NO_SERVER_ERR != wError) 
	{ 
		ERRLOG7(g_Log, "CID:%10u ��ų ������ �����Ͽ����ϴ�. ��ų��:%d/%d"
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
	bool bUnlockFlag = false;	// Unlock �ɶ��� SkillErase �� Ack ��Ŷ�� ������ �ʴ´�.

	if (SKILL::MAX_SLOT_NUM < Skill.wSlotNum)
	{
		ERRLOG2(g_Log, "CID:0x%08x ��ų ������ �̻��մϴ�. ���� ��:%d", m_dwCID, Skill.wSlotNum);
		return false;
	}	

    if (Skill.wSlotNum <= Index_In)
    {
        ERRLOG3(g_Log, "CID:0x%08x ��ų ���� ���� : �߸��� �ε����Դϴ�. ��ų ����:%d, �ε���:%d", m_dwCID, Skill.wSlotNum, Index_In);
        return false;
    }

	SkillID	= Skill.SSlot[Index_In].SKILLINFO.wSkill;	
	
	const Skill::ProtoType* lpProtoType = CSkillMgr::GetInstance().GetSkillProtoType(SkillID);
	if (NULL == lpProtoType)
	{
		ERRLOG2(g_Log, "CID:0x%08x ��ų ���̵� �̻��մϴ�. ��ų ���̵�:0x%04x", m_dwCID, SkillID);
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
		// �����ǵ� Ȯ��
		lpItem = m_Inventory.GetItem(InvenPos);
		if (NULL == lpItem)
		{
			ERRLOG4(g_Log, "CID:0x%08x ��ų���� ���� : ��û�� ��ġ�� �������� �����ϴ�. SkillID:%d, Pos:%d, Index:%d", m_dwCID, SkillID, InvenPos.m_cPos, InvenPos.m_cIndex);

			GameClientSendPacket::SendCharSkillCommand(m_lpGameClientDispatch->GetSendStream(), m_dwCID, 
				CmdCharSkillErase, Index_In, SkillID, PktSk::FAIL_NO_ITEM);

			return true;
		}
		if (Item::EtcItemID::OBLIVION_STONE != lpItem->GetPrototypeID())
		{
			ERRLOG3(g_Log, "CID:0x%08x ��ų���� ���� : ����Ϸ��� �������� ������ ���� �ƴմϴ�. SkillID:%d, ItemID:%d", m_dwCID, SkillID, lpItem->GetPrototypeID());

			GameClientSendPacket::SendCharSkillCommand(m_lpGameClientDispatch->GetSendStream(), m_dwCID, 
				CmdCharSkillErase, Index_In, SkillID, PktSk::FAIL_INVALID_ITEM);

			return true;
		}
		// �����ǵ� ������ �̻����� Ȯ��
		if(lpItem->GetNumOrDurability() <= 0)
		{
			ERRLOG4(g_Log, "CID:0x%08x ��ų���� ���� : ������ ���� ������ �̻��մϴ�. SkillID:%d, Pos:%d, Index:%d", m_dwCID, SkillID, InvenPos.m_cPos, InvenPos.m_cIndex);

			GameClientSendPacket::SendCharSkillCommand(m_lpGameClientDispatch->GetSendStream(), m_dwCID, 
				CmdCharSkillErase, Index_In, SkillID, PktSk::FAIL_NO_ITEM);

			return true;
		}
	}


	if (Index_In >= Skill.wSlotNum)
	{
		wError = PktSk::FAIL_NOT_SKILL;				// �ش� �ε����� ��ų ����
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
					// ���� �ܰ� 0 ���� ��ų�� ����� ��� ���� �ܰ� 5 ������ ����� �־�� �Ѵ�.
					// ���⼭ ������ ���� ������ TRUE �ؼ� �Ʒ����� �����ְ��Ѵ�.

					// SkillUnLock �Լ��ȿ���bUnlockFlag�� true�϶� ��Ŷ�� ������ ������ 
					// bUnlockFlag�� true�̸� �Ʒ����� ��Ŷ�� ������ �ʴ´�.
					// ���Լ� ������ �̹� ��Ŷ�� ������ ������..
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
				wError = PktSk::FAIL_NON_LEVEL;		// ��ų ����� ����
			}
			else if (1 == SkillSlot.SKILLINFO.cSkillLevel) 
			{
				// Ŭ���� ��ų�� ��������� üũ
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
		// �������� ���������� �����ǵ� ����
		if(InvenPos.m_cPos != TakeType::TS_ADMIN)
		{
			// �����ǵ� ���� 1�� ����
			lpItem->SetNumOrDurability(lpItem->GetNumOrDurability() - 1);

			if (0 == lpItem->GetNumOrDurability())
			{
				if (RemoveItem(InvenPos))
				{
					DELETE_ITEM(lpItem);
				}
				else
				{
					ERRLOG4(g_Log, "CID:0x%08x ��ų���� ���� : ������ ���� ������ �� �����ϴ�. SkillID:%d, Pos:%d, Index:%d", m_dwCID, SkillID, InvenPos.m_cPos, InvenPos.m_cIndex);

					m_Inventory.DumpItemInfo();

					GameClientSendPacket::SendCharSkillCommand(m_lpGameClientDispatch->GetSendStream(), m_dwCID, 
						CmdCharSkillErase, Index_In, SkillID, PktSk::FAIL_NO_ITEM);

					return true;
				}
			}
		}

		// ��ų�� Ȯ��
		UpdateQuickSlotSkill(Skill.SSlot[Index_In]);
	}

	// ��Ŷ ������
    if (false == bUnlockFlag && NULL != m_lpGameClientDispatch)
    {
        GameClientSendPacket::SendCharSkillCommand(m_lpGameClientDispatch->GetSendStream(), m_dwCID, 
            CmdCharSkillErase, Index_In, SkillID, wError, 0, &InvenPos);
    }

	if (PktBase::NO_SERVER_ERR != wError) 
	{
		ERRLOG5(g_Log, "CID:0x%08x ��ų ������ �����Ͽ����ϴ�. Erase Skill Index:%d, SkillID:%d, ��ų ����:%d, Error:%d", m_dwCID, Index_In, SkillID, Skill.wSlotNum, wError);

		LogSkillSlot(m_DBData);		
	}	

	return true;
}

bool CCharacter::SkillLock(unsigned char Index_In)
{
	SKILL &Skill = m_DBData.m_Skill;

	if (SKILL::MAX_SLOT_NUM < Skill.wSlotNum) 
	{
		ERRLOG2(g_Log, "CID:0x%08x ��ų ������ �̻��մϴ�. ���� ��:%d", m_dwCID, Skill.wSlotNum);
		return false;
	}	

    if (Skill.wSlotNum <= Index_In)
    {
        ERRLOG3(g_Log, "CID:0x%08x �� ���� : �߸��� �ε����Դϴ�. ��ų ����:%d, �ε���:%d",
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
				wError = PktSk::FAIL_FULL_LOCK;			// ��ų�� �� �� �ʰ�
			}	
		}
		else
		{
			wError = PktSk::FAIL_NOT_ENOUGH_LEVEL;		// �� �Ҽ� ���� ��ų ����
		}
	}
	else
	{
		wError = PktSk::FAIL_NOT_SKILL;					// �ش� �ε����� ��ų ����
	}

	if (PktBase::NO_SERVER_ERR == wError)
	{
		UpdateQuickSlotSkill(Skill.SSlot[Index_In]);

		// ��ų �������϶� �ܰ踦 �ٲٸ� ��ų�� �����鼭 ��Ÿ���� ������ ����
		switch (SkillID)
		{
			case 0x8304:	// ������		(����)
				GetEnchantInfo().ResetFlag(Skill::SpellID::ManaShell);
				break;

			case 0x9204:	// �÷�������Ƽ	(����)
				GetEnchantInfo().ResetFlag(Skill::SpellID::Flexibility);
				break;

			case 0x8704:	// ���ڽ�		(���)
			case 0x8805:	// ĳ���ö���	(��ó)
			case 0x9504:	// ĳ���ö���	(�ų�)
			case 0x9804:	// ���ڽ�		(����)
				GetEnchantInfo().ResetFlag(Skill::SpellID::Stealth);
				break;
		}
	}

	// ��Ŷ ������
    if (NULL != m_lpGameClientDispatch)
    {
        GameClientSendPacket::SendCharSkillCommand(m_lpGameClientDispatch->GetSendStream(), m_dwCID, 
            CmdCharSkillLock, Index_In, SkillID, wError);
    }

	if (wError) 
	{ 
		ERRLOG3(g_Log, "CID:0x%08x ��ų ���� �����Ͽ����ϴ�. Lock Skill Index:%d, Error:%d",
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
		ERRLOG2(g_Log, "CID:0x%08x ��ų ������ �̻��մϴ�. ���� ��:%d", m_dwCID, Skill.wSlotNum);
		return false;
	}	

    if (Skill.wSlotNum <= Index_In)
    {
        ERRLOG3(g_Log, "CID:0x%08x �� ���� ���� : �߸��� �ε����Դϴ�. ��ų ����:%d, �ε���:%d",
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
			wError = PktSk::FAIL_NON_LOCK;			// �ѹ��� ������ ���� ��ų
		}	
	}
	else
	{
		wError = PktSk::FAIL_NOT_SKILL;				// �ش� �ε����� ��ų ����
	}

	if (0 == wError) 
	{
		UpdateQuickSlotSkill(Skill.SSlot[Index_In]);

		// ��ų �������϶� �ܰ踦 �ٲٸ� ��ų�� �����鼭 ��Ÿ���� ������ ����
		switch (SkillID)
		{
			case 0x8304:	// ������		(����)
				GetEnchantInfo().ResetFlag(Skill::SpellID::ManaShell);
				break;

			case 0x9204:	// �÷�������Ƽ	(����)
				GetEnchantInfo().ResetFlag(Skill::SpellID::Flexibility);
				break;

			case 0x8704:	// ���ڽ�		(���)
			case 0x8805:	// ĳ���ö���	(��ó)
			case 0x9504:	// ĳ���ö���	(�ų�)
			case 0x9804:	// ���ڽ�		(����)
				GetEnchantInfo().ResetFlag(Skill::SpellID::Stealth);
				break;
		}
	}
    else
    {
		ERRLOG3(g_Log, "CID:0x%08x ��ų �� ������ �����Ͽ����ϴ�. UnLock Skill Index:%d, Error:%d",
			m_dwCID, Index_In, wError);
		LogSkillSlot(m_DBData);
    }

	// ��Ŷ ������
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
	bool bLockFlag = false;		// ���� �� ���� SkillCreate �� Ack ��Ŷ�� ������ �ʴ´�.

	SKILL &Skill = m_DBData.m_Skill;
	if (SKILL::MAX_SLOT_NUM < Skill.wSlotNum) 
	{
		ERRLOG2(g_Log, "CID:%10u / ��ų ���� ���� : ��ų ������ �̻��մϴ�. ���� ��:%d", m_dwCID, Skill.wSlotNum);
		return false;
	}	

	// �ε��� ���ϱ�
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

				// ������ ������Ʈ
				UpdateQuickSlotSkill(Skill.SSlot[Index]);
			}

			break;
		}
	}
	
	if (PktBase::NO_SERVER_ERR != wError) 
	{ 
		ERRLOG7(g_Log, "CID:%10u 5�ܰ� ��ų ���� �����Ͽ����ϴ�. ��ų��:%d/%d"
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
	bool bUnlockFlag = false;		// ���� �� ���� SkillCreate �� Ack ��Ŷ�� ������ �ʴ´�.

	SKILL &Skill = m_DBData.m_Skill;

	// �ε��� ���ϱ�
	for (Index = 0; Index < Skill.wSlotNum; ++Index) 
	{
		if (wSkillID == Skill.SSlot[Index].SKILLINFO.wSkill)
		{

			const Skill::ProtoType* lpProtoType = CSkillMgr::GetInstance().GetSkillProtoType(wSkillID);
			if (NULL == lpProtoType)
			{
				ERRLOG2(g_Log, "CID:0x%08x ��ų ���̵� �̻��մϴ�. ��ų ���̵�:0x%04x", m_dwCID, wSkillID);
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

			//// ��Ŷ ������
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
		ERRLOG4(g_Log, "CID:0x%08x 5�ܰ� ��ų �� ������ �����Ͽ����ϴ�. Erase Skill Index:%d, ��ų ����:%d, Error:%d",
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
		ERRLOG2(g_Log, "CID:0x%08x �������� �ʴ� ��ų ���̵��Դϴ�. Skill ID:0x%04x", m_dwCID, usSkillType);
		return false;
	}

	// �ڽ� ��ų�� ����� ��� �θ� ��ų�� �ִ��� üũ�Ѵ�.
	if (0 != pSkillProtoType->m_usParentSkill)
	{
		pSkillProtoType = CSkillMgr::GetInstance().GetSkillProtoType(pSkillProtoType->m_usParentSkill);
		usSkillType = pSkillProtoType->m_usSkill_ID;
	}

	if (Skill::Type::ITEM == pSkillProtoType->m_eSkillType || 
		Skill::Type::SET == pSkillProtoType->m_eSkillType || 
		Skill::Type::ACTION == pSkillProtoType->m_eSkillType)
	{
		// ������ ����� �� �ִ� ��ų (���Ƿ�, ��æƮ�� �߰� ȿ�� ��ų)
		return true;
	}

	if (true == pSkillProtoType->m_bIsClassSkill)
	{
		unsigned char cSkillClass = static_cast<unsigned char>((((pSkillProtoType->m_usSkill_ID - Skill::SKILL_MASK) & 0xFF00) >> 8) & 0x00FF);
		if (m_DBData.m_Info.Class != cSkillClass && 
			CClass::GetPreviousJob(static_cast<unsigned char>(m_DBData.m_Info.Class)) != cSkillClass)
		{
			ERRLOG3(g_Log, "CID:0x%08x �˸��� ���� Ŭ���� ��ų�� ����Ϸ��մϴ�. Skill ID:0x%04x, Class:%d", 
				m_dwCID, pSkillProtoType->m_usSkill_ID, m_DBData.m_Info.Class);
			return false;
		}

		return true;
	}

	int nMaxSlotNum = m_DBData.m_Skill.wSlotNum;
	if (SKILL::MAX_SLOT_NUM < nMaxSlotNum)
	{
		ERRLOG2(g_Log, "CID:0x%08x ��ų ������ �̻��մϴ�. ���� �� = %d",
			m_dwCID, m_DBData.m_Skill.wSlotNum);
		return false;
	}

	for (int nSSlot = 0; nSSlot < nMaxSlotNum; ++nSSlot) 
	{
		const SKILLSLOT& SkillSlot = m_DBData.m_Skill.SSlot[nSSlot];

		// ��ų ã�Ƽ� �ִ� ��ų����, ��ų ������ �Ǵ��� Ȯ��.
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

	// Ŭ���� ��ų, SET �迭 ��ų�� ��ų�� ������ ���� �ʾƵ� ����� �� �ִ�.
	if (true == lpProtoType->m_bIsClassSkill || 
		Skill::Type::SET == lpProtoType->m_eSkillType ||
		Skill::Type::ACTION == lpProtoType->m_eSkillType)
	{
		return 0;
	}

	// �θ� ��ų�� �ִ� ��� �θ� ��ų�� ��ī��Ʈ�� �̿��Ѵ�.
	if (0 != lpProtoType->m_usParentSkill)
	{
		usSkillType = lpProtoType->m_usParentSkill;
	}

	int nMaxSlotNum = m_DBData.m_Skill.wSlotNum;
	if (SKILL::MAX_SLOT_NUM < nMaxSlotNum)
	{
		ERRLOG2(g_Log, "CID:0x%08x ��ų ������ �̻��մϴ�. ���� �� = %d",
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

	// �θ� ��ų�� �ִ� ��� �θ� ��ų�� ��ī��Ʈ�� �̿��Ѵ�.
	if (0 != lpProtoType->m_usParentSkill)
	{
		usSkillType = lpProtoType->m_usParentSkill;
	}

	int nMaxSlotNum = m_DBData.m_Skill.wSlotNum;
	if (SKILL::MAX_SLOT_NUM < nMaxSlotNum)
	{
		ERRLOG2(g_Log, "CID:0x%08x ��ų ������ �̻��մϴ�. ���� �� = %d",
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

	// �θ� ��ų�� �ִ� ��� �θ� ��ų�� ��ī��Ʈ�� �̿��Ѵ�.
	if (0 != lpProtoType->m_usParentSkill)
	{
		usSkillType = lpProtoType->m_usParentSkill;
	}

	int nMaxSlotNum = m_DBData.m_Skill.wSlotNum;
	if (SKILL::MAX_SLOT_NUM < nMaxSlotNum)
	{
		ERRLOG2(g_Log, "CID:0x%08x ��ų ������ �̻��մϴ�. ���� �� = %d",
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
					// ������ ������ ���� ����� DeBuff
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
					// ������ ������ ���� ����� DeBuff
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
			ERRLOG2(g_Log, "CID:0x%08x ��ų ���Ἲ �˻� : �������� �ʴ� ��ų�Դϴ�. SkillID:0x%04x", 
				m_dwCID, Skill.SSlot[nSlotIndex].SKILLINFO.wSkill);

			bEraseSkill = true;
		}		

		if (Skill.SSlot[nSlotIndex].SKILLINFO.cLockCount < 0 || 
			Skill.SSlot[nSlotIndex].SKILLINFO.cLockCount >= CSkillMgr::MAX_SKILL_LOCKCOUNT)
		{
			ERRLOG3(g_Log, "CID:0x%08x ��ų ���Ἲ �˻� : ��ī��Ʈ�� ���������Դϴ�. SkillID:0x%04x, SkillLockCount:%d", 
				m_dwCID, Skill.SSlot[nSlotIndex].SKILLINFO.wSkill, Skill.SSlot[nSlotIndex].SKILLINFO.cLockCount);

			bEraseSkill = true;
		}

		if (Skill.SSlot[nSlotIndex].SKILLINFO.cSkillLevel < 0 || 
			Skill.SSlot[nSlotIndex].SKILLINFO.cSkillLevel > CSkillMgr::MAX_SKILL_LEVEL)
		{
			ERRLOG3(g_Log, "CID:0x%08x ��ų ���Ἲ �˻� : ������ ���������Դϴ�. SkillID:0x%04x, SkillLevel:%d", 
				m_dwCID, Skill.SSlot[nSlotIndex].SKILLINFO.wSkill, Skill.SSlot[nSlotIndex].SKILLINFO.cSkillLevel);

			bEraseSkill = true;
		}

		if (Skill.SSlot[nSlotIndex].SKILLINFO.cLockCount == 0 && 
			Skill.SSlot[nSlotIndex].SKILLINFO.cSkillLevel == 0)
		{
			ERRLOG2(g_Log, "CID:0x%08x ��ų ���Ἲ �˻� : ��ī��Ʈ�� ������ ��� 0�Դϴ�. SkillID:0x%04x", 
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