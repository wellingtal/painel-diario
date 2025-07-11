
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
// Description : ����Ʈ ����Ʈ�� ũ���� �߰�
//
// Inputs : pAttackCreature - �߰��� ũ������ ������
//			lThreatAmount - ����Ʈ��
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

	// �̹� ����Ʈ�� �ִ� �༮�� ó��
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

	// ó�� ����Ʈ�� ���� �༮�� ó��
	if (0 > lThreatAmount) 
    {
        return;
    }

	size_t nThreatListSize = m_ThreatList.size();
	if (nThreatListSize > MAX_THREAT_TARGET)
	{
		ERRLOG2(g_Log, "CID:0x%08x ����Ʈ ����Ʈ ����� �ִ�ġ�� �Ѿ���ϴ�. Size:%d", 
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
// Description : ��ȣ������ ���� ����Ʈ�� ũ���� �߰�
//
// Inputs : pDefendCreature - �߰��� ũ������ ������
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
        // ThreatenedList�� ���� ���� ����.
        m_ThreatenedList.push_back(pDefendCreature);
    }
}


///////////////////////////////////////////////////////////////////////////////////
// Function : CThreat::HealThreat
//
// Description : 'Heal Threat'�� ����
//
// Inputs : pHealCreature - ������ �� ũ����
//			lThreatAmount - ����
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
// Description : ����Ʈ���� ������ �ִ� Ÿ��Ʈ, ��Ÿ��Ʈ�� ���� ���.
//
// Inputs : pTauntCreature - Ÿ��Ʈ�� ũ����
//			lThreatAmount - Ÿ��Ʈ��
//			eType - Ÿ��Ʈ/��Ÿ��Ʈ ����
//
// Outputs : None.
//
// Returns : None.
///////////////////////////////////////////////////////////////////////////////////
void CThreat::AffectThreat(CAggresiveCreature* pTauntCreature, long lDamage, AffectThreatType eType)
{
	// ����Ÿ�� ����Ʈ�� ������ ���� �ʴ´�.
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
		ERRLOG1(g_Log, "Ÿ��Ʈ�ϴ� ������ ����Ʈ ����Ʈ�� �̻��մϴ�. ����Ʈ ������ : %d", wThreatNum);
	}

	AddToThreatList(pTauntCreature, lThreatAmount * wThreatNum);
}


