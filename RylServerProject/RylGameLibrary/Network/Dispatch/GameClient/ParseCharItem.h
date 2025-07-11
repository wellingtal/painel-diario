#ifndef _PARSE_CHAR_ITEM_H_
#define _PARSE_CHAR_ITEM_H_

// 전방 참조
struct PktBase;
struct PktTr;

namespace Item
{
	struct ItemPos;
	class CItem;
};

class CGameClientDispatch;

namespace GameClientParsePacket
{
    bool ParseCharTakeItem(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);		// 아이템 이동 및 스택
    bool ParseCharSwapItem(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);		// 아이템 스왑
    bool ParseCharRepairItem(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);		// 아이템 수리
	bool ParseCharRepairAllItem(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);	// 아이템 수리 (장비 아이템 모두 수리)
    bool ParseCharUseItem(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);			// 아이템 사용
    bool ParseCharUseCashItem(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);		// 캐쉬 아이템 사용
	

	bool ParseCharTradeItem(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);		// 아이템 거래
	bool ParseCharEquipShopInfo(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);	// NPC 장비 상점 정보

	bool ParseCharPickUp(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);			// 아이템 집기
    bool ParseCharPullDown(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);		// 아이템 떨구기
    bool ParseCharSplitItem(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);		// 아이템 나누기

	bool ParseCharTakeGold(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);		// 돈 다루기
    bool ParseCharDepositCmd(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);		// 창고 관련 패킷들
	bool ParseCharAutoRouting(CGameClientDispatch& GameClientDispatch, PktBase* lpPktBase);		// 오토 루팅
};


#endif