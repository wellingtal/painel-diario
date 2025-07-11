#include <winsock2.h>
#include "RYLFriendPanel.h"
#include "RYLLabel.h"
#include "RYLButton.h"
#include "RYLStringGrid.h"
#include "RYLSpriteEx.h"
#include "RYLGameData.h"
#include "RYLNetworkData.h"
#include "RYLStringTable.h"
#include "RYLChattingDlg.h"
#include "RYLSpriteList.h"
#include "RYLImageCabinet.h"
#include "RYLCommunityData.h"
#include "GMImageDib.h"
#include "RYLImage.h"
#include "RYLImageManager.h"
#include "RYLGuildInfoPanel.h"
#include "RYLGuildFrameDlg.h"

#include "RYLSceneManager.h"
#include "RYLGameScene.h"
#include "RYLMessageBox.h"
#include "RYLMessageBoxManager.h"
#include "RYLSocietyFrameDlg.h"

#include <Network/ClientSocket/Send/SendCommunity.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>
#include "GMMemory.h"

CRYLFriendPanel*	g_pFriendPanel = NULL ;

CRYLFriendPanel::CRYLFriendPanel( CGMUICustomDlg *pParentDlg ) : CGMUICustomPanel( pParentDlg )
{
	m_pGMUIParentDlg = pParentDlg ;
	
	m_pNameLabel				= NULL ;		// 이름
	m_pClassLabel				= NULL ;		// 클레스
	m_pLevelLabel				= NULL ;		// 레벨
	m_pStringGrid				= NULL ;		// 그리드 
	m_pDelFriendListButton		= NULL ;		// 친구 리스트에 삭제
	m_pAddResusalListButton		= NULL ;		// 거부 리스트에 추가
	m_pPagePrevButton			= NULL ;		// 이전 페이지
	m_pPageNextButton			= NULL ;		// 다음 페이지
	m_pPageInfoLabel			= NULL ;		// 페이지 정보
	m_pFriendCmdLabel			= NULL ;		// 친구등록 설명
	m_pGuildMarkSprite			= NULL;

	m_dwRemoveResult			= 0L ;
	m_dwAddResult				= 0L ;
	m_dwCommandType				= 0L ;
	m_dwChrID					= 0L ;

	m_dwStartSlot				= 0L ;

	CRYLCommunityData::Instance()->m_dwSelectMemberSlot = 0xFFFFFFFF;
	CRYLCommunityData::Instance()->m_dwSelectPartySlot = 0xFFFFFFFF;

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
	pImageDIB->MakeFrameRect( 7,   254, 243, 109, 0xffB99E6B, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect( 13,   27, 233,   1, 0xff959595, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect( 166, 229, 68,   18, 0xff959595, 0xc0000000,  1 ) ;
 	pImageDIB->MakeFrameRect( 64,  262,  97,  19, 0xff959595, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect( 201, 262,  37,  18, 0xff959595, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect( 64,  285,  68,  19, 0xff959595, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect( 201, 285,  37,  18, 0xff959595, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect( 64,  308,  68,  18, 0xff959595, 0xc0000000,  1 ) ;

	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	AttachPanelImage( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;
	
	SetWidth( 256 ) ;
	SetHeight( 395 ) ;

	g_pFriendPanel = this ;
}

CRYLFriendPanel::~CRYLFriendPanel() 
{
	FinalCtrl() ;
}

VOID	CRYLFriendPanel::InitCtrl() 
{
	RECT rt ;
	CGMImageDib* pSprite			= CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;

	m_pNameLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pNameLabel->SetFontColor (0xffffffff);
	SetRect( &rt, 17, 11, 127, 22 ) ;
	m_pNameLabel->SetAlignRect( rt ) ;
	m_pNameLabel->SetAlign( CENTER ) ;
	m_pNameLabel->SetCaption( CRYLStringTable::m_strString[ 1926 ] ) ;
	m_pNameLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pNameLabel ) ;
	
	m_pClassLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pClassLabel->SetFontColor (0xffffffff);
	SetRect( &rt, 127, 11, 207, 22 ) ;
	m_pClassLabel->SetAlignRect( rt ) ;
	m_pClassLabel->SetAlign( CENTER ) ;
	m_pClassLabel->SetCaption( CRYLStringTable::m_strString[ 1928 ] ) ;
	m_pClassLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pClassLabel ) ;
	
	m_pLevelLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pLevelLabel->SetFontColor (0xffffffff);
	SetRect( &rt, 207, 11, 246, 22 ) ;
	m_pLevelLabel->SetAlignRect( rt ) ;
	m_pLevelLabel->SetAlign( CENTER ) ;
	m_pLevelLabel->SetCaption( CRYLStringTable::m_strString[ 1930 ] ) ;
	m_pLevelLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pLevelLabel ) ;

	m_pStringGrid = new CRYLStringGrid( m_pGMUIParentDlg, FALSE ) ;
	m_pStringGrid->SetLeft( 16 ) ;
	m_pStringGrid->SetTop( 35 ) ;
	m_pStringGrid->SetWidth( 182 ) ;
	m_pStringGrid->SetHeight( 182 ) ;
	m_pStringGrid->SetColCount( 10 ) ;
	m_pStringGrid->SetFontColor( 0xffffffff ) ;
	m_pStringGrid->SetInstance( RYLFRIENDPANEL::FRIEND_STRINGGRID ) ;
	m_pStringGrid->OnClickEvent = DlgClick ;
	m_pStringGrid->OnDblClickEvent = DlgDBClick ;
	CGMUICustomPanel::AddChild( m_pStringGrid ) ;

	m_pInfoNameLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pInfoNameLabel->SetFontColor (0xffffffff);
	SetRect( &rt, 10, 266, 61, 277 ) ;
	m_pInfoNameLabel->SetAlignRect( rt ) ;
	m_pInfoNameLabel->SetAlign( RIGHT ) ;
	m_pInfoNameLabel->SetCaption( CRYLStringTable::m_strString[ 1926 ] ) ;
	m_pInfoNameLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pInfoNameLabel ) ;

	m_pInfoGuildLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pInfoGuildLabel->SetFontColor (0xffffffff);
	SetRect( &rt, 165, 266, 198, 277 ) ;
	m_pInfoGuildLabel->SetAlignRect( rt ) ;
	m_pInfoGuildLabel->SetAlign( RIGHT ) ;
	m_pInfoGuildLabel->SetCaption( CRYLStringTable::m_strString[ 1927 ] ) ;
	m_pInfoGuildLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pInfoGuildLabel ) ;

	m_pInfoClassLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pInfoClassLabel->SetFontColor (0xffffffff);
	SetRect( &rt, 10, 288, 61, 299 ) ;
	m_pInfoClassLabel->SetAlignRect( rt ) ;
	m_pInfoClassLabel->SetAlign( RIGHT ) ;
	m_pInfoClassLabel->SetCaption( CRYLStringTable::m_strString[ 1928 ] ) ;
	m_pInfoClassLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pInfoClassLabel ) ;

	m_pInfoLevelLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pInfoLevelLabel->SetFontColor (0xffffffff);
	SetRect( &rt, 165, 288, 198, 299 ) ;
	m_pInfoLevelLabel->SetAlignRect( rt ) ;
	m_pInfoLevelLabel->SetAlign( RIGHT ) ;
	m_pInfoLevelLabel->SetCaption( CRYLStringTable::m_strString[ 1930 ] ) ;
	m_pInfoLevelLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pInfoLevelLabel ) ;

	m_pInfoPlaceLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pInfoPlaceLabel->SetFontColor (0xffffffff);
	SetRect( &rt, 10, 311, 61, 322 ) ;
	m_pInfoPlaceLabel->SetAlignRect( rt ) ;
	m_pInfoPlaceLabel->SetAlign( RIGHT ) ;
	m_pInfoPlaceLabel->SetCaption( CRYLStringTable::m_strString[ 1966 ] ) ;
	m_pInfoPlaceLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pInfoPlaceLabel ) ;

	// 이름 값
	m_pNameLabelValue = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pNameLabelValue->SetFontColor (0xffffffff);
	SetRect( &rt, 64, 266, 160, 277 ) ;
	m_pNameLabelValue->SetAlignRect( rt ) ;
	m_pNameLabelValue->SetAlign( CENTER ) ;
	m_pNameLabelValue->SetCaption( "" ) ;
	m_pNameLabelValue->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pNameLabelValue ) ;

	// 클래스 값
	m_pClassLabelValue = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pClassLabelValue->SetFontColor (0xffffffff);
	SetRect( &rt, 64, 288, 131, 299 ) ;
	m_pClassLabelValue->SetAlignRect( rt ) ;
	m_pClassLabelValue->SetAlign( CENTER ) ;
	m_pClassLabelValue->SetCaption( "" ) ;
	m_pClassLabelValue->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pClassLabelValue ) ;

	// 레벨 값
	m_pLevelLabelValue = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pLevelLabelValue->SetFontColor (0xffffffff);
	SetRect( &rt, 201, 288, 237, 299 ) ;
	m_pLevelLabelValue->SetAlignRect( rt ) ;
	m_pLevelLabelValue->SetAlign( CENTER ) ;
	m_pLevelLabelValue->SetCaption( "" ) ;
	m_pLevelLabelValue->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pLevelLabelValue ) ;

	// 장소 값
	m_pPlaceValueLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pPlaceValueLabel->SetFontColor (0xffffffff);
	SetRect( &rt, 64, 311, 131, 322 ) ;
	m_pPlaceValueLabel->SetAlignRect( rt ) ;
	m_pPlaceValueLabel->SetAlign( CENTER ) ;
	m_pPlaceValueLabel->SetCaption( "" ) ;
	m_pPlaceValueLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pPlaceValueLabel ) ;

	//--------------------------------------------------------
	m_pZoneLabelValue = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pZoneLabelValue->SetFontColor (0xffffffff);
	SetRect( &rt, 64, 310, 131, 321 ) ;
	m_pZoneLabelValue->SetAlignRect( rt ) ;
	m_pZoneLabelValue->SetAlign( CENTER ) ;
	m_pZoneLabelValue->SetCaption( "" ) ;
	m_pZoneLabelValue->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pZoneLabelValue ) ;

	// 길드 마크
	m_pGuildMarkSprite = new CRYLSpriteEx( m_pGMUIParentDlg );
	m_pGuildMarkSprite->SetLeft( 209 );
	m_pGuildMarkSprite->SetTop( 266 );
	m_pGuildMarkSprite->SetWidth( 18 );
	m_pGuildMarkSprite->SetHeight( 12 );
	CGMUICustomPanel::AddChild( m_pGuildMarkSprite );

