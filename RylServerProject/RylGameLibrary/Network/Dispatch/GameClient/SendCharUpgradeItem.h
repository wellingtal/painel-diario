#ifndef _SEND_CHAR_UPGRADE_ITEM_H_
#define _SEND_CHAR_UPGRADE_ITEM_H_

#include <DB/DBdefine.h>
#include <Item/Item.h>


// ���� ����
class   CSendStream;

namespace GameClientSendPacket
{
	// ������ �ν��� ����
	bool SendCharInstallSocket(CSendStream& SendStream, unsigned long dwCharID, 
		Item::ItemPos EquipPos, Item::ItemPos GemPos, Item::CEquipment* lpEquipment, unsigned short wError);

	// ������ �ռ�
	bool SendCharItemChemical(CSendStream& SendStream, unsigned long dwCharID, Item::ItemPos PickkingPos, Item::ItemPos TargetPos, 
		unsigned short wPickkingID, unsigned char cPickkingNum, Item::CItem* lpResultItem, unsigned short wError);

	// ������ ���׷��̵�
    bool SendCharUpgradeItem(CSendStream& SendStream, unsigned long dwCharID, unsigned long dwCurrentGold, 
        Item::CItem* lpItem, unsigned char cCurrentMineralNum, unsigned char cValue, unsigned short wError);

	// ������ �ɼ� �̽�
	bool SendCharItemOptionGraft(CSendStream& SendStream, unsigned long dwCharID, unsigned long dwCurrentGold, 
		Item::CItem* lpResultItem, unsigned short wError);

	// ������ ���� �Ǹ�
	bool SendCharItemCompensation(CSendStream& SendStream, unsigned long dwCharID, unsigned short wError);

	// ������ �� ��ġ/����.
	bool SendCharRuneInstallSocket(CSendStream& SendStream, unsigned long dwCharID, Item::ItemPos EquipPos, Item::ItemPos RunePos, 
		Item::CEquipment* lpEquipment, unsigned char cType, unsigned short wError);
};

#endif