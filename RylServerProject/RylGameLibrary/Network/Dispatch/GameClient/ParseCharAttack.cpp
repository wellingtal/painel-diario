
#include "stdafx.h"

#include "ParseCharAttack.h"
#include "SendCharAttack.h"
#include "GameClientDispatch.h"
//hz added this for cooldown check
#include <Skill/SkillTable.h>
#include <Skill/Spell/SpellTable.h>
#include <Skill/Spell/GlobalSpellMgr.h>
//hz end
#include <Network/Dispatch/ParseUtils.h>
#include <Network/Dispatch/Chat/ChatDispatch.h>

#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/WrapPacket.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>
#include <Network/Packet/PacketStruct/CharMovePacket.h>
#include <Network/Packet/PacketStruct/CharAttackPacket.h>

#include <Creature/CreatureManager.h>
#include <Creature/Character/Character.h>
#include <Creature/Character/CharRespawnMgr.h>
#include <Creature/Character/SphereTree/CharSphereTree.h>
#include <Creature/Monster/Monster.h>
#include <Creature/Siege/SiegeObjectMgr.h>
#include <Creature/Siege/SiegeObject.h>

#include <Map/FieldMap/CellManager.h>
#include <Map/FieldMap/VirtualArea/VirtualArea.h>
#include <Map/FieldMap/VirtualArea/VirtualAreaMgr.h>

#include <Community/Party/Party.h>
#include <Log/ItemLog.h>
#include <Utility/Setup/ServerSetup.h>
#include <Utility/Math/Math.h>

//#include <Network/Dispatch/PacketDecrypt.h>
//hz added this for cooldown

bool CheckLastUseTime(CCharacter* lpCharacter, unsigned short SkillID, unsigned long cooldownTime)
{
	/*bool cast=false;
	if(lpCharacter->IsCast(SkillID))
	{
		cast=true;
	}
	if(!cast)
	{
		if(cooldownTime > 2000)
			cooldownTime /= 2;
	}
	else
		cooldownTime=static_cast<unsigned long>(cooldownTime*0.5f);*/

	cooldownTime /= 2;

	if((SkillID >= 0x8D01) && (SkillID <= 0x8D16))
	{
		int index=lpCharacter->GetPositionOfSkillInArr(SkillID);
		if(index == 12)
			return false;

		if(lpCharacter->LastUsePotionTime[index] == 0)
		{
			lpCharacter->LastUsePotionTime[index]=timeGetTime();
			return false;
		}

		long different=timeGetTime() - lpCharacter->LastUsePotionTime[index];

		if(different < cooldownTime)
		{
			return true;
		}
	}
	else
	{
		bool found=false;
		int index=lpCharacter->GetPositionOfSkillInBigArr(SkillID);
		if(index == 130)
			return false;

		found=true;
		if ((timeGetTime() - lpCharacter->LastUseSkillTime[index])  >= cooldownTime ){
			lpCharacter->LastUseSkillTime[index]=timeGetTime();
			return false;
		}
		else
		{
			return true;
		}

		return false;
	}
	return false;
}
//hz end

