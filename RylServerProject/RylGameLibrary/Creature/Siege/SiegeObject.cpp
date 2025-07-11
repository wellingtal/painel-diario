#include "stdafx.h"
#include "SiegeObject.h"

#include <Utility/Math/Math.h>
#include <Utility/Setup/ServerSetup.h>

#include <Item/ItemFactory.h>

#include <Network/ClientSocket/ClientConstants.h>
#include <Network/Dispatch/GameClient/GameClientDispatch.h>
#include <Network/Dispatch/GameClient/SendCharItem.h>
#include <Network/Dispatch/GameClient/SendCharCastle.h>
#include <Network/Dispatch/GameClient/SendCharAttack.h>
#include <Network/Dispatch/DBAgent/DBAgentDispatch.h>
#include <Network/Dispatch/DBAgent/DBAgentRequest.h>
#include <Network/Packet/WrapPacket.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>

#include <Community/Guild/Guild.h>
#include <Community/Guild/GuildMgr.h>

#include <Creature/Siege/SiegeObjectMgr.h>
#include <Creature/Monster/MonsterMgr.h>
#include <Creature/EnemyCheck.h>

#include <Castle/Castle.h>
#include <Castle/CastleMgr.h>

#include <Map/FieldMap/CellManager.h>
#include <Map/DuelMap/DuelCellManager.h>

#include <Skill/SkillTable.h>
#include <Skill/SkillMgr.h>

#include <GameTime/GameTimeConstants.h>
#include <GameTime/GameTimeMgr.h>


CSiegeObject::CSiegeObject(MonsterCreateInfo& MonsterCreate, const CastleObjectInfo& CastleObject)
: CSkillMonster(MonsterCreate), m_dwCampOrCastleID(CastleObject.m_dwCastleID), m_dwOwnerID(CastleObject.m_dwOwnerID),
  m_cNation(CastleObject.m_cNation), m_dwGID(0), m_wObjectType(CastleObject.m_wObjectType), m_cState(CastleObject.m_cState),
  m_cSubState(CastleObject.m_cSubState), m_cUpgradeStep(CastleObject.m_cUpgradeStep), m_cUpgradeType(CastleObject.m_cUpgradeType),
  m_cMaterial(0), m_cSiegeCount(0), m_fDefaultDir(CastleObject.m_fDefaultDir), m_dwRiderCID(0), m_cHPUpdateCount(0),
  m_dwBroadCastSize(0), m_dwDeltaSize(0)
{
	InitMonster(MonsterCreate.m_Pos);
	m_MotionInfo.m_fDirection = m_fDefaultDir;

	m_CreatureStatus.m_nNowHP = static_cast<unsigned short>(CastleObject.m_dwHP);

	switch (m_wObjectType)
	{
		case Siege::GUARD:		m_wDefaultSearchRange = 32;	break;
		case Siege::EMBLEM:		m_wDefaultSearchRange = 32;	break;
	}

	// 브로드캐스팅 데이터 준비
	m_SerializeSiegeObjectData.PrepareData( *this );
	m_SerializeSiegeObjectData.ClearDeltaData();
}

CSiegeObject::CSiegeObject(MonsterCreateInfo& MonsterCreate, unsigned long dwCampID, unsigned long dwGID, 
						   unsigned long dwHP, unsigned short wObjectType, unsigned char cState, 
						   unsigned char cSubState, unsigned char cUpgradeStep, unsigned char cMaterial, 
						   unsigned char cSiegeCount, bool bFullHP)
: CSkillMonster(MonsterCreate), m_dwCampOrCastleID(dwCampID), m_dwOwnerID(0), m_wObjectType(wObjectType),
  m_dwGID(dwGID), m_cNation(0), m_cState(cState), m_cSubState(cSubState), m_cUpgradeStep(cUpgradeStep),
  m_cUpgradeType(0), m_cMaterial(cMaterial), m_cSiegeCount(cSiegeCount), m_fDefaultDir(0), m_dwRiderCID(0),
  m_cHPUpdateCount(0), m_dwBroadCastSize(0),m_dwDeltaSize(0)
{
	InitMonster(MonsterCreate.m_Pos);

	if (bFullHP) m_CreatureStatus.m_nNowHP = m_CreatureStatus.m_StatusInfo.m_nMaxHP;
	else m_CreatureStatus.m_nNowHP = static_cast<unsigned short>(dwHP);

	// 브로드캐스팅 데이터 준비
	m_SerializeSiegeObjectData.PrepareData( *this );
	m_SerializeSiegeObjectData.ClearDeltaData();
}

CSiegeObject::CSiegeObject(MonsterCreateInfo& MonsterCreate, unsigned long dwOwnerID, unsigned char cNation,
						   unsigned long dwHP, unsigned short wObjectType, unsigned char cState, unsigned char cUpgradeStep)