// Rodin : 임시 봉인 (친구/거부 리스트 추가 커맨드)
/*
	// 친구등록 설명
	m_pFriendCmdLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pFriendCmdLabel->SetFontColor (0xffffffff);
	SetRect( &rt, 10, 234, 150, 245 ) ;
	m_pFriendCmdLabel->SetAlignRect( rt ) ;
	m_pFriendCmdLabel->SetAlign( LEFT ) ;
	m_pFriendCmdLabel->SetCaption( CRYLStringTable::m_strString[ 3547 ] ) ;
	m_pFriendCmdLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pFriendCmdLabel ) ;
*/
            
	// 친구 리스트에 삭제
	m_pDelFriendListButton = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pDelFriendListButton->SetLeft( 101 ) ;
	m_pDelFriendListButton->SetTop( 331 ) ;
	m_pDelFriendListButton->SetCaption( CRYLStringTable::m_strString[ 1967 ] ) ;
	m_pDelFriendListButton->SetFontColor( 0xffffffff) ; 
	m_pDelFriendListButton->SetColor( 0xffffffff) ;
	m_pDelFriendListButton->OnClickEvent = DlgClick ;
	m_pDelFriendListButton->SetInstance( RYLFRIENDPANEL::DELFRIENDLIST_BUTTON ) ;
	m_pDelFriendListButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pDelFriendListButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pDelFriendListButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pDelFriendListButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	CGMUICustomPanel::AddChild( m_pDelFriendListButton ) ;

	// 거부 리스트에 추가
	m_pAddResusalListButton = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pAddResusalListButton->SetLeft( 174 ) ;
	m_pAddResusalListButton->SetTop( 331 ) ;
	m_pAddResusalListButton->SetCaption( CRYLStringTable::m_strString[ 1968 ] ) ;
	m_pAddResusalListButton->SetFontColor( 0xffffffff) ;
	m_pAddResusalListButton->SetColor( 0xffffffff) ;
	m_pAddResusalListButton->OnClickEvent = DlgClick ;
	m_pAddResusalListButton->SetInstance( RYLFRIENDPANEL::ADDREFUSALLIST_BUTTON ) ;
	m_pAddResusalListButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pAddResusalListButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pAddResusalListButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pAddResusalListButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	CGMUICustomPanel::AddChild( m_pAddResusalListButton ) ;

	// 페이지 정보 값
	m_pPageInfoLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pPageInfoLabel->SetLeft( 172 ) ;
	m_pPageInfoLabel->SetTop( 232 ) ;
	m_pPageInfoLabel->SetFontColor (0xffffffff);
	SetRect( &rt, 166, 232, 233, 243 ) ;
	m_pPageInfoLabel->SetAlignRect( rt ) ;
	m_pPageInfoLabel->SetAlign( CENTER ) ;
	m_pPageInfoLabel->SetCaption( "" ) ;
	m_pPageInfoLabel->SetAutoSize( TRUE ) ;
	m_pPageInfoLabel->SetVisible( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pPageInfoLabel ) ;
	
	pSprite	= CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ;
	// 이전 페이지
	m_pPagePrevButton = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pPagePrevButton->SetLeft( 151 ) ;
	m_pPagePrevButton->SetTop( 232 ) ;
	m_pPagePrevButton->OnClickEvent = DlgClick ;
	m_pPagePrevButton->SetInstance( RYLFRIENDPANEL::PAGEPREV_BUTTON ) ;
	SetRect( &rt, 217, 215, 230, 227 ) ;
	m_pPagePrevButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 217, 243, 230, 255 ) ;
	m_pPagePrevButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 217, 229, 230, 241 ) ;
	m_pPagePrevButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 217, 215, 230, 227 ) ;
	m_pPagePrevButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	CGMUICustomPanel::AddChild( m_pPagePrevButton ) ;

	// 다음 페이지
	m_pPageNextButton = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pPageNextButton->SetLeft( 236 ) ;
	m_pPageNextButton->SetTop( 232 ) ;
	m_pPageNextButton->OnClickEvent = DlgClick ;
	m_pPageNextButton->SetInstance( RYLFRIENDPANEL::PAGENEXT_BUTTON ) ;
	SetRect( &rt, 232, 215, 245, 227 ) ;
	m_pPageNextButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 232, 243, 245, 255 ) ;
	m_pPageNextButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 232, 229, 245, 241 ) ;
	m_pPageNextButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 232, 215, 245, 227 ) ;
	m_pPageNextButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	CGMUICustomPanel::AddChild( m_pPageNextButton ) ;
}