// ���� ��Ŷ
bool GameClientParsePacket::ParseCharAttack(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
/*	// ���� ���忡 ���� ��Ŷ ��ȣȭ
	// edith 2009.08.11 ���Ӱ��� 2.5 ���׷��̵�
	unsigned long dwOriginalSize = 
		PacketDecrypt(reinterpret_cast<void*>(lpPktBase + 1), lpPktBase->GetLen() - sizeof(PktBase));
	if (0 == dwOriginalSize)
	{
		GameClientDispatch.PrintGameGuardError();
		return false;
	}
	lpPktBase->SetLen(static_cast<PktBase::LengthType>(sizeof(PktBase) + dwOriginalSize));
*/

	CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktAt), GameClientDispatch);

	PktAt*      lpPktAt     = static_cast<PktAt*>(lpPktBase);
	CCharacter* lpCharacter = GameClientDispatch.GetCharacter();

	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());


	unsigned short usError = PktBase::NO_SERVER_ERR;

	unsigned long dwCharID = lpPktAt->m_dwCharID;

	AtType AtType = lpPktAt->m_AtType;
	AtNode AtNode = lpPktAt->m_AtNode;

	lpCharacter->MoveTo(lpPktAt->m_Postion, false);
	lpCharacter->SetDirection(lpPktAt->m_fDir);

	// ���� ������Ʈ�� ž���ϰ� �ִٸ�, ���� ������Ʈ�� ������ �������ش�.
	if (Creature::CT_SIEGE_OBJECT == Creature::GetCreatureType(dwCharID) &&	lpCharacter->IsRideArms())
	{
		CSiegeObject* lpArms = CSiegeObjectMgr::GetInstance().GetSiegeObject(dwCharID);
		if (lpArms)
		{
			lpArms->MoveTo(lpPktAt->m_Postion);
			lpArms->SetDirection(lpPktAt->m_fDir);
		}

		if (false == lpArms->AttackCID(lpCharacter, AtType, AtNode, usError))
		{
			if (PktBase::NO_SERVER_ERR == usError) 
			{
				usError = PktAtAck::SERVER_ERROR;
			}

			CSendStream& SendStream = GameClientDispatch.GetSendStream();
			char* lpBuffer = SendStream.GetBuffer(sizeof(PktAtAck));
			if (NULL != lpBuffer) 
			{
				PktAtAck* lpPktAtAck = reinterpret_cast<PktAtAck*>(lpBuffer);

				lpPktAtAck->m_dwCharID = 0;        

				lpPktAtAck->m_wHP = 0;
				lpPktAtAck->m_wMP = 0;
				lpPktAtAck->m_wMPHeal = 0;

				lpPktAtAck->m_cJudge = 0;
				lpPktAtAck->m_cDefenserNum = 0;

				return SendStream.WrapCrypt(sizeof(PktAtAck), CmdCharAttack, 0, usError);
			}
		}
	}
	else
	{
		// ĳ����
		if (0 != AtType.m_cCasting)
		{
			lpCharacter->Casting(AtType, AtNode);
		}
		// ����
		else
		{
	/*		bool doSkill=false;
	if(AtType.m_wType > 100)//skill
	{
		const Skill::ProtoType* pThisSkill = CSkillMgr::GetInstance().GetSkillProtoType(AtType.m_wType);
		if (0 == pThisSkill) 
		{ 
			return false;
		}

		int index=lpCharacter->GetPositionOfSkillInBigArr(AtType.m_wType);
		if(index != 130)
		{
			if(CheckLastUseTime(lpCharacter,pThisSkill->m_usSkill_ID,pThisSkill->m_dwCoolDownTime))//need to get the actual cooldown
			{
				CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
				if (NULL != lpDispatch)
				{
					lpDispatch->Disconnect();
					lpDispatch->Disconnected();
				}
				return false;
			}
		}
			
	}*/


			if (false == lpCharacter->AttackCID(AtType, AtNode, usError)) 
			{
				if (PktBase::NO_SERVER_ERR == usError) 
				{
					usError = PktAtAck::SERVER_ERROR;
				}

				CSendStream& SendStream = GameClientDispatch.GetSendStream();
				char* lpBuffer = SendStream.GetBuffer(sizeof(PktAtAck));
				if (NULL != lpBuffer) 
				{
					PktAtAck* lpPktAtAck = reinterpret_cast<PktAtAck*>(lpBuffer);

					lpPktAtAck->m_dwCharID = 0;        

					lpPktAtAck->m_wHP = 0;
					lpPktAtAck->m_wMP = 0;
					lpPktAtAck->m_wMPHeal = 0;

					lpPktAtAck->m_cJudge = 0;
					lpPktAtAck->m_cDefenserNum = 0;

					return SendStream.WrapCrypt(sizeof(PktAtAck), CmdCharAttack, 0, usError);
				}
			}
		}
	}

	return true;
}


// �չٲٱ�
bool GameClientParsePacket::ParseCharSwitchEQ(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktSwEQ), GameClientDispatch);

	PktSwEQ*		lpPktSwEQ	= static_cast<PktSwEQ*>(lpPktBase);
	CCharacter*	lpCharacter	= GameClientDispatch.GetCharacter();

	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	unsigned char cType = lpPktSwEQ->m_cType;
	unsigned char cSelect = lpPktSwEQ->m_cSelect;

	bool bResult = false;

	if(cType == 0)
	{
		bResult = lpCharacter->ChangeWeaponAndShield(cSelect);
		GAMELOG::LogChangeWeapon(*lpCharacter, cSelect);
	}
	else if(cType == 1)
	{
		// edith 2008.06.23 ��Ÿ�� ������
		// ���� Ÿ�� ������.
		bResult = lpCharacter->ChangeRide(cSelect);
		GAMELOG::LogChangeRide(*lpCharacter, cSelect);
	}

	return bResult;
}


