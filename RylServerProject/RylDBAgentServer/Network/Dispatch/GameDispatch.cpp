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
    // ������ ������Ʈ ����
    AddDispatch(CmdDepositUpdate,           ParseCharUpdate::UpdateDepositDB);
    AddDispatch(CmdDeposit,                 ParseCharUpdate::UpdateDeposit);
    AddDispatch(CmdFriendDB,                ParseCharUpdate::UpdateFriendDB);
    AddDispatch(CmdQuestDB,                 ParseCharUpdate::UpdateQuestDB);
    AddDispatch(CmdConfigInfoDB,            ParseCharUpdate::UpdateConfigInfoDB);

    // ���̵� ����
    AddDispatch(CmdAgentZone,               ParseMoveZone::Parse);

    // ��Ƽ ����
    AddDispatch(CmdAgentParty,              ParseParty::Parse);

    // ��Ʋ�׶��� ����
    AddDispatch(CmdBGServerCharSlot,        ParseCharManage::BGServerCharSlot);

    // �α� ����
    AddDispatch(CmdServerLog,               ParseServerLog);
    
    // ��� ����
    AddDispatch(CmdCreateGuild,             ParseGuild::CreateGuild);
    AddDispatch(CmdGuildCmd,                ParseGuild::GuildCmd);
    AddDispatch(CmdGuildMark,               ParseGuild::GuildMark);
    AddDispatch(CmdGuildLevel,              ParseGuild::GuildLevel);
	AddDispatch(CmdGuildRelation,           ParseGuild::GuildRelation);
    AddDispatch(CmdGuildInclination,	    ParseGuild::GuildInclination);
    AddDispatch(CmdGuildRight,			    ParseGuild::SetGuildRight);
    AddDispatch(CmdGuildSafe,				ParseGuild::GuildSafe);
    AddDispatch(CmdGuildMemberInfoUpdate,   ParseGuild::GuildMemberInfoUpdate);

    // ���� ����
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

	// ��� ����.
	AddDispatch(CmdSaveEnemy,				ParseCharUpdate::SaveEnemy);

	// ���̽�ƽ ����.
	AddDispatch(CmdKeyInfo,					ParseCharUpdate::UpdateKeyInfo);

	// ������ ����
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
            // ������ �ø��� DB�� ����
            m_GameItemSerialMgr.SaveItemSerial(
                CDBSingleObject::GetInstance(), m_dwServerID);
        }

        // ä�� ���� ������Ʈ
        DBAgent::SendPacket::UpdateChannelAllServer();

        // ���� ������ �α��ε� ����� ���� ����������.
        DataStorage::CSessionDataMgr::GetInstance().SessionCloseCurrentServer(m_dwServerID);
    }
}



bool CGameDispatch::DispatchPacket(PktBase* lpPktBase)
{
	// ���Ӽ������� DBAgent�� ������ ��Ŷ����

    PktBase::CMDType cCmd =lpPktBase->GetCmd();

    bool bResult = false;
    GameDispatchFunc lpDispatchFunc = 0;

    
    CSendStream& SendStream = GetSendStream();

    switch(cCmd)
    {
        // �α��� /  �α׾ƿ� / ������ ������Ʈ
    case CmdDBUpdateData:       
		// ������ �α����ϰų� �α׾ƿ��Ҷ� ����� �޽����� ������.
		// ���⼭ Ÿ��DB�� ������Ʈ �ϰų� �Ѵ�.
        bResult = ParseCharUpdate::Parse(SendStream, m_dwServerID, 
            m_GameItemSerialMgr, static_cast<PktDBUpdate*>(lpPktBase));
        break;

    case CmdSysServerLogin:         bResult = ServerLogin(lpPktBase);           break;  // ���� �α���
    case CmdSysServerLogout:        bResult = ServerLogout(lpPktBase);          break;  // ���� �α׾ƿ�
    case CmdUserKill:               bResult = UserKillAck(lpPktBase);           break;  // ���� ���̱�
    case CmdCharAdminCmd:           bResult = CharAdminCmd(lpPktBase);          break;  // ��� ���
    case CmdGiveItemToTempInven:    bResult = GiveItemToTempInven(lpPktBase);   break;  // �ӽ��κ����� ������ �ֱ�
    case CmdCharNameChange:         bResult = ChangeName(lpPktBase);            break;  // ĳ���� �̸��� �ٲ۴�

    default:

        // ��Ÿ ���(���̺��� �˻�)
        lpDispatchFunc = CGameDispatchTable::GetInstance().GetDispatch(cCmd);
        if(0 != lpDispatchFunc)
        {
            bResult = lpDispatchFunc(SendStream, lpPktBase);
        }
        else
        {
            ERRLOG5(g_Log, "SS:0x%08x/DP:0x%08x/IP:%15s/Cmd:0x%02X/ServerID:0x%08X / "
                "���Ӽ��� ��Ŷ ó�� ���� : ���� Ŀ�ǵ��Դϴ�",
                &GetSession(), this, GetRemoteAddr().get_addr_string(), cCmd, m_dwServerID);

            bResult = true;
        }
        break;
    }

    if(!bResult)
    {
        ERRLOG5(g_Log, "SS:0x%08x/DP:0x%08x/IP:%15s/Cmd:0x%02X/ServerID:0x%08X / ���Ӽ��� ��Ŷ ó�� ���� : ó���� �����߽��ϴ�",
            &GetSession(), this, GetRemoteAddr().get_addr_string(), cCmd, m_dwServerID);
    }

    return true;
}




