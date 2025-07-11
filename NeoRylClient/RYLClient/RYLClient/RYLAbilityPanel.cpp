#include <winsock2.h>
#include "RYLAbilityPanel.h"
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
// AbilitySlot
CRYLAbilitySlot*		g_AbilitySlot = NULL ;
CRYLAbilitySlot::CRYLAbilitySlot( CGMUICustomDlg *pParentDlg ) : CGMUICustomPanel( pParentDlg )
{
	m_pGMUIParentDlg = pParentDlg ;
	g_AbilitySlot		 = this ;

	m_pAbilitySprite			= NULL ;
	m_pAbilityNameLabel		= NULL ;
	m_pAbilityLevelLabel		= NULL ;
	m_pAbilityProgrss			= NULL ;
	m_pMasterAbilityProgrss	= NULL ;

	m_iAbilitySlotIndex		= 0 ;
}

CRYLAbilitySlot::~CRYLAbilitySlot() 
{
	GM_DELETE( m_pAbilityNameLabel ) ;
	GM_DELETE( m_pAbilityLevelLabel ) ;
	GM_DELETE( m_pAbilitySprite ) ;
	GM_DELETE( m_pAbilityProgrss ) ;	
	GM_DELETE( m_pMasterAbilityProgrss ) ;
}

VOID		CRYLAbilitySlot::CreateAbilityName( LPSTR lpAbilityName )
{
	m_pAbilityNameLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pAbilityNameLabel->SetLeft( 43 ) ;
	m_pAbilityNameLabel->SetTop( 53 ) ;

	m_pAbilityNameLabel->SetFontColor( 0xffffffff ) ;
	m_pAbilityNameLabel->SetAutoSize( TRUE ) ;
	m_pAbilityNameLabel->SetCaption( lpAbilityName ) ;
	CGMUICustomPanel::AddChild( m_pAbilityNameLabel ) ;
}

VOID		CRYLAbilitySlot::SetAbilityName( LPSTR lpAbilityName ) 
{
	m_pAbilityNameLabel->SetCaption( lpAbilityName ) ;
}

VOID		CRYLAbilitySlot::CreateAbilityLevel( LPSTR lpAbilityName )
{
	m_pAbilityLevelLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pAbilityLevelLabel->SetLeft( 65 ) ;
	m_pAbilityLevelLabel->SetTop( 61 ) ;
	m_pAbilityLevelLabel->SetFontColor( 0xffffffff ) ;
	m_pAbilityLevelLabel->SetAutoSize( TRUE ) ;
	m_pAbilityLevelLabel->SetCaption( lpAbilityName ) ;
	CGMUICustomPanel::AddChild( m_pAbilityLevelLabel ) ;
}

VOID		CRYLAbilitySlot::SetAbilityLevel( DWORD dwLevel )
{
	CHAR	strString[ MAX_PATH ] ;
	sprintf( strString, "%d", dwLevel ) ;

	m_pAbilityLevelLabel->SetCaption( strString ) ;
}


VOID		CRYLAbilitySlot::MakeAbilitySlot( INT iAbilityKind  ) 
{
	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;

	// Background Image
	SetWidth( 233 ) ;
	SetHeight( 42 ) ;
	CRYLSpriteList::Instance()->CreateGMImage( 233, 42, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;

	RECT			rt ;
	SetRect( &rt, 0, 84,  233, 126 ) ;
	// 배경이미지
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_SKILLSLOT_256x256 ) ) ;
/*	
	// 이건 LV 이미지와 프로그래시브가 들어갈 영역을 그리는 루틴이다.
	SetRect( &rt, 88, 168, 110, 182 ) ;
	pImageDIB->DrawRect( &rt, 43, 22, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_SKILLSLOT_256x256 ) ) ;
	SetRect( &rt, 110, 168, 204, 181 ) ;
	pImageDIB->DrawRect( &rt, 86, 23, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_SKILLSLOT_256x256 ) ) ;
*/
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	AttachPanelImage( ( CGMTexture*)pImage ) ;
	GM_DELETE( pImageDIB ) ;
}


VOID		CRYLAbilitySlot::CreateAbilityProgrssBar( INT iLevel ) 
{
	RECT rt ;
	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;

	m_pAbilityProgrss = new CRYLProgressBar( m_pGMUIParentDlg ) ;
	m_pAbilityProgrss->SetLeft( 89 ) ;
	m_pAbilityProgrss->SetTop( 62 ) ;
	m_pAbilityProgrss->SetWidth( 90 ) ;
	m_pAbilityProgrss->SetHeight( 9 ) ;
	m_pAbilityProgrss->SetMax( 5 ) ;
	
	CRYLSpriteList::Instance()->CreateGMImage( 90, 9, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 168, 88, 175 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_SKILLSLOT_256x256 ) ) ;
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	m_pAbilityProgrss->AttachBarImageTexture( ( CGMTexture*)pImage ) ;
	m_pAbilityProgrss->SetPosition( iLevel ) ;
	
	CGMUICustomPanel::AddChild( m_pAbilityProgrss ) ;
	GM_DELETE( pImageDIB ) ;
}

