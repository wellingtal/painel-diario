
#include <winsock2.h>

#include <Item/ItemMgr.h>
#include <Item/ItemFactory.h>
#include <Network/ClientSocket/Send/SendCastle.h>

#include "WinInput.h"
#include "GMImageDib.h"

#include "RYLImageManager.h"
#include "RYLImageCabinet.h"
#include "RYLImage.h"
#include "RYLSpriteList.h"
#include "RYLButton.h"
#include "RYLLabel.h"
#include "RYLEdit.h"
#include "RYLMessageBox.h"
#include "RYLStringTable.h"

#include "RYLGameScene.h"
#include "RYLSceneManager.h"
#include "RYLGameData.h"
#include "RYLNetworkData.h"

#include "RYLCamp.h"
#include "RYLCampManager.h"

#include "RYLCampRepairDlg.h"
#include "GMMemory.h"

CRYLCampRepairDlg*	g_pRYLCampRepairDlg = NULL ;

CRYLCampRepairDlg::CRYLCampRepairDlg() 
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
	pImageDIB->MakeFrameRect( 13, 73, 55, 55, 0xff946b42, 0xc0000000,  1 ) ;	// 25 -> 13, 93(78) -> 88(73)

	pImageDIB->DrawFrameRect( 14, 100, 53, 1, 0xff675e46 ) ;
	pImageDIB->DrawFrameRect( 39, 74, 1, 53, 0xff675e46 ) ;

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

	g_pRYLCampRepairDlg = this ;

	m_dwCampID = 0L ;
	m_dwCurrHP = 0L ;
	m_dwMaxHP = 0L ;
	m_bWorldWeapon = FALSE;
}

CRYLCampRepairDlg::~CRYLCampRepairDlg() 
{
	FinalCtrl() ;
}

