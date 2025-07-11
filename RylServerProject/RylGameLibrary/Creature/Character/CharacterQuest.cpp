
#include "stdafx.h"
#include "Character.h"

#include <Network/Dispatch/DBAgent/DBAgentRequest.h>
#include <Network/Dispatch/DBAgent/DBAgentDispatch.h>
#include <Network/Dispatch/GameClient/GameClientDispatch.h>
#include <Network/Dispatch/GameClient/SendCharQuest.h>
#include <Network/Dispatch/GameClient/SendCharLevelUp.h>
#include <Network/Dispatch/GameClient/SendCharItem.h>
#include <Network/Dispatch/GameClient/SendCharEtc.h>
#include <Network/Packet/PacketStruct/CharQuestPacket.h>

#include <Item/Item.h>
#include <Item/ItemFactory.h>
#include <Item/ItemMgr.h>

#include <Log/LogStruct.h>
#include <Log/ItemLog.h>
#include <Log/CharacterLog.h>

#include <Creature/Character/ExpTable.h>
#include <Creature/Monster/AwardTable.h>

#include <Map/FieldMap/CellManager.h>
#include <Community/Party/Party.h>
#include <Quest/QuestMgr.h>

#include <Utility/Math/Math.h>

using namespace Quest;

bool CCharacter::CheckQuest(Quest::QuestNode* lpQuestNode, unsigned short& wError)
{
	if (NULL == lpQuestNode)
	{
		wError = PktStartQuest::FAIL_NOT_HAVE_QUEST;
		return false;
	}

	// 이미 한 퀘스트거나, 지금 수행중인 퀘스트 체크
	if (true == HasQuest(lpQuestNode->m_wQuestID))
	{
		wError = PktStartQuest::FAIL_HAS_QUEST;
		return false;
	}

	// 레벨 체크
	if(lpQuestNode->m_wMinLevel != 0 && lpQuestNode->m_wMaxLevel != 0)
	{
		if (lpQuestNode->m_wMinLevel > GetLevel())
		{
			wError = PktStartQuest::FAIL_MIN_LEVEL;
			return false;
		}

		if (lpQuestNode->m_wMaxLevel < GetLevel())
		{
			wError = PktStartQuest::FAIL_MAX_LEVEL;
			return false;
		}
	}

	// 명성 체크
	if(lpQuestNode->m_dwMinFame != 0 && lpQuestNode->m_dwMaxFame != 0) 
	{
		if (lpQuestNode->m_dwMinFame > GetFame())
		{
			wError = PktStartQuest::FAIL_MIN_FAME;
			return false;
		}

		if (lpQuestNode->m_dwMaxFame < GetFame())
		{
			wError = PktStartQuest::FAIL_MAX_FAME;
			return false;
		}
	}

	// 수행할 수 있는 국적인지 체크
	if (true == lpQuestNode->CheckNationDependent(GetNation()))
	{
		wError = PktStartQuest::FAIL_WRONG_NATION;
		return false;
	}

	// 클래스 제한 체크
	if (!(lpQuestNode->m_dwClass & (0x00000001 << (GetClass() - 1))))
	{
		wError = PktStartQuest::FAIL_WRONG_CLASS;
		return false;
	}

	// 선수행 퀘스트 체크 (0xFFFF 는 클라이언트에서 팝업메뉴에 나타나지 않게 하기위해서 사용)
	if (0 != lpQuestNode->m_wCompletedQuest && 0xFFFF != lpQuestNode->m_wCompletedQuest)
	{
		bool bCompleted = false;
		for (int nIndex = 0; nIndex < MAX_HISTORY_QUEST; ++nIndex)
		{
			if (m_wHistoryQuest[nIndex] == lpQuestNode->m_wCompletedQuest)
			{
				bCompleted = true;
				break;
			}
		}

		if (!bCompleted)
		{
			wError = PktStartQuest::FAIL_NOT_COMPLETE_PRIOR_QUEST;
			return false;
		}
	}

	return true;
}

bool CCharacter::HasQuest(unsigned short wQuestID)
{
	int nIndex = 0;

	for (nIndex = 0; nIndex < MAX_EXECUTING_QUEST; ++nIndex)
	{
		if (NULL != m_ExecutingQuest[nIndex].m_QuestNode)
		{
			if (m_ExecutingQuest[nIndex].m_QuestNode->m_wQuestID == wQuestID)
			{
				return true;
			}
		}
	}

	for (nIndex = 0; nIndex < MAX_HISTORY_QUEST; ++nIndex)
	{
		if (m_wHistoryQuest[nIndex] == wQuestID)
		{
			return true;
		}
	}

	return false;
}


bool CCharacter::GiveQuest(QuestNode* lpQuestNode)
{
	unsigned char cIndex = 0;
	while (cIndex < MAX_EXECUTING_QUEST)
	{
		if (NULL == m_ExecutingQuest[cIndex].m_QuestNode)
		{
			break;
		}

		++cIndex;
	}

	if (cIndex >= MAX_EXECUTING_QUEST)
	{
		// 퀘스트를 더 얻을 수 없음
		return false;
	}

	m_ExecutingQuest[cIndex].m_QuestNode = lpQuestNode;

	return true;
}


