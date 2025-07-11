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

//// ĳ���� �α���
bool CharLogin(CSendStream& SendStream, 
    unsigned long dwServerID, PktDBUpdate* lpPktDBUpdate);

// ĳ���� �α׾ƿ�
bool CharLogout(CSendStream& SendStream, unsigned long dwServerID, 
                Item::CItemSerialMgr& itemSerialMgr, PktDBUpdate* lpPktDBUpdate);

// ĳ���� ���̵�
bool CharMoveZone(CSendStream& SendStream, unsigned long dwServerID, 
                  Item::CItemSerialMgr& itemSerialMgr, PktDBUpdate* lpPktDBUpdate);

// ĳ���� ������Ʈ
bool CharUpdate(CSendStream& SendStream, unsigned long dwServerID, 
                Item::CItemSerialMgr& itemSerialMgr, PktDBUpdate* lpPktDBUpdate);


bool Parse(CSendStream& SendStream, unsigned long dwServerID, 
           Item::CItemSerialMgr& itemSerialMgr, PktDBUpdate* lpPktDBUpdate)
{    
    bool bResult = false;

    switch(lpPktDBUpdate->m_TypeCode)
    {
    case DBUpdateData::LOGIN:	            // ĳ���� �α���
        bResult = CharLogin(SendStream, dwServerID, lpPktDBUpdate);
        break;

    case DBUpdateData::LOGOUT:          	// ĳ���� �α׾ƿ�
    case DBUpdateData::ABNORMAL_LOGOUT:     // ĳ���� ������ �α׾ƿ�(������ ������Ʈ ����)
        bResult = CharLogout(SendStream, dwServerID, itemSerialMgr, lpPktDBUpdate);
        break;

    case DBUpdateData::ZONEMOVE:            // ĳ���� ���̵�
        bResult = CharMoveZone(SendStream, dwServerID, itemSerialMgr, lpPktDBUpdate);
        break;

    case DBUpdateData::UPDATE:              // ĳ���� ������Ʈ
        bResult = CharUpdate(SendStream, dwServerID, itemSerialMgr, lpPktDBUpdate);
        break;
            
    default:
        
        ERRLOG1(g_Log, "ParseCharUpdate ���� : �� �� ���� ��� 0x%04X �� �޾ҽ��ϴ�.", lpPktDBUpdate->m_TypeCode);
        bResult = true;        
        break;
    }

    return bResult;
}

