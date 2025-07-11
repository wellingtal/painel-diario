#ifndef _SKILL_TABLE_H_
#define _SKILL_TABLE_H_

#pragma once
//hz
#include <Creature/Character/Character.h>

#include <Pattern/Singleton.h>
#include <Creature/AggresiveCreature.h>

#include <Utility/Math/Math.h>
#include <Utility/TypeArray.h>

#include <MMSystem.h>
#include <Skill/Spell/Spell.h>
#include <Skill/Spell/SpellUtil.h>
#include <Skill/Spell/SpellTable.h>
#include <Skill/Spell/GlobalSpellMgr.h>

//hz added for auto kick
#include <Network\Dispatch\GameClient\GameClientDispatch.h>
//hz end
#include "SkillMgr.h"



namespace Skill 
{
	enum PassiveType 
	{
		ActivatePassive		= 1, 
		DeactivatePassive	= 2 
	};

	enum MasteryType
	{
		NONE_MASTERY	= 0,
		SWORD_MASTERY	= 1,
		AXE_MASTERY		= 2,
		BLUNT_MASTERY	= 3,
		DAGGER_MASTERY	= 4,
		CRUSH_WEAPON	= 5,
		BLADE			= 6,
		CLAW_MASTERY	= 7,
		WAND_MASTERY	= 8
	};

	typedef unsigned short (*FnProcess)(const Skill::ProtoType& ProtoType, AtType attackType, 
		CAggresiveCreature* lpSkillUser, CAggresiveCreature* lpVictim, unsigned char &cOffencerJudge,
		unsigned char &cDefenserJudge, unsigned short& wOffencerMPHeal, unsigned short& wDefenserMPHeal, unsigned short &wError);

	class CProcessTable : public CSingleton<CProcessTable>
    {
    public:

		struct ProcessInfo
        {
	        unsigned short		    m_usSkill_ID;
            Skill::FnProcess	    m_fnProcess;
            Skill::ProtoType*	    m_lpProtoType;
			static Skill::ProtoType m_NullProtoType;

			ProcessInfo() : m_usSkill_ID(0), m_fnProcess(NULL), m_lpProtoType(&m_NullProtoType) { }
            ProcessInfo(unsigned short usSkill_ID, Skill::FnProcess fnProcess, Skill::ProtoType* lpProtoType)
		        :	m_usSkill_ID(usSkill_ID), m_fnProcess(fnProcess), m_lpProtoType(lpProtoType)
            { }

            inline bool operator < (const ProcessInfo& rhs) const { return (m_usSkill_ID < rhs.m_usSkill_ID); }
        };

        typedef std::vector<ProcessInfo>    ProcessVector;

        bool Initialize();
		bool InsertSkill(Skill::CProcessTable::ProcessVector& processVector, unsigned short usSkill_ID, FnProcess fnProcess);
        const ProcessInfo*  GetProcessInfo(unsigned short usSkill_ID);

		inline unsigned short UseSkill(AtType attackType, CAggresiveCreature* lpSkillUser, CAggresiveCreature* lpVictim,
									   unsigned char &cOffencerJudge, unsigned char &cDefenserJudge, unsigned short& wOffencerMPHeal,
									   unsigned short& wDefenserMPHeal,	unsigned short &wError)
		{
			const ProcessInfo* lpProcessInfo = GetProcessInfo(attackType.m_wType);

			if (NULL == lpProcessInfo)
				return 0;

			char cLockCount = CSkillMgr::MAX_SKILL_LOCKCOUNT;
			if(lpProcessInfo->m_lpProtoType->m_eSkillType == Skill::Type::ITEM)
				cLockCount = 7;

			if (attackType.m_cSkillLockCount >= cLockCount || 
				attackType.m_cSkillLevel > CSkillMgr::MAX_SKILL_LEVEL)
			{
				unsigned long dwCID = (NULL != lpSkillUser) ? lpSkillUser->GetCID() : 0;
				ERRLOG3(g_Log, "CID:0x%08x 잘못된 락카운트/레벨의 스킬을 사용하려 합니다. LockCount:%d, Level:%d", 
					dwCID, attackType.m_cSkillLockCount, attackType.m_cSkillLevel);
			}
			else
			{
				return lpProcessInfo->m_fnProcess(lpProcessInfo->m_lpProtoType[attackType.m_cSkillLockCount], attackType, 
					lpSkillUser, lpVictim, cOffencerJudge, cDefenserJudge, wOffencerMPHeal, wDefenserMPHeal, wError);
			}

			return 0;
		}

    private:

        CProcessTable();
	    ~CProcessTable();

	    unsigned short 	m_usProcessInfo;
	    ProcessInfo*    m_fnProcessTable;

        static CProcessTable ms_this;
    };

	class CFunctions 
    {
	public:

		static short ConsumeMP(AtType attackType, CAggresiveCreature* lpSkillUser, unsigned char cOffencerJudge);	


	protected:	// 함수 테이블을 통해서만 호출할 수 있도록 한다.

		enum Const 
		{
			VarCastMeeleEnchantLevelMultiplier = 4
		};

		friend class CProcessTable;

		// 패시브 스킬		: 리턴값 - 일단은 0;
		// 인스턴스 스킬	: 리턴값 - Victim에게 입힌 대미지
		// 캐스트 스킬		: 리턴값 - Victim에게 입힌 대미지
		
