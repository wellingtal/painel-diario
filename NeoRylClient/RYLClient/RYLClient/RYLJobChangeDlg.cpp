#include <winsock2.h>
#include "RYLClientMain.h"
#include "RYLJobChangeDlg.h"
#include "GMUIObject.h"
#include "BaseDataDefine.h"
#include "RYLTabButton.h"
#include "RYLImageManager.h"
#include "RYLImage.h"
#include "RYLSpriteList.h"
#include "GMImageDib.h"
#include "RYLGameData.h"
#include "RYLNetworkData.h"
#include "RYLStringTable.h"
#include "RYLChattingDlg.h"
#include "RYLListBox.h"
#include "RYLLabel.h"
#include <WinInput.h>

#include "BaseGraphicsLayer.h"
#include "RYLMessageBox.h"
#include "RYLMessageBoxManager.h"
#include <Network/ClientSocket/Send/SendEtc.h>
#include <RenderOption.h>
#include "GMMemory.h"

CRYLJobChangeDlg*	g_pJobChangeDlg = NULL ;

CRYLJobChangeDlg::CRYLJobChangeDlg()  : CRYLDialog()
{
	m_dwChangeResult = 0L ;
	m_wClassChange   = 0L ;

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


	CRYLSpriteList::Instance()->CreateGMImage( 256, 397, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 15, 256, 37 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	for ( int i = 0 ; i < 74 ; i ++ )
	{
		SetRect( &rt, 0, 38, 256, 43 ) ;
		pImageDIB->DrawRect( &rt, 0, 21 + ( i * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	}
	SetRect( &rt, 0, 42, 256, 52 ) ;
	pImageDIB->DrawRect( &rt, 0, 387, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	
	pImageDIB->DrawFrameRect( 2, 0, 252, 21, 0xc0000000 ) ;
	pImageDIB->DrawFrameRect( 2, 20, 252, 1, 0xffa88a60 ) ;
	
	pImageDIB->MakeFrameRect( 7, 29, 242, 327, 0xff6e6857, 0xc0000000,  1 ) ;		
	
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	AttachWindowTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;
	
	SetWidth( 256 ) ;
	SetHeight( 419 ) ;
	SetLeft( ( CGMUIScreen::GetInstance()->GetWidth() - GetWidth() ) / 2 ) ;                                
	SetTop( ( CGMUIScreen::GetInstance()->GetHeight() - GetHeight() ) / 2 ) ;

	SetFormType( RYLDLG_TYPE_CLOSEBUTTON ) ;

	g_pJobChangeDlg = this ;
	InitCtrl() ;
}

CRYLJobChangeDlg::~CRYLJobChangeDlg() 
{
	FinalCtrl() ;
}

VOID	CRYLJobChangeDlg::InitCtrl() 
{
	RECT rt ;
	CGMImageDib* pSprite			= CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;

	// 전직하기
	m_pChangeJobLabel = new CRYLLabel( this ) ;		
	m_pChangeJobLabel->SetLeft( 9 ) ;
	m_pChangeJobLabel->SetTop( 18 ) ;
	m_pChangeJobLabel->SetFontColor (0xffffffff);
	m_pChangeJobLabel->SetAutoSize( TRUE ) ;
	m_pChangeJobLabel->SetCaption( CRYLStringTable::m_strString[ 2166 ] ) ;
	AttachGMUIControl( m_pChangeJobLabel ) ;

	// 워리어 
	m_pWarriorButton = new CRYLButton( this ) ;
	m_pWarriorButton->SetLeft( 172 ) ;
	m_pWarriorButton->SetTop( 377 ) ;
	m_pWarriorButton->SetCaption( CRYLStringTable::m_strString[ 456 ] ) ;
	m_pWarriorButton->SetFontColor( 0xffffffff) ;
	m_pWarriorButton->SetColor( 0xffffffff) ;
	m_pWarriorButton->OnClickEvent = DlgClick ;
	m_pWarriorButton->SetInstance( RYLJOBCHANGEDLG::WARRIOR_BUTTON ) ;
	m_pWarriorButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pWarriorButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pWarriorButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pWarriorButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pWarriorButton->SetVisible( FALSE ) ;
	AttachGMUIControl( m_pWarriorButton ) ;

	// 디펜더 
	m_pDefenderButton = new CRYLButton( this ) ;
	m_pDefenderButton->SetLeft( 90 ) ;
	m_pDefenderButton->SetTop( 377 ) ;
	m_pDefenderButton->SetCaption( CRYLStringTable::m_strString[ 455 ] ) ;
	m_pDefenderButton->SetFontColor( 0xffffffff) ;
	m_pDefenderButton->SetColor( 0xffffffff) ;
	m_pDefenderButton->OnClickEvent = DlgClick ;
	m_pDefenderButton->SetInstance( RYLJOBCHANGEDLG::DEFENDER_BUTTON ) ;
	m_pDefenderButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pDefenderButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pDefenderButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pDefenderButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pDefenderButton->SetVisible( FALSE ) ;
	AttachGMUIControl( m_pDefenderButton ) ;

	// 클래릭 
	m_pClericButton = new CRYLButton( this ) ;
	m_pClericButton->SetLeft( 172 ) ;
	m_pClericButton->SetTop( 377 ) ;
	m_pClericButton->SetCaption( CRYLStringTable::m_strString[ 462 ] ) ;
	m_pClericButton->SetFontColor( 0xffffffff) ;
	m_pClericButton->SetColor( 0xffffffff) ;
	m_pClericButton->OnClickEvent = DlgClick ;
	m_pClericButton->SetInstance( RYLJOBCHANGEDLG::CLERIC_BUTTON ) ;
	m_pClericButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pClericButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pClericButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pClericButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pClericButton->SetVisible( FALSE ) ;
	AttachGMUIControl( m_pClericButton ) ;

	// 프리스트
	m_pPriestButton = new CRYLButton( this ) ;
	m_pPriestButton->SetLeft( 90 ) ;
	m_pPriestButton->SetTop( 377 ) ;
	m_pPriestButton->SetCaption( CRYLStringTable::m_strString[ 461 ] ) ;
	m_pPriestButton->SetFontColor( 0xffffffff) ;
	m_pPriestButton->SetColor( 0xffffffff) ;
	m_pPriestButton->OnClickEvent = DlgClick ;
	m_pPriestButton->SetInstance( RYLJOBCHANGEDLG::PRIEST_BUTTON ) ;
	m_pPriestButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pPriestButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pPriestButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pPriestButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pPriestButton->SetVisible( FALSE ) ;
	AttachGMUIControl( m_pPriestButton ) ;

	// 소서리
	m_pSorcererButton = new CRYLButton( this ) ;
	m_pSorcererButton->SetLeft( 172 ) ;
	m_pSorcererButton->SetTop( 377 ) ;
	m_pSorcererButton->SetCaption( CRYLStringTable::m_strString[ 459 ] ) ;
	m_pSorcererButton->SetFontColor( 0xffffffff) ;
	m_pSorcererButton->SetColor( 0xffffffff) ;
	m_pSorcererButton->OnClickEvent = DlgClick ;
	m_pSorcererButton->SetInstance( RYLJOBCHANGEDLG::SORCERER_BUTTON ) ;
	m_pSorcererButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pSorcererButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pSorcererButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pSorcererButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pSorcererButton->SetVisible( FALSE ) ;
	AttachGMUIControl( m_pSorcererButton ) ;

	// 인첸터
	m_pEnchanterButton = new CRYLButton( this ) ;
	m_pEnchanterButton->SetLeft( 90 ) ;
	m_pEnchanterButton->SetTop( 377 ) ;
	m_pEnchanterButton->SetCaption( CRYLStringTable::m_strString[ 460 ] ) ;
	m_pEnchanterButton->SetFontColor( 0xffffffff) ;
	m_pEnchanterButton->SetColor( 0xffffffff) ;
	m_pEnchanterButton->OnClickEvent = DlgClick ;
	m_pEnchanterButton->SetInstance( RYLJOBCHANGEDLG::ENCHANTER_BUTTON ) ;
	m_pEnchanterButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pEnchanterButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pEnchanterButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pEnchanterButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pEnchanterButton->SetVisible( FALSE ) ;
	AttachGMUIControl( m_pEnchanterButton ) ;

	// 아처
	m_pArcherButton = new CRYLButton( this ) ;
	m_pArcherButton->SetLeft( 172 ) ;
	m_pArcherButton->SetTop( 377 ) ;
	m_pArcherButton->SetCaption( CRYLStringTable::m_strString[ 458 ] ) ;
	m_pArcherButton->SetFontColor( 0xffffffff) ;
	m_pArcherButton->SetColor( 0xffffffff) ;
	m_pArcherButton->OnClickEvent = DlgClick ;
	m_pArcherButton->SetInstance( RYLJOBCHANGEDLG::ARCHER_BUTTON ) ;
	m_pArcherButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pArcherButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pArcherButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pArcherButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pArcherButton->SetVisible( FALSE ) ;
	AttachGMUIControl( m_pArcherButton ) ;

	// 어쎄신
	m_pAssassinButton = new CRYLButton( this ) ;
	m_pAssassinButton->SetLeft( 90 ) ;
	m_pAssassinButton->SetTop( 377 ) ;
	m_pAssassinButton->SetCaption( CRYLStringTable::m_strString[ 457 ] ) ;
	m_pAssassinButton->SetFontColor( 0xffffffff) ;
	m_pAssassinButton->SetColor( 0xffffffff) ;
	m_pAssassinButton->OnClickEvent = DlgClick ;
	m_pAssassinButton->SetInstance( RYLJOBCHANGEDLG::ASSASSIN_BUTTON ) ;
	m_pAssassinButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pAssassinButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pAssassinButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pAssassinButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pAssassinButton->SetVisible( FALSE ) ;
	AttachGMUIControl( m_pAssassinButton ) ;

	// 템플러
	m_pTemplarButton = new CRYLButton( this ) ;
	m_pTemplarButton->SetLeft( 8 ) ;
	m_pTemplarButton->SetTop( 377 ) ;
	m_pTemplarButton->SetCaption( CRYLStringTable::m_strString[ 469 ] ) ;
	m_pTemplarButton->SetFontColor( 0xffffffff) ;
	m_pTemplarButton->SetColor( 0xffffffff) ;
	m_pTemplarButton->OnClickEvent = DlgClick ;
	m_pTemplarButton->SetInstance( RYLJOBCHANGEDLG::TEMPLAR_BUTTON ) ;
	m_pTemplarButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pTemplarButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pTemplarButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pTemplarButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pTemplarButton->SetVisible( FALSE ) ;
	AttachGMUIControl( m_pTemplarButton ) ;

	// 어태커
	m_pAttackerButton = new CRYLButton( this ) ;
	m_pAttackerButton->SetLeft( 90 ) ;
	m_pAttackerButton->SetTop( 377 ) ;
	m_pAttackerButton->SetCaption( CRYLStringTable::m_strString[ 470 ] ) ;
	m_pAttackerButton->SetFontColor( 0xffffffff) ;
	m_pAttackerButton->SetColor( 0xffffffff) ;
	m_pAttackerButton->OnClickEvent = DlgClick ;
	m_pAttackerButton->SetInstance( RYLJOBCHANGEDLG::ATTACKER_BUTTON ) ;
	m_pAttackerButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pAttackerButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pAttackerButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pAttackerButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pAttackerButton->SetVisible( FALSE ) ;
	AttachGMUIControl( m_pAttackerButton ) ;

	// 거너
	m_pGunnerButton = new CRYLButton( this ) ;
	m_pGunnerButton->SetLeft( 172 ) ;
	m_pGunnerButton->SetTop( 377 ) ;
	m_pGunnerButton->SetCaption( CRYLStringTable::m_strString[ 471 ] ) ;
	m_pGunnerButton->SetFontColor( 0xffffffff) ;
	m_pGunnerButton->SetColor( 0xffffffff) ;
	m_pGunnerButton->OnClickEvent = DlgClick ;
	m_pGunnerButton->SetInstance( RYLJOBCHANGEDLG::GUNNER_BUTTON ) ;
	m_pGunnerButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pGunnerButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pGunnerButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pGunnerButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pGunnerButton->SetVisible( FALSE ) ;
	AttachGMUIControl( m_pGunnerButton ) ;

	// 룬오프
	m_pRuneOffButton = new CRYLButton( this ) ;
	m_pRuneOffButton->SetLeft( 8 ) ;
	m_pRuneOffButton->SetTop( 377 ) ;
	m_pRuneOffButton->SetCaption( CRYLStringTable::m_strString[ 472 ] ) ;
	m_pRuneOffButton->SetFontColor( 0xffffffff) ;
	m_pRuneOffButton->SetColor( 0xffffffff) ;
	m_pRuneOffButton->OnClickEvent = DlgClick ;
	m_pRuneOffButton->SetInstance( RYLJOBCHANGEDLG::RUNEOFF_BUTTON ) ;
	m_pRuneOffButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pRuneOffButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pRuneOffButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pRuneOffButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pRuneOffButton->SetVisible( FALSE ) ;
	AttachGMUIControl( m_pRuneOffButton ) ;

	// 라이프오프
	m_pLifeOffButton = new CRYLButton( this ) ;
	m_pLifeOffButton->SetLeft( 90 ) ;
	m_pLifeOffButton->SetTop( 377 ) ;
	m_pLifeOffButton->SetCaption( CRYLStringTable::m_strString[ 473 ] ) ;
	m_pLifeOffButton->SetFontColor( 0xffffffff) ;
	m_pLifeOffButton->SetColor( 0xffffffff) ;
	m_pLifeOffButton->OnClickEvent = DlgClick ;
	m_pLifeOffButton->SetInstance( RYLJOBCHANGEDLG::LIFEOFF_BUTTON ) ;
	m_pLifeOffButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pLifeOffButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pLifeOffButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pLifeOffButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pLifeOffButton->SetVisible( FALSE ) ;
	AttachGMUIControl( m_pLifeOffButton ) ;

	// 쉐도우오프
	m_pShadowOffButton = new CRYLButton( this ) ;
	m_pShadowOffButton->SetLeft( 172 ) ;
	m_pShadowOffButton->SetTop( 377 ) ;
	m_pShadowOffButton->SetCaption( CRYLStringTable::m_strString[ 474 ] ) ;
	m_pShadowOffButton->SetFontColor( 0xffffffff) ;
	m_pShadowOffButton->SetColor( 0xffffffff) ;
	m_pShadowOffButton->OnClickEvent = DlgClick ;
	m_pShadowOffButton->SetInstance( RYLJOBCHANGEDLG::SHADOWOFF_BUTTON ) ;
	m_pShadowOffButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pShadowOffButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pShadowOffButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pShadowOffButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pShadowOffButton->SetVisible( FALSE ) ;
	AttachGMUIControl( m_pShadowOffButton ) ;

	pSprite			= CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ;
	m_pJobDataListBox = new CRYLListBox( this, TRUE ) ;
	m_pJobDataListBox->SetLeft( 10 ) ;
	m_pJobDataListBox->SetTop( 46 ) ;
	m_pJobDataListBox->SetWidth( 237 ) ;
	m_pJobDataListBox->SetHeight( 322 ) ;
	
	SetRect( &rt, 198, 186, 212, 200 ) ;
	m_pJobDataListBox->AttachTopButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Normal ) ;
	SetRect( &rt, 226, 186, 240, 200 ) ;
	m_pJobDataListBox->AttachTopButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Down ) ;
	SetRect( &rt, 212, 186, 226, 200 ) ;
	m_pJobDataListBox->AttachTopButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Up ) ;
	SetRect( &rt, 198, 186, 212, 200 ) ;
	m_pJobDataListBox->AttachTopButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Disible ) ;

	SetRect( &rt, 235, 164, 243, 184 ) ;
 	m_pJobDataListBox->AttachTrackButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Normal ) ;
	SetRect( &rt, 235, 164, 243, 184 ) ;					 
	m_pJobDataListBox->AttachTrackButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Down ) ;
	SetRect( &rt, 235, 164, 243, 184 ) ;					 
	m_pJobDataListBox->AttachTrackButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Up ) ;
	SetRect( &rt, 235, 164, 243, 184 ) ;					 
	m_pJobDataListBox->AttachTrackButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Disible ) ;

	SetRect( &rt, 198, 200, 212, 214 ) ;
	m_pJobDataListBox->AttachBottomButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Normal ) ;
	SetRect( &rt, 226, 200, 240, 214 ) ;					  
	m_pJobDataListBox->AttachBottomButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Down ) ;
	SetRect( &rt, 212, 200, 226, 214 ) ;					  
	m_pJobDataListBox->AttachBottomButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Up ) ;
	SetRect( &rt, 198, 200, 212, 214 ) ;					  
	m_pJobDataListBox->AttachBottomButtonImage( pSprite, &rt, CGMUIListBox::ButtonImageStateType_Disible ) ;
 
	m_pJobDataListBox->SetColor( 0xffffffff ) ;
	AttachGMUIControl( m_pJobDataListBox ) ;

}

