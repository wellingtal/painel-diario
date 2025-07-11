#include "stdafx.h"
#include "GameDispatch.h"

#include <Log/ServerLog.h>
#include <Utility/Setup/ServerSetup.h>

#include <Network/Address/INET_Addr.h>
#include <Network/Stream/SendStream.h>
#include <Network/Packet/PacketDispatchTable.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/WrapPacket.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>
#include <Network/Packet/PacketStruct/ServerPacket.h>
#include <Network/Packet/PacketStruct/ServerLogPacket.h>
#include <Network/Packet/PacketStruct/CharAdminPacket.h>

#include <Network/ParsePacket/ParseMoveZone.h>
#include <Network/ParsePacket/ParseCharManage.h>
#include <Network/ParsePacket/ParseCharUpdate.h>
#include <Network/ParsePacket/ParseParty.h>
#include <Network/ParsePacket/ParseCastle.h>
#include <Network/ParsePacket/ParseGuild.h>

#include <Network/SendPacket/SendServerInfo.h>
#include <Network/Dispatch/GameClient/SendCharAdmin.h>

#include <Creature/Character/CharacterClass.h>
#include <Creature/StatueDBMgr.h>

#include <DB/DBComponent.h>
#include <DB/GameDBComponent.h>
#include <DataStorage/SessionData.h>
#include <DataStorage/SessionDataMgr.h>

#include <Castle/CastleDBMgr.h>
#include <GameTime/GameTimeDBMgr.h>
#include <Creature/Siege/SiegeObjectDBMgr.h>
#include <Community/Guild/GuildDBMgr.h>
#include <Ranking/RankingDBMgr.h>
#include <GameEvent/GiveItemMgr.h>

#include <DataStorage/SessionDataMgr.h>
#include <DataStorage/CharacterDataMgr.h>

#include <Map/FieldMap/MineralVeinDBMgr.h>

#include <Ranking/RankingMgr.h>

#include <Community/Party/PartyDBMgr.h>

namespace DBAgent
{

typedef bool (*GameDispatchFunc) (CSendStream& SendStream, PktBase* lpPktBase);


// forward delc.
bool ParseServerLog(CSendStream& SendStream, PktBase* lpPktBase);


class CGameDispatchTable : public CPacketDispatchTable<GameDispatchFunc>
{
public:

    static CGameDispatchTable& GetInstance();

private:

    CGameDispatchTable();
};


CGameDispatchTable& CGameDispatchTable::GetInstance()
{
    static CGameDispatchTable gameDispatchTable;
    return gameDispatchTable;
}

CGameDispatchTable::CGameDispatchTable()
:    CPacketDispatchTable<GameDispatchFunc>(UCHAR_MAX)
{
    // 데이터 업데이트 관련
    AddDispatch(CmdDepositUpdate,           ParseCharUpdate::UpdateDepositDB);
    AddDispatch(CmdDeposit,                 ParseCharUpdate::UpdateDeposit);
    AddDispatch(CmdFriendDB,                ParseCharUpdate::UpdateFriendDB);
    AddDispatch(CmdQuestDB,                 ParseCharUpdate::UpdateQuestDB);
    AddDispatch(CmdConfigInfoDB,            ParseCharUpdate::UpdateConfigInfoDB);

    // 존이동 관련
    AddDispatch(CmdAgentZone,               ParseMoveZone::Parse);

    // 파티 관련
    AddDispatch(CmdAgentParty,              ParseParty::Parse);

    // 배틀그라운드 관련
    AddDispatch(CmdBGServerCharSlot,        ParseCharManage::BGServerCharSlot);

    // 로그 관련
    AddDispatch(CmdServerLog,               ParseServerLog);
    
    // 길드 관련
    AddDispatch(CmdCreateGuild,             ParseGuild::CreateGuild);
    AddDispatch(CmdGuildCmd,                ParseGuild::GuildCmd);
    AddDispatch(CmdGuildMark,               ParseGuild::GuildMark);
    AddDispatch(CmdGuildLevel,              ParseGuild::GuildLevel);
	AddDispatch(CmdGuildRelation,           ParseGuild::GuildRelation);
    AddDispatch(CmdGuildInclination,	    ParseGuild::GuildInclination);
    AddDispatch(CmdGuildRight,			    ParseGuild::SetGuildRight);
    AddDispatch(CmdGuildSafe,				ParseGuild::GuildSafe);
    AddDispatch(CmdGuildMemberInfoUpdate,   ParseGuild::GuildMemberInfoUpdate);

    // 공성 관련
    AddDispatch(CmdCreateCamp,              ParseCastle::CreateCamp);
    AddDispatch(CmdCreateSiegeArms,         ParseCastle::CreateSiegeArms);
    AddDispatch(CmdCastleCmd,               ParseCastle::CastleCmd);
    AddDispatch(CmdCampCmd,                 ParseCastle::CampCmd);
    AddDispatch(CmdSiegeArmsCmd,            ParseCastle::SiegeArmsCmd);
    AddDispatch(CmdCastleRight,             ParseCastle::SetCastleRight);
    AddDispatch(CmdCampRight,               ParseCastle::SetCampRight);
	AddDispatch(CmdCampMessage,				ParseCastle::CampMessage);
	AddDispatch(CmdMiningCampMineralInfo,	ParseCastle::MiningCampMineralInfo);
	AddDispatch(CmdFertilityInfo,			ParseCastle::FertilityInfo);
	AddDispatch(CmdCampShopInfo,			ParseCastle::CampShopInfo);
	AddDispatch(CmdCastleTaxMove,			ParseCastle::CastleTaxMove);
	AddDispatch(CmdCastleMineralInfo,		ParseCastle::CastleMineralInfo);
	AddDispatch(CmdWarOnOff,				ParseCastle::WarOnOff);
	AddDispatch(CmdStatueCmd,				ParseCastle::StatueCmd);

	AddDispatch(CmdAdminCommandLog,			ParseCharAdmin::AdminCommandLog);
	
	AddDispatch(CmdRealmPoint,				ParseCastle::RealmPoint);

	// 듀얼 관련.
	AddDispatch(CmdSaveEnemy,				ParseCharUpdate::SaveEnemy);

	// 조이스틱 관련.
	AddDispatch(CmdKeyInfo,					ParseCharUpdate::UpdateKeyInfo);

	// 아이템 관련
	AddDispatch(CmdCharUseCashItem,			ParseCharUpdate::UseCashItem);
	AddDispatch(CmdExtraEvent,				ParseCharUpdate::ExtraEvent);
}


CMultiDispatch& CGameDispatch::GetDispatchTable()
{
    static CMultiDispatch gameDispatch;
    return gameDispatch;
}


CGameDispatch::CGameDispatch(CSession& Session)
    :   CRylServerDispatch(Session, MAX_PACKET_DISPATCH_PER_PULSE),
        m_nHumanNum(1), 
        m_nAkhanNum(1), 
        m_dwServerID(0)
{


}

CGameDispatch::~CGameDispatch()
{


}

void CGameDispatch::Connected()
{
    DETLOG3(g_Log, "SS:0x%08x/DP:0x%08x/IP:%15s/GameServer Connected",
        &GetSession(), this, GetRemoteAddr().get_addr_string());		
}

void CGameDispatch::Disconnected()
{
    DETLOG3(g_Log, "SS:0x%08x/DP:0x%08x/IP:%15s/GameServer Disconnected",
        &GetSession(), this, GetRemoteAddr().get_addr_string());

    if(0 != m_dwServerID)
    {
        GetDispatchTable().RemoveDispatch(m_dwServerID);

        if(0LL != m_GameItemSerialMgr.GetItemSerial())
        {
            // 아이템 시리얼 DB에 저장
            m_GameItemSerialMgr.SaveItemSerial(
                CDBSingleObject::GetInstance(), m_dwServerID);
        }

        // 채널 정보 업데이트
        DBAgent::SendPacket::UpdateChannelAllServer();

        // 현재 서버에 로그인된 사람을 전부 내려버린다.
        DataStorage::CSessionDataMgr::GetInstance().SessionCloseCurrentServer(m_dwServerID);
    }
}



bool CGameDispatch::DispatchPacket(PktBase* lpPktBase)
{
	// 게임서버에서 DBAgent로 보내는 패킷정보

    PktBase::CMDType cCmd =lpPktBase->GetCmd();

    bool bResult = false;
    GameDispatchFunc lpDispatchFunc = 0;

    
    CSendStream& SendStream = GetSendStream();

    switch(cCmd)
    {
        // 로그인 /  로그아웃 / 데이터 업데이트
    case CmdDBUpdateData:       
		// 유저가 로그인하거나 로그아웃할때 여기로 메시지를 보낸다.
		// 여기서 타임DB를 업데이트 하거나 한다.
        bResult = ParseCharUpdate::Parse(SendStream, m_dwServerID, 
            m_GameItemSerialMgr, static_cast<PktDBUpdate*>(lpPktBase));
        break;

    case CmdSysServerLogin:         bResult = ServerLogin(lpPktBase);           break;  // 서버 로그인
    case CmdSysServerLogout:        bResult = ServerLogout(lpPktBase);          break;  // 서버 로그아웃
    case CmdUserKill:               bResult = UserKillAck(lpPktBase);           break;  // 유저 죽이기
    case CmdCharAdminCmd:           bResult = CharAdminCmd(lpPktBase);          break;  // 운영자 명령
    case CmdGiveItemToTempInven:    bResult = GiveItemToTempInven(lpPktBase);   break;  // 임시인벤으로 아이템 주기
    case CmdCharNameChange:         bResult = ChangeName(lpPktBase);            break;  // 캐릭터 이름을 바꾼다

    default:

        // 기타 명령(테이블에서 검색)
        lpDispatchFunc = CGameDispatchTable::GetInstance().GetDispatch(cCmd);
        if(0 != lpDispatchFunc)
        {
            bResult = lpDispatchFunc(SendStream, lpPktBase);
        }
        else
        {
            ERRLOG5(g_Log, "SS:0x%08x/DP:0x%08x/IP:%15s/Cmd:0x%02X/ServerID:0x%08X / "
                "게임서버 패킷 처리 실패 : 없는 커맨드입니다",
                &GetSession(), this, GetRemoteAddr().get_addr_string(), cCmd, m_dwServerID);

            bResult = true;
        }
        break;
    }

    if(!bResult)
    {
        ERRLOG5(g_Log, "SS:0x%08x/DP:0x%08x/IP:%15s/Cmd:0x%02X/ServerID:0x%08X / 게임서버 패킷 처리 실패 : 처리를 실패했습니다",
            &GetSession(), this, GetRemoteAddr().get_addr_string(), cCmd, m_dwServerID);
    }

    return true;
}




bool CGameDispatch::ServerLogin(PktBase* lpPktBase)
{
    PktSL* lpPktSL = static_cast<PktSL*>(lpPktBase);

    m_dwServerID = lpPktSL->m_dwServerID;

    INFLOG4(g_Log, "SS:0x%08x/DP:0x%08x/IP:%15s/ServerID:0x%08X/게임서버 연결 시도 : 패킷 받음",
        &GetSession(), this, GetRemoteAddr().get_addr_string(), m_dwServerID);

    GET_MULTI_DISPATCH(lpGameDispatch, m_dwServerID, 
        CGameDispatch, GetDispatchTable());
    
    if(0 != lpGameDispatch)
    {
        ERRLOG4(g_Log, "SS:0x%08x/DP:0x%08x/IP:%15s/ServerID:0x%08X/게임서버 연결 실패 : 이미 연결이 있습니다. 접속을 끊습니다.",
            &GetSession(), this, GetRemoteAddr().get_addr_string(), m_dwServerID);

        CloseSession();
    }
    else
    {
        // Dispatch 세팅.
        GetDispatchTable().SetDispatch(m_dwServerID, this);

        // 아이템 시리얼 로드.
        if(!m_GameItemSerialMgr.LoadItemSerial(CDBSingleObject::GetInstance(), m_dwServerID))
        {
            ERRLOG4(g_Log, "SS:0x%08x/DP:0x%08x/IP:%15s/ServerID:0x%08X/게임서버 연결 실패 : 아이템 시리얼 로드 실패. 접속을 끊습니다.",
                &GetSession(), this, GetRemoteAddr().get_addr_string(), m_dwServerID);

            CloseSession();
            return false;
        }
        
        CSendStream& SendStream = GetSendStream();

        // 길드 정보 업데이트
        Guild::CGuildDBMgr::GetInstance().SendGuildDBList(SendStream);
        
        SERVER_ID serverID;
        serverID.dwID = m_dwServerID;

        /// 성 및 캠프, 공성병기 관련 정보 업데이트.
        Castle::CCastleDBMgr::GetInstance().SendCastleInfo(SendStream, serverID.GetZone(), serverID.GetChannel());
		CGameTimeDBMgr::GetInstance().SendGameTimeInfo(GameTime::TYPE_GAMESERVER_LOGIN, true, true, true, true, true);
		CSiegeObjectDBMgr::GetInstance().SendWorldWeaponInfo(SendStream, serverID.GetZone(), serverID.GetChannel());
        CSiegeObjectDBMgr::GetInstance().SendCampInfo(SendStream, serverID.GetZone(), serverID.GetChannel());
        CSiegeObjectDBMgr::GetInstance().SendSiegeArmsInfo(SendStream, serverID.GetZone(), serverID.GetChannel());

		// 컨텐츠 : 다크 카나번 국가 전쟁
		//if (true == CServerSetup::GetInstance().UseContents(GameRYL::STONE_BATTLE))//hayzohar statue war fix
		//{
			// 석상 정보 업데이트
			// 주의 : 성 정보가 석상 정보보다 먼저 보내져야한다.!!
			//		  성에 의해 석상 인챈트 효과가 영향을 받기 때문이다.!!
			if (/*SERVER_ID::STONE_WAR1*/SERVER_ID::CAPITAL <= serverID.GetZone() && serverID.GetZone() <= SERVER_ID::STONE_WAR3)
			{
				CStatueDBMgr::GetInstance().SendStatueInfo(SendStream, serverID.GetChannel());
			}
		//}
/*
		// 컨텐츠 : 신규존
		if (true == CServerSetup::GetInstance().UseContents(GameRYL::NEWZONE_ZONE9))
		{
			// 생명축출기 정보 업데이트
			// 주의 : 성 정보가 생명축출기 정보보다 먼저 보내져야한다.!!			
			if (serverID.GetZone() == SERVER_ID::ZONE9)
			{
				CStatueDBMgr::GetInstance().SendStatueInfo(SendStream, serverID.GetChannel());
			}
		}
*/
		// 지력 정보 업데이트
		CMineralVeinDBMgr::GetInstance().SendFertilityInfo(SendStream, serverID.GetZone(), serverID.GetChannel());

        // 채널 정보 업데이트
        DBAgent::SendPacket::UpdateChannelAllServer();

        // 랭킹 정보 업데이트
        CRankingDBMgr::GetInstance().SendRankingInfo(SendStream);

		// 파티 정보가 있으면 보내준다 //
		CPartyDBMgr::GetInstance().SendToGameServerPartyData(SendStream);

        // 서버 로그인 Ack보냄.
        return SendPacket::ServerLoginAck(m_SendStream, 
            m_dwServerID, m_GameItemSerialMgr.GetItemSerial());		
    }
    
    return true;
}

bool CGameDispatch::ServerLogout(PktBase* lpPktBase)
{
    // 받은 패킷 그대로 돌려 준다.

    char* lpBuffer = m_SendStream.GetBuffer(sizeof(PktBase));
    if(0 != lpBuffer)
    {
        return m_SendStream.WrapHeader(sizeof(PktBase), CmdSysServerLogout, 0, 0);
    }

    return false;
}


bool CGameDispatch::UserKillAck(PktBase* lpPktBase)
{
    // 유저 죽이기를 실패해서, Ack로 오는 경우이다.

    PktUK* lpPktUK = static_cast<PktUK*>(lpPktBase);

    if(2 == lpPktUK->GetError())
    {
        using namespace DataStorage;

        CSessionData* lpSessionData = 
            CSessionDataMgr::GetInstance().GetOpenSession(lpPktUK->m_dwUserID);

        if(0 != lpSessionData && 
            CSessionData::SE_CHAR_ENABLED == lpSessionData->GetSessionState())
        {
            if(lpSessionData->GetServerID() == lpPktUK->m_dwServerID)
            {
                CSessionDataMgr::GetInstance().SessionCloseWithLogout(
                    lpPktUK->m_dwUserID, lpPktUK->m_dwServerID);

                ERRLOG2(g_Log, "UID:%10u / ServerID:0x%08X / 게임서버 세션 강제 종료 : 유저 죽이기 실패", 
                    lpPktUK->m_dwUserID, lpPktUK->m_dwServerID);
            }
            else
            {
                ERRLOG3(g_Log, "UID:%10u / FromServerID:0x%08X / TargetServerID:0x%08X / "
                    "게임서버 세션 강제 종료 실패 : 실패 서버와 죽이기 요청 서버가 다름",
                    lpPktUK->m_dwUserID, lpPktUK->m_dwServerID, lpSessionData->GetServerID());
            }
        }
    }

    return true;
}



bool CGameDispatch::CharAdminCmd(PktBase* lpPktBase)
{
    PktAdmin* lpPktAdmin = static_cast<PktAdmin*>(lpPktBase);

    PktAdmin pktSendAdmin;
    
    // 패킷 내용을 그대로 복사.
    memcpy(&pktSendAdmin, lpPktAdmin, sizeof(PktAdmin));

	// 중계서버 강제 종료
	/*if(lpPktAdmin->m_usCmd == PktAdmin::DBAGENT_KILL)
	{
		int nZero = 0;
		int nError = 10 / nZero;
		*reinterpret_cast<int*>(nError) = 0xFFFFFFFF;
	}*/


    // 추적한 위치 좌표를, 게임 서버로부터 받은 경우라면...
    if (lpPktAdmin->m_usCmd == PktAdmin::REQUEST_TRACE_POS)
    {
        // 해당 게임 서버에게만 TRACE 명령 전송
        pktSendAdmin.m_usCmd = PktAdmin::TRACE_CHAR;

        SERVER_ID serverID;

        serverID.sID.Type       = CServerSetup::GameServer;
        serverID.sID.Group      = CServerSetup::GetInstance().GetServerGroup();
        serverID.sID.Channel    = lpPktAdmin->m_ZoneInfo.m_cChannel;
        serverID.sID.ID         = lpPktAdmin->m_ZoneInfo.m_cZone;
        
        GET_MULTI_DISPATCH(lpGameDispatch, serverID.dwID, 
            CGameDispatch, CGameDispatch::GetDispatchTable());

        if(0 != lpGameDispatch)
        {
			serverID.dwID = m_dwServerID;

            // 게임 서버로 보낼때는 channel 이 -1 이어야 한다. -_-;
			pktSendAdmin.m_ZoneInfo.m_cZone = serverID.GetZone();
			pktSendAdmin.m_ZoneInfo.m_cChannel = -1;

            if(PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktSendAdmin), 
                sizeof(PktAdmin), CmdCharAdminCmd, 0, 0))
            {
                lpGameDispatch->GetSendStream().PutBuffer(
                    reinterpret_cast<char*>(&pktSendAdmin), sizeof(PktAdmin), CmdCharAdminCmd);
            }
        }

