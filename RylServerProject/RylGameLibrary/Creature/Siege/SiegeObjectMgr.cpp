#include "stdafx.h"
#include "SiegeObjectMgr.h"

#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/WrapPacket.h>
#include <network/packet/packetstruct/serverinfo.h>

#include <Network/Dispatch/DBAgent/DBAgentDispatch.h>

#include <Creature/Monster/Monster.h>
#include <Creature/Siege/SiegeObject.h>
#include <Creature/Siege/Camp.h>
#include <Creature/Siege/MiningCamp.h>
#include <Creature/Siege/CampShop.h>
#include <Creature/Siege/WorldWeapon.h>
#include <Creature/Siege/CastleEmblem.h>
#include <Creature/Siege/CastleGate.h>
#include <Creature/Siege/CastleArms.h>
#include <Creature/Siege/CastleGuard.h>
#include <Creature/Siege/SiegeArms.h>
#include <Creature/Siege/Airship.h>
#include <Creature/Character/CharRespawnMgr.h>

#include <Community/Guild/Guild.h>
#include <Community/Guild/GuildMgr.h>

#include <Map/FieldMap/MineralVeinMgr.h>


CSiegeObjectMgr&	CSiegeObjectMgr::GetInstance()
{
	static CSiegeObjectMgr	ms_this;
	return ms_this;
}

CSiegeObjectMgr::CSiegeObjectMgr()
: m_lpWorldWeapon(NULL), m_cKarRemainSiegeTime(0), m_cMerRemainSiegeTime(0), m_bRequestWorldWeaponCreation(false)
{
}

CSiegeObjectMgr::~CSiegeObjectMgr()
{
	Destroy();
}

void			CSiegeObjectMgr::Destroy()
{
	DestroyAllCamp();

	if (0 == m_SiegeObjectMap.size()) return;

	SiegeObjectMap::iterator itr = m_SiegeObjectMap.begin();
	SiegeObjectMap::iterator end = m_SiegeObjectMap.end();
	CSiegeObject* lpSiegeObject = NULL;

	while (itr != end)
	{
		lpSiegeObject = itr->second;
		if (lpSiegeObject)
		{
			// 객체 삭제
			delete lpSiegeObject;
			lpSiegeObject = NULL;
		}

		++itr;
	}

	m_SiegeObjectMap.clear();
}

void			CSiegeObjectMgr::DestroyAllCamp()
{
	if (0 == m_CampMap.size()) return;

	CampMap::iterator itr = m_CampMap.begin();
	CampMap::iterator end = m_CampMap.end();
	CCamp* lpCamp = NULL;

	while (itr != end)
	{
		lpCamp = itr->second;
		if (lpCamp)
		{
			// CreatureManager 에서 삭제
			CCreatureManager::GetInstance().DeleteCreature(lpCamp->GetCID());

			// SiegeObjectMap 에서 삭제
			SiegeObjectMap::iterator pos = m_SiegeObjectMap.find(lpCamp->GetCID());
			if (pos != m_SiegeObjectMap.end())
			{
				m_SiegeObjectMap.erase(pos);
			}

            // 객체 삭제
			delete lpCamp;
			lpCamp = NULL;
		}

		++itr;
	}

	m_CampMap.clear();
}

CSiegeObject*	CSiegeObjectMgr::CreateCastleObject(CastleObjectInfo& CastleObject)
{
	int nKID = Siege::GetKID(CastleObject.m_wObjectType, CastleObject.m_cState,
		CastleObject.m_cUpgradeType, CastleObject.m_cUpgradeStep);

	CMonster::MonsterCreateInfo MonsterCreate;
	MonsterCreate.m_dwCID			= CastleObject.m_dwCID;
	MonsterCreate.m_dwPID			= 0x00000000;
	MonsterCreate.m_nKID			= nKID;
	MonsterCreate.m_nMovingPattern	= CMonster::FIXED;
	MonsterCreate.m_wRespawnArea	= 0;
	MonsterCreate.m_bScout			= false;
	MonsterCreate.m_Pos				= CastleObject.m_Pos;

	// 타입에 맞는 객체를 생성한다.
	CSiegeObject* lpCastleObject = NULL;

    switch (CastleObject.m_wObjectType)
	{
	case Siege::EMBLEM :
		lpCastleObject = new CCastleEmblem(MonsterCreate, CastleObject);
		break;

	case Siege::GATE :
		lpCastleObject = new CCastleGate(MonsterCreate, CastleObject);
		break;

	case Siege::CASTLE_ARMS_NPC :
	case Siege::SHORT_RANGE_CASTLE_ARMS :
	case Siege::LONG_RANGE_CASTLE_ARMS :
		lpCastleObject = new CCastleArms(MonsterCreate, CastleObject);
		break;

	case Siege::GUARD :
		lpCastleObject = new CCastleGuard(MonsterCreate, CastleObject);
		break;

	default :
		lpCastleObject = new CSiegeObject(MonsterCreate, CastleObject);
		break;
	}

	if (lpCastleObject)
	{
		if ( !m_SiegeObjectMap.insert(std::make_pair(lpCastleObject->GetCID(), lpCastleObject)).second )
		{
			DeleteSiegeObject(lpCastleObject->GetCID());
			return NULL;
		}

		// CreatureManager 에 등록
		CCreatureManager::GetInstance().AddCreature(lpCastleObject);

		return lpCastleObject;
	}

	return NULL;
}

