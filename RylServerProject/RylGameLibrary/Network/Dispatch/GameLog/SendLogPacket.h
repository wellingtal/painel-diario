#ifndef _SEND_LOG_PACKET_H_
#define _SEND_LOG_PACKET_H_

// 전방 참조
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
    로그인
    로그아웃
    아이템 집기
    아이템 버리기
    아이템 사기
    아이템 팔기
    아이템 교환 
    아이템 사용 스킬 락 해제(망각의 돌 제거)
    아이템 제련
    소켓에 보석 박기
    퀘스트 아이템 얻기
    퀘스트 보상으로 아이템 얻기
    퀘스트 아이템 제거
    
    아이템 수리
    운영자 명령(dropitem)

    돈 로그

        /로그인시 인벤토리 금액
        /로그인시 창고 금액
        /로그아웃시 인벤토리 금액
        /로그아웃시 창고 금액
        /돈 집어서 인벤토리에 놓기
        창고 대여료 지불            // 대여료 및 보관료
        /퀘스트 보상으로 돈 얻음
        /아이템 사고 팔기
        /돈 교환시
        스킬 락 해제시 돈 소모
        아이템 수리시 돈 소모
        운영자 명령(givegold)
    */

    // 전방 참조 - 로그 커맨드.     
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