VOID	CRYLJobChangeDlg::FinalCtrl() 
{
	ClearText() ;
	GM_DELETE( m_pChangeJobLabel ) ;
	GM_DELETE( m_pJobDataListBox ) ;
	GM_DELETE( m_pWarriorButton ) ;	
	GM_DELETE( m_pDefenderButton ) ;
	GM_DELETE( m_pClericButton ) ;	
	GM_DELETE( m_pPriestButton ) ;	
	GM_DELETE( m_pSorcererButton ) ;
	GM_DELETE( m_pEnchanterButton ) ; 
	GM_DELETE( m_pArcherButton ) ;	
	GM_DELETE( m_pAssassinButton ) ;
	GM_DELETE( m_pTemplarButton ) ;	
	GM_DELETE( m_pAttackerButton ) ;
	GM_DELETE( m_pGunnerButton  ) ;	
	GM_DELETE( m_pRuneOffButton ) ; 	
	GM_DELETE( m_pLifeOffButton ) ;	
	GM_DELETE( m_pShadowOffButton ) ;
}

VOID	CRYLJobChangeDlg::Show( CGMUIObject* Sender ) 
{
	m_pWarriorButton->SetVisible( FALSE ) ;
	m_pDefenderButton->SetVisible( FALSE ) ;
	m_pClericButton->SetVisible( FALSE ) ;
	m_pPriestButton->SetVisible( FALSE ) ;
	m_pSorcererButton->SetVisible( FALSE ) ;
	m_pEnchanterButton->SetVisible( FALSE ) ;
	m_pArcherButton->SetVisible( FALSE ) ;
	m_pAssassinButton->SetVisible( FALSE ) ;
	m_pTemplarButton->SetVisible( FALSE ) ;
	m_pAttackerButton->SetVisible( FALSE ) ;
	m_pGunnerButton->SetVisible( FALSE ) ;
	m_pRuneOffButton->SetVisible( FALSE ) ;
	m_pLifeOffButton->SetVisible( FALSE ) ;
	m_pShadowOffButton->SetVisible( FALSE ) ;

	m_dwChangeResult = 0 ;
	m_wClassChange   = 0 ;

	switch( CRYLGameData::Instance()->m_csStatus.m_Info.Class)
	{
		case 1:
			m_pWarriorButton->SetVisible( TRUE ) ;
			m_pDefenderButton->SetVisible( TRUE ) ;
			break;

		case 2:
			m_pArcherButton->SetVisible( TRUE ) ;
			m_pAssassinButton->SetVisible( TRUE ) ;
			break;

		case 3:
			m_pSorcererButton->SetVisible( TRUE ) ;
		//	m_pEnchanterButton->SetVisible( TRUE ) ;  // blackdog class mod
			break;

		case 4:
		//	m_pClericButton->SetVisible( TRUE ) ; // blackdog class mod
			m_pPriestButton->SetVisible( TRUE ) ;
			break;

		case 17 :
			m_pTemplarButton->SetVisible( TRUE ) ;	
			m_pAttackerButton->SetVisible( TRUE ) ;	
			m_pGunnerButton->SetVisible( TRUE ) ;
			break ;
			
		case 18 :
			m_pRuneOffButton->SetVisible( TRUE ) ; 	
			m_pLifeOffButton->SetVisible( TRUE ) ; 	
			m_pShadowOffButton->SetVisible( TRUE ) ; 	
			break ;
	}
}

