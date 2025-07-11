#ifndef _RYL_DBAGENT_GIVE_ITEM_MGR_H_
#define _RYL_DBAGENT_GIVE_ITEM_MGR_H_

#include <list>
#include <ctime>
#include <boost/pool/pool_alloc.hpp>

#include <DB/OleDB.h>
#include <Network/Packet/PacketStruct/CharAdminPacket.h>

#ifdef AUTH_MY
#include "NFAuthClient/AuthClient.h"
#endif

// forward decl.
class CDBComponent;


// 특정 캐릭터의 임시 인벤에 아이템을 넣어 주는 클래스이다.
class CGiveItemMgr
{
public:

    static CGiveItemMgr& GetInstance();

	void SetDBComponent(CDBComponent& DBComponent);
	void CheckGiveItem();

    void ReadGiveInfo();
    bool UpdateItemSerial(unsigned long dwCreateID, unsigned __int64 dwItemSerial);

#ifdef AUTH_MY
	bool UpdateBuyItemInfo();
#endif

private:

    // first  : 줄 아이템 정보
    // second : 네트워크 전송 여부 (마지막 전송 시간을 기록한다. 0이면 전송 안 한 것임)
    typedef std::pair<GiveItemInfo, unsigned long> GiveItem;

    typedef std::map<unsigned long, GiveItem, std::less<unsigned long>,
        boost::fast_pool_allocator< std::pair<unsigned long, GiveItem> > >  GiveItemMap;

    CGiveItemMgr();
    ~CGiveItemMgr();

    bool UpdateItemSerial(GiveItemMap::iterator pos, 
        GiveItemMap::iterator& NextPos, unsigned __int64 dwItemSerial);

    bool SetItem(char* szData, unsigned long& dwDataSize_InOut, 
        unsigned long dwMaxDataSize, GiveItemInfo& giveItemInfo, 
        unsigned long dwMaxItemNum, unsigned __int64& dwItemUID);

    CDBComponent*       m_lpDBComponent;
    GiveItemMap         m_GiveItemMap;
    OleDB::PARAM_INFO   m_UpdateParamInfo;

#ifdef AUTH_MY
	std::list<NFITEMINFO>	m_NFItemInfo;
#endif

};


#endif