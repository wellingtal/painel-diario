#include <winsock2.h>
#include "RYLRankingDlg.h"
#include "BaseDataDefine.h"

#include "RYLButton.h"
#include "RYLStringGrid.h"
#include "RYLLabel.h"
#include "RYLComboBox.h"

#include "RYLImageManager.h"
#include "RYLImage.h"

#include "RYLSpriteList.h"
#include "GMImageDib.h"
#include "WinInput.h"
#include "RYLGameData.h"
#include "RYLNetworkData.h"
#include "RYLStringTable.h"

#include <Network/Packet/PacketStruct/RankingPacket.h>
#include <Network/ClientSocket/Send/SendCommunity.h>
#include <Creature/Character/CharacterClass.h>
#include <RenderOption.h>
#include "GMMemory.h"
CRYLRankingDlg*		g_RankingDlg = NULL ;

CRYLRankingDlg::CRYLRankingDlg() 
{
	m_pServerNameLabel				= NULL ;		// 서버이름
	m_pServerNameValueLabel			= NULL ;		// 서버이름 값
	m_pCurrentRankingLabel			= NULL ;		// 현재 랭킹
	m_pCurrentRankingValueLabel		= NULL ;		// 현재 랭킹값
	m_pSortLabel					= NULL ;		// 정렬 방식

	m_pRankingLabel					= NULL ;		// 순위
	m_pClassLabel					= NULL ;		// 클래스
	m_pNameLabel					= NULL ;		// 이름
	m_pPointLabel					= NULL ;		// 포인트
	m_pLebelLabel					= NULL ;		// 레벨

	m_pPrevButton					= NULL ;		// 이전 페이지
	m_pNextButton					= NULL ;		// 다음 페이지
	m_pRankingStrGrid				= NULL ;		// 랭킹 리스트
	m_pClassCBBox					= NULL ;		// 클래스 콤보박스

	m_cCurrentPage					= 0 ;			// 현재 페이지
	m_iClass						= 0 ;			// 선택중인 클래스
	
	//----------------------------------------------------------------

	RECT rt ;
	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;

	CRYLSpriteList::Instance()->CreateGMImage( 323, 15, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 0, 256, 15) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_RANKING01_256x256 ) ) ;
	SetRect( &rt, 0, 0, 67, 15 ) ;
	pImageDIB->DrawRect( &rt, 256, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_RANKING02_256x256 ) ) ;
	
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	AttachCaptionTexture( ( CGMTexture*)pImage ) ;
	GM_DELETE( pImageDIB ) ;


	CRYLSpriteList::Instance()->CreateGMImage( 323, 398, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 15, 256, 256 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_RANKING01_256x256 ) ) ;

	SetRect( &rt, 0, 15, 67, 256 ) ;
	pImageDIB->DrawRect( &rt, 256, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_RANKING02_256x256 ) ) ;
	
	SetRect( &rt, 0, 0, 256, 157) ;
	pImageDIB->DrawRect( &rt, 0, 241, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_RANKING03_256x256 ) ) ;
	
	SetRect( &rt, 67, 0, 134, 157) ;
	pImageDIB->DrawRect( &rt, 256, 241, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_RANKING02_256x256 ) ) ;
	
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
    AttachWindowTexture( ( CGMTexture*)pImage ) ;
	GM_DELETE( pImageDIB ) ;

	
	SetWidth( 322 ) ;
	SetHeight( 413 ) ;
	SetLeft( ( CGMUIScreen::GetInstance()->GetWidth() - GetWidth() ) / 2 ) ;
	SetTop( ( CGMUIScreen::GetInstance()->GetHeight() - GetHeight() ) / 2 ) ;
	SetFormType( RYLDLG_TYPE_CLOSEBUTTON ) ;

	InitCtrl() ;
	g_RankingDlg = this ;
}

CRYLRankingDlg::~CRYLRankingDlg() 
{
	FinalCtrl() ;
}

