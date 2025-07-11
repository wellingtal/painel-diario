
#include "stdafx.h"

#include <Network/ClientSocket/ClientConstants.h>
#include <Network/Dispatch/GameClient/GameClientDispatch.h>
#include <Network/Dispatch/GameClient/SendCharEtc.h>

#include <Creature/Character/Character.h>
#include <Creature/Monster/Monster.h>
#include <Creature/Monster/MonsterStructure.h>
#include <Creature/Monster/VirtualMonsterMgr.h>

#include <Community/Party/Party.h>
#include <Map/FieldMap/CellManager.h>
#include <Map/FieldMap/VirtualArea/VirtualArea.h>
#include <Map/FieldMap/VirtualArea/VirtualAreaMgr.h>

#include "SkillTable.h"

#define CHECK_SKILL_ARGUMENT_IS_NULL(pSkillUser_In, pVictim_In) \
	if (0 == (pSkillUser_In) || 0 == (pVictim_In)) { \
	ERRLOG2(g_Log, "스킬 사용자가 잘못되었습니다. 스킬 사용자는 %p이고 희생자는 %p입니다.", \
	pSkillUser_In, pVictim_In); \
	return 0; \
	}


using namespace std;
using namespace Skill;

CProcessTable   CProcessTable::ms_this;
ProtoType       CProcessTable::ProcessInfo::m_NullProtoType;

CProcessTable::CProcessTable()
:	m_usProcessInfo(0), m_fnProcessTable(NULL)
{

}

CProcessTable::~CProcessTable()
{
	if (0 != m_fnProcessTable)
	{
		delete [] m_fnProcessTable;
		m_fnProcessTable = NULL;
	}
}


bool CProcessTable::Initialize()
{
	ProcessVector   tempProcessInfo;

#define INSERT_SKILL(usSkill_ID, FnFunction) \
	if (!InsertSkill(tempProcessInfo, (usSkill_ID), CFunctions::##FnFunction)) { return false; }

	// 소설모션
	INSERT_SKILL(0x4001, SocialMotion);
	INSERT_SKILL(0x4002, SocialMotion);
	INSERT_SKILL(0x4003, SocialMotion);
	INSERT_SKILL(0x4004, SocialMotion);


	// 파이터
	INSERT_SKILL(0x8101, Net);

	INSERT_SKILL(0x8102, SwordMastery);
	INSERT_SKILL(0x8103, AxeMastery);
	INSERT_SKILL(0x8104, HardHit);
	INSERT_SKILL(0x8105, BluntMastery);
	INSERT_SKILL(0x8106, WillPower);
	INSERT_SKILL(0x8107, Toughness);

	
	// 파이터
	INSERT_SKILL(0x8101, Net);

	INSERT_SKILL(0x8102, SwordMastery);
	INSERT_SKILL(0x8103, AxeMastery);
	INSERT_SKILL(0x8104, HardHit);
	INSERT_SKILL(0x8105, BluntMastery);
	INSERT_SKILL(0x8106, WillPower);
	INSERT_SKILL(0x8107, Toughness);

	// 로그
	INSERT_SKILL(0x8201, Detection);

	INSERT_SKILL(0x8202, ImprovedCritical);
	INSERT_SKILL(0x8203, Evasion);
	INSERT_SKILL(0x8205, NeedleSpit);

	// 메이지
	INSERT_SKILL(0x8302, BloodyMana);
	INSERT_SKILL(0x8303, VampiricTouch);
	INSERT_SKILL(0x8304, ManaShell);
	INSERT_SKILL(0x8305, Grease);
	INSERT_SKILL(0x8306, Serenity);

	// 어콜라이트
	INSERT_SKILL(0x8401, Purification);

	INSERT_SKILL(0x8402, Faith);
	INSERT_SKILL(0x8404, Encourage);
	INSERT_SKILL(0x8405, FirstAid);
	INSERT_SKILL(0x8406, HammerOfLight);

	// 디펜더
	INSERT_SKILL(0x8501, Charging);

	INSERT_SKILL(0x8503, SharedPain);
	INSERT_SKILL(0x8504, FullSwing);

	// 워리어
	INSERT_SKILL(0x8601, Blaze);

	INSERT_SKILL(0x8602, BattleSong);
	INSERT_SKILL(0x8603, ChainAction);
	INSERT_SKILL(0x8604, DualWieldMastery);
	// 사용안함
	INSERT_SKILL(0x8605, CounterAttack);
	INSERT_SKILL(0x8606, CounterAttackMotion);

	// 어쌔신
	INSERT_SKILL(0x8702, BackStab);
	INSERT_SKILL(0x8704, Stealth);
	INSERT_SKILL(0x8705, DaggerMastery);
	INSERT_SKILL(0x8706, DualWieldMastery);
	// 사용안함
	INSERT_SKILL(0x8707, BombSet);
	INSERT_SKILL(0x8708, Explosion);

	// 추가스킬 
	INSERT_SKILL(0x8709, StealHand);


	// 아처
	INSERT_SKILL(0x8803, AimedShot);
	INSERT_SKILL(0x8804, DualShot);
	INSERT_SKILL(0x8805, Camouflage);
	INSERT_SKILL(0x8806, Accuracy);

	// 소서러
	INSERT_SKILL(0x8901, Recall);

	INSERT_SKILL(0x8902, FireBolt);
	INSERT_SKILL(0x8903, LightningArrow);
	INSERT_SKILL(0x8904, FrostBolt);
	INSERT_SKILL(0x8905, DeathRay);
	// 사용안함
	INSERT_SKILL(0x8906, FreshToStone);
	INSERT_SKILL(0x8907, FreshToStone);

	// 엔찬터
	INSERT_SKILL(0x8A01, Disenchant);
	INSERT_SKILL(0x8A07, Recall);

	INSERT_SKILL(0x8A02, Entangle);
	INSERT_SKILL(0x8A04, LowerStrength);
	INSERT_SKILL(0x8A05, EnchantWeapon);
	INSERT_SKILL(0x8A06, Shatter);

	// 프리스트
	INSERT_SKILL(0x8B01, Resurrection);

	INSERT_SKILL(0x8B02, MaintenanceChant);
	INSERT_SKILL(0x8B03, AccelerationChant);
	INSERT_SKILL(0x8B04, BrightArmor);
	INSERT_SKILL(0x8B05, WoundsCrafting);	// WoundsMake 따라옴
	INSERT_SKILL(0x8B06, ManaFlow);
	INSERT_SKILL(0x8B07, ManaBurn);

	INSERT_SKILL(0x8B08, WoundsMake);

	// 클레릭
	INSERT_SKILL(0x8C01, Resurrection);

	INSERT_SKILL(0x8C02, Dazzle);			// CureLight 따라옴
	INSERT_SKILL(0x8C03, CureWounds);
	INSERT_SKILL(0x8C04, WoundsCrafting);	// WoundsMake 따라옴
	INSERT_SKILL(0x8C08, LifeAura);

	INSERT_SKILL(0x8C05, CureLight);
	INSERT_SKILL(0x8C06, WoundsMake);
	INSERT_SKILL(0x8C07, Regeneration);	// 사용안함


	// 컴배턴트
	INSERT_SKILL(0x9102, CrusherMastery);
	INSERT_SKILL(0x9104, HardHit);
	INSERT_SKILL(0x9105, BladeMastery);
	INSERT_SKILL(0x9106, SplitLife);
	INSERT_SKILL(0x9107, ExtraLife);
	INSERT_SKILL(0x9108, ImprovedCritical);
	INSERT_SKILL(0x9109, WillPower);
	INSERT_SKILL(0x9110, Toughness);
	INSERT_SKILL(0x9303, HardenSkin);

	// 오피세이터
	INSERT_SKILL(0x9201, Dispel);

	INSERT_SKILL(0x9202, MagicMissile);
	INSERT_SKILL(0x9204, Flexibility);
	INSERT_SKILL(0x9206, BloodyMana);
	INSERT_SKILL(0x9207, ClawMastery);
	INSERT_SKILL(0x9208, Grease);
	INSERT_SKILL(0x9209, LifeAura);
	INSERT_SKILL(0x9210, Encourage);
	INSERT_SKILL(0x9212, Serenity);
	INSERT_SKILL(0x9213, Evasion);
	INSERT_SKILL(0x9214, InnerSight);

	// 템플러
	INSERT_SKILL(0x9301, Net);
	INSERT_SKILL(0x9308, Charging);

	INSERT_SKILL(0x9302, Guard);
	INSERT_SKILL(0x9305, FastHit);
	INSERT_SKILL(0x9307, ManaConvert);

	// 어태커
	INSERT_SKILL(0x9401, Net);
	INSERT_SKILL(0x9408, Blaze);

	INSERT_SKILL(0x9402, PowerDrain);
	INSERT_SKILL(0x9404, BattleSong);
	INSERT_SKILL(0x9405, SkillArmFight);
	INSERT_SKILL(0x9407, RingGeyser);

	// 거너
	INSERT_SKILL(0x9501, Detection);

	INSERT_SKILL(0x9502, FireRing);
	INSERT_SKILL(0x9504, Camouflage);
	INSERT_SKILL(0x9506, Blast);

	// 룬오프
	INSERT_SKILL(0x9601, Recall);

	INSERT_SKILL(0x9602, Rot);
	INSERT_SKILL(0x9603, Shock);
	INSERT_SKILL(0x9604, Shackle);
	INSERT_SKILL(0x9605, Crevice);
	INSERT_SKILL(0x9606, EnchantWeapon);

	INSERT_SKILL(0x9607, SummonKindling);	// 몬스터 소환시 버그생김(몬스터가 소환안됨)

	// 라이프오프
	INSERT_SKILL(0x9701, Resurrection);
	INSERT_SKILL(0x9702, Purification);

	INSERT_SKILL(0x9703, WoundsCrafting);
	INSERT_SKILL(0x9704, CureWounds);
	INSERT_SKILL(0x9706, Flash);
	INSERT_SKILL(0x9708, MaintenanceChant);
	INSERT_SKILL(0x9709, AccelerationChant);

	INSERT_SKILL(0x9707, WoundsMake);	// 운즈 그래프트 연계스킬

	// 쉐도우오프
	INSERT_SKILL(0x9801, Detection);

	INSERT_SKILL(0x9802, WoundsBlast);
	INSERT_SKILL(0x9803, SkillArmFight);
	INSERT_SKILL(0x9804, Stealth);
	INSERT_SKILL(0x9805, DaggerFire);
	// 사용안함
	INSERT_SKILL(0x9806, Envenom);

	// 추가스킬 
	INSERT_SKILL(0x9807, StealHand);


	// 포션용 스킬 New
	INSERT_SKILL(0x8D01, UseFood);
	//	INSERT_SKILL(0x8D02, UseFoodPer);

	INSERT_SKILL(0x8D03, UseDrink);
	//	INSERT_SKILL(0x8D04, UseDrinkPer);

	INSERT_SKILL(0x8D05, UseFusionFood);
	//	INSERT_SKILL(0x8D06, UseFusionFoodPer);

	//	INSERT_SKILL(0x8D11, UseHealPotion);
	INSERT_SKILL(0x8D12, UseHealPotionPer);

	//	INSERT_SKILL(0x8D13, UseManaPotion);
	INSERT_SKILL(0x8D14, UseManaPotionPer);

	//	INSERT_SKILL(0x8D15, UseRefreshPotion);
	INSERT_SKILL(0x8D16, UseRefreshPotionPer);

	// 버프포션
//	INSERT_SKILL(0x9901, DemagePotion);
	INSERT_SKILL(0x9902, DemagePotionPer);

	//	INSERT_SKILL(0x9903, ArmorPotion);
	INSERT_SKILL(0x9904, ArmorPotionPer);

	//	INSERT_SKILL(0x9905, HitRatePotion);
	INSERT_SKILL(0x9906, HitRatePotionPer);

	//	INSERT_SKILL(0x9907, EvadePotion);
	INSERT_SKILL(0x9908, EvadePotionPer);

	//	INSERT_SKILL(0x9909, MaxHPPotion);
	INSERT_SKILL(0x9910, MaxHPPotionPer);

	//	INSERT_SKILL(0x9911, MaxMPPotion);
	INSERT_SKILL(0x9912, MaxMPPotionPer);

	//	INSERT_SKILL(0x9913, HPRegenPotion);
	//	INSERT_SKILL(0x9914, HPRegenPotionPer);

	//	INSERT_SKILL(0x9915, MPRegenPotion);
	//	INSERT_SKILL(0x9916, MPRegenPotionPer);

	//	INSERT_SKILL(0x9917, CriticalPotion);
	INSERT_SKILL(0x9918, CriticalPotionPer);

	//	INSERT_SKILL(0x9919, BlockPotion);
	INSERT_SKILL(0x9920, BlockPotionPer);

	//	INSERT_SKILL(0x9921, SpeedPotion);
	INSERT_SKILL(0x9921, SpeedPotionPer);

	//	INSERT_SKILL(0x9923, DeCoolDownPotion);
	INSERT_SKILL(0x9922, DeCoolDownPotionPer);

	//	INSERT_SKILL(0x9923, MagicPowerPotion);
	INSERT_SKILL(0x9924, MagicPowerPotionPer);

	//	INSERT_SKILL(0x9925, MagicResistPotion);
	INSERT_SKILL(0x9926, MagicResistPotionPer);

	INSERT_SKILL(0x9927, ConcentrationPotionPer);

	// 폭죽
	INSERT_SKILL(0x8F01, FireCracker);
	INSERT_SKILL(0x8F02, FireCracker);
	INSERT_SKILL(0x8F03, FireCracker);
	INSERT_SKILL(0x8F04, FireCracker);
	INSERT_SKILL(0x8F05, FireCracker);
	INSERT_SKILL(0x8F06, FireCracker);
	INSERT_SKILL(0x8F07, FireCracker);
	INSERT_SKILL(0x8F08, FireCracker);
	INSERT_SKILL(0x8F09, ChinaFireCracker);

	// 기타
	INSERT_SKILL(0x990A, Drunk);
	INSERT_SKILL(0x990B, LuckyExpOrb);

	INSERT_SKILL(0x990C, MichaelBless);

	INSERT_SKILL(0x990D, ExpOrb);
	INSERT_SKILL(0x990E, LuckyOrb);

	// 리제렉션으로 사용 누가 사용하는거냐?
	INSERT_SKILL(0x990F, Resurrection);

	// 디버프 포션
	INSERT_SKILL(0x99A1, AntidotePotion);	// 해독제 (독해독)
	INSERT_SKILL(0x99A2, AntiFreeze);		// 냉기 방지
	INSERT_SKILL(0x99A3, AntiFire);			// 화염 방지




	// 공성 병기 스킬
	INSERT_SKILL(0x9A01, SRSAAttack);
	INSERT_SKILL(0x9A02, LRSAAttack);
	INSERT_SKILL(0x9A03, SRCAAttack);
	INSERT_SKILL(0x9A04, LRCAAttack);
	INSERT_SKILL(0x9A05, GuardAttack);
	INSERT_SKILL(0x9A06, EmblemAttack);
	INSERT_SKILL(0x9A07, CampAttack);

	// 상징물 스킬(수정 되었음 0x9A06 사용안함).
	INSERT_SKILL(0x9A08, EmblemNewAttack);

	// 게더
	INSERT_SKILL(0x9E01, Gather); // 일반 채집
	INSERT_SKILL(0x9E02, Gather); // 어빌리티 채집
	INSERT_SKILL(0x9E03, Gather); // 일반 채굴
	INSERT_SKILL(0x9E04, Gather); // 어빌리티 채굴




	// 스피드버프(챈트)
	if (!InsertSkill(tempProcessInfo, (0x9F01), NULL)) { return false; }

	// 쿨타임감소(범위형)
	if (!InsertSkill(tempProcessInfo, (0x9F02), NULL)) { return false; }

	// 마나셀(범위형)
	if (!InsertSkill(tempProcessInfo, (0x9F03), NULL)) { return false; }

	m_usProcessInfo = static_cast<unsigned short>(tempProcessInfo.size());
	m_fnProcessTable = new ProcessInfo[m_usProcessInfo];
	if (NULL == m_fnProcessTable)
	{
		ERRLOG0(g_Log, "메모리가 부족해서 스킬 처리 테이블을 초기화 할 수 없습니다.");
		return false;
	}

	std::sort(tempProcessInfo.begin(), tempProcessInfo.end());
	std::copy(tempProcessInfo.begin(), tempProcessInfo.end(), m_fnProcessTable);    
	return true;
}


bool CProcessTable::InsertSkill(CProcessTable::ProcessVector& processVector,
								unsigned short usSkill_ID, FnProcess fnProcess)
{
	const ProtoType* lpProtoType = CSkillMgr::GetInstance().GetSkillProtoType(usSkill_ID);
	if (NULL == lpProtoType)
	{
		ERRLOG1(g_Log, "다음 스킬의 추가에 실패했습니다. ID:0x%04x : 스킬 스크립트에 존재하지 않는 스킬입니다.", usSkill_ID);
		return false;
	}

	processVector.push_back(ProcessInfo(usSkill_ID, fnProcess, const_cast<ProtoType *>(lpProtoType)));
	return true;
}


const CProcessTable::ProcessInfo* CProcessTable::GetProcessInfo(unsigned short usSkill_ID)
{
	ProcessInfo* lpFirst = m_fnProcessTable;
	ProcessInfo* lpLast  = m_fnProcessTable + m_usProcessInfo;
	ProcessInfo* lpMid   = NULL;

	size_t nCount   = m_usProcessInfo;
	size_t nCount2  = 0;

	for (; 0 < nCount; )
	{
		nCount2 = nCount / 2;
		lpMid = lpFirst + nCount2;

		if (lpMid->m_usSkill_ID < usSkill_ID)
		{
			lpFirst = ++lpMid, nCount -= nCount2 + 1;
		}
		else
		{
			nCount = nCount2;
		}
	}

	return (lpFirst != lpLast && !(usSkill_ID < lpFirst->m_usSkill_ID)) ? lpFirst : NULL;
}

//check if the character have enough mp to use the skill
//returns :
// -1 if not
// 0 ??
// nSkillMP if it drained the MP
short Skill::CFunctions::ConsumeMP(AtType attackType, CAggresiveCreature* lpSkillUser, unsigned char cOffencerJudge) 
{
	// 스킬 아이템 사용시에는 MP를 소모하지 않습니다.
	if (ClientConstants::Judge_SkillItem == cOffencerJudge)
	{
		return 0;
	}

	if (attackType.m_cSkillLockCount < CSkillMgr::MAX_SKILL_LOCKCOUNT)
	{
		const Skill::ProtoType* pProtoType = CSkillMgr::GetInstance().GetSkillProtoType(attackType.m_wType);
		if (NULL != pProtoType)
		{
			const short wLockCount = (true == pProtoType->m_bIsClassSkill) ? 0 : lpSkillUser->GetSkillLockCount(attackType.m_wType);
			short nSkillMP = pProtoType[attackType.m_cSkillLockCount].m_StartMP + pProtoType[wLockCount].m_LockMP + 
				(pProtoType[attackType.m_cSkillLockCount].m_LevelMP + pProtoType[wLockCount].m_LockMP) * attackType.m_cSkillLevel;

			if (lpSkillUser->m_CreatureStatus.m_nNowMP >= nSkillMP)
			{
				if (0 >= lpSkillUser->GetConsumeMPCount())
				{
					lpSkillUser->m_CreatureStatus.m_nNowMP -= nSkillMP;
				}

				return nSkillMP;
			}
		}
	}

	return -1;
}


//-------------------------------------------------------------------------------------------------
// 파이터 스킬
//hz abilities define
DEFINE_SKILL(TrainedRider)
{
	return 0;
}
DEFINE_SKILL(MasterThief)
{
	return 0;
}
DEFINE_SKILL(Explorer)
{
	return 0;
}
DEFINE_SKILL(IronStomach)
{
	return 0;
}
//Permissions:
DEFINE_SKILL(Chemical)
{
	return 0;
}
DEFINE_SKILL(Miner)
{
	return 0;
}
DEFINE_SKILL(HerbCollector)
{
	return 0;
}
DEFINE_SKILL(JewelryCrafter)
{
	return 0;
}
DEFINE_SKILL(Refiner)
{
	return 0;			
}
DEFINE_SKILL(Botanist)
{
	return 0;			
}
DEFINE_SKILL(GemCrafter)
{
	return 0;			
}
DEFINE_SKILL(Cook)
{
	return 0;			
}
//Normal Abilities:
DEFINE_SKILL(BardsMelody)
{
	return 0;			
}
DEFINE_SKILL(CripplingStrike)
{
	return 0;			
}
DEFINE_SKILL(TitansStrenght)
{
	return 0;			
}
DEFINE_SKILL(MagicStrenght)
{
	return 0;			
}
DEFINE_SKILL(StickyMagic)
{
	return 0;			
}
DEFINE_SKILL(Swiftness)
{
	return 0;			
}
DEFINE_SKILL(Celerity)
{
	return 0;			
}
DEFINE_SKILL(ArmorBreaker)
{
	return 0;			
}
DEFINE_SKILL(MagicStrike)
{
	return 0;			
}
DEFINE_SKILL(IceBreaker)
{
	return 0;			
}
DEFINE_SKILL(PoisonBreaker)
{
	return 0;			
}
DEFINE_SKILL(StunBreaker)
{
	return 0;			
}
DEFINE_SKILL(Scout)
{
	return 0;			
}
DEFINE_SKILL(BuffMastery)
{
	return 0;			
}
DEFINE_SKILL(IronSkin)
{
	return 0;			
}
DEFINE_SKILL(WeaponBreaker)
{
	return 0;
}

DEFINE_SKILL(StunMastery)
{
	return 0;
}
//hz end
DEFINE_SKILL(Net)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

	// 몬스터/공성병기는 무조건 저항
	const unsigned char cCreatureType = Creature::GetCreatureType(lpVictim->GetCID());
	if (Creature::CT_MONSTER == cCreatureType ||
		Creature::CT_SUMMON == cCreatureType ||
		Creature::CT_STRUCT == cCreatureType ||
		Creature::CT_SIEGE_OBJECT == cCreatureType)//check what kind of a enemy is that
	{
		cDefenserJudge = ClientConstants::Judge_Resist;
		return 0; 
	}

	// edith 2009.06.13 넷 유지시간 증가 
//	unsigned short wResult = CAddSpell<CHoldSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, SpellType::PHYSICAL_SPELL, SpellID::Hold, 
//		lpSkillUser->GetStatus().m_nLevel, static_cast<unsigned long>(6 * LevelFactor(lpSkillUser, lpVictim))))(lpVictim);
	unsigned short wResult = CAddSpell<CHoldSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, SpellType::PHYSICAL_SPELL, SpellID::Hold, 
		lpSkillUser->GetStatus().m_nLevel, static_cast<unsigned long>(12 * LevelFactor(lpSkillUser, lpVictim))))(lpVictim);

	switch (wResult)
	{
		case CSpell::NO_ENCHANT_ERROR :
		case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
			cDefenserJudge = ClientConstants::Judge_Enchant;
			break;

		case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
			wError = PktAtAck::FAIL_SPELL;
			return 0;

		case CSpell::ENCHANT_FAIL_BY_RESIST :
			cDefenserJudge = ClientConstants::Judge_Resist;
			break;
	}

	lpVictim->GetThreat().AddToThreatList(lpSkillUser, 1);
	if (Creature::CT_MONSTER == cCreatureType ||
		Creature::CT_SUMMON == cCreatureType ||
		Creature::CT_STRUCT == cCreatureType)
	{
		CMonster* lpMonster = reinterpret_cast<CMonster *>(lpVictim);
		lpMonster->Attacked();
	}
	
	return 0;
}


DEFINE_SKILL(SwordMastery)
{
	return 0;
}


DEFINE_SKILL(AxeMastery)
{
	return 0;
}

//hayzohar added a new skill wand mastery
DEFINE_SKILL(WandMastery)
{
	return 0;
}
//hayzohar end


DEFINE_SKILL(HardHit)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

	float fAddRate = 100.0f;
	unsigned short wLockCount = GetSkillLockCount(lpSkillUser, attackType);
	unsigned short wResult = CSpell::NO_ENCHANT_ERROR ;

	switch (lpSkillUser->GetStatus().m_StatusInfo.m_cCriticalType)
	{
		case Item::Attribute::SWORD_TYPE:
		{
			fAddRate = (150 + 15 * (attackType.m_cSkillLevel + wLockCount * 6)) / 100.0f;
			break;
		} 

		case Item::Attribute::AXE_TYPE:
		{
			fAddRate = (150 + 10 * (attackType.m_cSkillLevel + wLockCount * 6)) / 100.0f;

			
			unsigned short wEnchantLevel = (attackType.m_cSkillLevel*3) + (wLockCount*24);
			unsigned long dwDurationSec = static_cast<unsigned long>(20 * LevelFactor(lpSkillUser, lpVictim));

			wResult = CAddSpell<CArmorBrokenSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
				SpellType::MAGICAL_SPELL, SpellID::ArmorBroken, wEnchantLevel, dwDurationSec))(lpVictim);
			break;
		} 

		case Item::Attribute::BLUNT_TYPE:
		{
			fAddRate = (150 + 10 * (attackType.m_cSkillLevel + wLockCount * 6)) / 100.0f;

			unsigned short wEnchantLevel = attackType.m_cSkillLevel + wLockCount * 6;
			unsigned long dwDurationSec = static_cast<unsigned long>(
				(attackType.m_cSkillLevel + wLockCount * 6) * 0.5f * LevelFactor(lpSkillUser, lpVictim));

			wResult = CAddSpell<CStunSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
				SpellType::MAGICAL_SPELL, SpellID::Stun, wEnchantLevel, dwDurationSec))(lpVictim);
			break;
		}

		default:
		{
			if(Creature::GetCreatureType(lpSkillUser->GetCID()) == Creature::CT_MONSTER)
			{

				fAddRate = (150 + 15 * (attackType.m_cSkillLevel + wLockCount * 6)) / 100.0f;
				break;
			}
			else
			{
				ERRLOG2(g_Log, "CID:0x%08x fuckyousideways bash :%d", 
					lpSkillUser->GetCID(), lpSkillUser->GetStatus().m_StatusInfo.m_cCriticalType);
				return 0;
			}
		}
	}

	switch (wResult)
	{
		case CSpell::NO_ENCHANT_ERROR :
		case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
			break;

		case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
			wError = PktAtAck::FAIL_SPELL;
			break;

		case CSpell::ENCHANT_FAIL_BY_RESIST :
			cDefenserJudge = ClientConstants::Judge_Resist;
			break;
	}

	if (false == SlowlySkillAttack(ProtoType, attackType, lpSkillUser, lpVictim))
	{
		wError = PktAtAck::FAIL_SPELL;
	}

	
	unsigned short wDamage = lpSkillUser->MeleeAttack(lpVictim, Creature::RIGHT_NON_PASSIVE_TYPE, 

		LevelFactor(lpSkillUser, lpVictim), cDefenserJudge);

	long lDamage = static_cast<long>(wDamage * fAddRate);
	wDamage = static_cast<unsigned short>(min(lDamage, USHRT_MAX));

	lpVictim->GetThreat().AffectThreat(lpSkillUser, wDamage, CThreat::TAUNT);
	return wDamage;
}


DEFINE_SKILL(BluntMastery)
{
	return 0;
}


DEFINE_SKILL(WillPower)
{
	return 0;
}


DEFINE_SKILL(Toughness)
{
	return 0;
}



//-------------------------------------------------------------------------------------------------
// 로그 스킬

DEFINE_SKILL(Detection)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

	if (true == lpVictim->GetSpellMgr().GetAffectedInfo().RemoveEnchantBySpellType(SpellID::Stealth))
	{
		lpVictim->GetThreat().AddToThreatList(lpSkillUser, 1);

		Creature::CreatureType eCreatureType = Creature::GetCreatureType(lpVictim->GetCID());
		if (Creature::CT_MONSTER == eCreatureType ||
			Creature::CT_SUMMON == eCreatureType ||
			Creature::CT_STRUCT == eCreatureType)
		{
			CMonster* lpMonster = reinterpret_cast<CMonster *>(lpVictim);
			lpMonster->Attacked();
		}
	}

	cDefenserJudge = ClientConstants::Judge_Disenchant;
	return 0;
}


DEFINE_SKILL(ImprovedCritical)
{
	return 0;
}


DEFINE_SKILL(Evasion)
{
	return 0;
}


DEFINE_SKILL(NeedleSpit)
{	
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	 Position currPos=lpSkillUser->GetCurrentPos();
	 	if(((currPos.m_fPointX >= 1824)&&(currPos.m_fPointX <= 2250))&&((currPos.m_fPointZ >= 2817)&&(currPos.m_fPointZ <= 3189))){if(lpVictim->IsSafeZones(lpSkillUser , lpVictim)) {return 0;}}
		//return 0;

	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

	unsigned short wLockCount = GetSkillLockCount(lpSkillUser, attackType);
	//if(wLockCount == 3)//hz halt from level 4
	//	{
	//		if (SuccessHitRate(lpSkillUser, lpVictim, 1) > Math::Random::ComplexRandom(100))
	//		{
	//		//	unsigned short wEnchantLevel = attackType.m_cSkillLevel + wLockCount * 6;
	//		//	unsigned long dwDurationSec = CastEnchantTick(lpSkillUser, lpVictim, attackType, 1);
	//		//	dwDurationSec=static_cast<unsigned long>(dwDurationSec/(2.0f));//hz added this

	//		//	unsigned short wResult = CAddSpell<CHoldSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
	//		//		SpellType::MAGICAL_SPELL, SpellID::Hold, wEnchantLevel, dwDurationSec))(lpVictim);

	//		//	switch (wResult)
	//		//	{
	//		//		case CSpell::NO_ENCHANT_ERROR :
	//		//		case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
	//		//			{
	//		//				lpVictim->GetThreat().AddToThreatList(lpSkillUser, 1);

	//		//				Creature::CreatureType eCreatureType = Creature::GetCreatureType(lpVictim->GetCID());
	//		//				if (Creature::CT_MONSTER == eCreatureType ||
	//		//					Creature::CT_SUMMON == eCreatureType ||
	//		//					Creature::CT_STRUCT == eCreatureType)
	//		//				{
	//		//					CMonster* lpMonster = reinterpret_cast<CMonster *>(lpVictim);
	//		//					lpMonster->Attacked();
	//		//				}

	//		//				cDefenserJudge = ClientConstants::Judge_Enchant;
	//		//			}
	//		//			break;

	//		//		case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
	//		//			wError = PktAtAck::FAIL_SPELL;
	//		//			break;

	//		//		case CSpell::ENCHANT_FAIL_BY_RESIST :
	//		//			cDefenserJudge = ClientConstants::Judge_Resist;
	//		//			break;
	//		//	}
	//		//}
	//		//else
	//		//	cDefenserJudge = ClientConstants::Judge_Resist;
	//}

	long lDamage = static_cast<long>((attackType.m_cSkillLevel + wLockCount * 6) * 45 * //was 30
		(1 + lpSkillUser->GetStatus().m_nLevel * 0.05f) * 
		(100 - lpVictim->GetStatus().m_StatusInfo.m_fDefenceRate) / 100.0f *
		LevelFactor(lpSkillUser, lpVictim) * (115 + Math::Random::ComplexRandom(10)) / 100.0f);

	return static_cast<unsigned short>(min(lDamage, USHRT_MAX));
}



//-------------------------------------------------------------------------------------------------
// 메이지 스킬