VOID	CRYLCampRepairDlg::InitCtrl() 
{
	CHAR strString[MAX_PATH];
	CGMImageDib* pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;

	// 길드 요새 수리 메뉴
	m_pCampRepairMenuLabel = new CRYLLabel( this ) ;
	m_pCampRepairMenuLabel->SetLeft( 12 ) ;
	m_pCampRepairMenuLabel->SetTop( 18 ) ;
	m_pCampRepairMenuLabel->SetFontColor (0xffffffff);
	m_pCampRepairMenuLabel->SetAutoSize( TRUE ) ;
	m_pCampRepairMenuLabel->SetCaption( CRYLStringTable::m_strString[ 3052 ] ) ;
	AttachGMUIControl( m_pCampRepairMenuLabel ) ;

	// 길드 요새 내구도
	m_pCampDurability = new CRYLLabel( this ) ;
	m_pCampDurability->SetLeft( 78 ) ;
	m_pCampDurability->SetTop( 104 ) ;
	m_pCampDurability->SetFontColor (0xffffffff);
	m_pCampDurability->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pCampDurability ) ;

	// 길드 요새 수리 설명 1
	m_pCampRepairDescLabel1 = new CRYLLabel( this ) ;
	m_pCampRepairDescLabel1->SetLeft( 12 ) ;
	m_pCampRepairDescLabel1->SetTop( 48 ) ;
	m_pCampRepairDescLabel1->SetFontColor (0xffffffff);
	m_pCampRepairDescLabel1->SetAutoSize( TRUE ) ;
	m_pCampRepairDescLabel1->SetCaption( CRYLStringTable::m_strString[ 2696 ] ) ;
	AttachGMUIControl( m_pCampRepairDescLabel1 ) ;

	// 길드 요새 수리 설명 2
	m_pCampRepairDescLabel2 = new CRYLLabel( this ) ;
	m_pCampRepairDescLabel2->SetLeft( 78 ) ;
	m_pCampRepairDescLabel2->SetTop( 88 ) ;
	m_pCampRepairDescLabel2->SetFontColor (0xffffffff);
	m_pCampRepairDescLabel2->SetAutoSize( TRUE ) ;
	m_pCampRepairDescLabel2->SetCaption( CRYLStringTable::m_strString[ 3053 ] ) ;
	AttachGMUIControl( m_pCampRepairDescLabel2 ) ;

	// 길드 요새 수리 설명 3
	sprintf( strString, CRYLStringTable::m_strString[ 3054 ], Siege::CAMP_REPAIR_HP_PER_MATERIAL ) ;
	m_pCampRepairDescLabel3 = new CRYLLabel( this ) ;
	m_pCampRepairDescLabel3->SetLeft( 78 ) ;
	m_pCampRepairDescLabel3->SetTop( 120 ) ;
	m_pCampRepairDescLabel3->SetFontColor (0xffffffff);
	m_pCampRepairDescLabel3->SetAutoSize( TRUE ) ;
	m_pCampRepairDescLabel3->SetCaption( strString ) ;
	AttachGMUIControl( m_pCampRepairDescLabel3 ) ;

	// 길드 요새 수리 설명 4
	sprintf( strString, CRYLStringTable::m_strString[ 3055 ], Siege::CAMP_REPAIR_TIME ) ;
	m_pCampRepairDescLabel4 = new CRYLLabel( this ) ;
	m_pCampRepairDescLabel4->SetLeft( 78 ) ;
	m_pCampRepairDescLabel4->SetTop( 136 ) ;
	m_pCampRepairDescLabel4->SetFontColor (0xffffffff);
	m_pCampRepairDescLabel4->SetAutoSize( TRUE ) ;
	m_pCampRepairDescLabel4->SetCaption( strString ) ;
	AttachGMUIControl( m_pCampRepairDescLabel4 ) ;

	pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;
	// 확인 버튼
	m_pOKButton = new CRYLButton( this ) ;
	m_pOKButton->SetLeft( 99 ) ;
	m_pOKButton->SetTop( 159 ) ;
	m_pOKButton->SetCaption( CRYLStringTable::m_strString[ 3056 ] ) ;
	m_pOKButton->SetFontColor( 0xffffffff) ;
	m_pOKButton->SetColor( 0xffffffff) ;
	m_pOKButton->OnClickEvent = DlgClick ;
	m_pOKButton->SetInstance( RYLCAMPREPAIRDLG::OK_BUTTON ) ;
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
	m_pCancelButton->SetInstance( RYLCAMPREPAIRDLG::CANCEL_BUTTON ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pCancelButton ) ;
}

VOID	CRYLCampRepairDlg::FinalCtrl() 
{
	GM_DELETE( m_pCampRepairMenuLabel ) ;			// 길드 요새 수리 메뉴
	GM_DELETE( m_pCampDurability ) ;				// 길드 요새 내구도
	GM_DELETE( m_pCampRepairDescLabel1 ) ;			// 길드 요새 수리 설명 1
	GM_DELETE( m_pCampRepairDescLabel2 ) ;			// 길드 요새 수리 설명 2
	GM_DELETE( m_pCampRepairDescLabel3 ) ;			// 길드 요새 수리 설명 3
	GM_DELETE( m_pCampRepairDescLabel4 ) ;			// 길드 요새 수리 설명 4
	GM_DELETE( m_pOKButton ) ;
	GM_DELETE( m_pCancelButton ) ;
}

VOID	CRYLCampRepairDlg::InitValue() 
{
}

VOID	CRYLCampRepairDlg::Show( CGMUIObject* Sender ) 
{
	ChangeDlg();
}

VOID	CRYLCampRepairDlg::Hide( CGMUIObject* Sender ) 
{
	m_bWorldWeapon = FALSE;
	m_dwCurrHP = m_dwMaxHP = 0L;
}

