#ifndef _MONSTER_STRUCTURE_H_
#define _MONSTER_STRUCTURE_H_

#include <Creature/CreatureStructure.h>

#pragma pack(8)

struct MonsterInfo
{
	enum MaxNumber
	{
		MAX_MOTION_NUM				= 4,		// �ִ� ��� ����
		MAX_ORIGINAL_ITEM_NUM		= 5,		// ���� ���� ���� �ִ��

		MAX_NAME_LEN				= 64,		// ���� �̸� ����
		MAX_MODELING_FLAG_LENGTH	= 32		// �𵨸� ������ �̸� ����
	};

	enum MonsterPattern
	{
		PATTERN_COMMON		= 0,	
		PATTERN_WARRIOR		= 1,				// ������
		PATTERN_DEFENDER	= 2,				// �����
		PATTERN_MAGE		= 3,				// ������
		PATTERN_ACOLYTE		= 4,				// ���ݶ���Ʈ
        PATTERN_BOSS		= 5,				// ����
		PATTERN_BG			= 6,				// background?
		PATTERN_SUMMON		= 7,				// ��ȯ��
		PATTERN_STRUCTURE	= 8,				// structure (stone type)
		PATTERN_NAMED		= 9,				// ���ӵ�
		PATTERN_CHIEF		= 10,				// ġ��
		PATTERN_OBJECT		= 11,				// flowers, mushrooms, etc..
		PATTERN_GUARD		= 12,				// ���
		PATTERN_GATHER		= 13,				// Collecting type
		MAX_PATTERN			= 14
	};

	enum SkillPattern
	{
		ATTACK_SKILL		= 0,				// ���ݿ� ��ų
		RECOVER_SKILL		= 1,				// ȸ���� ��ų
		ENCHANT_SKILL		= 2,				// �ο��� ��ų
		EXTENT_SKILL		= 3,				// ������ ��ų
		BOSS_CHANT_SKILL	= 4,				// ������ æƮ ��ų

		MAX_SKILL_PATTERN	= 5	
	};

	enum HitBox
	{
		X_PLUS				= 0, 
		X_MINUS				= 1, 
		Y_PLUS				= 2, 
		Y_MINUS				= 3,

		MAX_HITBOX_NUM		= 4
	};

	enum Z3D_CHR_ACTION
	{
		Z3D_CA_RUN			= 1,				// �޸���
		Z3D_CA_ATTACK		= 2,				// ����
		Z3D_CA_WALK			= 8,				// �ȱ�
		Z3D_CA_CASTING		= 24				// ĳ����
	};

	enum AwardKind 
	{ 
		ORIGINAL1			= 0,				// ����1
		ORIGINAL2			= 1,				// ����2
		ORIGINAL3			= 2,				// ����3
		ORIGINAL4			= 3,				// ����4
		ORIGINAL5			= 4,				// ����5

		COIN				= 5,				// ��
		GEM					= 6,				// ����
		METAL				= 7,				// ����
		POTION				= 8,				// ����
		SKILL				= 9,				// ��ų��

		CASH_ITEM			= 10,				// ĳ����
		LOTTERY				= 11,				// ����

		F_STANDARD_EQUIP	= 12,				// Standard-Option Item (Grade F)
		D_STANDARD_EQUIP	= 13,				// Standard-Option Item (Grade D)
		C_STANDARD_EQUIP	= 14,				// Standard-Option Item (Grade C)
		B_STANDARD_EQUIP	= 15,				// Standard-Option Item (Grade B)
		A_STANDARD_EQUIP	= 16,				// Standard-Option Item (Grade A)

		F_OVER_EQUIP		= 17,				// Over-Option Item (Grade F)
		D_OVER_EQUIP		= 18,				// Over-Option Item (Grade D)
		C_OVER_EQUIP		= 19,				// Over-Option Item (Grade C)
		B_OVER_EQUIP		= 20,				// Over-Option Item (Grade B)
		A_OVER_EQUIP		= 21,				// Over-Option Item (Grade A)

