#include "RYLNameDlg.h"
#include "RYLButton.h"
#include "RYLLabel.h"
#include "RYLImageManager.h"
#include "RYLImageCabinet.h"
#include "RYLSpriteList.h"
#include "WinInput.h"

CRYLNameDlg*	g_pNameDlg = NULL ;

CRYLNameDlg::CRYLNameDlg() : CRYLDialog()
{
	m_pNameLabel		= NULL ;				// 이름
	m_pHideButton		= NULL ;				// Hide 버튼

	RECT rt ;
	SetRect( &rt, 0, 24, 147, 48 ) ;
	AttachWindowTexture( ( CGMTexture*)CRYLSpriteList::Instance()->GetImage( &rt, ERYLSPRITENAME::RYL_MAINGUI_256x256 ) ) ;
	
	SetWidth( rt.right - rt.left ) ;
	SetHeight( rt.bottom - rt.top ) ;
	
	SetLeft( 0 ) ;
	SetTop( 576 ) ;
	SetFormType( RYLDLG_TYPE_NONE ) ;
	
	g_pNameDlg = this ;	
	InitCtrl() ;
}

CRYLNameDlg::~CRYLNameDlg ()
{
	FinalCtrl() ;
}

VOID	CRYLNameDlg::InitCtrl ()
{
	RECT rt ;
	CGMImageDib* pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI_256x256 ) ;

	// 이름
	m_pNameLabel = new CRYLLabel( this ) ;
	m_pNameLabel->SetLeft( 3 ) ;
	m_pNameLabel->SetTop( 7 ) ;
	m_pNameLabel->SetFontColor (0xffffffff);
	m_pNameLabel->SetAutoSize( TRUE ) ;
	m_pNameLabel->SetCaption( "이효리" ) ;
	AttachGMUIControl( m_pNameLabel ) ;

	// Hide 버튼
	m_pHideButton = new CRYLButton( this ) ;
	m_pHideButton->SetLeft( 134 ) ;
	m_pHideButton->SetTop( 3 ) ;
	m_pHideButton->OnClickEvent = DlgClick ;
	m_pHideButton->SetInstance( RYLNAMEDLG::HIDE_BUTTON ) ;
	SetRect( &rt, 202, 0, 213, 9 ) ;
	m_pHideButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 202, 18, 213, 27 ) ;
	m_pHideButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 202, 9, 213, 18 ) ;
	m_pHideButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 202, 9, 213, 18 ) ;
	m_pHideButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pHideButton ) ;
}

VOID	CRYLNameDlg::FinalCtrl ()
{
	GM_DELETE( m_pNameLabel	) ;
	GM_DELETE( m_pHideButton ) ;
}

VOID	CRYLNameDlg::Show( CGMUIObject* Sender )
{
	
}
	
VOID	CRYLNameDlg::Hide( CGMUIObject* Sender )
{
	
}

VOID	CRYLNameDlg::GMUIPaint( INT xx, INT yy )
{
	CRYLDialog::GMUIPaint( xx, yy ) ;
}

HRESULT		CRYLNameDlg::Update( BOOL &bClick, BOOL &bEdge ) 
{
	if(g_DeviceInput.InRect( GetLeft(), GetTop(), GetLeft() + GetWidth(), GetTop() + GetHeight() ) )
	{
		bEdge = TRUE;

		if(g_DeviceInput.GetIsLeftMouseDown() || g_DeviceInput.GetIsMiddleMouseDown() || g_DeviceInput.GetIsRightMouseDown() ||
			g_DeviceInput.GetIsLeftMousePress() || g_DeviceInput.GetIsMiddleMousePress() || g_DeviceInput.GetIsRightMousePress() ||
			g_DeviceInput.GetIsLeftMouseUp() || g_DeviceInput.GetIsMiddleMouseUp() || g_DeviceInput.GetIsRightMouseUp())
		{
			bClick = TRUE;
		}
	}

	return S_OK ;
}


VOID	CRYLNameDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y )
{
	if ( !g_pNameDlg )
		return ;

	switch( Sender->GetInstance() )
	{
		case RYLNAMEDLG::HIDE_BUTTON :
			break ;
	}
}
