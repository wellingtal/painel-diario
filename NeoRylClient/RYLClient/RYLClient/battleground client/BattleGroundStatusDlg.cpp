//-----------------------------------------------------------------------------------------------------
// File Name: BattleGroundStatusDlg.cpp
//
// Programmer: Zergra( Park Jongtae ) in GamaSoft corp.
//
// Date: 2004. 2. 3. Tuesday
//-----------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------
// Headers
#include "RYLClientMain.h"

#include "GMImageDib.h"
#include "RYLImage.h"
#include "RYLSpriteList.h"
#include "RYLImageManager.h"

#include "RYLGameData.h"
#include "RYLNetworkData.h"
#include "RYLStringTable.h"
        
#include <WinInput.h>
#include <Network/ClientSocket/Send/SendBGServer.h>

#include "RYLButton.h"
#include "RYLLabel.h"
#include "RYLStringGrid.h"

#include "BattleGroundStatusDlg.h"
#include "RYLChattingDlg.h"
#include "GMMemory.h"


///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
// class CRYLBattleGroundStatusDlg
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
CRYLBattleGroundStatusDlg* g_BattleGroundStatusDlg = NULL;
//-----------------------------------------------------------------------------------------------------
// Constructor, Destructor
//-----------------------------------------------------------------------------------------------------
CRYLBattleGroundStatusDlg::CRYLBattleGroundStatusDlg()
{
    m_pSortFormLabel        = NULL;
    m_pSortFormValue        = NULL;

    m_pChannelListStringGrid= NULL;

    m_pGameTypeLabel        = NULL;
    m_pGameTypeValue        = NULL;

    m_pTimeInfoLabel        = NULL;
    m_pTimeInfoValue        = NULL;

    m_pMaxPeopleLabel       = NULL;
    m_pMaxPeopleValue       = NULL;

    m_pTargetLabel          = NULL;
    m_pTargetValue          = NULL;

    m_pCurrentPeopleLabel   = NULL;
    m_pCurrentPeopleValue   = NULL;

    m_pScoreLabel           = NULL;
    m_pScoreValue           = NULL;

    m_pJoinPlayerButton     = NULL;
    m_pJoinSpectatorButton  = NULL;

    //--  initialize dialog image and controls --//
    RECT			rt;
	CGMImageDib*	pImageDIB = NULL;
	CRYLImage*		pImage	  = NULL;

    // Ä¸¼Ç¹Ù
    CRYLSpriteList::Instance()->CreateGMImage( 290, 38, &pImageDIB );
	pImageDIB->ClearbyColor( 0x00000000 );
	SetRect( &rt, 0, 0, 256, 38 );
    pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BATTLEGROUND_STATUS_256x256 ) );
    SetRect( &rt, 0, 38, 34, 76 );
	pImageDIB->DrawRect( &rt, 256, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BATTLEGROUND_STATUS_256x256 ) );
	
	pImage = CRYLImageManager::Instance()->CreateRYLImage();
	pImage->LoadFromImage( pImageDIB );
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage );
	AttachCaptionTexture( ( CGMTexture*)pImage );

	GM_DELETE( pImageDIB );

    // ¸öÅë
    CRYLSpriteList::Instance()->CreateGMImage( 290, 410, &pImageDIB );
	pImageDIB->ClearbyColor( 0x00000000 );	

    for( int i = 0; i < 80; ++i )
    {
        SetRect( &rt, 0, 76, 256, 81 );
        pImageDIB->DrawRect( &rt, 0, i * 5, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BATTLEGROUND_STATUS_256x256 ) );
        SetRect( &rt, 34, 38, 68, 43 );
	    pImageDIB->DrawRect( &rt, 256, i * 5, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BATTLEGROUND_STATUS_256x256 ) );
    }
    
    // ¹Ù´Ú
	SetRect( &rt, 0, 81, 256, 91 );
    pImageDIB->DrawRect( &rt, 0, 400, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BATTLEGROUND_STATUS_256x256 ) );
    SetRect( &rt, 68, 38, 102, 48 );
	pImageDIB->DrawRect( &rt, 256, 400, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BATTLEGROUND_STATUS_256x256 ) );
	
	// frame rect
    pImageDIB->MakeFrameRect(  7, 26, 275,191, 0xffB99E6B, 0x80000000, 1 );    // channel list frame
    pImageDIB->MakeFrameRect(  7,240, 275,136, 0xffB99E6B, 0x80000000, 1 );    // channel info frame
    pImageDIB->MakeFrameRect( 12, 49, 265,  1, 0xff6e6857, 0x80000000, 1 );    // ±¸ºÐ¼±

    pImageDIB->MakeFrameRect( 92, 247, 92, 18, 0xff6e6857, 0x80000000, 1 );    // channel info frame ¾È : game type frame
    pImageDIB->MakeFrameRect( 92, 273, 38, 18, 0xff6e6857, 0x80000000, 1 );    // channel info frame ¾È : next match time frame
    pImageDIB->MakeFrameRect(205, 273, 38, 18, 0xff6e6857, 0x80000000, 1 );    // channel info frame ¾È : max number of people frame
    pImageDIB->MakeFrameRect( 92, 299,140, 18, 0xff6e6857, 0x80000000, 1 );    // channel info frame ¾È : target frame
    pImageDIB->MakeFrameRect( 92, 325,140, 18, 0xff6e6857, 0x80000000, 1 );    // channel info frame ¾È : current number of people frame
    pImageDIB->MakeFrameRect( 92, 351,140, 18, 0xff6e6857, 0x80000000, 1 );    // channel info frame ¾È : current score frame

    pImage = CRYLImageManager::Instance()->CreateRYLImage();
	pImage->LoadFromImage( pImageDIB );
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage );
	AttachWindowTexture( ( CGMTexture*)pImage );

	GM_DELETE( pImageDIB );

    InitCtrl();

    // initialize dialog first position
    SetWidth( 290 );
	SetHeight( 443 );
    SetLeft( ( CGMUIScreen::GetInstance()->GetWidth() - GetWidth() ) / 2 - 50 );
    SetTop( ( CGMUIScreen::GetInstance()->GetHeight() - GetHeight() ) / 2 - 50 );

    SetFormType( RYLDLG_FRAME_TYPE_CLOSEBUTTON );

    // set global pointer
    g_BattleGroundStatusDlg = this;

    //-- initialize member variables --//
    m_dwJoinType        = 0;
    m_dwJoinChNumber    = 0;
    m_ltChannelInfo.clear();
    m_bIsPlayer         = TRUE;
}