VOID	CRYLJobChangeDlg::Hide( CGMUIObject* Sender ) 
{
	ClearText() ;
}

VOID	CRYLJobChangeDlg::GMUIPaint( INT xx, INT yy ) 
{
	CRYLDialog::GMUIPaint( xx, yy ) ;
}

HRESULT		CRYLJobChangeDlg::Render( LPDIRECT3DDEVICE8 lpD3DDevice )  
{
	return S_OK ;
}

HRESULT		CRYLJobChangeDlg::FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) 
{
	

	return S_OK ;
}

HRESULT		CRYLJobChangeDlg::Update( BOOL &bClick, BOOL &bEdge ) 
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

	if(m_dwChangeResult & MB_YES)
	{
		g_pClientSocket->SetStatusFlag(NS_UPGRADECLASS);
		SendPacket::CharClassUpgrade(g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, m_wClassChange);

		m_dwChangeResult = 0;
		m_wClassChange = 0;
		SetVisible( FALSE ) ;
		return TRUE;
	} 
	else if(m_dwChangeResult & MB_NO || m_dwChangeResult & MB_EXIT)
	{
		m_dwChangeResult = 0;
		m_wClassChange = 0;
		return TRUE;
	}

	
	return S_OK ;
}

VOID	CRYLJobChangeDlg::JobChangeMsgBox( INT iJob )
{
	CHAR	strText[ MAX_PATH ] ;
	CRYLGameData* pGame = CRYLGameData::Instance() ;

	CRYLMessageBox *lpMessageBox = new CRYLMessageBox;
	sprintf(strText, CRYLStringTable::m_strString[185], pGame->GetClassName( iJob ) ) ;
	lpMessageBox->Create( strText, MB_YES | MB_NO | MB_EXIT ) ;
	lpMessageBox->SetResult( &m_dwChangeResult ) ;
	m_dwChangeResult = 0 ;
	m_wClassChange = iJob ;
}

