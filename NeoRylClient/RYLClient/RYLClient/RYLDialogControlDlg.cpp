#include "RYLClientMain.h"
#include "RYLDialogControlDlg.h"
#include "RYLButton.h"
#include "RYLImageManager.h"
#include "RYLImageCabinet.h"
#include "RYLSpriteList.h"

#include "RYLSceneManager.h"
#include "RYLGameScene.h"
#include "RYLStatusAndQuestFrameDlg.h"
#include "RYLSocietyFrameDlg.h"
#include "RYLGuildFrameDlg.h"
#include "RYLSystemDlg.h"
#include "WinInput.h"
#include "RYLMiniMapDlg.h"
#include "RYLStringTable.h"

#include "GMUITooltipBox.h"

#include "RYLImage.h"
#include "GMImageDib.h"

#include "RYLGameData.h"
#include "RYLHumanInventoryFrameDlg.h"

#include "RYLAnimationButton.h"
#include "GMMemory.h"

#include <Creature/EnemyCheck.h>

// add by shadow
#include "RYLVaultDlg.h"
#include "RYLVaultPasswordDlg.h"
#include "RYLEdit.h"
#include "RYLNetworkData.h"
#include <Network/ClientSocket/Send/SendItem.h>
#include "RYLBlacksmithDlg.h"
#include "RYLGameData.h"

#include "RYLRaceBaseDefine.h"
#include "RYLRaceBase.h" // RYLClientMain.h    RYLHumanInventoryFrameDlg.h
#include "RYLClientMain.h"
#include "RYLHumanInventoryFrameDlg.h"

CRYLDialogControlDlg*	g_pDialogControlDlg = NULL ;

CRYLDialogControlDlg::CRYLDialogControlDlg( INT nLeft, INT nTop, BOOL bVert ) : CRYLDialog()
{
	m_pMapButton		= NULL ;			// 멥 버튼
	m_pStatusButton		= NULL ;			// 스탯 버튼
	m_pSocietyButton	= NULL ;			// 사회 버튼
	m_pInvenButton		= NULL ;			// 인벤토리 버튼
	m_pGuildButton		= NULL ;			// 길드 버튼
	//m_pBattleButton		= NULL ;			// 전장 버튼
//	m_pGMReportButton		= NULL ;			// 소셜 버튼
	m_pHelpButton		= NULL ;			// 헬프 버튼
	m_pOptionButton		= NULL ;			// 옵션 버튼
	m_pRotationButton	= NULL ;			// 회전 버튼
	m_pSiegeTimeButton	= NULL ;

	RECT rt ;
	SetRect( &rt, 0, 98, 245, 139 ) ;
	AttachWindowTexture( ( CGMTexture*)CRYLSpriteList::Instance()->GetImage( &rt, ERYLSPRITENAME::RYL_MAINGUI_256x256 ) ) ;

	CGMImageDib*	pImageDIB = NULL ;
	CRYLSpriteList::Instance()->CreateGMImage( 41, 268, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 0, 41, 256 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI02_256x256 ) ) ;
	SetRect( &rt, 125, 0, 166, 12 ) ;
	pImageDIB->DrawRect( &rt, 0, 256, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI02_256x256 ) ) ;
	
	m_pVertImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	m_pVertImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( m_pVertImage ) ;
	GM_DELETE( pImageDIB ) ;

	
	SetWidth( rt.right - rt.left ) ;
	SetHeight( rt.bottom - rt.top ) ;
	
	SetLeft( nLeft ) ;
	SetTop( nTop ) ;
	
	SetFormType( RYLDLG_TYPE_NONE ) ;
	
	g_pDialogControlDlg = this ;	
	InitCtrl() ;

	m_bVertEnable = FALSE ;
	if ( bVert ) RotationDlg() ;
}

CRYLDialogControlDlg::~CRYLDialogControlDlg ()
{
	FinalCtrl() ;
}

