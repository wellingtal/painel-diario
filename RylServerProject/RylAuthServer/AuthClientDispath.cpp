#include "stdafx.h"

#include <mmsystem.h>

#include <Creature/Character/CharacterCreate.h>
#include <Log/ServerLog.h>
#include <Stream/Buffer/BufferFactory.h>

#include <Utility/Filter/Filter.h>
#include <Utility/Resource/EnsureCleanup.h>
#include <Utility/Setup/ServerSetup.h>

#include <Network/Session/Session.h>
#include <Network/Packet/WrapPacket.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/ServerPacket.h>
#include <Network/Packet/PacketStruct/UnifiedCharPacket.h>
#include <Network/Packet/PacketStruct/ClientToAuthServer.h>
#include <Network/Packet/PacketStruct/AuthServerToDBAgentServer.h>
#include <Network/Dispatch/ServerRequest.h>
#include <Network/Packet/PacketStruct/PartyPacket.h>
#include <Network/Packet/PacketStruct/GuildPacket.h>

#include "SendUserLoginout.h"
#include "AuthAgentServerTable.h"
#include "AuthorizeUser.h"
#include "AuthorizeThread.h"
#include "AuthClientDispatch.h"

CAuthClientDispatch::CAuthClientDispatch(CSession& Session)
:	CRylServerDispatch(Session, 10), 
	m_dwUID(0), 
	m_dwSessionID(0), 
	m_CnFlag(0), 
	m_usPacketProcessFlag(0),
	m_cBlockedFlag(0),
    m_dwMoveServerID(0)
{
    memset(m_CharView, 0, sizeof(CHAR_VIEW) * USER_INFO::MAX_CHAR_NUM);
    memset(m_szBlockedID, 0, sizeof(char) * CHAR_INFOST::MAX_NAME_LEN * USER_INFO::MAX_CHAR_NUM);
}

CAuthClientDispatch::~CAuthClientDispatch()
{
    DETLOG3(g_Log, "SS:0x%p/DP:0x%p/IP:%15s/Destroy AuthClientDispatch", 
        &GetSession(), this, GetSession().GetRemoteAddr().get_addr_string());
}

CMultiDispatch& CAuthClientDispatch::GetDispatchTable()
{
	static CMultiDispatch multiDispatch;
	return multiDispatch;
}

void CAuthClientDispatch::Connected()
{
    DETLOG3(g_Log, "SS:0x%p/DP:0x%p/IP:%15s/Connected AuthClientDispatch", 
        &GetSession(), this, GetSession().GetRemoteAddr().get_addr_string());
}

void CAuthClientDispatch::Disconnected()
{
	UserLogout();

    DETLOG3(g_Log, "SS:0x%p/DP:0x%p/IP:%15s/Disconnected AuthClientDispatch", 
        &GetSession(), this, GetSession().GetRemoteAddr().get_addr_string());
}

bool CAuthClientDispatch::Dispatch()
{
	unsigned long dwCurrentTime = timeGetTime();

    if (m_CheckPing.CheckPing(dwCurrentTime))
    {
        return CRylServerDispatch::Dispatch();
    }
    else
    {
        unsigned long dwPingCount           = 0;
        unsigned long dwLastPingRecvTime    = 0;
        unsigned long dwFirstCheckTime      = 0;

        m_CheckPing.GetPingData(dwPingCount, 
            dwLastPingRecvTime, dwFirstCheckTime);

        const int MAX_BUFFER = 256;
        char szBuffer[MAX_BUFFER];

        _snprintf(szBuffer, MAX_BUFFER - 1, 
            "UID:%u/CurrentTime:%u/LastPingTime:%u/PingCount:%u/FirstCheckTime:%u/PingCheck failed. disconnect now.",
			m_dwUID, dwCurrentTime, dwLastPingRecvTime, dwPingCount, dwFirstCheckTime);

        szBuffer[MAX_BUFFER - 1] = 0;

        LogErrorPacket(szBuffer, 0);
    }

    return false;
}


void CAuthClientDispatch::UserLogin(unsigned long dwSessionID, unsigned long dwUID)
{
	if(0 != dwUID && 0 != dwSessionID)
	{
        m_dwSessionID = dwSessionID;
		m_dwUID = dwUID;

        INFLOG2(g_Log, "UID:%10u / SessionID:%10u / ���� �α���", m_dwUID, m_dwSessionID);

		GetDispatchTable().SetDispatch(m_dwUID, this);
	}
}

void CAuthClientDispatch::UserLogout(void)
{
	/////////////////////////////////////////////////////////////////////////////////
	// edith 2008.01.22 �����Ͼ� �α׾ƿ� ó���߰�
	// ����ġ�� ���� ������ ĳ���� ����ȭ�鿡�� ���������� ó���ϱ� ���ؼ��̴�.
	CAuthInfo* lpAuthInfo = new CAuthInfo(NULL, m_szAccountName, PktAU::ID_LEN);

	if (0 == lpAuthInfo)
    	DETLOG0(g_Log, "User authorize(logout) info create failed");
	else
	{
		// ���������� �߰�. �̷��� �ϸ�. AuthorizeThread ���� ������Ʈ���ȴ�.
		if (!CAuthorizeUser::GetInstance().AddAuthorizeUser(lpAuthInfo))
		{
			DETLOG1(g_Log, "Can't over authorize limit. logout Account name : %s", m_szAccountName);
			delete lpAuthInfo;
		}
	}
	/////////////////////////////////////////////////////////////////////////////////

	if (0 != m_dwUID && 0 != m_dwSessionID)
	{
		GET_MULTI_DISPATCH(lpAuthClientDispatch, m_dwUID, 
			CAuthClientDispatch, CAuthClientDispatch::GetDispatchTable());
		
		if (0 != lpAuthClientDispatch && this == lpAuthClientDispatch)
		{
            INFLOG2(g_Log, "UID:%10u / SessionID:%10u / ���� �α׾ƿ�", m_dwUID, m_dwSessionID);

			GetDispatchTable().RemoveDispatch(m_dwUID);
            AuthSendPacket::SendUserLogout(m_dwSessionID, m_dwUID);

			m_dwUID = 0;
			m_dwSessionID = 0;
		}
		else
		{
			ERRLOG3(g_Log, "this:0x%p/UIDDB_DP:0x%p/Unknown Dispatch's User(UID:%08d) Logout",
				this, lpAuthClientDispatch, m_dwUID);
		}
	}
    else
    {
        if(IsSetPacketProcessFlag(AUTHORIZE_PENDED))
        {
            CAuthorizeUser::GetInstance().RemovePending(m_szAccountName);
        }
    }
}