CSiegeObject*	CSiegeObjectMgr::CreateCamp(unsigned long dwCID, unsigned long dwCampID, unsigned long dwGID, unsigned long dwHP,
											unsigned char cState, unsigned char cUpgradeStep, const CampRight& campRight,
											Position Pos, unsigned char cMaterial, bool bFullHP)
{
	int nKID = Siege::GetKID(Siege::CAMP, cState, 0, cUpgradeStep);

	CMonster::MonsterCreateInfo MonsterCreate;
	MonsterCreate.m_dwCID			= dwCID;
	MonsterCreate.m_dwPID			= 0x00000000;
	MonsterCreate.m_nKID			= nKID;
	MonsterCreate.m_nMovingPattern	= CMonster::FIXED;
	MonsterCreate.m_wRespawnArea	= 0;
	MonsterCreate.m_bScout			= false;
	MonsterCreate.m_Pos				= Pos;

	CSiegeObject* lpCamp = new CCamp(MonsterCreate, dwCampID, dwGID, dwHP, Siege::CAMP, cState, 0, cUpgradeStep, cMaterial, 0, campRight, bFullHP);

	if (lpCamp)
	{
		if ( !m_SiegeObjectMap.insert(std::make_pair(lpCamp->GetCID(), lpCamp)).second )
		{
			DeleteSiegeObject(lpCamp->GetCID());
			return NULL;
		}

		if ( !m_CampMap.insert(std::make_pair(lpCamp->GetCampID(), reinterpret_cast<CCamp*>(lpCamp))).second )
		{
			// 에러 로그
		}

		if (bFullHP)
		{
			lpCamp->SendHPUpdateToDBAgent(true);
		}

		// CreatureManager 에 등록
		CCreatureManager::GetInstance().AddCreature(lpCamp);

		// 리스폰 포인트 추가 (구축중이거나, 구축 취소중에는 안됨)
		if (Siege::DEVELOPING != cState && Siege::CANCELING != cState)
		{
			CCharRespawnMgr::GetInstance().AddCampRespawnPoint(lpCamp->GetCampID(), lpCamp->GetGID(), lpCamp->GetPosition());
		}

		return lpCamp;
	}

	return NULL;
}

CSiegeObject*	CSiegeObjectMgr::CreateWorldWeapon(unsigned long dwCID, unsigned long dwCampID, unsigned long dwGID,	unsigned long dwHP,
													unsigned char cState, unsigned char cUpgradeStep, const CampRight& campRight,
													Position Pos, unsigned char cMaterial, bool bFullHP, unsigned short wObjectType)
{
	int nKID = Siege::GetKID(wObjectType, cState, 0, cUpgradeStep);

	CMonster::MonsterCreateInfo MonsterCreate;
	MonsterCreate.m_dwCID			= dwCID;
	MonsterCreate.m_dwPID			= 0x00000000;
	MonsterCreate.m_nKID			= nKID;
	MonsterCreate.m_nMovingPattern	= CMonster::FIXED;
	MonsterCreate.m_wRespawnArea	= 0;
	MonsterCreate.m_bScout			= false;
	MonsterCreate.m_Pos				= Pos;

	CSiegeObject* lpCamp = new CWorldWeapon(MonsterCreate, dwCampID, dwGID, dwHP, wObjectType, cState, 0, cUpgradeStep, cMaterial, 0, campRight, bFullHP);

	if (lpCamp)
	{
		if ( !m_SiegeObjectMap.insert(std::make_pair(lpCamp->GetCID(), lpCamp)).second )
		{
			DeleteSiegeObject(lpCamp->GetCID());
			return NULL;
		}

		if ( !m_CampMap.insert(std::make_pair(lpCamp->GetCampID(), reinterpret_cast<CCamp*>(lpCamp))).second )
		{
			// 에러 로그
		}

		if (bFullHP)
		{
			lpCamp->SendHPUpdateToDBAgent(true);
		}

		// CreatureManager 에 등록
		CCreatureManager::GetInstance().AddCreature(lpCamp);

		// 리스폰 포인트 추가 (구축중이거나, 구축 취소중에는 안됨)
		if (Siege::DEVELOPING != cState && Siege::CANCELING != cState)
		{
			CCharRespawnMgr::GetInstance().AddCampRespawnPoint(lpCamp->GetCampID(), lpCamp->GetGID(), lpCamp->GetPosition());
		}

		unsigned char cNation = Creature::STATELESS;
		switch (wObjectType)
		{
			case Siege::KARTERANT_WEAPON:	cNation = Creature::KARTERANT;	break;
			case Siege::MERKADIA_WEAPON:	cNation	= Creature::MERKADIA;	break;
		}

		// WorldWeapon 국가 설정
		if (NULL == m_lpWorldWeapon)
		{
			m_lpWorldWeapon = reinterpret_cast<CCamp*>(lpCamp);
		}
		else
		{
			ERRLOG2(g_Log, "월드 웨폰이 2개 이상 존재합니다. OldNation : %d,  NewNation : %d", m_lpWorldWeapon->GetNation(), cNation);
		}

		return lpCamp;
	}

	return NULL;
}

CSiegeObject*	CSiegeObjectMgr::CreateSiegeArms(unsigned long dwCID, unsigned long dwOwnerID, unsigned char cNation, unsigned long dwHP,
												 unsigned short wObjectType, unsigned char cState, unsigned char cUpgradeStep,
												 Position Pos)
{
	if (m_SiegeObjectMap.end() != m_SiegeObjectMap.find(dwCID))
	{
		ERRLOG1(g_Log, "CID:0x%08x 이미 존재하는 공성 병기를 생성하려 합니다.", dwCID);
		return NULL;
	}

	int nKID = Siege::GetKID(wObjectType, cState, 0, cUpgradeStep);

	CMonster::MonsterCreateInfo MonsterCreate;
	MonsterCreate.m_dwCID			= dwCID;
	MonsterCreate.m_dwPID			= 0x00000000;
	MonsterCreate.m_nKID			= nKID;
	MonsterCreate.m_nMovingPattern	= CMonster::NONAREA;
	MonsterCreate.m_wRespawnArea	= 0;
	MonsterCreate.m_bScout			= false;
	MonsterCreate.m_Pos				= Pos;

	// 타입에 맞는 객체를 생성한다.
	CSiegeObject* lpSiegeArms = NULL;

	switch (wObjectType)
	{
	case Siege::SHORT_RANGE_SIEGE_ARMS :
	case Siege::LONG_RANGE_SIEGE_ARMS :
		lpSiegeArms = new CSiegeArms(MonsterCreate, dwOwnerID, cNation, dwHP, wObjectType, cState, cUpgradeStep);
		break;

	case Siege::AIRSHIP :
		lpSiegeArms = new CAirship(MonsterCreate, dwOwnerID, cNation, dwHP, wObjectType, cState, cUpgradeStep);
		break;
	}

	if (lpSiegeArms)
	{
		if ( !m_SiegeObjectMap.insert(std::make_pair(lpSiegeArms->GetCID(), lpSiegeArms)).second )
		{
			DeleteSiegeObject(lpSiegeArms->GetCID());
			return NULL;
		}

		if (0 == dwHP)
		{
			lpSiegeArms->SendHPUpdateToDBAgent(true);
		}

		// CreatureManager 에 등록
		CCreatureManager::GetInstance().AddCreature(lpSiegeArms);

		return lpSiegeArms;
	}

	return NULL;
}