DEFINE_SKILL(BloodyMana)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);
	Position currPos=lpSkillUser->GetCurrentPos();
	 	if(((currPos.m_fPointX >= 1824)&&(currPos.m_fPointX <= 2250))&&((currPos.m_fPointZ >= 2817)&&(currPos.m_fPointZ <= 3189))){if(lpVictim->IsSafeZones(lpSkillUser , lpVictim)) {return 0;}}
		//return 0;
	 

	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

	unsigned short wLockCount = GetSkillLockCount(lpSkillUser, attackType);
	unsigned short wHPCost = static_cast<unsigned short>((attackType.m_cSkillLevel + wLockCount * 6) * 20 * 
		(100 + lpSkillUser->GetStatus().m_StatusInfo.m_wMagicPower) / 100.0f);

	// HP가 부족하면 발동되지 않는다.
	if (lpSkillUser->m_CreatureStatus.m_nNowHP <= wHPCost)
	{
//		lpSkillUser->m_CreatureStatus.m_nNowHP = 0;
		cDefenserJudge = ClientConstants::Judge_ManaHeal;
		return 0;
	}

	const unsigned short wOldMP = lpVictim->m_CreatureStatus.m_nNowMP;
	lpSkillUser->m_CreatureStatus.m_nNowHP -= wHPCost;

	const unsigned short wAdditionalMP = InstanceMagicRegen(lpSkillUser, attackType, 40);
	if (lpVictim->m_CreatureStatus.m_nNowMP > USHRT_MAX - wAdditionalMP)
	{
		lpVictim->m_CreatureStatus.m_nNowMP = lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxMP;
	}
	else
	{
		lpVictim->m_CreatureStatus.m_nNowMP += wAdditionalMP;
		lpVictim->m_CreatureStatus.m_nNowMP = std::min(lpVictim->m_CreatureStatus.m_nNowMP, lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxMP);
	}

	cDefenserJudge = ClientConstants::Judge_ManaHeal;
	wDefenserMPHeal = (lpVictim->m_CreatureStatus.m_nNowMP > wOldMP) ? lpVictim->m_CreatureStatus.m_nNowMP - wOldMP : 0;
	return 0;
}
/*//--------------------------------------------------------------
DEFINE_SKILL(Judgment)//hz1
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);
	 
	 Position currPos=lpSkillUser->GetCurrentPos();
		if(((currPos.m_fPointX >= 1824)&&(currPos.m_fPointX <= 2250))&&((currPos.m_fPointZ >= 2817)&&(currPos.m_fPointZ <= 3189))){if(lpVictim->IsSafeZones(lpSkillUser , lpVictim)) {return 0;}}
		//return 0;

	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

	const unsigned short wDamage =(lpSkillUser->m_CreatureStatus.m_nLevel * 6) + (lpSkillUser->m_CreatureStatus.m_StatusInfo.m_wMagicPower * 0.6);

	CCell* lpCell =lpSkillUser->m_CellPos.m_lpCell  ;
	int divider = 1;
	int TargetIndex=0;
	CAggresiveCreature* TargetHeal[AtNode::MAX_DEFENDER_NUM];
	DefenserNode TargetNode[AtNode::MAX_DEFENDER_NUM];

	 for (int nDirection = 0; nDirection < CCell::CONNECT_NUM && TargetIndex <AtNode::MAX_DEFENDER_NUM; ++nDirection)
    {
        CCell* lpConnectCell = lpCell->GetConnectCell(nDirection);
        if (NULL == lpConnectCell) 
        {
            continue;
        }

        CAggresiveCreature* lpTempTarget = lpConnectCell->GetFirstAggresiveCreature();
        while (NULL != lpTempTarget )
        {
            const float fDX = lpTempTarget->GetCurrentPos().m_fPointX - lpSkillUser->GetCurrentPos().m_fPointX ;
            const float fDZ = lpTempTarget->GetCurrentPos().m_fPointZ - lpSkillUser->GetCurrentPos().m_fPointZ;				
            const float fDistance = (fDX * fDX) + (fDZ * fDZ);
			float nRange = 20;
            const float fSquareAttackRange = nRange * nRange;
			if (fDistance <= fSquareAttackRange)
            {
				bool bException = false;
				if (lpTempTarget->GetNation() !=lpSkillUser->GetNation()){bException = true;divider++;}	
				
				if (false == bException)
				{		
					DefenserNode Dnode;					
					Dnode.m_dwCharID	= lpTempTarget->m_dwCID;
					Dnode.m_wMaxHP	= lpTempTarget->GetStatus().m_StatusInfo.m_nMaxHP;
					Dnode.m_wMaxMP	= lpTempTarget->GetStatus().m_StatusInfo.m_nMaxMP;
					Dnode.m_sCurrHP	= lpTempTarget->GetStatus().m_nNowHP;
					Dnode.m_sCurrMP	= lpTempTarget-> GetStatus().m_nNowMP;
					Dnode.m_wMPHeal	= wDamage;
					Dnode.m_wDamage	= wDamage;
					Dnode.m_cJudge	= ClientConstants::Judge_Enchant;

					TargetHeal[TargetIndex] = lpTempTarget;
					TargetNode[TargetIndex] = Dnode;
					TargetIndex++;				
                }
            }

            lpTempTarget = lpConnectCell->GetNextAggresiveCreature();
        }
    }

	 for (int i = 0 ; i<TargetIndex;i++){
		if (!TargetHeal[i]){continue;}
		
		TargetHeal[i]->RegenHPAndMP(wDamage/divider,wDamage/divider,true);
		if (i>0){
			lpCell->SendAttackInfo(lpSkillUser->m_dwCID , attackType, 1, &TargetNode[TargetIndex]);
		}
	 }
		 			
	cOffencerJudge = ClientConstants::Judge_Heal;
	//wOffencerMPHeal =wDamage;
	lpSkillUser->m_CreatureStatus.m_nNowHP+=wDamage;
	return wDamage;
}
*/
DEFINE_SKILL(VampiricTouch)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);
	Position currPos=lpSkillUser->GetCurrentPos();
	 	if(((currPos.m_fPointX >= 1824)&&(currPos.m_fPointX <= 2250))&&((currPos.m_fPointZ >= 2817)&&(currPos.m_fPointZ <= 3189))){if(lpVictim->IsSafeZones(lpSkillUser , lpVictim)) {return 0;}}
		//return 0;
	 

	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

	const unsigned short wDamage = CastMagicDamage(lpSkillUser, lpVictim, attackType, 15);

	const unsigned short wLockCount = GetSkillLockCount(lpSkillUser, attackType);
	const unsigned short wOldMP = lpSkillUser->GetStatus().m_nNowMP;

	const long lAdditionalHP = static_cast<long>(wDamage * (4 * (attackType.m_cSkillLevel + wLockCount * 6) / 100.0f));
	const unsigned short wAdditionalHP = static_cast<unsigned short>(min(lAdditionalHP, USHRT_MAX));

	const long lAdditionalMP = static_cast<long>(wDamage * (2 * (attackType.m_cSkillLevel + wLockCount * 6) / 100.0f));
	const unsigned short wAdditionalMP = static_cast<unsigned short>(min(lAdditionalMP, USHRT_MAX));

	if (lpSkillUser->GetStatus().m_nNowHP > USHRT_MAX - wAdditionalHP)
	{
		lpSkillUser->GetStatus().m_nNowHP = lpSkillUser->GetStatus().m_StatusInfo.m_nMaxHP;
	}
	else
	{
		lpSkillUser->GetStatus().m_nNowHP += wAdditionalHP;
		lpSkillUser->GetStatus().m_nNowHP = 
			std::min(lpSkillUser->GetStatus().m_nNowHP, lpSkillUser->GetStatus().m_StatusInfo.m_nMaxHP);
	}

	if (lpSkillUser->GetStatus().m_nNowMP > USHRT_MAX - wAdditionalMP)
	{
		lpSkillUser->GetStatus().m_nNowMP = lpSkillUser->GetStatus().m_StatusInfo.m_nMaxMP;
	}
	else
	{
		lpSkillUser->GetStatus().m_nNowMP += wAdditionalMP;
		lpSkillUser->GetStatus().m_nNowMP = 
			std::min(lpSkillUser->GetStatus().m_nNowMP, lpSkillUser->GetStatus().m_StatusInfo.m_nMaxMP);
	}
	
	// TODO : HP와 MP가 동시에 회복되는 경우는...?
	cOffencerJudge = ClientConstants::Judge_Heal;
	wOffencerMPHeal = (lpSkillUser->GetStatus().m_nNowMP > wOldMP) ? lpSkillUser->GetStatus().m_nNowMP - wOldMP : 0;

	return wDamage;
}


DEFINE_SKILL(ManaShell)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	 
	 

	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

	unsigned short wEnchantLevel = static_cast<unsigned short>(
		floor( pow( (attackType.m_cSkillLockCount + 1) + (attackType.m_cSkillLevel * 0.15f), 2 ) ) * 5 );

	unsigned long dwDurationSec = static_cast<unsigned long>((100 + lpSkillUser->GetStatus().m_StatusInfo.m_wMagicPower / 8.0f) * 
		(attackType.m_cSkillLockCount + 1));

	unsigned short wResult = CAddSpell<CManaShellSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
		SpellType::MAGICAL_SPELL, SpellID::ManaShell, wEnchantLevel, dwDurationSec))(lpVictim);

	switch (wResult)
	{
		case CSpell::NO_ENCHANT_ERROR :
		case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
			cDefenserJudge = ClientConstants::Judge_Enchant;
			break;

		case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
			wError = PktAtAck::FAIL_SPELL;
			break;

		case CSpell::ENCHANT_FAIL_BY_RESIST :
			cDefenserJudge = ClientConstants::Judge_Resist;
			break;
	}

	return 0;
}


DEFINE_SKILL(Grease)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	 Position currPos=lpSkillUser->GetCurrentPos();
	 	if(((currPos.m_fPointX >= 1824)&&(currPos.m_fPointX <= 2250))&&((currPos.m_fPointZ >= 2817)&&(currPos.m_fPointZ <= 3189))){if(lpVictim->IsSafeZones(lpSkillUser , lpVictim)) {return 0;}}
		//return 0;

	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }
	
	char successRate=GetStickyMagicSuccessRate(lpSkillUser,lpVictim,2);

	if (successRate > Math::Random::ComplexRandom(100))
	{
		unsigned short wEnchantLevel = attackType.m_cSkillLevel + GetSkillLockCount(lpSkillUser, attackType) * 6;
		unsigned long dwDurationSec = CastEnchantTick(lpSkillUser, lpVictim, attackType, 2);

		unsigned short wResult = CAddSpell<CHoldSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
			SpellType::MAGICAL_SPELL, SpellID::Hold, wEnchantLevel, dwDurationSec))(lpVictim);

		switch (wResult)
		{
			case CSpell::NO_ENCHANT_ERROR :
			case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
				{
					lpVictim->GetThreat().AddToThreatList(lpSkillUser, 1);

					Creature::CreatureType eCreatureType = Creature::GetCreatureType(lpVictim->GetCID());
					if (Creature::CT_MONSTER == eCreatureType ||
						Creature::CT_SUMMON == eCreatureType ||
						Creature::CT_STRUCT == eCreatureType)
					{
						CMonster* lpMonster = reinterpret_cast<CMonster *>(lpVictim);
						lpMonster->Attacked();
					}

					cDefenserJudge = ClientConstants::Judge_Enchant;
				}
				break;

			case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
				wError = PktAtAck::FAIL_SPELL;
				break;

			case CSpell::ENCHANT_FAIL_BY_RESIST :
				cDefenserJudge = ClientConstants::Judge_Resist;
				break;
		}
	}
	else
	{
		cDefenserJudge = ClientConstants::Judge_Resist;
	}
    	
	return 0;
}


DEFINE_SKILL(Serenity)
{
	return 0;
}



//-------------------------------------------------------------------------------------------------
// 어콜라이트 스킬

DEFINE_SKILL(Purification)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);
	Position currPos=lpSkillUser->GetCurrentPos();
	 	if(((currPos.m_fPointX >= 1824)&&(currPos.m_fPointX <= 2250))&&((currPos.m_fPointZ >= 2817)&&(currPos.m_fPointZ <= 3189))){if(lpVictim->IsSafeZones(lpSkillUser , lpVictim)) {return 0;}}
		//return 0;
	 

	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

	lpVictim->GetSpellMgr().GetAffectedInfo().Disenchant(SpellType::MAGICAL_SPELL, SpellTarget::ENEMY_TARGET_ENCHANT, 
		Disenchant::NONE, 0, Disenchant::INFINITE_NUM);

	cOffencerJudge = ClientConstants::Judge_Disenchant;
	cDefenserJudge = ClientConstants::Judge_Disenchant;
	return 0;
}


DEFINE_SKILL(Faith)
{
	return 0;
}


DEFINE_SKILL(Encourage)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	 Position currPos=lpSkillUser->GetCurrentPos();
	 	if(((currPos.m_fPointX >= 1824)&&(currPos.m_fPointX <= 2250))&&((currPos.m_fPointZ >= 2817)&&(currPos.m_fPointZ <= 3189))){if(lpVictim->IsSafeZones(lpSkillUser , lpVictim)) {return 0;}}
		//return 0;

	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

	unsigned short wEnchantLevel = CastEnchantLevel(lpSkillUser, lpVictim, attackType, 1);
	char BuffMasteryLevel=GetAbilityLevel(lpSkillUser,0x1061);
	float BuffMasteryRatio=GetAbilityRatio(BuffMasteryLevel,1.1,0.1);

	unsigned long dwDurationSec = static_cast<unsigned long>((100 + lpSkillUser->GetStatus().m_StatusInfo.m_wMagicPower / 8.0f)*BuffMasteryRatio);

	unsigned short wResult = CAddSpell<CEncourageSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
		SpellType::MAGICAL_SPELL, SpellID::Encourage, wEnchantLevel, dwDurationSec))(lpVictim);

	switch (wResult)
	{
		case CSpell::NO_ENCHANT_ERROR :
		case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
			cDefenserJudge = ClientConstants::Judge_Enchant;
			break;

		case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
			wError = PktAtAck::FAIL_SPELL;
			break;

		case CSpell::ENCHANT_FAIL_BY_RESIST :
			cDefenserJudge = ClientConstants::Judge_Resist;
			break;
	}

	return 0;
}


DEFINE_SKILL(FirstAid)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);
	Position currPos=lpSkillUser->GetCurrentPos();
	 	if(((currPos.m_fPointX >= 1824)&&(currPos.m_fPointX <= 2250))&&((currPos.m_fPointZ >= 2817)&&(currPos.m_fPointZ <= 3189))){if(lpVictim->IsSafeZones(lpSkillUser , lpVictim)) {return 0;}}
		//return 0;
	 

	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

	const unsigned short wPreviousHP = lpVictim->m_CreatureStatus.m_nNowHP;

	const unsigned short wBaseRegenLine = static_cast<unsigned short>(
		(100 + (attackType.m_cSkillLevel + GetSkillLockCount(lpSkillUser, attackType) * 6) * 20) * 
		(100 + lpSkillUser->GetStatus().m_StatusInfo.m_wMagicPower) / 100.0f);
	lpVictim->m_CreatureStatus.m_nNowHP = 
		std::max(lpVictim->m_CreatureStatus.m_nNowHP, wBaseRegenLine);

	const unsigned short wAdditionalHP = InstanceMagicRegen(lpSkillUser, attackType, 10);
	if (lpVictim->m_CreatureStatus.m_nNowHP > USHRT_MAX - wAdditionalHP)
	{
		lpVictim->m_CreatureStatus.m_nNowHP = lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxHP;
	}
	else
	{
		lpVictim->m_CreatureStatus.m_nNowHP += wAdditionalHP;
		lpVictim->m_CreatureStatus.m_nNowHP = 
			std::min(lpVictim->m_CreatureStatus.m_nNowHP, lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxHP);
	}

	lpVictim->GetThreat().HealThreat(lpSkillUser, (lpVictim->m_CreatureStatus.m_nNowHP - wPreviousHP) / 2);

	cDefenserJudge = ClientConstants::Judge_Heal;
	return 0;
}


DEFINE_SKILL(HammerOfLight)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	 Position currPos=lpSkillUser->GetCurrentPos();
	 	if(((currPos.m_fPointX >= 1824)&&(currPos.m_fPointX <= 2250))&&((currPos.m_fPointZ >= 2817)&&(currPos.m_fPointZ <= 3189))){if(lpVictim->IsSafeZones(lpSkillUser , lpVictim)) {return 0;}}
	//	return 0;

	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }
	unsigned char successRate=GetStunMasterySuccessRate(lpSkillUser,lpVictim,0.5f);

		char StunBreakerLevel=GetAbilityLevel(lpVictim,0x105F);
		float StunBreakerRatio=GetAbilityRatio(StunBreakerLevel,1.10,0.10);
		float finalSuccessRate=successRate;
		finalSuccessRate*=StunBreakerRatio;
		successRate=static_cast<unsigned char>(finalSuccessRate);

	if (successRate > Math::Random::ComplexRandom(100))
	{
		unsigned short wEnchantLevel = attackType.m_cSkillLevel + GetSkillLockCount(lpSkillUser, attackType) * 6;
		unsigned long dwDurationSec = CastEnchantTick(lpSkillUser, lpVictim, attackType, 1);
		dwDurationSec=static_cast<unsigned long>(dwDurationSec*0.7f);
		unsigned short wResult = CAddSpell<CStunSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
			SpellType::MAGICAL_SPELL, SpellID::Stun, wEnchantLevel, dwDurationSec))(lpVictim);

		switch (wResult)
		{
			case CSpell::NO_ENCHANT_ERROR :
			case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
				break;

			case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
				wError = PktAtAck::FAIL_SPELL;
				break;

			case CSpell::ENCHANT_FAIL_BY_RESIST :
				cDefenserJudge = ClientConstants::Judge_Resist;
				break;
		}
	}
	else
	{
		cDefenserJudge = ClientConstants::Judge_Resist;
	}

	// CastMagicDamage() 함수중에서 wLockCount 곱셈이 하나 빠진 공식이다.!!
	const unsigned char cFactor = 15;
	const unsigned short wLockCount = GetSkillLockCount(lpSkillUser, attackType);

	long lDamage = static_cast<long>(((wLockCount + 1) * attackType.m_cSkillLevel + 1 + attackType.m_cSkillLockCount * 5) *
		cFactor * (75 + Math::Random::ComplexRandom(75) + lpSkillUser->GetStatus().m_StatusInfo.m_wMagicPower) / 100.0f * 
		(100 - lpVictim->GetStatus().m_StatusInfo.m_fMagicResistRate) / 100.0f * LevelFactor(lpSkillUser, lpVictim));
	lDamage=static_cast<long>(lDamage /4.0f);
	return static_cast<unsigned short>(min(lDamage, USHRT_MAX));
}



//-------------------------------------------------------------------------------------------------
// 디펜더 스킬

DEFINE_SKILL(Charging)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	 
	 

	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

	lpVictim->GetSpellMgr().GetAffectedInfo().Disenchant(SpellType::NONE, SpellTarget::ENEMY_TARGET_ENCHANT, 
		Disenchant::NONE, 0, Disenchant::INFINITE_NUM);

	unsigned short wResult = CAddSpell<CChargingSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
		SpellType::MAGICAL_SPELL, SpellID::Charging, lpSkillUser->GetStatus().m_nLevel, 30))(lpVictim);

	switch (wResult)
	{
		case CSpell::NO_ENCHANT_ERROR :
		case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
			cDefenserJudge = ClientConstants::Judge_Enchant;
			break;

		case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
			wError = PktAtAck::FAIL_SPELL;
			break;

		case CSpell::ENCHANT_FAIL_BY_RESIST :
			cDefenserJudge = ClientConstants::Judge_Resist;
			break;
	}

	return 0;
}


DEFINE_SKILL(SharedPain)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

	if (false == SlowlySkillAttack(ProtoType, attackType, lpSkillUser, lpVictim))
	{
		wError = PktAtAck::FAIL_SPELL;
	}

	unsigned short wLockCount = GetSkillLockCount(lpSkillUser, attackType);
//	float fAddRate = (150 + 10 * (attackType.m_cSkillLevel + wLockCount * 6)) / 100.0f;
	float fAddRate = (1000 + 100 * (attackType.m_cSkillLevel + wLockCount * 6)) / 100.0f;

	unsigned short wDamage = lpSkillUser->MeleeAttack(lpVictim, Creature::LEFT_NON_PASSIVE_TYPE, 
		LevelFactor(lpSkillUser, lpVictim), cDefenserJudge);
	long lDamage = static_cast<long>(wDamage * fAddRate);
	wDamage = static_cast<unsigned short>(min(lDamage, USHRT_MAX));

	lpVictim->GetThreat().AffectThreat(lpSkillUser, wDamage, CThreat::DETAUNT);

	if(4 <= attackType.m_cSkillLockCount)
	{
		//const float fHPRegenRate = static_cast<float>((120 + (attackType.m_cSkillLevel * 5)) / 100.0f);
		//const float fMPRegenRate = static_cast<float>((10 + (attackType.m_cSkillLevel * 15)) / 100.0f);
		const float fHPRegenRate = static_cast<float>((1500 + (attackType.m_cSkillLevel * 6)) / 100.0f);
		const float fMPRegenRate = static_cast<float>((150 + (attackType.m_cSkillLevel * 10)) / 100.0f);
		long lAdditionalHP = static_cast<long>(wDamage * fHPRegenRate);;
		const unsigned short wAdditionalHP = static_cast<unsigned short>(min(lAdditionalHP, USHRT_MAX));

		long lAdditionalMP = static_cast<long>(wDamage * fMPRegenRate);;
		const unsigned short wAdditionalMP = static_cast<unsigned short>(min(lAdditionalMP, USHRT_MAX));

		if (lpSkillUser->m_CreatureStatus.m_nNowHP > USHRT_MAX - wAdditionalHP)
		{
			lpSkillUser->m_CreatureStatus.m_nNowHP = lpSkillUser->m_CreatureStatus.m_StatusInfo.m_nMaxHP;
		}
		else
		{
			lpSkillUser->m_CreatureStatus.m_nNowHP += wAdditionalHP;
			lpSkillUser->m_CreatureStatus.m_nNowHP = 
				std::min(lpSkillUser->m_CreatureStatus.m_nNowHP, lpSkillUser->m_CreatureStatus.m_StatusInfo.m_nMaxHP);
		}

		if (lpVictim->m_CreatureStatus.m_nNowMP < wAdditionalMP)
		{
			lpVictim->m_CreatureStatus.m_nNowMP = 0;
		}
		else
		{
			lpVictim->m_CreatureStatus.m_nNowMP -= wAdditionalMP;
			lpVictim->m_CreatureStatus.m_nNowMP = 
				std::max(lpVictim->m_CreatureStatus.m_nNowMP, static_cast<unsigned short>(0));
		}

		const unsigned short wOffencerOldMP = lpSkillUser->m_CreatureStatus.m_nNowMP;
		if (lpSkillUser->m_CreatureStatus.m_nNowMP > USHRT_MAX - wAdditionalMP)
		{
			lpSkillUser->m_CreatureStatus.m_nNowMP = lpSkillUser->m_CreatureStatus.m_StatusInfo.m_nMaxMP;
		}
		else
		{
			lpSkillUser->m_CreatureStatus.m_nNowMP += wAdditionalMP;
			lpSkillUser->m_CreatureStatus.m_nNowMP = 
				std::min(lpSkillUser->m_CreatureStatus.m_nNowMP, lpSkillUser->m_CreatureStatus.m_StatusInfo.m_nMaxMP);
		}

		wOffencerMPHeal = (lpSkillUser->m_CreatureStatus.m_nNowMP > wOffencerOldMP) ? lpSkillUser->m_CreatureStatus.m_nNowMP - wOffencerOldMP : 0;
	}
	else
	{
		const float fRegenRate = 5 * (attackType.m_cSkillLevel + wLockCount * 6) / 100.0f; // was 5 *

		long lAdditionalHP = static_cast<long>(wDamage * fRegenRate);
		const unsigned short wAdditionalHP = static_cast<unsigned short>(min(lAdditionalHP, USHRT_MAX));

		if (lpSkillUser->m_CreatureStatus.m_nNowHP > USHRT_MAX - wAdditionalHP)
		{
			lpSkillUser->m_CreatureStatus.m_nNowHP = lpSkillUser->m_CreatureStatus.m_StatusInfo.m_nMaxHP;
		}
		else
		{
			lpSkillUser->m_CreatureStatus.m_nNowHP += wAdditionalHP;
			lpSkillUser->m_CreatureStatus.m_nNowHP = 
				std::min(lpSkillUser->m_CreatureStatus.m_nNowHP, lpSkillUser->m_CreatureStatus.m_StatusInfo.m_nMaxHP);
		}
	}

	cOffencerJudge = ClientConstants::Judge_Heal;

	return wDamage;
}




/*-------------- original code
DEFINE_SKILL(SharedPain)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);
	Position currPos=lpSkillUser->GetCurrentPos();
	 	if(((currPos.m_fPointX >= 1824)&&(currPos.m_fPointX <= 2250))&&((currPos.m_fPointZ >= 2817)&&(currPos.m_fPointZ <= 3189))){if(lpVictim->IsSafeZones(lpSkillUser , lpVictim)) {return 0;}}
		//return 0;
	 

	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

	unsigned short wLockCount = GetSkillLockCount(lpSkillUser, attackType);
	unsigned short wHPCost, wMPCost, wDamage;

	if(4 <= attackType.m_cSkillLockCount)
	{
		wHPCost = 10000;
		wMPCost = 4000;


		// HP가 1만보다 적거나 MP가 4000보다 적으면 발동되지 않는다.
		if(wHPCost > lpSkillUser->m_CreatureStatus.m_nNowHP || wMPCost > lpSkillUser->m_CreatureStatus.m_nNowMP)
		{
			return 0;
		}

		// 공성병기는 무조건 저항 
		const unsigned char cCreatureType = Creature::GetCreatureType(lpVictim->GetCID());
		if (Creature::CT_STRUCT == cCreatureType ||
			Creature::CT_SIEGE_OBJECT == cCreatureType)
		{
			cDefenserJudge = ClientConstants::Judge_Resist;
			return 0; 
		}


		if(-5 < lpSkillUser->CalculateFixLevelGap(lpVictim))
		{
			// 락카운트의 수치만큼 방어력과 방어%를 감소 시킨다.
			float fDecreaseDefenceRate = static_cast<float>(2 + attackType.m_cSkillLevel * 8);
			float fDecreasedDefenceRate = static_cast<float>(lpVictim->GetStatus().m_StatusInfo.m_fDefenceRate - 
				((lpVictim->GetStatus().m_StatusInfo.m_fDefenceRate * fDecreaseDefenceRate) / 100.0f));

			long lDamage = static_cast<long>(lpVictim->m_CreatureStatus.m_nNowMP * 
				(100 - fDecreasedDefenceRate) / 100.0f * LevelFactor(lpSkillUser, lpVictim));

			wDamage = static_cast<unsigned short>(min(lDamage, USHRT_MAX));

			lpSkillUser->m_CreatureStatus.m_nNowHP = static_cast<unsigned short>(lpSkillUser->m_CreatureStatus.m_nNowHP / 100.0f);
			lpSkillUser->m_CreatureStatus.m_nNowMP = static_cast<unsigned short>(lpSkillUser->m_CreatureStatus.m_nNowMP / 100.0f);
			lpVictim->m_CreatureStatus.m_nNowMP = 0;
		}
		else
		{
			cDefenserJudge = ClientConstants::Judge_Resist;
			return 0;
		}
	}
	else
	{
		wHPCost = static_cast<unsigned short>((attackType.m_cSkillLevel + wLockCount * 6) * 15 * 
			(1 + lpSkillUser->GetStatus().m_nLevel * 0.05f));

		// HP가 부족하면 발동되지 않는다.
		if (lpSkillUser->m_CreatureStatus.m_nNowHP <= wHPCost)
		{
//			lpSkillUser->m_CreatureStatus.m_nNowHP = 0;
			return 0;
		}

		lpSkillUser->m_CreatureStatus.m_nNowHP -= wHPCost;

		// edith 2008.12.18 벨런스 패치
		// edith 2009.12.25 벨런스 패치 1.5배 처리
		long lDamage = static_cast<long>((float)wHPCost*1.5f);
/*
		long lDamage = static_cast<long>((attackType.m_cSkillLevel + wLockCount * 6) * 80 * 
			(1 + lpSkillUser->GetStatus().m_nLevel * 0.05f) * 
			(100 - lpVictim->GetStatus().m_StatusInfo.m_fDefenceRate) / 100.0f * 
			LevelFactor(lpSkillUser, lpVictim) * (115 + Math::Random::ComplexRandom(10)) / 100.0f);
*/
/* ---------
		wDamage = static_cast<unsigned short>(min(lDamage, USHRT_MAX));
	}

	lpVictim->GetThreat().AffectThreat(lpSkillUser, wDamage, CThreat::TAUNT);
	return wDamage;
}
*/ //------------

DEFINE_SKILL(FullSwing)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	 Position currPos=lpSkillUser->GetCurrentPos();
	 	if(((currPos.m_fPointX >= 1824)&&(currPos.m_fPointX <= 2250))&&((currPos.m_fPointZ >= 2817)&&(currPos.m_fPointZ <= 3189))){if(lpVictim->IsSafeZones(lpSkillUser , lpVictim)) {return 0;}}
		//return 0;

	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

	if (false == SlowlySkillAttack(ProtoType, attackType, lpSkillUser, lpVictim))
	{
		wError = PktAtAck::FAIL_SPELL;
	}

	unsigned short wLockCount = GetSkillLockCount(lpSkillUser, attackType);
	float fAddRate = 2 + (8 * (attackType.m_cSkillLevel + wLockCount * 6) / 100.0f);

	// edith 2009.03.15 스킬공격시 패시브 스킬 포함되게 수정
//	unsigned short wDamage = lpSkillUser->MeleeAttack(lpVictim, Creature::RIGHT_NON_PASSIVE_TYPE, 
	unsigned short wDamage = lpSkillUser->MeleeAttack(lpVictim, Creature::RIGHT_PASSIVE_TYPE, 
	LevelFactor(lpSkillUser, lpVictim), cDefenserJudge);

	//For Full Swing Weapon Breaker:
	//unsigned short wResult = CSpell::NO_ENCHANT_ERROR ;
	fAddRate = (150 + 10 * (attackType.m_cSkillLevel + wLockCount * 6)) / 100.0f;
	unsigned short wEnchantLevel = (attackType.m_cSkillLevel*3) + (wLockCount*24);
	unsigned long dwDurationSec = static_cast<unsigned long>(20 * LevelFactor(lpSkillUser, lpVictim));

	float Ratio=1;

	unsigned short WeaponbreakerLevel=static_cast<unsigned short>(GetAbilityLevel(lpSkillUser,0x1063));
	if(WeaponbreakerLevel != 0)
	{
			wEnchantLevel = (attackType.m_cSkillLevel*3) + (wLockCount*24);
			Ratio=GetAbilityRatio(WeaponbreakerLevel,1.25,0.25);
					
			float FinalEnchantLevel=static_cast<float>(wEnchantLevel);
			FinalEnchantLevel*=Ratio;
			wEnchantLevel=static_cast<unsigned short>(FinalEnchantLevel);

			CAddSpell<CDeEnchantSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
			SpellType::MAGICAL_SPELL, SpellID::DeEnchant, wEnchantLevel, dwDurationSec))(lpVictim);
	}

	//end Weapon breaker

	long lDamage = static_cast<long>(wDamage * fAddRate);
	wDamage = static_cast<unsigned short>(min(lDamage, USHRT_MAX));

	lpVictim->GetThreat().AffectThreat(lpSkillUser, wDamage, CThreat::TAUNT);
	return wDamage;
}

DEFINE_SKILL(Taunt)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);
	Position currPos=lpSkillUser->GetCurrentPos();
	 	if(((currPos.m_fPointX >= 1824)&&(currPos.m_fPointX <= 2250))&&((currPos.m_fPointZ >= 2817)&&(currPos.m_fPointZ <= 3189))){if(lpVictim->IsSafeZones(lpSkillUser , lpVictim)) {return 0;}}
		//return 0;
	 

	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

	if (false == SlowlySkillAttack(ProtoType, attackType, lpSkillUser, lpVictim))
	{
		wError = PktAtAck::FAIL_SPELL;
	}

	unsigned short wLockCount = GetSkillLockCount(lpSkillUser, attackType);
	float fAddRate = 2 + (8 * (attackType.m_cSkillLevel + wLockCount * 6) / 100.0f);
	unsigned short wResult = CSpell::NO_ENCHANT_ERROR ;

	// edith 2009.03.15 스킬공격시 패시브 스킬 포함되게 수정
//	unsigned short wDamage = lpSkillUser->MeleeAttack(lpVictim, Creature::RIGHT_NON_PASSIVE_TYPE, 
	unsigned short wDamage = lpSkillUser->MeleeAttack(lpVictim, Creature::RIGHT_PASSIVE_TYPE, 
		LevelFactor(lpSkillUser, lpVictim), cDefenserJudge);

	long lDamage = static_cast<long>(wDamage * fAddRate);
	wDamage = static_cast<unsigned short>(min(lDamage, USHRT_MAX));

	lpVictim->GetThreat().AffectThreat(lpSkillUser, wDamage, CThreat::TAUNT);

	if(lpSkillUser->GetStatus().m_StatusInfo.m_cCriticalType == Item::Attribute::AXE_TYPE)
		{
			fAddRate = (150 + 10 * (attackType.m_cSkillLevel + wLockCount * 6)) / 100.0f;
			unsigned short wEnchantLevel = (attackType.m_cSkillLevel*3) + (wLockCount*18);
			unsigned long dwDurationSec = static_cast<unsigned long>(20 * LevelFactor(lpSkillUser, lpVictim));
			CCharacter* lpCharacter = reinterpret_cast<CCharacter *>(lpSkillUser);
			//for Armor Breaker:
			if(lpCharacter->GetClass() == CClass::Templar)
			{
				unsigned short ArmorbreakerLevel=static_cast<unsigned short>(GetAbilityLevel(lpSkillUser,0x105B));
				float Ratio=1;

				if(ArmorbreakerLevel != 0)
				{
					Ratio=GetAbilityRatio(ArmorbreakerLevel,1.25,0.25);
					
					float FinalEnchantLevel=static_cast<float>(wEnchantLevel);
					FinalEnchantLevel*=Ratio;
					wEnchantLevel=static_cast<unsigned short>(FinalEnchantLevel);

					wResult = CAddSpell<CArmorBrokenSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
						SpellType::MAGICAL_SPELL, SpellID::ArmorBroken, wEnchantLevel, dwDurationSec))(lpVictim);				
				}
			}
			//For Weapon Breaker:
			
			if(lpCharacter->GetClass() == CClass::Defender)
			{
				unsigned short WeaponbreakerLevel=static_cast<unsigned short>(GetAbilityLevel(lpSkillUser,0x1063));
				if(WeaponbreakerLevel != 0)
				{
					wEnchantLevel = (attackType.m_cSkillLevel*3) + (wLockCount*24);
					float Ratio=1;
					Ratio=GetAbilityRatio(WeaponbreakerLevel,1.25,0.25);

					float FinalEnchantLevel=static_cast<float>(wEnchantLevel);
					FinalEnchantLevel*=Ratio;
					wEnchantLevel=static_cast<unsigned short>(FinalEnchantLevel);

					CAddSpell<CDeEnchantSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
					SpellType::MAGICAL_SPELL, SpellID::DeEnchant, wEnchantLevel, dwDurationSec))(lpVictim);
				}
			}

			//end Weapon breaker

			switch (wResult)
			{
				case CSpell::NO_ENCHANT_ERROR :
				case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
					break;

				case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
					wError = PktAtAck::FAIL_SPELL;
					break;

				case CSpell::ENCHANT_FAIL_BY_RESIST :
					cDefenserJudge = ClientConstants::Judge_Resist;
					break;
			}
	}

	return wDamage;
}

