#ifndef _RYL_GAME_ITEM_FACTORY_H_
#define _RYL_GAME_ITEM_FACTORY_H_

#include <map>
#include <boost/pool/pool_alloc.hpp>
#include <Item/ItemStructure.h>
#include <Network/Packet/PacketStruct/CharAdminPacket.h>

#define DELETE_ITEM(p)  if(p) { Item::CItemFactory::GetInstance().DeleteItem(p); (p)=0; }

namespace Item
{
    // 전방 참조
    class   CItem;
    class   CItemMgr;
    struct  ItemInfo;

    class CItemOwnerInfo
    {
    public:

        CItemOwnerInfo(const char* szName, unsigned long dwUID, unsigned long dwCID);

        const char*   GetName() const { return m_szName; }
        unsigned long GetUID()  const { return m_dwUID; }
        unsigned long GetCID()  const { return m_dwCID; }

    private:

        enum 
        {
            MAX_NAME = 16 
        };

        char                m_szName[MAX_NAME];        
        unsigned long       m_dwUID;
        unsigned long       m_dwCID;
    };

    class CItemFactory
    {
    public:

        static CItemFactory& GetInstance();
        
        unsigned __int64 GetItemUID()    { return m_nCurrentUID; }
        unsigned __int64 GetNewItemUID() { return m_nCurrentUID++; }
	    void SetItemUID(unsigned __int64 nCurrentUID) { m_nCurrentUID = nCurrentUID; }

        // Desc : 아이템 시리얼이 0인 임시 아이템을 생성한다. - 아이템 시리얼을 매기지 않는다.
        CItem* CreateTempItem(const ItemInfo& itemInfo);

        // Desc : 종류로 아이템 생성(기본 능력치) - 아이템 시리얼을 매긴다.
        CItem* CreateItem(const ItemInfo& itemInfo);

        // Desc : 종류 ID로 아이템 생성(기본 능력치) - 아이템 시리얼을 매긴다.
        CItem* CreateItem(unsigned short usProtoTypeID);

        // Desc : SerializeOut된 버퍼로 아이템 생성. 아이템 시리얼 매기지 않는다!
        // In   : 버퍼, 버퍼 크기
        // Out  : (Return)성공 여부, nParseLength_InOut - 사용한 버퍼 크기
        CItem* CreateItem(const char* lpSerializedItem_In, size_t& nParseLength_InOut);

        // 아이템을 삭제한다.
        void DeleteItem(CItem* lpDeleteItem);

        // ---------------------------------------
        // 아이템 복사 체크 메커니즘 개요. (2004/08/11, by sparrowhawk)

        // 캐릭터가 로그인할때, 캐릭터 아이템을 몽땅 맵에 때려박는다.
        // 때려박다가, 안박아지면 DB에 남긴다. (이미 있던 아이템, 넣을때 걸린 아이템 둘 다..)
        // 나중에 왜 겹치는 ID가 생겼는지 로그조사해서 회수하던지, 버그를 고치던지 한다..

        // TODO : Map을 boost pool을 사용하고 싶지만, 클라이언트는 boost라이브러리를 사용하지 않는다.
        // 나중에 클라이언트쪽에 boost pool라이브러리를 추가하자..

        // 아이템 UID맵에 아이템 추가. 리턴값이 NULL이면 추가 성공. 아니면 추가 실패(이미 있음)
        CItemOwnerInfo* AddItemMap(unsigned __int64 dwItemUID, const CItemOwnerInfo& itemOwnerInfo);
        void RemoveItemMap(unsigned __int64 dwItemUID);
        void ClearItemMap();
        
    private:

        CItemFactory();
        ~CItemFactory();

        typedef std::map<unsigned __int64, CItemOwnerInfo, std::less<unsigned __int64>,
            boost::fast_pool_allocator<std::pair<unsigned __int64, CItemOwnerInfo> > > ItemSerialMap;
        
        unsigned __int64    m_nCurrentUID;
        ItemSerialMap       m_ItemSerialMap;
    };

    Item::CItem* CreateItemFromGiveInfo(Item::CItemMgr& itemMgr, 
        CItemFactory& itemFactory, GiveItemInfo& giveItemInfo, Item::ItemPos itemPos);

    bool CreateItemDataFromGiveInfo(Item::CItemMgr& itemMgr, GiveItemInfo& giveItemInfo, 
        char* szData, unsigned long& dwDataSize_InOut, unsigned __int64 dwItemUID, Item::ItemPos itemPos);
};

#endif