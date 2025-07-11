
#include <Log/ServerLog.h>

#include <Creature/Siege/SiegeConstants.h>
#include <Creature/Monster/MonsterStructure.h>

#include "EnemyCheck.h"
#include "GMMemory.h"

using namespace EnemyCheck;
using namespace Creature;
using namespace Siege;

CCheckTable   CCheckTable::ms_this;

const unsigned char EnemyCheck::CFunctions::ms_AttackableTable[EnemyCheck::CFunctions::MAX_ATTACKABLE_CREATURE][EnemyCheck::CFunctions::MAX_ATTACKABLE_CREATURE] =
{
//    �Ʒ� ���ڴ� EnemyCheck::CFunctions::ReturnACT ��� ����!!
//	  PC,	����,	�� ��¡��,	����,	��� ���,	����,	�ٹ�,	����,	�ٰ�,	����,	���ۼ�,	NONE
	{ 1,	1,		1,			1,		1,			1,		1,		1,		1,		1,		1,		0},		// PC
	{ 1,	1,		0,			0,		0,			0,		0,		0,		0,		0,		0,		0},		// ����
	{ 1,	0,		0,			0,		0,			0,		0,		0,		0,		0,		0,		0},		// �� ��¡��
	{ 0,	0,		0,			0,		0,			0,		0,		0,		0,		0,		0,		0},		// ����
	{ 1,	0,		0,			0,		0,			0,		0,		0,		0,		0,		0,		0},		// ��� ���
	{ 1,	0,		0,			0,		0,			0,		0,		0,		1,		1,		0,		0},		// ����
	{ 0,	0,		0,			0,		0,			0,		0,		0,		1,		1,		1,		0},		// �ٰŸ� ���� ����
	{ 1,	0,		0,			0,		0,			0,		0,		0,		1,		1,		1,		0},		// ���Ÿ� ���� ���� (������� �ٴڿ� �������� ó���� ���Ŀ� �߰��Ѵ�.!!)
	{ 0,	0,		0,			1,		0,			0,		0,		0,		0,		0,		0,		0},		// �ٰŸ� ���� ���� (������� �ٴڿ� �������� ó���� ���Ŀ� �߰��Ѵ�.!!)
	{ 1,	0,		0,			1,		0,			1,		1,		1,		1,		1,		1,		0},		// ���Ÿ� ���� ���� (������� �ٴڿ� �������� ó���� ���Ŀ� �߰��Ѵ�.!!)
	{ 0,	0,		0,			0,		0,			0,		0,		0,		0,		0,		0,		0},		// ���ۼ�
	{ 0,	0,		0,			0,		0,			0,		0,		0,		0,		0,		0,		0}		// �޹�, NPC, �������NPC
} ;

bool EnemyCheck::IsCamp(unsigned short wObjectType)
{
	switch ( wObjectType )
	{
		case Siege::CAMP:
		case Siege::CAMP_SHOP:
		case Siege::MINING_CAMP:
		case Siege::KARTERANT_WEAPON:
		case Siege::MERKADIA_WEAPON:
			return true;
	}

	return false;
}

bool EnemyCheck::IsCastleArms(unsigned short wObjectType)
{
	switch ( wObjectType )
	{
		case Siege::GUARD :	
		case Siege::SHORT_RANGE_CASTLE_ARMS :
		case Siege::LONG_RANGE_CASTLE_ARMS :
		case Siege::EMBLEM:
		case Siege::GATE:	
			return true;
	}

	return false;
}

bool EnemyCheck::IsCastleNPC(unsigned short wObjectType)
{
	if ( Siege::CASTLE_ARMS_NPC == wObjectType )
	{
		return true;
	}

	return false;
}

bool EnemyCheck::IsSiegeArms(unsigned short wObjectType)
{
	switch ( wObjectType )
	{
		case Siege::SHORT_RANGE_SIEGE_ARMS :
		case Siege::LONG_RANGE_SIEGE_ARMS :
		case Siege::AIRSHIP :
			return true;
	}

	return false;
}

bool EnemyCheck::IsEmblem(unsigned short wObjectType)
{
	if(wObjectType==Siege::EMBLEM)
	{
		return true;
	}
	return false;
}

bool EnemyCheck::IsGate(unsigned short wObjectType)
{
	if(wObjectType==Siege::GATE)
	{
		return true;
	}
	return false;
}

bool EnemyCheck::IsStruct(unsigned short wKind)
{
	if (MonsterInfo::MIN_STATUE_KID <= wKind &&
		MonsterInfo::MAX_STATUE_KID >= wKind)
	{
		return true;
	}

	return false;
}