CSiegeObject*	CSiegeObjectMgr::ChangeCampType(unsigned long dwCampID, unsigned short wChangeType)
{
	CCamp* lpCamp = GetCamp( dwCampID );
	if ( lpCamp )
	{
		CampInfoDB campInfoDB;
		unsigned long dwCID			= lpCamp->GetCID();
		campInfoDB.m_dwCampID		= lpCamp->GetCampID();
		campInfoDB.m_dwGID			= lpCamp->GetGID();
		campInfoDB.m_dwHP			= lpCamp->GetNowHP();
		campInfoDB.m_cZone			= CServerSetup::GetInstance().GetServerZone();
		campInfoDB.m_cChannel		= CServerSetup::GetInstance().GetServerChannel();
		campInfoDB.m_wObjectType	= wChangeType;
		campInfoDB.m_cState			= Siege::COMPLETE;
		campInfoDB.m_cSubState		= Siege::NONE;
		campInfoDB.m_cUpgradeStep	= lpCamp->GetUpgradeStep();
		::memset(&campInfoDB.m_TimeValue, 0, sizeof(TIME));
		::memset(&campInfoDB.m_LastUseTime, 0, sizeof(TIME));
		::memcpy(campInfoDB.m_szRight, lpCamp->GetCampRight().m_aryCampRight, CampRight::MAX_RIGHT_NUM);
		campInfoDB.m_Pos.fPointX	= lpCamp->GetCurrentPos().m_fPointX;
		campInfoDB.m_Pos.fPointY	= lpCamp->GetCurrentPos().m_fPointY;
		campInfoDB.m_Pos.fPointZ	= lpCamp->GetCurrentPos().m_fPointZ;
		campInfoDB.m_cMaterial		= lpCamp->GetMaterialNum();
		campInfoDB.m_cSiegeCount	= 0;

		// 기존 객체 삭제
		DeleteCamp( dwCampID );

		// 새로운 객체로 새로 추가
		CSiegeObject* lpSiegeObject = NULL;
		switch ( wChangeType )
		{
			case Siege::CAMP:			lpSiegeObject = CreateCamp(reinterpret_cast<char*>(&campInfoDB), 0, dwCID);			break;
			case Siege::MINING_CAMP:	lpSiegeObject = CreateMiningCamp(reinterpret_cast<char*>(&campInfoDB), 0, dwCID);	break;
			case Siege::CAMP_SHOP:		lpSiegeObject = CreateCampShop(reinterpret_cast<char*>(&campInfoDB), 0, dwCID);		break;
		}

		// 리스트에 추가
		if ( lpSiegeObject )
		{
			if ( !m_SiegeObjectMap.insert(std::make_pair(lpSiegeObject->GetCID(), lpSiegeObject)).second )
			{
				DeleteSiegeObject(lpSiegeObject->GetCID());
				return NULL;
			}

			if ( !m_CampMap.insert(std::make_pair(lpSiegeObject->GetCampID(), reinterpret_cast<CCamp*>(lpSiegeObject))).second )
			{
				// 에러 로그
			}

			// CreatureManager 에 등록
			CCreatureManager::GetInstance().AddCreature(lpSiegeObject);
			return lpSiegeObject;
		}

		return NULL;
	}

	return NULL;
}

CCamp*	CSiegeObjectMgr::GetCamp(unsigned long dwCampID)
{
	if (0 == m_CampMap.size()) return NULL;

	CampMap::iterator pos = m_CampMap.find(dwCampID);
	if (pos != m_CampMap.end()) return (pos->second);

	return NULL;
}

CSiegeObject*	CSiegeObjectMgr::GetSiegeObject(unsigned long dwObjectID)
{
	if (0 == m_SiegeObjectMap.size()) return NULL;

	SiegeObjectMap::iterator pos = m_SiegeObjectMap.find(dwObjectID);
	if (pos != m_SiegeObjectMap.end()) return (pos->second);

	return NULL;
}

void	CSiegeObjectMgr::SetWorldWeaponInfo(unsigned char cKarRemainSiegeTime, unsigned char cMerRemainSiegeTime)
{
	m_cKarRemainSiegeTime = cKarRemainSiegeTime;
	m_cMerRemainSiegeTime = cMerRemainSiegeTime;
}

void	CSiegeObjectMgr::UpdateWorldWeaponInfo(unsigned char cNation, unsigned char cRemainSiegeTime)
{
	switch (cNation)
	{
		case Creature::KARTERANT:
		{
			m_cKarRemainSiegeTime = cRemainSiegeTime;
		}
		break;

		case Creature::MERKADIA:
		{
			m_cMerRemainSiegeTime = cRemainSiegeTime;
		}
		break;
	}
}

bool	CSiegeObjectMgr::EnableCreateWorldWeapon(unsigned char cNation, unsigned short& wError) const
{
	// 월드 웨폰 구성. 12번 글로벌에서 빼졌다.
	if (SERVER_ID::CAPITAL != CServerSetup::GetInstance().GetServerZone())
	{
		wError = PktUI::FAIL_CAPITAL_ZONE;
		return false;
	}

/*
	// 월드 웨폰 구성. 12번 글로벌에서 빼졌다.
	if (SERVER_ID::ZONE12 == CServerSetup::GetInstance().GetServerZone())
	{
		wError = PktUI::FAIL_ALMIGHTY_GROUND;
		return false;
	}
*/

/*
	// 다크 카나번도 월드 웨폰을 구축할 수 있도록 룰이 변경되었습니다. (2005-06-03 by 로딘)
	if (SERVER_ID::STONE_WAR == CServerSetup::GetInstance().GetServerZone())
	{
		wError = PktUI::FAIL_STONE_BATTLE;
		return false;
	}
*/

	if (Creature::ALMIGHTY_PIRATE == cNation)
	{
		wError = PktUI::FAIL_ALMIGHTY_PIRATE;
		return true;
	}

	if (NULL == m_lpWorldWeapon && false == m_bRequestWorldWeaponCreation)
	{
		switch (cNation)
		{
			case Creature::KARTERANT:
			{
				if (0 == m_cKarRemainSiegeTime)
				{
					return true;
				}
				else
				{
					switch (m_cKarRemainSiegeTime)
					{
						case 1:		wError = PktUI::FAIL_REMAIN_1_DAY;		break;
						case 2:		wError = PktUI::FAIL_REMAIN_2_DAY;		break;
						case 3:		wError = PktUI::FAIL_REMAIN_3_DAY;		break;
						default:	wError = PktUI::FAIL_REMAIN_3_DAY;		break;
					}

					return false;
				}
			}
			break;

			case Creature::MERKADIA:
			{
				if (0 == m_cMerRemainSiegeTime)
				{
					return true;
				}
				else
				{
					switch (m_cMerRemainSiegeTime)
					{
						case 1:		wError = PktUI::FAIL_REMAIN_1_DAY;		break;
						case 2:		wError = PktUI::FAIL_REMAIN_2_DAY;		break;
						case 3:		wError = PktUI::FAIL_REMAIN_3_DAY;		break;
						default:	wError = PktUI::FAIL_REMAIN_3_DAY;		break;
					}

					return false;
				}
			}
			break;
		}
	}

	wError = PktUI::FAIL_EXIST_ANOTHER_WEAPON;
	return false;
}

