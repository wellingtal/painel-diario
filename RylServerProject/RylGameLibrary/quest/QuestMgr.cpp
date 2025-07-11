
#include "stdafx.h"

#include <Item/ItemConstants.h>

#include "QuestStruct.h"
#include "QuestMgr.h"
#include <Creature/CreatureManager.h>
#include <Creature/Character/Character.h>

using namespace Quest;

const char* szQuestScriptFileName = "./Script/Game/Quest.mcf";

CQuestMgr CQuestMgr::ms_this;


CQuestMgr::CQuestMgr()
	: m_lpQuestNode(NULL), m_lpPhaseNode(NULL), m_lpTriggerNode(NULL),
	m_bFalseEvent(false), m_bDangerousEvent(false), m_vecPendingQuest(NULL), m_vecSubPendingQuest(NULL), m_bPendingQuest(false)
{
}

CQuestMgr::~CQuestMgr()
{
	ClearQuest();
}


bool CQuestMgr::AddQuest(QuestNode* questNode)
{
	m_lpQuestNode = questNode;
	m_lstQuest.push_back(questNode);

	m_lpPhaseNode = NULL;
	m_lpTriggerNode = NULL;

	return true;
}

bool CQuestMgr::AddPhase(PhaseNode* phaseNode)
{
	if (NULL == m_lpQuestNode)
	{
		SERLOG0(g_Log, "퀘스트가 존재하지 않습니다.");
		return false;
	}

	if (phaseNode->m_dwPhaseNumber != 1)
	{
		if (NULL == m_lpPhaseNode || phaseNode->m_dwPhaseNumber != m_lpPhaseNode->m_dwPhaseNumber + 1)
		{
			SERLOG1(g_Log, "페이즈 순서가 잘못되었습니다. QuestID:0x%04x", m_lpQuestNode->m_wQuestID);
			return false;
		}
	}

	m_lpPhaseNode = phaseNode;
	m_lpQuestNode->m_lstPhase.push_back(phaseNode);
	m_lpTriggerNode = NULL;

	return true;
}

bool CQuestMgr::AddTrigger(TriggerNode* triggerNode)
{
	if (NULL == m_lpPhaseNode)
	{
		SERLOG0(g_Log, "페이즈가 존재하지 않습니다.");
		return false;
	}

	m_lpTriggerNode = triggerNode;
	m_lpPhaseNode->m_lstTrigger.push_back(triggerNode);

	m_bFalseEvent = false;
	m_bDangerousEvent = false;

	return true;
}

bool CQuestMgr::AddEvent(EventNode* eventNode)
{
	if (NULL == m_lpTriggerNode)
	{
		SERLOG0(g_Log, "트리거가 존재하지 않습니다.");
		return false;
	}

	// 실패할 수 있는 이벤트(Disappear, Get)는 위험한 이벤트(Award) 뒤에 올 수 없다.
	if (EVENT_AWARD == eventNode->m_dwEventKind)
	{
		m_bDangerousEvent = true;
	}
	if (true == m_bDangerousEvent)
	{
		if (EVENT_DISAPPEAR == eventNode->m_dwEventKind || EVENT_GET == eventNode->m_dwEventKind)
		{
			SERLOG2(g_Log, "퀘스트 스크립트 에러. QuestID:0x%04x Phase:%d",
				m_lpQuestNode->m_wQuestID, m_lpPhaseNode->m_dwPhaseNumber);
			return false;
		}
	}

	// 퀘스트 삭제시 함께 지워질 아이템의 목록을 작성해둔다.
	if (EVENT_GET == eventNode->m_dwEventKind)
	{
		unsigned short wItemID = static_cast<unsigned short>(eventNode->m_dwEventNumber);
		unsigned char cItemNum = static_cast<unsigned char>(eventNode->m_dwEventAmount1);

		m_lpPhaseNode->m_lstDeletedItem.push_back(PhaseNode::DeletedItem(wItemID, cItemNum));
	}

	if (true == m_bFalseEvent)
	{
		m_lpTriggerNode->m_lstFalseEvent.push_back(eventNode);
	}
	else
	{
		m_lpTriggerNode->m_lstEvent.push_back(eventNode);
	}

	return true;
}