VOID	CRYLDialogControlDlg::InitCtrl ()
{
	RECT rt ;
	CGMImageDib* pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI_256x256 ) ;
	
	// 멥 버튼
	m_pMapButton = new CRYLButton( this ) ;
	m_pMapButton->SetLeft( 2 ) ;
	m_pMapButton->SetTop( 14 ) ;
	m_pMapButton->OnClickEvent = DlgClick ;
	m_pMapButton->SetInstance( RYLDIALOGCONTROLDLG::MAP_BUTTON ) ;
	SetRect( &rt, 0, 139, 27, 164 ) ;
	m_pMapButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 0, 189, 27, 214 ) ;
	m_pMapButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 0, 164, 27, 189 ) ;
	m_pMapButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 0, 164, 27, 189 ) ;
	m_pMapButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pMapButton ) ;
	m_pMapButton->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 1772 ], 1772 ) ;
	
	// 스탯 버튼
	m_pStatusButton = new CRYLButton( this ) ;
	m_pStatusButton->SetLeft( 29 ) ;
	m_pStatusButton->SetTop( 14 ) ;
	m_pStatusButton->OnClickEvent = DlgClick ;
	m_pStatusButton->SetInstance( RYLDIALOGCONTROLDLG::STATUS_BUTTON ) ;
	SetRect( &rt, 27, 139, 54, 164 ) ;
	m_pStatusButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 27, 189, 54, 214 ) ;
	m_pStatusButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 27, 164, 54, 189 ) ;
	m_pStatusButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 27, 164, 54, 189 ) ;
	m_pStatusButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pStatusButton ) ;
	m_pStatusButton->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 1773 ], 1773 ) ;

	// 사회 버튼
	m_pSocietyButton = new CRYLButton( this ) ;
	m_pSocietyButton->SetLeft( 56 ) ;
	m_pSocietyButton->SetTop( 14 ) ;
	m_pSocietyButton->OnClickEvent = DlgClick ;
	m_pSocietyButton->SetInstance( RYLDIALOGCONTROLDLG::SOCIETY_BUTTON ) ;
	SetRect( &rt, 54, 139, 81, 164 ) ;
	m_pSocietyButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 54, 189, 81, 214 ) ;
	m_pSocietyButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 54, 164, 81, 189 ) ;
	m_pSocietyButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 54, 164, 81, 189 ) ;
	m_pSocietyButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pSocietyButton ) ;
	m_pSocietyButton->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 1774 ], 1774 ) ;

	// 인벤토리 버튼
	m_pInvenButton = new CRYLButton( this ) ;
	m_pInvenButton->SetLeft( 83 ) ;
	m_pInvenButton->SetTop( 14 ) ;
	m_pInvenButton->OnClickEvent = DlgClick ;
	m_pInvenButton->SetInstance( RYLDIALOGCONTROLDLG::INVEN_BUTTON ) ;
	SetRect( &rt, 81, 139, 108, 164 ) ;
	m_pInvenButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 81, 189, 108, 214 ) ;
	m_pInvenButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 81, 164, 108, 189 ) ;
	m_pInvenButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 81, 164, 108, 189 ) ;
	m_pInvenButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pInvenButton ) ;
	m_pInvenButton->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 1775 ], 1775 ) ;

	// 길드 버튼
	m_pGuildButton = new CRYLButton( this ) ;
	m_pGuildButton->SetLeft( 110 ) ;
	m_pGuildButton->SetTop( 14 ) ;
	m_pGuildButton->OnClickEvent = DlgClick ;
	m_pGuildButton->SetInstance( RYLDIALOGCONTROLDLG::GUILD_BUTTON ) ;
	SetRect( &rt, 108, 139, 135, 164 ) ;
	m_pGuildButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 108, 189, 135, 214 ) ;
	m_pGuildButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 108, 164, 135, 189 ) ;
	m_pGuildButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 108, 164, 135, 189 ) ;
	m_pGuildButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pGuildButton ) ;
	m_pGuildButton->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 1776 ], 1776 ) ;

	// 전장 버튼
	m_pBattleButton = new CRYLButton( this ) ;
	m_pBattleButton->SetLeft( 137 ) ;
	m_pBattleButton->SetTop( 14 ) ;
	m_pBattleButton->OnClickEvent = DlgClick ;
	m_pBattleButton->SetInstance( RYLDIALOGCONTROLDLG::BATTLE_BUTTON ) ;
	SetRect( &rt, 135, 139, 162, 164 ) ;
	m_pBattleButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 135, 189, 162, 214 ) ;
	m_pBattleButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 135, 164, 162, 189 ) ;
	m_pBattleButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 135, 164, 162, 189 ) ;
	m_pBattleButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pBattleButton ) ;
	m_pBattleButton->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[3818], 3818 ) ;	
