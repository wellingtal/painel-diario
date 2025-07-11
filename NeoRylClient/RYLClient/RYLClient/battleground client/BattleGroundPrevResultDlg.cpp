//-----------------------------------------------------------------------------------------------------
// File Name: BattleGroundPrevResultDlg.cpp
//
// Programmer: Zergra( Park Jongtae ) in GamaSoft corp.
//
// Date: 2004. 2. 4. Tuesday
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

#include "RYLLabel.h"

#include "BattleGroundPrevResultDlg.h"
#include "GMMemory.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
// class CRYLBattleGroundPrevResultDlg
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------------------------------
// Constructor, Destructor
//-----------------------------------------------------------------------------------------------------
CRYLBattleGroundPrevResultDlg::CRYLBattleGroundPrevResultDlg()
{
    m_pDlgNameLabel = NULL;

    for( int i = 0 ; i < MAX_CHANNEL_NUMBER ; ++i )
    {
        m_pChannelNumberLabel[ i ] = NULL;
        m_pChannelResultLabel[ i ] = NULL;
    }

    //--  make dialog image --//
    CGMImageDib*	pImageDIB = NULL;
	CRYLImage*		pImage	  = NULL;
	RECT			rt;

    // Ä¸¼Ç¹Ù
    CRYLSpriteList::Instance()->CreateGMImage( 256, 15, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 0, 256, 15 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	AttachCaptionTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;

    // ¸öÅë
	CRYLSpriteList::Instance()->CreateGMImage( 256, 5 * 5 * ( MAX_CHANNEL_NUMBER - 1 ) + 10, &pImageDIB );
	pImageDIB->ClearbyColor( 0x00000000 );	
    int i = 0;
	for( ; i < 5 * ( MAX_CHANNEL_NUMBER - 1 ) ; ++i )
	{
		SetRect( &rt, 0, 38, 256, 43 );
		pImageDIB->DrawRect( &rt, 0, ( i * 5 ), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) );
	}

    // ¹Ù´Ú
	SetRect( &rt, 0, 42, 256, 52 );
	pImageDIB->DrawRect( &rt, 0, ( i - 1 ) * 5, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) );

    // frame rect
    pImageDIB->MakeFrameRect(   45,  17, 200,  18, 0xffB99E6B, 0x80000000,  1 ) ;
    pImageDIB->MakeFrameRect(   45,  37, 200,  18, 0xffB99E6B, 0x80000000,  1 ) ;
    pImageDIB->MakeFrameRect(   45,  57, 200,  18, 0xffB99E6B, 0x80000000,  1 ) ;
    pImageDIB->MakeFrameRect(   45,  77, 200,  18, 0xffB99E6B, 0x80000000,  1 ) ;
    pImageDIB->MakeFrameRect(   45,  97, 200,  18, 0xffB99E6B, 0x80000000,  1 ) ;
    pImageDIB->MakeFrameRect(   45, 117, 200,  18, 0xffB99E6B, 0x80000000,  1 ) ;
    pImageDIB->MakeFrameRect(   45, 137, 200,  18, 0xffB99E6B, 0x80000000,  1 ) ;
    pImageDIB->MakeFrameRect(   45, 157, 200,  18, 0xffB99E6B, 0x80000000,  1 ) ;

    pImage = CRYLImageManager::Instance()->CreateRYLImage();
	pImage->LoadFromImage( pImageDIB );
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage );
	
	AttachWindowTexture( ( CGMTexture*)pImage );

	GM_DELETE( pImageDIB );

    // initialize dialog first position
    SetWidth( 256 );
	SetHeight( 105 );

	SetLeft( ( CGMUIScreen::GetInstance()->GetWidth() - GetWidth() ) / 2 );                                
	SetTop( ( CGMUIScreen::GetInstance()->GetHeight() - GetHeight() ) / 2 - ( 5 * MAX_CHANNEL_NUMBER / 2 ) );

    SetFormType( RYLDLG_FRAME_TYPE_CLOSEBUTTON ) ;

    InitCtrl();
}

CRYLBattleGroundPrevResultDlg::~CRYLBattleGroundPrevResultDlg()
{
    FinalCtrl() ;
}

