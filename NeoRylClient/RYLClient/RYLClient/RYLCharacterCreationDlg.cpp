
#include "RYLRaceBaseDefine.h"
#include "RYLClientMain.h"
#include "RYLGameData.h"
#include "RYLNetworkData.h"
#include "RYLSceneManager.h"

#include <RenderOption.h>
#include <Item/ItemMgr.h>
#include <Creature/Character/CharacterClass.h>

#include "..\\StringFilter.h"
#include "WinInput.h"
#include "BaseDataDefine.h"
#include "FrameTimer.h"
#include "GMUIScreen.h"
#include "GMImageDib.h"

#include "RYLImageManager.h"
#include "RYLImage.h"
#include "RYLButton.h"
#include "RYLEdit.h"
#include "RYLPicture.h"
#include "RYLLabel.h"
#include "RYLMessageBoxManager.h"
#include "RYLStringTable.h"
#include "RYLSpriteList.h"

#include "RYLChattingDlg.h"
#include "RYLCharacterCreationDlg.h"

#include "RYLCharacterSelectScene.h"
#include "GMMemory.h"

//------------------------------------------------------------------------------

CRYLCharacterCreationDlg*	g_pCharacterCreationDlg = NULL ;

CRYLCharacterCreationDlg::CRYLCharacterCreationDlg( unsigned long dwRace ) : CRYLDialog()
{
	m_pCharacterCreateLabel	= NULL ;

	// 캐릭터 이름
	m_pNameLabel			= NULL ;
	m_pNameEdit				= NULL ;

	// 클레스 선택
	m_pClassLabel			= NULL ;
	m_pClassSelectLabel		= NULL ;
	m_pClassNextBtn			= NULL ;
	m_pClassPrevBtn			= NULL ;

	// 성별
	m_pSexLabel				= NULL ;
	m_pSexSelectLabel		= NULL ;
	m_pSexNextBtn			= NULL ;
	m_pSexPrevBtn			= NULL ;

	// 헤어 스타일 버튼
	m_pHairLabel			= NULL ;
	m_pHairSelectLabel		= NULL ;
	m_pHairNextBtn			= NULL ;
	m_pHairPrevBtn			= NULL ;

	// 얼굴 스타일 버튼
	m_pFaceLabel			= NULL ;
	m_pFaceSelectLabel		= NULL ;
	m_pFaceNextBtn			= NULL ;
	m_pFacePrevBtn			= NULL ;

	// 셔츠
	m_pShirtLabel			= NULL ;
	m_pShirtSelectLabel		= NULL ;
	m_pShirtNextBtn			= NULL ;
	m_pShirtPrevBtn			= NULL ;

	// 캐릭터 회전
	m_pCameraLabel			= NULL ;
	m_pCameraSelectLabel	= NULL ;
	m_pCameraRightBtn		= NULL ;
	m_pCameraLeftBtn		= NULL ;

	// 스테이터스 분배
	m_pStatusDivideLabel	= NULL ;

	// STR 포인트 증가,감소 버튼
	m_pStrLabel				= NULL ;
	m_pStrValueLabel		= NULL ;
	m_pStrPointIncBtn		= NULL ;
	m_pStrPointDecBtn		= NULL ;

	// DEX 포인트 증가,감소 버튼
	m_pDexLabel				= NULL ;
	m_pDexValueLabel		= NULL ;
	m_pDexPointIncBtn		= NULL ;
	m_pDexPointDecBtn		= NULL ;

	// CON 포인트 증가,감소 버튼
	m_pConLabel				= NULL ;
	m_pConValueLabel		= NULL ;
	m_pConPointIncBtn		= NULL ;
	m_pConPointDecBtn		= NULL ;

	// INT 포인트 증가,감소 버튼
	m_pIntLabel				= NULL ;
	m_pIntValueLabel		= NULL ;
	m_pIntPointIncBtn		= NULL ;
	m_pIntPointDecBtn		= NULL ;

	// WIS 포인트 증가,감소 버튼
	m_pWisLabel				= NULL ;
	m_pWisValueLabel		= NULL ;
	m_pWisPointIncBtn		= NULL ;
	m_pWisPointDecBtn		= NULL ;

	// 보너스 포인트
	m_pBonusPointLabel		= NULL ;
	m_pBonusPointValueLabel	= NULL ;

	// 만들기
	m_pMakeBtn				= NULL ;

	// 취소
	m_pCalcelBtn			= NULL ;

	m_pBonusPicture1		= NULL ;
	m_pBonusPicture2		= NULL ;

	m_dwRace				= dwRace ;
	m_dwClass				= 0 ;
	m_dwSex					= 0	;

	m_dwIP					= 0 ;
	m_dwSTR					= 0 ;
	m_dwDEX					= 0 ;
	m_dwCON					= 0 ;
	m_dwINT					= 0 ;
	m_dwWIS					= 0 ;
	m_dwFace				= 0 ;
	m_dwHair				= 0 ;
	m_dwShirt				= 0 ;

	m_dwEndTimer			= 0 ;

	sprintf( m_szHumanClassName[ 0 ], CRYLStringTable::m_strString[3102] ) ;
	sprintf( m_szHumanClassName[ 1 ], CRYLStringTable::m_strString[3103] ) ;
	sprintf( m_szHumanClassName[ 2 ], CRYLStringTable::m_strString[3104] ) ;
	sprintf( m_szHumanClassName[ 3 ], CRYLStringTable::m_strString[3105] ) ;

	sprintf( m_szAkhanClassName[ 0 ], CRYLStringTable::m_strString[3106] ) ;
    sprintf( m_szAkhanClassName[ 1 ], CRYLStringTable::m_strString[3107] ) ;

	sprintf( m_szSexName[ 0 ], CRYLStringTable::m_strString[3108] ) ;
	sprintf( m_szSexName[ 1 ], CRYLStringTable::m_strString[3109] ) ;

	sprintf( m_szHairName[ 0 ], CRYLStringTable::m_strString[3110] ) ;
	sprintf( m_szHairName[ 1 ], CRYLStringTable::m_strString[3111] ) ;
	sprintf( m_szHairName[ 2 ], CRYLStringTable::m_strString[3112] ) ;
	sprintf( m_szHairName[ 3 ], CRYLStringTable::m_strString[3113] ) ;
	sprintf( m_szHairName[ 4 ], CRYLStringTable::m_strString[3114] ) ;
	sprintf( m_szHairName[ 5 ], CRYLStringTable::m_strString[3115] ) ;
	sprintf( m_szHairName[ 6 ], CRYLStringTable::m_strString[3116] ) ;

	sprintf( m_szFaceName[ 0 ], CRYLStringTable::m_strString[3117] ) ;
	sprintf( m_szFaceName[ 1 ], CRYLStringTable::m_strString[3118] ) ;
	sprintf( m_szFaceName[ 2 ], CRYLStringTable::m_strString[3119] ) ;
	sprintf( m_szFaceName[ 3 ], CRYLStringTable::m_strString[3120] ) ;
	sprintf( m_szFaceName[ 4 ], CRYLStringTable::m_strString[3121] ) ;
	sprintf( m_szFaceName[ 5 ], CRYLStringTable::m_strString[3122] ) ;
	sprintf( m_szFaceName[ 6 ], CRYLStringTable::m_strString[3123] ) ;

	sprintf( m_szShirtName[ 0 ], CRYLStringTable::m_strString[3124] ) ;
	sprintf( m_szShirtName[ 1 ], CRYLStringTable::m_strString[3125] ) ;
	sprintf( m_szShirtName[ 2 ], CRYLStringTable::m_strString[3126] ) ;
	sprintf( m_szShirtName[ 3 ], CRYLStringTable::m_strString[3127] ) ;
	sprintf( m_szShirtName[ 4 ], CRYLStringTable::m_strString[3128] ) ;
	sprintf( m_szShirtName[ 5 ], CRYLStringTable::m_strString[3129] ) ;

	m_iCurrentClass			= 0 ;
	m_iCurrentSex			= 0 ;
	m_iCurrentHair			= 0 ;
	m_iCurrentFace			= 0 ;
	m_iCurrentShirt			= 0 ;


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

	CRYLSpriteList::Instance()->CreateGMImage( 256, 523, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 15, 256, 37 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;

	int i = 0;
	for (  ; i < 102 ; i ++ )
	{
		SetRect( &rt, 0, 38, 256, 43 ) ;
		pImageDIB->DrawRect( &rt, 0, (i * 5), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	}

	SetRect( &rt, 0, 42, 256, 52 ) ;
	pImageDIB->DrawRect( &rt, 0, ((i-1) * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;

	pImageDIB->DrawFrameRect( 2, 0, 252, 21, 0xc0000000 ) ;
	pImageDIB->MakeFrameRect(	2,   20, 252,   1, 0xffB99E6B, 0x80000000,  1 ) ;	// 한줄 라인
	pImageDIB->MakeFrameRect(	8,   28, 242, 215, 0xffB99E6B, 0x80000000,  1 ) ;	// 안에 있는 큰 사각박스 위에꺼
	pImageDIB->MakeFrameRect(	8,  252, 242, 219, 0xffB99E6B, 0x80000000,  1 ) ;	// 안에 있는 큰 사각박스 아래꺼
	pImageDIB->MakeFrameRect( 108,   37, 101,  19, 0xff959595, 0x80000000,  1 ) ;	// 이름 박스
	pImageDIB->MakeFrameRect( 124,	 67,  70,  19, 0xff959595, 0x80000000,  1 ) ;	// 성별 박스
	pImageDIB->MakeFrameRect( 124,	 97,  70,  19, 0xff959595, 0x80000000,  1 ) ;	// 헤어 스타일 박스
	pImageDIB->MakeFrameRect( 124,	127,  70,  19, 0xff959595, 0x80000000,  1 ) ;	// 얼굴 박스
	pImageDIB->MakeFrameRect( 124,	157,  70,  19, 0xff959595, 0x80000000,  1 ) ;	// 셔츠 박스
	pImageDIB->MakeFrameRect( 124,	187,  70,  19, 0xff959595, 0x80000000,  1 ) ;	// 클래스 박스
	pImageDIB->MakeFrameRect( 124,	217,  70,  19, 0xff959595, 0x80000000,  1 ) ;	// 캐릭터 회전 박스

	for ( int i = 0 ; i < 5 ;  i ++ )
		pImageDIB->MakeFrameRect( 77,  285 + ( i * 33 ), 37,  18, 0xff959595, 0x80000000,  1 ) ;	// 스탯 박스

	pImageDIB->MakeFrameRect( 198,  446,  37,  18, 0xff959595, 0x80000000,  1 ) ;	// 보너스 박스

	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;

	AttachWindowTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;

	SetWidth( 256 ) ;
	SetHeight( 528 ) ;
	SetLeft( 10 ) ;
	SetTop( ( CGMUIScreen::GetInstance()->GetHeight() - GetHeight() ) / 2 ) ;

	SetFormType( RYLDLG_TYPE_CLOSEBUTTON ) ;

	g_pCharacterCreationDlg = this ;

	InitCtrl() ;
	InitValue() ;
}

CRYLCharacterCreationDlg::~CRYLCharacterCreationDlg()
{
	FinalCtrl() ;
}

VOID	CRYLCharacterCreationDlg::InitCtrl()
{
	RECT rt ;
	CGMImageDib*	pSprite			= CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ;
	CRYLSpriteList*	pRYLButtonRect	= CRYLSpriteList::Instance() ;

	////////////////////////////////////////////////////////////////////////////////////////////////
	// 캐릭터생성
	m_pCharacterCreateLabel = new CRYLLabel( this ) ;
	m_pCharacterCreateLabel->SetFontColor (0xffffffff);
	SetRect( &rt, 8, 18, 121, 29 ) ;
	m_pCharacterCreateLabel->SetAlignRect( rt ) ;
	m_pCharacterCreateLabel->SetAlign( LEFT ) ;
	m_pCharacterCreateLabel->SetCaption( CRYLStringTable::m_strString[ 1903 ] ) ;
	m_pCharacterCreateLabel->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pCharacterCreateLabel ) ;
	////////////////////////////////////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////////////////////////////////////////
	// 캐릭터 이름
	m_pNameLabel = new CRYLLabel( this ) ;
	m_pNameLabel->SetFontColor (0xffffffff); 
	SetRect( &rt, 8, 56, 102, 67 ) ;
	m_pNameLabel->SetAlignRect( rt ) ;
	m_pNameLabel->SetAlign( RIGHT ) ;
	m_pNameLabel->SetCaption( CRYLStringTable::m_strString[ 1904 ] ) ;
	m_pNameLabel->SetAutoSize( TRUE ) ;
	m_pNameLabel->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 1873 ], 1873 ) ;
	AttachGMUIControl( m_pNameLabel ) ;

	// 캐릭터 이름 값
	m_pNameEdit = new CRYLEdit( this ) ;
	m_pNameEdit->SetLeft( 110 ) ;
	m_pNameEdit->SetTop( 54 ) ;
	m_pNameEdit->SetWidth( 97 ) ;
	m_pNameEdit->SetHeight( 15 ) ;
	m_pNameEdit->SetFontColor( 0xffffffff ) ;
	m_pNameEdit->SetColor( 0xff000000) ;
	m_pNameEdit->SetInstance( RYLCHATACTERCREATION::NAME_EDIT ) ;
	m_pNameEdit->SetMaxLength( 15 ) ;
	m_pNameEdit->OnKeyDown = DlgKeyDown; 
	AttachGMUIControl( m_pNameEdit ) ;
	////////////////////////////////////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////////////////////////////////////////
	// 성별 선택
	m_pSexLabel = new CRYLLabel( this ) ;
	m_pSexLabel->SetFontColor( 0xffffffff ) ;
	SetRect( &rt, 8, 86, 102, 97 ) ;
	m_pSexLabel->SetAlignRect( rt ) ;
	m_pSexLabel->SetAlign( RIGHT ) ;
	m_pSexLabel->SetCaption( CRYLStringTable::m_strString[ 1920 ] ) ;
	m_pSexLabel->SetAutoSize( TRUE ) ;
	m_pSexLabel->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 1874 ], 1874 ) ;
	AttachGMUIControl( m_pSexLabel ) ;

	// 성별 선택 값
	m_pSexSelectLabel = new CRYLLabel( this ) ;
	m_pSexSelectLabel->SetFontColor( 0xffffffff ) ;
	SetRect( &rt, 125, 86, 193, 97 ) ;
	m_pSexSelectLabel->SetAlignRect( rt ) ;
	m_pSexSelectLabel->SetAlign( CENTER ) ;
	m_pSexSelectLabel->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pSexSelectLabel ) ;

	// 성별 선택 좌측 버튼
	m_pSexPrevBtn = new CRYLButton( this ) ;
	m_pSexPrevBtn->SetLeft( 108 ) ;
	m_pSexPrevBtn->SetTop( 85 ) ;
	m_pSexPrevBtn->OnClickEvent = DlgClick ;
	m_pSexPrevBtn->SetInstance( RYLCHATACTERCREATION::SEXPREV_BUTTON ) ;
	SetRect( &rt, 217, 215, 230, 227 ) ;
	m_pSexPrevBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 217, 243, 230, 255 ) ;
	m_pSexPrevBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 217, 229, 230, 241 ) ;
	m_pSexPrevBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 217, 215, 230, 227 ) ;
	m_pSexPrevBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pSexPrevBtn ) ;

	// 성별 선택 우측 버튼
	m_pSexNextBtn = new CRYLButton( this ) ;
	m_pSexNextBtn->SetLeft( 197 ) ;
	m_pSexNextBtn->SetTop( 85 ) ;
	m_pSexNextBtn->OnClickEvent = DlgClick ;
	m_pSexNextBtn->SetInstance( RYLCHATACTERCREATION::SEXNEXT_BUTTON ) ;
	SetRect( &rt, 232, 215, 245, 227 ) ;
	m_pSexNextBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 232, 243, 245, 255 ) ;
	m_pSexNextBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 232, 229, 245, 241 ) ;
	m_pSexNextBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 232, 215, 245, 227 ) ;
	m_pSexNextBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pSexNextBtn ) ;
	////////////////////////////////////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////////////////////////////////////////
	// 헤어 스타일 선택
	m_pHairLabel = new CRYLLabel( this ) ;
	m_pHairLabel->SetFontColor( 0xffffffff ) ;
	SetRect( &rt, 8, 116, 102, 127 ) ;
	m_pHairLabel->SetAlignRect( rt ) ;
	m_pHairLabel->SetAlign( RIGHT ) ;
	m_pHairLabel->SetCaption( CRYLStringTable::m_strString[ 1905 ] ) ;
	m_pHairLabel->SetAutoSize( TRUE ) ;
	m_pHairLabel->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 1875 ], 1875 ) ;
	AttachGMUIControl( m_pHairLabel ) ;

	// 헤어 스타일 선택 값
	m_pHairSelectLabel = new CRYLLabel( this ) ;
	m_pHairSelectLabel->SetFontColor( 0xffffffff ) ;
	SetRect( &rt, 125, 116, 193, 127 ) ;
	m_pHairSelectLabel->SetAlignRect( rt ) ;
	m_pHairSelectLabel->SetAlign( CENTER ) ;
	m_pHairSelectLabel->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pHairSelectLabel ) ;

	// 헤어 스타일 선택 좌측 버튼
	m_pHairPrevBtn = new CRYLButton( this ) ;
	m_pHairPrevBtn->SetLeft( 108 ) ;
	m_pHairPrevBtn->SetTop( 115 ) ;
	m_pHairPrevBtn->OnClickEvent = DlgClick ;
	m_pHairPrevBtn->SetInstance( RYLCHATACTERCREATION::HAIRPREV_BUTTON ) ;
	SetRect( &rt, 217, 215, 230, 227 ) ;
	m_pHairPrevBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 217, 243, 230, 255 ) ;
	m_pHairPrevBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 217, 229, 230, 241 ) ;
	m_pHairPrevBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 217, 215, 230, 227 ) ;
	m_pHairPrevBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pHairPrevBtn ) ;

	// 헤어 스타일 선택 우측 버튼
	m_pHairNextBtn = new CRYLButton( this ) ;
	m_pHairNextBtn->SetLeft( 197 ) ;
	m_pHairNextBtn->SetTop( 115 ) ;
	m_pHairNextBtn->OnClickEvent = DlgClick ;
	m_pHairNextBtn->SetInstance( RYLCHATACTERCREATION::HAIRNEXT_BUTTON ) ;
	SetRect( &rt, 232, 215, 245, 227 ) ;
	m_pHairNextBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 232, 243, 245, 255 ) ;
	m_pHairNextBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 232, 229, 245, 241 ) ;
	m_pHairNextBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 232, 215, 245, 227 ) ;
	m_pHairNextBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pHairNextBtn ) ;
	////////////////////////////////////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////////////////////////////////////////
	// 얼굴 스타일 선택
	m_pFaceLabel = new CRYLLabel( this ) ;
	m_pFaceLabel->SetFontColor( 0xffffffff ) ;
	SetRect( &rt, 8, 146, 102, 157 ) ;
	m_pFaceLabel->SetAlignRect( rt ) ;
	m_pFaceLabel->SetAlign( RIGHT ) ;
	m_pFaceLabel->SetCaption( CRYLStringTable::m_strString[ 1906 ] ) ;
	m_pFaceLabel->SetAutoSize( TRUE ) ;
	m_pFaceLabel->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 1876 ], 1876 ) ;
	AttachGMUIControl( m_pFaceLabel ) ;

	// 얼굴 스타일 선택 값
	m_pFaceSelectLabel = new CRYLLabel( this ) ;
	m_pFaceSelectLabel->SetFontColor( 0xffffffff ) ;
	SetRect( &rt, 125, 146, 193, 157 ) ;
	m_pFaceSelectLabel->SetAlignRect( rt ) ;
	m_pFaceSelectLabel->SetAlign( CENTER ) ;
	m_pFaceSelectLabel->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pFaceSelectLabel ) ;

	// 얼굴 스타일 선택 좌측 버튼
	m_pFacePrevBtn = new CRYLButton( this ) ;
	m_pFacePrevBtn->SetLeft( 108 ) ;
	m_pFacePrevBtn->SetTop( 145 ) ;
	m_pFacePrevBtn->OnClickEvent = DlgClick ;
	m_pFacePrevBtn->SetInstance( RYLCHATACTERCREATION::FACEPREV_BUTTON ) ;
	SetRect( &rt, 217, 215, 230, 227 ) ;
	m_pFacePrevBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 217, 243, 230, 255 ) ;
	m_pFacePrevBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 217, 229, 230, 241 ) ;
	m_pFacePrevBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 217, 215, 230, 227 ) ;
	m_pFacePrevBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pFacePrevBtn ) ;

	// 얼굴 스타일 선택 우측 버튼
	m_pFaceNextBtn = new CRYLButton( this ) ;
	m_pFaceNextBtn->SetLeft( 197 ) ;
	m_pFaceNextBtn->SetTop( 145 ) ;
	m_pFaceNextBtn->OnClickEvent = DlgClick ;
	m_pFaceNextBtn->SetInstance( RYLCHATACTERCREATION::FACENEXT_BUTTON ) ;
	SetRect( &rt, 232, 215, 245, 227 ) ;
	m_pFaceNextBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 232, 243, 245, 255 ) ;
	m_pFaceNextBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 232, 229, 245, 241 ) ;
	m_pFaceNextBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 232, 215, 245, 227 ) ;
	m_pFaceNextBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pFaceNextBtn ) ;
	////////////////////////////////////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////////////////////////////////////////
	// 셔츠 선택
	m_pShirtLabel = new CRYLLabel( this ) ;
	m_pShirtLabel->SetFontColor( 0xffffffff ) ;
	SetRect( &rt, 8, 176, 102, 187 ) ;
	m_pShirtLabel->SetAlignRect( rt ) ;
	m_pShirtLabel->SetAlign( RIGHT ) ;
	m_pShirtLabel->SetCaption( CRYLStringTable::m_strString[ 1921 ] ) ;
	m_pShirtLabel->SetAutoSize( TRUE ) ;
	m_pShirtLabel->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 1877 ], 1877 ) ;
	AttachGMUIControl( m_pShirtLabel ) ;

	// 셔츠 선택 값
	m_pShirtSelectLabel = new CRYLLabel( this ) ;
	m_pShirtSelectLabel->SetFontColor( 0xffffffff ) ;
	SetRect( &rt, 125, 176, 193, 187 ) ;
	m_pShirtSelectLabel->SetAlignRect( rt ) ;
	m_pShirtSelectLabel->SetAlign( CENTER ) ;
	m_pShirtSelectLabel->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pShirtSelectLabel ) ;

	// 셔츠 선택 좌측 버튼
	m_pShirtPrevBtn = new CRYLButton( this ) ;
	m_pShirtPrevBtn->SetLeft( 108 ) ;
	m_pShirtPrevBtn->SetTop( 175 ) ;
	m_pShirtPrevBtn->OnClickEvent = DlgClick ;
	m_pShirtPrevBtn->SetInstance( RYLCHATACTERCREATION::SHIRTPREV_BUTTON ) ;
	SetRect( &rt, 217, 215, 230, 227 ) ;
	m_pShirtPrevBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 217, 243, 230, 255 ) ;
	m_pShirtPrevBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 217, 229, 230, 241 ) ;
	m_pShirtPrevBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 217, 215, 230, 227 ) ;
	m_pShirtPrevBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pShirtPrevBtn ) ;

	// 셔츠 선택 우측 버튼
	m_pShirtNextBtn = new CRYLButton( this ) ;
	m_pShirtNextBtn->SetLeft( 197 ) ;
	m_pShirtNextBtn->SetTop( 175 ) ;
	m_pShirtNextBtn->OnClickEvent = DlgClick ;
	m_pShirtNextBtn->SetInstance( RYLCHATACTERCREATION::SHIRTNEXT_BUTTON ) ;
	SetRect( &rt, 232, 215, 245, 227 ) ;
	m_pShirtNextBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 232, 243, 245, 255 ) ;
	m_pShirtNextBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 232, 229, 245, 241 ) ;
	m_pShirtNextBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 232, 215, 245, 227 ) ;
	m_pShirtNextBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pShirtNextBtn ) ;
	////////////////////////////////////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////////////////////////////////////////
	// 클래스 선택
	m_pClassLabel = new CRYLLabel( this ) ;
	m_pClassLabel->SetFontColor( 0xffffffff ) ;
	SetRect( &rt, 8, 206, 102, 217 ) ;
	m_pClassLabel->SetAlignRect( rt ) ;
	m_pClassLabel->SetAlign( RIGHT ) ;
	m_pClassLabel->SetCaption( CRYLStringTable::m_strString[ 1907 ] ) ;
	m_pClassLabel->SetAutoSize( TRUE ) ;
	m_pClassLabel->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 1878 ], 1878 ) ;
	AttachGMUIControl( m_pClassLabel ) ;

	// 클래스 선택 값
	m_pClassSelectLabel = new CRYLLabel( this ) ;
	m_pClassSelectLabel->SetFontColor( 0xffffffff ) ;
	SetRect( &rt, 125, 206, 193, 217 ) ;
	m_pClassSelectLabel->SetAlignRect( rt ) ;
	m_pClassSelectLabel->SetAlign( CENTER ) ;
	m_pClassSelectLabel->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pClassSelectLabel ) ;

	// 클래스 선택 좌측 버튼
	m_pClassPrevBtn = new CRYLButton( this ) ;
	m_pClassPrevBtn->SetLeft( 108 ) ;
	m_pClassPrevBtn->SetTop( 205 ) ;
	m_pClassPrevBtn->OnClickEvent = DlgClick ;
	m_pClassPrevBtn->SetInstance( RYLCHATACTERCREATION::CLASSPREV_BUTTON ) ;
	SetRect( &rt, 217, 215, 230, 227 ) ;
	m_pClassPrevBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 217, 243, 230, 255 ) ;
	m_pClassPrevBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 217, 229, 230, 241 ) ;
	m_pClassPrevBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 217, 215, 230, 227 ) ;
	m_pClassPrevBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pClassPrevBtn ) ;

	// 클래스 선택 우측 버튼
	m_pClassNextBtn = new CRYLButton( this ) ;
	m_pClassNextBtn->SetLeft( 197 ) ;
	m_pClassNextBtn->SetTop( 205 ) ;
	m_pClassNextBtn->OnClickEvent = DlgClick ;
	m_pClassNextBtn->SetInstance( RYLCHATACTERCREATION::CLASSNEXT_BUTTON ) ;
	SetRect( &rt, 232, 215, 245, 227 ) ;
	m_pClassNextBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 232, 243, 245, 255 ) ;
	m_pClassNextBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 232, 229, 245, 241 ) ;
	m_pClassNextBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 232, 215, 245, 227 ) ;
	m_pClassNextBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pClassNextBtn ) ;
	////////////////////////////////////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////////////////////////////////////////
	// 캐릭터 회전
	m_pCameraLabel = new CRYLLabel( this ) ;
	m_pCameraLabel->SetFontColor( 0xffffffff ) ;
	SetRect( &rt, 8, 236, 102, 247 ) ;
	m_pCameraLabel->SetAlignRect( rt ) ;
	m_pCameraLabel->SetAlign( RIGHT ) ;
	m_pCameraLabel->SetCaption( CRYLStringTable::m_strString[ 3131 ] ) ;
	m_pCameraLabel->SetAutoSize( TRUE ) ;
	m_pCameraLabel->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 1879 ], 1879 ) ;
	AttachGMUIControl( m_pCameraLabel ) ;

	// 캐릭터 회전 선택 값
	m_pCameraSelectLabel = new CRYLLabel( this ) ;
	m_pCameraSelectLabel->SetFontColor( 0xffffffff ) ;
	SetRect( &rt, 125, 236, 193, 247 ) ;
	m_pCameraSelectLabel->SetAlignRect( rt ) ;
	m_pCameraSelectLabel->SetAlign( CENTER ) ;
	m_pCameraSelectLabel->SetCaption( CRYLStringTable::m_strString[ 3132 ] ) ;
	m_pCameraSelectLabel->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pCameraSelectLabel ) ;

	// 캐릭터 회전 좌측 버튼
	m_pCameraLeftBtn = new CRYLButton( this ) ;
	m_pCameraLeftBtn->SetLeft( 108 ) ;
	m_pCameraLeftBtn->SetTop( 235 ) ;
	m_pCameraLeftBtn->OnClickEvent = DlgClick ;
	m_pCameraLeftBtn->SetInstance( RYLCHATACTERCREATION::CAMERALEFT_BUTTON ) ;
	SetRect( &rt, 217, 215, 230, 227 ) ;
	m_pCameraLeftBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 217, 243, 230, 255 ) ;
	m_pCameraLeftBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 217, 229, 230, 241 ) ;
	m_pCameraLeftBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 217, 215, 230, 227 ) ;
	m_pCameraLeftBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pCameraLeftBtn ) ;

	// 캐릭터 회전 우측 버튼
	m_pCameraRightBtn = new CRYLButton( this ) ;
	m_pCameraRightBtn->SetLeft( 197 ) ;
	m_pCameraRightBtn->SetTop( 235 ) ;
	m_pCameraRightBtn->OnClickEvent = DlgClick ;
	m_pCameraRightBtn->SetInstance( RYLCHATACTERCREATION::CAMERARIGHT_BUTTON ) ;
	SetRect( &rt, 232, 215, 245, 227 ) ;
	m_pCameraRightBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 232, 243, 245, 255 ) ;
	m_pCameraRightBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 232, 229, 245, 241 ) ;
	m_pCameraRightBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 232, 215, 245, 227 ) ;
	m_pCameraRightBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pCameraRightBtn ) ;
	////////////////////////////////////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////////////////////////////////////////
	// 스테이터스 분배
	m_pStatusDivideLabel = new CRYLLabel( this ) ;
	m_pStatusDivideLabel->SetFontColor (0xffffffff);
	SetRect( &rt, 8, 277, 121, 278 ) ;
	m_pStatusDivideLabel->SetAlignRect( rt ) ;
	m_pStatusDivideLabel->SetAlign( CENTER ) ;
	m_pStatusDivideLabel->SetCaption( CRYLStringTable::m_strString[ 1908 ] ) ;
	m_pStatusDivideLabel->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pStatusDivideLabel ) ;
	////////////////////////////////////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////////////////////////////////////////
	// STR 포인트 증가,감소 버튼
	m_pStrLabel = new CRYLLabel( this ) ;
	m_pStrLabel->SetFontColor (0xffffffff);
	SetRect( &rt, 8, 304, 67, 315 ) ;
	m_pStrLabel->SetAlignRect( rt ) ;
	m_pStrLabel->SetAlign( RIGHT ) ;
	m_pStrLabel->SetCaption( "STR" ) ;
	m_pStrLabel->SetAutoSize( TRUE ) ;
	m_pStrLabel->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 1880 ], 1880 ) ;
	AttachGMUIControl( m_pStrLabel ) ;

	m_pStrValueLabel = new CRYLLabel( this ) ;
	m_pStrValueLabel->SetFontColor (0xffffffff);
	SetRect( &rt, 78, 304, 113, 315 ) ;
	m_pStrValueLabel->SetAlignRect( rt ) ;
	m_pStrValueLabel->SetAlign( CENTER ) ;
	m_pStrValueLabel->SetCaption( "20" ) ;
	m_pStrValueLabel->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pStrValueLabel ) ;

	m_pStrPointIncBtn = new CRYLButton( this ) ;
	m_pStrPointIncBtn->SetLeft( 128 ) ;
	m_pStrPointIncBtn->SetTop( 296 ) ;
	m_pStrPointIncBtn->OnClickEvent = DlgClick ;
	m_pStrPointIncBtn->SetInstance( RYLCHATACTERCREATION::STRPOINTINC_BUTTON ) ;
	SetRect( &rt, 26, 78, 53, 103 ) ;
	m_pStrPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 26, 128, 53, 153) ;
	m_pStrPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 26, 103, 53, 128) ;
	m_pStrPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 26, 53, 53, 78) ;
	m_pStrPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pStrPointIncBtn ) ;

	m_pStrPointDecBtn = new CRYLButton( this ) ;
	m_pStrPointDecBtn->SetLeft( 162 ) ;
	m_pStrPointDecBtn->SetTop( 296 ) ;
	m_pStrPointDecBtn->OnClickEvent = DlgClick ;
	m_pStrPointDecBtn->SetInstance( RYLCHATACTERCREATION::STRPOINTDEC_BUTTON ) ;
	SetRect( &rt, 53, 78, 80, 103 ) ;
	m_pStrPointDecBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 53, 128, 80, 153) ;
	m_pStrPointDecBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 53, 103, 80, 128) ;
	m_pStrPointDecBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 53, 53, 80, 78) ;
	m_pStrPointDecBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pStrPointDecBtn ) ;
	////////////////////////////////////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////////////////////////////////////////
	// DEX 포인트 증가,감소 버튼
	m_pDexLabel = new CRYLLabel( this ) ;
	m_pDexLabel->SetFontColor (0xffffffff);
	SetRect( &rt, 8, 337, 67, 348 ) ;
	m_pDexLabel->SetAlignRect( rt ) ;
	m_pDexLabel->SetAlign( RIGHT ) ;
	m_pDexLabel->SetCaption( "DEX" ) ;
	m_pDexLabel->SetAutoSize( TRUE ) ;
	m_pDexLabel->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 1881 ], 1881 ) ;
	AttachGMUIControl( m_pDexLabel ) ;

	m_pDexValueLabel = new CRYLLabel( this ) ;
	m_pDexValueLabel->SetFontColor (0xffffffff);
	SetRect( &rt, 78, 337, 113, 348 ) ;
	m_pDexValueLabel->SetAlignRect( rt ) ;
	m_pDexValueLabel->SetAlign( CENTER ) ;
	m_pDexValueLabel->SetCaption( "20" ) ;
	m_pDexValueLabel->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pDexValueLabel ) ;

	m_pDexPointIncBtn = new CRYLButton( this ) ;
	m_pDexPointIncBtn->SetLeft( 128 ) ;
	m_pDexPointIncBtn->SetTop( 329 ) ;
	m_pDexPointIncBtn->OnClickEvent = DlgClick ;
	m_pDexPointIncBtn->SetInstance( RYLCHATACTERCREATION::DEXPOINTINC_BUTTON ) ;
	SetRect( &rt, 26, 78, 53, 103 ) ;
	m_pDexPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 26, 128, 53, 153) ;
	m_pDexPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 26, 103, 53, 128) ;
	m_pDexPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 26, 53, 53, 78) ;
	m_pDexPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pDexPointIncBtn ) ;

	m_pDexPointDecBtn = new CRYLButton( this ) ;
	m_pDexPointDecBtn->SetLeft( 162 ) ;
	m_pDexPointDecBtn->SetTop( 329 ) ;
	m_pDexPointDecBtn->OnClickEvent = DlgClick ;
	m_pDexPointDecBtn->SetInstance( RYLCHATACTERCREATION::DEXPOINTDEC_BUTTON ) ;
	SetRect( &rt, 53, 78, 80, 103 ) ;
	m_pDexPointDecBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 53, 128, 80, 153) ;
	m_pDexPointDecBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 53, 103, 80, 128) ;
	m_pDexPointDecBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 53, 53, 80, 78) ;
	m_pDexPointDecBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pDexPointDecBtn ) ;
	////////////////////////////////////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////////////////////////////////////////
	// CON 포인트 증가,감소 버튼
	m_pConLabel = new CRYLLabel( this ) ;
	m_pConLabel->SetFontColor (0xffffffff);
	SetRect( &rt, 8, 370, 67, 381 ) ;
	m_pConLabel->SetAlignRect( rt ) ;
	m_pConLabel->SetAlign( RIGHT ) ;
	m_pConLabel->SetCaption( "CON" ) ;
	m_pConLabel->SetAutoSize( TRUE ) ;
	m_pConLabel->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 1882 ], 1882 ) ;
	AttachGMUIControl( m_pConLabel ) ;

	m_pConValueLabel = new CRYLLabel( this ) ;
	m_pConValueLabel->SetFontColor (0xffffffff);
	SetRect( &rt, 78, 370, 113, 381 ) ;
	m_pConValueLabel->SetAlignRect( rt ) ;
	m_pConValueLabel->SetAlign( CENTER ) ;
	m_pConValueLabel->SetCaption( "20" ) ;
	m_pConValueLabel->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pConValueLabel ) ;

	m_pConPointIncBtn = new CRYLButton( this ) ;
	m_pConPointIncBtn->SetLeft( 128 ) ;
	m_pConPointIncBtn->SetTop( 362 ) ;
	m_pConPointIncBtn->OnClickEvent = DlgClick ;
	m_pConPointIncBtn->SetInstance( RYLCHATACTERCREATION::CONPOINTINC_BUTTON ) ;
	SetRect( &rt, 26, 78, 53, 103 ) ;
	m_pConPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 26, 128, 53, 153) ;
	m_pConPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 26, 103, 53, 128) ;
	m_pConPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 26, 53, 53, 78) ;
	m_pConPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pConPointIncBtn ) ;

	m_pConPointDecBtn = new CRYLButton( this ) ;
	m_pConPointDecBtn->SetLeft( 162 ) ;
	m_pConPointDecBtn->SetTop( 362 ) ;
	m_pConPointDecBtn->OnClickEvent = DlgClick ;
	m_pConPointDecBtn->SetInstance( RYLCHATACTERCREATION::CONPOINTDEC_BUTTON ) ;
	SetRect( &rt, 53, 78, 80, 103 ) ;
	m_pConPointDecBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 53, 128, 80, 153) ;
	m_pConPointDecBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 53, 103, 80, 128) ;
	m_pConPointDecBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 53, 53, 80, 78) ;
	m_pConPointDecBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pConPointDecBtn ) ;
	////////////////////////////////////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////////////////////////////////////////
	// INT 포인트 증가,감소 버튼
	m_pIntLabel = new CRYLLabel( this ) ;
	m_pIntLabel->SetFontColor (0xffffffff);
	SetRect( &rt, 8, 403, 67, 414 ) ;
	m_pIntLabel->SetAlignRect( rt ) ;
	m_pIntLabel->SetAlign( RIGHT ) ;
	m_pIntLabel->SetCaption( "INT" ) ;
	m_pIntLabel->SetAutoSize( TRUE ) ;
	m_pIntLabel->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 1883 ], 1883 ) ;
	AttachGMUIControl( m_pIntLabel ) ;

	m_pIntValueLabel = new CRYLLabel( this ) ;
	m_pIntValueLabel->SetFontColor (0xffffffff);
	SetRect( &rt, 78, 403, 113, 414 ) ;
	m_pIntValueLabel->SetAlignRect( rt ) ;
	m_pIntValueLabel->SetAlign( CENTER ) ;
	m_pIntValueLabel->SetCaption( "20" ) ;
	m_pIntValueLabel->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pIntValueLabel ) ;

	m_pIntPointIncBtn = new CRYLButton( this ) ;
	m_pIntPointIncBtn->SetLeft( 128 ) ;
	m_pIntPointIncBtn->SetTop( 395 ) ;
	m_pIntPointIncBtn->OnClickEvent = DlgClick ;
	m_pIntPointIncBtn->SetInstance( RYLCHATACTERCREATION::INTPOINTINC_BUTTON ) ;
	SetRect( &rt, 26, 78, 53, 103 ) ;
	m_pIntPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 26, 128, 53, 153) ;
	m_pIntPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 26, 103, 53, 128) ;
	m_pIntPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 26, 53, 53, 78) ;
	m_pIntPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pIntPointIncBtn ) ;

	m_pIntPointDecBtn = new CRYLButton( this ) ;
	m_pIntPointDecBtn->SetLeft( 162 ) ;
	m_pIntPointDecBtn->SetTop( 395 ) ;
	m_pIntPointDecBtn->OnClickEvent = DlgClick ;
	m_pIntPointDecBtn->SetInstance( RYLCHATACTERCREATION::INTPOINTDEC_BUTTON ) ;
	SetRect( &rt, 53, 78, 80, 103 ) ;
	m_pIntPointDecBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 53, 128, 80, 153) ;
	m_pIntPointDecBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 53, 103, 80, 128) ;
	m_pIntPointDecBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 53, 53, 80, 78) ;
	m_pIntPointDecBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pIntPointDecBtn ) ;
	////////////////////////////////////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////////////////////////////////////////
	// WIS 포인트 증가,감소 버튼
	m_pWisLabel = new CRYLLabel( this ) ;
	m_pWisLabel->SetFontColor (0xffffffff);
	SetRect( &rt, 8, 436, 67, 447 ) ;
	m_pWisLabel->SetAlignRect( rt ) ;
	m_pWisLabel->SetAlign( RIGHT ) ;
	m_pWisLabel->SetCaption( "WIS" ) ;
	m_pWisLabel->SetAutoSize( TRUE ) ;
	m_pWisLabel->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 1884 ], 1884 ) ;
	AttachGMUIControl( m_pWisLabel ) ;

	m_pWisValueLabel = new CRYLLabel( this ) ;
	m_pWisValueLabel->SetFontColor (0xffffffff);
	SetRect( &rt, 78, 436, 113, 447 ) ;
	m_pWisValueLabel->SetAlignRect( rt ) ;
	m_pWisValueLabel->SetAlign( CENTER ) ;
	m_pWisValueLabel->SetCaption( "20" ) ;
	m_pWisValueLabel->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pWisValueLabel ) ;

	m_pWisPointIncBtn = new CRYLButton( this ) ;
	m_pWisPointIncBtn->SetLeft( 128 ) ;
	m_pWisPointIncBtn->SetTop( 428 ) ;
	m_pWisPointIncBtn->OnClickEvent = DlgClick ;
	m_pWisPointIncBtn->SetInstance( RYLCHATACTERCREATION::WISPOINTINC_BUTTON ) ;
	SetRect( &rt, 26, 78, 53, 103 ) ;
	m_pWisPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 26, 128, 53, 153) ;
	m_pWisPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 26, 103, 53, 128) ;
	m_pWisPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 26, 53, 53, 78) ;
	m_pWisPointIncBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pWisPointIncBtn ) ;

	m_pWisPointDecBtn = new CRYLButton( this ) ;
	m_pWisPointDecBtn->SetLeft( 162 ) ;
	m_pWisPointDecBtn->SetTop( 428 ) ;
	m_pWisPointDecBtn->OnClickEvent = DlgClick ;
	m_pWisPointDecBtn->SetInstance( RYLCHATACTERCREATION::WISPOINTDEC_BUTTON ) ;
	SetRect( &rt, 53, 78, 80, 103 ) ;
	m_pWisPointDecBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 53, 128, 80, 153) ;
	m_pWisPointDecBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 53, 103, 80, 128) ;
	m_pWisPointDecBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 53, 53, 80, 78) ;
	m_pWisPointDecBtn->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pWisPointDecBtn ) ;
	////////////////////////////////////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////////////////////////////////////////
	// 보너스 포인트
	m_pBonusPointLabel = new CRYLLabel( this ) ;
	m_pBonusPointLabel->SetFontColor (0xffffffff);
	SetRect( &rt, 8, 465, 193, 476 ) ;
	m_pBonusPointLabel->SetAlignRect( rt ) ;
	m_pBonusPointLabel->SetAlign( RIGHT ) ;
	m_pBonusPointLabel->SetCaption( CRYLStringTable::m_strString[ 1910 ] ) ;
	m_pBonusPointLabel->SetAutoSize( TRUE ) ;
	m_pBonusPointLabel->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 1885 ], 1885 ) ;
	AttachGMUIControl( m_pBonusPointLabel ) ;

	m_pBonusPointValueLabel = new CRYLLabel( this ) ;
	m_pBonusPointValueLabel->SetFontColor (0xffffffff);
	SetRect( &rt, 199, 465, 234, 476 ) ;
	m_pBonusPointValueLabel->SetAlignRect( rt ) ;
	m_pBonusPointValueLabel->SetAlign( CENTER ) ;
	m_pBonusPointValueLabel->SetCaption( "5" ) ;
	m_pBonusPointValueLabel->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pBonusPointValueLabel ) ;
	////////////////////////////////////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////////////////////////////////////////
	// 생성 / 취소 버튼
	pSprite	= CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;

	m_pMakeBtn = new CRYLButton( this ) ;
	m_pMakeBtn->SetLeft( 111 ) ;
	m_pMakeBtn->SetTop( 494 ) ;
	m_pMakeBtn->SetCaption( CRYLStringTable::m_strString[ 1909 ] ) ;
	m_pMakeBtn->SetFontColor( 0xffffffff) ;
	m_pMakeBtn->SetColor( 0xffffffff) ;
	m_pMakeBtn->OnClickEvent = DlgClick ;
	m_pMakeBtn->SetInstance( RYLCHATACTERCREATION::MAKE_BUTTON ) ;
	m_pMakeBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pMakeBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pMakeBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pMakeBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pMakeBtn->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 1886 ], 1886 ) ;
	AttachGMUIControl( m_pMakeBtn ) ;

	m_pCalcelBtn = new CRYLButton( this ) ;
	m_pCalcelBtn->SetLeft( 184 ) ;
	m_pCalcelBtn->SetTop( 494 ) ;
	m_pCalcelBtn->SetCaption( CRYLStringTable::m_strString[ 1897 ] ) ;
	m_pCalcelBtn->SetFontColor( 0xffffffff) ;
	m_pCalcelBtn->SetColor( 0xffffffff) ;
	m_pCalcelBtn->OnClickEvent = DlgClick ;
	m_pCalcelBtn->SetInstance( RYLCHATACTERCREATION::CALCEL_BUTTON ) ;
	m_pCalcelBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pCalcelBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pCalcelBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pCalcelBtn->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pCalcelBtn->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 1887 ], 1887 ) ;
	AttachGMUIControl( m_pCalcelBtn ) ;
	////////////////////////////////////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////////////////////////////////////////
	// 1 + / levelup 이미지
	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;
	
	m_pBonusPicture1 = new CRYLPicture( this ) ;
	m_pBonusPicture1->SetLeft( 196 ) ;
	m_pBonusPicture1->SetTop( 297 ) ;

	m_pBonusPicture2 = new CRYLPicture( this ) ;
	m_pBonusPicture2->SetLeft( 196 ) ;
	m_pBonusPicture2->SetTop( 330 ) ;
	
	// Bounus Dex Image
	CRYLSpriteList::Instance()->CreateGMImage( 27, 25, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 129, 72, 156, 97 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	m_pBonusPicture1->AttachImageTexture( (CGMTexture* )pImage ) ;
	GM_DELETE( pImageDIB ) ;
	AttachGMUIControl( m_pBonusPicture1 ) ;

	// Bounus Con Image
	CRYLSpriteList::Instance()->CreateGMImage( 27, 25, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 129, 72, 156, 97 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	m_pBonusPicture2->AttachImageTexture( (CGMTexture* )pImage ) ;
	GM_DELETE( pImageDIB ) ;
	AttachGMUIControl( m_pBonusPicture2 ) ;
}

VOID	CRYLCharacterCreationDlg::FinalCtrl()
{
	GM_DELETE( m_pCharacterCreateLabel ) ;

	GM_DELETE( m_pNameLabel ) ;
	GM_DELETE( m_pNameEdit ) ;

	GM_DELETE( m_pClassLabel ) ;
	GM_DELETE( m_pClassSelectLabel ) ;
	GM_DELETE( m_pClassNextBtn ) ;
	GM_DELETE( m_pClassPrevBtn ) ;

	GM_DELETE( m_pSexLabel ) ;
	GM_DELETE( m_pSexSelectLabel ) ;
	GM_DELETE( m_pSexNextBtn ) ;
	GM_DELETE( m_pSexPrevBtn ) ;

	GM_DELETE( m_pHairLabel ) ;
	GM_DELETE( m_pHairSelectLabel ) ;
	GM_DELETE( m_pHairNextBtn ) ;
	GM_DELETE( m_pHairPrevBtn ) ;

	GM_DELETE( m_pFaceLabel ) ;
	GM_DELETE( m_pFaceSelectLabel ) ;
	GM_DELETE( m_pFaceNextBtn ) ;
	GM_DELETE( m_pFacePrevBtn ) ;

	GM_DELETE( m_pShirtLabel ) ;
	GM_DELETE( m_pShirtSelectLabel ) ;
	GM_DELETE( m_pShirtNextBtn ) ;
	GM_DELETE( m_pShirtPrevBtn ) ;

	GM_DELETE( m_pCameraLabel ) ;
	GM_DELETE( m_pCameraSelectLabel ) ;
	GM_DELETE( m_pCameraRightBtn ) ;
	GM_DELETE( m_pCameraLeftBtn ) ;

	GM_DELETE( m_pStatusDivideLabel ) ;

	GM_DELETE( m_pStrLabel ) ;
	GM_DELETE( m_pStrValueLabel ) ;
	GM_DELETE( m_pStrPointIncBtn ) ;
	GM_DELETE( m_pStrPointDecBtn ) ;

	GM_DELETE( m_pDexLabel ) ;
	GM_DELETE( m_pDexValueLabel ) ;
	GM_DELETE( m_pDexPointIncBtn ) ;
	GM_DELETE( m_pDexPointDecBtn ) ;

	GM_DELETE( m_pConLabel ) ;
	GM_DELETE( m_pConValueLabel ) ;
	GM_DELETE( m_pConPointIncBtn ) ;
	GM_DELETE( m_pConPointDecBtn ) ;

	GM_DELETE( m_pIntLabel ) ;
	GM_DELETE( m_pIntValueLabel ) ;
	GM_DELETE( m_pIntPointIncBtn ) ;
	GM_DELETE( m_pIntPointDecBtn ) ;

	GM_DELETE( m_pWisLabel ) ;
	GM_DELETE( m_pWisValueLabel ) ;
	GM_DELETE( m_pWisPointIncBtn ) ;
	GM_DELETE( m_pWisPointDecBtn ) ;

	GM_DELETE( m_pBonusPointLabel ) ;
	GM_DELETE( m_pBonusPointValueLabel ) ;

	GM_DELETE( m_pMakeBtn ) ;
	GM_DELETE( m_pCalcelBtn ) ;

	GM_DELETE( m_pBonusPicture1 ) ;
	GM_DELETE( m_pBonusPicture2 ) ;
}

VOID	CRYLCharacterCreationDlg::Show( CGMUIObject* Sender )
{
	// 처음 생성 창이 뜰때 모델링 교체
	ChangeModelByRace() ;

	// BG Server에서 로그인 시 안내 멘트(?)를 보여주기 위한 다이얼로그
	if( CRYLNetworkData::Instance()->m_eServerType == GameRYL::SERVER_BATTLE_LOHAN )
	{
		CRYLMessageBox *lpMessageBox;
		lpMessageBox = new CRYLMessageBox;
		lpMessageBox->Create( CRYLStringTable::m_strString[ 3467 ], MB_CONFIRM | MB_EXIT );

		CGMUIScreen::GetInstance()->SetModal( lpMessageBox );
	}
}

VOID	CRYLCharacterCreationDlg::Hide( CGMUIObject* Sender )
{
	if ( CRYLChattingDlg::Instance() )
	{
		CRYLChattingDlg::Instance()->m_pChatEdit->SetFocus();
		CRYLChattingDlg::Instance()->m_pChatEdit->SetFocusState( FALSE ) ;
	}
}

VOID	CRYLCharacterCreationDlg::GMUIPaint( INT xx, INT yy )
{
	CRYLDialog::GMUIPaint( xx, yy ) ;
}

HRESULT		CRYLCharacterCreationDlg::Update( BOOL &bClick, BOOL &bEdge ) 
{
	if (NULL != GetCloseButton())
	{
		GetCloseButton()->SetVisible( FALSE ) ;
	}

	if ( CRYLSceneManager::Instance()->GetSceneID() != CHARACTERSELECT_SCENE )
	{
		return S_OK ;
	}

	if ( m_dwEndTimer != 0xFFFFFFFF )
	{
		if ( CFrameTimer::GetUpdateTimer( m_dwEndTimer ) >= 1.0f )
		{
			CRYLGameData::Instance()->m_dwClientState = CHRSELECT_INITSELECT ;
			CRYLGameData::Instance()->m_bCreateChar = FALSE ;
		}
		return S_OK ;
	}

	if ( CRYLGameData::Instance()->m_bCreateChar )
	{
		if ( g_pClientSocket->GetStatusFlag() == NS_NOWAITING )
		{
			if ( !CRYLNetworkData::Instance()->m_dwNetworkError )
			{
				InitValue();
				SetVisible( FALSE ) ;

				CHAR	strCameraFileName[ MAX_PATH ] ;
				CRYLGameData* pGame = CRYLGameData::Instance();
				m_dwEndTimer = CFrameTimer::Regist(1.0f);

				switch ( pGame->m_dwSelectChr )
				{
					case 0:	strcpy( strCameraFileName, "Back_CharacterSelect1.csf" ) ;   break ;
					case 1:	strcpy( strCameraFileName, "Back_CharacterSelect2.csf" ) ;   break ;
					case 2:	strcpy( strCameraFileName, "Back_CharacterSelect3.csf" ) ;   break ;
					case 3:	strcpy( strCameraFileName, "Back_CharacterSelect4.csf" ) ;   break ;
					case 4:	strcpy( strCameraFileName, "Back_CharacterSelect5.csf" ) ;   break ;
				}

				pGame->m_lpSceneManager->m_pCameraScript->InitCameraScript() ;
				pGame->m_lpSceneManager->m_pCameraScript->LoadScript( strCameraFileName ) ;
			//	pGame->m_lpSceneManager->m_pCameraScript->SetPlay( true, false, true ) ;
				pGame->m_dwSelectChr = 0xFFFFFFFF;

				CRYLCharacterSelectScene* pScene = ( CRYLCharacterSelectScene* )CRYLSceneManager::GetCurrentScene() ;
				pScene->ShowNationDlg() ;
			} 
			else
			{
				CRYLGameData::Instance()->m_bCreateChar = FALSE ;
				m_pClassSelectLabel->SetCaption( "" ) ;
			}
		}

		return S_OK ;
	}

	CHAR	szBuff[ 16 ] ;
	m_pBonusPointValueLabel->SetCaption( itoa( m_dwIP, szBuff, 10 ) ) ;
	m_pStrValueLabel->SetCaption( itoa( m_dwSTR, szBuff, 10 ) ) ;
	m_pDexValueLabel->SetCaption( itoa( m_dwDEX, szBuff, 10 ) ) ;
	m_pConValueLabel->SetCaption( itoa( m_dwCON, szBuff, 10 ) ) ;
	m_pIntValueLabel->SetCaption( itoa( m_dwINT, szBuff, 10 ) ) ;
	m_pWisValueLabel->SetCaption( itoa( m_dwWIS, szBuff, 10 ) ) ;

	switch ( m_dwRace )
	{
		case CClass::RaceType::HUMAN :
		{
			m_pClassSelectLabel->SetCaption( m_szHumanClassName[ m_iCurrentClass ] ) ;
			m_pSexSelectLabel->SetCaption( m_szSexName[ m_iCurrentSex ] ) ;
			m_pHairSelectLabel->SetCaption( m_szHairName[ m_iCurrentHair ] ) ;
			m_pFaceSelectLabel->SetCaption( m_szFaceName[ m_iCurrentFace ] ) ;
			m_pShirtSelectLabel->SetCaption( m_szShirtName[ m_iCurrentShirt ] ) ;
		}
		break ;

		case CClass::RaceType::AKHAN :
		{
			m_pClassSelectLabel->SetCaption( m_szAkhanClassName[ m_iCurrentClass ] ) ;
			m_pSexSelectLabel->SetCaption( CRYLStringTable::m_strString[ 3133 ] ) ;
			m_pHairSelectLabel->SetCaption( m_szHairName[ m_iCurrentHair ] ) ;
			m_pFaceSelectLabel->SetCaption( m_szFaceName[ m_iCurrentFace ] ) ;
			m_pShirtSelectLabel->SetCaption( CRYLStringTable::m_strString[ 3133 ] ) ;
		}
		break ;

		default :
		{
			m_pClassSelectLabel->SetCaption( "" ) ;
			m_pSexSelectLabel->SetCaption( "" ) ;
			m_pHairSelectLabel->SetCaption( "" ) ;
			m_pFaceSelectLabel->SetCaption( "" ) ;
			m_pShirtSelectLabel->SetCaption( "" ) ;
		}
		break ;
	}

	if ( g_DeviceInput.GetIsLeftMousePress() )
	{
		// 캐릭터 오른쪽 회전
		if ( g_DeviceInput.InRect( GetLeft() + 108, GetTop() + 235, GetLeft() + 121, GetTop() + 247 ) )
		{
			CharRightRotate() ;
		}

		// 캐릭터 왼쪽 회전
		if ( g_DeviceInput.InRect( GetLeft() + 197, GetTop() + 235, GetLeft() + 210, GetTop() + 247 ) )
		{
			CharLeftRotate() ;
		}
	}

	return S_OK ;
}

VOID	CRYLCharacterCreationDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y )
{
	if ( !g_pCharacterCreationDlg )
		return ;

	CRYLGameData*	pGame = CRYLGameData::Instance() ;
	RYLCreature*	pTempCreature = RYLCreatureManager::Instance()->GetCreatureByAddedOrder( pGame->m_dwSelectChr );
	if( !pTempCreature )
		return;

	CZ3DGeneralChrModel* pTempModel = pTempCreature->GetCharModel();
	if( !pTempModel )
		return;

	switch ( Sender->GetInstance() )
	{
		case RYLCHATACTERCREATION::CLASSNEXT_BUTTON :
		{
			g_pCharacterCreationDlg->ClassNameNext() ;
		}
		break;

		case RYLCHATACTERCREATION::CLASSPREV_BUTTON :
		{
			g_pCharacterCreationDlg->ClassNamePrev() ;
		}
		break;
		
		case RYLCHATACTERCREATION::SEXNEXT_BUTTON :
		{
			g_pCharacterCreationDlg->SexNameNext() ;
		}
		break;

		case RYLCHATACTERCREATION::SEXPREV_BUTTON :
		{
			g_pCharacterCreationDlg->SexNamePrev() ;
		}
		break;

		case RYLCHATACTERCREATION::HAIRNEXT_BUTTON :
		{
			g_pCharacterCreationDlg->HairNameNext() ;			
		}
		break ;

		case RYLCHATACTERCREATION::HAIRPREV_BUTTON :
		{
			g_pCharacterCreationDlg->HairNamePrev() ;
		}
		break ;

		case RYLCHATACTERCREATION::FACENEXT_BUTTON :
		{
			g_pCharacterCreationDlg->FaceNameNext() ;			
		}
		break ;

		case RYLCHATACTERCREATION::FACEPREV_BUTTON :
		{
			g_pCharacterCreationDlg->FaceNamePrev() ;
		}
		break ;

		case RYLCHATACTERCREATION::SHIRTNEXT_BUTTON :
		{
			g_pCharacterCreationDlg->ShirtNameNext() ;
		}
		break ;

		case RYLCHATACTERCREATION::SHIRTPREV_BUTTON :
		{
			g_pCharacterCreationDlg->ShirtNamePrev() ;
		}
		break ;

		case RYLCHATACTERCREATION::STRPOINTINC_BUTTON :
		{
			g_pCharacterCreationDlg->PointInc( TYPE_STR ) ;
		}
		break ;

		case RYLCHATACTERCREATION::STRPOINTDEC_BUTTON :
		{
			g_pCharacterCreationDlg->PointDec( TYPE_STR ) ;
		}
		break ;

		case RYLCHATACTERCREATION::DEXPOINTINC_BUTTON :
		{
			g_pCharacterCreationDlg->PointInc( TYPE_DEX ) ;
		}
		break ;

		case RYLCHATACTERCREATION::DEXPOINTDEC_BUTTON :
		{
			g_pCharacterCreationDlg->PointDec( TYPE_DEX ) ;
		}
		break ;

		case RYLCHATACTERCREATION::CONPOINTINC_BUTTON :
		{
			g_pCharacterCreationDlg->PointInc( TYPE_CON ) ;
		}
		break ;

		case RYLCHATACTERCREATION::CONPOINTDEC_BUTTON :
		{
			g_pCharacterCreationDlg->PointDec( TYPE_CON ) ;
		}
		break ;

		case RYLCHATACTERCREATION::INTPOINTINC_BUTTON :
		{
			g_pCharacterCreationDlg->PointInc( TYPE_INT ) ;
		}
		break ;

		case RYLCHATACTERCREATION::INTPOINTDEC_BUTTON :
		{
			g_pCharacterCreationDlg->PointDec( TYPE_INT ) ;
		}
		break ;

		case RYLCHATACTERCREATION::WISPOINTINC_BUTTON :
		{
			g_pCharacterCreationDlg->PointInc( TYPE_WIS ) ;
		}
		break ;

		case RYLCHATACTERCREATION::WISPOINTDEC_BUTTON :
		{
			g_pCharacterCreationDlg->PointDec( TYPE_WIS ) ;
		}
		break ;

		case RYLCHATACTERCREATION::MAKE_BUTTON :
		{
			g_pCharacterCreationDlg->MakeButton() ;
		}
		break ;

		case RYLCHATACTERCREATION::CALCEL_BUTTON :
		{
			g_pCharacterCreationDlg->CancelButton() ;
		}
		break ;

		case RYLCHATACTERCREATION::CAMERARIGHT_BUTTON :
		{
			g_pCharacterCreationDlg->CharRightRotate() ;
		}
		break ;

		case RYLCHATACTERCREATION::CAMERALEFT_BUTTON :
		{
			g_pCharacterCreationDlg->CharLeftRotate() ;
		}
		break ;
	}
}

