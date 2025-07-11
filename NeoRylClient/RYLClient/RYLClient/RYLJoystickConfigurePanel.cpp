#include <winsock2.h>
#include "RYLJoystickConfigurePanel.h"
#include "GMUICustomDlg.h"

#include "RYLClientMain.h"
#include "RYLGameData.h"
#include "RYLNetworkData.h"
#include "RYLStringTable.h"
#include "RYLImage.h"
#include "RYLImageManager.h"
#include "GMImageDib.h"
#include "RYLImageCabinet.h"
#include "RYLSpriteEx.h"
#include "RYLSpriteList.h"

#include "RYLButton.h"
#include "RYLLabel.h"
#include "RYLCheckBox.h"
#include "RYLSceneManager.h"
#include "RYLGameScene.h"
#include "RYLStringGrid.h"

#include "InputDevice.h"

#include "InputJoystick.h"

#include <Network/ClientSocket/Send/SendLoginout.h>

CRYLJoystickConfigurePanel*	g_pJoystickConfigurePanel = NULL ;

CRYLJoystickConfigurePanel::CRYLJoystickConfigurePanel( CGMUICustomDlg *pParentDlg ) : CGMUICustomPanel( pParentDlg )
{
	m_nKeyIndex				= -1;
	m_cPage					= 0;
	m_dwTempKey				= 0;
	m_cForceFeedback		= 1;
	m_pForceFeedBack		= NULL;
	m_pForceFeedBackLable	= NULL;
	m_pKeySetup				= NULL;
	m_pKeyInit				= NULL;
	m_pKeyCancel			= NULL;
	m_pKeyIuput				= NULL;
	m_pPrevButton			= NULL;
	m_pNextButton			= NULL;
	m_pKeyLabel				= NULL;
	m_pDefaultKeyLabel1		= NULL;
	m_pDefaultKeyLabel2		= NULL;
	m_pSetupKeyLabel		= NULL;
	m_pKeyStringGrid		= NULL;
	m_pKeyDescLable			= NULL;
	m_pKeyDefaultDescLable	= NULL;
	m_pKeySetupDescLabel	= NULL;
	m_cInputMode			= KeyControl::JOY_INPUT_PLAYMODE;

	RECT	rt ;
	INT		iTempWidth = 0;

	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;

	// Background Image
	CRYLSpriteList::Instance()->CreateGMImage( 448, 370, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;

	CGMImageDib* pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 );

	for ( int i = 0 ; i < 72 ; i ++ )
	{
		iTempWidth = i * 5 ;

		SetRect( &rt, 0, 38, 50, 43) ;
		pImageDIB->DrawRect( &rt, 0, iTempWidth, pSprite) ;
		
		SetRect( &rt, 50, 38, 150, 43 ) ;
		pImageDIB->DrawRect( &rt, 50,  iTempWidth, pSprite ) ;			
		pImageDIB->DrawRect( &rt, 150, iTempWidth, pSprite ) ;
		pImageDIB->DrawRect( &rt, 250, iTempWidth, pSprite ) ;

		SetRect( &rt, 50, 38, 86, 43 ) ;
		pImageDIB->DrawRect( &rt, 350, iTempWidth, pSprite ) ;
		
		SetRect( &rt, 194, 38, 256, 43 ) ;
		pImageDIB->DrawRect( &rt, 386, iTempWidth, pSprite ) ;
	}
	
	SetRect( &rt, 0, 42, 50, 52) ;
	pImageDIB->DrawRect( &rt, 0, iTempWidth, pSprite) ;
	
	SetRect( &rt, 50, 42, 150, 52 ) ;	
	pImageDIB->DrawRect( &rt, 50,  iTempWidth, pSprite ) ;			
	pImageDIB->DrawRect( &rt, 150, iTempWidth, pSprite ) ;
	pImageDIB->DrawRect( &rt, 250, iTempWidth, pSprite ) ;
	
	SetRect( &rt, 50, 42, 86, 52 ) ;
	pImageDIB->DrawRect( &rt, 350, iTempWidth, pSprite ) ;
	
	SetRect( &rt, 194, 42, 256, 52 ) ;
	pImageDIB->DrawRect( &rt, 386, iTempWidth, pSprite ) ;
	
	pImageDIB->MakeFrameRect( 6, 18, 435, 324, 0xffB99E6B, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect( 15, 28, 414, 197, 0xffB99E6B, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect( 20, 48, 404,  1, 0xff959595, 0xc0000000,  1 ) ;

	pImageDIB->MakeFrameRect( 15, 250, 414, 80, 0xffB99E6B, 0xc0000000,  1 ) ;

	pImageDIB->MakeFrameRect( 175, 31, 1, 191, 0xff959595, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect( 290, 31, 1, 191, 0xff959595, 0xc0000000,  1 ) ;	
	
	pImageDIB->MakeFrameRect( 23, 260, 140, 25, 0xff959595, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect( 170, 260, 108, 25, 0xff959595, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect( 23, 295, 255, 25, 0xff959595, 0xc0000000,  1 ) ;

	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	AttachPanelImage( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;

	SetWidth( 448 ) ;
	SetHeight( 370 ) ;

	m_pGMUIParentDlg = pParentDlg ;
	g_pJoystickConfigurePanel = this ;

	memset(m_bPressKey, false, sizeof(bool)*MAX_KEY_COUNT);

	memset(m_cAllocKey, 0, MAX_KEY_COUNT);

	memset(m_dwDefaultKey, 0, sizeof(unsigned long)*MAX_KEY_COUNT);	

	m_dwDefaultKey[KeyControl::KEY_COLUM_FORWARD]		= KeyControl::KEY_UP;
	m_dwDefaultKey[KeyControl::KEY_COLUM_BACKWARD]		= KeyControl::KEY_DOWN;
	m_dwDefaultKey[KeyControl::KEY_COLUM_LEFT]			= KeyControl::KEY_LEFT;
	m_dwDefaultKey[KeyControl::KEY_COLUM_RIGHT]			= KeyControl::KEY_RIGHT;

	m_dwDefaultKey[KeyControl::KEY_COLUM_ZOOMIN]		= KeyControl::KEY_ZROT_UP | KeyControl::KEY_BTN_12;
	m_dwDefaultKey[KeyControl::KEY_COLUM_ZOOMOUT]		= KeyControl::KEY_ZROT_DOWN | KeyControl::KEY_BTN_12;

	m_dwDefaultKey[KeyControl::KEY_COLUM_DIRECTION_LEFT]	= KeyControl::KEY_Z_UP;
	m_dwDefaultKey[KeyControl::KEY_COLUM_DIRECTION_RIGHT]	= KeyControl::KEY_Z_DOWN;
	m_dwDefaultKey[KeyControl::KEY_COLUM_DIRECTION_UP]		= KeyControl::KEY_ZROT_UP;
	m_dwDefaultKey[KeyControl::KEY_COLUM_DIRECTION_DOWN]	= KeyControl::KEY_ZROT_DOWN;

	m_dwDefaultKey[KeyControl::KEY_COLUM_WEAPONCHANGE]	= KeyControl::KEY_BTN_9;

	m_dwDefaultKey[KeyControl::KEY_COLUM_PREV]			= KeyControl::KEY_POV_LEFT,
	m_dwDefaultKey[KeyControl::KEY_COLUM_NEXT]			= KeyControl::KEY_POV_RIGHT,
		
	m_dwDefaultKey[KeyControl::KEY_COLUM_01]			= KeyControl::KEY_BTN_7;
	m_dwDefaultKey[KeyControl::KEY_COLUM_02]			= KeyControl::KEY_BTN_8;

	m_dwDefaultKey[KeyControl::KEY_COLUM_ATTACK]		= KeyControl::KEY_BTN_2;
	m_dwDefaultKey[KeyControl::KEY_COLUM_CASTING]		= KeyControl::KEY_BTN_3;
	
	m_dwDefaultKey[KeyControl::KEY_COLUM_ITEM]			= KeyControl::KEY_BTN_4;
	m_dwDefaultKey[KeyControl::KEY_COLUM_JUMP]			= KeyControl::KEY_BTN_1;
	m_dwDefaultKey[KeyControl::KEY_COLUM_REST]			= KeyControl::KEY_BTN_10;	

	m_dwDefaultKey[KeyControl::KEY_COLUM_SELFTARGET]	= KeyControl::KEY_POV_UP;

	memcpy(m_dwSettingKey, m_dwDefaultKey, sizeof(unsigned long)*MAX_KEY_COUNT);
}

CRYLJoystickConfigurePanel::~CRYLJoystickConfigurePanel() 
{
	FinalCtrl() ;
}


VOID	CRYLJoystickConfigurePanel::InitCtrl() 
{
	RECT rt ;

	CGMImageDib* pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ;
	
	// 키 정보 라벨.
	m_pKeyDescLable = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pKeyDescLable->SetFontColor (0xffffffff);
	SetRect( &rt, 32, 267, 152, 279) ;
	m_pKeyDescLable->SetAlignRect( rt ) ;
	m_pKeyDescLable->SetAlign( CENTER ) ;
	m_pKeyDescLable->SetAutoSize( TRUE ) ;	
	m_pKeyDescLable->SetVisible(FALSE);
	CGMUICustomPanel::AddChild( m_pKeyDescLable ) ;

	// 기본키 정보 라벨.
	m_pKeyDefaultDescLable = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pKeyDefaultDescLable->SetFontColor (0xffffffff);
	SetRect( &rt, 173, 267, 276, 279) ;
	m_pKeyDefaultDescLable->SetAlignRect( rt ) ;
	m_pKeyDefaultDescLable->SetAlign( CENTER ) ;	
	m_pKeyDefaultDescLable->SetAutoSize( TRUE ) ;	
	m_pKeyDefaultDescLable->SetVisible(FALSE);
	CGMUICustomPanel::AddChild( m_pKeyDefaultDescLable ) ;

	// 키 임시 설정 정보 라벨.
	m_pKeySetupDescLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pKeySetupDescLabel->SetFontColor (0xffffffff);
	SetRect( &rt, 32, 304, 276, 316) ;
	m_pKeySetupDescLabel->SetAlignRect( rt ) ;
	m_pKeySetupDescLabel->SetAlign( CENTER ) ;	
	m_pKeySetupDescLabel->SetAutoSize( TRUE ) ;	
	m_pKeySetupDescLabel->SetVisible(FALSE);
	CGMUICustomPanel::AddChild( m_pKeySetupDescLabel ) ;

	// 조작 라벨.
	m_pDefaultKeyLabel1 = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pDefaultKeyLabel1->SetFontColor (0xffffffff);
	SetRect( &rt, 85, 33, 120, 45) ;
	m_pDefaultKeyLabel1->SetAlignRect( rt ) ;
	m_pDefaultKeyLabel1->SetAlign( LEFT ) ;
	m_pDefaultKeyLabel1->SetCaption( CRYLStringTable::m_strString[ 4126 ] ) ;
	m_pDefaultKeyLabel1->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pDefaultKeyLabel1 ) ;

	// 기본 라벨.
	m_pDefaultKeyLabel2 = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pDefaultKeyLabel2->SetFontColor (0xffffffff);
	SetRect( &rt, 222, 33, 220, 45) ;
	m_pDefaultKeyLabel2->SetAlignRect( rt ) ;
	m_pDefaultKeyLabel2->SetAlign( LEFT ) ;
	m_pDefaultKeyLabel2->SetCaption( CRYLStringTable::m_strString[ 4127 ] ) ;
	m_pDefaultKeyLabel2->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pDefaultKeyLabel2 ) ;

	// 설정 라벨
	m_pSetupKeyLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pSetupKeyLabel->SetFontColor (0xffffffff);
	SetRect( &rt, 344, 33, 320, 45) ;
	m_pSetupKeyLabel->SetAlignRect( rt ) ;
	m_pSetupKeyLabel->SetAlign( LEFT ) ;
	m_pSetupKeyLabel->SetCaption( CRYLStringTable::m_strString[ 4128 ] ) ;
	m_pSetupKeyLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pSetupKeyLabel ) ;

	// 진동체크 라벨
	m_pForceFeedBackLable = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pForceFeedBackLable->SetFontColor (0xffffffff);
	SetRect( &rt, 45, 232, 54, 244) ;
	m_pForceFeedBackLable->SetAlignRect( rt ) ;
	m_pForceFeedBackLable->SetAlign( LEFT ) ;
	m_pForceFeedBackLable->SetCaption( CRYLStringTable::m_strString[ 4123 ] ) ;
	m_pForceFeedBackLable->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pForceFeedBackLable ) ;

	// 진동 체크
	m_pForceFeedBack = new CRYLCheckBox( m_pGMUIParentDlg ) ;
	m_pForceFeedBack->SetLeft( 20 ) ;
	m_pForceFeedBack->SetTop( 230 ) ;
	m_pForceFeedBack->SetInstance( RYLJOYSTICKCONFIGUREPANEL::CHECK_BOX ) ;
	m_pForceFeedBack->OnClickEvent = DlgClick ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pForceFeedBack->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pForceFeedBack->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 181, 238, 197, 254 ) ;
	m_pForceFeedBack->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 181, 220, 197, 236 ) ;
	m_pForceFeedBack->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	SetRect( &rt, 163, 238, 179, 254 ) ;
	m_pForceFeedBack->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_CHECK_DISIBLE ) ;
	SetRect( &rt, 163, 220, 179, 236 ) ;
	m_pForceFeedBack->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NCHECK_DISIBLE ) ;
	m_pForceFeedBack->SetChecked( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pForceFeedBack ) ;

	// 이전 버튼.
	m_pPrevButton = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pPrevButton->SetLeft( 400 ) ;
	m_pPrevButton->SetTop( 232) ;
	m_pPrevButton->OnClickEvent = DlgClick ;
	m_pPrevButton->SetInstance( RYLJOYSTICKCONFIGUREPANEL::PREV_BUTTON ) ;
	SetRect( &rt, 217, 215, 230, 227 ) ;
	m_pPrevButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 217, 243, 230, 255 ) ;
	m_pPrevButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 217, 229, 230, 241 ) ;
	m_pPrevButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 217, 215, 230, 227 ) ;
	m_pPrevButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	CGMUICustomPanel::AddChild( m_pPrevButton ) ;

	// 다음 버튼.
	m_pNextButton = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pNextButton->SetLeft( 416 ) ;
	m_pNextButton->SetTop( 232) ;
	m_pNextButton->OnClickEvent = DlgClick ;
	m_pNextButton->SetInstance( RYLJOYSTICKCONFIGUREPANEL::NEXT_BUTTON ) ;
	SetRect( &rt, 232, 215, 245, 227 ) ;
	m_pNextButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 232, 243, 245, 255 ) ;
	m_pNextButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 232, 229, 245, 241 ) ;
	m_pNextButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 232, 215, 245, 227 ) ;
	m_pNextButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	CGMUICustomPanel::AddChild( m_pNextButton ) ;

	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;

	pSprite	= CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;	

	// 키 설정.
	m_pKeySetup = new CRYLButton(m_pGMUIParentDlg) ;
	m_pKeySetup->SetLeft(284) ;
	m_pKeySetup->SetTop(295);
	m_pKeySetup->OnClickEvent = DlgClick ;
	m_pKeySetup->SetEnable(FALSE);
	m_pKeySetup->SetInstance(RYLJOYSTICKCONFIGUREPANEL::KEYSETUP_BUTTON) ;
	m_pKeySetup->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pKeySetup->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pKeySetup->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pKeySetup->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	CGMUICustomPanel::AddChild( m_pKeySetup ) ;
	m_pKeySetup->SetCaption(CRYLStringTable::m_strString[4124]);	

	// 전체 초괴화.
	m_pKeyInit = new CRYLButton(m_pGMUIParentDlg) ;
	m_pKeyInit->SetLeft(356) ;
	m_pKeyInit->SetTop(295);
	m_pKeyInit->OnClickEvent = DlgClick ;
	m_pKeyInit->SetInstance(RYLJOYSTICKCONFIGUREPANEL::KEYINIT_BUTTON) ;
	m_pKeyInit->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pKeyInit->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pKeyInit->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pKeyInit->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	CGMUICustomPanel::AddChild( m_pKeyInit ) ;
	m_pKeyInit->SetCaption(CRYLStringTable::m_strString[4125]);		

	// 취소.
	m_pKeyCancel = new CRYLButton(m_pGMUIParentDlg) ;
	m_pKeyCancel->SetLeft(356) ;
	m_pKeyCancel->SetTop(260);
	m_pKeyCancel->OnClickEvent = DlgClick ;
	m_pKeyCancel->SetEnable(FALSE);
	m_pKeyCancel->SetInstance(RYLJOYSTICKCONFIGUREPANEL::KEYCANLE_BUTTON) ;
	m_pKeyCancel->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pKeyCancel->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pKeyCancel->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pKeyCancel->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	CGMUICustomPanel::AddChild( m_pKeyCancel ) ;
	m_pKeyCancel->SetCaption(CRYLStringTable::m_strString[4129]);		

	// 입력.
	m_pKeyIuput = new CRYLButton(m_pGMUIParentDlg) ;
	m_pKeyIuput->SetLeft(284) ;
	m_pKeyIuput->SetTop(260);
	m_pKeyIuput->OnClickEvent = DlgClick ;
	m_pKeyIuput->SetEnable(FALSE);
	m_pKeyIuput->SetInstance(RYLJOYSTICKCONFIGUREPANEL::KEYINPUT_BUTTON) ;
	m_pKeyIuput->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pKeyIuput->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pKeyIuput->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pKeyIuput->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	CGMUICustomPanel::AddChild( m_pKeyIuput ) ;
	m_pKeyIuput->SetCaption(CRYLStringTable::m_strString[4130]);			

	// 키 그리드.
	m_pKeyStringGrid = new CRYLStringGrid( m_pGMUIParentDlg, FALSE ) ;
	m_pKeyStringGrid->SetLeft( 20 ) ;
	m_pKeyStringGrid->SetTop( 59 ) ;
	m_pKeyStringGrid->SetWidth( 420 ) ;
	m_pKeyStringGrid->SetHeight( 170 ) ;
	m_pKeyStringGrid->SetColCount( 3 ) ;
	m_pKeyStringGrid->SetItemHeight( 16 ) ;
	m_pKeyStringGrid->SetFontColor( 0xffffffff ) ;
	m_pKeyStringGrid->SetSelectedItemColor( 0xFFC9FFC5 ) ;
	m_pKeyStringGrid->SetInstance( RYLJOYSTICKCONFIGUREPANEL::KEY_STRINGGRID) ;
	m_pKeyStringGrid->OnClickEvent = DlgClick ;	
	CGMUICustomPanel::AddChild( m_pKeyStringGrid ) ;			

	UpdateKeyPage();
}