//-----------------------------------------------------------------------------------------------------
// Member Functions
//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------
// CRYLBattleGroundPrevResultDlg::InitCtrl()
VOID CRYLBattleGroundPrevResultDlg::InitCtrl()
{
    // ÀÌ¸§
	m_pDlgNameLabel = new CRYLLabel( this );
	m_pDlgNameLabel->SetLeft( 70 );
	m_pDlgNameLabel->SetTop( 15 );
	m_pDlgNameLabel->SetFontColor(0xffffffff);
	m_pDlgNameLabel->SetAutoSize( TRUE );
	m_pDlgNameLabel->SetCaption( CRYLStringTable::m_strString[ 2310 ] );
	AttachGMUIControl( m_pDlgNameLabel );

    for( int i = 0 ; i < MAX_CHANNEL_NUMBER ; ++i )
    {
        m_pChannelNumberLabel[ i ] = new CRYLLabel( this );
	    m_pChannelNumberLabel[ i ]->SetLeft( 5 + 4 );
	    m_pChannelNumberLabel[ i ]->SetTop( 18 + 17 + ( i * 20 ) );
	    m_pChannelNumberLabel[ i ]->SetFontColor( 0xffffffff );
	    m_pChannelNumberLabel[ i ]->SetAutoSize( TRUE );
        char    strTemp[ 36 ];
        sprintf( strTemp, CRYLStringTable::m_strString[ 2311 ], i + 1 );
	    m_pChannelNumberLabel[ i ]->SetCaption( strTemp );
	    AttachGMUIControl( m_pChannelNumberLabel[ i ] );    

        m_pChannelResultLabel[ i ] = new CRYLLabel( this );
	    m_pChannelResultLabel[ i ]->SetLeft( 25 + 65 );
	    m_pChannelResultLabel[ i ]->SetTop( 18 + 17 + ( i * 20 ) );
	    m_pChannelResultLabel[ i ]->SetFontColor( 0xffffffff );
	    m_pChannelResultLabel[ i ]->SetAutoSize( TRUE );
	    m_pChannelResultLabel[ i ]->SetCaption( "" );
	    AttachGMUIControl( m_pChannelResultLabel[ i ] );
    }
}

//-----------------------------------------------------------------------------------------------------
// CRYLBattleGroundPrevResultDlg::FinalCtrl()
VOID CRYLBattleGroundPrevResultDlg::FinalCtrl()
{
    GM_DELETE( m_pDlgNameLabel );

    for( int i = 0 ; i < MAX_CHANNEL_NUMBER ; ++i )
    {
        GM_DELETE( m_pChannelNumberLabel[ i ] );
        GM_DELETE( m_pChannelResultLabel[ i ] );
    }
}

//-----------------------------------------------------------------------------------------------------
// CRYLBattleGroundPrevResultDlg::Show()
VOID CRYLBattleGroundPrevResultDlg::Show( CGMUIObject* Sender )
{
    CRYLGameData::Instance()->m_bSimpleMouse = TRUE;
    CRYLGameData::Instance()->m_dwKindCursor = CURSOR_NORMAL;
}

//-----------------------------------------------------------------------------------------------------
// CRYLBattleGroundPrevResultDlg::Hide()
VOID CRYLBattleGroundPrevResultDlg::Hide( CGMUIObject* Sender )
{
    CRYLGameData::Instance()->m_dwKindCursor = CURSOR_NORMAL;
    CRYLGameData::Instance()->m_bSimpleMouse = FALSE;
}

//-----------------------------------------------------------------------------------------------------
// CRYLBattleGroundPrevResultDlg::GMUIPaint()
VOID CRYLBattleGroundPrevResultDlg::GMUIPaint( INT nPosX, INT nPosY )
{
    CRYLDialog::GMUIPaint( nPosX, nPosY );
}

//-----------------------------------------------------------------------------------------------------
// CRYLBattleGroundPrevResultDlg::Update()
HRESULT CRYLBattleGroundPrevResultDlg::Update( BOOL& bClick, BOOL& bEdge )
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

    // update value label
    for( int i = 0 ; i < MAX_CHANNEL_NUMBER ; ++i )
    {
        string  strResult;
        
        char    strTemp[ 36 ] = { 0, };
        sprintf( strTemp, " %d : %d ", m_pPrevChannelResult[ i ].m_wScore[ 0 ], m_pPrevChannelResult[ i ].m_wScore[ 1 ] );

        strResult = strTemp;

        switch( m_pPrevChannelResult[ i ].m_cWinRace )
        {
        case 0:
            {
                strResult += CRYLStringTable::m_strString[ 2312 ];
            }
            break;

        case 1:
            {
                strResult += CRYLStringTable::m_strString[ 2313 ];
            }
            break;

        case 2:
            {
                strResult += CRYLStringTable::m_strString[ 2314 ];
            }
            break;
        }

        m_pChannelResultLabel[ i ]->SetCaption( const_cast<LPSTR>( strResult.c_str() ) );
    }

    return S_OK;
}


//-----------------------------------------------------------------------------------------------------
// CRYLBattleGroundPrevResultDlg::DlgClick()
VOID CRYLBattleGroundPrevResultDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y )
{
}

//-----------------------------------------------------------------------------------------------------
// CRYLBattleGroundPrevResultDlg::ChangeChannelInfo()
VOID CRYLBattleGroundPrevResultDlg::ChangePrevChannelResult( unsigned char ucChannelNum, PrevChannelResult* pChannelResult )
{
    if( ucChannelNum < MAX_CHANNEL_NUMBER )
    {
        m_pPrevChannelResult[ ucChannelNum ] = *pChannelResult;
    }
}
