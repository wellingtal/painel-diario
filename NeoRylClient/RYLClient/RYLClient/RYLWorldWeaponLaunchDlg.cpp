
#include <winsock2.h>

#include <Item/ItemMgr.h>
#include <Item/ItemFactory.h>
#include <Network/ClientSocket/Send/SendCastle.h>

#include "WinInput.h"
#include "GMImageDib.h"

#include "RYLImageManager.h"
#include "RYLImageCabinet.h"
#include "RYLImage.h"
#include "RYLSpriteList.h"
#include "RYLButton.h"
#include "RYLLabel.h"
#include "RYLEdit.h"
#include "RYLMessageBox.h"
#include "RYLStringTable.h"

#include "RYLGameScene.h"
#include "RYLSceneManager.h"
#include "RYLGameData.h"
#include "RYLNetworkData.h"

#include "RYLCampManager.h"
#include "RYLCamp.h"
#include "RYLWorldWeaponLaunchDlg.h"
#include "RYLWorldWeaponLoadDlg.h"
#include "RYLChattingDlg.h"
#include "GMMemory.h"

CRYLWorldWeaponLaunchDlg*	g_pRYLWorldWeaponLaunchDlg = NULL ;

CRYLWorldWeaponLaunchDlg::CRYLWorldWeaponLaunchDlg() 
{
	RECT rt ;
	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;
	m_pWeaponLaunchLabel = NULL;
	m_pWeaponLaunchPosionLabel = NULL ;		// 발사 좌표
	m_pWeaponLaunchExpla1Label = NULL ;		// 웨폰 발사 설명 1
	m_pWeaponLaunchExpla2Label = NULL ;		// 웨폰 발사 설명 2
	m_pWeaponLaunchPosXLabel = NULL ;			// X 좌표
	m_pWeaponLaunchPosYLabel = NULL ;			// Y 좌표
	m_pOKButton = NULL ;
	m_pCancelButton = NULL ;
	m_pPosXEdit = NULL ;						//좌표X
	m_pPosYEdit = NULL ;						//좌표Y

	CRYLSpriteList::Instance()->CreateGMImage( 256, 15, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 0, 256, 15 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	AttachCaptionTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;

	CRYLSpriteList::Instance()->CreateGMImage( 256, 170, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	int i = 0;
	for (  ; i < 33 ; i ++ )
	{
		SetRect( &rt, 0, 38, 256, 43 ) ;
		pImageDIB->DrawRect( &rt, 0, ( i * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	}
	SetRect( &rt, 0, 42, 256, 52 ) ;
	pImageDIB->DrawRect( &rt, 0, ( ( i - 1 ) * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	
	pImageDIB->MakeFrameRect( 2, 20, 252, 1, 0xff946b42, 0xc0000000,  1 ) ;	

	pImageDIB->MakeFrameRect( 100, 59-15, 75, 17, 0xff959595, 0x80000000,  1 ) ;	
	pImageDIB->MakeFrameRect( 100, 81-15, 75, 17, 0xff959595, 0x80000000,  1 ) ;	
	
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	AttachWindowTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;

	
	SetWidth( 256 ) ;
	SetHeight( 182 ) ;
	SetLeft( ( CGMUIScreen::GetInstance()->GetWidth() - GetWidth() ) / 2 ) ;
	SetTop( ( CGMUIScreen::GetInstance()->GetHeight() - GetHeight() ) / 2 ) ;
	SetFormType( RYLDLG_TYPE_CLOSEBUTTON ) ;

	m_nPosX = 0;
	m_nPosY = 0;
	m_dwCampID = 0;

	InitCtrl() ;
	
	g_pRYLWorldWeaponLaunchDlg = this ;


}

CRYLWorldWeaponLaunchDlg::~CRYLWorldWeaponLaunchDlg() 
{
	FinalCtrl() ;
}

VOID	CRYLWorldWeaponLaunchDlg::InitCtrl() 
{
	CGMImageDib* pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;

	// 월드 웨폰 발사
	m_pWeaponLaunchLabel = new CRYLLabel( this ) ;
	m_pWeaponLaunchLabel->SetLeft( 11 ) ;
	m_pWeaponLaunchLabel->SetTop( 18 ) ;
	m_pWeaponLaunchLabel->SetFontColor (0xffffffff);
	m_pWeaponLaunchLabel->SetAutoSize( TRUE ) ;
	m_pWeaponLaunchLabel->SetCaption( CRYLStringTable::m_strString[ 3185 ] ) ;
	AttachGMUIControl( m_pWeaponLaunchLabel ) ;

	
	//좌표X
	m_pPosXEdit = new CRYLEdit( this ) ;
	m_pPosXEdit->SetLeft( 100 ) ;
	m_pPosXEdit->SetTop( 60 ) ;
	m_pPosXEdit->SetWidth( 73 ) ;
	m_pPosXEdit->SetHeight( 15 ) ;
	m_pPosXEdit->SetColor( 0xFF000000) ;
	m_pPosXEdit->SetFontColor( 0xffffffff ) ;
	m_pPosXEdit->SetAlpha( 0xFF000000 );
	m_pPosXEdit->SetInstance( RYLWORLDWEAPONLAUNCHDLG::ID_POSX_EDIT ) ;
	m_pPosXEdit->SetMaxLength( 5 ) ; 
	m_pPosXEdit->SetNumeralOnly( TRUE );

	m_pPosXEdit->OnKeyDown = DlgKeyDown; 
	AttachGMUIControl( m_pPosXEdit ) ;


	//좌표Y
	m_pPosYEdit = new CRYLEdit( this ) ;
	m_pPosYEdit->SetLeft( 100 ) ;
	m_pPosYEdit->SetTop( 82 ) ;
	m_pPosYEdit->SetWidth( 73 ) ;
	m_pPosYEdit->SetHeight( 15 ) ;
	m_pPosYEdit->SetColor( 0xFF000000) ;
	m_pPosYEdit->SetFontColor( 0xffffffff ) ;
	m_pPosYEdit->SetAlpha( 0xFF000000 );
	m_pPosYEdit->SetInstance( RYLWORLDWEAPONLAUNCHDLG::ID_POSY_EDIT ) ;
	m_pPosYEdit->SetMaxLength( 5 ) ; 
	m_pPosYEdit->SetNumeralOnly( TRUE );
	m_pPosYEdit->OnKeyDown = DlgKeyDown; 
	AttachGMUIControl( m_pPosYEdit ) ;

	// 발사 좌표
	m_pWeaponLaunchPosionLabel = new CRYLLabel( this ) ;
	m_pWeaponLaunchPosionLabel->SetLeft( 11 ) ;
	m_pWeaponLaunchPosionLabel->SetTop( 43 ) ;
	m_pWeaponLaunchPosionLabel->SetFontColor (0xffffffff);
	m_pWeaponLaunchPosionLabel->SetAutoSize( TRUE ) ;
	m_pWeaponLaunchPosionLabel->SetCaption( CRYLStringTable::m_strString[ 3186 ] ) ;
	AttachGMUIControl( m_pWeaponLaunchPosionLabel ) ;

	// 웨폰 발사 설명 1
	m_pWeaponLaunchExpla1Label = new CRYLLabel( this ) ;
	m_pWeaponLaunchExpla1Label->SetLeft( 11 ) ;
	m_pWeaponLaunchExpla1Label->SetTop( 108 ) ;
	m_pWeaponLaunchExpla1Label->SetFontColor (0xffffffff);
	m_pWeaponLaunchExpla1Label->SetAutoSize( TRUE ) ;
	m_pWeaponLaunchExpla1Label->SetCaption( CRYLStringTable::m_strString[ 3189 ] ) ;
	AttachGMUIControl( m_pWeaponLaunchExpla1Label ) ;


	// 웨폰 발사 설명 2
	m_pWeaponLaunchExpla2Label = new CRYLLabel( this ) ;
	m_pWeaponLaunchExpla2Label->SetLeft( 11 ) ;
	m_pWeaponLaunchExpla2Label->SetTop( 124 ) ;
	m_pWeaponLaunchExpla2Label->SetFontColor (0xffffffff);
	m_pWeaponLaunchExpla2Label->SetAutoSize( TRUE ) ;
	m_pWeaponLaunchExpla2Label->SetCaption( CRYLStringTable::m_strString[ 3190 ] ) ;
	AttachGMUIControl( m_pWeaponLaunchExpla2Label ) ;


	// X 좌표
	m_pWeaponLaunchPosXLabel = new CRYLLabel( this ) ;
	m_pWeaponLaunchPosXLabel->SetLeft( 51 ) ;
	m_pWeaponLaunchPosXLabel->SetTop( 62 ) ;
	m_pWeaponLaunchPosXLabel->SetFontColor (0xffffffff);
	m_pWeaponLaunchPosXLabel->SetAutoSize( TRUE ) ;
	m_pWeaponLaunchPosXLabel->SetCaption( CRYLStringTable::m_strString[ 3187 ] ) ;
	AttachGMUIControl( m_pWeaponLaunchPosXLabel ) ;

	// Y 좌표
	m_pWeaponLaunchPosYLabel = new CRYLLabel( this ) ;
	m_pWeaponLaunchPosYLabel->SetLeft( 51 ) ;
	m_pWeaponLaunchPosYLabel->SetTop( 84 ) ;
	m_pWeaponLaunchPosYLabel->SetFontColor (0xffffffff);
	m_pWeaponLaunchPosYLabel->SetAutoSize( TRUE ) ;
	m_pWeaponLaunchPosYLabel->SetCaption( CRYLStringTable::m_strString[ 3188 ] ) ;
	AttachGMUIControl( m_pWeaponLaunchPosYLabel ) ;



	pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;
	
	// 확인 버튼
	m_pOKButton = new CRYLButton( this ) ;
	m_pOKButton->SetLeft( 45 ) ;
	m_pOKButton->SetTop( 145 ) ;
	m_pOKButton->SetCaption( CRYLStringTable::m_strString[ 2164 ] ) ;
	m_pOKButton->SetFontColor( 0xffffffff) ;
	m_pOKButton->SetColor( 0xffffffff) ;
	m_pOKButton->OnClickEvent = DlgClick ;
	m_pOKButton->SetInstance( RYLWORLDWEAPONLAUNCHDLG::OK_BUTTON ) ;
	m_pOKButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pOKButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pOKButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pOKButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pOKButton ) ;

	// 취소 버튼
	m_pCancelButton = new CRYLButton( this ) ;
	m_pCancelButton->SetLeft( 133 ) ;
	m_pCancelButton->SetTop( 145 ) ;
	m_pCancelButton->SetCaption( CRYLStringTable::m_strString[ 1897 ] ) ;
	m_pCancelButton->SetFontColor( 0xffffffff) ;
	m_pCancelButton->SetColor( 0xffffffff) ;
	m_pCancelButton->OnClickEvent = DlgClick ;
	m_pCancelButton->SetInstance( RYLWORLDWEAPONLAUNCHDLG::CANCEL_BUTTON ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pCancelButton ) ;
}

VOID	CRYLWorldWeaponLaunchDlg::FinalCtrl() 
{
	GM_DELETE( m_pWeaponLaunchLabel ) ;			// 병기 생성 설명 5
	GM_DELETE( m_pWeaponLaunchLabel );
	GM_DELETE( m_pWeaponLaunchPosionLabel ) ;		// 발사 좌표
	GM_DELETE( m_pWeaponLaunchExpla1Label ) ;		// 웨폰 발사 설명 1
	GM_DELETE( m_pWeaponLaunchExpla2Label ) ;		// 웨폰 발사 설명 2
	GM_DELETE( m_pWeaponLaunchPosXLabel ) ;			// X 좌표
	GM_DELETE( m_pWeaponLaunchPosYLabel ) ;			// Y 좌표
	GM_DELETE( m_pOKButton ) ;
	GM_DELETE( m_pCancelButton ) ;
	GM_DELETE( m_pPosXEdit ) ;						//좌표X
	GM_DELETE( m_pPosYEdit ) ;						//좌표Y

	GM_DELETE( m_pOKButton ) ;
	GM_DELETE( m_pCancelButton ) ;
}

VOID	CRYLWorldWeaponLaunchDlg::InitValue() 
{
}

VOID	CRYLWorldWeaponLaunchDlg::Show( CGMUIObject* Sender ) 
{
	m_pPosXEdit->SetFocus();
	m_pPosXEdit->SetFocusState( TRUE );

	CRYLCamp* pCamp = CRYLCampManager::Instance()->FindCamp( m_dwCampID ) ;
	if ( pCamp )
	{
		RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( pCamp->GetCID() ) ;
		if( pTempCreature && pTempCreature->GetSubState() == Siege::WEAPON_READY )
		{
			m_pOKButton->SetEnable( TRUE );
			return;
		}
	}
	m_pOKButton->SetEnable( FALSE );
}

VOID	CRYLWorldWeaponLaunchDlg::Hide( CGMUIObject* Sender ) 
{
	m_pPosXEdit->SetFocusState( FALSE );
	m_pPosYEdit->SetFocusState( FALSE );
	
	m_pPosXEdit->SetText("");
	m_pPosYEdit->SetText("");

	if ( CRYLChattingDlg::Instance() )
    {
		CRYLChattingDlg::Instance()->m_pChatEdit->SetFocus();
        CRYLChattingDlg::Instance()->m_pChatEdit->SetFocusState( FALSE );
    }

	CRYLGameData*	pGame = CRYLGameData::Instance() ;
	pGame->SetEnableMove( TRUE ) ;
}

VOID	CRYLWorldWeaponLaunchDlg::GMUIPaint( INT xx, INT yy ) 
{
	LPDIRECT3DDEVICE8   lpD3DDevice = BaseGraphicsLayer::GetDevice() ;
	CRYLDialog::GMUIPaint( xx, yy ) ;

	CRYLGameData* pGame = CRYLGameData::Instance() ;


}

HRESULT		CRYLWorldWeaponLaunchDlg::Render( LPDIRECT3DDEVICE8 lpD3DDevice )  
{
	return S_OK ;
}

HRESULT		CRYLWorldWeaponLaunchDlg::FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) 
{
	return S_OK ;
}

HRESULT		CRYLWorldWeaponLaunchDlg::Update( BOOL &bClick, BOOL &bEdge ) 
{
	if (FALSE == GetVisible())
	{
		return S_OK;
	}

	CRYLGameData*	pGame = CRYLGameData::Instance() ;
	pGame->SetAutoRun( FALSE ) ;
	pGame->SetEnableMove( FALSE ) ;

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

BOOL	CRYLWorldWeaponLaunchDlg::CheckLaunch() 
{
	CRYLCamp* pCamp = CRYLCampManager::Instance()->FindCamp( m_dwCampID ) ;
	if ( pCamp )
	{
		RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( pCamp->GetCID() ) ;
		if( pTempCreature && pTempCreature->GetSubState() == Siege::WEAPON_READY )
		{
			string  strTemp1 = g_pRYLWorldWeaponLaunchDlg->m_pPosXEdit->GetText();
			string  strTemp2 = g_pRYLWorldWeaponLaunchDlg->m_pPosYEdit->GetText();
			m_nPosX = atoi( strTemp1.c_str() );
			m_nPosY = atoi( strTemp2.c_str() );
			// 범위 체크
			return TRUE;
		}
	}
	return FALSE;
}

VOID	CRYLWorldWeaponLaunchDlg::WorldWeaponLaunch()
{
	CRYLNetworkData* pNetwork = CRYLNetworkData::Instance() ;
	SendPacket::CharCampCmd(	g_GameSession, 
								pNetwork->m_dwMyChrID, 
								m_dwCampID, 
								m_nPosX, m_nPosY,
								PktCampCmd::WORLDWEAPON_FIRE ) ;
}

VOID	CRYLWorldWeaponLaunchDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y ) 
{
	if ( !g_pRYLWorldWeaponLaunchDlg )
		return ;

	switch( Sender->GetInstance() )
	{
		case RYLWORLDWEAPONLAUNCHDLG::OK_BUTTON :
			{
				if( g_pRYLWorldWeaponLaunchDlg->CheckLaunch() )
				{
					g_pRYLWorldWeaponLaunchDlg->WorldWeaponLaunch();
					g_pRYLWorldWeaponLaunchDlg->SetVisible( FALSE ) ;
				}
			}
			break ;
		case RYLWORLDWEAPONLAUNCHDLG::CANCEL_BUTTON :
			{	
				g_pRYLWorldWeaponLaunchDlg->SetVisible( FALSE ) ;
			}
			break ;
	}

//	발사 이펙트 테스트 코드
/* 
	if ( !g_pRYLWorldWeaponLaunchDlg )
		return ;

	switch( Sender->GetInstance() )
	{
		case RYLWORLDWEAPONLAUNCHDLG::OK_BUTTON :
		{
			if ( g_pRYLWorldWeaponLaunchDlg->CheckLaunch() )
			{
				CRYLGameData* pGame = CRYLGameData::Instance() ;
				CEffScript *test_script;
				test_script = new CEffScript;
				test_script->GetScriptBinData( "WWeaponHuman.esf" );

				CSceneManager::AddEffectScript( test_script );
				vector3 vecHightField = vector3( g_pRYLWorldWeaponLaunchDlg->GetPosX() * 100, 0.0f, g_pRYLWorldWeaponLaunchDlg->GetPosY() * 100 ) ;
				vecHightField.y = CSceneManager::m_HeightField.GetHeight( vecHightField ) ;

				test_script->SetStartPos( g_pRYLWorldWeaponLaunchDlg->GetPosX() * 100, vecHightField.y, g_pRYLWorldWeaponLaunchDlg->GetPosY() * 100 );
				test_script->SetEndPos( g_pRYLWorldWeaponLaunchDlg->GetPosX() * 100, vecHightField.y, g_pRYLWorldWeaponLaunchDlg->GetPosY() * 100 );
				test_script->SetChrFwd(0.0f,0.0f,1.0f);	
				test_script->SetChr( NULL, NULL);
			}

			g_pRYLWorldWeaponLaunchDlg->SetVisible( FALSE ) ;
		}
		break ;

		case RYLWORLDWEAPONLAUNCHDLG::CANCEL_BUTTON :
		{	
			if ( g_pRYLWorldWeaponLaunchDlg->CheckLaunch() )
			{
				CRYLGameData* pGame = CRYLGameData::Instance() ;
				CEffScript *test_script;
				test_script = new CEffScript;
				test_script->GetScriptBinData( "WWeaponAhkan.esf" );

				CSceneManager::AddEffectScript( test_script );
				vector3 vecHightField = vector3( g_pRYLWorldWeaponLaunchDlg->GetPosX() * 100, 0.0f, g_pRYLWorldWeaponLaunchDlg->GetPosY() * 100 ) ;
				vecHightField.y = CSceneManager::m_HeightField.GetHeight( vecHightField ) ;

				test_script->SetStartPos( g_pRYLWorldWeaponLaunchDlg->GetPosX() * 100, vecHightField.y, g_pRYLWorldWeaponLaunchDlg->GetPosY() * 100 );
				test_script->SetEndPos( g_pRYLWorldWeaponLaunchDlg->GetPosX() * 100, vecHightField.y, g_pRYLWorldWeaponLaunchDlg->GetPosY() * 100 );
				test_script->SetChrFwd(0.0f,0.0f,1.0f);	
				test_script->SetChr( NULL, NULL);
			}

			g_pRYLWorldWeaponLaunchDlg->SetVisible( FALSE ) ;
		}
		break ;
	}
*/
}


VOID 
CRYLWorldWeaponLaunchDlg::DlgKeyDown( CGMUIObject* Sender, WORD key, INT Shift)
{
	if ( !g_pRYLWorldWeaponLaunchDlg )
		return ;

	switch ( Sender->GetInstance() )
	{
		case RYLWORLDWEAPONLAUNCHDLG::ID_POSX_EDIT :
			if (key == 13)
				g_pRYLWorldWeaponLaunchDlg->m_pPosYEdit->SetFocus ();
			break;
		case RYLWORLDWEAPONLAUNCHDLG::ID_POSY_EDIT : 
			if (key == 13)
				g_pRYLWorldWeaponLaunchDlg->m_pPosXEdit->SetFocus () ;
			break;
	}
}

VOID
CRYLWorldWeaponLaunchDlg::Effect( DWORD nPosX, DWORD nPosZ )
{
	CRYLGameData* pGame = CRYLGameData::Instance() ;
	CEffScript *test_script;
	CRYLCamp* pCamp = CRYLCampManager::Instance()->FindCamp( m_dwCampID ) ;
	if ( pCamp )
	{
		RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( pCamp->GetCID() ) ;
		if( pTempCreature && pTempCreature->GetObjectType() == Siege::MERKADIA_WEAPON )
		{
			test_script = new CEffScript;
			test_script->GetScriptBinData( "WWeaponAhkan.esf" );
		}
		else
		{
			test_script = new CEffScript;
			test_script->GetScriptBinData( "WWeaponHuman.esf" );
		}
	}
	else
	{
		return ;
	}

	CSceneManager::AddEffectScript( test_script );
	vector3 vecHightField = vector3( nPosX, 0.0f, nPosZ ) ;
	vecHightField.y = CSceneManager::m_HeightField.GetHeight( vecHightField ) ;

	test_script->SetStartPos( nPosX, vecHightField.y, nPosZ );
	test_script->SetEndPos( nPosX, vecHightField.y, nPosZ );
	test_script->SetChrFwd(0.0f,0.0f,1.0f);	
	test_script->SetChr( NULL, NULL);
}