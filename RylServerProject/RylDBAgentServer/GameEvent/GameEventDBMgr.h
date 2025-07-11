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

		// ���� : ��¥�� �ð��� ������ ����մϴ�.
		//		�־��� ��¥ ������ �Ϸ� �� �־��� �ð� ���̿��� ����� �̷�����ϴ�.
		//
		//		ex) 2004/03/12 20:03 ~ 2004/03/15 21:10�� �� �Ⱓ ������ ��� ���� ����� ������ ���� �ƴ϶�,
		//			12�� ~ 15���� �Ϸ�(12, 13, 14, 15��)�� 20:03 ~ 21:10 ���̿��� ����� �̷�����ϴ�.
		//			��, 2004/03/13 21:00�� ����� ���������� 2004/03/13 19:00�� ����� �Ұ����� �ð��Դϴ�.

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

    // � ������ ������ ������ ������.
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