#include <winsock2.h>
#include "RYLPartyPanel.h"
#include "RYLLabel.h"
#include "RYLSpriteEx.h"
#include "RYLButton.h"
#include "RYLStringGrid.h"
#include "RYLCheckBox.h"
#include "RYLGameData.h"
#include "RYLNetworkData.h"
#include "RYLSpriteList.h"
#include "RYLImageCabinet.h"
#include "GMImageDib.h"
#include "RYLImage.h"
#include "RYLImageManager.h"
#include "RYLGuildInfoPanel.h"
#include "RYLGuildFrameDlg.h"

#include "RYLGameScene.h"
#include "RYLSceneManager.h"
#include "RYLMiniPartyDlg.h"
#include "RYLStringTable.h"
#include "RYLSocietyFrameDlg.h"

#include <Network/ClientSocket/Send/SendCommunity.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>
#include "GMMemory.h"

#include <Creature/EnemyCheck.h>

CRYLPartyPanel*	g_pPartyPanel = NULL ;

__PARTYMEMBERINFO::__PARTYMEMBERINFO()
{
	pAutoRoutingCBox = NULL ;
	pGuildMark       = NULL ;
	iIndex			 = 0 ;
}
__PARTYMEMBERINFO::~__PARTYMEMBERINFO()
{
	GM_DELETE( pAutoRoutingCBox ) ;
	GM_DELETE( pGuildMark ) ;
}

