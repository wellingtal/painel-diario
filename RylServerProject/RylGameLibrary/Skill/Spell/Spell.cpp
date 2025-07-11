
#include "stdafx.h"

#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/WrapPacket.h>
#include <Network/Packet/PacketStruct/CharAttackPacket.h>

#include <Map/FieldMap/Cell.h>
#include <Creature/AggresiveCreature.h>
#include <Creature/Monster/Monster.h>

#include "../SkillMgr.h"
#include "SpellMgr.h"
#include "GlobalSpellMgr.h"
#include "Spell.h"


#define IS_SET(dwFlag, dwValue) ((dwValue) == ((dwFlag) & (dwValue)))

#define SEND_SPELL_INFO(dwOperateFlag, lpAffected, cSpellType, nEnchantLevel, dwEnchantTime, bOnOff) \
    if(!IS_SET(dwOperateFlag, DO_NOT_SEND)) { SendSpellInfo(lpAffected, cSpellType, nEnchantLevel, dwEnchantTime, bOnOff); }


CSpell::CSpell(Spell_Info& spell_Info, Skill::Type::SkillType eSpellType)
:	m_pNextSpell(NULL), m_ProtoType(spell_Info.m_SkillProtoType), m_pCaster(NULL), m_eSkillType(eSpellType), 
    m_wSpellID(spell_Info.m_wSpellID), m_wSpellLevel(spell_Info.m_wSpellLevel), 
    m_cSpellType(spell_Info.m_cSpellType), m_dwDurationSec(spell_Info.m_dwDurationSec),
    m_dwActivateStatus(0), m_cAffectedNum(0), m_cInternalFlags(0), 
	m_cSkillLockCount(spell_Info.m_cSkillLockCount), m_cSkillLevel(spell_Info.m_cSkillLevel)
{
	SetCaster(spell_Info.m_lpCaster);
}


void CSpell::SetCaster(CAggresiveCreature* pCaster)
{
	if (NULL != m_pCaster)
	{
		// edith 2009.05.15 스킬 삭제로그 덤프
//		SERLOG1(g_SkillLog, "CSpell::SetCaster %d", m_pCaster->GetCID());

        m_pCaster->GetSpellMgr().GetCastingInfo().Remove(this);	
	}

	m_pCaster = pCaster;

	if (NULL != m_pCaster)
	{
		// edith 2009.05.15 스킬 삭제로그 덤프
//		SERLOG1(g_SkillLog, "CSpell::SetCaster %d", m_pCaster->GetCID());

		m_pCaster->GetSpellMgr().GetCastingInfo().Add(this);
	}
}


//	Caster, Affected정보를 전부 제거.
void CSpell::ClearAll(void)
{
	if (NULL != m_pCaster)
	{
		// edith 2009.05.15 스킬 삭제로그 덤프
//		SERLOG1(g_SkillLog, "CSpell::ClearAll %d", m_pCaster->GetCID());

		m_pCaster->GetSpellMgr().GetCastingInfo().Remove(this);
		m_pCaster = NULL;
	}

    ClearAffected();
}


void CSpell::Destroy(void)
{
	if (DESTROYED != (DESTROYED & m_cInternalFlags))
	{		
		ClearAll();
		m_cInternalFlags |= DESTROYED;
	}
}


bool CSpell::Activate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->GetEnchantInfo().SetFlag(static_cast<unsigned char>(m_wSpellID));
	SEND_SPELL_INFO(dwOperateFlag, lpAffected, static_cast<unsigned char>(m_wSpellID), m_wSpellLevel, m_dwDurationSec, true);
	return true;
}


bool CSpell::Deactivate(CAggresiveCreature* lpAffected, unsigned long dwOperateFlag)
{
	lpAffected->GetEnchantInfo().ResetFlag(static_cast<unsigned char>(m_wSpellID));
	SEND_SPELL_INFO(dwOperateFlag, lpAffected, static_cast<unsigned char>(m_wSpellID), m_wSpellLevel, 0, false);
	return true;
}


// Affected관련
bool CSpell::AddAffected(CAggresiveCreature* lpAffected, unsigned short& wError)
{
	if (NULL == lpAffected || m_cAffectedNum >= MAX_AFFECTED)
	{
		return false;
	}

	CAggresiveCreature**	ppBegin		= m_pAffected;
	CAggresiveCreature**	ppPastEnd	= m_pAffected + m_cAffectedNum;
    
	// 이미 있는 캐릭터를 또 넣으려는지 여부 검사.
	for (;ppBegin != ppPastEnd; ++ppBegin)
	{
		if ((*ppBegin) == lpAffected)
		{
			return false;
		}
	}

	m_pAffected[m_cAffectedNum] = lpAffected;

	// edith 2009.05.15 스킬 삭제로그 덤프
//	SERLOG1(g_SkillLog, "CSpell::AddAffected %d", lpAffected->GetCID());

	// 상호참조 설정. 설정에 실패하면 넣지 않는다.
    if (true == lpAffected->GetSpellMgr().GetAffectedInfo().Add(this, wError))
	{
		if (true == IsEnabled())
		{
			if (m_eSkillType == Skill::Type::CHANT)
			{
				CheckRange();
			}
			else
			{
				if (true == Activate(lpAffected, 0))
				{
					SetActivate(m_cAffectedNum);				
				}
			}
		}

		++m_cAffectedNum;
		return true;
	}

	return false;
}


