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

#include "RYLMiningAmountDlg.h"
#include <Item/ItemMgr.h>
#include <Item/ItemFactory.h>
#include <Map/FieldMap/MineralVeinMgr.h>
#include "GMMemory.h"

CRYLMiningAmountDlg*		g_pMiningAmountDlg = NULL ;

CRYLMiningAmountDlg::CRYLMiningAmountDlg() 
{

	RECT rt ;
	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;

	CRYLSpriteList::Instance()->CreateGMImage( 256, 15, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 0, 256, 15 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BLACKSMITH_256x256 ) ) ;
	
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	AttachCaptionTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;

	CRYLSpriteList::Instance()->CreateGMImage( 256, 437, &pImageDIB ) ;

	int i;
	CGMImageDib*	pTempImage;
	pTempImage = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_GUILD_256x256 );
	for ( i = 0 ; i < 85 ; i ++ )
	{
		SetRect( &rt, 0, 38, 184, 43 ) ;
		pImageDIB->DrawRect( &rt, 0, i * 5, pTempImage ) ;
		SetRect( &rt, 72, 53, 144, 58 ) ;
		pImageDIB->DrawRect( &rt, 184, i * 5, pTempImage ) ;
	}
	// 2 pixel 추가
	SetRect( &rt, 0, 38, 184, 40 ) ;
	pImageDIB->DrawRect( &rt, 0, i * 5, pTempImage ) ;
	SetRect( &rt, 72, 53, 144, 55 ) ;
	pImageDIB->DrawRect( &rt, 184, i * 5, pTempImage ) ;
	// 마지막 10 pixel
	SetRect( &rt, 0, 43, 184, 53 ) ;
	pImageDIB->DrawRect( &rt, 0, i * 5 + 2, pTempImage ) ;
	SetRect( &rt, 144, 53, 216, 63 ) ;
	pImageDIB->DrawRect( &rt, 184, i * 5 + 2, pTempImage  ) ;

	pImageDIB->MakeFrameRect( 2, 20, 252, 1, 0xffB99E6B, 0x80000000,  1 ) ;			// 위쪽 라인 박스
	pImageDIB->MakeFrameRect( 12, 28, 232, 26, 0xffB99E6B, 0x80000000,  1 ) ;		// 광맥 이름 박스
	pImageDIB->MakeFrameRect( 12, 61, 175, 259, 0xffB99E6B, 0x80000000, 1 ) ;		// 왼쪽 박스
	pImageDIB->MakeFrameRect( 197, 61, 47, 259, 0xffB99E6B, 0x80000000, 1 ) ;		// 오른쪽 박스
	pImageDIB->MakeFrameRect( 160, 329, 68, 18, 0xff959595, 0x80000000, 1 ) ;		// 페이즈 박스
	pImageDIB->MakeFrameRect( 2, 354, 252, 1, 0xffB99E6B, 0x80000000,  1 ) ;		// 아래쪽 라인 박스

	for ( i = 0 ; i < 8 ; i++ )			//8개 씩 있다.
	{
		pImageDIB->MakeFrameRect( 18, 67 + 31 * i, 28, 28, 0xffB99E6B, 0x80000000, 1 ) ;		//1x1 인벤
		pImageDIB->MakeFrameRect( 52, 71 + 31 * i, 129, 19, 0xff959595, 0x80000000, 1 ) ;		//광물 이름 박스
		pImageDIB->MakeFrameRect( 204, 71 + 31 * i, 33, 19, 0xff959595, 0x80000000, 1 ) ;		//광물 수량 박스
	}
	
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	AttachWindowTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;
	
	SetWidth( 256 ) ;
	SetHeight( 437 ) ;
	SetLeft( ( CGMUIScreen::GetInstance()->GetWidth() - GetWidth() ) / 2 ) ;
	SetTop( ( CGMUIScreen::GetInstance()->GetHeight() - GetHeight() ) / 2 ) ;

	SetFormType( RYLDLG_FRAME_TYPE_CLOSEBUTTON ) ;

	m_pPrevButton				= NULL ;						// 페이즈 이동 버튼
	m_pNextButton				= NULL ;						// 페이즈 이동 버튼
	m_pPhaseLabel				= NULL ;
	m_pTitleLabel				= NULL;							// 광물의 매입량 확인 ( 타이틀 )
	m_nCurPhase = 0 ;
	m_nMaxPhase = 1 ;
	for( i = 0 ; i < 8 ; i++ )
	{
		m_pMineNameLabel[i] = NULL;						// 광물 이름
		m_pMineCountLabel[i] = NULL;					// 광물 수량
	}

	g_pMiningAmountDlg = this ;
	InitCtrl() ;
}