// ĳ���� ������
bool GameClientParsePacket::ParseCharRespawn(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktRs), GameClientDispatch);

	PktRs*		lpPktRs	= static_cast<PktRs*>(lpPktBase);
	CCharacter*	lpCharacter	= GameClientDispatch.GetCharacter();

	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	unsigned long dwCharID = lpPktRs->m_dwCharID;
	unsigned char cCmd = lpPktRs->m_cCmd;
	unsigned long dwTownID = lpPktRs->m_dwTownID;
	Position DestPos = lpPktRs->m_Position;

	switch (cCmd)
	{
	case PktRs::RS_NORMAL:
		{
			if (false == lpCharacter->Respawn())
			{	
				ERRLOG1(g_Log, "CID:0x%08x ĳ���� ������ (����) ����.", dwCharID);
			}
            else if (0 != lpCharacter->GetPID())
            {		
                // ��Ƽ�� �������� �˸���.
                GameClientSendPacket::SendCharDeadToParty(lpCharacter, 0, PktDeadInfo::RESPAWN);
            }
			break;
		} 

	case PktRs::RS_SELECT:
		{
			// �������ҷ��� ĳ���� �߰�
			// ... �ùٸ� ������ �˻�� �Ŀ� �߰�����!!
            unsigned char eRet = CCharRespawnMgr::GetInstance().AddCharacter(dwTownID, dwCharID);

			switch (eRet)
			{
			case CCharRespawnMgr::RET_TRUE:
				{
					// edith 2009.08.26 �������� �߰� ����
					lpCharacter->SetRespawnTownID(dwTownID);

					// ������ ���� ���� ������ �ٽ� �ѹ� �����ش�.
					CCharRespawnMgr::GetInstance().SendRespawnAreaInfo(dwCharID, dwTownID, true);
				}
				break;

			case CCharRespawnMgr::RET_TOWN:
				{
					// �ٷ� ������
					if (false == lpCharacter->Respawn(CCharRespawnMgr::RST_TOWN, CCharRespawnMgr::GetInstance().GetRespawnPos(dwTownID)))
					{	
						ERRLOG1(g_Log, "CID:0x%08x ĳ���� ������ (����) ����.", dwCharID);
					}
                    else if (0 != lpCharacter->GetPID())
                    {		
                        // ��Ƽ�� �������� �˸���.
                        GameClientSendPacket::SendCharDeadToParty(lpCharacter, 0, PktDeadInfo::RESPAWN);
                    }
					break;
				}
				break;

			case CCharRespawnMgr::RET_FALSE:
				{
                    ERRLOG2(g_Log, "CID:0x%08x TownID : 0x%08x ĳ���� ������ (����) ����.", dwCharID, dwTownID);

					CSendStream& SendStream = GameClientDispatch.GetSendStream();
					char* lpBuffer = SendStream.GetBuffer(sizeof(PktRsAck));
					if (NULL != lpBuffer)
					{
						PktRsAck* lpPktRsAck = reinterpret_cast<PktRsAck*>(lpBuffer);
						::memset(lpPktRsAck, 0, sizeof(PktRsAck));
						SendStream.WrapCrypt(sizeof(PktRsAck), CmdCharRespawn, 0, PktRsAck::ERR_NOT_EXIST_CAMP);
					}
				}
				break;
			}

			break;
		} 

	case PktRs::RS_FORMER_PLACE:
		{
			// TODO : ���� ��� ���ڸ� ������
			break;
		} 

	case PktRs::RS_BG_HUMAN1:
	case PktRs::RS_BG_HUMAN2:
	case PktRs::RS_BG_HUMAN3:
	case PktRs::RS_BG_AKHAN1:
	case PktRs::RS_BG_AKHAN2:
	case PktRs::RS_BG_AKHAN3:
		{
			if (SERVER_ID::ZONE3 == CServerSetup::GetInstance().GetServerZone())
			{
				// TODO : �ٸ� Ŀ�ǵ� ������ ��ȣ�� 3�� �и� �����̴�. Ŭ���̾�Ʈ ���� ���� �� Ŀ��Ʈ �ѹ����� �ٲ㼭 ����ϰ� ó�� ����
				CCreatureManager::GetInstance().PushRespawnQueue(lpCharacter, cCmd - 3);
			}
			else
			{
				ERRLOG1(g_Log, "CID:0x%08x ��Ʋ�׶��尡 �ƴ� ������ ��Ʋ�׶����� ������ ������ �����Ͽ����ϴ�.", dwCharID);	
			}
			break;
		}

		// BG_TODO : ������ ó�� (Ŭ���̾�Ʈ�� �̰� ������ �Ѵ�.)
	case PktRs::RS_BGSERVER_RANDOM:
		{
			if (lpCharacter->GetRace() > CClass::MAX_RACE)
			{
				ERRLOG2(g_Log, "CID:0x%08x ĳ������ ������ �̻��մϴ�. ����:%d", dwCharID, lpCharacter->GetRace());	
				break;
			}

			VirtualArea::CVirtualArea* lpVirtualArea = VirtualArea::CVirtualAreaMgr::GetInstance().GetVirtualArea( lpCharacter->GetMapIndex() );
			if (NULL == lpVirtualArea)
			{
				ERRLOG2(g_Log, "CID:0x%08x ĳ���Ͱ� �ִ� ���� ��(MapIndex : %d)�� �������� �ʽ��ϴ�.", dwCharID, lpCharacter->GetMapIndex());
				break;
			}

			DestPos = lpVirtualArea->GetRespawnPosition( lpCharacter->GetRace(), Math::Random::ComplexRandom( lpVirtualArea->GetMaxRespawnPos() ) );
			if (false == lpCharacter->Respawn(CCharRespawnMgr::RST_BATTLE, DestPos))
			{	
				ERRLOG1(g_Log, "CID:0x%08x ĳ���� ������ (����) ����.", dwCharID);	
			}
            else if (0 != lpCharacter->GetPID())
            {		
                // ��Ƽ�� �������� �˸���.
                GameClientSendPacket::SendCharDeadToParty(lpCharacter, 0, PktDeadInfo::RESPAWN);
            }
			break;
		}

	case PktRs::RS_BG_EXIT:
		{
			if (SERVER_ID::ZONE3 == CServerSetup::GetInstance().GetServerZone())
			{
				unsigned char cZone = SERVER_ID::NONE;
				switch (lpCharacter->GetRace())
				{
				case CClass::HUMAN:
					cZone = SERVER_ID::ZONE1;//was zone1 // WAS ZONE12
					break;

				case CClass::AKHAN:
					cZone = SERVER_ID::ZONE2;//was zone2 // WAS ZONE12
					break;

				default:
					ERRLOG2(g_Log, "CID:0x%08x ĳ������ ������ �̻��մϴ�. ����:%d", dwCharID, lpCharacter->GetRace());	
					return true;
				}

				lpCharacter->GetStatus().m_nNowHP = lpCharacter->GetStatus().m_StatusInfo.m_nMaxHP;
				lpCharacter->GetStatus().m_nNowMP = lpCharacter->GetStatus().m_StatusInfo.m_nMaxMP;

				// ä���� 2�� �̻� �ִ� ��� ä��â�� ����ְ� ���̵��� ���� �ʽ��ϴ�. 
				// �� ��� ä��â�� ������� ��Ƽ�� ���� ���� ���� �����Ƿ� 1ä�η� ���� �̵���Ű�� �����Ͽ����ϴ�. (2004-09-20 by �ε�)
				if (false == lpCharacter->MoveZone(lpPktRs->m_Position, cZone, 0))
				{
					ERRLOG1(g_Log, "CID:0x%08x ��Ʋ�׶��� Ż�⿡ �����Ͽ����ϴ�.", dwCharID);	
					return true;
				}
			}
			else
			{
				ERRLOG1(g_Log, "CID:0x%08x ��Ʋ�׶��尡 �ƴ� ������ ��Ʋ�׶��� Ż���� �õ��մϴ�.", dwCharID);	
			}

			break;
		}

	default:
		{
			ERRLOG1(g_Log, "CID:0x%08x �� �� ���� ������ Ŀ�ǵ��Դϴ�.", dwCharID);	
			break;
		} 
	}

	return true;
}