CRYLBattleGroundStatusDlg::~CRYLBattleGroundStatusDlg()
{
    ClearChannelInfo();
    FinalCtrl();
}

//-----------------------------------------------------------------------------------------------------
// Member Functions
//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------
// CRYLBattleGroundStatusDlg::InitCtrl()
VOID CRYLBattleGroundStatusDlg::InitCtrl()
{
    m_pCHANNEL = new CRYLLabel( this );
	m_pCHANNEL->SetLeft( 18 );
	m_pCHANNEL->SetTop( 71 );
	m_pCHANNEL->SetFontColor( 0xffffffff );
	m_pCHANNEL->SetAutoSize( TRUE );
	m_pCHANNEL->SetCaption( CRYLStringTable::m_strString[ 2315 ] );
	AttachGMUIControl( m_pCHANNEL );

    m_pGAMETYPE = new CRYLLabel( this );
	m_pGAMETYPE->SetLeft( 71 );
	m_pGAMETYPE->SetTop( 71 );
	m_pGAMETYPE->SetFontColor( 0xffffffff );
	m_pGAMETYPE->SetAutoSize( TRUE );
	m_pGAMETYPE->SetCaption( CRYLStringTable::m_strString[ 2316 ] );
	AttachGMUIControl( m_pGAMETYPE );

    m_pMAXPEOPLE = new CRYLLabel( this );
	m_pMAXPEOPLE->SetLeft( 160 );
	m_pMAXPEOPLE->SetTop( 71 );
	m_pMAXPEOPLE->SetFontColor( 0xffffffff );
	m_pMAXPEOPLE->SetAutoSize( TRUE );
	m_pMAXPEOPLE->SetCaption( CRYLStringTable::m_strString[ 2317 ] );
	AttachGMUIControl( m_pMAXPEOPLE );

    m_pREMAINTIME = new CRYLLabel( this );
	m_pREMAINTIME->SetLeft( 224 );
	m_pREMAINTIME->SetTop( 71 );
	m_pREMAINTIME->SetFontColor( 0xffffffff );
	m_pREMAINTIME->SetAutoSize( TRUE );
	m_pREMAINTIME->SetCaption( CRYLStringTable::m_strString[ 2318 ] );
	AttachGMUIControl( m_pREMAINTIME );

    m_pChannelListStringGrid = new CRYLStringGrid( this, FALSE );
    m_pChannelListStringGrid->SetLeft( 18 );
    m_pChannelListStringGrid->SetTop( 93 );
    m_pChannelListStringGrid->SetWidth( 250 );
    m_pChannelListStringGrid->SetHeight( 155 );
    m_pChannelListStringGrid->SetColCount( 10 );
    m_pChannelListStringGrid->SetItemHeight( 16 );
    m_pChannelListStringGrid->SetFontColor( 0xFFFFFFFF );
    m_pChannelListStringGrid->SetInstance( RYL_BATTLEGROUND_STATUS_DLG::CHANNEL_STRINGGRID );
    m_pChannelListStringGrid->OnClickEvent = DlgClick;
    AttachGMUIControl( m_pChannelListStringGrid );

    m_pGameTypeLabel = new CRYLLabel( this );
	m_pGameTypeLabel->SetLeft( 42 );
	m_pGameTypeLabel->SetTop( 288 );
	m_pGameTypeLabel->SetFontColor( 0xffffffff );
	m_pGameTypeLabel->SetAutoSize( TRUE );
	m_pGameTypeLabel->SetCaption( CRYLStringTable::m_strString[ 2316 ] );
	AttachGMUIControl( m_pGameTypeLabel );

    m_pGameTypeValue = new CRYLLabel( this );
	m_pGameTypeValue->SetLeft( 99 );
	m_pGameTypeValue->SetTop( 288 );
	m_pGameTypeValue->SetFontColor( 0xffffffff );
	m_pGameTypeValue->SetAutoSize( TRUE );
	m_pGameTypeValue->SetCaption( "" );
	AttachGMUIControl( m_pGameTypeValue );

    m_pTimeInfoLabel = new CRYLLabel( this );
	m_pTimeInfoLabel->SetLeft( 18 );
	m_pTimeInfoLabel->SetTop( 314 );
	m_pTimeInfoLabel->SetFontColor( 0xffffffff );
	m_pTimeInfoLabel->SetAutoSize( TRUE );
	m_pTimeInfoLabel->SetCaption( CRYLStringTable::m_strString[ 2319 ] );
	AttachGMUIControl( m_pTimeInfoLabel );

    m_pTimeInfoValue = new CRYLLabel( this );
	m_pTimeInfoValue->SetLeft( 99 );
	m_pTimeInfoValue->SetTop( 314 );
	m_pTimeInfoValue->SetFontColor( 0xffffffff );
	m_pTimeInfoValue->SetAutoSize( TRUE );
	m_pTimeInfoValue->SetCaption( "" );
	AttachGMUIControl( m_pTimeInfoValue );

    m_pMaxPeopleLabel = new CRYLLabel( this );
	m_pMaxPeopleLabel->SetLeft( 154 );
	m_pMaxPeopleLabel->SetTop( 314 );
	m_pMaxPeopleLabel->SetFontColor( 0xffffffff );
	m_pMaxPeopleLabel->SetAutoSize( TRUE );
	m_pMaxPeopleLabel->SetCaption( CRYLStringTable::m_strString[ 2317 ] );
	AttachGMUIControl( m_pMaxPeopleLabel );

    m_pMaxPeopleValue = new CRYLLabel( this );
	m_pMaxPeopleValue->SetLeft( 212 );
	m_pMaxPeopleValue->SetTop( 314 );
	m_pMaxPeopleValue->SetFontColor( 0xffffffff );
	m_pMaxPeopleValue->SetAutoSize( TRUE );
	m_pMaxPeopleValue->SetCaption( "" );
	AttachGMUIControl( m_pMaxPeopleValue );

    m_pTargetLabel = new CRYLLabel( this );
	m_pTargetLabel->SetLeft( 66 );
	m_pTargetLabel->SetTop( 340 );
	m_pTargetLabel->SetFontColor( 0xffffffff );
	m_pTargetLabel->SetAutoSize( TRUE );
	m_pTargetLabel->SetCaption( CRYLStringTable::m_strString[ 2320 ] );
	AttachGMUIControl( m_pTargetLabel );

    m_pTargetValue = new CRYLLabel( this );
	m_pTargetValue->SetLeft( 99 );
	m_pTargetValue->SetTop( 340 );
	m_pTargetValue->SetFontColor( 0xffffffff );
	m_pTargetValue->SetAutoSize( TRUE );
	m_pTargetValue->SetCaption( "" );
	AttachGMUIControl( m_pTargetValue );

    m_pCurrentPeopleLabel = new CRYLLabel( this );
	m_pCurrentPeopleLabel->SetLeft( 42 );
	m_pCurrentPeopleLabel->SetTop( 366 );
	m_pCurrentPeopleLabel->SetFontColor( 0xffffffff );
	m_pCurrentPeopleLabel->SetAutoSize( TRUE );
	m_pCurrentPeopleLabel->SetCaption( CRYLStringTable::m_strString[ 2321 ] );
	AttachGMUIControl( m_pCurrentPeopleLabel );

    m_pCurrentPeopleValue = new CRYLLabel( this );
	m_pCurrentPeopleValue->SetLeft( 99 );
	m_pCurrentPeopleValue->SetTop( 366 );
	m_pCurrentPeopleValue->SetFontColor( 0xffffffff );
	m_pCurrentPeopleValue->SetAutoSize( TRUE );
	m_pCurrentPeopleValue->SetCaption( "" );
	AttachGMUIControl( m_pCurrentPeopleValue );

    m_pScoreLabel = new CRYLLabel( this );
	m_pScoreLabel->SetLeft( 30 );
	m_pScoreLabel->SetTop( 392 );
	m_pScoreLabel->SetFontColor( 0xffffffff );
	m_pScoreLabel->SetAutoSize( TRUE );
	m_pScoreLabel->SetCaption( CRYLStringTable::m_strString[ 2322 ] );
	AttachGMUIControl( m_pScoreLabel );

    m_pScoreValue = new CRYLLabel( this );
	m_pScoreValue->SetLeft( 99 );
	m_pScoreValue->SetTop( 392 );
	m_pScoreValue->SetFontColor( 0xffffffff );
	m_pScoreValue->SetAutoSize( TRUE );
	m_pScoreValue->SetCaption( "" );
	AttachGMUIControl( m_pScoreValue );

    CGMImageDib* pSprite			= CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 );
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance();

    m_pJoinPlayerButton = new CRYLButton( this );
    m_pJoinPlayerButton->SetLeft( 41 );
	m_pJoinPlayerButton->SetTop( 417 );
    m_pJoinPlayerButton->SetCaption( CRYLStringTable::m_strString[ 2323 ] );
	m_pJoinPlayerButton->SetFontColor( 0xffffffff);
	m_pJoinPlayerButton->SetColor( 0xffffffff);
	m_pJoinPlayerButton->OnClickEvent = DlgClick;
    m_pJoinPlayerButton->SetInstance( RYL_BATTLEGROUND_STATUS_DLG::CHANNEL_JOIN_PLAYER );
	m_pJoinPlayerButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 6 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL );
	m_pJoinPlayerButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 6 ), CRYLImageCabinet::EDCT_BUTTON_DOWN );
	m_pJoinPlayerButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 6 ), CRYLImageCabinet::EDCT_BUTTON_UP );
	m_pJoinPlayerButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 6 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE );
	AttachGMUIControl( m_pJoinPlayerButton);

    m_pJoinSpectatorButton = new CRYLButton( this );
    m_pJoinSpectatorButton->SetLeft( 157 );
	m_pJoinSpectatorButton->SetTop( 417 );
    m_pJoinSpectatorButton->SetCaption( CRYLStringTable::m_strString[ 2324 ] );
	m_pJoinSpectatorButton->SetFontColor( 0xffffffff);
	m_pJoinSpectatorButton->SetColor( 0xffffffff);
	m_pJoinSpectatorButton->OnClickEvent = DlgClick;
    m_pJoinSpectatorButton->SetInstance( RYL_BATTLEGROUND_STATUS_DLG::CHANNEL_JOIN_SPECTATOR );
	m_pJoinSpectatorButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 6 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL );
	m_pJoinSpectatorButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 6 ), CRYLImageCabinet::EDCT_BUTTON_DOWN );
	m_pJoinSpectatorButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 6 ), CRYLImageCabinet::EDCT_BUTTON_UP );
	m_pJoinSpectatorButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 6 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE );
	AttachGMUIControl( m_pJoinSpectatorButton);
}
                                        
