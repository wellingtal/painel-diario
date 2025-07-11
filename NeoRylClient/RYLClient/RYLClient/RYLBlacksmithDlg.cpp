
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

	m_pReinforcementButton		= NULL ;	// ��ȭ
	m_pRepairButton				= NULL ;	// �����ϱ�
	m_pAllRepairButton			= NULL ;	// ��� �����ϱ�
	m_pRefineryLabel			= NULL ;	// ����
	m_pReinforcementWeaponLabel	= NULL ;	// ��ȭ��ų ���
	m_pMetalLabel				= NULL ;	// �ݼ�
	m_pGoldLabel				= NULL ;	// ���

	m_pResmeltTax				= NULL ;	// ���� ����
	m_pRepairTax				= NULL ;	// ���� ����
	m_pResmeltTaxValue			= NULL ;	// ���� ���� ��
	m_pRepairTaxValue			= NULL ;	// ���� ���� ��


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

// CASTLE_TODO : ������ ���ݰ��� UI�� ���Ƶд�.

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

	// ��ȭ
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

	// �����ϱ�
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

	// ��� �����ϱ�
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

	// ����
	m_pRefineryLabel = new CRYLLabel( this ) ;
	m_pRefineryLabel->SetLeft( 18 ) ;
	m_pRefineryLabel->SetTop( 20 ) ;
	m_pRefineryLabel->SetFontColor( 0xffffffff ) ;
	m_pRefineryLabel->SetAutoSize( TRUE ) ;
	m_pRefineryLabel->SetCaption( CRYLStringTable::m_strString[ 2089 ] ) ;
	AttachGMUIControl( m_pRefineryLabel ) ;

	// ��ȭ��ų ���
	m_pReinforcementWeaponLabel = new CRYLLabel( this ) ;
	m_pReinforcementWeaponLabel->SetLeft( 11 ) ;
	m_pReinforcementWeaponLabel->SetTop( 56 ) ;
	m_pReinforcementWeaponLabel->SetFontColor( 0xffffffff ) ;
	m_pReinforcementWeaponLabel->SetAutoSize( TRUE ) ;
	m_pReinforcementWeaponLabel->SetCaption( CRYLStringTable::m_strString[ 2090 ] ) ;
	AttachGMUIControl( m_pReinforcementWeaponLabel ) ;

	// �ݼ�
	m_pMetalLabel = new CRYLLabel( this ) ;
	m_pMetalLabel->SetLeft( 94 ) ;
	m_pMetalLabel->SetTop( 56 ) ;
	m_pMetalLabel->SetFontColor( 0xffffffff ) ;
	m_pMetalLabel->SetAutoSize( TRUE ) ;
	m_pMetalLabel->SetCaption( CRYLStringTable::m_strString[ 2091 ] ) ;
	AttachGMUIControl( m_pMetalLabel ) ;
	
	// ���
	m_pGoldLabel = new CRYLLabel( this ) ;
	m_pGoldLabel->SetLeft( 157 ) ;
	m_pGoldLabel->SetTop( 78 ) ;
	m_pGoldLabel->SetFontColor( 0xffffffff ) ;
	m_pGoldLabel->SetAutoSize( TRUE ) ;
	AttachGMUIControl( m_pGoldLabel ) ;

// CASTLE_TODO : ������ ���ݰ��� UI�� ���Ƶд�.