CRYLMiningAmountDlg::~CRYLMiningAmountDlg() 
{
	FinalCtrl() ;
}

VOID	CRYLMiningAmountDlg::InitCtrl() 
{
	CGMImageDib* pSprite			;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;

	pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ;
	RECT rt ;

	m_pPrevButton = new CRYLButton( this ) ;
	m_pPrevButton->SetLeft( 145 ) ;
	m_pPrevButton->SetTop( 347 ) ;
	m_pPrevButton->OnClickEvent = DlgClick ;
	m_pPrevButton->SetInstance( RYLMININGAMOUNTDLG::PREV_BUTTON ) ;
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
	m_pNextButton->SetLeft( 230 ) ;
	m_pNextButton->SetTop( 347 ) ;
	m_pNextButton->OnClickEvent = DlgClick ;
	m_pNextButton->SetInstance( RYLMININGAMOUNTDLG::NEXT_BUTTON ) ;
	SetRect( &rt, 232, 215, 245, 227 ) ;
	m_pNextButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 232, 243, 245, 255 ) ;
	m_pNextButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 232, 229, 245, 241 ) ;
	m_pNextButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 232, 215, 245, 227 ) ;
	m_pNextButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pNextButton ) ;

	m_pPhaseLabel = new CRYLLabel( this ) ;		
	m_pPhaseLabel->SetLeft( 169 ) ;
	m_pPhaseLabel->SetTop( 348 ) ;
	m_pPhaseLabel->SetFontColor (0xffffffff);
	m_pPhaseLabel->SetAutoSize( TRUE ) ;
	m_pPhaseLabel->SetCaption( "" ) ;
	AttachGMUIControl( m_pPhaseLabel ) ;


	m_pTitleLabel = new CRYLLabel( this ) ;		
	m_pTitleLabel->SetLeft( 12 ) ;
	m_pTitleLabel->SetTop( 18 ) ;
	m_pTitleLabel->SetFontColor (0xffffffff);
	m_pTitleLabel->SetAutoSize( TRUE ) ;
	m_pTitleLabel->SetCaption( CRYLStringTable::m_strString[3178]  ) ;
	AttachGMUIControl( m_pTitleLabel ) ;

	m_pVeinNameLabel = new CRYLLabel( this ) ;		
	m_pVeinNameLabel->SetLeft( 0 ) ;
	m_pVeinNameLabel->SetTop( 51 ) ;
	m_pVeinNameLabel->SetFontColor (0xffffffff);
	m_pVeinNameLabel->SetAutoSize( TRUE ) ;
	m_pVeinNameLabel->SetCaption( ""  ) ;
	AttachGMUIControl( m_pVeinNameLabel ) ;

	m_pDescription1 = new CRYLLabel( this ) ;
	m_pDescription1->SetLeft( 12 ) ;
	m_pDescription1->SetTop( 377 ) ;
	m_pDescription1->SetFontColor (0xffffffff);
	m_pDescription1->SetAutoSize( TRUE ) ;
	m_pDescription1->SetCaption( CRYLStringTable::m_strString[ 2901 ] ) ;
	AttachGMUIControl( m_pDescription1 ) ;

	m_pDescription2 = new CRYLLabel( this ) ;
	m_pDescription2->SetLeft( 12 ) ;
	m_pDescription2->SetTop( 393 ) ;
	m_pDescription2->SetFontColor (0xffffffff);
	m_pDescription2->SetAutoSize( TRUE ) ;
	m_pDescription2->SetCaption( CRYLStringTable::m_strString[ 2902 ] ) ;
	AttachGMUIControl( m_pDescription2 ) ;

	m_pDescription3 = new CRYLLabel( this ) ;
	m_pDescription3->SetLeft( 12 ) ;
	m_pDescription3->SetTop( 409 ) ;
	m_pDescription3->SetFontColor (0xffffffff);
	m_pDescription3->SetAutoSize( TRUE ) ;
	m_pDescription3->SetCaption( CRYLStringTable::m_strString[ 2903 ] ) ;
	AttachGMUIControl( m_pDescription3 ) ;

	m_pDescription4 = new CRYLLabel( this ) ;
	m_pDescription4->SetLeft( 12 ) ;
	m_pDescription4->SetTop( 425 ) ;
	m_pDescription4->SetFontColor (0xffffffff);
	m_pDescription4->SetAutoSize( TRUE ) ;
	m_pDescription4->SetCaption( CRYLStringTable::m_strString[ 2904 ] ) ;
	AttachGMUIControl( m_pDescription4 ) ;

	for( int i = 0 ; i < 8 ; i++ )
	{
		// 광물 이름
		m_pMineNameLabel[i] = new CRYLLabel( this ) ;		
		m_pMineNameLabel[i]->SetLeft( 57 ) ;
		m_pMineNameLabel[i]->SetTop( 90 + 31 * i ) ;
		m_pMineNameLabel[i]->SetFontColor (0xffffffff);
		m_pMineNameLabel[i]->SetAutoSize( TRUE ) ;
		m_pMineNameLabel[i]->SetCaption( "" ) ;
		AttachGMUIControl( m_pMineNameLabel[i] ) ;
		// 광물 수량
		m_pMineCountLabel[i] = new CRYLLabel( this ) ;		
		m_pMineCountLabel[i]->SetLeft( 209 ) ;
		m_pMineCountLabel[i]->SetTop( 90 + 31 * i ) ;
		m_pMineCountLabel[i]->SetFontColor (0xffffffff);
		m_pMineCountLabel[i]->SetAutoSize( TRUE ) ;
		m_pMineCountLabel[i]->SetCaption( "" ) ;
		AttachGMUIControl( m_pMineCountLabel[i] ) ;
	}
	SetPhaseLabel();
	
}

