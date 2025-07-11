#ifndef _CHAR_ITEM_PACKET_H_
#define _CHAR_ITEM_PACKET_H_

// CharItemPacket.h
// ItemStructure.h�� �����ؾ� �Ѵ�. (����, ������ ���)

#include <Item/ItemStructure.h>
#include <Network/Packet/PacketBase.h>
#include <Network/Packet/PacketStruct/DataPacket.h>

#include "CharItemPacketStruct.h"

#pragma pack(1)

// ���� (���� �ʿ�)
struct PktPU : public PktBase
{
    unsigned __int64    m_nObjectID;    // ������Ʈ ���̵�
	unsigned long	    m_dwCharID;     // ĳ���� ���̵�
    Item::ItemPos       m_itemPos;      // ������ ��ġ
};

// ���� Ack
struct PktPUAck : public PktBase
{
	enum ObjectType { Item = 0, Gold = 1 };
	enum PktPUAckError 
    {
		NO_SERVER_ERR			= 0,
        SERVER_ERROR			= 1,
        FAIL_GET_CELL			= 2,
        FAIL_GET_ITEM			= 3,
        FAIL_PICKUP_INVEN		= 4,
		FAIL_NOT_OWNER_OF_ITEM	= 5,
		FAIL_NOT_ITEM			= 6,
		FAIL_CREATE_ITEM		= 7,
		FAIL_AUTO_ROUTING		= 8,
		FAIL_GOLD_OVERFLOW		= 9
    };

    unsigned __int64	m_nObjectID;	// ������Ʈ ���̵�
	unsigned long		m_dwCharID;	    // ĳ���� ���̵�	
	unsigned long		m_dwSize;       // Objectũ��(Ȥ�� �� ��)

    Item::ItemPos		m_itemPos;      // �������� ���� ��ġ
	unsigned char		m_cType;        // Object����(������ Ȥ�� ��)
   	unsigned char		m_cNum;			// ������ ����
};

// ���� ����
struct PktPUInfo : public PktBase
{
    unsigned __int64	m_nObjectID;	// ������Ʈ ���̵�
	unsigned long		m_dwCharID;     // ĳ���� ���̵�	
};

// ���� ����
struct PktAutoRouting : public PktBase
{
	enum PktARCmd
	{
		ARC_ORDER		= 0,
		ARC_POSSIBLE	= 1,
		ARC_IMPOSSIBLE	= 2
	};

    unsigned __int64	m_nObjectID;	// ������Ʈ ���̵�
	unsigned long		m_dwCharID;     // ĳ���� ���̵�	

	unsigned short		m_wItemID;		// ������ ���̵�
	unsigned char		m_cNum;			// ������ ����

	Item::ItemPos		m_itemPos;      // ������ ��ġ

	unsigned char		m_cCmd;			// ���
};

// ������ (���� �ʿ�)
struct PktPD : public PktBase 
{
	unsigned long	m_dwCharID;		// ĳ���� ���̵�
    Item::ItemPos   m_itemPos;      // ������ ��ġ �� �ε���
    unsigned char   m_cNum;         // ������ ����
};

// ������ Ack
struct PktPDAck : public PktBase 
{
	enum PktPDAckError
    {	
		SERVER_ERROR	= 1,
        FAIL_GET_CELL	= 2,
        FAIL_DROP_INVEN	= 3,
        FAIL_DROP_FIELD	= 4,
        FAIL_ZERO_ITEM	= 5
	};

	unsigned long	m_dwCharID;     // ĳ���� ���̵�
    FieldObject		m_FieldObject;  // FieldObject
    Item::ItemPos   m_itemPos;      // ������ ��ġ �� �ε���
};

// ������ ����
struct PktPDInfo : public PktBase 
{
	unsigned long	m_dwCharID;
	FieldObject		m_FieldObject;
};

// �������
struct PktDisappearItem : public PktBase
{
	enum PktDICmd
	{
		DIC_DEFAULT			= 0,	// �Ϲ����� ������ ����
		DIC_LINKED_QUEST	= 1		// ����Ʈ ���� �������� ���� (�޼��� ��� �ʿ�)
	};

