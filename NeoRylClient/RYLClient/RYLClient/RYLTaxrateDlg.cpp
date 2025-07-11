
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

#include "RYLTaxrateDlg.h"

CRYLTaxrateDlg*	g_pRYLTaxrateDlg = NULL ;

CRYLTaxrateDlg::CRYLTaxrateDlg() 
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
	pImageDIB->MakeFrameRect( 50, 73, 144, 19, 0xff959595, 0x80000000,  1 ) ;

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
	
	g_pRYLTaxrateDlg = this ;
}

CRYLTaxrateDlg::~CRYLTaxrateDlg() 
{
	FinalCtrl() ;
}

VOID	CRYLTaxrateDlg::InitCtrl() 
{
	CGMImageDib* pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;

	// 세율변경
	m_pTaxrateLabel = new CRYLLabel( this ) ;
	m_pTaxrateLabel->SetLeft( 11 ) ;
	m_pTaxrateLabel->SetTop( 18 ) ;
	m_pTaxrateLabel->SetFontColor (0xffffffff);
	m_pTaxrateLabel->SetAutoSize( TRUE ) ;
	m_pTaxrateLabel->SetCaption( CRYLStringTable::m_strString[ 2804 ] ) ;
	AttachGMUIControl( m_pTaxrateLabel ) ;

	// 현제 세율은 000%입니다.
	m_pCurrentTaxLabel = new CRYLLabel( this ) ;
	m_pCurrentTaxLabel->SetLeft( 51 ) ;
	m_pCurrentTaxLabel->SetTop( 43 ) ;
	m_pCurrentTaxLabel->SetFontColor (0xffffffff);
	m_pCurrentTaxLabel->SetAutoSize( TRUE ) ;
	m_pCurrentTaxLabel->SetCaption( CRYLStringTable::m_strString[ 2805 ] ) ;
	AttachGMUIControl( m_pCurrentTaxLabel ) ;

	// 세율을 변경 하겠습니까? ( 0 - 100% )
	m_pChangeTaxLabel = new CRYLLabel( this ) ;
	m_pChangeTaxLabel->SetLeft( 27 ) ;
	m_pChangeTaxLabel->SetTop( 64 ) ;
	m_pChangeTaxLabel->SetFontColor (0xffffffff);
	m_pChangeTaxLabel->SetAutoSize( TRUE ) ;
	m_pChangeTaxLabel->SetCaption( CRYLStringTable::m_strString[ 2806 ] ) ;
	AttachGMUIControl( m_pChangeTaxLabel ) ;

	// %
	m_pTaxpercentLabel = new CRYLLabel( this ) ;
	m_pTaxpercentLabel->SetLeft( 198 ) ;
	m_pTaxpercentLabel->SetTop( 95 ) ;
	m_pTaxpercentLabel->SetFontColor (0xffffffff);
	m_pTaxpercentLabel->SetAutoSize( TRUE ) ;
	m_pTaxpercentLabel->SetCaption( "%" ) ;
	AttachGMUIControl( m_pTaxpercentLabel ) ;

	// 세율 에디트 박스
	m_pTaxEdit = new CRYLEdit( this ) ;
	m_pTaxEdit->SetLeft( 51 ) ;
	m_pTaxEdit->SetTop( 89 ) ;
	m_pTaxEdit->SetWidth( 142 ) ;
	m_pTaxEdit->SetHeight( 17 ) ;
	m_pTaxEdit->SetColor( 0xff000000) ;
	m_pTaxEdit->SetFontColor( 0xffffffff ) ;
	m_pTaxEdit->SetInstance( RYLTAXRATEDLG::TAX_EDIT ) ;
	m_pTaxEdit->SetMaxLength( 86 ) ;
	m_pTaxEdit->OnKeyDown = DlgKeyDown;
	AttachGMUIControl( m_pTaxEdit ) ;	

	pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;
	// 확인 버튼
	m_pOKButton = new CRYLButton( this ) ;
	m_pOKButton->SetLeft( 44 ) ;
	m_pOKButton->SetTop( 120 ) ;
	m_pOKButton->SetCaption( CRYLStringTable::m_strString[ 2164 ] ) ;
	m_pOKButton->SetFontColor( 0xffffffff) ;
	m_pOKButton->SetColor( 0xffffffff) ;
	m_pOKButton->OnClickEvent = DlgClick ;
	m_pOKButton->SetInstance( RYLTAXRATEDLG::OK_BUTTON ) ;
	m_pOKButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pOKButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pOKButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pOKButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pOKButton ) ;

	// 취소 버튼
	m_pCancelButton = new CRYLButton( this ) ;
	m_pCancelButton->SetLeft( 133 ) ;
	m_pCancelButton->SetTop( 120 ) ;
	m_pCancelButton->SetCaption( CRYLStringTable::m_strString[ 1897 ] ) ;
	m_pCancelButton->SetFontColor( 0xffffffff) ;
	m_pCancelButton->SetColor( 0xffffffff) ;
	m_pCancelButton->OnClickEvent = DlgClick ;
	m_pCancelButton->SetInstance( RYLTAXRATEDLG::CANCEL_BUTTON ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pCancelButton ) ;
}

VOID	CRYLTaxrateDlg::FinalCtrl() 
{
	GM_DELETE( m_pTaxrateLabel ) ;			// 세율변경
	GM_DELETE( m_pCurrentTaxLabel ) ;		// 현제 세율은 000%입니다.
	GM_DELETE( m_pChangeTaxLabel ) ;		// 세율을 변경 하겠습니까? ( 0 - 100% )
	GM_DELETE( m_pTaxpercentLabel ) ;		// %
	GM_DELETE( m_pTaxEdit ) ;				// 세율 에디트 박스
	GM_DELETE( m_pOKButton ) ;				// 확인
	GM_DELETE( m_pCancelButton ) ;			// 취소
}

VOID	CRYLTaxrateDlg::InitValue() 
{
}

VOID	CRYLTaxrateDlg::Show( CGMUIObject* Sender ) 
{
}

VOID	CRYLTaxrateDlg::Hide( CGMUIObject* Sender ) 
{
}

VOID	CRYLTaxrateDlg::GMUIPaint( INT xx, INT yy ) 
{
	CRYLDialog::GMUIPaint( xx, yy ) ;
}

HRESULT		CRYLTaxrateDlg::Render( LPDIRECT3DDEVICE8 lpD3DDevice )  
{
	return S_OK ;
}

HRESULT		CRYLTaxrateDlg::FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) 
{
	return S_OK ;
}

HRESULT		CRYLTaxrateDlg::Update( BOOL &bClick, BOOL &bEdge ) 
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

VOID	CRYLTaxrateDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y ) 
{
	if ( !g_pRYLTaxrateDlg )
		return ;

	switch( Sender->GetInstance() )
	{
		case RYLTAXRATEDLG::OK_BUTTON :
			{
			}
			break ;

		case RYLTAXRATEDLG::CANCEL_BUTTON :
			{
			}
			break ;
	}
}

VOID	CRYLTaxrateDlg::DlgKeyDown( CGMUIObject* Sender, WORD key, INT Shift ) 
{
}