VOID	CRYLAbilitySlot::SetAbilityLockCount( INT iCount )
{
	if ( iCount == 6 )
	{
		m_pAbilityProgrss->SetVisible( FALSE ) ;
		m_pMasterAbilityProgrss->SetVisible( TRUE ) ;
	}
	else
	{
		m_pAbilityProgrss->SetPosition( iCount ) ;
		m_pAbilityProgrss->SetVisible( TRUE ) ;
		m_pMasterAbilityProgrss->SetVisible( FALSE ) ;
	}

	// 어빌리티는 프로그래시브 바가 없다.
	m_pAbilityProgrss->SetVisible( FALSE ) ;
	m_pMasterAbilityProgrss->SetVisible( FALSE ) ;
}

VOID	CRYLAbilitySlot::CreateMasterAbilityProgrssBar() 
{
	RECT rt ;
	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;

	m_pMasterAbilityProgrss = new CRYLProgressBar( m_pGMUIParentDlg ) ;
	m_pMasterAbilityProgrss->SetLeft( 89 ) ;
	m_pMasterAbilityProgrss->SetTop( 62 ) ;
	m_pMasterAbilityProgrss->SetWidth( 90 ) ;
	m_pMasterAbilityProgrss->SetHeight( 9 ) ;
	m_pMasterAbilityProgrss->SetMax( 5 ) ;
	
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
			m_pMasterAbilityProgrss->AttachBarImageTexture( ( CGMTexture*)pImage ) ;
		}
		
		m_pMasterAbilityProgrss->SetPosition( 5 ) ;
		CGMUICustomPanel::AddChild( m_pMasterAbilityProgrss ) ;

		GM_DELETE( pImageDIB ) ;
	}	
}


//-----------------------------------------------------------------------------
// Ability Button
CRYLAbilityButton::CRYLAbilityButton( CGMUICustomDlg *pParentDlg ) : CRYLButton( pParentDlg )
{

}
CRYLAbilityButton::~CRYLAbilityButton() 
{
}

//-----------------------------------------------------------------------------
// RYLAbilityPanel
CRYLAbilityPanel*	g_pAbilityPanel = NULL ;
CRYLAbilityPanel::CRYLAbilityPanel( CGMUICustomDlg *pParentDlg ) : CGMUICustomPanel( pParentDlg )
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
	g_pAbilityPanel = this ;

	m_iAbilityYMargin = 0 ;
	m_dwStartSlot   = 0 ;
	m_dwEraseResult = 0 ;
	
	m_dwProcessAbilityIndex	= 0 ;
	m_dwProcessAbilityPosIndex = 0;

	m_cAbilitySlotIndex		= 0 ;
	m_pPrevAbilityButton			= NULL ;
	m_pNextAbilityButton			= NULL ;
	m_pAbilityPointLabel			= NULL ; // 스킬 레벨
	m_pAbilityPointValueLabel		= NULL ; // 스킬 레벨 값

	m_lpStatusRetrainItem = NULL;	// 망각의돌 아이템 변수
}

CRYLAbilityPanel::~CRYLAbilityPanel() 
{
	FinalCtrl() ;
}