//-------------------------------------------------------------------------------------------------
// 워리어 스킬


DEFINE_SKILL(Blaze)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);
	Position currPos=lpSkillUser->GetCurrentPos();
	 	if(((currPos.m_fPointX >= 1824)&&(currPos.m_fPointX <= 2250))&&((currPos.m_fPointZ >= 2817)&&(currPos.m_fPointZ <= 3189))){if(lpVictim->IsSafeZones(lpSkillUser , lpVictim)) {return 0;}}
		//return 0;
	 

	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

	lpVictim->GetSpellMgr().GetAffectedInfo().Disenchant(SpellType::NONE, SpellTarget::ENEMY_TARGET_ENCHANT, 
		Disenchant::NONE, 0, Disenchant::INFINITE_NUM);

	unsigned short wResult = CAddSpell<CBlazeSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
		SpellType::MAGICAL_SPELL, SpellID::Blaze, lpSkillUser->GetStatus().m_nLevel, 30))(lpVictim);
	
	switch (wResult)
	{
		case CSpell::NO_ENCHANT_ERROR :
		case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
			cDefenserJudge = ClientConstants::Judge_Enchant;
			break;

		case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
			wError = PktAtAck::FAIL_SPELL;
			break;

		case CSpell::ENCHANT_FAIL_BY_RESIST :
			cDefenserJudge = ClientConstants::Judge_Resist;
			break;
	}

	return 0;
}


DEFINE_SKILL(BattleSong)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	if (true == CheckChantOff(attackType.m_cSkillLockCount, attackType.m_cSkillLevel))
	{
		lpSkillUser->GetSpellMgr().GetCastingInfo().ClearChant();

		cDefenserJudge = ClientConstants::Judge_Chant;
		return 0;
	}

	short nConsumeMPAmount = ConsumeMP(attackType, lpSkillUser, cOffencerJudge);
	if (-1 == nConsumeMPAmount) { return 0; }

	CSpell* pSpell = new CBattleSongSpell(CSpell::Spell_Info(ProtoType, lpSkillUser, 
		SpellType::MAGICAL_SPELL, SpellID::BattleSong,
		MeleeChantLevel(lpSkillUser, attackType, 1), CSpell::INFINITE_DURATION), nConsumeMPAmount);

	CParty* pParty = lpVictim->GetParty();
	if (NULL != pParty) 
	{
		pParty->GetPartySpellMgr().AddAffectedToAllMember(pSpell, lpSkillUser->GetMapIndex());
	} 
	else 
	{
		unsigned short wError = CSpell::NO_ENCHANT_ERROR;
		pSpell->AddAffected(lpVictim, wError);		
	}
	CGlobalSpellMgr::GetInstance().Add(pSpell);

	cDefenserJudge = ClientConstants::Judge_Chant;
	return 0;
}


DEFINE_SKILL(ChainAction)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);
	Position currPos=lpSkillUser->GetCurrentPos();
	 	if(((currPos.m_fPointX >= 1824)&&(currPos.m_fPointX <= 2250))&&((currPos.m_fPointZ >= 2817)&&(currPos.m_fPointZ <= 3189))){if(lpVictim->IsSafeZones(lpSkillUser , lpVictim)) {return 0;}}
		//return 0;
	 

	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

	if (false == SlowlySkillAttack(ProtoType, attackType, lpSkillUser, lpVictim))
	{
		wError = PktAtAck::FAIL_SPELL;
	}

	unsigned short wLockCount = GetSkillLockCount(lpSkillUser, attackType);
	float fAddRate = (200 + 12 * (attackType.m_cSkillLevel + wLockCount * 6)) / 100.0f;//was 15 *

	// edith 2009.03.15 스킬공격시 패시브 스킬 포함되게 수정
//	unsigned short wDamage = lpSkillUser->MeleeAttack(lpVictim, Creature::RIGHT_NON_PASSIVE_TYPE, 
	unsigned short wDamage = lpSkillUser->MeleeAttack(lpVictim, Creature::RIGHT_PASSIVE_TYPE, 
		LevelFactor(lpSkillUser, lpVictim), cDefenserJudge);

	long lDamage = static_cast<long>(wDamage * fAddRate);
	wDamage = static_cast<unsigned short>(min(lDamage, USHRT_MAX));

	if(4 <= attackType.m_cSkillLockCount)
	{
		unsigned char cSuccessRate = static_cast<unsigned char>(20 + (attackType.m_cSkillLevel * 5));
		if (cSuccessRate > Math::Random::ComplexRandom(100))
		{
			long lDamage = static_cast<long>(wDamage + (wDamage * (2 + (attackType.m_cSkillLevel * 8)) / 100.0f));
			wDamage = static_cast<unsigned short>(min(lDamage, USHRT_MAX));
		}

	}

	lpVictim->GetThreat().AffectThreat(lpSkillUser, wDamage, CThreat::DETAUNT);
	return wDamage;
}


DEFINE_SKILL(DualWieldMastery)
{
	return 0;
}


DEFINE_SKILL(CounterAttack)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);
	Position currPos=lpSkillUser->GetCurrentPos();
	 	if(((currPos.m_fPointX >= 1824)&&(currPos.m_fPointX <= 2250))&&((currPos.m_fPointZ >= 2817)&&(currPos.m_fPointZ <= 3189))){if(lpVictim->IsSafeZones(lpSkillUser , lpVictim)) {return 0;}}
		//return 0;
	 

	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

	const unsigned short wEnchantLevel = attackType.m_cSkillLevel + GetSkillLockCount(lpSkillUser, attackType) * 6;
	const unsigned long dwDurationSec = (attackType.m_cSkillLevel + GetSkillLockCount(lpSkillUser, attackType) * 6) * 2;

	unsigned short wResult = CAddSpell<CCounterAttackSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
		SpellType::MAGICAL_SPELL, SpellID::CounterAttack, wEnchantLevel, dwDurationSec))(lpVictim);
	
	switch (wResult)
	{
		case CSpell::NO_ENCHANT_ERROR :
		case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
			cDefenserJudge = ClientConstants::Judge_Enchant;
			break;

		case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
			wError = PktAtAck::FAIL_SPELL;
			break;

		case CSpell::ENCHANT_FAIL_BY_RESIST :
			cDefenserJudge = ClientConstants::Judge_Resist;
			break;
	}

	return 0;
}


DEFINE_SKILL(CounterAttackMotion)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	 Position currPos=lpSkillUser->GetCurrentPos();
	 	if(((currPos.m_fPointX >= 1824)&&(currPos.m_fPointX <= 2250))&&((currPos.m_fPointZ >= 2817)&&(currPos.m_fPointZ <= 3189))){if(lpVictim->IsSafeZones(lpSkillUser , lpVictim)) {return 0;}}
		//return 0;

	// CounterAttack의 부가적인 효과이므로 MP를 깍을 필요는 없다.
	
	unsigned short wDamage = 0;

	CSpell *lpSpell = lpSkillUser->GetSpellMgr().GetAffectedInfo().GetSpell(Skill::SpellID::CounterAttack);
	if (NULL != lpSpell)
	{
		float fAddRate = (200 + 22 * lpSpell->GetSpellLevel()) / 100.0f;

	// edith 2009.03.15 스킬공격시 패시브 스킬 포함되게 수정
//		wDamage = lpSkillUser->MeleeAttack(lpVictim, Creature::RIGHT_NON_PASSIVE_TYPE, 
		wDamage = lpSkillUser->MeleeAttack(lpVictim, Creature::RIGHT_PASSIVE_TYPE, 
			LevelFactor(lpSkillUser, lpVictim), cDefenserJudge);

		long lDamage = static_cast<long>(wDamage * fAddRate);
		wDamage = static_cast<unsigned short>(min(lDamage, USHRT_MAX));

		lpVictim->GetThreat().AffectThreat(lpSkillUser, wDamage, CThreat::DETAUNT);
	}

	return wDamage;
}

DEFINE_SKILL(DemonicCry)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	 Position currPos=lpSkillUser->GetCurrentPos();
	 	if(((currPos.m_fPointX >= 1824)&&(currPos.m_fPointX <= 2250))&&((currPos.m_fPointZ >= 2817)&&(currPos.m_fPointZ <= 3189))){if(lpVictim->IsSafeZones(lpSkillUser , lpVictim)) {return 0;}}
		//return 0;

	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

	lpVictim->GetSpellMgr().GetAffectedInfo().Disenchant(SpellType::NONE, SpellTarget::ENEMY_TARGET_ENCHANT, 
		Disenchant::NONE, 0, Disenchant::INFINITE_NUM);

	unsigned short wResult = CAddSpell<CBlazeSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
		SpellType::MAGICAL_SPELL, SpellID::Blaze, lpSkillUser->GetStatus().m_nLevel, 30))(lpVictim);
	
	switch (wResult)
	{
		case CSpell::NO_ENCHANT_ERROR :
		case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
			cDefenserJudge = ClientConstants::Judge_Enchant;
			break;

		case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
			wError = PktAtAck::FAIL_SPELL;
			break;

		case CSpell::ENCHANT_FAIL_BY_RESIST :
			cDefenserJudge = ClientConstants::Judge_Resist;
			break;
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------
// 어쌔신 스킬

DEFINE_SKILL(BackStab)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim); 
	Position currPos=lpSkillUser->GetCurrentPos();
		if(((currPos.m_fPointX >= 1824)&&(currPos.m_fPointX <= 2250))&&((currPos.m_fPointZ >= 2817)&&(currPos.m_fPointZ <= 3189))){if(lpVictim->IsSafeZones(lpSkillUser , lpVictim)) {return 0;}}
	//	return 0;

	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

	unsigned short wLockCount = GetSkillLockCount(lpSkillUser, attackType);
	float fAddRate = (true == lpSkillUser->GetEnchantInfo().GetFlag(SpellID::Stealth)) ? 
	//	(300 + 45 * (attackType.m_cSkillLevel + wLockCount * 6)) / 100.0f : //was 50
	//	(150 + 10 * (attackType.m_cSkillLevel + wLockCount * 6)) / 100.0f;

		(300 + 55 * (attackType.m_cSkillLevel + wLockCount * 6)) / 100.0f : //was 50
		(150 + 15 * (attackType.m_cSkillLevel + wLockCount * 6)) / 100.0f;


	// edith 2009.03.15 스킬공격시 패시브 스킬 포함되게 수정
//	unsigned short wDamage = lpSkillUser->MeleeAttack(lpVictim, Creature::RIGHT_NON_PASSIVE_TYPE, 
	unsigned short wDamage = lpSkillUser->MeleeAttack(lpVictim, Creature::RIGHT_PASSIVE_TYPE, 
		LevelFactor(lpSkillUser, lpVictim), cDefenserJudge);

	long lDamage = static_cast<long>(wDamage * fAddRate);
	wDamage = static_cast<unsigned short>(min(lDamage, USHRT_MAX));

	char abilityLevel=GetAbilityLevel(lpSkillUser,0x1056);
	if(abilityLevel)
	{
		if (SuccessCripplingRate(lpSkillUser,abilityLevel) > Math::Random::ComplexRandom(100))
		{
			unsigned long duration=abilityLevel*2;//how much time the slow is going to hold.
		
			unsigned short wResult = CAddSpell<CSlowSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, SpellType::PHYSICAL_SPELL, SpellID::Slow, 
			lpSkillUser->GetStatus().m_nLevel, duration))(lpVictim);

			switch (wResult)
			{
				case CSpell::NO_ENCHANT_ERROR :
				case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
					cDefenserJudge = ClientConstants::Judge_Enchant;
					break;

				case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
					wError = PktAtAck::FAIL_SPELL;
					return 0;

				case CSpell::ENCHANT_FAIL_BY_RESIST :
					cDefenserJudge = ClientConstants::Judge_Resist;
					break;
			}

			lpVictim->GetThreat().AddToThreatList(lpSkillUser, 1);
		}
	}

	lpVictim->GetThreat().AffectThreat(lpSkillUser, wDamage, CThreat::DETAUNT);
	return wDamage;
}


DEFINE_SKILL(Stealth)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	 
	 

	short nConsumeMPAmount = ConsumeMP(attackType, lpSkillUser, cOffencerJudge);
	if (-1 == nConsumeMPAmount) { return 0; }
	
	const unsigned short wEnchantLevel = (attackType.m_cSkillLevel + GetSkillLockCount(lpSkillUser, attackType) * 6) * 2;
	unsigned long dwDurationSec = (attackType.m_cSkillLevel + GetSkillLockCount(lpSkillUser, attackType) * 6) * 2;

	//for Scout Ability:
	float finalDurationSec = static_cast<float>(dwDurationSec);
	float ScoutFactor=1;
	char ScoutLevel=GetAbilityLevel(lpSkillUser,0x1060);
	ScoutFactor=GetAbilityRatio(ScoutLevel,1.1,0.1);
	
	finalDurationSec*=ScoutFactor;

	dwDurationSec=static_cast<unsigned long>(finalDurationSec);
	//end
	unsigned short wResult = CAddSpell<CStealthSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
		SpellType::MAGICAL_SPELL, SpellID::Stealth, wEnchantLevel, dwDurationSec, attackType.m_cSkillLockCount, attackType.m_cSkillLevel))(lpVictim);
	
	switch (wResult)
	{
		case CSpell::NO_ENCHANT_ERROR :
			{
				if(4 <= attackType.m_cSkillLockCount)
				{
					if (true == CheckChantOff(attackType.m_cSkillLockCount, attackType.m_cSkillLevel))
					{
						lpSkillUser->GetSpellMgr().GetCastingInfo().ClearChant();

						cDefenserJudge = ClientConstants::Judge_Chant;
						return 0;
					}

					CProcessTable& processTable = CProcessTable::GetInstance();
					const CProcessTable::ProcessInfo* lpProcessInfo =  processTable.GetProcessInfo(0x9F01);

					if (NULL != lpProcessInfo)
					{
						CSpell* pSpell = new CSpeedBuffChantSpell(CSpell::Spell_Info(lpProcessInfo->m_lpProtoType[0], 
							lpSkillUser, SpellType::MAGICAL_SPELL, SpellID::SpeedBuff, (4 + (attackType.m_cSkillLevel * 4)), 
							dwDurationSec), 0);

						CParty* pParty = lpVictim->GetParty();
						if (NULL != pParty) 
						{
							pParty->GetPartySpellMgr().AddAffectedToAllMember(pSpell, lpSkillUser->GetMapIndex());
						} 
						else 
						{
							unsigned short wError = CSpell::NO_ENCHANT_ERROR;
							pSpell->AddAffected(lpVictim, wError);		
						}
						CGlobalSpellMgr::GetInstance().Add(pSpell);
					}
				}
				cDefenserJudge = ClientConstants::Judge_Chant;
			}
			break;
		case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
			cDefenserJudge = ClientConstants::Judge_Enchant;
			break;

		case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
			wError = PktAtAck::FAIL_SPELL;
			break;

		case CSpell::ENCHANT_FAIL_BY_RESIST :
			cDefenserJudge = ClientConstants::Judge_Resist;
			break;
	}

	return 0;
}


DEFINE_SKILL(DaggerMastery)
{
	return 0;
}


DEFINE_SKILL(BombSet)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);
	Position currPos=lpSkillUser->GetCurrentPos();
	 	if(((currPos.m_fPointX >= 1824)&&(currPos.m_fPointX <= 2250))&&((currPos.m_fPointZ >= 2817)&&(currPos.m_fPointZ <= 3189))){if(lpVictim->IsSafeZones(lpSkillUser , lpVictim)) {return 0;}}
		//return 0;
	 

	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

	unsigned short wResult = CAddSpell<CBombSetSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
		SpellType::MAGICAL_SPELL, SpellID::BombSet, 
		attackType.m_cSkillLevel + GetSkillLockCount(lpSkillUser, attackType) * 6, 5))(lpVictim);

	switch (wResult)
	{
		case CSpell::NO_ENCHANT_ERROR :
		case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
			cDefenserJudge = ClientConstants::Judge_Enchant;
			break;

		case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
			wError = PktAtAck::FAIL_SPELL;
			break;

		case CSpell::ENCHANT_FAIL_BY_RESIST :
			cDefenserJudge = ClientConstants::Judge_Resist;
			break;
	}

	return 0;
}


DEFINE_SKILL(Explosion)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);
	Position currPos=lpSkillUser->GetCurrentPos();
	 	if(((currPos.m_fPointX >= 1824)&&(currPos.m_fPointX <= 2250))&&((currPos.m_fPointZ >= 2817)&&(currPos.m_fPointZ <= 3189))){if(lpVictim->IsSafeZones(lpSkillUser , lpVictim)) {return 0;}}
	//	return 0;
	 

	// BombSet의 부가적인 효과이므로 MP를 깍을 필요는 없다.

	long lDamage = static_cast<long>(((attackType.m_cSkillLevel + attackType.m_cSkillLockCount * 6) * 6) * 
		50 * (1 + lpSkillUser->GetStatus().m_nLevel * 0.05f) * 
		(100 - lpVictim->GetStatus().m_StatusInfo.m_fBlockRate) / 100.0f * 
		LevelFactor(lpSkillUser, lpVictim) * (75 + Math::Random::ComplexRandom(50)) / 100.0f);

	return static_cast<unsigned short>(min(lDamage, USHRT_MAX));
}

DEFINE_SKILL(StealHand)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);


	if (true == CheckChantOff(attackType.m_cSkillLockCount, attackType.m_cSkillLevel))
	{
		lpSkillUser->GetSpellMgr().GetCastingInfo().ClearChant();

		cDefenserJudge = ClientConstants::Judge_Chant;
		return 0;
	}

	 
	 

	short nConsumeMPAmount = ConsumeMP(attackType, lpSkillUser, cOffencerJudge);
	if (-1 == nConsumeMPAmount) { return 0; }

	// 1~12렙까지는 골드드랍율 증가.
	// 13랩부터는 골드드랍율 + 럭찬 퍼센트 증가
	// iLevel 이 12 이상이면.. 챈트 레벨이 증가 이부분을 좀 수정해야함.
	int iLevel = (attackType.m_cSkillLockCount*6) + attackType.m_cSkillLevel;

	CSpell* pSpell = new CStealHandSpell(CSpell::Spell_Info(ProtoType, lpSkillUser, 
		SpellType::MAGICAL_SPELL, SpellID::StealHand, iLevel, CSpell::INFINITE_DURATION), nConsumeMPAmount);

	CParty* pParty = lpVictim->GetParty();
	if (NULL != pParty) 
	{
		pParty->GetPartySpellMgr().AddAffectedToAllMember(pSpell, lpSkillUser->GetMapIndex());
	} 
	else 
	{
		unsigned short wError = CSpell::NO_ENCHANT_ERROR;
		pSpell->AddAffected(lpVictim, wError);		
	}
	CGlobalSpellMgr::GetInstance().Add(pSpell);

	cDefenserJudge = ClientConstants::Judge_Chant;
	return 0;
}



//-------------------------------------------------------------------------------------------------
// 아처 스킬

DEFINE_SKILL(AimedShot)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);
	Position currPos=lpSkillUser->GetCurrentPos();
		if(((currPos.m_fPointX >= 1824)&&(currPos.m_fPointX <= 2250))&&((currPos.m_fPointZ >= 2817)&&(currPos.m_fPointZ <= 3189))){if(lpVictim->IsSafeZones(lpSkillUser , lpVictim)) {return 0;}}
	//	return 0;

	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

	float fAddRate = (true == lpSkillUser->GetEnchantInfo().GetFlag(SpellID::Stealth)) ? 
		(300 + 75 * (attackType.m_cSkillLevel + attackType.m_cSkillLockCount * 6)) / 100.0f : //was 60
		(150 + 20 * (attackType.m_cSkillLevel + attackType.m_cSkillLockCount * 6)) / 100.0f;
    // Gunner skill boom
	//	(300 + 75 * (attackType.m_cSkillLevel + attackType.m_cSkillLockCount * 6)) / 100.0f : //was 90
	//	(150 + 20 * (attackType.m_cSkillLevel + attackType.m_cSkillLockCount * 6)) / 100.0f;

	// Rodin : 화살 소모

	unsigned int ExType = 0;

	Creature::CreatureType eCreatureType = Creature::GetCreatureType(lpSkillUser->GetCID()); 
	Creature::CreatureType eDefenderCreatureType = Creature::GetCreatureType(lpVictim->GetCID());

	if (Creature::CT_MONSTER == eCreatureType ||
		Creature::CT_SUMMON == eCreatureType ||
		Creature::CT_STRUCT == eCreatureType ||
		Creature::CT_MONSTER == eDefenderCreatureType ||
		Creature::CT_SUMMON == eDefenderCreatureType ||
		Creature::CT_STRUCT == eDefenderCreatureType)
	{
		ExType = 0;
	}
	else
	{
		// edith 2010.01.03 1/5로 크리티컬 데미지 영향을 준다
		//ExType = Math::Random::ComplexRandom(5);
		ExType = 0;
	}

	// edith 2009.03.15 스킬공격시 패시브 스킬 포함되게 수정
//	unsigned short wDamage = lpSkillUser->MeleeAttack(lpVictim, Creature::RIGHT_NON_PASSIVE_TYPE, 
	unsigned short wDamage = lpSkillUser->MeleeAttack(lpVictim, Creature::RIGHT_PASSIVE_TYPE, 
		LevelFactor(lpSkillUser, lpVictim), cDefenserJudge, ExType);

	long lDamage = static_cast<long>(wDamage * fAddRate);
	wDamage = static_cast<unsigned short>(min(lDamage, USHRT_MAX));

	char abilityLevel=GetAbilityLevel(lpSkillUser,0x1056);
	if(abilityLevel)
	{
		if (SuccessCripplingRate(lpSkillUser,abilityLevel) > Math::Random::ComplexRandom(100))
		{
			unsigned long duration=abilityLevel*2;//how much time the slow is going to hold.
		
			unsigned short wResult = CAddSpell<CSlowSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, SpellType::PHYSICAL_SPELL, SpellID::Slow, 
			lpSkillUser->GetStatus().m_nLevel, duration))(lpVictim);

			switch (wResult)
			{
				case CSpell::NO_ENCHANT_ERROR :
				case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
					cDefenserJudge = ClientConstants::Judge_Enchant;
					break;

				case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
					wError = PktAtAck::FAIL_SPELL;
					return 0;

				case CSpell::ENCHANT_FAIL_BY_RESIST :
					cDefenserJudge = ClientConstants::Judge_Resist;
					break;
			}

			lpVictim->GetThreat().AddToThreatList(lpSkillUser, 1);
		}
	}

	return wDamage;
}
DEFINE_SKILL(AgilityAura){
	return 0;
}

DEFINE_SKILL(DualShot)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	 Position currPos=lpSkillUser->GetCurrentPos();
	 	if(((currPos.m_fPointX >= 1824)&&(currPos.m_fPointX <= 2250))&&((currPos.m_fPointZ >= 2817)&&(currPos.m_fPointZ <= 3189))){if(lpVictim->IsSafeZones(lpSkillUser , lpVictim)) {return 0;}}
	//	return 0;


	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

	// Rodin : 화살 소모

	unsigned short wLockCount = GetSkillLockCount(lpSkillUser, attackType);
	float fAddRate = (90 + 10 * (attackType.m_cSkillLevel + wLockCount * 6)) / 100.0f;

	if(4 <= attackType.m_cSkillLockCount)
	{
		unsigned char cSuccessRate = static_cast<unsigned char>(20 + (attackType.m_cSkillLevel * 5));
		if (cSuccessRate > Math::Random::ComplexRandom(100))
		{
			if (false == SlowlySkillAttack(ProtoType, attackType, lpSkillUser, lpVictim))
			{
				wError = PktAtAck::FAIL_SPELL;
			}
		}
	}

	// edith 2009.03.15 스킬공격시 패시브 스킬 포함되게 수정
//	unsigned short wDamage = lpSkillUser->MeleeAttack(lpVictim, Creature::RIGHT_NON_PASSIVE_TYPE, 
	unsigned short wDamage = lpSkillUser->MeleeAttack(lpVictim, Creature::RIGHT_PASSIVE_TYPE, 
		LevelFactor(lpSkillUser, lpVictim), cDefenserJudge);

	long lDamage = static_cast<long>(wDamage * fAddRate);
	wDamage = static_cast<unsigned short>(min(lDamage, USHRT_MAX));

	return wDamage;
}


DEFINE_SKILL(Camouflage)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	 
	 

	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

	const unsigned short wEnchantLevel = attackType.m_cSkillLevel + GetSkillLockCount(lpSkillUser, attackType) * 6;
	unsigned long dwDurationSec = (attackType.m_cSkillLevel + GetSkillLockCount(lpSkillUser, attackType) * 6) * 2;

	//for Scout Ability:
	float finalDurationSec = static_cast<float>(dwDurationSec);
	float ScoutFactor=1;
	char ScoutLevel=GetAbilityLevel(lpSkillUser,0x1060);

	ScoutFactor=GetAbilityRatio(ScoutLevel,1.1,0.1);
	
	finalDurationSec*=ScoutFactor;

	dwDurationSec=static_cast<unsigned long>(finalDurationSec);
	//end

	unsigned short wResult = CAddSpell<CStealthSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
		SpellType::MAGICAL_SPELL, SpellID::Stealth, wEnchantLevel, dwDurationSec))(lpVictim);

	switch (wResult)
	{
		case CSpell::NO_ENCHANT_ERROR :
		case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
			cDefenserJudge = ClientConstants::Judge_Enchant;
			break;

		case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
			wError = PktAtAck::FAIL_SPELL;
			break;

		case CSpell::ENCHANT_FAIL_BY_RESIST :
			cDefenserJudge = ClientConstants::Judge_Resist;
			break;
	}

	return 0;
}


DEFINE_SKILL(Accuracy)
{
	return 0;
}



//-------------------------------------------------------------------------------------------------
// 소서러 스킬

DEFINE_SKILL(Recall)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

	CParty* pParty = lpVictim->GetParty();
	if (NULL != pParty)
	{
		if (Creature::CT_PC == Creature::GetCreatureType(lpVictim->GetCID()))
		{
			CCharacter* lpCharacter = reinterpret_cast<CCharacter *>(lpVictim);
			lpCharacter->SetCastingPos();
			reinterpret_cast<CCharacterParty* >(pParty)->SendRecall(lpCharacter);
		}
	}

	cOffencerJudge = ClientConstants::Judge_NonShow;
	cDefenserJudge = ClientConstants::Judge_NonShow;
	return 0;
}


DEFINE_SKILL(FireBolt)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);
	Position currPos=lpSkillUser->GetCurrentPos();
		if(((currPos.m_fPointX >= 1824)&&(currPos.m_fPointX <= 2250))&&((currPos.m_fPointZ >= 2817)&&(currPos.m_fPointZ <= 3189))){if(lpVictim->IsSafeZones(lpSkillUser , lpVictim)) {return 0;}}
		//return 0;

	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

	if(4 <= attackType.m_cSkillLockCount)
	{
		char successRate=GetStickyMagicSuccessRate(lpSkillUser,lpVictim,0.5f);

		char PoisonBreakerLevel=GetAbilityLevel(lpVictim,0x105E);
		float PoisonBreakerRatio=GetAbilityRatio(PoisonBreakerLevel,1.10,0.10);
		float finalSuccessRate=successRate;
		finalSuccessRate*=PoisonBreakerRatio;
		successRate=static_cast<unsigned char>(finalSuccessRate);

		if (successRate > Math::Random::ComplexRandom(100))
		{
			//unsigned short wEnchantLevel = 70 + (attackType.m_cSkillLevel * 30);hz was this
			unsigned short wEnchantLevel = attackType.m_cSkillLevel*10 + GetSkillLockCount(lpSkillUser, attackType) * 6;
			unsigned long dwDurationSec = static_cast<unsigned long>(20 * LevelFactor(lpSkillUser, lpVictim));

			unsigned short wResult = CAddSpell<CFiredSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
				SpellType::MAGICAL_SPELL, SpellID::Fired, wEnchantLevel, dwDurationSec))(lpVictim);

			switch (wResult)
			{
			case CSpell::NO_ENCHANT_ERROR :
			case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
				break;

			case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
				wError = PktAtAck::FAIL_SPELL;
				break;

			case CSpell::ENCHANT_FAIL_BY_RESIST :
				cDefenserJudge = ClientConstants::Judge_Resist;
				break;
			}
		}
		else
		{
			cDefenserJudge = ClientConstants::Judge_Resist;
		}
	}

	return CastMagicDamage(lpSkillUser, lpVictim, attackType, (attackType.m_cSkillLockCount < 3) ? 20 : 25);
}


DEFINE_SKILL(LightningArrow)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);
	Position currPos=lpSkillUser->GetCurrentPos();
	 	if(((currPos.m_fPointX >= 1824)&&(currPos.m_fPointX <= 2250))&&((currPos.m_fPointZ >= 2817)&&(currPos.m_fPointZ <= 3189))){if(lpVictim->IsSafeZones(lpSkillUser , lpVictim)) {return 0;}}
		//return 0;
	 

	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }
	char successRate=GetStickyMagicSuccessRate(lpSkillUser,lpVictim,0.5f);

	if (successRate > Math::Random::ComplexRandom(100))
	{
		unsigned short wEnchantLevel = attackType.m_cSkillLevel + GetSkillLockCount(lpSkillUser, attackType) * 6;
		unsigned long dwDurationSec = CastEnchantTick(lpSkillUser, lpVictim, attackType, 0.5f);
		dwDurationSec=static_cast<unsigned long>(dwDurationSec*0.7f);
		unsigned short wResult = CAddSpell<CStunSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
			SpellType::MAGICAL_SPELL, SpellID::Stun, wEnchantLevel, dwDurationSec))(lpVictim);

		switch (wResult)
		{
			case CSpell::NO_ENCHANT_ERROR :
			case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
				break;

			case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
				wError = PktAtAck::FAIL_SPELL;
				break;

			case CSpell::ENCHANT_FAIL_BY_RESIST :
				cDefenserJudge = ClientConstants::Judge_Resist;
				break;
		}
	}
	else
	{
		cDefenserJudge = ClientConstants::Judge_Resist;
	}

	return CastMagicDamage(lpSkillUser, lpVictim, attackType, (attackType.m_cSkillLockCount < 3) ? 15 : 20);
}


DEFINE_SKILL(FrostBolt)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);
	Position currPos=lpSkillUser->GetCurrentPos();
	 	if(((currPos.m_fPointX >= 1824)&&(currPos.m_fPointX <= 2250))&&((currPos.m_fPointZ >= 2817)&&(currPos.m_fPointZ <= 3189))){if(lpVictim->IsSafeZones(lpSkillUser , lpVictim)) {return 0;}}
		//return 0;
	 

	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

	char successRate=GetStickyMagicSuccessRate(lpSkillUser,lpVictim,0.5f);
	
	char IceBreakerLevel=GetAbilityLevel(lpVictim,0x105D);
	float IceBreakerRatio=GetAbilityRatio(IceBreakerLevel,1.10,0.10);

	float finalSuccessRate=successRate;
	finalSuccessRate*=IceBreakerRatio;
	successRate=static_cast<unsigned char>(finalSuccessRate);

	if (successRate > Math::Random::ComplexRandom(100))
	{
		unsigned short wEnchantLevel = attackType.m_cSkillLevel + GetSkillLockCount(lpSkillUser, attackType) * 6;
		unsigned long dwDurationSec = CastEnchantTick(lpSkillUser, lpVictim, attackType, 2);

		unsigned short wResult = CAddSpell<CFrozenSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
			SpellType::MAGICAL_SPELL, SpellID::Frozen, wEnchantLevel, dwDurationSec))(lpVictim);

		switch (wResult)
		{
			case CSpell::NO_ENCHANT_ERROR :
			case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
				break;

			case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
				wError = PktAtAck::FAIL_SPELL;
				break;

			case CSpell::ENCHANT_FAIL_BY_RESIST :
				cDefenserJudge = ClientConstants::Judge_Resist;
				break;
		}
	}
	else
	{
		cDefenserJudge = ClientConstants::Judge_Resist;
	}

	return CastMagicDamage(lpSkillUser, lpVictim, attackType, (attackType.m_cSkillLockCount < 3) ? 15 : 20);
}