VOID	CRYLJoystickConfigurePanel::FinalCtrl() 
{
	GM_DELETE( m_pForceFeedBackLable ) ;
	GM_DELETE( m_pForceFeedBack ) ;

	GM_DELETE( m_pKeySetup ) ;
	GM_DELETE( m_pKeyInit ) ;

	GM_DELETE( m_pKeyLabel ) ;
	GM_DELETE( m_pDefaultKeyLabel1 ) ;
	GM_DELETE( m_pDefaultKeyLabel2 ) ;
	GM_DELETE( m_pSetupKeyLabel ) ;	

	GM_DELETE( m_pKeyCancel ) ;
	GM_DELETE( m_pKeyIuput ) ;	

	GM_DELETE( m_pPrevButton ) ;
	GM_DELETE( m_pNextButton ) ;

	GM_DELETE( m_pKeyStringGrid ) ;	

	GM_DELETE( m_pKeyDescLable) ;
	GM_DELETE( m_pKeyDefaultDescLable) ;
	GM_DELETE( m_pKeySetupDescLabel) ;
}

HRESULT	CRYLJoystickConfigurePanel::Update( BOOL &bClick, BOOL &bEdge )
{	
	if (m_dwTempKey)
	{				
		unsigned long dwKeyValues	= 0x00000001;
		unsigned char cChk			= 0;
		char szInfo[0xff] = {0, };

		for (unsigned char cJoyIndex = 0; cJoyIndex < MAX_JOY_KEY_COUNT; cJoyIndex++)
		{
			if (m_dwTempKey & dwKeyValues)
			{
				if (cChk==1)
				{
					strcat(szInfo, ", ");
					strcat(szInfo, CRYLStringTable::m_strString[4197+cJoyIndex]);
					break;
				}
				else
				{
					strcpy(szInfo, CRYLStringTable::m_strString[4197+cJoyIndex]);
					cChk++;
				}
			}

			dwKeyValues = dwKeyValues << 1;
		}

		m_pKeySetupDescLabel->SetCaption(szInfo);
		m_pKeySetupDescLabel->SetVisible(TRUE);

	}
	else
	{
		m_pKeySetupDescLabel->SetVisible(FALSE);
	}

	return S_OK ;	
}

