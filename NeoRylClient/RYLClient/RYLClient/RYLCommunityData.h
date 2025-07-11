//=================================================================
// Name : RYLCommunityData.h
// Data : 2003. 11. 20
// Desc : RYL Community Data
//			- 파티, 길드, 듀얼등.. 커뮤니티에 관련된 클레스
//=================================================================
#ifndef __RYL_COMMUNITYDATA_H__
#define __RYL_COMMUNITYDATA_H__

#include <Network/Packet/PacketStruct/CharCommunityPacket.h>

#include <windows.h>
#include <list>

typedef struct RefuseNode
{
	unsigned long m_dwRefuseChrID;
	long m_lRefuseTimer;
} RefuseNode, *LPRefuseNode;

class CRYLCommunityData
{
public :
	std::list<LPRefuseNode>		m_lstRefuse ;			// 거부상대 리스트

	BOOL 			m_bPartyFind;						// 파티 찾기
	BOOL 			m_bPartyPlayerFind;					// 파티원 찾기
	unsigned char 	m_cNumMember;						// 파티 찾기에 필요한 변수
	unsigned char 	m_cNumParty;						// 파티 찾기에 필요한 변수
	MemberFindParty m_mfpMember[30];					// 파티 찾기에 필요한 변수
	PartyFindMember m_pfmParty[30];						// 파티 찾기에 필요한 변수
	unsigned long 	m_dwSelectMemberSlot;				// 파티 찾기에 필요한 변수
	unsigned long 	m_dwSelectPartySlot;				// 파티 찾기에 필요한 변수

	char			m_strRecentChatChrID[ MAX_PATH ] ;	// 최근 귓속말한 캐릭터의 ID
	BOOL			m_bInstanceWhisper;
	char			m_strWhisperChrID[ MAX_PATH ] ;		// 현재 귀속말 상대
	char			m_strChatMessage[ 3 ][ MAX_PATH ] ;	// 도배 방지

//	unsigned long	m_dwFriendResult;					// 프랜드 관련 메세지창 결과값
//	char			m_strFriendName[ MAX_PATH ] ;		// 친구 이름

	bool			m_bPeaceMode;						// 평화모드 / 반전모드
	
	unsigned long	m_dwPeaceModeResult;				// 평화모드시의 메세지창 결과값
	unsigned long	m_dwPeaceMode;						// 평화모드 / 반전모드 메세지창 구분

	BOOL			m_bStoneStatueRespawn ;

	INT				m_iGulidMsgBoxState ; 
	DWORD			m_dwRefenceCID ;
	DWORD			m_dwRefenceGID ;
	DWORD			m_dwResInvite ;
	bool			m_bCheckRelation ;					// 내 길드와의 관계가 변동된 길드가 있는가?

	DWORD			m_dwCastleDoorID ;					// 현제 선택되어 있는 성문 ID


public :
	CRYLCommunityData() ;
	~CRYLCommunityData() ;

	VOID	DestroyRefuseList() ;
	void	AddRefuseList( unsigned long dwChrID ) ;
	BOOL	GetIsRefuse( unsigned long dwChrID ) ;
	void	UpdateRefuseList( float fUpdate ) ;

	BOOL	GetStoneStatueRespawn()							{ return m_bStoneStatueRespawn ;		}
	VOID	SetStoneStatueRespawn( BOOL bRespawn )			{ m_bStoneStatueRespawn = bRespawn ;	}

	VOID	SetMsgBoxState( INT iState, DWORD dwRefenceGID, DWORD dwRefenceCID ) ;

// Signton
public :
	static CRYLCommunityData*	s_pInstance ;
	static CRYLCommunityData*	Instance()	{ return s_pInstance ; } 
} ;

inline CRYLCommunityData* GRYLCommunityDataInstance()	{ return CRYLCommunityData::Instance();		}

#endif //__RYL_COMMUNITYDATA_H__