bool CCharacter::StartPhase(unsigned short wQuestID, unsigned char cPhase)
{
	for (int nQuestIndex = 0; nQuestIndex < MAX_EXECUTING_QUEST; ++nQuestIndex)
	{
		QuestNode* lpQuestNode = m_ExecutingQuest[nQuestIndex].m_QuestNode;
		if (NULL != lpQuestNode)
		{
			if (wQuestID == lpQuestNode->m_wQuestID)
			{
				if (cPhase <= lpQuestNode->m_lstPhase.size())
				{
					m_ExecutingQuest[nQuestIndex].m_cPhase = cPhase;
					PhaseNode* phaseNode = lpQuestNode->m_lstPhase[m_ExecutingQuest[nQuestIndex].m_cPhase - 1];

					for (size_t nTriggerIndex = 0; nTriggerIndex < phaseNode->m_lstTrigger.size(); ++nTriggerIndex)
					{
						m_ExecutingQuest[nQuestIndex].m_cTriggerCount[nTriggerIndex] = 
							static_cast<unsigned char>(phaseNode->m_lstTrigger[nTriggerIndex]->m_dwMaxCount);

						if (TRIGGER_START == phaseNode->m_lstTrigger[nTriggerIndex]->m_dwTriggerKind)
						{
							m_ExecutingQuest[nQuestIndex].m_cTriggerCount[nTriggerIndex]--;
							OperateTrigger(lpQuestNode->m_wQuestID, m_ExecutingQuest[nQuestIndex].m_cPhase, 
								static_cast<unsigned char>(nTriggerIndex), m_ExecutingQuest[nQuestIndex].m_cTriggerCount[nTriggerIndex], 
								m_CurrentPos);				
						}

						if (TRIGGER_PICK == phaseNode->m_lstTrigger[nTriggerIndex]->m_dwTriggerKind)
						{
							unsigned short wProtoTypeID = 
								static_cast<unsigned short>(phaseNode->m_lstTrigger[nTriggerIndex]->m_dwTriggerID);
							unsigned short wCount = m_Inventory.GetItemNum(wProtoTypeID);
							if (0 != wCount)
							{
								CheckTrigger(TRIGGER_PICK, wProtoTypeID, Position(), wCount);
							}
						}

						if (TRIGGER_FAME == phaseNode->m_lstTrigger[nTriggerIndex]->m_dwTriggerKind)
						{
							CheckTrigger(TRIGGER_FAME, GetFame(), Position(), 1);
						}
					}

					GET_SINGLE_DISPATCH(lpDBAgentDispatch, 
						CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());

					if (0 != lpDBAgentDispatch)
					{
						GameClientSendPacket::SendCharQuestInfo(lpDBAgentDispatch->GetSendStream(), this);
					}

					return true;
				}

				return false;
			}
		}
	}

	return false;
}


bool CCharacter::CancelQuest(unsigned short wQuestID, bool bFinished)
{
	for (unsigned char cQuestIndex = 0; cQuestIndex < MAX_EXECUTING_QUEST; ++cQuestIndex)
	{
		if (NULL != m_ExecutingQuest[cQuestIndex].m_QuestNode)
		{
			if (m_ExecutingQuest[cQuestIndex].m_QuestNode->m_wQuestID == wQuestID)
			{
				// 종속 아이템 삭제
				if (true == m_ExecutingQuest[cQuestIndex].m_QuestNode->m_bDelLinkItem && !bFinished)
				{
					for (unsigned char cPhaseIndex = 0; cPhaseIndex < m_ExecutingQuest[cQuestIndex].m_cPhase; ++cPhaseIndex)
					{
						PhaseNode* lpNode = m_ExecutingQuest[cQuestIndex].m_QuestNode->m_lstPhase[cPhaseIndex];

						if(lpNode)
						{
							for (unsigned char cItemIndex = 0; cItemIndex < lpNode->m_lstDeletedItem.size(); ++cItemIndex)
							{
								const unsigned short wItemID = lpNode->m_lstDeletedItem[cItemIndex].m_wItemID;
								const unsigned short wItemNum = 
									std::min(static_cast<unsigned short>(lpNode->m_lstDeletedItem[cItemIndex].m_cItemNum), m_Inventory.GetItemNum(wItemID));

								// Rodin : << 고쳐야할 사항 >>
								//
								// * 트리거와 이벤트의 분리
								//	- 트리거 큐를 두고 트리거가 발생하면 푸쉬
								//	- 메인 루프에서 트리거 큐에 트리거가 있으면 이벤트를 실행하고 큐에서 팝 
								//	- 이 때 꼭 큐를 복사해서 이 프로세스 중에 발생한 트리거는 새로운 큐에 들어가도록 한다. (다음 펄스에 실행)
								// * TRIGGER_PICK의 제거
								// * Item::CArrayContainer::DisappearItem 리팩토링
								m_Inventory.DisappearItem(wItemID, wItemNum);
							}
						}						
					}
				}

				// 퀘스트 삭제
				std::copy(m_ExecutingQuest + cQuestIndex + 1, m_ExecutingQuest + MAX_EXECUTING_QUEST, 
					m_ExecutingQuest + cQuestIndex);
				m_ExecutingQuest[MAX_EXECUTING_QUEST - 1] = ExecutingQuest();
				return true;
			}
		}
	}

	return false;
}


