
#include "stdafx.h"

#include <Utility/Math/Math.h>
#include <Utility/Setup/ServerSetup.h>

#include <Network/Dispatch/GameClient/GameClientDispatch.h>
#include <Network/Dispatch/GameClient/SendCharEtc.h>
#include <Network/Dispatch/GameClient/SendCharLevelUp.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>
#include <Network/Packet/PacketCommand.h>

#include <Community/Party/Party.h>
#include <Skill/Spell/Spell.h>

#include <Creature/Character/CharacterClass.h>
#include <Creature/Character/Character.h>
#include <Creature/Character/ExpTable.h>
#include <Creature/Monster/Monster.h>
#include <Creature/Monster/PatternMonster.h>
#include <Creature/Monster/AwardTable.h>
#include <Creature/Monster/VirtualMonsterMgr.h>

#include "AggresiveCreature.h"
#include "CreatureManager.h"
#include "Threat.h"

#include <Map/FieldMap/VirtualArea/VirtualArea.h>
#include <Map/FieldMap/VirtualArea/VirtualAreaMgr.h>

#include <Creature/Siege/SiegeConstants.h>
#include <Creature/Siege/SiegeObject.h>



/*struct CompareAmount 
{
    int operator() (const CThreat::ThreatInfo& ThreatInfo1, const CThreat::ThreatInfo& ThreatInfo2)
    {
        return ThreatInfo1.m_lThreatAmount < ThreatInfo2.m_lThreatAmount;
    }
};

struct CompareLevel 
{
	int operator() (const CThreat::ThreatInfo& ThreatInfo1, const CThreat::ThreatInfo& ThreatInfo2)
    {
        return ThreatInfo1.m_pCreature->GetStatus().m_nLevel < ThreatInfo2.m_pCreature->GetStatus().m_nLevel;
    }
};

struct CompareFame
{
	int operator() (const CThreat::ThreatInfo& ThreatInfo1, const CThreat::ThreatInfo& ThreatInfo2)
	{
		return ThreatInfo1.m_pCreature->GetFame() < ThreatInfo2.m_pCreature->GetFame();
	}
};


CThreat::CThreat(void)
:	m_pOwner(NULL)
{
	ClearAll();
}

CThreat::~CThreat(void)
{
}


void CThreat::ClearAll(void)	
{
	m_cMaxLevel = 0;
	m_eMaxLevelCreatureType = Creature::CT_NONE_TYPE;

	ClearThreatList();
	ClearThreatenedList();
}

void CThreat::ClearThreatList(void)
{
	for (ThreatList::iterator ThreatIt = m_ThreatList.begin(), ThreatEnd = m_ThreatList.end();
		ThreatIt != ThreatEnd; ++ThreatIt) 
	{
		ThreatIt->m_pCreature->GetThreat().DeleteThreatened(m_pOwner);
	}
	m_ThreatList.clear();
}

void CThreat::ClearThreatenedList(void)
{
	for (ThreatenedList::iterator ThreatenedIt = m_ThreatenedList.begin(), ThreatenedEnd = m_ThreatenedList.end();
		ThreatenedIt != ThreatenedEnd; ++ThreatenedIt)
	{
		(*ThreatenedIt)->GetThreat().DeleteThreat(m_pOwner);
	}
	m_ThreatenedList.clear();
}


///////////////////////////////////////////////////////////////////////////////////
// Function : CThreat::AddToThreatList
//
// Description : 쓰레트 리스트에 크리쳐 추가
//
// Inputs : pAttackCreature - 추가할 크리쳐의 포인터
//			lThreatAmount - 쓰레트량
//
// Outputs : None.
//
// Returns : None.
///////////////////////////////////////////////////////////////////////////////////
void CThreat::AddToThreatList(CAggresiveCreature* pAttackCreature, long lThreatAmount)
{
	if (NULL == pAttackCreature || 0 == lThreatAmount)
    {
        return;
    }
	if(!m_pOwner)//hz added this;
		return;

	if (EnemyCheck::EC_ENEMY != m_pOwner->IsEnemy(pAttackCreature))
	{
		return;
	}

	// 이미 리스트에 있는 녀석의 처리
	for (ThreatList::iterator it = m_ThreatList.begin(); it != m_ThreatList.end(); ++it)
	{
		if (pAttackCreature == it->m_pCreature)
		{
			it->m_lThreatAmount += lThreatAmount;
			if (0 >= it->m_lThreatAmount) 
			{
				pAttackCreature->GetThreat().DeleteThreatened(m_pOwner);
				DeleteThreat(pAttackCreature);
			}

			m_ThreatList.sort(CompareAmount());
			return;
		}
	}

	// 처음 리스트에 들어온 녀석의 처리
	if (0 > lThreatAmount) 
    {
        return;
    }

	size_t nThreatListSize = m_ThreatList.size();
	if (nThreatListSize > MAX_THREAT_TARGET)
	{
		ERRLOG2(g_Log, "CID:0x%08x 쓰레트 리스트 사이즈가 최대치를 넘어섰습니다. Size:%d", 
			m_pOwner->GetCID(), nThreatListSize);

        return;
	}
	else if (nThreatListSize == MAX_THREAT_TARGET)
	{
		ThreatInfo& MinThreatInfo = m_ThreatList.front();
		if (lThreatAmount < MinThreatInfo.m_lThreatAmount)
        {
            return;
        }

		MinThreatInfo.m_pCreature->GetThreat().DeleteThreatened(m_pOwner);
		m_ThreatList.pop_front();
	}

	if (pAttackCreature->GetStatus().m_nLevel > m_cMaxLevel)
	{
		m_cMaxLevel = pAttackCreature->GetStatus().m_nLevel;
		m_eMaxLevelCreatureType = Creature::GetCreatureType(pAttackCreature->GetCID());
	}

	pAttackCreature->GetThreat().AddToThreatenedList(m_pOwner);
	m_ThreatList.push_front(ThreatInfo(pAttackCreature, lThreatAmount));
	m_ThreatList.sort(CompareAmount());
}


///////////////////////////////////////////////////////////////////////////////////
// Function : CThreat::AddToThreatenedList
//
// Description : 상호참조를 위한 리스트에 크리쳐 추가
//
// Inputs : pDefendCreature - 추가할 크리쳐의 포인터
//
// Outputs : None.
//
// Returns : None.
///////////////////////////////////////////////////////////////////////////////////
void CThreat::AddToThreatenedList(CAggresiveCreature* pDefendCreature)
{
	ThreatenedList::iterator FindIt = std::find(m_ThreatenedList.begin(), m_ThreatenedList.end(), pDefendCreature);
	if (FindIt == m_ThreatenedList.end())
    {
        // ThreatenedList에 없을 때만 삽입.
        m_ThreatenedList.push_back(pDefendCreature);
    }
}


///////////////////////////////////////////////////////////////////////////////////
// Function : CThreat::HealThreat
//
// Description : 'Heal Threat'의 적용
//
// Inputs : pHealCreature - 도움을 준 크리쳐
//			lThreatAmount - 도움량
//
// Outputs : None.
//
// Returns : None.
///////////////////////////////////////////////////////////////////////////////////

class CAddToThreatList
{
public:
    CAddToThreatList(CAggresiveCreature* lpAggresiveCreature, long lThreatAmount)
        : m_lpAdd(lpAggresiveCreature), m_nAmount(lThreatAmount) { }

    void operator () (CAggresiveCreature* lpAdded) 
    { if (NULL != lpAdded) { lpAdded->GetThreat().AddToThreatList(m_lpAdd, m_nAmount); } }

private:
    CAggresiveCreature* m_lpAdd;
    int                 m_nAmount;
};


void CThreat::HealThreat(CAggresiveCreature* pHealCreature, long lThreatAmount)
{
    const int               MAX_HEAL_THREATEN_NUM = 30;
    CAggresiveCreature*     lpHealThreaten[MAX_HEAL_THREATEN_NUM];

    CAggresiveCreature**    lpDestination = lpHealThreaten;
    unsigned long           nCount = 0;

    for (ThreatenedList::iterator first = m_ThreatenedList.begin(), last = m_ThreatenedList.end();
        first != last && nCount < MAX_HEAL_THREATEN_NUM; ++lpDestination, ++nCount, ++first)
    {
		*lpDestination = *first;
    }
	
    std::for_each(lpHealThreaten, lpDestination, CAddToThreatList(pHealCreature, lThreatAmount));
}


///////////////////////////////////////////////////////////////////////////////////
// Function : CThreat::AffectThreat
//
// Description : 쓰레트값에 영향을 주는 타운트, 디타운트에 관한 계산.
//
// Inputs : pTauntCreature - 타운트한 크리쳐
//			lThreatAmount - 타운트량
//			eType - 타운트/디타운트 구분
//
// Outputs : None.
//
// Returns : None.
///////////////////////////////////////////////////////////////////////////////////
void CThreat::AffectThreat(CAggresiveCreature* pTauntCreature, long lDamage, AffectThreatType eType)
{
	// 결정타는 쓰레트에 영향을 주지 않는다.
	if (m_pOwner->GetStatus().m_nNowHP <= lDamage) 
    {
		return;
	}

	const long lThreatAmount = lDamage * ((eType == TAUNT) ? 1 : (-1));

	unsigned short wThreatNum = 0;
	for (ThreatList::iterator it = m_ThreatList.begin(); it != m_ThreatList.end(); ++it)
	{
        ThreatInfo& threatInfo = *it;

		threatInfo.m_lThreatAmount = (threatInfo.m_lThreatAmount > lThreatAmount) ? 
			(threatInfo.m_lThreatAmount - lThreatAmount) : 0;

		++wThreatNum;
	}

	if (wThreatNum > MAX_THREAT_TARGET || wThreatNum < 0) 
    {
		ERRLOG1(g_Log, "타운트하는 몬스터의 쓰레트 리스트가 이상합니다. 리스트 사이즈 : %d", wThreatNum);
	}

	AddToThreatList(pTauntCreature, lThreatAmount * wThreatNum);
}


///////////////////////////////////////////////////////////////////////////////////
// Function : CThreat::DeleteThreatened
//
// Description : 상호참조를 위한 리스트에서 크리쳐를 삭제
//
// Inputs : pDefendCreature - 삭제할 크리쳐의 포인터
//
// Outputs : None.
//
// Returns : None.
///////////////////////////////////////////////////////////////////////////////////
bool CThreat::DeleteThreatened(CAggresiveCreature* pDefendCreature)
{
	ThreatenedList::iterator FindIt = std::find(m_ThreatenedList.begin(), m_ThreatenedList.end(), pDefendCreature);
	if (FindIt == m_ThreatenedList.end()) 
    {
        return false;
    }

	m_ThreatenedList.erase(FindIt);
	return true;
}


///////////////////////////////////////////////////////////////////////////////////
// Function : CThreat::DeleteThreat
//
// Description : 쓰레트 리스트에서 크리쳐를 삭제
//
// Inputs : pAttackCreature - 삭제할 크리쳐의 포인터
//
// Outputs : None.
//
// Returns : None.
///////////////////////////////////////////////////////////////////////////////////
bool CThreat::DeleteThreat(CAggresiveCreature* pAttackCreature)
{
	for (ThreatList::iterator it = m_ThreatList.begin(); it != m_ThreatList.end(); ++it)
    {
		if (pAttackCreature == it->m_pCreature) 
        {
			m_ThreatList.erase(it);
			return true;
		}
	}

	return false;
}


///////////////////////////////////////////////////////////////////////////////////
// Function : CThreat::GetTarget
//
// Description : 쓰레트 리스트를 검색해 타겟을 구한다.
//
// Inputs : None.
//
// Outputs : None.
//
// Returns : CAggresiveCreature* - 타겟 크리쳐
///////////////////////////////////////////////////////////////////////////////////
CAggresiveCreature* CThreat::GetTarget(void) 
{
	if (true == m_ThreatList.empty())
    { 
		return NULL; 
	}

	Creature::CreatureType eCreatureType = Creature::GetCreatureType(m_pOwner->GetCID());

	if (Creature::CT_MONSTER == eCreatureType ||
		Creature::CT_SUMMON == eCreatureType ||
		Creature::CT_STRUCT == eCreatureType)
	{
		CMonster* lpMonster = reinterpret_cast<CMonster *>(m_pOwner);

		ThreatList TargetThreatList;
		ThreatInfo TempTargetInfo;

		for (ThreatList::iterator it = m_ThreatList.begin(); it != m_ThreatList.end(); ++it)
		{
			TempTargetInfo = *it;
			if (NULL == TempTargetInfo.m_pCreature) 
			{
				continue;
			}
			if (true == TempTargetInfo.m_pCreature->GetEnchantInfo().GetFlag(Skill::SpellID::Stealth)) 
			{
				continue;
			}
			if (EnemyCheck::EC_ENEMY != m_pOwner->IsEnemy(TempTargetInfo.m_pCreature))
			{
				continue;
			}

			const float fAttackRange = lpMonster->GetAttackRange() / 100.0f;

			MotionInfo Motion;
			lpMonster->GetMotion(MonsterInfo::Z3D_CA_WALK, Motion);
			const float fMovingAttackRange = fAttackRange + Motion.m_fVelocity;

			const float fDX = TempTargetInfo.m_pCreature->GetCurrentPos().m_fPointX - m_pOwner->GetCurrentPos().m_fPointX;
			const float fDZ = TempTargetInfo.m_pCreature->GetCurrentPos().m_fPointZ - m_pOwner->GetCurrentPos().m_fPointZ;
			const float fDistance = sqrtf((fDX * fDX) + (fDZ * fDZ));

			if (fDistance < fAttackRange) 
			{
				TempTargetInfo.m_lThreatAmount *= ATTACK_TAUNT_RATE;
			}
			else if (fDistance < fMovingAttackRange)
			{
				TempTargetInfo.m_lThreatAmount *= MOVING_ATTACK_TAUNT_RATE;
			}

			TargetThreatList.push_back(TempTargetInfo);
		}

		if (true == TargetThreatList.empty())
		{
			return NULL;
		}

		TargetThreatList.sort(CompareAmount());
		return TargetThreatList.back().m_pCreature;
		
	}
	else if (Creature::CT_SIEGE_OBJECT == eCreatureType)
	{
		CSiegeObject* lpSiegeObject = reinterpret_cast<CSiegeObject*>(m_pOwner);

		if (!lpSiegeObject)
		{
			ERRLOG1(g_Log, "CID:0x%08x 공격 대상을 찾는 공성 오브젝트가 존재하지 않습니다.", m_pOwner->GetCID());
			return NULL;
		}

		if (!lpSiegeObject->IsGuard() && !lpSiegeObject->IsEmblem() && !lpSiegeObject->IsCamp())
		{
			ERRLOG2(g_Log, "CID:0x%08x wObjectType:%d 공격 대상을 찾는 공성 오브젝트가 가드, 상징물, 요새가 아닙니다.",
					m_pOwner->GetCID(), lpSiegeObject->GetObjectType());
			return NULL;
		}
		
		ThreatList TargetThreatList;
		ThreatInfo TempTargetInfo;

		for (ThreatList::iterator it = m_ThreatList.begin(); it != m_ThreatList.end(); ++it)
		{
			TempTargetInfo = *it;
			if (NULL == TempTargetInfo.m_pCreature) 
			{
				continue;
			}
			if (true == TempTargetInfo.m_pCreature->GetEnchantInfo().GetFlag(Skill::SpellID::Stealth)) 
			{
				continue;
			}
			if (EnemyCheck::EC_ENEMY != m_pOwner->IsEnemy(TempTargetInfo.m_pCreature))
			{
				continue;
			}

			TargetThreatList.push_back(TempTargetInfo);
		}

		if (true == TargetThreatList.empty())
		{
			return NULL;
		}

		TargetThreatList.sort(CompareAmount());
		return TargetThreatList.back().m_pCreature;
	}

	ERRLOG1(g_Log, "CID:0x%08x 몬스터 혹은 가드, 성 상징물, 요새가 아닌 녀석이 공격 타겟을 찾습니다.", m_pOwner->GetCID());
	return NULL;
}


///////////////////////////////////////////////////////////////////////////////////
// Function : CThreat::GetAggravation
//
// Description : 레벨갭 얻기 (경험치 분배용)
//
// Inputs : pCreature - 비교할 크리쳐
//
// Outputs : None.
//
// Returns : float - 레벨갭
///////////////////////////////////////////////////////////////////////////////////
float CThreat::GetAggravation(CAggresiveCreature* pCreature)
{
	static const float aryAggravation[CAggresiveCreature::AGGRAVATION_NUM] = { 
		0.0f,	0.0f,	0.0f,	0.0f,	0.0f,	0.0f,	0.0f,	0.0f,	0.0f,	0.0f,	
	    0.0f,	0.1f,	0.2f,	0.3f,	0.4f,	0.5f,	0.6f,	0.7f,	0.8f,	0.9f,	
		1.0f, 
		1.05f,	1.1f,	1.15f,	1.2f,	1.25f,	1.3f,	1.35f,	1.4f,	1.45f, 	1.5f,
		1.55f,	1.6f,	1.65f,	1.7f,	1.75f,	1.8f,	1.85f,	1.9f,	1.95f, 	2.0f,
	};

	const int nLevelDifference = m_pOwner->CalculateLevelGap(pCreature);
	return aryAggravation[nLevelDifference + CAggresiveCreature::MAX_LEVEL_GAP];
}


unsigned char CThreat::GetAward(unsigned long* aryItemID, Item::CItem** aryItem, unsigned long* dwOwnerID,bool farm)
{
	if (true == m_ThreatList.empty()) { return 0; }
	if (Creature::CT_MONSTER != Creature::GetCreatureType(m_pOwner->GetCID())) { return 0; }

	if (Creature::CT_PC != m_eMaxLevelCreatureType && Creature::CT_SUMMON != m_eMaxLevelCreatureType)
	{
		// 쓰레트 리스트에 들어있는 크리쳐 중 가장 레벨이 높은 크리쳐가 
		// '플레이어'나 '소환수'인 경우에만 아이템을 드랍한다.
		return 0;
	}

	int nRandPoint = 0;
	int nGradeSpace = 0;
	int nLevelDifferenceForArray = 0;

	// 몬스터의 레벨과 유저의 최대레벨을 비교해 그차이를 구한다.
	const int nLevelDifference = m_pOwner->CalculateLevelGap(m_pOwner->GetStatus().m_nLevel, m_cMaxLevel);
	// 레벨갭+20 을해준다.
	nLevelDifferenceForArray = nLevelDifference + CAggresiveCreature::MAX_LEVEL_GAP;

	ThreatInfo HighestThreatInfo = m_ThreatList.back();

	if (NULL == HighestThreatInfo.m_pCreature)
	{
		ERRLOG0(g_Log, "쓰레트 리스트의 탑에 크리쳐 포인터가 없습니다.");
		return 0;
	}
	
	CMonster*	lpMonster	= reinterpret_cast<CMonster *>(m_pOwner);
	CCharacter*	lpCharacter = NULL;

	float		fPt = 1.0f;
	
	switch (Creature::GetCreatureType(HighestThreatInfo.m_pCreature->GetCID()))
	{
		case Creature::CT_PC:		lpCharacter = reinterpret_cast<CCharacter *>(HighestThreatInfo.m_pCreature);					break;
		case Creature::CT_SUMMON:	lpCharacter = reinterpret_cast<CSummonMonster *>(HighestThreatInfo.m_pCreature)->GetMaster();	break;

		default:					return 0;
	}

	// 퍼센트가 0이면 아무것도 못구한다. 그냥 리턴
	fPt = lpCharacter->GetAwardPer();
	if(fPt == 0.0f)
		return 0;

	bool bPremiumTime = lpCharacter->CheckPremiumTime();

	*dwOwnerID = lpCharacter->GetCID();

	// 루팅 권한을 가진 캐릭터의 파티원 중에, 전투에 참여했고 '행운의 오브'를 사용했다면 드랍률에 영향을 준다.
	bool			bLuckyOrb	= false;	

	if (NULL != lpCharacter->GetParty())
	{
		for (ThreatList::iterator it = m_ThreatList.begin(); it != m_ThreatList.end(); ++it)
		{
			if (it->m_pCreature->GetParty() == lpCharacter->GetParty())
			{
				if (true == it->m_pCreature->GetEnchantInfo().GetFlag(Skill::SpellID::LuckyOrb))
				{
					bLuckyOrb = true;
					break;
				}
			}
		}
	}

	float		fAddDropPt = 0.0f;

	// '행운의 오브' 사용시
	if (true == bLuckyOrb)
	{
		fAddDropPt += 0.3f;
	}
	
	// 프리미엄 서비스
	if(bPremiumTime)
	{
		fAddDropPt += lpCharacter->GetPremiumPt();
	}

	// '부의 석상' 사용시.
	if (true == lpCharacter->GetEnchantInfo().GetFlag(Skill::SpellID::WealthStatue))
	{			
		unsigned short wLevel = lpCharacter->GetEnchantLevel(Skill::SpellID::WealthStatue);
		fAddDropPt += (wLevel * 0.01f);
	}

	// 몬스터의드랍 확율을 더한다.
	unsigned char cItemKindIndex = 0;
	int nTotalRate = 0;
	for (cItemKindIndex = 0; cItemKindIndex < MonsterInfo::MAX_AWARD_KIND; ++cItemKindIndex) 
    {
		// 스크립트에 정의된 아이템별 드랍수치를 모두 합한다.
		nTotalRate += lpMonster->GetDropRate(cItemKindIndex);
	}

	// 할로윈 이벤트용 및 게더 오브젝트용 (게더도 최대 3개까지 떨군다)
	int MonID = lpMonster->GetKID();
	int MonDropMax = 1+Math::Random::ComplexRandom(2);

	bool bAllInOne = false;

	if(3006 <= MonID && MonID <= 3009)
		bAllInOne = true;
	else if(3101 <= MonID && MonID <= 3500)
		bAllInOne = true;	// 게더 오브젝트

	int nDropItemIndex = 0, nDropItemNum = 0;
	for (; nDropItemIndex < AwardTable::MAX_DROP_ITEM + EliteBonus::MAX_BONUS_DROP_ITEM; 
		++nDropItemIndex)
	{
		aryItemID[nDropItemNum] = 0;

		unsigned short wDropRate = 0;
		if (nDropItemIndex < AwardTable::MAX_DROP_ITEM)
		{
			wDropRate = AwardTable::aryItemDropRate[nLevelDifferenceForArray][nDropItemIndex];
		}
		else
		{
			// 서버 엘리트 보너스에 의한 추가 드랍률
			char eliteBonus = lpCharacter->GetEliteBonus();
			if (0 < eliteBonus)
			{
				wDropRate = EliteBonus::usBonusItemDropRate[eliteBonus - 1][nDropItemIndex - AwardTable::MAX_DROP_ITEM];
			}
		}

		// 추가드랍율 추가.
		if(fAddDropPt > 0.0f)
		{
			wDropRate += static_cast<unsigned short>(wDropRate * fAddDropPt);
		}

		wDropRate = (unsigned short)(wDropRate*(CServerSetup::GetInstance().GetDropDefault() / 100.0f)*fPt);

		// fPt를 곱해준 이유는 fPt 값에 따라 드랍율이 달라진다.
		// CServerSetup::GetInstance().GetDropDefault() = 100 임.
		// 
		if (!bAllInOne && Math::Random::ComplexRandom(1000) > wDropRate) 
		{ 
			continue;
		}

		// 만약 총 드랍수치가 10만이면 10만을 가지고 드랍을 시킨다.
		nRandPoint = Math::Random::ComplexRandom(nTotalRate);

		for (cItemKindIndex = 0; cItemKindIndex < MonsterInfo::MAX_AWARD_KIND; ++cItemKindIndex)
		{
			// 드랍하지 않는 아이템종류면 리턴
			int nDropRate = lpMonster->GetDropRate(cItemKindIndex);
			if (0 == nDropRate) { continue; }

			// 아이템을 모두 돈다.
			// 각 아이템의 드랍율을 더해준다.
			nGradeSpace += nDropRate;

			// 랜덤으로 나온 값보다. 더해준 아이템의 드랍수치가 더 높으면
			// 드랍을 한다.
			if (nGradeSpace > nRandPoint)
			{
				aryItemID[nDropItemNum] = AwardTable::CAward::GetInstance().GetAward(
					cItemKindIndex, reinterpret_cast<CMonster *>(m_pOwner), lpCharacter, aryItem + nDropItemNum);

				if (0 == aryItemID[nDropItemNum])
				{
					CMonster* pMon = reinterpret_cast<CMonster *>(m_pOwner);
					
					if(pMon)
						ERRLOG2(g_Log, "떨어질 아이템이 결정되지 않았습니다. 몬스터KID : %d, 종류 : %d", pMon->GetKID(), cItemKindIndex);
				}
				else
				{
					++nDropItemNum;
				}

				// 초기화.
				cItemKindIndex = MonsterInfo::MAX_AWARD_KIND;
				nGradeSpace = 0;
			} 

			// 할로윈 이벤트. 3000~3005 
			// 보석상자 이벤트. 3006~3010
			if(3001 <= MonID && MonID <= 3010)
			{
				// 3006 번부터 3009번까지는 보석상자 
				// 무조건 1개만 떨구게 수정
				if(3006 <= MonID && MonID <= 3010 && nDropItemNum >= 1)
					return nDropItemNum;

				if(MonID == 3003 && nDropItemNum >= 1)
					return nDropItemNum;

				if(nDropItemNum >= MonDropMax)
					return nDropItemNum;
			}

			// 게더 오브젝트
			if(3101 <= MonID && MonID <= 3500)
			{
				if(nDropItemNum >= MonDropMax)
					return nDropItemNum;
			}
		}
	}

	// 할로윈 이벤트. 3000~3005 
	// 보석상자 이벤트. 3006~3010
	if(0 == nDropItemNum && 3001 <= MonID && MonID <= 3010)
	{
		// cItemKindIndex 를 0으로 하는 이유는 잡탬 A를 기본적으로 떨구게 하기 위해서이다.
		cItemKindIndex = 0;
		
		// 실패한 뒤에 잡탬A를 떨어뜨리게 했는데 이 잡템A가 0 즉 없는 아이템이면 
		// 이건 아무것도 떨구지 않는 잡몹이라는뜻.
		// 드랍하지 않는 아이템종류면 리턴
		int nDropRate = lpMonster->GetDropRate(cItemKindIndex);
		if (0 == nDropRate) 
			return 0; 

		// 무조건 드랍하는 이벤트인데 한개도 떨구지 않았으면 
		aryItemID[nDropItemNum] = AwardTable::CAward::GetInstance().GetAward(
			cItemKindIndex, reinterpret_cast<CMonster *>(m_pOwner), lpCharacter, aryItem + nDropItemNum);

		if (0 == aryItemID[nDropItemNum])
		{
			CMonster* pMon = reinterpret_cast<CMonster *>(m_pOwner);
			
			if(pMon)
				ERRLOG2(g_Log, "떨어질 아이템이 결정되지 않았습니다. 몬스터KID : %d, 종류 : %d", pMon->GetKID(), cItemKindIndex);

			return 0;
		}

		++nDropItemNum;
	}


	return nDropItemNum;
}


///////////////////////////////////////////////////////////////////////////////////
// Function : CThreat::DivisionExp
//
// Description : 경험치 분배
//
// Inputs : None.
//
// Outputs : None.
//
// Returns : None.
///////////////////////////////////////////////////////////////////////////////////
/*void CThreat::DivisionExp(void)
{
	if (true == m_ThreatList.empty()) 
	{ 
		ERRLOG0(g_Log, "스레트 리스트가 비어 있습니다. 이 상태에서는 Exp를 나눌 수 없습니다."); 
		return; 
	} 

	if (NULL == m_pOwner) 
	{ 
		ERRLOG0(g_Log, "소유자가 NULL입니다."); 
		return; 
	} 

	unsigned long lSumOfThreatAmount = 0;

    for (ThreatList::iterator it = m_ThreatList.begin(); it != m_ThreatList.end(); ++it)
    {
		lSumOfThreatAmount += it->m_lThreatAmount;
	}

	// 쓰레트 리스트에 있는 녀석들의 개별적인 경험치 계산
	AwardInfo aryIndividualAwardInfo[MAX_THREAT_TARGET];

	CAggresiveCreature* pHighestThreatCreature = m_ThreatList.back().m_pCreature;
	const unsigned char cHighestLevel = pHighestThreatCreature->GetStatus().m_nLevel;

	int nNumber = 0;
	for (ThreatList::iterator it = m_ThreatList.begin(); 
		it != m_ThreatList.end() && nNumber < MAX_THREAT_TARGET; ++it, ++nNumber)
	{
		if (nNumber >= MAX_THREAT_TARGET) 
		{ 
			ERRLOG1(g_Log, "스레트 리스트에서, 들어 있는 멤버의 수가 이상합니다. 멤버의 수는 %d입니다.", nNumber); 
			break;
		}

		const ThreatInfo& tempThreatInfo = *it;
		
		aryIndividualAwardInfo[nNumber].m_pCreature = tempThreatInfo.m_pCreature;
		aryIndividualAwardInfo[nNumber].m_pParty = tempThreatInfo.m_pCreature->GetParty();

		aryIndividualAwardInfo[nNumber].m_lAward = 
			static_cast<long>(m_pOwner->GetStatus().m_nExp * tempThreatInfo.m_lThreatAmount / 
			std::max(static_cast<float>(lSumOfThreatAmount), static_cast<float>(m_pOwner->GetStatus().m_StatusInfo.m_nMaxHP)));
	
		const int nIndividualLevel = aryIndividualAwardInfo[nNumber].m_pCreature->GetStatus().m_nLevel;
		if (::abs(cHighestLevel - nIndividualLevel) > EXP_CONSENT_GAP)
        { 
			aryIndividualAwardInfo[nNumber].m_lAward = 1; 
		} 
	}

	// 소환수의 경험치는 마스터에게 합쳐준다.
	for (int nSummoneeIndex = 0; nSummoneeIndex < nNumber; ++nSummoneeIndex)
	{
		unsigned long dwCID = aryIndividualAwardInfo[nSummoneeIndex].m_pCreature->GetCID();
		CAggresiveCreature* lpMaster = NULL;
		bool bSummonee = false;

		if (0 != aryIndividualAwardInfo[nSummoneeIndex].m_pCreature->GetMapIndex())
		{
			VirtualArea::CVirtualArea* lpVirtualArea = 
				VirtualArea::CVirtualAreaMgr::GetInstance().GetVirtualArea(
				aryIndividualAwardInfo[nSummoneeIndex].m_pCreature->GetMapIndex());

			if (NULL != lpVirtualArea)
			{
				CVirtualMonsterMgr* lpVirtualMonsterMgr = lpVirtualArea->GetMonsterManager();
				if (NULL != lpVirtualMonsterMgr && true == lpVirtualMonsterMgr->IsSummonee(dwCID))
				{
					lpMaster = reinterpret_cast<CSummonMonster *>(
						aryIndividualAwardInfo[nSummoneeIndex].m_pCreature)->GetMaster();
					bSummonee = true;
				}
			}
		}
		else
		{
			if (true == CCreatureManager::GetInstance().IsSummonee(dwCID))
			{
				lpMaster = reinterpret_cast<CSummonMonster *>(
					aryIndividualAwardInfo[nSummoneeIndex].m_pCreature)->GetMaster();
				bSummonee = true;
			}
		}

		if (true == bSummonee)
		{
			if (NULL != lpMaster)
			{
				if (pHighestThreatCreature == aryIndividualAwardInfo[nSummoneeIndex].m_pCreature)
				{
					pHighestThreatCreature = lpMaster;
				}	

				int nMasterIndex = 0;
				for (; nMasterIndex < nNumber; ++nMasterIndex)
				{
					if (aryIndividualAwardInfo[nMasterIndex].m_pCreature == lpMaster)
					{
						aryIndividualAwardInfo[nMasterIndex].m_lAward += aryIndividualAwardInfo[nSummoneeIndex].m_lAward;
						aryIndividualAwardInfo[nSummoneeIndex] = AwardInfo();
						break;
					}
				}

				if (nMasterIndex == nNumber)
				{
					aryIndividualAwardInfo[nSummoneeIndex].m_pCreature = lpMaster;
					aryIndividualAwardInfo[nSummoneeIndex].m_pParty = lpMaster->GetParty();
				}
			}
			else
			{
				aryIndividualAwardInfo[nSummoneeIndex] = AwardInfo();
			}
		}
	}

	// 파티 정보에 따라 경험치 분배
	AwardInfo aryPartyAwardInfo[MAX_THREAT_TARGET];

	nNumber = 0;
	for (int nIndividualIndex = 0; nIndividualIndex < MAX_THREAT_TARGET; ++nIndividualIndex)
	{
		if (NULL == aryIndividualAwardInfo[nIndividualIndex].m_pCreature) 
        { 
			continue; 
		}

		// 몬스터는 경험치 분배에서 제외된다.
		Creature::CreatureType eCreatureType = Creature::GetCreatureType(aryIndividualAwardInfo[nIndividualIndex].m_pCreature->GetCID());
		if (Creature::CT_MONSTER == eCreatureType || Creature::CT_STRUCT == eCreatureType)
		{
			continue;
		}

		bool newParty = true;
		for (int nPartyIndex = 0; nPartyIndex < nNumber; ++nPartyIndex)
		{
			if (NULL != aryPartyAwardInfo[nPartyIndex].m_pParty)
			{
				if (aryPartyAwardInfo[nPartyIndex].m_pParty == aryIndividualAwardInfo[nIndividualIndex].m_pParty) 
				{
					aryPartyAwardInfo[nPartyIndex].m_lAward += aryIndividualAwardInfo[nIndividualIndex].m_lAward;

					newParty = false;
					break;
				}
			}
		}

		if (true == newParty)
		{
			aryPartyAwardInfo[nNumber] = aryIndividualAwardInfo[nIndividualIndex];
			if (++nNumber > MAX_THREAT_TARGET) 
            { 
				ERRLOG1(g_Log, "스레트 리스트에서, 들어 있는 멤버의 수가 이상합니다. 멤버의 수는 %d입니다.", nNumber); 
			}
		}
	}

	// 경험치량에 따라 정렬해서 MAX_THREAT_REWARD 만큼만 나눠준다.
	for (int nPartyIndex = 0; nPartyIndex < MAX_THREAT_REWARD; ++nPartyIndex)
	{
		for (int nCompareIndex = nPartyIndex + 1; nCompareIndex < nNumber; ++nCompareIndex)
		{
			if (aryPartyAwardInfo[nPartyIndex].m_lAward < aryPartyAwardInfo[nCompareIndex].m_lAward)
            {
				std::swap(aryPartyAwardInfo[nPartyIndex], aryPartyAwardInfo[nCompareIndex]);
			}
		}

		if (NULL == aryPartyAwardInfo[nPartyIndex].m_pCreature) 
        { 
			break; 
		}

		long lIndividualExp = aryPartyAwardInfo[nPartyIndex].m_lAward;

		time_t t = time(0);   // get time now
		struct tm * now = localtime( & t );
	
		if (now->tm_mday >=1 && now->tm_mday <= 7 ){
			lIndividualExp = lIndividualExp * 1.2 ;// 20% extra exp
		}

		// 파티
		if (NULL != aryPartyAwardInfo[nPartyIndex].m_pParty) 
		{
			reinterpret_cast<CCharacterParty* >(aryPartyAwardInfo[nPartyIndex].m_pParty)->SendDivisionExp(
				reinterpret_cast<CCharacter *>(aryPartyAwardInfo[nPartyIndex].m_pCreature), m_pOwner, lIndividualExp, cHighestLevel);
		} 
		// 개인
		else 
		{
			CCharacter *lpCharacter = reinterpret_cast<CCharacter *>(aryPartyAwardInfo[nPartyIndex].m_pCreature);
			lIndividualExp = static_cast<long>(lIndividualExp * GetAggravation(lpCharacter));

			// Threat List의 2위 이하 캐릭터의 경우 절반만을 받는다.
			if (pHighestThreatCreature != lpCharacter) 
			{
				lIndividualExp /= 2;
			}
//			if(lpCharacter->GetLevel() >(unsigned char)(m_pOwner->GetStatus().m_nLevel + 4))//hayzohar
//				return;
			//if(lpCharacter->GetLevel() < 50)//hz exp up to lvl 50
				lpCharacter->GetHuntingExp(m_pOwner, lIndividualExp, 1);//actual exp giving function hayzohar
		}
	}
}*/
/**********************
void CThreat::DivisionExp(void)
{
	if (true == m_ThreatList.empty()) 
	{ 
		ERRLOG0(g_Log, "스레트 리스트가 비어 있습니다. 이 상태에서는 Exp를 나눌 수 없습니다."); 
		return; 
	} 

	if (NULL == m_pOwner) 
	{ 
		ERRLOG0(g_Log, "소유자가 NULL입니다."); 
		return; 
	} 

	unsigned long lSumOfThreatAmount = 0;

    for (ThreatList::iterator it = m_ThreatList.begin(); it != m_ThreatList.end(); ++it)
    {
		lSumOfThreatAmount += it->m_lThreatAmount;
	}

	// 쓰레트 리스트에 있는 녀석들의 개별적인 경험치 계산
	AwardInfo aryIndividualAwardInfo[MAX_THREAT_TARGET];

	CAggresiveCreature* pHighestThreatCreature = m_ThreatList.back().m_pCreature;
	const unsigned char cHighestLevel = pHighestThreatCreature->GetStatus().m_nLevel;

	int nNumber = 0;
	for (ThreatList::iterator it = m_ThreatList.begin(); 
		it != m_ThreatList.end() && nNumber < MAX_THREAT_TARGET; ++it, ++nNumber)
	{
		if (nNumber >= MAX_THREAT_TARGET) 
		{ 
			ERRLOG1(g_Log, "스레트 리스트에서, 들어 있는 멤버의 수가 이상합니다. 멤버의 수는 %d입니다.", nNumber); 
			break;
		}

		const ThreatInfo& tempThreatInfo = *it;
		
		aryIndividualAwardInfo[nNumber].m_pCreature = tempThreatInfo.m_pCreature;
		aryIndividualAwardInfo[nNumber].m_pParty = tempThreatInfo.m_pCreature->GetParty();

		aryIndividualAwardInfo[nNumber].m_lAward = 
			static_cast<long>(m_pOwner->GetStatus().m_nExp * tempThreatInfo.m_lThreatAmount / 
			std::max(static_cast<float>(lSumOfThreatAmount), static_cast<float>(m_pOwner->GetStatus().m_StatusInfo.m_nMaxHP)));
	
		const int nIndividualLevel = aryIndividualAwardInfo[nNumber].m_pCreature->GetStatus().m_nLevel;
		if (::abs(cHighestLevel - nIndividualLevel) > EXP_CONSENT_GAP)
        { 
			aryIndividualAwardInfo[nNumber].m_lAward = 1; 
		} 
	}

	// 소환수의 경험치는 마스터에게 합쳐준다.
	for (int nSummoneeIndex = 0; nSummoneeIndex < nNumber; ++nSummoneeIndex)
	{
		unsigned long dwCID = aryIndividualAwardInfo[nSummoneeIndex].m_pCreature->GetCID();
		CAggresiveCreature* lpMaster = NULL;
		bool bSummonee = false;

		if (0 != aryIndividualAwardInfo[nSummoneeIndex].m_pCreature->GetMapIndex())
		{
			VirtualArea::CVirtualArea* lpVirtualArea = 
				VirtualArea::CVirtualAreaMgr::GetInstance().GetVirtualArea(
				aryIndividualAwardInfo[nSummoneeIndex].m_pCreature->GetMapIndex());

			if (NULL != lpVirtualArea)
			{
				CVirtualMonsterMgr* lpVirtualMonsterMgr = lpVirtualArea->GetMonsterManager();
				if (NULL != lpVirtualMonsterMgr && true == lpVirtualMonsterMgr->IsSummonee(dwCID))
				{
					lpMaster = reinterpret_cast<CSummonMonster *>(
						aryIndividualAwardInfo[nSummoneeIndex].m_pCreature)->GetMaster();
					bSummonee = true;
				}
			}
		}
		else
		{
			if (true == CCreatureManager::GetInstance().IsSummonee(dwCID))
			{
				lpMaster = reinterpret_cast<CSummonMonster *>(
					aryIndividualAwardInfo[nSummoneeIndex].m_pCreature)->GetMaster();
				bSummonee = true;
			}
		}

		if (true == bSummonee)
		{
			if (NULL != lpMaster)
			{
				if (pHighestThreatCreature == aryIndividualAwardInfo[nSummoneeIndex].m_pCreature)
				{
					pHighestThreatCreature = lpMaster;
				}	

				int nMasterIndex = 0;
				for (; nMasterIndex < nNumber; ++nMasterIndex)
				{
					if (aryIndividualAwardInfo[nMasterIndex].m_pCreature == lpMaster)
					{
						aryIndividualAwardInfo[nMasterIndex].m_lAward += aryIndividualAwardInfo[nSummoneeIndex].m_lAward;
						aryIndividualAwardInfo[nSummoneeIndex] = AwardInfo();
						break;
					}
				}

				if (nMasterIndex == nNumber)
				{
					aryIndividualAwardInfo[nSummoneeIndex].m_pCreature = lpMaster;
					aryIndividualAwardInfo[nSummoneeIndex].m_pParty = lpMaster->GetParty();
				}
			}
			else
			{
				aryIndividualAwardInfo[nSummoneeIndex] = AwardInfo();
			}
		}
	}

	// 파티 정보에 따라 경험치 분배
	AwardInfo aryPartyAwardInfo[MAX_THREAT_TARGET];

	nNumber = 0;
	for (int nIndividualIndex = 0; nIndividualIndex < MAX_THREAT_TARGET; ++nIndividualIndex)
	{
		if (NULL == aryIndividualAwardInfo[nIndividualIndex].m_pCreature) 
        { 
			continue; 
		}

		// 몬스터는 경험치 분배에서 제외된다.
		Creature::CreatureType eCreatureType = Creature::GetCreatureType(aryIndividualAwardInfo[nIndividualIndex].m_pCreature->GetCID());
		if (Creature::CT_MONSTER == eCreatureType || Creature::CT_STRUCT == eCreatureType)
		{
			continue;
		}

		bool newParty = true;
		for (int nPartyIndex = 0; nPartyIndex < nNumber; ++nPartyIndex)
		{
			if (NULL != aryPartyAwardInfo[nPartyIndex].m_pParty)
			{
				if (aryPartyAwardInfo[nPartyIndex].m_pParty == aryIndividualAwardInfo[nIndividualIndex].m_pParty) 
				{
					aryPartyAwardInfo[nPartyIndex].m_lAward += aryIndividualAwardInfo[nIndividualIndex].m_lAward;

					newParty = false;
					break;
				}
			}
		}

		if (true == newParty)
		{
			aryPartyAwardInfo[nNumber] = aryIndividualAwardInfo[nIndividualIndex];
			if (++nNumber > MAX_THREAT_TARGET) 
            { 
				ERRLOG1(g_Log, "스레트 리스트에서, 들어 있는 멤버의 수가 이상합니다. 멤버의 수는 %d입니다.", nNumber); 
			}
		}
	}

	// 경험치량에 따라 정렬해서 MAX_THREAT_REWARD 만큼만 나눠준다.
	for (int nPartyIndex = 0; nPartyIndex < MAX_THREAT_REWARD; ++nPartyIndex)
	{
		for (int nCompareIndex = nPartyIndex + 1; nCompareIndex < nNumber; ++nCompareIndex)
		{
			if (aryPartyAwardInfo[nPartyIndex].m_lAward < aryPartyAwardInfo[nCompareIndex].m_lAward)
            {
				std::swap(aryPartyAwardInfo[nPartyIndex], aryPartyAwardInfo[nCompareIndex]);
			}
		}

		if (NULL == aryPartyAwardInfo[nPartyIndex].m_pCreature) 
        { 
			break; 
		}

		long lIndividualExp = aryPartyAwardInfo[nPartyIndex].m_lAward;

		// 파티
		if (NULL != aryPartyAwardInfo[nPartyIndex].m_pParty) 
		{
			reinterpret_cast<CCharacterParty* >(aryPartyAwardInfo[nPartyIndex].m_pParty)->SendDivisionExp(
				reinterpret_cast<CCharacter *>(aryPartyAwardInfo[nPartyIndex].m_pCreature), m_pOwner, lIndividualExp, cHighestLevel);
		} 
		// 개인
		else 
		{
			CCharacter *lpCharacter = reinterpret_cast<CCharacter *>(aryPartyAwardInfo[nPartyIndex].m_pCreature);
			lIndividualExp = static_cast<long>(lIndividualExp * GetAggravation(lpCharacter));

			// Threat List의 2위 이하 캐릭터의 경우 절반만을 받는다.
			if (pHighestThreatCreature != lpCharacter) 
			{
				lIndividualExp /= 2;
			}

			lpCharacter->GetHuntingExp(m_pOwner, lIndividualExp, 1);
		}
	}
}


bool CThreat::SaveEnemy(unsigned long dwCID)
{
	int nOldestIndex = 0;
	int nLoopIndex = 0;

	const unsigned long dwLastTickCount = ::GetTickCount();

	for (nLoopIndex = 0; nLoopIndex < SaveEnemyInfo::MAX_SAVING_ENEMY; ++nLoopIndex)
	{
		if (0 == m_LatestEnemy[nLoopIndex].m_dwCID) 
		{ 
			continue; 
		}

		// 미국의 토너먼트 서버군에서는 PvP시의 명성 어뷰징 방지책이 강화된다.
		const unsigned long dwSavingTicCount = SaveEnemyInfo::MAX_SAVING_TICKCOUNT;

		if (dwLastTickCount - m_LatestEnemy[nLoopIndex].m_dwTickCount >= dwSavingTicCount)
		{
			m_LatestEnemy[nLoopIndex] = SaveEnemyInfo();
			continue;
		}

		if (dwCID == m_LatestEnemy[nLoopIndex].m_dwCID)
		{
			m_LatestEnemy[nLoopIndex].m_dwTickCount = dwLastTickCount;
			return true;
		}

		if (m_LatestEnemy[nLoopIndex].m_dwTickCount < m_LatestEnemy[nOldestIndex].m_dwTickCount)
		{
			nOldestIndex = nLoopIndex;
		}
	}

	for (nLoopIndex = 0; nLoopIndex < SaveEnemyInfo::MAX_SAVING_ENEMY; ++nLoopIndex)
	{
		if (0 == m_LatestEnemy[nLoopIndex].m_dwCID) 
		{ 
			m_LatestEnemy[nLoopIndex].m_dwCID = dwCID;
			m_LatestEnemy[nLoopIndex].m_dwTickCount = dwLastTickCount;

			return false;
		}
	}

	m_LatestEnemy[nOldestIndex].m_dwCID = dwCID;
	m_LatestEnemy[nOldestIndex].m_dwTickCount = dwLastTickCount;

	return false;
}
*/
struct CompareAmount 
{
    int operator() (const CThreat::ThreatInfo& ThreatInfo1, const CThreat::ThreatInfo& ThreatInfo2)
    {
        return ThreatInfo1.m_lThreatAmount < ThreatInfo2.m_lThreatAmount;
    }
};

