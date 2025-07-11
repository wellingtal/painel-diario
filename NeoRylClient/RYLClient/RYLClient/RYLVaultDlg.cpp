#include <winsock2.h>
#include "RYLClientmain.h"
#include "RYLVaultDlg.h"
#include "BaseDataDefine.h"
#include "RYLEdit.h"
#include "RYLTabButton.h"
#include "RYLImageManager.h"
#include "RYLImage.h"

#include "RYLSprite.h"
#include "RYLSpriteList.h"
#include "GMImageDib.h"

#include "RYLGameData.h"
#include "RYLNetworkData.h"
#include "RYLStringTable.h"

#include "RYLChattingDlg.h"
#include "RYLMessageBox.h"
#include "RYLMessageBoxManager.h"
#include "..\\GUITooltipItem.h"

#include <WinInput.h>
#include <Item/ItemMgr.h>
#include <Item/ItemFactory.h>
#include <Network/ClientSocket/Send/SendItem.h>

#include "BaseGraphicsLayer.h"

#include "RYLVaultPasswordDlg.h"
#include "RYLGameScene.h"
#include "RYLSceneManager.h"
#include <RenderOption.h>
#include "SoundMgr.h"
#include "GMMemory.h"
CRYLVaultDlg*	g_pVaultDlg = NULL ;