/*
	// WASN GM RAPORT BUTTON...// NEW VAULT BUTTON
	m_pGMReportButton = new CRYLButton( this ) ;
	m_pGMReportButton->SetLeft( 164 ) ;
	m_pGMReportButton->SetTop( 14 ) ;
	m_pGMReportButton->OnClickEvent = DlgClick ;
	m_pGMReportButton->SetInstance( RYLDIALOGCONTROLDLG::GMREPORT_BUTTON ) ;
	SetRect( &rt, 90, 73, 117, 98 ) ;
	m_pGMReportButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 171, 48, 198, 73 ) ;
	m_pGMReportButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 90, 48, 117, 73 ) ;
	m_pGMReportButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 90, 48, 117, 73 ) ;
	m_pGMReportButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pGMReportButton ) ;
	m_pGMReportButton->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 4902 ], 4902 ) ; // was 1779
*/
	// 헬프 버튼
	m_pHelpButton = new CRYLButton( this ) ;
	m_pHelpButton->SetLeft( 191 ) ;
	m_pHelpButton->SetTop( 14 ) ;
	m_pHelpButton->OnClickEvent = DlgClick ;
	m_pHelpButton->SetInstance( RYLDIALOGCONTROLDLG::HELP_BUTTON ) ;
	SetRect( &rt, 117, 73, 144, 98 ) ;
	m_pHelpButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 198, 48, 225, 73 ) ;
	m_pHelpButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 117, 48, 144, 73 ) ;
	m_pHelpButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 117, 48, 144, 73 ) ;
	m_pHelpButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pHelpButton ) ;
	m_pHelpButton->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 1777 ], 1777 ) ;

	// 옵션 버튼
	m_pOptionButton = new CRYLButton( this ) ;
	m_pOptionButton->SetLeft( 218 ) ;
	m_pOptionButton->SetTop( 14 ) ;
	m_pOptionButton->OnClickEvent = DlgClick ;
	m_pOptionButton->SetInstance( RYLDIALOGCONTROLDLG::OPTION_BUTTON ) ;
	SetRect( &rt, 144, 73, 171, 98 ) ;
	m_pOptionButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 225, 48, 252, 73 ) ;
	m_pOptionButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 144, 48, 171, 73 ) ;
	m_pOptionButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 144, 48, 171, 73 ) ;
	m_pOptionButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pOptionButton ) ;
	m_pOptionButton->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 1780 ], 1780 ) ;

	// 회전 버튼

	m_pRotationButton = new CRYLButton( this ) ;
	m_pRotationButton->SetLeft( 221 ) ;
	m_pRotationButton->SetTop( 3 ) ;
	m_pRotationButton->OnClickEvent = DlgClick ;
	m_pRotationButton->SetInstance( RYLDIALOGCONTROLDLG::ROTATION_BUTTON ) ;
	SetRect( &rt, 212, 0, 222, 9 ) ;
	m_pRotationButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 212, 18, 222, 27 ) ;
	m_pRotationButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 212, 9, 222, 18 ) ;
	m_pRotationButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 212, 9, 222, 18 ) ;
	m_pRotationButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pRotationButton->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 1870 ], 1870 ) ;
	AttachGMUIControl( m_pRotationButton ) ;

	// 애니메이션 버튼.

	m_pSiegeTimeButton = new CRYLAnimationButton( this ) ;
	m_pSiegeTimeButton->SetLeft(137) ;
	m_pSiegeTimeButton->SetTop(14) ;
	m_pSiegeTimeButton->OnClickEvent = DlgClick ;
	m_pSiegeTimeButton->SetInstance( RYLDIALOGCONTROLDLG::SIEGETIME_BUTTON ) ;	
	
	pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI03_256x256 ) ;
	
	for(unsigned char cIndex = 0; cIndex < 9; cIndex++)
	{
		SetRect(&rt, 27*cIndex, 50, 27*(cIndex+1), 75) ;
		m_pSiegeTimeButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;

		SetRect(&rt, 27*cIndex, 0, 27*(cIndex+1), 25) ;
		m_pSiegeTimeButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP) ;
	}

	for(unsigned char cIndex = 0; cIndex < 3; cIndex++)
	{
		SetRect(&rt, 81+27*cIndex, 25, 81+27*(cIndex+1), 50) ;
		m_pSiegeTimeButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;

		SetRect(&rt, 27*cIndex, 25, 27*(cIndex+1), 50) ;
		m_pSiegeTimeButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP) ;
	}
	
	for(unsigned char cIndex = 0; cIndex < 9; cIndex++)
	{
		SetRect(&rt, 27*cIndex, 125, 27*(cIndex+1), 150) ;
		m_pSiegeTimeButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;

		SetRect(&rt, 27*cIndex, 75, 27*(cIndex+1), 100) ;
		m_pSiegeTimeButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP) ;
	}	

	for(unsigned char cIndex = 0; cIndex < 3; cIndex++)
	{
		SetRect(&rt, 81+27*cIndex, 100, 81+27*(cIndex+1), 125) ;
		m_pSiegeTimeButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;

		SetRect(&rt, 27*cIndex, 100, 27*(cIndex+1), 125) ;
		m_pSiegeTimeButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP) ;
	}

	SetRect( &rt, 162, 25, 189, 50) ;
	m_pSiegeTimeButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 162, 100, 189, 125 ) ;
	m_pSiegeTimeButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;

	m_pSiegeTimeButton->SetFrame(0, 11, CRYLImageCabinet::EDCT_BUTTON_NORMAL);
	m_pSiegeTimeButton->SetFrame(0, 11, CRYLImageCabinet::EDCT_BUTTON_UP);
	m_pSiegeTimeButton->SetFrame(0, 0, CRYLImageCabinet::EDCT_BUTTON_DOWN);
	m_pSiegeTimeButton->SetFrameTick(100);
	m_pSiegeTimeButton->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[3818], 3818 ) ;	
	AttachGMUIControl( m_pSiegeTimeButton ) ;

	m_pSiegeTimeButton->SetVisible(FALSE);
}

VOID	CRYLDialogControlDlg::SetWarTypeButton(unsigned char cType)
{
	if(!cType)
	{
		m_pSiegeTimeButton->SetVisible(FALSE);
		m_pBattleButton->SetVisible(TRUE);		
	}
	else
	{
		m_pSiegeTimeButton->SetVisible(TRUE);
		m_pBattleButton->SetVisible(FALSE);

		if(cType==GUILD_WAR_TYPE)
		{
			m_pSiegeTimeButton->SetFrame(0, 11, CRYLImageCabinet::EDCT_BUTTON_NORMAL);
			m_pSiegeTimeButton->SetFrame(0, 11, CRYLImageCabinet::EDCT_BUTTON_UP);
			m_pSiegeTimeButton->SetFrame(0, 0, CRYLImageCabinet::EDCT_BUTTON_DOWN);
		}
		else if(cType==REALM_WAR_TYPE)
		{
			m_pSiegeTimeButton->SetFrame(12, 23, CRYLImageCabinet::EDCT_BUTTON_NORMAL);
			m_pSiegeTimeButton->SetFrame(12, 23, CRYLImageCabinet::EDCT_BUTTON_UP);
			m_pSiegeTimeButton->SetFrame(1, 1, CRYLImageCabinet::EDCT_BUTTON_DOWN);
		}
	}
}

VOID	CRYLDialogControlDlg::FinalCtrl ()
{
	GM_DELETE( m_pMapButton ) ;				// 멥 버튼
	GM_DELETE( m_pStatusButton ) ;			// 스탯 버튼
	GM_DELETE( m_pSocietyButton ) ;			// 사회 버튼
	GM_DELETE( m_pInvenButton ) ;			// 인벤토리 버튼
	GM_DELETE( m_pGuildButton ) ;			// 길드 버튼
	GM_DELETE( m_pBattleButton ) ;			// 전장 버튼
//	GM_DELETE( m_pGMReportButton ) ;			// 소셜 버튼
	GM_DELETE( m_pHelpButton ) ;			// 헬프 버튼
	GM_DELETE( m_pOptionButton ) ;			// 옵션 버튼
	GM_DELETE( m_pRotationButton ) ;		// 회전 버튼
	GM_DELETE( m_pSiegeTimeButton );		// 전장 상태 확인 버튼.
}

