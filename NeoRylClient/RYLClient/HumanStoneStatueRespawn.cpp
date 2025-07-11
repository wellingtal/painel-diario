
#include <Network/ClientSocket/Send/SendAttack.h>
#include <Creature/Monster/MonsterMgr.h>

#include "WinInput.h"
#include "GUITextEdit.h"
#include "FrameTimer.h"
#include "BaseGraphicsLayer.h"

#include "RYLCharacterDataManager.h"
#include "RYLMessageBox.h"
#include "RYLStringTable.h"
#include "RYLTimer.h"

#include "RYLClientMain.h"
#include "RYLRaceBase.h"
#include "RYLGameData.h"
#include "RYLNetworkData.h"
#include "RYLCommunityData.h"

#include "RYLChattingDlg.h"

#include "HumanStoneStatueRespawn.h"
#include "GMMemory.h"

// -----------------------------------------------------------------------------------------
// class CHumanStoneStatue

CHumanStoneStatue::CHumanStoneStatue()
{
	m_iCurrentHuman = 0 ;
	m_iCurrentAkhan = 0 ;
}

CHumanStoneStatue::~CHumanStoneStatue() 
{
}

BOOL	CHumanStoneStatue::Init(unsigned short x, unsigned short y)
{
	CRYLSprite*	lpSprite ;

	// 58, 53, 49

	long dwStatueSpriteX, dwHPBarFrameX, dwHPBarX;
	dwStatueSpriteX = g_ClientMain.m_iScreenWidth-58;
	dwHPBarFrameX = g_ClientMain.m_iScreenWidth-53;
	dwHPBarX = g_ClientMain.m_iScreenWidth-49;

	// 중립 정상 : 0 
	lpSprite = new CRYLSprite;
	lpSprite->Create( dwStatueSpriteX, 30, 0, 0, 60, 83, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[ 0 ] ) ;
	m_lstSprite.push_back(lpSprite);
	// 중립 정상-공격중 : 1
	lpSprite = new CRYLSprite;
	lpSprite->Create( dwStatueSpriteX, 30, 60, 0, 120, 83, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[ 0 ] ) ;
	m_lstSprite.push_back(lpSprite);
	// 휴먼 리스폰 : 2
	lpSprite = new CRYLSprite;
	lpSprite->Create( dwStatueSpriteX, 30, 120, 166, 280, 249, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[ 0 ] ) ;
	m_lstSprite.push_back(lpSprite);
	// 휴먼 리스폰-공격중 : 3
	lpSprite = new CRYLSprite;
	lpSprite->Create( dwStatueSpriteX, 30, 180, 166, 240, 249, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[ 0 ] ) ;
	m_lstSprite.push_back(lpSprite);
	// 휴먼 정상 : 4
	lpSprite = new CRYLSprite;
	lpSprite->Create( dwStatueSpriteX, 30, 0, 166, 60, 249, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[ 0 ] ) ;
	m_lstSprite.push_back(lpSprite);
	// 휴먼 정상-공격중 : 5
	lpSprite = new CRYLSprite;
	lpSprite->Create( dwStatueSpriteX, 30, 60, 166, 120, 249, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[ 0 ] ) ;
	m_lstSprite.push_back(lpSprite);
	// 아칸 리스폰 6
	lpSprite = new CRYLSprite;
	lpSprite->Create( dwStatueSpriteX, 30, 120, 83, 180, 166, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[ 0 ] ) ;
	m_lstSprite.push_back(lpSprite);
	// 아칸 리스폰-공격중 : 7
	lpSprite = new CRYLSprite;
	lpSprite->Create( dwStatueSpriteX, 30, 180, 83, 240, 166, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[ 0 ] ) ;
	m_lstSprite.push_back(lpSprite);
	// 아칸 정상 : 8
	lpSprite = new CRYLSprite;
	lpSprite->Create( dwStatueSpriteX, 30, 0, 83, 60, 166, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[ 0 ] ) ;
	m_lstSprite.push_back(lpSprite);
	// 아칸 정상-공격중 : 9
	lpSprite = new CRYLSprite;
	lpSprite->Create( dwStatueSpriteX, 30, 60, 83, 120, 166, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[ 0 ] ) ;
	m_lstSprite.push_back(lpSprite);

	// 중립 정상 : 10
	lpSprite = new CRYLSprite;
	lpSprite->Create( dwStatueSpriteX, 129, 0, 0, 60, 83, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[ 0 ] ) ;
	m_lstSprite.push_back(lpSprite);
	// 중립 정상-공격중 : 11
	lpSprite = new CRYLSprite;
	lpSprite->Create( dwStatueSpriteX, 129, 60, 0, 120, 83, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[ 0 ] ) ;
	m_lstSprite.push_back(lpSprite);
	// 휴먼 리스폰 : 12
	lpSprite = new CRYLSprite;
	lpSprite->Create( dwStatueSpriteX, 129, 120, 166, 280, 249, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[ 0 ] ) ;
	m_lstSprite.push_back(lpSprite);
	// 휴먼 리스폰-공격중 : 13
	lpSprite = new CRYLSprite;
	lpSprite->Create( dwStatueSpriteX, 129, 180, 166, 240, 249, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[ 0 ] ) ;
	m_lstSprite.push_back(lpSprite);
	// 휴먼 정상 : 14
	lpSprite = new CRYLSprite;
	lpSprite->Create( dwStatueSpriteX, 129, 0, 166, 60, 249, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[ 0 ] ) ;
	m_lstSprite.push_back(lpSprite);
	// 휴먼 정상-공격중 : 15
	lpSprite = new CRYLSprite;
	lpSprite->Create( dwStatueSpriteX, 129, 60, 166, 120, 249, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[ 0 ] ) ;
	m_lstSprite.push_back(lpSprite);
	// 아칸 리스폰 16
	lpSprite = new CRYLSprite;
	lpSprite->Create( dwStatueSpriteX, 129, 120, 83, 180, 166, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[ 0 ] ) ;
	m_lstSprite.push_back(lpSprite);
	// 아칸 리스폰-공격중 : 17
	lpSprite = new CRYLSprite;
	lpSprite->Create( dwStatueSpriteX, 129, 180, 83, 240, 166, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[ 0 ] ) ;
	m_lstSprite.push_back(lpSprite);
	// 아칸 정상 : 18
	lpSprite = new CRYLSprite;
	lpSprite->Create( dwStatueSpriteX, 129, 0, 83, 60, 166, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[ 0 ] ) ;
	m_lstSprite.push_back(lpSprite);
	// 아칸 정상-공격중 : 19
	lpSprite = new CRYLSprite;
	lpSprite->Create( dwStatueSpriteX, 129, 60, 83, 120, 166, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[ 0 ] ) ;
	m_lstSprite.push_back(lpSprite);

	
	// 중립 정상 : 20
	lpSprite = new CRYLSprite;
	lpSprite->Create( dwStatueSpriteX, 228, 0, 0, 60, 83, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[ 0 ] ) ;
	m_lstSprite.push_back(lpSprite);
	// 중립 정상-공격중 : 21
	lpSprite = new CRYLSprite;
	lpSprite->Create( dwStatueSpriteX, 228, 60, 0, 120, 83, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[ 0 ] ) ;
	m_lstSprite.push_back(lpSprite);
	// 휴먼 리스폰 : 22
	lpSprite = new CRYLSprite;
	lpSprite->Create( dwStatueSpriteX, 228, 120, 166, 280, 249, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[ 0 ] ) ;
	m_lstSprite.push_back(lpSprite);
	// 휴먼 리스폰-공격중 : 23
	lpSprite = new CRYLSprite;
	lpSprite->Create( dwStatueSpriteX, 228, 180, 166, 240, 249, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[ 0 ] ) ;
	m_lstSprite.push_back(lpSprite);
	// 휴먼 정상 : 24
	lpSprite = new CRYLSprite;
	lpSprite->Create( dwStatueSpriteX, 228, 0, 166, 60, 249, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[ 0 ] ) ;
	m_lstSprite.push_back(lpSprite);
	// 휴먼 정상-공격중 : 25
	lpSprite = new CRYLSprite;
	lpSprite->Create( dwStatueSpriteX, 228, 60, 166, 120, 249, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[ 0 ] ) ;
	m_lstSprite.push_back(lpSprite);
	// 아칸 리스폰 26
	lpSprite = new CRYLSprite;
	lpSprite->Create( dwStatueSpriteX, 228, 120, 83, 180, 166, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[ 0 ] ) ;
	m_lstSprite.push_back(lpSprite);
	// 아칸 리스폰-공격중 : 27
	lpSprite = new CRYLSprite;
	lpSprite->Create( dwStatueSpriteX, 228, 180, 83, 240, 166, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[ 0 ] ) ;
	m_lstSprite.push_back(lpSprite);
	// 아칸 정상 : 28
	lpSprite = new CRYLSprite;
	lpSprite->Create( dwStatueSpriteX, 228, 0, 83, 60, 166, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[ 0 ] ) ;
	m_lstSprite.push_back(lpSprite);
	// 아칸 정상-공격중 : 29
	lpSprite = new CRYLSprite;
	lpSprite->Create( dwStatueSpriteX, 228, 60, 83, 120, 166, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[ 0 ] ) ;
	m_lstSprite.push_back(lpSprite);

	//=============================================================================================
	// 중립 HP Bar Frame : 30
	lpSprite = new CRYLSprite;
	lpSprite->Create( dwHPBarFrameX, 112, 206, 0, 255, 8, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[ 1 ] ) ;
	m_lstSprite.push_back(lpSprite);

	// 중립 HP Bar : 31
	lpSprite = new CRYLSprite;
	lpSprite->Create( dwHPBarX, 114, 206, 8, 247, 12, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[ 1 ] ) ;
	m_lstSprite.push_back(lpSprite);

	// 인간 HP Bar Frame : 32
	lpSprite = new CRYLSprite;
	lpSprite->Create( dwHPBarFrameX, 211, 206, 0, 255, 8, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[ 1 ] ) ;
	m_lstSprite.push_back(lpSprite);

	// 인간 HP Bar : 33
	lpSprite = new CRYLSprite;
	lpSprite->Create( dwHPBarX, 213, 206, 8, 247, 12, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[ 1 ] ) ;
	m_lstSprite.push_back(lpSprite);

	// 아칸 HP Bar Frame : 34
	lpSprite = new CRYLSprite;
	lpSprite->Create( dwHPBarFrameX, 310, 206, 0, 255, 8, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[ 1 ] ) ;
	m_lstSprite.push_back(lpSprite);

	// 아칸 HP Bar : 35
	lpSprite = new CRYLSprite;
	lpSprite->Create( dwHPBarX, 312, 206, 8, 247, 12, CRYLGameData::Instance()->m_lpInterface->m_lstTexture[ 1 ] ) ;
	m_lstSprite.push_back(lpSprite);

	FadeIn( 0 ) ;
	FadeIn( 1 ) ;
	FadeIn( 2 ) ;

	return TRUE ;
}

