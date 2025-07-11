
#include "stdafx.h"
#include "Character.h"

#include <Network/Dispatch/GameClient/GameClientDispatch.h>
#include <Network/Dispatch/GameClient/SendCharEtc.h>
#include <Network/Dispatch/GameClient/SendCharAttack.h>
#include <Network/Dispatch/GameClient/SendCharLoginOut.h>
#include <Network/Dispatch/GameClient/SendCharCommunity.h>
#include <Network/Dispatch/DBAgent/DBAgentRequest.h>
#include <Network/Dispatch/Chat/ChatDispatch.h>

#include <Network/Packet/ChatPacket.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/CharStatusPacket.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>

#include <Item/Item.h>
#include <Item/ItemFactory.h>

#include <Map/FieldMap/Cell.h>
#include <Map/FieldMap/CellManager.h>

#include <Community/Party/Party.h>
#include <Creature/CreatureManager.h>
#include <Creature/Character/CharRespawnMgr.h>
#include <Log/CharacterLog.h>
#include <Utility/Math/Math.h>
#include <utility/Setup/ServerSetup.h>


bool CCharacter::DropItem(unsigned short usProtoTypeID, unsigned char cNum)
{
	Item::CItem* lpItem = Item::CItemFactory::GetInstance().CreateItem(usProtoTypeID);
	if (NULL != lpItem)
	{
		lpItem->SetNumOrDurability(min(cNum, lpItem->GetMaxNumOrDurability()));

		// edith 2009.08.19 DropItem ���� ��ɾ�� ��� ����� �����ڵ� ����.
		Item::CEquipment* lpEquip = Item::CEquipment::DowncastToEquipment(lpItem);
		if(lpEquip && lpEquip->GetSeasonRecord() == 0)
			lpEquip->SetNewEquip();

		if (false == GiveItem(lpItem))
		{
            ERRLOG2(g_Log, "CID:%10u %d������ �������� ������ ���߽��ϴ�.", 
				GetCID(), usProtoTypeID);                   

			DELETE_ITEM(lpItem);
			return false;
		}

		// GievItem ���� ���õ� ���
		if (lpItem->IsSet(Item::DetailData::STACKABLE) && 0 == lpItem->GetNumOrDurability())
		{
			DELETE_ITEM(lpItem);
		}
	}

	return true;
}

bool CCharacter::MovePos(Position Pos, char cZone, const bool bSitDown)
{
    if (0 != CCellManager::GetInstance().GetCell(m_CellPos.m_wMapIndex, Pos.PositionToPOS()))
    {
		DETLOG5(g_Log, "CID:%10u ���� ����� ����Ͽ����ϴ�. NewPos : %.1f, %.1f, %.1f, Zone : %d", 
			m_dwCID, Pos.m_fPointX, Pos.m_fPointY, Pos.m_fPointZ, cZone);

		// edith 2009.07.21 ������ ��Ż ���� ���� ��ǥ�����ϱ� (�ҹ��̵� ���� ���� ���ϱ� ���ؼ�)
		m_CurrentPos.m_fPointX = Pos.m_fPointX;
		m_CurrentPos.m_fPointZ = Pos.m_fPointZ;
		return GameClientSendPacket::SendCharBindPosition(*this, 0, PktBP::BP_WARP, Pos, cZone, 0);
	}

	return false;
}

bool CCharacter::MoveZone(POS NewPos, char cZone, char Channel)
{
    if (IsOperationFlagSet(CCharacter::MOVEZONE_PROCESSED))
    {
        return true;
    }

	// BG_TODO : ��Ʋ �׶��� ���� ������ �۾��� �߰��ؾ� �Ѵ�.
/*
	if (cZone == SERVER_ID::ZONE3)
	{
		SYSTEMTIME systemTime;
		GetSystemTime(&systemTime);

		// ��Ʋ �׶��� �޽� �ð����ȿ��� ������ �Ҽ� ����.
		if ((systemTime.wMinute >= CCreatureManager::STATUE_REST_TIME_1ST_START && systemTime.wMinute <= CCreatureManager::STATUE_REST_TIME_1ST_END) ||
			(systemTime.wMinute >= CCreatureManager::STATUE_REST_TIME_2ND_START && systemTime.wMinute <= CCreatureManager::STATUE_REST_TIME_2ND_END))
		{
			return false;
		}
	}
*/  
    if (NULL != m_lpGameClientDispatch)
    {
	    CDBRequest  DBRequest(*m_lpGameClientDispatch, 600);

        if (DBRequest.IsValid())
        {
            if (GameClientSendPacket::SendMoveZoneToDBAgent(DBRequest.GetSendStream(), 
                NewPos, DBRequest.GetRequestKey(), GetUID(), cZone, Channel))
            {
				char* szPeaceMode = (true == IsPeaceMode()) ? "��ȭ���" : "������";

				DETLOG5(g_Log, "UID:%d/CID:%10u(0x%p)/RequestKey:%d/DispatchPointer:0x%p �� �̵� ����� ����Ͽ����ϴ�.", 
					m_dwUID, m_dwCID, this, DBRequest.GetRequestKey(), m_lpGameClientDispatch);
				
                DETLOG8(g_Log, "UID:%d/CID:%10u/ ���̵� ����� �����Դϴ�. NewPos : (%.1f, %.1f, %.1f), Zone : %d, Channel : %d (%s)", 
					m_dwUID, m_dwCID, NewPos.fPointX, NewPos.fPointY, NewPos.fPointZ, cZone, Channel, szPeaceMode);

                GAMELOG::LogZoneMove(*this, cZone, Channel, 0);

                SetOperationFlag(CCharacter::MOVEZONE_PROCESSED);
                m_lpGameClientDispatch->PushRequestKey(DBRequest.GetRequestKey());
                return true;
            }

			DBRequest.CancelRequest();
        }
    }

	return false;
}

