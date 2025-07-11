
#include "stdafx.h"

#include <Log/LogCommands.h>
#include <Log/CharacterLog.h>

#include <Utility/Math/Math.h>

#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>
#include <Network/Packet/PacketStruct/CharAttackPacket.h>
#include <Network/Packet/PacketStruct/CharItemPacket.h>
#include <Network/Packet/PacketStruct/CharStatusPacket.h>
#include <Network/Packet/PacketStruct/ServerLogPacket.h>

#include <Network/Dispatch/DBAgent/DBAgentRequest.h>
#include <Network/Dispatch/DBAgent/DBAgentDispatch.h>
#include <Network/Dispatch/DBAgent/DBAgentPacketParse.h>
#include <Network/Dispatch/DBAgent/RegularAgentDispatch.h>
#include <Network/Dispatch/GameClient/GameClientDispatch.h>
#include <Network/Dispatch/GameClient/SendCharLoginOut.h>
#include <Network/Dispatch/GameClient/SendCharAttack.h>
#include <Network/Dispatch/GameClient/SendCharCommunity.h>
#include <Network/Dispatch/GameClient/SendCharQuest.h>
#include <Network/Dispatch/GameClient/SendCharEtc.h>
#include <Network/Dispatch/GameClient/SendCharCastle.h>
#include <Network/Dispatch/GameLog/SendLogPacket.h>

#include <Network/Dispatch/DBAgent/CastlePacketParse.h>

#include <Network/Dispatch/Chat/ChatDispatch.h>

#include <Map/FieldMap/Cell.h>
#include <Map/FieldMap/VirtualArea/VirtualArea.h>
#include <Map/FieldMap/VirtualArea/VirtualAreaMgr.h>
#include <Map/FieldMap/CellManager.h>

#include <Item/ItemFactory.h>
#include <Item/Container/ExchangeContainer.h>

#include <Skill/SkillTable.h>
#include <Skill/Spell/SpellUtil.h>
#include <Skill/Spell/SpellTable.h>
#include <Skill/Spell/GlobalSpellMgr.h>

#include <Creature/CreatureManager.h>
#include <Creature/Character/CharRespawnMgr.h>
#include <Creature/Monster/Monster.h>
#include <Creature/Character/ExpTable.h>
#include <Creature/Siege/SiegeObject.h>
#include <Creature/Siege/Camp.h>
#include <Creature/Siege/CastleGate.h>
#include <Creature/Siege/SiegeObjectMgr.h>


#include <Castle/Castle.h>
#include <Castle/CastleMgr.h>

#include <Community/Guild/GuildMgr.h>
#include <Community/Guild/Guild.h>

#include <Community/Party/PartyMgr.h>

#include <GameTime/GameTimeConstants.h>
#include <GameTime/GameTimeMgr.h>

#include "Character.h"
#include "CharacterCreate.h"
#include "SphereTree/CharSphereTree.h"

// forward decl.
void CheckDuplicatedItem(CCharacter& character);

void SendItemDuplicatedLog(CSendStream& SendStream, unsigned __int64 dwItemSerial, 
                           Item::CItemOwnerInfo& itemOwnerInfo, unsigned long dwItemQty);

void CCharacter::PrepareLogin(void)
{
    // HP/MP����
	
    m_CreatureStatus.m_nNowHP = m_DBData.m_Info.HP;
    m_CreatureStatus.m_nNowMP = m_DBData.m_Info.MP;
	
    // ���� ������ �˻�
    CheckDuplicatedItem(*this);
	
	// edith 2009.02.07 ������ ��ġ ���� �������� �α����ϴ°� ����
	//if (/*SERVER_ID::STONE_WAR1*/SERVER_ID::CAPITAL <= CServerSetup::GetInstance().GetServerZone() && CServerSetup::GetInstance().GetServerZone() <= SERVER_ID::STONE_WAR3)
	{
		// ��ǥ �˻�
		if (!CCellManager::GetInstance().CheckPositionInZone(m_CurrentPos))
		{
			if (CServerSetup::GetInstance().IsBattleGameServer())
			{
				m_CurrentPos = CharCreate::BGServerStartPos[GetRace()][Math::Random::ComplexRandom(CharCreate::MAX_LOBBY_RESPAWN_POS)];//hz pos possible needs to be changed
				m_CurrentPos.m_fPointX += Math::Random::SimpleRandom(GetTickCount(), 20) - 10;
				m_CurrentPos.m_fPointZ += Math::Random::SimpleRandom(GetTickCount(), 20) - 10;
			}
			else
			{
				m_CurrentPos = CCharRespawnMgr::GetInstance().GetDefaultRespawnPos(GetNation());
			}

			m_DBData.m_Pos.LastPoint.fPointX = m_CurrentPos.m_fPointX;
			m_DBData.m_Pos.LastPoint.fPointY = m_CurrentPos.m_fPointY;
			m_DBData.m_Pos.LastPoint.fPointZ = m_CurrentPos.m_fPointZ;
		}
	}
	
    // ��ε�ĳ���� ������ �غ�
    m_SerializeCharacterData.PrepareData(*this);
    m_SerializeCharacterData.ClearDeltaData();

    SetOperationFlag(CHAR_INFO_LOADED);
}

