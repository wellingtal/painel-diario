#ifndef _SEND_CHAR_ITEM_H_
#define _SEND_CHAR_ITEM_H_

#include <Map/FieldMap/Cell.h>
#include <Network/Packet/PacketStruct/CharItemPacket.h>


// 전방 참조
class   CSendStream;
class	CCharacter;

namespace Item
{
    class   CItem;
}

namespace GameClientSendPacket
{
    // 아이템 이동
    bool SendCharTakeItem(CSendStream& SendStream, unsigned long dwCharID, 
        TakeType takeType, unsigned short usError);

    // 아이템 스왑
    bool SendCharSwapItem(CSendStream& SendStream, unsigned long dwCharID, 
        TakeType takeSrc, TakeType takeDst, unsigned short usError);

    // 아이템 수리
    bool SendCharRepairItem(CSendStream& SendStream, unsigned long dwCharID, 
        unsigned long dwRepairGold, Item::ItemPos itemPos, unsigned short usError);

	// 아이템 수리 (장비하고 있는 아이템 모두 수리)
	bool SendCharRepairAllItem(CSendStream& SendStream, unsigned long dwCharID, 
		unsigned long dwRepairGold, unsigned short usError);

    // 아이템 사용
    bool SendCharUseItem(CSendStream& SendStream, unsigned long dwSender, unsigned long dwReceiver,
        Item::ItemPos itemPos, unsigned char cRemainItemNum, unsigned short usError);

    // 캐쉬 아이템 사용
    bool SendCharUseCashItem(CSendStream& SendStream, unsigned long dwSender, unsigned long dwReceiver,
        Item::ItemPos itemPos, unsigned char cRemainItemNum, unsigned short usError);

    // 아이템 팔고 사기
    bool SendCharTradeItem(CSendStream& SendStream, CCharacter* lpCharacter, unsigned long dwNPCID, Item::ItemPos CouponPos, 
        Item::CItem* lpItem, Item::ItemPos itemPos, unsigned char cNum, unsigned short usError);

	// NPC 장비 상점 정보
	bool SendCharEquipShopInfo(CSendStream& SendStream, unsigned long dwCID, unsigned long dwNPCID, unsigned long dwTime, 
		unsigned char cRace, unsigned char cTabPage, unsigned char cNum, 
		unsigned short* aryItemID, unsigned char* aryItemGrade, unsigned short usError);

    // 아이템 집기
    bool SendCharPickUp(CSendStream& SendStream, unsigned long dwCID, unsigned __int64 nObjectID,
        unsigned long dwGold, Item::CItem* lpItem, Item::ItemPos dstPos, unsigned char cNum, unsigned short usError);

	// 오토 루팅
	bool SendCharAutoRouting(CSendStream& SendStream, unsigned long dwCID, unsigned __int64 nObjectID, 
		unsigned short wItemID, unsigned char cNum, unsigned char cCmd);
											   
	// 아이템 버리기
    bool SendCharPullDown(CSendStream& SendStream, unsigned long dwCID, Item::ItemPos itemPos, 
        CCell::ItemInfo& itemInfo, unsigned short usError);

	// 아이템 사라지기
	bool SendCharDisappearItem(CSendStream& SendStream, unsigned long dwCID, Item::ItemPos itemPos, 
		unsigned char cNum, unsigned char cCmd, unsigned short usError);

    // 아이템 나누기
    bool SendCharSplitItem(CSendStream& SendStream, unsigned long dwCID, 
        Item::CItem* lpSplitItem, TakeType takeType, unsigned short usError);

    // 돈 옮기기
    bool SendCharTakeGold(CSendStream& SendStream, unsigned long dwCID, unsigned long dwGold, 
        unsigned char cSrcPos, unsigned char cDstPos, unsigned short usError);

    // 교환 아이템 정보 주기
    bool SendCharExchangeItem(CSendStream& SendStream, unsigned long dwOwnerCID, unsigned long dwGold, 
        Item::CItem* lpItem, Item::ItemPos itemPos, bool bStack, bool bRemove);

    // 창고 업데이트 커맨드.
    bool SendCharDepositCmd(CSendStream& SendStream, unsigned char cCmd, 
        char* szData, size_t nDataLength, unsigned short usError);

    // 창고 업데이트(패스워드)
    bool SendCharDepositPasswordToDBAgent(CSendStream& SendStream, unsigned long dwUID, 
        const char* szPassword, size_t nPasswordLen);

    // 창고 업데이트(Gold)
    bool SendCharDepositGoldToDBAgent(CSendStream& SendStream, unsigned long dwUID, unsigned long dwGold);

	// 복권 결과
	bool SendCharLotteryResult(CSendStream& SendStream, unsigned long CID, 
		Item::ItemPos itemPos, Item::CItem* lpItem, unsigned short usError);
};

#endif