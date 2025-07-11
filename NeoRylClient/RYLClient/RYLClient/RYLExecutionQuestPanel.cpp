#include <winsock2.h>
#include "RYLExecutionQuestPanel.h"
#include "RYLButton.h"
#include "RYLLabel.h"
#include "RYLImage.h"
#include "RYLImageManager.h"
#include "GMImageDib.h"
#include "RYLImageCabinet.h"
#include "RYLSpriteList.h"
#include "RYLStringTable.h"

#include "RYLNetworkData.h"
#include "RYLGameData.h"
#include "RYLGameScene.h"
#include "RYLSceneManager.h"
#include "RYLLargeMapFrameDlg.h"
#include "RYLQuestDetailDataDlg.h"

#include "RYLMessageBox.h"
#include "RYLMessageBoxManager.h"

#include <Network/ClientSocket/Send/SendQuest.h>
#include "GMMemory.h"

//-----------------------------------------------------------------------------
// QuestSlot - Map Button
CRYLExeQuestMapButton::CRYLExeQuestMapButton( CGMUICustomDlg *pParentDlg ) : CRYLButton( pParentDlg )
{
	m_bAlwaysHide = FALSE ;
}

CRYLExeQuestMapButton::~CRYLExeQuestMapButton() 
{
}

VOID CRYLExeQuestMapButton::SetVisible( BOOL bVisible )
{
	if ( m_bAlwaysHide )
	{
		CGMUICaptionButton::SetVisible( FALSE ) ;
	}
	else
	{
		CGMUICaptionButton::SetVisible( bVisible ) ;
	}
}

//-----------------------------------------------------------------------------
// Quest Slot
CRYLExecutionQuestSlot*		g_ExecutionQuestSlot = NULL ;
CRYLExecutionQuestSlot::CRYLExecutionQuestSlot( CGMUICustomDlg *pParentDlg ) : CGMUICustomPanel( pParentDlg )
{
	m_pGMUIParentDlg		= pParentDlg ;
	g_ExecutionQuestSlot	= this ;

	m_pQuestTitleLabel		= NULL ;			// 퀘스트 제목
	m_pQuestLevelLabel		= NULL ;			// 퀘스트 레벨
	m_pQuestMapButton		= NULL ;			// 퀘스트 멥
	m_pQuestTextButton		= NULL ;			// 퀘스트 설명 버튼
	m_pQuestQuitButton		= NULL ;			// 퀘스트 종료
	m_iQuestSlotIndex		= NULL ;			// 퀘스트슬롯 인덱스
}

CRYLExecutionQuestSlot::~CRYLExecutionQuestSlot() 
{
	GM_DELETE( m_pQuestTitleLabel ) ;			// 퀘스트 제목
	GM_DELETE( m_pQuestLevelLabel ) ;			// 퀘스트 레벨
	GM_DELETE( m_pQuestMapButton ) ;			// 퀘스트 멥
	GM_DELETE( m_pQuestTextButton ) ;			// 퀘스트 설명 버튼
	GM_DELETE( m_pQuestQuitButton ) ;			// 퀘스트 종료
}

VOID		CRYLExecutionQuestSlot::MakeExeQuestSlot() 
{
	RECT			rt ;
	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;

	// Background Image
	SetWidth( 233 ) ;
	SetHeight( 42 ) ;
	CRYLSpriteList::Instance()->CreateGMImage( 233, 42, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;

	SetRect( &rt, 0, 0, 233, 42 ) ;
	
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_QUESTSLOT_256x256 ) ) ;
	
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	AttachPanelImage( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;
}

VOID		CRYLExecutionQuestSlot::CreateExeQuestTitle( LPSTR lpQuestTitle ) 
{
	m_pQuestTitleLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pQuestTitleLabel->SetLeft( 9 ) ;
	m_pQuestTitleLabel->SetTop( 42 ) ;
	m_pQuestTitleLabel->SetFontColor( 0xffffffff ) ;
	m_pQuestTitleLabel->SetAutoSize( TRUE ) ;
	m_pQuestTitleLabel->SetCaption( lpQuestTitle ) ;
	CGMUICustomPanel::AddChild( m_pQuestTitleLabel ) ;
}