bool CCharacter::Login()
{
    if (NULL == m_lpGameClientDispatch)
    {
        return false;
    }

	CSendStream& SendStream = m_lpGameClientDispatch->GetSendStream();
/*
#ifndef NO_GAMEGUARD
	// �α��� �ϱ����� ���Ӱ��� ó��
	// edith 2009.08.11 ���Ӱ��� �׽�Ʈ
	if (NULL != m_lpGameClientDispatch)// && 0 == GetAdminLevel()) 
	{ 				
		// edith 2009.08.11 ���Ӱ��� 2.5 ���׷��̵�
//		if (false == m_lpGameClientDispatch->IsAuth())
//		{
//			ERRLOG1(g_Log, "CID:0x%08x ���� ���� ���� �ڵ带 �������ʾ� ������ �����ϴ�.", GetCID());
//			m_lpGameClientDispatch->Disconnect();
//			return false;
//		}

		GG_AUTH_DATA* lpAuthData = NULL;				
		if (false == m_lpGameClientDispatch->GetAuthQuery(&lpAuthData))
		{
			ERRLOG1(g_Log, "CID:0x%08x ���� ���� ���� �ڵ�(2) üũ�� �����Ͽ� ������ �����ϴ�.", GetCID());
			m_lpGameClientDispatch->Disconnect();
			return false;
		}

		GameClientSendPacket::SendCSAuth(SendStream, 
			GetCID(), m_lpGameClientDispatch->GetAuthCode(), lpAuthData, PktBase::NO_SERVER_ERR);
	}
#endif
*/   


// DELETE_ME : ���̻� Ŭ���̾�Ʈ�� ���� ������ ������ �Ұ����ϴ�. ���丸 �������� ��. (2005-05-31 by �ε�)
/*
    // ���� ��� ����
	if (SERVER_ID::ZONE12 == CServerSetup::GetInstance().GetServerZone() ||
		SERVER_ID::CAPITAL == CServerSetup::GetInstance().GetServerZone())
	{
        m_PeaceMode.m_bPeace = true;
	}
	else
	{
        m_PeaceMode.m_bPeace = false;
	}

	SetPeaceMode(m_PeaceMode, false);
    GameClientSendPacket::SendCharPeaceMode(SendStream, m_dwCID, 0, m_PeaceMode.m_bPeace, 0);
*/

    // ��Ʋ �׶��� ���������� ������ 40���� ����
    if (SERVER_ID::BATTLE_SERVER == CServerSetup::GetInstance().GetServerZone())
    {
        // ������ 40���� ����
        while (static_cast<unsigned long>(m_CreatureStatus.m_nLevel) < 40)
        {
            if (false == IncrementExp(static_cast<unsigned long>(
                EXP::ExpTable[m_CreatureStatus.m_nLevel - 1])))
            {
                break;
            }
        }
    }

	// Cell �� �������� ���� �����̹Ƿ� MoveTo �Լ��� �ѹ� ȣ�����ش�.
	MoveTo(GetCurrentPos(), false);

	// edith 2009.06.13 16�� �� / 17�� ������ �����ð� ����
	int InvincibleTime = 30;

	if (/*SERVER_ID::STONE_WAR1*/SERVER_ID::CAPITAL <= CServerSetup::GetInstance().GetServerZone() && CServerSetup::GetInstance().GetServerZone() <= SERVER_ID::STONE_WAR3)
	{
		InvincibleTime = 15;

		char cZone = CServerSetup::GetInstance().GetServerZone();
		//  edith 2009.06.13 ���������� �α����� ��� ��Ÿ�� ����Ʈ���� �������Ǿ ����
		// ��ġ�� �̵���Ű���� MovePos �� ������ �̿��ؼ� �ش���ǥ�� �����̵���Ų��.
		//const Position targetPos = CCharRespawnMgr::GetInstance().GetTownRespawnPos(cZone, GetNation());//wasnt commented out autospawn fix
		//MovePos(targetPos, cZone, false);//wasnt commented out autospawn fix

		// ���� ���¸� ������ ��Ų��.
        if (0 == m_CreatureStatus.m_nNowHP)
		{
			Respawn();
		}
	}


    // �α��ν� 30�ʰ� ����
    Skill::CAddSpell<CInvincibleSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, this, 
        Skill::SpellType::MAGICAL_SPELL, Skill::SpellID::Invincible, 1, InvincibleTime))(this);

	// ���� ������ �����ϸ� ���� ���� ��æƮ ����
	CCamp* lpWorldWeapon = CSiegeObjectMgr::GetInstance().GetWorldWeapon();
	if (lpWorldWeapon)
	{
		AddWorldWeaponEnchant(reinterpret_cast<CAggresiveCreature*>(lpWorldWeapon), lpWorldWeapon->GetNation()); 
	}

	// DB���� ������ �о�� �ٽ� �ɾ��ش�.
	const SPELL spell = GetSpell();
	GetSpellMgr().SetSpell(spell);


    // ��� ������ �ش�.
    CGuild* lpGuild = 0;
    if (0 != GetGID() && 0 != (lpGuild = CGuildMgr::GetInstance().GetGuild(GetGID())))
    {
        unsigned short wError = PktBase::NO_SERVER_ERR;
        unsigned char cTitle = lpGuild->GetTitle(m_dwCID);

        if (Guild::NONE == cTitle)
        {
            wError = PktBase::SERVER_ERROR;
        }

        GameClientSendPacket::SendCharMyGuildInfo(SendStream, 
            lpGuild->GetGold(), lpGuild->GetRight(), cTitle, wError);
    }

    // �� ���� ����
    GameClientSendPacket::SendCharCastleInfo(SendStream);
    
	// ���� �ð� ���� ����
	GameClientSendPacket::SendCharGameTimeInfo(SendStream);

    // ��� ��� ���� ����
    GameClientSendPacket::SendCharCampInfo(SendStream);

