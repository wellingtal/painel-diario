#ifndef _CAGGRESIVE_CREATURE_H_
#define _CAGGRESIVE_CREATURE_H_

#include <Thread/Lock.h>
#include <Network/Packet/PacketStruct/CharAttackPacketStruct.h>
#include <Creature/Character/CharacterStructure.h>

#include <Utility/Time/Pulse/Pulse.h>
#include <Utility/Setup/ServerSetup.h>

#include <Skill/SkillStructure.h>
#include <Skill/Spell/SpellMgr.h>

#include "Threat.h"
#include "Creature.h"


// 전방 참조
class CParty;
struct TakeType;
namespace Skill 
{ 
	class CFunctions; 
}

// Creature가 있는 Cell에 대한정보 (셀 좌표와 셀을 가리키는 포인터)
struct CellPosition
{	
	CCell*			m_lpCell;	

	unsigned short	m_wMapIndex;

	unsigned char	m_cCellX;		
	unsigned char	m_cCellZ;

	unsigned short	m_wInX;
	unsigned short	m_wInZ;

	CellPosition();
	CellPosition(const Position& WorldPos);
	bool MoveTo(const Position& WorldPos);
};

class CAggresiveCreature : public CCreature
{
public:
    
    enum Const 
	{ 
		REGEN_TIME			= 50,	// HP/MP 회복 주기
		MAX_LEVEL_GAP		= 20,	// 최대 레벨갭
		AGGRAVATION_NUM		= 41,	// 레벨갭에 의한 가중치 범위 (-20 ~ 20)
		MAX_CASTE			= 10	// 최대 계급
	};

	CAggresiveCreature(unsigned long dwCID);
	virtual ~CAggresiveCreature();

	// ------------------------------------------------------------------------
	// 위치 관련

	const CellPosition& GetCellPos(void) const		{ return m_CellPos;						}
	void SetMapIndex(unsigned short wMapIndex)		{ m_CellPos.m_wMapIndex = wMapIndex;	}
	const unsigned short GetMapIndex(void) const	{ return m_CellPos.m_wMapIndex;			}

	float CalcDir2D(const float fSrcX, const float fSrcY, const float fDstX, const float fDstY);

	// 이동 (크리쳐있을때)
	MoveInfo MoveTo(const Position& NewPosition, bool bSitDown);	// 앉아 있으면 bSitMode가 true

	// ------------------------------------------------------------------------
	// 챈트/인챈트 관련
	void CalculateEnchantStatus(void)	
	{ 
		m_EquipStatus.CalculateEnchantInfo(m_aryEnchantLevel, m_CreatureStatus.m_StatusInfo);	
	}

	void SetEnchantLevel(Skill::SpellID::Type eSpellType, unsigned short wLevel)	
	{ 
		m_aryEnchantLevel[eSpellType] = wLevel;					
	}

	unsigned short GetEnchantLevel(Skill::SpellID::Type eSpellType)
	{
		return m_aryEnchantLevel[eSpellType];
	}

	void GetEnchantLevel(unsigned short* wEnchantLevel)								
	{ 
		std::copy(&(m_aryEnchantLevel[0]), &(m_aryEnchantLevel[Skill::SpellID::MAX_SPELL_TYPE]), wEnchantLevel);
	}

	// ------------------------------------------------------------------------
	// 레벨갭 관련

	virtual const int CalculateFixLevelGap(CAggresiveCreature *pDefender) = 0;
	const int CalculateLevelGap(CAggresiveCreature *pDefender);
	const int CalculateLevelGap(int nOffencerLevel, int nDefenderLevel);
	const float CalculateLevelGapAffect(CAggresiveCreature *pDefender);

	// ------------------------------------------------------------------------
	// 전투 관련
	//hz added for safe zone attack fix
	bool IsSafeZones( CAggresiveCreature* lpSkillUser , CAggresiveCreature* lpVictim );
	//hz end
	bool MultiAttack(AtType attackType, unsigned char nDefenderNum, CAggresiveCreature** ppDefenders, unsigned char* cDefenserJudges,
					Position CenterPos, float fDir, float nRange, float fAngle, char cTargetType);

	unsigned short MeleeAttack(CAggresiveCreature* lpTarget, Creature::StatusType eHandType, 
		const float fLevelGap, unsigned char &cDefenserJudge, unsigned int ExType = 0);

	virtual unsigned short ApplyDamage(AtType attackType, CAggresiveCreature* pOffencer, unsigned char &cOffencerJudge,
		unsigned char &cDefenserJudge, unsigned short& wOffencerMPHeal, unsigned short& wDefenserMPHeal, unsigned short &wError);

	virtual bool CalculateEquipDurability(unsigned short wAttackType) { return true; }

	virtual bool Attack(AtType attackType, unsigned char nDefenderNum, CAggresiveCreature** pDefenders, unsigned char* cDefenserJudges, unsigned short* wDefenserMPHeal) = 0;	
	virtual bool AttackUsingBow(unsigned short wType) { return true; }