// �����, �������� �ش��ϴ� ������ �̵�
void CCharacter::MoveToGuildWarZone()
{
	const Position targetPos = CCharRespawnMgr::GetInstance().GetTownRespawnPos(SERVER_ID::CAPITAL, GetNation());

	if (CServerSetup::GetInstance().GetServerZone() == SERVER_ID::CAPITAL)
	{
		MoveTo(targetPos, false);

		POS pos;
		pos.fPointX = targetPos.m_fPointX;
		pos.fPointY = targetPos.m_fPointY;
		pos.fPointZ = targetPos.m_fPointZ;

		GameClientSendPacket::SendCharBindPosition(*this, 0, PktBP::BP_WARP, pos, CServerSetup::GetInstance().GetServerZone(), 0);
	}
	else
	{
		POS pos;
		pos.fPointX = targetPos.m_fPointX;
		pos.fPointY = targetPos.m_fPointY;
		pos.fPointZ = targetPos.m_fPointZ;

		MoveZone(pos, SERVER_ID::CAPITAL, -1);
	}
}

void CCharacter::MoveToRealmWarZone()
{
	const int iLevel = GetLevel();
	int iZone = SERVER_ID::STONE_WAR3;

	// edith 2009.06.13 ����� ������ ����
	if(iLevel <= 60)
		iZone = SERVER_ID::STONE_WAR3;  // ZONE16 STONE_WAR1
	else if(iLevel <= 80)
		iZone = SERVER_ID::CAPITAL;  // ZONE18 was STONE_WAR3
//	else
//		iZone = SERVER_ID::STONE_WAR3;

	const Position targetPos = CCharRespawnMgr::GetInstance().GetTownRespawnPos(iZone, GetNation());


	if (CServerSetup::GetInstance().GetServerZone() == iZone)
	{
		MoveTo(targetPos, false);

		POS pos;
		pos.fPointX = targetPos.m_fPointX;
		pos.fPointY = targetPos.m_fPointY;
		pos.fPointZ = targetPos.m_fPointZ;

		GameClientSendPacket::SendCharBindPosition(*this, 0, PktBP::BP_WARP, pos, CServerSetup::GetInstance().GetServerZone(), 0);
	}
	else
	{
		POS pos;
		pos.fPointX = targetPos.m_fPointX;
		pos.fPointY = targetPos.m_fPointY;
		pos.fPointZ = targetPos.m_fPointZ;

		MoveZone(pos, iZone, -1);
	}
}

bool CCharacter::Kill(CAggresiveCreature* lpAttacker)
{
	if (true == Dead(NULL))
	{
		AtType Type = {0,};
		Type.m_wType = 1;

		const unsigned short wDamage = m_CreatureStatus.m_nNowHP;
		m_CreatureStatus.m_nNowHP = 0;

		// �����ڰ� NULL �̸� ��ڰ� �ٸ� ���� �ִ� ĳ���͸� ��� ������� ���� ����̴�.
		// �̷����� �ڱⰡ ���ΰ����� ó���Ѵ�.
		if (NULL == lpAttacker) lpAttacker = this;

		if (NULL != m_lpGameClientDispatch)
		{
			return GameClientSendPacket::SendCharAttacked(m_lpGameClientDispatch->GetSendStream(), 
				lpAttacker, this, Type, 0, wDamage, 0, 0, PktBase::NO_SERVER_ERR);
		}

		return true;
	}

	return false;
}

