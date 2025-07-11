#include "stdafx.h"

#include <Creature/AggresiveCreature.h>

#include "Spell.h"
#include "Affected.h"
#include "PartySpellMgr.h"


#include <Utility/Setup/ServerSetup.h>
#include <Community/Party/Party.h>


CPartySpellMgr::CPartySpellMgr()
:	m_dwMemberNum(0), m_lpOwnerParty(0)
{
	std::fill_n(&m_pPartyMember[0], int(MAX_MEMBER), reinterpret_cast<CAggresiveCreature*>(0));
}

CPartySpellMgr::~CPartySpellMgr()
{
	ClearMember();
}


bool CPartySpellMgr::AddMember(CAggresiveCreature* lpNewMember)
{
	if (m_dwMemberNum < MAX_MEMBER && NULL != lpNewMember)
	{
        // 멤버가 있는지 살핀다. 있으면 즐.
		CAggresiveCreature** ppBegin	= m_pPartyMember;
		CAggresiveCreature** ppPastEnd	= m_pPartyMember + m_dwMemberNum;
        
        for (;ppBegin != ppPastEnd; ++ppBegin)
        {
            if(lpNewMember == (*ppBegin))
            {
                LogChantBug(lpNewMember, m_lpOwnerParty, 
                    "멤버 추가에 실패했습니다. 멤버가 이미 있습니다.", LOG_FFLCHANT);
                return false;
            }
        }

		CAffectedSpell& newAffectedInfo	= lpNewMember->GetSpellMgr().GetAffectedInfo();

		ppBegin   = m_pPartyMember;
		ppPastEnd = m_pPartyMember + m_dwMemberNum;
		
		for (;ppBegin != ppPastEnd; ++ppBegin)
		{
			// 현재 캐릭터가 가지고 있는 주문에, 새로운 캐릭터를 추가시킴.
            CAggresiveCreature* lpOldMember = (*ppBegin);
			CAffectedSpell& memberAffectedInfo = lpOldMember->GetSpellMgr().GetAffectedInfo();

			memberAffectedInfo.ApplyPartyChant(lpNewMember);

			// 새로 넣으려는 캐릭터가 가지고 있는 주문에, 현재 캐릭터들을 추가시킴.
            newAffectedInfo.ApplyPartyChant(lpOldMember);
		}

        m_pPartyMember[m_dwMemberNum] = lpNewMember;
		++m_dwMemberNum;

        LogChantBug(lpNewMember, m_lpOwnerParty,
            "멤버 추가에 성공했습니다", LOG_FFLCHANT);
		return true;
	}

	return false;
}


bool CPartySpellMgr::RemoveMember(CAggresiveCreature* lpRemoveMember)
{
	CAggresiveCreature** ppBegin	= m_pPartyMember;
	CAggresiveCreature** ppPastEnd	= m_pPartyMember + m_dwMemberNum;

	// 제거하려는 멤버를 검색한다.
	for (;ppBegin != ppPastEnd; ++ppBegin)
	{		
		if (0 != lpRemoveMember && (*ppBegin) == lpRemoveMember)
		{
			//	멤버 제거.
			std::copy(ppBegin + 1, ppPastEnd, ppBegin);
			--m_dwMemberNum;

            // 멤버 제거한 후에, 0으로 채운다.
            m_pPartyMember[m_dwMemberNum] = 0;

			CAffectedSpell& removeAffectedInfo = 
                lpRemoveMember->GetSpellMgr().GetAffectedInfo();

            ppBegin   = m_pPartyMember;
            ppPastEnd = m_pPartyMember + m_dwMemberNum;
			for (; ppBegin != ppPastEnd; ++ppBegin)
			{
                CAggresiveCreature* lpOldMember = (*ppBegin);

				//	제거하려는 멤버에게서, 다른 멤버들이 Caster로 있는 Chant를 전부 지운다.
                removeAffectedInfo.RemoveChantByCaster(lpOldMember);

				//	다른 멤버들에게서, 지우려고 하는 멤버가 Caster로 있는 Chant를 전부 지운다.
				CAffectedSpell& memberAffectedInfo = 
                    lpOldMember->GetSpellMgr().GetAffectedInfo();

				memberAffectedInfo.RemoveChantByCaster(lpRemoveMember);
			}

            LogChantBug(lpRemoveMember, m_lpOwnerParty,
                "멤버 제거에 성공했습니다", LOG_FFLCHANT);
			return true;
		}
	}

	return false;
}