///////////////////////////////////////////////////////////////////////////////////
// Function : CThreat::DeleteThreatened
//
// Description : ��ȣ������ ���� ����Ʈ���� ũ���ĸ� ����
//
// Inputs : pDefendCreature - ������ ũ������ ������
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
// Description : ����Ʈ ����Ʈ���� ũ���ĸ� ����
//
// Inputs : pAttackCreature - ������ ũ������ ������
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
// Description : ����Ʈ ����Ʈ�� �˻��� Ÿ���� ���Ѵ�.
//
// Inputs : None.
//
// Outputs : None.
//
// Returns : CAggresiveCreature* - Ÿ�� ũ����
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
			ERRLOG1(g_Log, "CID:0x%08x ���� ����� ã�� ���� ������Ʈ�� �������� �ʽ��ϴ�.", m_pOwner->GetCID());
			return NULL;
		}

		if (!lpSiegeObject->IsGuard() && !lpSiegeObject->IsEmblem() && !lpSiegeObject->IsCamp())
		{
			ERRLOG2(g_Log, "CID:0x%08x wObjectType:%d ���� ����� ã�� ���� ������Ʈ�� ����, ��¡��, ����� �ƴմϴ�.",
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

	ERRLOG1(g_Log, "CID:0x%08x ���� Ȥ�� ����, �� ��¡��, ����� �ƴ� �༮�� ���� Ÿ���� ã���ϴ�.", m_pOwner->GetCID());
	return NULL;
}


///////////////////////////////////////////////////////////////////////////////////
// Function : CThreat::GetAggravation
//
// Description : ������ ��� (����ġ �й��)
//
// Inputs : pCreature - ���� ũ����
//
// Outputs : None.
//
// Returns : float - ������
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
		// ����Ʈ ����Ʈ�� ����ִ� ũ���� �� ���� ������ ���� ũ���İ� 
		// '�÷��̾�'�� '��ȯ��'�� ��쿡�� �������� ����Ѵ�.
		return 0;
	}

	int nRandPoint = 0;
	int nGradeSpace = 0;
	int nLevelDifferenceForArray = 0;

	// ������ ������ ������ �ִ뷹���� ���� �����̸� ���Ѵ�.
	const int nLevelDifference = m_pOwner->CalculateLevelGap(m_pOwner->GetStatus().m_nLevel, m_cMaxLevel);
	// ������+20 �����ش�.
	nLevelDifferenceForArray = nLevelDifference + CAggresiveCreature::MAX_LEVEL_GAP;

	ThreatInfo HighestThreatInfo = m_ThreatList.back();

	if (NULL == HighestThreatInfo.m_pCreature)
	{
		ERRLOG0(g_Log, "����Ʈ ����Ʈ�� ž�� ũ���� �����Ͱ� �����ϴ�.");
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

	// �ۼ�Ʈ�� 0�̸� �ƹ��͵� �����Ѵ�. �׳� ����
	fPt = lpCharacter->GetAwardPer();
	if(fPt == 0.0f)
		return 0;

	bool bPremiumTime = lpCharacter->CheckPremiumTime();

	*dwOwnerID = lpCharacter->GetCID();

	// ���� ������ ���� ĳ������ ��Ƽ�� �߿�, ������ �����߰� '����� ����'�� ����ߴٸ� ������� ������ �ش�.
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

	// '����� ����' ����
	if (true == bLuckyOrb)
	{
		fAddDropPt += 0.3f;
	}
	
	// �����̾� ����
	if(bPremiumTime)
	{
		fAddDropPt += lpCharacter->GetPremiumPt();
	}

	// '���� ����' ����.
	if (true == lpCharacter->GetEnchantInfo().GetFlag(Skill::SpellID::WealthStatue))
	{			
		unsigned short wLevel = lpCharacter->GetEnchantLevel(Skill::SpellID::WealthStatue);
		fAddDropPt += (wLevel * 0.01f);
	}

	// �����ǵ�� Ȯ���� ���Ѵ�.
	unsigned char cItemKindIndex = 0;
	int nTotalRate = 0;
	for (cItemKindIndex = 0; cItemKindIndex < MonsterInfo::MAX_AWARD_KIND; ++cItemKindIndex) 
    {
		// ��ũ��Ʈ�� ���ǵ� �����ۺ� �����ġ�� ��� ���Ѵ�.
		nTotalRate += lpMonster->GetDropRate(cItemKindIndex);
	}

	// �ҷ��� �̺�Ʈ�� �� �Դ� ������Ʈ�� (�Դ��� �ִ� 3������ ������)
	int MonID = lpMonster->GetKID();
	int MonDropMax = 1+Math::Random::ComplexRandom(2);

	bool bAllInOne = false;

	if(3006 <= MonID && MonID <= 3009)
		bAllInOne = true;
	else if(3101 <= MonID && MonID <= 3500)
		bAllInOne = true;	// �Դ� ������Ʈ

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
			// ���� ����Ʈ ���ʽ��� ���� �߰� �����
			char eliteBonus = lpCharacter->GetEliteBonus();
			if (0 < eliteBonus)
			{
				wDropRate = EliteBonus::usBonusItemDropRate[eliteBonus - 1][nDropItemIndex - AwardTable::MAX_DROP_ITEM];
			}
		}

		// �߰������ �߰�.
		if(fAddDropPt > 0.0f)
		{
			wDropRate += static_cast<unsigned short>(wDropRate * fAddDropPt);
		}

		wDropRate = (unsigned short)(wDropRate*(CServerSetup::GetInstance().GetDropDefault() / 100.0f)*fPt);

		// fPt�� ������ ������ fPt ���� ���� ������� �޶�����.
		// CServerSetup::GetInstance().GetDropDefault() = 100 ��.
		// 
		if (!bAllInOne && Math::Random::ComplexRandom(1000) > wDropRate) 
		{ 
			continue;
		}

		// ���� �� �����ġ�� 10���̸� 10���� ������ ����� ��Ų��.
		nRandPoint = Math::Random::ComplexRandom(nTotalRate);

		for (cItemKindIndex = 0; cItemKindIndex < MonsterInfo::MAX_AWARD_KIND; ++cItemKindIndex)
		{
			// ������� �ʴ� ������������ ����
			int nDropRate = lpMonster->GetDropRate(cItemKindIndex);
			if (0 == nDropRate) { continue; }

			// �������� ��� ����.
			// �� �������� ������� �����ش�.
			nGradeSpace += nDropRate;

			// �������� ���� ������. ������ �������� �����ġ�� �� ������
			// ����� �Ѵ�.
			if (nGradeSpace > nRandPoint)
			{
				aryItemID[nDropItemNum] = AwardTable::CAward::GetInstance().GetAward(
					cItemKindIndex, reinterpret_cast<CMonster *>(m_pOwner), lpCharacter, aryItem + nDropItemNum);

				if (0 == aryItemID[nDropItemNum])
				{
					CMonster* pMon = reinterpret_cast<CMonster *>(m_pOwner);
					
					if(pMon)
						ERRLOG2(g_Log, "������ �������� �������� �ʾҽ��ϴ�. ����KID : %d, ���� : %d", pMon->GetKID(), cItemKindIndex);
				}
				else
				{
					++nDropItemNum;
				}

				// �ʱ�ȭ.
				cItemKindIndex = MonsterInfo::MAX_AWARD_KIND;
				nGradeSpace = 0;
			} 

			// �ҷ��� �̺�Ʈ. 3000~3005 
			// �������� �̺�Ʈ. 3006~3010
			if(3001 <= MonID && MonID <= 3010)
			{
				// 3006 ������ 3009�������� �������� 
				// ������ 1���� ������ ����
				if(3006 <= MonID && MonID <= 3010 && nDropItemNum >= 1)
					return nDropItemNum;

				if(MonID == 3003 && nDropItemNum >= 1)
					return nDropItemNum;

				if(nDropItemNum >= MonDropMax)
					return nDropItemNum;
			}

			// �Դ� ������Ʈ
			if(3101 <= MonID && MonID <= 3500)
			{
				if(nDropItemNum >= MonDropMax)
					return nDropItemNum;
			}
		}
	}

	// �ҷ��� �̺�Ʈ. 3000~3005 
	// �������� �̺�Ʈ. 3006~3010
	if(0 == nDropItemNum && 3001 <= MonID && MonID <= 3010)
	{
		// cItemKindIndex �� 0���� �ϴ� ������ ���� A�� �⺻������ ������ �ϱ� ���ؼ��̴�.
		cItemKindIndex = 0;
		
		// ������ �ڿ� ����A�� ����߸��� �ߴµ� �� ����A�� 0 �� ���� �������̸� 
		// �̰� �ƹ��͵� ������ �ʴ� ����̶�¶�.
		// ������� �ʴ� ������������ ����
		int nDropRate = lpMonster->GetDropRate(cItemKindIndex);
		if (0 == nDropRate) 
			return 0; 

		// ������ ����ϴ� �̺�Ʈ�ε� �Ѱ��� ������ �ʾ����� 
		aryItemID[nDropItemNum] = AwardTable::CAward::GetInstance().GetAward(
			cItemKindIndex, reinterpret_cast<CMonster *>(m_pOwner), lpCharacter, aryItem + nDropItemNum);

		if (0 == aryItemID[nDropItemNum])
		{
			CMonster* pMon = reinterpret_cast<CMonster *>(m_pOwner);
			
			if(pMon)
				ERRLOG2(g_Log, "������ �������� �������� �ʾҽ��ϴ�. ����KID : %d, ���� : %d", pMon->GetKID(), cItemKindIndex);

			return 0;
		}

		++nDropItemNum;
	}


	return nDropItemNum;
}