VOID		CRYLExecutionQuestSlot::CreateQuestLevelAndPhase( char* szLevel, INT iCurPage, INT iTotalPage ) 
{
//	CHAR szTemp[ MAX_PATH ];
//	sprintf( szTemp, CRYLStringTable::m_strString[ 1925 ], (int)( (iCurPage - 1) * 100.0f / iTotalPage ) ) ;

	CHAR szLevelAndPhase[ MAX_PATH ] ;
	// edith 2008.03.21 퀘스트 진행 퍼센트 표시 변경
//	sprintf( szLevelAndPhase, "%s  %s", szLevel, szTemp ) ;
	sprintf( szLevelAndPhase, "%s (%d/%d)", szLevel, iCurPage, iTotalPage) ;

	// 퀘스트 레벨
	m_pQuestLevelLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pQuestLevelLabel->SetLeft( 9 ) ;
	m_pQuestLevelLabel->SetTop( 59 ) ;
	m_pQuestLevelLabel->SetFontColor( 0xfffff8b9 ) ;
	m_pQuestLevelLabel->SetAutoSize( TRUE ) ;
	m_pQuestLevelLabel->SetCaption( szLevelAndPhase ) ;
	CGMUICustomPanel::AddChild( m_pQuestLevelLabel ) ;
}

VOID		CRYLExecutionQuestSlot::SetQuestLevelAndPhase( char* szLevel, INT iCurPage, INT iTotalPage ) 
{
//	CHAR szTemp[ MAX_PATH ];
//	sprintf( szTemp, CRYLStringTable::m_strString[ 1925 ], (int)( (iCurPage - 1)  * 100.0f / iTotalPage ) ) ;

	CHAR szLevelAndPhase[ MAX_PATH ] ;
	// edith 2008.03.21 퀘스트 진행 퍼센트 표시 변경
//	sprintf( szLevelAndPhase, "%s  %s", szLevel, szTemp ) ;
	sprintf( szLevelAndPhase, "%s (%d/%d)", szLevel, iCurPage, iTotalPage) ;

	m_pQuestLevelLabel->SetCaption( szLevelAndPhase ) ;
}

const CHAR*	CRYLExecutionQuestSlot::GetQuestTitle()
{
	return m_pQuestTitleLabel->GetCaption() ;
}
//-----------------------------------------------------------------------------------------

CRYLExecutionQuestPanel*	g_ExecutionQuestPanel = NULL ;

