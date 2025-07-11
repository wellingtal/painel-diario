#include "stdafx.h"

#include <Network/Session/Session.h>
#include <Network/Winsock/SocketFactory.h>
#include <Network/Dispatch/ServerRequest.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/WrapPacket.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>
#include <Network/Packet/PacketStruct/ServerPacket.h>
#include <Network/Packet/PacketStruct/UnifiedCharPacket.h>
#include <Network/Packet/PacketStruct/AuthServerToDBAgentServer.h>

#include <Network/Packet/PacketStruct/PartyPacket.h>
#include <Network/Packet/PacketStruct/GuildPacket.h>

#include <Stream/Buffer/BufferFactory.h>
#include <Utility/Setup/ServerSetup.h>
#include <Creature/Character/CharacterCreate.h>

#include <Item/ItemStructure.h>
#include <Item/ItemConstants.h>
#include <Item/ItemFactory.h>
#include <Item/Item.h>

#include <Log/ServerLog.h>
#include <Log/CharacterLog.h>
#include <Log/LogCommands.h>

#include "AuthAgentDispatch.h"
#include "AuthClientDispatch.h"
#include "AuthAgentServerTable.h"
#include "ItemCreator.h"
#include "SendUserLoginout.h"

// forward decl.
bool SendPartyLeaveToDBAgent(CSendStream& SendStream, unsigned long dwPID, unsigned long dwCID, const char* szName);
bool SendGuildLeaveToDBAgent(CSendStream& SendStream, unsigned long dwGID, unsigned long dwCID, const char* szName);

CAuthAgentDispatch::CAuthAgentDispatch(CSession& Session)
:	CRylServerDispatch(Session, 1000)
{


}

CAuthAgentDispatch::~CAuthAgentDispatch()	
{
    DETLOG1(g_Log, "DP:0x%p/Destroy AuthAgentDispatch", this);
}


CSingleDispatch& CAuthAgentDispatch::GetDispatchTable()
{
	static CSingleDispatch singleDispatchTable;
	return singleDispatchTable;
}


void CAuthAgentDispatch::Connected()
{
	// ���� �����ÿ� ȣ���Ѵ�(Connected �������̵�)
    SendSysServerLogin(CServerSetup::GetInstance().GetServerID());
	GetDispatchTable().SetDispatch(this);

    DETLOG2(g_Log, "SS:0x%p/DP:0x%p/Connected CAuthAgentDispatch", &GetSession(), this);
}

void CAuthAgentDispatch::Disconnected()
{
	GetDispatchTable().RemoveDispatch(this);
    DETLOG2(g_Log, "SS:0x%p/DP:0x%p/Disconnected CAuthAgentDispatch", &GetSession(), this);
}


bool CAuthAgentDispatch::DispatchPacket(PktBase* lpPktBase)
{
	// DBAgent���� ���� ��Ŷ�� ó���ϴ� ��ƾ�̴�.

    switch (lpPktBase->GetCmd())
    {
		case CmdSysServerLogin:         return ParseSysServerLogin(static_cast<PktSLAck*>(lpPktBase));
		case CmdSysServerVerUpdate:     return ParseSysServerVerUpdate(static_cast<PktSVU*>(lpPktBase));
		case CmdSysChannelUpdate:       return ParseSysChannelUpdate(static_cast<PktSCInfo*>(lpPktBase));    
		case CmdDBGetData:              return ParseDBGetData(static_cast<PktDD*>(lpPktBase));
		case CmdAgentZone:			    return ParseAgentZone(static_cast<PktDD*>(lpPktBase));
		case CmdUserKill:               return ParseUserKill(static_cast<PktUK*>(lpPktBase));
		case CmdBillingTimeoutNotify:   return ParseBillingTimeoutNotify(static_cast<PktBTN*>(lpPktBase));
		case CmdBillingTimeCheckNotify:	return ParseBillingTimeCheckNotify(static_cast<PktBTN*>(lpPktBase));

        case CmdUnifiedCharInfo:        return ParseUnifiedCharInfo(static_cast<PktUnifiedCharInfo*>(lpPktBase));
        case CmdUnifiedCharSelect:      return ParseUnifiedCharSelectAck(static_cast<PktUnifiedCharSelectAck*>(lpPktBase));

		// ������ ���������� �࿩�� �������� ������ �𸥴�.
		case CmdGuildCmd:				return true;

		default:        
			LogErrorPacket("Invalid packet command(AuthAgentDispatch can't process this packet)",
				lpPktBase->GetCmd());
			break;
    }

	// �߸��� ��Ŷ�� ������ ������ ���� �ʴ´�.
    return true;
}


