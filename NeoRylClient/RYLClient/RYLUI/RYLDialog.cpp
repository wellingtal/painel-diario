#include "RYLDialog.h"
#include "GMTexture.h"

#include "GMUIScreen.h"
#include "GMUIUtil.h"

#include "RYLFontTexture.h"
#include "GMFont.h"

#include "RYLImageManager.h"
#include "RYLRectangle.h"

#include "GMImageDib.h"
#include "RYLSpriteList.h"
#include "RYLStringTable.h"
#include "GMMemory.h"

CRYLDlgCloseButton::CRYLDlgCloseButton( CRYLDialog *pRYLDialog ) : CRYLButton( pRYLDialog )
{
	SetWidth( 10 ) ;
	SetHeight( 9 ) ;
}

CRYLDlgCloseButton::~CRYLDlgCloseButton()
{
}

VOID CRYLDlgCloseButton::Click( CGMUIObject* Sender, INT X, INT Y )
{
	if ( !CGMUIObject::GetEnable() )
		return ;

	CRYLDialog *pRYLDialog ;

	pRYLDialog = ( CRYLDialog* )m_pGMUIParentDlg ;
	pRYLDialog->SetVisible( FALSE ) ;

	CRYLButton::Click( Sender, X, Y ) ;
}

VOID CRYLDlgCloseButton::SetDefaultTexture( INT State )
{
	CRYLImageCabinet::EDefaultCtrlType DefaultCtrlType ;

	switch (State)
	{
		case ButtonImageStateType_Up:
			DefaultCtrlType = CRYLImageCabinet::EDCT_CAPTION_CLOSE_BUTTON_UP;
			break;
		case ButtonImageStateType_Down:
			DefaultCtrlType = CRYLImageCabinet::EDCT_CAPTION_CLOSE_BUTTON_DOWN;
			break;
		case ButtonImageStateType_Disible:
			DefaultCtrlType = CRYLImageCabinet::EDCT_CAPTION_CLOSE_BUTTON_DISIBLE;
			break;
		case ButtonImageStateType_Enter:
			DefaultCtrlType = CRYLImageCabinet::EDCT_CAPTION_CLOSE_BUTTON_ENTER;
			break;
	}
	CRYLButton::AttachButtonTexture( ( CRYLButton::EButtonImageStateType)State, (CGMTexture*)CRYLImageManager::s_pDefaultImageCabinet->GetRYLImage(m_iWidth, m_iHeight, DefaultCtrlType) );
}

VOID CRYLDlgCloseButton::DoDraw( CGMUIObject* Sender, RECT r, INT State )
{
	if ( ! CRYLButton::GetIsButtonTexture( ( CRYLButton::EButtonImageStateType )State ) )
	{
		SetDefaultTexture( State ) ;
	}
	
	CRYLButton::DoDraw( Sender, r, State ) ;
}

/////////////////////////////////////////////////////////////////////////////////////////////
CRYLDlgMinimizeButton::CRYLDlgMinimizeButton( CRYLDialog *pRYLDialog ) : CRYLButton( pRYLDialog )
{
	CRYLButton::SetWidth( 16 ) ;
	CRYLButton::SetHeight( 16 ) ;
}

CRYLDlgMinimizeButton::~CRYLDlgMinimizeButton()
{
}

VOID CRYLDlgMinimizeButton::Click( CGMUIObject* Sender, INT X, INT Y )
{
	CRYLDialog *pRYLDialog ;

	pRYLDialog = ( CRYLDialog* )m_pGMUIParentDlg ;

	CRYLButton::Click( Sender, X, Y ) ;

	if ( pRYLDialog )
	{
		if ( pRYLDialog->GetFormState() == CRYLDialog::RYLDLG_STATE_NORMAL )
		{
			pRYLDialog->SetFormState( CRYLDialog::RYLDLG_STATE_MINIMIZE ) ;
		}
		else
		{
			pRYLDialog->SetFormState( CRYLDialog::RYLDLG_STATE_NORMAL ) ;
		}
	}
}

