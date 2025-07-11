#include <winsock2.h>
#include "RYLSkillPanel.h"
#include "GMUICustomDlg.h"
#include "RYLClientMain.h"
#include "RYLGameData.h"
#include "RYLNetworkData.h"
#include "RYLStringTable.h"
#include "RYLImage.h"
#include "RYLImageManager.h"
#include "GMImageDib.h"
#include "RYLImageCabinet.h"
#include "RYLSpriteEx.h"
#include "RYLSpriteList.h"

#include "RYLButton.h"
#include "RYLLabel.h"
#include "RYLProgressBar.h"
#include "RYLChattingDlg.h"

#include "RYLQuickSlotDlg.h"
#include "RYLSceneManager.h"
#include "RYLGameScene.h"

#include "WinInput.h"
#include "..\\GUITooltipItem.h"
#include "RYLMessageBox.h"
#include "..\\ItemInstance.h"

#include <Item/ItemMgr.h>
#include <Item/ItemFactory.h>
#include <Network/ClientSocket/Send/SendItem.h>
#include <Network/ClientSocket/Send/SendSkill.h>
#include <Skill/SkillMgr.h>
#include "GMMemory.h"


//-----------------------------------------------------------------------------
// SkillSlot
CRYLSkillSlot*		g_SkillSlot = NULL ;
CRYLSkillSlot::CRYLSkillSlot( CGMUICustomDlg *pParentDlg ) : CGMUICustomPanel( pParentDlg )
{
	m_pGMUIParentDlg = pParentDlg ;
	g_SkillSlot		 = this ;

	m_pSkillSprite			= NULL ;
	m_pSkillNameLabel		= NULL ;
	m_pSkillLevelLabel		= NULL ;
	m_pSkillProgrss			= NULL ;
	m_pMasterSkillProgrss	= NULL ;

	m_iSkillSlotIndex		= 0 ;
	m_dwSkillClass			= 0 ;
}

CRYLSkillSlot::~CRYLSkillSlot() 
{
	GM_DELETE( m_pSkillNameLabel ) ;
	GM_DELETE( m_pSkillLevelLabel ) ;
	GM_DELETE( m_pSkillSprite ) ;
	GM_DELETE( m_pSkillProgrss ) ;	
	GM_DELETE( m_pMasterSkillProgrss ) ;
}

VOID		CRYLSkillSlot::CreateSkillName( LPSTR lpSkillName, bool bAlterSkill )
{
	m_pSkillNameLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	if ( bAlterSkill )
		m_pSkillNameLabel->SetLeft( 65 ) ;
	else
		m_pSkillNameLabel->SetLeft( 43 ) ;
	m_pSkillNameLabel->SetTop( 45 ) ;
	m_pSkillNameLabel->SetFontColor( 0xffffffff ) ;
	m_pSkillNameLabel->SetAutoSize( TRUE ) ;
	m_pSkillNameLabel->SetCaption( lpSkillName ) ;
	CGMUICustomPanel::AddChild( m_pSkillNameLabel ) ;
}

VOID		CRYLSkillSlot::SetSkillName( LPSTR lpSkillName ) 
{
	m_pSkillNameLabel->SetCaption( lpSkillName ) ;
}

VOID		CRYLSkillSlot::CreateSkillLevel( LPSTR lpSkillName )
{
	m_pSkillLevelLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pSkillLevelLabel->SetLeft( 65 ) ;
	m_pSkillLevelLabel->SetTop( 61 ) ;
	m_pSkillLevelLabel->SetFontColor( 0xffffffff ) ;
	m_pSkillLevelLabel->SetAutoSize( TRUE ) ;
	m_pSkillLevelLabel->SetCaption( lpSkillName ) ;
	CGMUICustomPanel::AddChild( m_pSkillLevelLabel ) ;
}

VOID		CRYLSkillSlot::SetSkillLevel( DWORD dwLevel )
{
	CHAR	strString[ MAX_PATH ] ;
	sprintf( strString, "%d", dwLevel ) ;

	m_pSkillLevelLabel->SetCaption( strString ) ;
}


VOID		CRYLSkillSlot::MakeSkillSlot( INT iSkillKind, UINT uiKind  ) 
{
	RECT			rt ;
	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;

	// Background Image
	SetWidth( 233 ) ;
	SetHeight( 42 ) ;
	CRYLSpriteList::Instance()->CreateGMImage( 233, 42, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;

	switch( iSkillKind )
	{
	case Skill::Type::CAST :	SetRect( &rt, 0, 0,   233, 42 ) ;		break ;
	case Skill::Type::CHANT :	SetRect( &rt, 0, 42,  233, 84 ) ;		break ;
	case Skill::Type::PASSIVE :	SetRect( &rt, 0, 84,  233, 126 ) ;		break ;
	case Skill::Type::INSTANCE :SetRect( &rt, 0, 126, 233, 168 ) ;		break ;
	case Skill::Type::GATHER:	SetRect( &rt, 0, 126, 233, 168 ) ;		break ;
	}
	
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_SKILLSLOT_256x256 ) ) ;
	
	if ( uiKind == GENERALSKILL )
	{
		SetRect( &rt, 88, 168, 110, 182 ) ;
		pImageDIB->DrawRect( &rt, 43, 22, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_SKILLSLOT_256x256 ) ) ;
		SetRect( &rt, 110, 168, 204, 181 ) ;
		pImageDIB->DrawRect( &rt, 86, 23, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_SKILLSLOT_256x256 ) ) ;
	}

	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	AttachPanelImage( ( CGMTexture*)pImage ) ;
	GM_DELETE( pImageDIB ) ;
}

