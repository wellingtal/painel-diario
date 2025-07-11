
#include "stdafx.h"

#include <Creature/CreatureManager.h>
#include <Creature/Character/Character.h>
#include <Creature/Character/ExpTable.h>
#include <Creature/Character/CharRespawnMgr.h>

#include <Map/FieldMap/VirtualArea/VirtualArea.h>
#include <Map/FieldMap/VirtualArea/VirtualAreaMgr.h>

#include <Network/Dispatch/ParseUtils.h>
#include <Network/Dispatch/Chat/ChatDispatch.h>

#include <Network/Packet/PacketStruct/ServerInfo.h>
#include <Network/Packet/PacketStruct/CharStatusPacket.h>
#include <Network/Packet/PacketStruct/CharItemPacket.h>
#include <Network/Packet/PacketStruct/CharItemPacketStruct.h>
#include <Network/Packet/PacketStruct/CharCommunityPacket.h>
#include <Network/Packet/PacketStruct/CharConfigPacket.h>
#include <Network/Packet/PacketStruct/RankingPacket.h>

#include <Network/Dispatch/GameClient/SendCharAttack.h>

#include <Utility/Setup/ServerSetup.h>
#include <Ranking/RankingMgr.h>

#include "GameClientDispatch.h"
#include "SendCharEtc.h"
#include "ParseCharEtc.h"


// ĳ���� �ڻ�	
bool GameClientParsePacket::ParseCharSuicide(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
    CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktSC), GameClientDispatch);

	PktSC*      lpPktSC     = static_cast<PktSC*>(lpPktBase);
    CCharacter* lpCharacter = GameClientDispatch.GetCharacter();
    
    CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	unsigned long dwCharID = lpPktSC->m_dwCharID;
	Position Pos = Position::Position();

	lpCharacter->Dead(NULL);
	if (0 != lpCharacter->GetMapIndex())
	{
		VirtualArea::CVirtualArea* lpVirtualArea = VirtualArea::CVirtualAreaMgr::GetInstance().GetVirtualArea( lpCharacter->GetMapIndex() );
		if (NULL != lpVirtualArea)
		{
			Pos = lpVirtualArea->GetRespawnPosition(lpCharacter->GetRace(), 0);
		}
	}

	lpCharacter->Respawn(CCharRespawnMgr::RST_TOWN, Pos);
	lpCharacter->SaveToDBData();

    if (0 != lpCharacter->GetPID())
    {		
        // ��Ƽ�� �������� �˸���.
        GameClientSendPacket::SendCharDeadToParty(lpCharacter, 0, PktDeadInfo::RESPAWN);
    }

	return true;
}


// ĳ���� ���ε� ������	
bool GameClientParsePacket::ParseCharBindPosition(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
    CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktBP), GameClientDispatch);

	PktBP*      lpPktBP     = static_cast<PktBP*>(lpPktBase);
    CCharacter* lpCharacter = GameClientDispatch.GetCharacter();
    
    CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	unsigned long dwNPCID = lpPktBP->m_dwNPCID;
	unsigned char cCmd = lpPktBP->m_cCmd;
	POS &Pos = lpPktBP->m_Pos;
	char cZone = lpPktBP->m_cZone;

	Position DestPos = Pos;

	switch (cCmd)	
	{
		case PktBP::BP_SAVE_POSITION:	// �� ��ġ ����
			lpCharacter->BindPositionToNPC(dwNPCID);			
			break;	

		case PktBP::BP_WARP:			// ����
			DETLOG5(g_Log, "CID:0x%08x ���� ����� ����Ͽ����ϴ�. NewPos : %.1f, %.1f, %.1f, Zone : %d", 
				lpCharacter->GetCID(), Pos.fPointX, Pos.fPointY, Pos.fPointZ, cZone);
			lpCharacter->MoveTo(DestPos, false);				
			break;
	}

	return GameClientSendPacket::SendCharBindPosition(*lpCharacter, 
        dwNPCID, cCmd, Pos, cZone, 0);
}

