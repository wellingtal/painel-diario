#ifndef _CREATURE_STRUCTURE_H_
#define _CREATURE_STRUCTURE_H_

#include <winsock2.h>
#include <windows.h>
#include <cmath>

#include <DB/DBDefine.h>

// ���� ����
struct CreatureStatus;
struct CharacterStatus;
struct SKILL;

namespace Item
{
	class CItem;
	class CEquipment;

	namespace EquipType
	{
		enum DoubleSwordType;
	}
}

namespace BattleInclination
{
	enum Const
	{
		IS_PEACEMODE        = ( 1 << 0 ),   // ��ȭ����� ��� ����
		IS_SAFETY_ZONE      = ( 1 << 1 ),   // ������Ƽ �� ���ο� �ִ� ��� ����
		HAS_CAMP            = ( 1 << 2 ),   // ��� ����� ���� ��忡 ���� ���� ��� ����
		HAS_DEVELOPING_CAMP	= ( 1 << 3 ),   // �Ǽ����� ��� ����� ���� ��忡 ���� ���� ��� ����
		IS_CASTLE_OBJECT    = ( 1 << 4 ),   // ���� Object�̸鼭 ���������� ��츸 ����
		IS_CASTLE_NPC		= ( 1 << 5 ),	// ���� Object�̸鼭 ���� ������ ���� ���� NPC �� �޹� NPC
		IS_GOD_MODE			= ( 1 << 6 ),	// G����� ��� ���� 
		IS_TEAM_BATTLE		= ( 1 << 7 ),	// ����Ʋ ���� ��� ����
	};

	enum VSType
	{
		VS_CHARACTER		= ( 1 << 0 ),	// PvP
		VS_PARTY			= ( 1 << 1 ),	// ��Ƽ�� ��Ʋ
		VS_GUILD			= ( 1 << 2 ),	// ��尣 ��Ʋ
	};

	// �Ǿƽĺ��� ��ü�� �Ǵ� ĳ���� ����
	// ���⼭ ������ �߰��ϰų� �����ϴ� ���, 
	// ��ε�ĳ���� ��, ä�ü����� ������ �����ϴ� �κе� �����ؾ��Ѵ�.
	struct CharData
	{
		bool IsPeaceMode(void) const		{ return 0 != (m_cFlags & IS_PEACEMODE);   		}	// ��ȭ����� ���
		bool IsSafetyZone(void) const		{ return 0 != (m_cFlags & IS_SAFETY_ZONE); 		}	// ���� Safety�� ���ο� �ִ���
		bool HasCamp(void) const			{ return 0 != (m_cFlags & HAS_CAMP);       		}	// ���� ��尡 ��� ����� ������ �ִ���...
		bool HasDevelopingCamp(void) const	{ return 0 != (m_cFlags & HAS_DEVELOPING_CAMP); }	// ���� ��尡 ��� ����� �Ǽ�������...
		bool IsCastleObject(void) const		{ return 0 != (m_cFlags & IS_CASTLE_OBJECT);	}	// ���������� ���, ������������...
		bool IsCastleNPC(void) const		{ return 0 != (m_cFlags & IS_CASTLE_NPC);		}	// ���� NPC �� ���
		bool IsGODMode(void) const			{ return 0 != (m_cFlags & IS_GOD_MODE);			}	// G����� ���
		bool IsTeamBattle(void) const		{ return 0 != (m_cFlags & IS_TEAM_BATTLE);		}	// ����Ʋ ���� ���

		void SetPeaceMode(bool bPeaceMode)					{ m_cFlags |= bPeaceMode ? IS_PEACEMODE : 0;				}
		void SetSafetyZone(bool bInSafetyZone)				{ m_cFlags |= bInSafetyZone ? IS_SAFETY_ZONE : 0;			}
		void SetHasCamp(bool bHasCamp)						{ m_cFlags |= bHasCamp ? HAS_CAMP : 0;						}
		void SetHasDevelopingCamp(bool bHasDevelopingCamp)	{ m_cFlags |= bHasDevelopingCamp ? HAS_DEVELOPING_CAMP : 0;	}
		void SetCastleObject(bool bCastleObject)			{ m_cFlags |= bCastleObject ? IS_CASTLE_OBJECT : 0; 		}
		void SetCastleNPC(bool bCastleNPC)					{ m_cFlags |= bCastleNPC ? IS_CASTLE_NPC : 0;				}
		void SetGODMode(bool bGODMode)						{ m_cFlags |= bGODMode ? IS_GOD_MODE : 0;					}
		void SetTeamBattle(bool bTeamBattle)				{ m_cFlags |= bTeamBattle ? IS_TEAM_BATTLE : 0;				}

		unsigned long 	m_dwCID;          	// ĳ���� CID
		unsigned long 	m_dwGID;          	// ĳ���� GID        
		unsigned long 	m_dwCastleID;     	// ���� ������ ���, ���� ���� ID�� �ִ´�.

		unsigned char 	m_cSiegeState;    	// ���� ������ ���, ���� ���� ���¸� �ִ´�.
		unsigned char 	m_cNation;        	// ĳ���� ����
		unsigned char 	m_cAdminLevel;    	// ��� ����
		unsigned char 	m_cFlags;         	// ��Ÿ ����

		unsigned long	m_dwPID;			// ĳ���Ϳ� ��Ƽ ����
		unsigned char	m_cTacticsFlag;		// ĳ���Ϳ� �뺴 ����

		unsigned char	m_cRealmWarFlag;
		unsigned char	m_cGuildWarFlag;

		unsigned char	m_cCreatureType;	// ũ���� Ÿ��		
		unsigned short	m_wObjectType;		// ��Ʈ���� ����
		unsigned short	m_wKind;			// ��Ʈ���� ����

		unsigned char	m_cRide;			// ���⿡ ž�� ����.
	};    

	// �Ǿƽĺ��� ���̴� ĳ���Ͱ��� ���� (�ϴ��� ä�� �������� �� �ʿ� ���� ������ ����)
	struct RelationData
	{
		unsigned char	m_cVSFlag;			// PvP, ����Ʋ ����
		bool			m_bHostileGuild;	// ���� ����ΰ�?
	};
};

namespace Creature
{
	enum Const
	{
		PC_MAX_LEVEL		= 100,		// ĳ���� �ִ� ����
		MONSTER_MAX_LEVEL	= 150,		// ���� �ִ� ����

		// edith 2008.02.27 ��� ���� ��������
		LEVEL_ABLE_WAR		= 1		// ������ ������ �ּ� ���� (��� ����/���� ��� ����)
	};

	enum IdentifyCIDBit 
	{
		MONSTER_KIND_BIT	= 0x0000FFFF,

		MONSTER_BIT			= 0x80000000,
		NPC_BIT				= 0x40000000,
		SIEGE_OBJECT_BIT	= 0x10000000,
		SUMMON_MONSTER_BIT	= 0xA0000000,	// MONSTER_BIT (0x80000000) + 0x20000000
		NPC_LINK_BIT		= 0x50000000,	// ��ȯ �Ϸ�� ������ ��� �� ��Ʈ�� ��Ʈ�� NPC�� ��ũ�Ǿ� �޴޻����� ������ �Ѵ�.
											// NPC_BIT (0x40000000) + 0x10000000

		// ���� KID (MonsterStructure.h ���� ���ǵǾ� ����)
		MIN_STATUE_KID		= 4001,
		MAX_STATUE_KID		= 4070
	};

	enum CreatureType
	{
		CT_PC				= 0,
		CT_NPC				= 1,
		CT_MONSTER			= 2,
		CT_SUMMON			= 3,
		CT_STRUCT			= 4,
		CT_SIEGE_OBJECT		= 5,
		CT_NONE_TYPE		= 99
	};