///////////////////////////////////////////////////////////////////////////////////
// Function : CThreat::DivisionExp
//
// Description : ����ġ �й�
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
		ERRLOG0(g_Log, "����Ʈ ����Ʈ�� ��� �ֽ��ϴ�. �� ���¿����� Exp�� ���� �� �����ϴ�."); 
		return; 
	} 

	if (NULL == m_pOwner) 
	{ 
		ERRLOG0(g_Log, "�����ڰ� NULL�Դϴ�."); 
		return; 
	} 

	unsigned long lSumOfThreatAmount = 0;

    for (ThreatList::iterator it = m_ThreatList.begin(); it != m_ThreatList.end(); ++it)
    {
		lSumOfThreatAmount += it->m_lThreatAmount;
	}

	// ����Ʈ ����Ʈ�� �ִ� �༮���� �������� ����ġ ���
	AwardInfo aryIndividualAwardInfo[MAX_THREAT_TARGET];

	CAggresiveCreature* pHighestThreatCreature = m_ThreatList.back().m_pCreature;
	const unsigned char cHighestLevel = pHighestThreatCreature->GetStatus().m_nLevel;

	int nNumber = 0;
	for (ThreatList::iterator it = m_ThreatList.begin(); 
		it != m_ThreatList.end() && nNumber < MAX_THREAT_TARGET; ++it, ++nNumber)
	{
		if (nNumber >= MAX_THREAT_TARGET) 
		{ 
			ERRLOG1(g_Log, "����Ʈ ����Ʈ����, ��� �ִ� ����� ���� �̻��մϴ�. ����� ���� %d�Դϴ�.", nNumber); 
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

	// ��ȯ���� ����ġ�� �����Ϳ��� �����ش�.
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

	// ��Ƽ ������ ���� ����ġ �й�
	AwardInfo aryPartyAwardInfo[MAX_THREAT_TARGET];

	nNumber = 0;
	for (int nIndividualIndex = 0; nIndividualIndex < MAX_THREAT_TARGET; ++nIndividualIndex)
	{
		if (NULL == aryIndividualAwardInfo[nIndividualIndex].m_pCreature) 
        { 
			continue; 
		}

		// ���ʹ� ����ġ �й迡�� ���ܵȴ�.
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
				ERRLOG1(g_Log, "����Ʈ ����Ʈ����, ��� �ִ� ����� ���� �̻��մϴ�. ����� ���� %d�Դϴ�.", nNumber); 
			}
		}
	}

	// ����ġ���� ���� �����ؼ� MAX_THREAT_REWARD ��ŭ�� �����ش�.
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

		// ��Ƽ
		if (NULL != aryPartyAwardInfo[nPartyIndex].m_pParty) 
		{
			reinterpret_cast<CCharacterParty* >(aryPartyAwardInfo[nPartyIndex].m_pParty)->SendDivisionExp(
				reinterpret_cast<CCharacter *>(aryPartyAwardInfo[nPartyIndex].m_pCreature), m_pOwner, lIndividualExp, cHighestLevel);
		} 
		// ����
		else 
		{
			CCharacter *lpCharacter = reinterpret_cast<CCharacter *>(aryPartyAwardInfo[nPartyIndex].m_pCreature);
			lIndividualExp = static_cast<long>(lIndividualExp * GetAggravation(lpCharacter));

			// Threat List�� 2�� ���� ĳ������ ��� ���ݸ��� �޴´�.
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
		ERRLOG0(g_Log, "����Ʈ ����Ʈ�� ��� �ֽ��ϴ�. �� ���¿����� Exp�� ���� �� �����ϴ�."); 
		return; 
	} 

	if (NULL == m_pOwner) 
	{ 
		ERRLOG0(g_Log, "�����ڰ� NULL�Դϴ�."); 
		return; 
	} 

	unsigned long lSumOfThreatAmount = 0;

    for (ThreatList::iterator it = m_ThreatList.begin(); it != m_ThreatList.end(); ++it)
    {
		lSumOfThreatAmount += it->m_lThreatAmount;
	}

	// ����Ʈ ����Ʈ�� �ִ� �༮���� �������� ����ġ ���
	AwardInfo aryIndividualAwardInfo[MAX_THREAT_TARGET];

	CAggresiveCreature* pHighestThreatCreature = m_ThreatList.back().m_pCreature;
	const unsigned char cHighestLevel = pHighestThreatCreature->GetStatus().m_nLevel;

	int nNumber = 0;
	for (ThreatList::iterator it = m_ThreatList.begin(); 
		it != m_ThreatList.end() && nNumber < MAX_THREAT_TARGET; ++it, ++nNumber)
	{
		if (nNumber >= MAX_THREAT_TARGET) 
		{ 
			ERRLOG1(g_Log, "����Ʈ ����Ʈ����, ��� �ִ� ����� ���� �̻��մϴ�. ����� ���� %d�Դϴ�.", nNumber); 
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

	// ��ȯ���� ����ġ�� �����Ϳ��� �����ش�.
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

	// ��Ƽ ������ ���� ����ġ �й�
	AwardInfo aryPartyAwardInfo[MAX_THREAT_TARGET];

	nNumber = 0;
	for (int nIndividualIndex = 0; nIndividualIndex < MAX_THREAT_TARGET; ++nIndividualIndex)
	{
		if (NULL == aryIndividualAwardInfo[nIndividualIndex].m_pCreature) 
        { 
			continue; 
		}

		// ���ʹ� ����ġ �й迡�� ���ܵȴ�.
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
				ERRLOG1(g_Log, "����Ʈ ����Ʈ����, ��� �ִ� ����� ���� �̻��մϴ�. ����� ���� %d�Դϴ�.", nNumber); 
			}
		}
	}

	// ����ġ���� ���� �����ؼ� MAX_THREAT_REWARD ��ŭ�� �����ش�.
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

		// ��Ƽ
		if (NULL != aryPartyAwardInfo[nPartyIndex].m_pParty) 
		{
			reinterpret_cast<CCharacterParty* >(aryPartyAwardInfo[nPartyIndex].m_pParty)->SendDivisionExp(
				reinterpret_cast<CCharacter *>(aryPartyAwardInfo[nPartyIndex].m_pCreature), m_pOwner, lIndividualExp, cHighestLevel);
		} 
		// ����
		else 
		{
			CCharacter *lpCharacter = reinterpret_cast<CCharacter *>(aryPartyAwardInfo[nPartyIndex].m_pCreature);
			lIndividualExp = static_cast<long>(lIndividualExp * GetAggravation(lpCharacter));

			// Threat List�� 2�� ���� ĳ������ ��� ���ݸ��� �޴´�.
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

		// �̱��� ��ʸ�Ʈ ������������ PvP���� �� ���¡ ����å�� ��ȭ�ȴ�.
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
// Description : ����Ʈ ����Ʈ�� ũ���� �߰�
//
// Inputs : pAttackCreature - �߰��� ũ������ ������
//			lThreatAmount - ����Ʈ��
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

	// �̹� ����Ʈ�� �ִ� �༮�� ó��
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

	// ó�� ����Ʈ�� ���� �༮�� ó��
	if (0 > lThreatAmount) 
    {
        return;
    }

	size_t nThreatListSize = m_ThreatList.size();
	if (nThreatListSize > MAX_THREAT_TARGET)
	{
		ERRLOG2(g_Log, "CID:0x%08x ����Ʈ ����Ʈ ����� �ִ�ġ�� �Ѿ���ϴ�. Size:%d", 
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
// Description : ��ȣ������ ���� ����Ʈ�� ũ���� �߰�
//
// Inputs : pDefendCreature - �߰��� ũ������ ������
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
        // ThreatenedList�� ���� ���� ����.
        m_ThreatenedList.push_back(pDefendCreature);
    }
}


///////////////////////////////////////////////////////////////////////////////////
// Function : CThreat::HealThreat
//
// Description : 'Heal Threat'�� ����
//
// Inputs : pHealCreature - ������ �� ũ����
//			lThreatAmount - ����
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
// Description : ����Ʈ���� ������ �ִ� Ÿ��Ʈ, ��Ÿ��Ʈ�� ���� ���.
//
// Inputs : pTauntCreature - Ÿ��Ʈ�� ũ����
//			lThreatAmount - Ÿ��Ʈ��
//			eType - Ÿ��Ʈ/��Ÿ��Ʈ ����
//
// Outputs : None.
//
// Returns : None.
///////////////////////////////////////////////////////////////////////////////////
void CThreat::AffectThreat(CAggresiveCreature* pTauntCreature, long lDamage, AffectThreatType eType)
{
	// ����Ÿ�� ����Ʈ�� ������ ���� �ʴ´�.
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
		ERRLOG1(g_Log, "Ÿ��Ʈ�ϴ� ������ ����Ʈ ����Ʈ�� �̻��մϴ�. ����Ʈ ������ : %d", wThreatNum);
	}

	AddToThreatList(pTauntCreature, lThreatAmount * wThreatNum);
}


