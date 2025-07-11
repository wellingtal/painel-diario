
#include <winsock2.h>

#include <RenderOption.h>

#include <Item/ItemFactory.h>
#include <Network/ClientSocket/Send/SendItem.h>

#include "BaseDataDefine.h"
#include "BaseGraphicsLayer.h"

#include "GMImageDib.h"
#include "WinInput.h"

#include "RYLMessageBox.h"
#include "RYLButton.h"
#include "RYLLabel.h"
#include "RYLSpriteList.h"
#include "RYLImageManager.h"
#include "RYLImageCabinet.h"
#include "RYLImage.h"
#include "RYLStringTable.h"

#include "RYLSceneManager.h"
#include "RYLGameScene.h"
#include "RYLNetworkData.h"
#include "RYLGameData.h"
#include "RYLMiniMapDlg.h"

#include "RYLItemReparationDlg.h"
#include "GMMemory.h"


const unsigned int srcItemL = 35 +1;
const unsigned int srcItemR = srcItemL + 77;
const unsigned int srcItemT = 50 + 15 +1;
const unsigned int srcItemB = srcItemT + 102 ;
const unsigned int destItemL = 141 +1;
const unsigned int destItemR = destItemL + 77;
const unsigned int destItemT = 50 + 15 + 1;
const unsigned int destItemB = destItemT + 102;

CRYLItemReparationDlg*		g_pItemReparationDlg = NULL;

CRYLItemReparationDlg::CRYLItemReparationDlg() 
{
	m_pOKButton			= NULL;			// 확인 버튼
	m_pCancelButton		= NULL;			// 취소 버튼
	m_pDlgLabel			= NULL;			// 다이얼로그 라벨
	m_pSrcItemLabel		= NULL;			// 원본 아이템 라벨
	m_pDestItemLabel	= NULL;			// 변환 아이템 라벨
	m_pCostValueLabel	= NULL;			// 비용 값
	m_pCostLabel		= NULL;			// 비용

	m_lCompensationGold = 0;			// 환불 금액	
	m_cResultErr		= 0;			// 에러값

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

	CRYLSpriteList::Instance()->CreateGMImage( 256, 295, &pImageDIB ) ;

	int i;
	CGMImageDib*	pTempImage;
	pTempImage = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_GUILD_256x256 );
	for ( i = 0 ; i < 57 ; i ++ )
	{
		SetRect( &rt, 0, 38, 184, 43 ) ;
		pImageDIB->DrawRect( &rt, 0, i * 5, pTempImage ) ;
		SetRect( &rt, 72, 53, 144, 58 ) ;
		pImageDIB->DrawRect( &rt, 184, i * 5, pTempImage ) ;
	}
	SetRect( &rt, 0, 43, 184, 53 ) ;
	pImageDIB->DrawRect( &rt, 0, i * 5, pTempImage ) ;
	SetRect( &rt, 144, 53, 216, 63 ) ;
	pImageDIB->DrawRect( &rt, 184, i * 5, pTempImage  ) ;

	/*
	if( CRenderOption::m_TextureMethod == 1 )			
		pImageDIB->ClearbyColor( 0x80000000 ) ;
	else												//LowTexture 라면
		pImageDIB->ClearbyColor( 0xFF000000 ) ;
	*/
	pImageDIB->MakeFrameRect( 2, 20, 252, 1, 0xffB99E6B, 0x80000000,  1 ) ;


	SetRect( &rt, 8, 74, 87, 179 ) ;
	pImageDIB->DrawRect( &rt, srcItemL-1 , srcItemT-1 - 15, 
		CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BLACKSMITH_256x256 ) ) ;			// 원본 아이템 슬롯
	pImageDIB->DrawRect( &rt, destItemL-1 , destItemT-1 - 15, 
		CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BLACKSMITH_256x256 ) ) ;			// 변환 아이템 슬롯

	SetRect( &rt, 126, 74, 245, 94 ) ;
	pImageDIB->DrawRect( &rt, 70, 167, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BLACKSMITH_256x256 ) ) ;			// 돈 아이콘
	
	/*
	// 테두리
	pImageDIB->MakeFrameRect( 1, 0, 1, 294, 0xffB99E6B, 0x80000000, 1 ) ;			
	pImageDIB->MakeFrameRect( 254, 0, 1, 294, 0xffB99E6B, 0x80000000, 1 ) ;			
	pImageDIB->MakeFrameRect( 1, 294, 254, 1, 0xffB99E6B, 0x80000000, 1 ) ;			
	*/
	
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	AttachWindowTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;
	
	SetWidth( 256 ) ;
	SetHeight( 295 ) ;
	SetLeft( ( CGMUIScreen::GetInstance()->GetWidth() - GetWidth() ) / 2 ) ;                                
	SetTop( ( CGMUIScreen::GetInstance()->GetHeight() - GetHeight() ) / 2 ) ;

	SetFormType( RYLDLG_TYPE_CLOSEBUTTON ) ;

	g_pItemReparationDlg = this;

	InitCtrl() ;
}


