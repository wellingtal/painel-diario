#ifndef _CITEM_CREATOR_H_
#define _CITEM_CREATOR_H_

#include <BaseLibrary/Thread/Lock.h>
#include <BaseLibrary/Pattern/Singleton.h>

// ���� ����
namespace Item
{
	class	CItem;
	class	CItemMgr;
    class   CItemFactory;
};


class CItemCreator
{
public:
    
    static CItemCreator& GetInstance();

    // Desc : ������ ��ũ��Ʈ �ε� �� �ʱ�ȭ ���.
    bool Initialize();

    // Desc : ������ UID ����
    void SetUID(DWORD64 dwUID); 
    DWORD64 GetUID() const;

    // Desc : ������ ����
    Item::CItem* Create(unsigned short usID);

private:

    CItemCreator();
    
    typedef CCSLock			ItemCreationLock;
    ItemCreationLock		m_ItemCreationLock;
};


#endif