#include "stdafx.h"
#include "SendAgentPacket.h"
#include "UIDAgentDispatch.h"

#include <Log/ServerLog.h>
#include <Network/Session/Session.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/WrapPacket.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>
#include <Network/Packet/PacketStruct/ServerPacket.h>

#include <Stream/Buffer/BufferFactory.h>

class CSendUIDAgentDispatch
{
public:

    CSendUIDAgentDispatch(const void* lpBuffer, size_t nLength) : m_lpBuffer(lpBuffer), m_nLength(nLength) { }
    ~CSendUIDAgentDispatch() { }

    bool operator() (unsigned long dwGroup, CPacketDispatch& packetDispatch)
    {
        if(0 != m_lpBuffer)
        {
            CBuffer* lpBuffer = CREATE_BUFFER(
                packetDispatch.GetSession().GetPolicy().GetBufferFactory(), m_nLength);

            if(0 != lpBuffer)
            {
                memcpy(lpBuffer->wr_ptr(), m_lpBuffer, m_nLength);
                lpBuffer->wr_ptr(m_nLength);

                packetDispatch.GetSession().SendPending(lpBuffer);
            }
        }

        return true;
    }

private:

    const void* m_lpBuffer;
    size_t      m_nLength;
};


bool AgentSendPacket::SendUserKill(char Group, unsigned long UserID)
{
	GET_MULTI_DISPATCH(lpAgentDispatch, Group, 
		CUIDAgentDispatch, CUIDAgentDispatch::GetDispatchTable());

	if(0 == lpAgentDispatch) 
	{
		ERRLOG1(g_Log, "그룹:%d 연결세션 구하기 실패", Group);
		return false;
	}


    CSendStream& SendStream = lpAgentDispatch->GetSendStream();	
	char* lpBuffer = SendStream.GetBuffer(sizeof(PktUK));	
	if(0 != lpBuffer)
	{		
		PktUK* lpUK = reinterpret_cast<PktUK*>(lpBuffer);
		
		lpUK->m_dwServerID		= 0;
		lpUK->m_dwUserID		= UserID;
		lpUK->m_dwCharID		= 0;
		
        return SendStream.WrapHeader(sizeof(PktUK), CmdUserKill, 0, 0);
	}

	return false;
}

bool AgentSendPacket::SendUpdateUIDTable(char AgentGroup, char Cmd, unsigned long UserID, unsigned long Flag,
                                         int PlayTime, unsigned long dwCRMIndex1, 
                                         char BillingType, unsigned short usState, unsigned short usErrorCode)
{
	GET_MULTI_DISPATCH(lpAgentDispatch, AgentGroup, 
		CUIDAgentDispatch, CUIDAgentDispatch::GetDispatchTable());

	if(0 == lpAgentDispatch) 
	{
		ERRLOG1(g_Log, "그룹:%d 연결세션 구하기 실패", AgentGroup);
		return false;
	}

    CSendStream& SendStream = lpAgentDispatch->GetSendStream();
    char* lpBuffer = SendStream.GetBuffer(sizeof(PktUUTAck));	
	if(0 != lpBuffer)
	{		
		PktUUTAck* lpUUTAck = reinterpret_cast<PktUUTAck*>(lpBuffer);

		lpUUTAck->m_cCmd			= Cmd;
		lpUUTAck->m_dwUserID		= UserID;
		lpUUTAck->m_dwFlag			= Flag;
		lpUUTAck->m_nPlayTime		= PlayTime;
        lpUUTAck->m_dwCRMIndex1     = dwCRMIndex1;
		lpUUTAck->m_cstrBillingType	= BillingType;

        return SendStream.WrapHeader(sizeof(PktUUTAck), CmdUpdateUIDTable, usState, usErrorCode);
	}

	return false;
}