bool EnemyCheck::CCheckTable::Initialize(void)
{
	m_fnIsEnemy[EC_CHECK_ENEMY][RULE_HUNTING]				= CFunctions::IsEnemyForHunting;
	m_fnIsEnemy[EC_CHECK_NEUTRAL_EXCETION][RULE_HUNTING]	= CFunctions::IsNeutralExceptionForHunting;
	m_fnIsEnemy[EC_CHECK_FRIEND][RULE_HUNTING]				= CFunctions::IsFriendForHunting;
	m_fnIsEnemy[EC_CHECK_FRIEND_2ND][RULE_HUNTING]			= CFunctions::IsFriend2ndForHunting;

	m_fnIsEnemy[EC_CHECK_ENEMY][RULE_GUILD_WAR]				= CFunctions::IsEnemyForGuildWar;
	m_fnIsEnemy[EC_CHECK_NEUTRAL_EXCETION][RULE_GUILD_WAR]	= CFunctions::IsNeutralExceptionForGuildWar;
	m_fnIsEnemy[EC_CHECK_FRIEND][RULE_GUILD_WAR]			= CFunctions::IsFriendForGuildWar;
	m_fnIsEnemy[EC_CHECK_FRIEND_2ND][RULE_GUILD_WAR]		= CFunctions::IsFriend2ndForGuildWar;

	m_fnIsEnemy[EC_CHECK_ENEMY][RULE_REALM_WAR]				= CFunctions::IsEnemyForRealmWar;
	m_fnIsEnemy[EC_CHECK_NEUTRAL_EXCETION][RULE_REALM_WAR]	= CFunctions::IsNeutralExceptionForRealmWar;
	m_fnIsEnemy[EC_CHECK_FRIEND][RULE_REALM_WAR]			= CFunctions::IsFriendForRealmWar;
	m_fnIsEnemy[EC_CHECK_FRIEND_2ND][RULE_REALM_WAR]		= CFunctions::IsFriend2ndForRealmWar;

	m_fnIsEnemy[EC_CHECK_ENEMY][RULE_STATUE_WAR]			= CFunctions::IsEnemyForStatueWar;
	m_fnIsEnemy[EC_CHECK_NEUTRAL_EXCETION][RULE_STATUE_WAR]	= CFunctions::IsNeutralExceptionForStatueWar;
	m_fnIsEnemy[EC_CHECK_FRIEND][RULE_STATUE_WAR]			= CFunctions::IsFriendForStatueWar;
	m_fnIsEnemy[EC_CHECK_FRIEND_2ND][RULE_STATUE_WAR]		= CFunctions::IsFriend2ndForStatueWar;

	m_fnIsEnemy[EC_CHECK_ENEMY][RULE_SIEGE_WAR]				= CFunctions::IsEnemyForSiegeWar;
	m_fnIsEnemy[EC_CHECK_NEUTRAL_EXCETION][RULE_SIEGE_WAR]	= CFunctions::IsNeutralExceptionForSiegeWar;
	m_fnIsEnemy[EC_CHECK_FRIEND][RULE_SIEGE_WAR]			= CFunctions::IsFriendForSiegeWar;
	m_fnIsEnemy[EC_CHECK_FRIEND_2ND][RULE_SIEGE_WAR]		= CFunctions::IsFriend2ndForSiegeWar;

	m_fnCheckSkip[EC_CHECK_ENEMY]				= CFunctions::IsEnemySkip;
	m_fnCheckSkip[EC_CHECK_NEUTRAL_EXCETION]	= CFunctions::IsNeutralExceptionSkip;
	m_fnCheckSkip[EC_CHECK_FRIEND]				= CFunctions::IsFriendSkip;
	m_fnCheckSkip[EC_CHECK_FRIEND_2ND]			= CFunctions::IsFriend2ndSkip;

	m_bInit = true;

	return true;
}

unsigned long EnemyCheck::CCheckTable::IsEnemyFromStruct(BattleInclination::CharData& ownerInfo, 
														 BattleInclination::CharData& targetInfo, 
														 BattleInclination::RelationData& relationInfo, 
														 unsigned char& cResult)
{
	if (false == m_bInit)
	{
		ERRLOG0(g_Log, "�Ǿƽĺ� �Լ� ���̺��� �ʱ�ȭ���� ���� ���¿��� �Ǿƽĺ��� �õ��Ͽ����ϴ�.");
		return EC_ERROR;
	}

	if (MAX_RULE_NUM == m_eRule)
	{
		ERRLOG0(g_Log, "�Ǿƽĺ� ���� �������� ���� ���¿��� �Ǿƽĺ��� �õ��Ͽ����ϴ�.");
		return EC_ERROR;
	}

	// ���������� ����ȴ�.
	for (unsigned char cIndex = 0; cIndex < MAX_CHECK_TYPE; ++cIndex)
	{
		if (NULL != m_fnIsEnemy[cIndex][m_eRule])
		{
			if (NULL != m_fnCheckSkip[cIndex] && true == m_fnCheckSkip[cIndex](ownerInfo, targetInfo, m_eRule))
			{
				continue;
			}

			if (true == m_fnIsEnemy[cIndex][m_eRule](ownerInfo, targetInfo, relationInfo, cResult))
			{
				return GetEnemyType(cIndex);
			}
		}
	}

	// �߸� ó��
	return EC_NEUTRAL;
}

