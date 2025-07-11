
#include <winsock2.h>

#include <Item/ItemMgr.h>
#include <Item/ItemFactory.h>
#include <Network/ClientSocket/Send/SendItem.h>

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

#include "RYLArmsCreatmenuDlg.h"
#include "GMMemory.h"

CRYLArmsCreatMenuDlg*	g_pRYLArmsCreatmenuDlg = NULL ;

CRYLArmsCreatMenuDlg::CRYLArmsCreatMenuDlg() 
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
	pImageDIB->MakeFrameRect( 25, 78, 55, 55, 0xff946b42, 0xc0000000,  1 ) ;	

	pImageDIB->DrawFrameRect( 26, 105, 52, 1, 0xff675e46 ) ;
	pImageDIB->DrawFrameRect( 51, 79, 1, 53, 0xff675e46 ) ;
	
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
	
	g_pRYLArmsCreatmenuDlg = this ;
}

CRYLArmsCreatMenuDlg::~CRYLArmsCreatMenuDlg() 
{
	FinalCtrl() ;
}

VOID	CRYLArmsCreatMenuDlg::InitCtrl() 
{
	CGMImageDib* pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;

	// 병기 생성 메뉴
	m_pArmsCreatMenuLabel = new CRYLLabel( this ) ;
	m_pArmsCreatMenuLabel->SetLeft( 12 ) ;
	m_pArmsCreatMenuLabel->SetTop( 18 ) ;
	m_pArmsCreatMenuLabel->SetFontColor (0xffffffff);
	m_pArmsCreatMenuLabel->SetAutoSize( TRUE ) ;
	m_pArmsCreatMenuLabel->SetCaption( CRYLStringTable::m_strString[ 2807 ] ) ;
	AttachGMUIControl( m_pArmsCreatMenuLabel ) ;

	// [자제]
	m_pArticleLabel = new CRYLLabel( this ) ;
	m_pArticleLabel->SetLeft( 32 ) ;
	m_pArticleLabel->SetTop( 72 ) ;
	m_pArticleLabel->SetFontColor (0xffffffff);
	m_pArticleLabel->SetAutoSize( TRUE ) ;
	m_pArticleLabel->SetCaption( CRYLStringTable::m_strString[ 2691 ] ) ;
	AttachGMUIControl( m_pArticleLabel ) ;

	// 병기 생성 설명 1
	m_pArmsCreatExpla1Label = new CRYLLabel( this ) ;
	m_pArmsCreatExpla1Label->SetLeft( 102 ) ;
	m_pArmsCreatExpla1Label->SetTop( 88 ) ;
	m_pArmsCreatExpla1Label->SetFontColor (0xffffffff);
	m_pArmsCreatExpla1Label->SetAutoSize( TRUE ) ;
	m_pArmsCreatExpla1Label->SetCaption( CRYLStringTable::m_strString[ 2808 ] ) ;
	AttachGMUIControl( m_pArmsCreatExpla1Label ) ;

	// 병기 생성 설명 2
	m_pArmsCreatExpla2Label = new CRYLLabel( this ) ;
	m_pArmsCreatExpla2Label->SetLeft( 102 ) ;
	m_pArmsCreatExpla2Label->SetTop( 104 ) ;
	m_pArmsCreatExpla2Label->SetFontColor (0xffffffff);
	m_pArmsCreatExpla2Label->SetAutoSize( TRUE ) ;
	m_pArmsCreatExpla2Label->SetCaption( CRYLStringTable::m_strString[ 2809 ] ) ;
	AttachGMUIControl( m_pArmsCreatExpla2Label ) ;

	// 병기 생성 설명 5
	m_pArmsCreatExpla3Label = new CRYLLabel( this ) ;
	m_pArmsCreatExpla3Label->SetLeft( 12 ) ;
	m_pArmsCreatExpla3Label->SetTop( 48 ) ;
	m_pArmsCreatExpla3Label->SetFontColor (0xffffffff);
	m_pArmsCreatExpla3Label->SetAutoSize( TRUE ) ;
	m_pArmsCreatExpla3Label->SetCaption( CRYLStringTable::m_strString[ 2696 ] ) ;
	AttachGMUIControl( m_pArmsCreatExpla3Label ) ;

	pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;
	// 확인 버튼
	m_pOKButton = new CRYLButton( this ) ;
	m_pOKButton->SetLeft( 99 ) ;
	m_pOKButton->SetTop( 159 ) ;
	m_pOKButton->SetCaption( CRYLStringTable::m_strString[ 2164 ] ) ;
	m_pOKButton->SetFontColor( 0xffffffff) ;
	m_pOKButton->SetColor( 0xffffffff) ;
	m_pOKButton->OnClickEvent = DlgClick ;
	m_pOKButton->SetInstance( RYLARMSCREATMENUDLG::OK_BUTTON ) ;
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
	m_pCancelButton->SetInstance( RYLARMSCREATMENUDLG::CANCEL_BUTTON ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pCancelButton ) ;
}