VOID		CRYLSkillSlot::MakeAlterSkillSlot( INT iSkillKind, UINT uiKind  ) 
{
	RECT			rt ;
	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;

	// Background Image
	SetWidth( 233 ) ;
	SetHeight( 42 ) ;
	CRYLSpriteList::Instance()->CreateGMImage( 233, 42, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;

	switch( iSkillKind )
	{
	case Skill::Type::CAST :	SetRect( &rt, 0, 0,   233, 42 ) ;		break ;
	case Skill::Type::CHANT :	SetRect( &rt, 0, 42,  233, 84 ) ;		break ;
	case Skill::Type::PASSIVE :	SetRect( &rt, 0, 84,  233, 126 ) ;		break ;
	case Skill::Type::INSTANCE :SetRect( &rt, 0, 126, 233, 168 ) ;		break ;
	case Skill::Type::GATHER:	SetRect( &rt, 0, 126, 233, 168 ) ;		break ;
	}
	
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_ALTERNATIVE_SKILL ) ) ;
	
	/*
	if ( uiKind == ALTERNATIVESKILL )
	{
		SetRect( &rt, 88, 168, 110, 182 ) ;
		pImageDIB->DrawRect( &rt, 43, 22, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_ALTERNATIVE_SKILL ) ) ;
		SetRect( &rt, 110, 168, 204, 181 ) ;
		pImageDIB->DrawRect( &rt, 86, 23, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_ALTERNATIVE_SKILL ) ) ;
	}
	*/

	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	AttachPanelImage( ( CGMTexture*)pImage ) ;
	GM_DELETE( pImageDIB ) ;
}

VOID		CRYLSkillSlot::CreateSkillProgrssBar( INT iLevel ) 
{
	RECT rt ;
	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;

	m_pSkillProgrss = new CRYLProgressBar( m_pGMUIParentDlg ) ;
	m_pSkillProgrss->SetLeft( 89 ) ;
	m_pSkillProgrss->SetTop( 62 ) ;
	m_pSkillProgrss->SetWidth( 90 ) ;
	m_pSkillProgrss->SetHeight( 9 ) ;
	m_pSkillProgrss->SetMax( 5 ) ;
	
	CRYLSpriteList::Instance()->CreateGMImage( 90, 9, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 168, 88, 175 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_SKILLSLOT_256x256 ) ) ;
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	m_pSkillProgrss->AttachBarImageTexture( ( CGMTexture*)pImage ) ;
	m_pSkillProgrss->SetPosition( iLevel ) ;
	
	CGMUICustomPanel::AddChild( m_pSkillProgrss ) ;
	GM_DELETE( pImageDIB ) ;
}

VOID	CRYLSkillSlot::SetSkillLockCount( INT iCount )
{
	if ( iCount == 6 )
	{
		m_pSkillProgrss->SetVisible( FALSE ) ;
		m_pMasterSkillProgrss->SetVisible( TRUE ) ;
	}
	else
	{
		m_pSkillProgrss->SetPosition( iCount ) ;
		m_pSkillProgrss->SetVisible( TRUE ) ;
		m_pMasterSkillProgrss->SetVisible( FALSE ) ;
	}
}

VOID	CRYLSkillSlot::CreateMasterSkillProgrssBar() 
{
	RECT rt ;
	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;

	m_pMasterSkillProgrss = new CRYLProgressBar( m_pGMUIParentDlg ) ;
	m_pMasterSkillProgrss->SetLeft( 89 ) ;
	m_pMasterSkillProgrss->SetTop( 62 ) ;
	m_pMasterSkillProgrss->SetWidth( 90 ) ;
	m_pMasterSkillProgrss->SetHeight( 9 ) ;
	m_pMasterSkillProgrss->SetMax( 5 ) ;
	
	CRYLSpriteList::Instance()->CreateGMImage( 90, 9, &pImageDIB ) ;

	if(pImageDIB)
	{
		pImageDIB->ClearbyColor( 0x00000000 ) ;
		SetRect( &rt, 0, 175, 88, 182 ) ;
		pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_SKILLSLOT_256x256 ) ) ;
		
		pImage = CRYLImageManager::Instance()->CreateRYLImage() ;

		if(pImage)
		{
			pImage->LoadFromImage( pImageDIB ) ;
			CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
			m_pMasterSkillProgrss->AttachBarImageTexture( ( CGMTexture*)pImage ) ;
		}
		
		m_pMasterSkillProgrss->SetPosition( 5 ) ;
		CGMUICustomPanel::AddChild( m_pMasterSkillProgrss ) ;

		GM_DELETE( pImageDIB ) ;
	}	
}


//-----------------------------------------------------------------------------
// Skill Button
CRYLSkillButton::CRYLSkillButton( CGMUICustomDlg *pParentDlg ) : CRYLButton( pParentDlg )
{

}
CRYLSkillButton::~CRYLSkillButton() 
{
}

//-----------------------------------------------------------------------------
// RYLSkillPanel
CRYLSkillPanel*	g_pSkillPanel = NULL ;
CRYLSkillPanel::CRYLSkillPanel( CGMUICustomDlg *pParentDlg ) : CGMUICustomPanel( pParentDlg )
{
	RECT	rt ;
	INT		iTempWidth ;
	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;

	// Background Image
	CRYLSpriteList::Instance()->CreateGMImage( 375, 440, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;

	SetRect( &rt, 0, 38, 375, 43 ) ;
	for ( int i = 0 ; i < 87 ; i ++ )
	{
		iTempWidth = i * 5 ;
		pImageDIB->DrawRect( &rt, 0, iTempWidth, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT02_256x256 ) ) ;
	}
	SetRect( &rt, 0, 42, 375, 52 ) ;
	pImageDIB->DrawRect( &rt, 0, iTempWidth, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT02_256x256 ) ) ;
	
	// Frame Image
	pImageDIB->MakeFrameRect( 6,  28, 243, 389, 0xffB99E6B, 0xc0000000,  1 ) ;
	pImageDIB->MakeFrameRect( 84, 5, 68, 18, 0xff959595, 0xc0000000,  1 ) ;
	
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	AttachPanelImage( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;

	SetWidth( 375 ) ;
	SetHeight( 440 ) ;

	m_pGMUIParentDlg = pParentDlg ;
	g_pSkillPanel = this ;

	m_iSkillYMargin = 0 ;
	m_dwStartSlot   = 0 ;
	m_dwEraseResult = 0 ;
	m_dwProcessSkillIndex	= 0 ;
	m_cSkillButtonIndex		= 0 ;
	m_cSkillSlotIndex		= 0 ;
	m_pPrevSkillButton			= NULL ;
	m_pNextSkillButton			= NULL ;
	m_pQuickSlotButton			= NULL ;
	m_pSkillPointLabel			= NULL ; // 스킬 레벨
	m_pSkillPointValueLabel		= NULL ; // 스킬 레벨 값

	m_lpStatusRetrainItem = NULL;	// 망각의돌 아이템 변수

	m_cTmpAddAlterSkillNum = 0 ;
}

