#include <winsock2.h>
#include "RYLClientMain.h"
#include "RYLStatusDlg.h"
#include "RYLButton.h"
#include "RYLLabel.h"
#include "RYLProgressBar.h"
#include "RYLImageManager.h"
#include "RYLImageCabinet.h"
#include "RYLSpriteList.h"
#include "WinInput.h"
#include "GMUIObject.h"

#include "GMImageDib.h"
#include "RYLImage.h"
#include "RYLStringTable.h"

#include "RYLGameData.h"
#include <RenderOption.h>
#include "RYLStatusExtensionSlotOne.h"
#include "RYLStatusExtensionSlotTwo.h"

#include "RYLGameScene.h"
#include "RYLSceneManager.h"
#include "GMMemory.h"

CRYLStatusDlg*	g_pStatusDlg = NULL ;

CRYLStatusDlg::CRYLStatusDlg( INT nLeft, INT nTop, INT nLevel )	
{
	m_pHideButton		= NULL ;				// Hide 버튼
	m_pExtensionButton	= NULL ;				// 확장 버튼
	m_nExtensionLevel	= 0;
	m_pSlotOne			= NULL;
	m_pSlotTwo			= NULL;

	RECT rt ;
	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;
	
	CRYLSpriteList::Instance()->CreateGMImage( 347, 97, &pImageDIB ) ; // was 238, 41, &pImageDIB
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 0, 347, 97 ) ; // was SetRect( &rt, 0, 0, 147, 41 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_STATUS_256x256 ) ) ; // was RYL_MAINGUI_256x256
//	SetRect( &rt, 0, 41, 90, 81 ) ;
//	pImageDIB->DrawRect( &rt, 147, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI_256x256 ) ) ;
	
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	GM_DELETE( pImageDIB ) ;

	AttachWindowTexture( ( CGMTexture*)pImage ) ;
	
	SetWidth( 347 ) ; // was 238
	SetHeight( 97 ) ; // was 41
	
	SetLeft( nLeft ) ;
	SetTop( nTop ) ;
	m_nHeight = GetHeight();

	SetFormType( RYLDLG_TYPE_NONE ) ;
	
	g_pStatusDlg = this ;	
	InitCtrl() ;

	m_nExtensionLevel = nLevel;
	UpdateExtension();
}

CRYLStatusDlg::~CRYLStatusDlg ()
{
	FinalCtrl() ;
}