: CSkillMonster(MonsterCreate), m_dwCampOrCastleID(0), m_dwOwnerID(dwOwnerID), m_wObjectType(wObjectType), m_cNation(cNation),
  m_dwGID(0), m_cState(cState), m_cSubState(0), m_cUpgradeStep(cUpgradeStep), m_cUpgradeType(0), m_cMaterial(0),
  m_cSiegeCount(0), m_fDefaultDir(0), m_dwRiderCID(0), m_cHPUpdateCount(0), m_dwBroadCastSize(0),m_dwDeltaSize(0)
{
	InitMonster(MonsterCreate.m_Pos);

	if (0 == dwHP) m_CreatureStatus.m_nNowHP = m_CreatureStatus.m_StatusInfo.m_nMaxHP;
	else m_CreatureStatus.m_nNowHP = static_cast<unsigned short>(dwHP);

	// 브로드캐스팅 데이터 준비
	m_SerializeSiegeObjectData.PrepareData( *this );
	m_SerializeSiegeObjectData.ClearDeltaData();
}

CSiegeObject::~CSiegeObject()
{
}

EnemyCheck::EnemyType CSiegeObject::IsEnemy(CCreature* lpTarget, unsigned char* cResult)
{
	if (NULL == lpTarget)
	{
		ERRLOG1(g_Log, "CID:0x%08x 피아식별할 타겟이 없습니다.", m_dwCID);
		return EnemyCheck::EC_NEUTRAL;
	}

	BattleInclination::CharData     ownerInfo;
	BattleInclination::CharData     targetInfo;

	BattleInclination::SetCharData(*this, ownerInfo);
	BattleInclination::SetCharData(*lpTarget, targetInfo);

	BattleInclination::RelationData	relationInfo;
	BattleInclination::SetRelationData(*this, *lpTarget, relationInfo);

	unsigned char cResultFromStruct = 0;
	unsigned long dwResult = EnemyCheck::CCheckTable::GetInstance().IsEnemyFromStruct(
		ownerInfo, targetInfo, relationInfo, cResultFromStruct);

	return static_cast<EnemyCheck::EnemyType>(dwResult);
}

unsigned char	CSiegeObject::IsRider(unsigned long dwCID) const
{
	if (m_dwRiderCID == dwCID)
	{
		return Siege::RIDER_FOR_OWNER;
	}

	return Siege::NOT_RIDER;
}

bool CSiegeObject::SkillAttack()
{
	unsigned char cSkillLockCount = m_cUpgradeStep + 1;
	unsigned char cSkillLevel = CSkillMonster::USE_SKILL_LEVEL;

	if (cSkillLockCount >= CSkillMgr::MAX_SKILL_LOCKCOUNT - 1)
	{
		cSkillLockCount = CSkillMgr::MAX_SKILL_LOCKCOUNT - 2;
	}


	enum { FIRST_PATTERN=0, SECOND_PATTERN=1, MAX_PATTERN = 2 };
	int nSelectPattern = FIRST_PATTERN;

	CAggresiveCreature*	ppAggresiveCreature[AtNode::MAX_DEFENDER_NUM] = {0, };

	AtType attackType;
	attackType.m_cSkillLockCount	= cSkillLockCount;
	attackType.m_cSkillLevel		= cSkillLevel;

	char nSkillPattern = 0;

	while (nSelectPattern < MAX_PATTERN)
	{
		switch (nSelectPattern)
		{
		case FIRST_PATTERN:
			{
				// 조건		대상	행동
				// A		B		A	: 스킬 사용 가능 MP 잔여량 있음 / 대상이 스킬 사용 가능 범위에 있을 때			
				//						: 자신에게 가장 위협적인 적에게
				//						: A 스킬 사용

				// 스킬이 없다면.. 다음 패턴으로..
				if (0 == m_MonsterInfo.m_wSkillID[A_SKILL])
				{
					++nSelectPattern;
					continue;
				}

				nSkillPattern = A_SKILL;
				attackType.m_wType = m_MonsterInfo.m_wSkillID[A_SKILL];
				ppAggresiveCreature[0] = m_lpTarget;
			}
			break;

		case SECOND_PATTERN:
			{
				// 조건		대상	행동
				// A		B		D	: 스킬 사용 가능 MP 잔여량 있음 / 대상이 스킬 사용 가능 범위에 있을 때			
				//						: 자신에게 가장 위협적인 적에게
				//						: D 스킬 사용

				// 스킬이 없다면.. 다음 패턴으로..
				if (0 == m_MonsterInfo.m_wSkillID[D_SKILL])
				{
					++nSelectPattern;
					continue;
				}

				nSkillPattern = D_SKILL;
				attackType.m_wType = m_MonsterInfo.m_wSkillID[D_SKILL];
				ppAggresiveCreature[0] = m_lpTarget;
			}
			break;

		default:
			{
				ERRLOG1(g_Log, "CID:0x%08x 없는 패턴이 넘어왔습니다.", m_dwCID);
				return false;
			}
		}

		// 스킬 사용 (캐스팅 타입인 경우에는 사용할수 있는지만 체크하게 된다.)
		if (true == UseSkill(attackType, ppAggresiveCreature, nSkillPattern))
		{
			return true;
		}
		else
		{
			++nSelectPattern;
		}
	}

	return false;
}

