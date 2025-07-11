#include "stdafx.h"
#include "ParseParty.h"

#include <Community/Party/PartyDBInfo.h>
#include <Community/Party/PartyDBMgr.h>
#include <Community/Guild/Guild.h>
#include <Community/Guild/GuildMgr.h>

#include <Network/Stream/SendStream.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/WrapPacket.h>
#include <Network/Packet/PacketStruct/PartyPacket.h>
#include <Network/Dispatch/GameDispatch.h>
#include <Network/SendPacket/SendParty.h>

#include <Log/ServerLog.h>
#include <DB/DBComponent.h>

#include <DataStorage/SessionData.h>
#include <DataStorage/CharacterData.h>
#include <DataStorage/SessionDataMgr.h>

#include <Community/Guild/Guild.h>

namespace DBAgent
{
namespace ParseParty
{

bool CreateParty(CSendStream& SendStream, PktCPD* lpPktCPD);
bool DeleteParty(CSendStream& SendStream, PktDPD* lpPktDPD);
bool GetPartyInfo(CSendStream& SendStream, PktPID* lpPktPID);
bool TransferLeader(CSendStream& SendStream, PktPMD* lpPktPMD);
bool InsertPartyMember(CSendStream& SendStream, PktPMD* lpPktPMD);
bool DeletePartyMember(CSendStream& SendStream, PktPMD* lpPktPMD);
bool AutoRoutingOn(CSendStream& SendStream, PktPMD* lpPktPMD);
bool AutoRoutingOff(CSendStream& SendStream, PktPMD* lpPktPMD);
bool RelayPacket(CSendStream& SendStream, PktDD* lpPktDD);

bool Parse(CSendStream& SendStream, PktBase* lpPktBase)
{   
    bool bResult = false;

    PktDD* lpPktDD = static_cast<PktDD*>(lpPktBase);
    
    switch (lpPktDD->m_wCmd) 
    {
    case PktDD::SCmdCreateParty:	bResult = CreateParty(SendStream,		static_cast<PktCPD*>(lpPktDD)); break;	// ��Ƽ ����
    case PktDD::SCmdDeleteParty:	bResult = DeleteParty(SendStream,		static_cast<PktDPD*>(lpPktDD));	break;	// ��Ƽ ����
    case PktDD::SCmdGetPartyInfo:	bResult = GetPartyInfo(SendStream,	    static_cast<PktPID*>(lpPktDD));	break;	// ��Ƽ ���� ���
    case PktDD::SCmdTransferLeader:	bResult = TransferLeader(SendStream,	static_cast<PktPMD*>(lpPktDD));	break;	// ���� ���� �絵
    case PktDD::SCmdInsertPartyMem:	bResult = InsertPartyMember(SendStream,	static_cast<PktPMD*>(lpPktDD));	break;	// ��Ƽ ��� �߰�
    case PktDD::SCmdDeletePartyMem: bResult = DeletePartyMember(SendStream,	static_cast<PktPMD*>(lpPktDD));	break;	// ��Ƽ ��� ����

    case PktDD::SCmdDeadPartyMem:
    case PktDD::SCmdMoveZonePartyMem:
    case PktDD::SCmdLevelUpPartyMem:
    case PktDD::SCmdChangeClassPartyMem:
	case PktDD::SCmdChangeGuildPartyMem:
        bResult = RelayPacket(SendStream, lpPktDD);
        break;

    case PktDD::SCmdAutoRoutingOn:	bResult = AutoRoutingOn(SendStream,	    static_cast<PktPMD*>(lpPktDD));	break;	// ��Ƽ ������� �ѱ�
    case PktDD::SCmdAutoRoutingOff:	bResult = AutoRoutingOff(SendStream,	static_cast<PktPMD*>(lpPktDD));	break;	// ��Ƽ ������� ����
    
    default:	
        bResult = true;
        ERRLOG1(g_Log, "��Ƽ �α� : Cmd:0x%02x �� �� ���� ��Ƽ �����", lpPktDD->m_wCmd);
        break;
    }

    if(!bResult)
    {
        ERRLOG1(g_Log, "��Ƽ �α� : Cmd:0x%02x ��� ó�� ����.", lpPktDD->m_wCmd);
    }

    return true;
}

bool CreateParty(CSendStream& SendStream, PktCPD* lpPktCPD)
{
    // ��Ƽ ����
    CPartyDBInfo* lpParty = CPartyDBMgr::GetInstance().CreateParty(CDBSingleObject::GetInstance(), lpPktCPD->m_dwLeaderID, lpPktCPD->m_cLeaderLevel, 
        lpPktCPD->m_wLeaderClass, lpPktCPD->m_dwLeaderGID, lpPktCPD->m_dwMemberID, lpPktCPD->m_cMemberLevel, lpPktCPD->m_wMemberClass, lpPktCPD->m_dwMemberGID);

    char* lpBuffer = 0;

    if (0 == lpParty)
    {
        ERRLOG2(g_Log, "��Ƽ �α� : LeaderCID:%10u / MemberCID:%10u / ��Ƽ ���� ���� : ��Ƽ ���� ����",
            lpPktCPD->m_dwLeaderID, lpPktCPD->m_dwMemberID);        
    }
    else 
    {
		PktCPDAck sPktCPDAck;        
		memset(&sPktCPDAck, 0, sizeof(PktCPDAck));
        sPktCPDAck.m_wCmd     = lpPktCPD->m_wCmd;
        sPktCPDAck.m_Party    = lpParty->GetParty();		

		sPktCPDAck.m_dwLeaderID		= lpPktCPD->m_dwLeaderID;
		sPktCPDAck.m_dwMemberID		= lpPktCPD->m_dwMemberID;
		sPktCPDAck.m_dwLeaderGID	= lpPktCPD->m_dwLeaderGID;
		sPktCPDAck.m_wLeaderClass	= lpPktCPD->m_wLeaderClass;
		sPktCPDAck.m_cLeaderLevel	= lpPktCPD->m_cLeaderLevel;
		sPktCPDAck.m_dwMemberGID	= lpPktCPD->m_dwMemberGID;
		sPktCPDAck.m_wMemberClass	= lpPktCPD->m_wMemberClass;
		sPktCPDAck.m_cMemberLevel	= lpPktCPD->m_cMemberLevel;
		
		// ĳ���� ���� ������Ʈ //
		lpParty->SetUserInfoLevel(lpPktCPD->m_dwLeaderID, lpPktCPD->m_cLeaderLevel);
		lpParty->SetUserInfoClass(lpPktCPD->m_dwLeaderID, lpPktCPD->m_wLeaderClass);
		lpParty->SetUserInfoGID(lpPktCPD->m_dwLeaderID, lpPktCPD->m_dwLeaderGID);

		lpParty->SetUserInfoLevel(lpPktCPD->m_dwMemberID, lpPktCPD->m_cMemberLevel);
		lpParty->SetUserInfoClass(lpPktCPD->m_dwMemberID, lpPktCPD->m_wMemberClass);
		lpParty->SetUserInfoGID(lpPktCPD->m_dwMemberID, lpPktCPD->m_dwMemberGID);

		if(PacketWrap::WrapHeader(reinterpret_cast<char*>(&sPktCPDAck), sizeof(PktCPDAck), CmdAgentParty, 0, 0))
		{
			// ���� ������ ��ε�ĳ���� //
			DBAgent::CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
				reinterpret_cast<char*>(&sPktCPDAck), sizeof(PktCPDAck), CmdAgentParty));

			return true;
		}
    }
	return false;
}