unsigned long EnemyCheck::CCheckTable::GetEnemyType(unsigned char cEnemyCheckType) const
{
	switch (cEnemyCheckType)
	{
		case EC_CHECK_ENEMY:				return EC_ENEMY;
		case EC_CHECK_NEUTRAL_EXCETION:		return EC_NEUTRAL;
		case EC_CHECK_FRIEND:				return EC_FRIEND;
		case EC_CHECK_FRIEND_2ND:			return EC_FRIEND;

		default:							return EC_NEUTRAL;
	}

	return EC_NEUTRAL;
}


EnemyCheck::CCheckTable::CCheckTable()
:	m_bInit(false), m_eRule(MAX_RULE_NUM)
{
}

EnemyCheck::CCheckTable::~CCheckTable()
{
}

// Rodin : ������� �� �ӽ� Ż����� ���� ó�� �ʿ�

// ----------------------------------------------------------------------
// ����

DEFINE_ENEMY_CKECK(IsEnemyCommon)
{
	if (relationInfo.m_cVSFlag & BattleInclination::VS_CHARACTER)
	{
		// ����� PvP�� ����� �� (���)
		cResult = EN_COMMON_PVP;
		return true;
	}

	if (Creature::STATELESS == targetInfo.m_cNation)
	{
		if (CT_MONSTER == targetInfo.m_cCreatureType)
		{
			// ������(��3��) ����
			cResult = EN_COMMON_STATELESS_MONSTER;
			return true;
		}
	}

	return false;
}

DEFINE_ENEMY_CKECK(IsNeutralExceptionCommon)
{
	if (ownerInfo.m_dwCID != targetInfo.m_dwCID && true == targetInfo.IsGODMode())
	{
		// ��밡 ������ �ƴϸ� G����� ��� �߸� ó��
		cResult = NE_COMMON_GMODE;
		return true;
	}

	if (Creature::STATELESS != targetInfo.m_cNation &&
		ownerInfo.m_cNation != targetInfo.m_cNation && 
		(true == ownerInfo.IsPeaceMode() || true == targetInfo.IsPeaceMode()))
	{
		// ��밡 ������ �ƴϸ� G����� ��� �߸� ó��
		cResult = NE_COMMON_GMODE;
		return true;
	}

	if (ownerInfo.m_cNation != targetInfo.m_cNation &&
		Creature::STATELESS != targetInfo.m_cNation &&
		CT_NPC == targetInfo.m_cCreatureType)
	{
		// ��� ������ NPC
		cResult = NE_COMMON_ENEMY_NPC;
		return true;
	}

	return false;
}

DEFINE_ENEMY_CKECK(IsFriendCommon)
{
	if (0 != ownerInfo.m_dwPID && 0 != targetInfo.m_dwPID)
	{
		if (ownerInfo.m_dwPID == targetInfo.m_dwPID)
		{
			// ��������� ����� ���� ��Ƽ
			cResult = FR_COMMON_MYPARTY;
			return true;
		}
	}

	if (0 != ownerInfo.m_dwGID && 0 != targetInfo.m_dwGID)
	{
		if (ownerInfo.m_dwGID == targetInfo.m_dwGID)
		{
			// ��������� ����� ���� ���
			cResult = FR_COMMON_MYGUILD;
			return true;
		}
	}

	if (ownerInfo.m_dwCID == targetInfo.m_dwCID)
	{
		// �ڱ� �ڽ�
		cResult = FR_COMMON_MYSELF;
		return true;
	}

	return false;
}

DEFINE_ENEMY_CKECK(IsFriend2ndCommon)
{
	if (ownerInfo.m_cNation == targetInfo.m_cNation)
	{
		if (CT_PC == targetInfo.m_cCreatureType || 
			CT_NPC == targetInfo.m_cCreatureType)
		{
			// �ڱ� PC, NPC
			cResult = FR2_COMMON_MYNATION_PC_NPC;
			return true;
		}
	}

	if (Creature::STATELESS == targetInfo.m_cNation)
	{
		if (CT_NPC == targetInfo.m_cCreatureType)
		{
			// ������ NPC
			cResult = FR2_COMMON_STATELESS_NPC;
			return true;
		}
	}

	return false;
}

// ----------------------------------------------------------------------
// ���÷�

DEFINE_ENEMY_CKECK(IsEnemyForHunting)
{
	if (true == USE_ENEMY_CKECK(IsEnemyCommon))
	{
		// ����
		return true;
	}

	if(CT_SIEGE_OBJECT == ownerInfo.m_cCreatureType ||
		CT_SIEGE_OBJECT == targetInfo.m_cCreatureType)
		return false;

	if (relationInfo.m_cVSFlag & BattleInclination::VS_PARTY || 
		relationInfo.m_cVSFlag & BattleInclination::VS_GUILD)
	{
		// ����� �������� ����� �׷� ��� (��Ƽ/��� ���)
		cResult = EN_HUNTING_VSPARTY_VSGUILD;
		return true;
	}

	if ( !ownerInfo.IsSafetyZone() && !targetInfo.IsSafetyZone())
	{
		// ���� ���븦 �����߰ų� ���κ��� ���븦 ���� ���� ���
		if (true == relationInfo.m_bHostileGuild)
		{
			// �ڽŰ� ����� ������� �������̸� ���� ���� �϶�
			cResult = EN_GUILDWAR_HOSTILITY_GUILD;
			return true;
		}
	}

	return false;
}