void	CSiegeObject::UpdateObjectInfo(unsigned char cHPType, unsigned short wRepairHP)
{
	unsigned short wPrevMaxHP = m_CreatureStatus.m_StatusInfo.m_nMaxHP;
	unsigned short wNowHP = m_CreatureStatus.m_nNowHP;
	int nKID = Siege::GetKID(m_wObjectType, m_cState, m_cUpgradeType, m_cUpgradeStep);

	const CMonsterMgr::MonsterProtoType* lpProtoType = CMonsterMgr::GetInstance().GetMonsterProtoType(nKID);
	if (lpProtoType)
	{
		m_CreatureStatus = lpProtoType->m_CreatureStatus;
		m_MonsterInfo = lpProtoType->m_MonsterInfo;

		m_CreatureStatus.m_StatusInfo.CalculateSubStatus();
	}

	unsigned long dwTempHp;
	switch (cHPType)
	{
	case Siege::NOW_HP:
		m_CreatureStatus.m_nNowHP = wNowHP;
		SendHPUpdateToDBAgent();
		break;

	case Siege::UPGRADE_HP:
		dwTempHp = wNowHP + m_CreatureStatus.m_StatusInfo.m_nMaxHP - wPrevMaxHP;
		m_CreatureStatus.m_nNowHP = static_cast<unsigned short>(
            std::min(dwTempHp, static_cast<unsigned long>(m_CreatureStatus.m_StatusInfo.m_nMaxHP)));
		SendHPUpdateToDBAgent(true);
		break;

	case Siege::REPAIR_HP:
		dwTempHp = wNowHP + wRepairHP;
		m_CreatureStatus.m_nNowHP = static_cast<unsigned short>(
            std::min(dwTempHp, static_cast<unsigned long>(m_CreatureStatus.m_StatusInfo.m_nMaxHP)));
		SendHPUpdateToDBAgent(true);
		break;

	case Siege::FULL_HP:
		m_CreatureStatus.m_nNowHP = m_CreatureStatus.m_StatusInfo.m_nMaxHP;
		SendHPUpdateToDBAgent(true);
		break;
	}
}

void	CSiegeObject::UpgradeByEmblem(unsigned char cUpgradeType, unsigned char cUpgradeStep)
{
	// 성 관련 오브젝트로 상징물의 업그레이드 효과를 받는 오브젝트만
	if (!IsCastleObject())
	{
		return;
	}

	int nKID = Siege::GetKID(Siege::EMBLEM, Siege::COMPLETE, cUpgradeType, cUpgradeStep);

	const CMonsterMgr::MonsterProtoType* lpProtoType = CMonsterMgr::GetInstance().GetMonsterProtoType(nKID);
	if (lpProtoType)
	{
		// 업그레이드 속도 향상
		m_MonsterInfo.m_cUpgradeSpeed -= static_cast<unsigned char>(m_MonsterInfo.m_cUpgradeSpeed
			* lpProtoType->m_MonsterInfo.m_fUpgradeSpeedUp);

		// 개발 속도 향상
		m_MonsterInfo.m_cDevelopSpeed -= static_cast<unsigned char>(m_MonsterInfo.m_cDevelopSpeed
			* lpProtoType->m_MonsterInfo.m_fDevelopSpeedUp);

		// 방어력 향상
		m_CreatureStatus.m_StatusInfo.m_wArmor += static_cast<short>(m_CreatureStatus.m_StatusInfo.m_wArmor
			* lpProtoType->m_MonsterInfo.m_fDefenseUp);
		m_CreatureStatus.m_StatusInfo.m_wMagicResist += static_cast<short>(m_CreatureStatus.m_StatusInfo.m_wMagicResist
			* lpProtoType->m_MonsterInfo.m_fDefenseUp);

		// HP 향상
		unsigned short wAddHP = static_cast<unsigned short>(m_CreatureStatus.m_StatusInfo.m_nMaxHP
			* lpProtoType->m_MonsterInfo.m_fHPUp);
		m_CreatureStatus.m_StatusInfo.m_nMaxHP += wAddHP;
		m_CreatureStatus.m_nNowHP += wAddHP;

		// 공격력 향상
		m_CreatureStatus.m_StatusInfo.m_lMinDamage += static_cast<long>(m_CreatureStatus.m_StatusInfo.m_lMinDamage
			* lpProtoType->m_MonsterInfo.m_fOffenseUp);
		m_CreatureStatus.m_StatusInfo.m_lMaxDamage += static_cast<long>(m_CreatureStatus.m_StatusInfo.m_lMaxDamage
			* lpProtoType->m_MonsterInfo.m_fOffenseUp);
		m_CreatureStatus.m_StatusInfo.m_wMagicPower += static_cast<short>(m_CreatureStatus.m_StatusInfo.m_wMagicPower
			* lpProtoType->m_MonsterInfo.m_fOffenseUp);

		// 생성 비용 절감
		m_MonsterInfo.m_dwDevelopGold -= static_cast<unsigned long>(m_MonsterInfo.m_dwDevelopGold
			* lpProtoType->m_MonsterInfo.m_fDevelopGoldDown);

		// 업그레이드 비용 절감
		m_MonsterInfo.m_dwUpgradeGold -= static_cast<unsigned long>(m_MonsterInfo.m_dwUpgradeGold
			* lpProtoType->m_MonsterInfo.m_fUpgradeGoldDown);
	}
}