VOID	CRYLArmsCreatMenuDlg::FinalCtrl() 
{
	GM_DELETE( m_pArmsCreatMenuLabel ) ;			// 병기 생성 메뉴
	GM_DELETE( m_pArticleLabel ) ;					// [자제]
	GM_DELETE( m_pArmsCreatExpla1Label ) ;			// 병기 생성 설명 1
	GM_DELETE( m_pArmsCreatExpla2Label ) ;			// 병기 생성 설명 2
	GM_DELETE( m_pArmsCreatExpla3Label ) ;			// 병기 생성 설명 3
	GM_DELETE( m_pOKButton ) ;
	GM_DELETE( m_pCancelButton ) ;
}

VOID	CRYLArmsCreatMenuDlg::InitValue() 
{
}

VOID	CRYLArmsCreatMenuDlg::Show( CGMUIObject* Sender ) 
{
}

VOID	CRYLArmsCreatMenuDlg::Hide( CGMUIObject* Sender ) 
{
	CRYLGameData::Instance()->m_lpStartKit = NULL ;
}

VOID	CRYLArmsCreatMenuDlg::GMUIPaint( INT xx, INT yy ) 
{
	LPDIRECT3DDEVICE8   lpD3DDevice = BaseGraphicsLayer::GetDevice() ;
	CRYLDialog::GMUIPaint( xx, yy ) ;

	CRYLGameData* pGame = CRYLGameData::Instance() ;

	if ( pGame->m_csStatus.m_lpSiegeArmsMaterial )
	{
		pGame->m_csStatus.m_lpSiegeArmsMaterial->m_wPutX = GetLeft() + 6 +  ( pGame->m_csStatus.m_lpSiegeArmsMaterial->m_lpSprite->GetSizeX() / 2 ) ;
		pGame->m_csStatus.m_lpSiegeArmsMaterial->m_wPutY = GetTop()  + 78 + ( pGame->m_csStatus.m_lpSiegeArmsMaterial->m_lpSprite->GetSizeY() / 2 ) ;
		pGame->m_csStatus.m_lpSiegeArmsMaterial->Render( lpD3DDevice ) ;
	}
	
	if ( pGame->m_lpPickItem )
	{
		if(g_DeviceInput.InRect( GetLeft() + 25, GetTop() + 93, GetLeft() + 78, GetTop() + 147 ) )
		{
			pGame->RenderRect( lpD3DDevice, GetLeft() + 25, GetTop() + 93, GetLeft() + 78, GetTop() + 147, 0xFF0000FF, 0x80 ) ;
		} 
	}
}

HRESULT		CRYLArmsCreatMenuDlg::Render( LPDIRECT3DDEVICE8 lpD3DDevice )  
{
	return S_OK ;
}

HRESULT		CRYLArmsCreatMenuDlg::FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) 
{
	return S_OK ;
}

