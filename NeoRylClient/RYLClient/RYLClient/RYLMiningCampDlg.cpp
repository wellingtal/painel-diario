#include <winsock2.h>

#include <Network/ClientSocket/Send/SendCastle.h>

#include "WinInput.h"
#include "GMImageDib.h"

#include "RYLClientMain.h"
#include "RYLImageManager.h"
#include "RYLImageCabinet.h"
#include "RYLSpriteList.h"
#include "RYLImage.h"
#include "RYLButton.h"
#include "RYLLabel.h"
#include "RYLEdit.h"
#include "RYLMessageBox.h"
#include "RYLStringTable.h"

#include "RYLGameData.h"
#include "RYLNetworkData.h"


#include "RYLMiningCampDlg.h"
#include <Item/ItemMgr.h>
#include <Item/ItemFactory.h>
#include "SoundMgr.h"


#include "RYLCampManager.h"
#include "RYLCamp.h"
#include "GMMemory.h"


CRYLMiningCampDlg*		g_pMiningCampDlg = NULL ;

// 328 * 370
CRYLMiningCampDlg::CRYLMiningCampDlg() 
{

	RECT rt ;
	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;

	CRYLSpriteList::Instance()->CreateGMImage( 328, 15, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 0, 256, 15 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_GUILD_256x256 ) ) ;
	SetRect( &rt, 0, 53, 72, 68 ) ;
	pImageDIB->DrawRect( &rt, 256, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_GUILD_256x256 ) ) ;

	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	AttachCaptionTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;

	CRYLSpriteList::Instance()->CreateGMImage( 328, 355, &pImageDIB ) ;

	int i;
	CGMImageDib*	pTempImage;
	pTempImage = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_GUILD_256x256 );
	for ( i = 0 ; i < 69 ; i ++ )
	{
		SetRect( &rt, 0, 38, 256, 43 ) ;
		pImageDIB->DrawRect( &rt, 0, i * 5, pTempImage ) ;
		SetRect( &rt, 72, 53, 144, 58 ) ;
		pImageDIB->DrawRect( &rt, 256, i * 5, pTempImage ) ;
	}
	SetRect( &rt, 0, 43, 256, 53 ) ;
	pImageDIB->DrawRect( &rt, 0, i * 5, pTempImage ) ;
	SetRect( &rt, 144, 53, 216, 63 ) ;
	pImageDIB->DrawRect( &rt, 256, i * 5, pTempImage  ) ;




	pImageDIB->MakeFrameRect( 12, 34 -15, 175, 259, 0xffB99E6B, 0x80000000, 1 ) ;   //왼쪽 박스
	pImageDIB->MakeFrameRect( 197, 34 -15, 47, 259, 0xffB99E6B, 0x80000000, 1 ) ;   //오른쪽 박스
	pImageDIB->MakeFrameRect( 231, 302 -15, 68, 18, 0xff959595, 0x80000000, 1 ) ;   //페이즈 박스
	pImageDIB->MakeFrameRect( 254, 34 -15, 60, 259, 0xffB99E6B, 0x80000000, 1 ) ;   //버튼 박스
