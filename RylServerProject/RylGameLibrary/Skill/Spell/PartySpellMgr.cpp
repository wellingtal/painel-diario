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
        // ����� �ִ��� ���ɴ�. ������ ��.
		CAggresiveCreature** ppBegin	= m_pPartyMember;
		CAggresiveCreature** ppPastEnd	= m_pPartyMember + m_dwMemberNum;
        
        for (;ppBegin != ppPastEnd; ++ppBegin)
        {
            if(lpNewMember == (*ppBegin))
            {
                LogChantBug(lpNewMember, m_lpOwnerParty, 
                    "��� �߰��� �����߽��ϴ�. ����� �̹� �ֽ��ϴ�.", LOG_FFLCHANT);
                return false;
            }
        }

		CAffectedSpell& newAffectedInfo	= lpNewMember->GetSpellMgr().GetAffectedInfo();

		ppBegin   = m_pPartyMember;
		ppPastEnd = m_pPartyMember + m_dwMemberNum;
		
		for (;ppBegin != ppPastEnd; ++ppBegin)
		{
			// ���� ĳ���Ͱ� ������ �ִ� �ֹ���, ���ο� ĳ���͸� �߰���Ŵ.
            CAggresiveCreature* lpOldMember = (*ppBegin);
			CAffectedSpell& memberAffectedInfo = lpOldMember->GetSpellMgr().GetAffectedInfo();

			memberAffectedInfo.ApplyPartyChant(lpNewMember);

			// ���� �������� ĳ���Ͱ� ������ �ִ� �ֹ���, ���� ĳ���͵��� �߰���Ŵ.
            newAffectedInfo.ApplyPartyChant(lpOldMember);
		}

        m_pPartyMember[m_dwMemberNum] = lpNewMember;
		++m_dwMemberNum;

        LogChantBug(lpNewMember, m_lpOwnerParty,
            "��� �߰��� �����߽��ϴ�", LOG_FFLCHANT);
		return true;
	}

	return false;
}


bool CPartySpellMgr::RemoveMember(CAggresiveCreature* lpRemoveMember)
{
	CAggresiveCreature** ppBegin	= m_pPartyMember;
	CAggresiveCreature** ppPastEnd	= m_pPartyMember + m_dwMemberNum;

	// �����Ϸ��� ����� �˻��Ѵ�.
	for (;ppBegin != ppPastEnd; ++ppBegin)
	{		
		if (0 != lpRemoveMember && (*ppBegin) == lpRemoveMember)
		{
			//	��� ����.
			std::copy(ppBegin + 1, ppPastEnd, ppBegin);
			--m_dwMemberNum;

            // ��� ������ �Ŀ�, 0���� ä���.
            m_pPartyMember[m_dwMemberNum] = 0;

			CAffectedSpell& removeAffectedInfo = 
                lpRemoveMember->GetSpellMgr().GetAffectedInfo();

            ppBegin   = m_pPartyMember;
            ppPastEnd = m_pPartyMember + m_dwMemberNum;
			for (; ppBegin != ppPastEnd; ++ppBegin)
			{
                CAggresiveCreature* lpOldMember = (*ppBegin);

				//	�����Ϸ��� ������Լ�, �ٸ� ������� Caster�� �ִ� Chant�� ���� �����.
                removeAffectedInfo.RemoveChantByCaster(lpOldMember);

				//	�ٸ� ����鿡�Լ�, ������� �ϴ� ����� Caster�� �ִ� Chant�� ���� �����.
				CAffectedSpell& memberAffectedInfo = 
                    lpOldMember->GetSpellMgr().GetAffectedInfo();

				memberAffectedInfo.RemoveChantByCaster(lpRemoveMember);
			}

            LogChantBug(lpRemoveMember, m_lpOwnerParty,
                "��� ���ſ� �����߽��ϴ�", LOG_FFLCHANT);
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
        "��� ���� ���ſ� �����߽��ϴ�", LOG_FFLCHANT);
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
            // Creature�� NULL�̰ų� ���� �ٸ��� ��ȿ
            continue;
        }

		if (lpCreature->GetEnchantInfo().GetFlag(Skill::SpellID::Hide))				
        {
            // Creature�� ���� ������ ��ȿ
            continue; 
        }
		
        if (0 == pSpell->GetCaster() ||
            EnemyCheck::EC_FRIEND != pSpell->GetCaster()->IsEnemy(lpCreature))
        {
            // �����ڰ� ���ų�, �����ڰ� �Ʊ��� �ƴϸ� ��ȿ
            continue; 
        }

		pSpell->AddAffected(*ppBegin, wError);
	}
}



// æƮ ���� ���� �α׸� ��´�. ���� �ذ��� ���߿� ���� �����Ѵ�.
void LogChantBug(CAggresiveCreature* lpCreature, CParty* lpParty,
                 const char* szMessage, const char* lpRtn, const char* lpFileName, int nLine)
{
    if(0 == CServerSetup::GetInstance().GetServerGroup() && 0 != szMessage && 0 != lpParty)
    {
        if(0 != lpCreature)
        {
            // �׼��̰�. Creature�� ����� ���õǾ��� ���� ��´�.
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
