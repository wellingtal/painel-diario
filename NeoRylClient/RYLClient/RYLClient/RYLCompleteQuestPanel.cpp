#include <winsock2.h>
#include "RYLCompleteQuestPanel.h"
#include "RYLImage.h"
#include "RYLImageManager.h"
#include "GMImageDib.h"
#include "RYLImageCabinet.h"
#include "RYLSpriteList.h"

#include "RYLButton.h"
#include "RYLLabel.h"

#include "RYLStringTable.h"

#include "..\\CharStatus.h"
#include "GMMemory.h"


//-----------------------------------------------------------------------------
// Quest Slot
CRYLCompleteQuestSlot*		g_CompleteQuestSlot = NULL ;
CRYLCompleteQuestSlot::CRYLCompleteQuestSlot( CGMUICustomDlg *pParentDlg ) : CGMUICustomPanel( pParentDlg )
{
	m_pGMUIParentDlg		= pParentDlg ;
	g_CompleteQuestSlot	= this ;

	m_pQuestTitleLabel		= NULL ;			// 퀘스트 제목
	m_pQuestLevelLabel		= NULL ;			// 퀘스트 난이도
	m_iQuestSlotIndex		= 0 ;
}

CRYLCompleteQuestSlot::~CRYLCompleteQuestSlot() 
{
	GM_DELETE( m_pQuestTitleLabel ) ;			// 퀘스트 제목
	GM_DELETE( m_pQuestLevelLabel ) ;			// 퀘스트 난이도
}

VOID		CRYLCompleteQuestSlot::MakeCompleteQuestSlot() 
{
	RECT			rt ;
	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;

	// Background Image
	SetWidth( 233 ) ;
	SetHeight( 42 ) ;
	CRYLSpriteList::Instance()->CreateGMImage( 233, 42, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;

	SetRect( &rt, 0, 42, 233, 83 ) ;
	
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_QUESTSLOT_256x256 ) ) ;
	
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	AttachPanelImage( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;
}

VOID		CRYLCompleteQuestSlot::CreateCompleteQuestTitle( unsigned long ulQuestID, LPSTR lpQuestTitle, LPSTR lpQuestLevel ) 
{
    m_ulCompleteQuestID = ulQuestID;

	m_pQuestTitleLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pQuestTitleLabel->SetLeft( 9 ) ;
	m_pQuestTitleLabel->SetTop( 42 ) ;
	m_pQuestTitleLabel->SetFontColor( 0xffffffff ) ;
	m_pQuestTitleLabel->SetAutoSize( TRUE ) ;
	m_pQuestTitleLabel->SetCaption( lpQuestTitle ) ;
	CGMUICustomPanel::AddChild( m_pQuestTitleLabel ) ;

	m_pQuestLevelLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pQuestLevelLabel->SetLeft( 9 ) ;
	m_pQuestLevelLabel->SetTop( 59 ) ;
	m_pQuestLevelLabel->SetFontColor( 0xffffffff ) ;
	m_pQuestLevelLabel->SetAutoSize( TRUE ) ;
	m_pQuestLevelLabel->SetCaption( lpQuestLevel ) ;
	CGMUICustomPanel::AddChild( m_pQuestLevelLabel ) ;
}