//	pImageDIB->MakeFrameRect( 12, 333 -15, 129, 19, 0xffB99E6B, 0x80000000, 1 ) ;   //채굴 완료 박스
	pImageDIB->MakeFrameRect( 12, 302 -15, 175, 19, 0xffB99E6B, 0x80000000, 1 ) ;   //채굴 완료 박스


	for( i = 0 ; i < 8 ; i++ )			//8개 씩 있다.
	{
		pImageDIB->MakeFrameRect( 18, 40 + 31*i -15, 28, 28, 0xffB99E6B, 0x80000000, 1 ) ;			//1x1 인벤
		pImageDIB->MakeFrameRect( 52, 44 + 31*i -15, 129, 19, 0xff959595, 0x80000000, 1 ) ;			//광물 이름 박스
		pImageDIB->MakeFrameRect( 204, 44 + 31*i -15, 33, 19, 0xff959595, 0x80000000, 1 ) ;			//광물 수량 박스
	}

	
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	AttachWindowTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;
	
	SetWidth( 256 ) ;
	SetHeight( 315 ) ;
	SetLeft( ( CGMUIScreen::GetInstance()->GetWidth() - GetWidth() ) / 2 ) ;
	SetTop( ( CGMUIScreen::GetInstance()->GetHeight() - GetHeight() ) / 2 ) ;

	SetFormType( RYLDLG_FRAME_TYPE_CLOSEBUTTON ) ;

	m_pPrevButton				= NULL ;							// 페이즈 이동 버튼
	m_pNextButton				= NULL ;							// 페이즈 이동 버튼
	m_pPhaseLabel				= NULL ;
	m_pDoneLabel				= NULL ;							// 완료 라벨
	m_pTitleLabel				= NULL ;							// 광물의 매입량 확인 ( 타이틀 )
	m_pOnButton					= NULL ;							// 채굴기 켜기 버튼
	m_pOffButton				= NULL ;							// 채굴기 끄기 버튼
	

	m_nCurPhase = 0 ;
	m_nMaxPhase = 1 ;
	m_dwRemainTime = 0 ;
	for( i = 0 ; i < 8 ; i++ )
	{
		m_pMineNameLabel[i] = NULL;					// 광물 이름
		m_pMineCountLabel[i] = NULL;					// 광물 수량
		m_pGetMineButton[i] = NULL;					// 광물 획득 버튼
	}
	
	m_dwSplitResult = 0 ;
	m_dwSplitValue	= 0 ;
	m_dwSplitIndex  = 0 ;
	m_dwCampID		= 0 ;
	g_pMiningCampDlg = this ;

	InitCtrl() ;
}


CRYLMiningCampDlg::~CRYLMiningCampDlg() 
{
	FinalCtrl() ;
}