// CASTLE_TODO : ���� ��� ������ �ƴϹǷ� �ϴ� ���Ƶд�. 

    // ������ �ӵ� ��� ����
//    if (0 != GetGID())
//    {
//        Castle::CCastle* lpCastle = Castle::CCastleMgr::GetInstance().GetCastleByGID(GetGID());
//        if (lpCastle)
//        {
//            CSiegeObject* lpEmblem = lpCastle->GetCastleEmblem();
//            if (lpEmblem && lpEmblem->GetUpgradeStep() > 0)
//            {
//                UpgradeRespawnSpeedByEmblem(lpEmblem->GetUpgradeType(), lpEmblem->GetUpgradeStep());
//            }
//        }
//    }

    // ����Ʈ ���ʽ� ���� ����
    GameClientSendPacket::SendCharEliteBonus(SendStream, GetEliteBonus());

	// �ź� �ɼ� ���� ����
	GameClientSendPacket::SendCharControlOption(SendStream, m_dwCID, m_RejectOption);

	// �����Ƽ ����Ʈ ������Ʈ
	UpdateUseAbilityPoint();

	// edith 2008.06.03 �������� ����Ʈȿ��
	// ������ : ��ũ ī���� ���� ����
	//if (true == CServerSetup::GetInstance().UseContents(GameRYL::STONE_BATTLE))//wasnt commented out hayzohar statue war fix
	//{
		// �������� �������� ����Ʈ ȿ��.
		if (CGameTimeMgr::GetInstance().IsRealmWarTime() &&
			(/*SERVER_ID::STONE_WAR1*/SERVER_ID::CAPITAL <= CServerSetup::GetInstance().GetServerZone() && CServerSetup::GetInstance().GetServerZone() <= SERVER_ID::STONE_WAR3))
		{
			RealmSkill::RealmInchantAdd(this);
		}
	//}
	// Admin ĳ���Ϳ��� ����ó�� (QA �� ��û).
	if (CGameTimeMgr::GetInstance().IsRealmWarTime() &&
			(/*SERVER_ID::STONE_WAR1*/SERVER_ID::CAPITAL <= CServerSetup::GetInstance().GetServerZone() && CServerSetup::GetInstance().GetServerZone() <= SERVER_ID::STONE_WAR3))//was(true == CServerSetup::GetInstance().UseContents(GameRYL::STONE_BATTLE)) hayzohar statue war fix
	{
		if(GetAdminLevel())
		{
			goto lb_move;
		}
	}
/*
	if (true == CServerSetup::GetInstance().UseContents(GameRYL::NEWZONE_ZONE9))
	{
		if(GetAdminLevel())
		{
			goto lb_move;
		}
	}
*/
	// �����, ������ ���� ĳ���ʹ� �ش� �ð��� �α��ν� �� �̵� ó���� ���ش�.
	if (CGameTimeMgr::GetInstance().IsGuildWarTime() &&
		(GetGuildWarFlag() == Creature::WAR_ON || GetGuildWarFlag() == Creature::WAR_INSTANCE))
	{
		MoveToGuildWarZone();
	}

	// ������ �� �α����� ��� �������� ������ �������� �̵���Ų��.
	// edith 2008.07.17 ĳ��Ż���� �α����ߴµ� �����ð��̸� �������������� �̵���.
	if(CServerSetup::GetInstance().GetServerZone() == SERVER_ID::CAPITAL)
	{
		if(CGameTimeMgr::GetInstance().IsSiegeWarTime())
		{
			using namespace DBAgentPacketParse;

			
			SiegeMovePos(this);
		}
	}

	// ������ : ��ũ ī���� ���� ����
	//if (true == CServerSetup::GetInstance().UseContents(GameRYL::STONE_BATTLE))//wasnt commented out hayzohar statue war fix
	//{
		if ((CGameTimeMgr::GetInstance().IsRealmWarReadyTime() || CGameTimeMgr::GetInstance().IsRealmWarTime()) &&
			(GetRealmWarFlag() == Creature::WAR_ON || GetRealmWarFlag() == Creature::WAR_INSTANCE) &&

			(/*SERVER_ID::STONE_WAR1*/SERVER_ID::CAPITAL < CServerSetup::GetInstance().GetServerZone() || CServerSetup::GetInstance().GetServerZone() > SERVER_ID::STONE_WAR3) )
		{
			// ������ �ð��ε� ���������� �Ǿ��ְ�.������ 16���� �ƴϴ� ������ 16������ �̵�.
			MoveToRealmWarZone();
		}

		if (CGameTimeMgr::GetInstance().IsRealmWarTime() &&
			GetRealmWarFlag() == Creature::WAR_OFF &&
			(/*SERVER_ID::STONE_WAR1*/SERVER_ID::CAPITAL <= CServerSetup::GetInstance().GetServerZone() && CServerSetup::GetInstance().GetServerZone() <= SERVER_ID::STONE_WAR3))
		{
			// edith 2008.07.08 �￡ ������ ���ߴµ� 16�� ���̰�.. ������ �ð��̸� ���� ����
			GET_SINGLE_DISPATCH(lpDBAgentDispatch, 
				CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());

			if (0 != lpDBAgentDispatch)
			{
				GameClientSendPacket::SendWarOnOff(lpDBAgentDispatch->GetSendStream(), GetCID(), GameTime::REALM, Creature::WAR_INSTANCE, 0);
			}

			// ����ð��� �α����ߴµ� �� ���� ������ �ʿ��� �α����� �߾�.. �̸��� ��������ġ�� �����̵���..
			// edith 2008.07.08 ���￡ �������� ��������� 16������ �����ϸ� ��������ġ�� ���� �̵���Ų��.
//			MoveToRealmWarZone();
		}	