		static inline bool CheckChantOff(unsigned char cSkillLockCount, unsigned char cSkillLevel);
		static inline bool SlowlySkillAttack(const Skill::ProtoType& ProtoType, AtType attackType, 
			CAggresiveCreature* lpSkillUser, CAggresiveCreature* lpVictim);

		// ------------------------------------------------------------------------------------------------------------
		// 기획 측에서 사용하는 용어/수식을 구현한 함수들

		// 락카운트
		static inline const unsigned short GetSkillLockCount(CAggresiveCreature* lpSkillUser, AtType attackType);

		// 엘리트, 레벨갭
		static inline const float LevelFactor(CAggresiveCreature* lpSkillUser, CAggresiveCreature* lpVictim);

		// 성공 확률
		static inline const unsigned short SuccessHitRate(CAggresiveCreature* lpSkillUser, CAggresiveCreature* lpVictim, float fFactor);
		//hz
		static inline const unsigned short SuccessCripplingRate(CAggresiveCreature* lpSkillUser, float CripplingLevel);
		
		// 성공 확률
		static inline const unsigned short SuccessRate(CAggresiveCreature* lpSkillUser, CAggresiveCreature* lpVictim, float fFactor);

		// 인스턴스 마법 데미지
		static inline const unsigned short InstanceMagicDamage(CAggresiveCreature* lpSkillUser, CAggresiveCreature* lpVictim, 
			AtType attackType, unsigned char cFactor);

		// 인스턴스 적대 인챈트
		static inline const unsigned long InstanceEnemyEnchantTick(CAggresiveCreature* lpSkillUser, CAggresiveCreature* lpVictim, 
			AtType attackType, float fFactor);

		// 인스턴스 마법 회복
		static inline const unsigned short InstanceMagicRegen(CAggresiveCreature* lpSkillUser, AtType attackType, unsigned char cFactor);

		// 캐스트 마법 데미지
		static inline const unsigned short CastMagicDamage(CAggresiveCreature* lpSkillUser, CAggresiveCreature* lpVictim, 
			AtType attackType, unsigned char cFactor);

		// 그리스 인챈트
		static inline const unsigned long CastEnchantTick(CAggresiveCreature* lpSkillUser, CAggresiveCreature* lpVictim, 
			AtType attackType, float fFactor);

		// 캐스트 인챈트
		static inline const unsigned short CastEnchantLevel(CAggresiveCreature* lpSkillUser, CAggresiveCreature* lpVictim, 
			AtType attackType, float fFactor);

		// 밀리 케스트 인챈트
		static inline const unsigned short CastMeelsEnchantLevel(CAggresiveCreature* lpSkillUser, CAggresiveCreature* lpVictim, 
			AtType attackType, float fFactor);

		// 캐스트 마법 회복
		static inline const unsigned short CastMagicRegen(CAggresiveCreature* lpSkillUser, AtType attackType, unsigned char cFactor);

		// 챈트 공식
		static inline const unsigned short ChantLevel(CAggresiveCreature* lpSkillUser, AtType attackType, unsigned char cFactor);
		// 회복류 아이템의 HP, MP 절대값 리턴 함수
		static inline const unsigned short PotionHPValue(unsigned char cPercent);
		static inline const unsigned short PotionMPValue(unsigned char cPercent);
		//hz
		static inline const float GetMelodyFactor(CAggresiveCreature* lpSkillUser);
		static inline const float GetCripplingFactor(CAggresiveCreature* lpSkillUser);
		static inline const char GetAbilityLevel(CAggresiveCreature* lpSkillUser, unsigned short SkillID);
		static inline bool CheckLastUseTime(CCharacter* lpCharacter, unsigned short SkillID,unsigned long cooldownTime);
		static inline bool CheckLastUseTimeClassSkill(CCharacter* lpCharacter, unsigned short SkillID, unsigned long cooldownTime);
		static inline const float GetAbilityRatio(char abilityLevel, float FirstRatio, float RatioDifference);
		static inline const char GetStickyMagicSuccessRate(CAggresiveCreature* lpSkillUser, CAggresiveCreature* lpVictim, float factor);
		static inline const char GetStunMasterySuccessRate(CAggresiveCreature* lpSkillUser, CAggresiveCreature* lpVictim, float factor);

		// ------------------------------------------------------------------------------------------------------------
		// 3차 밸런스 패치 관련 함수
		// 인스턴스 밀리 회복
		static inline const unsigned short InstanceMeleeRegen(CAggresiveCreature* lpSkillUser, AtType attackType, unsigned char cFactor);

		// 캐스트 밀리 회복
		static inline const unsigned short CastMeleeRegen(CAggresiveCreature* lpSkillUser, AtType attackType, unsigned char cFactor);

		// 밀리 챈트 공식
		static inline const unsigned short MeleeChantLevel(CAggresiveCreature* lpSkillUser, AtType attackType, unsigned char cFactor);

		// 캐스트 인챈트 스킬 레벨
		static inline const unsigned short CastEnchantSkillLevel(CAggresiveCreature* lpSkillUser, CAggresiveCreature* lpVictim,
			AtType attackType, float fFactor1, float fFactor2 = 1.0f, float fFactor3 = 0.0f);
		