void CAuthClientDispatch::UserMove(void)
{
	if (0 != m_dwUID && 0 != m_dwSessionID)
	{
		GET_MULTI_DISPATCH(lpAuthClientDispatch, m_dwUID, 
			CAuthClientDispatch, GetDispatchTable());
				
		if (0 != lpAuthClientDispatch && this == lpAuthClientDispatch)
		{
            INFLOG2(g_Log, "UID:%10u / SessionID:%10u / ���� �̵�", m_dwUID, m_dwSessionID);

			GetDispatchTable().RemoveDispatch(m_dwUID);
			AuthSendPacket::SendUserMove(m_dwSessionID, m_dwUID);

			m_dwUID = 0;
			m_dwSessionID = 0;
		}
		else
		{
			ERRLOG3(g_Log, "this:0x%p/UIDDB_DP:0x%p/Unknown Dispatch's User(UID:%08d) Move",
				this, lpAuthClientDispatch, m_dwUID);
		}
	}
}

bool CAuthClientDispatch::DispatchPacket(PktBase* lpPktBase)
{
	// Client ���� ���� ��Ŷ�� ó���ϴ� �κ��̴�.

    switch(lpPktBase->GetCmd())
    {
		case CmdAuthAccount:			return ParseAuthAccount(static_cast<PktAU*>(lpPktBase));
		case CmdJapanAuthAccount:		return ParseJapanAuthAccount(static_cast<PktJPAU*>(lpPktBase));
		case CmdUserLogin:				return ParseUserLogin(static_cast<PktULi*>(lpPktBase));
		case CmdSysPatchAddress:		return ParseSysPatchAddress(static_cast<PktSPI*>(lpPktBase));

		case CmdCharSelect:				return ParseCharSelect(static_cast<PktCS*>(lpPktBase));
		case CmdCharCreate:			    return ParseCharCreate(static_cast<PktCC*>(lpPktBase));
		case CmdCharDelete:				return ParseCharDelete(static_cast<PktCD*>(lpPktBase));

		// WORK_LIST 2.1 ���� ���� �߰�
		case CmdSelectAccountNation:	return ParseSelectAccountNation(static_cast<PktSelectAccountNation*>(lpPktBase));

        case CmdUnifiedCharSelect:      return ParseUnifiedCharSelect(static_cast<PktUnifiedCharSelectReq*>(lpPktBase));

		case CmdServerZone:				return ParseServerZone(static_cast<PktSZ*>(lpPktBase));
	    
		case CmdSysPing:

			m_CheckPing.SetLastPingRecvTime(timeGetTime());			
			return true;

		default:        
			LogErrorPacket("��Ŷ ����� ��Ŷ Ŀ�ǵ尡 �߸��Ǿ����ϴ�.", lpPktBase->GetCmd());
			break;
    }

    return true;
}

bool CAuthClientDispatch::ParseServerZone(PktSZ* lpPktSZ)
{
	// Ŭ���̾�Ʈ�� ������ �����϶�� ��Ŷ�� ������ �κ�.

	if(IsSetPacketProcessFlag(PENDED_SERVER_ZONE)
        || IsSetPacketProcessFlag(SERVER_ZONE_SUCCESS))
	{
		// ��Ŷ�� �ѹ� �� ���� �����Ѵ�.
		return true;
	}

	unsigned char	Zone	= lpPktSZ->m_cZone;
	unsigned short	Channel	= lpPktSZ->m_cChannel;

	GET_SINGLE_DISPATCH(lpAuthAgentDispatch, CAuthAgentDispatch,
		CAuthAgentDispatch::GetDispatchTable());

	if(0 != lpAuthAgentDispatch)
    {
        CSendStream& SendStream = lpAuthAgentDispatch->GetSendStream();

	    char* lpBuffer = SendStream.GetBuffer(sizeof(PktSA));
		if (0 != lpBuffer)
        {
			unsigned long dwRequestKey = CServerRequest::GetInstance().AddRequest(this,
				lpAuthAgentDispatch, 100);

			// ������ �α׾ƿ� ��Ų��. ���⼭..��.


            if (0 != dwRequestKey)
            {
                PktSA* lpPktSA = reinterpret_cast<PktSA*>(lpBuffer);
                lpPktSA->InitPtSubCmd(dwRequestKey, PktSA::SCmdServerZone);

                lpPktSA->m_dwUserID	= GetUID();
                lpPktSA->m_cZone	= Zone;
                lpPktSA->m_cChannel	= (char)Channel;

				if(SendStream.WrapHeader(sizeof(PktSA), CmdAgentZone, 0, 0))
				{
                    DETLOG5(g_Log, "SS:0x%p/DP:0x%p/IP:%15s/sCmd:0x%02x/RequestKey:%d/DBRequest Send success.",
						&GetSession(), this, GetRemoteAddr().get_addr_string(),                        
                        PktSA::SCmdServerZone, dwRequestKey);

                    SetPacketProcessFlag(PENDED_SERVER_ZONE);
					return true;
				}

				CServerRequest::GetInstance().RemoveRequest(dwRequestKey);
            }			
        }
        else
        {
			ERRLOG2(g_Log, "SS:0x%p/DP:0x%p/Buffer allocate failed.", 
				&GetSession(), this);
        }
    }
    else
    {        
		ERRLOG0(g_Log, "Get agent session failed.");
    }

	return true;
}

