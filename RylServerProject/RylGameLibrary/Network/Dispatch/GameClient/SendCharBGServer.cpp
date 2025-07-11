
#include "stdafx.h"
#include "SendCharBGServer.h"

#include <Creature/Character/Character.h>
#include <Creature/CreatureManager.h>

#include <Network/Dispatch/GameClient/GameClientDispatch.h>
#include <Network/Stream/SendStream.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/BGServerPacket.h>



// 배틀 그라운드 서버군 존 이동 (방이동)
bool GameClientSendPacket::SendCharBGServerMoveZone(CSendStream& SendStream, unsigned char cZone, unsigned char cMoveType, unsigned short wError)
{
	char* lpBuffer = SendStream.GetBuffer(sizeof(PktBGServerMoveZone));
	if (NULL != lpBuffer)
	{
		PktBGServerMoveZone* lpPktBGSMZ = reinterpret_cast<PktBGServerMoveZone *>(lpBuffer);

		lpPktBGSMZ->m_wMapNumber	= 0;
		lpPktBGSMZ->m_cMoveType		= cMoveType;
		lpPktBGSMZ->m_cZone			= cZone;

		return SendStream.WrapCrypt(sizeof(PktBGServerMoveZone), CmdBGServerMoveZone, 0, wError);
	}

	return false;
}

// 배틀 그라운드 서버군 환전소 명령
bool GameClientSendPacket::SendCharBGServerMileageChange(CSendStream& SendStream, unsigned long dwCID, unsigned char cGroup, 
														 unsigned char cCmd, unsigned long dwGold, unsigned long dwMileage, unsigned short wError)
{
	char* lpBuffer = SendStream.GetBuffer(sizeof(PktBGServerMileageChange));
	if (NULL != lpBuffer)
	{
		PktBGServerMileageChange* lpPktBGSMC = reinterpret_cast<PktBGServerMileageChange *>(lpBuffer);

		lpPktBGSMC->m_dwCID = dwCID;
		lpPktBGSMC->m_cGroup = cGroup;
		lpPktBGSMC->m_cCmd = cCmd;
		lpPktBGSMC->m_dwGold = dwGold;
		lpPktBGSMC->m_dwMileage = dwMileage;

		return SendStream.WrapCrypt(sizeof(PktBGServerMileageChange), CmdBGServerMileageChange, 0, wError);
	}

	return false;
}