VOID CRYLDlgMinimizeButton::SetDefaultTexture (INT State)
{
	CRYLImageCabinet::EDefaultCtrlType DefaultCtrlType;
	switch (State)
	{
		case ButtonImageStateType_Up:
			DefaultCtrlType = CRYLImageCabinet::EDCT_CAPTION_MINI_BUTTON_UP;
			break;
		case ButtonImageStateType_Down:
			DefaultCtrlType = CRYLImageCabinet::EDCT_CAPTION_MINI_BUTTON_DOWN;
			break;
		case ButtonImageStateType_Disible:
			DefaultCtrlType = CRYLImageCabinet::EDCT_CAPTION_MINI_BUTTON_DISIBLE;
			break;
		case ButtonImageStateType_Enter:
			DefaultCtrlType = CRYLImageCabinet::EDCT_CAPTION_MINI_BUTTON_ENTER;
			break;
	}
	CRYLButton::AttachButtonTexture( ( CRYLButton::EButtonImageStateType )State, ( CGMTexture* )CRYLImageManager::s_pDefaultImageCabinet->GetRYLImage( m_iWidth, m_iHeight, DefaultCtrlType ) ) ;
}

VOID CRYLDlgMinimizeButton::DoDraw( CGMUIObject* Sender, RECT r, INT State )
{
	if (!CRYLButton::GetIsButtonTexture( ( CRYLButton::EButtonImageStateType )State ) )
	{
		SetDefaultTexture (State);
	}
	
	CRYLButton::DoDraw( Sender, r, State ) ;
}


//------------------------------------------------------------------------------
//					CRYLDialog : public CGMUICustomDlg
//------------------------------------------------------------------------------
CRYLDialog::CRYLDialog() : CGMUICustomDlg()
{
	m_CaptionHeight		= 0 ; //
	m_dwColor			= 0xF7C4C4C4 ;

	m_bDlgTypeFlag		= FALSE ;

	m_iOrWidth			= CGMUICustomDlg::GetWidth() ;
	m_iOrHeight			= CGMUICustomDlg::GetHeight() ;

	m_DlgState			= CRYLDialog::RYLDLG_STATE_NORMAL ;
	m_DlgType			= RYLDLG_TYPE_NONE ;

	m_pCloseButton		= NULL ;
	m_pMinimizeButton	= NULL ;
	m_pCaptionTexture	= NULL ;
	m_pCaptionImage		= NULL ;
	m_pWindowImage		= NULL ;
	m_pSmallIconImage	= NULL ;
	m_pLargeIconImage	= NULL ;
}

CRYLDialog::~CRYLDialog()
{
	GM_DELETE( m_pCloseButton ) ;
	GM_DELETE( m_pMinimizeButton ) ;

	GM_DELETE( m_pCaptionTexture ) ;
	m_pCaptionImage		= NULL ;
	m_pWindowImage		= NULL ;
	m_pSmallIconImage	= NULL ;
	m_pLargeIconImage	= NULL ;
}

VOID CRYLDialog::SetCaption( LPSTR pCaption )
{
	CGMUICustomDlg::SetCaption( pCaption ) ;
	INT w = GetGMUITextWidth( m_Caption ) ;

	if (!m_pCaptionTexture)
	{
		CGMFont::Instance()->CreateFontTexture( &m_pCaptionTexture ) ;
	}
	m_pCaptionTexture->SetFontColor( 0xffffffff ) ;
	m_pCaptionTexture->SetSize (w, 16);
	m_pCaptionTexture->SetText (m_Caption);
}

VOID CRYLDialog::SetCaptionHeight( INT Height )
{
	m_CaptionHeight = Height ;
	SetRect( &m_MovingRect, 0, 0, m_iWidth, m_CaptionHeight ) ;	
}