bool CAuthAgentDispatch::ParseSysServerLogin(PktSLAck* lpPktSLAck)
{	
    // ������ �ø��� �ʱ�ȭ
    CItemCreator::GetInstance().SetUID(lpPktSLAck->m_dlItemUID);    

    // ���� ���� ���.
    RylServerInfo   ServerInfo;
    
	ServerInfo.m_dwClientVer    = lpPktSLAck->m_dwClientVer;
    ServerInfo.m_dwChecksum     = lpPktSLAck->m_dwCheckSum;
		
    strncpy(ServerInfo.m_szPatchAddress, lpPktSLAck->m_PatchAddress, 
        RylServerInfo::PATCH_ADDRESS_LENGTH - 1);
    
    ServerInfo.m_szPatchAddress[RylServerInfo::PATCH_ADDRESS_LENGTH - 1] = '\0';
    ServerInfo.m_nPatchAddressLen = strlen(ServerInfo.m_szPatchAddress);

    // ������ ���.
	CAuthAgentServerTable::GetInstance().SetAgentServerInfo(ServerInfo);
    return true;
}


bool CAuthAgentDispatch::ParseSysChannelUpdate(PktSCInfo* lpPktSCInfo)
{
    CAuthAgentServerTable::GetInstance().UpdateChannelUserNum(
		lpPktSCInfo->m_usChannelClientNum, lpPktSCInfo->m_cChannelNum);

    return true;
}

bool CAuthAgentDispatch::ParseSysServerVerUpdate(PktSVU* lpPktSVU)
{   
    CAuthAgentServerTable::GetInstance().UpdateServerData(lpPktSVU->m_dwCheckSum, 
		lpPktSVU->m_dwClientVer, lpPktSVU->m_PatchAddress);

    return true;
}


