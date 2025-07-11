#ifndef _LOGCOMMANDS_H_
#define _LOGCOMMANDS_H_

#include "LogStruct.h"

namespace GAMELOG
{
	namespace CMD
	{
        // Depleted �׸���, ���� ���ϰ��� ȣȯ���� ���� ���ܵδ� �κ��̹Ƿ�
        // �� ������ �����ؼ��� �� �� ��.

        const CMDType CHAR_LOGIN			= 0x01;	// ĳ���� �α���
		const CMDType CHAR_LOGOUT			= 0x02;	// ĳ���� �α׾ƿ�
		const CMDType CHAR_CREATE			= 0x03;	// ĳ���� ����
		const CMDType CHAR_DELETE			= 0x04;	// ĳ���� ����
		const CMDType CHAR_DBUPDATE			= 0x05; // DB�� ������Ʈ �� �� ���� ��� ���Ϸ� ���
		const CMDType CHAR_LEVELUP			= 0x06;	// ĳ���� ���� ��
		const CMDType CHAR_BIND_POS			= 0x07; // ĳ���� ��ġ ���ε�
		const CMDType CHAR_DEAD				= 0x08;	// ĳ���� ���
		const CMDType CHAR_RESPAWN			= 0x09;	// ĳ���� ������
			
		const CMDType MOVE_ITEM				= 0x10;
		const CMDType SWAP_ITEM				= 0x11;
		const CMDType USE_ITEM				= 0x12;
		const CMDType SPLIT_ITEM			= 0x13;
		const CMDType PICKUP_ITEM			= 0x14;
		const CMDType DROP_ITEM				= 0x15;
		const CMDType BUY_ITEM				= 0x16;
		const CMDType SELL_ITEM				= 0x17;
		const CMDType BEFORE_EXCHANGE_ITEM	= 0x18;
		const CMDType AFTER_EXCHANGE_ITEM	= 0x19;
		const CMDType INSTALL_SOCKET_ITEM	= 0x20;
        const CMDType REPAIR_ITEM           = 0x21;
        const CMDType CHANGE_WEAPON         = 0x22;
        const CMDType TAKE_GOLD             = 0x23; // Depleted. 
        const CMDType UPGRADE_ITEM          = 0x24;

        // ������ ����. 2003/05/30 �߰� (by sparrowhawk)
        const CMDType STALL_OPEN_CLOSE              = 0x25;
        const CMDType STALL_ENTER_LEAVE             = 0x26;
        const CMDType STALL_ITEM_REGISTER_REMOVE    = 0x27;

        // �޴� �������� ������ ��� �߰�. ���� �ƴ϶� ���ϸ����� ���δ�. 2003/07/11 �߰� (by Sparrowhawk)
        const CMDType MEDAL_BUY_ITEM        = 0x28;

        // TakeGold Version2. �̵��� �ݾ� �� ������ �߰���.
        const CMDType TAKE_GOLD_V2          = 0x29;

        // ���͸�
        const CMDType USE_LOTTERY	        = 0x2A;

        // ��� �α� �߰� 2004/03/22 �߰� (by sparrowhawk)
        const CMDType GUILD_CREATE          = 0x30;     // ��� ����
        const CMDType GUILD_JOIN            = 0x31;     // ��� ���� ��û(���Դ���� ������ ���Ե�)
        const CMDType GUILD_MEMBER_LEVEL    = 0x32;     // ��� ��� ���� ����        
        const CMDType GUILD_LEAVE           = 0x33;     // ��� Ż�� (���� Ż�� Ȥ��, �ٸ� ����� Ż���Ŵ)
        
        const CMDType GUILD_RIGHTS_CHANGE   = 0x34;     // ��� ��ü ���� ����
        const CMDType GUILD_LEVEL_ADJUST    = 0x35;     // ��� ���� ����
        const CMDType GUILD_MARK_ADJUST     = 0x36;     // ��� ��ũ ���/����
        const CMDType GUILD_GOLD_CHANGE     = 0x37;     // ��� â�� �� �Ա�/���
                
        const CMDType GUILD_DISSOLVE        = 0x38;     // ��� �Ҹ�

        const CMDType ZONE_MOVE             = 0x39;     // �� �̵�

        const CMDType ITEM_ATTACH_OPTION    = 0x3A;     // ������ �ɼ� �̽�
        const CMDType ITEM_COMPENSATION     = 0x3B;     // ������ ����

        const CMDType TICKET_BUY_SKILLBOOK  = 0x3C;     // ��ų�� ��ȯ������ ��ų�� ����
        const CMDType UPGRADE_ITEM_V2       = 0x3D;     // ���׷��̵� ������ V2 (����/���п��� ��)

        const CMDType MONSTER_DEAD          = 0x3E;     // ���� ���(�� �׿� ������ ������ �α�)

		const CMDType FAME_GET_BATTLE		= 0x3F;		// ������ ���� ��
		const CMDType FAME_LOSE_BATTLE		= 0x40;		// ������ ���� ��

		const CMDType FAME_GET_CAMP			= 0x41;		// ����� ���� ��
		const CMDType FAME_LOSE_CAMP		= 0x42;		// ����� ���� ��

		const CMDType QUEST_GET_REWARD		= 0x43;		// ����Ʈ �������� ���� ����

		const CMDType CHANGE_RIDE           = 0x44;

		const CMDType ILLEGAL_ITEM          = 0x45;
		const CMDType ILLEGAL_WARPPOS       = 0x46;
		const CMDType HACK_DOUBT			= 0x47;

		const CMDType MAX_LOGCMD			= 0x48;     // ������ Ŀ�ǵ�
	};

    /*
     	���� : �� �̵��� ���õ� Ŀ�ǵ��
     
        const CMDType PICKUP_ITEM			= 0x14;
        const CMDType DROP_ITEM				= 0x15;
       
        const CMDType BUY_ITEM				= 0x16;
        const CMDType SELL_ITEM				= 0x17;
        const CMDType BEFORE_EXCHANGE_ITEM	= 0x18;
        const CMDType AFTER_EXCHANGE_ITEM	= 0x19;
        const CMDType REPAIR_ITEM           = 0x21;
        const CMDType TAKE_GOLD             = 0x23;
        const CMDType UPGRADE_ITEM          = 0x24; 

    */

    //  ���� ���� ���� ����...
	namespace ERR
	{
		const ERRType NO_LOG_ERR			= 0x00;	// ���� �ƴ�
		const ERRType CANNOT_UPDATE_DB		= 0x01;	// DB�� ������Ʈ �� �� �����ϴ�.
		const ERRType CANNOT_UPDATE_CLIENT	= 0x02;	// Ŭ���̾�Ʈ�� ������Ʈ �� �� �����ϴ�.
		const ERRType CANNOT_GET_DATA		= 0x03; // �����͸� ��� �� �� �����ϴ�.
	};

};


#endif