VOID	CRYLAbilityPanel::InitCtrl() 
{
	RECT rt ;
	CGMImageDib* pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT02_256x256 ) ;

	m_pPrevAbilityButton = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pPrevAbilityButton->SetLeft( 220 ) ;
	m_pPrevAbilityButton->SetTop( 420 ) ;
	m_pPrevAbilityButton->OnClickEvent = DlgClick ;
	m_pPrevAbilityButton->SetInstance( RYLABILITYPANEL::PREVABILITY_BUTTON ) ;
	SetRect( &rt, 217, 215, 230, 227 ) ;
	m_pPrevAbilityButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 217, 243, 230, 255 ) ;
	m_pPrevAbilityButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 217, 229, 230, 241 ) ;
	m_pPrevAbilityButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 217, 215, 230, 227 ) ;
	m_pPrevAbilityButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	CGMUICustomPanel::AddChild( m_pPrevAbilityButton ) ;

	m_pNextAbilityButton = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pNextAbilityButton->SetLeft( 235 ) ;
	m_pNextAbilityButton->SetTop( 420 ) ;
	m_pNextAbilityButton->OnClickEvent = DlgClick ;
	m_pNextAbilityButton->SetInstance( RYLABILITYPANEL::NEXTABILITY_BUTTON ) ;
	SetRect( &rt, 232, 215, 245, 227 ) ;
	m_pNextAbilityButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 232, 243, 245, 255 ) ;
	m_pNextAbilityButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 232, 229, 245, 241 ) ;
	m_pNextAbilityButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 232, 215, 245, 227 ) ;
	m_pNextAbilityButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	CGMUICustomPanel::AddChild( m_pNextAbilityButton ) ;

	m_pAbilityPointLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pAbilityPointLabel->SetLeft( 16 ) ;
	m_pAbilityPointLabel->SetTop( 8 ) ;
	m_pAbilityPointLabel->SetFontColor( 0xffffffff ) ;
	SetRect( &rt, 17, 8, 80, 19 ) ;
	m_pAbilityPointLabel->SetAlignRect( rt ) ;
	m_pAbilityPointLabel->SetAlign( CENTER ) ;
	m_pAbilityPointLabel->SetCaption( CRYLStringTable::m_strString[ 2081 ] ) ;
	m_pAbilityPointLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pAbilityPointLabel ) ;

	m_pAbilityPointValueLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pAbilityPointValueLabel->SetLeft( 98 ) ;
	m_pAbilityPointValueLabel->SetTop( 8 ) ;
	m_pAbilityPointValueLabel->SetFontColor( 0xffffffff ) ;
	SetRect( &rt, 84, 8, 151, 19 ) ;
	m_pAbilityPointValueLabel->SetAlignRect( rt ) ;
	m_pAbilityPointValueLabel->SetAlign( CENTER ) ;
	m_pAbilityPointValueLabel->SetCaption( "" ) ;
	m_pAbilityPointValueLabel->SetAutoSize( TRUE ) ;
	CGMUICustomPanel::AddChild( m_pAbilityPointValueLabel ) ;
}

VOID	CRYLAbilityPanel::FinalCtrl() 
{
	GM_DELETE( m_pPrevAbilityButton ) ;
	GM_DELETE( m_pNextAbilityButton ) ;
	GM_DELETE( m_pAbilityPointLabel ) ;
	GM_DELETE( m_pAbilityPointValueLabel ) ;

	ClearAbilitySlot() ;
	ClearVirtualAbility() ;
}

VOID		CRYLAbilityPanel::ClearAbilitySlot() 
{
	if ( m_vecAbilitySlot.empty() )
		return ;

	for ( std::vector< CRYLAbilitySlot* >::iterator it = m_vecAbilitySlot.begin() ; it != m_vecAbilitySlot.end() ;++ it )
	{
		CGMUICustomPanel::DeleteChild( ( *it )->m_pEraseButton ) ;
		GM_DELETE( ( *it )->m_pEraseButton ) ;

		CGMUICustomPanel::DeleteChild( ( *it ) ) ;
		GM_DELETE( (*it ) ) ;
	}

	m_vecAbilitySlot.clear() ;

	CRYLGameData* pGame = CRYLGameData::Instance() ;
	pGame->m_csStatus.SetUseAbilityPoint(0);	
}