	unsigned long	m_dwCharID;		// ĳ���� ���̵�
	
	Item::ItemPos   m_itemPos;      // ������ ��ġ �� �ε���
	unsigned char   m_cNum;         // ������ ����

	unsigned char	m_cCmd;			// ���
};


/* TODO : REMOVE HERE 

�� ����
struct PktCeInfo : public PktBase
{
    struct Stall_Info
    {
        enum { MAX_STALL_NAME = 32 };

        unsigned long   m_dwCID;
        char            m_szStallName[MAX_STALL_NAME];
    };

	unsigned char   m_cItemNum;     // �ڿ� ������� ������ ����.
    unsigned char   m_cStallNum;    // �ڿ� ������� ���� �̸� ����.    
};
*/

// ������ ó�� (���� �ʿ�)
struct PktTI : public PktBase 
{
	enum PktTIError
    {
        SERVER_ERROR	= 1,
        FAIL_MOVE	    = 2,
		FAIL_MOVE_FIFTHSKILL_LOCKITEM = 3
    };

	unsigned long	m_dwCharID;     // ĳ���� ���̵�
	TakeType		m_TakeType;     // ������ ���� ����ü
};

// �� ó�� (���� �ʿ�)
struct PktTG : public PktBase
{
	enum PktTGError
    {
        SERVER_ERROR	= 1,
        FAIL_MOVE       = 2,
        GOLD_OVERFLOW   = 3
    };

	unsigned long	m_dwCharID;     // ĳ���� ���̵�
	unsigned long	m_dwGold;       // �� �׼�

    unsigned char	m_cSrcPos : 4;  // Src��ġ - TakeType ����
	unsigned char	m_cDstPos : 4;  // Dst��ġ - TakeType ����
};

// ������ ó�� (���� �ʿ�)
struct PktTIs : public PktBase 
{
    enum PktTlsError
    {
        SERVER_ERROR    = 1
    };

	unsigned long	m_dwCharID;     // ĳ���� ���̵�
	unsigned char	m_TakeNum;      // ������ ���� ����
};

// ������ ����
struct PktSwI : public PktBase 
{
	enum PktSwlError
    {
        SERVER_ERROR	= 1,
        FAIL_SRC_MOVE	= 2,
        FAIL_DST_MOVE	= 3,
        CANT_SWAP       = 4
    };

	unsigned long	m_dwCharID;     //	- ĳ���� ���̵�
	TakeType		m_SwapSrc;      //	- ���� �ҽ� ������ ���� ����ü
	TakeType		m_SwapDst;      //	- ���� ��� ������ ���� ����ü
};

// ������ �ŷ�
struct PktTr : public PktBase
{
	enum PktTrError
    {
        FAIL_DROP_INVEN		= 2,		// �κ��丮�� �������� ���� �ȱ� ����
        FAIL_WRONG_NPC		= 3,		// �߸��� NPC
        FAIL_ITEM_BUY		= 4,		// ������ ���� ����
        FAIL_PUT_INVEN		= 5,		// �κ��丮�� ������ ���� ����
		FAIL_FULL_INVEN		= 6,		// �κ��丮�� �� �� ����
		FAIL_ENEMY_ITEM		= 7,		// �����ð��� ���� ��� ��� �������� ������ ���� ����.
		FAIL_GOLD_OVERFLOW	= 8			// �ŷ��� �ִ� ������ �ʰ�
    };

	enum PktTrCmd
	{
		TRC_BUY_WITH_GOLD			= 0,	// ������ ���� (�Ϲ� ����)
		TRC_BUY_WITH_MILEAGE		= 1,	// ������ ���� (�޴� ����)
		TRC_SELL					= 2,	// ������ �Ǹ�
		TRC_BUY_EQUIP				= 3,	// ���� �ɼ��� ���� ��� ����
		TRC_BUY_WITH_SKILL_COUPON	= 4		// ������ ���� (���� Ʈ���̳�)
	};

	unsigned long	m_dwCustomerID;	// �մ� ���̵�
	unsigned long	m_dwOwnerID;	// ���� ���� ���̵� (NPC �Ǵ� ĳ����)

