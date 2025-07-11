
#include <winsock2.h>

#include "NPC/NPCList.h"
#include <Creature/Character/CharacterClass.h>
#include <Item/ItemMgr.h>

#include "GMUIObject.h"
#include "GMImageDib.h"
#include "BaseDataDefine.h"
#include "BaseGraphicsLayer.h"

#include "RYLTabButton.h"
#include "RYLImageManager.h"
#include "RYLImage.h"
#include "RYLStringTable.h"
#include "RYLSpriteList.h"
#include "RYLMessageBox.h"
#include "RYLMessageBoxManager.h"

#include "RYLGameScene.h"
#include "RYLSceneManager.h"
#include "RYLGameData.h"
#include "RYLNetworkData.h"

#include "RYLHumanInventoryFrameDlg.h"
#include "RYLTradeDlg.h"
#include "RYLNewTradeDlg.h"
#include "RYLBlacksmithDlg.h"

#include "..\\GUITooltipItem.h"
#include "..\\ItemInstance.h"

#include "RYLCastle.h"
#include "RYLCastleManager.h"
#include <Castle/CastleConstants.h>

#include <RenderOption.h>
#include <WinInput.h>
#include "GMMemory.h"


using namespace Item;

CRYLBlacksmithDlg*	g_pBlacksmithDlg = NULL ;

CRYLBlacksmithDlg::CRYLBlacksmithDlg() 
{
	RECT rt ;
	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;

	m_pReinforcementButton		= NULL ;	// 강화
	m_pRepairButton				= NULL ;	// 수리하기
	m_pAllRepairButton			= NULL ;	// 모두 수리하기
	m_pRefineryLabel			= NULL ;	// 제련
	m_pReinforcementWeaponLabel	= NULL ;	// 강화시킬 장비
	m_pMetalLabel				= NULL ;	// 금속
	m_pGoldLabel				= NULL ;	// 골드

	m_pResmeltTax				= NULL ;	// 제련 세율
	m_pRepairTax				= NULL ;	// 수리 세율
	m_pResmeltTaxValue			= NULL ;	// 제련 세율 값
	m_pRepairTaxValue			= NULL ;	// 수리 세율 값


	CRYLSpriteList::Instance()->CreateGMImage( 256, 15, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 0, 256, 15 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BLACKSMITH_256x256 ) ) ;
	
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	AttachCaptionTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;

	CRYLSpriteList::Instance()->CreateGMImage( 256, 192, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 15, 256, 192 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BLACKSMITH_256x256 ) ) ;

// CASTLE_TODO : 공성전 세금관련 UI는 막아둔다.

//	if ( true == CRYLNetworkData::Instance()->UseContents(GameRYL::SIEGE) )
//	{
//		pImageDIB->MakeFrameRect( 152,  91, 38, 19, 0xffB99E6B, 0xFF120F0C,  1 ) ;
//		pImageDIB->MakeFrameRect( 152, 115, 38, 19, 0xffB99E6B, 0xFF120F0C,  1 ) ;
//	}
	
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

	g_pBlacksmithDlg = this ;
	InitCtrl() ;

	m_dwUpgradeGold		= 0 ;
	m_dwUpgradeResult	= 0 ; 
}

CRYLBlacksmithDlg::~CRYLBlacksmithDlg() 
{
	FinalCtrl() ;
}