void CPartySpellMgr::ClearMember()
{
    CAggresiveCreature**    ppExternalBegin = m_pPartyMember;
	CAggresiveCreature**	ppPastEnd       = m_pPartyMember + m_dwMemberNum;

	for (; ppExternalBegin != ppPastEnd; ++ppExternalBegin)
	{		
		CAffectedSpell& memberAffectedInfo = (*ppExternalBegin)->GetSpellMgr().GetAffectedInfo();

        CAggresiveCreature** ppInternalBegin = m_pPartyMember;
		for (; ppInternalBegin != ppPastEnd; ++ppInternalBegin)
		{
			if (*ppInternalBegin != *ppExternalBegin)
			{
				memberAffectedInfo.RemoveChantByCaster(*ppInternalBegin);
			}
		}
	}

    std::fill_n(&m_pPartyMember[0], int(MAX_MEMBER), 
        reinterpret_cast<CAggresiveCreature*>(0));

	m_dwMemberNum = 0;

    LogChantBug(0, m_lpOwnerParty, 
        "멤버 전부 제거에 성공했습니다", LOG_FFLCHANT);
}


void CPartySpellMgr::AddAffectedToAllMember(CSpell* pSpell, unsigned short wMapIndex)
{
	CAggresiveCreature** ppBegin	= m_pPartyMember;
	CAggresiveCreature** ppPastEnd	= m_pPartyMember + m_dwMemberNum;
	
	unsigned short wError = CSpell::NO_ENCHANT_ERROR;
	for (;ppBegin != ppPastEnd; ++ppBegin)
	{
        CAggresiveCreature* lpCreature = (*ppBegin);

		if (0 == lpCreature || lpCreature->GetMapIndex() != wMapIndex)	
        {
            // Creature가 NULL이거나 맵이 다르면 무효
            continue;
        }

		if (lpCreature->GetEnchantInfo().GetFlag(Skill::SpellID::Hide))				
        {
            // Creature가 숨어 있으면 무효
            continue; 
        }
		
        if (0 == pSpell->GetCaster() ||
            EnemyCheck::EC_FRIEND != pSpell->GetCaster()->IsEnemy(lpCreature))
        {
            // 시전자가 없거나, 시전자가 아군이 아니면 무효
            continue; 
        }

		pSpell->AddAffected(*ppBegin, wError);
	}
}



// 챈트 관련 버그 로그를 찍는다. 문제 해결후 나중에 전부 제거한다.
void LogChantBug(CAggresiveCreature* lpCreature, CParty* lpParty,
                 const char* szMessage, const char* lpRtn, const char* lpFileName, int nLine)
{
    if(0 == CServerSetup::GetInstance().GetServerGroup() && 0 != szMessage && 0 != lpParty)
    {
        if(0 != lpCreature)
        {
            // 테섭이고. Creature가 제대로 세팅되었을 때만 찍는다.
           /* g_Log.DetailLog(LOG_DETAIL, lpRtn, lpFileName, nLine, "CID:0x%08x(0x%08x)/PID:0x%08x(0x%08x)/ChantBug - %s",
                lpCreature->GetCID(), lpCreature, lpParty->GetUID(), lpParty, szMessage);*///hz chantbug log disable
        }
        else
        {
          /*  g_Log.DetailLog(LOG_DETAIL, lpRtn, lpFileName, nLine, "PID:0x%08x(0x%08x)/ChantBug - %s",
                lpParty->GetUID(), lpParty, szMessage);*///hz chantbug log disable
        }
    }
}