//-----------------------------------------------------------------------------------------------------
// CRYLBattleGroundStatusDlg::FinalCtrl()
VOID CRYLBattleGroundStatusDlg::FinalCtrl()
{
    GM_DELETE( m_pSortFormLabel );
    GM_DELETE( m_pSortFormValue );

	GM_DELETE( m_pCHANNEL ) ;
	GM_DELETE( m_pGAMETYPE ) ;
	GM_DELETE( m_pMAXPEOPLE ) ;
	GM_DELETE( m_pREMAINTIME ) ;

    GM_DELETE( m_pChannelListStringGrid );    

    GM_DELETE( m_pGameTypeLabel );
    GM_DELETE( m_pGameTypeValue );

    GM_DELETE( m_pTimeInfoLabel );
    GM_DELETE( m_pTimeInfoValue );

    GM_DELETE( m_pMaxPeopleLabel );
    GM_DELETE( m_pMaxPeopleValue );

    GM_DELETE( m_pTargetLabel );
    GM_DELETE( m_pTargetValue );

    GM_DELETE( m_pCurrentPeopleLabel );
    GM_DELETE( m_pCurrentPeopleValue );

    GM_DELETE( m_pScoreLabel );
    GM_DELETE( m_pScoreValue );

	GM_DELETE( m_pJoinPlayerButton ) ;
	GM_DELETE( m_pJoinSpectatorButton ) ;
}