	enum Nation		
	{ 
		STATELESS		= 0,	// ������ (����, NPC)
		KARTERANT		= 1,	// ī���׶�Ʈ
		MERKADIA		= 2,	// �޸�ī���
		ALMIGHTY_PIRATE	= 3,	// ����������

		MAX_NATION		= 4
	};			  

	enum WarFlag
	{
		WAR_OFF			= 0,	// ���� ���� ����
		WAR_ON			= 1,	// ���� ����
		WAR_INSTANCE	= 2		// �ν��Ͻ� ���� ����
	};

	enum StatusType
	{
		RIGHT_PASSIVE_TYPE		= 0, 	// �нú� ��ų�� ����� ������ ��� ���� (�⺻������ CreatureStatus�� �ٷ� ���ȴ�.)
		LEFT_PASSIVE_TYPE		= 1,	// �нú� ��ų�� ����� �޼� ��� ���� (�̵���)
		RIGHT_NON_PASSIVE_TYPE	= 2,	// �нú� ��ų�� ������� ���� ������ ��� ���� (������ ���⸦ ����ϴ� ��ų)
		LEFT_NON_PASSIVE_TYPE	= 3,	// �нú� ��ų�� ������� ���� �޼� ��� ���� (�޼� ����(��ų��)�� ����ϴ� ��ų)

		MAX_STATUS_TYPE			= 4
	};

	// �������� ����ϴ� Creature �Ǻ� �Լ���
	// Ŭ���̾�Ʈ������ CRYLGameData::GetClientIdentity() �Լ� ���!!
	static bool	IsSummonMonster(unsigned long dwCID)
	{
		return (Creature::SUMMON_MONSTER_BIT == (dwCID & Creature::SUMMON_MONSTER_BIT));
	}

	static bool	IsStruct(unsigned long dwCID)
	{
		unsigned short wKID = static_cast<unsigned short>(dwCID & Creature::MONSTER_KIND_BIT);
		return (wKID >= MIN_STATUE_KID && wKID <= MAX_STATUE_KID);
	}

	static bool	IsLinkedNPC(unsigned long dwCID)
	{
		return (Creature::NPC_LINK_BIT == (dwCID & Creature::NPC_LINK_BIT));
	}

	static CreatureType GetCreatureType(unsigned long dwCID)
	{
		if (dwCID & Creature::MONSTER_BIT)
		{
			if (IsSummonMonster(dwCID)) return Creature::CT_SUMMON;
			else if (IsStruct(dwCID)) return Creature::CT_STRUCT;

			return Creature::CT_MONSTER;
		}
		else if (dwCID & Creature::NPC_BIT)	return Creature::CT_NPC;
		else if (dwCID & Creature::SIEGE_OBJECT_BIT) return Creature::CT_SIEGE_OBJECT;

		return Creature::CT_PC;
	}

    inline static const char* GetNationName(unsigned char cNation)
    {
        switch(cNation)
        {
        case KARTERANT:         return "Hignis";//was KARTERANT
        case MERKADIA:          return "Aglacies";//was MERKADIA
        case ALMIGHTY_PIRATE:   return "ALMIGHTY_PIRATE";
        }

        return "STATELESS";
    }

    inline static const char* GetShortNationName(unsigned char cNation)
    {
        switch(cNation)
        {
        case KARTERANT:         return "Hignis";
        case MERKADIA:          return "Aglacies";
        case ALMIGHTY_PIRATE:   return "AP";
        }

        return "ST";
    }
};


// ����Ʈ ���ʽ� ����
namespace EliteBonus
{
	enum Const
	{ 
		MAX_BONUS_LEVEL		= 10,
		MAX_BONUS_DROP_ITEM	= 3
	};