DEFINE_SKILL(DeathRay)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	 Position currPos=lpSkillUser->GetCurrentPos();
	 	if(((currPos.m_fPointX >= 1824)&&(currPos.m_fPointX <= 2250))&&((currPos.m_fPointZ >= 2817)&&(currPos.m_fPointZ <= 3189))){if(lpVictim->IsSafeZones(lpSkillUser , lpVictim)) {return 0;}}
	//	return 0;

	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }
	return InstanceMagicDamage(lpSkillUser, lpVictim, attackType, 20);//was 40
}


DEFINE_SKILL(FreshToStone)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);
	Position currPos=lpSkillUser->GetCurrentPos();
	 	if(((currPos.m_fPointX >= 1824)&&(currPos.m_fPointX <= 2250))&&((currPos.m_fPointZ >= 2817)&&(currPos.m_fPointZ <= 3189))){if(lpVictim->IsSafeZones(lpSkillUser , lpVictim)) {return 0;}}
		//return 0;
	 

	char successRate=GetStickyMagicSuccessRate(lpSkillUser,lpVictim,1);


	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

	bool bUseSkill = false;
	if (lpSkillUser == lpVictim)
	{
		bUseSkill = true;
		cDefenserJudge = ClientConstants::Judge_Enchant;
	}
	else if (successRate > Math::Random::ComplexRandom(100))
	{
		bUseSkill = true;
		cDefenserJudge = ClientConstants::Judge_Enchant;
	}
	else
	{
		cDefenserJudge = ClientConstants::Judge_Resist;
	}

	if (bUseSkill)
	{
		unsigned short wEnchantLevel = attackType.m_cSkillLevel + GetSkillLockCount(lpSkillUser, attackType) * 6;
		unsigned long dwDurationSec = InstanceEnemyEnchantTick(lpSkillUser, lpVictim, attackType, 1.5f);

		unsigned short wResult = CAddSpell<CStoneFormSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
			SpellType::MAGICAL_SPELL, SpellID::StoneForm, wEnchantLevel, dwDurationSec))(lpVictim);

		switch (wResult)
		{
			case CSpell::NO_ENCHANT_ERROR :
			case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
				{
					cDefenserJudge = ClientConstants::Judge_Enchant;
					
					lpVictim->GetThreat().AddToThreatList(lpSkillUser, 1);

					Creature::CreatureType eCreatureType = Creature::GetCreatureType(lpVictim->GetCID());
					if (Creature::CT_MONSTER == eCreatureType ||
						Creature::CT_SUMMON == eCreatureType ||
						Creature::CT_STRUCT == eCreatureType)
					{
						CMonster* lpMonster = reinterpret_cast<CMonster *>(lpVictim);
						lpMonster->Attacked();
					}
				}
				break;

			case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
				wError = PktAtAck::FAIL_SPELL;
				break;

			case CSpell::ENCHANT_FAIL_BY_RESIST :
				cDefenserJudge = ClientConstants::Judge_Resist;
				break;
		}
	}

	return 0;
}



//-------------------------------------------------------------------------------------------------
// 엔찬터 스킬

DEFINE_SKILL(Disenchant)
{
	 
	 

	if (0 != lpVictim->GetSpellMgr().GetAffectedInfo().Disenchant(SpellType::MAGICAL_SPELL, SpellTarget::FRIEND_TARGET_ENCHANT, 
		Disenchant::NONE, 0, Disenchant::INFINITE_NUM))
	{
        lpVictim->GetThreat().AddToThreatList(lpSkillUser, 1);

		Creature::CreatureType eCreatureType = Creature::GetCreatureType(lpVictim->GetCID());
		if (Creature::CT_MONSTER == eCreatureType ||
			Creature::CT_SUMMON == eCreatureType ||
			Creature::CT_STRUCT == eCreatureType)
		{
			CMonster* lpMonster = reinterpret_cast<CMonster *>(lpVictim);
			lpMonster->Attacked();
		}
	}

	cDefenserJudge = ClientConstants::Judge_Disenchant;
	return 0;
}


DEFINE_SKILL(Entangle)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);
	Position currPos=lpSkillUser->GetCurrentPos();
	 	if(((currPos.m_fPointX >= 1824)&&(currPos.m_fPointX <= 2250))&&((currPos.m_fPointZ >= 2817)&&(currPos.m_fPointZ <= 3189))){if(lpVictim->IsSafeZones(lpSkillUser , lpVictim)) {return 0;}}
	//	return 0;
	 

	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

	char successRate=GetStickyMagicSuccessRate(lpSkillUser,lpVictim,1);

	if (successRate > Math::Random::ComplexRandom(100))
	{
		unsigned short wEnchantLevel = attackType.m_cSkillLevel + GetSkillLockCount(lpSkillUser, attackType) * 6;
		unsigned long dwDurationSec = CastEnchantTick(lpSkillUser, lpVictim, attackType, 2);

		unsigned short wResult = CAddSpell<CHoldSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
			SpellType::MAGICAL_SPELL, SpellID::Hold, wEnchantLevel, dwDurationSec))(lpVictim);

		switch (wResult)
		{
			case CSpell::NO_ENCHANT_ERROR :
			case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
				cDefenserJudge = ClientConstants::Judge_Enchant;
				break;

			case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
				wError = PktAtAck::FAIL_SPELL;
				break;

			case CSpell::ENCHANT_FAIL_BY_RESIST :
				cDefenserJudge = ClientConstants::Judge_Resist;
				break;
		}
	}
	else
	{
		cDefenserJudge = ClientConstants::Judge_Resist;
	}

	// CastMagicDamage() 함수중에서 wLockCount 곱셈이 하나 빠진 공식이다.!!
	const unsigned char cFactor = 10;
	const unsigned short wLockCount = GetSkillLockCount(lpSkillUser, attackType);

	long lDamage = static_cast<long>(((wLockCount + 1) * attackType.m_cSkillLevel + 1 + attackType.m_cSkillLockCount * 5) *
		cFactor * (75 + Math::Random::ComplexRandom(75) + lpSkillUser->GetStatus().m_StatusInfo.m_wMagicPower) / 100.0f * 
		(100 - lpVictim->GetStatus().m_StatusInfo.m_fMagicResistRate) / 100.0f * LevelFactor(lpSkillUser, lpVictim));

	return static_cast<unsigned short>(min(lDamage, USHRT_MAX));
}


DEFINE_SKILL(LowerStrength)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	 Position currPos=lpSkillUser->GetCurrentPos();
	 	if(((currPos.m_fPointX >= 1824)&&(currPos.m_fPointX <= 2250))&&((currPos.m_fPointZ >= 2817)&&(currPos.m_fPointZ <= 3189))){if(lpVictim->IsSafeZones(lpSkillUser , lpVictim)) {return 0;}}
		//return 0;

	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

	char successRate=GetStickyMagicSuccessRate(lpSkillUser,lpVictim,1);

	if (successRate > Math::Random::ComplexRandom(100))
	{
		unsigned short wEnchantLevel = CastEnchantLevel(lpSkillUser, lpVictim, attackType, 1);
		unsigned long dwDurationSec = static_cast<unsigned long>(20 * LevelFactor(lpSkillUser, lpVictim));

		unsigned short wResult = CAddSpell<CLowerStrengthSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
			SpellType::MAGICAL_SPELL, SpellID::LowerStrength, wEnchantLevel, dwDurationSec))(lpVictim);

		switch (wResult)
		{
			case CSpell::NO_ENCHANT_ERROR :
			case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
				{
					cDefenserJudge = ClientConstants::Judge_Enchant;

					lpVictim->GetThreat().AddToThreatList(lpSkillUser, 1);

					Creature::CreatureType eCreatureType = Creature::GetCreatureType(lpVictim->GetCID());
					if (Creature::CT_MONSTER == eCreatureType ||
						Creature::CT_SUMMON == eCreatureType ||
						Creature::CT_STRUCT == eCreatureType)
					{
						CMonster* lpMonster = reinterpret_cast<CMonster *>(lpVictim);
						lpMonster->Attacked();
					}
				}
				break;

			case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
				wError = PktAtAck::FAIL_SPELL;
				break;

			case CSpell::ENCHANT_FAIL_BY_RESIST :
				cDefenserJudge = ClientConstants::Judge_Resist;
				break;
		}
	}
	else
	{
		cDefenserJudge = ClientConstants::Judge_Resist;
	}
	
	return 0;
}


DEFINE_SKILL(EnchantWeapon)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	 
	 

	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

	unsigned short wEnchantLevel;

	if(4 <= attackType.m_cSkillLockCount)
	{
		wEnchantLevel = static_cast<unsigned short>(CastEnchantSkillLevel(lpSkillUser, lpVictim, attackType, 1, 0.5f, 116));
	}
	else
	{
		wEnchantLevel = static_cast<unsigned short>(CastEnchantSkillLevel(lpSkillUser, lpVictim, attackType, 1, 1.3f));
	}

	char BuffMasteryLevel=GetAbilityLevel(lpSkillUser,0x1061);
	float BuffMasteryRatio=GetAbilityRatio(BuffMasteryLevel,1.1,0.1);

	unsigned long dwDurationSec = static_cast<unsigned long>(((100 + lpSkillUser->GetStatus().m_StatusInfo.m_wMagicPower / 8.0f) * 
		(attackType.m_cSkillLockCount + 1))*BuffMasteryRatio);

	unsigned short wResult = CAddSpell<CEnchantWeaponSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
		SpellType::MAGICAL_SPELL, SpellID::EnchantWeapon, wEnchantLevel, dwDurationSec))(lpVictim);
	
	switch (wResult)
	{
		case CSpell::NO_ENCHANT_ERROR :
			if(4 <= attackType.m_cSkillLockCount)
			{
				CProcessTable& processTable = CProcessTable::GetInstance();
				const CProcessTable::ProcessInfo* lpProcessInfo =  processTable.GetProcessInfo(0x9F02);

				if (NULL != lpProcessInfo)
				{
                    CAddSpell<CDeCoolDownSpell>(CSpell::Spell_Info(lpProcessInfo->m_lpProtoType[0], lpSkillUser, 
						SpellType::MAGICAL_SPELL, SpellID::DeCoolDown, (3 + (attackType.m_cSkillLevel * 3)), dwDurationSec))(lpVictim);
				}
			}
			cDefenserJudge = ClientConstants::Judge_Enchant;
			break;
		case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
			cDefenserJudge = ClientConstants::Judge_Enchant;
			break;

		case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
			wError = PktAtAck::FAIL_SPELL;
			break;

		case CSpell::ENCHANT_FAIL_BY_RESIST :
			cDefenserJudge = ClientConstants::Judge_Resist;
			break;
	}

	return 0;
}


DEFINE_SKILL(SummonKindling)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);
	Position currPos=lpSkillUser->GetCurrentPos();
	 	if(((currPos.m_fPointX >= 1824)&&(currPos.m_fPointX <= 2250))&&((currPos.m_fPointZ >= 2817)&&(currPos.m_fPointZ <= 3189))){if(lpVictim->IsSafeZones(lpSkillUser , lpVictim)) {return 0;}}
	//	return 0;
	 

	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

	unsigned short nKID = 0;
	switch (attackType.m_cSkillLockCount) 
	{
		case 0:	nKID = MonsterInfo::SUMMON_KINDLING + attackType.m_cSkillLevel; 		break;
		case 1:	nKID = MonsterInfo::SUMMON_FLAMEWALKER + attackType.m_cSkillLevel; 		break;
		case 2: nKID = MonsterInfo::SUMMON_BURNINGSHIELD + attackType.m_cSkillLevel;	break;
		case 3: nKID = MonsterInfo::SUMMON_FIREWING + attackType.m_cSkillLevel;			break;
		case 4: nKID = MonsterInfo::SUMMON_DEATHBURN + attackType.m_cSkillLevel;		break;
	}

	Position DestPos;
	DestPos.m_fPointX = lpSkillUser->GetCurrentPos().m_fPointX - 
		cosf(lpSkillUser->GetMotionInfo().m_fDirection) * ProtoType.m_fMaxRange;
	DestPos.m_fPointZ = lpSkillUser->GetCurrentPos().m_fPointZ + 
		sinf(lpSkillUser->GetMotionInfo().m_fDirection) * ProtoType.m_fMaxRange;
	DestPos.m_fPointY = lpSkillUser->GetCurrentPos().m_fPointY + 1;

	if (Creature::CT_PC == Creature::GetCreatureType(lpSkillUser->GetCID()))
	{
		CCharacter* lpCharSkillUser = reinterpret_cast<CCharacter *>(lpSkillUser);

		if (0 != lpCharSkillUser->GetMapIndex())
		{
			VirtualArea::CVirtualArea* lpVirtualArea = VirtualArea::CVirtualAreaMgr::GetInstance().GetVirtualArea(lpCharSkillUser->GetMapIndex());
			if (lpVirtualArea)
			{
				CVirtualMonsterMgr* lpVirtualMonsterManager = lpVirtualArea->GetMonsterManager();
				if (NULL == lpVirtualMonsterManager || false == lpVirtualMonsterManager->SummonMonster(nKID, DestPos, lpCharSkillUser))
				{
					ERRLOG2(g_Log, "CID:0x%08x 소환 스킬을 실패하였습니다. 소환수 종류 : %d", lpCharSkillUser->GetCID(), nKID);
					return 0;
				}
			}
		}
		else
		{
			if (false == CCellManager::GetInstance().SummonMonster(nKID, DestPos, lpCharSkillUser))
			{
				ERRLOG2(g_Log, "CID:0x%08x 소환 스킬을 실패하였습니다. 소환수 종류 : %d", 
					lpCharSkillUser->GetCID(), nKID);
				return 0;
			}
		}
	}

	cOffencerJudge = ClientConstants::Judge_NonShow;
	cDefenserJudge = ClientConstants::Judge_NonShow;

	return 0;
}


DEFINE_SKILL(Shatter)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);
	Position currPos=lpSkillUser->GetCurrentPos();
	 	if(((currPos.m_fPointX >= 1824)&&(currPos.m_fPointX <= 2250))&&((currPos.m_fPointZ >= 2817)&&(currPos.m_fPointZ <= 3189))){if(lpVictim->IsSafeZones(lpSkillUser , lpVictim)) {return 0;}}
	//	return 0;
	 

	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

	lpVictim->GetSpellMgr().GetAffectedInfo().Disenchant(SpellType::MAGICAL_SPELL, SpellTarget::FRIEND_TARGET_ENCHANT, 
		Disenchant::NONE, 0, Disenchant::INFINITE_NUM);

	char successRate=GetStickyMagicSuccessRate(lpSkillUser,lpVictim,0.5f);

	if (successRate > Math::Random::ComplexRandom(100))
	{
		unsigned short wEnchantLevel = attackType.m_cSkillLevel + GetSkillLockCount(lpSkillUser, attackType) * 6;
		unsigned long dwDurationSec = InstanceEnemyEnchantTick(lpSkillUser, lpVictim, attackType, 0.5f);

		unsigned short wResult = CAddSpell<CSlowSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
			SpellType::MAGICAL_SPELL, SpellID::Slow, wEnchantLevel, dwDurationSec))(lpVictim);

		switch (wResult)
		{
			case CSpell::NO_ENCHANT_ERROR :
			case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
				break;

			case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
				wError = PktAtAck::FAIL_SPELL;
				break;

			case CSpell::ENCHANT_FAIL_BY_RESIST :
				cDefenserJudge = ClientConstants::Judge_Resist;
				break;
		}
	}
	else
	{
		cDefenserJudge = ClientConstants::Judge_Resist;
	}

	return InstanceMagicDamage(lpSkillUser, lpVictim, attackType, 20);
}



//-------------------------------------------------------------------------------------------------
// 프리스트 스킬

DEFINE_SKILL(Resurrection)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	 
	 

	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

	if (0 != lpVictim->GetStatus().m_nNowHP) { return 0; }

	if (Creature::CT_PC == Creature::GetCreatureType(lpSkillUser->GetCID()) &&
		Creature::CT_PC == Creature::GetCreatureType(lpVictim->GetCID()))
	{
		CCharacter* lpCharSkillUser = reinterpret_cast<CCharacter *>(lpSkillUser);
		CCharacter* lpCharVictim = reinterpret_cast<CCharacter *>(lpVictim);

		if (true == lpCharSkillUser->IsPeaceMode() && false == lpCharVictim->IsPeaceMode())
		{
			// edith 2009.04.26 미하엘사용. 반전 모드 캐릭터는 전투 모드 캐릭터에게 리저렉션을 시전할 수 없다.
			wError = PktAtAck::FAIL_PEACE_TO_WAR;
			return 0;
		}

		CGameClientDispatch* lpDispatch = lpCharVictim->GetDispatcher();
		if (NULL == lpDispatch) 
		{ 
			return 0; 
		}

		if (false == GameClientSendPacket::SendCharAuthorizePanel(lpDispatch->GetSendStream(), lpCharSkillUser, 
			PktAP::AP_RESURRECTION))
		{
			return 0;
		}
	}

	cOffencerJudge = ClientConstants::Judge_NonShow;
	cDefenserJudge = ClientConstants::Judge_NonShow;
	return 0;
}


DEFINE_SKILL(MaintenanceChant)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	if (true == CheckChantOff(attackType.m_cSkillLockCount, attackType.m_cSkillLevel))
	{
		lpSkillUser->GetSpellMgr().GetCastingInfo().ClearChant();

		cDefenserJudge = ClientConstants::Judge_Chant;
		return 0;
	}

	short nConsumeMPAmount = ConsumeMP(attackType, lpSkillUser, cOffencerJudge);
	if (-1 == nConsumeMPAmount) { return 0; }
	
	

	CSpell* pSpell = new CMaintenanceChantSpell(CSpell::Spell_Info(ProtoType, lpSkillUser, 
		SpellType::MAGICAL_SPELL, SpellID::MaintenanceChant,
		ChantLevel(lpSkillUser, attackType, 1), CSpell::INFINITE_DURATION), nConsumeMPAmount);

	CParty* pParty = lpVictim->GetParty();
	if (NULL != pParty) 
	{
		pParty->GetPartySpellMgr().AddAffectedToAllMember(pSpell, lpSkillUser->GetMapIndex());
	} 
	else 
	{
		unsigned short wError = CSpell::NO_ENCHANT_ERROR;
		pSpell->AddAffected(lpVictim, wError);		
	}
	CGlobalSpellMgr::GetInstance().Add(pSpell);

	cDefenserJudge = ClientConstants::Judge_Chant;
	return 0;
}


DEFINE_SKILL(AccelerationChant)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	if (true == CheckChantOff(attackType.m_cSkillLockCount, attackType.m_cSkillLevel))
	{
		lpSkillUser->GetSpellMgr().GetCastingInfo().ClearChant();

		cDefenserJudge = ClientConstants::Judge_Chant;
		return 0;
	}

	short nConsumeMPAmount = ConsumeMP(attackType, lpSkillUser, cOffencerJudge);
	if (-1 == nConsumeMPAmount) { return 0; }
	
	CSpell* pSpell = new CAccelerationChantSpell(CSpell::Spell_Info(ProtoType, lpSkillUser, 
		SpellType::MAGICAL_SPELL, SpellID::AccelerationChant,
		ChantLevel(lpSkillUser, attackType, 1), CSpell::INFINITE_DURATION), nConsumeMPAmount);

	CParty* pParty = lpVictim->GetParty();
	if (NULL != pParty) 
	{
		pParty->GetPartySpellMgr().AddAffectedToAllMember(pSpell, lpSkillUser->GetMapIndex());
	} 
	else 
	{
		unsigned short wError = CSpell::NO_ENCHANT_ERROR;
		pSpell->AddAffected(lpVictim, wError);		
	}
	CGlobalSpellMgr::GetInstance().Add(pSpell);

	cDefenserJudge = ClientConstants::Judge_Chant;
	return 0;
}


DEFINE_SKILL(BrightArmor)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

	unsigned short wEnchantLevel;

	char BuffMasteryLevel=GetAbilityLevel(lpSkillUser,0x1061);
	float BuffMasteryRatio=GetAbilityRatio(BuffMasteryLevel,1.1,0.1);

	unsigned long dwDurationSec = static_cast<unsigned long>(((100 + lpSkillUser->GetStatus().m_StatusInfo.m_wMagicPower / 8.0f) * 
		(attackType.m_cSkillLockCount + 1))*BuffMasteryRatio);


	if(4 <= attackType.m_cSkillLockCount)
	{
		wEnchantLevel = static_cast<unsigned short>(CastEnchantSkillLevel(lpSkillUser, lpVictim, attackType, 1, 0.5f, 116));
	}
	else
	{
		wEnchantLevel = static_cast<unsigned short>(CastEnchantSkillLevel(lpSkillUser, lpVictim, attackType, 1, 1.3f));
	}
	wEnchantLevel = static_cast<unsigned short>(wEnchantLevel / 2);//hz added this

	unsigned short wResult = CAddSpell<CBrightArmorSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
		SpellType::MAGICAL_SPELL, SpellID::BrightArmor, wEnchantLevel, dwDurationSec))(lpVictim);

	switch (wResult)
	{
		case CSpell::NO_ENCHANT_ERROR :
			if(4 <= attackType.m_cSkillLockCount)
			{
				CProcessTable& processTable = CProcessTable::GetInstance();
				const CProcessTable::ProcessInfo* lpProcessInfo =  processTable.GetProcessInfo(0x9F03);

				if (NULL != lpProcessInfo)
				{
					CAddSpell<CManaShellSpell>(CSpell::Spell_Info(lpProcessInfo->m_lpProtoType[0], lpSkillUser, 
						SpellType::MAGICAL_SPELL, SpellID::ManaShell, 5, dwDurationSec))(lpVictim);
				}
			}
			cDefenserJudge = ClientConstants::Judge_Enchant;
			break;
		case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
			cDefenserJudge = ClientConstants::Judge_Enchant;
			break;

		case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
			wError = PktAtAck::FAIL_SPELL;
			break;

		case CSpell::ENCHANT_FAIL_BY_RESIST :
			cDefenserJudge = ClientConstants::Judge_Resist;
			break;
	}

	return 0;
}


DEFINE_SKILL(ManaFlow)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	 
	 

	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

	const unsigned short wOldMP = lpVictim->m_CreatureStatus.m_nNowMP;
	const unsigned short wAdditionalMP = InstanceMagicRegen(lpSkillUser, attackType, 30);

	if (lpVictim->m_CreatureStatus.m_nNowMP > USHRT_MAX - wAdditionalMP)
	{
		lpVictim->m_CreatureStatus.m_nNowMP = lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxMP;
	}
	else
	{
		lpVictim->m_CreatureStatus.m_nNowMP += wAdditionalMP;
		lpVictim->m_CreatureStatus.m_nNowMP = 
			std::min(lpVictim->m_CreatureStatus.m_nNowMP, lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxMP);
	}

	cDefenserJudge = ClientConstants::Judge_ManaHeal;
	wDefenserMPHeal = (lpVictim->m_CreatureStatus.m_nNowMP > wOldMP) ? lpVictim->m_CreatureStatus.m_nNowMP - wOldMP : 0;
	return 0;
}


DEFINE_SKILL(ManaBurn)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	 Position currPos=lpSkillUser->GetCurrentPos();
	 	if(((currPos.m_fPointX >= 1824)&&(currPos.m_fPointX <= 2250))&&((currPos.m_fPointZ >= 2817)&&(currPos.m_fPointZ <= 3189))){if(lpVictim->IsSafeZones(lpSkillUser , lpVictim)) {return 0;}}
		//return 0;

	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

	unsigned short wDamage = 
		std::min(lpVictim->m_CreatureStatus.m_nNowMP, InstanceMagicDamage(lpSkillUser, lpVictim, attackType, 15));

	lpVictim->m_CreatureStatus.m_nNowMP -= wDamage;
    return wDamage;
}



//-------------------------------------------------------------------------------------------------
// 클레릭 스킬

DEFINE_SKILL(Dazzle)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);
	Position currPos=lpSkillUser->GetCurrentPos();
	 	if(((currPos.m_fPointX >= 1824)&&(currPos.m_fPointX <= 2250))&&((currPos.m_fPointZ >= 2817)&&(currPos.m_fPointZ <= 3189))){if(lpVictim->IsSafeZones(lpSkillUser , lpVictim)) {return 0;}}
	//	return 0;
	 

	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

	unsigned char successRate=GetStunMasterySuccessRate(lpSkillUser,lpVictim,0.5f);

		char StunBreakerLevel=GetAbilityLevel(lpVictim,0x105F);
		float StunBreakerRatio=GetAbilityRatio(StunBreakerLevel,1.10,0.10);
		float finalSuccessRate=successRate;
		finalSuccessRate*=StunBreakerRatio;
		successRate=static_cast<unsigned char>(finalSuccessRate);

	if (successRate > Math::Random::ComplexRandom(100))
	{
		unsigned short wEnchantLevel = attackType.m_cSkillLevel + GetSkillLockCount(lpSkillUser, attackType) * 6;
		unsigned long dwDurationSec = InstanceEnemyEnchantTick(lpSkillUser, lpVictim, attackType, 0.5f);
		dwDurationSec=static_cast<unsigned long>(dwDurationSec*0.7f);
		unsigned short wResult = CAddSpell<CStunSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
			SpellType::MAGICAL_SPELL, SpellID::Stun, wEnchantLevel, dwDurationSec))(lpVictim);

		switch (wResult)
		{
			case CSpell::NO_ENCHANT_ERROR :
			case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
				{
					cDefenserJudge = ClientConstants::Judge_Enchant;

					lpVictim->GetThreat().AddToThreatList(lpSkillUser, 1);

					Creature::CreatureType eCreatureType = Creature::GetCreatureType(lpVictim->GetCID());
					if (Creature::CT_MONSTER == eCreatureType ||
						Creature::CT_SUMMON == eCreatureType ||
						Creature::CT_STRUCT == eCreatureType)
					{
						CMonster* lpMonster = reinterpret_cast<CMonster *>(lpVictim);
						lpMonster->Attacked();
					}
				}
                break;

			case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
				wError = PktAtAck::FAIL_SPELL;
				break;

			case CSpell::ENCHANT_FAIL_BY_RESIST :
				cDefenserJudge = ClientConstants::Judge_Resist;
				break;
		}
	}
	else
	{
		cDefenserJudge = ClientConstants::Judge_Resist;
	}

	return 0;
}


DEFINE_SKILL(CureWounds)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	 Position currPos=lpSkillUser->GetCurrentPos();
	 	if(((currPos.m_fPointX >= 1824)&&(currPos.m_fPointX <= 2250))&&((currPos.m_fPointZ >= 2817)&&(currPos.m_fPointZ <= 3189))){if(lpVictim->IsSafeZones(lpSkillUser , lpVictim)) {return 0;}}
		//return 0;

	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

	// 3단계 이상 캐스팅시 성향(마법/물리)에 관계없이 부정적인 인챈트 모두 파괴
	if (2 <= attackType.m_cSkillLockCount)
	{
		lpVictim->GetSpellMgr().GetAffectedInfo().Disenchant(SpellType::NONE, SpellTarget::ENEMY_TARGET_ENCHANT, 
			Disenchant::NONE, 0, Disenchant::INFINITE_NUM);
	}
	// 2단계 이상 캐스팅시 마법 성향 부정적인 인챈트 모두 파괴 
	else if (1 <= attackType.m_cSkillLockCount)
	{
		lpVictim->GetSpellMgr().GetAffectedInfo().Disenchant(SpellType::MAGICAL_SPELL, SpellTarget::ENEMY_TARGET_ENCHANT, 
			Disenchant::NONE, 0, Disenchant::INFINITE_NUM);
	}

	const unsigned short wPreviousHP = lpVictim->m_CreatureStatus.m_nNowHP;
	const unsigned short wAdditionalHP = CastMagicRegen(lpSkillUser, attackType, 30);

	if (lpVictim->m_CreatureStatus.m_nNowHP > USHRT_MAX - wAdditionalHP)
	{
		lpVictim->m_CreatureStatus.m_nNowHP = lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxHP;
	}
	else
	{
		lpVictim->m_CreatureStatus.m_nNowHP += wAdditionalHP;
		lpVictim->m_CreatureStatus.m_nNowHP = 
			std::min(lpVictim->m_CreatureStatus.m_nNowHP, lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxHP);
	}

	lpVictim->GetThreat().HealThreat(lpSkillUser, (lpVictim->m_CreatureStatus.m_nNowHP - wPreviousHP) / 2);

	cDefenserJudge = ClientConstants::Judge_Heal;
	return 0;
}


DEFINE_SKILL(WoundsCrafting)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);
	Position currPos=lpSkillUser->GetCurrentPos();
	 	if(((currPos.m_fPointX >= 1824)&&(currPos.m_fPointX <= 2250))&&((currPos.m_fPointZ >= 2817)&&(currPos.m_fPointZ <= 3189))){if(lpVictim->IsSafeZones(lpSkillUser , lpVictim)) {return 0;}}
	//	return 0;
	 

	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

	unsigned short wHPCost = static_cast<unsigned short>((attackType.m_cSkillLevel + GetSkillLockCount(lpSkillUser, attackType) * 6) * 
		10 * (100 + lpSkillUser->GetStatus().m_StatusInfo.m_wMagicPower) / 100.0f);

	// edith 2009.01.08 운즈그래프트 사용시 에너지가 부족하면 사용불가
	// HP가 부족하면 발동되지 않는다.
	if (lpSkillUser->m_CreatureStatus.m_nNowHP <= wHPCost)
	{
		cDefenserJudge = ClientConstants::Judge_Heal;
	}
	else
	{
		lpSkillUser->m_CreatureStatus.m_nNowHP -= wHPCost;

		const unsigned short wPreviousHP = lpVictim->m_CreatureStatus.m_nNowHP;
		const unsigned short wAdditionalHP = InstanceMagicRegen(lpSkillUser, attackType, 45);

		if (lpVictim->m_CreatureStatus.m_nNowHP > USHRT_MAX - wAdditionalHP)
		{
			lpVictim->m_CreatureStatus.m_nNowHP = lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxHP;
		}
		else
		{
			lpVictim->m_CreatureStatus.m_nNowHP += wAdditionalHP;
			lpVictim->m_CreatureStatus.m_nNowHP = 
				std::min(lpVictim->m_CreatureStatus.m_nNowHP, lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxHP);
		}

		lpVictim->GetThreat().HealThreat(lpSkillUser, (lpVictim->m_CreatureStatus.m_nNowHP - wPreviousHP) / 2);

		cDefenserJudge = ClientConstants::Judge_Heal;
	}
	return 0;
}


DEFINE_SKILL(CureLight)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);
	Position currPos=lpSkillUser->GetCurrentPos();
	 	if(((currPos.m_fPointX >= 1824)&&(currPos.m_fPointX <= 2250))&&((currPos.m_fPointZ >= 2817)&&(currPos.m_fPointZ <= 3189))){if(lpVictim->IsSafeZones(lpSkillUser , lpVictim)) {return 0;}}
	//	return 0;
	 

	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

	const unsigned short wPreviousHP = lpVictim->m_CreatureStatus.m_nNowHP;
	const unsigned short wAdditionalHP = InstanceMagicRegen(lpSkillUser, attackType, 20);

	if (lpVictim->m_CreatureStatus.m_nNowHP > USHRT_MAX - wAdditionalHP)
	{
		lpVictim->m_CreatureStatus.m_nNowHP = lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxHP;
	}
	else
	{
		lpVictim->m_CreatureStatus.m_nNowHP += wAdditionalHP;
		lpVictim->m_CreatureStatus.m_nNowHP = 
			std::min(lpVictim->m_CreatureStatus.m_nNowHP, lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxHP);
	}

	lpVictim->GetThreat().HealThreat(lpSkillUser, (lpVictim->m_CreatureStatus.m_nNowHP - wPreviousHP) / 2);

	cDefenserJudge = ClientConstants::Judge_Heal;

	unsigned short wLockCount = GetSkillLockCount(lpSkillUser, attackType);
	if(4 <= attackType.m_cSkillLockCount)
	{
		lpVictim->GetSpellMgr().GetAffectedInfo().Disenchant(SpellType::MAGICAL_SPELL, SpellTarget::ENEMY_TARGET_ENCHANT, 
			Disenchant::NONE, 0, Disenchant::INFINITE_NUM);
	}

	return 0;
}