lb_move:

		// edith 2008.06.03 ���� ��þƮȿ��
		// ��ũ ī������ �α��ν�
		if (/*SERVER_ID::STONE_WAR1*/SERVER_ID::CAPITAL <= CServerSetup::GetInstance().GetServerZone() && CServerSetup::GetInstance().GetServerZone() <= SERVER_ID::STONE_WAR3)
		{
			// ���� ������ �����ش�.
			CCreatureManager::GetInstance().SendRealmStatueDisplayInfo(SendStream);

			bool bAddRealmStatueEnchant = false;
			//if (true == CServerSetup::GetInstance().UseContents(GameRYL::STONE_BATTLE))//wasnt commented out hayzohar statue war fix
			//{
				// ���� ���� �ð��� �ƴ϶��, ���� ��æƮ ȿ���� �ɾ��ش�.
				if (!CGameTimeMgr::GetInstance().IsRealmWarTime())
				{
					bAddRealmStatueEnchant = true;
				}
			//}
/*
			// �������� �ʰ� �������� ���� ���α� ������ �ش� ���� ���ʿ�
			if (true == CServerSetup::GetInstance().UseContents(GameRYL::SIEGE))
			{
				// ������ ������ ���Խ� ������ �ð��� �ƴ϶��, ���� ��æƮ ȿ���� �ɾ��ش�.
				if (CGameTimeMgr::GetInstance().IsSiegeWarTime())
				{
					bAddRealmStatueEnchant = false;
				}
			}
*/
/*
			// ���� ��æƮ ȿ���� ������ �� �ִ� ��Ȳ�̶��....
			if (bAddRealmStatueEnchant)
			{
				// edith 2008.06.03 �������� ������ 11�ð� ������ �ɾ��ش�.
				CCreatureManager::GetInstance().AddRealmStatueEnchant(this);
			}
*/
		}
		// �ű��� �α��ν�
		else if (CServerSetup::GetInstance().GetServerZone() == SERVER_ID::ZONE5)
		{
			// ��������� ������ �����ش�.
			CCreatureManager::GetInstance().SendRealmStatueDisplayInfo(SendStream);
		}
	//}

    LOG_INOUT(
        
        const int MAX_LOG_BUFFER = 1024;
        char szBuffer[MAX_LOG_BUFFER];        
        unsigned long dwDispatchUID = m_lpGameClientDispatch->GetUID();
        const SOCKADDR_IN& sockAddr = m_lpGameClientDispatch->GetRemoteAddr().get_addr_in();

        _snprintf(szBuffer, MAX_LOG_BUFFER - 1,
            "UID:%d/CID:0x%08x(0x%p)/DispatchUID:%d/DispatchPointer:0x%p �α��ο� �����Ͽ����ϴ�. "
            "��� �⺻ ������ ����ϴ�. %s(lev:%2d, exp:%016I64u) IP:%15s:%5u", 
            m_dwUID, m_dwCID, this, dwDispatchUID, m_lpGameClientDispatch, 
            m_DBData.m_Info.Name, m_DBData.m_Info.Level, m_DBData.m_Info.Exp, 
            inet_ntoa(sockAddr.sin_addr), ntohs(sockAddr.sin_port));

        szBuffer[MAX_LOG_BUFFER - 1] = 0;
        DETLOG0(g_Log, szBuffer);

        );
    
	return true;
}



