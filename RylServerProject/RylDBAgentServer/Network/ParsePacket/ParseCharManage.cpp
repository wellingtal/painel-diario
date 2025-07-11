#include "stdafx.h"

#include <DataStorage/StoreDataMgr.h>
#include <DataStorage/CharacterDataMgr.h>
#include <DataStorage/SessionDataMgr.h>
#include <DataStorage/DBItemSerialMgr.h>

#include <Network/Stream/SendStream.h>
#include <Network/SendPacket/SendServerInfo.h>
#include <Network/SendPacket/SendLoginout.h>
#include <Network/SendPacket/SendCharManage.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>
#include <Network/Packet/PacketStruct/DataPacket.h>
#include <Network/Packet/PacketStruct/BGServerPacket.h>
#include <Network/Packet/PacketStruct/AuthServerToDBAgentServer.h>
#include <Network/Packet/PacketStruct/ClientToAuthServer.h>

#include <Network/Dispatch/Part1DBAgentDispatch.h>

// WORK_LIST 2.3 계정 국적 변경 기능 구현
#include <Network/SendPacket/SendGuild.h>

#include <DB/DBComponent.h>
#include <DB/GameDBComponent.h>

#include <Utility/Setup/ServerSetup.h>
#include <Creature/CreatureStructure.h>
#include <Creature/Character/CharacterClass.h>
#include <Log/ServerLog.h>

#include <Community/Guild/GuildDB.h>
#include <Community/Guild/GuildDBMgr.h>

#include "ParseCharManage.h"
#include <GameTime/GameTimeDBMgr.h>

#ifdef AUTH_MY
#include "NFAuthClient/AuthClient.h"
#endif


