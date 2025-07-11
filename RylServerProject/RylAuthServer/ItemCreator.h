#ifndef _CITEM_CREATOR_H_
#define _CITEM_CREATOR_H_

#include <BaseLibrary/Thread/Lock.h>
#include <BaseLibrary/Pattern/Singleton.h>

// 전방 참조
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

    // Desc : 아이템 스크립트 로딩 및 초기화 등등.
    bool Initialize();

    // Desc : 아이템 UID 세팅
    void SetUID(DWORD64 dwUID); 
    DWORD64 GetUID() const;

    // Desc : 아이템 생성
    Item::CItem* Create(unsigned short usID);

private:

    CItemCreator();
    
    typedef CCSLock			ItemCreationLock;
    ItemCreationLock		m_ItemCreationLock;
};


#endif