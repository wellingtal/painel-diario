
#include "stdafx.h"

#include <Log/GameLog.h>
#include <Log/CharacterLog.h>
#include <Log/ItemLog.h>
#include <Log/LogStruct.h>

#include <Item/Item.h>
#include <Item/ItemFactory.h>
#include <Item/Container/ItemContainer.h>
#include <Item/Container/EquipmentsContainer.h>

#include <Network/Dispatch/DBAgent/DBAgentDispatch.h>
#include <Network/Dispatch/GameClient/GameClientDispatch.h>
#include <Network/Dispatch/GameClient/SendCharLevelUp.h>
#include <Network/Dispatch/GameClient/SendCharEtc.h>
#include <Network/Dispatch/GameClient/SendCharAttack.h>
#include <Network/Packet/PacketStruct/CharStatuspacket.h>
#include <Network/Packet/PacketCommand.h>

#include <Utility/Setup/ServerSetup.h>
#include <Utility/Math/Math.h>

#include <Community/Guild/GuildMgr.h>
#include <Community/Guild/Guild.h>

#include <Creature/Siege/SiegeConstants.h>
#include <Creature/Monster/Monster.h>
#include <Creature/Monster/AwardTable.h>

#include <Castle/CastleMgr.h>

#include <Skill/SkillTable.h>

#include <Map/FieldMap/Cell.h>

#include <GameTime/GameTimeMgr.h>

#include "ExpTable.h"
#include "Character.h"

bool CCharacter::CheckRenameWarrant(Item::ItemPos InvenPos, bool bItemAccept)
{
	// ����㰡�� Ȯ��
	Item::CItem* lpItem = NULL;
	if(InvenPos.m_cPos != TakeType::TS_ADMIN)
	{
		// �����ǵ� Ȯ��
		lpItem = m_Inventory.GetItem(InvenPos);
		if (NULL == lpItem)
		{
			ERRLOG3(g_Log, "CID:0x%08x �̸����� ���� : ��û�� ��ġ�� �������� �����ϴ�. Pos:%d, Index:%d", m_dwCID, InvenPos.m_cPos, InvenPos.m_cIndex);
			return false;
		}
		if (Item::EtcItemID::RENAME_WARRANT != lpItem->GetPrototypeID())
		{
			ERRLOG2(g_Log, "CID:0x%08x �̸����� ���� : ����Ϸ��� �������� ����㰡���� �ƴմϴ�. ItemID:%d", m_dwCID, lpItem->GetPrototypeID());
			return false;
		}
		// ����㰡���� ������ �̻����� Ȯ��
		if(lpItem->GetNumOrDurability() <= 0)
		{
			ERRLOG3(g_Log, "CID:0x%08x �̸����� ���� : ����㰡���� ������ �̻��մϴ�. Pos:%d, Index:%d", m_dwCID, InvenPos.m_cPos, InvenPos.m_cIndex);
			return false;
		}
	}

	if(bItemAccept == true)
	{
		// �������� ���������� ����㰡�� ����
		if(InvenPos.m_cPos != TakeType::TS_ADMIN)
		{
			// ����㰡�� ���� 1�� ����
			lpItem->SetNumOrDurability(lpItem->GetNumOrDurability() - 1);

			if (0 == lpItem->GetNumOrDurability())
			{
				if (RemoveItem(InvenPos))
				{
					DELETE_ITEM(lpItem);
				}
				else
				{
					ERRLOG3(g_Log, "CID:0x%08x ��ų���� ���� : ����㰡���� ������ �� �����ϴ�. Pos:%d, Index:%d", m_dwCID, InvenPos.m_cPos, InvenPos.m_cIndex);
					m_Inventory.DumpItemInfo();
					return false;
				}
			}
		}
	}

	return true;
}

bool CCharacter::CalculateAbility(const SKILL& skill)
{
	memset(m_AbilityValue, 0, sizeof(m_AbilityValue));

	// �����Ƽ ��ų ȿ��
	for (unsigned char cSlotIndex = 0; cSlotIndex < skill.wSlotNum; ++cSlotIndex) 
	{
		const SKILLSLOT& SkillSlot = skill.SSlot[cSlotIndex];

		unsigned short wSkillID = SkillSlot.SKILLINFO.wSkill;

		// ���ͳ�Ƽ�� ��ų�� ���
		const Skill::ProtoType* lpProtoType = CSkillMgr::GetInstance().GetSkillProtoType(wSkillID);
		if (NULL == lpProtoType)
			continue;

		if(lpProtoType->m_eSkillType != Skill::Type::ABILITY)
			continue;

		int cLock = SkillSlot.SKILLINFO.cLockCount;
	
		int id = wSkillID-0x1001;
		m_AbilityValue[Skill::Type::AB_ANIMAL_TALK+id] = static_cast<unsigned short>(lpProtoType[cLock].m_fMaxRange);
/*
			AB_ANIMAL_TALK		= 0,	// ����NPC�� ��ȭ
			AB_SOCIALEX			= 1,	// Ȯ�� �Ҽȸ�� ��밡��
			AB_NOFIRSTATK		= 2,	// ������ ���� �ȴ���
			AB_MOUNT_SPEEDUP	= 3,	// Ż�� �̼� ����
			AB_TEMPINVEN_TIMEUP = 4,	// �ӽ��κ��丮 �ð�����
			AB_GETUP_GOLD		= 5,	// ��� ȹ�淮 ����
			AB_RESPAWN_EX		= 6,	// Ȯ�� ������ ��� 
			AB_FOOD_UP			= 7,	// ���ķ� �ɷ�ġ ����
			AB_SHOP_MEMBERSHIP	= 8,	// ���� �����(���Ű��� �϶�, �ǸŰ��� ���)
			AB_CHEMICAL			= 9,	// ������ ȸ�������� ����
			AB_ENDUR_SHILD		= 10,	// ������ �϶� ����
			AB_RECOVERY_UP		= 11,	// HP,MP ȸ���� ����
			AB_AWARD_UP			= 12,	// ����Ʈ ���� ����
			AB_MYEQUIP_UP		= 13,	// �ڽ��� ��� ȹ���� ����
			AB_LUCK_UP			= 14,	// ���� ����
			AB_EQUIP_LEVELDOWN	= 15,	// ����� ���� ������ �ٿ��Ų��.
*/
	}
	return true;
}