    virtual bool RegenHPAndMP(unsigned short usAdditionalHP, unsigned short usAdditionalMP, bool bAddDefaultRegenValue);
	bool CAggresiveCreature::GuardAura(unsigned short usAdditionalBlock, unsigned short usAdditionalDefense, bool bAddDefaultRegenValue);

	inline virtual bool Dead(CAggresiveCreature* pOffencer);
	 
	// ------------------------------------------------------------------------
	// 기타 Get/Set 함수

	void SetParty(CParty* pParty)			{ m_pParty = pParty;		}
	CParty* GetParty(void)					{ return m_pParty;			}

	CharacterStatus& GetCharStatus(void)	{ return m_CharacterStatus;	}
	CreatureStatus& GetStatus(void)			{ return m_CreatureStatus;	}

	CThreat& GetThreat(void)				{ return m_Threat;			}

	virtual const FightStatus& GetEtcTypeStatus(Creature::StatusType eType)								{ return m_EquipStatus;		}
	virtual bool HasSkill(unsigned short usSkillType, unsigned char cLockCount, unsigned char cLevel)	{ return true;				}
	virtual short GetSkillLockCount(unsigned short usSkillType)											{ return 0;					}

	virtual char GetConsumeMPCount(void)	{ return 0;					}
	virtual void ConsumeMPCount(void)		{							}
    
	virtual unsigned long GetUID(void) const			{ return 0;		}
	virtual const unsigned long GetFame(void)			{ return 0;		}
	virtual char GetEliteBonus(void)					{ return 0;		}
	virtual unsigned short GetClass(void)				{ return 0;		}
	virtual CAggresiveCreature* GetDuelOpponent(void)	{ return NULL;	}
    virtual bool IsPeaceMode(void)						{ return false; }
	virtual bool IsRideArms(void) const					{ return false; }
	virtual unsigned long GetGID(void) const			{ return 0;		}
	virtual unsigned short GetObjectType() const		{ return 0;		}

	virtual EnemyCheck::EnemyType IsEnemy(CCreature* lpTarget, unsigned char* cResult = NULL) = 0;
	virtual unsigned char GetNation(void) const = 0;

	virtual void SaveSpell(BOOL bDead = FALSE)			{ return ;					}

	inline CSpellMgr& GetSpellMgr(void) 	{ return m_SpellMgr;		}
    bool IsLogout(void) const				{ return m_bLogout;			}

	virtual bool ItemDump(char* pBuffer, int* nBufferSize_InOut) const	{ return true;	}
	virtual Item::CItem* SellToCharacter(CCharacter *lpCustomer, unsigned short wKindItem, TakeType takeType, 
		Item::CItem* lpRequestItem, unsigned long &dwPrice, unsigned short wCouponID, unsigned short &usError) = 0;

	virtual unsigned char GetRealmPoint(void) = 0;

	friend class Skill::CFunctions;

protected:
	
	unsigned long	m_dwLastTime;		// 마지막 체크 시간(사망/부활)

	CParty*			m_pParty;			// 파티 정보를 가리키는 포인터
	CThreat			m_Threat;			// 스레트 리스트

	CharacterStatus	m_CharacterStatus;	// 캐릭터가 갖는 STR, INT, DEX 등의 능력치	
	CreatureStatus	m_CreatureStatus;	// 실제 전투 등 수식에서 사용되는 능력치

	FightStatus		m_EquipStatus;		// 기본 스탯, 장비 효과까지 적용된 능력치
	
	CSpellMgr		m_SpellMgr;
	unsigned short	m_aryEnchantLevel[Skill::SpellID::MAX_SPELL_TYPE];

	CellPosition	m_CellPos;			// 셀 위치

	bool	        m_bLogout;
	bool	        m_bSitDown;
    bool            m_bPadding[2];

	unsigned short CalculateDamage(const FightStatus& OffencerStatusInfo, const float fLevelGap, unsigned char &cDefenserJudge, unsigned int ExType = 0);
};


inline bool CAggresiveCreature::Dead(CAggresiveCreature* pOffencer)
{
	m_dwLastTime = CPulse::GetInstance().GetLastTick();

	//	Enchant는 전부 없애버린다. 
	m_SpellMgr.GetAffectedInfo().Disenchant(Skill::SpellType::NONE,
		Skill::SpellTarget::ALL_ENCHANT, Skill::Disenchant::NONE, 1, Skill::Disenchant::INFINITE_NUM);
	m_SpellMgr.GetAffectedInfo().Disenchant(Skill::SpellType::ETERNAL_SPELL,
		Skill::SpellTarget::ALL_ENCHANT, Skill::Disenchant::NONE, 1, Skill::Disenchant::INFINITE_NUM);
	m_SpellMgr.GetCastingInfo().ClearEnchant();

	//	Chant는, 내가 건 스킬만 Disable한다. ( 나에게 걸린건 굳이 Disable할 필요 없다. )
	m_SpellMgr.GetCastingInfo().ClearChant();

	return true;
}


#endif