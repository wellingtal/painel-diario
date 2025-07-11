
#include <winsock2.h>

#include <Skill/SkillMgr.h>
#include <Network/ClientSocket/Send/SendItem.h>
#include <Network/Packet/PacketStruct/CastlePacket.h>

#include "RYLClientMain.h"
#include "RYLRaceBase.h"
#include "RYLGameData.h"
#include "RYLNetworkData.h"
#include "RYLGameScene.h"
#include "RYLSceneManager.h"
#include "RYLGameTimeManager.h"

#include "WinInput.h"
#include "BaseGraphicsLayer.h"
#include "GMImageDib.h"

#include "RYLButton.h"
#include "RYLImageManager.h"
#include "RYLImageCabinet.h"
#include "RYLImage.h"
#include "RYLSpriteList.h"
#include "RYLEdit.h"
#include "RYLStringTable.h"
#include "RYLCheckBox.h"
#include "RYLMessageBoxManager.h"

#include "RYLVaultPasswordDlg.h"
#include "RYLChattingDlg.h"
#include "RYLCampShopDlg.h"
#include "RYLStreetStallDlg.h"
#include "RYLWorldWeaponLaunchDlg.h"
#include "RYLInventoryPanel.h"
#include "RYLQuickSlotDlg.h"
#include "GMMemory.h"

#include "RYLJoystickConfigurePanel.h"

CRYLQuickSlotDlg*	g_pQuickSlotDlg = NULL ;

CRYLQuickSlotDlg::CRYLQuickSlotDlg( INT nLeft, INT nTop, BOOL bVert ) : CRYLDialog()
{
	RECT rt ;
	CGMImageDib* pImageDIB ;
	CRYLSpriteList::Instance()->CreateGMImage( 317, 41, &pImageDIB ) ;

	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 215, 27, 256 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI_256x256 ) ) ;
	SetRect( &rt, 28, 215, 56, 256 ) ;
	pImageDIB->DrawRect( &rt, 27, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI_256x256 ) ) ;
	pImageDIB->DrawRect( &rt, 54, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI_256x256 ) ) ;
	pImageDIB->DrawRect( &rt, 81, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI_256x256 ) ) ;
	pImageDIB->DrawRect( &rt, 108, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI_256x256 ) ) ;
	pImageDIB->DrawRect( &rt, 135, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI_256x256 ) ) ;
	pImageDIB->DrawRect( &rt, 162, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI_256x256 ) ) ;
	pImageDIB->DrawRect( &rt, 189, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI_256x256 ) ) ;
	pImageDIB->DrawRect( &rt, 216, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI_256x256 ) ) ;
	pImageDIB->DrawRect( &rt, 243, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI_256x256 ) ) ;
	SetRect( &rt, 56, 215, 102, 256 ) ;
	pImageDIB->DrawRect( &rt, 271, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI_256x256 ) ) ;
	//-------------------------------------------------------------
	// 숫자
	SetRect( &rt, 2, 82, 4, 90 ) ;
	pImageDIB->DrawRect( &rt, 6, 4, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI_256x256 ) ) ;
	SetRect( &rt, 9, 82, 15, 90 ) ;
	pImageDIB->DrawRect( &rt, 31, 4, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI_256x256 ) ) ;
	SetRect( &rt, 17, 82, 22, 90 ) ;
	pImageDIB->DrawRect( &rt, 59, 4, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI_256x256 ) ) ;
	SetRect( &rt, 25, 82, 31, 90 ) ;
	pImageDIB->DrawRect( &rt, 85, 4, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI_256x256 ) ) ;
	SetRect( &rt, 34, 82, 39, 90 ) ;
	pImageDIB->DrawRect( &rt, 113, 4, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI_256x256 ) ) ;
	SetRect( &rt, 41, 82, 46, 90 ) ;
	pImageDIB->DrawRect( &rt, 139, 4, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI_256x256 ) ) ;
	SetRect( &rt, 50, 82, 54, 90 ) ;
	pImageDIB->DrawRect( &rt, 167, 4, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI_256x256 ) ) ;
	SetRect( &rt, 57, 82, 62, 90 ) ;
	pImageDIB->DrawRect( &rt, 194, 4, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI_256x256 ) ) ;
	SetRect( &rt, 65, 82, 70, 90 ) ;
	pImageDIB->DrawRect( &rt, 221, 4, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI_256x256 ) ) ;
	SetRect( &rt, 74, 82, 79, 90 ) ;
	pImageDIB->DrawRect( &rt, 248, 4, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI_256x256 ) ) ;
    
	m_pHorzImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	m_pHorzImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( m_pHorzImage ) ;
	AttachWindowTexture( ( CGMTexture*)m_pHorzImage ) ;
	GM_DELETE( pImageDIB ) ;

	//-------------------------------------------------------------------------------------------------------
	// 세로
	CRYLSpriteList::Instance()->CreateGMImage( 41, 329, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 125, 11, 166, 124 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI02_256x256 ) ) ;
	SetRect( &rt, 125, 95, 166, 124 ) ;
	pImageDIB->DrawRect( &rt, 0, 111, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI02_256x256 ) ) ;
	pImageDIB->DrawRect( &rt, 0, 138, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI02_256x256 ) ) ;
	pImageDIB->DrawRect( &rt, 0, 165, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI02_256x256 ) ) ;
	pImageDIB->DrawRect( &rt, 0, 192, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI02_256x256 ) ) ;
	pImageDIB->DrawRect( &rt, 0, 219, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI02_256x256 ) ) ;
	pImageDIB->DrawRect( &rt, 0, 246, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI02_256x256 ) ) ;
	pImageDIB->DrawRect( &rt, 0, 273, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI02_256x256 ) ) ;
	pImageDIB->DrawRect( &rt, 0, 300, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI02_256x256 ) ) ;
	//-------------------------------------------------------------
	// 숫자
	SetRect( &rt, 2, 82, 4, 90 ) ;
	pImageDIB->DrawRect( &rt, 6, 60, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI_256x256 ) ) ;
	SetRect( &rt, 9, 82, 15, 90 ) ;
	pImageDIB->DrawRect( &rt, 5, 87, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI_256x256 ) ) ;
	SetRect( &rt, 17, 82, 22, 90 ) ;
	pImageDIB->DrawRect( &rt, 5, 114, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI_256x256 ) ) ;
	SetRect( &rt, 25, 82, 31, 90 ) ;
	pImageDIB->DrawRect( &rt, 5, 141, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI_256x256 ) ) ;
	SetRect( &rt, 34, 82, 39, 90 ) ;
	pImageDIB->DrawRect( &rt, 5, 169, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI_256x256 ) ) ;
	SetRect( &rt, 41, 82, 46, 90 ) ;
	pImageDIB->DrawRect( &rt, 5, 195, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI_256x256 ) ) ;
	SetRect( &rt, 50, 82, 54, 90 ) ;
	pImageDIB->DrawRect( &rt, 6, 222, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI_256x256 ) ) ;
	SetRect( &rt, 57, 82, 62, 90 ) ;
	pImageDIB->DrawRect( &rt, 5, 249, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI_256x256 ) ) ;
	SetRect( &rt, 65, 82, 70, 90 ) ;
	pImageDIB->DrawRect( &rt, 5, 276, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI_256x256 ) ) ;
	SetRect( &rt, 74, 82, 79, 90 ) ;
	pImageDIB->DrawRect( &rt, 6, 303, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI_256x256 ) ) ;

	m_pVertImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	m_pVertImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( m_pVertImage ) ;
	GM_DELETE( pImageDIB ) ;
	
