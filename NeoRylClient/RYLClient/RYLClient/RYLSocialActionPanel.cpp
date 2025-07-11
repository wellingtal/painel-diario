#include <winsock2.h>
#include "RYLSocialActionPanel.h"
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
#include "RYLLabel.h"

#include "WinInput.h"
#include "..\\GUITooltipItem.h"
#include "RYLMessageBox.h"
#include "..\\ItemInstance.h"

#include <Item/ItemMgr.h>
#include <Item/ItemFactory.h>
#include "ItemInstance.h"
#include <Skill/SkillMgr.h>
#include "RYLGameScene.h"
#include "RYLSceneManager.h"
#include "GMMemory.h"


//-----------------------------------------------------------------------------
// SkillSlot
CRYLSocialActionSlot::CRYLSocialActionSlot( CGMUICustomDlg *pParentDlg ) : CGMUICustomPanel( pParentDlg )
{
	m_pGMUIParentDlg = pParentDlg ;
	m_pActionNameLabel = NULL ;
}

CRYLSocialActionSlot::~CRYLSocialActionSlot() 
{
	GM_DELETE( m_pActionNameLabel ) ;
	GM_DELETE( m_itmActuonSlot ) ;
}

VOID		CRYLSocialActionSlot::MakeSkillSlot( char* lpszActionName ) 
{
	RECT			rt ;
	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;

	// Background Image
	SetWidth( 233 ) ;
	SetHeight( 42 ) ;
	CRYLSpriteList::Instance()->CreateGMImage( 233, 42, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;

	SetRect( &rt, 0, 126, 233, 168 ) ;

	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_SKILLSLOT_256x256 ) ) ;

	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;

	AttachPanelImage( ( CGMTexture*)pImage ) ;
	GM_DELETE( pImageDIB ) ;

	m_pActionNameLabel = new CRYLLabel( m_pGMUIParentDlg ) ;
	m_pActionNameLabel->SetLeft( 46 ) ;
	m_pActionNameLabel->SetTop( 53 ) ;
	m_pActionNameLabel->SetFontColor( 0xffffffff ) ;
	m_pActionNameLabel->SetAutoSize( TRUE ) ;
	m_pActionNameLabel->SetCaption( lpszActionName ) ;
	CGMUICustomPanel::AddChild( m_pActionNameLabel ) ;
}

//-----------------------------------------------------------------------------
// RYLSkillPanel
CRYLSocialActionPanel* g_pSocialActionPanel = NULL ;
CRYLSocialActionPanel::CRYLSocialActionPanel( CGMUICustomDlg *pParentDlg ) : CGMUICustomPanel( pParentDlg )
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
	
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;

	AttachPanelImage( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;

	SetWidth( 375 ) ;
	SetHeight( 440 ) ;

	m_pGMUIParentDlg = pParentDlg ;
	g_pSocialActionPanel = this ;

	m_dwSlotHeightMargin = 0L ;
	m_dwStartSlot        = 0L ;

	memset(m_wActionExTable, 0, sizeof(m_wActionExTable));
}

CRYLSocialActionPanel::~CRYLSocialActionPanel() 
{
	FinalCtrl() ;
}

