#include "RYLSelectedConfirmDlg.h"
#include "RYLButton.h"
#include "RYLLabel.h"
#include "RYLCheckBox.h"
#include "RYLImageManager.h"
#include "RYLImage.h"
#include "GMImageDib.h"
#include "RYLImage.h"
#include "RYLSpriteList.h"
#include "RYLImageManager.h"
#include "RYLLoginScene.h"
#include "RYLIntegServerSelCharDlg.h"
#include "RYLIntegServerSelVaultDlg.h"
#include "RYLSceneManager.h"
#include "RYLMessageBox.h"
#include "RYLStringTable.h"
#include "GMMemory.h"

//------------------------------------------------------------------------------

CRYLSelectedConfirmDlg*	g_pRYLSelectedConfirmDlg = NULL ;

CRYLSelectedConfirmDlg::CRYLSelectedConfirmDlg () : CRYLDialog()
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

	CRYLSpriteList::Instance()->CreateGMImage( 256, 370, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	int i = 0;
	for (  ; i < 72 ; i ++ )
	{
		SetRect( &rt, 0, 38, 256, 43 ) ;
		pImageDIB->DrawRect( &rt, 0, ( i * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	}
	SetRect( &rt, 0, 42, 256, 52 ) ;
	pImageDIB->DrawRect( &rt, 0, ( ( i - 1 ) * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;

	pImageDIB->MakeFrameRect( 2, 20, 252, 1, 0xff946b42, 0x80000000,  1 ) ;		
	pImageDIB->MakeFrameRect( 33, 47, 195, 274, 0xff946b42, 0x80000000,  1 ) ;		
	for ( i = 0 ; i < 5 ; i ++ )
		pImageDIB->MakeFrameRect( 38, 69 + ( i * 38 ), 184, 35, 0xff959595, 0xc0000000,  1 ) ;

	pImageDIB->MakeFrameRect( 38, 280, 184, 35, 0xff959595, 0xc0000000,  1 ) ;

	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	AttachWindowTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;

	SetWidth( 256 ) ;
	SetHeight( 383 ) ;
	SetLeft( ( CGMUIScreen::GetInstance()->GetWidth() - GetWidth() ) / 2 ) ;
	SetTop( ( CGMUIScreen::GetInstance()->GetHeight() - GetHeight() ) / 2 ) ;

	SetFormType( RYLDLG_TYPE_CLOSEBUTTON ) ;
	
	g_pRYLSelectedConfirmDlg = this ;	
	InitCtrl() ;
}

CRYLSelectedConfirmDlg::~CRYLSelectedConfirmDlg ()
{
	FinalCtrl() ;
}

VOID CRYLSelectedConfirmDlg::InitCtrl ()
{
	CGMImageDib*	pSprite			= CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;

	for ( int i = 0 ; i < 5 ; i ++ )
	{
		int iJumpHeight = i * 38 ;
		
		m_ConfirmSlot[ i ].pCharNameLabel = new CRYLLabel( this ) ;
		m_ConfirmSlot[ i ].pCharNameLabel->SetLeft( 44 ) ;
		m_ConfirmSlot[ i ].pCharNameLabel->SetTop( 90 + iJumpHeight ) ;
		m_ConfirmSlot[ i ].pCharNameLabel->SetFontColor (0xffffffff);
		m_ConfirmSlot[ i ].pCharNameLabel->SetAutoSize( TRUE ) ;
		AttachGMUIControl( m_ConfirmSlot[ i ].pCharNameLabel ) ;

		m_ConfirmSlot[ i ].pServerAndClassLabel = new CRYLLabel( this ) ;
		m_ConfirmSlot[ i ].pServerAndClassLabel->SetLeft( 44 ) ;
		m_ConfirmSlot[ i ].pServerAndClassLabel->SetTop( 104 + iJumpHeight ) ;
		m_ConfirmSlot[ i ].pServerAndClassLabel->SetFontColor (0xffffffb2);
		m_ConfirmSlot[ i ].pServerAndClassLabel->SetAutoSize( TRUE ) ;
		AttachGMUIControl( m_ConfirmSlot[ i ].pServerAndClassLabel ) ;

		m_ConfirmSlot[ i ].pLevelLabel = new CRYLLabel( this ) ;
		m_ConfirmSlot[ i ].pLevelLabel->SetLeft( 185 ) ;
		m_ConfirmSlot[ i ].pLevelLabel->SetTop( 104 + iJumpHeight ) ;
		m_ConfirmSlot[ i ].pLevelLabel->SetFontColor (0xffffffb2);
		m_ConfirmSlot[ i ].pLevelLabel->SetAutoSize( TRUE ) ;
		AttachGMUIControl( m_ConfirmSlot[ i ].pLevelLabel ) ;
	}

	// 선택확인
	m_pTitleLabel = new CRYLLabel( this ) ;
	m_pTitleLabel->SetLeft( 7 ) ;
	m_pTitleLabel->SetTop( 19 ) ;
	m_pTitleLabel->SetFontColor (0xffffffff);
	m_pTitleLabel->SetAutoSize( TRUE ) ;
	m_pTitleLabel->SetCaption( CRYLStringTable::m_strString[ 3530 ] ) ;
	AttachGMUIControl( m_pTitleLabel ) ;  

	// 캐릭터 / 창고 선택결과
	m_pSelectResultLabel = new CRYLLabel( this ) ;
	m_pSelectResultLabel->SetLeft( 33 ) ;
	m_pSelectResultLabel->SetTop( 45 ) ;
	m_pSelectResultLabel->SetFontColor (0xffffffff);
	m_pSelectResultLabel->SetAutoSize( TRUE ) ;
	m_pSelectResultLabel->SetCaption( CRYLStringTable::m_strString[ 3531 ] ) ;
	AttachGMUIControl( m_pSelectResultLabel ) ;  
	
	// 캐릭터
	m_pCharLabel = new CRYLLabel( this ) ;
	m_pCharLabel->SetLeft( 42 ) ;
	m_pCharLabel->SetTop( 69 ) ;
	m_pCharLabel->SetFontColor (0xffffffff);
	m_pCharLabel->SetAutoSize( TRUE ) ;
	m_pCharLabel->SetCaption( CRYLStringTable::m_strString[ 1919 ] ) ;
	AttachGMUIControl( m_pCharLabel ) ;  

	// 창고
	m_pVaultLabel = new CRYLLabel( this ) ;
	m_pVaultLabel->SetLeft( 42 ) ;
	m_pVaultLabel->SetTop( 280 ) ;
	m_pVaultLabel->SetFontColor (0xffffffff);
	m_pVaultLabel->SetAutoSize( TRUE ) ;
	m_pVaultLabel->SetCaption( CRYLStringTable::m_strString[ 3532 ] ) ;
	AttachGMUIControl( m_pVaultLabel ) ;  

	// 창고 이름
	m_pVaultNameLabel = new CRYLLabel( this ) ;
	m_pVaultNameLabel->SetLeft( 44 ) ;
	m_pVaultNameLabel->SetTop( 300 ) ;
	m_pVaultNameLabel->SetFontColor (0xffffffff);
	m_pVaultNameLabel->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pVaultNameLabel ) ;  

	// 창고탭 개수
	m_pUsedNumLabel = new CRYLLabel( this ) ;
	m_pUsedNumLabel->SetLeft( 44 ) ;
	m_pUsedNumLabel->SetTop( 315 ) ;
	m_pUsedNumLabel->SetFontColor (0xffffffb2);
	m_pUsedNumLabel->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pUsedNumLabel ) ; 

	// 금고
	m_pGoldLabel = new CRYLLabel( this ) ;
	m_pGoldLabel->SetLeft( 78 ) ;
	m_pGoldLabel->SetTop( 316 ) ;
	m_pGoldLabel->SetFontColor (0xffffffb2);
	m_pGoldLabel->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pGoldLabel ) ; 

	// 캐릭터수정
	m_pEditCharButton = new CRYLButton( this ) ;
	m_pEditCharButton->SetLeft( 8 ) ;
	m_pEditCharButton->SetTop( 347 ) ;
	m_pEditCharButton->SetCaption( CRYLStringTable::m_strString[ 3533 ] ) ;
	m_pEditCharButton->SetFontColor( 0xffffffff) ;
	m_pEditCharButton->SetColor( 0xffffffff) ;
	m_pEditCharButton->OnClickEvent = DlgClick ;
	m_pEditCharButton->SetInstance( RYLSELECTEDCONFIRMDLG::CHAREDIT_BUTTON ) ;
	m_pEditCharButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pEditCharButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pEditCharButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pEditCharButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pEditCharButton ) ;

	// 창고수정
	m_pEditVaultButton = new CRYLButton( this ) ;
	m_pEditVaultButton->SetLeft( 89 ) ;
	m_pEditVaultButton->SetTop( 347 ) ;
	m_pEditVaultButton->SetCaption( CRYLStringTable::m_strString[ 3534 ] ) ;
	m_pEditVaultButton->SetFontColor( 0xffffffff) ;
	m_pEditVaultButton->SetColor( 0xffffffff) ;
	m_pEditVaultButton->OnClickEvent = DlgClick ;
	m_pEditVaultButton->SetInstance( RYLSELECTEDCONFIRMDLG::VAULTEDIT_BUTTON ) ;
	m_pEditVaultButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pEditVaultButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pEditVaultButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pEditVaultButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pEditVaultButton ) ;

	// 선택완료
	m_pConfirmButton = new CRYLButton( this ) ;
	m_pConfirmButton->SetLeft( 170 ) ;
	m_pConfirmButton->SetTop( 347 ) ;
	m_pConfirmButton->SetCaption( CRYLStringTable::m_strString[ 3360 ] ) ;
	m_pConfirmButton->SetFontColor( 0xffffffff) ;
	m_pConfirmButton->SetColor( 0xffffffff) ;
	m_pConfirmButton->OnClickEvent = DlgClick ;
	m_pConfirmButton->SetInstance( RYLSELECTEDCONFIRMDLG::CONFIRM_BUTTON ) ;
	m_pConfirmButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pConfirmButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pConfirmButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pConfirmButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pConfirmButton ) ;
}