//	if ( true == CRYLNetworkData::Instance()->UseContents(GameRYL::SIEGE) )
//	{
//		// ���� ����
//		m_pResmeltTax = new CRYLLabel( this ) ;
//		m_pResmeltTax->SetLeft( 94 ) ;
//		m_pResmeltTax->SetTop( 110 ) ;
//		m_pResmeltTax->SetFontColor( 0xFFFFFFFF ) ;
//		m_pResmeltTax->SetAutoSize( TRUE ) ;
//		m_pResmeltTax->SetCaption( CRYLStringTable::m_strString[ 3867 ] ) ;
//		AttachGMUIControl( m_pResmeltTax ) ;
//
//		// ���� ����
//		m_pRepairTax = new CRYLLabel( this ) ;
//		m_pRepairTax->SetLeft( 94 ) ;
//		m_pRepairTax->SetTop( 134 ) ;
//		m_pRepairTax->SetFontColor( 0xFFFFFFFF ) ;
//		m_pRepairTax->SetAutoSize( TRUE ) ;
//		m_pRepairTax->SetCaption( CRYLStringTable::m_strString[ 3868 ] ) ;
//		AttachGMUIControl( m_pRepairTax ) ;
//
//		// ���� ���� ��
//		m_pResmeltTaxValue = new CRYLLabel( this ) ;
//		SetRect( &rt, 153, 110, 188, 120 ) ;
//		m_pResmeltTaxValue->SetAlignRect( rt ) ;
//		m_pResmeltTaxValue->SetAlign( CENTER ) ;
//		m_pResmeltTaxValue->SetCaption( "---" ) ;
//		m_pResmeltTaxValue->SetFontColor( 0xFFFFFFFF ) ;
//		m_pResmeltTaxValue->SetAutoSize( TRUE ) ;
//		AttachGMUIControl( m_pResmeltTaxValue ) ;
//
//		// ���� ���� ��
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
	GM_DELETE( m_pReinforcementButton  ) ;			// ��ȭ
	GM_DELETE( m_pRepairButton ) ;					// �����ϱ�
	GM_DELETE( m_pAllRepairButton ) ;				// ��� �����ϱ�
	GM_DELETE( m_pRefineryLabel ) ;					// ����
	GM_DELETE( m_pReinforcementWeaponLabel ) ;		// ��ȭ��ų ���
	GM_DELETE( m_pMetalLabel ) ;					// �ݼ�
	GM_DELETE( m_pGoldLabel ) ;						// ���
	GM_DELETE( m_pResmeltTax ) ;					// ���� ����
	GM_DELETE( m_pRepairTax ) ;						// ���� ����
	GM_DELETE( m_pResmeltTaxValue ) ;				// ���� ���� ��
	GM_DELETE( m_pRepairTaxValue ) ;				// ���� ���� ��
}

VOID	CRYLBlacksmithDlg::Show( CGMUIObject* Sender ) 
{
	// �κ�â�� �Բ� ����.
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

// CASTLE_TODO : ������ ���ݰ��� UI�� ���Ƶд�.

	// ���� ���� NPC �� ��� ���� ǥ��
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
		// �������� �� ����Ʈ�� ������ ��Ŷ�� ���۵ȴ� =+=
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
							// ������ �̵�
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
							// �����ϱ�
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
			if (pGame->m_lpPickItem)	// �տ� ������
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
						else	// �ٸ� ����
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
				// ������ �ݴ´�.
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

		// ���� ���� NPC ��� ���� ���� ����
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
		// ��� ����.
		CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
		lpMessageBox->Create(CRYLStringTable::m_strString[264]);
		return;
	} 

	if (NULL == pGame->m_csStatus.m_lpUpgrade[ExtraSpacePos::UPGRADE_MATERIAL_POS])
	{
		// �ݼ��� ����.
		CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
		lpMessageBox->Create(CRYLStringTable::m_strString[265]);
		return;
	} 

	if (pGame->m_csStatus.m_Info.Gold < m_dwUpgradeGold)
	{
		// ���� ����.
		CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
		lpMessageBox->Create(CRYLStringTable::m_strString[266]);
		return;
	} 

	if (false == pGame->m_csStatus.m_lpUpgrade[ExtraSpacePos::UPGRADE_EQUIPMENT_POS]->GetIsEnableUpgrade())
	{
		// ��ȭ �� �� �ִ� �������� �ƴϴ�.
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
			// �� �̻� ��ȭ �Ͻ� �� �����ϴ�.
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
		// ����� �� �˻�.
		if(EtcItemID::ANCIENTMETAL == pGame->m_csStatus.m_lpUpgrade[ExtraSpacePos::UPGRADE_MATERIAL_POS]->GetProtoTypeID())
		{
			if(lpEquip->GetUpgradeCnt()==Item::MAX_UPGRADE_COUNT)
			{
				// ����� �� �� ����.
				CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[3817]);
				return;
			}
		}

		// ������ �˻��ؼ� 1�� �̻� �϶��� ó���ϰ� �Ѵ�.
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

			// ������� ���׷��̵� �����̴�.
			pGame->m_csStatus.SetUpgrading( TRUE );

			CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
			lpMessageBox->Create(szMessage, MB_YES | MB_NO | MB_EXIT);
			lpMessageBox->SetResult(&m_dwUpgradeResult);
	
			m_dwUpgradeResult = 0;
		}
	} 
	else
	{
		// Ʋ�� �����Դϴ�.
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

	// �����ϱ�
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