// ��ġ ���� �ּҸ� Ŭ���̾�Ʈ���� �����ش�.
bool CAuthClientDispatch::ParseSysPatchAddress(PktSPI* lpPktSPI)
{
    char* lpBuffer = GetSendStream().GetBuffer(sizeof(PktSPIAck));
    if (0 != lpBuffer)
    {
        PktSPIAck* lpPktSLAck = reinterpret_cast<PktSPIAck*>(lpBuffer);
        
        CAuthAgentServerTable::GetInstance().GetPatchServerAddress(lpPktSLAck->m_dwClientVer,
			lpPktSLAck->m_PatchAddress, PktSPIAck::PATCH_ADDRESS_LENGTH);

        return GetSendStream().WrapHeader(sizeof(PktSPIAck), CmdSysPatchAddress, 0, 0);
    }

    return false;
}


bool CAuthClientDispatch::ParseAuthAccount(PktAU* lpPktAU)
{    
	// edith 2008.01.15 ĳ���Ͱ� �α����ߴ�.
	char *UserAccount	= lpPktAU->m_UserAccount;
	char *UserPassword	= lpPktAU->m_UserPassword;

	unsigned long   SessionID	= lpPktAU->m_dwSessionID;
	unsigned long   ClientVer	= lpPktAU->m_dwClientVer;
	unsigned long   CheckSum    = lpPktAU->m_dwCheckSum; 

	unsigned short	Flag	    = lpPktAU->m_usFlag;
	unsigned short	CnFlag	    = (((Flag & 0x2000) == 0x2000) ? 1 : 0);
	unsigned long	UserID	    = 0;

//    UserAccount[PktAU::ID_LEN - 1] = 0;
//    UserPassword[PktAU::PASS_LEN - 1] = 0;

    if(IsSetPacketProcessFlag(AUTHORIZE_PENDED) 
        || IsSetPacketProcessFlag(AUTHORIZED_SUCCESS))
    {
        // ��Ŷ ������
        return true;
    }        

    // ��Ŷ ó�� �÷��� ����
    SetPacketProcessFlag(AUTHORIZE_PENDED);

	if (1 == GetCnFlag() && CnFlag)
	{
		// ������ ���� �����⸦ �ߺ����� ������ �ڴ� ������ ���� ������ ���´�.
        DETLOG2(g_Log, "UID:%10u/CnFlag:0x%04x/Disconnect account count is more than twice. Disconnect now.", UserID, CnFlag);
		Shutdown();
		return false;
	}

	SetCnFlag(CnFlag);

	// üũ�� �˻�
	unsigned long dwServerChecksum = CAuthAgentServerTable::GetInstance().GetServerChecksum();
	if ((CheckSum != dwServerChecksum && 1 != dwServerChecksum) || 0 == dwServerChecksum)
	{
        ERRLOG3(g_Log, "Checksum error [%s] : Client:0x%08X, Server:0x%08X", UserAccount, CheckSum, dwServerChecksum);
		AuthSendPacket::SendAuthAccount(this, UserID, 3);		// üũ�� Ʋ��
		return false;
	}
	//hz added this for client check.
	char CheckClient=lpPktAU->m_dwDiskOk;

	if(CheckClient != 'S')
	{
		ERRLOG3(g_Log, "Checksum error [%s] : Client:0x%08X, Server:0x%08X", UserAccount, CheckSum, dwServerChecksum);
		AuthSendPacket::SendAuthAccount(this, UserID, 3);		// üũ�� Ʋ��
		return false;
	}
	//hz end.
	// ���� üũ
	unsigned long dwServerVersion = CAuthAgentServerTable::GetInstance().GetServerVersion();
	if (ClientVer != dwServerVersion) 
    {
        ERRLOG3(g_Log, "Version error [%s] : Client:%3d, Server:%3d", UserAccount, ClientVer, dwServerVersion);
		AuthSendPacket::SendAuthAccount(this, UserID, 4);		// ���� ���� Ʋ��
		return true;
    }

    // �̸�/�н����� üũ
    if (!CAuthorizeThread::CheckUserIDAndPass(UserAccount, UserPassword))
    {
        DETLOG2(g_Log, "Can't recognize account name : [%s][%s]", UserAccount, UserPassword);
        AuthSendPacket::SendAuthAccount(this, UserID, 5);		// �̸�/�н� ����
        return true;
    }

 	// ���� �� üũ
    if (!Filter::AccountCheck(UserAccount))
	{
		DETLOG1(g_Log, "Blocked user : %s", UserAccount);
		AuthSendPacket::SendAuthAccount(this, UserID, 8);		// ���� ��
		return true;
    }

	// ���� ó�� �߰�
    CAuthInfo* lpAuthInfo = new CAuthInfo(this, SessionID, UserAccount, 
        UserPassword, PktAU::ID_LEN, PktAU::PASS_LEN, Flag, CnFlag);

    if (0 == lpAuthInfo)
	{
    	DETLOG0(g_Log, "User authorize info create failed");
		AuthSendPacket::SendAuthAccount(this, UserID, 1);
		return true;
	}

	// edith 2008.01.15 DB������ �õ��ϱ� ���� �����带 ���������ؼ�
	// ���������� �߰�. �̷��� �ϸ�. AuthorizeThread ���� ������Ʈ���ȴ�.
	// AuthInfo���� 
    if (!CAuthorizeUser::GetInstance().AddAuthorizeUser(lpAuthInfo))
    {
		DETLOG1(g_Log, "Can't over authorize limit. Account name : %s", UserAccount);
    	AuthSendPacket::SendAuthAccount(this, UserID, 7);

	    delete lpAuthInfo;
		return true;
    }

    strcpy(m_szAccountName, UserAccount);
	return true;
}


