#include "stdafx.h"
#include "SendUserLoginout.h"
#include "AuthAgentServerTable.h"

#include <Creature/Character/CharacterCreate.h>

#include <Network/Session/Session.h>
#include <Utility/Resource/EnsureCleanup.h>

#include <Network/Packet/WrapPacket.h>
#include <Network/Packet/PacketCommand.h>

#include <Network/Packet/PacketStruct/ServerPacket.h>
#include <Network/Packet/PacketStruct/UnifiedCharPacket.h>
#include <Network/Packet/PacketStruct/AuthServerToDBAgentServer.h>
#include <Network/Packet/PacketStruct/ClientToAuthServer.h>

#include <Stream/Buffer/BufferFactory.h>
#include <ctime>

#include "AuthClientDispatch.h"

bool AuthSendPacket::SendAuthAccount(CAuthClientDispatch* lpDispatch, unsigned long UserID_In,                                      
                                     unsigned short usErrorCode_In)
{
    if(0 != lpDispatch)
    {
        lpDispatch->ResetPacketProcessFlag(CAuthClientDispatch::AUTHORIZE_PENDED);

        CSendStream& SendStream = lpDispatch->GetSendStream();
	    char* lpBuffer = SendStream.GetBuffer(sizeof(PktAUAck));
	    if (0 != lpBuffer)
        {
            PktAUAck* lpPktAUAck = reinterpret_cast<PktAUAck*>(lpBuffer);
            lpPktAUAck->m_dwUserID = UserID_In;

			lpPktAUAck->m_ClientA='}';//hz added for client check if ok.

            lpPktAUAck->m_cAgentServerType = lpDispatch->GetAgentServerType();
            lpPktAUAck->m_cFirstLogin = lpDispatch->GetFirstLogin();

            return SendStream.WrapCrypt(sizeof(PktAUAck), CmdAuthAccount, 0, usErrorCode_In);
        }
    }

    return false;
}

// WORK_LIST 2.1 계정 국적 추가
bool AuthSendPacket::SendUserLogin(CSendStream& SendStream, unsigned long SessionID_In, unsigned long UserID_In, CHAR_VIEW* lpCharView_In, 
								   sGuildData* lpGuildData_In, unsigned long dwFlag, unsigned char cAccountNation, int nPlayTime, unsigned short usAdminLevel,
								   unsigned char cBillingType, unsigned char cBillingUser, unsigned char cLoginType, unsigned short usError_In)
{
	char* lpBuffer = SendStream.GetBuffer(sizeof(PktULiAck));
    if (0 != lpBuffer)
    {
        PktULiAck* lpPktULiAck = reinterpret_cast<PktULiAck*>(lpBuffer);
        
        if (0 == usError_In)
        {
			lpPktULiAck->m_dwSessionID  	= SessionID_In;
		    lpPktULiAck->m_dwUserID	    	= UserID_In;

            lpPktULiAck->m_dwCurrentTime	= time(0);
			
            lpPktULiAck->m_nPlayTime    	= nPlayTime;
            lpPktULiAck->m_usAdminLevel 	= usAdminLevel;
            lpPktULiAck->m_cBillingType 	= cBillingType;
            lpPktULiAck->m_cBillingUser 	= cBillingUser;
			
			lpPktULiAck->m_dwTabFlag		= dwFlag;

			lpPktULiAck->m_cLoginType		= cLoginType;

			// WORK_LIST 2.1 계정 국적 추가
			lpPktULiAck->m_cAccountNation	= cAccountNation;

			memcpy(lpPktULiAck->m_CharView, lpCharView_In, sizeof(CHAR_VIEW) * PktULiAck::MAX_CHAR_VIEW);
			memcpy(lpPktULiAck->m_GuildData, lpGuildData_In, sizeof(sGuildData) * PktULiAck::MAX_CHAR_VIEW);
	    }
	    else
	    {
			lpPktULiAck->m_dwSessionID  	= 0;
		    lpPktULiAck->m_dwUserID	    	= 0;

            lpPktULiAck->m_dwCurrentTime	= 0;

            lpPktULiAck->m_nPlayTime    	= 0;
            lpPktULiAck->m_usAdminLevel 	= 0;
            lpPktULiAck->m_cBillingType 	= 0;
            lpPktULiAck->m_cBillingUser 	= 0;
			
			lpPktULiAck->m_cLoginType		= 0;

			// WORK_LIST 2.1 계정 국적 추가
			lpPktULiAck->m_cAccountNation	= 0;

			memset(lpPktULiAck->m_CharView, 0, sizeof(CHAR_VIEW) * PktULiAck::MAX_CHAR_VIEW);
			memset(lpPktULiAck->m_GuildData, 0, sizeof(sGuildData) * PktULiAck::MAX_CHAR_VIEW);
        }

        if (SendStream.WrapCrypt(sizeof(PktULiAck), CmdUserLogin, 0, usError_In))
		{
            return true;
		}
    }

    return false;
}