CRYLVaultDlg::CRYLVaultDlg() 
{
	m_pTabButton	= NULL ;
	m_bLogined		= FALSE ;
	strcpy( m_TabTitle[ 0 ], "I" ) ;
	strcpy( m_TabTitle[ 1 ], "II" ) ;
	strcpy( m_TabTitle[ 2 ], "III" ) ;
	strcpy( m_TabTitle[ 3 ], "IV" ) ;
	
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


	CRYLSpriteList::Instance()->CreateGMImage( 256, 417, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 15, 256, 37 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	int i = 0;
	for (  ; i < 76 ; i ++ )
	{
		SetRect( &rt, 0, 38, 256, 43 ) ;
		pImageDIB->DrawRect( &rt, 0, 21 + ( i * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	}
	SetRect( &rt, 0, 42, 256, 52 ) ;
	pImageDIB->DrawRect( &rt, 0, 21 + ( ( i - 1 ) * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	
	for ( int ver = 0 ; ver < 12 ; ver ++ )
	{
		for ( int hor = 0 ; hor < 8 ; hor ++ )
		{
			pImageDIB->MakeFrameRect( 23 + ( hor * 26 ), 36 + (ver * 26 ), 27, 28, 0xff6e6857, 0xc0000000,  1 ) ;		
		}
	}
	pImageDIB->DrawFrameRect( 218, 0, 35, 21, 0xc0000000 ) ;
	pImageDIB->DrawFrameRect( 218, 20, 35, 1, 0xffa88a60 ) ;

	pImageDIB->DrawFrameRect( 23, 36, 209, 1, 0xffB99E6B ) ;
	pImageDIB->DrawFrameRect( 23, 36, 1, 313, 0xffB99E6B ) ;
	pImageDIB->DrawFrameRect( 23, 349, 209, 1, 0xffB99E6B ) ;
	pImageDIB->DrawFrameRect( 231, 36, 1, 313, 0xffB99E6B ) ;

	pImageDIB->MakeFrameRect( 47, 352, 133, 20, 0xffB99E6B, 0xc0000000,  1 ) ;		
	
	SetRect( &rt, 188, 97, 213, 117 ) ;
	pImageDIB->DrawRect( &rt, 23, 352, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;

	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	AttachWindowTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;
	
	SetWidth( 256 ) ;
	SetHeight( 408 ) ;
	SetLeft( ( CGMUIScreen::GetInstance()->GetWidth() - GetWidth() ) / 2 ) ;                                
	SetTop( ( CGMUIScreen::GetInstance()->GetHeight() - GetHeight() ) / 2 ) ;

	SetFormType( RYLDLG_FRAME_TYPE_CLOSEBUTTON ) ;

	g_pVaultDlg = this ;
	InitCtrl() ;
}

CRYLVaultDlg::~CRYLVaultDlg() 
{
	FinalCtrl() ;
}

VOID	CRYLVaultDlg::InitCtrl() 
{
	CGMImageDib* pSprite			= CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;
	
	m_pTabButton = new CRYLTabButton( this ) ;
	m_pTabButton->SetLeft( 1 ) ;
	m_pTabButton->SetTop( 15 ) ;
	m_pTabButton->SetWidth( 252 ) ;
	m_pTabButton->SetHeight( 31 ) ;
	m_pTabButton->SetInstance( RYLVAULTDLG::TAB_BUTTON ) ;
	m_pTabButton->OnClickEvent = DlgClick ;
	m_pTabButton->AttachTabButton( "I", 3 ) ;
	m_pTabButton->AttachTabButton( CRYLStringTable::m_strString[ 2203 ], 3 ) ;
	m_pTabButton->AttachTabButton( CRYLStringTable::m_strString[ 2203 ], 3 ) ;
	m_pTabButton->AttachTabButton( CRYLStringTable::m_strString[ 2203 ], 3 ) ;
	AttachGMUIControl( m_pTabButton ) ;
	
	// 입금
	m_pDepositGoldButton = new CRYLButton( this ) ;
	m_pDepositGoldButton->SetLeft( 18 ) ;
	m_pDepositGoldButton->SetTop( 391 ) ;
	m_pDepositGoldButton->SetCaption( CRYLStringTable::m_strString[ 2067 ] ) ;
	m_pDepositGoldButton->SetFontColor( 0xffffffff) ;
	m_pDepositGoldButton->SetColor( 0xffffffff) ;
	m_pDepositGoldButton->OnClickEvent = DlgClick ;
	m_pDepositGoldButton->SetInstance( RYLVAULTDLG::DEPOSITGOLD_BUTTON ) ;
	m_pDepositGoldButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pDepositGoldButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pDepositGoldButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pDepositGoldButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pDepositGoldButton->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 1892 ], 1892 ) ;
	AttachGMUIControl( m_pDepositGoldButton ) ;

	// 출금
	m_pPaymentGoldButton = new CRYLButton( this ) ;
	m_pPaymentGoldButton->SetLeft( 90 ) ;
	m_pPaymentGoldButton->SetTop( 391 ) ;
	m_pPaymentGoldButton->SetCaption( CRYLStringTable::m_strString[ 2066 ] ) ;
	m_pPaymentGoldButton->SetFontColor( 0xffffffff) ;
	m_pPaymentGoldButton->SetColor( 0xffffffff) ;
	m_pPaymentGoldButton->OnClickEvent = DlgClick ;
	m_pPaymentGoldButton->SetInstance( RYLVAULTDLG::PAYMENTGOLD_BUTTON ) ;
	m_pPaymentGoldButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pPaymentGoldButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pPaymentGoldButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pPaymentGoldButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pPaymentGoldButton->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 1893 ], 1893 ) ;
	AttachGMUIControl( m_pPaymentGoldButton ) ;

	// 페스워드 변경
	m_pPasswordChangeButton = new CRYLButton( this ) ;
	m_pPasswordChangeButton->SetLeft( 161 ) ;
	m_pPasswordChangeButton->SetTop( 391 ) ;
	m_pPasswordChangeButton->SetCaption( CRYLStringTable::m_strString[ 2204 ] ) ;
	m_pPasswordChangeButton->SetFontColor( 0xffffffff) ;
	m_pPasswordChangeButton->SetColor( 0xffffffff) ;
	m_pPasswordChangeButton->OnClickEvent = DlgClick ;
	m_pPasswordChangeButton->SetInstance( RYLVAULTDLG::PASSWORD_BUTTON ) ;
	m_pPasswordChangeButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 6 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pPasswordChangeButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 6 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pPasswordChangeButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 6 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pPasswordChangeButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 6 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pPasswordChangeButton->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 1894 ], 1894 ) ;
	AttachGMUIControl( m_pPasswordChangeButton ) ;

	m_pGoldLabel = new CRYLLabel( this ) ;
	m_pGoldLabel->SetLeft( 53 ) ;
	m_pGoldLabel->SetTop( 370 ) ;
	m_pGoldLabel->SetFontColor (0xffffffff);
	m_pGoldLabel->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pGoldLabel ) ;
}

VOID	CRYLVaultDlg::FinalCtrl() 
{
	GM_DELETE( m_pTabButton ) ;
	GM_DELETE( m_pPaymentGoldButton ) ;						// 출금
	GM_DELETE( m_pDepositGoldButton ) ;						// 입금
	GM_DELETE( m_pPasswordChangeButton	) ; 				// 페스워드 변경
	GM_DELETE( m_pGoldLabel ) ;
}

VOID	CRYLVaultDlg::Show( CGMUIObject* Sender ) 
{
	InitValue();
}