bool CAuthAgentDispatch::ParseDBGetData(PktDD* lpPktDD)
{
	// WORK_LIST 2.3 ���� ���� ���� ��� ����
	bool bRemoveRequestKey = true;
	if ( PktDD::SCmdNationChangeResult == lpPktDD->m_wCmd )
	{
		bRemoveRequestKey = false;
	}

	CServerRequest::Result result(lpPktDD->m_dwRequestKey, bRemoveRequestKey);

	CAuthClientDispatch* lpAuthClientDispatch = 
		static_cast<CAuthClientDispatch*>(result.GetDispatch());

	if (0 == lpAuthClientDispatch)
	{
		ERRLOG2(g_Log, "sCmd:0x%02x/RequestKey:%d/DBRequest Failed.",
			lpPktDD->m_wCmd, lpPktDD->m_dwRequestKey);

		return true;	
	}

	CSession* lpSession = result.GetSession();
	if (0 == lpSession)
    {
		ERRLOG2(g_Log, "sCmd:0x%02x/RequestKey:%d/Get Session Failed.",
			lpPktDD->m_wCmd, lpPktDD->m_dwRequestKey);

		return true;
	}

    DETLOG5(g_Log, "SS:0x%p/DP:0x%p/IP:%15s/sCmd:0x%02x/RequestKey:%d/DBRequest Success.",
        &lpAuthClientDispatch->GetSession(), lpAuthClientDispatch,
        lpAuthClientDispatch->GetRemoteAddr().get_addr_string(),
		lpPktDD->m_wCmd, lpPktDD->m_dwRequestKey);

	switch(lpPktDD->m_wCmd)
	{
		case PktDD::SCmdStartSession:			// �������� (���� ������ ����)
		{
			PktSSD*         lpPktSSD            = static_cast<PktSSD*>(lpPktDD);
			unsigned long   UserID	            = lpPktSSD->m_dwUserID;
			unsigned long   SessionID           = lpPktSSD->m_dwSessionID;
			
            unsigned short  wError              = lpPktSSD->GetError();

            unsigned char   cAgentServerType    = lpPktSSD->m_cAgentServerType; // �߰輭�� Ÿ�� (Part1:0 / Part1Unified:1 / Part2:2 / Part2Unified:3 / Part2Selectable:4
            unsigned char   cFirstLogin         = lpPktSSD->m_cFirstLogin;      // ���� �α��� ���� (���� �α���:0 / ������ �α���:1)


			// ���� �α���(���� ����)
			if (wError == 0)
			{
				lpAuthClientDispatch->UserLogin(SessionID, UserID);

                if (lpAuthClientDispatch->GetSession().CalledDisconnected())
                {
                    // �̹� ������ ��������, Disconnected�� ȣ��������
                    // ������ �α׾ƿ���Ŵ.
                    lpAuthClientDispatch->UserLogout();
                }
                else
                {
                    // ���ռ��� ���� ����
                    lpAuthClientDispatch->SetUnifiedInfo(cAgentServerType, cFirstLogin);

                    // ������ ���� ���� ��Ŷ�� ����.
                    lpAuthClientDispatch->SetPacketProcessFlag(CAuthClientDispatch::AUTHORIZED_SUCCESS);
                }
			}

            if (!AuthSendPacket::SendAuthAccount(lpAuthClientDispatch, UserID, wError))
            {
                // �ٽ� ���� ��Ŷ�� ���� �� ����.
                lpAuthClientDispatch->ResetPacketProcessFlag(CAuthClientDispatch::AUTHORIZED_SUCCESS);
            }
			break;
		}

		case PktDD::SCmdUserLogin:
		{ 
            PktULDAck*      lpULDAck        = static_cast<PktULDAck*>(lpPktDD);

			unsigned long	UserID		    = lpULDAck->m_dwUserID;
			CHAR_VIEW*		lpCharView	    = lpULDAck->m_CharView;
			sGuildData*		lpGuildData		= lpULDAck->m_GuildData;
            int             nPlayTime       = lpULDAck->m_nPlayTime;		// BillingType�� ������ ���� ���� ��¥. ������ ���� ���� �ð�(��)
            unsigned short  AdminLevel      = lpULDAck->m_usAdminLevel;
            unsigned char   cBillingType    = lpULDAck->m_cBillingType;		// D : ���� T : ����   0Ȥ��N : ����
            unsigned char   cBillingUser    = lpULDAck->m_cBillingUser;		// 0 : ���� 1 : �ǽù�
			unsigned char	cLoginType		= lpULDAck->m_cLoginType;
			unsigned long	dwFlag			= lpULDAck->m_sStoreInfo.Flag;

			// WORK_LIST 2.1 ���� ���� �߰�
			unsigned char	cAccountNation	= lpULDAck->m_cAccountNation;	// ���� ����

			lpAuthClientDispatch->SetCharacterView(lpCharView);

			// Store Info ���� //

			STORE_INFO* pStoreInfo = lpAuthClientDispatch->GetStoreInfo();

			memset(pStoreInfo, 0, sizeof(STORE_INFO));
			memcpy(pStoreInfo, &lpULDAck->m_sStoreInfo, sizeof(STORE_INFO));
			
			// WORK_LIST 2.1 ���� ���� �߰�
            AuthSendPacket::SendUserLogin(lpAuthClientDispatch->GetSendStream(), lpAuthClientDispatch->GetSessionID(), 
                UserID, lpCharView, lpGuildData, dwFlag, cAccountNation, nPlayTime, AdminLevel, cBillingType,
				cBillingUser, cLoginType, lpULDAck->GetError());	
			break;
		} 

		// WORK_LIST 2.1 ���� ���� �߰�
		case PktDD::SCmdSelectNation:
		{
			PktSAND* lpPktSAND = static_cast<PktSAND*>(lpPktDD);

			CAuthClientDispatch::SendSelectAccountNation(lpAuthClientDispatch->GetSendStream(), lpPktSAND->m_dwUserID,
				lpPktSAND->m_cType, lpPktSAND->m_cAccountNation, lpPktSAND->GetError());

			break;
		}

		// WORK_LIST 2.3 ���� ���� ���� ��� ����
		case PktDD::SCmdNationChangeResult:
		{
			PktANCAck* lpPktANCAck = static_cast<PktANCAck*>(lpPktDD);

			CAuthClientDispatch::SendNationChangeResult(lpAuthClientDispatch->GetSendStream(), lpPktANCAck->m_dwUserID,
				lpPktANCAck->m_dwGID, lpPktANCAck->m_dwFame, 0);
			break;
		}

		case PktDD::SCmdCharCreate:
		{
			PktCCDAck* lpPktCCDAck = static_cast<PktCCDAck*>(lpPktDD);

            CAuthClientDispatch::SendCharCreate(lpAuthClientDispatch->GetSendStream(), lpPktCCDAck->m_dwCharID, 
                lpPktCCDAck->m_dwSlot, &lpPktCCDAck->m_CharView, lpPktCCDAck->GetError());

			if (0 == lpPktCCDAck->GetError()) 
			{
                SendCreateCharEquipItem(lpPktCCDAck->m_dwCharID, 
                    lpPktCCDAck->m_CharView.Equip[Item::EquipmentPos::SHIRT],
					lpPktCCDAck->m_CharView.Equip[Item::EquipmentPos::ARMOUR],
                    lpPktCCDAck->m_CharView.Equip[Item::EquipmentPos::BOOTS],
					lpPktCCDAck->m_CharView.Equip[Item::EquipmentPos::WEAPON_HAND1]);
			}

            lpAuthClientDispatch->ResetPacketProcessFlag(CAuthClientDispatch::PENDED_CHAR_CREATE);

			GAMELOG::LogCharCreateDelete(lpAuthClientDispatch->GetUID(), 
				&lpSession->GetRemoteAddr().get_addr_in(), 
				lpPktCCDAck->m_dwCharID, GAMELOG::CMD::CHAR_CREATE, lpPktCCDAck->GetError());
			break;
		}

		case PktDD::SCmdCharDelete:	
		{
			PktCDD* lpPktCDD = static_cast<PktCDD*>(lpPktDD);
			
			GET_SINGLE_DISPATCH(lpAuthAgentDispatch, CAuthAgentDispatch,
				CAuthAgentDispatch::GetDispatchTable());

			// ������ ������ ��Ƽ, ��忡�� ����
			if(!lpPktCDD->GetError())
			{
				CHAR_VIEW& CurrentCharView = lpAuthClientDispatch->GetCharacterView(lpPktCDD->m_dwSlotNum);				

				if(lpAuthAgentDispatch)
				{
					if(CurrentCharView.PID)
					{	
						// ��Ƽ ����� ��� ��Ƽ���� Ż��
						SendPartyLeaveToDBAgent(lpAuthAgentDispatch->GetSendStream(), 
							CurrentCharView.PID, lpPktCDD->m_dwCharID, CurrentCharView.Name);
					}

					if(CurrentCharView.GID)
					{
						// ��� ����� ��� ��忡�� Ż��
						SendGuildLeaveToDBAgent(lpAuthAgentDispatch->GetSendStream(), 
							CurrentCharView.GID, lpPktCDD->m_dwCharID, CurrentCharView.Name);
					}
				}
			}			

			CAuthClientDispatch::SendCharDelete(lpAuthClientDispatch->GetSendStream(), lpPktCDD->m_dwUserID, 
                lpPktCDD->m_dwSlotNum, lpPktCDD->GetError());

            lpAuthClientDispatch->ResetPacketProcessFlag(CAuthClientDispatch::PENDED_CHAR_DELETE);

			GAMELOG::LogCharCreateDelete(lpAuthClientDispatch->GetUID(), 
				&lpSession->GetRemoteAddr().get_addr_in(), lpPktCDD->m_dwCharID, GAMELOG::CMD::CHAR_DELETE, lpPktCDD->GetError());

			break;
		} 

		case PktDD::SCmdCharSelect:
		{
			PktCSDAck* lpPktCSDAck = static_cast<PktCSDAck*>(lpPktDD);

            lpAuthClientDispatch->ResetPacketProcessFlag(
                CAuthClientDispatch::PENDED_CHAR_SELECT);

            unsigned long dwChannelCount = std::accumulate(lpPktCSDAck->m_wChannelNum,
                lpPktCSDAck->m_wChannelNum + PktCSDAck::MAX_CHANNEL_NUM, 0);

            if (0 == lpPktCSDAck->GetError() && 0 < dwChannelCount)
            {
                lpAuthClientDispatch->SetPacketProcessFlag(
                    CAuthClientDispatch::CHAR_SELECT_SUCCESS);
            }            

			if (!CAuthClientDispatch::SendCharSelect(lpAuthClientDispatch->GetSendStream(), lpPktCSDAck->m_cZone, 
                lpPktCSDAck->m_wChannelNum, lpPktCSDAck->GetError()))
            {
                lpAuthClientDispatch->ResetPacketProcessFlag(
                    CAuthClientDispatch::CHAR_SELECT_SUCCESS);
            }
			break;
		}

		default:
		{
			ERRLOG2(g_Log, "DB�κ��� �̻��� ��Ŷ�� ���Խ��ϴ�. Ŀ�ǵ�� %x, ���� ���� �����ʹ� 0x%p �Դϴ�.",
				lpPktDD->GetCmd(), lpSession);
			break;
		}
	}

    return true;
}