//Interface/////////////////////////////////////////////////////////////////////////////////////
//
// ���� �α��� ó��		TODO : ���� �α����� ���� ����.. �ܼ��� ĳ���� �� ��û���θ� ���..
//
// Parameter :
//
// Do :
//	1. üũ�� �˻�		TODO : ���� �������� �Ѿ��..
//	2. ���� �α��� ������Ʈ���� �˷���
//	3. ������Ʈ���� 5 ĳ���� �� ��� Ŭ���̾�Ʈ���� ����
//
// Return :
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool CAuthClientDispatch::ParseUserLogin(PktULi* lpPktULi)
{
	GET_SINGLE_DISPATCH(lpAuthAgentDispatch, CAuthAgentDispatch,
		CAuthAgentDispatch::GetDispatchTable());

	if(0 != lpAuthAgentDispatch)
    {	  
        CSendStream& SendStream = lpAuthAgentDispatch->GetSendStream();
		char* lpBuffer = SendStream.GetBuffer(sizeof(PktULD));
        if (0 != lpBuffer)
        {
			unsigned long dwRequestKey = CServerRequest::GetInstance().AddRequest(this, 
				lpAuthAgentDispatch, 100);

            if (0 != dwRequestKey)
            {
                PktULD* lpPktULD = reinterpret_cast<PktULD*>(lpBuffer);
			    lpPktULD->InitPtSubCmd(dwRequestKey, PktULD::SCmdUserLogin);
                
                lpPktULD->m_dwUserID	= lpPktULi->m_dwUserID;
				lpPktULD->m_cLoginType	= lpPktULi->m_cLoginType;
				lpPktULD->m_dwSessionID	= GetSessionID();
			    lpPktULD->m_Address		= GetRemoteAddr().get_addr_in().sin_addr;

                if(SendStream.WrapHeader(sizeof(PktULD), CmdDBGetData, 0, 0))
				{
                    DETLOG5(g_Log, "SS:0x%p/DP:0x%p/IP:%15s/sCmd:0x%02x/RequestKey:%d/DBRequest Send success.",
						&GetSession(), this, GetRemoteAddr().get_addr_string(),
                        PktULD::SCmdUserLogin, dwRequestKey);

					return true;
				}

				CServerRequest::GetInstance().RemoveRequest(dwRequestKey);
            }
        }
        else
        {
			ERRLOG2(g_Log, "SS:0x%p/DP:0x%p/Buffer allocate failed.", 
				&GetSession(), this);
        }
    }
    else
    {        
		ERRLOG0(g_Log, "Get agent session failed.");
    }
    
	// WORK_LIST 2.1 ���� ���� �߰�
	return AuthSendPacket::SendUserLogin(GetSendStream(), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, PktULiAck::SERVER_ERROR);
}




//Interface/////////////////////////////////////////////////////////////////////////////////////
//
// ĳ���� ���� ó��
//
// Parameter :
//
// Do :
//	1. ���õ� ĳ���� ������Ʈ���� Ȯ��
//	2. ������Ʈ���� �ش� ĳ������ �� ���� �ּҸ� ��� Ŭ���̾�Ʈ���� ����
//
// Return :
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool CAuthClientDispatch::ParseCharSelect(PktCS* lpPktCS)
{
    if (2 == m_cBlockedFlag)
    {
        for(int nCharView = 0; nCharView < USER_INFO::MAX_CHAR_NUM; ++nCharView)
        {
            for(int nBlockedID = 0; nBlockedID < USER_INFO::MAX_CHAR_NUM; ++nBlockedID)
            {
                if (lpPktCS->m_dwCharID == m_CharView[nCharView].CID && 0 != m_szBlockedID[nBlockedID] &&
                    0 == strncmp(m_szBlockedID[nBlockedID], m_CharView[nCharView].Name, CHAR_INFOST::MAX_NAME_LEN))
                {
                    // ���� ������ ĳ���Ͱ� ��ϵǾ���.
                    return SendCharSelect(GetSendStream(), 0, 0, 5);
                }
            }
        }
    }
	else if(IsSetPacketProcessFlag(PENDED_CHAR_SELECT)
        || IsSetPacketProcessFlag(CHAR_SELECT_SUCCESS))
	{
		// ��Ŷ�� �ѹ� �� ���� �����Ѵ�.
		return true;
	}

	GET_SINGLE_DISPATCH(lpAuthAgentDispatch, CAuthAgentDispatch,
		CAuthAgentDispatch::GetDispatchTable());

	if(0 != lpAuthAgentDispatch)
    {	  
        CSendStream& SendStream = lpAuthAgentDispatch->GetSendStream();
        char* lpBuffer = SendStream.GetBuffer(sizeof(PktCSD));
        if (0 != lpBuffer)
        {
            unsigned long dwRequestKey = CServerRequest::GetInstance().AddRequest(this, 
				lpAuthAgentDispatch, 100);
            
            if (0 != dwRequestKey)
            {
                // ������Ʈ���� ���� ������
	            PktCSD* lpPktCSD = reinterpret_cast<PktCSD*>(lpBuffer);
    	        
			    lpPktCSD->InitPtSubCmd(dwRequestKey, PktCSD::SCmdCharSelect);

	            lpPktCSD->m_dwUserID = lpPktCS->m_dwUserID;
	            lpPktCSD->m_dwCharID = lpPktCS->m_dwCharID;

                if(SendStream.WrapHeader(sizeof(PktCSD), CmdDBGetData, 0, 0))
				{
                	SetPacketProcessFlag(PENDED_CHAR_SELECT);

                    DETLOG5(g_Log, "SS:0x%p/DP:0x%p/IP:%15s/sCmd:0x%02x/RequestKey:%d/DBRequest Send success.",
						&GetSession(), this, GetRemoteAddr().get_addr_string(),
                        PktCSD::SCmdCharSelect, dwRequestKey);

					return true;
				}

				CServerRequest::GetInstance().RemoveRequest(dwRequestKey);
            }
        }
        else
        {
            ERRLOG1(g_Log, "Session:0x%p Buffer allocation failed.", &GetSession());
        }
    }
    else
    {
 		ERRLOG0(g_Log, "Get agent session failed.");
    }

    return SendCharSelect(GetSendStream(), 0, 0, 1); 
}