bool AuthSendPacket::SendUserLogout(unsigned long SessionID_In, unsigned long UserID_In)
{
	GET_SINGLE_DISPATCH(lpAgentDispatch, CAuthAgentDispatch, 
		CAuthAgentDispatch::GetDispatchTable());

	if (0 != lpAgentDispatch)
    {		

        CSendStream& SendStream = lpAgentDispatch->GetSendStream();
        char* lpBuffer = SendStream.GetBuffer(sizeof(PktULoD));
        if (0 != lpBuffer)
        {
            PktULoD* lpPktULoD = reinterpret_cast<PktULoD*>(lpBuffer);            
            
			lpPktULoD->InitPtSubCmd(0, PktDD::SCmdUserLogout);
			
			lpPktULoD->m_dwSessionID	= SessionID_In;
			lpPktULoD->m_dwUserID		= UserID_In;

            return SendStream.WrapCrypt(sizeof(PktULoD), CmdDBGetData, 0, 0);
        }
    }

	return false;
}

bool AuthSendPacket::SendUserMove(unsigned long SessionID_In, unsigned long UserID_In)
{
	GET_SINGLE_DISPATCH(lpAgentDispatch, CAuthAgentDispatch, 
		CAuthAgentDispatch::GetDispatchTable());

	if (0 != lpAgentDispatch)
    {		
        CSendStream& SendStream = lpAgentDispatch->GetSendStream();
		
		char* lpBuffer = SendStream.GetBuffer(sizeof(PktUMvD));
        if (0 != lpBuffer)
        {
            PktUMvD* lpPktUMvD = reinterpret_cast<PktUMvD*>(lpBuffer);            
            
			lpPktUMvD->InitPtSubCmd(0, PktDD::SCmdUserMove);
			
			lpPktUMvD->m_dwSessionID	= SessionID_In;
			lpPktUMvD->m_dwUserID		= UserID_In;

            return SendStream.WrapCrypt(sizeof(PktUMvD), CmdDBGetData, 0, 0);
        }
    }

	return false;
}

bool AuthSendPacket::SendServerZone(CSendStream& SendStream, unsigned long ServerID_In, 
                                    const sockaddr_in& gameServerTCPAddress, unsigned short usError_In)
{
    char* lpBuffer = SendStream.GetBuffer(sizeof(PktSZAck));
    if (0 != lpBuffer)
    {	    
	    PktSZAck* lpPktSZAck = reinterpret_cast<PktSZAck*>(lpBuffer);

		lpPktSZAck->m_dwServerID	    = ServerID_In;
		lpPktSZAck->m_GameServerTCPAddr = gameServerTCPAddress;

        if (SendStream.WrapCrypt(sizeof(PktSZAck), CmdServerZone, 0, usError_In))
		{
            return true;
		}
    }

    return false;
}


bool AuthSendPacket::SendUnifiedCharSelectAck(CSendStream& SendStream, unsigned long dwUID, 
                                              unsigned long dwRequestKey, unsigned short usError_In)
{
    char* lpBuffer = SendStream.GetBuffer(sizeof(PktUnifiedCharSelectAck));
    if (0 != lpBuffer)
    {
        PktUnifiedCharSelectAck* lpPktUnifiedCharSelectAck = 
            reinterpret_cast<PktUnifiedCharSelectAck*>(lpBuffer);

        lpPktUnifiedCharSelectAck->dwUID = dwUID;
        lpPktUnifiedCharSelectAck->dwRequestKey = dwRequestKey;

        if (SendStream.WrapCrypt(sizeof(PktUnifiedCharSelectAck), 
            CmdUnifiedCharSelect, 0, usError_In))
        {
            return true;
        }
    }

    return false;
}