void	CSiegeObject::DegradeByEmblem(unsigned char cUpgradeType, unsigned char cUpgradeStep)
{
	// 성 관련 오브젝트로 상징물의 업그레이드 효과를 받는 오브젝트만
	if (!IsCastleObject())
	{
		return;
	}

	int nKID = Siege::GetKID(Siege::EMBLEM, Siege::COMPLETE, cUpgradeType, cUpgradeStep);
	const CMonsterMgr::MonsterProtoType* lpProtoTypeEmblem = CMonsterMgr::GetInstance().GetMonsterProtoType(nKID);

	nKID = GetKID();
	const CMonsterMgr::MonsterProtoType* lpProtoType = CMonsterMgr::GetInstance().GetMonsterProtoType(nKID);

	if (lpProtoType && lpProtoTypeEmblem)
	{
		unsigned short wNowHP = m_CreatureStatus.m_nNowHP -
			static_cast<unsigned short>(m_CreatureStatus.m_nNowHP * lpProtoTypeEmblem->m_MonsterInfo.m_fHPUp);

		m_CreatureStatus = lpProtoType->m_CreatureStatus;
		m_MonsterInfo = lpProtoType->m_MonsterInfo;

		m_CreatureStatus.m_nNowHP = wNowHP;
	}
}

unsigned short CSiegeObject::GetDefaultNowHP()
{
	// 성 관련 오브젝트로 상징물의 업그레이드 효과를 받는 오브젝트만
	if (IsCastleObject())
	{
		Castle::CCastle* lpCastle = Castle::CCastleMgr::GetInstance().GetCastle( GetCastleID() );
		CSiegeObject* lpEmblem = lpCastle->GetCastleEmblem();

		if (lpCastle && lpEmblem)
		{
			int nKID = lpEmblem->GetKID();
			const CMonsterMgr::MonsterProtoType* lpProtoTypeEmblem = CMonsterMgr::GetInstance().GetMonsterProtoType(nKID);

			if (lpProtoTypeEmblem)
			{
				return m_CreatureStatus.m_nNowHP -	static_cast<unsigned short>(m_CreatureStatus.m_nNowHP * lpProtoTypeEmblem->m_MonsterInfo.m_fHPUp);
			}
		}
	}

	return m_CreatureStatus.m_nNowHP;
}

void	CSiegeObject::MoveTo(const Position& NewPosition)
{
	CAggresiveCreature::MoveTo(NewPosition, false);
}


unsigned long	CSiegeObject::GetRepairHP()
{
	return m_CreatureStatus.m_StatusInfo.m_nMaxHP - m_CreatureStatus.m_nNowHP;
}


 

int				CSiegeObject::GetKID() const
{
	return Siege::GetKID(m_wObjectType, m_cState, m_cUpgradeType, m_cUpgradeStep);
}

void			CSiegeObject::GetRiders( unsigned long* pRiders ) const
{
	pRiders[0] = m_dwRiderCID;
}

unsigned char	CSiegeObject::GetRiderNum() const
{
	return (0 == m_dwRiderCID) ? 0 : 1;
}

unsigned long	CSiegeObject::GetDevelopGold(unsigned short wDefaultObjectType) const
{
	wDefaultObjectType = (wDefaultObjectType == Siege::DEFAULT_TYPE) ? m_wObjectType : wDefaultObjectType;
    int nKID = Siege::GetKID(wDefaultObjectType, Siege::DEVELOPING, m_cUpgradeType, m_cUpgradeStep);
	const CMonsterMgr::MonsterProtoType* pProtoType = CMonsterMgr::GetInstance().GetMonsterProtoType(nKID);
	
	if ( pProtoType )
	{
		return pProtoType->m_MonsterInfo.m_dwDevelopGold;
	}

	ERRLOG1(g_Log, "개발 비용 오류 ( KID : %d )", nKID);
	return 0;
}

unsigned char	CSiegeObject::GetDevelopSpeed(unsigned short wDefaultObjectType) const
{
	wDefaultObjectType = (wDefaultObjectType == Siege::DEFAULT_TYPE) ? m_wObjectType : wDefaultObjectType;
	int nKID = Siege::GetKID(wDefaultObjectType, Siege::DEVELOPING, m_cUpgradeType, m_cUpgradeStep);
	const CMonsterMgr::MonsterProtoType* pProtoType = CMonsterMgr::GetInstance().GetMonsterProtoType(nKID);

	if ( pProtoType )
	{
		return pProtoType->m_MonsterInfo.m_cDevelopSpeed;
	}

	ERRLOG1(g_Log, "개발 시간 오류 ( KID : %d )", nKID);
	return Siege::DEFAULT_TIME_VALUE;
}