void CCharacter::CheckTrigger(unsigned char cTriggerKind, unsigned long dwReferenceID, Position Pos, short wCount)
{
	for (int nQuestIndex = 0; nQuestIndex < MAX_EXECUTING_QUEST; ++nQuestIndex)
	{
		QuestNode* lpQuestNode = m_ExecutingQuest[nQuestIndex].m_QuestNode;
		if (NULL == lpQuestNode) 
		{ 
			break; 
		}

		if(lpQuestNode->m_wMaxPhase<m_ExecutingQuest[nQuestIndex].m_cPhase)
		{
			m_ExecutingQuest[nQuestIndex].m_cPhase = (char)lpQuestNode->m_wMaxPhase;
		}

		PhaseNode* phaseNode = lpQuestNode->m_lstPhase[m_ExecutingQuest[nQuestIndex].m_cPhase - 1];

		if(phaseNode)
		{
			for (size_t nTriggerIndex = 0; nTriggerIndex < phaseNode->m_lstTrigger.size(); ++nTriggerIndex)
			{
				if (cTriggerKind == phaseNode->m_lstTrigger[nTriggerIndex]->m_dwTriggerKind)
				{
					bool bOperateTrigger = false;

					switch (cTriggerKind)
					{
					case TRIGGER_START:
						break;

					case TRIGGER_PUTON:
						if (dwReferenceID == phaseNode->m_lstTrigger[nTriggerIndex]->m_dwTriggerID) 
						{ 
							Position TriggerPos(phaseNode->m_lstTrigger[nTriggerIndex]->m_fPosX, 
								phaseNode->m_lstTrigger[nTriggerIndex]->m_fPosY,
								phaseNode->m_lstTrigger[nTriggerIndex]->m_fPosZ);
							if (Pos.GetDistance(TriggerPos) <= phaseNode->m_lstTrigger[nTriggerIndex]->m_fDistance)
							{
								bOperateTrigger = true; 
							}
						}
						break;

					case TRIGGER_GETON:
					case TRIGGER_TALK:
						break;

					case TRIGGER_KILL:
						// edith 2008.01.24 퀘스트 중에서 몬스터 잡을때 1001~1500번 몬스터 아이디를 처리하는 부분
						if(1000 < dwReferenceID && dwReferenceID <= 1500)
						{
							// 1001~1500번은 1~500번과 동일한 아이디이다.
							if((dwReferenceID-1000) == phaseNode->m_lstTrigger[nTriggerIndex]->m_dwTriggerID) 
							{
								bOperateTrigger = true; 
							}
						}
						else if(dwReferenceID == phaseNode->m_lstTrigger[nTriggerIndex]->m_dwTriggerID) 
						{ 
							bOperateTrigger = true; 
						}
						break;

					case TRIGGER_PICK:	
						if(dwReferenceID == phaseNode->m_lstTrigger[nTriggerIndex]->m_dwTriggerID) 
						{ 
							bOperateTrigger = true; 
						}
						break;

					case TRIGGER_FAME:
						if (dwReferenceID >= phaseNode->m_lstTrigger[nTriggerIndex]->m_dwTriggerID)
						{
							bOperateTrigger = true;
						}
						break;

					case TRIGGER_LEVEL_TALK:
						if (dwReferenceID >= phaseNode->m_lstTrigger[nTriggerIndex]->m_dwLevel)
						{
							bOperateTrigger = true;
						}
						break;

					case TRIGGER_FAME_TALK:
						if (dwReferenceID >= phaseNode->m_lstTrigger[nTriggerIndex]->m_dwLevel)
						{
							bOperateTrigger = true;
						}
						break;

					case TRIGGER_ABILITY_TALK:
/*
						ID / phaseNode->m_lstTrigger[nTriggerIndex]->m_dwZoneID
						LV / phaseNode->m_lstTrigger[nTriggerIndex]->m_dwLevel

						if (dwReferenceID >= )
						{
							bOperateTrigger = true;
						}
*/
						break;
					}

					if (true == bOperateTrigger)
					{
						m_ExecutingQuest[nQuestIndex].m_cTriggerCount[nTriggerIndex] -= wCount;

						OperateTrigger(lpQuestNode->m_wQuestID, m_ExecutingQuest[nQuestIndex].m_cPhase, 
							static_cast<unsigned char>(nTriggerIndex), m_ExecutingQuest[nQuestIndex].m_cTriggerCount[nTriggerIndex], Pos);

						if (NULL != GetParty())
						{
							CCharacterParty* lpParty = reinterpret_cast<CCharacterParty*>(GetParty());
							if (NULL != lpParty)
							{
								lpParty->CheckTrigger(GetCID(), cTriggerKind, dwReferenceID, Pos, wCount);
							}
						}

						// 한 번에 한 번만 트리거 발동
						return;
					}
				}
			}
		}
	}

	if (NULL != GetParty())
	{
		CCharacterParty* lpParty = reinterpret_cast<CCharacterParty*>(GetParty());
		if (NULL != lpParty)
		{
			lpParty->CheckTrigger(GetCID(), cTriggerKind, dwReferenceID, Pos, wCount);
		}
	}
}

unsigned short CCharacter::OperateTrigger(unsigned short wQuestID, unsigned char cPhase, 
										  unsigned char cTrigger, unsigned char cCount, Position Pos, unsigned char cType)
{
	// 큐에 삽입.	
	CQuestMgr::GetInstance().AddPendingQuest(m_dwCID, wQuestID, cPhase, cTrigger, cCount, Pos);
	
	return PktOperateTrigger::NO_SERVER_ERR;	
}

void CCharacter::PendingQuest(TriggerMsg* pTriggerMsg)
{
	unsigned short	wQuestID	= pTriggerMsg->m_wQuestID;
	unsigned char	cPhase		= pTriggerMsg->m_cPhase; 
	unsigned char	cTrigger	= pTriggerMsg->m_cTrigger;
	unsigned char	cCount		= pTriggerMsg->m_cCount;	
	unsigned short	wError		= PktOperateTrigger::FAIL_WRONG_QUEST;
	ErrorCode		eError		= S_SUCCESS;
	bool			bSave		= false;
	int				nQuestIndex = 0;

	Position		Pos(pTriggerMsg->m_Pos);

	bool			bShowLog	= true;	

	for (; nQuestIndex < MAX_EXECUTING_QUEST; ++nQuestIndex)
	{
		if (NULL != m_ExecutingQuest[nQuestIndex].m_QuestNode)
		{
			if (wQuestID == m_ExecutingQuest[nQuestIndex].m_QuestNode->m_wQuestID)
			{
				if (cPhase != m_ExecutingQuest[nQuestIndex].m_cPhase)
				{
					wError = PktOperateTrigger::FAIL_WRONG_PHASE;
					break;
				}

				PhaseNode* phaseNode = m_ExecutingQuest[nQuestIndex].m_QuestNode->m_lstPhase[cPhase - 1];
				if (cTrigger >= phaseNode->m_lstTrigger.size())
				{
					wError = PktOperateTrigger::FAIL_WRONG_TRIGGER;
					break;
				}

				// 퀘스트가 끝나면 바로 삭제하므로 그 전에 히스토리에 저장할 것인가의 여부를 남겨둔다.
				bSave = m_ExecutingQuest[nQuestIndex].m_QuestNode->m_bSave;

				// 트리거 카운트를 새로 설정하고 그 값이 0이 되면 이벤트 발생.
				m_ExecutingQuest[nQuestIndex].m_cTriggerCount[cTrigger] = cCount;
				if (0 == m_ExecutingQuest[nQuestIndex].m_cTriggerCount[cTrigger])
				{
					eError = ExecuteEvent(m_ExecutingQuest[nQuestIndex], phaseNode->m_lstTrigger[cTrigger], Pos);

					// 디서피어면 로그를 남기지 않는다.
					if (eError == E_EVENT_DISAPPEAR)
						bShowLog = false;

					if (eError == E_EVENT_DISAPPEAR || 
						eError == E_EVENT_MONSTERDROP || 
						eError == E_EVENT_AWARDITEM )
					{
						wError = PktOperateTrigger::FAIL_EVENT_ERROR;
						++m_ExecutingQuest[nQuestIndex].m_cTriggerCount[cTrigger];
						break;
					}
					else if (eError == E_EVENT_GET)
					{
						wError = PktOperateTrigger::FAIL_EVENT_GET_ERROR;
						++m_ExecutingQuest[nQuestIndex].m_cTriggerCount[cTrigger];
						break;
					}
				}

				wError = PktOperateTrigger::NO_SERVER_ERR;
				break;
			}
		}
	}

	if (NULL != m_lpGameClientDispatch)
	{
		GameClientSendPacket::SendCharOperateTrigger(m_lpGameClientDispatch->GetSendStream(), 
			m_dwCID, wQuestID, cPhase, cTrigger, cCount, wError);

		if (S_END_QUEST == eError)
		{
			GameClientSendPacket::SendCharEndQuest(m_lpGameClientDispatch->GetSendStream(), 
				m_dwCID, wQuestID, bSave);
			
			if(bShowLog && PktOperateTrigger::NO_SERVER_ERR != wError)
			{
				ERRLOG4(g_Log, "CID:0x%08x 트리거 발동에 에러가 발생하였습니다. [0] 퀘스트:%d(%d),  에러코드:%d", m_dwCID, wQuestID, nQuestIndex, wError);
			}
			return;
		}
	}

	if (cPhase != m_ExecutingQuest[nQuestIndex].m_cPhase)
	{
		StartPhase(wQuestID, m_ExecutingQuest[nQuestIndex].m_cPhase);
	}

	if(bShowLog && PktOperateTrigger::NO_SERVER_ERR != wError)
	{
		ERRLOG4(g_Log, "CID:0x%08x 트리거 발동에 에러가 발생하였습니다. [1] 퀘스트:%d(%d), 에러코드:%d", m_dwCID, wQuestID, nQuestIndex, wError);
	}
}