		// ------------------------------------------------------------------------------------------------------------

		#define DECLARE_SKILL(SpellName) \
		    static unsigned short SpellName(const Skill::ProtoType& ProtoType, AtType attackType, CAggresiveCreature* lpSkillUser, \
				CAggresiveCreature* lpVictim, unsigned char &cOffencerJudge, unsigned char &cDefenserJudge, \
				unsigned short& wOffencerMPHeal, unsigned short& wDefenserMPHeal, unsigned short &wError)

		#define DEFINE_SKILL(SpellName) \
			unsigned short Skill::CFunctions::##SpellName(const Skill::ProtoType& ProtoType, AtType attackType, \
                CAggresiveCreature* lpSkillUser, CAggresiveCreature* lpVictim, \
                unsigned char &cOffencerJudge, unsigned char &cDefenserJudge, \
				unsigned short& wOffencerMPHeal, unsigned short& wDefenserMPHeal, unsigned short &wError)


		// 소설모션
		DECLARE_SKILL(SocialMotion);

		// 파이터
		DECLARE_SKILL(Net);

		DECLARE_SKILL(SwordMastery);
		DECLARE_SKILL(AxeMastery);	
		DECLARE_SKILL(HardHit);	
		DECLARE_SKILL(BluntMastery);
		DECLARE_SKILL(WillPower);
		DECLARE_SKILL(Toughness);
		//hayzohar added wand mastery skill
		DECLARE_SKILL(WandMastery);
		//hayzohar end
		// 로그
		DECLARE_SKILL(Detection);	
		DECLARE_SKILL(ImprovedCritical);
		DECLARE_SKILL(Evasion);
		DECLARE_SKILL(NeedleSpit);	
		
		// 메이지
		DECLARE_SKILL(BloodyMana);	
		DECLARE_SKILL(VampiricTouch);
		DECLARE_SKILL(ManaShell);
		DECLARE_SKILL(Grease);
		DECLARE_SKILL(Serenity);
		
		// 어콜라이트
		DECLARE_SKILL(Purification);

		DECLARE_SKILL(Faith);
		DECLARE_SKILL(Encourage);	
		DECLARE_SKILL(FirstAid);	
		DECLARE_SKILL(HammerOfLight);
		DECLARE_SKILL(GuardianAngel);
		DECLARE_SKILL(Judgment);

		// 디펜더
		DECLARE_SKILL(Charging);

		DECLARE_SKILL(SharedPain);	
		DECLARE_SKILL(FullSwing);
		DECLARE_SKILL(Taunt);
		
		// 워리어
		DECLARE_SKILL(Blaze);

		DECLARE_SKILL(BattleSong);
		DECLARE_SKILL(ChainAction);
		DECLARE_SKILL(DualWieldMastery);
		DECLARE_SKILL(CounterAttack);
		DECLARE_SKILL(CounterAttackMotion);
		DECLARE_SKILL(DemonicCry);
		
		// 어쌔신
		DECLARE_SKILL(BackStab);	
		DECLARE_SKILL(Stealth);	
		DECLARE_SKILL(DaggerMastery);	
		
		DECLARE_SKILL(BombSet);
		DECLARE_SKILL(Explosion);

		DECLARE_SKILL(StealHand);

		// 아처
		DECLARE_SKILL(AimedShot);	
		DECLARE_SKILL(DualShot);	
		DECLARE_SKILL(Camouflage);	
		DECLARE_SKILL(Accuracy);
		DECLARE_SKILL(AgilityAura);
		
		// 소서러
		DECLARE_SKILL(Recall);

		DECLARE_SKILL(FireBolt);	
		DECLARE_SKILL(LightningArrow);	
		DECLARE_SKILL(FrostBolt);	
		DECLARE_SKILL(DeathRay);	
		DECLARE_SKILL(FreshToStone);	

		// 엔찬터
		DECLARE_SKILL(Disenchant);	

		DECLARE_SKILL(Entangle);	
		DECLARE_SKILL(LowerStrength);	
		DECLARE_SKILL(EnchantWeapon);	
		DECLARE_SKILL(Shatter);	

		// 프리스트
		DECLARE_SKILL(Resurrection);

		DECLARE_SKILL(MaintenanceChant);	
		DECLARE_SKILL(AccelerationChant);	
		DECLARE_SKILL(BrightArmor);	
		DECLARE_SKILL(ManaFlow);	
		DECLARE_SKILL(ManaBurn);	

		// 클레릭
		DECLARE_SKILL(Dazzle);	
		DECLARE_SKILL(CureWounds);	
		DECLARE_SKILL(WoundsCrafting);
		DECLARE_SKILL(CureLight);
		DECLARE_SKILL(WoundsMake);
		DECLARE_SKILL(Regeneration);


		// 컴배턴트
		DECLARE_SKILL(CrusherMastery);
		DECLARE_SKILL(BladeMastery);		
		DECLARE_SKILL(SplitLife);	
		DECLARE_SKILL(ExtraLife);

		// 오피세이터
		DECLARE_SKILL(Dispel);

		DECLARE_SKILL(MagicMissile);
		DECLARE_SKILL(Flexibility);
		DECLARE_SKILL(ClawMastery);
		DECLARE_SKILL(LifeAura);
		DECLARE_SKILL(GuardAura);
		DECLARE_SKILL(InnerSight);

