
#include "stdafx.h"

#include "QuestMgr.h"
#include "QuestStruct.h"

using namespace Quest;

ExecutingQuest::ExecutingQuest() 
:	m_cPhase(0), m_QuestNode(NULL) 
{ 
	std::fill_n(m_cTriggerCount, int(MAX_TRIGGER), 0);
}

ExecutingQuest::ExecutingQuest(unsigned short wQuestID, unsigned char cPhase, unsigned char* aryTriggerCount) 
:	m_cPhase(cPhase)
{
	std::copy(&(aryTriggerCount[0]), &(aryTriggerCount[MAX_TRIGGER]), m_cTriggerCount);
	m_QuestNode = CQuestMgr::GetInstance().GetQuestNode(wQuestID);
}