bool CSpell::RemoveAffected(CAggresiveCreature* pRemoved)
{
	CAggresiveCreature**	ppBegin		= m_pAffected;
	CAggresiveCreature**	ppPastEnd	= m_pAffected + m_cAffectedNum;
    
	unsigned char cIndex = 0;

	for (;ppBegin != ppPastEnd; ++ppBegin, ++cIndex)
	{		
		if ((*ppBegin) == pRemoved)
		{
			// Deactivate & Remove
			if (true == IsActivate(cIndex))
			{
				if (true == Deactivate(pRemoved, 0))
				{
					SetDeactivate(cIndex);
				}
			}

			// edith 2009.05.15 스킬 삭제로그 덤프
//			SERLOG1(g_SkillLog, "CSpell::RemoveAffected %d", pRemoved->GetCID());

            if (false == (*ppBegin)->GetSpellMgr().GetAffectedInfo().Remove(this))
			{
				SPLOG_ERRLEVEL(ERRLOG1(g_Log, "CID:0x%08x 상호참조에 문제가 있습니다! "
					"캐릭터에서 주문을 제거할 수 없습니다.", (*ppBegin)->GetCID()));	
			}

			std::copy(ppBegin + 1, ppPastEnd, ppBegin);
			RemoveIndex(cIndex);
			--m_cAffectedNum;
			return true;
		}
	}

	return false;
}


void CSpell::ClearAffected(void)
{
	CAggresiveCreature**	ppBegin		= m_pAffected;
	CAggresiveCreature**	ppPastEnd	= m_pAffected + m_cAffectedNum;
    
	unsigned char cIndex = 0;

	for (;ppBegin != ppPastEnd; ++ppBegin, ++cIndex)
	{		
		// Deactivate & Remove
		if (true == IsActivate(cIndex))
		{
			if (true == Deactivate(*ppBegin, 0))
			{
				SetDeactivate(cIndex);
			}
		}

		// edith 2009.05.15 스킬 삭제로그 덤프
//		SERLOG0(g_SkillLog, "CSpell::ClearAffected");

        if (false == (*ppBegin)->GetSpellMgr().GetAffectedInfo().Remove(this))
		{
			SPLOG_ERRLEVEL(ERRLOG1(g_Log, "CID:0x%08x 상호참조에 문제가 있습니다! "
				"캐릭터에서 주문을 제거할 수 없습니다.", (*ppBegin)->GetCID()));	
		}	
	}

	m_cAffectedNum = 0;
}


bool CSpell::Operate(void)
{
	if (INFINITE_DURATION != m_dwDurationSec)
	{
		if (0 == m_dwDurationSec)
		{
			return false;
		}

		--m_dwDurationSec;
	}
	
    
    if (m_eSkillType == Skill::Type::CHANT && true == IsEnabled())
	{
        CheckRange();
	}

	CAggresiveCreature** ppBegin	= m_pAffected;
	CAggresiveCreature** ppPastEnd	= m_pAffected + m_cAffectedNum;

	unsigned char cIndex = 0;

	for (; ppBegin != ppPastEnd; ++ppBegin, ++cIndex)
	{
		if (true == IsActivate(cIndex))
		{
			Operate(*ppBegin);
		}
	}

	return true;
}


void CSpell::CheckRange(void)
{
	//	Caster가 있을 경우에만 체크한다.
	if (NULL != m_pCaster && true == IsEnabled())
	{
		const Position&		CasterPos	= m_pCaster->GetCurrentPos();
		const float			fRange		= m_ProtoType.m_fEffectExtent;
	
		CAggresiveCreature**	ppBegin		= m_pAffected;
		CAggresiveCreature**	ppPastEnd	= m_pAffected + m_cAffectedNum;

		unsigned char cIndex = 0;

		for (; ppBegin != ppPastEnd; ++ppBegin, ++cIndex)
		{
			if (CasterPos.GetDistance((*ppBegin)->GetCurrentPos()) <= fRange)
			{
				if (false == IsActivate(cIndex))
				{
					CSpell* pSpell = (*ppBegin)->GetSpellMgr().GetAffectedInfo().GetSpell(m_wSpellID);
					if (NULL == pSpell)
					{
						if (true == Activate(*ppBegin, 0))
						{
							SetActivate(cIndex);
						}
					}
					else
					{
						if (pSpell->GetSpellLevel() < m_wSpellLevel)
						{
							if (true == pSpell->Deactivate(*ppBegin, 0))
							{
								unsigned char cAffectedIndex = pSpell->GetAffectedIndex(*ppBegin);
								if (MAX_AFFECTED != cAffectedIndex)
								{
									pSpell->SetDeactivate(cAffectedIndex);
								}

								if (true == Activate(*ppBegin, 0))
								{
									SetActivate(cIndex);
								}
							}
						}
					}
				}
			}
			else
			{
				if (true == IsActivate(cIndex))
				{
					if (true == Deactivate(*ppBegin, 0))
					{
						SetDeactivate(cIndex);
					}
				}
			}
		}
	}
}