VOID	CRYLJobChangeDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y ) 
{
	if ( !g_pJobChangeDlg )
		return ;

	switch( Sender->GetInstance() )
	{
	case RYLJOBCHANGEDLG::WARRIOR_BUTTON :
		g_pJobChangeDlg->JobChangeMsgBox( 6 ) ;
		break ;
	case RYLJOBCHANGEDLG::DEFENDER_BUTTON :	
		g_pJobChangeDlg->JobChangeMsgBox( 5 ) ;
		break ;
	case RYLJOBCHANGEDLG::CLERIC_BUTTON	:
		g_pJobChangeDlg->JobChangeMsgBox( 12 ) ;
		break ;
	case RYLJOBCHANGEDLG::PRIEST_BUTTON	:	
		g_pJobChangeDlg->JobChangeMsgBox( 11 ) ;
		break ;
	case RYLJOBCHANGEDLG::SORCERER_BUTTON :	
		g_pJobChangeDlg->JobChangeMsgBox( 9 ) ;
		break ;
	case RYLJOBCHANGEDLG::ENCHANTER_BUTTON :
		g_pJobChangeDlg->JobChangeMsgBox( 10 ) ;
		break ;
	case RYLJOBCHANGEDLG::ARCHER_BUTTON	:
		g_pJobChangeDlg->JobChangeMsgBox( 8 ) ;
		break ;
	case RYLJOBCHANGEDLG::ASSASSIN_BUTTON :	
		g_pJobChangeDlg->JobChangeMsgBox( 7 ) ;
		break ;
	case RYLJOBCHANGEDLG::TEMPLAR_BUTTON :	
		g_pJobChangeDlg->JobChangeMsgBox( 19 ) ;
		break ;
	case RYLJOBCHANGEDLG::ATTACKER_BUTTON :	
		g_pJobChangeDlg->JobChangeMsgBox( 20 ) ;
		break ;
	case RYLJOBCHANGEDLG::GUNNER_BUTTON	:
		g_pJobChangeDlg->JobChangeMsgBox( 21 ) ;
		break ;
	case RYLJOBCHANGEDLG::RUNEOFF_BUTTON :	
		g_pJobChangeDlg->JobChangeMsgBox( 22 ) ;
		break ;
	case RYLJOBCHANGEDLG::LIFEOFF_BUTTON :	
		g_pJobChangeDlg->JobChangeMsgBox( 23 ) ;
		break ;
	case RYLJOBCHANGEDLG::SHADOWOFF_BUTTON :
		g_pJobChangeDlg->JobChangeMsgBox( 24 ) ;
		break ;
	}	
}