bool CCharacter::CalculateStatusData(bool bFullHPandMP)
{
	// �⺻ ����
	m_CharacterStatus.Init(m_DBData.m_Info);
	m_CreatureStatus.m_StatusInfo.m_cCalculateState = FightStatus::CS_NONE;

	const unsigned char cLevel = static_cast<unsigned char>(m_DBData.m_Info.Level);
	const unsigned char cClass = static_cast<unsigned char>(m_DBData.m_Info.Class);

	m_CreatureStatus.m_StatusInfo.CalculateBaseInfo(m_CharacterStatus, cLevel, cClass);

	// �����Ƽ�� ������ش�
	CalculateAbility(m_DBData.m_Skill);

	// �������� ���������ش�.
	if(m_AbilityValue[Skill::Type::AB_RECOVERY_UP])
	{
		int Regen = m_AbilityValue[Skill::Type::AB_RECOVERY_UP];

		m_CreatureStatus.m_StatusInfo.m_wHPRegen += Regen;
		m_CreatureStatus.m_StatusInfo.m_wMPRegen += Regen;
	}


	// ��� ���� �߰� ����
	Item::CEquipment* aryEquipList[Item::EquipmentPos::MAX_EQUPMENT_POS];
	m_Equipments.GetEquipList(aryEquipList);

	bool bFirstWeaponSlot = (m_cHandPos == 1) ? false : true;
	bool bRidesSlot = (m_cRidePos == 0) ? false : true;

	// �⺻������ ���Ǵ� ��� ����
	m_CreatureStatus.m_StatusInfo.CalculateEquipInfo(const_cast<const Item::CEquipment** >(aryEquipList), 
		bFirstWeaponSlot, true, bRidesSlot, m_DBData.m_Skill, cLevel, m_AbilityValue[Skill::Type::AB_MOUNT_SPEEDUP], m_AbilityValue[Skill::Type::AB_LUCK_UP], m_AbilityValue[Skill::Type::AB_EQUIP_LEVELDOWN], m_CharacterStatus, cClass, m_EquipStatus);

	// ��Ÿ Ư���� ��쿡 ���Ǵ� ���ȵ�
	m_CreatureStatus.m_StatusInfo.CalculateEquipInfo(const_cast<const Item::CEquipment** >(aryEquipList), 
		bFirstWeaponSlot, false, bRidesSlot, m_DBData.m_Skill, cLevel, m_AbilityValue[Skill::Type::AB_MOUNT_SPEEDUP], m_AbilityValue[Skill::Type::AB_LUCK_UP], m_AbilityValue[Skill::Type::AB_EQUIP_LEVELDOWN], m_CharacterStatus, cClass, m_EtcTypeStatus[Creature::LEFT_PASSIVE_TYPE]);

	m_CreatureStatus.m_StatusInfo.CalculateEquipInfo(const_cast<const Item::CEquipment** >(aryEquipList), 
		bFirstWeaponSlot, true, bRidesSlot, SKILL(), cLevel, m_AbilityValue[Skill::Type::AB_MOUNT_SPEEDUP], m_AbilityValue[Skill::Type::AB_LUCK_UP], m_AbilityValue[Skill::Type::AB_EQUIP_LEVELDOWN], m_CharacterStatus, cClass, m_EtcTypeStatus[Creature::RIGHT_NON_PASSIVE_TYPE]);

	m_CreatureStatus.m_StatusInfo.CalculateEquipInfo(const_cast<const Item::CEquipment** >(aryEquipList), 
		bFirstWeaponSlot, false, bRidesSlot, SKILL(), cLevel, m_AbilityValue[Skill::Type::AB_MOUNT_SPEEDUP], m_AbilityValue[Skill::Type::AB_LUCK_UP], m_AbilityValue[Skill::Type::AB_EQUIP_LEVELDOWN], m_CharacterStatus, cClass, m_EtcTypeStatus[Creature::LEFT_NON_PASSIVE_TYPE]);



	// ����Ʈ�� ���� �޴� ������ ���
	CalculateStatusByQuest();

	// ��æƮ�� ���� �߰� ����
	CalculateEnchantStatus();


	if (true == bFullHPandMP)
	{
		m_CreatureStatus.m_nNowHP = m_CreatureStatus.m_StatusInfo.m_nMaxHP;
		m_CreatureStatus.m_nNowMP = m_CreatureStatus.m_StatusInfo.m_nMaxMP;
	}

	// �ִ밪���� ���簪�� �� ū ��� �ִ밪���� �������ش�.
	m_CreatureStatus.m_nNowHP = std::min(m_CreatureStatus.m_nNowHP, m_CreatureStatus.m_StatusInfo.m_nMaxHP);
	m_CreatureStatus.m_nNowMP = std::min(m_CreatureStatus.m_nNowMP, m_CreatureStatus.m_StatusInfo.m_nMaxMP);


	return true;
}

// edith 2008.02.13 ���� ��Ŷ.
bool CCharacter::ChangeClass(unsigned char cClassType)
{
	unsigned short usError = 0;

	if (cClassType >= CClass::MAX_CLASS) 
	{
		ERRLOG3(g_Log, "CID:0x%08x Ŭ���� ü���� ����. ���� ĳ������ Ŭ������ %d, �ٲٰ��� �ϴ� Ŭ������ %d�Դϴ�.",
			m_dwCID, m_DBData.m_Info.Class, cClassType);
		usError = 1;
	}

	m_DBData.m_Info.Exp = m_CreatureStatus.m_nExp;
	if (false == ClassTable[cClassType].JobChange(&m_DBData, cClassType)) 
	{
		ERRLOG3(g_Log, "CID:0x%08x Ŭ���� ü���� ����. ���� ĳ������ Ŭ������ %d, �ٲٰ��� �ϴ� Ŭ������ %d�Դϴ�.",
			m_dwCID, m_DBData.m_Info.Class, cClassType);	
		usError = 1;
	}
	m_CreatureStatus.Init(m_DBData.m_Info);

	// ���� ���� ������Ʈ
	CGuild* lpGuild = Guild::CGuildMgr::GetInstance().GetGuild(GetGID());
	if (0 != lpGuild)
	{
		lpGuild->UpdateMemberInfo(m_dwCID, cClassType, Guild::TYPE_CLASS);
	}

    // ��Ƽ�� Ŭ������ ����Ǿ����� ����
    if (0 != m_pParty)
    {
        CCharacterParty* lpParty = static_cast<CCharacterParty*>(m_pParty);
        lpParty->SendPartyMemberDataToDBAgent(m_dwCID, 0, cClassType, 0, 0, 
            GetCharacterName(), PktDD::SCmdChangeClassPartyMem);
    }

	// ģ�� ����Ʈ�� Ŭ���� ���� �޼����� ������ //	
	FriendInfoUpdate(GetUID(), GetCID(), GetGID(), GetClass(), GetLevel(), CServerSetup::GetInstance().GetServerID());	

    if (0 != m_lpGameClientDispatch && 
        !GameClientSendPacket::SendCharClassUpgrade(
        m_lpGameClientDispatch->GetSendStream(), m_dwCID, m_DBData, usError) && 
        0 != usError)
    {
		return false;
    }

	return CalculateStatusData(true);
}


// Desc : Admin ������� ������ �ٿ� ��ų�� ����ϴ� �Լ�
//        ���� 1�� �ʱ� Ŭ������ ����� ������.
bool CCharacter::InitLevel1Char(unsigned char cClassType)
{
	if (cClassType >= CClass::MAX_CLASS) 
	{
		ERRLOG3(g_Log, "CID:0x%08x Ŭ���� ü���� ����. ���� ĳ������ Ŭ������ %d, �ٲٰ��� �ϴ� Ŭ������ %d�Դϴ�.",
				m_dwCID, m_DBData.m_Info.Class, cClassType);
		return false;
	}

	m_DBData.m_Info.Exp = m_CreatureStatus.m_nExp;
	if (false == ClassTable[cClassType].InitializeClass(&m_DBData, cClassType))
	{
		ERRLOG3(g_Log, "CID:0x%08x Ŭ���� �ʱ�ȭ ����. ���� ĳ������ Ŭ������ %d, �ٲٰ��� �ϴ� Ŭ������ %d�Դϴ�.",
				m_dwCID, m_DBData.m_Info.Class, cClassType);
		return false;
	}
	m_CreatureStatus.Init(m_DBData.m_Info);

	if (NULL != m_lpGameClientDispatch)
	{
		if (false == GameClientSendPacket::SendCharClassUpgrade(m_lpGameClientDispatch->GetSendStream(),
						m_dwCID, m_DBData, PktBase::NO_SERVER_ERR))
		{
			return false;
		}
	}

	return CalculateStatusData(true);
}



