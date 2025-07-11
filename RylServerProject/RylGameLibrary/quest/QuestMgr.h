
#ifndef _QUEST_MGR_H_
#define _QUEST_MGR_H_

#include "QuestStruct.h"
#include <ScriptEngine/ScriptEngine.h>

class CQuestMgr : public CSingleton<CQuestMgr>
{
public:

	CQuestMgr();
	~CQuestMgr();

	bool AddQuest(Quest::QuestNode* questNode);
	bool AddPhase(Quest::PhaseNode* phaseNode);
	bool AddTrigger(Quest::TriggerNode* triggerNode);
	bool AddEvent(Quest::EventNode* eventNode);

	bool AddPendingQuest(unsigned long dwGID, unsigned short wQuestID, unsigned char cPhase, unsigned char cTrigger, unsigned char cCount, Position Pos);	

	bool SwapPendingQuest();

	void ProcessPendingQuest();

	inline void Else(void);
	inline void QuestEnd(void);
	inline void QuestCompleteSave(bool bSave);
	inline void QuestType(unsigned short wQuestType);
	inline void QuestArea(int nZone, float fX, float fY, float fZ, float fDist);
	inline void QuestSkillPointBonus(unsigned char cBonusNum);
	inline void QuestAbilityPoint(unsigned char cBonusNum);
	
	inline void QuestSetDelLinkItem(bool bDelLinkItem);
	inline unsigned char GetSkillPointBonusNum(unsigned short wQuestID);

	void SortQuests(void);
	Quest::QuestNode* GetQuestNode(unsigned short wQuestID);

	static bool LoadQuestInfo(void);

	// NPC 스크립트용 함수 목록
	static void ScriptErrorMessage(const char *msg);

	// 퀘스트 스크립트용 함수 목록
	static void ScriptQuestStartLevel(int nQuestID, int nMinLevel, int nMaxLevel, int nClass, int nCompleteQuest, int nNation, bool bNationDependent);
	static void ScriptQuestStartFame(int nQuestID, int nMinFame, int nMaxFame, int nClass, int nCompleteQuest, int nNation, bool bNationDependent);
	static void ScriptQuestType(unsigned short wQuestType, unsigned short wPartyQuest);
	static void ScriptQuestArea(int nZone, float fPosX, float fPosY, float fPosZ, float fDistance);
	static void ScriptQuestTitle(const char *strQuestTitle);
	static void ScriptQuestDesc(const char *strQuestDesc);
	static void ScriptQuestShortDesc(const char *strQuestShortDesc);
	static void ScriptQuestIcon(const char *strIconTexture, int nPosX1, int nPosY1, int nPosX2, int nPosY2);
	static void ScriptQuestCompleteSave(bool bSave);
	static void ScriptQuestLevel(const char *strQuestLevel);
	static void ScriptQuestAward(const char *strQuestAward);
	static void ScriptQuestSkillPointBonus(int nBonusNum);
	static void ScriptQuestAbilityPoint(int nBonusNum);

	static void ScriptQuestCancelItemDel(bool bDelLinkItem);
	static void ScriptAddPhase(int nZoneID, int nPhaseNumber, const char* strQuestDesc);
	static void ScriptPhaseTarget(int nType, int nPosX, int nPosY);
	static void ScriptTriggerStart(void);
	static void ScriptTriggerPuton(int nItemID, int nZoneID, float fPosX, float fPosY, float fPosZ, float fDistance);
	static void ScriptTriggerGeton(int nItemID, int nZoneID, float fPosX, float fPosY, float fPosZ, float fDistance);
	static void ScriptTriggerTalk(int nNpcID);
	static void ScriptTriggerKill(int nCount, int nMonsterID);
	static void ScriptTriggerPick(int nCount, int nItemID);
	static void ScriptTriggerFame(int nFame);
	static void ScriptTriggerLevelTalk(int nLevel, int nNPCID, const char* strElseMsg);
	static void ScriptTriggerFameTalk(int nFame, int nNPCID, const char* strElseMsg);
	static void ScriptTriggerAbilityTalk(int nAbilityID, int nAbilityLV, int nNPCID, const char* strElesMsg);