VOID	CRYLFriendPanel::FinalCtrl() 
{
	GM_DELETE( m_pNameLabel ) ;					// 이름
	GM_DELETE( m_pClassLabel ) ;				// 클레스
	GM_DELETE( m_pLevelLabel ) ;				// 레벨
	GM_DELETE( m_pStringGrid ) ;				// 그리드 
	GM_DELETE( m_pDelFriendListButton ) ;		// 친구 추가
	GM_DELETE( m_pAddResusalListButton ) ;		// 친구 삭제
	GM_DELETE( m_pPagePrevButton ) ;			// 이전 페이지
	GM_DELETE( m_pPageNextButton ) ;			// 다음 페이지
	GM_DELETE( m_pPageInfoLabel ) ;
	GM_DELETE( m_pFriendCmdLabel ) ;

	GM_DELETE( m_pZoneLabelValue );				// 위치 값
	GM_DELETE( m_pGuildMarkSprite );			// 길드 마크

	GM_DELETE( m_pInfoNameLabel ) ;
	GM_DELETE( m_pInfoGuildLabel ) ;
	GM_DELETE( m_pInfoClassLabel ) ;
	GM_DELETE( m_pInfoLevelLabel ) ;
	GM_DELETE( m_pInfoPlaceLabel ) ;

	GM_DELETE( m_pNameLabelValue ) ;		
	GM_DELETE( m_pClassLabelValue ) ;	
	GM_DELETE( m_pLevelLabelValue ) ;	
	GM_DELETE( m_pPlaceValueLabel ) ;	
}