        return true;
    }
    else if (lpPktAdmin->m_usCmd == PktAdmin::TRACE_CHAR)
    {
		using namespace DBAgent::DataStorage;

		// 캐릭터가 로그아웃한 상태라면
		if (false == CSessionDataMgr::GetInstance().IsLoadedChar(lpPktAdmin->m_stName))
		{
			// 해당 게임 서버에게 해당 캐릭터가 로그아웃 했다는 정보를 보낸다.
			pktSendAdmin.m_usCmd = PktAdmin::TRACE_CHAR;

			GET_MULTI_DISPATCH(lpGameDispatch, m_dwServerID, 
				CGameDispatch, CGameDispatch::GetDispatchTable());

			if(0 != lpGameDispatch)
			{
				// 게임 서버로 보낼때는 channel 이 -1 이어야 한다. -_-;
				pktSendAdmin.m_ZoneInfo.m_cChannel = -1;
				pktSendAdmin.m_Position.fPointX = 0.0f;
				pktSendAdmin.m_Position.fPointY = 0.0f;
				pktSendAdmin.m_Position.fPointZ = 0.0f;

				if(PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktSendAdmin), 
					sizeof(PktAdmin), CmdCharAdminCmd, 0, PktAdmin::E_LOGOUT_CHAR))
				{
					lpGameDispatch->GetSendStream().PutBuffer(
						reinterpret_cast<char*>(&pktSendAdmin), sizeof(PktAdmin), CmdCharAdminCmd);
				}
			}
		}
		else
		{
			pktSendAdmin.m_usCmd = PktAdmin::REQUEST_TRACE_POS;

			SERVER_ID serverID;
			serverID.dwID = m_dwServerID;

			// TRACE 시켜야 할 캐릭터가 현재 있는 존의 번호를 기억해둔다.
			pktSendAdmin.m_ZoneInfo.m_cZone = serverID.GetZone();
			pktSendAdmin.m_ZoneInfo.m_cChannel = 0;
		}
    }