BOOL	CHumanStoneStatue::Update(BOOL &bClick, BOOL &bEdge)
{
	for ( INT i = 0 ; i < 3 ; i ++ )
	{
		if ( m_StonStatueInfo[ i ].dwFadeMode == FADE_IN && m_StonStatueInfo[ i ].lScreenFade <= FADE_END ) 
			return TRUE ;

		if ( m_StonStatueInfo[ i ].lScreenFade >= FADE_END * 5 )
		{
			m_StonStatueInfo[ i ].dwFadeMode = FADE_IN ;
			m_StonStatueInfo[ i ].lScreenFade = FADE_START ;
			m_StonStatueInfo[ i ].bStoneStatueStateFadeable = FALSE ;
		}
	}

	return TRUE ;
}

void	CHumanStoneStatue::ShowWindow(BOOL bShow) 
{
	if(bShow)
	{
		m_bShow = TRUE;
	} else
	{
		m_bShow = FALSE;
	}
}

void	CHumanStoneStatue::FadeIn( INT Index )	
{		
	m_StonStatueInfo[ Index ].dwFadeMode = FADE_IN ;		
	m_StonStatueInfo[ Index ].lScreenFade = FADE_START ;	
}
void	CHumanStoneStatue::FadeOut( INT Index )	
{	
	m_StonStatueInfo[ Index ].dwFadeMode = FADE_OUT ;		
	m_StonStatueInfo[ Index ].lScreenFade = FADE_END ; 		
}