CRYLSkillPanel::~CRYLSkillPanel() 
{
	FinalCtrl() ;
}


VOID	CRYLSkillPanel::InitCtrl() 
{
	RECT rt ;
	CGMImageDib* pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT02_256x256 ) ;

	m_pPrevSkillButton = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pPrevSkillButton->SetLeft( 220 ) ;
	m_pPrevSkillButton->SetTop( 420 ) ;
	m_pPrevSkillButton->OnClickEvent = DlgClick ;
	m_pPrevSkillButton->SetInstance( RYLSKILLPANEL::PREVSKILL_BUTTON ) ;
	SetRect( &rt, 217, 215, 230, 227 ) ;
	m_pPrevSkillButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 217, 243, 230, 255 ) ;
	m_pPrevSkillButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 217, 229, 230, 241 ) ;
	m_pPrevSkillButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 217, 215, 230, 227 ) ;
	m_pPrevSkillButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	CGMUICustomPanel::AddChild( m_pPrevSkillButton ) ;

	m_pNextSkillButton = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pNextSkillButton->SetLeft( 235 ) ;
	m_pNextSkillButton->SetTop( 420 ) ;
	m_pNextSkillButton->OnClickEvent = DlgClick ;
	m_pNextSkillButton->SetInstance( RYLSKILLPANEL::NEXTSKILL_BUTTON ) ;
	SetRect( &rt, 232, 215, 245, 227 ) ;
	m_pNextSkillButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 232, 243, 245, 255 ) ;
	m_pNextSkillButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 232, 229, 245, 241 ) ;
	m_pNextSkillButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 232, 215, 245, 227 ) ;
	m_pNextSkillButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	CGMUICustomPanel::AddChild( m_pNextSkillButton ) ;

	pSprite			= CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;
	m_pQuickSlotButton = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pQuickSlotButton->SetLeft( 183 ) ;
	m_pQuickSlotButton->SetTop( 2 ) ;
	m_pQuickSlotButton->SetCaption( CRYLStringTable::m_strString[ 2092 ] ) ;
	m_pQuickSlotButton->SetFontColor( 0xffffffff) ;
	m_pQuickSlotButton->SetColor( 0xffffffff) ;
	m_pQuickSlotButton->OnClickEvent = DlgClick ;
	m_pQuickSlotButton->SetInstance( RYLSKILLPANEL::QUICKSLOT_BUTTON ) ;
	m_pQuickSlotButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pQuickSlotButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pQuickSlotButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pQuickSlotButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	CGMUICustomPanel::AddChild( m_pQuickSlotButton ) ;

	m_pSkillPointLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pSkillPointLabel->SetLeft( 16 ) ;
	m_pSkillPointLabel->SetTop( 8 ) ;
	m_pSkillPointLabel->SetFontColor( 0xffffffff ) ;
	SetRect( &rt, 17, 8, 80, 19 ) ;
	m_pSkillPointLabel->SetAlignRect( rt ) ;
	m_pSkillPointLabel->SetAlign( CENTER ) ;
	m_pSkillPointLabel->SetCaption( CRYLStringTable::m_strString[ 2093 ] ) ;
	m_pSkillPointLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pSkillPointLabel ) ;

	m_pSkillPointValueLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pSkillPointValueLabel->SetLeft( 98 ) ;
	m_pSkillPointValueLabel->SetTop( 8 ) ;
	m_pSkillPointValueLabel->SetFontColor( 0xffffffff ) ;
	SetRect( &rt, 84, 8, 151, 19 ) ;
	m_pSkillPointValueLabel->SetAlignRect( rt ) ;
	m_pSkillPointValueLabel->SetAlign( CENTER ) ;
	m_pSkillPointValueLabel->SetCaption( "" ) ;
	m_pSkillPointValueLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pSkillPointValueLabel ) ;
}

VOID	CRYLSkillPanel::FinalCtrl() 
{
	GM_DELETE( m_pPrevSkillButton ) ;
	GM_DELETE( m_pNextSkillButton ) ;
	GM_DELETE( m_pQuickSlotButton ) ;
	GM_DELETE( m_pSkillPointLabel ) ;
	GM_DELETE( m_pSkillPointValueLabel ) ;

	ClearSkillSlot() ;
	ClearVirtualSkill() ;
}

VOID		CRYLSkillPanel::ClearSkillSlot() 
{
	if ( m_vecSkillSlot.empty() )
		return ;

	for ( std::vector< CRYLSkillSlot* >::iterator it = m_vecSkillSlot.begin() ; it != m_vecSkillSlot.end() ;++ it )
	{
		if ( ( *it )->m_dwSkillClass != ALTERNATIVESKILL )
		{
			CGMUICustomPanel::DeleteChild( ( *it )->m_pEraseButton ) ;
			GM_DELETE( ( *it )->m_pEraseButton ) ;
		}
		CGMUICustomPanel::DeleteChild( ( *it ) ) ;
		GM_DELETE( (*it ) ) ;
	}

	m_vecSkillSlot.clear() ;
	
}

