#ifndef _PARSE_CHAR_QUEST_H_
#define _PARSE_CHAR_QUEST_H_


// 전방 참조
struct  PktBase;
class   CGameClientDispatch;

namespace GameClientParsePacket
{
	bool ParseCharStartQuest(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);			// 퀘스트 시작
	bool ParseCharOperateTrigger(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);		// 트리거 발동
	bool ParseCharCancelQuest(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);			// 퀘스트 취소
};


#endif