VOID CRYLFriendPanel::Show( CGMUIObject* Sender )
{
	m_pGuildMarkSprite->SetVisible( TRUE );
}

VOID CRYLFriendPanel::Hide( CGMUIObject* Sender )
{
	m_pGuildMarkSprite->SetVisible( FALSE );
}

std::list< FriendInfo* >::iterator		CRYLFriendPanel::FindIter( INT iIndex ) 
{
	INT				iCount = 0 ;
	CRYLGameData*	pGame = CRYLGameData::Instance() ;
	std::list< FriendInfo* >::iterator it ;
	
	for ( it = pGame->m_csStatus.m_lstFriendList.begin() ; it != pGame->m_csStatus.m_lstFriendList.end() ; ++ it )
	{
		if ( iCount == iIndex )
		{
			return it ;
		}
		++ iCount ;
	}
	return pGame->m_csStatus.m_lstFriendList.end() ;
}

HRESULT		CRYLFriendPanel::FriendListUpdate()
{
	CRYLGameData*	pGame = CRYLGameData::Instance() ;
	m_pStringGrid->ClearItem() ;

	if ( pGame->m_csStatus.m_lstFriendList.empty() )
	{
		m_pPageInfoLabel->SetCaption( "" ) ;
		return E_FAIL ;
	}

	if ( m_dwStartSlot == pGame->m_csStatus.m_lstFriendList.size() )
		m_dwStartSlot -= 10;

	std::list<FriendInfo *>::iterator it = FindIter( m_dwStartSlot ) ;

	FriendInfo*		lpFriendInfo = NULL ;
	CHAR			szLevel[ 32 ] ;
	
	for ( INT i = 0 ; i < GetCurrentPageLine() && it != pGame->m_csStatus.m_lstFriendList.end(); ++i, ++it )
	{
		lpFriendInfo = ( *it ) ;

        RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( (*it)->m_dwCID );

        if ( lpFriendInfo->IsLogined() )
		{
			m_pStringGrid->AddItem( 0, (* it )->m_szName, 110, 0xFFFFFFFF ) ;

			if ( pTempCreature )
			{
                CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( pTempCreature->GetCharID() );
				if ( pTempData )
				{
					m_pStringGrid->AddItem( 1, pGame->GetClassName( pTempData->m_usClass ), 80, 0xFFFFFFFF ) ;
					sprintf( szLevel, "%d", pTempData->m_cLevel ) ;
					m_pStringGrid->AddItem( 2, szLevel, 34, 0xFFFFFFFF ) ;
				}
				else
				{
					m_pStringGrid->AddItem( 1, pGame->GetClassName( (* it )->m_wClass ), 80, 0xFFFFFFFF ) ;
					sprintf( szLevel, "%d", (* it )->m_cLevel ) ;
					m_pStringGrid->AddItem( 2, szLevel, 34, 0xFFFFFFFF ) ;
				}
			}
			else
			{
				m_pStringGrid->AddItem( 1, pGame->GetClassName( (* it )->m_wClass ), 80, 0xFFFFFFFF ) ;
				sprintf( szLevel, "%d", (* it )->m_cLevel ) ;
				m_pStringGrid->AddItem( 2, szLevel, 34, 0xFFFFFFFF ) ;
			}
		}
		else
		{	
			unsigned long dwColor = 0xFF808080;

			if ((*it)->m_dwServerID)
			{
				dwColor = 0xFFFFFFFF;
			}
						
			m_pStringGrid->AddItem( 0, (* it )->m_szName, 110, dwColor ) ;

			m_pStringGrid->AddItem( 1, pGame->GetClassName((* it )->m_wClass), 80, dwColor ) ;

			sprintf( szLevel, "%d", (* it )->m_cLevel) ;
			m_pStringGrid->AddItem( 2, szLevel, 34, dwColor ) ;

		}
	}

	// 페이지 정보 값 갱신
	
	char szPageInfo[32] ;
	sprintf( szPageInfo, "%2d / %2d", (m_dwStartSlot / 10) + 1, (pGame->m_csStatus.m_lstFriendList.size() / 11) + 1 ) ;
	m_pPageInfoLabel->SetCaption( szPageInfo ) ;
	
	return S_OK ;
}

