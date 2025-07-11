#include "stdafx.h"
#include "UserIDTable.h"
#include "UIDAgentDispatch.h"
#include "SendAgentPacket.h"

#include <Log/ServerLog.h>
#include <Network/Packet/PacketStruct/ServerPacket.h>
#include <Utility/Setup/ServerSetup.h>
#include <DB/DBComponent.h>
#include <DB/BillingDBComponent.h>

#include <Utility/Debug/PerformanceCheck.h>

#include "RylUIDServer.h"

// 감마니아 오픈 베타용 과금 타입 (2005-09-20 zun!)
bool CUIDAgentDispatch::ProcessBillingGammania(PktUUT* lpPktUUT)
{
	unsigned char Cmd		= lpPktUUT->m_cCmd;
	unsigned long ServerID	= lpPktUUT->m_dwServerID;
    unsigned long SessionID	= lpPktUUT->m_dwSessionID;
	unsigned long UserID	= lpPktUUT->m_dwUserID;
	unsigned long CharID	= lpPktUUT->m_dwCharID;
	char *AccountName		= lpPktUUT->m_strAccount;
	// edith 2005.1.22 감마니아는 게임안에서의 처리를 여기서 해야한다.
	char *Password			= lpPktUUT->m_strPassword;
	IN_ADDR& IPAddress		= lpPktUUT->m_IPAddress;

	LPUIDINFO pInfo = CRylUIDServer::GetInstance().FindAuth(AccountName);
	if(pInfo == NULL)
	{
		UIDINFO stTemp;
		stTemp.Cmd = Cmd;
		stTemp.ServerID = ServerID;
		stTemp.SessionID = SessionID;
		stTemp.Group = GetGroup();
		stTemp.UserID = UserID;
		stTemp.CharID = CharID;
		stTemp.Address = IPAddress;
		strcpy(stTemp.AccountName, AccountName);
		strcpy(stTemp.Password, Password);		
		pInfo = CRylUIDServer::GetInstance().AddAuth(stTemp);
	}

	// 커맨드와 패스워드는 매번 변경한다.
	pInfo->Cmd = Cmd;

	if(strlen(Password) > 0 && strncmp(pInfo->Password, Password, PktUUT::MaxPasswordLen) != 0)
	{
		strncpy(pInfo->Password, Password, PktUUT::MaxPasswordLen);
	}

    switch(Cmd)
    {
    case PktUUT::UpdateUIDTableUserLogin:
        {
            CPerformanceInstrument userLogin("UserLogin");
	        CAutoInstrument autoInstrument(userLogin);

			const char* szAddress = inet_ntoa(pInfo->Address);

			// edith 2008.01.22 감마니아 인증 처리부분
			char strBuff[512];

			sprintf(strBuff, "%s'%s'%s'%s'1'I'0'%s'%s;%d;'\r\n", 
				CServerSetup::GetInstance().GetGammaniaCode()
				, CServerSetup::GetInstance().GetGammaniaRegin()
				, pInfo->AccountName
				, pInfo->Password
				, szAddress
				, pInfo->AccountName 
				, Cmd);

			CRylUIDServer::GetInstance().SendPost(0, strBuff);
		}
		break;

	case PktUUT::UpdateUIDTableUserLogout:
	case PktUUT::UpdateUIDTableCharLogout:
        {
            CPerformanceInstrument logout("Logout");
	        CAutoInstrument autoInstrument(logout);

			const char* szAddress = inet_ntoa(pInfo->Address);

			// 감마니아 로그아웃 처리
			char strBuff[512];

			sprintf(strBuff, "%s'%s'%s'R'''%s'%s;'\r\n", 
				CServerSetup::GetInstance().GetGammaniaCode()
				, CServerSetup::GetInstance().GetGammaniaRegin()
				, pInfo->AccountName
				, szAddress
				, pInfo->AccountName );

			CRylUIDServer::GetInstance().SendPost(1, strBuff);
		}
		break;

   case PktUUT::UpdateUIDTableCharLogin:
        {
            CPerformanceInstrument charLogin("CharLogin");
	        CAutoInstrument autoInstrument(charLogin);

			const char* szAddress = inet_ntoa(pInfo->Address);

			// edith 2008.01.22 감마니아 인증 처리부분
			char strBuff[512];

			sprintf(strBuff, "%s'%s'%s'%s'1'I'0'%s'%s;%d;'\r\n", 
				CServerSetup::GetInstance().GetGammaniaCode()
				, CServerSetup::GetInstance().GetGammaniaRegin()
				, pInfo->AccountName
				, pInfo->Password
				, szAddress
				, pInfo->AccountName
				, Cmd);

			CRylUIDServer::GetInstance().SendPost(0, strBuff);
		}
		break;

	case PktUUT::UpdateUIDTableUserMove:
	case PktUUT::UpdateUIDTableCharMove:
		{
            CPerformanceInstrument move("ZoneMove");
	        CAutoInstrument autoInstrument(move);

			const char* szAddress = inet_ntoa(pInfo->Address);

			// 감마니아 로그아웃 처리
			char strBuff[512];

			sprintf(strBuff, "%s'%s'%s'R'''%s'%s;'\r\n", 
				CServerSetup::GetInstance().GetGammaniaCode()
				, CServerSetup::GetInstance().GetGammaniaRegin()
				, pInfo->AccountName
				, szAddress
				, pInfo->AccountName );

			CRylUIDServer::GetInstance().SendPost(1, strBuff);
		}
		break;
	}

	return true;
}

bool CUIDAgentDispatch::ProcessDisconnectGammania()
{
/*
	unsigned long dwResult = 0;
    if (false == DBComponent::BillingDB::USPServer_End(CDBSingleObject::GetInstance(), m_Group, &dwResult))
    {
        SERLOG3(g_Log, "%d 그룹 로그아웃 실패. 결과값:0x%08x DB에러 로그:%s", 
            m_Group, dwResult, CDBSingleObject::GetInstance().GetErrorString());
    }
    else
    {
        SERLOG1(g_Log, "%d 그룹 USEServer_End 호출 성공", m_Group);
    }
*/
    return true;
}