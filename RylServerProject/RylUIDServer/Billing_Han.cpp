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

// �Ѱ��� ���� ó�� ��ƾ. DB���� ���н� ���� �ִ� �κи� ���� �����ϰ�,
// �ڵ� ������ �Ͽ��� �Ѵ�.
bool CUIDAgentDispatch::ProcessBillingHan(PktUUT* lpPktUUT)
{
	unsigned char Cmd		= lpPktUUT->m_cCmd;
	unsigned long ServerID	= lpPktUUT->m_dwServerID;
    unsigned long SessionID	= lpPktUUT->m_dwSessionID;
	unsigned long UserID	= lpPktUUT->m_dwUserID;
	unsigned long CharID	= lpPktUUT->m_dwCharID;
	char *AccountName		= lpPktUUT->m_strAccount;
	IN_ADDR& IPAddress		= lpPktUUT->m_IPAddress;

	// �Ѱ��� ó�� ��ƾ
	switch(Cmd) 
	{
	case PktUUT::UpdateUIDTableUserLogin:
		{
            CPerformanceInstrument userLogin("UserLogin");
	        CAutoInstrument autoInstrument(userLogin);

			// Ű�� ���� ó��
			RE_USPCheckBilling_Login GetData = {0,};

			char Check = CServerSetup::GetInstance().GetFreeCheck() ? 1 : 0; // �⺻(=0), �������(=1)
			if(!DBComponent::BillingDB::USPCheckBilling_Login(CDBSingleObject::GetInstance(), 
                AccountName, UserID, Check, inet_ntoa(IPAddress), GetGroup(), &GetData))
			{
                SERLOG6(g_Log, "����(%u) [%s] ����(0x%08x) ST:%d USPCheckBilling_Login ȣ�� ���� %s : %s", 
                    UserID, AccountName, ServerID, Cmd, inet_ntoa(IPAddress), CDBSingleObject::GetInstance().GetErrorString());

                // DB Query����(Ŭ���̾�Ʈ���� ���� 1�� (�������� ������ �����ϴ�) �߻�
                AgentSendPacket::SendUpdateUIDTable(GetGroup(), Cmd, UserID, 4, 0, 0, 'N', 0, 0);
				return true;
			}

			// ������Ʈ Ack
			if(FALSE == CServerSetup::GetInstance().GetIgnoreFlag())
			{
                if(1 == GetData.Flag && PktUUT::DISCONNECT_USER == lpPktUUT->GetError())
				{
                    // ������ �̹� ������ �ְ�, ���� ���̱� �÷��װ� ����, ������ ���δ�.
                    AgentSendPacket::SendHanUnitedUserKill(GetData.intCRMIndex1, AccountName, 0);
				}

                AgentSendPacket::SendUpdateUIDTable(GetGroup(), PktUUT::UpdateUIDTableUserLogin, 
                    UserID, GetData.Flag, GetData.PlayTime, GetData.intCRMIndex1, 
                    GetData.strBillingType[0], 0, lpPktUUT->GetError());
			}
			else
			{
                if(1 == GetData.Flag && PktUUT::DISCONNECT_USER == lpPktUUT->GetError())
                {
                    // ������ �̹� ������ �ְ�, ���� ���̱� �÷��װ� ����, ������ ���δ�.
                    AgentSendPacket::SendHanUnitedUserKill(GetData.intCRMIndex1, AccountName, 0);
                }

                // ������ ������Ų��. �� �ߺ��α����̸� �α��� ���ϵ��� �Ѵ�.
                AgentSendPacket::SendUpdateUIDTable(GetGroup(), Cmd, 
                    UserID, (1 == GetData.Flag) ? 1 : 0, GetData.PlayTime, GetData.intCRMIndex1, 
                    GetData.strBillingType[0], 0, lpPktUUT->GetError());
			}

			if(0 == GetData.Flag)
			{
				IncreaseSucceedNum();
			}
			else
			{
				IncreaseFailNum();
			}

            DETLOG7(g_Log, "����(%u) [%s] ����(0x%08x) ST:%d USPCheckBilling_Login ȣ�� ���� �� ���. %u, %u [%s]", 
                UserID, AccountName, ServerID, Cmd, GetData.Flag, GetData.intCRMIndex1, inet_ntoa(IPAddress));

			IncreaseUnitNum();
		}
		break;

	case PktUUT::UpdateUIDTableCharLogin:
		{
            CPerformanceInstrument charLogin("CharLogin");
	        CAutoInstrument autoInstrument(charLogin);

			// Ű�� ���� ó��
			RE_USPCheckBilling_Login GetData = {0,};

			char Check = CServerSetup::GetInstance().GetFreeCheck() ? 1 : 0; // �⺻(=0), �������(=1)
			if(!DBComponent::BillingDB::USPCheckBilling_CharIDLogin(CDBSingleObject::GetInstance(), 
                AccountName, UserID, Check, inet_ntoa(IPAddress), GetGroup(), &GetData))
			{
                SERLOG6(g_Log, "����(%u) [%s] ����(0x%08x) ST:%d USPCheckBilling_CharIDLogin ȣ�� ���� %s : %s", 
                    UserID, AccountName, ServerID, Cmd, inet_ntoa(IPAddress), CDBSingleObject::GetInstance().GetErrorString());

                // DB Query����(Ŭ���̾�Ʈ���� ���� 1�� (�������� ������ �����ϴ�) �߻�
                AgentSendPacket::SendUpdateUIDTable(GetGroup(), Cmd, UserID, 4, 0, 0, 'N', 0, 0);
				return true;
			}

			// ������Ʈ Ack
			if(FALSE == CServerSetup::GetInstance().GetIgnoreFlag())
			{
				AgentSendPacket::SendUpdateUIDTable(GetGroup(), PktUUT::UpdateUIDTableCharLogin, 
					UserID, GetData.Flag, GetData.PlayTime, GetData.intCRMIndex1, GetData.strBillingType[0], 0, 0);
			}
			else
			{
				AgentSendPacket::SendUpdateUIDTable(GetGroup(), PktUUT::UpdateUIDTableCharLogin, 
					UserID, 0, GetData.PlayTime, GetData.intCRMIndex1, GetData.strBillingType[0], 0, 0);
			}

			if(0 == GetData.Flag)
			{
				IncreaseSucceedNum();
			}
			else
			{
				IncreaseFailNum();
			}

            DETLOG7(g_Log, "����(%u) [%s] ����(0x%08x) ST:%d USPCheckBilling_CharIDLogin ȣ�� ���� �� ���. %u, %u [%s]",
                UserID, AccountName, ServerID, Cmd, GetData.Flag, GetData.intCRMIndex1, inet_ntoa(IPAddress));

			IncreaseUnitNum();
		}
		break;

	case PktUUT::UpdateUIDTableUserLogout:
	case PktUUT::UpdateUIDTableCharLogout:
		{
            CPerformanceInstrument logout("Logout");
	        CAutoInstrument autoInstrument(logout);

			// Ű�� ó��
			unsigned long Return = 0;

			if(!DBComponent::BillingDB::USPCheckBilling_LogOut(CDBSingleObject::GetInstance(), AccountName, &Return))
			{
                SERLOG5(g_Log, "����(%u) [%s] ����(0x%08x) ST:%d USPCheckBilling_LogOut ȣ�� ���� : %s", 
                    UserID, AccountName, ServerID, Cmd, CDBSingleObject::GetInstance().GetErrorString());
				return true;
			}

			if(0 != Return)
			{
                SERLOG5(g_Log, "����(%u) [%s] ����(0x%08x) ST:%d USPCheckBilling_LogOut ȣ�� ���� ����. %u", 
                    UserID, AccountName, ServerID, Cmd, Return);
				return true;
			}

            DETLOG5(g_Log, "����(%u) [%s] ����(0x%08x) ST:%d USPCheckBilling_LogOut ȣ�� ���� �� ���. %u", 
                UserID, AccountName, ServerID, Cmd, Return);

			DecreaseUnitNum();
		}
		break;

	case PktUUT::UpdateUIDTableUserMove:
	case PktUUT::UpdateUIDTableCharMove:
		{
            CPerformanceInstrument move("ZoneMove");
	        CAutoInstrument autoInstrument(move);

			// Ű�� ó��
			unsigned long Return = 0;

			if(!DBComponent::BillingDB::USPCheckBilling_LogOut(CDBSingleObject::GetInstance(), AccountName, &Return))
			{
                SERLOG5(g_Log, "����(%u) [%s] ����(0x%08x) ST:%d USPCheckBilling_LogOut ȣ�� ���� : %s",
                    UserID, AccountName, ServerID, Cmd, CDBSingleObject::GetInstance().GetErrorString());
				return true;
			}

			if(0 != Return)
			{
                SERLOG5(g_Log, "����(%u) [%s] ����(0x%08x) ST:%d USPCheckBilling_LogOut ȣ�� ���� ����. %u", 
                    UserID, AccountName, ServerID, Cmd, Return);
				return true;
			}

            DETLOG5(g_Log, "����(%u) [%s] ����(0x%08x) ST:%d USPCheckBilling_LogOut ȣ�� ���� �� ���. %u", 
                UserID, AccountName, ServerID, Cmd, Return);

			DecreaseUnitNum();
		}
		break;
	
	default:
		break;
	}

    return true;
}


bool CUIDAgentDispatch::ProcessDisconnectHan()
{
    // �Ѱ��� �����̳�, �Ѱ��� ���պ����� ���.
	unsigned long Result = 0;

	if(!DBComponent::BillingDB::USPDisConnectLogOut(CDBSingleObject::GetInstance(), m_Group, &Result))
	{
        SERLOG3(g_Log, "%d �׷� �α׾ƿ� ����. �����:0x%08x DB���� �α�:%s",
            m_Group, Result, CDBSingleObject::GetInstance().GetErrorString());				
	}
	else
	{
        SERLOG1(g_Log, "%d �׷� USPDisConnectLogOut ȣ�� ����", m_Group);

		if(!DBComponent::BillingDB::USPServer_End(CDBSingleObject::GetInstance(), m_Group, &Result))
		{
			SERLOG3(g_Log, "%d �׷� �α׾ƿ� ����. �����:0x%08x DB���� �α�:%s", 
				m_Group, Result, CDBSingleObject::GetInstance().GetErrorString());
		}
		else
		{
			SERLOG1(g_Log, "%d �׷� USEServer_End ȣ�� ����", m_Group);
		}
	}

    return true;
}