VOID	CRYLCharacterCreationDlg::DlgKeyDown( CGMUIObject* Sender, WORD key, INT Shift )
{
	if ( !g_pCharacterCreationDlg )
		return ;

	switch ( Sender->GetInstance() )
	{
		case RYLCHATACTERCREATION::NAME_EDIT :
			break;
	}
}

VOID	CRYLCharacterCreationDlg::InitValue()
{
	m_iCurrentClass	= 0 ;
	m_iCurrentSex = 0 ;
	m_iCurrentHair = 1 ;
	m_iCurrentFace = 0 ;
	m_iCurrentShirt = 0 ;

	m_dwSTR			= 20 ;
	m_dwDEX			= 20 ;
	m_dwCON			= 20 ;
	m_dwINT			= 20 ;
	m_dwWIS			= 20 ;
	m_dwIP			= 5 ;

	CRYLGameData* pGame = CRYLGameData::Instance() ;
	switch(pGame->m_dwClientRace)
	{
		case CClass::RaceType::HUMAN:
			m_dwClass = 1;
			break;

		case CClass::RaceType::AKHAN:
			m_dwClass = 17;
			break;
	}
	m_dwSex			= 1 ;
	m_dwFace		= 1 ;
	m_dwHair		= 2 ;
	m_dwShirt		= 0 ;
	m_dwEndTimer	= 0xFFFFFFFF ;

	DrawBonusPicture() ;

	CRYLGameData::Instance()->m_bCreateChar = FALSE ;

	m_pNameEdit->SetText("") ;
}


