#ifndef _SEND_LOG_PACKET_H_
#define _SEND_LOG_PACKET_H_

// ���� ����
class CCharacter;
class CSendStream;

namespace Item
{
    class CItem;
    class CEquipment;
};

namespace SendLogPacket
{
    bool ServerLogin(CSendStream& LogSendStream, unsigned long dwServerID);

    bool DefaultInfo(CSendStream& LogSendStream, CCharacter& Character, 
        unsigned long dwGold, unsigned long dwSecondUID, unsigned long dwSecondCID, 
        unsigned char cCommand, unsigned char cFlags);

    bool ItemInfo(CSendStream& LogSendStream, CCharacter& Character, Item::CItem& Item,
        unsigned long dwGold, unsigned long dwSecondUID, unsigned long dwSecondCID, 
        unsigned char cCommand, unsigned char cFlags);
    
    bool EquipInfo(CSendStream& LogSendStream, CCharacter& Character, Item::CEquipment& Equipment,
        unsigned long dwGold, unsigned long dwSecondUID, unsigned long dwSecondCID, 
        unsigned char cCommand, unsigned char cFlags);

    /*
    �α���
    �α׾ƿ�
    ������ ����
    ������ ������
    ������ ���
    ������ �ȱ�
    ������ ��ȯ 
    ������ ��� ��ų �� ����(������ �� ����)
    ������ ����
    ���Ͽ� ���� �ڱ�
    ����Ʈ ������ ���
    ����Ʈ �������� ������ ���
    ����Ʈ ������ ����
    
    ������ ����
    ��� ���(dropitem)

    �� �α�

        /�α��ν� �κ��丮 �ݾ�
        /�α��ν� â�� �ݾ�
        /�α׾ƿ��� �κ��丮 �ݾ�
        /�α׾ƿ��� â�� �ݾ�
        /�� ��� �κ��丮�� ����
        â�� �뿩�� ����            // �뿩�� �� ������
        /����Ʈ �������� �� ����
        /������ ��� �ȱ�
        /�� ��ȯ��
        ��ų �� ������ �� �Ҹ�
        ������ ������ �� �Ҹ�
        ��� ���(givegold)
    */

    // ���� ���� - �α� Ŀ�ǵ�.     
    struct Const;

    void CharLogin(CCharacter& Character);
    void CharLogout(CCharacter& Character);
    void Pickup(CCharacter& Character, Item::CItem* lpPickupItem, unsigned long dwPickupGold);
    void Drop(CCharacter& Character, Item::CItem* lpDropItem, unsigned long dwDropGold);

    void BuyItem(CCharacter& Buyer, unsigned long dwSellerUID, unsigned dwSellerCID, 
        Item::CItem* lpBuyItem, unsigned long dwPrice);

    void SellItem(CCharacter& Seller, unsigned long dwBuyerUID, unsigned dwBuyerCID,
        Item::CItem* lpSell_Item, unsigned long dwPrice);

    void BeforeExchange(CCharacter& Character, unsigned long dwExchangerUID, unsigned long dwExchangerCID, unsigned long dwExchangeGold);
    void AfterExchange(CCharacter& Character, unsigned long dwExchangerUID, unsigned long dwExchangerCID, unsigned long dwExchangeGold);

    void ItemUse(CCharacter& Character, Item::CItem* lpUseItem, unsigned long dwUseGold);
    void ItemUpgrade(CCharacter& Character, Item::CEquipment* lpEquipment, Item::CItem* lpMineral, unsigned long dwUseGold);
    void InstallSocket(CCharacter& Character, Item::CEquipment* lpEquipment, Item::CItem* lpGems);
    void RepairItem(CCharacter& Character, Item::CEquipment* lpEquipment, 
        unsigned long dwRepairNPC_CID, unsigned long dwUsedGold);

    void GetQuestItem(CCharacter& Character, Item::CItem* lpQuestItem, 
        unsigned long dwQuestNPC_CID, unsigned long dwGetGold);

    void RemoveQuestItem(CCharacter& Character, Item::CItem* lpQuestItem, 
        unsigned long dwQuestNPC_CID, unsigned long dwRemoveGold);

    void GetQuestReward(CCharacter& Character, Item::CItem* lpQuestReward, 
        unsigned long dwQuestNPC_CID, unsigned long dwRewardGold);
    
    void AdminDropItem(CCharacter& Character, unsigned long dwAdminUID, unsigned long dwAdminCID,
        Item::CItem* lpAdminItem, unsigned long dwGiveGold);

    void RentDepositGold(CCharacter& Character, unsigned long dwRentDepositGold);
};


#endif