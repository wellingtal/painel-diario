#ifndef _RYL_DBAGENT_GAME_EVENT_MGR_H_
#define _RYL_DBAGENT_GAME_EVENT_MGR_H_

#include <list>
#include <ctime>
#include <boost/pool/pool_alloc.hpp>

// forward decl.
class CDBComponent;

class CGameEventDBMgr
{
public:

	struct LimitDropItem
	{
		unsigned short	m_wItemID;

		unsigned short	m_wNowNum;
		unsigned short	m_wMaxNum;

		// 주의 : 날짜와 시간은 별개로 사용합니다.
		//		주어진 날짜 사이의 하루 중 주어진 시간 사이에만 드랍이 이루어집니다.
		//
		//		ex) 2004/03/12 20:03 ~ 2004/03/15 21:10은 이 기간 사이의 어느 때나 드랍이 가능한 것이 아니라,
		//			12일 ~ 15일중 하루(12, 13, 14, 15일)의 20:03 ~ 21:10 사이에만 드랍이 이루어집니다.
		//			즉, 2004/03/13 21:00는 드랍이 가능하지만 2004/03/13 19:00는 드랍이 불가능한 시각입니다.

		tm				m_StartTime;
		tm				m_EndTime;
		tm				m_NextDropTime;
		
		LimitDropItem();
		LimitDropItem(unsigned short wItemID, unsigned short wNowNum, 
            unsigned short wMaxNum, tm StartTime, tm EndTime);
	};

    typedef std::list<LimitDropItem, boost::fast_pool_allocator<LimitDropItem> > LimitDropItemList;

    static CGameEventDBMgr& GetInstance();

	bool Initialize(CDBComponent& DBComponent);
	void CheckGameEvent();
	bool InsertDropItem(unsigned short wItemID, unsigned short wMaxNum, tm StartTime, tm EndTime);
    bool EraseDropItem(unsigned short wItemID);

    // 운영 서버로 아이템 정보를 보낸다.
    void SendDropItemInfo();

private:

    CGameEventDBMgr();
    ~CGameEventDBMgr();

	void UpdateTimeDropItemEvent(LimitDropItem& ItemInfo);
	bool SendDropItemEvent(unsigned short wItemID);

    LimitDropItemList   m_lstItem;
    CDBComponent*       m_lpDBComponent;
};


#endif