	unsigned char	m_cCmd;			// ���

	unsigned short	m_wBuyItemID;   // ����� �ϴ� ������ ���� ID
	TakeType		m_TakeType;		// ������ ���� ����ü

	Item::ItemPos   m_CouponPos;	// ��ų�� ���� ��ġ (TRC_BUY_WITH_SKILL_COUPON ����� ��� �� ��� ������ ����Ѵ�.)

	unsigned short	m_wSize;		// ������ ũ�� (TRC_BUY_EQUIP ����� ��� '�������� ������'�� ���ٴ´�.)

	// Rodin : ������ ��ü�� �ƴ� �Ӽ� �迭�� �־� ��Ŷ ���� üũ�� ���Ҿ� ��ŷ ������ ���̵��� �����սô�.
};

// ������ �ŷ� Ack (+ �������� ������)
struct PktTrAck : public PktBase 
{
    unsigned long   m_dwCharID;     // ĳ���� ���̵�
    unsigned long   m_dwNPCID;      // ���� NPC ���̵�

	unsigned long	m_dwGold;       // ���� ������
	unsigned long	m_dwMileage;	// ���� ���ϸ���

	Item::ItemPos   m_CouponPos;	// ��ų�� ���� ��ġ (TRC_BUY_WITH_SKILL_COUPON ����� ��� �� ��� ������ ����Ѵ�.)

	unsigned short	m_wSize;        // ������ ũ��
    Item::ItemPos   m_itemPos;      // ������ ��ġ
	unsigned char	m_cNum;			// ������ ����
};

// NPC ��� ���� ���� (+ Ack�ÿ� ���� ���� ������ ���)
struct PktEquipShopInfo : public PktBase
{
	unsigned long	m_dwCharID;		// ĳ���� ���̵�
	unsigned long	m_dwNPCID;		// NPC ���̵�

	unsigned long	m_dwTime;		// ��ǰ ������ �ð� (�׷��̵尡 ���� å���� �ð�)

	unsigned char	m_cRace;		// ����
	unsigned char	m_cTabPage;		// ��

	unsigned char	m_cNum;			// ������ ����
};

// �������� ���� (���� �ʿ�)
struct PktRpI : public PktBase
{
	enum Err
	{
		NOT_ENOUGH_GOLD	= 2,				// ������ �ִ� ���� ����
		FAIL_NOT_CREATURE = 10,				// ũ�������İ� �������� �ʽ��ϴ�.
		FAIL_NOT_NPCZONE = 11,				// NPC�� ����ȣ�� �ٸ��ϴ�.
		FAIL_NOT_REPAIR = 12,				// �����������̾���.
		FAIL_NOT_POSITEM = 13,				// �������� ��ġ������.
		FAIL_NOT_STACKABLE = 14,			// �Ҹ𼺾������̴�.
		FAIL_NOT_EQUIP = 15,				// ���������̾ƴϴ�.
		FAIL_FULL_DRUA = 16,				// �������� Ǯ�̴�.
	};

	enum Pos
	{
		EQUIPMENT       = 1,
		INVENTORY       = 2,
		EQUIPMENT_ALL   = 3,
		INVENTORY_ALL   = 4,
	};

	unsigned long	m_dwCharID;	    // ĳ���� ���̵�
	unsigned long	m_dwGold;       // ���� ���
    Item::ItemPos   m_itemPos;      // ���� ������ ��ġ
};

// ����ϰ� �ִ� ��� ������ ���� (���� �ʿ�)
struct PktRpAI : public PktBase
{
	enum Err
	{
		NOT_ENOUGH_GOLD	= 2,				// ������ �ִ� ���� ����
		FAIL_NOT_CREATURE = 10,				// ũ�������İ� �������� �ʽ��ϴ�.
		FAIL_NOT_NPCZONE = 11,				// NPC�� ����ȣ�� �ٸ��ϴ�.
		FAIL_NOT_REPAIR = 12,				// �����������̾���.
	};