VOID	CRYLVaultDlg::Hide( CGMUIObject* Sender ) 
{
	if ( m_bLogined )
	{
		SendPacket::StoreLogout( g_GameSession ) ;
	}

	if ( CRYLChattingDlg::Instance() )
    {
        CRYLChattingDlg::Instance()->m_pChatEdit->SetFocus();
	    CRYLChattingDlg::Instance()->m_pChatEdit->SetFocusState( FALSE ) ;
    }
}

VOID	CRYLVaultDlg::InitValue() 
{
	m_dwVaultTab		= m_pTabButton->GetFocusTabButton() ;
	m_dwShowVault		= 0 ;
	m_cSavePass			= 0 ;
	m_dwBuyTabResult	= 0 ;
	m_lpSplitItem		= NULL ;
	m_dwSplitResult		= 0 ;
	m_dwSplitValue		= 0 ;
	m_dwVaultGoldResult = 0 ;
	m_dwInvenGoldResult = 0 ;
	m_dwGoldValue		= 0 ;
}

VOID	CRYLVaultDlg::GMUIPaint( INT xx, INT yy ) 
{
	LPDIRECT3DDEVICE8   lpD3DDevice = BaseGraphicsLayer::GetDevice() ;
	CRYLGameData* pGame = CRYLGameData::Instance() ;
	CRYLDialog::GMUIPaint( xx, yy ) ;

	if ( m_bLogined )
	{
		list<CItemInstance *>::iterator it;
		for(it = pGame->m_csStatus.m_lstDeposit.begin(); it != pGame->m_csStatus.m_lstDeposit.end(); it++)
		{
			if ((*it)->m_lpItemBase->GetPos().GetZIndex() != m_dwVaultTab)
			{
				continue;
			}

			(*it)->Render(lpD3DDevice);
		}

		if (pGame->m_lpPickItem && g_MessageBoxManager.m_lstMessageBox.empty())
		{
			if (g_DeviceInput.InRect( GetLeft() + 23, GetTop() + 52, GetLeft() + 231, GetTop() + 364) )
			{
				Item::ItemPos pItemIndex;
				POINT *ptMousePos = g_DeviceInput.GetMouseLocal();
				pItemIndex.SetPos((ptMousePos->x - ( GetLeft() + 23)) / 26, (ptMousePos->y - (GetTop() + 52)) / 26, 0);
				__int64 ddItem;

				if ((ddItem = pGame->m_csStatus.IsCheckItem( pGame->m_lpPickItem, TakeType::TS_DEPOSIT, pItemIndex.GetXIndex(), pItemIndex.GetYIndex(), m_dwVaultTab)) != ITEM_FAILED)
				{
					pGame->RenderRect(lpD3DDevice,
						GetLeft() + 23 + pItemIndex.GetXIndex() * 26,
						GetTop()  + 52 + pItemIndex.GetYIndex() * 26,
						GetLeft() + 23 + pGame->m_lpPickItem->GetItemInfo().m_DetailData.m_cXSize * 26 + pItemIndex.GetXIndex() * 26,
						GetTop()  + 52 + pGame->m_lpPickItem->GetItemInfo().m_DetailData.m_cYSize * 26 + pItemIndex.GetYIndex() * 26,
						0xFF0000FF, 0x80);
				} 
				else
				{
					if (pGame->m_lpPickItem->GetItemInfo().m_DetailData.m_cXSize + pItemIndex.GetXIndex() < 8 &&
						pGame->m_lpPickItem->GetItemInfo().m_DetailData.m_cYSize + pItemIndex.GetYIndex() < 12)
					{
						pGame->RenderRect(lpD3DDevice,
							GetLeft() + 23 + pItemIndex.GetXIndex() * 26,
							GetTop()  + 52 + pItemIndex.GetYIndex() * 26,
							GetLeft() + 23 + pGame->m_lpPickItem->GetItemInfo().m_DetailData.m_cXSize * 26 + pItemIndex.GetXIndex() * 26,
							GetTop()  + 52 + pGame->m_lpPickItem->GetItemInfo().m_DetailData.m_cYSize * 26 + pItemIndex.GetYIndex() * 26,
							0xFFFF0000, 0x80);
					}
				}
			}
		}
	}
}

HRESULT		CRYLVaultDlg::Render( LPDIRECT3DDEVICE8 lpD3DDevice )  
{
	return S_OK ;
}

HRESULT		CRYLVaultDlg::FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) 
{
	return S_OK ;
}