VOID	CRYLStatusDlg::InitCtrl ()
{
	RECT rt ;
	CGMImageDib* pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI_256x256 ) ; // was RYL_MAINGUI_256X256

	// NATION LABEL
	m_pNationLabel = new CRYLLabel( this ) ;
	m_pNationLabel->SetLeft( 320 ) ; // was 212
	m_pNationLabel->SetTop( 75 ) ; // was 26
	m_pNationLabel->SetFontColor (0xffffffff);
	m_pNationLabel->SetCaption( "" ) ;
	SetRect( &rt, 313, 74, 343, 89 ) ;
	m_pNationLabel->SetAlignRect( rt ) ;
	m_pNationLabel->SetAlign( CENTER ) ;
	m_pNationLabel->SetAutoSize( TRUE ) ;
	m_pNationLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1797 ], 1797, TOOLTIP_PANEL ) ;
	AttachGMUIControl( m_pNationLabel ) ;

	// DAMAGE LABEL
	m_pStrikingPowerLabel = new CRYLLabel( this ) ;
	m_pStrikingPowerLabel->SetLeft( 60 ) ; // was 212
	m_pStrikingPowerLabel->SetTop( 83 ) ; // was 26
	m_pStrikingPowerLabel->SetFontColor (0xffffffff);
	m_pStrikingPowerLabel->SetCaption( "" ) ;
	SetRect( &rt, 60, 82, 62, 83 ) ;
	m_pStrikingPowerLabel->SetAlignRect( rt ) ;
	m_pStrikingPowerLabel->SetAlign( CENTER ) ;
	m_pStrikingPowerLabel->SetAutoSize( TRUE ) ;
	m_pStrikingPowerLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1807 ], 1807, TOOLTIP_PANEL ) ;
	AttachGMUIControl( m_pStrikingPowerLabel ) ;

	// ACCUARY LABEL
	m_pHitRatioLabel = new CRYLLabel( this ) ;
	m_pHitRatioLabel->SetLeft( 145 ) ; // was 212
	m_pHitRatioLabel->SetTop( 83 ) ; // was 26
	m_pHitRatioLabel->SetFontColor (0xffffffff);
	m_pHitRatioLabel->SetCaption( "" ) ;
	SetRect( &rt, 145, 82, 147, 83 ) ;
	m_pHitRatioLabel->SetAlignRect( rt ) ;
	m_pHitRatioLabel->SetAlign( CENTER ) ;
	m_pHitRatioLabel->SetAutoSize( TRUE ) ;
	m_pHitRatioLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1812 ], 1812, TOOLTIP_PANEL ) ;
	AttachGMUIControl( m_pHitRatioLabel ) ;

	// CRITICAL LABEL
	m_pCriticalLabel = new CRYLLabel( this ) ;
	m_pCriticalLabel->SetLeft( 54 ) ; // was 212
	m_pCriticalLabel->SetTop( 95 ) ; // was 26
	m_pCriticalLabel->SetFontColor (0xffffffff);
	m_pCriticalLabel->SetCaption( "" ) ;
	SetRect( &rt, 24, 94, 95, 109 ) ;
	m_pCriticalLabel->SetAlignRect( rt ) ;
	m_pCriticalLabel->SetAlign( CENTER ) ;
	m_pCriticalLabel->SetAutoSize( TRUE ) ;
	m_pCriticalLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1797 ], 1797, TOOLTIP_PANEL ) ;
	AttachGMUIControl( m_pCriticalLabel ) ;

	// DEFENCE LABEL
	m_pDefensivePowerLabel = new CRYLLabel( this ) ;
	m_pDefensivePowerLabel->SetLeft(218 ) ; // was 212
	m_pDefensivePowerLabel->SetTop( 83 ) ; // was 26
	m_pDefensivePowerLabel->SetFontColor (0xffffffff);
	m_pDefensivePowerLabel->SetCaption( "" ) ;
	SetRect( &rt, 218, 82, 220, 83 ) ;
	m_pDefensivePowerLabel->SetAlignRect( rt ) ;
	m_pDefensivePowerLabel->SetAlign( CENTER ) ;
	m_pDefensivePowerLabel->SetAutoSize( TRUE ) ;
	m_pDefensivePowerLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1808 ], 1808, TOOLTIP_PANEL ) ;
	AttachGMUIControl( m_pDefensivePowerLabel ) ;

	// MAGIC DEFENCE LABEL
	m_pMagicResistLabel = new CRYLLabel( this ) ;
	m_pMagicResistLabel->SetLeft( 295 ) ; // was 212
	m_pMagicResistLabel->SetTop( 83 ) ; // was 26
	m_pMagicResistLabel->SetFontColor (0xffffffff);
	m_pMagicResistLabel->SetCaption( "" ) ;
	SetRect( &rt, 295, 82, 297, 83 ) ;
	m_pMagicResistLabel->SetAlignRect( rt ) ;
	m_pMagicResistLabel->SetAlign( CENTER ) ;
	m_pMagicResistLabel->SetAutoSize( TRUE ) ;
	m_pMagicResistLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1817 ], 1817, TOOLTIP_PANEL ) ;
	AttachGMUIControl( m_pMagicResistLabel ) ;

	// EVADE LABEL
	m_pEvasionLabel = new CRYLLabel( this ) ;
	m_pEvasionLabel->SetLeft( 152) ; // was 212
	m_pEvasionLabel->SetTop( 95 ) ; // was 26
	m_pEvasionLabel->SetFontColor (0xffffffff);
	m_pEvasionLabel->SetCaption( "" ) ;
	SetRect( &rt, 122, 95, 193, 109 ) ;
	m_pEvasionLabel->SetAlignRect( rt ) ;
	m_pEvasionLabel->SetAlign( CENTER ) ;
	m_pEvasionLabel->SetAutoSize( TRUE ) ;
	m_pEvasionLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1797 ], 1797, TOOLTIP_PANEL ) ;
	AttachGMUIControl( m_pEvasionLabel ) ;

	// BLOCK LABEL
	m_pBlockPowerLabel = new CRYLLabel( this ) ;
	m_pBlockPowerLabel->SetLeft( 254) ; // was 212
	m_pBlockPowerLabel->SetTop( 55 ) ; // was 26
	m_pBlockPowerLabel->SetFontColor (0xffffffff);
	m_pBlockPowerLabel->SetCaption( "" ) ;
	SetRect( &rt, 220, 55, 292, 69 ) ;
	m_pBlockPowerLabel->SetAlignRect( rt ) ;
	m_pBlockPowerLabel->SetAlign( CENTER ) ;
	m_pBlockPowerLabel->SetAutoSize( TRUE ) ;
	m_pBlockPowerLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1797 ], 1797, TOOLTIP_PANEL ) ;
	AttachGMUIControl( m_pBlockPowerLabel ) ;

	// MAGIC POWER LABEL
	m_pMagicPowerLabel = new CRYLLabel( this ) ;
	m_pMagicPowerLabel->SetLeft( 254) ; // was 212
	m_pMagicPowerLabel->SetTop( 75 ) ; // was 26
	m_pMagicPowerLabel->SetFontColor (0xffffffff);
	m_pMagicPowerLabel->SetCaption( "" ) ;
	SetRect( &rt, 220, 75, 291, 89 ) ;
	m_pMagicPowerLabel->SetAlignRect( rt ) ;
	m_pMagicPowerLabel->SetAlign( CENTER ) ;
	m_pMagicPowerLabel->SetAutoSize( TRUE ) ;
	m_pMagicPowerLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1816 ], 1816, TOOLTIP_PANEL ) ;
	AttachGMUIControl( m_pMagicPowerLabel ) ;

	// HP-REC LABEL
	m_pHPRECLabel = new CRYLLabel( this ) ;
	m_pHPRECLabel->SetLeft( 254) ; // was 212
	m_pHPRECLabel->SetTop( 95 ) ; // was 26
	m_pHPRECLabel->SetFontColor (0xffffffff);
	m_pHPRECLabel->SetCaption( "" ) ;
	SetRect( &rt, 220, 95, 292, 109 ) ;
	m_pHPRECLabel->SetAlignRect( rt ) ;
	m_pHPRECLabel->SetAlign( CENTER ) ;
	m_pHPRECLabel->SetAutoSize( TRUE ) ;
	m_pHPRECLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1816 ], 1816, TOOLTIP_PANEL ) ;
	AttachGMUIControl( m_pHPRECLabel ) ;

	// Hide 버튼
	m_pHideButton = new CRYLButton( this ) ;
	m_pHideButton->SetLeft( 324 ) ; // was 224
	m_pHideButton->SetTop( 3 ) ;
	m_pHideButton->OnClickEvent = DlgClick ;
	m_pHideButton->SetInstance( RYLSTATUSDLG::HIDE_BUTTON ) ;
	SetRect( &rt, 202, 0, 213, 9 ) ;
	m_pHideButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 202, 18, 213, 27 ) ;
	m_pHideButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 202, 9, 213, 18 ) ;
	m_pHideButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 202, 9, 213, 18 ) ;
	m_pHideButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pHideButton->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 1868], 1868 ) ;
	AttachGMUIControl( m_pHideButton ) ;
	
	// 확장 버튼
	pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI02_256x256 ) ;
	m_pExtensionButton = new CRYLButton( this ) ;
	m_pExtensionButton->SetLeft( 335 ) ; // was 212
	m_pExtensionButton->SetTop( 3 ) ;
	m_pExtensionButton->OnClickEvent = DlgClick ;
	m_pExtensionButton->SetInstance( RYLSTATUSDLG::EXTENSION_BUTTON ) ;
	SetRect( &rt, 153, 126, 163, 135 ) ;
	m_pExtensionButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 153, 135, 163, 144 ) ;
	m_pExtensionButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 153, 144, 163, 153 ) ;
	m_pExtensionButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 153, 144, 163, 153 ) ;
	m_pExtensionButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pExtensionButton->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[2307], 1868 ) ;
	AttachGMUIControl( m_pExtensionButton ) ;

	// Exp 프로그레스바
	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;

	// 
	m_pHPProgressBar = new CRYLProgressBar( this ) ;    // unde se incadreaza imaginea
	m_pHPProgressBar->SetLeft( 5 ) ; // was 18
	m_pHPProgressBar->SetTop( 16 ) ; // was 16
	m_pHPProgressBar->SetWidth( 337 ) ; // was 81
	m_pHPProgressBar->SetHeight( 15 ) ; // was 13
	m_pHPProgressBar->SetMax( 10 ) ; // was 10
	
	CRYLSpriteList::Instance()->CreateGMImage( 337, 15, &pImageDIB ) ;  // was 81, 13 // dimesniunea la bara hp care trebuie sa o incadrezi in imagine
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 126, 337, 141 ) ; // was SetRect( &rt, 166, 106, 247, 119 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_STATUS_256x256 ) ) ; // was RYL_MAINGUI02_256x256
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	m_pHPProgressBar->AttachBarImageTexture( ( CGMTexture*)pImage ) ;
	m_pHPProgressBar->SetPosition( 10 ) ;
	AttachGMUIControl( m_pHPProgressBar ) ;
	GM_DELETE( pImageDIB ) ;

	// MP 
	m_pMPProgressBar = new CRYLProgressBar( this ) ;
	m_pMPProgressBar->SetLeft( 5) ; // was 120 
	m_pMPProgressBar->SetTop( 35 ) ; // was 16  
	m_pMPProgressBar->SetWidth( 337 ) ; // was 81
	m_pMPProgressBar->SetHeight( 16 ) ; // was 13
	m_pMPProgressBar->SetMax( 10 ) ; // was 10
	
	CRYLSpriteList::Instance()->CreateGMImage( 337, 16, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt,0, 142, 337, 156 ) ; // was SetRect( &rt,166, 92, 247, 105 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_STATUS_256x256 ) ) ; // was RYL_MAINGUI02_256x256
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	m_pMPProgressBar->AttachBarImageTexture( ( CGMTexture*)pImage ) ;
	m_pMPProgressBar->SetPosition( 10 ) ;
	AttachGMUIControl( m_pMPProgressBar ) ;
	GM_DELETE( pImageDIB ) ;

	m_pExpProgressBar = new CRYLProgressBar( this ) ; // exp bar unde se incadreaza
	m_pExpProgressBar->SetLeft( 5 ) ; // was 20
	m_pExpProgressBar->SetTop( 55 ) ;  // was 35 // was 113
	m_pExpProgressBar->SetWidth( 337 ) ; // was182
	m_pExpProgressBar->SetHeight( 16 ) ; // was 1 // was 4
	m_pExpProgressBar->SetMax( 10 ) ; // was 10
	
	CRYLSpriteList::Instance()->CreateGMImage( 337, 16, &pImageDIB ) ; // was 182, 1 // exp bar de unde ia imaginea din fisier (coordonate) // was 4
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt,0, 158, 337, 172 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_STATUS_256x256 ) ) ;
//	SetRect( &rt,153, 124, 244, 125 ) ;
//	pImageDIB->DrawRect( &rt, 89, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_NewMiniMap_256x256 ) ) ;
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	m_pExpProgressBar->AttachBarImageTexture( ( CGMTexture*)pImage ) ;
	m_pExpProgressBar->SetPosition( 10 ) ;
	AttachGMUIControl( m_pExpProgressBar ) ;
	GM_DELETE( pImageDIB ) ;

	m_pHPLebel = new CRYLLabel( this ) ;
	m_pHPLebel->SetLeft( 150 ) ; // was 22
	m_pHPLebel->SetTop( 17 ) ; // was 18
	m_pHPLebel->SetFontColor (0xffffffff);
	m_pHPLebel->SetCaption( "" ) ;
	SetRect( &rt, 4, 16, 343, 31 ) ;
	m_pHPLebel->SetAlignRect( rt ) ;
	m_pHPLebel->SetAlign( CENTER ) ;
	m_pHPLebel->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pHPLebel ) ;

	m_pMPLebel = new CRYLLabel( this ) ;
	m_pMPLebel->SetLeft( 150 ) ; // was 125
	m_pMPLebel->SetTop( 36 ) ;  // was 18
	m_pMPLebel->SetFontColor (0xffffffff);
	m_pMPLebel->SetCaption( "" ) ;
	SetRect( &rt, 4, 35, 343, 50 ) ;
	m_pMPLebel->SetAlignRect( rt ) ;
	m_pMPLebel->SetAlign( CENTER ) ;
	m_pMPLebel->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pMPLebel ) ;

	// VALUE EXP LABEL
	m_pExperienceBalueLabel = new CRYLLabel( this ) ;
	m_pExperienceBalueLabel->SetLeft( 150 ) ; // was 212
	m_pExperienceBalueLabel->SetTop( 53 ) ; // was 26
	m_pExperienceBalueLabel->SetFontColor (0xffffffff);
	m_pExperienceBalueLabel->SetCaption( "" ) ;
	SetRect( &rt, 4, 53, 343, 72 ) ;
	m_pExperienceBalueLabel->SetAlignRect( rt ) ;
	m_pExperienceBalueLabel->SetAlign( CENTER ) ;
	m_pExperienceBalueLabel->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pExperienceBalueLabel ) ;

	// LEVEL LABEL
	m_pLevelLabel = new CRYLLabel( this ) ;
	m_pLevelLabel->SetLeft( 315 ) ; // was 212
	m_pLevelLabel->SetTop( 55 ) ; // was 26
	m_pLevelLabel->SetFontColor (0xffffffff);
	m_pLevelLabel->SetCaption( "" ) ;
	SetRect( &rt, 314, 54, 342, 69 ) ;
	m_pLevelLabel->SetAlignRect( rt ) ;
	m_pLevelLabel->SetAlign( CENTER ) ;
	m_pLevelLabel->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pLevelLabel ) ;

	

	// Slot 1
	if( !m_pSlotOne )
		m_pSlotOne = new CRYLStatusExtensionSlotOne( this ) ;
	m_pSlotOne->SetLeft( 0 ) ;
	m_pSlotOne->SetTop( GetHeight() ) ;
	m_pSlotOne->SetWidth( GetWidth() ) ;
	m_pSlotOne->SetHeight( 21 ) ;
	m_pSlotOne->SetVisible( FALSE ) ;
	AttachGMUIControl( m_pSlotOne ) ;


	// Slot 2
	if( !m_pSlotTwo )
		m_pSlotTwo = new CRYLStatusExtensionSlotTwo( this ) ;
	m_pSlotTwo->SetLeft( 0 ) ;
	m_pSlotTwo->SetTop( GetHeight()+m_pSlotOne->GetHeight() ) ;
	m_pSlotTwo->SetWidth( GetWidth() ) ;
	m_pSlotTwo->SetHeight( 21 ) ;
	m_pSlotTwo->SetVisible( FALSE ) ;
	AttachGMUIControl( m_pSlotTwo ) ;
	

}