DEFINE_ENEMY_CKECK(IsNeutralExceptionForHunting)
{
	if (true == USE_ENEMY_CKECK(IsNeutralExceptionCommon))
	{
		// ����
		return true;
	}

	// ����Ʋ�� �������� PC
	if (true == ownerInfo.IsTeamBattle())
	{
		if (false == targetInfo.IsTeamBattle())
		{
			// ������ �������� ���� ��� �ο�
			cResult = NE_HUNTING_NON_BATTLER;
			return true;
		}

		if (!(relationInfo.m_cVSFlag & BattleInclination::VS_PARTY || 
			  relationInfo.m_cVSFlag & BattleInclination::VS_GUILD))
		{
			// �ڽŰ� �ٸ� ����Ʋ�� �������� ��� �ο�
			cResult = NE_HUNTING_OTHER_BATTLER;
			return true;
		}

	}
	// ����Ʋ�� ���������� ���� PC
	else
	{
		if (true == targetInfo.IsTeamBattle())
		{
			// ����Ʋ�� �������� ��� �ο�
			cResult = NE_HUNTING_BATTLER;
			return true;
		}
	}

	return false;
}

DEFINE_ENEMY_CKECK(IsFriendForHunting)
{
	if (true == USE_ENEMY_CKECK(IsFriendCommon))
	{
		// ����
		return true;
	}

	return false;
}

DEFINE_ENEMY_CKECK(IsFriend2ndForHunting)
{
	if (true == USE_ENEMY_CKECK(IsFriend2ndCommon))
	{
		// ����
		return true;
	}

	return false;
}

// ----------------------------------------------------------------------
// �������

DEFINE_ENEMY_CKECK(IsEnemyForGuildWar)
{
	if (true == USE_ENEMY_CKECK(IsEnemyCommon))
	{
		// ����
		return true;
	}

	// ������϶� ��� ������Ʈ�ε� ������ �ٸ��� ������ ��������̴�.
	if(CT_SIEGE_OBJECT == ownerInfo.m_cCreatureType || CT_SIEGE_OBJECT == targetInfo.m_cCreatureType)
	{
//		if (ownerInfo.m_cNation != targetInfo.m_cNation && Creature::STATELESS != targetInfo.m_cNation)
		if(ownerInfo.m_dwGID != targetInfo.m_dwGID)
		{
			// ��� ������ PC, ����, NPC
			cResult = EN_REALMWAR_ENEMY_PC_MONSTER_NPC;
			return true;
		}
	}
/*
	// �ڽ��� ����� ������
	if ( !ownerInfo.IsSafetyZone() &&
		 (Creature::WAR_ON == ownerInfo.m_cGuildWarFlag ||
		  Creature::WAR_INSTANCE == ownerInfo.m_cGuildWarFlag) )
		 
	{
		// ����� ����� �������϶�
		if ( !targetInfo.IsSafetyZone() && 
			 (Creature::WAR_ON == targetInfo.m_cGuildWarFlag ||
			  Creature::WAR_INSTANCE == targetInfo.m_cGuildWarFlag) )
		{
			// ���� ���븦 �����߰ų� ���κ��� ���븦 ���� ���� ���
			if (true == relationInfo.m_bHostileGuild)
			{
				// �ڽŰ� ����� ������� �������̸� ���� ���� �϶�
				cResult = EN_GUILDWAR_HOSTILITY_GUILD;
				return true;
			}
		}
		
		if(!targetInfo.IsSafetyZone())
		{
			// edith 2009.06.13 ����������� ���� ��带 �����ϰ� ��� ��
			if(targetInfo.m_cCreatureType == CT_PC)
			{
				if(ownerInfo.m_dwGID == targetInfo.m_dwGID)
					return USE_ENEMY_CKECK(IsEnemyForRealmWar);

				// ��ȣ��尡�ƴҰ�� ������ �� 
				cResult = EN_GUILDWAR_HOSTILITY_GUILD;
				return true;
			}

			// ����� ������� ���������� ������
			return USE_ENEMY_CKECK(IsEnemyForRealmWar);
		}
	}
	// �ڽ��� ����� ��������
	else if (Creature::WAR_OFF == ownerInfo.m_cGuildWarFlag)
	{
		if(!targetInfo.IsSafetyZone())
		{
			// edith 2009.06.13 ����������� ���� ��带 �����ϰ� ��� ��
			if(targetInfo.m_cCreatureType == CT_PC)
			{
				if(ownerInfo.m_dwGID == targetInfo.m_dwGID)
					return USE_ENEMY_CKECK(IsEnemyForRealmWar);

				// ��ȣ��尡�ƴҰ�� ������ �� 
				cResult = EN_GUILDWAR_HOSTILITY_GUILD;
				return true;
			}
		}

		// ���÷� ����
		return USE_ENEMY_CKECK(IsEnemyForRealmWar);
	}
*/

	// �ڽ��� ����� ������
	if ( !ownerInfo.IsSafetyZone() )		 
	{
		// ����� ����� �������϶�
		if ( !targetInfo.IsSafetyZone() )
		{
			// ���� ���븦 �����߰ų� ���κ��� ���븦 ���� ���� ���
			if (true == relationInfo.m_bHostileGuild)
			{
				// �ڽŰ� ����� ������� �������̸� ���� ���� �϶�
				cResult = EN_GUILDWAR_HOSTILITY_GUILD;
				return true;
			}

			// edith 2009.06.13 ����������� ���� ��带 �����ϰ� ��� ��
			if(targetInfo.m_cCreatureType == CT_PC)
			{
				if(ownerInfo.m_dwGID == targetInfo.m_dwGID)
					return USE_ENEMY_CKECK(IsEnemyForRealmWar);

				// ��ȣ��尡�ƴҰ�� ������ �� 
				cResult = EN_GUILDWAR_HOSTILITY_GUILD;
				return true;
			}

			// ����� ������� ���������� ������
			return USE_ENEMY_CKECK(IsEnemyForRealmWar);
		}
	}
	return false;
}