		NO_EQUIP			= 22,				// No-Option Item

		//--//	start..
		F_RING				= 23,				// ���� (Grade F)
		D_RING				= 24,				// ���� (Grade D)
		C_RING				= 25,				// ���� (Grade C)
		B_RING				= 26,				// ���� (Grade B)
		A_RING				= 27,				// ���� (Grade A)

		F_NECKLACE			= 28,				// ����� (Grade F)
		D_NECKLACE			= 29,				// ����� (Grade D)
		C_NECKLACE			= 30,				// ����� (Grade C)
		B_NECKLACE			= 31,				// ����� (Grade B)
		A_NECKLACE			= 32,				// ����� (Grade A)
		
		F_RUNE				= 33,				// �� (Grade F)
		D_RUNE				= 34,				// �� (Grade D)
		C_RUNE				= 35,				// �� (Grade C)
		B_RUNE				= 36,				// �� (Grade B)
		A_RUNE				= 37,				// �� (Grade A)
		DESTRUCTION_RUNE	= 38,				// �Ҹ��� ��

		MAX_AWARD_KIND		= 39
		//--//	end..
	};

	enum SpeacialKindID
	{
		// Pets
		SUMMON_KINDLING			= 3500, 
		SUMMON_FLAMEWALKER		= 3506,
		SUMMON_BURNINGSHIELD	= 3513,
		SUMMON_FIREWING			= 3520,
		SUMMON_DEATHBURN		= 3527,

		// FIRST LEVEL STONE
		// Statue of honor
		STATUE_NEUTRALITY1		= 4001,
		STATUE_HUMAN_LOADING1	= 4002,
		STATUE_HUMAN_COMPLETE1	= 4003,
		STATUE_AKHAN_LOADING1	= 4004,
		STATUE_AKHAN_COMPLETE1	= 4005,

		// BattleGround Server, Statue of honor
		BG_STATUE_NEUTRALITY1			= 4006,
		BG_STATUE_HUMAN_LOADING1		= 4007,
		BG_STATUE_HUMAN_COMPLETE1		= 4008,
		BG_STATUE_AKHAN_LOADING1		= 4009,
		BG_STATUE_AKHAN_COMPLETE1		= 4010,

		// dark caernarvon Statue
		POWER_STATUE_NEUTRALITY1		= 4011,		// power
		POWER_STATUE_HUMAN_LOADING1		= 4012,
		POWER_STATUE_HUMAN_COMPLETE1	= 4013,
		POWER_STATUE_AKHAN_LOADING1		= 4014,
		POWER_STATUE_AKHAN_COMPLETE1	= 4015,

		INT_STATUE_NEUTRALITY1			= 4016,		// wisdom
		INT_STATUE_HUMAN_LOADING1		= 4017,
		INT_STATUE_HUMAN_COMPLETE1		= 4018,
		INT_STATUE_AKHAN_LOADING1		= 4019,
		INT_STATUE_AKHAN_COMPLETE1		= 4020,

		EXP_STATUE_NEUTRALITY1			= 4021,		// experience
		EXP_STATUE_HUMAN_LOADING1		= 4022,
		EXP_STATUE_HUMAN_COMPLETE1		= 4023,
		EXP_STATUE_AKHAN_LOADING1		= 4024,
		EXP_STATUE_AKHAN_COMPLETE1		= 4025,

		WEALTH_STATUE_NEUTRALITY1		= 4026,		// wealth
		WEALTH_STATUE_HUMAN_LOADING1	= 4027,
		WEALTH_STATUE_HUMAN_COMPLETE1	= 4028,
		WEALTH_STATUE_AKHAN_LOADING1	= 4029,
		WEALTH_STATUE_AKHAN_COMPLETE1	= 4030,

		LIFE_EXTRACT_NEUTRALITY1		= 4031,		// life
		LIFE_EXTRACT_HUMAN_LOADING1		= 4032,
		LIFE_EXTRACT_HUMAN_COMPLETE1	= 4033,
		LIFE_EXTRACT_AKHAN_LOADING1		= 4034,
		LIFE_EXTRACT_AKHAN_COMPLETE1	= 4035,