VOID	CRYLStatusDlg::FinalCtrl ()
{
	GM_DELETE( m_pLevelLabel ) ;
	GM_DELETE( m_pHideButton ) ;
	GM_DELETE( m_pExtensionButton );
	GM_DELETE( m_pHPProgressBar ) ;
	GM_DELETE( m_pMPProgressBar ) ;
	GM_DELETE( m_pExpProgressBar ) ;
	GM_DELETE( m_pHPLebel ) ;
	GM_DELETE( m_pMPLebel ) ;
	GM_DELETE( m_pSlotOne ) ;
	GM_DELETE( m_pSlotTwo ) ;

	// NEW LABEL BY SHADOW

	GM_DELETE( m_pExperienceBalueLabel ) ;
	GM_DELETE( m_pNationLabel ) ;

	GM_DELETE( m_pStrikingPowerLabel ) ;
	GM_DELETE( m_pHitRatioLabel ) ;
	GM_DELETE( m_pCriticalLabel ) ;

	GM_DELETE( m_pDefensivePowerLabel ) ;
	GM_DELETE( m_pMagicResistLabel ) ;
	GM_DELETE( m_pEvasionLabel ) ;

	GM_DELETE( m_pBlockPowerLabel ) ;
	GM_DELETE( m_pMagicPowerLabel ) ;
	GM_DELETE( m_pHPRECLabel ) ;

}

