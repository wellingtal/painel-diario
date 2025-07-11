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

// �븸�� ����Ÿ����, �� 30�и��� �ð��� ���ϴ� ������� ����ȴ�.
// �ð��� ���ϴ� ��ƾ��, �߰輭������ �����Ѵ�.
bool CUIDAgentDispatch::ProcessBillingYouxiLand(PktUUT* lpPktUUT)
{
	unsigned char Cmd		= lpPktUUT->m_cCmd;
	unsigned long ServerID	= lpPktUUT->m_dwServerID;
    unsigned long SessionID	= lpPktUUT->m_dwSessionID;
	unsigned long UserID	= lpPktUUT->m_dwUserID;
	unsigned long CharID	= lpPktUUT->m_dwCharID;
	char *AccountName		= lpPktUUT->m_strAccount;
	char *Password			= lpPktUUT->m_strPassword;
	IN_ADDR& IPAddress		= lpPktUUT->m_IPAddress;

    // �� �ߺ��α��� �� �α��ηα׾ƿ� �α� ��ƾ
    switch(Cmd)
    {
    case PktUUT::UpdateUIDTableUserLogin:
        {
            CPerformanceInstrument userLogin("UserLogin");
	        CAutoInstrument autoInstrument(userLogin);

			// Ű�� ���� ó��
			RE_USPCheckBilling_Login GetData = {0,};
			
			if(!DBComponent::BillingDB::USPCheckBilling_Login(CDBSingleObject::GetInstance(), 
                AccountName, UserID, 0, inet_ntoa(IPAddress), GetGroup(), &GetData))
			{
                SERLOG6(g_Log, "����(%u) [%s] ����(0x%08x) ST:%d USPCheckBilling_Login ȣ�� ���� %s : %s", 
                    UserID, AccountName, ServerID, Cmd, inet_ntoa(IPAddress), CDBSingleObject::GetInstance().GetErrorString());

                // DB Query����(Ŭ���̾�Ʈ���� ���� 1�� (�������� ������ �����ϴ�) �߻�
                AgentSendPacket::SendUpdateUIDTable(GetGroup(), Cmd, UserID, 4, 0, 0, 'N', 0, 0);
				return true;
			}

			// ������Ʈ Ack

            if(0 != GetData.Flag)
            {
                AgentSendPacket::SendUpdateUIDTable(GetGroup(), Cmd, UserID, 20, 0, 0, 'N', 0, 0);
                return true;
            }

//            DETLOG7(g_Log, "����(%u) [%s] ����(0x%08x) ST:%d USPCheckBilling_Login ȣ�� ���� �� ���. %u, %u [%s]", 
//                UserID, AccountName, ServerID, Cmd, GetData.Flag, GetData.intCRMIndex1, inet_ntoa(IPAddress));
        }
        break;

    case PktUUT::UpdateUIDTableCharLogin:
        {
            CPerformanceInstrument charLogin("CharLogin");
	        CAutoInstrument autoInstrument(charLogin);

			// Ű�� ���� ó��
			RE_USPCheckBilling_Login GetData = {0,};
			
			if(!DBComponent::BillingDB::USPCheckBilling_CharIDLogin(CDBSingleObject::GetInstance(), 
                AccountName, UserID, 0, inet_ntoa(IPAddress), GetGroup(), &GetData))
			{
                SERLOG6(g_Log, "����(%u) [%s] ����(0x%08x) ST:%d USPCheckBilling_CharIDLogin ȣ�� ���� %s : %s", 
                    UserID, AccountName, ServerID, Cmd, inet_ntoa(IPAddress), CDBSingleObject::GetInstance().GetErrorString());

                // DB Query����(Ŭ���̾�Ʈ���� ���� 1�� (�������� ������ �����ϴ�) �߻�
                AgentSendPacket::SendUpdateUIDTable(GetGroup(), Cmd, UserID, 4, 0, 0, 'N', 0, 0);
				return true;
			}

			// ������Ʈ Ack
            if(0 != GetData.Flag)
            {
                AgentSendPacket::SendUpdateUIDTable(GetGroup(), Cmd, UserID, 20, 0, 0, 'N', 0, 0);
                return true;
            }

//            DETLOG7(g_Log, "����(%u) [%s] ����(0x%08x) ST:%d USPCheckBilling_CharIDLogin ȣ�� ���� �� ���. %u, %u [%s]",
//                UserID, AccountName, ServerID, Cmd, GetData.Flag, GetData.intCRMIndex1, inet_ntoa(IPAddress));
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
			}
/*
            else
            {
                DETLOG5(g_Log, "����(%u) [%s] ����(0x%08x) ST:%d USPCheckBilling_LogOut ȣ�� ���� �� ���. %u", 
                    UserID, AccountName, ServerID, Cmd, Return);
            }
*/
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
            }
/*
            else
            {
                DETLOG5(g_Log, "����(%u) [%s] ����(0x%08x) ST:%d USPCheckBilling_LogOut ȣ�� ���� �� ���. %u", 
                    UserID, AccountName, ServerID, Cmd, Return);
            }
*/
		}
		break;
    };

	// ��÷��� ���� ó�� ��ƾ
	switch(Cmd)
	{
	case PktUUT::UpdateUIDTableUserLogin:
	case PktUUT::UpdateUIDTableBillingCheck:
		{
            // �븸 ����
            PAY_AUTH PayAuth = {0,};  
                            
			// 12�д� 20����Ʈ�� ����ó���� �ϴµ�
			// PayAUthMyth �����ȿ� ���� �ű⼭ 12, 20�̶� ���ڸ� ������ ó���� �Ѵ�.
            if (!DBComponent::BillingDB::PayAuthMyth(CDBSingleObject::GetInstance(), 
                AccountName, inet_ntoa(IPAddress), &PayAuth))
            {
                SERLOG6(g_Log, "����(%u) [%s] ����(0x%08x) ST:%d PayAuthMyth ȣ�� ���� %s : %s", 
                    UserID, AccountName, ServerID, Cmd, inet_ntoa(IPAddress), 
                    CDBSingleObject::GetInstance().GetErrorString());

                if(PktUUT::UpdateUIDTableUserLogin == Cmd)
                {
                    // ���� �α����̸�, �α��� ���!

                    unsigned long Return = 0;
			        if(!DBComponent::BillingDB::USPCheckBilling_LogOut(CDBSingleObject::GetInstance(), AccountName, &Return))
			        {
                        SERLOG5(g_Log, "����(%u) [%s] ����(0x%08x) ST:%d PayAuthMyth ȣ�� ���з� �α׾ƿ���. USPCheckBilling_LogOut ȣ�� ���� : %s",
                            UserID, AccountName, ServerID, Cmd, CDBSingleObject::GetInstance().GetErrorString());

				        return true;
			        }

			        if(0 != Return)
			        {
                        SERLOG5(g_Log, "����(%u) [%s] ����(0x%08x) ST:%d - ���� ���ν��� ȣ�� ���� �α׾ƿ� - USPCheckBilling_LogOut ȣ�� ���� ����. %u", 
                            UserID, AccountName, ServerID, Cmd, Return);
			        }
                    else
                    {
                        DETLOG5(g_Log, "����(%u) [%s] ����(0x%08x) ST:%d - ���� ���ν��� ȣ�� ���� �α׾ƿ� -  USPCheckBilling_LogOut ȣ�� ���� �� ���. %u", 
                            UserID, AccountName, ServerID, Cmd, Return);
                    }
                }

                return true;
            }
                            
            // ������Ʈ Ack
            if(FALSE == CServerSetup::GetInstance().GetIgnoreFlag())
            {
                if(0 != PayAuth.Errcode && Cmd == PktUUT::UpdateUIDTableUserLogin)
                {
                    // ���� �α����̸�, �α��� ���!

                    unsigned long Return = 0;
			        if(!DBComponent::BillingDB::USPCheckBilling_LogOut(CDBSingleObject::GetInstance(), AccountName, &Return))
			        {
                        SERLOG6(g_Log, "����(%u) [%s] ����(0x%08x) ST:%d PayAuthMyth(Err:%d) USPCheckBilling_LogOut ȣ�� ���� : %s",
                            UserID, AccountName, ServerID, Cmd, PayAuth.Errcode, CDBSingleObject::GetInstance().GetErrorString());

				        return true;
			        }

			        if(0 != Return)
			        {
                        SERLOG5(g_Log, "����(%u) [%s] ����(0x%08x) ST:%d - ���� ���ν��� ��� ���� �α׾ƿ� - USPCheckBilling_LogOut ȣ�� ���� ����. %u", 
                            UserID, AccountName, ServerID, Cmd, Return);
			        }
                    else
                    {
                        DETLOG5(g_Log, "����(%u) [%s] ����(0x%08x) ST:%d - ���� ���ν��� ��� ���� �α׾ƿ� - USPCheckBilling_LogOut ȣ�� ���� �� ���. %u", 
                            UserID, AccountName, ServerID, Cmd, Return);
                    }
                }

                AgentSendPacket::SendUpdateUIDTable(GetGroup(), Cmd, UserID, 
                    PayAuth.Errcode, PayAuth.Time, 0, 'Y', static_cast<unsigned short>(PayAuth.WarningCode), 0);
            }
            else
            {
                AgentSendPacket::SendUpdateUIDTable(GetGroup(), Cmd, UserID, 0, 0, 0, 'N', 0, 0);
            }
            
//            DETLOG7(g_Log, "����(%u) [%s] ����(0x%08x) ST:%d - PayAuthMyth ȣ�� ���� �� ���. Error:%u Time:%u, [%s]", 
//                UserID, AccountName, ServerID, Cmd, PayAuth.Errcode, PayAuth.Time, inet_ntoa(IPAddress));
		}
		break;

	case PktUUT::UpdateUIDTableCharLogin:
		{
			AgentSendPacket::SendUpdateUIDTable(GetGroup(), Cmd, UserID, 0, 0, 0, 'N', 0, 0);
		}
		break;

	case PktUUT::UpdateUIDTableUserLogout:
	case PktUUT::UpdateUIDTableCharLogout:
	case PktUUT::UpdateUIDTableUserMove:
	case PktUUT::UpdateUIDTableCharMove:
		break;
	}

    return true;
}


bool CUIDAgentDispatch::ProcessDisconnectYouxiLand()
{
    unsigned long dwResult = 0;

    if(!DBComponent::BillingDB::USPServer_End(CDBSingleObject::GetInstance(), m_Group, &dwResult))
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