VOID CRYLSelectedConfirmDlg::FinalCtrl ()
{
	for ( int i = 0 ; i < 5 ; i ++ )
	{
		GM_DELETE( m_ConfirmSlot[ i ].pCharNameLabel ) ;
		GM_DELETE( m_ConfirmSlot[ i ].pServerAndClassLabel ) ;
		GM_DELETE( m_ConfirmSlot[ i ].pLevelLabel ) ;
	}

	GM_DELETE( m_pTitleLabel ) ;			// 선택확인
	GM_DELETE( m_pSelectResultLabel ) ;		// 캐릭터 / 창고 선택결과
	
	GM_DELETE( m_pCharLabel ) ;				// 캐릭터
	GM_DELETE( m_pVaultLabel ) ;			// 창고
	GM_DELETE( m_pVaultNameLabel ) ;		// 창고 이름
	GM_DELETE( m_pUsedNumLabel ) ;			// 창고탭 개수
	GM_DELETE( m_pGoldLabel ) ;

	GM_DELETE( m_pEditCharButton ) ;		// 캐릭터 수정
	GM_DELETE( m_pEditVaultButton ) ;		// 창고수정
	GM_DELETE( m_pConfirmButton ) ;			// 선택완료
}

VOID CRYLSelectedConfirmDlg::Show( CGMUIObject* Sender )
{
	UpdateDlg() ;
}