// ĳ���� �α���
bool CharLogin(CSendStream& SendStream, unsigned long dwServerID, PktDBUpdate* lpPktDBUpdate)
{
	// ������ �α����� �õ��ߴ�.
	// DBAgent�� �ش� ��Ŷ�� �޾Ҵ�. UID������ ������ ���� ��Ŷ�� �����Ѵ�.

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
        // ������ ���� ��쿡�� �α��� �Ұ�. ���� �α��ν� ���� ���� �ϴµ� �� ����!
        // ���� �α׾ƿ��ÿ� ���� ���ִ� �͵� �ƴϱ�!
        ERRLOG5(g_Log, "UID:%10u / CID:%10u / SessionID:%10u / ServerID:0x%08X / SessionData:0x%p / "
            "ĳ���� �α��� ���� : �����̳� ĳ���� �����Ͱ� �����ϴ�.",
            dwUID, dwCID, dwSessionID, dwServerID, lpSessionData);

        usCharLoginError = 1;
    }
    else if(lpSessionData->GetSessionID() != dwSessionID)
    {
        // ���� ID�� �ٸ��ϴ�.
        ERRLOG5(g_Log, "UID:%10u / CID:%10u / RealSessionID:%10u / ReceivedSessionID:%10u / ServerID:0x%08X / "
            "ĳ���� �α��� ���� : ���� ID�� �ٸ��ϴ�", dwUID, dwCID, 
            lpSessionData->GetSessionID(), dwSessionID, dwServerID);
        
        usCharLoginError = 1;
    }
    else if(CSessionData::SE_USER_ENABLED == lpSessionData->GetSessionState() ||
        CSessionData::SE_CHAR_ENABLED == lpSessionData->GetSessionState())
    {
        // Ȱ��ȭ�� ĳ���ͷ�  �α����� �õ��߽��ϴ�.
        ERRLOG5(g_Log, "UID:%10u / CID:%10u / SessionID:%10u / ServerID:0x%08X / ST:%s"
            "ĳ���� �α��� ���� : Ȱ��ȭ�� �������� �α����� �õ��Ͽ����ϴ�", dwUID, dwCID, 
            dwSessionID, dwServerID, g_szSessionStateString[lpSessionData->GetSessionState()]);

        usCharLoginError = 1;
    }
    else if(!lpSessionData->HasCharacter(dwCID))
    {
        // ���� �����ִ� ĳ���Ͱ� �ƴմϴ�.
        const USER_INFO& userInfo = lpSessionData->GetUserInfo();

		// WORK_LIST 2.1 ���� ���� �߰�
        ERRLOG10(g_Log, "UID:%10u / CID:%10u / SessionID:%10u / ServerID:0x%08X / "
			"Char1:%10u / Char2:%10u / Char3:%10u / Char4:%10u / Char5:%10u / Nation:%d /"
            "ĳ���� �α��� ���� : �������� ���� ĳ���ͷ� �α����� �õ��Ͽ����ϴ�", 
            dwUID, dwCID, dwSessionID, dwServerID, 
            userInfo.CharID[0], userInfo.CharID[1], userInfo.CharID[2],
			userInfo.CharID[3], userInfo.CharID[4], userInfo.Nation);

        usCharLoginError = 5;        
    }
    else 
    {
		// edith 2008.01.15 UI�� Login ������Ŷ�� UpdateUIDTable �� �̷�����ִ� .���⿡ Pass �߰���.
        // edith 2008.01.15 UID������ ĳ���͸� �α��ν��ѵ� �Ǵ����� �����.        
        lpSessionData->PushRequest(
            RequestData(dwRequestKey, dwCID, dwServerID, dwSessionID, peerAddress, 
            lpSessionData->GetAccountName(), NULL));

        if(!SendPacket::UpdateUIDTable(PktUUT::UpdateUIDTableCharLogin,
            lpSessionData->GetAccountName(), NULL, dwUID, dwCID, dwSessionID, dwServerID, peerAddress))
        {
            ERRLOG4(g_Log, "UID:%10u / CID:%10u / SessionID:%10u / ServerID:0x%08X / "
                "ĳ���� �α��� ���� : UID������ UpdateUIDTableCharLogin ���� ����",
                dwUID, dwCID, dwSessionID, dwServerID);

            lpSessionData->PopRequest();
            usCharLoginError = 1;
        }
        else
        {
            INFLOG4(g_Log, "UID:%10u / CID:%10u / SessionID:%10u / ServerID:0x%08X / "
                "ĳ���� �α��� �õ� : UID������ UpdateUIDTableCharLogin ���� ����",
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

// ĳ���� �α׾ƿ�
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
        // ���� ���� ��� ����
        ERRLOG6(g_Log, "UID:%10u / CID:%10u / SessionID:%10u / ServerID:0x%08X / SessionData:0x%p / CharacterData:0x%p /"
            "ĳ���� �α׾ƿ� ���� : �����̳� ĳ���� �����Ͱ� �����ϴ�.",
            dwUID, dwCID, dwSessionID, dwServerID, lpSessionData, lpCharacterData);
    }
	else if(!lpSessionData->GetRealmPointCheck())
	{
	}
    else if(lpSessionData->GetSessionID() != dwSessionID || 
        lpSessionData->GetServerID() != dwServerID ||
        dwCID != lpSessionData->GetCID() || dwCID != lpCharacterData->GetCID())
    {
        // ����ID / ���� ID / CID�ٸ�
        ERRLOG8(g_Log, "UID:%10u / CID:%10u / SessionID:%10u / ServerID:0x%08X / "
            "SessionDataCID:%10u / CharacterDataCID:%10u / "
            "SessionDataSessionID:%10u / SessionDataServerID:0x%08X / "
            "ĳ���� �α׾ƿ� ���� : ����ID or ����ID or CID�ٸ�",
            dwUID, dwCID, dwSessionID, dwServerID, lpSessionData->GetCID(), lpCharacterData->GetCID(),
            lpSessionData->GetSessionID(), lpSessionData->GetServerID());
    }
    else if(CSessionData::SE_CHAR_ENABLED != lpSessionData->GetSessionState())
    {
        // ĳ���� ���°� Ȱ��ȭ ���°� �ƴմϴ�.
        ERRLOG4(g_Log, "UID:%10u / CID:%10u / SessionID:%10u / ServerID:0x%08X / "
            "ĳ���� �α׾ƿ� ���� : ĳ���� Ȱ��ȭ ���°� �ƴմϴ�.",
            dwUID, dwCID, dwSessionID, dwServerID);
    }
    else
    {
        // ĳ���� ������ ������Ʈ
        unsigned long dwDataLen = lpPktDBUpdate->GetLen() - sizeof(PktDBUpdate);

        if(lpPktDBUpdate->m_TypeCode == DBUpdateData::LOGOUT && 
            sizeof(PktDBUpdate) <= lpPktDBUpdate->GetLen())
        {
            if(!lpCharacterData->SerializeIn(reinterpret_cast<char*>(lpPktDBUpdate + 1), 
                lpPktDBUpdate->m_usUpdate, dwDataLen, DBUpdateData::MAX_UPDATE_DB))
            {
                ERRLOG4(g_Log, "UID:%10u / CID:%10u / SessionID:%10u / ServerID:0x%08X / "
                    "ĳ���� �α׾ƿ� �̻� : ĳ���� ������ ������Ʈ ����",
                    dwUID, dwCID, dwSessionID, dwServerID);                
            }
            else                
            {
                // ������ ������Ʈ ����. ��ŷ�� ������Ʈ�Ѵ�.
                const CHAR_INFOST& charInfo = lpCharacterData->GetInfo();								

                CRankingDBMgr::GetInstance().UpdateRanking(
                    RankingNode(charInfo.Name, charInfo.Fame, 
                    static_cast<unsigned char>(charInfo.Level), 
                    static_cast<unsigned char>(charInfo.Class)));
            }
        }

		lpCharacterData->SetAdminFlag(cAdminFlag);		

        // ��Ƽ�� �α׾ƿ� �ϸ�
        CPartyDBInfo* lpParty = CPartyDBMgr::GetInstance().OpenParty(
            CDBSingleObject::GetInstance(), lpCharacterData->GetPID());

        if (0 != lpParty)
        {
            CPartyDBMgr::GetInstance().LogoutPartyMember(
                CDBSingleObject::GetInstance(), lpParty, dwCID);
        }

        // ��忡 �α׾ƿ� �˸�
        CGuildDB* lpGuild = static_cast<CGuildDB*>(
            CGuildDBMgr::GetInstance().GetGuild(lpCharacterData->GetGID()));

        if (0 != lpGuild)
        {
            lpGuild->SendLoginOutMember(dwCID, 0);
        }
		
        // ĳ���� ��Ȱ��ȭ.
        if(!lpSessionData->CharDisable(dwCID, dwServerID))
        {
            ERRLOG4(g_Log, "UID:%10u / CID:%10u / SessionID:%10u / ServerID:0x%08X / "
                "ĳ���� �α׾ƿ� ���� : ĳ���� ��Ȱ��ȭ ����", dwUID, dwCID, dwSessionID, dwServerID);
        }
        else
        {
            INFLOG4(g_Log, "UID:%10u / CID:%10u / SessionID:%10u / ServerID:0x%08X / "
                "ĳ���� �α׾ƿ� ����", dwUID, dwCID, dwSessionID, dwServerID);

            // �α׾ƿ�������, ������ �ݴ´�.
            CSessionDataMgr::GetInstance().SessionClose(dwUID, dwSessionID);			

			if (GameRYL::CHINA == CServerSetup::GetInstance().GetNationType())
			{
				// LogOut ó���� ���ش�.
				DBComponent::GameDB::UpdatePlayTime(CDBSingleObject::GetInstance(), dwUID, 1);
			}
        }
    }

    // ������ �ø��� ������Ʈ
    if(itemSerialMgr.SetItemSerial(dwItemSerial))
    {
        // �ø����� �ٲ�����Ƿ� DB�� ������Ʈ�Ѵ�.
        if(!itemSerialMgr.SaveItemSerial(CDBSingleObject::GetInstance(), dwServerID))
        {   
            ERRLOG4(g_Log, "UID:%10u / CID:%10u / ServerID:0x%08X / Serial:0x%016I64X / ĳ���� �α׾ƿ� : ������ �ø��� ������Ʈ ����", 
                dwUID, dwCID, dwServerID, itemSerialMgr.GetItemSerial());
        }
    }

    return true;
}

// ĳ���� ���̵�
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
        // ���� ���� ��� ����
        ERRLOG6(g_Log, "UID:%10u / CID:%10u / SessionID:%10u / ServerID:0x%08X / SessionData:0x%p / CharacterData:0x%p "
            "ĳ���� ���̵� ���� : ���� �Ǵ� ĳ���� �����Ͱ� �����ϴ�", 
            dwUID, dwCID, dwSessionID, dwServerID, lpSessionData, lpCharacterData);
    }
    else if(lpSessionData->GetSessionID() != dwSessionID || 
        lpSessionData->GetServerID() != dwServerID ||
        dwCID != lpSessionData->GetCID() || dwCID != lpCharacterData->GetCID())
    {
        // ����ID / ���� ID / CID�ٸ�
        ERRLOG8(g_Log, "UID:%10u / CID:%10u / SessionID:%10u / ServerID:0x%08X / "
            "SessionDataCID:%10u / CharacterDataCID:%10u / "
            "SessionDataSessionID:%10u / SessionDataServerID:0x%08X / "
            "ĳ���� ���̵� ���� : ����ID or ����ID or CID�ٸ�",
            dwUID, dwCID, dwSessionID, dwServerID, lpSessionData->GetCID(), lpCharacterData->GetCID(),
            lpSessionData->GetSessionID(), lpSessionData->GetServerID());
    }
    else if(CSessionData::SE_CHAR_ENABLED != lpSessionData->GetSessionState())
    {
        // ĳ���� ���°� Ȱ��ȭ ���°� �ƴմϴ�.
        ERRLOG4(g_Log, "UID:%10u / CID:%10u / SessionID:%10u / ServerID:0x%08X / "
            "ĳ���� ���̵� ���� : ĳ���� Ȱ��ȭ ���°� �ƴմϴ�.",
            dwUID, dwCID, dwSessionID, dwServerID);
    }
    else
    {
        // ĳ���� ������ ������Ʈ
        unsigned long dwDataLen = lpPktDBUpdate->GetLen() - sizeof(PktDBUpdate);

        if(lpPktDBUpdate->m_TypeCode == DBUpdateData::ZONEMOVE && 
            sizeof(PktDBUpdate) <= lpPktDBUpdate->GetLen())
        {
            if(!lpCharacterData->SerializeIn(reinterpret_cast<char*>(lpPktDBUpdate + 1), 
                lpPktDBUpdate->m_usUpdate, dwDataLen, DBUpdateData::MAX_UPDATE_DB))
            {
                ERRLOG4(g_Log, "UID:%10u / CID:%10u / SessionID:%10u / ServerID:0x%08X / "
                    "ĳ���� ���̵� �̻� : ĳ���� ������ ������Ʈ ����",
                    dwUID, dwCID, dwSessionID, dwServerID);
            }
            else
            {
                // ������ ������Ʈ ����. ��ŷ�� ������Ʈ�Ѵ�.
                const CHAR_INFOST& charInfo = lpCharacterData->GetInfo();

                CRankingDBMgr::GetInstance().UpdateRanking(
                    RankingNode(charInfo.Name, charInfo.Fame, 
                    static_cast<unsigned char>(charInfo.Level), 
                    static_cast<unsigned char>(charInfo.Class)));
            }
        }

        // ��Ƽ�� �α׾ƿ� �ϸ�
        CPartyDBInfo* lpParty = CPartyDBMgr::GetInstance().OpenParty(
            CDBSingleObject::GetInstance(), lpCharacterData->GetPID());

        if (0 != lpParty)
        {
            CPartyDBMgr::GetInstance().LogoutPartyMember(
                CDBSingleObject::GetInstance(), lpParty, dwCID);
        }

        // ��忡 �α׾ƿ� �˸�
        CGuildDB* lpGuild = static_cast<CGuildDB*>(
            CGuildDBMgr::GetInstance().GetGuild(lpCharacterData->GetGID()));

        if (0 != lpGuild)
        {
            lpGuild->SendLoginOutMember(dwCID, 0);
        }

		// ĳ���� ��Ȱ��ȭ.
		if(!lpSessionData->CharMove(dwCID, dwServerID))
		{
			ERRLOG4(g_Log, "UID:%10u / CID:%10u / SessionID:%10u / ServerID:0x%08X / "
				"ĳ���� ���̵� ���� : ĳ���� ��Ȱ��ȭ ����", dwUID, dwCID, dwSessionID, dwServerID);
		}
		else
		{
			INFLOG4(g_Log, "UID:%10u / CID:%10u / SessionID:%10u / ServerID:0x%08X / "
				"ĳ���� ���̵� ����", dwUID, dwCID, dwSessionID, dwServerID);

			// ���̵��ÿ��� ������ ���� �ʴ´�.
			// CSessionDataMgr::GetInstance().SessionClose(dwUID, dwSessionID);

			if (GameRYL::CHINA == CServerSetup::GetInstance().GetNationType())
			{
				// LogOut ó���� ���ش�.
				DBComponent::GameDB::UpdatePlayTime(CDBSingleObject::GetInstance(), dwUID, 1);
			}
		}
    }

    // ������ �ø��� ������Ʈ
    if(itemSerialMgr.SetItemSerial(dwItemSerial))
    {
        // �ø����� �ٲ�����Ƿ� DB�� ������Ʈ�Ѵ�.
        if(!itemSerialMgr.SaveItemSerial(CDBSingleObject::GetInstance(), dwServerID))
        {   
            ERRLOG4(g_Log, "UID:%10u / CID:%10u / ServerID:0x%08X / Serial:0x%016I64X / ĳ���� ���̵� : ������ �ø��� ������Ʈ ����", 
                dwUID, dwCID, dwServerID, itemSerialMgr.GetItemSerial());
        }
    }

    return true;
}