struct CompareLevel 
{
	int operator() (const CThreat::ThreatInfo& ThreatInfo1, const CThreat::ThreatInfo& ThreatInfo2)
    {
        return ThreatInfo1.m_pCreature->GetStatus().m_nLevel < ThreatInfo2.m_pCreature->GetStatus().m_nLevel;
    }
};

struct CompareFame
{
	int operator() (const CThreat::ThreatInfo& ThreatInfo1, const CThreat::ThreatInfo& ThreatInfo2)
	{
		return ThreatInfo1.m_pCreature->GetFame() < ThreatInfo2.m_pCreature->GetFame();
	}
};


CThreat::CThreat(void)
:	m_pOwner(NULL)
{
	ClearAll();
}

CThreat::~CThreat(void)
{
}


void CThreat::ClearAll(void)	
{
	m_cMaxLevel = 0;
	m_eMaxLevelCreatureType = Creature::CT_NONE_TYPE;

	ClearThreatList();
	ClearThreatenedList();
}

void CThreat::ClearThreatList(void)
{
	for (ThreatList::iterator ThreatIt = m_ThreatList.begin(), ThreatEnd = m_ThreatList.end();
		ThreatIt != ThreatEnd; ++ThreatIt) 
	{
		ThreatIt->m_pCreature->GetThreat().DeleteThreatened(m_pOwner);
	}
	m_ThreatList.clear();
}