void	CSiegeObjectMgr::DecreaseWeaponRemainSiegeTime()
{
	if (m_cKarRemainSiegeTime > 0)
	{
		--m_cKarRemainSiegeTime;
	}

	if (m_cMerRemainSiegeTime > 0)
	{
		--m_cMerRemainSiegeTime;
	}
}

bool	CSiegeObjectMgr::DeleteCamp(unsigned long dwCampID)
{
	CampMap::iterator itr = m_CampMap.find(dwCampID);

	if (itr != m_CampMap.end())
	{
		CCamp* lpCamp = itr->second;
		if (lpCamp)
		{
			// 월드 웨폰 지우기
			if (Siege::KARTERANT_WEAPON == lpCamp->GetObjectType() ||
				Siege::MERKADIA_WEAPON == lpCamp->GetObjectType())
			{
				m_lpWorldWeapon = NULL;
			}

			// SiegeObjectMap 에서 삭제
			SiegeObjectMap::iterator pos = m_SiegeObjectMap.find(lpCamp->GetCID());
			if (pos != m_SiegeObjectMap.end())
			{
				m_SiegeObjectMap.erase(pos);
			}

			// CreatureManager 에서 삭제
			CCreatureManager::GetInstance().DeleteCreature(lpCamp->GetCID());

			// 객체 삭제
			delete lpCamp;
			lpCamp = NULL;
		}

		m_CampMap.erase(itr);
	}

	return true;
}

bool	CSiegeObjectMgr::DeleteSiegeObject(unsigned long dwCID)
{
	SiegeObjectMap::iterator itr = m_SiegeObjectMap.find(dwCID);

	if (itr != m_SiegeObjectMap.end())
	{
		CSiegeObject* lpObject = itr->second;
		if (lpObject)
		{
			if (lpObject->IsCamp())
			{
				return DeleteCamp( lpObject->GetCampID() );
			}

			// 객체 삭제
			delete lpObject;
			lpObject = NULL;
		}

		m_SiegeObjectMap.erase(itr);
	}

	return true;
}

bool	CSiegeObjectMgr::DeleteSiegeObject(CSiegeObject* lpObject)
{
	if (lpObject == NULL) return false;
	return DeleteSiegeObject(lpObject->GetCID());
}

bool	CSiegeObjectMgr::ExistBuildingOrDestroyingCamp(unsigned long dwGID)
{
	CampMap::iterator itr = m_CampMap.begin();
	while (itr != m_CampMap.end())
	{
		CCamp* lpCamp = itr->second;
		if (lpCamp)
		{
			if (lpCamp->GetGID() == dwGID)
			{
				switch (lpCamp->GetState())
				{
					case Siege::DEVELOPING :
					case Siege::CANCELING :
					case Siege::DESTROYING :
						return true;
				}
			}
		}

		++itr;
	}

	return false;
}

bool	CSiegeObjectMgr::ExistCampInRadius(const Position& Pos)
{
	CampMap::iterator itr = m_CampMap.begin();
	while (itr != m_CampMap.end())
	{
		CCamp* lpCamp = itr->second;
		if (lpCamp)
		{
			const float fDX = Pos.m_fPointX - lpCamp->GetPosition().m_fPointX;
			const float fDZ = Pos.m_fPointZ - lpCamp->GetPosition().m_fPointZ;
			const float fDistance = sqrtf( (fDX * fDX) + (fDZ * fDZ) );

			if (fDistance <= Siege::CAMP_BUILDING_RADIUS)
			{
				return true;
			}
		}

		++itr;
	}

	return false;
}

bool	CSiegeObjectMgr::ExistCompleteCampInRadius(const Position& Pos, unsigned long dwGID)
{
	CampMap::iterator itr = m_CampMap.begin();
	while (itr != m_CampMap.end())
	{
		CCamp* lpCamp = itr->second;
		if (lpCamp)
		{
			const float fDX = Pos.m_fPointX - lpCamp->GetPosition().m_fPointX;
			const float fDZ = Pos.m_fPointZ - lpCamp->GetPosition().m_fPointZ;
			const float fDistance = sqrtf( (fDX * fDX) + (fDZ * fDZ) );

			if (fDistance <= Siege::CAMP_BUILDING_RADIUS && lpCamp->GetGID() == dwGID)
			{
				switch (lpCamp->GetState())
				{
					case Siege::COMPLETE:
					case Siege::UPGRADING:
					case Siege::REPARING:
					{
						return true;
					}
					break;
				}
			}
		}

		++itr;
	}

	return false;
}

void	CSiegeObjectMgr::HasCampByGID(unsigned long dwGID, bool& bHasDevelopingCamp, bool& bHasCompleteCamp)
{
	bHasDevelopingCamp = bHasCompleteCamp = false;

	CampMap::iterator itr = m_CampMap.begin();
	while (itr != m_CampMap.end())
	{
		CCamp* lpCamp = itr->second;
		if (lpCamp && lpCamp->GetGID() == dwGID)
		{
			if (Siege::DEVELOPING == lpCamp->GetState())
			{
				// 건설중인 길드 요새
				bHasDevelopingCamp = true;
			}
			else
			{
				// 완성된 길드 요새
				bHasCompleteCamp = true;
			}
		}

		++itr;
	}
}

void	CSiegeObjectMgr::ProcessAllSiegeObject()
{
	SiegeObjectMap::iterator itr = m_SiegeObjectMap.begin();
	while (itr != m_SiegeObjectMap.end())
	{
		CSiegeObject* lpSiegeObject = itr->second;
		if (lpSiegeObject)
		{
			if (lpSiegeObject->IsGuard() || lpSiegeObject->IsEmblem() || lpSiegeObject->IsCamp() )
			{
				lpSiegeObject->Process();
			}
		}

		++itr;
	}
}