/*
	else if (lpPktAdmin->m_usCmd == PktAdmin::INFO_CHAR)
	{
		using namespace DBAgent::DataStorage;

		// 캐릭터가 로그아웃한 상태라면
		if (false == CSessionDataMgr::GetInstance().IsLoadedChar(lpPktAdmin->m_stName))
		{
			pktSendAdmin.m_usCmd = PktAdmin::INFO_CHAR;

			GET_MULTI_DISPATCH(lpGameDispatch, m_dwServerID, 
				CGameDispatch, CGameDispatch::GetDispatchTable());

			if(0 != lpGameDispatch)
			{
				// 게임 서버로 보낼때는 channel 이 -1 이어야 한다. -_-;
				pktSendAdmin.m_ZoneInfo.m_cChannel = -1;
				pktSendAdmin.m_Position.fPointX = 0.0f;
				pktSendAdmin.m_Position.fPointY = 0.0f;
				pktSendAdmin.m_Position.fPointZ = 0.0f;

				if (PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktSendAdmin), 
					sizeof(PktAdmin), CmdCharAdminCmd, 0, PktAdmin::E_LOGOUT_CHAR))
				{
					lpGameDispatch->GetSendStream().PutBuffer(
						reinterpret_cast<char*>(&pktSendAdmin), sizeof(PktAdmin), CmdCharAdminCmd);
				}
			}
		}
	}
*/    
    if(PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktSendAdmin), 
        sizeof(PktAdmin), CmdCharAdminCmd, 0, 0))
    {
        // 모든 게임서버로 전송.
        GetDispatchTable().Process(CSendPacketAllServer(
            reinterpret_cast<char*>(&pktSendAdmin), sizeof(PktAdmin), CmdCharAdminCmd));
    }

    return true;
}