HRESULT		CRYLFriendPanel::Update()
{
	if ( m_dwRemoveResult & MB_YES )
	{
		SendPacket::CharFriendRemoveRequest( g_GameSession, m_dwChrID, (PktFriendRemoveReq::CMD)m_dwCommandType, NULL ) ;

		m_dwRemoveResult = 0 ;
		return S_OK ;
	} 
	else if ( m_dwRemoveResult & MB_NO || m_dwRemoveResult & MB_EXIT )
	{
		m_dwRemoveResult = 0 ;
		return S_OK ;
	}

	if ( m_dwAddResult & MB_YES )
	{
		SendPacket::CharFriendAddRequest( g_GameSession, m_strName, (PktFriendAddReq::CMD)m_dwCommandType ) ;

		m_dwAddResult = 0 ;
		return S_OK ;
	} 
	else if ( m_dwAddResult & MB_NO || m_dwAddResult & MB_EXIT )
	{
		m_dwAddResult = 0 ;
		return S_OK ;
	}

	return S_OK ;
}

VOID		CRYLFriendPanel::DelFriendListEvent() 
{ 
	CRYLGameData*	pGame = CRYLGameData::Instance() ;
	FriendInfo*		lpFriend = pGame->m_csStatus.GetFriendtoNum( m_dwStartSlot + m_pStringGrid->GetItemIndex() ) ;
	
	if ( lpFriend )
	{
		char strMessage[MAX_PATH];
		sprintf( strMessage, CRYLStringTable::m_strString[224], lpFriend->m_szName ) ;
		CRYLMessageBox *lpMessageBox;
		lpMessageBox = new CRYLMessageBox;
		lpMessageBox->SetWordBreak(TRUE);
		lpMessageBox->Create(strMessage, MB_YES | MB_NO | MB_EXIT);
		lpMessageBox->SetResult(&m_dwRemoveResult);
		m_dwRemoveResult = 0;
		m_dwChrID = lpFriend->m_dwCID;
		m_dwCommandType = PktFriendRemoveReq::REMOVE_FRIEND_REQ ;
	}
}

