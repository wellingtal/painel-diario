#include "stdafx.h"
#include "GameEventDBMgr.h"

#include <DB/DBComponent.h>
#include <DB/GameDBComponent.h>

#include <Utility/Math/Math.h>
#include <Utility/Setup/ServerSetup.h>
#include <Network/Dispatch/GameDispatch.h>
#include <Network/Stream/SendStream.h>

#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>
#include <Network/Packet/PacketStruct/GameEventPacket.h>
#include <Network/SendPacket/SendAdminTool.h>

#include <Log/ServerLog.h>

#pragma pack(1)

struct LimitDropItemDB
{
    unsigned long	m_dwItemIndex;		// 아이템 아이디
    unsigned long	m_dwItemAmount;		// 현재 드랍 수량
    unsigned long	m_dwMaxAmount;		// 최대 드랍 수량
    TIME			m_StartTime;		// 드랍 시작 시각
    TIME			m_EndTime;			// 드랍 종료 시각
};

#pragma pack()




CGameEventDBMgr::LimitDropItem::LimitDropItem()
:	m_wItemID(0), m_wNowNum(0), m_wMaxNum(0)
{ 
    ZeroMemory(&m_StartTime, sizeof(tm));
    ZeroMemory(&m_EndTime, sizeof(tm));
    ZeroMemory(&m_NextDropTime, sizeof(tm));
}

CGameEventDBMgr::LimitDropItem::LimitDropItem(unsigned short wItemID, unsigned short wNowNum, 
                                            unsigned short wMaxNum, tm StartTime, tm EndTime)
:	m_wItemID(wItemID), m_wNowNum(wNowNum), m_wMaxNum(wMaxNum), 
    m_StartTime(StartTime), m_EndTime(EndTime), m_NextDropTime(StartTime)
{ 

}


CGameEventDBMgr& CGameEventDBMgr::GetInstance()
{
    static CGameEventDBMgr gameEventMgr;
    return gameEventMgr;
}

CGameEventDBMgr::CGameEventDBMgr()
:   m_lpDBComponent(0)
{

}

CGameEventDBMgr::~CGameEventDBMgr()
{

}


bool CGameEventDBMgr::Initialize(CDBComponent& DBComponent)
{	
    m_lpDBComponent = &DBComponent;

	const int MAX_ROWS = 1024;
	int nGetRows = 0;

	if (!DBComponent.ExecuteQuery(
        "SELECT nItemIndex, nItemAmount, nMaxAmount, StartTime, EndTime FROM TblEventItem"))
	{
		ERRLOG0(g_Log, "게임 이벤트 데이터 얻어오기 실패");
		return false;
	}
	
	LimitDropItemDB* dropItemDB = new LimitDropItemDB[MAX_ROWS];
	memset(dropItemDB, 0, sizeof(LimitDropItemDB) * MAX_ROWS);

	while (DBComponent.GetData((void**)dropItemDB, sizeof(LimitDropItemDB), MAX_ROWS, &nGetRows))
	{   
		if (0 == nGetRows) 
        { 
            break; 
        }

		for (LimitDropItemDB* lpDropItemDB = dropItemDB; nGetRows > 0; --nGetRows, ++lpDropItemDB)
		{
			tm StartTime, EndTime;
			memset(&StartTime, 0, sizeof(tm));
			memset(&EndTime, 0, sizeof(tm));

			StartTime.tm_year = lpDropItemDB->m_StartTime.Year - 1900;
			StartTime.tm_mon = lpDropItemDB->m_StartTime.Month - 1;
			StartTime.tm_mday = lpDropItemDB->m_StartTime.Day;
			StartTime.tm_hour = lpDropItemDB->m_StartTime.Hour;
			StartTime.tm_min = lpDropItemDB->m_StartTime.Minute;
			StartTime.tm_sec = lpDropItemDB->m_StartTime.Second;

			EndTime.tm_year = lpDropItemDB->m_EndTime.Year - 1900;
			EndTime.tm_mon = lpDropItemDB->m_EndTime.Month - 1;
			EndTime.tm_mday = lpDropItemDB->m_EndTime.Day;
			EndTime.tm_hour = lpDropItemDB->m_EndTime.Hour;
			EndTime.tm_min = lpDropItemDB->m_EndTime.Minute;
			EndTime.tm_sec = lpDropItemDB->m_EndTime.Second;

			LimitDropItem dropItem(static_cast<unsigned short>(lpDropItemDB->m_dwItemIndex), 
				static_cast<unsigned short>(lpDropItemDB->m_dwItemAmount), 
				static_cast<unsigned short>(lpDropItemDB->m_dwMaxAmount), 
				StartTime, EndTime);

			m_lstItem.push_back(dropItem);
		}

		memset(dropItemDB, 0, sizeof(LimitDropItemDB) * MAX_ROWS);
	}

	delete [] dropItemDB;
	return true;
}

