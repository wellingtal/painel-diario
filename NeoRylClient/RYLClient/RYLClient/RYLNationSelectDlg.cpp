
// WORK_LIST 2.3 계정 국적 변경 UI 추가

#include <winsock2.h>

#include <Creature/CreatureStructure.h>
#include <Creature/Character/CharacterClass.h>

#include "GMUIScreen.h"
#include "GMImageDib.h"

#include "RYLButton.h"
#include "RYLLabel.h"
#include "RYLSpriteList.h"
#include "RYLImage.h"
#include "RYLImageManager.h"
#include "RYLImageCabinet.h"
#include "RYLStringTable.h"
#include "RYLMessageBox.h"

#include "RYLGameData.h"
#include "RYLRaceBase.h"

#include "RYLNationSelectDlg.h"

#include "RYLLoginScene.h"
#include "RYLIntegServerSelVaultDlg.h"
#include "RYLSelectedConfirmDlg.h"
#include "RYLSceneManager.h"
#include "GMMemory.h"



CRYLNationSelectDlg*	g_pNationSelectDlg = NULL ;
CRYLNationSelectDlg::CRYLNationSelectDlg() : CRYLDialog()
{
	int i, j;

	m_pKarterantBtn		= NULL ;
	m_pMerkadiaBtn		= NULL ;
	m_pPirateBtn		= NULL ;

	m_pChangeButton		= NULL ;	// 계정 국가 변경 버튼
	m_pCancelButton		= NULL ;	// 계정 국가 변경 취소 버튼

	m_pDlgName			= NULL ;	// 국적 변경 텍스트
	for ( i=0; i<DESCRIPTION_LINE; ++i )
	{
		m_pDescLabel[ i ] = NULL ;	// 다이얼로그 설명 텍스트
	}
	m_pKarterantLabel	= NULL ;
	m_pMerkadiaLabel	= NULL ;
	m_pPirateLabel		= NULL ;

	m_dUnifiedEnable	= false ;

	for ( i=0; i<Creature::MAX_NATION; ++i )
	{
		for ( j=0; j<DESCRIPTION_LINE; ++j )
		{
			sprintf( m_szNationDesc[ i ][ j ], "" ) ;
		}
	}

	sprintf( m_szNationDesc[ 0 ][ 0 ], CRYLStringTable::m_strString[ 3300 ] ) ;
	sprintf( m_szNationDesc[ 0 ][ 1 ], CRYLStringTable::m_strString[ 3301 ] ) ;
	sprintf( m_szNationDesc[ 0 ][ 2 ], CRYLStringTable::m_strString[ 3302 ] ) ;
	sprintf( m_szNationDesc[ 0 ][ 3 ], CRYLStringTable::m_strString[ 3303 ] ) ;
	sprintf( m_szNationDesc[ 0 ][ 4 ], CRYLStringTable::m_strString[ 3304 ] ) ;
	sprintf( m_szNationDesc[ 0 ][ 5 ], CRYLStringTable::m_strString[ 3305 ] ) ;
	sprintf( m_szNationDesc[ 0 ][ 6 ], CRYLStringTable::m_strString[ 3306 ] ) ;
	sprintf( m_szNationDesc[ 0 ][ 7 ], CRYLStringTable::m_strString[ 3307 ] ) ;
	sprintf( m_szNationDesc[ 0 ][ 8 ], CRYLStringTable::m_strString[ 3308 ] ) ;
	sprintf( m_szNationDesc[ 0 ][ 9 ], CRYLStringTable::m_strString[ 3309 ] ) ;

	sprintf( m_szNationDesc[ Creature::KARTERANT ][ 0 ], CRYLStringTable::m_strString[ 3310 ] ) ;
	sprintf( m_szNationDesc[ Creature::KARTERANT ][ 1 ], CRYLStringTable::m_strString[ 3311 ] ) ;
	sprintf( m_szNationDesc[ Creature::KARTERANT ][ 2 ], CRYLStringTable::m_strString[ 3312 ] ) ;
	sprintf( m_szNationDesc[ Creature::KARTERANT ][ 3 ], CRYLStringTable::m_strString[ 3313 ] ) ;
	sprintf( m_szNationDesc[ Creature::KARTERANT ][ 4 ], CRYLStringTable::m_strString[ 3314 ] ) ;
	sprintf( m_szNationDesc[ Creature::KARTERANT ][ 5 ], CRYLStringTable::m_strString[ 3315 ] ) ;
	sprintf( m_szNationDesc[ Creature::KARTERANT ][ 6 ], CRYLStringTable::m_strString[ 3316 ] ) ;
	sprintf( m_szNationDesc[ Creature::KARTERANT ][ 7 ], CRYLStringTable::m_strString[ 3317 ] ) ;

	sprintf( m_szNationDesc[ Creature::MERKADIA ][ 0 ], CRYLStringTable::m_strString[ 3318 ] ) ;
	sprintf( m_szNationDesc[ Creature::MERKADIA ][ 1 ], CRYLStringTable::m_strString[ 3319 ] ) ;
	sprintf( m_szNationDesc[ Creature::MERKADIA ][ 2 ], CRYLStringTable::m_strString[ 3320 ] ) ;
	sprintf( m_szNationDesc[ Creature::MERKADIA ][ 3 ], CRYLStringTable::m_strString[ 3321 ] ) ;
	sprintf( m_szNationDesc[ Creature::MERKADIA ][ 4 ], CRYLStringTable::m_strString[ 3322 ] ) ;
	sprintf( m_szNationDesc[ Creature::MERKADIA ][ 5 ], CRYLStringTable::m_strString[ 3323 ] ) ;
	sprintf( m_szNationDesc[ Creature::MERKADIA ][ 6 ], CRYLStringTable::m_strString[ 3324 ] ) ;
	sprintf( m_szNationDesc[ Creature::MERKADIA ][ 7 ], CRYLStringTable::m_strString[ 3325 ] ) ;
	   
	sprintf( m_szNationDesc[ Creature::ALMIGHTY_PIRATE ][ 0 ], CRYLStringTable::m_strString[ 3326 ] ) ;
	sprintf( m_szNationDesc[ Creature::ALMIGHTY_PIRATE ][ 1 ], CRYLStringTable::m_strString[ 3327 ] ) ;
	sprintf( m_szNationDesc[ Creature::ALMIGHTY_PIRATE ][ 2 ], CRYLStringTable::m_strString[ 3328 ] ) ;
	sprintf( m_szNationDesc[ Creature::ALMIGHTY_PIRATE ][ 3 ], CRYLStringTable::m_strString[ 3329 ] ) ;
	sprintf( m_szNationDesc[ Creature::ALMIGHTY_PIRATE ][ 4 ], CRYLStringTable::m_strString[ 3330 ] ) ;
	sprintf( m_szNationDesc[ Creature::ALMIGHTY_PIRATE ][ 5 ], CRYLStringTable::m_strString[ 3331 ] ) ;
	sprintf( m_szNationDesc[ Creature::ALMIGHTY_PIRATE ][ 6 ], CRYLStringTable::m_strString[ 3332 ] ) ;
	sprintf( m_szNationDesc[ Creature::ALMIGHTY_PIRATE ][ 7 ], CRYLStringTable::m_strString[ 3333 ] ) ;
	sprintf( m_szNationDesc[ Creature::ALMIGHTY_PIRATE ][ 8 ], CRYLStringTable::m_strString[ 3334 ] ) ;

	m_dwSelectNation	= 0 ;		// 현재 선택한 국가
	m_dwRace			= 0 ;		// 현재 종족


	// 다이얼로그 뒷면 만들기
	RECT rt ;
	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;

	// Caption Image
	CRYLSpriteList::Instance()->CreateGMImage( 256, 15, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 0, 256, 15 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;

	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	AttachCaptionTexture( ( CGMTexture*)pImage ) ;
	GM_DELETE( pImageDIB ) ;

	// Body Image
	CRYLSpriteList::Instance()->CreateGMImage( 256, 402, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	for ( i=0; i < 78 ; ++i )
	{
		SetRect( &rt, 0, 38, 256, 43 ) ;
		pImageDIB->DrawRect( &rt, 0, i * 5, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	}
	SetRect( &rt, 0, 40, 256, 52 ) ;
	pImageDIB->DrawRect( &rt, 0, i * 5, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;

	pImageDIB->MakeFrameRect(   2,  20, 252,   1, 0xffB99E6B, 0x80000000, 1 ) ;	// 한줄 금색 라인
	pImageDIB->MakeFrameRect(   6,  26, 243, 171, 0xff959595, 0x80000000, 1 ) ;	// 설명 텍스트 박스
	pImageDIB->MakeFrameRect(   8, 326,  70,  19, 0xff959595, 0x80000000, 1 ) ;	// 카르테란트 국가 이름 박스
	pImageDIB->MakeFrameRect(  92, 326,  70,  19, 0xff959595, 0x80000000, 1 ) ;	// 메르카디아 국가 이름 박스
	pImageDIB->MakeFrameRect( 175, 326,  70,  19, 0xff959595, 0x80000000, 1 ) ;	// 신의 해적단 국가 이름 박스

	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	AttachWindowTexture( ( CGMTexture*)pImage ) ;
	GM_DELETE( pImageDIB ) ;

	SetWidth( 256 ) ;
	SetHeight( 417 ) ;
	SetLeft( ( CGMUIScreen::GetInstance()->GetWidth() - GetWidth() ) / 2 ) ;
	SetTop( ( CGMUIScreen::GetInstance()->GetHeight() - GetHeight() ) / 2 ) ;
	SetFormType( RYLDLG_TYPE_CLOSEBUTTON ) ;

	InitCtrl() ;
	g_pNationSelectDlg = this ;
}

CRYLNationSelectDlg::~CRYLNationSelectDlg() 
{
	FinalCtrl() ;
}

VOID	CRYLNationSelectDlg::InitCtrl()
{
	RECT rt;

/*	CGMImageDib* pKarterantSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_NATION_KARTERANT_256x256 ) ;
	m_pKarterantBtn = new CRYLButton( this ) ;
	m_pKarterantBtn->SetLeft( 0 ) ;
	m_pKarterantBtn->SetTop( 0 ) ;
	m_pKarterantBtn->OnClickEvent = DlgClick ;
	m_pKarterantBtn->SetInstance( RYLNATIONSELECTDLG::KARTERANT_BUTTON ) ;
	SetRect( &rt, 0, 0, 100, 200 ) ;
	m_pKarterantBtn->AttachButtonImage( pKarterantSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 0, 0, 100, 200 ) ; //SetRect( &rt, 78, 4, 149, 112 ) ; hayzohar
	m_pKarterantBtn->AttachButtonImage( pKarterantSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 0, 0, 100, 200 ) ;
	m_pKarterantBtn->AttachButtonImage( pKarterantSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 0, 0, 100, 200 ) ;
	m_pKarterantBtn->AttachButtonImage( pKarterantSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pKarterantBtn ) ;
	m_pKarterantBtn->SetVisible( TRUE ) ;*/

	// 카르테란트 국적 표시 버튼
	CGMImageDib* pKarterantSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_NATION_KARTERANT_256x256 ) ;
	m_pKarterantBtn = new CRYLButton( this ) ;
	m_pKarterantBtn->SetLeft( 8 ) ;
	m_pKarterantBtn->SetTop( 223 ) ;
	m_pKarterantBtn->OnClickEvent = DlgClick ;
	m_pKarterantBtn->SetInstance( RYLNATIONSELECTDLG::KARTERANT_BUTTON ) ;
	SetRect( &rt, 3, 4, 71, 112 ) ;
	m_pKarterantBtn->AttachButtonImage( pKarterantSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 78, 4, 149, 112 ) ; //SetRect( &rt, 78, 4, 149, 112 ) ; hayzohar
	m_pKarterantBtn->AttachButtonImage( pKarterantSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 153, 4, 221, 112 ) ;
	m_pKarterantBtn->AttachButtonImage( pKarterantSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 3, 121, 71, 229 ) ;
	m_pKarterantBtn->AttachButtonImage( pKarterantSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pKarterantBtn ) ;
	m_pKarterantBtn->SetVisible( TRUE ) ;
	
	// 메르카디아 국적 표시 버튼
	CGMImageDib* pMerkadiaSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_NATION_MERKADIA_256x256 ) ;
	m_pMerkadiaBtn = new CRYLButton( this ) ;
	m_pMerkadiaBtn->SetLeft( 92 ) ;
	m_pMerkadiaBtn->SetTop( 223 ) ;
	m_pMerkadiaBtn->OnClickEvent = DlgClick ;
	m_pMerkadiaBtn->SetInstance( RYLNATIONSELECTDLG::MERKADIA_BUTTON ) ;
	SetRect( &rt, 3, 4, 71, 112 ) ;
	m_pMerkadiaBtn->AttachButtonImage( pMerkadiaSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 78, 4, 149, 112 ) ;//SetRect( &rt, 78, 4, 149, 112 ) ; hayzohar
	m_pMerkadiaBtn->AttachButtonImage( pMerkadiaSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 153, 4, 221, 112 ) ;
	m_pMerkadiaBtn->AttachButtonImage( pMerkadiaSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 3, 121, 71, 229 ) ;
	m_pMerkadiaBtn->AttachButtonImage( pMerkadiaSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pMerkadiaBtn ) ;
	m_pMerkadiaBtn->SetVisible( TRUE ) ;

	// 신의 해적단 국적 표시 버튼
	CGMImageDib* pPirateSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_NATION_PIRATE_256x256 ) ;
	m_pPirateBtn = new CRYLButton( this ) ;
	m_pPirateBtn->SetLeft( 175 ) ;
	m_pPirateBtn->SetTop( 223 ) ;
	m_pPirateBtn->OnClickEvent = DlgClick ;
	m_pPirateBtn->SetInstance( RYLNATIONSELECTDLG::PIRATE_BUTTON ) ;
	SetRect( &rt, 3, 4, 71, 112 ) ;
	m_pPirateBtn->AttachButtonImage( pPirateSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 78, 4, 149, 112 ) ;//SetRect( &rt, 78, 4, 149, 112 ) ; hayzohar
	m_pPirateBtn->AttachButtonImage( pPirateSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 153, 4, 221, 112 ) ;
	m_pPirateBtn->AttachButtonImage( pPirateSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 3, 121, 71, 229 ) ;
	m_pPirateBtn->AttachButtonImage( pPirateSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pPirateBtn ) ;
	m_pPirateBtn->SetVisible( TRUE ) ;



	CGMImageDib* pSprite			= CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;

	// 변경 버튼
	m_pChangeButton = new CRYLButton( this ) ;
	m_pChangeButton->SetLeft( 110 ) ;
	m_pChangeButton->SetTop( 378 ) ;
	m_pChangeButton->SetCaption( CRYLStringTable::m_strString[2026] ) ;
	m_pChangeButton->SetFontColor( 0xFFFFFFFF ) ;
	m_pChangeButton->SetColor( 0xFFFFFFFF ) ;
	m_pChangeButton->OnClickEvent = DlgClick ;
	m_pChangeButton->SetInstance( RYLNATIONSELECTDLG::CHANGE_BUTTON ) ;
	m_pChangeButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pChangeButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pChangeButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pChangeButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pChangeButton->SetVisible( TRUE ) ;
	AttachGMUIControl( m_pChangeButton ) ;

	// 취소 버튼
	m_pCancelButton = new CRYLButton( this ) ;
	m_pCancelButton->SetLeft( 180 ) ;
	m_pCancelButton->SetTop( 378 ) ;
	m_pCancelButton->SetCaption( CRYLStringTable::m_strString[1897] ) ;
	m_pCancelButton->SetFontColor( 0xFFFFFFFF ) ;
	m_pCancelButton->SetColor( 0xFFFFFFFF ) ;
	m_pCancelButton->OnClickEvent = DlgClick ;
	m_pCancelButton->SetInstance( RYLNATIONSELECTDLG::CANCEL_BUTTON ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pCancelButton->SetVisible( TRUE ) ;
	AttachGMUIControl( m_pCancelButton ) ;

	// 국적 변경 텍스트
	m_pDlgName = new CRYLLabel( this ) ;
	m_pDlgName->SetFontColor( 0xFFFFFFFF ) ;
	SetRect( &rt, 9, 19, 100, 30 ) ;
	m_pDlgName->SetAlignRect( rt ) ;
	m_pDlgName->SetAlign( LEFT ) ;
	m_pDlgName->SetCaption( CRYLStringTable::m_strString[3335] ) ;
	m_pDlgName->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pDlgName ) ;

	// 설명 텍스쳐
	for ( int i=0; i<DESCRIPTION_LINE; ++i )
	{
		m_pDescLabel[ i ] = new CRYLLabel( this ) ;
		m_pDescLabel[ i ]->SetLeft( 16 ) ;
		m_pDescLabel[ i ]->SetTop( 49 + i * 16 ) ;
		m_pDescLabel[ i ]->SetFontColor( 0xFFFFFFFF ) ;
		m_pDescLabel[ i ]->SetAutoSize( TRUE ) ;
		m_pDescLabel[ i ]->SetCaption( m_szNationDesc[ 0 ][ i ] ) ;
		AttachGMUIControl( m_pDescLabel[ i ] ) ;
	}

	// 국가 이름 표시
	m_pKarterantLabel = new CRYLLabel( this ) ;
	m_pKarterantLabel->SetFontColor( 0xFFFFFFFF ) ;
	SetRect( &rt, 9, 345, 77, 356 ) ;
	m_pKarterantLabel->SetAlignRect( rt ) ;
	m_pKarterantLabel->SetAlign( CENTER ) ;
	m_pKarterantLabel->SetCaption( CRYLStringTable::m_strString[2233] ) ;
	m_pKarterantLabel->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pKarterantLabel ) ;

	m_pMerkadiaLabel = new CRYLLabel( this ) ;
	m_pMerkadiaLabel->SetFontColor( 0xFFFFFFFF ) ;
	SetRect( &rt, 92, 345, 161, 356 ) ;
	m_pMerkadiaLabel->SetAlignRect( rt ) ;
	m_pMerkadiaLabel->SetAlign( CENTER ) ;
	m_pMerkadiaLabel->SetCaption( CRYLStringTable::m_strString[2264] ) ;
	m_pMerkadiaLabel->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pMerkadiaLabel ) ;

	m_pPirateLabel = new CRYLLabel( this ) ;
	m_pPirateLabel->SetFontColor( 0xFFFFFFFF ) ;
	SetRect( &rt, 175, 345, 244, 356 ) ;
	m_pPirateLabel->SetAlignRect( rt ) ;
	m_pPirateLabel->SetAlign( CENTER ) ;
	m_pPirateLabel->SetCaption( CRYLStringTable::m_strString[2268] ) ;
	m_pPirateLabel->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pPirateLabel ) ;
}