VOID		CRYLAbilityPanel::InsertAbilitySlot( CItemInstance* lpAbility, unsigned short wAbilityID, unsigned char cGrade, unsigned char cLevel, 
											 unsigned char cPos, unsigned char cIndex )  
{
	CGMImageDib* pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;
	CRYLGameData* pGame = CRYLGameData::Instance() ;


	const Skill::ProtoType *pSkillType = g_SkillMgr.GetSkillProtoType(wAbilityID);
	if (pSkillType != NULL)
	{
		for(int i = 0; i <= cGrade; ++i)
		{
			short wAbilityPoint = pGame->m_csStatus.GetUseAbilityPoint()+pSkillType[i].m_fMinRange;	// 사용되는 어빌리티 포인트
			pGame->m_csStatus.SetUseAbilityPoint(wAbilityPoint);	
		}
	}


	CRYLAbilitySlot* pAbility = new CRYLAbilitySlot( m_pGMUIParentDlg ) ;

	pAbility->m_wAbilityID = wAbilityID;
	pAbility->m_wAbilityLV = cGrade;

	pAbility->SetLeft( 11 ) ;
	pAbility->SetTop( m_iAbilityYMargin + 34 ) ;
	
	pAbility->m_pEraseButton = new CRYLAbilityButton( m_pGMUIParentDlg ) ;
	pAbility->m_pEraseButton->SetLeft( 195 ) ;
	pAbility->m_pEraseButton->SetTop( 43 + m_iAbilityYMargin ) ;
	pAbility->m_pEraseButton->SetCaption( CRYLStringTable::m_strString[ 2094 ] ) ;
	pAbility->m_pEraseButton->SetFontColor( 0xffffffff ) ;
	pAbility->m_pEraseButton->SetColor( 0xffffffff ) ;
	pAbility->m_pEraseButton->OnClickEvent = DlgClick ;
	pAbility->m_pEraseButton->SetInstance( RYLABILITYPANEL::ERASE_BUTTON ) ;
	pAbility->m_pEraseButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 2 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	pAbility->m_pEraseButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 2 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	pAbility->m_pEraseButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 2 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	pAbility->m_pEraseButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 2 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	pAbility->m_pEraseButton->SetAbilityPosIndex( cIndex ) ;
	pAbility->m_pEraseButton->SetAbilityIndex( m_cAbilitySlotIndex ) ;
	pAbility->m_pEraseButton->SetVisible( FALSE ) ;

	CGMUICustomPanel::AddChild( pAbility->m_pEraseButton ) ;

	CHAR	strString[ MAX_PATH ] ;
	strcpy( strString, lpAbility->GetItemInfo().m_SpriteData.m_szName ) ;
	if (strlen(strString) > 21)
	{
		strString[21] = '.';
		strString[22] = '.';
		strString[23] = '.';
		strString[24] = '\0';
	}
	pAbility->CreateAbilityName( strString ) ;
	sprintf( strString, "%d", lpAbility->GetNowDurability() ) ;
	pAbility->CreateAbilityLevel( strString ) ;
	pAbility->CreateAbilityProgrssBar( lpAbility->GetNowDurability() ) ;
	pAbility->CreateMasterAbilityProgrssBar() ; 
	pAbility->SetInstance( RYLABILITYPANEL::ABILITYSLOT_PANEL ) ;
	pAbility->MakeAbilitySlot( lpAbility->GetSkillType() ) ;
	pAbility->SetAbilitySlotIndex( m_cAbilitySlotIndex ) ;
	pAbility->SetVisible( FALSE ) ;
	pAbility->m_pMasterAbilityProgrss->SetVisible( FALSE ) ;
	CGMUICustomPanel::AddChild( pAbility ) ;

	m_iAbilityYMargin += 41 ;
	++ m_cAbilitySlotIndex ;
	
	m_vecAbilitySlot.push_back( pAbility ) ;
} 

VOID	CRYLAbilityPanel::SetAbilitySlot( CItemInstance* lpAbilityInstance, INT iKInd, int iIndex )  
{
	CRYLGameData* pGame = CRYLGameData::Instance() ;

	INT iAbilityIndex = 0 ; 

	switch ( iKInd )
	{
	case EDIT_ABILITY : // 수정
		{
			iAbilityIndex = m_dwProcessAbilityIndex;
		}
		break ;
	case DEL_ABILITYSLOT : // 삭제
		{
			iAbilityIndex = m_dwProcessAbilityIndex;
		}
		break ;
	}

	std::vector< CRYLAbilitySlot* >::iterator it = m_vecAbilitySlot.begin() ;
	CRYLAbilitySlot* lpAbilitySlot = NULL ;
	for ( std::vector< CRYLAbilitySlot* >::iterator it = m_vecAbilitySlot.begin() ; it != m_vecAbilitySlot.end() ; ++ it )
	{
		if ( ( *it )->GetAbilitySlotIndex() == iAbilityIndex )
		{
			lpAbilitySlot = ( *it ) ;
			break ;
		}
	}

	if ( !lpAbilitySlot ) return ;

	const Skill::ProtoType *pSkillType = g_SkillMgr.GetSkillProtoType(lpAbilitySlot->m_wAbilityID);
	if (pSkillType != NULL)
	{
		if(iKInd == EDIT_ABILITY)
		{
			lpAbilitySlot->m_wAbilityLV++;
			short wAbilityPoint = pGame->m_csStatus.GetUseAbilityPoint()+pSkillType[lpAbilitySlot->m_wAbilityLV].m_fMinRange;	// 사용되는 어빌리티 포인트
			pGame->m_csStatus.SetUseAbilityPoint(wAbilityPoint);	
		}
		else if(iKInd == DEL_ABILITYSLOT)
		{
			short wAbilityPoint = pGame->m_csStatus.GetUseAbilityPoint()-pSkillType[lpAbilitySlot->m_wAbilityLV].m_fMinRange;	// 사용되는 어빌리티 포인트
			pGame->m_csStatus.SetUseAbilityPoint(wAbilityPoint);	
			lpAbilitySlot->m_wAbilityLV--;
		}
	}

	CHAR	strString[ MAX_PATH ] ;
	strcpy( strString, lpAbilityInstance->GetItemInfo().m_SpriteData.m_szName ) ;
	if (strlen(strString) > 21)
	{
		strString[21] = '.' ;
		strString[22] = '.' ;
		strString[23] = '.' ;
		strString[24] = '\0' ;
	}
	lpAbilitySlot->SetAbilityName( strString ) ;

	lpAbilitySlot->SetAbilityLevel( lpAbilityInstance->GetNowDurability() ) ;
	lpAbilitySlot->SetAbilityLockCount( lpAbilityInstance->GetNowDurability() ) ;
}