DEFINE_SKILL(WoundsMake)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);
	Position currPos=lpSkillUser->GetCurrentPos();
	 	if(((currPos.m_fPointX >= 1824)&&(currPos.m_fPointX <= 2250))&&((currPos.m_fPointZ >= 2817)&&(currPos.m_fPointZ <= 3189))){if(lpVictim->IsSafeZones(lpSkillUser , lpVictim)) {return 0;}}
		//return 0;

	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

	return InstanceMagicDamage(lpSkillUser, lpVictim, attackType, 10);//was 30
}


DEFINE_SKILL(Regeneration)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);
	 

	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

	unsigned short wEnchantLevel = CastEnchantLevel(lpSkillUser, lpVictim, attackType, 1);
	unsigned long dwDurationSec = static_cast<unsigned long>((50 + lpSkillUser->GetStatus().m_StatusInfo.m_wMagicPower / 4.0f) * 
		(attackType.m_cSkillLockCount + 1));

	unsigned short wResult = CAddSpell<CRegenerationSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
		SpellType::MAGICAL_SPELL, SpellID::Regeneration, wEnchantLevel, dwDurationSec))(lpVictim);

	switch (wResult)
	{
		case CSpell::NO_ENCHANT_ERROR :
		case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
			cDefenserJudge = ClientConstants::Judge_Enchant;
			break;

		case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
			wError = PktAtAck::FAIL_SPELL;
			break;

		case CSpell::ENCHANT_FAIL_BY_RESIST :
			cDefenserJudge = ClientConstants::Judge_Resist;
			break;
	}

	return 0;
}


//-------------------------------------------------------------------------------------------------
// 컴배턴트 스킬

DEFINE_SKILL(CrusherMastery)
{
	return 0;
}


DEFINE_SKILL(BladeMastery)
{
	return 0;
}


DEFINE_SKILL(SplitLife)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);


	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

	const unsigned short wPreviousHP = lpVictim->m_CreatureStatus.m_nNowHP;

	unsigned short wLockCount = GetSkillLockCount(lpSkillUser, attackType);
	unsigned short wHPCost = static_cast<unsigned short>((attackType.m_cSkillLevel + wLockCount * 6) * 50 *
		(100 + ((attackType.m_cSkillLevel + wLockCount * 6) * 25 * 0.3f)) / 100.0f);

	// HP가 부족하면 발동되지 않는다.
	if (lpSkillUser->m_CreatureStatus.m_nNowHP <= wHPCost)
	{
//		lpSkillUser->m_CreatureStatus.m_nNowHP = 0;
		cDefenserJudge = ClientConstants::Judge_Heal;
		return 0;
	}

	lpSkillUser->m_CreatureStatus.m_nNowHP -= wHPCost;
	
	const unsigned short wAdditionalHP = InstanceMeleeRegen(lpSkillUser, attackType, 30);
	if (lpVictim->m_CreatureStatus.m_nNowHP > USHRT_MAX - wAdditionalHP)
	{
		lpVictim->m_CreatureStatus.m_nNowHP = lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxHP;
	}
	else
	{
		lpVictim->m_CreatureStatus.m_nNowHP += wAdditionalHP;
		lpVictim->m_CreatureStatus.m_nNowHP = 
			std::min(lpVictim->m_CreatureStatus.m_nNowHP, lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxHP);
	}

	lpVictim->GetThreat().HealThreat(lpSkillUser, (lpVictim->m_CreatureStatus.m_nNowHP - wPreviousHP) / 2);

	cDefenserJudge = ClientConstants::Judge_Heal;
	return 0;
}


DEFINE_SKILL(ExtraLife)
{
	return 0;
}


//-------------------------------------------------------------------------------------------------
// 오피세이터 스킬

DEFINE_SKILL(Dispel)
{
	 
	 

	if (0 != lpVictim->GetSpellMgr().GetAffectedInfo().Disenchant(SpellType::MAGICAL_SPELL, SpellTarget::FRIEND_TARGET_ENCHANT, 
		Disenchant::NONE, 0, Disenchant::INFINITE_NUM))
	{
        lpVictim->GetThreat().AddToThreatList(lpSkillUser, 1);

		Creature::CreatureType eCreatureType = Creature::GetCreatureType(lpVictim->GetCID());
		if (Creature::CT_MONSTER == eCreatureType ||
			Creature::CT_SUMMON == eCreatureType ||
			Creature::CT_STRUCT == eCreatureType)
		{
			CMonster* lpMonster = reinterpret_cast<CMonster *>(lpVictim);
			lpMonster->Attacked();
		}
	}

	cDefenserJudge = ClientConstants::Judge_Disenchant;
	return 0;
}


DEFINE_SKILL(MagicMissile)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);
	Position currPos=lpSkillUser->GetCurrentPos();
	 	if(((currPos.m_fPointX >= 1824)&&(currPos.m_fPointX <= 2250))&&((currPos.m_fPointZ >= 2817)&&(currPos.m_fPointZ <= 3189))){if(lpVictim->IsSafeZones(lpSkillUser , lpVictim)) {return 0;}}
		//return 0;
	 

	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }
	return CastMagicDamage(lpSkillUser, lpVictim, attackType, 25);
}


DEFINE_SKILL(Flexibility)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	 
	 

	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

//	short count = GetSkillLockCount(lpSkillUser, attackType);
	const unsigned short wEnchantLevel = static_cast<unsigned short>(attackType.m_cSkillLevel + GetSkillLockCount(lpSkillUser, attackType) * 6 + (lpSkillUser->GetStatus().m_StatusInfo.m_wMagicPower / 100.0f));
	const unsigned long dwDurationSec = (attackType.m_cSkillLevel + GetSkillLockCount(lpSkillUser, attackType) * 6) * 15;

	unsigned short wResult = CAddSpell<CFlexibilitySpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
		SpellType::MAGICAL_SPELL, SpellID::Flexibility, wEnchantLevel, dwDurationSec))(lpVictim);

	switch (wResult)
	{
		case CSpell::NO_ENCHANT_ERROR :
		case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
			cDefenserJudge = ClientConstants::Judge_Enchant;
			break;

		case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
			wError = PktAtAck::FAIL_SPELL;
			break;

		case CSpell::ENCHANT_FAIL_BY_RESIST :
			cDefenserJudge = ClientConstants::Judge_Resist;
			break;
	}

	return 0;
}


DEFINE_SKILL(ClawMastery)
{
	return 0;
}


DEFINE_SKILL(LifeAura)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	if (true == CheckChantOff(attackType.m_cSkillLockCount, attackType.m_cSkillLevel))
	{
		lpSkillUser->GetSpellMgr().GetCastingInfo().ClearChant();

		cDefenserJudge = ClientConstants::Judge_Chant;
		return 0;
	}

	short nConsumeMPAmount = ConsumeMP(attackType, lpSkillUser, cOffencerJudge);
	if (-1 == nConsumeMPAmount) { return 0; }
	
	CSpell* pSpell = new CLifeAuraSpell(CSpell::Spell_Info(ProtoType, lpSkillUser, 
		SpellType::MAGICAL_SPELL, SpellID::LifeAura,
		ChantLevel(lpSkillUser, attackType, 1), CSpell::INFINITE_DURATION), nConsumeMPAmount);

	CParty* pParty = lpVictim->GetParty();
	if (NULL != pParty) 
	{
		pParty->GetPartySpellMgr().AddAffectedToAllMember(pSpell, lpSkillUser->GetMapIndex());
	} 
	else 
	{
		unsigned short wError = CSpell::NO_ENCHANT_ERROR;
		pSpell->AddAffected(lpVictim, wError);		
	}
	CGlobalSpellMgr::GetInstance().Add(pSpell);

	cDefenserJudge = ClientConstants::Judge_Chant;
	return 0;
}

DEFINE_SKILL(GuardAura)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	if (true == CheckChantOff(attackType.m_cSkillLockCount, attackType.m_cSkillLevel))
	{
		lpSkillUser->GetSpellMgr().GetCastingInfo().ClearChant();

		cDefenserJudge = ClientConstants::Judge_Chant;
		return 0;
	}

	short nConsumeMPAmount = ConsumeMP(attackType, lpSkillUser, cOffencerJudge);
	if (-1 == nConsumeMPAmount) { return 0; }
	
	CSpell* pSpell = new CGuardAuraSpell(CSpell::Spell_Info(ProtoType, lpSkillUser, 
		SpellType::MAGICAL_SPELL, SpellID::GuardAura,
		ChantLevel(lpSkillUser, attackType, 1), CSpell::INFINITE_DURATION), nConsumeMPAmount);

	CParty* pParty = lpVictim->GetParty();
	if (NULL != pParty) 
	{
		pParty->GetPartySpellMgr().AddAffectedToAllMember(pSpell, lpSkillUser->GetMapIndex());
	} 
	else 
	{
		unsigned short wError = CSpell::NO_ENCHANT_ERROR;
		pSpell->AddAffected(lpVictim, wError);		
	}
	CGlobalSpellMgr::GetInstance().Add(pSpell);

	cDefenserJudge = ClientConstants::Judge_Chant;
	return 0;
}

DEFINE_SKILL(GuardianAngel)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

		if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

		unsigned short wEnchantLevel = CastMeelsEnchantLevel(lpSkillUser, lpVictim, attackType, 1);
		//wEnchantLevel = static_cast<unsigned short>(wEnchantLevel / 2);//hz added this
		unsigned long dwDurationSec = static_cast<unsigned long>((100 + (GetSkillLockCount(lpSkillUser, attackType) * 100 * 0.75f) / 8.0f)
																* (attackType.m_cSkillLockCount + 1));
		
		unsigned short wResult = CAddSpell<CHardenSkinSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
			SpellType::MAGICAL_SPELL, SpellID::HardenSkin, wEnchantLevel, dwDurationSec))(lpVictim);
		
		switch (wResult)
		{
			case CSpell::NO_ENCHANT_ERROR :
			case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
				cDefenserJudge = ClientConstants::Judge_Enchant;
				break;

			case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
				wError = PktAtAck::FAIL_SPELL;
				break;

			case CSpell::ENCHANT_FAIL_BY_RESIST :
				cDefenserJudge = ClientConstants::Judge_Resist;
				break;
		}

		return 0;
}


DEFINE_SKILL(InnerSight)
{
	return 0;
}



//-------------------------------------------------------------------------------------------------
// 템플러 스킬

DEFINE_SKILL(Guard)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	 
	 

	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

	unsigned short wEnchantLevel = CastEnchantSkillLevel(lpSkillUser, lpVictim, attackType, 1);
	char BuffMasteryLevel=GetAbilityLevel(lpSkillUser,0x1061);
	float BuffMasteryRatio=GetAbilityRatio(BuffMasteryLevel,1.1,0.1);

	unsigned long dwDurationSec = static_cast<unsigned long>(((100 + (GetSkillLockCount(lpSkillUser, attackType) * 100 * 0.3f) / 8.0f) * 
		(attackType.m_cSkillLockCount + 1))*BuffMasteryRatio);


	unsigned short wResult = CAddSpell<CGuardSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
		SpellType::MAGICAL_SPELL, SpellID::Guard, wEnchantLevel, dwDurationSec, attackType.m_cSkillLockCount, attackType.m_cSkillLevel))(lpVictim);

	switch (wResult)
	{
		case CSpell::NO_ENCHANT_ERROR :
		case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
			cDefenserJudge = ClientConstants::Judge_Enchant;
			break;

		case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
			wError = PktAtAck::FAIL_SPELL;
			break;

		case CSpell::ENCHANT_FAIL_BY_RESIST :
			cDefenserJudge = ClientConstants::Judge_Resist;
			break;
	}

	return 0;
}


DEFINE_SKILL(HardenSkin)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

	unsigned short wEnchantLevel = CastMeelsEnchantLevel(lpSkillUser, lpVictim, attackType, 1);
	char BuffMasteryLevel=GetAbilityLevel(lpSkillUser,0x1061);
	float BuffMasteryRatio=GetAbilityRatio(BuffMasteryLevel,1.1,0.1);

	unsigned long dwDurationSec = static_cast<unsigned long>(((100 + (GetSkillLockCount(lpSkillUser, attackType) * 100 * 0.75f) / 8.0f)
															* (attackType.m_cSkillLockCount + 1))*BuffMasteryRatio);

	wEnchantLevel = static_cast<unsigned short>( wEnchantLevel / 2); //hz added this

	unsigned short wResult = CAddSpell<CHardenSkinSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
		SpellType::MAGICAL_SPELL, SpellID::HardenSkin, wEnchantLevel, dwDurationSec))(lpVictim);
	
	switch (wResult)
	{
		case CSpell::NO_ENCHANT_ERROR :
		case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
			cDefenserJudge = ClientConstants::Judge_Enchant;
			break;

		case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
			wError = PktAtAck::FAIL_SPELL;
			break;

		case CSpell::ENCHANT_FAIL_BY_RESIST :
			cDefenserJudge = ClientConstants::Judge_Resist;
			break;
	}

	return 0;
}


DEFINE_SKILL(FastHit)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);
	Position currPos=lpSkillUser->GetCurrentPos();
	 	if(((currPos.m_fPointX >= 1824)&&(currPos.m_fPointX <= 2250))&&((currPos.m_fPointZ >= 2817)&&(currPos.m_fPointZ <= 3189))){if(lpVictim->IsSafeZones(lpSkillUser , lpVictim)) {return 0;}}
		//return 0;
	 

	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

	if (false == SlowlySkillAttack(ProtoType, attackType, lpSkillUser, lpVictim))
	{
		wError = PktAtAck::FAIL_SPELL;
	}

	float fAddRate = (150 + 10 * ((attackType.m_cSkillLevel + GetSkillLockCount(lpSkillUser, attackType) * 6) - 
		(attackType.m_cSkillLockCount * 6))) / 100.0f;
	unsigned short wLockCount = GetSkillLockCount(lpSkillUser, attackType);
	// edith 2009.03.15 스킬공격시 패시브 스킬 포함되게 수정
//	unsigned short wDamage = lpSkillUser->MeleeAttack(lpVictim, Creature::RIGHT_NON_PASSIVE_TYPE, 
	unsigned short wDamage = lpSkillUser->MeleeAttack(lpVictim, Creature::RIGHT_PASSIVE_TYPE, 
		LevelFactor(lpSkillUser, lpVictim), cDefenserJudge);

	long lDamage = static_cast<long>(wDamage * fAddRate);
	wDamage = static_cast<unsigned short>(min(lDamage, USHRT_MAX));

	lpVictim->GetThreat().AffectThreat(lpSkillUser, wDamage, CThreat::TAUNT);

	return wDamage;
}



DEFINE_SKILL(ManaConvert)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	 
	 

	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

	const unsigned short wPreviousHP = lpVictim->m_CreatureStatus.m_nNowHP;
	const unsigned short wAdditionalHP = InstanceMeleeRegen(lpSkillUser, attackType, 25);

	if (lpVictim->m_CreatureStatus.m_nNowHP > USHRT_MAX - wAdditionalHP)
	{
		lpVictim->m_CreatureStatus.m_nNowHP = lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxHP;
	}
	else
	{
        lpVictim->m_CreatureStatus.m_nNowHP += wAdditionalHP;
		lpVictim->m_CreatureStatus.m_nNowHP = 
			std::min(lpVictim->m_CreatureStatus.m_nNowHP, lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxHP);
	}

	lpVictim->GetThreat().HealThreat(lpSkillUser, (lpVictim->m_CreatureStatus.m_nNowHP - wPreviousHP) / 2);

	cDefenserJudge = ClientConstants::Judge_Heal;
	return 0;
}



//-------------------------------------------------------------------------------------------------
// 어태커 스킬 //

DEFINE_SKILL(PowerDrain)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

	if (false == SlowlySkillAttack(ProtoType, attackType, lpSkillUser, lpVictim))
	{
		wError = PktAtAck::FAIL_SPELL;
	}

	unsigned short wLockCount = GetSkillLockCount(lpSkillUser, attackType);
//	float fAddRate = (150 + 10 * (attackType.m_cSkillLevel + wLockCount * 6)) / 100.0f;
	float fAddRate = (135 + 10 * (attackType.m_cSkillLevel + wLockCount * 6)) / 100.0f;

	unsigned short wDamage = lpSkillUser->MeleeAttack(lpVictim, Creature::LEFT_NON_PASSIVE_TYPE, 
		LevelFactor(lpSkillUser, lpVictim), cDefenserJudge);
	long lDamage = static_cast<long>(wDamage * fAddRate);
	wDamage = static_cast<unsigned short>(min(lDamage, USHRT_MAX));

	lpVictim->GetThreat().AffectThreat(lpSkillUser, wDamage, CThreat::DETAUNT);

	if(4 <= attackType.m_cSkillLockCount)
	{
		//const float fHPRegenRate = static_cast<float>((120 + (attackType.m_cSkillLevel * 5)) / 100.0f);
		//const float fMPRegenRate = static_cast<float>((10 + (attackType.m_cSkillLevel * 15)) / 100.0f);
		const float fHPRegenRate = static_cast<float>((120 + (attackType.m_cSkillLevel * 5)) / 100.0f);
		const float fMPRegenRate = static_cast<float>((10 + (attackType.m_cSkillLevel * 15)) / 100.0f);
		long lAdditionalHP = static_cast<long>(wDamage * fHPRegenRate);;
		const unsigned short wAdditionalHP = static_cast<unsigned short>(min(lAdditionalHP, USHRT_MAX));

		long lAdditionalMP = static_cast<long>(wDamage * fMPRegenRate);;
		const unsigned short wAdditionalMP = static_cast<unsigned short>(min(lAdditionalMP, USHRT_MAX));

		if (lpSkillUser->m_CreatureStatus.m_nNowHP > USHRT_MAX - wAdditionalHP)
		{
			lpSkillUser->m_CreatureStatus.m_nNowHP = lpSkillUser->m_CreatureStatus.m_StatusInfo.m_nMaxHP;
		}
		else
		{
			lpSkillUser->m_CreatureStatus.m_nNowHP += wAdditionalHP;
			lpSkillUser->m_CreatureStatus.m_nNowHP = 
				std::min(lpSkillUser->m_CreatureStatus.m_nNowHP, lpSkillUser->m_CreatureStatus.m_StatusInfo.m_nMaxHP);
		}

		if (lpVictim->m_CreatureStatus.m_nNowMP < wAdditionalMP)
		{
			lpVictim->m_CreatureStatus.m_nNowMP = 0;
		}
		else
		{
			lpVictim->m_CreatureStatus.m_nNowMP -= wAdditionalMP;
			lpVictim->m_CreatureStatus.m_nNowMP = 
				std::max(lpVictim->m_CreatureStatus.m_nNowMP, static_cast<unsigned short>(0));
		}

		const unsigned short wOffencerOldMP = lpSkillUser->m_CreatureStatus.m_nNowMP;
		if (lpSkillUser->m_CreatureStatus.m_nNowMP > USHRT_MAX - wAdditionalMP)
		{
			lpSkillUser->m_CreatureStatus.m_nNowMP = lpSkillUser->m_CreatureStatus.m_StatusInfo.m_nMaxMP;
		}
		else
		{
			lpSkillUser->m_CreatureStatus.m_nNowMP += wAdditionalMP;
			lpSkillUser->m_CreatureStatus.m_nNowMP = 
				std::min(lpSkillUser->m_CreatureStatus.m_nNowMP, lpSkillUser->m_CreatureStatus.m_StatusInfo.m_nMaxMP);
		}

		wOffencerMPHeal = (lpSkillUser->m_CreatureStatus.m_nNowMP > wOffencerOldMP) ? lpSkillUser->m_CreatureStatus.m_nNowMP - wOffencerOldMP : 0;
	}
	else
	{
		const float fRegenRate = 5 * (attackType.m_cSkillLevel + wLockCount * 6) / 100.0f; // was 5 *

		long lAdditionalHP = static_cast<long>(wDamage * fRegenRate);
		const unsigned short wAdditionalHP = static_cast<unsigned short>(min(lAdditionalHP, USHRT_MAX));

		if (lpSkillUser->m_CreatureStatus.m_nNowHP > USHRT_MAX - wAdditionalHP)
		{
			lpSkillUser->m_CreatureStatus.m_nNowHP = lpSkillUser->m_CreatureStatus.m_StatusInfo.m_nMaxHP;
		}
		else
		{
			lpSkillUser->m_CreatureStatus.m_nNowHP += wAdditionalHP;
			lpSkillUser->m_CreatureStatus.m_nNowHP = 
				std::min(lpSkillUser->m_CreatureStatus.m_nNowHP, lpSkillUser->m_CreatureStatus.m_StatusInfo.m_nMaxHP);
		}
	}

	cOffencerJudge = ClientConstants::Judge_Heal;

	return wDamage;
}


DEFINE_SKILL(SkillArmFight)
{
	return 0;
}


DEFINE_SKILL(RingGeyser)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);
	Position currPos=lpSkillUser->GetCurrentPos();
	 	if(((currPos.m_fPointX >= 1824)&&(currPos.m_fPointX <= 2250))&&((currPos.m_fPointZ >= 2817)&&(currPos.m_fPointZ <= 3189))){if(lpVictim->IsSafeZones(lpSkillUser , lpVictim)) {return 0;}}
		//return 0;
	 

	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

	if (false == SlowlySkillAttack(ProtoType, attackType, lpSkillUser, lpVictim))
	{
		wError = PktAtAck::FAIL_SPELL;
	}

	unsigned short wLockCount = GetSkillLockCount(lpSkillUser, attackType);
	float fAddRate = (200 + 10 * (attackType.m_cSkillLevel + wLockCount * 6)) / 100.0f;

	// edith 2009.03.15 스킬공격시 패시브 스킬 포함되게 수정
//	unsigned short wDamage = lpSkillUser->MeleeAttack(lpVictim, Creature::LEFT_NON_PASSIVE_TYPE, 
	unsigned short wDamage = lpSkillUser->MeleeAttack(lpVictim, Creature::LEFT_PASSIVE_TYPE, 
		LevelFactor(lpSkillUser, lpVictim), cDefenserJudge);
	long lDamage = static_cast<long>(wDamage * fAddRate);
	wDamage = static_cast<unsigned short>(min(lDamage, USHRT_MAX));

	lpVictim->GetThreat().AffectThreat(lpSkillUser, wDamage, CThreat::DETAUNT);
	return wDamage;
}



//-------------------------------------------------------------------------------------------------
// 거너 스킬

DEFINE_SKILL(FireRing)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);
	Position currPos=lpSkillUser->GetCurrentPos();
	 	if(((currPos.m_fPointX >= 1824)&&(currPos.m_fPointX <= 2250))&&((currPos.m_fPointZ >= 2817)&&(currPos.m_fPointZ <= 3189))){if(lpVictim->IsSafeZones(lpSkillUser , lpVictim)) {return 0;}}
		//return 0;
	 

	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

	// Rodin : 센트리포지션 효과 추가
	// Rodin : 탄약 소모

	unsigned short wLockCount = GetSkillLockCount(lpSkillUser, attackType);

	FightStatus resultStatus;
	unsigned short aryEnchantLevel[Skill::SpellID::MAX_SPELL_TYPE] = { 0, };
	lpSkillUser->GetEnchantLevel(aryEnchantLevel);

	// edith 2009.03.15 스킬공격시 패시브 스킬 포함되게 수정
//	lpSkillUser->GetEtcTypeStatus(Creature::LEFT_NON_PASSIVE_TYPE).CalculateEnchantInfo(aryEnchantLevel, resultStatus);
	lpSkillUser->GetEtcTypeStatus(Creature::LEFT_PASSIVE_TYPE).CalculateEnchantInfo(aryEnchantLevel, resultStatus);
		
	// edith 2009.06.13 파이어링 밸런스 수정
/*
	resultStatus.m_lMinDamage += static_cast<long>((attackType.m_cSkillLevel + wLockCount * 6) * 
		(1 + lpSkillUser->GetStatus().m_nLevel * 0.02f));
	resultStatus.m_lMaxDamage += static_cast<long>((attackType.m_cSkillLevel + wLockCount * 6) * 
		(1 + lpSkillUser->GetStatus().m_nLevel * 0.02f));
	resultStatus.m_wHitRate += static_cast<unsigned short>((attackType.m_cSkillLevel + wLockCount * 6) * 
		(1 + lpSkillUser->GetStatus().m_nLevel * 0.02f));
*/
	resultStatus.m_lMinDamage += static_cast<long>((attackType.m_cSkillLevel * 2 + wLockCount * 12) * 
		(1 + lpSkillUser->GetStatus().m_nLevel * 0.02f));
	resultStatus.m_lMaxDamage += static_cast<long>((attackType.m_cSkillLevel * 2 + wLockCount * 12) * 
		(1 + lpSkillUser->GetStatus().m_nLevel * 0.02f));
	resultStatus.m_wHitRate += static_cast<unsigned short>((attackType.m_cSkillLevel * 2 + wLockCount * 12) * 
		(1 + lpSkillUser->GetStatus().m_nLevel * 0.02f));

	Creature::CreatureType eCreatureType = Creature::GetCreatureType(lpVictim->GetCID());

	short wHitRate = lpSkillUser->m_CreatureStatus.m_StatusInfo.m_wHitRate;

	short wMissRate;
	if(4 <= attackType.m_cSkillLockCount)
	{
		resultStatus.m_wCritical += static_cast<unsigned short>(10 + (attackType.m_cSkillLevel * 15));

		wMissRate =  wHitRate + lpVictim->m_CreatureStatus.m_StatusInfo.m_wEvade - 
					 static_cast<unsigned short>(lpVictim->m_CreatureStatus.m_StatusInfo.m_wEvade * (10 + (attackType.m_cSkillLevel * 15)) / 100.0f);
	}
	else
	{
		wMissRate =  wHitRate + lpVictim->m_CreatureStatus.m_StatusInfo.m_wEvade;
	}


	// edith 2008.05.20 블록판정 추가
	short wBlockRate = lpVictim->m_CreatureStatus.m_StatusInfo.m_wBlock;
	short wEvdRate = wMissRate-wHitRate;

	wMissRate = wEvdRate+wHitRate+wBlockRate;

	unsigned long dwRate = Math::Random::ComplexRandom(wMissRate);

	// 블록판정
	if (dwRate >= static_cast<unsigned short>(wEvdRate+wHitRate))
	{
		lpVictim->GetThreat().AddToThreatList(lpSkillUser, 1);		

		if (Creature::CT_MONSTER == eCreatureType ||
			Creature::CT_SUMMON == eCreatureType ||
			Creature::CT_STRUCT == eCreatureType)
		{
			CMonster* lpMonster = reinterpret_cast<CMonster *>(lpVictim);
			lpMonster->Attacked();
		}

		cDefenserJudge = ClientConstants::Judge_Guard;
		return 0;
	}
	// 회피판정
	if (dwRate >= static_cast<unsigned short>(wHitRate))
	{
		lpVictim->GetThreat().AddToThreatList(lpSkillUser, 1);		
		
		if (Creature::CT_MONSTER == eCreatureType ||
			Creature::CT_SUMMON == eCreatureType ||
			Creature::CT_STRUCT == eCreatureType)
		{
			CMonster* lpMonster = reinterpret_cast<CMonster *>(lpVictim);
			lpMonster->Attacked();
		}

		cDefenserJudge = ClientConstants::Judge_Miss;
		return 0;
	}
/*
	// D. 명중 판정
	if (Math::Random::ComplexRandom(wMissRate) >= static_cast<unsigned short>(wHitRate))
	{
		lpVictim->GetThreat().AddToThreatList(lpSkillUser, 1);		

		if (Creature::CT_MONSTER == eCreatureType ||
			Creature::CT_SUMMON == eCreatureType ||
			Creature::CT_STRUCT == eCreatureType)
		{
			CMonster* lpMonster = reinterpret_cast<CMonster *>(lpVictim);
			lpMonster->Attacked();
		}

		cDefenserJudge = ClientConstants::Judge_Miss;
		return 0;
	}

	// E. 블록 판정
	if (Math::Random::ComplexRandom(10000) < lpVictim->m_CreatureStatus.m_StatusInfo.m_fBlockRate * 100)
	{
		lpVictim->GetThreat().AddToThreatList(lpSkillUser, 1);		

		if (Creature::CT_MONSTER == eCreatureType ||
			Creature::CT_SUMMON == eCreatureType ||
			Creature::CT_STRUCT == eCreatureType)
		{
			CMonster* lpMonster = reinterpret_cast<CMonster *>(lpVictim);
			lpMonster->Attacked();
		}

		cDefenserJudge = ClientConstants::Judge_Guard;
		return 0;
	}
*/
	const unsigned short wDamage = lpVictim->CalculateDamage(resultStatus, 
		LevelFactor(lpSkillUser, lpVictim), cDefenserJudge);
	lpSkillUser->CalculateEquipDurability(AtType::LEFT_MELEE);

	return wDamage;
}


DEFINE_SKILL(Blast)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);
	Position currPos=lpSkillUser->GetCurrentPos();
		if(((currPos.m_fPointX >= 1824)&&(currPos.m_fPointX <= 2250))&&((currPos.m_fPointZ >= 2817)&&(currPos.m_fPointZ <= 3189))){if(lpVictim->IsSafeZones(lpSkillUser , lpVictim)) {return 0;}}
		//return 0;

	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

	float fAddRate = (true == lpSkillUser->GetEnchantInfo().GetFlag(SpellID::Stealth)) ? 
	//	Gunner code
	//	(300 + 50 * (attackType.m_cSkillLevel + attackType.m_cSkillLockCount * 6)) / 100.0f : 
	//	(150 + 12 * (attackType.m_cSkillLevel + attackType.m_cSkillLockCount * 6)) / 100.0f;
	//  Archer Code
		(300 + 75 * (attackType.m_cSkillLevel + attackType.m_cSkillLockCount * 6)) / 100.0f : //was 90
		(150 + 20 * (attackType.m_cSkillLevel + attackType.m_cSkillLockCount * 6)) / 100.0f;

	// Rodin : 탄약 소모

	unsigned int ExType = 0;

	Creature::CreatureType eCreatureType = Creature::GetCreatureType(lpSkillUser->GetCID()); 
	Creature::CreatureType eDefenderCreatureType = Creature::GetCreatureType(lpVictim->GetCID());

	if (Creature::CT_MONSTER == eCreatureType ||
		Creature::CT_SUMMON == eCreatureType ||
		Creature::CT_STRUCT == eCreatureType ||
		Creature::CT_MONSTER == eDefenderCreatureType ||
		Creature::CT_SUMMON == eDefenderCreatureType ||
		Creature::CT_STRUCT == eDefenderCreatureType)
	{
		ExType = 0;
	}
	else
	{
		// edith 2010.01.03 1/5로 크리티컬 데미지 영향을 준다
		//ExType = Math::Random::ComplexRandom(5);
		ExType = 0;
	}

	// edith 2009.03.15 스킬공격시 패시브 스킬 포함되게 수정
//	unsigned short wDamage = lpSkillUser->MeleeAttack(lpVictim, Creature::LEFT_NON_PASSIVE_TYPE, 
	unsigned short wDamage = lpSkillUser->MeleeAttack(lpVictim, Creature::LEFT_PASSIVE_TYPE, 
		LevelFactor(lpSkillUser, lpVictim), cDefenserJudge, ExType);

	long lDamage = static_cast<long>(wDamage * fAddRate);
	wDamage = static_cast<unsigned short>(min(lDamage, USHRT_MAX));

	char abilityLevel=GetAbilityLevel(lpSkillUser,0x1056);
	if(abilityLevel)
	{
		if (SuccessCripplingRate(lpSkillUser,abilityLevel) > Math::Random::ComplexRandom(100))
		{
			unsigned long duration=abilityLevel*2;//how much time the slow is going to hold.
		
			unsigned short wResult = CAddSpell<CSlowSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, SpellType::PHYSICAL_SPELL, SpellID::Slow, 
			lpSkillUser->GetStatus().m_nLevel, duration))(lpVictim);

			switch (wResult)
			{
				case CSpell::NO_ENCHANT_ERROR :
				case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
					cDefenserJudge = ClientConstants::Judge_Enchant;
					break;

				case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
					wError = PktAtAck::FAIL_SPELL;
					return 0;

				case CSpell::ENCHANT_FAIL_BY_RESIST :
					cDefenserJudge = ClientConstants::Judge_Resist;
					break;
			}

			lpVictim->GetThreat().AddToThreatList(lpSkillUser, 1);
		}
	}

	lpSkillUser->CalculateEquipDurability(AtType::LEFT_MELEE);
	return wDamage;
}



//-------------------------------------------------------------------------------------------------
// 룬오프 스킬