	static void ScriptElse(void);
	static void ScriptEventDisappear(int nAmount, int nItemID, int nGold);
	static void ScriptEventGet(int nAmount, int nItemID);
	static void ScriptEventSpawn(int nMonsterID, float fPosX, float fPosY, float fPosZ);
	static void ScriptEventMonsterDrop(int nAmount, int nItemID);
	static void ScriptEventAward(int nExp, int nGold, int nFame, int nMileage);
	static void ScriptEventMsgBox(const char *strMessage);
	static void ScriptEventAlarmBox(const char *strMessage);
	static void ScriptEventPhase(int nPhaseNumber);
	static void ScriptEventEnd(void);
	static void ScriptEventAddQuest(int nNpcID, int nQuestID);
	static void ScriptEventAwardAbility(int nAbilityID, int nAbilityLV);
	static void ScriptEventAwardItem(int nOptionType, int nLevel, int nGrade, int nBaseNum);
	static void ScriptEventAwardItemID(int nOptionType, int nID, int nGrade, int nBaseNum);
	static void ScriptEventAwardItemClass(int nOptionType, int nClass, int nLevel, int nGrade, int nBaseNum);
	static void ScriptEventMove(int nZoneID, float fPosX, float fPosY, float fPosZ);
	static void ScriptEventTheaterMode(void);
	static void ScriptQuestEnd();

private:

	void ClearQuest(void);
	void ClearTempNode(void);


	static CQuestMgr		ms_this;

	vector<Quest::QuestNode *>			m_lstQuest;
	map<unsigned short, unsigned char>	m_mapSkillPointBonus;

	// 퀘스트 큐.
	vector<Quest::TriggerMsg>			m_vecPendingQuest;
	vector<Quest::TriggerMsg>			m_vecSubPendingQuest;

	Quest::QuestNode*		m_lpQuestNode;
	Quest::PhaseNode*		m_lpPhaseNode;
	Quest::TriggerNode*		m_lpTriggerNode;

	bool	m_bFalseEvent;
	bool	m_bDangerousEvent;
	bool	m_bPendingQuest;
};

void CQuestMgr::Else(void)								
{ 
	m_bFalseEvent = true; 
}

void CQuestMgr::QuestEnd(void)							
{ 
	m_lpQuestNode->m_wMaxPhase = static_cast<unsigned short>(m_lpQuestNode->m_lstPhase.size()); 
	m_lpQuestNode = NULL; 
}

void CQuestMgr::QuestCompleteSave(bool bSave)			
{ 
	m_lpQuestNode->m_bSave = bSave; 
}

void CQuestMgr::QuestType(unsigned short wQuestType)	
{ 
	m_lpQuestNode->m_wQuestType = wQuestType; 
}

void CQuestMgr::QuestArea(int nZone, float fX, float fY, float fZ, float fDist)
{
	m_lpQuestNode->m_dwZoneID = nZone;
	m_lpQuestNode->m_fPosX = fX;
	m_lpQuestNode->m_fPosY = fY;
	m_lpQuestNode->m_fPosZ = fZ;
	m_lpQuestNode->m_fDistance = fDist;
}

void CQuestMgr::QuestSkillPointBonus(unsigned char cBonusNum)
{
	m_lpQuestNode->m_cSkillPointBonus = cBonusNum; 
}

void CQuestMgr::QuestAbilityPoint(unsigned char cBonusNum)
{
	m_lpQuestNode->m_cAbilityPointBonus = cBonusNum; 
}

void CQuestMgr::QuestSetDelLinkItem(bool bDelLinkItem)
{
	m_lpQuestNode->m_bDelLinkItem = bDelLinkItem;
}
#endif