CRYLExecutionQuestPanel::CRYLExecutionQuestPanel( CGMUICustomDlg *pParentDlg ) : CGMUICustomPanel( pParentDlg )
{
	m_pGMUIParentDlg = pParentDlg ;
	g_ExecutionQuestPanel = this ;

	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;
	RECT			rt ;

	CRYLSpriteList::Instance()->CreateGMImage( 256, 382, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	int i = 0 ;
	for ( ; i < 74 ; i ++ )
	{
		SetRect( &rt, 0, 38, 256, 43 ) ;
		pImageDIB->DrawRect( &rt, 0, 1 + (i * 5), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	}
	SetRect( &rt, 0, 42, 256, 52 ) ;
	pImageDIB->DrawRect( &rt, 0, ( 1 +(i-1) * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	
	pImageDIB->MakeFrameRect( 6,  3, 243, 344, 0xffB99E6B, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect( 167, 351, 68, 18, 0xff959595, 0xc0000000,  1 ) ;

	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	AttachPanelImage( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;
	
	SetWidth( 256 ) ;
	SetHeight( 375 ) ;

	m_iQuestYMargin			= 0 ; 
	m_iQuestButtonIndex		= 0 ;
	m_dwStartSlot			= 0 ;
	m_dwDeleteQuestResult	= 0 ;

	m_lStartQuest			= 0;
	m_lStartDesc			= 0;
	m_lStartDone			= 0;
	m_dwDeleteQuestResult	= 0;
	m_lpDeleteQuest			= NULL;
	m_lpSelectQuest			= NULL;

	m_dwSelectQuestSlot		= 0xFFFFFFFF;

	m_dwProcessQuestTextIndex	= -1 ;
	m_dwProcessQuestMapIndex	= -1 ;
}

CRYLExecutionQuestPanel::~CRYLExecutionQuestPanel() 
{
	FinalCtrl() ;
}

VOID	CRYLExecutionQuestPanel::InitDesc() 
{
	vector<LPMessage>::iterator it ;
	LPMessage					lpMessage ;
	
	for ( it = m_lstMessage.begin() ; it != m_lstMessage.end() ; )
	{
		lpMessage = ( *it ) ;
		it = m_lstMessage.erase( it ) ;
		GM_DELETE( lpMessage ) ;
	}
	m_lstMessage.clear() ;
}

VOID	CRYLExecutionQuestPanel::InitValue() 
{
	m_lStartQuest		= 0;
	m_lStartDesc		= 0;
	m_lStartDone		= 0;
	m_dwDeleteQuestResult = 0;
	m_lpDeleteQuest = NULL;
	m_lpSelectQuest = NULL;

	m_dwSelectQuestSlot = 0xFFFFFFFF;

	m_dwQuestWindowMode = 0;
	
	m_dwProcessQuestTextIndex	= -1 ;
	m_dwProcessQuestMapIndex	= -1 ;
}

VOID	CRYLExecutionQuestPanel::InitCtrl() 
{
	SetWidth( 256 ) ;
	SetHeight( 375 ) ;
	RECT rt ;
	CGMImageDib* pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ;

	m_pPrevButton = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pPrevButton->SetLeft( 152 ) ;
	m_pPrevButton->SetTop( 354 ) ;
	m_pPrevButton->OnClickEvent = DlgClick ;
	m_pPrevButton->SetInstance( RYLEXECUTIONQUESTDLG::PREV_BUTTON ) ;
	SetRect( &rt, 217, 215, 230, 227 ) ;
	m_pPrevButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 217, 243, 230, 255 ) ;
	m_pPrevButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 217, 229, 230, 241 ) ;
	m_pPrevButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 217, 215, 230, 227 ) ;
	m_pPrevButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	CGMUICustomPanel::AddChild( m_pPrevButton ) ;

	m_pNextButton = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pNextButton->SetLeft( 237 ) ;
	m_pNextButton->SetTop( 354 ) ;
	m_pNextButton->OnClickEvent = DlgClick ;
	m_pNextButton->SetInstance( RYLEXECUTIONQUESTDLG::NEXT_BUTTON ) ;
	SetRect( &rt, 232, 215, 245, 227 ) ;
	m_pNextButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 232, 243, 245, 255 ) ;
	m_pNextButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 232, 229, 245, 241 ) ;
	m_pNextButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 232, 215, 245, 227 ) ;
	m_pNextButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	CGMUICustomPanel::AddChild( m_pNextButton ) ;

	// 현재 페이지 상황값
	m_pPageState = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pPageState->SetLeft( 179 ) ;
	m_pPageState->SetTop( 355 ) ;
	m_pPageState->SetFontColor (0xffffffff);
	m_pPageState->SetAutoSize( TRUE ) ;
	m_pPageState->SetCaption( "" ) ;
	CGMUICustomPanel::AddChild( m_pPageState ) ;
}

VOID	CRYLExecutionQuestPanel::FinalCtrl() 
{
	GM_DELETE( m_pPrevButton ) ;
	GM_DELETE( m_pNextButton ) ;
	GM_DELETE( m_pPageState ) ;

	ClearSlot() ;
}

VOID	CRYLExecutionQuestPanel::ClearSlot()
{
	for ( std::list< CRYLExecutionQuestSlot* >::iterator itSlot = m_vecQuestSlot.begin() ; itSlot != m_vecQuestSlot.end() ; ++ itSlot )
	{
		CGMUICustomPanel::DeleteChild( (*itSlot ) ) ;
		CGMUICustomPanel::DeleteChild( (*itSlot )->m_pQuestMapButton ) ;
		CGMUICustomPanel::DeleteChild( (*itSlot )->m_pQuestTextButton ) ;
		CGMUICustomPanel::DeleteChild( (*itSlot )->m_pQuestQuitButton ) ;

		GM_DELETE( (*itSlot) ) ;
	}
	m_vecQuestSlot.clear() ;

	m_lpDeleteQuest = NULL ;
	m_lpSelectQuest = NULL ;
}