	// ����Ʈ ���ʽ��� ���� ����ġ/����� ���ʽ��� ���ŵǾ����ϴ�. (2004-09-17 by �ε�)
	static short usBonusExp[MAX_BONUS_LEVEL] = { 
		100 + 0, 
		100 + 0, 
		100 + 0, 
		100 + 0, 
		100 + 0,
		100 + 0,
		100 + 0, 
		100 + 0,
		100 + 0,
		100 + 0
	};

	static unsigned char usBonusItemDropRate[MAX_BONUS_LEVEL][MAX_BONUS_DROP_ITEM] = { 
		{ 0, 0, 0 }, 
		{ 0, 0, 0 }, 
		{ 0, 0, 0 }, 
		{ 0, 0, 0 }, 
		{ 0, 0, 0 }, 
		{ 0, 0, 0 }, 
		{ 0, 0, 0 }, 
		{ 0, 0, 0 }, 
		{ 0, 0, 0 }, 
		{ 0, 0, 0 }
	};

	struct EliteBonusData
	{
		char m_cNation;
		unsigned char m_cLevel;

		EliteBonusData();
		EliteBonusData(char cNation, unsigned char cLevel);
	};
}


// �� �̵� ����
struct Position
{
	float m_fPointX;				// Point X ��ǥ
	float m_fPointY;				// Point Y ��ǥ
	float m_fPointZ;				// Point Z ��ǥ

	inline Position();
	inline Position(const POS& Pos);
	inline Position(const Position& Pos);
	inline Position(float fPointX, float fPointY, float fPointZ);	

    inline POS PositionToPOS() const;

	inline unsigned long GetDistance(const Position& rhs) const
	{ 
		return static_cast<unsigned long>(sqrt(
			(m_fPointX - rhs.m_fPointX) * (m_fPointX - rhs.m_fPointX) + 
			(m_fPointY - rhs.m_fPointY) * (m_fPointY - rhs.m_fPointY) +
			(m_fPointZ - rhs.m_fPointZ) * (m_fPointZ - rhs.m_fPointZ)));
	}

    inline unsigned long GetSquaredDistance(const Position& rhs) const
    { 
        return static_cast<unsigned long>(
            (m_fPointX - rhs.m_fPointX) * (m_fPointX - rhs.m_fPointX) + 
            (m_fPointY - rhs.m_fPointY) * (m_fPointY - rhs.m_fPointY) +
            (m_fPointZ - rhs.m_fPointZ) * (m_fPointZ - rhs.m_fPointZ));
    }

	inline bool operator == (const Position& rhs) const
	{
		if ( m_fPointX == rhs.m_fPointX && m_fPointY == rhs.m_fPointY && m_fPointZ == rhs.m_fPointZ )
		{
			return true;
		}

		return false;
	}

	inline bool operator != (const Position& rhs) const
	{
		if ( m_fPointX != rhs.m_fPointX || m_fPointY != rhs.m_fPointY || m_fPointZ != rhs.m_fPointZ )
		{
			return true;
		}

		return false;
	}

	inline void operator () (const Position& rhs)
	{
		m_fPointX	= rhs.m_fPointX;
		m_fPointY	= rhs.m_fPointY;
		m_fPointZ	= rhs.m_fPointZ;
	}
};

inline Position::Position()
:	m_fPointX(0.0f), m_fPointY(0.0f), m_fPointZ(0.0f) { }

inline Position::Position(const POS& Pos)
:	m_fPointX(Pos.fPointX), m_fPointY(Pos.fPointY), m_fPointZ(Pos.fPointZ) { }

inline Position::Position(const Position& Pos)
:	m_fPointX(Pos.m_fPointX), m_fPointY(Pos.m_fPointY), m_fPointZ(Pos.m_fPointZ) { }

inline Position::Position(float fPointX, float fPointY, float fPointZ)
:	m_fPointX(fPointX), m_fPointY(fPointY), m_fPointZ(fPointZ) { }

inline POS Position::PositionToPOS() const
{
    POS stPos;
    stPos.fPointX = m_fPointX;
    stPos.fPointY = m_fPointY;
    stPos.fPointZ = m_fPointZ;

    return stPos;
}