bool CCharacter::Logout(DBUpdateData::UpdateType eUpdateType)
{
	// ��Ʋ�׶����� ��� (���� ���¶��) ������ ť���� ����
	if (SERVER_ID::ZONE3 == CServerSetup::GetInstance().GetServerZone())
	{
		CCreatureManager::GetInstance().PopRespawnQueue(this);
	}

	// �׾��µ� ������ ���ϰ� �α׾ƿ��ϸ� 
	// �������� �ȱ��̴� ���װ� �־ �α׾ƿ��Ҷ� ��Ȱ�� ���°� �ƴϸ� �������� ��ƹ�����.
	if(IsDead())
	{
		switch(m_eLastDeadType)
		{
		case DEAD_BY_NONE:      // �� �� ����
		case DEAD_BY_CHARACTER: // ĳ���Ϳ� ���� ���            
			break;

		case DEAD_BY_SUICIDE:   // �ڻ�

			// �ڻ�� ������ ����
			CalculateAllEquipDurability(DURABILITY_DECREASE_PERSENT_BY_SUICIDE);
			break;

		case DEAD_BY_MONSTER:   // ���Ϳ� ���� ���

			if (!CServerSetup::GetInstance().GetDeathPenaltyEvent())
			{
				unsigned char cDecreasePersent = 0;

				if(m_AbilityValue[Skill::Type::AB_ENDUR_SHILD] != 0)
					cDecreasePersent = DURABILITY_DECREASE_PERSENT_BY_MONSTER*m_AbilityValue[Skill::Type::AB_ENDUR_SHILD]/100;

				// ���Ϳ��� �׾��� �� ������ ����
                CalculateAllEquipDurability(DURABILITY_DECREASE_PERSENT_BY_MONSTER-cDecreasePersent);
			}
			break;
		}
	}

	// ���� ���� ��æƮ�� �����Ѵ�.
	ClearWorldWeaponEnchant();

	// ������ ���� �ִ� ���̾��ٸ�... ���� ���⸦ ����Ѵ�.
	if (0 != m_dwProtectGateCID)
	{
		CCastleGate* lpGate = reinterpret_cast<CCastleGate*>( CSiegeObjectMgr::GetInstance().GetSiegeObject(m_dwProtectGateCID) );
		if (lpGate)
		{
			lpGate->DeleteProtectGate(this);

			// Ŭ���̾�Ʈ���� ����
			CGameClientDispatch* lpDispatch = GetDispatcher();
			if (NULL != lpDispatch)
			{
				GameClientSendPacket::SendCharCastleCmd(lpDispatch->GetSendStream(), lpGate->GetCastleID(),
					lpGate->GetCID(), 0, 0,
					PktCastleCmd::CASTLE_GATE_PROTECT_CANCEL,
					PktCastleCmd::NO_SERVER_ERR);
			}
		}
	}

	// �� �α׾ƿ�
	if (NULL != m_CellPos.m_lpCell)
	{
		m_CellPos.m_lpCell->DeleteCreature(m_dwCID);
		m_CellPos.m_lpCell = NULL;
	}

    // ä�� ������ �α׾ƿ��� ������.
    m_SerializeCharacterData.SendChatLogout(*this);

	// BG_TODO : ����̳�, ������ ��쿡��.. �ٽ� ó���ؾ� ������ �𸥴�.
	// VirtualArea �� �־��ٸ�, VirtualArea �� ĳ���͸���Ʈ���� ����
	if (0 != GetMapIndex())
	{
		VirtualArea::CVirtualAreaMgr::GetInstance().LeaveVirtualArea(this);
	}

	// ��ȯ���� �ִٸ� ���ش�.
	if (NULL != m_lpSummonee)
	{
		m_lpSummonee->Dead(NULL);		
	}

	// �ŷ����̶�� �ŷ� ���
    CCharacter* lpExchangeCharacter = m_Exchange.GetExchangeCharacter();
	if (NULL != lpExchangeCharacter)
	{
		m_Exchange.ExchangeOK(false);

        CGameClientDispatch* lpExchangerDispatch = lpExchangeCharacter->GetDispatcher();
        if (NULL != lpExchangerDispatch)
        {
            GameClientSendPacket::SendCharExchangeCmd(lpExchangerDispatch->GetSendStream(), 
                m_dwCID, lpExchangeCharacter->GetCID(), PktExC::EXC_QUIT, PktExC::NO_SERVER_ERR);
        }
	}

	// ��� �ʱ�ȭ
	DuelInit(PktDuC::DUC_LOGOUT);

    // ��Ƽã�� ����Ʈ���� ����
	CPartyMgr::GetInstance().DeleteFindPartyList(m_dwCID);

    // ��Ƽ �ֹ� ����.
    if (0 != GetPID())
    {
        // ��Ƽ �α׾ƿ�
        if (NULL != m_pParty)
        {
			m_pParty->PrepareLogout(m_dwCID);
        }
        else
        {
            ERRLOG3(g_Log, "CID:0x%08x ��Ƽ�� ���� �༮�� ��Ƽ �����Ͱ� NULL�Դϴ�. PID:0x%08x, PartyPointer:0x%08x", 
                m_dwCID, m_DBData.m_Info.PID, m_pParty);
        }
    }
	
	// ĳ���Ϳ� ���� �ɷ��ִ� ������� DB�� �����Ѵ�.
	// ������ �ƴϸ� ������ �ٽ� ���� �ʴ´�. 
	// �������� ������ �������� ������ �����ϱ� ������ ���⼭ �ٽ� ������ �����ϸ�
	// ������ �������.
	if(!IsDead())
	{
		const SPELL spell = GetSpellMgr().GetAffectedInfo().GetSpellInfo();
		SetSpell(spell);
	}

	// ȯ���� ����� ����ؼ� �ӽ� ��ü�� �����ִٸ� ����
	CRegularAgentDispatch::GetTempCharacterMgr().EraseChar(m_dwCID);
	
	GET_SINGLE_DISPATCH(lpDBAgentDispatch, 
		CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());

	if (0 != lpDBAgentDispatch)
	{
        CSendStream& AgentSendStream = lpDBAgentDispatch->GetSendStream();

        // ����Ʈ ���� ����
		GameClientSendPacket::SendCharQuestInfo(AgentSendStream, this);

		// ȯ�� ���� ���� ����
		GameClientSendPacket::SendConfigInfoDB(AgentSendStream, this);
	}

	// ���⿡ ž�����̶�� ���������� ����
	if (IsRideArms())
	{
		CSiegeObject* lpArms = CSiegeObjectMgr::GetInstance().GetSiegeObject(m_dwRideArmsCID);
		if (lpArms)
		{
			// ���ۼ� �����簡 �������
			if (Siege::RIDER_FOR_OWNER == lpArms->IsRider(m_dwCID))
			{
				lpArms->AllGetOff();
			}
			else
			{
				m_dwRideArmsCID = 0;
				lpArms->GetOff(m_dwCID);
			}
		}
	}

	int     nTotalSize = sizeof(PktDBUpdate) + DBUpdateData::MAX_DBUPDATE_SIZE;
    char    szCharBuffer[sizeof(PktDBUpdate) + DBUpdateData::MAX_DBUPDATE_SIZE];
	char*   lpCharacterInfo = szCharBuffer + sizeof(PktDBUpdate);

    PktDBUpdate* lpPktDBUpdate = reinterpret_cast<PktDBUpdate*>(szCharBuffer);
    memset(lpPktDBUpdate, 0, sizeof(PktDBUpdate));
	
    unsigned short usError = 0;
	unsigned char cAdmin = (true == IsAdmin()) ? 1 : 0;
    
	bool bCharacterUpdate = GetCharacterInfo(lpCharacterInfo, &nTotalSize, lpPktDBUpdate->m_usUpdate);
	if (!bCharacterUpdate) 
	{
		nTotalSize = 0;
        usError = 1;        
        std::fill_n(lpPktDBUpdate->m_usUpdate, unsigned short(DBUpdateData::MAX_UPDATE_DB), 0);		
		ERRLOG1(g_Log, "CID:0x%08x �����͸� ������ �� �� �����ϴ�. DB�� ������Ʈ �� �� �����ϴ�.", m_dwCID);
	}
    else if (0 != lpDBAgentDispatch)
    {
	    if (IsOperationFlagSet(CHAR_INFO_LOADED))
	    {
            CSendStream& AgentSendStream = lpDBAgentDispatch->GetSendStream();

            // â�� ������ ������Ʈ(â�� ���� ������ ������Ʈ�Ѵ�.)
            // ���� ����! DB�� Update�� ���� â�� ������Ʈ ��, ĳ���͸� ������Ʈ�Ѵ�.
            if (!m_Deposit.DBUpdate(AgentSendStream))
            {
                ERRLOG1(g_Log, "CID:0x%08x â�� ������Ʈ ����", m_dwCID);
            }

            // �α׾ƿ� ���� DBAgent�� ������
            lpPktDBUpdate->m_dlItemSerial	= Item::CItemFactory::GetInstance().GetItemUID();
            lpPktDBUpdate->m_dwSessionID    = m_dwSessionID;
            lpPktDBUpdate->m_dwUserID		= m_dwUID;
            lpPktDBUpdate->m_dwCharID		= m_dwCID;
            lpPktDBUpdate->m_TypeCode		= eUpdateType;
            lpPktDBUpdate->m_dwRequestKey   = 0;
			lpPktDBUpdate->m_cAdminFlag		= GetGMModelFlag();

            lpPktDBUpdate->m_dwRequestKey   = 0;
            lpPktDBUpdate->m_Address.S_un.S_addr = 0;
            lpPktDBUpdate->m_cAdminLevel    = 0;
			// WORK_LIST 2.4 ���� ������ ���Ӽ����� ĳ���Ͱ� �������� ����
            //lpPktDBUpdate->m_cPadding			= 0;
			lpPktDBUpdate->m_cAccountNation		= 0;
            lpPktDBUpdate->m_cNameChangeCount	= 0;
			lpPktDBUpdate->m_cGuildWarFlag		= 0;
			lpPktDBUpdate->m_cRealmWarFlag		= 0;
			lpPktDBUpdate->m_cRealmPoint		= 0;
			lpPktDBUpdate->m_cTacticsFlag		= 0;
			lpPktDBUpdate->m_PlayTime			= 0;
			lpPktDBUpdate->m_PremiumTime		= 0;
			lpPktDBUpdate->m_PremiumType		= 0;
			

            if (AgentSendStream.WrapCompress(reinterpret_cast<char*>(lpPktDBUpdate), 
                static_cast<unsigned short>(sizeof(PktDBUpdate) + nTotalSize), CmdDBUpdateData, 0, 0))
            {
 			    LOG_INOUT(
				    char szExp[64];
				    Math::Convert::Hex64ToStr(szExp, m_DBData.m_Info.Exp);
                    DETLOG7(g_Log, "UID:%d/CID:0x%08x(0x%p)/DispatchPointer:0x%p �� ĳ���� ������ DBAgent�� ������Ʈ�մϴ�. ���� �α׾ƿ��� ó���մϴ�."
					    " �α׾ƿ��� �����Ͽ����ϴ�. ��� �⺻ ������ ����ϴ�. %s(lev:%2d, exp:%s)",
					    m_dwUID, m_dwCID, this, m_lpGameClientDispatch, m_DBData.m_Info.Name, m_DBData.m_Info.Level, szExp)
			    );
			}
			else
			{
				ERRLOG1(g_Log, "CID:0x%08x DB�� ĳ���� ������Ʈ�� �� �� �����ϴ�. ���ۿ� �����߽��ϴ�.", m_dwCID);
            }
	    }
        else
        {
            // �����Ͱ� ���õ��� �ʾ�����, ���� �α׾ƿ��� �ƴϴ�.
            DBAgentPacketParse::SendAbnormalLogout(
                m_dwUID, m_dwCID, m_dwSessionID, 0, m_lpGameClientDispatch);        
        }
    }
	else
	{
        ERRLOG2(g_Log, "CID:0x%08x/AgentSession:0x%p/ DBUpdate failed.",
            m_dwCID, lpDBAgentDispatch);
	}

    SOCKADDR_IN remoteAddr;
    if (0 != m_lpGameClientDispatch)
    {
        remoteAddr = m_lpGameClientDispatch->GetRemoteAddr().get_addr_in();
    }
    else
    {
        memset(&remoteAddr, 0, sizeof(SOCKADDR_IN));
    }

    // ���ӷα׿� ĳ���� �α׾ƿ� �����
	GAMELOG::LogCharLoginOut(m_dwUID, this, &remoteAddr, lpCharacterInfo, nTotalSize, 
        lpPktDBUpdate->m_usUpdate, GAMELOG::CMD::CHAR_LOGOUT, usError);

	// �α� ������ �α׾ƿ� ������.
	// SendLogPacket::CharLogout(*this);

	LOG_INOUT(DETLOG4(g_Log, "UID:%d/CID:0x%08x(0x%p)/DispatchPointer:0x%p ���� ������ �α׾ƿ��� ó���մϴ�. ���� ������ �����մϴ�.", 
		m_dwUID, m_dwCID, this, m_lpGameClientDispatch));

	return CCreatureManager::GetInstance().DeleteCreature(m_dwCID);
}