VOID	CRYLCharacterCreationDlg::DrawBonusPicture()
{
	m_pBonusPicture2->SetVisible( TRUE ) ;

	switch ( m_dwClass )
	{
	case 1 :
		m_pBonusPicture1->SetTop( 297 ) ;
		m_pBonusPicture2->SetTop( 361 ) ;
		break ;
		
	case 2 :
		m_pBonusPicture1->SetTop( 297 ) ;
		m_pBonusPicture2->SetTop( 329 ) ;
		break ;
	
	case 3 :
		m_pBonusPicture1->SetTop( 329 ) ;
		m_pBonusPicture2->SetTop( 393 ) ;
		break ;
	
	case 4 :
		m_pBonusPicture1->SetTop( 361 ) ;
		m_pBonusPicture2->SetTop( 425 ) ;
		break ;

	case 17 :
		m_pBonusPicture1->SetTop( 297 ) ;
//		m_pBonusPicture1->SetTop( 361 ) ;
		m_pBonusPicture2->SetVisible( FALSE ) ;
		break ;

	case 18 :
		m_pBonusPicture1->SetTop( 329 ) ;
		m_pBonusPicture2->SetVisible( FALSE ) ;
		break ;
	}
}

VOID	CRYLCharacterCreationDlg::ClassNameNext()
{
	int nMaxNum = RYLCHATACTERCREATION::HUMAN_CLASS_NUM ;
	if ( m_dwRace == CClass::RaceType::AKHAN ) nMaxNum = RYLCHATACTERCREATION::AKHAN_CLASS_NUM ;

	if ( m_iCurrentClass == (nMaxNum - 1) )
		m_iCurrentClass = 0 ;
	else
		++ m_iCurrentClass ;

	switch ( m_dwRace )
	{
		case CClass::RaceType::HUMAN :	m_dwClass = m_iCurrentClass + 1 ;	break ;
		case CClass::RaceType::AKHAN :	m_dwClass = m_iCurrentClass + 17 ;	break ;
	}

	ChangeModelByClass() ;
	DrawBonusPicture() ;
}

