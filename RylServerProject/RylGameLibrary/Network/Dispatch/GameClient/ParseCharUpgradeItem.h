
#ifndef _PARSE_CHAR_UPGRADE_ITEM_H_
#define _PARSE_CHAR_UPGRADE_ITEM_H_


// ���� ����
struct  PktBase;
class   CGameClientDispatch;

namespace GameClientParsePacket
{
    bool ParseCharInstallSocket(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);		// ������ �ν��� ����
	bool ParseCharItemChemical(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);		// ������ �ռ�
    bool ParseCharUpgradeItem(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);			// ������ ���׷��̵�
	bool ParseCharItemOptionGraft(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);		// ������ �ɼ� �̽�
	bool ParseCharItemCompensation(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);	// ������ ���� �Ǹ�
	bool ParseCharInstallRuneSocket(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);	// �� �����ۼ�ġ, ����.
};


#endif