VOID	CRYLStatusDlg::Show( CGMUIObject* Sender )
{
	
}
	
VOID	CRYLStatusDlg::Hide( CGMUIObject* Sender )
{
	
}

VOID	CRYLStatusDlg::GMUIPaint( INT xx, INT yy )
{
	CRYLDialog::GMUIPaint( xx, yy ) ;
}

HRESULT		CRYLStatusDlg::Update( BOOL &bClick, BOOL &bEdge ) 
{
	if (FALSE == GetVisible())
	{
		return S_OK;
	}

	if (g_DeviceInput.InRect( GetLeft(), GetTop(), GetLeft() + GetWidth(), GetTop() + GetHeight() ) )
	{
		bEdge = TRUE;

		if (g_DeviceInput.GetIsLeftMouseDown() || g_DeviceInput.GetIsMiddleMouseDown() || g_DeviceInput.GetIsRightMouseDown() ||
			g_DeviceInput.GetIsLeftMousePress() || g_DeviceInput.GetIsMiddleMousePress() || g_DeviceInput.GetIsRightMousePress() ||
			g_DeviceInput.GetIsLeftMouseUp() || g_DeviceInput.GetIsMiddleMouseUp() || g_DeviceInput.GetIsRightMouseUp())
		{
			bClick = TRUE;
		}
	}
	if( m_pSlotOne->GetVisible() )	((CRYLStatusExtensionSlotOne*)m_pSlotOne)->Update( bClick, bEdge ) ;
	if( m_pSlotTwo->GetVisible() )	((CRYLStatusExtensionSlotTwo*)m_pSlotTwo)->Update( bClick, bEdge ) ;
    

	char			strString[MAX_PATH];
	CRYLGameData*	pGame = CRYLGameData::Instance() ;

	// ------------------------------------------------------------------------------------------
	// HP/MP bar

	sprintf(strString, "%d / %d", 
		static_cast<unsigned short>(pGame->m_csStatus.m_Info.HP), 
		pGame->m_csStatus.GetMaxHP() );
	m_pHPLebel->SetCaption( strString ) ;
	
	sprintf(strString, "%d / %d", 
		static_cast<unsigned short>(pGame->m_csStatus.m_Info.MP), 
		pGame->m_csStatus.GetMaxMP() );
	m_pMPLebel->SetCaption( strString ) ;

	m_pHPProgressBar->SetMax( pGame->m_csStatus.GetMaxHP() ) ;
    if( static_cast<unsigned short>(pGame->m_csStatus.m_Info.HP) < pGame->m_csStatus.GetMaxHP() )
    {   
        m_pHPProgressBar->SetPosition( static_cast<unsigned short>(pGame->m_csStatus.m_Info.HP) ) ;
    }
    else
    { 
        m_pHPProgressBar->SetPosition( pGame->m_csStatus.GetMaxHP() ) ;
    }

	m_pMPProgressBar->SetMax( pGame->m_csStatus.GetMaxMP() ) ;
    if( static_cast<unsigned short>(pGame->m_csStatus.m_Info.MP) < pGame->m_csStatus.GetMaxMP() )
    {   
        m_pMPProgressBar->SetPosition( static_cast<unsigned short>(pGame->m_csStatus.m_Info.MP) ) ;
    }
    else
    { 
        m_pMPProgressBar->SetPosition( pGame->m_csStatus.GetMaxMP() ) ;
    }

	// ------------------------------------------------------------------------------------------
	// EXP bar

	unsigned long dwExp, dwExpMax , type;


	type = 1;
	if (1000000 < pGame->m_csStatus.m_llExpMax && pGame->m_csStatus.m_llExpMax <= 1000000000)
	{
		dwExpMax = pGame->m_csStatus.m_llExpMax / 1000;
		type = 1000;
	} 
	else if (1000000000 < pGame->m_csStatus.m_llExpMax && pGame->m_csStatus.m_llExpMax <= 1000000000000)
	{
		dwExpMax = pGame->m_csStatus.m_llExpMax / 1000000;
		type = 1000000;
	} 
	else if (1000000000000 < pGame->m_csStatus.m_llExpMax && pGame->m_csStatus.m_llExpMax <= 1000000000000000)
	{
		dwExpMax = pGame->m_csStatus.m_llExpMax / 1000000000;
		type = 1000000000;
	} 
	else
	{
		dwExpMax = pGame->m_csStatus.m_llExpMax;
		type = 1;
	}

	dwExp = pGame->m_csStatus.m_Info.Exp / type;


	m_pExpProgressBar->SetMax( dwExpMax ) ;
	m_pExpProgressBar->SetPosition( dwExp ) ;

	CHAR szBuff[ 8 ] ;
	sprintf( szBuff, "%d", pGame->m_csStatus.m_Info.Level ) ;
	m_pLevelLabel->SetCaption( szBuff ) ;


	// EXP bar LABEL
//	char strExp[2], strExpMax[2];
    sprintf(strString, "%4.2f %%", static_cast<float>( (double)pGame->m_csStatus.m_Info.Exp / (double)pGame->m_csStatus.m_llExpMax * 100.0f ) );
	m_pExperienceBalueLabel->SetCaption( strString );

/*
	// NATION LABEL
	if ( Creature::MERKADIA == pGame->m_cAccountNation )
	{
		m_pNationLabel->SetCaption( "MK" ) ;
		m_pNationLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 2286 ], 1812, TOOLTIP_PANEL ) ;
	}
	else if ( Creature::KARTERANT == pGame->m_cAccountNation )
	{
		m_pNationLabel->SetCaption( "KR" ) ;
		m_pNationLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 2285 ], 1812, TOOLTIP_PANEL ) ;
	}
	else if ( Creature::ALMIGHTY_PIRATE == pGame->m_cAccountNation )	// 신의 해적단
	{
		m_pNationLabel->SetCaption( "AP" ) ;
		m_pNationLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 2287 ], 1812, TOOLTIP_PANEL ) ;
	}
*/


	// DAMAGE LABEL
	sprintf(strString, "%4d ~ %4d", pGame->m_csStatus.GetMinDamage(), pGame->m_csStatus.GetMaxDamage());
	long m_lDamage = pGame->m_csStatus.GetMinDamage() + ( pGame->m_csStatus.GetMaxDamage() - pGame->m_csStatus.GetMinDamage()) / 2;
	long m_lRealDamage = pGame->m_csStatus.GetBaseMinDamage() + ( pGame->m_csStatus.GetBaseMaxDamage() - pGame->m_csStatus.GetBaseMinDamage()) / 2;
	if(m_lDamage < m_lRealDamage)
	{
		m_pStrikingPowerLabel->SetCaption( strString ) ;
		m_pStrikingPowerLabel->SetFontColor( 0xffff0000 ) ;
	} else if(m_lDamage > m_lRealDamage)
	{
		m_pStrikingPowerLabel->SetCaption( strString ) ;
		m_pStrikingPowerLabel->SetFontColor( 0xff00ff00 ) ;
	} else
	{
		m_pStrikingPowerLabel->SetCaption( strString ) ;
		m_pStrikingPowerLabel->SetFontColor( 0xffffffff ) ;
	}


	// ACCUARY LABEL	
	sprintf( strString, "%4d", pGame->m_csStatus.GetHitRate() ) ;
	m_pHitRatioLabel->SetCaption( strString ) ;

	sprintf( strString,  CRYLStringTable::m_strString[ 2280 ], pGame->m_csStatus.GetHitRate(), pGame->m_csStatus.GetHitRate()/2, pGame->m_csStatus.GetHitRate()/3 ) ;
	m_pHitRatioLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, strString, 1812, TOOLTIP_PANEL ) ;

	if( pGame->m_csStatus.GetHitRate() < pGame->m_csStatus.GetBaseHitRate() )		
	{
		m_pHitRatioLabel->SetFontColor( 0xffff0000 );
	}
	else if ( pGame->m_csStatus.GetHitRate() > pGame->m_csStatus.GetBaseHitRate() )	
	{
		m_pHitRatioLabel->SetFontColor( 0xff00ff00 );
	}
	else
	{
		m_pHitRatioLabel->SetFontColor( 0xffffffff );
	}