void	CSiegeObjectMgr::ProcessCampRegenHPAndMP()
{
	SiegeObjectMap::iterator itr = m_SiegeObjectMap.begin();
	while (itr != m_SiegeObjectMap.end())
	{
		CSiegeObject* lpSiegeObject = itr->second;
		if (lpSiegeObject && lpSiegeObject->IsCamp())
		{
			lpSiegeObject->RegenHPAndMP(0, 0, true);
			lpSiegeObject->SendHPUpdateToDBAgent();
		}

		++itr;
	}
}


void	CSiegeObjectMgr::ProcessWorldWeaponFire()
{
	// 1 초에 한번 호출된다.
	if (NULL != m_lpWorldWeapon && Siege::WEAPON_FIRE == m_lpWorldWeapon->GetSubState())
	{
		CWorldWeapon* lpWeapon = reinterpret_cast<CWorldWeapon* >(m_lpWorldWeapon);
		if (NULL != lpWeapon)
		{
			if (0 == lpWeapon->GetFireCount())
			{
				// 발사 처리
				lpWeapon->Fire();

				DeleteCamp(lpWeapon->GetCampID());
			}
			else
			{
				// 몇초 남았다는 메세지 전송
				lpWeapon->FireWarning();
			}
		}
	}
}

void			CSiegeObjectMgr::ProcessCampShopUpdate(bool bForce)
{
	CampMap::iterator itr = m_CampMap.begin();
	CampMap::iterator end = m_CampMap.end();

	while (itr != end)
	{
		CCamp* lpCamp = itr->second;
		if (NULL != lpCamp && Siege::CAMP_SHOP == lpCamp->GetObjectType())
		{
			CCampShop* lpCampShop = reinterpret_cast<CCampShop* >(lpCamp);
			lpCampShop->DBUpdate(bForce);
		}

		++itr;
	}
}

void			CSiegeObjectMgr::ProcessMiningCamp(unsigned long dwProcessType)
{
	// 사전 준비 작업 : 채굴중인 채굴기의 숫자를 갱신해준다.
	CMineralVeinMgr::GetInstance().InitializeMiningCampNum();

	CampMap::iterator itr = m_CampMap.begin();
	CampMap::iterator end = m_CampMap.end();

	while ( itr != end )
	{
		CCamp* lpCamp = itr->second;
		if ( lpCamp && Siege::MINING_CAMP == lpCamp->GetObjectType() )
		{
			CMiningCamp* lpMiningCamp = reinterpret_cast<CMiningCamp* >( lpCamp );
			if ( lpMiningCamp && Siege::MINING_ON == lpMiningCamp->GetSubState() )
			{
				// 해당 지역의 채굴기 수 업데이트
				CMineralVeinMgr::GetInstance().CalculateMiningCampNum( lpCamp->GetCurrentPos().m_fPointX, lpCamp->GetCurrentPos().m_fPointZ );
			}
		}

		++itr;
	}

	// 1. 가동중이던 채굴기의 임시 광물에 새로 추가해준다.
	// 2. 지정된 공성이 지난 채굴기의 임시 광물을 누적 광물로 옮겨준다.
	itr = m_CampMap.begin();
	end = m_CampMap.end();

	while ( itr != end )
	{
		CCamp* lpCamp = itr->second;
		if ( lpCamp && Siege::MINING_CAMP == lpCamp->GetObjectType() )
		{
			CMiningCamp* lpMiningCamp = reinterpret_cast<CMiningCamp* >( lpCamp );
			if ( lpMiningCamp && Siege::MINING_ON == lpMiningCamp->GetSubState() )
			{
				lpMiningCamp->ProcessMining(dwProcessType);
			}
		}

		++itr;
	}

	// 3. 지력을 갱신해준다.
	CMineralVeinMgr::GetInstance().CalculateFertility(dwProcessType);

	GET_SINGLE_DISPATCH(lpDBAgentDispatch, CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());
	if (0 == lpDBAgentDispatch)
	{
		ERRLOG0(g_Log, "에이전트 얻기 실패.");	
	}
	else
	{
		char szBuffer[ 2048 ];

		unsigned short wSize = 0, wTotalSize = 0;
		PktFertilityInfo* lpPktFI = reinterpret_cast<PktFertilityInfo* >(szBuffer);
		ZoneChannelInfo* lpZoneChannel = reinterpret_cast<ZoneChannelInfo* >(lpPktFI + 1);
		unsigned long* lpData = reinterpret_cast<unsigned long*>(lpZoneChannel + 1);

		lpPktFI->m_wNum = static_cast<unsigned short>( CMineralVeinMgr::GetInstance().GetVeinNum() );
		CMineralVeinMgr::GetInstance().SerializeOutFertility(lpData, wSize);
		lpPktFI->m_wSize = wSize;

		lpZoneChannel->m_cZone = CServerSetup::GetInstance().GetServerZone();
		lpZoneChannel->m_cChannel = CServerSetup::GetInstance().GetServerChannel();

		wTotalSize = sizeof(PktFertilityInfo) + sizeof(ZoneChannelInfo) + wSize;
		
		CSendStream& SendStream = lpDBAgentDispatch->GetSendStream();
		SendStream.WrapCompress(szBuffer, wTotalSize, CmdFertilityInfo, 0, 0);
	}
}


