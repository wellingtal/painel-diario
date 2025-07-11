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

// WORK_LIST 2.3 ���� ���� ���� ��� ����
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
// WORK_LIST 2.1 ���� ���� �߰�
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
	case PktDD::SCmdStartSession:	// ���� ����
		{
			PktSSD* lpPktSSD = static_cast<PktSSD*>(lpPktDD);
			dwSessionID = lpPktSSD->m_dwSessionID;
			dwUID = lpPktSSD->m_dwUserID;
		}
		break;

	case PktDD::SCmdUserLogin:      // ���� �α��� - �ϴ� ���� ĳ���� �� ��� �����̴�.
		{
			PktULD* lpPktULD = static_cast<PktULD*>(lpPktDD);
			dwSessionID = lpPktULD->m_dwSessionID;
			dwUID = lpPktULD->m_dwUserID;
		}
		break;

	case PktDD::SCmdUserLogout:		// ���� �α׾ƿ� - ���� �ݱ�.
		{
			PktULoD* lpPktULoD = static_cast<PktULoD*>(lpPktDD);
			dwSessionID = lpPktULoD->m_dwSessionID;
			dwUID = lpPktULoD->m_dwUserID;
		}
		break;

	case PktDD::SCmdUserMove:		// ���� �̵�
		{
			PktUMvD* lpPktUMvD = static_cast<PktUMvD*>(lpPktDD);
			dwSessionID = lpPktUMvD->m_dwSessionID;
			dwUID = lpPktUMvD->m_dwUserID;
		}
		break;

		// WORK_LIST 2.1 ���� ���� �߰�
	case PktDD::SCmdSelectNation:	// ���� ���� ���� �� ����
		{
			PktSAND* lpPktSAND = static_cast<PktSAND*>(lpPktDD);
			dwUID = lpPktSAND->m_dwUserID;
		}
		break;

	case PktDD::SCmdCharSelect:		// ĳ���� ����
		{
			PktCSD* lpPktCSD = static_cast<PktCSD*>(lpPktDD);
			dwUID = lpPktCSD->m_dwUserID;
			dwCID = lpPktCSD->m_dwCharID;
		}
		break;

	case PktDD::SCmdCharCreate:		// ĳ���� ����
		{
			PktCCD* lpPktCCD = static_cast<PktCCD*>(lpPktDD);
			dwUID = lpPktCCD->m_dwUserID;
		}
		break;

	case PktDD::SCmdCharDelete:		// ĳ���� ����
		{
			PktCDD* lpPktCCD = static_cast<PktCDD*>(lpPktDD);
			dwUID = lpPktCCD->m_dwUserID;
			dwCID = lpPktCCD->m_dwCharID;
		}
		break;

	case PktDD::SCmdCharCreateItem:	// ĳ���� ���� ������
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


	// ����� ĳ���� ����,���� ȭ�鿡��
	// ���ӽ��� �ϱ� ������ �ϴ°��ε�.
    bool bResult = false;
    
    switch (lpPktDD->m_wCmd) 
    {
    case PktDD::SCmdStartSession:	// ���� ����
//      SERLOG0(g_Log, "���ǽ���");
        bResult = StartSession(SendStream, dwServerID, static_cast<PktSSD*>(lpPktDD));
        SERLOG0(g_Log, "���ǽ��� �Ϸ�");
        break;

    case PktDD::SCmdUserLogin:      // ���� �α��� - �ϴ� ���� ĳ���� �� ��� �����̴�.
//        SERLOG0(g_Log, "�����α��� �õ�");
        bResult = UserLogin(SendStream, static_cast<PktULD*>(lpPktDD));
        SERLOG0(g_Log, "�����α��� �Ϸ�");
        break;

    case PktDD::SCmdUserLogout:		// ���� �α׾ƿ� - ���� �ݱ�.
//        SERLOG0(g_Log, "�����α׾ƿ� �õ�");
        bResult = UserLogout(SendStream, static_cast<PktULoD*>(lpPktDD));
        SERLOG0(g_Log, "�����α׾ƿ� �Ϸ�");
        break;
    
    case PktDD::SCmdUserMove:		// ���� �̵�
        bResult = UserMove(SendStream, static_cast<PktUMvD*>(lpPktDD));
        break;

	// WORK_LIST 2.1 ���� ���� �߰�
	case PktDD::SCmdSelectNation:	// ���� ���� ���� �� ����
		bResult = SelectAccountNation(SendStream, static_cast<PktSAND*>(lpPktDD));
		break;

    case PktDD::SCmdCharSelect:		// ĳ���� ����
        bResult = CharSelect(SendStream, static_cast<PktCSD*>(lpPktDD));
        break;

    case PktDD::SCmdCharCreate:		// ĳ���� ����
        bResult = CharCreate(SendStream, static_cast<PktCCD*>(lpPktDD));
        break;

    case PktDD::SCmdCharDelete:		// ĳ���� ����
        bResult = CharDelete(SendStream, static_cast<PktCDD*>(lpPktDD));
        break;

    case PktDD::SCmdCharCreateItem:	// ĳ���� ���� ������
        bResult = CreateItem(SendStream, dwServerID, itemSerialMgr, static_cast<PktCCID*>(lpPktDD));
        break;

    default:        
        ERRLOG1(g_Log, "CharacterManage ��Ŷ ó�� ���� : �� �� ���� ��Ŷ CMD:0x%02X�� �޾ҽ��ϴ�.", 
            lpPktDD->m_wCmd);

        bResult = true;
        break;
    }

    return bResult;
}


