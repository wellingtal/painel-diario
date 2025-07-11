#ifndef _SEND_CHAR_UPGRADE_ITEM_H_
#define _SEND_CHAR_UPGRADE_ITEM_H_

#include <DB/DBdefine.h>
#include <Item/Item.h>


// 전방 참조
class   CSendStream;

namespace GameClientSendPacket
{
	// 아이템 인스톨 소켓
	bool SendCharInstallSocket(CSendStream& SendStream, unsigned long dwCharID, 
		Item::ItemPos EquipPos, Item::ItemPos GemPos, Item::CEquipment* lpEquipment, unsigned short wError);

	// 아이템 합성
	bool SendCharItemChemical(CSendStream& SendStream, unsigned long dwCharID, Item::ItemPos PickkingPos, Item::ItemPos TargetPos, 
		unsigned short wPickkingID, unsigned char cPickkingNum, Item::CItem* lpResultItem, unsigned short wError);

	// 아이템 업그레이드
    bool SendCharUpgradeItem(CSendStream& SendStream, unsigned long dwCharID, unsigned long dwCurrentGold, 
        Item::CItem* lpItem, unsigned char cCurrentMineralNum, unsigned char cValue, unsigned short wError);

	// 아이템 옵션 이식
	bool SendCharItemOptionGraft(CSendStream& SendStream, unsigned long dwCharID, unsigned long dwCurrentGold, 
		Item::CItem* lpResultItem, unsigned short wError);

	// 아이템 보상 판매
	bool SendCharItemCompensation(CSendStream& SendStream, unsigned long dwCharID, unsigned short wError);

	// 아이템 룬 설치/제거.
	bool SendCharRuneInstallSocket(CSendStream& SendStream, unsigned long dwCharID, Item::ItemPos EquipPos, Item::ItemPos RunePos, 
		Item::CEquipment* lpEquipment, unsigned char cType, unsigned short wError);
};

#endif