INT CRYLDialog::GetCaptionHeight()
{
	return m_CaptionHeight ;
}

VOID CRYLDialog::SetNormalState()
{
	if ( CGMUICustomDlg::GetVisible() == FALSE )
		CGMUICustomDlg::SetVisible( TRUE ) ;

	m_iWidth	= m_iOrWidth ;
	m_iHeight	= m_iOrHeight ;
	
	SetChangeSizeType() ;

	//CGMUICustomDlg::AllShowCtrl() ;
	m_DlgState = CRYLDialog::RYLDLG_STATE_NORMAL ;
}

VOID CRYLDialog::SetMiniMizeState ()
{
	CGMUICustomDlg::AllHideCtrl() ;

	m_iWidth	= m_iOrWidth ;
	m_iHeight	= m_CaptionHeight ;
	
	if ( m_pMinimizeButton )  
		m_pMinimizeButton->SetVisible( TRUE ) ;

	if ( m_pCloseButton ) 
		m_pCloseButton->SetVisible( TRUE ) ;
	m_DlgState = CRYLDialog::RYLDLG_STATE_MINIMIZE ;
}

VOID CRYLDialog::SetIconState ()
{
	if ( !m_pLargeIconImage )
	{
		GMASSERT( FALSE, "CRYLDialog::SetIconState Failed" ) ;
		m_DlgState = CRYLDialog::RYLDLG_STATE_NORMAL ;
		DoChangeDlgState( this, m_DlgState ) ;

		return ;
	}

	CGMUICustomDlg::AllHideCtrl() ; 

	if ( m_pLargeIconImage )
	{
		m_iWidth  = m_pLargeIconImage->GetWidth() ;
		m_iHeight = m_pLargeIconImage->GetHeight() ;
	}
	
	m_DlgState = CRYLDialog::RYLDLG_STATE_ICON;
}

VOID CRYLDialog::DoChangeDlgState( CGMUIObject* Sender, ERYLDLG_STATE DlgState )
{
	m_iOrHeight = m_iHeight ;
	m_iOrWidth	= m_iWidth ;

	switch( m_DlgState )
	{
		case RYLDLG_STATE_NORMAL:		SetNormalState ();			break;
		case RYLDLG_STATE_MINIMIZE:		SetMiniMizeState ();		break;
		case RYLDLG_STATE_ICON:			SetIconState ();			break;
	}
}

VOID CRYLDialog::CreateCloseButton ()
{
	m_pCloseButton = new CRYLDlgCloseButton (this);
	RECT rt ;
	CGMImageDib* pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI_256x256 ) ;
	//SetRect( &rt, 202, 0, 213, 9 ) ;//hz guigui
	SetRect( &rt, 4, 347, 15, 356 ) ;
	m_pCloseButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	//SetRect( &rt, 202, 18, 213, 27 ) ;
	SetRect( &rt, 4, 267, 15, 276 ) ;
	m_pCloseButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	//SetRect( &rt, 202, 9, 213, 18 ) ;
	SetRect( &rt, 4, 257, 15, 266 ) ;
	m_pCloseButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	//SetRect( &rt, 202, 9, 213, 18 ) ;
	SetRect( &rt, 4, 257, 15, 266 ) ;
	m_pCloseButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pCloseButton->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 1868], 1868, TOOLTIP_PANEL ) ;
	CGMUICustomDlg::AttachGMUIControl( m_pCloseButton ) ;
}

VOID CRYLDialog::CreateMinimizeButton ()
{
	m_pMinimizeButton = new CRYLDlgMinimizeButton (this);
	CGMUICustomDlg::AttachGMUIControl( m_pMinimizeButton ) ;
}