// ĳ���� �� ���� �̵�
bool GameClientParsePacket::ParseCharQuickSlotMove(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
    CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktQSM), GameClientDispatch);

	PktQSM*		lpPktQSM	= static_cast<PktQSM*>(lpPktBase);
    CCharacter*	lpCharacter = GameClientDispatch.GetCharacter();
    
    CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	TakeType takeType = lpPktQSM->m_TakeType;
	unsigned short usSkillID = lpPktQSM->m_usSkillID;
	unsigned char cLockCount = lpPktQSM->m_cLockCount;
	unsigned char cSkillLevel = lpPktQSM->m_cSkillLevel;

	unsigned short usError = 0;
		
	if (false == lpCharacter->MoveQuickSlot(takeType, usSkillID, cLockCount, cSkillLevel)) 
	{
		usError = PktQSM::FAIL_MOVE;
	}

	return GameClientSendPacket::SendCharQuickSlotMove(
		GameClientDispatch.GetSendStream(), takeType, usSkillID, usError);
}


// ĳ���� �ɼ� ����
bool GameClientParsePacket::ParseCharControlOption(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
    CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktCOp), GameClientDispatch);

	PktCOp*		lpPktCOp	= static_cast<PktCOp*>(lpPktBase);
    CCharacter*	lpCharacter = GameClientDispatch.GetCharacter();
    
    CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	unsigned long dwCharID = lpPktCOp->m_dwCharID;
	RejectOption Reject = lpPktCOp->m_RejectOption;

	unsigned short usError = 0;
		
	// �ź� �ɼ� ����
	if (false == lpCharacter->ControlOption(Reject, false)) 
	{	
		ERRLOG1(g_Log, "CID:0x%08x ĳ���� �ź� �ɼ� ���� ����.", dwCharID);	
		return false;
	}

	return GameClientSendPacket::SendCharControlOption(GameClientDispatch.GetSendStream(), dwCharID, Reject);
}


// ���� �ǳڿ� ���� ����
bool GameClientParsePacket::ParseCharAuthorizePanel(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktAP), GameClientDispatch);

	PktAP*		lpPktAP		= static_cast<PktAP *>(lpPktBase);
	CCharacter*	lpCharacter = GameClientDispatch.GetCharacter();

	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	unsigned long dwAffectedID = lpPktAP->m_dwAffectedID;
	unsigned long dwCasterID = lpPktAP->m_dwCasterID;
	unsigned char cCmd = lpPktAP->m_cCmd;

	switch (cCmd)
	{
		case PktAP::AP_RESURRECTION:
		{
			if (0 == lpCharacter->GetStatus().m_nNowHP)
			{
				if (SERVER_ID::ZONE3 == CServerSetup::GetInstance().GetServerZone())
				{
					CCreatureManager::GetInstance().PopRespawnQueue(lpCharacter);
				}

				if (false == lpCharacter->Respawn(CCharRespawnMgr::RST_RESURRECTION, lpCharacter->GetCurrentPos()))
				{	
					ERRLOG1(g_Log, "CID:0x%08x ĳ���� �������� (����) ����.", dwAffectedID);	
				}            

				if (0 != lpCharacter->GetPID())
				{		
					// ��Ƽ������ �������� �˸���.
					GameClientSendPacket::SendCharDeadToParty(lpCharacter, 
						CCreatureManager::GetInstance().GetCharacter(dwCasterID), PktDeadInfo::RESURRECT);
				}
			}
		} break;

		case PktAP::AP_RECALL:
		{
			CCharacter* lpCaster = CCreatureManager::GetInstance().GetCharacter(dwCasterID);
			if (NULL == lpCaster) 
			{ 
				ERRLOG1(g_Log, "CID:0x%08x ���ݿ� �����߽��ϴ�.", dwCasterID);	
				break; 
			}

			const Position& Pos = lpCaster->GetCastingPos();
			lpCharacter->MoveTo(Pos, false);

			// edith 2009.03.07 ���� ���� ����
			POS DestPos;
			DestPos.fPointX = Pos.m_fPointX;
			DestPos.fPointY = Pos.m_fPointY+0.3f;
			DestPos.fPointZ = Pos.m_fPointZ;

			return GameClientSendPacket::SendCharBindPosition(*lpCharacter, 
				dwCasterID, PktBP::BP_RECALL, DestPos, CServerSetup::GetInstance().GetServerZone(), 0);
		
		} break;
	}

	return true;
}