VOID		CRYLVaultDlg::EnableTabButton() 
{
	LPTABBUTTONINFO		lpTabInfo = NULL ;
	for ( unsigned long i = 0 ; i < CRYLGameData::Instance()->m_csStatus.m_dwMaxTab ; i ++ )
	{
		lpTabInfo = m_pTabButton->GetTabButtonInfo( i ) ;
		lpTabInfo->m_pTabButton->SetCaption( m_TabTitle[ i ], TRUE ) ;
	}

	m_dwVaultTab = CRYLGameData::Instance()->m_csStatus.m_dwMaxTab - 1;
}

HRESULT		CRYLVaultDlg::Update( BOOL &bClick, BOOL &bEdge ) 
{
	if ( !GetVisible() || g_MessageBoxManager.GetIsExistModal() ) return E_FAIL ;

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

	CRYLGameData* pGame			= CRYLGameData::Instance() ;
	CRYLNetworkData* pNetwork	= CRYLNetworkData::Instance() ;

	m_pGoldLabel->SetCaption( CRYLStringTable::MakePrintGold( pGame->m_csStatus.m_dwDepositGold ) ) ;
	
	// 입금
	if ( m_dwVaultGoldResult & MB_YES )
	{
		if ( pGame->m_csStatus.m_Info.Gold <= m_dwGoldValue )
		{
			m_dwGoldValue = pGame->m_csStatus.m_Info.Gold ;
		}

		g_pClientSocket->SetStatusFlag( NS_TAKEGOLD ) ;
		SendPacket::CharTakeGold( g_GameSession, pNetwork->m_dwMyChrID, TakeType::TS_INVEN, TakeType::TS_DEPOSIT, m_dwGoldValue ) ;

		m_dwVaultGoldResult		= 0 ;
		m_dwGoldValue			= 0 ;
		CRYLChattingDlg::Instance()->m_dwEditMode = RYLCHATMODE::RYL_EDITMODE_CHAT ;

		return TRUE ;
	} 
	else if ( m_dwVaultGoldResult & MB_NO || m_dwVaultGoldResult & MB_EXIT )
	{
		m_dwVaultGoldResult		= 0 ;
		m_dwGoldValue			= 0 ;
		CRYLChattingDlg::Instance()->m_dwEditMode = RYLCHATMODE::RYL_EDITMODE_CHAT ;

		return TRUE ;
	}

	// 출금
	if ( m_dwInvenGoldResult & MB_YES )
	{
		if ( pGame->m_csStatus.m_dwDepositGold <= m_dwGoldValue )
		{
			m_dwGoldValue = pGame->m_csStatus.m_dwDepositGold ;
		}

		if ( pGame->m_csStatus.m_Info.Gold > ULONG_MAX - m_dwGoldValue )
		{
			CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
			lpMessageBox->Create( CRYLStringTable::m_strString[ 219 ] ) ;
		}
        else
		{
			g_pClientSocket->SetStatusFlag( NS_TAKEGOLD ) ;
			SendPacket::CharTakeGold( g_GameSession, pNetwork->m_dwMyChrID, TakeType::TS_DEPOSIT, TakeType::TS_INVEN, m_dwGoldValue ) ;
		}

		m_dwInvenGoldResult		= 0 ;
		m_dwGoldValue			= 0 ;
		CRYLChattingDlg::Instance()->m_dwEditMode = RYLCHATMODE::RYL_EDITMODE_CHAT ;

		return TRUE ;
	} 
	else if ( m_dwInvenGoldResult & MB_NO || m_dwInvenGoldResult & MB_EXIT )
	{
		m_dwInvenGoldResult		= 0 ;
		m_dwGoldValue			= 0 ;
		CRYLChattingDlg::Instance()->m_dwEditMode = RYLCHATMODE::RYL_EDITMODE_CHAT ;

		return TRUE ;
	}

	if ( m_dwBuyTabResult & MB_YES )
	{
		// 산다. 패킷 전송
		SendPacket::StoreBuyTab( g_GameSession, pGame->m_csStatus.m_dwMaxTab ) ;

		m_dwBuyTabResult = 0 ;
		return TRUE ;
	} 
	else if ( m_dwBuyTabResult & MB_NO || m_dwBuyTabResult & MB_EXIT )
	{
		m_dwBuyTabResult = 0 ;
		return TRUE ;
	}

	// 나누기
	if ( m_dwSplitResult & MB_YES )
	{
		if ( m_dwSplitValue )
		{
			if ( m_dwSplitValue >= m_lpSplitItem->GetNowDurability() )
			{
				pGame->SetPickItem( m_lpSplitItem ) ;
				pGame->m_csStatus.DeleteDepositItem( m_lpSplitItem ) ;
			}
			else
			{
				pGame->m_lpPickItem = new CItemInstance ;
				pGame->m_lpPickItem->m_lpItemBase = Item::CItemFactory::GetInstance().CreateItem( m_lpSplitItem->GetProtoTypeID() ) ;
				pGame->m_lpPickItem->m_lpItemBase->MoveItem( m_lpSplitItem->m_lpItemBase->GetPos() ) ;
				pGame->m_lpPickItem->SetNowDurability( m_dwSplitValue ) ;

				pGame->m_lpPickItem->SetItemPos( TakeType::TS_DEPOSIT ) ;
				pGame->m_lpPickItem->m_dwItemUID = m_lpSplitItem->m_dwItemUID ;
				pGame->m_lpPickItem->m_lCoolDownTimeCount = m_lpSplitItem->m_lCoolDownTimeCount ;
				pGame->m_lpPickItem->m_lpSprite = new CRYLSprite ;
				pGame->m_lpPickItem->m_lpSprite->Create( 0, 0, 
															   pGame->m_lpPickItem->GetItemInfo().m_SpriteData.m_nSpriteMinX,
															   pGame->m_lpPickItem->GetItemInfo().m_SpriteData.m_nSpriteMinY,
															   pGame->m_lpPickItem->GetItemInfo().m_SpriteData.m_nSpriteMaxX,
															   pGame->m_lpPickItem->GetItemInfo().m_SpriteData.m_nSpriteMaxY, NULL ) ;
				pGame->m_lpPickItem->m_lpTooltip = new CGUITooltipItem ;
				((CGUITooltipItem *)pGame->m_lpPickItem->m_lpTooltip)->SetItem(pGame->m_lpPickItem);

				unsigned long dwSplitValue = m_lpSplitItem->GetNowDurability() - m_dwSplitValue ;
				m_lpSplitItem->SetNowDurability( dwSplitValue ) ;
			}
		}

		m_dwSplitResult			= 0 ;
		m_dwSplitValue			= 0 ;
		m_lpSplitItem			= NULL ;
		CRYLChattingDlg::Instance()->m_dwEditMode = RYLCHATMODE::RYL_EDITMODE_CHAT ;

		return TRUE ;
	} 
	else if ( m_dwSplitResult & MB_NO || m_dwSplitResult & MB_EXIT )
	{
		m_dwSplitResult			= 0 ;
		m_dwSplitValue			= 0 ;
		m_lpSplitItem			= NULL ;
		CRYLChattingDlg::Instance()->m_dwEditMode = RYLCHATMODE::RYL_EDITMODE_CHAT ;

		return TRUE ;
	}

	switch(m_dwShowVault)
	{
		case 0:
		{
			if (m_bLogined)
			{
				// 물건 셀렉트
				if (SelectItem()) 
				{
					return TRUE;
				}

				POINT *ptMousePos = g_DeviceInput.GetMouseLocal();
				unsigned long dwInvenX = ( ptMousePos->x - (GetLeft() + 23)) / 26;
				unsigned long dwInvenY = ( ptMousePos->y - (GetTop() + 52)) / 26;

				__int64 ddItem;

				if ( g_DeviceInput.GetIsLeftMouseDown() && pGame->m_lpPickItem && pGame->m_lpPickItem->GetItemPos() != TakeType::TS_QSLOT)
				{
					if (g_DeviceInput.InRect(GetLeft() + 23, GetTop() + 52, GetLeft() + 231, GetTop() + 364))
					{
						if ((ddItem = pGame->m_csStatus.IsCheckItem(pGame->m_lpPickItem, TakeType::TS_DEPOSIT, dwInvenX, dwInvenY, m_dwVaultTab)) != ITEM_FAILED)
						{
							if (false == pGame->m_lpPickItem->m_lpItemBase->GetItemInfo().m_DetailData.m_bExchangeAndDrop)
							{
								CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
								lpMessageBox->Create(CRYLStringTable::m_strString[ 3578 ]);
								return TRUE;
							}

							Item::ItemPos pItemIndex;
							pItemIndex.SetPos(dwInvenX, dwInvenY, m_dwVaultTab);
							pItemIndex.m_cPos = TakeType::TS_DEPOSIT;
							if (pGame->m_lpPickItem->m_lpItemBase->GetPos() == pItemIndex)
							{
								CItemInstance* lpExistItem = pGame->m_csStatus.GetDepositItem(pGame->m_lpPickItem->m_lpItemBase->GetPos());

								if (lpExistItem)
								{
									if (pGame->m_lpPickItem->GetIsEnableStack())
									{
										unsigned long dwDurability = lpExistItem->GetNowDurability() + pGame->m_lpPickItem->GetNowDurability();
										lpExistItem->SetNowDurability(dwDurability);
										pGame->m_csStatus.SetQuickSlot(lpExistItem);
										delete pGame->m_lpPickItem;
										pGame->SetPickItem(NULL);
									}
								} 
								else
								{
									pGame->m_csStatus.AddDepositItem(pGame->m_lpPickItem);
									pGame->SetPickItem(NULL);
								}

								return TRUE;
							}

							if (ddItem == ITEM_EMPTY)
							{
								if (pGame->m_lpPickItem->GetIsEnableStack())
								{
									CItemInstance *lpExistItem = pGame->m_csStatus.GetItem(pGame->m_lpPickItem->m_lpItemBase->GetPos());
									if (NULL != lpExistItem && lpExistItem->GetProtoTypeID() == pGame->m_lpPickItem->GetProtoTypeID())
									{
										// 기존의 자리에 아이템이 남아있으면 아이템이 나눠진걸로 생각, 아이템을 스택한다.
										pNetwork->SendSplitItem(pGame->m_lpPickItem, &pItemIndex, pGame->m_lpPickItem->GetNowDurability());
									} 
									else
									{
										pNetwork->SendTakeItem(pGame->m_lpPickItem, &pItemIndex, pGame->m_lpPickItem->GetNowDurability());
									}
								} 
								else
								{
									// 아이템 이동
									pNetwork->SendTakeItem(pGame->m_lpPickItem, &pItemIndex, 1);
								}
							} 
							else
							{
								CItemInstance* lpExistItem = pGame->m_csStatus.GetDepositItem(ddItem);

								if (pGame->m_lpPickItem->GetIsEnableStack() && lpExistItem &&
									(dwInvenX == lpExistItem->m_lpItemBase->GetPos().GetXIndex() &&
									dwInvenY == lpExistItem->m_lpItemBase->GetPos().GetYIndex()) &&
									lpExistItem->GetProtoTypeID() == pGame->m_lpPickItem->GetProtoTypeID() &&
									lpExistItem->GetNowDurability() < lpExistItem->GetMaxDurability())
								{
									// 아이템을 스택한다.
									unsigned char cNum = 0;
									if (lpExistItem->GetMaxDurability() - lpExistItem->GetNowDurability() < pGame->m_lpPickItem->GetNowDurability())
									{
										cNum = lpExistItem->GetMaxDurability() - lpExistItem->GetNowDurability();
									} 
									else
									{
										cNum = pGame->m_lpPickItem->GetNowDurability();
									}

									pNetwork->SendTakeItem(pGame->m_lpPickItem, &pItemIndex, cNum);
									return TRUE;
								}

								// 인벤토리 리스트에서 아이템을 찾는다.(완료)
								// 스왑하기
								CItemInstance* lpInvenItem = NULL;

								if (pGame->m_lpPickItem->GetIsEnableStack() && pGame->m_lpPickItem->GetItemPos() != TakeType::TS_TEMP)
								{
									lpInvenItem = pGame->m_csStatus.GetDepositItem(pGame->m_lpPickItem->m_lpItemBase->GetPos());
								}

								if (!lpInvenItem)
								{
									pNetwork->SendSwapItem(pGame->m_lpPickItem, &pItemIndex, lpExistItem);
								}
							}

							return TRUE;
						}
					}
				}
			}
			break;
		}
	}

	return S_OK ;
}