/*
	// 퀵슬롯 hide 시키기 위해서 만든 이미지
	CRYLSpriteList::Instance()->CreateGMImage( 24, 24, &pImageDIB ) ;
	pImageDIB->ClearbyColor( D3DCOLOR_ARGB(200,200,200,200) ) ;
	m_pHideImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	m_pHideImage->LoadFromImage( pImageDIB ) ;
	GM_DELETE( pImageDIB ) ;
*/



	SetWidth( 317 ) ;
	SetHeight( 41 ) ;
	
	SetLeft( nLeft ) ;
	SetTop( nTop ) ;
	
	SetFormType( RYLDLG_TYPE_NONE ) ;
	
	g_pQuickSlotDlg = this ;	
	InitCtrl() ;

	SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 1783 ], 1783, TOOLTIP_DLG ) ;

	m_bVertEnable = FALSE ;
	if ( bVert ) RotationDlg() ;
}

CRYLQuickSlotDlg::~CRYLQuickSlotDlg()
{
	FinalCtrl() ;
}

VOID	CRYLQuickSlotDlg::InitCtrl ()
{
	RECT rt ;
	CGMImageDib* pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI_256x256 ) ;
	
	// 회전 버튼
	m_pRotationButton = new CRYLButton( this ) ;
	m_pRotationButton->SetLeft( 292 ) ;
	m_pRotationButton->SetTop( 3 ) ;
	m_pRotationButton->OnClickEvent = DlgClick ;
	m_pRotationButton->SetInstance( RYLQUICKSLOTDLG::ROTATION_BUTTON ) ;
	SetRect( &rt, 212, 0, 222, 9 ) ;
	m_pRotationButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 212, 18, 222, 27 ) ;
	m_pRotationButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 212, 9, 222, 18 ) ;
	m_pRotationButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 212, 9, 222, 18 ) ;
	m_pRotationButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pRotationButton->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 1870], 1870 ) ;
	AttachGMUIControl( m_pRotationButton ) ;

	// Hide 버튼
	m_pHideButton = new CRYLButton( this ) ;
	m_pHideButton->SetLeft( 304 ) ;
	m_pHideButton->SetTop( 3 ) ;
	m_pHideButton->OnClickEvent = DlgClick ;
	m_pHideButton->SetInstance( RYLQUICKSLOTDLG::HIDE_BUTTON ) ;
	SetRect( &rt, 202, 0, 213, 9 ) ;
	m_pHideButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 202, 18, 213, 27 ) ;
	m_pHideButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 202, 9, 213, 18 ) ;
	m_pHideButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 202, 9, 213, 18 ) ;
	m_pHideButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pHideButton->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 1868], 1868 ) ;
	AttachGMUIControl( m_pHideButton ) ;

	// Z 버튼
	m_pZCheckBox = new CRYLCheckBox( this ) ;
	m_pZCheckBox->SetLeft( 272 ) ;
	m_pZCheckBox->SetTop( 12 ) ;
	m_pZCheckBox->SetInstance( RYLQUICKSLOTDLG::Z_CHECKBOX ) ;
	m_pZCheckBox->OnClickEvent = DlgClick ;
	SetRect( &rt, 102, 214, 116, 227 ) ;
	m_pZCheckBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 102, 227, 116, 240 ) ;
	m_pZCheckBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 102, 240, 116, 253 ) ;
	m_pZCheckBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 102, 214, 116, 227 ) ;
	m_pZCheckBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pZCheckBox->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 2225 ], 2225 ) ;
	AttachGMUIControl( m_pZCheckBox ) ;

	// X 버튼
	m_pXCheckBox = new CRYLCheckBox( this ) ;
	m_pXCheckBox->SetLeft( 272 ) ;
	m_pXCheckBox->SetTop( 26 ) ;
	m_pXCheckBox->SetInstance( RYLQUICKSLOTDLG::X_CHECKBOX ) ;
	m_pXCheckBox->OnClickEvent = DlgClick ;
	SetRect( &rt, 116, 214, 130, 227 ) ;
	m_pXCheckBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 116, 227, 130, 240 ) ;
	m_pXCheckBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 116, 240, 130, 253 ) ;
	m_pXCheckBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 116, 214, 130, 227 ) ;
	m_pXCheckBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	m_pXCheckBox->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 2226 ], 2226 ) ;
	AttachGMUIControl( m_pXCheckBox ) ;

	// Z 버튼 활성화 상태로 초기화
	ZButton() ;
}

VOID	CRYLQuickSlotDlg::FinalCtrl ()
{
	GM_DELETE( m_pRotationButton ) ;		// 회전 버튼
	GM_DELETE( m_pHideButton ) ;			// Hide 버튼
	GM_DELETE( m_pZCheckBox ) ;				// Z 버튼
	GM_DELETE( m_pXCheckBox ) ;				// X 버튼
}

VOID	CRYLQuickSlotDlg::Show( CGMUIObject* Sender )
{
	
}
	
VOID	CRYLQuickSlotDlg::Hide( CGMUIObject* Sender )
{
	
}

