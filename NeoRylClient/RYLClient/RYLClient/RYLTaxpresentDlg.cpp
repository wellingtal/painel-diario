
#include "WinInput.h"
#include "GMImageDib.h"

#include "RYLImageManager.h"
#include "RYLImageCabinet.h"
#include "RYLImage.h"
#include "RYLSpriteList.h"
#include "RYLButton.h"
#include "RYLLabel.h"
#include "RYLEdit.h"
#include "RYLStringTable.h"

#include "RYLTaxpresentDlg.h"

CRYLTaxpresentDlg*	g_pRYLTaxpresentDlg = NULL ;

CRYLTaxpresentDlg::CRYLTaxpresentDlg() 
{
	RECT rt ;
	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;

	CRYLSpriteList::Instance()->CreateGMImage( 256, 15, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 0, 256, 15 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	
	pImage = new CRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	AttachCaptionTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;

	CRYLSpriteList::Instance()->CreateGMImage( 256, 145, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	for ( int i = 0 ; i < 27 ; i ++ )
	{
		SetRect( &rt, 0, 38, 256, 43 ) ;
		pImageDIB->DrawRect( &rt, 0, ( i * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	}
	SetRect( &rt, 0, 42, 256, 52 ) ;
	pImageDIB->DrawRect( &rt, 0, ( ( i - 1 ) * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	
	pImageDIB->MakeFrameRect( 2, 20, 252, 1, 0xff946b42, 0xc0000000,  1 ) ;	
	pImageDIB->MakeFrameRect( 138, 29, 23, 17, 0xff959595, 0x80000000,  1 ) ;
	pImageDIB->MakeFrameRect( 138, 56, 65, 17, 0xff959595, 0x80000000,  1 ) ;
	pImageDIB->MakeFrameRect( 138, 83, 23, 17, 0xff959595, 0x80000000,  1 ) ;
	

	pImage = new CRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	AttachWindowTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;

	
	SetWidth( 256 ) ;
	SetHeight( 255 ) ;
	SetLeft( ( CGMUIScreen::GetInstance()->GetWidth() - GetWidth() ) / 2 ) ;
	SetTop( ( CGMUIScreen::GetInstance()->GetHeight() - GetHeight() ) / 2 ) ;
	SetFormType( RYLDLG_TYPE_CLOSEBUTTON ) ;
	
	InitCtrl() ;
	
	g_pRYLTaxpresentDlg = this ;
}

CRYLTaxpresentDlg::~CRYLTaxpresentDlg() 
{
	FinalCtrl() ;
}

VOID	CRYLTaxpresentDlg::InitCtrl() 
{
	CGMImageDib* pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;

	// 세금 현황
	m_pTaxpresentLabel = new CRYLLabel( this ) ;
	m_pTaxpresentLabel->SetLeft( 11 ) ;
	m_pTaxpresentLabel->SetTop( 18 ) ;
	m_pTaxpresentLabel->SetFontColor (0xffffffff);
	m_pTaxpresentLabel->SetAutoSize( TRUE ) ;
	m_pTaxpresentLabel->SetCaption( CRYLStringTable::m_strString[ 2800 ] ) ;
	AttachGMUIControl( m_pTaxpresentLabel ) ;

	// 세 율
	m_pTaxLabel = new CRYLLabel( this ) ;
	m_pTaxLabel->SetLeft( 99 ) ;
	m_pTaxLabel->SetTop( 46 ) ;
	m_pTaxLabel->SetFontColor (0xffffffff);
	m_pTaxLabel->SetAutoSize( TRUE ) ;
	m_pTaxLabel->SetCaption( CRYLStringTable::m_strString[ 2801 ] ) ;
	AttachGMUIControl( m_pTaxLabel ) ;

	// 세율값
	m_pTaxValueLabel = new CRYLLabel( this ) ;
	m_pTaxValueLabel->SetLeft( 140 ) ;
	m_pTaxValueLabel->SetTop( 47 ) ;
	m_pTaxValueLabel->SetFontColor (0xffffffff);
	m_pTaxValueLabel->SetAutoSize( TRUE ) ;
	m_pTaxValueLabel->SetCaption( "000" ) ;
	AttachGMUIControl( m_pTaxValueLabel ) ;

	// 세금 징수 현황
	m_pTaxLevyPresentLabel = new CRYLLabel( this ) ;
	m_pTaxLevyPresentLabel->SetLeft( 50 ) ;
	m_pTaxLevyPresentLabel->SetTop( 73 ) ;
	m_pTaxLevyPresentLabel->SetFontColor (0xffffffff);
	m_pTaxLevyPresentLabel->SetAutoSize( TRUE ) ;
	m_pTaxLevyPresentLabel->SetCaption( CRYLStringTable::m_strString[ 2802 ] ) ;
	AttachGMUIControl( m_pTaxLevyPresentLabel ) ;

	// 세금 징수 현황값
	m_pTaxLevyPresentValueLabel = new CRYLLabel( this ) ;
	m_pTaxLevyPresentValueLabel->SetLeft( 140 ) ;
	m_pTaxLevyPresentValueLabel->SetTop( 74 ) ;
	m_pTaxLevyPresentValueLabel->SetFontColor (0xffffffff);
	m_pTaxLevyPresentValueLabel->SetAutoSize( TRUE ) ;
	m_pTaxLevyPresentValueLabel->SetCaption( "0000000000" ) ;
	AttachGMUIControl( m_pTaxLevyPresentValueLabel ) ;

	// 세금 징수 만기
	m_pTaxLevyExpirationLabel = new CRYLLabel( this ) ;
	m_pTaxLevyExpirationLabel->SetLeft( 50 ) ;
	m_pTaxLevyExpirationLabel->SetTop( 100 ) ;
	m_pTaxLevyExpirationLabel->SetFontColor (0xffffffff);
	m_pTaxLevyExpirationLabel->SetAutoSize( TRUE ) ;
	m_pTaxLevyExpirationLabel->SetCaption( CRYLStringTable::m_strString[ 2803 ] ) ;
	AttachGMUIControl( m_pTaxLevyExpirationLabel ) ;

	// 세금 징구 만기값
	m_pTaxLevyExpirationValueLabel = new CRYLLabel( this ) ;
	m_pTaxLevyExpirationValueLabel->SetLeft( 140 ) ;
	m_pTaxLevyExpirationValueLabel->SetTop( 101 ) ;
	m_pTaxLevyExpirationValueLabel->SetFontColor (0xffffffff);
	m_pTaxLevyExpirationValueLabel->SetAutoSize( TRUE ) ;
	m_pTaxLevyExpirationValueLabel->SetCaption( "000" ) ;
	AttachGMUIControl( m_pTaxLevyExpirationValueLabel ) ;

	pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;

	// 확인 버튼
	m_pOKButton = new CRYLButton( this ) ;
	m_pOKButton->SetLeft( 95 ) ;
	m_pOKButton->SetTop( 120 ) ;
	m_pOKButton->SetCaption( CRYLStringTable::m_strString[ 2164 ] ) ;
	m_pOKButton->SetFontColor( 0xffffffff) ;
	m_pOKButton->SetColor( 0xffffffff) ;
	m_pOKButton->OnClickEvent = DlgClick ;
	m_pOKButton->SetInstance( RYLTAXPRESENTDLG::OK_BUTTON ) ;
	m_pOKButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pOKButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pOKButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pOKButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pOKButton ) ;
}

VOID	CRYLTaxpresentDlg::FinalCtrl() 
{
	GM_DELETE( m_pTaxpresentLabel ) ;				// 세금 현황
	GM_DELETE( m_pTaxLabel ) ;						// 세율
	GM_DELETE( m_pTaxValueLabel ) ;					// 세율
	GM_DELETE( m_pTaxLevyPresentLabel ) ;			// 세금 징수 현황
	GM_DELETE( m_pTaxLevyPresentValueLabel ) ;		// 세금 징수 현황
	GM_DELETE( m_pTaxLevyExpirationLabel ) ;		// 세금 징구 만기
	GM_DELETE( m_pTaxLevyExpirationValueLabel ) ;	// 세금 징구 만기
	GM_DELETE( m_pOKButton ) ;						// 확인 버튼	
}

VOID	CRYLTaxpresentDlg::InitValue() 
{
}

VOID	CRYLTaxpresentDlg::Show( CGMUIObject* Sender ) 
{
}

VOID	CRYLTaxpresentDlg::Hide( CGMUIObject* Sender ) 
{
}

VOID	CRYLTaxpresentDlg::GMUIPaint( INT xx, INT yy ) 
{
	CRYLDialog::GMUIPaint( xx, yy ) ;
}

HRESULT		CRYLTaxpresentDlg::Render( LPDIRECT3DDEVICE8 lpD3DDevice )  
{
	return S_OK ;
}

HRESULT		CRYLTaxpresentDlg::FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) 
{
	return S_OK ;
}

HRESULT		CRYLTaxpresentDlg::Update( BOOL &bClick, BOOL &bEdge ) 
{
	if (FALSE == GetVisible())
	{
		return S_OK;
	}

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

VOID	CRYLTaxpresentDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y ) 
{
	if ( !g_pRYLTaxpresentDlg )
		return ;

	switch( Sender->GetInstance() )
	{
		case RYLTAXPRESENTDLG::OK_BUTTON :
			{
				g_pRYLTaxpresentDlg->SetVisible( FALSE ) ;
			}
			break ;
	}
}

VOID	CRYLTaxpresentDlg::SetData( unsigned long dwCurrentTax,
									unsigned long dwTaxLevyPresent,
									unsigned long dwTaxLevyExpiration ) 
{
	char szBuff[ 32 ] ;
	sprintf( szBuff, "%d", dwCurrentTax ) ;
	m_pTaxValueLabel->SetCaption( szBuff ) ;
	sprintf( szBuff, "%d", dwTaxLevyPresent ) ;
	m_pTaxLevyPresentValueLabel->SetCaption( szBuff ) ;
	sprintf( szBuff, "%d", dwTaxLevyExpiration ) ;
	m_pTaxLevyExpirationValueLabel->SetCaption( szBuff ) ;
}