VOID	CRYLNationSelectDlg::FinalCtrl() 
{
	GM_DELETE( m_pKarterantBtn ) ;
	GM_DELETE( m_pMerkadiaBtn ) ;
	GM_DELETE( m_pPirateBtn ) ;

	GM_DELETE( m_pChangeButton ) ;			// 계정 국가 변경 버튼
	GM_DELETE( m_pCancelButton ) ;			// 계정 국가 변경 취소 버튼

	GM_DELETE( m_pDlgName ) ;				// 국적 변경 텍스트
	for ( int i=0; i<DESCRIPTION_LINE; ++i )
	{
		GM_DELETE( m_pDescLabel[ i ] ) ;	// 다이얼로그 설명 텍스트
	}
	GM_DELETE( m_pKarterantLabel ) ;		
	GM_DELETE( m_pMerkadiaLabel ) ;
	GM_DELETE( m_pPirateLabel ) ;
}

VOID	CRYLNationSelectDlg::Show( CGMUIObject* Sender ) 
{
}

VOID	CRYLNationSelectDlg::Hide( CGMUIObject* Sender ) 
{
}

VOID	CRYLNationSelectDlg::GMUIPaint( INT xx, INT yy ) 
{
	CRYLDialog::GMUIPaint( xx, yy ) ;
}