bool CCharacter::NotifyInfo(unsigned long dwAdminCID)
{
	char szMessage[PktChat::PktChatMaxSize] = "";

    int nLen = _snprintf(szMessage, PktChat::PktChatMaxSize, 
		"Name:%s, Class:%d, HP:%3d%%, MP:%3d%%, Zone:%d, X:%.1f, Y:%.1f, Z:%.1f, "
        "MinD:%d, MaxD:%d, HitRate:%d, Evade:%d, Armor:%d", 

        GetCharacterName(), m_DBData.m_Info.Class, 
        m_CreatureStatus.m_nNowHP * 100 / m_CreatureStatus.m_StatusInfo.m_nMaxHP, 
        m_CreatureStatus.m_nNowMP * 100 / m_CreatureStatus.m_StatusInfo.m_nMaxMP, 
		CServerSetup::GetInstance().GetServerZone(), m_CurrentPos.m_fPointX, m_CurrentPos.m_fPointY, m_CurrentPos.m_fPointZ,
        
        m_CreatureStatus.m_StatusInfo.m_lMinDamage, m_CreatureStatus.m_StatusInfo.m_lMaxDamage,
        m_CreatureStatus.m_StatusInfo.m_wHitRate,
        m_CreatureStatus.m_StatusInfo.m_wEvade,
        m_CreatureStatus.m_StatusInfo.m_wArmor);

    if (0 < nLen)
    {
        szMessage[PktChat::PktChatMaxSize - 1] = 0;

		// ä�� ������ ������ ������.
		GET_SINGLE_DISPATCH(lpChatDispatch, CChatDispatch, 
			CChatDispatch::GetDispatchTable());

		if (0 != lpChatDispatch)
		{
			const Position& pos = GetCurrentPos();
			char strAdminCID[CHAR_INFOST::MAX_NAME_LEN] = "";
			strcpy(strAdminCID, "0x");
			char* strHexPos = (strAdminCID + 2);
			Math::Convert::Hex32ToStr(strHexPos, dwAdminCID);

			CChatRequestPacket chatReqPacket(szMessage, 
				PktChat::NOTIFY_CHAR_INFO, 0, GetUID(), GetCID(), 
				static_cast<unsigned short>(pos.m_fPointX), 
				static_cast<unsigned short>(pos.m_fPointY),
				static_cast<unsigned short>(pos.m_fPointZ),
				strAdminCID, 1);

			if (chatReqPacket.IsValid())
			{
				return lpChatDispatch->GetSendStream().PutBuffer(
					chatReqPacket.GetCompressedPacket(),
					chatReqPacket.GetCompressedSize(), CmdCharChat);
			}
		}

		return true;
    }

    return false;
}

bool CCharacter::DuelInit(unsigned char cCmd)
{
	// ������̶�� ��� ���
	CCharacter* lpDuelOpponent = static_cast<CCharacter*>(GetDuelOpponent());
	if (NULL != lpDuelOpponent)
	{
		CGameClientDispatch* lpOpponentDispatch = lpDuelOpponent->GetDispatcher();
		if (NULL != lpOpponentDispatch)
		{
			GameClientSendPacket::SendCharDuelCmd(lpOpponentDispatch->GetSendStream(), m_dwCID, 
				lpDuelOpponent->GetCID(), cCmd, PktDuC::NO_SERVER_ERR);
		}
		if (NULL != m_lpGameClientDispatch)
		{
			GameClientSendPacket::SendCharDuelCmd(m_lpGameClientDispatch->GetSendStream(), m_dwCID, 
				lpDuelOpponent->GetCID(), cCmd, PktDuC::NO_SERVER_ERR);
		}

		lpDuelOpponent->SetDuelOpponent(NULL);
        SetDuelOpponent(NULL);
	}

	// ����Ʋ���� ����
	if (NULL != m_pParty)
	{
		if (NULL != reinterpret_cast<CCharacterParty* >(m_pParty)->GetHostileParty())
		{
			if (PktDuC::DUC_CANCEL == cCmd || 
				0 == reinterpret_cast<CCharacterParty* >(m_pParty)->DropMember(this, static_cast<PktDuC::DuelCmd>(cCmd)))
			{
				reinterpret_cast<CCharacterParty* >(m_pParty)->GetHostileParty()->EndTeamBattle();
				reinterpret_cast<CCharacterParty* >(m_pParty)->EndTeamBattle();
			}
		}
	}

	if(cCmd == PktDuC::DUC_DEAD)
	{
		return false;
	}
	return true;
}