// ������ ť ����� ���� (��Ʋ�׶����)
bool GameClientParsePacket::ParseCharRespawnWaitQueue(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktRsWaitQueue), GameClientDispatch);

	PktRsWaitQueue*	lpRsWQ = static_cast<PktRsWaitQueue *>(lpPktBase);
	CCharacter*	lpCharacter	= GameClientDispatch.GetCharacter();

	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	unsigned long dwCharID = lpRsWQ->m_dwCharID;

	return CCreatureManager::GetInstance().SendRespawnQueue(dwCharID);
}

// ������ ���� (������ ��ġ��)
bool GameClientParsePacket::ParseCharRespawnInfo(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktRsInfo), GameClientDispatch);

	PktRsInfo*	lpRsInfo = static_cast<PktRsInfo *>(lpPktBase);
	CCharacter*	lpCharacter	= GameClientDispatch.GetCharacter();

	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	unsigned long dwCharID = lpRsInfo->m_dwCharID;

	return CCharRespawnMgr::GetInstance().SendRespawnInfo(dwCharID);
}

// ������ ���� ���� ����
bool GameClientParsePacket::ParseCharRespawnAreaInfo(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktRsAreaInfo), GameClientDispatch);

	PktRsAreaInfo*	lpRsAreaInfo = static_cast<PktRsAreaInfo *>(lpPktBase);
	CCharacter*	lpCharacter	= GameClientDispatch.GetCharacter();

	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	unsigned long dwCharID = lpRsAreaInfo->m_dwCharID;
	unsigned long dwTownID = lpRsAreaInfo->m_dwTownID;

	return CCharRespawnMgr::GetInstance().SendRespawnAreaInfo(dwCharID, dwTownID);
}


// ĳ���� ������ ������Ʈ
/*
bool GameClientParsePacket::ParseCharMoveUpdate(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktMU), GameClientDispatch);

	PktMU*		lpPktMU	= static_cast<PktMU*>(lpPktBase);
	CCharacter*	lpCharacter	= GameClientDispatch.GetCharacter();

	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	POS	Pos	= lpPktMU->m_Position;
	bool bSitMode = lpPktMU->m_bSitMode;

	Position DestPos = Pos;
	switch (lpCharacter->MoveTo(DestPos, bSitMode))
	{
	case CCreature::S_CELL_CHANGED:			break;	// �� ü����
	case CCreature::S_CELL_UNCHANGED:		break;	// �̵�
	case CCreature::F_CELL_CANNOTCHANGE:	break;	// �� ü���� ����
	case CCreature::F_TOO_FAST:				break;	// �̵� Ʀ
	}

	if (lpCharacter->IsRideArms())
	{
		CSiegeObject* lpArms = CSiegeObjectMgr::GetInstance().GetSiegeObject(lpCharacter->GetRideArmsCID());
		if (lpArms)
		{
			lpArms->MoveTo(DestPos);
		}
	}

	return true;
}
*/