VOID	CRYLCharacterCreationDlg::ClassNamePrev()
{
	int nMaxNum = RYLCHATACTERCREATION::HUMAN_CLASS_NUM ;
	if ( m_dwRace == CClass::RaceType::AKHAN ) nMaxNum = RYLCHATACTERCREATION::AKHAN_CLASS_NUM ;

	if ( m_iCurrentClass == 0 )
		m_iCurrentClass = nMaxNum - 1 ;
	else
		-- m_iCurrentClass ;

	switch ( m_dwRace )
	{
		case CClass::RaceType::HUMAN :	m_dwClass = m_iCurrentClass + 1 ;	break ;
		case CClass::RaceType::AKHAN :	m_dwClass = m_iCurrentClass + 17 ;	break ;
	}

	ChangeModelByClass() ;
	DrawBonusPicture() ;
}

VOID	CRYLCharacterCreationDlg::SexNameNext()
{
	if ( m_dwRace != CClass::RaceType::AKHAN )
	{
		if ( m_iCurrentSex == 1 )
		{
			m_iCurrentSex = 0 ;
		}
		else
		{
			++ m_iCurrentSex ;
		}

		m_dwSex = m_iCurrentSex + 1 ;

		// 남녀의 헤어스타일 수가 다르므로 초기화 필요
		HairNameNext();					

		ChangeModelBySex() ;
	}
}

