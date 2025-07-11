#ifndef _ITEM_LOG_H_
#define _ITEM_LOG_H_

#include <Network/Packet/PacketStruct/CharItemPacket.h>

// 전방 참조
class	CAggresiveCreature;
class   CCharacter;

namespace Item
{
    class   CItem;
    class   CExchangeContainer;
};

namespace GAMELOG
{
    struct sMinItemInfo;

    // MOVE_ITEM
	void LogMoveItem(const CCharacter& character, const TakeType takeType, 
        const sMinItemInfo& minItemInfo, const unsigned short eError);

	// SWAP_ITEM
	void LogSwapItem(const CCharacter& character, const TakeType srcTake, const TakeType dstTake, 
        Item::CItem* lpSrcItem, Item::CItem* lpDstItem, const unsigned short eError);

	// USE_ITEM
	void LogUseItem(const CCharacter& character, const Item::ItemPos usePos, 
        Item::CItem* lpUseItem, const unsigned short eError);

	// USE_LOTTERY
	void LogUseLottery(const CCharacter& character, Item::CItem* lpGetItem, const unsigned short eError);

	// SPLIT_ITEM
	void LogSplitItem(const CCharacter& character, const TakeType splitTake, 
        Item::CItem* lpPrevItem, Item::CItem* lpSplitItem, const unsigned short eError);

	// PICKUP_ITEM
	void LogPickupItem(const CCharacter& character, const Item::ItemPos pickupPos, 
        Item::CItem* lpItem, const unsigned long dwGold, const unsigned short eError);

	// DROP_ITEM
	void LogDropItem(const CCharacter& character, const Item::ItemPos dropFrom, 
        Item::CItem* lpItem, const unsigned long dwGold, const unsigned short eError);

	// BUY_ITEM, SELL_ITEM, MEDAL_BUY_ITEM,
	void LogTradeItem(const CAggresiveCreature& creature, const unsigned long dwTraderCID, 
        const unsigned long dwGold, Item::CItem* lpItem, const Item::ItemPos itemPos, 
        const unsigned char cCmd, const unsigned short eError);

	// BEFORE_EXCHANGE_ITEM, AFTER_EXCHANGE_ITEM
	void LogExchangeItem(const CCharacter& character, const unsigned long dwDstCID,
        const Item::CExchangeContainer& Exchange, const unsigned char cLogCMD);

	// INSTALL_SOCKET_ITEM
	void LogInstallSocket(const CCharacter& character, const TakeType GemAndEquip, 
        Item::CItem* lpGemItem, Item::CItem* lpEquipItem, const unsigned short eError);

    // UPGRADE_ITEM_V2
    void LogUpgradeItem(const CCharacter& character, 
        unsigned long dwCurrentGold, unsigned long dwUsedGold, const unsigned char cLastUpgradeLevel,
        Item::CItem* lpResult, Item::CItem* lpMineral, const unsigned char cValue, unsigned short eError);

    // TAKE_GOLD_V2
    void LogTakeGold(const CCharacter& character, unsigned long dwSrcGold, unsigned long dwDstGold,
        unsigned long dwMoveGold, unsigned char cSrcPos, unsigned char cDstPos, 
        unsigned char cPurpose, unsigned short eError);

    // REPAIR_ITEM
    void LogRepairItem(const CCharacter& character, Item::CItem* lpRepairedItem, 
        const unsigned long dwRepairPrice, const unsigned char cPreRepairDurability, const unsigned short eError);

    // CHANGE_WEAPON
    void LogChangeWeapon(const CCharacter& character, const unsigned char cCurrentHand);

    // CHANGE_RIDE
	void LogChangeRide(const CCharacter& character, const unsigned char cCurrentRide);

	// ILLEGAL_ITEM
	void LogIllegalItem(const unsigned long dwCID, const Item::ItemPos usePos, const Item::CItem* lpItem);

	// ILLEGAL_WARPPOS
	void LogIllegalWarpPos(const CCharacter& character, const Position &oldPos, const Position &Pos);

	// HACK_DOUBT
	void LogHackBoubt(const CCharacter& character, unsigned short wHackID, unsigned short wKindItem, Item::CItem* lpItem, unsigned short usError);

    // STALL_OPEN_CLOSE
    void LogStallOpenClose(const CAggresiveCreature& creature, const char* szStallName, const bool bOpen);

    // STALL_ENTER_LEAVE
    void LogStallEnterLeave(const CAggresiveCreature& creature, const unsigned long dwCustomerCID, const bool bEnter);
    
    // STALL_ITEM_REGISTER_REMOVE
    void LogStallRegisterRemoveItem(const CCharacter& character, const Item::CItem* lpItem, const TakeType takeType,
        const unsigned char cPktStRICMD, const unsigned short usError);

    // ITEM_ATTACH_OPTION   아이템 옵션 이식
    void LogItemAttachOption(const CCharacter& character, 
        unsigned long dwOldInvenGold, unsigned char cSelectedOptionIndex,
        const char* szSerializedOldItem, size_t nOldItemSize, 
        const char* szUseItemData, size_t nUseItemSize,
        Item::CItem* lpNewItem, unsigned short usError);

    // ITEM_COMPENSATION    아이템 보상
    void LogItemCompensation(const CCharacter& character,
        unsigned long dwOldInvenGold, Item::CItem* lpOldItem, Item::CItem* lpNewItem, unsigned short usError);
};

#endif