bool SendPartyLeaveToDBAgent(CSendStream& SendStream, unsigned long dwPID, 
							 unsigned long dwCID, const char* szName)
{
	char* lpBuffer = SendStream.GetBuffer(sizeof(PktPMD));

	if (0 != lpBuffer)
	{
		PktPMD* lpPktPMD = reinterpret_cast<PktPMD *>(lpBuffer);

		lpPktPMD->InitPtSubCmd(0, PktCCD::SCmdDeletePartyMem);

		lpPktPMD->m_dwPartyID	= dwPID;
		lpPktPMD->m_dwSenderID	= dwCID;
		lpPktPMD->m_dwGID		= dwCID;

		if(0 != szName)
		{
			strncpy(lpPktPMD->m_strSenderName, szName, CHAR_INFOST::MAX_NAME_LEN);
		}
		else
		{
			memset(lpPktPMD->m_strSenderName, 0, CHAR_INFOST::MAX_NAME_LEN);
		}

		return SendStream.WrapHeader(sizeof(PktPMD), CmdAgentParty, 0, 0);
	}

	return false;
}

bool CAuthAgentDispatch::ParseAgentZone(PktDD* lpPktDD)
{
	CServerRequest::Result result(lpPktDD->m_dwRequestKey);
	CAuthClientDispatch* lpAuthClientDispatch = 
		static_cast<CAuthClientDispatch*>(result.GetDispatch());

	if (0 == lpAuthClientDispatch)
	{
		ERRLOG2(g_Log, "sCmd:0x%02x/RequestKey:%d/DBRequest Failed.",
			lpPktDD->m_wCmd, lpPktDD->m_dwRequestKey);
		return true;	
	}

	CSession* lpSession = result.GetSession();
	if (0 == lpSession)
    {
		ERRLOG2(g_Log, "sCmd:0x%02x/RequestKey:%d/Get Session Failed.",
			lpPktDD->m_wCmd, lpPktDD->m_dwRequestKey);
		return true;
	}

	DETLOG4(g_Log, "SS:0x%p/DP:0x%p/sCmd:0x%02x/RequestKey:%d/DBRequest Success.",
        &lpAuthClientDispatch->GetSession(), lpAuthClientDispatch,
		lpPktDD->m_wCmd, lpPktDD->m_dwRequestKey);

	switch(lpPktDD->m_wCmd)
	{
	case PktDD::SCmdServerZone:
		{ 
			PktSAAck* lpPktSAAck = static_cast<PktSAAck*>(lpPktDD);

            lpAuthClientDispatch->ResetPacketProcessFlag(
                CAuthClientDispatch::PENDED_SERVER_ZONE);

            if (0 == lpPktSAAck->GetError())
            {
			    // ���� �α� �ƿ� ó��
			    // (... Ŭ���̾�Ʈ�� �ͼ� �ٴ� �ͺ��� �̰� �ʰ� ���� �뷫 ����.)
			    lpAuthClientDispatch->UserMove();

                lpAuthClientDispatch->SetPacketProcessFlag(
                    CAuthClientDispatch::SERVER_ZONE_SUCCESS);
            }
            else
            {
                lpAuthClientDispatch->ResetPacketProcessFlag(
                    CAuthClientDispatch::CHAR_SELECT_SUCCESS);
            }

            lpAuthClientDispatch->SetMoveAddress(lpPktSAAck->m_dwServerID, 
                INET_Addr(lpPktSAAck->m_GameAddress, CServerSetup::GetGameServerTCPPort(lpPktSAAck->m_dwServerID)));

            if (0 != lpPktSAAck->GetError())
            {
                if (!AuthSendPacket::SendServerZone(lpAuthClientDispatch->GetSendStream(), lpPktSAAck->m_dwServerID,
                    lpAuthClientDispatch->GetMoveAddress().get_addr_in(), lpPktSAAck->GetError()))
                {
                    lpAuthClientDispatch->ResetPacketProcessFlag(
                        CAuthClientDispatch::SERVER_ZONE_SUCCESS);
                }
            }
            else
            {
  
	            char* lpBuffer = GetSendStream().GetBuffer(sizeof(PktDD));
		        if (0 != lpBuffer)
                {
			        unsigned long dwRequestKey = CServerRequest::GetInstance().AddRequest(
                        lpAuthClientDispatch, this, 100);

                    if (0 != dwRequestKey)
                    {
                        PktDD* lpPktDD = reinterpret_cast<PktDD*>(lpBuffer);
                        lpPktDD->InitPtSubCmd(dwRequestKey, PktDD::SCmdServerZoneEnd);

				        if (GetSendStream().WrapHeader(sizeof(PktDD), CmdAgentZone, 0, 0))
				        {
                            DETLOG5(g_Log, "SS:0x%p/DP:0x%p/IP:%15s/sCmd:0x%02x/RequestKey:%d/DBRequest Send success.",
						        &lpAuthClientDispatch->GetSession(), lpAuthClientDispatch,
                                lpAuthClientDispatch->GetRemoteAddr().get_addr_string(),
                                PktSA::SCmdServerZoneEnd, dwRequestKey);

                            break;
				        }
                        else
                        {
                            CServerRequest::GetInstance().RemoveRequest(dwRequestKey);
                        }
                    }
                }
            }

            break;
        }

    case PktDD::SCmdServerZoneEnd:

		if (!AuthSendPacket::SendServerZone(lpAuthClientDispatch->GetSendStream(), 
            lpAuthClientDispatch->GetMoveServerID(), lpAuthClientDispatch->GetMoveAddress().get_addr_in(), 0))
        {
            lpAuthClientDispatch->ResetPacketProcessFlag(
                CAuthClientDispatch::SERVER_ZONE_SUCCESS);
        }
		break;

	default:

		ERRLOG2(g_Log, "DB�κ��� �̻��� ��Ŷ�� ���Խ��ϴ�. Ŀ�ǵ�� %x, ���� ���� �����ʹ� 0x%p �Դϴ�.", 
			lpPktDD->GetCmd(), lpSession);
		return true;
	}

    return true;
}