void CThreat::ClearThreatenedList(void)
{
	for (ThreatenedList::iterator ThreatenedIt = m_ThreatenedList.begin(), ThreatenedEnd = m_ThreatenedList.end();
		ThreatenedIt != ThreatenedEnd; ++ThreatenedIt)
	{
		(*ThreatenedIt)->GetThreat().DeleteThreat(m_pOwner);
	}
	m_ThreatenedList.clear();
}


///////////////////////////////////////////////////////////////////////////////////
// Function : CThreat::AddToThreatList
//
// Description : 쓰레트 리스트에 크리쳐 추가
//
// Inputs : pAttackCreature - 추가할 크리쳐의 포인터
//			lThreatAmount - 쓰레트량
//
// Outputs : None.
//
// Returns : None.
///////////////////////////////////////////////////////////////////////////////////
void CThreat::AddToThreatList(CAggresiveCreature* pAttackCreature, long lThreatAmount)
{
	if (NULL == pAttackCreature || 0 == lThreatAmount)
    {
        return;
    }

	if (EnemyCheck::EC_ENEMY != m_pOwner->IsEnemy(pAttackCreature))
	{
		return;
	}

	// 이미 리스트에 있는 녀석의 처리
	for (ThreatList::iterator it = m_ThreatList.begin(); it != m_ThreatList.end(); ++it)
	{
		if (pAttackCreature == it->m_pCreature)
		{
			it->m_lThreatAmount += lThreatAmount;
			if (0 >= it->m_lThreatAmount) 
			{
				pAttackCreature->GetThreat().DeleteThreatened(m_pOwner);
				DeleteThreat(pAttackCreature);
			}

			m_ThreatList.sort(CompareAmount());
			return;
		}
	}

	// 처음 리스트에 들어온 녀석의 처리
	if (0 > lThreatAmount) 
    {
        return;
    }

	size_t nThreatListSize = m_ThreatList.size();
	if (nThreatListSize > MAX_THREAT_TARGET)
	{
		ERRLOG2(g_Log, "CID:0x%08x 쓰레트 리스트 사이즈가 최대치를 넘어섰습니다. Size:%d", 
			m_pOwner->GetCID(), nThreatListSize);

        return;
	}
	else if (nThreatListSize == MAX_THREAT_TARGET)
	{
		ThreatInfo& MinThreatInfo = m_ThreatList.front();
		if (lThreatAmount < MinThreatInfo.m_lThreatAmount)
        {
            return;
        }

		MinThreatInfo.m_pCreature->GetThreat().DeleteThreatened(m_pOwner);
		m_ThreatList.pop_front();
	}

	if (pAttackCreature->GetStatus().m_nLevel > m_cMaxLevel)
	{
		m_cMaxLevel = pAttackCreature->GetStatus().m_nLevel;
		m_eMaxLevelCreatureType = Creature::GetCreatureType(pAttackCreature->GetCID());
	}

	pAttackCreature->GetThreat().AddToThreatenedList(m_pOwner);
	m_ThreatList.push_front(ThreatInfo(pAttackCreature, lThreatAmount));
	m_ThreatList.sort(CompareAmount());
}