bool DeleteParty(CSendStream& SendStream, PktDPD* lpPktDPD)
{
    // ��Ƽ ����
    if (!CPartyDBMgr::GetInstance().DestoryParty(
        CDBSingleObject::GetInstance(), lpPktDPD->m_dwPartyID))
    {
        ERRLOG1(g_Log, "��Ƽ �α� : PID:%10u / ��Ƽ ���� ����", lpPktDPD->m_dwPartyID);        
    }

    return true;
}


bool GetPartyInfo(CSendStream& SendStream, PktPID* lpPktPID)
{
    unsigned long dwPID = lpPktPID->m_dwPartyID;
    unsigned long dwCID = lpPktPID->m_dwCharID;

    // ��Ƽ ����
    CPartyDBInfo* lpParty = CPartyDBMgr::GetInstance().OpenParty(
        CDBSingleObject::GetInstance(), dwPID);

    if (0 == lpParty) 
    {
        ERRLOG2(g_Log, "��Ƽ �α� : PID:%10u / CID:%10u / ��Ƽ ���� ��� ���� : ��Ƽ ��� ����",
            dwPID, dwCID);        
    }
    else
    {		
        unsigned long dwServerID = 0;

        using namespace DataStorage;
        CSessionData* lpSessionData = CSessionDataMgr::GetInstance().GetCharLoadedSession(dwCID);
        if(0 != lpSessionData)
        {
            dwServerID = lpSessionData->GetServerID();
        }

//		lpParty->PartyMemAllLogout();
//		lpParty->PartyMemLogin(dwCID, dwSID);
//		lpParty->SetLeader(dwCID);

		DETLOG2(g_Log, "��Ƽ �α� : PID:%10u / CID:%10u / ��Ƽ ���� ���", dwPID, dwCID);		

        // ��Ŷ ������
		PktPIDAck pktPIDAck;
		memset(&pktPIDAck, 0, sizeof(PktPIDAck));
     
        pktPIDAck.m_wCmd		= lpPktPID->m_wCmd;
        pktPIDAck.m_Party		= lpParty->GetParty();
		pktPIDAck.m_dwCharID	= lpPktPID->m_dwCharID;
		pktPIDAck.m_dwGID		= lpPktPID->m_dwGID;
		pktPIDAck.m_wClass		= lpPktPID->m_wClass;
		pktPIDAck.m_cLevel		= lpPktPID->m_cLevel;
		pktPIDAck.m_dwServerID	= lpPktPID->m_dwServerID;
            
		if(PacketWrap::WrapHeader(reinterpret_cast<char*>(&pktPIDAck), 
			sizeof(PktPIDAck), CmdAgentParty, 0, 0))
		{
			// ���� ������ ��ε�ĳ���� //
			CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
				reinterpret_cast<char*>(&pktPIDAck), sizeof(PktPIDAck), CmdAgentParty));

			return true;
		}			
    }

    return true;
}