HRESULT	CRYLJoystickConfigurePanel::Render( )  
{

	return S_OK ;
}

HRESULT	CRYLJoystickConfigurePanel::FinalRender( ) 
{
	return S_OK ;
}

VOID CRYLJoystickConfigurePanel::UpdateNextKeyPage()
{
	if (m_cPage < MAX_KEY_PAGE)
	{
		m_cPage += 1;
		UpdateKeyPage();
		SetKeyIndex();
	}
}

VOID CRYLJoystickConfigurePanel::UpdatePrevKeyPage()
{
	if (m_cPage > 0)
	{
		m_cPage -= 1;
		UpdateKeyPage();		
		SetKeyIndex();
	}
}

VOID CRYLJoystickConfigurePanel::UpdateKeyPage()
{
	m_pKeyStringGrid->ClearItem();

	unsigned long dwColor = D3DCOLOR_RGBA( 255, 255, 255, 255);	

	for (unsigned char cIndex = 0 ; cIndex < MAX_GIRD_COUNT; cIndex++)
	{
		if (m_cPage*MAX_GIRD_COUNT+cIndex >= MAX_KEY_COUNT )
		{
			break;
		}

		m_pKeyStringGrid->AddItem( 0, CRYLStringTable::m_strString[4131+m_cPage*MAX_GIRD_COUNT+cIndex], 150, dwColor) ;
		m_pKeyStringGrid->AddItem( 1, CRYLStringTable::m_strString[4164+m_cPage*MAX_GIRD_COUNT+cIndex], 120, dwColor) ;

		char szInfo[0xff] = {0, };

		unsigned long dwKey			= m_dwSettingKey[m_cPage*MAX_GIRD_COUNT+cIndex];
		unsigned long dwKeyValues	= 0x00000001;
		unsigned char cChk			= 0;

		for (unsigned char cJoyIndex = 0; cJoyIndex < MAX_JOY_KEY_COUNT; cJoyIndex++)
		{
			if (dwKey & dwKeyValues)
			{
				if (cChk==1)
				{
					strcat(szInfo, ", ");
					strcat(szInfo, CRYLStringTable::m_strString[4197+cJoyIndex]);
					break;
				}
				else
				{
					strcpy(szInfo, CRYLStringTable::m_strString[4197+cJoyIndex]);
					cChk++;
				}
			}

			dwKeyValues = dwKeyValues << 1;
		}

		m_pKeyStringGrid->AddItem( 2, szInfo, 130, dwColor) ;		
	}
}