VOID	CRYLMiningAmountDlg::FinalCtrl() 
{
	ClearItem();
	GM_DELETE( m_pPrevButton );							// 페이즈 이동 버튼
	GM_DELETE( m_pNextButton );							// 페이즈 이동 버튼
	GM_DELETE( m_pPhaseLabel );
	GM_DELETE( m_pTitleLabel );							// 광물의 매입량 확인 ( 타이틀 )
	GM_DELETE( m_pVeinNameLabel );						// 광맥 이름
	GM_DELETE( m_pDescription1 );						// 설명 1
	GM_DELETE( m_pDescription2 );						// 설명 2
	GM_DELETE( m_pDescription3 );						// 설명 3
	GM_DELETE( m_pDescription4 );						// 설명 4

	for( int i = 0 ; i < 8 ; i++ )
	{
		GM_DELETE( m_pMineNameLabel[i] );					// 광물 이름
		GM_DELETE( m_pMineCountLabel[i] );					// 광물 수량
	}
}

VOID
CRYLMiningAmountDlg::ClearItem()
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

	m_pVeinNameLabel->SetCaption( "" ) ;
	for( int i = 0 ; i < 8 ; i++ )
	{
		if( m_pMineCountLabel[i] )
			m_pMineCountLabel[i]->SetCaption( "" ) ;
	}


}

VOID	CRYLMiningAmountDlg::Show( CGMUIObject* Sender ) 
{
	CRYLGameData*	pGame = CRYLGameData::Instance() ;
	pGame->SetAutoRun( FALSE ) ;
	SetPhaseLabel();
}

VOID	CRYLMiningAmountDlg::Hide( CGMUIObject* Sender ) 
{
}

VOID	CRYLMiningAmountDlg::GMUIPaint( INT xx, INT yy ) 
{
	LPDIRECT3DDEVICE8   lpD3DDevice = BaseGraphicsLayer::GetDevice() ;
	CRYLDialog::GMUIPaint( xx, yy ) ;

	std::vector< CItemInstance* >::iterator iter;
	int	i = 0;
	int nMin, nMax, nSize;
	//nMinCnt = m_nCurPhase * 8;
	//nMaxCnt = nMinCnt + 8;
	CItemInstance* pItem;


	nSize = m_vecMineCnt.size();
	nMin = m_nCurPhase*8;
	nMax = min( nMin+8, nSize );
	if( nMin < nSize )
	{
		for( i = nMin ; i < nMax ; i++ )
		{
			pItem = m_vecMineIcon[i];
			if( pItem )
			{
				int nX = 0;
				int nY = 0;
				nX = ( 28 - pItem->m_lpSprite->GetSizeX() ) /2 ;
				nY = ( 28 - pItem->m_lpSprite->GetSizeY() ) /2 ;
				pItem->m_wPutX = GetLeft() + 18 + nX;
				pItem->m_wPutY = GetTop() + 86 + 31 * (i-nMin) + nY;
				pItem->Render( lpD3DDevice );
			}
		}
	}
	/*
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
			pItem->Render( lpD3DDevice );
			i++;
		}
	}
	*/
}