VOID	CRYLMiningCampDlg::InitCtrl() 
{
	int	i;
	CGMImageDib* pSprite			;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;

	pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ;
	RECT rt ;

	m_pPrevButton = new CRYLButton( this ) ;
	m_pPrevButton->SetLeft( 216 ) ;
	m_pPrevButton->SetTop( 305 ) ;
	m_pPrevButton->OnClickEvent = DlgClick ;
	m_pPrevButton->SetInstance( RYLMININGCAMPDLG::PREV_BUTTON ) ;
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
	m_pNextButton->SetLeft( 301 ) ;
	m_pNextButton->SetTop( 305 ) ;
	m_pNextButton->OnClickEvent = DlgClick ;
	m_pNextButton->SetInstance( RYLMININGCAMPDLG::NEXT_BUTTON ) ;
	SetRect( &rt, 232, 215, 245, 227 ) ;
	m_pNextButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 232, 243, 245, 255 ) ;
	m_pNextButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 232, 229, 245, 241 ) ;
	m_pNextButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 232, 215, 245, 227 ) ;
	m_pNextButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pNextButton ) ;



	pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;

	// 채굴기 켜기 버튼
	m_pOnButton = new CRYLButton( this ) ;
	m_pOnButton->SetLeft( 150 ) ;
	m_pOnButton->SetTop( 330 ) ;
	m_pOnButton->SetCaption(  CRYLStringTable::m_strString[3174] ) ;
	m_pOnButton->SetFontColor( 0xffffffff) ;
	m_pOnButton->SetColor( 0xffffffff) ;
	m_pOnButton->OnClickEvent = DlgClick ;
	m_pOnButton->SetInstance( RYLMININGCAMPDLG::ON_BUTTON ) ;
	m_pOnButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pOnButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pOnButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pOnButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pOnButton ) ;

	// 채굴기 끄기 버튼
	m_pOffButton = new CRYLButton( this ) ;
	m_pOffButton->SetLeft( 236 ) ;
	m_pOffButton->SetTop( 330 ) ;
	m_pOffButton->SetCaption( CRYLStringTable::m_strString[3175] ) ;
	m_pOffButton->SetFontColor( 0xffffffff) ;
	m_pOffButton->SetColor( 0xffffffff) ;
	m_pOffButton->OnClickEvent = DlgClick ;
	m_pOffButton->SetInstance( RYLMININGCAMPDLG::OFF_BUTTON) ;
	m_pOffButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pOffButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pOffButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pOffButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 5 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pOffButton ) ;

	for( i = 0 ; i < 8 ; i++ )
	{
		// 광물 획득 버튼
		m_pGetMineButton[i]  = new CRYLButton( this ) ;
		m_pGetMineButton[i]->SetLeft( 261 ) ;
		m_pGetMineButton[i]->SetTop( 41 +31*i ) ;
		m_pGetMineButton[i]->SetCaption( CRYLStringTable::m_strString[3176] ) ;
		m_pGetMineButton[i]->SetFontColor( 0xffffffff) ;
		m_pGetMineButton[i]->SetColor( 0xffffffff) ;
		m_pGetMineButton[i]->OnClickEvent = DlgClick ;
		m_pGetMineButton[i]->SetInstance( RYLMININGCAMPDLG::GET_BUTTON_0 + i ) ;
		m_pGetMineButton[i]->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 2 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
		m_pGetMineButton[i]->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 2 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
		m_pGetMineButton[i]->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 2 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
		m_pGetMineButton[i]->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 2 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
		AttachGMUIControl( m_pGetMineButton[i]  ) ;
	}


	m_pPhaseLabel = new CRYLLabel( this ) ;		
	m_pPhaseLabel->SetLeft( 240 ) ;
	m_pPhaseLabel->SetTop( 306 ) ;
	m_pPhaseLabel->SetFontColor (0xffffffff);
	m_pPhaseLabel->SetAutoSize( TRUE ) ;
	m_pPhaseLabel->SetCaption( "" ) ;
	AttachGMUIControl( m_pPhaseLabel ) ;


	// 완료 라벨
	m_pDoneLabel = new CRYLLabel( this ) ;
	m_pDoneLabel->SetFontColor(0xffffffff);
	SetRect( &rt, 13, 306, 185, 316 ) ;
	m_pDoneLabel->SetAlignRect( rt ) ;
	m_pDoneLabel->SetAlign( CENTER ) ;
	m_pDoneLabel->SetCaption( "" ) ;
	m_pDoneLabel->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pDoneLabel ) ;

	
	m_pTitleLabel = new CRYLLabel( this ) ;		
	m_pTitleLabel->SetLeft( 12 ) ;
	m_pTitleLabel->SetTop( 18 ) ;
	m_pTitleLabel->SetFontColor (0xffffffff);
	m_pTitleLabel->SetAutoSize( TRUE ) ;
	m_pTitleLabel->SetCaption( CRYLStringTable::m_strString[3179] ) ;
	AttachGMUIControl( m_pTitleLabel ) ;

	for( i = 0 ; i < 8 ; i++ )
	{
		// 광물 이름
		m_pMineNameLabel[i] = new CRYLLabel( this ) ;		
		m_pMineNameLabel[i]->SetLeft( 57 ) ;
		m_pMineNameLabel[i]->SetTop( 48+31*i ) ;
		m_pMineNameLabel[i]->SetFontColor (0xffffffff);
		m_pMineNameLabel[i]->SetAutoSize( TRUE ) ;
		m_pMineNameLabel[i]->SetCaption( "" ) ;
		AttachGMUIControl( m_pMineNameLabel[i] ) ;
		// 광물 수량
		m_pMineCountLabel[i] = new CRYLLabel( this ) ;		
		m_pMineCountLabel[i]->SetLeft( 209 ) ;
		m_pMineCountLabel[i]->SetTop( 48+31*i ) ;
		m_pMineCountLabel[i]->SetFontColor (0xffffffff);
		m_pMineCountLabel[i]->SetAutoSize( TRUE ) ;
		m_pMineCountLabel[i]->SetCaption( "" ) ;
		AttachGMUIControl( m_pMineCountLabel[i] ) ;
	}
	SetPhaseLabel();
	
}