//-----------------------------------------------------------------------------------------------------
// CRYLBattleGroundStatusDlg::Show()
VOID CRYLBattleGroundStatusDlg::Show( CGMUIObject* Sender )
{
    CRYLGameData::Instance()->m_bSimpleMouse = TRUE;
    CRYLGameData::Instance()->m_dwKindCursor = CURSOR_NORMAL;

    m_pGameTypeValue->SetCaption( "" );
    m_pTimeInfoValue->SetCaption( "" );
    m_pMaxPeopleValue->SetCaption( "" );
    m_pTargetValue->SetCaption( "" );
    m_pCurrentPeopleValue->SetCaption( "" );
    m_pScoreValue->SetCaption( "" );
}

//-----------------------------------------------------------------------------------------------------
// CRYLBattleGroundStatusDlg::Hide()
VOID CRYLBattleGroundStatusDlg::Hide( CGMUIObject* Sender )
{
    CRYLGameData::Instance()->m_dwKindCursor = CURSOR_NORMAL;
    CRYLGameData::Instance()->m_bSimpleMouse = FALSE;

    ClearChannelInfo();
}

//-----------------------------------------------------------------------------------------------------
// CRYLBattleGroundStatusDlg::GMUIPaint()
VOID CRYLBattleGroundStatusDlg::GMUIPaint( INT nPosX, INT nPosY )
{
    CRYLDialog::GMUIPaint( nPosX, nPosY );
}