VOID	CRYLCampRepairDlg::GMUIPaint( INT xx, INT yy ) 
{
	LPDIRECT3DDEVICE8   lpD3DDevice = BaseGraphicsLayer::GetDevice() ;
	CRYLDialog::GMUIPaint( xx, yy ) ;

	CRYLGameData* pGame = CRYLGameData::Instance() ;

	CRYLCamp* pCamp = CRYLCampManager::Instance()->FindCamp( m_dwCampID ) ;
	if ( pCamp && pCamp->GetMaterial() )
	{
		if ( pCamp->GetMaterial()->GetNowDurability() > 0 )
		{
			pCamp->GetMaterial()->m_wPutX = GetLeft() - 6 + ( pCamp->GetMaterial()->m_lpSprite->GetSizeX() / 2 ) ;
			pCamp->GetMaterial()->m_wPutY = GetTop()  + 73 + ( pCamp->GetMaterial()->m_lpSprite->GetSizeY() / 2 ) ;
			pCamp->GetMaterial()->Render( lpD3DDevice ) ;
		}
	}

	if ( pGame->m_lpPickItem )
	{
		if ( g_DeviceInput.InRect( GetLeft() + 13, GetTop() + 88, GetLeft() + 66, GetTop() + 142 ) )
		{
			pGame->RenderRect( lpD3DDevice, GetLeft() + 14, GetTop() + 89, GetLeft() + 67, GetTop() + 143, 0xFF0000FF, 0x80 ) ;
		} 
	}
}

HRESULT		CRYLCampRepairDlg::Render( LPDIRECT3DDEVICE8 lpD3DDevice )  
{
	return S_OK ;
}

HRESULT		CRYLCampRepairDlg::FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) 
{
	return S_OK ;
}