namespace DBAgent
{
namespace ParseCharManage
{

bool StartSession(CSendStream& SendStream, unsigned long dwServerID, PktSSD* lpPktSSD);
bool UserLogin(CSendStream& SendStream, PktULD* lpPktULD);
bool UserLogout(CSendStream& SendStream, PktULoD* lpPktULoD);
bool UserMove(CSendStream& SendStream, PktUMvD* lpPktUMvD);
bool CharSelect(CSendStream& SendStream, PktCSD* lpPktCSD);
bool CharCreate(CSendStream& SendStream, PktCCD* lpPktCCD);
bool CharDelete(CSendStream& SendStream, PktCDD* lpPktCDD);
bool CreateItem(CSendStream& SendStream, unsigned long dwServerID,
                Item::CItemSerialMgr& itemSerialMgr, PktCCID* lpPktCCID);
// WORK_LIST 2.1 계정 국적 추가
bool SelectAccountNation(CSendStream& SendStream, PktSAND* lpPktSAND);

bool Parse(CSendStream& SendStream, unsigned long dwServerID, 
           Item::CItemSerialMgr& itemSerialMgr, PktDD* lpPktDD)
{    
#ifdef ENABLE_PACKET_LOG
	unsigned short shCmd   = lpPktDD->m_wCmd;
	unsigned long dwRequestKey  = lpPktDD->m_dwRequestKey;
	unsigned long dwSessionID   = 0;
	unsigned long dwUID         = 0;
	unsigned long dwCID         = 0;

	bool bLog = true;
	switch (lpPktDD->m_wCmd) 
	{
	case PktDD::SCmdStartSession:	// 세션 시작
		{
			PktSSD* lpPktSSD = static_cast<PktSSD*>(lpPktDD);
			dwSessionID = lpPktSSD->m_dwSessionID;
			dwUID = lpPktSSD->m_dwUserID;
		}
		break;

	case PktDD::SCmdUserLogin:      // 유저 로그인 - 하는 일은 캐릭터 뷰 얻기 정도이다.
		{
			PktULD* lpPktULD = static_cast<PktULD*>(lpPktDD);
			dwSessionID = lpPktULD->m_dwSessionID;
			dwUID = lpPktULD->m_dwUserID;
		}
		break;

	case PktDD::SCmdUserLogout:		// 유저 로그아웃 - 세션 닫기.
		{
			PktULoD* lpPktULoD = static_cast<PktULoD*>(lpPktDD);
			dwSessionID = lpPktULoD->m_dwSessionID;
			dwUID = lpPktULoD->m_dwUserID;
		}
		break;

	case PktDD::SCmdUserMove:		// 유저 이동
		{
			PktUMvD* lpPktUMvD = static_cast<PktUMvD*>(lpPktDD);
			dwSessionID = lpPktUMvD->m_dwSessionID;
			dwUID = lpPktUMvD->m_dwUserID;
		}
		break;

		// WORK_LIST 2.1 계정 국적 추가
	case PktDD::SCmdSelectNation:	// 계정 국적 설정 및 변경
		{
			PktSAND* lpPktSAND = static_cast<PktSAND*>(lpPktDD);
			dwUID = lpPktSAND->m_dwUserID;
		}
		break;

	case PktDD::SCmdCharSelect:		// 캐릭터 선택
		{
			PktCSD* lpPktCSD = static_cast<PktCSD*>(lpPktDD);
			dwUID = lpPktCSD->m_dwUserID;
			dwCID = lpPktCSD->m_dwCharID;
		}
		break;

	case PktDD::SCmdCharCreate:		// 캐릭터 생성
		{
			PktCCD* lpPktCCD = static_cast<PktCCD*>(lpPktDD);
			dwUID = lpPktCCD->m_dwUserID;
		}
		break;

	case PktDD::SCmdCharDelete:		// 캐릭터 삭제
		{
			PktCDD* lpPktCCD = static_cast<PktCDD*>(lpPktDD);
			dwUID = lpPktCCD->m_dwUserID;
			dwCID = lpPktCCD->m_dwCharID;
		}
		break;

	case PktDD::SCmdCharCreateItem:	// 캐릭터 생성 아이템
		{
			PktCCID* lpPktCCID = static_cast<PktCCID*>(lpPktDD);
			dwCID = lpPktCCID->m_dwCharID;
		}
		break;

	default:
		bLog = false;
		break;
	}

	if(bLog)
	{
		DETLOG5(g_PacketLog, "ParseCharManage::Parse %d : UID:%10u / CID:%10u / SessionID:%10u / ServerID:0x%08X / ",
			shCmd, dwUID, dwCID, dwSessionID, dwServerID);
	}
#endif


	// 여기는 캐릭터 선택,삭제 화면에서
	// 게임시작 하기 전까지 하는곳인듯.
    bool bResult = false;
    
    switch (lpPktDD->m_wCmd) 
    {
    case PktDD::SCmdStartSession:	// 세션 시작
//      SERLOG0(g_Log, "세션시작");
        bResult = StartSession(SendStream, dwServerID, static_cast<PktSSD*>(lpPktDD));
        SERLOG0(g_Log, "세션시작 완료");
        break;

    case PktDD::SCmdUserLogin:      // 유저 로그인 - 하는 일은 캐릭터 뷰 얻기 정도이다.
//        SERLOG0(g_Log, "유저로그인 시도");
        bResult = UserLogin(SendStream, static_cast<PktULD*>(lpPktDD));
        SERLOG0(g_Log, "유저로그인 완료");
        break;

    case PktDD::SCmdUserLogout:		// 유저 로그아웃 - 세션 닫기.
//        SERLOG0(g_Log, "유저로그아웃 시도");
        bResult = UserLogout(SendStream, static_cast<PktULoD*>(lpPktDD));
        SERLOG0(g_Log, "유저로그아웃 완료");
        break;
    
    case PktDD::SCmdUserMove:		// 유저 이동
        bResult = UserMove(SendStream, static_cast<PktUMvD*>(lpPktDD));
        break;

	// WORK_LIST 2.1 계정 국적 추가
	case PktDD::SCmdSelectNation:	// 계정 국적 설정 및 변경
		bResult = SelectAccountNation(SendStream, static_cast<PktSAND*>(lpPktDD));
		break;

    case PktDD::SCmdCharSelect:		// 캐릭터 선택
        bResult = CharSelect(SendStream, static_cast<PktCSD*>(lpPktDD));
        break;

    case PktDD::SCmdCharCreate:		// 캐릭터 생성
        bResult = CharCreate(SendStream, static_cast<PktCCD*>(lpPktDD));
        break;

    case PktDD::SCmdCharDelete:		// 캐릭터 삭제
        bResult = CharDelete(SendStream, static_cast<PktCDD*>(lpPktDD));
        break;

    case PktDD::SCmdCharCreateItem:	// 캐릭터 생성 아이템
        bResult = CreateItem(SendStream, dwServerID, itemSerialMgr, static_cast<PktCCID*>(lpPktDD));
        break;

    default:        
        ERRLOG1(g_Log, "CharacterManage 패킷 처리 에러 : 알 수 없는 패킷 CMD:0x%02X를 받았습니다.", 
            lpPktDD->m_wCmd);

        bResult = true;
        break;
    }

    return bResult;
}


bool StartSession(CSendStream& SendStream, unsigned long dwServerID, PktSSD* lpPktSSD)
{
	// edith 2008.01.15 Auth 에서 세션 시작패킷이 날라왔다.
    unsigned long dwSessionID	= lpPktSSD->m_dwSessionID;
    unsigned long dwUID	        = lpPktSSD->m_dwUserID;
    unsigned long dwRequestKey  = lpPktSSD->m_dwRequestKey;

    IN_ADDR	peerAddress         = lpPktSSD->m_Address;	
    const char* szAccountName   = lpPktSSD->m_AccountName;
	const char* szPassword		= lpPktSSD->m_Password;

    unsigned short usFlag  = lpPktSSD->GetError();	// 접속 끊기.
    unsigned short usStartSessionError = 0;

    using namespace DataStorage;

    // 세션 열기
    CSessionData* lpSessionData = CSessionDataMgr::GetInstance().GetOpenSession(dwUID);
    
    if(0 != lpSessionData && 
        (CSessionData::SE_USER_ENABLED == lpSessionData->GetSessionState() ||
         CSessionData::SE_CHAR_ENABLED == lpSessionData->GetSessionState()))
    {
        // 이미 로그인 해 있으므로 접속 끊기 루틴 수행
        if(0 == usFlag)
        {
            // 접속 끊기 보내라고 요청
            usStartSessionError = 41;
        }
        else if(1 == usFlag)
        {
            // 같은 서버군에서 접속 끊기 처리를 한다.
            DETLOG5(g_Log, "UID:%10u / CID:%10u / ServerID:0x%08X / Account:%s / IP:%15s / 접속 끊기 전송 : 인증 서버 명령으로 접속 끊기 보냄.",
                lpSessionData->GetUID(), lpSessionData->GetCID(),
                lpSessionData->GetServerID(), lpSessionData->GetAccountName(), inet_ntoa(peerAddress));

            SendPacket::UserKill(*lpSessionData);

            // 재접속 시도 요청
            usStartSessionError = 42;
        }
        else
        {
            // 알 수 없는 경우. 그냥 세션 열기 실패로 간다.
            ERRLOG4(g_Log, "UID:%10u / CID:%10u / ServerID:0x%08X / usFlag:%d / 세션 열기 실패 : 클라이언트가 알 수 없는 플래그를 보냈습니다.",
                lpSessionData->GetUID(), lpSessionData->GetCID(), lpSessionData->GetServerID(), usFlag);

            usStartSessionError = 40;
        }
    }
    else if(0 == (lpSessionData = CSessionDataMgr::GetInstance().SessionOpen(dwUID)))
    {
        // 세션 열기 실패
        usStartSessionError = 40;
    }
    else
    {

#ifdef AUTH_MY
		// edith 2009.09.11 MY를 위한 AllowIP 처리작업
		char* szAdd = inet_ntoa(peerAddress);
		if(!g_IPSec.IsAliveIP(szAdd))
		{
			usStartSessionError = 57;

			ERRLOG4(g_Log, "DBAgent Error. IPSEC Error:%d Account:%s UID:%d (%s)", usStartSessionError, lpSessionData->GetAccountName(), lpSessionData->GetUID(), szAdd);
		}
#endif

		if(usStartSessionError == 0)
		{
			SERLOG4(g_Log, "UID:%10u / CID:%10u / ServerID:0x%08X / %s / UID로 인증 시도", 
				lpSessionData->GetUID(), lpSessionData->GetCID(), lpSessionData->GetServerID(), szAccountName);

			// 인증서버로 서버군 ID세팅한다.
			SERVER_ID serverID;

			serverID.sID.ID         = 0;
			serverID.sID.Channel    = 0;
			serverID.sID.Group      = CServerSetup::GetInstance().GetServerGroup();
			serverID.sID.Type       = CServerSetup::AuthServer;

			dwServerID = serverID.dwID;

			// edith 2008.01.15 세션을 열기에 성공했다. 리퀘스트 데이타를 세션에 입력한다. 설정후 UID로 로그인시도.
			// 패스워드를 저장하려면 여기까지 날아와야한다.
			// 세션 열기 성공
			lpSessionData->PushRequest(
				RequestData(dwRequestKey, 0, dwServerID, dwSessionID, peerAddress, szAccountName, szPassword));
	        
			// edith 2008.01.15 UID서버로 유저 로그인을 전송한다.
			if (!SendPacket::UpdateUIDTable(PktUUT::UpdateUIDTableUserLogin, 
				szAccountName, szPassword, dwUID, 0, dwSessionID, dwServerID, peerAddress,
				(1 == usFlag) ? PktUUT::DISCONNECT_USER : 0))
			{
				ERRLOG1(g_Log, "UID:%10u / UID서버에 유저 로그인 전송 실패", dwUID);

				lpSessionData->PopRequest();
				usStartSessionError = 1;
			}
			// 나중에 빼자
	//		INFLOG5(g_Log, "UID:%10u / Account:%s, Pass:%s, Session:0x%08X, ServerID:0x%08X ", 
	//            dwUID, szAccountName, szPassword, dwSessionID, dwServerID);
		}
    }

    if(0 != usStartSessionError)
    {
        if(!SendPacket::StartSession(SendStream, dwRequestKey, 
            dwUID, dwSessionID, 0, usStartSessionError))
        {
            ERRLOG2(g_Log, "UID:%10u / 인증서버에 세션 시작 실패 에러 전송 실패 : %u",
                dwUID, usStartSessionError);
        }        
    }

    return true;
}

bool UserLogin(CSendStream& SendStream, PktULD* lpPktULD)
{
    unsigned long   dwSessionID   = lpPktULD->m_dwSessionID;
    unsigned long   dwUID         = lpPktULD->m_dwUserID;
    IN_ADDR	        peerAddress	  = lpPktULD->m_Address;	
    unsigned char   cLoginType    = lpPktULD->m_cLoginType;
    unsigned long   dwRequestKey  = lpPktULD->m_dwRequestKey;

    unsigned short  usError         = 0;

    int             nPlayTime       = 0;
    unsigned char   cBillingUser    = 0;
    unsigned char   cBillingType    = 0;
    unsigned char   cAdminLevel     = 0;

    const CHAR_VIEW* lpCharView     = 0;
    unsigned long    dwCharViewNum  = 0;
	// WORK_LIST 2.1 계정 국적 추가
	unsigned char	cAccountNation	= 0;

	sGuildData	GuildData[PktULDAck::MAX_CHAR_VIEW];
	memset(&GuildData[0], 0, sizeof(sGuildData) * PktULDAck::MAX_CHAR_VIEW);

	STORE_INFO storeInfo;
	memset(&storeInfo, 0, sizeof(STORE_INFO));

    using namespace DataStorage;

    CSessionData* lpSessionData = CSessionDataMgr::GetInstance().GetOpenSession(dwUID);

    // 세션 얻기
    if(0 == lpSessionData)
    {
        ERRLOG1(g_Log, "UID:%10u / 유저 로그인 실패 : 세션이 없습니다", dwUID);

        usError = 1;
    }
    // 세션 ID검사
    else if(dwSessionID != lpSessionData->GetSessionID())
    {
        ERRLOG3(g_Log, "UID:%10u / SessionID:%10u / ReceivedSessionID:%10u / "
            "유저 로그인 실패 : - 세션ID가 다릅니다", 
            dwUID, lpSessionData->GetSessionID(), dwSessionID);

        usError = 1;
    }    
    else 
    {
        unsigned char cAgentServerType = static_cast<unsigned char>(
            CServerSetup::GetInstance().GetAgentServerType());

        switch(cAgentServerType)
        {
        case UnifiedConst::Part1:
        case UnifiedConst::Part1Unified:

            // 있을 수 없다. 이건 파트 2서버라구!
            usError = 1;
            SERLOG0(g_Log, "Part2 서버를 Part1설정으로 켜려고 하셨습니다. 서버를 다시 켜 주십시오");
            break;

        case UnifiedConst::ROW:
        case UnifiedConst::Part2Unified:

            // Part2 / Part2 통합서버는, 읽어올 UserInfo가 하나뿐이다.
            lpSessionData->SetOldServerGroupID(cAgentServerType);
            break;

        case UnifiedConst::Part2Selectable:
            // 선택을 안 하고 그냥 UserLogin을 보내면, 신규/기존인가에 따라
            // UnifiedConst::Part2Selectable 번 혹은 1번에서 데이터를 읽어온다.
            break;
        }

        // 유저 로그인
        if(0 == usError)
        {
            if (!(lpSessionData->GetFirstLogin() & UnifiedConst::SELECTED_PART2))
            {
/*
                ERRLOG2(g_Log, "UID:%10u / FirstLogin:%d / "
                    "UserLogin 까지 진입했는데, Part2 에서 창고 선택한 적 없는 것처럼 되어 있음. 값을 세팅함.",
                    lpSessionData->GetUID(), lpSessionData->GetFirstLogin());
*/                
                lpSessionData->SetFirstLogin(
                    lpSessionData->GetFirstLogin() | UnifiedConst::SELECTED_PART2);

                DBComponent::GameDB::UpdateUserFirstLogin(CDBSingleObject::GetInstance(), 
                    lpSessionData->GetUID(), lpSessionData->GetFirstLogin());
            }

            CBilling& Billing = lpSessionData->GetBilling();

            nPlayTime       = Billing.GetPlayTime();
            cBillingUser    = (0 == Billing.GetCRMData()) ? 0 : 1;
            cBillingType    = Billing.GetBillingType();

            cAdminLevel     = lpSessionData->GetAdminLevel();

            lpCharView      = lpSessionData->GetCharView(0);
            dwCharViewNum   = USER_INFO::MAX_CHAR_NUM;

            // WORK_LIST 2.1 계정 국적 추가
		    cAccountNation	= lpSessionData->GetAccountNation();

		    // 길드 및 국적 정보 셋팅
		    for (int i=0; i< USER_INFO::MAX_CHAR_NUM; ++i)
		    {
			    if (0 != lpCharView[i].CID)
			    {
				    if (lpCharView[i].GID)
				    {
					    Guild::CGuildDB* lpGuild = static_cast<Guild::CGuildDB*>(
                            Guild::CGuildDBMgr::GetInstance().GetGuild( lpCharView[i].GID ));

					    if (lpGuild)
					    {
						    GuildData[i].m_cNation = lpGuild->GetNation();
						    strcpy(GuildData[i].m_szName, lpGuild->GetName());
					    }
				    }
				    else
				    {
					    // WORK_LIST 2.1 계정 국적 추가
					    GuildData[i].m_cNation = 0;
					    strcpy(GuildData[i].m_szName, "");
    //					switch (lpCharView[i].Race)
    //					{
    //						case CClass::RaceType::HUMAN:	GuildData[i].m_cNation = Creature::KARTERANT;	break;
    //						case CClass::RaceType::AKHAN:	GuildData[i].m_cNation = Creature::MERKADIA;	break;
    //						default:						GuildData[i].m_cNation = Creature::STATELESS;	break;
    //					}
				    }
			    }
		    }

			// 로그인한다. 
			// PlayTime을 Login 상태로 바꾼다.
			using namespace DBComponent;

			GameDB::GetUnifiedItemStoreInfo(CDBSingleObject::GetInstance(), dwUID, 
				lpSessionData->GetOldServerGroupID(), &storeInfo, true);

			KeyInfo* pKeyInfo = lpSessionData->GetKeyInfo();
			memset(pKeyInfo, 0, sizeof(KeyInfo));
			
			//GameDB::GetKeyInfo(CDBSingleObject::GetInstance(), pKeyInfo, dwUID);

            lpSessionData->LogUserData("캐릭터 뷰 얻기 성공");			
        }
    }

	// WORK_LIST 2.1 계정 국적 추가
    if(!SendPacket::UserLogin(SendStream, dwRequestKey, dwUID, 
        lpCharView, GuildData, &storeInfo, dwCharViewNum, cAccountNation, nPlayTime, cAdminLevel, 
        cBillingUser, cBillingType, cLoginType, usError))
    {
        ERRLOG1(g_Log, "UID:%10u / 캐릭터 뷰 얻기 실패 : 캐릭터 뷰 전송 실패", dwUID);
        return false;
    }

    return true;
}


bool UserLogout(CSendStream& SendStream, PktULoD* lpPktULoD)
{
    unsigned long dwSessionID	= lpPktULoD->m_dwSessionID;
    unsigned long dwUID	        = lpPktULoD->m_dwUserID;

    // 접속 DB에서 유저 검사
    using namespace DataStorage;

    CSessionData* lpSessionData = CSessionDataMgr::GetInstance().GetOpenSession(dwUID);

    // 세션 얻기
    if(0 == lpSessionData)
    {
        ERRLOG1(g_Log, "UID:%10u / 유저 로그아웃 실패 : 세션이 없습니다", dwUID);
    }
    // 세션 ID검사
    else if(dwSessionID != lpSessionData->GetSessionID())
    {
        ERRLOG3(g_Log, "UID:%10u / SessionID:%10u / RSessionID:%10u / 유저 로그아웃 실패 : 세션ID가 다릅니다",
            dwUID, lpSessionData->GetSessionID(), dwSessionID);
    }		
    else if(!lpSessionData->UserDisable())
    {
        ERRLOG1(g_Log, "UID:%10u / 유저 로그아웃 실패 : 유저 비활성화 실패", dwUID);
    }
    else
    {
		// 게임타임을 업데이트한다.
		// 로그아웃한다. 
		// PlayTime을 Logout 상태로 바꾼다.
        INFLOG1(g_Log, "UID:%10u / 유저 로그아웃 성공 : 유저를 비활성화했습니다.", dwUID);
        return true;
    }

    return false;
}

bool UserMove(CSendStream& SendStream, PktUMvD* lpPktUMvD)
{
    unsigned long dwSessionID	= lpPktUMvD->m_dwSessionID;
    unsigned long dwUID	        = lpPktUMvD->m_dwUserID;

    // 접속 DB에서 유저 검사
    using namespace DataStorage;

    CSessionData* lpSessionData = CSessionDataMgr::GetInstance().GetOpenSession(dwUID);

    // 세션 얻기
    if(0 == lpSessionData)
    {
        ERRLOG1(g_Log, "UID:%10u / 유저 이동(캐릭터 선택) 실패 : 세션이 없습니다", dwUID);
    }
    // 세션 ID검사
    else if(dwSessionID != lpSessionData->GetSessionID())
    {
        ERRLOG3(g_Log, "UID:%10u / SessionID:%10u / ReceivedSessionID:%10u / "
            "유저 이동(캐릭터 선택) 실패 : 세션ID가 다릅니다",
            dwUID, lpSessionData->GetSessionID(), dwSessionID);
    }
    else if(!lpSessionData->UserMove())
    {
        ERRLOG1(g_Log, "UID:%10u / 유저 이동(캐릭터 선택) 실패 : 유저 이동 실패", dwUID);
    }
    else
    {
        return true;
    }

    return false;
}

// WORK_LIST 2.1 계정 국적 추가
bool SelectAccountNation(CSendStream& SendStream, PktSAND* lpPktSAND)
{
	unsigned long dwUID				= lpPktSAND->m_dwUserID;
	unsigned char cType				= lpPktSAND->m_cType;
	unsigned char cAccountNation	= lpPktSAND->m_cAccountNation;
	unsigned long dwRequestKey		= lpPktSAND->m_dwRequestKey;

	unsigned short usError = 0;

	using namespace DataStorage;

	CSessionData* lpSessionData = CSessionDataMgr::GetInstance().GetOpenSession(dwUID);

	// 세션 얻기
	if(0 == lpSessionData)
	{
		ERRLOG1(g_Log, "UID:%10u / 계정 국적 설정 및 변경 실패 : 세션이 없습니다", dwUID);
		usError = 4;
	}
	else
	{
        unsigned char cOldServerGroupID = lpSessionData->GetOldServerGroupID();

		// 계정 국가 설정
		if ( PktSAND::TYPE_SET == cType )
		{
			if (!DBComponent::GameDB::UpdateAccountNation( 
                CDBSingleObject::GetInstance(), dwUID, cAccountNation, cOldServerGroupID))
			{
                ERRLOG3(g_Log, "UID:%10u / AccountNation:%d / ServerGroupID:%d / 계정 국적 설정 실패 : UpdateAccountNation DB에러",
					dwUID, cAccountNation, cOldServerGroupID);

				usError = 2;
			}
		}
		// WORK_LIST 2.3 계정 국적 변경 기능 구현
		// 계정 국가 변경
		else if ( PktSAND::TYPE_CHANGE == cType )
		{
			if ( lpSessionData->GetAccountNation() != cAccountNation )
			{
				if ( !DBComponent::GameDB::UpdateAccountNation( CDBSingleObject::GetInstance(), 
                    dwUID, cAccountNation, cOldServerGroupID ) )
				{
                    ERRLOG3(g_Log, "UID:%10u / AccountNation:%d / ServerGroupID:%d / 계정 국적 설정 실패 : UpdateAccountNation DB에러",
						dwUID, cAccountNation, cOldServerGroupID);

					usError = 2;
				}
				else
				{
                    unsigned char cOldAccountNation = lpSessionData->GetAccountNation();

					unsigned long aryGID[ USER_INFO::MAX_CHAR_NUM ] = { 0, 0, 0, 0, 0 };
					unsigned long aryFame[ USER_INFO::MAX_CHAR_NUM ] = { 0, 0, 0, 0, 0 };

                    unsigned long aryOldFame[ USER_INFO::MAX_CHAR_NUM ] = { 0, 0, 0, 0, 0 };
                    unsigned long aryOldGID[ USER_INFO::MAX_CHAR_NUM ] = { 0, 0, 0, 0, 0 };

					for (int i=0; i < USER_INFO::MAX_CHAR_NUM; ++i)
					{
						const CHAR_VIEW* pCharView = lpSessionData->GetCharView(i);
						if ( 0 != pCharView->CID )
						{
                            aryOldFame[ i ] = pCharView->Fame;
                            aryOldGID[ i ]  = pCharView->GID;

							Guild::CGuildDB* lpGuild = static_cast<Guild::CGuildDB*>(
								Guild::CGuildDBMgr::GetInstance().GetGuild( pCharView->GID ));

							if (NULL != lpGuild)
							{
								// 명성이 5000(미국은 1000) 이상이었다면, 명성치에 패널티 적용
								const unsigned short wFamePaneltyValue1st = CSessionData::FAME_PANELTY_VALUE_1ST;

								if ( pCharView->Fame > wFamePaneltyValue1st )
								{
									if ( pCharView->Fame < CSessionData::FAME_PANELTY_VALUE_2ND )
									{
										lpGuild->AddMemberFame( Guild::TYPE_VALUE, pCharView->CID,
											int(wFamePaneltyValue1st - pCharView->Fame) );
										lpSessionData->SetCharViewFame( pCharView->CID, wFamePaneltyValue1st );
									}
									else
									{
										lpGuild->AddMemberFame( Guild::TYPE_HALF, pCharView->CID, 0 );
										lpSessionData->SetCharViewFame( pCharView->CID, pCharView->Fame / 2 );
									}
								}

								if ( 1 < lpGuild->GetCurrentMemberNum() )
								{
									bool bLeaveGuild = true;

									// 길드 마스터의 경우 권한을 양도한 후 탈퇴시킨다.
									if (pCharView->CID == lpGuild->GetMaster().m_dwCID)
									{
										unsigned long dwNewMaster = lpGuild->SetNewMaster();
										if (0 != dwNewMaster)
										{
											SendPacket::GuildCmd(&SendStream, lpGuild->GetGID(), dwNewMaster, MASTER, 
												PktGuildCmd::GC_SETUP_TITLE, NULL, NULL, PktBase::NO_SERVER_ERR);

											DETLOG3(g_Log, "GID:%10u 길드마스터(이전:%10u, 현재:%10u)가 변경되었습니다. - 길드마스터 국적 변경", 
												lpGuild->GetGID(), pCharView->CID, dwNewMaster);
										}
										else
										{
											DETLOG1(g_Log, "GID:%10u 길드가 해체되었습니다. - 길드마스터 국적 변경(양도할 길원 없음)", lpGuild->GetGID());
											CGuildDBMgr::GetInstance().DissolveGuild(lpGuild->GetGID(), 
												PktCreateGuild::NONE_NEXT_GUILDMASTER_BY_GUILDMASTER_OUT);

											bLeaveGuild = false;
										}
									}

									if (true == bLeaveGuild)
									{
										// 길드 국적과 변경된 계정 국적이 다르므로, 탈퇴 처리 시킨다.
										if ( lpGuild->LeaveMember( pCharView->CID ) )
										{
											// 길드 탈퇴 정보를 모든 게임 서버로 전송
											SendPacket::GuildCmd( NULL, pCharView->GID, pCharView->CID, 0, 
												PktGuildCmd::GC_KICK, NULL, NULL, PktBase::NO_SERVER_ERR );
										}
										else
										{
											// 길드 탈퇴 실패
											ERRLOG3(g_Log, "UID:%10u / CID:%10u / GID:%10u 길드 강제 탈퇴 실패 : 계정 국적 변경으로 인한 탈퇴 실패",
												dwUID, pCharView->CID, pCharView->GID);
										}
									}
								}
								else
								{
									// 길드원이 한명뿐이므로, 길드를 해체한다.
									DETLOG1(g_Log, "GID:%10u 길드가 해체되었습니다. - 길드마스터 국적 변경(길마뿐인 길드)", lpGuild->GetGID());
									CGuildDBMgr::GetInstance().DissolveGuild(
										lpGuild->GetGID(), PktCreateGuild::NONE_NEXT_GUILDMASTER_BY_GUILDMASTER_OUT);
								}

								lpSessionData->SetCharViewGID( pCharView->CID, 0 );
							}
							else
							{
								// 명성이 5000(미국은 1000) 이상이었다면, 명성치에 패널티 적용
								const unsigned short wFamePaneltyValue1st = CSessionData::FAME_PANELTY_VALUE_1ST;

								if ( pCharView->Fame > wFamePaneltyValue1st )
								{
									CCharacterData* lpCharacterData = 0;
									CSessionData*	lpCharSessionData = 
										CSessionDataMgr::GetInstance().GetCharLoadedSession( pCharView->CID );

									if ( 0 == lpCharSessionData || 0 == (lpCharacterData = lpCharSessionData->GetCharacterData()) )
									{
										// 캐릭터가 안매달려있는 경우, 캐릭터를 불러온다.
										lpCharacterData = static_cast<CCharacterData*>(
											CCharacterDataMgr::GetInstance().GetLogoutData( pCharView->CID ));

										if ( 0 != lpCharacterData )
										{
											CHAR_INFOST charInfoST = lpCharacterData->GetInfo();

											if ( pCharView->Fame < CSessionData::FAME_PANELTY_VALUE_2ND )
											{
												charInfoST.Fame = wFamePaneltyValue1st;
												lpSessionData->SetCharViewFame( pCharView->CID, wFamePaneltyValue1st );
											}
											else
											{
												charInfoST.Fame /= 2;
												lpSessionData->SetCharViewFame( pCharView->CID, pCharView->Fame / 2 );
											}

											lpCharacterData->SetInfo( charInfoST );
										}
									}
									else if( CSessionData::SE_CHAR_ENABLED == lpCharSessionData->GetSessionState() )
									{
										ERRLOG1( g_Log, "CID:%10u 캐릭터 선택화면에서 계정 국적 변경시, 이미 게임에 로드된 캐릭터가 존재합니다.", pCharView->CID );
									}
								}

								lpSessionData->SetCharViewGID( pCharView->CID, 0 );	
							}
							
							aryGID[ i ]     = pCharView->GID;
							aryFame[ i ]    = pCharView->Fame;

                            /*
                                - 국적변경 관련 로그

                                UID CID 캐릭터명	변경전국가	변경후국가	기존명성	변경명성	
                                현재레벨	이전길드ID  새길드ID    파티ID
                            */

                            INFLOG11(g_Log, "국적로그\t%10u\t%10u\t%s\t%s\t%s\t%u\t%u\t%u\t%u\t%u\t%u\t", 
                                dwUID, pCharView->CID, pCharView->Name, 
                                Creature::GetShortNationName(cOldAccountNation),
                                Creature::GetShortNationName(cAccountNation),
                                aryOldFame[ i ], aryFame[ i ], pCharView->Level, 
                                aryOldGID[ i ], aryGID[ i ], pCharView->PID);
                        }
					}

					// 국적 변경으로 인해 길드 ID가 바뀐 정보를 Auth 서버로 보내서, Client 가 갱신하도록 한다.
					SendPacket::NationChangeResult(SendStream, dwRequestKey, dwUID, aryGID, aryFame);
				}
			}
			else
			{
				ERRLOG3(g_Log, "UID:%10u / Old_AccountNation:%d / New_AccountNation:%d / 계정 국적 변경 실패 : 같은 국적임",
						dwUID, lpSessionData->GetAccountNation(), cAccountNation);

				usError = 5;	// 이미 변경할려는 국적이다.
			}
		}

		if (0 == usError)
		{
			// 계정 국가 설정 및 변경 성공
			RULLOG2(g_Log, "UID:%10u / AccountNation:%d 계정 국가 설정", dwUID, cAccountNation);

			// 계정 국가 설정
			lpSessionData->SetAccountNation( cAccountNation );
		}
	}

	return SendPacket::SelectAccountNation(SendStream, dwRequestKey, dwUID, cType, cAccountNation, usError);
}

bool CharSelect(CSendStream& SendStream, PktCSD* lpPktCSD)
{
    // 서버 세션 얻기
    unsigned long dwUID         = lpPktCSD->m_dwUserID;
    unsigned long dwCID         = lpPktCSD->m_dwCharID;
    unsigned long dwRequestKey  = lpPktCSD->m_dwRequestKey;

    unsigned short usError = 0;
    unsigned char  cZone = 1;

    using namespace DataStorage;

    CSessionData* lpSessionData = CSessionDataMgr::GetInstance().GetOpenSession(dwUID);
    CCharacterData* lpCharacterData = 0;

    // 세션 얻기
    if(0 == lpSessionData)
    {
        ERRLOG1(g_Log, "UID:%10u / 캐릭터 선택 실패 : 세션이 없습니다", dwUID);
        usError = 5;
    }
    // 캐릭터 아이디 검사
    else if (!lpSessionData->HasCharacter(dwCID))
    {
        const USER_INFO& userInfo = lpSessionData->GetUserInfo();

		// WORK_LIST 2.1 계정 국적 추가
		ERRLOG8(g_Log, "UID:%10u / SelectedCID:%10u / Char1:%10u / Char2:%10u / Char3:%10u / Char4:%10u / Char5:%10u / Nation:%d"
            "캐릭터 선택 실패 : 소유하고 있지 않은 캐릭터입니다.",
            dwUID, dwCID, userInfo.CharID[0], userInfo.CharID[1], userInfo.CharID[2], 
            userInfo.CharID[3], userInfo.CharID[4], userInfo.Nation);

        usError = 3;        
    }
    else 
    {
        if (!lpSessionData->LoadData(dwCID))
        {
            ERRLOG2(g_Log, "UID:%10u / SelectedCID:%10u / 캐릭터 선택 실패 : 캐릭터 로드 실패", dwUID, dwCID);
            usError = 3;
        }
        else
        {
            CCharacterData* lpCharacterData = lpSessionData->GetCharacterData();
			lpCharacterData->SetUID(dwUID);

            SERVER_ID serverID;
            serverID.dwID = lpCharacterData->GetInfoEx().ServerID;

			// 국가전쟁 미 참여시 공헌훈장 포인트 삭감.
			unsigned char cRealmPoint = lpCharacterData->GetRealmPoint();
			
			unsigned long dwRealmCount_Out = 0;

			// 서버 세션이 시작되면 
			// 로그아웃 시간을 기점으로 공헌훈장의 포인트 추가 삭감을
			// 계산한다.
			TIME* pTime = lpCharacterData->GetLogoutTime();

			if(pTime)
			{
				CTime timeLoggout(pTime->Year, pTime->Month, pTime->Day, pTime->Hour, pTime->Minute, pTime->Second);
					
				CGameTimeDBMgr::GetInstance().CheckRealmCount(timeLoggout, &dwRealmCount_Out);

				if(lpCharacterData->GetRealmCheckPoint() && dwRealmCount_Out)
				{
					dwRealmCount_Out -= 1;
					lpCharacterData->SetRealmCheckPoint(0);
					lpCharacterData->SetRealmMinute(0);

					if(cRealmPoint<=dwRealmCount_Out)
					{
						lpCharacterData->SetRealmPoint(0);						
					}
					else
					{
						cRealmPoint -= static_cast<unsigned char>(dwRealmCount_Out);
						lpCharacterData->SetRealmPoint(cRealmPoint);						
					}
				}
				else if(dwRealmCount_Out)
				{
					if(cRealmPoint<=dwRealmCount_Out)
					{
						lpCharacterData->SetRealmPoint(0);						
					}
					else
					{
						cRealmPoint -= static_cast<unsigned char>(dwRealmCount_Out);
						lpCharacterData->SetRealmPoint(cRealmPoint);						
					}
					
					lpCharacterData->SetRealmMinute(0);
				}
				else if(lpCharacterData->GetRealmCheckPoint() && !dwRealmCount_Out)
				{
					if(CGameTimeDBMgr::GetInstance().GetCurrentGameTime(GameTime::REALM)!=GameTime::RT_REALM_WAR)
					{
						unsigned char cRealmPoint = lpCharacterData->GetRealmPoint();

						lpCharacterData->SetRealmCheckPoint(0);
						lpCharacterData->SetRealmMinute(0);

						if(cRealmPoint<DBAgent::DataStorage::CSessionData::MAX_REALM_POINT)
						{
							cRealmPoint++;
							lpCharacterData->SetRealmPoint(cRealmPoint);
						}
					}			
				}

				CTime timeLogin = CTime::GetCurrentTime();

				pTime->Year		= timeLogin.GetYear();
				pTime->Month	= timeLogin.GetMonth();
				pTime->Day		= timeLogin.GetDay();			
				pTime->Hour		= timeLogin.GetHour();
				pTime->Minute	= timeLogin.GetMinute();
				pTime->Second	= timeLogin.GetSecond();
			}				

			// edith 2010.01.02 공헌포인트를 길드원 정보에 따라 처리해주는 함수.
			unsigned int GID = lpCharacterData->GetGID();
			Guild::CGuildDB* lpGuild = static_cast<Guild::CGuildDB*>(
                Guild::CGuildDBMgr::GetInstance().GetGuild( GID ));

			if(lpGuild)
			{
				unsigned char MaxNum = lpGuild->GetMaxMemberNum();
				unsigned char CurNum = lpGuild->GetCurrentMemberNum();
				lpCharacterData->SetRealmPoint(MaxNum/10);
			}
			else 
				lpCharacterData->SetRealmPoint(0);

            if (CServerSetup::AuthServer == serverID.GetType() || 0 == serverID.dwID)
            {
                // 초기화되어 있지 않은 캐릭터
//                cZone = CServerSetup::GetInstance().IsBattleAgentServer() 
//                 ? SERVER_ID::BATTLE_SERVER : SERVER_ID::ZONE12;
				if(CServerSetup::GetInstance().IsBattleAgentServer())
					cZone = SERVER_ID::BATTLE_SERVER;
				else
				{
                    unsigned char cRace = (0 != lpCharacterData) ? lpCharacterData->GetRace() : -1;

					if(cRace == CHAR_CREATE::HUMAN)
						cZone  = SERVER_ID::CAPITAL;//was SERVER_ID::ZONE12
					else
						cZone  = SERVER_ID::CAPITAL;//was SERVER_ID::ZONE12
				}

			}
            else
            {
                cZone = serverID.sID.ID;

                if (SERVER_ID::BATTLE_ZONE == cZone)
                {
                    cZone = SERVER_ID::ZONE3;
                }

				// edith 2009.06.13
                // 해당 종족 검사
                if (!lpSessionData->CheckCharZone(cZone))
                {
                    // 0  : Human
                    // 1  : Akhan
                    // -1 : 캐릭터 데이터 연결 안됨

                    unsigned char cRace = (0 != lpCharacterData) ? lpCharacterData->GetRace() : -1;

                    unsigned char cStartZone = lpSessionData->GetCharStartZone(cRace);

/*
					if(cRace == CHAR_CREATE::HUMAN)
						cStartZone  = SERVER_ID::ZONE1;
					else
						cStartZone  = SERVER_ID::ZONE2;
*/
/*
					if(SERVER_ID::ZONE5 == cZone && !CGameTimeDBMgr::GetInstance().IsEnterTime())
					{
						// 테섭용 신규존 진입제한에 걸렸음.
						DETLOG5(g_Log, "UID:%10u / CID:%10u / Zone:%u / StartZone:%u / Race:%u / 신규존 진입제한 : 시작 존으로 이동합니다", 
							dwUID, dwCID, cZone, cStartZone, cRace);
					}
					else
					{
						DETLOG5(g_Log, "UID:%10u / CID:%10u / Zone:%u / StartZone:%u / Race:%u / 잘못된 존 체크 : 시작 존으로 이동합니다", 
							dwUID, dwCID, cZone, cStartZone, cRace);
					}
*/
					DETLOG5(g_Log, "UID:%10u / CID:%10u / Zone:%u / StartZone:%u / Race:%u / 잘못된 존 체크 : 시작 존으로 이동합니다", 
						dwUID, dwCID, cZone, cStartZone, cRace);

                    cZone = cStartZone;
                }
/*
				// edith 2009.06.13 Character Select GameTime Over
				if(CGameTimeDBMgr::GetInstance().IsGuildWarTime() && cZone == SERVER_ID::CAPITAL)
				{
					DETLOG5(g_Log, "UID:%10u / CID:%10u / Zone:%u / StartZone:%u / Race:%u / IsGuildWarTime 존 체크 : 시작 존으로 이동합니다", 
						dwUID, dwCID, cZone, cStartZone, cRace);

					cZone = cStartZone;
				}
				else if(CGameTimeDBMgr::GetInstance().IsRealmWarTime() && (SERVER_ID::STONE_WAR1 <= cZone && cZone <= SERVER_ID::STONE_WAR3))
				{
					DETLOG5(g_Log, "UID:%10u / CID:%10u / Zone:%u / StartZone:%u / Race:%u / IsRealmWarTime 존 체크 : 시작 존으로 이동합니다", 
						dwUID, dwCID, cZone, cStartZone, cRace);

					cZone = cStartZone;
				}
*/
            }
            
            INFLOG2(g_Log, "UID:%10u / SelectedCID:%10u / 캐릭터 선택 성공", dwUID, dwCID);
            lpSessionData->UnloadData();
        }
    }

    return SendPacket::CharSelect(SendStream, dwRequestKey, cZone, usError);    
}


bool CharCreate(CSendStream& SendStream, PktCCD* lpPktCCD)//what default values you get when you create a new char hayzohar
{
    unsigned long   dwUID           = lpPktCCD->m_dwUserID;
    unsigned long   dwSlot          = lpPktCCD->m_dwSlot;
    unsigned long   dwGold          = lpPktCCD->m_dwGold;
    unsigned long   dwRequestKey    = lpPktCCD->m_dwRequestKey;
    unsigned long   dwCID           = 0;

    CHAR_VIEW       CharView	    = {0,};
    
    unsigned short  usError         = 0;


    using namespace DataStorage;

    CSessionData* lpSessionData = CSessionDataMgr::GetInstance().GetOpenSession(dwUID);

    // 세션 얻기
    if(0 == lpSessionData)
    {
        ERRLOG1(g_Log, "UID:%10u / 캐릭터 생성 실패 : 세션이 없습니다", dwUID);
        usError = PktCCAck::NULL_SESSION;
    }
    else
    {
        // 스킬 세팅
        SKILL Skill(1, 1);//(skill num , slot num)

        switch(lpPktCCD->m_CharCreate.Class) 
        {
        case CClass::Fighter:
		case CClass::Warrior:
		case CClass::Defender:
			Skill.SSlot[0] = SKILLSLOT(0x8104, 0, 1);		
		break;
		//-------
        case CClass::Rogue:
		case CClass::Assassin:
			Skill.SSlot[0] = SKILLSLOT(0x8205, 0, 1);
		break;
		//-------
		case CClass::Archer:
			Skill.SSlot[0] = SKILLSLOT(0x8803, 0, 1);
		break;
        case CClass::Mage:
		case CClass::Sorcerer:
		case CClass::Enchanter:
			Skill.SSlot[0] = SKILLSLOT(0x9215, 0, 1);
		break;
        case CClass::Acolyte:
		case CClass::Priest:
		case CClass::Cleric:				
			Skill.SSlot[0] = SKILLSLOT(0x8C03, 0, 1);	
		break;
        case CClass::Combatant:		
		case CClass::Attacker:
		case CClass::Templar:
			Skill.SSlot[0] = SKILLSLOT(0x9104, 0, 1);
		break;
		case CClass::Gunner:
			Skill.SSlot[0] = SKILLSLOT(0x9502, 0, 1);
		break;
		//----
        case CClass::Officiator:
			Skill.SSlot[0] = SKILLSLOT(0x8307, 0, 1);
		break;
		//----
		case CClass::RuneOff:
			Skill.SSlot[0] = SKILLSLOT(0x9602, 0, 1);
		break;
		case CClass::ShadowOff:
			Skill.SSlot[0] = SKILLSLOT(0x8307, 0, 1);
		break;
		case CClass::LifeOff:
			Skill.SSlot[0] = SKILLSLOT(0x9217, 0, 1);
		break;

        default:
            ERRLOG3(g_Log, "UID:%10u / CID:%10u / Class:%u/ 캐릭터 생성 : 스킬 넣기 실패 - 잘못된 클래스입니다",
                dwUID, dwCID, lpPktCCD->m_CharCreate.Class);

			usError = PktCCAck::FAIL_INSERT_DATA; 
            break;
        }
		//Skill.SSlot[1] = SKILLSLOT(0x9E03, 0, 1);
		//Skill.SSlot[2] = SKILLSLOT(0x9E04, 0, 1);

			// Edith 2008.06.14 캐릭터 생성시 생성값 문제 검출
		if(dwGold > 0) 
		{
			ERRLOG3(g_Log, "UID:%10u / CID:%10u / Gold:%u/ 캐릭터 생성 : 골드값 이상", dwUID, dwCID, dwGold);
			usError = PktCCAck::FAIL_INSERT_DATA; 
		}
		else if(lpPktCCD->m_CharCreate.Face < 0 && lpPktCCD->m_CharCreate.Face >= 7)
		{
			ERRLOG3(g_Log, "UID:%10u / CID:%10u / Face:%u/ 캐릭터 생성 : 얼굴 이상", dwUID, dwCID, lpPktCCD->m_CharCreate.Hair);
			usError = PktCCAck::FAIL_INSERT_DATA; 
		}
		else if(lpPktCCD->m_CharCreate.Sex == 1 || lpPktCCD->m_CharCreate.Sex == 2)
		{
			if(lpPktCCD->m_CharCreate.Sex == 1)	// 인간 남자
			{
				if(lpPktCCD->m_CharCreate.Hair < 0 && lpPktCCD->m_CharCreate.Hair >= 5)
				{
					ERRLOG3(g_Log, "UID:%10u / CID:%10u / Hair:%u/ 캐릭터 생성 : 헤어 이상", dwUID, dwCID, lpPktCCD->m_CharCreate.Hair);
					usError = PktCCAck::FAIL_INSERT_DATA; 
				}
			}
			else if(lpPktCCD->m_CharCreate.Sex == 2)// 인간 여자
			{
				if(lpPktCCD->m_CharCreate.Hair < 0 && lpPktCCD->m_CharCreate.Hair >= 7)
				{
					ERRLOG3(g_Log, "UID:%10u / CID:%10u / Hair:%u/ 캐릭터 생성 : 헤어 이상", dwUID, dwCID, lpPktCCD->m_CharCreate.Hair);
					usError = PktCCAck::FAIL_INSERT_DATA; 
				}
			}
			else
			{
				ERRLOG3(g_Log, "UID:%10u / CID:%10u / Sex:%u/ 캐릭터 생성 : 성별 이상", dwUID, dwCID, lpPktCCD->m_CharCreate.Sex);
				usError = PktCCAck::FAIL_INSERT_DATA; 
			}
		}
		else if(lpPktCCD->m_CharCreate.Sex == 3 || lpPktCCD->m_CharCreate.Sex == 4)
		{
			if(lpPktCCD->m_CharCreate.Sex == 3)	// 아칸남자
			{
				if(lpPktCCD->m_CharCreate.Hair < 0 && lpPktCCD->m_CharCreate.Hair >= 7)
				{
					ERRLOG3(g_Log, "UID:%10u / CID:%10u / Hair:%u/ 캐릭터 생성 : 헤어 이상", dwUID, dwCID, lpPktCCD->m_CharCreate.Hair);
					usError = PktCCAck::FAIL_INSERT_DATA; 
				}
			}
			else if(lpPktCCD->m_CharCreate.Sex == 4) // 아칸여자
			{
				if(lpPktCCD->m_CharCreate.Hair < 0 && lpPktCCD->m_CharCreate.Hair >= 7)
				{
					ERRLOG3(g_Log, "UID:%10u / CID:%10u / Hair:%u/ 캐릭터 생성 : 헤어 이상", dwUID, dwCID, lpPktCCD->m_CharCreate.Hair);
					usError = PktCCAck::FAIL_INSERT_DATA; 
				}
			}
			else
			{
				ERRLOG3(g_Log, "UID:%10u / CID:%10u / Sex:%u/ 캐릭터 생성 : 성별 이상", dwUID, dwCID, lpPktCCD->m_CharCreate.Sex);
				usError = PktCCAck::FAIL_INSERT_DATA; 
			}
		}
		else
		{
			ERRLOG3(g_Log, "UID:%10u / CID:%10u / Race:%u/ 캐릭터 생성 : 종족 이상", dwUID, dwCID, lpPktCCD->m_CharCreate.Race);
			usError = PktCCAck::FAIL_INSERT_DATA; 
		}


		int iStat = lpPktCCD->m_CharCreate.STR+lpPktCCD->m_CharCreate.DEX
			+lpPktCCD->m_CharCreate.CON+lpPktCCD->m_CharCreate.INT+lpPktCCD->m_CharCreate.WIS - 100;

		if(iStat != 5)
		{
			ERRLOG3(g_Log, "UID:%10u / CID:%10u / iStat:%u/ 캐릭터 생성 : 스텟 이상", dwUID, dwCID, iStat+100);
			usError = PktCCAck::FAIL_INSERT_DATA; 
		}

		if (usError != 0)
		{
			;
		}
		// DB에 캐릭터 생성
		else if (!DBComponent::GameDB::InsertChar(CDBSingleObject::GetInstance(), dwUID, dwSlot, 
            lpPktCCD->m_CharCreate, dwGold, lpPktCCD->m_Pos, Skill, 
            lpSessionData->GetOldServerGroupID(), dwCID)) 
        {
            ERRLOG1(g_Log, "UID:%10u / 캐릭터 생성 실패 : DB에러", dwUID);

            // 캐릭터 생성 실패
            usError = PktCCAck::FAIL_INSERT_DATA; 
        }
        else if (-1 == dwCID) 
        {	
            // 동일한 이름을 가지고 있는 캐릭터가 있는 경우
            usError = PktCCAck::EXIST_CHAR_NAME;            
        }
        else if (-2 == dwCID) 
        {	
            // 동일한 이름을 가지고 있는 캐릭터가 있는 경우
            usError = PktCCAck::EXIST_SLOT;
        }
        else if (!lpSessionData->GetUserInfoFromDB(CDBSingleObject::GetInstance()))
        {
            // 캐릭터를 생성했으니. 뷰를 다시한번 로드한다.
            ERRLOG1(g_Log, "UID:%10u / 캐릭터 생성 실패 : 캐릭터 뷰 재로드 실패.", dwUID);
            usError = PktCCAck::VIEW_RELOAD_FAILED;
        }
        else if (!lpSessionData->GetCharView(dwCID, CharView))
        {
            // 뷰를 얻어서 보내 준다.
            ERRLOG1(g_Log, "UID:%10u / 캐릭터 생성 실패 : 캐릭터 뷰 얻기 실패.", dwUID);
            usError = PktCCAck::VIEW_RELOAD_FAILED;                
        } 
        else
        {
            // 캐릭터 생성 성공.
            RULLOG4(g_Log, "UID:%10u / CID:%10u / 캐릭터 [%s]%s 생성", 
                dwUID, dwCID, (CharView.Race == 0) ? "Human" : "Akhan", CharView.Name);

            /*
                - 국적변경 관련 로그

                UID CID 캐릭터명	변경전국가	변경후국가	기존명성	변경명성	
                현재레벨	이전길드ID  새길드ID    파티ID
            */

            INFLOG11(g_Log, "국적로그\t%10u\t%10u\t%s\t%s\t%s\t%u\t%u\t%u\t%u\t%u\t%u\t", 
                dwUID, dwCID, CharView.Name, 
                Creature::GetShortNationName(lpSessionData->GetUserInfo().Nation),
                Creature::GetShortNationName(lpSessionData->GetUserInfo().Nation),
                0, CharView.Fame, CharView.Level, 
                0, CharView.GID, CharView.PID);
        }
    }

    return SendPacket::CharCreate(SendStream, 
        dwRequestKey, dwCID, dwSlot, CharView, usError);
}

bool CharDelete(CSendStream& SendStream, PktCDD* lpPktCDD)
{
    unsigned long dwUID         = lpPktCDD->m_dwUserID;
    unsigned long dwCID         = lpPktCDD->m_dwCharID;
    unsigned long dwSlot        = lpPktCDD->m_dwSlotNum;
    unsigned long dwRequestKey  = lpPktCDD->m_dwRequestKey;

    unsigned short usError = 0;

    using namespace DataStorage;

    CSessionData* lpSessionData = CSessionDataMgr::GetInstance().GetOpenSession(dwUID);
	
    // 세션 얻기
    if(0 == lpSessionData)
    {
        ERRLOG1(g_Log, "UID:%10u / 캐릭터 삭제 실패 : 세션이 없습니다", dwUID);
        usError = 4;
    }
    else if (CCharacterDataMgr::GetInstance().IsDataLoginDB(dwCID) || 
        CSessionData::SE_CHAR_ENABLED == lpSessionData->GetSessionState())
    {
        // 활성화된 캐릭터 삭제 시도
        ERRLOG2(g_Log, "UID:%10u / CID:%10u / 캐릭터 삭제 실패 : 활성화된 캐릭터 삭제 시도", dwUID, dwCID);
        usError = 3;
    }
    else
    {
        const CHAR_VIEW* lpCharView = 
            lpSessionData->GetCharView(static_cast<unsigned char>(dwSlot));

        if(0 == lpCharView)
        {
            ERRLOG3(g_Log, "UID:%10u / CID:%10u / Slot:%u / 캐릭터 삭제 실패 : 원하는 슬롯에 뷰 없음",
                dwUID, dwCID, dwSlot);

            usError = 2;
        }
        else if(lpCharView->CID != dwCID)
        {
            ERRLOG4(g_Log, "UID:%10u / CID:%10u / DeleteCID:%10u / Slot:%u / 캐릭터 삭제 실패 : 뷰의 CID와 지우려는 캐릭터의 CID다름",
                dwUID, lpCharView->CID, dwCID, dwSlot);

            usError = 2;
        }
        else
        {				
			// DB로 부터 캐릭터 삭제
			if(CServerSetup::GetInstance().IsBattleAgentServer())
			{
				if(!DBComponent::GameDB::DeleteCharBattleServer(
					CDBSingleObject::GetInstance(), dwUID, dwCID, 
					static_cast<unsigned char>(dwSlot),
					CServerSetup::GetInstance().GetServerGroup(), 
					lpSessionData->GetOldServerGroupID()))
				{
					ERRLOG3(g_Log, "UID:%10u / CID:%10u / Slot:%u / 캐릭터 삭제 실패 : DeleteCharBattleServer DB에러",
						dwUID, dwCID, dwSlot);

					usError = 2;
				}
			}
			else 
			{
				if (!DBComponent::GameDB::DeleteChar(
					CDBSingleObject::GetInstance(), dwUID, dwCID, 
					static_cast<unsigned char>(dwSlot),
					CServerSetup::GetInstance().GetServerGroup(), 
					lpSessionData->GetOldServerGroupID()))
				{
					ERRLOG3(g_Log, "UID:%10u / CID:%10u / Slot:%u / 캐릭터 삭제 실패 : DeleteChar DB에러", 
						dwUID, dwCID, dwSlot);

					usError = 2;
				}
			}

			if(0 == usError)
			{
				// 캐릭터 삭제 성공
				RULLOG4(g_Log, "UID:%10u / CID:%10u / 캐릭터 [%s]%s 삭제", 
					dwUID, dwCID, (lpCharView->Race == 0) ? "Human" : "Akhan", lpCharView->Name);

				// 캐릭터 초기화
				lpSessionData->UnloadData();
				lpSessionData->DelUnifiedCharData(dwCID);
			}
        }
    }

    return SendPacket::CharDelete(SendStream, 
        dwRequestKey, dwUID, dwSlot, dwCID, usError);
}

bool CreateItem(CSendStream& SendStream, unsigned long dwServerID, 
                Item::CItemSerialMgr& itemSerialMgr, PktCCID* lpPktCCID)
{    
    if (sizeof(PktCCID) < lpPktCCID->GetLen()) 
    {
        EQUIP Equip;
        memset(&Equip, 0, sizeof(EQUIP));

        Equip.dwSize = lpPktCCID->m_dwSize;
        memcpy(Equip.Data, lpPktCCID + 1, lpPktCCID->m_dwSize);

        if(!DBComponent::GameDB::UpdateEquip(
            CDBSingleObject::GetInstance(), lpPktCCID->m_dwCharID, &Equip))        
        {
            ERRLOG1(g_Log, "CID:%10u / 장비 업데이트 실패", lpPktCCID->m_dwCharID);
        }
        else
        {            
            // 아이템 시리얼 업데이트
            if(itemSerialMgr.SetItemSerial(lpPktCCID->m_dwItemSerial))
            {
                // 시리얼이 바뀌었으므로 DB에 업데이트한다.
                if(!itemSerialMgr.SaveItemSerial(CDBSingleObject::GetInstance(), dwServerID))
                {   
                    ERRLOG3(g_Log, "CID:%10u / ServerID:0x%08X / Serial:0x%016I64X / 아이템 시리얼 업데이트 실패", 
                        lpPktCCID->m_dwCharID, dwServerID, itemSerialMgr.GetItemSerial());
                }
            }
        }

        return true;
    }

    return false;
}



// TODO : FIX FOR BATTLEGROUND!
bool BGServerCharSlot(CSendStream& SendStream, PktBase* lpPktBase)
{
    PktBGServerCharSlot* lpPktBGServerCharSlot = static_cast<PktBGServerCharSlot*>(lpPktBase);

    unsigned long dwUID = lpPktBGServerCharSlot->m_dwUID;
    unsigned long dwCID = lpPktBGServerCharSlot->m_dwCID;
    
    unsigned char cGroup = lpPktBGServerCharSlot->m_cGroup;

    unsigned short usError = 0;

    char* lpBuffer = SendStream.GetBuffer(sizeof(PktBGServerCharSlotAck));

    if(0 != lpBuffer)
    {
        PktBGServerCharSlotAck* lpPktBGServerCharSlotAck = 
            reinterpret_cast<PktBGServerCharSlotAck*>(lpBuffer);

        lpPktBGServerCharSlotAck->m_dwCID  = dwCID;
        lpPktBGServerCharSlotAck->m_cGroup = cGroup;

        memset(lpPktBGServerCharSlotAck->m_dwSlotCID, 0, 
            sizeof(unsigned long) * USER_INFO::MAX_CHAR_NUM);

        memset(lpPktBGServerCharSlotAck->m_szSlotName, 0, 
            sizeof(char) * USER_INFO::MAX_CHAR_NUM * CHAR_INFOST::MAX_NAME_LEN);

        unsigned char cOldServerGroupID = 0;

        switch(CServerSetup::GetInstance().GetAgentServerType())
        {
        case UnifiedConst::Part1:
        case UnifiedConst::Part1Unified:
        default:
            cOldServerGroupID = 0;
            break;

        case UnifiedConst::ROW:
        case UnifiedConst::Part2Unified:
        case UnifiedConst::Part2Selectable:
            cOldServerGroupID = static_cast<unsigned char>(
                CServerSetup::GetInstance().GetAgentServerType());
            break;
        }

        USER_INFO userInfo;
        memset(&userInfo, 0, sizeof(USER_INFO));
   
        if (!DBComponent::GameDB::GetUserInfo(CDBSingleObject::GetInstance(), 
            dwUID, userInfo, cOldServerGroupID))
        {
            ERRLOG2(g_Log, "UID:%10u / 배틀로한으로 캐릭터 정보 주기 실패 : 유저 정보 얻기 실패 : %s",
                dwUID, CDBSingleObject::GetInstance().GetErrorString());

            usError = PktBGServerCharSlotAck::FAIL_NOT_USER;
        }
        else
        {
            CHAR_VIEW charView;

			// WORK_LIST 2.1 계정 국적 추가 (배틀 로한에 Nation 정보를 줘야하나? 아직 추가안했음)
            for (unsigned int nIndex = 0; nIndex < USER_INFO::MAX_CHAR_NUM; ++nIndex)
            {
                unsigned long dwLoadViewCID = userInfo.CharID[nIndex];
                lpPktBGServerCharSlotAck->m_dwSlotCID[nIndex] = dwLoadViewCID;
                
                if (0 != dwLoadViewCID)
                {     
                    memset(&charView, 0, sizeof(CHAR_VIEW));

                    if (!DBComponent::GameDB::GetCharView(CDBSingleObject::GetInstance(),
                        dwLoadViewCID, &charView)) 
                    {
                        ERRLOG3(g_Log, "UID:%10u / CID:%10u / 배틀로한으로 캐릭터 정보 주기 실패 : 캐릭터 뷰 얻기 실패 : %s",
                            dwUID, dwLoadViewCID, CDBSingleObject::GetInstance().GetErrorString());
                        
                        usError = PktBGServerCharSlotAck::FAIL_NOT_CHAR;
                        break;
                    }

                    strncpy(lpPktBGServerCharSlotAck->m_szSlotName[nIndex], 
                        charView.Name, CHAR_INFOST::MAX_NAME_LEN);

                    lpPktBGServerCharSlotAck->m_szSlotName[nIndex][CHAR_INFOST::MAX_NAME_LEN - 1] = 0;
                }
            }
        }

        return SendStream.WrapHeader(sizeof(PktBGServerCharSlotAck), CmdBGServerCharSlot, 0, usError);
    }

    return false;
}


bool UnifiedCharSelect(CSendStream& SendStream, PktBase* lpPktBase)
{
    PktUnifiedCharSelectReq* lpUnifiedSelectReq = static_cast<PktUnifiedCharSelectReq*>(lpPktBase);

    /*
    Part2Unified
        0 : 창고 가져온 적 없다
        1 : Part1 통합에서 창고 가져왔다
        2 : Part2 통합에서 창고 가져왔다.

    Part2Selectable

    */

    unsigned char cAgentServerType = static_cast<unsigned char>(
        CServerSetup::GetInstance().GetAgentServerType());

    using namespace DBAgent::DataStorage;

    CSessionData*   lpSessionData           = 0;

    unsigned long   dwUID                   = lpUnifiedSelectReq->dwUID;
    unsigned long   dwRequestKey            = lpUnifiedSelectReq->dwRequestKey;
    unsigned short  usError                 = 0;

    unsigned char   cSelectedServerGroupID  = lpUnifiedSelectReq->cSelectedServerGroupID;
    unsigned char   cSelectedNation         = lpUnifiedSelectReq->cSelectedNation;

    bool            bSupressSendAck         = false;

    switch(cAgentServerType)
    {
    case UnifiedConst::Part2Unified:

        if(0 == (lpSessionData = CSessionDataMgr::GetInstance().GetOpenSession(dwUID)))
        {
            // 해당 UID가 없다.
            ERRLOG1(g_Log, "UID:%10u / Part2Unified 캐릭터 선택 실패 - 세션이 닫혀 있습니다", dwUID);

            usError = PktUnifiedCharSelectAck::SERVER_ERROR;
        }
        else
        {
            // 일단 데이터를 확인하고, 정리한 후에, Part1 DB중계서버에 선택 결과를 알려준다.
            // Part1 DB중계서버는 해당 계정/캐릭터가 캐쉬에 있으면, 언로드하고,
            // 해당 캐릭터를 파티/길드에서 탈퇴시킨 후 Ack를 보내준다.

            // Ack를 받으면 창고/캐릭터 선택을 시작한다.

            if (!(lpSessionData->GetFirstLogin() & UnifiedConst::SELECTED_PART2))
            {
                // 존재하는 창고를 선택했는지 검증
                if (!lpSessionData->HasUnifiedStoreInfo(cSelectedServerGroupID))
                {
                    usError = PktUnifiedCharSelectAck::WRONG_STORE_SELECTED;
                }
            }

            if(0 == usError)
            {
                // 가져올 대상 캐릭터를 검증한다.
                USER_INFO userInfo = lpSessionData->GetUserInfo();
                
                unsigned long   dwSelectedCID[USER_INFO::MAX_CHAR_NUM]; 
                unsigned long*  lpdwRequestedCID = lpUnifiedSelectReq->dwCID;

                int nSelectedCount = 0;

                std::fill_n(dwSelectedCID, size_t(USER_INFO::MAX_CHAR_NUM), 0);

                // 데이터를 제대로 얻어 왔다.
                // 현재 바인딩 된 캐릭터인지 아닌지 판단한다.
                unsigned char cRestrictedPart1ToPart2Level = 
                    CServerSetup::GetInstance().GetRestrictedPart1ToPart2Level();

                for(int nCount = 0; nCount < USER_INFO::MAX_CHAR_NUM; ++nCount)
                {
                    unsigned long dwBindCID = lpdwRequestedCID[nCount];
                    const UnifiedCharData* lpUnifiedCharData = 0;

                    if (0 == userInfo.CharID[nCount] && 
                        0 != dwBindCID && !userInfo.HasCharacter(dwBindCID) &&
                        0 != (lpUnifiedCharData = lpSessionData->GetUnifiedCharData(dwBindCID)) &&
                        (lpUnifiedCharData->cOldServerGroupID < UnifiedConst::Part1 ||
                         (lpUnifiedCharData->cOldServerGroupID == UnifiedConst::Part1Unified &&
                          cRestrictedPart1ToPart2Level <= lpUnifiedCharData->cLevel)))
                    {
                        dwSelectedCID[nCount] = dwBindCID;
                        ++nSelectedCount;
                    }
                }
                
                if (0 < nSelectedCount &&
                    CClass::MAX_RACE == lpSessionData->CheckUnifiedCharRace(dwSelectedCID))
                {
                    // 캐릭터가 있는데 종족이 다르면 에러!
                    usError = PktUnifiedCharSelectAck::PACKET_ERROR;
                }
                else if (CServerSetup::GetInstance().GetMaxTransferPart1ToPart2Count() <
                    nSelectedCount + lpSessionData->GetTransferedCharCount(CDBSingleObject::GetInstance()))
                {
                    // 최대 캐릭터 선택 회수 이상 선택하면 에러
                    usError = PktUnifiedCharSelectAck::TRANSFER_COUNT_OVER;
                }
                else if (0 < nSelectedCount || !(lpSessionData->GetFirstLogin() & UnifiedConst::SELECTED_PART2))
                {
                    // Part1중계서버로 '검증된' 데이터를 보낸다.
                    GET_SINGLE_DISPATCH(lpPart1DBAgentDispatch, CPart1DBAgentDispatch, 
                        CPart1DBAgentDispatch::GetDispatchTable());

                    if (0 != lpPart1DBAgentDispatch)
                    {
                        bSupressSendAck = SendPacket::UnifiedCharSelectReq(lpPart1DBAgentDispatch->GetSendStream(),
                            dwUID, dwRequestKey, dwSelectedCID, USER_INFO::MAX_CHAR_NUM, 
                            cSelectedServerGroupID, cSelectedNation);

                        usError = bSupressSendAck ? 0 : PktUnifiedCharSelectAck::SERVER_ERROR;
                    }
                }
            }
        }
        break;

    case UnifiedConst::Part2Selectable:

        if(0 == (lpSessionData = CSessionDataMgr::GetInstance().GetOpenSession(dwUID)))
        {
            // 해당 UID가 없다.
            ERRLOG1(g_Log, "UID:%10u / Part2Selectable 캐릭터 선택 실패 - 세션이 닫혀 있습니다", dwUID);

            usError = PktUnifiedCharSelectAck::SERVER_ERROR;
        }
        else
        {
            if (0 < cSelectedServerGroupID && 
                cSelectedServerGroupID <= CServerSetup::GetInstance().GetSelectableUnifiedServerNum())
            {
                // 여기서는 데이터를 컨버팅 할 필요가 없다.
                // 이미 컨버팅 된 데이터를 넣을 예정이다.

                // 서버군을 선택했다. 선택한 서버군을 세팅한다.            
                lpSessionData->SetOldServerGroupID(cSelectedServerGroupID);

                if(!lpSessionData->GetUserInfoFromDB(CDBSingleObject::GetInstance()))
                {
                    ERRLOG3(g_Log, "UID:%10u / FirstLogin:%d / SelectedServerGroupID:%d / "
                        "Part2Selectable 캐릭터 선택 실패 - 유저 정보 DB에서 읽기 실패.", 
                        dwUID, lpSessionData->GetFirstLogin(), cSelectedServerGroupID);

                    usError = PktUnifiedCharSelectAck::SERVER_ERROR;
                }
            }
            else
            {
                ERRLOG3(g_Log, "UID:%10u / FirstLogin:%d / SelectedServerGroupID:%d / "
                    "Part2Selectable 캐릭터 선택 실패 - FirstLogin이나 SelectedServerGroupID값이 다릅니다.", 
                    dwUID, lpSessionData->GetFirstLogin(), cSelectedServerGroupID);

                usError = PktUnifiedCharSelectAck::PACKET_ERROR;
            }
        }
        break;

    default:
            
        usError = PktUnifiedCharSelectAck::PACKET_ERROR;
        break;
    }

    return !bSupressSendAck ? 
        SendPacket::UnifiedCharSelectAck(SendStream, dwUID, dwRequestKey, usError) : true;
}   


}
}