// ĳ���� ������Ʈ
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
        // ���� ���� ��� ����
        ERRLOG6(g_Log, "UID:%10u / CID:%10u / SessionID:%10u / ServerID:0x%08X / SessionData:0x%p / CharacterData:0x%p /"
            "ĳ���� ������ ������Ʈ ���� : �����̳� ĳ���� �����Ͱ� �����ϴ�",
            dwUID, dwCID, dwSessionID, dwServerID, lpSessionData, lpCharacterData);
    }
    else if(lpSessionData->GetSessionID() != dwSessionID || 
        lpSessionData->GetServerID() != dwServerID ||
        dwCID != lpSessionData->GetCID() || dwCID != lpCharacterData->GetCID())
    {
        // ����ID / ���� ID / CID�ٸ�
        ERRLOG8(g_Log, "UID:%10u / CID:%10u / SessionID:%10u / ServerID:0x%08X / "
            "SessionDataCID:%10u / CharacterDataCID:%10u / "
            "SessionDataSessionID:%10u / SessionDataServerID:0x%08X / "
            "ĳ���� ������ ������Ʈ ���� : ����ID or ����ID or CID�ٸ�",
            dwUID, dwCID, dwSessionID, dwServerID, lpSessionData->GetCID(), lpCharacterData->GetCID(),
            lpSessionData->GetSessionID(), lpSessionData->GetServerID());
    }
    else if(CSessionData::SE_CHAR_ENABLED != lpSessionData->GetSessionState())
    {
        // ĳ���� ���°� Ȱ��ȭ ���°� �ƴմϴ�.
        ERRLOG4(g_Log, "UID:%10u / CID:%10u / SessionID:%10u / ServerID:0x%08X / "
            "ĳ���� ������ ������Ʈ ���� : ĳ���� Ȱ��ȭ ���°� �ƴմϴ�.",
            dwUID, dwCID, dwSessionID, dwServerID);
    }
    else
    {
        // ĳ���� ������ ������Ʈ
        unsigned long dwDataLen = lpPktDBUpdate->GetLen() - sizeof(PktDBUpdate);

        if(lpPktDBUpdate->m_TypeCode == DBUpdateData::UPDATE && 
            sizeof(PktDBUpdate) <= lpPktDBUpdate->GetLen())
        {
            if(!lpCharacterData->SerializeIn(reinterpret_cast<char*>(lpPktDBUpdate + 1), 
                lpPktDBUpdate->m_usUpdate, dwDataLen, DBUpdateData::MAX_UPDATE_DB))
            {
                ERRLOG4(g_Log, "UID:%10u / CID:%10u / SessionID:%10u / ServerID:0x%08X / "
                    "ĳ���� ������ ������Ʈ : ĳ���� ������ ������Ʈ ����",
                    dwUID, dwCID, dwSessionID, dwServerID);
            }
            else
            {
                // ������ ������Ʈ ����. ��ŷ�� ������Ʈ�Ѵ�.
                const CHAR_INFOST& charInfo = lpCharacterData->GetInfo();

                CRankingDBMgr::GetInstance().UpdateRanking(
                    RankingNode(charInfo.Name, charInfo.Fame, 
                    static_cast<unsigned char>(charInfo.Level), 
                    static_cast<unsigned char>(charInfo.Class)));
            }
        }
    }

    // ������ �ø��� ������Ʈ
    if(itemSerialMgr.SetItemSerial(dwItemSerial))
    {
        // �ø����� �ٲ�����Ƿ� DB�� ������Ʈ�Ѵ�.
        if(!itemSerialMgr.SaveItemSerial(CDBSingleObject::GetInstance(), dwServerID))
        {   
            ERRLOG4(g_Log, "UID:%10u / CID:%10u / ServerID:0x%08X / Serial:0x%016I64X / ĳ���� ������Ʈ : ������ �ø��� ������Ʈ ����", 
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

    // ���� DB���� ���� Ȯ��
    using namespace DataStorage;
    CSessionData*   lpSessionData = CSessionDataMgr::GetInstance().GetOpenSession(dwUID);
    CStoreData*     lpStoreData = 0;

    if(0 == lpSessionData || 0 == (lpStoreData = lpSessionData->GetStoreData()))
    {
        ERRLOG6(g_Log, "UID:%10u / Cmd:%u / Password:%s / Gold:%10u / SessionData:0x%p / StoreData:0x%p / "
            "â�� ���� ������Ʈ ���� : �����̳� â�� �����Ͱ� �����ϴ�", 
            dwUID, cCmd, szPassword, dwGold, lpSessionData, lpStoreData);
    }
    else if(CSessionData::SE_CHAR_ENABLED != lpSessionData->GetSessionState())
    {
        ERRLOG4(g_Log, "UID:%10u / Cmd:%u / Password:%s / Gold:%10u / â�� ���� ������Ʈ ���� : ĳ���Ͱ� Ȱ��ȭ�Ǿ� ���� �ʽ��ϴ�", 
            dwUID, cCmd, szPassword, dwGold);
    }
    else if(dwUID != lpSessionData->GetUID() || dwUID != lpStoreData->GetUID())
    {
        ERRLOG6(g_Log, "UID:%10u / Cmd:%u / SessionUID:%10u / StoreUID:%10u / Password:%s / Gold:%10u / "
            "â�� ���� ������Ʈ ���� : â�� UID�� ����UID�� �ٸ��ϴ�", 
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

    // ���� DB���� ���� Ȯ��
    using namespace DataStorage;
    CSessionData*   lpSessionData = CSessionDataMgr::GetInstance().GetOpenSession(dwUID);
    CStoreData*     lpStoreData = 0;

    if(0 == lpSessionData || 0 == (lpStoreData = lpSessionData->GetStoreData()))
    {
        ERRLOG4(g_Log, "UID:%10u / CID:%10u / SessionData:0x%p / StoreData:0x%p / â�� ���� ������Ʈ ���� : ������ ���ų� â�� �����Ͱ� �����ϴ�.", 
            dwUID, dwCID, lpSessionData, lpStoreData);
    }
    else if(CSessionData::SE_CHAR_ENABLED != lpSessionData->GetSessionState())
    {
        ERRLOG2(g_Log, "UID:%10u / CID:%10u / â�� ���� ������Ʈ ���� : ĳ���Ͱ� Ȱ��ȭ�Ǿ� ���� �ʽ��ϴ�", 
            dwUID, dwCID);
    }
    else if(dwUID != lpSessionData->GetUID() || dwUID != lpStoreData->GetUID())
    {
        ERRLOG4(g_Log, "UID:%10u / CID:%10u / SessionUID:%10u / StoreUID:%10u / "
            "â�� ���� ������Ʈ ���� : â�� UID�� ����UID�� �ٸ��ϴ�", 
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
	
	unsigned long	dwGID		= lpPktFriendDB->m_dwGID;				// ��� ����.
	unsigned short	wClass		= lpPktFriendDB->m_wClass;				// Ŭ���� ����.
	char			cLevel		= lpPktFriendDB->m_cLevel;				// ���� ����.
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
        // ���� ���ų� ������ ����
        ERRLOG5(g_Log, "UID:%10u / CID:%10u / RefCID:%10u / SessionData:0x%p / CharacterData:0x%p / "
            "ģ�� ��Ŷ ó�� ���� : ������ ���ų�, ���ǿ� ĳ���� �����Ͱ� �����ϴ�.", 
            dwUID, dwCID, dwRefCID, lpSessionData, lpCharacterData);
    }
    else if(CSessionData::SE_CHAR_ENABLED != lpSessionData->GetSessionState())
    {
        // ���� ���� �̻�
        ERRLOG4(g_Log, "UID:%10u / CID:%10u / RefCID:%10u / ST:%s / "
            "ģ�� ��Ŷ ó�� ���� : ������ Ȱ��ȭ���� �ʾҽ��ϴ�", 
            dwUID, dwCID, dwRefCID, g_szSessionStateString[lpSessionData->GetSessionState()]);
    }
    else if(dwCID != lpCharacterData->GetCID() || dwCID != lpSessionData->GetCID())
    {
        // CID�̻�
        ERRLOG5(g_Log, "UID:%10u / CID:%10u / RefCID:%10u / SessionDataCID:%10u / CharacterDataCID:%10u / "
            "ģ�� ��Ŷ ó�� ���� : ���� �Ǵ� ĳ������ CID�� �̻��մϴ�", 
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
                    "RefSessionDataCID:%10u / RefCharacterDataCID:%10u / ģ�� ��Ŷ ó�� ���� : ģ�� �߰� ���� - ģ���� �α������� �ʾҽ��ϴ�",
                    dwUID, dwCID, dwRefCID, lpRefSessionData, lpRefCharacterData,
                    dwRefCharacterDataCID, dwRefSessionDataCID);
            }
            else if(!lpCharacterData->AddFriend(dwRefCID, lpRefCharacterData->GetName(), dwGID, wClass, cLevel, dwServerID))
            {
                ERRLOG3(g_Log, "UID:%10u / CID:%10u / RefCID:%10u / ģ�� ��Ŷ ó�� ���� : ģ�� �߰� ����",
                    dwUID, dwCID, dwRefCID);
            }
            break;

        case PktFriendDB::REMOVE_FRIEND:
    
            if(!lpCharacterData->RemoveFriend(dwRefCID))
            {
                ERRLOG3(g_Log, "UID:%10u / CID:%10u / RefCID:%10u / ģ�� ��Ŷ ó�� ���� : ģ�� ���� ����",
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
                    "RefSessionDataCID:%10u / RefCharacterDataCID:%10u / ģ�� ��Ŷ ó�� ���� : �ź� �߰� ���� - �ź� ����ڰ� �α������� �ʾҽ��ϴ�",
                    dwUID, dwCID, dwRefCID, lpRefSessionData, lpRefCharacterData,
                    dwRefCharacterDataCID, dwRefSessionDataCID);
            }
            else if(!lpCharacterData->AddBan(dwRefCID, lpRefCharacterData->GetName(), lpRefCharacterData->GetGID(),
				lpRefCharacterData->GetClass(), lpRefCharacterData->GetLevel(), dwServerID))
            {
                ERRLOG3(g_Log, "UID:%10u / CID:%10u / RefCID:%10u / ģ�� ��Ŷ ó�� ���� : �ź� �߰� ����",
                    dwUID, dwCID, dwRefCID);
            }
            break;

        case PktFriendDB::REMOVE_BAN:

            if(!lpCharacterData->RemoveBan(dwRefCID))
            {
                ERRLOG3(g_Log, "UID:%10u / CID:%10u / RefCID:%10u / ģ�� ��Ŷ ó�� ���� : �ź� ���� ����",
                    dwUID, dwCID, dwRefCID);
            }
            break;

        case PktFriendDB::SETGROUP:

            if(!lpCharacterData->SetFriendGroup(dwRefCID, dwData))
            {
                ERRLOG4(g_Log, "UID:%10u / CID:%10u / RefCID:%10u / dwData:0x%08X / ģ�� ��Ŷ ó�� ���� : �׷� ���� ����",
                    dwUID, dwCID, dwRefCID, dwData);
            }
            break;		
		case PktFriendDB::FRIEND_INFO:
			{
				// ģ�� ���� ������Ʈ //				
				CFriendList friendList = lpCharacterData->GetFriendList();

				CFriendList::Rebind* lpRebind = friendList.GetFriend(dwRefCID);

				if(lpRebind)
				{
					// ������Ʈ //
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
				// �ź� ���� ������Ʈ //
				CBanList banList = lpCharacterData->GetBanList();
				
				CBanList::Rebind* lpRebind = banList.GetBan(dwRefCID);

				if(lpRebind)
				{
					// ������Ʈ //
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
        // ���� ���ų� ������ ����
        ERRLOG4(g_Log, "UID:%10u / CID:%10u / SessionData:0x%p / CharacterData:0x%p / "
            "����Ʈ ��Ŷ ó�� ���� : ������ ���ų�, ���ǿ� ĳ���� �����Ͱ� �����ϴ�.", 
            dwUID, dwCID, lpSessionData, lpCharacterData);
    }
    else if(CSessionData::SE_CHAR_ENABLED != lpSessionData->GetSessionState())
    {
        // ���� ���� �̻�
        ERRLOG3(g_Log, "UID:%10u / CID:%10u / ST:%s / "
            "����Ʈ ��Ŷ ó�� ���� : ������ Ȱ��ȭ���� �ʾҽ��ϴ�", 
            dwUID, dwCID, g_szSessionStateString[lpSessionData->GetSessionState()]);
    }
    else if(dwCID != lpCharacterData->GetCID() || dwCID != lpSessionData->GetCID())
    {
        // CID�̻�
        ERRLOG4(g_Log, "UID:%10u / CID:%10u / SessionDataCID:%10u / CharacterDataCID:%10u / "
            "����Ʈ ��Ŷ ó�� ���� : ���� �Ǵ� ĳ������ CID�� �̻��մϴ�", 
            dwUID, dwCID, lpSessionData->GetCID(), lpCharacterData->GetCID());
    }
    else
    {
        QUEST   quest;
        memset(&quest, 0, sizeof(QUEST));
        quest.dwSize = dwExecuteQuestSize;
        memcpy(quest.Data, lpPktQuestDB + 1, dwExecuteQuestSize);

		SERLOG5(g_Log, "UID:%10u / CID:%10u / SessionDataCID:%10u / CharacterDataCID:%10u / "
			"����Ʈ ��Ŷ ó�� : Quest Size : %d", dwUID, dwCID, lpSessionData->GetCID(), lpCharacterData->GetCID(), dwExecuteQuestSize);

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
        // ���� ���ų� ������ ����
        ERRLOG4(g_Log, "UID:%10u / CID:%10u / SessionData:%0xp / CharacterData:%0xp / "
            "���� ��Ŷ ó�� ���� : ������ ���ų�, ���ǿ� ĳ���� �����Ͱ� �����ϴ�.", 
            dwUID, dwCID, lpSessionData, lpCharacterData);
    }
    else if(CSessionData::SE_CHAR_ENABLED != lpSessionData->GetSessionState())
    {
        // ���� ���� �̻�
        ERRLOG3(g_Log, "UID:%10u / CID:%10u / ST:%s / "
            "���� ��Ŷ ó�� ���� : ������ Ȱ��ȭ���� �ʾҽ��ϴ�", 
            dwUID, dwCID, g_szSessionStateString[lpSessionData->GetSessionState()]);
    }
    else if(dwCID != lpCharacterData->GetCID() || dwCID != lpSessionData->GetCID())
    {
        // CID�̻�
        ERRLOG4(g_Log, "UID:%10u / CID:%10u / SessionDataCID:%10u / CharacterDataCID:%10u / "
            "���� ��Ŷ ó�� ���� : ���� �Ǵ� ĳ������ CID�� �̻��մϴ�", 
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

	/// ���������ν��� �߰��� (2009.05.09 �� ������������ ���������ν����� ������)
	// ���⼭ DB�� ������ �����ش�. 1�ð� ����ݾ�
	if(DBComponent::GameDB::AddPremiumService(CDBSingleObject::GetInstance(), lpSessionData->GetUID(), 1, lpPktUI->m_cRemainItemNum))
	{
		// ��� ������
		SendPacket::CharUseCashItem(SendStream, lpPktUI->m_dwSender, lpPktUI->m_dwRecver,
			lpPktUI->m_itemPos, lpPktUI->m_cRemainItemNum, 0);
		return true;
	}
	// ��� ������
    SendPacket::CharUseCashItem(SendStream, lpPktUI->m_dwSender, lpPktUI->m_dwRecver,
		lpPktUI->m_itemPos, lpPktUI->m_cRemainItemNum, PktUI::USE_FAILED);

	return false ;
}

bool ExtraEvent(CSendStream& SendStream, PktBase* lpPktBase)
{
	// edith 2009.09.05 �ű� �̺�Ʈ ���ۺκ�
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

			// ��� day�� ����
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