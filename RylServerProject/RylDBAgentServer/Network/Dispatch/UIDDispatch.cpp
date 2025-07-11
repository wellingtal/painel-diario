#include "stdafx.h"
#include "UIDDispatch.h"

#include "AuthDispatch.h"
#include "GameDispatch.h"
#include "ChatDispatch.h"

#include <DB/DBComponent.h>
#include <DB/GameDBComponent.h>

#include <Log/ServerLog.h>
#include <Network/Address/INET_Addr.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>
#include <Network/Packet/PacketStruct/ServerPacket.h>
#include <Network/Packet/PacketStruct/CharLoginOutPacket.h>
#include <Network/Packet/PacketStruct/CharLoginOutPacketStruct.h>
#include <Network/SendPacket/SendServerInfo.h>
#include <Network/SendPacket/SendLoginout.h>
#include <Network/SendPacket/SendCharManage.h>

#include <DataStorage/Billing.h>
#include <DataStorage/SessionData.h>
#include <DataStorage/SessionDataMgr.h>

#include <Utility/Setup/ServerSetup.h>

namespace DBAgent
{

const char* g_szPktUUTString[PktUUT::MAX_UPDATE_TYPE] =
{
    "UpdateUIDTableNone",
    "UpdateUIDTableUserLogin",
    "UpdateUIDTableUserLogout",
    "UpdateUIDTableUserMove",
    "UpdateUIDTableCharLogin",
    "UpdateUIDTableCharLogout",
    "UpdateUIDTableCharMove",
    "UpdateUIDTableBillingCheck",
};



// UpdateUIDTable처리시 유저 로그인 처리를 한다.
// 리턴값은 상수 에러 문자열이다. NULL이면 성공이다.
const char* ProcessUpdateTableUserLogin(DataStorage::CSessionData& sessionData, 
                                        DataStorage::RequestData& requestData, 
                                        unsigned long dwFlag, int nPlayTime, 
                                        unsigned long dwCRMIndex1,
                                        unsigned char cCmd, char cBillingType, 
                                        unsigned short usUUKAckError);

// UpdateUIDTable처리시 캐릭터 로그인 처리를 한다.
// 리턴값은 상수 에러 문자열이다. NULL이면 성공이다.
const char* ProcessUpdateTableCharLogin(DataStorage::CSessionData& sessionData, 
                                        DataStorage::RequestData& requestData, 
                                        unsigned long dwFlag, int nPlayTime, 
                                        unsigned long dwCRMIndex1, 
                                        unsigned char cCmd, char cBillingType);

CSingleDispatch& CUIDDispatch::GetDispatchTable()
{
    static CSingleDispatch uidDispatch;
    return uidDispatch;
}

CUIDDispatch::CUIDDispatch(CSession& Session)
    :   CRylServerDispatch(Session, MAX_PACKET_DISPATCH_PER_PULSE)
{


}

CUIDDispatch::~CUIDDispatch()
{


}

void CUIDDispatch::Connected()
{
    DETLOG3(g_Log, "SS:0x%08x/DP:0x%08x/IP:%15s/UIDServer Connected",
        &GetSession(), this, GetRemoteAddr().get_addr_string());

    GetDispatchTable().SetDispatch(this);

    SendPacket::ServerLogin(GetSendStream(), 
        CServerSetup::GetInstance().GetServerID());
}

void CUIDDispatch::Disconnected()
{
    DETLOG3(g_Log, "SS:0x%08x/DP:0x%08x/IP:%15s/UIDServer Disconnected",
        &GetSession(), this, GetRemoteAddr().get_addr_string());

    GetDispatchTable().RemoveDispatch(this);
}

bool CUIDDispatch::DispatchPacket(PktBase* lpPktBase)
{
    PktBase::CMDType cCmd = lpPktBase->GetCmd();

    bool bResult = false;

    switch(cCmd)
    {
    case CmdSysServerLogout:        bResult = ParseServerLogoutAck(lpPktBase);                              break;
    case CmdUserKill:               bResult = ParseUserKill(static_cast<PktUK*>(lpPktBase));                break;
    case CmdBillingTimeoutNotify:   bResult = ParseBillingTimeNotify(static_cast<PktBTN*>(lpPktBase));      break;
    case CmdBillingTimeCheckNotify: bResult = ParseBillingTimeCheckNotify(static_cast<PktBTN*>(lpPktBase)); break;
    case CmdHanBTNWarning:          bResult = ParseHanBTNWarning(static_cast<PktHanBTN*>(lpPktBase));       break;
    case CmdHanBTNUserKill:         bResult = ParseHanBTNUserKill(static_cast<PktHanUserKill*>(lpPktBase)); break;
    case CmdUpdateUIDTable:         bResult = ParseUpdateUIDTable(static_cast<PktUUTAck*>(lpPktBase));      break;

    default:

        ERRLOG4(g_Log, "SS:0x%08x/DP:0x%08x/IP:%15s/Cmd:0x%02X/ UID서버 패킷 처리 실패 : 없는 커맨드입니다",
            &GetSession(), this, GetRemoteAddr().get_addr_string(), cCmd);

        bResult = true;
        break;
    }

    if(!bResult)
    {
        ERRLOG4(g_Log, "SS:0x%08x/DP:0x%08x/IP:%15s/Cmd:0x%02X/ UID서버 패킷 처리 실패 : 처리를 실패했습니다",
            &GetSession(), this, GetRemoteAddr().get_addr_string(), cCmd);
    }

    return true;
}


bool CUIDDispatch::ParseServerLogoutAck(PktBase* lpPktBase)
{
    INFLOG3(g_Log, "SS:0x%08x/DP:0x%08x/IP:%15s/Cmd:0x%02X/ UID서버 접속 종료 : 서버 로그아웃 패킷을 받았습니다.",
        &GetSession(), this, GetRemoteAddr().get_addr_string());

    CloseSession();
    return true;
}


bool CUIDDispatch::ParseUpdateUIDTable(PktUUTAck* lpPktUUTAck)
{    
	// edith 2008.01.15 UID에서 보낸 UpdateUIDTable 결과값 처리루틴

    using namespace DBAgent::DataStorage;

    unsigned char   cCmd		        = lpPktUUTAck->m_cCmd;
    unsigned long   dwUID               = lpPktUUTAck->m_dwUserID;
    unsigned long   dwFlag		        = lpPktUUTAck->m_dwFlag;
    int             nPlayTime	        = lpPktUUTAck->m_nPlayTime;
    unsigned long   dwCRMIndex1         = lpPktUUTAck->m_dwCRMIndex1;       // 피시방 ID (0이면 개인유저)
    char            cBillingType	    = lpPktUUTAck->m_cstrBillingType;

    CBilling::WarningType eOldWarnMsgType = CBilling::WARN_BEFORE_1SEC;
    CBilling::WarningType eNewWarnMsgType = static_cast<CBilling::WarningType>(lpPktUUTAck->GetState());

    const char*     szErrorReason       = 0;

    RequestData requestData;
    memset(&requestData, 0, sizeof(RequestData));

    CSessionData* lpSessionData = CSessionDataMgr::GetInstance().GetOpenSession(dwUID);

    if(PktUUT::MAX_UPDATE_TYPE <= cCmd)
    {
        szErrorReason = "로그인 실패 - 알 수 없는 커맨드입니다";
    }
    else if(0 == lpSessionData)
    {
        szErrorReason = "로그인 실패 - 세션이 없습니다";
    }
    else
    {
        switch(cCmd)
        {
        case PktUUT::UpdateUIDTableUserLogin:

            requestData = lpSessionData->PopRequest();
            
            if(0 == requestData.m_dwRequestKey)
            {
                szErrorReason = "로그인 실패 - RequestKey가 이상합니다";
            }
            else
            {
                szErrorReason = ProcessUpdateTableUserLogin(*lpSessionData, requestData, dwFlag, 
                    nPlayTime, dwCRMIndex1, cCmd, cBillingType, lpPktUUTAck->GetError());

                eOldWarnMsgType = lpSessionData->GetBilling().GetWarnMsgType();
                lpSessionData->GetBilling().SetWarnMsgType(eNewWarnMsgType);

                if(CBilling::WARN_EVERY_MINUTE == eNewWarnMsgType)
                {
                    // 경고 상태로 이행. 매분마다 메시지를 보내도록 한다.
                    SendPacket::BillingCheckNotify(*lpSessionData, 5, cBillingType);
                }
            }
            break;

        case PktUUT::UpdateUIDTableCharLogin:

            requestData = lpSessionData->PopRequest();
            
            if(0 == requestData.m_dwRequestKey)
            {
                szErrorReason = "로그인 실패 - RequestKey가 이상합니다";
            }
            else
            {
                szErrorReason = ProcessUpdateTableCharLogin(*lpSessionData, requestData, 
                    dwFlag, nPlayTime, dwCRMIndex1, cCmd, cBillingType);
            }
            break;

        case PktUUT::UpdateUIDTableBillingCheck:
            
            eOldWarnMsgType = lpSessionData->GetBilling().GetWarnMsgType();

            if(0 == dwFlag)
            {
                lpSessionData->GetBilling().SetWarnMsgType(eNewWarnMsgType);

                if (CBilling::NO_WARNING == eOldWarnMsgType && 
                    CBilling::WARN_EVERY_MINUTE == eNewWarnMsgType)
                {
                    // 무경고 상태에서 경고 상태로 이행. 매분마다 메시지를 보내도록 한다.
                    SendPacket::BillingCheckNotify(*lpSessionData, 5, cBillingType);
                }
                else 
                    if (CBilling::WARN_EVERY_MINUTE == eOldWarnMsgType && 
                        CBilling::NO_WARNING == eNewWarnMsgType)
                    {
                        // 경고 상태에서 무경고 상태로 이행. 메시지를 그만 보내도록 한다.
                        SendPacket::BillingCheckNotify(*lpSessionData, 20, cBillingType);
                    }
            }

            lpSessionData->GetBilling().AddYouxiLandPoint(nPlayTime, cBillingType);

            DETLOG4(g_Log, "UID:%10u / CID:%10u / nPlayTime:%d / cBillingType:%c / UpdateUIDTableBillingCheck 성공",
                lpSessionData->GetUID(), lpSessionData->GetCID(), nPlayTime, cBillingType);

            if (0 != dwFlag)
            {
                // 유저 킬 처리 - 특별히 하지 않는다. 나중에 빌링체크에서 끊을 것이다.
                // KillUser(lpDataSession);
            }

            break;
        }

    }

    if(0 != szErrorReason)
    {
        ERRLOG10(g_Log, "UID:%10u / Cmd:%s / Flag:%u / PlayTime:%d / CRMIndex:%u / BillingType:%c / "
            " RequestKey:%10u / SelectedCID:%10u / ServerID:0x%08X / ParseUIDTable 실패 : %s", 
            dwUID, g_szPktUUTString[cCmd], dwFlag, nPlayTime, dwCRMIndex1, cBillingType, 
            requestData.m_dwRequestKey, requestData.m_dwSelectedCID, 
            requestData.m_dwServerID, szErrorReason);

        if(0 == dwFlag)
        {
            PktUUT::UpdateType eUpdateType = PktUUT::UpdateUIDTableNone;

            switch(cCmd)
            {
            case PktUUT::UpdateUIDTableUserLogin:
                eUpdateType = PktUUT::UpdateUIDTableUserLogout;
                break;

            case PktUUT::UpdateUIDTableCharLogin:
                eUpdateType = PktUUT::UpdateUIDTableCharLogout;
                break;
            }

			// edith 2008.01.15 UID로 로그인 메시지 전송
            if(0 == lpSessionData)
            {
                ERRLOG1(g_Log, "UID:%10u / UID서버 전송 실패 : 세션이 없습니다.", dwUID);
            }
            else if(eUpdateType != PktUUT::UpdateUIDTableNone &&
				!SendPacket::UpdateUIDTable(eUpdateType, requestData.m_szAccount, requestData.m_szPassword,
                dwUID, requestData.m_dwSelectedCID, requestData.m_dwSessionID, 
                requestData.m_dwServerID, requestData.m_PeerAddress))
            {
                ERRLOG1(g_Log, "UID:%10u / UID서버 전송 실패 : 전송에 실패했습니다.", dwUID);
            }
        }
    }


    return true;
}

const char* ProcessUpdateTableUserLogin(DataStorage::CSessionData& sessionData, 
                                        DataStorage::RequestData& requestData, 
                                        unsigned long dwFlag, int nPlayTime, unsigned long dwCRMIndex1,
                                        unsigned char cCmd, char cBillingType, unsigned short usUUKAckError)
{
	// edith 2008.01.15 UpdateTableUserLogin에 관련된 프로세싱 로직
    const char* szErrorReason = 0; 
    unsigned long dwUID = sessionData.GetUID();

    GET_SINGLE_DISPATCH(lpAuthDispatch, 
        CAuthDispatch, CAuthDispatch::GetDispatchTable());

    if(0 == lpAuthDispatch)
    {
        szErrorReason = "유저 로그인 실패 - 인증서버 접속 끊김";
    }
    else
    {
        unsigned short usError = 1;  // Default : Server Error(Unknown Error)

        if (0 == dwFlag)
        {
            // 통합서버 정보를 얻어서 보내준다.
            unsigned char cAgentServerType = static_cast<unsigned char>(
                CServerSetup::GetInstance().GetAgentServerType());

            unsigned char cFirstLogin = 0;

            switch(cAgentServerType)
            {
            case UnifiedConst::Part1:
            case UnifiedConst::Part1Unified:
                // 서버 세팅 잘못 했음. 나중에 UserLogin
                szErrorReason = "유저 로그인 실패 - 서버 잘못 켰음";
                break;

            case UnifiedConst::ROW:                
                sessionData.SetOldServerGroupID(UnifiedConst::ROW);
                break;

            case UnifiedConst::Part2Unified:
            case UnifiedConst::Part2Selectable:

                if(!sessionData.GetUnifiedDataFromDB(CDBSingleObject::GetInstance()))
                {                 
                    szErrorReason = "유저 로그인 실패 - 통합서버 데이터 얻기 실패";
                }
                break;
            }

            cFirstLogin = sessionData.GetFirstLogin();

            if(0 != szErrorReason)
            {
                // 데이터 읽어오다 에러 난 모양이다. UserLogin을 취소한다.
            }
            else if(!sessionData.UserEnable(CDBSingleObject::GetInstance(), dwUID))
            {
                szErrorReason = "유저 로그인 실패 - 유저 활성화 실패";
            }
            else if(!SendPacket::StartSession(lpAuthDispatch->GetSendStream(),
                requestData.m_dwRequestKey, dwUID, requestData.m_dwSessionID, cFirstLogin, 0))
            {
                szErrorReason = "유저 로그인 실패 - 유저 로그인 성공 전송 실패";
            }
            else
            {
                // 빌링 시작(접속 끊기 시작. 예전 한게임 과금과 요시랜드 과금만 사용함).
                // 인증서버에서는 과금 안함. 과금 정보 표시를 위해서 세팅함.
                sessionData.GetBilling().StartBilling(nPlayTime, dwCRMIndex1, cBillingType);
                
				// edith 2008.01.15 로그인에 성공했으니 세션에 등록한다.
                // 로그인 성공했을때만 세션 ID등의 데이터를 세팅한다.                
                sessionData.SetRequestData(requestData);

                // 세션 열기 성공
                usError = 0;
                
                // StartSession을 받았을 때, 에러가 0이고, 중계서버 타입이 통합서버이면,
                // 통합서버 데이터를 기다리므로, 데이터를 여기서 전송해 준다.

                switch(cAgentServerType)
                {
                case UnifiedConst::Part2Unified:
                case UnifiedConst::Part2Selectable:

                    // 신규 계정인지 아닌지 생각할 필요는 없다. 일단 정보 주고 나면
                    // 선택할 캐릭이 없으면 알아서 그냥 로그인하기 때문이다.
                    if (!SendPacket::UnifiedCharInfo(lpAuthDispatch->GetSendStream(), 
                        dwUID, sessionData.GetTransferedCharCount(CDBSingleObject::GetInstance()),
                        sessionData.GetUserInfo(),                         
                        sessionData.GetUnifiedStoreInfo(), sessionData.GetUnifiedStoreInfoNum(),
                        sessionData.GetUnifiedCharData(), sessionData.GetUnifiedCharDataNum()))
                    {
                        // 정보 전송에 실패했으면 뭐 하는 수 없지;; 알아서 데이터 기다리다 끊어지겠지;;
                        ERRLOG1(g_Log, "UID:%10u / 유저 로그인 실패 - 통합서버 정보 전송 실패", dwUID);
                    }
                    break;
                }
            }
        }		
        else if (1 == dwFlag) 
        {
            switch(usUUKAckError)
            {
            default:
                usError = 41; // 중복로그인
                break;

            case PktUUT::DISCONNECT_USER:
                usError = 42; // Disconnect를 했으니 재시도해 봐..
                break;
            }			
        }
        else if (2 == dwFlag) { usError = 24; } // 개인,피시방 관련 과금 정보가 없음(한게임)
        else if (4 == dwFlag) { usError = 1;  } // 서버 에러 (DB Query 실패)
        else if (11 == dwFlag || 13 == dwFlag || 19 == dwFlag)
        {
            // 개인,피시방 관련 과금 정보가 없음(대만)
            usError = 24;   
        }
        else if (20 == dwFlag) { usError = 40; } // 대만, 일본 중복 로그인 방지
		// edith 2009.09.11 MY를 위한 AllowIP 처리작업
		else if (57 == dwFlag) { usError = 57; }	// ROW에서 AllowIP 에서 걸림
		else if (100 >= dwFlag) { usError = (unsigned short)(dwFlag-100); }; // 감마니아 오류

        if(0 != usError)
        {
            SendPacket::StartSession(lpAuthDispatch->GetSendStream(),
                requestData.m_dwRequestKey, dwUID, sessionData.GetSessionID(), 0, usError);
        }
    }

    return szErrorReason;
}



const char* ProcessUpdateTableCharLogin(DataStorage::CSessionData& sessionData, 
                                        DataStorage::RequestData& requestData, 
                                        unsigned long dwFlag, int nPlayTime, 
                                        unsigned long dwCRMIndex1, 
                                        unsigned char cCmd, char cBillingType)
{
    const char* szErrorReason = 0;
    unsigned long dwUID = sessionData.GetUID();

    GET_MULTI_DISPATCH(lpGameDispatch, requestData.m_dwServerID, 
        CGameDispatch, CGameDispatch::GetDispatchTable());

    if(0 == lpGameDispatch)
    {
        szErrorReason = "캐릭터 로그인 실패 - 게임서버 접속 끊김";
    }
    else if(!sessionData.HasCharacter(requestData.m_dwSelectedCID))
    {
        szErrorReason = "캐릭터 로그인 실패 - 소유하지 않은 캐릭터 활성화";

        const USER_INFO& userInfo = sessionData.GetUserInfo();

		// WORK_LIST 2.1 계정 국적 추가
		SERLOG9(g_Log, "UID:%10u / SelectedCID:%10u / Char1:%10u / Char2:%10u / Char3:%10u / Char4:%10u / Char5:%10u / Nation:%d %s", 
            dwUID, requestData.m_dwSelectedCID, 
            userInfo.CharID[0], userInfo.CharID[1], userInfo.CharID[2], 
			userInfo.CharID[3], userInfo.CharID[4], userInfo.Nation, szErrorReason);
    }
    else
    {
        // 기본값은 실패. 서버 에러.
        unsigned short usError = 1;

        if (0 == dwFlag)
        {
            // 캐릭터 활성화			
            if (!sessionData.CharEnable(requestData.m_dwSelectedCID, requestData.m_dwServerID))
            {
                szErrorReason = "캐릭터 로그인 실패 - 세션 활성화 실패";
            }
            // 캐릭터 데이터 전송
            else if (!SendPacket::CharLogin(lpGameDispatch->GetSendStream(), 
                sessionData, requestData.m_dwRequestKey))
            {
                szErrorReason = "캐릭터 로그인 실패 - 캐릭터 데이터 전송 실패";

                // 활성화된 캐릭터를 Disable함.
                sessionData.CharDisable(requestData.m_dwSelectedCID, requestData.m_dwServerID);
            }
            else 
            {
                // 빌링 시작
                switch(CServerSetup::GetInstance().GetBillingType())
                {
                case CServerSetup::GamaBilling:
                case CServerSetup::GamaUnitedBilling:                
                    sessionData.GetBilling().StartBilling(nPlayTime, dwCRMIndex1, cBillingType);
                    break;
                }

                // 로그인 성공했을때만 세션 ID등의 데이터를 세팅한다.
                sessionData.SetRequestData(requestData);

                // 접속 성공!
                usError = 0;    
            }
        }
        else if (1 == dwFlag) { usError = 41; }  // 중복로그인
        else if (2 == dwFlag) { usError = 24; }  // 개인,피시방 관련 과금 정보가 없음
        else if (4 == dwFlag) { usError = 1;  }  // 서버 에러 (DB Query 실패)
        else if (11 == dwFlag || 13 == dwFlag || 19 == dwFlag)
        {
            // 개인,피시방 관련 과금 정보가 없음(대만)
            usError = 24;    
        }
        else if (20 == dwFlag) { usError = 40; }  // 대만, 일본 중복 로그인 방지
		// edith 2009.09.11 MY를 위한 AllowIP 처리작업
		else if (57 == dwFlag) { usError = 57; }	// ROW에서 AllowIP 에서 걸림
		else if (100 >= dwFlag) { usError = (unsigned short)(dwFlag-100); }; // 감마니아 오류

        if (0 != usError)
        {            
            SendPacket::CharLoginError(lpGameDispatch->GetSendStream(), 
                requestData.m_dwRequestKey, DBUpdateData::LOGIN, 
                dwUID, requestData.m_dwSelectedCID, usError);
        }
    }

    return szErrorReason;
}


bool CUIDDispatch::ParseBillingTimeNotify(PktBTN* lpPktBTN)
{
    using namespace DataStorage;

    CSessionData* lpSessionData = CSessionDataMgr::GetInstance().GetOpenSession(lpPktBTN->m_dwUserID);
    if(0 == lpSessionData)
    {
        ERRLOG2(g_Log, "UID:%10u / cRemainMin:%u / 시간 만료 공지를 보내기 실패 : 유저가 없습니다",
            lpPktBTN->m_dwUserID, lpPktBTN->m_cRemainMinute);
    }
    else if(CSessionData::SE_CHAR_ENABLED != lpSessionData->GetSessionState())
    {
        ERRLOG2(g_Log, "UID:%10u / ST:%s / 시간 만료 공지를 보내기 실패 : 캐릭터가 비활성화되어 있습니다",
            lpPktBTN->m_dwUserID, g_szSessionStateString[lpSessionData->GetSessionState()]);
    }
    else
    {
        // 시간이 다 떨어져서 경고 보냄.
        if(!SendPacket::BillingNotify(*lpSessionData, 
            lpPktBTN->m_cRemainMinute, lpPktBTN->m_cBillingType))
        {
            INFLOG4(g_Log, "UID:%10u / ServerID:0x%08X / cRemainMin:%u / cBillingType:%c / 시간 만료 공지 보내기 성공",
                lpPktBTN->m_dwUserID, lpSessionData->GetServerID(), lpPktBTN->m_cRemainMinute, lpPktBTN->m_cBillingType);
        }
        else
        {
            ERRLOG2(g_Log, "UID:%10u / ServerID:0x%08X / 시간 만료 공지를 보내기 실패 : 게임서버와의 연결이 끊어져 있습니다",
                lpPktBTN->m_dwUserID, lpSessionData->GetServerID());
        }
    }

    return true;
}

bool CUIDDispatch::ParseBillingTimeCheckNotify(PktBTN* lpPktBTN)
{
    using namespace DataStorage;

    CSessionData* lpSessionData = CSessionDataMgr::GetInstance().GetOpenSession(lpPktBTN->m_dwUserID);
    if(0 == lpSessionData)
    {
        ERRLOG2(g_Log, "UID:%10u / cRemainMin:%u / 시간 다시 체크(대만) 공지를 보내기 실패 : 유저가 없습니다",
            lpPktBTN->m_dwUserID, lpPktBTN->m_cRemainMinute);
    }
    else if(CSessionData::SE_CHAR_ENABLED != lpSessionData->GetSessionState())
    {
        ERRLOG2(g_Log, "UID:%10u / ST:%s / 시간 다시 체크(대만) 공지를 보내기 실패 : 캐릭터가 비활성화되어 있습니다",
            lpPktBTN->m_dwUserID, g_szSessionStateString[lpSessionData->GetSessionState()]);
    }
    else
    {
        // 시간이 다 떨어져서 경고 보냄.
        if(!SendPacket::BillingCheckNotify(*lpSessionData, 
            lpPktBTN->m_cRemainMinute, lpPktBTN->m_cBillingType))
        {
            INFLOG4(g_Log, "UID:%10u / ServerID:0x%08X / cRemainMin:%u / cBillingType:%c / 시간 다시 체크(대만) 공지 보내기 성공",
                lpPktBTN->m_dwUserID, lpSessionData->GetServerID(), lpPktBTN->m_cRemainMinute, lpPktBTN->m_cBillingType);
        }
        else
        {
            ERRLOG2(g_Log, "UID:%10u / ServerID:0x%08X / 시간 다시 체크 공지(대만)를 보내기 실패 : 게임서버와의 연결이 끊어져 있습니다",
                lpPktBTN->m_dwUserID, lpSessionData->GetServerID());
        }
    }

    return true;
}


bool CUIDDispatch::ParseHanBTNWarning(PktHanBTN* lpPktHanBTN)
{
    // 채팅서버로 메시지를 Relay한다.
    using namespace DataStorage;

    IN_ADDR peerAddress;
    peerAddress.S_un.S_addr = lpPktHanBTN->m_dwIP;

    bool bCheckAddress = (0xFFFFFFFF != peerAddress.S_un.S_addr);

    CSessionData* lpSessionData = CSessionDataMgr::GetInstance().GetOpenSession(
        lpPktHanBTN->m_szAccount, peerAddress, bCheckAddress);
    
    if(0 == lpSessionData)
    {
        ERRLOG2(g_Log, "AccountName:%16s / IP:%15s / 과금 만료 메시지 전송 실패 : 유저가 없습니다",
            lpPktHanBTN->m_szAccount, inet_ntoa(peerAddress));
    }
    else if(CSessionData::SE_CHAR_ENABLED != lpSessionData->GetSessionState())
    {
        ERRLOG4(g_Log, "UID:%10u / AccountName:%16s / IP:%15s / ST:%s / 과금 만료 메시지 전송 실패 : 캐릭터가 비활성화되어 있습니다",
            lpSessionData->GetUID(), lpPktHanBTN->m_szAccount, inet_ntoa(peerAddress), 
            g_szSessionStateString[lpSessionData->GetSessionState()]);
    }
    else 
    {
        GET_SINGLE_DISPATCH(lpChatDispatch, 
            CChatDispatch, CChatDispatch::GetDispatchTable());

        if(0 != lpChatDispatch)
        {
            char* lpBuffer = lpChatDispatch->GetSendStream().GetBuffer(sizeof(PktHanBTN));

            if(0 != lpBuffer)
            {
                PktHanBTN* lpPktChatBTN = reinterpret_cast<PktHanBTN*>(lpBuffer);

                *lpPktChatBTN = *lpPktHanBTN;

                lpPktChatBTN->m_dwUID = lpSessionData->GetUID();
                lpPktChatBTN->m_dwCID = lpSessionData->GetCID();

                return lpChatDispatch->GetSendStream().WrapHeader(
                    sizeof(PktHanBTN), CmdHanBTNWarning, 0, 0);
            }
        }

        ERRLOG5(g_Log, "UID:%10u / AccountName:%16s / IP:%15s / ST:%s / ChatDispatch:%p / 과금 만료 메시지 전송 실패 : 채팅서버와 연결이 끊겼거나, 버퍼 할당 실패",
            lpSessionData->GetUID(), lpPktHanBTN->m_szAccount, inet_ntoa(peerAddress), 
            g_szSessionStateString[lpSessionData->GetSessionState()], lpChatDispatch);
    }

    return true;
}

bool CUIDDispatch::ParseHanBTNUserKill(PktHanUserKill* lpPktHanUserKill)
{
    using namespace DataStorage;

    IN_ADDR peerAddress;
    peerAddress.S_un.S_addr = lpPktHanUserKill->m_dwIP;

    // 접속 DB를 전부 뒤져서, 계정과 IP가 일치하는 넘한테 뜨거운 맛을 보여 준다.
    bool bCheckAddress = (peerAddress.S_un.S_addr != 0xFFFFFFFF);

    CSessionData* lpSessionData = CSessionDataMgr::GetInstance().GetOpenSession(
        lpPktHanUserKill->m_szAccount, peerAddress, bCheckAddress);

    if(0 == lpSessionData)
    {
        ERRLOG2(g_Log, "AccountName:%16s / IP:%15s / 과금 만료 접속 끊기 실패 : 유저가 없습니다",
            lpPktHanUserKill->m_szAccount, inet_ntoa(peerAddress));
    }
    else 
	{
		SERVER_ID serverID;
		serverID.dwID = lpSessionData->GetServerID();
		CSessionData::SessionState eSessionState = lpSessionData->GetSessionState();

		if ((serverID.GetType() == CServerSetup::AuthServer && CSessionData::SE_USER_ENABLED != eSessionState) ||
			(serverID.GetType() == CServerSetup::GameServer && CSessionData::SE_CHAR_ENABLED != eSessionState))
		{
			ERRLOG3(g_Log, "UID:%10u / ServerID:0x%08X / ST:%s / 과금 만료 접속 끊기 실패 : 유저나 캐릭터가 비활성화되어 있습니다", 
				lpSessionData->GetUID(), serverID.dwID, g_szSessionStateString[lpSessionData->GetSessionState()]);
		}
		else if(!SendPacket::UserKill(*lpSessionData))
		{
			ERRLOG3(g_Log, "UID:%10u / ServerID:0x%08X / ST:%s / 과금 만료 접속 끊기 실패 : 접속 끊기 패킷 보내기 실패", 
				lpSessionData->GetUID(), serverID.dwID, g_szSessionStateString[lpSessionData->GetSessionState()]);
		}
	}

    return true;
}

bool CUIDDispatch::ParseUserKill(PktUK* lpPktUK)
{
    using namespace DataStorage;

    // 접속 DB를 전부 뒤져서, 계정과 IP가 일치하는 넘한테 뜨거운 맛을 보여 준다.
    CSessionData* lpSessionData = 
		CSessionDataMgr::GetInstance().GetOpenSession(lpPktUK->m_dwUserID);

    if(0 == lpSessionData)
    {
        ERRLOG1(g_Log, "UID:%10u / UID 서버 유저 접속 끊기 실패 : 유저가 없습니다", lpPktUK->m_dwUserID);
    }
    else 
	{
		SERVER_ID serverID;
		serverID.dwID = lpSessionData->GetServerID();
		CSessionData::SessionState eSessionState = lpSessionData->GetSessionState();

		if ((serverID.GetType() == CServerSetup::AuthServer && CSessionData::SE_USER_ENABLED != eSessionState) ||
			(serverID.GetType() == CServerSetup::GameServer && CSessionData::SE_CHAR_ENABLED != eSessionState))
		{
			ERRLOG3(g_Log, "UID:%10u / ServerID:0x%08X / ST:%s / UID 서버 유저 접속 끊기 실패 : 유저나 캐릭터가 비활성화되어 있습니다", 
				lpPktUK->m_dwUserID, serverID.dwID, g_szSessionStateString[lpSessionData->GetSessionState()]);
		}
		else if(!SendPacket::UserKill(*lpSessionData))
		{
			ERRLOG3(g_Log, "UID:%10u / ServerID:0x%08X / ST:%s / UID 서버 유저 접속 끊기 실패 : 접속 끊기 패킷 보내기 실패", 
				lpPktUK->m_dwUserID, serverID.dwID, g_szSessionStateString[lpSessionData->GetSessionState()]);
		}
	}

    return true;
}


}