VOID	CRYLCharacterCreationDlg::SexNamePrev()
{
	if ( m_dwRace != CClass::RaceType::AKHAN )
	{
		if ( m_iCurrentSex == 0 )
		{
			m_iCurrentSex = 1 ;
		}
		else
		{
			-- m_iCurrentSex ;
		}

		m_dwSex = m_iCurrentSex + 1 ;

		// 남녀의 헤어스타일 수가 다르므로 초기화 필요
		HairNameNext();					

		ChangeModelBySex() ;
	}
}

VOID	CRYLCharacterCreationDlg::HairNameNext()
{
	CRYLGameData*	pGame = CRYLGameData::Instance() ;
	RYLCreature*	pTempCreature = RYLCreatureManager::Instance()->GetCreatureByAddedOrder( pGame->m_dwSelectChr );
	if( pTempCreature )
	{
		CZ3DGeneralChrModel* pTempModel = pTempCreature->GetCharModel();
		if( pTempModel )
		{
			int nMaxNum = RYLCHATACTERCREATION::HUMAN_HAIR_MAN_NUM ;		// 남자 : 1
			if ( m_dwRace == CClass::RaceType::HUMAN )
			{
				if ( m_dwSex == 2 ) 
				{
					nMaxNum = RYLCHATACTERCREATION::HUMAN_HAIR_WOMEN_NUM ;		// 여자 : 2
				}
			} 
			else if ( m_dwRace == CClass::RaceType::AKHAN ) 
			{
				nMaxNum = RYLCHATACTERCREATION::AKHAN_HAIR_NUM ;
			}

			if ( m_iCurrentHair >= (nMaxNum - 1) )
			{
				m_iCurrentHair = 0 ;
			}
			else 
			{
				++ m_iCurrentHair ;
			}

			m_dwHair = m_iCurrentHair + 1 ;

			char Temp[ 10 ] ;
			sprintf( Temp, "HAIR%02d", m_dwHair ) ;
			pTempModel->SetStaticSlot( "HAIR", Temp ) ;
		}
	}
}