		// SECOND LEVEL STONE
		// ���� ����
		STATUE_NEUTRALITY2		= 4036,
		STATUE_HUMAN_LOADING2	= 4037,
		STATUE_HUMAN_COMPLETE2	= 4038,
		STATUE_AKHAN_LOADING2	= 4039,
		STATUE_AKHAN_COMPLETE2	= 4040,

		// BattleGround Server ���� ����
		BG_STATUE_NEUTRALITY2			= 4041,
		BG_STATUE_HUMAN_LOADING2		= 4042,
		BG_STATUE_HUMAN_COMPLETE2		= 4043,
		BG_STATUE_AKHAN_LOADING2		= 4044,
		BG_STATUE_AKHAN_COMPLETE2		= 4045,

		// ��ũ ī���� ����
		POWER_STATUE_NEUTRALITY2		= 4046,		// ���� ����
		POWER_STATUE_HUMAN_LOADING2		= 4047,
		POWER_STATUE_HUMAN_COMPLETE2	= 4048,
		POWER_STATUE_AKHAN_LOADING2		= 4049,
		POWER_STATUE_AKHAN_COMPLETE2	= 4050,

		INT_STATUE_NEUTRALITY2			= 4051,		// ������ ����
		INT_STATUE_HUMAN_LOADING2		= 4052,
		INT_STATUE_HUMAN_COMPLETE2		= 4053,
		INT_STATUE_AKHAN_LOADING2		= 4054,
		INT_STATUE_AKHAN_COMPLETE2		= 4055,

		EXP_STATUE_NEUTRALITY2			= 4056,		// ������ ����
		EXP_STATUE_HUMAN_LOADING2		= 4057,
		EXP_STATUE_HUMAN_COMPLETE2		= 4058,
		EXP_STATUE_AKHAN_LOADING2		= 4059,
		EXP_STATUE_AKHAN_COMPLETE2		= 4060,

		WEALTH_STATUE_NEUTRALITY2		= 4061,		// ���� ����
		WEALTH_STATUE_HUMAN_LOADING2	= 4062,
		WEALTH_STATUE_HUMAN_COMPLETE2	= 4063,
		WEALTH_STATUE_AKHAN_LOADING2	= 4064,
		WEALTH_STATUE_AKHAN_COMPLETE2	= 4065,

		LIFE_EXTRACT_NEUTRALITY2		= 4066,		// ���� �����
		LIFE_EXTRACT_HUMAN_LOADING2		= 4067,
		LIFE_EXTRACT_HUMAN_COMPLETE2	= 4068,
		LIFE_EXTRACT_AKHAN_LOADING2		= 4069,
		LIFE_EXTRACT_AKHAN_COMPLETE2	= 4070,

		// ���� KID (CreatureStructure.h ���� ���ǵǾ� ����)
		MIN_STATUE_KID			= 4001,
		MAX_STATUE_KID			= 4070
	};

	enum StatueEffectType
	{
		SE_NONE				= 0,
		SE_HP				= 1,		// Max HP effect
		SE_MP				= 2,		// Max MP effect
		SE_EXP				= 3,		// obtain exp
		SE_DROPRATE			= 4,		// obtain DropRate

		MAX_EFFECT_TYPE_NUM	= 5
	};


	MotionInfo	m_MonsterMotions[MAX_MOTION_NUM];				// Behavioral information of monsters (walk, run, attack, cast)
	char		m_strName[MAX_NAME_LEN];						// name
	char		m_strModelingFlag[MAX_MODELING_FLAG_LENGTH];	// model
	float		m_fHitBox[MAX_HITBOX_NUM];						// hitbox