void CheckDuplicatedItem(CCharacter& character)
{
    using namespace Item;

    CItemOwnerInfo itemOwnerInfo(
        character.GetCharacterName(), character.GetUID(), character.GetCID());

    CItemOwnerInfo* lpDuplicateOwner = 0;

    CItemFactory& itemFactory = Item::CItemFactory::GetInstance();
    
    // first : ������ ���� / second : ���� ������ Qty
    typedef std::pair<CItemOwnerInfo*, unsigned long>   DuplicateInfo;

    typedef std::multimap<unsigned __int64, DuplicateInfo, std::less<unsigned __int64>,
        boost::fast_pool_allocator<std::pair<unsigned __int64, DuplicateInfo> > > DuplicatedMap;

    DuplicatedMap duplicatedItemMap;

    // ���� �����Ͱ� ������ DB�߰�� �۽��ϱ� ���ؼ�..
    GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, 
        CDBAgentDispatch::GetDispatchTable());

    // ������ �����̳ʵ��� �迭�� �ִ´�.
    const int MAX_CONTAINER_SET = 7;
    CItemContainer* containerSet[MAX_CONTAINER_SET] = 
    {
        &character.GetInventory(),
        &character.GetEquipments(),
        &character.GetExtra(),
        &character.GetExchange(),
        &character.GetDeposit(),
        &character.GetStall(),
        &character.GetTempInven(),
    };

    CItemContainer** lppContainerPos = containerSet;
    CItemContainer** lppContainerEnd = containerSet + MAX_CONTAINER_SET;

    for(; lppContainerPos != lppContainerEnd; ++lppContainerPos)
    {
        CItemContainer& itemContainer = **lppContainerPos;

        CItemContainer::iterator pos = itemContainer.begin();
        CItemContainer::iterator end = itemContainer.end();
        
        for(; pos != end; ++pos)
        {
            CItem* lpItem = *pos;

            if (0 != lpItem)
            {
                unsigned __int64 dwItemSerial = lpItem->GetUID();

                lpDuplicateOwner = itemFactory.AddItemMap(dwItemSerial, itemOwnerInfo);
                if (0 != lpDuplicateOwner)
                {
                    // ���� �߰�. �ϴ� ���� �̳༮�� �ø��� �� ������ ����Ʈ ������ ����صξ��ٰ�
                    // ������ ��� ���߿� �ѹ��� ������.
                    
                    // �� ������ ����   : dwItemSerial, itemOwnerInfo
                    // ��� ������ ���� : dwItemSerial, lpDuplicateOwner

                    std::pair<DuplicatedMap::iterator, DuplicatedMap::iterator> result = 
                        duplicatedItemMap.equal_range(dwItemSerial);

                    bool bAddedQty = false;
                    bool bAddedDuplicateOwner = false;

                    for(;result.first != result.second; ++result.first)
                    {
                        unsigned __int64 dwItemUID = result.first->first;

                        // first : ������ ���� / second : ���� ������ Qty
                        DuplicateInfo&  duplicateInfo = result.first->second;                        
                        CItemOwnerInfo& duplicateOwnerInfo = *duplicateInfo.first;

                        if (duplicateOwnerInfo.GetCID() == itemOwnerInfo.GetCID())
                        {
                            // �����ڰ� ���� �������̴�. Qty�� ���������ش�.
                            // Qty�� ������ �������� '����' ��� ������ �ִ����� ��Ÿ����.
                            ++duplicateInfo.second;
                            bAddedQty = true;
                        }                       
                        else if (duplicateOwnerInfo.GetCID() == lpDuplicateOwner->GetCID())
                        {
                            // ������ ������ �ִ� ����� ���� �ƴ϶� �ٸ� ����̴�.                            
                            bAddedDuplicateOwner = true;
                        }
                    }
                    
                    if (!bAddedQty)
                    {
                        // ���ڰ� �߰����� �ʾ�����, �׸� ���� �߰�.
                        duplicatedItemMap.insert(result.second, DuplicatedMap::value_type(dwItemSerial, 
                            DuplicateInfo(&itemOwnerInfo, (itemOwnerInfo.GetCID() == lpDuplicateOwner->GetCID()) ? 2 : 1)));
                    }

                    if (!bAddedDuplicateOwner && lpDuplicateOwner->GetCID() != itemOwnerInfo.GetCID())
                    {
                        duplicatedItemMap.insert(result.second, 
                            DuplicatedMap::value_type(dwItemSerial, DuplicateInfo(lpDuplicateOwner, 1)));
                    }
                }
            }
        }
    }

    DuplicatedMap::iterator pos = duplicatedItemMap.begin();
    DuplicatedMap::iterator end = duplicatedItemMap.end();

    if (0 != lpDBAgentDispatch)
    {
        // ����
        for(; pos != end; ++pos)
        {
            SendItemDuplicatedLog(lpDBAgentDispatch->GetSendStream(), 
                pos->first, *pos->second.first, pos->second.second);
        }
    }
    else
    {
        // �α׸� �����.
        for(; pos != end; ++pos)
        {
            DuplicateInfo& dupInfo = pos->second;

            ERRLOG5(g_Log, "UID:%10u / CID:%10u / CharName:%s / ItemSerial:0x%I64X / Qty:%u / ������ ���簡 �߰ߵǾ����ϴ�",
                dupInfo.first->GetUID(), dupInfo.first->GetCID(), dupInfo.first->GetName(), pos->first, dupInfo.second);
        }
    }
}