VOID CRYLSelectedConfirmDlg::Hide( CGMUIObject* Sender )
{
}

VOID CRYLSelectedConfirmDlg::GMUIPaint( INT xx, INT yy )
{
	CRYLDialog::GMUIPaint( xx, yy ) ;
}

HRESULT	CRYLSelectedConfirmDlg::Update( BOOL &bClick, BOOL &bEdge ) 
{
	if (FALSE == GetVisible())
	{
		return S_OK;
	}
	if ( GetCloseButton() ) GetCloseButton()->SetVisible( FALSE ) ;

	return S_OK ;
}

VOID CRYLSelectedConfirmDlg::UpdateDlg() 
{
	CRYLLoginScene* pScene = ( CRYLLoginScene* )(CRYLSceneManager::Instance()->FindScene( LOGIN_SCENE ) ) ;
	CRYLIntegServerSelCharDlg* pCharDlg  = static_cast< CRYLIntegServerSelCharDlg* >( pScene->GetIntegServerSelCharDlg() ) ;
	CRYLIntegServerSelVaultDlg* pVaultDlg= static_cast< CRYLIntegServerSelVaultDlg* >( pScene->GetIntegServerSelVaultDlg() ) ;

	for ( int i = 0, iCnt = 0 ; i < 5 ; i ++ )	
	{
		m_ConfirmSlot[ i ].pCharNameLabel->SetCaption( "" ) ;
		m_ConfirmSlot[ i ].pServerAndClassLabel->SetCaption( "" ) ;
		m_ConfirmSlot[ i ].pLevelLabel->SetCaption( "" ) ;

		if ( pCharDlg->m_SelCharSlot[ i ].bEnable )
		{
			m_ConfirmSlot[ iCnt ].pCharNameLabel->SetCaption( pCharDlg->m_SelCharSlot[ i ].pCharNameLabel->GetCaption() ) ;
			m_ConfirmSlot[ iCnt ].pServerAndClassLabel->SetCaption( pCharDlg->m_SelCharSlot[ i ].pCharClassLabel->GetCaption() ) ;
			m_ConfirmSlot[ iCnt ].pLevelLabel->SetCaption( pCharDlg->m_SelCharSlot[ i ].pCharLevelLabel->GetCaption() ) ;
			++ iCnt ;
		}
	}

	if ( pVaultDlg->m_lpTmpVaultSlot )
	{
		m_pVaultNameLabel->SetCaption( pVaultDlg->m_lpTmpVaultSlot->pServerNameLabel->GetCaption() ) ;
		m_pUsedNumLabel->SetCaption( pVaultDlg->m_lpTmpVaultSlot->pUsedNumLabel->GetCaption() ) ;

		if ( pVaultDlg->m_lpTmpVaultSlot->pGoldLabel->GetVisible() )
			m_pGoldLabel->SetCaption( pVaultDlg->m_lpTmpVaultSlot->pGoldLabel->GetCaption() ) ;
	}
}

VOID CRYLSelectedConfirmDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y )
{
	if ( !g_pRYLSelectedConfirmDlg )
		return ;

	CRYLLoginScene* pScene = ( CRYLLoginScene* )(CRYLSceneManager::Instance()->FindScene( LOGIN_SCENE ) ) ;
	switch ( Sender->GetInstance() )
	{
	case RYLSELECTEDCONFIRMDLG::CHAREDIT_BUTTON :	
		{
			pScene->GetIntegServerSelCharDlg()->SetVisible( TRUE ) ;
			g_pRYLSelectedConfirmDlg->SetVisible( FALSE ) ;
		}
		break ;
	case RYLSELECTEDCONFIRMDLG::VAULTEDIT_BUTTON :	
		{
			CRYLIntegServerSelVaultDlg* pVaultDlg = static_cast< CRYLIntegServerSelVaultDlg* >( pScene->GetIntegServerSelVaultDlg() ) ;

			if ( pVaultDlg->m_dwStoreInfoNum_Out == 0 )
			{
				CRYLMessageBox* lpMessageBox = NULL;
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create( CRYLStringTable::m_strString[ 3535 ] ) ;
			}
			else
			{
				pScene->GetIntegServerSelVaultDlg()->SetVisible( TRUE ) ;
				g_pRYLSelectedConfirmDlg->SetVisible( FALSE ) ;
			}
		}
		break ;
	case RYLSELECTEDCONFIRMDLG::CONFIRM_BUTTON :	
		{
			char buff[ 512 ] ;
			sprintf( buff, "%s%s%s",
				CRYLStringTable::m_strString[ 3536 ],
				CRYLStringTable::m_strString[ 3537 ],
				CRYLStringTable::m_strString[ 3538 ] ) ;

			CRYLMessageBox *lpMessageBox ;
			lpMessageBox = new CRYLMessageBox ; 
			lpMessageBox->Create( CRYLStringTable::m_strString[ 3539 ], CRYLStringTable::m_strString[ 3360 ], buff, MB_YES | MB_NO | MB_EXIT ) ;
			lpMessageBox->SetResult( &pScene->m_dwConfirmResult ) ;
			g_pRYLSelectedConfirmDlg->SetVisible( FALSE ) ;
		}
		break ;
	}
}