	unsigned short	m_aryAwardItem[MAX_ORIGINAL_ITEM_NUM];		// 
	unsigned long	m_aryDropRate[MAX_AWARD_KIND];				// ������ ������ �����
	unsigned char	m_cStandardBaseNum;							// Standard-Option ����� �ּ� + ����
	unsigned char	m_cOverBaseNum;								// Over-Option ����� �ּ� + ����

	unsigned long	m_dwKID;									// ���� ID
	unsigned long	m_dwRespawnTime;							// ������ Ÿ��

	float	m_fSize;											// ������
	float	m_fAttackAngle;										// ���� ����
	short	m_wAttackRange;										// ���� �Ÿ�

	unsigned char	m_cSkillPattern;							// ��ų ����
    unsigned char	m_cSkillLevel;								// ��ų ����

	unsigned char	m_cFixLevelGap;								// ���� ������
	bool			m_bFixLevelGap;								// ���� ������ ���� ����
		
	char	m_cNation;											// ����

	unsigned short	m_wSkillUseRate;							// ��ų ��� Ȯ��
	unsigned short	m_wSkillID[MAX_SKILL_PATTERN];				// ���� ��ų ID
	unsigned char	m_cEnchantSpellType;						// �ο� ��ų�� ���� ����Ÿ�� ( Skill::SpellID::Type )
	unsigned char	m_cChantSpellType;							// ��Ƽ æƮ ��ų�� ���� ����Ÿ�� ( Skill::SpellID::Type )
	float			m_fSkillEffectSize;							// ��ų ����Ʈ ������ (�ΰ�: 1.0)
	
	bool	m_bIgnoreEnchant;									// æƮ, ��æƮ ���� ����
	bool	m_bCollision;										// �浹 üũ ����
	bool	m_bFirstAttack;										// �������� ����
	bool	m_bReturnPosition;									// Ÿ���� �������� ���ڸ��� ���ư����� ����
	bool	m_bUseLOD;											// LOD ó�� ����

	// ���� ������Ʈ ����
	unsigned long	m_dwDevelopGold;							// ���� ���
	unsigned char	m_cDevelopSpeed;							// ���� �ӵ� (���� : ��)
	unsigned long	m_dwUpgradeGold;							// ���׷��̵� ���
	unsigned char	m_cUpgradeSpeed;							// ���׷��̵� �ӵ� (���� : ��)
	
	float	m_fDevelopGoldDown;									// ���� ��� ���� ��ġ (%)
	float	m_fDevelopSpeedUp;									// ���� �ӵ� ��� ��ġ (%)
	float	m_fUpgradeGoldDown;									// ���׷��̵� ��� ���� ��ġ (%)
	float	m_fUpgradeSpeedUp;									// ���׷��̵� �ӵ� ��� ��ġ (%)
	float	m_fDefenseUp;										// ���� ��� ��ġ (%)
	float	m_fOffenseUp;										// ���ݷ� ��� ��ġ (%)
	float	m_fHPUp;											// HP ��� ��ġ (%)
	float	m_fBonusRate;										// �� ���� ���ʽ� (%)
	float	m_fRespawnSpeedUp;									// PC ������ �ӵ� ��� ��ġ (%)

	// ������ ���� ȿ�� ����
	unsigned char	m_cStatueEffectType;						// ���� ȿ�� Ÿ��
	unsigned char	m_cStatueEffectPercent;						// ���� ȿ�� �ۼ�Ʈ
	bool			m_bStatueRespawnEnable;						// ���� ������ ���� ����
		
	MonsterInfo();
    static MonsterPattern GetMonsterPattern(const char* szMonsterType);
	static StatueEffectType	GetStatueEffectType(const char* szStatueEffectType);

	static unsigned short	GetCompleteStatueKID(unsigned short wNowKID);
	static unsigned short	GetLoadingStatueKID(unsigned char cRace, unsigned short wNowKID);
	static unsigned short	GetDefaultStatueKID(unsigned short wNowKID);
	static bool				IsLoadingStatueKID(unsigned short wNowKID);
	static bool				IsRaceCompleteStatueKID(unsigned short wNowKID);
};


#pragma pack()

#endif
