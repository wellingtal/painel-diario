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

// �Ϻ� ���� ��Ÿ�� ���� Ÿ�� (2005-09-20 zun!)
bool CUIDAgentDispatch::ProcessBillingJapan(PktUUT* lpPktUUT)
{
	unsigned char Cmd		= lpPktUUT->m_cCmd;
	unsigned long ServerID	= lpPktUUT->m_dwServerID;
    unsigned long SessionID	= lpPktUUT->m_dwSessionID;
	unsigned long UserID	= lpPktUUT->m_dwUserID;
	unsigned long CharID	= lpPktUUT->m_dwCharID;
	char *AccountName		= lpPktUUT->m_strAccount;
	IN_ADDR& IPAddress		= lpPktUUT->m_IPAddress;

    switch(Cmd)
    {
    case PktUUT::UpdateUIDTableUserLogin:
        {
            CPerformanceInstrument userLogin("UserLogin");
	        CAutoInstrument autoInstrument(userLogin);

			unsigned long dwReturnedFlag = 0;
			
			if (false == DBComponent::BillingDB::USPJapanUserLoginToAuth(CDBSingleObject::GetInstance(),
				AccountName, UserID, 0, inet_ntoa(IPAddress), GetGroup(), &dwReturnedFlag))
			{
				ERRLOG6(g_Log, "USPJapanUserLoginToAuth(User Login) ȣ�� ����- UID: %u, AccountName: %s, ServerID: 0x%08x, CMD: %d, IP: %s, ErrMsg: %s",
                    UserID, AccountName, ServerID, Cmd, inet_ntoa(IPAddress), CDBSingleObject::GetInstance().GetErrorString());

				// Flag 4 -> '�������� ������ �߻��߽��ϴ�.'
                AgentSendPacket::SendUpdateUIDTable(GetGroup(), Cmd, UserID, 4, 0, 0, 'N', 0, 0);
				return true;
			}

            if (0 != dwReturnedFlag)
            {
				// Flag 20 -> �ߺ� �α��� ����
                AgentSendPacket::SendUpdateUIDTable(GetGroup(), Cmd, UserID, 20, 0, 0, 'N', 0, 0);
                return true;
            }

			DETLOG5(g_Log, "���� �α���- UID: %u, AccountName: %s, ServerID: 0x%08x, CMD: %d, IP: %s",
				UserID, AccountName, ServerID, Cmd, inet_ntoa(IPAddress));

			// ���������� �α��� ����
			AgentSendPacket::SendUpdateUIDTable(GetGroup(), Cmd, UserID, 0, 0, 0, 'N', 0, 0);
        }
        break;

    case PktUUT::UpdateUIDTableCharLogin:
        {
            CPerformanceInstrument charLogin("CharLogin");
	        CAutoInstrument autoInstrument(charLogin);

			unsigned long dwReturnedFlag = 0;

			if (false == DBComponent::BillingDB::USPJapanUserLoginToZone(CDBSingleObject::GetInstance(),
				AccountName, UserID, 0, inet_ntoa(IPAddress), GetGroup(), &dwReturnedFlag))
			{
                ERRLOG6(g_Log, "USPJapanUserLoginToZone(Char Login) ȣ�� ����- UID: %u, AccountName: %s, ServerID: 0x%08x, CMD: %d, IP: %s, ErrMsg: %s",
                    UserID, AccountName, ServerID, Cmd, inet_ntoa(IPAddress), CDBSingleObject::GetInstance().GetErrorString());

                AgentSendPacket::SendUpdateUIDTable(GetGroup(), Cmd, UserID, 4, 0, 0, 'N', 0, 0);
				return true;
			}

            if (0 != dwReturnedFlag)
            {
                AgentSendPacket::SendUpdateUIDTable(GetGroup(), Cmd, UserID, 20, 0, 0, 'N', 0, 0);
                return true;
            }

            DETLOG5(g_Log, "ĳ���� �α���- UID: %u, AccountName: %s, ServerID: 0x%08x, CMD: %d, IP: %s",
				UserID, AccountName, ServerID, Cmd, inet_ntoa(IPAddress));

			// ���Ӽ����� �α��� ����
			AgentSendPacket::SendUpdateUIDTable(GetGroup(), Cmd, UserID, 0, 0, 0, 'N', 0, 0);
        }
        break;

	case PktUUT::UpdateUIDTableUserLogout:
	case PktUUT::UpdateUIDTableCharLogout:
		{
            CPerformanceInstrument logout("Logout");
	        CAutoInstrument autoInstrument(logout);

			unsigned long dwReturnedFlag = 0;

			if (false == DBComponent::BillingDB::USPJapanUserLogOut(CDBSingleObject::GetInstance(),
				AccountName, &dwReturnedFlag))
			{
				ERRLOG5(g_Log, "USPJapanUserLogOut ȣ�� ����- UID: %u, AccountName: %s, ServerID: 0x%08x, CMD: %d, ErrMsg: %s",
                    UserID, AccountName, ServerID, Cmd, CDBSingleObject::GetInstance().GetErrorString());

				return true;
			}

			DETLOG5(g_Log, "USPJapanUserLogOut ȣ�� ���� �� ���- UID: %u, AccountName: %s, ServerID: 0x%08x, CMD: %d, ReturnedFlag: %u",
				UserID, AccountName, ServerID, Cmd, dwReturnedFlag);
        }
        break;

	case PktUUT::UpdateUIDTableUserMove:
	case PktUUT::UpdateUIDTableCharMove:
		{
            CPerformanceInstrument move("ZoneMove");
	        CAutoInstrument autoInstrument(move);

			unsigned long dwReturnedFlag = 0;

			if (false == DBComponent::BillingDB::USPJapanUserLogOut(CDBSingleObject::GetInstance(),
				AccountName, &dwReturnedFlag))
			{
				ERRLOG5(g_Log, "USPJapanUserLogOut ȣ�� ����- UID: %u, AccountName: %s, ServerID: 0x%08x, CMD: %d, ErrMsg: %s",
                    UserID, AccountName, ServerID, Cmd, CDBSingleObject::GetInstance().GetErrorString());

				return true;
			}

			DETLOG5(g_Log, "USPJapanUserLogOut ȣ�� ���� �� ���- UID: %u, AccountName: %s, ServerID: 0x%08x, CMD: %d, ReturnedFlag: %u",
				UserID, AccountName, ServerID, Cmd, dwReturnedFlag);
		}
		break;
    };

    return true;
}

bool CUIDAgentDispatch::ProcessDisconnectJapan()
{
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

    return true;
}