VOID CRYLJoystickConfigurePanel::SetKeyIndex()
{
	int nKeyIndex = m_pKeyStringGrid->GetItemIndex();

	if (nKeyIndex > -1)
	{
		m_nKeyIndex = m_cPage*MAX_GIRD_COUNT + nKeyIndex ;

		m_pKeyDescLable->SetVisible(TRUE);			
		m_pKeyDescLable->SetCaption(CRYLStringTable::m_strString[4131+m_nKeyIndex]);

		m_pKeyDefaultDescLable->SetVisible(TRUE);
		m_pKeyDefaultDescLable->SetCaption(CRYLStringTable::m_strString[4164+m_nKeyIndex]);			

		m_pKeyCancel->SetEnable(TRUE);
		m_pKeyIuput->SetEnable(TRUE);
	}
	else
	{
		m_nKeyIndex = -1;
		
		m_pKeyDescLable->SetVisible(FALSE);			
		m_pKeyDefaultDescLable->SetVisible(FALSE);

		m_pKeyCancel->SetEnable(FALSE);
		m_pKeyIuput->SetEnable(FALSE);

		SetInput(KeyControl::JOY_INPUT_PLAYMODE);
	}
}

unsigned char CRYLJoystickConfigurePanel::GetForcefeedback(void)
{
	CRYLGameData* pGame = CRYLGameData::Instance() ;

	if (pGame->m_bMouseMode)	
	{
		return 0;
	}

	return m_cForceFeedback;
}