VOID	CRYLRankingDlg::InitCtrl() 
{
	RECT rt ;
	CGMImageDib*			pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;

	// 클레스 콤보박스
	m_pClassCBBox = new CRYLComboBox( this ) ;
	SetRect( &rt, 199, 201, 211, 213 ) ;
	m_pClassCBBox->AttachButtonImageEx( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 213, 201, 225, 213 ) ;
	m_pClassCBBox->AttachButtonImageEx( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 227, 201, 239, 213 ) ;
	m_pClassCBBox->AttachButtonImageEx( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 199, 201, 211, 213 ) ;
	m_pClassCBBox->AttachButtonImageEx( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;

	switch( CRYLGameData::Instance()->m_dwClientRace )
	{
		case CClass::RaceType::HUMAN : 
		{
			m_pClassCBBox->AddItem( CRYLStringTable::m_strString[ 2227 ], 0xffffffff, 0xff00ffff, 0xffffffff ) ;
			m_pClassCBBox->AddItem( CRYLStringTable::m_strString[ 455 ], 0xffffffff, 0xff00ffff, 0xffffffff ) ;
			m_pClassCBBox->AddItem( CRYLStringTable::m_strString[ 456 ], 0xffffffff, 0xff00ffff, 0xffffffff ) ;
			//m_pClassCBBox->AddItem( CRYLStringTable::m_strString[ 457 ], 0xffffffff, 0xff00ffff, 0xffffffff ) ;//assassin rank
			m_pClassCBBox->AddItem( CRYLStringTable::m_strString[ 458 ], 0xffffffff, 0xff00ffff, 0xffffffff ) ;
			m_pClassCBBox->AddItem( CRYLStringTable::m_strString[ 459 ], 0xffffffff, 0xff00ffff, 0xffffffff ) ;
			//m_pClassCBBox->AddItem( CRYLStringTable::m_strString[ 460 ], 0xffffffff, 0xff00ffff, 0xffffffff ) ;//enchanter rank
			//m_pClassCBBox->AddItem( CRYLStringTable::m_strString[ 461 ], 0xffffffff, 0xff00ffff, 0xffffffff ) ;//priest rank
			m_pClassCBBox->AddItem( CRYLStringTable::m_strString[ 462 ], 0xffffffff, 0xff00ffff, 0xffffffff ) ;
			//m_pClassCBBox->AddItem( CRYLStringTable::m_strString[ 2227 ], 0xffffffff, 0xff00ffff, 0xffffffff ) ;//all
			m_pClassCBBox->AddItem( CRYLStringTable::m_strString[ 469 ], 0xffffffff, 0xff00ffff, 0xffffffff ) ;//templar
			m_pClassCBBox->AddItem( CRYLStringTable::m_strString[ 470 ], 0xffffffff, 0xff00ffff, 0xffffffff ) ;//attacker
			//m_pClassCBBox->AddItem( CRYLStringTable::m_strString[ 471 ], 0xffffffff, 0xff00ffff, 0xffffffff ) ;//gunner rank
			m_pClassCBBox->AddItem( CRYLStringTable::m_strString[ 472 ], 0xffffffff, 0xff00ffff, 0xffffffff ) ;//rune off
			m_pClassCBBox->AddItem( CRYLStringTable::m_strString[ 473 ], 0xffffffff, 0xff00ffff, 0xffffffff ) ;//life off
			m_pClassCBBox->AddItem( CRYLStringTable::m_strString[ 474 ], 0xffffffff, 0xff00ffff, 0xffffffff ) ;//shadow
			break ;
		}

		case CClass::RaceType::AKHAN : 
		{
			m_pClassCBBox->AddItem( CRYLStringTable::m_strString[ 2227 ], 0xffffffff, 0xff00ffff, 0xffffffff ) ;
			m_pClassCBBox->AddItem( CRYLStringTable::m_strString[ 455 ], 0xffffffff, 0xff00ffff, 0xffffffff ) ;
			m_pClassCBBox->AddItem( CRYLStringTable::m_strString[ 456 ], 0xffffffff, 0xff00ffff, 0xffffffff ) ;
			//m_pClassCBBox->AddItem( CRYLStringTable::m_strString[ 457 ], 0xffffffff, 0xff00ffff, 0xffffffff ) ;//assassin rank
			m_pClassCBBox->AddItem( CRYLStringTable::m_strString[ 458 ], 0xffffffff, 0xff00ffff, 0xffffffff ) ;
			m_pClassCBBox->AddItem( CRYLStringTable::m_strString[ 459 ], 0xffffffff, 0xff00ffff, 0xffffffff ) ;
			//m_pClassCBBox->AddItem( CRYLStringTable::m_strString[ 460 ], 0xffffffff, 0xff00ffff, 0xffffffff ) ;//enchanter rank
			//m_pClassCBBox->AddItem( CRYLStringTable::m_strString[ 461 ], 0xffffffff, 0xff00ffff, 0xffffffff ) ;//priest rank
			m_pClassCBBox->AddItem( CRYLStringTable::m_strString[ 462 ], 0xffffffff, 0xff00ffff, 0xffffffff ) ;

			m_pClassCBBox->AddItem( CRYLStringTable::m_strString[ 469 ], 0xffffffff, 0xff00ffff, 0xffffffff ) ;//templar
			m_pClassCBBox->AddItem( CRYLStringTable::m_strString[ 470 ], 0xffffffff, 0xff00ffff, 0xffffffff ) ;//attacker
			//m_pClassCBBox->AddItem( CRYLStringTable::m_strString[ 471 ], 0xffffffff, 0xff00ffff, 0xffffffff ) ;//gunner rank
			m_pClassCBBox->AddItem( CRYLStringTable::m_strString[ 472 ], 0xffffffff, 0xff00ffff, 0xffffffff ) ;//rune off
			m_pClassCBBox->AddItem( CRYLStringTable::m_strString[ 473 ], 0xffffffff, 0xff00ffff, 0xffffffff ) ;//life off
			m_pClassCBBox->AddItem( CRYLStringTable::m_strString[ 474 ], 0xffffffff, 0xff00ffff, 0xffffffff ) ;//shadow
			break ;
		}
	}

	m_pClassCBBox->SetColor( 0xff000000 ) ;
		
	m_pClassCBBox->OnClickEvent = DlgClick ;
	m_pClassCBBox->SetLeft( 61 ) ;
	m_pClassCBBox->SetTop( 46 ) ;//set top(higher go down
	m_pClassCBBox->SetWidth( 72 ) ;//set righter(higher go right
	m_pClassCBBox->SetHeight( 15 ) ;
	m_pClassCBBox->SetButtonPos() ;

	m_pClassCBBox->SetFontColor( 0xffffffff ) ;
	m_pClassCBBox->SelectMenuData( 0 ) ;
	
	m_pClassCBBox->GetButton()->SetInstance( RYLRANKINGDLG::RANKING_COMBO ) ;//wasnt commented out, disable Class rank hz
	m_pClassCBBox->GetButton()->OnClickEvent = DlgClick ;
	m_pClassCBBox->GetMenu()->SetInstance( RYLRANKINGDLG::RANKING_MENU ) ;
	m_pClassCBBox->GetMenu()->OnClickEvent = DlgClick ;

	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;
	CRYLSpriteList::Instance()->CreateGMImage( 77, 181, &pImageDIB ) ;//was 148
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	if( CClass::RaceType::HUMAN == CRYLGameData::Instance()->m_dwClientRace )
	{
		pImageDIB->MakeFrameRect( 1, 1, 76, 180, 0xff959595, 0xff000000,  1 ) ;//was 147
	}
	else
	{
		pImageDIB->MakeFrameRect( 1, 1, 76, 180, 0xff959595, 0xff000000,  1 ) ;//was 116
	}
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	GM_DELETE( pImageDIB ) ;
	m_pClassCBBox->GetMenu()->AttachMenuImage( pImage ) ;

	AttachGMUIControl( m_pClassCBBox ) ;

	// 랭킹창
	m_pRankingDlgLabel = new CRYLLabel( this ) ;		
	m_pRankingDlgLabel->SetLeft( 9 ) ;
	m_pRankingDlgLabel->SetTop( 18 ) ;
	m_pRankingDlgLabel->SetFontColor (0xffffffff);
	m_pRankingDlgLabel->SetAutoSize( TRUE ) ;
	m_pRankingDlgLabel->SetCaption( CRYLStringTable::m_strString[ 1943 ] ) ;
	AttachGMUIControl( m_pRankingDlgLabel ) ;

	// 서버이름
	m_pServerNameLabel = new CRYLLabel( this ) ;		
	m_pServerNameLabel->SetLeft( 147 ) ;
	m_pServerNameLabel->SetTop( 47 ) ;
	m_pServerNameLabel->SetFontColor (0xffffffff);
	m_pServerNameLabel->SetAutoSize( TRUE ) ;
	m_pServerNameLabel->SetCaption( CRYLStringTable::m_strString[ 2237 ] ) ;
	AttachGMUIControl( m_pServerNameLabel ) ;

	// 서버이름 값
	m_pServerNameValueLabel = new CRYLLabel( this ) ;		
	m_pServerNameValueLabel->SetLeft( 211 ) ;
	m_pServerNameValueLabel->SetTop( 47 ) ;
	m_pServerNameValueLabel->SetFontColor (0xffffffff);
	m_pServerNameValueLabel->SetAutoSize( TRUE ) ;
	m_pServerNameValueLabel->SetCaption( "" ) ;
	AttachGMUIControl( m_pServerNameValueLabel ) ;

	// 현재 랭킹
	m_pCurrentRankingLabel = new CRYLLabel( this ) ;		
	m_pCurrentRankingLabel->SetLeft( 190 ) ;
	m_pCurrentRankingLabel->SetTop( 70 ) ;
	m_pCurrentRankingLabel->SetFontColor (0xffffffff);
	m_pCurrentRankingLabel->SetAutoSize( TRUE ) ;
	m_pCurrentRankingLabel->SetCaption( CRYLStringTable::m_strString[ 2238 ] ) ;
	AttachGMUIControl( m_pCurrentRankingLabel ) ;

	// 현재 랭킹값
	m_pCurrentRankingValueLabel = new CRYLLabel( this ) ;		
	m_pCurrentRankingValueLabel->SetLeft( 255 ) ;
	m_pCurrentRankingValueLabel->SetTop( 70 ) ;
	m_pCurrentRankingValueLabel->SetFontColor (0xffffffff);
	m_pCurrentRankingValueLabel->SetAutoSize( TRUE ) ;
	m_pCurrentRankingValueLabel->SetCaption( "" ) ;
	AttachGMUIControl( m_pCurrentRankingValueLabel ) ;

	// 순위
	m_pRankingLabel = new CRYLLabel( this ) ;		
	m_pRankingLabel->SetLeft( 15 ) ;
	m_pRankingLabel->SetTop( 94 ) ;
	m_pRankingLabel->SetFontColor (0xffffffff);
	m_pRankingLabel->SetAutoSize( TRUE ) ;
	m_pRankingLabel->SetCaption( CRYLStringTable::m_strString[ 2013 ] ) ;
	AttachGMUIControl( m_pRankingLabel ) ;

	// 클래스
	m_pClassLabel = new CRYLLabel( this ) ;		
	m_pClassLabel->SetLeft( 60 ) ;
	m_pClassLabel->SetTop( 94 ) ;
	m_pClassLabel->SetFontColor (0xffffffff);
	m_pClassLabel->SetAutoSize( TRUE ) ;
	m_pClassLabel->SetCaption( CRYLStringTable::m_strString[ 1141 ] ) ;
	AttachGMUIControl( m_pClassLabel ) ;

	// 이름
	m_pNameLabel = new CRYLLabel( this ) ;		
	m_pNameLabel->SetLeft( 153 ) ;
	m_pNameLabel->SetTop( 94 ) ;
	m_pNameLabel->SetFontColor (0xffffffff);
	m_pNameLabel->SetAutoSize( TRUE ) ;
	m_pNameLabel->SetCaption( CRYLStringTable::m_strString[ 1144 ] ) ;
	AttachGMUIControl( m_pNameLabel ) ;

	// 포인트
	m_pPointLabel = new CRYLLabel( this ) ;		
	m_pPointLabel->SetLeft( 234 ) ;
	m_pPointLabel->SetTop( 94 ) ;
	m_pPointLabel->SetFontColor (0xffffffff);
	m_pPointLabel->SetAutoSize( TRUE ) ;
	m_pPointLabel->SetCaption( CRYLStringTable::m_strString[ 2239 ] ) ;
	AttachGMUIControl( m_pPointLabel ) ;

	// 레벨
	m_pLebelLabel = new CRYLLabel( this ) ;		
	m_pLebelLabel->SetLeft( 287 ) ;
	m_pLebelLabel->SetTop( 94 ) ;
	m_pLebelLabel->SetFontColor (0xffffffff);
	m_pLebelLabel->SetAutoSize( TRUE ) ;
	m_pLebelLabel->SetCaption( CRYLStringTable::m_strString[ 192 ] ) ;
	AttachGMUIControl( m_pLebelLabel ) ;

	// 정렬 방식
	m_pSortLabel = new CRYLLabel( this ) ;		
	m_pSortLabel->SetLeft( 9 ) ;
	m_pSortLabel->SetTop( 47 ) ;
	m_pSortLabel->SetFontColor (0xffffffff);
	m_pSortLabel->SetAutoSize( TRUE ) ;
	m_pSortLabel->SetCaption( CRYLStringTable::m_strString[ 2011 ] ) ;
	AttachGMUIControl( m_pSortLabel ) ;
	
	// 이전 페이지
	m_pPrevButton = new CRYLButton( this ) ;
	m_pPrevButton->SetLeft( 287 ) ;
	m_pPrevButton->SetTop( 387 ) ;
	m_pPrevButton->OnClickEvent = DlgClick ;
	m_pPrevButton->SetInstance( RYLRANKINGDLG::PREVRANK_BUTTON ) ;
	SetRect( &rt, 217, 215, 230, 227 ) ;
	m_pPrevButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 217, 243, 230, 255 ) ;
	m_pPrevButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 217, 229, 230, 241 ) ;
	m_pPrevButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 217, 215, 230, 227 ) ;
	m_pPrevButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pPrevButton ) ;

	m_pNextButton = new CRYLButton( this ) ;
	m_pNextButton->SetLeft( 302 ) ;
	m_pNextButton->SetTop( 387 ) ;
	m_pNextButton->OnClickEvent = DlgClick ;
	m_pNextButton->SetInstance( RYLRANKINGDLG::NEXTRANK_BUTTON ) ;
	SetRect( &rt, 232, 215, 245, 227 ) ;
	m_pNextButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 232, 243, 245, 255 ) ;
	m_pNextButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 232, 229, 245, 241 ) ;
	m_pNextButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 232, 215, 245, 227 ) ;
	m_pNextButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pNextButton ) ;
	m_pRankingStrGrid = new CRYLStringGrid( this, FALSE ) ;
	m_pRankingStrGrid->SetLeft( 13 ) ;
	m_pRankingStrGrid->SetTop( 113 ) ;
	m_pRankingStrGrid->SetWidth( 292 ) ;
	m_pRankingStrGrid->SetHeight( 264 ) ;
	m_pRankingStrGrid->SetColCount( 18 ) ;
	m_pRankingStrGrid->SetFontColor( 0xffffffff ) ;