CRYLPartyPanel::CRYLPartyPanel( CGMUICustomDlg *pParentDlg ) : CGMUICustomPanel( pParentDlg )
{
	m_pGuildMarkSprite = NULL;

	m_pGMUIParentDlg = pParentDlg ;

	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;

	RECT			rt ;

	CRYLSpriteList::Instance()->CreateGMImage( 256, 395, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	int i = 0 ;
	for ( ; i < 74 ; i ++ )
	{
		SetRect( &rt, 0, 38, 256, 43 ) ;
		pImageDIB->DrawRect( &rt, 0, i * 5, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	}
	SetRect( &rt, 0, 42, 256, 52 ) ;
	pImageDIB->DrawRect( &rt, 0, ( i - 1 ) * 5, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;

	pImageDIB->MakeFrameRect( 7,     3, 243, 223, 0xffB99E6B, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect( 7,   230, 243, 109, 0xffB99E6B, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect( 13,   27, 233,   1, 0xff959595, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect( 64,  238,  97,  19, 0xff959595, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect( 201, 238,  37,  18, 0xff959595, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect( 64,  261,  68,  19, 0xff959595, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect( 201, 261,  37,  18, 0xff959595, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect( 64,  284,  68,  18, 0xff959595, 0xc0000000,  1 ) ;
	
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	AttachPanelImage( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;
	
	SetWidth( 256 ) ;
	SetHeight( 395 ) ;

	g_pPartyPanel = this ;
	m_bAutoRouting = TRUE ;

	strcpy( m_szSelectedMember, "" ) ;
}

CRYLPartyPanel::~CRYLPartyPanel() 
{
	FinalCtrl() ;
}

VOID	CRYLPartyPanel::InitCtrl() 
{
	RECT rt ;
	CGMImageDib* pSprite			= CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;

	m_pNameLabel1 = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pNameLabel1->SetFontColor (0xffffffff);
	SetRect( &rt, 16, 11, 107, 22 ) ;
	m_pNameLabel1->SetAlignRect( rt ) ;
	m_pNameLabel1->SetAlign( CENTER ) ;
	m_pNameLabel1->SetCaption( CRYLStringTable::m_strString[ 1926 ] ) ;
	m_pNameLabel1->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pNameLabel1 ) ;
	
	m_pClassLabel1 = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pClassLabel1->SetFontColor (0xffffffff);
	SetRect( &rt, 107, 11, 175, 22 ) ;
	m_pClassLabel1->SetAlignRect( rt ) ;
	m_pClassLabel1->SetAlign( CENTER ) ;
	m_pClassLabel1->SetCaption( CRYLStringTable::m_strString[ 1928 ] ) ;
	m_pClassLabel1->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pClassLabel1 ) ;
	
	m_pLevelLabel1 = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pLevelLabel1->SetFontColor (0xffffffff);
	SetRect( &rt, 175, 11, 217, 22 ) ;
	m_pLevelLabel1->SetAlignRect( rt ) ;
	m_pLevelLabel1->SetAlign( CENTER ) ;
	m_pLevelLabel1->SetCaption( CRYLStringTable::m_strString[ 1930 ] ) ;
	m_pLevelLabel1->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pLevelLabel1 ) ;

	m_pRoutingLabel1 = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pRoutingLabel1->SetFontColor (0xffffffff);
	SetRect( &rt, 217, 11, 244, 22 ) ;
	m_pRoutingLabel1->SetAlignRect( rt ) ;
	m_pRoutingLabel1->SetAlign( CENTER ) ;
	m_pRoutingLabel1->SetCaption( CRYLStringTable::m_strString[ 1977 ] ) ;
	m_pRoutingLabel1->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pRoutingLabel1 ) ;

	m_pStringGrid = new CRYLStringGrid( m_pGMUIParentDlg, FALSE ) ;
	m_pStringGrid->SetLeft( 16 ) ;
	m_pStringGrid->SetTop( 35 ) ;
	m_pStringGrid->SetWidth( 175 ) ;
	m_pStringGrid->SetHeight( 181 ) ;
	m_pStringGrid->SetColCount( 10 ) ;
	m_pStringGrid->SetFontColor( 0xffffffff ) ;
	m_pStringGrid->SetInstance( RYLPARTYPANEL::PARTY_STRINGGRID ) ;
	m_pStringGrid->OnClickEvent = DlgClick ;
	m_pStringGrid->OnDblClickEvent = DlgDBClick ;
	CGMUICustomPanel::AddChild( m_pStringGrid ) ;

	m_pNameLabel2 = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pNameLabel2->SetFontColor (0xffffffff);
	SetRect( &rt, 10, 242, 61, 253 ) ;
	m_pNameLabel2->SetAlignRect( rt ) ;
	m_pNameLabel2->SetAlign( RIGHT ) ;
	m_pNameLabel2->SetCaption( CRYLStringTable::m_strString[ 1926 ] ) ;
	m_pNameLabel2->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pNameLabel2 ) ;
	
	m_pClassLabel2 = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pClassLabel2->SetFontColor (0xffffffff);
	SetRect( &rt, 10, 264, 61, 275 ) ;
	m_pClassLabel2->SetAlignRect( rt ) ;
	m_pClassLabel2->SetAlign( RIGHT ) ;
	m_pClassLabel2->SetCaption( CRYLStringTable::m_strString[ 1928 ] ) ;
	m_pClassLabel2->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pClassLabel2 ) ;

	m_pGuildLabel2 = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pGuildLabel2->SetFontColor (0xffffffff);
	SetRect( &rt, 165, 242, 198, 253 ) ;
	m_pGuildLabel2->SetAlignRect( rt ) ;
	m_pGuildLabel2->SetAlign( RIGHT ) ;
	m_pGuildLabel2->SetCaption( CRYLStringTable::m_strString[ 1927 ] ) ;
	m_pGuildLabel2->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pGuildLabel2 ) ;

	m_pLevelLabel2 = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pLevelLabel2->SetFontColor (0xffffffff);
	SetRect( &rt, 165, 264, 198, 275 ) ;
	m_pLevelLabel2->SetAlignRect( rt ) ;
	m_pLevelLabel2->SetAlign( RIGHT ) ;
	m_pLevelLabel2->SetCaption( CRYLStringTable::m_strString[ 1930 ] ) ;
	m_pLevelLabel2->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pLevelLabel2 ) ;

	m_pPlaceLabel2 = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pPlaceLabel2->SetFontColor (0xffffffff);
	SetRect( &rt, 10, 287, 61, 298 ) ;
	m_pPlaceLabel2->SetAlignRect( rt ) ;
	m_pPlaceLabel2->SetAlign( RIGHT ) ;
	m_pPlaceLabel2->SetCaption( CRYLStringTable::m_strString[ 1966 ] ) ;
	m_pPlaceLabel2->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pPlaceLabel2 ) ;

	//--------------------------------------------------------
	m_pNameLabelValue = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pNameLabelValue->SetFontColor (0xffffffff);
	SetRect( &rt, 64, 242, 160, 253 ) ;
	m_pNameLabelValue->SetAlignRect( rt ) ;
	m_pNameLabelValue->SetAlign( CENTER ) ;
	m_pNameLabelValue->SetCaption( "" ) ;
	m_pNameLabelValue->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pNameLabelValue ) ;

	m_pClassLabelValue = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pClassLabelValue->SetFontColor (0xffffffff);
	SetRect( &rt, 64, 264, 131, 275 ) ;
	m_pClassLabelValue->SetAlignRect( rt ) ;
	m_pClassLabelValue->SetAlign( CENTER ) ;
	m_pClassLabelValue->SetCaption( "" ) ;
	m_pClassLabelValue->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pClassLabelValue ) ;

	m_pLevelLabelValue = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pLevelLabelValue->SetFontColor (0xffffffff);
	SetRect( &rt, 201, 264, 237, 275 ) ;
	m_pLevelLabelValue->SetAlignRect( rt ) ;
	m_pLevelLabelValue->SetAlign( CENTER ) ;
	m_pLevelLabelValue->SetCaption( "" ) ;
	m_pLevelLabelValue->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pLevelLabelValue) ;

	m_pPlaceValueLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pPlaceValueLabel->SetFontColor (0xffffffff);
	SetRect( &rt, 64, 287, 131, 298 ) ;
	m_pPlaceValueLabel->SetAlignRect( rt ) ;
	m_pPlaceValueLabel->SetAlign( CENTER ) ;
	m_pPlaceValueLabel->SetCaption( "" ) ;
	m_pPlaceValueLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pPlaceValueLabel) ;

	//--------------------------------------------------------
	m_pZoneLabelValue = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pZoneLabelValue->SetFontColor (0xffffffff);
	SetRect( &rt, 64, 287, 131, 298 ) ;
	m_pZoneLabelValue->SetAlignRect( rt ) ;
	m_pZoneLabelValue->SetAlign( CENTER ) ;
	m_pZoneLabelValue->SetCaption( "" ) ;
	m_pZoneLabelValue->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pZoneLabelValue ) ;

	// 길드 마크
	m_pGuildMarkSprite = new CRYLSpriteEx( m_pGMUIParentDlg );
	m_pGuildMarkSprite->SetLeft( 209 );
	m_pGuildMarkSprite->SetTop( 240 );
	m_pGuildMarkSprite->SetWidth( 18 );
	m_pGuildMarkSprite->SetHeight( 12 );
	CGMUICustomPanel::AddChild( m_pGuildMarkSprite );

	m_pExpulsionButton = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pExpulsionButton->SetLeft( 101 ) ;
	m_pExpulsionButton->SetTop( 307 ) ;
	m_pExpulsionButton->SetCaption( CRYLStringTable::m_strString[ 1978 ] ) ;
	m_pExpulsionButton->SetFontColor( 0xffffffff) ;
	m_pExpulsionButton->SetColor( 0xffffffff) ;
	m_pExpulsionButton->OnClickEvent = DlgClick ;
	m_pExpulsionButton->SetInstance( RYLPARTYPANEL::BANISH_BUTTON ) ;
	m_pExpulsionButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pExpulsionButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pExpulsionButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pExpulsionButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pExpulsionButton->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1822 ], 1822, TOOLTIP_PANEL ) ;
	CGMUICustomPanel::AddChild( m_pExpulsionButton ) ;

	m_pTransferButton = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pTransferButton->SetLeft( 174 ) ;
	m_pTransferButton->SetTop( 307 ) ;
	m_pTransferButton->SetCaption( CRYLStringTable::m_strString[ 1979 ] ) ;
	m_pTransferButton->SetFontColor( 0xffffffff) ;
	m_pTransferButton->SetColor( 0xffffffff) ;
	m_pTransferButton->OnClickEvent = DlgClick ;
	m_pTransferButton->SetInstance( RYLPARTYPANEL::TRANSFER_BUTTON ) ;
	m_pTransferButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pTransferButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pTransferButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pTransferButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pTransferButton->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1823 ], 1823, TOOLTIP_PANEL ) ;
	CGMUICustomPanel::AddChild( m_pTransferButton ) ;
	
	m_pPartyLeave = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pPartyLeave->SetLeft( 35 ) ;
	m_pPartyLeave->SetTop( 344 ) ;
	m_pPartyLeave->SetCaption( CRYLStringTable::m_strString[ 1980 ] ) ;
	m_pPartyLeave->SetFontColor( 0xffffffff) ;
	m_pPartyLeave->SetColor( 0xffffffff) ;
	m_pPartyLeave->OnClickEvent = DlgClick ;
	m_pPartyLeave->SetInstance( RYLPARTYPANEL::PARTYLEAVE_BUTTON ) ;
	m_pPartyLeave->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pPartyLeave->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pPartyLeave->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pPartyLeave->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pPartyLeave->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1825 ], 1825, TOOLTIP_PANEL ) ;
	CGMUICustomPanel::AddChild( m_pPartyLeave ) ;

	m_pMiniParty = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pMiniParty->SetLeft( 172 ) ;
	m_pMiniParty->SetTop( 344 ) ;
	m_pMiniParty->SetCaption( CRYLStringTable::m_strString[ 1981 ] ) ;
	m_pMiniParty->SetFontColor( 0xffffffff) ;
	m_pMiniParty->SetColor( 0xffffffff) ;
	m_pMiniParty->OnClickEvent = DlgClick ;
	m_pMiniParty->SetInstance( RYLPARTYPANEL::MINIPARTY_BUTTON ) ;
	m_pMiniParty->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pMiniParty->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pMiniParty->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pMiniParty->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pMiniParty->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1827 ], 1827, TOOLTIP_PANEL ) ;
	CGMUICustomPanel::AddChild( m_pMiniParty ) ;

	m_pAllAutoroutingButton = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pAllAutoroutingButton->SetLeft( 104 ) ;
	m_pAllAutoroutingButton->SetTop( 344 ) ;
	m_pAllAutoroutingButton->SetCaption( CRYLStringTable::m_strString[ 1982 ] ) ;
	m_pAllAutoroutingButton->SetFontColor( 0xffffffff) ;
	m_pAllAutoroutingButton->SetColor( 0xffffffff) ;
	m_pAllAutoroutingButton->OnClickEvent = DlgClick ;
	m_pAllAutoroutingButton->SetInstance( RYLPARTYPANEL::ALLAUTOROUTING_BUTTON ) ;
	m_pAllAutoroutingButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pAllAutoroutingButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pAllAutoroutingButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pAllAutoroutingButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pAllAutoroutingButton->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1826 ], 1826, TOOLTIP_PANEL ) ;
	CGMUICustomPanel::AddChild( m_pAllAutoroutingButton ) ;

	pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ;
	m_pPartyControl = new PARTYMEMBERINFO[ PARTY::MAX_MEM ] ;

	for ( INT i = 0 ; i < PARTY::MAX_MEM ; i ++ )
	{
		m_pPartyControl[ i ].iIndex = i ;
		m_pPartyControl[ i ].pAutoRoutingCBox = new CRYLCheckBox( m_pGMUIParentDlg ) ;
		m_pPartyControl[ i ].pAutoRoutingCBox->SetLeft( 222 ) ;
		m_pPartyControl[ i ].pAutoRoutingCBox->SetTop( 35 + ( i * 15 ) ) ;
		m_pPartyControl[ i ].pAutoRoutingCBox->SetWidth( 10 ) ;
		m_pPartyControl[ i ].pAutoRoutingCBox->SetHeight( 10 ) ;
		m_pPartyControl[ i ].pAutoRoutingCBox->SetInstance( i) ;
		m_pPartyControl[ i ].pAutoRoutingCBox->OnClickEvent = DlgClick ;
		SetRect( &rt, 181, 220, 197, 236 ) ;
		m_pPartyControl[ i ].pAutoRoutingCBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
		SetRect( &rt, 181, 238, 197, 254 ) ;
		m_pPartyControl[ i ].pAutoRoutingCBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
		SetRect( &rt, 181, 238, 197, 254 ) ;
		m_pPartyControl[ i ].pAutoRoutingCBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
		SetRect( &rt, 181, 220, 197, 236 ) ;
		m_pPartyControl[ i ].pAutoRoutingCBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
		SetRect( &rt, 181, 220, 197, 236 ) ;
		m_pPartyControl[ i ].pAutoRoutingCBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
		SetRect( &rt, 163, 238, 179, 254 ) ;
		m_pPartyControl[ i ].pAutoRoutingCBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_CHECK_DISIBLE ) ;
		SetRect( &rt, 163, 220, 179, 236 ) ;
		m_pPartyControl[ i ].pAutoRoutingCBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NCHECK_DISIBLE ) ;
		m_pPartyControl[ i ].pAutoRoutingCBox->SetVisible( FALSE ) ;
		m_pPartyControl[ i ].pAutoRoutingCBox->SetTooltip( &m_pGMUIParentDlg->m_TooltipManager, CRYLStringTable::m_strString[ 1824 ], 1824, TOOLTIP_PANEL ) ;
		CGMUICustomPanel::AddChild( m_pPartyControl[ i ].pAutoRoutingCBox ) ;
	}
}