		// 템플러
		DECLARE_SKILL(Guard);
		DECLARE_SKILL(HardenSkin);
		DECLARE_SKILL(FastHit);
		DECLARE_SKILL(ManaConvert);

		// 어태커
		DECLARE_SKILL(PowerDrain);
		DECLARE_SKILL(SkillArmFight);
		DECLARE_SKILL(RingGeyser);

		// 거너
		DECLARE_SKILL(FireRing);
		DECLARE_SKILL(Blast);

		// 룬오프
		DECLARE_SKILL(Rot);
		DECLARE_SKILL(Shock);
		DECLARE_SKILL(Shackle);
		DECLARE_SKILL(Crevice);
		DECLARE_SKILL(SummonKindling);
		DECLARE_SKILL(Clairvoyance);

		// 라이프오프
		DECLARE_SKILL(Flash);

		// 쉐도우오프
		DECLARE_SKILL(WoundsBlast);
		DECLARE_SKILL(DaggerFire);
		DECLARE_SKILL(Envenom);


		// 회복 아이템
		DECLARE_SKILL(UseFood);	
//		DECLARE_SKILL(UseFoodPer);	
		DECLARE_SKILL(UseDrink);	
//		DECLARE_SKILL(UseDrinkPer);	
		DECLARE_SKILL(UseFusionFood);	
//		DECLARE_SKILL(UseFusionFoodPer);	

//		DECLARE_SKILL(UseHealPotion);	
		DECLARE_SKILL(UseHealPotionPer);	
//		DECLARE_SKILL(UseManaPotion);	
		DECLARE_SKILL(UseManaPotionPer);	
//		DECLARE_SKILL(UseRefreshPotion);	
		DECLARE_SKILL(UseRefreshPotionPer);

		// 버프포션
//		DECLARE_SKILL(DemagePotion);
		DECLARE_SKILL(DemagePotionPer);

//		DECLARE_SKILL(ArmorPotion);
		DECLARE_SKILL(ArmorPotionPer);
	
//		DECLARE_SKILL(HitRatePotion);
		DECLARE_SKILL(HitRatePotionPer);

//		DECLARE_SKILL(EvadePotion);
		DECLARE_SKILL(EvadePotionPer);

//		DECLARE_SKILL(MaxHPPotion);
		DECLARE_SKILL(MaxHPPotionPer);

//		DECLARE_SKILL(MaxMPPotion);
		DECLARE_SKILL(MaxMPPotionPer);

//		DECLARE_SKILL(HPRegenPotion);
//		DECLARE_SKILL(HPRegenPotionPer);
				
//		DECLARE_SKILL(MPRegenPotion);
//		DECLARE_SKILL(MPRegenPotionPer);

//		DECLARE_SKILL(CriticalPotion);
		DECLARE_SKILL(CriticalPotionPer);

//		DECLARE_SKILL(BlockPotion);
		DECLARE_SKILL(BlockPotionPer);

//		DECLARE_SKILL(SpeedPotion);
		DECLARE_SKILL(SpeedPotionPer);

//		DECLARE_SKILL(DeCoolDownPotion);
		DECLARE_SKILL(DeCoolDownPotionPer);

//		DECLARE_SKILL(MagicPowerPotion);
		DECLARE_SKILL(MagicPowerPotionPer);

//		DECLARE_SKILL(MagicResistPotion);
		DECLARE_SKILL(MagicResistPotionPer);

		DECLARE_SKILL(ConcentrationPotionPer);

		DECLARE_SKILL(FireCracker);
		DECLARE_SKILL(ChinaFireCracker);	

		DECLARE_SKILL(Drunk);	
		DECLARE_SKILL(LuckyExpOrb);

		DECLARE_SKILL(MichaelBless);

		DECLARE_SKILL(ExpOrb);
		DECLARE_SKILL(LuckyOrb);

		DECLARE_SKILL(AntidotePotion);
		DECLARE_SKILL(AntiFreeze);
		DECLARE_SKILL(AntiFire);

		DECLARE_SKILL(Gather);
		DECLARE_SKILL(TrainedRider);
		DECLARE_SKILL(MasterThief);
		DECLARE_SKILL(Explorer);
		DECLARE_SKILL(IronStomach);
		//Permisions
		DECLARE_SKILL(Chemical);
		DECLARE_SKILL(Miner);
		DECLARE_SKILL(HerbCollector);
		DECLARE_SKILL(JewelryCrafter);
		DECLARE_SKILL(Refiner);
		DECLARE_SKILL(Botanist);
		DECLARE_SKILL(GemCrafter);
		DECLARE_SKILL(Cook);
		//Normal Abilities
		DECLARE_SKILL(BardsMelody);
		DECLARE_SKILL(CripplingStrike);
		DECLARE_SKILL(TitansStrenght);
		DECLARE_SKILL(MagicStrenght);
		DECLARE_SKILL(StickyMagic);
		DECLARE_SKILL(Swiftness);
		DECLARE_SKILL(Celerity);
		DECLARE_SKILL(ArmorBreaker);
		DECLARE_SKILL(MagicStrike);
		DECLARE_SKILL(IceBreaker);
		DECLARE_SKILL(PoisonBreaker);
		DECLARE_SKILL(StunBreaker);
		DECLARE_SKILL(Scout);
		DECLARE_SKILL(BuffMastery);
		DECLARE_SKILL(IronSkin);
		DECLARE_SKILL(WeaponBreaker);
		DECLARE_SKILL(StunMastery);

