
#include <winsock2.h>

#include "NPC/NPCList.h"
#include <Item/ItemMgr.h>

#include <Network/ClientSocket/Send/SendItem.h>
#include <Network/ClientSocket/Send/SendCommunity.h>
#include <Network/Packet/PacketStruct/CharCommunityPacket.h>

#include "..\\GUITooltipItem.h"
#include "..\\StringFilter.h"

#include "WinInput.h"
#include "RenderOption.h"
#include "BaseGraphicsLayer.h"
#include "BaseDataDefine.h"

#include "GMUIObject.h"
#include "GMImageDib.h"

#include "RYLListBox.h"
#include "RYLLabel.h"
#include "RYLEdit.h"
#include "RYLImageManager.h"
#include "RYLImage.h"
#include "RYLSpriteList.h"
#include "RYLStringTable.h"
#include "RYLMessageBox.h"
#include "RYLMessageBoxManager.h"

#include "RYLGameData.h"
#include "RYLNetworkData.h"

#include "RYLGameScene.h"
#include "RYLSceneManager.h"

#include "RYLHumanInventoryPanel.h"
#include "RYLSkillPanel.h"
#include "RYLChattingDlg.h"
#include "RYLStreetStallDlg.h"
#include "GMMemory.h"

CRYLStreetStallDlg*	g_pStreetStallDlg = NULL ;

CRYLStreetStallDlg::CRYLStreetStallDlg() 
{
	m_pStreetStallLabel			= NULL ;	
	m_pStorekeeperLabel			= NULL ;		
	m_pStorekeeperNameLabel		= NULL ; 
	m_pCurrentVisitor			= NULL ;			

	m_dwNameResult				= 0 ;
	m_dwRegistItemResult		= 0 ;
	m_dwRegistItemValue			= 0 ;
	m_dwBuyResult				= 0 ;
	m_lpBuyItem					= NULL ;
	m_dwItemNumber				= 0 ;
	strcpy(m_strMarketName, "");
	strcpy(m_strOwner, "");

	RECT rt ;
	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;

	CRYLSpriteList::Instance()->CreateGMImage( 380, 15, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 0, 256, 15 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_STREET_STALL01_256x256 ) ) ;
	SetRect( &rt, 0, 0, 124, 15 ) ;
	pImageDIB->DrawRect( &rt, 256, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_STREET_STALL02_256x256 ) ) ;
	
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	AttachCaptionTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;


	CRYLSpriteList::Instance()->CreateGMImage( 380, 398, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 15, 256, 256 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_STREET_STALL01_256x256 ) ) ;
	SetRect( &rt, 0, 15, 124, 256 ) ;
	pImageDIB->DrawRect( &rt, 256, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_STREET_STALL02_256x256 ) ) ;
	SetRect( &rt, 0, 0, 256, 157 ) ;
	pImageDIB->DrawRect( &rt, 0, 241, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_STREET_STALL03_256x256 ) ) ;
	SetRect( &rt, 124, 0, 248, 157 ) ;
	pImageDIB->DrawRect( &rt, 256, 241, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_STREET_STALL02_256x256 ) ) ;
	
	pImageDIB->MakeFrameRect( 8, 370, 364, 1, 0xffB99E6B, 0xc0000000,  1 ) ;		

	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	AttachWindowTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;
	
	SetWidth( 380 ) ;
	SetHeight( 413 ) ;
	SetLeft( ( CGMUIScreen::GetInstance()->GetWidth() - GetWidth() ) / 2 ) ;                                
	SetTop( ( CGMUIScreen::GetInstance()->GetHeight() - GetHeight() ) / 2 ) ;
	
	SetFormType( RYLDLG_TYPE_CLOSEBUTTON ) ;

	InitCtrl() ;
	g_pStreetStallDlg = this ;    
}

CRYLStreetStallDlg::~CRYLStreetStallDlg() 
{
	FinalCtrl() ;
	DeleteTradeItem() ;
}

VOID	CRYLStreetStallDlg::InitValue()
{
	m_dwNameResult			= 0 ;
	m_dwRegistItemResult	= 0 ;
	m_dwRegistItemValue		= 0 ;
	m_dwBuyResult			= 0 ;
	m_lpBuyItem				= NULL ;
	m_dwItemNumber			= 0 ;
}