DEFINE_SKILL(Rot)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);
	Position currPos=lpSkillUser->GetCurrentPos();
	 	if(((currPos.m_fPointX >= 1824)&&(currPos.m_fPointX <= 2250))&&((currPos.m_fPointZ >= 2817)&&(currPos.m_fPointZ <= 3189))){if(lpVictim->IsSafeZones(lpSkillUser , lpVictim)) {return 0;}}
		//return 0;
	 

	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

	char successRate=GetStickyMagicSuccessRate(lpSkillUser,lpVictim,0.5f);

	char PoisonBreakerLevel=GetAbilityLevel(lpVictim,0x105E);
	float PoisonBreakerRatio=GetAbilityRatio(PoisonBreakerLevel,1.10,0.10);

	float finalSuccessRate=successRate;
	finalSuccessRate*=PoisonBreakerRatio;
	successRate=static_cast<unsigned char>(finalSuccessRate);

	if (successRate > Math::Random::ComplexRandom(100))
//	if (70 > Math::Random::ComplexRandom(100))
	{
//		int iRate = (lpSkillUser->GetStatus().m_StatusInfo.m_wMagicPower + lpVictim->GetStatus().m_StatusInfo.m_fMagicResistRate);
//		if ((unsigned long)lpSkillUser->GetStatus().m_StatusInfo.m_wMagicPower > Math::Random::ComplexRandom(iRate))
//		{
		unsigned short wEnchantLevel = CastEnchantLevel(lpSkillUser, lpVictim, attackType, 1);
		unsigned long dwDurationSec = static_cast<unsigned long>(20 * LevelFactor(lpSkillUser, lpVictim));

		unsigned short wResult = CAddSpell<CPoisonedSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
			SpellType::MAGICAL_SPELL, SpellID::Poisoned, wEnchantLevel, dwDurationSec))(lpVictim);

		switch (wResult)
		{
			case CSpell::NO_ENCHANT_ERROR :
			case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
				break;

			case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
				wError = PktAtAck::FAIL_SPELL;
				break;

			case CSpell::ENCHANT_FAIL_BY_RESIST :
				cDefenserJudge = ClientConstants::Judge_Resist;
				break;
		}
//		}
//		else
//		{
//			cDefenserJudge = ClientConstants::Judge_Resist;
//		}
	}
	else
	{
		cDefenserJudge = ClientConstants::Judge_Resist;
	}

	return CastMagicDamage(lpSkillUser, lpVictim, attackType, 25);/*(attackType.m_cSkillLockCount < 3) ? 15 : 20);*/
}


DEFINE_SKILL(Shock)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);
	Position currPos=lpSkillUser->GetCurrentPos();
	 	if(((currPos.m_fPointX >= 1824)&&(currPos.m_fPointX <= 2250))&&((currPos.m_fPointZ >= 2817)&&(currPos.m_fPointZ <= 3189))){if(lpVictim->IsSafeZones(lpSkillUser , lpVictim)) {return 0;}}
		//return 0;
	 

	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }
	if(4 <= attackType.m_cSkillLockCount)
	{
		unsigned char cSuccessRate = static_cast<unsigned char>(10 + (attackType.m_cSkillLevel * 10));
		if (cSuccessRate > Math::Random::ComplexRandom(100))
		{
			unsigned short wEnchantLevel = CastEnchantLevel(lpSkillUser, lpVictim, attackType, 1);
			unsigned long dwDurationSec = CastEnchantTick(lpSkillUser, lpVictim, attackType, 0.5f);
			//no stun:
			/*unsigned short wResult = CAddSpell<CStunSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
				SpellType::MAGICAL_SPELL, SpellID::Stun, wEnchantLevel, dwDurationSec))(lpVictim);

			switch (wResult)
			{
			case CSpell::NO_ENCHANT_ERROR :
			case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
				break;

			case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
				wError = PktAtAck::FAIL_SPELL;
				break;

			case CSpell::ENCHANT_FAIL_BY_RESIST :
				cDefenserJudge = ClientConstants::Judge_Resist;
				break;
			}*/
		}
		else
		{
			cDefenserJudge = ClientConstants::Judge_Resist;
		}
	}
	return CastMagicDamage(lpSkillUser, lpVictim, attackType, (attackType.m_cSkillLockCount < 3) ? 20 : 25);
}


DEFINE_SKILL(Shackle)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);
	Position currPos=lpSkillUser->GetCurrentPos();
	 
	 	if(((currPos.m_fPointX >= 1824)&&(currPos.m_fPointX <= 2250))&&((currPos.m_fPointZ >= 2817)&&(currPos.m_fPointZ <= 3189))){if(lpVictim->IsSafeZones(lpSkillUser , lpVictim)) {return 0;}}
		//return 0;

	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

	char successRate=GetStickyMagicSuccessRate(lpSkillUser,lpVictim,0.5f);

	if (successRate > Math::Random::ComplexRandom(100))
	{
		unsigned short wEnchantLevel = attackType.m_cSkillLevel + GetSkillLockCount(lpSkillUser, attackType) * 6;
		unsigned long dwDurationSec = CastEnchantTick(lpSkillUser, lpVictim, attackType, 2);

		unsigned short wResult = CAddSpell<CHoldSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
			SpellType::MAGICAL_SPELL, SpellID::Hold, wEnchantLevel, dwDurationSec))(lpVictim);

		switch (wResult)
		{
			case CSpell::NO_ENCHANT_ERROR :
			case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
				break;

			case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
				wError = PktAtAck::FAIL_SPELL;
				break;

			case CSpell::ENCHANT_FAIL_BY_RESIST :
				cDefenserJudge = ClientConstants::Judge_Resist;
				break;
		}
	}
	else
	{
		cDefenserJudge = ClientConstants::Judge_Resist;
	}

	return CastMagicDamage(lpSkillUser, lpVictim, attackType, (attackType.m_cSkillLockCount < 3) ? 15 : 20);
}


DEFINE_SKILL(Crevice)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);
	Position currPos=lpSkillUser->GetCurrentPos();
	 	if(((currPos.m_fPointX >= 1824)&&(currPos.m_fPointX <= 2250))&&((currPos.m_fPointZ >= 2817)&&(currPos.m_fPointZ <= 3189))){if(lpVictim->IsSafeZones(lpSkillUser , lpVictim)) {return 0;}}
		//return 0;
	 

	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }
	return InstanceMagicDamage(lpSkillUser, lpVictim, attackType, 20);//was 40
}

DEFINE_SKILL(Clairvoyance)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	if (true == CheckChantOff(attackType.m_cSkillLockCount, attackType.m_cSkillLevel))
	{
		lpSkillUser->GetSpellMgr().GetCastingInfo().ClearChant();

		cDefenserJudge = ClientConstants::Judge_Chant;
		return 0;
	}

	short nConsumeMPAmount = ConsumeMP(attackType, lpSkillUser, cOffencerJudge);
	if (-1 == nConsumeMPAmount) { return 0; }
	
	CSpell* pSpell = new CClairvoyanceChantSpell(CSpell::Spell_Info(ProtoType, lpSkillUser, 
		SpellType::MAGICAL_SPELL, SpellID::Clairvoyance,
		ChantLevel(lpSkillUser, attackType, 1), CSpell::INFINITE_DURATION), nConsumeMPAmount);

	CParty* pParty = lpVictim->GetParty();
	if (NULL != pParty) 
	{
		pParty->GetPartySpellMgr().AddAffectedToAllMember(pSpell, lpSkillUser->GetMapIndex());
	} 
	else 
	{
		unsigned short wError = CSpell::NO_ENCHANT_ERROR;
		pSpell->AddAffected(lpVictim, wError);		
	}
	CGlobalSpellMgr::GetInstance().Add(pSpell);

	cDefenserJudge = ClientConstants::Judge_Chant;
	return 0;
}


//-------------------------------------------------------------------------------------------------
// 라이프오프 스킬

DEFINE_SKILL(Flash)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);
	Position currPos=lpSkillUser->GetCurrentPos();
	 	if(((currPos.m_fPointX >= 1824)&&(currPos.m_fPointX <= 2250))&&((currPos.m_fPointZ >= 2817)&&(currPos.m_fPointZ <= 3189))){if(lpVictim->IsSafeZones(lpSkillUser , lpVictim)) {return 0;}}
		//return 0;
	 

	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }


	if(4 <= attackType.m_cSkillLockCount)
	{
		char successRate=GetStickyMagicSuccessRate(lpSkillUser,lpVictim,1);

		if (successRate > Math::Random::ComplexRandom(100))
		{
			unsigned short wEnchantLevel = 70 + (attackType.m_cSkillLevel * 30);
			unsigned long dwDurationSec = static_cast<unsigned long>(20 * LevelFactor(lpSkillUser, lpVictim));

			unsigned short wResult = CAddSpell<CCurseOfBlindSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
				SpellType::MAGICAL_SPELL, SpellID::CurseOfBlind, wEnchantLevel, dwDurationSec))(lpVictim);

			switch (wResult)
			{
			case CSpell::NO_ENCHANT_ERROR :
			case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
				{
					cDefenserJudge = ClientConstants::Judge_Enchant;

					lpVictim->GetThreat().AddToThreatList(lpSkillUser, 1);

					Creature::CreatureType eCreatureType = Creature::GetCreatureType(lpVictim->GetCID());
					if (Creature::CT_MONSTER == eCreatureType ||
						Creature::CT_SUMMON == eCreatureType ||
						Creature::CT_STRUCT == eCreatureType)
					{
						CMonster* lpMonster = reinterpret_cast<CMonster *>(lpVictim);
						lpMonster->Attacked();
					}
				}
				break;

			case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
				wError = PktAtAck::FAIL_SPELL;
				break;

			case CSpell::ENCHANT_FAIL_BY_RESIST :
				cDefenserJudge = ClientConstants::Judge_Resist;
				break;
			}
		}
		else
		{
			cDefenserJudge = ClientConstants::Judge_Resist;
		}
	}

	unsigned char successRate=GetStunMasterySuccessRate(lpSkillUser,lpVictim,0.5f);

		char StunBreakerLevel=GetAbilityLevel(lpVictim,0x105F);
		float StunBreakerRatio=GetAbilityRatio(StunBreakerLevel,1.10,0.10);
		float finalSuccessRate=successRate;
		finalSuccessRate*=StunBreakerRatio;
		successRate=static_cast<unsigned char>(finalSuccessRate);

	if (successRate > Math::Random::ComplexRandom(100))
	{
		unsigned short wEnchantLevel = attackType.m_cSkillLevel + GetSkillLockCount(lpSkillUser, attackType) * 6;
		unsigned long dwDurationSec = InstanceEnemyEnchantTick(lpSkillUser, lpVictim, attackType, 0.5f);
		dwDurationSec=static_cast<unsigned long>(dwDurationSec*0.7f);
		unsigned short wResult = CAddSpell<CStunSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
			SpellType::MAGICAL_SPELL, SpellID::Stun, wEnchantLevel, dwDurationSec))(lpVictim);

		switch (wResult)
		{
			case CSpell::NO_ENCHANT_ERROR :
			case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
				break;

			case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
				wError = PktAtAck::FAIL_SPELL;
				break;

			case CSpell::ENCHANT_FAIL_BY_RESIST :
				cDefenserJudge = ClientConstants::Judge_Resist;
				break;
		}
	}
	else
	{
		cDefenserJudge = ClientConstants::Judge_Resist;
	}

	return InstanceMagicDamage(lpSkillUser, lpVictim, attackType, 5);//was 15
}



//-------------------------------------------------------------------------------------------------
// 쉐도우오프 스킬

DEFINE_SKILL(WoundsBlast)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim); 
	Position currPos=lpSkillUser->GetCurrentPos();
		if(((currPos.m_fPointX >= 1824)&&(currPos.m_fPointX <= 2250))&&((currPos.m_fPointZ >= 2817)&&(currPos.m_fPointZ <= 3189))){if(lpVictim->IsSafeZones(lpSkillUser , lpVictim)) {return 0;}}
		//return 0;

	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

	unsigned short wLockCount = GetSkillLockCount(lpSkillUser, attackType);
	float fAddRate = (true == lpSkillUser->GetEnchantInfo().GetFlag(SpellID::Stealth)) ? 
		(300 + 55 * (attackType.m_cSkillLevel + wLockCount * 6)) / 100.0f : 
		(150 + 15 * (attackType.m_cSkillLevel + wLockCount * 6)) / 100.0f;

	// edith 2009.03.15 스킬공격시 패시브 스킬 포함되게 수정
//	unsigned short wDamage = lpSkillUser->MeleeAttack(lpVictim, Creature::LEFT_NON_PASSIVE_TYPE, 
	unsigned short wDamage = lpSkillUser->MeleeAttack(lpVictim, Creature::LEFT_PASSIVE_TYPE, 
		LevelFactor(lpSkillUser, lpVictim), cDefenserJudge);
	long lDamage = static_cast<long>(wDamage * fAddRate);
	wDamage = static_cast<unsigned short>(min(lDamage, USHRT_MAX));

	char abilityLevel=GetAbilityLevel(lpSkillUser,0x1056);
	if(abilityLevel)
	{
		if (SuccessCripplingRate(lpSkillUser,abilityLevel) > Math::Random::ComplexRandom(100))
		{
			unsigned long duration=abilityLevel*2;//how much time the slow is going to hold.
		
			unsigned short wResult = CAddSpell<CSlowSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, SpellType::PHYSICAL_SPELL, SpellID::Slow, 
			lpSkillUser->GetStatus().m_nLevel, duration))(lpVictim);

			switch (wResult)
			{
				case CSpell::NO_ENCHANT_ERROR :
				case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
					cDefenserJudge = ClientConstants::Judge_Enchant;
					break;

				case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
					wError = PktAtAck::FAIL_SPELL;
					return 0;

				case CSpell::ENCHANT_FAIL_BY_RESIST :
					cDefenserJudge = ClientConstants::Judge_Resist;
					break;
			}

			lpVictim->GetThreat().AddToThreatList(lpSkillUser, 1);
		}
	}

	lpVictim->GetThreat().AffectThreat(lpSkillUser, wDamage, CThreat::DETAUNT);
	return wDamage;
}


DEFINE_SKILL(DaggerFire)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);
	Position currPos=lpSkillUser->GetCurrentPos();
	 	if(((currPos.m_fPointX >= 1824)&&(currPos.m_fPointX <= 2250))&&((currPos.m_fPointZ >= 2817)&&(currPos.m_fPointZ <= 3189))){if(lpVictim->IsSafeZones(lpSkillUser , lpVictim)) {return 0;}}
		//return 0;
	 

	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

	unsigned short wLockCount = GetSkillLockCount(lpSkillUser, attackType);

	if (SuccessHitRate(lpSkillUser, lpVictim, 1) > Math::Random::ComplexRandom(100))
	{
		unsigned short wEnchantLevel = attackType.m_cSkillLevel + wLockCount * 6;
		unsigned long dwDurationSec = CastEnchantTick(lpSkillUser, lpVictim, attackType, 1);

		unsigned short wResult = CAddSpell<CHoldSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
			SpellType::MAGICAL_SPELL, SpellID::Hold, wEnchantLevel, dwDurationSec))(lpVictim);

		switch (wResult)
		{
			case CSpell::NO_ENCHANT_ERROR :
			case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
				{
					lpVictim->GetThreat().AddToThreatList(lpSkillUser, 1);

					Creature::CreatureType eCreatureType = Creature::GetCreatureType(lpVictim->GetCID());
					if (Creature::CT_MONSTER == eCreatureType ||
						Creature::CT_SUMMON == eCreatureType ||
						Creature::CT_STRUCT == eCreatureType)
					{
						CMonster* lpMonster = reinterpret_cast<CMonster *>(lpVictim);
						lpMonster->Attacked();
					}

					cDefenserJudge = ClientConstants::Judge_Enchant;
				}
				break;

			case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
				wError = PktAtAck::FAIL_SPELL;
				break;

			case CSpell::ENCHANT_FAIL_BY_RESIST :
				cDefenserJudge = ClientConstants::Judge_Resist;
				break;
		}
	}
	else
		cDefenserJudge = ClientConstants::Judge_Resist;

	long lDamage = static_cast<long>((attackType.m_cSkillLevel + wLockCount * 6) * 30 * 
		(1 + lpSkillUser->GetStatus().m_nLevel * 0.05f) * 
		(100 - lpVictim->GetStatus().m_StatusInfo.m_fDefenceRate) / 100.0f *
		LevelFactor(lpSkillUser, lpVictim) * (115 + Math::Random::ComplexRandom(10)) / 100.0f);

	return static_cast<unsigned short>(min(lDamage, USHRT_MAX));
}


DEFINE_SKILL(Envenom)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);
	Position currPos=lpSkillUser->GetCurrentPos();
	 	if(((currPos.m_fPointX >= 1824)&&(currPos.m_fPointX <= 2250))&&((currPos.m_fPointZ >= 2817)&&(currPos.m_fPointZ <= 3189))){if(lpVictim->IsSafeZones(lpSkillUser , lpVictim)) {return 0;}}
		//return 0;
	 

	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

	const unsigned short wEnchantLevel = attackType.m_cSkillLevel + GetSkillLockCount(lpSkillUser, attackType) * 6;
	const unsigned long dwDurationSec = (attackType.m_cSkillLevel + GetSkillLockCount(lpSkillUser, attackType) * 6) * 2;

	unsigned short wResult = CAddSpell<CEnvenomSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
		SpellType::MAGICAL_SPELL, SpellID::Envenom, wEnchantLevel, dwDurationSec))(lpVictim);

	switch (wResult)
	{
		case CSpell::NO_ENCHANT_ERROR :
		case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
			cDefenserJudge = ClientConstants::Judge_Enchant;
			break;

		case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
			wError = PktAtAck::FAIL_SPELL;
			break;

		case CSpell::ENCHANT_FAIL_BY_RESIST :
			cDefenserJudge = ClientConstants::Judge_Resist;
			break;
	}

	return 0;
}



//-------------------------------------------------------------------------------------------------
// 기타 (포션 사용)
DEFINE_SKILL(UseFood)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	unsigned short wAdditionalHP = (attackType.m_cSkillLockCount + 1) * 100;//hz was 100

	if (Creature::CT_PC == Creature::GetCreatureType(lpVictim->GetCID()))
	{
		CCharacter* lpCharacter = reinterpret_cast<CCharacter *>(lpVictim);
		float fd = static_cast<float>(lpCharacter->GetAbilityValue(Skill::Type::AB_FOOD_UP))/100.0f;
		wAdditionalHP += static_cast<unsigned short>((float)wAdditionalHP*fd);
	}

	if (lpVictim->m_CreatureStatus.m_nNowHP > USHRT_MAX - wAdditionalHP)
	{
		lpVictim->m_CreatureStatus.m_nNowHP = lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxHP;
	}
	else
	{
		lpVictim->m_CreatureStatus.m_nNowHP += wAdditionalHP;
		lpVictim->m_CreatureStatus.m_nNowHP = 
			std::min(lpVictim->m_CreatureStatus.m_nNowHP, lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxHP);
	}

	cDefenserJudge = ClientConstants::Judge_Heal;
	return 0;
}

DEFINE_SKILL(UseDrink)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);
		
	 
	 


	unsigned short wAdditionalMP = (attackType.m_cSkillLockCount + 1) * 100;//was 100
	const unsigned short wOldMP = lpVictim->m_CreatureStatus.m_nNowMP;

	if (Creature::CT_PC == Creature::GetCreatureType(lpVictim->GetCID()))
	{
		CCharacter* lpCharacter = reinterpret_cast<CCharacter *>(lpVictim);
		float fd = static_cast<float>(lpCharacter->GetAbilityValue(Skill::Type::AB_FOOD_UP))/100.0f;
		wAdditionalMP += static_cast<unsigned short>((float)wAdditionalMP*fd);
	}

	if (lpVictim->m_CreatureStatus.m_nNowMP > USHRT_MAX - wAdditionalMP)
	{
		lpVictim->m_CreatureStatus.m_nNowMP = lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxMP;
	}
	else
	{
		lpVictim->m_CreatureStatus.m_nNowMP += wAdditionalMP;
		lpVictim->m_CreatureStatus.m_nNowMP = 
			std::min(lpVictim->m_CreatureStatus.m_nNowMP, lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxMP);
	}

	wDefenserMPHeal = (lpVictim->m_CreatureStatus.m_nNowMP > wOldMP) ? lpVictim->m_CreatureStatus.m_nNowMP - wOldMP : 0;

	cDefenserJudge = ClientConstants::Judge_ManaHeal;
	return 0;
}
/*
DEFINE_SKILL(UseDrinkPer)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	float fPer = (attackType.m_cSkillLockCount+1) * 0.1f;

	const unsigned short wAdditionalMP = static_cast<unsigned short>(lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxMP * fPer);	

	const unsigned short wOldMP = lpVictim->m_CreatureStatus.m_nNowMP;

	if (lpVictim->m_CreatureStatus.m_nNowMP > USHRT_MAX - wAdditionalMP)
	{
		lpVictim->m_CreatureStatus.m_nNowMP = lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxMP;
	}
	else
	{
		lpVictim->m_CreatureStatus.m_nNowMP += wAdditionalMP;
		lpVictim->m_CreatureStatus.m_nNowMP = 
			std::min(lpVictim->m_CreatureStatus.m_nNowMP, lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxMP);
	}

	wDefenserMPHeal = (lpVictim->m_CreatureStatus.m_nNowMP > wOldMP) ? lpVictim->m_CreatureStatus.m_nNowMP - wOldMP : 0;

	cDefenserJudge = ClientConstants::Judge_ManaHeal;
	return 0;
}
*/
DEFINE_SKILL(UseFusionFood)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);
		
	unsigned short wAdditionalHP = (attackType.m_cSkillLockCount + 1) * 75;//was *75

	if (Creature::CT_PC == Creature::GetCreatureType(lpVictim->GetCID()))
	{
		CCharacter* lpCharacter = reinterpret_cast<CCharacter *>(lpVictim);
		float fd = static_cast<float>(lpCharacter->GetAbilityValue(Skill::Type::AB_FOOD_UP))/100.0f;
		wAdditionalHP += static_cast<unsigned short>((float)wAdditionalHP*fd);
	}

	if (lpVictim->m_CreatureStatus.m_nNowHP > USHRT_MAX - wAdditionalHP)
	{
		lpVictim->m_CreatureStatus.m_nNowHP = lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxHP;
	}
	else
	{
		lpVictim->m_CreatureStatus.m_nNowHP += wAdditionalHP;
		lpVictim->m_CreatureStatus.m_nNowHP = 
			std::min(lpVictim->m_CreatureStatus.m_nNowHP, lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxHP);
	}

	// MP
	unsigned short wAdditionalMP = (attackType.m_cSkillLockCount + 1) * 75;
	const unsigned short wOldMP = lpVictim->m_CreatureStatus.m_nNowMP;

	if (Creature::CT_PC == Creature::GetCreatureType(lpVictim->GetCID()))
	{
		CCharacter* lpCharacter = reinterpret_cast<CCharacter *>(lpVictim);
		float fd = static_cast<float>(lpCharacter->GetAbilityValue(Skill::Type::AB_FOOD_UP))/100.0f;
		wAdditionalMP += static_cast<unsigned short>((float)wAdditionalMP*fd);
	}

	if (lpVictim->m_CreatureStatus.m_nNowMP > USHRT_MAX - wAdditionalMP)
	{
		lpVictim->m_CreatureStatus.m_nNowMP = lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxMP;
	}
	else
	{
		lpVictim->m_CreatureStatus.m_nNowMP += wAdditionalMP;
		lpVictim->m_CreatureStatus.m_nNowMP = 
			std::min(lpVictim->m_CreatureStatus.m_nNowMP, lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxMP);
	}

	wDefenserMPHeal = (lpVictim->m_CreatureStatus.m_nNowMP > wOldMP) ? lpVictim->m_CreatureStatus.m_nNowMP - wOldMP : 0;

	cDefenserJudge = ClientConstants::Judge_Heal;
	return 0;
}
/*
DEFINE_SKILL(UseFusionFoodPer)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	// HP
	float fPer = (attackType.m_cSkillLockCount+1) * 0.08f;

	const unsigned short wAdditionalHP = static_cast<unsigned short>(lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxHP * fPer);	

	if (lpVictim->m_CreatureStatus.m_nNowHP > USHRT_MAX - wAdditionalHP)
	{
		lpVictim->m_CreatureStatus.m_nNowHP = lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxHP;
	}
	else
	{
		lpVictim->m_CreatureStatus.m_nNowHP += wAdditionalHP;
		lpVictim->m_CreatureStatus.m_nNowHP = 
			std::min(lpVictim->m_CreatureStatus.m_nNowHP, lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxHP);
	}

	cDefenserJudge = ClientConstants::Judge_Heal;

	// MP
	fPer = (attackType.m_cSkillLockCount+1) * 0.08f;

	const unsigned short wAdditionalMP = static_cast<unsigned short>(lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxMP * fPer);	

	const unsigned short wOldMP = lpVictim->m_CreatureStatus.m_nNowMP;

	if (lpVictim->m_CreatureStatus.m_nNowMP > USHRT_MAX - wAdditionalMP)
	{
		lpVictim->m_CreatureStatus.m_nNowMP = lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxMP;
	}
	else
	{
		lpVictim->m_CreatureStatus.m_nNowMP += wAdditionalMP;
		lpVictim->m_CreatureStatus.m_nNowMP = 
			std::min(lpVictim->m_CreatureStatus.m_nNowMP, lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxMP);
	}

	wDefenserMPHeal = (lpVictim->m_CreatureStatus.m_nNowMP > wOldMP) ? lpVictim->m_CreatureStatus.m_nNowMP - wOldMP : 0;

	cDefenserJudge = ClientConstants::Judge_Heal;
	return 0;
}
*/

DEFINE_SKILL(UseHealPotionPer)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	float fPer = 0.0f;
	switch (attackType.m_cSkillLockCount) 
	{
		case 0:	fPer = 0.2f;	break;			
		case 1:	fPer = 0.3f;	break;			
		case 2:	fPer = 0.4f;	break;			
		case 3:	fPer = 0.8f;	break;			
		case 4:	fPer = 0.6f;	break;			
		case 5:	fPer = 0.7f;	break;			
		case 6:	fPer = 1.0f;	break;			
	}   	

	const unsigned short wAdditionalHP = static_cast<unsigned short>(lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxHP * fPer);	

	if (lpVictim->m_CreatureStatus.m_nNowHP > USHRT_MAX - wAdditionalHP)
	{
		lpVictim->m_CreatureStatus.m_nNowHP = lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxHP;
	}
	else
	{
		lpVictim->m_CreatureStatus.m_nNowHP += wAdditionalHP;
		lpVictim->m_CreatureStatus.m_nNowHP = 
			std::min(lpVictim->m_CreatureStatus.m_nNowHP, lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxHP);
	}

	cDefenserJudge = ClientConstants::Judge_Heal;

	return 0;
}

DEFINE_SKILL(UseManaPotionPer)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	float fPer = 0.0f;
	switch (attackType.m_cSkillLockCount) 
	{
		case 0:	fPer = 0.2f;	break;			
		case 1:	fPer = 0.3f;	break;			
		case 2:	fPer = 0.4f;	break;			
		case 3:	fPer = 0.5f;	break;			
		case 4:	fPer = 0.6f;	break;			
		case 5:	fPer = 0.7f;	break;			
		case 6:	fPer = 1.0f;	break;			
	}   	

	const unsigned short wAdditionalMP = static_cast<unsigned short>(lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxMP * fPer);	

	const unsigned short wOldMP = lpVictim->m_CreatureStatus.m_nNowMP;

	if (lpVictim->m_CreatureStatus.m_nNowMP > USHRT_MAX - wAdditionalMP)
	{
		lpVictim->m_CreatureStatus.m_nNowMP = lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxMP;
	}
	else
	{
		lpVictim->m_CreatureStatus.m_nNowMP += wAdditionalMP;
		lpVictim->m_CreatureStatus.m_nNowMP = 
			std::min(lpVictim->m_CreatureStatus.m_nNowMP, lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxMP);
	}

	wDefenserMPHeal = (lpVictim->m_CreatureStatus.m_nNowMP > wOldMP) ? lpVictim->m_CreatureStatus.m_nNowMP - wOldMP : 0;

	cDefenserJudge = ClientConstants::Judge_ManaHeal;
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////  AVALANSA START POTION RYL2 SYSTEM  /////////////////////////////////////////////////////////////////////////////////
DEFINE_SKILL(UseRefreshPotionPer)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// avalansa start magic,magix resist + speed potion hahaha

	unsigned short wSpellLevel = (attackType.m_cSkillLockCount);

	unsigned short wResult = CAddSpell<CBuffPotionSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
		SpellType::BUFF_SPELL, SpellID::MagicPowerPotion, wSpellLevel, 300))(lpVictim);
	
	switch (wResult)
	{
		case CSpell::NO_ENCHANT_ERROR :
		case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
			cDefenserJudge = ClientConstants::Judge_Enchant;
			break;

		case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
			wError = PktAtAck::FAIL_SPELL;
			break;

		case CSpell::ENCHANT_FAIL_BY_RESIST :
			cDefenserJudge = ClientConstants::Judge_Resist;
			break;
     }

	///////////////////////////////////////////// DISPEL SYSTEM ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

	if (0 != lpVictim->GetSpellMgr().GetAffectedInfo().Disenchant(SpellType::MAGICAL_SPELL, SpellTarget::ALL_ENCHANT, 
		Disenchant::NONE, 0, Disenchant::INFINITE_NUM))
	{
		lpVictim->GetThreat().AddToThreatList(lpSkillUser, 1);

		Creature::CreatureType eCreatureType = Creature::GetCreatureType(lpVictim->GetCID());
		if (Creature::CT_MONSTER == eCreatureType ||
			Creature::CT_SUMMON == eCreatureType ||
			Creature::CT_STRUCT == eCreatureType)
		{
			CMonster* lpMonster = reinterpret_cast<CMonster *>(lpVictim);
			lpMonster->Attacked();
		}
	}

	cDefenserJudge = ClientConstants::Judge_Disenchant;

	// avalansa end magic,magix resist + speed potion
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	unsigned short wAdditionalHP = (attackType.m_cSkillLockCount + 1) * 1000;

	if (Creature::CT_PC == Creature::GetCreatureType(lpVictim->GetCID()))
	{
		CCharacter* lpCharacter = reinterpret_cast<CCharacter *>(lpVictim);
		float fd = static_cast<float>(lpCharacter->GetAbilityValue(Skill::Type::AB_FOOD_UP))/100.0f;
		wAdditionalHP += static_cast<unsigned short>((float)wAdditionalHP*fd);
	}

	if (lpVictim->m_CreatureStatus.m_nNowHP > USHRT_MAX - wAdditionalHP)
	{
		lpVictim->m_CreatureStatus.m_nNowHP = lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxHP;
	}
	else
	{
		lpVictim->m_CreatureStatus.m_nNowHP += wAdditionalHP;
		lpVictim->m_CreatureStatus.m_nNowHP = 
			std::min(lpVictim->m_CreatureStatus.m_nNowHP, lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxHP);
	}

	// MP
	unsigned short wAdditionalMP = (attackType.m_cSkillLockCount + 1) * 1000;
	const unsigned short wOldMP = lpVictim->m_CreatureStatus.m_nNowMP;

	if (Creature::CT_PC == Creature::GetCreatureType(lpVictim->GetCID()))
	{
		CCharacter* lpCharacter = reinterpret_cast<CCharacter *>(lpVictim);
		float fd = static_cast<float>(lpCharacter->GetAbilityValue(Skill::Type::AB_FOOD_UP))/100.0f;
		wAdditionalMP += static_cast<unsigned short>((float)wAdditionalMP*fd);
	}

	if (lpVictim->m_CreatureStatus.m_nNowMP > USHRT_MAX - wAdditionalMP)
	{
		lpVictim->m_CreatureStatus.m_nNowMP = lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxMP;
	}
	else
	{
		lpVictim->m_CreatureStatus.m_nNowMP += wAdditionalMP;
		lpVictim->m_CreatureStatus.m_nNowMP = 
			std::min(lpVictim->m_CreatureStatus.m_nNowMP, lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxMP);
	}

	wDefenserMPHeal = (lpVictim->m_CreatureStatus.m_nNowMP > wOldMP) ? lpVictim->m_CreatureStatus.m_nNowMP - wOldMP : 0;

	cDefenserJudge = ClientConstants::Judge_Heal;
	return 0;
}