//-----------------------------------------------------------------------------------------------------
// CRYLBattleGroundStatusDlg::Update()
HRESULT CRYLBattleGroundStatusDlg::Update( BOOL& bClick, BOOL& bEdge )
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

    return S_OK;
}


//-----------------------------------------------------------------------------------------------------
// CRYLBattleGroundStatusDlg::DlgClick()
VOID CRYLBattleGroundStatusDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y )
{
    if( !g_BattleGroundStatusDlg )
        return;

    switch( Sender->GetInstance() )
    {
    case RYL_BATTLEGROUND_STATUS_DLG::CHANNEL_STRINGGRID:
        {
            g_BattleGroundStatusDlg->SelectChannel();
        }
        break;

    case RYL_BATTLEGROUND_STATUS_DLG::CHANNEL_JOIN_PLAYER:
        {
            // join game as player
            g_BattleGroundStatusDlg->JoinChannel( true );
        }
        break;

    case RYL_BATTLEGROUND_STATUS_DLG::CHANNEL_JOIN_SPECTATOR:
        {
            CRYLGameData::Instance()->m_dwClientMode = CLIENT_ZONEMOVE;

            // join game as spectator
            g_BattleGroundStatusDlg->JoinChannel( false );
        }
        break;
    }
}

//-----------------------------------------------------------------------------------------------------
// CRYLBattleGroundStatusDlg::ChangeChennelStatus()
VOID CRYLBattleGroundStatusDlg::ChangeChennelStatus( int nIndex, ChannelInfo* pChannelInfo )
{
    ChannelInfo* pTempInfo = new ChannelInfo( *pChannelInfo );
    m_ltChannelInfo.push_back( pTempInfo );

    UpdateChannelListStringGrid();
}