VOID	CRYLSocialActionPanel::InitCtrl() 
{
	RECT rt ;
	CGMImageDib* pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT02_256x256 ) ;

	m_pPrevActionButton = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pPrevActionButton->SetLeft( 220 ) ;
	m_pPrevActionButton->SetTop( 420 ) ;
	m_pPrevActionButton->OnClickEvent = DlgClick ;
	m_pPrevActionButton->SetInstance( RYLSOCIALACTIONPANEL::PREVACTION_BUTTON ) ;
	SetRect( &rt, 217, 215, 230, 227 ) ;
	m_pPrevActionButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 217, 243, 230, 255 ) ;
	m_pPrevActionButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 217, 229, 230, 241 ) ;
	m_pPrevActionButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 217, 215, 230, 227 ) ;
	m_pPrevActionButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	CGMUICustomPanel::AddChild( m_pPrevActionButton ) ;

	m_pNextActionButton = new CRYLButton( m_pGMUIParentDlg ) ;
	m_pNextActionButton->SetLeft( 235 ) ;
	m_pNextActionButton->SetTop( 420 ) ;
	m_pNextActionButton->OnClickEvent = DlgClick ;
	m_pNextActionButton->SetInstance( RYLSOCIALACTIONPANEL::NEXTACTION_BUTTON ) ;
	SetRect( &rt, 232, 215, 245, 227 ) ;
	m_pNextActionButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	SetRect( &rt, 232, 243, 245, 255 ) ;
	m_pNextActionButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	SetRect( &rt, 232, 229, 245, 241 ) ;
	m_pNextActionButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	SetRect( &rt, 232, 215, 245, 227 ) ;
	m_pNextActionButton->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	CGMUICustomPanel::AddChild( m_pNextActionButton ) ;

	m_wActionTable[ 0 ][ 0 ] = 0x4001 ;	//	/laugh		웃음	긍정적
	m_wActionTable[ 0 ][ 1 ] = 0x4001 ;	//	/despair	좌절	부정적
	m_wActionTable[ 0 ][ 2 ] = 0x4001 ;	//	/shy		부끄럼	중의적
	m_wActionTable[ 0 ][ 3 ] = 0x4001 ;	//	/surprise	놀라기	중의적
	m_wActionTable[ 0 ][ 4 ] = 0x4001 ;	//	/roar		포효	중의적

	m_wActionTable[ 1 ][ 0 ] = 0x4002 ;	//	/wave		인사	긍정적
	m_wActionTable[ 1 ][ 1 ] = 0x4002 ;	//	/nod		끄덕이다	긍정적
	m_wActionTable[ 1 ][ 2 ] = 0x4002 ;	//	/no			거절	부정적
	m_wActionTable[ 1 ][ 3 ] = 0x4002 ;	//	/thank		감사	긍정적
	m_wActionTable[ 1 ][ 4 ] = 0x4002 ;	//	/salute		경례	중의적
											
	m_wActionTable[ 2 ][ 0 ] = 0x4003 ;	//	/cheer		환호	긍정적
	m_wActionTable[ 2 ][ 1 ] = 0x4003 ;	//	/sorry		사과	부정적
	m_wActionTable[ 2 ][ 2 ] = 0x4003 ;	//	/talk		대화	중의적
	m_wActionTable[ 2 ][ 3 ] = 0x4003 ;	//	/charge		돌격	중의적
	m_wActionTable[ 2 ][ 4 ] = 0x4003 ;	//	/comfort	위로	부정적
											
	m_wActionTable[ 3 ][ 0 ] = 0x0000 ;	//	
	m_wActionTable[ 3 ][ 1 ] = 0x0000 ;	//	
	m_wActionTable[ 3 ][ 2 ] = 0x0000 ;	//	
	m_wActionTable[ 3 ][ 3 ] = 0x0000 ;	//	
	m_wActionTable[ 3 ][ 4 ] = 0x0000 ;	//	

	InitSocialAction() ;
	ActionSlotAlign() ;
}

VOID	CRYLSocialActionPanel::FinalCtrl()
{
	GM_DELETE( m_pPrevActionButton ) ;
	GM_DELETE( m_pNextActionButton ) ;

	ClearSocialAction();
}

void CRYLSocialActionPanel::ClearSocialAction()
{
	std::vector< CRYLSocialActionSlot* >::iterator iter = m_vecActionSlot.begin() ;

	while( iter != m_vecActionSlot.end() )
	{
		CGMUICustomPanel::DeleteChild( ( *iter ) ) ;
		GM_DELETE( ( *iter ) ) ;

		++ iter ;
	}
	m_vecActionSlot.clear() ;

	m_dwStartSlot = 0;
}

HRESULT		CRYLSocialActionPanel::Update( BOOL &bClick, BOOL &bEdge ) 
{
	long lStartSkill;
	CRYLGameData* pGame = CRYLGameData::Instance() ;
	
	if ( m_dwStartSlot < m_vecActionSlot.size() )
		lStartSkill = 0 ;
	else
		lStartSkill = m_dwStartSlot - m_vecActionSlot.size() ;
	
	unsigned long dwSlot = 0 ;
	for(unsigned long i = m_dwStartSlot ; i < m_vecActionSlot.size() ; i ++ )
	{
		if ( dwSlot < MAX_SLOT_PER_PAGE )	
			dwSlot++ ;
		else				
			break;
	}

	lStartSkill -= dwSlot;

	if ( g_DeviceInput.GetIsLeftMouseDown() )
	{
		unsigned short wStart = m_dwStartSlot ;
		unsigned short wMaxSlot = m_vecActionSlot.size() ;
		unsigned short wCurrPageMaxSlot = wMaxSlot - wStart ;

		if ( wCurrPageMaxSlot > MAX_SLOT_PER_PAGE )
			wCurrPageMaxSlot = wStart + MAX_SLOT_PER_PAGE ;
		else
			wCurrPageMaxSlot += wStart ;

		for ( unsigned short i = wStart ; i < wCurrPageMaxSlot ; i ++ )
		{
			CItemInstance* lpSelectAction = m_vecActionSlot[ i ]->m_itmActuonSlot ;
			
			if ( NULL != lpSelectAction && lpSelectAction->IsOverMouse() && NULL == pGame->m_lpPickItem )
			{
				const Skill::ProtoType *lpProtoType = 
					g_SkillMgr.g_SkillMgr.GetSkillProtoType( lpSelectAction->m_lpItemBase->GetPrototypeID() ) ;

				if ( NULL != lpProtoType )
				{
					pGame->m_lpPickItem = new CItemInstance;
					pGame->m_lpPickItem->SetSkillInfo( lpSelectAction );
					pGame->SetPickItem( pGame->m_lpPickItem ) ;

					CRYLGameScene* pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
					pScene->GetAbilityTooltipDlg()->SetVisible( FALSE );

					return S_OK ;
				}
			}
		}
	}

	return S_OK ;
}