HRESULT		CRYLArmsCreatMenuDlg::Update( BOOL &bClick, BOOL &bEdge ) 
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

	CRYLGameData* pGame = CRYLGameData::Instance() ;
	CRYLNetworkData* pNetwork = CRYLNetworkData::Instance() ;
	POINT *ptMousePos = g_DeviceInput.GetMouseLocal() ;
	unsigned long dwMousePosX = ptMousePos->x - ( GetLeft() + 25 ) ;
	unsigned long dwMousePosY = ptMousePos->y - ( GetTop() + 78 ) ;
	unsigned long dwWindowX = dwMousePosX / 26 ;
	unsigned long dwWindowY = dwMousePosY / 26 ;

	if(g_DeviceInput.InRect( GetLeft() + 25, GetTop() + 93, GetLeft() + 78, GetTop() + 147 ) )
	{
		if(g_DeviceInput.GetIsLeftMouseDown())
		{
			if(pGame->m_lpPickItem)	// 손에 있을시
			{
				/*
				if ( pGame->m_lpPickItem->GetItemType() == Item::ItemType::SHORT_RANGE_ARMS_KIT ||
					 pGame->m_lpPickItem->GetItemType() == Item::ItemType::LONG_RANGE_ARMS_KIT || 
					 pGame->m_lpPickItem->GetItemType() == Item::ItemType::AIRSHIP_KIT )
				*/
				if ( pGame->m_lpPickItem->GetItemType() == Item::ItemType::ETC )
				{
					if(pGame->m_csStatus.m_lpSiegeArmsMaterial)
					{
						if(pGame->m_lpPickItem->GetProtoTypeID() == pGame->m_csStatus.m_lpSiegeArmsMaterial->GetProtoTypeID())
						{
							unsigned char cNum;
							if(pGame->m_csStatus.m_lpSiegeArmsMaterial->GetMaxDurability() - pGame->m_csStatus.m_lpSiegeArmsMaterial->GetNowDurability() < pGame->m_lpPickItem->GetNowDurability())
							{
								cNum = pGame->m_csStatus.m_lpSiegeArmsMaterial->GetMaxDurability() - pGame->m_csStatus.m_lpSiegeArmsMaterial->GetNowDurability();
							} 
							else
							{
								cNum = pGame->m_lpPickItem->GetNowDurability();
							}
							pNetwork->SendTakeItem(pGame->m_lpPickItem, &pGame->m_csStatus.m_lpSiegeArmsMaterial->m_lpItemBase->GetPos(), cNum);

							return S_OK;
						} 
						else	// 다른 광물
						{
							Item::ItemPos pItemIndex;
							pItemIndex.m_cIndex = Item::ExtraSpacePos::KIT_MATERIAL_POS ;
							pItemIndex.m_cPos = TakeType::TS_EXTRA;

							pNetwork->SendSwapItem(pGame->m_lpPickItem, &pItemIndex, pGame->m_csStatus.m_lpSiegeArmsMaterial);

							return S_OK;
						}
					} 
					else
					{
						CItemInstance *lpExistItem = pGame->m_csStatus.GetInventoryItem(pGame->m_lpPickItem->m_lpItemBase->GetPos());

						Item::ItemPos pItemIndex;
						pItemIndex.m_cIndex = Item::ExtraSpacePos::KIT_MATERIAL_POS;
						pItemIndex.m_cPos = TakeType::TS_EXTRA;

						if(pGame->m_lpPickItem->m_lpItemBase->GetPos() == pItemIndex)
						{
							pGame->m_csStatus.m_lpSiegeArmsMaterial = pGame->m_lpPickItem;
							pGame->SetPickItem(NULL);

							return S_OK;
						}											

						if(lpExistItem)
						{
							pNetwork->SendSplitItem(pGame->m_lpPickItem, &pItemIndex, pGame->m_lpPickItem->GetNowDurability());
						} 
						else
						{
							pNetwork->SendTakeItem(pGame->m_lpPickItem, &pItemIndex, pGame->m_lpPickItem->GetNowDurability());
						}

						return S_OK;
					}
				}
			} 
			else
			{
				// 광물을 줍는다.
				if(pGame->m_dwKindCursor == CURSOR_NORMAL && pGame->m_csStatus.m_lpSiegeArmsMaterial)
				{
					pGame->SetPickItem(pGame->m_csStatus.m_lpSiegeArmsMaterial);
					pGame->m_csStatus.m_lpSiegeArmsMaterial = NULL;
				}
			}
		}
	}

	return S_OK ;
}

VOID	CRYLArmsCreatMenuDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y ) 
{
	if ( !g_pRYLArmsCreatmenuDlg )
		return ;

	switch( Sender->GetInstance() )
	{
		case RYLARMSCREATMENUDLG::OK_BUTTON :
			{
				CItemInstance *lpItem = CRYLGameData::Instance()->m_lpStartKit ;

				if(!lpItem)
					return ;

				Item::CUseItem *lpStartKit = Item::CUseItem::DowncastToUseItem(lpItem->m_lpItemBase);
				if ( !lpStartKit ) 
					return ;

				CRYLGameData::SetAutoRun(FALSE);

				CRYLNetworkData* pNetwork = CRYLNetworkData::Instance() ;
				g_pClientSocket->SetStatusFlag(NS_USEITEM);
				SendPacket::CharUseItem(g_GameSession, pNetwork->m_dwMyChrID, pNetwork->m_dwMyChrID, &lpStartKit->GetPos());
				g_pRYLArmsCreatmenuDlg->SetVisible( FALSE ) ;
			}
			break ;
		case RYLARMSCREATMENUDLG::CANCEL_BUTTON :
			{
				g_pRYLArmsCreatmenuDlg->SetVisible( FALSE ) ;
				CRYLGameData::Instance()->m_lpStartKit = NULL ;
			}
			break ;
	}
}

VOID	CRYLArmsCreatMenuDlg::DlgKeyDown( CGMUIObject* Sender, WORD key, INT Shift ) 
{
}