bool CQuestMgr::AddPendingQuest(unsigned long dwGID, unsigned short wQuestID, unsigned char cPhase, unsigned char cTrigger, unsigned char cCount, Position Pos)
{
	TriggerMsg triggerMsg;
	triggerMsg.m_dwCID = dwGID;
	triggerMsg.m_cCount = cCount;
	triggerMsg.m_cPhase = cPhase;
	triggerMsg.m_cTrigger = cTrigger;
	triggerMsg.m_wQuestID = wQuestID;
	triggerMsg.m_Pos(Pos);

	if (!m_bPendingQuest)
	{
		m_vecPendingQuest.push_back(triggerMsg);
	}
	else
	{
		m_vecSubPendingQuest.push_back(triggerMsg);
	}

	return true;
}

bool CQuestMgr::SwapPendingQuest()
{
	std::swap(m_vecPendingQuest, m_vecSubPendingQuest);

	return true;
}

void CQuestMgr::ProcessPendingQuest()
{
	m_bPendingQuest = true;

	while (!m_vecPendingQuest.empty() || !m_vecSubPendingQuest.empty())
	{
		// 양쪽에 큐에 퀘스트가 없으면 리턴.
		if (m_vecPendingQuest.empty() && m_vecSubPendingQuest.empty())
		{
			goto lb_exit;
		}

		// 퀘스트 처리.
		if (!m_vecPendingQuest.empty())
		{
			vector<Quest::TriggerMsg>::iterator pos = m_vecPendingQuest.begin();
			vector<Quest::TriggerMsg>::iterator end = m_vecPendingQuest.end();

			for (; pos != end; ++pos)
			{
				CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter((*pos).m_dwCID);

				if (lpCharacter)
				{
					lpCharacter->PendingQuest(&(*pos));
				}

			}
			// 퀘스트 삭제.
			m_vecPendingQuest.clear();
		}

		// 퀘스트가 없으면.
		if (m_vecPendingQuest.empty())
		{
			SwapPendingQuest();
		}
	}

lb_exit:

	m_bPendingQuest = false;
}

struct CompareQuestIDForSort
{
	int operator() (const QuestNode* node1, const QuestNode* node2) { return (node1->m_wQuestID < node2->m_wQuestID); }
};

void CQuestMgr::SortQuests(void)
{
	std::sort(m_lstQuest.begin(), m_lstQuest.end(), CompareQuestIDForSort());
}


struct CompareQuestIDForSearch
{
	bool operator () (const QuestNode* node, const unsigned short id) { return (node->m_wQuestID < id); }
	bool operator () (const unsigned short id, const QuestNode* node) { return (id < node->m_wQuestID); }
};

Quest::QuestNode* CQuestMgr::GetQuestNode(unsigned short wQuestID)
{
	vector<QuestNode *>::iterator itr =
		std::lower_bound(m_lstQuest.begin(), m_lstQuest.end(), wQuestID, CompareQuestIDForSearch());

	return (itr != m_lstQuest.end() && (*itr)->m_wQuestID == wQuestID) ? *itr : NULL;
}


void CQuestMgr::ClearQuest(void)
{
	vector<QuestNode *>::iterator itQuest;
	vector<PhaseNode *>::iterator itPhase;
	vector<TriggerNode *>::iterator itTrigger;
	vector<EventNode *>::iterator itEvent;

	QuestNode* lpQuest = NULL;
	PhaseNode* lpPhase = NULL;
	TriggerNode* lpTrigger = NULL;
	EventNode* lpEvent = NULL;

	for (itQuest = m_lstQuest.begin(); itQuest != m_lstQuest.end(); )
	{
		lpQuest = (*itQuest);
		itQuest = m_lstQuest.erase(itQuest);

		for (itPhase = lpQuest->m_lstPhase.begin(); itPhase != lpQuest->m_lstPhase.end(); )
		{
			lpPhase = (*itPhase);
			itPhase = lpQuest->m_lstPhase.erase(itPhase);

			for (itTrigger = lpPhase->m_lstTrigger.begin(); itTrigger != lpPhase->m_lstTrigger.end(); )
			{
				lpTrigger = (*itTrigger);
				itTrigger = lpPhase->m_lstTrigger.erase(itTrigger);

				for (itEvent = lpTrigger->m_lstEvent.begin(); itEvent != lpTrigger->m_lstEvent.end(); )
				{
					lpEvent = (*itEvent);
					itEvent = lpTrigger->m_lstEvent.erase(itEvent);

					delete lpEvent;
				}
				lpTrigger->m_lstEvent.clear();

				for (itEvent = lpTrigger->m_lstFalseEvent.begin(); itEvent != lpTrigger->m_lstFalseEvent.end(); )
				{
					lpEvent = (*itEvent);
					itEvent = lpTrigger->m_lstFalseEvent.erase(itEvent);

					delete lpEvent;
				}
				lpTrigger->m_lstFalseEvent.clear();

				delete lpTrigger;
			}
			lpPhase->m_lstTrigger.clear();

			delete lpPhase;
		}

		lpQuest->m_lstPhase.clear();

		delete lpQuest;
	}

	m_lstQuest.clear();
}