//	m_pRankingStrGrid->SetInstance( RYLGUILDPANEL::GUILD_STRINGGRID ) ;
	m_pRankingStrGrid->OnClickEvent = DlgClick ;
	AttachGMUIControl( m_pRankingStrGrid ) ;
}

HRESULT		CRYLRankingDlg::Update( BOOL &bClick, BOOL &bEdge ) 
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

	static CHAR strText[ MAX_PATH ] ;

	switch( CRYLNetworkData::m_dwServerID )
	{
		case 0:		strcpy( strText, CRYLStringTable::m_strString[ 2228 ] ) ;	break ;
		case 1:		strcpy( strText, CRYLStringTable::m_strString[ 2229 ] ) ;	break ;
		case 2:		strcpy( strText, CRYLStringTable::m_strString[ 2230 ] ) ;	break ;
		case 3:		strcpy( strText, CRYLStringTable::m_strString[ 2231 ] ) ;	break ;
		default:	strcpy( strText, CRYLStringTable::m_strString[ 2232 ] ) ;	break ;
	}
	m_pServerNameValueLabel->SetCaption( strText ) ;
	
	if ( CRYLGameData::Instance()->m_csStatus.m_dwMyTotalRanking )
	{
		sprintf( strText, "%d", CRYLGameData::Instance()->m_csStatus.m_dwMyTotalRanking ) ;
	} 
	else
	{
		strcpy( strText, CRYLStringTable::m_strString[ 2236 ] ) ;
	}
	
	m_pCurrentRankingValueLabel->SetCaption( strText ) ;


	return S_OK ;
}