VOID CRYLJoystickConfigurePanel::SetForceFeedback(unsigned char cForceFeedBack)
{
	m_pForceFeedBack->SetChecked(cForceFeedBack);
	
	m_cForceFeedback = cForceFeedBack;
}

VOID CRYLJoystickConfigurePanel::ChangeForceFeedback()
{
	BOOL bCheck = !m_pForceFeedBack->GetChecked();

	m_pForceFeedBack->SetChecked(bCheck);

	m_cForceFeedback = static_cast<unsigned char>(bCheck);

	// 서버로 패킷을 보낸다 //
	unsigned long dwKeyInfo[MAX_KEY_COUNT+1] = {0, };
	memcpy(dwKeyInfo, m_dwSettingKey, sizeof(unsigned long)*MAX_KEY_COUNT);
	dwKeyInfo[MAX_KEY_COUNT] = m_cForceFeedback;

	SendPacket::KeyInfo(g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, dwKeyInfo);
}

VOID CRYLJoystickConfigurePanel::SetInitialize()
{
	SetKeyIndex();

	SetInput(KeyControl::JOY_INPUT_PLAYMODE);
}

VOID CRYLJoystickConfigurePanel::AllInitialize()
{
	memcpy(m_dwSettingKey, m_dwDefaultKey, sizeof(unsigned long)*MAX_KEY_COUNT);

	UpdateKeyPage();

	SetKeyIndex();

	SetInput(KeyControl::JOY_INPUT_PLAYMODE);

	// 서버로 패킷을 보낸다 //
	unsigned long dwKeyInfo[MAX_KEY_COUNT+1] = {0, };

	memcpy(dwKeyInfo, m_dwSettingKey, sizeof(unsigned long)*MAX_KEY_COUNT);
	dwKeyInfo[MAX_KEY_COUNT] = m_cForceFeedback;

	SendPacket::KeyInfo(g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, dwKeyInfo);
}