bool CQuestMgr::LoadQuestInfo(void)
{
	CQuestMgr::GetInstance().ClearQuest();

	_SE_SetMessageFunction(CQuestMgr::ScriptErrorMessage);

	SCRIPT Script = _SE_Create(szQuestScriptFileName);
	if (NULL == Script) { return false; }

	_SE_RegisterFunction(Script, ScriptQuestStartLevel, T_VOID, "QuestStartLevel", T_INT, T_INT, T_INT, T_INT, T_INT, T_INT, T_BOOL, 0);
	_SE_RegisterFunction(Script, ScriptQuestStartFame, T_VOID, "QuestStartFame", T_INT, T_INT, T_INT, T_INT, T_INT, T_INT, T_BOOL, 0);
	_SE_RegisterFunction(Script, ScriptQuestEnd, T_VOID, "QuestEnd", 0);
	_SE_RegisterFunction(Script, ScriptQuestType, T_VOID, "QuestType", T_INT, T_INT, 0);
	_SE_RegisterFunction(Script, ScriptQuestArea, T_VOID, "QuestArea", T_INT, T_FLOAT, T_FLOAT, T_FLOAT, T_FLOAT, 0);
	_SE_RegisterFunction(Script, ScriptQuestTitle, T_VOID, "QuestTitle", T_STRING, 0);
	_SE_RegisterFunction(Script, ScriptQuestDesc, T_VOID, "QuestDesc", T_STRING, 0);
	_SE_RegisterFunction(Script, ScriptQuestShortDesc, T_VOID, "QuestShortDesc", T_STRING, 0);
	_SE_RegisterFunction(Script, ScriptQuestIcon, T_VOID, "QuestIcon", T_STRING, T_INT, T_INT, T_INT, T_INT, 0);
	_SE_RegisterFunction(Script, ScriptQuestCompleteSave, T_VOID, "QuestCompleteSave", T_BOOL, 0);
	_SE_RegisterFunction(Script, ScriptQuestLevel, T_VOID, "QuestLevel", T_STRING, 0);
	_SE_RegisterFunction(Script, ScriptQuestAward, T_VOID, "QuestAward", T_STRING, 0);
	_SE_RegisterFunction(Script, ScriptQuestSkillPointBonus, T_VOID, "QuestSkillPointBonus", T_INT, 0);
	_SE_RegisterFunction(Script, ScriptQuestCancelItemDel, T_VOID, "QuestCancelItemDel", T_BOOL, 0);

	_SE_RegisterFunction(Script, ScriptAddPhase, T_VOID, "AddPhase", T_INT, T_INT, T_STRING, 0);
	_SE_RegisterFunction(Script, ScriptPhaseTarget, T_VOID, "Phase_Target", T_INT, T_INT, T_INT, 0);
	_SE_RegisterFunction(Script, ScriptTriggerStart, T_VOID, "Trigger_Start", 0);
	_SE_RegisterFunction(Script, ScriptTriggerPuton, T_VOID, "Trigger_Puton", T_INT, T_INT, T_FLOAT, T_FLOAT, T_FLOAT, T_FLOAT, 0);
	_SE_RegisterFunction(Script, ScriptTriggerGeton, T_VOID, "Trigger_Geton", T_INT, T_INT, T_FLOAT, T_FLOAT, T_FLOAT, T_FLOAT, 0);
	_SE_RegisterFunction(Script, ScriptTriggerTalk, T_VOID, "Trigger_Talk", T_INT, 0);
	_SE_RegisterFunction(Script, ScriptTriggerKill, T_VOID, "Trigger_Kill", T_INT, T_INT, 0);
	_SE_RegisterFunction(Script, ScriptTriggerPick, T_VOID, "Trigger_Pick", T_INT, T_INT, 0);
	_SE_RegisterFunction(Script, ScriptTriggerFame, T_VOID, "Trigger_Fame", T_INT, 0);
	_SE_RegisterFunction(Script, ScriptTriggerLevelTalk, T_VOID, "Trigger_LevelTalk", T_INT, T_INT, T_STRING, 0);
	_SE_RegisterFunction(Script, ScriptTriggerFameTalk, T_VOID, "Trigger_FameTalk", T_INT, T_INT, T_STRING, 0);
	_SE_RegisterFunction(Script, ScriptElse, T_VOID, "Else", 0);

	_SE_RegisterFunction(Script, ScriptEventDisappear, T_VOID, "Event_Disappear", T_INT, T_INT, T_INT, 0);
	_SE_RegisterFunction(Script, ScriptEventGet, T_VOID, "Event_Get", T_INT, T_INT, 0);
	_SE_RegisterFunction(Script, ScriptEventSpawn, T_VOID, "Event_Spawn", T_INT, T_FLOAT, T_FLOAT, T_FLOAT, 0);
	_SE_RegisterFunction(Script, ScriptEventMonsterDrop, T_VOID, "Event_MonsterDrop", T_INT, T_INT, 0);
	_SE_RegisterFunction(Script, ScriptEventAward, T_VOID, "Event_Award", T_INT, T_INT, T_INT, T_INT, 0);
	_SE_RegisterFunction(Script, ScriptEventMsgBox, T_VOID, "Event_MsgBox", T_STRING, 0);
	_SE_RegisterFunction(Script, ScriptEventPhase, T_VOID, "Event_Phase", T_INT, 0);
	_SE_RegisterFunction(Script, ScriptEventEnd, T_VOID, "Event_End", T_VOID, 0);
	_SE_RegisterFunction(Script, ScriptEventAddQuest, T_VOID, "Event_AddQuest", T_INT, T_INT, 0);
	_SE_RegisterFunction(Script, ScriptEventAwardItem, T_VOID, "Event_AwardItem", T_INT, T_INT, T_INT, T_INT, 0);
	_SE_RegisterFunction(Script, ScriptEventAwardItemID, T_VOID, "Event_AwardItemID", T_INT, T_INT, T_INT, T_INT, 0);
	_SE_RegisterFunction(Script, ScriptEventAwardItemClass, T_VOID, "Event_AwardItemClass", T_INT, T_INT, T_INT, T_INT, T_INT, 0);
	_SE_RegisterFunction(Script, ScriptEventMove, T_VOID, "Event_Move", T_INT, T_FLOAT, T_FLOAT, T_FLOAT, 0);
	_SE_RegisterFunction(Script, ScriptEventTheaterMode, T_VOID, "Event_TheaterMode", 0);
	_SE_RegisterFunction(Script, ScriptEventAlarmBox, T_VOID, "Event_AlarmBox", T_STRING, 0);

	// 어빌리티 시스템
	_SE_RegisterFunction(Script, ScriptQuestAbilityPoint, T_VOID, "QuestAbilityPoint", T_INT, 0);
	_SE_RegisterFunction(Script, ScriptEventAwardAbility, T_VOID, "Event_AwardAbility", T_INT, T_INT, 0);
	_SE_RegisterFunction(Script, ScriptTriggerAbilityTalk, T_VOID, "Trigger_AbilityTalk", T_INT, T_INT, T_INT, T_STRING, 0);


	_SE_Execute(Script);
	_SE_Destroy(Script);

	CQuestMgr::GetInstance().SortQuests();

	return true;
}