//Interface/////////////////////////////////////////////////////////////////////////////////////
//
// ĳ���� ���� ó��
//
// Parameter :
//
// Do :
//	1. ������ ĳ���� ���� �˻� (�̸�, �ɷ�ġ)
//	2. ������Ʈ�� ���� ������
//	3. ������Ʈ���� ������ ĳ���� ���� Ŭ���̾�Ʈ���� ����
//
// Return :
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool CAuthClientDispatch::ParseCharCreate(PktCC* lpPktCC)
{
    unsigned short usErrorCode = 1;

    CHAR_CREATE& CharCreateInfo = lpPktCC->m_CreateChar;

    if(IsSetPacketProcessFlag(PENDED_CHAR_CREATE))
    {
        // ĳ���� ������ Pending�Ǿ� ������ ���� �Ұ�.
        return true;
    }

    if(CServerSetup::GetInstance().IsSupressCharCreate())
    {
        // ĳ���� ���� �Ұ� ��� ���õ�
        usErrorCode = PktCCAck::SUPRESSED_CHAR_CREATE;
    }	
	else if (!CharCreate::CheckCharCreateName(CharCreateInfo.Name, CServerSetup::GetInstance().GetHanCheck()))
	{
        // ���� ĳ���� �̸� �˻�
        usErrorCode = PktCCAck::WRONG_CHAR_NAME;
		ERRLOG2(g_Log, "UID:0x%08X �߸��� ĳ���� �̸� %s", lpPktCC->m_dwUserID, CharCreateInfo.Name);
	}
    else if (!Filter::NameCheck(CharCreateInfo.Name))
	{
        // ���� ĳ���� �̸� ���͸�
        usErrorCode = PktCCAck::SLANG_FILTERED;		    
		ERRLOG2(g_Log, "UID:0x%08X �弳 ĳ���� �̸� %s", lpPktCC->m_dwUserID, CharCreateInfo.Name);
	}
    else if (!CharCreate::CheckCharCreateData(CharCreateInfo))
	{
        // ���� ĳ���� ���� �˻�
        usErrorCode = PktCCAck::WRONG_CHAR_DATA;		
		ERRLOG1(g_Log, "UID:0x%08X �߸��� ĳ���� ������", lpPktCC->m_dwUserID);		
	}
    else
    {
		GET_SINGLE_DISPATCH(lpAuthAgentDispatch, CAuthAgentDispatch,
			CAuthAgentDispatch::GetDispatchTable());

		if(0 != lpAuthAgentDispatch)
		{	  
			RylServerInfo AgentServerInfo;
            CAuthAgentServerTable::GetInstance().GetAgentServerInfo(AgentServerInfo);
            		
            CSendStream& SendStream = lpAuthAgentDispatch->GetSendStream();
            char* lpBuffer = SendStream.GetBuffer(sizeof(PktCCD));
            if (0 != lpBuffer)
            {
                unsigned long dwRequestKey = CServerRequest::GetInstance().AddRequest(this, 
					lpAuthAgentDispatch, 100);

                if (0 != dwRequestKey)
                {
                    PktCCD* lpPktCCD = reinterpret_cast<PktCCD*>(lpBuffer);
                        
	                // ������Ʈ�� ĳ���� ���� ����.            	
				    lpPktCCD->InitPtSubCmd(dwRequestKey, PktCCD::SCmdCharCreate);

                    // ������ ĳ���� ������ ����
	                lpPktCCD->m_dwUserID		= lpPktCC->m_dwUserID;
	                lpPktCCD->m_dwSlot			= lpPktCC->m_dwSlotNum;
	                lpPktCCD->m_dwGold			= CharCreate::GetDefaultStartGold();
                    lpPktCCD->m_Pos				= CharCreate::GetDefaultCharacterPos(CharCreateInfo.Race,
                                                    AgentServerInfo.m_usChannelClientNum[CharCreateInfo.Race]);

                    lpPktCCD->m_CharCreate		= CharCreateInfo;

                    SetPacketProcessFlag(PENDED_CHAR_CREATE);

                    if(SendStream.WrapHeader(sizeof(PktCCD), CmdDBGetData, 0, 0))						
					{
                        DETLOG5(g_Log, "SS:0x%p/DP:0x%p/IP:%15s/sCmd:0x%02x/RequestKey:%d/DBRequest Send success.",
							&GetSession(), this, GetRemoteAddr().get_addr_string(),
                            PktCCD::SCmdCharCreate, dwRequestKey);
                        
                        return true;
                    }

                    ResetPacketProcessFlag(PENDED_CHAR_CREATE);
					CServerRequest::GetInstance().RemoveRequest(dwRequestKey);
                }
            }
            else
            {
                ERRLOG1(g_Log, "Session:0x%p Buffer allocate failed.", &GetSession());
            }
        }
        else
        {
		    ERRLOG0(g_Log, "Get agent session failed.");
        }
    }

    return SendCharCreate(GetSendStream(), 0, 0, 0, usErrorCode); 
}



