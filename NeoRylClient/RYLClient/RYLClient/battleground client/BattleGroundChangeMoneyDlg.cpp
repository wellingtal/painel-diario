//-----------------------------------------------------------------------------------------------------
// File Name: BattleGroundChangeMoneyDlg.cpp
//
// Programmer: Zergra( Park Jongtae ) in GamaSoft corp.
//
// Date: 2004. 2. 12. Tuesday
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
#include <Network/Packet/PacketStruct/BGServerPacket.h>
#include <Network/ClientSocket/Send/SendBGServer.h>

#include "RYLButton.h"
#include "RYLLabel.h"
#include "RYLEdit.h"
#include "RYLStringGrid.h"

#include "RYLMessageBox.h"
#include "RYLMessageBoxManager.h"

#include "RYLChattingDlg.h"
#include "BattleGroundChangeMoneyDlg.h"
#include "GMMemory.h"

// Rodin : Part II에서는 배틀 로한이 삭제되었습니다. 이에 본 소스는 유지/보수하지 않습니다.
string g_strServerName[6] = { 
	CRYLStringTable::m_strString[ 2229 ], 
	CRYLStringTable::m_strString[ 2230 ], 
	CRYLStringTable::m_strString[ 2231 ], 
	CRYLStringTable::m_strString[ 2232 ], 
	CRYLStringTable::m_strString[ 2232 ], 
	CRYLStringTable::m_strString[ 2232 ] 
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
// class CRYLBattleGroundChangeMoneyDlg
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
CRYLBattleGroundChangeMoneyDlg* g_BattleGroundChangeMoneyDlg = NULL;
//-----------------------------------------------------------------------------------------------------
// constructor, destructor
//-----------------------------------------------------------------------------------------------------
CRYLBattleGroundChangeMoneyDlg::CRYLBattleGroundChangeMoneyDlg()
{
    m_pDialogNameLabel = NULL;
    m_pServerListLabel = NULL;
    m_pSelectedServerValue = NULL;
    m_pSelectedServerLabel = NULL;
    m_pCurrentMoneyValue = NULL;
    m_pCurrentMedalLabel = NULL;
    m_pCurrentMedalValue = NULL;
    m_pSelectMedalLabel = NULL;
    m_pInputMedalValueEdit = NULL;
    m_pChangedMoneyLabel = NULL;
    m_pChangedMoneyValue = NULL;

    m_pServerListStringGrid = NULL;

    m_pAcceptChangeButton = NULL;
    m_pCancelChangeButton = NULL;

    //--  initialize dialog image and controls --//
    RECT			rt;
	CGMImageDib*	pImageDIB = NULL;
	CRYLImage*		pImage	  = NULL;

    // 캡션바
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

    // 몸통
    CRYLSpriteList::Instance()->CreateGMImage( 290, 235, &pImageDIB );
	pImageDIB->ClearbyColor( 0x00000000 );	

    for( int i = 0; i < 45 ; ++i )
    {
        SetRect( &rt, 0, 76, 256, 81 );
        pImageDIB->DrawRect( &rt, 0, i * 5, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BATTLEGROUND_STATUS_256x256 ) );
        SetRect( &rt, 34, 38, 68, 43 );
	    pImageDIB->DrawRect( &rt, 256, i * 5, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BATTLEGROUND_STATUS_256x256 ) );
    }

    // 동전 모양
    SetRect( &rt, 188, 97, 213, 117 ) ;
	pImageDIB->DrawRect( &rt, 95, 30, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
    
    // 바닥
	SetRect( &rt, 0, 81, 256, 91 );
    pImageDIB->DrawRect( &rt, 0, 225, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BATTLEGROUND_STATUS_256x256 ) );
    SetRect( &rt, 68, 38, 102, 48 );
	pImageDIB->DrawRect( &rt, 256, 225, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BATTLEGROUND_STATUS_256x256 ) );

    // frame rect
    pImageDIB->MakeFrameRect(  7,  6, 81,190, 0xffB99E6B, 0x80000000, 1 );   // channel list frame
    pImageDIB->MakeFrameRect( 12, 30, 72,  1, 0xff6e6857, 0x80000000, 1 );   // 구분선
    pImageDIB->MakeFrameRect( 95,  6, 81, 20, 0xff6e6857, 0x80000000, 1 );   // selected server name value frame
    pImageDIB->MakeFrameRect(115, 30,133, 20, 0xffB99E6B, 0x80000000, 1 );   // selected server money value frame
    pImageDIB->MakeFrameRect( 95, 80,157, 20, 0xff6e6857, 0x80000000, 1 );   // currnet medal value frame
    pImageDIB->MakeFrameRect( 95,129,157, 20, 0xff6e6857, 0x80000000, 1 );   // input medal value frame
    pImageDIB->MakeFrameRect( 95,177,157, 20, 0xff6e6857, 0x80000000, 1 );   // change money value frame

    pImage = CRYLImageManager::Instance()->CreateRYLImage();
	pImage->LoadFromImage( pImageDIB );
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage );
	AttachWindowTexture( ( CGMTexture*)pImage );

	GM_DELETE( pImageDIB );

    InitCtrl();

    // initialize dialog first position
    SetWidth( 290 );
	SetHeight( 240 );
    SetLeft( ( CGMUIScreen::GetInstance()->GetWidth() - GetWidth() ) / 2 - 50 );
    SetTop( ( CGMUIScreen::GetInstance()->GetHeight() - GetHeight() ) / 2 - 50 );

    SetFormType( RYLDLG_FRAME_TYPE_CLOSEBUTTON );

    // set global pointer
    g_BattleGroundChangeMoneyDlg = this;

    //-- initialize member variables --//    
    m_cGroup = 0;
    m_dwGold = 0;
    m_dwMileage = 0;
}