bool AgentSendPacket::SendHanBTN(char cAgentGroup, unsigned long dwUID, 
                                 unsigned char cRemainMinute, unsigned char cBillingType)
{    
	GET_MULTI_DISPATCH(lpAgentDispatch, cAgentGroup, 
		CUIDAgentDispatch, CUIDAgentDispatch::GetDispatchTable());

	if(0 == lpAgentDispatch) 
	{
		ERRLOG1(g_Log, "그룹:%d 연결세션 구하기 실패", cAgentGroup);
	}
    else
    {
        CSendStream& SendStream = lpAgentDispatch->GetSendStream();	    
	    char* lpBuffer = SendStream.GetBuffer(sizeof(PktBTN));

        if(0 != lpBuffer)
        {
            PktBTN* lpPktBTN = reinterpret_cast<PktBTN*>(lpBuffer);

            lpPktBTN->m_dwUserID        = dwUID;
            lpPktBTN->m_dwCharID        = 0;
            lpPktBTN->m_dwServerID      = 0;
            lpPktBTN->m_cRemainMinute   = cRemainMinute;
            lpPktBTN->m_cBillingType    = cBillingType;

            return SendStream.WrapHeader(sizeof(PktBTN), CmdBillingTimeoutNotify, 0, 0);
        }
    }

    return false;
}




bool AgentSendPacket::SendHanUnitedBTN(const char* szID, const char* szIP, const char* szMsg)
{
    PktHanBTN pktHanBTN;
    memset(&pktHanBTN, 0, sizeof(PktHanBTN));

    pktHanBTN.m_dwIP = inet_addr(szIP);
    
    _snprintf(pktHanBTN.m_szAccount, PktUUT::MaxAccountLen, "%s", szID);
    pktHanBTN.m_szAccount[PktUUT::MaxAccountLen - 1] = 0;

    _snprintf(pktHanBTN.m_szMsg, PktHanBTN::MAX_HAN_BTN, "%s", szMsg);
    pktHanBTN.m_szMsg[PktHanBTN::MAX_HAN_BTN - 1] = 0;
    
    if(PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktHanBTN), 
        sizeof(PktHanBTN), CmdHanBTNWarning, 0, 0))
    {
        CUIDAgentDispatch::GetDispatchTable().Process(
            CSendUIDAgentDispatch(&pktHanBTN, sizeof(PktHanBTN)));
        return true;
    }

    return false;
}

bool AgentSendPacket::SendHanUnitedUserKill(const char* szID, const char* szIP)
{
    PktHanUserKill pktHanUserKill;
    memset(&pktHanUserKill, 0, sizeof(PktHanUserKill));

    pktHanUserKill.m_dwIP = inet_addr(szIP);

    _snprintf(pktHanUserKill.m_szAccount, PktUUT::MaxAccountLen, "%s", szID);
    pktHanUserKill.m_szAccount[PktUUT::MaxAccountLen - 1] = 0;

    if(PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktHanUserKill),
        sizeof(pktHanUserKill), CmdHanBTNUserKill, 0, 0))
    {
        CUIDAgentDispatch::GetDispatchTable().Process(
            CSendUIDAgentDispatch(&pktHanUserKill, sizeof(pktHanUserKill)));
        return true;
    }

    return false;
}

bool AgentSendPacket::SendHanUnitedUserKill(unsigned long dwAgentGroup, const char* szID, const char* szIP)
{
    GET_MULTI_DISPATCH(lpAgentDispatch, dwAgentGroup, 
        CUIDAgentDispatch, CUIDAgentDispatch::GetDispatchTable());

    if(0 == lpAgentDispatch) 
    {
        ERRLOG1(g_Log, "그룹:%d 연결세션 구하기 실패", dwAgentGroup);
        return false;
    }

    CSendStream& SendStream = lpAgentDispatch->GetSendStream();	
    char* lpBuffer = SendStream.GetBuffer(sizeof(PktHanUserKill));	
    if(0 != lpBuffer)
    {		
        PktHanUserKill* lpHanUserKill = reinterpret_cast<PktHanUserKill*>(lpBuffer);

        if(0 != szIP)
        {
            lpHanUserKill->m_dwIP = inet_addr(szIP);
        }
        else
        {
            lpHanUserKill->m_dwIP = PktHanUserKill::DISCONN_ALL_IP;
        }

        _snprintf(lpHanUserKill->m_szAccount, PktUUT::MaxAccountLen, "%s", szID);
        lpHanUserKill->m_szAccount[PktUUT::MaxAccountLen - 1] = 0;

        return SendStream.WrapHeader(sizeof(PktHanUserKill), CmdHanBTNUserKill, 0, 0);
    }

    return false;
}