VOID		CRYLFriendPanel::AddResusalListEvent() 
{
	CRYLGameData*	pGame = CRYLGameData::Instance() ;
	FriendInfo*		lpFriend = pGame->m_csStatus.GetFriendtoNum( m_dwStartSlot + m_pStringGrid->GetItemIndex() ) ;
	
	if ( lpFriend )
	{
		if ( pGame->m_csStatus.m_lstBanList.size() >= 100 )
		{
			CRYLChattingDlg::Instance()->AddMessage( CRYLStringTable::m_strString[ 225 ], CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
		} 
		else
		{
			char strMessage[MAX_PATH];
			sprintf( strMessage, CRYLStringTable::m_strString[ 226 ], lpFriend->m_szName ) ;
			CRYLMessageBox *lpMessageBox;
			lpMessageBox = new CRYLMessageBox;
			lpMessageBox->SetWordBreak(TRUE);
			lpMessageBox->Create(strMessage, MB_YES | MB_NO | MB_EXIT);
			lpMessageBox->SetResult(&m_dwAddResult);
			m_dwAddResult = 0;
			strcpy(m_strName, lpFriend->m_szName);
			m_dwCommandType = PktFriendAddReq::BAN_FRIEND_REQ ;
		}
	}
}

VOID CRYLFriendPanel::ClearWhisperCharacter(char* szName)
{
	if (strcmp(m_pNameLabelValue->GetCaption(), szName)==0)
	{
		// 이름 값
		m_pNameLabelValue->SetCaption("");
		// 클래스 값
		m_pClassLabelValue->SetCaption("");
		// 레벨 값
		m_pLevelLabelValue->SetCaption("");

		m_pZoneLabelValue->SetCaption("");

		m_pGuildMarkSprite->SetVisible( FALSE );
	}
}

VOID CRYLFriendPanel::ClearWhisperCharacter()
{	
	// 이름 값
	m_pNameLabelValue->SetCaption("");
	// 클래스 값
	m_pClassLabelValue->SetCaption("");
	// 레벨 값
	m_pLevelLabelValue->SetCaption("");

	m_pZoneLabelValue->SetCaption("");

	m_pGuildMarkSprite->SetVisible( FALSE );
}

VOID CRYLFriendPanel::UpdateWhisperCharacter(char* szName)
{
	if (!szName)
		return;

	if (strcmp(m_pNameLabelValue->GetCaption(), szName)==0)
	{
		m_pGuildMarkSprite->SetVisible( FALSE );
				
		CRYLGameData*	pGame = CRYLGameData::Instance() ;

		if ( m_dwStartSlot == pGame->m_csStatus.m_lstFriendList.size() )
			m_dwStartSlot -= 10;

		std::list<FriendInfo *>::iterator it = FindIter( m_dwStartSlot ) ;

		for ( INT i = 0 ; i < GetCurrentPageLine() && it != pGame->m_csStatus.m_lstFriendList.end(); ++i, ++it )
		{		
			if (strcmp((* it )->m_szName, m_szSelectedMember)==0)
			{
				SERVER_ID serverID;
				serverID.dwID = (*it)->m_dwServerID;

				m_pZoneLabelValue->SetCaption(const_cast<char*>(CRYLNetworkData::Instance()->GetZoneName(serverID.GetZone()))) ;
				
				CHAR szLevel[ 32 ] ;
				sprintf( szLevel, "%d", (* it )->m_cLevel) ;				

				// 클래스 값
				m_pClassLabelValue->SetCaption(pGame->GetClassName((* it )->m_wClass)) ;
				// 레벨 값
				m_pLevelLabelValue->SetCaption(szLevel) ;

				CRYLGameScene*		pScene	= ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );
				CRYLGuildFrameDlg*	pDlg	= ( CRYLGuildFrameDlg* )pScene->GetGuildFrameDlg();
				CRYLGuildInfoPanel* pPanel	= ( CRYLGuildInfoPanel* )pDlg->GetGuildInfoPanel();

				// 길드 마크 출력
				LPDIRECT3DDEVICE8   lpD3DDevice = BaseGraphicsLayer::GetDevice();

				CRYLSprite* pSprite = NULL;

				if (strcmp(szName, pGame->m_csStatus.GetName())==0)
				{
					pSprite = pPanel->GetGuildSprite(pGame->m_csStatus.GetGuild());
				}
				else
				{
					pSprite = pPanel->GetGuildSprite((*it)->m_dwGID);
				}

				if (pSprite)
				{
					m_pGuildMarkSprite->SetVisible( TRUE );
					m_pGuildMarkSprite->AttachImageTexture( pSprite );
				}				

				break;
			}
		}
	}
}