Quest::ErrorCode CCharacter::ExecuteEvent(ExecutingQuest executingQuest, const TriggerNode* triggerNode, Position Pos)
{
	// Desc : 아이템을 없애는 이벤트의 경우 따로 가비지 리스트로 보관하다가 
	//		다른 종류의 이벤트 발생 혹은 이벤트 종료시 처리한다.
	vector<Item::ItemGarbage> vecItemGarbage;

	// 돈을 관리하는 자리
	vecItemGarbage.push_back(Item::ItemGarbage(NULL, 0));

	GAMELOG::ERRType eReturnCode = 0;

	for (size_t nIndex = 0; nIndex < triggerNode->m_lstEvent.size(); ++nIndex)
	{
		EventNode* eventNode = triggerNode->m_lstEvent[nIndex];

		if (EVENT_DISAPPEAR != eventNode->m_dwEventKind && false == vecItemGarbage.empty())
		{
			ClearGarbage(vecItemGarbage, PktDisappearItem::DIC_DEFAULT);
		}

		switch (eventNode->m_dwEventKind) 
		{
			case EVENT_DISAPPEAR:
			{
				if (0 != eventNode->m_dwEventAmount2)
				{
					vecItemGarbage[0].m_dwRemainNum += eventNode->m_dwEventAmount2;
				}

				if(eventNode->m_dwEventNumber)
				{
					if (false == m_Inventory.DisappearItem(static_cast<unsigned short>(eventNode->m_dwEventNumber), 
						static_cast<unsigned short>(eventNode->m_dwEventAmount1), vecItemGarbage))
					{
						return E_EVENT_DISAPPEAR;
					}
				}
				break;
			} 

			case EVENT_GET:
			{
				Item::CItem* lpItem = 
					Item::CItemFactory::GetInstance().CreateItem(static_cast<unsigned short>(eventNode->m_dwEventNumber));
				if (NULL == lpItem)
				{
					ERRLOG2(g_Log, "CID:%10u 아이템 스크립트에 존재하지 않는 아이템을 퀘스트 스크립트에서 요청하였습니다. ItemID:%d", 
						m_dwCID, eventNode->m_dwEventNumber);
					return E_EVENT_GET;
				}

				if (true == lpItem->IsSet(Item::DetailData::STACKABLE))
				{
					lpItem->SetNumOrDurability(static_cast<unsigned char>(eventNode->m_dwEventAmount1));
				}

				Item::CEquipment* lpEquip = Item::CEquipment::DowncastToEquipment(lpItem);
				if (NULL != lpEquip)
				{
					lpEquip->SetNewEquip();
				}

				if (false == TestItem(lpItem))
				{
					ERRLOG1(g_Log, "CID:%10u 퀘스트 아이템을 넣을 공간이 없어서 이벤트 실행 실패", m_dwCID);
					DELETE_ITEM(lpItem);
					return E_EVENT_GET;
				}

				if (false == GiveItem(lpItem))
				{
					ERRLOG1(g_Log, "CID:%10u 퀘스트 아이템을 받는데 실패하였습니다.", m_dwCID);
					DELETE_ITEM(lpItem);                    
				}

				GAMELOG::LogQuestGetReward(*this, executingQuest.m_QuestNode->m_wQuestID, lpItem, 
						0, 0, 0, 0, eReturnCode);


				// GievItem 으로 스택된 경우
				if (lpItem->IsSet(Item::DetailData::STACKABLE) && 0 == lpItem->GetNumOrDurability())
				{
					DELETE_ITEM(lpItem);
				}

				break;
			} 

			case EVENT_SPAWN:
			{
				Position DestPos(eventNode->m_fPosX, eventNode->m_fPosY, eventNode->m_fPosZ);

				// 배틀 그라운드 서버군의 가상존에서는 몬스터를 만들어 낼수 없다!!
				if (0 != GetMapIndex())
				{
/*					VirtualArea::CVirtualArea* lpVirtualArea = VirtualArea::CVirtualAreaMgr::GetInstance().GetVirtualArea(lpAdmin->GetMapIndex());
					if (lpVirtualArea)
					{
						CVirtualMonsterMgr* lpVirtualMonsterMgr = lpVirtualArea->GetMonsterManager();
						if (NULL == lpVirtualMonsterMgr || 
							false == lpVirtualMonsterMgr->AdminSummonMonster(lpPktAdmin->m_usProtoTypeID, DestPos, lpAdmin->GetMapIndex()))
						{
							ERRLOG1(g_Log, "CID:%10u 어드민 명령 오류 : 몬스터를 생성하는데 실패하였습니다.", lpAdmin->GetCID());
							break;
						}
					}
*/
				}
				else
				{
					if (0 == eventNode->m_fPosX && 0 == eventNode->m_fPosY && 0 == eventNode->m_fPosZ)
					{
						// 캐릭터 주변에 랜덤한 위치에...
						unsigned long dwTick = timeGetTime();
						DestPos.m_fPointX = GetCurrentPos().m_fPointX + static_cast<float>(Math::Random::SimpleRandom(dwTick, 10) - 20);
						DestPos.m_fPointX = GetCurrentPos().m_fPointY;
						DestPos.m_fPointX = GetCurrentPos().m_fPointZ + static_cast<float>(Math::Random::SimpleRandom(dwTick, 10) - 20);
					}
					
					if (false == CCellManager::GetInstance().AdminSummonMonster(eventNode->m_dwEventNumber, DestPos))
					{
						ERRLOG1(g_Log, "CID:%10u 퀘스트로 몬스터를 생성하는데 실패하였습니다.", GetCID());
						break;
					}
				}

				DETLOG5(g_Log, "CID:%10u 퀘스트로 몬스터를 소환하였습니다. 몬스터(ID:%d, (%.1f, %.1f, %.1f))", 
						GetCID(), eventNode->m_dwEventNumber, DestPos.m_fPointX, DestPos.m_fPointY, DestPos.m_fPointZ);
				break;
			} 

			case EVENT_MONSTERDROP:
			{
				CCell* lpCell = CCellManager::GetInstance().GetCell(0, 
					static_cast<unsigned long>(Pos.m_fPointX), 
					static_cast<unsigned long>(Pos.m_fPointY), 
					static_cast<unsigned long>(Pos.m_fPointZ));

				if (NULL == lpCell)
				{
					ERRLOG4(g_Log, "CID:%10u 퀘스트 아이템을 드랍할 몬스터의 위치가 이상합니다. X:%.1f, Y:%.1f, Z:%.1f", 
						m_dwCID, Pos.m_fPointX, Pos.m_fPointY, Pos.m_fPointZ);
					return E_EVENT_MONSTERDROP;
				}

				const unsigned short wItemID = static_cast<unsigned short>(eventNode->m_dwEventAmount1);
				const unsigned char cNum = static_cast<unsigned char>(eventNode->m_dwEventNumber);

				Item::CItem* lpItem = Item::CItemFactory::GetInstance().CreateItem(wItemID);
				if (NULL == lpItem)
				{
					ERRLOG2(g_Log, "CID:%10u 퀘스트 아이템 생성에 실패하였습니다. ItemID:%d", 
						m_dwCID, eventNode->m_dwEventAmount1);
					return E_EVENT_MONSTERDROP;
				}

				if (true == lpItem->IsSet(Item::DetailData::STACKABLE))
				{
					lpItem->SetNumOrDurability(cNum);
				}

				CCell::ItemInfo itemInfo;
				lpCell->SetItem(Pos, lpItem, 0, m_dwCID, CCell::NONE, itemInfo);
				break;
			} 

			case EVENT_END:
			{
				EventEnd(executingQuest.m_QuestNode->m_wQuestID, executingQuest.m_QuestNode->m_bSave);
				return S_END_QUEST;
			}

			case EVENT_AWARD:		
			{
				bool hasQuest=HasExecutingQuest(executingQuest.m_QuestNode->m_wQuestID);
					if(hasQuest)
					{
						EventAward(eventNode->m_dwEventNumber, eventNode->m_dwEventAmount1, eventNode->m_dwEventAmount2, eventNode->m_dwEventAmount3);
						GAMELOG::LogQuestGetReward(*this, executingQuest.m_QuestNode->m_wQuestID, 0, 
							eventNode->m_dwEventNumber, eventNode->m_dwEventAmount1, eventNode->m_dwEventAmount2, eventNode->m_dwEventAmount3, eReturnCode);
						break;
					}
					break;
			}

			case EVENT_MSGBOX:		
			{
				break;
			}

			case EVENT_ALARMBOX:
			{
				break;
			}

			case EVENT_PHASE:	
			{
				for (int nIndex = 0; nIndex < MAX_EXECUTING_QUEST; ++nIndex)
				{
					if (NULL != m_ExecutingQuest[nIndex].m_QuestNode)
					{
						if (executingQuest.m_QuestNode->m_wQuestID == m_ExecutingQuest[nIndex].m_QuestNode->m_wQuestID)
						{
							m_ExecutingQuest[nIndex].m_cPhase = static_cast<unsigned char>(eventNode->m_dwEventNumber);
							break;
						}
					}
				}
				break;
			}

			case EVENT_ADDQUEST:
			{
				unsigned short	wError		= 0;
				unsigned long	dwNPCID		= eventNode->m_dwEventNumber;
				unsigned short	wQuestID	= static_cast<unsigned short>(eventNode->m_dwEventAmount1);

				Quest::QuestNode* lpQuest = CQuestMgr::GetInstance().GetQuestNode(wQuestID);
				if (NULL == lpQuest)
				{
					ERRLOG2(g_Log, "CID:%10u 이벤트 퀘스트 추가 실패 : QuestID:0x%04x 퀘스트가 존재하지 않습니다.", 
						m_dwCID, wQuestID);
					break;
				}

				if (false == CheckQuest(lpQuest, wError))
				{
					ERRLOG3(g_Log, "CID:%10u 이벤트 퀘스트 추가 실패 : QuestID:0x%04x, wError : %d", m_dwCID, wQuestID, wError);
					break;
				}

				if (false == GiveQuest(lpQuest))
				{
					ERRLOG2(g_Log, "CID:%10u 이벤트 퀘스트 추가 실패 : QuestID:0x%4x 퀘스트를 주는데 실패했습니다.", 
						m_dwCID, wQuestID);
					break;
				}

				if (NULL != m_lpGameClientDispatch)
				{
					GameClientSendPacket::SendCharStartQuest(m_lpGameClientDispatch->GetSendStream(), 
						m_dwCID, dwNPCID, wQuestID, PktBase::NO_SERVER_ERR);
				}

				if (false == StartPhase(wQuestID, 1))
				{
					ERRLOG2(g_Log, "CID:%10u 이벤트 퀘스트 추가 실패 : QuestID:0x%x 퀘스트의 첫번째 단계를 실행하는데 실패했습니다.",
						m_dwCID, wQuestID);

					// 해당 퀘스트 제거
					CancelQuest(wQuestID);
					if (NULL != m_lpGameClientDispatch)
					{
						GameClientSendPacket::SendCharCancelQuest(m_lpGameClientDispatch->GetSendStream(),
							m_dwCID, wQuestID, PktBase::NO_SERVER_ERR);
					}
				}
				break;
			}

			case EVENT_AWARDABILITY:
			{
				unsigned short	wError		= 0;
				unsigned short	wAbilityID  = static_cast<unsigned short>(eventNode->m_dwEventNumber);
				unsigned short	wAbilityLV	= static_cast<unsigned short>(eventNode->m_dwEventAmount1);

				// 이미 배워둔 어빌리티인지 체크
				if ( wAbilityLV <= GetSkillLockCount(wAbilityID) )
				{
					ERRLOG3(g_Log, "CID:0x%08x 퀘스트 보상 실패 : AbilityID:0x%08x wAbilityLV:%d 어빌리티를 배우는데 실패하였습니다.", 
							m_dwCID, wAbilityID, wAbilityLV);
					return E_EVENT_AWARDITEM;
				}

				using namespace Item;

				ItemInfo tempItemInfo(wAbilityID);
				tempItemInfo.m_UseItemInfo.m_usSkill_ID         = wAbilityID;
				tempItemInfo.m_UseItemInfo.m_usSkill_LockCount  = wAbilityLV;
				tempItemInfo.m_DetailData.m_dwFlags |= DetailData::USE_ITEM;

				// ItemSerial 이 0 인 임시 아이템을 생성한다.
				CUseItem* lpUseItem = CUseItem::DowncastToUseItem(
					CItemFactory::GetInstance().CreateTempItem(tempItemInfo));

				if (!AbilityCreate(lpUseItem))
				{
					ERRLOG3(g_Log, "CID:0x%08x 퀘스트 보상 실패 : AbilityID:0x%08x wAbilityLV:%d 어빌리티를 배우는데 실패하였습니다.", 
							m_dwCID, wAbilityID, wAbilityLV);

					return E_EVENT_AWARDITEM;
				}

				DELETE_ITEM(lpUseItem);
				break;
			}

			case EVENT_AWARDITEM:
			{
				Item::EquipType::OptionType	eOptionType	= static_cast<Item::EquipType::OptionType>(eventNode->m_dwEventNumber);
				unsigned char				cLevel		= static_cast<unsigned char>(eventNode->m_dwEventAmount1);
				Item::EquipType::Grade		eItemGrade	= static_cast<Item::EquipType::Grade>(eventNode->m_dwEventAmount2);
				unsigned char				cBaseNum	= static_cast<unsigned char>(eventNode->m_dwEventAmount3);

				if (0 == cLevel)
				{
					// 캐릭터의 레벨을 참조하는 경우
					cLevel = m_DBData.m_Info.Level;
				}
				
				unsigned short wItemID = AwardTable::CAward::GetInstance().GetAwardEquipment(eOptionType, cLevel, this, true);
				if (0 == wItemID)
				{
					ERRLOG3(g_Log, "CID:%10u 퀘스트 보상으로 줄 아이템이 없습니다. ItemType:%d, ItemLevel:%d", 
						m_dwCID, eventNode->m_dwEventNumber, eventNode->m_dwEventAmount1);
					return E_EVENT_AWARDITEM;
				}

				Item::CItem* lpItem = Item::CItemFactory::GetInstance().CreateItem(wItemID);
				if (NULL == lpItem)
				{
					ERRLOG2(g_Log, "CID:%10u 아이템 생성에 실패하였습니다. ItemID:%d", 
						m_dwCID, wItemID);
					return E_EVENT_AWARDITEM;
				}

				Item::CEquipment* lpEquip = Item::CEquipment::DowncastToEquipment(lpItem);
				if (NULL != lpEquip)
				{
					if(lpEquip->AddRandomOption(eItemGrade, cBaseNum, GetMagicChancePoint()))
						lpEquip->SetNewEquip(2);
					else
						lpEquip->SetNewEquip();
				}
                
				if (false == GiveItem(lpItem))
				{
					ERRLOG1(g_Log, "CID:%10u 퀘스트 보상으로 장비를 받는데 실패하였습니다.", m_dwCID);
					DELETE_ITEM(lpItem);                    
				}

				GAMELOG::LogQuestGetReward(*this, executingQuest.m_QuestNode->m_wQuestID, lpItem, 
						0, 0, 0, 0, eReturnCode);


				// GievItem 으로 스택된 경우
				if (lpItem->IsSet(Item::DetailData::STACKABLE) && 0 == lpItem->GetNumOrDurability())
				{
					DELETE_ITEM(lpItem);
				}

				break;
			}

			case EVENT_AWARDITEMID:
			{
				Item::EquipType::OptionType	eOptionType	= static_cast<Item::EquipType::OptionType>(eventNode->m_dwEventNumber);
				unsigned long				nID			= static_cast<unsigned long>(eventNode->m_dwEventAmount1);
				Item::EquipType::Grade		eItemGrade	= static_cast<Item::EquipType::Grade>(eventNode->m_dwEventAmount2);
				unsigned char				cBaseNum	= static_cast<unsigned char>(eventNode->m_dwEventAmount3);

				unsigned short wItemID = static_cast<unsigned short>(nID);
				if (0 == wItemID)
				{
					ERRLOG3(g_Log, "CID:%10u 퀘스트 보상으로 줄 아이템이 없습니다. ItemType:%d, ItemId:%d", 
						m_dwCID, eventNode->m_dwEventNumber, eventNode->m_dwEventAmount1);
					return E_EVENT_AWARDITEM;
				}

				Item::CItem* lpItem = Item::CItemFactory::GetInstance().CreateItem(wItemID);
				if (NULL == lpItem)
				{
					ERRLOG2(g_Log, "CID:%10u 아이템 생성에 실패하였습니다. ItemID:%d", 
						m_dwCID, wItemID);
					return E_EVENT_AWARDITEM;
				}

				Item::CEquipment* lpEquip = Item::CEquipment::DowncastToEquipment(lpItem);
				if (NULL != lpEquip)
				{
					if(lpEquip->AddRandomOption(eItemGrade, cBaseNum, GetMagicChancePoint()))
						lpEquip->SetNewEquip(2);
					else
						lpEquip->SetNewEquip();
				}
                
				if (false == GiveItem(lpItem))
				{
					ERRLOG1(g_Log, "CID:%10u 퀘스트 보상으로 장비를 받는데 실패하였습니다.", m_dwCID);
					DELETE_ITEM(lpItem);                    
				}

				GAMELOG::LogQuestGetReward(*this, executingQuest.m_QuestNode->m_wQuestID, lpItem, 
						0, 0, 0, 0, eReturnCode);


				// GievItem 으로 스택된 경우
				if (lpItem->IsSet(Item::DetailData::STACKABLE) && 0 == lpItem->GetNumOrDurability())
				{
					DELETE_ITEM(lpItem);
				}

				break;
			}

			case EVENT_AWARDITEMCLASS:
			{
				Item::EquipType::OptionType	eOptionType	= static_cast<Item::EquipType::OptionType>(eventNode->m_dwEventNumber);
				unsigned char				cClass		= static_cast<unsigned char>(eventNode->m_dwEventAmount1);
				unsigned char				cLevel		= static_cast<unsigned char>(eventNode->m_dwEventAmount2);
				Item::EquipType::Grade		eItemGrade	= static_cast<Item::EquipType::Grade>(eventNode->m_dwEventAmount3);
				unsigned char				cBaseNum	= static_cast<unsigned char>(eventNode->m_fPosX);

				if (0 == cClass)
				{
					// 캐릭터의 레벨을 참조하는 경우
					cLevel = static_cast<unsigned char>(m_DBData.m_Info.Class);
				}
				if (0 == cLevel)
				{
					// 캐릭터의 레벨을 참조하는 경우
					cLevel = m_DBData.m_Info.Level;
				}
				
				unsigned short wItemID = AwardTable::CAward::GetInstance().GetQuestEquipmentClass(eOptionType, cClass, cLevel, this);
				if (0 == wItemID)
				{
					ERRLOG3(g_Log, "CID:%10u 퀘스트 보상으로 줄 아이템이 없습니다. ItemType:%d, ItemLevel:%d", 
						m_dwCID, eventNode->m_dwEventNumber, eventNode->m_dwEventAmount1);
					return E_EVENT_AWARDITEM;
				}

				Item::CItem* lpItem = Item::CItemFactory::GetInstance().CreateItem(wItemID);
				if (NULL == lpItem)
				{
					ERRLOG2(g_Log, "CID:%10u 아이템 생성에 실패하였습니다. ItemID:%d", 
						m_dwCID, wItemID);
					return E_EVENT_AWARDITEM;
				}

				Item::CEquipment* lpEquip = Item::CEquipment::DowncastToEquipment(lpItem);
				if (NULL != lpEquip)
				{
					if(lpEquip->AddRandomOption(eItemGrade, cBaseNum, GetMagicChancePoint()))
						lpEquip->SetNewEquip(2);
					else
						lpEquip->SetNewEquip();
				}
                
				if (false == GiveItem(lpItem))
				{
					ERRLOG1(g_Log, "CID:%10u 퀘스트 보상으로 장비를 받는데 실패하였습니다.", m_dwCID);
					DELETE_ITEM(lpItem);                    
				}

				GAMELOG::LogQuestGetReward(*this, executingQuest.m_QuestNode->m_wQuestID, lpItem, 
						0, 0, 0, 0, eReturnCode);


				// GievItem 으로 스택된 경우
				if (lpItem->IsSet(Item::DetailData::STACKABLE) && 0 == lpItem->GetNumOrDurability())
				{
					DELETE_ITEM(lpItem);
				}

				break;
			}

			case EVENT_MOVE:
			{
				if (CServerSetup::GetInstance().GetServerZone() == eventNode->m_dwEventNumber)
				{
					Position newPos;
					newPos.m_fPointX = eventNode->m_fPosX;
					newPos.m_fPointY = eventNode->m_fPosY;
					newPos.m_fPointZ = eventNode->m_fPosZ;

					if (false == MovePos(newPos, static_cast<char>(eventNode->m_dwEventNumber), false))
					{
						ERRLOG5(g_Log, "CID:%10u 퀘스트 이벤트 존 이동에 실패했습니다. Zone:%d  X:%f Y:%f Z:%f",
								m_dwCID, eventNode->m_dwEventNumber, newPos.m_fPointX, newPos.m_fPointY, newPos.m_fPointZ);
					}
				}
				else
				{
					POS newPos;
					newPos.fPointX = eventNode->m_fPosX;
					newPos.fPointY = eventNode->m_fPosY;
					newPos.fPointZ = eventNode->m_fPosZ;

					if (false == MoveZone(newPos, static_cast<char>(eventNode->m_dwEventNumber), -1))
					{
						ERRLOG5(g_Log, "CID:%10u 퀘스트 이벤트 존 이동에 실패했습니다. Zone:%d  X:%f Y:%f Z:%f",
								m_dwCID, eventNode->m_dwEventNumber, newPos.fPointX, newPos.fPointY, newPos.fPointZ);
					}
				}

				break;
			}

			case EVENT_THEATERMODE:
			{
				break;
			}

			default:
			{
				ERRLOG2(g_Log, "CID:%10u 존재하지 않는 이벤트 종류입니다. 종류:%d", m_dwCID, eventNode->m_dwEventKind);
				break;
			}
		}
	}

	if (false == vecItemGarbage.empty())
	{
		ClearGarbage(vecItemGarbage, PktDisappearItem::DIC_DEFAULT);
	}

	return S_SUCCESS;
}