// ��� ��Ŷ ���
bool GameClientParsePacket::ParseCharDuelCmd(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktDuC), GameClientDispatch);

	PktDuC*		lpPktDuC	= static_cast<PktDuC*>(lpPktBase);
	CCharacter*	lpCharacter	= GameClientDispatch.GetCharacter();

	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	unsigned long dwSenderID = lpPktDuC->m_dwSenderID;
	unsigned long dwRecverID = lpPktDuC->m_dwRecverID;
	unsigned char cCmd = lpPktDuC->m_cCmd;

	// ĳ���� ���
	CCharacter* lpSCharacter = CCreatureManager::GetInstance().GetCharacter(dwSenderID); 
	CCharacter* lpRCharacter = CCreatureManager::GetInstance().GetCharacter(dwRecverID); 
	if (NULL == lpSCharacter || NULL == lpRCharacter)
	{	
		GameClientSendPacket::SendCharDuelCmd(GameClientDispatch.GetSendStream(), dwSenderID, dwRecverID, cCmd, PktDuC::SERVER_ERROR);
		ERRLOG2(g_Log, "CID:0x%08x (Recver), SenderCID:0x%08x ĳ���� ��� ����.", dwRecverID, dwSenderID);	
		return true;
	}

	CGameClientDispatch* lpSendDispatch = lpSCharacter->GetDispatcher();
	CGameClientDispatch* lpRecvDispatch = lpRCharacter->GetDispatcher();

	switch (cCmd)
	{
	case PktDuC::DUC_PROPOSE:		// ����
		{
			if (NULL != lpSCharacter->GetDuelOpponent() || NULL != lpRCharacter->GetDuelOpponent())
			{
				GameClientSendPacket::SendCharDuelCmd(GameClientDispatch.GetSendStream(), dwSenderID, dwRecverID, cCmd, PktDuC::FAIL_DUELING_CHAR);
				break;	
			}

			if (lpRCharacter->GetRejectOption().Reject.m_Duel ||
				lpRCharacter->GetBanList().IsBan(lpCharacter->GetCID(), lpCharacter->GetCharacterName()))
			{
				GameClientSendPacket::SendCharDuelCmd(GameClientDispatch.GetSendStream(), dwSenderID, dwRecverID, cCmd, PktDuC::FAIL_REJECT);
				break;	
			}

			if (NULL != lpRecvDispatch)
			{
				GameClientSendPacket::SendCharDuelCmd(lpRecvDispatch->GetSendStream(),
					dwSenderID, dwRecverID, cCmd, PktDuC::NO_SERVER_ERR);
			}

		} break;		

	case PktDuC::DUC_READY:			// ����
		{	
			if (NULL != lpSCharacter->GetDuelOpponent() || NULL != lpRCharacter->GetDuelOpponent())
			{
				GameClientSendPacket::SendCharDuelCmd(GameClientDispatch.GetSendStream(), dwSenderID, dwRecverID, cCmd, PktDuC::FAIL_DUELING_CHAR);
				break;	
			}

			lpSCharacter->SetDuelOpponent(lpRCharacter); 
			lpRCharacter->SetDuelOpponent(lpSCharacter);

			if (NULL != lpSendDispatch)
			{
				GameClientSendPacket::SendCharDuelCmd(lpSendDispatch->GetSendStream(),
					dwSenderID, dwRecverID, cCmd, PktDuC::NO_SERVER_ERR);
			}

			if (NULL != lpRecvDispatch)
			{
				GameClientSendPacket::SendCharDuelCmd(lpRecvDispatch->GetSendStream(),
					dwSenderID, dwRecverID, cCmd, PktDuC::NO_SERVER_ERR);
			}

		} break;		

	case PktDuC::DUC_REFUSE:		// ����		
		{
			if (NULL != lpRecvDispatch)
			{
				GameClientSendPacket::SendCharDuelCmd(lpRecvDispatch->GetSendStream(),
					dwSenderID, dwRecverID, cCmd, PktDuC::NO_SERVER_ERR);
			}

		} break;		

	case PktDuC::DUC_REQUEST_PARTY_INFO:	// ����� ��Ƽ ���� ��û
		{
			CCharacterParty* lpSParty = reinterpret_cast<CCharacterParty* >(lpSCharacter->GetParty());
			CCharacterParty* lpRParty = reinterpret_cast<CCharacterParty* >(lpRCharacter->GetParty());
			if (NULL == lpSParty || NULL == lpRParty)
			{
				GameClientSendPacket::SendCharDuelCmd(GameClientDispatch.GetSendStream(), dwSenderID, dwRecverID, cCmd, PktDuC::FAIL_NOT_PARTY);
				break;	
			}

			if (false == lpSParty->IsLeader(lpSCharacter->GetCID()))
			{
				GameClientSendPacket::SendCharDuelCmd(GameClientDispatch.GetSendStream(), dwSenderID, dwRecverID, cCmd, PktDuC::FAIL_NOT_LEADER);
				break;	
			}

			if (NULL != lpSParty->GetHostileParty() || NULL != lpRParty->GetHostileParty())
			{
				GameClientSendPacket::SendCharDuelCmd(GameClientDispatch.GetSendStream(), dwSenderID, dwRecverID, cCmd, PktDuC::FAIL_DUELING_CHAR);
				break;	
			}

			const int MAX_BUFFER = sizeof(PktTBInfo) + PARTY::MAX_MEM * (sizeof(unsigned long) + sizeof(unsigned char));
			char Buffer[MAX_BUFFER];
			unsigned short dwBufferSize = 0;

			if (true == lpRParty->MakeTeamBattleInfo(Buffer, dwBufferSize, lpRCharacter, cCmd))
			{
				if (NULL != lpSendDispatch)
				{
					lpSendDispatch->GetSendStream().PutBuffer(Buffer, dwBufferSize, CmdCharTeamBattleInfo);
				}
			}

		} break;

	case PktDuC::DUC_PARTY_PROPOSE:			// ��Ƽ ��� ��û
		{
			CCharacterParty* lpSParty = reinterpret_cast<CCharacterParty* >(lpSCharacter->GetParty());
			CCharacterParty* lpRParty = reinterpret_cast<CCharacterParty* >(lpRCharacter->GetParty());
			if (NULL == lpSParty || NULL == lpRParty)
			{
				GameClientSendPacket::SendCharDuelCmd(GameClientDispatch.GetSendStream(), dwSenderID, dwRecverID, cCmd, PktDuC::FAIL_NOT_PARTY);
				break;	
			}

			if (NULL != lpSParty->GetHostileParty() || NULL != lpRParty->GetHostileParty())
			{
				GameClientSendPacket::SendCharDuelCmd(GameClientDispatch.GetSendStream(), dwSenderID, dwRecverID, cCmd, PktDuC::FAIL_DUELING_CHAR);
				break;	
			}

			const int MAX_BUFFER = sizeof(PktTBInfo) + PARTY::MAX_MEM * (sizeof(unsigned long) + sizeof(unsigned char));
			char Buffer[MAX_BUFFER];
			unsigned short dwBufferSize = 0;

			if (true == lpSParty->MakeTeamBattleInfo(Buffer, dwBufferSize, lpSCharacter, cCmd))
			{
				CCharacter* pLeader = CCreatureManager::GetInstance().GetCharacter(lpRParty->GetLeader());
				if (NULL == pLeader)
				{
					GameClientSendPacket::SendCharDuelCmd(GameClientDispatch.GetSendStream(), dwSenderID, dwRecverID, cCmd, PktDuC::SERVER_ERROR);
					break;	
				}

				CGameClientDispatch* pDispatch = pLeader->GetDispatcher();
				if (NULL != pDispatch)
				{
					pDispatch->GetSendStream().PutBuffer(Buffer, dwBufferSize, CmdCharTeamBattleInfo);
				}
			}

		} break;		

	case PktDuC::DUC_PARTY_READY:			// ��Ƽ ��� ����
		{
			CCharacterParty* lpSParty = reinterpret_cast<CCharacterParty* >(lpSCharacter->GetParty());
			CCharacterParty* lpRParty = reinterpret_cast<CCharacterParty* >(lpRCharacter->GetParty());
			if (NULL == lpSParty || NULL == lpRParty)
			{
				GameClientSendPacket::SendCharDuelCmd(GameClientDispatch.GetSendStream(), dwSenderID, dwRecverID, cCmd, PktDuC::FAIL_NOT_PARTY);
				break;	
			}

			if (NULL != lpSParty->GetHostileParty() || NULL != lpRParty->GetHostileParty())
			{
				GameClientSendPacket::SendCharDuelCmd(GameClientDispatch.GetSendStream(), dwSenderID, dwRecverID, cCmd, PktDuC::FAIL_DUELING_CHAR);
				break;	
			}

			lpSParty->StartTeamBattle(lpRParty);
			lpRParty->StartTeamBattle(lpSParty);

		} break;	
	}

	return true;
}