bool CGameDispatch::GiveItemToTempInven(PktBase* lpPktBase)
{
    PktGiveItemToTempInven* lpPktGiveItemToTempInven = 
        static_cast<PktGiveItemToTempInven*>(lpPktBase);

    GiveItemInfo* lpPos = lpPktGiveItemToTempInven->m_GiveItemInfo;
    GiveItemInfo* lpEnd = lpPos + lpPktGiveItemToTempInven->m_cGiveItemNum;

    CGiveItemMgr& giveItemMgr = CGiveItemMgr::GetInstance();

    for(; lpPos != lpEnd; ++lpPos)
    {
        // 아이템 넣기에 성공했으면 아이템 시리얼을 채워서 준다. 채워서 준 시리얼을 DB에 갱신한다.
        if(0 != lpPos->m_dwItemUID)
        {
            giveItemMgr.UpdateItemSerial(
                lpPos->m_dwCreateID, lpPos->m_dwItemUID);
        }
    }

    return true;
}

bool CGameDispatch::ChangeName(PktBase* lpPktBase)
{
    PktCharNameChange* lpCharNameChange = 
        reinterpret_cast<PktCharNameChange*>(lpPktBase);

    unsigned long dwUID = lpCharNameChange->m_dwUID;
    unsigned long dwCID = lpCharNameChange->m_dwCID;
    const char* szChangeName	= lpCharNameChange->m_szCharName;
   	Item::ItemPos   ItemPos		= lpCharNameChange->m_ItemPos;         // 계명허가서 위치

    unsigned char   cNameChangeCount = 0;
    unsigned short  usError          = 0;

    using namespace DBAgent::DataStorage;

    CSessionData*   lpSessionData = 
        CSessionDataMgr::GetInstance().GetCharLoadedSession(dwCID);

    CCharacterData* lpCharacterData = 0;

    if (0 != lpSessionData && 
        lpSessionData->GetUID() == dwUID &&
        lpSessionData->GetCID() == dwCID &&
        (0 != (lpCharacterData = lpSessionData->GetCharacterData())) && 
        dwCID == lpCharacterData->GetCID())
    {
        unsigned long dwResult = 0;

		const char* szPrevName = lpCharacterData->GetName();

        // 카운트를 줄인다.
        CHAR_INFOEX infoEx = lpCharacterData->GetInfoEx();
/*
        if (0 == infoEx.cNameChangeCount)
        {
            usError = PktCharNameChange::ERR_NAMECHANGE_COUNT;
        }
        else
*/
		// 이상이 없으면 여기서 아이템을 감소시킨다.
		if (DBComponent::GameDB::ChangeCharName(
            CDBSingleObject::GetInstance(), dwCID, szChangeName, dwResult))
        {   
            if (0 == dwResult)
            {
                CHAR_INFOST charInfoST = lpCharacterData->GetInfo();

                strncpy(charInfoST.Name, szChangeName, CHAR_INFOST::MAX_NAME_LEN);
                charInfoST.Name[CHAR_INFOST::MAX_NAME_LEN - 1] = 0;

                lpCharacterData->SetInfo(charInfoST, true);

				infoEx.cNameChangeCount = 0;
                // --infoEx.cNameChangeCount;     // 아이템으로 바뀐후 이부분은 사용안함.

                cNameChangeCount = infoEx.cNameChangeCount;
                lpCharacterData->SetInfoEx(infoEx);

				// 랭킹에서 찾는다 //
				if(CRankingMgr::GetInstance().GetRank(szPrevName, (unsigned char)lpCharacterData->GetClass()))
				{	
					const CHAR_INFOST& charInfo = lpCharacterData->GetInfo();

					// 데이터 삭제 //
					CRankingDBMgr::GetInstance().DeleteRanking(RankingNode(szPrevName, charInfo.Fame, 
						static_cast<unsigned char>(charInfo.Level), static_cast<unsigned char>(charInfo.Class)));

					// 데이터 업데이트 //
					CRankingDBMgr::GetInstance().UpdateRanking(RankingNode(szChangeName, charInfo.Fame, 
									static_cast<unsigned char>(charInfo.Level), static_cast<unsigned char>(charInfo.Class)));
				}
            }
            else if(1 == dwResult)
            {
                ERRLOG3(g_Log, "UID:%10u / CID:%10u / ChangeName:%s / 캐릭터 이름 바꾸기 실패 : 이름이 이미 있습니다",
                    dwUID, dwCID, szChangeName);

                usError = PktCharNameChange::ERR_ALREADY_USE_NAME;
            }
            else
            {
                ERRLOG4(g_Log, "UID:%10u / CID:%10u / ChangeName:%s / dwResult:%10u / 캐릭터 이름 바꾸기 실패 : 알 수 없는 dwResult값입니다",
                    dwUID, dwCID, szChangeName, dwResult);

                usError = PktCharNameChange::ERR_SERVER;
            }
        }
        else
        {
            ERRLOG3(g_Log, "UID:%10u / CID:%10u / ChangeName:%s / 캐릭터 이름 바꾸기 실패 : DB호출 에러",
                dwUID, dwCID, szChangeName);

            usError = PktCharNameChange::ERR_SERVER;
        }
    }
    else
    {
        ERRLOG3(g_Log, "UID:%10u / CID:%10u / ChangeName:%s / "
            "캐릭터 이름 바꾸기 실패 : 캐릭터가 활성화 된 세션이 없습니다",
            dwUID, dwCID, szChangeName);

        usError = PktCharNameChange::ERR_CANNOT_FIND_CHARACTER;
    }

    GameClientSendPacket::SendCharNameChange(GetSendStream(), dwUID, dwCID, 
        szChangeName, cNameChangeCount, &ItemPos, usError);
    return true;
}


