#ifndef _PARSE_CHAR_QUEST_H_
#define _PARSE_CHAR_QUEST_H_


// ���� ����
struct  PktBase;
class   CGameClientDispatch;

namespace GameClientParsePacket
{
	bool ParseCharStartQuest(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);			// ����Ʈ ����
	bool ParseCharOperateTrigger(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);		// Ʈ���� �ߵ�
	bool ParseCharCancelQuest(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);			// ����Ʈ ���
};


#endif