// ���� ���
bool GameClientParsePacket::ParseCharPeaceMode(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
// DELETE_ME : ���̻� Ŭ���̾�Ʈ�� ���� ������ ������ �Ұ����ϴ�. ���丸 �������� ��. (2005-05-31 by �ε�)
/*
	CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktPeace), GameClientDispatch);

	PktPeace*	lpPktPeace	= static_cast<PktPeace *>(lpPktBase);
	CCharacter*	lpCharacter	= GameClientDispatch.GetCharacter();

	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	unsigned long dwCharID = lpPktPeace->m_dwCharID;
	bool bPeace = lpPktPeace->m_bPeace;

	unsigned short wError = PktPeace::NO_SERVER_ERR;
	char cLeftTime = -1;

	char cServerZone = CServerSetup::GetInstance().GetServerZone();
	if (SERVER_ID::ZONE3 == cServerZone)
	{
		wError = PktPeace::FAIL_BATTLE_GROUND;
		goto SEND_PACKET;
	}

	if (0 != lpCharacter->GetGID())
	{
		wError = PktPeace::FAIL_GUILD_MEMBER;
		goto SEND_PACKET;
	}

	cLeftTime = lpCharacter->SetPeaceMode(bPeace);
	if (-1 == cLeftTime)
	{
		wError = PktPeace::FAIL_WRONG_SYNC;
	}

SEND_PACKET:

	return GameClientSendPacket::SendCharPeaceMode(GameClientDispatch.GetSendStream(), dwCharID, 
		cLeftTime, bPeace, wError);
*/

	return true;
}


// ��ȯ�� ���
bool GameClientParsePacket::ParseCharSummonCmd(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktSummonCmd), GameClientDispatch);

	PktSummonCmd* lpPktSuC = static_cast<PktSummonCmd *>(lpPktBase);
	CCharacter*	lpCharacter = GameClientDispatch.GetCharacter();

	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	unsigned long dwCharID = lpPktSuC->m_dwCharID;
	unsigned char cCmd = lpPktSuC->m_cCmd;
	unsigned long dwTargetID = lpPktSuC->m_dwTargetID;

	CMonster* lpSummonee = lpCharacter->GetSummonee();
	if (NULL == lpSummonee)
	{
		ERRLOG3(g_Log, "CID:0x%08x ��ȯ���� ��ȯ���� ���� ĳ���Ͱ� ��ȯ�� ����� ���Ƚ��ϴ�. cCmd:%d, dwTargetID:0x%08x", 
			dwCharID, cCmd, dwTargetID);
		return true;
	}

	switch (cCmd)
	{
	case PktSummonCmd::SUC_ATTACK:
		{
			CAggresiveCreature* lpTarget = CCreatureManager::GetInstance().GetAggresiveCreature(dwTargetID);
			if (NULL == lpTarget)
			{
				ERRLOG2(g_Log, "CID:0x%08x �������� �ʴ� ID�� Ÿ������ ���� ����� ���Ƚ��ϴ�. TargetID:0x%08x", 
					dwCharID, dwTargetID);
				break;
			}

			lpSummonee->GetThreat().ClearThreatList();
			lpSummonee->AttackCmd(lpTarget, 1);

		} break;

	case PktSummonCmd::SUC_RETURN:
		{
			lpSummonee->CancelTarget();

		} break;

	case PktSummonCmd::SUC_DIE:
		{
			// �Ҹ��� ��ε�ĳ���õȴ�.
			return lpSummonee->Dead(NULL);

		} break;

	case PktSummonCmd::SUC_GUARD_ON:
		{
			lpSummonee->SetGuard(true);

		} break;

	case PktSummonCmd::SUC_GUARD_OFF:
		{
			lpSummonee->SetGuard(false);

		} break;
	}

	return GameClientSendPacket::SendCharSummonCmd(lpCharacter, lpSummonee, cCmd, dwTargetID);
}