HRESULT		CRYLMiningAmountDlg::Update( BOOL &bClick, BOOL &bEdge ) 
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

	return S_OK;
}

VOID	CRYLMiningAmountDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y ) 
{
	if ( !g_pMiningAmountDlg )
		return ;
	CRYLGameData* pGame = CRYLGameData::Instance() ;
	switch( Sender->GetInstance() )
	{
	case RYLMININGAMOUNTDLG::NEXT_BUTTON :
		{
			g_pMiningAmountDlg->NextPhase() ;
		}
		break;
	case RYLMININGAMOUNTDLG::PREV_BUTTON :
		{
			g_pMiningAmountDlg->PrevPhase() ;
		}
		break;
	}
}

VOID	CRYLMiningAmountDlg::NextPhase()
{
	m_nCurPhase = max( 0, min( m_nCurPhase+1, m_nMaxPhase-1 ) );
	SetItemIcon();
	SetPhaseLabel();
}

VOID	CRYLMiningAmountDlg::PrevPhase()
{
	m_nCurPhase = max( 0, m_nCurPhase -1 );
	SetPhaseLabel();
}

VOID	CRYLMiningAmountDlg::SetItemIcon()
{
	CItemInstance* lpItem = new CItemInstance;
	Item::ItemPos pos( 0, 0 );
	lpItem->SetItemInfofromID( 1915, pos );
	
	m_vecMineIcon.push_back( lpItem );
}

VOID	CRYLMiningAmountDlg::SetPhaseLabel()
{
	char strString[MAX_PATH];
	int nSize, nMin, nMax;
	int nViewPhase = min(m_nCurPhase + 1, m_nMaxPhase );
	int i;
	CItemInstance* lpItem;
	
	sprintf( strString, "%3d / %3d", nViewPhase, m_nMaxPhase ) ;
	m_pPhaseLabel->SetCaption( strString ) ;

	for( i = 0 ; i < 8 ; i++ )
	{
		m_pMineCountLabel[i]->SetCaption( "" );
		m_pMineNameLabel[i]->SetCaption( "" );
	}
	nSize = m_vecMineCnt.size();
	nMin = m_nCurPhase*8;
	nMax = min( nMin+8, nSize );
	if( nMin < nSize )
	{
		for( i = nMin ; i < nMax ; i++ )
		{
			sprintf( strString, "%3d", m_vecMineCnt[i] ) ;
			m_pMineCountLabel[i-nMin]->SetCaption( strString );
			lpItem = m_vecMineIcon[i];
			strcpy( strString, lpItem->GetItemInfo().m_SpriteData.m_szName );
			m_pMineNameLabel[i-nMin]->SetCaption( strString );
		}
	}
	
}


VOID	
CRYLMiningAmountDlg::UpdateMineral()
{
	MineralVeinInfo* pVeinInfo;
	RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
	if ( !pSelfCreature )
		return;
	CItemInstance* lpItem;
	Item::ItemPos pos( 0, 0 );
	ClearItem();

	pVeinInfo = g_MinveralVeinMgr.GetMineralVein( pSelfCreature->GetPosition()->x/100.0f, pSelfCreature->GetPosition()->z/100.0f);
	if( pVeinInfo )
	{
		if ( pVeinInfo->m_szVeinName )
		{
			int nLeft = 128 - CRYLStringTable::GetStringWidth( pVeinInfo->m_szVeinName ) / 2;
			if ( nLeft > 15 )
			{
				m_pVeinNameLabel->SetLeft( nLeft );
			}
			else
			{
				m_pVeinNameLabel->SetLeft( 15 );
			}

//			m_pVeinNameLabel->SetCaption( pVeinInfo->m_szVeinName );
		}

		int nTotalCnt = 0;
		MineralInfoList::iterator iter;
		for( iter = pVeinInfo->m_lstMineralInfo.begin(); iter != pVeinInfo->m_lstMineralInfo.end(); iter++ )
		{
			if ( 0 == (*iter).m_cMax )
				continue;

			lpItem = new CItemInstance;
			if( lpItem->SetItemInfofromID( (*iter).m_dwMineralID, pos ) == 0 )		//없는 프로토 타입이라면!
			{
				delete lpItem;
				continue;
			}

			m_vecMineIcon.push_back( lpItem );
			m_vecMineCnt.push_back( (*iter).m_cMax );

			++nTotalCnt;
		}

		nTotalCnt = (nTotalCnt + 7) / 8;
		m_nMaxPhase = max( 1, nTotalCnt );
	}
	SetVisible( TRUE );
}