void SendItemDuplicatedLog(CSendStream& SendStream, unsigned __int64 dwItemSerial,
                           Item::CItemOwnerInfo& itemOwnerInfo, unsigned long dwItemQty)
{
    PktItemDuplicated* lpPktItemDuplicated = 
        reinterpret_cast<PktItemDuplicated*>(SendStream.GetBuffer(sizeof(PktItemDuplicated)));

    if (0 != lpPktItemDuplicated)
    {
        lpPktItemDuplicated->m_cLogCmd = PktServerLog::ITEM_DUPLICATED_LOG;

        strncpy(lpPktItemDuplicated->m_szName, itemOwnerInfo.GetName(), PktItemDuplicated::MAX_NAME - 1);
        lpPktItemDuplicated->m_szName[PktItemDuplicated::MAX_NAME - 1] = 0;

        lpPktItemDuplicated->m_dwItemSerial = dwItemSerial;
        lpPktItemDuplicated->m_dwUID        = itemOwnerInfo.GetUID();
        lpPktItemDuplicated->m_dwCID        = itemOwnerInfo.GetCID();
        lpPktItemDuplicated->m_dwQty        = dwItemQty;

        SendStream.WrapHeader(sizeof(PktItemDuplicated), CmdServerLog, 0, 0);
    }
}