VOID	CRYLJobChangeDlg::SetText( LPSTR strMessage )
{
	m_pJobDataListBox->ClearItem() ;
	ClearText() ;
	
	if( g_ClientMain.m_iLanguage == 2)
	{
		WCHAR*	strWideString = NULL;
		WCHAR	seps[] = L"\\" ;
		WCHAR*	token;
		char	strBreak[1024];
		char*	strMessageTemp = NULL ;

		char* strWBMessage = CRYLStringTable::MakeWordBreak(strMessage, 230);

		int nLen = MultiByteToWideChar( CP_ACP, 0, strWBMessage, lstrlen(strWBMessage), NULL, NULL);
		strWideString = SysAllocStringLen(NULL, nLen);
		MultiByteToWideChar( CP_ACP, 0, strWBMessage, strlen(strWBMessage), strWideString, nLen ) ;

		token = wcstok( strWideString, seps ) ;

		LPMessage lpMessage = NULL;
		while( token != NULL )
		{
			lpMessage = new Message;
			
			nLen = wcslen( token ) ;

			WideCharToMultiByte(CP_ACP, 0, token, nLen + 1, strBreak, 1024, NULL, NULL);

			strcpy(lpMessage->strMessage, strBreak);
			m_lstMessage.push_back(lpMessage);
			m_pJobDataListBox->AddItem( lpMessage->strMessage, 0xffffffff, 0xffffffff, 0xffffffff ) ;

			token = wcstok(NULL, seps);
		}

		SysFreeString(strWideString);
	}
	else
	{
		char *strMessageTemp, *token, seps[] = "\\";
		strMessageTemp = new char[strlen(strMessage) + 100];
		strcpy(strMessageTemp, CRYLStringTable::MakeWordBreak(strMessage, 220));
		token = strtok(strMessageTemp, seps);

		LPMessage lpMessage = NULL;
		while(token != NULL)
		{
			lpMessage = new Message;
			strcpy(lpMessage->strMessage, token);
			m_lstMessage.push_back(lpMessage);
			m_pJobDataListBox->AddItem( lpMessage->strMessage, 0xffffffff, 0xffffffff, 0xffffffff ) ;

			token = strtok(NULL, seps);
		}

		delete[] strMessageTemp;
	}
}

VOID	CRYLJobChangeDlg::ClearText()
{
	vector<LPMessage>::iterator it;
	LPMessage lpMessage;
	for(it = m_lstMessage.begin(); it != m_lstMessage.end();)
	{
		lpMessage = (*it);
		it = m_lstMessage.erase(it);
		delete lpMessage;
	}
	m_lstMessage.clear();
}