VOID	CRYLExecutionQuestPanel::InsertExecutionQuest( QuestInstance* lpQuest ) 
{
	CGMImageDib* pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_QUESTSLOT_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;

	CRYLExecutionQuestSlot* pQuest = new CRYLExecutionQuestSlot( m_pGMUIParentDlg ) ;
	pQuest->SetLeft( 11 ) ;
	pQuest->SetTop( m_iQuestYMargin + 8 ) ;

	RECT rt ;

	pQuest->m_pQuestMapButton = new CRYLExeQuestMapButton( m_pGMUIParentDlg ) ;
	pQuest->m_pQuestMapButton->SetLeft( 172 ) ;
//	pQuest->m_pQuestMapButton->SetLeft( 200 ) ;
	pQuest->m_pQuestMapButton->SetTop( 15 + m_iQuestYMargin ) ;
	pQuest->m_pQuestMapButton->OnClickEvent = DlgClick ;
	pQuest->m_pQuestMapButton->SetInstance( RYLEXECUTIONQUESTDLG::MAP_BUTTON ) ;

	SetRect( &rt, 28, 85, 53, 110 ) ;
	pQuest->m_pQuestMapButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 56, 85, 81, 110 ) ;
	pQuest->m_pQuestMapButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 84, 85, 109, 110 ) ;
	pQuest->m_pQuestMapButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 28, 85, 53, 110 ) ;
	pQuest->m_pQuestMapButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	pQuest->m_pQuestMapButton->SetQuestIndex( m_iQuestButtonIndex ) ;
	CGMUICustomPanel::AddChild( pQuest->m_pQuestMapButton ) ;

	pQuest->m_pQuestTextButton = new CRYLExeQuestMapButton( m_pGMUIParentDlg ) ;
	pQuest->m_pQuestTextButton->SetLeft( 200 ) ;
//	pQuest->m_pQuestTextButton->SetLeft( 172 ) ;
	pQuest->m_pQuestTextButton->SetTop( 15 + m_iQuestYMargin ) ;
	pQuest->m_pQuestTextButton->SetFontColor( 0xffffffff ) ;
	pQuest->m_pQuestTextButton->SetColor( 0xffffffff ) ;
	pQuest->m_pQuestTextButton->OnClickEvent = DlgClick ;
	pQuest->m_pQuestTextButton->SetInstance( RYLEXECUTIONQUESTDLG::TEXT_BUTTON ) ;
	SetRect( &rt, 112, 85, 137, 110 ) ;
	pQuest->m_pQuestTextButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 140, 85, 165, 110 ) ;
	pQuest->m_pQuestTextButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 168, 85, 193, 110 ) ;
	pQuest->m_pQuestTextButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 112, 85, 137, 110 ) ;
	pQuest->m_pQuestTextButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	pQuest->m_pQuestTextButton->SetQuestIndex( m_iQuestButtonIndex ) ;
	CGMUICustomPanel::AddChild( pQuest->m_pQuestTextButton ) ;

	pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI_256x256 ) ;

	pQuest->m_pQuestQuitButton = new CRYLExeQuestMapButton( m_pGMUIParentDlg ) ;
	pQuest->m_pQuestQuitButton->SetLeft( 230 ) ;
	pQuest->m_pQuestQuitButton->SetTop( 11 + m_iQuestYMargin ) ;
	pQuest->m_pQuestQuitButton->OnClickEvent = DlgClick ;
	pQuest->m_pQuestQuitButton->SetInstance( RYLEXECUTIONQUESTDLG::QUIT_BUTTON ) ;
	//SetRect( &rt, 202, 0, 212, 9 ) ;hz guigui
	SetRect( &rt, 4, 347, 15, 356 ) ;
	pQuest->m_pQuestQuitButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	//SetRect( &rt, 202, 18, 212, 27 ) ;
	SetRect( &rt, 4, 267, 15, 276 ) ;
	pQuest->m_pQuestQuitButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	//SetRect( &rt, 202, 9, 212, 18 ) ;
	SetRect( &rt, 4, 257, 15, 266 ) ;
	pQuest->m_pQuestQuitButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	//SetRect( &rt, 202, 9, 212, 18 ) ;
	SetRect( &rt, 4, 257, 15, 266 ) ;
	pQuest->m_pQuestQuitButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	pQuest->m_pQuestQuitButton->SetQuestIndex( m_iQuestButtonIndex ) ;
	CGMUICustomPanel::AddChild( pQuest->m_pQuestQuitButton ) ;

	if (lpQuest->m_dwQuestID & QuestInstance::DISABLE_CANCEL_QUEST_BIT)
	{
		pQuest->SetVisible(FALSE);

		pQuest->m_pQuestQuitButton->SetAlwaysHide( TRUE ) ;
		pQuest->m_pQuestQuitButton->SetVisible( FALSE ) ;
	}

	pQuest->CreateExeQuestTitle( lpQuest->m_lpQuestScript->m_strQuestTitle ) ;
	pQuest->CreateQuestLevelAndPhase( lpQuest->m_lpQuestScript->m_strQuestLevel, 
		lpQuest->m_dwPresentPhase, lpQuest->m_lpQuestScript->m_wMaxPhase ) ;
	pQuest->MakeExeQuestSlot() ;
	pQuest->SetQuestSlotIndex( m_iQuestButtonIndex ) ;
	CGMUICustomPanel::AddChild( pQuest ) ;

	m_iQuestYMargin += 41 ;
	++ m_iQuestButtonIndex ;

	m_vecQuestSlot.push_front( pQuest ) ;

	QuestSlotVisible() ;
}