bool CAuthAgentDispatch::ParseUserKill(PktUK* lpPktUK)
{
	// ���� ���

	GET_MULTI_DISPATCH(lpAuthClientDispatch, lpPktUK->m_dwUserID, 
		CAuthClientDispatch, CAuthClientDispatch::GetDispatchTable());

	if (0 == lpAuthClientDispatch)
	{	
		ERRLOG1(g_Log, "UID:%u/GetUser failed.", lpPktUK->m_dwUserID);	
		SendUserKill(lpPktUK->m_dwUserID, 2);
	}
	else
	{
        // ���� Ŭ����
		ERRLOG3(g_Log, "SS:0x%p/DP:0x%p/UID:%u/Process UserKill.", 
			&lpAuthClientDispatch->GetSession(), 
			lpAuthClientDispatch, lpPktUK->m_dwUserID);

		lpAuthClientDispatch->Shutdown();
	}

	return true;
}

bool CAuthAgentDispatch::SendUserKill(unsigned long dwUserID, unsigned short usError)
{
    char* lpBuffer = GetSendStream().GetBuffer(sizeof(PktUK));
    if (0 != lpBuffer)
    {	    
	    PktUK* lpPktUK = reinterpret_cast<PktUK*>(lpBuffer);
        
		lpPktUK->m_dwServerID	= CServerSetup::GetInstance().GetServerID();
		lpPktUK->m_dwUserID		= dwUserID;
		lpPktUK->m_dwCharID		= 0;

        return GetSendStream().WrapCrypt(sizeof(PktUK), CmdUserKill, 0, usError);
    }
	
	return false;
}

