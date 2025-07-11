#include <winsock2.h>
#include "RYLClientMain.h"
#include "RYLStatusAndQuestFrameDlg.h"
#include "BaseDataDefine.h"
#include "RYLTabButton.h"
#include "RYLImageManager.h"
#include "RYLImage.h"

#include "RYLSpriteList.h"
#include "GMImageDib.h"
#include "WinInput.h"

#include "RYLStatusPanel.h"
#include "RYLExecutionQuestPanel.h"
#include "RYLCompleteQuestPanel.h"
#include <RenderOption.h>

#include "RYLStringTable.h"
#include "GMMemory.h"

CRYLStatusAndQuestFrameDlg* g_pStatusAndQuestFramdDlg = NULL ;

CRYLStatusAndQuestFrameDlg::CRYLStatusAndQuestFrameDlg() 
{
	m_pTabButton = NULL ;

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

	CRYLSpriteList::Instance()->CreateGMImage( 256, 397, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 15, 256, 37 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	
	pImageDIB->DrawFrameRect( 236, 0, 17, 21, 0xc0000000 ) ;
	pImageDIB->DrawFrameRect( 236, 20, 17, 1, 0xffa88a60 ) ;
	
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	AttachWindowTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;

	SetWidth( 256 ) ;
	SetHeight( 375 ) ;

	SetTop( 135 ) ;
	SetLeft( 0 ) ;  

	SetFormType( RYLDLG_FRAME_TYPE_CLOSEBUTTON ) ;

	InitCtrl() ;
	g_pStatusAndQuestFramdDlg = this ;
}

CRYLStatusAndQuestFrameDlg::~CRYLStatusAndQuestFrameDlg() 
{
	FinalCtrl() ;
}

VOID	CRYLStatusAndQuestFrameDlg::InitCtrl() 
{
	m_pTabButton = new CRYLTabButton( this ) ;
	m_pTabButton->SetLeft( 1 ) ;
	m_pTabButton->SetTop( 15 ) ;
	m_pTabButton->SetWidth( 252 ) ;
	m_pTabButton->SetHeight( 31 ) ;
	m_pTabButton->SetInstance( RYLSTATUSANDQUESTODLG::TAB_BUTTON ) ;
	m_pTabButton->OnClickEvent = DlgClick ;
	m_pTabButton->AttachTabButton( CRYLStringTable::m_strString[ 1922 ], 5 ) ;
	m_pTabButton->AttachTabButton( CRYLStringTable::m_strString[ 1923 ], 5 ) ;
	m_pTabButton->AttachTabButton( CRYLStringTable::m_strString[ 1924 ], 5 ) ;
	AttachGMUIControl( m_pTabButton ) ;

	LPTABBUTTONINFO		lpTabButton = NULL ;
	lpTabButton = m_pTabButton->GetTabButtonInfo( 0 ) ;
	lpTabButton->m_pTabButton->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 1789 ], 1789, TOOLTIP_TAB ) ;
	lpTabButton = m_pTabButton->GetTabButtonInfo( 1 ) ;
	lpTabButton->m_pTabButton->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 1790 ], 1790, TOOLTIP_TAB ) ;
	lpTabButton = m_pTabButton->GetTabButtonInfo( 2 ) ;
	lpTabButton->m_pTabButton->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 1791 ], 1791, TOOLTIP_TAB ) ;

	m_pStatusPanel = new CRYLStatusPanel( this ) ;
	m_pStatusPanel->SetLeft( 0 ) ;
	m_pStatusPanel->SetTop( 36 ) ;
	m_pStatusPanel->SetWidth( 255 ) ;
	m_pStatusPanel->SetHeight( 395 ) ;
	( ( CRYLStatusPanel* )m_pStatusPanel )->InitCtrl() ;
	m_pStatusPanel->SetVisible( TRUE ) ;
	AttachGMUIControl( m_pStatusPanel ) ;

	m_pExecutionQuestPanel = new CRYLExecutionQuestPanel( this ) ;
	m_pExecutionQuestPanel->SetLeft( 0 ) ;
	m_pExecutionQuestPanel->SetTop( 36 ) ;
	m_pExecutionQuestPanel->SetWidth( 255 ) ;
	m_pExecutionQuestPanel->SetHeight( 395 ) ;
	( ( CRYLExecutionQuestPanel* )m_pExecutionQuestPanel )->InitCtrl() ;
	m_pExecutionQuestPanel->SetVisible( FALSE ) ;
	AttachGMUIControl( m_pExecutionQuestPanel ) ;

	m_pCompleteQuestPanel = new CRYLCompleteQuestPanel( this ) ;
	m_pCompleteQuestPanel->SetLeft( 0 ) ;
	m_pCompleteQuestPanel->SetTop( 36 ) ;
	m_pCompleteQuestPanel->SetWidth( 255 ) ;
	m_pCompleteQuestPanel->SetHeight( 395 ) ;
	( ( CRYLCompleteQuestPanel* )m_pCompleteQuestPanel )->InitCtrl() ;
	m_pCompleteQuestPanel->SetVisible( FALSE ) ;
	AttachGMUIControl( m_pCompleteQuestPanel ) ;
}