VOID	CRYLMiningCampDlg::FinalCtrl() 
{
	ClearItem();
	GM_DELETE( m_pPrevButton );							// 페이즈 이동 버튼
	GM_DELETE( m_pNextButton );							// 페이즈 이동 버튼
	GM_DELETE( m_pPhaseLabel );
	GM_DELETE( m_pDoneLabel	);							// 완료 라벨
	GM_DELETE( m_pTitleLabel );							// 광물의 매입량 확인 ( 타이틀 )
	GM_DELETE( m_pOnButton );							// 채굴기 켜기 버튼
	GM_DELETE( m_pOffButton );							// 채굴기 끄기 버튼
	

	for( int i = 0 ; i < 8 ; i++ )
	{
		GM_DELETE( m_pMineNameLabel[i] );					// 광물 이름
		GM_DELETE( m_pMineCountLabel[i] );					// 광물 수량
		GM_DELETE( m_pGetMineButton[i] );					// 광물 획득 버튼
	}
}

VOID	CRYLMiningCampDlg::Show( CGMUIObject* Sender ) 
{
	CRYLGameData* pGame = CRYLGameData::Instance() ;
	pGame->SetAutoRun( FALSE ) ;
	InitMineral();
}

VOID	CRYLMiningCampDlg::Hide( CGMUIObject* Sender ) 
{
}

VOID	CRYLMiningCampDlg::GMUIPaint( INT xx, INT yy ) 
{
	LPDIRECT3DDEVICE8   lpD3DDevice = BaseGraphicsLayer::GetDevice() ;
	CRYLDialog::GMUIPaint( xx, yy ) ;

	std::vector< CItemInstance* >::iterator iter;
	int	i = 0;
	int nMinCnt, nMaxCnt;
	nMinCnt = m_nCurPhase * 8;
	nMaxCnt = nMinCnt + 8;
	CItemInstance* pItem;
	for( iter = m_vecMineIcon.begin(); iter != m_vecMineIcon.end(); iter++ )
	{
		if( i == nMaxCnt )
			break;
		else if( i < nMinCnt )
		{
			i++;
			continue;
		}

		pItem = *iter;
		if( pItem )
		{
			int nX = 0;
			int nY = 0;
			nX = ( 28 - pItem->m_lpSprite->GetSizeX() ) /2 ;
			nY = ( 28 - pItem->m_lpSprite->GetSizeY() ) /2 ;
			pItem->m_wPutX = GetLeft() + 18 + nX;
			pItem->m_wPutY = GetTop() + 40 + 31*(i-nMinCnt) + nY;
			//pItem->m_wPutX = GetLeft() + 18 +2;
			//pItem->m_wPutY = GetTop() + 40 + 31*(i-nMinCnt) +2;
			pItem->Render( lpD3DDevice );
			i++;
		}
	}
}

HRESULT		CRYLMiningCampDlg::Update( BOOL &bClick, BOOL &bEdge ) 
{
	if (FALSE == GetVisible())
	{
		return S_OK;
	}

	CRYLGameData* pGame			= CRYLGameData::Instance() ;
	CRYLNetworkData* pNetwork	= CRYLNetworkData::Instance() ;
	bEdge = TRUE;

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
	// 나누기
	if( m_dwSplitResult & MB_EXIT )
	{
		m_dwSplitIndex = 0;
		m_dwSplitValue = 0;
		m_dwSplitResult = MB_NORMAL;
	}
	else if ( m_dwSplitResult & MB_NO )
	{
		m_dwSplitIndex = 0;
		m_dwSplitValue = 0;
		m_dwSplitResult = MB_NORMAL;
	}
	else if ( m_dwSplitResult & MB_YES )
	{
		m_dwSplitResult = MB_NORMAL;
		if ( m_dwSplitValue )
		{
			if( m_dwCampID )
			{
				CRYLNetworkData* pNetwork = CRYLNetworkData::Instance() ;
				SendPacket::CharCampCmd( g_GameSession, 
					pNetwork->m_dwMyChrID, 
					m_dwCampID, 
					m_dwSplitIndex, 
					m_dwSplitValue,
					PktCampCmd::MINING_CAMP_GAIN_MINERAL ) ;
				
			}
			m_dwSplitIndex = 0;
			m_dwSplitValue = 0;
		}
		m_dwSplitIndex = 0;
		m_dwSplitValue = 0;
	}

	return S_OK;
}