	unsigned long	m_dwCharID;	    // ĳ���� ���̵�
	unsigned long	m_dwGold;       // ���� ���
};

// �������� ��� (���� �ʿ�)
struct PktUI : public PktBase
{
	enum PktUIAck
    {
        USE_FAILED					= 2,
												/* ���� ���� ������ ��� ���� */
		NOT_SIEGE_TIME				= 3,		// ���� �ð��� �ƴ�
		NOT_EXIST_GUILD				= 4,		// ��尡 ���ų�, ��忡 �������� �ʾ���
		NOT_MASTER					= 5,		// ��� �����Ͱ� �ƴ�
		NOT_ENOUGH_FAME				= 6,		// ��� �� ����
		ANOTHER_DEV_CAMP			= 7,		// ������, �����, �ı����� �ٸ� ������ ����
		INNER_RADIUS				= 8,		// �ٸ� ������ ���� �ݰ� �ȿ� ����
		OUTER_RADIUS				= 9,		// �Ʊ� ������ ���� �ۿ� ����
		NOT_ENOUGH_MATERIAL			= 10,		// ���� ���� ���� ���� ����
		IN_SAFETYZONE				= 11,		// ������ ����� ���� ����
		FAIL_PEACE_GUILD			= 12,		// ��ȭ ��� ���
		FAIL_CAPITAL_ZONE			= 13,		// ī�������� ���� ����
//		FAIL_ALMIGHTY_GROUND		= 13,		// ���� �������� ���� �Ҽ� ����
		FAIL_STONE_BATTLE			= 14,		// ��ũ ī�������� ���� �Ҽ� ����
		FAIL_EXIST_ANOTHER_WEAPON	= 15,		// �̹� ���� ������ ������
		FAIL_ALMIGHTY_PIRATE		= 16,		// ���� �������� ���� �Ұ�
		FAIL_REMAIN_3_DAY			= 17,		// 3���� ���� �ð��Ŀ� ����
		FAIL_REMAIN_2_DAY			= 18,		// 2���� ���� �ð��Ŀ� ����
		FAIL_REMAIN_1_DAY			= 19,		// 1���� ���� �ð��Ŀ� ����
		FAIL_POSITION				= 20,		// ��ġ�� �̻���

		NOT_EXIST_QUEST				= 21,		// ����Ʈ�� �������� ����
		FAIL_HAS_QUEST				= 22,		// �̹� ������ �ִ� ����Ʈ
		FAIL_FULL_QUEST				= 23,		// ���� �� �ִ� ����Ʈ���� �� á��
		FAIL_QUEST_LEVEL			= 24,		// ����Ʈ�� �����Ҽ� �ִ� ���� �ƴ�

		FAIL_NOT_GT_GUILD_WAR_TIME	= 25,		// ����� �ð��� �ƴ�

		OUTER_RADIUS_SIEGE			= 26,		// ���� �ۿ��� ���� ���⸦ ����
		FAIL_HAS_SIEGE_ARMS			= 27,		// 1�� �̻� ���� ���⸦ ���� �Ϸ��� �Ҷ�.		
		FAIL_MATERIAL_COUNT			= 28,		// ������ ���簡 �����Ҷ�.

		NOT_GUILD_MASTER			= 29,		// ��� �����Ͱ� �ƴ� ���.
		NOT_CASTLE_NATION			= 30,		// �������� ���.

		FAIL_MAX_SIEGE_OBJECT		= 31,		// ������ �ִ밪���� �����Ǿ��� ���.

		FAIL_NEWZONE				= 32,		// �ű��������� ���� �Ҽ� ����

		NOT_LIMIT_LEVEL				= 33		// 80 ���� �̸� �� ���.
    };

	unsigned long	m_dwSender;         // ����� ���̵�
	unsigned long	m_dwRecver;		    // ����� ���̵�
	Item::ItemPos   m_itemPos;          // ��� ������ ��ġ
    unsigned char   m_cRemainItemNum;   // ���� ������ ����. ����->Ŭ���̾�Ʈ�� ������ ��Ŷ�� ��쿡�� �ǹ� ����.
};

