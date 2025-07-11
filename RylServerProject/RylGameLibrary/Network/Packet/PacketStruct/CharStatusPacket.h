#ifndef _CHAR_STATUS_PACKET_H_
#define _CHAR_STATUS_PACKET_H_

// CharStatusPacket.h
#include <Network/Packet/PacketBase.h>
#include <DB/DBdefine.h>
#include <Item/ItemStructure.h>

#include "CharStatusPacketStruct.h"

#pragma pack(1)

// ĳ���� �ڻ�
typedef struct PktSC* LPPktSC;
struct PktSC : public PktBase
{
	unsigned long	m_dwCharID;	        // ĳ���� ���̵�
};

// ĳ���� �ڻ� Ack
typedef struct PktSCAck PktSCAck, *LPPktSCAck;
struct PktSCAck : public PktBase 
{
    POS				m_Pos;
	unsigned long	m_dwCharID;		    // ĳ���� ���̵�
};

// ���ε� ������
typedef struct PktBP* LPPktBP;
struct PktBP : public PktBase 
{
	enum PktBPCmd
	{
		BP_SAVE_POSITION	= 0,
		BP_WARP				= 1,
		BP_RECALL			= 2
	};

  	POS				m_Pos;              // ���ε��� ��ġ
	char			m_cZone;			// ���ε��� ��

	unsigned long	m_dwNPCID;          // ���ε��� NPC ID
	unsigned char	m_cCmd;             // ������ ���		(0 = ���̺� ��ġ, 1 = ��ġ �̵�)
};

// ������
typedef struct PktLU* LPPktLU;
struct PktLU : public PktBase 
{
	ChState			m_State;
	unsigned long	m_dwCharID;         // ĳ���� ���̵�
	unsigned char	m_cLevel;	        // ����
};

// ������ ����
typedef struct PktLUInfo* LPPktLUInfo;
struct PktLUInfo : public PktBase
{
	unsigned long	m_dwCharID;         // ĳ���� ���̵�
	unsigned char	m_cLevel;	        // ĳ���� ����
};

// ���� �ʱ�ȭ
typedef struct PktSR* LPPktSR;
struct PktSR : public PktBase
{
	ChState			m_State;
};

// ���� �ʱ�ȭ
typedef struct PktSRAck* LPPktSRAck;
struct PktSRAck : public PktBase
{
	enum PktSRAckErr
	{
		FAIL_NO_CHANCE	= 2
	};

	ChState			m_State;
	SKILL			m_Skill;
};

// �������ͽ� ��й�
typedef struct PktSRT* LPPktSRT;
struct PktSRT : public PktBase
{
	ChState			m_State;
	Item::ItemPos   m_ItemPos;          // ������ �� ��ġ
};

// �������ͽ� ��й� Ack
typedef struct PktSRTAck* LPPktSRTAck;
struct PktSRTAck : public PktBase
{
	enum PktSRTAckErr
	{
		FAIL_INVALID_IP			= 2,	// ��й� �� �� �ִ� ����Ʈ�� �ʰ��Ͽ� ��й��Ϸ� �� ���
		FAIL_NO_ITEM			= 3,	// ��û�� ��ġ�� ������(������ ��)�� ���� (Ȥ�� ������ �̻��ϰų� �������� ������ ���ſ� ������ ���)
		FAIL_INVALID_ITEM		= 4,	// ��û�� ��ġ�� �ִ� �������� �ùٸ� ������(������ ��)�� �ƴ� ���
		FAIL_NOT_ENOUGH_GOLD	= 5,	// �ݾ� ����
		FAIL_INVALID_STATUS		= 6		// ��й�� ���ȿ� ������ �ִ� ��� (�� IP�� ���� �ʰų� Ŭ���� �ּ� ���ȿ� �� ��ġ�� ���)
	};

	enum Const
	{
		MAX_STATUS_RETRAIN	= 800
	};

	ChState			m_State;
	SKILL			m_Skill;
	Item::ItemPos   m_ItemPos;          // ������ �� ��ġ
	unsigned long	m_dwGold;           // �Һ� �ݾ�
};

// �ɷ�ġ ����
typedef struct PktIP* LPPktIP;	
struct PktIP : public PktBase 
{
	unsigned long	m_dwCharID;         // ĳ���� ID
	unsigned char	m_cStateType;       // �ɷ�ġ Ÿ��
};

// �ɷ�ġ ���� Ack
typedef struct PktIPAck* LPPktIPAck;	
struct PktIPAck : public PktBase 
{
    ChState			m_State;
	unsigned long	m_dwCharID;	        // ĳ���� ID
};

// Ŭ���� ���׷��̵�
typedef struct PktCU* LPPktCU;
struct PktCU : public PktBase
{
	unsigned long	m_dwCharID;         // ĳ���� ID
	unsigned char	m_cClass;           // Ŭ���� ��ȣ
};

// Ŭ���� ���׷��̵� Ack
typedef struct PktCUAck* LPPktCUAck;
struct PktCUAck : public PktBase 
{
	ChState			m_State;            // ĳ���� ����
	unsigned long	m_dwCharID;         // ĳ���� ID
	unsigned long	m_dwFame;			// ��
	unsigned char	m_cLevel;			// ����
	unsigned char	m_cClass;	        // Ŭ���� ��ȣ
};

// �׽�Ʈ�� �̺�Ʈ
// edith 2009.09.05 �ű� �̺�Ʈ ���ۺκ�
typedef struct PktExtraEvent* LPPktExtraEvent;
struct PktExtraEvent : public PktBase 
{
	enum PktExtraEventErr
	{
		EVENT_NONE					= 2,	// ó������ �ʴ� �̺�Ʈ 
		EVENT_CLEAR					= 3,	// Ŭ�����ߴ�.
		FAIL_EVENT					= 4,	// �̺�Ʈ ����
		FAIL_REWARD					= 5,	// �̺�Ʈ ���� ����
	};

