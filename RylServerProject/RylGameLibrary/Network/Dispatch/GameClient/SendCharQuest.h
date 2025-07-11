#ifndef _SEND_CHAR_QUEST_H_
#define _SEND_CHAR_QUEST_H_


// 전방 참조
class CSendStream;
class CCharacter;

namespace GameClientSendPacket
{
	// 퀘스트 시작
	bool SendCharStartQuest(CSendStream& SendStream, unsigned long dwCharID, 
		unsigned long dwNPCID, unsigned short wQuestID, unsigned short wError);

	// 트리거 발동
	bool SendCharOperateTrigger(CSendStream& SendStream, unsigned long dwCharID, 
		unsigned short wQuestID, unsigned char cPhase, unsigned char cTrigger, unsigned char cCount, unsigned short wError);

	// 퀘스트 정보
	bool SendCharQuestInfo(CSendStream& SendStream, CCharacter* lpCharacter);

	// 퀘스트 종료
	bool SendCharEndQuest(CSendStream& SendStream, unsigned long dwCharID, unsigned short wQuestID, bool bSave);

	// 퀘스트 취소
	bool SendCharCancelQuest(CSendStream& SendStream, unsigned long dwCharID, unsigned short wQuestID, unsigned short wError);
};


#endif