HRESULT		CRYLStatusAndQuestFrameDlg::Update( BOOL &bClick, BOOL &bEdge ) 
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

	if ( m_pStatusPanel && m_pStatusPanel->GetVisible() )
		( ( CRYLStatusPanel* )m_pStatusPanel )->Update() ;

	if ( m_pExecutionQuestPanel && m_pExecutionQuestPanel->GetVisible() )
		( ( CRYLExecutionQuestPanel* )m_pExecutionQuestPanel )->Update( bClick, bEdge ) ;

	if ( m_pCompleteQuestPanel && m_pCompleteQuestPanel->GetVisible() )
		( ( CRYLCompleteQuestPanel* )m_pCompleteQuestPanel )->Update( bClick, bEdge ) ;

	return S_OK ;
}

VOID	CRYLStatusAndQuestFrameDlg::FinalCtrl() 
{
	GM_DELETE( m_pTabButton ) ;
	GM_DELETE( m_pStatusPanel ) ;
	GM_DELETE( m_pExecutionQuestPanel ) ;
	GM_DELETE( m_pCompleteQuestPanel ) ;
}

VOID	CRYLStatusAndQuestFrameDlg::Show( CGMUIObject* Sender ) 
{
}

VOID	CRYLStatusAndQuestFrameDlg::Hide( CGMUIObject* Sender ) 
{
}

VOID	CRYLStatusAndQuestFrameDlg::GMUIPaint( INT xx, INT yy ) 
{
	CRYLDialog::GMUIPaint( xx, yy ) ;
}

VOID	CRYLStatusAndQuestFrameDlg::ShowStatusPanel() 
{
	if(GetStatusPanel())
		( ( CRYLStatusPanel*)GetStatusPanel() )->SetVisible( TRUE ) ;

	if(GetExecutionQuestPanel())
		( ( CRYLExecutionQuestPanel*)GetExecutionQuestPanel() )->SetVisible( FALSE ) ;

	if(GetCompleteQuestPanel())
		( ( CRYLCompleteQuestPanel*)GetCompleteQuestPanel() )->SetVisible( FALSE ) ;

	if(GetTabButton())
		GetTabButton()->SetFocusTabButton( 0 ) ;
}

VOID	CRYLStatusAndQuestFrameDlg::ShowExcutionQuestPanel()
{
	if(GetStatusPanel())
		( ( CRYLStatusPanel*)GetStatusPanel() )->SetVisible( FALSE ) ;

	if(GetExecutionQuestPanel())
		( ( CRYLExecutionQuestPanel*)GetExecutionQuestPanel() )->SetVisible( TRUE ) ;

	if(GetCompleteQuestPanel())
		( ( CRYLCompleteQuestPanel*)GetCompleteQuestPanel() )->SetVisible( FALSE ) ;

	if(GetExecutionQuestPanel())
	{
		( ( CRYLExecutionQuestPanel*)GetExecutionQuestPanel() )->InitValue() ;
		( ( CRYLExecutionQuestPanel*)GetExecutionQuestPanel() )->InitDesc() ;
	}

	if(GetTabButton())
		GetTabButton()->SetFocusTabButton( 1 ) ;
}

VOID	CRYLStatusAndQuestFrameDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y )
{
	if ( !g_pStatusAndQuestFramdDlg || !Sender)
		return ;


	switch( Sender->GetInstance() )
	{
	case RYLSTATUSANDQUESTODLG::TAB_BUTTON :
		{
			INT		iFocus = g_pStatusAndQuestFramdDlg->GetTabButton()->GetFocusTabButton() ;
			switch( iFocus )
			{
			case 0 : 
				{
					g_pStatusAndQuestFramdDlg->ShowStatusPanel() ;
				}
				break ;
			case 1 : 
				{
					g_pStatusAndQuestFramdDlg->ShowExcutionQuestPanel() ;
				}
				break ;
			case 2 :
				{
					if(g_pStatusAndQuestFramdDlg->GetStatusPanel())
						( ( CRYLStatusPanel*)g_pStatusAndQuestFramdDlg->GetStatusPanel() )->SetVisible( FALSE ) ;

					if(g_pStatusAndQuestFramdDlg->GetExecutionQuestPanel())
						( ( CRYLExecutionQuestPanel*)g_pStatusAndQuestFramdDlg->GetExecutionQuestPanel() )->SetVisible( FALSE ) ;

					if(g_pStatusAndQuestFramdDlg->GetCompleteQuestPanel())
						( ( CRYLCompleteQuestPanel*)g_pStatusAndQuestFramdDlg->GetCompleteQuestPanel() )->SetVisible( TRUE ) ;
				}
				break ;
			}
		}
		break ;
	}
}