VOID CRYLDialog::DoChangeDlgType( CGMUIObject* Sender, ERYLDLG_TYPE DlgType )
{
	switch (DlgType)
	{
		case RYLDLG_TYPE_NONE :
		case RYLDLG_FRAME_TYPE_NONE : 
			if ( m_pCloseButton ) 
				m_pCloseButton->SetVisible( FALSE ) ;

			if ( m_pMinimizeButton ) 
				m_pMinimizeButton->SetVisible( FALSE ) ;

			break ;

		case RYLDLG_TYPE_CLOSEBUTTON :
		case RYLDLG_FRAME_TYPE_CLOSEBUTTON :
			if ( !m_pCloseButton )  
				CreateCloseButton() ;
			m_pCloseButton->SetVisible( TRUE ) ;
			m_pCloseButton->SetLeft( (m_iWidth - m_pCloseButton->GetWidth() ) - 2 ) ;
			m_pCloseButton->SetTop( 3 ) ;
			break ;

		case RYLDLG_TYPE_MINIMIZEBUTTON :
		case RYLDLG_FRAME_TYPE_MINIMIZEBUTTON :
			if ( !m_pMinimizeButton )
				CreateMinimizeButton() ;
			m_pMinimizeButton->SetVisible( TRUE ) ;
			m_pMinimizeButton->SetLeft( (m_iWidth - m_pMinimizeButton->GetWidth() ) - 34 ) ;
			m_pMinimizeButton->SetTop( 6 ) ;
			break ;

		case RYLDLG_TYPE_BOTHBUTTON :
		case RYLDLG_FRAME_TYPE_BOTHBUTTON : 
			if (!m_pCloseButton)
				CreateCloseButton() ;

			m_pCloseButton->SetVisible( TRUE ) ;
			m_pCloseButton->SetLeft( m_iWidth - m_pCloseButton->GetWidth() - 34 ) ;
			m_pCloseButton->SetTop( 6 ) ;
			if ( !m_pMinimizeButton )
				CreateMinimizeButton() ;
			m_pMinimizeButton->SetVisible( TRUE ) ;
			m_pMinimizeButton->SetLeft( m_iWidth - ( m_pMinimizeButton->GetWidth() + m_pCloseButton->GetWidth () ) - 34 ) ;
			m_pMinimizeButton->SetTop( 6 ) ;
			break ;
	}

	DoChangeDlgState( this, m_DlgState ) ;
	m_bDlgTypeFlag = FALSE ;
}

VOID CRYLDialog::SetFormState( ERYLDLG_STATE DlgState )
{
	m_DlgState = DlgState  ;
	DoChangeDlgState( this, m_DlgState ) ;
}

CRYLDialog::ERYLDLG_STATE CRYLDialog::GetFormState ()
{
	return m_DlgState ;
}

VOID CRYLDialog::SetFormType( ERYLDLG_TYPE DlgType )
{
	m_DlgType = DlgType ;
	m_bDlgTypeFlag = TRUE ;
}

CRYLDialog::ERYLDLG_TYPE CRYLDialog::GetFormType ()
{
	return m_DlgType ;
}

VOID CRYLDialog::AttachCaptionTexture( CGMTexture* pGMTexture )
{
	m_pCaptionImage = pGMTexture ;
}

VOID CRYLDialog::DetachCaptionTexture ()
{
	m_pCaptionImage = NULL ;
}

VOID CRYLDialog::AttachWindowTexture( CGMTexture* pGMTexture )
{
	m_pWindowImage = pGMTexture ;
}

CGMTexture*	CRYLDialog::DetachWindowTexture ()
{
	CGMTexture*		pTexture = m_pWindowImage;
	m_pWindowImage = NULL;
	return pTexture;
}

VOID CRYLDialog::AttachSmallIconTexture( CGMTexture* pGMTexture )
{
	m_pSmallIconImage = pGMTexture;
}

VOID CRYLDialog::DetachSmallIconTexture ()
{
	m_pSmallIconImage = NULL;
}