/*bool CCharacter::GetHuntingExp(CAggresiveCreature* lpDeadCreature, unsigned long dwExp, unsigned char cMemberNum)
{
// Edith party experience
// Experienced by the party that bonus cap
	static const float fExpCapPartyBonus[PARTY::MAX_MEM] = {
//		1.0f, 1.1f, 1.2f, 1.3f, 1.4f, 1.5f, 1.6f, 1.7f, 1.8f, 1.9f
//		1.0f, 1.2f, 1.4f, 1.6f, 1.8f, 2.0f, 2.2f, 2.4f, 2.6f, 2.8f
		1.0f, 1.2f, 1.4f, 1.6f, 1.8f, 2.0f, 2.2f, 2.4f, 2.6f, 2.8f
	};

	float fPartyExpCap = fExpCapPartyBonus[cMemberNum -1]; // was -1

	// Edith when the parties signed the receive bonus experience points.
	dwExp = static_cast<unsigned long>(dwExp * fPartyExpCap);

	// ������ ĸ ����
	const float fExpCapRevision = EXP::ExpCapTable[m_CreatureStatus.m_nLevel - 1] * 
		(CServerSetup::GetInstance().GetExpDefault() / 100.0f);

//-------
// TEST EXP ALEX PT
	
	// edith 2008.06.30
    // If we took the party monster, the experience gives the reward.
	if (NULL != lpDeadCreature->GetParty())
	{
		// Compensation for the difficulty level of the / group (party)
		CMonsterParty* lpParty = reinterpret_cast<CMonsterParty* >(lpDeadCreature->GetParty());

		switch (lpParty->GetMemberTypeNum())
		{
			case 1: dwExp += static_cast<unsigned long>(dwExp * 5.06f);	break;
			case 2: dwExp += static_cast<unsigned long>(dwExp * 6.12f);	break;
			case 3: dwExp += static_cast<unsigned long>(dwExp * 7.18f);	break;
			case 4: dwExp += static_cast<unsigned long>(dwExp * 8.24f);	break;
			case 5: dwExp += static_cast<unsigned long>(dwExp * 9.3f);	break;
		}
	}

//-------
	dwExp = std::max(dwExp, static_cast<unsigned long>(1));
	dwExp = static_cast<unsigned long>(dwExp * (CServerSetup::GetInstance().GetExpDefault() / 100.0f));

	if (dwExp > fExpCapRevision)
	{
		//----

		RULLOG9(g_Log, "CID:0x%08x ȹ�� ����ġ�� ������ ĸ�� �ʰ��Ͽ����ϴ�. �����:%d "
			"���� : %d%%, ȹ�� ����ġ : %ld, ������ ĸ(����) : %.1f (%d), Ŭ���� : %d, ���ص� �༮(����) : 0x%08x (%d)", 
			m_dwCID, cMemberNum, CServerSetup::GetInstance().GetExpDefault(), 
			dwExp, fExpCapRevision, m_CreatureStatus.m_nLevel, m_DBData.m_Info.Class, 
			lpDeadCreature->GetCID(), lpDeadCreature->GetStatus().m_nLevel);

		//----
		dwExp = static_cast<unsigned long>(fExpCapRevision);
	}

	// ���� ����Ʈ ���ʽ��� ���� �߰� ������ (ĸ�� ������ ���� �ʴ´�)
	char eliteBonus = GetEliteBonus();
	if (0 < eliteBonus)
	{
		dwExp = static_cast<unsigned long>(dwExp * EliteBonus::usBonusExp[eliteBonus - 1] / 100.0f);
	}

	// ��ȯ���� �ִ� ��� 90%�� ����ġ���� �޴´�.
	if (NULL != m_lpSummonee)
	{
		dwExp = static_cast<unsigned long>(dwExp * 0.9f);
	}

	unsigned long dwAddExp = 0;
	unsigned long dwAddDrop = 0;
	float fPt = 0.0f;
	float fAddDropPt = 0.0f;
	// 'Of the experience' use
	if (true == GetEnchantInfo().GetFlag(Skill::SpellID::ExpOrb))
	{
		// ���� 50% ���
		fPt += 0.3f; // EFECT 1 ORA MORE 30% EXPERIENCE  // was fPt += 0.3f;
	}

	//----------------
	
	if (true == GetEnchantInfo().GetFlag(Skill::SpellID::MichaelBless))
	{
		fPt += 1.0f; // EFECT 12 ORA MORE 100% EXPERIENCE  // was fPt += 0.3f;
		fAddDropPt += 1.0f;
	}

	//----------------

	// �����̾� ���񽺽� ����ġ �߰�����
	if(CheckPremiumTime())
	{
		fPt += GetPremiumPt();
	}

	// '������ ����' ����.
	if (true == GetEnchantInfo().GetFlag(Skill::SpellID::ExpStatue))
	{			
		unsigned short wLevel = GetEnchantLevel(Skill::SpellID::ExpStatue);
		fPt += (wLevel*0.03f); // was fPt += (wLevel*0.01f); ( new 30# exprate buff)
	}

	// ������ �������� ���� �ݰ� xxx m ������ ��ɽ� ����ġ 10%�� �� ȹ��
	if (true == CServerSetup::GetInstance().UseContents(GameRYL::SIEGE))
	{
		// ������ �����̱� ������.. 
		// ������, ������ �ð��� �ƴҶ���,
		if (!CGameTimeMgr::GetInstance().IsSiegeWarTime())
		{
			int iMyCastleNum = Castle::CCastleMgr::GetInstance().GetCastleNum(
				GetNation(), CServerSetup::GetInstance().GetServerZone(), GetCurrentPos() );

			if (iMyCastleNum > 0)
			{
				fPt += (iMyCastleNum*0.01f);
			}
		}
	}

	// edith 2010.01.02 ���� ���� ��尡������ ���� ���� ���ʽ� ����Ʈ �߰�
	CGuild* lpGuild = Guild::CGuildMgr::GetInstance().GetGuild(GetGID());
	if (0 != lpGuild)
	{
		float fD = lpGuild->GetGuildBonus(GetRealmPoint());

		fPt += fD;
	}

	if(fPt > 0.0f)
		dwAddExp = static_cast<unsigned long>((float)dwExp*fPt);

	return IncrementExp(dwExp + dwAddExp);
}*/
bool CCharacter::GetHuntingExp(CAggresiveCreature* lpDeadCreature, unsigned long dwExp, unsigned char cMemberNum)  // Blackdog
{

	static const float fExpCapPartyBonus[PARTY::MAX_MEM] = {
		1.0f, 1.4f, 1.8f, 2.2f, 2.6f, 3.0f, 3.4f, 3.8f, 4.2f, 4.6f
	};

	float fPartyExpCap = fExpCapPartyBonus[cMemberNum -1];

	
	dwExp = static_cast<unsigned long>(dwExp * fPartyExpCap);

	
	const float fExpCapRevision = EXP::ExpCapTable[m_CreatureStatus.m_nLevel - 1] * 
		(CServerSetup::GetInstance().GetExpDefault() / 100.0f);

	//char eliteBonus = GetEliteBonus();
	//dwExp = std::max(dwExp, static_cast<unsigned long>(1));
	//dwExp = static_cast<unsigned long>(dwExp * EliteBonus::usBonusExp[eliteBonus - 1] * (CServerSetup::GetInstance().GetExpDefault() / 10.0f));
	dwExp = std::max(dwExp, static_cast<unsigned long>(1));
	dwExp = static_cast<unsigned long>(dwExp * ((lpDeadCreature->GetStatus().m_nLevel)*2) * (CServerSetup::GetInstance().GetExpDefault() / 100.0f));
	

	if (dwExp > fExpCapRevision)
	{
     /// here is takeoff button start
		RULLOG9(g_Log, "CID:0x%08x huntingexp:%d "
			"gethunting : %d%%, exp : %ld,  : %.1f (%d),  : %d,  : 0x%08x (%d)", 
			m_dwCID, cMemberNum, CServerSetup::GetInstance().GetExpDefault(), 
			dwExp, fExpCapRevision, m_CreatureStatus.m_nLevel, m_DBData.m_Info.Class, 
			lpDeadCreature->GetCID(), lpDeadCreature->GetStatus().m_nLevel);
  /// here is takeoff button start
		dwExp = static_cast<unsigned long>(fExpCapRevision);
	}


	char eliteBonus = GetEliteBonus();
	if (0 < eliteBonus)
	{
		dwExp = static_cast<unsigned long>(dwExp * EliteBonus::usBonusExp[eliteBonus - 1] / 100.0f);
	}


	if (NULL != m_lpSummonee)
	{
		dwExp = static_cast<unsigned long>(dwExp * 0.9f);
	}

	unsigned long dwAddExp = 0;
	float fPt = 0.0f;
	
	if (true == GetEnchantInfo().GetFlag(Skill::SpellID::ExpOrb))
	{
		
		fPt += 0.3f;
	}

	
	if(CheckPremiumTime())
	{
		fPt += GetPremiumPt();
	}

	
	if (true == GetEnchantInfo().GetFlag(Skill::SpellID::ExpStatue))
	{			
		unsigned short wLevel = GetEnchantLevel(Skill::SpellID::ExpStatue);
		fPt += (wLevel*0.01f);
	}

	
	if (true == CServerSetup::GetInstance().UseContents(GameRYL::SIEGE))
	{
		
		if (!CGameTimeMgr::GetInstance().IsSiegeWarTime())
		{
			int iMyCastleNum = Castle::CCastleMgr::GetInstance().GetCastleNum(
				GetNation(), CServerSetup::GetInstance().GetServerZone(), GetCurrentPos() );

			if (iMyCastleNum > 0)
			{
				fPt += (iMyCastleNum*0.01f);
			}
		}
	}

	
	CGuild* lpGuild = Guild::CGuildMgr::GetInstance().GetGuild(GetGID());
	if (0 != lpGuild)
	{
		float fD = lpGuild->GetGuildBonus(GetRealmPoint());

		fPt += fD;
	}

	if(fPt > 0.05f)
		dwAddExp = static_cast<unsigned long>((float)dwExp*fPt);

	return IncrementExp(dwExp + dwAddExp);
}