void CCharacter::EventEnd(unsigned short wQuestID, bool bSave)
{
	if (true == bSave)
	{
		int nIndex = 0;

		for (; nIndex < MAX_HISTORY_QUEST; ++nIndex)
		{
			if (0 == m_wHistoryQuest[nIndex])
			{
				m_wHistoryQuest[nIndex] = wQuestID;
				break;
			}
		}

		if (MAX_HISTORY_QUEST == nIndex)
		{
			ERRLOG2(g_Log, "CID:%10u 퀘스트 히스토리에 저장된 퀘스트가 %d(MAX)개를 초과하였습니다.", 
				m_dwCID, MAX_HISTORY_QUEST);
		}
		
		// 최대 스킬 포인트를 늘여주는 퀘스트의 처리
		QuestNode* lpQuest = CQuestMgr::GetInstance().GetQuestNode(m_wHistoryQuest[nIndex]);
		if (NULL != lpQuest)
		{
			m_CreatureStatus.m_StatusInfo.m_wSkillPoint += lpQuest->m_cSkillPointBonus;
			m_iAbilityPoint += lpQuest->m_cAbilityPointBonus;
		}
	}

	// 퀘스트를 삭제한다.
	CancelQuest(wQuestID, true);
}

void CCharacter::EventAward(unsigned long dwExp, unsigned long dwGold, unsigned long dwFame, unsigned long dwMileage)
{
	IncrementExp(dwExp);				

	float fPt = GetAwardPer();
	unsigned long dwGold1 = (unsigned long)((float)dwGold*fPt);
	unsigned long dwFame1 = (unsigned long)((float)dwFame*fPt);
	unsigned long dwMileage1 = (unsigned long)((float)dwMileage*fPt);

	if (0 != dwGold1)
	{
		unsigned long dwSrcGold = GetGold();

		AddGold(dwGold1, true);
		
		GAMELOG::LogTakeGold(*this, dwSrcGold, GetGold(), dwGold1, 
			TakeType::TS_INVEN, TakeType::TS_INVEN, GAMELOG::sTakeGoldLogV2::QUEST_AWARD, 0);
	}

    SetFame(GetFame() + dwFame1);
	SetMileage(GetMileage() + dwMileage1);

	if (NULL != m_lpGameClientDispatch)
	{
		GameClientSendPacket::SendCharFameInfo(m_lpGameClientDispatch->GetSendStream(), this, 
			"", "", 0, 0, PktFIAck::FAME_INFO, PktBase::NO_SERVER_ERR);
	}
}