VOID	CRYLExecutionQuestPanel::QuestSlotVisible()
{
	UpdateQuestSlotPosition() ;

	INT iCount = 0 ;
	CHAR strTemp[ MAX_PATH ] ;
	std::list< CRYLExecutionQuestSlot* >::iterator it = FindIter( m_dwStartSlot ) ;
	CRYLExecutionQuestSlot* lpExeQuestSlot = NULL ;

	// 현재 페이지가 처음이 아닐때.. 이전 페널들을 숨긴다.
	if ( m_dwStartSlot >= 8 )
	{
		for ( std::list< CRYLExecutionQuestSlot* >::iterator it = m_vecQuestSlot.begin() ; it != m_vecQuestSlot.end() ; ++ it )
		{
			if ( ( *it ) == FindPtr( m_dwStartSlot ) )
				break ;
			else
			{
				( *it )->SetVisible( FALSE ) ;
				( *it )->m_pQuestMapButton->SetVisible( FALSE ) ;
				( *it )->m_pQuestTextButton->SetVisible( FALSE ) ;
				( *it )->m_pQuestQuitButton->SetVisible( FALSE ) ;
			}
		}
	}

	for ( std::list< CRYLExecutionQuestSlot* >::iterator it = FindIter( m_dwStartSlot ) ; it != m_vecQuestSlot.end() ; ++ it )
	{
		if ( iCount >= 8 )
		{
			( *it )->SetVisible( FALSE ) ;
			( *it )->m_pQuestMapButton->SetVisible( FALSE ) ;
			( *it )->m_pQuestTextButton->SetVisible( FALSE ) ;
			( *it )->m_pQuestQuitButton->SetVisible( FALSE ) ;
		}
		else
		{
			( *it )->SetVisible( TRUE ) ;
			( *it )->m_pQuestMapButton->SetVisible( TRUE ) ;
			( *it )->m_pQuestTextButton->SetVisible( TRUE ) ;
			( *it )->m_pQuestQuitButton->SetVisible( TRUE ) ;
		}

		++ iCount ;
	}

	if ( m_vecQuestSlot.empty() )
	{
		sprintf( strTemp, " %2d / %2d", 1, 1 ) ;
	}
	else
	{
		sprintf( strTemp, " %2d / %2d", m_dwStartSlot / 8 + 1, ( m_vecQuestSlot.size() - 1 ) / 8 + 1 ) ;
	}

	int iLength = CRYLStringTable::GetStringWidth( strTemp ) ;
	m_pPageState->SetLeft( 198 - iLength / 2 ) ;
	m_pPageState->SetCaption( strTemp ) ;
}