///////////////////////////////////////////////////////////////////////////////////
// Function : CThreat::AddToThreatenedList
//
// Description : 상호참조를 위한 리스트에 크리쳐 추가
//
// Inputs : pDefendCreature - 추가할 크리쳐의 포인터
//
// Outputs : None.
//
// Returns : None.
///////////////////////////////////////////////////////////////////////////////////
void CThreat::AddToThreatenedList(CAggresiveCreature* pDefendCreature)
{
	ThreatenedList::iterator FindIt = std::find(m_ThreatenedList.begin(), m_ThreatenedList.end(), pDefendCreature);
	if (FindIt == m_ThreatenedList.end())
    {
        // ThreatenedList에 없을 때만 삽입.
        m_ThreatenedList.push_back(pDefendCreature);
    }
}


///////////////////////////////////////////////////////////////////////////////////
// Function : CThreat::HealThreat
//
// Description : 'Heal Threat'의 적용
//
// Inputs : pHealCreature - 도움을 준 크리쳐
//			lThreatAmount - 도움량
//
// Outputs : None.
//
// Returns : None.
///////////////////////////////////////////////////////////////////////////////////

class CAddToThreatList
{
public:
    CAddToThreatList(CAggresiveCreature* lpAggresiveCreature, long lThreatAmount)
        : m_lpAdd(lpAggresiveCreature), m_nAmount(lThreatAmount) { }