/*

	// CRITICAL LABEL
	sprintf(strString, "%3d", pGame->m_csStatus.GetCritical() ) ;
	m_pCriticalLabel->SetCaption( strString ) ;
	sprintf(strString,  CRYLStringTable::m_strString[ 2282 ], pGame->m_csStatus.GetCriticalRate(), pGame->m_csStatus.GetCriticalMagnification() ) ;
	m_pCriticalLabel->SetTooltip(  &m_pGMUIParentDlg->m_TooltipManager,  strString, 1792, TOOLTIP_PANEL ) ;

	if( pGame->m_csStatus.GetCritical() < pGame->m_csStatus.GetBaseCritical() )	
	{
		m_pCriticalLabel->SetFontColor( 0xffff0000 );
	}
	else if ( pGame->m_csStatus.GetCritical() > pGame->m_csStatus.GetBaseCritical() )	
	{
		m_pCriticalLabel->SetFontColor( 0xff00ff00 );
	}
	else
	{
		m_pCriticalLabel->SetFontColor( 0xffffffff );
	}
*/

	// DEFENCE LABEL
	sprintf(strString, "%4d", pGame->m_csStatus.GetArmor());
	if ( pGame->m_csStatus.GetArmor() < pGame->m_csStatus.GetBaseArmor() )
	{
		m_pDefensivePowerLabel->SetCaption( strString ) ;
		m_pDefensivePowerLabel->SetFontColor( 0xffff0000 ) ;

		m_pDefensivePowerLabel->SetFontColor( 0xffff0000 ) ;
	} 
	else if( pGame->m_csStatus.GetArmor() > pGame->m_csStatus.GetBaseArmor() )
	{
		m_pDefensivePowerLabel->SetCaption( strString ) ;
		m_pDefensivePowerLabel->SetFontColor( 0xff00ff00 ) ;

		m_pDefensivePowerLabel->SetFontColor( 0xff00ff00 ) ;
	} 
	else
	{
		m_pDefensivePowerLabel->SetCaption( strString ) ;
		m_pDefensivePowerLabel->SetFontColor( 0xffffffff ) ;

		m_pDefensivePowerLabel->SetFontColor( 0xffffffff ) ;
	}



	// MAGIC DEFENCE LABEL
	sprintf(strString, "%4d", pGame->m_csStatus.GetMagicResist() ) ;
	m_pMagicResistLabel->SetCaption( strString ) ;

	sprintf(strString, CRYLStringTable::m_strString[ 2284 ], pGame->m_csStatus.GetMagicResistRate() ) ;
	m_pMagicResistLabel->SetTooltip(  &m_pGMUIParentDlg->m_TooltipManager,  strString, 1792, TOOLTIP_PANEL ) ;

	if( pGame->m_csStatus.GetMagicResist() < pGame->m_csStatus.GetBaseMagicResist() )	
	{
		m_pMagicResistLabel->SetFontColor( 0xffff0000 );
	}
	else if ( pGame->m_csStatus.GetMagicResist() > pGame->m_csStatus.GetBaseMagicResist() )	
	{
		m_pMagicResistLabel->SetFontColor( 0xff00ff00 );
	}
	else
	{
		m_pMagicResistLabel->SetFontColor( 0xffffffff );
	}