VOID	CRYLPartyPanel::FinalCtrl() 
{
	DeletePartyList() ;

	GM_DELETE( m_pStringGrid ) ;

	GM_DELETE( m_pNameLabel1 ) ;
	GM_DELETE( m_pClassLabel1 ) ;
	GM_DELETE( m_pLevelLabel1 ) ;
	GM_DELETE( m_pRoutingLabel1 ) ;

	GM_DELETE( m_pNameLabel2 ) ;
	GM_DELETE( m_pClassLabel2 ) ;
	GM_DELETE( m_pLevelLabel2 ) ;
	GM_DELETE( m_pGuildLabel2 ) ;
	GM_DELETE( m_pPlaceLabel2 ) ;
	
	GM_DELETE( m_pNameLabelValue ) ;			// 이름 값
	GM_DELETE( m_pClassLabelValue ) ;			// 클래스 값
	GM_DELETE( m_pLevelLabelValue ) ;			// 레벨 값
	GM_DELETE( m_pPlaceValueLabel ) ;
	GM_DELETE( m_pZoneLabelValue );				// 위치 값

	GM_DELETE( m_pGuildMarkSprite );			// 길드 마크
	
	GM_DELETE( m_pExpulsionButton ) ;			// 파티에서 축출
	GM_DELETE( m_pTransferButton ) ;			// 리더권 양도
	GM_DELETE( m_pPartyLeave ) ;				// 파티 탈퇴
	GM_DELETE( m_pMiniParty ) ;					// 미니파티
	GM_DELETE( m_pAllAutoroutingButton ) ;		// 전체 오토루팅
	GM_DELETE_ARRAY( m_pPartyControl ) ;
}