VOID CRYLDialog::AttachLargeIconTexture( CGMTexture* pGMTexture )
{
	m_pLargeIconImage = pGMTexture ;
}

VOID CRYLDialog::DetachLargeIconTexture ()
{
	m_pLargeIconImage = NULL;
}

VOID CRYLDialog::AttachCloseButtonTexture ( CGMUIButton::EButtonImageStateType StateType, CGMTexture* pGMTexture )
{
	if ( m_DlgType == RYLDLG_TYPE_NONE || m_DlgType == RYLDLG_FRAME_TYPE_NONE )
	{
		GMASSERT( FALSE, "CRYLDialog::AttachCloseButtonTexture Faild" ) ;
		return;
	}
	if ( !m_pCloseButton )
		CreateCloseButton() ;
	m_pCloseButton->AttachButtonTexture( StateType, pGMTexture ) ;
}

VOID CRYLDialog::DetachCloseButtonTexture( CGMUIButton::EButtonImageStateType StateType )
{
	if (m_pCloseButton)
		m_pCloseButton->DetachButtonTexture( StateType ) ;
}

VOID CRYLDialog::AttachMinimizeButtonTexture( CGMUIButton::EButtonImageStateType StateType, CGMTexture* pGMTexture )
{
	if ( m_DlgType == RYLDLG_TYPE_NONE || m_DlgType == RYLDLG_FRAME_TYPE_NONE )
	{
		GMASSERT( FALSE, "CRYLDialog::AttachMinimizeButtonTexture Failed" ) ;
		return ;
	}

	if ( !m_pMinimizeButton )
		CreateMinimizeButton() ;
	m_pMinimizeButton->AttachButtonTexture( StateType, pGMTexture ) ;
}

VOID CRYLDialog::DetachMinimizeButtonTexture( CGMUIButton::EButtonImageStateType StateType )
{
	if ( m_pMinimizeButton )
		m_pMinimizeButton->DetachButtonTexture (StateType);
}

VOID CRYLDialog::SetSize( CGMUIObject* Sender )
{
	CGMUICustomDlg::SetSize(Sender ) ;
	if ( m_DlgState == RYLDLG_STATE_NORMAL )
	{
		m_iOrWidth  = m_iWidth ;
		m_iOrHeight = m_iHeight ;
	}
}

VOID CRYLDialog::Click( CGMUIObject* Sender, INT X, INT Y)
{
	CGMUICustomDlg::Click (Sender, X, Y);
}

VOID	CRYLDialog::MouseUp( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y)
{
	CGMUICustomDlg::MouseUp(Sender, Button, Shift, X, Y);
}

VOID	CRYLDialog::MouseDown( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y ) 
{
	CGMUICustomDlg::MouseDown( Sender, Button, Shift, X, Y ) ;
}

VOID CRYLDialog::DblClick( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y)
{
	if ( m_DlgState == RYLDLG_STATE_ICON )
	{
		SetFormState( RYLDLG_STATE_NORMAL ) ;
	}
	else
	{
		if ( m_pSmallIconImage )
		{
			if ( InArea( 0, 0, m_pSmallIconImage->GetWidth (), m_pSmallIconImage->GetHeight (), X, Y ) == 0 )
				SetFormState( RYLDLG_STATE_ICON ) ;
		}
	}

	CGMUICustomDlg::DblClick( Sender, Button, Shift, X, Y ) ;
}

VOID CRYLDialog::DefaultCaptionDraw( INT X, INT Y, INT Width, INT Height )
{
	CRYLRectangle::Instance()->Draw( X, Y, Width, Height, m_dwAlpha, m_dwColor ) ;
	//m_pCaptionImage = (CGMTexture*)CRYLImageManager::s_pDefaultImageCabinet->GetRYLImage(m_iWidth, m_CaptionHeight, CRYLImageCabinet::EDCT_FORM_CAPTION, 0xC48A0000);
}