bool GameClientParsePacket::ParseCharNameCheck(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)//hz added for new packet
{
	CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktNameCheck), GameClientDispatch);

	PktNameCheck*		lpPktFI		= static_cast<PktNameCheck *>(lpPktBase);
	CCharacter*	lpCharacter2 = GameClientDispatch.GetCharacter();

	CHECK_CHARACTER_PTR(lpCharacter2, GameClientDispatch, lpPktBase->GetCmd());

	unsigned long dwCharID = lpPktFI->charID;
	CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter(dwCharID);
	if(lpCharacter)
		return false;

	if(lpCharacter->GetCharacterName() == lpPktFI->name)
		return true;
	else
	{
		CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
			if (NULL != lpDispatch)
			{
				lpDispatch->Disconnect();
				lpDispatch->Disconnected();
			}
	}
}

// ĳ���� ��(���嵵) ���� ��û
bool GameClientParsePacket::ParseCharFameInfo(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktNameCheck), GameClientDispatch);

	PktNameCheck*		lpPktFI		= static_cast<PktNameCheck *>(lpPktBase);
	CCharacter*	lpCharacter = GameClientDispatch.GetCharacter();
	unsigned long dwCharID = lpPktFI->charID;
	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());
	bool ok=true;
	if(lpPktFI->check == 1)//if i want to check the name hz name check hack
	{
		CCharacter* lpCharacter2 = CCreatureManager::GetInstance().GetCharacter(dwCharID);
		char charName[16]; 
		strncpy(charName,lpPktFI->name,16);
		int size1=strlen(charName);
		char* name=const_cast<char*>(lpCharacter2->GetCharacterName());
		int size2=strlen(name);
		if(size1 == size2)
		{
			for(int i=0;charName[i]!='\0';i++)
			{
				if(charName[i] != name[i])
				{
					ok=false;
					break;
				}
			}
		}
		else
			ok=false;
	
		if(ok)
		{
			return true;
		}
		else
		{
			CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
				if (NULL != lpDispatch)
				{
					lpDispatch->Disconnect();
					lpDispatch->Disconnected();
				}
		}
	}
	else
		return GameClientSendPacket::SendCharFameInfo(GameClientDispatch.GetSendStream(), 
				lpCharacter, "", "", 0, 0, PktFIAck::FAME_INFO, PktBase::NO_SERVER_ERR);
}


// ��ŷ ����
bool GameClientParsePacket::ParseCharRankingInfo(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktRankingInfo), GameClientDispatch);

	PktRankingInfo*	lpRankingInfo	= static_cast<PktRankingInfo *>(lpPktBase);
	CCharacter*		lpCharacter		= GameClientDispatch.GetCharacter();

	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	unsigned long dwCharID = lpRankingInfo->m_dwCharID;
	unsigned char cClass = lpRankingInfo->m_cClass;
	unsigned char cPage = lpRankingInfo->m_cPage;

	return CRankingMgr::GetInstance().SendRankingInfo(dwCharID, cClass, cPage);
}

bool GameClientParsePacket::ParseCastObjectInfo(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
    CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktCOInfo), GameClientDispatch);

    PktCOInfo* lpPktCOInfo = static_cast<PktCOInfo*>(lpPktBase);
    CCharacter*lpCharacter = GameClientDispatch.GetCharacter();

    CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

    if(lpCharacter->GetCID() == lpPktCOInfo->m_dwSenderID)
    {
        // ������ ID�� ���� ��쿡�� ����.
        CCell* lpCell = lpCharacter->GetCellPos().m_lpCell;

        if(0 != lpCell)
        {
            lpCell->SendCastObjectInfo(lpPktCOInfo->m_dwSenderID, 
                lpPktCOInfo->m_dwReceiverID, lpPktCOInfo->m_sCastObject);
        }
    }
	else if (lpCharacter->IsRideArms() && lpCharacter->GetRideArmsCID() == lpPktCOInfo->m_dwSenderID)
	{
		// ������ ID�� ���� Ÿ�� �ִ� ������ ��쿡�� ����.
		CCell* lpCell = lpCharacter->GetCellPos().m_lpCell;

		if(0 != lpCell)
		{
			lpCell->SendCastObjectInfo(lpPktCOInfo->m_dwSenderID, 
				lpPktCOInfo->m_dwReceiverID, lpPktCOInfo->m_sCastObject);
		}
	}

    return true;
}