		// 공성 오브젝트 스킬
		DECLARE_SKILL(SRSAAttack);
		DECLARE_SKILL(LRSAAttack);
		DECLARE_SKILL(SRCAAttack);
		DECLARE_SKILL(LRCAAttack);
		DECLARE_SKILL(GuardAttack);
		DECLARE_SKILL(EmblemAttack);
		DECLARE_SKILL(CampAttack);

		DECLARE_SKILL(EmblemNewAttack);
	};
	
	// 스텔스 시킬인가?
	static inline bool IsStealthSkill(unsigned short wSkillID);
};
inline const char Skill::CFunctions::GetStickyMagicSuccessRate(CAggresiveCreature* lpSkillUser, CAggresiveCreature* lpVictim, float factor)
{
	char abilitylevel=GetAbilityLevel(lpSkillUser,0x1058);
	unsigned short successRate=SuccessRate(lpSkillUser, lpVictim, factor);
	switch(abilitylevel)
	{
	case 1:
		successRate*=1.05;
		break;

	case 2:
		successRate*=1.10;
		break;

	case 3:
		successRate*=1.15;
		break;

	default:
		break;
	}

	return successRate;

}

inline const char Skill::CFunctions::GetStunMasterySuccessRate(CAggresiveCreature* lpSkillUser, CAggresiveCreature* lpVictim, float factor)
{
	char abilitylevel=GetAbilityLevel(lpSkillUser,0x1064);
	unsigned short successRate=SuccessRate(lpSkillUser, lpVictim, factor);
	switch(abilitylevel)
	{
	case 1:
		successRate*=1.10;
		break;

	case 2:
		successRate*=1.20;
		break;

	case 3:
		successRate*=1.30;
		break;

	default:
		break;
	}

	return successRate;

}

inline const float Skill::CFunctions::GetAbilityRatio(char abilityLevel, float FirstRatio, float RatioDifference)
{
	float finalRatio=1;

	if(abilityLevel == 0)
		return finalRatio;
	
	switch(abilityLevel)
	{
	case 1:
		finalRatio=FirstRatio;
		break;

	case 2:
		finalRatio=(FirstRatio + RatioDifference);
		break;

	case 3:
		finalRatio=(FirstRatio + 2*RatioDifference);
		break;

	default:
		break;

	}

	return finalRatio;
}
inline const char Skill::CFunctions::GetAbilityLevel(CAggresiveCreature* lpSkillUser, unsigned short SkillID)
{
	CCharacter* lpCharacter = reinterpret_cast<CCharacter *>(lpSkillUser);
	SKILL &Skill = lpCharacter->m_DBData.m_Skill;
	float AbilityLevel=0;
		for (int Index = 0; Index < lpCharacter->m_DBData.m_Skill.wSlotNum; ++Index)
        {
			if(lpCharacter->m_DBData.m_Skill.SSlot[Index].SKILLINFO.wSkill == SkillID)
			{
				return (lpCharacter->m_DBData.m_Skill.SSlot[Index].SKILLINFO.cLockCount+1);
			}
		}

		return 0;
}

inline bool Skill::CFunctions::CheckLastUseTimeClassSkill(CCharacter* lpCharacter, unsigned short SkillID, unsigned long cooldownTime)
{

	int index=lpCharacter->GetPositionOfSkillInBigArr(SkillID);
	if(index == 130)
		return false;

	if(cooldownTime > 2000)
		cooldownTime -= 1000;

	if ((timeGetTime() - lpCharacter->LastUseSkillTime[index])  >= cooldownTime ){
		lpCharacter->LastUseSkillTime[index]=timeGetTime();
		return false;
		}
	else
		{
			CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
				if (NULL != lpDispatch)
				{
					lpDispatch->Disconnect();
					lpDispatch->Disconnected();
				}
			return true;
		}	
}


inline bool Skill::CFunctions::CheckLastUseTime(CCharacter* lpCharacter, unsigned short SkillID, unsigned long cooldownTime)
{
	if(cooldownTime > 2000)
		cooldownTime -= 1000;

	if((SkillID >= 0x8D01) && (SkillID <= 0x8D16))
	{
		int index=lpCharacter->GetPositionOfSkillInArr(SkillID);
		if(index == 12)
			return false;

		if(lpCharacter->LastUsePotionTime[index] == 0)
		{
			lpCharacter->LastUsePotionTime[index]=timeGetTime();
			return false;
		}
		
		long different=timeGetTime() - lpCharacter->LastUsePotionTime[index];

		if(different < cooldownTime)
		{
			CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
				if (NULL != lpDispatch)
				{
					lpDispatch->Disconnect();
					lpDispatch->Disconnected();
				}
			return true;
		}
	}
	else
	{
	SKILL &Skill = lpCharacter->m_DBData.m_Skill;

	bool found=false;
	int index=lpCharacter->GetPositionOfSkillInBigArr(SkillID);
	if(index == 130)
		return false;

	found=true;
	if ((timeGetTime() - lpCharacter->LastUseSkillTime[index])  >= cooldownTime ){
		lpCharacter->LastUseSkillTime[index]=timeGetTime();
		return false;
	}
	else
	{
		CGameClientDispatch* lpDispatch = lpCharacter->GetDispatcher();
		if (NULL != lpDispatch)
		{
			lpDispatch->Disconnect();
			lpDispatch->Disconnected();
		}
		return true;
	}

	return false;
	}

}