void	CHumanStoneStatue::Render(LPDIRECT3DDEVICE8 lpD3DDevice)
{
	for ( int i = 0 ; i < 3 ; i ++ )
	{
		if ( m_StonStatueInfo[ i ].bStoneStatueStateFadeable )
		{
			// 석상 리스폰중일때 FadeIn
			if ( ( m_StonStatueInfo[ i ].dwFadeMode == FADE_IN ) &&  ( m_StonStatueInfo[ i ].bRespawnable ) )
			{
				float fUpdate = CFrameTimer::GetUpdateTimer( GRYLTimer.GetShowInterfaceTimerID() ) * 0.15 ;
				m_StonStatueInfo[ i ].lScreenFade += fUpdate ;
				m_lstSprite[ m_StonStatueInfo[ i ].iStoneStatueState ]->Render( lpD3DDevice, ( unsigned char )( ( ( float )m_StonStatueInfo[ i ].lScreenFade / FADE_END ) * 255.0f ) ) ;
			}
			// 석상이 공격 받았을때
			else if ( ( m_StonStatueInfo[ i ].dwFadeMode == FADE_IN ) &&  ( !m_StonStatueInfo[ i ].bRespawnable ) )
			{
				float fUpdate = CFrameTimer::GetUpdateTimer( GRYLTimer.GetShowInterfaceTimerID() ) ;
				m_StonStatueInfo[ i ].lScreenFade += fUpdate ;
				m_lstSprite[ m_StonStatueInfo[ i ].iStoneStatueState ]->Render( lpD3DDevice, ( unsigned char )( ( ( float )m_StonStatueInfo[ i ].lScreenFade / FADE_END ) * 255.0f ) ) ;
			}
		}
		else
		{
			// 석상이 소환 됐을때
			m_lstSprite[ m_StonStatueInfo[ i ].iStoneStatueState ]->Render( lpD3DDevice, m_cAlpha ) ;
		}
	}


	RECT rcRect ;
	// 중립 HP Bar
	m_lstSprite[ 30 ]->Render( lpD3DDevice, m_cAlpha ) ;
	SetRect( &rcRect, 0, 0, m_StonStatueInfo[ 0 ].wNowHP, 4 ) ;
	m_lstSprite[ 31 ]->Render( lpD3DDevice, m_cAlpha, &rcRect ) ;

	// 인간 HP Bar
	m_lstSprite[ 32 ]->Render( lpD3DDevice, m_cAlpha ) ;
	SetRect( &rcRect, 0, 0, m_StonStatueInfo[ 1 ].wNowHP, 4 ) ;
	m_lstSprite[ 33 ]->Render( lpD3DDevice, m_cAlpha, &rcRect ) ;

	// 아칸 HP Bar
	m_lstSprite[ 34 ]->Render( lpD3DDevice, m_cAlpha ) ;
	SetRect( &rcRect, 0, 0, m_StonStatueInfo[ 2 ].wNowHP, 4 ) ;
	m_lstSprite[ 35 ]->Render( lpD3DDevice, m_cAlpha, &rcRect ) ;


	
	char buff[ 128 ] ;

	// 해상도에 맞게 Render
	int iW = g_ClientMain.m_iScreenWidth-66;
	int iH = g_ClientMain.m_iScreenHeight-272;
	// 66, 272
	CRYLGameData::Instance()->RenderRect( lpD3DDevice, iW, iH, iW+2, iH+36, 0xFF000000, 0x80 ) ;

	// 현재 접속현황
	sprintf( buff, CRYLStringTable::m_strString[ 1211 ], m_iCurrentHuman ) ;
	// 62, 266, 56, 12
	iW = g_ClientMain.m_iScreenWidth-62;
	iH = g_ClientMain.m_iScreenHeight-266;
	SetRect( &rcRect, iW, iH, iW+56, iH+12 ) ;
	g_TextEdit.DrawText( buff, &rcRect, DT_RIGHT | DT_VCENTER, D3DCOLOR_RGBA(0, 0, 0, m_cAlpha ) ) ;
	// 63, 267, 56, 12
	iW = g_ClientMain.m_iScreenWidth-63;
	iH = g_ClientMain.m_iScreenHeight-267;
	SetRect( &rcRect, iW, iH, iW+56, iH+12 ) ;
	g_TextEdit.DrawText( buff, &rcRect, DT_RIGHT | DT_VCENTER, D3DCOLOR_RGBA(255, 255, 255, m_cAlpha ) ) ;

	sprintf( buff, CRYLStringTable::m_strString[ 1212 ], m_iCurrentAkhan ) ;
	// 62, 251, 56, 12
	iW = g_ClientMain.m_iScreenWidth-62;
	iH = g_ClientMain.m_iScreenHeight-251;
	SetRect( &rcRect, iW, iH, iW+56, iH+12 ) ;
	g_TextEdit.DrawText( buff, &rcRect, DT_RIGHT | DT_VCENTER, D3DCOLOR_RGBA(0, 0, 0, m_cAlpha ) ) ;
	// 63, 252, 56, 12
	iW = g_ClientMain.m_iScreenWidth-63;
	iH = g_ClientMain.m_iScreenHeight-252;
	SetRect( &rcRect, iW, iH, iW+56, iH+12 ) ;
	g_TextEdit.DrawText( buff, &rcRect, DT_RIGHT | DT_VCENTER, D3DCOLOR_RGBA(255, 255, 255, m_cAlpha ) ) ;
}