VOID		CRYLAbilityPanel::UpdateAbilitySlotPosition()
{
	if ( m_vecAbilitySlot.empty() ) return ;

	INT iTempY = 0;
	INT	iCount = 0 ;
	CRYLAbilitySlot* lpSlot = NULL ;
	
	for ( std::vector< CRYLAbilitySlot* >::iterator it = m_vecAbilitySlot.begin() ; it != m_vecAbilitySlot.end() ; ++ it )
	{
		if ( ( iCount % MAX_SLOT_PER_PAGE ) == 0 )
		{
			iTempY = 0 ;
		}

		lpSlot = ( *it ) ;
		lpSlot->SetTop( iTempY + 34 ) ;
		
		lpSlot->m_pEraseButton->SetTop( iTempY + 43 ) ;
		lpSlot->m_pEraseButton->SetAbilityIndex( iCount ) ;

		lpSlot->SetAbilitySlotIndex( iCount ) ;
		
		iTempY += 41 ;
		++ iCount ;
	}
	m_iAbilityYMargin = iTempY ;
}

VOID		CRYLAbilityPanel::DeleteCurrentAbilitySlot() 
{
	//=======================================================================
	// delete virtual slot
	//=======================================================================
	std::vector< VIRTUAL_ABILITY_SLOT * >::iterator	iterPos = m_lstDisplayAbilitySlot.begin() ; 
	unsigned short wSlotIdx = 0 ;
	while( iterPos != m_lstDisplayAbilitySlot.end() )
	{
		CItemInstance* lpItem = ( * iterPos )->lpItem ;
		if ( NULL != lpItem )
		{
			if ( lpItem->GetProtoTypeID() == CRYLGameData::Instance()->m_csStatus.m_Skill.SSlot[ m_dwProcessAbilityPosIndex ].SKILLINFO.wSkill )
			{
				unsigned short wAbilityID = lpItem->GetProtoTypeID() ;
				const Skill::ProtoType *lpAbilityBase    = g_SkillMgr.GetSkillProtoType( wAbilityID ) ;
				
				iterPos = m_lstDisplayAbilitySlot.erase( iterPos ) ;	
				break ;
			}
		}
		++ wSlotIdx ;
		++ iterPos ;
	}

	INT iAbilityIndex = m_dwProcessAbilityIndex;

	std::vector< CRYLAbilitySlot* >::iterator it = m_vecAbilitySlot.begin() ;
	CRYLAbilitySlot* lpAbilitySlot = NULL ;
	for ( it = m_vecAbilitySlot.begin() ; it != m_vecAbilitySlot.end() ; ++ it )
	{
		if ( ( *it )->GetAbilitySlotIndex() == iAbilityIndex )
		{
			lpAbilitySlot = ( *it ) ;
			it = m_vecAbilitySlot.erase( it ) ;
			DeleteAbilitySlot( lpAbilitySlot ) ;
			break ;
		}
	}

	AbilitySlotVisible();
}

void		CRYLAbilityPanel::DeleteAbilitySlot( CRYLAbilitySlot* lpAbilitySlot ) 
{
	if ( !lpAbilitySlot ) return ;


	CGMUICustomPanel::DeleteChild( lpAbilitySlot ) ;
	CGMUICustomPanel::DeleteChild( lpAbilitySlot->m_pEraseButton ) ;
}