bool CGameDispatch::ServerLogin(PktBase* lpPktBase)
{
    PktSL* lpPktSL = static_cast<PktSL*>(lpPktBase);

    m_dwServerID = lpPktSL->m_dwServerID;

    INFLOG4(g_Log, "SS:0x%08x/DP:0x%08x/IP:%15s/ServerID:0x%08X/���Ӽ��� ���� �õ� : ��Ŷ ����",
        &GetSession(), this, GetRemoteAddr().get_addr_string(), m_dwServerID);

    GET_MULTI_DISPATCH(lpGameDispatch, m_dwServerID, 
        CGameDispatch, GetDispatchTable());
    
    if(0 != lpGameDispatch)
    {
        ERRLOG4(g_Log, "SS:0x%08x/DP:0x%08x/IP:%15s/ServerID:0x%08X/���Ӽ��� ���� ���� : �̹� ������ �ֽ��ϴ�. ������ �����ϴ�.",
            &GetSession(), this, GetRemoteAddr().get_addr_string(), m_dwServerID);

        CloseSession();
    }
    else
    {
        // Dispatch ����.
        GetDispatchTable().SetDispatch(m_dwServerID, this);

        // ������ �ø��� �ε�.
        if(!m_GameItemSerialMgr.LoadItemSerial(CDBSingleObject::GetInstance(), m_dwServerID))
        {
            ERRLOG4(g_Log, "SS:0x%08x/DP:0x%08x/IP:%15s/ServerID:0x%08X/���Ӽ��� ���� ���� : ������ �ø��� �ε� ����. ������ �����ϴ�.",
                &GetSession(), this, GetRemoteAddr().get_addr_string(), m_dwServerID);

            CloseSession();
            return false;
        }
        
        CSendStream& SendStream = GetSendStream();

        // ��� ���� ������Ʈ
        Guild::CGuildDBMgr::GetInstance().SendGuildDBList(SendStream);
        
        SERVER_ID serverID;
        serverID.dwID = m_dwServerID;

        /// �� �� ķ��, �������� ���� ���� ������Ʈ.
        Castle::CCastleDBMgr::GetInstance().SendCastleInfo(SendStream, serverID.GetZone(), serverID.GetChannel());
		CGameTimeDBMgr::GetInstance().SendGameTimeInfo(GameTime::TYPE_GAMESERVER_LOGIN, true, true, true, true, true);
		CSiegeObjectDBMgr::GetInstance().SendWorldWeaponInfo(SendStream, serverID.GetZone(), serverID.GetChannel());
        CSiegeObjectDBMgr::GetInstance().SendCampInfo(SendStream, serverID.GetZone(), serverID.GetChannel());
        CSiegeObjectDBMgr::GetInstance().SendSiegeArmsInfo(SendStream, serverID.GetZone(), serverID.GetChannel());

		// ������ : ��ũ ī���� ���� ����
		//if (true == CServerSetup::GetInstance().UseContents(GameRYL::STONE_BATTLE))//hayzohar statue war fix
		//{
			// ���� ���� ������Ʈ
			// ���� : �� ������ ���� �������� ���� ���������Ѵ�.!!
			//		  ���� ���� ���� ��æƮ ȿ���� ������ �ޱ� �����̴�.!!
			if (/*SERVER_ID::STONE_WAR1*/SERVER_ID::CAPITAL <= serverID.GetZone() && serverID.GetZone() <= SERVER_ID::STONE_WAR3)
			{
				CStatueDBMgr::GetInstance().SendStatueInfo(SendStream, serverID.GetChannel());
			}
		//}
/*
		// ������ : �ű���
		if (true == CServerSetup::GetInstance().UseContents(GameRYL::NEWZONE_ZONE9))
		{
			// ��������� ���� ������Ʈ
			// ���� : �� ������ ��������� �������� ���� ���������Ѵ�.!!			
			if (serverID.GetZone() == SERVER_ID::ZONE9)
			{
				CStatueDBMgr::GetInstance().SendStatueInfo(SendStream, serverID.GetChannel());
			}
		}
*/
		// ���� ���� ������Ʈ
		CMineralVeinDBMgr::GetInstance().SendFertilityInfo(SendStream, serverID.GetZone(), serverID.GetChannel());

        // ä�� ���� ������Ʈ
        DBAgent::SendPacket::UpdateChannelAllServer();

        // ��ŷ ���� ������Ʈ
        CRankingDBMgr::GetInstance().SendRankingInfo(SendStream);

		// ��Ƽ ������ ������ �����ش� //
		CPartyDBMgr::GetInstance().SendToGameServerPartyData(SendStream);

        // ���� �α��� Ack����.
        return SendPacket::ServerLoginAck(m_SendStream, 
            m_dwServerID, m_GameItemSerialMgr.GetItemSerial());		
    }
    
    return true;
}

