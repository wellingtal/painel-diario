#ifndef _RYL_GAME_LIB_CONTAINER_CONSTANTS_H_
#define _RYL_GAME_LIB_CONTAINER_CONSTANTS_H_

namespace ContainerConstant
{
	enum Const
	{
		QUEST_INVENTORY_TAB		= 3		// 인벤토리의 퀘스트 아이템 탭 (4번째 탭)
	};

	enum Size
    {   
		// -------------------------------------
		// 캐릭터

        INVENTORY_WIDTH		= 8,    
        INVENTORY_HEIGHT	= 13,    
        MAX_INVENTORY_TAB	= 4,    // 인벤토리

        EXCHANGE_WIDTH		= 8,    
        EXCHANGE_HEIGHT		= 4,    // 교환창

        DEPOSIT_WIDTH       = 8,    
        DEPOSIT_HEIGHT      = 12,   
        MAX_DEPOSIT_TAB     = 4,    // 창고

        STALL_WIDTH			= 10,	
        STALL_HEIGHT		= 8,	// 노점상

		// -------------------------------------
		// 길드 요새 상점

		CAMPSHOP_WIDTH		= 10, 
		CAMPSHOP_HEIGHT		= 8,	// 판매 목록
	};
};

#endif