VOID		CRYLSkillPanel::InsertSkillSlot( CItemInstance* lpSkill, unsigned short wSkillID, unsigned char cGrade, unsigned char cLevel, 
											 unsigned char cPos, unsigned char cIndex, UINT uiKind )  
{
	CGMImageDib* pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;
	CRYLGameData* pGame = CRYLGameData::Instance() ;

	CRYLSkillSlot* pSkill = new CRYLSkillSlot( m_pGMUIParentDlg ) ;
	pSkill->SetLeft( 11 ) ;
	pSkill->SetTop( m_iSkillYMargin + 34 ) ;
	
	pSkill->m_pEraseButton = new CRYLSkillButton( m_pGMUIParentDlg ) ;
	pSkill->m_pEraseButton->SetLeft( 195 ) ;
	pSkill->m_pEraseButton->SetTop( 43 + m_iSkillYMargin ) ;
	pSkill->m_pEraseButton->SetCaption( CRYLStringTable::m_strString[ 2094 ] ) ;
	pSkill->m_pEraseButton->SetFontColor( 0xffffffff ) ;
	pSkill->m_pEraseButton->SetColor( 0xffffffff ) ;
	pSkill->m_pEraseButton->OnClickEvent = DlgClick ;
	pSkill->m_pEraseButton->SetInstance( RYLSKILLPANEL::ERASE_BUTTON ) ;
	pSkill->m_pEraseButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 2 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	pSkill->m_pEraseButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 2 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	pSkill->m_pEraseButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 2 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	pSkill->m_pEraseButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 2 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	pSkill->m_pEraseButton->SetSkillIndex( m_cSkillButtonIndex ) ;
	pSkill->m_pEraseButton->SetVisible( FALSE ) ;

	CGMUICustomPanel::AddChild( pSkill->m_pEraseButton ) ;

	pSkill->m_dwSkillClass = uiKind ;
	
	CHAR	strString[ MAX_PATH ] ;
	strcpy( strString, lpSkill->GetItemInfo().m_SpriteData.m_szName ) ;
	if (strlen(strString) > 21)
	{
		strString[21] = '.';
		strString[22] = '.';
		strString[23] = '.';
		strString[24] = '\0';
	}
	pSkill->CreateSkillName( strString ) ;
	sprintf( strString, "%d", lpSkill->GetNowDurability() ) ;
	pSkill->CreateSkillLevel( strString ) ;
	pSkill->CreateSkillProgrssBar( lpSkill->GetNowDurability() ) ;
	pSkill->CreateMasterSkillProgrssBar() ; 
	pSkill->SetInstance( RYLSKILLPANEL::SKILLSLOT_PANEL ) ;
	pSkill->MakeSkillSlot( lpSkill->GetSkillType(), uiKind ) ;
	pSkill->SetSkillSlotIndex( m_cSkillSlotIndex ) ;
	pSkill->SetVisible( FALSE ) ;
	pSkill->m_pMasterSkillProgrss->SetVisible( FALSE ) ;
	CGMUICustomPanel::AddChild( pSkill ) ;

	m_iSkillYMargin += 41 ;
	++ m_cSkillButtonIndex ;
	++ m_cSkillSlotIndex ;
	
	m_vecSkillSlot.push_back( pSkill ) ;
} 

VOID		CRYLSkillPanel::InsertAlterSkillSlot( CItemInstance* pItem,
												  unsigned short wSkillID, 
												  unsigned char cIndex, 
												  UINT uiKind )  
{
	CGMImageDib* pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;
	CRYLGameData* pGame = CRYLGameData::Instance() ;

	CRYLSkillSlot* pSkill = new CRYLSkillSlot( m_pGMUIParentDlg ) ;
	pSkill->SetLeft( 11 ) ;
	pSkill->SetTop( m_iSkillYMargin + 34 ) ;
	pSkill->m_dwSkillClass = uiKind ;

	CHAR	strString[ MAX_PATH ] ;
	strcpy( strString, pItem->GetItemInfo().m_SpriteData.m_szName ) ;
	if (strlen(strString) > 21)
	{
		strString[21] = '.';
		strString[22] = '.';
		strString[23] = '.';
		strString[24] = '\0';
	}
	pSkill->CreateSkillName( strString, true ) ;
	pSkill->SetInstance( RYLSKILLPANEL::SKILLSLOT_PANEL ) ;
	pSkill->MakeAlterSkillSlot( pItem->GetSkillType(), uiKind ) ;
	pSkill->SetSkillSlotIndex( m_cSkillSlotIndex ) ;
	pSkill->SetVisible( FALSE ) ;
	CGMUICustomPanel::AddChild( pSkill ) ;
	
	++ m_cSkillSlotIndex ;

	m_iSkillYMargin += 41 ;
	m_vecSkillSlot.push_back( pSkill ) ;
}

unsigned char CRYLSkillPanel::GetAlterSkillNum( int iIdx ) 
{
	unsigned char cIndex = 0, nAlter = 0 ;

	std::vector< VIRTUAL_SKILL_SLOT * >::iterator	iterPos = m_lstDisplaySkillSlot.begin() ; 
	
	while( iterPos != m_lstDisplaySkillSlot.end() )
	{	
		VIRTUAL_SKILL_SLOT* lpSlot = ( *iterPos ) ;
		
		if ( cIndex > iIdx )
			break ;

		if ( lpSlot->bChild )
		{
			++ nAlter ;
		}
		else
		{
			++ cIndex ;
		}
		++ iterPos ;
	}

	return nAlter ;
}

VOID	CRYLSkillPanel::SetSkillSlot( CItemInstance* lpSkillInstance, INT iKInd, INT iIndex, bool bAlter )  
{
	INT iSkillIndex = 0 ; 

	switch ( iKInd )
	{
	case EDIT_SKILL : // 수정
		{
			iSkillIndex = iIndex + CRYLGameData::Instance()->m_csStatus.m_lstClassSkill.size() ;
			
			if ( bAlter )
				m_cTmpAddAlterSkillNum = 0 ;
			else
				m_cTmpAddAlterSkillNum = GetAlterSkillNum( iIndex ) ;
		}
		break ;
	case DEL_SKILLSLOT : // 삭제
		{
			if ( bAlter )
				iSkillIndex = iIndex + CRYLGameData::Instance()->m_csStatus.m_lstClassSkill.size() ;
			else
				iSkillIndex = m_dwProcessSkillIndex + CRYLGameData::Instance()->m_csStatus.m_lstClassSkill.size() ;
		}
		break ;
	}

	iSkillIndex += m_cTmpAddAlterSkillNum ;

	std::vector< CRYLSkillSlot* >::iterator it = m_vecSkillSlot.begin() ;
	CRYLSkillSlot* lpSkillSlot = NULL ;
	for ( std::vector< CRYLSkillSlot* >::iterator it = m_vecSkillSlot.begin() ; it != m_vecSkillSlot.end() ; ++ it )
	{
		if ( ( *it )->GetSkillSlotIndex() == iSkillIndex )
		{
			lpSkillSlot = ( *it ) ;
			break ;
		}
	}

	if ( !lpSkillSlot ) return ;

	CHAR	strString[ MAX_PATH ] ;
	strcpy( strString, lpSkillInstance->GetItemInfo().m_SpriteData.m_szName ) ;
	if (strlen(strString) > 21)
	{
		strString[21] = '.' ;
		strString[22] = '.' ;
		strString[23] = '.' ;
		strString[24] = '\0' ;
	}
	lpSkillSlot->SetSkillName( strString ) ;
	if ( !bAlter )
	{
		lpSkillSlot->SetSkillLevel( lpSkillInstance->GetNowDurability() ) ;
		lpSkillSlot->SetSkillLockCount( lpSkillInstance->GetNowDurability() ) ;
	}
}