VOID	CRYLStreetStallDlg::InitCtrl() 
{
	RECT			rtRect ;
	CGMImageDib*	pSprite	= CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ;

	m_pStreetStallLabel = new CRYLLabel( this ) ;
	m_pStreetStallLabel->SetLeft( 13 ) ;	
	m_pStreetStallLabel->SetTop( 20 ) ;
	m_pStreetStallLabel->SetFontColor (0xffffffff);
	m_pStreetStallLabel->SetAutoSize( TRUE ) ;
	m_pStreetStallLabel->SetCaption( CRYLStringTable::m_strString[ 2210 ] ) ;
	AttachGMUIControl( m_pStreetStallLabel ) ;

	m_pStorekeeperLabel = new CRYLLabel( this ) ;
	m_pStorekeeperLabel->SetLeft( 193 ) ;	
	m_pStorekeeperLabel->SetTop( 46 ) ;
	m_pStorekeeperLabel->SetFontColor (0xffffffff);
	m_pStorekeeperLabel->SetAutoSize( TRUE ) ;
	m_pStorekeeperLabel->SetCaption( CRYLStringTable::m_strString[ 2211 ] ) ;
	AttachGMUIControl( m_pStorekeeperLabel ) ;

	m_pStorekeeperNameLabel = new CRYLLabel( this ) ;
	m_pStorekeeperNameLabel->SetLeft( 248 ) ;	
	m_pStorekeeperNameLabel->SetTop( 46 ) ;
	m_pStorekeeperNameLabel->SetFontColor (0xffffffff);
	m_pStorekeeperNameLabel->SetAutoSize( TRUE ) ;
	m_pStorekeeperNameLabel->SetCaption( CRYLStringTable::m_strString[ 2211 ] ) ;
	AttachGMUIControl( m_pStorekeeperNameLabel ) ;

	m_pCurrentVisitor = new CRYLLabel( this ) ;
	m_pCurrentVisitor->SetLeft( 26 ) ;	
	m_pCurrentVisitor->SetTop( 74 ) ;
	m_pCurrentVisitor->SetFontColor (0xffffffff);
	m_pCurrentVisitor->SetAutoSize( TRUE ) ;
	m_pCurrentVisitor->SetCaption( CRYLStringTable::m_strString[ 2212 ] ) ;
	AttachGMUIControl( m_pCurrentVisitor ) ;

	// 입장객 리스트 박스
	m_pVisitorListBox = new CRYLListBox( this, TRUE ) ;
	m_pVisitorListBox->SetLeft( 10 ) ;
	m_pVisitorListBox->SetTop( 97 ) ;
	m_pVisitorListBox->SetWidth( 94 ) ;
	m_pVisitorListBox->SetHeight( 173 ) ;
	
	SetRect( &rtRect, 198, 186, 212, 200 ) ;
	m_pVisitorListBox->AttachTopButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Normal ) ;
	SetRect( &rtRect, 226, 186, 240, 200 ) ;
	m_pVisitorListBox->AttachTopButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Down ) ;
	SetRect( &rtRect, 212, 186, 226, 200 ) ;
	m_pVisitorListBox->AttachTopButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Up ) ;
	SetRect( &rtRect, 198, 186, 212, 200 ) ;
	m_pVisitorListBox->AttachTopButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Disible ) ;

	SetRect( &rtRect, 235, 164, 243, 184 ) ;
 	m_pVisitorListBox->AttachTrackButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Normal ) ;
	SetRect( &rtRect, 235, 164, 243, 184 ) ;					 
	m_pVisitorListBox->AttachTrackButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Down ) ;
	SetRect( &rtRect, 235, 164, 243, 184 ) ;					 
	m_pVisitorListBox->AttachTrackButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Up ) ;
	SetRect( &rtRect, 235, 164, 243, 184 ) ;					 
	m_pVisitorListBox->AttachTrackButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Disible ) ;

	SetRect( &rtRect, 198, 200, 212, 214 ) ;
	m_pVisitorListBox->AttachBottomButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Normal ) ;
	SetRect( &rtRect, 226, 200, 240, 214 ) ;					  
	m_pVisitorListBox->AttachBottomButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Down ) ;
	SetRect( &rtRect, 212, 200, 226, 214 ) ;					  
	m_pVisitorListBox->AttachBottomButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Up ) ;
	SetRect( &rtRect, 198, 200, 212, 214 ) ;					  
	m_pVisitorListBox->AttachBottomButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Disible ) ;
 
	m_pVisitorListBox->SetColor( 0xffffffff ) ;
	AttachGMUIControl( m_pVisitorListBox ) ;

	// 입장객 채팅 리스트 박스
	m_pChattingListBox = new CRYLListBox( this, TRUE ) ;
	m_pChattingListBox->SetLeft( 13 ) ;
	m_pChattingListBox->SetTop( 284 ) ;
	m_pChattingListBox->SetWidth( 349 ) ;
	m_pChattingListBox->SetHeight( 100 ) ;

	SetRect( &rtRect, 198, 186, 212, 200 ) ;
	m_pChattingListBox->AttachTopButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Normal ) ;
	SetRect( &rtRect, 226, 186, 240, 200 ) ;
	m_pChattingListBox->AttachTopButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Down ) ;
	SetRect( &rtRect, 212, 186, 226, 200 ) ;
	m_pChattingListBox->AttachTopButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Up ) ;
	SetRect( &rtRect, 198, 186, 212, 200 ) ;
	m_pChattingListBox->AttachTopButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Disible ) ;

	SetRect( &rtRect, 235, 164, 243, 184 ) ;
 	m_pChattingListBox->AttachTrackButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Normal ) ;
	SetRect( &rtRect, 235, 164, 243, 184 ) ;					 
	m_pChattingListBox->AttachTrackButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Down ) ;
	SetRect( &rtRect, 235, 164, 243, 184 ) ;					 
	m_pChattingListBox->AttachTrackButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Up ) ;
	SetRect( &rtRect, 235, 164, 243, 184 ) ;					 
	m_pChattingListBox->AttachTrackButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Disible ) ;

	SetRect( &rtRect, 198, 200, 212, 214 ) ;
	m_pChattingListBox->AttachBottomButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Normal ) ;
	SetRect( &rtRect, 226, 200, 240, 214 ) ;					  
	m_pChattingListBox->AttachBottomButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Down ) ;
	SetRect( &rtRect, 212, 200, 226, 214 ) ;					  
	m_pChattingListBox->AttachBottomButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Up ) ;
	SetRect( &rtRect, 198, 200, 212, 214 ) ;					  
	m_pChattingListBox->AttachBottomButtonImage( pSprite, &rtRect, CGMUIListBox::ButtonImageStateType_Disible ) ;
	
	m_pChattingListBox->SetColor( 0xffffffff ) ;
	AttachGMUIControl( m_pChattingListBox ) ;

	m_pChattingEdit = new CRYLEdit( this ) ;
	m_pChattingEdit->SetLeft( 10 ) ;
	m_pChattingEdit->SetTop( 387 ) ;
	m_pChattingEdit->SetWidth( 360 ) ;
	m_pChattingEdit->SetHeight( 15 ) ;
	m_pChattingEdit->SetColor( 0xff000000) ;
	m_pChattingEdit->SetFontColor( 0xffffffff ) ;
	m_pChattingEdit->SetInstance( RYLSTREETSTALLDLG::CHATTING_EDIT ) ;
	m_pChattingEdit->SetMaxLength( 48 ) ;
	m_pChattingEdit->OnKeyDown = DlgKeyDown; 
	AttachGMUIControl( m_pChattingEdit ) ;

	InitValue() ;
}

