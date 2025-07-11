
#include "stdafx.h"

#include <Network/Winsock/SocketFactory.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/ServerPacket.h>
#include <Network/Dispatch/GameClient/GameClientDispatch.h>

#include <Utility/Setup/ServerSetup.h>

#include "GuildPacketParse.h"
#include "CastlePacketParse.h"
#include "DBAgentPacketParse.h"
#include "DBAgentDispatch.h"


enum
{
	// edith 2008.03.04 CDBAgentDispatch 수정
    MAX_DBAGENT_PACKET_DISPATCH_PER_PULSE = 200
};

CSingleDispatch& CDBAgentDispatch::GetDispatchTable()
{
	static CSingleDispatch singleDispatch;
	return singleDispatch;
}	


CDBAgentDispatch::CDBAgentDispatch(CSession& Session)
:	CRylServerDispatch(Session, MAX_DBAGENT_PACKET_DISPATCH_PER_PULSE)
{


}

CDBAgentDispatch::~CDBAgentDispatch()
{


}


void CDBAgentDispatch::Connected()
{
    char* lpBuffer = m_SendStream.GetBuffer(sizeof(PktSL));
    if(NULL != lpBuffer)
    {
        PktSL* lpPktSL = reinterpret_cast<PktSL*>(lpBuffer);

        const int MAX_ADDRESS = 128;
        char szAddress[MAX_ADDRESS];

        CTCPFactory tcpFactory;		
		tcpFactory.GetNetworkInfo(szAddress, MAX_ADDRESS);

        lpPktSL->m_Address.S_un.S_addr = inet_addr(szAddress);
        lpPktSL->m_dwServerID = CServerSetup::GetInstance().GetServerID();

        if(m_SendStream.WrapHeader(sizeof(PktSL), CmdSysServerLogin, 0, 0))
		{
			INFLOG2(g_Log, "중계세션(0x%p)에 서버(0x%08x)의 로그인패킷을 전송", 
				this, CServerSetup::GetInstance().GetServerID());
		}
    }
}


void CDBAgentDispatch::Disconnected()
{
    GetDispatchTable().RemoveDispatch(this);
}