VOID	CRYLRankingDlg::FinalCtrl() 
{
	GM_DELETE( m_pRankingDlgLabel ) ;
	GM_DELETE( m_pServerNameLabel ) ; 			// 서버이름
	GM_DELETE( m_pServerNameValueLabel ) ;		// 서버이름 값
	GM_DELETE( m_pCurrentRankingLabel ) ;		// 현재 랭킹
	GM_DELETE( m_pCurrentRankingValueLabel ) ;	// 현재 랭킹값

	GM_DELETE( m_pRankingLabel ) ;				// 순위
	GM_DELETE( m_pClassLabel ) ;				// 클래스
	GM_DELETE( m_pNameLabel	) ;					// 이름
	GM_DELETE( m_pPointLabel ) ;				// 포인트
	GM_DELETE( m_pLebelLabel ) ;				// 레벨

	GM_DELETE( m_pPrevButton ) ;				// 이전 페이지
	GM_DELETE( m_pNextButton ) ;				// 다음 페이지
	GM_DELETE( m_pRankingStrGrid ) ;			// 랭킹 리스트
	GM_DELETE( m_pClassCBBox ) ;				// 클레스 콤보박스

	GM_DELETE( m_pSortLabel ) ;					// 소트 방식
}

VOID	CRYLRankingDlg::Show( CGMUIObject* Sender ) 
{
}

VOID	CRYLRankingDlg::Hide( CGMUIObject* Sender ) 
{
	
}

VOID	CRYLRankingDlg::GMUIPaint( INT xx, INT yy ) 
{
	CRYLDialog::GMUIPaint( xx, yy ) ;
}

VOID	CRYLRankingDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y )
{
	if ( !g_RankingDlg )
		return ;

	switch( Sender->GetInstance() )
	{
		case RYLRANKINGDLG::PREVRANK_BUTTON :
			SendPacket::CharRankingInfo( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, 
				static_cast<unsigned char>(g_RankingDlg->m_iClass), g_RankingDlg->GetCurrentPage() - 1 ) ;
			break ;

		case RYLRANKINGDLG::NEXTRANK_BUTTON : 		
			SendPacket::CharRankingInfo( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, 
				static_cast<unsigned char>(g_RankingDlg->m_iClass), g_RankingDlg->GetCurrentPage() + 1 ) ;
			break ;

		case RYLRANKINGDLG::RANKING_COMBO :
//			g_RankingDlg->InsertDataListbox( g_RankingDlg->m_pClassCBBox->GetMenu()->GetItemIndex() ) ;//was commented out hz show class rank
			break ;

		case RYLRANKINGDLG::RANKING_MENU :
		{
			switch( CRYLGameData::Instance()->m_dwClientRace )
			{
				case CClass::RaceType::HUMAN : 
				{
					switch ( g_RankingDlg->m_pClassCBBox->GetMenu()->GetItemIndex() )
					{
						case 0 :/*전체순위*/	g_RankingDlg->m_iClass = 0 ; 	break ;
						case 1 :/*디펜더*/		g_RankingDlg->m_iClass = 5 ; 	break ;
						case 2 :/*워리어*/		g_RankingDlg->m_iClass = 6 ; 	break ;
						//case 3 :/*어쎄신*/		g_RankingDlg->m_iClass = 7 ; 	break ;
						case 3 :/*아처*/		g_RankingDlg->m_iClass = 8 ; 	break ;
						case 4 :/*소서러*/		g_RankingDlg->m_iClass = 9 ; 	break ;
						//case 6 :/*인챈터*/		g_RankingDlg->m_iClass = 10 ; 	break ;
						//case 7 :/*프리스트*/	g_RankingDlg->m_iClass = 11 ; 	break ;
						case 5 :/*클레릭*/		g_RankingDlg->m_iClass = 12 ; 	break ;
						case 6 :/*템플러*/		g_RankingDlg->m_iClass = 19 ; 	break ;
						case 7 :/*어태커*/		g_RankingDlg->m_iClass = 20 ; 	break ;
						//case 3 :/*거너*/		g_RankingDlg->m_iClass = 21 ; 	break ;
						case 8 :/*룬오프*/		g_RankingDlg->m_iClass = 22 ; 	break ;
						case 9 :/*라이프오프*/	g_RankingDlg->m_iClass = 23 ; 	break ;
						case 110 :/*쉐도우오프*/	g_RankingDlg->m_iClass = 24 ; 	break ;
					}
					break ;
				}

				case CClass::RaceType::AKHAN : 
				{
					switch ( g_RankingDlg->m_pClassCBBox->GetMenu()->GetItemIndex() )
					{
						case 0 :/*전체순위*/	g_RankingDlg->m_iClass = 0 ; 	break ;
						case 1 :/*디펜더*/		g_RankingDlg->m_iClass = 5 ; 	break ;
						case 2 :/*워리어*/		g_RankingDlg->m_iClass = 6 ; 	break ;
						//case 3 :/*어쎄신*/		g_RankingDlg->m_iClass = 7 ; 	break ;
						case 3 :/*아처*/		g_RankingDlg->m_iClass = 8 ; 	break ;
						case 4 :/*소서러*/		g_RankingDlg->m_iClass = 9 ; 	break ;
						//case 6 :/*인챈터*/		g_RankingDlg->m_iClass = 10 ; 	break ;
						//case 7 :/*프리스트*/	g_RankingDlg->m_iClass = 11 ; 	break ;
						case 5 :/*클레릭*/		g_RankingDlg->m_iClass = 12 ; 	break ;
						case 6 :/*템플러*/		g_RankingDlg->m_iClass = 19 ; 	break ;
						case 7 :/*어태커*/		g_RankingDlg->m_iClass = 20 ; 	break ;
						//case 3 :/*거너*/		g_RankingDlg->m_iClass = 21 ; 	break ;
						case 8 :/*룬오프*/		g_RankingDlg->m_iClass = 22 ; 	break ;
						case 9 :/*라이프오프*/	g_RankingDlg->m_iClass = 23 ; 	break ;
						case 10 :/*쉐도우오프*/	g_RankingDlg->m_iClass = 24 ; 	break ;
					}
					break ;
				}
			}

			SendPacket::CharRankingInfo( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, g_RankingDlg->m_iClass, 0 ) ;
			break ;
		}

		case RYLRANKINGDLG::RANKING_LISTBOX : 		
			break ;
	}
}