CRYLBattleGroundChangeMoneyDlg::~CRYLBattleGroundChangeMoneyDlg()
{
    FinalCtrl();
}

//-----------------------------------------------------------------------------------------------------
// Member Functions
//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------
// CRYLBattleGroundChangeMoneyDlg::InitCtrl()
VOID CRYLBattleGroundChangeMoneyDlg::InitCtrl()
{
    m_pDialogNameLabel = new CRYLLabel( this );
	m_pDialogNameLabel->SetLeft( 9 );
	m_pDialogNameLabel->SetTop( 17 );
	m_pDialogNameLabel->SetFontColor( 0xffffffff );
	m_pDialogNameLabel->SetAutoSize( TRUE );
	m_pDialogNameLabel->SetCaption( CRYLStringTable::m_strString[ 2335 ] );
	AttachGMUIControl( m_pDialogNameLabel );

    m_pServerListLabel = new CRYLLabel( this );
	m_pServerListLabel->SetLeft( 18 );
	m_pServerListLabel->SetTop( 51 );
	m_pServerListLabel->SetFontColor( 0xffffffff );
	m_pServerListLabel->SetAutoSize( TRUE );
	m_pServerListLabel->SetCaption( CRYLStringTable::m_strString[ 2336 ] );
	AttachGMUIControl( m_pServerListLabel );

    m_pSelectedServerValue = new CRYLLabel( this );
	m_pSelectedServerValue->SetLeft( 108 );
	m_pSelectedServerValue->SetTop( 48 );
	m_pSelectedServerValue->SetFontColor( 0xffffffff );
	m_pSelectedServerValue->SetAutoSize( TRUE );
	m_pSelectedServerValue->SetCaption( "" );
	AttachGMUIControl( m_pSelectedServerValue );

    m_pSelectedServerLabel = new CRYLLabel( this );
	m_pSelectedServerLabel->SetLeft( 180 );
	m_pSelectedServerLabel->SetTop( 48 );
	m_pSelectedServerLabel->SetFontColor( 0xffffffff );
	m_pSelectedServerLabel->SetAutoSize( TRUE );
	m_pSelectedServerLabel->SetCaption( CRYLStringTable::m_strString[ 2337 ] );
	AttachGMUIControl( m_pSelectedServerLabel );

    m_pCurrentMoneyValue = new CRYLLabel( this );
	m_pCurrentMoneyValue->SetLeft( 171 );
	m_pCurrentMoneyValue->SetTop( 73 );
	m_pCurrentMoneyValue->SetFontColor( 0xffffffff );
	m_pCurrentMoneyValue->SetAutoSize( TRUE );
	m_pCurrentMoneyValue->SetCaption( "" );
	AttachGMUIControl( m_pCurrentMoneyValue );

    m_pCurrentMedalLabel = new CRYLLabel( this );
	m_pCurrentMedalLabel->SetLeft( 97 );
	m_pCurrentMedalLabel->SetTop( 102 );
	m_pCurrentMedalLabel->SetFontColor( 0xffffffff );
	m_pCurrentMedalLabel->SetAutoSize( TRUE );
	m_pCurrentMedalLabel->SetCaption( CRYLStringTable::m_strString[ 2338 ] );
	AttachGMUIControl( m_pCurrentMedalLabel );

    m_pCurrentMedalValue = new CRYLLabel( this );
	m_pCurrentMedalValue->SetLeft( 171 );
	m_pCurrentMedalValue->SetTop( 123 );
	m_pCurrentMedalValue->SetFontColor( 0xffffffff );
	m_pCurrentMedalValue->SetAutoSize( TRUE );
	m_pCurrentMedalValue->SetCaption( "" );
	AttachGMUIControl( m_pCurrentMedalValue );

    m_pSelectMedalLabel = new CRYLLabel( this );
	m_pSelectMedalLabel->SetLeft( 97 );
	m_pSelectMedalLabel->SetTop( 151 );
	m_pSelectMedalLabel->SetFontColor( 0xffffffff );
	m_pSelectMedalLabel->SetAutoSize( TRUE );
	m_pSelectMedalLabel->SetCaption( CRYLStringTable::m_strString[ 2339 ] );
	AttachGMUIControl( m_pSelectMedalLabel );

    m_pInputMedalValueEdit = new CRYLEdit( this );
	m_pInputMedalValueEdit->SetLeft( 96 );
	m_pInputMedalValueEdit->SetTop( 168 );
	m_pInputMedalValueEdit->SetWidth( 155 );
	m_pInputMedalValueEdit->SetHeight( 18 );
	m_pInputMedalValueEdit->SetColor( 0x80000000 );
	m_pInputMedalValueEdit->SetFontColor( 0xffffffff );
    m_pInputMedalValueEdit->SetInstance( RYL_BATTLEGROUND_CHANGE_MONEY_DLG::INPUT_MEDAL_VALUE_EDIT );
	m_pInputMedalValueEdit->SetMaxLength( 10 );
    m_pInputMedalValueEdit->SetText( "" );
    m_pInputMedalValueEdit->SetNumeralOnly( TRUE );
    m_pInputMedalValueEdit->OnKeyDown = DlgKeyDown;
	AttachGMUIControl( m_pInputMedalValueEdit );

    m_pChangedMoneyLabel = new CRYLLabel( this );
	m_pChangedMoneyLabel->SetLeft( 98 );
	m_pChangedMoneyLabel->SetTop( 198 );
	m_pChangedMoneyLabel->SetFontColor( 0xffffffff );
	m_pChangedMoneyLabel->SetAutoSize( TRUE );
	m_pChangedMoneyLabel->SetCaption( CRYLStringTable::m_strString[ 2340 ] );
	AttachGMUIControl( m_pChangedMoneyLabel );

    m_pChangedMoneyValue = new CRYLLabel( this );
	m_pChangedMoneyValue->SetLeft( 171 );
	m_pChangedMoneyValue->SetTop( 220 );
	m_pChangedMoneyValue->SetFontColor( 0xffffffff );
	m_pChangedMoneyValue->SetAutoSize( TRUE );
	m_pChangedMoneyValue->SetCaption( "" );
	AttachGMUIControl( m_pChangedMoneyValue );

    m_pServerListStringGrid = new CRYLStringGrid( this, FALSE );
    m_pServerListStringGrid->SetLeft( 12 );
    m_pServerListStringGrid->SetTop( 73 );
    m_pServerListStringGrid->SetWidth( 67 );
    m_pServerListStringGrid->SetHeight( 147 );
    m_pServerListStringGrid->SetColCount( 7 );
    m_pServerListStringGrid->SetItemHeight( 16 );
    m_pServerListStringGrid->SetFontColor( 0xFFFFFFFF );
    m_pServerListStringGrid->SetInstance( RYL_BATTLEGROUND_CHANGE_MONEY_DLG::SERVER_LIST_STRING_GRID );
    m_pServerListStringGrid->OnClickEvent = DlgClick;
    for( int i = 0 ; i < 6 ; ++i )
    {
        m_pServerListStringGrid->AddItem( 0, const_cast<LPSTR>( g_strServerName[ i ].c_str() ), 67, 0xFFFFFFFF );
    }
    AttachGMUIControl( m_pServerListStringGrid );

    CGMImageDib* pSprite			= CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 );
	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance();

    m_pAcceptChangeButton = new CRYLButton( this );
    m_pAcceptChangeButton->SetLeft( 66 );
	m_pAcceptChangeButton->SetTop( 240 );
    m_pAcceptChangeButton->SetCaption( CRYLStringTable::m_strString[ 2341 ] );
	m_pAcceptChangeButton->SetFontColor( 0xffffffff );
	m_pAcceptChangeButton->SetColor( 0xffffffff );
	m_pAcceptChangeButton->OnClickEvent = DlgClick;
    m_pAcceptChangeButton->SetInstance( RYL_BATTLEGROUND_CHANGE_MONEY_DLG::CHANGE_MONEY_ACCEPT_BUTTON );
	m_pAcceptChangeButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 6 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL );
	m_pAcceptChangeButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 6 ), CRYLImageCabinet::EDCT_BUTTON_DOWN );
	m_pAcceptChangeButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 6 ), CRYLImageCabinet::EDCT_BUTTON_UP );
	m_pAcceptChangeButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 6 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE );
	AttachGMUIControl( m_pAcceptChangeButton );

    m_pCancelChangeButton = new CRYLButton( this );
    m_pCancelChangeButton->SetLeft( 182 );
	m_pCancelChangeButton->SetTop( 240 );
    m_pCancelChangeButton->SetCaption( CRYLStringTable::m_strString[ 126 ] );
	m_pCancelChangeButton->SetFontColor( 0xffffffff );
	m_pCancelChangeButton->SetColor( 0xffffffff );
	m_pCancelChangeButton->OnClickEvent = DlgClick;
    m_pCancelChangeButton->SetInstance( RYL_BATTLEGROUND_CHANGE_MONEY_DLG::CHANGE_MONEY_CANCEL_BUTTON );
	m_pCancelChangeButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 6 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL );
	m_pCancelChangeButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 6 ), CRYLImageCabinet::EDCT_BUTTON_DOWN );
	m_pCancelChangeButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 6 ), CRYLImageCabinet::EDCT_BUTTON_UP );
	m_pCancelChangeButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 6 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE );
	AttachGMUIControl( m_pCancelChangeButton );
}