VOID	CHumanStoneStatue::SetStoneStatueState( INT Index, 
												INT iKind, 
												INT iState, 
												INT wNowHP, 
												DWORD dwMileage, 
												WORD wHumanNum, 
												WORD wAkhanNum ) 
{
	CRYLChattingDlg* pChat = CRYLChattingDlg::Instance() ;

	// 마일리지
	if ( dwMileage != 0 )
	{
        RYLCreature*    pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
        if( !pSelfCreature )
            return;

        CharDataNode*   pSelfData = RYLCharacterDataManager::Instance()->GetCharData( pSelfCreature->GetCharID() );
        if( !pSelfData )
            return;

        pSelfData->m_ulMileage = CRYLGameData::Instance()->m_csStatus.m_Info.Mileage = dwMileage;
        
		// 128 연두색
		// 129 빨간색
		pChat->AddMessage( CRYLStringTable::m_strString[ 1210 ], CRYLNetworkData::Instance()->m_dwMyChrID, 128 ) ;
	}
	// 인간 : 아칸 Text
	m_iCurrentHuman = ( INT )wHumanNum ;
	m_iCurrentAkhan = ( INT )wAkhanNum ;
    	
	const CMonsterMgr::MonsterProtoType *lpMonster = g_MonsterMgr.GetMonsterProtoType( iKind ) ;
	if(lpMonster)
	{
		if ( lpMonster && lpMonster->m_MonsterInfo.m_cSkillPattern == MonsterInfo::MonsterPattern::PATTERN_STRUCTURE )
		{
			// 현제 HP 100분율		
			m_StonStatueInfo[ Index ].wNowHP = ( INT )( ( ( ( FLOAT )wNowHP / lpMonster->m_CreatureStatus.m_StatusInfo.m_nMaxHP ) ) * 41) ;
		}
	}

/*
	1034 : 중립
	1035 : 인간 소환
	1036 : 인간
	1037 : 아칸 소환
	1038 : 아칸

	1 : Normal
	2 : Attack
*/
	switch ( iKind )
	{
		case MonsterInfo::STATUE_NEUTRALITY1 : 
		case MonsterInfo::STATUE_NEUTRALITY2 : 
			if ( iState == 1 )			
				m_StonStatueInfo[ Index ].iStoneStatueState = ( Index * 10 ) + 0 ; //NEUTRALITY_NORMAL ;
			else if ( iState == 2 )		
				m_StonStatueInfo[ Index ].iStoneStatueState = ( Index * 10 ) + 1 ; //NEUTRALITY_ATTACKING ;
			break ;

		case MonsterInfo::STATUE_HUMAN_LOADING1 : 
		case MonsterInfo::STATUE_HUMAN_LOADING2 : 
			if ( iState == 1 )			
				m_StonStatueInfo[ Index ].iStoneStatueState = ( Index * 10 ) + 2 ; //HUMAN_RESPAWN ;
			else if ( iState == 2 )		
				m_StonStatueInfo[ Index ].iStoneStatueState = ( Index * 10 ) + 3 ; //HUMAN_RESPAWN_ATTACKING ;
			break ;

		case MonsterInfo::STATUE_HUMAN_COMPLETE1 : 
		case MonsterInfo::STATUE_HUMAN_COMPLETE2 : 
			if ( iState == 1 )			
				m_StonStatueInfo[ Index ].iStoneStatueState = ( Index * 10 ) + 4 ; //HUMAN_NORMAL ;
			else if ( iState == 2 )		
				m_StonStatueInfo[ Index ].iStoneStatueState = ( Index * 10 ) + 5 ; //HUMAN_ATTACKING ;
			break ;

		case MonsterInfo::STATUE_AKHAN_LOADING1 : 
		case MonsterInfo::STATUE_AKHAN_LOADING2 : 
			if ( iState == 1 )			
				m_StonStatueInfo[ Index ].iStoneStatueState = ( Index * 10 ) + 6 ; //AKHAN_RESPAWN ;
			else if ( iState == 2 )		
				m_StonStatueInfo[ Index ].iStoneStatueState = ( Index * 10 ) + 7 ; //AKHAN_RESPAWN_ATTACKING ;
			break ;

		case MonsterInfo::STATUE_AKHAN_COMPLETE1 : 
		case MonsterInfo::STATUE_AKHAN_COMPLETE2 : 
			if ( iState == 1 )			
				m_StonStatueInfo[ Index ].iStoneStatueState = ( Index * 10 ) + 8 ; //AKHAN_NORMAL ;
			else if ( iState == 2 )		
				m_StonStatueInfo[ Index ].iStoneStatueState = ( Index * 10 ) + 9 ; //AKHAN_ATTACKING ;
			break ;
	}

	switch( m_StonStatueInfo[ Index ].iStoneStatueState )
	{
		case NEUTRALITY_ATTACKING :		case NEUTRALITY_ATTACKING + 10 :	case NEUTRALITY_ATTACKING + 20 :
		case HUMAN_ATTACKING :			case HUMAN_ATTACKING + 10 :			case HUMAN_ATTACKING + 20 :
		case HUMAN_RESPAWN_ATTACKING :	case HUMAN_RESPAWN_ATTACKING + 10 : case HUMAN_RESPAWN_ATTACKING + 20 :
		case AKHAN_ATTACKING :			case AKHAN_ATTACKING + 10 :			case AKHAN_ATTACKING + 20 :
		case AKHAN_RESPAWN_ATTACKING :	case AKHAN_RESPAWN_ATTACKING + 10 : case AKHAN_RESPAWN_ATTACKING + 20 :
		{
			m_StonStatueInfo[ Index ].bStoneStatueStateFadeable = TRUE ;
			break ;
		}

		case HUMAN_RESPAWN : case HUMAN_RESPAWN + 10 : case HUMAN_RESPAWN + 20 :
		case AKHAN_RESPAWN : case AKHAN_RESPAWN + 10 : case AKHAN_RESPAWN + 20 :
		{
			m_StonStatueInfo[ Index ].bRespawnable = TRUE ;
			m_StonStatueInfo[ Index ].bStoneStatueStateFadeable = TRUE ;
			break ;
		}

		case NEUTRALITY_NORMAL : case NEUTRALITY_NORMAL + 10 : case NEUTRALITY_NORMAL + 20 :
		case HUMAN_NORMAL :      case HUMAN_NORMAL + 10 :      case HUMAN_NORMAL + 20 :
		case AKHAN_NORMAL :      case AKHAN_NORMAL + 10 :      case AKHAN_NORMAL + 20 :
		{
			m_StonStatueInfo[ Index ].bRespawnable = FALSE ;
			m_StonStatueInfo[ Index ].bStoneStatueStateFadeable = FALSE ;
			break ;
		}
	}
}