bool CCharacter::IncrementExp(unsigned long dwExp)
{
	// �߱��̸�
	float fPt = GetAwardPer();
	dwExp = (unsigned long)((float)dwExp*fPt);

	// ���� ĳ���� ���� ���� ����� �� ������ �˻�.
	if (m_CreatureStatus.m_nLevel > EXP::GetUsingMaxLevel() - 1|| m_CreatureStatus.m_nLevel <= 0) 
	{ 
		if (m_CreatureStatus.m_nLevel == EXP::GetUsingMaxLevel())
		{
			return true;
		}

		ERRLOG2(g_Log, "CID:0x%08x �������� �� �� �����ϴ� : ���� ĳ������ ���� ���� �̻��մϴ�."
			" ���� ���� %d�Դϴ�.", m_dwCID, m_CreatureStatus.m_nLevel);
		return false;
	}

	// ���� Ŭ������ �������� Ȯ��.
	if (m_DBData.m_Info.Class >= CClass::MAX_CLASS) 
	{
		ERRLOG2(g_Log, "CID:0x%08x �������� �� �� �����ϴ� : ���� ĳ������ Ŭ������ �̻��մϴ�."
			" Ŭ���� ���� %d�Դϴ�.", m_dwCID, m_DBData.m_Info.Class);
		return false;
	}

	if (NULL != m_lpGameClientDispatch)
	{
		GameClientSendPacket::SendCharAward(m_lpGameClientDispatch->GetSendStream(), 
			m_dwCID, dwExp);
	}

	// ���� Exp�� ������ �� �� �ִ����� �˻�.
	m_CreatureStatus.m_nExp += dwExp;		
	if (m_CreatureStatus.m_nExp < EXP::ExpTable[m_CreatureStatus.m_nLevel - 1])
	{
		// ���� ����ġ�� ���ڶ� �������� �� �� ����.
		return true;
	}

	// �������� �ѹ��� �ΰ� �̻� �Ϸ����� Ȯ��.
	if (m_CreatureStatus.m_nExp - EXP::ExpTable[m_CreatureStatus.m_nLevel - 1] >= EXP::ExpTable[m_CreatureStatus.m_nLevel]) 
	{
		RULLOG2(g_Log, "CID:0x%08x �ѹ��� 2���� �̻� �������� �Ϸ� �մϴ�. �������� ����:%d", m_dwCID, m_CreatureStatus.m_nLevel);

		m_DBData.m_Info.Exp = m_CreatureStatus.m_nExp = EXP::ExpTable[m_CreatureStatus.m_nLevel] - 1;
	}
	else
	{
		m_DBData.m_Info.Exp = m_CreatureStatus.m_nExp = m_CreatureStatus.m_nExp - EXP::ExpTable[m_CreatureStatus.m_nLevel - 1];
	}

	++m_DBData.m_Info.Level;
	++m_CreatureStatus.m_nLevel;		

	// Ŭ������ ���� �������ͽ� �ڵ� ����. IP�� 2�� �÷� ��.
	ClassTable[m_DBData.m_Info.Class].LevelUp(&m_DBData.m_Info);

	m_DBData.m_Info.HP = m_CreatureStatus.m_nNowHP = m_CreatureStatus.m_StatusInfo.m_nMaxMP;
	m_DBData.m_Info.MP = m_CreatureStatus.m_nNowMP = m_CreatureStatus.m_StatusInfo.m_nMaxMP;

	// ���� ���� ������Ʈ
	CGuild* lpGuild = Guild::CGuildMgr::GetInstance().GetGuild(GetGID());
	if (0 != lpGuild)
	{
		lpGuild->UpdateMemberInfo(m_dwCID, m_CreatureStatus.m_nLevel, Guild::TYPE_LEVEL);
	}

    // ��Ƽ�� ������������ ����    
    if (0 != m_pParty)
    {
        CCharacterParty* lpParty = static_cast<CCharacterParty*>(m_pParty);
        lpParty->SendPartyMemberDataToDBAgent(m_dwCID, 0, 0, 0, m_CreatureStatus.m_nLevel,
            GetCharacterName(), PktDD::SCmdLevelUpPartyMem);
    }

	// ģ�� ����Ʈ�� ������ �޼����� ������ //
	FriendInfoUpdate(GetUID(), GetCID(), GetGID(), GetClass(), GetLevel(), CServerSetup::GetInstance().GetServerID());

	// ������ �̺�Ʈ
	// edith 2009.09.05 �ű� �̺�Ʈ ���ۺκ�
	if (CServerSetup::GetInstance().GetLevelUpEvent() && 0 != m_CellPos.m_lpCell)
	{
		GET_SINGLE_DISPATCH(lpDBAgentDispatch, 
			CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());

		if (0 == lpDBAgentDispatch)
		{
			ERRLOG0(g_Log, "�̺�Ʈ�α� : ������Ʈ ��� ����.");                            
		}
		else if(m_DBData.m_Info.Level == 30)	// �̺κ� �̺�Ʈ�� 30������ �̺�Ʈ 
		{
			// �������� ���õ� �̺�Ʈ ����
			GameClientSendPacket::SendExtraEvent(lpDBAgentDispatch->GetSendStream(), GetUID(),
				GetCID(), MAKEWPARAM(1,30), 0, 0, 0);
		}
		else if(m_DBData.m_Info.Level == 60)	// �̺κ� �̺�Ʈ�� 30������ �̺�Ʈ 
		{
			// �������� ���õ� �̺�Ʈ ����
			GameClientSendPacket::SendExtraEvent(lpDBAgentDispatch->GetSendStream(), GetUID(),
				GetCID(), MAKEWPARAM(1,60), 0, 0, 0);
		}
		else if(m_DBData.m_Info.Level == 90)	// �̺κ� �̺�Ʈ�� 30������ �̺�Ʈ 
		{
			// �������� ���õ� �̺�Ʈ ����
			GameClientSendPacket::SendExtraEvent(lpDBAgentDispatch->GetSendStream(), GetUID(),
				GetCID(), MAKEWPARAM(1,90), 0, 0, 0);
		}
	}
/*
	if (CServerSetup::GetInstance().GetLevelUpEvent() && 0 != m_CellPos.m_lpCell)
	{
		unsigned short wItemKind = AwardTable::CAward::GetInstance().GetAwardEquipment(
			Item::EquipType::STANDARD_OPTION, m_DBData.m_Info.Level, this, true);

		Item::CItem* lpItem = Item::CItemFactory::GetInstance().CreateItem(wItemKind);
		Item::CEquipment* lpEquip = Item::CEquipment::DowncastToEquipment(lpItem);
	    
		if (NULL != lpEquip)
		{
			lpEquip->AddRandomOption(Item::EquipType::F_GRADE, 0);
			lpEquip->SetNewEquip();
		}

		GiveItem(lpItem);

		// GievItem ���� ���õ� ���
		if (lpItem->IsSet(Item::DetailData::STACKABLE) && 0 == lpItem->GetNumOrDurability())
		{
			DELETE_ITEM(lpItem);
		}
	}
*/
	// ĳ���� ���� �� �α׸� �����.
	GAMELOG::LogCharLevelUp(*this, m_DBData.m_Info.IP, m_DBData.m_Info.Level, 0);

    if (0 != m_lpGameClientDispatch)
    {
        // �������ϴ� ������� ������ �̺�Ʈ�� ������.
        GameClientSendPacket::SendCharLevelUp(
            m_lpGameClientDispatch->GetSendStream(), m_dwCID, m_DBData);
    }

    // �ֺ� �ι��鿡�� ������ �̺�Ʈ ������ ������. 
    // ������ ������ ����Ʈ�� �ѷ� �ش�.
    if(0 != m_CellPos.m_lpCell)
    {
        PktLUInfo pktLUInfo;
        memset(&pktLUInfo, 0, sizeof(PktLUInfo));

        pktLUInfo.m_dwCharID = m_dwCID;
        pktLUInfo.m_cLevel   = m_DBData.m_Info.Level;

        m_CellPos.m_lpCell->SendAllNearCellCharacter(
            &pktLUInfo, sizeof(PktLUInfo), CmdCharLevelUpInfo);
    }

/*
	// edith 100�� �޼� ����Ʈ
	if (true == CServerSetup::GetInstance().UseContents(GameRYL::LEVEL_LIMIT_100))
	{
		if(100 == m_DBData.m_Info.Level)
		{
			Skill::CAddSpell<CHundredLevelSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, NULL, 
				Skill::SpellType::ETERNAL_SPELL, Skill::SpellID::HundredLevel, 1, CSpell::INFINITE_DURATION))(this);
		}
	}
*/
    DBUpdateForce(DBUpdateData::UPDATE);
	return CalculateStatusData(true);
}


