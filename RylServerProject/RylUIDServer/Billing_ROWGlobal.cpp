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

// 대만의 과금타입은, 매 30분마다 시간을 제하는 방식으로 진행된다.
// 시간을 제하는 루틴은, 중계서버에서 진행한다.
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

	// 요시랜드 과금 처리 루틴
	switch(Cmd)
	{
	case PktUUT::UpdateUIDTableUserLogin:
		{
			CPerformanceInstrument userLogin("UserLogin");
			CAutoInstrument autoInstrument(userLogin);

#ifdef AUTH_MY
			// edith 2009.09.11 MY를 위한 AllowIP 처리작업
			char* szAdd = inet_ntoa(IPAddress);
			if(!g_IPSec.IsAliveIP(szAdd))
			{
				AgentSendPacket::SendUpdateUIDTable(GetGroup(), Cmd, UserID, 57, 0, 0, 'N', 0, 0);

				ERRLOG5(g_Log, "유저(%u) [%s] 서버(0x%08x) ST:%d Alive 호출 실패 %s", 
					UserID, AccountName, ServerID, Cmd, inet_ntoa(IPAddress));

				return true;
			}
#endif

			// 키퍼 관련 처리
			RE_USPCheckBilling_Login GetData = {0,};
			
			if(!DBComponent::BillingDB::USPCheckUser_Login(CDBSingleObject::GetInstance(), 
				AccountName, UserID, 0, inet_ntoa(IPAddress), GetGroup(), &GetData))
			{
				ERRLOG6(g_Log, "유저(%u) [%s] 서버(0x%08x) ST:%d USPCheckUser_Login 호출 실패 %s : %s", 
					UserID, AccountName, ServerID, Cmd, inet_ntoa(IPAddress), CDBSingleObject::GetInstance().GetErrorString());

				// DB Query실패(클라이언트에는 에러 1번 (서버에서 에러가 났습니다) 발생
//				AgentSendPacket::SendUpdateUIDTable(GetGroup(), Cmd, UserID, 4, 0, 0, 'N', 0, 0);
//				return true;
			}

			// ROW에선 요시랜드 인증을 사용하지 않는다.
			// ROW에선 자체요금제를 사용한다.
			AgentSendPacket::SendUpdateUIDTable(GetGroup(), Cmd, UserID, 0, 0, 0, 'N', 0, 0);
		}
		break;

	case PktUUT::UpdateUIDTableCharLogin:
		{
#ifdef AUTH_MY
			// edith 2009.09.11 MY를 위한 AllowIP 처리작업
			char* szAdd = inet_ntoa(IPAddress);
			if(!g_IPSec.IsAliveIP(szAdd))
			{
				AgentSendPacket::SendUpdateUIDTable(GetGroup(), Cmd, UserID, 30, 0, 0, 'N', 0, 0);

				ERRLOG5(g_Log, "유저(%u) [%s] 서버(0x%08x) ST:%d Alive 호출 실패 %s", 
					UserID, AccountName, ServerID, Cmd, inet_ntoa(IPAddress));

				return true;
			}
#endif
/*			CPerformanceInstrument userLogin("CharLogin");
			CAutoInstrument autoInstrument(userLogin);

			// 키퍼 관련 처리
			RE_USPCheckBilling_Login GetData = {0,};
			
			if(!DBComponent::BillingDB::USPCheckUser_CharIDLogin(CDBSingleObject::GetInstance(), 
				AccountName, UserID, 0, inet_ntoa(IPAddress), GetGroup(), &GetData))
			{
				ERRLOG6(g_Log, "유저(%u) [%s] 서버(0x%08x) ST:%d USPCheckUser_Login 호출 실패 %s : %s", 
					UserID, AccountName, ServerID, Cmd, inet_ntoa(IPAddress), CDBSingleObject::GetInstance().GetErrorString());

				// DB Query실패(클라이언트에는 에러 1번 (서버에서 에러가 났습니다) 발생
//				AgentSendPacket::SendUpdateUIDTable(GetGroup(), Cmd, UserID, 4, 0, 0, 'N', 0, 0);
//				return true;
			}
*/
			// ROW에선 요시랜드 인증을 사용하지 않는다.
			// ROW에선 자체요금제를 사용한다.
			AgentSendPacket::SendUpdateUIDTable(GetGroup(), Cmd, UserID, 0, 0, 0, 'N', 0, 0);
		}
		break;

	case PktUUT::UpdateUIDTableBillingCheck:
		// ROW에선 요시랜드 인증을 사용하지 않는다.
		// ROW에선 자체요금제를 사용한다.
		AgentSendPacket::SendUpdateUIDTable(GetGroup(), Cmd, UserID, 0, 0, 0, 'N', 0, 0);
		break;

	case PktUUT::UpdateUIDTableUserLogout:
//	case PktUUT::UpdateUIDTableCharLogout:
		{
			CPerformanceInstrument logout("Logout");
			CAutoInstrument autoInstrument(logout);

			// 키퍼 처리
			unsigned long Return = 0;

			if(!DBComponent::BillingDB::USPCheckUser_LogOut(CDBSingleObject::GetInstance(), AccountName, &Return))
			{
				ERRLOG5(g_Log, "유저(%u) [%s] 서버(0x%08x) ST:%d USPCheckUser_LogOut 호출 실패 : %s", 
					UserID, AccountName, ServerID, Cmd, CDBSingleObject::GetInstance().GetErrorString());
				return true;
			}

			if(0 != Return)
			{
				ERRLOG5(g_Log, "유저(%u) [%s] 서버(0x%08x) ST:%d USPCheckUser_LogOut 호출 리턴 에러. %u", 
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

			// 키퍼 처리
			unsigned long Return = 0;

			if(!DBComponent::BillingDB::USPCheckUser_LogOut(CDBSingleObject::GetInstance(), AccountName, &Return))
			{
				ERRLOG5(g_Log, "유저(%u) [%s] 서버(0x%08x) ST:%d USPCheckUser_LogOut 호출 실패 : %s", 
					UserID, AccountName, ServerID, Cmd, CDBSingleObject::GetInstance().GetErrorString());
				return true;
			}

			if(0 != Return)
			{
				ERRLOG5(g_Log, "유저(%u) [%s] 서버(0x%08x) ST:%d USPCheckUser_LogOut 호출 리턴 에러. %u", 
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
	// ROW에선 요시랜드 관련 인증을 사용하지 않기 때문에 무시한다.
    return true;
}
