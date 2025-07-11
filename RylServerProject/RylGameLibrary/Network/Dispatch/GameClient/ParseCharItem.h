#ifndef _PARSE_CHAR_ITEM_H_
#define _PARSE_CHAR_ITEM_H_

// ���� ����
struct PktBase;
struct PktTr;

namespace Item
{
	struct ItemPos;
	class CItem;
};

class CGameClientDispatch;

namespace GameClientParsePacket
{
    bool ParseCharTakeItem(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);		// ������ �̵� �� ����
    bool ParseCharSwapItem(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);		// ������ ����
    bool ParseCharRepairItem(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);		// ������ ����
	bool ParseCharRepairAllItem(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);	// ������ ���� (��� ������ ��� ����)
    bool ParseCharUseItem(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);			// ������ ���
    bool ParseCharUseCashItem(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);		// ĳ�� ������ ���
	

	bool ParseCharTradeItem(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);		// ������ �ŷ�
	bool ParseCharEquipShopInfo(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);	// NPC ��� ���� ����

	bool ParseCharPickUp(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);			// ������ ����
    bool ParseCharPullDown(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);		// ������ ������
    bool ParseCharSplitItem(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);		// ������ ������

	bool ParseCharTakeGold(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);		// �� �ٷ��
    bool ParseCharDepositCmd(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);		// â�� ���� ��Ŷ��
	bool ParseCharAutoRouting(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);		// ���� ����
};


#endif