void CGameDispatch::IncCharNum(int nRace)
{
    switch(nRace)
    {
    case CClass::HUMAN: ++m_nHumanNum;  break;
    case CClass::AKHAN: ++m_nAkhanNum;  break;
    }
}

void CGameDispatch::DecCharNum(int nRace)
{
    switch(nRace)
    {
    case CClass::HUMAN: --m_nHumanNum;  break;
    case CClass::AKHAN: --m_nAkhanNum;  break;
    }
}

int CGameDispatch::GetCharNum(int nRace) const
{
    int nNum = -1;

    switch(nRace)
    {
    case CClass::HUMAN: nNum = m_nHumanNum; break;
    case CClass::AKHAN: nNum = m_nAkhanNum; break;
    }

    return (nNum < 1) ? 1 : nNum;
}

int CGameDispatch::GetCharNum() const 
{
    int nUserNumber = m_nHumanNum + m_nAkhanNum - 1;
    return (nUserNumber < 1) ? 1 : nUserNumber;
}



// 나중에 개수 많아지면 분리할 것..
bool ParseServerLog(CSendStream& SendStream, PktBase* lpPktBase)
{
    PktServerLog* lpPktServerLog = static_cast<PktServerLog*>(lpPktBase);

    switch(lpPktServerLog->m_cLogCmd)
    {
    case PktServerLog::ITEM_DUPLICATED_LOG:
        {
            PktItemDuplicated* lpPktItemDup = static_cast<PktItemDuplicated*>(lpPktServerLog);

            if(!DBComponent::GameDB::InsertItemDuplicatedLog(CDBSingleObject::GetInstance(),
                lpPktItemDup->m_dwItemSerial, lpPktItemDup->m_szName, 
                lpPktItemDup->m_dwUID, lpPktItemDup->m_dwCID, lpPktItemDup->m_dwQty))
            {
                ERRLOG5(g_Log, "UID:%10u / CID:%10u / CharName:%s / ItemSerial:0x%I64X / Qty:%u / 아이템 복사가 발견되었습니다",
                    lpPktItemDup->m_dwUID, lpPktItemDup->m_dwCID, lpPktItemDup->m_szName, 
                    lpPktItemDup->m_dwItemSerial, lpPktItemDup->m_dwQty);
            }
        }
        break;


    }

    return true;
}




