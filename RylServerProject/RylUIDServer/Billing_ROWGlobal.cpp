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

#ifdef AUTH_MY
#include "NFAuthClient/AuthClient.h"
#endif

// �븸�� ����Ÿ����, �� 30�и��� �ð��� ���ϴ� ������� ����ȴ�.
// �ð��� ���ϴ� ��ƾ��, �߰輭������ �����Ѵ�.
bool CUIDAgentDispatch::ProcessBillingROWGlobal(PktUUT* lpPktUUT)
{
	unsigned char Cmd		= lpPktUUT->m_cCmd;
	unsigned long ServerID	= lpPktUUT->m_dwServerID;
    unsigned long SessionID	= lpPktUUT->m_dwSessionID;
	unsigned long UserID	= lpPktUUT->m_dwUserID;
	unsigned long CharID	= lpPktUUT->m_dwCharID;
	char *AccountName		= lpPktUUT->m_strAccount;
	char *Password			= lpPktUUT->m_strPassword;
	IN_ADDR& IPAddress		= lpPktUUT->m_IPAddress;

	// ��÷��� ���� ó�� ��ƾ
	switch(Cmd)
	{
	case PktUUT::UpdateUIDTableUserLogin:
		{
			CPerformanceInstrument userLogin("UserLogin");
			CAutoInstrument autoInstrument(userLogin);

#ifdef AUTH_MY
			// edith 2009.09.11 MY�� ���� AllowIP ó���۾�
			char* szAdd = inet_ntoa(IPAddress);
			if(!g_IPSec.IsAliveIP(szAdd))
			{
				AgentSendPacket::SendUpdateUIDTable(GetGroup(), Cmd, UserID, 57, 0, 0, 'N', 0, 0);

				ERRLOG5(g_Log, "����(%u) [%s] ����(0x%08x) ST:%d Alive ȣ�� ���� %s", 
					UserID, AccountName, ServerID, Cmd, inet_ntoa(IPAddress));

				return true;
			}
#endif

			// Ű�� ���� ó��
			RE_USPCheckBilling_Login GetData = {0,};
			
			if(!DBComponent::BillingDB::USPCheckUser_Login(CDBSingleObject::GetInstance(), 
				AccountName, UserID, 0, inet_ntoa(IPAddress), GetGroup(), &GetData))
			{
				ERRLOG6(g_Log, "����(%u) [%s] ����(0x%08x) ST:%d USPCheckUser_Login ȣ�� ���� %s : %s", 
					UserID, AccountName, ServerID, Cmd, inet_ntoa(IPAddress), CDBSingleObject::GetInstance().GetErrorString());

				// DB Query����(Ŭ���̾�Ʈ���� ���� 1�� (�������� ������ �����ϴ�) �߻�
//				AgentSendPacket::SendUpdateUIDTable(GetGroup(), Cmd, UserID, 4, 0, 0, 'N', 0, 0);
//				return true;
			}

			// ROW���� ��÷��� ������ ������� �ʴ´�.
			// ROW���� ��ü������� ����Ѵ�.
			AgentSendPacket::SendUpdateUIDTable(GetGroup(), Cmd, UserID, 0, 0, 0, 'N', 0, 0);
		}
		break;

	case PktUUT::UpdateUIDTableCharLogin:
		{
#ifdef AUTH_MY
			// edith 2009.09.11 MY�� ���� AllowIP ó���۾�
			char* szAdd = inet_ntoa(IPAddress);
			if(!g_IPSec.IsAliveIP(szAdd))
			{
				AgentSendPacket::SendUpdateUIDTable(GetGroup(), Cmd, UserID, 30, 0, 0, 'N', 0, 0);

				ERRLOG5(g_Log, "����(%u) [%s] ����(0x%08x) ST:%d Alive ȣ�� ���� %s", 
					UserID, AccountName, ServerID, Cmd, inet_ntoa(IPAddress));

				return true;
			}
#endif
/*			CPerformanceInstrument userLogin("CharLogin");
			CAutoInstrument autoInstrument(userLogin);

			// Ű�� ���� ó��
			RE_USPCheckBilling_Login GetData = {0,};
			
			if(!DBComponent::BillingDB::USPCheckUser_CharIDLogin(CDBSingleObject::GetInstance(), 
				AccountName, UserID, 0, inet_ntoa(IPAddress), GetGroup(), &GetData))
			{
				ERRLOG6(g_Log, "����(%u) [%s] ����(0x%08x) ST:%d USPCheckUser_Login ȣ�� ���� %s : %s", 
					UserID, AccountName, ServerID, Cmd, inet_ntoa(IPAddress), CDBSingleObject::GetInstance().GetErrorString());

				// DB Query����(Ŭ���̾�Ʈ���� ���� 1�� (�������� ������ �����ϴ�) �߻�
//				AgentSendPacket::SendUpdateUIDTable(GetGroup(), Cmd, UserID, 4, 0, 0, 'N', 0, 0);
//				return true;
			}
*/
			// ROW���� ��÷��� ������ ������� �ʴ´�.
			// ROW���� ��ü������� ����Ѵ�.
			AgentSendPacket::SendUpdateUIDTable(GetGroup(), Cmd, UserID, 0, 0, 0, 'N', 0, 0);
		}
		break;

	case PktUUT::UpdateUIDTableBillingCheck:
		// ROW���� ��÷��� ������ ������� �ʴ´�.
		// ROW���� ��ü������� ����Ѵ�.
		AgentSendPacket::SendUpdateUIDTable(GetGroup(), Cmd, UserID, 0, 0, 0, 'N', 0, 0);
		break;

	case PktUUT::UpdateUIDTableUserLogout:
//	case PktUUT::UpdateUIDTableCharLogout:
		{
			CPerformanceInstrument logout("Logout");
			CAutoInstrument autoInstrument(logout);

			// Ű�� ó��
			unsigned long Return = 0;

			if(!DBComponent::BillingDB::USPCheckUser_LogOut(CDBSingleObject::GetInstance(), AccountName, &Return))
			{
				ERRLOG5(g_Log, "����(%u) [%s] ����(0x%08x) ST:%d USPCheckUser_LogOut ȣ�� ���� : %s", 
					UserID, AccountName, ServerID, Cmd, CDBSingleObject::GetInstance().GetErrorString());
				return true;
			}

			if(0 != Return)
			{
				ERRLOG5(g_Log, "����(%u) [%s] ����(0x%08x) ST:%d USPCheckUser_LogOut ȣ�� ���� ����. %u", 
					UserID, AccountName, ServerID, Cmd, Return);				
			}
		}
		break;
/*
	case PktUUT::UpdateUIDTableUserMove:
	case PktUUT::UpdateUIDTableCharMove:
		{
			CPerformanceInstrument move("ZoneMove");
			CAutoInstrument autoInstrument(move);

			// Ű�� ó��
			unsigned long Return = 0;

			if(!DBComponent::BillingDB::USPCheckUser_LogOut(CDBSingleObject::GetInstance(), AccountName, &Return))
			{
				ERRLOG5(g_Log, "����(%u) [%s] ����(0x%08x) ST:%d USPCheckUser_LogOut ȣ�� ���� : %s", 
					UserID, AccountName, ServerID, Cmd, CDBSingleObject::GetInstance().GetErrorString());
				return true;
			}

			if(0 != Return)
			{
				ERRLOG5(g_Log, "����(%u) [%s] ����(0x%08x) ST:%d USPCheckUser_LogOut ȣ�� ���� ����. %u", 
					UserID, AccountName, ServerID, Cmd, Return);				
			}
		}
		break;
*/
	}

	return true;
}


bool CUIDAgentDispatch::ProcessDisconnectROWGlobal()
{
	// ROW���� ��÷��� ���� ������ ������� �ʱ� ������ �����Ѵ�.
    return true;
}