VOID		CRYLSkillPanel::UpdateSkillSlotPosition()
{
	if ( m_vecSkillSlot.empty() ) return ;

	INT iTempY = 0, iTempIndex = 0 ;
	INT	iCount = 0 ;
	CRYLSkillSlot* lpSlot = NULL ;
	
	for ( std::vector< CRYLSkillSlot* >::iterator it = m_vecSkillSlot.begin() ; it != m_vecSkillSlot.end() ; ++ it )
	{
		if ( ( iCount % MAX_SLOT_PER_PAGE ) == 0 )
		{
			iTempY = 0 ;
		}

		lpSlot = ( *it ) ;
		lpSlot->SetTop( iTempY + 34 ) ;
		
		if ( lpSlot->m_dwSkillClass == GENERALSKILL ) 
		{
			lpSlot->m_pEraseButton->SetTop( iTempY + 43 ) ;
			lpSlot->m_pEraseButton->SetSkillIndex( iTempIndex ) ;
			++ iTempIndex ;
		}
		lpSlot->SetSkillSlotIndex( iCount ) ;
		
		iTempY += 41 ;
		++ iCount ;
	}
	m_iSkillYMargin = iTempY ;
}

VOID		CRYLSkillPanel::DeleteCurrentSkillSlot() 
{
	//=======================================================================
	// delete virtual slot
	//=======================================================================
	std::vector< VIRTUAL_SKILL_SLOT * >::iterator	iterPos = m_lstDisplaySkillSlot.begin() ; 
	unsigned short wSlotIdx = 0 ;
	while( iterPos != m_lstDisplaySkillSlot.end() )
	{
		CItemInstance* lpItem = ( * iterPos )->lpItem ;
		if ( NULL != lpItem )
		{
			if ( lpItem->GetProtoTypeID() == CRYLGameData::Instance()->m_csStatus.m_Skill.SSlot[ m_dwProcessSkillIndex ].SKILLINFO.wSkill )
			{
				unsigned short wSkillID = lpItem->GetProtoTypeID() ;
				const Skill::ProtoType *lpSkillBase    = g_SkillMgr.GetSkillProtoType( wSkillID ) ;
				iterPos = m_lstDisplaySkillSlot.erase( iterPos ) ;	

				if ( lpSkillBase->m_usChildSkill )
				{
					lpItem = ( * iterPos )->lpItem ;
					GM_DELETE( lpItem ) ;
					m_lstDisplaySkillSlot.erase( iterPos ) ;	
				}
				
				break ;
			}
		}
		++ wSlotIdx ;
		++ iterPos ;
	}

	INT iSkillIndex = m_dwProcessSkillIndex + CRYLGameData::Instance()->m_csStatus.m_lstClassSkill.size() ;
	iSkillIndex += GetAlterSkillNum( m_dwProcessSkillIndex ) ;

	std::vector< CRYLSkillSlot* >::iterator it = m_vecSkillSlot.begin() ;
	CRYLSkillSlot* lpSkillSlot = NULL ;
	for ( it = m_vecSkillSlot.begin() ; it != m_vecSkillSlot.end() ; ++ it )
	{
		if ( ( *it )->GetSkillSlotIndex() == iSkillIndex )
		{
			lpSkillSlot = ( *it ) ;
			it = m_vecSkillSlot.erase( it ) ;
			DeleteSkillSlot( lpSkillSlot ) ;

			if ( ( *it )->m_dwSkillClass == ALTERNATIVESKILL )
			{
				lpSkillSlot = ( *it ) ;
				m_vecSkillSlot.erase( it ) ;
				DeleteSkillSlot( lpSkillSlot ) ;
			}
			break ;
		}
	}

	SkillSlotVisible();
}

void		CRYLSkillPanel::DeleteSkillSlot( CRYLSkillSlot* lpSkillSlot ) 
{
	if ( !lpSkillSlot ) return ;
	
	CGMUICustomPanel::DeleteChild( lpSkillSlot ) ;
	if ( lpSkillSlot->m_dwSkillClass != ALTERNATIVESKILL )
	{
		CGMUICustomPanel::DeleteChild( lpSkillSlot->m_pEraseButton ) ;
	}
}

