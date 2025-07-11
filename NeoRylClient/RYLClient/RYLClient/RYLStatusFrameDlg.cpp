//--------------------------------------------------------------------------------------------------------------------------
// File Name: RYLStatusFrameDlg.cpp
//
// Programmer: Zergra( Park Jongtae ) in GamaSoft corp.
//
// File Desciption: 새로운 스테이터스 윈도우
//
// Date: 2004. 8. 24. (Tue)
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
// Headers
//--------------------------------------------------------------------------------------------------------------------------
#include <winsock2.h>
#include "RYLClientMain.h"
#include "RYLStatusAndQuestFrameDlg.h"
#include "BaseDataDefine.h"
#include "RYLTabButton.h"
#include "RYLImageManager.h"
#include "RYLImage.h"

#include "RYLStringTable.h"
#include "RYLSpriteList.h"
#include "GMImageDib.h"
#include "WinInput.h"

#include <RenderOption.h>

#include "RYLCharacterInfoPanel.h"
#include "RYLStatusInfoPanel.h"

#include "RYLStatusFrameDlg.h"
#include "GMMemory.h"

//--------------------------------------------------------------------------------------------------------------------------
// global variables
//--------------------------------------------------------------------------------------------------------------------------
CRYLStatusFrameDlg* g_pStatusFrameDlg = NULL;

//--------------------------------------------------------------------------------------------------------------------------
// definitions
//--------------------------------------------------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
// class CRYLStatusFrameDlg
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
//--------------------------------------------------------------------------------------------------------------------------
// constructor, destructor
//--------------------------------------------------------------------------------------------------------------------------
CRYLStatusFrameDlg::CRYLStatusFrameDlg()
{
    m_pTabButton = NULL;

	RECT rt;
	CGMImageDib*	pImageDIB = NULL;
	CRYLImage*		pImage	  = NULL;

	CRYLSpriteList::Instance()->CreateGMImage( 256, 15, &pImageDIB );
	pImageDIB->ClearbyColor( 0x00000000 );
	SetRect( &rt, 0, 0, 256, 15 );
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) );
	
	pImage = CRYLImageManager::Instance()->CreateRYLImage();
	pImage->LoadFromImage( pImageDIB );
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage );
	AttachCaptionTexture( ( CGMTexture*)pImage );

	GM_DELETE( pImageDIB );

	CRYLSpriteList::Instance()->CreateGMImage( 256, 410, &pImageDIB );
	pImageDIB->ClearbyColor( 0x00000000 );
	SetRect( &rt, 0, 15, 256, 37 );
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) );
	
    pImageDIB->DrawFrameRect( 110, 0, 143, 21, 0xc0000000 ) ;
	pImageDIB->DrawFrameRect( 110, 20, 143, 1, 0xffa88a60 ) ;
	
	pImage = CRYLImageManager::Instance()->CreateRYLImage();
	pImage->LoadFromImage( pImageDIB );
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage );
	
	AttachWindowTexture( ( CGMTexture*)pImage );

	GM_DELETE( pImageDIB );

	SetWidth( 256 );
	SetHeight( 375 );

	SetTop( 135 ) ;
	SetLeft( 0 ) ;  

	SetFormType( RYLDLG_FRAME_TYPE_CLOSEBUTTON );

	InitCtrl();

	g_pStatusFrameDlg = this;
}

CRYLStatusFrameDlg::~CRYLStatusFrameDlg()
{
	FinalCtrl() ;
}

//--------------------------------------------------------------------------------------------------------------------------
// member functions
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
// CRYLStatusFrameDlg::InitCtrl()
VOID CRYLStatusFrameDlg::InitCtrl()
{
    m_pTabButton = new CRYLTabButton( this ) ;
	m_pTabButton->SetLeft( 1 ) ;
	m_pTabButton->SetTop( 15 ) ;
	m_pTabButton->SetWidth( 252 ) ;
	m_pTabButton->SetHeight( 31 ) ;
	m_pTabButton->SetInstance( RYLSTATUSFRAMEDLG::TAB_BUTTON ) ;
	m_pTabButton->OnClickEvent = DlgClick ;
	m_pTabButton->AttachTabButton( CRYLStringTable::m_strString[ 2788 ], 5 ) ;
	m_pTabButton->AttachTabButton( CRYLStringTable::m_strString[ 2789 ], 5 ) ;
	AttachGMUIControl( m_pTabButton ) ;

	LPTABBUTTONINFO		lpTabButton = NULL ;
	lpTabButton = m_pTabButton->GetTabButtonInfo( 0 ) ;
	lpTabButton->m_pTabButton->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 1789 ], 1789, TOOLTIP_TAB ) ;
	lpTabButton = m_pTabButton->GetTabButtonInfo( 1 ) ;
	lpTabButton->m_pTabButton->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 1790 ], 1790, TOOLTIP_TAB ) ;

	m_pCharacterInfoPanel = new CRYLCharacterInfoPanel( this ) ;
	m_pCharacterInfoPanel->SetLeft( 0 ) ;
	m_pCharacterInfoPanel->SetTop( 36 ) ;
	m_pCharacterInfoPanel->SetWidth( 255 ) ;
	m_pCharacterInfoPanel->SetHeight( 395 ) ;
	( ( CRYLCharacterInfoPanel* )m_pCharacterInfoPanel )->InitCtrl() ;
	m_pCharacterInfoPanel->SetVisible( TRUE ) ;
	AttachGMUIControl( m_pCharacterInfoPanel ) ;

	m_pStatusInfoPanel = new CRYLStatusInfoPanel( this ) ;
	m_pStatusInfoPanel->SetLeft( 0 ) ;
	m_pStatusInfoPanel->SetTop( 36 ) ;
	m_pStatusInfoPanel->SetWidth( 255 ) ;
	m_pStatusInfoPanel->SetHeight( 395 ) ;
	( ( CRYLStatusInfoPanel* )m_pStatusInfoPanel )->InitCtrl() ;
	m_pStatusInfoPanel->SetVisible( FALSE ) ;
	AttachGMUIControl( m_pStatusInfoPanel ) ;
}

