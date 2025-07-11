#ifndef _RYL_GAME_ITEM_FACTORY_H_
#define _RYL_GAME_ITEM_FACTORY_H_

#include <map>
#include <boost/pool/pool_alloc.hpp>
#include <Item/ItemStructure.h>
#include <Network/Packet/PacketStruct/CharAdminPacket.h>

#define DELETE_ITEM(p)  if(p) { Item::CItemFactory::GetInstance().DeleteItem(p); (p)=0; }

namespace Item
{
    // ���� ����
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

        // Desc : ������ �ø����� 0�� �ӽ� �������� �����Ѵ�. - ������ �ø����� �ű��� �ʴ´�.
        CItem* CreateTempItem(const ItemInfo& itemInfo);

        // Desc : ������ ������ ����(�⺻ �ɷ�ġ) - ������ �ø����� �ű��.
        CItem* CreateItem(const ItemInfo& itemInfo);

        // Desc : ���� ID�� ������ ����(�⺻ �ɷ�ġ) - ������ �ø����� �ű��.
        CItem* CreateItem(unsigned short usProtoTypeID);

        // Desc : SerializeOut�� ���۷� ������ ����. ������ �ø��� �ű��� �ʴ´�!
        // In   : ����, ���� ũ��
        // Out  : (Return)���� ����, nParseLength_InOut - ����� ���� ũ��
        CItem* CreateItem(const char* lpSerializedItem_In, size_t& nParseLength_InOut);

        // �������� �����Ѵ�.
        void DeleteItem(CItem* lpDeleteItem);

        // ---------------------------------------
        // ������ ���� üũ ��Ŀ���� ����. (2004/08/11, by sparrowhawk)

        // ĳ���Ͱ� �α����Ҷ�, ĳ���� �������� ���� �ʿ� �����ڴ´�.
        // �����ڴٰ�, �ȹھ����� DB�� �����. (�̹� �ִ� ������, ������ �ɸ� ������ �� ��..)
        // ���߿� �� ��ġ�� ID�� ������� �α������ؼ� ȸ���ϴ���, ���׸� ��ġ���� �Ѵ�..

        // TODO : Map�� boost pool�� ����ϰ� ������, Ŭ���̾�Ʈ�� boost���̺귯���� ������� �ʴ´�.
        // ���߿� Ŭ���̾�Ʈ�ʿ� boost pool���̺귯���� �߰�����..

        // ������ UID�ʿ� ������ �߰�. ���ϰ��� NULL�̸� �߰� ����. �ƴϸ� �߰� ����(�̹� ����)
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