unsigned long	CSiegeObject::GetUpgradeGold() const
{
	int nKID = Siege::GetKID(m_wObjectType, Siege::UPGRADING, m_cUpgradeType, m_cUpgradeStep);
	if ( m_cUpgradeStep < Siege::MAX_UPGRADE_NUM )
	{
		const CMonsterMgr::MonsterProtoType* pProtoType = CMonsterMgr::GetInstance().GetMonsterProtoType(nKID);

		if ( pProtoType )
		{
			return pProtoType->m_MonsterInfo.m_dwUpgradeGold;
		}
	}

	ERRLOG1(g_Log, "업그레이드 비용 오류 ( KID : %d )", nKID);
	return 0;
}

unsigned char	CSiegeObject::GetUpgradeSpeed(unsigned char cDefaultUpgradeType) const
{
	cDefaultUpgradeType = (cDefaultUpgradeType == Siege::NO_JEWEL) ? m_cUpgradeType : cDefaultUpgradeType;
	int nKID = Siege::GetKID(m_wObjectType, Siege::UPGRADING, cDefaultUpgradeType, m_cUpgradeStep);

	if ( m_cUpgradeStep < Siege::MAX_UPGRADE_NUM )
	{
		const CMonsterMgr::MonsterProtoType* pProtoType = CMonsterMgr::GetInstance().GetMonsterProtoType(nKID);

		if ( pProtoType )
		{
			return pProtoType->m_MonsterInfo.m_cUpgradeSpeed;
		}
	}

	ERRLOG1(g_Log, "업그레이드 시간 오류 ( KID : %d )", nKID);
	return Siege::DEFAULT_TIME_VALUE;
}

float			CSiegeObject::GetBonusRate() const
{
	return m_MonsterInfo.m_fBonusRate;
}

unsigned long	CSiegeObject::GetGID() const
{
	// 병기에 타고 있는 캐릭터가 있으면, 병기에 탑승한 캐릭터의 GetGID() 호출
	if ( IsRidable() && 0 != m_dwRiderCID )
	{
		CCharacter* lpRider = CCreatureManager::GetInstance().GetCharacter(m_dwRiderCID);
		if (lpRider)
		{
			return lpRider->GetGID();
		}
	}

    return m_dwGID;
}

unsigned char	CSiegeObject::GetNation() const
{
	// 병기에 타고 있는 캐릭터가 있으면, 병기에 탑승한 캐릭터의 GetNation() 호출
	if ( IsRidable() && 0 != m_dwRiderCID )
	{
		CCharacter* lpRider = CCreatureManager::GetInstance().GetCharacter(m_dwRiderCID);
		if (lpRider)
		{
			return lpRider->GetNation();
		}
	}

	if ( IsCastleObject() || IsSiegeArms() )
	{
		return m_cNation;
	}
	else
	{
		// 해당 Guild 의 GetNation() 호출
		CGuild* lpGuild = CGuildMgr::GetInstance().GetGuild(m_dwGID);
		if (NULL != lpGuild)
		{
			return lpGuild->GetNation();
		}
	}
	
	// 성주가 없는 공성 병기에 대해서 무국적을 리턴한다.
	return Creature::STATELESS;
}

bool		CSiegeObject::IsPeaceMode()
{
	// 병기에 타고 있는 캐릭터가 있으면, 병기에 탑승한 캐릭터의 GetNation() 호출
	if ( IsRidable() && 0 != m_dwRiderCID )
	{
		CCharacter* lpRider = CCreatureManager::GetInstance().GetCharacter(m_dwRiderCID);
		if (lpRider)
		{
			return lpRider->IsPeaceMode();
		}
	}

	if ( IsCastleObject() || IsSiegeArms() )
	{
		return false;
	}
	else
	{
		// 해당 Guild 의 GetNation() 호출
		CGuild* lpGuild = CGuildMgr::GetInstance().GetGuild(m_dwGID);
		if (NULL != lpGuild)
		{
			return lpGuild->IsPeaceMode();
		}
	}

	// 성주가 없는 공성 병기에 대해서 전쟁모드
	return false;
}

CAggresiveCreature*	CSiegeObject::GetDuelOpponent(void) const
{
	// 병기에 타고 있는 캐릭터가 있으면, 병기에 탑승한 캐릭터의 GetNation() 호출
	if ( IsRidable() && 0 != m_dwRiderCID )
	{
		CCharacter* lpRider = CCreatureManager::GetInstance().GetCharacter(m_dwRiderCID);
		if (lpRider)
		{
			return lpRider->GetDuelOpponent();
		}
	}

	return NULL;
}

bool	CSiegeObject::IsWorldWeapon() const
{
	if (Siege::KARTERANT_WEAPON == m_wObjectType || Siege::MERKADIA_WEAPON == m_wObjectType)
	{
		return true;
	}

	return false;
}