DEFINE_SKILL(DemagePotionPer)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	// 0~7이면 10%~80%
//	const float fPer = (attackType.m_cSkillLockCount+1)*0.1f;
	float fPer = 0.0f;

	unsigned char cSpellType = SpellType::BUFF_SPELL;
	unsigned short shTime = 600;
	switch (attackType.m_cSkillLockCount) 
	{
		case 0:	fPer = 0.05f;	break;			
		case 1:	fPer = 0.10f;	break;			
		case 2:	fPer = 0.15f;	break;			
		case 3:	fPer = 0.30f;	cSpellType = SpellType::PAYBUFF_SPELL; shTime = 1800;	break;			
		case 4:	fPer = 0.12f;	break;			
		case 5:	fPer = 0.15f;	cSpellType = SpellType::PAYBUFF_SPELL; shTime = 1800;	break;			
		case 6:	fPer = 0.3f;	cSpellType = SpellType::PAYBUFF_SPELL; shTime = 1800;	break;			
	}   	

	unsigned short wSpellLevel = (int)(fPer*100.0f);//static_cast<unsigned short>(lpVictim->m_EquipStatus.m_lMinDamage*fPer);

	if(wSpellLevel == 0)
		wSpellLevel = 1;

	unsigned short wResult = CAddSpell<CBuffPotionSpell>(CSpell::Spell_Info(ProtoType, NULL, 
//	unsigned short wResult = CAddSpell<CBuffPotionSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
		cSpellType, SpellID::DemagePotion, wSpellLevel, shTime))(lpVictim);
	
	switch (wResult)
	{
		case CSpell::NO_ENCHANT_ERROR :
		case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
			cDefenserJudge = ClientConstants::Judge_Enchant;
			break;

		case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
			wError = PktAtAck::FAIL_SPELL;
			break;

		case CSpell::ENCHANT_FAIL_BY_RESIST :
			cDefenserJudge = ClientConstants::Judge_Resist;
			break;
	}

	if(cSpellType == SpellType::PAYBUFF_SPELL)
		lpSkillUser->SaveSpell();

	return 0;
}


DEFINE_SKILL(ArmorPotionPer)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	float fPer = 0.0f;
	unsigned char cSpellType = SpellType::BUFF_SPELL;
	unsigned short shTime = 600;
	switch (attackType.m_cSkillLockCount) 
	{
		case 0:	fPer = 0.05f;	break;			
		case 1:	fPer = 0.10f;	break;			
		case 2:	fPer = 0.15f;	break;			
		case 3:	fPer = 0.30f;	cSpellType = SpellType::PAYBUFF_SPELL; shTime = 1800;	break;			
		case 4:	fPer = 0.12f;	cSpellType = SpellType::PAYBUFF_SPELL; shTime = 1800;	break;			
		case 5:	fPer = 0.15f;	cSpellType = SpellType::PAYBUFF_SPELL; shTime = 1800;	break;			
		case 6:	fPer = 0.3f;	cSpellType = SpellType::PAYBUFF_SPELL; shTime = 1800;	break;			
	}   	

	unsigned short wSpellLevel = (int)(fPer*100.0f);//static_cast<unsigned short>(lpVictim->m_EquipStatus.m_wArmor*fPer);
	if(wSpellLevel == 0)
		wSpellLevel = 1;

	unsigned short wResult = CAddSpell<CBuffPotionSpell>(CSpell::Spell_Info(ProtoType, NULL, 
//	unsigned short wResult = CAddSpell<CBuffPotionSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
		cSpellType, SpellID::ArmorPotion, wSpellLevel, shTime))(lpVictim);
	
	switch (wResult)
	{
		case CSpell::NO_ENCHANT_ERROR :
		case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
			cDefenserJudge = ClientConstants::Judge_Enchant;
			break;

		case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
			wError = PktAtAck::FAIL_SPELL;
			break;

		case CSpell::ENCHANT_FAIL_BY_RESIST :
			cDefenserJudge = ClientConstants::Judge_Resist;
			break;
	}

	if(cSpellType == SpellType::PAYBUFF_SPELL)
		lpSkillUser->SaveSpell();

	return 0;
}

/*
DEFINE_SKILL(HitRatePotion)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	// 0~7이면 10~80 까지 증가시켜줌
	unsigned short wSpellLevel = (attackType.m_cSkillLockCount+1)*3;

	unsigned short wResult = CAddSpell<CBuffPotionSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
		SpellType::BUFF_SPELL, SpellID::HitRatePotion, wSpellLevel, 300))(lpVictim);
	
	switch (wResult)
	{
		case CSpell::NO_ENCHANT_ERROR :
		case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
			cDefenserJudge = ClientConstants::Judge_Enchant;
			break;

		case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
			wError = PktAtAck::FAIL_SPELL;
			break;

		case CSpell::ENCHANT_FAIL_BY_RESIST :
			cDefenserJudge = ClientConstants::Judge_Resist;
			break;
	}

	return 0;
}
*/
DEFINE_SKILL(HitRatePotionPer)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	 
	 


	float fPer = 0.0f;
	unsigned char cSpellType = SpellType::BUFF_SPELL;
	unsigned short shTime = 600;
	switch (attackType.m_cSkillLockCount) 
	{
		case 0:	fPer = 0.03f;	break;			
		case 1:	fPer = 0.06f;	break;			
		case 2:	fPer = 0.09f;	break;			
		case 3:	fPer = 0.12f;	break;			
		case 4:	fPer = 0.12f;	break;			
		case 5:	fPer = 0.15f;	cSpellType = SpellType::PAYBUFF_SPELL; shTime = 1800;	break;			
		case 6:	fPer = 0.3f;	cSpellType = SpellType::PAYBUFF_SPELL; shTime = 1800;	break;			
	}   	
	unsigned short wSpellLevel = (int)(fPer*100.0f);//static_cast<unsigned short>(lpVictim->m_EquipStatus.m_wHitRate*fPer);
	if(wSpellLevel == 0)
		wSpellLevel = 1;

	unsigned short wResult = CAddSpell<CBuffPotionSpell>(CSpell::Spell_Info(ProtoType, NULL, 
//	unsigned short wResult = CAddSpell<CBuffPotionSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
		cSpellType, SpellID::HitRatePotion, wSpellLevel, shTime))(lpVictim);
	
	switch (wResult)
	{
		case CSpell::NO_ENCHANT_ERROR :
		case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
			cDefenserJudge = ClientConstants::Judge_Enchant;
			break;

		case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
			wError = PktAtAck::FAIL_SPELL;
			break;

		case CSpell::ENCHANT_FAIL_BY_RESIST :
			cDefenserJudge = ClientConstants::Judge_Resist;
			break;
	}

	if(cSpellType == SpellType::PAYBUFF_SPELL)
		lpSkillUser->SaveSpell();

	return 0;
}

/*
DEFINE_SKILL(EvadePotion)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	// 0~7이면 10~80 까지 증가시켜줌
	unsigned short wSpellLevel = (attackType.m_cSkillLockCount+1)*3;

	unsigned short wResult = CAddSpell<CBuffPotionSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
		SpellType::BUFF_SPELL, SpellID::EvadePotion, wSpellLevel, 300))(lpVictim);
	
	switch (wResult)
	{
		case CSpell::NO_ENCHANT_ERROR :
		case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
			cDefenserJudge = ClientConstants::Judge_Enchant;
			break;

		case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
			wError = PktAtAck::FAIL_SPELL;
			break;

		case CSpell::ENCHANT_FAIL_BY_RESIST :
			cDefenserJudge = ClientConstants::Judge_Resist;
			break;
	}

	return 0;
}
*/

DEFINE_SKILL(EvadePotionPer)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	 
	 


	float fPer = 0.0f;
	unsigned char cSpellType = SpellType::BUFF_SPELL;
	unsigned short shTime = 600;
	switch (attackType.m_cSkillLockCount) 
	{
		case 0:	fPer = 0.03f;	break;			
		case 1:	fPer = 0.06f;	break;			
		case 2:	fPer = 0.09f;	break;			
		case 3:	fPer = 0.12f;	break;			
		case 4:	fPer = 0.12f;	break;			
		case 5:	fPer = 0.15f;	cSpellType = SpellType::PAYBUFF_SPELL; shTime = 1800;	break;			
		case 6:	fPer = 0.3f;	cSpellType = SpellType::PAYBUFF_SPELL; shTime = 1800;	break;			
	}   	

	unsigned short wSpellLevel = (int)(fPer*100.0f);//static_cast<unsigned short>(lpVictim->m_EquipStatus.m_wEvade*fPer);
	if(wSpellLevel == 0)
		wSpellLevel = 1;

	unsigned short wResult = CAddSpell<CBuffPotionSpell>(CSpell::Spell_Info(ProtoType, NULL, 
//	unsigned short wResult = CAddSpell<CBuffPotionSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
		cSpellType, SpellID::EvadePotion, wSpellLevel, shTime))(lpVictim);
	
	switch (wResult)
	{
		case CSpell::NO_ENCHANT_ERROR :
		case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
			cDefenserJudge = ClientConstants::Judge_Enchant;
			break;

		case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
			wError = PktAtAck::FAIL_SPELL;
			break;

		case CSpell::ENCHANT_FAIL_BY_RESIST :
			cDefenserJudge = ClientConstants::Judge_Resist;
			break;
	}

	if(cSpellType == SpellType::PAYBUFF_SPELL)
		lpSkillUser->SaveSpell();

	return 0;
}
/*
DEFINE_SKILL(MaxHPPotion)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	unsigned short wGrade[7] = { 300, 600, 900, 1200, 1500, 2000, 3000 };

	// 0~7이면 10~80 까지 증가시켜줌
	unsigned short wSpellLevel = wGrade[attackType.m_cSkillLockCount];

	unsigned short wResult = CAddSpell<CBuffPotionSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
		SpellType::BUFF_SPELL, SpellID::MaxHPPotion, wSpellLevel, 300))(lpVictim);
	
	switch (wResult)
	{
		case CSpell::NO_ENCHANT_ERROR :
		case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
			cDefenserJudge = ClientConstants::Judge_Enchant;
			break;

		case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
			wError = PktAtAck::FAIL_SPELL;
			break;

		case CSpell::ENCHANT_FAIL_BY_RESIST :
			cDefenserJudge = ClientConstants::Judge_Resist;
			break;
	}

	return 0;
}
*/
DEFINE_SKILL(MaxHPPotionPer)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	float fPer = 0.0f;
	unsigned char cSpellType = SpellType::BUFF_SPELL;
	unsigned short shTime = 600;
	switch (attackType.m_cSkillLockCount) 
	{
		case 0:	fPer = 0.05f;	break;			
		case 1:	fPer = 0.10f;	break;			
		case 2:	fPer = 0.15f;	break;			
		case 3:	fPer = 0.30f;	cSpellType = SpellType::PAYBUFF_SPELL; shTime = 1800;	break;			
		case 4:	fPer = 0.12f;	break;			
		case 5:	fPer = 0.15f;	cSpellType = SpellType::PAYBUFF_SPELL; shTime = 1800;	break;			
		case 6:	fPer = 0.3f;	cSpellType = SpellType::PAYBUFF_SPELL; shTime = 1800;	break;			
	}   	

	unsigned short wSpellLevel = (int)(fPer*100.0f);//static_cast<unsigned short>(lpVictim->m_EquipStatus.m_nMaxHP*fPer);
	if(wSpellLevel == 0)
		wSpellLevel = 1;

	unsigned short wResult = CAddSpell<CBuffPotionSpell>(CSpell::Spell_Info(ProtoType, NULL, 
//	unsigned short wResult = CAddSpell<CBuffPotionSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
		cSpellType, SpellID::MaxHPPotion, wSpellLevel, shTime))(lpVictim);
	
	switch (wResult)
	{
		case CSpell::NO_ENCHANT_ERROR :
		case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
			cDefenserJudge = ClientConstants::Judge_Enchant;
			break;

		case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
			wError = PktAtAck::FAIL_SPELL;
			break;

		case CSpell::ENCHANT_FAIL_BY_RESIST :
			cDefenserJudge = ClientConstants::Judge_Resist;
			break;
	}

	if(cSpellType == SpellType::PAYBUFF_SPELL)
		lpSkillUser->SaveSpell();

	return 0;
}
/*
DEFINE_SKILL(MaxMPPotion)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	unsigned short wGrade[7] = { 300, 600, 900, 1200, 1500, 2000, 3000 };

	// 0~7이면 10~80 까지 증가시켜줌
	unsigned short wSpellLevel = wGrade[attackType.m_cSkillLockCount];

	unsigned short wResult = CAddSpell<CBuffPotionSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
		SpellType::BUFF_SPELL, SpellID::MaxMPPotion, wSpellLevel, 300))(lpVictim);
	
	switch (wResult)
	{
		case CSpell::NO_ENCHANT_ERROR :
		case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
			cDefenserJudge = ClientConstants::Judge_Enchant;
			break;

		case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
			wError = PktAtAck::FAIL_SPELL;
			break;

		case CSpell::ENCHANT_FAIL_BY_RESIST :
			cDefenserJudge = ClientConstants::Judge_Resist;
			break;
	}

	return 0;
}
*/
DEFINE_SKILL(MaxMPPotionPer)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	 
	 


	float fPer = 0.0f;
	unsigned char cSpellType = SpellType::BUFF_SPELL;
	unsigned short shTime = 600;
	switch (attackType.m_cSkillLockCount) 
	{
		case 0:	fPer = 0.03f;	break;			
		case 1:	fPer = 0.06f;	break;			
		case 2:	fPer = 0.09f;	break;			
		case 3:	fPer = 0.12f;	break;			
		case 4:	fPer = 0.12f;	break;			
		case 5:	fPer = 0.15f;	cSpellType = SpellType::PAYBUFF_SPELL; shTime = 1800;	break;			
		case 6:	fPer = 0.3f;	cSpellType = SpellType::PAYBUFF_SPELL; shTime = 1800;	break;			
	}   	

	unsigned short wSpellLevel = (int)(fPer*100.0f);//static_cast<unsigned short>(lpVictim->m_EquipStatus.m_nMaxMP*fPer);
	if(wSpellLevel == 0)
		wSpellLevel = 1;

	unsigned short wResult = CAddSpell<CBuffPotionSpell>(CSpell::Spell_Info(ProtoType, NULL, 
//	unsigned short wResult = CAddSpell<CBuffPotionSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
		cSpellType, SpellID::MaxMPPotion, wSpellLevel, shTime))(lpVictim);
	
	switch (wResult)
	{
		case CSpell::NO_ENCHANT_ERROR :
		case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
			cDefenserJudge = ClientConstants::Judge_Enchant;
			break;

		case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
			wError = PktAtAck::FAIL_SPELL;
			break;

		case CSpell::ENCHANT_FAIL_BY_RESIST :
			cDefenserJudge = ClientConstants::Judge_Resist;
			break;
	}

	if(cSpellType == SpellType::PAYBUFF_SPELL)
		lpSkillUser->SaveSpell();

	return 0;
}
/*
DEFINE_SKILL(HPRegenPotion)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	// 0~7이면 10~80 까지 증가시켜줌
	unsigned short wSpellLevel = (attackType.m_cSkillLockCount+1)*5;

	unsigned short wResult = CAddSpell<CBuffPotionSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
		SpellType::BUFF_SPELL, SpellID::HPRegenPotion, wSpellLevel, 300))(lpVictim);
	
	switch (wResult)
	{
		case CSpell::NO_ENCHANT_ERROR :
		case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
			cDefenserJudge = ClientConstants::Judge_Enchant;
			break;

		case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
			wError = PktAtAck::FAIL_SPELL;
			break;

		case CSpell::ENCHANT_FAIL_BY_RESIST :
			cDefenserJudge = ClientConstants::Judge_Resist;
			break;
	}

	return 0;
}

DEFINE_SKILL(HPRegenPotionPer)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	// 0~7이면 10%~80%
	const float fPer = (attackType.m_cSkillLockCount+1)*0.1f;
	unsigned short wSpellLevel = static_cast<unsigned short>(lpVictim->m_EquipStatus.m_wHPRegen*fPer);

	unsigned short wResult = CAddSpell<CBuffPotionSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
		SpellType::BUFF_SPELL, SpellID::HPRegenPotion, wSpellLevel, 300))(lpVictim);
	
	switch (wResult)
	{
		case CSpell::NO_ENCHANT_ERROR :
		case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
			cDefenserJudge = ClientConstants::Judge_Enchant;
			break;

		case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
			wError = PktAtAck::FAIL_SPELL;
			break;

		case CSpell::ENCHANT_FAIL_BY_RESIST :
			cDefenserJudge = ClientConstants::Judge_Resist;
			break;
	}
	return 0;
}

DEFINE_SKILL(MPRegenPotion)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	// 0~7이면 10~80 까지 증가시켜줌
	unsigned short wSpellLevel = (attackType.m_cSkillLockCount+1)*5;

	unsigned short wResult = CAddSpell<CBuffPotionSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
		SpellType::BUFF_SPELL, SpellID::MPRegenPotion, wSpellLevel, 300))(lpVictim);
	
	switch (wResult)
	{
		case CSpell::NO_ENCHANT_ERROR :
		case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
			cDefenserJudge = ClientConstants::Judge_Enchant;
			break;

		case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
			wError = PktAtAck::FAIL_SPELL;
			break;

		case CSpell::ENCHANT_FAIL_BY_RESIST :
			cDefenserJudge = ClientConstants::Judge_Resist;
			break;
	}

	return 0;
}

DEFINE_SKILL(MPRegenPotionPer)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	// 0~7이면 10%~80%
	const float fPer = (attackType.m_cSkillLockCount+1)*0.1f;
	unsigned short wSpellLevel = static_cast<unsigned short>(lpVictim->m_EquipStatus.m_wMPRegen*fPer);

	unsigned short wResult = CAddSpell<CBuffPotionSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
		SpellType::BUFF_SPELL, SpellID::MPRegenPotion, wSpellLevel, 300))(lpVictim);
	
	switch (wResult)
	{
		case CSpell::NO_ENCHANT_ERROR :
		case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
			cDefenserJudge = ClientConstants::Judge_Enchant;
			break;

		case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
			wError = PktAtAck::FAIL_SPELL;
			break;

		case CSpell::ENCHANT_FAIL_BY_RESIST :
			cDefenserJudge = ClientConstants::Judge_Resist;
			break;
	}
	return 0;
}

DEFINE_SKILL(CriticalPotion)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	// 0~7이면 10~80 까지 증가시켜줌
	unsigned short wSpellLevel = (attackType.m_cSkillLockCount+1)*3;

	unsigned short wResult = CAddSpell<CBuffPotionSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
		SpellType::BUFF_SPELL, SpellID::CriticalPotion, wSpellLevel, 300))(lpVictim);
	
	switch (wResult)
	{
		case CSpell::NO_ENCHANT_ERROR :
		case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
			cDefenserJudge = ClientConstants::Judge_Enchant;
			break;

		case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
			wError = PktAtAck::FAIL_SPELL;
			break;

		case CSpell::ENCHANT_FAIL_BY_RESIST :
			cDefenserJudge = ClientConstants::Judge_Resist;
			break;
	}

	return 0;
}
*/
DEFINE_SKILL(CriticalPotionPer)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	float fPer = 0.0f;
	unsigned char cSpellType = SpellType::BUFF_SPELL;
	unsigned short shTime = 600;

	switch (attackType.m_cSkillLockCount) 
	{
		case 0:	fPer = 0.05f;	break;			
		case 1:	fPer = 0.10f;	break;			
		case 2:	fPer = 0.15f;	break;			
		case 3:	fPer = 0.30f;	cSpellType = SpellType::PAYBUFF_SPELL; shTime = 1800;	break;			
		case 4:	fPer = 0.12f;	break;			
		case 5:	fPer = 0.15f;	cSpellType = SpellType::PAYBUFF_SPELL; shTime = 1800;	break;			
		case 6:	fPer = 0.3f;	cSpellType = SpellType::PAYBUFF_SPELL; shTime = 1800;	break;			
	} 
	unsigned short wSpellLevel = (int)(fPer*100.0f);//static_cast<unsigned short>(lpVictim->m_EquipStatus.m_wCritical*fPer);
	if(wSpellLevel == 0)
		wSpellLevel = 1;

	unsigned short wResult = CAddSpell<CBuffPotionSpell>(CSpell::Spell_Info(ProtoType, NULL, 
//	unsigned short wResult = CAddSpell<CBuffPotionSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
		cSpellType, SpellID::CriticalPotion, wSpellLevel, shTime))(lpVictim);
	
	switch (wResult)
	{
		case CSpell::NO_ENCHANT_ERROR :
		case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
			cDefenserJudge = ClientConstants::Judge_Enchant;
			break;

		case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
			wError = PktAtAck::FAIL_SPELL;
			break;

		case CSpell::ENCHANT_FAIL_BY_RESIST :
			cDefenserJudge = ClientConstants::Judge_Resist;
			break;
	}

	if(cSpellType == SpellType::PAYBUFF_SPELL)
		lpSkillUser->SaveSpell();

	return 0;
}
/*
DEFINE_SKILL(BlockPotion)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	// 0~7이면 10~80 까지 증가시켜줌
	unsigned short wSpellLevel = (attackType.m_cSkillLockCount+1)*3;

	unsigned short wResult = CAddSpell<CBuffPotionSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
		SpellType::BUFF_SPELL, SpellID::BlockPotion, wSpellLevel, 300))(lpVictim);
	
	switch (wResult)
	{
		case CSpell::NO_ENCHANT_ERROR :
		case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
			cDefenserJudge = ClientConstants::Judge_Enchant;
			break;

		case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
			wError = PktAtAck::FAIL_SPELL;
			break;

		case CSpell::ENCHANT_FAIL_BY_RESIST :
			cDefenserJudge = ClientConstants::Judge_Resist;
			break;
	}

	return 0;
}
*/
DEFINE_SKILL(BlockPotionPer)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	 
	 


	float fPer = 0.0f;
	unsigned char cSpellType = SpellType::BUFF_SPELL;
	unsigned short shTime = 600;

	switch (attackType.m_cSkillLockCount) 
	{
		case 0:	fPer = 0.03f;	break;			
		case 1:	fPer = 0.06f;	break;			
		case 2:	fPer = 0.09f;	break;			
		case 3:	fPer = 0.12f;	break;			
		case 4:	fPer = 0.12f;	break;			
		case 5:	fPer = 0.15f;	cSpellType = SpellType::PAYBUFF_SPELL; shTime = 1800;	break;			
		case 6:	fPer = 0.3f;	cSpellType = SpellType::PAYBUFF_SPELL; shTime = 1800;	break;			
	} 

	unsigned short wSpellLevel = (int)(fPer*100.0f);//static_cast<unsigned short>(lpVictim->m_EquipStatus.m_wBlock*fPer);
	if(wSpellLevel == 0)
		wSpellLevel = 1;

	unsigned short wResult = CAddSpell<CBuffPotionSpell>(CSpell::Spell_Info(ProtoType, NULL, 
//	unsigned short wResult = CAddSpell<CBuffPotionSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
		cSpellType, SpellID::BlockPotion, wSpellLevel, shTime))(lpVictim);
	
	switch (wResult)
	{
		case CSpell::NO_ENCHANT_ERROR :
		case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
			cDefenserJudge = ClientConstants::Judge_Enchant;
			break;

		case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
			wError = PktAtAck::FAIL_SPELL;
			break;

		case CSpell::ENCHANT_FAIL_BY_RESIST :
			cDefenserJudge = ClientConstants::Judge_Resist;
			break;
	}

	if(cSpellType == SpellType::PAYBUFF_SPELL)
		lpSkillUser->SaveSpell();

	return 0;
}
/*
DEFINE_SKILL(SpeedPotion)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	// 0~7이면 10~80 까지 증가시켜줌
	unsigned short wSpellLevel = (attackType.m_cSkillLockCount+1)*10;

	unsigned short wResult = CAddSpell<CBuffPotionSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
		SpellType::BUFF_SPELL, SpellID::SpeedPotion, wSpellLevel, 300))(lpVictim);
	
	switch (wResult)
	{
		case CSpell::NO_ENCHANT_ERROR :
		case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
			cDefenserJudge = ClientConstants::Judge_Enchant;
			break;

		case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
			wError = PktAtAck::FAIL_SPELL;
			break;

		case CSpell::ENCHANT_FAIL_BY_RESIST :
			cDefenserJudge = ClientConstants::Judge_Resist;
			break;
	}

	return 0;
}
*/
DEFINE_SKILL(SpeedPotionPer)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	 
	 


	float fPer = 0.0f;
	unsigned char cSpellType = SpellType::BUFF_SPELL;
	unsigned short shTime = 600;

	switch (attackType.m_cSkillLockCount) 
	{
		case 0:	fPer = 0.03f;	break;			
		case 1:	fPer = 0.06f;	break;			
		case 2:	fPer = 0.09f;	break;			
		case 3:	fPer = 0.12f;	break;			
		case 4:	fPer = 0.12f;	break;			
		case 5:	fPer = 0.15f;	cSpellType = SpellType::PAYBUFF_SPELL; shTime = 1800;	break;			
		case 6:	fPer = 0.3f;	cSpellType = SpellType::PAYBUFF_SPELL; shTime = 1800;	break;			
	} 

	unsigned short wSpellLevel = (int)(fPer*100.0f);//static_cast<unsigned short>(lpVictim->m_EquipStatus.m_wSpeed*fPer);
	

	if(wSpellLevel == 0)
		wSpellLevel = 1;

	unsigned short wResult = CAddSpell<CBuffPotionSpell>(CSpell::Spell_Info(ProtoType, NULL, 
//	unsigned short wResult = CAddSpell<CBuffPotionSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
		cSpellType, SpellID::SpeedPotion, wSpellLevel, shTime))(lpVictim);
	
	switch (wResult)
	{
		case CSpell::NO_ENCHANT_ERROR :
		case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
			cDefenserJudge = ClientConstants::Judge_Enchant;
			break;

		case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
			wError = PktAtAck::FAIL_SPELL;
			break;

		case CSpell::ENCHANT_FAIL_BY_RESIST :
			cDefenserJudge = ClientConstants::Judge_Resist;
			break;
	}

	if(cSpellType == SpellType::PAYBUFF_SPELL)
		lpSkillUser->SaveSpell();

	return 0;
}
/*
DEFINE_SKILL(DeCoolDownPotion)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	// 0~7이면 10~80 까지 증가시켜줌
	unsigned short wSpellLevel = (attackType.m_cSkillLockCount+1)*10;

	unsigned short wResult = CAddSpell<CBuffPotionSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
		SpellType::BUFF_SPELL, SpellID::DeCoolDownPotion, wSpellLevel, 300))(lpVictim);
	
	switch (wResult)
	{
		case CSpell::NO_ENCHANT_ERROR :
		case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
			cDefenserJudge = ClientConstants::Judge_Enchant;
			break;

		case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
			wError = PktAtAck::FAIL_SPELL;
			break;

		case CSpell::ENCHANT_FAIL_BY_RESIST :
			cDefenserJudge = ClientConstants::Judge_Resist;
			break;
	}

	return 0;
}
*/
DEFINE_SKILL(DeCoolDownPotionPer)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	 
	 


	float fPer = 0.0f;
	unsigned char cSpellType = SpellType::BUFF_SPELL;
	unsigned short shTime = 600;

	switch (attackType.m_cSkillLockCount) 
	{
		case 0:	fPer = 0.03f;	break;			
		case 1:	fPer = 0.06f;	break;			
		case 2:	fPer = 0.09f;	break;			
		case 3:	fPer = 0.12f;	break;			
		case 4:	fPer = 0.12f;	break;			
		case 5:	fPer = 0.15f;	cSpellType = SpellType::PAYBUFF_SPELL; shTime = 1800;	break;			
		case 6:	fPer = 0.3f;	cSpellType = SpellType::PAYBUFF_SPELL; shTime = 1800;	break;			
	} 

	unsigned short wSpellLevel = (int)(fPer*100.0f);//static_cast<unsigned short>(lpVictim->m_EquipStatus.m_wSpeed*fPer);
	

	if(wSpellLevel == 0)
		wSpellLevel = 1;

	unsigned short wResult = CAddSpell<CBuffPotionSpell>(CSpell::Spell_Info(ProtoType, NULL, 
//	unsigned short wResult = CAddSpell<CBuffPotionSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
	cSpellType, SpellID::DeCoolDown, wSpellLevel, shTime))(lpVictim);
	
	switch (wResult)
	{
		case CSpell::NO_ENCHANT_ERROR :
		case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
			cDefenserJudge = ClientConstants::Judge_Enchant;
			break;

		case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
			wError = PktAtAck::FAIL_SPELL;
			break;

		case CSpell::ENCHANT_FAIL_BY_RESIST :
			cDefenserJudge = ClientConstants::Judge_Resist;
			break;
	}

	if(cSpellType == SpellType::PAYBUFF_SPELL)
		lpSkillUser->SaveSpell();

	return 0;
}

DEFINE_SKILL(MagicPowerPotionPer)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	// 0~7이면 10%~80%
	float fPer = 0.0f;
	unsigned char cSpellType = SpellType::BUFF_SPELL;
	unsigned short shTime = 600;

	switch (attackType.m_cSkillLockCount) 
	{
		case 0:	fPer = 0.05f;	break;			
		case 1:	fPer = 0.10f;	break;			
		case 2:	fPer = 0.15f;	break;			
		case 3:	fPer = 0.30f;	cSpellType = SpellType::PAYBUFF_SPELL; shTime = 1800;	break;			
		case 4:	fPer = 0.12f;	break;			
		case 5:	fPer = 0.15f;	cSpellType = SpellType::PAYBUFF_SPELL; shTime = 1800;	break;			
		case 6:	fPer = 0.3f;	cSpellType = SpellType::PAYBUFF_SPELL; shTime = 1800;	break;			
	}   	

	unsigned short wSpellLevel = (int)(fPer*100.0f);//static_cast<unsigned short>(lpVictim->m_EquipStatus.m_wMagicPower*fPer);
	if(wSpellLevel == 0)
		wSpellLevel = 1;

	unsigned short wResult = CAddSpell<CBuffPotionSpell>(CSpell::Spell_Info(ProtoType, NULL, 
//	unsigned short wResult = CAddSpell<CBuffPotionSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
		cSpellType, SpellID::MagicPowerPotion, wSpellLevel, shTime))(lpVictim);
	
	switch (wResult)
	{
		case CSpell::NO_ENCHANT_ERROR :
		case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
			cDefenserJudge = ClientConstants::Judge_Enchant;
			break;

		case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
			wError = PktAtAck::FAIL_SPELL;
			break;

		case CSpell::ENCHANT_FAIL_BY_RESIST :
			cDefenserJudge = ClientConstants::Judge_Resist;
			break;
	}

	if(cSpellType == SpellType::PAYBUFF_SPELL)
		lpSkillUser->SaveSpell();

	return 0;
}

/*
DEFINE_SKILL(MagicResistPotion)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	// 0~7이면 10~80 까지 증가시켜줌
	unsigned short wSpellLevel = (attackType.m_cSkillLockCount+1)*3;

	unsigned short wResult = CAddSpell<CBuffPotionSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
		SpellType::BUFF_SPELL, SpellID::MagicResistPotion, wSpellLevel, 300))(lpVictim);
	
	switch (wResult)
	{
		case CSpell::NO_ENCHANT_ERROR :
		case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
			cDefenserJudge = ClientConstants::Judge_Enchant;
			break;

		case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
			wError = PktAtAck::FAIL_SPELL;
			break;

		case CSpell::ENCHANT_FAIL_BY_RESIST :
			cDefenserJudge = ClientConstants::Judge_Resist;
			break;
	}

	return 0;
}
*/

DEFINE_SKILL(MagicResistPotionPer)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	// 0~7이면 10%~80%
	float fPer = 0.0f;
	unsigned char cSpellType = SpellType::BUFF_SPELL;
	unsigned short shTime = 600;

	switch (attackType.m_cSkillLockCount) 
	{
		case 0:	fPer = 0.05f;	break;			
		case 1:	fPer = 0.10f;	break;			
		case 2:	fPer = 0.15f;	break;			
		case 3:	fPer = 0.30f;	cSpellType = SpellType::PAYBUFF_SPELL; shTime = 1800;	break;				
		case 4:	fPer = 0.12f;	break;			
		case 5:	fPer = 0.15f;	cSpellType = SpellType::PAYBUFF_SPELL; shTime = 1800;	break;			
		case 6:	fPer = 0.3f;	cSpellType = SpellType::PAYBUFF_SPELL; shTime = 1800;	break;			
	}   	

	unsigned short wSpellLevel = (int)(fPer*100.0f);//static_cast<unsigned short>(lpVictim->m_EquipStatus.m_wMagicResist*fPer);
	if(wSpellLevel == 0)
		wSpellLevel = 1;

	unsigned short wResult = CAddSpell<CBuffPotionSpell>(CSpell::Spell_Info(ProtoType, NULL, 
//	unsigned short wResult = CAddSpell<CBuffPotionSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
		cSpellType, SpellID::MagicResistPotion, wSpellLevel, shTime))(lpVictim);
	
	switch (wResult)
	{
		case CSpell::NO_ENCHANT_ERROR :
		case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
			cDefenserJudge = ClientConstants::Judge_Enchant;
			break;

		case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
			wError = PktAtAck::FAIL_SPELL;
			break;

		case CSpell::ENCHANT_FAIL_BY_RESIST :
			cDefenserJudge = ClientConstants::Judge_Resist;
			break;
	}

	if(cSpellType == SpellType::PAYBUFF_SPELL)
		lpSkillUser->SaveSpell();

	return 0;
}

