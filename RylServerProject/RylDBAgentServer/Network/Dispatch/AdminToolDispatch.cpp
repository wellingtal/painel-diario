#include "stdafx.h"
#include "AdminToolDispatch.h"

#include <Log/ServerLog.h>
#include <Network/Address/INET_Addr.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>
#include <Network/Packet/PacketStruct/ServerPacket.h>
#include <Network/Packet/PacketStruct/CharAdminPacket.h>
#include <Network/Packet/PacketStruct/GameEventPacket.h>
#include <Network/Packet/PacketStruct/CharLoginOutPacketStruct.h>

#include <Network/SendPacket/SendAdminTool.h>
#include <Network/SendPacket/SendServerInfo.h>

#include <Network/Dispatch/GameDispatch.h>
#include <Network/Dispatch/GameClient/SendCharAdmin.h>

#include <DataStorage/AdminDataMgr.h>
#include <DataStorage/SessionData.h>
#include <DataStorage/SessionDataMgr.h>
#include <DataStorage/StoreData.h>
#include <DataStorage/StoreDataMgr.h>
#include <DataStorage/CharacterData.h>
#include <DataStorage/CharacterDataMgr.h>

#include <DB/DBComponent.h>
#include <DB/GameDBComponent.h>
#include <DB/GuildDBComponent.h>
#include <GameEvent/GameEventDBMgr.h>
#include <Utility/Setup/ServerSetup.h>
#include <Creature/Character/CharacterClass.h>

#include <Network/Packet/PacketStruct/GuildPacket.h>

#include <Community/Guild/GuildDB.h>
#include <Community/Guild/GuildDBMgr.h>