VOID CRYLPartyPanel::Show( CGMUIObject* Sender )
{
	m_pGuildMarkSprite->SetVisible( TRUE );
}

VOID CRYLPartyPanel::Hide( CGMUIObject* Sender )
{
	m_pGuildMarkSprite->SetVisible( FALSE );
}

VOID		CRYLPartyPanel::DeletePartyList() 
{
	m_pStringGrid->ClearItem() ;

	for ( INT i = 0 ; i < PARTY::MAX_MEM ; i ++ )
	{
		m_pPartyControl[ i ].pAutoRoutingCBox->SetVisible( FALSE ) ;
	}
}

VOID		CRYLPartyPanel::UpdateControl() 
{
	CRYLGameData*	pGame = CRYLGameData::Instance() ;

	RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetCreature( CRYLNetworkData::Instance()->m_dwMyChrID );
	
	if(pSelfCreature  && (EnemyCheck::IsCastleArms(pSelfCreature->GetObjectType()) || EnemyCheck::IsSiegeArms(pSelfCreature->GetObjectType())))
	{
		m_pPartyLeave->SetEnable( FALSE ) ;					// 파티 탈퇴
		m_pExpulsionButton->SetEnable( FALSE ) ;			// 파티에서 축출
		m_pTransferButton->SetEnable( FALSE ) ;				// 리더권 양도
		m_pAllAutoroutingButton->SetEnable( FALSE ) ;		// 전체 오토루팅		
	}
	else
	{
		if ( pGame->m_csStatus.m_PartyInfo.m_dwLeaderID == CRYLNetworkData::Instance()->m_dwMyChrID )
		{
			m_pExpulsionButton->SetEnable( TRUE ) ;				// 파티에서 축출
			m_pTransferButton->SetEnable( TRUE ) ;				// 리더권 양도
			m_pAllAutoroutingButton->SetEnable( TRUE ) ;		// 전체 오토루팅			
		}
		else
		{
			m_pExpulsionButton->SetEnable( FALSE ) ;			// 파티에서 축출
			m_pTransferButton->SetEnable( FALSE ) ;				// 리더권 양도
			m_pAllAutoroutingButton->SetEnable( FALSE ) ;		// 전체 오토루팅		
			m_pAllAutoroutingButton->SetCaption( CRYLStringTable::m_strString[ 1982 ] ) ;
		}
	}	

	if ( pGame->m_csStatus.m_PartyInfo.m_dwLeaderID == CRYLNetworkData::Instance()->m_dwMyChrID )
	{
		for(int i = 0; i < PARTY::MAX_MEM; ++i)
		{
			if ( pGame->m_csStatus.m_PartyInfo.MemberCID[ i ] )
			{
				m_pPartyControl[ i ].pAutoRoutingCBox->SetVisible( TRUE ) ;
				if ( pGame->m_csStatus.m_PartyInfo.m_bAutoRouting[ i ] )			
				{
					m_pPartyControl[ i ].pAutoRoutingCBox->SetEnable( TRUE ) ;
					m_pPartyControl[ i ].pAutoRoutingCBox->SetChecked( TRUE ) ;						
				}
				else
				{
					m_pPartyControl[ i ].pAutoRoutingCBox->SetEnable( TRUE ) ;
					m_pPartyControl[ i ].pAutoRoutingCBox->SetChecked( FALSE ) ;
				}
			}
		}
	}
	else
	{
		for(int i = 0; i < PARTY::MAX_MEM; ++i)
		{
			if ( pGame->m_csStatus.m_PartyInfo.MemberCID[ i ] )
			{
				m_pPartyControl[ i ].pAutoRoutingCBox->SetVisible( TRUE ) ;
				if ( pGame->m_csStatus.m_PartyInfo.m_bAutoRouting[ i ] )			
				{
					m_pPartyControl[ i ].pAutoRoutingCBox->SetDisableEmptyCheck( TRUE ) ;						
				}
				else
				{
					m_pPartyControl[ i ].pAutoRoutingCBox->SetDisableEmptyCheck( FALSE ) ;
				}
			}
		}
	}
}