VOID	CRYLCharacterCreationDlg::HairNamePrev()
{
	CRYLGameData*	pGame = CRYLGameData::Instance() ;
	RYLCreature*	pTempCreature = RYLCreatureManager::Instance()->GetCreatureByAddedOrder( pGame->m_dwSelectChr );
	if( pTempCreature )
	{
		CZ3DGeneralChrModel* pTempModel = pTempCreature->GetCharModel();
		if( pTempModel )
		{
			int nMaxNum = RYLCHATACTERCREATION::HUMAN_HAIR_MAN_NUM ;		// 남자 : 1
			if ( m_dwRace == CClass::RaceType::HUMAN )
			{
				if ( m_dwSex == 2 ) 
				{
					nMaxNum = RYLCHATACTERCREATION::HUMAN_HAIR_WOMEN_NUM ;		// 여자 : 2
				}
			} 
			else if ( m_dwRace == CClass::RaceType::AKHAN ) 
			{
				nMaxNum = RYLCHATACTERCREATION::AKHAN_HAIR_NUM ;
			}

			if ( m_iCurrentHair == 0 )
			{
				m_iCurrentHair = nMaxNum - 1 ;
			}
			else 
			{
				-- m_iCurrentHair ;
			}

			m_dwHair = m_iCurrentHair + 1 ;

			char Temp[ 10 ] ;
			sprintf( Temp, "HAIR%02d", m_dwHair ) ;
			pTempModel->SetStaticSlot( "HAIR", Temp ) ;
		}
	}
}

VOID	CRYLCharacterCreationDlg::FaceNameNext()
{
	CRYLGameData*	pGame = CRYLGameData::Instance() ;
	RYLCreature*	pTempCreature = RYLCreatureManager::Instance()->GetCreatureByAddedOrder( pGame->m_dwSelectChr );
	if( !pTempCreature )
		return;

	CZ3DGeneralChrModel* pTempModel = pTempCreature->GetCharModel();
	if( !pTempModel )
		return;

	int nMaxNum = RYLCHATACTERCREATION::HUMAN_FACE_NUM ;
	if ( m_dwRace == CClass::RaceType::AKHAN ) nMaxNum = RYLCHATACTERCREATION::AKHAN_FACE_NUM ;

	if ( m_iCurrentFace == (nMaxNum - 1) )
		m_iCurrentFace = 0 ;
	else 
		++ m_iCurrentFace ;

	m_dwFace = m_iCurrentFace + 1 ;

	char Temp[ 10 ] ;
	sprintf( Temp, "FACE%02d", m_dwFace ) ;
	pTempModel->SetStaticSlot( "FACE", Temp ) ;
}

VOID	CRYLCharacterCreationDlg::FaceNamePrev()
{
	CRYLGameData*	pGame = CRYLGameData::Instance() ;
	RYLCreature*	pTempCreature = RYLCreatureManager::Instance()->GetCreatureByAddedOrder( pGame->m_dwSelectChr );
	if( !pTempCreature )
		return;

	CZ3DGeneralChrModel* pTempModel = pTempCreature->GetCharModel();
	if( !pTempModel )
		return;

	int nMaxNum = RYLCHATACTERCREATION::HUMAN_FACE_NUM ;
	if ( m_dwRace == CClass::RaceType::AKHAN ) nMaxNum = RYLCHATACTERCREATION::AKHAN_FACE_NUM ;

	if ( m_iCurrentFace == 0 )
		m_iCurrentFace = nMaxNum - 1 ;
	else
		-- m_iCurrentFace ;

	m_dwFace = m_iCurrentFace + 1 ;

	char Temp[ 10 ] ;
	sprintf( Temp, "FACE%02d", m_dwFace ) ;
	pTempModel->SetStaticSlot( "FACE", Temp ) ;
}

VOID	CRYLCharacterCreationDlg::ShirtNameNext()
{
	if ( m_dwRace == CClass::RaceType::AKHAN )
		return;

	CRYLGameData* pGame = CRYLGameData::Instance() ;
	RYLCreature*	pTempCreature = RYLCreatureManager::Instance()->GetCreatureByAddedOrder( pGame->m_dwSelectChr );
	if( !pTempCreature )
		return;

	CZ3DGeneralChrModel* pTempModel = pTempCreature->GetCharModel();
	if( !pTempModel )
		return;

	if ( m_iCurrentShirt == RYLCHATACTERCREATION::HUMAN_SHIRT_NUM )
		m_iCurrentShirt = 0 ;
	else
		++ m_iCurrentShirt ;

	m_dwShirt = m_iCurrentShirt ;

	pTempModel->BeginOutfitSetting() ;
	pTempModel->SetOutfitSlot( "BODY", g_ItemMgr.GetEquipName( m_dwShirt + 201 ) ) ;
	pTempModel->SetOutfitSlot( "FOOT", g_ItemMgr.GetEquipName( 601 ) ) ;
	pTempModel->EndOutfitSetting() ;
}

VOID	CRYLCharacterCreationDlg::ShirtNamePrev()
{
	if ( m_dwRace == CClass::RaceType::AKHAN )
		return;

	CRYLGameData* pGame = CRYLGameData::Instance() ;
	RYLCreature*	pTempCreature = RYLCreatureManager::Instance()->GetCreatureByAddedOrder( pGame->m_dwSelectChr );
	if( !pTempCreature )
		return;

	CZ3DGeneralChrModel* pTempModel = pTempCreature->GetCharModel();
	if( !pTempModel )
		return;

	if ( m_iCurrentShirt == 0 )
		m_iCurrentShirt = RYLCHATACTERCREATION::HUMAN_SHIRT_NUM ;
	else
		-- m_iCurrentShirt ;

	m_dwShirt = m_iCurrentShirt ;

	pTempModel->BeginOutfitSetting() ;
	pTempModel->SetOutfitSlot( "BODY", g_ItemMgr.GetEquipName( m_dwShirt + 201 ) ) ;
	pTempModel->SetOutfitSlot( "FOOT", g_ItemMgr.GetEquipName( 601 ) ) ;
	pTempModel->EndOutfitSetting() ;
}

VOID	CRYLCharacterCreationDlg::PointInc(char cType)
{
	if ( cType >= MAX_TYPE_NUM )
		return;

    if ( m_dwIP )
	{
		m_dwIP -- ;

		switch ( cType )
		{
			case TYPE_STR :		++ m_dwSTR ;	break ;
			case TYPE_DEX :		++ m_dwDEX ;	break ;
			case TYPE_CON :		++ m_dwCON ;	break ;
			case TYPE_INT :		++ m_dwINT ;	break ;
			case TYPE_WIS :		++ m_dwWIS ;	break ;
		}
	}
}

VOID	CRYLCharacterCreationDlg::PointDec(char cType)
{
	if ( cType >= MAX_TYPE_NUM )
		return;

	switch ( cType )
	{
		case TYPE_STR :
		{
			if ( m_dwSTR > 20 )
			{
				-- m_dwSTR ;
				++ m_dwIP ;
			}
		}
		break;

		case TYPE_DEX :
		{
			if ( m_dwDEX > 20 )
			{
				-- m_dwDEX ;
				++ m_dwIP ;
			}
		}
		break;

		case TYPE_CON :
		{
			if ( m_dwCON > 20 )
			{
				-- m_dwCON ;
				++ m_dwIP ;
			}
		}
		break;

		case TYPE_INT :
		{
			if ( m_dwINT > 20 )
			{
				-- m_dwINT ;
				++ m_dwIP ;
			}
		}
		break;

		case TYPE_WIS :
		{
			if ( m_dwWIS > 20 )
			{
				-- m_dwWIS ;
				++ m_dwIP ;
			}
		}
		break;
	}
}

VOID	CRYLCharacterCreationDlg::CharRightRotate()
{
	CRYLGameData* pGame = CRYLGameData::Instance() ;
	RYLCreature*	pTempCreature = RYLCreatureManager::Instance()->GetCreatureByAddedOrder( pGame->m_dwSelectChr );
	if( !pTempCreature )
		return;

	CZ3DGeneralChrModel* pTempModel = pTempCreature->GetCharModel();
	if( !pTempModel )
		return;

	float fDir = pTempCreature->GetDirection() ;
	pTempCreature->SetDirection( fDir + 0.15f ) ;
}

VOID	CRYLCharacterCreationDlg::CharLeftRotate()
{
	CRYLGameData* pGame = CRYLGameData::Instance() ;
	RYLCreature*	pTempCreature = RYLCreatureManager::Instance()->GetCreatureByAddedOrder( pGame->m_dwSelectChr );
	if( !pTempCreature )
		return;

	CZ3DGeneralChrModel* pTempModel = pTempCreature->GetCharModel();
	if( !pTempModel )
		return;

	float fDir = pTempCreature->GetDirection() ;
	pTempCreature->SetDirection( fDir - 0.15f ) ;
}