inline const float Skill::CFunctions::LevelFactor(CAggresiveCreature* lpSkillUser, CAggresiveCreature* lpVictim) 
{
	return lpSkillUser->CalculateLevelGapAffect(lpVictim);
}

inline const unsigned short Skill::CFunctions::SuccessHitRate(CAggresiveCreature* lpSkillUser, CAggresiveCreature* lpVictim, float fFactor)
{
	return static_cast<unsigned short>((lpSkillUser->GetStatus().m_StatusInfo.m_wHitRate * fFactor / 
		(lpSkillUser->GetStatus().m_StatusInfo.m_wHitRate * fFactor + lpVictim->GetStatus().m_StatusInfo.m_wEvade)) * 100);
}

inline const unsigned short Skill::CFunctions::SuccessCripplingRate(CAggresiveCreature* lpSkillUser, float CripplingLevel)
{
	if(lpSkillUser->GetClass() == CClass::Archer)
		return static_cast<unsigned short>(CripplingLevel * 10) ;
	else 
		return static_cast<unsigned short>(CripplingLevel * 20) ;
}

inline const unsigned short Skill::CFunctions::SuccessRate(CAggresiveCreature* lpSkillUser, CAggresiveCreature* lpVictim, float fFactor)
{
	return static_cast<unsigned short>((lpSkillUser->GetStatus().m_StatusInfo.m_wMagicPower * fFactor / 
		(lpSkillUser->GetStatus().m_StatusInfo.m_wMagicPower * fFactor + lpVictim->GetStatus().m_StatusInfo.m_fMagicResistRate)) * 100);
}

inline const unsigned short Skill::CFunctions::InstanceMagicDamage(CAggresiveCreature* lpSkillUser, CAggresiveCreature* lpVictim, 
																   AtType attackType, unsigned char cFactor)
{
	long lDamage = static_cast<long>((attackType.m_cSkillLevel + attackType.m_cSkillLockCount * 6) * cFactor * 
		(75 + Math::Random::ComplexRandom(75) + lpSkillUser->GetStatus().m_StatusInfo.m_wMagicPower) / 100.0f * 
		(100 - lpVictim->GetStatus().m_StatusInfo.m_fMagicResistRate) / 100.0f * LevelFactor(lpSkillUser, lpVictim));

	return static_cast<unsigned short>(min(lDamage, USHRT_MAX));
}

inline const unsigned long Skill::CFunctions::InstanceEnemyEnchantTick(CAggresiveCreature* lpSkillUser, CAggresiveCreature* lpVictim, 
																		AtType attackType, float fFactor)
{
	return static_cast<unsigned long>((attackType.m_cSkillLevel + attackType.m_cSkillLockCount * 6) * 
		fFactor * LevelFactor(lpSkillUser, lpVictim));
}

inline const unsigned short Skill::CFunctions::InstanceMagicRegen(CAggresiveCreature* lpSkillUser, AtType attackType, unsigned char cFactor)
{
	return static_cast<unsigned short>((attackType.m_cSkillLevel + attackType.m_cSkillLockCount * 6) * 
		cFactor * (50 + Math::Random::ComplexRandom(50) + lpSkillUser->GetStatus().m_StatusInfo.m_wMagicPower) / 100.0f);
}

inline const unsigned short Skill::CFunctions::CastMagicDamage(CAggresiveCreature* lpSkillUser, CAggresiveCreature* lpVictim, 
															   AtType attackType, unsigned char cFactor)
{
	const unsigned short wLockCount = GetSkillLockCount(lpSkillUser, attackType);

	long lDamage = static_cast<long>(((wLockCount + 1) * attackType.m_cSkillLevel + 1 + attackType.m_cSkillLockCount * wLockCount * 5) *
		cFactor * (75 + Math::Random::ComplexRandom(75) + lpSkillUser->GetStatus().m_StatusInfo.m_wMagicPower) / 100.0f * 
		(100 - lpVictim->GetStatus().m_StatusInfo.m_fMagicResistRate) / 100.0f * LevelFactor(lpSkillUser, lpVictim));

	lDamage=static_cast<long>(lDamage * 0.6f);
	return static_cast<unsigned short>(min(lDamage, USHRT_MAX));
}

inline const unsigned long Skill::CFunctions::CastEnchantTick(CAggresiveCreature* lpSkillUser, CAggresiveCreature* lpVictim, 
															   AtType attackType, float fFactor)
{
	return static_cast<unsigned long>((GetSkillLockCount(lpSkillUser, attackType) + attackType.m_cSkillLevel + attackType.m_cSkillLockCount * 2) * 
		fFactor * LevelFactor(lpSkillUser, lpVictim));
}