HRESULT		CRYLAbilityPanel::Update( BOOL &bClick, BOOL &bEdge )
{
	CRYLGameData* pGame = CRYLGameData::Instance() ;
	//////// 스킬 지우기
	if ( m_dwEraseResult & MB_YES )
	{
		Item::ItemPos* temp= new Item::ItemPos(0,0);//hz delete ability doesnt require oblivion stone
		// edith 2008.02.14 스킬 삭제로직
		g_pClientSocket->SetStatusFlag( NS_SKILLERASE ) ;
		SendPacket::CharSkillErase( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID,
									pGame->m_csStatus.m_Skill.SSlot[ m_dwProcessAbilityPosIndex ].SKILLINFO.wSkill,
									m_dwProcessAbilityPosIndex, /*&m_lpStatusRetrainItem->m_lpItemBase->GetPos()*/temp ) ;//hz delete ability doesnt require oblivion stone was without the /**/
		delete temp;//hz delete ability doesnt require oblivion stone

		m_dwEraseResult = 0 ;
		return S_OK ;
	} 
	else if ( m_dwEraseResult & MB_NO || m_dwEraseResult & MB_EXIT )
	{
		m_dwEraseResult = 0 ;
		return TRUE ;
	}

	POINT *ptMousePos = g_DeviceInput.GetMouseLocal() ;

	long lStartAbility = 0;
	unsigned long dwSlot = 0 ;
	unsigned long dwFunction ;
	if ( g_DeviceInput.GetIsLeftMouseDown() )
	{
		if ( !pGame->CheckStillCasting( dwFunction ) )
		{
			unsigned short wStart = m_dwStartSlot ;
			unsigned short wMaxSlot = m_lstDisplayAbilitySlot.size();
			unsigned short wCurrPageMaxSlot = wMaxSlot - wStart ;

			if ( wCurrPageMaxSlot > MAX_SLOT_PER_PAGE )
				wCurrPageMaxSlot = wStart + MAX_SLOT_PER_PAGE ;
			else
				wCurrPageMaxSlot += wStart ;
/*			
			for ( unsigned short i = wStart ; i < wCurrPageMaxSlot ; i ++ )
			{
				CItemInstance* lpSelectAbility = NULL;
				lpSelectAbility = pGame->m_csStatus.m_lstClassSkill[ i ];

				const Skill::ProtoType *lpSelectAbilityBase    = g_SkillMgr.GetSkillProtoType( lpSelectAbility->m_wPreProtoTypeID ) ;
				unsigned short usParentID = 0;

				if ( lpSelectAbilityBase )
				{
					if(lpSelectAbilityBase->m_usParentSkill)
					{
						for ( unsigned short i = wStart ; i < wCurrPageMaxSlot; i ++ )
						{
							CItemInstance* lpSelectAbilityParent = NULL;
							lpSelectAbilityParent = pGame->m_csStatus.m_lstClassSkill[ i ];

							if(lpSelectAbilityBase->m_usParentSkill == 
								lpSelectAbilityParent->m_wPreProtoTypeID)
							{
								lpSelectAbility->m_cCountCooldown = lpSelectAbilityParent->m_cCountCooldown;
								lpSelectAbility->m_lCoolDownTimeCount = lpSelectAbilityParent->m_lCoolDownTimeCount;
								lpSelectAbility->m_lCoolDownTime = lpSelectAbilityParent->m_lCoolDownTime;
								memcpy(lpSelectAbility->m_pCoolDownVertices, lpSelectAbilityParent->m_pCoolDownVertices, 10);
								memcpy(lpSelectAbility->m_wCoolDownIndex, lpSelectAbilityParent->m_wCoolDownIndex, 24);
							}
						}
					}
				}


				if ( NULL != lpSelectAbility && lpSelectAbility->IsOverMouse() && NULL == pGame->m_lpPickItem )
				{
					const Skill::ProtoType *lpProtoType = 
						g_SkillMgr.g_SkillMgr.GetSkillProtoType(lpSelectAbility->m_lpItemBase->GetPrototypeID());

					if (NULL != lpProtoType)
					{
						// 패시브 스킬은 선택이 안된다.
						pGame->m_lpPickItem = new CItemInstance;
						pGame->m_lpPickItem->SetSkillInfo( lpSelectAbility );
						pGame->SetPickItem( pGame->m_lpPickItem ) ;

						CRYLGameScene* pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
						pScene->GetSkillTooltipDlg()->SetVisible( FALSE );
				
						return S_OK ;
					}
				}
			}
*/
		}
	}

	
	CHAR strString[ MAX_PATH ] ;

	int AbilityPoints=0;
	if(pGame->m_csStatus.GetLevel() > 40)
	{
		AbilityPoints=pGame->m_csStatus.GetLevel();
		AbilityPoints-=40;
	}

	sprintf( strString, "%d / %d", pGame->m_csStatus.GetUseAbilityPoint(),  AbilityPoints ) ;//was pGame->m_csStatus.GetAbilityPoint()
	m_pAbilityPointValueLabel->SetCaption( strString ) ;

	return S_OK ;	
}

HRESULT		CRYLAbilityPanel::Render( LPDIRECT3DDEVICE8 lpD3DDevice )  
{

	return S_OK ;
}