//Interface/////////////////////////////////////////////////////////////////////////////////////
//
// ĳ���� ���� ó��
//
// Parameter :
//
// Do :
//	1. ������ ĳ���� ���� ���� (����)
//	2. ������Ʈ�� ���� ������
//	3. ������Ʈ���� ������ ĳ���� ������ Ŭ���̾�Ʈ���� ����
//
// Return :
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool CAuthClientDispatch::ParseCharDelete(PktCD* lpPktCD)
{
    unsigned short wError = PktCDAck::SERVER_ERROR;

    if (CServerSetup::GetInstance().IsSupressCharDelete())
    {
        // ���� �� ����
        wError = PktCDAck::BLOCKED_DELETE_DATA;
    }
    else if (0 <= lpPktCD->m_dwSlotNum && lpPktCD->m_dwSlotNum < USER_INFO::MAX_CHAR_NUM)
    {
        if(IsSetPacketProcessFlag(PENDED_CHAR_DELETE))
        {
            return true;
        }

		GET_SINGLE_DISPATCH(lpAuthAgentDispatch, CAuthAgentDispatch,
			CAuthAgentDispatch::GetDispatchTable());

		char*		    lpBuffer		= 0;
		unsigned long	dwRequestKey	= 0;

		const TCHAR* szErrorMessage = 0;

		if (0 == lpAuthAgentDispatch)
		{
			szErrorMessage = _T("Get agent failed.");
		}
		else if(0 == (dwRequestKey = CServerRequest::GetInstance().AddRequest(this, lpAuthAgentDispatch, 100)))
		{
			szErrorMessage = _T("Get requestkey failed.");
		}

		if(0 != szErrorMessage)
		{
			ERRLOG2(g_Log, "SS:0x%p/%s", GetSession(), szErrorMessage);
		}
		else
		{   
			// ��� üũ //
			STORE_INFO* pStoreInfo = GetStoreInfo();

			if(pStoreInfo)
			{
				if(memcmp(pStoreInfo->Password, lpPktCD->m_szPassword, STORE_INFO::MAX_PASS_LEN)!=0)
				{
					wError = 7;
				}				
			}

			if(wError!=7)
			{
				CSendStream& SendStream = lpAuthAgentDispatch->GetSendStream();

				CHAR_VIEW& CurrentCharView = m_CharView[lpPktCD->m_dwSlotNum];

				unsigned long dwCID = lpPktCD->m_dwCharID;

				const char*	szCharName = CurrentCharView.Name;

				// ������Ʈ�� ���� ����
				PktCDD* lpPktCDD = reinterpret_cast<PktCDD*>(SendStream.GetBuffer(sizeof(PktCDD)));

				if(0 != lpPktCDD)
				{
					lpPktCDD->InitPtSubCmd(dwRequestKey, PktCCD::SCmdCharDelete);

					lpPktCDD->m_dwUserID	= lpPktCD->m_dwUserID;
					lpPktCDD->m_dwCharID	= lpPktCD->m_dwCharID;
					lpPktCDD->m_dwSlotNum	= lpPktCD->m_dwSlotNum;				

					SetPacketProcessFlag(PENDED_CHAR_DELETE);

					if (SendStream.WrapHeader(sizeof(PktCDD), CmdDBGetData, 0, 0))
					{				
						DETLOG5(g_Log, "SS:0x%p/DP:0x%p/IP:%15s/sCmd:0x%02x/RequestKey:%d/DBRequest Send success.",
							&GetSession(), this, GetRemoteAddr().get_addr_string(),
							PktCCD::SCmdCharDelete, dwRequestKey);

						return true;
					}

					ResetPacketProcessFlag(PENDED_CHAR_DELETE);
				}
			}
        }

		if(0 != dwRequestKey)
		{
			// ���� ���н� RequestKey �ݳ�
			CServerRequest::GetInstance().RemoveRequest(dwRequestKey);
		}
	}

    return SendCharDelete(GetSendStream(), 0, 0, wError);
}