//-----------------------------------------------------------------------------------------------------
// CRYLBattleGroundStatusDlg::SelectChannel()                                                                                                                   
VOID CRYLBattleGroundStatusDlg::SelectChannel()
{
    int nIndex = m_pChannelListStringGrid->GetItemIndex();

    if( nIndex == -1 )
    {
        m_pGameTypeValue->SetCaption( "" );
        m_pTimeInfoValue->SetCaption( "" );
        m_pMaxPeopleValue->SetCaption( "" );
        m_pTargetValue->SetCaption( "" );
        m_pCurrentPeopleValue->SetCaption( "" );
        m_pScoreValue->SetCaption( "" );
        return;
    }
    else
    {
        UpdateSelectedChannelInfo( nIndex );
    }
}

//-----------------------------------------------------------------------------------------------------
// CRYLBattleGroundStatusDlg::ClearChannelInfo()
VOID CRYLBattleGroundStatusDlg::ClearChannelInfo()
{
    if( !m_ltChannelInfo.empty() )
    {
        std::list<ChannelInfo*>::iterator iter = m_ltChannelInfo.begin();

        while( iter != m_ltChannelInfo.end() )
        {
            GM_DELETE( (*iter) );
            ++iter;
        }

        m_ltChannelInfo.clear();
    }
}

//-----------------------------------------------------------------------------------------------------
// CRYLBattleGroundStatusDlg::UpdateChannelListStringGrid()
VOID CRYLBattleGroundStatusDlg::UpdateChannelListStringGrid()
{
    m_pChannelListStringGrid->ClearItem();

    if( !m_ltChannelInfo.empty() )
    {
        std::list<ChannelInfo*>::iterator iter = m_ltChannelInfo.begin();
        size_t nChannelNum = min( m_ltChannelInfo.size(), 10L );

        for( unsigned int i = 0; i < nChannelNum; ++i )
        {
            char    pstrTemp[ 32 ] = { 0, };
            DWORD   dwFontColor = (*iter)->m_bPlaying ? 0xFFFFFFFF : 0xFFFF0000;
            
            // channel number
            sprintf( pstrTemp, "%d", static_cast<int>( (*iter)->m_ucChannelNum ) );
            m_pChannelListStringGrid->AddItem( 0, pstrTemp, 46, dwFontColor ); 

            // game type
            switch( (*iter)->m_cMapType )
            {
            case 0:
                {
                    sprintf( pstrTemp, CRYLStringTable::m_strString[ 2325 ] );
                }
                break;

            case 1:
                {
                    sprintf( pstrTemp, CRYLStringTable::m_strString[ 2326 ] );
                }
                break;
            }
            m_pChannelListStringGrid->AddItem( 1, pstrTemp, 108, dwFontColor );

            // max number of people
            sprintf( pstrTemp, CRYLStringTable::m_strString[ 2327 ], static_cast<int>( (*iter)->m_cMaxCharNumOfNation ) );
            m_pChannelListStringGrid->AddItem( 2,  pstrTemp, 57, dwFontColor );

            // time info
            sprintf( pstrTemp, CRYLStringTable::m_strString[ 2328 ], static_cast<int>( (*iter)->m_cRemainMin ) );
            m_pChannelListStringGrid->AddItem( 3, pstrTemp, 47, dwFontColor );

            ++iter;
        }
    }
}