bool CCharacter::RegenHPAndMP(unsigned short usAdditionalHP, unsigned short usAdditionalMP, bool bAddDefaultRegenValue)
{
    if (false == CAggresiveCreature::RegenHPAndMP(usAdditionalHP, usAdditionalMP, bAddDefaultRegenValue)) {
		return false;
	}

    if (NULL != m_lpGameClientDispatch)
    {
        return GameClientSendPacket::SendCharHPRegen(m_lpGameClientDispatch->GetSendStream(), 
            m_dwCID, m_CreatureStatus.m_nNowHP, m_CreatureStatus.m_nNowMP);
    }

	return true;
}


unsigned short CCharacter::AddState(unsigned char Type_In, ChState& ChState_Out)
{ 
	unsigned char cDecrementIP = CClass::GetRequiredIP(m_DBData.m_Info.Class, Type_In);
    
	if (m_DBData.m_Info.IP < cDecrementIP) 
	{
		return 2;
	}

	switch(Type_In)	
	{
		case CClass::STR: ++m_DBData.m_Info.STR; break;
		case CClass::DEX: ++m_DBData.m_Info.DEX; break;
		case CClass::CON: ++m_DBData.m_Info.CON; break;
		case CClass::INT: ++m_DBData.m_Info.INT; break;
		case CClass::WIS: ++m_DBData.m_Info.WIS; break;
	}

	m_DBData.m_Info.IP -= cDecrementIP;

	ChState_Out.m_wIP	= m_DBData.m_Info.IP;
	ChState_Out.m_wSTR	= m_DBData.m_Info.STR;
	ChState_Out.m_wDEX	= m_DBData.m_Info.DEX;
	ChState_Out.m_wCON	= m_DBData.m_Info.CON;
	ChState_Out.m_wINT	= m_DBData.m_Info.INT;
	ChState_Out.m_wWIS	= m_DBData.m_Info.WIS;

	CalculateStatusData(false);
	return 0;
}