// ������Ʈ ������ ( ���� �ʿ� )
struct PktCO : public PktBase
{
    CastObject		m_sCastObject;
	unsigned long	m_dwSenderID;       // ������ ���
    unsigned long	m_dwReceiverID;	    // �޴� ���
};

// ������Ʈ ������ (Cast Object)
struct PktCOInfo : public PktBase
{
    CastObject		m_sCastObject;
	unsigned long	m_dwSenderID;       // ������ ���
    unsigned long	m_dwReceiverID;	    // �޴� ���
};

// ���� ��ġ
struct PktIS : public PktBase 
{
    unsigned long   m_dwCharID;			// ĳ���� ���̵�
    Item::ItemPos   m_equipPos;         // ��� ��ġ
    Item::ItemPos   m_gemPos;           // ���� ��ġ
};

// ���� ��ġ Ack (+ ������ ��ġ�� ��� ������)
struct PktISAck : public PktBase 
{
    unsigned long   m_dwCharID;			// ĳ���� ���̵�
    Item::ItemPos   m_equipPos;         // ��� ��ġ
    Item::ItemPos   m_gemPos;           // ���� ��ġ
	unsigned char	m_cSize;            // ������ ũ��
};

// �� ��ġ/����.
struct PktIRS : public PktBase
{
	enum
	{
		RUNE_INSTALL	= 0x02,
		RUNE_UNINSTALL	= 0x03,		
	};

	unsigned long	m_dwCharID;			// ĳ���� ���̵�.
	Item::ItemPos	m_equipPos;			// ��� ��ġ.
	Item::ItemPos	m_runePos;			// �� ��ġ.	
	unsigned char	m_cType;			// ��Ŷ Ÿ��(��ġ/����).
};

// �� ��ġ/���� Ack.
struct PktIRSAck : public PktBase
{
	enum
	{
		RUNE_INSTALL	= 0x02,
		RUNE_UNINSTALL	= 0x03,		
	};

	unsigned long   m_dwCharID;			// ĳ���� ���̵�
	Item::ItemPos	m_equipPos;			// ��� ��ġ.
	Item::ItemPos	m_runePos;			// �� ��ġ.
	unsigned char	m_cSize;			// ������ ũ��.
	unsigned char	m_cType;			// ��Ŷ Ÿ��(��ġ/����).
};

// ������ �ռ�
struct PktItemChemical : public PktBase
{
	unsigned long	m_dwCharID;			// ĳ���� ���̵�

	Item::ItemPos	m_pickkingPos;		// ��� �ִ� ������ ��ġ
	Item::ItemPos	m_targetPos;		// �ռ� ��� ������ ��ġ

	unsigned char	m_cPickkingNum;		// ��� �ִ� ������ ����
};

// ������ �ռ� Ack
struct PktItemChemicalAck : public PktBase
{
	unsigned long	m_dwCharID;			// ĳ���� ���̵�

	Item::ItemPos	m_pickkingPos;		// ��� �ִ� ������ ��ġ
	Item::ItemPos	m_targetPos;		// �ռ� ��� ������ ��ġ

	unsigned short	m_wPickkingID;		// ��� �ִ� ������ ���̵�
	unsigned char	m_cPickkingNum;		// ��� �ִ� �������� ���� ����

	unsigned char	m_cResultSize;		// �ռ� ����� ũ��
};

// ���׷��̵� ������
struct PktUgI : public PktBase
{
	unsigned long	m_dwNPCID;			// �������� NPC ���̵�
};

// ���׷��̵� ������ Ack (+ ������ ����ü)
struct PktUgIAck : public PktBase
{
	unsigned long	m_dwCharID;		        // ĳ���� ���̵�
    unsigned long   m_dwCurrentGold;        // ���� �κ��丮�� ���� �ݾ�, ������ �ǹ� ����
    unsigned char   m_cCurrentMineralNum;   // ���� ���� ���� ����, ������ �ǹ� ����.
    unsigned char   m_cValue;				// ���׷��̵� �߰� ����
	unsigned char	m_cSize;                // ��� ������ ũ��
};

