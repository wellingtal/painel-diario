#ifndef _CHAR_ATTACK_PACKET_STRUCT_H_
#define _CHAR_ATTACK_PACKET_STRUCT_H_

#include <DB/DBDefine.h>

#pragma pack(1)

// ����� ����	( ���� �������� ó�� �� ������ ����ü )
typedef struct AtNode *LPAtNode;
struct AtNode
{
	enum 
	{ 
		MAX_MONSTER_DEFENDER_NUM	= 5,
		MAX_DEFENDER_NUM			= 10 
	};

	unsigned char	m_wDefenserNum;                     // ����� ��
	unsigned long	m_dwDefenser[MAX_DEFENDER_NUM];     // ����� ���̵�
	unsigned char	m_cDefenserJudge[MAX_DEFENDER_NUM]; // ����� ����
};

// ���� ����
typedef struct AtType* LPAtType;
struct AtType 
{
	enum 
	{ 
		SKILL_BIT	= 0x8000 
	};

	enum AttackType
	{
		RIGHT_MELEE	= 0, 
		LEFT_MELEE	= 1,

		BOW         = 3,
		CROSSBOW	= 4,

		DEFENCE		= 5,
		GUARD		= 6
	};

	unsigned short	m_wType;                    // ���� Ÿ�� (�Ǵ� ��ų ID)

	unsigned char	m_cCasting			: 1;	// ĳ���� (���� Ÿ���� ��ų ID�� ����ϹǷ� ������ ������ �Ӵϴ�.)
	unsigned char	m_cAtCount			: 3;    // ���� ī��Ʈ (0~7)
	unsigned char	m_cSkillLockCount	: 3;    // ��ų ��ī��Ʈ (0~7)
	unsigned char	m_cMissileAttack	: 1;	// ��ź�� �������� ��ġ�� ���� ���� (Ư�� Ÿ���� �ƴ�)

	char			m_cSkillLevel;			    // ��ų ���� (-127 ~ 128)

	POS				m_DstPos;					// ���� ������ ��ź�� ������ ��ġ
};

// ����� ���
typedef struct DefenserNode* LPDefenserNode;
struct DefenserNode 
{
	unsigned long	m_dwCharID;     // ����� ĳ���� ���̵�
	
	unsigned short	m_wMaxHP;	    // �ִ� HP
	unsigned short	m_sCurrHP;		// ���� HP
	unsigned short	m_wMaxMP;		// �ִ� MP
	unsigned short	m_sCurrMP;      // ���� MP
	unsigned short	m_wMPHeal;		// MP ȸ����

    unsigned short	m_wDamage;      // ������ ��
    unsigned char	m_cJudge;       // ���� ��
};

#pragma pack()

#endif