//-----------------------------------------------------------------------------------------------------
// CRYLBattleGroundStatusDlg::UpdateSelectedChannelInfo()
VOID CRYLBattleGroundStatusDlg::UpdateSelectedChannelInfo( int nIndex )
{
	if( !m_ltChannelInfo.empty() )
	{
		std::list<ChannelInfo*>::iterator iter = m_ltChannelInfo.begin();

		for( int i = 0; i < nIndex; ++i )
		{
			// increase iterator
			++iter;

			if( iter == m_ltChannelInfo.end() )
				return;
		}

		string strTemp;
		char  strTemp1[ 64 ] = { 0, };

		// game type, target
		switch( (*iter)->m_cMapType )
		{
			case 0:
			{
				strTemp = CRYLStringTable::m_strString[ 2325 ];
				sprintf( strTemp1, CRYLStringTable::m_strString[ 2329 ], (*iter)->m_wTargetScore );
			}
			break;

			case 1:
			{
				strTemp = CRYLStringTable::m_strString[ 2326 ];
				sprintf( strTemp1, CRYLStringTable::m_strString[ 2330 ] );
			}
			break;
		}

		m_pGameTypeValue->SetCaption( const_cast<LPSTR>( strTemp.c_str() ) );
		m_pTargetValue->SetCaption( static_cast<LPSTR>( strTemp1 ) );

		// max people
		sprintf( strTemp1, "%d", (*iter)->m_cMaxCharNumOfNation );
		m_pMaxPeopleValue->SetCaption( static_cast<LPSTR>( strTemp1 ) );

		// time info
		strTemp = (*iter)->m_bPlaying ? CRYLStringTable::m_strString[ 2331 ] : CRYLStringTable::m_strString[ 2319 ];
		m_pTimeInfoLabel->SetCaption( const_cast<LPSTR>( strTemp.c_str() ) );
		sprintf( strTemp1, CRYLStringTable::m_strString[ 2328 ], (*iter)->m_cRemainMin );
		m_pTimeInfoValue->SetCaption( static_cast<LPSTR>( strTemp1 ) );

		// currnet number of people
		sprintf( strTemp1, CRYLStringTable::m_strString[ 2332 ], (*iter)->m_cCurrentCharNum[ 0 ], (*iter)->m_cCurrentCharNum[ 1 ] );
		m_pCurrentPeopleValue->SetCaption( static_cast<LPSTR>( strTemp1 ) );

		// score
		sprintf( strTemp1, CRYLStringTable::m_strString[ 2332 ], (*iter)->m_wScore[ 0 ], (*iter)->m_wScore[ 1 ] );
		m_pScoreValue->SetCaption( static_cast<LPSTR>( strTemp1 ) );
	}
}

//-----------------------------------------------------------------------------------------------------
// CRYLBattleGroundStatusDlg::JoinChannel()
VOID CRYLBattleGroundStatusDlg::JoinChannel( bool bIsPlayer )
{
    int nIndex = m_pChannelListStringGrid->GetItemIndex();

    if( nIndex == -1 )
        return;

    std::list<ChannelInfo*>::iterator iter = m_ltChannelInfo.begin();

    for( int i = 0; i < nIndex; ++i )
    {
        // increase iterator
        ++iter;

        if( iter == m_ltChannelInfo.end() )
            return;
    }

    m_bIsPlayer = static_cast<BOOL>( bIsPlayer );

    SendPacket::CharBGServerMoveZone( g_GameSession, (*iter)->m_ucChannelNum, bIsPlayer ? 0 : 1 );
}

//-----------------------------------------------------------------------------------------------------
// CRYLBattleGroundStatusDlg::CheckChannelStart()
VOID CRYLBattleGroundStatusDlg::CheckChannelStart( int nIndex, bool bPlaying )
{
    if( !m_ltChannelInfo.empty() && m_ltChannelInfo.size() > static_cast<unsigned int>( nIndex ) )
    {
        std::list<ChannelInfo*>::iterator iter = m_ltChannelInfo.begin();

        for( int i = 0 ; i < nIndex ; ++i )
        {
            ++iter;
        }

        if( !(*iter)->m_bPlaying && bPlaying )
        {
            char strTemp[ 128 ] = { 0, };
			sprintf( strTemp, CRYLStringTable::m_strString[ 2333 ], nIndex + 1 );
            CRYLChattingDlg::Instance()->AddMessage( strTemp, CRYLNetworkData::Instance()->m_dwMyChrID, 128 );
        }
        else if( (*iter)->m_bPlaying && !bPlaying )
        {
            char strTemp[ 128 ] = { 0, };
            sprintf( strTemp, CRYLStringTable::m_strString[ 2334 ], nIndex + 1 );
            CRYLChattingDlg::Instance()->AddMessage( strTemp, CRYLNetworkData::Instance()->m_dwMyChrID, 128 );
        }
    }
}

//-----------------------------------------------------------------------------------------------------
// CRYLBattleGroundStatusDlg::