VOID	CRYLBlacksmithDlg::InitCtrl() 
{
	CGMImageDib* pSprite			= CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;

	// 강화
	m_pReinforcementButton = new CRYLButton( this ) ;
	m_pReinforcementButton->SetLeft( 201 ) ;
	m_pReinforcementButton->SetTop( 106 ) ;
	m_pReinforcementButton->SetCaption( CRYLStringTable::m_strString[ 2086 ] ) ;
	m_pReinforcementButton->SetFontColor( 0xffffffff) ;
	m_pReinforcementButton->SetColor( 0xffffffff) ;
	m_pReinforcementButton->OnClickEvent = DlgClick ;
	m_pReinforcementButton->SetInstance( RYLBLACKSMITHDLG::REINFORCEMENT_BUTTON ) ;
	m_pReinforcementButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 2 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pReinforcementButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 2 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pReinforcementButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 2 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pReinforcementButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 2 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pReinforcementButton->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 1889 ], 1889 ) ;
	AttachGMUIControl( m_pReinforcementButton ) ;

	// 수리하기
	m_pRepairButton = new CRYLButton( this ) ;
	m_pRepairButton->SetLeft( 93 ) ;
	m_pRepairButton->SetTop( 154 ) ;
	m_pRepairButton->SetCaption( CRYLStringTable::m_strString[ 2087 ] ) ;
	m_pRepairButton->SetFontColor( 0xffffffff) ;
	m_pRepairButton->SetColor( 0xffffffff) ;
	m_pRepairButton->OnClickEvent = DlgClick ;
	m_pRepairButton->SetInstance( RYLBLACKSMITHDLG::REPAIR_BUTTON ) ;
	m_pRepairButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pRepairButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pRepairButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pRepairButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pRepairButton->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 1888 ], 1888 ) ;
	AttachGMUIControl( m_pRepairButton ) ;

	// 모두 수리하기
	m_pAllRepairButton = new CRYLButton( this ) ;
	m_pAllRepairButton->SetLeft( 179 ) ;
	m_pAllRepairButton->SetTop( 154 ) ;
	m_pAllRepairButton->SetCaption( CRYLStringTable::m_strString[ 2088 ] ) ;
	m_pAllRepairButton->SetFontColor( 0xffffffff) ;
	m_pAllRepairButton->SetColor( 0xffffffff) ;
	m_pAllRepairButton->OnClickEvent = DlgClick ;
	m_pAllRepairButton->SetInstance( RYLBLACKSMITHDLG::ALLREPAIR_BUTTON ) ;
	m_pAllRepairButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pAllRepairButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pAllRepairButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pAllRepairButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pAllRepairButton->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 1891 ], 1891 ) ;
	AttachGMUIControl( m_pAllRepairButton ) ;

	// 제련
	m_pRefineryLabel = new CRYLLabel( this ) ;
	m_pRefineryLabel->SetLeft( 18 ) ;
	m_pRefineryLabel->SetTop( 20 ) ;
	m_pRefineryLabel->SetFontColor( 0xffffffff ) ;
	m_pRefineryLabel->SetAutoSize( TRUE ) ;
	m_pRefineryLabel->SetCaption( CRYLStringTable::m_strString[ 2089 ] ) ;
	AttachGMUIControl( m_pRefineryLabel ) ;

	// 강화시킬 장비
	m_pReinforcementWeaponLabel = new CRYLLabel( this ) ;
	m_pReinforcementWeaponLabel->SetLeft( 11 ) ;
	m_pReinforcementWeaponLabel->SetTop( 56 ) ;
	m_pReinforcementWeaponLabel->SetFontColor( 0xffffffff ) ;
	m_pReinforcementWeaponLabel->SetAutoSize( TRUE ) ;
	m_pReinforcementWeaponLabel->SetCaption( CRYLStringTable::m_strString[ 2090 ] ) ;
	AttachGMUIControl( m_pReinforcementWeaponLabel ) ;

	// 금속
	m_pMetalLabel = new CRYLLabel( this ) ;
	m_pMetalLabel->SetLeft( 94 ) ;
	m_pMetalLabel->SetTop( 56 ) ;
	m_pMetalLabel->SetFontColor( 0xffffffff ) ;
	m_pMetalLabel->SetAutoSize( TRUE ) ;
	m_pMetalLabel->SetCaption( CRYLStringTable::m_strString[ 2091 ] ) ;
	AttachGMUIControl( m_pMetalLabel ) ;
	
	// 골드
	m_pGoldLabel = new CRYLLabel( this ) ;
	m_pGoldLabel->SetLeft( 157 ) ;
	m_pGoldLabel->SetTop( 78 ) ;
	m_pGoldLabel->SetFontColor( 0xffffffff ) ;
	m_pGoldLabel->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pGoldLabel ) ;

// CASTLE_TODO : 공성전 세금관련 UI는 막아둔다.