// ���� ����
struct MotionInfo
{
	float			m_fDirection;	// �ٶ󺸴� ����
	float			m_fVelocity;	// �ӵ�
	unsigned short	m_wAction;		// ���ϴ� �ൿ
	unsigned long	m_dwFrame;		// ��� ������ ( ����, NPC�� ��� )

	MotionInfo();
};


// ��æƮ ����
struct EnchantInfo
{
	enum 
	{
		ULONG_BIT		= 32,	// unsigned long (32����Ʈ)
		MAX_ARRAY		= 4		// ������ ������ UCHAR_BIT * MAX_ARRAY
	};

	unsigned long	m_dwStatusFlag[MAX_ARRAY];	// æƮ/��æƮ �÷���

	EnchantInfo();
    EnchantInfo( const EnchantInfo& Info )      // ������ ���� ���� ������
    {
        for( int i = 0 ; i < MAX_ARRAY ; ++i )
        {
            m_dwStatusFlag[ i ] = Info.m_dwStatusFlag[ i ];
        }
    }

	void SetFlag(unsigned char cSpellKind);
	void ResetFlag(unsigned char cSpellKind);
	void ClearFlag(void);

	bool GetFlag(unsigned char cSpellKind) const;
};


// ������ ������ �߰� ȿ�� ����
struct CalculateDamageInfo
{
	bool	m_bForceDRC;			// '��'�̸� �⺻ DRC�� �����ϰ� �߰� DRC�� ��ü

	float	m_fDRC;					// �߰� DRC
    short	m_nOffenceRevision;		// �߰� ���ݺ���
	short	m_nMinDamage;			// �߰� �ּҵ�����
	short	m_nMaxDamage;			// �߰� �ִ뵥����

	CalculateDamageInfo()
	:	m_bForceDRC(false), m_fDRC(0.0f), m_nOffenceRevision(0), m_nMinDamage(0), m_nMaxDamage(0) 
	{ }

	CalculateDamageInfo(bool bForceDRC, float fDRC, short nOffenceRevision, short nMinDamage, short nMaxDamage)
	:	m_bForceDRC(bForceDRC), m_fDRC(fDRC), 
		m_nOffenceRevision(nOffenceRevision), m_nMinDamage(nMinDamage), m_nMaxDamage(nMaxDamage) 
	{ }
};
typedef CalculateDamageInfo* LPCalculateDamageInfo;


// ĳ���Ϳ� ���� �Ӽ�
struct CharacterStatus
{
	short	m_nSTR;		// ĳ���� STR
	short	m_nDEX;		// ĳ���� DEX
	short	m_nCON;		// ĳ���� CON
	short	m_nINT;		// ĳ���� INT
	short	m_nWIS;		// ĳ���� WIS

	CharacterStatus();
	CharacterStatus(CHAR_INFOST& characterDBData);
	void Init(CHAR_INFOST& characterDBData);
};
typedef CharacterStatus*	LPCharacterStatus;

struct FightStatus
{
	// ��� ���� (�ߺ� ��� ����)
	enum CalculateState
	{
		CS_NONE				= 0,				// �ʱ�ȭ
		CS_BASE_INFO		= 1,				// �⺻ �������� ����� �Ϸ��� ����
		CS_EQUIP_INFO		= 2,				// ��� ȿ���� �߰� ����� ����
		CS_ENCHANT_INFO		= 3					// ��æƮ ȿ���� ���� ����
	};

	enum Const
	{
		MAX_INCHANT_HP		= 15000,
		MAX_INCHANT_MP		= 15000
	};

	long			m_lMinDamage;				// �ּҵ�����
	long			m_lMaxDamage;				// �ִ뵥����
	short			m_wArmor;					// ����
	short			m_wHitRate;					// ����
	short			m_wEvade;					// ȸ��

	unsigned short	m_nMaxHP;					// HP �ִ밪
	short			m_wHPRegen;					// HP ȸ��
	unsigned short	m_nMaxMP;					// MP �ִ밪
	short			m_wMPRegen;					// MP ȸ��

	short			m_wCritical;				// ũ��Ƽ��
	short			m_wBlock;					// ���
	short			m_wSpeed;					// �ӵ�
	short			m_wCoolDownRevision;		// ��ٿ��
	short			m_wSkillPoint;				// ��ų����Ʈ

	short			m_wMagicPower;				// ������
	short			m_wMagicResist;				// ��������

	int				m_wLuck;					// ��

	//--//	start..
	short			m_wFrost;					// �ñ�/���� �Ӽ�
	short			m_wFire;					// ��/�� �Ӽ�
	short			m_wElectro;					// ����/��� �Ӽ�
	short			m_wDarkness;				// ����/�� �Ӽ�
	//--//	end..

	// test avalansa
//	short			m_wSTR;				    // ����/�� �Ӽ�

	float			m_fDefenceRate;				// ���%
	float			m_fCriticalMagnification;	// ũ��Ƽ�ù���
	float			m_fCriticalRate;			// ũ��Ƽ��%
	float			m_fBlockRate;				// ����%
	float			m_fSpeedRate;				// �ӵ�%
	float			m_fCoolDownRevisionRate;	// ��ٿ��%
	float			m_fMagicResistRate;			// ��������%

	float			m_fLuckResistRate;			// ��Ű����%

	float			m_fLevelExplosion;			// ���� �ͽ��÷���

	unsigned char	m_cCriticalType;			// ũ��Ƽ�� Ÿ��
	unsigned char	m_cComboCount;				// ���� �޺� Ƚ��

	unsigned char	m_cCalculateState;			// ��� ����


	FightStatus();

	bool CalculateBaseInfo(const CharacterStatus& characterStatus, const unsigned short wLevel, const unsigned char cClass);
		
	bool CalculateEquipInfo(const Item::CEquipment** ppEquip, const bool bFirstWeaponSlot, const bool bRightHand,  const bool bRideFlag,
		const SKILL& skill, const unsigned char cLevel, const unsigned short wRiderAbility, const unsigned short wLuckAbility, const unsigned short wLevelAbility,
		const CharacterStatus& characterStatus, const unsigned char cClass, FightStatus& resultStatus, int& resultLuck) const;

	bool CalculateEnchantInfo(const unsigned short* aryEnchantLevel, FightStatus& resultStatus) const;
		

	void CalculateSubStatus(void);

	static bool CheckEquipable(const Item::CEquipment* lpEquip, const CharacterStatus& characterStatus,
		const unsigned char cClass, const unsigned char cLevel, const unsigned short wLevelAbility);	//--//
};

#pragma pack(8)

//------------------------------------------------------------------------------------
// TODO : memory alignment ������ ��.
//		: 4byte������ alignment �� �ǵ��� �� ��.
//		: �޸� �����ÿ��� ũ�Ⱑ ���� �ʿ� ���� ����� short������ ���� ��.

// ���⳪ ������ ���ؼ� ������ ���� �ɷ�ġ�� ���� (���ӻ󿡼� ��� ����� �̰����� ��)
struct CreatureStatus
{
	__int64			m_nExp;			// ����� ���� �ڽ��� ������, ������ ���� ������ �� ������
	int				m_nLevel;		// ����
	unsigned short	m_nNowHP;		// ���� HP
	unsigned short	m_nNowMP;		// ���� MP (������ ���� ������ �ִ밪���� �ʱ�ȭ)

	FightStatus		m_StatusInfo;	// ��κ��� ������ �̰��� ����

	CreatureStatus();
	CreatureStatus(CHAR_INFOST& characterDBData);
	void Init(CHAR_INFOST& characterDBData);
};
typedef CreatureStatus* LPCreatureStatus;

#pragma pack()

#endif