//--------------------------------------------------------------------------------------------------------------------------
// CRYLStatusFrameDlg::FinalCtrl()
VOID CRYLStatusFrameDlg::FinalCtrl()
{
    GM_DELETE( m_pTabButton );
    GM_DELETE( m_pCharacterInfoPanel );
    GM_DELETE( m_pStatusInfoPanel );
}

//--------------------------------------------------------------------------------------------------------------------------
// CRYLStatusFrameDlg::Show()
VOID CRYLStatusFrameDlg::Show( CGMUIObject* Sender )
{
}

//--------------------------------------------------------------------------------------------------------------------------
// CRYLStatusFrameDlg::Hide()
VOID CRYLStatusFrameDlg::Hide( CGMUIObject* Sender )
{
}

//--------------------------------------------------------------------------------------------------------------------------
// CRYLStatusFrameDlg::GMUIPaint()
VOID CRYLStatusFrameDlg::GMUIPaint( INT xx, INT yy )
{
    CRYLDialog::GMUIPaint( xx, yy ) ;
}

//--------------------------------------------------------------------------------------------------------------------------
// CRYLStatusFrameDlg::Update()
HRESULT CRYLStatusFrameDlg::Update( BOOL &bClick, BOOL &bEdge )
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

    if( m_pCharacterInfoPanel->GetVisible() )
    {
        static_cast<CRYLCharacterInfoPanel*>( m_pCharacterInfoPanel )->Update();
    }

    if( m_pStatusInfoPanel->GetVisible() )
    {
        static_cast<CRYLStatusInfoPanel*>( m_pStatusInfoPanel )->Update();
    }

    return S_OK;
}

//--------------------------------------------------------------------------------------------------------------------------
// CRYLStatusFrameDlg::DlgClick()
VOID CRYLStatusFrameDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y )
{
    if( !g_pStatusFrameDlg )
        return;

    switch( Sender->GetInstance() )
	{
	case RYLSTATUSFRAMEDLG::TAB_BUTTON:
        {
            INT		iFocus = g_pStatusFrameDlg->GetTabButton()->GetFocusTabButton();

			switch( iFocus )
			{
			case 0: 
				{
					g_pStatusFrameDlg->ShowCharacterInfoPanel();
				}
				break;

			case 1: 
				{
					g_pStatusFrameDlg->ShowStatusInfoPanel();
				}
				break;
			}
        }
        break;
    }
}

//--------------------------------------------------------------------------------------------------------------------------
// CRYLStatusFrameDlg::ShowCharacterInfoPanel()
VOID CRYLStatusFrameDlg::ShowCharacterInfoPanel()
{
    static_cast<CRYLCharacterInfoPanel*>( m_pCharacterInfoPanel )->SetVisible( TRUE );
    static_cast<CRYLStatusInfoPanel*>( m_pStatusInfoPanel )->SetVisible( FALSE );

    GetTabButton()->SetFocusTabButton( 0 ) ;
}

//--------------------------------------------------------------------------------------------------------------------------
// CRYLStatusFrameDlg::ShowStatusInfoPanel()
VOID CRYLStatusFrameDlg::ShowStatusInfoPanel()
{
    static_cast<CRYLCharacterInfoPanel*>( m_pCharacterInfoPanel )->SetVisible( FALSE );
    static_cast<CRYLStatusInfoPanel*>( m_pStatusInfoPanel )->SetVisible( TRUE );

    GetTabButton()->SetFocusTabButton( 1 ) ;
}