void CGameEventDBMgr::CheckGameEvent()
{
	if (0 != m_lpDBComponent && !m_lstItem.empty())
	{
        LimitDropItemList::iterator pos = m_lstItem.begin();
        LimitDropItemList::iterator end = m_lstItem.end();

		for (; pos != end; ++pos)
		{
			LimitDropItem& dropItem = *pos;

			// 정해진 수량을 다 떨군 경우
			if (dropItem.m_wNowNum >= dropItem.m_wMaxNum)
			{
				continue;
			}

			time_t localTime;
			time(&localTime);

			// 이벤트 기간이 아닌 경우
			if (0 >= difftime(localTime, mktime(&dropItem.m_StartTime)) || 
				0 <= difftime(localTime, mktime(&dropItem.m_EndTime)))
			{
				continue;
			}

			if (0 <= difftime(localTime, mktime(&dropItem.m_NextDropTime)))		
			{
				// 차회 드랍 시각이 결정되어 있는 경우에만 드랍한다.
				if (dropItem.m_NextDropTime.tm_year != dropItem.m_StartTime.tm_year || 
					dropItem.m_NextDropTime.tm_mon != dropItem.m_StartTime.tm_mon || 
					dropItem.m_NextDropTime.tm_mday != dropItem.m_StartTime.tm_mday || 
					dropItem.m_NextDropTime.tm_hour != dropItem.m_StartTime.tm_hour || 
					dropItem.m_NextDropTime.tm_min != dropItem.m_StartTime.tm_min || 
					dropItem.m_NextDropTime.tm_sec != dropItem.m_StartTime.tm_sec)
				{
					if (SendDropItemEvent(dropItem.m_wItemID))
					{
						++dropItem.m_wNowNum;

                        DETLOG4(g_Log, "아이템 수량 변동 : ID:%d/Qty:%d/StartTime:%d/EndTime:%d",                        
                            dropItem.m_wItemID, dropItem.m_wNowNum,
                            mktime(&dropItem.m_StartTime), mktime(&dropItem.m_EndTime));

                        if(!DBComponent::GameDB::UpdateEventItem(
                            *m_lpDBComponent, dropItem.m_wItemID, dropItem.m_wNowNum))
                        {
                            ERRLOG2(g_Log, "ItemID:%6u / NowNum:%u / 변경된 수량 DB에 업데이트 실패",
                                dropItem.m_wItemID, dropItem.m_wNowNum);
                        }
                        else if(DBAgent::SendPacket::ItemQtyCheck(dropItem.m_wItemID, 
                            dropItem.m_wMaxNum, dropItem.m_wNowNum, 
                            mktime(&dropItem.m_StartTime), mktime(&dropItem.m_EndTime), 
                            PktItemQtyControl::STATUS_ACK))
                        {
						    DETLOG4(g_Log, "아이템 수량 변동 전송 성공 : ID:%d/Qty:%d/StartTime:%d/EndTime:%d",                        
							    dropItem.m_wItemID, dropItem.m_wNowNum,
							    mktime(&dropItem.m_StartTime), mktime(&dropItem.m_EndTime));
                        }
                    }
				}

				dropItem.m_NextDropTime = *localtime(&localTime);
				UpdateTimeDropItemEvent(dropItem);
			}
		}
	}
}

bool CGameEventDBMgr::InsertDropItem(unsigned short wItemID, unsigned short wMaxNum, tm StartTime, tm EndTime)
{
    if(0 != m_lpDBComponent)
    {
        LimitDropItemList::iterator pos = m_lstItem.begin();
        LimitDropItemList::iterator end = m_lstItem.end();

	    for (; pos != end; ++pos)
	    {
		    // 기존에 존재하는 것에 대한 수정
            LimitDropItem& dropItem = *pos;

		    if (dropItem.m_wItemID == wItemID)
		    {
			    dropItem.m_wMaxNum      = wMaxNum;
			    dropItem.m_StartTime    = StartTime;
			    dropItem.m_EndTime      = EndTime;
			    dropItem.m_NextDropTime = StartTime;

                return DBComponent::GameDB::UpdateEventItem(
                    *m_lpDBComponent, wItemID, wMaxNum, StartTime, EndTime);
		    }
	    }

	    // 새로 추가되는 경우
	    m_lstItem.push_back(LimitDropItem(wItemID, 0, wMaxNum, StartTime, EndTime));

	    return DBComponent::GameDB::InsertEventItem(
            *m_lpDBComponent, wItemID, wMaxNum, StartTime, EndTime);
    }

    return false;
}