VOID	CRYLDialogControlDlg::Show( CGMUIObject* Sender )
{
	
}
	
VOID	CRYLDialogControlDlg::Hide( CGMUIObject* Sender )
{
	
}

VOID	CRYLDialogControlDlg::GMUIPaint( INT xx, INT yy )
{
	CRYLDialog::GMUIPaint( xx, yy ) ;
}

HRESULT		CRYLDialogControlDlg::Update( BOOL &bClick, BOOL &bEdge ) 
{
	if (FALSE == GetVisible() )
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

VOID	CRYLDialogControlDlg::RotationDlg()
{
	RECT rt ;
	CGMUITooltip* lpTooltip = NULL ;

	if ( m_bVertEnable )
	{
		
		SetWidth( 245 ) ;
		SetHeight( 41 ) ;
		SetRect( &rt, 0, 98, 245, 139 ) ;
		AttachWindowTexture( ( CGMTexture*)CRYLSpriteList::Instance()->GetImage( &rt, ERYLSPRITENAME::RYL_MAINGUI_256x256 ) ) ;
		m_pRotationButton->SetLeft( 221 ) ;
		m_pRotationButton->SetTop( 3 ) ;
		
		CGMImageDib* pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI_256x256 ) ;
		m_pMapButton->SetLeft( 2 ) ;
		m_pMapButton->SetTop( 14 ) ;	
		SetRect( &rt, 0, 139, 27, 164 ) ;
		m_pMapButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
		SetRect( &rt, 0, 189, 27, 214 ) ;
		m_pMapButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
		SetRect( &rt, 0, 164, 27, 189 ) ;
		m_pMapButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
		SetRect( &rt, 0, 164, 27, 189 ) ;
		m_pMapButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
		lpTooltip = m_TooltipManager.FindTooltip( 1772 ) ;
		if ( lpTooltip )
		{
			lpTooltip->SetRect( m_pMapButton->GetLeft(), 
								m_pMapButton->GetTop(), 
								m_pMapButton->GetLeft() + m_pMapButton->GetWidth(), 
								m_pMapButton->GetTop()  + m_pMapButton->GetHeight() ) ;
		}

		m_pStatusButton->SetLeft( 29 ) ;
		m_pStatusButton->SetTop( 14 ) ;
		SetRect( &rt, 27, 139, 54, 164 ) ;
		m_pStatusButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
		SetRect( &rt, 27, 189, 54, 214 ) ;
		m_pStatusButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
		SetRect( &rt, 27, 164, 54, 189 ) ;
		m_pStatusButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
		SetRect( &rt, 27, 164, 54, 189 ) ;
		m_pStatusButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
		lpTooltip = m_TooltipManager.FindTooltip( 1773 ) ;
		if ( lpTooltip )
		{
			lpTooltip->SetRect( m_pStatusButton->GetLeft(), 
								m_pStatusButton->GetTop(), 
								m_pStatusButton->GetLeft() + m_pStatusButton->GetWidth(), 
								m_pStatusButton->GetTop()  + m_pStatusButton->GetHeight() ) ;
		}


		m_pSocietyButton->SetLeft( 56 ) ;
		m_pSocietyButton->SetTop( 14 ) ;
		SetRect( &rt, 54, 139, 81, 164 ) ;
		m_pSocietyButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
		SetRect( &rt, 54, 189, 81, 214 ) ;
		m_pSocietyButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
		SetRect( &rt, 54, 164, 81, 189 ) ;
		m_pSocietyButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
		SetRect( &rt, 54, 164, 81, 189 ) ;
		m_pSocietyButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
		lpTooltip = m_TooltipManager.FindTooltip( 1774 ) ;
		if ( lpTooltip )
		{
			lpTooltip->SetRect( m_pSocietyButton->GetLeft(), 
								m_pSocietyButton->GetTop(), 
								m_pSocietyButton->GetLeft() + m_pSocietyButton->GetWidth(), 
								m_pSocietyButton->GetTop()  + m_pSocietyButton->GetHeight() ) ;
		}

		m_pInvenButton->SetLeft( 83 ) ;
		m_pInvenButton->SetTop( 14 ) ;
		SetRect( &rt, 81, 139, 108, 164 ) ;
		m_pInvenButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
		SetRect( &rt, 81, 189, 108, 214 ) ;
		m_pInvenButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
		SetRect( &rt, 81, 164, 108, 189 ) ;
		m_pInvenButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
		SetRect( &rt, 81, 164, 108, 189 ) ;
		m_pInvenButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
		lpTooltip = m_TooltipManager.FindTooltip( 1775 ) ;
		if ( lpTooltip )
		{
			lpTooltip->SetRect( m_pInvenButton->GetLeft(), 
								m_pInvenButton->GetTop(), 
								m_pInvenButton->GetLeft() + m_pInvenButton->GetWidth(), 
								m_pInvenButton->GetTop()  + m_pInvenButton->GetHeight() ) ;
		}

		m_pGuildButton->SetLeft( 110 ) ;
		m_pGuildButton->SetTop( 14 ) ;
		SetRect( &rt, 108, 139, 135, 164 ) ;
		m_pGuildButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
		SetRect( &rt, 108, 189, 135, 214 ) ;
		m_pGuildButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
		SetRect( &rt, 108, 164, 135, 189 ) ;
		m_pGuildButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
		SetRect( &rt, 108, 164, 135, 189 ) ;
		m_pGuildButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
		lpTooltip = m_TooltipManager.FindTooltip( 1776 ) ;
		if ( lpTooltip )
		{
			lpTooltip->SetRect( m_pGuildButton->GetLeft(), 
								m_pGuildButton->GetTop(), 
								m_pGuildButton->GetLeft() + m_pGuildButton->GetWidth(), 
								m_pGuildButton->GetTop()  + m_pGuildButton->GetHeight() ) ;
		}

		m_pBattleButton->SetLeft( 137 ) ;
		m_pBattleButton->SetTop( 14 ) ;
		SetRect( &rt, 135, 139, 162, 164 ) ;
		m_pBattleButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
		SetRect( &rt, 135, 189, 162, 214 ) ;
		m_pBattleButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
		SetRect( &rt, 135, 164, 162, 189 ) ;
		m_pBattleButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
		SetRect( &rt, 135, 164, 162, 189 ) ;
		m_pBattleButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
		lpTooltip = m_TooltipManager.FindTooltip( 3818 ) ;
		if ( lpTooltip )
		{
			lpTooltip->SetRect( m_pBattleButton->GetLeft(), 
								m_pBattleButton->GetTop(), 
								m_pBattleButton->GetLeft() + m_pBattleButton->GetWidth(), 
								m_pBattleButton->GetTop()  + m_pBattleButton->GetHeight() ) ;
		}

		m_pSiegeTimeButton->SetLeft( 137 ) ;
		m_pSiegeTimeButton->SetTop( 14 ) ;

		lpTooltip = m_TooltipManager.FindTooltip( 3818 ) ;
		if ( lpTooltip )
		{
			lpTooltip->SetRect( m_pSiegeTimeButton->GetLeft(), 
				m_pSiegeTimeButton->GetTop(), 
				m_pSiegeTimeButton->GetLeft() + m_pSiegeTimeButton->GetWidth(), 
				m_pSiegeTimeButton->GetTop()  + m_pSiegeTimeButton->GetHeight() ) ;
		}
/*
		m_pGMReportButton->SetLeft( 164 ) ;
		m_pGMReportButton->SetTop( 14 ) ;
		SetRect( &rt, 90, 73, 117, 98 ) ;
		m_pGMReportButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
		SetRect( &rt, 171, 48, 198, 73 ) ;
		m_pGMReportButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
		SetRect( &rt, 90, 48, 117, 73 ) ;
		m_pGMReportButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
		SetRect( &rt, 90, 48, 117, 73 ) ;
		m_pGMReportButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
		lpTooltip = m_TooltipManager.FindTooltip( 1778 ) ;
		if ( lpTooltip )
		{
			lpTooltip->SetRect( m_pGMReportButton->GetLeft(), 
								m_pGMReportButton->GetTop(), 
								m_pGMReportButton->GetLeft() + m_pGMReportButton->GetWidth(), 
								m_pGMReportButton->GetTop()  + m_pGMReportButton->GetHeight() ) ;
		}
*/
		m_pHelpButton->SetLeft( 191 ) ;
		m_pHelpButton->SetTop( 14 ) ;
		SetRect( &rt, 117, 73, 144, 98 ) ;
		m_pHelpButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
		SetRect( &rt, 198, 48, 225, 73 ) ;
		m_pHelpButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
		SetRect( &rt, 117, 48, 144, 73 ) ;
		m_pHelpButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
		SetRect( &rt, 117, 48, 144, 73 ) ;
		m_pHelpButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
		lpTooltip = m_TooltipManager.FindTooltip( 1779 ) ;
		if ( lpTooltip )
		{
			lpTooltip->SetRect( m_pHelpButton->GetLeft(), 
								m_pHelpButton->GetTop(), 
								m_pHelpButton->GetLeft() + m_pHelpButton->GetWidth(), 
								m_pHelpButton->GetTop()  + m_pHelpButton->GetHeight() ) ;
		}

		m_pOptionButton->SetLeft( 218 ) ;
		m_pOptionButton->SetTop( 14 ) ;
		SetRect( &rt, 144, 73, 171, 98 ) ;
		m_pOptionButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
		SetRect( &rt, 225, 48, 252, 73 ) ;
		m_pOptionButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
		SetRect( &rt, 144, 48, 171, 73 ) ;
		m_pOptionButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
		SetRect( &rt, 144, 48, 171, 73 ) ;
		m_pOptionButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
		lpTooltip = m_TooltipManager.FindTooltip( 1780 ) ;
		if ( lpTooltip )
		{
			lpTooltip->SetRect( m_pOptionButton->GetLeft(), 
								m_pOptionButton->GetTop(), 
								m_pOptionButton->GetLeft() + m_pOptionButton->GetWidth(), 
								m_pOptionButton->GetTop()  + m_pOptionButton->GetHeight() ) ;
		}
	}
	else
	{
		SetWidth( 41 ) ;
		SetHeight( 268 ) ;
		AttachWindowTexture( ( CGMTexture*)m_pVertImage ) ;
		m_pRotationButton->SetLeft( 18 ) ;
		m_pRotationButton->SetTop( 3 ) ;

		CGMImageDib* pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI02_256x256 ) ;
		m_pMapButton->SetLeft( 10 ) ;
		m_pMapButton->SetTop( 13 ) ;	
		SetRect( &rt, 41, 4, 69, 32 ) ;
		m_pMapButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
		SetRect( &rt, 97, 4, 125, 32 ) ;
		m_pMapButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
		SetRect( &rt, 69, 4, 97, 32 ) ;
		m_pMapButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
		SetRect( &rt, 69, 4, 97, 32 ) ;
		m_pMapButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
		lpTooltip = m_TooltipManager.FindTooltip( 1772 ) ;
		if ( lpTooltip )
		{
			lpTooltip->SetRect( m_pMapButton->GetLeft(), 
								m_pMapButton->GetTop(), 
								m_pMapButton->GetLeft() + m_pMapButton->GetWidth(), 
								m_pMapButton->GetTop()  + m_pMapButton->GetHeight() ) ;
		}

		m_pStatusButton->SetLeft( 10 ) ;
		m_pStatusButton->SetTop( 41 ) ;
		SetRect( &rt, 41, 32, 69, 60 ) ;
		m_pStatusButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
		SetRect( &rt, 97, 32, 125, 60 ) ;
		m_pStatusButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
		SetRect( &rt, 69, 32, 97, 60 ) ;
		m_pStatusButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
		SetRect( &rt, 69, 32, 97, 60 ) ;
		m_pStatusButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
		lpTooltip = m_TooltipManager.FindTooltip( 1773 ) ;
		if ( lpTooltip )
		{
			lpTooltip->SetRect( m_pStatusButton->GetLeft(), 
								m_pStatusButton->GetTop(), 
								m_pStatusButton->GetLeft() + m_pStatusButton->GetWidth(), 
								m_pStatusButton->GetTop()  + m_pStatusButton->GetHeight() ) ;
		}

		m_pSocietyButton->SetLeft( 10 ) ;
		m_pSocietyButton->SetTop( 69 ) ;
		SetRect( &rt, 41, 60, 69, 88 ) ;
		m_pSocietyButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
		SetRect( &rt, 97, 60, 125, 88 ) ;
		m_pSocietyButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
		SetRect( &rt, 69, 60, 97, 88 ) ;
		m_pSocietyButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
		SetRect( &rt, 69, 60, 97, 88 ) ;
		m_pSocietyButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
		lpTooltip = m_TooltipManager.FindTooltip( 1774 ) ;
		if ( lpTooltip )
		{
			lpTooltip->SetRect( m_pSocietyButton->GetLeft(), 
								m_pSocietyButton->GetTop(), 
								m_pSocietyButton->GetLeft() + m_pSocietyButton->GetWidth(), 
								m_pSocietyButton->GetTop()  + m_pSocietyButton->GetHeight() ) ;
		}

		m_pInvenButton->SetLeft( 10 ) ;
		m_pInvenButton->SetTop( 97 ) ;
		SetRect( &rt, 41, 88, 69, 116 ) ;
		m_pInvenButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
		SetRect( &rt, 97, 88, 125, 116 ) ;
		m_pInvenButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
		SetRect( &rt, 69, 88, 97, 116 ) ;
		m_pInvenButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
		SetRect( &rt, 69, 88, 97, 116 ) ;
		m_pInvenButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
		lpTooltip = m_TooltipManager.FindTooltip( 1775 ) ;
		if ( lpTooltip )
		{
			lpTooltip->SetRect( m_pInvenButton->GetLeft(), 
								m_pInvenButton->GetTop(), 
								m_pInvenButton->GetLeft() + m_pInvenButton->GetWidth(), 
								m_pInvenButton->GetTop()  + m_pInvenButton->GetHeight() ) ;
		}

		m_pGuildButton->SetLeft( 10 ) ;
		m_pGuildButton->SetTop( 125 ) ;
		SetRect( &rt, 41, 116, 69, 144 ) ;
		m_pGuildButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
		SetRect( &rt, 97, 116, 125, 144 ) ;
		m_pGuildButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
		SetRect( &rt, 69, 116, 97, 144 ) ;
		m_pGuildButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
		SetRect( &rt, 69, 116, 97, 144 ) ;
		m_pGuildButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
		lpTooltip = m_TooltipManager.FindTooltip( 1776 ) ;
		if ( lpTooltip )
		{
			lpTooltip->SetRect( m_pGuildButton->GetLeft(), 
								m_pGuildButton->GetTop(), 
								m_pGuildButton->GetLeft() + m_pGuildButton->GetWidth(), 
								m_pGuildButton->GetTop()  + m_pGuildButton->GetHeight() ) ;
		}

		m_pBattleButton->SetLeft( 10 ) ;
		m_pBattleButton->SetTop( 153 ) ;
		SetRect( &rt, 41, 144, 69, 172 ) ;
		m_pBattleButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
		SetRect( &rt, 97, 144, 125, 172 ) ;
		m_pBattleButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
		SetRect( &rt, 69, 144, 97, 172 ) ;
		m_pBattleButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
		SetRect( &rt, 69, 144, 97, 172 ) ;
		m_pBattleButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
		lpTooltip = m_TooltipManager.FindTooltip( 3818 ) ;
		if ( lpTooltip ) 
		{
			lpTooltip->SetRect( m_pBattleButton->GetLeft(), 
								m_pBattleButton->GetTop(), 
								m_pBattleButton->GetLeft() + m_pBattleButton->GetWidth(), 
								m_pBattleButton->GetTop()  + m_pBattleButton->GetHeight() ) ;
		}

		m_pSiegeTimeButton->SetLeft( 10 ) ;
		m_pSiegeTimeButton->SetTop( 153 ) ;

		lpTooltip = m_TooltipManager.FindTooltip( 3818 ) ;

		if ( lpTooltip ) 
		{
			lpTooltip->SetRect( m_pSiegeTimeButton->GetLeft(), 
			m_pSiegeTimeButton->GetTop(), 
			m_pSiegeTimeButton->GetLeft() + m_pSiegeTimeButton->GetWidth(), 
			m_pSiegeTimeButton->GetTop()  + m_pSiegeTimeButton->GetHeight() ) ;
		}
/*
		m_pGMReportButton->SetLeft( 10 ) ;
		m_pGMReportButton->SetTop( 181 ) ;
		SetRect( &rt, 41, 172, 69, 200 ) ;
		m_pGMReportButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
		SetRect( &rt, 97, 172, 125, 200 ) ;
		m_pGMReportButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
		SetRect( &rt, 69, 172, 97, 200 ) ;
		m_pGMReportButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
		SetRect( &rt, 69, 172, 97, 200 ) ;
		m_pGMReportButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
		lpTooltip = m_TooltipManager.FindTooltip( 1778 ) ;
		if ( lpTooltip )
		{
			lpTooltip->SetRect( m_pGMReportButton->GetLeft(), 
								m_pGMReportButton->GetTop(), 
								m_pGMReportButton->GetLeft() + m_pGMReportButton->GetWidth(), 
								m_pGMReportButton->GetTop()  + m_pGMReportButton->GetHeight() ) ;
		}
*/
		m_pHelpButton->SetLeft( 10 ) ;
		m_pHelpButton->SetTop( 209 ) ;
		SetRect( &rt, 41, 200, 69, 228 ) ;
		m_pHelpButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
		SetRect( &rt, 97, 200, 125, 228 ) ;
		m_pHelpButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
		SetRect( &rt, 69, 200, 97, 228 ) ;
		m_pHelpButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
		SetRect( &rt, 69, 200, 97, 228 ) ;
		m_pHelpButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
		lpTooltip = m_TooltipManager.FindTooltip( 1779 ) ;
		if ( lpTooltip )
		{
			lpTooltip->SetRect( m_pHelpButton->GetLeft(), 
								m_pHelpButton->GetTop(), 
								m_pHelpButton->GetLeft() + m_pHelpButton->GetWidth(), 
								m_pHelpButton->GetTop()  + m_pHelpButton->GetHeight() ) ;
		}

		m_pOptionButton->SetLeft( 10 ) ;
		m_pOptionButton->SetTop( 235 ) ;
		SetRect( &rt, 41, 228, 69, 256 ) ;
		m_pOptionButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
		SetRect( &rt, 97, 228, 125, 256 ) ;
		m_pOptionButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
		SetRect( &rt, 69, 228, 97, 256 ) ;
		m_pOptionButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
		SetRect( &rt, 69, 228, 97, 256 ) ;
		m_pOptionButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
		lpTooltip = m_TooltipManager.FindTooltip( 1780 ) ;
		if ( lpTooltip )
		{
			lpTooltip->SetRect( m_pOptionButton->GetLeft(), 
								m_pOptionButton->GetTop(), 
								m_pOptionButton->GetLeft() + m_pOptionButton->GetWidth(), 
								m_pOptionButton->GetTop()  + m_pOptionButton->GetHeight() ) ;
		}
	}
	m_bVertEnable = !m_bVertEnable ;
}

