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

// 한게임 기존 처리 루틴. DB관련 실패시 에러 주는 부분만 새로 삽입하고,
// 코드 정리를 하여야 한다.
bool CUIDAgentDispatch::ProcessBillingHan(PktUUT* lpPktUUT)
{
	unsigned char Cmd		= lpPktUUT->m_cCmd;
	unsigned long ServerID	= lpPktUUT->m_dwServerID;
    unsigned long SessionID	= lpPktUUT->m_dwSessionID;
	unsigned long UserID	= lpPktUUT->m_dwUserID;
	unsigned long CharID	= lpPktUUT->m_dwCharID;
	char *AccountName		= lpPktUUT->m_strAccount;
	IN_ADDR& IPAddress		= lpPktUUT->m_IPAddress;

	// 한게임 처리 루틴
	switch(Cmd) 
	{
	case PktUUT::UpdateUIDTableUserLogin:
		{
            CPerformanceInstrument userLogin("UserLogin");
	        CAutoInstrument autoInstrument(userLogin);

			// 키퍼 관련 처리
			RE_USPCheckBilling_Login GetData = {0,};

			char Check = CServerSetup::GetInstance().GetFreeCheck() ? 1 : 0; // 기본(=0), 무료계정(=1)
			if(!DBComponent::BillingDB::USPCheckBilling_Login(CDBSingleObject::GetInstance(), 
                AccountName, UserID, Check, inet_ntoa(IPAddress), GetGroup(), &GetData))
			{
                SERLOG6(g_Log, "유저(%u) [%s] 서버(0x%08x) ST:%d USPCheckBilling_Login 호출 실패 %s : %s", 
                    UserID, AccountName, ServerID, Cmd, inet_ntoa(IPAddress), CDBSingleObject::GetInstance().GetErrorString());

                // DB Query실패(클라이언트에는 에러 1번 (서버에서 에러가 났습니다) 발생
                AgentSendPacket::SendUpdateUIDTable(GetGroup(), Cmd, UserID, 4, 0, 0, 'N', 0, 0);
				return true;
			}

			// 업데이트 Ack
			if(FALSE == CServerSetup::GetInstance().GetIgnoreFlag())
			{
                if(1 == GetData.Flag && PktUUT::DISCONNECT_USER == lpPktUUT->GetError())
				{
                    // 유저가 이미 접속해 있고, 유저 죽이기 플래그가 오면, 유저를 죽인다.
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
                    // 유저가 이미 접속해 있고, 유저 죽이기 플래그가 오면, 유저를 죽인다.
                    AgentSendPacket::SendHanUnitedUserKill(GetData.intCRMIndex1, AccountName, 0);
                }

                // 무조건 성공시킨다. 단 중복로그인이면 로그인 못하도록 한다.
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

            DETLOG7(g_Log, "유저(%u) [%s] 서버(0x%08x) ST:%d USPCheckBilling_Login 호출 리턴 값 출력. %u, %u [%s]", 
                UserID, AccountName, ServerID, Cmd, GetData.Flag, GetData.intCRMIndex1, inet_ntoa(IPAddress));

			IncreaseUnitNum();
		}
		break;

	case PktUUT::UpdateUIDTableCharLogin:
		{
            CPerformanceInstrument charLogin("CharLogin");
	        CAutoInstrument autoInstrument(charLogin);

			// 키퍼 관련 처리
			RE_USPCheckBilling_Login GetData = {0,};

			char Check = CServerSetup::GetInstance().GetFreeCheck() ? 1 : 0; // 기본(=0), 무료계정(=1)
			if(!DBComponent::BillingDB::USPCheckBilling_CharIDLogin(CDBSingleObject::GetInstance(), 
                AccountName, UserID, Check, inet_ntoa(IPAddress), GetGroup(), &GetData))
			{
                SERLOG6(g_Log, "유저(%u) [%s] 서버(0x%08x) ST:%d USPCheckBilling_CharIDLogin 호출 실패 %s : %s", 
                    UserID, AccountName, ServerID, Cmd, inet_ntoa(IPAddress), CDBSingleObject::GetInstance().GetErrorString());

                // DB Query실패(클라이언트에는 에러 1번 (서버에서 에러가 났습니다) 발생
                AgentSendPacket::SendUpdateUIDTable(GetGroup(), Cmd, UserID, 4, 0, 0, 'N', 0, 0);
				return true;
			}

			// 업데이트 Ack
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

            DETLOG7(g_Log, "유저(%u) [%s] 서버(0x%08x) ST:%d USPCheckBilling_CharIDLogin 호출 리턴 값 출력. %u, %u [%s]",
                UserID, AccountName, ServerID, Cmd, GetData.Flag, GetData.intCRMIndex1, inet_ntoa(IPAddress));

			IncreaseUnitNum();
		}
		break;

	case PktUUT::UpdateUIDTableUserLogout:
	case PktUUT::UpdateUIDTableCharLogout:
		{
            CPerformanceInstrument logout("Logout");
	        CAutoInstrument autoInstrument(logout);

			// 키퍼 처리
			unsigned long Return = 0;

			if(!DBComponent::BillingDB::USPCheckBilling_LogOut(CDBSingleObject::GetInstance(), AccountName, &Return))
			{
                SERLOG5(g_Log, "유저(%u) [%s] 서버(0x%08x) ST:%d USPCheckBilling_LogOut 호출 실패 : %s", 
                    UserID, AccountName, ServerID, Cmd, CDBSingleObject::GetInstance().GetErrorString());
				return true;
			}

			if(0 != Return)
			{
                SERLOG5(g_Log, "유저(%u) [%s] 서버(0x%08x) ST:%d USPCheckBilling_LogOut 호출 리턴 에러. %u", 
                    UserID, AccountName, ServerID, Cmd, Return);
				return true;
			}

            DETLOG5(g_Log, "유저(%u) [%s] 서버(0x%08x) ST:%d USPCheckBilling_LogOut 호출 리턴 값 출력. %u", 
                UserID, AccountName, ServerID, Cmd, Return);

			DecreaseUnitNum();
		}
		break;

	case PktUUT::UpdateUIDTableUserMove:
	case PktUUT::UpdateUIDTableCharMove:
		{
            CPerformanceInstrument move("ZoneMove");
	        CAutoInstrument autoInstrument(move);

			// 키퍼 처리
			unsigned long Return = 0;

			if(!DBComponent::BillingDB::USPCheckBilling_LogOut(CDBSingleObject::GetInstance(), AccountName, &Return))
			{
                SERLOG5(g_Log, "유저(%u) [%s] 서버(0x%08x) ST:%d USPCheckBilling_LogOut 호출 실패 : %s",
                    UserID, AccountName, ServerID, Cmd, CDBSingleObject::GetInstance().GetErrorString());
				return true;
			}

			if(0 != Return)
			{
                SERLOG5(g_Log, "유저(%u) [%s] 서버(0x%08x) ST:%d USPCheckBilling_LogOut 호출 리턴 에러. %u", 
                    UserID, AccountName, ServerID, Cmd, Return);
				return true;
			}

            DETLOG5(g_Log, "유저(%u) [%s] 서버(0x%08x) ST:%d USPCheckBilling_LogOut 호출 리턴 값 출력. %u", 
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
    // 한게임 빌링이나, 한게임 통합빌링인 경우.
	unsigned long Result = 0;

	if(!DBComponent::BillingDB::USPDisConnectLogOut(CDBSingleObject::GetInstance(), m_Group, &Result))
	{
        SERLOG3(g_Log, "%d 그룹 로그아웃 실패. 결과값:0x%08x DB에러 로그:%s",
            m_Group, Result, CDBSingleObject::GetInstance().GetErrorString());				
	}
	else
	{
        SERLOG1(g_Log, "%d 그룹 USPDisConnectLogOut 호출 성공", m_Group);

		if(!DBComponent::BillingDB::USPServer_End(CDBSingleObject::GetInstance(), m_Group, &Result))
		{
			SERLOG3(g_Log, "%d 그룹 로그아웃 실패. 결과값:0x%08x DB에러 로그:%s", 
				m_Group, Result, CDBSingleObject::GetInstance().GetErrorString());
		}
		else
		{
			SERLOG1(g_Log, "%d 그룹 USEServer_End 호출 성공", m_Group);
		}
	}

    return true;
}