unsigned long CRYLJoystickConfigurePanel::GetJoyKey(unsigned char cKeyColumn)
{
	if (MAX_KEY_COUNT <= cKeyColumn)
	{
		return 0;
	}

	return m_dwSettingKey[cKeyColumn];
}

VOID CRYLJoystickConfigurePanel::SetInput(unsigned char cInputMode)
{
	m_dwTempKey		= 0;
	m_cCount		= 0;
	m_cInputMode	= cInputMode;

	if (cInputMode==KeyControl::JOY_INPUT_CHECKMODE)
	{
		m_pKeySetup->SetEnable(TRUE);
	}
	else if (cInputMode==KeyControl::JOY_INPUT_PLAYMODE)
	{
		m_pKeySetup->SetEnable(FALSE);
	}
}

VOID CRYLJoystickConfigurePanel::SetCount(unsigned char cCount)
{
	m_cCount = cCount;
}

VOID CRYLJoystickConfigurePanel::SetTempKey(unsigned long dwKey)
{
	m_dwTempKey = dwKey;
}

VOID CRYLJoystickConfigurePanel::SetInitializeKey()
{
	memset(m_cAllocKey, 0, MAX_KEY_COUNT);
}

VOID CRYLJoystickConfigurePanel::SetAllocKey(unsigned char cIndex)
{
	m_cAllocKey[cIndex] = 1;
}

VOID CRYLJoystickConfigurePanel::SetPressKey(unsigned char cIndex, bool bPressKey)
{
	m_bPressKey[cIndex] = bPressKey;
}

VOID CRYLJoystickConfigurePanel::SetJoyKey()
{
	if (m_nKeyIndex!=-1)
	{
		for (unsigned char cIndex = 0; cIndex < MAX_KEY_COUNT; cIndex++)
		{
			if (m_dwSettingKey[cIndex]==m_dwTempKey)
			{
				m_dwSettingKey[cIndex] = 0;
			}
		}

		m_dwSettingKey[m_nKeyIndex] = m_dwTempKey;

		m_dwTempKey = 0;

		SetInput(KeyControl::JOY_INPUT_PLAYMODE);
		UpdateKeyPage();		
		SetKeyIndex();

		// 서버로 패킷을 보낸다 //
		unsigned long dwKeyInfo[MAX_KEY_COUNT+1] = {0, };

		memcpy(dwKeyInfo, m_dwSettingKey, sizeof(unsigned long)*MAX_KEY_COUNT);
		dwKeyInfo[MAX_KEY_COUNT] = m_cForceFeedback;

		SendPacket::KeyInfo(g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, dwKeyInfo);
	}
}