VOID	CRYLStreetStallDlg::FinalCtrl() 
{
	GM_DELETE( m_pStreetStallLabel ) ;	
	GM_DELETE( m_pStorekeeperLabel ) ;	
	GM_DELETE( m_pStorekeeperNameLabel ) ; 
	GM_DELETE( m_pCurrentVisitor ) ;		
	GM_DELETE( m_pVisitorListBox ) ;				
	GM_DELETE( m_pChattingListBox ) ;
	GM_DELETE( m_pChattingEdit ) ;
}


VOID	CRYLStreetStallDlg::Show( CGMUIObject* Sender ) 
{
	m_pChattingEdit->SetText( "" ) ;

	InitValue() ;
}

VOID	CRYLStreetStallDlg::Hide( CGMUIObject* Sender ) 
{
	m_pChattingEdit->SetText( "" ) ;

	CRYLGameData* pGame = CRYLGameData::Instance() ;
	pGame->m_bRYLStreetStallDlgClick = FALSE ;

	if(pGame->m_bMovieVersion)
	{
		return;
	}
	
	if (m_dwOwnerID == CRYLNetworkData::Instance()->m_dwMyChrID)
	{
		if ( strcmp( m_strOwner, "" ) )
		{
			SendPacket::CharStallOpen(g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, "");
		}
	} 
	else
	{
			CRYLNetworkData::SendStallLeave();
	}

	strcpy(m_strMarketName, "");
	strcpy(m_strOwner, "");
	m_dwOwnerID = 0;
	DeleteTradeItem();

	m_pChattingListBox->ClearItem() ;		// 입장객 채팅 리스트 박스
	m_pVisitorListBox->ClearItem() ;				// 입장객 리스트 박스

    // set focus to chatting dialog
	if ( CRYLChattingDlg::Instance() )
    {
        CRYLChattingDlg::Instance()->m_pChatEdit->SetFocus();
	    CRYLChattingDlg::Instance()->m_pChatEdit->SetFocusState( FALSE ) ;
    }
	
}

VOID	CRYLStreetStallDlg::GMUIPaint( INT xx, INT yy ) 
{
	LPDIRECT3DDEVICE8   lpD3DDevice = BaseGraphicsLayer::GetDevice() ;
	CRYLDialog::GMUIPaint( xx, yy ) ;
	
	std::list<CItemInstance *>::iterator it;
	const Item::ItemInfo *lpItemInfo;
	CRYLGameData* pGame = CRYLGameData::Instance() ;

	for(it = pGame->m_csStatus.m_lstStreetTrade.begin(); it != pGame->m_csStatus.m_lstStreetTrade.end(); it++)
	{
		lpItemInfo = &(*it)->GetItemInfo();
		(*it)->m_wPutX = GetLeft() + 111 + 26 * (*it)->m_lpItemBase->GetPos().GetXIndex() + (((lpItemInfo->m_DetailData.m_cXSize * 26 - 1) - (*it)->m_lpSprite->GetSizeX()) / 2);
		(*it)->m_wPutY = GetTop() + 67 + 26 * (*it)->m_lpItemBase->GetPos().GetYIndex() + (((lpItemInfo->m_DetailData.m_cYSize * 26 - 1) - (*it)->m_lpSprite->GetSizeY()) / 2);
		(*it)->Render(lpD3DDevice);
	}

	if (pGame->m_lpPickItem && g_MessageBoxManager.m_lstMessageBox.empty())
	{
		if (g_DeviceInput.InRect( GetLeft() + 111, GetTop() + 67, GetLeft() + 372, GetTop() + 270))
		{
			Item::ItemPos pItemIndex;
			POINT *ptMousePos = g_DeviceInput.GetMouseLocal();
			pItemIndex.SetPos((ptMousePos->x - ( GetLeft() + 111 ) ) / 26, (ptMousePos->y - ( GetTop() + 67)) / 26, 0);
			__int64 ddItem;

			if ((ddItem = pGame->m_csStatus.IsCheckItem(pGame->m_lpPickItem, TakeType::TS_STALL, pItemIndex.GetXIndex(), pItemIndex.GetYIndex(), 0)) != ITEM_FAILED)
			{
				pGame->RenderRect(lpD3DDevice,
					GetLeft() + 111 + pItemIndex.GetXIndex() * 26,
					GetTop()  + 67  + pItemIndex.GetYIndex() * 26,
					GetLeft() + 111 + pGame->m_lpPickItem->GetItemInfo().m_DetailData.m_cXSize * 26 + pItemIndex.GetXIndex() * 26,
					GetTop()  + 67  + pGame->m_lpPickItem->GetItemInfo().m_DetailData.m_cYSize * 26 + pItemIndex.GetYIndex() * 26,
					0xFF0000FF, 0x80);
			} else
			{
				if (pGame->m_lpPickItem->GetItemInfo().m_DetailData.m_cXSize + pItemIndex.GetXIndex() < 8 &&
					pGame->m_lpPickItem->GetItemInfo().m_DetailData.m_cYSize + pItemIndex.GetYIndex() < 8)
				{
					pGame->RenderRect(lpD3DDevice,
						GetLeft() + 111 + pItemIndex.GetXIndex() * 26,
						GetTop()  + 67  + pItemIndex.GetYIndex() * 26,
						GetLeft() + 111 + pGame->m_lpPickItem->GetItemInfo().m_DetailData.m_cXSize * 26 + pItemIndex.GetXIndex() * 26,
						GetTop()  + 67  + pGame->m_lpPickItem->GetItemInfo().m_DetailData.m_cYSize * 26 + pItemIndex.GetYIndex() * 26,
						0xFFFF0000, 0x80);
				}
			}
		}
	}
}