void RealmSkill::RealmInchantAdd(CCharacter* lpCharacter)
{
	// edith 2010.01.02 ���� ������ ������ �ٲ�鼭 �������� ������ �����Ѵ�.
	return;

	// edith 2008.05.28 ���� ���庰 ���� ����
	// ���� ���� ����Ʈ ���� //
	unsigned char cLevel[2][6] =
	{
		0, 1, 1, 2, 2, 3,
		0, 0, 1, 1, 2, 2
	};

	if(!lpCharacter)
	{
		return;
	}

	unsigned char cPoint = lpCharacter->GetRealmPoint();

	if(!cPoint)
	{
		return;
	}	

	// HP ȿ��.
	if(cLevel[0][cPoint])
	{
		Skill::CAddSpell<CRealmHPSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpCharacter, 
			Skill::SpellType::REALM_SPELL, Skill::SpellID::RealmHP, cLevel[0][cPoint], CSpell::INFINITE_DURATION))(lpCharacter);
	}

	// MP ȿ��.
	if(cLevel[1][cPoint])
	{
		Skill::CAddSpell<CRealmMPSpell>(CSpell::Spell_Info(Skill::CProcessTable::ProcessInfo::m_NullProtoType, lpCharacter, 
			Skill::SpellType::REALM_SPELL, Skill::SpellID::RealmMP, cLevel[1][cPoint], CSpell::INFINITE_DURATION))(lpCharacter);
	}
}

void RealmSkill::RealmInchantRemove(CCharacter* lpCharacter)
{
	if(!lpCharacter)
		return;

	// HP ȿ��.
	if(lpCharacter->GetSpellMgr().GetAffectedInfo().GetSpell(Skill::SpellID::RealmHP))
	{
		lpCharacter->GetSpellMgr().GetAffectedInfo().RemoveEnchantBySpellType(Skill::SpellID::RealmHP);
	}

	// MP ȿ��.
	if(lpCharacter->GetSpellMgr().GetAffectedInfo().GetSpell(Skill::SpellID::RealmMP))
	{
		lpCharacter->GetSpellMgr().GetAffectedInfo().RemoveEnchantBySpellType(Skill::SpellID::RealmMP);
	}
}