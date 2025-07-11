#include "stdafx.h"
#include "SendServerInfo.h"

#include <Network/Stream/SendStream.h>
#include <Network/Winsock/SocketFactory.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>
#include <Network/Packet/PacketStruct/ServerPacket.h>

#include <Network/Dispatch/LoginDispatch.h>
#include <Network/Dispatch/AuthDispatch.h>
#include <Network/Dispatch/GameDispatch.h>
#include <Network/Dispatch/UIDDispatch.h>

#include <DataStorage/SessionData.h>
#include <Utility/Setup/ServerSetup.h>

#include <Log/ServerLog.h>

namespace DBAgent
{
namespace SendPacket
{

class CSendUpdateServerVersionAllGameServer
{
public:
    bool operator () (unsigned long dwServer, CPacketDispatch& packetDispatch)
    {
        return DBAgent::SendPacket::UpdateServerVersion(
            static_cast<DBAgent::CGameDispatch&>(packetDispatch).GetSendStream());
    }
};

bool ServerLogin(CSendStream& SendStream, unsigned long dwServerID)
{
    char* lpBuffer = SendStream.GetBuffer(sizeof(PktSL));
    if(0 != lpBuffer)
    {
        const int MAX_ADDRESS = 64;
        char szAddress[MAX_ADDRESS];

        CTCPFactory tcpFactory;
        tcpFactory.GetNetworkInfo(szAddress, MAX_ADDRESS);

        PktSL* lpPktSL = reinterpret_cast<PktSL*>(lpBuffer);

        lpPktSL->m_dwServerID           = dwServerID;
        lpPktSL->m_Address.S_un.S_addr  = inet_addr(szAddress);

        return SendStream.WrapHeader(sizeof(PktSL), CmdSysServerLogin, 0, 0);
    }

    return false;
}

bool ServerLoginAck(CSendStream& SendStream, unsigned long dwServerID,
                    unsigned __int64 dwItemSerial)
{
    char* lpBuffer = SendStream.GetBuffer(sizeof(PktSLAck));

    if(0 != lpBuffer)
    {
        PktSLAck* lpPktSLAck = reinterpret_cast<PktSLAck*>(lpBuffer);

        CServerSetup& serverSetup = CServerSetup::GetInstance();

        lpPktSLAck->m_dlItemUID     = dwItemSerial;
        lpPktSLAck->m_dwServerID    = dwServerID;

        lpPktSLAck->m_dwClientVer   = serverSetup.GetClientVer();
        lpPktSLAck->m_dwCheckSum    = serverSetup.GetCheckSum();

        strncpy(lpPktSLAck->m_PatchAddress, 
            serverSetup.GetPatchAddress(), PktSLAck::PATCH_ADDRESS_LENGTH - 1);

        lpPktSLAck->m_PatchAddress[PktSLAck::PATCH_ADDRESS_LENGTH - 1] = 0;

        return SendStream.WrapHeader(sizeof(PktSLAck), CmdSysServerLogin, 0, 0);
    }

    return false;
}



bool UpdateChannel(CSendStream& SendStream, unsigned long dwServerID)
{
    char* lpBuffer = SendStream.GetBuffer(sizeof(PktSCInfo));
    if(0 != lpBuffer)
    {
        PktSCInfo* lpPktSCInfo = reinterpret_cast<PktSCInfo*>(lpBuffer);

        lpPktSCInfo->m_dwServerID = dwServerID;

        CGetTotalCount getCount(
            lpPktSCInfo->m_usChannelClientNum[0], 
            lpPktSCInfo->m_usChannelClientNum[1],
            lpPktSCInfo->m_cChannelNum);
        
        CGameDispatch::GetDispatchTable().Process(getCount);

        if(0 < lpPktSCInfo->m_cChannelNum)
        {
            if(lpPktSCInfo->m_cChannelNum <= lpPktSCInfo->m_usChannelClientNum[0])
            {
                lpPktSCInfo->m_usChannelClientNum[0] -= lpPktSCInfo->m_cChannelNum; 
                ++lpPktSCInfo->m_usChannelClientNum[0];
            }

            if(lpPktSCInfo->m_cChannelNum <= lpPktSCInfo->m_usChannelClientNum[1]) 
            { 
                lpPktSCInfo->m_usChannelClientNum[1] -= lpPktSCInfo->m_cChannelNum; 
                ++lpPktSCInfo->m_usChannelClientNum[1];
            }

            lpPktSCInfo->m_cChannelNum = 2;
        }

        return SendStream.WrapCrypt(sizeof(PktSCInfo), CmdSysChannelUpdate, 0, 0);
    }

    return false;
}


bool UpdateServerVersion(CSendStream& SendStream)
{
    char* lpBuffer = SendStream.GetBuffer(sizeof(PktSVU));
    if(0 != lpBuffer)
    {
        PktSVU* lpPktSVU = reinterpret_cast<PktSVU*>(lpBuffer);

        CServerSetup& serverSetup = CServerSetup::GetInstance();

        lpPktSVU->m_dwClientVer = serverSetup.GetClientVer();
        lpPktSVU->m_dwCheckSum  = serverSetup.GetCheckSum();
        strncpy(lpPktSVU->m_PatchAddress, serverSetup.GetPatchAddress(), 
            PktSVU::PATCH_ADDRESS_LENGTH - 1);

        lpPktSVU->m_PatchAddress[PktSVU::PATCH_ADDRESS_LENGTH - 1] = 0;

        GET_SINGLE_DISPATCH(lpAuthDispatch, 
            CAuthDispatch, CAuthDispatch::GetDispatchTable());

        if(0 != lpAuthDispatch)
        {
            lpPktSVU->m_ServerAddr = lpAuthDispatch->GetRemoteAddr().get_addr_in().sin_addr;
        }
        else
        {
            lpPktSVU->m_ServerAddr.S_un.S_addr = 0;
        }

        return SendStream.WrapCrypt(sizeof(PktSVU), CmdSysServerVerUpdate, 0, 0);
    }

    return false;
}




void UpdateChannelAllServer()
{
    // 로그인 서버로 서버 버전 업데이트 보냄
    GET_SINGLE_DISPATCH(lpLoginDispatch, DBAgent::CLoginDispatch,
        DBAgent::CLoginDispatch::GetDispatchTable());

    if(0 != lpLoginDispatch)
    {
        DBAgent::SendPacket::UpdateServerVersion(lpLoginDispatch->GetSendStream());
    }

    // 인증 서버로 서버 버전 업데이트 보냄
    GET_SINGLE_DISPATCH(lpAuthDispatch, DBAgent::CAuthDispatch,
        DBAgent::CAuthDispatch::GetDispatchTable());

    if(0 != lpAuthDispatch)
    {
        DBAgent::SendPacket::UpdateServerVersion(lpAuthDispatch->GetSendStream());
    }

    // 게임 서버로 서버 버전 업데이트 보냄
    DBAgent::CGameDispatch::GetDispatchTable().Process(
        CSendUpdateServerVersionAllGameServer());
}


bool UpdateUIDTable(PktUUT::UpdateType eUpdateType, const char* szAccountName, const char* szPassword, 
                    unsigned long dwUID, unsigned long dwCID, unsigned long dwSessionID, 
                    unsigned long dwServerID, IN_ADDR peerAddr, unsigned short usError)
{    
    const char* szErrorReason = 0;

    GET_SINGLE_DISPATCH(lpUIDDispatch, CUIDDispatch, CUIDDispatch::GetDispatchTable());

    if(0 != lpUIDDispatch)
    {
		// edith 2008.01.15 UID 서버로 캐릭터 로그인 메시지를 전달한다.
		// 이유는 캐릭터 로그인을 전달해 빌링서버에서 인증을 하기위함이다.
        CSendStream& uidSendStream = lpUIDDispatch->GetSendStream();

        char* lpBuffer = uidSendStream.GetBuffer(sizeof(PktUUT));
        if(0 != lpBuffer)
        {
            PktUUT* lpPktUUT = reinterpret_cast<PktUUT*>(lpBuffer);

            lpPktUUT->m_cCmd		= static_cast<unsigned char>(eUpdateType);

			ZeroMemory(lpPktUUT->m_strAccount, PktUUT::MaxAccountLen);
			ZeroMemory(lpPktUUT->m_strPassword, PktUUT::MaxPasswordLen);

			if(szAccountName && strlen(szAccountName) > 0)
			{
				strcpy(lpPktUUT->m_strAccount, szAccountName);
			}

			// edith 2008.01.15 패스워드 추가작업
			if(szPassword && strlen(szPassword) > 0)
			{
				strcpy(lpPktUUT->m_strPassword, szPassword);
//				strncpy(lpPktUUT->m_strPassword, szPassword, PktUUT::MaxPasswordLen - 1);
//	            lpPktUUT->m_strPassword[PktUUT::MaxPasswordLen - 1] = 0;
			}

            lpPktUUT->m_IPAddress	= peerAddr;

            lpPktUUT->m_dwSessionID	= dwSessionID;
            lpPktUUT->m_dwServerID	= dwServerID;
            lpPktUUT->m_dwUserID	= dwUID;
            lpPktUUT->m_dwCharID	= dwCID;

            if(uidSendStream.WrapHeader(sizeof(PktUUT), CmdUpdateUIDTable, 0, usError))
            {
                INFLOG3(g_Log, "UID:%10u / CID:%10u / ST:%s / UpdateUIDTable 전송 성공", 
                    dwUID, dwCID, g_szPktUUTString[eUpdateType]);

                return true;
            }           
            else
            {
                szErrorReason = "WrapHeader 실패";
            }
        }
        else
        {
            szErrorReason = "버퍼 할당 실패";
        }
    }
    else
    {
        szErrorReason = "UID서버와 연결 끊어짐";
    }

    if(0 != szErrorReason)
    {
        ERRLOG4(g_Log, "UID:%10u / CID:%10u / ST:%s / UpdateUIDTable 전송 실패 : %s", 
            dwUID, dwCID, g_szPktUUTString[eUpdateType], szErrorReason);
    }

    return false;
}
  
bool UserKill(const DataStorage::CSessionData& SessionData)
{
    SERVER_ID serverID;
    serverID.dwID = SessionData.GetServerID();

    CSendStream* lpSendStream = 0;

    if(serverID.GetType() == CServerSetup::AuthServer)
    {
        GET_SINGLE_DISPATCH(lpAuthDispatch, CAuthDispatch, 
            CAuthDispatch::GetDispatchTable());

        if(0 != lpAuthDispatch)
        {
            lpSendStream = &lpAuthDispatch->GetSendStream();
        }
    }    
    else if(serverID.GetType() == CServerSetup::GameServer)
    {
        GET_MULTI_DISPATCH(lpGameDispatch, serverID.dwID, 
            CGameDispatch, CGameDispatch::GetDispatchTable());

        if(0 != lpGameDispatch)
        {
            lpSendStream = &lpGameDispatch->GetSendStream();
        }
    }

    const char* szErrorReason = 0;

    if(0 == lpSendStream)
    {
        szErrorReason = "해당 서버가 없습니다";
    }
    else
    {
        char* lpBuffer = lpSendStream->GetBuffer(sizeof(PktUK));
        if(0 == lpBuffer)
        {
            szErrorReason = "버퍼 할당 실패";
        }
        else
        {
            PktUK* lpPktUK = reinterpret_cast<PktUK*>(lpBuffer);

            lpPktUK->m_dwUserID   = SessionData.GetUID();
            lpPktUK->m_dwCharID   = SessionData.GetCID();
            lpPktUK->m_dwServerID = SessionData.GetServerID();

            if(!lpSendStream->WrapHeader(sizeof(PktUK), CmdUserKill, 0, 0))
            {
                szErrorReason = "전송 실패";
            }
        }
    }

    if(0 != szErrorReason)
    {
        ERRLOG4(g_Log, "UID:%10u / CID:%10u / ServerID:0x%08X / 유저 죽이기 전송 실패 : %s",
            SessionData.GetUID(), SessionData.GetCID(), SessionData.GetServerID(), szErrorReason);
        
        return false;
    }

    INFLOG3(g_Log, "UID:%10u / CID:%10u / ServerID:0x%08X / 유저 죽이기 전송 성공",
        SessionData.GetUID(), SessionData.GetCID(), SessionData.GetServerID());

    return true;
}

bool BillingNotify(const DataStorage::CSessionData& SessionData, 
                   unsigned char cRemainMinute, char cBillingType)
{
    GET_MULTI_DISPATCH(lpGameDispatch, SessionData.GetServerID(), 
        CGameDispatch, CGameDispatch::GetDispatchTable());

    if(0 != lpGameDispatch)
    {
        CSendStream& SendStream = lpGameDispatch->GetSendStream();

        char* lpBuffer = SendStream.GetBuffer(sizeof(PktBTN));
        if(0 != lpBuffer)
        {
            PktBTN* lpPktBTN = reinterpret_cast<PktBTN*>(lpBuffer);

            lpPktBTN->m_dwUserID		= SessionData.GetUID();
            lpPktBTN->m_dwCharID		= SessionData.GetCID();
            lpPktBTN->m_dwServerID	    = SessionData.GetServerID();
            lpPktBTN->m_cRemainMinute   = cRemainMinute;
            lpPktBTN->m_cBillingType    = cBillingType;

            return SendStream.WrapHeader(sizeof(PktBTN), CmdBillingTimeoutNotify, 0, 0);
        }
    }

    return false;
}

bool BillingCheckNotify(const DataStorage::CSessionData& SessionData,
                        unsigned char cRemainMinute, char cBillingType)
{

    SERVER_ID serverID;
    serverID.dwID = SessionData.GetServerID();

    if (CServerSetup::AuthServer == serverID.GetType())
    {
        GET_SINGLE_DISPATCH(lpAuthDispatch, 
            CAuthDispatch, CAuthDispatch::GetDispatchTable());

        CSendStream& SendStream = lpAuthDispatch->GetSendStream();

        char* lpBuffer = SendStream.GetBuffer(sizeof(PktBTN));
        if(0 != lpBuffer)
        {
            PktBTN* lpPktBTN = reinterpret_cast<PktBTN*>(lpBuffer);

            lpPktBTN->m_dwUserID		= SessionData.GetUID();
            lpPktBTN->m_dwCharID		= SessionData.GetCID();
            lpPktBTN->m_dwServerID	    = SessionData.GetServerID();
            lpPktBTN->m_cRemainMinute   = cRemainMinute;
            lpPktBTN->m_cBillingType    = cBillingType;

            return SendStream.WrapHeader(sizeof(PktBTN), CmdBillingTimeCheckNotify, 0, 0);
        }
    }
    else if(CServerSetup::GameServer == serverID.GetType())
    {
        GET_MULTI_DISPATCH(lpGameDispatch, SessionData.GetServerID(), 
            CGameDispatch, CGameDispatch::GetDispatchTable());

        if(0 != lpGameDispatch)
        {
            CSendStream& SendStream = lpGameDispatch->GetSendStream();

            char* lpBuffer = SendStream.GetBuffer(sizeof(PktBTN));
            if(0 != lpBuffer)
            {
                PktBTN* lpPktBTN = reinterpret_cast<PktBTN*>(lpBuffer);

                lpPktBTN->m_dwUserID		= SessionData.GetUID();
                lpPktBTN->m_dwCharID		= SessionData.GetCID();
                lpPktBTN->m_dwServerID	    = SessionData.GetServerID();
                lpPktBTN->m_cRemainMinute   = cRemainMinute;
                lpPktBTN->m_cBillingType    = cBillingType;

                return SendStream.WrapHeader(sizeof(PktBTN), CmdBillingTimeCheckNotify, 0, 0);
            }
        }
    }

    return false;
}



}
}