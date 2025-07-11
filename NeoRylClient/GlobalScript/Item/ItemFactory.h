#ifndef _RYL_GAME_ITEM_FACTORY_H_
#define _RYL_GAME_ITEM_FACTORY_H_

#include <map>

#define DELETE_ITEM(p)  if(p) { Item::CItemFactory::GetInstance().DeleteItem(p); (p)=0; }

namespace Item
{
    // ���� ����
    class   CItem;
    struct  ItemInfo;

    class CItemFactory
    {
    public:

        static CItemFactory& GetInstance();
        
        unsigned __int64 GetItemUID() { return m_nCurrentUID; }
	    void SetItemUID(unsigned __int64 nCurrentUID) { m_nCurrentUID = nCurrentUID; }

        // Desc : ������ �ø����� 0�� �ӽ� �������� �����Ѵ�. - ������ �ø����� �ű��� �ʴ´�.
        CItem* CreateTempItem(const ItemInfo& itemInfo);

        // Desc : ������ ������ ����(�⺻ �ɷ�ġ) - ������ �ø����� �ű��.
        CItem* CreateItem(const ItemInfo& itemInfo);

        // Desc : ���� ID�� ������ ����(�⺻ �ɷ�ġ) - ������ �ø����� �ű��.
        CItem* CreateItem(unsigned short usProtoTypeID);

        // Desc : SerializeOut�� ���۷� ������ ����.
        // In   : ����, ���� ũ��
        // Out  : (Return)���� ����, nParseLength_InOut - ����� ���� ũ��
        CItem* CreateItem(const char* lpSerializedItem_In, size_t& nParseLength_InOut);

        // �������� �����Ѵ�.
        void DeleteItem(CItem* lpDeleteItem);

    private:

        CItemFactory();
        ~CItemFactory();

        unsigned __int64    m_nCurrentUID;
    };


};

#endif