bool CGameEventDBMgr::EraseDropItem(unsigned short wItemID)
{
    if(0 != m_lpDBComponent)
    {
        LimitDropItemList::iterator pos = m_lstItem.begin();
        LimitDropItemList::iterator end = m_lstItem.end();

        for (; pos != end; ++pos)
	    {
		    if (pos->m_wItemID == wItemID)
		    {
			    m_lstItem.erase(pos);

                return DBComponent::GameDB::DeleteEventItem(
                    *m_lpDBComponent, wItemID);
		    }
	    }
    }

	return false;
}


void CGameEventDBMgr::UpdateTimeDropItemEvent(LimitDropItem& ItemInfo)
{
	tm	startTime, endTime;

	// 날짜 간격 계산 (일 단위)
	ZeroMemory(&startTime, sizeof(tm));
	ZeroMemory(&endTime, sizeof(tm));

	startTime.tm_year = ItemInfo.m_NextDropTime.tm_year;
	startTime.tm_mon = ItemInfo.m_NextDropTime.tm_mon;
	startTime.tm_mday = ItemInfo.m_NextDropTime.tm_mday;

	endTime.tm_year = ItemInfo.m_EndTime.tm_year;
	endTime.tm_mon = ItemInfo.m_EndTime.tm_mon;
	endTime.tm_mday = ItemInfo.m_EndTime.tm_mday;

	double dateDuration = difftime(mktime(&endTime), mktime(&startTime));
	int nDay = static_cast<int>(dateDuration) / 60 / 60 / 24 + 1;

	// 시간 간격 계산 (분 단위)
	ZeroMemory(&startTime, sizeof(tm));
	ZeroMemory(&endTime, sizeof(tm));

	// mktime() 실패 방지를 위한 초기화
	{
		// day of the month - [1,31] 
		startTime.tm_mday = 1;
		endTime.tm_mday = 1;

		// time_t 는 1970년부터 표기한다.
		startTime.tm_year = 70;
		endTime.tm_year = 70;
	}

	startTime.tm_hour = ItemInfo.m_NextDropTime.tm_hour;
	startTime.tm_min = ItemInfo.m_NextDropTime.tm_min;
	startTime.tm_sec = ItemInfo.m_NextDropTime.tm_sec;

	endTime.tm_hour = ItemInfo.m_EndTime.tm_hour;
	endTime.tm_min = ItemInfo.m_EndTime.tm_min;
	endTime.tm_sec = ItemInfo.m_EndTime.tm_sec;

	double timeDuration = difftime(mktime(&endTime), mktime(&startTime));
	int nTime = static_cast<int>(timeDuration) / 60;
	if (nTime <= 0)
	{
		ERRLOG1(g_Log, "드랍 아이템 이벤트 시간이 이상합니다. 시간차:%d", nTime);
		return;
	}

	// Rodin : 계산 방식을 변경해야 한다. 
	//		m_NextDropTime에 저장한 현재 시각이... 
	//		날짜는 범위내지만, 시간은 범위밖이라면 곤란하잖아~

	// 차후 드랍 시각까지 흘러야할 총 시간
	if (ItemInfo.m_wMaxNum <= ItemInfo.m_wNowNum) { return; }
	int nTotalDuration = nDay * nTime / (ItemInfo.m_wMaxNum - ItemInfo.m_wNowNum);

    int nNextTotal = Math::Random::ComplexRandom(nTotalDuration);
	int nNextDate = nNextTotal / nTime;
	int nNextTime = nNextTotal % nTime;

	time_t nextDropTime = mktime(&ItemInfo.m_NextDropTime);
	nextDropTime = nextDropTime + nNextDate * 24 * 60 * 60 + nNextTime * 60;
	ItemInfo.m_NextDropTime = *localtime(&nextDropTime);

	// 드랍이 결정된 날짜의 최후 시각 (이 시각을 넘긴 시각으로 결정되면 다음 날로 변경해준다.)
	tm DropDateLastTime = ItemInfo.m_NextDropTime;
	DropDateLastTime.tm_hour = ItemInfo.m_EndTime.tm_hour;
	DropDateLastTime.tm_min = ItemInfo.m_EndTime.tm_min;
	DropDateLastTime.tm_sec = ItemInfo.m_EndTime.tm_sec;

	if (0 < difftime(mktime(&DropDateLastTime), nextDropTime))
	{
		nextDropTime = static_cast<time_t>(nextDropTime + 24 * 60 * 60 - timeDuration);
		ItemInfo.m_NextDropTime = *localtime(&nextDropTime);
	}
}


