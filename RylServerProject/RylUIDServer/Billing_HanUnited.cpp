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

// 한게임 통합빌링 처리. 아주 복잡하고도 골치아픈 작업이 될 거 같다 --;;
bool CUIDAgentDispatch::ProcessBillingHanUnited(PktUUT* lpPktUUT)
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
			if(!DBComponent::BillingDB::USPCheckBilling_Login_Post(CDBSingleObject::GetInstance(), 
                AccountName, UserID, Check, inet_ntoa(IPAddress), GetGroup(), &GetData))
			{
                SERLOG6(g_Log, "유저(%u) [%s] 서버(0x%08x) ST:%d USPCheckBilling_Login_Post 호출 실패 %s : %s", 
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

                if(2 <= GetData.Flag)
                {
                    // 빌링 실패한 경우 - 통합빌링으로 체크를 보낸다.
                    GET_SINGLE_DISPATCH(lpHanUnitedDispatch, CHanUnitedDispatch, 
                        CHanUnitedDispatch::GetDispatchTable());

                    if(0 == lpHanUnitedDispatch || 
                        !lpHanUnitedDispatch->SendCanLogin(*lpPktUUT, GetData, GetGroup()))
                    {
                        SERLOG5(g_Log, "ID:%s/IP:%s/UID:%u/CID:%u/ServerID:0x%08x/통합빌링으로 과금 가능 여부 전송 실패.",
                            lpPktUUT->m_strAccount, inet_ntoa(lpPktUUT->m_IPAddress), 
                            lpPktUUT->m_dwUserID, lpPktUUT->m_dwCharID, ServerID);

                        AgentSendPacket::SendUpdateUIDTable(GetGroup(), Cmd, UserID, 4, 0, 0, 'N', 0, 0);
                    }
                }
                else
                {
                    // DB에서 얻은 값을 전부 보낸다.
				    AgentSendPacket::SendUpdateUIDTable(GetGroup(), Cmd, 
                        UserID, GetData.Flag, GetData.PlayTime, GetData.intCRMIndex1, 
                        GetData.strBillingType[0], 0, lpPktUUT->GetError());
                }
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

            DETLOG7(g_Log, "유저(%u) [%s] 서버(0x%08x) ST:%d USPCheckBilling_Login_Post 호출 리턴 값 출력. %u, %u [%s]", 
                UserID, AccountName, ServerID, Cmd, GetData.Flag, GetData.intCRMIndex1, inet_ntoa(IPAddress));
		}
		break;

	case PktUUT::UpdateUIDTableCharLogin:
		{
            CPerformanceInstrument charLogin("CharLogin");
	        CAutoInstrument autoInstrument(charLogin);
    
			// 키퍼 관련 처리
			RE_USPCheckBilling_Login GetData = {0,};

			char Check = CServerSetup::GetInstance().GetFreeCheck() ? 1 : 0; // 기본(=0), 무료계정(=1)
			if(!DBComponent::BillingDB::USPCheckBilling_CharIDLogin_Post(CDBSingleObject::GetInstance(), 
                AccountName, UserID, Check, inet_ntoa(IPAddress), GetGroup(), &GetData))
			{
                SERLOG6(g_Log, "유저(%u) [%s] 서버(0x%08x) ST:%d USPCheckBilling_CharIDLogin_Post 호출 실패 %s : %s", 
                    UserID, AccountName, ServerID, Cmd, inet_ntoa(IPAddress), CDBSingleObject::GetInstance().GetErrorString());

                // DB Query실패(클라이언트에는 에러 1번 (서버에서 에러가 났습니다) 발생
                AgentSendPacket::SendUpdateUIDTable(GetGroup(), Cmd, UserID, 4, 0, 0, 'N', 0, 0);
				return true;
			}

			// 업데이트 Ack
			if(FALSE == CServerSetup::GetInstance().GetIgnoreFlag())
			{
                if(2 <= GetData.Flag)
                {
                    // 빌링 실패한 경우 - 통합빌링으로 체크를 보낸다.
                    GET_SINGLE_DISPATCH(lpHanUnitedDispatch, CHanUnitedDispatch, 
                        CHanUnitedDispatch::GetDispatchTable());

                    if(0 == lpHanUnitedDispatch || 
                        !lpHanUnitedDispatch->SendLogin(*lpPktUUT, GetData, GetGroup()))
                    {
                        SERLOG5(g_Log, "ID:%s/IP:%s/UID:%u/CID:%u/ServerID:0x%08x/통합빌링으로 로그인 전송 실패.",
                            lpPktUUT->m_strAccount, inet_ntoa(lpPktUUT->m_IPAddress), 
                            lpPktUUT->m_dwUserID, lpPktUUT->m_dwCharID, ServerID);

                        AgentSendPacket::SendUpdateUIDTable(GetGroup(), Cmd, UserID, 4, 0, 0, 'N', 0, 0);
                    }
                }
                else
                {
                    // DB에서 얻은 값을 전부 보낸다.
				    AgentSendPacket::SendUpdateUIDTable(GetGroup(), Cmd, 
                        UserID, GetData.Flag, GetData.PlayTime, GetData.intCRMIndex1, GetData.strBillingType[0], 0, 0);
                }
			}
			else
			{
				AgentSendPacket::SendUpdateUIDTable(GetGroup(), Cmd, 
					UserID, 0, GetData.PlayTime, GetData.intCRMIndex1, GetData.strBillingType[0], 0, 0);
			}

            DETLOG7(g_Log, "유저(%u) [%s] 서버(0x%08x) ST:%d USPCheckBilling_CharIDLogin_Post 호출 리턴 값 출력. %u, %u [%s]",
                UserID, AccountName, ServerID, Cmd, GetData.Flag, GetData.intCRMIndex1, inet_ntoa(IPAddress));
		}
		break;

	case PktUUT::UpdateUIDTableUserLogout:
	case PktUUT::UpdateUIDTableCharLogout:

        // 더 이상 접속을 해제하지 않아도 된다.
        CHanUnitedDisconnectID::GetInstance().RemoveDisconnectInfo(UserID);

	case PktUUT::UpdateUIDTableUserMove:
	case PktUUT::UpdateUIDTableCharMove:

		{
            CPerformanceInstrument logout("Logout");
	        CAutoInstrument autoInstrument(logout);

			// 키퍼 처리
			unsigned long Return = 0;

			if(!DBComponent::BillingDB::USPCheckBilling_LogOut_Post(CDBSingleObject::GetInstance(), AccountName, &Return))
			{
                SERLOG5(g_Log, "유저(%u) [%s] 서버(0x%08x) ST:%d USPCheckBilling_LogOut_Post 호출 실패 : %s", 
                    UserID, AccountName, ServerID, Cmd, CDBSingleObject::GetInstance().GetErrorString());
				return true;
			}

			if(0 != Return && 10 != Return)
			{
                SERLOG5(g_Log, "유저(%u) [%s] 서버(0x%08x) ST:%d USPCheckBilling_LogOut_Post 호출 리턴 에러. %u", 
                    UserID, AccountName, ServerID, Cmd, Return);				
			}
            else
            {
                DETLOG5(g_Log, "유저(%u) [%s] 서버(0x%08x) ST:%d USPCheckBilling_LogOut_Post 호출 리턴 값 출력. %u", 
                    UserID, AccountName, ServerID, Cmd, Return);

                if(10 == Return)
                {
                    if(!CHanUnitedLogout::GetInstance().AddLogout(*lpPktUUT))
                    {
                        SERLOG5(g_Log, "ID:%s/IP:%s/UID:%u/CID:%u/ServerID:0x%08x/통합빌링으로 로그아웃 보내기 실패.",
                            lpPktUUT->m_strAccount, inet_ntoa(lpPktUUT->m_IPAddress), 
                            lpPktUUT->m_dwUserID, lpPktUUT->m_dwCharID, ServerID);
                    }
                }
            }
		}
		break;

    default:
        SERLOG1(g_Log, "알 수 없는 커맨드(%d)가 들어왔습니다.", Cmd);
		break;
	}

    return true;
}

bool CUIDAgentDispatch::ProcessDisconnectHanUnited()
{
    // 한게임 빌링이나, 한게임 통합빌링인 경우.
	unsigned long Result = 0;

    // TODO : 로그아웃 처리.
	if(!DBComponent::BillingDB::USPDisConnectLogOut_Post(CDBSingleObject::GetInstance(), m_Group))
	{
        SERLOG3(g_Log, "%d 그룹 로그아웃 실패. 결과값:0x%08x DB에러 로그:%s",
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
                    SERLOG3(g_Log, "ID:%s/IP:%s/ServerGroup:%d/서버군 접속 끊김으로 통합빌링으로 로그아웃 보내기 실패.",
                        lpBillingUser->m_szID, lpBillingUser->m_szIP, m_Group);
                }
            }

            memset(&hanUnitedBillingUser, 0, sizeof(HanUnitedBillingUser) * OleDB::MaxRowNum);
        }

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