void CQuestMgr::ScriptErrorMessage(const char *msg)
{
	MessageBox(NULL, msg, "Script Error", MB_OK);
}

void CQuestMgr::ScriptQuestStartLevel(int nQuestID, int nMinLevel, int nMaxLevel, int nClass, int nCompleteQuest, int nNation, bool bNationDependent)
{
	// Quest를 세팅한다.
	QuestNode* questNode = new QuestNode(nQuestID, 0, 0, nMinLevel, nMaxLevel, 0, 0, nClass, nCompleteQuest, false, nNation, bNationDependent);

	CQuestMgr::GetInstance().AddQuest(questNode);
}

void CQuestMgr::ScriptQuestStartFame(int nQuestID, int nMinFame, int nMaxFame, int nClass, int nCompleteQuest, int nNation, bool bNationDependent)
{
	// Quest를 세팅한다.
	QuestNode* questNode = new QuestNode(nQuestID, 0, 0, 0, 0, nMinFame, nMaxFame, nClass, nCompleteQuest, false, nNation, bNationDependent);

	CQuestMgr::GetInstance().AddQuest(questNode);
}

void CQuestMgr::ScriptQuestEnd()
{
	CQuestMgr::GetInstance().QuestEnd();
}

void CQuestMgr::ScriptQuestType(unsigned short wQuestType, unsigned short wPartyQuest)
{
	if (0 != wPartyQuest)
	{
		wQuestType |= Quest::PARTY_QUEST_BIT;
	}

	CQuestMgr::GetInstance().QuestType(wQuestType);
}