//-----------------------------------------------------------------------------------------------------
// CRYLBattleGroundChangeMoneyDlg::InitCtrl()
VOID CRYLBattleGroundChangeMoneyDlg::FinalCtrl()
{
    GM_DELETE( m_pDialogNameLabel );
    GM_DELETE( m_pServerListLabel );
    GM_DELETE( m_pSelectedServerValue );
    GM_DELETE( m_pSelectedServerLabel );
    GM_DELETE( m_pCurrentMoneyValue );
    GM_DELETE( m_pCurrentMedalLabel );
    GM_DELETE( m_pCurrentMedalValue );
    GM_DELETE( m_pSelectMedalLabel );
    GM_DELETE( m_pInputMedalValueEdit );
    GM_DELETE( m_pChangedMoneyLabel );
    GM_DELETE( m_pChangedMoneyValue );

    GM_DELETE( m_pServerListStringGrid );

    GM_DELETE( m_pAcceptChangeButton );
    GM_DELETE( m_pCancelChangeButton );
}

//-----------------------------------------------------------------------------------------------------
// CRYLBattleGroundChangeMoneyDlg::Show()
VOID CRYLBattleGroundChangeMoneyDlg::Show( CGMUIObject* Sender )
{
    CRYLGameData::Instance()->m_bSimpleMouse = TRUE;
    CRYLGameData::Instance()->m_dwKindCursor = CURSOR_NORMAL;

    m_pInputMedalValueEdit->SetText( "" );
    m_pInputMedalValueEdit->SetFocus();
    m_pInputMedalValueEdit->SetFocusState( TRUE );

    m_pSelectedServerValue->SetCaption( "" );
    m_pCurrentMoneyValue->SetCaption( "" );
    m_pCurrentMedalValue->SetCaption( "" );
    m_pChangedMoneyValue->SetCaption( "" );

    CRYLGameData *pGame = CRYLGameData::Instance() ;
    char pstrCurrentMedalValue[ 32 ] = { 0, };
    sprintf( pstrCurrentMedalValue, "%u", pGame->m_csStatus.m_Info.Mileage ) ;
	m_pCurrentMedalValue->SetCaption( pstrCurrentMedalValue );
    m_dwMileage = pGame->m_csStatus.m_Info.Mileage;

    m_pServerListStringGrid->SetItemIndex( m_cGroup - 1 );
    if ( m_cGroup != 0 )
    {
        SendPacket::CharBGServerCharSlot( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, m_cGroup );
    }
}