DEFINE_ENEMY_CKECK(IsNeutralExceptionForGuildWar)
{
	if (true == USE_ENEMY_CKECK(IsNeutralExceptionCommon))
	{
		// ����
		return true;
	}

	// �ڽ��� ����� ������
	if (Creature::WAR_ON == ownerInfo.m_cGuildWarFlag ||
		Creature::WAR_INSTANCE == ownerInfo.m_cGuildWarFlag)
	{
		if ( ownerInfo.IsSafetyZone() )
		{
			// ���� ���븦 �����߰ų� ���κ��� ���븦 ���� ���� ���
			if (true == relationInfo.m_bHostileGuild)
			{
				// ���� ������Ƽ ���� ���� ��� ���� ���� ������ ��� �ο�
				cResult = NE_GUILDWAR_IN_SAFETYZONE;
				return true;
			}
		}

		if (Creature::WAR_OFF == targetInfo.m_cGuildWarFlag)
		{
			// ����� ���������� ���� ��� �ο�
			cResult = NE_GUILDWAR_NON_BATTLER;
			return true;
		}

		if ( ownerInfo.m_dwGID != targetInfo.m_dwGID &&
			 (Creature::WAR_ON == targetInfo.m_cGuildWarFlag || Creature::WAR_INSTANCE == targetInfo.m_cGuildWarFlag) )
		{
			// �ڽ��� ��尡 �ƴ� ������� ������
			cResult = NE_GUILDWAR_OTHER_BATTLER;
			return true;
		}
	}
	// �ڽ��� ����� ��������
	else if (Creature::WAR_OFF == ownerInfo.m_cGuildWarFlag)
	{
		if (Creature::WAR_ON == targetInfo.m_cGuildWarFlag ||
			Creature::WAR_INSTANCE == targetInfo.m_cGuildWarFlag)
		{
			// ����� �������� ��� �ο�
			cResult = NE_GUILDWAR_BATTLER;
			return true;
		}

		// ���÷� ����
		return USE_ENEMY_CKECK(IsNeutralExceptionForRealmWar);
	}

	return false;
}

DEFINE_ENEMY_CKECK(IsFriendForGuildWar)
{
	if (true == USE_ENEMY_CKECK(IsFriendCommon))
	{
		// ����
		return true;
	}

	// �ڽ��� ����� ������
	if (Creature::WAR_ON == ownerInfo.m_cGuildWarFlag ||
		Creature::WAR_INSTANCE == ownerInfo.m_cGuildWarFlag)
	{
		if (ownerInfo.m_dwGID == targetInfo.m_dwGID)
		{
			// ���� ������ ��� �׷�
			cResult = FR_GUILDWAR_MYGUILD;
			return true;
		}
	}
	// �ڽ��� ����� ��������
	else if (Creature::WAR_OFF == ownerInfo.m_cGuildWarFlag)
	{
		// ���÷� ����
		return USE_ENEMY_CKECK(IsFriendForRealmWar);
	}

	return false;
}

DEFINE_ENEMY_CKECK(IsFriend2ndForGuildWar)
{
	if (true == USE_ENEMY_CKECK(IsFriend2ndCommon))
	{
		// ����
		return true;
	}

	return false;
}

// ----------------------------------------------------------------------
// ��������

DEFINE_ENEMY_CKECK(IsEnemyForRealmWar)
{
	if (true == USE_ENEMY_CKECK(IsEnemyCommon))
	{
		// ����
		return true;
	}

	if(CT_SIEGE_OBJECT == ownerInfo.m_cCreatureType ||
		CT_SIEGE_OBJECT == targetInfo.m_cCreatureType)
		return false;
/*
	if (ownerInfo.m_cNation != targetInfo.m_cNation && Creature::STATELESS != targetInfo.m_cNation)
	{
		if (CT_PC == targetInfo.m_cCreatureType || 
			CT_MONSTER == targetInfo.m_cCreatureType || 
			CT_NPC == targetInfo.m_cCreatureType)
		{
			// ��� ������ PC, ����, NPC
			cResult = EN_REALMWAR_ENEMY_PC_MONSTER_NPC;
			return true;
		}
	}
*/

	if ( !ownerInfo.IsSafetyZone() && !targetInfo.IsSafetyZone())
	{
		if (ownerInfo.m_cNation != targetInfo.m_cNation && Creature::STATELESS != targetInfo.m_cNation)
		{
			if (CT_PC == targetInfo.m_cCreatureType || 
				CT_MONSTER == targetInfo.m_cCreatureType || 
				CT_NPC == targetInfo.m_cCreatureType)
			{
				// ��� ������ PC, ����, NPC
				cResult = EN_REALMWAR_ENEMY_PC_MONSTER_NPC;
				return true;
			}
		}

		// ���� ���븦 �����߰ų� ���κ��� ���븦 ���� ���� ���
		if (true == relationInfo.m_bHostileGuild)
		{
			// �ڽŰ� ����� ������� �������̸� ���� ���� �϶�
			cResult = EN_GUILDWAR_HOSTILITY_GUILD;
			return true;
		}
	}
	
	return false;
}

DEFINE_ENEMY_CKECK(IsNeutralExceptionForRealmWar)
{
	if (true == USE_ENEMY_CKECK(IsNeutralExceptionCommon))
	{
		// ����
		return true;
	}

	if (ownerInfo.m_cNation != targetInfo.m_cNation)
	{
		if (CT_STRUCT == targetInfo.m_cCreatureType)
		{
			// �ڽŰ� �ٸ� ������ ���� ��Ʈ����
			cResult = NE_REALMWAR_ENEMY_STRUCT;
			return true;
		}

		if (IsGate(targetInfo.m_wObjectType) ||
			IsEmblem(targetInfo.m_wObjectType) ||
			IsSiegeArms(targetInfo.m_wObjectType))
		{
			// ��� ������ ���� ��ȯ��, ���� �빮, ��ȯ�� ���� ����.
			cResult = NE_REALMWAR_ENEMY_GATE_EMBLEM_SIEGEOBJECT;
			return true;
		}		
	}



	return false;
}

DEFINE_ENEMY_CKECK(IsFriendForRealmWar)
{
	if (true == USE_ENEMY_CKECK(IsFriendCommon))
	{
		// ����
		return true;
	}

	if (ownerInfo.m_cNation == targetInfo.m_cNation)
	{
		if (CT_PC == targetInfo.m_cCreatureType || 
			CT_NPC == targetInfo.m_cCreatureType || 
			CT_MONSTER == targetInfo.m_cCreatureType ||
			CT_STRUCT == targetInfo.m_cCreatureType)
		{
			// �ڱ� ������ PC, NPC, ����, ����
			cResult = FR_REALMWAR_MYNATION_PC_NPC_MONSTER_STRUCT;
			return true;
		}

		if (IsGate(targetInfo.m_wObjectType) ||
			IsEmblem(targetInfo.m_wObjectType) ||
			IsSiegeArms(targetInfo.m_wObjectType))
		{
			// �ڱ� ������ ��ȯ��, ���� �빮, ���� ����.
			cResult = FR_REALMWAR_MYNATION_GATE_EMBLEM_SIEGEOBJECT;
			return true;
		}		
	}

	return false;
}

DEFINE_ENEMY_CKECK(IsFriend2ndForRealmWar)
{
	if (true == USE_ENEMY_CKECK(IsFriend2ndCommon))
	{
		// ����
		return true;
	}

	return false;
}

// ----------------------------------------------------------------------
// ��������

DEFINE_ENEMY_CKECK(IsEnemyForStatueWar)
{
	if (true == USE_ENEMY_CKECK(IsEnemyCommon))
	{
		// ����
		return true;
	}
	if ( !ownerInfo.IsSafetyZone() && !targetInfo.IsSafetyZone())//hz added this check issafetyzone for edin attack racewar.
	{
		if (ownerInfo.m_cNation != targetInfo.m_cNation)
		{
			if (Creature::STATELESS != targetInfo.m_cNation)
			{
				if (CT_PC == targetInfo.m_cCreatureType || 
					CT_MONSTER == targetInfo.m_cCreatureType || 
					CT_STRUCT == targetInfo.m_cCreatureType)
				{
					// ��� ������ PC, ����, ��Ʈ��ó
					cResult = EN_STATUEWAR_ENEMY_PC_MONSTER_STRUCT;
					return true;
				}
			}
			else
			{
				if (CT_STRUCT == targetInfo.m_cCreatureType)
				{
					// �� 3���� ��Ʈ��ó
					cResult = EN_STATUEWAR_STATELESS_STRUCT;
					return true;
				}
			}
		}
	}
	return false;
}