VOID		CRYLVaultDlg::PaymentGoldButtonEvent() 						// 출금
{
	CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
	lpMessageBox->Create(CRYLStringTable::m_strString[616], MB_NUMBER | MB_YES | MB_NO | MB_EXIT);
	lpMessageBox->SetResult(&m_dwInvenGoldResult);
	lpMessageBox->SetValue(&m_dwGoldValue);

	m_dwInvenGoldResult = 0;
	m_dwGoldValue = 0;
}

VOID		CRYLVaultDlg::DepositGoldButtonEvent() 						// 입금
{
	CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
	lpMessageBox->Create(CRYLStringTable::m_strString[615], MB_NUMBER | MB_YES | MB_NO | MB_EXIT);
	lpMessageBox->SetResult(&m_dwVaultGoldResult);
	lpMessageBox->SetValue(&m_dwGoldValue);
	
	m_dwVaultGoldResult = 0;
	m_dwGoldValue = 0;
}

VOID		CRYLVaultDlg::PasswordChangeButtonEvent()					// 패스워드 
{
	m_dwShowVault = 2;
	CRYLGameScene*			pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
	CRYLVaultPasswordDlg*	pVaultPasswordDlg = ( CRYLVaultPasswordDlg* )pScene->GetVaultPasswordDlg() ;
	pVaultPasswordDlg->SetVisible( TRUE ) ;
}