    void operator () (CAggresiveCreature* lpAdded) 
    { if (NULL != lpAdded) { lpAdded->GetThreat().AddToThreatList(m_lpAdd, m_nAmount); } }

private:
    CAggresiveCreature* m_lpAdd;
    int                 m_nAmount;
};


void CThreat::HealThreat(CAggresiveCreature* pHealCreature, long lThreatAmount)
{
    const int               MAX_HEAL_THREATEN_NUM = 30;
    CAggresiveCreature*     lpHealThreaten[MAX_HEAL_THREATEN_NUM];

    CAggresiveCreature**    lpDestination = lpHealThreaten;
    unsigned long           nCount = 0;

    for (ThreatenedList::iterator first = m_ThreatenedList.begin(), last = m_ThreatenedList.end();
        first != last && nCount < MAX_HEAL_THREATEN_NUM; ++lpDestination, ++nCount, ++first)
    {
		*lpDestination = *first;
    }
	
    std::for_each(lpHealThreaten, lpDestination, CAddToThreatList(pHealCreature, lThreatAmount));
}


///////////////////////////////////////////////////////////////////////////////////
// Function : CThreat::AffectThreat
//
// Description : 쓰레트값에 영향을 주는 타운트, 디타운트에 관한 계산.
//
// Inputs : pTauntCreature - 타운트한 크리쳐
//			lThreatAmount - 타운트량
//			eType - 타운트/디타운트 구분
//
// Outputs : None.
//
// Returns : None.
///////////////////////////////////////////////////////////////////////////////////
void CThreat::AffectThreat(CAggresiveCreature* pTauntCreature, long lDamage, AffectThreatType eType)
{
	// 결정타는 쓰레트에 영향을 주지 않는다.
	if (m_pOwner->GetStatus().m_nNowHP <= lDamage) 
    {
		return;
	}

	const long lThreatAmount = lDamage * ((eType == TAUNT) ? 1 : (-1));

	unsigned short wThreatNum = 0;
	for (ThreatList::iterator it = m_ThreatList.begin(); it != m_ThreatList.end(); ++it)
	{
        ThreatInfo& threatInfo = *it;

		threatInfo.m_lThreatAmount = (threatInfo.m_lThreatAmount > lThreatAmount) ? 
			(threatInfo.m_lThreatAmount - lThreatAmount) : 0;

		++wThreatNum;
	}

	if (wThreatNum > MAX_THREAT_TARGET || wThreatNum < 0) 
    {
		ERRLOG1(g_Log, "타운트하는 몬스터의 쓰레트 리스트가 이상합니다. 리스트 사이즈 : %d", wThreatNum);
	}

	AddToThreatList(pTauntCreature, lThreatAmount * wThreatNum);
}


