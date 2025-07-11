#ifndef _SKILL_STRUCTURE_H_
#define _SKILL_STRUCTURE_H_

#pragma once

#include <Utility/TypeArray.h>
#include <Item/ItemStructure.h>

#pragma pack(8)

namespace Skill
{
    enum Const
	{ 
		SKILL_MASK			= 0x8000,
	
		ERROR_OF_DISTANCE	= 10		// �Ÿ� ���� ���� (��ų Ÿ�� �Ÿ� ���� ���)
	};

	namespace Type 
	{
		enum AbilityType
		{
			AB_ANIMAL_TALK		= 0,	// ����NPC�� ��ȭ
			AB_SOCIALEX			= 1,	// Ȯ�� �Ҽȸ�� ��밡��
			AB_NOFIRSTATK		= 2,	// ������ ���� �ȴ���
			AB_MOUNT_SPEEDUP	= 3,	// Ż�� �̼� ����
			AB_TEMPINVEN_TIMEUP = 4,	// �ӽ��κ��丮 �ð�����
			AB_GETUP_GOLD		= 5,	// ��� ȹ�淮 ����
			AB_RESPAWN_EX		= 6,	// Ȯ�� ������ ��� 
			AB_FOOD_UP			= 7,	// ���ķ� �ɷ�ġ ����
			AB_SHOP_MEMBERSHIP	= 8,	// ���� �����(���Ű��� �϶�, �ǸŰ��� ���)
			AB_CHEMICAL			= 9,	// ������ ȸ�������� ����(ĳ����)
			AB_ENDUR_SHILD		= 10,	// ������ �϶� ����
			AB_RECOVERY_UP		= 11,	// HP,MP ȸ���� ����
			AB_GATHER_FIND		= 12,	// �̴ϸʿ� �Դ��� �����ش�
			AB_MYEQUIP_UP		= 13,	// �ڽ��� ��� ȹ���� ����
			AB_LUCK_UP			= 14,	// ���� ����
			AB_EQUIP_LEVELDOWN	= 15,	// ����� ���� ������ �ٿ��Ų��.
			AB_HERBS			= 16,	// ���� ä��
			AB_MINING			= 17,	// ä��
			MAX_ABILITY_TYPE	= 18
		};

        enum SkillType
        {
		    NONE			= 0,
		    PASSIVE			= 1,
		    INSTANCE		= 2,
		    CAST			= 3,
		    CHANT			= 4,
		    ENCHANT			= 5,
			ITEM			= 6,
			SET				= 7,
			ACTION			= 8, 
			ABILITY			= 9,
			GATHER			= 10,

		    MAX_SKILL_TYPE	= 11
        };

		extern const CTypeName SkillTypes[MAX_SKILL_TYPE];

		enum DRCType
		{
			MAGIC			= 0,
			RIGHT_WEAPON	= 1,
			LEFT_WEAPON		= 2
		};
	};

	namespace UseLimit 
	{
        enum Type
        {
		    NONE					= 0,

		    // �ΰ�
		    FOR_FIGHTER				= 1, //  ���� ��� �Ǽ� Ȱ ���� ���Ұ�
		    WITH_SHIELD				= 2, //  ��������				: ���а� ������ ����� �� ����.
		    FOR_FIGHTER_TWOHANDED	= 3, //  ��չ�������(��������)	: ��չ��� ����ÿ��� ����� �����ϴ�. �������� ��쿡�� ����� �Ұ����ϴ�.
		    FOR_ALL_ONEHAND			= 4, //  �Ѽչ�������			: ��չ��⳪ �Ǽ����δ� ����� �Ұ����ϴ�. ��� ����
		    WITH_DAGGER				= 5, //  �������				: ��� ����ÿ��� ����� �����ϴ�.
		    WITH_THROWING_DAGGER	= 6, //  �����״�żҸ�			: �κ��丮�� ������ ��Ű� �������� ������ ����� �����ϴ�. (�տ� �����Ǿ� ���� �ʿ�� ����.)
		    RANGED					= 7, //  Ȱ/��������,ȭ��Ҹ�	: Ȱ�̳� ������ �����Ǿ� �ְ� �κ��丮�� ȭ���� �������� ������ ����� �����ϴ�.
    		
		    // ��ĭ		
		    LEFT_ARM	            = 8,  // ��������			: ������ �����ϰ� �־������ ����� �����ϴ�.
		    GUARD_ARM	            = 9,  // ���������			: ������ ���� �����ϰ� �־������ ����� �����ϴ�.
		    WITH_WEAPON	            = 10, // ��������			: �������� �Ǽ��� ������ ����� �Ұ����ϴ�.
		    ATTACK_ARM	            = 11, // ���þ�����			: ���ݿ��� ���� �����ϰ� �־������ ����� �����ϴ�.
		    GUN_ARM		            = 12, // �Ǿ����� ź��Ҹ�	: ��Ÿ����� ���� �����ϰ� ź���� ������ �־������ ����� �����ϴ�.
		    KNIFE_ARM	            = 13, // ������������		: ���������� ���� �����ϰ� �־������ ����� �����ϴ�.
		    WITH_CLOW	            = 14, // Ŭ�ο�����			: Ŭ�ο� ����ÿ��� ����� �����ϴ�.

