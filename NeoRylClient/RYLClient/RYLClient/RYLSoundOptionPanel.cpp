#include <winsock2.h>
#include "RYLSoundOptionPanel.h"
#include "RYLLabel.h"
#include "RYLCheckBox.h"
#include "RYLButton.h"
#include "RYLProgressBar.h"
#include "RYLStringTable.h"
#include "RYLSpriteList.h"
#include "RYLImageCabinet.h"
#include "GMImageDib.h"
#include "RYLImage.h"
#include "RYLImageManager.h"
#include "RYLGameData.h"
#include "RYLOptionDlg.h"
#include "SoundMgr.h"
#include "BgmManager.h"
#include "AmbienceManager.h"
#include <WinInput.h>

#include "RYLClientMain.h"
#include "RYLGameScene.h"
#include "RYLSceneManager.h"
#include "RYLOptionDlg.h"
#include "RYLNetworkData.h"
#include <Network/ClientSocket/Send/SendEtc.h>
#include "GMMemory.h"

CRYLSoundOptionPanel*	g_pSoundOption = NULL ;

CRYLSoundOptionPanel::CRYLSoundOptionPanel( CGMUICustomDlg *pParentDlg ) : CGMUICustomPanel( pParentDlg )
{
	m_pMasterLabel				= NULL ;
	m_pEffectLabel				= NULL ;		// 효과음
	m_pBGMLabel					= NULL ;		// BGM
	
	m_pMasterPulsButton			= NULL ;
	m_pMasterMinusButton		= NULL ;
	m_pEffectPulsButton			= NULL ;		// 효과음 크게
	m_pEffectMinusButton		= NULL ;		// 효과음 작게
	m_pBGMPulsButton			= NULL ;		// BGM 크게
	m_pBGMMinusutton			= NULL ;		// BGM 작게
	
	m_pMasterOnOffButton		= NULL ;
	m_pEffectOnOffButton		= NULL ;		// 효과음 켜기/끄기
	m_pBGMOnOffButton			= NULL ;		// BGM 켜기/끄기

	m_pMasterProgressBar		= NULL ;
	m_pEffectProgressBar		= NULL ;		// 효과음 프로그레스바
	m_pBGMProgressBar			= NULL ;		// BGM 프로그레스바

	m_pMasterVolumeLabel		= NULL ;
	m_pEffectVolumeLabel		= NULL ;		// 효과음
	m_pBGMVolumeLabel			= NULL ;		// BGM
	
	m_pGMUIParentDlg = pParentDlg ;
	
	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;
	RECT			rt ;

	CRYLSpriteList::Instance()->CreateGMImage( 256, 395, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	int i = 0;
	for (  ; i < 74 ; i ++ )
	{
		SetRect( &rt, 0, 38, 256, 43 ) ;
		pImageDIB->DrawRect( &rt, 0, i * 5, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	}
	SetRect( &rt, 0, 42, 256, 52 ) ;
	pImageDIB->DrawRect( &rt, 0, ( i - 1 ) * 5, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	
	pImageDIB->MakeFrameRect( 24, 24, 195, 48, 0xff6e6857, 0xc0000000,  1 ) ;		
	pImageDIB->MakeFrameRect( 24, 105, 195, 48, 0xff6e6857, 0xc0000000,  1 ) ;		
	pImageDIB->MakeFrameRect( 24, 186, 195, 48, 0xff6e6857, 0xc0000000,  1 ) ;		

	pImageDIB->MakeFrameRect( 58, 41, 102, 18, 0xff959595, 0xc0000000,  1 ) ;		
	pImageDIB->MakeFrameRect( 58, 122, 102, 18, 0xff959595, 0xc0000000,  1 ) ;		
	pImageDIB->MakeFrameRect( 58, 203, 102, 18, 0xff959595, 0xc0000000,  1 ) ;		

	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	AttachPanelImage( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;
	
	SetWidth( 256 ) ;
	SetHeight( 395 ) ;

	g_pSoundOption = this ;

	m_fBgmVolume = 1.0f; 
	m_fAmbVolume = 1.0f; 
	m_fMasterVolume = 1.0f;	

	m_fOldMasterVolume = 0.0f;
	m_fOldBGMVolume = 0.0f ;
	m_fOldAmbVolume = 0.0f ;
}

CRYLSoundOptionPanel::~CRYLSoundOptionPanel() 
{
	FinalCtrl() ;
}

VOID	CRYLSoundOptionPanel::InitCtrl() 
{	
	// 마스터
	m_pMasterLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pMasterLabel->SetLeft( 27 ) ; 
	m_pMasterLabel->SetTop( 9 ) ;
	m_pMasterLabel->SetFontColor (0xffffffff);
	m_pMasterLabel->SetAutoSize( TRUE ) ;
	m_pMasterLabel->SetCaption(  CRYLStringTable::m_strString[ 2248 ] ) ;
	CGMUICustomPanel::AddChild( m_pMasterLabel ) ;
					
	// BGM
	m_pBGMLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pBGMLabel->SetLeft( 27 ) ; 
	m_pBGMLabel->SetTop( 90 ) ;
	m_pBGMLabel->SetFontColor (0xffffffff);
	m_pBGMLabel->SetAutoSize( TRUE ) ;
	m_pBGMLabel->SetCaption( CRYLStringTable::m_strString[ 2355 ] ) ;
	CGMUICustomPanel::AddChild( m_pBGMLabel ) ;

	// 효과음
	m_pEffectLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pEffectLabel->SetLeft( 27 ) ; 
	m_pEffectLabel->SetTop( 171 ) ;
	m_pEffectLabel->SetFontColor (0xffffffff);
	m_pEffectLabel->SetAutoSize( TRUE ) ;
	m_pEffectLabel->SetCaption( CRYLStringTable::m_strString[ 2354 ] ) ;
	CGMUICustomPanel::AddChild( m_pEffectLabel ) ;

	
	RECT rt ;
	CGMImageDib* pSprite			= CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ;

	// 마스터 켜기/끄기
	m_pMasterOnOffButton = new CRYLCheckBox( m_pGMUIParentDlg ) ;
	m_pMasterOnOffButton->SetLeft( 188 ) ;
	m_pMasterOnOffButton->SetTop( 41 ) ;
	m_pMasterOnOffButton->SetInstance( RYLSOUNDOPTIONPANEL::MASTER_ON_OFF_BUTTON ) ;
	m_pMasterOnOffButton->SetSecondParent( this ) ;
	m_pMasterOnOffButton->OnMouseUpEvent = DlgMouseUp ;
	SetRect( &rt, 214, 97, 232, 115 ) ;
	m_pMasterOnOffButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 214, 117, 232, 135) ;
	m_pMasterOnOffButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 214, 117, 232, 135) ;
	m_pMasterOnOffButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 214, 97, 232, 115 ) ;
	m_pMasterOnOffButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	CGMUICustomPanel::AddChild( m_pMasterOnOffButton ) ;

	// BGM 켜기/끄기
	m_pBGMOnOffButton = new CRYLCheckBox( m_pGMUIParentDlg ) ;
	m_pBGMOnOffButton->SetLeft( 188 ) ;
	m_pBGMOnOffButton->SetTop( 122 ) ;
	m_pBGMOnOffButton->SetInstance( RYLSOUNDOPTIONPANEL::BGM_ON_OFF_BUTTON ) ;
	m_pBGMOnOffButton->SetSecondParent( this ) ;
	m_pBGMOnOffButton->OnMouseUpEvent = DlgMouseUp ;
	SetRect( &rt, 214, 97, 232, 115 ) ;
	m_pBGMOnOffButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 214, 117, 232, 135) ;
	m_pBGMOnOffButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 214, 117, 232, 135) ;
	m_pBGMOnOffButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 214, 97, 232, 115 ) ;
	m_pBGMOnOffButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	CGMUICustomPanel::AddChild( m_pBGMOnOffButton ) ;

	// 효과음 켜기/끄기
	m_pEffectOnOffButton = new CRYLCheckBox( m_pGMUIParentDlg ) ;
	m_pEffectOnOffButton->SetLeft( 188 ) ;
	m_pEffectOnOffButton->SetTop( 203 ) ;
	m_pEffectOnOffButton->SetInstance( RYLSOUNDOPTIONPANEL::EFFECT_ON_OFF_BUTTON ) ;
	m_pEffectOnOffButton->SetSecondParent( this ) ;
	m_pEffectOnOffButton->OnMouseUpEvent = DlgMouseUp ;
	SetRect( &rt, 214, 97, 232, 115 ) ;
	m_pEffectOnOffButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 214, 117, 232, 135) ;
	m_pEffectOnOffButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 214, 117, 232, 135) ;
	m_pEffectOnOffButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 214, 97, 232, 115 ) ;
	m_pEffectOnOffButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	CGMUICustomPanel::AddChild( m_pEffectOnOffButton ) ;


	// 효과음 크게
	m_pMasterPulsButton = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pMasterPulsButton->SetLeft( 159 ) ;
	m_pMasterPulsButton->SetTop( 37 ) ;
	m_pMasterPulsButton->OnClickEvent = DlgClick ;
	m_pMasterPulsButton->SetInstance( RYLSOUNDOPTIONPANEL::MASTER_PULS_BUTTON ) ;
	SetRect( &rt, 26, 78, 53, 103 ) ;
	m_pMasterPulsButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 26, 128, 53, 153) ;
	m_pMasterPulsButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 26, 103, 53, 128) ;
	m_pMasterPulsButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 26, 53, 53, 78) ;
	m_pMasterPulsButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	CGMUICustomPanel::AddChild( m_pMasterPulsButton ) ;

	// 효과음 작게
	m_pMasterMinusButton = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pMasterMinusButton->SetLeft( 33 ) ;
	m_pMasterMinusButton->SetTop( 37 ) ;
	m_pMasterMinusButton->OnClickEvent = DlgClick ;
	m_pMasterMinusButton->SetInstance( RYLSOUNDOPTIONPANEL::MASTER_MINUS_BUTTON ) ;
	SetRect( &rt, 53, 78, 80, 103 ) ;
	m_pMasterMinusButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 53, 128, 80, 153) ;
	m_pMasterMinusButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 53, 103, 80, 128) ;
	m_pMasterMinusButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 53, 53, 80, 78) ;
	m_pMasterMinusButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	CGMUICustomPanel::AddChild( m_pMasterMinusButton ) ;

	
	// BGM 크게
	m_pBGMPulsButton = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pBGMPulsButton->SetLeft( 159 ) ;
	m_pBGMPulsButton->SetTop( 118 ) ;
	m_pBGMPulsButton->OnClickEvent = DlgClick ;
	m_pBGMPulsButton->SetInstance( RYLSOUNDOPTIONPANEL::BGM_PULS_BUTTON ) ;
	SetRect( &rt, 26, 78, 53, 103 ) ;
	m_pBGMPulsButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 26, 128, 53, 153) ;
	m_pBGMPulsButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 26, 103, 53, 128) ;
	m_pBGMPulsButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 26, 53, 53, 78) ;
	m_pBGMPulsButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	CGMUICustomPanel::AddChild( m_pBGMPulsButton ) ;

	// BGM 작게
	m_pBGMMinusutton = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pBGMMinusutton->SetLeft( 33 ) ;
	m_pBGMMinusutton->SetTop( 118 ) ;
	m_pBGMMinusutton->OnClickEvent = DlgClick ;
	m_pBGMMinusutton->SetInstance( RYLSOUNDOPTIONPANEL::BGM_MINUS_BUTTON ) ;
	SetRect( &rt, 53, 78, 80, 103 ) ;
	m_pBGMMinusutton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 53, 128, 80, 153) ;
	m_pBGMMinusutton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 53, 103, 80, 128) ;
	m_pBGMMinusutton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 53, 53, 80, 78) ;
	m_pBGMMinusutton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	CGMUICustomPanel::AddChild( m_pBGMMinusutton ) ;


	// 효과음 크게
	m_pEffectPulsButton = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pEffectPulsButton->SetLeft( 159 ) ;
	m_pEffectPulsButton->SetTop( 199 ) ;
	m_pEffectPulsButton->OnClickEvent = DlgClick ;
	m_pEffectPulsButton->SetInstance( RYLSOUNDOPTIONPANEL::EFFECT_PULS_BUTTON ) ;
	SetRect( &rt, 26, 78, 53, 103 ) ;
	m_pEffectPulsButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 26, 128, 53, 153) ;
	m_pEffectPulsButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 26, 103, 53, 128) ;
	m_pEffectPulsButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 26, 53, 53, 78) ;
	m_pEffectPulsButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	CGMUICustomPanel::AddChild( m_pEffectPulsButton ) ;

	// 효과음 작게
	m_pEffectMinusButton = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pEffectMinusButton->SetLeft( 33 ) ;
	m_pEffectMinusButton->SetTop( 199 ) ;
	m_pEffectMinusButton->OnClickEvent = DlgClick ;
	m_pEffectMinusButton->SetInstance( RYLSOUNDOPTIONPANEL::EFFECT_MINUS_BUTTON ) ;
	SetRect( &rt, 53, 78, 80, 103 ) ;
	m_pEffectMinusButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 53, 128, 80, 153) ;
	m_pEffectMinusButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 53, 103, 80, 128) ;
	m_pEffectMinusButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 53, 53, 80, 78) ;
	m_pEffectMinusButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	CGMUICustomPanel::AddChild( m_pEffectMinusButton ) ;


	char szText[0xff] = {0, };

	// Master
	int values = m_fMasterVolume*100;

	sprintf(szText, "%d%%", values);
		
	m_pMasterVolumeLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pMasterVolumeLabel->SetLeft( 99 ) ; 
	m_pMasterVolumeLabel->SetTop( 45 ) ;
	m_pMasterVolumeLabel->SetFontColor (0xffffffff);
	m_pMasterVolumeLabel->SetAutoSize( TRUE ) ;
	m_pMasterVolumeLabel->SetCaption(szText) ;
	CGMUICustomPanel::AddChild( m_pMasterVolumeLabel ) ;

	// BGM
	values = m_fBgmVolume*100;
	
	sprintf(szText, "%d%%", values);

	m_pBGMVolumeLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pBGMVolumeLabel->SetLeft( 99 ) ; 
	m_pBGMVolumeLabel->SetTop( 126 ) ;
	m_pBGMVolumeLabel->SetFontColor (0xffffffff);
	m_pBGMVolumeLabel->SetAutoSize( TRUE ) ;
	m_pBGMVolumeLabel->SetCaption(szText) ;
	CGMUICustomPanel::AddChild( m_pBGMVolumeLabel ) ;

	// 효과음
	values = m_fAmbVolume*100;

	sprintf(szText, "%d%%", values);
		
	m_pEffectVolumeLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pEffectVolumeLabel->SetLeft( 99 ) ; 
	m_pEffectVolumeLabel->SetTop( 207 ) ;
	m_pEffectVolumeLabel->SetFontColor (0xffffffff);
	m_pEffectVolumeLabel->SetAutoSize( TRUE ) ;
	m_pEffectVolumeLabel->SetCaption(szText) ;
	CGMUICustomPanel::AddChild( m_pEffectVolumeLabel ) ;

	// 마스터 프로그레스바
	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;

	m_pMasterProgressBar = new CRYLProgressBar( m_pGMUIParentDlg ) ;
	m_pMasterProgressBar->SetLeft( 60 ) ;
	m_pMasterProgressBar->SetTop( 43 ) ;
	m_pMasterProgressBar->SetWidth( 98 ) ;
	m_pMasterProgressBar->SetHeight( 14 ) ;
	m_pMasterProgressBar->SetMax( 100 ) ;
		
	CRYLSpriteList::Instance()->CreateGMImage( 98, 14, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt,157, 70, 255, 84 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	m_pMasterProgressBar->AttachBarImageTexture( ( CGMTexture*)pImage ) ;
	m_pMasterProgressBar->SetPosition( m_fMasterVolume * 100 ) ;
	CGMUICustomPanel::AddChild( m_pMasterProgressBar ) ;
	GM_DELETE( pImageDIB ) ;


	// BGM 프로그레스바
	m_pBGMProgressBar = new CRYLProgressBar( m_pGMUIParentDlg ) ;
	m_pBGMProgressBar->SetLeft( 60 ) ;
	m_pBGMProgressBar->SetTop( 124 ) ;
	m_pBGMProgressBar->SetWidth( 98 ) ;
	m_pBGMProgressBar->SetHeight( 14 ) ;
	m_pBGMProgressBar->SetMax( 100 ) ;	
	
	CRYLSpriteList::Instance()->CreateGMImage( 98, 14, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt,157, 70, 255, 84 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	m_pBGMProgressBar->AttachBarImageTexture( ( CGMTexture*)pImage ) ;
	m_pBGMProgressBar->SetPosition( m_fBgmVolume * 100 ) ;
	CGMUICustomPanel::AddChild( m_pBGMProgressBar ) ;
	GM_DELETE( pImageDIB ) ;


	// 효과음 프로그레스바
	m_pEffectProgressBar = new CRYLProgressBar( m_pGMUIParentDlg ) ;
	m_pEffectProgressBar->SetLeft( 60 ) ;
	m_pEffectProgressBar->SetTop( 205 ) ;
	m_pEffectProgressBar->SetWidth( 98 ) ;
	m_pEffectProgressBar->SetHeight( 14 ) ;
	m_pEffectProgressBar->SetMax( 100 ) ;
		
	CRYLSpriteList::Instance()->CreateGMImage( 98, 14, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt,157, 70, 255, 84 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	m_pEffectProgressBar->AttachBarImageTexture( ( CGMTexture*)pImage ) ;
	m_pEffectProgressBar->SetPosition( m_fAmbVolume * 100 ) ;
	CGMUICustomPanel::AddChild( m_pEffectProgressBar ) ;
	GM_DELETE( pImageDIB ) ;



	pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;

	// 확인
	m_pOkButton = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pOkButton->SetLeft( 154 ) ;
	m_pOkButton->SetTop( 345 ) ;
	m_pOkButton->SetCaption( CRYLStringTable::m_strString[ 2164 ] ) ;
	m_pOkButton->SetFontColor( 0xffffffff) ;
	m_pOkButton->SetColor( 0xffffffff) ;
	m_pOkButton->OnClickEvent = DlgClick ;
	m_pOkButton->SetInstance( RYLSOUNDOPTIONPANEL::OK_BUTTON ) ;
	m_pOkButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pOkButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pOkButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pOkButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	CGMUICustomPanel::AddChild( m_pOkButton ) ;
}

VOID	CRYLSoundOptionPanel::FinalCtrl() 
{
	GM_DELETE( m_pMasterLabel ) ;			// 마스터
	GM_DELETE( m_pBGMLabel ) ;				// BGM
	GM_DELETE( m_pEffectLabel ) ;			// 효과음

	GM_DELETE( m_pMasterPulsButton ) ;		// 마스터 크게
	GM_DELETE( m_pMasterMinusButton ) ;		// 마스터 작게
	GM_DELETE( m_pBGMPulsButton ) ;			// BGM 크게
	GM_DELETE( m_pBGMMinusutton ) ;			// BGM 작게
	GM_DELETE( m_pEffectPulsButton ) ;		// 효과음 크게
	GM_DELETE( m_pEffectMinusButton ) ;		// 효과음 작게
	
	GM_DELETE( m_pMasterOnOffButton ) ;		// 마스터 켜기/끄기
	GM_DELETE( m_pBGMOnOffButton ) ;		// BGM 켜기/끄기
	GM_DELETE( m_pEffectOnOffButton ) ;		// 효과음 켜기/끄기

	GM_DELETE( m_pMasterProgressBar ) ;		// 마스터 프로그레스바
	GM_DELETE( m_pBGMProgressBar ) ;		// BGM 프로그레스바
	GM_DELETE( m_pEffectProgressBar ) ;		// 효과음 프로그레스바

	GM_DELETE( m_pMasterVolumeLabel ) ;		// 마스터
	GM_DELETE( m_pBGMVolumeLabel ) ;		// BGM
	GM_DELETE( m_pEffectVolumeLabel ) ;		// 효과음

	GM_DELETE( m_pOkButton ) ;				// 확인
}

HRESULT		CRYLSoundOptionPanel::Update()
{
	if (g_DeviceInput.GetIsLeftMousePress())
	{
		// 마스터 작게
		if ( g_DeviceInput.InRect( m_pGMUIParentDlg->GetLeft() + 36, m_pGMUIParentDlg->GetTop() + 76, 
								   m_pGMUIParentDlg->GetLeft() + 52, m_pGMUIParentDlg->GetTop() + 92 ) )
		{
			m_pMasterOnOffButton->SetChecked( FALSE ) ;

			if(g_pSoundOption->m_fMasterVolume <= 0.01f)
				g_pSoundOption->m_fMasterVolume = 0.0f; 
			else 
				g_pSoundOption->m_fMasterVolume -= 0.01f; 
			
			if( g_pSoundOption->m_fMasterVolume < 0.0f ) 
				g_pSoundOption->m_fMasterVolume = 0.0f; 

			CSoundMgr::_GetInstance()->SetVolume(g_pSoundOption->m_fMasterVolume); 

			g_pSoundOption->m_pMasterProgressBar->SetPosition( g_pSoundOption->m_fMasterVolume * 100 ) ;
			CHAR Buff[ 32 ] ;
			sprintf( Buff, "%d%%", ( int )(g_pSoundOption->m_fMasterVolume * 100 )) ;
			g_pSoundOption->m_pMasterVolumeLabel->SetCaption( Buff ) ;
		}

		// 마스터 크게
		if ( g_DeviceInput.InRect( m_pGMUIParentDlg->GetLeft() + 162, m_pGMUIParentDlg->GetTop() + 76, 
								   m_pGMUIParentDlg->GetLeft() + 178, m_pGMUIParentDlg->GetTop() + 92 ) )
		{
			m_pMasterOnOffButton->SetChecked( FALSE ) ;
			g_pSoundOption->m_fMasterVolume += 0.01f; 

			if( g_pSoundOption->m_fMasterVolume > 1.0f ) 
				g_pSoundOption->m_fMasterVolume = 1.0f; 

			CSoundMgr::_GetInstance()->SetVolume(g_pSoundOption->m_fMasterVolume); 

			g_pSoundOption->m_pMasterProgressBar->SetPosition( g_pSoundOption->m_fMasterVolume * 100 ) ;
			CHAR Buff[ 32 ] ;
			sprintf( Buff, "%d%%", (int)(g_pSoundOption->m_fMasterVolume * 100) ) ;
			g_pSoundOption->m_pMasterVolumeLabel->SetCaption( Buff ) ;
		}


		// BGM 작게
		if ( g_DeviceInput.InRect( m_pGMUIParentDlg->GetLeft() + 36, m_pGMUIParentDlg->GetTop() + 157, 
								   m_pGMUIParentDlg->GetLeft() + 52, m_pGMUIParentDlg->GetTop() + 173 ) )
		{
			m_pBGMOnOffButton->SetChecked( FALSE ) ;

			if(g_pSoundOption->m_fBgmVolume <= 0.01f)
				g_pSoundOption->m_fBgmVolume = 0.0f; 
			else
				g_pSoundOption->m_fBgmVolume -= 0.01f; 
			
			if( g_pSoundOption->m_fBgmVolume < 0.0f ) 
				g_pSoundOption->m_fBgmVolume = 0.0f; 
			
			CBgmManager::_GetInstance()->SetVolume(g_pSoundOption->m_fBgmVolume); 

			g_pSoundOption->m_pBGMProgressBar->SetPosition( g_pSoundOption->m_fBgmVolume * 100 ) ;
			CHAR Buff[ 32 ] ;
			sprintf( Buff, "%d%%", (int)(g_pSoundOption->m_fBgmVolume * 100 ) ) ;
			g_pSoundOption->m_pBGMVolumeLabel->SetCaption( Buff ) ;
		}

		// BGM 크게
		if ( g_DeviceInput.InRect( m_pGMUIParentDlg->GetLeft() + 162, m_pGMUIParentDlg->GetTop() + 157, 
								   m_pGMUIParentDlg->GetLeft() + 178, m_pGMUIParentDlg->GetTop() + 173 ) )
		{
			m_pBGMOnOffButton->SetChecked( FALSE ) ;
			g_pSoundOption->m_fBgmVolume += 0.01f; 
			
			if( g_pSoundOption->m_fBgmVolume > 1.0f ) 
				g_pSoundOption->m_fBgmVolume = 1.0f; 

			CBgmManager::_GetInstance()->SetVolume(g_pSoundOption->m_fBgmVolume); 

			g_pSoundOption->m_pBGMProgressBar->SetPosition( g_pSoundOption->m_fBgmVolume * 100 ) ;
			CHAR Buff[ 32 ] ;
			sprintf( Buff, "%d%%", (int)(g_pSoundOption->m_fBgmVolume * 100) ) ;
			g_pSoundOption->m_pBGMVolumeLabel->SetCaption( Buff ) ;
		}


		// 환경음 작게
		if ( g_DeviceInput.InRect( m_pGMUIParentDlg->GetLeft() + 36, m_pGMUIParentDlg->GetTop() + 238, 
								   m_pGMUIParentDlg->GetLeft() + 52, m_pGMUIParentDlg->GetTop() + 254 ) )
		{
			m_pEffectOnOffButton->SetChecked( FALSE ) ;

			if(g_pSoundOption->m_fAmbVolume <= 0.01f)
				g_pSoundOption->m_fAmbVolume = 0.0f; 
			else
				g_pSoundOption->m_fAmbVolume -= 0.01f; 
			
			if( g_pSoundOption->m_fAmbVolume < 0.0f ) 
				g_pSoundOption->m_fAmbVolume = 0.0f; 

			CAmbienceManager::_GetInstance()->SetVolume(g_pSoundOption->m_fAmbVolume); 

			g_pSoundOption->m_pEffectProgressBar->SetPosition( g_pSoundOption->m_fAmbVolume * 100 ) ;
			CHAR Buff[ 32 ] ;
			sprintf( Buff, "%d%%", ( int )(g_pSoundOption->m_fAmbVolume * 100 )) ;
			g_pSoundOption->m_pEffectVolumeLabel->SetCaption( Buff ) ;
		}

		// 환경음 크게
		if ( g_DeviceInput.InRect( m_pGMUIParentDlg->GetLeft() + 162, m_pGMUIParentDlg->GetTop() + 238, 
								   m_pGMUIParentDlg->GetLeft() + 178, m_pGMUIParentDlg->GetTop() + 254 ) )
		{
			m_pEffectOnOffButton->SetChecked( FALSE ) ;
			g_pSoundOption->m_fAmbVolume += 0.01f; 

			if( g_pSoundOption->m_fAmbVolume > 1.0f ) 
				g_pSoundOption->m_fAmbVolume = 1.0f; 

			CAmbienceManager::_GetInstance()->SetVolume(g_pSoundOption->m_fAmbVolume); 

			g_pSoundOption->m_pEffectProgressBar->SetPosition( g_pSoundOption->m_fAmbVolume * 100 ) ;
			CHAR Buff[ 32 ] ;
			sprintf( Buff, "%d%%", (int)(g_pSoundOption->m_fAmbVolume * 100) ) ;
			g_pSoundOption->m_pEffectVolumeLabel->SetCaption( Buff ) ;
		}
	}
	
	return S_OK ;
}

VOID		CRYLSoundOptionPanel::DlgClick( CGMUIObject* Sender, INT X, INT Y ) 
{
	if ( !g_pSoundOption )
		return ;

	CRYLGameData* pGame = CRYLGameData::Instance() ;
	
	switch ( Sender->GetInstance() )
	{
		case RYLSOUNDOPTIONPANEL::OK_BUTTON	:
		{
			// BGM, 효과음 //
			CRYLOptionDlg::m_coOption.m_fMasterVolume	= g_pSoundOption->m_fMasterVolume;
			CRYLOptionDlg::m_coOption.m_fAmbVolume	= g_pSoundOption->m_fAmbVolume;
			CRYLOptionDlg::m_coOption.m_fBgmVolume	= g_pSoundOption->m_fBgmVolume;

			CRYLGameScene*	pScene		= ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );
			CRYLOptionDlg*  pOptionDlg	= static_cast<CRYLOptionDlg *>( pScene->GetOptionDlg() );

			pOptionDlg->OK();
			break;
		}
		break ;
	}
}

VOID	CRYLSoundOptionPanel::ReLoadOption() 
{
	CHAR Buff[ 32 ] ;

	m_fMasterVolume = CRYLOptionDlg::m_coOption.m_fMasterVolume;
	m_fOldMasterVolume = m_fMasterVolume ;

	if ( CRYLOptionDlg::m_coOption.m_dwMasterSound )
	{
		CSoundMgr::_GetInstance()->SetVolume( 0.0f ); 
		m_pMasterOnOffButton->SetChecked( TRUE ) ;
	}
	else
	{
		CSoundMgr::_GetInstance()->SetVolume( m_fMasterVolume ); 
		m_pMasterOnOffButton->SetChecked( FALSE ) ;
	}

	if(m_pMasterProgressBar && m_pMasterVolumeLabel)
	{
		m_pMasterProgressBar->SetPosition( m_fMasterVolume * 100 ) ;
		sprintf( Buff, "%d%%", ( int )(m_fMasterVolume * 100 )) ;
		m_pMasterVolumeLabel->SetCaption( Buff ) ;
	}


	m_fAmbVolume = CRYLOptionDlg::m_coOption.m_fAmbVolume;
	m_fOldAmbVolume = m_fAmbVolume ;

	if ( CRYLOptionDlg::m_coOption.m_dwEffectSound )
	{
		CAmbienceManager::_GetInstance()->SetVolume( 0.0f ); 
		m_pEffectOnOffButton->SetChecked( TRUE ) ;
	}
	else
	{
		CAmbienceManager::_GetInstance()->SetVolume( m_fAmbVolume ); 
		m_pEffectOnOffButton->SetChecked( FALSE ) ;
	}

	if(m_pEffectProgressBar && m_pEffectVolumeLabel)
	{
		m_pEffectProgressBar->SetPosition( m_fAmbVolume * 100 ) ;
		sprintf( Buff, "%d%%", (int)(m_fAmbVolume * 100) ) ;
		m_pEffectVolumeLabel->SetCaption( Buff ) ;
	}


	m_fBgmVolume = CRYLOptionDlg::m_coOption.m_fBgmVolume;
	m_fOldBGMVolume = m_fBgmVolume ;

	if ( CRYLOptionDlg::m_coOption.m_dwBGMSound )	
	{
		CBgmManager::_GetInstance()->SetVolume( 0.0f );
		m_pBGMOnOffButton->SetChecked( TRUE ) ;
	}
	else
	{
		CBgmManager::_GetInstance()->SetVolume( m_fBgmVolume ); 				
		m_pBGMOnOffButton->SetChecked( FALSE ) ;
	}

	if(m_pBGMProgressBar && m_pBGMVolumeLabel)
	{
		m_pBGMProgressBar->SetPosition( m_fBgmVolume * 100 ) ;
		sprintf( Buff, "%d%%", (int)(m_fBgmVolume * 100 ) ) ;
		m_pBGMVolumeLabel->SetCaption( Buff ) ;
	}
}

VOID	CRYLSoundOptionPanel::DlgMouseUp( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y ) 
{
	if ( !g_pSoundOption )
		return ;

	CRYLGameData* pGame = CRYLGameData::Instance() ;
	
	switch ( Sender->GetInstance() )
	{
		case 	RYLSOUNDOPTIONPANEL::MASTER_ON_OFF_BUTTON :
		{
			g_pSoundOption->m_fOldMasterVolume = g_pSoundOption->m_fMasterVolume ;

			if ( g_pSoundOption->m_pMasterOnOffButton->GetChecked() )		
			{
				CSoundMgr::_GetInstance()->SetVolume( 0.0f ); 
				CRYLOptionDlg::m_coOption.m_dwMasterSound = 1 ;
			}
			else
			{
				CSoundMgr::_GetInstance()->SetVolume( g_pSoundOption->m_fOldMasterVolume ); 
				CRYLOptionDlg::m_coOption.m_dwMasterSound = 0 ;
			}
			
			pGame->PlayClickSound() ;
			break ;
		}

		case 	RYLSOUNDOPTIONPANEL::EFFECT_ON_OFF_BUTTON :
		{
			g_pSoundOption->m_fOldBGMVolume = g_pSoundOption->m_fAmbVolume ;

			if ( g_pSoundOption->m_pEffectOnOffButton->GetChecked() )		
			{
				CAmbienceManager::_GetInstance()->SetVolume( 0.0f ); 
				CRYLOptionDlg::m_coOption.m_dwEffectSound = 1 ;
			}
			else
			{
				CAmbienceManager::_GetInstance()->SetVolume( g_pSoundOption->m_fOldBGMVolume ); 
				CRYLOptionDlg::m_coOption.m_dwEffectSound = 0 ;
			}
			
			pGame->PlayClickSound() ;
			break ;
		}

		case	RYLSOUNDOPTIONPANEL::BGM_ON_OFF_BUTTON	:
		{
			g_pSoundOption->m_fOldAmbVolume = g_pSoundOption->m_fBgmVolume ;

			if ( g_pSoundOption->m_pBGMOnOffButton->GetChecked() )		
			{
				CBgmManager::_GetInstance()->SetVolume( 0.0f );
				CRYLOptionDlg::m_coOption.m_dwBGMSound = 1 ;
			}
			else
			{
				CBgmManager::_GetInstance()->SetVolume( g_pSoundOption->m_fOldAmbVolume ); 				
				CRYLOptionDlg::m_coOption.m_dwBGMSound = 0 ;
			}

			pGame->PlayClickSound() ;
			break ;
		}
	}
}