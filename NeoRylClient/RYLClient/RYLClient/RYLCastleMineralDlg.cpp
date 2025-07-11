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

#include "RYLCastleMineralDlg.h"
#include <Item/ItemMgr.h>
#include <Item/ItemFactory.h>
#include "SoundMgr.h"

#include "RYLCastleManager.h"
#include "RYLCastle.h"
#include "GMMemory.h"


CRYLCastleMineralDlg*		g_pCastleMineralDlg = NULL ;

// 328 * 370
CRYLCastleMineralDlg::CRYLCastleMineralDlg() 
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

	CRYLSpriteList::Instance()->CreateGMImage( 328, 320, &pImageDIB ) ;

	int i;
	CGMImageDib*	pTempImage;
	pTempImage = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_GUILD_256x256 );
	for ( i = 0 ; i < 62 ; i ++ )
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

	SetWidth( 328 ) ;
	SetHeight( 335 ) ;
	SetLeft( ( CGMUIScreen::GetInstance()->GetWidth() - GetWidth() ) / 2 ) ;
	SetTop( ( CGMUIScreen::GetInstance()->GetHeight() - GetHeight() ) / 2 ) ;

	SetFormType( RYLDLG_FRAME_TYPE_CLOSEBUTTON ) ;

	m_pPrevButton				= NULL ;							// 페이즈 이동 버튼
	m_pNextButton				= NULL ;							// 페이즈 이동 버튼
	m_pPhaseLabel				= NULL ;
	m_pTitleLabel				= NULL ;							// 광물의 매입량 확인 ( 타이틀 )

	m_nCurPhase = 0 ;
	m_nMaxPhase = 1 ;
	for( i = 0 ; i < 8 ; i++ )
	{
		m_pMineNameLabel[i] = NULL;					// 광물 이름
		m_pMineCountLabel[i] = NULL;				// 광물 수량
		m_pGetMineButton[i] = NULL;					// 광물 획득 버튼
	}

	m_dwSplitResult = 0 ;
	m_dwSplitValue	= 0 ;
	m_dwSplitIndex  = 0 ;
	m_dwCastleID	= 0 ;
	g_pCastleMineralDlg = this ;

	InitCtrl() ;
}


CRYLCastleMineralDlg::~CRYLCastleMineralDlg() 
{
	FinalCtrl() ;
}

VOID	CRYLCastleMineralDlg::InitCtrl() 
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
	m_pPrevButton->SetInstance( RYLCASTLEMINERALDLG::PREV_BUTTON ) ;
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
	m_pNextButton->SetInstance( RYLCASTLEMINERALDLG::NEXT_BUTTON ) ;
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
		m_pGetMineButton[i]->SetInstance( RYLCASTLEMINERALDLG::GET_BUTTON_0 + i ) ;
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


	m_pTitleLabel = new CRYLLabel( this ) ;		
	m_pTitleLabel->SetLeft( 12 ) ;
	m_pTitleLabel->SetTop( 18 ) ;
	m_pTitleLabel->SetFontColor (0xffffffff);
	m_pTitleLabel->SetAutoSize( TRUE ) ;
	m_pTitleLabel->SetCaption( CRYLStringTable::m_strString[4006] ) ;
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

VOID	CRYLCastleMineralDlg::FinalCtrl() 
{
	ClearItem();
	GM_DELETE( m_pPrevButton );							// 페이즈 이동 버튼
	GM_DELETE( m_pNextButton );							// 페이즈 이동 버튼
	GM_DELETE( m_pPhaseLabel );
	GM_DELETE( m_pTitleLabel );							// 광물의 매입량 확인 ( 타이틀 )

	for( int i = 0 ; i < 8 ; i++ )
	{
		GM_DELETE( m_pMineNameLabel[i] );					// 광물 이름
		GM_DELETE( m_pMineCountLabel[i] );					// 광물 수량
		GM_DELETE( m_pGetMineButton[i] );					// 광물 획득 버튼
	}
}

VOID	CRYLCastleMineralDlg::Show( CGMUIObject* Sender ) 
{
	CRYLGameData* pGame = CRYLGameData::Instance() ;
	pGame->SetAutoRun( FALSE ) ;
	InitMineral();
}

VOID	CRYLCastleMineralDlg::Hide( CGMUIObject* Sender ) 
{
}

VOID	CRYLCastleMineralDlg::GMUIPaint( INT xx, INT yy ) 
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

