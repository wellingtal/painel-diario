
#include "stdafx.h"

#include "SendCharSkill.h"

#include <Network/Stream/SendStream.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/CharStatusPacket.h>


bool GameClientSendPacket::SendCharSkillCommand(CSendStream& SendStream, unsigned long dwCharID, 
												PktBase::CMDType Cmd, unsigned char cIndex, unsigned short usSkillID, unsigned short usError, unsigned char cSkillLevel, Item::ItemPos* lpItem)
{
    char* lpBuffer = SendStream.GetBuffer(sizeof(PktSk));
	if (NULL == lpBuffer) { return false; }

	PktSk* lpPktSk = reinterpret_cast<PktSk*>(lpBuffer);

	lpPktSk->m_dwCharID		= dwCharID;
	lpPktSk->m_cIndex		= cIndex;
	lpPktSk->m_wSkill		= usSkillID;
	lpPktSk->m_cSkillLevel	= cSkillLevel;

	if(lpItem == NULL)
		lpPktSk->m_ItemPos	= Item::ItemPos(0, 0);
	else
		lpPktSk->m_ItemPos	= *lpItem;

    return SendStream.WrapCrypt(sizeof(PktSk), Cmd, 0, usError);
}