VOID	CRYLQuickSlotDlg::GMUIPaint( INT xx, INT yy )
{
	CRYLDialog::GMUIPaint( xx, yy ) ;

	LPDIRECT3DDEVICE8   lpD3DDevice = BaseGraphicsLayer::GetDevice() ;

    RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
    if ( !pSelfCreature )
        return ;

	CRYLGameData* pGame = CRYLGameData::Instance() ;
	for (int i = 0; i < MAX_QUICK_SLOT_NUM; ++i)
	{
		if (pGame->m_csStatus.m_lstQuickSlot[pGame->m_dwStartQuickSlot + i])
		{
			if (pGame->m_csStatus.m_lstQuickSlot[pGame->m_dwStartQuickSlot + i]->m_bSkill)
			{
				if ( !m_bVertEnable )
				{
					pGame->m_csStatus.m_lstQuickSlot[pGame->m_dwStartQuickSlot + i]->m_wPutX = GetLeft() + 3 + i * 27 ; 
					pGame->m_csStatus.m_lstQuickSlot[pGame->m_dwStartQuickSlot + i]->m_wPutY = GetTop() + 14 ;
				}
				else
				{
					pGame->m_csStatus.m_lstQuickSlot[pGame->m_dwStartQuickSlot + i]->m_wPutX = GetLeft() + 14 ; 
					pGame->m_csStatus.m_lstQuickSlot[pGame->m_dwStartQuickSlot + i]->m_wPutY = GetTop() + 60 + i * 27 ;
				}

				// edith 2008.01.21 퀵스킬 거리가 안되면 디세이블 이미지 출력
				// 스킬일때 Enable 처리한다.
				pGame->m_csStatus.m_lstQuickSlot[pGame->m_dwStartQuickSlot + i]->SetSkillEnable(pGame->m_csStatus.m_bQuickSlotLength[pGame->m_dwStartQuickSlot + i]);
			} 
			else
			{
				if ( !m_bVertEnable )
				{
					pGame->m_csStatus.m_lstQuickSlot[pGame->m_dwStartQuickSlot + i]->m_wPutX = GetLeft() + 3 + i * 27 + 12 - pGame->m_csStatus.m_lstQuickSlot[pGame->m_dwStartQuickSlot + i]->m_lpSprite->GetSizeX() / 2;
					pGame->m_csStatus.m_lstQuickSlot[pGame->m_dwStartQuickSlot + i]->m_wPutY = GetTop() + 14 + 10 - pGame->m_csStatus.m_lstQuickSlot[pGame->m_dwStartQuickSlot + i]->m_lpSprite->GetSizeY() / 2;
				}
				else
				{
					pGame->m_csStatus.m_lstQuickSlot[pGame->m_dwStartQuickSlot + i]->m_wPutX = GetLeft() + 14 + 10 - pGame->m_csStatus.m_lstQuickSlot[pGame->m_dwStartQuickSlot + i]->m_lpSprite->GetSizeX() / 2;
					pGame->m_csStatus.m_lstQuickSlot[pGame->m_dwStartQuickSlot + i]->m_wPutY = GetTop() + 60 + i * 27 + 12 - pGame->m_csStatus.m_lstQuickSlot[pGame->m_dwStartQuickSlot + i]->m_lpSprite->GetSizeY() / 2;
				}
			}

			pGame->m_csStatus.m_lstQuickSlot[pGame->m_dwStartQuickSlot + i]->Render(lpD3DDevice);

			if(pGame->m_csStatus.m_lstQuickSlot[pGame->m_dwStartQuickSlot + i]->GetSkillType() == Skill::Type::CHANT)	// CHANT 일때
			{
				unsigned short usSlotSkillID = pGame->m_csStatus.m_lstQuickSlot[pGame->m_dwStartQuickSlot + i]->GetSkillID();
				unsigned short usSkillID = pGame->m_csStatus.m_wUseChantSkillID;
				// 퀵슬롯에 있는 스킬 아이디와 내가 사용한 챈트 스킬 아이디가 같을때만 확인한다.
				if(usSlotSkillID == usSkillID)
				{
					// edith 2008.11.10 챈트 활성화 퀵슬롯아이콘에 표시 (노란색)
					// 챈트 버프이므로 해당 챈트 버프가 걸려있는지를 확인한다.
					if ( pSelfCreature->GetEffect()->GetChantStatus( Skill::SpellID::BattleSong ) && (usSkillID == 0x8602 || usSkillID == 0x9404 ) )
					{
						pGame->m_csStatus.m_lstQuickSlot[pGame->m_dwStartQuickSlot + i]->RenderEnableChant(lpD3DDevice);
					}
					else if ( pSelfCreature->GetEffect()->GetChantStatus( Skill::SpellID::MaintenanceChant ) && (usSkillID == 0x8B02 || usSkillID == 0x9708 ) )
					{
						pGame->m_csStatus.m_lstQuickSlot[pGame->m_dwStartQuickSlot + i]->RenderEnableChant(lpD3DDevice);
					}
					else if ( pSelfCreature->GetEffect()->GetChantStatus( Skill::SpellID::AccelerationChant ) && (usSkillID == 0x8B03 || usSkillID == 0x9709 ) )
					{
						pGame->m_csStatus.m_lstQuickSlot[pGame->m_dwStartQuickSlot + i]->RenderEnableChant(lpD3DDevice);
					}
					else if ( pSelfCreature->GetEffect()->GetChantStatus( Skill::SpellID::LifeAura ) && (usSkillID == 0x8C08 || usSkillID == 0x9209 ) )
					{
						pGame->m_csStatus.m_lstQuickSlot[pGame->m_dwStartQuickSlot + i]->RenderEnableChant(lpD3DDevice);
					}
					else if ( pSelfCreature->GetEffect()->GetChantStatus( Skill::SpellID::StealHand ) && (usSkillID == 0x8709 || usSkillID == 0x9807 ) )
					{
						pGame->m_csStatus.m_lstQuickSlot[pGame->m_dwStartQuickSlot + i]->RenderEnableChant(lpD3DDevice);
					}
				}
/*
				unsigned short usSkillID = pGame->m_csStatus.m_lstQuickSlot[pGame->m_dwStartQuickSlot + i]->GetSkillID();
				if(pGame->m_csStatus.m_wUseChantSkillID == usSkillID)
				{
					pGame->m_csStatus.m_lstQuickSlot[pGame->m_dwStartQuickSlot + i]->RenderEnableChant(lpD3DDevice);
				}
*/
			}

/*
			// edith 퀵슬롯의 스킬이 거리가 안되면 디세이블 이미지를 출력한다.
			// 거리가 안되면 화면에 거리이미지를 출력해준다.
			if(!pGame->m_csStatus.m_bQuickSlotLength[pGame->m_dwStartQuickSlot + i])
			{
				int iX = pGame->m_csStatus.m_lstQuickSlot[pGame->m_dwStartQuickSlot + i]->m_wPutX;
				int iY = pGame->m_csStatus.m_lstQuickSlot[pGame->m_dwStartQuickSlot + i]->m_wPutY;
				m_pHideImage->Draw(iX, iY);
			}
*/
		}
	}

	if (pGame->m_csStatus.m_lpQuickSelected)
	{
		if ( !m_bVertEnable )
		{
			pGame->m_csStatus.m_lpQuickSelected->m_wPutX = GetLeft() + 290 ;
			pGame->m_csStatus.m_lpQuickSelected->m_wPutY = GetTop() + 14 ;
		}
		else
		{
			pGame->m_csStatus.m_lpQuickSelected->m_wPutX = GetLeft() + 14 ;
			pGame->m_csStatus.m_lpQuickSelected->m_wPutY = GetTop() + 15 ;
		}

		pGame->m_csStatus.m_lpQuickSelected->Render(lpD3DDevice);
	}
}

