
#include "stdafx.h"

#include <Creature/CreatureManager.h>

#include "RylGameServer.h"
#include "Commands.h"


CConsoleCommand* CCMDAutoBalance::Clone(const char* szCommand, size_t nCommandLength)
{
	const int MAX_BUFFER = 256;
	char szBuffer[MAX_BUFFER + 1];

	const char* szDelimiter = " \t\r\n";

	_snprintf(szBuffer, MAX_BUFFER, "%s", szCommand);
	szBuffer[MAX_BUFFER] = '\0';

	char* token = strtok(szBuffer, szDelimiter);

	CCMDAutoBalance* lpCMD = new CCMDAutoBalance;
	if (NULL != lpCMD)
	{
		token = strtok(NULL, szDelimiter);        
		if (NULL == token)
		{
			lpCMD->m_cCmd = NOW_STATE;
		}
		else if (0 == strcmp("on", token))
		{
			lpCMD->m_cCmd = AUTO_BALANCE_ON;
		}
		else if (0 == strcmp("off", token))
		{
			lpCMD->m_cCmd = AUTO_BALANCE_OFF;
		}
		else
		{
			lpCMD->m_cCmd = ERROR_CMD;
		}
	}

	return lpCMD;
}



bool CCMDAutoBalance::DoProcess()
{
	const TCHAR* szMessage = 0;

	switch (m_cCmd)
	{
		case NOW_STATE:
			if (CCreatureManager::GetInstance().GetAutoBalance())
			{
				szMessage = _T("AutoBalance is now On");
			}
			else
			{
				szMessage = _T("AutoBalance is now Off");
			}
			break;

		case AUTO_BALANCE_ON:
			CCreatureManager::GetInstance().SetAutoBalance(true);
			szMessage = _T("Turn On AutoBalance");
			break;

		case AUTO_BALANCE_OFF:
			CCreatureManager::GetInstance().SetAutoBalance(false);
			szMessage = _T("Turn Off AutoBalance");
			break;

		case ERROR_CMD:
		default:
			szMessage = _T("Invalid autobalance command");
			break;
	}

	if(0 != szMessage)
	{
        CRylGameServer::GetInstance().PrintOutput(szMessage);
	}

	return true;
}