HRESULT		CRYLSocialActionPanel::Render( LPDIRECT3DDEVICE8 lpD3DDevice )  
{
	return S_OK ;
}

HRESULT		CRYLSocialActionPanel::FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) 
{
	unsigned long dwPrintSlotNum = 0;
	unsigned long dwActionNum = m_vecActionSlot.size();

	for (unsigned long dwActionIndex = m_dwStartSlot; dwActionIndex < dwActionNum && dwPrintSlotNum < MAX_SLOT_PER_PAGE; ++dwActionIndex )
	{
		CItemInstance* lpSlot = m_vecActionSlot[ dwActionIndex ]->m_itmActuonSlot ;
		if ( lpSlot )
		{
			lpSlot->m_wPutX = m_pGMUIParentDlg->GetLeft() + 21 ;
			lpSlot->m_wPutY = m_pGMUIParentDlg->GetTop() + 79 + dwPrintSlotNum * 41 ;
			lpSlot->Render( lpD3DDevice ) ;
		}

		++dwPrintSlotNum;
	}

	return S_OK ;
}

VOID		CRYLSocialActionPanel::DlgClick( CGMUIObject* Sender, INT X, INT Y ) 
{
	if ( !g_pSocialActionPanel )
		return ;

	switch( Sender->GetInstance())
	{
	case RYLSOCIALACTIONPANEL::NEXTACTION_BUTTON :
		g_pSocialActionPanel->NextActionSlotPage() ;
		break ;
	
	case RYLSOCIALACTIONPANEL::PREVACTION_BUTTON :
		g_pSocialActionPanel->PrevActionSlotPage() ;
		break ;
	}
}

void	CRYLSocialActionPanel::InitSocialActionAbilityEx(int nAbilityValue)
{
	memset(m_wActionExTable, 0, sizeof(m_wActionExTable));
	
	CRYLGameData* pGame = CRYLGameData::Instance() ;

	if(nAbilityValue == 0)
		return;

	unsigned short AbilitySocial[MAX_ACTIONEX] = { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 };

	for(int i = 1; i <= nAbilityValue; ++i)
	{
		m_wActionExTable[ i-1 ][ 0 ] = AbilitySocial[i-1] ;	//	/laugh		웃음	긍정적
		m_wActionExTable[ i-1 ][ 1 ] = AbilitySocial[i-1] ;	//	/despair	좌절	부정적
		m_wActionExTable[ i-1 ][ 2 ] = AbilitySocial[i-1] ;	//	/shy		부끄럼	중의적
		m_wActionExTable[ i-1 ][ 3 ] = AbilitySocial[i-1] ;	//	/surprise	놀라기	중의적
		m_wActionExTable[ i-1 ][ 4 ] = AbilitySocial[i-1] ;	//	/roar		포효	중의적
	}

	CRYLSocialActionSlot* pActionSlot = NULL ;
	int	nCount = m_vecActionSlot.size() ;

	for ( int nAction = 0 ; nAction < MAX_ACTIONEX ; nAction ++ )
	{
		for( int nType = 0 ; nType < ACTION_TYPE ; nType ++ )
		{
			if ( m_wActionExTable[ nAction ][ nType ] != 0x0000 )
			{
				pActionSlot = new CRYLSocialActionSlot( m_pGMUIParentDlg ) ;
				
				pActionSlot->m_itmActuonSlot = new CItemInstance;

				pGame->m_csStatus.m_lstSocialSlot.push_back(pActionSlot->m_itmActuonSlot);

				if (FALSE == pActionSlot->m_itmActuonSlot->SetSkillInfo(
					m_wActionExTable[ nAction ][ nType ], nType, nType, TakeType::TS_SSLOT, nCount, 0))
				{
					CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
					lpMessageBox->Create(CRYLStringTable::m_strString[ 3871 ]);
					PostMessage(g_ClientMain.m_hWnd, WM_DESTROY, 0, 0);
					CRYLGameData::Instance()->m_dwClientMode = CLIENT_END;
					return;
				}

				pActionSlot->SetLeft( 11 ) ;
				pActionSlot->SetTop( m_dwSlotHeightMargin + 34 ) ;
				
				CHAR	strString[ 32 ] ;
				strcpy( strString, pActionSlot->m_itmActuonSlot->GetItemInfo().m_SpriteData.m_szName ) ;
				if (strlen(strString) > 21)
				{
					strString[21] = '.';
					strString[22] = '.';
					strString[23] = '.';
					strString[24] = '\0';
				}
				pActionSlot->MakeSkillSlot( strString ) ;
				CGMUICustomPanel::AddChild( pActionSlot ) ;

				m_vecActionSlot.push_back( pActionSlot ) ;

				m_dwSlotHeightMargin += 41 ;

				++ nCount ;
			}
		}
	}

}