VOID		CRYLExecutionQuestPanel::UpdateQuestSlotPosition()
{
	if ( m_vecQuestSlot.empty() ) return ;

	INT iTempY = 0, iTempIndex = 0 ;
	INT	iCount = 0 ;
	
	for ( std::list< CRYLExecutionQuestSlot* >::iterator it = m_vecQuestSlot.begin() ; it != m_vecQuestSlot.end() ; ++ it )
	{
		if ( ( iCount % 8 ) == 0 )
		{
			iTempY = 0 ;
		}

		( *it )->SetTop( iTempY + 13 ) ;
		( *it )->m_pQuestMapButton->SetTop( iTempY + 20 ) ;
		( *it )->m_pQuestTextButton->SetTop( iTempY + 20 ) ;
		( *it )->m_pQuestQuitButton->SetTop( iTempY + 17 ) ;
		
		( *it )->m_pQuestMapButton->SetQuestIndex( iTempIndex ) ;
		( *it )->m_pQuestTextButton->SetQuestIndex( iTempIndex ) ;
		( *it )->m_pQuestQuitButton->SetQuestIndex( iTempIndex ) ;
		( *it )->SetQuestSlotIndex( iTempIndex ) ;
		
		iTempY += 41 ;
		++ iTempIndex ;
		++ iCount ;
	}
	m_iQuestYMargin = iTempY ;
	m_iQuestButtonIndex = iTempIndex ;
}

HRESULT		CRYLExecutionQuestPanel::Update( BOOL &bClick, BOOL &bEdge ) 
{
	//g_ExecutionQuestPanel = this ;

	// 퀘스트 삭제
	if ( m_dwDeleteQuestResult & MB_YES )
	{
		SendPacket::CharCancelQuest( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, m_lpDeleteQuest->m_dwQuestID ) ;

		HideDetailDataDlg( m_lpDeleteQuest ) ;

		m_dwDeleteQuestResult = 0;
		m_lpDeleteQuest = NULL;
	} 
	else if(m_dwDeleteQuestResult & MB_NO || m_dwDeleteQuestResult & MB_NO)
	{
		m_dwDeleteQuestResult = 0;
		m_lpDeleteQuest = NULL;
	}

	return S_OK ;
}

HRESULT		CRYLExecutionQuestPanel::Render( LPDIRECT3DDEVICE8 lpD3DDevice )  
{
	return S_OK ;
}

HRESULT		CRYLExecutionQuestPanel::FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) 
{
	return S_OK ;
}

VOID	CRYLExecutionQuestPanel::PrevQuestSlotPage() 
{
	if ( m_dwStartSlot != 0 )
	{
		SetStartSlot( m_dwStartSlot -= 8 ) ;
		QuestSlotVisible() ;
	}
}

VOID	CRYLExecutionQuestPanel::NextQuestSlotPage() 
{
	if ( m_vecQuestSlot.size() - m_dwStartSlot > 8 )
	{
		SetStartSlot( m_dwStartSlot += 8 ) ;
		QuestSlotVisible() ;
	}
}

std::list< CRYLExecutionQuestSlot* >::iterator CRYLExecutionQuestPanel::FindIter( INT iIndex ) 
{
	INT iCount = 0 ;
	
	std::list< CRYLExecutionQuestSlot* >::iterator it ;
	for ( it = m_vecQuestSlot.begin() ; it != m_vecQuestSlot.end() ; ++ it )
	{
		if ( iCount == iIndex )
		{
			return it ;
		}
		++ iCount ;
	}

	return m_vecQuestSlot.end() ;
}

std::list< CRYLExecutionQuestSlot* >::iterator CRYLExecutionQuestPanel::FindIter( const CHAR* strTitle )
{
	if ( NULL == strTitle ) return m_vecQuestSlot.end() ;

	std::list< CRYLExecutionQuestSlot* >::iterator it ;
	for ( it = m_vecQuestSlot.begin() ; it != m_vecQuestSlot.end() ; ++ it )
	{
		if ( !strcmp( (*it)->GetQuestTitle(), strTitle ) )
		{
			return it ;
		}
	}

	return m_vecQuestSlot.end() ;
}