	unsigned long	m_dwUserID;         // ���� ID
	unsigned long	m_dwCharID;	        // ĳ���� ID
	unsigned long	m_dwEventID;		// �̺�Ʈ ID
	unsigned long	m_dwValue1;			// �߰���
	unsigned long	m_dwValue2;			// �߰���
};

// ��ų (����/�����/��/������)
typedef struct PktSk* LPPktSk;
struct PktSk : public PktBase 
{
	enum PktSkError
	{	
		// TODO : ��ġ�� ������� Ŭ���̾�Ʈ�� �����Ͽ� ����ũ�ϰ� �����սô�.

		// ��ų ���� ���� ����
		FAIL_NOT_SKILL				= 2,									
		FAIL_ERASE_CLASS_SKILL		= 5,	// Ŭ���� ��ų ����
		FAIL_NO_ITEM				= 6,	// ��û�� ��ġ�� ������(������ ��)�� ���� (Ȥ�� ������ �̻��ϰų� �������� ������ ���ſ� ������ ���)
		FAIL_INVALID_ITEM			= 7,	// ��û�� ��ġ�� �ִ� �������� �ùٸ� ������(������ ��)�� �ƴ� ���

		// ���� ���� 
		FAIL_MAX_LEVEL				= 2,	// �ִ� ������ (���ؾ���)
		FAIL_NOT_CURRENT_LOCK_COUNT	= 3,	// ���� �ܰ谡 �ƴ� ��ų
		FAIL_NOT_ENOUGH_SLOT		= 4,	// ���� ����
		FAIL_NOT_CURRENT_CLASS		= 5,	// �ش� Ŭ������ �ƴ�
		FAIL_NOT_ENOUGH_STATUS		= 6,	// ������ ���ڶ�
		FAIL_NOT_ENOUGH_SKILL_POINT	= 7,	// ��ų ����Ʈ ����

		// ����� ����
		FAIL_NON_LEVEL				= 3,	// ������ ����
		FAIL_ALREADY_LOCK			= 4,	// �̹� ���� ��ų

		// �� ����
		FAIL_FULL_LOCK				= 3,	// ���� ����� ��ų
		FAIL_NOT_ENOUGH_LEVEL		= 4,	// ������ �� �������� ��ų

		// �� ���� ����
		FAIL_NON_LOCK				= 3,	// ������ ���� ��ų
		FAIL_FIFTHSKILL_UNLOCK		= 4		// 5�ܰ� ��ų�� ������ 4�ܰ�� ������ ����.


	};

	unsigned long	m_dwCharID;         // ĳ���� ID
	unsigned short	m_wSkill;           // ��ų ��ȣ
	unsigned char	m_cIndex;           // ��ų ��ġ
	unsigned char	m_cSkillLevel;		// ��ų ����

	Item::ItemPos   m_ItemPos;          // ������ �� ��ġ
};
/*
// ��ų �� ����
typedef struct PktSkUL* LPPktSkUL;
struct PktSkUL : public PktSk 
{
    Item::ItemPos   m_itemPos;          // ������ �� ��ġ PktSk�� ����
};
*/
// ��ų �� ���� Ack
typedef struct PktSkULAck* LPPktSkULAck;
struct PktSkULAck : public PktSk 
{
    unsigned long	m_dwGold;           // �Һ� �ݾ�
//    Item::ItemPos   m_itemPos;          // ������ �� ��ġ PktSk�� ����
};

// HP ���� ��Ŷ	( Char HP Regen )
typedef struct PktHP* LPPktHP;
struct PktHP : public PktBase 
{
	unsigned long	m_dwCharID;         // ĳ���� ���̵�	
	unsigned short	m_sHP;              // ���� hp
	unsigned short	m_sMP;              // ���� mp
};

// ���� ��û	( Require Info )
//	- P2P�� �ٸ� Ŭ���̾�Ʈ���� ���� ���� �ÿ��� �ּҳ����� �߰� �Ǹ�,
//		�ش� Ŭ���̾�Ʈ ���̵�� ������ ���� ĳ���� ���̵� �ǰڴ�.
typedef struct PktRI* LPPktRI;
struct PktRI : public PktBase
{
	SOCKADDR_IN		m_PublicAddress;
	SOCKADDR_IN		m_PrivateAddress;

	unsigned long	m_dwCharID;         // �ش� ĳ���� ���̵�
	unsigned char   m_cCmd;             // Ŀ�ǵ� (0 : HP, MP ����, 1 : UDP �ּ�, 2 : ���)
};

// ĳ���� �Ѹ�� ����
typedef struct PktSI* LPPktSI;
struct PktSI : public PktBase
{
	enum { MAX_STALL_NAME_LEN = 32 };

	enum WEAPON_HAND
    { 
		WEAPON_HAND1 = 1,
        WEAPON_HAND2 = 2 
	};

	CHAR_VIEW		m_CharView;         // ĳ���� �Ѹ��
	unsigned long	m_dwStatusFlag;     // ĳ���Ϳ� �ɷ� �ִ� chant, enchant Flag
	bool			m_bPeace;			// ���� ���
	unsigned char	m_cSelectedHands;   // ���� ������� �� (1 = ����1 & ����1, 2 = ����2 & ����2)

	char			m_StallName[MAX_STALL_NAME_LEN];	// ������ �̸�
};

// ����Ʈ ���ʽ�
typedef struct PktEB* LPPktEB;
struct PktEB : public PktBase 
{
	char			m_cEliteBonus;		// ����Ʈ ���ʽ�
};

#pragma pack()

#endif