///////////////////////////////////////////////////////////////////////////////////
// Function : CThreat::DeleteThreatened
//
// Description : 상호참조를 위한 리스트에서 크리쳐를 삭제
//
// Inputs : pDefendCreature - 삭제할 크리쳐의 포인터
//
// Outputs : None.
//
// Returns : None.
///////////////////////////////////////////////////////////////////////////////////
bool CThreat::DeleteThreatened(CAggresiveCreature* pDefendCreature)
{
	ThreatenedList::iterator FindIt = std::find(m_ThreatenedList.begin(), m_ThreatenedList.end(), pDefendCreature);
	if (FindIt == m_ThreatenedList.end()) 
    {
        return false;
    }

	m_ThreatenedList.erase(FindIt);
	return true;
}


///////////////////////////////////////////////////////////////////////////////////
// Function : CThreat::DeleteThreat
//
// Description : 쓰레트 리스트에서 크리쳐를 삭제
//
// Inputs : pAttackCreature - 삭제할 크리쳐의 포인터
//
// Outputs : None.
//
// Returns : None.
///////////////////////////////////////////////////////////////////////////////////
bool CThreat::DeleteThreat(CAggresiveCreature* pAttackCreature)
{
	for (ThreatList::iterator it = m_ThreatList.begin(); it != m_ThreatList.end(); ++it)
    {
		if (pAttackCreature == it->m_pCreature) 
        {
			m_ThreatList.erase(it);
			return true;
		}
	}

	return false;
}


///////////////////////////////////////////////////////////////////////////////////
// Function : CThreat::GetTarget
//
// Description : 쓰레트 리스트를 검색해 타겟을 구한다.
//
// Inputs : None.
//
// Outputs : None.
//
// Returns : CAggresiveCreature* - 타겟 크리쳐
///////////////////////////////////////////////////////////////////////////////////
CAggresiveCreature* CThreat::GetTarget(void) 
{
	if (true == m_ThreatList.empty())
    { 
		return NULL; 
	}

	Creature::CreatureType eCreatureType = Creature::GetCreatureType(m_pOwner->GetCID());

	if (Creature::CT_MONSTER == eCreatureType ||
		Creature::CT_SUMMON == eCreatureType ||
		Creature::CT_STRUCT == eCreatureType)
	{
		CMonster* lpMonster = reinterpret_cast<CMonster *>(m_pOwner);

		ThreatList TargetThreatList;
		ThreatInfo TempTargetInfo;

		for (ThreatList::iterator it = m_ThreatList.begin(); it != m_ThreatList.end(); ++it)
		{
			TempTargetInfo = *it;
			if (NULL == TempTargetInfo.m_pCreature) 
			{
				continue;
			}
			if (true == TempTargetInfo.m_pCreature->GetEnchantInfo().GetFlag(Skill::SpellID::Stealth)) 
			{
				continue;
			}
			if (EnemyCheck::EC_ENEMY != m_pOwner->IsEnemy(TempTargetInfo.m_pCreature))
			{
				continue;
			}

			const float fAttackRange = lpMonster->GetAttackRange() / 100.0f;

			MotionInfo Motion;
			lpMonster->GetMotion(MonsterInfo::Z3D_CA_WALK, Motion);
			const float fMovingAttackRange = fAttackRange + Motion.m_fVelocity;

			const float fDX = TempTargetInfo.m_pCreature->GetCurrentPos().m_fPointX - m_pOwner->GetCurrentPos().m_fPointX;
			const float fDZ = TempTargetInfo.m_pCreature->GetCurrentPos().m_fPointZ - m_pOwner->GetCurrentPos().m_fPointZ;
			const float fDistance = sqrtf((fDX * fDX) + (fDZ * fDZ));

			if (fDistance < fAttackRange) 
			{
				TempTargetInfo.m_lThreatAmount *= ATTACK_TAUNT_RATE;
			}
			else if (fDistance < fMovingAttackRange)
			{
				TempTargetInfo.m_lThreatAmount *= MOVING_ATTACK_TAUNT_RATE;
			}

			TargetThreatList.push_back(TempTargetInfo);
		}

		if (true == TargetThreatList.empty())
		{
			return NULL;
		}

		TargetThreatList.sort(CompareAmount());
		return TargetThreatList.back().m_pCreature;
		
	}
	else if (Creature::CT_SIEGE_OBJECT == eCreatureType)
	{
		CSiegeObject* lpSiegeObject = reinterpret_cast<CSiegeObject*>(m_pOwner);

		if (!lpSiegeObject)
		{
			ERRLOG1(g_Log, "CID:0x%08x 공격 대상을 찾는 공성 오브젝트가 존재하지 않습니다.", m_pOwner->GetCID());
			return NULL;
		}

		if (!lpSiegeObject->IsGuard() && !lpSiegeObject->IsEmblem() && !lpSiegeObject->IsCamp())
		{
			ERRLOG2(g_Log, "CID:0x%08x wObjectType:%d 공격 대상을 찾는 공성 오브젝트가 가드, 상징물, 요새가 아닙니다.",
					m_pOwner->GetCID(), lpSiegeObject->GetObjectType());
			return NULL;
		}
		
		ThreatList TargetThreatList;
		ThreatInfo TempTargetInfo;

		for (ThreatList::iterator it = m_ThreatList.begin(); it != m_ThreatList.end(); ++it)
		{
			TempTargetInfo = *it;
			if (NULL == TempTargetInfo.m_pCreature) 
			{
				continue;
			}
			if (true == TempTargetInfo.m_pCreature->GetEnchantInfo().GetFlag(Skill::SpellID::Stealth)) 
			{
				continue;
			}
			if (EnemyCheck::EC_ENEMY != m_pOwner->IsEnemy(TempTargetInfo.m_pCreature))
			{
				continue;
			}

			TargetThreatList.push_back(TempTargetInfo);
		}

		if (true == TargetThreatList.empty())
		{
			return NULL;
		}

		TargetThreatList.sort(CompareAmount());
		return TargetThreatList.back().m_pCreature;
	}

	ERRLOG1(g_Log, "CID:0x%08x 몬스터 혹은 가드, 성 상징물, 요새가 아닌 녀석이 공격 타겟을 찾습니다.", m_pOwner->GetCID());
	return NULL;
}


///////////////////////////////////////////////////////////////////////////////////
// Function : CThreat::GetAggravation
//
// Description : 레벨갭 얻기 (경험치 분배용)
//
// Inputs : pCreature - 비교할 크리쳐
//
// Outputs : None.
//
// Returns : float - 레벨갭
///////////////////////////////////////////////////////////////////////////////////
float CThreat::GetAggravation(CAggresiveCreature* pCreature)
{
	static const float aryAggravation[CAggresiveCreature::AGGRAVATION_NUM] = { 
		0.0f,	0.0f,	0.0f,	0.0f,	0.0f,	0.0f,	0.0f,	0.0f,	0.0f,	0.0f,	
	    0.0f,	0.1f,	0.2f,	0.3f,	0.4f,	0.5f,	0.6f,	0.7f,	0.8f,	0.9f,	
		1.0f, 
		1.05f,	1.1f,	1.15f,	1.2f,	1.25f,	1.3f,	1.35f,	1.4f,	1.45f, 	1.5f,
		1.55f,	1.6f,	1.65f,	1.7f,	1.75f,	1.8f,	1.85f,	1.9f,	1.95f, 	2.0f,
	};

	const int nLevelDifference = m_pOwner->CalculateLevelGap(pCreature);
	return aryAggravation[nLevelDifference + CAggresiveCreature::MAX_LEVEL_GAP];
}


