#ifndef _RYL_GAME_LIBRARY_GAME_EVENT_MGR_H_
#define _RYL_GAME_LIBRARY_GAME_EVENT_MGR_H_

#include <list>
#include "./LotteryEvent/LotteryEvent.h"


class CGameEventMgr
{
public:

    typedef std::list<unsigned short> DropEventItemList;

    static CGameEventMgr& GetInstance();

	bool Initialize(const char* szLotteryFileName = NULL);

	CLotteryEvent&	GetLotteryEvent(void) { return m_LotteryEvent; }

	inline void PushDropEventItem(unsigned short usItemID);
	inline unsigned short PopDropEventItem(void);

private:

    CGameEventMgr();
    virtual ~CGameEventMgr();

	CLotteryEvent	    m_LotteryEvent;
    DropEventItemList   m_lstDropEventItem;
};


inline void CGameEventMgr::PushDropEventItem(unsigned short usItemID)
{
	m_lstDropEventItem.push_back(usItemID);
}

inline unsigned short CGameEventMgr::PopDropEventItem(void)
{
	unsigned short usItemID = 0;

	if (false == m_lstDropEventItem.empty())
	{
		usItemID = m_lstDropEventItem.front();
		m_lstDropEventItem.pop_front();
	}

	return usItemID;
}


#endif