HRESULT		CRYLAbilityPanel::FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) 
{
	CRYLGameData* pGame = CRYLGameData::Instance() ;

	unsigned long dwPrintSlotNum = 0;

	// 일반 스킬 슬롯 렌더링
	for (unsigned long dwDisplayIndex = 0; dwDisplayIndex < m_lstDisplayAbilitySlot.size() && dwPrintSlotNum < MAX_SLOT_PER_PAGE; ++dwDisplayIndex)
	{
		RenderAbilitySlot(lpD3DDevice, m_lstDisplayAbilitySlot[ dwDisplayIndex ], 
			m_pGMUIParentDlg->GetLeft() + 11, m_pGMUIParentDlg->GetTop() + 74 + dwPrintSlotNum * 41) ;

		++dwPrintSlotNum;
	}

	return S_OK ;
}

VOID		CRYLAbilityPanel::RenderAbilitySlot( LPDIRECT3DDEVICE8 lpD3DDevice, 
											 VIRTUAL_ABILITY_SLOT* lpSlot, 
											 LONG lMoveX, LONG lMoveY, bool bAlterAbility ) 
{
	if ( lpSlot->lpItem )
	{
		lpSlot->lpItem->m_wPutX = lMoveX + 10 ;
		lpSlot->lpItem->m_wPutY = lMoveY + 5;
		lpSlot->lpItem->Render(lpD3DDevice, bAlterAbility );
	}
}

VOID	CRYLAbilityPanel::AbilitySlotVisible()
{
	CRYLGameData* pGame = CRYLGameData::Instance() ;

	UpdateAbilitySlotPosition() ;

	for ( size_t i = 0; i < m_vecAbilitySlot.size(); ++i)
	{
		if (m_dwStartSlot <= i && i < m_dwStartSlot + MAX_SLOT_PER_PAGE)
		{
			m_vecAbilitySlot[ i ]->SetVisible( TRUE ) ;

			m_vecAbilitySlot[ i ]->m_pAbilityProgrss->SetVisible( TRUE );
			m_vecAbilitySlot[ i ]->m_pMasterAbilityProgrss->SetVisible( FALSE );
			m_vecAbilitySlot[ i ]->m_pEraseButton->SetVisible( TRUE ) ;
			m_vecAbilitySlot[ i ]->m_pAbilityLevelLabel->SetVisible( TRUE ) ;
		}
		else
		{
			m_vecAbilitySlot[ i ]->SetVisible( FALSE ) ;

			m_vecAbilitySlot[ i ]->m_pEraseButton->SetVisible( FALSE ) ;
			m_vecAbilitySlot[ i ]->m_pAbilityLevelLabel->SetVisible( FALSE ) ;
		}

		// 어빌리티는 프로그래시브가 필요없다.
		m_vecAbilitySlot[ i ]->m_pAbilityProgrss->SetVisible( FALSE );
		m_vecAbilitySlot[ i ]->m_pMasterAbilityProgrss->SetVisible( FALSE );
		m_vecAbilitySlot[ i ]->m_pAbilityLevelLabel->SetVisible( FALSE ) ;
	}
}

VOID	CRYLAbilityPanel::PrevAbilitySlotPage()
{
	if ( m_dwStartSlot != 0 )
	{
		SetStartSlot( m_dwStartSlot -= MAX_SLOT_PER_PAGE ) ;
		AbilitySlotVisible() ;
	}
}

VOID	CRYLAbilityPanel::NextAbilitySlotPage()
{	
	if ( m_vecAbilitySlot.size() - m_dwStartSlot > MAX_SLOT_PER_PAGE )
	{
		SetStartSlot( m_dwStartSlot += MAX_SLOT_PER_PAGE ) ;
		AbilitySlotVisible() ;
	}
}

