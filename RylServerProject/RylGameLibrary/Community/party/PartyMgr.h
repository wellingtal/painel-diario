#ifndef _RYL_GAME_LIBRARY_PARTY_MGR_H_
#define _RYL_GAME_LIBRARY_PARTY_MGR_H_

#include <Thread/Lock.h>

#include <map>
#include <boost/pool/pool_alloc.hpp>

#include "Party.h"

class CPartyMgr
{
public:	
    static CPartyMgr& GetInstance();

	void DestoryPartyList();

	bool RequestPartyInfoToDB(unsigned long dwLeaderCID, unsigned long dwPartyUID);
	
	bool AddParty(CParty *pParty);
	CParty* GetParty(unsigned long dwPartyUID);
	bool DeleteParty(unsigned long dwPartyUID);

	bool AddFindPartyList(unsigned long dwCID);
	bool DeleteFindPartyList(unsigned long dwCID);
	bool AddFindMemberList(unsigned long dwPartyUID);
	bool DeleteFindMemberList(unsigned long dwPartyUID);

	unsigned long GetTempUID(void) { m_tempUID++; return m_tempUID; }

	void SendPartyFind(CCharacter* pCharacter);

    void UpdatePartyData();	

protected:    

    CPartyMgr();
    ~CPartyMgr();

	typedef std::map<unsigned long, CParty*, std::less<unsigned long>, 
		boost::fast_pool_allocator<std::pair<unsigned long, CParty* > > >	    PartyMap;

	typedef std::map<unsigned long, CCharacter*, std::less<unsigned long>, 
		boost::fast_pool_allocator<std::pair<unsigned long, CCharacter* > > >	MemberFindPartyMap;

	typedef std::map<unsigned long, CParty*, std::less<unsigned long>, 
		boost::fast_pool_allocator<std::pair<unsigned long, CParty* > > >       PartyFindMemberMap;

    PartyMap	        m_PartyMap;
	MemberFindPartyMap	m_MemberFindPartyMap;
	PartyFindMemberMap	m_PartyFindMemberMap;    

	unsigned long		m_tempUID;
};


#endif