// -----------------------------------------------------------------------------------
// 각종 함수자 메서드

CGetTotalCount::CGetTotalCount(unsigned short&  usHumanCount, 
                               unsigned short&  usAkhanCount, 
                               unsigned char&   cChannelCount)
                               :   m_usHumanCount(usHumanCount), 
                                   m_usAkhanCount(usAkhanCount), 
                                   m_cChannelCount(cChannelCount)
{
    m_usHumanCount = 0;
    m_usAkhanCount = 0;
    m_cChannelCount = 0;
}

bool CGetTotalCount::operator () (unsigned long dwServerID, CPacketDispatch& packetDispatch)
{
    CGameDispatch& gameDispatch = reinterpret_cast<CGameDispatch&>(packetDispatch);

    int nCharNum = gameDispatch.GetCharNum(CClass::HUMAN);
    m_usHumanCount += nCharNum;

    nCharNum = gameDispatch.GetCharNum(CClass::AKHAN);
    m_usAkhanCount += nCharNum;
     
    ++m_cChannelCount;
    return true;
}

CUserPercentageInZone::CUserPercentageInZone(unsigned short* lpChannelUserNum, 
                                             unsigned char cZone, unsigned char cChannelNum)
                                             :  m_lpChannelUserNum(lpChannelUserNum),
                                                m_cZone(cZone),
                                                m_cChannelNum(cChannelNum),
                                                m_nMaxUserNum(CServerSetup::GetInstance().GetUserLimit())
{
    std::fill_n(m_lpChannelUserNum, m_cChannelNum, 0);
}