// ������ �ɼ� �̽�
struct PktItemOptionGraft : public PktBase
{
	unsigned long	m_dwCharID;			// ĳ���� ���̵�
	unsigned char	m_cAttributeType;	// �̽��� �ɼ� ����
};

// ������ �ɼ� �̽� Ack (+ ������ ����ü)
struct PktItemOptionGraftAck : public PktBase
{
	unsigned long	m_dwCharID;			// ĳ���� ���̵�

	unsigned long	m_dwGold;			// ���� �ݾ�
	unsigned char	m_cSize;			// ��� ������ ũ��
};

// ������ ���� �Ǹ�
struct PktItemCompensation : public PktBase
{
	unsigned long	m_dwCharID;			// ĳ���� ���̵�
};

// ������ ���ø� ��û (���� �ʿ�)
struct PktSplt : public PktBase
{
	unsigned long	m_dwCharID;     // ĳ���� ���̵�
	TakeType		m_TakeType;	    // ������ ���� ����ü 
                                    //  ( Src : ���� ������, Dst : ����� ���� �ڸ�(�׻� ��� �־�� ��), 
                                    //    Num : ���� ������ ����)
};

// ������ ���ø� Ack (���� �ʿ�) (+ �������� ������ ����ü (����� ���� ������ ����))
struct PktSpItAck : public PktBase 
{
	enum PktSpltAckError
    {
        SERVER_ERROR	= 1,
        FAIL_SPLIT	    = 2
    };

	unsigned long	m_dwCharID;     // ĳ���� ���̵�
	TakeType		m_TakeType;     // ������ ���� ����ü
	unsigned char   m_cSize;        // ������ ũ��
};

// �� ���� �̵�	(���� �ʿ�)
struct PktQSM : public PktBase 
{
	enum PktQSMError
    {
        SERVER_ERROR	= 1,
        FAIL_MOVE	    = 2
    };

	TakeType		m_TakeType;		
	unsigned short	m_usSkillID;
	unsigned char	m_cLockCount;
	unsigned char	m_cSkillLevel;
};

// ����ġ ���
struct PktSwEQ : public PktBase 
{
	unsigned char	m_cType;		// 0 : �ڵ�, 1 : ��
	unsigned char	m_cSelect;      // ���� �ڵ� ( 1 = ����1 & ����1, 2 = ����2 & ����2	) 
};

// ��ȯ ���
struct PktExC : public PktBase 
{
	enum ExchageCmd 
	{ 
		EXC_PROPOSE		= 0,	// ����
		EXC_ACCEPT		= 1,	// ����
		EXC_REFUSE		= 2,	// ����
		EXC_OK			= 3,	// Ȯ��
		EXC_CANCEL		= 4,	// Ȯ�� ���
		EXC_EXCHANGE	= 5,	// �ŷ� ����
		EXC_QUIT		= 6,	// �ŷ� ���
		EXC_LOCK		= 7,	// ��
		EXC_UNLOCK		= 8		// ���
	};

	enum PktExCError
	{
		FAIL_EXCHANGING_CHAR	= 2,	// �ٸ� ĳ���Ϳ� ��ȯ���� ĳ���Ϳ��� ��ȯ ��û
		FAIL_NOT_ALL_OK			= 3,	// ���� ��� Ȯ������ ���� ���¿��� �ŷ� ����
		FAIL_REJECT				= 4,	// �ź� �ɼ��� ���� ����
		FAIL_EXCHANGING_ID		= 5		// ������������ �ŷ��������� ���̵� Ʋ�����.
	};

	unsigned long	m_dwSenderID;   // ������ ���̵�
	unsigned long	m_dwRecverID;   // �޴��� ���̵�
    unsigned char   m_cCmd;         // ���
};

// ��ȯ ������ (+ ������ ����ü)
struct PktExI : public PktBase 
{
	enum Type
    {
		Item    = 0,
        Gold    = 1,
        Remove  = 2
	};