HRESULT		CRYLCastleMineralDlg::Update( BOOL &bClick, BOOL &bEdge ) 
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
			if ( m_dwCastleID )
			{
				// 광물 세금 회수
				CRYLCastle* pCastle = CRYLCastleManager::Instance()->FindCastle( m_dwCastleID ) ;
				if ( pCastle )
				{
					RYLCreature* pEmblem = pCastle->GetEmblem() ;
					if ( pEmblem )
					{
						SendPacket::CharCastleCmd( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, 
							m_dwCastleID, pEmblem->GetCharID(), m_dwSplitIndex, m_dwSplitValue, PktCastleCmd::CASTLE_GAIN_MINERAL ) ;
					}
				}
			}
			m_dwSplitIndex = 0;
			m_dwSplitValue = 0;
		}
		m_dwSplitIndex = 0;
		m_dwSplitValue = 0;
	}

	return S_OK;
}

VOID	CRYLCastleMineralDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y ) 
{
	if ( !g_pCastleMineralDlg )
		return ;

	CRYLGameData* pGame = CRYLGameData::Instance() ;
	switch( Sender->GetInstance() )
	{
		case RYLCASTLEMINERALDLG::NEXT_BUTTON :
		{
			g_pCastleMineralDlg->NextPhase() ;
		}
		break;

		case RYLCASTLEMINERALDLG::PREV_BUTTON :
		{
			g_pCastleMineralDlg->PrevPhase() ;
		}
		break;

		case RYLCASTLEMINERALDLG::GET_BUTTON_0 :
		{
			g_pCastleMineralDlg->GetMine( 0 );
		}
		break;

		case RYLCASTLEMINERALDLG::GET_BUTTON_1 :
		{
			g_pCastleMineralDlg->GetMine( 1 );
		}
		break;

		case RYLCASTLEMINERALDLG::GET_BUTTON_2 :
		{
			g_pCastleMineralDlg->GetMine( 2 );
		}
		break;

		case RYLCASTLEMINERALDLG::GET_BUTTON_3 :
		{
			g_pCastleMineralDlg->GetMine( 3 );
		}
		break;

		case RYLCASTLEMINERALDLG::GET_BUTTON_4 :
		{
			g_pCastleMineralDlg->GetMine( 4 );
		}
		break;

		case RYLCASTLEMINERALDLG::GET_BUTTON_5 :
		{
			g_pCastleMineralDlg->GetMine( 5 );
		}
		break;
			
		case RYLCASTLEMINERALDLG::GET_BUTTON_6 :
		{
			g_pCastleMineralDlg->GetMine( 6 );
		}
		break;

		case RYLCASTLEMINERALDLG::GET_BUTTON_7 :
		{
			g_pCastleMineralDlg->GetMine( 7 );
		}
		break;
	}
}

VOID	CRYLCastleMineralDlg::NextPhase()
{
	m_nCurPhase = max( 0, min( m_nCurPhase+1, m_nMaxPhase-1 ) );
	//SetItemIcon();
	SetPhaseLabel();
}

VOID	CRYLCastleMineralDlg::PrevPhase()
{
	m_nCurPhase = max( 0, m_nCurPhase -1 );
	SetPhaseLabel();
}

VOID	CRYLCastleMineralDlg::AddItem( unsigned short id, unsigned short amount )
{
	CItemInstance* lpItem = new CItemInstance;
	Item::ItemPos pos( 0, 0 );
	lpItem->SetItemInfofromID( id, pos );

	m_vecMineIcon.push_back( lpItem );
	m_vecMineCnt.push_back( amount );
}


VOID
CRYLCastleMineralDlg::ClearItem()
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


VOID	CRYLCastleMineralDlg::SetPhaseLabel()
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
}


VOID				
//광물 획득 버튼 ( 1~8 )
CRYLCastleMineralDlg::GetMine( int index )
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
CRYLCastleMineralDlg::InitMineral()
{
	if ( m_dwCastleID )
	{
		// 광물 세금을 요청한다.
		CRYLCastle* pCastle = CRYLCastleManager::Instance()->FindCastle( m_dwCastleID ) ;
		if ( pCastle )
		{
			RYLCreature* pEmblem = pCastle->GetEmblem() ;
			if ( pEmblem )
			{
				SendPacket::CharCastleCmd( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, 
					m_dwCastleID, pEmblem->GetCharID(), 0, 0, PktCastleCmd::CASTLE_SHOW_MINERAL ) ;
			}
		}
	}
}


VOID	
CRYLCastleMineralDlg::SetMineral( unsigned long dwCastleID, unsigned char cNum, CastleMineral* lpMineralList )
{
	ClearItem();
	m_nMaxPhase = (cNum + 7 ) / 8 ;
	m_nMaxPhase = max( 1, m_nMaxPhase );
	for( int i = 0 ; i < cNum ; i++ )
	{
		AddItem( lpMineralList[ i ].m_wMineralID, lpMineralList[ i ].m_wAmount );
	}
	SetPhaseLabel();
}


BOOL
CRYLCastleMineralDlg::RemoveItem( unsigned short id, unsigned short amountleft )
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