HRESULT		CRYLSkillPanel::Update( BOOL &bClick, BOOL &bEdge )
{
	CRYLGameData* pGame = CRYLGameData::Instance() ;
	//////// 스킬 지우기
	if ( m_dwEraseResult & MB_YES )
	{
		// edith 2008.02.14 스킬 삭제로직
		g_pClientSocket->SetStatusFlag( NS_SKILLERASE ) ;
		SendPacket::CharSkillErase( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID,
									pGame->m_csStatus.m_Skill.SSlot[ m_dwProcessSkillIndex ].SKILLINFO.wSkill,
									m_dwProcessSkillIndex, &m_lpStatusRetrainItem->m_lpItemBase->GetPos() ) ;

		m_dwEraseResult = 0 ;
		return S_OK ;
	} 
	else if ( m_dwEraseResult & MB_NO || m_dwEraseResult & MB_EXIT )
	{
		m_dwEraseResult = 0 ;
		return TRUE ;
	}

	POINT *ptMousePos = g_DeviceInput.GetMouseLocal() ;

	long lStartSkill;
	if ( m_dwStartSlot < pGame->m_csStatus.m_lstClassSkill.size() )
	{
		lStartSkill = 0 ;
	} 
	else
	{
		lStartSkill = m_dwStartSlot - pGame->m_csStatus.m_lstClassSkill.size() ;
	}

	unsigned long dwSlot = 0 ;
	for(unsigned long i = m_dwStartSlot ; i < pGame->m_csStatus.m_lstClassSkill.size() ; i ++ )
	{
		if ( dwSlot < MAX_SLOT_PER_PAGE )	
			dwSlot++ ;
		else				
			break;
	}

	lStartSkill -= dwSlot;

	unsigned long dwFunction ;
	if ( g_DeviceInput.GetIsLeftMouseDown() )
	{
		if ( !pGame->CheckStillCasting( dwFunction ) )
		{
			unsigned short wStart = m_dwStartSlot ;
			unsigned short wMaxSlot = m_lstDisplaySkillSlot.size() + pGame->m_csStatus.m_lstClassSkill.size() ;
			unsigned short wCurrPageMaxSlot = wMaxSlot - wStart ;

			if ( wCurrPageMaxSlot > MAX_SLOT_PER_PAGE )
				wCurrPageMaxSlot = wStart + MAX_SLOT_PER_PAGE ;
			else
				wCurrPageMaxSlot += wStart ;
			
			for ( unsigned short i = wStart ; i < wCurrPageMaxSlot ; i ++ )
			{
				CItemInstance* lpSelectSkill = NULL;
				if ( i < pGame->m_csStatus.m_lstClassSkill.size() )
				{
					lpSelectSkill = pGame->m_csStatus.m_lstClassSkill[ i ];
				}
				else
				{
					lpSelectSkill = m_lstDisplaySkillSlot[ i - pGame->m_csStatus.m_lstClassSkill.size() ]->lpItem ;
				}

				const Skill::ProtoType *lpSelectSkillBase    = g_SkillMgr.GetSkillProtoType( lpSelectSkill->m_wPreProtoTypeID ) ;
				unsigned short usParentID = 0;

				if ( lpSelectSkillBase )
				{
					if(lpSelectSkillBase->m_usParentSkill)
					{
						for ( unsigned short i = wStart ; i < wCurrPageMaxSlot; i ++ )
						{
							CItemInstance* lpSelectSkillParent = NULL;

							if ( i < pGame->m_csStatus.m_lstClassSkill.size() )
							{
								lpSelectSkillParent = pGame->m_csStatus.m_lstClassSkill[ i ];
							}
							else
							{
								lpSelectSkillParent = m_lstDisplaySkillSlot[ i - pGame->m_csStatus.m_lstClassSkill.size() ]->lpItem ;
							}

							if(lpSelectSkillBase->m_usParentSkill == 
								lpSelectSkillParent->m_wPreProtoTypeID)
							{
								lpSelectSkill->m_cCountCooldown = lpSelectSkillParent->m_cCountCooldown;
								lpSelectSkill->m_lCoolDownTimeCount = lpSelectSkillParent->m_lCoolDownTimeCount;
								lpSelectSkill->m_lCoolDownTime = lpSelectSkillParent->m_lCoolDownTime;
								memcpy(lpSelectSkill->m_pCoolDownVertices, lpSelectSkillParent->m_pCoolDownVertices, 10);
								memcpy(lpSelectSkill->m_wCoolDownIndex, lpSelectSkillParent->m_wCoolDownIndex, 24);
							}
						}
					}
					else if(lpSelectSkillBase->m_usChildSkill)
					{
						for ( unsigned short i = wStart ; i < wCurrPageMaxSlot; i ++ )
						{
							CItemInstance* lpSelectSkillChild = NULL;

							if ( i < pGame->m_csStatus.m_lstClassSkill.size() )
							{
								lpSelectSkillChild = pGame->m_csStatus.m_lstClassSkill[ i ];
							}
							else
							{
								lpSelectSkillChild = m_lstDisplaySkillSlot[ i - pGame->m_csStatus.m_lstClassSkill.size() ]->lpItem ;
							}

							if(lpSelectSkillBase->m_usChildSkill == 
								lpSelectSkillChild->m_wPreProtoTypeID)
							{
								lpSelectSkillChild->m_cCountCooldown = lpSelectSkill->m_cCountCooldown;
								lpSelectSkillChild->m_lCoolDownTimeCount = lpSelectSkill->m_lCoolDownTimeCount;
								lpSelectSkillChild->m_lCoolDownTime = lpSelectSkill->m_lCoolDownTime;
								memcpy(lpSelectSkillChild->m_pCoolDownVertices, lpSelectSkill->m_pCoolDownVertices, 10);
								memcpy(lpSelectSkillChild->m_wCoolDownIndex, lpSelectSkill->m_wCoolDownIndex, 24);
							}
						}
					}
				}


				if ( NULL != lpSelectSkill && lpSelectSkill->IsOverMouse() && NULL == pGame->m_lpPickItem )
				{
					const Skill::ProtoType *lpProtoType = 
						g_SkillMgr.g_SkillMgr.GetSkillProtoType(lpSelectSkill->m_lpItemBase->GetPrototypeID());

					if (NULL != lpProtoType)
					{
						// 패시브 스킬은 선택이 안된다.
						if (lpProtoType->m_eSkillType != Skill::Type::PASSIVE)		
						{
							pGame->m_lpPickItem = new CItemInstance;
							pGame->m_lpPickItem->SetSkillInfo( lpSelectSkill );
							pGame->SetPickItem( pGame->m_lpPickItem ) ;

							CRYLGameScene* pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
							pScene->GetSkillTooltipDlg()->SetVisible( FALSE );
					
							return S_OK ;
						} 
						else
						{
							CRYLChattingDlg::Instance()->AddMessage( CRYLStringTable::m_strString[215], CRYLNetworkData::Instance()->m_dwMyChrID, 0 );
						}
					}
				}
			}
		}
	}

	
	CHAR strString[ MAX_PATH ] ;

	sprintf( strString, "%d / %d", pGame->m_csStatus.m_Skill.GetSkillNum(),  pGame->m_csStatus.GetSkillPoint() ) ;//was .wSkillNum
	m_pSkillPointValueLabel->SetCaption( strString ) ;

	return S_OK ;	
}

HRESULT		CRYLSkillPanel::Render( LPDIRECT3DDEVICE8 lpD3DDevice )  
{

	return S_OK ;
}