HRESULT		CRYLNationSelectDlg::Render( LPDIRECT3DDEVICE8 lpD3DDevice )  
{ 
	return S_OK ; 
} 

HRESULT		CRYLNationSelectDlg::FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) 
{ 
	return S_OK ; 
}

HRESULT		CRYLNationSelectDlg::Update( BOOL &bClick, BOOL &bEdge )  
{
	return S_OK ;
}

VOID	CRYLNationSelectDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y ) 
{
	if ( !g_pNationSelectDlg )
		return  ;

	switch( Sender->GetInstance() )
	{
	case RYLNATIONSELECTDLG::KARTERANT_BUTTON :
		g_pNationSelectDlg->ChangeNationDesc( Creature::KARTERANT ) ;
		break ;

	case RYLNATIONSELECTDLG::MERKADIA_BUTTON :
		g_pNationSelectDlg->ChangeNationDesc( Creature::MERKADIA ) ;
		break ;

	case RYLNATIONSELECTDLG::PIRATE_BUTTON :
		g_pNationSelectDlg->ChangeNationDesc( Creature::ALMIGHTY_PIRATE ) ;
		break ;

	case RYLNATIONSELECTDLG::CHANGE_BUTTON :
		{
			if ( !g_pNationSelectDlg->m_dUnifiedEnable )
			{
				CRYLGameData* pGame = CRYLGameData::Instance() ;
				CHAR szTemp[ 2048 ] = "" ;
				
				bool bHasGuild = false ;
				for ( int i = 0; i < MAX_CHAR_SLOT; ++ i )
				{
					if ( pGame->m_scLoginChar[ i ].CID )
					{
						if ( 0 == strcmp( szTemp, "" ) )
						{
							sprintf( szTemp, "%s", pGame->m_scLoginChar[ i ].Name ) ;
						}
						else
						{
							sprintf( szTemp, "%s, %s", szTemp, pGame->m_scLoginChar[ i ].Name ) ;
						}
						
						if ( 0 != pGame->m_scLoginChar[ i ].GID )
						{
							bHasGuild = true ;
						}
					}
				}

			
				CRYLMessageBox* lpMessageBox = new CRYLMessageBox ;

				if ( bHasGuild )
				{
					strcat( szTemp, CRYLStringTable::m_strString[ 3336 ] ) ;
					lpMessageBox->Create( szTemp, MB_EXIT | MB_YES | MB_NO ) ;
					lpMessageBox->SetResult( &CRYLGameData::Instance()->m_lpInterface->m_dwChangeNationResult ) ;
				}
				else
				{
					lpMessageBox->Create( CRYLStringTable::m_strString[ 3337 ], MB_EXIT | MB_YES | MB_NO ) ;
					lpMessageBox->SetResult( &CRYLGameData::Instance()->m_lpInterface->m_dwChangeNationResult ) ;
				}
			}
		}
		break ;

	case RYLNATIONSELECTDLG::CANCEL_BUTTON :
		{
			g_pNationSelectDlg->SetVisible( FALSE ) ;

			if ( g_pNationSelectDlg->m_dUnifiedEnable )
				g_pNationSelectDlg->UnifiedNextPage() ;
		}
		break ;
	}
}