const Skill::SpellTarget::Type CSpell::GetSpellTarget(void)
{
	if (Skill::Type::CHANT == m_eSkillType)
	{
		return Skill::SpellTarget::CHANT;
	}
	else if (Skill::Type::ENCHANT == m_eSkillType)
	{
		switch (m_ProtoType.m_eTargetType)
		{
			case Skill::Target::FRIEND:
			case Skill::Target::FRIEND_EXCEPT_SELF:
			case Skill::Target::FRIEND_OBJECT:
			case Skill::Target::DEAD_FRIEND:
			case Skill::Target::PARTY:
				return Skill::SpellTarget::FRIEND_TARGET_ENCHANT;

			case Skill::Target::MELEE:
			case Skill::Target::ENEMY:
			case Skill::Target::ENEMY_OBJECT:
			case Skill::Target::DEAD_ENEMY:
				return Skill::SpellTarget::ENEMY_TARGET_ENCHANT;
		}
    }

	return Skill::SpellTarget::NONE;
}


void CSpell::Enable(unsigned long dwOperateFlag)
{
	if (false == IsEnabled())
	{
		m_cInternalFlags &= ~DISABLED;

        if (m_eSkillType == Skill::Type::CHANT)
		{
			CheckRange();
		}
		else
		{
			CAggresiveCreature** ppBegin	= m_pAffected;
			CAggresiveCreature** ppPastEnd	= m_pAffected + m_cAffectedNum;
			unsigned char cIndex = 0;

			for (; ppBegin != ppPastEnd; ++ppBegin, ++cIndex)
			{
				if (false == IsActivate(cIndex))
				{
					if (true == Activate(*ppBegin, dwOperateFlag))
					{
						SetActivate(cIndex);
					}
				}
			}
		}
	}
}


void CSpell::Disable(unsigned long dwOperateFlag)
{
	if (true == IsEnabled())
	{
		CAggresiveCreature** ppBegin	= m_pAffected;
		CAggresiveCreature** ppPastEnd	= m_pAffected + m_cAffectedNum;
		unsigned char cIndex = 0;		

		for (; ppBegin != ppPastEnd; ++ppBegin, ++cIndex)
		{
			if (true == IsActivate(cIndex))
			{				
				if (true == Deactivate(*ppBegin, dwOperateFlag))
				{
					SetDeactivate(cIndex);
				}
			}
		}

		m_cInternalFlags |= DISABLED;
	}
}

unsigned char CSpell::GetAffectedIndex(CAggresiveCreature* lpAffected)
{
	CAggresiveCreature** ppBegin	= m_pAffected;
	CAggresiveCreature** ppPastEnd	= m_pAffected + m_cAffectedNum;
	unsigned char cIndex = 0;

	for (; ppBegin != ppPastEnd; ++ppBegin, ++cIndex)
	{
		if ((*ppBegin) == lpAffected)
		{
			return cIndex;
		}
	}

	return MAX_AFFECTED;
}


void CSpell::SendSpellInfo(CAggresiveCreature* lpAffected, unsigned char cSpellType, 
                           unsigned short nEnchantLevel, unsigned long dwEnchantTime, bool bOnOff)
{
	CCell* pCell = lpAffected->GetCellPos().m_lpCell;
	if (pCell != NULL)
    { 
		PktSpInfo pktSpInfo;
        memset(&pktSpInfo, 0, sizeof(PktSpInfo));

	    pktSpInfo.m_dwCharID = lpAffected->GetCID();

	    pktSpInfo.m_cSpellType		= cSpellType;
	    pktSpInfo.m_nEnchantLevel	= nEnchantLevel;
	    pktSpInfo.m_bOnOff			= bOnOff;				
		pktSpInfo.m_dwEnchantTime	= dwEnchantTime;

		if (PacketWrap::WrapCrypt(reinterpret_cast<char*>(&pktSpInfo), sizeof(PktSpInfo), CmdCharSpellInfo, 0))
		{
			pCell->SendNowAllNearCellCharacter(reinterpret_cast<char*>(&pktSpInfo), sizeof(PktSpInfo), CmdCharSpellInfo);
		}
	}
}