VOID		CRYLVaultDlg::VaultTabI() 
{
	CRYLGameData* pGame = CRYLGameData::Instance() ;
	
	if ( pGame->m_csStatus.m_dwMaxTab > 0 ) 
	{
		m_dwVaultTab = 0  ;
	}
}

VOID		CRYLVaultDlg::VaultTabII()
{
	CRYLGameData* pGame = CRYLGameData::Instance() ;

	if ( pGame->m_csStatus.m_dwMaxTab > 1 )
	{
		m_dwVaultTab = 1 ;
	}
	else
	{
		unsigned long dwGold[ 4 ] = { 0, 1000, 10000, 50000 };
		if ( pGame->m_csStatus.m_Info.Gold >= dwGold[ pGame->m_csStatus.m_dwMaxTab ] )
		{
			char strText[ MAX_PATH ] ;
			sprintf( strText, CRYLStringTable::m_strString[613], pGame->m_csStatus.m_dwMaxTab + 1, dwGold[pGame->m_csStatus.m_dwMaxTab] ) ;
			CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
			lpMessageBox->Create(strText, MB_YES | MB_NO | MB_EXIT);
			lpMessageBox->SetResult(&m_dwBuyTabResult);
		} 
		else
		{
			CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
			lpMessageBox->Create(CRYLStringTable::m_strString[614]);
		}
	}
}