VOID	CRYLDialogControlDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y )
{
	CRYLGameScene* pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
	if (!g_pDialogControlDlg || pScene->GetCastleDoorblockDlg()->GetVisible() || pScene->GetGuildWarPenaltyDlg()->GetVisible() )
		return ;

	RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();

	if(EnemyCheck::IsSiegeArms(pSelfCreature->GetObjectType()) || EnemyCheck::IsCastleArms(pSelfCreature->GetObjectType()))
		return;

	switch( Sender->GetInstance() )
	{
		case RYLDIALOGCONTROLDLG::MAP_BUTTON :
		{
			CRYLGameScene* pScene = static_cast<CRYLGameScene*>( CRYLSceneManager::GetCurrentScene() ) ;
			if ( pScene )
			{
				pScene->GetMiniMapDlg()->SetVisible( !pScene->GetMiniMapDlg()->GetVisible() ) ;

				// Visible Flag Setting
				pScene->SetVisibleInfo( RYLMAINWINDOW::MINIMAP_WINDOW, pScene->GetMiniMapDlg()->GetVisible() ) ;
			}
			break ;
		}

		case RYLDIALOGCONTROLDLG::STATUS_BUTTON :
			( ( CRYLGameScene* )CRYLSceneManager::GetCurrentScene() )->GetStatusAndQuestFrameDlg()->SetVisible( 
				!( ( CRYLGameScene* )CRYLSceneManager::GetCurrentScene() )->GetStatusAndQuestFrameDlg()->GetVisible() ) ;
			break ;

		case RYLDIALOGCONTROLDLG::SOCIETY_BUTTON :
			( ( CRYLGameScene* )CRYLSceneManager::GetCurrentScene() )->GetSocietyFrameDlg()->SetVisible( 
				!( ( CRYLGameScene* )CRYLSceneManager::GetCurrentScene() )->GetSocietyFrameDlg()->GetVisible() ) ;
			break ;

		case RYLDIALOGCONTROLDLG::INVEN_BUTTON :
		{
			CRYLGameScene*				pScene			= ( CRYLGameScene * )CRYLSceneManager::Instance()->GetCurrentScene() ;
			CRYLHumanInventoryFrameDlg* pInventoryDlg	= ( CRYLHumanInventoryFrameDlg* )pScene->GetHumanInventoryFrameDlg() ;
			pInventoryDlg->ShowInventory() ;
			pInventoryDlg->SetVisible( !pInventoryDlg->GetVisible() ) ;

			// Simple Mouse Mode 로 변경
			CRYLGameData*	pGame = CRYLGameData::Instance() ;
			if ( pGame && !pGame->m_bMouseMode )
			{
				if ( !pScene->GetStatusAndQuestFrameDlg()->GetVisible() && !pScene->GetSocietyFrameDlg()->GetVisible() &&
					 !pScene->GetGuildFrameDlg()->GetVisible() && !pScene->GetLargeMapDlg()->GetVisible() &&
					 !pScene->GetHumanInventoryFrameDlg()->GetVisible() && !pScene->GetSystemDlg()->GetVisible() &&
					 !pScene->GetBlacksmithDlg()->GetVisible() && !pScene->GetExchangeDlg()->GetVisible() &&
					 !pScene->GetJobChangeDlg()->GetVisible() && !pScene->GetOptionDlg()->GetVisible() &&
					 !pScene->GetQuestDetailDataDlg()->GetVisible() && !pScene->GetQuestOpeningDlg()->GetVisible() &&
					 !pScene->GetRankingDlg()->GetVisible() && !pScene->GetStatusRetrainDlg()->GetVisible() &&
					 !pScene->GetStreetStallDlg()->GetVisible() && !pScene->GetNewTradeDlg()->GetVisible() &&
					 !pScene->GetVaultCertificationdDlg()->GetVisible() && !pScene->GetVaultDlg()->GetVisible() &&
					 !pScene->GetVaultPasswordDlg()->GetVisible() && !pScene->GetEmblemUpgradeDlg()->GetVisible() )
				{
					pGame->m_bSimpleMouse = FALSE ;
				}
				else 
				{
					pGame->m_bSimpleMouse = TRUE ;
				}
			}
			break ;
		}

		case RYLDIALOGCONTROLDLG::GUILD_BUTTON :
			pScene->GUIVisible( FALSE ); // Minotaurs 길드창띄우기
			( ( CRYLGameScene* )CRYLSceneManager::GetCurrentScene() )->GetGuildFrameDlg()->SetVisible( 
				!( ( CRYLGameScene* )CRYLSceneManager::GetCurrentScene() )->GetGuildFrameDlg()->GetVisible() ) ;
			break ;

	
#ifdef _DEBUG        
			( ( CRYLGameScene* )CRYLSceneManager::GetCurrentScene() )->GetGMReportDlg()->SetVisible( 
				!( ( CRYLGameScene* )CRYLSceneManager::GetCurrentScene() )->GetGMReportDlg()->GetVisible() ) ;
#endif //_DEBUG
/*
case RYLDIALOGCONTROLDLG::GMREPORT_BUTTON :
// VAUALT BUTTON 
// Start

        {
	CRYLGameScene*				pScene					= ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );
	CRYLVaultDlg*				pVaultDlg				= ( CRYLVaultDlg* )pScene->GetVaultDlg();
	CRYLVaultPasswordDlg*		pVaultPasswordDlg		= ( CRYLVaultPasswordDlg* )pScene->GetVaultPasswordDlg();
	CRYLVaultCertificationdDlg*	pVaultCertificationDlg	= ( CRYLVaultCertificationdDlg* )pScene->GetVaultCertificationdDlg();

	if ( pVaultDlg->m_bLogined )
	   {
		SendPacket::StoreLogout( g_GameSession );
	   }

	pVaultDlg->InitVault();
	pVaultPasswordDlg->m_pChangePasswordEdit->SetText( "" );
	pVaultPasswordDlg->m_pCurrentPasswordEdit->SetText( "" );

	pVaultCertificationDlg->m_pPasswordEdit->SetText( "" );
       }
		break ;
//End
*/



	//		break ;

		case RYLDIALOGCONTROLDLG::HELP_BUTTON :	
			( ( CRYLGameScene* )CRYLSceneManager::GetCurrentScene() )->GetHelpDlg()->SetVisible( 
				!( ( CRYLGameScene* )CRYLSceneManager::GetCurrentScene() )->GetHelpDlg()->GetVisible() ) ;
			break;

		case RYLDIALOGCONTROLDLG::OPTION_BUTTON :
		{
			CRYLGameScene*				pScene			= ( CRYLGameScene * )CRYLSceneManager::Instance()->GetCurrentScene() ;
			pScene->GetSystemDlg()->SetVisible( !pScene->GetSystemDlg()->GetVisible() ) ;			

			// Simple Mouse Mode 로 변경
			CRYLGameData*	pGame = CRYLGameData::Instance() ;
			if ( pGame && !pGame->m_bMouseMode && pScene->GetSystemDlg()->GetVisible() ) 
			{
				pGame->m_bSimpleMouse = TRUE ;
			}
			break ;
		}

		case RYLDIALOGCONTROLDLG::ROTATION_BUTTON :
			g_pDialogControlDlg->RotationDlg() ;
			break ;

		case RYLDIALOGCONTROLDLG::SIEGETIME_BUTTON:
		{
			// 전장 상태 확인창
			pScene->GetSiegeTimeDlg()->SetVisible(!pScene->GetSiegeTimeDlg()->GetVisible());

			// Visible Flag Setting
			pScene->SetVisibleInfo( RYLMAINWINDOW::SIEGETIME_WINDOW, pScene->GetSiegeTimeDlg()->GetVisible() ) ;
			break;
		}
	}
}
