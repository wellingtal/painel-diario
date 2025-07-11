
#include "stdafx.h"

#include "SendCharQuest.h"

#include <Network/Stream/SendStream.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/CharQuestPacket.h>

#include <Creature/Character/Character.h>


bool GameClientSendPacket::SendCharStartQuest(CSendStream& SendStream, unsigned long dwCharID, 
											  unsigned long dwNPCID, unsigned short wQuestID, unsigned short wError)
{
	char* lpBuffer = SendStream.GetBuffer(sizeof(PktStartQuest));
	if (NULL == lpBuffer) { return false; }

	PktStartQuest* lpPktSQAck = reinterpret_cast<PktStartQuest*>(lpBuffer);

	lpPktSQAck->m_dwCharID = dwCharID;
	lpPktSQAck->m_dwNPCID = dwNPCID;
	lpPktSQAck->m_wQuestID = wQuestID;

	return SendStream.WrapCrypt(sizeof(PktStartQuest), CmdCharStartQuest, 0, wError);
}

bool GameClientSendPacket::SendCharOperateTrigger(CSendStream& SendStream, unsigned long dwCharID, unsigned short wQuestID, unsigned char cPhase,
												  unsigned char cTrigger, unsigned char cCount, unsigned short wError)
{
	char* lpBuffer = SendStream.GetBuffer(sizeof(PktOperateTrigger));
	if (NULL == lpBuffer) { return false; }

	PktOperateTrigger* lpPktOTAck = reinterpret_cast<PktOperateTrigger*>(lpBuffer);

	lpPktOTAck->m_dwCharID = dwCharID;
	lpPktOTAck->m_wQuestID = wQuestID;
	lpPktOTAck->m_cPhase = cPhase;
	lpPktOTAck->m_cTrigger = cTrigger;

	if (PktOperateTrigger::NO_SERVER_ERR != wError)
	{
		cCount++;
	}
	lpPktOTAck->m_cCount = cCount;

	return SendStream.WrapCrypt(sizeof(PktOperateTrigger), CmdCharOperateTrigger, 0, wError);
}

bool GameClientSendPacket::SendCharQuestInfo(CSendStream& SendStream, CCharacter* lpCharacter)
{
	const int MAX_BUFFER = sizeof(PktQuestDB) + PktQuestDB::MAX_EXECUTING_QUEST * sizeof(PktQuestDB::ExecutingQuest) + 
		PktQuestDB::MAX_HISTORY_QUEST * sizeof(unsigned short);
	char szBuffer[MAX_BUFFER];

	PktQuestDB*	lpPktQuestDB = reinterpret_cast<PktQuestDB *>(szBuffer);

	lpPktQuestDB->m_dwUID = lpCharacter->GetUID(); 
	lpPktQuestDB->m_dwCID = lpCharacter->GetCID();
	lpPktQuestDB->m_wExecuteQuestSize = 0;
	lpPktQuestDB->m_wHistoryQuestSize = 0;

	Quest::ExecutingQuest* ExecutingQuest = lpCharacter->GetExecutingQuest();
	unsigned short *wHistoryQuest = lpCharacter->GetHistoryQuest();

	int nIndex = 0;

	for (nIndex = 0; nIndex < PktQuestDB::MAX_EXECUTING_QUEST; nIndex++)
	{
		if (NULL == ExecutingQuest[nIndex].m_QuestNode) 
		{ 
			break; 
		}

		PktQuestDB::ExecutingQuest* ExecuteQuest = 
			reinterpret_cast<PktQuestDB::ExecutingQuest *>(szBuffer + sizeof(PktQuestDB) + lpPktQuestDB->m_wExecuteQuestSize);

		ExecuteQuest->m_wQuestID = ExecutingQuest[nIndex].m_QuestNode->m_wQuestID;
		ExecuteQuest->m_cPhase = ExecutingQuest[nIndex].m_cPhase;
		::memcpy(ExecuteQuest->m_cTriggerCount, ExecutingQuest[nIndex].m_cTriggerCount, 
			sizeof(unsigned char) * PktQuestDB::MAX_TRIGGER);

		lpPktQuestDB->m_wExecuteQuestSize += sizeof(PktQuestDB::ExecutingQuest);;
	}

	for (nIndex = 0; nIndex < PktQuestDB::MAX_HISTORY_QUEST; nIndex++)
	{
		if (0 == wHistoryQuest[nIndex]) { break; }

		unsigned short* wHistoryQuestForPkt = 
			reinterpret_cast<unsigned short *>(szBuffer + sizeof(PktQuestDB) + 
			lpPktQuestDB->m_wExecuteQuestSize + lpPktQuestDB->m_wHistoryQuestSize);

		*wHistoryQuestForPkt = wHistoryQuest[nIndex];

		lpPktQuestDB->m_wHistoryQuestSize += sizeof(unsigned short);
	}

	return SendStream.WrapCompress(szBuffer, sizeof(PktQuestDB) + lpPktQuestDB->m_wExecuteQuestSize + lpPktQuestDB->m_wHistoryQuestSize, 
		CmdQuestDB, 0, 0);
}

bool GameClientSendPacket::SendCharEndQuest(CSendStream& SendStream, unsigned long dwCharID, unsigned short wQuestID, bool bSave)
{
	char* lpBuffer = SendStream.GetBuffer(sizeof(PktEndQuest));
	if (NULL == lpBuffer) { return false; }

	PktEndQuest* lpPktEQ = reinterpret_cast<PktEndQuest*>(lpBuffer);

	lpPktEQ->m_dwCharID = dwCharID;
	lpPktEQ->m_wQuestID = wQuestID;
	lpPktEQ->m_bSave = bSave;

	return SendStream.WrapCrypt(sizeof(PktEndQuest), CmdCharEndQuest, 0, 0);	
}

bool GameClientSendPacket::SendCharCancelQuest(CSendStream& SendStream, unsigned long dwCharID, 
											   unsigned short wQuestID, unsigned short wError)
{
	char* lpBuffer = SendStream.GetBuffer(sizeof(PktCancelQuest));
	if (NULL == lpBuffer) { return false; }

	PktCancelQuest* lpPktCQ = reinterpret_cast<PktCancelQuest*>(lpBuffer);

	lpPktCQ->m_dwCharID = dwCharID;
	lpPktCQ->m_wQuestID = wQuestID;

	return SendStream.WrapCrypt(sizeof(PktCancelQuest), CmdCharCancelQuest, 0, wError);	
}


