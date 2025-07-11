//=================================================================
// Name : RYLCommunityData.h
// Data : 2003. 11. 20
// Desc : RYL Community Data
//			- ��Ƽ, ���, ����.. Ŀ�´�Ƽ�� ���õ� Ŭ����
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
	std::list<LPRefuseNode>		m_lstRefuse ;			// �źλ�� ����Ʈ

	BOOL 			m_bPartyFind;						// ��Ƽ ã��
	BOOL 			m_bPartyPlayerFind;					// ��Ƽ�� ã��
	unsigned char 	m_cNumMember;						// ��Ƽ ã�⿡ �ʿ��� ����
	unsigned char 	m_cNumParty;						// ��Ƽ ã�⿡ �ʿ��� ����
	MemberFindParty m_mfpMember[30];					// ��Ƽ ã�⿡ �ʿ��� ����
	PartyFindMember m_pfmParty[30];						// ��Ƽ ã�⿡ �ʿ��� ����
	unsigned long 	m_dwSelectMemberSlot;				// ��Ƽ ã�⿡ �ʿ��� ����
	unsigned long 	m_dwSelectPartySlot;				// ��Ƽ ã�⿡ �ʿ��� ����

	char			m_strRecentChatChrID[ MAX_PATH ] ;	// �ֱ� �ӼӸ��� ĳ������ ID
	BOOL			m_bInstanceWhisper;
	char			m_strWhisperChrID[ MAX_PATH ] ;		// ���� �ͼӸ� ���
	char			m_strChatMessage[ 3 ][ MAX_PATH ] ;	// ���� ����

//	unsigned long	m_dwFriendResult;					// ������ ���� �޼���â �����
//	char			m_strFriendName[ MAX_PATH ] ;		// ģ�� �̸�

	bool			m_bPeaceMode;						// ��ȭ��� / �������
	
	unsigned long	m_dwPeaceModeResult;				// ��ȭ������ �޼���â �����
	unsigned long	m_dwPeaceMode;						// ��ȭ��� / ������� �޼���â ����

	BOOL			m_bStoneStatueRespawn ;

	INT				m_iGulidMsgBoxState ; 
	DWORD			m_dwRefenceCID ;
	DWORD			m_dwRefenceGID ;
	DWORD			m_dwResInvite ;
	bool			m_bCheckRelation ;					// �� ������ ���谡 ������ ��尡 �ִ°�?

	DWORD			m_dwCastleDoorID ;					// ���� ���õǾ� �ִ� ���� ID


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