HRESULT		CRYLSkillPanel::FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) 
{
	CRYLGameData* pGame = CRYLGameData::Instance() ;

	unsigned long dwPrintSlotNum = 0;
	unsigned long dwClassSkillNum = pGame->m_csStatus.m_lstClassSkill.size();

	// 클래스 스킬 슬롯 렌더링
	if(!m_vecSkillSlot.empty())
	{
		for (unsigned long dwClassIndex = m_dwStartSlot; dwClassIndex < dwClassSkillNum && dwPrintSlotNum < MAX_SLOT_PER_PAGE; ++dwClassIndex)
		{
			RenderClassSlot(lpD3DDevice, pGame->m_csStatus.m_lstClassSkill[ dwClassIndex ], dwPrintSlotNum, 
				m_pGMUIParentDlg->GetLeft() + 11, m_pGMUIParentDlg->GetTop() + 74 + dwPrintSlotNum * 41) ;
			++dwPrintSlotNum;
		}
	}

	if(!m_lstDisplaySkillSlot.empty())
	{
		// 일반 스킬 슬롯 렌더링
		unsigned long dwDisplayIndex = (m_dwStartSlot < dwClassSkillNum) ? 0 : m_dwStartSlot - dwClassSkillNum;
		for (; dwDisplayIndex < m_lstDisplaySkillSlot.size() && dwPrintSlotNum < MAX_SLOT_PER_PAGE; ++dwDisplayIndex)
		{
			RenderSkillSlot(lpD3DDevice, m_lstDisplaySkillSlot[ dwDisplayIndex ], 
				m_pGMUIParentDlg->GetLeft() + 11, m_pGMUIParentDlg->GetTop() + 74 + dwPrintSlotNum * 41) ;

			++dwPrintSlotNum;
		}
	}

	return S_OK ;
}

VOID		CRYLSkillPanel::RenderClassSlot( LPDIRECT3DDEVICE8 lpD3DDevice, CItemInstance *lpSkill, LONG lSlot, 
										     LONG lMoveX, LONG lMoveY )
{
	if (lpSkill)
	{
		lpSkill->m_wPutX = lMoveX + 10 ;
		lpSkill->m_wPutY = lMoveY + 5;
		lpSkill->Render(lpD3DDevice);

		if ( m_vecSkillSlot[ lSlot ]->m_dwSkillClass != ALTERNATIVESKILL )
		{
			m_vecSkillSlot[ lSlot ]->m_pEraseButton->SetVisible( FALSE ) ;
			m_vecSkillSlot[ lSlot ]->m_pSkillProgrss->SetVisible( FALSE ) ;
			m_vecSkillSlot[ lSlot ]->m_pMasterSkillProgrss->SetVisible( FALSE ) ;
			m_vecSkillSlot[ lSlot ]->m_pSkillLevelLabel->SetVisible( FALSE ) ;
		}
	}
}

VOID		CRYLSkillPanel::RenderSkillSlot( LPDIRECT3DDEVICE8 lpD3DDevice, 
											 VIRTUAL_SKILL_SLOT* lpSlot, 
											 LONG lMoveX, LONG lMoveY, bool bAlterSkill ) 
{
	if ( lpSlot->bChild )
	{
		if ( lpSlot->lpItem )
		{
			lpSlot->lpItem->m_wPutX = lMoveX + 31 ;
			lpSlot->lpItem->m_wPutY = lMoveY + 5;
			lpSlot->lpItem->Render(lpD3DDevice, bAlterSkill );
		}
	}
	else
	{
		if ( lpSlot->lpItem )
		{
			lpSlot->lpItem->m_wPutX = lMoveX + 10 ;
			lpSlot->lpItem->m_wPutY = lMoveY + 5;
			lpSlot->lpItem->Render(lpD3DDevice, bAlterSkill );
		}
	}
}

VOID	CRYLSkillPanel::SkillSlotVisible()
{
	CRYLGameData* pGame = CRYLGameData::Instance() ;

	UpdateSkillSlotPosition() ;

	for ( size_t i = 0; i < m_vecSkillSlot.size(); ++i)
	{
		if (m_dwStartSlot <= i && i < m_dwStartSlot + MAX_SLOT_PER_PAGE)
		{
			m_vecSkillSlot[ i ]->SetVisible( TRUE ) ;
			// alternative skill 표기 하지 않는다.
			if ( m_vecSkillSlot[ i ]->m_dwSkillClass != ALTERNATIVESKILL )
			{
				if ( ( i < pGame->m_csStatus.m_lstClassSkill.size() ) && ( m_dwStartSlot == 0 ) )
				{
					m_vecSkillSlot[ i ]->m_pEraseButton->SetVisible( FALSE ) ;
					m_vecSkillSlot[ i ]->m_pSkillProgrss->SetVisible( FALSE ) ;
					m_vecSkillSlot[ i ]->m_pMasterSkillProgrss->SetVisible( FALSE ) ;
					m_vecSkillSlot[ i ]->m_pSkillLevelLabel->SetVisible( FALSE ) ;
				}
				else
				{
					if ( pGame->m_csStatus.m_Skill.SSlot[ i - pGame->m_csStatus.m_lstClassSkill.size() ].SKILLINFO.cSkillLevel == 6)
					{
						m_vecSkillSlot[ i ]->m_pSkillProgrss->SetVisible( FALSE );
						m_vecSkillSlot[ i ]->m_pMasterSkillProgrss->SetVisible( TRUE );
					}
					else
					{
						m_vecSkillSlot[ i ]->m_pSkillProgrss->SetVisible( TRUE );
						m_vecSkillSlot[ i ]->m_pMasterSkillProgrss->SetVisible( FALSE );
					}

					m_vecSkillSlot[ i ]->m_pEraseButton->SetVisible( TRUE ) ;
					m_vecSkillSlot[ i ]->m_pSkillLevelLabel->SetVisible( TRUE ) ;
				}
			}
		}
		else
		{
			m_vecSkillSlot[ i ]->SetVisible( FALSE ) ;
			// alternative skill 표기 하지 않는다.
			if ( m_vecSkillSlot[ i ]->m_dwSkillClass != ALTERNATIVESKILL )
			{
				m_vecSkillSlot[ i ]->m_pEraseButton->SetVisible( FALSE ) ;
				m_vecSkillSlot[ i ]->m_pSkillLevelLabel->SetVisible( FALSE ) ;
			}
		}
	}
}

VOID	CRYLSkillPanel::PrevSkillSlotPage()
{
	if ( m_dwStartSlot != 0 )
	{
		SetStartSlot( m_dwStartSlot -= MAX_SLOT_PER_PAGE ) ;
		SkillSlotVisible() ;
	}
}