bool CCharacter::ClearGarbage(vector<Item::ItemGarbage>& vecItemGarbage, unsigned char cCmd)
{
	// 돈
	unsigned long dwGold = vecItemGarbage[0].m_dwRemainNum;
	if (0 != dwGold)
	{
        unsigned long dwSrcGold = GetGold();

		if (false == DeductGold(dwGold, false)) 
		{ 
			return false; 
		}		
        else
        {
            GAMELOG::LogTakeGold(*this, dwSrcGold, GetGold(), dwGold, 
                TakeType::TS_INVEN, TakeType::TS_INVEN, GAMELOG::sTakeGoldLogV2::QUEST_DEDUCT, 0);
        }

		if (NULL != m_lpGameClientDispatch)
		{
			GameClientSendPacket::SendCharTakeGold(m_lpGameClientDispatch->GetSendStream(), 
				m_dwCID, dwGold, TakeType::TS_INVEN, TakeType::TS_NONE, 0);
		}
	}

	// 아이템
	for (size_t nIndex = vecItemGarbage.size() - 1; nIndex > 0; --nIndex)
	{
		Item::CItem* lpItem = vecItemGarbage[nIndex].m_lpItem;
		unsigned char cRemainNum = static_cast<unsigned char>(vecItemGarbage[nIndex].m_dwRemainNum);

		if (NULL != m_lpGameClientDispatch)
		{
			GameClientSendPacket::SendCharDisappearItem(m_lpGameClientDispatch->GetSendStream(), 
				m_dwCID, lpItem->GetPos(), cRemainNum, cCmd, PktBase::NO_SERVER_ERR);
		}

		if (0 == cRemainNum)
		{
			RemoveItem(lpItem->GetPos());
			DELETE_ITEM(lpItem);
		}
		else
		{
			lpItem->SetNumOrDurability(cRemainNum);
		}

		vecItemGarbage.pop_back();
	}

	return true;
}