// WORK_LIST 2.1 ���� ���� �߰�
//Interface/////////////////////////////////////////////////////////////////////////////////////
//
// ���� ���� ���� �� ����
//
// Parameter :
//
// Do :
//	1. DB AgentServer �� ���� ����
//	
// Return :
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool CAuthClientDispatch::ParseSelectAccountNation(PktSelectAccountNation* lpPktSAN_In)
{
	if (0 <= lpPktSAN_In->m_cAccountNation && lpPktSAN_In->m_cAccountNation < Creature::MAX_NATION)
	{
		GET_SINGLE_DISPATCH(lpAuthAgentDispatch, CAuthAgentDispatch,
			CAuthAgentDispatch::GetDispatchTable());

		char*		    lpBuffer		= 0;
		unsigned long	dwRequestKey	= 0;

		const TCHAR* szErrorMessage = 0;

		if (0 == lpAuthAgentDispatch)
		{
			szErrorMessage = _T("Get agent failed.");
		}
		else if(0 == (dwRequestKey = CServerRequest::GetInstance().AddRequest(this, lpAuthAgentDispatch, 100)))
		{
			szErrorMessage = _T("Get requestkey failed.");
		}

		if(0 != szErrorMessage)
		{
			ERRLOG2(g_Log, "SS:0x%p/%s", GetSession(), szErrorMessage);
		}
		else
		{
			CSendStream& SendStream = lpAuthAgentDispatch->GetSendStream();

			// ������Ʈ�� ����
			PktSAND* lpPktSAND = reinterpret_cast<PktSAND*>(SendStream.GetBuffer(sizeof(PktSAND)));

			if(0 != lpPktSAND)
			{
				lpPktSAND->InitPtSubCmd(dwRequestKey, PktCCD::SCmdSelectNation);

				lpPktSAND->m_dwUserID		= lpPktSAN_In->m_dwUserID;
				lpPktSAND->m_cType			= lpPktSAN_In->m_cType;
				lpPktSAND->m_cAccountNation	= lpPktSAN_In->m_cAccountNation;

				if (SendStream.WrapHeader(sizeof(PktSAND), CmdDBGetData, 0, 0))
				{				
                    DETLOG5(g_Log, "SS:0x%p/DP:0x%p/IP:%15s/sCmd:0x%02x/RequestKey:%d/DBRequest Send success.",
						&GetSession(), this, GetRemoteAddr().get_addr_string(),
                        PktCCD::SCmdSelectNation, dwRequestKey);

					return true;
				}
			}
		}

		if(0 != dwRequestKey)
		{
			// ���� ���н� RequestKey �ݳ�
			CServerRequest::GetInstance().RemoveRequest(dwRequestKey);
		}
	}

	return SendSelectAccountNation(GetSendStream(), 0, 0, 0, PktSelectAccountNation::SERVER_ERROR);
}


void CAuthClientDispatch::SetBlockedID(unsigned char cBlockedFlag, const char* szBlockedChar[USER_INFO::MAX_CHAR_NUM])
{
    m_cBlockedFlag = cBlockedFlag;
    
    for(unsigned int nCount = 0; nCount < USER_INFO::MAX_CHAR_NUM; ++nCount)
    {
        if (0 != szBlockedChar[nCount])
        {
            strncpy(m_szBlockedID[nCount], szBlockedChar[nCount], CHAR_INFOST::MAX_NAME_LEN);
        }
    }
}



bool CAuthClientDispatch::ParseJapanAuthAccount(PktJPAU* lpPktJPAU)
{    
	unsigned long SessionID		= lpPktJPAU->m_dwSessionID;
	unsigned long ClientVer		= lpPktJPAU->m_dwClientVer;
	unsigned long CheckSum		= lpPktJPAU->m_dwCheckSum; 
    unsigned long UserID        = lpPktJPAU->m_dwUserID;

    lpPktJPAU->m_szUserAccount[PktJPAU::NAME_LEN - 1] = 0;

	unsigned short Flag	= lpPktJPAU->m_usFlag;
	unsigned short CnFlag = (((Flag & 0x2000) == 0x2000) ? 1 : 0);

    if(IsSetPacketProcessFlag(AUTHORIZE_PENDED) 
        || IsSetPacketProcessFlag(AUTHORIZED_SUCCESS))
    {
        // ��Ŷ ������
        return true;
    }        

    // ��Ŷ ó�� �÷��� ����
    SetPacketProcessFlag(AUTHORIZE_PENDED);


	// ������ ���� �����⸦ �ߺ����� ������ �ڴ� ������ ���� ������ ���´�.
	if (1 == GetCnFlag() && CnFlag)
	{
        DETLOG2(g_Log, "UID:%10u/CnFlag:0x%04x/Disconnect account count is more than twice. Disconnect now.", UserID, CnFlag);
		GetSession().Shutdown();
		return false;
	}
	SetCnFlag(CnFlag);

	// üũ�� �˻�
	unsigned long dwServerChecksum = CAuthAgentServerTable::GetInstance().GetServerChecksum();
	if ((CheckSum != dwServerChecksum && 1 != dwServerChecksum) || 0 == dwServerChecksum)
	{
        ERRLOG3(g_Log, "Checksum failed [%s] : Client:0x%08X, Server:0x%08X", 
            lpPktJPAU->m_szUserAccount, CheckSum, dwServerChecksum);
		AuthSendPacket::SendAuthAccount(this, UserID, 3);		// üũ�� Ʋ��
		return false;
	}

	// ���� üũ
	unsigned long dwServerVersion = CAuthAgentServerTable::GetInstance().GetServerVersion();
	if (ClientVer != dwServerVersion) 
    {
        ERRLOG3(g_Log, "Version error [%s] : Client:%3d, Server:%3d", 
            lpPktJPAU->m_szUserAccount, ClientVer, dwServerVersion);

		AuthSendPacket::SendAuthAccount(this, UserID, 4);		// ���� ���� Ʋ��
		return true;
    }

 	// ���� �� üũ
    if (!Filter::AccountCheck(lpPktJPAU->m_szUserAccount))
	{
		DETLOG1(g_Log, "Blocked Account : %s", lpPktJPAU->m_szUserAccount);

		AuthSendPacket::SendAuthAccount(this, UserID, 8);		// ���� ��
		return true;
    }

	// ���� ó�� �߰�
    CAuthInfo* lpAuthInfo = new CAuthInfo(this, SessionID, lpPktJPAU->m_szUserAccount, 
        reinterpret_cast<char*>(&lpPktJPAU->m_dwUserID), PktAU::ID_LEN, sizeof(unsigned long), Flag, CnFlag);

    if (0 == lpAuthInfo)
	{
    	DETLOG0(g_Log, "Authorize info create failed");

		AuthSendPacket::SendAuthAccount(this, UserID, 1);
		return true;
	}

    if (!CAuthorizeUser::GetInstance().AddAuthorizeUser(lpAuthInfo))
    {
		DETLOG1(g_Log, "Authorize user limit failed - Account : %s", lpPktJPAU->m_szUserAccount);

		AuthSendPacket::SendAuthAccount(this, UserID, 7);

	    delete lpAuthInfo;
		return true;
    }

	return true;
}