		    MAX_USE_LIMIT			= 15
        };

		extern const CTypeName UseLimits[MAX_USE_LIMIT];
	};

	// ��ų ���� ����
	namespace StatusLimit	
	{	
        enum Type
        {
		    NONE	            = 0,	// ���� ����
		    STR		            = 1,	// STR�� ���� �̻� �ʿ�
		    DEX		            = 2,	// DEX�� ���� �̻� �ʿ�
		    CON		            = 3,	// CON�� ���� �̻� �ʿ�
		    INT		            = 4,	// INT�� ���� �̻� �ʿ�
		    WIS		            = 5, 	// WIS�� ���� �̻� �ʿ�
			LEVEL				= 6,	// LEVEL�� ���� �̻� �ʿ�	//--//

            MAX_STATUS_LIMIT    = 7		//--//
        };
		
		extern const CTypeName StatusLimits[MAX_STATUS_LIMIT];
	};

	namespace Target
	{ 
        enum Type
        {
		    NONE				= 0,
		    MELEE				= 1,
		    FRIEND				= 2,
		    ENEMY				= 3,
		    DEAD_FRIEND			= 4,
		    DEAD_ENEMY			= 5,
		    FRIEND_OBJECT		= 6,
		    ENEMY_OBJECT		= 7,
		    PARTY				= 8,
		    FRIEND_EXCEPT_SELF	= 9,
		    LINE_ENEMY			= 10,
			SUMMON				= 11,
		    MAX_TARGET_TYPE		= 12
        };

		extern const CTypeName TargetTypes[MAX_TARGET_TYPE];
	};


    struct ProtoType
	{
		enum 
		{ 
			MAX_LIMIT_NUM		= 2, 
			MAX_FILE_NAME		= 32, 
			MAX_SKILL_NAME		= 32, 
			MAX_SKILL_DESCRIBE	= 256 
		};


		char			    m_szEffectFileName[MAX_FILE_NAME];		// ����Ʈ ���ϸ�
		char			    m_szHitFileName[MAX_FILE_NAME];			// ��Ʈ ���ϸ�
		char			    m_szCastingFileName[MAX_FILE_NAME];		// ĳ���� ���ϸ�
        Item::SpriteData    m_SpriteInfo;							// ��������Ʈ ����
		char				m_szSkillDescribe[MAX_SKILL_DESCRIBE];	// ��ų ����
		
		Type::SkillType     m_eSkillType;		// See Namespace Type
        Target::Type	    m_eTargetType;		// See Namespace Target
        UseLimit::Type      m_eUseLimit;		// See Namespace UseLimit

        unsigned char	    m_StatusLimitType[MAX_LIMIT_NUM];		// ���� ���� ���� ����
		char			    m_StatusLimitValue[MAX_LIMIT_NUM];		// ���� ���� ���� ��

		unsigned short	    m_usSkill_ID;		// ��ų ���̵�

		unsigned short	    m_StartMP;			// ���� �Ҹ� MP
        unsigned short	    m_LevelMP;			// ������ �Ҹ� MP
		unsigned short		m_LockMP;			// ��ī��Ʈ�� �Ҹ� MP

		unsigned short	    m_StartTick;		// ���� ƽī��Ʈ
        unsigned short	    m_LevelTick;		// ������ ƽī��Ʈ

		unsigned short	    m_usParentSkill;	// �θ� ��ų ���̵�
		unsigned short		m_usChildSkill;		// �ڽ� ��ų ���̵�
		
		unsigned long		m_dwCoolDownTime;	// ��ٿ� �ð�
		
		float				m_fMinRange;		// �ּ� ���� �Ÿ�
		float			    m_fMaxRange;		// �ִ� ���� �Ÿ�
		float			    m_fEffectExtent;	// ȿ�� ����

		bool			    m_bProtection;		// ��ȣ ��ų�� ���� �����°�?
		bool			    m_bGauge;			// ���� ����� ������ ��ų�ΰ�? (ex. �н�Ʈ����)
        bool			    m_bIsClassSkill;	// Ŭ���� ��ų�ΰ�?
        bool			    m_bInterrupt;		// ������ ���� ��ų�ΰ�?
		bool				m_bCounter;			// ī���� ���ϴ� ��ų�ΰ�?

		unsigned char	    m_cEndScript;		// EndScript�� ����ϴ°�?
        unsigned char	    m_cEndCoolDown;		// ��ų ȿ���� �������� ��ٿ��� ���ư���. (ex. ���ڽ�)
        unsigned char	    m_cStrikeNum;		// �ٴ� ��Ʈ Ƚ��

		unsigned char	    m_cPadding[1];

		ProtoType();
        void Initialize();
	};
}

#pragma pack()

#endif