CRYLItemReparationDlg::~CRYLItemReparationDlg() 
{
	FinalCtrl() ;
	g_pItemReparationDlg = NULL;
}

VOID	CRYLItemReparationDlg::InitCtrl() 
{
	CGMImageDib*			pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;

	// 확인 버튼
	m_pOKButton = new CRYLButton( this ) ;
	m_pOKButton->SetLeft( 58 ) ;
	m_pOKButton->SetTop( 268 ) ;
	m_pOKButton->SetCaption( CRYLStringTable::m_strString[ 2164 ] ) ;
	m_pOKButton->SetFontColor( 0xffffffff) ;
	m_pOKButton->SetColor( 0xffffffff) ;
	m_pOKButton->OnClickEvent = DlgClick ;
	m_pOKButton->SetInstance( RYLITEMREPARATIONDLG::OK_BUTTON ) ;
	m_pOKButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pOKButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pOKButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pOKButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pOKButton ) ;

	// 취소 버튼
	m_pCancelButton = new CRYLButton( this ) ;
	m_pCancelButton->SetLeft( 132 ) ;
	m_pCancelButton->SetTop( 268 ) ;
	m_pCancelButton->SetCaption( CRYLStringTable::m_strString[ 1897 ] ) ;
	m_pCancelButton->SetFontColor( 0xffffffff) ;
	m_pCancelButton->SetColor( 0xffffffff) ;
	m_pCancelButton->OnClickEvent = DlgClick ;
	m_pCancelButton->SetInstance( RYLITEMREPARATIONDLG::CANCEL_BUTTON ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pCancelButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pCancelButton ) ;	

	// 다이얼로그 라벨
	m_pDlgLabel = new CRYLLabel( this ) ;								
	m_pDlgLabel->SetLeft( 12 ) ;
	m_pDlgLabel->SetTop( 18 ) ;
	m_pDlgLabel->SetFontColor (0xffffffff);
	m_pDlgLabel->SetAutoSize( TRUE ) ;
	m_pDlgLabel->SetCaption( CRYLStringTable::m_strString[ 2739 ] ) ;
	AttachGMUIControl( m_pDlgLabel ) ;

	// 원본 아이템 라벨
	m_pSrcItemLabel = new CRYLLabel( this ) ;								
	m_pSrcItemLabel->SetLeft( 44 ) ;
	m_pSrcItemLabel->SetTop( 48 ) ;
	m_pSrcItemLabel->SetFontColor (0xffffffff);
	m_pSrcItemLabel->SetAutoSize( TRUE ) ;
	m_pSrcItemLabel->SetCaption( CRYLStringTable::m_strString[ 2740 ] ) ;
	AttachGMUIControl( m_pSrcItemLabel ) ;

	// 변환 아이템 라벨
	m_pDestItemLabel = new CRYLLabel( this ) ;								
	m_pDestItemLabel->SetLeft( 150 ) ;
	m_pDestItemLabel->SetTop( 48 ) ;
	m_pDestItemLabel->SetFontColor (0xffffffff);
	m_pDestItemLabel->SetAutoSize( TRUE ) ;
	m_pDestItemLabel->SetCaption( CRYLStringTable::m_strString[ 2741 ] ) ;
	AttachGMUIControl( m_pDestItemLabel ) ;

	// 설명 첫째줄
	m_pDescriptionLabel1 = new CRYLLabel( this ) ;						
	m_pDescriptionLabel1->SetLeft( 24 ) ;
	m_pDescriptionLabel1->SetTop( 211 ) ;
	m_pDescriptionLabel1->SetFontColor (0xffffffff);
	m_pDescriptionLabel1->SetAutoSize( TRUE ) ;
	m_pDescriptionLabel1->SetCaption( CRYLStringTable::m_strString[ 2742 ] ) ;
	AttachGMUIControl( m_pDescriptionLabel1 ) ;

	// 설명 둘째줄
	m_pDescriptionLabel2 = new CRYLLabel( this ) ;						
	m_pDescriptionLabel2->SetLeft( 24 ) ;
	m_pDescriptionLabel2->SetTop( 227 ) ;
	m_pDescriptionLabel2->SetFontColor (0xffffffff);
	m_pDescriptionLabel2->SetAutoSize( TRUE ) ;
	m_pDescriptionLabel2->SetCaption( CRYLStringTable::m_strString[ 2743 ] ) ;
	AttachGMUIControl( m_pDescriptionLabel2 ) ;

	// 설명 셋째줄
	m_pDescriptionLabel3 = new CRYLLabel( this ) ;						
	m_pDescriptionLabel3->SetLeft( 24 ) ;
	m_pDescriptionLabel3->SetTop( 243 ) ;
	m_pDescriptionLabel3->SetFontColor (0xffffffff);
	m_pDescriptionLabel3->SetAutoSize( TRUE ) ;
	m_pDescriptionLabel3->SetCaption( CRYLStringTable::m_strString[ 2744 ] ) ;
	AttachGMUIControl( m_pDescriptionLabel3 ) ;

	// 비용 값
	m_pCostValueLabel = new CRYLLabel( this ) ;							
	m_pCostValueLabel->SetLeft( 110 ) ;
	m_pCostValueLabel->SetTop( 187 ) ;
	m_pCostValueLabel->SetFontColor (0xffffffff);
	m_pCostValueLabel->SetAutoSize( TRUE ) ;
	m_pCostValueLabel->SetCaption( "" ) ;
	AttachGMUIControl( m_pCostValueLabel ) ;

	// 비용
	m_pCostLabel = new CRYLLabel( this ) ;								
	m_pCostLabel->SetLeft( 110 ) ;
	m_pCostLabel->SetTop( 187 ) ;
	m_pCostLabel->SetFontColor (0xffffffff);
	m_pCostLabel->SetAutoSize( TRUE ) ;
	m_pCostLabel->SetCaption( "0" ) ;
	AttachGMUIControl( m_pCostLabel ) ;
}