//-----------------------------------------------------------------------------------------------------
// CRYLBattleGroundChangeMoneyDlg::Hide()
VOID CRYLBattleGroundChangeMoneyDlg::Hide( CGMUIObject* Sender )
{
    CRYLGameData::Instance()->m_dwKindCursor = CURSOR_NORMAL;
    CRYLGameData::Instance()->m_bSimpleMouse = FALSE;

    m_pInputMedalValueEdit->SetText( "" );
    CRYLChattingDlg::Instance()->m_pChatEdit->SetFocus();
    CRYLChattingDlg::Instance()->m_pChatEdit->SetFocusState( FALSE );

    m_pServerListStringGrid->SelectItemIndex( -1 );
}

//-----------------------------------------------------------------------------------------------------
// CRYLBattleGroundChangeMoneyDlg::GMUIPaint()
VOID CRYLBattleGroundChangeMoneyDlg::GMUIPaint( INT nPosX, INT nPosY )
{
    CRYLDialog::GMUIPaint( nPosX, nPosY );
}

//-----------------------------------------------------------------------------------------------------
// CRYLBattleGroundChangeMoneyDlg::Update()
HRESULT	CRYLBattleGroundChangeMoneyDlg::Update( BOOL &bClick, BOOL &bEdge )
{
	if (FALSE == GetVisible())
	{
		return S_OK;
	}

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

    return S_OK;
}