namespace DBAgent
{

// forward decl.
bool GetUIDCIDFromName(const char* szName, unsigned long& dwUID_Out, 
                       unsigned long& dwCID_Out, unsigned char& cOldServerGroupID);


CMultiDispatch& CAdminToolDispatch::GetDispatchTable()
{
    static CMultiDispatch gameDispatch;
    return gameDispatch;
}


CAdminToolDispatch::CAdminToolDispatch(CSession& Session)
    :   CRylServerDispatch(Session, MAX_PACKET_DISPATCH_PER_PULSE),
        m_dwServerID(0)
{


}

CAdminToolDispatch::~CAdminToolDispatch()
{


}

void CAdminToolDispatch::Connected()
{
    DETLOG3(g_Log, "SS:0x%08x/DP:0x%08x/IP:%15s/AdminToolServer Connected",
        &GetSession(), this, GetRemoteAddr().get_addr_string());
}

void CAdminToolDispatch::Disconnected()
{
    DETLOG3(g_Log, "SS:0x%08x/DP:0x%08x/IP:%15s/AdminToolServer Disconnected",
        &GetSession(), this, GetRemoteAddr().get_addr_string());

    if(0 != m_dwServerID)
    {
        GetDispatchTable().RemoveDispatch(m_dwServerID);

        if(0LL != m_AdminItemSerialMgr.GetItemSerial())
        {
            // 아이템 시리얼 DB에 저장
            m_AdminItemSerialMgr.SaveItemSerial(
                CDBSingleObject::GetInstance(), m_dwServerID);
        }
    }
}


bool CAdminToolDispatch::DispatchPacket(PktBase* lpPktBase)
{
    bool bResult = false;
    PktBase::CMDType cCmd = lpPktBase->GetCmd();

    switch(cCmd)
    {
    case CmdSysServerLogin:     bResult = ParseServerLogin(static_cast<PktSL*>(lpPktBase));                     break;
    case CmdSysServerLogout:    bResult = ParseServerLogout(lpPktBase);                                         break;
    case CmdUserKill:           bResult = ParseUserKill(static_cast<PktUK*>(lpPktBase));                        break;
    case CmdAdminToolGetData:   bResult = ParseGetData(static_cast<PktAdminToolGetData*>(lpPktBase));           break;
    case CmdAdminToolSetData:   bResult = ParseSetData(static_cast<PktAdminToolSetData*>(lpPktBase));           break;
    case CmdItemQtyCheck:       bResult = ParseItemQtyCheck(static_cast<PktItemQtyControl*>(lpPktBase));        break;
	case CmdGuildPosition:		bResult = ParseGuildPosition(static_cast<PktGuildPosition*>(lpPktBase));		break;

    default:

        ERRLOG4(g_Log, "SS:0x%08x/DP:0x%08x/IP:%15s/Cmd:0x%02X/ 운영툴 서버 패킷 처리 실패 : 없는 커맨드입니다",
            &GetSession(), this, GetRemoteAddr().get_addr_string(), cCmd);

        bResult = true;
        break;
    }

    if(!bResult)
    {
        ERRLOG4(g_Log, "SS:0x%08x/DP:0x%08x/IP:%15s/Cmd:0x%02X/ 운영툴 서버 패킷 처리 실패 : 처리를 실패했습니다",
            &GetSession(), this, GetRemoteAddr().get_addr_string(), cCmd);
    }

    return true;
}

bool CAdminToolDispatch::ParseServerLogin(PktSL* lpPktSL)
{
    m_dwServerID = lpPktSL->m_dwServerID;

    INFLOG4(g_Log, "SS:0x%08x/DP:0x%08x/IP:%15s/ServerID:0x%08X/ 운영툴 서버 연결 시도 : 패킷 받음",
        &GetSession(), this, GetRemoteAddr().get_addr_string(), m_dwServerID);

    GET_MULTI_DISPATCH(lpAdminToolDispatch, m_dwServerID, 
        CAdminToolDispatch, GetDispatchTable());

    if(0 != lpAdminToolDispatch)
    {
        ERRLOG4(g_Log, "SS:0x%08x/DP:0x%08x/IP:%15s/ServerID:0x%08X/ 운영툴 서버 연결 실패 : 이미 연결이 있습니다",
            &GetSession(), this, GetRemoteAddr().get_addr_string(), m_dwServerID);

        CloseSession();
    }
    else
    {
        // Dispatch 세팅.
        GetDispatchTable().SetDispatch(m_dwServerID, this);

        // 아이템 시리얼 로드.
        if(!m_AdminItemSerialMgr.LoadItemSerial(CDBSingleObject::GetInstance(), m_dwServerID))
        {
            ERRLOG4(g_Log, "SS:0x%08x/DP:0x%08x/IP:%15s/ServerID:0x%08X/ 운영툴 서버 연결 실패 : 아이템 시리얼 로드 실패. 접속을 끊습니다",
                &GetSession(), this, GetRemoteAddr().get_addr_string(), m_dwServerID);

            CloseSession();
            return false;
        }

        return SendPacket::ServerLoginAck(m_SendStream, 
            m_dwServerID, m_AdminItemSerialMgr.GetItemSerial());
    }

    return true;
}

bool CAdminToolDispatch::ParseServerLogout(PktBase* lpPktBase)
{
    // 받은 패킷 그대로 돌려 준다.

    char* lpBuffer = m_SendStream.GetBuffer(sizeof(PktBase));
    if(0 != lpBuffer)
    {
        return m_SendStream.WrapHeader(sizeof(PktBase), CmdSysServerLogout, 0, 0);
    }

    return false;
}

bool CAdminToolDispatch::ParseUserKill(PktUK* lpPktUK)
{
    using namespace DataStorage;

    // 접속 DB를 전부 뒤져서, 계정과 IP가 일치하는 넘한테 뜨거운 맛을 보여 준다.
    CSessionData* lpSessionData = 
		CSessionDataMgr::GetInstance().GetOpenSession(lpPktUK->m_dwUserID);

    if(0 == lpSessionData)
    {
        ERRLOG1(g_Log, "UID:%10u / 운영툴 서버 유저 접속 끊기 실패 : 유저가 없습니다", lpPktUK->m_dwUserID);
    }
    else 
	{
		SERVER_ID serverID;
		serverID.dwID = lpSessionData->GetServerID();
		CSessionData::SessionState eSessionState = lpSessionData->GetSessionState();

		if ((serverID.GetType() == CServerSetup::AuthServer && CSessionData::SE_USER_ENABLED != eSessionState) ||
			(serverID.GetType() == CServerSetup::GameServer && CSessionData::SE_CHAR_ENABLED != eSessionState))
		{
			ERRLOG3(g_Log, "UID:%10u / ServerID:0x%08X / ST:%s / 운영툴 서버 유저 접속 끊기 실패 : 유저나 캐릭터가 비활성화되어 있습니다", 
				lpPktUK->m_dwUserID, serverID.dwID, g_szSessionStateString[lpSessionData->GetSessionState()]);
		}
		else if(!SendPacket::UserKill(*lpSessionData))
		{
			ERRLOG3(g_Log, "UID:%10u / ServerID:0x%08X / ST:%s / 운영툴 서버 유저 접속 끊기 실패 : 접속 끊기 패킷 보내기 실패", 
				lpPktUK->m_dwUserID, serverID.dwID, g_szSessionStateString[lpSessionData->GetSessionState()]);
		}
	}

    return true;
}


bool CAdminToolDispatch::ParseGetData(PktBase* lpPktBase)
{
    // 데이터 얻어 오기. 패킷 오면 캐릭터 정보 및 창고 정보를 준다.
    PktAdminToolGetData* lpPktAdminToolGetData = static_cast<PktAdminToolGetData*>(lpPktBase);

    switch(lpPktAdminToolGetData->m_cType)
    {
    case PktAdminToolGetData::GET_CHAR_DATA:

        return ParseAdminToolGetData(lpPktAdminToolGetData);

    case PktAdminToolGetData::CHANGE_CHAR_NAME:

        return ParseAdminToolChangeName(lpPktAdminToolGetData);

    default:

        ERRLOG2(g_Log, "IP:%15s / Cmd:%d / 알 수 없는 운영툴 데이터 얻기 커맨드입니다.", 
            GetRemoteAddr().get_addr_string(), lpPktAdminToolGetData->m_cType);

        return true;
    }

    return false;
}


bool CAdminToolDispatch::ParseSetData(PktBase* lpPktBase)
{
    // 데이터 기록하기. 패킷 오면 캐릭터 관련 데이터를 쓴다.
    // CharLogin때 보내주는 데이터, 창고 데이터 1, 창고 데이터 2, 창고 정보

    // 데이터 얻어 오기. 패킷 오면 캐릭터 정보 및 창고 정보를 준다.
    PktAdminToolSetData* lpPktAdminToolSetData = static_cast<PktAdminToolSetData*>(lpPktBase);

    switch(lpPktAdminToolSetData->m_cType)
    {
    case PktAdminToolSetData::CHAR_BASIC_DATA:
    case PktAdminToolSetData::CHAR_EXTRA_DATA:
    case PktAdminToolSetData::CHAR_FRIEND_DATA:
    case PktAdminToolSetData::CHAR_BAN_DATA:
    case PktAdminToolSetData::STORE_12_DATA:
    case PktAdminToolSetData::STORE_34_DATA:
    case PktAdminToolSetData::OPEN_UPDATE_DATA:
    case PktAdminToolSetData::FINISH_UPDATE_DATA:

        return ParseAdminToolSetData(lpPktAdminToolSetData);

    case PktAdminToolSetData::RELOAD_ADMINLIST:

        return DataStorage::CAdminDataMgr::GetInstance().LoadAdminList(CDBSingleObject::GetInstance());

    default:

        ERRLOG2(g_Log, "IP:%15s / Cmd:%d / 알 수 없는 운영자 데이터 셋 패킷 커맨드입니다", 
            GetRemoteAddr().get_addr_string(), lpPktAdminToolSetData->m_cType);

        return true;
    }

    return false;
}

bool CAdminToolDispatch::ParseAdminToolGetData(PktAdminToolGetData* lpPktAdminToolGetData)
{
    // DBRequestKey.
    unsigned long dwRequestKey = lpPktAdminToolGetData->m_dwRequestKey;
    unsigned long dwUID = lpPktAdminToolGetData->m_dwUID;
    unsigned long dwCID = lpPktAdminToolGetData->m_dwCID;
	unsigned char cOldServerGroupID = 0;

	if (UnifiedConst::Part2Selectable == CServerSetup::GetInstance().GetAgentServerType())
	{
		cOldServerGroupID = lpPktAdminToolGetData->m_cOldServerGroupID;
	}
	else
	{
		cOldServerGroupID = static_cast<unsigned char>(
			CServerSetup::GetInstance().GetAgentServerType());
	}

    PktAdminToolGetData::GetDataType eDataType = 
        static_cast<PktAdminToolGetData::GetDataType>(lpPktAdminToolGetData->m_cType);

    unsigned short wError = 0;

    if(eDataType != PktAdminToolGetData::GET_CHAR_DATA)
    {
        // 잘못된 패킷임
        wError = PktAdminToolGetData::INVALID_REQUEST;
    }
    else
    {        
        // UID, CID가 없으면 캐릭터 이름을 가지고 UID와 CID를 얻어 온다.
        if(0 == dwUID || 0 == dwCID || 0 == cOldServerGroupID)
        {
            if(!GetUIDCIDFromName(lpPktAdminToolGetData->m_szName, 
                dwUID, dwCID, cOldServerGroupID))
            {
                // DB Query 실패.
                wError = PktAdminToolGetData::GET_UIDCID_QUERY_FAILED;				
            }

            if(0 == dwUID || 0 == dwCID || 0 == cOldServerGroupID)
            {
                // 얻어온 데이터가 이상함.
                wError = PktAdminToolGetData::INVALID_UID_CID_DATA;				
            }
        }

        DETLOG4(g_Log, "UID:%10u / ServerGroupID:%d / CID:%10u / IP:%15s / 운영툴로부터 데이터 요청 받음", 
            dwUID, cOldServerGroupID, dwCID, GetRemoteAddr().get_addr_string());

        if(0 == wError)
        {
            using namespace DataStorage;

            CStoreData*     lpStoreData     = 0;
            CCharacterData* lpCharacterData = 0;

            CSessionData* lpSessionData = CSessionDataMgr::GetInstance().GetOpenSession(dwUID);

            if(0 != lpSessionData)
            {
                // 이미 로드된 세션에서 데이터 가져옴.
                if(dwUID == lpSessionData->GetUID())
                {
                    lpStoreData = lpSessionData->GetStoreData();
                }

                if(dwCID == lpSessionData->GetCID())
                {
                    lpCharacterData = lpSessionData->GetCharacterData();
                }
            }

            // 캐쉬에서 데이터 로드. 캐쉬에 데이터가 없으면, DB에서 로드해서 캐쉬에 추가.
            if(0 == lpStoreData)
            {
                lpStoreData = CStoreDataMgr::GetInstance().GetLogoutData(
                    UnifiedStoreKey(dwUID, cOldServerGroupID));
            }

            if(0 == lpCharacterData)
            {
                lpCharacterData = CCharacterDataMgr::GetInstance().GetLogoutData(dwCID);
            }

                
            if(0 == lpStoreData)
            {
                wError = PktAdminToolGetData::GET_STORE_QUERY_FAILED;
            }
            else if(0 == lpCharacterData)
            {
                wError = PktAdminToolGetData::GET_CHAR_DATA_QUERY_FAILED;
            }
            else
            {
                SendPacket::TotalDataToAdminTool(GetSendStream(), dwRequestKey, 
                    GetRemoteAddr().get_addr_in().sin_addr, *lpStoreData, *lpCharacterData);
            }
        }
    }

    // 최종 전송
    return SendPacket::AdminToolGetDataAck(GetSendStream(),
        PktAdminToolGetData::FINISH_GET_DATA, dwRequestKey, dwUID, dwCID, 0, 0, wError, cOldServerGroupID);
}

bool CAdminToolDispatch::ParseAdminToolSetData(PktAdminToolSetData* lpPktAdminToolSetData)
{
    SERVER_ID serverID;
    serverID.dwID = m_dwServerID;

    unsigned long dwUID         = lpPktAdminToolSetData->m_dwUID;
    unsigned long dwCID         = lpPktAdminToolSetData->m_dwCID;
    unsigned long dwRequestKey  = lpPktAdminToolSetData->m_dwRequestKey;
    unsigned long dwDataSize	= lpPktAdminToolSetData->GetLen() - sizeof(PktAdminToolSetData);
    unsigned char cType			= lpPktAdminToolSetData->m_cType;
	unsigned char cOldServerGroupID = 0;

	if (UnifiedConst::Part2Selectable == CServerSetup::GetInstance().GetAgentServerType())
	{
		cOldServerGroupID = lpPktAdminToolSetData->m_cOldServerGroupID;
	}
	else
	{
		cOldServerGroupID = static_cast<unsigned char>(
			CServerSetup::GetInstance().GetAgentServerType());
	}

    unsigned short	wError	= 0;

    if(0 == dwUID || 0 == dwCID)
    {
        wError = PktAdminToolSetData::INVALID_UID_CID_ERROR;
    }
    else
    {
        DETLOG3(g_Log, "UID:%10u / CID:%10u / IP:%15s / 운영자가 운영툴로 데이터 세팅 시도",
            dwUID, dwCID, GetRemoteAddr().get_addr_string());

        using namespace DataStorage;

        CStoreData*     lpStoreData     = 0;
        CCharacterData* lpCharacterData = 0;

        CSessionData*   lpSessionData = CSessionDataMgr::GetInstance().GetOpenSession(dwUID);

        if(0 != lpSessionData)
        {
			if(CSessionData::SE_USER_ENABLED == lpSessionData->GetSessionState() || 
				CSessionData::SE_CHAR_ENABLED == lpSessionData->GetSessionState())
			{
				// 활성화된 캐릭터 편집 에러
				wError = PktAdminToolSetData::LOGIN_ANOTHER_GAMESERVER;
			}
			else
			{
                // 이미 로드된 세션에서 데이터 가져옴.
				if(dwUID == lpSessionData->GetUID())
				{
					lpStoreData = lpSessionData->GetStoreData();
				}

				if(dwCID == lpSessionData->GetCID())
				{
					lpCharacterData = lpSessionData->GetCharacterData();
				}
			}
        }

        if(0 == wError)
        {
            // 캐쉬에서 데이터 로드. 캐쉬에 데이터가 없으면, DB에서 로드해서 캐쉬에 추가.
            if(0 == lpStoreData)
            {
                lpStoreData = CStoreDataMgr::GetInstance().GetLogoutData(
                    UnifiedStoreKey(dwUID, cOldServerGroupID));
            }

            if(0 == lpCharacterData)
            {
                lpCharacterData = CCharacterDataMgr::GetInstance().GetLogoutData(dwCID);
            }

            if(0 != lpStoreData && 0 != lpCharacterData)
            {
                switch(lpPktAdminToolSetData->m_cType)
                {
                case PktAdminToolSetData::OPEN_UPDATE_DATA:
                    // 세션을 열고 DB에 업데이트하려 시도한다.      : 이제는 세션 열 필요 없다. 그냥 한다.
                    break;

                case PktAdminToolSetData::FINISH_UPDATE_DATA:	
                    // 데이터를 DB에 업데이트하고 세션을 닫는다.    : 이제는 세션 닫을 필요 없다. 그냥 한다.
                    break;

                case PktAdminToolSetData::CHAR_BASIC_DATA:		// 캐릭터 정보. 다음 정보들이 들어감. 

                    // 세션이 운영툴에서 연 세션인지 확인한다. 운영툴에서 연 세션이면, 데이터를 세팅한다.
                    {
                        unsigned short* lpSizeArray = reinterpret_cast<unsigned short*>(lpPktAdminToolSetData + 1);
                        char* lpData = reinterpret_cast<char*>(lpSizeArray + DBUpdateData::MAX_UPDATE_DB);

                        if(!lpCharacterData->SerializeIn(lpData, lpSizeArray, 
                            dwDataSize - sizeof(unsigned short) * DBUpdateData::MAX_UPDATE_DB, DBUpdateData::MAX_UPDATE_DB))
                        {
                            ERRLOG3(g_Log, "UID:%10u / CID:%10u / IP:%15s / 운영툴 데이터 세팅 실패 : CHAR_BASIC_DATA",
                                dwUID, dwCID, GetRemoteAddr().get_addr_string());
                            
                            wError = PktAdminToolSetData::SETTING_DATA_FAILED;
                        }
                        else
                        {
                            INFLOG3(g_Log, "UID:%10u / CID:%10u / IP:%15s / 운영툴 데이터 세팅 성공 : CHAR_BASIC_DATA",
                                dwUID, dwCID, GetRemoteAddr().get_addr_string());
                        }
                    }
                    break;

                case PktAdminToolSetData::CHAR_EXTRA_DATA:		// 캐릭터 추가 정보들. 다음 정보들이 들어감.
                    
                    {
                        unsigned long dwExpectDataSize = sizeof(CHAR_INFOEX) + sizeof(QUEST) + 
                            sizeof(HISTORY) + sizeof(CONFIG) + sizeof(STORE_INFO);

                        if(dwDataSize == dwExpectDataSize)
                        {
                            char* lpCharDataExPos = reinterpret_cast<char*>(lpPktAdminToolSetData + 1);

                            // CHAR_INFOEX	: 캐릭터 추가 정보
                            lpCharacterData->SetInfoEx(*reinterpret_cast<CHAR_INFOEX*>(lpCharDataExPos));
                            lpCharDataExPos += sizeof(CHAR_INFOEX);

                            // QUEST		: 퀘스트
                            lpCharacterData->SetQuest(*reinterpret_cast<QUEST*>(lpCharDataExPos));
                            lpCharDataExPos += sizeof(QUEST);		

                            // HISTORY		: 히스토리
                            lpCharacterData->SetHistory(*reinterpret_cast<HISTORY*>(lpCharDataExPos));
                            lpCharDataExPos += sizeof(HISTORY);		

                            // CONFIG		: 설정
                            lpCharacterData->SetConfig(*reinterpret_cast<CONFIG*>(lpCharDataExPos));
                            lpCharDataExPos += sizeof(CONFIG);		

                            // STORE_INFO	: 창고 정보
                            lpStoreData->SetStoreInfo(*reinterpret_cast<STORE_INFO*>(lpCharDataExPos));
                            lpCharDataExPos += sizeof(STORE_INFO);	
                        }
                        else
                        {
                            wError = PktAdminToolSetData::SETTING_DATA_FAILED;
                        }
                    }
                    break;

                case PktAdminToolSetData::CHAR_FRIEND_DATA:		// 캐릭터 친구 정보

                    if(!lpCharacterData->FriendSerializeIn(
                        reinterpret_cast<char*>(lpPktAdminToolSetData + 1), dwDataSize))
                    {
                        wError = PktAdminToolSetData::SETTING_DATA_FAILED;
                    }
                    break;

                case PktAdminToolSetData::CHAR_BAN_DATA:		// 캐릭터 거부 정보

                    if(!lpCharacterData->BanSerializeIn(
                        reinterpret_cast<char*>(lpPktAdminToolSetData + 1), dwDataSize))
                    {
                        wError = PktAdminToolSetData::SETTING_DATA_FAILED;
                    }
                    break;

                case PktAdminToolSetData::STORE_12_DATA:		// 창고 1, 2탭 데이터

                    if(dwDataSize == sizeof(STORE))
                    {
                        const STORE& store1 = *reinterpret_cast<STORE*>(lpPktAdminToolSetData + 1);
                        lpStoreData->SetStore1(store1.Data, store1.dwSize);
                    }
                    else
                    {
                        wError = PktAdminToolSetData::SETTING_DATA_FAILED;
                    }
                    break;

                case PktAdminToolSetData::STORE_34_DATA:		// 창고 3, 4탭 데이터

                    if(dwDataSize == sizeof(STORE))
                    {
                        const STORE& store2 = *reinterpret_cast<STORE*>(lpPktAdminToolSetData + 1);
                        lpStoreData->SetStore2(store2.Data, store2.dwSize);
                    }
                    else
                    {
                        wError = PktAdminToolSetData::SETTING_DATA_FAILED;
                    }
                    break;
                }
            }
        }
    }

    // ItemSerial 업데이트
    if(m_AdminItemSerialMgr.SetItemSerial(lpPktAdminToolSetData->m_dwSerial))
    {
        m_AdminItemSerialMgr.SaveItemSerial(CDBSingleObject::GetInstance(), m_dwServerID);
    }

    return SendPacket::AdminToolSetDataAck(
        GetSendStream(), dwRequestKey, dwUID, dwCID, cType, wError);
}


bool CAdminToolDispatch::ParseGuildPosition(PktGuildPosition* lpPktGuildPosition)
{	
	using namespace DataStorage;

	unsigned short wError = PktBase::NO_SERVER_ERR;

	unsigned long	dwGID		= lpPktGuildPosition->m_dwGID;
	unsigned long	dwCID		= lpPktGuildPosition->m_dwCID;	    
	unsigned char	cPosition	= lpPktGuildPosition->m_cPosition;	
	unsigned char	cType		= lpPktGuildPosition->m_cType;

	using namespace Guild;

	CGuildDB* lpGuild = static_cast<CGuildDB*>(CGuildDBMgr::GetInstance().GetGuild(dwGID));

	char* lpBuffer = GetSendStream().GetBuffer(sizeof(PktGuildPosition));

	PktGuildPosition* pktSendPacket = NULL;

	if(lpBuffer)
	{
		pktSendPacket = reinterpret_cast<PktGuildPosition*>(lpBuffer);

		pktSendPacket->m_cPosition	= lpPktGuildPosition->m_cPosition;
		pktSendPacket->m_dwCID		= lpPktGuildPosition->m_dwCID;
		pktSendPacket->m_dwGID		= lpPktGuildPosition->m_dwGID;
		pktSendPacket->m_cType		= lpPktGuildPosition->m_cType;
	}

	if(!lpGuild)
	{
		wError = PktGuildPosition::FAIL_GUILD;
	}
	else
	{
		CSessionData* lpSessionData = CSessionDataMgr::GetInstance().GetCharLoadedSession(dwCID);

		if(lpSessionData)
		{
			wError = PktGuildPosition::FAIL_CONNECT_CHAR;
		}
		else
		{	
			if(cType==PktGuildPosition::TYPE_POSITION)
			{
				lpGuild->SetTitle(dwCID, cPosition);

				SYSTEMTIME systemTime;
				::memset(&systemTime, 0, sizeof(SYSTEMTIME));						
				GetLocalTime(&systemTime);

				if(!DBComponent::GuildDB::UpdateMemberTitle(CDBSingleObject::GetInstance(), dwCID, cPosition, systemTime))
				{
					wError = PktGuildPosition::FAIL_DB_UPDATE;
				}
			}
			else if(cType==PktGuildPosition::TYPE_MEMBERDELETE)
			{				
				if(!lpGuild->LeaveMember(dwCID))
				{
					wError = PktGuildPosition::FAIL_MEMBER_DELETE;
				}
				else
				{
					using namespace DBAgent::DataStorage;
					
					DBComponent::GameDB::UpdateGuildWarFlag(CDBSingleObject::GetInstance(), dwCID, Creature::WAR_OFF);
				}				
			}
		}
	}

	return GetSendStream().WrapHeader(sizeof(PktGuildPosition), CmdGuildPosition, 0, wError);
}

bool CAdminToolDispatch::ParseItemQtyCheck(PktItemQtyControl* lpPktItemQtyControl)
{
    tm* lpTm = localtime(&lpPktItemQtyControl->m_tStartTime);
    if(0 == lpTm)
    {
        ERRLOG2(g_Log, "IP:%15s / StartTime:%d / 아이템 수량 제어 실패 : 시간이 맞지 않습니다", 
            GetRemoteAddr().get_addr_string(), lpPktItemQtyControl->m_tStartTime);
    }
    else 
    {
        tm tStartTime = *lpTm;

        lpTm = localtime(&lpPktItemQtyControl->m_tEndTime);
        if(0 == lpTm)
        {
            ERRLOG2(g_Log, "IP:%15s / EndTime:%d / 아이템 수량 제어 실패 : 시간이 맞지 않습니다", 
                GetRemoteAddr().get_addr_string(), lpPktItemQtyControl->m_tEndTime);
        }
        else
        {
            tm tEndTime = *lpTm;

            if(lpPktItemQtyControl->m_cGroup != CServerSetup::GetInstance().GetServerGroup())
            {
                ERRLOG3(g_Log, "IP:%15s / CurrentServerGroup:%d / ReceivedServerGroup:%d / 아이템 수량 제어 실패 : 서버 그룹이 맞지 않습니다",
                    GetRemoteAddr().get_addr_string(), CServerSetup::GetInstance().GetServerGroup(), lpPktItemQtyControl->m_cGroup);    
            }
            else
            {
                bool bResult = false;

                switch(lpPktItemQtyControl->m_cType)
                {
                case PktItemQtyControl::ADD:

                    bResult = CGameEventDBMgr::GetInstance().InsertDropItem(
                        static_cast<unsigned short>(lpPktItemQtyControl->m_dwItemTypeID),
                        static_cast<unsigned short>(lpPktItemQtyControl->m_dwItemQty),
                        tStartTime, tEndTime);

                    DETLOG5(g_Log, "아이템 수량 제어 추가 %s : ID:%u/Qty:%u/StartTime:%d/EndTime:%d",
                        bResult ? "성공" : "실패", 
                        lpPktItemQtyControl->m_dwItemTypeID, lpPktItemQtyControl->m_dwItemQty,
                        lpPktItemQtyControl->m_tStartTime, lpPktItemQtyControl->m_tEndTime);

                    SendPacket::ItemQtyCheck(lpPktItemQtyControl->m_dwItemTypeID, 
                        lpPktItemQtyControl->m_dwItemQty, 0,
                        lpPktItemQtyControl->m_tStartTime, 
                        lpPktItemQtyControl->m_tEndTime, 
                        PktItemQtyControl::ADD, bResult ? 0 : 1);
                    break;

                case PktItemQtyControl::DEL:

                    bResult = CGameEventDBMgr::GetInstance().EraseDropItem(
                        static_cast<unsigned short>(lpPktItemQtyControl->m_dwItemTypeID));

                    DETLOG5(g_Log, "아이템 수량 제어 삭제 %s : ID:%u/Qty:%u/StartTime:%d/EndTime:%d",
                        bResult ? "성공" : "실패", 
                        lpPktItemQtyControl->m_dwItemTypeID, lpPktItemQtyControl->m_dwItemQty,
                        lpPktItemQtyControl->m_tStartTime, lpPktItemQtyControl->m_tEndTime);

                    SendPacket::ItemQtyCheck(lpPktItemQtyControl->m_dwItemTypeID, 
                        lpPktItemQtyControl->m_dwItemQty, 0,
                        lpPktItemQtyControl->m_tStartTime, lpPktItemQtyControl->m_tEndTime, 
                        PktItemQtyControl::DEL, bResult ? 0 : 1);
                    break;

                case PktItemQtyControl::STATUS_REQUEST:

                    DETLOG0(g_Log, "아이템 수량 제어 상태 요청");
                    CGameEventDBMgr::GetInstance().SendDropItemInfo();
                    break;
                }
            }
        }
    }

    return true;
}


class CSendNameChanged
{
public:

    CSendNameChanged(unsigned long dwUID, unsigned long dwCID, const char* szChangedName,
        unsigned char cNameChangeCount, unsigned short usError)
        :   m_dwUID(dwUID), m_dwCID(dwCID), 
            m_cNameChangeCount(cNameChangeCount),
            m_usError(usError)
    {
        strncpy(m_szChangedName, szChangedName, CHAR_INFOST::MAX_NAME_LEN);
        m_szChangedName[ CHAR_INFOST::MAX_NAME_LEN - 1 ] = 0;
    }

    bool operator () (unsigned long dwServerID, CPacketDispatch& dispatch)
    {
        return GameClientSendPacket::SendCharNameChange(
            static_cast<CRylServerDispatch&>(dispatch).GetSendStream(), 
            m_dwUID, m_dwCID, m_szChangedName, m_cNameChangeCount, NULL, m_usError);
    }

private:

    unsigned long   m_dwUID;
    unsigned long   m_dwCID;
    unsigned char   m_cNameChangeCount;
    unsigned short  m_usError;

    char            m_szChangedName[CHAR_INFOST::MAX_NAME_LEN];
};

bool CAdminToolDispatch::ParseAdminToolChangeName(PktAdminToolGetData* lpPktAdminToolGetData)
{
    unsigned long dwUID         = lpPktAdminToolGetData->m_dwUID;
    unsigned long dwCID         = lpPktAdminToolGetData->m_dwCID;
    unsigned long dwRequestKey  = lpPktAdminToolGetData->m_dwRequestKey;
    const char* szChangeName    = lpPktAdminToolGetData->m_szName;

    unsigned short  usError          = 0;

    using namespace DBAgent::DataStorage;

    CSessionData*   lpSessionData = 
        CSessionDataMgr::GetInstance().GetCharLoadedSession(dwCID);

    CCharacterData* lpCharacterData = 0;

    if (0 == lpSessionData ||
        lpSessionData->GetUID() != dwUID ||
        lpSessionData->GetCID() != dwCID ||
        (0 == (lpCharacterData = lpSessionData->GetCharacterData())) ||
        dwCID != lpCharacterData->GetCID())
    {
        lpCharacterData = CCharacterDataMgr::GetInstance().GetLogoutData(dwCID);
    }

    if (0 != lpCharacterData)
    {
        unsigned long dwResult = 0;

        if (DBComponent::GameDB::ChangeCharName(
            CDBSingleObject::GetInstance(), dwCID, szChangeName, dwResult))
        {   
            if (0 == dwResult)
            {
                // 이름을 바꾼다.
                CHAR_INFOST charInfoST = lpCharacterData->GetInfo();

                strncpy(charInfoST.Name, szChangeName, CHAR_INFOST::MAX_NAME_LEN);
                charInfoST.Name[CHAR_INFOST::MAX_NAME_LEN - 1] = 0;

                lpCharacterData->SetInfo(charInfoST, true);
            }
            else if(1 == dwResult)
            {
                ERRLOG3(g_Log, "UID:%10u / CID:%10u / ChangeName:%s / 운영툴로 캐릭터 이름 바꾸기 실패 : 이름이 이미 있습니다",
                    dwUID, dwCID, szChangeName);

                usError = PktCharNameChange::ERR_ALREADY_USE_NAME;
            }
            else
            {
                ERRLOG4(g_Log, "UID:%10u / CID:%10u / ChangeName:%s / dwResult:%10u / 운영툴로 캐릭터 이름 바꾸기 실패 : 알 수 없는 dwResult값입니다",
                    dwUID, dwCID, szChangeName, dwResult);

                usError = PktCharNameChange::ERR_SERVER;
            }
        }
        else
        {
            ERRLOG3(g_Log, "UID:%10u / CID:%10u / ChangeName:%s / 운영툴로 캐릭터 이름 바꾸기 실패 : DB호출 에러",
                dwUID, dwCID, szChangeName);

            usError = PktCharNameChange::ERR_SERVER;
        }
    }
    else
    {
        ERRLOG3(g_Log, "UID:%10u / CID:%10u / ChangeName:%s / "
            "운영툴로 캐릭터 이름 바꾸기 실패 : 해당하는 캐릭터를 찾을 수 없습니다.", dwUID, dwCID, szChangeName);

        usError = PktCharNameChange::ERR_CANNOT_FIND_CHARACTER;
    }

    if (0 != lpSessionData && 0 != lpCharacterData && 0 == usError)
    {
        // 모든 게임 서버에 전송한다.
        DBAgent::CGameDispatch::GetDispatchTable().Process(
            CSendNameChanged(dwUID, dwCID, szChangeName, 
            lpCharacterData->GetInfoEx().cNameChangeCount, usError));
    }

    // 운영툴로 Ack를 전송한다.
    return SendPacket::AdminToolGetDataAck(GetSendStream(), 
        PktAdminToolGetData::CHANGE_CHAR_NAME, dwRequestKey, dwUID, dwCID, 0, 0, usError);
}


bool GetUIDCIDFromName(const char* szName, unsigned long& dwUID_Out, 
                       unsigned long& dwCID_Out, unsigned char& cOldServerGroupID)
{
#pragma pack(1)

    struct UIDCID
    {
        unsigned long dwUID;
        unsigned long dwCID;
        unsigned char cOldServerGroupID;
    };

#pragma pack()

    UIDCID uidcid;

    uidcid.dwUID = 0;
    uidcid.dwCID = 0;

    const int MAX_BUFFER = 1024;
    char szQuery[MAX_BUFFER] = { 0, };

    int nLength = _snprintf(szQuery, MAX_BUFFER - 1, "dbo.GetUIDCIDFromName '%s'", szName);
    szQuery[MAX_BUFFER - 1] = 0;

    if(0 < nLength)
    {
        szQuery[nLength] = 0;
        if(CDBSingleObject::GetInstance().ExecuteQueryGetData(szQuery, &uidcid))
        {
            dwUID_Out           = uidcid.dwUID;
            dwCID_Out           = uidcid.dwCID;
            cOldServerGroupID   = uidcid.cOldServerGroupID;
            return true;
        }
    }

    ERRLOG2(g_Log, "캐릭터 이름으로 UID / CID 얻어 오는 쿼리에 실패했습니다. %s : %s", 
        CDBSingleObject::GetInstance().GetErrorString(), szQuery);

    return false;
}


}