VOID		CRYLFriendPanel::SetWhisperCharacter() 
{
	INT iIndex = m_pStringGrid->GetItemIndex() ;

	if ( iIndex == -1 )
		return ;

	m_pGuildMarkSprite->SetVisible( FALSE );

	// 이름 값
	m_pNameLabelValue->SetCaption( m_pStringGrid->GetItem( iIndex, 0 ) ) ;
	// 클래스 값
	m_pClassLabelValue->SetCaption( m_pStringGrid->GetItem( iIndex, 1 ) ) ;
	// 레벨 값
	m_pLevelLabelValue->SetCaption( m_pStringGrid->GetItem( iIndex, 2 ) ) ;


	// 이름 기억해두기
	strcpy( m_szSelectedMember, m_pStringGrid->GetItem( iIndex, 0 ) ) ;

	// 위치값.
	CRYLGameData*	pGame = CRYLGameData::Instance() ;

	if ( m_dwStartSlot == pGame->m_csStatus.m_lstFriendList.size() )
		m_dwStartSlot -= 10;

	std::list<FriendInfo *>::iterator it = FindIter( m_dwStartSlot ) ;

	for ( INT i = 0 ; i < GetCurrentPageLine() && it != pGame->m_csStatus.m_lstFriendList.end(); ++i, ++it )
	{		
		if (strcmp((* it )->m_szName, m_szSelectedMember)==0)
		{
			SERVER_ID serverID;
			serverID.dwID = (*it)->m_dwServerID;

			m_pZoneLabelValue->SetCaption(const_cast<char*>(CRYLNetworkData::Instance()->GetZoneName(serverID.GetZone()))) ;

			CRYLGameScene*		pScene	= ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );
			CRYLGuildFrameDlg*	pDlg	= ( CRYLGuildFrameDlg* )pScene->GetGuildFrameDlg();
			CRYLGuildInfoPanel* pPanel	= ( CRYLGuildInfoPanel* )pDlg->GetGuildInfoPanel();

			// 길드 마크 출력
			LPDIRECT3DDEVICE8   lpD3DDevice = BaseGraphicsLayer::GetDevice();

			CRYLSprite* pSprite = NULL;

			if (strcmp(m_pStringGrid->GetItem(iIndex, 0), pGame->m_csStatus.GetName())==0)
			{
				pSprite = pPanel->GetGuildSprite(pGame->m_csStatus.GetGuild());
			}
			else
			{
				pSprite = pPanel->GetGuildSprite((*it)->m_dwGID);
			}

			if (pSprite)
			{
				m_pGuildMarkSprite->SetVisible( TRUE );
				m_pGuildMarkSprite->AttachImageTexture( pSprite );
			}				

			break;
		}
	}
}

