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

// �����Ͼ� ���� ��Ÿ�� ���� Ÿ�� (2005-09-20 zun!)
bool CUIDAgentDispatch::ProcessBillingGammania(PktUUT* lpPktUUT)
{
	unsigned char Cmd		= lpPktUUT->m_cCmd;
	unsigned long ServerID	= lpPktUUT->m_dwServerID;
    unsigned long SessionID	= lpPktUUT->m_dwSessionID;
	unsigned long UserID	= lpPktUUT->m_dwUserID;
	unsigned long CharID	= lpPktUUT->m_dwCharID;
	char *AccountName		= lpPktUUT->m_strAccount;
	// edith 2005.1.22 �����Ͼƴ� ���Ӿȿ����� ó���� ���⼭ �ؾ��Ѵ�.
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

	// Ŀ�ǵ�� �н������ �Ź� �����Ѵ�.
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

			// edith 2008.01.22 �����Ͼ� ���� ó���κ�
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

			// �����Ͼ� �α׾ƿ� ó��
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

			// edith 2008.01.22 �����Ͼ� ���� ó���κ�
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

			// �����Ͼ� �α׾ƿ� ó��
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
        SERLOG3(g_Log, "%d �׷� �α׾ƿ� ����. �����:0x%08x DB���� �α�:%s", 
            m_Group, dwResult, CDBSingleObject::GetInstance().GetErrorString());
    }
    else
    {
        SERLOG1(g_Log, "%d �׷� USEServer_End ȣ�� ����", m_Group);
    }
*/
    return true;
}