
#include "stdafx.h"
#include "GameEventMgr.h"

CGameEventMgr& CGameEventMgr::GetInstance()
{
    static CGameEventMgr gameEventMgr;
    return gameEventMgr;
}

CGameEventMgr::CGameEventMgr()
{

}

CGameEventMgr::~CGameEventMgr()
{

}

bool CGameEventMgr::Initialize(const char* szLotteryFileName)
{
	if (false == m_LotteryEvent.Initialize(szLotteryFileName))
	{
		ERRLOG0(g_Log, "복권 이벤트 초기화에 실패하였습니다.");
		return false;
	}

	return true;
}
