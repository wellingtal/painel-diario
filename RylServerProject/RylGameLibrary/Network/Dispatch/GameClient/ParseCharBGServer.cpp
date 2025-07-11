
#include "stdafx.h"

#include <Log/ItemLog.h>
#include <Utility/Setup/ServerSetup.h>

#include <Network/Dispatch/ParseUtils.h>
#include <Network/Dispatch/DBAgent/RegularAgentDispatch.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/BGServerPacket.h>

#include <Creature/CreatureManager.h>
#include <Map/FieldMap/VirtualArea/VirtualAreaMgr.h>

#include "ParseCharBGServer.h"
#include "GameClientDispatch.h"


// ��Ʋ �׶��� ������ �� ����Ʈ ��û
bool GameClientParsePacket::ParseCharBGServerMapList(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	PktBGServerMapList*		lpPktBGServerMapList	= reinterpret_cast<PktBGServerMapList *>(lpPktBase);
	CCharacter*				lpCharacter				= GameClientDispatch.GetCharacter();

	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	VirtualArea::CVirtualAreaMgr::GetInstance().SendBGServerMapList(lpCharacter);
	return true;
}


// ��Ʋ �׶��� ������ �� ��� ����Ʈ
bool GameClientParsePacket::ParseCharBGServerResultList(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	PktBGServerResultList*	lpPktBGServerResultList	= reinterpret_cast<PktBGServerResultList *>(lpPktBase);
	CCharacter*				lpCharacter				= GameClientDispatch.GetCharacter();

	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	VirtualArea::CVirtualAreaMgr::GetInstance().SendBGServerResultList(lpCharacter);
	return true;
}


// ��Ʋ �׶��� ������ �� �̵�
bool GameClientParsePacket::ParseCharBGServerMoveZone(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	PktBGServerMoveZone*	lpPktBGServerMoveZone	= reinterpret_cast<PktBGServerMoveZone *>(lpPktBase);
	CCharacter*				lpCharacter				= GameClientDispatch.GetCharacter();

    CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	if (0 == lpPktBGServerMoveZone->m_wMapNumber)
	{
		// ���Ƿ� �̵�
		VirtualArea::CVirtualAreaMgr::GetInstance().LeaveVirtualArea(lpCharacter);
	}
	else
	{
		// �ش� ��(��) ��ȣ�� �ش��ϴ� ��Ʋ �׶��� ������ �̵�
		unsigned short wMapIndex	= lpPktBGServerMoveZone->m_wMapNumber | VirtualArea::BGSERVERMAP;		// �̵��Ϸ��� MapIndex
		unsigned char cMoveType		= lpPktBGServerMoveZone->m_cMoveType;

		VirtualArea::CVirtualAreaMgr::GetInstance().EnterVirtualArea(lpCharacter, wMapIndex, cMoveType);
	}

	return true;
}


// ��Ʋ �׶��� ������ ȯ���� ���
bool GameClientParsePacket::ParseCharBGServerMileageChange(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	PktBGServerMileageChange*	lpPktBGServerMileageChange	= reinterpret_cast<PktBGServerMileageChange *>(lpPktBase);
	CCharacter*					lpCharacter					= GameClientDispatch.GetCharacter();

	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	unsigned long dwCID		= lpPktBGServerMileageChange->m_dwCID;
	unsigned char cGroup	= lpPktBGServerMileageChange->m_cGroup;
	unsigned char cCmd		= lpPktBGServerMileageChange->m_cCmd;
	unsigned long dwMileage	= lpPktBGServerMileageChange->m_dwMileage;

	switch (cCmd)
	{
		case PktBGServerMileageChange::MC_REQUEST:
			break;

		case PktBGServerMileageChange::MC_CHANGE:
			RegularAgentPacketParse::SendSetCharData(dwCID, dwMileage, cGroup);
			break;

		default:
			ERRLOG2(g_Log, "CID:0x%08x �߸��� ȯ���� ����Դϴ�. Cmd:%d", dwCID, cCmd);
			break;
	}

	return true;
}


// ��Ʋ �׶��� ���� ���� ĳ���� ���� ��û
bool GameClientParsePacket::ParseCharBGServerCharSlot(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	PktBGServerCharSlot*	lpPktBGServerCharSlot	= reinterpret_cast<PktBGServerCharSlot *>(lpPktBase);
	CCharacter*				lpCharacter				= GameClientDispatch.GetCharacter();

	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	unsigned long dwCID		= lpPktBGServerCharSlot->m_dwCID;
	unsigned char cGroup	= lpPktBGServerCharSlot->m_cGroup;

	RegularAgentPacketParse::SendGetCharSlot(dwCID, cGroup);

	return true;
}