DEFINE_SKILL(ConcentrationPotionPer)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	 
	 


	// 0~7이면 10%~80%
	unsigned char cSpellType = SpellType::BUFF_SPELL;
	unsigned short shTime = 5;

	switch (attackType.m_cSkillLockCount) 
	{
		case 0:	shTime = 10;	break;			
		case 1:	shTime = 30;	break;			
//		case 2:	shTime = 15;	break;			
//		case 3:	shTime = 20;	break;			
//		case 4:	shTime = 25;	break;			
//		case 5:	cSpellType = SpellType::PAYBUFF_SPELL; shTime = 40;	break;			
//		case 6:	cSpellType = SpellType::PAYBUFF_SPELL; shTime = 60;	break;			
	}   	

	unsigned short wSpellLevel = attackType.m_cSkillLockCount+1;

	unsigned short wResult = CAddSpell<CBuffPotionSpell>(CSpell::Spell_Info(ProtoType, NULL, 
//	unsigned short wResult = CAddSpell<CBuffPotionSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
		cSpellType, SpellID::Concentration, wSpellLevel, shTime))(lpVictim);
	
	switch (wResult)
	{
		case CSpell::NO_ENCHANT_ERROR :
		case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
			cDefenserJudge = ClientConstants::Judge_Enchant;
			break;

		case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
			wError = PktAtAck::FAIL_SPELL;
			break;

		case CSpell::ENCHANT_FAIL_BY_RESIST :
			cDefenserJudge = ClientConstants::Judge_Resist;
			break;
	}
	return 0;
}

DEFINE_SKILL(SocialMotion)
{
	cDefenserJudge = ClientConstants::Judge_Social;
	return 0;
}

DEFINE_SKILL(Gather)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	cDefenserJudge = ClientConstants::Judge_Gather;
	CCharacter* lpCharacter = reinterpret_cast<CCharacter *>(lpSkillUser);
	SKILL &Skill = lpCharacter->m_DBData.m_Skill;
	char cSkillLockCount=-1;
	if(attackType.m_wType == 0x9E03)
	{
		for (int Index = 0; Index < lpCharacter->m_DBData.m_Skill.wSlotNum; ++Index)
        {
			if(lpCharacter->m_DBData.m_Skill.SSlot[Index].SKILLINFO.wSkill == 0x1011)
			{
				//cSkillLockCount = lpCharacter->m_DBData.m_Skill.SSlot[Index].SKILLINFO.cSkillLevel;
				cSkillLockCount = lpCharacter->m_DBData.m_Skill.SSlot[Index].SKILLINFO.cLockCount;
				break;
			}
        }
	}
	else if(attackType.m_wType == 0x9E04)
	{
		for (int Index = 0; Index < lpCharacter->m_DBData.m_Skill.wSlotNum; ++Index)
        {
			if(lpCharacter->m_DBData.m_Skill.SSlot[Index].SKILLINFO.wSkill == 0x1012)
			{
				//cSkillLockCount = lpCharacter->m_DBData.m_Skill.SSlot[Index].SKILLINFO.cSkillLevel;
				cSkillLockCount = lpCharacter->m_DBData.m_Skill.SSlot[Index].SKILLINFO.cLockCount;
				break;
			}
        }
	}
	const unsigned char cCreatureType = Creature::GetCreatureType(lpVictim->GetCID());//test if its a monster hayzohar
	if (Creature::CT_MONSTER == cCreatureType )
	{
		unsigned long dwMonID = lpVictim->GetCID() & 0x0000FFFF;
		
		bool bExt = false;
		const int CNT = 13;
		const unsigned short usSkillID[CNT] = { 0x9E00, 0x9E01, 0x9E02, 0x9E03, 0x9E04, 
													0x9E03, 0x9E04, 0x9E03, 0x9E04, 0x9E03, 
													0x9E04, 0x9E03, 0x9E04 };
		/*{ 0x9E00, 0x9E01, 0x9E02, 0x9E03, 0x9E04, 
													0x9E03, 0x9E04, 0x9E03, 0x9E04, 0x9E03, 
													0x9E04, 0x9E03, 0x9E04 };*/

		const unsigned short usMonID[CNT] = { 3100, 3150, 3200, 3225, 3250, 
												3275, 3300, 3325, 3350, 3375, 
												3400, 3425, 3450 };

		char cLock[CNT] = { 0, 0, 0, 0, 0,
									1, 1, 2, 2, 3, 
									3, 4, 4 };

		//const unsigned char cSkillLockCount = attackType.m_cSkillLockCount;

		/*if(0x9E03 == attackType.m_wType && (usMonID[0] < dwMonID && dwMonID <= usMonID[1]) )
		{
			return 1000*(cSkillLockCount+1);
		}
		if(0x9E04 == attackType.m_wType && (usMonID[1] < dwMonID && dwMonID <= usMonID[2]))
		{
			return 1000*(cSkillLockCount+1);
		}*///wasnt commented out hz
		for(int xx = 1; xx < CNT; ++xx)
		{
			if(usSkillID[xx] == attackType.m_wType &&
				(usMonID[xx-1] < dwMonID && dwMonID <= usMonID[xx]) &&
				(cSkillLockCount) >= cLock[xx] )//was without the +1
			{
				if((dwMonID > 3250) && ((lpCharacter->GetSkillLockCount(0x990C) == 2) ||(lpCharacter->GetSkillLockCount(0x990C) == 3)||(lpCharacter->GetSkillLockCount(0x990C) == 4)))
					return 0;

				return 1000*(cSkillLockCount+1);
			}
		}
	}

	return 0;
}


DEFINE_SKILL(FireCracker)
{
	cDefenserJudge = ClientConstants::Judge_FireCracker;
	return 0;
}


DEFINE_SKILL(ChinaFireCracker)
{
	cDefenserJudge = ClientConstants::Judge_ChinaFireCracker;
	return 0;
}


/*
DEFINE_SKILL(ChinaValentine)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	unsigned short wResult = CSpell::NO_ENCHANT_ERROR;
	switch (attackType.m_cSkillLockCount) 
	{
		case 0:		// 초콜렛
		{
			wResult = CAddSpell<CChocolateSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
				SpellType::MAGICAL_SPELL, SpellID::Chocolate, 1, 180))(lpVictim);
			break;
		}

		case 1:		// 술맛 초콜렛
		{
			wResult = CAddSpell<CTasteWineSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
				SpellType::MAGICAL_SPELL, SpellID::TasteWine, 1, 180))(lpVictim);
			break;		
		}

		case 2:		// 부인 과자
		{
			if (Creature::CT_PC == Creature::GetCreatureType(lpVictim->GetCID()))
			{
				CCharacter* lpCharacter = reinterpret_cast<CCharacter*>(lpVictim);
				lpCharacter->IncrementExp(1000000);
			}

			wResult = CAddSpell<CWifeCrackerSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
				SpellType::PHYSICAL_SPELL, SpellID::WifeCracker, 1, 1800))(lpVictim);
			break;		
		}

		case 3:		// 큐피트의 초콜렛
		{
			wResult = CAddSpell<CWifeCrackerSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
				SpellType::PHYSICAL_SPELL, SpellID::WifeCracker, 1, 300))(lpVictim);

			if (CSpell::NO_ENCHANT_ERROR == wResult || CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED == wResult)
			{
				wResult = CAddSpell<CEnchantWeaponSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
					SpellType::MAGICAL_SPELL, SpellID::EnchantWeapon, 135, 300))(lpVictim);
			}
			break;		
		}

		case 4:	
			break;
	}

	switch (wResult)
	{
		case CSpell::NO_ENCHANT_ERROR :
		case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
			cDefenserJudge = ClientConstants::Judge_Enchant;
			break;

		case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
			wError = PktAtAck::FAIL_SPELL;
			break;

		case CSpell::ENCHANT_FAIL_BY_RESIST :
			cDefenserJudge = ClientConstants::Judge_Resist;
			break;
	}

	return 0;
}


DEFINE_SKILL(Malysia1stAnniversary)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	unsigned short wAdditionalHP = 0;
	unsigned short wAdditionalMP = 0;

	switch (attackType.m_cSkillLockCount) 
	{
		case 0:	wAdditionalHP = 8888;	wAdditionalMP = 8888;	cDefenserJudge = ClientConstants::Judge_Heal;		break;		// 1주년 기념
		case 1:	wAdditionalHP = 0;		wAdditionalMP = 0;		cDefenserJudge = ClientConstants::Judge_Heal;		break;		// 미결
		case 2:	wAdditionalHP = 0;		wAdditionalMP = 0;		cDefenserJudge = ClientConstants::Judge_ManaHeal;	break;		// 미결
		case 3:	wAdditionalHP = 0;		wAdditionalMP = 0;		cDefenserJudge = ClientConstants::Judge_Heal;		break;		// 미결
		case 4:	wAdditionalHP = 0;		wAdditionalMP = 0;		cDefenserJudge = ClientConstants::Judge_Heal;		break;		// 미결
	}   	

	const unsigned short wOldMP = lpVictim->m_CreatureStatus.m_nNowMP;

	if (lpVictim->m_CreatureStatus.m_nNowHP > USHRT_MAX - wAdditionalHP)
	{
		lpVictim->m_CreatureStatus.m_nNowHP = lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxHP;
	}
	else
	{
		lpVictim->m_CreatureStatus.m_nNowHP += wAdditionalHP;
		lpVictim->m_CreatureStatus.m_nNowHP = 
			std::min(lpVictim->m_CreatureStatus.m_nNowHP, lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxHP);
	}

	if (lpVictim->m_CreatureStatus.m_nNowMP > USHRT_MAX - wAdditionalMP)
	{
		lpVictim->m_CreatureStatus.m_nNowMP = lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxMP;
	}
	else
	{
		lpVictim->m_CreatureStatus.m_nNowMP += wAdditionalMP;
		lpVictim->m_CreatureStatus.m_nNowMP = 
			std::min(lpVictim->m_CreatureStatus.m_nNowMP, lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxMP);
	}

	wDefenserMPHeal = (lpVictim->m_CreatureStatus.m_nNowMP > wOldMP) ? lpVictim->m_CreatureStatus.m_nNowMP - wOldMP : 0;

	return 0;
}


DEFINE_SKILL(RefreshmentPotion)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	const unsigned short wOldMP = lpVictim->m_CreatureStatus.m_nNowMP;
	unsigned short wAdditionalHP = 0, wAdditionalMP = 0;

	switch (attackType.m_cSkillLockCount)
	{
		case 0:	// Refresh Postion S
		case 1:	// Refresh Postion M
		case 2:	// Refresh Postion L
			wAdditionalHP = PotionHP7Value( (attackType.m_cSkillLockCount + 1) * 10 );
			wAdditionalMP = PotionMPValue( (attackType.m_cSkillLockCount + 1) * 5 );
			break;

		case 3:	// Refresh Postion G
			wAdditionalHP = 7000;
			wAdditionalMP = 3000;
			break;
	}

	if (lpVictim->m_CreatureStatus.m_nNowHP > USHRT_MAX - wAdditionalHP)
	{
		lpVictim->m_CreatureStatus.m_nNowHP = lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxHP;
	}
	else
	{
		lpVictim->m_CreatureStatus.m_nNowHP += wAdditionalHP;
		lpVictim->m_CreatureStatus.m_nNowHP = 
			std::min(lpVictim->m_CreatureStatus.m_nNowHP, lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxHP);
	}
	

	if (lpVictim->m_CreatureStatus.m_nNowMP > USHRT_MAX - wAdditionalMP)
	{
		lpVictim->m_CreatureStatus.m_nNowMP = lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxMP;
	}
	else
	{
		lpVictim->m_CreatureStatus.m_nNowMP += wAdditionalMP;
		lpVictim->m_CreatureStatus.m_nNowMP = 
			std::min(lpVictim->m_CreatureStatus.m_nNowMP, lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxMP);
	}

	cDefenserJudge = ClientConstants::Judge_Heal;
	wDefenserMPHeal = (lpVictim->m_CreatureStatus.m_nNowMP > wOldMP) ? lpVictim->m_CreatureStatus.m_nNowMP - wOldMP : 0;

	return 0;
}

DEFINE_SKILL(HealingPotion)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	const unsigned short wAdditionalHP = PotionHPValue( (attackType.m_cSkillLockCount + 1) * 20 );

	if (lpVictim->m_CreatureStatus.m_nNowHP > USHRT_MAX - wAdditionalHP)
	{
		lpVictim->m_CreatureStatus.m_nNowHP = lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxHP;
	}
	else
	{
		lpVictim->m_CreatureStatus.m_nNowHP += wAdditionalHP;
		lpVictim->m_CreatureStatus.m_nNowHP = 
			std::min(lpVictim->m_CreatureStatus.m_nNowHP, lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxHP);
	}

	cDefenserJudge = ClientConstants::Judge_Heal;
	return 0;
}

DEFINE_SKILL(SpellPotion)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

	lpVictim->GetSpellMgr().GetAffectedInfo().Disenchant(SpellType::MAGICAL_SPELL, SpellTarget::ALL_ENCHANT, 
		Disenchant::NONE, 0, Disenchant::INFINITE_NUM);

	unsigned short wResult = CAddSpell<CSpellPotionSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
		SpellType::MAGICAL_SPELL, SpellID::SpellPotion, (attackType.m_cSkillLockCount + 1), 180))(lpVictim);

	switch (wResult)
	{
		case CSpell::NO_ENCHANT_ERROR :
		case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
			cDefenserJudge = ClientConstants::Judge_Heal;
			break;

		case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
			wError = PktAtAck::FAIL_SPELL;
			break;

		case CSpell::ENCHANT_FAIL_BY_RESIST :
			cDefenserJudge = ClientConstants::Judge_Resist;
			break;
	}

	const unsigned short wOldMP = lpVictim->m_CreatureStatus.m_nNowMP;
	unsigned short wAdditionalHP = 0, wAdditionalMP = 0;

	switch (attackType.m_cSkillLockCount)
	{
		case 0:	// Spell Postion S
		case 1:	// Spell Postion M
		case 2:	// Spell Postion L
			wAdditionalHP = PotionHPValue( (attackType.m_cSkillLockCount + 1) * 10 );
			wAdditionalMP = PotionMPValue( (attackType.m_cSkillLockCount + 1) * 5 );
			break;

		case 3:	// Spell Postion G
			wAdditionalHP = 7000;
			wAdditionalMP = 3000;
			break;
	}

	if (lpVictim->m_CreatureStatus.m_nNowHP > USHRT_MAX - wAdditionalHP)
	{
		lpVictim->m_CreatureStatus.m_nNowHP = lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxHP;
	}
    else
	{
        lpVictim->m_CreatureStatus.m_nNowHP += wAdditionalHP;
		lpVictim->m_CreatureStatus.m_nNowHP = 
			std::min(lpVictim->m_CreatureStatus.m_nNowHP, lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxHP);
	}

	if (lpVictim->m_CreatureStatus.m_nNowMP > USHRT_MAX - wAdditionalMP)
	{
		lpVictim->m_CreatureStatus.m_nNowMP = lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxMP;
	}
	else
	{
		lpVictim->m_CreatureStatus.m_nNowMP += wAdditionalMP;
		lpVictim->m_CreatureStatus.m_nNowMP = 
			std::min(lpVictim->m_CreatureStatus.m_nNowMP, lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxMP);
	}

	cDefenserJudge = ClientConstants::Judge_Heal;
	wDefenserMPHeal = (lpVictim->m_CreatureStatus.m_nNowMP > wOldMP) ? lpVictim->m_CreatureStatus.m_nNowMP - wOldMP : 0;

	return 0;
}

DEFINE_SKILL(MoonCake)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	unsigned short nAdditionalHP = 1200;
	unsigned short nAdditionalMP = 500;

	const unsigned short wOldMP = lpVictim->m_CreatureStatus.m_nNowMP;

	if (lpVictim->m_CreatureStatus.m_nNowHP > USHRT_MAX - nAdditionalHP)
	{
		lpVictim->m_CreatureStatus.m_nNowHP = lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxHP;
	}
	else
	{
		lpVictim->m_CreatureStatus.m_nNowHP += nAdditionalHP;
		if (lpVictim->m_CreatureStatus.m_nNowHP > lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxHP) 
		{
			lpVictim->m_CreatureStatus.m_nNowHP = lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxHP;
		}
	}

	if (lpVictim->m_CreatureStatus.m_nNowMP > USHRT_MAX - nAdditionalMP)
	{
		lpVictim->m_CreatureStatus.m_nNowMP = lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxMP;
	}
	else
	{
		lpVictim->m_CreatureStatus.m_nNowMP += nAdditionalMP;
		if (lpVictim->m_CreatureStatus.m_nNowMP > lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxMP) 
		{
			lpVictim->m_CreatureStatus.m_nNowMP = lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxMP;
		}
	}

	cDefenserJudge = ClientConstants::Judge_Heal;
	wDefenserMPHeal = (lpVictim->m_CreatureStatus.m_nNowMP > wOldMP) ? lpVictim->m_CreatureStatus.m_nNowMP - wOldMP : 0;

	return 0;
}

DEFINE_SKILL(RedMoonCake)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	if (attackType.m_cSkillLockCount >= 3)
	{
		ERRLOG3(g_Log, "CID:0x%08x 존재하지 않는 음식을 먹으려 합니다. SkillType : 0x%04x, LockCount : %d", 
			lpSkillUser->GetCID(), attackType.m_wType, attackType.m_cSkillLockCount);
		return 0;
	}

	unsigned short wAdditionalHP = 0;

	switch (attackType.m_cSkillLockCount) 
	{
		case 0:	wAdditionalHP = 1500;	break;		// 빨간 월병(소)
		case 1:	wAdditionalHP = 3000;	break;		// 빨간 월병(중)
		case 2:	wAdditionalHP = 5000;	break;		// 빨간 월병(대)
		case 3:	wAdditionalHP = 0;		break;		// X
		case 4:	wAdditionalHP = 0;		break;		// X
	}   	

	if (lpVictim->m_CreatureStatus.m_nNowHP > USHRT_MAX - wAdditionalHP)
	{
		lpVictim->m_CreatureStatus.m_nNowHP = lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxHP;
	}
	else
	{
		lpVictim->m_CreatureStatus.m_nNowHP += wAdditionalHP;
		lpVictim->m_CreatureStatus.m_nNowHP = 
			std::min(lpVictim->m_CreatureStatus.m_nNowHP, lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxHP);
	}

	cDefenserJudge = ClientConstants::Judge_Heal;
	return 0;
}

DEFINE_SKILL(BlueMoonCake)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	if (attackType.m_cSkillLockCount >= 3)
	{
		ERRLOG3(g_Log, "CID:0x%08x 존재하지 않는 음식을 먹으려 합니다. SkillType : 0x%04x, LockCount : %d", 
			lpSkillUser->GetCID(), attackType.m_wType, attackType.m_cSkillLockCount);
		return 0;
	}

	unsigned short wAdditionalMP = 0;

	switch (attackType.m_cSkillLockCount) 
	{
		case 0:	wAdditionalMP = 1000;	break;		// 파란 월병(소)
		case 1:	wAdditionalMP = 2000;	break;		// 파란 월병(중)
		case 2:	wAdditionalMP = 3000;	break;		// 파란 월병(대)
		case 3:	wAdditionalMP = 0;		break;		// X
		case 4:	wAdditionalMP = 0;		break;		// X
	}   	

	const unsigned short wOldMP = lpVictim->m_CreatureStatus.m_nNowMP;

	if (lpVictim->m_CreatureStatus.m_nNowMP > USHRT_MAX - wAdditionalMP)
	{
		lpVictim->m_CreatureStatus.m_nNowMP = lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxMP;
	}
	else
	{
		lpVictim->m_CreatureStatus.m_nNowMP += wAdditionalMP;
		lpVictim->m_CreatureStatus.m_nNowMP = 
			std::min(lpVictim->m_CreatureStatus.m_nNowMP, lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxMP);
	}

	cDefenserJudge = ClientConstants::Judge_ManaHeal;
	wDefenserMPHeal = (lpVictim->m_CreatureStatus.m_nNowMP > wOldMP) ? lpVictim->m_CreatureStatus.m_nNowMP - wOldMP : 0;

	return 0;
}
*/

DEFINE_SKILL(Drunk)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	 
	 


	unsigned short wSpellLevel = (attackType.m_cSkillLockCount+1);

	// HP
	const unsigned short wAdditionalHP = wSpellLevel * 500;

	if (lpVictim->m_CreatureStatus.m_nNowHP > USHRT_MAX - wAdditionalHP)
	{
		lpVictim->m_CreatureStatus.m_nNowHP = lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxHP;
	}
	else
	{
		lpVictim->m_CreatureStatus.m_nNowHP += wAdditionalHP;
		lpVictim->m_CreatureStatus.m_nNowHP = 
			std::min(lpVictim->m_CreatureStatus.m_nNowHP, lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxHP);
	}

	// MP
	const unsigned short wAdditionalMP = wSpellLevel * 500;
	const unsigned short wOldMP = lpVictim->m_CreatureStatus.m_nNowMP;

	if (lpVictim->m_CreatureStatus.m_nNowMP > USHRT_MAX - wAdditionalMP)
	{
		lpVictim->m_CreatureStatus.m_nNowMP = lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxMP;
	}
	else
	{
		lpVictim->m_CreatureStatus.m_nNowMP += wAdditionalMP;
		lpVictim->m_CreatureStatus.m_nNowMP = 
			std::min(lpVictim->m_CreatureStatus.m_nNowMP, lpVictim->m_CreatureStatus.m_StatusInfo.m_nMaxMP);
	}

	wDefenserMPHeal = (lpVictim->m_CreatureStatus.m_nNowMP > wOldMP) ? lpVictim->m_CreatureStatus.m_nNowMP - wOldMP : 0;

	unsigned short wResult = CAddSpell<CBuffPotionSpell>(CSpell::Spell_Info(ProtoType, NULL, 
//	unsigned short wResult = CAddSpell<CBuffPotionSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
		SpellType::BUFF_SPELL, SpellID::Drunk, wSpellLevel, 30))(lpVictim);
	
	switch (wResult)
	{
		case CSpell::NO_ENCHANT_ERROR :
			cDefenserJudge = ClientConstants::Judge_Heal;
			break;

		case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
			cDefenserJudge = ClientConstants::Judge_Enchant;
			break;

		case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
			wError = PktAtAck::FAIL_SPELL;
			break;

		case CSpell::ENCHANT_FAIL_BY_RESIST :
			cDefenserJudge = ClientConstants::Judge_Resist;
			break;
	}

	return 0;

}

DEFINE_SKILL(MichaelBless)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	 
	 


	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

	unsigned long dwAdditionalTime = 0;

	switch (attackType.m_cSkillLockCount) 
	{
		// 300 5분 초다. 60 * 분 해서 나온값.
		case 0:	dwAdditionalTime = 86400;	break;			// 미하엘의 가호(1시간) //was 0
		case 1:	dwAdditionalTime = 21600;	break;			// 미하엘의 가호(6시간)
		case 2:	dwAdditionalTime = 86400;	break;			// 미하엘의 가호(1일)//was 2
		case 3:	dwAdditionalTime = 604800;	break;			// 미하엘의 가호(7일)
		case 4:	dwAdditionalTime = 2592000;	break;			// 미하엘의 가호(1달)
	}   	

	// CMichaelBlessSpell	
	unsigned short wResult = CAddSpell<CMichaelBlessSpell>(CSpell::Spell_Info(ProtoType, NULL, 
//	unsigned short wResult = CAddSpell<CMichaelBlessSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
		SpellType::PAYBUFF_SPELL, SpellID::MichaelBless, 1, dwAdditionalTime))(lpVictim);

	switch (wResult)
	{
		case CSpell::NO_ENCHANT_ERROR :
		case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
			cDefenserJudge = ClientConstants::Judge_Enchant;
			break;

		case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
			wError = PktAtAck::FAIL_SPELL;
			break;

		case CSpell::ENCHANT_FAIL_BY_RESIST :
			cDefenserJudge = ClientConstants::Judge_Resist;
			break;
	}

	lpSkillUser->SaveSpell();

	return 0;
}


DEFINE_SKILL(LuckyExpOrb)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	 
	 


	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }
/*
	// 제한없앴음
	if (attackType.m_cSkillLockCount >= 4)
	{
		ERRLOG3(g_Log, "CID:0x%08x 존재하지 않는 경험의 오브을 먹으려 합니다. SkillType : 0x%04x, LockCount : %d", 
			lpSkillUser->GetCID(), attackType.m_wType, attackType.m_cSkillLockCount);
		return 0;
	}
*/
	unsigned long dwAdditionalTime = 0;

	switch (attackType.m_cSkillLockCount) 
	{
		// 300 5분 초다. 60 * 분 해서 나온값.
		case 0:	dwAdditionalTime = 3600;	break;			// 경험의 오브(1시간) 
		case 1:	dwAdditionalTime = 21600;	break;			// 경험의 오브(6시간)
		case 2:	dwAdditionalTime = 86400;	break;			// 경험의 오브(1일)
		case 3:	dwAdditionalTime = 604800;	break;			// 경험의 오브(7일)
		case 4:	dwAdditionalTime = 2592000;	break;			// 경험의 오브(1달)
	}   	

	//CExpOrbSpell
	unsigned short wResult = CAddSpell<CExpOrbSpell>(CSpell::Spell_Info(ProtoType, NULL, 
//	unsigned short wResult = CAddSpell<CExpOrbSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
		SpellType::PAYBUFF_SPELL, SpellID::ExpOrb, 10, dwAdditionalTime))(lpVictim);

	//CLuckyOrbSpell
	wResult = CAddSpell<CLuckyOrbSpell>(CSpell::Spell_Info(ProtoType, NULL, 
//	wResult = CAddSpell<CLuckyOrbSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
		SpellType::PAYBUFF_SPELL, SpellID::LuckyOrb, 10, dwAdditionalTime))(lpVictim);

	switch (wResult)
	{
		case CSpell::NO_ENCHANT_ERROR :
		case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
			cDefenserJudge = ClientConstants::Judge_Enchant;
			break;

		case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
			wError = PktAtAck::FAIL_SPELL;
			break;

		case CSpell::ENCHANT_FAIL_BY_RESIST :
			cDefenserJudge = ClientConstants::Judge_Resist;
			break;
	}

	lpSkillUser->SaveSpell();

	return 0;
}


DEFINE_SKILL(ExpOrb)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	 
	 


	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }
/*
	// 제한없앴음
	if (attackType.m_cSkillLockCount >= 4)
	{
		ERRLOG3(g_Log, "CID:0x%08x 존재하지 않는 경험의 오브을 먹으려 합니다. SkillType : 0x%04x, LockCount : %d", 
			lpSkillUser->GetCID(), attackType.m_wType, attackType.m_cSkillLockCount);
		return 0;
	}
*/
	unsigned long dwAdditionalTime = 0;

	switch (attackType.m_cSkillLockCount) 
	{
		// 300 5분 초다. 60 * 분 해서 나온값.
		case 0:	dwAdditionalTime = 3600;	break;			// 경험의 오브(1시간) 
		case 1:	dwAdditionalTime = 21600;	break;			// 경험의 오브(6시간)
		case 2:	dwAdditionalTime = 86400;	break;			// 경험의 오브(1일)
		case 3:	dwAdditionalTime = 604800;	break;			// 경험의 오브(7일)
		case 4:	dwAdditionalTime = 2592000;	break;			// 경험의 오브(1달)
	}   	

	// CExpOrbSpell	
	unsigned short wResult = CAddSpell<CExpOrbSpell>(CSpell::Spell_Info(ProtoType, NULL, 
//	unsigned short wResult = CAddSpell<CExpOrbSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
		SpellType::PAYBUFF_SPELL, SpellID::ExpOrb, 10, dwAdditionalTime))(lpVictim);

	switch (wResult)
	{
		case CSpell::NO_ENCHANT_ERROR :
		case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
			cDefenserJudge = ClientConstants::Judge_Enchant;
			break;

		case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
			wError = PktAtAck::FAIL_SPELL;
			break;

		case CSpell::ENCHANT_FAIL_BY_RESIST :
			cDefenserJudge = ClientConstants::Judge_Resist;
			break;
	}

	lpSkillUser->SaveSpell();

	return 0;
}

DEFINE_SKILL(LuckyOrb)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	 
	 


	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

	unsigned long dwAdditionalTime = 0;

	switch (attackType.m_cSkillLockCount) 
	{
		// 300 5분 초다. 60 * 분 해서 나온값.
		case 0:	dwAdditionalTime = 3600;	break;			// 행운의 오브(1시간) 
		case 1:	dwAdditionalTime = 21600;	break;			// 행운의 오브(6시간)
		case 2:	dwAdditionalTime = 86400;	break;			// 행운의 오브(1일)
		case 3:	dwAdditionalTime = 604800;	break;			// 행운의 오브(7일)
		case 4:	dwAdditionalTime = 2592000;	break;			// 행운의 오브(1달)
	}   	

	//CLuckyOrbSpell
	unsigned short wResult = CAddSpell<CLuckyOrbSpell>(CSpell::Spell_Info(ProtoType, NULL, 
//	unsigned short wResult = CAddSpell<CLuckyOrbSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
		SpellType::PAYBUFF_SPELL, SpellID::LuckyOrb, 1, dwAdditionalTime))(lpVictim);

	switch (wResult)
	{
		case CSpell::NO_ENCHANT_ERROR :
		case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
			cDefenserJudge = ClientConstants::Judge_Enchant;
			break;

		case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
			wError = PktAtAck::FAIL_SPELL;
			break;

		case CSpell::ENCHANT_FAIL_BY_RESIST :
			cDefenserJudge = ClientConstants::Judge_Resist;
			break;
	}

	lpSkillUser->SaveSpell();

	return 0;
}


DEFINE_SKILL(AntidotePotion)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	 
	 


	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

	if (true == lpSkillUser->GetSpellMgr().GetAffectedInfo().RemoveEnchantBySpellType(SpellID::Poisoned))
		cDefenserJudge = ClientConstants::Judge_Disenchant;
	else 
		wError = PktAtAck::FAIL_SPELL;

	return 0;
}


DEFINE_SKILL(AntiFreeze)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	 
	 


	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

	if (true == lpSkillUser->GetSpellMgr().GetAffectedInfo().RemoveEnchantBySpellType(SpellID::Frozen))
		cDefenserJudge = ClientConstants::Judge_Disenchant;
	else 
		wError = PktAtAck::FAIL_SPELL;

	return 0;
}


DEFINE_SKILL(AntiFire)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	 
	 


	if (-1 == ConsumeMP(attackType, lpSkillUser, cOffencerJudge)) { return 0; }

	if (true == lpSkillUser->GetSpellMgr().GetAffectedInfo().RemoveEnchantBySpellType(SpellID::Fired))
		cDefenserJudge = ClientConstants::Judge_Disenchant;
	else 
		wError = PktAtAck::FAIL_SPELL;

	return 0;
}

//-------------------------------------------------------------------------------------------------
// 공성 병기 스킬

DEFINE_SKILL(SRSAAttack)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	// 10 % 랜덤값 적용(+,-)

	unsigned short wDamage = static_cast<unsigned short>(10800 + Math::Random::ComplexRandom(2400));

	return wDamage;
}


DEFINE_SKILL(LRSAAttack)
{
	return 0;
}


DEFINE_SKILL(SRCAAttack)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	// 10 % 랜덤값 적용(+,-)
	unsigned short wDamage = static_cast<unsigned short>(3600 + Math::Random::ComplexRandom(800));

	return wDamage;
}


DEFINE_SKILL(LRCAAttack)
{
	return 0;
}


DEFINE_SKILL(GuardAttack)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	const unsigned short wDamage = 500;
	lpVictim->GetThreat().AffectThreat(lpSkillUser, wDamage, CThreat::DETAUNT);
	return wDamage;
}


// 랏과 동일
DEFINE_SKILL(EmblemNewAttack)
{	
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);
	
	char successRate=GetStickyMagicSuccessRate(lpSkillUser,lpVictim,0.5f);

	if (successRate > Math::Random::ComplexRandom(100))
	{
		unsigned short wResult = CAddSpell<CPoisonedSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
			SpellType::MAGICAL_SPELL, SpellID::Poisoned, 283, 12))(lpVictim);

		switch (wResult)
		{
		case CSpell::NO_ENCHANT_ERROR :
		case CSpell::ENCHNAT_FAIL_ALREADY_AFFECTED :
			break;

		case CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT :
			wError = PktAtAck::FAIL_SPELL;
			break;

		case CSpell::ENCHANT_FAIL_BY_RESIST :
			cDefenserJudge = ClientConstants::Judge_Resist;
			break;
		}
	}
	else
	{
		cDefenserJudge = ClientConstants::Judge_Resist;
	}

	return 4000;
}

// 크레비스와 동일한 공격
DEFINE_SKILL(EmblemAttack)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);
	return InstanceMagicDamage(lpSkillUser, lpVictim, attackType, 40);
}


DEFINE_SKILL(CampAttack)
{
	CHECK_SKILL_ARGUMENT_IS_NULL(lpSkillUser, lpVictim);

	return static_cast<unsigned short>((150 + 100 * (attackType.m_cSkillLevel + attackType.m_cSkillLockCount * 6)) * 
		(100 - lpVictim->GetStatus().m_StatusInfo.m_fMagicResistRate) / 100.0f);
}