//-------------------------------------------------------------------------
// CompleteQuest Panel
CRYLCompleteQuestPanel*		g_CompleteQuestPanel = NULL ;
CRYLCompleteQuestPanel::CRYLCompleteQuestPanel( CGMUICustomDlg *pParentDlg ) : CGMUICustomPanel( pParentDlg )
{
	m_pGMUIParentDlg = pParentDlg ;

	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;
	RECT			rt ;

	CRYLSpriteList::Instance()->CreateGMImage( 256, 382, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	int i = 0;
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

	m_iQuestYMargin = 0 ;
	m_dwStartSlot   = 0 ;

	g_CompleteQuestPanel = this ;
}

CRYLCompleteQuestPanel::~CRYLCompleteQuestPanel() 
{
	FinalCtrl() ;
}

VOID	CRYLCompleteQuestPanel::InitCtrl() 
{
	SetWidth( 256 ) ;
	SetHeight( 375 ) ;
	RECT rt ;
	CGMImageDib* pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ;

	m_pPrevButton = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pPrevButton->SetLeft( 152 ) ;
	m_pPrevButton->SetTop( 354 ) ;
	m_pPrevButton->OnClickEvent = DlgClick ;
	m_pPrevButton->SetInstance( RYLCOMPLETEQUESTDLG::PREV_BUTTON ) ;
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
	m_pNextButton->SetInstance( RYLCOMPLETEQUESTDLG::NEXT_BUTTON ) ;
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

VOID	CRYLCompleteQuestPanel::FinalCtrl()
{
	GM_DELETE( m_pPrevButton ) ;
	GM_DELETE( m_pNextButton ) ;
	GM_DELETE( m_pPageState ) ;

	ClearSlot() ;
}

VOID	CRYLCompleteQuestPanel::InsertQuestSlot( QuestInstance* lpQuest ) 
{
	CGMImageDib* pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;

	CRYLCompleteQuestSlot* pQuest = new CRYLCompleteQuestSlot( m_pGMUIParentDlg ) ;
	pQuest->SetLeft( 11 ) ;
	pQuest->SetTop( m_iQuestYMargin + 8 ) ;
	
	pQuest->CreateCompleteQuestTitle( lpQuest->m_dwQuestID, lpQuest->m_lpQuestScript->m_strQuestTitle, lpQuest->m_lpQuestScript->m_strQuestLevel ) ;
	pQuest->MakeCompleteQuestSlot() ;
	CGMUICustomPanel::AddChild( pQuest ) ;

	m_iQuestYMargin += 41 ;
	
	m_vecQuestSlot.push_front( pQuest ) ;

	QuestSlotVisible() ;
}

VOID	CRYLCompleteQuestPanel::QuestSlotVisible()
{
	UpdateQuestSlotPosition() ;

	INT iCount = 0 ;
	CHAR strTemp[ MAX_PATH ] ;
	std::list< CRYLCompleteQuestSlot* >::iterator StartItr = FindIter( m_dwStartSlot ) ;
	CRYLCompleteQuestSlot* lpCompleteQuestSlot = NULL ;

	// 현재 페이지가 처음이 아닐때.. 이전 페널들을 숨긴다.
	if ( m_dwStartSlot >= 8 )
	{
		for ( std::list< CRYLCompleteQuestSlot* >::iterator it = m_vecQuestSlot.begin() ; it != m_vecQuestSlot.end() ; ++ it )
		{
			if ( it == StartItr )
				break ;
			else
			{
				( *it )->SetVisible( FALSE ) ;
			}
		}
	}

	for ( std::list< CRYLCompleteQuestSlot* >::iterator it = StartItr ; it != m_vecQuestSlot.end() ; ++ it )
	{
		if ( iCount >= 8 )
		{
			( *it )->SetVisible( FALSE ) ;
		}
		else
		{
			( *it )->SetVisible( TRUE ) ;
		}

		++ iCount ;
	}

	sprintf( strTemp, " %2d / %2d", m_dwStartSlot / 8 + 1, ( m_vecQuestSlot.size() - 1 ) / 8 + 1 ) ;
	int iLength = CRYLStringTable::GetStringWidth( strTemp ) ;
	m_pPageState->SetLeft( 198 - iLength / 2 ) ;
	m_pPageState->SetCaption( strTemp ) ;
}


	
VOID		CRYLCompleteQuestPanel::UpdateQuestSlotPosition()
{
	if ( m_vecQuestSlot.empty() ) return ;

	INT iTempY = 0 ;
	INT	iCount = 0 ;
	
	for ( std::list< CRYLCompleteQuestSlot* >::iterator it = m_vecQuestSlot.begin() ; it != m_vecQuestSlot.end() ; ++ it )
	{
		if ( ( iCount % 8 ) == 0 )
		{
			iTempY = 0 ;
		}

		( *it )->SetTop( iTempY + 13 ) ;
		
		iTempY += 41 ;
		++ iCount ;
	}
	m_iQuestYMargin = iTempY ;
}

HRESULT		CRYLCompleteQuestPanel::Update( BOOL &bClick, BOOL &bEdge ) 
{
	return S_OK ;
}


HRESULT		CRYLCompleteQuestPanel::Render( LPDIRECT3DDEVICE8 lpD3DDevice )  
{
	return S_OK ;
}

HRESULT		CRYLCompleteQuestPanel::FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) 
{
	return S_OK ;
}

VOID	CRYLCompleteQuestPanel::PrevQuestSlotPage() 
{
	if ( m_dwStartSlot != 0 )
	{
		SetStartSlot( m_dwStartSlot -= 8 ) ;
		QuestSlotVisible() ;
	}
}

VOID	CRYLCompleteQuestPanel::NextQuestSlotPage() 
{
	if ( m_vecQuestSlot.size() - m_dwStartSlot > 8 )
	{
		SetStartSlot( m_dwStartSlot += 8 ) ;
		QuestSlotVisible() ;
	}
}

VOID	CRYLCompleteQuestPanel::DlgClick( CGMUIObject* Sender, INT X, INT Y ) 
{
	if ( !g_CompleteQuestPanel )
		return ;

	switch( Sender->GetInstance() )
	{
	case RYLCOMPLETEQUESTDLG::PREV_BUTTON :
		g_CompleteQuestPanel->PrevQuestSlotPage() ;		
		break ;

	case RYLCOMPLETEQUESTDLG::NEXT_BUTTON :
		g_CompleteQuestPanel->NextQuestSlotPage() ;
		break ;
	}
}

VOID	CRYLCompleteQuestPanel::ClearSlot()
{
	for ( std::list< CRYLCompleteQuestSlot* >::iterator itSlot = m_vecQuestSlot.begin() ; itSlot != m_vecQuestSlot.end() ; ++ itSlot )
	{
		CGMUICustomPanel::DeleteChild( (*itSlot) ) ;
		GM_DELETE( (*itSlot) ) ;
	}
	m_vecQuestSlot.clear() ;
}

std::list< CRYLCompleteQuestSlot* >::iterator CRYLCompleteQuestPanel::FindIter( INT iIndex ) 
{
	INT iCount = 0 ;

	std::list< CRYLCompleteQuestSlot* >::iterator it ;
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

//--------------------------------------------------------------------------------------------------------------------------
// CRYLCompleteQuestPanel::GetIsCompleteQuest()
bool CRYLCompleteQuestPanel::GetIsCompleteQuest( unsigned long ulQuestID )
{
    std::list< CRYLCompleteQuestSlot* >::iterator iter;
	for( iter = m_vecQuestSlot.begin() ; iter != m_vecQuestSlot.end() ; ++iter )
	{
        if( (*iter)->GetQuestID() == ulQuestID )
            return true;
    }

    return false;
}