HRESULT		CRYLStreetStallDlg::Render( LPDIRECT3DDEVICE8 lpD3DDevice )  
{
	return S_OK ;
}

HRESULT		CRYLStreetStallDlg::FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) 
{

	return S_OK ;
}

HRESULT		CRYLStreetStallDlg::Update( BOOL &bClick, BOOL &bEdge ) 
{
	if ( !GetVisible() || g_MessageBoxManager.GetIsExistModal() ) return E_FAIL ;

	CRYLGameData* pGame = CRYLGameData::Instance() ;
	CRYLNetworkData* pNetwork = CRYLNetworkData::Instance() ;

	if ( pGame->m_bRYLHumanInventoryFrameDlgClick || g_MessageBoxManager.GetIsExistModal() )
		return S_OK ;

	pGame->m_bRYLStreetStallDlgClick = FALSE ;
	if (g_DeviceInput.InRect( GetLeft(), GetTop(), GetLeft() + GetWidth(), GetTop() + GetHeight() ) )
	{
		bEdge = TRUE;

		if (g_DeviceInput.GetIsLeftMouseDown() || g_DeviceInput.GetIsMiddleMouseDown() || g_DeviceInput.GetIsRightMouseDown() ||
			g_DeviceInput.GetIsLeftMousePress() || g_DeviceInput.GetIsMiddleMousePress() || g_DeviceInput.GetIsRightMousePress() ||
			g_DeviceInput.GetIsLeftMouseUp() || g_DeviceInput.GetIsMiddleMouseUp() || g_DeviceInput.GetIsRightMouseUp())
		{
			bClick = TRUE;
			pGame->m_bRYLStreetStallDlgClick = TRUE ;
		}
	}

	if ( m_dwRegistItemResult & MB_YES )
	{
		if(!pGame->m_lpPickItem || pGame->m_bStallRegFlag)
		{
			m_wItemIndex			= 0 ;
			m_dwRegistItemResult	= 0 ;
			m_dwRegistItemValue		= 0 ;
			return S_OK ;
		}

		if (false == pGame->m_lpPickItem->m_lpItemBase->GetItemInfo().m_DetailData.m_bExchangeAndDrop)
		{
			m_wItemIndex			= 0 ;
			m_dwRegistItemResult	= 0 ;
			m_dwRegistItemValue		= 0 ;

			CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
			lpMessageBox->Create(CRYLStringTable::m_strString[ 136 ]);
			return true;
		}

		if ( m_dwRegistItemValue < 1 ) 
		{
			char strMessage[ MAX_PATH ] ;
			sprintf( strMessage, CRYLStringTable::m_strString[ 3484 ], CRYLStringTable::MakePrintGold( ULONG_MAX ) ) ;
			CRYLMessageBox* lpMessageBox = new CRYLMessageBox ;
			lpMessageBox->Create( strMessage ) ;
		}
		else
		{
			// 상점 아이템 올려놓기
			TakeType pTakeType ;
			pTakeType.m_srcPos			= pGame->m_lpPickItem->m_lpItemBase->GetPos() ;
			pTakeType.m_dstPos.m_cPos	= TakeType::TS_STALL ;
			pTakeType.m_dstPos.m_cIndex = m_wItemIndex ;
			
			if ( pGame->m_lpPickItem->GetIsEnableStack() )
			{
				pTakeType.m_cNum = pGame->m_lpPickItem->GetNowDurability() ;
			} 
			else
			{
				pTakeType.m_cNum = 1 ;
			}
			
			pGame->m_bStallRegFlag = TRUE;
//			g_pClientSocket->SetStatusFlag( NS_STALLREGISTERITEM ) ;
			SendPacket::CharStallRegisterItem( g_GameSession, pNetwork->m_dwMyChrID, pNetwork->m_dwMyChrID, 
				pTakeType, m_dwRegistItemValue, PktStRI::SC_REGISTER ) ;
		}

		m_wItemIndex			= 0 ;
		m_dwRegistItemResult	= 0 ;
		m_dwRegistItemValue		= 0 ;
		return S_OK ;
	} 
	else if ( m_dwRegistItemResult & MB_NO || m_dwRegistItemResult & MB_EXIT )
	{
		m_wItemIndex			= 0 ;
		m_dwRegistItemResult	= 0 ;
		m_dwRegistItemValue		= 0 ;
		return S_OK ;
	}

	if ( m_dwBuyResult & MB_YES )
	{
		if(CRYLGameData::Instance()->m_bGuildSafeResult)
		{
			m_dwItemNumber  = 0 ;
			m_dwBuyResult	= 0 ;
			m_lpBuyItem		= NULL ;
			return S_OK;
		}

		if ( m_lpBuyItem )
		{
			// 산다. 패킷 전송
			Item::ItemPos	pItemIndex ;
			FieldInstance	pTradeItem ;
			pTradeItem.m_dwInstanceID	= 0 ;
			pTradeItem.m_nOID			= 0 ;
			pTradeItem.m_dwTypeID		= m_lpBuyItem->GetProtoTypeID() ;
			
			if ( m_lpBuyItem->GetIsEnableStack() )
			{
				if (m_dwItemNumber > m_lpBuyItem->GetNowDurability() )
					m_dwItemNumber = m_lpBuyItem->GetNowDurability() ;
				
				pTradeItem.m_cNum = m_dwItemNumber ;
			}
			else
				pTradeItem.m_cNum = 1;

			if ( pTradeItem.m_cNum )
			{
				BOOL bPass = FALSE ;

				if ( pGame->m_csStatus.m_Info.Gold < (pTradeItem.m_cNum * m_lpBuyItem->m_lpItemBase->GetBuyPrice() ) ) 
					bPass = TRUE;

				if ( bPass )
				{
					CRYLMessageBox *lpMessageBox ;
					lpMessageBox = new CRYLMessageBox ;
					lpMessageBox->Create( CRYLStringTable::m_strString[ 252 ] ) ;

					m_dwBuyResult	= 0 ;
					m_lpBuyItem		= NULL ;
					return S_OK ;
				}
				
				if ( pGame->m_csStatus.GetAbleInven( &pTradeItem, pItemIndex, FALSE ) )
				{
					Item::ItemPos pPos = m_lpBuyItem->m_lpItemBase->GetPos() ;
					CRYLNetworkData::SendTradeItem( PktTr::TRC_BUY_WITH_GOLD, m_dwOwnerID, 
						m_lpBuyItem->GetProtoTypeID(), 
                        TakeType(pPos, pItemIndex, pTradeItem.m_cNum), Item::ItemPos(), NULL);
				}
			}
		}

		m_dwItemNumber  = 0 ;
		m_dwBuyResult	= 0 ;
		m_lpBuyItem		= NULL ;
		return S_OK ;
	} 
	else if ( m_dwBuyResult & MB_NO || m_dwBuyResult & MB_EXIT )
	{
		m_dwItemNumber	= 0 ;
		m_dwBuyResult	= 0 ;
		m_lpBuyItem		= NULL ;
		return S_OK ;
	}

	POINT *ptMousePos = g_DeviceInput.GetMouseLocal() ;
	unsigned long dwInvenX = (ptMousePos->x - ( GetLeft() + 111 ) ) / 26 ;
	unsigned long dwInvenY = (ptMousePos->y - ( GetTop() + 67 ) ) / 26 ;

	// 물건 셀렉트
	if ( m_dwOwnerID == CRYLNetworkData::Instance()->m_dwMyChrID )
	{
		if ( SelectItem() )
		{
			TakeType pTakeType ;
			pTakeType.m_srcPos.m_cPos	= TakeType::TS_STALL ;
			pTakeType.m_srcPos.m_cIndex = 0 ;
			pTakeType.m_dstPos			= pGame->m_lpPickItem->m_lpItemBase->GetPos() ;
			
			if ( pGame->m_lpPickItem->GetIsEnableStack() )
			{
				pTakeType.m_cNum = pGame->m_lpPickItem->GetNowDurability() ;
			} 
			else
			{
				pTakeType.m_cNum = 1 ;
			}
			pGame->m_bStallRegFlag = TRUE;
//			g_pClientSocket->SetStatusFlag( NS_STALLREGISTERITEM ) ;
			SendPacket::CharStallRegisterItem( g_GameSession, pNetwork->m_dwMyChrID, pNetwork->m_dwMyChrID, 
				pTakeType, 0, PktStRI::SC_CANCEL ) ;

			return S_OK ;
		}

		__int64 ddItem ;

		if ( g_DeviceInput.GetIsLeftMouseUp() && pGame->m_lpPickItem && pGame->m_lpPickItem->GetItemPos() == TakeType::TS_INVEN )
		{
			// 아이템 등록
			if (g_DeviceInput.InRect( GetLeft() + 111, GetTop() + 67, GetLeft() + 372, GetTop() + 270 ) )		
			{
				if ( ( ddItem = pGame->m_csStatus.IsCheckItem(pGame->m_lpPickItem, TakeType::TS_STALL, dwInvenX, dwInvenY, 0)) != ITEM_FAILED )
				{
					if (false == pGame->m_lpPickItem->m_lpItemBase->GetItemInfo().m_DetailData.m_bSell)
					{
						CRYLChattingDlg::Instance()->AddMessage( CRYLStringTable::m_strString[ 135 ], pNetwork->m_dwMyChrID, 129 );
						return S_OK;
					}

					if(pGame->m_bStallRegFlag == TRUE)
					{
						return S_OK;
					}

					// edith 2008.05.12 트레이드 불가능 아이템 검사
					if (false == pGame->m_lpPickItem->m_lpItemBase->GetItemInfo().m_DetailData.m_bExchangeAndDrop)
					{
						CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
						lpMessageBox->Create(CRYLStringTable::m_strString[ 136 ]);
						return S_OK;
					}

					Item::ItemPos	pItemIndex;
					pItemIndex.SetPos( dwInvenX, dwInvenY, 0 ) ;
					pItemIndex.m_cPos = TakeType::TS_STALL ;

					if ( ddItem == ITEM_EMPTY )
					{
						if ( pGame->m_lpPickItem->GetIsEnableStack() )
						{
							CItemInstance *lpExistItem = pGame->m_csStatus.GetItem(pGame->m_lpPickItem->m_lpItemBase->GetPos());
							if (NULL != lpExistItem && lpExistItem->GetProtoTypeID() == pGame->m_lpPickItem->GetProtoTypeID())
							{
								CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
								lpMessageBox->Create(CRYLStringTable::m_strString[ 139 ]);
								return S_OK ;
							}
						}

						CRYLMessageBox *lpMessageBox ;
						lpMessageBox = new CRYLMessageBox ;
						lpMessageBox->Create( CRYLStringTable::m_strString[590], MB_NUMBER | MB_YES | MB_NO | MB_EXIT ) ;
						lpMessageBox->SetResult( &m_dwRegistItemResult ) ;
						lpMessageBox->SetValue( &m_dwRegistItemValue ) ;
						lpMessageBox->SetEnableEnter( TRUE ) ;
						m_wItemIndex = pItemIndex.m_cIndex ;

						m_dwRegistItemResult = 0 ;
						m_dwRegistItemValue  = 0 ;
					}

					return S_OK ;
				}
			}
		}
	} 
	else
	{
		std::list<CItemInstance *>::iterator		it ;
		
		for ( it = pGame->m_csStatus.m_lstStreetTrade.begin(); it != pGame->m_csStatus.m_lstStreetTrade.end(); it++)
		{
			if ( !pGame->m_lpPickItem && (g_DeviceInput.GetIsLeftMouseUp() || g_DeviceInput.GetIsRightMouseUp()) && (*it)->IsOverMouse() )
			{
				// 아이템 구매
				BOOL bPass = FALSE;
				if (pGame->m_csStatus.m_Info.Gold >= (*it)->m_lpItemBase->GetBuyPrice()) bPass = TRUE;

				if (bPass)
				{					
					Item::ItemPos	pItemIndex ;
					FieldInstance	pTradeItem ;
					pTradeItem.m_dwInstanceID	= 0 ;
					pTradeItem.m_nOID			= 0 ;
					pTradeItem.m_dwTypeID		= ( *it )->GetProtoTypeID() ;
					pTradeItem.m_cNum			= 1 ;
					
					if ( pGame->m_csStatus.GetAbleInven( &pTradeItem, pItemIndex, FALSE ) )
					{
						if ( g_DeviceInput.GetIsLeftMouseUp() )
						{
							CRYLMessageBox*		lpMessageBox ;
							char strMessage[ MAX_PATH ] ;
							lpMessageBox = new CRYLMessageBox ;
							m_dwBuyResult	= 0 ;
							m_lpBuyItem		= ( *it ) ;
							m_dwItemNumber	= 0 ;

							if ( m_lpBuyItem->GetIsEnableStack() )
							{
								strcpy( strMessage, CRYLStringTable::m_strString[ 254 ] ) ;
								lpMessageBox->Create( strMessage, MB_NUMBER | MB_YES | MB_NO | MB_EXIT ) ;
								lpMessageBox->SetValue( &m_dwItemNumber ) ;
                                lpMessageBox->SetEnableEnter( TRUE ) ;
							} 
							else
							{
								sprintf( strMessage, CRYLStringTable::m_strString[ 255 ], 
									CRYLStringTable::MakePrintGold((*it)->m_lpItemBase->GetBuyPrice()) ) ;
								lpMessageBox->Create( strMessage, MB_YES | MB_NO | MB_EXIT ) ;
							}
							lpMessageBox->SetResult( &m_dwBuyResult ) ;
						} 
						else if ( g_DeviceInput.GetIsRightMouseUp() )
						{
							m_dwBuyResult	= MB_YES ;
							m_lpBuyItem		= ( *it ) ;
							if ( m_lpBuyItem->GetIsEnableStack() )
							{
								m_dwItemNumber = m_lpBuyItem->GetNowDurability() ;
							}
						}
					} 
					else
					{
						CRYLMessageBox *lpMessageBox ;
						lpMessageBox = new CRYLMessageBox ;
						lpMessageBox->Create( CRYLStringTable::m_strString[ 13 ] ) ;
					}
				} 
				else
				{
					CRYLMessageBox *lpMessageBox ;
					lpMessageBox = new CRYLMessageBox ;
					lpMessageBox->Create( CRYLStringTable::m_strString[ 252 ] ) ;
				}

				return S_OK ;
			}
		}
	}

	return S_OK ;
}

