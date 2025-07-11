#include <winsock2.h>
#include "RYLJobpanel.h"
#include "GMUICustomDlg.h"

#include "RYLClientMain.h"
#include "RYLGameData.h"
#include "RYLNetworkData.h"
#include "RYLStringTable.h"
#include "RYLImage.h"
#include "RYLImageManager.h"
#include "GMImageDib.h"
#include "RYLImageCabinet.h"
#include "RYLSpriteEx.h"
#include "RYLSpriteList.h"

#include "RYLButton.h"
#include "RYLLabel.h"

#include "RYLSceneManager.h"
#include "RYLGameScene.h"

#include "GMMemory.h"



//-----------------------------------------------------------------------------
// RYLJobPanel
CRYLJobPanel* g_pSocialActionPanel = NULL ;
CRYLJobPanel::CRYLJobPanel( CGMUICustomDlg *pParentDlg ) : CGMUICustomPanel( pParentDlg )
{
	RECT	rt ;
	INT		iTempWidth ;
	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;

	// Background Image
	CRYLSpriteList::Instance()->CreateGMImage( 256, 440, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;

	SetRect( &rt, 0, 38, 256, 43 ) ;
	for ( int i = 0 ; i < 87 ; i ++ )
	{
		iTempWidth = i * 5 ;
		pImageDIB->DrawRect( &rt, 0, iTempWidth, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	}
	SetRect( &rt, 0, 42, 256, 52 ) ;
	pImageDIB->DrawRect( &rt, 0, iTempWidth, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;

	// Frame Image
	pImageDIB->MakeFrameRect( 6,  28, 243, 389, 0xffB99E6B, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect( 84, 5, 68, 18, 0xff959595, 0xc0000000,  1 ) ;

	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;

	AttachPanelImage( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;

	m_pJobPointLabel			= NULL ; // 스킬 레벨
	m_pJobPointValueLabel		= NULL ; // 스킬 레벨 값
	m_pComingSoonLabel			= NULL ;

	SetWidth( 256 ) ;
	SetHeight( 440 ) ;


	m_pGMUIParentDlg = pParentDlg ;
	g_pSocialActionPanel = this ;

	m_dwSlotHeightMargin = 0L ;
	m_dwStartSlot        = 0L ;
}

CRYLJobPanel::~CRYLJobPanel() 
{
	FinalCtrl() ;
}

VOID	CRYLJobPanel::InitCtrl() 
{
	RECT rt ;
	CGMImageDib* pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ;

	m_pPrevJobButton = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pPrevJobButton->SetLeft( 220 ) ;
	m_pPrevJobButton->SetTop( 420 ) ;
	m_pPrevJobButton->OnClickEvent = DlgClick ;
	m_pPrevJobButton->SetInstance( RYLJOBPANEL::PREVJOB_BUTTON ) ;
	SetRect( &rt, 217, 215, 230, 227 ) ;
	m_pPrevJobButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 217, 243, 230, 255 ) ;
	m_pPrevJobButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 217, 229, 230, 241 ) ;
	m_pPrevJobButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 217, 215, 230, 227 ) ;
	m_pPrevJobButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	CGMUICustomPanel::AddChild( m_pPrevJobButton ) ;

	m_pNextJobButton = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pNextJobButton->SetLeft( 235 ) ;
	m_pNextJobButton->SetTop( 420 ) ;
	m_pNextJobButton->OnClickEvent = DlgClick ;
	m_pNextJobButton->SetInstance( RYLJOBPANEL::NEXTJOB_BUTTON ) ;
	SetRect( &rt, 232, 215, 245, 227 ) ;
	m_pNextJobButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 232, 243, 245, 255 ) ;
	m_pNextJobButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 232, 229, 245, 241 ) ;
	m_pNextJobButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 232, 215, 245, 227 ) ;
	m_pNextJobButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	CGMUICustomPanel::AddChild( m_pNextJobButton ) ;

	// edith 2009.09.06 잡 탭 추가하기
	m_pJobPointLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pJobPointLabel->SetLeft( 16 ) ;
	m_pJobPointLabel->SetTop( 8 ) ;
	m_pJobPointLabel->SetFontColor( 0xffffffff ) ;
	SetRect( &rt, 17, 8, 80, 19 ) ;
	m_pJobPointLabel->SetAlignRect( rt ) ;
	m_pJobPointLabel->SetAlign( CENTER ) ;
	m_pJobPointLabel->SetCaption( CRYLStringTable::m_strString[ 2081 ] ) ;
	m_pJobPointLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pJobPointLabel ) ;

	m_pJobPointValueLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pJobPointValueLabel->SetLeft( 98 ) ;
	m_pJobPointValueLabel->SetTop( 8 ) ;
	m_pJobPointValueLabel->SetFontColor( 0xffffffff ) ;
	SetRect( &rt, 84, 8, 151, 19 ) ;
	m_pJobPointValueLabel->SetAlignRect( rt ) ;
	m_pJobPointValueLabel->SetAlign( CENTER ) ;
	m_pJobPointValueLabel->SetCaption( "0" ) ;
	m_pJobPointValueLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pJobPointValueLabel ) ;

	m_pComingSoonLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pComingSoonLabel->SetLeft( 16 ) ;
	m_pComingSoonLabel->SetTop( 180 ) ;
	m_pComingSoonLabel->SetFontColor( 0xffffffff ) ;
	SetRect( &rt, 17, 180, 17+235, 180+12 ) ;
	m_pComingSoonLabel->SetAlignRect( rt ) ;
	m_pComingSoonLabel->SetAlign( CENTER ) ;
	m_pComingSoonLabel->SetCaption( CRYLStringTable::m_strString[ 4802 ] ) ;
	m_pComingSoonLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pComingSoonLabel ) ;
	

	InitJob() ;
	JobSlotAlign() ;
}

VOID	CRYLJobPanel::FinalCtrl()
{
	GM_DELETE( m_pPrevJobButton ) ;
	GM_DELETE( m_pNextJobButton ) ;
	GM_DELETE( m_pJobPointLabel ) ;
	GM_DELETE( m_pJobPointValueLabel ) ;
	GM_DELETE( m_pComingSoonLabel ) ;
	
}

HRESULT		CRYLJobPanel::Update( BOOL &bClick, BOOL &bEdge ) 
{
	CRYLGameData* pGame = CRYLGameData::Instance() ;

	return S_OK ;
}

HRESULT		CRYLJobPanel::Render( LPDIRECT3DDEVICE8 lpD3DDevice )  
{
	return S_OK ;
}

HRESULT		CRYLJobPanel::FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) 
{
	return S_OK ;
}

VOID		CRYLJobPanel::DlgClick( CGMUIObject* Sender, INT X, INT Y ) 
{
	if ( !g_pSocialActionPanel )
		return ;

	switch( Sender->GetInstance())
	{
	case RYLJOBPANEL::NEXTJOB_BUTTON :
		g_pSocialActionPanel->NextJobSlotPage() ;
		break ;

	case RYLJOBPANEL::PREVJOB_BUTTON :
		g_pSocialActionPanel->PrevJobSlotPage() ;
		break ;
	}
}

void	CRYLJobPanel::InitJob() 
{
}

void	CRYLJobPanel::PrevJobSlotPage()
{
}

void	CRYLJobPanel::NextJobSlotPage()
{	
}

void		CRYLJobPanel::UpdateJobSlotPosition()
{
}

void	CRYLJobPanel::JobSlotAlign()
{
	UpdateJobSlotPosition() ;
}