CRYLExecutionQuestSlot*				   CRYLExecutionQuestPanel::FindPtr( INT iIndex ) 
{
	INT iCount = 0 ;
	
	std::list< CRYLExecutionQuestSlot* >::iterator it ;
	for ( it = m_vecQuestSlot.begin() ; it != m_vecQuestSlot.end() ; ++ it )
	{
		if ( iCount == iIndex )
		{
			return ( *it ) ;
		}
		++ iCount ;
	}

	return NULL ;
}

CRYLExecutionQuestSlot*					CRYLExecutionQuestPanel::FindPtr( const CHAR* strTitle )
{
	if ( NULL == strTitle ) return NULL ;

	std::list< CRYLExecutionQuestSlot* >::iterator it ;
	for ( it = m_vecQuestSlot.begin() ; it != m_vecQuestSlot.end() ; ++ it )
	{
		if ( !strcmp( (*it)->GetQuestTitle(), strTitle ) )
		{
			return ( *it ) ;
		}
	}

	return NULL ;
}

VOID	CRYLExecutionQuestPanel::DeleteIter( INT iIndex )
{
	INT iCount = 0 ;
	
	std::list< CRYLExecutionQuestSlot* >::iterator it ;
	for ( it = m_vecQuestSlot.begin() ; it != m_vecQuestSlot.end() ; ++ it )
	{
		if ( iCount == iIndex )
		{
			CGMUICustomPanel::DeleteChild( (*it ) ) ;
			CGMUICustomPanel::DeleteChild( (*it )->m_pQuestMapButton ) ;
			CGMUICustomPanel::DeleteChild( (*it )->m_pQuestTextButton ) ;
			CGMUICustomPanel::DeleteChild( (*it )->m_pQuestQuitButton ) ;
			m_vecQuestSlot.erase( it ) ;
			QuestSlotVisible() ;
			return ;
		}
		++ iCount ;
	}

	QuestSlotVisible() ;
}

VOID	CRYLExecutionQuestPanel::SetProcessQuestTextIndex( INT iIndex )	
{ 
	list<LPQuestInstance>::iterator it ;
	INT iCount = 0 ;
	
	for ( it = CRYLGameData::Instance()->m_csStatus.m_lstQuest.begin() ; it != CRYLGameData::Instance()->m_csStatus.m_lstQuest.end() ; ++ it )
	{
		if ( iCount == iIndex )
		{
			m_lpDeleteQuest = m_lpSelectQuest = ( *it ) ;
			m_dwProcessQuestTextIndex = iIndex ; 
			return ;
		}
		++ iCount ;
	}
	m_dwProcessQuestTextIndex = iIndex ; 
}

VOID	CRYLExecutionQuestPanel::SetProcessQuestMapIndex( INT iIndex )	
{ 
	list<LPQuestInstance>::iterator it ;
	INT iCount = 0 ;

	for ( it = CRYLGameData::Instance()->m_csStatus.m_lstQuest.begin() ; it != CRYLGameData::Instance()->m_csStatus.m_lstQuest.end() ; ++ it )
	{
		if ( iCount == iIndex )
		{
			m_lpDeleteQuest = m_lpSelectQuest = ( *it ) ;
			m_dwProcessQuestMapIndex = iIndex ; 
			return ;
		}
		++ iCount ;
	}
	m_dwProcessQuestMapIndex = iIndex ; 
}

VOID	CRYLExecutionQuestPanel::SetProcessQuestQuitIndex( INT iIndex )
{ 
	list<LPQuestInstance>::iterator it ;
	INT iCount = 0 ;

	for ( it = CRYLGameData::Instance()->m_csStatus.m_lstQuest.begin() ; it != CRYLGameData::Instance()->m_csStatus.m_lstQuest.end() ; ++ it )
	{
		if ( iCount == iIndex )
		{
			m_lpDeleteQuest = m_lpSelectQuest = ( *it ) ;
			return ;
		}
		++ iCount ;
	}
}