//	if ( true == CRYLNetworkData::Instance()->UseContents(GameRYL::SIEGE) )
//	{
//		// 제련 세율
//		m_pResmeltTax = new CRYLLabel( this ) ;
//		m_pResmeltTax->SetLeft( 94 ) ;
//		m_pResmeltTax->SetTop( 110 ) ;
//		m_pResmeltTax->SetFontColor( 0xFFFFFFFF ) ;
//		m_pResmeltTax->SetAutoSize( TRUE ) ;
//		m_pResmeltTax->SetCaption( CRYLStringTable::m_strString[ 3867 ] ) ;
//		AttachGMUIControl( m_pResmeltTax ) ;
//
//		// 수리 세율
//		m_pRepairTax = new CRYLLabel( this ) ;
//		m_pRepairTax->SetLeft( 94 ) ;
//		m_pRepairTax->SetTop( 134 ) ;
//		m_pRepairTax->SetFontColor( 0xFFFFFFFF ) ;
//		m_pRepairTax->SetAutoSize( TRUE ) ;
//		m_pRepairTax->SetCaption( CRYLStringTable::m_strString[ 3868 ] ) ;
//		AttachGMUIControl( m_pRepairTax ) ;
//
//		// 제련 세율 값
//		m_pResmeltTaxValue = new CRYLLabel( this ) ;
//		SetRect( &rt, 153, 110, 188, 120 ) ;
//		m_pResmeltTaxValue->SetAlignRect( rt ) ;
//		m_pResmeltTaxValue->SetAlign( CENTER ) ;
//		m_pResmeltTaxValue->SetCaption( "---" ) ;
//		m_pResmeltTaxValue->SetFontColor( 0xFFFFFFFF ) ;
//		m_pResmeltTaxValue->SetAutoSize( TRUE ) ;
//		AttachGMUIControl( m_pResmeltTaxValue ) ;
//
//		// 수리 세율 값
//		m_pRepairTaxValue = new CRYLLabel( this ) ;
//		SetRect( &rt, 153, 134, 188, 144 ) ;
//		m_pRepairTaxValue->SetAlignRect( rt ) ;
//		m_pRepairTaxValue->SetAlign( CENTER ) ;
//		m_pRepairTaxValue->SetCaption( "---" ) ;
//		m_pRepairTaxValue->SetFontColor( 0xFFFFFFFF ) ;
//		m_pRepairTaxValue->SetAutoSize( TRUE ) ;
//		AttachGMUIControl( m_pRepairTaxValue ) ;
//	}
}

VOID	CRYLBlacksmithDlg::FinalCtrl() 
{
	GM_DELETE( m_pReinforcementButton  ) ;			// 강화
	GM_DELETE( m_pRepairButton ) ;					// 수리하기
	GM_DELETE( m_pAllRepairButton ) ;				// 모두 수리하기
	GM_DELETE( m_pRefineryLabel ) ;					// 제련
	GM_DELETE( m_pReinforcementWeaponLabel ) ;		// 강화시킬 장비
	GM_DELETE( m_pMetalLabel ) ;					// 금속
	GM_DELETE( m_pGoldLabel ) ;						// 골드
	GM_DELETE( m_pResmeltTax ) ;					// 제련 세율
	GM_DELETE( m_pRepairTax ) ;						// 수리 세율
	GM_DELETE( m_pResmeltTaxValue ) ;				// 제련 세율 값
	GM_DELETE( m_pRepairTaxValue ) ;				// 수리 세율 값
}

VOID	CRYLBlacksmithDlg::Show( CGMUIObject* Sender ) 
{
	// 인벤창을 함께 띄운다.
	CRYLGameScene* pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
	CRYLHumanInventoryFrameDlg* pInventoryDlg = ( CRYLHumanInventoryFrameDlg* )pScene->GetHumanInventoryFrameDlg() ;
	pInventoryDlg->SetVisible( TRUE ) ;
	pInventoryDlg->ShowInventory() ;

	m_dwUpgradeGold		= 0 ;
	m_dwUpgradeResult	= 0 ; 
}

VOID	CRYLBlacksmithDlg::Hide( CGMUIObject* Sender ) 
{
	CRYLGameData::Instance()->m_dwKindCursor = CURSOR_NORMAL ;
	if ( m_pResmeltTaxValue ) m_pResmeltTaxValue->SetCaption( "---" ) ;
	if ( m_pRepairTaxValue ) m_pRepairTaxValue->SetCaption( "---" ) ;
}

