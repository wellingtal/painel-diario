#ifndef _RYL_DBAGENT_PARTY_DB_MGR_H_
#define _RYL_DBAGENT_PARTY_DB_MGR_H_

#pragma once

#include <map>
#include <boost/pool/pool_alloc.hpp>
#include <boost/pool/pool.hpp>

// forward decl.
class CDBComponent;
class CSendStream;

namespace DBAgent
{

// forward delc.
class CPartyDBInfo;

class CPartyDBMgr
{
public:

    static CPartyDBMgr& GetInstance();	
    
	CPartyDBInfo* OpenParty(CDBComponent& DBComponent, unsigned long dwPartyID);
	bool CloseParty(CDBComponent& DBComponent, unsigned long dwPartyID);					
	
	CPartyDBInfo* CreateParty(CDBComponent& DBComponent, unsigned long dwLeaderID, char cLeaderLevel, unsigned short wLeaderClass, unsigned long dwLeaderGID,
								unsigned long dwMemberID, char cMemberLevel, unsigned short wMemberClass, unsigned long dwMemberGID);

	bool DestoryParty(CDBComponent& DBComponent, unsigned long dwPartyID);

	bool InsertPartyMember(CDBComponent& DBComponent, unsigned long dwPartyID, unsigned long dwCharID, unsigned long dwGID,
							unsigned short wClass, char cLevel);	

	bool DeletePartyMember(CDBComponent& DBComponent, unsigned long dwPartyID, 
        unsigned long dwCharID, unsigned long dwReference);

	bool AutoRoutingOn(CDBComponent& DBComponent, unsigned long dwPartyID, unsigned long dwCharID);
	bool AutoRoutingOff(CDBComponent& DBComponent, unsigned long dwPartyID, unsigned long dwCharID);
    
    bool InsertPartyMap(unsigned long dwPartyID, CPartyDBInfo* lpParty);
    bool DeletePartyMap(unsigned long dwPartyID);
    CPartyDBInfo* FindPartyMap(unsigned long dwPartyID_In);

	void LoginPartyMember(CPartyDBInfo* lpParty, unsigned long dwUserID, unsigned long dwCharID, unsigned long dwGID, 
			unsigned short wClass, unsigned long dwServerID, char cLevel);

	void LogoutPartyMember(CDBComponent& DBComponent, CPartyDBInfo* lpParty, unsigned long dwCharID);	

	void SendToGameServerPartyData(CSendStream& SendStream);	

private:

    CPartyDBMgr();
    ~CPartyDBMgr();

    CPartyDBInfo* CreatePartyDBInfo(unsigned long dwPID);
    void DeletePartyDBInfo(CPartyDBInfo* lpPartyDBInfo);    

	enum Const
	{
		MIN_MEMBER_NUM	= 2
	};

	typedef std::map<unsigned long, CPartyDBInfo*, std::less<unsigned long>,
		boost::fast_pool_allocator<std::pair<unsigned long, CPartyDBInfo*> > > PartyMap;

    PartyMap        m_PartyMap;
    boost::pool<>   m_PartyDBPool;
};

}

#endif