VOID		CRYLVaultDlg::VaultTabIII()
{
	CRYLGameData* pGame = CRYLGameData::Instance() ;

	if (pGame->m_csStatus.m_dwMaxTab > 2)
	{
		m_dwVaultTab = 2;
	}
	else
	{
		unsigned long dwGold[4] = { 0, 1000, 10000, 50000 };
		if (pGame->m_csStatus.m_Info.Gold >= dwGold[pGame->m_csStatus.m_dwMaxTab])
		{
			char strText[MAX_PATH];
			sprintf(strText, CRYLStringTable::m_strString[613], pGame->m_csStatus.m_dwMaxTab + 1, dwGold[pGame->m_csStatus.m_dwMaxTab]);
			CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
			lpMessageBox->Create(strText, MB_YES | MB_NO | MB_EXIT);
			lpMessageBox->SetResult(&m_dwBuyTabResult);
		} 
		else
		{
			CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
			lpMessageBox->Create(CRYLStringTable::m_strString[614]);
		}
	}
}

VOID		CRYLVaultDlg::VaultTabIV() 
{
	CRYLGameData* pGame = CRYLGameData::Instance() ;

	if (pGame->m_csStatus.m_dwMaxTab > 3)
	{
		m_dwVaultTab = 3;
	}
	else
	{
		unsigned long dwGold[4] = { 0, 1000, 10000, 50000 };
		if (pGame->m_csStatus.m_Info.Gold >= dwGold[pGame->m_csStatus.m_dwMaxTab])
		{
			char strText[MAX_PATH];
			sprintf(strText, CRYLStringTable::m_strString[613], pGame->m_csStatus.m_dwMaxTab + 1, dwGold[pGame->m_csStatus.m_dwMaxTab]);
			CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
			lpMessageBox->Create(strText, MB_YES | MB_NO | MB_EXIT);
			lpMessageBox->SetResult(&m_dwBuyTabResult);
		} 
		else
		{
			CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
			lpMessageBox->Create(CRYLStringTable::m_strString[614]);
		}
	}
}

VOID	CRYLVaultDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y ) 
{
	if ( !g_pVaultDlg )
		return ;

	switch( Sender->GetInstance() )
	{
		case RYLVAULTDLG::TAB_BUTTON :
		{
			INT		iFocus = g_pVaultDlg->GetTabButton()->GetFocusTabButton() ;
			switch( iFocus )
			{
				case 0 : 			g_pVaultDlg->VaultTabI() ;			break ;
				case 1 :			g_pVaultDlg->VaultTabII() ;			break ;
				case 2 :			g_pVaultDlg->VaultTabIII() ;		break ;
				case 3 :			g_pVaultDlg->VaultTabIV() ;			break ;
			}
			break ;
		}

		case RYLVAULTDLG::PAYMENTGOLD_BUTTON :			// 출금
			g_pVaultDlg->PaymentGoldButtonEvent() ;
			break ;

		case RYLVAULTDLG::DEPOSITGOLD_BUTTON :			// 입금
			g_pVaultDlg->DepositGoldButtonEvent() ;
			break ;

		case RYLVAULTDLG::PASSWORD_BUTTON	 :			// 패스워드 변경
			g_pVaultDlg->PasswordChangeButtonEvent() ;
			break ;
	}
}