//-----------------------------------------------------------------------------------------------------
// CRYLBattleGroundChangeMoneyDlg::DlgKeyDown()
VOID CRYLBattleGroundChangeMoneyDlg::DlgKeyDown( CGMUIObject* Sender, WORD key, INT Shift ) 
{
    if ( !g_BattleGroundChangeMoneyDlg )
        return;

    switch( Sender->GetInstance() )
    {
    case RYL_BATTLEGROUND_CHANGE_MONEY_DLG::INPUT_MEDAL_VALUE_EDIT:
        {   
            g_BattleGroundChangeMoneyDlg->SetChangeMoneyValue();
        }
        break;
    }
}    

//-----------------------------------------------------------------------------------------------------
// CRYLBattleGroundChangeMoneyDlg::DlgClick()
VOID CRYLBattleGroundChangeMoneyDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y )
{
    if ( !g_BattleGroundChangeMoneyDlg )
        return;

    switch( Sender->GetInstance() )
    {
    case RYL_BATTLEGROUND_CHANGE_MONEY_DLG::SERVER_LIST_STRING_GRID:
        {
            g_BattleGroundChangeMoneyDlg->SelectServer();
        }
        break;
        
    case RYL_BATTLEGROUND_CHANGE_MONEY_DLG::CHANGE_MONEY_ACCEPT_BUTTON:
        {
            g_BattleGroundChangeMoneyDlg->AcceptMileage();
        }
        break;

    case RYL_BATTLEGROUND_CHANGE_MONEY_DLG::CHANGE_MONEY_CANCEL_BUTTON:
        {
            g_BattleGroundChangeMoneyDlg->SetVisible( FALSE );
        }
        break;
    }
}