bool CDBAgentDispatch::DispatchPacket(PktBase* lpPktBase)
{
	bool bResult = false;

    switch (lpPktBase->GetCmd())
    {
		case CmdUserKill:               bResult =  DBAgentPacketParse::ParseUserKill(*this, lpPktBase);             	break;
		case CmdDBUpdateData:           bResult =  DBAgentPacketParse::ParseUpdateDBData(*this, lpPktBase);         	break;
		case CmdAgentParty:		        bResult =  DBAgentPacketParse::ParseAgentParty(*this, lpPktBase);		    	break;
		case CmdAgentZone:              bResult =  DBAgentPacketParse::ParseAgentZone(*this, lpPktBase);            	break;
		case CmdSysServerLogin:         bResult =  DBAgentPacketParse::ParseSysServerLogin(*this, lpPktBase);       	break;
		case CmdSysServerVerUpdate:     bResult =  DBAgentPacketParse::ParseSysServerVerUpdate(*this, lpPktBase);   	break;
		case CmdSysChannelUpdate:       bResult =  DBAgentPacketParse::ParseSysChannelUpdate(*this, lpPktBase);			break;  
		case CmdDepositUpdate:          bResult =  DBAgentPacketParse::ParseDepositUpdate(*this, lpPktBase);        	break;
		case CmdDeposit:                bResult =  DBAgentPacketParse::ParseDepositCmd(*this, lpPktBase);           	break;
		case CmdFriendDB:			    bResult =  DBAgentPacketParse::ParseFriendList(*this, lpPktBase);				break;
		case CmdQuestDB:			    bResult =  DBAgentPacketParse::ParseQuestDB(*this, lpPktBase);					break;
		case CmdConfigInfoDB:		    bResult =  DBAgentPacketParse::ParseConfigInfoDB(*this, lpPktBase);				break;
		case CmdRankingInfo:		    bResult =  DBAgentPacketParse::ParseSysRankingUpdate(*this, lpPktBase);			break;
        case CmdBillingTimeoutNotify:   bResult =  DBAgentPacketParse::ParseBillingTimeoutNotify(*this, lpPktBase); 	break;
        case CmdBillingTimeCheckNotify: bResult =  DBAgentPacketParse::ParseBillingTimeCheckNotify(*this, lpPktBase); 	break;
        case CmdEventDropItem:			bResult =  DBAgentPacketParse::ParseEventDropItem(*this, lpPktBase);			break;
        case CmdCharUseCashItem:		bResult =  DBAgentPacketParse::ParseCharUseCashItem(*this, lpPktBase);			break;
		case CmdExtraEvent:				bResult =  DBAgentPacketParse::ParseExtraEvent(*this, lpPktBase);				break;

		case CmdCreateGuild:			bResult =  DBAgentPacketParse::ParseCreateGuild(*this, lpPktBase);				break;
		case CmdGuildCmd:				bResult =  DBAgentPacketParse::ParseGuildCmd(*this, lpPktBase);					break;
		case CmdGuildMark:				bResult =  DBAgentPacketParse::ParseGuildMark(*this, lpPktBase);				break;
		case CmdGuildLevel:				bResult =  DBAgentPacketParse::ParseGuildLevel(*this, lpPktBase);				break;
		case CmdGuildRelation:			bResult =  DBAgentPacketParse::ParseGuildRelation(*this, lpPktBase);			break;
		case CmdGuildInclination:		bResult =  DBAgentPacketParse::ParseGuildInclination(*this, lpPktBase);			break;
		case CmdGuildRight:				bResult =  DBAgentPacketParse::ParseGuildRight(*this, lpPktBase);				break;
		case CmdGuildDB:				bResult =  DBAgentPacketParse::ParseGuildDB(*this, lpPktBase);					break;
		case CmdGuildSafe:				bResult =  DBAgentPacketParse::ParseGuildSafe(*this, lpPktBase);				break;
		case CmdGuildMemberInfoUpdate:	bResult =  DBAgentPacketParse::ParseGuildMemberInfoUpdate(*this, lpPktBase);	break;
		case CmdGuildMemberFameUpdate:	bResult =  DBAgentPacketParse::ParseGuildMemberFameUpdate(*this, lpPktBase);	break;
		case CmdGuildMemberGoldUpdate:	bResult =  DBAgentPacketParse::ParseGuildMemberGoldUpdate(*this, lpPktBase);	break;

		case CmdCastleInfo:				bResult =  DBAgentPacketParse::ParseCastleInfo(*this, lpPktBase);				break;
		case CmdCampInfo:				bResult =  DBAgentPacketParse::ParseCampInfo(*this, lpPktBase);					break;
		case CmdFertilityInfo:			bResult =  DBAgentPacketParse::ParseFertilityInfo(*this, lpPktBase);			break;
		case CmdWorldWeaponInfo:		bResult =  DBAgentPacketParse::ParseWorldWeaponInfo(*this, lpPktBase);			break;
		case CmdCreateCamp:				bResult =  DBAgentPacketParse::ParseCreateCamp(*this, lpPktBase);				break;
		case CmdCreateSiegeArms:		bResult =  DBAgentPacketParse::ParseCreateSiegeArms(*this, lpPktBase);			break;
		case CmdCastleCmd:				bResult =  DBAgentPacketParse::ParseCastleCmd(*this, lpPktBase);				break;
		case CmdCampCmd:				bResult =  DBAgentPacketParse::ParseCampCmd(*this, lpPktBase);					break;
		case CmdSiegeArmsCmd:			bResult =  DBAgentPacketParse::ParseSiegeArmsCmd(*this, lpPktBase);				break;
		case CmdCastleRight:			bResult =  DBAgentPacketParse::ParseCastleRight(*this, lpPktBase);				break;
		case CmdCampRight:				bResult =  DBAgentPacketParse::ParseCampRight(*this, lpPktBase);				break;
		case CmdGameTimeInfo:			bResult =  DBAgentPacketParse::ParseGameTimeInfo(*this, lpPktBase);				break;
		case CmdCampMessage:			bResult =  DBAgentPacketParse::ParseCampMessage(*this, lpPktBase);				break;
		case CmdProcessMining:			bResult =  DBAgentPacketParse::ParseProcessMining(*this, lpPktBase);			break;
		
		case CmdCharAdminCmd:			bResult =  DBAgentPacketParse::ParseCharAdminCmd(*this, lpPktBase);				break;

        case CmdGiveItemToTempInven:    bResult =  DBAgentPacketParse::ParseGiveItemToTempInven(*this, lpPktBase);      break;
        case CmdCharNameChange:         bResult =  DBAgentPacketParse::ParseCharNameChangeAck(*this, lpPktBase);        break;

		case CmdCastleTaxMove:			bResult =  DBAgentPacketParse::ParseCastleTaxMove(*this, lpPktBase);			break;
		case CmdCastleSiegeCount:		bResult	=  DBAgentPacketParse::ParseCastleSiegeCount(*this, lpPktBase);			break;

		case CmdWarOnOff:				bResult =  DBAgentPacketParse::ParseWarOnOff(*this, lpPktBase);					break;
		case CmdRealmPoint:				bResult =  DBAgentPacketParse::ParseRealmPoint(*this, lpPktBase);				break;
		case CmdStatueCmd:				bResult =  DBAgentPacketParse::ParseStatueCmd(*this, lpPktBase);				break;

		case CmdSaveEnemy:				bResult =  DBAgentPacketParse::ParseSaveEnemy(*this, lpPktBase);				break;
		case CmdKeyInfo:				bResult =  DBAgentPacketParse::ParseKeyInfo(*this, lpPktBase);					break;

		case CmdSysServerLogout:
			bResult = true;
			CloseSession();
			break;

		default:
			bResult = true;
			ERRLOG2(g_Log, "DP:0x%p/Cmd:0x%02x/Unknown Agentpacket", 
				this, lpPktBase->GetCmd());

			break;
    }

	if(!bResult)
	{
		ERRLOG2(g_Log, "DP:0x%p/Cmd:0x%02x/Agentpacket Process failed", 
			this, lpPktBase->GetCmd());
	}

    return true;
}