/*
bool	CSiegeObject::SerializeOut(char* szBuffer_In, unsigned short& wDataLen)
{
	PktSiegeBroadCast* lpBroadCast = reinterpret_cast<PktSiegeBroadCast*>(szBuffer_In);

	lpBroadCast->m_dwCID		= m_dwCID;
	lpBroadCast->m_dwOwnerID	= m_dwOwnerID;		// 성, 길드 요새, 소유캐릭터 ID
	lpBroadCast->m_dwGID		= m_dwGID;			// 길드 ID
	
	if (IsEmblem() && m_dwEnemyGID)
	{
		lpBroadCast->m_dwGID	= m_dwEnemyGID;		// 길드 ID
	}

	lpBroadCast->m_wObjectType		= m_wObjectType;
	lpBroadCast->m_dwNowHP			= m_CreatureStatus.m_nNowHP;
	lpBroadCast->m_dwMaxHP			= m_CreatureStatus.m_StatusInfo.m_nMaxHP;
	lpBroadCast->m_cState			= m_cState;
	lpBroadCast->m_cSubState		= m_cSubState;
	lpBroadCast->m_cUpgradeStep		= m_cUpgradeStep;
	lpBroadCast->m_cUpgradeType		= m_cUpgradeType;
	lpBroadCast->m_fDefaultDir		= m_fDefaultDir;
	std::fill_n(&lpBroadCast->m_dwRiderID[0], int(Siege::AIRSHIP_RIDER_NUM), 0);
	GetRiders( lpBroadCast->m_dwRiderID );

	float fY = GetCurrentPos().m_fPointY;
    const MotionInfo& motionInfo = GetMotionInfo();

	if (IsSiegeArms())
	{
		if (Siege::AIRSHIP != m_wObjectType)
		{
			fY = 0.0f;
		}
	}
	
	lpBroadCast->m_NetworkPos.Initialize(GetCurrentPos().m_fPointX, fY, GetCurrentPos().m_fPointZ, motionInfo.m_fDirection,
										 (0 == motionInfo.m_dwFrame) ? 0.0f : motionInfo.m_fVelocity / motionInfo.m_dwFrame);
	
	wDataLen = sizeof(PktSiegeBroadCast);
	return true;
}
*/

/*
void	CSiegeObject::SendToRadiusCell(const char* szPacket, unsigned long dwPacketSize, unsigned char cCMD_In)
{
	// 반경 5셀에 패킷 전송
	CCell* lpCenterCell = GetCellPos().m_lpCell;
	if (lpCenterCell)
	{
		int nCellWidth = Siege::BROADCAST_CELL_SIZE * 2 - 1;
		int nCellHeight = Siege::BROADCAST_CELL_SIZE * 2 - 1;
		int i, j;

		CCell* lpStartCell = NULL;
		for (i=1; i<Siege::BROADCAST_CELL_SIZE; ++i)
		{
			lpStartCell = lpCenterCell->GetConnectCell(CCell::LEFT);
			if (NULL == lpStartCell)
			{
				nCellWidth = nCellWidth - (Siege::BROADCAST_CELL_SIZE - i);
				break;
			}

			lpCenterCell = lpStartCell;
		}

		for (i=1; i<Siege::BROADCAST_CELL_SIZE; ++i)
		{
			lpStartCell = lpCenterCell->GetConnectCell(CCell::UP);
			if (NULL == lpStartCell)
			{
				nCellHeight = nCellHeight - (Siege::BROADCAST_CELL_SIZE - i);
				break;
			}

			lpCenterCell = lpStartCell;
		}

		lpStartCell = lpCenterCell;

		for (i=0; i<nCellHeight; ++i)
		{
			CCell* lpTempCell = lpStartCell;
			for (j=0; j<nCellWidth; ++j)
			{
				if (lpTempCell)
				{
					lpTempCell->SendAllCharacter(szPacket, dwPacketSize, cCMD_In);
					lpTempCell = lpTempCell->GetConnectCell(CCell::RIGHT);
				}
				else break;
			}

			lpStartCell = lpStartCell->GetConnectCell(CCell::DOWN);
			if (NULL == lpStartCell) break;
		}
	}
}
*/