VOID	CRYLCharacterCreationDlg::MakeButton()
{
	int		len = strlen( m_pNameEdit->GetText() ), i ;
	LPSTR	pszName = m_pNameEdit->GetText() ;

	if ( len == 0 )// 캐릭터 이름이 없습니다.
	{
		CRYLMessageBox *lpMessage;
		lpMessage = new CRYLMessageBox;
		lpMessage->Create( CRYLStringTable::m_strString[110]);
		lpMessage->SetTimer(1000);
		return ;
	} 
	else if( len < 4 )// 캐릭터 이름이 짧습니다.
	{
		CRYLMessageBox *lpMessage;
		lpMessage = new CRYLMessageBox;
		lpMessage->Create(CRYLStringTable::m_strString[111]);
		lpMessage->SetTimer(1000);
		return ;
	}

	if ( g_pCharacterCreationDlg->m_dwIP )// 포인트가 남았다.
	{
		CRYLMessageBox *lpMessage;
		lpMessage = new CRYLMessageBox;
		lpMessage->Create(CRYLStringTable::m_strString[112]);
		lpMessage->SetTimer(1000);
		return ;
	}

	for ( i = 0; i < len; i ++ )
	{
		if ( pszName[ i ] == ' ' )// 빈칸이 있습니다.
		{
			CRYLMessageBox *lpMessage;
			lpMessage = new CRYLMessageBox;
			lpMessage->Create(CRYLStringTable::m_strString[113]);
			lpMessage->SetTimer(1000);
			return ;
		}

		if ( pszName[ i ] == 34 || pszName[ i ] == 39 )// 이름이 따옴표가 있습니다.
		{
			CRYLMessageBox *lpMessage;
			lpMessage = new CRYLMessageBox;
			lpMessage->Create(CRYLStringTable::m_strString[114]);
			lpMessage->SetTimer(1000);
			return ;
		}
	}
/*
	if(g_ClientMain.m_wAdminMode != ADMIN_L3) 
	{
		// 이름 제제
		if (!strcmp(pszName, "edith"))
		{
			CRYLMessageBox* lpMessage = new CRYLMessageBox;
			lpMessage->Create(CRYLStringTable::m_strString[115]);
			lpMessage->SetTimer(1000);
			return ;
		}
	}
*/
	if ( false == g_StringFilter.Filter( pszName ) )
	{
		CRYLMessageBox* lpMessage = new CRYLMessageBox;
		lpMessage->Create(CRYLStringTable::m_strString[2784]);
		lpMessage->SetTimer(1000);
		return ;
	}

	if ( CRYLNetworkData::Instance()->m_eInternationalCode == GameRYL::KOREA ) // 포인트가 남았다.
	{
		if ( !CheckCharCreateName( m_pNameEdit->GetText() ) )
		{
			CRYLMessageBox* lpMessage = new CRYLMessageBox;
			lpMessage->Create(CRYLStringTable::m_strString[116]);
			lpMessage->SetTimer(1000);
			return ;
		}
	}

	CHAR_CREATE	CreateChrInfo ;

	ZeroMemory( &CreateChrInfo, sizeof( CHAR_CREATE ) ) ;
	memcpy( CreateChrInfo.Name, pszName, 0x10 ) ;

	CreateChrInfo.Race	= m_dwRace ;
	CreateChrInfo.Sex	= m_dwSex ;
	CreateChrInfo.Face	= m_dwFace ;
	CreateChrInfo.Hair	= m_dwHair ;
	CreateChrInfo.Class = m_dwClass ; 

	CreateChrInfo.STR = m_dwSTR ;
	CreateChrInfo.DEX = m_dwDEX ;
	CreateChrInfo.CON = m_dwCON ;
	CreateChrInfo.INT = m_dwINT ;
	CreateChrInfo.WIS = m_dwWIS ;

	CreateChrInfo.Equip[ 0 ] = 0 ;
	CreateChrInfo.Equip[ 1 ] = 0 ;
	CreateChrInfo.Equip[ 2 ] = 0 ;
	CreateChrInfo.Equip[ 3 ] = 0 ;
	CreateChrInfo.Equip[ 4 ] = 0 ;
	CreateChrInfo.Equip[ 5 ] = 0 ;
	CreateChrInfo.Equip[ 6 ] = 0 ;
	CreateChrInfo.Equip[ 7 ] = 0 ;
	CreateChrInfo.Equip[ 8 ] = 0 ;
	CreateChrInfo.Equip[ 9 ] = 0 ;
	CreateChrInfo.Equip[ 10 ] = 0 ;
	CreateChrInfo.Equip[ 11 ] = 0 ;
	CreateChrInfo.Equip[ 12 ] = 0 ;
	CreateChrInfo.Equip[ 13 ] = 0 ;
	CreateChrInfo.Equip[ 14 ] = 0 ;

	CRYLGameData* pGame = CRYLGameData::Instance() ;

	switch ( m_dwRace )
	{
		case CClass::RaceType::HUMAN :
		{
			CreateChrInfo.Equip[ 1 ] = m_dwShirt + 201 ;					// 셔츠
			CreateChrInfo.Equip[ 5 ] = 601 ;								// 래더 부츠

			switch ( m_dwClass )
			{
				case 1 :	CreateChrInfo.Equip[ 11 ] = 701 ;		break ;	// 프랙티스 소드
				case 2 :	CreateChrInfo.Equip[ 11 ] = 1601 ;		break ;	// 대거
				case 3 :	CreateChrInfo.Equip[ 11 ] = 1501 ;		break ;	// 스태프
				case 4 :	CreateChrInfo.Equip[ 11 ] = 801 ;		break ;	// 몽둥이
			}
		}
		break;
			
		case CClass::RaceType::AKHAN :
		{
			switch ( m_dwClass )
			{
				case 17 :
				{
					CreateChrInfo.Equip[ 3 ] = 5101 ;		// 값옷 (컴베턴트)
					CreateChrInfo.Equip[ 5 ] = 5201 ;		// 펠비스 (컴베턴트)
					CreateChrInfo.Equip[ 11 ] = 5401 ;		// 스틸 스틱
				}
				break ;

				case 18 :
				{
					CreateChrInfo.Equip[ 3 ] = 5110 ;		// 갑옷 (오피세이터)
					CreateChrInfo.Equip[ 5 ] = 5210 ;		// 펠비스 (오피세이터)
					CreateChrInfo.Equip[ 11 ] = 5801 ;		// 웨이브 슬러셔
				}
				break ;
			}
		}
		break;
	}

	CRYLNetworkData::Instance()->SendCreateChar( CRYLGameData::Instance()->m_dwSelectChr, CreateChrInfo ) ;

	CRYLMessageBox *lpMessage;
	lpMessage = new CRYLMessageBox;
	lpMessage->Create(CRYLStringTable::m_strString[117]);
	lpMessage->SetTimer(100000);

	CRYLGameData::Instance()->m_bCreateChar = TRUE ;
}

VOID	CRYLCharacterCreationDlg::CancelButton()
{
	CRYLGameData* pGame = CRYLGameData::Instance() ;
	RYLCreature*	pTempCreature = RYLCreatureManager::Instance()->GetCreatureByAddedOrder( pGame->m_dwSelectChr );
	if( !pTempCreature )
		return;

	CZ3DGeneralChrModel* pTempModel = pTempCreature->GetCharModel();
	if( !pTempModel )
		return;

	for( CZ3DGeneralChrModel* pTempModel2 = RYLCreatureManager::Instance()->GetModelListHead() ;
		pTempModel2 ;
		pTempModel2 = RYLCreatureManager::Instance()->GetModelListNext() )
	{
		if( pTempModel == pTempModel2 )
		{
			RYLCreatureManager::Instance()->FreeModel( pTempModel );

			pTempCreature->SetModelFileName( "NPC_ChrSelect.GCMDS" );
			pTempCreature->SetHairType( "FACE01" );
			pTempCreature->SetFaceType( "HAIR01" );

			pTempModel2 = pTempModel = RYLCreatureManager::Instance()->AllocModel( pTempCreature );

			break;
		}
	}

	pTempModel->SetPosition( pGame->m_vecCharSelectPos[ pGame->m_dwSelectChr ] ) ;
	pTempModel->SetDirection( pGame->m_fCharSelectDir[ pGame->m_dwSelectChr ] ) ;
	pTempCreature->InitWeapon( 0 ) ;

	InitValue();
	SetVisible( FALSE ) ;

	CHAR	strCameraFileName[ MAX_PATH ] ;
	m_dwEndTimer = CFrameTimer::Regist(1.0f);

	switch ( pGame->m_dwSelectChr )
	{
		case 0:	strcpy( strCameraFileName, "Back_CharacterSelect1.csf" ) ;   break ;
		case 1:	strcpy( strCameraFileName, "Back_CharacterSelect2.csf" ) ;   break ;
		case 2:	strcpy( strCameraFileName, "Back_CharacterSelect3.csf" ) ;   break ;
		case 3:	strcpy( strCameraFileName, "Back_CharacterSelect4.csf" ) ;   break ;
		case 4:	strcpy( strCameraFileName, "Back_CharacterSelect5.csf" ) ;   break ;
	}

	pGame->m_lpSceneManager->m_pCameraScript->InitCameraScript() ;
	pGame->m_lpSceneManager->m_pCameraScript->LoadScript( strCameraFileName ) ;
//	pGame->m_lpSceneManager->m_pCameraScript->SetPlay( true, false, true ) ;
	pGame->m_dwSelectChr = 0xFFFFFFFF;

	CRYLCharacterSelectScene* pScene = ( CRYLCharacterSelectScene* )CRYLSceneManager::GetCurrentScene() ;
	pScene->ShowNationDlg() ;
}

VOID	CRYLCharacterCreationDlg::ChangeModelByRace()
{
	CRYLGameData* pGame = CRYLGameData::Instance() ;

	RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreatureByAddedOrder( pGame->m_dwSelectChr );
	if( !pTempCreature )
		return;

	CZ3DGeneralChrModel* pTempModel = pTempCreature->GetCharModel();
	if( !pTempModel )
		return;

	char Temp[ 10 ], Temp1[ 10 ] ;
	const char* strShape[Item::EquipmentPos::MAX_EQUPMENT_POS];
	std::fill_n(strShape, int(Item::EquipmentPos::MAX_EQUPMENT_POS), reinterpret_cast<const char *>(NULL));

	switch ( m_dwRace )
	{
		// 인간
		case CClass::RaceType::HUMAN :
		{
			InitValue();

			sprintf( Temp,  "HAIR%02d", m_dwHair ) ;
			sprintf( Temp1, "FACE%02d", m_dwFace ) ;

			for ( CZ3DGeneralChrModel* pTempModel2 = RYLCreatureManager::Instance()->GetModelListHead() ;
				  pTempModel2 ;
				  pTempModel2 = RYLCreatureManager::Instance()->GetModelListNext() )
			{
				if ( pTempModel == pTempModel2 )
				{
					RYLCreatureManager::Instance()->FreeModel( pTempModel );

					pTempCreature->SetModelFileName( "PC_MAN.GCMDS" );
					pTempCreature->SetHairType( Temp );
					pTempCreature->SetFaceType( Temp1 );

					pTempModel2 = pTempModel = RYLCreatureManager::Instance()->AllocModel( pTempCreature ); 

					pTempModel->SetStaticSlot( "HAIR", Temp ) ;
					pTempModel->SetStaticSlot( "FACE", Temp1 ) ;

					strShape[ Item::EquipmentPos::SHIRT ] = g_ItemMgr.GetEquipName( 201 ) ;			// 그린 셔츠
					strShape[ Item::EquipmentPos::BOOTS ] = g_ItemMgr.GetEquipName( 601 ) ;			// 래더 부츠
					strShape[ Item::EquipmentPos::WEAPON_HAND1 ] = g_ItemMgr.GetEquipName( 701 ) ;	// 프랙티스 소드

					RYLCharacterDataManager::Instance()->InitCharData( pTempCreature->GetCharID(), CClass::RaceType::HUMAN ) ;
					pTempCreature->SetShape( strShape, Broadcast2nd::CNetworkEquipGrade(), true ) ;
					break;
				}				
			}

			pTempModel->SetPosition( pGame->m_vecCharSelectPos[ pGame->m_dwSelectChr ] ) ;
			pTempModel->SetDirection( pGame->m_fCharSelectDir[ pGame->m_dwSelectChr ] ) ;
			pTempCreature->InitWeapon( 0 ) ;
		}
		break ;

		// 아칸
		case CClass::RaceType::AKHAN : 
		{
			InitValue();

			m_dwClass = 17 ;

			sprintf( Temp,  "HAIR%02d", m_dwHair ) ;
			sprintf( Temp1, "FACE%02d", m_dwFace ) ;

			for ( CZ3DGeneralChrModel* pTempModel2 = RYLCreatureManager::Instance()->GetModelListHead() ;
				  pTempModel2 ;
				  pTempModel2 = RYLCreatureManager::Instance()->GetModelListNext() )
			{
				if ( pTempModel == pTempModel2 )
				{
					RYLCreatureManager::Instance()->FreeModel( pTempModel );

					pTempCreature->SetModelFileName( "PC_Akhan_A.GCMDS" );
					pTempCreature->SetHairType( Temp );
					pTempCreature->SetFaceType( Temp1 );

					pTempModel2 = pTempModel = RYLCreatureManager::Instance()->AllocModel( pTempCreature );

					pTempModel->SetStaticSlot( "HAIR", Temp ) ;
					pTempModel->SetStaticSlot( "FACE", Temp1 ) ;

					strShape[ Item::EquipmentPos::BODY ] = g_ItemMgr.GetEquipName( 5101 ) ;			// 갑옷
					strShape[ Item::EquipmentPos::PELVIS ] = g_ItemMgr.GetEquipName( 5201 ) ;		// 펠비스
					strShape[ Item::EquipmentPos::WEAPON ] = g_ItemMgr.GetEquipName( 5401 ) ;		// 스틸 스틱

					RYLCharacterDataManager::Instance()->InitCharData( pTempCreature->GetCharID(), CClass::RaceType::AKHAN ) ;
					pTempCreature->SetShape( strShape, Broadcast2nd::CNetworkEquipGrade(), true ) ;
					break;
				}
			}

			pTempModel->SetPosition( pGame->m_vecCharSelectPos[ pGame->m_dwSelectChr ] ) ;
			pTempModel->SetDirection( pGame->m_fCharSelectDir[ pGame->m_dwSelectChr ] ) ;
			pTempCreature->InitWeapon( 0 ) ;
		}
		break ;

	}
}