INT			CRYLPartyPanel::GetItemIndexByName( LPSTR szName )
{
	for ( INT iIndex = 0 ; iIndex < m_pStringGrid->Count() ; ++iIndex )
	{
		if (0 == strcmp( szName, m_pStringGrid->GetItem( iIndex, 0 ) ) )
		{
			return iIndex;
		}
	}

	return -1;
}

VOID		CRYLPartyPanel::UpdatePartyList() 
{
	DeletePartyList() ;

	DWORD			dwColor;
	CRYLGameData*	pGame = CRYLGameData::Instance() ;

	for ( int i = 0; i < PARTY::MAX_MEM ; ++i )
	{
		if ( 0 != pGame->m_csStatus.m_PartyInfo.MemberCID[ i ] )
		{
            dwColor = 0xFFFFFFFF ;
			if ( pGame->m_csStatus.m_PartyInfo.MemberCID[ i ] == pGame->m_csStatus.m_PartyInfo.m_dwLeaderID ) dwColor = 0xFF00FF00 ;
			else if ( 0 == strcmp( pGame->m_csStatus.m_PartyInfo.Name[ i ], m_szSelectedMember) ) dwColor = 0xFFFFFF00 ;
			else if ( 0 == pGame->m_csStatus.m_PartyInfo.ServerID[ i ] ) dwColor = 0xFF808080 ;

			// 이름
			m_pStringGrid->AddItem( 0, pGame->m_csStatus.m_PartyInfo.Name[ i ], 92, dwColor ) ;
			
            RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( 
                pGame->m_csStatus.m_PartyInfo.MemberCID[ i ] );

			if( 0 != pGame->m_csStatus.m_PartyInfo.ServerID[ i ] && 0 != pTempCreature)
			{
                CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( 
                    pTempCreature->GetCharID() );

                if( pTempData )
                {
					// 클래스
                    m_pStringGrid->AddItem( 1, pGame->GetClassName( pTempData->m_usClass ), 70, dwColor );

					// 레벨
					CHAR szBuff[ 16 ];
                    sprintf( szBuff, "%d", pTempData->m_cLevel );
					m_pStringGrid->AddItem( 2, szBuff, 49, dwColor );
                }
            }
            else
			{
				// 클래스
				m_pStringGrid->AddItem( 1, pGame->GetClassName( 
                    pGame->m_csStatus.m_PartyInfo.m_wClass[i] ), 70, dwColor );

				// 레벨
				CHAR szBuff[ 16 ];
				sprintf( szBuff, "%d", pGame->m_csStatus.m_PartyInfo.m_cLevel[i] );
				m_pStringGrid->AddItem( 2, szBuff, 49, dwColor ); 						
			}
		}
	}

	if ( -1 == GetItemIndexByName( m_szSelectedMember ) )
		CaptionClear();

}