HRESULT		CRYLQuickSlotDlg::Update( BOOL &bClick, BOOL &bEdge ) 
{
	// 	퀵슬롯은 닫은 상태에서도 단축키의 작동을 위해 GetVisible() 검사를 하지 않는다.
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

	CRYLGameData* pGame = CRYLGameData::Instance() ;
	CRYLNetworkData* pNetwork = CRYLNetworkData::Instance() ;

	// 공성 병기일때는 퀵슬롯에 등록시키지 못하게 한다.
	if ( pGame->GetClientIdentity( pNetwork->m_dwMyChrID ) == Creature::CT_SIEGE_OBJECT )
		return S_OK ;

    RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
    if ( !pSelfCreature )
        return S_FALSE;

    if ( pSelfCreature->GetStillCasting() )
        return S_OK;
	
	CRYLGameScene*	 			pScene		= ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );
	CRYLCampShopDlg* 			pShopDlg	= ( CRYLCampShopDlg* )pScene->GetCampShopDlg();
	CRYLStreetStallDlg*			pStallDlg	= ( CRYLStreetStallDlg* )pScene->GetStreetStallDlg();
	CRYLWorldWeaponLaunchDlg*	pLaunchDlg	= ( CRYLWorldWeaponLaunchDlg* )pScene->GetWorldWeaponLaunchDlg();

	if (FALSE == CRYLChattingDlg::Instance()->m_pChatEdit->GetFocusState() &&
		FALSE == pShopDlg->GetChattingEdit()->GetFocusState() && 
		FALSE == pStallDlg->GetChattingEdit()->GetFocusState() && 
		FALSE == pLaunchDlg->GetPosXEdit()->GetFocusState() &&
		FALSE == pLaunchDlg->GetPosYEdit()->GetFocusState())
	{
		if (TRUE == g_DeviceInput.KeyDown('Z'))
		{
			ZButton() ;
		}
		else if (TRUE == g_DeviceInput.KeyDown('X'))
		{
			XButton() ;
		}
		else if (TRUE == g_DeviceInput.KeyDown(VK_OEM_3))
		{
			ToggleButton() ;
		}
	}

    if ( pSelfCreature->GetEffect()->GetChantStatus( Skill::SpellID::Stun ) )
        return S_OK;

	UINT uiLeft, uiTop, uiRight, uiBottom ;
	if ( !m_bVertEnable )
	{
		uiLeft = GetLeft() + 290 ;
		uiTop  = GetTop() + 14 ;
		uiRight= GetLeft() + 314 ;
		uiBottom = GetTop() + 38 ;
	}
	else
	{
		uiLeft = GetLeft() + 14 ;
		uiTop  = GetTop() + 15 ;
		uiRight= GetLeft() + 38 ;
		uiBottom = GetTop() + 38 ;
	}

	if (g_DeviceInput.InRect( uiLeft, uiTop, uiRight, uiBottom ) && g_DeviceInput.GetIsLeftMouseDown())
	{
		if (NULL != pGame->m_lpPickItem)
		{
			if (TRUE == pGame->m_lpPickItem->m_bSkill || 
				Item::ItemType::SKILL_ITEM == pGame->m_lpPickItem->GetItemType())
			{
				for (int i = 0; i < QUICK::MAX_QUICK_NUM; ++i)
				{
					if (NULL == pGame->m_csStatus.m_lstQuickSlot[i])
					{
						if (pGame->m_lpPickItem->m_lpItemBase->GetPos().m_cIndex == i && 
							pGame->m_lpPickItem->GetItemPos() == TakeType::TS_QSLOT)
						{
							// 바로 놓는다.
							pGame->m_csStatus.m_lstQuickSlot[i] = pGame->m_lpPickItem;
							pGame->m_csStatus.m_lstQuickSlot[i]->SetItemPos(TakeType::TS_QSLOT);
							pGame->m_csStatus.m_lstQuickSlot[i]->SetPos(i);
							pGame->SetPickItem(NULL);

							pNetwork->SetChant(pGame->m_csStatus.m_lstQuickSlot[i]);
							pGame->m_csStatus.SetActivateSkill(pGame->m_csStatus.m_lstQuickSlot[i]);
						} 
						else
						{
							// 선택에 바로 담기
							pNetwork->SendQuickSlotMove(pGame->m_lpPickItem, TakeType::TS_QSLOT, i, 0);
						}

						return S_OK ;
					}
				}
			}
		}
	}

	if ( GetVisible() )
	{
		for (int i = 0; i < MAX_QUICK_SLOT_NUM; ++i)
		{
			if ( !m_bVertEnable )
			{
				uiLeft = GetLeft() + 3 + i * 27 ;
				uiTop  = GetTop() + 14 ;
				uiRight= GetLeft() + 24+ i * 27 ;
				uiBottom = GetTop() + 40 ;
			}
			else
			{
				uiLeft = GetLeft() + 14 ;
				uiTop  = GetTop() + 60 + i * 27 ;
				uiRight= GetLeft() + 38 ;
				uiBottom = GetTop() + 60 + 24 + i * 27 ;
			}

			if (g_DeviceInput.InRect( uiLeft, uiTop, uiRight, uiBottom ))
			{
				if (g_DeviceInput.GetIsLeftMouseDown())
				{
					if (!pGame->m_csStatus.m_lstQuickSlot[pGame->m_dwStartQuickSlot + i])
					{	
						// 퀵슬롯에 아이템이 없을때
						if (pGame->m_lpPickItem)
						{
							Item::ItemPos pItemIndex;
							pItemIndex.m_cIndex = pGame->m_dwStartQuickSlot + i;
							pItemIndex.m_cPos = TakeType::TS_QSLOT;

							if (pGame->m_lpPickItem->m_lpItemBase->GetPos() == pItemIndex)
							{	
								// 그냥 내려 놓는다.
								pGame->m_csStatus.m_lstQuickSlot[pItemIndex.m_cIndex] = pGame->m_lpPickItem;
								pGame->m_csStatus.m_lstQuickSlot[pItemIndex.m_cIndex]->SetItemPos(TakeType::TS_QSLOT);
								pGame->m_csStatus.m_lstQuickSlot[pItemIndex.m_cIndex]->SetPos(pItemIndex.m_cIndex);
								pGame->SetPickItem(NULL);
							} 
							else if (pGame->m_lpPickItem->m_bSkill)
							{
								pNetwork->SendQuickSlotMove(pGame->m_lpPickItem, TakeType::TS_QSLOT, pItemIndex.m_cIndex);
							} 
							else if (pGame->m_lpPickItem->m_lpItemBase->IsSet(Item::DetailData::QUICKSLOT_IN))
							{
								pNetwork->SendQuickSlotMove(pGame->m_lpPickItem, TakeType::TS_QSLOT, pItemIndex.m_cIndex);
							}

							return S_OK;
						}
					} 
					else
					{
						if (pGame->m_lpPickItem)
						{
							if (pGame->m_lpPickItem->GetItemPos() == TakeType::TS_QSLOT)
							{
								// 스왑이 일어난다. but TakeItem으로 보낸다.
								pNetwork->SendQuickSlotMove(pGame->m_lpPickItem, TakeType::TS_QSLOT, pGame->m_dwStartQuickSlot + i);
							}

							return S_OK;
						} 
						else
						{
							pGame->SetPickItem(pGame->m_csStatus.m_lstQuickSlot[pGame->m_dwStartQuickSlot + i]);
							if (pGame->m_csStatus.m_lstQuickSlot[pGame->m_dwStartQuickSlot + i] == pGame->m_csStatus.m_lpQuickSelected)
							{
								pNetwork->SetChant(NULL);
								pGame->m_csStatus.SetActivateSkill(NULL);
							}
							pGame->m_csStatus.m_lstQuickSlot[pGame->m_dwStartQuickSlot + i] = NULL;

							CRYLGameScene* pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
							pScene->GetSkillTooltipDlg()->SetVisible( FALSE );
							pScene->GetAbilityTooltipDlg()->SetVisible( FALSE );

							return S_OK;
						}
					}
				}

				if (g_DeviceInput.GetIsRightMouseDown())
				{
                    if ( pSelfCreature->GetIsDead() )
                        continue;

					CItemInstance* lpQuickSlotItem = pGame->m_csStatus.m_lstQuickSlot[pGame->m_dwStartQuickSlot + i];

					if (NULL != lpQuickSlotItem)
					{
						if (TRUE == lpQuickSlotItem->m_bSkill || 
							Item::ItemType::SKILL_ITEM == lpQuickSlotItem->GetItemType())
						{
							if ( lpQuickSlotItem->GetSkillType() == Skill::Type::CHANT )
							{
								pNetwork->SetChant(lpQuickSlotItem);
								pGame->m_csStatus.SetActivateSkill(lpQuickSlotItem);
							}
							else
							{
								pGame->m_csStatus.SetActivateSkill(lpQuickSlotItem);
							}
						}
						else
						{
							UseItem(lpQuickSlotItem);
						}

						return S_OK;
					}
				}
			}
		}
	}

	if (g_MessageBoxManager.GetIsExistModal() || 
		pScene->GetVaultPasswordDlg()->GetVisible() || 
		pScene->GetVaultCertificationdDlg()->GetVisible() || 
		pScene->GetBattleGroundChangeMoney()->GetVisible())
	{
		return S_OK;
	}


	for ( INT iSlot = 0, iKey = 0 ; iSlot < MAX_QUICK_SLOT_NUM ; ++iSlot, ++iKey )
	{
        if ( FALSE == pSelfCreature->GetIsDead() )
		{
			INT iTmpSlotNum = pGame->m_dwStartQuickSlot + iSlot ;
			if ( iSlot == 9 )
			{
				iKey = -1 ;
			}
			
			if (FALSE == CRYLChattingDlg::Instance()->m_pChatEdit->GetFocusState() &&
				FALSE == pShopDlg->GetChattingEdit()->GetFocusState() && 
				FALSE == pStallDlg->GetChattingEdit()->GetFocusState() && 
				FALSE == pLaunchDlg->GetPosXEdit()->GetFocusState() &&
				FALSE == pLaunchDlg->GetPosYEdit()->GetFocusState())
			{
				if ( FALSE == g_DeviceInput.KeyHold( VK_SHIFT ) && 
					(TRUE == g_DeviceInput.KeyDown( '1' + iKey ) || KeyControl::IsKeyPress(KeyControl::KEY_COLUM_01+iKey)) )
				{
#ifdef _NEW_KEY_
					if ( NULL != pGame->m_csStatus.m_lstQuickSlot[ iTmpSlotNum ] )
					{
						if (TRUE == pGame->m_csStatus.m_lstQuickSlot[ iTmpSlotNum ]->m_bSkill || 
							Item::ItemType::SKILL_ITEM == pGame->m_csStatus.m_lstQuickSlot[ iTmpSlotNum ]->GetItemType())
						{
							// edith 여기가 1~0 눌렀을때 퀵슬롯으로 가는것.
							pGame->m_csStatus.SetActivateSkill(pGame->m_csStatus.m_lstQuickSlot[ iTmpSlotNum ] ) ;

							// 퀵슬롯에 스킬을 넣지 않으면 스킬이 사용되지 않는다.
							// 현재 퀵슬롯에 들어간 스킬이 시전되게 되어있기 때문이다.
							// 여기서 퀵슬롯으로 연결하고 특정 변수를 One해주면 LButtonPress 같이 작동하게?
							g_DeviceInput.SetSkillKey(TRUE);
//							if(!pSelfCreature->GetIsDead())
//								pSelfCreature->SetStillCasting( true );
						}
						else
						{
							g_DeviceInput.SetSkillKey(FALSE);
							UseItem(pGame->m_csStatus.m_lstQuickSlot[pGame->m_dwStartQuickSlot + iSlot]);
						}
						return S_OK;
					}
					else // Minotaurs 빈슬롯을 클릭하면 퀵슬롯을 NULL로 만든다.
					{
						pGame->m_csStatus.m_lpQuickSelected = NULL;
					}
#else
					if ( NULL != pGame->m_csStatus.m_lstQuickSlot[ iTmpSlotNum ] )
					{
						unsigned short wSkillID = pGame->m_csStatus.m_lstQuickSlot[ iTmpSlotNum ]->GetProtoTypeID();
						// 소셜모션은 점프중 사용이 불가능하다.
						if(0x4000 < wSkillID && wSkillID < 0x5000)
						{
							unsigned long dwLock = pGame->m_csStatus.m_lstQuickSlot[ iTmpSlotNum ]->GetSkillLockCount();
							unsigned long dwLevel = pGame->m_csStatus.m_lstQuickSlot[ iTmpSlotNum ]->GetNowDurability();

							pGame->m_csStatus.UseSocialAction(wSkillID, dwLock);
						}
						else if (TRUE == pGame->m_csStatus.m_lstQuickSlot[ iTmpSlotNum ]->m_bSkill || 
							Item::ItemType::SKILL_ITEM == pGame->m_csStatus.m_lstQuickSlot[ iTmpSlotNum ]->GetItemType())
						{
							pGame->m_csStatus.SetActivateSkill(pGame->m_csStatus.m_lstQuickSlot[ iTmpSlotNum ] ) ;
						}
						else
						{
							UseItem(pGame->m_csStatus.m_lstQuickSlot[pGame->m_dwStartQuickSlot + iSlot]); //Minotaurs 아이템 사용 
						}

						return S_OK;
					}
#endif
				}

				if ( KeyControl::IsKeyPress(KeyControl::KEY_COLUM_PREV) )
				{
					long PrevQuickSlot = pGame->m_dwStartQuickSlot + MAX_QUICK_SLOT_NUM - 1;

					if (NULL == pGame->m_csStatus.m_lpQuickSelected)
					{
						while ((long)pGame->m_dwStartQuickSlot < PrevQuickSlot && 
							NULL == pGame->m_csStatus.m_lstQuickSlot[ PrevQuickSlot ])
							
						{
							--PrevQuickSlot;
						}

						if (NULL != pGame->m_csStatus.m_lstQuickSlot[ PrevQuickSlot ])
						{
//							if (TRUE == pGame->m_csStatus.m_lstQuickSlot[ PrevQuickSlot ]->m_bSkill || 
//								Item::ItemType::SKILL_ITEM == pGame->m_csStatus.m_lstQuickSlot[ PrevQuickSlot ]->GetItemType())
//							{
								pGame->m_csStatus.SetActivateSkill(pGame->m_csStatus.m_lstQuickSlot[ PrevQuickSlot ] ) ;
//							}
//							else
//							{
//								UseItem(pGame->m_csStatus.m_lstQuickSlot[ PrevQuickSlot ]);
//							}
						}
					}
					else
					{
						while ((long)pGame->m_dwStartQuickSlot < PrevQuickSlot)
						{
							if (pGame->m_csStatus.m_lpQuickSelected == pGame->m_csStatus.m_lstQuickSlot[ PrevQuickSlot ])
							{
								break;
							}

							--PrevQuickSlot;
						}

							do
							{
								if ((long)pGame->m_dwStartQuickSlot > (PrevQuickSlot - 1))
								{
									break;
								}

								--PrevQuickSlot;
							} while ((long)pGame->m_dwStartQuickSlot <= PrevQuickSlot &&
								NULL == pGame->m_csStatus.m_lstQuickSlot[ PrevQuickSlot ]);

						if (NULL != pGame->m_csStatus.m_lstQuickSlot[ PrevQuickSlot ])
						{
//							if (TRUE == pGame->m_csStatus.m_lstQuickSlot[ PrevQuickSlot ]->m_bSkill || 
//								Item::ItemType::SKILL_ITEM == pGame->m_csStatus.m_lstQuickSlot[ PrevQuickSlot ]->GetItemType())
//							{
								pGame->m_csStatus.SetActivateSkill(pGame->m_csStatus.m_lstQuickSlot[ PrevQuickSlot ] ) ;
//							}
//							else
//							{
//								UseItem(pGame->m_csStatus.m_lstQuickSlot[ PrevQuickSlot ]);
//							}
						}
					}

					break;
				}	

				if ( KeyControl::IsKeyPress(KeyControl::KEY_COLUM_NEXT) )
				{
					long NextQuickSlot = pGame->m_dwStartQuickSlot;

					if (NULL == pGame->m_csStatus.m_lpQuickSelected)
					{
						while ((long)(pGame->m_dwStartQuickSlot + MAX_QUICK_SLOT_NUM) > NextQuickSlot && 
							NULL == pGame->m_csStatus.m_lstQuickSlot[ NextQuickSlot ])

						{
							++NextQuickSlot;
						}

						if (NULL != pGame->m_csStatus.m_lstQuickSlot[ NextQuickSlot ])
						{
//							if (TRUE == pGame->m_csStatus.m_lstQuickSlot[ NextQuickSlot ]->m_bSkill || 
//								Item::ItemType::SKILL_ITEM == pGame->m_csStatus.m_lstQuickSlot[ NextQuickSlot ]->GetItemType())
//							{
								pGame->m_csStatus.SetActivateSkill(pGame->m_csStatus.m_lstQuickSlot[ NextQuickSlot ] ) ;
//							}
//							else
//							{
//								UseItem(pGame->m_csStatus.m_lstQuickSlot[ NextQuickSlot ]);
//							}
						}
					}
					else
					{
						while ((long)(pGame->m_dwStartQuickSlot + MAX_QUICK_SLOT_NUM) > NextQuickSlot)
						{
							if (pGame->m_csStatus.m_lpQuickSelected == pGame->m_csStatus.m_lstQuickSlot[ NextQuickSlot ])
							{
								break;
							}
							++NextQuickSlot;
						}

						do
						{
							if ((long)(pGame->m_dwStartQuickSlot + MAX_QUICK_SLOT_NUM) <= (NextQuickSlot + 1))
							{
								break;
							}

							++NextQuickSlot;
						} while ((long)(pGame->m_dwStartQuickSlot + MAX_QUICK_SLOT_NUM) > NextQuickSlot &&
							NULL == pGame->m_csStatus.m_lstQuickSlot[ NextQuickSlot ]);

						if (NULL != pGame->m_csStatus.m_lstQuickSlot[ NextQuickSlot ])
						{
//							if (TRUE == pGame->m_csStatus.m_lstQuickSlot[ NextQuickSlot ]->m_bSkill || 
//								Item::ItemType::SKILL_ITEM == pGame->m_csStatus.m_lstQuickSlot[ NextQuickSlot ]->GetItemType())
//							{
								pGame->m_csStatus.SetActivateSkill(pGame->m_csStatus.m_lstQuickSlot[ NextQuickSlot ] ) ;
//							}
//							else
//							{
//								UseItem(pGame->m_csStatus.m_lstQuickSlot[ NextQuickSlot ]);
//							}
						}
					}

					break;
				}	
			}
		}
	}

	
	return S_OK ;
}

