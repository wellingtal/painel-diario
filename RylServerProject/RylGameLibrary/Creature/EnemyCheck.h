#ifndef	_ENEMYCHECK_H_
#define	_ENEMYCHECK_H_

#include <Pattern/Singleton.h>

#include "CreatureStructure.h"

// ���Ʊ� �ĺ� ���� (����, Ŭ���̾�Ʈ �������� ���)
namespace EnemyCheck
{
	enum EnemyType
	{
		// ���� Ÿ��
		EC_ENEMY			= 0,	// ����
		EC_NEUTRAL			= 1,	// �߸�
		EC_FRIEND			= 2,	// �Ʊ�

		EC_ERROR			= 3		// ����
	};

	enum EnemyCheckType
	{
		// üũ Ÿ��
		EC_CHECK_ENEMY				= 0,	// ���ĺ�
		EC_CHECK_NEUTRAL_EXCETION	= 1,	// �߸� ���� ó��
		EC_CHECK_FRIEND				= 2,	// �Ʊ� �ĺ�
		EC_CHECK_FRIEND_2ND			= 3,	// 2�� �Ʊ� �ĺ�

		MAX_CHECK_TYPE				= 4		// �ĺ��� üũ ��ƾ ��
	};

	enum EnemyCheckRule
	{
		RULE_HUNTING		= 0,	// ���÷�
		RULE_GUILD_WAR		= 1,	// �������
		RULE_REALM_WAR		= 2,	// ��������
		RULE_STATUE_WAR		= 3,	// ��������
		RULE_SIEGE_WAR		= 4,	// ��������

		MAX_RULE_NUM		= 5
	};

	// -------------------------------------------------------------------------
	// Siege ���� �Լ�

	bool	IsCamp(unsigned short wObjectType);						// ��� ���
	bool	IsCastleArms(unsigned short wObjectType);				// ���� ����
	bool	IsSiegeArms(unsigned short wObjectType);				// ���� ����
	bool	IsStruct(unsigned short wKind);							// ��Ʈ����

	bool	IsGate(unsigned short wObjectType);						// ����
	bool	IsEmblem(unsigned short wObjectType);					// �� ��¡��
	bool	IsCastleNPC(unsigned short wObjectType);				// ���� ���� NPC


	// -------------------------------------------------------------------------
	// �籹 ü�� ���� �߰��� ����

	typedef bool (*FnProcess)(BattleInclination::CharData& ownerInfo, BattleInclination::CharData& targetInfo, 
		BattleInclination::RelationData& relationInfo, unsigned char& cResult);

	typedef bool (*FnCheck)(BattleInclination::CharData& ownerInfo, BattleInclination::CharData& targetInfo,
		EnemyCheck::EnemyCheckRule eRule);

	class CCheckTable : public CSingleton<CCheckTable>
	{
	public:     

		bool Initialize(void);

		void SetEnemyCheckRule(EnemyCheckRule eRule)	{ m_eRule = eRule;	}
		EnemyCheckRule GetEnemyCheckRule(void)			{ return m_eRule;	}

		unsigned long IsEnemyFromStruct(BattleInclination::CharData& ownerInfo, BattleInclination::CharData& targetInfo, 
			BattleInclination::RelationData& relationInfo, unsigned char& cResult);

	private:

		CCheckTable();
		~CCheckTable();

		unsigned long	GetEnemyType(unsigned char cEnemyCheckType) const;	// üũ Ÿ�Կ� �ش��ϴ� ���ĺ� ��� ����
		
		bool				m_bInit;										// �ʱ�ȭ ����

		EnemyCheckRule		m_eRule;										// ���� ���� ���� �Ǿƽĺ� ��
		FnProcess			m_fnIsEnemy[MAX_CHECK_TYPE][MAX_RULE_NUM];		// �� ��Ȳ�� ���� �Ǿƽĺ� �Լ����� ���̺�
		FnCheck				m_fnCheckSkip[MAX_CHECK_TYPE];					// �� ��Ȳ�� ���� �Ǿƽĺ��� ��ŵ�ؾ� �ϴ��� üũ�ϴ� �Լ����� ���̺�
		
		static CCheckTable	ms_this;
	};

	class CFunctions
	{
	private:

		// ������� �ǹ̸� �дٱ⺸��, ���ڰ� ��ġ�� �� �����ϱ� ���� enum���� ����
		enum ResultType
		{
			// EC_CHECK_ENEMY �� �����
			EN_COMMON_PVP				= 1,				// ����� PvP�� ����� �� (���)
			EN_COMMON_STATELESS_MONSTER,					// ������(��3��) ����
			EN_HUNTING_VSPARTY_VSGUILD,						// ����� �������� ����� �׷� ��� (��Ƽ/��� ���)
			EN_GUILDWAR_HOSTILITY_GUILD,					// �ڽŰ� ����� ������� �������̸� ���� ���� �϶�
			EN_REALMWAR_ENEMY_PC_MONSTER_NPC,				// ��� ������ PC, ����, NPC
			EN_STATUEWAR_ENEMY_PC_MONSTER_STRUCT,			// ��� ������ PC, ����, ��Ʈ��ó
			EN_STATUEWAR_STATELESS_STRUCT,					// �� 3���� ��Ʈ��ó
			EN_SIEGE_ENEMY_PC_MONSTER,						// ��� ������ PC, ����
			EN_SIEGE_ENEMY_GATE_EMBLEM_CASTLE_ARMS,			// ��� ������ ����, �� ��¡��, ���� ����
			EN_SIEGE_ENEMY_SIEGE_ARMS,						// ��� ������ ��ȯ�� ���� ����.