VOID	CRYLMiningCampDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y ) 
{
	if ( !g_pMiningCampDlg )
		return ;
	CRYLGameData* pGame = CRYLGameData::Instance() ;
	switch( Sender->GetInstance() )
	{
	case RYLMININGCAMPDLG::NEXT_BUTTON :
		{
			g_pMiningCampDlg->NextPhase() ;
		}
		break;
	case RYLMININGCAMPDLG::PREV_BUTTON :
		{
			g_pMiningCampDlg->PrevPhase() ;
		}
		break;
	case RYLMININGCAMPDLG::ON_BUTTON :
		{
			g_pMiningCampDlg->MiningOn();
		}
		break;
	case RYLMININGCAMPDLG::OFF_BUTTON :
		{
			g_pMiningCampDlg->MiningOff();
		}
		break;
	case RYLMININGCAMPDLG::GET_BUTTON_0 :
		{
			g_pMiningCampDlg->GetMine( 0 );
		}
		break;
	case RYLMININGCAMPDLG::GET_BUTTON_1 :
		{
			g_pMiningCampDlg->GetMine( 1 );
		}
		break;
	case RYLMININGCAMPDLG::GET_BUTTON_2 :
		{
			g_pMiningCampDlg->GetMine( 2 );
		}
		break;
	case RYLMININGCAMPDLG::GET_BUTTON_3 :
		{
			g_pMiningCampDlg->GetMine( 3 );
		}
		break;
	case RYLMININGCAMPDLG::GET_BUTTON_4 :
		{
			g_pMiningCampDlg->GetMine( 4 );
		}
		break;
	case RYLMININGCAMPDLG::GET_BUTTON_5 :
		{
			g_pMiningCampDlg->GetMine( 5 );
		}
		break;
	case RYLMININGCAMPDLG::GET_BUTTON_6 :
		{
			g_pMiningCampDlg->GetMine( 6 );
		}
		break;
	case RYLMININGCAMPDLG::GET_BUTTON_7 :
		{
			g_pMiningCampDlg->GetMine( 7 );
		}
		break;
	}
}

VOID	CRYLMiningCampDlg::NextPhase()
{
	m_nCurPhase = max( 0, min( m_nCurPhase+1, m_nMaxPhase-1 ) );
	//SetItemIcon();
	SetPhaseLabel();
}

VOID	CRYLMiningCampDlg::PrevPhase()
{
	m_nCurPhase = max( 0, m_nCurPhase -1 );
	SetPhaseLabel();
}

VOID	CRYLMiningCampDlg::AddItem( unsigned short id, unsigned short amount )
{
	CItemInstance* lpItem = new CItemInstance;
	Item::ItemPos pos( 0, 0 );
	lpItem->SetItemInfofromID( id, pos );
	
	m_vecMineIcon.push_back( lpItem );
	m_vecMineCnt.push_back( amount );
}


VOID
CRYLMiningCampDlg::ClearItem()
{
	m_nMaxPhase = 1;
	m_nCurPhase = 0;
	SetPhaseLabel();
	std::vector< CItemInstance* >::iterator iter;
	for( iter = m_vecMineIcon.begin(); iter != m_vecMineIcon.end(); iter++ )
	{
		if( *iter )
		{
			GM_DELETE(*iter);
		}
	}
	m_vecMineIcon.clear();

	m_vecMineCnt.clear();

	for( int i = 0 ; i < 8 ; i++ )
	{
		if( m_pMineCountLabel[i] )
			m_pMineCountLabel[i]->SetCaption( "" ) ;
	}
}


