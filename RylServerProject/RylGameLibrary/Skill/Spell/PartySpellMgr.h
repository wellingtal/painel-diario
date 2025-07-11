#ifndef _CPARTY_SPELL_MGR_
#define _CPARTY_SPELL_MGR_

//	전방 참조
class CParty;
class CSpell;
class CAggresiveCreature;


#define LOG_FFLCHANT __FUNCTION__, __FILE__, __LINE__

// 챈트 관련 버그 로그를 찍는다. 문제 해결후 나중에 전부 제거한다.
void LogChantBug(CAggresiveCreature* lpCreature, CParty* lpParty, 
                 const char* szMessage, const char* lpRtn, const char* lpFileName, int nLine);


class CPartySpellMgr
{
public:

	CPartySpellMgr();
	~CPartySpellMgr();

    void SetOwner(CParty* lpParty) { m_lpOwnerParty = lpParty; }
	
	bool AddMember(CAggresiveCreature* pNewMember);
	bool RemoveMember(CAggresiveCreature* pRemoveMember);

	void ClearMember();

	void AddAffectedToAllMember(CSpell* pSpell, unsigned short wMapIndex=0);

protected:

	enum { MAX_MEMBER = 12 };//was 10 hayzohar

    CParty*             m_lpOwnerParty;
	CAggresiveCreature*	m_pPartyMember[MAX_MEMBER];
	unsigned long       m_dwMemberNum;
};

#endif