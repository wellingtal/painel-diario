
#include "stdafx.h"
#include "ParseCharQuest.h"
#include "SendCharQuest.h"
#include "GameClientDispatch.h"

#include <Network/Dispatch/ParseUtils.h>
#include <Network/Packet/PacketStruct/CharQuestPacket.h>

#include <Creature/CreatureManager.h>
#include <Creature/Character/Character.h>
#include <Creature/NPC/NPC.h>

#include <Quest/QuestStruct.h>
#include <Quest/QuestMgr.h>

#include <Community/Party/Party.h>


// 퀘스트 시작
bool GameClientParsePacket::ParseCharStartQuest(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktStartQuest), GameClientDispatch);

	PktStartQuest* lpPktSQ = static_cast<PktStartQuest*>(lpPktBase);
	CCharacter* lpCharacter = GameClientDispatch.GetCharacter();

	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	unsigned long dwCharID = lpPktSQ->m_dwCharID;
	unsigned long dwNPCID = lpPktSQ->m_dwNPCID;
	unsigned short wQuestID = lpPktSQ->m_wQuestID;

	unsigned short wError = PktStartQuest::NO_SERVER_ERR;

	Quest::QuestNode* lpQuestNode = CQuestMgr::GetInstance().GetQuestNode(wQuestID);
	if (lpQuestNode)
	{
		switch (lpQuestNode->GetQuestType())
		{
			case Quest::NPC_QUEST:
			{
				CNPC* lpNPC = CCreatureManager::GetInstance().GetNPC(dwNPCID);
				if (NULL == lpNPC)
				{
					ERRLOG2(g_Log, "CID:0x%08x 존재하지 않는 NPC입니다. NID:0x%08x", lpCharacter->GetCID(), dwNPCID);
					wError = PktStartQuest::FAIL_NOT_NPC;
				}
				else
				{
					lpQuestNode = NULL;
					if (false == lpNPC->GetQuest(lpCharacter, wQuestID, &lpQuestNode))
					{
						wError = PktStartQuest::FAIL_NOT_HAVE_QUEST;
					}
					else
					{
//						if (lpQuestNode->IsPartyQuest() && NULL != lpCharacter->GetParty())
//						{
//							CCharacterParty* lpParty = reinterpret_cast<CCharacterParty*>(lpCharacter->GetParty());
//							if (lpParty)
//							{
//								return lpParty->StartQuest(lpQuestNode, lpNPC->GetCurrentPos(), dwNPCID);
//							}
//						}
//						else
						{
							if (true == lpCharacter->CheckQuest(lpQuestNode, wError))
							{
								if (false == lpCharacter->GiveQuest(lpQuestNode))
								{
									wError = PktStartQuest::FAIL_FULL_QUEST;
								}
							}
						}
					}
				}
			}
			break;

			case Quest::ITEM_QUEST:
			{
				// 아이템 사용 퀘스틑 UseItem 패킷을 사용한다.
				ERRLOG1(g_Log, "CID:0x%08x ITEM_QUEST 를 StartQuest 패킷으로 요청하였습니다.", lpCharacter->GetCID());
				wError = PktStartQuest::SERVER_ERROR;
				return true;
			}
			break;

			case Quest::AREA_QUEST:
			{
				if (lpQuestNode->m_dwZoneID == CServerSetup::GetInstance().GetServerZone())
				{
					const Position QuestPos(lpQuestNode->m_fPosX, lpQuestNode->m_fPosY, lpQuestNode->m_fPosZ);
					unsigned long dwDist = lpCharacter->GetCurrentPos().GetDistance(QuestPos);

					if (dwDist <= lpQuestNode->m_fDistance + Quest::ERROR_OF_DIST)
					{
//						if (lpQuestNode->IsPartyQuest() && NULL != lpCharacter->GetParty())
//						{
//							CCharacterParty* lpParty = reinterpret_cast<CCharacterParty*>(lpCharacter->GetParty());
//							if (lpParty)
//							{
//								return lpParty->StartQuest(lpQuestNode, QuestPos, 0);
//							}
//						}
//						else
						{
							if (true == lpCharacter->CheckQuest(lpQuestNode, wError))
							{
								if (false == lpCharacter->GiveQuest(lpQuestNode))
								{
									wError = PktStartQuest::FAIL_FULL_QUEST;
								}
							}
						}
					}
					else
					{
						wError = PktStartQuest::FAIL_TOO_FAR;
					}
				}
				else
				{
					wError = PktStartQuest::SERVER_ERROR;
				}
			}
			break;
		}
	}
	else
	{
		ERRLOG2(g_Log, "CID:0x%08x QuestID:0x%08x 존재하지 않는 Quest입니다.", lpCharacter->GetCID(), wQuestID);
		wError = PktStartQuest::NOT_EXIST_QUEST;
	}

	bool bResult = GameClientSendPacket::SendCharStartQuest(GameClientDispatch.GetSendStream(), 
															dwCharID, dwNPCID, wQuestID, wError);
	if (PktStartQuest::NO_SERVER_ERR == wError)
	{
		lpCharacter->StartPhase(wQuestID, 1);
	}

	return bResult;
}