bool			CSiegeObjectMgr::SerializeIn(char* lpBuffer, unsigned short wSize, unsigned long dwCID)
{
	if ( !lpBuffer )
	{
		return false;
	}

	CSiegeObject* lpCamp = NULL;
	CampInfoDB* lpCampInfoDB = reinterpret_cast<CampInfoDB*>(lpBuffer);
	switch (lpCampInfoDB->m_wObjectType)
	{
		case Siege::CAMP:				lpCamp = CreateCamp(lpBuffer, wSize, dwCID);								break;
		case Siege::MINING_CAMP:		lpCamp = CreateMiningCamp(lpBuffer, wSize, dwCID);							break;
		case Siege::CAMP_SHOP:			lpCamp = CreateCampShop(lpBuffer, wSize, dwCID);							break;
		case Siege::KARTERANT_WEAPON:	lpCamp = CreateWorldWeapon(lpBuffer, wSize, dwCID, Creature::KARTERANT);	break;
		case Siege::MERKADIA_WEAPON:	lpCamp = CreateWorldWeapon(lpBuffer, wSize, dwCID, Creature::MERKADIA);		break;
	}

	if ( lpCamp )
	{
		if ( !m_SiegeObjectMap.insert(std::make_pair(lpCamp->GetCID(), lpCamp)).second )
		{
			DeleteSiegeObject(lpCamp->GetCID());
			return NULL;
		}

		if ( !m_CampMap.insert(std::make_pair(lpCamp->GetCampID(), reinterpret_cast<CCamp*>(lpCamp))).second )
		{
			// 에러 로그
		}

		// CreatureManager 에 등록
		CCreatureManager::GetInstance().AddCreature(lpCamp);

		// 리스폰 포인트 추가 (구축중이거나, 구축 취소중에는 안됨)
		if (Siege::DEVELOPING != lpCamp->GetState() && Siege::CANCELING != lpCamp->GetState())
		{
			CCharRespawnMgr::GetInstance().AddCampRespawnPoint(lpCamp->GetCampID(), lpCamp->GetGID(), lpCamp->GetPosition());
		}

		return true;
	}

	return false;
}

bool			CSiegeObjectMgr::SendCampInfo(CSendStream& SendStream)
{
	CampMap::iterator itr = m_CampMap.begin();
	CampMap::iterator end = m_CampMap.end();

	while (itr != end)
	{
		CCamp* lpCamp = itr->second;

		if (lpCamp)
		{
			char* lpBuffer = SendStream.GetBuffer(sizeof(PktCreateCamp));
			if (lpBuffer)
			{
				PktCreateCamp* lpPktCreateCamp = reinterpret_cast<PktCreateCamp*>(lpBuffer);

				lpPktCreateCamp->m_dwActorCID		= 0;
				lpPktCreateCamp->m_dwCID			= lpCamp->m_dwCID;
				lpPktCreateCamp->m_dwCampID			= lpCamp->GetCampID();
				lpPktCreateCamp->m_dwGID			= lpCamp->m_dwGID;
				lpPktCreateCamp->m_dwHP				= lpCamp->m_CreatureStatus.m_nNowHP;
				lpPktCreateCamp->m_wObjectType		= lpCamp->GetObjectType();
				lpPktCreateCamp->m_cZone			= CServerSetup::GetInstance().GetServerZone();
				lpPktCreateCamp->m_cChannel			= CServerSetup::GetInstance().GetServerChannel();
				lpPktCreateCamp->m_cState			= lpCamp->m_cState;
				lpPktCreateCamp->m_cSubState		= lpCamp->m_cSubState;
				lpPktCreateCamp->m_cUpgradeStep		= lpCamp->m_cUpgradeStep;
				lpPktCreateCamp->m_CampRight		= lpCamp->m_CampRight;
				lpPktCreateCamp->m_Pos.fPointX		= lpCamp->m_CurrentPos.m_fPointX;
				lpPktCreateCamp->m_Pos.fPointY		= lpCamp->m_CurrentPos.m_fPointY;
				lpPktCreateCamp->m_Pos.fPointZ		= lpCamp->m_CurrentPos.m_fPointZ;
				lpPktCreateCamp->m_cMaterial		= lpCamp->GetMaterialNum();

				Guild::CGuild* lpGuild = Guild::CGuildMgr::GetInstance().GetGuild( lpCamp->m_dwGID );
				if (lpGuild)
				{
					lpPktCreateCamp->m_cNation		= lpGuild->GetNation();
				}
				else
				{
					// 길드 정보가 없을 때 제삼국적으로 임시 사용
					lpPktCreateCamp->m_cNation		= Creature::ALMIGHTY_PIRATE;
				}

				SendStream.WrapCrypt(sizeof(PktCreateCamp), CmdCreateCamp, 0, PktBase::NO_SERVER_ERR);
			}
		}

		++itr;
	}

	return true;
}


// 해당 성의 모든 오브젝트의 주인이 바뀌었다는것을 병기의 반경 5셀에 있는 캐릭터에게 알려준다.
bool			CSiegeObjectMgr::SendChangeMaster(unsigned long dwCastleID, unsigned char cNation)
{
	SiegeObjectMap::iterator itr = m_SiegeObjectMap.begin();
	while (itr != m_SiegeObjectMap.end())
	{
		CSiegeObject* lpCastleObject = itr->second;
		if (lpCastleObject && lpCastleObject->GetCastleID() == dwCastleID)
		{
			lpCastleObject->SetNation(cNation);

			if (lpCastleObject->IsEmblem())
			{
				lpCastleObject->SetState(Siege::COMPLETE);
			}

			PktCastleCmd pktCC;
			pktCC.m_dwCastleID			= dwCastleID;
			pktCC.m_dwCastleObjectID	= lpCastleObject->GetCID();
			pktCC.m_cState				= lpCastleObject->GetState();
			pktCC.m_dwValue1			= cNation;		// 새로운 성의 소유 국가
			pktCC.m_dwValue2			= 0;
			pktCC.m_cSubCmd				= PktCastleCmd::CASTLE_ARMS_CHANGE_MASTER;

			char* szPacket = reinterpret_cast<char *>(&pktCC);
			if (PacketWrap::WrapCrypt(szPacket, sizeof(PktCastleCmd), CmdCastleCmd, 0, 0))
			{
				// 반경 5셀에 패킷 전송
				// Vincent - 브로드 캐스트 변경 테스트 코드
				//lpCastleObject->SendToRadiusCell(szPacket, sizeof(PktCastleCmd), CmdCastleCmd);
				lpCastleObject->SendToRange(Siege::BROADCAST_RADIUS, szPacket, sizeof(PktCastleCmd), CmdCastleCmd);
			}
		}

		++itr;
	}

	return true;
}

