
#include <winsock2.h>

#include <Network/ClientSocket/Send/SendCastle.h>
#include <Creature/Monster/MonsterMgr.h>

#include "../ItemInstance.h"

#include "WinInput.h"
#include "GMImageDib.h"
#include "BaseGraphicsLayer.h"

#include "RYLSpriteList.h"
#include "RYLImageManager.h"
#include "RYLImageCabinet.h"
#include "RYLImage.h"
#include "RYLButton.h"
#include "RYLLabel.h"
#include "RYLEdit.h"
#include "RYLStringTable.h"

#include "RYLGameData.h"
#include "RYLNetworkData.h"
#include "RYLGameScene.h"
#include "RYLSceneManager.h"
#include "RYLCastle.h"
#include "RYLCastleManager.h"

#include "RYLGuildFrameDlg.h"
#include "RYLAuthoritySettingDlg.h"
#include "RYLChattingDlg.h"

#include "RYLEmblemUpgradeDlg.h"
#include "RYLMessageBox.h"
#include "GMMemory.h"

CRYLEmblemUpgradeDlg*	g_pRYLEmblemUpgradeDlg = NULL ;

CRYLEmblemUpgradeDlg::CRYLEmblemUpgradeDlg() 
{
	RECT rt ;
	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;

	CRYLSpriteList::Instance()->CreateGMImage( 256, 15, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 0, 256, 15 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	AttachCaptionTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;

	CRYLSpriteList::Instance()->CreateGMImage( 256, 180, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	int i = 0;
	for (  ; i < 35 ; i ++ )
	{
		SetRect( &rt, 0, 38, 256, 43 ) ;
		pImageDIB->DrawRect( &rt, 0, ( i * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	}
	SetRect( &rt, 0, 42, 256, 52 ) ;
	pImageDIB->DrawRect( &rt, 0, ( ( i - 1 ) * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	
	pImageDIB->MakeFrameRect( 2, 20, 252, 1, 0xff946b42, 0xc0000000,  1 ) ;	
	pImageDIB->MakeFrameRect( 12, 73, 80, 80, 0xff946b42, 0xc0000000,  1 ) ;	

	pImageDIB->DrawFrameRect( 38, 74, 1, 79, 0xff675e46 ) ;
	pImageDIB->DrawFrameRect( 64, 74, 1, 79, 0xff675e46 ) ;
	pImageDIB->DrawFrameRect( 13, 100, 79, 1, 0xff675e46 ) ;
	pImageDIB->DrawFrameRect( 13, 126, 79, 1, 0xff675e46 ) ;
	

	/*
	pImageDIB->MakeFrameRect( 138, 29, 23, 17, 0xff959595, 0x80000000,  1 ) ;
	pImageDIB->MakeFrameRect( 138, 56, 65, 17, 0xff959595, 0x80000000,  1 ) ;
	pImageDIB->MakeFrameRect( 138, 83, 23, 17, 0xff959595, 0x80000000,  1 ) ;
	*/
	

	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	AttachWindowTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;

	
	SetWidth( 256 ) ;
	SetHeight( 192 ) ;
	SetLeft( ( CGMUIScreen::GetInstance()->GetWidth() - GetWidth() ) / 2 ) ;
	SetTop( ( CGMUIScreen::GetInstance()->GetHeight() - GetHeight() ) / 2 ) ;
	SetFormType( RYLDLG_TYPE_CLOSEBUTTON ) ;
	
	InitCtrl() ;
	
	g_pRYLEmblemUpgradeDlg = this ;
}

CRYLEmblemUpgradeDlg::~CRYLEmblemUpgradeDlg() 
{
	FinalCtrl() ;
}

VOID	CRYLEmblemUpgradeDlg::InitCtrl() 
{
	CGMImageDib* pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;

	// 업그레이드 생성 메뉴
	m_pEmblemUpgradeLabel = new CRYLLabel( this ) ;
	m_pEmblemUpgradeLabel->SetLeft( 12 ) ;
	m_pEmblemUpgradeLabel->SetTop( 18 ) ;
	m_pEmblemUpgradeLabel->SetFontColor (0xffffffff);
	m_pEmblemUpgradeLabel->SetAutoSize( TRUE ) ;
	m_pEmblemUpgradeLabel->SetCaption( CRYLStringTable::m_strString[ 2542 ] ) ;
	AttachGMUIControl( m_pEmblemUpgradeLabel ) ;

	// [ 보석 조각 ]
	m_pJewelLabel = new CRYLLabel( this ) ;
	m_pJewelLabel->SetLeft( 12 ) ;
	m_pJewelLabel->SetTop( 72 ) ;
	m_pJewelLabel->SetFontColor (0xffffffff);
	m_pJewelLabel->SetAutoSize( TRUE ) ;
	m_pJewelLabel->SetCaption( CRYLStringTable::m_strString[ 2543 ] ) ;
	AttachGMUIControl( m_pJewelLabel ) ;

	// 업그레이드 생성 설명 1
	m_pEmblemUpgradeExpla1Label = new CRYLLabel( this ) ;
	m_pEmblemUpgradeExpla1Label->SetLeft( 95 ) ;
	m_pEmblemUpgradeExpla1Label->SetTop( 93 ) ;
	m_pEmblemUpgradeExpla1Label->SetFontColor (0xffffffff);
	m_pEmblemUpgradeExpla1Label->SetAutoSize( TRUE ) ;
	m_pEmblemUpgradeExpla1Label->SetCaption( CRYLStringTable::m_strString[ 2544 ] ) ;
	AttachGMUIControl( m_pEmblemUpgradeExpla1Label ) ;

	// 업그레이드 생성 설명 2
	m_pEmblemUpgradeExpla2Label = new CRYLLabel( this ) ;
	m_pEmblemUpgradeExpla2Label->SetLeft( 95 ) ;
	m_pEmblemUpgradeExpla2Label->SetTop( 109 ) ;
	m_pEmblemUpgradeExpla2Label->SetFontColor (0xffffffff);
	m_pEmblemUpgradeExpla2Label->SetAutoSize( TRUE ) ;
	m_pEmblemUpgradeExpla2Label->SetCaption( CRYLStringTable::m_strString[ 2543 ] ) ;
	AttachGMUIControl( m_pEmblemUpgradeExpla2Label ) ;

	// 업그레이드 생성 설명 3
	m_pEmblemUpgradeExpla3Label = new CRYLLabel( this ) ;
	m_pEmblemUpgradeExpla3Label->SetLeft( 95 ) ;
	m_pEmblemUpgradeExpla3Label->SetTop( 125 ) ;
	m_pEmblemUpgradeExpla3Label->SetFontColor (0xffffffff);
	m_pEmblemUpgradeExpla3Label->SetAutoSize( TRUE ) ;
	m_pEmblemUpgradeExpla3Label->SetCaption( CRYLStringTable::m_strString[ 2545 ] ) ;
	AttachGMUIControl( m_pEmblemUpgradeExpla3Label ) ;

	// 업그레이드 생성 설명 4
	m_pEmblemUpgradeExpla4Label = new CRYLLabel( this ) ;
	m_pEmblemUpgradeExpla4Label->SetLeft( 12 ) ;
	m_pEmblemUpgradeExpla4Label->SetTop( 48 ) ;
	m_pEmblemUpgradeExpla4Label->SetFontColor (0xffffffff);
	m_pEmblemUpgradeExpla4Label->SetAutoSize( TRUE ) ;
	m_pEmblemUpgradeExpla4Label->SetCaption( CRYLStringTable::m_strString[ 2546 ] ) ;
	AttachGMUIControl( m_pEmblemUpgradeExpla4Label ) ;

	pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;

	// 확인 버튼
	m_pOKButton = new CRYLButton( this ) ;
	m_pOKButton->SetLeft( 99 ) ;
	m_pOKButton->SetTop( 159 ) ;
	m_pOKButton->SetCaption( CRYLStringTable::m_strString[ 2164 ] ) ;
	m_pOKButton->SetFontColor( 0xffffffff) ;
	m_pOKButton->SetColor( 0xffffffff) ;
	m_pOKButton->OnClickEvent = DlgClick ;
	m_pOKButton->SetInstance( RYLEMBLEMUPGRADEDLG::OK_BUTTON ) ;
	m_pOKButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pOKButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pOKButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pOKButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pOKButton ) ;

	// 취소 버튼
	m_pCancelButton = new CRYLButton( this ) ;
	m_pCancelButton->SetLeft( 173 ) ;
	m_pCancelButton->SetTop( 159 ) ;
	m_pCancelButton->SetCaption( CRYLStringTable::m_strString[ 1897 ] ) ;
	m_pCancelButton->SetFontColor( 0xffffffff) ;
	m_pCancelButton->SetColor( 0xffffffff) ;
	m_pCancelButton->OnClickEvent = DlgClick ;
	m_pCancelButton->SetInstance( RYLEMBLEMUPGRADEDLG::CANCEL_BUTTON ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pCancelButton ) ;
}

VOID	CRYLEmblemUpgradeDlg::FinalCtrl() 
{
	GM_DELETE( m_pEmblemUpgradeLabel ) ;			// 업그레이드 메뉴
	GM_DELETE( m_pJewelLabel ) ;					// [보석조각]
	GM_DELETE( m_pEmblemUpgradeExpla1Label ) ;		// 업그레이드 생성 설명 1
	GM_DELETE( m_pEmblemUpgradeExpla2Label ) ;		// 업그레이드 생성 설명 2
	GM_DELETE( m_pEmblemUpgradeExpla3Label ) ;		// 업그레이드 생성 설명 3
	GM_DELETE( m_pEmblemUpgradeExpla4Label ) ;		// 업그레이드 생성 설명 4
	GM_DELETE( m_pOKButton ) ;
	GM_DELETE( m_pCancelButton ) ;
}

VOID	CRYLEmblemUpgradeDlg::InitValue() 
{
}

VOID	CRYLEmblemUpgradeDlg::Show( CGMUIObject* Sender ) 
{
}

VOID	CRYLEmblemUpgradeDlg::Hide( CGMUIObject* Sender ) 
{
}

VOID	CRYLEmblemUpgradeDlg::GMUIPaint( INT xx, INT yy ) 
{
	LPDIRECT3DDEVICE8   lpD3DDevice = BaseGraphicsLayer::GetDevice() ;
	CRYLDialog::GMUIPaint( xx, yy ) ;

	CRYLGameData* pGame = CRYLGameData::Instance() ;
	CRYLCastle* pCastle = CRYLCastleManager::Instance()->FindCastle( m_dwCastleID ) ;
	if ( pCastle )
	{
		int iIndex = 0 ;
		CItemInstance* lpUpgradeItem = NULL ;
		for ( int y = 0 ; y < 3 ; y ++ )
		{
			for ( int x = 0 ; x < 3 ; x ++ )
			{
				lpUpgradeItem = pCastle->GetUpgradeItem( iIndex ) ;
				if ( lpUpgradeItem && lpUpgradeItem->GetNowDurability() > 0 )
				{
					lpUpgradeItem->m_wPutX = GetLeft() + 13 + ( x * 26 ) + ( 12 - lpUpgradeItem->m_lpSprite->GetSizeX() / 2 ) ;
					lpUpgradeItem->m_wPutY = GetTop()  + 89 + ( y * 26 ) + ( 12 - lpUpgradeItem->m_lpSprite->GetSizeY() / 2 ) ;

					lpUpgradeItem->Render( lpD3DDevice ) ;
				}

				++ iIndex ;
			}
		}
	}

	if ( pGame->m_lpPickItem )
	{
		if ( g_DeviceInput.InRect( GetLeft() + 13, GetTop() + 89, GetLeft() + 78, GetTop() + 78 ) )
		{
//			if ( pGame->m_lpPickItem->GetIsEnableUpgrade() )
//			{
				pGame->RenderRect( lpD3DDevice, GetLeft() + 13, GetTop() + 89, GetLeft() + 78, GetTop() + 78, 0xFF0000FF, 0x80 ) ;
//			}
//			else
//			{
//				pGame->RenderRect( lpD3DDevice, GetLeft() + 13, GetTop() + 89, GetLeft() + 78, GetTop() + 78, 0xFFFF0000, 0x80 ) ;
//			}
		} 
	}
}

HRESULT		CRYLEmblemUpgradeDlg::Render( LPDIRECT3DDEVICE8 lpD3DDevice )  
{
	return S_OK ;
}

HRESULT		CRYLEmblemUpgradeDlg::FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) 
{
	return S_OK ;
}

HRESULT		CRYLEmblemUpgradeDlg::Update( BOOL &bClick, BOOL &bEdge ) 
{
	if (FALSE == GetVisible())
	{
		return S_OK;
	}

	CRYLCastle* lpCastle = CRYLCastleManager::Instance()->FindCastle( m_dwCastleID ) ;
	RYLCreature* lpEmblem = RYLCreatureManager::Instance()->GetCreature( m_dwEmblemCID ) ;
	if ( !lpCastle || !lpEmblem )
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

	CRYLGameData* pGame = CRYLGameData::Instance() ;
	CRYLNetworkData* pNetwork = CRYLNetworkData::Instance() ;
	POINT *ptMousePos = g_DeviceInput.GetMouseLocal() ;
	unsigned long dwMousePosX = ptMousePos->x - ( GetLeft() + 9 ) ;
	unsigned long dwMousePosY = ptMousePos->y - ( GetTop() + 75 ) ;
	unsigned long dwWindowX = dwMousePosX / 26 ;
	unsigned long dwWindowY = dwMousePosY / 26 ;

	int iIndex = 0 ;

	for ( int y = 0 ; y < 3 ; y ++ )
	{
		for ( int x = 0 ; x < 3 ; x ++ )
		{
			int iX = 13 + ( x * 27 ) ;
			int iY = 89 + ( y * 27 ) ;

			if ( g_DeviceInput.InRect( GetLeft() + iX, GetTop() + iY, GetLeft() + iX + 26, GetTop() + iY + 26 ) )
			{
				if ( g_DeviceInput.GetIsLeftMouseDown() )
				{
                    if ( pGame->m_lpPickItem )	// 손에 있을시
					{
						if ( pGame->m_lpPickItem->GetItemType() == Item::ItemType::GEM )
						{
							// 업그레이드된 상징물이라면...
							if ( lpEmblem->GetUpgradeStep() > 0 )
							{
								unsigned short wNeedItemID = Siege::JEWEL_PROTOTYPE_ID + Siege::JEWEL_KIND * lpEmblem->GetUpgradeStep() + lpEmblem->GetUpgradeType() ;
								if ( pGame->m_lpPickItem->GetProtoTypeID() != wNeedItemID )
								{
									CRYLMessageBox* lpMessageBox = new CRYLMessageBox ;
									lpMessageBox->Create( CRYLStringTable::m_strString[4000] ) ;
									return S_OK ;
								}
							}
							else
							{
								unsigned short wCurrItemID = lpCastle->GetCurrentItemID() ;
								if ( 0 != wCurrItemID && pGame->m_lpPickItem->GetProtoTypeID() != wCurrItemID )
								{
									CRYLMessageBox* lpMessageBox = new CRYLMessageBox ;
									lpMessageBox->Create( CRYLStringTable::m_strString[4001] ) ;
									return S_OK ;
								}

								if ( 0 == wCurrItemID && pGame->m_lpPickItem->GetProtoTypeID() - Siege::JEWEL_PROTOTYPE_ID > Siege::JEWEL_KIND )
								{
									CRYLMessageBox* lpMessageBox = new CRYLMessageBox ;
									lpMessageBox->Create( CRYLStringTable::m_strString[4002] ) ;
									return S_OK ;
								}
							}

							CItemInstance* lpUpgradeItem = lpCastle->GetUpgradeItem( iIndex ) ;
							if ( lpUpgradeItem )
							{
								if ( lpUpgradeItem->GetNowDurability() < lpUpgradeItem->GetMaxDurability() )
								{
									unsigned char cNum = 0 ;
									if ( lpUpgradeItem->GetMaxDurability() - lpUpgradeItem->GetNowDurability() < pGame->m_lpPickItem->GetNowDurability() )
									{
										cNum = lpUpgradeItem->GetMaxDurability() - lpUpgradeItem->GetNowDurability() ;
									}
									else
									{
										cNum = pGame->m_lpPickItem->GetNowDurability() ;
									}

									pNetwork->SendTakeCastleJewel(m_dwCastleID, PktTakeCastleJewel::SC_TAKE_IN, lpUpgradeItem->GetProtoTypeID(),
										iIndex, cNum, pGame->m_lpPickItem->m_lpItemBase->GetPos());
								}

								return S_OK ;
							}
							else
							{
								pNetwork->SendTakeCastleJewel(m_dwCastleID, PktTakeCastleJewel::SC_TAKE_IN, pGame->m_lpPickItem->GetProtoTypeID(),
									iIndex, pGame->m_lpPickItem->GetNowDurability(), pGame->m_lpPickItem->m_lpItemBase->GetPos());

								return S_OK ;
							}
						}
					}
					else
					{
						// 광물을 줍는다.
						CItemInstance* lpUpgradeItem = lpCastle->GetUpgradeItem( iIndex ) ;
						if ( pGame->m_dwKindCursor == CURSOR_NORMAL && lpUpgradeItem )
						{
							if ( lpUpgradeItem->GetNowDurability() > 0 )
							{
								Item::ItemPos ItemIndex ;
								ItemIndex.m_cIndex = Item::ExtraSpacePos::HOLDITEM_POS ;
								ItemIndex.m_cPos = TakeType::TS_TEMP ;

								pNetwork->SendTakeCastleJewel(m_dwCastleID, PktTakeCastleJewel::SC_TAKE_OUT, lpUpgradeItem->GetProtoTypeID(),
									iIndex, lpUpgradeItem->GetNowDurability(), ItemIndex);
							}
						}
					}
				}
			}

			++ iIndex ;
		}
	}

	return S_OK ;
}

CRYLEmblemUpgradeDlg::eUpgradeError	CRYLEmblemUpgradeDlg::CheckUpgradable()
{
	CRYLGameData* pGame = CRYLGameData::Instance() ;
	eUpgradeError eError = S_ALL_READY ;
	int nJewelType = 0 ;

	CRYLCastle* lpCastle = CRYLCastleManager::Instance()->FindCastle( m_dwCastleID ) ;
	if ( lpCastle )
	{
		CRYLGameScene*				pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );
		CRYLGuildFrameDlg*			pDlg   = ( CRYLGuildFrameDlg* )pScene->GetGuildFrameDlg();
		CRYLAuthoritySettingDlg*	pASDlg = ( CRYLAuthoritySettingDlg* )pDlg->GetAuthoritySettingDlg();

		if ( false == lpCastle->GetCastleRight()->Check( CastleRight::CONTROL_CASTLE_EMBLEM, pASDlg->GetTitle() ) )
		{
			eError = E_NOT_RIGHT ;
		}
		else
		{
            RYLCreature* lpEmblem = RYLCreatureManager::Instance()->GetCreature( m_dwEmblemCID ) ;
			if ( lpEmblem )
			{
				if ( lpEmblem->GetUpgradeStep() == Siege::MAX_EMBLEM_UPGRADE )
				{
					eError = E_MAX_LEVEL ;
				}
				else
				{
					for (int iIndex = 0; iIndex < Castle::EMBLEM_UPGRADE_JEWEL_POS_NUM; ++iIndex)
					{
						CItemInstance* lpUpgradeItem = lpCastle->GetUpgradeItem( iIndex ) ;
						if ( lpUpgradeItem )
						{
							if ( lpUpgradeItem->GetNowDurability() < Castle::EMBLEM_UPGRADE_JEWEL_NUM_PER_POS )
							{
								eError = E_NOT_ENOUGH_NUM ;
								break ;
							}

							if ( 0 == nJewelType )
							{
								nJewelType = lpUpgradeItem->GetProtoTypeID() ;
							}
							else if ( nJewelType != lpUpgradeItem->GetProtoTypeID() )
							{
								eError = E_DIFFERENT_JEWEL ;
								break ;
							}
						}
						else
						{
							eError = E_NOT_ENOUGH_NUM ;
							break ;
						}
					}

					if ( S_ALL_READY == eError )
					{
						int iUpgradeType = ( nJewelType - Siege::JEWEL_PROTOTYPE_ID ) - Siege::JEWEL_KIND * lpEmblem->GetUpgradeStep() ;

						if ( iUpgradeType < 0 || iUpgradeType > Siege::JEWEL_KIND )
						{
							eError = E_DIFFERENT_JEWEL ;
						}
						else if ( 0 > lpEmblem->GetUpgradeStep() && lpEmblem->GetUpgradeType() != iUpgradeType )
						{
							eError = E_DIFFERENT_JEWEL ;
						}
					}
				}
			}
		}
	}

	return eError ;
}

VOID	CRYLEmblemUpgradeDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y ) 
{
	if ( !g_pRYLEmblemUpgradeDlg )
		return ;

	switch( Sender->GetInstance() )
	{
		case RYLEMBLEMUPGRADEDLG::OK_BUTTON :
			{
				CRYLChattingDlg* pChat = CRYLChattingDlg::Instance() ;
				eUpgradeError eError = g_pRYLEmblemUpgradeDlg->CheckUpgradable() ;

				switch ( eError )
				{
				case S_ALL_READY :
					{
						CRYLCastle* pCastle = CRYLCastleManager::Instance()->FindCastle( g_pRYLEmblemUpgradeDlg->GetCastleID() ) ;
						if ( pCastle )
						{
							SendPacket::CharCastleCmd( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, 
														g_pRYLEmblemUpgradeDlg->GetCastleID(),
														g_pRYLEmblemUpgradeDlg->GetEmblemCID(),
														0, 0, PktCastleCmd::CASTLE_UPGRADE_EMBLEM ) ;
						}

						g_pRYLEmblemUpgradeDlg->SetVisible( FALSE ) ;
					}
					break;

				case E_NOT_RIGHT :
					pChat->AddMessage( CRYLStringTable::m_strString[ 2547 ], CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
					break ;

				case E_MAX_LEVEL :
					pChat->AddMessage( CRYLStringTable::m_strString[ 2548 ], CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
					break ;

				case E_NOT_ENOUGH_NUM :
					pChat->AddMessage( CRYLStringTable::m_strString[ 2549 ], CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
					break ;

				case E_DIFFERENT_JEWEL :
					pChat->AddMessage( CRYLStringTable::m_strString[ 2550 ], CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
					break ;
				}
			}
			break ;
		case RYLEMBLEMUPGRADEDLG::CANCEL_BUTTON :
			{
				g_pRYLEmblemUpgradeDlg->SetVisible( FALSE ) ;
			}
			break ;
	}
}

void	CRYLEmblemUpgradeDlg::SetEmblemInfo( unsigned long dwEmblemID ) 
{
    RYLCreature*    lpCreature = RYLCreatureManager::Instance()->GetCreature( dwEmblemID ) ;
	if ( !lpCreature )
	{
		return ;
	}

	CRYLCastle*     lpCastle   = CRYLCastleManager::Instance()->FindCastle( lpCreature->GetCastleID() ) ;
	if ( !lpCastle )
	{
		return ;
	}

	RYLCreature*    lpEmblem   = lpCastle->GetEmblem() ;
    if ( lpEmblem )
	{
		char szBuff[ 64 ] ;
		char szTemp[ 32 ] ;

		if ( lpEmblem->GetUpgradeStep() == 0 )
		{
			m_pEmblemUpgradeExpla1Label->SetCaption( CRYLStringTable::m_strString[ 2544 ] ) ;
			m_pEmblemUpgradeExpla2Label->SetCaption( CRYLStringTable::m_strString[ 2543 ] ) ;
			return ;
		}
		else 
		{
			switch (lpCreature->GetUpgradeType() )
			{
				case Siege::RUBY :			strcpy( szTemp, CRYLStringTable::m_strString[ 2551 ] ) ; 	break ;
				case Siege::EMERALD :		strcpy( szTemp, CRYLStringTable::m_strString[ 2552 ] ) ; 	break ;
				case Siege::SAPPHIRE :		strcpy( szTemp, CRYLStringTable::m_strString[ 2553 ] )  ;	break ;
				case Siege::DIAMOND :		strcpy( szTemp, CRYLStringTable::m_strString[ 2554 ] )	;	break ;
				case Siege::BLACKMOON :		strcpy( szTemp, CRYLStringTable::m_strString[ 2555 ] )	;	break ;
			}

			sprintf( szBuff, CRYLStringTable::m_strString[ 2556 ], lpEmblem->GetUpgradeStep() + 1 ) ;
			m_pEmblemUpgradeExpla1Label->SetCaption( szBuff ) ;

			if ( lpEmblem->GetUpgradeStep() == 1 )
				sprintf( szBuff, CRYLStringTable::m_strString[ 2557 ], szTemp ) ;
			else if ( lpEmblem->GetUpgradeStep() == 2 )
				sprintf( szBuff, "[ %s ]", szTemp ) ;

			m_pEmblemUpgradeExpla2Label->SetCaption( szBuff ) ;
		}
	}	
}