VOID	CRYLItemReparationDlg::FinalCtrl() 
{
	GM_DELETE( m_pOKButton ) ;								// 확인 버튼
	GM_DELETE( m_pCancelButton ) ;							// 취소 버튼

	GM_DELETE( m_pDlgLabel );								// 다이얼로그 라벨
	GM_DELETE( m_pSrcItemLabel );							// 원본 아이템 라벨
	GM_DELETE( m_pDestItemLabel );							// 변환 아이템 라벨

	GM_DELETE( m_pDescriptionLabel1 ) ;						// 설명 첫째줄
	GM_DELETE( m_pDescriptionLabel2 ) ;						// 설명 둘째줄
	GM_DELETE( m_pDescriptionLabel3 ) ;						// 설명 셋째줄

	GM_DELETE( m_pCostValueLabel ) ;						// 비용 값
	GM_DELETE( m_pCostLabel ) ;								// 비용
}

VOID	CRYLItemReparationDlg::Show( CGMUIObject* Sender ) 
{
}

VOID	CRYLItemReparationDlg::Hide( CGMUIObject* Sender ) 
{
}

VOID	CRYLItemReparationDlg::GMUIPaint( INT xx, INT yy ) 
{
	CRYLDialog::GMUIPaint( xx, yy ) ;
	CRYLGameData* pGame = CRYLGameData::Instance() ;
	LPDIRECT3DDEVICE8   lpD3DDevice = BaseGraphicsLayer::GetDevice() ;

	// 보상 판매 대상 아이템
	if (NULL != pGame->m_csStatus.m_lpCompensation)
	{
		pGame->m_csStatus.m_lpCompensation->m_wPutX = GetLeft() + srcItemL +
			((77 / 2) - (pGame->m_csStatus.m_lpCompensation->m_lpSprite->GetSizeX() / 2));
		pGame->m_csStatus.m_lpCompensation->m_wPutY = GetTop() + srcItemT +
			((103 / 2) - (pGame->m_csStatus.m_lpCompensation->m_lpSprite->GetSizeY() / 2));
		pGame->m_csStatus.m_lpCompensation->Render(lpD3DDevice);
	}

	// 보상 판매 결과 아이템
	if (NULL != pGame->m_csStatus.m_lpCompensationResult)
	{
		pGame->m_csStatus.m_lpCompensationResult->m_wPutX = GetLeft() + destItemL +
			((77 / 2) - (pGame->m_csStatus.m_lpCompensationResult->m_lpSprite->GetSizeX() / 2));
		pGame->m_csStatus.m_lpCompensationResult->m_wPutY = GetTop() + destItemT +
			((103 / 2) - (pGame->m_csStatus.m_lpCompensationResult->m_lpSprite->GetSizeY() / 2));
		pGame->m_csStatus.m_lpCompensationResult->Render(lpD3DDevice);
	}

	if (pGame->m_lpPickItem)
	{
		// 보상 판매 대상 슬롯
		if (g_DeviceInput.InRect( GetLeft() + srcItemL, GetTop() + srcItemT, GetLeft() + srcItemR, GetTop() + srcItemB))
		{
			if (true == pGame->m_lpPickItem->GetIsEnableAddOption()) 
			{
				pGame->RenderRect(lpD3DDevice, GetLeft() + srcItemL, GetTop() + srcItemT, 
					GetLeft() + srcItemR, GetTop() + srcItemB, 0xFF0000FF, 0x80);
			} 
			else
			{
				pGame->RenderRect(lpD3DDevice, GetLeft() + srcItemL, GetTop() + srcItemT, 
					GetLeft() + srcItemR, GetTop() + srcItemB, 0xFFFF0000, 0x80);
			}
		} 
		// 보상 판매 결과 슬롯
		else if (g_DeviceInput.InRect( GetLeft() + destItemL, GetTop() + destItemT, GetLeft() + destItemR, GetTop() + destItemB))
		{
			pGame->RenderRect(lpD3DDevice, GetLeft() + destItemL, GetTop() + destItemT, 
				GetLeft() + destItemR, GetTop() + destItemB, 0xFFFF0000, 0x80);
		} 	
	}
}

