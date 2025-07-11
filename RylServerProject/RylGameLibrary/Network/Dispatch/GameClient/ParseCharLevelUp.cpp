
#include "stdafx.h"
#include "ParseCharLevelUp.h"
#include "SendCharLevelUp.h"
#include "GameClientDispatch.h"

#include <Network/Dispatch/ParseUtils.h>
#include <Network/Packet/PacketStruct/CharStatusPacket.h>
#include <Creature/Character/Character.h>


// 클래스 업그레이드
bool GameClientParsePacket::ParseCharClassUpgrade(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
    CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktCU), GameClientDispatch);

	PktCU*      lpPktCU     = static_cast<PktCU *>(lpPktBase);
    CCharacter* lpCharacter = GameClientDispatch.GetCharacter();
    
    CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	unsigned long dwCharID = lpPktCU->m_dwCharID;
	unsigned char cClass = lpPktCU->m_cClass;

	return lpCharacter->ChangeClass(cClass);
}


// IP 증가 패킷
bool GameClientParsePacket::ParseCharIncreasePoint(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
    CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktIP), GameClientDispatch);

	PktIP*      lpPktIP     = static_cast<PktIP *>(lpPktBase);
    CCharacter* lpCharacter = GameClientDispatch.GetCharacter();
    
    CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	unsigned long dwCharID = lpPktIP->m_dwCharID;
	unsigned char cStateType = lpPktIP->m_cStateType;

	// 포인트 증가
	ChState State = {0,};
	unsigned short usError = lpCharacter->AddState(cStateType, State);

	return GameClientSendPacket::SendCharIncreasePoint(GameClientDispatch.GetSendStream(), dwCharID, State, usError);
}


// 스탯 재분배 (클래스 처음 상태로)
bool GameClientParsePacket::ParseCharStateRedistribution(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktSR), GameClientDispatch);

	PktSR*      lpPktSR     = static_cast<PktSR *>(lpPktBase);
	CCharacter* lpCharacter = GameClientDispatch.GetCharacter();

	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	ChState State = lpPktSR->m_State;

	return lpCharacter->StateRedistribution(State);
}

// 스테이터스 재훈련 (스탯 일정량을 다시 분배)
bool GameClientParsePacket::ParseCharStatusRetrain(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase)
{
	CHECK_FIXED_PACKETSIZE(lpPktBase, sizeof(PktSRT), GameClientDispatch);

	PktSRT*      lpPktSRT   = static_cast<PktSRT *>(lpPktBase);
	CCharacter* lpCharacter = GameClientDispatch.GetCharacter();

	CHECK_CHARACTER_PTR(lpCharacter, GameClientDispatch, lpPktBase->GetCmd());

	ChState State			= lpPktSRT->m_State;
	Item::ItemPos InvenPos	= lpPktSRT->m_ItemPos;

	return lpCharacter->StatusRetrain(State, InvenPos);
}