class CGetGameServerDispatch
{
public:

    typedef std::vector<DBAgent::CGameDispatch*>    GameDispatchList;

    CGetGameServerDispatch(GameDispatchList& list) : m_list(list) { m_list.reserve(30); }
    bool operator () (unsigned long, CPacketDispatch& packetDispatch)
    {
        DBAgent::CGameDispatch& GameDispatch = static_cast<DBAgent::CGameDispatch&>(packetDispatch);

        SERVER_ID serverID;
        serverID.dwID = GameDispatch.GetServerID();

		switch(serverID.GetZone())
		{
		case SERVER_ID::ZONE1:
		case SERVER_ID::ZONE2:
		case SERVER_ID::ZONE3:
		case SERVER_ID::ZONE4:
		case SERVER_ID::ZONE5:
		case SERVER_ID::CAPITAL:
		case SERVER_ID::ZONE9:
		case SERVER_ID::ZONE12:
            m_list.push_back(&GameDispatch);
			break;

		default:
//			if(( (SERVER_ID::STONE_WAR1 == serverID.GetZone() || SERVER_ID::STONE_WAR2 == serverID.GetZone() || SERVER_ID::STONE_WAR3 == serverID.GetZone()) && true == CServerSetup::GetInstance().UseContents(GameRYL::STONE_BATTLE)) ||
//				(SERVER_ID::ZONE9 == serverID.GetZone() && true == CServerSetup::GetInstance().UseContents(GameRYL::NEWZONE_ZONE9)))

			if (/*SERVER_ID::STONE_WAR1*/SERVER_ID::CAPITAL == serverID.GetZone() || SERVER_ID::STONE_WAR2 == serverID.GetZone() || SERVER_ID::STONE_WAR3 == serverID.GetZone())  /*true == CServerSetup::GetInstance().UseContents(GameRYL::STONE_BATTLE)*/ //was with e&& to the commented part hayzohar statue war fix
			{
				m_list.push_back(&GameDispatch);
			}
			break;
		}

        
        return true;
    }

private:

    GameDispatchList& m_list;
};

bool CGameEventDBMgr::SendDropItemEvent(unsigned short wItemID)
{
    CGetGameServerDispatch::GameDispatchList gameDispatchList;

    // 원하는 서버군들을 얻어온다.
    DBAgent::CGameDispatch::GetDispatchTable().Process(
        CGetGameServerDispatch(gameDispatchList));

	// 랜덤으로 한 서버를 골라 정보를 보낸다.
	if (!gameDispatchList.empty())
	{
        std::random_shuffle(gameDispatchList.begin(), gameDispatchList.end());

        DBAgent::CGameDispatch* lpGameDispatch = *(gameDispatchList.begin());
		if (0 != lpGameDispatch)
		{
            CSendStream& SendStream = lpGameDispatch->GetSendStream();

            char* lpBuffer = SendStream.GetBuffer(sizeof(PktEventDropItem));
            if(0 != lpBuffer)
            {
                PktEventDropItem* lpPktEventDropItem = 
                    reinterpret_cast<PktEventDropItem*>(lpBuffer);

                lpPktEventDropItem->m_usItemID = wItemID;

                if(SendStream.WrapHeader(sizeof(PktEventDropItem), CmdEventDropItem, 0, 0))
                {
                    DETLOG2(g_Log, "ServerID:0x%08X / ItemID:%6u / 게임 서버로 이벤트 아이템 정보 전송", 
                        lpGameDispatch->GetServerID(), wItemID);

                    return true;
                }
            }
		}
	}

	return false;
}


void CGameEventDBMgr::SendDropItemInfo()
{
    LimitDropItemList::iterator pos = m_lstItem.begin();
    LimitDropItemList::iterator end = m_lstItem.end();

    for (; pos != end; ++pos)
    {
        LimitDropItem& dropItem = *pos;

        DBAgent::SendPacket::ItemQtyCheck(dropItem.m_wItemID, dropItem.m_wMaxNum, dropItem.m_wNowNum,
            mktime(&dropItem.m_StartTime), mktime(&dropItem.m_EndTime), PktItemQtyControl::STATUS_ACK);
    }

    DBAgent::SendPacket::ItemQtyCheck(0, 0, 0, 0, 0, PktItemQtyControl::STATUS_ACK_END);
}