VOID CRYLJoystickConfigurePanel::DlgClick( CGMUIObject* Sender, INT X, INT Y ) 
{
	if ( !g_pJoystickConfigurePanel )
		return ;

	switch ( Sender->GetInstance() )
	{
	case RYLJOYSTICKCONFIGUREPANEL::PREV_BUTTON:		// 이전.
		g_pJoystickConfigurePanel->UpdatePrevKeyPage();
		break;
	case RYLJOYSTICKCONFIGUREPANEL::NEXT_BUTTON:		// 다음.
		g_pJoystickConfigurePanel->UpdateNextKeyPage();
		break;
	case RYLJOYSTICKCONFIGUREPANEL::KEY_STRINGGRID:
		g_pJoystickConfigurePanel->SetKeyIndex();
		break;
	case RYLJOYSTICKCONFIGUREPANEL::CHECK_BOX:			// 진동 체크.
		g_pJoystickConfigurePanel->ChangeForceFeedback();
		break;
	case RYLJOYSTICKCONFIGUREPANEL::KEYINIT_BUTTON:		// 전체 초기화.
		g_pJoystickConfigurePanel->AllInitialize();		
		break;
	case RYLJOYSTICKCONFIGUREPANEL::KEYINPUT_BUTTON:
		g_pJoystickConfigurePanel->SetInput(KeyControl::JOY_INPUT_CHECKMODE);
		break;
	case RYLJOYSTICKCONFIGUREPANEL::KEYCANLE_BUTTON:
		g_pJoystickConfigurePanel->SetInput(KeyControl::JOY_INPUT_PLAYMODE);
		break;
	case RYLJOYSTICKCONFIGUREPANEL::KEYSETUP_BUTTON:
		g_pJoystickConfigurePanel->SetJoyKey();
		break;

	}
}

