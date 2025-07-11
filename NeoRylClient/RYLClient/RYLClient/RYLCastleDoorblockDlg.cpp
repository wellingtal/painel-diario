
#include <winsock2.h>

#include <Network/ClientSocket/Send/SendCastle.h>

#include "WinInput.h"
#include "RYLImageManager.h"
#include "RYLImageCabinet.h"
#include "RYLSpriteList.h"
#include "GMImageDib.h"
#include "RYLImage.h"
#include "RYLButton.h"
#include "RYLLabel.h"
#include "RYLEdit.h"
#include "RYLStringTable.h"

#include "RYLClientMain.h"
#include "RYLGameData.h"
#include "RYLNetworkData.h"
#include "RYLCommunityData.h"

#include "RYLCastleDoorblockDlg.h"
#include "GMMemory.h"

CRYLCastleDoorblockDlg*	g_pRYLCastleDoorblockDlg = NULL ;

CRYLCastleDoorblockDlg::CRYLCastleDoorblockDlg() 
{
	RECT rt ;
	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;

	CRYLSpriteList::Instance()->CreateGMImage( 256, 15, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 0, 256, 15 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	AttachCaptionTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;

	CRYLSpriteList::Instance()->CreateGMImage( 256, 145, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	int i = 0 ;
	for ( ; i < 27 ; i ++ )
	{
		SetRect( &rt, 0, 38, 256, 43 ) ;
		pImageDIB->DrawRect( &rt, 0, ( i * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	}
	SetRect( &rt, 0, 42, 256, 52 ) ;
	pImageDIB->DrawRect( &rt, 0, ( ( i - 1 ) * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	
	pImageDIB->MakeFrameRect( 2, 20, 252, 1, 0xff946b42, 0xc0000000,  1 ) ;	
	pImageDIB->MakeFrameRect( 89, 43, 77, 19, 0xff959595, 0x80000000,  1 ) ;
	pImageDIB->MakeFrameRect( 89, 79, 77, 19, 0xff959595, 0x80000000,  1 ) ;

	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
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
	
	g_pRYLCastleDoorblockDlg = this ;
}

CRYLCastleDoorblockDlg::~CRYLCastleDoorblockDlg() 
{
	FinalCtrl() ;
}

VOID	CRYLCastleDoorblockDlg::InitCtrl() 
{
	CGMImageDib* pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;

	// 성문막기
	m_pCastleDoorLabel = new CRYLLabel( this ) ;
	m_pCastleDoorLabel->SetLeft( 11 ) ;
	m_pCastleDoorLabel->SetTop( 18 ) ;
	m_pCastleDoorLabel->SetFontColor (0xffffffff);
	m_pCastleDoorLabel->SetAutoSize( TRUE ) ;
	m_pCastleDoorLabel->SetCaption( CRYLStringTable::m_strString[ 2558 ] ) ;
	AttachGMUIControl( m_pCastleDoorLabel ) ;

	// 데미지 감소율(%)
	m_pDamageDecLabel = new CRYLLabel( this ) ;
	m_pDamageDecLabel->SetLeft( 79 ) ;
	m_pDamageDecLabel->SetTop( 43 ) ;
	m_pDamageDecLabel->SetFontColor (0xffffffff);
	m_pDamageDecLabel->SetAutoSize( TRUE ) ;
	m_pDamageDecLabel->SetCaption( CRYLStringTable::m_strString[ 2559 ] ) ;
	AttachGMUIControl( m_pDamageDecLabel ) ;

	// 성문의 남은 HP
	m_pRemainHPLabel = new CRYLLabel( this ) ;
	m_pRemainHPLabel->SetLeft( 87) ;
	m_pRemainHPLabel->SetTop( 81 ) ;
	m_pRemainHPLabel->SetFontColor (0xffffffff);
	m_pRemainHPLabel->SetAutoSize( TRUE ) ;
	m_pRemainHPLabel->SetCaption( CRYLStringTable::m_strString[ 2560 ] ) ;
	AttachGMUIControl( m_pRemainHPLabel ) ;

	// 데미지 감소율(%)
	m_pDamageDecLabelValue = new CRYLLabel( this ) ;
	m_pDamageDecLabelValue->SetLeft( 95 ) ;
	m_pDamageDecLabelValue->SetTop( 62 ) ;
	m_pDamageDecLabelValue->SetFontColor (0xffffffff);
	m_pDamageDecLabelValue->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pDamageDecLabelValue ) ;

	// 데미지 감소율(%)
	m_pRemainHPLabelValue = new CRYLLabel( this ) ;
	m_pRemainHPLabelValue->SetLeft( 95 ) ;
	m_pRemainHPLabelValue->SetTop( 98 ) ;
	m_pRemainHPLabelValue->SetFontColor (0xffffffff);
	m_pRemainHPLabelValue->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pRemainHPLabelValue ) ;

	pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;
	// 취소 버튼
	m_pCancelButton = new CRYLButton( this ) ;
	m_pCancelButton->SetLeft( 89 ) ;
	m_pCancelButton->SetTop( 120 ) ;
	m_pCancelButton->SetCaption( CRYLStringTable::m_strString[ 2561 ] ) ;
	m_pCancelButton->SetFontColor( 0xffffffff) ;
	m_pCancelButton->SetColor( 0xffffffff) ;
	m_pCancelButton->OnClickEvent = DlgClick ;
	m_pCancelButton->SetInstance( RYLCASTLEDOORBLOCKDLG::CANCEL_BUTTON ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pCancelButton ) ;
}

VOID	CRYLCastleDoorblockDlg::FinalCtrl() 
{
	GM_DELETE( m_pCastleDoorLabel ) ;
	GM_DELETE( m_pDamageDecLabel ) ;
	GM_DELETE( m_pRemainHPLabel ) ;
	GM_DELETE( m_pDamageDecLabelValue ) ;
	GM_DELETE( m_pRemainHPLabelValue ) ;
	GM_DELETE( m_pCancelButton ) ;	
}

VOID	CRYLCastleDoorblockDlg::InitValue() 
{
}

VOID	CRYLCastleDoorblockDlg::Show( CGMUIObject* Sender ) 
{
}

VOID	CRYLCastleDoorblockDlg::Hide( CGMUIObject* Sender ) 
{
}

VOID	CRYLCastleDoorblockDlg::GMUIPaint( INT xx, INT yy ) 
{	
	CRYLDialog::GMUIPaint( xx, yy ) ;
}

HRESULT		CRYLCastleDoorblockDlg::Render( LPDIRECT3DDEVICE8 lpD3DDevice )  
{
	return S_OK ;
}

HRESULT		CRYLCastleDoorblockDlg::FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) 
{
	return S_OK ;
}

HRESULT		CRYLCastleDoorblockDlg::Update( BOOL &bClick, BOOL &bEdge ) 
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

VOID	CRYLCastleDoorblockDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y ) 
{
	if ( !g_pRYLCastleDoorblockDlg )
		return ;

	switch( Sender->GetInstance() )
	{
		case RYLCASTLEDOORBLOCKDLG::CANCEL_BUTTON :
			{
                RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( CRYLCommunityData::Instance()->m_dwCastleDoorID ) ;

				SendPacket::CharCastleCmd( g_GameSession, 
										   CRYLNetworkData::Instance()->m_dwMyChrID, 
										   pTempCreature->GetCastleID(),   
										   CRYLCommunityData::Instance()->m_dwCastleDoorID, 
										   0, 0, 
										   PktCastleCmd::CASTLE_GATE_PROTECT_CANCEL ) ;
			}
			break ;
	}
}

VOID	CRYLCastleDoorblockDlg::DlgKeyDown( CGMUIObject* Sender, WORD key, INT Shift ) 
{
}

void	CRYLCastleDoorblockDlg::SetData( unsigned long ulDec, unsigned long ulRemainHP ) 
{
	char szBuff[ 32 ] ;
	sprintf( szBuff, "%d", ulDec ) ;
	m_pDamageDecLabelValue->SetCaption( szBuff ) ;
	sprintf( szBuff, "%d", ulRemainHP ) ;
	m_pRemainHPLabelValue->SetCaption( szBuff ) ;
}