	unsigned long	m_dwCharID;     // ĳ���� ���̵�
   	unsigned long	m_dwSize;       // ������ ������ ũ��(Ȥ�� �� ��)
    Item::ItemPos   m_itemPos;      // ������ ��ġ
	unsigned char	m_cType;        // ������ ����(������ = 0, �� = 1)
    unsigned char   m_cNum;         // ������ ����(stack�������� ���, ������ ����)
};


namespace Deposit
{
    enum Const { PASSWORD_LENGTH = 4 };
};

// â�� (GameServer <--> DBAgent)
// �α��νÿ�, PktDBUpdate�� ���� ��, PktDepositUpdateDB�� ������ �ش�.
// ������ ��Ŷ�� m_bUpdateComplete�� true�� Set�� �ش�.

// ���� �߿��� ���� ������ ���÷� Update�� �Ѵ�.
struct PktDepositUpdateDB : public PktDD
{
    enum TabFlag
    {
        ITEM_TAB1       = (  1 << 0 ),
        ITEM_TAB2       = (  1 << 1 ),
        ITEM_TAB3       = (  1 << 2 ),
        ITEM_TAB4       = (  1 << 3 ),
        USED_DEPOSIT    = (  1 << 30),  // 0 : â�� ��� X,     1 : â�� �����.
        SAVED_PASSWORD  = (  1 << 31)   // 0 : NOSAVE           1 : SAVE
    };

    enum TabNum
    {
        TAB_12          = 0,
        TAB_34          = 1,
    };

    unsigned long   m_dwUID;            // ĳ���� UID
    unsigned long   m_dwCID;            // ĳ���� CID
    unsigned long   m_dwTabFlag;        // ���� ���� ���� ���¸� ��Ÿ���� bitset
    unsigned short  m_usDataSize;       // ��(Ȥ�� ��ȣ) ������ ũ��. ��ȣ�� ���� Deposit::PASSWORD_LENGTH�� ���ƾ� �Ѵ�.
    unsigned char   m_cTabNum;          // password�� ���� PASSWORD_TABNUM��
    bool            m_bUpdateComplete;  // ������Ʈ�� �Ϸ�Ǵ� ���, true�� set.
};

// â�� (Client <--> GameServer)
struct PktDeposit : public PktDD
{
    enum CMD
    {
        LOGIN       = 1,    // Ack����(Ack������ ����) (Client<->GameServer) szData[0]~[3]�� password, �� ������ ��ȣ ���� ���� 1byte (0, 1)
        BUY_TAB     = 2,    // Ack����(Ack������ ����) (Client<->GameServer) szData[0]�� ����� �� ��ȣ�� �־� �� 
        RETURN_TAB  = 3,    // Ack����(Ack������ ����) (Client<->GameServer) szData[0]�� ����� �� ��ȣ�� �־� ��, ���� �������� ����

        LOGOUT      = 4,    // Ack���� (Client->GameServer)   ������ ����. 
        CHANGE_PASS = 5,    // Ack���� (Client->GameServer)   szData[0]~[3]�� password. ������ ���¿����� ����.
        SAVED_PASS  = 6,    // Ack���� (GameServer -> Client) ���� �α��ν� ��. szData[0]~[3]�� Gold, [4]�� ��ȣ ���� ���� 1byte (0, 1)
                                
 		PASSWORD	= 7,	// AgentServer -> GameServer (CID, Password),   GameServer -> AgentServer (UID, Password)
		GOLD		= 8		// AgentServer -> GameServer (CID, Gold),       GameServer -> AgentServer (UID, Gold)
    };

    enum DepositError
    {        
        INSUFFICIENT_MONEY  = 2,
        AUTH_FAILED         = 3,
        INVALID_ARGUMENT    = 4
    };

    enum { MIN_DATA_SIZE = Deposit::PASSWORD_LENGTH + 4 };

    unsigned char   m_cCmd;
    char            m_szData[MIN_DATA_SIZE];
};

// ���� ���
struct PktLotteryResult : public PktBase
{
	enum PktLRErr
	{
		FAIL_BLANK		= 2			// �������� �ʴ� ��ǰ
	};

	unsigned long	m_dwCharID;
	unsigned long	m_dwSize;

	Item::ItemPos	m_itemPos;
};

#pragma pack()

#endif