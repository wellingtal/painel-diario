#include "stdafx.h"
#include "ParseCharUpdate.h"

#include <Network/Stream/SendStream.h>

#include <Network/Packet/PacketStruct/FriendPacket.h>
#include <Network/Packet/PacketStruct/CharItemPacket.h>
#include <Network/Packet/PacketStruct/CharQuestPacket.h>
#include <Network/Packet/PacketStruct/CharConfigPacket.h>

#include <Network/Packet/PacketStruct/CharConfigPacketStruct.h>
#include <Network/Packet/PacketStruct/CharLoginOutPacketStruct.h>
#include <Network/SendPacket/SendServerInfo.h>
#include <Network/SendPacket/SendLoginout.h>
#include <Network/SendPacket/SendCharManage.h>

#include <Log/ServerLog.h>
#include <Utility/Setup/ServerSetup.h>

#include <DataStorage/CharacterData.h>
#include <DataStorage/StoreData.h>
#include <DataStorage/SessionData.h>
#include <DataStorage/SessionDataMgr.h>
#include <DataStorage/DBItemSerialMgr.h>

#include <DB/DBComponent.h>
#include <Ranking/RankingDBMgr.h>

#include <Community/Party/PartyDBInfo.h>
#include <Community/Party/PartyDBMgr.h>

#include <Community/Guild/GuildDB.h>
#include <Community/Guild/GuildDBMgr.h>

#include <Network/Packet/PacketStruct/CharStatusPacket.h>
#include <DataStorage/SessionData.h>

#include <Network/Packet/PacketStruct/CharCommunityPacket.h>

#include <Network/Packet/PacketStruct/CharAdminPacket.h>
#include <DB/GameDBComponent.h>