VOID	CRYLRankingDlg::InsertDataListbox( INT iClass ) 
{
	m_pRankingStrGrid->ClearItem() ;

	INT iRank = m_cCurrentPage * PktRankingInfo::MAX_NUM_PER_PAGE + 1 ;

	// 순위
	// 클레스
	// 이름
	// 포인트
	// 레벨
	switch( iClass )
	{
		case 0:		// 전체
			{
				//std::vector<RankingNode>			m_vecAll ;			// 전체 리스트		
				for ( std::vector<RankingNode>::iterator it = m_vecAll.begin() ; it != m_vecAll.end() ; ++ it )
				{
					CHAR szBuff[ 32 ] ;
					sprintf( szBuff, "%d", iRank ) ;
					m_pRankingStrGrid->AddItem( 0, szBuff, 38, 0xffffffff ) ;		// 순위
					m_pRankingStrGrid->AddItem( 1, CRYLGameData::Instance()->GetClassName( it->m_cClass ), 73, 0xffffffff ) ;		// 클레스
					m_pRankingStrGrid->AddItem( 2, it->m_szCharName, 93, 0xffffffff ) ;			// 이름
					sprintf( szBuff, "%d", it->m_dwPoint ) ;
					m_pRankingStrGrid->AddItem( 3, szBuff, 78, 0xffffffff ) ;			// 포인트
					sprintf( szBuff, "%d", it->m_cLevel ) ;
					m_pRankingStrGrid->AddItem( 4, szBuff, 17, 0xffffffff ) ;			// 레벨
					++ iRank ;
				}
			}
			break ;			
		case 1:		// 파이터
			break ;
		case 2:		// 로그
			break ;
		case 3:		// 메이지
			break ;
		case 4:		// 성직자
			break ;
		case 5:		// 디펜더
			{
				//std::vector<RankingNode>			m_vecDefender ;		// 디펜더 리스트		
				for ( std::vector<RankingNode>::iterator it = m_vecDefender.begin() ; it != m_vecDefender.end() ; ++ it )
				{
					CHAR szBuff[ 32 ] ;
					sprintf( szBuff, "%d", iRank ) ;
					m_pRankingStrGrid->AddItem( 0, szBuff, 38, 0xffffffff ) ;		// 순위
					m_pRankingStrGrid->AddItem( 1, CRYLGameData::Instance()->GetClassName( it->m_cClass ), 73, 0xffffffff ) ;		// 클레스
					m_pRankingStrGrid->AddItem( 2, it->m_szCharName, 93, 0xffffffff ) ;			// 이름
					sprintf( szBuff, "%d", it->m_dwPoint ) ;
					m_pRankingStrGrid->AddItem( 3, szBuff, 78, 0xffffffff ) ;			// 포인트
					sprintf( szBuff, "%d", it->m_cLevel ) ;
					m_pRankingStrGrid->AddItem( 4, szBuff, 17, 0xffffffff ) ;			// 레벨
					++ iRank ;
				}
			}
			break ;
		case 6:		// 워리어
			{
				//std::vector<RankingNode>			m_vecWarrior ;		// 워리어 리스트
				for ( std::vector<RankingNode>::iterator it = m_vecWarrior.begin() ; it != m_vecWarrior.end(); ++ it )
				{
					CHAR szBuff[ 32 ] ;
					sprintf( szBuff, "%d", iRank ) ;
					m_pRankingStrGrid->AddItem( 0, szBuff, 38, 0xffffffff ) ;		// 순위
					m_pRankingStrGrid->AddItem( 1, CRYLGameData::Instance()->GetClassName( it->m_cClass ), 73, 0xffffffff ) ;		// 클레스
					m_pRankingStrGrid->AddItem( 2, it->m_szCharName, 93, 0xffffffff ) ;			// 이름
					sprintf( szBuff, "%d", it->m_dwPoint ) ;
					m_pRankingStrGrid->AddItem( 3, szBuff, 78, 0xffffffff ) ;			// 포인트
					sprintf( szBuff, "%d", it->m_cLevel ) ;
					m_pRankingStrGrid->AddItem( 4, szBuff, 17, 0xffffffff ) ;			// 레벨
					++ iRank ;
				}
			}
			break ;
		case 7:		// 어쌔신
			{
				//std::vector<RankingNode>			m_vecAssassin ;		// 어쎄신 리스트
				for ( std::vector<RankingNode>::iterator it = m_vecAssassin.begin() ; it != m_vecAssassin.end() ; ++ it )
				{
					CHAR szBuff[ 32 ] ;
					sprintf( szBuff, "%d", iRank ) ;
					m_pRankingStrGrid->AddItem( 0, szBuff, 38, 0xffffffff ) ;		// 순위
					m_pRankingStrGrid->AddItem( 1, CRYLGameData::Instance()->GetClassName( it->m_cClass ), 73, 0xffffffff ) ;		// 클레스
					m_pRankingStrGrid->AddItem( 2, it->m_szCharName, 93, 0xffffffff ) ;			// 이름
					sprintf( szBuff, "%d", it->m_dwPoint ) ;
					m_pRankingStrGrid->AddItem( 3, szBuff, 78, 0xffffffff ) ;			// 포인트
					sprintf( szBuff, "%d", it->m_cLevel ) ;
					m_pRankingStrGrid->AddItem( 4, szBuff, 17, 0xffffffff ) ;			// 레벨
					++ iRank ;
				}
			}
			break ;
		case 8:		// 아처
			{
				//std::vector<RankingNode>			m_vecArcher ;		// 아처 리스트
				for ( std::vector<RankingNode>::iterator it = m_vecArcher.begin() ; it != m_vecArcher.end() ; ++ it )
				{
					CHAR szBuff[ 32 ] ;
					sprintf( szBuff, "%d", iRank ) ;
					m_pRankingStrGrid->AddItem( 0, szBuff, 38, 0xffffffff ) ;		// 순위
					m_pRankingStrGrid->AddItem( 1, CRYLGameData::Instance()->GetClassName( it->m_cClass ), 73, 0xffffffff ) ;		// 클레스
					m_pRankingStrGrid->AddItem( 2, it->m_szCharName, 93, 0xffffffff ) ;			// 이름
					sprintf( szBuff, "%d", it->m_dwPoint ) ;
					m_pRankingStrGrid->AddItem( 3, szBuff, 78, 0xffffffff ) ;			// 포인트
					sprintf( szBuff, "%d", it->m_cLevel ) ;
					m_pRankingStrGrid->AddItem( 4, szBuff, 17, 0xffffffff ) ;			// 레벨
					++ iRank ;
				}
			}
			
			break ;
		case 9:		// 소서러
			{
				//std::vector<RankingNode>			m_vecSorcerer ;		// 소서리 리스트
				for ( std::vector<RankingNode>::iterator it = m_vecSorcerer.begin() ; it != m_vecSorcerer.end() ; ++ it )
				{
					CHAR szBuff[ 32 ] ;
					sprintf( szBuff, "%d", iRank ) ;
					m_pRankingStrGrid->AddItem( 0, szBuff, 38, 0xffffffff ) ;		// 순위
					m_pRankingStrGrid->AddItem( 1, CRYLGameData::Instance()->GetClassName( it->m_cClass ), 73, 0xffffffff ) ;		// 클레스
					m_pRankingStrGrid->AddItem( 2, it->m_szCharName, 93, 0xffffffff ) ;			// 이름
					sprintf( szBuff, "%d", it->m_dwPoint ) ;
					m_pRankingStrGrid->AddItem( 3, szBuff, 78, 0xffffffff ) ;			// 포인트
					sprintf( szBuff, "%d", it->m_cLevel ) ;
					m_pRankingStrGrid->AddItem( 4, szBuff, 17, 0xffffffff ) ;			// 레벨
					++ iRank ;
				}
			}
			break ;
		case 10:		// 인챈터
			{
				//std::vector<RankingNode>			m_vecEnchanter ;	// 인첸터 리스트
				for ( std::vector<RankingNode>::iterator it = m_vecEnchanter.begin() ; it != m_vecEnchanter.end() ; ++ it )
				{
					CHAR szBuff[ 32 ] ;
					sprintf( szBuff, "%d", iRank ) ;
					m_pRankingStrGrid->AddItem( 0, szBuff, 38, 0xffffffff ) ;		// 순위
					m_pRankingStrGrid->AddItem( 1, CRYLGameData::Instance()->GetClassName( it->m_cClass ), 73, 0xffffffff ) ;		// 클레스
					m_pRankingStrGrid->AddItem( 2, it->m_szCharName, 93, 0xffffffff ) ;			// 이름
					sprintf( szBuff, "%d", it->m_dwPoint ) ;
					m_pRankingStrGrid->AddItem( 3, szBuff, 78, 0xffffffff ) ;			// 포인트
					sprintf( szBuff, "%d", it->m_cLevel ) ;
					m_pRankingStrGrid->AddItem( 4, szBuff, 17, 0xffffffff ) ;			// 레벨
					++ iRank ;
				}
			}
			break ;
		case 11:		// 프리스트
			{
				//std::vector<RankingNode>			m_vecPriest ;		// 프리스트 리스트
				for ( std::vector<RankingNode>::iterator it = m_vecPriest.begin() ; it != m_vecPriest.end() ; ++ it )
				{
					CHAR szBuff[ 32 ] ;
					sprintf( szBuff, "%d", iRank ) ;
					m_pRankingStrGrid->AddItem( 0, szBuff, 38, 0xffffffff ) ;		// 순위
					m_pRankingStrGrid->AddItem( 1, CRYLGameData::Instance()->GetClassName( it->m_cClass ), 73, 0xffffffff ) ;		// 클레스
					m_pRankingStrGrid->AddItem( 2, it->m_szCharName, 93, 0xffffffff ) ;			// 이름
					sprintf( szBuff, "%d", it->m_dwPoint ) ;
					m_pRankingStrGrid->AddItem( 3, szBuff, 78, 0xffffffff ) ;			// 포인트
					sprintf( szBuff, "%d", it->m_cLevel ) ;
					m_pRankingStrGrid->AddItem( 4, szBuff, 17, 0xffffffff ) ;			// 레벨
					++ iRank ;
				}
			}
			break ;
		case 12:		// 클레릭
			{
				//std::vector<RankingNode>			m_vecCleric ;		// 클래릭 리스트
				for ( std::vector<RankingNode>::iterator it = m_vecCleric.begin() ; it != m_vecCleric.end() ; ++ it )
				{
					CHAR szBuff[ 32 ] ;
					sprintf( szBuff, "%d", iRank ) ;
					m_pRankingStrGrid->AddItem( 0, szBuff, 38, 0xffffffff ) ;		// 순위
					m_pRankingStrGrid->AddItem( 1, CRYLGameData::Instance()->GetClassName( it->m_cClass ), 73, 0xffffffff ) ;		// 클레스
					m_pRankingStrGrid->AddItem( 2, it->m_szCharName, 93, 0xffffffff ) ;			// 이름
					sprintf( szBuff, "%d", it->m_dwPoint ) ;
					m_pRankingStrGrid->AddItem( 3, szBuff, 78, 0xffffffff ) ;			// 포인트
					sprintf( szBuff, "%d", it->m_cLevel ) ;
					m_pRankingStrGrid->AddItem( 4, szBuff, 17, 0xffffffff ) ;			// 레벨
					++ iRank ;
				}
			}
			break ;
		case 17:		// 컴배턴트
			break ;
		case 18:		// 오피세이터
			break ;
		case 19:		// 템플러
			{
				//std::vector<RankingNode>			m_vecTemplar ;		// 템플러 리스트
				for ( std::vector<RankingNode>::iterator it = m_vecTemplar.begin() ; it != m_vecTemplar.end() ; ++ it )
				{
					CHAR szBuff[ 32 ] ;
					sprintf( szBuff, "%d", iRank ) ;
					m_pRankingStrGrid->AddItem( 0, szBuff, 38, 0xffffffff ) ;		// 순위
					m_pRankingStrGrid->AddItem( 1, CRYLGameData::Instance()->GetClassName( it->m_cClass ), 73, 0xffffffff ) ;		// 클레스
					m_pRankingStrGrid->AddItem( 2, it->m_szCharName, 93, 0xffffffff ) ;			// 이름
					sprintf( szBuff, "%d", it->m_dwPoint ) ;
					m_pRankingStrGrid->AddItem( 3, szBuff, 78, 0xffffffff ) ;			// 포인트
					sprintf( szBuff, "%d", it->m_cLevel ) ;
					m_pRankingStrGrid->AddItem( 4, szBuff, 17, 0xffffffff ) ;			// 레벨
					++ iRank ;
				}
			}
			break ;
		case 20:		// 어태커
			{
				//std::vector<RankingNode>			m_vecAttacker ;		// 어태커 리스트
				for ( std::vector<RankingNode>::iterator it = m_vecAttacker.begin() ; it != m_vecAttacker.end() ; ++ it )
				{
					CHAR szBuff[ 32 ] ;
					sprintf( szBuff, "%d", iRank ) ;
					m_pRankingStrGrid->AddItem( 0, szBuff, 38, 0xffffffff ) ;		// 순위
					m_pRankingStrGrid->AddItem( 1, CRYLGameData::Instance()->GetClassName( it->m_cClass ), 73, 0xffffffff ) ;		// 클레스
					m_pRankingStrGrid->AddItem( 2, it->m_szCharName, 93, 0xffffffff ) ;			// 이름
					sprintf( szBuff, "%d", it->m_dwPoint ) ;
					m_pRankingStrGrid->AddItem( 3, szBuff, 78, 0xffffffff ) ;			// 포인트
					sprintf( szBuff, "%d", it->m_cLevel ) ;
					m_pRankingStrGrid->AddItem( 4, szBuff, 17, 0xffffffff ) ;			// 레벨
					++ iRank ;
				}
			}
			break ;
		case 21:		// 거너
			{
				//std::vector<RankingNode>			m_vecGunner ;		// 거너	리스트
				for ( std::vector<RankingNode>::iterator it = m_vecGunner.begin() ; it != m_vecGunner.end() ; ++ it )
				{
					CHAR szBuff[ 32 ] ;
					sprintf( szBuff, "%d", iRank ) ;
					m_pRankingStrGrid->AddItem( 0, szBuff, 38, 0xffffffff ) ;		// 순위
					m_pRankingStrGrid->AddItem( 1, CRYLGameData::Instance()->GetClassName( it->m_cClass ), 73, 0xffffffff ) ;		// 클레스
					m_pRankingStrGrid->AddItem( 2, it->m_szCharName, 93, 0xffffffff ) ;			// 이름
					sprintf( szBuff, "%d", it->m_dwPoint ) ;
					m_pRankingStrGrid->AddItem( 3, szBuff, 78, 0xffffffff ) ;			// 포인트
					sprintf( szBuff, "%d", it->m_cLevel ) ;
					m_pRankingStrGrid->AddItem( 4, szBuff, 17, 0xffffffff ) ;			// 레벨
					++ iRank ;
				}
			}
			break ;
		case 22:		// 룬오프
			{
				//std::vector<RankingNode>			m_vecRuneOff ;		// 룬오프 리스트
				for ( std::vector<RankingNode>::iterator it = m_vecRuneOff.begin() ; it != m_vecRuneOff.end() ; ++ it )
				{
					CHAR szBuff[ 32 ] ;
					sprintf( szBuff, "%d", iRank ) ;
					m_pRankingStrGrid->AddItem( 0, szBuff, 38, 0xffffffff ) ;		// 순위
					m_pRankingStrGrid->AddItem( 1, CRYLGameData::Instance()->GetClassName( it->m_cClass ), 73, 0xffffffff ) ;		// 클레스
					m_pRankingStrGrid->AddItem( 2, it->m_szCharName, 93, 0xffffffff ) ;			// 이름
					sprintf( szBuff, "%d", it->m_dwPoint ) ;
					m_pRankingStrGrid->AddItem( 3, szBuff, 78, 0xffffffff ) ;			// 포인트
					sprintf( szBuff, "%d", it->m_cLevel ) ;
					m_pRankingStrGrid->AddItem( 4, szBuff, 17, 0xffffffff ) ;			// 레벨
					++ iRank ;
				}
			}
			break ;
		case 23:		// 라이프오프
			{
				//std::vector<RankingNode>			m_vecLifeOff ;		// 라이프오프 리스트
				for ( std::vector<RankingNode>::iterator it = m_vecLifeOff.begin() ; it != m_vecLifeOff.end() ; ++ it )
				{
					CHAR szBuff[ 32 ] ;
					sprintf( szBuff, "%d", iRank ) ;
					m_pRankingStrGrid->AddItem( 0, szBuff, 38, 0xffffffff ) ;		// 순위
					m_pRankingStrGrid->AddItem( 1, CRYLGameData::Instance()->GetClassName( it->m_cClass ), 73, 0xffffffff ) ;		// 클레스
					m_pRankingStrGrid->AddItem( 2, it->m_szCharName, 93, 0xffffffff ) ;			// 이름
					sprintf( szBuff, "%d", it->m_dwPoint ) ;
					m_pRankingStrGrid->AddItem( 3, szBuff, 78, 0xffffffff ) ;			// 포인트
					sprintf( szBuff, "%d", it->m_cLevel ) ;
					m_pRankingStrGrid->AddItem( 4, szBuff, 17, 0xffffffff ) ;			// 레벨
					++ iRank ;
				}
			}
			break ;
		case 24:		// 쉐도우오프
			{
				//std::vector<RankingNode>			m_vecShadowOff ;	// 쉐도우오프 리스트
				for ( std::vector<RankingNode>::iterator it = m_vecShadowOff.begin() ; it != m_vecShadowOff.end() ; ++ it )
				{
					CHAR szBuff[ 32 ] ;
					sprintf( szBuff, "%d", iRank ) ;
					m_pRankingStrGrid->AddItem( 0, szBuff, 38, 0xffffffff ) ;		// 순위
					m_pRankingStrGrid->AddItem( 1, CRYLGameData::Instance()->GetClassName( it->m_cClass ), 73, 0xffffffff ) ;		// 클레스
					m_pRankingStrGrid->AddItem( 2, it->m_szCharName, 93, 0xffffffff ) ;			// 이름
					sprintf( szBuff, "%d", it->m_dwPoint ) ;
					m_pRankingStrGrid->AddItem( 3, szBuff, 78, 0xffffffff ) ;			// 포인트
					sprintf( szBuff, "%d", it->m_cLevel ) ;
					m_pRankingStrGrid->AddItem( 4, szBuff, 17, 0xffffffff ) ;			// 레벨
					++ iRank ;
				}

			}
			break ;
	}
}