bool CGameDispatch::ServerLogout(PktBase* lpPktBase)
{
    // ���� ��Ŷ �״�� ���� �ش�.

    char* lpBuffer = m_SendStream.GetBuffer(sizeof(PktBase));
    if(0 != lpBuffer)
    {
        return m_SendStream.WrapHeader(sizeof(PktBase), CmdSysServerLogout, 0, 0);
    }

    return false;
}


bool CGameDispatch::UserKillAck(PktBase* lpPktBase)
{
    // ���� ���̱⸦ �����ؼ�, Ack�� ���� ����̴�.

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

                ERRLOG2(g_Log, "UID:%10u / ServerID:0x%08X / ���Ӽ��� ���� ���� ���� : ���� ���̱� ����", 
                    lpPktUK->m_dwUserID, lpPktUK->m_dwServerID);
            }
            else
            {
                ERRLOG3(g_Log, "UID:%10u / FromServerID:0x%08X / TargetServerID:0x%08X / "
                    "���Ӽ��� ���� ���� ���� ���� : ���� ������ ���̱� ��û ������ �ٸ�",
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
    
    // ��Ŷ ������ �״�� ����.
    memcpy(&pktSendAdmin, lpPktAdmin, sizeof(PktAdmin));

	// �߰輭�� ���� ����
	/*if(lpPktAdmin->m_usCmd == PktAdmin::DBAGENT_KILL)
	{
		int nZero = 0;
		int nError = 10 / nZero;
		*reinterpret_cast<int*>(nError) = 0xFFFFFFFF;
	}*/


    // ������ ��ġ ��ǥ��, ���� �����κ��� ���� �����...
    if (lpPktAdmin->m_usCmd == PktAdmin::REQUEST_TRACE_POS)
    {
        // �ش� ���� �������Ը� TRACE ��� ����
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

            // ���� ������ �������� channel �� -1 �̾�� �Ѵ�. -_-;
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

		// ĳ���Ͱ� �α׾ƿ��� ���¶��
		if (false == CSessionDataMgr::GetInstance().IsLoadedChar(lpPktAdmin->m_stName))
		{
			// �ش� ���� �������� �ش� ĳ���Ͱ� �α׾ƿ� �ߴٴ� ������ ������.
			pktSendAdmin.m_usCmd = PktAdmin::TRACE_CHAR;

			GET_MULTI_DISPATCH(lpGameDispatch, m_dwServerID, 
				CGameDispatch, CGameDispatch::GetDispatchTable());

			if(0 != lpGameDispatch)
			{
				// ���� ������ �������� channel �� -1 �̾�� �Ѵ�. -_-;
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

			// TRACE ���Ѿ� �� ĳ���Ͱ� ���� �ִ� ���� ��ȣ�� ����صд�.
			pktSendAdmin.m_ZoneInfo.m_cZone = serverID.GetZone();
			pktSendAdmin.m_ZoneInfo.m_cChannel = 0;
		}
    }
/*
	else if (lpPktAdmin->m_usCmd == PktAdmin::INFO_CHAR)
	{
		using namespace DBAgent::DataStorage;

		// ĳ���Ͱ� �α׾ƿ��� ���¶��
		if (false == CSessionDataMgr::GetInstance().IsLoadedChar(lpPktAdmin->m_stName))
		{
			pktSendAdmin.m_usCmd = PktAdmin::INFO_CHAR;

			GET_MULTI_DISPATCH(lpGameDispatch, m_dwServerID, 
				CGameDispatch, CGameDispatch::GetDispatchTable());

			if(0 != lpGameDispatch)
			{
				// ���� ������ �������� channel �� -1 �̾�� �Ѵ�. -_-;
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
        // ��� ���Ӽ����� ����.
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
        // ������ �ֱ⿡ ���������� ������ �ø����� ä���� �ش�. ä���� �� �ø����� DB�� �����Ѵ�.
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
   	Item::ItemPos   ItemPos		= lpCharNameChange->m_ItemPos;         // ����㰡�� ��ġ

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

        // ī��Ʈ�� ���δ�.
        CHAR_INFOEX infoEx = lpCharacterData->GetInfoEx();
/*
        if (0 == infoEx.cNameChangeCount)
        {
            usError = PktCharNameChange::ERR_NAMECHANGE_COUNT;
        }
        else
*/
		// �̻��� ������ ���⼭ �������� ���ҽ�Ų��.
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
                // --infoEx.cNameChangeCount;     // ���������� �ٲ��� �̺κ��� ������.

                cNameChangeCount = infoEx.cNameChangeCount;
                lpCharacterData->SetInfoEx(infoEx);

				// ��ŷ���� ã�´� //
				if(CRankingMgr::GetInstance().GetRank(szPrevName, (unsigned char)lpCharacterData->GetClass()))
				{	
					const CHAR_INFOST& charInfo = lpCharacterData->GetInfo();

					// ������ ���� //
					CRankingDBMgr::GetInstance().DeleteRanking(RankingNode(szPrevName, charInfo.Fame, 
						static_cast<unsigned char>(charInfo.Level), static_cast<unsigned char>(charInfo.Class)));

					// ������ ������Ʈ //
					CRankingDBMgr::GetInstance().UpdateRanking(RankingNode(szChangeName, charInfo.Fame, 
									static_cast<unsigned char>(charInfo.Level), static_cast<unsigned char>(charInfo.Class)));
				}
            }
            else if(1 == dwResult)
            {
                ERRLOG3(g_Log, "UID:%10u / CID:%10u / ChangeName:%s / ĳ���� �̸� �ٲٱ� ���� : �̸��� �̹� �ֽ��ϴ�",
                    dwUID, dwCID, szChangeName);

                usError = PktCharNameChange::ERR_ALREADY_USE_NAME;
            }
            else
            {
                ERRLOG4(g_Log, "UID:%10u / CID:%10u / ChangeName:%s / dwResult:%10u / ĳ���� �̸� �ٲٱ� ���� : �� �� ���� dwResult���Դϴ�",
                    dwUID, dwCID, szChangeName, dwResult);

                usError = PktCharNameChange::ERR_SERVER;
            }
        }
        else
        {
            ERRLOG3(g_Log, "UID:%10u / CID:%10u / ChangeName:%s / ĳ���� �̸� �ٲٱ� ���� : DBȣ�� ����",
                dwUID, dwCID, szChangeName);

            usError = PktCharNameChange::ERR_SERVER;
        }
    }
    else
    {
        ERRLOG3(g_Log, "UID:%10u / CID:%10u / ChangeName:%s / "
            "ĳ���� �̸� �ٲٱ� ���� : ĳ���Ͱ� Ȱ��ȭ �� ������ �����ϴ�",
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



// ���߿� ���� �������� �и��� ��..
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
                ERRLOG5(g_Log, "UID:%10u / CID:%10u / CharName:%s / ItemSerial:0x%I64X / Qty:%u / ������ ���簡 �߰ߵǾ����ϴ�",
                    lpPktItemDup->m_dwUID, lpPktItemDup->m_dwCID, lpPktItemDup->m_szName, 
                    lpPktItemDup->m_dwItemSerial, lpPktItemDup->m_dwQty);
            }
        }
        break;


    }

    return true;
}




// -----------------------------------------------------------------------------------
// ���� �Լ��� �޼���

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