void			CSiegeObject::RangeTest()
{
    // 브로드캐스트 반경을 벗어난 캐릭터들을 제거한다. (제거하라고 알려준다.)
	PktNewSiegeBroadCast pktNSBC;
	ZeroMemory(&pktNSBC, sizeof(PktNewSiegeBroadCast));
    pktNSBC.m_dwCID = GetCID();
	pktNSBC.m_cType = BroadCastSiege::BROADCAST_DELETE_DATA;

	bool bDeletePacketCrypt = true;
	char* szPacket = reinterpret_cast<char *>(&pktNSBC);
	if (!PacketWrap::WrapCrypt(szPacket, sizeof(PktNewSiegeBroadCast), CmdNewSiegeBroadCast, 0, 0))
	{
		ERRLOG0(g_Log, "Siege Broadcast delete data packet crypt fail..");
		bDeletePacketCrypt = false;
	}

	BroadCastSet::iterator itr = m_BroadCastSet.begin();
	while (itr != m_BroadCastSet.end())
	{
		CCharacter* lpCharacter = CCreatureManager::GetInstance().GetCharacter( (*itr) );
		if (lpCharacter)
		{
			if (lpCharacter->GetCurrentPos().GetSquaredDistance( GetCurrentPos() ) > Siege::BROADCAST_SQUARED_RADIUS)
			{
				itr = m_BroadCastSet.erase(itr);

				CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
				if (NULL != lpDispatch && bDeletePacketCrypt)
				{
					lpDispatch->GetSendStream().PutBuffer(szPacket, sizeof(PktNewSiegeBroadCast), CmdNewSiegeBroadCast);
				}
			}
			else
			{
				++itr;
			}
		}
		else
		{
			itr = m_BroadCastSet.erase(itr);
		}
	}

	// BroadCast Data 와 Delta Data 를 압축해둔다.
	char szSrcBuffer[ BroadCastSiege::MAX_SIEGEBROADCAST_BUFFER_SIZE ] ;
	char szDeltaSrcBuffer[ BroadCastSiege::MAX_SIEGEBROADCAST_BUFFER_SIZE ] ;
	m_dwBroadCastSize = BroadCastSiege::MAX_SIEGEBROADCAST_BUFFER_SIZE;
	m_dwDeltaSize = 0;

	PktNewSiegeBroadCast* lpPktNewSiegeBC = reinterpret_cast<PktNewSiegeBroadCast*>( szSrcBuffer );
	char* lpDataPos = reinterpret_cast<char*>( lpPktNewSiegeBC + 1 );
	lpPktNewSiegeBC->m_dwCID = GetCID();
	lpPktNewSiegeBC->m_cType = BroadCastSiege::BROADCAST_ALL_DATA;
	memcpy( lpDataPos, m_SerializeSiegeObjectData.GetBroadCastData(), m_SerializeSiegeObjectData.GetBroadCastDataLen() );
	PacketWrap::WrapCompress( m_szBroadCastBuffer, m_dwBroadCastSize, szSrcBuffer,
							  static_cast<unsigned short>( sizeof(PktNewSiegeBroadCast) + m_SerializeSiegeObjectData.GetBroadCastDataLen() ),
							  CmdNewSiegeBroadCast, 0, 0 );

	if ( m_SerializeSiegeObjectData.GetDeltaDataLen() > 0 )
	{
		m_dwDeltaSize = BroadCastSiege::MAX_SIEGEBROADCAST_BUFFER_SIZE;

		lpPktNewSiegeBC = reinterpret_cast<PktNewSiegeBroadCast*>( szDeltaSrcBuffer ) ;
		lpDataPos = reinterpret_cast<char*>( lpPktNewSiegeBC + 1 );
		lpPktNewSiegeBC->m_dwCID = GetCID();
		lpPktNewSiegeBC->m_cType = BroadCastSiege::BROADCAST_DELTA_DATA;
		memcpy( lpDataPos, m_SerializeSiegeObjectData.GetDeltaData(), m_SerializeSiegeObjectData.GetDeltaDataLen() );
		PacketWrap::WrapCompress( m_szDeltaBuffer, m_dwDeltaSize, szDeltaSrcBuffer,
								static_cast<unsigned short>( sizeof(PktNewSiegeBroadCast) + m_SerializeSiegeObjectData.GetDeltaDataLen() ),
								CmdNewSiegeBroadCast, 0, 0 );
	}

	// 범위내에 있는 캐릭터노드들을 검사한다.
	CCharSphereTree::GetInstance().RangeTest( GetCurrentPos(), Siege::BROADCAST_RADIUS, this );
}

void			CSiegeObject::RangeTestCallBack( const Position& centerPos, float fDistance, CCharSphereNode* pNode )
{
	if ( pNode )
	{
		BroadCastSet::iterator itr = m_BroadCastSet.find(pNode->GetCID());
		if (itr != m_BroadCastSet.end())
		{
			CCharacter* lpCharacter = pNode->GetCharacter();
			if (NULL != lpCharacter)
			{
				// 기존에 있던 캐릭터이다. Delta 정보만 보내준다.
				if (m_SerializeSiegeObjectData.GetDeltaDataLen() > 0)
				{
					CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
					if (NULL != lpDispatch)
					{
						lpDispatch->GetSendStream().PutBuffer(m_szDeltaBuffer, m_dwDeltaSize, CmdNewSiegeBroadCast);
					}
				}
			}
		}
		else
		{
			// 새로 추가된 캐릭터이다. BroadCast 로 모든 정보를 보낸다.
			CCharacter* lpCharacter = pNode->GetCharacter();
			if (NULL != lpCharacter)
			{
				CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
				if (NULL != lpDispatch)
				{
					lpDispatch->GetSendStream().PutBuffer(m_szBroadCastBuffer, m_dwBroadCastSize, CmdNewSiegeBroadCast);
				}

				m_BroadCastSet.insert( pNode->GetCID() );
			}
		}
	}
}