bool TransferLeader(CSendStream& SendStream, PktPMD* lpPktPMD)
{
    unsigned long dwPartyID		= lpPktPMD->m_dwPartyID;
    unsigned long dwSenderID	= lpPktPMD->m_dwSenderID;
    unsigned long dwReferenceID	= lpPktPMD->m_dwGID;

    using namespace DataStorage;
    CSessionData*   lpSessionData = CSessionDataMgr::GetInstance().GetCharLoadedSession(dwSenderID);
    CCharacterData* lpCharacterData = 0;
    CPartyDBInfo*   lpParty = 0;

    if(0 == lpSessionData)    
    {
        ERRLOG2(g_Log, "��Ƽ �α� : PID:%10u / CID:%10u / ���� �̾� ���� : ĳ���� ã�� ����", 
            dwPartyID, dwSenderID);        
    }
    else if(0 == (lpCharacterData = lpSessionData->GetCharacterData()))
    {
        ERRLOG2(g_Log, "��Ƽ �α� : PID:%10u / CID:%10u / ���� �̾� ���� : ĳ���� ������ ����", 
            dwPartyID, dwSenderID);        
    }
    else if(0 == (lpParty = CPartyDBMgr::GetInstance().FindPartyMap(dwPartyID)))
    {
        ERRLOG2(g_Log, "��Ƽ �α� : PID:%10u / CID:%10u / ���� �̾� ���� : ��Ƽ ã�� ����", 
            dwPartyID, dwSenderID);        
    }
    else if(!lpParty->TransferLeader(dwReferenceID, dwSenderID))
    {
        ERRLOG3(g_Log, "��Ƽ �α� : PID:%10u / OldLeaderCID:%10u / NewLeaderCID:%10u / ���� �̾� ����",
            dwPartyID, dwReferenceID, dwSenderID);        
    }
    else
    {
        DETLOG3(g_Log, "��Ƽ �α� : PID:%10u / OldLeaderCID:%10u / NewLeaderCID:%10u / ���� �̾� ����",
            dwPartyID, dwReferenceID, dwSenderID);        
		
        SendPacket::PartyCmd(PktDD::SCmdTransferLeader, lpParty->GetPID(), dwSenderID, 0, 0,
            lpSessionData->GetServerID(), 0, lpCharacterData->GetName());
    }

    return true;
}

bool InsertPartyMember(CSendStream& SendStream, PktPMD* lpPktPMD)
{
    unsigned long	dwPartyID	= lpPktPMD->m_dwPartyID;
    unsigned long	dwSenderID	= lpPktPMD->m_dwSenderID;
    unsigned long	dwGID		= lpPktPMD->m_dwGID;
	unsigned short	wClass		= lpPktPMD->m_wClass;
	char			cLevel		= lpPktPMD->m_cLevel;	

    // ��Ƽ�� �߰�
    if (!CPartyDBMgr::GetInstance().InsertPartyMember(
        CDBSingleObject::GetInstance(), dwPartyID, dwSenderID, dwGID, wClass, cLevel))
    {
        ERRLOG2(g_Log, "��Ƽ �α� : PID:%10u / CID:%10u / ��Ƽ ��� �߰� ����", dwPartyID, dwSenderID);        
    }

	CPartyDBInfo* lpParty = CPartyDBMgr::GetInstance().OpenParty(CDBSingleObject::GetInstance(), dwPartyID);

	if(lpParty)
	{
		// ĳ���� ���� ������Ʈ //
		lpParty->SetUserInfoLevel(dwSenderID, cLevel);
		lpParty->SetUserInfoClass(dwSenderID, wClass);
		lpParty->SetUserInfoGID(dwSenderID, dwGID);
	}

    return true;
}