bool CAuthAgentDispatch::SendSysServerLogin(unsigned long ServerID_In)
{
    char* lpBuffer = GetSendStream().GetBuffer(sizeof(PktSL));
    if (0 != lpBuffer)
    {	    
	    PktSL* lpPktSL = reinterpret_cast<PktSL*>(lpBuffer);
        
        const int   MAX_ADDRESS_SIZE = 128;
	    char        szAddress[MAX_ADDRESS_SIZE];

        GetSession().GetPolicy().GetSocketFactory().GetNetworkInfo(szAddress, MAX_ADDRESS_SIZE);
	    
	    lpPktSL->m_Address.S_un.S_addr	= inet_addr(szAddress);
	    lpPktSL->m_dwServerID			= ServerID_In;

        return GetSendStream().WrapHeader(sizeof(PktSL), CmdSysServerLogin, 0, 0);
    }
	
    return false;
}

bool CAuthAgentDispatch::SendStartSession(CAuthClientDispatch* lpDispatch_In, unsigned long dwSessionID_In, 
                                          const char* AccountName_In, const char* Password_In, 
                                          unsigned long dwUserID_In, unsigned short usCnFlag_In)
{
	GET_SINGLE_DISPATCH(lpAuthAgentDispatch, CAuthAgentDispatch, 
		CAuthAgentDispatch::GetDispatchTable());

	if (0 != lpAuthAgentDispatch && 0 != lpDispatch_In)
    {		
        CSendStream& SendStream = lpAuthAgentDispatch->GetSendStream();
		char* lpBuffer = SendStream.GetBuffer(sizeof(PktSSD));
        if (0 != lpBuffer)
        {
			unsigned long dwRequestKey = CServerRequest::GetInstance().AddRequest(
				lpDispatch_In, lpAuthAgentDispatch, 100);

            if (0 != dwRequestKey)
            {
				// edith 2008.01.15 Auth���� Agent�� ���ǽ��� ��Ŷ�� ������.??
				// PktSSD �� �н����带 �߰��ؼ� Agent�� �������� �ؾ��Ѵ�. �Լ��� �����ؾ���.
                PktSSD* lpPktSSDPt = reinterpret_cast<PktSSD*>(lpBuffer);
			    lpPktSSDPt->InitPtSubCmd(dwRequestKey, PktSSD::SCmdStartSession);

			    lpPktSSDPt->m_dwSessionID	= dwSessionID_In;
			    lpPktSSDPt->m_dwUserID      = dwUserID_In;

				if (0 != AccountName_In && strlen(AccountName_In) > 0)
                {
                    memcpy(lpPktSSDPt->m_AccountName, AccountName_In, sizeof(char) * PktSSD::MaxName);
                }
				else
                    ZeroMemory(lpPktSSDPt->m_AccountName, sizeof(char) * PktSSD::MaxName);

				if (0 != Password_In && strlen(Password_In) > 0)
                {
                    memcpy(lpPktSSDPt->m_Password, Password_In, sizeof(char) * PktSSD::MaxPass);
                }
				else
					ZeroMemory(lpPktSSDPt->m_Password, sizeof(char) * PktSSD::MaxPass);
			    
			    lpPktSSDPt->m_Address = lpDispatch_In->GetSession().GetRemoteAddr().get_addr_in().sin_addr;

			    if (SendStream.WrapCrypt(sizeof(PktSSD), CmdDBGetData, 0, usCnFlag_In))
                {
					INFLOG4(g_Log, "UID:%10u / DP:0x%p / SessionID:%10u / Account:%s / �α��� ����. DB�߰�� ���� ���� ����",
                        dwUserID_In, lpDispatch_In, dwSessionID_In, AccountName_In);

                    DETLOG5(g_Log, "SS:0x%p/DP:0x%p/IP:%15s/sCmd:0x%02x/RequestKey:%d/DBRequest Send success.",
						&lpDispatch_In->GetSession(), lpDispatch_In,
                        lpDispatch_In->GetRemoteAddr().get_addr_string(),
						PktSSD::SCmdStartSession, dwRequestKey);

                    return true;
                }
            }
        }

	}

	if (lpDispatch_In)
	{
        AuthSendPacket::SendAuthAccount(lpDispatch_In, lpDispatch_In->GetUID(), 1);
	}

	return false;
}