bool CCharacter::StateRedistribution(ChState& State)
{
	unsigned short usError = PktSRAck::NO_SERVER_ERR;
/*
	if (0 == m_DBData.m_Info.Chance)
	{
		usError = PktSRAck::FAIL_NO_CHANCE;
	}
	else
*/	{
		int CheckStat = State.m_wIP+State.m_wSTR+State.m_wDEX+State.m_wCON+State.m_wINT+State.m_wWIS;
		if(CheckStat >= 10000)
		{
			ERRLOG9(g_Log, "CID:0x%08x ��й�� ������ �ùٸ��� �ʽ��ϴ�. "
				"Ŭ����:%d, ����:%d, STR:%d, DEX:%d, CON:%d, INT:%d, WIS:%d IP:%d",
				m_dwCID, m_DBData.m_Info.Class, m_DBData.m_Info.Level, 
				State.m_wSTR, State.m_wDEX, State.m_wCON, State.m_wINT, State.m_wWIS, State.m_wIP);
			return false;
		}

		if (false == ClassTable[m_DBData.m_Info.Class].CheckState(State, m_DBData.m_Info.Level))
		{
			ERRLOG9(g_Log, "CID:0x%08x ��й�� ������ �ùٸ��� �ʽ��ϴ�. "
				"Ŭ����:%d, ����:%d, STR:%d, DEX:%d, CON:%d, INT:%d, WIS:%d IP:%d",
				m_dwCID, m_DBData.m_Info.Class, m_DBData.m_Info.Level, 
				State.m_wSTR, State.m_wDEX, State.m_wCON, State.m_wINT, State.m_wWIS, State.m_wIP);
			return false;
		}

		const CHAR_INFOST tempInfoSt = m_DBData.m_Info;

		m_DBData.m_Info.IP = State.m_wIP;
		m_DBData.m_Info.STR = State.m_wSTR;
		m_DBData.m_Info.DEX = State.m_wDEX;
		m_DBData.m_Info.CON = State.m_wCON;
		m_DBData.m_Info.INT = State.m_wINT;
		m_DBData.m_Info.WIS = State.m_wWIS;

		// ��й�� ���ȿ� ���� ��ų ������
		if (false == RedistributionSkill())
		{
			ERRLOG1(g_Log, "CID:0x%08x ��ų ��й迡 �����Ͽ����ϴ�.", m_dwCID);
			m_DBData.m_Info = tempInfoSt;
			return false;	
		}

		// ������ �ʱ�ȭ
		m_DBData.m_Quick = QUICK::QUICK();

//		m_DBData.m_Info.Chance--;
		m_DBData.m_Info.Chance = 1;

		DETLOG2(g_Log, "CID:%u �� ĳ���Ͱ� ���� ��й� ��ȸ�� ����Ͽ����ϴ�. (����� ���� Chance: %d)",
			m_dwCID, m_DBData.m_Info.Chance);
	}

	if (NULL != m_lpGameClientDispatch)
	{
		if (false == GameClientSendPacket::SendCharStateRedistribution(m_lpGameClientDispatch->GetSendStream(), 
			m_dwCID, GetState(), GetSkill(), usError)) 
		{
			return false;
		}
	}

	return CalculateStatusData(true);
}

// ������ ������ �����޴� ó���� �ϴ� �Լ� (������ �� ���)
bool CCharacter::StatusRetrain(ChState& State, Item::ItemPos InvenPos)
{
	unsigned short usError = PktSRTAck::NO_SERVER_ERR;

	if (State.m_wIP - m_DBData.m_Info.IP > PktSRTAck::MAX_STATUS_RETRAIN)
	{
		usError = PktSRTAck::FAIL_INVALID_IP;
		goto SEND_PACKET;
	}
	
	// ��Ȱ�� ��
	Item::CItem* lpItem = m_Inventory.GetItem(InvenPos);
	if (NULL == lpItem)
	{
		ERRLOG2(g_Log, "�������ͽ� ���Ʒ� ���� : ��û�� ��ġ�� �������� �����ϴ�. Pos:%d, Index:%d", 
			InvenPos.m_cPos, InvenPos.m_cIndex);

		usError = PktSRTAck::FAIL_NO_ITEM;
		goto SEND_PACKET;
	}

	if (Item::EtcItemID::REBIRTH_STONE != lpItem->GetPrototypeID())
	{
		ERRLOG1(g_Log, "�������ͽ� ���Ʒ� ���� : ����Ϸ��� �������� ��Ȱ�� ���� �ƴմϴ�. ItemID:%d", 
			lpItem->GetPrototypeID());

		usError = PktSRTAck::FAIL_INVALID_ITEM;
		goto SEND_PACKET;
	}
/*
	unsigned long dwReduceGold = m_CreatureStatus.m_nLevel * 10000;
	if (dwReduceGold > m_DBData.m_Info.Gold)
	{
		ERRLOG2(g_Log, "�������ͽ� ���Ʒ� ���� : ���� �����մϴ�. �䱸�ݾ�:%ld, ������:%ld", 
			dwReduceGold, m_DBData.m_Info.Gold);
		
		usError = PktSRTAck::FAIL_NOT_ENOUGH_GOLD;
		goto SEND_PACKET;
	}
*/
	int CheckStat = State.m_wIP+State.m_wSTR+State.m_wDEX+State.m_wCON+State.m_wINT+State.m_wWIS;
	if(CheckStat >= 10000)
	{
		ERRLOG9(g_Log, "CID:0x%08x ��й�� ������ �ùٸ��� �ʽ��ϴ�. "
			"Ŭ����:%d, ����:%d, STR:%d, DEX:%d, CON:%d, INT:%d, WIS:%d IP:%d",
			m_dwCID, m_DBData.m_Info.Class, m_DBData.m_Info.Level, 
			State.m_wSTR, State.m_wDEX, State.m_wCON, State.m_wINT, State.m_wWIS, State.m_wIP);

		usError = PktSRTAck::FAIL_INVALID_STATUS;
		goto SEND_PACKET;
	}

	// IP �� ��ġ�� �ùٸ��� üũ
	if (false == ClassTable[m_DBData.m_Info.Class].CheckState(State, m_DBData.m_Info.Level))
	{
		ERRLOG9(g_Log, "CID:0x%08x ��й�� ������ �ùٸ��� �ʽ��ϴ�. "
			"Ŭ����:%d, ����:%d, STR:%d, DEX:%d, CON:%d, INT:%d, WIS:%d IP:%d",
			m_dwCID, m_DBData.m_Info.Class, m_DBData.m_Info.Level, 
			State.m_wSTR, State.m_wDEX, State.m_wCON, State.m_wINT, State.m_wWIS, State.m_wIP);

		usError = PktSRTAck::FAIL_INVALID_STATUS;
		goto SEND_PACKET;
	}

	// �ּ� ���� üũ
	if (false == ClassTable[m_DBData.m_Info.Class].CheckMinState(State, m_DBData.m_Info.Level))
	{
		ERRLOG9(g_Log, "CID:0x%08x ��й�� ������ �ùٸ��� �ʽ��ϴ�. "
			"Ŭ����:%d, ����:%d, STR:%d, DEX:%d, CON:%d, INT:%d, WIS:%d IP:%d",
			m_dwCID, m_DBData.m_Info.Class, m_DBData.m_Info.Level, 
			State.m_wSTR, State.m_wDEX, State.m_wCON, State.m_wINT, State.m_wWIS, State.m_wIP);

		usError = PktSRTAck::FAIL_INVALID_STATUS;
		goto SEND_PACKET;
	}	

	if (usError == PktSRTAck::NO_SERVER_ERR)
	{
        const CHAR_INFOST tempInfoSt = m_DBData.m_Info;

		// �������ͽ� ���Ʒ� ������ ������ ���� �� �� ����
		if (0 < lpItem->GetNumOrDurability())
		{
			lpItem->SetNumOrDurability(lpItem->GetNumOrDurability() - 1);
		}
		else
		{
			ERRLOG2(g_Log, "CID:0x%08x �������ͽ� ���Ʒ� ���� : ��Ȱ�� ���� ������ �̻��մϴ�. Pos:%d, Index:%d", 
				InvenPos.m_cPos, InvenPos.m_cIndex);

			usError = PktSRTAck::FAIL_NO_ITEM;
			goto SEND_PACKET;
		}

		if (0 == lpItem->GetNumOrDurability())
		{
			if (RemoveItem(InvenPos))
			{
				DELETE_ITEM(lpItem);
			}
			else
			{
				ERRLOG2(g_Log, "CID:0x%08x �������ͽ� ���Ʒ� ���� : ��Ȱ�� ���� ������ �� �����ϴ�. Pos:%d, Index:%d", 
					InvenPos.m_cPos, InvenPos.m_cIndex);

				m_Inventory.DumpItemInfo();

				usError = PktSRTAck::FAIL_NO_ITEM;
				goto SEND_PACKET;
			}
		}

//        unsigned long dwSrcGold = GetGold();
//        DeductGold(dwReduceGold, false);

//        GAMELOG::LogTakeGold(*this, dwSrcGold, GetGold(), dwReduceGold, 
//            TakeType::TS_INVEN, TakeType::TS_INVEN, GAMELOG::sTakeGoldLogV2::STATE_REDISTRIBUTE, 0);

		// ���� ���� �˻�
		int OriStat = tempInfoSt.IP+tempInfoSt.STR+tempInfoSt.DEX+tempInfoSt.CON+tempInfoSt.INT+tempInfoSt.WIS;
		int NewStat = State.m_wIP+State.m_wSTR+State.m_wDEX+State.m_wCON+State.m_wINT+State.m_wWIS;

		// �ٲ� ������ ����
		m_DBData.m_Info.IP	= State.m_wIP;
		m_DBData.m_Info.STR = State.m_wSTR;
		m_DBData.m_Info.DEX = State.m_wDEX;
		m_DBData.m_Info.CON = State.m_wCON;
		m_DBData.m_Info.INT = State.m_wINT;
		m_DBData.m_Info.WIS = State.m_wWIS;

		// ��й�� ���ȿ� ���� ��ų ������
		if (false == RedistributionSkill())
		{
			ERRLOG1(g_Log, "CID:0x%08x ��ų ��й迡 �����Ͽ����ϴ�.", m_dwCID);
			m_DBData.m_Info = tempInfoSt;
			return false;	
		}

		DETLOG7(g_Log, "CID:%u �� ĳ���Ͱ� ���� ��й踦 �߽��ϴ�.(��������:IP:%d,STR:%d,DEX:%d,CON:%d,INT:%d,WIS:%d)", m_dwCID, tempInfoSt.IP, tempInfoSt.STR, tempInfoSt.DEX, tempInfoSt.CON, tempInfoSt.INT, tempInfoSt.WIS);
		DETLOG7(g_Log, "CID:%u �� ĳ���Ͱ� ���� ��й踦 �߽��ϴ�.(IP:%d,STR:%d,DEX:%d,CON:%d,INT:%d,WIS:%d)", m_dwCID, State.m_wIP, State.m_wSTR, State.m_wDEX, State.m_wCON, State.m_wINT, State.m_wWIS);

		// ������ �ʱ�ȭ
		m_DBData.m_Quick = QUICK::QUICK();
	}

SEND_PACKET:

	if (NULL != m_lpGameClientDispatch)
	{
		if (false == GameClientSendPacket::SendCharStatusRetrain(m_lpGameClientDispatch->GetSendStream(), 
			m_dwCID, m_DBData, InvenPos, m_DBData.m_Info.Gold, usError)) 
		{
			return false;
		}
		else
		{
			if (false == GameClientSendPacket::SendCharStateRedistribution(m_lpGameClientDispatch->GetSendStream(), 
				m_dwCID, GetState(), GetSkill(), usError)) 
			{
				return false;
			}
		}
	}

	if (usError == PktSRTAck::NO_SERVER_ERR)
	{
		return CalculateStatusData(true);
	}
    
	// usError �� Error ���� ������ �ִ� ��Ŷ�� ���±� ������
	// true �� �����ؼ� Ŭ���̾�Ʈ�� ������ ���� �ʴ´�.
	return true;
}