//unsigned char CThreat::GetAward(unsigned long* aryItemID, Item::CItem** aryItem, unsigned long* dwOwnerID) ROGE CODE
unsigned char CThreat::GetAward(unsigned long* aryItemID, Item::CItem** aryItem, unsigned long* dwOwnerID,bool farm) //for autolot alex
{
	if (true == m_ThreatList.empty()) { return 0; }
	if (Creature::CT_MONSTER != Creature::GetCreatureType(m_pOwner->GetCID())) { return 0; }

	if (Creature::CT_PC != m_eMaxLevelCreatureType && Creature::CT_SUMMON != m_eMaxLevelCreatureType)
	{
		// 쓰레트 리스트에 들어있는 크리쳐 중 가장 레벨이 높은 크리쳐가 
		// '플레이어'나 '소환수'인 경우에만 아이템을 드랍한다.
		return 0;
	}

	int nRandPoint = 0;
	int nGradeSpace = 0;
	int nLevelDifferenceForArray = 0;

	// 몬스터의 레벨과 유저의 최대레벨을 비교해 그차이를 구한다.
	const int nLevelDifference = m_pOwner->CalculateLevelGap(m_pOwner->GetStatus().m_nLevel, m_cMaxLevel);
	// 레벨갭+20 을해준다.
	nLevelDifferenceForArray = nLevelDifference + CAggresiveCreature::MAX_LEVEL_GAP;

	ThreatInfo HighestThreatInfo = m_ThreatList.back();

	if (NULL == HighestThreatInfo.m_pCreature)
	{
		ERRLOG0(g_Log, "쓰레트 리스트의 탑에 크리쳐 포인터가 없습니다.");
		return 0;
	}

	CMonster*	lpMonster	= reinterpret_cast<CMonster *>(m_pOwner);
	CCharacter*	lpCharacter = NULL;

	float		fPt = 1.0f;
	
	switch (Creature::GetCreatureType(HighestThreatInfo.m_pCreature->GetCID()))
	{
		case Creature::CT_PC:		lpCharacter = reinterpret_cast<CCharacter *>(HighestThreatInfo.m_pCreature);					break;
		case Creature::CT_SUMMON:	lpCharacter = reinterpret_cast<CSummonMonster *>(HighestThreatInfo.m_pCreature)->GetMaster();	break;

		default:					return 0;
	}

	// 퍼센트가 0이면 아무것도 못구한다. 그냥 리턴
	fPt = lpCharacter->GetAwardPer();
	if(fPt == 0.0f)
		return 0;

	bool bPremiumTime = lpCharacter->CheckPremiumTime();

	*dwOwnerID = lpCharacter->GetCID();

	// 루팅 권한을 가진 캐릭터의 파티원 중에, 전투에 참여했고 '행운의 오브'를 사용했다면 드랍률에 영향을 준다.
	bool			bLuckyOrb	= false;	

	if (NULL != lpCharacter->GetParty())
	{
		for (ThreatList::iterator it = m_ThreatList.begin(); it != m_ThreatList.end(); ++it)
		{
			if (it->m_pCreature->GetParty() == lpCharacter->GetParty())
			{
				if (true == it->m_pCreature->GetEnchantInfo().GetFlag(Skill::SpellID::LuckyOrb))
				{
					bLuckyOrb = true;
					break;
				}
			}
		}
	}

	float		fAddDropPt = 0.0f;

	// '행운의 오브' 사용시
	if (true == bLuckyOrb)
	{
		fAddDropPt += 0.3f;
	}
	
	// 프리미엄 서비스
	if(bPremiumTime)
	{
		fAddDropPt += lpCharacter->GetPremiumPt();
	}

	// '부의 석상' 사용시.
	if (true == lpCharacter->GetEnchantInfo().GetFlag(Skill::SpellID::WealthStatue))
	{			
		unsigned short wLevel = lpCharacter->GetEnchantLevel(Skill::SpellID::WealthStatue);
		fAddDropPt += (wLevel * 0.01f);
	}

	// 몬스터의드랍 확율을 더한다.
	unsigned char cItemKindIndex = 0;
	int nTotalRate = 0;
	for (cItemKindIndex = 0; cItemKindIndex < MonsterInfo::MAX_AWARD_KIND; ++cItemKindIndex) 
    {
		// 스크립트에 정의된 아이템별 드랍수치를 모두 합한다.
		nTotalRate += lpMonster->GetDropRate(cItemKindIndex);
	}

	// 할로윈 이벤트용 및 게더 오브젝트용 (게더도 최대 3개까지 떨군다)
	int MonID = lpMonster->GetKID();
	int MonDropMax = 1+Math::Random::ComplexRandom(2);

	bool bAllInOne = false;

	if(3006 <= MonID && MonID <= 3009)
		bAllInOne = true;
	else if(3101 <= MonID && MonID <= 3500)
		bAllInOne = true;	// 게더 오브젝트

	CMonster* mon = reinterpret_cast<CMonster *>(m_pOwner);

	int nDropItemIndex = 0, nDropItemNum = 0;
	for (; nDropItemIndex < AwardTable::MAX_DROP_ITEM + EliteBonus::MAX_BONUS_DROP_ITEM; 
		++nDropItemIndex)
	{
		aryItemID[nDropItemNum] = 0;

		unsigned short wDropRate = 0;
		if (nDropItemIndex < AwardTable::MAX_DROP_ITEM)
		{
			wDropRate = AwardTable::aryItemDropRate[nLevelDifferenceForArray][nDropItemIndex];
		}
	/*	else if(mon->GetPattern() == MonsterInfo::PATTERN_NAMED)
		{
			wDropRate = EliteBonus::usBonusItemDropRate[0][nDropItemIndex - AwardTable::MAX_DROP_ITEM];
		} */
		else
		{
			// 서버 엘리트 보너스에 의한 추가 드랍률
			char eliteBonus = lpCharacter->GetEliteBonus();
			if (0 < eliteBonus)
			{
				wDropRate = EliteBonus::usBonusItemDropRate[eliteBonus - 1][nDropItemIndex - AwardTable::MAX_DROP_ITEM];
			}
		}

		// 추가드랍율 추가.
		if(fAddDropPt > 0.0f)
		{
			wDropRate += static_cast<unsigned short>(wDropRate * fAddDropPt);
		}

		wDropRate = (unsigned short)(wDropRate*(CServerSetup::GetInstance().GetDropDefault() / 100.0f)*fPt);

		// fPt를 곱해준 이유는 fPt 값에 따라 드랍율이 달라진다.
		// CServerSetup::GetInstance().GetDropDefault() = 100 임.
		// 
		if (!bAllInOne && Math::Random::ComplexRandom(1000) > wDropRate) 
		{ 
			continue;
		}

		// 만약 총 드랍수치가 10만이면 10만을 가지고 드랍을 시킨다.
		nRandPoint = Math::Random::ComplexRandom(nTotalRate);

		for (cItemKindIndex = 0; cItemKindIndex < MonsterInfo::MAX_AWARD_KIND; ++cItemKindIndex)
		{
			// 드랍하지 않는 아이템종류면 리턴
			int nDropRate = lpMonster->GetDropRate(cItemKindIndex);
			if (0 == nDropRate) { continue; }

			// 아이템을 모두 돈다.
			// 각 아이템의 드랍율을 더해준다.
			nGradeSpace += nDropRate;

			// 랜덤으로 나온 값보다. 더해준 아이템의 드랍수치가 더 높으면
			// 드랍을 한다.
			if (nGradeSpace > nRandPoint)
			{
				aryItemID[nDropItemNum] = AwardTable::CAward::GetInstance().GetAward(
					cItemKindIndex, reinterpret_cast<CMonster *>(m_pOwner), lpCharacter, aryItem + nDropItemNum);

				if (0 == aryItemID[nDropItemNum])
				{
					CMonster* pMon = reinterpret_cast<CMonster *>(m_pOwner);
					
					if(pMon)
						ERRLOG2(g_Log, "떨어질 아이템이 결정되지 않았습니다. 몬스터KID : %d, 종류 : %d", pMon->GetKID(), cItemKindIndex);
				}
				else
				{
					++nDropItemNum;
				}

				// 초기화.
				cItemKindIndex = MonsterInfo::MAX_AWARD_KIND;
				nGradeSpace = 0;
			} 

			// 할로윈 이벤트. 3000~3005 
			// 보석상자 이벤트. 3006~3010
			if(3001 <= MonID && MonID <= 3010)
			{
				// 3006 번부터 3009번까지는 보석상자 
				// 무조건 1개만 떨구게 수정
				if(3006 <= MonID && MonID <= 3010 && nDropItemNum >= 1)
					return nDropItemNum;

				if(MonID == 3003 && nDropItemNum >= 1)
					return nDropItemNum;

				if(nDropItemNum >= MonDropMax)
					return nDropItemNum;
			}

			// 게더 오브젝트
			if(3101 <= MonID && MonID <= 3500)
			{
				if(nDropItemNum >= MonDropMax)
					return nDropItemNum;
			}
		}
	}

	// 할로윈 이벤트. 3000~3005 
	// 보석상자 이벤트. 3006~3010
	if(0 == nDropItemNum && 3001 <= MonID && MonID <= 3010)
	{
		// cItemKindIndex 를 0으로 하는 이유는 잡탬 A를 기본적으로 떨구게 하기 위해서이다.
		cItemKindIndex = 0;
		
		// 실패한 뒤에 잡탬A를 떨어뜨리게 했는데 이 잡템A가 0 즉 없는 아이템이면 
		// 이건 아무것도 떨구지 않는 잡몹이라는뜻.
		// 드랍하지 않는 아이템종류면 리턴
		int nDropRate = lpMonster->GetDropRate(cItemKindIndex);
		if (0 == nDropRate) 
			return 0; 

		// 무조건 드랍하는 이벤트인데 한개도 떨구지 않았으면 
		aryItemID[nDropItemNum] = AwardTable::CAward::GetInstance().GetAward(
			cItemKindIndex, reinterpret_cast<CMonster *>(m_pOwner), lpCharacter, aryItem + nDropItemNum);

		if (0 == aryItemID[nDropItemNum])
		{
			CMonster* pMon = reinterpret_cast<CMonster *>(m_pOwner);
			
			if(pMon)
				ERRLOG2(g_Log, "떨어질 아이템이 결정되지 않았습니다. 몬스터KID : %d, 종류 : %d", pMon->GetKID(), cItemKindIndex);

			return 0;
		}

		++nDropItemNum;
	}


	return nDropItemNum;
}