VOID	CRYLBlacksmithDlg::GMUIPaint( INT xx, INT yy ) 
{
	LPDIRECT3DDEVICE8   lpD3DDevice = BaseGraphicsLayer::GetDevice() ;
	CRYLDialog::GMUIPaint( xx, yy ) ;

	CRYLGameData* pGame = CRYLGameData::Instance() ;

	if ( pGame->m_csStatus.m_lpUpgrade[ExtraSpacePos::UPGRADE_EQUIPMENT_POS])
	{
		pGame->m_csStatus.m_lpUpgrade[ExtraSpacePos::UPGRADE_EQUIPMENT_POS]->m_wPutX = GetLeft() + 9 +
			((77 / 2) - (pGame->m_csStatus.m_lpUpgrade[ExtraSpacePos::UPGRADE_EQUIPMENT_POS]->m_lpSprite->GetSizeX() / 2));
		pGame->m_csStatus.m_lpUpgrade[ExtraSpacePos::UPGRADE_EQUIPMENT_POS]->m_wPutY = GetTop() + 75 +
			((103 / 2) - (pGame->m_csStatus.m_lpUpgrade[ExtraSpacePos::UPGRADE_EQUIPMENT_POS]->m_lpSprite->GetSizeY() / 2));
		pGame->m_csStatus.m_lpUpgrade[ExtraSpacePos::UPGRADE_EQUIPMENT_POS]->Render(lpD3DDevice);
	}
	if (pGame->m_csStatus.m_lpUpgrade[ExtraSpacePos::UPGRADE_MATERIAL_POS])
	{
		pGame->m_csStatus.m_lpUpgrade[ExtraSpacePos::UPGRADE_MATERIAL_POS]->m_wPutX = GetLeft() + 94 + 
			((24 - pGame->m_csStatus.m_lpUpgrade[ExtraSpacePos::UPGRADE_MATERIAL_POS]->m_lpSprite->GetSizeX()) / 2);
		pGame->m_csStatus.m_lpUpgrade[ExtraSpacePos::UPGRADE_MATERIAL_POS]->m_wPutY = GetTop() + 75 + 
			((24 - pGame->m_csStatus.m_lpUpgrade[ExtraSpacePos::UPGRADE_MATERIAL_POS]->m_lpSprite->GetSizeY()) / 2);
		pGame->m_csStatus.m_lpUpgrade[ExtraSpacePos::UPGRADE_MATERIAL_POS]->Render(lpD3DDevice);
	}

	if (pGame->m_lpPickItem)
	{
		if (g_DeviceInput.InRect( GetLeft() + 9, GetTop() + 75, GetLeft() + 85, GetTop() + 177))
		{
			if (true == pGame->m_lpPickItem->GetIsEnableUpgrade())
			{
				
				pGame->RenderRect(lpD3DDevice, GetLeft() + 9, GetTop() + 75, GetLeft() + 85, GetTop() + 177, 0xFF0000FF, 0x80);
			} else
			{
				pGame->RenderRect(lpD3DDevice, GetLeft() + 9, GetTop() + 75, GetLeft() + 85, GetTop() + 177, 0xFFFF0000, 0x80);
			}
		} 
		else if (g_DeviceInput.InRect(GetLeft() + 94, GetTop() + 75, GetLeft() + 118, GetTop() + 99))
		{
			if (pGame->m_lpPickItem->GetItemType() == ItemType::ORE)
			{
				pGame->RenderRect(lpD3DDevice, GetLeft() + 94, GetTop() + 75, GetLeft() + 118, GetTop() + 99, 0xFF0000FF, 0x80);
			} else
			{
				pGame->RenderRect(lpD3DDevice, GetLeft() + 94, GetTop() + 75, GetLeft() + 118, GetTop() + 99, 0xFFFF0000, 0x80);
			}
		}
	}

	char strGold[MAX_PATH];
	sprintf(strGold, "%d", m_dwUpgradeGold);
	m_pGoldLabel->SetCaption( strGold ) ;
}

HRESULT		CRYLBlacksmithDlg::Render( LPDIRECT3DDEVICE8 lpD3DDevice )  
{
	return S_OK ;
}

HRESULT		CRYLBlacksmithDlg::FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) 
{
	
	

//	m_ButtonManager.Render(lpD3DDevice);

	return S_OK ;
}