VOID	CRYLSkillPanel::NextSkillSlotPage()
{	
	if ( m_vecSkillSlot.size() - m_dwStartSlot > MAX_SLOT_PER_PAGE )
	{
		SetStartSlot( m_dwStartSlot += MAX_SLOT_PER_PAGE ) ;
		SkillSlotVisible() ;
	}
}

VOID	CRYLSkillPanel::DlgClick( CGMUIObject* Sender, INT X, INT Y ) 
{
	if ( !g_pSkillPanel )
		return ;

	switch( Sender->GetInstance() )
	{
	case RYLSKILLPANEL::PREVSKILL_BUTTON :
		g_pSkillPanel->PrevSkillSlotPage() ;		
		break ;
	case RYLSKILLPANEL::NEXTSKILL_BUTTON :
		g_pSkillPanel->NextSkillSlotPage() ;
		break ;
	case RYLSKILLPANEL::ERASE_BUTTON :
		{
			// edith 2008.02.14 스킬 삭제처리로직

			// 망각의돌 검색하기
			g_pSkillPanel->m_lpStatusRetrainItem = NULL;

			CRYLGameData* pGame = CRYLGameData::Instance() ;
			CItemInstance *lpItem = pGame->m_csStatus.GetInventoryItemfromID(Item::EtcItemID::OBLIVION_STONE);		
			if(lpItem)
			{
				g_pSkillPanel->m_lpStatusRetrainItem = lpItem;

				// Yes / No 창 뜨기
				CRYLMessageBox *lpMessageBox;
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create( CRYLStringTable::m_strString[213], MB_YES | MB_NO | MB_EXIT ) ;
				lpMessageBox->SetResult( &g_pSkillPanel->m_dwEraseResult ) ;
				g_pSkillPanel->m_dwEraseResult = 0 ;

				int iSkillBtn   = ( ( CRYLSkillButton* )Sender )->GetSkillIndex() ; 

				int iClassSkill = CRYLGameData::Instance()->m_csStatus.m_lstClassSkill.size() ;
				
				g_pSkillPanel->m_cTmpAddAlterSkillNum   = g_pSkillPanel->GetAlterSkillNum( iSkillBtn ) ;
				
				//INT iProcessIdx = ( ( ( CRYLSkillButton* )Sender )->GetSkillIndex() - 
				//	( CRYLGameData::Instance()->m_csStatus.m_lstClassSkill.size() + g_pSkillPanel->m_cTmpAddAlterSkillNum ) ) ;
									
				g_pSkillPanel->SetProcessSkillIndex( iSkillBtn ) ;
			}
			else
			{
				// 망각의돌이 없으면 메시지 박스
				CRYLMessageBox *lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[1410]);
			}
		}
		break ;

	case RYLSKILLPANEL::QUICKSLOT_BUTTON :
		{
			CRYLGameScene* pScene = static_cast<CRYLGameScene*>( CRYLSceneManager::GetCurrentScene() ) ;
			if ( pScene )
			{
				pScene->GetQuickSlotDlg()->SetVisible( !pScene->GetQuickSlotDlg()->GetVisible() ) ;

				// Visible Flag Setting
				pScene->SetVisibleInfo( RYLMAINWINDOW::QUICKSLOT_WINDOW, pScene->GetQuickSlotDlg()->GetVisible() ) ;
			}
		}
		break ;
	}
}

//==========================================================================
// virtual skill slot
//==========================================================================
VOID	CRYLSkillPanel::InsertVirtualSkill( CItemInstance* lpItem, bool bChild ) 
{
	VIRTUAL_SKILL_SLOT*		lpVSlot = new VIRTUAL_SKILL_SLOT ;
	lpVSlot->lpItem = lpItem ;
	lpVSlot->bChild = bChild ;
	m_lstDisplaySkillSlot.push_back( lpVSlot ) ;
}

CItemInstance*	CRYLSkillPanel::FindVirtualSkill( unsigned short wSkillID ) 
{
	std::vector< VIRTUAL_SKILL_SLOT * >::iterator	iterPos = m_lstDisplaySkillSlot.begin() ; 

	while( iterPos != m_lstDisplaySkillSlot.end() )
	{	
		VIRTUAL_SKILL_SLOT* lpSlot = ( *iterPos ) ;

		if ( lpSlot->lpItem->GetProtoTypeID() == wSkillID )
		{
			return lpSlot->lpItem ;
		}
		++ iterPos ;
	}
	return NULL ;
}

unsigned short	CRYLSkillPanel::FindVirtualIndex( unsigned short wSkillID ) 
{
	unsigned short nCount = 0 ;
	std::vector< VIRTUAL_SKILL_SLOT * >::iterator	iterPos = m_lstDisplaySkillSlot.begin() ; 

	while( iterPos != m_lstDisplaySkillSlot.end() )
	{	
		VIRTUAL_SKILL_SLOT* lpSlot = ( *iterPos ) ;

		if ( lpSlot->lpItem->GetProtoTypeID() == wSkillID )
		{
			return nCount ;
		}
		++ iterPos ;
		++ nCount ;
	}
	return NULL ;
}

VOID	CRYLSkillPanel::DelVirtualSkill( unsigned short wSkillID ) 
{
}

VOID	CRYLSkillPanel::ClearVirtualSkill() 
{
	std::vector< VIRTUAL_SKILL_SLOT * >::iterator	iterPos = m_lstDisplaySkillSlot.begin() ; 
	
	while( iterPos != m_lstDisplaySkillSlot.end() )
	{	
		VIRTUAL_SKILL_SLOT* lpSlot = ( *iterPos ) ;
		
		if ( lpSlot->bChild )
		{
			GM_DELETE( lpSlot->lpItem ) ;
		}

		GM_DELETE( lpSlot ) ;
		++ iterPos ;
	}

	m_lstDisplaySkillSlot.clear() ;
}

void	CRYLSkillPanel::SetBindingItem( CItemInstance* lpItem, int iIndex ) 
{
	unsigned char cSlot = iIndex + GetAlterSkillNum( iIndex ) ;

	m_lstDisplaySkillSlot[ cSlot ]->lpItem = lpItem ;//that works
}