VOID	CRYLRankingDlg::InsertRanking( INT iClass, INT iPage, INT iNum, RankingNode* lpRankingNode ) 
{
	RankingNode AddNode;
	m_cCurrentPage = iPage ;

	switch( iClass )
	{
		case 0:		// 전체
			{
				//std::vector<RankingNode>			m_vecAll ;			// 전체 리스트
				m_vecAll.clear();
				for( INT i = 0 ; i < iNum ; i ++ )
				{
					strncpy( AddNode.m_szCharName, lpRankingNode[ i ].m_szCharName, RankingNode::MAX_NAME ) ;
					AddNode.m_cClass = lpRankingNode[ i ].m_cClass ;
					AddNode.m_cLevel = lpRankingNode[ i ].m_cLevel ;
					AddNode.m_dwPoint = lpRankingNode[ i ].m_dwPoint ;
					m_vecAll.push_back( AddNode ) ;
				}
			}
			break ;
		case 1:		// 파이터
			break ;
		case 2:		// 로그
			break ;
		case 3:		// 메이지
			break ;
		case 4:		// 성직자
			break ;
		case 5:		// 디펜더
			{
				//std::vector<RankingNode>			m_vecDefender ;		// 디펜더 리스트
				m_vecDefender.clear();
				for( INT i = 0 ; i < iNum ; i ++ )
				{
					strncpy( AddNode.m_szCharName, lpRankingNode[ i ].m_szCharName, RankingNode::MAX_NAME ) ;
					AddNode.m_cClass = lpRankingNode[ i ].m_cClass ;
					AddNode.m_cLevel = lpRankingNode[ i ].m_cLevel ;
					AddNode.m_dwPoint = lpRankingNode[ i ].m_dwPoint ;
					m_vecDefender.push_back( AddNode ) ;
				}
			}
			break ;
		case 6:		// 워리어
			{
				//std::vector<RankingNode>			m_vecWarrior ;		// 워리어 리스트
				m_vecWarrior.clear();
				for( INT i = 0 ; i < iNum ; i ++ )
				{
					strncpy( AddNode.m_szCharName, lpRankingNode[ i ].m_szCharName, RankingNode::MAX_NAME ) ;
					AddNode.m_cClass = lpRankingNode[ i ].m_cClass ;
					AddNode.m_cLevel = lpRankingNode[ i ].m_cLevel ;
					AddNode.m_dwPoint = lpRankingNode[ i ].m_dwPoint ;
					m_vecWarrior.push_back( AddNode ) ;
				}
			}
			break ;
		case 7:		// 어쌔신
			{
				//std::vector<RankingNode>			m_vecAssassin ;		// 어쎄신 리스트
				m_vecAssassin.clear();
				for( INT i = 0 ; i < iNum ; i ++ )
				{
					strncpy( AddNode.m_szCharName, lpRankingNode[ i ].m_szCharName, RankingNode::MAX_NAME ) ;
					AddNode.m_cClass = lpRankingNode[ i ].m_cClass ;
					AddNode.m_cLevel = lpRankingNode[ i ].m_cLevel ;
					AddNode.m_dwPoint = lpRankingNode[ i ].m_dwPoint ;
					m_vecAssassin.push_back( AddNode ) ;
				}
			}
			break ;
		case 8:		// 아처
			{
				//std::vector<RankingNode>			m_vecArcher ;		// 아처 리스트
				m_vecArcher.clear();
				for( INT i = 0 ; i < iNum ; i ++ )
				{
					strncpy( AddNode.m_szCharName, lpRankingNode[ i ].m_szCharName, RankingNode::MAX_NAME ) ;
					AddNode.m_cClass = lpRankingNode[ i ].m_cClass ;
					AddNode.m_cLevel = lpRankingNode[ i ].m_cLevel ;
					AddNode.m_dwPoint = lpRankingNode[ i ].m_dwPoint ;
					m_vecArcher.push_back( AddNode ) ;
				}
			}
			
			break ;
		case 9:		// 소서러
			{
				//std::vector<RankingNode>			m_vecSorcerer ;		// 소서리 리스트
				m_vecSorcerer.clear();
				for( INT i = 0 ; i < iNum ; i ++ )
				{
					strncpy( AddNode.m_szCharName, lpRankingNode[ i ].m_szCharName, RankingNode::MAX_NAME ) ;
					AddNode.m_cClass = lpRankingNode[ i ].m_cClass ;
					AddNode.m_cLevel = lpRankingNode[ i ].m_cLevel ;
					AddNode.m_dwPoint = lpRankingNode[ i ].m_dwPoint ;
					m_vecSorcerer.push_back( AddNode ) ;
				}
			}
			break ;
		case 10:		// 인챈터
			{
				//std::vector<RankingNode>			m_vecEnchanter ;	// 인첸터 리스트
				m_vecEnchanter.clear();
				for( INT i = 0 ; i < iNum ; i ++ )
				{
					strncpy( AddNode.m_szCharName, lpRankingNode[ i ].m_szCharName, RankingNode::MAX_NAME ) ;
					AddNode.m_cClass = lpRankingNode[ i ].m_cClass ;
					AddNode.m_cLevel = lpRankingNode[ i ].m_cLevel ;
					AddNode.m_dwPoint = lpRankingNode[ i ].m_dwPoint ;
					m_vecEnchanter.push_back( AddNode ) ;
				}
			}
			break ;
		case 11:		// 프리스트
			{
				//std::vector<RankingNode>			m_vecPriest ;		// 프리스트 리스트
				m_vecPriest.clear();
				for( INT i = 0 ; i < iNum ; i ++ )
				{
					strncpy( AddNode.m_szCharName, lpRankingNode[ i ].m_szCharName, RankingNode::MAX_NAME ) ;
					AddNode.m_cClass = lpRankingNode[ i ].m_cClass ;
					AddNode.m_cLevel = lpRankingNode[ i ].m_cLevel ;
					AddNode.m_dwPoint = lpRankingNode[ i ].m_dwPoint ;
					m_vecPriest.push_back( AddNode ) ;
				}
			}
			break ;
		case 12:		// 클레릭
			{
				//std::vector<RankingNode>			m_vecCleric ;		// 클래릭 리스트
				m_vecCleric.clear();
				for( INT i = 0 ; i < iNum ; i ++ )
				{
					strncpy( AddNode.m_szCharName, lpRankingNode[ i ].m_szCharName, RankingNode::MAX_NAME ) ;
					AddNode.m_cClass = lpRankingNode[ i ].m_cClass ;
					AddNode.m_cLevel = lpRankingNode[ i ].m_cLevel ;
					AddNode.m_dwPoint = lpRankingNode[ i ].m_dwPoint ;
					m_vecCleric.push_back( AddNode ) ;
				}
			}
			break ;
		case 17:		// 컴배턴트
			break ;
		case 18:		// 오피세이터
			break ;
		case 19:		// 템플러
			{
				//std::vector<RankingNode>			m_vecTemplar ;		// 템플러 리스트
				m_vecTemplar.clear();
				for( INT i = 0 ; i < iNum ; i ++ )
				{
					strncpy( AddNode.m_szCharName, lpRankingNode[ i ].m_szCharName, RankingNode::MAX_NAME ) ;
					AddNode.m_cClass = lpRankingNode[ i ].m_cClass ;
					AddNode.m_cLevel = lpRankingNode[ i ].m_cLevel ;
					AddNode.m_dwPoint = lpRankingNode[ i ].m_dwPoint ;
					m_vecTemplar.push_back( AddNode ) ;
				}
			}
			break ;
		case 20:		// 어태커
			{
				//std::vector<RankingNode>			m_vecAttacker ;		// 어태커 리스트
				m_vecAttacker.clear();
				for( INT i = 0 ; i < iNum ; i ++ )
				{
					strncpy( AddNode.m_szCharName, lpRankingNode[ i ].m_szCharName, RankingNode::MAX_NAME ) ;
					AddNode.m_cClass = lpRankingNode[ i ].m_cClass ;
					AddNode.m_cLevel = lpRankingNode[ i ].m_cLevel ;
					AddNode.m_dwPoint = lpRankingNode[ i ].m_dwPoint ;
					m_vecAttacker.push_back( AddNode ) ;
				}
			}
			break ;
		case 21:		// 거너
			{
				//std::vector<RankingNode>			m_vecGunner ;		// 거너	리스트
				m_vecGunner.clear();
				for( INT i = 0 ; i < iNum ; i ++ )
				{
					strncpy( AddNode.m_szCharName, lpRankingNode[ i ].m_szCharName, RankingNode::MAX_NAME ) ;
					AddNode.m_cClass = lpRankingNode[ i ].m_cClass ;
					AddNode.m_cLevel = lpRankingNode[ i ].m_cLevel ;
					AddNode.m_dwPoint = lpRankingNode[ i ].m_dwPoint ;
					m_vecGunner.push_back( AddNode ) ;
				}
			}
			break ;
		case 22:		// 룬오프
			{
				//std::vector<RankingNode>			m_vecRuneOff ;		// 룬오프 리스트
				m_vecRuneOff.clear();
				for( INT i = 0 ; i < iNum ; i ++ )
				{
					strncpy( AddNode.m_szCharName, lpRankingNode[ i ].m_szCharName, RankingNode::MAX_NAME ) ;
					AddNode.m_cClass = lpRankingNode[ i ].m_cClass ;
					AddNode.m_cLevel = lpRankingNode[ i ].m_cLevel ;
					AddNode.m_dwPoint = lpRankingNode[ i ].m_dwPoint ;
					m_vecRuneOff.push_back( AddNode ) ;
				}
			}
			break ;
		case 23:		// 라이프오프
			{
				//std::vector<RankingNode>			m_vecLifeOff ;		// 라이프오프 리스트
				m_vecLifeOff.clear();
				for( INT i = 0 ; i < iNum ; i ++ )
				{
					strncpy( AddNode.m_szCharName, lpRankingNode[ i ].m_szCharName, RankingNode::MAX_NAME ) ;
					AddNode.m_cClass = lpRankingNode[ i ].m_cClass ;
					AddNode.m_cLevel = lpRankingNode[ i ].m_cLevel ;
					AddNode.m_dwPoint = lpRankingNode[ i ].m_dwPoint ;
					m_vecLifeOff.push_back( AddNode ) ;
				}
			}
			break ;
		case 24:		// 쉐도우오프
			{
				//std::vector<RankingNode>			m_vecShadowOff ;	// 쉐도우오프 리스트
				m_vecShadowOff.clear();
				for( INT i = 0 ; i < iNum ; i ++ )
				{
					strncpy( AddNode.m_szCharName, lpRankingNode[ i ].m_szCharName, RankingNode::MAX_NAME ) ;
					AddNode.m_cClass = lpRankingNode[ i ].m_cClass ;
					AddNode.m_cLevel = lpRankingNode[ i ].m_cLevel ;
					AddNode.m_dwPoint = lpRankingNode[ i ].m_dwPoint ;
					m_vecShadowOff.push_back( AddNode ) ;
				}
			}
			break ;
	}
}