void CQuestMgr::ScriptQuestArea(int nZone, float fPosX, float fPosY, float fPosZ, float fDistance)
{
	CQuestMgr::GetInstance().QuestArea(nZone, fPosX, fPosY, fPosZ, fDistance);
}

void CQuestMgr::ScriptQuestTitle(const char *strQuestTitle)
{

}

void CQuestMgr::ScriptQuestDesc(const char *strQuestDesc)
{

}

void CQuestMgr::ScriptQuestShortDesc(const char *strQuestShortDesc)
{

}

void CQuestMgr::ScriptQuestIcon(const char *strIconTexture, int nPosX1, int nPosY1, int nPosX2, int nPosY2)
{

}

void CQuestMgr::ScriptQuestCompleteSave(bool bSave)
{
	CQuestMgr::GetInstance().QuestCompleteSave(bSave);
}

void CQuestMgr::ScriptQuestLevel(const char *strQuestLevel)
{

}

void CQuestMgr::ScriptQuestAward(const char *strQuestAward)
{

}

void CQuestMgr::ScriptQuestSkillPointBonus(int nBonusNum)
{
	CQuestMgr::GetInstance().QuestSkillPointBonus(nBonusNum);
}

void CQuestMgr::ScriptQuestAbilityPoint(int nBonusNum)
{
	CQuestMgr::GetInstance().QuestAbilityPoint(nBonusNum);
}

void CQuestMgr::ScriptQuestCancelItemDel(bool bDelLinkItem)
{
	CQuestMgr::GetInstance().QuestSetDelLinkItem(bDelLinkItem);
}

void CQuestMgr::ScriptAddPhase(int nZoneID, int nPhaseNumber, const char* strQuestDesc)
{
	PhaseNode* phaseNode = new PhaseNode(nZoneID, nPhaseNumber);
	if (false == CQuestMgr::GetInstance().AddPhase(phaseNode))
	{
		SERLOG3(g_Log, "퀘스트 스크립트 에러. AddPhase(%d, %d, %s)",
			nZoneID, nPhaseNumber, strQuestDesc);
		assert(false && "Quest 스크립트에 문제가 있습니다. 자세한 사항은 로그를 참고하세요.");

		delete phaseNode;
	}
}

void CQuestMgr::ScriptPhaseTarget(int nType, int nPosX, int nPosY)
{

}

void CQuestMgr::ScriptTriggerStart(void)
{
	TriggerNode* triggerNode = new TriggerNode(TRIGGER_START, 0, 0, 1, 0, 0, 0, 0, 0);
	if (false == CQuestMgr::GetInstance().AddTrigger(triggerNode))
	{
		delete triggerNode;
		assert(false && "Quest 스크립트에 문제가 있습니다. 자세한 사항은 로그를 참고하세요.");
	}
}

void CQuestMgr::ScriptTriggerPuton(int nItemID, int nZoneID, float fPosX, float fPosY, float fPosZ, float fDistance)
{
	TriggerNode* triggerNode = new TriggerNode(TRIGGER_PUTON, nItemID, nZoneID, 1, 0, fPosX, fPosY, fPosZ, fDistance);
	if (false == CQuestMgr::GetInstance().AddTrigger(triggerNode))
	{
		delete triggerNode;
		assert(false && "Quest 스크립트에 문제가 있습니다. 자세한 사항은 로그를 참고하세요.");
	}
}