VOID	CRYLQuickSlotDlg::RotationDlg() 
{
	RECT rt ;
	CGMUITooltip* lpTooltip = NULL ;

	if ( m_bVertEnable )
	{
		AttachWindowTexture( ( CGMTexture*)m_pHorzImage ) ;
		SetWidth( 317 ) ;
		SetHeight( 41 ) ;
		m_pRotationButton->SetLeft( 292 ) ;
		m_pRotationButton->SetTop( 3 ) ;
		m_pHideButton->SetLeft( 304 ) ;
		m_pHideButton->SetTop( 3 ) ;

		lpTooltip = m_TooltipManager.FindTooltip( 1783 ) ;
		if ( lpTooltip )
		{
			lpTooltip->SetRect( GetLeft(), 
								GetTop(), 
								GetLeft() + GetWidth(), 
								GetTop()  + GetHeight() ) ;
		}

		// Z 버튼
		CGMImageDib* pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI_256x256 ) ;
		m_pZCheckBox->SetLeft( 272 ) ;
		m_pZCheckBox->SetTop( 12 ) ;
		SetRect( &rt, 102, 214, 116, 227 ) ;
		m_pZCheckBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
		SetRect( &rt, 102, 227, 116, 240 ) ;
		m_pZCheckBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
		SetRect( &rt, 102, 240, 116, 253 ) ;
		m_pZCheckBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
		SetRect( &rt, 102, 214, 116, 227 ) ;
		m_pZCheckBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;

		lpTooltip = m_TooltipManager.FindTooltip( 2225 ) ;
		if ( lpTooltip )
		{
			lpTooltip->SetRect( m_pZCheckBox->GetLeft(), 
								m_pZCheckBox->GetTop(), 
								m_pZCheckBox->GetLeft() + m_pZCheckBox->GetWidth(), 
								m_pZCheckBox->GetTop()  + m_pZCheckBox->GetHeight() ) ;
		}

		// X 버튼
		m_pXCheckBox->SetLeft( 272 ) ;
		m_pXCheckBox->SetTop( 26 ) ;
		SetRect( &rt, 116, 214, 130, 227 ) ;
		m_pXCheckBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
		SetRect( &rt, 116, 227, 130, 240 ) ;
		m_pXCheckBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
		SetRect( &rt, 116, 240, 130, 253 ) ;
		m_pXCheckBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
		SetRect( &rt, 116, 214, 130, 227 ) ;
		m_pXCheckBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;

		lpTooltip = m_TooltipManager.FindTooltip( 2226 ) ;
		if ( lpTooltip )
		{
			lpTooltip->SetRect( m_pXCheckBox->GetLeft(), 
								m_pXCheckBox->GetTop(), 
								m_pXCheckBox->GetLeft() + m_pXCheckBox->GetWidth(), 
								m_pXCheckBox->GetTop()  + m_pXCheckBox->GetHeight() ) ;
		}
	}
	else
	{
		AttachWindowTexture( ( CGMTexture*)m_pVertImage ) ;
		SetWidth( 41 ) ;
		SetHeight( 329 ) ;
		m_pRotationButton->SetLeft( 17 ) ;
		m_pRotationButton->SetTop( 3 ) ;
		m_pHideButton->SetLeft( 27 ) ;
		m_pHideButton->SetTop( 3 ) ;

		lpTooltip = m_TooltipManager.FindTooltip( 1783 ) ;
		if ( lpTooltip )
		{
			lpTooltip->SetRect( GetLeft(), 
								GetTop(), 
								GetLeft() + GetWidth(), 
								GetTop()  + GetHeight() ) ;
		}

		// Z 버튼
		CGMImageDib* pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_MAINGUI02_256x256 ) ;
		m_pZCheckBox->SetLeft( 12 ) ;
		m_pZCheckBox->SetTop( 43 ) ;
		SetRect( &rt, 125, 124, 139, 137 ) ;
		m_pZCheckBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
		SetRect( &rt, 125, 137, 139, 150 ) ;
		m_pZCheckBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
		SetRect( &rt, 125, 150, 139, 163 ) ;
		m_pZCheckBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
		SetRect( &rt, 125, 124, 139, 137 ) ;
		m_pZCheckBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
		
		lpTooltip = m_TooltipManager.FindTooltip( 2225 ) ;
		if ( lpTooltip )
		{
			lpTooltip->SetRect( m_pZCheckBox->GetLeft(), 
								m_pZCheckBox->GetTop(), 
								m_pZCheckBox->GetLeft() + m_pZCheckBox->GetWidth(), 
								m_pZCheckBox->GetTop()  + m_pZCheckBox->GetHeight() ) ;
		}
		
		// X 버튼
		m_pXCheckBox->SetLeft( 26 ) ;
		m_pXCheckBox->SetTop( 43 ) ;
		SetRect( &rt, 139, 124, 153, 137 ) ;
		m_pXCheckBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
		SetRect( &rt, 139, 137, 153, 150 ) ;
		m_pXCheckBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
		SetRect( &rt, 139, 150, 153, 163 ) ;
		m_pXCheckBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
		SetRect( &rt, 139, 124, 153, 137 ) ;
		m_pXCheckBox->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;

		lpTooltip = m_TooltipManager.FindTooltip( 2226 ) ;
		if ( lpTooltip )
		{
			lpTooltip->SetRect( m_pXCheckBox->GetLeft(), 
								m_pXCheckBox->GetTop(), 
								m_pXCheckBox->GetLeft() + m_pXCheckBox->GetWidth(), 
								m_pXCheckBox->GetTop()  + m_pXCheckBox->GetHeight() ) ;
		}
	}
	m_bVertEnable = !m_bVertEnable ;
}