///////////////////////////////////////////////////////////////////////////////////
// Function : CThreat::DeleteThreatened
//
// Description : ��ȣ������ ���� ����Ʈ���� ũ���ĸ� ����
//
// Inputs : pDefendCreature - ������ ũ������ ������
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
// Description : ����Ʈ ����Ʈ���� ũ���ĸ� ����
//
// Inputs : pAttackCreature - ������ ũ������ ������
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
// Description : ����Ʈ ����Ʈ�� �˻��� Ÿ���� ���Ѵ�.
//
// Inputs : None.
//
// Outputs : None.
//
// Returns : CAggresiveCreature* - Ÿ�� ũ����
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
			ERRLOG1(g_Log, "CID:0x%08x ���� ����� ã�� ���� ������Ʈ�� �������� �ʽ��ϴ�.", m_pOwner->GetCID());
			return NULL;
		}

		if (!lpSiegeObject->IsGuard() && !lpSiegeObject->IsEmblem() && !lpSiegeObject->IsCamp())
		{
			ERRLOG2(g_Log, "CID:0x%08x wObjectType:%d ���� ����� ã�� ���� ������Ʈ�� ����, ��¡��, ����� �ƴմϴ�.",
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

	ERRLOG1(g_Log, "CID:0x%08x ���� Ȥ�� ����, �� ��¡��, ����� �ƴ� �༮�� ���� Ÿ���� ã���ϴ�.", m_pOwner->GetCID());
	return NULL;
}


///////////////////////////////////////////////////////////////////////////////////
// Function : CThreat::GetAggravation
//
// Description : ������ ��� (����ġ �й��)
//
// Inputs : pCreature - ���� ũ����
//
// Outputs : None.
//
// Returns : float - ������
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
		// ����Ʈ ����Ʈ�� ����ִ� ũ���� �� ���� ������ ���� ũ���İ� 
		// '�÷��̾�'�� '��ȯ��'�� ��쿡�� �������� ����Ѵ�.
		return 0;
	}

	int nRandPoint = 0;
	int nGradeSpace = 0;
	int nLevelDifferenceForArray = 0;

	// ������ ������ ������ �ִ뷹���� ���� �����̸� ���Ѵ�.
	const int nLevelDifference = m_pOwner->CalculateLevelGap(m_pOwner->GetStatus().m_nLevel, m_cMaxLevel);
	// ������+20 �����ش�.
	nLevelDifferenceForArray = nLevelDifference + CAggresiveCreature::MAX_LEVEL_GAP;

	ThreatInfo HighestThreatInfo = m_ThreatList.back();

	if (NULL == HighestThreatInfo.m_pCreature)
	{
		ERRLOG0(g_Log, "����Ʈ ����Ʈ�� ž�� ũ���� �����Ͱ� �����ϴ�.");
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

	// �ۼ�Ʈ�� 0�̸� �ƹ��͵� �����Ѵ�. �׳� ����
	fPt = lpCharacter->GetAwardPer();
	if(fPt == 0.0f)
		return 0;

	bool bPremiumTime = lpCharacter->CheckPremiumTime();

	*dwOwnerID = lpCharacter->GetCID();

	// ���� ������ ���� ĳ������ ��Ƽ�� �߿�, ������ �����߰� '����� ����'�� ����ߴٸ� ������� ������ �ش�.
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

	// '����� ����' ����
	if (true == bLuckyOrb)
	{
		fAddDropPt += 0.3f;
	}
	
	// �����̾� ����
	if(bPremiumTime)
	{
		fAddDropPt += lpCharacter->GetPremiumPt();
	}

	// '���� ����' ����.
	if (true == lpCharacter->GetEnchantInfo().GetFlag(Skill::SpellID::WealthStatue))
	{			
		unsigned short wLevel = lpCharacter->GetEnchantLevel(Skill::SpellID::WealthStatue);
		fAddDropPt += (wLevel * 0.01f);
	}

	// �����ǵ�� Ȯ���� ���Ѵ�.
	unsigned char cItemKindIndex = 0;
	int nTotalRate = 0;
	for (cItemKindIndex = 0; cItemKindIndex < MonsterInfo::MAX_AWARD_KIND; ++cItemKindIndex) 
    {
		// ��ũ��Ʈ�� ���ǵ� �����ۺ� �����ġ�� ��� ���Ѵ�.
		nTotalRate += lpMonster->GetDropRate(cItemKindIndex);
	}

	// �ҷ��� �̺�Ʈ�� �� �Դ� ������Ʈ�� (�Դ��� �ִ� 3������ ������)
	int MonID = lpMonster->GetKID();
	int MonDropMax = 1+Math::Random::ComplexRandom(2);

	bool bAllInOne = false;

	if(3006 <= MonID && MonID <= 3009)
		bAllInOne = true;
	else if(3101 <= MonID && MonID <= 3500)
		bAllInOne = true;	// �Դ� ������Ʈ

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
			// ���� ����Ʈ ���ʽ��� ���� �߰� �����
			char eliteBonus = lpCharacter->GetEliteBonus();
			if (0 < eliteBonus)
			{
				wDropRate = EliteBonus::usBonusItemDropRate[eliteBonus - 1][nDropItemIndex - AwardTable::MAX_DROP_ITEM];
			}
		}

		// �߰������ �߰�.
		if(fAddDropPt > 0.0f)
		{
			wDropRate += static_cast<unsigned short>(wDropRate * fAddDropPt);
		}

		wDropRate = (unsigned short)(wDropRate*(CServerSetup::GetInstance().GetDropDefault() / 100.0f)*fPt);

		// fPt�� ������ ������ fPt ���� ���� ������� �޶�����.
		// CServerSetup::GetInstance().GetDropDefault() = 100 ��.
		// 
		if (!bAllInOne && Math::Random::ComplexRandom(1000) > wDropRate) 
		{ 
			continue;
		}

		// ���� �� �����ġ�� 10���̸� 10���� ������ ����� ��Ų��.
		nRandPoint = Math::Random::ComplexRandom(nTotalRate);

		for (cItemKindIndex = 0; cItemKindIndex < MonsterInfo::MAX_AWARD_KIND; ++cItemKindIndex)
		{
			// ������� �ʴ� ������������ ����
			int nDropRate = lpMonster->GetDropRate(cItemKindIndex);
			if (0 == nDropRate) { continue; }

			// �������� ��� ����.
			// �� �������� ������� �����ش�.
			nGradeSpace += nDropRate;

			// �������� ���� ������. ������ �������� �����ġ�� �� ������
			// ����� �Ѵ�.
			if (nGradeSpace > nRandPoint)
			{
				aryItemID[nDropItemNum] = AwardTable::CAward::GetInstance().GetAward(
					cItemKindIndex, reinterpret_cast<CMonster *>(m_pOwner), lpCharacter, aryItem + nDropItemNum);

				if (0 == aryItemID[nDropItemNum])
				{
					CMonster* pMon = reinterpret_cast<CMonster *>(m_pOwner);
					
					if(pMon)
						ERRLOG2(g_Log, "������ �������� �������� �ʾҽ��ϴ�. ����KID : %d, ���� : %d", pMon->GetKID(), cItemKindIndex);
				}
				else
				{
					++nDropItemNum;
				}

				// �ʱ�ȭ.
				cItemKindIndex = MonsterInfo::MAX_AWARD_KIND;
				nGradeSpace = 0;
			} 

			// �ҷ��� �̺�Ʈ. 3000~3005 
			// �������� �̺�Ʈ. 3006~3010
			if(3001 <= MonID && MonID <= 3010)
			{
				// 3006 ������ 3009�������� �������� 
				// ������ 1���� ������ ����
				if(3006 <= MonID && MonID <= 3010 && nDropItemNum >= 1)
					return nDropItemNum;

				if(MonID == 3003 && nDropItemNum >= 1)
					return nDropItemNum;

				if(nDropItemNum >= MonDropMax)
					return nDropItemNum;
			}

			// �Դ� ������Ʈ
			if(3101 <= MonID && MonID <= 3500)
			{
				if(nDropItemNum >= MonDropMax)
					return nDropItemNum;
			}
		}
	}

	// �ҷ��� �̺�Ʈ. 3000~3005 
	// �������� �̺�Ʈ. 3006~3010
	if(0 == nDropItemNum && 3001 <= MonID && MonID <= 3010)
	{
		// cItemKindIndex �� 0���� �ϴ� ������ ���� A�� �⺻������ ������ �ϱ� ���ؼ��̴�.
		cItemKindIndex = 0;
		
		// ������ �ڿ� ����A�� ����߸��� �ߴµ� �� ����A�� 0 �� ���� �������̸� 
		// �̰� �ƹ��͵� ������ �ʴ� ����̶�¶�.
		// ������� �ʴ� ������������ ����
		int nDropRate = lpMonster->GetDropRate(cItemKindIndex);
		if (0 == nDropRate) 
			return 0; 

		// ������ ����ϴ� �̺�Ʈ�ε� �Ѱ��� ������ �ʾ����� 
		aryItemID[nDropItemNum] = AwardTable::CAward::GetInstance().GetAward(
			cItemKindIndex, reinterpret_cast<CMonster *>(m_pOwner), lpCharacter, aryItem + nDropItemNum);

		if (0 == aryItemID[nDropItemNum])
		{
			CMonster* pMon = reinterpret_cast<CMonster *>(m_pOwner);
			
			if(pMon)
				ERRLOG2(g_Log, "������ �������� �������� �ʾҽ��ϴ�. ����KID : %d, ���� : %d", pMon->GetKID(), cItemKindIndex);

			return 0;
		}

		++nDropItemNum;
	}


	return nDropItemNum;
}