bool GameClientParsePacket::ParseCharMoveEx(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{    
	const unsigned long CA_WAIT = 0;
	const unsigned long CA_RUN = 1;
	const unsigned long CA_ATTACK = 2;
	const unsigned long CA_MOVEATTACK = 3;
	const unsigned long CA_WALKBACK = 4;
	const unsigned long CA_WALKLEFT = 5;
	const unsigned long CA_WALKRIGHT = 6;
	const unsigned long CA_BASH = 7;
	const unsigned long CA_WALK = 8;
	const unsigned long CA_FALLDOWN = 9;
	const unsigned long CA_REST = 10;
	const unsigned long CA_SITDOWN = 11;
	const unsigned long CA_STANDUP = 12;
	const unsigned long CA_ATTACKADVANCE = 13;
	const unsigned long CA_ATTACKLEFT = 14;
	const unsigned long CA_ATTACKRIGHT = 15;
	const unsigned long CA_ATTACKRETREAT = 16;
	const unsigned long CA_ATTACK2 = 17;
	const unsigned long CA_ATTACK3 = 18;
	const unsigned long CA_ATTACK4 = 19;
	const unsigned long CA_RESPAWN = 20;
	const unsigned long CA_JUMP = 21;
	const unsigned long CA_LEFTDASH = 22;
	const unsigned long CA_RIGHTDASH = 23;
	const unsigned long CA_CASTING = 24;
	const unsigned long CA_FIRE1 = 25;
	const unsigned long CA_FIRE2 = 26;
	const unsigned long CA_COMBINATIONBLOW = 27;
	const unsigned long CA_BACKSTAB = 28;
	const unsigned long CA_AIMEDSHOT2 = 29;
	const unsigned long CA_ATTACK5 = 30;
	const unsigned long CA_ATTACK6 = 31;
	const unsigned long CA_BACKDASH = 32;
	const unsigned long CA_AIMEDSHOT1 = 33;
	const unsigned long CA_JUMPATTACK = 34;
	const unsigned long CA_JUMPATTACK2 = 35;
	const unsigned long CA_FRONTDASH = 36;
	const unsigned long CA_LAND = 37;
	const unsigned long CA_FORWARDJUMP = 38;
	const unsigned long CA_BACKJUMP = 40;
	const unsigned long CA_SHOT = 41;
	const unsigned long CA_JUMPSHOT = 42;
	const unsigned long CA_OVERBASH1 = 43;
	const unsigned long CA_OVERBASH2 = 44;
	const unsigned long CA_OVERBASH3 = 45;
	const unsigned long CA_OVERBASH4 = 46;
	const unsigned long CA_OVERBASH5 = 47;
	const unsigned long CA_POWERDRAIN = 48;
	const unsigned long CA_TURNPUNCH1 = 49;
	const unsigned long CA_TURNPUNCH2 = 50;
	const unsigned long CA_TURNPUNCH3 = 51;
	const unsigned long CA_JUMPATTACK3 = 52;
	const unsigned long CA_AIMEDSHOT3 = 53;
	const unsigned long CA_STUN = 54;
	const unsigned long CA_ROUNDSWING = 55;
	const unsigned long CA_SUMMON = 56;
	const unsigned long CA_HIT1 = 57;
	const unsigned long CA_WAITH1 = 58;
	const unsigned long CA_HIT2 = 59;
	const unsigned long CA_WAITH2 = 60;
	const unsigned long CA_SWIM = 100;
	const unsigned long CA_FORWARD	= 101;	// ���� ������ �������� �ִ�
	const unsigned long CA_BACKWARD	= 102;	// ���� �ڷ� �������� �ִ�

	const unsigned long CA_GATHERSTART		= 111;
	const unsigned long CA_GATHER01			= 111;
	const unsigned long CA_GATHER02			= 112;
	const unsigned long CA_GATHER03			= 113;
	const unsigned long CA_GATHER04			= 114;
	const unsigned long CA_GATHER05			= 115;
	const unsigned long CA_GATHER06			= 116;
	const unsigned long CA_GATHER07			= 117;
	const unsigned long CA_GATHER08			= 118;
	const unsigned long CA_GATHER09			= 119;
	const unsigned long CA_GATHER10			= 120;
	const unsigned long CA_GATHEREND		= 120;

	const unsigned long CA_SOCIALSTART			= 151;
	const unsigned long CA_SOCIAL_LAUGH			= 151;
	const unsigned long CA_SOCIAL_DESPAIR		= 152;
	const unsigned long CA_SOCIAL_SHY			= 153;
	const unsigned long CA_SOCIAL_SURPRISE		= 154;
	const unsigned long CA_SOCIAL_ROAR			= 155;
	const unsigned long CA_SOCIAL_WAVE			= 156;
	const unsigned long CA_SOCIAL_THANK			= 157;
	const unsigned long CA_SOCIAL_SORRY			= 158;
	const unsigned long CA_SOCIAL_SALUTE		= 159;
	const unsigned long CA_SOCIAL_CHEER			= 160;
	const unsigned long CA_SOCIAL_NOD			= 161;
	const unsigned long CA_SOCIAL_NO			= 162;
	const unsigned long CA_SOCIAL_CHARGE		= 163;
	const unsigned long CA_SOCIAL_COMFORT		= 164;
	const unsigned long CA_SOCIAL_TALK			= 165;
	const unsigned long CA_SOCIALEND			= 165;

	PktMVEx* lpPktMVEx = static_cast<PktMVEx*>(lpPktBase);

	unsigned long dwCID = lpPktMVEx->GetServerInfo();	
	unsigned char cUAct = lpPktMVEx->m_cUAct;
	unsigned char cLAct = lpPktMVEx->m_cLAct;

	CCharacter*	lpCharacter = GameClientDispatch.GetCharacter();

    // ���� CID��, �ڱ� CID�ų�, �ڽ��� Ÿ���ִ� ���������� CID�� ��쿡�� �ֺ����� ������.
	if (0 != lpCharacter && 
       (dwCID == lpCharacter->GetCID() || dwCID == lpCharacter->GetRideArmsCID()))
	{
		CNetworkPos& netPos = lpPktMVEx->m_NetworkPos;
		Position Pos(netPos.GetXPos(), netPos.GetYPos(), netPos.GetZPos());

		if(lpCharacter->IsLogined())
		{
			Position oldPos = lpCharacter->GetCurrentPos();
			unsigned long dis = Pos.GetDistance(oldPos);

			if(dis > 100 && 0 == lpCharacter->GetAdminLevel())
			{
				// ILLEGAL_WARPPOS
				// edtih 2009.07.21 �ҹ��̵� ���� �α� �����
				GAMELOG::LogIllegalWarpPos(*lpCharacter, oldPos, Pos);
			}

			// CA_REST == 10�̴�.
			lpCharacter->MoveTo(Pos, (cUAct == CA_REST) && (cLAct == CA_REST));
			lpCharacter->IncrementMoveUpdateCount();
			lpCharacter->SetDirection(netPos.GetDirection());

			// CharSphereTree �� ��� ��ǥ ����
			CCharSphereTree::GetInstance().MoveTo( lpCharacter->GetCID(), Pos );

			// ���� ���� ž�½�, ������ ��ġ�� �����Ѵ�.
			CSiegeObject* lpArms = NULL;
			if (lpCharacter->IsRideArms() && dwCID == lpCharacter->GetRideArmsCID())
			{
				lpArms = CSiegeObjectMgr::GetInstance().GetSiegeObject(dwCID);
				if (lpArms)
				{
					lpArms->MoveTo(Pos);
					lpArms->SetDirection(netPos.GetDirection());
				}
			}

			unsigned char cLastUpdateCount = lpCharacter->GetMoveUpdateCount();
			unsigned long dwCurrentTime = timeGetTime();
			unsigned long dwLastMoveUpdateTime = lpCharacter->GetLastMoveUpdateTime();        

			lpCharacter->SetLastMoveUpdateTime(dwCurrentTime);

			// ������ ������Ʈ�� �ڷ� 150ms�̻� �귶�� ��.

			bool bForceSend = true;

			switch(cUAct)
			{
			case CA_WAIT:
			case CA_WALK:
			case CA_RUN:
			case CA_REST:
				bForceSend = false;
				break;
			}

			if(50 < dwCurrentTime - dwLastMoveUpdateTime || bForceSend)
			{
				// �ֺ� �� �ο��� �ľ�.
				CCell* lpCell = lpCharacter->GetCellPos().m_lpCell;

				if(0 != lpCell)
				{
					size_t nCharacterNum = lpCell->GetNearCellCharacterNum();
					bool bSendData = false;

					if(nCharacterNum < 100)
					{
						// 100�̸� (�Ź� ����)
						bSendData = true;
					}
					else if(nCharacterNum < 200)
					{
						// 100~200���� (4���� 1�� �Ⱥ���)
						bSendData = (0 != (cLastUpdateCount % 4));
					}
					else
					{
						// 200�� �̻�  (4���� 2�� �Ⱥ���)
						bSendData = (0 != (cLastUpdateCount % 2));
					}

					if(bSendData || bForceSend)
					{
						PktMVEx pktMVEx;
                        memset(&pktMVEx, 0, sizeof(PktMVEx));
                        
						pktMVEx.m_NetworkPos = netPos;
						pktMVEx.m_cUAct = lpPktMVEx->m_cUAct;
						pktMVEx.m_cLAct = lpPktMVEx->m_cLAct;

                        char* szPacketData = reinterpret_cast<char*>(&pktMVEx);

                        if (PacketWrap::WrapCrypt(szPacketData, 
                            sizeof(PktMVEx), CmdCharMove, dwCID))
                        {
						    if (lpCharacter->IsRideArms() && 0 != lpArms)
						    {
    							lpArms->SendToRange(Siege::BROADCAST_RADIUS, 
                                    szPacketData, sizeof(PktMVEx), CmdCharMove);
						    }
						    else
						    {
							    lpCell->SendNowAllNearCellCharacter(
                                    szPacketData, sizeof(PktMVEx), CmdCharMove);
						    }
                        }
					}
				}
			}
		}
	}

	return true;
}   