void CRYLNationSelectDlg::SetUnifiedEnable( bool bEnable )			
{ 
	m_dUnifiedEnable = bEnable ; 
	m_pCancelButton->SetCaption( CRYLStringTable::m_strString[ 3338 ] ) ;
	m_pChangeButton->SetVisible( FALSE ) ;
}

void CRYLNationSelectDlg::UnifiedNextPage() 
{
	CRYLLoginScene* pScene = ( CRYLLoginScene* )(CRYLSceneManager::Instance()->FindScene( LOGIN_SCENE ) ) ;
	CRYLIntegServerSelVaultDlg* pVaultDlg = static_cast< CRYLIntegServerSelVaultDlg* >( pScene->GetIntegServerSelVaultDlg() ) ;
	CRYLSelectedConfirmDlg* pConfirmDlg = static_cast< CRYLSelectedConfirmDlg* >( pScene->GetSelectedConfirmDlg() ) ;

	if ( pVaultDlg->GetVaultEnable() )
	{
		pScene->GetSelectedConfirmDlg()->SetVisible( TRUE ) ;
		pConfirmDlg->m_pVaultNameLabel->SetCaption( CRYLStringTable::m_strString[ 3339 ] ) ;
	}
	else
		pScene->GetIntegServerSelVaultDlg()->SetVisible( TRUE ) ;

	m_dUnifiedEnable = false ;
	m_pCancelButton->SetCaption( CRYLStringTable::m_strString[1897] ) ;
	m_pChangeButton->SetVisible( TRUE ) ;
}

