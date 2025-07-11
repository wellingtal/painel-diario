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

// 대만의 과금타입은, 매 30분마다 시간을 제하는 방식으로 진행된다.
// 시간을 제하는 루틴은, 중계서버에서 진행한다.
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

    // 새 중복로그인 및 로그인로그아웃 로그 루틴
    switch(Cmd)
    {
    case PktUUT::UpdateUIDTableUserLogin:
        {
            CPerformanceInstrument userLogin("UserLogin");
	        CAutoInstrument autoInstrument(userLogin);

			// 키퍼 관련 처리
			RE_USPCheckBilling_Login GetData = {0,};
			
			if(!DBComponent::BillingDB::USPCheckBilling_Login(CDBSingleObject::GetInstance(), 
                AccountName, UserID, 0, inet_ntoa(IPAddress), GetGroup(), &GetData))
			{
                SERLOG6(g_Log, "유저(%u) [%s] 서버(0x%08x) ST:%d USPCheckBilling_Login 호출 실패 %s : %s", 
                    UserID, AccountName, ServerID, Cmd, inet_ntoa(IPAddress), CDBSingleObject::GetInstance().GetErrorString());

                // DB Query실패(클라이언트에는 에러 1번 (서버에서 에러가 났습니다) 발생
                AgentSendPacket::SendUpdateUIDTable(GetGroup(), Cmd, UserID, 4, 0, 0, 'N', 0, 0);
				return true;
			}

			// 업데이트 Ack

            if(0 != GetData.Flag)
            {
                AgentSendPacket::SendUpdateUIDTable(GetGroup(), Cmd, UserID, 20, 0, 0, 'N', 0, 0);
                return true;
            }

//            DETLOG7(g_Log, "유저(%u) [%s] 서버(0x%08x) ST:%d USPCheckBilling_Login 호출 리턴 값 출력. %u, %u [%s]", 
//                UserID, AccountName, ServerID, Cmd, GetData.Flag, GetData.intCRMIndex1, inet_ntoa(IPAddress));
        }
        break;

    case PktUUT::UpdateUIDTableCharLogin:
        {
            CPerformanceInstrument charLogin("CharLogin");
	        CAutoInstrument autoInstrument(charLogin);

			// 키퍼 관련 처리
			RE_USPCheckBilling_Login GetData = {0,};
			
			if(!DBComponent::BillingDB::USPCheckBilling_CharIDLogin(CDBSingleObject::GetInstance(), 
                AccountName, UserID, 0, inet_ntoa(IPAddress), GetGroup(), &GetData))
			{
                SERLOG6(g_Log, "유저(%u) [%s] 서버(0x%08x) ST:%d USPCheckBilling_CharIDLogin 호출 실패 %s : %s", 
                    UserID, AccountName, ServerID, Cmd, inet_ntoa(IPAddress), CDBSingleObject::GetInstance().GetErrorString());

                // DB Query실패(클라이언트에는 에러 1번 (서버에서 에러가 났습니다) 발생
                AgentSendPacket::SendUpdateUIDTable(GetGroup(), Cmd, UserID, 4, 0, 0, 'N', 0, 0);
				return true;
			}

			// 업데이트 Ack
            if(0 != GetData.Flag)
            {
                AgentSendPacket::SendUpdateUIDTable(GetGroup(), Cmd, UserID, 20, 0, 0, 'N', 0, 0);
                return true;
            }

//            DETLOG7(g_Log, "유저(%u) [%s] 서버(0x%08x) ST:%d USPCheckBilling_CharIDLogin 호출 리턴 값 출력. %u, %u [%s]",
//                UserID, AccountName, ServerID, Cmd, GetData.Flag, GetData.intCRMIndex1, inet_ntoa(IPAddress));
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
			}
/*
            else
            {
                DETLOG5(g_Log, "유저(%u) [%s] 서버(0x%08x) ST:%d USPCheckBilling_LogOut 호출 리턴 값 출력. %u", 
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
            }
/*
            else
            {
                DETLOG5(g_Log, "유저(%u) [%s] 서버(0x%08x) ST:%d USPCheckBilling_LogOut 호출 리턴 값 출력. %u", 
                    UserID, AccountName, ServerID, Cmd, Return);
            }
*/
		}
		break;
    };

	// 요시랜드 과금 처리 루틴
	switch(Cmd)
	{
	case PktUUT::UpdateUIDTableUserLogin:
	case PktUUT::UpdateUIDTableBillingCheck:
		{
            // 대만 빌링
            PAY_AUTH PayAuth = {0,};  
                            
			// 12분당 20포인트를 과금처리를 하는듯
			// PayAUthMyth 쿼리안에 가면 거기서 12, 20이란 숫자를 가지고 처리를 한다.
            if (!DBComponent::BillingDB::PayAuthMyth(CDBSingleObject::GetInstance(), 
                AccountName, inet_ntoa(IPAddress), &PayAuth))
            {
                SERLOG6(g_Log, "유저(%u) [%s] 서버(0x%08x) ST:%d PayAuthMyth 호출 실패 %s : %s", 
                    UserID, AccountName, ServerID, Cmd, inet_ntoa(IPAddress), 
                    CDBSingleObject::GetInstance().GetErrorString());

                if(PktUUT::UpdateUIDTableUserLogin == Cmd)
                {
                    // 유저 로그인이면, 로그인 취소!

                    unsigned long Return = 0;
			        if(!DBComponent::BillingDB::USPCheckBilling_LogOut(CDBSingleObject::GetInstance(), AccountName, &Return))
			        {
                        SERLOG5(g_Log, "유저(%u) [%s] 서버(0x%08x) ST:%d PayAuthMyth 호출 실패로 로그아웃함. USPCheckBilling_LogOut 호출 실패 : %s",
                            UserID, AccountName, ServerID, Cmd, CDBSingleObject::GetInstance().GetErrorString());

				        return true;
			        }

			        if(0 != Return)
			        {
                        SERLOG5(g_Log, "유저(%u) [%s] 서버(0x%08x) ST:%d - 빌링 프로시저 호출 실패 로그아웃 - USPCheckBilling_LogOut 호출 리턴 에러. %u", 
                            UserID, AccountName, ServerID, Cmd, Return);
			        }
                    else
                    {
                        DETLOG5(g_Log, "유저(%u) [%s] 서버(0x%08x) ST:%d - 빌링 프로시저 호출 실패 로그아웃 -  USPCheckBilling_LogOut 호출 리턴 값 출력. %u", 
                            UserID, AccountName, ServerID, Cmd, Return);
                    }
                }

                return true;
            }
                            
            // 업데이트 Ack
            if(FALSE == CServerSetup::GetInstance().GetIgnoreFlag())
            {
                if(0 != PayAuth.Errcode && Cmd == PktUUT::UpdateUIDTableUserLogin)
                {
                    // 유저 로그인이면, 로그인 취소!

                    unsigned long Return = 0;
			        if(!DBComponent::BillingDB::USPCheckBilling_LogOut(CDBSingleObject::GetInstance(), AccountName, &Return))
			        {
                        SERLOG6(g_Log, "유저(%u) [%s] 서버(0x%08x) ST:%d PayAuthMyth(Err:%d) USPCheckBilling_LogOut 호출 실패 : %s",
                            UserID, AccountName, ServerID, Cmd, PayAuth.Errcode, CDBSingleObject::GetInstance().GetErrorString());

				        return true;
			        }

			        if(0 != Return)
			        {
                        SERLOG5(g_Log, "유저(%u) [%s] 서버(0x%08x) ST:%d - 빌링 프로시저 결과 실패 로그아웃 - USPCheckBilling_LogOut 호출 리턴 에러. %u", 
                            UserID, AccountName, ServerID, Cmd, Return);
			        }
                    else
                    {
                        DETLOG5(g_Log, "유저(%u) [%s] 서버(0x%08x) ST:%d - 빌링 프로시저 결과 실패 로그아웃 - USPCheckBilling_LogOut 호출 리턴 값 출력. %u", 
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
            
//            DETLOG7(g_Log, "유저(%u) [%s] 서버(0x%08x) ST:%d - PayAuthMyth 호출 리턴 값 출력. Error:%u Time:%u, [%s]", 
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
        SERLOG3(g_Log, "%d 그룹 로그아웃 실패. 결과값:0x%08x DB에러 로그:%s", 
            m_Group, dwResult, CDBSingleObject::GetInstance().GetErrorString());
    }
    else
    {
        SERLOG1(g_Log, "%d 그룹 USEServer_End 호출 성공", m_Group);
    }

    return true;
}