bool SendGuildLeaveToDBAgent(CSendStream& SendStream, unsigned long dwGID, 
							 unsigned long dwCID, const char* szName)
{
	char* lpBuffer = SendStream.GetBuffer(sizeof(PktGuildCmd));
	
	if (0 != lpBuffer)
	{
		PktGuildCmd* lpPktGC = reinterpret_cast<PktGuildCmd *>(lpBuffer);

		lpPktGC->m_dwGID = dwGID;

		lpPktGC->m_dwSenderID = dwCID;
		lpPktGC->m_dwReferenceID = dwCID;

		strcpy(lpPktGC->m_szGuildName, "Ż��");

		if(0 != szName)
		{
            strncpy(lpPktGC->m_szSenderName, szName, Guild::MAX_MEMBER_NAME_LEN);
		}
		else
		{
			memset(lpPktGC->m_szSenderName, 0, Guild::MAX_MEMBER_NAME_LEN);
		}

		lpPktGC->m_wCmd = PktGuildCmd::GC_KICK;

		return SendStream.WrapCrypt(sizeof(PktGuildCmd), CmdGuildCmd, 0, 0);
	}

	return false;
}

bool CAuthClientDispatch::ParseUnifiedCharSelect(PktUnifiedCharSelectReq* lpPktUnifiedCharSelectReq)
{
    unsigned short usError = 0;

    // ���� ���¿� ID/Password���� üũ�� ��, ������ DB�߰�� ��û�� ������.

    // �ѱ� ���ռ��� Ÿ�Կ���, â�� ���� ���� �� �ߴµ�, ����� �Ⱥ�������
    // ������ ������.
    lpPktUnifiedCharSelectReq->szPassword[ MAX_PASSWORD_LEN - 1 ] = 0;

    if (UnifiedConst::Part2Unified == m_cAgentServerType &&
        0 != strcmp(lpPktUnifiedCharSelectReq->szPassword, m_szSucceededPassword))
    {
        ERRLOG3(g_Log, "UID:%10u / ĳ���� ���� ���� : ��й�ȣ ���� ����. %s / %s",
            m_dwUID, lpPktUnifiedCharSelectReq->szPassword, m_szSucceededPassword);

        usError = PktUnifiedCharSelectAck::PASSWORD_FAILED;
    }
    else
    {
        GET_SINGLE_DISPATCH(lpAuthAgentDispatch, CAuthAgentDispatch,
            CAuthAgentDispatch::GetDispatchTable());

        if(0 == lpAuthAgentDispatch)
        {
            usError = PktUnifiedCharSelectAck::SERVER_ERROR;
        }
        else
        {   
            CSendStream& DBASendStream = lpAuthAgentDispatch->GetSendStream();

            char* lpBuffer = DBASendStream.GetBuffer(sizeof(PktUnifiedCharSelectReq));
            if (0 == lpBuffer)
            {
                ERRLOG1(g_Log, "Session:0x%p Buffer allocate failed.", &GetSession());
                usError = PktUnifiedCharSelectAck::SERVER_ERROR;
            }
            else
            {
                unsigned long dwRequestKey = CServerRequest::GetInstance().AddRequest(
                    this, lpAuthAgentDispatch, 100);

                if (0 != dwRequestKey)
                {
                    PktUnifiedCharSelectReq* lpPktSendInfo = 
                        reinterpret_cast<PktUnifiedCharSelectReq*>(lpBuffer);

                    *lpPktSendInfo = *lpPktUnifiedCharSelectReq;
                    lpPktSendInfo->dwUID = m_dwUID;
                    lpPktSendInfo->dwRequestKey = dwRequestKey;

                    if(DBASendStream.WrapHeader(sizeof(PktUnifiedCharSelectReq), CmdUnifiedCharSelect, 0, 0))
                    {
                        DETLOG4(g_Log, "SS:0x%p/DP:0x%p/IP:%15s/RequestKey:%d/DBRequest Send success.", 
                            &GetSession(), this, GetRemoteAddr().get_addr_string(), dwRequestKey);
                    }
                    else
                    {
                        usError = PktUnifiedCharSelectAck::SERVER_ERROR;
                        CServerRequest::GetInstance().RemoveRequest(dwRequestKey);
                    }                    
                }
            }
        }
    }

    if(0 != usError)
    {
        AuthSendPacket::SendUnifiedCharSelectAck(m_SendStream, m_dwUID, 0, usError);
    }

    return true;
}