
#ifndef _PARSE_CHAR_UPGRADE_ITEM_H_
#define _PARSE_CHAR_UPGRADE_ITEM_H_


// 전방 참조
struct  PktBase;
class   CGameClientDispatch;

namespace GameClientParsePacket
{
    bool ParseCharInstallSocket(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);		// 아이템 인스톨 소켓
	bool ParseCharItemChemical(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);		// 아이템 합성
    bool ParseCharUpgradeItem(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);			// 아이템 업그레이드
	bool ParseCharItemOptionGraft(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);		// 아이템 옵션 이식
	bool ParseCharItemCompensation(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);	// 아이템 보상 판매
	bool ParseCharInstallRuneSocket(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);	// 룬 아이템설치, 제거.
};


#endif