VOID	CRYLExecutionQuestPanel::ShowDetailDataDlg() 
{
	CRYLGameScene*		  pScene    = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
	CRYLQuestDetailDataDlg* pDlg	= ( CRYLQuestDetailDataDlg* )pScene->GetQuestDetailDataDlg() ;
	pDlg->SetDetailDataDlg( m_lpSelectQuest ) ;
	pDlg->SetVisible( TRUE ) ;	
}

VOID	CRYLExecutionQuestPanel::HideDetailDataDlg( QuestInstance* lpQuest )
{
	if ( lpQuest )
	{
		CRYLGameScene*		  pScene    = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
		CRYLQuestDetailDataDlg* pDlg	= ( CRYLQuestDetailDataDlg* )pScene->GetQuestDetailDataDlg() ;
		
		if ( !strcmp(lpQuest->m_lpQuestScript->m_strQuestTitle, pDlg->GetQuestTitle()) )
		{
			pDlg->SetQuestTitle( "" ) ;
			pDlg->SetVisible( FALSE ) ;
		}
	}
}

VOID	CRYLExecutionQuestPanel::DlgClick( CGMUIObject* Sender, INT X, INT Y ) 
{
	if ( !g_ExecutionQuestPanel )
		return ;

	switch( Sender->GetInstance() )
	{
	case RYLEXECUTIONQUESTDLG::PREV_BUTTON :
		g_ExecutionQuestPanel->PrevQuestSlotPage() ;		
		break ;

	case RYLEXECUTIONQUESTDLG::NEXT_BUTTON :
		g_ExecutionQuestPanel->NextQuestSlotPage() ;
		break ;

	case RYLEXECUTIONQUESTDLG::TEXT_BUTTON :
		{
			CRYLGameScene*		  pScene    = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
			CRYLQuestDetailDataDlg* pDlg	= ( CRYLQuestDetailDataDlg* )pScene->GetQuestDetailDataDlg() ;
			
			if ( pDlg->GetVisible() && 
				 g_ExecutionQuestPanel->GetProcessQuestTextIndex() == ( ( CRYLExeQuestMapButton* )Sender )->GetQuestIndex() )
			{
				pDlg->SetVisible( FALSE ) ;
			}
			else
			{
				g_ExecutionQuestPanel->SetProcessQuestTextIndex( ( ( CRYLExeQuestMapButton* )Sender )->GetQuestIndex() ) ;
				g_ExecutionQuestPanel->ShowDetailDataDlg() ;
			}
		}
		break ;

	case RYLEXECUTIONQUESTDLG::MAP_BUTTON :
		{
			CRYLGameScene*		  pScene    = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
			CRYLLargeMapFrameDlg* pDlg		= ( CRYLLargeMapFrameDlg * )pScene->GetLargeMapDlg() ;

			if ( pDlg->GetVisible() &&
				 g_ExecutionQuestPanel->GetProcessQuestMapIndex() == ( ( CRYLExeQuestMapButton* )Sender )->GetQuestIndex() )
			{
				pDlg->SetVisible( FALSE ) ;
			}
			else
			{
				g_ExecutionQuestPanel->SetProcessQuestMapIndex( ( ( CRYLExeQuestMapButton* )Sender )->GetQuestIndex() ) ;
				pDlg->ShowQuestTarget( g_ExecutionQuestPanel->m_lpSelectQuest ) ;
				pDlg->SetVisible( TRUE ) ;
			}
		}
		break ;
	case RYLEXECUTIONQUESTDLG::QUIT_BUTTON :
		{
			// Yes / No 창 뜨기
			CRYLMessageBox *lpMessageBox = new CRYLMessageBox;
			lpMessageBox->Create(CRYLStringTable::m_strString[702], MB_YES | MB_NO | MB_EXIT);
			lpMessageBox->SetResult(&g_ExecutionQuestPanel->m_dwDeleteQuestResult);
			g_ExecutionQuestPanel->m_dwDeleteQuestResult = 0;
			g_ExecutionQuestPanel->SetProcessQuestQuitIndex( ( ( CRYLExeQuestMapButton* )Sender )->GetQuestIndex() ) ;
		}
		break ;
	}
}