VOID	CRYLStreetStallDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y ) 
{
	if ( !g_pStreetStallDlg )
		return ;
}


VOID	CRYLStreetStallDlg::DlgKeyDown( CGMUIObject* Sender, WORD key, INT Shift ) 
{
	if ( !g_pStreetStallDlg )
		return ;

	switch( Sender->GetInstance() )
	{
		case RYLSTREETSTALLDLG::CHATTING_EDIT :
			if (key == 13)
			{
				if ( strcmp( g_pStreetStallDlg->m_pChattingEdit->GetText(), "" ) )
				{
					CHAR		strChat[ MAX_PATH ];
					DWORD		dwTotalLength ; 
					strcpy( strChat, g_pStreetStallDlg->m_pChattingEdit->GetText() ) ;
					dwTotalLength = strlen( strChat ) ;

					if ( g_StringFilter.Filter( g_pStreetStallDlg->m_pChattingEdit->GetText() ) )
					{
						CRYLNetworkData::SendChatMessage( NULL, g_pStreetStallDlg->m_pChattingEdit->GetText(), PktChat::STALL) ;
					}
					else
					{
						CRYLNetworkData::SendClientLog( 1, g_pStreetStallDlg->m_pChattingEdit->GetText(), g_StringFilter.m_strSearched ) ;
						g_pStreetStallDlg->m_pChattingListBox->AddItem( CRYLStringTable::m_strString[ 570 ], 0xff64ff64, 0xff64ff64, 0xffffffff ) ;
					}
				}
				g_pStreetStallDlg->m_pChattingEdit->SetText( "" ) ;
			}
		break;
	}
}