/*

	// EVASION LABEL
	sprintf( strString, "%4d", pGame->m_csStatus.GetEvade() ) ;
	m_pEvasionLabel->SetCaption( strString ) ;

	sprintf( strString,  CRYLStringTable::m_strString[ 2281 ], pGame->m_csStatus.GetEvade(), pGame->m_csStatus.GetEvade()*2, pGame->m_csStatus.GetEvade()*3 ) ;
	m_pEvasionLabel->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, strString, 1812, TOOLTIP_PANEL ) ;

	if( pGame->m_csStatus.GetEvade() < pGame->m_csStatus.GetBaseEvade() )			
	{
		m_pEvasionLabel->SetFontColor( 0xffff0000 );
	}
	else if ( pGame->m_csStatus.GetEvade() > pGame->m_csStatus.GetBaseEvade() )		
	{
		m_pEvasionLabel->SetFontColor( 0xff00ff00 );
	}
	else
	{
		m_pEvasionLabel->SetFontColor( 0xffffffff );
	}


	// BLOCK LABEL
	sprintf(strString, "%4d", pGame->m_csStatus.GetBlock() );
	m_pBlockPowerLabel->SetCaption( strString ) ;

	if ( pGame->m_csStatus.GetBlock() < pGame->m_csStatus.GetBaseBlock() )
	{
		m_pBlockPowerLabel->SetFontColor( 0xffff0000 );
	} 
	else if( pGame->m_csStatus.GetBlock() > pGame->m_csStatus.GetBaseBlock() )
	{
		m_pBlockPowerLabel->SetFontColor( 0xff00ff00 );
	} 
	else
	{
		m_pBlockPowerLabel->SetFontColor( 0xffffffff );
	}

	sprintf(strString, CRYLStringTable::m_strString[ 2283 ], pGame->m_csStatus.GetBlockRate());
	m_pBlockPowerLabel->SetTooltip(  &m_pGMUIParentDlg->m_TooltipManager,  strString, 1792, TOOLTIP_PANEL ) ;


    // MAGIC POWER
	sprintf( strString, "%4d", pGame->m_csStatus.GetMagicPower() );
	m_pMagicPowerLabel->SetCaption( strString );

	if( pGame->m_csStatus.GetMagicPower() < pGame->m_csStatus.GetBaseMagicPower() )	
	{
		m_pMagicPowerLabel->SetFontColor( 0xffff0000 );
	}
	else if ( pGame->m_csStatus.GetMagicPower() > pGame->m_csStatus.GetBaseMagicPower() )	
	{
		m_pMagicPowerLabel->SetFontColor( 0xff00ff00 );
	}
	else
	{
		m_pMagicPowerLabel->SetFontColor( 0xffffffff );
	}


	// HP-REC LABEL
	sprintf(strString, "%d", pGame->m_csStatus.GetHPRegen());
	m_pHPRECLabel->SetCaption( strString ) ;
	if ( pGame->m_csStatus.GetHPRegen() < pGame->m_csStatus.GetBaseHPRegen() )
		m_pHPRECLabel->SetFontColor( 0xffff0000 ) ;
	else if( pGame->m_csStatus.GetHPRegen() > pGame->m_csStatus.GetBaseHPRegen() )
		m_pHPRECLabel->SetFontColor( 0xff00ff00 ) ;
	else
		m_pHPRECLabel->SetFontColor( 0xffffffff ) ;

	*/
	return S_OK ;
}