///////////////////////////////////////////////////////////////////////////////////
// Function : CThreat::DivisionExp
//
// Description : 경험치 분배
//
// Inputs : None.
//
// Outputs : None.
//
// Returns : None.
///////////////////////////////////////////////////////////////////////////////////
void CThreat::DivisionExp(void)
{
	if (true == m_ThreatList.empty()) 
	{ 
		ERRLOG0(g_Log, "스레트 리스트가 비어 있습니다. 이 상태에서는 Exp를 나눌 수 없습니다."); 
		return; 
	} 

	if (NULL == m_pOwner) 
	{ 
		ERRLOG0(g_Log, "소유자가 NULL입니다."); 
		return; 
	} 

	unsigned long lSumOfThreatAmount = 0;

    for (ThreatList::iterator it = m_ThreatList.begin(); it != m_ThreatList.end(); ++it)
    {
		lSumOfThreatAmount += it->m_lThreatAmount;
	}

	// 쓰레트 리스트에 있는 녀석들의 개별적인 경험치 계산
	AwardInfo aryIndividualAwardInfo[MAX_THREAT_TARGET];

	CAggresiveCreature* pHighestThreatCreature = m_ThreatList.back().m_pCreature;
	const unsigned char cHighestLevel = pHighestThreatCreature->GetStatus().m_nLevel;

	int nNumber = 0;
	for (ThreatList::iterator it = m_ThreatList.begin(); 
		it != m_ThreatList.end() && nNumber < MAX_THREAT_TARGET; ++it, ++nNumber)
	{
		if (nNumber >= MAX_THREAT_TARGET) 
		{ 
			ERRLOG1(g_Log, "스레트 리스트에서, 들어 있는 멤버의 수가 이상합니다. 멤버의 수는 %d입니다.", nNumber); 
			break;
		}

		const ThreatInfo& tempThreatInfo = *it;
		
		aryIndividualAwardInfo[nNumber].m_pCreature = tempThreatInfo.m_pCreature;
		aryIndividualAwardInfo[nNumber].m_pParty = tempThreatInfo.m_pCreature->GetParty();

		aryIndividualAwardInfo[nNumber].m_lAward = 
			static_cast<long>(m_pOwner->GetStatus().m_nExp * tempThreatInfo.m_lThreatAmount / 
			std::max(static_cast<float>(lSumOfThreatAmount), static_cast<float>(m_pOwner->GetStatus().m_StatusInfo.m_nMaxHP)));
	
		const int nIndividualLevel = aryIndividualAwardInfo[nNumber].m_pCreature->GetStatus().m_nLevel;
		if (::abs(cHighestLevel - nIndividualLevel) > EXP_CONSENT_GAP)
        { 
			aryIndividualAwardInfo[nNumber].m_lAward = 1; 
		} 
	}

	// 소환수의 경험치는 마스터에게 합쳐준다.
	for (int nSummoneeIndex = 0; nSummoneeIndex < nNumber; ++nSummoneeIndex)
	{
		unsigned long dwCID = aryIndividualAwardInfo[nSummoneeIndex].m_pCreature->GetCID();
		CAggresiveCreature* lpMaster = NULL;
		bool bSummonee = false;

		if (0 != aryIndividualAwardInfo[nSummoneeIndex].m_pCreature->GetMapIndex())
		{
			VirtualArea::CVirtualArea* lpVirtualArea = 
				VirtualArea::CVirtualAreaMgr::GetInstance().GetVirtualArea(
				aryIndividualAwardInfo[nSummoneeIndex].m_pCreature->GetMapIndex());

			if (NULL != lpVirtualArea)
			{
				CVirtualMonsterMgr* lpVirtualMonsterMgr = lpVirtualArea->GetMonsterManager();
				if (NULL != lpVirtualMonsterMgr && true == lpVirtualMonsterMgr->IsSummonee(dwCID))
				{
					lpMaster = reinterpret_cast<CSummonMonster *>(
						aryIndividualAwardInfo[nSummoneeIndex].m_pCreature)->GetMaster();
					bSummonee = true;
				}
			}
		}
		else
		{
			if (true == CCreatureManager::GetInstance().IsSummonee(dwCID))
			{
				lpMaster = reinterpret_cast<CSummonMonster *>(
					aryIndividualAwardInfo[nSummoneeIndex].m_pCreature)->GetMaster();
				bSummonee = true;
			}
		}

		if (true == bSummonee)
		{
			if (NULL != lpMaster)
			{
				if (pHighestThreatCreature == aryIndividualAwardInfo[nSummoneeIndex].m_pCreature)
				{
					pHighestThreatCreature = lpMaster;
				}	

				int nMasterIndex = 0;
				for (; nMasterIndex < nNumber; ++nMasterIndex)
				{
					if (aryIndividualAwardInfo[nMasterIndex].m_pCreature == lpMaster)
					{
						aryIndividualAwardInfo[nMasterIndex].m_lAward += aryIndividualAwardInfo[nSummoneeIndex].m_lAward;
						aryIndividualAwardInfo[nSummoneeIndex] = AwardInfo();
						break;
					}
				}

				if (nMasterIndex == nNumber)
				{
					aryIndividualAwardInfo[nSummoneeIndex].m_pCreature = lpMaster;
					aryIndividualAwardInfo[nSummoneeIndex].m_pParty = lpMaster->GetParty();
				}
			}
			else
			{
				aryIndividualAwardInfo[nSummoneeIndex] = AwardInfo();
			}
		}
	}

	// 파티 정보에 따라 경험치 분배
	AwardInfo aryPartyAwardInfo[MAX_THREAT_TARGET];

	nNumber = 0;
	for (int nIndividualIndex = 0; nIndividualIndex < MAX_THREAT_TARGET; ++nIndividualIndex)
	{
		if (NULL == aryIndividualAwardInfo[nIndividualIndex].m_pCreature) 
        { 
			continue; 
		}

		// 몬스터는 경험치 분배에서 제외된다.
		Creature::CreatureType eCreatureType = Creature::GetCreatureType(aryIndividualAwardInfo[nIndividualIndex].m_pCreature->GetCID());
		if (Creature::CT_MONSTER == eCreatureType || Creature::CT_STRUCT == eCreatureType)
		{
			continue;
		}

		bool newParty = true;
		for (int nPartyIndex = 0; nPartyIndex < nNumber; ++nPartyIndex)
		{
			if (NULL != aryPartyAwardInfo[nPartyIndex].m_pParty)
			{
				if (aryPartyAwardInfo[nPartyIndex].m_pParty == aryIndividualAwardInfo[nIndividualIndex].m_pParty) 
				{
					aryPartyAwardInfo[nPartyIndex].m_lAward += aryIndividualAwardInfo[nIndividualIndex].m_lAward;

					newParty = false;
					break;
				}
			}
		}

		if (true == newParty)
		{
			aryPartyAwardInfo[nNumber] = aryIndividualAwardInfo[nIndividualIndex];
			if (++nNumber > MAX_THREAT_TARGET) 
            { 
				ERRLOG1(g_Log, "스레트 리스트에서, 들어 있는 멤버의 수가 이상합니다. 멤버의 수는 %d입니다.", nNumber); 
			}
		}
	}

	// 경험치량에 따라 정렬해서 MAX_THREAT_REWARD 만큼만 나눠준다.
	for (int nPartyIndex = 0; nPartyIndex < MAX_THREAT_REWARD; ++nPartyIndex)
	{
		for (int nCompareIndex = nPartyIndex + 1; nCompareIndex < nNumber; ++nCompareIndex)
		{
			if (aryPartyAwardInfo[nPartyIndex].m_lAward < aryPartyAwardInfo[nCompareIndex].m_lAward)
            {
				std::swap(aryPartyAwardInfo[nPartyIndex], aryPartyAwardInfo[nCompareIndex]);
			}
		}

		if (NULL == aryPartyAwardInfo[nPartyIndex].m_pCreature) 
        { 
			break; 
		}

		long lIndividualExp = aryPartyAwardInfo[nPartyIndex].m_lAward;

		// 파티
		if (NULL != aryPartyAwardInfo[nPartyIndex].m_pParty) 
		{
			reinterpret_cast<CCharacterParty* >(aryPartyAwardInfo[nPartyIndex].m_pParty)->SendDivisionExp(
				reinterpret_cast<CCharacter *>(aryPartyAwardInfo[nPartyIndex].m_pCreature), m_pOwner, lIndividualExp, cHighestLevel);
		} 
		// 개인
		else 
		{
			CCharacter *lpCharacter = reinterpret_cast<CCharacter *>(aryPartyAwardInfo[nPartyIndex].m_pCreature);
			lIndividualExp = static_cast<long>(lIndividualExp * GetAggravation(lpCharacter));

			// Threat List의 2위 이하 캐릭터의 경우 절반만을 받는다.
			if (pHighestThreatCreature != lpCharacter) 
			{
				lIndividualExp /= 2;
			}

			lpCharacter->GetHuntingExp(m_pOwner, lIndividualExp, 1);
		}
	}
}


bool CThreat::SaveEnemy(unsigned long dwCID)
{
	int nOldestIndex = 0;
	int nLoopIndex = 0;

	const unsigned long dwLastTickCount = ::GetTickCount();

	for (nLoopIndex = 0; nLoopIndex < SaveEnemyInfo::MAX_SAVING_ENEMY; ++nLoopIndex)
	{
		if (0 == m_LatestEnemy[nLoopIndex].m_dwCID) 
		{ 
			continue; 
		}

		// 미국의 토너먼트 서버군에서는 PvP시의 명성 어뷰징 방지책이 강화된다.
		const unsigned long dwSavingTicCount = SaveEnemyInfo::MAX_SAVING_TICKCOUNT;

		if (dwLastTickCount - m_LatestEnemy[nLoopIndex].m_dwTickCount >= dwSavingTicCount)
		{
			m_LatestEnemy[nLoopIndex] = SaveEnemyInfo();
			continue;
		}

		if (dwCID == m_LatestEnemy[nLoopIndex].m_dwCID)
		{
			m_LatestEnemy[nLoopIndex].m_dwTickCount = dwLastTickCount;
			return true;
		}

		if (m_LatestEnemy[nLoopIndex].m_dwTickCount < m_LatestEnemy[nOldestIndex].m_dwTickCount)
		{
			nOldestIndex = nLoopIndex;
		}
	}

	for (nLoopIndex = 0; nLoopIndex < SaveEnemyInfo::MAX_SAVING_ENEMY; ++nLoopIndex)
	{
		if (0 == m_LatestEnemy[nLoopIndex].m_dwCID) 
		{ 
			m_LatestEnemy[nLoopIndex].m_dwCID = dwCID;
			m_LatestEnemy[nLoopIndex].m_dwTickCount = dwLastTickCount;

			return false;
		}
	}

	m_LatestEnemy[nOldestIndex].m_dwCID = dwCID;
	m_LatestEnemy[nOldestIndex].m_dwTickCount = dwLastTickCount;

	return false;
}