BOOL	CRYLStreetStallDlg::SelectItem() 
{
	BOOL 					bPickItem = FALSE ;
	const Item::ItemInfo*	lpItemInfo ;
	CRYLGameData* pGame = CRYLGameData::Instance() ;

	// 창고 아이템 업데이트 및 선택
	std::list< CItemInstance* >::iterator	it ;
	for ( it = pGame->m_csStatus.m_lstStreetTrade.begin(); it != pGame->m_csStatus.m_lstStreetTrade.end() ; )
	{
		lpItemInfo = &( *it )->GetItemInfo() ;

		(*it)->m_wPutX = GetLeft() + 111 + 26 * (*it)->m_lpItemBase->GetPos().GetXIndex() + (((lpItemInfo->m_DetailData.m_cXSize * 26 - 1) - (*it)->m_lpSprite->GetSizeX()) / 2);
		(*it)->m_wPutY = GetTop()  + 67  + 26 * (*it)->m_lpItemBase->GetPos().GetYIndex() + (((lpItemInfo->m_DetailData.m_cYSize * 26 - 1) - (*it)->m_lpSprite->GetSizeY()) / 2);
		
		if ( !pGame->m_lpPickItem && g_DeviceInput.GetIsLeftMouseUp() && (*it)->IsOverMouse() )
		{
			// 등록된 아이템 취소
			pGame->SetPickItem( ( *it ) ) ;
			pGame->m_csStatus.CheckStreetTradeField( CCharStatus::REMOVE_FIELD, pGame->m_lpPickItem ) ;
			it = pGame->m_csStatus.m_lstStreetTrade.erase( it ) ;

			bPickItem = TRUE ;
		} 
		else
		{
			it++ ;
		}
	}

	if ( bPickItem )
	{
		return TRUE ;
	}

	return FALSE ;
}