bool StartSession(CSendStream& SendStream, unsigned long dwServerID, PktSSD* lpPktSSD)
{
	// edith 2008.01.15 Auth ���� ���� ������Ŷ�� ����Դ�.
    unsigned long dwSessionID	= lpPktSSD->m_dwSessionID;
    unsigned long dwUID	        = lpPktSSD->m_dwUserID;
    unsigned long dwRequestKey  = lpPktSSD->m_dwRequestKey;

    IN_ADDR	peerAddress         = lpPktSSD->m_Address;	
    const char* szAccountName   = lpPktSSD->m_AccountName;
	const char* szPassword		= lpPktSSD->m_Password;

    unsigned short usFlag  = lpPktSSD->GetError();	// ���� ����.
    unsigned short usStartSessionError = 0;

    using namespace DataStorage;

    // ���� ����
    CSessionData* lpSessionData = CSessionDataMgr::GetInstance().GetOpenSession(dwUID);
    
    if(0 != lpSessionData && 
        (CSessionData::SE_USER_ENABLED == lpSessionData->GetSessionState() ||
         CSessionData::SE_CHAR_ENABLED == lpSessionData->GetSessionState()))
    {
        // �̹� �α��� �� �����Ƿ� ���� ���� ��ƾ ����
        if(0 == usFlag)
        {
            // ���� ���� ������� ��û
            usStartSessionError = 41;
        }
        else if(1 == usFlag)
        {
            // ���� ���������� ���� ���� ó���� �Ѵ�.
            DETLOG5(g_Log, "UID:%10u / CID:%10u / ServerID:0x%08X / Account:%s / IP:%15s / ���� ���� ���� : ���� ���� ������� ���� ���� ����.",
                lpSessionData->GetUID(), lpSessionData->GetCID(),
                lpSessionData->GetServerID(), lpSessionData->GetAccountName(), inet_ntoa(peerAddress));

            SendPacket::UserKill(*lpSessionData);

            // ������ �õ� ��û
            usStartSessionError = 42;
        }
        else
        {
            // �� �� ���� ���. �׳� ���� ���� ���з� ����.
            ERRLOG4(g_Log, "UID:%10u / CID:%10u / ServerID:0x%08X / usFlag:%d / ���� ���� ���� : Ŭ���̾�Ʈ�� �� �� ���� �÷��׸� ���½��ϴ�.",
                lpSessionData->GetUID(), lpSessionData->GetCID(), lpSessionData->GetServerID(), usFlag);

            usStartSessionError = 40;
        }
    }
    else if(0 == (lpSessionData = CSessionDataMgr::GetInstance().SessionOpen(dwUID)))
    {
        // ���� ���� ����
        usStartSessionError = 40;
    }
    else
    {

#ifdef AUTH_MY
		// edith 2009.09.11 MY�� ���� AllowIP ó���۾�
		char* szAdd = inet_ntoa(peerAddress);
		if(!g_IPSec.IsAliveIP(szAdd))
		{
			usStartSessionError = 57;

			ERRLOG4(g_Log, "DBAgent Error. IPSEC Error:%d Account:%s UID:%d (%s)", usStartSessionError, lpSessionData->GetAccountName(), lpSessionData->GetUID(), szAdd);
		}
#endif

		if(usStartSessionError == 0)
		{
			SERLOG4(g_Log, "UID:%10u / CID:%10u / ServerID:0x%08X / %s / UID�� ���� �õ�", 
				lpSessionData->GetUID(), lpSessionData->GetCID(), lpSessionData->GetServerID(), szAccountName);

			// ���������� ������ ID�����Ѵ�.
			SERVER_ID serverID;

			serverID.sID.ID         = 0;
			serverID.sID.Channel    = 0;
			serverID.sID.Group      = CServerSetup::GetInstance().GetServerGroup();
			serverID.sID.Type       = CServerSetup::AuthServer;

			dwServerID = serverID.dwID;

			// edith 2008.01.15 ������ ���⿡ �����ߴ�. ������Ʈ ����Ÿ�� ���ǿ� �Է��Ѵ�. ������ UID�� �α��νõ�.
			// �н����带 �����Ϸ��� ������� ���ƿ;��Ѵ�.
			// ���� ���� ����
			lpSessionData->PushRequest(
				RequestData(dwRequestKey, 0, dwServerID, dwSessionID, peerAddress, szAccountName, szPassword));
	        
			// edith 2008.01.15 UID������ ���� �α����� �����Ѵ�.
			if (!SendPacket::UpdateUIDTable(PktUUT::UpdateUIDTableUserLogin, 
				szAccountName, szPassword, dwUID, 0, dwSessionID, dwServerID, peerAddress,
				(1 == usFlag) ? PktUUT::DISCONNECT_USER : 0))
			{
				ERRLOG1(g_Log, "UID:%10u / UID������ ���� �α��� ���� ����", dwUID);

				lpSessionData->PopRequest();
				usStartSessionError = 1;
			}
			// ���߿� ����
	//		INFLOG5(g_Log, "UID:%10u / Account:%s, Pass:%s, Session:0x%08X, ServerID:0x%08X ", 
	//            dwUID, szAccountName, szPassword, dwSessionID, dwServerID);
		}
    }

    if(0 != usStartSessionError)
    {
        if(!SendPacket::StartSession(SendStream, dwRequestKey, 
            dwUID, dwSessionID, 0, usStartSessionError))
        {
            ERRLOG2(g_Log, "UID:%10u / ���������� ���� ���� ���� ���� ���� ���� : %u",
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
	// WORK_LIST 2.1 ���� ���� �߰�
	unsigned char	cAccountNation	= 0;

	sGuildData	GuildData[PktULDAck::MAX_CHAR_VIEW];
	memset(&GuildData[0], 0, sizeof(sGuildData) * PktULDAck::MAX_CHAR_VIEW);

	STORE_INFO storeInfo;
	memset(&storeInfo, 0, sizeof(STORE_INFO));

    using namespace DataStorage;

    CSessionData* lpSessionData = CSessionDataMgr::GetInstance().GetOpenSession(dwUID);

    // ���� ���
    if(0 == lpSessionData)
    {
        ERRLOG1(g_Log, "UID:%10u / ���� �α��� ���� : ������ �����ϴ�", dwUID);

        usError = 1;
    }
    // ���� ID�˻�
    else if(dwSessionID != lpSessionData->GetSessionID())
    {
        ERRLOG3(g_Log, "UID:%10u / SessionID:%10u / ReceivedSessionID:%10u / "
            "���� �α��� ���� : - ����ID�� �ٸ��ϴ�", 
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

            // ���� �� ����. �̰� ��Ʈ 2������!
            usError = 1;
            SERLOG0(g_Log, "Part2 ������ Part1�������� �ѷ��� �ϼ̽��ϴ�. ������ �ٽ� �� �ֽʽÿ�");
            break;

        case UnifiedConst::ROW:
        case UnifiedConst::Part2Unified:

            // Part2 / Part2 ���ռ�����, �о�� UserInfo�� �ϳ����̴�.
            lpSessionData->SetOldServerGroupID(cAgentServerType);
            break;

        case UnifiedConst::Part2Selectable:
            // ������ �� �ϰ� �׳� UserLogin�� ������, �ű�/�����ΰ��� ����
            // UnifiedConst::Part2Selectable �� Ȥ�� 1������ �����͸� �о�´�.
            break;
        }

        // ���� �α���
        if(0 == usError)
        {
            if (!(lpSessionData->GetFirstLogin() & UnifiedConst::SELECTED_PART2))
            {
/*
                ERRLOG2(g_Log, "UID:%10u / FirstLogin:%d / "
                    "UserLogin ���� �����ߴµ�, Part2 ���� â�� ������ �� ���� ��ó�� �Ǿ� ����. ���� ������.",
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

            // WORK_LIST 2.1 ���� ���� �߰�
		    cAccountNation	= lpSessionData->GetAccountNation();

		    // ��� �� ���� ���� ����
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
					    // WORK_LIST 2.1 ���� ���� �߰�
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

			// �α����Ѵ�. 
			// PlayTime�� Login ���·� �ٲ۴�.
			using namespace DBComponent;

			GameDB::GetUnifiedItemStoreInfo(CDBSingleObject::GetInstance(), dwUID, 
				lpSessionData->GetOldServerGroupID(), &storeInfo, true);

			KeyInfo* pKeyInfo = lpSessionData->GetKeyInfo();
			memset(pKeyInfo, 0, sizeof(KeyInfo));
			
			//GameDB::GetKeyInfo(CDBSingleObject::GetInstance(), pKeyInfo, dwUID);

            lpSessionData->LogUserData("ĳ���� �� ��� ����");			
        }
    }

	// WORK_LIST 2.1 ���� ���� �߰�
    if(!SendPacket::UserLogin(SendStream, dwRequestKey, dwUID, 
        lpCharView, GuildData, &storeInfo, dwCharViewNum, cAccountNation, nPlayTime, cAdminLevel, 
        cBillingUser, cBillingType, cLoginType, usError))
    {
        ERRLOG1(g_Log, "UID:%10u / ĳ���� �� ��� ���� : ĳ���� �� ���� ����", dwUID);
        return false;
    }

    return true;
}


bool UserLogout(CSendStream& SendStream, PktULoD* lpPktULoD)
{
    unsigned long dwSessionID	= lpPktULoD->m_dwSessionID;
    unsigned long dwUID	        = lpPktULoD->m_dwUserID;

    // ���� DB���� ���� �˻�
    using namespace DataStorage;

    CSessionData* lpSessionData = CSessionDataMgr::GetInstance().GetOpenSession(dwUID);

    // ���� ���
    if(0 == lpSessionData)
    {
        ERRLOG1(g_Log, "UID:%10u / ���� �α׾ƿ� ���� : ������ �����ϴ�", dwUID);
    }
    // ���� ID�˻�
    else if(dwSessionID != lpSessionData->GetSessionID())
    {
        ERRLOG3(g_Log, "UID:%10u / SessionID:%10u / RSessionID:%10u / ���� �α׾ƿ� ���� : ����ID�� �ٸ��ϴ�",
            dwUID, lpSessionData->GetSessionID(), dwSessionID);
    }		
    else if(!lpSessionData->UserDisable())
    {
        ERRLOG1(g_Log, "UID:%10u / ���� �α׾ƿ� ���� : ���� ��Ȱ��ȭ ����", dwUID);
    }
    else
    {
		// ����Ÿ���� ������Ʈ�Ѵ�.
		// �α׾ƿ��Ѵ�. 
		// PlayTime�� Logout ���·� �ٲ۴�.
        INFLOG1(g_Log, "UID:%10u / ���� �α׾ƿ� ���� : ������ ��Ȱ��ȭ�߽��ϴ�.", dwUID);
        return true;
    }

    return false;
}

bool UserMove(CSendStream& SendStream, PktUMvD* lpPktUMvD)
{
    unsigned long dwSessionID	= lpPktUMvD->m_dwSessionID;
    unsigned long dwUID	        = lpPktUMvD->m_dwUserID;

    // ���� DB���� ���� �˻�
    using namespace DataStorage;

    CSessionData* lpSessionData = CSessionDataMgr::GetInstance().GetOpenSession(dwUID);

    // ���� ���
    if(0 == lpSessionData)
    {
        ERRLOG1(g_Log, "UID:%10u / ���� �̵�(ĳ���� ����) ���� : ������ �����ϴ�", dwUID);
    }
    // ���� ID�˻�
    else if(dwSessionID != lpSessionData->GetSessionID())
    {
        ERRLOG3(g_Log, "UID:%10u / SessionID:%10u / ReceivedSessionID:%10u / "
            "���� �̵�(ĳ���� ����) ���� : ����ID�� �ٸ��ϴ�",
            dwUID, lpSessionData->GetSessionID(), dwSessionID);
    }
    else if(!lpSessionData->UserMove())
    {
        ERRLOG1(g_Log, "UID:%10u / ���� �̵�(ĳ���� ����) ���� : ���� �̵� ����", dwUID);
    }
    else
    {
        return true;
    }

    return false;
}

// WORK_LIST 2.1 ���� ���� �߰�
bool SelectAccountNation(CSendStream& SendStream, PktSAND* lpPktSAND)
{
	unsigned long dwUID				= lpPktSAND->m_dwUserID;
	unsigned char cType				= lpPktSAND->m_cType;
	unsigned char cAccountNation	= lpPktSAND->m_cAccountNation;
	unsigned long dwRequestKey		= lpPktSAND->m_dwRequestKey;

	unsigned short usError = 0;

	using namespace DataStorage;

	CSessionData* lpSessionData = CSessionDataMgr::GetInstance().GetOpenSession(dwUID);

	// ���� ���
	if(0 == lpSessionData)
	{
		ERRLOG1(g_Log, "UID:%10u / ���� ���� ���� �� ���� ���� : ������ �����ϴ�", dwUID);
		usError = 4;
	}
	else
	{
        unsigned char cOldServerGroupID = lpSessionData->GetOldServerGroupID();

		// ���� ���� ����
		if ( PktSAND::TYPE_SET == cType )
		{
			if (!DBComponent::GameDB::UpdateAccountNation( 
                CDBSingleObject::GetInstance(), dwUID, cAccountNation, cOldServerGroupID))
			{
                ERRLOG3(g_Log, "UID:%10u / AccountNation:%d / ServerGroupID:%d / ���� ���� ���� ���� : UpdateAccountNation DB����",
					dwUID, cAccountNation, cOldServerGroupID);

				usError = 2;
			}
		}
		// WORK_LIST 2.3 ���� ���� ���� ��� ����
		// ���� ���� ����
		else if ( PktSAND::TYPE_CHANGE == cType )
		{
			if ( lpSessionData->GetAccountNation() != cAccountNation )
			{
				if ( !DBComponent::GameDB::UpdateAccountNation( CDBSingleObject::GetInstance(), 
                    dwUID, cAccountNation, cOldServerGroupID ) )
				{
                    ERRLOG3(g_Log, "UID:%10u / AccountNation:%d / ServerGroupID:%d / ���� ���� ���� ���� : UpdateAccountNation DB����",
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
								// ���� 5000(�̱��� 1000) �̻��̾��ٸ�, ��ġ�� �г�Ƽ ����
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

									// ��� �������� ��� ������ �絵�� �� Ż���Ų��.
									if (pCharView->CID == lpGuild->GetMaster().m_dwCID)
									{
										unsigned long dwNewMaster = lpGuild->SetNewMaster();
										if (0 != dwNewMaster)
										{
											SendPacket::GuildCmd(&SendStream, lpGuild->GetGID(), dwNewMaster, MASTER, 
												PktGuildCmd::GC_SETUP_TITLE, NULL, NULL, PktBase::NO_SERVER_ERR);

											DETLOG3(g_Log, "GID:%10u ��帶����(����:%10u, ����:%10u)�� ����Ǿ����ϴ�. - ��帶���� ���� ����", 
												lpGuild->GetGID(), pCharView->CID, dwNewMaster);
										}
										else
										{
											DETLOG1(g_Log, "GID:%10u ��尡 ��ü�Ǿ����ϴ�. - ��帶���� ���� ����(�絵�� ��� ����)", lpGuild->GetGID());
											CGuildDBMgr::GetInstance().DissolveGuild(lpGuild->GetGID(), 
												PktCreateGuild::NONE_NEXT_GUILDMASTER_BY_GUILDMASTER_OUT);

											bLeaveGuild = false;
										}
									}

									if (true == bLeaveGuild)
									{
										// ��� ������ ����� ���� ������ �ٸ��Ƿ�, Ż�� ó�� ��Ų��.
										if ( lpGuild->LeaveMember( pCharView->CID ) )
										{
											// ��� Ż�� ������ ��� ���� ������ ����
											SendPacket::GuildCmd( NULL, pCharView->GID, pCharView->CID, 0, 
												PktGuildCmd::GC_KICK, NULL, NULL, PktBase::NO_SERVER_ERR );
										}
										else
										{
											// ��� Ż�� ����
											ERRLOG3(g_Log, "UID:%10u / CID:%10u / GID:%10u ��� ���� Ż�� ���� : ���� ���� �������� ���� Ż�� ����",
												dwUID, pCharView->CID, pCharView->GID);
										}
									}
								}
								else
								{
									// ������ �Ѹ���̹Ƿ�, ��带 ��ü�Ѵ�.
									DETLOG1(g_Log, "GID:%10u ��尡 ��ü�Ǿ����ϴ�. - ��帶���� ���� ����(�渶���� ���)", lpGuild->GetGID());
									CGuildDBMgr::GetInstance().DissolveGuild(
										lpGuild->GetGID(), PktCreateGuild::NONE_NEXT_GUILDMASTER_BY_GUILDMASTER_OUT);
								}

								lpSessionData->SetCharViewGID( pCharView->CID, 0 );
							}
							else
							{
								// ���� 5000(�̱��� 1000) �̻��̾��ٸ�, ��ġ�� �г�Ƽ ����
								const unsigned short wFamePaneltyValue1st = CSessionData::FAME_PANELTY_VALUE_1ST;

								if ( pCharView->Fame > wFamePaneltyValue1st )
								{
									CCharacterData* lpCharacterData = 0;
									CSessionData*	lpCharSessionData = 
										CSessionDataMgr::GetInstance().GetCharLoadedSession( pCharView->CID );

									if ( 0 == lpCharSessionData || 0 == (lpCharacterData = lpCharSessionData->GetCharacterData()) )
									{
										// ĳ���Ͱ� �ȸŴ޷��ִ� ���, ĳ���͸� �ҷ��´�.
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
										ERRLOG1( g_Log, "CID:%10u ĳ���� ����ȭ�鿡�� ���� ���� �����, �̹� ���ӿ� �ε�� ĳ���Ͱ� �����մϴ�.", pCharView->CID );
									}
								}

								lpSessionData->SetCharViewGID( pCharView->CID, 0 );	
							}
							
							aryGID[ i ]     = pCharView->GID;
							aryFame[ i ]    = pCharView->Fame;

                            /*
                                - �������� ���� �α�

                                UID CID ĳ���͸�	����������	�����ı���	������	�����	
                                ���緹��	�������ID  �����ID    ��ƼID
                            */

                            INFLOG11(g_Log, "�����α�\t%10u\t%10u\t%s\t%s\t%s\t%u\t%u\t%u\t%u\t%u\t%u\t", 
                                dwUID, pCharView->CID, pCharView->Name, 
                                Creature::GetShortNationName(cOldAccountNation),
                                Creature::GetShortNationName(cAccountNation),
                                aryOldFame[ i ], aryFame[ i ], pCharView->Level, 
                                aryOldGID[ i ], aryGID[ i ], pCharView->PID);
                        }
					}

					// ���� �������� ���� ��� ID�� �ٲ� ������ Auth ������ ������, Client �� �����ϵ��� �Ѵ�.
					SendPacket::NationChangeResult(SendStream, dwRequestKey, dwUID, aryGID, aryFame);
				}
			}
			else
			{
				ERRLOG3(g_Log, "UID:%10u / Old_AccountNation:%d / New_AccountNation:%d / ���� ���� ���� ���� : ���� ������",
						dwUID, lpSessionData->GetAccountNation(), cAccountNation);

				usError = 5;	// �̹� �����ҷ��� �����̴�.
			}
		}

		if (0 == usError)
		{
			// ���� ���� ���� �� ���� ����
			RULLOG2(g_Log, "UID:%10u / AccountNation:%d ���� ���� ����", dwUID, cAccountNation);

			// ���� ���� ����
			lpSessionData->SetAccountNation( cAccountNation );
		}
	}

	return SendPacket::SelectAccountNation(SendStream, dwRequestKey, dwUID, cType, cAccountNation, usError);
}