VOID	CRYLPartyPanel::CaptionClear()
{
	strcpy( m_szSelectedMember, "" );

	m_pNameLabelValue->SetCaption( "" ) ;
	m_pClassLabelValue->SetCaption( "" ) ;
	m_pLevelLabelValue->SetCaption( "" ) ;
	m_pZoneLabelValue->SetCaption( "" ) ;
}

HRESULT		CRYLPartyPanel::Update()
{
    CRYLGameData*	pGame = CRYLGameData::Instance() ;
	if ( pGame->m_csStatus.m_PartyInfo.m_cMemberNum == 0 )
	{
		m_pExpulsionButton->SetEnable( FALSE ) ;
		m_pTransferButton->SetEnable( FALSE ) ;
		m_pPartyLeave->SetEnable( FALSE ) ; 
		m_pMiniParty->SetEnable( FALSE ) ; 
		m_pAllAutoroutingButton->SetEnable( FALSE ) ; 
	}
	else
	{
		RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetCreature( CRYLNetworkData::Instance()->m_dwMyChrID );

		if(pSelfCreature)			
		{
			if(!(EnemyCheck::IsCastleArms(pSelfCreature->GetObjectType()) || EnemyCheck::IsSiegeArms(pSelfCreature->GetObjectType())))
			{
				m_pPartyLeave->SetEnable( TRUE ) ; 
				m_pMiniParty->SetEnable( TRUE ) ; 
			}	
		}
	}
	
	return S_OK ;
}


