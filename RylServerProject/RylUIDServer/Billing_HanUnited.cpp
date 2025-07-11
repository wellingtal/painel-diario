#include "stdafx.h"
#include "UserIDTable.h"
#include "UIDAgentDispatch.h"
#include "SendAgentPacket.h"

#include "HanUnitedBilling.h"
#include "HanUnitedDisconnID.h"

#include <Log/ServerLog.h>
#include <Network/Packet/PacketStruct/ServerPacket.h>
#include <Utility/Setup/ServerSetup.h>
#include <DB/DBComponent.h>
#include <DB/BillingDBComponent.h>

#include <Utility/Debug/PerformanceCheck.h>

// �Ѱ��� ���պ��� ó��. ���� �����ϰ� ��ġ���� �۾��� �� �� ���� --;;
bool CUIDAgentDispatch::ProcessBillingHanUnited(PktUUT* lpPktUUT)
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
			if(!DBComponent::BillingDB::USPCheckBilling_Login_Post(CDBSingleObject::GetInstance(), 
                AccountName, UserID, Check, inet_ntoa(IPAddress), GetGroup(), &GetData))
			{
                SERLOG6(g_Log, "����(%u) [%s] ����(0x%08x) ST:%d USPCheckBilling_Login_Post ȣ�� ���� %s : %s", 
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

                if(2 <= GetData.Flag)
                {
                    // ���� ������ ��� - ���պ������� üũ�� ������.
                    GET_SINGLE_DISPATCH(lpHanUnitedDispatch, CHanUnitedDispatch, 
                        CHanUnitedDispatch::GetDispatchTable());

                    if(0 == lpHanUnitedDispatch || 
                        !lpHanUnitedDispatch->SendCanLogin(*lpPktUUT, GetData, GetGroup()))
                    {
                        SERLOG5(g_Log, "ID:%s/IP:%s/UID:%u/CID:%u/ServerID:0x%08x/���պ������� ���� ���� ���� ���� ����.",
                            lpPktUUT->m_strAccount, inet_ntoa(lpPktUUT->m_IPAddress), 
                            lpPktUUT->m_dwUserID, lpPktUUT->m_dwCharID, ServerID);

                        AgentSendPacket::SendUpdateUIDTable(GetGroup(), Cmd, UserID, 4, 0, 0, 'N', 0, 0);
                    }
                }
                else
                {
                    // DB���� ���� ���� ���� ������.
				    AgentSendPacket::SendUpdateUIDTable(GetGroup(), Cmd, 
                        UserID, GetData.Flag, GetData.PlayTime, GetData.intCRMIndex1, 
                        GetData.strBillingType[0], 0, lpPktUUT->GetError());
                }
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

            DETLOG7(g_Log, "����(%u) [%s] ����(0x%08x) ST:%d USPCheckBilling_Login_Post ȣ�� ���� �� ���. %u, %u [%s]", 
                UserID, AccountName, ServerID, Cmd, GetData.Flag, GetData.intCRMIndex1, inet_ntoa(IPAddress));
		}
		break;

	case PktUUT::UpdateUIDTableCharLogin:
		{
            CPerformanceInstrument charLogin("CharLogin");
	        CAutoInstrument autoInstrument(charLogin);
    
			// Ű�� ���� ó��
			RE_USPCheckBilling_Login GetData = {0,};

			char Check = CServerSetup::GetInstance().GetFreeCheck() ? 1 : 0; // �⺻(=0), �������(=1)
			if(!DBComponent::BillingDB::USPCheckBilling_CharIDLogin_Post(CDBSingleObject::GetInstance(), 
                AccountName, UserID, Check, inet_ntoa(IPAddress), GetGroup(), &GetData))
			{
                SERLOG6(g_Log, "����(%u) [%s] ����(0x%08x) ST:%d USPCheckBilling_CharIDLogin_Post ȣ�� ���� %s : %s", 
                    UserID, AccountName, ServerID, Cmd, inet_ntoa(IPAddress), CDBSingleObject::GetInstance().GetErrorString());

                // DB Query����(Ŭ���̾�Ʈ���� ���� 1�� (�������� ������ �����ϴ�) �߻�
                AgentSendPacket::SendUpdateUIDTable(GetGroup(), Cmd, UserID, 4, 0, 0, 'N', 0, 0);
				return true;
			}

			// ������Ʈ Ack
			if(FALSE == CServerSetup::GetInstance().GetIgnoreFlag())
			{
                if(2 <= GetData.Flag)
                {
                    // ���� ������ ��� - ���պ������� üũ�� ������.
                    GET_SINGLE_DISPATCH(lpHanUnitedDispatch, CHanUnitedDispatch, 
                        CHanUnitedDispatch::GetDispatchTable());

                    if(0 == lpHanUnitedDispatch || 
                        !lpHanUnitedDispatch->SendLogin(*lpPktUUT, GetData, GetGroup()))
                    {
                        SERLOG5(g_Log, "ID:%s/IP:%s/UID:%u/CID:%u/ServerID:0x%08x/���պ������� �α��� ���� ����.",
                            lpPktUUT->m_strAccount, inet_ntoa(lpPktUUT->m_IPAddress), 
                            lpPktUUT->m_dwUserID, lpPktUUT->m_dwCharID, ServerID);

                        AgentSendPacket::SendUpdateUIDTable(GetGroup(), Cmd, UserID, 4, 0, 0, 'N', 0, 0);
                    }
                }
                else
                {
                    // DB���� ���� ���� ���� ������.
				    AgentSendPacket::SendUpdateUIDTable(GetGroup(), Cmd, 
                        UserID, GetData.Flag, GetData.PlayTime, GetData.intCRMIndex1, GetData.strBillingType[0], 0, 0);
                }
			}
			else
			{
				AgentSendPacket::SendUpdateUIDTable(GetGroup(), Cmd, 
					UserID, 0, GetData.PlayTime, GetData.intCRMIndex1, GetData.strBillingType[0], 0, 0);
			}

            DETLOG7(g_Log, "����(%u) [%s] ����(0x%08x) ST:%d USPCheckBilling_CharIDLogin_Post ȣ�� ���� �� ���. %u, %u [%s]",
                UserID, AccountName, ServerID, Cmd, GetData.Flag, GetData.intCRMIndex1, inet_ntoa(IPAddress));
		}
		break;

	case PktUUT::UpdateUIDTableUserLogout:
	case PktUUT::UpdateUIDTableCharLogout:

        // �� �̻� ������ �������� �ʾƵ� �ȴ�.
        CHanUnitedDisconnectID::GetInstance().RemoveDisconnectInfo(UserID);

	case PktUUT::UpdateUIDTableUserMove:
	case PktUUT::UpdateUIDTableCharMove:

		{
            CPerformanceInstrument logout("Logout");
	        CAutoInstrument autoInstrument(logout);

			// Ű�� ó��
			unsigned long Return = 0;

			if(!DBComponent::BillingDB::USPCheckBilling_LogOut_Post(CDBSingleObject::GetInstance(), AccountName, &Return))
			{
                SERLOG5(g_Log, "����(%u) [%s] ����(0x%08x) ST:%d USPCheckBilling_LogOut_Post ȣ�� ���� : %s", 
                    UserID, AccountName, ServerID, Cmd, CDBSingleObject::GetInstance().GetErrorString());
				return true;
			}

			if(0 != Return && 10 != Return)
			{
                SERLOG5(g_Log, "����(%u) [%s] ����(0x%08x) ST:%d USPCheckBilling_LogOut_Post ȣ�� ���� ����. %u", 
                    UserID, AccountName, ServerID, Cmd, Return);				
			}
            else
            {
                DETLOG5(g_Log, "����(%u) [%s] ����(0x%08x) ST:%d USPCheckBilling_LogOut_Post ȣ�� ���� �� ���. %u", 
                    UserID, AccountName, ServerID, Cmd, Return);

                if(10 == Return)
                {
                    if(!CHanUnitedLogout::GetInstance().AddLogout(*lpPktUUT))
                    {
                        SERLOG5(g_Log, "ID:%s/IP:%s/UID:%u/CID:%u/ServerID:0x%08x/���պ������� �α׾ƿ� ������ ����.",
                            lpPktUUT->m_strAccount, inet_ntoa(lpPktUUT->m_IPAddress), 
                            lpPktUUT->m_dwUserID, lpPktUUT->m_dwCharID, ServerID);
                    }
                }
            }
		}
		break;

    default:
        SERLOG1(g_Log, "�� �� ���� Ŀ�ǵ�(%d)�� ���Խ��ϴ�.", Cmd);
		break;
	}

    return true;
}