bool			CSiegeObjectMgr::SendLoseOwnership(unsigned long dwCastleID)
{
	SiegeObjectMap::iterator itr = m_SiegeObjectMap.begin();
	while (itr != m_SiegeObjectMap.end())
	{
		CSiegeObject* lpCastleObject = itr->second;
		if (lpCastleObject && lpCastleObject->GetCastleID() == dwCastleID)
		{
			lpCastleObject->SetNation(0);

			if (lpCastleObject->IsEmblem())
			{
				lpCastleObject->SetState(Siege::COMPLETE);
			}

			PktCastleCmd pktCC;
			pktCC.m_dwCastleID			= dwCastleID;
			pktCC.m_dwCastleObjectID	= lpCastleObject->GetCID();
			pktCC.m_cState				= lpCastleObject->GetState();
			pktCC.m_dwValue1			= Creature::STATELESS;				// 새로운 성의 소유국가 (0)
			pktCC.m_dwValue2			= lpCastleObject->GetSubState();	// SubState
			pktCC.m_cSubCmd				= PktCastleCmd::CASTLE_ARMS_CHANGE_MASTER;

			char* szPacket = reinterpret_cast<char *>(&pktCC);
			if (PacketWrap::WrapCrypt(szPacket, sizeof(PktCastleCmd), CmdCastleCmd, 0, 0))
			{
				// 반경 5셀에 패킷 전송
				// Vincent - 브로드 캐스트 변경 테스트 코드
				//lpCastleObject->SendToRadiusCell(szPacket, sizeof(PktCastleCmd), CmdCastleCmd);
				lpCastleObject->SendToRange(Siege::BROADCAST_RADIUS, szPacket, sizeof(PktCastleCmd), CmdCastleCmd);
			}
		}

		++itr;
	}

	return true;
}

CSiegeObject*	CSiegeObjectMgr::CreateCamp(char* lpBuffer, unsigned short wSize, unsigned long dwCID)
{
	CampInfoDB* lpCampInfoDB = reinterpret_cast<CampInfoDB*>(lpBuffer);

	int nKID = Siege::GetKID(lpCampInfoDB->m_wObjectType, lpCampInfoDB->m_cState, 0, lpCampInfoDB->m_cUpgradeStep);

	CMonster::MonsterCreateInfo MonsterCreate;
	MonsterCreate.m_dwCID			= dwCID;
	MonsterCreate.m_dwPID			= 0x00000000;
	MonsterCreate.m_nKID			= nKID;
	MonsterCreate.m_nMovingPattern	= CMonster::FIXED;
	MonsterCreate.m_wRespawnArea	= 0;
	MonsterCreate.m_bScout			= false;
	MonsterCreate.m_Pos				= lpCampInfoDB->m_Pos;

	CampRight campRight;
	::memcpy(campRight.m_aryCampRight, lpCampInfoDB->m_szRight, CampRight::MAX_RIGHT_NUM);

	CSiegeObject* lpCamp = new CCamp(MonsterCreate, lpCampInfoDB->m_dwCampID, lpCampInfoDB->m_dwGID, lpCampInfoDB->m_dwHP,
									lpCampInfoDB->m_wObjectType, lpCampInfoDB->m_cState, lpCampInfoDB->m_cSubState,
									lpCampInfoDB->m_cUpgradeStep, lpCampInfoDB->m_cMaterial, lpCampInfoDB->m_cSiegeCount,
									campRight, false);
	
	return lpCamp;
}

CSiegeObject*	CSiegeObjectMgr::CreateMiningCamp(char* lpBuffer, unsigned short wSize, unsigned long dwCID)
{
	CampInfoDB* lpCampInfoDB = reinterpret_cast<CampInfoDB*>(lpBuffer);

	int nKID = Siege::GetKID(lpCampInfoDB->m_wObjectType, lpCampInfoDB->m_cState, 0, lpCampInfoDB->m_cUpgradeStep);

	CMonster::MonsterCreateInfo MonsterCreate;
	MonsterCreate.m_dwCID			= dwCID;
	MonsterCreate.m_dwPID			= 0x00000000;
	MonsterCreate.m_nKID			= nKID;
	MonsterCreate.m_nMovingPattern	= CMonster::FIXED;
	MonsterCreate.m_wRespawnArea	= 0;
	MonsterCreate.m_bScout			= false;
	MonsterCreate.m_Pos				= lpCampInfoDB->m_Pos;

	CampRight campRight;
	::memcpy(campRight.m_aryCampRight, lpCampInfoDB->m_szRight, CampRight::MAX_RIGHT_NUM);

	CSiegeObject* lpCamp = new CMiningCamp(MonsterCreate, lpCampInfoDB->m_dwCampID, lpCampInfoDB->m_dwGID, lpCampInfoDB->m_dwHP,
										lpCampInfoDB->m_wObjectType, lpCampInfoDB->m_cState, lpCampInfoDB->m_cSubState,
										lpCampInfoDB->m_cUpgradeStep, lpCampInfoDB->m_cMaterial, lpCampInfoDB->m_cSiegeCount,
										campRight, false);

	if ( lpCamp && wSize > 0 )
	{
		CMiningCamp* lpMiningCamp = reinterpret_cast<CMiningCamp*>(lpCamp);
		MiningCampInfo* lpMiningCampInfo = reinterpret_cast<MiningCampInfo*>(lpCampInfoDB + 1);

		if ( lpMiningCampInfo )
		{
			unsigned char cAccumulatedNum = lpMiningCampInfo->m_cAccumulatedNum;
			unsigned char cTemporaryNum = lpMiningCampInfo->m_cTemporaryNum;

			CampMineralInfo* lpMineralInfo = reinterpret_cast<CampMineralInfo*>(lpMiningCampInfo + 1);

			// 누적 광물 정보 셋팅
			for (int i=0; i<cAccumulatedNum; ++i, ++lpMineralInfo)
			{
				lpMiningCamp->AddMineral(Siege::ACCUMULATED_MINERAL, *lpMineralInfo);
			}

			// 임시 광물 정보 셋팅
			for (int i=0; i<cTemporaryNum; ++i, ++lpMineralInfo)
			{
				lpMiningCamp->AddMineral(Siege::TEMPORARY_MINERAL, *lpMineralInfo);
			}
		}
	}

	return lpCamp;
}

