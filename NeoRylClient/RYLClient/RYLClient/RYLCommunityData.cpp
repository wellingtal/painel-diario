#include "RYLCommunityData.h"
#include "GMMemory.h"

CRYLCommunityData*	CRYLCommunityData::s_pInstance = NULL ;

CRYLCommunityData::CRYLCommunityData() 
{
	m_cNumMember = 0;
	m_cNumParty = 0;
	ZeroMemory(m_mfpMember, sizeof(m_mfpMember));
	ZeroMemory(m_pfmParty, sizeof(m_pfmParty));

	strcpy(m_strRecentChatChrID, "");
	strcpy(m_strWhisperChrID, "");
	m_bInstanceWhisper = FALSE;

	m_dwPeaceModeResult = 0;
	m_dwPeaceMode = 0;
//	m_dwFriendResult = 0;

	m_bStoneStatueRespawn	= FALSE ;

	m_iGulidMsgBoxState 	= -1 ; 
	m_dwRefenceCID			= 0L ;
	m_dwRefenceGID			= 0L ;
	m_dwResInvite			= 0L ;
	
	m_dwCastleDoorID		= 0L ;
	m_bCheckRelation		= false ;
	

	m_bPartyFind = FALSE;
	m_bPartyPlayerFind = FALSE;

	s_pInstance				= this ;
}

CRYLCommunityData::~CRYLCommunityData() 
{
}

VOID	CRYLCommunityData::DestroyRefuseList() 
{
	std::list<LPRefuseNode>::iterator	it ;
	LPRefuseNode lpDelete ;

	for ( it = m_lstRefuse.begin() ; it != m_lstRefuse.end() ; )
	{
		lpDelete = ( *it ) ;
		it = m_lstRefuse.erase( it ) ;
		delete lpDelete ;
	}
}

void	CRYLCommunityData::AddRefuseList(unsigned long dwChrID)
{
	LPRefuseNode lpNode = new RefuseNode;
	lpNode->m_dwRefuseChrID = dwChrID;
	lpNode->m_lRefuseTimer = 5000;

	m_lstRefuse.push_back(lpNode);
}

BOOL	CRYLCommunityData::GetIsRefuse(unsigned long dwChrID)
{
	for(std::list<LPRefuseNode>::iterator it = m_lstRefuse.begin(); it != m_lstRefuse.end(); it++)
	{
		if((*it)->m_dwRefuseChrID == dwChrID) 
		{
			return TRUE;
		}
	}

	return FALSE;
}

void	CRYLCommunityData::UpdateRefuseList(float fUpdate)
{
	for(std::list<LPRefuseNode>::iterator it = m_lstRefuse.begin(); it != m_lstRefuse.end(); ++it)
	{
		(*it)->m_lRefuseTimer -= static_cast<long>(fUpdate);
		if((*it)->m_lRefuseTimer < 0)
		{
			LPRefuseNode lpDelete = (*it);
			it = m_lstRefuse.erase(it);
			delete lpDelete;
			--it;
		}
	}
}

//=======================================================================
// by ichabod
// desc : 길드 관련 메소드
VOID	CRYLCommunityData::SetMsgBoxState( INT iState, DWORD dwRefenceGID, DWORD dwRefenceCID ) 
{ 
	m_iGulidMsgBoxState = iState ; 
	m_dwRefenceCID		= dwRefenceCID ;
	m_dwRefenceGID		= dwRefenceGID ;
}