
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
		ERRLOG0(g_Log, "���� �̺�Ʈ �ʱ�ȭ�� �����Ͽ����ϴ�.");
		return false;
	}

	return true;
}