bool CCharacter::HasExecutingQuest(unsigned short wQuestID)
{
	for (int nIndex = 0; nIndex < MAX_EXECUTING_QUEST; ++nIndex)
	{
		if (NULL != m_ExecutingQuest[nIndex].m_QuestNode)
		{
			if (m_ExecutingQuest[nIndex].m_QuestNode->m_wQuestID == wQuestID)
			{
				return true;
			}
		}
	}

	return false;
}

bool CCharacter::HasHistoryQuest(unsigned short wQuestID)
{
	for (int nIndex = 0; nIndex < MAX_HISTORY_QUEST; ++nIndex)
	{
		if (m_wHistoryQuest[nIndex] == wQuestID)
		{
			return true;
		}
	}

	return false;
}

bool CCharacter::InsertHistoryQuest(unsigned short wQuestID)
{
	if (0 != m_wHistoryQuest[MAX_HISTORY_QUEST-1]) { return false;	}
	
	for (int nIndex = MAX_HISTORY_QUEST-1; nIndex > 0; --nIndex)
	{
		m_wHistoryQuest[nIndex] = m_wHistoryQuest[nIndex-1];
	}

	m_wHistoryQuest[0] = wQuestID;
	return true;
}

bool CCharacter::DeleteHistoryQuest(unsigned short wQuestID)
{
	if (false == HasHistoryQuest(wQuestID))	{ return false;	}

	for (int nIndex = 0; nIndex < MAX_HISTORY_QUEST; ++nIndex)
	{
		if (wQuestID == m_wHistoryQuest[nIndex])
		{
			if (nIndex == MAX_HISTORY_QUEST - 1)
			{
				m_wHistoryQuest[nIndex] = 0;
			}
			else
			{
				std::copy(&m_wHistoryQuest[nIndex + 1], &m_wHistoryQuest[MAX_HISTORY_QUEST - 1], &m_wHistoryQuest[nIndex]);
				m_wHistoryQuest[MAX_HISTORY_QUEST - 1] = 0;
			}

			return true;
		}
	}

	return false;
}