bool CharSelect(CSendStream& SendStream, PktCSD* lpPktCSD)
{
    // ���� ���� ���
    unsigned long dwUID         = lpPktCSD->m_dwUserID;
    unsigned long dwCID         = lpPktCSD->m_dwCharID;
    unsigned long dwRequestKey  = lpPktCSD->m_dwRequestKey;

    unsigned short usError = 0;
    unsigned char  cZone = 1;

    using namespace DataStorage;

    CSessionData* lpSessionData = CSessionDataMgr::GetInstance().GetOpenSession(dwUID);
    CCharacterData* lpCharacterData = 0;

    // ���� ���
    if(0 == lpSessionData)
    {
        ERRLOG1(g_Log, "UID:%10u / ĳ���� ���� ���� : ������ �����ϴ�", dwUID);
        usError = 5;
    }
    // ĳ���� ���̵� �˻�
    else if (!lpSessionData->HasCharacter(dwCID))
    {
        const USER_INFO& userInfo = lpSessionData->GetUserInfo();

		// WORK_LIST 2.1 ���� ���� �߰�
		ERRLOG8(g_Log, "UID:%10u / SelectedCID:%10u / Char1:%10u / Char2:%10u / Char3:%10u / Char4:%10u / Char5:%10u / Nation:%d"
            "ĳ���� ���� ���� : �����ϰ� ���� ���� ĳ�����Դϴ�.",
            dwUID, dwCID, userInfo.CharID[0], userInfo.CharID[1], userInfo.CharID[2], 
            userInfo.CharID[3], userInfo.CharID[4], userInfo.Nation);

        usError = 3;        
    }
    else 
    {
        if (!lpSessionData->LoadData(dwCID))
        {
            ERRLOG2(g_Log, "UID:%10u / SelectedCID:%10u / ĳ���� ���� ���� : ĳ���� �ε� ����", dwUID, dwCID);
            usError = 3;
        }
        else
        {
            CCharacterData* lpCharacterData = lpSessionData->GetCharacterData();
			lpCharacterData->SetUID(dwUID);

            SERVER_ID serverID;
            serverID.dwID = lpCharacterData->GetInfoEx().ServerID;

			// �������� �� ������ �������� ����Ʈ �谨.
			unsigned char cRealmPoint = lpCharacterData->GetRealmPoint();
			
			unsigned long dwRealmCount_Out = 0;

			// ���� ������ ���۵Ǹ� 
			// �α׾ƿ� �ð��� �������� ���������� ����Ʈ �߰� �谨��
			// ����Ѵ�.
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

			// edith 2010.01.02 ��������Ʈ�� ���� ������ ���� ó�����ִ� �Լ�.
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
                // �ʱ�ȭ�Ǿ� ���� ���� ĳ����
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
                // �ش� ���� �˻�
                if (!lpSessionData->CheckCharZone(cZone))
                {
                    // 0  : Human
                    // 1  : Akhan
                    // -1 : ĳ���� ������ ���� �ȵ�

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
						// �׼��� �ű��� �������ѿ� �ɷ���.
						DETLOG5(g_Log, "UID:%10u / CID:%10u / Zone:%u / StartZone:%u / Race:%u / �ű��� �������� : ���� ������ �̵��մϴ�", 
							dwUID, dwCID, cZone, cStartZone, cRace);
					}
					else
					{
						DETLOG5(g_Log, "UID:%10u / CID:%10u / Zone:%u / StartZone:%u / Race:%u / �߸��� �� üũ : ���� ������ �̵��մϴ�", 
							dwUID, dwCID, cZone, cStartZone, cRace);
					}
*/
					DETLOG5(g_Log, "UID:%10u / CID:%10u / Zone:%u / StartZone:%u / Race:%u / �߸��� �� üũ : ���� ������ �̵��մϴ�", 
						dwUID, dwCID, cZone, cStartZone, cRace);

                    cZone = cStartZone;
                }
/*
				// edith 2009.06.13 Character Select GameTime Over
				if(CGameTimeDBMgr::GetInstance().IsGuildWarTime() && cZone == SERVER_ID::CAPITAL)
				{
					DETLOG5(g_Log, "UID:%10u / CID:%10u / Zone:%u / StartZone:%u / Race:%u / IsGuildWarTime �� üũ : ���� ������ �̵��մϴ�", 
						dwUID, dwCID, cZone, cStartZone, cRace);

					cZone = cStartZone;
				}
				else if(CGameTimeDBMgr::GetInstance().IsRealmWarTime() && (SERVER_ID::STONE_WAR1 <= cZone && cZone <= SERVER_ID::STONE_WAR3))
				{
					DETLOG5(g_Log, "UID:%10u / CID:%10u / Zone:%u / StartZone:%u / Race:%u / IsRealmWarTime �� üũ : ���� ������ �̵��մϴ�", 
						dwUID, dwCID, cZone, cStartZone, cRace);

					cZone = cStartZone;
				}
*/
            }
            
            INFLOG2(g_Log, "UID:%10u / SelectedCID:%10u / ĳ���� ���� ����", dwUID, dwCID);
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

    // ���� ���
    if(0 == lpSessionData)
    {
        ERRLOG1(g_Log, "UID:%10u / ĳ���� ���� ���� : ������ �����ϴ�", dwUID);
        usError = PktCCAck::NULL_SESSION;
    }
    else
    {
        // ��ų ����
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
            ERRLOG3(g_Log, "UID:%10u / CID:%10u / Class:%u/ ĳ���� ���� : ��ų �ֱ� ���� - �߸��� Ŭ�����Դϴ�",
                dwUID, dwCID, lpPktCCD->m_CharCreate.Class);

			usError = PktCCAck::FAIL_INSERT_DATA; 
            break;
        }
		//Skill.SSlot[1] = SKILLSLOT(0x9E03, 0, 1);
		//Skill.SSlot[2] = SKILLSLOT(0x9E04, 0, 1);

			// Edith 2008.06.14 ĳ���� ������ ������ ���� ����
		if(dwGold > 0) 
		{
			ERRLOG3(g_Log, "UID:%10u / CID:%10u / Gold:%u/ ĳ���� ���� : ��尪 �̻�", dwUID, dwCID, dwGold);
			usError = PktCCAck::FAIL_INSERT_DATA; 
		}
		else if(lpPktCCD->m_CharCreate.Face < 0 && lpPktCCD->m_CharCreate.Face >= 7)
		{
			ERRLOG3(g_Log, "UID:%10u / CID:%10u / Face:%u/ ĳ���� ���� : �� �̻�", dwUID, dwCID, lpPktCCD->m_CharCreate.Hair);
			usError = PktCCAck::FAIL_INSERT_DATA; 
		}
		else if(lpPktCCD->m_CharCreate.Sex == 1 || lpPktCCD->m_CharCreate.Sex == 2)
		{
			if(lpPktCCD->m_CharCreate.Sex == 1)	// �ΰ� ����
			{
				if(lpPktCCD->m_CharCreate.Hair < 0 && lpPktCCD->m_CharCreate.Hair >= 5)
				{
					ERRLOG3(g_Log, "UID:%10u / CID:%10u / Hair:%u/ ĳ���� ���� : ��� �̻�", dwUID, dwCID, lpPktCCD->m_CharCreate.Hair);
					usError = PktCCAck::FAIL_INSERT_DATA; 
				}
			}
			else if(lpPktCCD->m_CharCreate.Sex == 2)// �ΰ� ����
			{
				if(lpPktCCD->m_CharCreate.Hair < 0 && lpPktCCD->m_CharCreate.Hair >= 7)
				{
					ERRLOG3(g_Log, "UID:%10u / CID:%10u / Hair:%u/ ĳ���� ���� : ��� �̻�", dwUID, dwCID, lpPktCCD->m_CharCreate.Hair);
					usError = PktCCAck::FAIL_INSERT_DATA; 
				}
			}
			else
			{
				ERRLOG3(g_Log, "UID:%10u / CID:%10u / Sex:%u/ ĳ���� ���� : ���� �̻�", dwUID, dwCID, lpPktCCD->m_CharCreate.Sex);
				usError = PktCCAck::FAIL_INSERT_DATA; 
			}
		}
		else if(lpPktCCD->m_CharCreate.Sex == 3 || lpPktCCD->m_CharCreate.Sex == 4)
		{
			if(lpPktCCD->m_CharCreate.Sex == 3)	// ��ĭ����
			{
				if(lpPktCCD->m_CharCreate.Hair < 0 && lpPktCCD->m_CharCreate.Hair >= 7)
				{
					ERRLOG3(g_Log, "UID:%10u / CID:%10u / Hair:%u/ ĳ���� ���� : ��� �̻�", dwUID, dwCID, lpPktCCD->m_CharCreate.Hair);
					usError = PktCCAck::FAIL_INSERT_DATA; 
				}
			}
			else if(lpPktCCD->m_CharCreate.Sex == 4) // ��ĭ����
			{
				if(lpPktCCD->m_CharCreate.Hair < 0 && lpPktCCD->m_CharCreate.Hair >= 7)
				{
					ERRLOG3(g_Log, "UID:%10u / CID:%10u / Hair:%u/ ĳ���� ���� : ��� �̻�", dwUID, dwCID, lpPktCCD->m_CharCreate.Hair);
					usError = PktCCAck::FAIL_INSERT_DATA; 
				}
			}
			else
			{
				ERRLOG3(g_Log, "UID:%10u / CID:%10u / Sex:%u/ ĳ���� ���� : ���� �̻�", dwUID, dwCID, lpPktCCD->m_CharCreate.Sex);
				usError = PktCCAck::FAIL_INSERT_DATA; 
			}
		}
		else
		{
			ERRLOG3(g_Log, "UID:%10u / CID:%10u / Race:%u/ ĳ���� ���� : ���� �̻�", dwUID, dwCID, lpPktCCD->m_CharCreate.Race);
			usError = PktCCAck::FAIL_INSERT_DATA; 
		}


		int iStat = lpPktCCD->m_CharCreate.STR+lpPktCCD->m_CharCreate.DEX
			+lpPktCCD->m_CharCreate.CON+lpPktCCD->m_CharCreate.INT+lpPktCCD->m_CharCreate.WIS - 100;

		if(iStat != 5)
		{
			ERRLOG3(g_Log, "UID:%10u / CID:%10u / iStat:%u/ ĳ���� ���� : ���� �̻�", dwUID, dwCID, iStat+100);
			usError = PktCCAck::FAIL_INSERT_DATA; 
		}

		if (usError != 0)
		{
			;
		}
		// DB�� ĳ���� ����
		else if (!DBComponent::GameDB::InsertChar(CDBSingleObject::GetInstance(), dwUID, dwSlot, 
            lpPktCCD->m_CharCreate, dwGold, lpPktCCD->m_Pos, Skill, 
            lpSessionData->GetOldServerGroupID(), dwCID)) 
        {
            ERRLOG1(g_Log, "UID:%10u / ĳ���� ���� ���� : DB����", dwUID);

            // ĳ���� ���� ����
            usError = PktCCAck::FAIL_INSERT_DATA; 
        }
        else if (-1 == dwCID) 
        {	
            // ������ �̸��� ������ �ִ� ĳ���Ͱ� �ִ� ���
            usError = PktCCAck::EXIST_CHAR_NAME;            
        }
        else if (-2 == dwCID) 
        {	
            // ������ �̸��� ������ �ִ� ĳ���Ͱ� �ִ� ���
            usError = PktCCAck::EXIST_SLOT;
        }
        else if (!lpSessionData->GetUserInfoFromDB(CDBSingleObject::GetInstance()))
        {
            // ĳ���͸� ����������. �並 �ٽ��ѹ� �ε��Ѵ�.
            ERRLOG1(g_Log, "UID:%10u / ĳ���� ���� ���� : ĳ���� �� ��ε� ����.", dwUID);
            usError = PktCCAck::VIEW_RELOAD_FAILED;
        }
        else if (!lpSessionData->GetCharView(dwCID, CharView))
        {
            // �並 �� ���� �ش�.
            ERRLOG1(g_Log, "UID:%10u / ĳ���� ���� ���� : ĳ���� �� ��� ����.", dwUID);
            usError = PktCCAck::VIEW_RELOAD_FAILED;                
        } 
        else
        {
            // ĳ���� ���� ����.
            RULLOG4(g_Log, "UID:%10u / CID:%10u / ĳ���� [%s]%s ����", 
                dwUID, dwCID, (CharView.Race == 0) ? "Human" : "Akhan", CharView.Name);

            /*
                - �������� ���� �α�

                UID CID ĳ���͸�	����������	�����ı���	������	�����	
                ���緹��	�������ID  �����ID    ��ƼID
            */

            INFLOG11(g_Log, "�����α�\t%10u\t%10u\t%s\t%s\t%s\t%u\t%u\t%u\t%u\t%u\t%u\t", 
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
	
    // ���� ���
    if(0 == lpSessionData)
    {
        ERRLOG1(g_Log, "UID:%10u / ĳ���� ���� ���� : ������ �����ϴ�", dwUID);
        usError = 4;
    }
    else if (CCharacterDataMgr::GetInstance().IsDataLoginDB(dwCID) || 
        CSessionData::SE_CHAR_ENABLED == lpSessionData->GetSessionState())
    {
        // Ȱ��ȭ�� ĳ���� ���� �õ�
        ERRLOG2(g_Log, "UID:%10u / CID:%10u / ĳ���� ���� ���� : Ȱ��ȭ�� ĳ���� ���� �õ�", dwUID, dwCID);
        usError = 3;
    }
    else
    {
        const CHAR_VIEW* lpCharView = 
            lpSessionData->GetCharView(static_cast<unsigned char>(dwSlot));

        if(0 == lpCharView)
        {
            ERRLOG3(g_Log, "UID:%10u / CID:%10u / Slot:%u / ĳ���� ���� ���� : ���ϴ� ���Կ� �� ����",
                dwUID, dwCID, dwSlot);

            usError = 2;
        }
        else if(lpCharView->CID != dwCID)
        {
            ERRLOG4(g_Log, "UID:%10u / CID:%10u / DeleteCID:%10u / Slot:%u / ĳ���� ���� ���� : ���� CID�� ������� ĳ������ CID�ٸ�",
                dwUID, lpCharView->CID, dwCID, dwSlot);

            usError = 2;
        }
        else
        {				
			// DB�� ���� ĳ���� ����
			if(CServerSetup::GetInstance().IsBattleAgentServer())
			{
				if(!DBComponent::GameDB::DeleteCharBattleServer(
					CDBSingleObject::GetInstance(), dwUID, dwCID, 
					static_cast<unsigned char>(dwSlot),
					CServerSetup::GetInstance().GetServerGroup(), 
					lpSessionData->GetOldServerGroupID()))
				{
					ERRLOG3(g_Log, "UID:%10u / CID:%10u / Slot:%u / ĳ���� ���� ���� : DeleteCharBattleServer DB����",
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
					ERRLOG3(g_Log, "UID:%10u / CID:%10u / Slot:%u / ĳ���� ���� ���� : DeleteChar DB����", 
						dwUID, dwCID, dwSlot);

					usError = 2;
				}
			}

			if(0 == usError)
			{
				// ĳ���� ���� ����
				RULLOG4(g_Log, "UID:%10u / CID:%10u / ĳ���� [%s]%s ����", 
					dwUID, dwCID, (lpCharView->Race == 0) ? "Human" : "Akhan", lpCharView->Name);

				// ĳ���� �ʱ�ȭ
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
            ERRLOG1(g_Log, "CID:%10u / ��� ������Ʈ ����", lpPktCCID->m_dwCharID);
        }
        else
        {            
            // ������ �ø��� ������Ʈ
            if(itemSerialMgr.SetItemSerial(lpPktCCID->m_dwItemSerial))
            {
                // �ø����� �ٲ�����Ƿ� DB�� ������Ʈ�Ѵ�.
                if(!itemSerialMgr.SaveItemSerial(CDBSingleObject::GetInstance(), dwServerID))
                {   
                    ERRLOG3(g_Log, "CID:%10u / ServerID:0x%08X / Serial:0x%016I64X / ������ �ø��� ������Ʈ ����", 
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
            ERRLOG2(g_Log, "UID:%10u / ��Ʋ�������� ĳ���� ���� �ֱ� ���� : ���� ���� ��� ���� : %s",
                dwUID, CDBSingleObject::GetInstance().GetErrorString());

            usError = PktBGServerCharSlotAck::FAIL_NOT_USER;
        }
        else
        {
            CHAR_VIEW charView;

			// WORK_LIST 2.1 ���� ���� �߰� (��Ʋ ���ѿ� Nation ������ ����ϳ�? ���� �߰�������)
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
                        ERRLOG3(g_Log, "UID:%10u / CID:%10u / ��Ʋ�������� ĳ���� ���� �ֱ� ���� : ĳ���� �� ��� ���� : %s",
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
        0 : â�� ������ �� ����
        1 : Part1 ���տ��� â�� �����Դ�
        2 : Part2 ���տ��� â�� �����Դ�.

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
            // �ش� UID�� ����.
            ERRLOG1(g_Log, "UID:%10u / Part2Unified ĳ���� ���� ���� - ������ ���� �ֽ��ϴ�", dwUID);

            usError = PktUnifiedCharSelectAck::SERVER_ERROR;
        }
        else
        {
            // �ϴ� �����͸� Ȯ���ϰ�, ������ �Ŀ�, Part1 DB�߰輭���� ���� ����� �˷��ش�.
            // Part1 DB�߰輭���� �ش� ����/ĳ���Ͱ� ĳ���� ������, ��ε��ϰ�,
            // �ش� ĳ���͸� ��Ƽ/��忡�� Ż���Ų �� Ack�� �����ش�.

            // Ack�� ������ â��/ĳ���� ������ �����Ѵ�.

            if (!(lpSessionData->GetFirstLogin() & UnifiedConst::SELECTED_PART2))
            {
                // �����ϴ� â�� �����ߴ��� ����
                if (!lpSessionData->HasUnifiedStoreInfo(cSelectedServerGroupID))
                {
                    usError = PktUnifiedCharSelectAck::WRONG_STORE_SELECTED;
                }
            }

            if(0 == usError)
            {
                // ������ ��� ĳ���͸� �����Ѵ�.
                USER_INFO userInfo = lpSessionData->GetUserInfo();
                
                unsigned long   dwSelectedCID[USER_INFO::MAX_CHAR_NUM]; 
                unsigned long*  lpdwRequestedCID = lpUnifiedSelectReq->dwCID;

                int nSelectedCount = 0;

                std::fill_n(dwSelectedCID, size_t(USER_INFO::MAX_CHAR_NUM), 0);

                // �����͸� ����� ��� �Դ�.
                // ���� ���ε� �� ĳ�������� �ƴ��� �Ǵ��Ѵ�.
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
                    // ĳ���Ͱ� �ִµ� ������ �ٸ��� ����!
                    usError = PktUnifiedCharSelectAck::PACKET_ERROR;
                }
                else if (CServerSetup::GetInstance().GetMaxTransferPart1ToPart2Count() <
                    nSelectedCount + lpSessionData->GetTransferedCharCount(CDBSingleObject::GetInstance()))
                {
                    // �ִ� ĳ���� ���� ȸ�� �̻� �����ϸ� ����
                    usError = PktUnifiedCharSelectAck::TRANSFER_COUNT_OVER;
                }
                else if (0 < nSelectedCount || !(lpSessionData->GetFirstLogin() & UnifiedConst::SELECTED_PART2))
                {
                    // Part1�߰輭���� '������' �����͸� ������.
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
            // �ش� UID�� ����.
            ERRLOG1(g_Log, "UID:%10u / Part2Selectable ĳ���� ���� ���� - ������ ���� �ֽ��ϴ�", dwUID);

            usError = PktUnifiedCharSelectAck::SERVER_ERROR;
        }
        else
        {
            if (0 < cSelectedServerGroupID && 
                cSelectedServerGroupID <= CServerSetup::GetInstance().GetSelectableUnifiedServerNum())
            {
                // ���⼭�� �����͸� ������ �� �ʿ䰡 ����.
                // �̹� ������ �� �����͸� ���� �����̴�.

                // �������� �����ߴ�. ������ �������� �����Ѵ�.            
                lpSessionData->SetOldServerGroupID(cSelectedServerGroupID);

                if(!lpSessionData->GetUserInfoFromDB(CDBSingleObject::GetInstance()))
                {
                    ERRLOG3(g_Log, "UID:%10u / FirstLogin:%d / SelectedServerGroupID:%d / "
                        "Part2Selectable ĳ���� ���� ���� - ���� ���� DB���� �б� ����.", 
                        dwUID, lpSessionData->GetFirstLogin(), cSelectedServerGroupID);

                    usError = PktUnifiedCharSelectAck::SERVER_ERROR;
                }
            }
            else
            {
                ERRLOG3(g_Log, "UID:%10u / FirstLogin:%d / SelectedServerGroupID:%d / "
                    "Part2Selectable ĳ���� ���� ���� - FirstLogin�̳� SelectedServerGroupID���� �ٸ��ϴ�.", 
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