VOID	CRYLCharacterCreationDlg::ChangeModelByClass()
{
	CRYLGameData* pGame = CRYLGameData::Instance() ;

	RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreatureByAddedOrder( pGame->m_dwSelectChr );
	if( !pTempCreature )
		return;

	CZ3DGeneralChrModel* pTempModel = pTempCreature->GetCharModel();
	if( !pTempModel )
		return;

	char Temp[ 10 ], Temp1[ 10 ] ;
	const char* strShape[Item::EquipmentPos::MAX_EQUPMENT_POS];
	sprintf( Temp,  "HAIR%02d", m_dwHair ) ;
	sprintf( Temp1, "FACE%02d", m_dwFace ) ;
	std::fill_n(strShape, int(Item::EquipmentPos::MAX_EQUPMENT_POS), reinterpret_cast<const char *>(NULL));

	switch ( m_dwClass )
	{
		// 휴먼은 손의 무기만 바꿔준다.

		case 1 :	// 파이터
		case 2 :	// 로그
		case 3 :	// 메이지
		case 4 :	// 어콜라이트
		{
			pTempModel->SetStaticSlot( "HAIR", Temp ) ;
			pTempModel->SetStaticSlot( "FACE", Temp1 ) ;

			strShape[ Item::EquipmentPos::SHIRT ] = g_ItemMgr.GetEquipName( m_dwShirt + 201 ) ;	// 셔츠
			strShape[ Item::EquipmentPos::BOOTS ] = g_ItemMgr.GetEquipName( 601 ) ;				// 래더 부츠

			switch ( m_dwClass )
			{
				case 1 :	strShape[ Item::EquipmentPos::WEAPON_HAND1 ] = g_ItemMgr.GetEquipName( 701 ) ;	break;	// 프랙티스 소드
				case 2 :	strShape[ Item::EquipmentPos::WEAPON_HAND1 ] = g_ItemMgr.GetEquipName( 1601 ) ;	break;	// 대거
				case 3 :	strShape[ Item::EquipmentPos::WEAPON_HAND1 ] = g_ItemMgr.GetEquipName( 1501 ) ;	break;	// 스태프
				case 4 :	strShape[ Item::EquipmentPos::WEAPON_HAND1 ] = g_ItemMgr.GetEquipName( 801 ) ;	break;	// 클럽
			}

			pTempCreature->SetShape( strShape, Broadcast2nd::CNetworkEquipGrade(), true ) ;
			pTempCreature->InitWeapon( 0 ) ;
		}
		break;
	
		// 아칸은 모델링을 교체해준다.

		// 컴베턴트
		case 17 :
		{
			m_dwSex	= 1 ;

			for ( CZ3DGeneralChrModel* pTempModel2 = RYLCreatureManager::Instance()->GetModelListHead() ;
				  pTempModel2 ;
				  pTempModel2 = RYLCreatureManager::Instance()->GetModelListNext() )
			{
				if ( pTempModel == pTempModel2 )
				{
					RYLCreatureManager::Instance()->FreeModel( pTempModel );

					pTempCreature->SetModelFileName( "PC_Akhan_A.GCMDS" );
					pTempCreature->SetHairType( Temp );
					pTempCreature->SetFaceType( Temp1 );

					pTempModel2 = pTempModel = RYLCreatureManager::Instance()->AllocModel( pTempCreature ); 

					pTempModel->SetStaticSlot( "HAIR", Temp ) ;
					pTempModel->SetStaticSlot( "FACE", Temp1 ) ;

					strShape[ Item::EquipmentPos::BODY ] = g_ItemMgr.GetEquipName( 5101 ) ;			// 갑옷
					strShape[ Item::EquipmentPos::PELVIS ] = g_ItemMgr.GetEquipName( 5201 ) ;		// 펠비스
					strShape[ Item::EquipmentPos::WEAPON ] = g_ItemMgr.GetEquipName( 5401 ) ;		// 스틸 스틱

					pTempCreature->SetShape( strShape, Broadcast2nd::CNetworkEquipGrade(), true ) ;
					break;
				}				
			}

			pTempModel->SetPosition( pGame->m_vecCharSelectPos[ pGame->m_dwSelectChr ] ) ;
			pTempModel->SetDirection( pGame->m_fCharSelectDir[ pGame->m_dwSelectChr ] ) ;
			pTempCreature->InitWeapon( 0 ) ;
		}
		break ;

		// 오피세이터
		case 18 : 
		{
			m_dwSex	= 2 ;

			for ( CZ3DGeneralChrModel* pTempModel2 = RYLCreatureManager::Instance()->GetModelListHead() ;
				  pTempModel2 ;
				  pTempModel2 = RYLCreatureManager::Instance()->GetModelListNext() )
			{
				if ( pTempModel == pTempModel2 )
				{
					RYLCreatureManager::Instance()->FreeModel( pTempModel );

					pTempCreature->SetModelFileName( "PC_Akhan_B.GCMDS" );
					pTempCreature->SetHairType( Temp );
					pTempCreature->SetFaceType( Temp1 );

					pTempModel2 = pTempModel = RYLCreatureManager::Instance()->AllocModel( pTempCreature );

					pTempModel->SetStaticSlot( "HAIR", Temp ) ;
					pTempModel->SetStaticSlot( "FACE", Temp1 ) ;

					strShape[ Item::EquipmentPos::BODY ] = g_ItemMgr.GetEquipName( 5110 ) ;			// 갑옷
					strShape[ Item::EquipmentPos::PELVIS ] = g_ItemMgr.GetEquipName( 5210 ) ;		// 펠비스
					strShape[ Item::EquipmentPos::WEAPON ] = g_ItemMgr.GetEquipName( 5801 ) ;		// 웨이브슬러셔

					pTempCreature->SetShape( strShape, Broadcast2nd::CNetworkEquipGrade(), true ) ;
					break;
				}				
			}

			pTempModel->SetPosition( pGame->m_vecCharSelectPos[ pGame->m_dwSelectChr ] ) ;
			pTempModel->SetDirection( pGame->m_fCharSelectDir[ pGame->m_dwSelectChr ] ) ;
			pTempCreature->InitWeapon( 0 ) ;
		}
		break ;

	}
}

VOID	CRYLCharacterCreationDlg::ChangeModelBySex()
{
	if ( CClass::RaceType::AKHAN == m_dwRace )
		return;

	CRYLGameData* pGame = CRYLGameData::Instance() ;

	RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreatureByAddedOrder( pGame->m_dwSelectChr );
	if( !pTempCreature )
		return;

	CZ3DGeneralChrModel* pTempModel = pTempCreature->GetCharModel();
	if( !pTempModel )
		return;

	char Temp[ 10 ], Temp1[ 10 ] ;
	const char* strShape[Item::EquipmentPos::MAX_EQUPMENT_POS];
	sprintf( Temp,  "HAIR%02d", m_dwHair ) ;
	sprintf( Temp1, "FACE%02d", m_dwFace ) ;
	std::fill_n(strShape, int(Item::EquipmentPos::MAX_EQUPMENT_POS), reinterpret_cast<const char *>(NULL));

	switch ( m_dwSex )
	{
		case 1 :	// 남자로
		{
			for ( CZ3DGeneralChrModel* pTempModel2 = RYLCreatureManager::Instance()->GetModelListHead() ;
				  pTempModel2 ;
				  pTempModel2 = RYLCreatureManager::Instance()->GetModelListNext() )
			{
				if ( pTempModel == pTempModel2 )
				{
					RYLCreatureManager::Instance()->FreeModel( pTempModel );

					pTempCreature->SetModelFileName( "PC_MAN.GCMDS" );
					pTempCreature->SetHairType( Temp );
					pTempCreature->SetFaceType( Temp1 );

					pTempModel2 = pTempModel = RYLCreatureManager::Instance()->AllocModel( pTempCreature );

					pTempModel->SetStaticSlot( "HAIR", Temp ) ;
					pTempModel->SetStaticSlot( "FACE", Temp1 ) ;

					strShape[ Item::EquipmentPos::SHIRT ] = g_ItemMgr.GetEquipName( m_dwShirt + 201 ) ;	// 셔츠
					strShape[ Item::EquipmentPos::BOOTS ] = g_ItemMgr.GetEquipName( 601 ) ;				// 래더 부츠

					switch ( m_dwClass )
					{
						case 1 :	strShape[ Item::EquipmentPos::WEAPON_HAND1 ] = g_ItemMgr.GetEquipName( 701 ) ;	break;	// 프랙티스 소드
						case 2 :	strShape[ Item::EquipmentPos::WEAPON_HAND1 ] = g_ItemMgr.GetEquipName( 1601 ) ;	break;	// 대거
						case 3 :	strShape[ Item::EquipmentPos::WEAPON_HAND1 ] = g_ItemMgr.GetEquipName( 1501 ) ;	break;	// 스태프
						case 4 :	strShape[ Item::EquipmentPos::WEAPON_HAND1 ] = g_ItemMgr.GetEquipName( 801 ) ;	break;	// 클럽
					}

					pTempCreature->SetShape( strShape, Broadcast2nd::CNetworkEquipGrade(), true ) ;
					break;
				}
			}

			pTempModel->SetPosition( pGame->m_vecCharSelectPos[ pGame->m_dwSelectChr ] ) ;
			pTempModel->SetDirection( pGame->m_fCharSelectDir[ pGame->m_dwSelectChr ] ) ;
			pTempCreature->InitWeapon( 0 );
		}
		break;

		case 2 :	// 여자로
		{
			for ( CZ3DGeneralChrModel* pTempModel2 = RYLCreatureManager::Instance()->GetModelListHead() ;
				  pTempModel2 ;
				  pTempModel2 = RYLCreatureManager::Instance()->GetModelListNext() )
			{
				if ( pTempModel == pTempModel2 )
				{
					RYLCreatureManager::Instance()->FreeModel( pTempModel );

					pTempCreature->SetModelFileName( "PC_WOMAN.GCMDS" );
					pTempCreature->SetHairType( Temp );
					pTempCreature->SetFaceType( Temp1 );

					pTempModel2 = pTempModel = RYLCreatureManager::Instance()->AllocModel( pTempCreature );

					pTempModel->SetStaticSlot( "HAIR", Temp ) ;
					pTempModel->SetStaticSlot( "FACE", Temp1 ) ;

					strShape[ Item::EquipmentPos::SHIRT ] = g_ItemMgr.GetEquipName( m_dwShirt + 201 ) ;	// 셔츠
					strShape[ Item::EquipmentPos::BOOTS ] = g_ItemMgr.GetEquipName( 601 ) ;				// 래더 부츠

					switch ( m_dwClass )
					{
						case 1 :	strShape[ Item::EquipmentPos::WEAPON_HAND1 ] = g_ItemMgr.GetEquipName( 701 ) ;	break;	// 프랙티스 소드
						case 2 :	strShape[ Item::EquipmentPos::WEAPON_HAND1 ] = g_ItemMgr.GetEquipName( 1601 ) ;	break;	// 대거
						case 3 :	strShape[ Item::EquipmentPos::WEAPON_HAND1 ] = g_ItemMgr.GetEquipName( 1501 ) ;	break;	// 스태프
						case 4 :	strShape[ Item::EquipmentPos::WEAPON_HAND1 ] = g_ItemMgr.GetEquipName( 801 ) ;	break;	// 클럽
					}

					pTempCreature->SetShape( strShape, Broadcast2nd::CNetworkEquipGrade(), true ) ;
					break;
				}
			}

			pTempModel->SetPosition( pGame->m_vecCharSelectPos[ pGame->m_dwSelectChr ] ) ;
			pTempModel->SetDirection( pGame->m_fCharSelectDir[ pGame->m_dwSelectChr ] ) ;
			pTempCreature->InitWeapon( 0 );
		}
		break;
	}
}