void	CRYLSocialActionPanel::InitSocialAction() 
{
	CRYLGameData* pGame = CRYLGameData::Instance() ;
	pGame->m_csStatus.m_lstSocialSlot.clear();

	CRYLSocialActionSlot* pActionSlot = NULL ;
	int	nCount = 0 ;

	for ( int nAction = 0 ; nAction < MAX_ACTION ; nAction ++ )
	{
		for( int nType = 0 ; nType < ACTION_TYPE ; nType ++ )
		{
			if ( m_wActionTable[ nAction ][ nType ] != 0x0000 )
			{
				pActionSlot = new CRYLSocialActionSlot( m_pGMUIParentDlg ) ;
				
				pActionSlot->m_itmActuonSlot = new CItemInstance;

				pGame->m_csStatus.m_lstSocialSlot.push_back(pActionSlot->m_itmActuonSlot);

				if (FALSE == pActionSlot->m_itmActuonSlot->SetSkillInfo(
					m_wActionTable[ nAction ][ nType ], nType, nType, TakeType::TS_SSLOT, nCount, 0))
				{
					CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
					lpMessageBox->Create(CRYLStringTable::m_strString[ 3871 ]);
					PostMessage(g_ClientMain.m_hWnd, WM_DESTROY, 0, 0);
					CRYLGameData::Instance()->m_dwClientMode = CLIENT_END;
					return;
				}

				pActionSlot->SetLeft( 11 ) ;
				pActionSlot->SetTop( m_dwSlotHeightMargin + 34 ) ;
				
				CHAR	strString[ 32 ] ;
				strcpy( strString, pActionSlot->m_itmActuonSlot->GetItemInfo().m_SpriteData.m_szName ) ;
				if (strlen(strString) > 21)
				{
					strString[21] = '.';
					strString[22] = '.';
					strString[23] = '.';
					strString[24] = '\0';
				}
				pActionSlot->MakeSkillSlot( strString ) ;
				CGMUICustomPanel::AddChild( pActionSlot ) ;

				m_vecActionSlot.push_back( pActionSlot ) ;

				m_dwSlotHeightMargin += 41 ;

				++ nCount ;
			}
		}
	}
}

void	CRYLSocialActionPanel::PrevActionSlotPage()
{
	if ( m_dwStartSlot != 0 )
	{
		m_dwStartSlot -= MAX_SLOT_PER_PAGE ;
		ActionSlotAlign() ;
	}
}

void	CRYLSocialActionPanel::NextActionSlotPage()
{	
	if ( m_vecActionSlot.size() - m_dwStartSlot > MAX_SLOT_PER_PAGE )
	{
		m_dwStartSlot += MAX_SLOT_PER_PAGE ;
		ActionSlotAlign() ;
	}
}

void		CRYLSocialActionPanel::UpdateActionSlotPosition()
{
	if ( m_vecActionSlot.empty() ) 
		return ;

	INT iTempY = 0, iTempIndex = 0 ;
	INT	iCount = 0 ;
	CRYLSocialActionSlot* lpSlot = NULL ;

	std::vector< CRYLSocialActionSlot* >::iterator it = m_vecActionSlot.begin() ;

	for ( ; it != m_vecActionSlot.end() ; ++ it )
	{
		if ( ( iCount % MAX_SLOT_PER_PAGE ) == 0 )
		{
			iTempY = 0 ;
		}

		lpSlot = ( *it ) ;
		lpSlot->SetTop( iTempY + 34 ) ;
		iTempY += 41 ;
		++ iCount ;
	}
}

void	CRYLSocialActionPanel::ActionSlotAlign()
{
	CRYLSocialActionSlot* pSlot = NULL ;
	UpdateActionSlotPosition() ;

	for ( size_t i = 0; i < m_vecActionSlot.size(); ++i)
	{
		pSlot = m_vecActionSlot[ i ] ;

		if ( m_dwStartSlot <= i && i < m_dwStartSlot + MAX_SLOT_PER_PAGE )
			pSlot->SetVisible( TRUE ) ;
		else
			pSlot->SetVisible( FALSE ) ;
	}
}