void CQuestMgr::ScriptTriggerGeton(int nItemID, int nZoneID, float fPosX, float fPosY, float fPosZ, float fDistance)
{
	TriggerNode* triggerNode = new TriggerNode(TRIGGER_GETON, nItemID, nZoneID, 1, 0, fPosX, fPosY, fPosZ, fDistance);
	if (false == CQuestMgr::GetInstance().AddTrigger(triggerNode))
	{
		delete triggerNode;
		assert(false && "Quest 스크립트에 문제가 있습니다. 자세한 사항은 로그를 참고하세요.");
	}
}

void CQuestMgr::ScriptTriggerTalk(int nNpcID)
{
	TriggerNode* triggerNode = new TriggerNode(TRIGGER_TALK, nNpcID, 0, 1, 0, 0, 0, 0, 0);
	if (false == CQuestMgr::GetInstance().AddTrigger(triggerNode))
	{
		delete triggerNode;
		assert(false && "Quest 스크립트에 문제가 있습니다. 자세한 사항은 로그를 참고하세요.");
	}
}

void CQuestMgr::ScriptTriggerKill(int nCount, int nMonsterID)
{
	TriggerNode* triggerNode = new TriggerNode(TRIGGER_KILL, nMonsterID, 0, nCount, 0, 0, 0, 0, 0);
	if (false == CQuestMgr::GetInstance().AddTrigger(triggerNode))
	{
		delete triggerNode;
		assert(false && "Quest 스크립트에 문제가 있습니다. 자세한 사항은 로그를 참고하세요.");
	}
}

void CQuestMgr::ScriptTriggerPick(int nCount, int nItemID)
{
	TriggerNode* triggerNode = new TriggerNode(TRIGGER_PICK, nItemID, 0, nCount, 0, 0, 0, 0, 0);
	if (false == CQuestMgr::GetInstance().AddTrigger(triggerNode))
	{
		delete triggerNode;
		assert(false && "Quest 스크립트에 문제가 있습니다. 자세한 사항은 로그를 참고하세요.");
	}
}

void CQuestMgr::ScriptTriggerFame(int nFame)
{
	TriggerNode* triggerNode = new TriggerNode(TRIGGER_FAME, nFame, 0, 1, 0, 0, 0, 0, 0);
	if (false == CQuestMgr::GetInstance().AddTrigger(triggerNode))
	{
		delete triggerNode;
		assert(false && "Quest 스크립트에 문제가 있습니다. 자세한 사항은 로그를 참고하세요.");
	}
}

void CQuestMgr::ScriptTriggerLevelTalk(int nLevel, int nNPCID, const char* strElesMsg)
{
	TriggerNode* triggerNode = new TriggerNode(TRIGGER_LEVEL_TALK, nNPCID, 0, 1, nLevel, 0, 0, 0, 0);
	if (false == CQuestMgr::GetInstance().AddTrigger(triggerNode))
	{
		delete triggerNode;
		assert(false && "Quest 스크립트에 문제가 있습니다. 자세한 사항은 로그를 참고하세요.");
	}
}

void CQuestMgr::ScriptTriggerFameTalk(int nFame, int nNPCID, const char* strElesMsg)
{
	TriggerNode* triggerNode = new TriggerNode(TRIGGER_FAME_TALK, nNPCID, 0, 1, nFame, 0, 0, 0, 0);
	if (false == CQuestMgr::GetInstance().AddTrigger(triggerNode))
	{
		delete triggerNode;
		assert(false && "Quest 스크립트에 문제가 있습니다. 자세한 사항은 로그를 참고하세요.");
	}
}

void CQuestMgr::ScriptTriggerAbilityTalk(int nAbilityID, int nAbilityLV, int nNPCID, const char* strElesMsg)
{
	TriggerNode* triggerNode = new TriggerNode(TRIGGER_ABILITY_TALK, nNPCID, nAbilityID, 1, nAbilityLV, 0, 0, 0, 0);
	if (false == CQuestMgr::GetInstance().AddTrigger(triggerNode))
	{
		delete triggerNode;
		assert(false && "Quest 스크립트에 문제가 있습니다. 자세한 사항은 로그를 참고하세요.");
	}
}


void CQuestMgr::ScriptElse(void)
{
	CQuestMgr::GetInstance().Else();
}