VOID	CRYLQuickSlotDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y )
{
	if ( !g_pQuickSlotDlg )
		return ;

	switch( Sender->GetInstance() )
	{
		case RYLQUICKSLOTDLG::ROTATION_BUTTON :
			g_pQuickSlotDlg->RotationDlg() ;
			break ;

		case RYLQUICKSLOTDLG::HIDE_BUTTON :
		{
			g_pQuickSlotDlg->SetVisible( FALSE ) ;

			// Visible Flag Setting
			CRYLGameScene* pScene = static_cast<CRYLGameScene*>( CRYLSceneManager::Instance()->GetCurrentScene() ) ;
			if ( pScene ) 
			{
				pScene->SetVisibleInfo( RYLMAINWINDOW::QUICKSLOT_WINDOW, FALSE ) ;
			}
			break ;
		}

		case RYLQUICKSLOTDLG::Z_CHECKBOX :
			g_pQuickSlotDlg->ZButton() ;
			break ;

		case RYLQUICKSLOTDLG::X_CHECKBOX :
			g_pQuickSlotDlg->XButton() ;
			break ;
	}
}

HRESULT		CRYLQuickSlotDlg::FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) 
{
	return S_OK ;
}

VOID	 CRYLQuickSlotDlg::UseItem(CItemInstance *lpItem)
{
	CRYLGameData* pGame = CRYLGameData::Instance() ;
	CRYLNetworkData* pNetwork = CRYLNetworkData::Instance() ;

	if (pGame->m_lpInterface && pGame->m_lpInterface->GetIsTrade()) 
	{
		return;
	}

	RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
	if (NULL == pSelfCreature)
	{
		return;
	}

	if (lpItem->m_lCoolDownTimeCount > 0 )
	{
		return;		
	}


	if (lpItem->GetItemInfo().m_UseItemInfo.m_cLimitRealmPoint > pSelfCreature->GetRealmPoint())
	{
		char szErr[MAX_PATH] = { 0, };
		sprintf(szErr, CRYLStringTable::m_strString[ 3865 ], lpItem->GetItemInfo().m_UseItemInfo.m_cLimitRealmPoint);
		CRYLChattingDlg::Instance()->AddMessage(szErr, CRYLNetworkData::Instance()->m_dwMyChrID, 129);
		return;
	}

	if (true == lpItem->GetItemInfo().m_UseItemInfo.m_bForStatueWar && 
		!CRYLGameTimeManager::Instance()->IsRealmWarTime())
	{
		CRYLChattingDlg::Instance()->AddMessage(CRYLStringTable::m_strString[ 3866 ], CRYLNetworkData::Instance()->m_dwMyChrID, 129);
		return;
	}
	
	if(lpItem->GetItemInfo().m_UseItemInfo.m_bForStatueWar && CRYLGameTimeManager::Instance()->IsRealmWarTime() && 
		pSelfCreature->GetRealmWarFlag() == Creature::WAR_OFF)
	{
		CRYLChattingDlg::Instance()->AddMessage(CRYLStringTable::m_strString[ 3872 ], CRYLNetworkData::Instance()->m_dwMyChrID, 129);
		return;
	}

	CItemInstance* lpFindIte = pGame->m_csStatus.GetInventoryItemfromID(lpItem->GetProtoTypeID());

	if(lpFindIte)
	{
		switch (lpItem->GetItemType())
		{
		case Item::ItemType::POTION:
			{
				CRYLInventoryPanel::UsePotion(lpFindIte);
				break;
			}

		case Item::ItemType::PORTAL:
		case Item::ItemType::FIXED_PORTAL:
			{
				CRYLInventoryPanel::UsePortal(lpFindIte);
				break;
			}

		case Item::ItemType::FIRE_CRACKER:
			{		
				SendPacket::CharUseItem(g_GameSession, pNetwork->m_dwMyChrID, pNetwork->m_dwMyChrID, &lpFindIte->m_lpItemBase->GetPos());				
				break ;
			}
		}
	}	
}

VOID	CRYLQuickSlotDlg::ZButton()
{
	CRYLGameData::Instance()->m_dwStartQuickSlot = 0;

	m_pZCheckBox->SetChecked( TRUE ) ;
	m_pXCheckBox->SetChecked( FALSE ) ;
}

VOID	CRYLQuickSlotDlg::XButton()
{
	CRYLGameData::Instance()->m_dwStartQuickSlot = 10;

	m_pZCheckBox->SetChecked( FALSE ) ;
	m_pXCheckBox->SetChecked( TRUE ) ;
}

VOID	CRYLQuickSlotDlg::ToggleButton() 
{
	if(CRYLGameData::Instance()->m_dwStartQuickSlot)
	{
		CRYLGameData::Instance()->m_dwStartQuickSlot = 0;

		m_pZCheckBox->SetChecked( TRUE ) ;
		m_pXCheckBox->SetChecked( FALSE ) ;
	}
	else
	{
		CRYLGameData::Instance()->m_dwStartQuickSlot = 10;

		m_pZCheckBox->SetChecked( FALSE ) ;
		m_pXCheckBox->SetChecked( TRUE ) ;
	}

	

}