namespace KeyControl	{

void Process()
{
	g_pJoystickConfigurePanel->SetInitializeKey();

	CInputDevice::GetInstance()->Process();

	if(g_pJoystickConfigurePanel->GetInputMode()==JOY_INPUT_CHECKMODE)
	{
		unsigned char cCount	= 0;
		unsigned long dwKey		= CheckKeyState(cCount);

		if (dwKey)
		{
			unsigned char cTempCount = g_pJoystickConfigurePanel->GetCount();

			if (!cTempCount || cCount==2)
			{
				g_pJoystickConfigurePanel->SetTempKey(dwKey);
				g_pJoystickConfigurePanel->SetCount(cCount);
			}
			else
			{
				unsigned long dwTempKey = g_pJoystickConfigurePanel->GetTempKey();

				if (dwTempKey&dwKey)
				{
					g_pJoystickConfigurePanel->SetTempKey(dwTempKey | dwKey);
					g_pJoystickConfigurePanel->SetCount(2);
				}
				else
				{
					g_pJoystickConfigurePanel->SetTempKey(dwKey);
					g_pJoystickConfigurePanel->SetCount(cCount);
				}
			}
		}		
	}
}

bool GetCheckInputJoystick(void)
{
	CInputJoystick* pInputJoystick = CInputJoystick::GetInstance();

	return pInputJoystick->GetCheckInputJoystick();
}

unsigned long CheckKeyState(unsigned char & cCount)
{
	unsigned long dwKeyValue	= 0x00000001;
	unsigned long dwTempKey		= 0x00000000;

	CInputJoystick* pInputJoystick = CInputJoystick::GetInstance();

	for (unsigned char cJoyIndex = 0; cJoyIndex < CRYLJoystickConfigurePanel::MAX_JOY_KEY_COUNT; cJoyIndex++)
	{	
		switch(dwKeyValue)
		{
		case KEY_UP:
			if (pInputJoystick->GetY() < 0)
			{
				dwTempKey |= dwKeyValue;
				cCount++;
			}
			break;
		case KEY_DOWN:
			if (pInputJoystick->GetY() > 0)
			{
				dwTempKey |= dwKeyValue;
				cCount++;
			}
			break;
		case KEY_LEFT:
			if (pInputJoystick->GetX() < 0)
			{
				dwTempKey |= dwKeyValue;
				cCount++;
			}
			break;
		case KEY_RIGHT:
			if (pInputJoystick->GetX() > 0)
			{
				dwTempKey |= dwKeyValue;
				cCount++;
			}
			break;
		case KEY_POV_UP:
			if ( pInputJoystick->GetPOV() == CInputJoystick::POV_UP)
			{
				dwTempKey |= dwKeyValue;
				cCount++;
			}				
			break;
		case KEY_POV_DOWN:
			if (pInputJoystick->GetPOV() == CInputJoystick::POV_DOWN)
			{
				dwTempKey |= dwKeyValue;
				cCount++;
			}
			break;
		case KEY_POV_LEFT:
			if (pInputJoystick->GetPOV() == CInputJoystick::POV_LEFT)
			{
				dwTempKey |= dwKeyValue;
				cCount++;
			}
			break;
		case KEY_POV_RIGHT:
			if (pInputJoystick->GetPOV() == CInputJoystick::POV_RIGHT)
			{
				dwTempKey |= dwKeyValue;
				cCount++;
			}
			break;

		case KEY_Z_UP:
			if (pInputJoystick->GetZ() < 0)
			{
				dwTempKey |= dwKeyValue;
				cCount++;
			}
			break;
		case KEY_Z_DOWN:
			if (pInputJoystick->GetZ() > 0)
			{
				dwTempKey |= dwKeyValue;
				cCount++;
			}
			break;
		case KEY_ZROT_UP:
			if (pInputJoystick->GetRotationZ() > 0)
			{
				dwTempKey |= dwKeyValue;
				cCount++;
			}
			break;
		case KEY_ZROT_DOWN:
			if (pInputJoystick->GetRotationZ() < 0)
			{
				dwTempKey |= dwKeyValue;
				cCount++;
			}
			break;
		case KEY_BTN_1:
		case KEY_BTN_2:
		case KEY_BTN_3:
		case KEY_BTN_4:
		case KEY_BTN_5:
		case KEY_BTN_6:
		case KEY_BTN_7:
		case KEY_BTN_8:
		case KEY_BTN_9:
		case KEY_BTN_10:
		case KEY_BTN_11:
		case KEY_BTN_12:
			{
				if (pInputJoystick->GetJoyButton(cJoyIndex-START_KEY_BUTTON))
				{
					dwTempKey |= dwKeyValue;
					cCount++;
				}					
			}
			break;
		}

		dwKeyValue = dwKeyValue << 1;

		if (cCount==2)
		{
			break;
		}
	}

	return dwTempKey ;
}

bool IsKeyState(unsigned char cKeyColumn)
{
	unsigned long dwKey			= g_pJoystickConfigurePanel->GetJoyKey(cKeyColumn);
	unsigned long dwKeyValue	= 0x00000001;
	
	if(g_pJoystickConfigurePanel->GetInputMode()!=JOY_INPUT_PLAYMODE)
	{
		return false;
	}

	if (!dwKey)
	{
		return false;
	}

	CInputJoystick* pInputJoystick = CInputJoystick::GetInstance();

	for (unsigned char cJoyIndex = 0; cJoyIndex < CRYLJoystickConfigurePanel::MAX_JOY_KEY_COUNT; cJoyIndex++)
	{
		if (dwKey & dwKeyValue)
		{	
			switch(dwKeyValue)
			{
			case KEY_UP:
				if (pInputJoystick->GetY() >= 0)
				{
					return false;
				}
				break;
			case KEY_DOWN:
				if (pInputJoystick->GetY() <= 0)
				{
					return false;
				}
				break;
			case KEY_LEFT:
				if (pInputJoystick->GetX() >= 0)
				{
					return false;
				}
				break;
			case KEY_RIGHT:
				if (pInputJoystick->GetX() <= 0)
				{
					return false;
				}
				break;
			case KEY_POV_UP:
				if ( pInputJoystick->GetPOV() != CInputJoystick::POV_UP)
				{
					return false;
				}				
				break;
			case KEY_POV_DOWN:
				if (pInputJoystick->GetPOV() != CInputJoystick::POV_DOWN)
				{
					return false;
				}
				break;
			case KEY_POV_LEFT:
				if (pInputJoystick->GetPOV() != CInputJoystick::POV_LEFT)
				{
					return false;
						
				}
				break;
			case KEY_POV_RIGHT:
				if (pInputJoystick->GetPOV() != CInputJoystick::POV_RIGHT)
				{
					return false;
				}
				break;			
			case KEY_Z_UP:
				if (pInputJoystick->GetZ() >= 0)
				{
					return false;
				}
				break;
			case KEY_Z_DOWN:
				if (pInputJoystick->GetZ() <= 0)
				{
					return false;
				}
				break;
			case KEY_ZROT_UP:
				if (pInputJoystick->GetRotationZ() <= 0)
				{
					return false;
				}
				break;
			case KEY_ZROT_DOWN:
				if (pInputJoystick->GetRotationZ() >= 0)
				{
					return false;
				}
				break;
			case KEY_BTN_1:
			case KEY_BTN_2:
			case KEY_BTN_3:
			case KEY_BTN_4:
			case KEY_BTN_5:
			case KEY_BTN_6:
			case KEY_BTN_7:
			case KEY_BTN_8:
			case KEY_BTN_9:
			case KEY_BTN_10:
			case KEY_BTN_11:
			case KEY_BTN_12:
				{
					if (!pInputJoystick->GetJoyButton(cJoyIndex-START_KEY_BUTTON))
					{
						return false;
					}					
				}
				break;
			}
		}

		dwKeyValue = dwKeyValue << 1;
	}
	
	for (unsigned char cJoyIndex = 0; cJoyIndex < CRYLJoystickConfigurePanel::MAX_JOY_KEY_COUNT; cJoyIndex++)
	{
		if (dwKey & dwKeyValue && dwKey!=dwKeyValue)
		{
			unsigned char cAllocKey = g_pJoystickConfigurePanel->GetAllocKey(cKeyColumn);

			if (cAllocKey)
			{
				return false;
			}
		}		
	}

	g_pJoystickConfigurePanel->SetAllocKey(cKeyColumn);

	return true;
}

bool IsKeyPress(unsigned char cKeyColumn)
{
	bool bKeyState = IsKeyState(cKeyColumn);
	bool bKeyPress = g_pJoystickConfigurePanel->GetPressKey(cKeyColumn);

	if(bKeyState != bKeyPress)
	{
		g_pJoystickConfigurePanel->SetPressKey(cKeyColumn, bKeyState);
		return bKeyState;
	}

	return false;
}

void StartForceFeedback(long lForceX, long lForceY, unsigned long dwTick)
{
	CInputJoystick* pInputJoystick = CInputJoystick::GetInstance();

	pInputJoystick->StartForceFeedback(lForceX, lForceY, dwTick);
}

};