VOID	CRYLNationSelectDlg::ChangeNationDesc( DWORD dwNation )
{
	m_dwSelectNation = dwNation ;

	for ( int i=0; i<DESCRIPTION_LINE; ++i )
	{
		m_pDescLabel[ i ]->SetCaption( m_szNationDesc[ dwNation ][ i ] ) ;
	}

	switch ( dwNation )
	{
	case Creature::KARTERANT :
		{
			m_pKarterantLabel->SetFontColor( 0xFFFFFFFF ) ;
			m_pMerkadiaLabel->SetFontColor( 0xFFFF0000 ) ;
			m_pPirateLabel->SetFontColor( 0xFF959595 ) ;
		}
		break;

	case Creature::MERKADIA :
		{
			m_pKarterantLabel->SetFontColor( 0xFFFF0000 ) ;
			m_pMerkadiaLabel->SetFontColor( 0xFFFFFFFF ) ;
			m_pPirateLabel->SetFontColor( 0xFF959595 ) ;
		}
		break;

	case Creature::ALMIGHTY_PIRATE :
		{
			if ( CClass::RaceType::HUMAN == m_dwRace )
			{
                m_pKarterantLabel->SetFontColor( 0xFF959595 ) ;
				m_pMerkadiaLabel->SetFontColor( 0xFFFF0000 ) ;
				m_pPirateLabel->SetFontColor( 0xFFFFFFFF ) ;
			}
			else if ( CClass::RaceType::AKHAN == m_dwRace )
			{
				m_pKarterantLabel->SetFontColor( 0xFFFF0000 ) ;
				m_pMerkadiaLabel->SetFontColor( 0xFF959595 ) ;
				m_pPirateLabel->SetFontColor( 0xFFFFFFFF ) ;
			}
		}
		break;
	}

	if ( CRYLGameData::Instance()->m_cAccountNation == m_dwSelectNation )
	{
		m_pChangeButton->SetEnable( FALSE ) ;
	}
	else
	{
		m_pChangeButton->SetEnable( TRUE ) ;
	}
}