CSiegeObject*	CSiegeObjectMgr::CreateCampShop(char* lpBuffer, unsigned short wSize, unsigned long dwCID)
{
	CampInfoDB* lpCampInfoDB = reinterpret_cast<CampInfoDB*>(lpBuffer);

	int nKID = Siege::GetKID(lpCampInfoDB->m_wObjectType, lpCampInfoDB->m_cState, 0, lpCampInfoDB->m_cUpgradeStep);

	CMonster::MonsterCreateInfo MonsterCreate;
	MonsterCreate.m_dwCID			= dwCID;
	MonsterCreate.m_dwPID			= 0x00000000;
	MonsterCreate.m_nKID			= nKID;
	MonsterCreate.m_nMovingPattern	= CMonster::FIXED;
	MonsterCreate.m_wRespawnArea	= 0;
	MonsterCreate.m_bScout			= false;
	MonsterCreate.m_Pos				= lpCampInfoDB->m_Pos;

	CampRight campRight;
	::memcpy(campRight.m_aryCampRight, lpCampInfoDB->m_szRight, CampRight::MAX_RIGHT_NUM);

	CSiegeObject* lpCamp = new CCampShop(MonsterCreate, lpCampInfoDB->m_dwCampID, lpCampInfoDB->m_dwGID, lpCampInfoDB->m_dwHP,
										lpCampInfoDB->m_wObjectType, lpCampInfoDB->m_cState, lpCampInfoDB->m_cSubState,
										lpCampInfoDB->m_cUpgradeStep, lpCampInfoDB->m_cMaterial, lpCampInfoDB->m_cSiegeCount,
										campRight, false);

	if (NULL != lpCamp && wSize > 0)
	{
		CCampShop* lpCampShop = reinterpret_cast<CCampShop*>(lpCamp);
		CampShopInfo* lpCampShopInfo = reinterpret_cast<CampShopInfo*>(lpCampInfoDB + 1);
		char* lpItemBuffer = reinterpret_cast<char*>(lpCampShopInfo + 1);

		if (NULL != lpCampShopInfo)
		{
			lpCampShop->SerializeIn(lpCampShopInfo->m_dwTempSafe, lpCampShopInfo->m_cTax, 
				lpItemBuffer, lpCampShopInfo->m_dwBufferSize, lpCampShopInfo->m_cItemNum);
		}
	}

	return lpCamp;
}

CSiegeObject*	CSiegeObjectMgr::CreateWorldWeapon(char* lpBuffer, unsigned short wSize, unsigned long dwCID, unsigned char cNation)
{
	// WorldWeapon 국가 설정
	if (NULL == m_lpWorldWeapon)
	{
		CampInfoDB* lpCampInfoDB = reinterpret_cast<CampInfoDB*>(lpBuffer);

		int nKID = Siege::GetKID(lpCampInfoDB->m_wObjectType, lpCampInfoDB->m_cState, 0, lpCampInfoDB->m_cUpgradeStep);

		CMonster::MonsterCreateInfo MonsterCreate;
		MonsterCreate.m_dwCID			= dwCID;
		MonsterCreate.m_dwPID			= 0x00000000;
		MonsterCreate.m_nKID			= nKID;
		MonsterCreate.m_nMovingPattern	= CMonster::FIXED;
		MonsterCreate.m_wRespawnArea	= 0;
		MonsterCreate.m_bScout			= false;
		MonsterCreate.m_Pos				= lpCampInfoDB->m_Pos;

		CampRight campRight;
		::memcpy(campRight.m_aryCampRight, lpCampInfoDB->m_szRight, CampRight::MAX_RIGHT_NUM);

		CSiegeObject* lpCamp = new CWorldWeapon(MonsterCreate, lpCampInfoDB->m_dwCampID, lpCampInfoDB->m_dwGID, lpCampInfoDB->m_dwHP,
												lpCampInfoDB->m_wObjectType, lpCampInfoDB->m_cState, lpCampInfoDB->m_cSubState,
												lpCampInfoDB->m_cUpgradeStep, lpCampInfoDB->m_cMaterial, lpCampInfoDB->m_cSiegeCount,
												campRight, false);

		if (lpCamp && Siege::DEVELOPING != lpCamp->GetState())
		{
			m_lpWorldWeapon = reinterpret_cast<CCamp*>(lpCamp);

			// 월드 웨폰 인챈트 적용
			unsigned char cNation = Creature::STATELESS;
			switch (lpCampInfoDB->m_wObjectType)
			{
				case Siege::KARTERANT_WEAPON:	cNation = Creature::KARTERANT;	break;
				case Siege::MERKADIA_WEAPON:	cNation = Creature::MERKADIA;	break;
			}

			CCreatureManager::GetInstance().AddWorldWeaponEnchant(reinterpret_cast<CAggresiveCreature*>(lpCamp), cNation);
		}

		return lpCamp;
	}
	else
	{
		ERRLOG2(g_Log, "월드 웨폰이 2개 이상 존재합니다. OldNation : %d,  NewNation : %d", m_lpWorldWeapon->GetNation(), cNation);
	}
	
	return NULL;
}


// 모든 공성관련 오브젝트들을 반경 5셀에 있는 캐릭터들에게 알려준다.
void CSiegeObjectMgr::PrepareBroadCast()
{
	SiegeObjectMap::iterator itr = m_SiegeObjectMap.begin();
	while (itr != m_SiegeObjectMap.end())
	{
		CSiegeObject* lpSiegeObject = itr->second;
		if (lpSiegeObject)
		{
			lpSiegeObject->GetSerializeData().PrepareData( *lpSiegeObject );
		}

		++itr;
	}
}

void CSiegeObjectMgr::BroadCast()
{
	SiegeObjectMap::iterator itr = m_SiegeObjectMap.begin();
	while (itr != m_SiegeObjectMap.end())
	{
		CSiegeObject* lpSiegeObject = itr->second;
		if (lpSiegeObject)
		{
			lpSiegeObject->RangeTest();
		}

		++itr;
	}
}

bool CSiegeObjectMgr::HasSiegeArms(unsigned long dwCID)
{
	SiegeObjectMap::iterator itr = m_SiegeObjectMap.begin();
	
	while (itr != m_SiegeObjectMap.end())
	{
		CSiegeObject* lpSiegeObject = itr->second;
		
		if (lpSiegeObject)
		{
			if(lpSiegeObject->IsSiegeArms())
			{
				if(lpSiegeObject->GetOwnerID()==dwCID)
				{
					return true;
				}
			}
		}

		++itr;
	}

	return false;
}

unsigned long CSiegeObjectMgr::GetSiegeObjectCount()
{
	unsigned long dwCnt = 0;

	SiegeObjectMap::iterator itr = m_SiegeObjectMap.begin();

	while (itr != m_SiegeObjectMap.end())
	{
		CSiegeObject* lpSiegeObject = itr->second;

		if (lpSiegeObject)
		{
			if(lpSiegeObject->IsSiegeArms())
			{
				dwCnt++;
			}
		}

		++itr;
	}

	return dwCnt;
}