//-----------------------------------------------------------------------------------------------------
// CRYLBattleGroundChangeMoneyDlg::SelectServer()
VOID CRYLBattleGroundChangeMoneyDlg::SelectServer()
{
    int nIndex = m_pServerListStringGrid->GetItemIndex();

    if ( nIndex == -1 )
    {
        m_pSelectedServerValue->SetCaption( "" );
        m_pCurrentMoneyValue->SetCaption( "" );
        m_pCurrentMedalValue->SetCaption( "" );
        m_pChangedMoneyValue->SetCaption( "" );
    }
    else
    {
        SendPacket::CharBGServerCharSlot( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, nIndex + 1 );
    }
}

//-----------------------------------------------------------------------------------------------------
// CRYLBattleGroundChangeMoneyDlg::SetValue()
VOID CRYLBattleGroundChangeMoneyDlg::SetValue( unsigned char cGroup, unsigned long dwGold, unsigned long dwMileage )
{
    m_cGroup    = cGroup;
    m_dwGold    = dwGold;
    m_dwMileage = dwMileage;

    char    strTemp[ 36 ] = { 0, };

    m_pSelectedServerValue->SetCaption( const_cast<LPSTR>( g_strServerName[ cGroup - 1 ].c_str() ) );
    sprintf( strTemp, "%u", dwGold );       
    m_pCurrentMoneyValue->SetCaption( strTemp );
    sprintf( strTemp, "%u", dwMileage );       
    m_pCurrentMedalValue->SetCaption( strTemp );
}

//-----------------------------------------------------------------------------------------------------
// CRYLBattleGroundChangeMoneyDlg::SetChangeMoneyValue()
VOID CRYLBattleGroundChangeMoneyDlg::SetChangeMoneyValue()
{
    string  strTemp1 = m_pInputMedalValueEdit->GetText();
    DWORD   dwTemp = atoi( strTemp1.c_str() );

    if ( dwTemp > m_dwMileage )
        dwTemp = m_dwMileage;

    char    strTemp2[ 36 ] = { 0, };
    sprintf( strTemp2, "%u", dwTemp );
    m_pInputMedalValueEdit->SetText( strTemp2 );

    if ( ( dwTemp * 10 ) + m_dwGold > 4200000000 )
    {
        CRYLMessageBox *lpMessageBox;
        lpMessageBox = new CRYLMessageBox;
		lpMessageBox->Create( CRYLStringTable::m_strString[ 2342 ] );

        if ( ( dwTemp * 10 ) + m_dwGold > 4200000000 )
            dwTemp = ( 4200000000 - m_dwMileage * 10 ) / 10;

        sprintf( strTemp2, "%u", dwTemp );
        m_pInputMedalValueEdit->SetText( strTemp2 );
    }

    char strTemp[ 36 ] = { 0, };
    sprintf( strTemp, "%u", atoi( m_pInputMedalValueEdit->GetText() ) * 10 );
    m_pChangedMoneyValue->SetCaption( strTemp );
}

//-----------------------------------------------------------------------------------------------------
// CRYLBattleGroundChangeMoneyDlg::AcceptMileage()
VOID CRYLBattleGroundChangeMoneyDlg::AcceptMileage()
{
	SendPacket::CharBGServerMileageChange( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, m_cGroup, 
		                                   PktBGServerMileageChange::MC_CHANGE, atoi(m_pInputMedalValueEdit->GetText()) );
}

//-----------------------------------------------------------------------------------------------------
// CRYLBattleGroundChangeMoneyDlg::SetAcceptButtonValidate()
VOID CRYLBattleGroundChangeMoneyDlg::SetAcceptButtonValidate( bool bValidate )
{
    if( bValidate )
    {
        m_pAcceptChangeButton->SetEnable( TRUE );
    }
    else
    {
        m_pAcceptChangeButton->SetEnable( FALSE );
    }
}

//-----------------------------------------------------------------------------------------------------
// CRYLBattleGroundChangeMoneyDlg::