HRESULT		CRYLItemReparationDlg::FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) 
{
	return S_OK ;
}

HRESULT		CRYLItemReparationDlg::Update( BOOL &bClick, BOOL &bEdge ) 
{
	if (FALSE == GetVisible())
	{
		return S_OK;
	}

	CRYLGameData* pGame			= CRYLGameData::Instance() ;
	CRYLNetworkData* pNetwork	= CRYLNetworkData::Instance() ;

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

	POINT *ptMousePos = g_DeviceInput.GetMouseLocal();

	if (g_DeviceInput.InRect( GetLeft() + srcItemL, GetTop() + srcItemT, GetLeft() + srcItemR, GetTop() + srcItemB))
	{
		if (g_DeviceInput.GetIsLeftMouseDown())
		{
			if (NULL != pGame->m_lpPickItem)
			{
				if (true == pGame->m_lpPickItem->GetIsEnableAddOption())
				{
					if (TRUE == pGame->m_lpPickItem->GetIsEquip())
					{
						Item::ItemPos pItemIndex;
						pItemIndex.m_cIndex = Item::ExtraSpacePos::COMPENSATION_POS;
						pItemIndex.m_cPos = TakeType::TS_EXTRA;

						if (NULL == pGame->m_csStatus.m_lpCompensation)
						{
							// 아이템 이동							
							if (pGame->m_lpPickItem->m_lpItemBase->GetPos() == pItemIndex)
							{
								pGame->m_csStatus.m_lpCompensation = pGame->m_lpPickItem;
								pGame->SetPickItem(NULL);
							} 
							else
							{
								pNetwork->SendTakeItem(pGame->m_lpPickItem, &pItemIndex, 1);
							}
						} 
						else
						{
							// 스왑하기
							pNetwork->SendSwapItem(pGame->m_lpPickItem, &pItemIndex, pGame->m_csStatus.m_lpCompensation);
						}
					}
				}
			} 
			else
			{
				if (pGame->m_dwKindCursor == CURSOR_NORMAL && pGame->m_csStatus.m_lpCompensation)
				{
					pGame->SetPickItem(pGame->m_csStatus.m_lpCompensation);
					pGame->m_csStatus.m_lpCompensation = NULL;

					GM_DELETE(pGame->m_csStatus.m_lpCompensationResult);
					pGame->m_csStatus.m_lpCompensationResult = NULL;					

					m_pCostLabel->SetCaption("0");

					m_cResultErr = Item::CEquipment::S_SUCCESS;
				}
			}

			return S_OK;
		}
	}

	if (NULL != pGame->m_csStatus.m_lpCompensation)
	{
		if (NULL == pGame->m_csStatus.m_lpCompensationResult && Item::CEquipment::S_SUCCESS == m_cResultErr)
		{
			Item::CEquipment* lpSrcEquip = 
				Item::CEquipment::DowncastToEquipment(pGame->m_csStatus.m_lpCompensation->m_lpItemBase);

			CharacterStatus	status;
			status.Init(pGame->m_csStatus.m_Info);

			Item::CEquipment* lpDstEquip = NULL;

			// 보상 결과물을 만든다.
			m_cResultErr = lpSrcEquip->Compensation(pGame->m_csStatus.m_Info.Class, pGame->m_csStatus.m_Info.Level, 
				status, &lpDstEquip, m_lCompensationGold);
			if (Item::CEquipment::S_SUCCESS == m_cResultErr)
			{
				GM_DELETE(pGame->m_csStatus.m_lpCompensationResult);

				pGame->m_csStatus.m_lpCompensationResult = new CItemInstance;
				pGame->m_csStatus.m_lpCompensationResult->SetItemInfofromItem(
					lpDstEquip, Item::ItemPos(TakeType::TS_EXTRA, Item::ExtraSpacePos::COMPENSATION_POS));

				Item::CItemFactory::GetInstance().DeleteItem(lpDstEquip);
			
				// 환불 금액 표기
				m_pCostLabel->SetCaption(CRYLStringTable::MakePrintGoldAbleMinus(m_lCompensationGold)) ;
			}
		}
	}

	return S_OK;
}