VOID	CRYLAbilityPanel::DlgClick( CGMUIObject* Sender, INT X, INT Y ) 
{
	if ( !g_pAbilityPanel )
		return ;

	switch( Sender->GetInstance() )
	{
	case RYLABILITYPANEL::PREVABILITY_BUTTON :
		g_pAbilityPanel->PrevAbilitySlotPage() ;		
		break ;
	case RYLABILITYPANEL::NEXTABILITY_BUTTON :
		g_pAbilityPanel->NextAbilitySlotPage() ;
		break ;
	case RYLABILITYPANEL::ERASE_BUTTON :
		{
			// edith 2008.02.14 스킬 삭제처리로직

			// 망각의돌 검색하기
			g_pAbilityPanel->m_lpStatusRetrainItem = NULL;

			CRYLGameData* pGame = CRYLGameData::Instance() ;
			//CItemInstance *lpItem = pGame->m_csStatus.GetInventoryItemfromID(Item::EtcItemID::OBLIVION_STONE);		//hz delete ability doesnt request oblivion stone
			if(1)//if(lpItem)//hz delete ability doesnt request oblivion stone
			{
			//	g_pAbilityPanel->m_lpStatusRetrainItem = lpItem;//hz delete ability doesnt request oblivion stone

				// Yes / No 창 뜨기
				CRYLMessageBox *lpMessageBox;
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create( CRYLStringTable::m_strString[213], MB_YES | MB_NO | MB_EXIT ) ;
				lpMessageBox->SetResult( &g_pAbilityPanel->m_dwEraseResult ) ;
				g_pAbilityPanel->m_dwEraseResult = 0 ;

				int iAbilityBtn   = ( ( CRYLAbilityButton* )Sender )->GetAbilityIndex() ; 
				int iAbilityPos   = ( ( CRYLAbilityButton* )Sender )->GetAbilityPosIndex() ; 
			
				g_pAbilityPanel->SetProcessAbilityIndex( iAbilityBtn ) ;
				g_pAbilityPanel->SetProcessAbilityPosIndex( iAbilityPos ) ;
			}
			else
			{
				// 망각의돌이 없으면 메시지 박스
				CRYLMessageBox *lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[1410]);
			}
		}
		break ;
	}
}

BOOL CRYLAbilityPanel::CheckAbility(unsigned short wAbilityID, unsigned short wAbilityLV)
{
	for ( size_t i = 0; i < m_vecAbilitySlot.size(); ++i)
	{
		CRYLAbilitySlot* pSlot = m_vecAbilitySlot[ i ];
		if(m_vecAbilitySlot[ i ]->m_wAbilityID == wAbilityID && 
			m_vecAbilitySlot[ i ]->m_wAbilityLV >= wAbilityLV)
			return TRUE;
	}

	return FALSE;
}

//==========================================================================
// virtual skill slot
//==========================================================================
VOID	CRYLAbilityPanel::InsertVirtualAbility( CItemInstance* lpItem ) 
{
	VIRTUAL_ABILITY_SLOT*		lpVSlot = new VIRTUAL_ABILITY_SLOT ;
	lpVSlot->lpItem = lpItem ;
	m_lstDisplayAbilitySlot.push_back( lpVSlot ) ;
}

CItemInstance*	CRYLAbilityPanel::FindVirtualAbility( unsigned short wAbilityID ) 
{
	std::vector< VIRTUAL_ABILITY_SLOT * >::iterator	iterPos = m_lstDisplayAbilitySlot.begin() ; 

	while( iterPos != m_lstDisplayAbilitySlot.end() )
	{	
		VIRTUAL_ABILITY_SLOT* lpSlot = ( *iterPos ) ;

		if ( lpSlot->lpItem->GetProtoTypeID() == wAbilityID )
		{
			return lpSlot->lpItem ;
		}
		++ iterPos ;
	}
	return NULL ;
}

unsigned short	CRYLAbilityPanel::FindVirtualIndex( unsigned short wAbilityID ) 
{
	unsigned short nCount = 0 ;
	std::vector< VIRTUAL_ABILITY_SLOT * >::iterator	iterPos = m_lstDisplayAbilitySlot.begin() ; 

	int iszie = m_lstDisplayAbilitySlot.size();

	while( iterPos != m_lstDisplayAbilitySlot.end() )
	{	
		VIRTUAL_ABILITY_SLOT* lpSlot = ( *iterPos ) ;

		if ( lpSlot->lpItem->GetProtoTypeID() == wAbilityID )
		{
			return nCount ;
		}
		++ iterPos ;
		++ nCount ;
	}
	return NULL ;
}

VOID	CRYLAbilityPanel::DelVirtualAbility( unsigned short wAbilityID ) 
{
}

VOID	CRYLAbilityPanel::ClearVirtualAbility() 
{
	std::vector< VIRTUAL_ABILITY_SLOT * >::iterator	iterPos = m_lstDisplayAbilitySlot.begin() ; 
	
	while( iterPos != m_lstDisplayAbilitySlot.end() )
	{	
		VIRTUAL_ABILITY_SLOT* lpSlot = ( *iterPos ) ;
		
		GM_DELETE( lpSlot ) ;
		++ iterPos ;
	}

	m_lstDisplayAbilitySlot.clear() ;
}

void	CRYLAbilityPanel::SetBindingItem( CItemInstance* lpItem, int iIndex ) 
{
	unsigned char cSlot = m_dwProcessAbilityIndex;

	m_lstDisplayAbilitySlot[ cSlot ]->lpItem = lpItem ;
}