HRESULT		CRYLCampRepairDlg::Update( BOOL &bClick, BOOL &bEdge ) 
{
	if (FALSE == GetVisible())
	{
		return S_OK;
	}

	if (0 != m_dwCampID)
	{
		CRYLCamp* pCamp = CRYLCampManager::Instance()->FindCamp( m_dwCampID ) ;
		if ( pCamp )
		{
			CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( pCamp->GetCID() );
			if ( pTempData && (pTempData->m_lCurrHP != m_dwCurrHP || pTempData->m_ulMaxHP != m_dwMaxHP) )
			{
				m_dwCurrHP = pTempData->m_lCurrHP ;
				m_dwMaxHP = pTempData->m_ulMaxHP ;

				CHAR strString[MAX_PATH];
				sprintf( strString, "[ %6d / %6d ]", pTempData->m_lCurrHP, pTempData->m_ulMaxHP ) ;
				m_pCampDurability->SetCaption( strString ) ;
			}
		}
		
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
	unsigned long dwMousePosX = ptMousePos->x - ( GetLeft() + 13 ) ;
	unsigned long dwMousePosY = ptMousePos->y - ( GetTop() + 73 ) ;
	unsigned long dwWindowX = dwMousePosX / 26 ;
	unsigned long dwWindowY = dwMousePosY / 26 ;

	if (g_DeviceInput.InRect( GetLeft() + 13, GetTop() + 88, GetLeft() + 66, GetTop() + 142 ) )
	{
		if (g_DeviceInput.GetIsLeftMouseDown())
		{
			CRYLCamp* pCamp = CRYLCampManager::Instance()->FindCamp( m_dwCampID ) ;
			if ( pCamp )
			{
				if ( pGame->m_lpPickItem )	// 손에 있을시
				{
					if ( pGame->m_lpPickItem->GetItemType() == Item::ItemType::ETC )
					{
						if ( pCamp->GetMaterial() )
						{
							if (pGame->m_lpPickItem->GetProtoTypeID() == pCamp->GetMaterial()->GetProtoTypeID())
							{
								if (pCamp->GetMaterial()->GetNowDurability() < pCamp->GetMaterial()->GetMaxDurability())
								{
									unsigned char cNum = 0;
									if (pCamp->GetMaterial()->GetMaxDurability() - pCamp->GetMaterial()->GetNowDurability() < pGame->m_lpPickItem->GetNowDurability())
									{
										cNum = pCamp->GetMaterial()->GetMaxDurability() - pCamp->GetMaterial()->GetNowDurability();
									} 
									else
									{
										cNum = pGame->m_lpPickItem->GetNowDurability();
									}

									pNetwork->SendTakeMaterial(m_dwCampID, PktTakeMaterial::SC_TAKE_IN, cNum, pGame->m_lpPickItem->m_lpItemBase->GetPos());
								}
								else
								{
									CRYLMessageBox* pMessageBox = new CRYLMessageBox ;
									pMessageBox->Create( CRYLStringTable::m_strString[ 3566 ] ) ;
								}

								return S_OK;
							} 
							else	// 다른 광물
							{
								// 에러 메세지 출력!!!!

								return S_OK;
							}
						} 
						else
						{
							pNetwork->SendTakeMaterial(m_dwCampID, PktTakeMaterial::SC_TAKE_IN,
								pGame->m_lpPickItem->GetNowDurability(), pGame->m_lpPickItem->m_lpItemBase->GetPos());

							return S_OK;
						}
					}
				} 
				else
				{
					// 광물을 줍는다.
					if (pGame->m_dwKindCursor == CURSOR_NORMAL && pCamp->GetMaterial())
					{
						if ( pCamp->GetMaterial()->GetNowDurability() > 0 )
						{
							Item::ItemPos ItemIndex;
							ItemIndex.m_cIndex = Item::ExtraSpacePos::HOLDITEM_POS;
							ItemIndex.m_cPos = TakeType::TS_TEMP;

							pNetwork->SendTakeMaterial(m_dwCampID, PktTakeMaterial::SC_TAKE_OUT,
								pCamp->GetMaterial()->GetNowDurability(), ItemIndex);
						}
					}
				}
			}
		}
	}

	return S_OK ;
}

VOID	CRYLCampRepairDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y ) 
{
	if ( !g_pRYLCampRepairDlg )
		return ;

	switch( Sender->GetInstance() )
	{
	case RYLCAMPREPAIRDLG::OK_BUTTON :
		{
			CRYLCamp* pCamp = CRYLCampManager::Instance()->FindCamp( g_pRYLCampRepairDlg->m_dwCampID );
			if ( pCamp )
			{
				if ( pCamp->GetMaterial() )
				{
					CRYLNetworkData* pNetwork = CRYLNetworkData::Instance() ;
					SendPacket::CharCampCmd( g_GameSession, 
						pNetwork->m_dwMyChrID, 
						g_pRYLCampRepairDlg->m_dwCampID, 
						0, 
						0,
						PktCampCmd::CAMP_REPAIR ) ;

					g_pRYLCampRepairDlg->SetVisible( FALSE ) ;
				}
				else
				{
					CRYLMessageBox* pMessageBox = new CRYLMessageBox ;
					pMessageBox->Create( CRYLStringTable::m_strString[ 3030 ] ) ;
				}
			}
		}
		break ;
	case RYLCAMPREPAIRDLG::CANCEL_BUTTON :
		{
			g_pRYLCampRepairDlg->SetVisible( FALSE ) ;
		}
		break ;
	}
}


VOID	
CRYLCampRepairDlg::ChangeDlg()
{
	if( m_bWorldWeapon )
	{
		m_pCampRepairMenuLabel->SetCaption( CRYLStringTable::m_strString[ 3057 ] ) ;	
		m_pCampRepairDescLabel2->SetCaption( CRYLStringTable::m_strString[ 3058 ] ) ;
	}
	else
	{
		m_pCampRepairMenuLabel->SetCaption( CRYLStringTable::m_strString[ 3052 ] ) ;
		m_pCampRepairDescLabel2->SetCaption( CRYLStringTable::m_strString[ 3053 ] ) ;
	}
}