VOID		CRYLPartyPanel::UpdateSelectPartyMember() 
{
    INT iIndex = m_pStringGrid->GetItemIndex() ;

	if ( iIndex == -1 )
		return ;
	
	// 이름 값
	m_pNameLabelValue->SetCaption( m_pStringGrid->GetItem( iIndex, 0 ) ) ;
	// 클래스 값
	m_pClassLabelValue->SetCaption( m_pStringGrid->GetItem( iIndex, 1 ) ) ;
	// 레벨 값
	m_pLevelLabelValue->SetCaption( m_pStringGrid->GetItem( iIndex, 2 ) ) ;

	// 위치값.
	CRYLGameData* pGame = CRYLGameData::Instance();

	for ( int i = 0; i < PARTY::MAX_MEM ; ++i )
	{
		if(pGame->m_csStatus.m_PartyInfo.MemberCID[i])
		{
			if(strcmp(m_pStringGrid->GetItem(iIndex, 0), pGame->m_csStatus.m_PartyInfo.Name[i])==0)
			{
				SERVER_ID serverID;
				serverID.dwID = pGame->m_csStatus.m_PartyInfo.ServerID[i];

				m_pZoneLabelValue->SetCaption(const_cast<char*>(CRYLNetworkData::Instance()->GetZoneName(serverID.GetZone())));

				CRYLGameScene*		pScene	= ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );
				CRYLGuildFrameDlg*	pDlg	= ( CRYLGuildFrameDlg* )pScene->GetGuildFrameDlg();
				CRYLGuildInfoPanel* pPanel	= ( CRYLGuildInfoPanel* )pDlg->GetGuildInfoPanel();

				// 길드 마크 출력
				LPDIRECT3DDEVICE8   lpD3DDevice = BaseGraphicsLayer::GetDevice();

				CRYLSprite* pSprite = NULL;
				
				if(strcmp(m_pStringGrid->GetItem(iIndex, 0), pGame->m_csStatus.GetName())==0)
				{
					pSprite = pPanel->GetGuildSprite(pGame->m_csStatus.GetGuild());
				}
				else
				{
					pSprite = pPanel->GetGuildSprite(pGame->m_csStatus.m_PartyInfo.m_dwGID[i]);
				}

				if(pSprite)
				{
					m_pGuildMarkSprite->SetVisible( TRUE );
					m_pGuildMarkSprite->AttachImageTexture( pSprite );
				}				
				break;
			}
		}
	}

	// 이름 기억해두기
	strcpy( m_szSelectedMember, m_pStringGrid->GetItem( iIndex, 0 ) ) ;

	// 리스트 다시 업데이트
	UpdatePartyList() ;
	UpdateControl() ;
}

VOID		CRYLPartyPanel::UpdateAutoRouting()
{
	CRYLGameData*	pGame = CRYLGameData::Instance() ;
	m_bAutoRouting = !m_bAutoRouting ;
	if ( m_bAutoRouting )
	{
		SendPacket::CharPartyCmd( g_GameSession, 
								  0, 
								  CRYLNetworkData::Instance()->m_dwMyChrID,
								  pGame->m_csStatus.m_PartyInfo.m_dwPartyID, 
								  ClientConstants::PartyCmd_AutoRoutingOff ) ;
		m_pAllAutoroutingButton->SetCaption( CRYLStringTable::m_strString[ 1983 ] ) ;
	}
	else 
	{
		SendPacket::CharPartyCmd( g_GameSession, 
							 	  0, 
								  CRYLNetworkData::Instance()->m_dwMyChrID,
								  pGame->m_csStatus.m_PartyInfo.m_dwPartyID, 
								  ClientConstants::PartyCmd_AutoRoutingOn ) ;
		m_pAllAutoroutingButton->SetCaption( CRYLStringTable::m_strString[ 1982 ] ) ;
	}
}

