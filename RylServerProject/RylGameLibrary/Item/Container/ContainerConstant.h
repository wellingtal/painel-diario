#ifndef _RYL_GAME_LIB_CONTAINER_CONSTANTS_H_
#define _RYL_GAME_LIB_CONTAINER_CONSTANTS_H_

namespace ContainerConstant
{
	enum Const
	{
		QUEST_INVENTORY_TAB		= 3		// �κ��丮�� ����Ʈ ������ �� (4��° ��)
	};

	enum Size
    {   
		// -------------------------------------
		// ĳ����

        INVENTORY_WIDTH		= 8,    
        INVENTORY_HEIGHT	= 13,    
        MAX_INVENTORY_TAB	= 4,    // �κ��丮

        EXCHANGE_WIDTH		= 8,    
        EXCHANGE_HEIGHT		= 4,    // ��ȯâ

        DEPOSIT_WIDTH       = 8,    
        DEPOSIT_HEIGHT      = 12,   
        MAX_DEPOSIT_TAB     = 4,    // â��

        STALL_WIDTH			= 10,	
        STALL_HEIGHT		= 8,	// ������

		// -------------------------------------
		// ��� ��� ����

		CAMPSHOP_WIDTH		= 10, 
		CAMPSHOP_HEIGHT		= 8,	// �Ǹ� ���
	};
};

#endif