void CCharacter::CalculateStatusByQuest(void)
{
	// 2009.02.21 해당 문제를 위로 올려 장비창에서 영향을 받게한다.
	// 현재 장비창에 있는 정보를 m_CreatureStatus.m_StatusInfo로 업데이트 하게 되어있는데
	// m_CreatureStatus.m_StatusInfo 이값은 여기서 1번만 적용이 되고
	// 나중에 스펠에 의해 능력치가 + 될때에는 CalculateStatusByQuest() 이함수가 호출되지 않기 때문에 값이 넘어가게된다.
	// 퀘스트에 의해 받는 영향을 계산

	// 어빌리티는 100% 퀘스트에 의해 영향을 받는다.
	m_iAbilityPoint = 0;
	m_iAbilityPoint += m_iAdminAbilityPoint;

	for (int nIndex = 0; nIndex < MAX_HISTORY_QUEST; ++nIndex)
	{
		QuestNode* lpQuest = CQuestMgr::GetInstance().GetQuestNode(m_wHistoryQuest[nIndex]);
		if (NULL != lpQuest)
		{
			m_EquipStatus.m_wSkillPoint += lpQuest->m_cSkillPointBonus;
			m_iAbilityPoint += lpQuest->m_cAbilityPointBonus;			
//			m_CreatureStatus.m_StatusInfo.m_wSkillPoint += lpQuest->m_cSkillPointBonus;
		}
	}
}