void CQuestMgr::ScriptEventDisappear(int nAmount, int nItemID, int nGold)
{
	EventNode* eventNode = new EventNode(EVENT_DISAPPEAR, nItemID, nAmount, nGold, 0, 0, 0, 0);
	if (false == CQuestMgr::GetInstance().AddEvent(eventNode))
	{
		delete eventNode;
		assert(false && "Quest 스크립트에 문제가 있습니다. 자세한 사항은 로그를 참고하세요.");
	}
}

void CQuestMgr::ScriptEventGet(int nAmount, int nItemID)
{
	EventNode* eventNode = new EventNode(EVENT_GET, nItemID, nAmount, 0, 0, 0, 0, 0);
	if (false == CQuestMgr::GetInstance().AddEvent(eventNode))
	{
		delete eventNode;
		assert(false && "Quest 스크립트에 문제가 있습니다. 자세한 사항은 로그를 참고하세요.");
	}
}

void CQuestMgr::ScriptEventSpawn(int nMonsterID, float fPosX, float fPosY, float fPosZ)
{
	EventNode* eventNode = new EventNode(EVENT_SPAWN, nMonsterID, 0, 0, 0, fPosX, fPosY, fPosZ);
	if (false == CQuestMgr::GetInstance().AddEvent(eventNode))
	{
		delete eventNode;
		assert(false && "Quest 스크립트에 문제가 있습니다. 자세한 사항은 로그를 참고하세요.");
	}
}

void CQuestMgr::ScriptEventMonsterDrop(int nAmount, int nItemID)
{
	EventNode* eventNode = new EventNode(EVENT_MONSTERDROP, nItemID, nAmount, 0, 0, 0, 0, 0);
	if (false == CQuestMgr::GetInstance().AddEvent(eventNode))
	{
		delete eventNode;
		assert(false && "Quest 스크립트에 문제가 있습니다. 자세한 사항은 로그를 참고하세요.");
	}
}

void CQuestMgr::ScriptEventAward(int nExp, int nGold, int nFame, int nMileage)
{
	EventNode* eventNode = new EventNode(EVENT_AWARD, nExp, nGold, nFame, nMileage, 0, 0, 0);
	if (false == CQuestMgr::GetInstance().AddEvent(eventNode))
	{
		delete eventNode;
		assert(false && "Quest 스크립트에 문제가 있습니다. 자세한 사항은 로그를 참고하세요.");
	}
}

void CQuestMgr::ScriptEventMsgBox(const char *strMessage)
{
	EventNode* eventNode = new EventNode(EVENT_MSGBOX, 0, 0, 0, 0, 0, 0, 0);
	if (false == CQuestMgr::GetInstance().AddEvent(eventNode))
	{
		delete eventNode;
		assert(false && "Quest 스크립트에 문제가 있습니다. 자세한 사항은 로그를 참고하세요.");
	}
}

void CQuestMgr::ScriptEventAlarmBox(const char *strMessage)
{
	EventNode* eventNode = new EventNode(EVENT_ALARMBOX, 0, 0, 0, 0, 0, 0, 0);
	if (false == CQuestMgr::GetInstance().AddEvent(eventNode))
	{
		delete eventNode;
		assert(false && "Quest 스크립트에 문제가 있습니다. 자세한 사항은 로그를 참고하세요.");
	}
}

void CQuestMgr::ScriptEventPhase(int nPhaseNumber)
{
	EventNode* eventNode = new EventNode(EVENT_PHASE, nPhaseNumber, 0, 0, 0, 0, 0, 0);
	if (false == CQuestMgr::GetInstance().AddEvent(eventNode))
	{
		delete eventNode;
		assert(false && "Quest 스크립트에 문제가 있습니다. 자세한 사항은 로그를 참고하세요.");
	}
}

void CQuestMgr::ScriptEventEnd(void)
{
	EventNode* eventNode = new EventNode(EVENT_END, 0, 0, 0, 0, 0, 0, 0);
	if (false == CQuestMgr::GetInstance().AddEvent(eventNode))
	{
		delete eventNode;
		assert(false && "Quest 스크립트에 문제가 있습니다. 자세한 사항은 로그를 참고하세요.");
	}
}

void CQuestMgr::ScriptEventAddQuest(int nNpcID, int nQuestID)
{
	EventNode* eventNode = new EventNode(EVENT_ADDQUEST, nNpcID, nQuestID, 0, 0, 0, 0, 0);
	if (false == CQuestMgr::GetInstance().AddEvent(eventNode))
	{
		delete eventNode;
		assert(false && "Quest 스크립트에 문제가 있습니다. 자세한 사항은 로그를 참고하세요.");
	}
}

