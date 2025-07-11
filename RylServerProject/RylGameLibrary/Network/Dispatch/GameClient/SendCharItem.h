#ifndef _SEND_CHAR_ITEM_H_
#define _SEND_CHAR_ITEM_H_

#include <Map/FieldMap/Cell.h>
#include <Network/Packet/PacketStruct/CharItemPacket.h>


// ���� ����
class   CSendStream;
class	CCharacter;

namespace Item
{
    class   CItem;
}

namespace GameClientSendPacket
{
    // ������ �̵�
    bool SendCharTakeItem(CSendStream& SendStream, unsigned long dwCharID, 
        TakeType takeType, unsigned short usError);

    // ������ ����
    bool SendCharSwapItem(CSendStream& SendStream, unsigned long dwCharID, 
        TakeType takeSrc, TakeType takeDst, unsigned short usError);

    // ������ ����
    bool SendCharRepairItem(CSendStream& SendStream, unsigned long dwCharID, 
        unsigned long dwRepairGold, Item::ItemPos itemPos, unsigned short usError);

	// ������ ���� (����ϰ� �ִ� ������ ��� ����)
	bool SendCharRepairAllItem(CSendStream& SendStream, unsigned long dwCharID, 
		unsigned long dwRepairGold, unsigned short usError);

    // ������ ���
    bool SendCharUseItem(CSendStream& SendStream, unsigned long dwSender, unsigned long dwReceiver,
        Item::ItemPos itemPos, unsigned char cRemainItemNum, unsigned short usError);

    // ĳ�� ������ ���
    bool SendCharUseCashItem(CSendStream& SendStream, unsigned long dwSender, unsigned long dwReceiver,
        Item::ItemPos itemPos, unsigned char cRemainItemNum, unsigned short usError);

    // ������ �Ȱ� ���
    bool SendCharTradeItem(CSendStream& SendStream, CCharacter* lpCharacter, unsigned long dwNPCID, Item::ItemPos CouponPos, 
        Item::CItem* lpItem, Item::ItemPos itemPos, unsigned char cNum, unsigned short usError);

	// NPC ��� ���� ����
	bool SendCharEquipShopInfo(CSendStream& SendStream, unsigned long dwCID, unsigned long dwNPCID, unsigned long dwTime, 
		unsigned char cRace, unsigned char cTabPage, unsigned char cNum, 
		unsigned short* aryItemID, unsigned char* aryItemGrade, unsigned short usError);

    // ������ ����
    bool SendCharPickUp(CSendStream& SendStream, unsigned long dwCID, unsigned __int64 nObjectID,
        unsigned long dwGold, Item::CItem* lpItem, Item::ItemPos dstPos, unsigned char cNum, unsigned short usError);

	// ���� ����
	bool SendCharAutoRouting(CSendStream& SendStream, unsigned long dwCID, unsigned __int64 nObjectID, 
		unsigned short wItemID, unsigned char cNum, unsigned char cCmd);
											   
	// ������ ������
    bool SendCharPullDown(CSendStream& SendStream, unsigned long dwCID, Item::ItemPos itemPos, 
        CCell::ItemInfo& itemInfo, unsigned short usError);

	// ������ �������
	bool SendCharDisappearItem(CSendStream& SendStream, unsigned long dwCID, Item::ItemPos itemPos, 
		unsigned char cNum, unsigned char cCmd, unsigned short usError);

    // ������ ������
    bool SendCharSplitItem(CSendStream& SendStream, unsigned long dwCID, 
        Item::CItem* lpSplitItem, TakeType takeType, unsigned short usError);

    // �� �ű��
    bool SendCharTakeGold(CSendStream& SendStream, unsigned long dwCID, unsigned long dwGold, 
        unsigned char cSrcPos, unsigned char cDstPos, unsigned short usError);

    // ��ȯ ������ ���� �ֱ�
    bool SendCharExchangeItem(CSendStream& SendStream, unsigned long dwOwnerCID, unsigned long dwGold, 
        Item::CItem* lpItem, Item::ItemPos itemPos, bool bStack, bool bRemove);

    // â�� ������Ʈ Ŀ�ǵ�.
    bool SendCharDepositCmd(CSendStream& SendStream, unsigned char cCmd, 
        char* szData, size_t nDataLength, unsigned short usError);

    // â�� ������Ʈ(�н�����)
    bool SendCharDepositPasswordToDBAgent(CSendStream& SendStream, unsigned long dwUID, 
        const char* szPassword, size_t nPasswordLen);

    // â�� ������Ʈ(Gold)
    bool SendCharDepositGoldToDBAgent(CSendStream& SendStream, unsigned long dwUID, unsigned long dwGold);

	// ���� ���
	bool SendCharLotteryResult(CSendStream& SendStream, unsigned long CID, 
		Item::ItemPos itemPos, Item::CItem* lpItem, unsigned short usError);
};

#endif