VOID	CRYLStreetStallDlg::InitStreetStall( DWORD dwChrID, LPSTR lpstrStreetName ) 
{
	m_dwOwnerID = dwChrID ;

	if ( m_dwOwnerID == CRYLNetworkData::Instance()->m_dwMyChrID )
	{
		if ( !lpstrStreetName )
		{
			strcpy( m_strMarketName, "" ) ;
			m_dwNameResult = 0 ;

			CRYLMessageBox *lpMessageBox = new CRYLMessageBox ;
			lpMessageBox->Create( CRYLStringTable::m_strString[ 591 ], MB_STRINGEX | MB_CONFIRM | MB_CANCEL | MB_EXIT ) ;
            lpMessageBox->m_pStringEdit->SetMaxLength( 30 );
			lpMessageBox->m_pStringEdit->SetWidth( 220 ) ;
			lpMessageBox->SetResult( &m_dwNameResult ) ;
			lpMessageBox->SetValue( ( char * )&m_strMarketName ) ;

			// 일본은 노점상 개설시 노점상 이름입력후 엔터를 무시한다.
			if ( CRYLNetworkData::Instance()->m_eInternationalCode == GameRYL::JAPAN )
			{
				lpMessageBox->SetEnableEnter( FALSE ) ;
			}
		} 
		else
		{
			RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetCreature( dwChrID ) ;
			CharDataNode*   pData = RYLCharacterDataManager::Instance()->GetCharData( pSelfCreature->GetCharID() );
			if ( !pData )
				return;
			pSelfCreature->MakeSignpostImg( const_cast<char*>( pData->m_strStreetStallName.c_str() ), false ) ;

			strcpy( m_strOwner, lpstrStreetName ) ;
			m_pStorekeeperNameLabel->SetCaption( m_strOwner ) ;

			m_pChattingListBox->AddItem( CRYLStringTable::m_strString[592], 0xff64ff64, 0xff64ff64, 0xffffffff ) ;
			m_pChattingListBox->AddItem( CRYLStringTable::m_strString[593], 0xffffffff, 0xffffffff, 0xffffffff ) ;
		}
	} 
	else
	{	
		strcpy(m_strOwner, lpstrStreetName);
		m_pStorekeeperNameLabel->SetCaption( m_strOwner ) ;

		char strText[MAX_PATH];
		sprintf(strText, CRYLStringTable::m_strString[594], m_strOwner);
		m_pChattingListBox->AddItem( strText, 0xff64ff64, 0xff64ff64, 0xffffffff ) ;
		m_pChattingListBox->AddItem( CRYLStringTable::m_strString[595], 0xffffffff, 0xffffffff, 0xffffffff ) ;
	}

	if (lpstrStreetName)
	{
		if (strcmp(lpstrStreetName, ""))
		{
			m_pChattingEdit->SetText( "" ) ;
			m_pChattingEdit->SetFocus() ;
		}
	}
}

