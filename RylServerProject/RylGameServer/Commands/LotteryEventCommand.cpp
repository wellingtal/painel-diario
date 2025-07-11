
#include "stdafx.h"

#include <GameEvent/GameEventMgr.h>
#include <GameEvent/LotteryEvent/LotteryEvent.h>

#include "RylGameServer.h"
#include "Commands.h"


CConsoleCommand* CCMDLotteryEvent::Clone(const char* szCommand, size_t nCommandLength)
{
	const int MAX_BUFFER = 256;
	char szBuffer[MAX_BUFFER + 1];

	const char* szDelimiter = " \t\r\n";

	_snprintf(szBuffer, MAX_BUFFER, "%s", szCommand);
	szBuffer[MAX_BUFFER] = '\0';

	char* token = strtok(szBuffer, szDelimiter);

	CCMDLotteryEvent* lpCMD = new CCMDLotteryEvent;
	if (NULL != lpCMD)
	{
		token = strtok(NULL, szDelimiter);        
		if (NULL == token)
		{
			lpCMD->m_cCmd = NOW_STATE;
		}
		else if (0 == strcmp("on", token))
		{
			lpCMD->m_cCmd = LOTTERY_EVENT_ON;
		}
		else if (0 == strcmp("off", token))
		{
			lpCMD->m_cCmd = LOTTERY_EVENT_OFF;
		}
		else
		{
			lpCMD->m_cCmd = ERROR_CMD;
		}
	}

	return lpCMD;
}



bool CCMDLotteryEvent::DoProcess()
{	
	CRylGameServer::GetInstance().PrintOutput(
        "Use server setup for modify lottery event.");

/*
	CLotteryEvent& LotteryEvent = CGameEventMgr::GetInstance().GetLotteryEvent();

	switch (m_cCmd)
	{
		case NOW_STATE:
			if (true == LotteryEvent.IsActive())
			{
				CRylGameServer::GetInstance().PrintOutput("현재 복권 이벤트가 진행중입니다.");
			}
			else
			{
				CRylGameServer::GetInstance().PrintOutput("현재 복권 이벤트를 진행하고 있지 않습니다.");
			}
			break;

		case LOTTERY_EVENT_ON:
			LotteryEvent.Active();
			CRylGameServer::GetInstance().PrintOutput("복권 이벤트를 시작하였습니다.");
			break;

		case LOTTERY_EVENT_OFF:
			LotteryEvent.DeActive();
			CRylGameServer::GetInstance().PrintOutput("복권 이벤트가 종료되었습니다.");
			break;

		case ERROR_CMD:
		default:
			CRylGameServer::GetInstance().PrintOutput("잘못된 명령입니다.");
			break;
	}
*/

	return true;
}

