#ifndef _RYL_DBAGENT_PARTY_DB_INFO_H_
#define _RYL_DBAGENT_PARTY_DB_INFO_H_

#pragma once

#include <DB/DBdefine.h>


namespace DBAgent
{

class CPartyDBInfo
{
public:

	CPartyDBInfo(unsigned long PartyID_In);

    bool SerializeIn(PARTY_INFO& PartyInfo_In, PARTY_USER_INFO& PartyUserInfo_In);
    bool SerializeOut(PARTY_INFO& PartyInfo_Out, PARTY_USER_INFO& PartyUserInfo_Out);

	unsigned long GetFirstMem();
	bool IsMember(unsigned long CharID_In);

	bool PartyMemLogin(unsigned long dwCharID_In, unsigned long dwServer_In);
	bool PartyMemLogout(unsigned long dwCharID_In);
	void PartyMemAllLogout();
	
	bool IsLogined(unsigned long CharID_In);
	bool IsLoginedByIndex(unsigned long Index) { return m_Party.ServerID[Index]; }

	bool TransferLeader(unsigned long OldLeaderID, unsigned long NewLeaderID);	
	bool InsertPartyMem(unsigned long ServerID_In, unsigned long CharID_In, unsigned short Class_In, char Level_In, unsigned long GID_In, const char* CharName_In);
	bool DeletePartyMem(unsigned long CharID_In, unsigned long& ServerID_Out);

	bool AutoRouting(unsigned long CharID_In, bool bOn, bool bAll = false);

	const PARTY&    GetParty() const { return m_Party; }		

    unsigned long	GetPID()			{ return m_Party.m_dwPartyID;   }	
    unsigned long	GetLeader()			{ return m_Party.m_dwLeaderID;  }
	unsigned long	GetPartyMemNum()	{ return m_Party.m_cMemberNum;	}
	
    unsigned long	SetPID(unsigned long PID_In)    { return m_Party.m_dwPartyID = PID_In;	}
    unsigned long	SetLeader(unsigned long ID_In)  { return m_Party.m_dwLeaderID = ID_In;	}

	// 유저 정보 관련 //

	const PARTY_USERINFO& GetPartyUserInfo() const { return m_PartyUserInfo; }	

	bool SetUserInfoLevel(unsigned long dwCID, char cLevel);
	bool SetUserInfoClass(unsigned long dwCID, unsigned short wClass);
	bool SetUserInfoGID(unsigned long dwCID, unsigned long dwGID);	

private:

    PARTY			m_Party;							// 파티에 기본 정보.
    bool			m_bAutoRouting[PARTY::MAX_MEM];
	
	PARTY_USERINFO	m_PartyUserInfo;					// 파티에 유저 정보
};

}

#endif