bool DeletePartyMember(CSendStream& SendStream, PktPMD* lpPktPMD)
{
    unsigned long dwPartyID		= lpPktPMD->m_dwPartyID;
    unsigned long dwSenderID	= lpPktPMD->m_dwSenderID;
    unsigned long dwReferenceID	= lpPktPMD->m_dwGID;

    // ű�� ���
    if (dwSenderID != dwReferenceID)
    {
        dwReferenceID = 0;
    }

	CPartyDBInfo* lpParty = CPartyDBMgr::GetInstance().OpenParty(CDBSingleObject::GetInstance(), dwPartyID);

	if(lpParty)
	{
		// ĳ���� ���� ������Ʈ //
		lpParty->SetUserInfoLevel(dwSenderID, 0);
		lpParty->SetUserInfoClass(dwSenderID, 0);
		lpParty->SetUserInfoGID(dwSenderID, 0);
	}	

    // ��Ƽ�� ����
    if (!CPartyDBMgr::GetInstance().DeletePartyMember(
        CDBSingleObject::GetInstance(), dwPartyID, dwSenderID, dwReferenceID))
    {
        ERRLOG2(g_Log, "��Ƽ �α� : PID:%10u / CID:%10u / ��Ƽ ��� ���� ����", dwPartyID, dwSenderID);        
    }

    return true;
}

bool AutoRoutingOn(CSendStream& SendStream, PktPMD* lpPktPMD)
{
    unsigned long dwPartyID		= lpPktPMD->m_dwPartyID;
    unsigned long dwSenderID	= lpPktPMD->m_dwSenderID;
    unsigned long dwReferenceID	= lpPktPMD->m_dwGID;

    // ������� �ѱ�
    if (!CPartyDBMgr::GetInstance().AutoRoutingOn(
        CDBSingleObject::GetInstance(), dwPartyID, dwSenderID))
    {
        ERRLOG2(g_Log, "��Ƽ �α� : PID:%10u / CID:%10u / ĳ������ ������� �ѱ� ���� (CID �� 0�̸� ALL)", 
            dwPartyID, dwSenderID);
    }

    return true;
}

bool AutoRoutingOff(CSendStream& SendStream, PktPMD* lpPktPMD)
{
    unsigned long dwPartyID		= lpPktPMD->m_dwPartyID;
    unsigned long dwSenderID	= lpPktPMD->m_dwSenderID;
    unsigned long dwReferenceID	= lpPktPMD->m_dwGID;

    // ������� ����
    if (!CPartyDBMgr::GetInstance().AutoRoutingOff(
        CDBSingleObject::GetInstance(), dwPartyID, dwSenderID))
    {
        ERRLOG2(g_Log, "��Ƽ �α� : PID:%10u / CID:%10u / ĳ������ ������� ���� ���� (CID �� 0�̸� ALL)", 
            dwPartyID, dwSenderID);
    }

    return true;
}

bool RelayPacket(CSendStream& SendStream, PktDD* lpPktDD)
{
	unsigned short usPacketSize = lpPktDD->GetLen();

	switch(lpPktDD->m_wCmd)
	{	 	 
	case PktDD::SCmdLevelUpPartyMem:
	case PktDD::SCmdChangeClassPartyMem:
	case PktDD::SCmdChangeGuildPartyMem:
		{
			PktPMD* lpPktPMD = static_cast<PktPMD*>(lpPktDD);

			CPartyDBInfo* lpParty = CPartyDBMgr::GetInstance().OpenParty(CDBSingleObject::GetInstance(), lpPktPMD->m_dwPartyID);

			if(lpParty)
			{
				// ĳ���� ���� ������Ʈ //
				if(PktDD::SCmdLevelUpPartyMem)
				{
					lpParty->SetUserInfoLevel(lpPktPMD->m_dwSenderID, lpPktPMD->m_cLevel);
				}
				if(PktDD::SCmdChangeClassPartyMem)
				{
					lpParty->SetUserInfoClass(lpPktPMD->m_dwSenderID, lpPktPMD->m_wClass);
				}
				if(PktDD::SCmdChangeGuildPartyMem)
				{
					lpParty->SetUserInfoGID(lpPktPMD->m_dwSenderID, lpPktPMD->m_dwGID);					
				}
			}
		}		
		break;
	}	

    if(PacketWrap::WrapHeader(reinterpret_cast<char*>(lpPktDD), 
        usPacketSize, CmdAgentParty, 0, 0))
    {
        CGameDispatch::GetDispatchTable().Process(CSendPacketAllServer(
            reinterpret_cast<char*>(lpPktDD), usPacketSize, CmdAgentParty));
    }

    return true;
}


}
}