			// EC_CHECK_NEUTRAL_EXCETION �� �����
			NE_COMMON_GMODE				= 21,				// ��밡 ������ �ƴϸ� G����� ��� �߸� ó��
			NE_COMMON_ENEMY_NPC,							// ��� ������ NPC
			
			NE_HUNTING_NON_BATTLER,							// ������ �������� ���� ��� �ο�
			NE_HUNTING_OTHER_BATTLER,						// �ڽŰ� �ٸ� ����Ʋ�� �������� ��� �ο�
			NE_HUNTING_BATTLER,								// ����Ʋ�� �������� ��� �ο�
			
			NE_GUILDWAR_IN_SAFETYZONE,						// ���� ������Ƽ ���� ���� ��� ���� ���� ������ ��� �ο�
			NE_GUILDWAR_NON_BATTLER,						// ����� ���������� ���� ��� �ο�
			NE_GUILDWAR_OTHER_BATTLER,						// �ڽ��� ��尡 �ƴ� ������� ������
			NE_GUILDWAR_BATTLER,							// ����� �������� ��� �ο�
			NE_GUILDWAR_MYGUILD,							// �ڽ��� ���� ����
			
			NE_REALMWAR_ENEMY_STRUCT,						// �ڽŰ� �ٸ� ������ ���� ��Ʈ����
			NE_REALMWAR_ENEMY_GATE_EMBLEM_SIEGEOBJECT,		// ��� ������ ���� ��ȯ��, ���� �빮, ��ȯ�� ���� ����

			NE_STATUEWAR_ENEMY_GATE_EMBLEM_SIEGEOBJECT,		// ��� ������ ���� ��ȯ��, ���� �빮, ��ȯ�� ���� ����

			NE_SIEGE_ENEMY_STRUCT,							// �ڽŰ� �ٸ� ������ ���� ��Ʈ����

			// EC_CHECK_FRIEND �� �����
			FR_COMMON_MYPARTY			= 41,				// ��������� ����� ���� ��Ƽ
			FR_COMMON_MYGUILD,								// ��������� ����� ���� ���
			FR_COMMON_MYSELF,								// �ڱ� �ڽ�
			FR_GUILDWAR_MYGUILD,							// ���� ������ ��� �׷�
			FR_REALMWAR_MYNATION_PC_NPC_MONSTER_STRUCT,		// �ڱ� ������ PC, NPC, ����, ����
			FR_REALMWAR_MYNATION_GATE_EMBLEM_SIEGEOBJECT,	// �ڱ� ������ ��ȯ��, ���� �빮, ���� ����.
			FR_STATUEWAR_MYNATION_PC_NPC_MONSTER_STRUCT,	// �ڱ� ������ PC, NPC, ����, ����
			FR_STATUEWAR_MYNATION_GATE_EMBLEM_SIEGEOBJECT,	// �ڱ� ������ ��ȯ��, ���� �빮, ���� ����.
			FR_SIEGEWAR_MYNATION_PC_NPC_MONSTER_STRUCT,		// �ڱ� ������ PC, NPC, ����, ����
			FR_SIEGEWAR_MYNATION_SIEGEOBJECT,				// �ڱ� ������ ��ȯ��, ���� �빮, ��������, ���� ����.
			

			// EC_CHECK_FRIEND_2ND �� �����
			FR2_COMMON_MYNATION_PC_NPC	= 61,				// �ڱ� PC, NPC
			FR2_COMMON_STATELESS_NPC,						// ������ NPC
		};


		// ���� ���� ���� üũ�� ���� ũ���� Ÿ��
		enum AttackableCreatureType
		{
			ACT_PC							= 0,	// PC
			ACT_MONSTER						= 1,	// ����
			ACT_EMBLEM						= 2,	// �� ��¡��
			ACT_GATE						= 3,	// ����
			ACT_CAMP						= 4,	// ��� ���
			ACT_GUARD						= 5,	// ����
			ACT_SHORT_RANGE_CASTLE_ARMS		= 6,	// �ٰŸ� ���� ����
			ACT_LONG_RANGE_CASTLE_ARMS		= 7,	// ���Ÿ� ���� ����
			ACT_SHORT_RANGE_SIEGE_ARMS		= 8,	// �ٰŸ� ���� ����
			ACT_LONG_RANGE_SIEGE_ARMS		= 9,	// ���Ÿ� ���� ����
			ACT_AIRSHIP						= 10,	// ���ۼ�
			ACT_NONE						= 11,	// �޹�, NPC, �������NPC

			MAX_ATTACKABLE_CREATURE			= 12,	// ���� �ð� �� �Ϲ� �ð��� ���ݰ����� ��� ��
		};

