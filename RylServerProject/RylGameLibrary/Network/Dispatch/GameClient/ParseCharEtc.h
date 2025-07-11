#ifndef _PARSE_CHAR_ETC_H_
#define _PARSE_CHAR_ETC_H_


// ���� ����
struct  PktBase;
class   CGameClientDispatch;

namespace GameClientParsePacket
{
    bool ParseCharSuicide(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);				// ĳ���� �ڻ�	
    bool ParseCharBindPosition(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);		// ĳ���� ���ε� ������

    bool ParseCharQuickSlotMove(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);		// ĳ���� �� ���� �̵�
    bool ParseCharControlOption(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);		// ĳ���� �ɼ� ����
	bool ParseCharAuthorizePanel(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);		// ���� �ǳڿ� ���� ����

	bool ParseCharFameInfo(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);			// ĳ���� ��(���嵵) ���� ��û
	bool ParseCharNameCheck(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);			// hz added for new packet
	bool ParseGetCharInfo(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);
	bool ParseCharRankingInfo(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);			// ��ŷ ����

    bool ParseCastObjectInfo(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);          // ������Ʈ ������ ��Ŷ ����
};


#endif