VOID
// EntensionLevel에 따라서 슬롯 (비)활성화 시키는 함수
CRYLStatusDlg::UpdateExtension()
{
	INT		yy, height, dy;
	yy = GetTop() ;
	height = g_ClientMain.m_iScreenHeight ;
	dy = 0;
	
	switch( m_nExtensionLevel )
	{
	case 0 :
////		m_pSlotOne->SetVisible( FALSE );
//		m_pSlotTwo->SetVisible( FALSE );
////		dy = m_nHeight;
		break;
	case 1: 
////		m_pSlotOne->SetVisible( TRUE );
//		m_pSlotTwo->SetVisible( FALSE );
////		dy = m_nHeight + m_pSlotOne->GetHeight() - 1;
		break;


	case 2:
////	m_pSlotOne->SetVisible( TRUE );
//		m_pSlotTwo->SetVisible( TRUE );
////		dy = m_nHeight + m_pSlotOne->GetHeight() + m_pSlotTwo->GetHeight() - 2;
		break;

	default:
////		m_pSlotOne->SetVisible( FALSE );
//		m_pSlotTwo->SetVisible( FALSE );
		break;


	}
	
	SetHeight( dy ) ;
	if( height < yy + dy )
	{
		SetTop( height - dy  );
	}

}


VOID	CRYLStatusDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y )
{
	if ( !g_pStatusDlg )
		return ;

	switch( Sender->GetInstance() )
	{
		case RYLSTATUSDLG::HIDE_BUTTON :
			{
				g_pStatusDlg->SetVisible( FALSE ) ;
				
				// Visible Flag Setting
				CRYLGameScene* pScene = static_cast<CRYLGameScene*>( CRYLSceneManager::Instance()->GetCurrentScene() ) ;
				if ( pScene ) pScene->SetVisibleInfo( RYLMAINWINDOW::STATUS_WINDOW, FALSE ) ;

			}
			break;

/*		case RYLSTATUSDLG::EXTENSION_BUTTON :
			{
				g_pStatusDlg->m_nExtensionLevel = ( g_pStatusDlg->m_nExtensionLevel + 1) % 3;
				g_pStatusDlg->UpdateExtension();
			}
			break ;  */

	}
}