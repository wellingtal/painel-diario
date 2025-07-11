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
            // ������ �ø��� DB�� ����
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

        ERRLOG4(g_Log, "SS:0x%08x/DP:0x%08x/IP:%15s/Cmd:0x%02X/ ��� ���� ��Ŷ ó�� ���� : ���� Ŀ�ǵ��Դϴ�",
            &GetSession(), this, GetRemoteAddr().get_addr_string(), cCmd);

        bResult = true;
        break;
    }

    if(!bResult)
    {
        ERRLOG4(g_Log, "SS:0x%08x/DP:0x%08x/IP:%15s/Cmd:0x%02X/ ��� ���� ��Ŷ ó�� ���� : ó���� �����߽��ϴ�",
            &GetSession(), this, GetRemoteAddr().get_addr_string(), cCmd);
    }

    return true;
}

bool CAdminToolDispatch::ParseServerLogin(PktSL* lpPktSL)
{
    m_dwServerID = lpPktSL->m_dwServerID;

    INFLOG4(g_Log, "SS:0x%08x/DP:0x%08x/IP:%15s/ServerID:0x%08X/ ��� ���� ���� �õ� : ��Ŷ ����",
        &GetSession(), this, GetRemoteAddr().get_addr_string(), m_dwServerID);

    GET_MULTI_DISPATCH(lpAdminToolDispatch, m_dwServerID, 
        CAdminToolDispatch, GetDispatchTable());

    if(0 != lpAdminToolDispatch)
    {
        ERRLOG4(g_Log, "SS:0x%08x/DP:0x%08x/IP:%15s/ServerID:0x%08X/ ��� ���� ���� ���� : �̹� ������ �ֽ��ϴ�",
            &GetSession(), this, GetRemoteAddr().get_addr_string(), m_dwServerID);

        CloseSession();
    }
    else
    {
        // Dispatch ����.
        GetDispatchTable().SetDispatch(m_dwServerID, this);

        // ������ �ø��� �ε�.
        if(!m_AdminItemSerialMgr.LoadItemSerial(CDBSingleObject::GetInstance(), m_dwServerID))
        {
            ERRLOG4(g_Log, "SS:0x%08x/DP:0x%08x/IP:%15s/ServerID:0x%08X/ ��� ���� ���� ���� : ������ �ø��� �ε� ����. ������ �����ϴ�",
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
    // ���� ��Ŷ �״�� ���� �ش�.

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

    // ���� DB�� ���� ������, ������ IP�� ��ġ�ϴ� ������ �߰ſ� ���� ���� �ش�.
    CSessionData* lpSessionData = 
		CSessionDataMgr::GetInstance().GetOpenSession(lpPktUK->m_dwUserID);

    if(0 == lpSessionData)
    {
        ERRLOG1(g_Log, "UID:%10u / ��� ���� ���� ���� ���� ���� : ������ �����ϴ�", lpPktUK->m_dwUserID);
    }
    else 
	{
		SERVER_ID serverID;
		serverID.dwID = lpSessionData->GetServerID();
		CSessionData::SessionState eSessionState = lpSessionData->GetSessionState();

		if ((serverID.GetType() == CServerSetup::AuthServer && CSessionData::SE_USER_ENABLED != eSessionState) ||
			(serverID.GetType() == CServerSetup::GameServer && CSessionData::SE_CHAR_ENABLED != eSessionState))
		{
			ERRLOG3(g_Log, "UID:%10u / ServerID:0x%08X / ST:%s / ��� ���� ���� ���� ���� ���� : ������ ĳ���Ͱ� ��Ȱ��ȭ�Ǿ� �ֽ��ϴ�", 
				lpPktUK->m_dwUserID, serverID.dwID, g_szSessionStateString[lpSessionData->GetSessionState()]);
		}
		else if(!SendPacket::UserKill(*lpSessionData))
		{
			ERRLOG3(g_Log, "UID:%10u / ServerID:0x%08X / ST:%s / ��� ���� ���� ���� ���� ���� : ���� ���� ��Ŷ ������ ����", 
				lpPktUK->m_dwUserID, serverID.dwID, g_szSessionStateString[lpSessionData->GetSessionState()]);
		}
	}

    return true;
}


bool CAdminToolDispatch::ParseGetData(PktBase* lpPktBase)
{
    // ������ ��� ����. ��Ŷ ���� ĳ���� ���� �� â�� ������ �ش�.
    PktAdminToolGetData* lpPktAdminToolGetData = static_cast<PktAdminToolGetData*>(lpPktBase);

    switch(lpPktAdminToolGetData->m_cType)
    {
    case PktAdminToolGetData::GET_CHAR_DATA:

        return ParseAdminToolGetData(lpPktAdminToolGetData);

    case PktAdminToolGetData::CHANGE_CHAR_NAME:

        return ParseAdminToolChangeName(lpPktAdminToolGetData);

    default:

        ERRLOG2(g_Log, "IP:%15s / Cmd:%d / �� �� ���� ��� ������ ��� Ŀ�ǵ��Դϴ�.", 
            GetRemoteAddr().get_addr_string(), lpPktAdminToolGetData->m_cType);

        return true;
    }

    return false;
}


bool CAdminToolDispatch::ParseSetData(PktBase* lpPktBase)
{
    // ������ ����ϱ�. ��Ŷ ���� ĳ���� ���� �����͸� ����.
    // CharLogin�� �����ִ� ������, â�� ������ 1, â�� ������ 2, â�� ����

    // ������ ��� ����. ��Ŷ ���� ĳ���� ���� �� â�� ������ �ش�.
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

        ERRLOG2(g_Log, "IP:%15s / Cmd:%d / �� �� ���� ��� ������ �� ��Ŷ Ŀ�ǵ��Դϴ�", 
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
        // �߸��� ��Ŷ��
        wError = PktAdminToolGetData::INVALID_REQUEST;
    }
    else
    {        
        // UID, CID�� ������ ĳ���� �̸��� ������ UID�� CID�� ��� �´�.
        if(0 == dwUID || 0 == dwCID || 0 == cOldServerGroupID)
        {
            if(!GetUIDCIDFromName(lpPktAdminToolGetData->m_szName, 
                dwUID, dwCID, cOldServerGroupID))
            {
                // DB Query ����.
                wError = PktAdminToolGetData::GET_UIDCID_QUERY_FAILED;				
            }

            if(0 == dwUID || 0 == dwCID || 0 == cOldServerGroupID)
            {
                // ���� �����Ͱ� �̻���.
                wError = PktAdminToolGetData::INVALID_UID_CID_DATA;				
            }
        }

        DETLOG4(g_Log, "UID:%10u / ServerGroupID:%d / CID:%10u / IP:%15s / ����κ��� ������ ��û ����", 
            dwUID, cOldServerGroupID, dwCID, GetRemoteAddr().get_addr_string());

        if(0 == wError)
        {
            using namespace DataStorage;

            CStoreData*     lpStoreData     = 0;
            CCharacterData* lpCharacterData = 0;

            CSessionData* lpSessionData = CSessionDataMgr::GetInstance().GetOpenSession(dwUID);

            if(0 != lpSessionData)
            {
                // �̹� �ε�� ���ǿ��� ������ ������.
                if(dwUID == lpSessionData->GetUID())
                {
                    lpStoreData = lpSessionData->GetStoreData();
                }

                if(dwCID == lpSessionData->GetCID())
                {
                    lpCharacterData = lpSessionData->GetCharacterData();
                }
            }

            // ĳ������ ������ �ε�. ĳ���� �����Ͱ� ������, DB���� �ε��ؼ� ĳ���� �߰�.
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

    // ���� ����
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
        DETLOG3(g_Log, "UID:%10u / CID:%10u / IP:%15s / ��ڰ� ����� ������ ���� �õ�",
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
				// Ȱ��ȭ�� ĳ���� ���� ����
				wError = PktAdminToolSetData::LOGIN_ANOTHER_GAMESERVER;
			}
			else
			{
                // �̹� �ε�� ���ǿ��� ������ ������.
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
            // ĳ������ ������ �ε�. ĳ���� �����Ͱ� ������, DB���� �ε��ؼ� ĳ���� �߰�.
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
                    // ������ ���� DB�� ������Ʈ�Ϸ� �õ��Ѵ�.      : ������ ���� �� �ʿ� ����. �׳� �Ѵ�.
                    break;

                case PktAdminToolSetData::FINISH_UPDATE_DATA:	
                    // �����͸� DB�� ������Ʈ�ϰ� ������ �ݴ´�.    : ������ ���� ���� �ʿ� ����. �׳� �Ѵ�.
                    break;

                case PktAdminToolSetData::CHAR_BASIC_DATA:		// ĳ���� ����. ���� �������� ��. 

                    // ������ ������� �� �������� Ȯ���Ѵ�. ������� �� �����̸�, �����͸� �����Ѵ�.
                    {
                        unsigned short* lpSizeArray = reinterpret_cast<unsigned short*>(lpPktAdminToolSetData + 1);
                        char* lpData = reinterpret_cast<char*>(lpSizeArray + DBUpdateData::MAX_UPDATE_DB);

                        if(!lpCharacterData->SerializeIn(lpData, lpSizeArray, 
                            dwDataSize - sizeof(unsigned short) * DBUpdateData::MAX_UPDATE_DB, DBUpdateData::MAX_UPDATE_DB))
                        {
                            ERRLOG3(g_Log, "UID:%10u / CID:%10u / IP:%15s / ��� ������ ���� ���� : CHAR_BASIC_DATA",
                                dwUID, dwCID, GetRemoteAddr().get_addr_string());
                            
                            wError = PktAdminToolSetData::SETTING_DATA_FAILED;
                        }
                        else
                        {
                            INFLOG3(g_Log, "UID:%10u / CID:%10u / IP:%15s / ��� ������ ���� ���� : CHAR_BASIC_DATA",
                                dwUID, dwCID, GetRemoteAddr().get_addr_string());
                        }
                    }
                    break;

                case PktAdminToolSetData::CHAR_EXTRA_DATA:		// ĳ���� �߰� ������. ���� �������� ��.
                    
                    {
                        unsigned long dwExpectDataSize = sizeof(CHAR_INFOEX) + sizeof(QUEST) + 
                            sizeof(HISTORY) + sizeof(CONFIG) + sizeof(STORE_INFO);

                        if(dwDataSize == dwExpectDataSize)
                        {
                            char* lpCharDataExPos = reinterpret_cast<char*>(lpPktAdminToolSetData + 1);

                            // CHAR_INFOEX	: ĳ���� �߰� ����
                            lpCharacterData->SetInfoEx(*reinterpret_cast<CHAR_INFOEX*>(lpCharDataExPos));
                            lpCharDataExPos += sizeof(CHAR_INFOEX);

                            // QUEST		: ����Ʈ
                            lpCharacterData->SetQuest(*reinterpret_cast<QUEST*>(lpCharDataExPos));
                            lpCharDataExPos += sizeof(QUEST);		

                            // HISTORY		: �����丮
                            lpCharacterData->SetHistory(*reinterpret_cast<HISTORY*>(lpCharDataExPos));
                            lpCharDataExPos += sizeof(HISTORY);		

                            // CONFIG		: ����
                            lpCharacterData->SetConfig(*reinterpret_cast<CONFIG*>(lpCharDataExPos));
                            lpCharDataExPos += sizeof(CONFIG);		

                            // STORE_INFO	: â�� ����
                            lpStoreData->SetStoreInfo(*reinterpret_cast<STORE_INFO*>(lpCharDataExPos));
                            lpCharDataExPos += sizeof(STORE_INFO);	
                        }
                        else
                        {
                            wError = PktAdminToolSetData::SETTING_DATA_FAILED;
                        }
                    }
                    break;

                case PktAdminToolSetData::CHAR_FRIEND_DATA:		// ĳ���� ģ�� ����

                    if(!lpCharacterData->FriendSerializeIn(
                        reinterpret_cast<char*>(lpPktAdminToolSetData + 1), dwDataSize))
                    {
                        wError = PktAdminToolSetData::SETTING_DATA_FAILED;
                    }
                    break;

                case PktAdminToolSetData::CHAR_BAN_DATA:		// ĳ���� �ź� ����

                    if(!lpCharacterData->BanSerializeIn(
                        reinterpret_cast<char*>(lpPktAdminToolSetData + 1), dwDataSize))
                    {
                        wError = PktAdminToolSetData::SETTING_DATA_FAILED;
                    }
                    break;

                case PktAdminToolSetData::STORE_12_DATA:		// â�� 1, 2�� ������

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

                case PktAdminToolSetData::STORE_34_DATA:		// â�� 3, 4�� ������

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

    // ItemSerial ������Ʈ
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
        ERRLOG2(g_Log, "IP:%15s / StartTime:%d / ������ ���� ���� ���� : �ð��� ���� �ʽ��ϴ�", 
            GetRemoteAddr().get_addr_string(), lpPktItemQtyControl->m_tStartTime);
    }
    else 
    {
        tm tStartTime = *lpTm;

        lpTm = localtime(&lpPktItemQtyControl->m_tEndTime);
        if(0 == lpTm)
        {
            ERRLOG2(g_Log, "IP:%15s / EndTime:%d / ������ ���� ���� ���� : �ð��� ���� �ʽ��ϴ�", 
                GetRemoteAddr().get_addr_string(), lpPktItemQtyControl->m_tEndTime);
        }
        else
        {
            tm tEndTime = *lpTm;

            if(lpPktItemQtyControl->m_cGroup != CServerSetup::GetInstance().GetServerGroup())
            {
                ERRLOG3(g_Log, "IP:%15s / CurrentServerGroup:%d / ReceivedServerGroup:%d / ������ ���� ���� ���� : ���� �׷��� ���� �ʽ��ϴ�",
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

                    DETLOG5(g_Log, "������ ���� ���� �߰� %s : ID:%u/Qty:%u/StartTime:%d/EndTime:%d",
                        bResult ? "����" : "����", 
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

                    DETLOG5(g_Log, "������ ���� ���� ���� %s : ID:%u/Qty:%u/StartTime:%d/EndTime:%d",
                        bResult ? "����" : "����", 
                        lpPktItemQtyControl->m_dwItemTypeID, lpPktItemQtyControl->m_dwItemQty,
                        lpPktItemQtyControl->m_tStartTime, lpPktItemQtyControl->m_tEndTime);

                    SendPacket::ItemQtyCheck(lpPktItemQtyControl->m_dwItemTypeID, 
                        lpPktItemQtyControl->m_dwItemQty, 0,
                        lpPktItemQtyControl->m_tStartTime, lpPktItemQtyControl->m_tEndTime, 
                        PktItemQtyControl::DEL, bResult ? 0 : 1);
                    break;

                case PktItemQtyControl::STATUS_REQUEST:

                    DETLOG0(g_Log, "������ ���� ���� ���� ��û");
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
                // �̸��� �ٲ۴�.
                CHAR_INFOST charInfoST = lpCharacterData->GetInfo();

                strncpy(charInfoST.Name, szChangeName, CHAR_INFOST::MAX_NAME_LEN);
                charInfoST.Name[CHAR_INFOST::MAX_NAME_LEN - 1] = 0;

                lpCharacterData->SetInfo(charInfoST, true);
            }
            else if(1 == dwResult)
            {
                ERRLOG3(g_Log, "UID:%10u / CID:%10u / ChangeName:%s / ����� ĳ���� �̸� �ٲٱ� ���� : �̸��� �̹� �ֽ��ϴ�",
                    dwUID, dwCID, szChangeName);

                usError = PktCharNameChange::ERR_ALREADY_USE_NAME;
            }
            else
            {
                ERRLOG4(g_Log, "UID:%10u / CID:%10u / ChangeName:%s / dwResult:%10u / ����� ĳ���� �̸� �ٲٱ� ���� : �� �� ���� dwResult���Դϴ�",
                    dwUID, dwCID, szChangeName, dwResult);

                usError = PktCharNameChange::ERR_SERVER;
            }
        }
        else
        {
            ERRLOG3(g_Log, "UID:%10u / CID:%10u / ChangeName:%s / ����� ĳ���� �̸� �ٲٱ� ���� : DBȣ�� ����",
                dwUID, dwCID, szChangeName);

            usError = PktCharNameChange::ERR_SERVER;
        }
    }
    else
    {
        ERRLOG3(g_Log, "UID:%10u / CID:%10u / ChangeName:%s / "
            "����� ĳ���� �̸� �ٲٱ� ���� : �ش��ϴ� ĳ���͸� ã�� �� �����ϴ�.", dwUID, dwCID, szChangeName);

        usError = PktCharNameChange::ERR_CANNOT_FIND_CHARACTER;
    }

    if (0 != lpSessionData && 0 != lpCharacterData && 0 == usError)
    {
        // ��� ���� ������ �����Ѵ�.
        DBAgent::CGameDispatch::GetDispatchTable().Process(
            CSendNameChanged(dwUID, dwCID, szChangeName, 
            lpCharacterData->GetInfoEx().cNameChangeCount, usError));
    }

    // ����� Ack�� �����Ѵ�.
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

    ERRLOG2(g_Log, "ĳ���� �̸����� UID / CID ��� ���� ������ �����߽��ϴ�. %s : %s", 
        CDBSingleObject::GetInstance().GetErrorString(), szQuery);

    return false;
}


}