void CQuestMgr::ScriptEventAwardAbility(int nAbilityID, int nAbilityLV)
{
	EventNode* eventNode = new EventNode(EVENT_AWARDABILITY, nAbilityID, nAbilityLV, 0, 0, 0, 0, 0);
	if (false == CQuestMgr::GetInstance().AddEvent(eventNode))
	{
		delete eventNode;
		assert(false && "Quest 스크립트에 문제가 있습니다. 자세한 사항은 로그를 참고하세요.");
	}
}

void CQuestMgr::ScriptEventAwardItem(int nOptionType, int nLevel, int nGrade, int nBaseNum)
{
	if (Item::MAX_OPTION_BASE_NUM < nBaseNum)
	{
		ERRLOG4(g_Log, "베이스 옵션 갯수가 최대치를 초과하였습니다. Event_AwardItem(%d, %d, %d, %d)",
			nOptionType, nLevel, nGrade, nBaseNum);
		nBaseNum = Item::MAX_OPTION_BASE_NUM;
	};

	EventNode* eventNode = new EventNode(EVENT_AWARDITEM, nOptionType, nLevel, nGrade, nBaseNum, 0, 0, 0);
	if (false == CQuestMgr::GetInstance().AddEvent(eventNode))
	{
		delete eventNode;
		assert(false && "Quest 스크립트에 문제가 있습니다. 자세한 사항은 로그를 참고하세요.");
	}
}

void CQuestMgr::ScriptEventAwardItemID(int nOptionType, int nID, int nGrade, int nBaseNum)
{
	if (Item::MAX_OPTION_BASE_NUM < nBaseNum)
	{
		ERRLOG4(g_Log, "베이스 옵션 갯수가 최대치를 초과하였습니다. Event_AwardItem(%d, %d, %d, %d)",
			nOptionType, nID, nGrade, nBaseNum);
		nBaseNum = Item::MAX_OPTION_BASE_NUM;
	};

	EventNode* eventNode = new EventNode(EVENT_AWARDITEMID, nOptionType, nID, nGrade, nBaseNum, 0, 0, 0);
	if (false == CQuestMgr::GetInstance().AddEvent(eventNode))
	{
		delete eventNode;
		assert(false && "Quest 스크립트에 문제가 있습니다. 자세한 사항은 로그를 참고하세요.");
	}
}

void CQuestMgr::ScriptEventAwardItemClass(int nOptionType, int nClass, int nLevel, int nGrade, int nBaseNum)
{
	if (Item::MAX_OPTION_BASE_NUM < nBaseNum)
	{
		ERRLOG5(g_Log, "베이스 옵션 갯수가 최대치를 초과하였습니다. Event_AwardItem(%d, %d, %d, %d, %d)",
			nOptionType, nClass, nLevel, nGrade, nBaseNum);
		nBaseNum = Item::MAX_OPTION_BASE_NUM;
	};

	EventNode* eventNode = new EventNode(EVENT_AWARDITEMCLASS, nOptionType, nClass, nLevel, nGrade, (float)nBaseNum, 0, 0);
	if (false == CQuestMgr::GetInstance().AddEvent(eventNode))
	{
		delete eventNode;
		assert(false && "Quest 스크립트에 문제가 있습니다. 자세한 사항은 로그를 참고하세요.");
	}
}

void CQuestMgr::ScriptEventMove(int nZoneID, float fPosX, float fPosY, float fPosZ)
{
	EventNode* eventNode = new EventNode(EVENT_MOVE, nZoneID, 0, 0, 0, fPosX, fPosY, fPosZ);
	if (false == CQuestMgr::GetInstance().AddEvent(eventNode))
	{
		delete eventNode;
		assert(false && "Quest 스크립트에 문제가 있습니다. 자세한 사항은 로그를 참고하세요.");
	}
}

void CQuestMgr::ScriptEventTheaterMode(void)
{
	EventNode* eventNode = new EventNode(EVENT_THEATERMODE, 0, 0, 0, 0, 0, 0, 0);
	if (false == CQuestMgr::GetInstance().AddEvent(eventNode))
	{
		delete eventNode;
		assert(false && "Quest 스크립트에 문제가 있습니다. 자세한 사항은 로그를 참고하세요.");
	}
}