inline const unsigned short Skill::CFunctions::CastEnchantLevel(CAggresiveCreature* lpSkillUser, CAggresiveCreature* lpVictim, 
																AtType attackType, float fFactor)
{
	unsigned short wFirstCase = static_cast<unsigned short>(((GetSkillLockCount(lpSkillUser, attackType) + 1) * 
		attackType.m_cSkillLevel + attackType.m_cSkillLockCount * GetSkillLockCount(lpSkillUser, attackType)) * fFactor * 
		(100 + lpSkillUser->GetStatus().m_StatusInfo.m_wMagicPower) / 100.0f);

	unsigned short wSecondCase = static_cast<unsigned short>(lpVictim->GetStatus().m_nLevel * 3 * fFactor);

	return min(wFirstCase, wSecondCase);
}

inline const unsigned short Skill::CFunctions::CastMeelsEnchantLevel(CAggresiveCreature* lpSkillUser, CAggresiveCreature* lpVictim, 
														 AtType attackType, float fFactor)
{

	unsigned short wFirstCase = static_cast<unsigned short>(((GetSkillLockCount(lpSkillUser, attackType) + 1) * 
		attackType.m_cSkillLevel + attackType.m_cSkillLockCount * 
		GetSkillLockCount(lpSkillUser, attackType)) * fFactor * 
		(VarCastMeeleEnchantLevelMultiplier + GetSkillLockCount(lpSkillUser, attackType)));
	
	unsigned short wSecondCase = static_cast<unsigned short>(lpVictim->GetStatus().m_nLevel * 3 * fFactor);

	return min(wFirstCase, wSecondCase);	
}


inline const unsigned short Skill::CFunctions::CastMagicRegen(CAggresiveCreature* lpSkillUser, AtType attackType, unsigned char cFactor)
{
	return static_cast<unsigned short>(((GetSkillLockCount(lpSkillUser, attackType) + 1) * 
		attackType.m_cSkillLevel + 1 + attackType.m_cSkillLockCount * GetSkillLockCount(lpSkillUser, attackType) * 5) * cFactor * 
		(50 + Math::Random::ComplexRandom(50) + lpSkillUser->GetStatus().m_StatusInfo.m_wMagicPower) / 100.0f);
}

inline const unsigned short Skill::CFunctions::PotionHPValue(unsigned char cPercent)
{
	switch (cPercent)
	{
		case 10:	return  1000;
		case 20:	return  3000;
		case 30:	return  5000;
		case 40:	return  6000;
		case 50:	return  7000;
		case 60:	return  8000;
		case 80:	return 10000;
	}

	return 0;
}

inline const unsigned short Skill::CFunctions::PotionMPValue(unsigned char cPercent)
{
	switch (cPercent)
	{
		case  5:	return  500;
		case 10:	return 1000;
		case 15:	return 2000;
		case 20:	return 3000;
		case 30:	return 4000;
		case 45:	return 6000;
		case 60:	return 8000;
	}

	return 0;
}

inline const float Skill::CFunctions::GetMelodyFactor(CAggresiveCreature* lpSkillUser)
{
	CCharacter* lpCharacter = reinterpret_cast<CCharacter *>(lpSkillUser);
	SKILL &Skill = lpCharacter->m_DBData.m_Skill;
	float MelodyFactor=1;
		for (int Index = 0; Index < lpCharacter->m_DBData.m_Skill.wSlotNum; ++Index)
        {
			if(lpCharacter->m_DBData.m_Skill.SSlot[Index].SKILLINFO.wSkill == 0x1055)
			{
				switch(lpCharacter->m_DBData.m_Skill.SSlot[Index].SKILLINFO.cLockCount+1)
				{
				case 1:
					MelodyFactor = 1.05;
					break;

				case 2:
					MelodyFactor = 1.10;
					break;

				case 3:
					MelodyFactor = 1.15;
					break;

				default:
					break;
				}
				break;
			}
        }

		return MelodyFactor;
}

inline const float Skill::CFunctions::GetCripplingFactor(CAggresiveCreature* lpSkillUser)
{
	CCharacter* lpCharacter = reinterpret_cast<CCharacter *>(lpSkillUser);
	SKILL &Skill = lpCharacter->m_DBData.m_Skill;
	float CripplingFactor=1;
		for (int Index = 0; Index < lpCharacter->m_DBData.m_Skill.wSlotNum; ++Index)
        {
			if(lpCharacter->m_DBData.m_Skill.SSlot[Index].SKILLINFO.wSkill == 0x1056)
			{
				switch(lpCharacter->m_DBData.m_Skill.SSlot[Index].SKILLINFO.cLockCount+1)
				{
				case 1:
					CripplingFactor = 1.05;
					break;

				case 2:
					CripplingFactor = 1.10;
					break;

				case 3:
					CripplingFactor = 1.15;
					break;

				default:
					break;
				}
				break;
			}
        }

		return CripplingFactor;
}

inline const unsigned short Skill::CFunctions::ChantLevel(CAggresiveCreature* lpSkillUser, AtType attackType, unsigned char cFactor)
{
	float MelodyFactor=GetMelodyFactor(lpSkillUser);

	return static_cast<unsigned short>((attackType.m_cSkillLevel + attackType.m_cSkillLockCount	* 6) * cFactor * MelodyFactor *
		(100 + lpSkillUser->GetStatus().m_StatusInfo.m_wMagicPower) / 100.0f);
}

