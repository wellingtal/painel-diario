#ifndef _SEND_CHAR_QUEST_H_
#define _SEND_CHAR_QUEST_H_


// ���� ����
class CSendStream;
class CCharacter;

namespace GameClientSendPacket
{
	// ����Ʈ ����
	bool SendCharStartQuest(CSendStream& SendStream, unsigned long dwCharID, 
		unsigned long dwNPCID, unsigned short wQuestID, unsigned short wError);

	// Ʈ���� �ߵ�
	bool SendCharOperateTrigger(CSendStream& SendStream, unsigned long dwCharID, 
		unsigned short wQuestID, unsigned char cPhase, unsigned char cTrigger, unsigned char cCount, unsigned short wError);

	// ����Ʈ ����
	bool SendCharQuestInfo(CSendStream& SendStream, CCharacter* lpCharacter);

	// ����Ʈ ����
	bool SendCharEndQuest(CSendStream& SendStream, unsigned long dwCharID, unsigned short wQuestID, bool bSave);

	// ����Ʈ ���
	bool SendCharCancelQuest(CSendStream& SendStream, unsigned long dwCharID, unsigned short wQuestID, unsigned short wError);
};


#endif