///////////////////////////////////////////////////////////////////////////////////
// Function : CThreat::DivisionExp
//
// Description : ����ġ �й�
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
		ERRLOG0(g_Log, "����Ʈ ����Ʈ�� ��� �ֽ��ϴ�. �� ���¿����� Exp�� ���� �� �����ϴ�."); 
		return; 
	} 

	if (NULL == m_pOwner) 
	{ 
		ERRLOG0(g_Log, "�����ڰ� NULL�Դϴ�."); 
		return; 
	} 

	unsigned long lSumOfThreatAmount = 0;

    for (ThreatList::iterator it = m_ThreatList.begin(); it != m_ThreatList.end(); ++it)
    {
		lSumOfThreatAmount += it->m_lThreatAmount;
	}

	// ����Ʈ ����Ʈ�� �ִ� �༮���� �������� ����ġ ���
	AwardInfo aryIndividualAwardInfo[MAX_THREAT_TARGET];

	CAggresiveCreature* pHighestThreatCreature = m_ThreatList.back().m_pCreature;
	const unsigned char cHighestLevel = pHighestThreatCreature->GetStatus().m_nLevel;

	int nNumber = 0;
	for (ThreatList::iterator it = m_ThreatList.begin(); 
		it != m_ThreatList.end() && nNumber < MAX_THREAT_TARGET; ++it, ++nNumber)
	{
		if (nNumber >= MAX_THREAT_TARGET) 
		{ 
			ERRLOG1(g_Log, "����Ʈ ����Ʈ����, ��� �ִ� ����� ���� �̻��մϴ�. ����� ���� %d�Դϴ�.", nNumber); 
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

	// ��ȯ���� ����ġ�� �����Ϳ��� �����ش�.
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

	// ��Ƽ ������ ���� ����ġ �й�
	AwardInfo aryPartyAwardInfo[MAX_THREAT_TARGET];

	nNumber = 0;
	for (int nIndividualIndex = 0; nIndividualIndex < MAX_THREAT_TARGET; ++nIndividualIndex)
	{
		if (NULL == aryIndividualAwardInfo[nIndividualIndex].m_pCreature) 
        { 
			continue; 
		}

		// ���ʹ� ����ġ �й迡�� ���ܵȴ�.
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
				ERRLOG1(g_Log, "����Ʈ ����Ʈ����, ��� �ִ� ����� ���� �̻��մϴ�. ����� ���� %d�Դϴ�.", nNumber); 
			}
		}
	}

	// ����ġ���� ���� �����ؼ� MAX_THREAT_REWARD ��ŭ�� �����ش�.
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

		// ��Ƽ
		if (NULL != aryPartyAwardInfo[nPartyIndex].m_pParty) 
		{
			reinterpret_cast<CCharacterParty* >(aryPartyAwardInfo[nPartyIndex].m_pParty)->SendDivisionExp(
				reinterpret_cast<CCharacter *>(aryPartyAwardInfo[nPartyIndex].m_pCreature), m_pOwner, lIndividualExp, cHighestLevel);
		} 
		// ����
		else 
		{
			CCharacter *lpCharacter = reinterpret_cast<CCharacter *>(aryPartyAwardInfo[nPartyIndex].m_pCreature);
			lIndividualExp = static_cast<long>(lIndividualExp * GetAggravation(lpCharacter));

			// Threat List�� 2�� ���� ĳ������ ��� ���ݸ��� �޴´�.
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

		// �̱��� ��ʸ�Ʈ ������������ PvP���� �� ���¡ ����å�� ��ȭ�ȴ�.
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