unsigned char CCharacter::GetCurrentDurability(unsigned char cEquipmentIndex)
{
	if (cEquipmentIndex >= Item::EquipmentPos::MAX_EQUPMENT_POS) {
		return 0;
	}
	
	Item::ItemPos itemPos(TakeType::TS_EQUIP, cEquipmentIndex);
	Item::CItem* lpItem = m_Equipments.GetItem(itemPos);
	if (NULL == lpItem) {
		return 0;
	}

	return lpItem->GetNumOrDurability();
}


bool CCharacter::ChangeWeaponAndShield(unsigned char cSelect) 
{ 
	if (cSelect < 0 || cSelect > 1) 
	{
		ERRLOG2(g_Log, "CID:0x%08x �߸��� ��ġ�� ����� ��ü�Ϸ� �մϴ�. Select: %d", m_dwCID, cSelect);
		return false;
	}

	if ((cSelect == 0 && Item::EquipmentPos::WEAPON_HAND1 == m_Equipments.GetRightHandIndex()) || 
		(cSelect == 1 && Item::EquipmentPos::WEAPON_HAND2 == m_Equipments.GetRightHandIndex())) 
	{
		ERRLOG2(g_Log, "CID:0x%08x �̹� ����ϰ� �ִ� ��ġ�� ����� ��ü�Ϸ� �մϴ�. Select: %d", m_dwCID, cSelect);
		return false;
	}

    m_cHandPos = cSelect;

/*
	if (cSelect <= 0 || cSelect > 2) 
	{
		ERRLOG2(g_Log, "CID:0x%08x �߸��� ��ġ�� ����� ��ü�Ϸ� �մϴ�. Select: %d", m_dwCID, cSelect);
		return false;
	}

	if ((cSelect == 1 && Item::EquipmentPos::WEAPON_HAND1 == m_Equipments.GetRightHandIndex()) || 
		(cSelect == 2 && Item::EquipmentPos::WEAPON_HAND2 == m_Equipments.GetRightHandIndex())) 
	{
		ERRLOG2(g_Log, "CID:0x%08x �̹� ����ϰ� �ִ� ��ġ�� ����� ��ü�Ϸ� �մϴ�. Select: %d", m_dwCID, cSelect);
		return false;
	}

    m_cHandPos = cSelect - 1;
*/

	m_Equipments.ChangeWeaponAndShield(); 
	CalculateStatusData(false);

	return true;
}

bool CCharacter::ChangeRide(unsigned char cSelect)			// �� Ÿ�� ������
{
	if (cSelect < 0 || cSelect > 1) 
	{
		ERRLOG2(g_Log, "CID:0x%08x �߸��� ��ġ�� ���� ��ü�Ϸ� �մϴ�. Select: %d", m_dwCID, cSelect);
		return false;
	}

	if (m_cRidePos == cSelect) 
	{
		ERRLOG2(g_Log, "CID:0x%08x �̹� ���� ���·� ���� ����ϰ� �ֽ��ϴ�. Select: %d", m_dwCID, cSelect);
		return false;
	}

    m_cRidePos = cSelect;
	m_Equipments.ChangeRide(m_cRidePos); 
	CalculateStatusData(false);

	return true;
}