bool CAuthAgentDispatch::SendCreateCharEquipItem(unsigned long CharID_In, unsigned short Shirt_In,
												 unsigned short Armour_In, unsigned short Boots_In, unsigned short Weapon_In)
{
    const size_t nMaxPacketLen = sizeof(PktCCID) + Item::MAX_ITEM_SIZE * 4;
	char* lpBuffer = GetSendStream().GetBuffer(nMaxPacketLen);

    if (NULL != lpBuffer)
    {
        char*       lpItemPos = lpBuffer + sizeof(PktCCID);
        PktCCID*    lpPktCCID = reinterpret_cast<PktCCID*>(lpBuffer);

	    size_t nSize = 0;
	    size_t nTotalSize = 0;

	    // ������ ����
        CItemCreator& ItemCreator = CItemCreator::GetInstance();
        
		Item::CItem* lpShirt	= (0 != Shirt_In) ? ItemCreator.Create(Shirt_In) : 0;
		Item::CItem* lpAmour	= (0 != Armour_In) ? ItemCreator.Create(Armour_In) : 0;
		Item::CItem* lpBoots	= (0 != Boots_In) ? ItemCreator.Create(Boots_In) : 0;
		Item::CItem* lpWeapon	= (0 != Weapon_In) ? ItemCreator.Create(Weapon_In) : 0;

		Item::CEquipment* lpEquip = NULL;

		lpEquip = Item::CEquipment::DowncastToEquipment(lpShirt);
		if (NULL != lpEquip)	{ lpEquip->SetNewEquip();	}

		lpEquip = Item::CEquipment::DowncastToEquipment(lpAmour);
		if (NULL != lpEquip)	{ lpEquip->SetNewEquip();	}

		lpEquip = Item::CEquipment::DowncastToEquipment(lpBoots);
		if (NULL != lpEquip)	{ lpEquip->SetNewEquip();	}

		lpEquip = Item::CEquipment::DowncastToEquipment(lpWeapon);
		if (NULL != lpEquip)	{ lpEquip->SetNewEquip();	}

        // ���� ����
	    if (NULL  != lpShirt)
	    {	
			nSize = nMaxPacketLen - nTotalSize;
			lpShirt->MoveItem(Item::ItemPos(1, Item::EquipmentPos::SHIRT));
			lpShirt->SerializeOut(lpItemPos, nSize);

			lpItemPos += nSize;
		    nTotalSize += nSize;

			DELETE_ITEM(lpShirt);
		}

		// �Ƹ� ����
		if (NULL  != lpAmour)
		{	
			nSize = nMaxPacketLen - nTotalSize;
			lpAmour->MoveItem(Item::ItemPos(1, Item::EquipmentPos::ARMOUR));
			lpAmour->SerializeOut(lpItemPos, nSize);

			lpItemPos += nSize;
			nTotalSize += nSize;

			DELETE_ITEM(lpAmour);
		}
    	
        // ���� ����
	    if (NULL  != lpBoots)
	    {	
			nSize = nMaxPacketLen - nTotalSize;
			lpBoots->MoveItem(Item::ItemPos(1, Item::EquipmentPos::BOOTS));
			lpBoots->SerializeOut(lpItemPos, nSize);

		    lpItemPos += nSize;
		    nTotalSize += nSize;

			DELETE_ITEM(lpBoots);
	    }

        // �⺻ ���� ����
	    if (NULL  != lpWeapon)
	    {	
			nSize = nMaxPacketLen - nTotalSize;
			lpWeapon->MoveItem(Item::ItemPos(1, Item::EquipmentPos::WEAPON_HAND1));
			lpWeapon->SerializeOut(lpItemPos, nSize);

		    lpItemPos += nSize;
		    nTotalSize += nSize;
			
            DELETE_ITEM(lpWeapon);
	    }
	    
		lpPktCCID->InitPtSubCmd(0, PktCCID::SCmdCharCreateItem);

        lpPktCCID->m_dwItemSerial   = ItemCreator.GetUID();
	    lpPktCCID->m_dwCharID	    = CharID_In;
	    lpPktCCID->m_dwSize         = (unsigned long)nTotalSize;	
        
        return GetSendStream().WrapHeader(
            static_cast<unsigned short>(sizeof(PktCCID) + nTotalSize), CmdDBGetData, 0, 0);
    }

    return false;
}