DEFINE_ENEMY_CKECK(IsNeutralExceptionForStatueWar)
{
	if (true == USE_ENEMY_CKECK(IsNeutralExceptionCommon))
	{
		// ����
		return true;
	}

	if (ownerInfo.m_cNation != targetInfo.m_cNation)
	{
		if (IsGate(targetInfo.m_wObjectType) ||
			IsEmblem(targetInfo.m_wObjectType) ||
			IsSiegeArms(targetInfo.m_wObjectType))
		{
			// ��� ������ ���� ��ȯ��, ���� �빮, ��ȯ�� ���� ����.
			cResult = NE_STATUEWAR_ENEMY_GATE_EMBLEM_SIEGEOBJECT;
			return true;
		}
	}	

	return false;
}

DEFINE_ENEMY_CKECK(IsFriendForStatueWar)
{
	if (true == USE_ENEMY_CKECK(IsFriendCommon))
	{
		// ����
		return true;
	}

	if (ownerInfo.m_cNation == targetInfo.m_cNation)
	{
		if (CT_PC == targetInfo.m_cCreatureType || 
			CT_NPC == targetInfo.m_cCreatureType || 
			CT_MONSTER == targetInfo.m_cCreatureType || 
			CT_STRUCT == targetInfo.m_cCreatureType)
		{
			// �ڱ� ������ PC, NPC, ����, ����
			cResult = FR_STATUEWAR_MYNATION_PC_NPC_MONSTER_STRUCT;
			return true;
		}

		if (IsGate(targetInfo.m_wObjectType) ||
			IsEmblem(targetInfo.m_wObjectType) ||
			IsSiegeArms(targetInfo.m_wObjectType))
		{
			// �ڱ� ������ ��ȯ��, ���� �빮, ���� ����.
			cResult = FR_STATUEWAR_MYNATION_GATE_EMBLEM_SIEGEOBJECT;
			return true;
		}		
	}

	return false;
}

DEFINE_ENEMY_CKECK(IsFriend2ndForStatueWar)
{
	if (true == USE_ENEMY_CKECK(IsFriend2ndCommon))
	{
		// ����
		return true;
	}

	return false;
}

// ----------------------------------------------------------------------
// ��������

DEFINE_ENEMY_CKECK(IsEnemyForSiegeWar)
{
	/*if (true == USE_ENEMY_CKECK(IsEnemyCommon))
	{
		// ����
		return true;
	}

	//if (ownerInfo.m_cNation != targetInfo.m_cNation)
	if (ownerInfo.m_dwPID != targetInfo.m_dwPID)  // blackdog new wartype
	{
		if (CT_PC == targetInfo.m_cCreatureType || CT_MONSTER == targetInfo.m_cCreatureType)
		{
			// ��� ������ PC, ����.
			cResult = EN_SIEGE_ENEMY_PC_MONSTER;					 
			return true;
		}
		
		if (IsCastleArms(targetInfo.m_wObjectType))
		{
			// ��� ������ ����, �� ��¡��, ���� ����
			cResult = EN_SIEGE_ENEMY_GATE_EMBLEM_CASTLE_ARMS;
			return true;
		}

		if(IsSiegeArms(targetInfo.m_wObjectType))
		{
			// ��� ������ ��ȯ�� ���� ����.
			cResult = EN_SIEGE_ENEMY_SIEGE_ARMS;
			return true;
		}
	}


	return false;*/
	/*NEW*/ return true;/*NEW*/
}

DEFINE_ENEMY_CKECK(IsNeutralExceptionForSiegeWar)
{
	/*if (true == USE_ENEMY_CKECK(IsNeutralExceptionCommon))
	{
		// ����
		return true;
	}

	//if (ownerInfo.m_cNation != targetInfo.m_cNation)
	if (ownerInfo.m_dwPID != targetInfo.m_dwPID) // blackdog new wartype
	{
		if (CT_STRUCT == targetInfo.m_cCreatureType)
		{
			// �ڽŰ� �ٸ� ������ ���� ��Ʈ����
			cResult = NE_SIEGE_ENEMY_STRUCT;
			return true;
		}
	}*/

	return false;
}

DEFINE_ENEMY_CKECK(IsFriendForSiegeWar)
{
	/*if (true == USE_ENEMY_CKECK(IsFriendCommon))
	{
		// ����
		return true;
	}

	if (ownerInfo.m_cNation == targetInfo.m_cNation)
	//if (ownerInfo.m_dwPID == targetInfo.m_dwPID)  // blackdog new wartype
	{
		if (CT_PC == targetInfo.m_cCreatureType || 
			CT_NPC == targetInfo.m_cCreatureType || 
			CT_MONSTER == targetInfo.m_cCreatureType ||
			CT_STRUCT == targetInfo.m_cCreatureType)
		{
			// �ڱ� ������ PC, NPC, ����, ����
			cResult = FR_SIEGEWAR_MYNATION_PC_NPC_MONSTER_STRUCT;
			return true;
		}

		if (IsSiegeArms(targetInfo.m_wObjectType) ||
			IsCastleArms(targetInfo.m_wObjectType) ||
			IsCastleNPC(targetInfo.m_wObjectType))
		{
			// �ڱ� ������ ��ȯ��, ���� �빮, ���� ����, ���� ����.
			cResult = FR_SIEGEWAR_MYNATION_SIEGEOBJECT;
			return true;
		}		
	}*/

	return false;
}