//check what is the lvl of the skill?
inline const unsigned short Skill::CFunctions::GetSkillLockCount(CAggresiveCreature* lpSkillUser, AtType attackType)
{
	unsigned short wLockCount = 0;
	Creature::CreatureType eCreatureType = Creature::GetCreatureType(lpSkillUser->GetCID());
	

	if (Creature::CT_MONSTER == eCreatureType ||
		Creature::CT_SUMMON == eCreatureType ||
		Creature::CT_STRUCT == eCreatureType)
	{
		wLockCount = attackType.m_cSkillLockCount;
	}
	else
	{
		wLockCount = lpSkillUser->GetSkillLockCount(attackType.m_wType);
	}

	if (wLockCount < 0 || wLockCount >= CSkillMgr::MAX_SKILL_LOCKCOUNT)
	{
		ERRLOG3(g_Log, "CID:0x%08x 쓰려는 스킬의 락카운트가 이상합니다. SkillType : 0x%04x, LockCount : %d", 
			lpSkillUser->GetCID(), attackType.m_wType, wLockCount);
		return 0;
	}

	return wLockCount;
}

inline bool Skill::CFunctions::CheckChantOff(unsigned char cSkillLockCount, unsigned char cSkillLevel)
{
    return (0 == cSkillLockCount && 0 == cSkillLevel) ? true : false;
}

//check if the skill worked??
inline bool Skill::CFunctions::SlowlySkillAttack(const Skill::ProtoType& ProtoType, AtType attackType, 
												 CAggresiveCreature* lpSkillUser, CAggresiveCreature* lpVictim)
{
	unsigned short wLockCount = GetSkillLockCount(lpSkillUser, attackType);

	unsigned short wEnchantLevel = attackType.m_cSkillLevel + wLockCount * 6;
	unsigned long dwDurationSec = static_cast<unsigned long>(
		(attackType.m_cSkillLevel + wLockCount * 6) * 0.5f * LevelFactor(lpSkillUser, lpVictim));

	unsigned short wResult = CAddSpell<CSlowSpell>(CSpell::Spell_Info(ProtoType, lpSkillUser, 
		SpellType::MAGICAL_SPELL, SpellID::Slow, wEnchantLevel, dwDurationSec))(lpVictim);

	return (wResult == CSpell::ENCHANT_FAIL_BY_ENEMY_ENCHANT) ? false : true;
}

//true=stealth skill
inline bool Skill::IsStealthSkill(unsigned short wSkillID)
{
	switch (wSkillID)
	{
		case 0x8704:	// 스텔스 (어쌔)
		case 0x9804:	// 스텔스 (쉐옵)
		case 0x8805:	// 캐모플라쥐 (아처)
		case 0x9504:	// 캐모플라쥐 (거너)
			return true;
	}

	return false;
}

// ------------------------------------------------------------------------------------------------------------
// 3차 밸런스 패치 관련 함수

inline const unsigned short Skill::CFunctions::InstanceMeleeRegen(CAggresiveCreature* lpSkillUser, AtType attackType, unsigned char cFactor)
{
	return static_cast<unsigned short>((attackType.m_cSkillLevel + attackType.m_cSkillLockCount * 6) * 
		cFactor * (50 + Math::Random::ComplexRandom(50) + (attackType.m_cSkillLevel + attackType.m_cSkillLockCount * 6) * 25) / 100.0f);
}

inline const unsigned short Skill::CFunctions::CastMeleeRegen(CAggresiveCreature* lpSkillUser, AtType attackType, unsigned char cFactor)
{
	return static_cast<unsigned short>(((GetSkillLockCount(lpSkillUser, attackType) + 1) * 
		attackType.m_cSkillLevel + 1 + attackType.m_cSkillLockCount * GetSkillLockCount(lpSkillUser, attackType) * 5) * cFactor * 
		(50 + Math::Random::ComplexRandom(50) + GetSkillLockCount(lpSkillUser, attackType) * 100) / 100.0f);
}

inline const unsigned short Skill::CFunctions::MeleeChantLevel(CAggresiveCreature* lpSkillUser, AtType attackType, unsigned char cFactor)
{
	float MelodyFactor=GetMelodyFactor(lpSkillUser);

	return static_cast<unsigned short>((attackType.m_cSkillLevel + attackType.m_cSkillLockCount	* 6) * cFactor * MelodyFactor *
		(100 + (attackType.m_cSkillLevel + attackType.m_cSkillLockCount	* 6) * 31) / 100.0f);
}

inline const unsigned short Skill::CFunctions::CastEnchantSkillLevel(CAggresiveCreature* lpSkillUser, CAggresiveCreature* lpVictim,
														 AtType attackType, float fFactor1, float fFactor2, float fFactor3)
{
	unsigned short wFirstCase = static_cast<unsigned short>((((GetSkillLockCount(lpSkillUser, attackType) + 1) * 
		attackType.m_cSkillLevel + attackType.m_cSkillLockCount * GetSkillLockCount(lpSkillUser, attackType)) * fFactor1 * 
		(GetSkillLockCount(lpSkillUser, attackType) + 4) * fFactor2) + fFactor3);

	unsigned short wSecondCase = static_cast<unsigned short>(lpVictim->GetStatus().m_nLevel * 3 * fFactor1);

	return min(wFirstCase, wSecondCase);
}

#endif