bool CAuthAgentDispatch::ParseBillingTimeoutNotify(PktBTN* lpPktBTN)
{
	GET_MULTI_DISPATCH(lpAuthClientDispatch, lpPktBTN->m_dwUserID, 
		CAuthClientDispatch, CAuthClientDispatch::GetDispatchTable());

    if (0 == lpAuthClientDispatch)
    {	
        ERRLOG1(g_Log, "UID:0x%08d ���� ��� ����.", lpPktBTN->m_dwUserID);	
    }
	else
	{
		CSendStream& SendStream = lpAuthClientDispatch->GetSendStream();
		char* lpBuffer = SendStream.GetBuffer(sizeof(PktBTN));
		if (0 != lpBuffer)
		{	    
			memcpy(lpBuffer, lpPktBTN, sizeof(PktBTN));
	        
			return SendStream.WrapCrypt(sizeof(PktBTN), CmdBillingTimeoutNotify, 0, 0);
		}
	}

    return false;
}

bool CAuthAgentDispatch::ParseBillingTimeCheckNotify(PktBTN* lpPktBTN)
{
	GET_MULTI_DISPATCH(lpAuthClientDispatch, lpPktBTN->m_dwUserID, 
		CAuthClientDispatch, CAuthClientDispatch::GetDispatchTable());

    if (0 == lpAuthClientDispatch)
    {	
		ERRLOG1(g_Log, "UID:0x%08d ���� ��� ����.", lpPktBTN->m_dwUserID);	
	}
	else
	{
        CSendStream& SendStream = lpAuthClientDispatch->GetSendStream();
		char* lpBuffer = SendStream.GetBuffer(sizeof(PktBTN));

		if (0 != lpBuffer)
		{	    
			memcpy(lpBuffer, lpPktBTN, sizeof(PktBTN));
			return SendStream.WrapCrypt(sizeof(PktBTN), CmdBillingTimeCheckNotify, 0, 0);
		}
	}

	return false;
}


bool CAuthAgentDispatch::ParseUnifiedCharInfo(PktUnifiedCharInfo* lpPktUnifiedCharInfo)
{
    // DB�߰輭������ ���� �����͸� Ŭ���̾�Ʈ�� �Ѱ��ش�.
    GET_MULTI_DISPATCH(lpAuthClientDispatch, lpPktUnifiedCharInfo->dwUID, 
        CAuthClientDispatch, CAuthClientDispatch::GetDispatchTable());

    if(0 != lpAuthClientDispatch)
    {
        // ���ռ��� ������ Ŭ���̾�Ʈ�� �Ѱ��ش�.
        lpAuthClientDispatch->GetSendStream().WrapCompress(
            reinterpret_cast<char*>(lpPktUnifiedCharInfo), lpPktUnifiedCharInfo->GetLen(), 
            CmdUnifiedCharInfo, 0, lpPktUnifiedCharInfo->GetError());
    }

    return true;
}

bool CAuthAgentDispatch::ParseUnifiedCharSelectAck(PktUnifiedCharSelectAck* lpPktUnifiedCharSelectAck)
{
    unsigned long dwRequestKey = lpPktUnifiedCharSelectAck->dwRequestKey;

    CServerRequest::Result result(dwRequestKey);

    CAuthClientDispatch* lpAuthClientDispatch = 
        static_cast<CAuthClientDispatch*>(result.GetDispatch());

    if(0 == lpAuthClientDispatch)
    {
        ERRLOG1(g_Log, "RequestKey:%d/DBRequest Failed.", dwRequestKey);
        return true;	
    }

    CSession* lpSession = result.GetSession();
    if(0 == lpSession)
    {
        ERRLOG1(g_Log, "RequestKey:%d/Get Session Failed.", dwRequestKey);
        return true;
    }

    DETLOG3(g_Log, "SS:0x%p/DP:0x%p/RequestKey:%d/DBRequest Success.",
        &lpAuthClientDispatch->GetSession(), lpAuthClientDispatch, dwRequestKey);

    AuthSendPacket::SendUnifiedCharSelectAck(lpAuthClientDispatch->GetSendStream(),
        lpPktUnifiedCharSelectAck->dwUID, 0, lpPktUnifiedCharSelectAck->GetError());

    return true;
}