bool CUserPercentageInZone::operator () (unsigned long dwServerID, CPacketDispatch& packetDispatch)
{
    CGameDispatch& gameDispatch = reinterpret_cast<CGameDispatch&>(packetDispatch);

    SERVER_ID serverID;
    serverID.dwID = dwServerID;

    if(serverID.GetZone() == m_cZone)
    {
        if(serverID.GetChannel() < m_cChannelNum)
        {
            m_lpChannelUserNum[serverID.GetChannel()] = 
                gameDispatch.GetCharNum() * 100 / m_nMaxUserNum + 1;
        }
    }

    return true;
}


CSendPacketToZone::CSendPacketToZone(const char* szData, unsigned long dwDataLen, 
                                     unsigned char cPacketCmd, unsigned char cZone)
                                     :  m_szData(szData), m_dwDataLen(dwDataLen),
                                        m_cPacketCmd(cPacketCmd), m_cZone(cZone)
{


}

bool CSendPacketToZone::operator () (unsigned long dwServerID, CPacketDispatch& packetDispatch)
{
    SERVER_ID serverID;
    serverID.dwID = dwServerID;

    if(serverID.GetZone() == m_cZone)
    {
        return reinterpret_cast<CRylServerDispatch&>(packetDispatch).GetSendStream().PutBuffer(
            m_szData, m_dwDataLen, m_cPacketCmd);
    }

    return true;
}

}