bool CUIDAgentDispatch::ProcessDisconnectHanUnited()
{
    // �Ѱ��� �����̳�, �Ѱ��� ���պ����� ���.
	unsigned long Result = 0;

    // TODO : �α׾ƿ� ó��.
	if(!DBComponent::BillingDB::USPDisConnectLogOut_Post(CDBSingleObject::GetInstance(), m_Group))
	{
        SERLOG3(g_Log, "%d �׷� �α׾ƿ� ����. �����:0x%08x DB���� �α�:%s",
            m_Group, Result, CDBSingleObject::GetInstance().GetErrorString());				
	}
	else
	{
#pragma pack(1)

        struct HanUnitedBillingUser
        {
            char    m_szID[20];
            char    m_szIP[15];
        };

#pragma pack()

        GET_SINGLE_DISPATCH(lpHanUnitedDispatch, CHanUnitedDispatch, 
            CHanUnitedDispatch::GetDispatchTable());

        HanUnitedBillingUser hanUnitedBillingUser[OleDB::MaxRowNum];
        memset(&hanUnitedBillingUser, 0, sizeof(HanUnitedBillingUser) * OleDB::MaxRowNum);

        PktUUT pktUUT;
        memset(&pktUUT, 0, sizeof(PktUUT));

        int nGetRow = 0;
        while(CDBSingleObject::GetInstance().GetData((void**)hanUnitedBillingUser, 
            sizeof(HanUnitedBillingUser), OleDB::MaxRowNum, &nGetRow))
        {
            if(0 == nGetRow)
            {
                break;
            }

            HanUnitedBillingUser* lpBillingUser = hanUnitedBillingUser;
            HanUnitedBillingUser* lpBillingUserEnd = hanUnitedBillingUser + nGetRow;

            for(; lpBillingUser != lpBillingUserEnd; ++lpBillingUser)
            {
                _snprintf(pktUUT.m_strAccount, 
                    PktUUT::MaxAccountLen, "%s", lpBillingUser->m_szID);
                pktUUT.m_strAccount[PktUUT::MaxAccountLen - 1] = 0;

                pktUUT.m_IPAddress.S_un.S_addr = inet_addr(lpBillingUser->m_szIP);
                
                if(!CHanUnitedLogout::GetInstance().AddLogout(pktUUT))
                {
                    SERLOG3(g_Log, "ID:%s/IP:%s/ServerGroup:%d/������ ���� �������� ���պ������� �α׾ƿ� ������ ����.",
                        lpBillingUser->m_szID, lpBillingUser->m_szIP, m_Group);
                }
            }

            memset(&hanUnitedBillingUser, 0, sizeof(HanUnitedBillingUser) * OleDB::MaxRowNum);
        }

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