void			CSiegeObject::SendToRange( float fRadius, const char* szPacket, unsigned long dwPacketSize, unsigned char cCMD_In )
{
	CCharSphereTree::GetInstance().SendToRange( GetCurrentPos(), fRadius, szPacket, dwPacketSize, cCMD_In );
}

void			CSiegeObject::SendHPUpdateToDBAgent(bool bForce)
{
	++m_cHPUpdateCount;

	// 중계 서버로 패킷 전송
	GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
	if (lpDBAgentDispatch)
	{
		switch (m_wObjectType)
		{
		case Siege::CAMP:						// 길드 요새
		case Siege::CAMP_SHOP:					// 길드 요새 상점
		case Siege::MINING_CAMP:				// 채굴기
		case Siege::KARTERANT_WEAPON:			// 월드 웨폰
		case Siege::MERKADIA_WEAPON:			// 월드 웨폰
			{
				if (bForce || m_cHPUpdateCount >= MAX_HP_UPDATE_COUNT)
				{
					GameClientSendPacket::SendCharCampCmdToDBAgent(lpDBAgentDispatch->GetSendStream(), 0, GetCampID(),
																m_CreatureStatus.m_nNowHP, 0, PktCampCmd::CAMP_UPDATE_HP,
																PktBase::NO_SERVER_ERR);
					m_cHPUpdateCount = 0;
				}
			}
			break;

		case Siege::EMBLEM:						// 성 상징물
		case Siege::GATE:						// 성문
		case Siege::GUARD:						// 가드
		case Siege::SHORT_RANGE_CASTLE_ARMS:	// 근거리 수성 병기
		case Siege::LONG_RANGE_CASTLE_ARMS:		// 원거리 수성 병기
			{
				if (bForce || m_cHPUpdateCount >= MAX_HP_UPDATE_COUNT)
				{
					GameClientSendPacket::SendCharCastleCmdToDBAgent(lpDBAgentDispatch->GetSendStream(), 0, GetCastleID(), m_dwCID,
																	GetDefaultNowHP(), 0, PktCastleCmd::CASTLE_UPDATE_HP,
																	PktBase::NO_SERVER_ERR);
					m_cHPUpdateCount = 0;
				}
			}
			break;

		case Siege::SHORT_RANGE_SIEGE_ARMS:		// 근거리 공성 병기
		case Siege::LONG_RANGE_SIEGE_ARMS:		// 원거리 공성 병기
		case Siege::AIRSHIP:					// 수송선
			{
				if (bForce || m_cHPUpdateCount >= MAX_HP_UPDATE_COUNT)
				{
					GameClientSendPacket::SendCharSiegeArmsCmdToDBAgent(lpDBAgentDispatch->GetSendStream(), m_dwOwnerID, m_dwCID,
																		m_CreatureStatus.m_nNowHP, PktSiegeArmsCmd::SIEGE_UPDATE_HP,
																		PktBase::NO_SERVER_ERR);
					m_cHPUpdateCount = 0;
				}
			}
		}
	}
}

bool	CSiegeObject::IsCamp() const
{
	switch ( m_wObjectType )
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

bool	CSiegeObject::IsCastleObject() const
{
	switch ( m_wObjectType )
	{
		case Siege::EMBLEM :
		case Siege::GATE :					
		case Siege::BACKDOOR :
		case Siege::CASTLE_ARMS_NPC :
		case Siege::GUARD :	
		case Siege::SHORT_RANGE_CASTLE_ARMS :
		case Siege::LONG_RANGE_CASTLE_ARMS :
		{
			return true;
		}
	}

	return false;
}

bool			CSiegeObject::IsCastleArms() const
{
	switch ( m_wObjectType )
	{
		case Siege::GUARD :	
		case Siege::SHORT_RANGE_CASTLE_ARMS :
		case Siege::LONG_RANGE_CASTLE_ARMS :
		case Siege::CASTLE_ARMS_NPC :
		{
			return true;
		}
	}

	return false;
}

bool			CSiegeObject::IsSiegeArms() const
{
	switch ( m_wObjectType )
	{
		case Siege::SHORT_RANGE_SIEGE_ARMS :
		case Siege::LONG_RANGE_SIEGE_ARMS :
		case Siege::AIRSHIP :
		{
			return true;
		}
	}

	return false;
}


bool			CSiegeObject::IsRidable() const
{
	switch ( m_wObjectType )
	{
		case Siege::SHORT_RANGE_CASTLE_ARMS :
		case Siege::LONG_RANGE_CASTLE_ARMS :
		case Siege::SHORT_RANGE_SIEGE_ARMS :
		case Siege::LONG_RANGE_SIEGE_ARMS :
		case Siege::AIRSHIP :
		{
			return true;
		}
	}

	return false;
}

void			CSiegeObject::SetMaterialNum(unsigned char cMaterial)
{
	m_cMaterial = cMaterial;
}

void			CSiegeObject::SetNation(unsigned char cNation)
{
	m_cNation = cNation;

	if (IsEmblem() && Creature::STATELESS == cNation)
	{
		m_cState = Siege::COMPLETE;
		m_cSubState = Siege::NONE;
	}
}