bool CCharacter::CalculateEquipDurability(unsigned short wAttackType) 
{
	Item::CItem* lpEquipment = NULL;
	unsigned char cEquipIndex = Item::EquipmentPos::MAX_EQUPMENT_POS;
	unsigned char cEquipValue = 0;

	int AbValue = m_AbilityValue[Skill::Type::AB_ENDUR_SHILD];	// �ۼ�Ʈ�� ���ִ�. (100 �̸� 100���θ�ŭ ���ʽ��� �޴´�)
	int AddPer = 0;

	switch (wAttackType)
	{
		// ��
		case AtType::GUARD:
		{
			AddPer = SHIELD_DURABILITY_DECRESE_PERCENTAGE*AbValue/100;
			if (0 == Math::Random::ComplexRandom(SHIELD_DURABILITY_DECRESE_PERCENTAGE+AddPer)) 
			{ 
				lpEquipment = GetEquipments().GetLeftHand();
				if (NULL == lpEquipment)
				{
					// �޼� ��� ������ ���������� ��
					lpEquipment = GetEquipments().GetRightHand();
					if (NULL == lpEquipment)
					{
						ERRLOG1(g_Log, "CID:0x%08x ���� ��� ���� ĳ���Ͱ� ���Ͽ����ϴ�.", m_dwCID);
						return false;
					}
				}
			}		
			break;
		}

		// ���
		case AtType::DEFENCE:
		{
			AddPer = ARMOUR_DURABILITY_DECRESE_PERCENTAGE*AbValue/100;
			if (0 == Math::Random::ComplexRandom(ARMOUR_DURABILITY_DECRESE_PERCENTAGE+AddPer)) 
			{ 
				const unsigned char MAX_ARMOUR_INDEX_NUM = 4;
				static unsigned char aryArmourShuffleList[MAX_ARMOUR_INDEX_NUM] = {
					Item::EquipmentPos::HELM, 
					Item::EquipmentPos::BOOTS, 
					Item::EquipmentPos::GLOVE, 
					Item::EquipmentPos::ARMOUR 
				};

				std::random_shuffle(aryArmourShuffleList, aryArmourShuffleList + MAX_ARMOUR_INDEX_NUM);
				lpEquipment = GetEquipments().GetItem(Item::ItemPos(TakeType::TS_EQUIP, aryArmourShuffleList[0]));
			}
			break;
		}

		// �޼� ����
		case AtType::LEFT_MELEE:
		{
			AddPer = WEAPON_DURABILITY_DECRESE_PERCENTAGE*AbValue/100;
			if (0 == Math::Random::ComplexRandom(WEAPON_DURABILITY_DECRESE_PERCENTAGE+AddPer)) 
			{ 
				lpEquipment = GetEquipments().GetLeftHand();
			}
			break;
		}

		// �Ϲ� ����
		default:
		{
			AddPer = WEAPON_DURABILITY_DECRESE_PERCENTAGE*AbValue/100;
			if (0 == Math::Random::ComplexRandom(WEAPON_DURABILITY_DECRESE_PERCENTAGE+AddPer)) 
			{ 
				lpEquipment = GetEquipments().GetRightHand();
			}
			break;
		}
	}

	if (NULL != lpEquipment)
	{
		if (0 < lpEquipment->GetNumOrDurability())
		{			
			lpEquipment->SetNumOrDurability(lpEquipment->GetNumOrDurability() - 1);
		}
		else if (0 == lpEquipment->GetNumOrDurability()) 
		{ 
			// edith 2008.01.16 ������ �ִ볻���� ����
			int idown = STARTAXDURABILITY+Math::Random::ComplexRandom(DOWNMAXDURABILITY);
			int curDur = lpEquipment->GetMaxNumOrDurability() - idown;
			if(curDur < 0)
				curDur = 1;

			lpEquipment->SetMaxNumOrDurability(curDur);
			lpEquipment->SetNumOrDurability(0);

			CalculateStatusData(false);
		}

		if (NULL != m_lpGameClientDispatch)
		{
			GameClientSendPacket::SendCharEquipDurability(m_lpGameClientDispatch->GetSendStream(), m_dwCID, 
				lpEquipment->GetPos().m_cIndex, lpEquipment->GetNumOrDurability(), lpEquipment->GetMaxNumOrDurability(), PktBase::NO_SERVER_ERR);
		}
	}

	return true;
}

bool CCharacter::CalculateAllEquipDurability(unsigned char cDecreasePersent)
{
	for (char cIndex = 0; cIndex < Item::EquipmentPos::MAX_EQUPMENT_POS; ++cIndex)
	{
		// ������ ���Ұ� ���� �������� ����
		if (Item::EquipmentPos::SHIRT == cIndex ||
			Item::EquipmentPos::TUNIC == cIndex ||
			Item::EquipmentPos::NECKLACE == cIndex ||
			Item::EquipmentPos::RINGR == cIndex ||
			Item::EquipmentPos::RINGL == cIndex || 
			Item::EquipmentPos::AVATA == cIndex )	
		{
			continue;
		}

		Item::ItemPos itemPos(TakeType::TS_EQUIP, cIndex);
        Item::CItem* lpItem = m_Equipments.GetItem(itemPos);
		if (NULL == lpItem)
		{
			continue;
		}

		if (lpItem->GetEnableRepair())
		{
			if (0 < lpItem->GetNumOrDurability())
			{
				const unsigned char cDecreasePoint = 
					static_cast<unsigned char>(lpItem->GetMaxNumOrDurability() * cDecreasePersent / 100.0f);

				if (lpItem->GetNumOrDurability() <= cDecreasePoint)
				{
					// edith 2008.01.16 ������ �ִ볻���� ����
					int idown = STARTAXDURABILITY+Math::Random::ComplexRandom(DOWNMAXDURABILITY)/2;
					int curDur = lpItem->GetMaxNumOrDurability() - idown;
					if(curDur < 0)
						curDur = 1;

					lpItem->SetMaxNumOrDurability(curDur);
					lpItem->SetNumOrDurability(0);
				}
				else
				{
					lpItem->SetNumOrDurability(lpItem->GetNumOrDurability() - cDecreasePoint);
				}
			}
			else if (0 == lpItem->GetNumOrDurability()) 
			{
				// edith 2008.01.16 ������ �ִ볻���� ����
				int idown = STARTAXDURABILITY+Math::Random::ComplexRandom(DOWNMAXDURABILITY);
				int curDur = lpItem->GetMaxNumOrDurability() - idown;
				if(curDur < 0)
					curDur = 1;

				lpItem->SetMaxNumOrDurability(curDur);
				lpItem->SetNumOrDurability(0);
			}

			if (0 == lpItem->GetNumOrDurability()) 
			{ 
				CalculateStatusData(false);
			}

			if (NULL != m_lpGameClientDispatch)
			{
				GameClientSendPacket::SendCharEquipDurability(m_lpGameClientDispatch->GetSendStream(), m_dwCID, 
					lpItem->GetPos().m_cIndex, lpItem->GetNumOrDurability(), lpItem->GetMaxNumOrDurability(), PktBase::NO_SERVER_ERR);
			}
		}
	}

	return true;
}

void CCharacter::UpgradeRespawnSpeedByEmblem(unsigned char cUpgradeType, unsigned char cUpgradeStep)
{
	int nKID = Siege::GetKID(Siege::EMBLEM, Siege::COMPLETE, cUpgradeType, cUpgradeStep);

	const CMonsterMgr::MonsterProtoType* lpProtoType = CMonsterMgr::GetInstance().GetMonsterProtoType(nKID);
	if (lpProtoType)
	{
		m_dwRespawnSpeed -= static_cast<unsigned long>(m_dwRespawnSpeed * lpProtoType->m_MonsterInfo.m_fRespawnSpeedUp);
	}
}

void CCharacter::DegradeRespawnSpeedByEmblem()
{
	m_dwRespawnSpeed = CCharacter::RESPAWN_PULSE;
}