VOID	CRYLStreetStallDlg::DeleteTradeItem()
{
	CRYLGameData* pGame = CRYLGameData::Instance() ;
	for ( INT j = 0 ; j < 8 ; j ++ )
	{
		for ( INT i = 0 ; i < 10 ; i ++ )
		{
			pGame->m_csStatus.m_aryStreetTrade[ j ][ i ] = ITEM_EMPTY ;
		}
	}

	list< CItemInstance* >::iterator	it ;
	CItemInstance*		lpDelete ;
	
	for ( it = pGame->m_csStatus.m_lstStreetTrade.begin() ; it != pGame->m_csStatus.m_lstStreetTrade.end() ; )
	{
		lpDelete = ( *it ) ;
		it = pGame->m_csStatus.m_lstStreetTrade.erase( it ) ;
		GM_DELETE( lpDelete ) ;
	}
	pGame->m_csStatus.m_lstStreetTrade.clear() ;

	std::fill_n(m_aryVisitorList, int(PktStIInfo::MAX_CUSTOMER_NUM), VisitorNode());
}

VOID	CRYLStreetStallDlg::RemoveBuyItem(CItemInstance* lpItem)
{
	if ( lpItem )
	{
		if ( m_lpBuyItem == lpItem )
		{
			m_lpBuyItem = NULL ;
		}
	}
}

bool CRYLStreetStallDlg::SendChat(char* szChatMessage)
{
	// 개인 노점상의 경우 주인은 별도로 처리한다.
	char szVisitorName[CHAR_INFOST::MAX_NAME_LEN * (1 + PktStIInfo::MAX_CUSTOMER_NUM)] = { 0, };

	std::copy(m_strOwner, m_strOwner + CHAR_INFOST::MAX_NAME_LEN, szVisitorName);

	int nIndex = 0;
	for (; nIndex < PktStIInfo::MAX_CUSTOMER_NUM; ++nIndex)
	{
		if (0 == m_aryVisitorList[nIndex].m_dwCharID)
		{
			break;
		}

		strncpy(szVisitorName + ((1 + nIndex) * CHAR_INFOST::MAX_NAME_LEN), 
			m_aryVisitorList[nIndex].m_szCharName, CHAR_INFOST::MAX_NAME_LEN);
	}

	return SendPacket::CharTargetedChat(g_GameSession, PktChat::STALL, 
		szVisitorName, 1 + nIndex, szChatMessage, strlen(szChatMessage));
}

void CRYLStreetStallDlg::AddItemList(CItemInstance* lpItem, bool bSound)
{
	CRYLGameData* pGame = CRYLGameData::Instance() ;

	CItemInstance* lpPreItem = pGame->m_csStatus.GetStreetTradeItem(lpItem->m_lpItemBase->GetPos().m_cIndex);
	if (NULL != lpPreItem) { return; }

	pGame->m_csStatus.m_lstStreetTrade.push_back(lpItem);
	pGame->m_csStatus.CheckStreetTradeField(CCharStatus::SET_FIELD, lpItem);

	CRYLStallDlg::AddItemList(lpItem, bSound);
}

VOID	CRYLStreetStallDlg::UpdateMessageBox() 
{
	if ( m_dwNameResult & MB_CONFIRM )
	{
		// 상점 개설 패킷 전송
		m_strMarketName[ 31 ] = 0 ;

		char szMarketName[ 32 ];
		strncpy( szMarketName, m_strMarketName, 32 );

		// edith 2009.07.28 노점상 이럼 선택화면에서 노점상 이름이 0일때. 처리
		// if( strtok( szMarketName, " \t\r\n" ) == NULL )
		if (strlen(szMarketName) == 0 || strtok( szMarketName, " \t\r\n" ) == NULL )
		{
			InitStreetStall( CRYLNetworkData::Instance()->m_dwMyChrID, NULL ) ;
			return;
		}
		else
		{
			if ( g_StringFilter.Filter( m_strMarketName ) )
			{
				SendPacket::CharStallOpen( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, m_strMarketName ) ;
			} 
			else
			{
				CRYLMessageBox *lpMessageBox ;
				lpMessageBox = new CRYLMessageBox ;
				lpMessageBox->Create( CRYLStringTable::m_strString[ 570 ] ) ;

				strcpy( m_strMarketName, "" ) ;
				m_dwNameResult = 0 ;
				SetVisible( FALSE ) ;

				return ;
			}
		}

		strcpy( m_strMarketName, "" ) ;
		m_dwNameResult = 0 ;
		
		CRYLGameScene*		 pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
		pScene->GetStreetStallDlg()->SetVisible( TRUE ) ;
		pScene->GetHumanInventoryFrameDlg()->SetVisible( TRUE ) ;
	} 
	else if ( m_dwNameResult & MB_CANCEL || m_dwNameResult & MB_EXIT )
	{
		strcpy( m_strMarketName, "" ) ;
		m_dwNameResult = 0 ;
		SetVisible( FALSE ) ;
	}
}