VOID	CRYLMiningCampDlg::SetPhaseLabel()
{
	
	char strString[MAX_PATH];
	int nSize, nMin, nMax;
	m_nCurPhase = max( 0, min( m_nCurPhase, m_nMaxPhase-1 ) );
	int nViewPhase = min(m_nCurPhase + 1, m_nMaxPhase );
	
	for( int i = 0 ; i < 8 ; i++ )
	{
		m_pMineCountLabel[i]->SetCaption( "" ) ;
		m_pMineNameLabel[i]->SetCaption( "" ) ;
		m_pGetMineButton[i]->SetEnable( FALSE );
	}

	sprintf( strString, "%3d / %3d", nViewPhase, m_nMaxPhase ) ;
	m_pPhaseLabel->SetCaption( strString ) ;

	nSize = m_vecMineCnt.size();
	nMin = m_nCurPhase*8;
	nMax = min( nMin+8, nSize );
	if( nMin < nSize )
	{
		for( int i = nMin ; i < nMax ; i++ )
		{
			sprintf( strString, "%3d", m_vecMineCnt[i] ) ;
			m_pMineCountLabel[i-nMin]->SetCaption( strString );
			Item::CItem* pItem = m_vecMineIcon[i]->m_lpItemBase;
			strcpy( strString, pItem->GetItemInfo().m_SpriteData.m_szName );
			m_pMineNameLabel[i-nMin]->SetCaption( strString );
			m_pGetMineButton[i-nMin]->SetEnable( TRUE );
		}
	}


	DWORD	dwState = 0;
	CRYLCamp* pCamp = CRYLCampManager::Instance()->FindCamp( m_dwCampID ) ;
	if ( pCamp )
	{
		RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( pCamp->GetCID() ) ;
		if( pTempCreature )
		{
			dwState = pTempCreature->GetSubState(); 
		}
	}


	sprintf( strString, CRYLStringTable::m_strString[3421], m_dwRemainTime ) ;

	switch( dwState )
	{
	case Siege::MINING_ON :
		
		m_pDoneLabel->SetCaption(strString);
		m_pOnButton->SetEnable( FALSE );
		m_pOffButton->SetEnable( TRUE );
		break;
	case Siege::MINING_OFF :
		m_pDoneLabel->SetCaption(CRYLStringTable::m_strString[3422]);
		m_pOnButton->SetEnable( TRUE );
		m_pOffButton->SetEnable( FALSE );
		break;
	case Siege::MINING_READY :
		m_pDoneLabel->SetCaption(CRYLStringTable::m_strString[3423]);
		m_pOnButton->SetEnable( FALSE );
		m_pOffButton->SetEnable( TRUE );
		break;

	}
	//7107
}


VOID				
//광물 획득 버튼 ( 1~8 )
CRYLMiningCampDlg::GetMine( int index )
{
	if(	!m_dwSplitIndex )
	{
		if( (index + m_nCurPhase*8) < (int)m_vecMineCnt.size() )
		{
			CRYLMessageBox *lpMessageBox ;
			lpMessageBox = new CRYLMessageBox ;

			m_dwSplitResult = 0 ;
			m_dwSplitValue	= 0 ;
			m_dwSplitIndex = m_vecMineIcon[ index + m_nCurPhase*8 ]->m_lpItemBase->GetPrototypeID();
			lpMessageBox->Create( CRYLStringTable::m_strString[ 170 ], MB_NUMBER | MB_YES | MB_NO | MB_EXIT ) ;
			lpMessageBox->SetResult( &m_dwSplitResult ) ;
			lpMessageBox->SetValue( &m_dwSplitValue ) ;
			char Temp[MAX_PATH]; 
			sprintf( Temp, "%s\\Sound\\Interface\\click.mnd", g_ClientMain.m_strClientPath ) ;
			CSound* pSound; 
			CSoundMgr::_GetInstance()->GetSound( pSound, Temp ); 
			pSound->Play(); 
		}
	}


}
	
VOID
//채굴기 켜기
CRYLMiningCampDlg::MiningOn()
{
	if( m_dwCampID )
	{

		CRYLNetworkData* pNetwork = CRYLNetworkData::Instance() ;
		SendPacket::CharCampCmd( g_GameSession, 
			pNetwork->m_dwMyChrID, 
			m_dwCampID, 
			Siege::MINING_ON, 
			0,
			PktCampCmd::MINING_CAMP_ON_OFF ) ;
	}
}



	
VOID
						//캐굴기 끄기