// 트리거 발동
bool GameClientParsePacket::ParseCharOperateTrigger(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktOperateTrigger), GameClientDispatch);

	PktOperateTrigger* lpPktOT = static_cast<PktOperateTrigger*>(lpPktBase);
	CCharacter* lpCharacter = GameClientDispatch.GetCharacter();

	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	unsigned long dwCharID = lpPktOT->m_dwCharID;
	unsigned short wQuestID = lpPktOT->m_wQuestID;
	unsigned char cPhase = lpPktOT->m_cPhase;
	unsigned char cTriggerKind = lpPktOT->m_cTriggerKind;
	unsigned char cTrigger = lpPktOT->m_cTrigger;
	unsigned char cCount = lpPktOT->m_cCount;
	
	Quest::QuestNode* lpQuestNode = CQuestMgr::GetInstance().GetQuestNode(wQuestID);
	if (lpQuestNode)
	{
		// 주의! : TRIGGER_LEVEL_TALK와 TRIGGER_LEVEL_TALK 및 TRIGGER_TALK 는 파티 퀘스트이던 관계없이 혼자만 실행된다.
		if (Quest::TRIGGER_LEVEL_TALK == cTriggerKind || Quest::TRIGGER_TALK == cTriggerKind || 
			Quest::TRIGGER_FAME_TALK == cTriggerKind || Quest::TRIGGER_ABILITY_TALK == cTriggerKind	)
		{
			lpCharacter->OperateTrigger(wQuestID, cPhase, cTrigger, cCount-1, lpCharacter->GetCurrentPos());			
		}
		else
		{
			if (lpQuestNode->IsPartyQuest() && NULL != lpCharacter->GetParty())
			{
				CCharacterParty* lpParty = reinterpret_cast<CCharacterParty*>(lpCharacter->GetParty());
				if (lpParty)
				{
					lpParty->OperateTrigger(wQuestID, cPhase, cTrigger, cCount-1, lpCharacter->GetCurrentPos());
				}
			}
			else
			{
				unsigned short wError = lpCharacter->OperateTrigger(wQuestID, cPhase, cTrigger, cCount-1, lpCharacter->GetCurrentPos());
				if (PktOperateTrigger::NO_SERVER_ERR != wError)
				{
					ERRLOG3(g_Log, "CID:0x%08x 트리거 발동에 에러가 발생하였습니다. [3] 퀘스트:%d, 에러코드:%d", dwCharID, wQuestID, wError);
				}
			}
		}
	}

	return true;
}


// 퀘스트 취소
bool GameClientParsePacket::ParseCharCancelQuest(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktCancelQuest), GameClientDispatch);

	PktCancelQuest* lpPktCQ = static_cast<PktCancelQuest*>(lpPktBase);
	CCharacter* lpCharacter = GameClientDispatch.GetCharacter();

	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	unsigned long dwCharID = lpPktCQ->m_dwCharID;
	unsigned short wQuestID = lpPktCQ->m_wQuestID;

	unsigned short wError = PktBase::NO_SERVER_ERR;
	if (wQuestID & Quest::DISABLE_CANCEL_QUEST_BIT)
	{
		wError = PktCancelQuest::FAIL_DISABLE_CANCEL;
	}
	else
	{
		if (false == lpCharacter->CancelQuest(wQuestID))
		{
			wError = PktCancelQuest::FAIL_NOT_HAVE_QUEST;
		}
	}

	return GameClientSendPacket::SendCharCancelQuest(GameClientDispatch.GetSendStream(), 
		dwCharID, wQuestID, wError);
}