VOID	CRYLVaultDlg::InitVault()
{
	CRYLGameScene*	pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;

	if ( CRYLGameData::Instance()->m_csStatus.m_cUsedDeposit)
	{
		m_dwShowVault = 1;

		CRYLVaultCertificationdDlg*	pVaultCertificationdDlg = ( CRYLVaultCertificationdDlg* )pScene->GetVaultCertificationdDlg() ;
		pVaultCertificationdDlg->SetVisible( TRUE ) ;
	
	} 
	else
	{
		m_dwShowVault = 2 ;

		CRYLVaultPasswordDlg*	pVaultPasswordDlg = ( CRYLVaultPasswordDlg* )pScene->GetVaultPasswordDlg() ;
		pVaultPasswordDlg->SetVisible( TRUE ) ;

		CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
		lpMessageBox->Create( CRYLStringTable::m_strString[618], MB_CONFIRM | MB_EXIT ) ;
	}
}

VOID	CRYLVaultDlg::ShowNormal(void)
{
	m_dwShowVault = 0;
}

BOOL CRYLVaultDlg::SelectItem()
{
	BOOL  bPickItem			= FALSE ;
	const Item::ItemInfo*	lpItemInfo ;
	CRYLGameData* pGame		= CRYLGameData::Instance() ;

	// 창고 아이템 업데이트 및 선택
	for(list<CItemInstance *>::iterator it = pGame->m_csStatus.m_lstDeposit.begin(); 
		it != pGame->m_csStatus.m_lstDeposit.end();)
	{
		lpItemInfo = &(*it)->GetItemInfo();

		if ((*it)->m_lpItemBase->GetPos().GetZIndex() != m_dwVaultTab)
		{
			++it;
			continue;
		}

		(*it)->m_wPutX = GetLeft() + 23 + 26 * (*it)->m_lpItemBase->GetPos().GetXIndex() + 
			(((lpItemInfo->m_DetailData.m_cXSize * 26 - 1) - (*it)->m_lpSprite->GetSizeX()) / 2);
		(*it)->m_wPutY = GetTop() + 52 + 26 * (*it)->m_lpItemBase->GetPos().GetYIndex() + 
			(((lpItemInfo->m_DetailData.m_cYSize * 26 - 1) - (*it)->m_lpSprite->GetSizeY()) / 2);
		
		if (!pGame->m_lpPickItem && g_DeviceInput.GetIsLeftMouseDown() && (*it)->IsOverMouse())
		{
			if ((*it)->GetIsEnableStack() && g_DeviceInput.KeyHold(VK_SHIFT) && (*it)->GetNowDurability() > 1)
			{
				CRYLMessageBox *lpMessageBox;
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create( CRYLStringTable::m_strString[170], MB_NUMBER | MB_YES | MB_NO | MB_EXIT);
				lpMessageBox->SetResult(&m_dwSplitResult);
				lpMessageBox->SetValue(&m_dwSplitValue);

				m_lpSplitItem = (*it);
				m_dwSplitResult = 0;
				m_dwSplitValue = 0;

				char Temp[MAX_PATH]; 
				sprintf(Temp, "%s\\Sound\\Interface\\click.mnd", g_ClientMain.m_strClientPath);
				CSound* pSound; 
				CSoundMgr::_GetInstance()->GetSound( pSound, Temp ); 
				pSound->Play(); 

				++it;
			} 
            // 아이템을 사용중이면 집을 수 없다.
            else if (!ClientSocket::IsNSFlagChainChecked(CmdCharUseItem))
			{                
				pGame->SetPickItem((*it));
				pGame->m_csStatus.CheckDepositField(CCharStatus::REMOVE_FIELD, pGame->m_lpPickItem);
				it = pGame->m_csStatus.m_lstDeposit.erase(it);

				bPickItem = TRUE;
			}
            else
            {
                ++it;
            }
		} 
		else
		{
			++it;
		}
	}

	if (bPickItem)
	{
		return TRUE;
	}

	return FALSE;
}