bool	CRYLItemReparationDlg::OK(void)
{
	CRYLGameData* pGame = CRYLGameData::Instance();

	// 보상 판매에 실패한 경우
	switch (m_cResultErr)
	{
		case Item::CEquipment::E_NEW_SEASON_EQUIP:
		{
			CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
			lpMessageBox->Create(CRYLStringTable::m_strString[ 2745 ]);
			return false;
		}

		case Item::CEquipment::E_WRONG_TYPE:
		{
			CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
			lpMessageBox->Create(CRYLStringTable::m_strString[ 2746 ]);
			return false;
		}

		case Item::CEquipment::E_LOW_STATUS_EQUIP:
		{
			CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
			lpMessageBox->Create(CRYLStringTable::m_strString[ 2792 ]);
			return false;
		}
	}

	if (m_lCompensationGold < 0 && static_cast<unsigned long>(abs(m_lCompensationGold)) > pGame->m_csStatus.m_Info.Gold)
	{
		CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
		lpMessageBox->Create(CRYLStringTable::m_strString[162]);
		return false;
	}

	if (true == SendPacket::CharItemCompensation(g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID))
	{
		g_pClientSocket->SetStatusFlag(NS_NOWAITING);
	}

	GM_DELETE(pGame->m_csStatus.m_lpCompensation);
	GM_DELETE(pGame->m_csStatus.m_lpCompensationResult);

	g_pItemReparationDlg->m_pCostLabel->SetCaption("0") ;

	return true;
}

VOID	CRYLItemReparationDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y )
{
	if ( NULL == g_pItemReparationDlg )
		return ;

	switch ( Sender->GetInstance() )
	{
		case RYLITEMREPARATIONDLG::OK_BUTTON:
		{
			if (true == g_pItemReparationDlg->OK())
			{
				g_pItemReparationDlg->SetVisible(FALSE);
			}
			break;
		}

		case RYLITEMREPARATIONDLG::CANCEL_BUTTON:
		{
			g_pItemReparationDlg->SetVisible(FALSE);
			break;
		}
	}
}