VOID		CRYLPartyPanel::DlgClick( CGMUIObject* Sender, INT X, INT Y ) 
{
	CRYLGameData*	pGame = CRYLGameData::Instance() ;

	if ( !g_pPartyPanel )
		return ;

	switch( Sender->GetInstance() )
	{
		case RYLPARTYPANEL::PARTY_STRINGGRID : 
		{
			g_pPartyPanel->UpdateSelectPartyMember() ;
			break;
		}

		// 파티 탈퇴
		case RYLPARTYPANEL::PARTYLEAVE_BUTTON :
		{
			CRYLNetworkData::Instance()->SendPartyCmd(0, ClientConstants::PartyCmd_Secession);

			// 미니 파티창 닫기
			CRYLGameScene*		 pScene		= ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
			CRYLMiniPartyDlg*    pMiniDlg	= ( CRYLMiniPartyDlg* )pScene->GetMiniPartyDlg() ;
			pMiniDlg->SetVisible( FALSE ) ;
			break ;
		}

		// 리더권 양도
		case RYLPARTYPANEL::TRANSFER_BUTTON	:
		{
			if ( 0 != strcmp( "", g_pPartyPanel->GetSelectedMemberName() ) )
			{
				INT iIndex = g_pPartyPanel->GetItemIndexByName( g_pPartyPanel->GetSelectedMemberName() ) ;
				if ( -1 != iIndex )
				{
					SendPacket::CharPartyCmd( g_GameSession, 
												pGame->m_csStatus.m_PartyInfo.MemberCID[ iIndex ],
												CRYLNetworkData::Instance()->m_dwMyChrID, 
												pGame->m_csStatus.m_PartyInfo.m_dwPartyID, 
												ClientConstants::PartyCmd_Transfer ) ;
				}
			}
			break ;
		}

		// 파티에서 축출
		case RYLPARTYPANEL::BANISH_BUTTON :
		{
			if ( 0 != strcmp( "", g_pPartyPanel->GetSelectedMemberName() ) )
			{
				INT iIndex = g_pPartyPanel->GetItemIndexByName( g_pPartyPanel->GetSelectedMemberName() ) ;
				if ( -1 != iIndex )
				{
					SendPacket::CharPartyCmd( g_GameSession, 
												pGame->m_csStatus.m_PartyInfo.MemberCID[ iIndex ], 
												CRYLNetworkData::Instance()->m_dwMyChrID,
												pGame->m_csStatus.m_PartyInfo.m_dwPartyID, 
												ClientConstants::PartyCmd_Banish ) ;
				}
			}
			break ;
		}

		// 미니파티
		case RYLPARTYPANEL::MINIPARTY_BUTTON :
		{
			CRYLGameScene*		 pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
			CRYLMiniPartyDlg*      pDlg = ( CRYLMiniPartyDlg* )pScene->GetMiniPartyDlg() ;
			pDlg->SetVisible( TRUE ) ;
			break ;
		}

		// 전체 오토루팅
		case RYLPARTYPANEL:: ALLAUTOROUTING_BUTTON :			
		{
			g_pPartyPanel->UpdateAutoRouting() ;
			break ;
		}

		case RYLPARTYPANEL::AUTOROUTING_CHECKBOX1 :
		case RYLPARTYPANEL::AUTOROUTING_CHECKBOX2 :
		case RYLPARTYPANEL::AUTOROUTING_CHECKBOX3 :
		case RYLPARTYPANEL::AUTOROUTING_CHECKBOX4 : 
		case RYLPARTYPANEL::AUTOROUTING_CHECKBOX5 :
		case RYLPARTYPANEL::AUTOROUTING_CHECKBOX6 :
		case RYLPARTYPANEL::AUTOROUTING_CHECKBOX7 :
		case RYLPARTYPANEL::AUTOROUTING_CHECKBOX8 :
		case RYLPARTYPANEL::AUTOROUTING_CHECKBOX9 :
		case RYLPARTYPANEL::AUTOROUTING_CHECKBOX10 :
		{
			
			INT			 iIndex = Sender->GetInstance() ;
			SendPacket::CharPartyCmd( g_GameSession, 
										pGame->m_csStatus.m_PartyInfo.MemberCID[ iIndex ], 
										CRYLNetworkData::Instance()->m_dwMyChrID,
										pGame->m_csStatus.m_PartyInfo.m_dwPartyID, 
										ClientConstants::PartyCmd_AutoRoutingOn + g_pPartyPanel->m_pPartyControl[ iIndex ].pAutoRoutingCBox->GetChecked() ) ;
			break ;
		}
	}	
}

VOID		CRYLPartyPanel::DlgDBClick( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y ) 
{
	if ( !g_pPartyPanel )
		return ;

	switch ( Sender->GetInstance() )
	{
		case RYLPARTYPANEL::PARTY_STRINGGRID : 
			// 귀속말 보내기
			static_cast< CRYLSocietyFrameDlg* >( g_pPartyPanel->GetParentDlg() )->SetWhisperCharacter( g_pPartyPanel->GetSelectedMemberName() ) ;
			break ;
	}
}