VOID CRYLDialog::DefaultWindowDraw( INT X, INT Y, INT Width, INT Height )
{
	CRYLRectangle::Instance()->Draw( X, Y, Width, Height, m_dwAlpha, m_dwColor ) ;
	//m_pWindowImage = (CGMTexture*)CRYLImageManager::s_pDefaultImageCabinet->GetRYLImage(m_iWidth, m_iHeight, CRYLImageCabinet::EDCT_FORM_WINDOW, m_dwColor);
}

VOID CRYLDialog::SetChangeSizeType (){
	if (m_DlgType == RYLDLG_TYPE_NONE || m_DlgType == RYLDLG_FRAME_TYPE_NONE )
	{
		if ( m_pWindowImage )
		{
			m_iWidth	= m_pWindowImage->GetWidth() ;
			m_iHeight	= m_pWindowImage->GetHeight() ;
			SetCaptionHeight( m_pWindowImage->GetHeight() ) ;
		}
		else
		{
			SetCaptionHeight( m_iHeight ) ;
		}
		m_CaptionHeight = 0 ;
	}
	else
	{
		if ( m_CaptionHeight == 0 )
		{
			if ( m_pCaptionImage )
			{
				m_CaptionHeight = m_pCaptionImage->GetHeight() ;
			}
			else
			{
				m_CaptionHeight = 20 ;
				m_iHeight = m_iHeight + m_CaptionHeight ;
			}
		}
		if ( m_pWindowImage )
		{
			m_iWidth  = m_pWindowImage->GetWidth() ;
			m_iHeight = m_pWindowImage->GetHeight() + m_CaptionHeight ;
		}
		SetCaptionHeight( m_CaptionHeight ) ;
	}
}

VOID CRYLDialog::DoStartDraw( CGMUIObject* Sender, RECT r )
{
	if ( m_bDlgTypeFlag)	
		SetChangeSizeType() ;

	if ( m_DlgType != RYLDLG_TYPE_NONE && m_DlgType != RYLDLG_FRAME_TYPE_NONE )
	{
		if ( !m_pCaptionImage )
			DefaultCaptionDraw( r.left, r.top, m_iWidth, m_CaptionHeight ) ;
		else
			m_pCaptionImage->Draw( r.left, r.top ) ;

		if ( m_pCaptionTexture ) 
			m_pCaptionTexture->Draw( r.left + 2, r.top + 4 ) ;
	}

	if ( m_bDlgTypeFlag )
		DoChangeDlgType( this, m_DlgType ) ;

	switch( m_DlgState )
	{
		case RYLDLG_STATE_NORMAL :

			if ( !m_pWindowImage )
				DefaultWindowDraw( r.left, r.top+m_CaptionHeight, m_iWidth, m_iHeight - m_CaptionHeight ) ;
			else
				m_pWindowImage->Draw( r.left, r.top + m_CaptionHeight ) ;

			if ( m_pSmallIconImage )
				m_pSmallIconImage->Draw( r.left + 2, r.top + 2 ) ;
			break;
	
		case RYLDLG_STATE_MINIMIZE :
			if ( m_pSmallIconImage )
				m_pSmallIconImage->Draw( r.left + 2, r.top + 2 ) ;
			break;
	}

	CGMUICustomDlg::DoStartDraw (Sender, r);
}

VOID CRYLDialog::DoEndDraw( CGMUIObject* Sender, RECT r )
{
	CGMUICustomDlg::DoEndDraw( Sender, r ) ;

	switch( m_DlgState )
	{
		case RYLDLG_STATE_ICON :
			if ( m_pLargeIconImage )
				m_pLargeIconImage->Draw( r.left, r.top ) ;
			break ;
	}
}

VOID CRYLDialog::SetVisible( BOOL bVisible )
{
	CGMUICustomDlg::SetVisible( bVisible ) ;

	if ( bVisible )
		DoChangeDlgState( this, m_DlgState ) ;
}