		// ���� ���� ���� ���ϰ�
		enum ReturnACT
		{
			RET_NONE		= 0,					// ���� �Ұ���
			RET_ALLWAYS		= 1,					// �׻� ���� ����
			RET_DEVELOPING	= 2,					// ���� ���϶��� ���� ����
		};

		const static unsigned char	ms_AttackableTable[MAX_ATTACKABLE_CREATURE][MAX_ATTACKABLE_CREATURE];	// �� ũ���� Ÿ�Կ� ���� ���� ���� ���� ���̺�


		friend class CCheckTable;

		// �Ǿƽĺ� �Լ� ���� ��ũ��
		#define DECLARE_ENEMY_CKECK(CheckRule) \
			static bool CheckRule(BattleInclination::CharData& ownerInfo, BattleInclination::CharData& targetInfo, \
			BattleInclination::RelationData& relationInfo, unsigned char& cResult)

		#define DEFINE_ENEMY_CKECK(CheckRule) \
			bool EnemyCheck::CFunctions::##CheckRule( \
			BattleInclination::CharData& ownerInfo, BattleInclination::CharData& targetInfo, \
			BattleInclination::RelationData& relationInfo, unsigned char& cResult)

		#define USE_ENEMY_CKECK(CheckRule) CheckRule(ownerInfo, targetInfo, relationInfo, cResult)


		// �Ǿƽĺ� ��ƾ ��ŵ üũ �Լ� ���� ��ũ��
		#define DECLARE_SKIP_CKECK(CheckType) \
			static bool CheckType(BattleInclination::CharData& ownerInfo, BattleInclination::CharData& targetInfo, \
			EnemyCheck::EnemyCheckRule eRule)

		#define DEFINE_SKIP_CHECK(CheckType) \
			bool EnemyCheck::CFunctions::##CheckType( \
			BattleInclination::CharData& ownerInfo, BattleInclination::CharData& targetInfo, \
			EnemyCheck::EnemyCheckRule eRule)

		
		// ���� ���� ���� üũ �Լ� ���� ��ũ��
		#define DECLARE_ATTACKABLE_CHECK(FuncName) \
			static bool FuncName(BattleInclination::CharData& ownerInfo, BattleInclination::CharData& targetInfo)

		#define DEFINE_ATTACKABLE_CHECK(FuncName) \
			bool EnemyCheck::CFunctions::##FuncName( \
			BattleInclination::CharData& ownerInfo, BattleInclination::CharData& targetInfo)


		// ����
		DECLARE_ENEMY_CKECK(IsEnemyCommon);
		DECLARE_ENEMY_CKECK(IsNeutralExceptionCommon);
		DECLARE_ENEMY_CKECK(IsFriendCommon);
		DECLARE_ENEMY_CKECK(IsFriend2ndCommon);

		// ���÷�
		DECLARE_ENEMY_CKECK(IsEnemyForHunting);
		DECLARE_ENEMY_CKECK(IsNeutralExceptionForHunting);
		DECLARE_ENEMY_CKECK(IsFriendForHunting);
		DECLARE_ENEMY_CKECK(IsFriend2ndForHunting);

		// �������
		DECLARE_ENEMY_CKECK(IsEnemyForGuildWar);
		DECLARE_ENEMY_CKECK(IsNeutralExceptionForGuildWar);
		DECLARE_ENEMY_CKECK(IsFriendForGuildWar);
		DECLARE_ENEMY_CKECK(IsFriend2ndForGuildWar);

		// ��������
		DECLARE_ENEMY_CKECK(IsEnemyForRealmWar);
		DECLARE_ENEMY_CKECK(IsNeutralExceptionForRealmWar);
		DECLARE_ENEMY_CKECK(IsFriendForRealmWar);
		DECLARE_ENEMY_CKECK(IsFriend2ndForRealmWar);

		// ��������
		DECLARE_ENEMY_CKECK(IsEnemyForStatueWar);
		DECLARE_ENEMY_CKECK(IsNeutralExceptionForStatueWar);
		DECLARE_ENEMY_CKECK(IsFriendForStatueWar);
		DECLARE_ENEMY_CKECK(IsFriend2ndForStatueWar);

		// ��������
		DECLARE_ENEMY_CKECK(IsEnemyForSiegeWar);
		DECLARE_ENEMY_CKECK(IsNeutralExceptionForSiegeWar);
		DECLARE_ENEMY_CKECK(IsFriendForSiegeWar);
		DECLARE_ENEMY_CKECK(IsFriend2ndForSiegeWar);

		// �Ǿƽĺ� ��ŵ üũ
		DECLARE_SKIP_CKECK(IsEnemySkip);
		DECLARE_SKIP_CKECK(IsNeutralExceptionSkip);
		DECLARE_SKIP_CKECK(IsFriendSkip);
		DECLARE_SKIP_CKECK(IsFriend2ndSkip);

		// ���� ���� ���� üũ �Լ�
		DECLARE_ATTACKABLE_CHECK(IsAttackable);

		static unsigned char	FindAttackableCreatureType(BattleInclination::CharData& charInfo);
	};
}

#endif