VOID		CRYLFriendPanel::DlgClick( CGMUIObject* Sender, INT X, INT Y ) 
{
	if ( !g_pFriendPanel )
		return ;

	switch ( Sender->GetInstance() )
	{
	case RYLFRIENDPANEL::FRIEND_STRINGGRID		:
		g_pFriendPanel->SetWhisperCharacter() ;
		break ;

	case RYLFRIENDPANEL::DELFRIENDLIST_BUTTON	:
		g_pFriendPanel->DelFriendListEvent() ;
		break ;

	case RYLFRIENDPANEL::ADDREFUSALLIST_BUTTON	:
		g_pFriendPanel->AddResusalListEvent() ;
		break ;

	case RYLFRIENDPANEL::PAGEPREV_BUTTON		:
		g_pFriendPanel->FriendListPrev() ;
		break ;

	case RYLFRIENDPANEL::PAGENEXT_BUTTON		:
		g_pFriendPanel->FriendListNext() ;
		break ;
	}
}

VOID		CRYLFriendPanel::DlgDBClick( CGMUIObject* Sender, INT Button, INT Shift, INT X, INT Y ) 
{
	if ( !g_pFriendPanel )
		return ;

	switch ( Sender->GetInstance() )
	{
	case RYLFRIENDPANEL::FRIEND_STRINGGRID		:
		// 귀속말 보내기
		static_cast< CRYLSocietyFrameDlg* >( g_pFriendPanel->GetParentDlg() )->SetWhisperCharacter( g_pFriendPanel->GetSelectedMemberName() ) ;
		break ;
	}
}

// 친구 리스트 페이지 버튼
VOID		CRYLFriendPanel::FriendListPrev() 
{
	if ( m_dwStartSlot != 0 )
	{
		m_dwStartSlot -= 10 ;
		FriendListUpdate() ;
	}
}

VOID		CRYLFriendPanel::FriendListNext() 
{
	CRYLGameData*	pGame = CRYLGameData::Instance() ;
	if ( pGame->m_csStatus.m_lstFriendList.size() - m_dwStartSlot > 10 )
	{
		m_dwStartSlot += 10 ;
		FriendListUpdate() ;
	}
}

INT		CRYLFriendPanel::GetCurrentPageLine()
{
	INT				iCurrentLine = 0 ;
	CRYLGameData*	pGame = CRYLGameData::Instance() ;

	if ( pGame->m_csStatus.m_lstFriendList.empty() )
		return 0 ;

	// 첫 페이지가 10개 미만일때
	if ( pGame->m_csStatus.m_lstFriendList.size() < 10 )
	{
		return pGame->m_csStatus.m_lstFriendList.size() ;
	}

	INT iResultPage = pGame->m_csStatus.m_lstFriendList.size() - m_dwStartSlot  ;

	// 마지막 페이지
	if ( iResultPage < 10 )
	{
		// 음수일때
		if ( iResultPage < 0 )
		{
			iCurrentLine = pGame->m_csStatus.m_lstFriendList.size() % 10 ;
		}
		else
		{
			 iCurrentLine = iResultPage ;
		}
	}
	else
	{
		iCurrentLine = 10 ;
	}
	
	return iCurrentLine ;
}