DEFINE_ENEMY_CKECK(IsFriend2ndForSiegeWar)
{
	/*if (true == USE_ENEMY_CKECK(IsFriend2ndCommon))
	{
		// ����
		return true;
	}*/

	return false;
}

// ----------------------------------------------------------------------
// �Ǿƽĺ� ��ŵ üũ
DEFINE_SKIP_CHECK(IsEnemySkip)
{
	if (true == targetInfo.IsGODMode())
	{
		// ��밡 G����� ��� ���ĺ� ��ŵ
		return true;
	}

	if (CT_PC == targetInfo.m_cCreatureType && (true == ownerInfo.IsPeaceMode() || true == targetInfo.IsPeaceMode()))
	{
		// ��밡 ��ȭ����� ��� ���ĺ� ��ŵ
		return true;
	}

	if (false == IsAttackable(ownerInfo, targetInfo))
	{
		// ��밡 ������ �� ���� ũ���Ķ�� ���ĺ� ��ŵ
		return true;
	}

	if (RULE_REALM_WAR == eRule || RULE_STATUE_WAR == eRule)
	{
		// �ڽ��� ������ ������Ʈ�� ž���� ��� ���ĺ� ��ŵ
		if (ownerInfo.m_cRide)
		{
			if (ownerInfo.IsCastleObject() || IsSiegeArms(ownerInfo.m_wObjectType)) 
			{
				return true;
			}
		}
	}
	
	return false;
}

DEFINE_SKIP_CHECK(IsNeutralExceptionSkip)
{
	if (ownerInfo.m_cNation == targetInfo.m_cNation &&
		Creature::STATELESS != targetInfo.m_cNation &&
		CT_NPC == targetInfo.m_cCreatureType)
	{
		// �ڱ� NPC �� ���� �߸� ����ó�� ��ŵ
		return true;
	}

	if (RULE_REALM_WAR == eRule || RULE_STATUE_WAR == eRule)
	{
		// �ڽ��� ������ ������Ʈ�� ž���� ��� �߸� ����ó�� ��ŵ
		if (ownerInfo.m_cRide)
		{
			if (ownerInfo.IsCastleObject() || IsSiegeArms(ownerInfo.m_wObjectType)) 
			{
				return true;
			}
		}
	}

	return false;
}

DEFINE_SKIP_CHECK(IsFriendSkip)
{
	return false;
}

DEFINE_SKIP_CHECK(IsFriend2ndSkip)
{
	return false;
}

// ----------------------------------------------------------------------
// �Ǿƽĺ� ��ŵ üũ
DEFINE_ATTACKABLE_CHECK(IsAttackable)
{
	unsigned char cOwnerType = FindAttackableCreatureType(ownerInfo);
	unsigned char cTargetType = FindAttackableCreatureType(targetInfo);

	if (RET_ALLWAYS == ms_AttackableTable[cOwnerType][cTargetType])
	{
		return true;
	}
	else if (RET_DEVELOPING == ms_AttackableTable[cOwnerType][cTargetType] && Siege::DEVELOPING == targetInfo.m_cSiegeState)
	{
		return true;
	}

	return false;
}

unsigned char	EnemyCheck::CFunctions::FindAttackableCreatureType(BattleInclination::CharData& charInfo)
{
	unsigned char cType = ACT_NONE;

	switch (charInfo.m_cCreatureType)
	{
		case CT_PC:
			cType = ACT_PC;
			break;

		case CT_NPC:
			cType = ACT_NONE;
			break;

		case CT_MONSTER:
		case CT_SUMMON:
		case CT_STRUCT:
			cType = ACT_MONSTER;
			break;

		case CT_SIEGE_OBJECT:
		{
			switch (charInfo.m_wObjectType)
			{
				case Siege::EMBLEM:
					cType = ACT_EMBLEM;
					break;

				case Siege::GATE:
					cType = ACT_GATE;
					break;

				case Siege::CAMP:
				case Siege::MINING_CAMP:
				case Siege::CAMP_SHOP:
				case Siege::KARTERANT_WEAPON:
				case Siege::MERKADIA_WEAPON:
					cType = ACT_CAMP;
					break;

				case Siege::BACKDOOR:
				case Siege::CASTLE_ARMS_NPC:
					cType = ACT_NONE;
					break;

				case Siege::GUARD:
					cType = ACT_GUARD;
					break;

				case Siege::SHORT_RANGE_CASTLE_ARMS:
					cType = ACT_SHORT_RANGE_CASTLE_ARMS;
					break;

				case Siege::LONG_RANGE_CASTLE_ARMS:
					cType = ACT_LONG_RANGE_CASTLE_ARMS;
					break;

				case Siege::SHORT_RANGE_SIEGE_ARMS:
					cType = ACT_SHORT_RANGE_SIEGE_ARMS;
					break;

				case Siege::LONG_RANGE_SIEGE_ARMS:
					cType = ACT_LONG_RANGE_SIEGE_ARMS;
					break;

				case Siege::AIRSHIP:
					cType = ACT_AIRSHIP;
					break;
			}

			break;
		}
	}

	return cType;
}