CRYLMiningCampDlg::MiningOff()
{
	DWORD	dwState = 0;
	CRYLCamp* pCamp = CRYLCampManager::Instance()->FindCamp( m_dwCampID ) ;
	if ( pCamp )
	{
		RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( pCamp->GetCID() ) ;
		if( pTempCreature )
		{
			dwState = pTempCreature->GetSubState(); 
		}
	}

	if( m_dwCampID )
	{
		if( dwState != Siege::MINING_OFF )
		{
			CRYLNetworkData* pNetwork = CRYLNetworkData::Instance() ;
			SendPacket::CharCampCmd( g_GameSession, 
				pNetwork->m_dwMyChrID, 
				m_dwCampID, 
				Siege::MINING_OFF,
				0,
				PktCampCmd::MINING_CAMP_ON_OFF ) ;

		}
	}

}


VOID
CRYLMiningCampDlg::InitMineral()
{
	if( m_dwCampID )
	{
		CRYLNetworkData* pNetwork = CRYLNetworkData::Instance() ;
		SendPacket::CharCampCmd( g_GameSession, 
			pNetwork->m_dwMyChrID, 
			m_dwCampID, 
			0, 
			0,
			PktCampCmd::MINING_CAMP_SHOW_MINERAL ) ;

	}
}


VOID	
CRYLMiningCampDlg::SetMineral( unsigned long dwCampID, unsigned char cState, unsigned long dwRemainTime, unsigned char cNum, CampMineralInfo* lpMineralList )
{
	ClearItem();
	m_nMaxPhase = (cNum + 7 ) / 8 ;
	m_nMaxPhase = max( 1, m_nMaxPhase );
	m_dwRemainTime = dwRemainTime;
	for( int i = 0 ; i < cNum ; i++ )
	{
		AddItem( lpMineralList[ i ].m_wMineralID, lpMineralList[ i ].m_wAmount );
	}
	SetPhaseLabel();
}


VOID	
CRYLMiningCampDlg::SetCampOnOff( unsigned long dwCampID, unsigned char cState )
{
	CRYLGameData* pGame = CRYLGameData::Instance() ;
	CRYLCamp* pCamp = CRYLCampManager::Instance()->FindCamp( m_dwCampID ) ;
	if ( pCamp )
	{
		RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( pCamp->GetCID() ) ;
		if( pTempCreature )
		{
			pTempCreature->SetSubState( cState );
		}
	}



	char strString[MAX_PATH];
	sprintf( strString, CRYLStringTable::m_strString[3421], m_dwRemainTime ) ;
	if( dwCampID == m_dwCampID )
	{
		switch( cState )
		{
		case Siege::MINING_ON :
			m_pDoneLabel->SetCaption(strString);
			m_pOnButton->SetEnable(FALSE);
			m_pOffButton->SetEnable(TRUE);
			break;

		case Siege::MINING_OFF :
			m_pDoneLabel->SetCaption(CRYLStringTable::m_strString[3422]);
			m_pOnButton->SetEnable(TRUE);
			m_pOffButton->SetEnable(FALSE);
			break;

		case Siege::MINING_READY :
			m_pDoneLabel->SetCaption(CRYLStringTable::m_strString[3423]);
			m_pOnButton->SetEnable(FALSE);
			m_pOffButton->SetEnable(TRUE);
			break;
		}
	}
}

BOOL
CRYLMiningCampDlg::RemoveItem( unsigned short id, unsigned short amountleft )
{
	int nIndex = -1;
	int nSize = 0;
	nSize = m_vecMineIcon.size();
	for( int i = 0 ; i < nSize ; i++ )
	{
		if( id == m_vecMineIcon[i]->m_lpItemBase->GetPrototypeID() )
		{
			nIndex = i;
		}
	}

	if( nIndex < 0 )
	{
		return FALSE;
	}

	if( amountleft == 0 )
	{
		m_vecMineIcon.erase(m_vecMineIcon.begin() + nIndex);
		m_vecMineCnt.erase(m_vecMineCnt.begin() + nIndex);
		nSize--;
		m_nMaxPhase = (nSize + 7 ) / 8 ;
		m_nCurPhase = min( m_nCurPhase , m_nMaxPhase );
	}
	else
	{
		m_vecMineCnt[ nIndex ] = amountleft;
	}
	SetPhaseLabel();

	return TRUE;
}