HRESULT		CRYLBlacksmithDlg::Update( BOOL &bClick, BOOL &bEdge ) 
{
	if (FALSE == GetVisible())
	{
		return S_OK;
	}

	CRYLGameData* pGame			= CRYLGameData::Instance() ;
	CRYLNetworkData* pNetwork  = CRYLNetworkData::Instance() ;

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

// CASTLE_TODO : 공성전 세금관련 UI는 막아둔다.

	// 성에 속한 NPC 일 경우 세율 표시
//	if ( true == CRYLNetworkData::Instance()->UseContents( GameRYL::SIEGE ) )
//	{
//		NPCNode* pNpcNode = g_NPCList.GetNPCNode( pGame->m_dwTradingNpcID );
//		if ( pNpcNode && pNpcNode->m_bBelongToCastle )
//		{
//			CRYLCastle* lpCastle = CRYLCastleManager::Instance()->FindCastleByNameID( pNpcNode->m_dwTownOrNameID );
//			if ( lpCastle )
//			{
//				char szBuff[ MAX_PATH ] ;
//				sprintf( szBuff, "%d%%", lpCastle->GetTax( Castle::RESMELT_TAX ) ) ;
//				m_pResmeltTaxValue->SetCaption( szBuff ) ;
//				sprintf( szBuff, "%d%%", lpCastle->GetTax( Castle::REPAIR_TAX ) ) ;
//				m_pRepairTaxValue->SetCaption( szBuff ) ;
//			}
//		}
//	}

	POINT *ptMousePos = g_DeviceInput.GetMouseLocal();
	unsigned long dwMousePosX = ptMousePos->x - (GetLeft() + 9);
	unsigned long dwMousePosY = ptMousePos->y - (GetTop() + 75);
	unsigned long dwWindowX = dwMousePosX / 26;
	unsigned long dwWindowY = dwMousePosY / 26;

	if (m_dwUpgradeResult & MB_YES)
	{
		pGame->m_csStatus.SetUpgrading( TRUE );
		// 거지같이 이 이팩트가 끝나야 패킷이 전송된다 =+=
		g_pClientSocket->SetStatusFlag(NS_ITEMUPGRADE);
		pGame->m_lpItemUpgrade = new CEffScript;
		pGame->m_lpItemUpgrade->SetInterfaceSet(true);
		pGame->m_lpItemUpgrade->GetScriptBinData("inter_enhance_start.esf");
		
		pGame->m_lpItemUpgrade->m_InterfacePos[ExtraSpacePos::UPGRADE_EQUIPMENT_POS] = GetLeft() + 47 ;
		pGame->m_lpItemUpgrade->m_InterfacePos[ExtraSpacePos::UPGRADE_MATERIAL_POS] = GetTop() + 250  ;

		CSceneManager::m_EffectManager.AddInterfaceScript(pGame->m_lpItemUpgrade);
		m_dwUpgradeResult = 0;
		return S_OK ;
	} 
	else if (m_dwUpgradeResult & MB_NO || m_dwUpgradeResult & MB_EXIT)
	{
		m_dwUpgradeResult = 0;
		pGame->m_csStatus.SetUpgrading( FALSE );

		return S_OK ;
	}
	if( pGame->m_csStatus.GetIsUpgrading() )
		return S_OK;

	CRYLGameScene*	    pScene  = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
    CRYLNewTradeDlg*    pDlg    = ( CRYLNewTradeDlg* )pScene->GetNewTradeDlg() ;

	pDlg->UpdateAllRepair( pGame ) ;

	if (g_DeviceInput.InRect( GetLeft() + 9, GetTop() + 75, GetLeft() + 85, GetTop() + 177))
	{
		if (g_DeviceInput.GetIsLeftMouseDown())
		{
			if (NULL != pGame->m_lpPickItem)
			{
				if (true == pGame->m_lpPickItem->GetIsEnableUpgrade())
				{
					if (pGame->m_lpPickItem->GetIsEquip())
					{
						ItemPos pItemIndex;
						pItemIndex.m_cIndex = ExtraSpacePos::UPGRADE_EQUIPMENT_POS;
						pItemIndex.m_cPos = TakeType::TS_EXTRA;

						if (NULL == pGame->m_csStatus.m_lpUpgrade[ExtraSpacePos::UPGRADE_EQUIPMENT_POS])
						{
							// 아이템 이동
							if (pGame->m_lpPickItem->m_lpItemBase->GetPos() == pItemIndex)
							{
								pGame->m_csStatus.m_lpUpgrade[ExtraSpacePos::UPGRADE_EQUIPMENT_POS] = pGame->m_lpPickItem;
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
							pNetwork->SendSwapItem(pGame->m_lpPickItem, &pItemIndex, 
								pGame->m_csStatus.m_lpUpgrade[ExtraSpacePos::UPGRADE_EQUIPMENT_POS]);
						}
					}
				}
			} 
			else
			{
				if (pGame->m_dwKindCursor == CURSOR_NORMAL && pGame->m_csStatus.m_lpUpgrade[ExtraSpacePos::UPGRADE_EQUIPMENT_POS])
				{
					pGame->SetPickItem(pGame->m_csStatus.m_lpUpgrade[ExtraSpacePos::UPGRADE_EQUIPMENT_POS]);
					pGame->m_csStatus.m_lpUpgrade[ExtraSpacePos::UPGRADE_EQUIPMENT_POS] = NULL;
				}
			}

			return S_OK;
		}
	} 
	else if (g_DeviceInput.InRect( GetLeft() + 94, GetTop() + 75, GetLeft() + 118, GetTop() + 99 ) )
	{
		if (g_DeviceInput.GetIsLeftMouseDown())
		{
			if (pGame->m_lpPickItem)	// 손에 있을시
			{
				if (pGame->m_lpPickItem->GetItemType() == ItemType::ORE)
				{
					if (pGame->m_csStatus.m_lpUpgrade[ExtraSpacePos::UPGRADE_MATERIAL_POS])
					{
						if (pGame->m_lpPickItem->GetProtoTypeID() == pGame->m_csStatus.m_lpUpgrade[ExtraSpacePos::UPGRADE_MATERIAL_POS]->GetProtoTypeID())
						{
							unsigned char cNum = pGame->m_csStatus.m_lpUpgrade[ExtraSpacePos::UPGRADE_MATERIAL_POS]->GetMaxDurability() - 
								pGame->m_csStatus.m_lpUpgrade[ExtraSpacePos::UPGRADE_MATERIAL_POS]->GetNowDurability();
							if (cNum >= pGame->m_lpPickItem->GetNowDurability())
							{
								cNum = pGame->m_lpPickItem->GetNowDurability();
							}

							pNetwork->SendTakeItem(pGame->m_lpPickItem, 
								&pGame->m_csStatus.m_lpUpgrade[ExtraSpacePos::UPGRADE_MATERIAL_POS]->m_lpItemBase->GetPos(), cNum);
							return S_OK;
						} 
						else	// 다른 광물
						{
							ItemPos pItemIndex;
							pItemIndex.m_cIndex = ExtraSpacePos::UPGRADE_MATERIAL_POS;
							pItemIndex.m_cPos = TakeType::TS_EXTRA;

							pNetwork->SendSwapItem(pGame->m_lpPickItem, &pItemIndex, pGame->m_csStatus.m_lpUpgrade[ExtraSpacePos::UPGRADE_MATERIAL_POS]);

							return S_OK;
						}
					} 
					else
					{
						CItemInstance *lpExistItem = pGame->m_csStatus.GetInventoryItem(pGame->m_lpPickItem->m_lpItemBase->GetPos());

						ItemPos pItemIndex;
						pItemIndex.m_cIndex = ExtraSpacePos::UPGRADE_MATERIAL_POS;
						pItemIndex.m_cPos = TakeType::TS_EXTRA;

						if (pGame->m_lpPickItem->m_lpItemBase->GetPos() == pItemIndex)
						{
							pGame->m_csStatus.m_lpUpgrade[ExtraSpacePos::UPGRADE_MATERIAL_POS] = pGame->m_lpPickItem;
							pGame->SetPickItem(NULL);

							return S_OK;
						}
						
						if (lpExistItem)
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
				if (pGame->m_dwKindCursor == CURSOR_NORMAL && pGame->m_csStatus.m_lpUpgrade[ExtraSpacePos::UPGRADE_MATERIAL_POS])
				{
					pGame->SetPickItem(pGame->m_csStatus.m_lpUpgrade[ExtraSpacePos::UPGRADE_MATERIAL_POS]);
					pGame->m_csStatus.m_lpUpgrade[ExtraSpacePos::UPGRADE_MATERIAL_POS] = NULL;
				}
			}
		}
	}

	if (g_DeviceInput.GetIsRightMouseDown() && pGame->m_dwKindCursor != CURSOR_NORMAL)
	{
		pGame->m_dwKindCursor = CURSOR_NORMAL;
	}

	if (NULL != pGame->m_csStatus.m_lpUpgrade[ExtraSpacePos::UPGRADE_EQUIPMENT_POS] && 
		true == pGame->m_csStatus.m_lpUpgrade[ExtraSpacePos::UPGRADE_EQUIPMENT_POS]->GetIsEnableUpgrade())
	{
		m_dwUpgradeGold = pGame->m_csStatus.m_lpUpgrade[ExtraSpacePos::UPGRADE_EQUIPMENT_POS]->m_lpItemBase->GetUpgradePrice();

		// 성에 속한 NPC 라면 제련 세율 적용
		unsigned long dwCastleRepairTaxGold = 0;
		NPCNode* pNpcNode = g_NPCList.GetNPCNode( pGame->m_dwTradingNpcID );
		if ( pNpcNode && pNpcNode->m_bBelongToCastle )
		{
			CRYLCastle* lpCastle = CRYLCastleManager::Instance()->FindCastleByNameID( pNpcNode->m_dwTownOrNameID );
			if ( lpCastle )
			{
				m_dwUpgradeGold += static_cast<unsigned long>( m_dwUpgradeGold * ( lpCastle->GetTax(Castle::RESMELT_TAX) / 100.f ) );
			}
		}
	} 
	else
	{
		m_dwUpgradeGold = 0;
	}
	
	return S_OK ;
}

VOID	CRYLBlacksmithDlg::ReinforcementButton() 
{
	CRYLGameData* pGame = CRYLGameData::Instance() ;

	if (NULL == pGame->m_csStatus.m_lpUpgrade[ExtraSpacePos::UPGRADE_EQUIPMENT_POS])
	{
		// 장비가 없다.
		CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
		lpMessageBox->Create(CRYLStringTable::m_strString[264]);
		return;
	} 

	if (NULL == pGame->m_csStatus.m_lpUpgrade[ExtraSpacePos::UPGRADE_MATERIAL_POS])
	{
		// 금속이 없다.
		CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
		lpMessageBox->Create(CRYLStringTable::m_strString[265]);
		return;
	} 

	if (pGame->m_csStatus.m_Info.Gold < m_dwUpgradeGold)
	{
		// 돈이 없다.
		CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
		lpMessageBox->Create(CRYLStringTable::m_strString[266]);
		return;
	} 

	if (false == pGame->m_csStatus.m_lpUpgrade[ExtraSpacePos::UPGRADE_EQUIPMENT_POS]->GetIsEnableUpgrade())
	{
		// 강화 할 수 있는 아이템이 아니다.
		CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
		
		if (pGame->m_csStatus.m_Info.Race == CClass::RaceType::HUMAN)
		{
			lpMessageBox->Create(CRYLStringTable::m_strString[267]);
			return;
		} 
		
		if (pGame->m_csStatus.m_Info.Race == CClass::RaceType::AKHAN)
		{
			lpMessageBox->Create(CRYLStringTable::m_strString[268]);
			return;
		} 

		lpMessageBox->Create(CRYLStringTable::m_strString[269]);
		return;
	} 

	CEquipment* lpEquip = reinterpret_cast<CEquipment *>(pGame->m_csStatus.m_lpUpgrade[ExtraSpacePos::UPGRADE_EQUIPMENT_POS]->m_lpItemBase);

	if(EtcItemID::ENDURANCESTONE != pGame->m_csStatus.m_lpUpgrade[ExtraSpacePos::UPGRADE_MATERIAL_POS]->GetProtoTypeID() &&
		EtcItemID::AWAKENCORE != pGame->m_csStatus.m_lpUpgrade[ExtraSpacePos::UPGRADE_MATERIAL_POS]->GetProtoTypeID() )
	{
		if (lpEquip->GetUpgradeLevel() >= EquipmentInfo::MAX_UPGRADE_LEVEL)
		{
			// 더 이상 강화 하실 수 없습니다.
			CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
			lpMessageBox->Create( CRYLStringTable::m_strString[630]);
			return;
		} 
	}

	if (EtcItemID::SILVIN == pGame->m_csStatus.m_lpUpgrade[ExtraSpacePos::UPGRADE_MATERIAL_POS]->GetProtoTypeID()		|| 
		EtcItemID::MITHRIL == pGame->m_csStatus.m_lpUpgrade[ExtraSpacePos::UPGRADE_MATERIAL_POS]->GetProtoTypeID()		|| 
		EtcItemID::ITERNIUM == pGame->m_csStatus.m_lpUpgrade[ExtraSpacePos::UPGRADE_MATERIAL_POS]->GetProtoTypeID()		|| 
	//	EtcItemID::SOULMETAL == pGame->m_csStatus.m_lpUpgrade[ExtraSpacePos::UPGRADE_MATERIAL_POS]->GetProtoTypeID()		|| 
		EtcItemID::ANTONIUM == pGame->m_csStatus.m_lpUpgrade[ExtraSpacePos::UPGRADE_MATERIAL_POS]->GetProtoTypeID()	||
		EtcItemID::ANCIENTMETAL == pGame->m_csStatus.m_lpUpgrade[ExtraSpacePos::UPGRADE_MATERIAL_POS]->GetProtoTypeID() || 		
		EtcItemID::ENDURANCESTONE == pGame->m_csStatus.m_lpUpgrade[ExtraSpacePos::UPGRADE_MATERIAL_POS]->GetProtoTypeID() ||		
		EtcItemID::AWAKENCORE == pGame->m_csStatus.m_lpUpgrade[ExtraSpacePos::UPGRADE_MATERIAL_POS]->GetProtoTypeID())		
	{
		// 고대의 돌 검사.
		if(EtcItemID::ANCIENTMETAL == pGame->m_csStatus.m_lpUpgrade[ExtraSpacePos::UPGRADE_MATERIAL_POS]->GetProtoTypeID())
		{
			if(lpEquip->GetUpgradeCnt()==Item::MAX_UPGRADE_COUNT)
			{
				// 재련을 할 수 없다.
				CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[3817]);
				return;
			}
		}

		// 광물을 검색해서 1개 이상 일때만 처리하게 한다.
		if (pGame->m_csStatus.m_lpUpgrade[ExtraSpacePos::UPGRADE_MATERIAL_POS]->GetNowDurability() >= 1)
		{
			char szMessage[MAX_PATH] = { 0, };

			unsigned char cType = 0;
			unsigned char cIndex = 0;
			for (; cIndex < Item::Attribute::MAX_ATTRIBUTE_NUM; ++cIndex)
			{
				if (true == lpEquip->CheckAttributeLimit(static_cast<Item::Attribute::Type>(cIndex), cType))
				{
					sprintf(szMessage, "%s", CRYLStringTable::m_strString[4223]);
					break;
				}
			}

			if (cIndex == Item::Attribute::MAX_ATTRIBUTE_NUM)
			{
				strncpy(szMessage, CRYLStringTable::m_strString[635], MAX_PATH);
			}

			// 여기부터 업그레이드 상태이다.
			pGame->m_csStatus.SetUpgrading( TRUE );

			CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
			lpMessageBox->Create(szMessage, MB_YES | MB_NO | MB_EXIT);
			lpMessageBox->SetResult(&m_dwUpgradeResult);
	
			m_dwUpgradeResult = 0;
		}
	} 
	else
	{
		// 틀린 광물입니다.
		CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
		lpMessageBox->Create(CRYLStringTable::m_strString[631]);
	}
}

VOID	CRYLBlacksmithDlg::RepairButton() 		
{
	CRYLGameData* pGame			= CRYLGameData::Instance() ;

	if(CRYLGameData::Instance()->m_bGuildSafeResult)
	{
		pGame->m_dwKindCursor = CURSOR_NORMAL;
		return;
	}

	// 수리하기
	if (pGame->m_dwKindCursor != CURSOR_REPAIR)
		pGame->m_dwKindCursor = CURSOR_REPAIR;
	else
		pGame->m_dwKindCursor = CURSOR_NORMAL;
}

VOID	CRYLBlacksmithDlg::AllRepairButton() 
{
	if(CRYLGameData::Instance()->m_bGuildSafeResult)
		return;

	CRYLGameScene*	    pScene  = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
    CRYLNewTradeDlg*    pDlg    = ( CRYLNewTradeDlg* )pScene->GetNewTradeDlg() ;

	pDlg->AllRepair() ;
}

VOID	CRYLBlacksmithDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y ) 
{
	if ( !g_pBlacksmithDlg )
		return ;

	CRYLGameData* pGame			= CRYLGameData::Instance() ;
	if( pGame->m_csStatus.GetIsUpgrading() )
		return;

	switch( Sender->GetInstance() )
	{
	case RYLBLACKSMITHDLG::REINFORCEMENT_BUTTON	:
		g_pBlacksmithDlg->ReinforcementButton() ;
		break ;
	case RYLBLACKSMITHDLG::REPAIR_BUTTON :
		g_pBlacksmithDlg->RepairButton() ;
		break ;
	case RYLBLACKSMITHDLG::ALLREPAIR_BUTTON :
		g_pBlacksmithDlg->AllRepairButton() ;
		break ;
	}
}