VOID	CRYLNationSelectDlg::InitValue( DWORD dwRace, DWORD dwNation )
{
	m_dwRace = dwRace ;
	m_dwSelectNation = dwNation ;

	m_pChangeButton->SetEnable( FALSE ) ;
	
	switch ( dwNation )
	{
		case Creature::KARTERANT :
		{
			m_pKarterantBtn->SetEnable( TRUE ) ;
			m_pMerkadiaBtn->SetEnable( TRUE ) ; //FALSE
			m_pPirateBtn->SetEnable( FALSE ) ; //TRUE

			m_pKarterantLabel->SetFontColor( 0xFFFFFFFF ) ;
			m_pMerkadiaLabel->SetFontColor( 0xFFFF0000 ) ;
			m_pPirateLabel->SetFontColor( 0xFF959595 ) ;
		}
		break ;

		case Creature::MERKADIA :
		{
			m_pKarterantBtn->SetEnable( TRUE ) ; //FALSE
			m_pMerkadiaBtn->SetEnable( TRUE ) ;
			m_pPirateBtn->SetEnable( FALSE ) ; //TRUE

			m_pKarterantLabel->SetFontColor( 0xFFFF0000 ) ;
			m_pMerkadiaLabel->SetFontColor( 0xFFFFFFFF ) ;
			m_pPirateLabel->SetFontColor( 0xFF959595 ) ;
		}
		break ;

		case Creature::ALMIGHTY_PIRATE :
		{
			if ( CClass::RaceType::HUMAN == dwRace )
			{
				m_pKarterantBtn->SetEnable( TRUE ) ;
				m_pMerkadiaBtn->SetEnable( FALSE ) ;
				m_pPirateBtn->SetEnable( TRUE ) ;

				m_pKarterantLabel->SetFontColor( 0xFF959595 ) ;
				m_pMerkadiaLabel->SetFontColor( 0xFFFF0000 ) ;
				m_pPirateLabel->SetFontColor( 0xFFFFFFFF ) ;
			}
			else if ( CClass::RaceType::AKHAN == dwRace )
			{
				m_pKarterantBtn->SetEnable( FALSE ) ;
				m_pMerkadiaBtn->SetEnable( TRUE ) ;
				m_pPirateBtn->SetEnable( TRUE ) ;

				m_pKarterantLabel->SetFontColor( 0xFFFF0000 ) ;
				m_pMerkadiaLabel->SetFontColor( 0xFF959595 ) ;
				m_pPirateLabel->SetFontColor( 0xFFFFFFFF ) ;
			}
		}
		break ;
	}
}
