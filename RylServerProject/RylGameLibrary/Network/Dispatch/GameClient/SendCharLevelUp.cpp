
#include "stdafx.h"
#include "SendCharLevelUp.h"

#include <DB/DBDefine.h>
#include <Creature/Character/CharacterStructure.h>

#include <Network/Stream/SendStream.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/CharStatusPacket.h>
#include <Network/Packet/PacketStruct/CharAttackPacket.h>


bool GameClientSendPacket::SendCharClassUpgrade(CSendStream& SendStream, unsigned long dwCharID, CharacterDBData& DBData, unsigned short usError)
{
    char* lpBuffer = SendStream.GetBuffer(sizeof(PktCUAck));
    if (NULL == lpBuffer)
    {
	    return false;
    }

    PktCUAck* lpPktCUAck = reinterpret_cast<PktCUAck*>(lpBuffer);

	lpPktCUAck->m_dwCharID		= dwCharID;
	
	lpPktCUAck->m_cClass		= static_cast<unsigned char>(DBData.m_Info.Class);
	lpPktCUAck->m_cLevel		= static_cast<unsigned char>(DBData.m_Info.Level);
	lpPktCUAck->m_dwFame		= DBData.m_Info.Fame;

	lpPktCUAck->m_State.m_wIP	= DBData.m_Info.IP;
	lpPktCUAck->m_State.m_wSTR	= DBData.m_Info.STR;
	lpPktCUAck->m_State.m_wDEX	= DBData.m_Info.DEX;
	lpPktCUAck->m_State.m_wCON	= DBData.m_Info.CON;
	lpPktCUAck->m_State.m_wINT	= DBData.m_Info.INT;
	lpPktCUAck->m_State.m_wWIS	= DBData.m_Info.WIS;

    return SendStream.WrapCrypt(sizeof(PktCUAck), CmdCharClassUpgrade, 0, usError);
}


bool GameClientSendPacket::SendCharIncreasePoint(CSendStream& SendStream, unsigned long dwCharID, ChState State, unsigned short usError)
{
    char* lpBuffer = SendStream.GetBuffer(sizeof(PktIPAck));
    if (NULL == lpBuffer)
    {
	    return false;
    }

    PktIPAck* lpPktIPAck = reinterpret_cast<PktIPAck*>(lpBuffer);

	lpPktIPAck->m_dwCharID = dwCharID;
	lpPktIPAck->m_State	= State;
            
    return SendStream.WrapCrypt(sizeof(PktIPAck), CmdCharIncreasePoint, 0, usError);
}


bool GameClientSendPacket::SendCharLevelUp(CSendStream& SendStream, unsigned long dwCharID, CharacterDBData& DBData)
{
    char* lpBuffer = SendStream.GetBuffer(sizeof(PktLU));
    if (NULL == lpBuffer)
    {
		ERRLOG1(g_Log, "CID:0x%08x 메모리 할당에 실패하였습니다.", dwCharID);		
	    return false;
    }

    PktLU* lpPktLU = reinterpret_cast<PktLU*>(lpBuffer);

	lpPktLU->m_dwCharID = dwCharID;
	lpPktLU->m_cLevel = DBData.m_Info.Level;
	
	lpPktLU->m_State.m_wIP = DBData.m_Info.IP;
	lpPktLU->m_State.m_wSTR = DBData.m_Info.STR;
	lpPktLU->m_State.m_wDEX = DBData.m_Info.DEX;
	lpPktLU->m_State.m_wCON = DBData.m_Info.CON;
	lpPktLU->m_State.m_wINT = DBData.m_Info.INT;
	lpPktLU->m_State.m_wWIS = DBData.m_Info.WIS;
            
    return SendStream.WrapCrypt(sizeof(PktLU), CmdCharLevelUp, 0, 0);
}


bool GameClientSendPacket::SendCharAward(CSendStream& SendStream, unsigned long dwCharID, unsigned long dwExp)
{
    char* lpBuffer = SendStream.GetBuffer(sizeof(PktAw));
    if (NULL == lpBuffer)
    {
		ERRLOG1(g_Log, "CID:0x%08x 메모리 할당에 실패하였습니다.", dwCharID);		
	    return false;
    }

    PktAw* lpPktAw = reinterpret_cast<PktAw*>(lpBuffer);

	lpPktAw->m_dwCharID = dwCharID;
	lpPktAw->m_dwExp = dwExp;
            
    return SendStream.WrapCrypt(sizeof(PktAw), CmdCharAward, 0, 0);
}

bool GameClientSendPacket::SendCharStateRedistribution(CSendStream& SendStream, unsigned long dwCharID, 
													   const ChState& state, const SKILL& skill, unsigned short usError)
{
	char* lpBuffer = SendStream.GetBuffer(sizeof(PktSRAck));
	if (NULL == lpBuffer)
	{
		return false;
	}

	PktSRAck* lpPktSRAck = reinterpret_cast<PktSRAck *>(lpBuffer);

	lpPktSRAck->m_State = state;
	lpPktSRAck->m_Skill = skill;

	return SendStream.WrapCrypt(sizeof(PktSRAck), CmdCharStateRedistribution, 0, usError);
}

bool GameClientSendPacket::SendCharStatusRetrain(CSendStream& SendStream, unsigned long dwCharID, CharacterDBData& DBData,
												Item::ItemPos InvenPos, unsigned long dwGold, unsigned short usError)
{
	char* lpBuffer = SendStream.GetBuffer(sizeof(PktSRTAck));
	if (NULL == lpBuffer) { return false; }

	PktSRTAck* lpPktSRTAck = reinterpret_cast<PktSRTAck*>(lpBuffer);

	lpPktSRTAck->m_State.m_wIP	= DBData.m_Info.IP;
	lpPktSRTAck->m_State.m_wSTR	= DBData.m_Info.STR;
	lpPktSRTAck->m_State.m_wDEX	= DBData.m_Info.DEX;
	lpPktSRTAck->m_State.m_wCON	= DBData.m_Info.CON;
	lpPktSRTAck->m_State.m_wINT	= DBData.m_Info.INT;
	lpPktSRTAck->m_State.m_wWIS	= DBData.m_Info.WIS;

	lpPktSRTAck->m_Skill		= DBData.m_Skill;

	lpPktSRTAck->m_ItemPos		= InvenPos;
	lpPktSRTAck->m_dwGold		= dwGold;

	return SendStream.WrapCrypt(sizeof(PktSRTAck), CmdCharStatusRetrain, 0, usError);
}