namespace DBAgent
{
namespace ParseCharUpdate
{

//// 캐릭터 로그인
bool CharLogin(CSendStream& SendStream, 
    unsigned long dwServerID, PktDBUpdate* lpPktDBUpdate);

// 캐릭터 로그아웃
bool CharLogout(CSendStream& SendStream, unsigned long dwServerID, 
                Item::CItemSerialMgr& itemSerialMgr, PktDBUpdate* lpPktDBUpdate);

// 캐릭터 존이동
bool CharMoveZone(CSendStream& SendStream, unsigned long dwServerID, 
                  Item::CItemSerialMgr& itemSerialMgr, PktDBUpdate* lpPktDBUpdate);

// 캐릭터 업데이트
bool CharUpdate(CSendStream& SendStream, unsigned long dwServerID, 
                Item::CItemSerialMgr& itemSerialMgr, PktDBUpdate* lpPktDBUpdate);


bool Parse(CSendStream& SendStream, unsigned long dwServerID, 
           Item::CItemSerialMgr& itemSerialMgr, PktDBUpdate* lpPktDBUpdate)
{    
    bool bResult = false;

    switch(lpPktDBUpdate->m_TypeCode)
    {
    case DBUpdateData::LOGIN:	            // 캐릭터 로그인
        bResult = CharLogin(SendStream, dwServerID, lpPktDBUpdate);
        break;

    case DBUpdateData::LOGOUT:          	// 캐릭터 로그아웃
    case DBUpdateData::ABNORMAL_LOGOUT:     // 캐릭터 비정상 로그아웃(데이터 업데이트 없음)
        bResult = CharLogout(SendStream, dwServerID, itemSerialMgr, lpPktDBUpdate);
        break;

    case DBUpdateData::ZONEMOVE:            // 캐릭터 존이동
        bResult = CharMoveZone(SendStream, dwServerID, itemSerialMgr, lpPktDBUpdate);
        break;

    case DBUpdateData::UPDATE:              // 캐릭터 업데이트
        bResult = CharUpdate(SendStream, dwServerID, itemSerialMgr, lpPktDBUpdate);
        break;
            
    default:
        
        ERRLOG1(g_Log, "ParseCharUpdate 실패 : 알 수 없는 명령 0x%04X 를 받았습니다.", lpPktDBUpdate->m_TypeCode);
        bResult = true;        
        break;
    }

    return bResult;
}

// 캐릭터 로그인
bool CharLogin(CSendStream& SendStream, unsigned long dwServerID, PktDBUpdate* lpPktDBUpdate)
{
	// 유저가 로그인을 시도했다.
	// DBAgent는 해당 패킷을 받았다. UID서버로 인증을 위해 패킷을 전달한다.

    unsigned long dwRequestKey  = lpPktDBUpdate->m_dwRequestKey;
    unsigned long dwSessionID   = lpPktDBUpdate->m_dwSessionID;
    unsigned long dwUID         = lpPktDBUpdate->m_dwUserID;
    unsigned long dwCID         = lpPktDBUpdate->m_dwCharID;

#ifdef ENABLE_PACKET_LOG
	DETLOG4(g_PacketLog, "ParseCharUpdate::CharLogin : UID:%10u / CID:%10u / SessionID:%10u / ServerID:0x%08X / ",
		dwUID, dwCID, dwSessionID, dwServerID);
#endif


    IN_ADDR	peerAddress = lpPktDBUpdate->m_Address;

    unsigned short usCharLoginError = 0;

    using namespace DataStorage;

    CSessionData* lpSessionData = CSessionDataMgr::GetInstance().GetOpenSession(dwUID);
    
    if(0 == lpSessionData)
    {
        // 세션이 없을 경우에는 로그인 불가. 유저 로그인시 세션 생성 하는데 왜 없어!
        // 유저 로그아웃시에 세션 없애는 것도 아니구!
        ERRLOG5(g_Log, "UID:%10u / CID:%10u / SessionID:%10u / ServerID:0x%08X / SessionData:0x%p / "
            "캐릭터 로그인 실패 : 세션이나 캐릭터 데이터가 없습니다.",
            dwUID, dwCID, dwSessionID, dwServerID, lpSessionData);

        usCharLoginError = 1;
    }
    else if(lpSessionData->GetSessionID() != dwSessionID)
    {
        // 세션 ID가 다릅니다.
        ERRLOG5(g_Log, "UID:%10u / CID:%10u / RealSessionID:%10u / ReceivedSessionID:%10u / ServerID:0x%08X / "
            "캐릭터 로그인 실패 : 세션 ID가 다릅니다", dwUID, dwCID, 
            lpSessionData->GetSessionID(), dwSessionID, dwServerID);
        
        usCharLoginError = 1;
    }
    else if(CSessionData::SE_USER_ENABLED == lpSessionData->GetSessionState() ||
        CSessionData::SE_CHAR_ENABLED == lpSessionData->GetSessionState())
    {
        // 활성화된 캐릭터로  로그인을 시도했습니다.
        ERRLOG5(g_Log, "UID:%10u / CID:%10u / SessionID:%10u / ServerID:0x%08X / ST:%s"
            "캐릭터 로그인 실패 : 활성화된 세션으로 로그인을 시도하였습니다", dwUID, dwCID, 
            dwSessionID, dwServerID, g_szSessionStateString[lpSessionData->GetSessionState()]);

        usCharLoginError = 1;
    }
    else if(!lpSessionData->HasCharacter(dwCID))
    {
        // 내가 갖고있는 캐릭터가 아닙니다.
        const USER_INFO& userInfo = lpSessionData->GetUserInfo();

		// WORK_LIST 2.1 계정 국적 추가
        ERRLOG10(g_Log, "UID:%10u / CID:%10u / SessionID:%10u / ServerID:0x%08X / "
			"Char1:%10u / Char2:%10u / Char3:%10u / Char4:%10u / Char5:%10u / Nation:%d /"
            "캐릭터 로그인 실패 : 소유하지 않은 캐릭터로 로그인을 시도하였습니다", 
            dwUID, dwCID, dwSessionID, dwServerID, 
            userInfo.CharID[0], userInfo.CharID[1], userInfo.CharID[2],
			userInfo.CharID[3], userInfo.CharID[4], userInfo.Nation);

        usCharLoginError = 5;        
    }
    else 
    {
		// edith 2008.01.15 UI에 Login 관련패킷은 UpdateUIDTable 로 이루어져있다 .여기에 Pass 추가됨.
        // edith 2008.01.15 UID서버로 캐릭터를 로그인시켜도 되는지를 물어본다.        
        lpSessionData->PushRequest(
            RequestData(dwRequestKey, dwCID, dwServerID, dwSessionID, peerAddress, 
            lpSessionData->GetAccountName(), NULL));

        if(!SendPacket::UpdateUIDTable(PktUUT::UpdateUIDTableCharLogin,
            lpSessionData->GetAccountName(), NULL, dwUID, dwCID, dwSessionID, dwServerID, peerAddress))
        {
            ERRLOG4(g_Log, "UID:%10u / CID:%10u / SessionID:%10u / ServerID:0x%08X / "
                "캐릭터 로그인 실패 : UID서버로 UpdateUIDTableCharLogin 전송 실패",
                dwUID, dwCID, dwSessionID, dwServerID);

            lpSessionData->PopRequest();
            usCharLoginError = 1;
        }
        else
        {
            INFLOG4(g_Log, "UID:%10u / CID:%10u / SessionID:%10u / ServerID:0x%08X / "
                "캐릭터 로그인 시도 : UID서버로 UpdateUIDTableCharLogin 전송 성공",
                dwUID, dwCID, dwSessionID, dwServerID);
        }
    }

    if(0 != usCharLoginError)
    {
        SendPacket::CharLoginError(SendStream, dwRequestKey, 
            DBUpdateData::LOGIN, dwUID, dwCID, usCharLoginError);
    }
    
    return true;
}

// 캐릭터 로그아웃
bool CharLogout(CSendStream& SendStream, unsigned long dwServerID, 
                Item::CItemSerialMgr& itemSerialMgr, PktDBUpdate* lpPktDBUpdate)
{
    unsigned __int64 dwItemSerial   = lpPktDBUpdate->m_dlItemSerial;
    unsigned long dwSessionID	    = lpPktDBUpdate->m_dwSessionID;
    unsigned long dwUID             = lpPktDBUpdate->m_dwUserID;
    unsigned long dwCID             = lpPktDBUpdate->m_dwCharID;
	unsigned char cAdminFlag		= lpPktDBUpdate->m_cAdminFlag;

#ifdef ENABLE_PACKET_LOG
	DETLOG4(g_PacketLog, "ParseCharUpdate::CharLogout : UID:%10u / CID:%10u / SessionID:%10u / ServerID:0x%08X / ",
		dwUID, dwCID, dwSessionID, dwServerID);
#endif

    using namespace DataStorage;

    CSessionData* lpSessionData = CSessionDataMgr::GetInstance().GetOpenSession(dwUID);
    CCharacterData* lpCharacterData = 0; 
	
    if(0 == lpSessionData || 0 == (lpCharacterData = lpSessionData->GetCharacterData()))
    {
        // 서버 세션 얻기 실패
        ERRLOG6(g_Log, "UID:%10u / CID:%10u / SessionID:%10u / ServerID:0x%08X / SessionData:0x%p / CharacterData:0x%p /"
            "캐릭터 로그아웃 실패 : 세션이나 캐릭터 데이터가 없습니다.",
            dwUID, dwCID, dwSessionID, dwServerID, lpSessionData, lpCharacterData);
    }
	else if(!lpSessionData->GetRealmPointCheck())
	{
	}
    else if(lpSessionData->GetSessionID() != dwSessionID || 
        lpSessionData->GetServerID() != dwServerID ||
        dwCID != lpSessionData->GetCID() || dwCID != lpCharacterData->GetCID())
    {
        // 세션ID / 서버 ID / CID다름
        ERRLOG8(g_Log, "UID:%10u / CID:%10u / SessionID:%10u / ServerID:0x%08X / "
            "SessionDataCID:%10u / CharacterDataCID:%10u / "
            "SessionDataSessionID:%10u / SessionDataServerID:0x%08X / "
            "캐릭터 로그아웃 실패 : 세션ID or 서버ID or CID다름",
            dwUID, dwCID, dwSessionID, dwServerID, lpSessionData->GetCID(), lpCharacterData->GetCID(),
            lpSessionData->GetSessionID(), lpSessionData->GetServerID());
    }
    else if(CSessionData::SE_CHAR_ENABLED != lpSessionData->GetSessionState())
    {
        // 캐릭터 상태가 활성화 상태가 아닙니다.
        ERRLOG4(g_Log, "UID:%10u / CID:%10u / SessionID:%10u / ServerID:0x%08X / "
            "캐릭터 로그아웃 실패 : 캐릭터 활성화 상태가 아닙니다.",
            dwUID, dwCID, dwSessionID, dwServerID);
    }
    else
    {
        // 캐릭터 데이터 업데이트
        unsigned long dwDataLen = lpPktDBUpdate->GetLen() - sizeof(PktDBUpdate);

        if(lpPktDBUpdate->m_TypeCode == DBUpdateData::LOGOUT && 
            sizeof(PktDBUpdate) <= lpPktDBUpdate->GetLen())
        {
            if(!lpCharacterData->SerializeIn(reinterpret_cast<char*>(lpPktDBUpdate + 1), 
                lpPktDBUpdate->m_usUpdate, dwDataLen, DBUpdateData::MAX_UPDATE_DB))
            {
                ERRLOG4(g_Log, "UID:%10u / CID:%10u / SessionID:%10u / ServerID:0x%08X / "
                    "캐릭터 로그아웃 이상 : 캐릭터 데이터 업데이트 실패",
                    dwUID, dwCID, dwSessionID, dwServerID);                
            }
            else                
            {
                // 데이터 업데이트 성공. 랭킹을 업데이트한다.
                const CHAR_INFOST& charInfo = lpCharacterData->GetInfo();								

                CRankingDBMgr::GetInstance().UpdateRanking(
                    RankingNode(charInfo.Name, charInfo.Fame, 
                    static_cast<unsigned char>(charInfo.Level), 
                    static_cast<unsigned char>(charInfo.Class)));
            }
        }

		lpCharacterData->SetAdminFlag(cAdminFlag);		

        // 파티에 로그아웃 일림
        CPartyDBInfo* lpParty = CPartyDBMgr::GetInstance().OpenParty(
            CDBSingleObject::GetInstance(), lpCharacterData->GetPID());

        if (0 != lpParty)
        {
            CPartyDBMgr::GetInstance().LogoutPartyMember(
                CDBSingleObject::GetInstance(), lpParty, dwCID);
        }

        // 길드에 로그아웃 알림
        CGuildDB* lpGuild = static_cast<CGuildDB*>(
            CGuildDBMgr::GetInstance().GetGuild(lpCharacterData->GetGID()));

        if (0 != lpGuild)
        {
            lpGuild->SendLoginOutMember(dwCID, 0);
        }
		
        // 캐릭터 비활성화.
        if(!lpSessionData->CharDisable(dwCID, dwServerID))
        {
            ERRLOG4(g_Log, "UID:%10u / CID:%10u / SessionID:%10u / ServerID:0x%08X / "
                "캐릭터 로그아웃 실패 : 캐릭터 비활성화 실패", dwUID, dwCID, dwSessionID, dwServerID);
        }
        else
        {
            INFLOG4(g_Log, "UID:%10u / CID:%10u / SessionID:%10u / ServerID:0x%08X / "
                "캐릭터 로그아웃 성공", dwUID, dwCID, dwSessionID, dwServerID);

            // 로그아웃했으니, 세션을 닫는다.
            CSessionDataMgr::GetInstance().SessionClose(dwUID, dwSessionID);			

			if (GameRYL::CHINA == CServerSetup::GetInstance().GetNationType())
			{
				// LogOut 처리를 해준다.
				DBComponent::GameDB::UpdatePlayTime(CDBSingleObject::GetInstance(), dwUID, 1);
			}
        }
    }

    // 아이템 시리얼 업데이트
    if(itemSerialMgr.SetItemSerial(dwItemSerial))
    {
        // 시리얼이 바뀌었으므로 DB에 업데이트한다.
        if(!itemSerialMgr.SaveItemSerial(CDBSingleObject::GetInstance(), dwServerID))
        {   
            ERRLOG4(g_Log, "UID:%10u / CID:%10u / ServerID:0x%08X / Serial:0x%016I64X / 캐릭터 로그아웃 : 아이템 시리얼 업데이트 실패", 
                dwUID, dwCID, dwServerID, itemSerialMgr.GetItemSerial());
        }
    }

    return true;
}

// 캐릭터 존이동
bool CharMoveZone(CSendStream& SendStream, unsigned long dwServerID, 
                  Item::CItemSerialMgr& itemSerialMgr, PktDBUpdate* lpPktDBUpdate)
{
    unsigned __int64 dwItemSerial = lpPktDBUpdate->m_dlItemSerial;
    unsigned long dwSessionID	= lpPktDBUpdate->m_dwSessionID;
    unsigned long dwUID         = lpPktDBUpdate->m_dwUserID;
    unsigned long dwCID         = lpPktDBUpdate->m_dwCharID;

#ifdef ENABLE_PACKET_LOG
	DETLOG4(g_PacketLog, "ParseCharUpdate::CharMoveZone : UID:%10u / CID:%10u / SessionID:%10u / ServerID:0x%08X / ",
		dwUID, dwCID, dwSessionID, dwServerID);
#endif

    using namespace DataStorage;

    CSessionData* lpSessionData = CSessionDataMgr::GetInstance().GetOpenSession(dwUID);
    CCharacterData* lpCharacterData = 0; 

    if(0 == lpSessionData || 0 == (lpCharacterData = lpSessionData->GetCharacterData()))
    {
        // 서버 세션 얻기 실패
        ERRLOG6(g_Log, "UID:%10u / CID:%10u / SessionID:%10u / ServerID:0x%08X / SessionData:0x%p / CharacterData:0x%p "
            "캐릭터 존이동 실패 : 세션 또는 캐릭터 데이터가 없습니다", 
            dwUID, dwCID, dwSessionID, dwServerID, lpSessionData, lpCharacterData);
    }
    else if(lpSessionData->GetSessionID() != dwSessionID || 
        lpSessionData->GetServerID() != dwServerID ||
        dwCID != lpSessionData->GetCID() || dwCID != lpCharacterData->GetCID())
    {
        // 세션ID / 서버 ID / CID다름
        ERRLOG8(g_Log, "UID:%10u / CID:%10u / SessionID:%10u / ServerID:0x%08X / "
            "SessionDataCID:%10u / CharacterDataCID:%10u / "
            "SessionDataSessionID:%10u / SessionDataServerID:0x%08X / "
            "캐릭터 존이동 실패 : 세션ID or 서버ID or CID다름",
            dwUID, dwCID, dwSessionID, dwServerID, lpSessionData->GetCID(), lpCharacterData->GetCID(),
            lpSessionData->GetSessionID(), lpSessionData->GetServerID());
    }
    else if(CSessionData::SE_CHAR_ENABLED != lpSessionData->GetSessionState())
    {
        // 캐릭터 상태가 활성화 상태가 아닙니다.
        ERRLOG4(g_Log, "UID:%10u / CID:%10u / SessionID:%10u / ServerID:0x%08X / "
            "캐릭터 존이동 실패 : 캐릭터 활성화 상태가 아닙니다.",
            dwUID, dwCID, dwSessionID, dwServerID);
    }
    else
    {
        // 캐릭터 데이터 업데이트
        unsigned long dwDataLen = lpPktDBUpdate->GetLen() - sizeof(PktDBUpdate);

        if(lpPktDBUpdate->m_TypeCode == DBUpdateData::ZONEMOVE && 
            sizeof(PktDBUpdate) <= lpPktDBUpdate->GetLen())
        {
            if(!lpCharacterData->SerializeIn(reinterpret_cast<char*>(lpPktDBUpdate + 1), 
                lpPktDBUpdate->m_usUpdate, dwDataLen, DBUpdateData::MAX_UPDATE_DB))
            {
                ERRLOG4(g_Log, "UID:%10u / CID:%10u / SessionID:%10u / ServerID:0x%08X / "
                    "캐릭터 존이동 이상 : 캐릭터 데이터 업데이트 실패",
                    dwUID, dwCID, dwSessionID, dwServerID);
            }
            else
            {
                // 데이터 업데이트 성공. 랭킹을 업데이트한다.
                const CHAR_INFOST& charInfo = lpCharacterData->GetInfo();

                CRankingDBMgr::GetInstance().UpdateRanking(
                    RankingNode(charInfo.Name, charInfo.Fame, 
                    static_cast<unsigned char>(charInfo.Level), 
                    static_cast<unsigned char>(charInfo.Class)));
            }
        }

        // 파티에 로그아웃 일림
        CPartyDBInfo* lpParty = CPartyDBMgr::GetInstance().OpenParty(
            CDBSingleObject::GetInstance(), lpCharacterData->GetPID());

        if (0 != lpParty)
        {
            CPartyDBMgr::GetInstance().LogoutPartyMember(
                CDBSingleObject::GetInstance(), lpParty, dwCID);
        }

        // 길드에 로그아웃 알림
        CGuildDB* lpGuild = static_cast<CGuildDB*>(
            CGuildDBMgr::GetInstance().GetGuild(lpCharacterData->GetGID()));

        if (0 != lpGuild)
        {
            lpGuild->SendLoginOutMember(dwCID, 0);
        }

		// 캐릭터 비활성화.
		if(!lpSessionData->CharMove(dwCID, dwServerID))
		{
			ERRLOG4(g_Log, "UID:%10u / CID:%10u / SessionID:%10u / ServerID:0x%08X / "
				"캐릭터 존이동 실패 : 캐릭터 비활성화 실패", dwUID, dwCID, dwSessionID, dwServerID);
		}
		else
		{
			INFLOG4(g_Log, "UID:%10u / CID:%10u / SessionID:%10u / ServerID:0x%08X / "
				"캐릭터 존이동 성공", dwUID, dwCID, dwSessionID, dwServerID);

			// 존이동시에는 세션을 닫지 않는다.
			// CSessionDataMgr::GetInstance().SessionClose(dwUID, dwSessionID);

			if (GameRYL::CHINA == CServerSetup::GetInstance().GetNationType())
			{
				// LogOut 처리를 해준다.
				DBComponent::GameDB::UpdatePlayTime(CDBSingleObject::GetInstance(), dwUID, 1);
			}
		}
    }

    // 아이템 시리얼 업데이트
    if(itemSerialMgr.SetItemSerial(dwItemSerial))
    {
        // 시리얼이 바뀌었으므로 DB에 업데이트한다.
        if(!itemSerialMgr.SaveItemSerial(CDBSingleObject::GetInstance(), dwServerID))
        {   
            ERRLOG4(g_Log, "UID:%10u / CID:%10u / ServerID:0x%08X / Serial:0x%016I64X / 캐릭터 존이동 : 아이템 시리얼 업데이트 실패", 
                dwUID, dwCID, dwServerID, itemSerialMgr.GetItemSerial());
        }
    }

    return true;
}

// 캐릭터 업데이트
bool CharUpdate(CSendStream& SendStream, unsigned long dwServerID, 
                Item::CItemSerialMgr& itemSerialMgr, PktDBUpdate* lpPktDBUpdate)
{
    unsigned __int64 dwItemSerial = lpPktDBUpdate->m_dlItemSerial;
    unsigned long dwSessionID	= lpPktDBUpdate->m_dwSessionID;
    unsigned long dwUID         = lpPktDBUpdate->m_dwUserID;
    unsigned long dwCID         = lpPktDBUpdate->m_dwCharID;

#ifdef ENABLE_PACKET_LOG
	DETLOG4(g_PacketLog, "ParseCharUpdate::CharUpdate : UID:%10u / CID:%10u / SessionID:%10u / ServerID:0x%08X / ",
		dwUID, dwCID, dwSessionID, dwServerID);
#endif

    using namespace DataStorage;

    CSessionData* lpSessionData = CSessionDataMgr::GetInstance().GetOpenSession(dwUID);
    CCharacterData* lpCharacterData = 0; 

    if(0 == lpSessionData || 0 == (lpCharacterData = lpSessionData->GetCharacterData()))
    {
        // 서버 세션 얻기 실패
        ERRLOG6(g_Log, "UID:%10u / CID:%10u / SessionID:%10u / ServerID:0x%08X / SessionData:0x%p / CharacterData:0x%p /"
            "캐릭터 데이터 업데이트 실패 : 세션이나 캐릭터 데이터가 없습니다",
            dwUID, dwCID, dwSessionID, dwServerID, lpSessionData, lpCharacterData);
    }
    else if(lpSessionData->GetSessionID() != dwSessionID || 
        lpSessionData->GetServerID() != dwServerID ||
        dwCID != lpSessionData->GetCID() || dwCID != lpCharacterData->GetCID())
    {
        // 세션ID / 서버 ID / CID다름
        ERRLOG8(g_Log, "UID:%10u / CID:%10u / SessionID:%10u / ServerID:0x%08X / "
            "SessionDataCID:%10u / CharacterDataCID:%10u / "
            "SessionDataSessionID:%10u / SessionDataServerID:0x%08X / "
            "캐릭터 데이터 업데이트 실패 : 세션ID or 서버ID or CID다름",
            dwUID, dwCID, dwSessionID, dwServerID, lpSessionData->GetCID(), lpCharacterData->GetCID(),
            lpSessionData->GetSessionID(), lpSessionData->GetServerID());
    }
    else if(CSessionData::SE_CHAR_ENABLED != lpSessionData->GetSessionState())
    {
        // 캐릭터 상태가 활성화 상태가 아닙니다.
        ERRLOG4(g_Log, "UID:%10u / CID:%10u / SessionID:%10u / ServerID:0x%08X / "
            "캐릭터 데이터 업데이트 실패 : 캐릭터 활성화 상태가 아닙니다.",
            dwUID, dwCID, dwSessionID, dwServerID);
    }
    else
    {
        // 캐릭터 데이터 업데이트
        unsigned long dwDataLen = lpPktDBUpdate->GetLen() - sizeof(PktDBUpdate);

        if(lpPktDBUpdate->m_TypeCode == DBUpdateData::UPDATE && 
            sizeof(PktDBUpdate) <= lpPktDBUpdate->GetLen())
        {
            if(!lpCharacterData->SerializeIn(reinterpret_cast<char*>(lpPktDBUpdate + 1), 
                lpPktDBUpdate->m_usUpdate, dwDataLen, DBUpdateData::MAX_UPDATE_DB))
            {
                ERRLOG4(g_Log, "UID:%10u / CID:%10u / SessionID:%10u / ServerID:0x%08X / "
                    "캐릭터 데이터 업데이트 : 캐릭터 데이터 업데이트 실패",
                    dwUID, dwCID, dwSessionID, dwServerID);
            }
            else
            {
                // 데이터 업데이트 성공. 랭킹을 업데이트한다.
                const CHAR_INFOST& charInfo = lpCharacterData->GetInfo();

                CRankingDBMgr::GetInstance().UpdateRanking(
                    RankingNode(charInfo.Name, charInfo.Fame, 
                    static_cast<unsigned char>(charInfo.Level), 
                    static_cast<unsigned char>(charInfo.Class)));
            }
        }
    }

    // 아이템 시리얼 업데이트
    if(itemSerialMgr.SetItemSerial(dwItemSerial))
    {
        // 시리얼이 바뀌었으므로 DB에 업데이트한다.
        if(!itemSerialMgr.SaveItemSerial(CDBSingleObject::GetInstance(), dwServerID))
        {   
            ERRLOG4(g_Log, "UID:%10u / CID:%10u / ServerID:0x%08X / Serial:0x%016I64X / 캐릭터 업데이트 : 아이템 시리얼 업데이트 실패", 
                dwUID, dwCID, dwServerID, itemSerialMgr.GetItemSerial());
        }
    }

    return true;
}



bool UpdateDeposit(CSendStream& SendStream, PktBase* lpPktBase)
{    
    PktDeposit* lpPktDeposit = static_cast<PktDeposit*>(lpPktBase);

    unsigned char cCmd  = lpPktDeposit->m_cCmd;
    char* lpData        = lpPktDeposit->m_szData + sizeof(unsigned long);

    unsigned long dwUID = 0;
    memcpy(&dwUID, lpPktDeposit->m_szData, sizeof(unsigned long));

#ifdef ENABLE_PACKET_LOG
	DETLOG4(g_PacketLog, "ParseCharUpdate::UpdateDeposit : UID:%10u / CID:%10u / SessionID:%10u / ServerID:0x%08X / ",
		dwUID, 0, 0, 0);
#endif

    char szPassword[Deposit::PASSWORD_LENGTH + 1];
    memset(szPassword, 0, sizeof(char) * (Deposit::PASSWORD_LENGTH + 1));

    unsigned long dwGold = 0;

    switch(cCmd)
    {
    case PktDeposit::CMD::PASSWORD: memcpy(szPassword, lpData, Deposit::PASSWORD_LENGTH);   break;
    case PktDeposit::CMD::GOLD:     memcpy(&dwGold, lpData, sizeof(unsigned long));         break;
    }

    // 접속 DB에서 세션 확인
    using namespace DataStorage;
    CSessionData*   lpSessionData = CSessionDataMgr::GetInstance().GetOpenSession(dwUID);
    CStoreData*     lpStoreData = 0;

    if(0 == lpSessionData || 0 == (lpStoreData = lpSessionData->GetStoreData()))
    {
        ERRLOG6(g_Log, "UID:%10u / Cmd:%u / Password:%s / Gold:%10u / SessionData:0x%p / StoreData:0x%p / "
            "창고 정보 업데이트 실패 : 세션이나 창고 데이터가 없습니다", 
            dwUID, cCmd, szPassword, dwGold, lpSessionData, lpStoreData);
    }
    else if(CSessionData::SE_CHAR_ENABLED != lpSessionData->GetSessionState())
    {
        ERRLOG4(g_Log, "UID:%10u / Cmd:%u / Password:%s / Gold:%10u / 창고 정보 업데이트 실패 : 캐릭터가 활성화되어 있지 않습니다", 
            dwUID, cCmd, szPassword, dwGold);
    }
    else if(dwUID != lpSessionData->GetUID() || dwUID != lpStoreData->GetUID())
    {
        ERRLOG6(g_Log, "UID:%10u / Cmd:%u / SessionUID:%10u / StoreUID:%10u / Password:%s / Gold:%10u / "
            "창고 정보 업데이트 실패 : 창고 UID와 세션UID가 다릅니다", 
            dwUID, cCmd, lpSessionData->GetUID(), lpStoreData->GetUID(), szPassword, dwGold);
    }
    else
    {
        STORE_INFO storeInfo = lpStoreData->GetStoreInfo();

        switch(cCmd)
        {
        case PktDeposit::CMD::PASSWORD:
            memcpy(storeInfo.Password, szPassword, 
                std::min(int(Deposit::PASSWORD_LENGTH), int(STORE_INFO::MAX_PASS_LEN)));

            storeInfo.Password[STORE_INFO::MAX_PASS_LEN - 1] = 0;
            break;

        case PktDeposit::CMD::GOLD:
            storeInfo.Gold = dwGold;
            break;
        }

        lpStoreData->SetStoreInfo(storeInfo);
    }

    return true;
}

bool UpdateDepositDB(CSendStream& SendStream, PktBase* lpPktBase)
{
    PktDepositUpdateDB* lpPktDepositUpdateDB = 
        static_cast<PktDepositUpdateDB*>(lpPktBase);

    unsigned long   dwUID       = lpPktDepositUpdateDB->m_dwUID;
    unsigned long   dwCID       = lpPktDepositUpdateDB->m_dwCID;
    unsigned long   dwTabFlag   = lpPktDepositUpdateDB->m_dwTabFlag;
    unsigned short  usDataSize  = lpPktDepositUpdateDB->m_usDataSize;
    unsigned char   cTabNum     = lpPktDepositUpdateDB->m_cTabNum;

#ifdef ENABLE_PACKET_LOG
	DETLOG4(g_PacketLog, "ParseCharUpdate::UpdateDepositDB : UID:%10u / CID:%10u / SessionID:%10u / ServerID:0x%08X / ",
		dwUID, dwCID, 0, 0);
#endif

    // 접속 DB에서 세션 확인
    using namespace DataStorage;
    CSessionData*   lpSessionData = CSessionDataMgr::GetInstance().GetOpenSession(dwUID);
    CStoreData*     lpStoreData = 0;

    if(0 == lpSessionData || 0 == (lpStoreData = lpSessionData->GetStoreData()))
    {
        ERRLOG4(g_Log, "UID:%10u / CID:%10u / SessionData:0x%p / StoreData:0x%p / 창고 정보 업데이트 실패 : 세션이 없거나 창고 데이터가 없습니다.", 
            dwUID, dwCID, lpSessionData, lpStoreData);
    }
    else if(CSessionData::SE_CHAR_ENABLED != lpSessionData->GetSessionState())
    {
        ERRLOG2(g_Log, "UID:%10u / CID:%10u / 창고 정보 업데이트 실패 : 캐릭터가 활성화되어 있지 않습니다", 
            dwUID, dwCID);
    }
    else if(dwUID != lpSessionData->GetUID() || dwUID != lpStoreData->GetUID())
    {
        ERRLOG4(g_Log, "UID:%10u / CID:%10u / SessionUID:%10u / StoreUID:%10u / "
            "창고 정보 업데이트 실패 : 창고 UID와 세션UID가 다릅니다", 
            dwUID, dwCID, lpSessionData->GetUID(), lpStoreData->GetUID());
    }
    else
    {
        STORE_INFO storeInfo = lpStoreData->GetStoreInfo();

        if(storeInfo.Flag != dwTabFlag)
        {
            storeInfo.Flag = dwTabFlag;
            lpStoreData->SetStoreInfo(storeInfo);
        }

        if (PktDepositUpdateDB::TabNum::TAB_12 == cTabNum)
        {
            lpStoreData->SetStore1(reinterpret_cast<char*>(lpPktDepositUpdateDB + 1), usDataSize);
        }
        else if (PktDepositUpdateDB::TabNum::TAB_34 == cTabNum)
        {
            lpStoreData->SetStore2(reinterpret_cast<char*>(lpPktDepositUpdateDB + 1), usDataSize);
        }
    }

    return true;
}


bool UpdateFriendDB(CSendStream& SendStream, PktBase* lpPktBase)
{    
    PktFriendDB* lpPktFriendDB = static_cast<PktFriendDB*>(lpPktBase);

    unsigned char	cCmd		= lpPktFriendDB->m_cCmd;
    unsigned long	dwUID		= lpPktFriendDB->m_dwOwnerUID;
    unsigned long	dwCID		= lpPktFriendDB->m_dwOwnerCID;
    unsigned long	dwRefCID	= lpPktFriendDB->m_dwReferenceCID;
    unsigned long	dwData		= lpPktFriendDB->m_dwData;
	
	unsigned long	dwGID		= lpPktFriendDB->m_dwGID;				// 길드 정보.
	unsigned short	wClass		= lpPktFriendDB->m_wClass;				// 클래스 정보.
	char			cLevel		= lpPktFriendDB->m_cLevel;				// 레벨 정보.
	unsigned long	dwServerID	= lpPktFriendDB->m_dwServerID;

#ifdef ENABLE_PACKET_LOG
	DETLOG4(g_PacketLog, "ParseCharUpdate::UpdateFriendDB : UID:%10u / CID:%10u / SessionID:%10u / ServerID:0x%08X / ",
		dwUID, dwCID, 0, dwServerID);
#endif

    using namespace DataStorage;
    CSessionData*   lpSessionData = CSessionDataMgr::GetInstance().GetOpenSession(dwUID);
    CCharacterData* lpCharacterData = 0;

    CSessionData*   lpRefSessionData = 0;
    CCharacterData* lpRefCharacterData = 0;

    unsigned long   dwRefSessionDataCID = 0;
    unsigned long   dwRefCharacterDataCID = 0;

    if(0 == lpSessionData || 0 == (lpCharacterData = lpSessionData->GetCharacterData()))
    {
        // 세션 없거나 데이터 없음
        ERRLOG5(g_Log, "UID:%10u / CID:%10u / RefCID:%10u / SessionData:0x%p / CharacterData:0x%p / "
            "친구 패킷 처리 실패 : 세션이 없거나, 세션에 캐릭터 데이터가 없습니다.", 
            dwUID, dwCID, dwRefCID, lpSessionData, lpCharacterData);
    }
    else if(CSessionData::SE_CHAR_ENABLED != lpSessionData->GetSessionState())
    {
        // 세션 상태 이상
        ERRLOG4(g_Log, "UID:%10u / CID:%10u / RefCID:%10u / ST:%s / "
            "친구 패킷 처리 실패 : 세션이 활성화되지 않았습니다", 
            dwUID, dwCID, dwRefCID, g_szSessionStateString[lpSessionData->GetSessionState()]);
    }
    else if(dwCID != lpCharacterData->GetCID() || dwCID != lpSessionData->GetCID())
    {
        // CID이상
        ERRLOG5(g_Log, "UID:%10u / CID:%10u / RefCID:%10u / SessionDataCID:%10u / CharacterDataCID:%10u / "
            "친구 패킷 처리 실패 : 세션 또는 캐릭터의 CID가 이상합니다", 
            dwUID, dwCID, dwRefCID, lpSessionData->GetCID(), lpCharacterData->GetCID());
    }
    else
    {
        switch(cCmd)
        {
        case PktFriendDB::ADD_FRIEND:

            lpRefSessionData = 
                CSessionDataMgr::GetInstance().GetCharLoadedSession(dwRefCID);

            if(0 == lpRefSessionData || 
                0 == (lpRefCharacterData = lpRefSessionData->GetCharacterData()) ||
                dwRefCID != (dwRefCharacterDataCID = lpRefCharacterData->GetCID()) || 
                dwRefCID != (dwRefSessionDataCID = lpRefSessionData->GetCID()))
            {
                ERRLOG7(g_Log, "UID:%10u / CID:%10u / RefCID:%10u / RefSession:0x%p / RefCharacterData:0x%p / "
                    "RefSessionDataCID:%10u / RefCharacterDataCID:%10u / 친구 패킷 처리 실패 : 친구 추가 실패 - 친구가 로그인하지 않았습니다",
                    dwUID, dwCID, dwRefCID, lpRefSessionData, lpRefCharacterData,
                    dwRefCharacterDataCID, dwRefSessionDataCID);
            }
            else if(!lpCharacterData->AddFriend(dwRefCID, lpRefCharacterData->GetName(), dwGID, wClass, cLevel, dwServerID))
            {
                ERRLOG3(g_Log, "UID:%10u / CID:%10u / RefCID:%10u / 친구 패킷 처리 실패 : 친구 추가 실패",
                    dwUID, dwCID, dwRefCID);
            }
            break;

        case PktFriendDB::REMOVE_FRIEND:
    
            if(!lpCharacterData->RemoveFriend(dwRefCID))
            {
                ERRLOG3(g_Log, "UID:%10u / CID:%10u / RefCID:%10u / 친구 패킷 처리 실패 : 친구 제거 실패",
                    dwUID, dwCID, dwRefCID);
            }
            break;

        case PktFriendDB::ADD_BAN:

            lpRefSessionData = 
                CSessionDataMgr::GetInstance().GetCharLoadedSession(dwRefCID);

            if(0 == lpRefSessionData || 
                0 == (lpRefCharacterData = lpRefSessionData->GetCharacterData()) ||
                dwRefCID != (dwRefCharacterDataCID = lpRefCharacterData->GetCID()) || 
                dwRefCID != (dwRefSessionDataCID = lpRefSessionData->GetCID()))
            {
                ERRLOG7(g_Log, "UID:%10u / CID:%10u / RefCID:%10u / RefSession:0x%p / RefCharacterData:0x%p / "
                    "RefSessionDataCID:%10u / RefCharacterDataCID:%10u / 친구 패킷 처리 실패 : 거부 추가 실패 - 거부 대상자가 로그인하지 않았습니다",
                    dwUID, dwCID, dwRefCID, lpRefSessionData, lpRefCharacterData,
                    dwRefCharacterDataCID, dwRefSessionDataCID);
            }
            else if(!lpCharacterData->AddBan(dwRefCID, lpRefCharacterData->GetName(), lpRefCharacterData->GetGID(),
				lpRefCharacterData->GetClass(), lpRefCharacterData->GetLevel(), dwServerID))
            {
                ERRLOG3(g_Log, "UID:%10u / CID:%10u / RefCID:%10u / 친구 패킷 처리 실패 : 거부 추가 실패",
                    dwUID, dwCID, dwRefCID);
            }
            break;

        case PktFriendDB::REMOVE_BAN:

            if(!lpCharacterData->RemoveBan(dwRefCID))
            {
                ERRLOG3(g_Log, "UID:%10u / CID:%10u / RefCID:%10u / 친구 패킷 처리 실패 : 거부 제거 실패",
                    dwUID, dwCID, dwRefCID);
            }
            break;

        case PktFriendDB::SETGROUP:

            if(!lpCharacterData->SetFriendGroup(dwRefCID, dwData))
            {
                ERRLOG4(g_Log, "UID:%10u / CID:%10u / RefCID:%10u / dwData:0x%08X / 친구 패킷 처리 실패 : 그룹 설정 실패",
                    dwUID, dwCID, dwRefCID, dwData);
            }
            break;		
		case PktFriendDB::FRIEND_INFO:
			{
				// 친구 정보 업데이트 //				
				CFriendList friendList = lpCharacterData->GetFriendList();

				CFriendList::Rebind* lpRebind = friendList.GetFriend(dwRefCID);

				if(lpRebind)
				{
					// 업데이트 //
					lpRebind->UpdateServerID(dwServerID);
					lpRebind->UpdateLevel(cLevel);
					lpRebind->UpdateGID(dwGID);
					lpRebind->UpdateClass(wClass);
					lpCharacterData->SetFriendUpdateData();
					lpCharacterData->UpdateDBData(CDBSingleObject::GetInstance());
				}
			}
			break;
		case PktFriendDB::BAN_INFO:
			{
				// 거부 정보 업데이트 //
				CBanList banList = lpCharacterData->GetBanList();
				
				CBanList::Rebind* lpRebind = banList.GetBan(dwRefCID);

				if(lpRebind)
				{
					// 업데이트 //
					lpRebind->UpdateServerID(dwServerID);
					lpRebind->UpdateLevel(cLevel);
					lpRebind->UpdateGID(dwGID);
					lpRebind->UpdateClass(wClass);
					lpCharacterData->SetBadUpdateData();
					lpCharacterData->UpdateDBData(CDBSingleObject::GetInstance());
				}
			}
			break;
        }

    }

    return true;  
}

bool UpdateQuestDB(CSendStream& SendStream, PktBase* lpPktBase)
{
    PktQuestDB* lpPktQuestDB = static_cast<PktQuestDB*>(lpPktBase);

    unsigned long dwUID = lpPktQuestDB->m_dwUID;
    unsigned long dwCID = lpPktQuestDB->m_dwCID;
    unsigned long dwExecuteQuestSize = lpPktQuestDB->m_wExecuteQuestSize;
    unsigned long dwHistoryQuestSize = lpPktQuestDB->m_wHistoryQuestSize;

#ifdef ENABLE_PACKET_LOG
	DETLOG4(g_PacketLog, "ParseCharUpdate::UpdateQuestDB : UID:%10u / CID:%10u / SessionID:%10u / ServerID:0x%08X / ",
		dwUID, dwCID, 0, 0);
#endif

    using namespace DataStorage;
    CSessionData*   lpSessionData = CSessionDataMgr::GetInstance().GetOpenSession(dwUID);
    CCharacterData* lpCharacterData = 0;

    if(0 == lpSessionData || 0 == (lpCharacterData = lpSessionData->GetCharacterData()))
    {
        // 세션 없거나 데이터 없음
        ERRLOG4(g_Log, "UID:%10u / CID:%10u / SessionData:0x%p / CharacterData:0x%p / "
            "퀘스트 패킷 처리 실패 : 세션이 없거나, 세션에 캐릭터 데이터가 없습니다.", 
            dwUID, dwCID, lpSessionData, lpCharacterData);
    }
    else if(CSessionData::SE_CHAR_ENABLED != lpSessionData->GetSessionState())
    {
        // 세션 상태 이상
        ERRLOG3(g_Log, "UID:%10u / CID:%10u / ST:%s / "
            "퀘스트 패킷 처리 실패 : 세션이 활성화되지 않았습니다", 
            dwUID, dwCID, g_szSessionStateString[lpSessionData->GetSessionState()]);
    }
    else if(dwCID != lpCharacterData->GetCID() || dwCID != lpSessionData->GetCID())
    {
        // CID이상
        ERRLOG4(g_Log, "UID:%10u / CID:%10u / SessionDataCID:%10u / CharacterDataCID:%10u / "
            "퀘스트 패킷 처리 실패 : 세션 또는 캐릭터의 CID가 이상합니다", 
            dwUID, dwCID, lpSessionData->GetCID(), lpCharacterData->GetCID());
    }
    else
    {
        QUEST   quest;
        memset(&quest, 0, sizeof(QUEST));
        quest.dwSize = dwExecuteQuestSize;
        memcpy(quest.Data, lpPktQuestDB + 1, dwExecuteQuestSize);

		SERLOG5(g_Log, "UID:%10u / CID:%10u / SessionDataCID:%10u / CharacterDataCID:%10u / "
			"퀘스트 패킷 처리 : Quest Size : %d", dwUID, dwCID, lpSessionData->GetCID(), lpCharacterData->GetCID(), dwExecuteQuestSize);

        HISTORY history;
        memset(&history, 0, sizeof(HISTORY));
        history.dwSize = dwHistoryQuestSize;
        memcpy(history.Data, reinterpret_cast<char*>(lpPktQuestDB + 1) + dwExecuteQuestSize, 
            dwHistoryQuestSize);

        lpCharacterData->SetQuest(quest);
        lpCharacterData->SetHistory(history);       
    }

    return true;
}

bool UpdateConfigInfoDB(CSendStream& SendStream, PktBase* lpPktBase)
{    
    PktConfigInfo* lpPktConfigInfo = static_cast<PktConfigInfo*>(lpPktBase);

    unsigned long dwUID     = lpPktConfigInfo->m_dwUserID;
    unsigned long dwCID     = lpPktConfigInfo->m_dwCharID;
    unsigned short usSize   = lpPktConfigInfo->m_wSize;

#ifdef ENABLE_PACKET_LOG
	DETLOG4(g_PacketLog, "ParseCharUpdate::UpdateConfigInfoDB : UID:%10u / CID:%10u / SessionID:%10u / ServerID:0x%08X / ",
		dwUID, dwCID, 0, 0);
#endif

    using namespace DataStorage;
    CSessionData*   lpSessionData = CSessionDataMgr::GetInstance().GetOpenSession(dwUID);
    CCharacterData* lpCharacterData = 0;

    if(0 == lpSessionData || 0 == (lpCharacterData = lpSessionData->GetCharacterData()))
    {
        // 세션 없거나 데이터 없음
        ERRLOG4(g_Log, "UID:%10u / CID:%10u / SessionData:%0xp / CharacterData:%0xp / "
            "설정 패킷 처리 실패 : 세션이 없거나, 세션에 캐릭터 데이터가 없습니다.", 
            dwUID, dwCID, lpSessionData, lpCharacterData);
    }
    else if(CSessionData::SE_CHAR_ENABLED != lpSessionData->GetSessionState())
    {
        // 세션 상태 이상
        ERRLOG3(g_Log, "UID:%10u / CID:%10u / ST:%s / "
            "설정 패킷 처리 실패 : 세션이 활성화되지 않았습니다", 
            dwUID, dwCID, g_szSessionStateString[lpSessionData->GetSessionState()]);
    }
    else if(dwCID != lpCharacterData->GetCID() || dwCID != lpSessionData->GetCID())
    {
        // CID이상
        ERRLOG4(g_Log, "UID:%10u / CID:%10u / SessionDataCID:%10u / CharacterDataCID:%10u / "
            "설정 패킷 처리 실패 : 세션 또는 캐릭터의 CID가 이상합니다", 
            dwUID, dwCID, lpSessionData->GetCID(), lpCharacterData->GetCID());
    }
    else
    {
        CONFIG config;
        memset(&config, 0, sizeof(CONFIG));
        
        char* lpSrc = reinterpret_cast<char*>(lpPktConfigInfo + 1);
        char* lpDst = config.Data;

        memcpy(lpDst, lpSrc, sizeof(PeaceModeInfo));
        lpSrc += sizeof(PeaceModeInfo);
        lpDst += sizeof(PeaceModeInfo);

        memcpy(lpDst, lpSrc, sizeof(RejectOption));
        lpSrc += sizeof(RejectOption);
        lpDst += sizeof(RejectOption);

        config.dwSize = sizeof(PeaceModeInfo)  + sizeof(RejectOption);

        lpCharacterData->SetConfig(config);
    }

    return true;
}

bool SaveEnemy(CSendStream& SendStream, PktBase* lpPktBase)
{
	PktSaveEnemy* lpSaveEnemy = static_cast<PktSaveEnemy*>(lpPktBase);
	
	unsigned long dwCID			= lpSaveEnemy->m_dwCID;
	unsigned long dwAttackedCID	= lpSaveEnemy->m_dwAttackedCID;
	unsigned long dwTick		= ::GetTickCount();
	

#ifdef ENABLE_PACKET_LOG
	DETLOG4(g_PacketLog, "ParseCharUpdate::SaveEnemy : UID:%10u / CID:%10u / SessionID:%10u / ServerID:0x%08X / ",
		0, dwCID, 0, 0);
#endif

	using namespace DataStorage;

	CSessionData* lpSessionData	= CSessionDataMgr::GetInstance().GetCharLoadedSession(dwCID);

	if(lpSessionData)
	{
		CSessionData::SaveEnemy* lpSaveEnemyInfo = lpSessionData->GetSaveEnemy();

		for(unsigned char cIndex = 0; cIndex < CSessionData::SaveEnemy::MAX_SAVING_ENEMY; cIndex++)
		{
			if(lpSaveEnemyInfo[cIndex].m_dwCID==dwAttackedCID)
			{				
				lpSaveEnemyInfo[cIndex].m_dwCID		= dwAttackedCID;
				lpSaveEnemyInfo[cIndex].m_dwTick	= dwTick;
				return true;
			}
		}

		for(unsigned char cIndex = 0; cIndex < CSessionData::SaveEnemy::MAX_SAVING_ENEMY; cIndex++)
		{
			if(!lpSaveEnemyInfo[cIndex].m_dwCID)
			{				
				lpSaveEnemyInfo[cIndex].m_dwCID		= dwAttackedCID;
				lpSaveEnemyInfo[cIndex].m_dwTick	= dwTick;
				return true;
			}
		}

		unsigned char cDeleteIndex	= 0xff;
		unsigned long dwDeleteTick	= dwTick;

		for(unsigned char cIndex = 0; cIndex < CSessionData::SaveEnemy::MAX_SAVING_ENEMY; cIndex++)
		{
			if(lpSaveEnemyInfo[cIndex].m_dwCID)
			{		
				if(lpSaveEnemyInfo[cIndex].m_dwTick<=dwDeleteTick)
				{
					dwDeleteTick	= lpSaveEnemyInfo[cIndex].m_dwTick;
					cDeleteIndex	= cIndex;
				}				
			}
		}
		
		if(cDeleteIndex!=0xff)
		{
			lpSaveEnemyInfo[cDeleteIndex].m_dwCID	= dwAttackedCID;
			lpSaveEnemyInfo[cDeleteIndex].m_dwTick	= dwTick;
		}

		return true;
	}

	return true;
}

bool UpdateKeyInfo(CSendStream& SendStream, PktBase* lpPktBase)
{
	PktKeyInfo* lpKeyInfo = static_cast<PktKeyInfo*>(lpPktBase);

	unsigned long dwCID = lpKeyInfo->m_dwCID;
	
#ifdef ENABLE_PACKET_LOG
	DETLOG4(g_PacketLog, "ParseCharUpdate::UpdateKeyInfo : UID:%10u / CID:%10u / SessionID:%10u / ServerID:0x%08X / ",
		0, dwCID, 0, 0);
#endif

	using namespace DataStorage;

	CSessionData* lpSessionData = CSessionDataMgr::GetInstance().GetCharLoadedSession(dwCID);

	if (lpSessionData)
	{
		KeyInfo* pKeyInfo = lpSessionData->GetKeyInfo();
		pKeyInfo->m_cUsed = 1;
		memcpy(pKeyInfo->m_dwKeyInfo, lpKeyInfo->m_dwKeyInfo, sizeof(unsigned long)*PktKeyInfo::MAX_KEY_INFO);

		//DBComponent::GameDB::UpdateKeyInfo(CDBSingleObject::GetInstance(), pKeyInfo, lpSessionData->GetUID());

	}

	return true;
}

bool UseCashItem(CSendStream& SendStream, PktBase* lpPktBase)
{
	PktUI* lpPktUI = static_cast<PktUI*>(lpPktBase);

	unsigned long dwCID = lpPktUI->m_dwRecver;
	

#ifdef ENABLE_PACKET_LOG
	DETLOG4(g_PacketLog, "ParseCharUpdate::UseCashItem : UID:%10u / CID:%10u / SessionID:%10u / ServerID:0x%08X / ",
		0, dwCID, 0, 0);
#endif

	using namespace DataStorage;

	CSessionData* lpSessionData = CSessionDataMgr::GetInstance().GetCharLoadedSession(dwCID);

	unsigned short usError = 0;

	/// 스토어드프로시져 추가함 (2009.05.09 일 무슨이유인지 스토어드프로시져가 없었음)
	// 여기서 DB에 쿼리를 날려준다. 1시간 결재금액
	if(DBComponent::GameDB::AddPremiumService(CDBSingleObject::GetInstance(), lpSessionData->GetUID(), 1, lpPktUI->m_cRemainItemNum))
	{
		// 결과 보내기
		SendPacket::CharUseCashItem(SendStream, lpPktUI->m_dwSender, lpPktUI->m_dwRecver,
			lpPktUI->m_itemPos, lpPktUI->m_cRemainItemNum, 0);
		return true;
	}
	// 결과 보내기
    SendPacket::CharUseCashItem(SendStream, lpPktUI->m_dwSender, lpPktUI->m_dwRecver,
		lpPktUI->m_itemPos, lpPktUI->m_cRemainItemNum, PktUI::USE_FAILED);

	return false ;
}

bool ExtraEvent(CSendStream& SendStream, PktBase* lpPktBase)
{
	// edith 2009.09.05 신규 이벤트 제작부분
	PktExtraEvent* lpPktEE = static_cast<PktExtraEvent*>(lpPktBase);

	unsigned long dwUID = lpPktEE->m_dwUserID;
	unsigned long dwCID = lpPktEE->m_dwCharID;
	unsigned long dwEventID = lpPktEE->m_dwEventID;

	unsigned int dwEVENT = LOWORD(dwEventID);
	unsigned int dwVALUE = HIWORD(dwEventID);

	using namespace DataStorage;

	CSessionData* lpSessionData = CSessionDataMgr::GetInstance().GetCharLoadedSession(dwCID);

	unsigned short usError = 0;
	unsigned int dwEventFlag = 0;

	if(DBComponent::GameDB::UpdateExtraEvent(CDBSingleObject::GetInstance(), lpSessionData->GetUID(), dwEventID, dwEVENT, dwVALUE, &dwEventFlag))
	{
		if(dwEventFlag != 1)
		{
			usError = PktExtraEvent::EVENT_CLEAR;
		}
		else if(dwEVENT == 1)
		{
			int day = 0;
			if(dwVALUE == 30)
				day = 7;
			else if(dwVALUE == 60)
				day = 15;
			else if(dwVALUE == 90)
				day = 30;

			// 골박 day일 보상
			if(!DBComponent::GameDB::AddPremiumService(CDBSingleObject::GetInstance(), dwUID, 1, day))
				usError = PktExtraEvent::FAIL_REWARD;
			else
			{
				lpPktEE->m_dwValue1 = dwEVENT;
				lpPktEE->m_dwValue2 = dwVALUE;
			}
		}
		else 
			usError = PktExtraEvent::EVENT_NONE;
	}
	else 
		usError = PktExtraEvent::FAIL_EVENT;

	SendPacket::CharExtraEvent(SendStream, lpPktEE->m_dwUserID, lpPktEE->m_dwCharID, lpPktEE->m_dwEventID, lpPktEE->m_dwValue1, lpPktEE->m_dwValue2, usError);
	return false;
}

}



namespace ParseCharAdmin
{

bool AdminCommandLog(CSendStream& SendStream, PktBase* lpPktBase)
{
	PktAdminCommandLog* lpAdminCommandLog = static_cast<PktAdminCommandLog*>(lpPktBase);

	char szCommand[PktAdminCommandLog::MAX_COMMAND_SIZE] = {0, };

	unsigned long dwAdminCID	= lpAdminCommandLog->m_dwAdminCID;
	unsigned long dwCID			= lpAdminCommandLog->m_dwCID;

	memcpy(szCommand, lpAdminCommandLog->m_szCommand, PktAdminCommandLog::MAX_COMMAND_SIZE);

	using namespace DataStorage;

	CSessionData* lpSessionData = CSessionDataMgr::GetInstance().GetCharLoadedSession(dwAdminCID);

	if(lpSessionData)
	{		
		DBComponent::GameDB::InsertAdminCommandLog(CDBSingleObject::GetInstance(), lpSessionData->GetOldServerGroupID(), dwAdminCID, dwCID, szCommand);
	
	}

	return true;	
}

}
}