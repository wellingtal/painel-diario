//-----------------------------------------------------------------------------------------------------
// File Name: BattleGroundResultDlg.cpp
//
// Programmer: Zergra( Park Jongtae ) in GamaSoft corp.
//
// Date: 2004. 2. 18. Tuesday
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

#include "BattleGroundResultDlg.h"
#include "GMMemory.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
// class CRYLBattleGroundResultDlg
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------------------------------
// Constructor, Destructor
//-----------------------------------------------------------------------------------------------------
CRYLBattleGroundResultDlg::CRYLBattleGroundResultDlg()
{
    CGMImageDib*	pImageDIB = NULL;
	CRYLImage*		pImage	  = NULL;
	RECT			rt;

    //-- carete background image --//
    CRYLSpriteList::Instance()->CreateGMImage( CGMUIScreen::GetInstance()->GetWidth(), CGMUIScreen::GetInstance()->GetHeight(), &pImageDIB );
    pImageDIB->ClearbyColor( 0x80000000 );
    m_pBackGroundImage = CRYLImageManager::Instance()->CreateRYLImage();
    m_pBackGroundImage->LoadFromImage( pImageDIB );
    GM_DELETE( pImageDIB );
    
    //-- create number images ( 0 ~ 9 ) --//
    for( int i = 0 ; i < 10 ; ++i )
    {
        CRYLSpriteList::Instance()->CreateGMImage( 32, 41, &pImageDIB );
        pImageDIB->ClearbyColor( 0x00000000 );
        if( i < 8 )
            SetRect( &rt, i * 32, 146, i * 32 + 32, 187 );
        else
            SetRect( &rt, ( i - 4 ) * 32, 92, ( i - 4 ) * 32 + 32, 133 );
        pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BATTLEGROUND_GAME_STATUS_256x256 ) );
        m_pWhiteNumberImage[ i ] = CRYLImageManager::Instance()->CreateRYLImage();
        m_pWhiteNumberImage[ i ]->LoadFromImage( pImageDIB );
        GM_DELETE( pImageDIB );
    }

    //--  initialize dialog image and controls --//
    CRYLSpriteList::Instance()->CreateGMImage( 462,250, &pImageDIB );
	pImageDIB->ClearbyColor( 0x00000000 );
    // Final Score
	SetRect( &rt, 61, 2, 237, 25 );
    pImageDIB->DrawRect( &rt, 60, 13, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BATTLEGROUND_GAME_RESULT_256x128 ) );
    // Play Time
    SetRect( &rt, 1, 27, 237, 49 );
    pImageDIB->DrawRect( &rt, 0, 63, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BATTLEGROUND_GAME_RESULT_256x128 ) );
    // Merits
	SetRect( &rt, 137, 51, 237, 73 );
	pImageDIB->DrawRect( &rt, 136, 113, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BATTLEGROUND_GAME_RESULT_256x128 ) );
    // :
	SetRect( &rt, 49, 53, 58, 82 );
	pImageDIB->DrawRect( &rt, 359, 8, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BATTLEGROUND_GAME_RESULT_256x128 ) );
    // min
	SetRect( &rt, 2, 53, 46, 69 );
	pImageDIB->DrawRect( &rt, 359, 72, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BATTLEGROUND_GAME_RESULT_256x128 ) );
    // Kill
    SetRect( &rt, 69, 75, 245, 97 );
	pImageDIB->DrawRect( &rt, 59, 163, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BATTLEGROUND_GAME_RESULT_256x128 ) );
    // Died
    SetRect( &rt, 69, 99, 245, 121 );
	pImageDIB->DrawRect( &rt, 59, 213, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BATTLEGROUND_GAME_RESULT_256x128 ) );

    pImage = CRYLImageManager::Instance()->CreateRYLImage();
	pImage->LoadFromImage( pImageDIB );
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage );
	GM_DELETE( pImageDIB );

	AttachWindowTexture( ( CGMTexture*)pImage );

    // initialize dialog first position
    SetWidth( 462 );
	SetHeight( 140 );
    SetLeft( ( CGMUIScreen::GetInstance()->GetWidth() - GetWidth() ) / 2 - 231 );                                
    SetTop( ( CGMUIScreen::GetInstance()->GetHeight() - GetHeight() ) / 2 - 100 );

    m_ptFinalScore1.x   = GetLeft() + 261;
    m_ptFinalScore1.y   = GetTop();

    m_ptFinalScore2.x   = GetLeft() + 371;
    m_ptFinalScore2.y   = GetTop();

    m_ptPlayTime.x      = GetLeft() + 261;
    m_ptPlayTime.y      = GetTop()  + 50;

    m_ptMerits.x        = GetLeft() + 261;
    m_ptMerits.y        = GetTop()  + 100;

    m_ptKill.x          = GetLeft() + 261;
    m_ptKill.y          = GetTop()  + 154;

    m_ptKilled.x        = GetLeft() + 261;
    m_ptKilled.y        = GetTop()  + 207;

    SetFormType( RYLDLG_TYPE_NONE );
    SetDlgMoving( FALSE );

    InitCtrl();
}

CRYLBattleGroundResultDlg::~CRYLBattleGroundResultDlg()
{
    CRYLImageManager::Instance()->DeleteRYLImage( m_pBackGroundImage );
    for( int i = 0 ; i < 10 ; ++i )
    {        
        CRYLImageManager::Instance()->DeleteRYLImage( m_pWhiteNumberImage[ i ] );
    }

    FinalCtrl() ;
}

//-----------------------------------------------------------------------------------------------------
// Member Functions
//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------
// CRYLBattleGroundResultDlg::InitCtrl()
VOID CRYLBattleGroundResultDlg::InitCtrl()
{
}

//-----------------------------------------------------------------------------------------------------
// CRYLBattleGroundResultDlg::FinalCtrl()
VOID CRYLBattleGroundResultDlg::FinalCtrl()
{
}

//-----------------------------------------------------------------------------------------------------
// CRYLBattleGroundResultDlg::Show()
VOID CRYLBattleGroundResultDlg::Show( CGMUIObject* Sender )
{
}

//-----------------------------------------------------------------------------------------------------
// CRYLBattleGroundResultDlg::Hide()
VOID CRYLBattleGroundResultDlg::Hide( CGMUIObject* Sender )
{
}

//-----------------------------------------------------------------------------------------------------
// CRYLBattleGroundResultDlg::GMUIPaint()
VOID CRYLBattleGroundResultDlg::GMUIPaint( INT nPosX, INT nPosY )
{
    m_pBackGroundImage->Draw( 0, 0 );

    CRYLDialog::GMUIPaint( nPosX, nPosY );

    int n100Number, n10Number, n1Number;

    //-- Human Score --//
    n100Number = static_cast<int>( m_GameResult.m_wScore[ 0 ] / 100 );
    if( n100Number > 9 )
        n100Number = 0;
    n10Number = static_cast<int>( m_GameResult.m_wScore[ 0 ] / 10 );
    if( n10Number > 9 )
        n10Number %= 10;
    n1Number = static_cast<int>( m_GameResult.m_wScore[ 0 ] % 10 );
    DrawNumber( n100Number, n10Number, n1Number, m_ptFinalScore1 );

    //-- Akhan Score --//
    n100Number = static_cast<int>( m_GameResult.m_wScore[ 1 ] / 100 );
    if( n100Number > 9 )
        n100Number = 0;
    n10Number = static_cast<int>( m_GameResult.m_wScore[ 1 ] / 10 );
    if( n10Number > 9 )
        n10Number %= 10;
    n1Number = static_cast<int>( m_GameResult.m_wScore[ 1 ] % 10 );
    DrawNumber( n100Number, n10Number, n1Number, m_ptFinalScore2 );

    //-- Play Time --//
    n100Number = static_cast<int>( m_GameResult.m_cPlayMin / 100 );
    if( n100Number > 9 )
        n100Number = 0;
    n10Number = static_cast<int>( m_GameResult.m_cPlayMin / 10 );
    if( n10Number > 9 )
        n10Number %= 10;
    n1Number = static_cast<int>( m_GameResult.m_cPlayMin % 10 );
    DrawNumber( n100Number, n10Number, n1Number, m_ptPlayTime );

    //-- Merits --//
    n100Number = static_cast<int>( m_GameResult.m_wAward / 100 );
    if( n100Number > 9 )
        n100Number = 0;
    n10Number = static_cast<int>( m_GameResult.m_wAward / 10 );
    if( n10Number > 9 )
        n10Number %= 10;
    n1Number = static_cast<int>( m_GameResult.m_wAward % 10 );
    DrawNumber( n100Number, n10Number, n1Number, m_ptMerits );

    //-- Kill --//
    n100Number = static_cast<int>( m_GameResult.m_cKill / 100 );
    if( n100Number > 9 )
        n100Number = 0;
    n10Number = static_cast<int>( m_GameResult.m_cKill / 10 );
    if( n10Number > 9 )
        n10Number %= 10;
    n1Number = static_cast<int>( m_GameResult.m_cKill % 10 );
    DrawNumber( n100Number, n10Number, n1Number, m_ptKill );

    //-- Killed --//
    n100Number = static_cast<int>( m_GameResult.m_cKilled / 100 );
    if( n100Number > 9 )
        n100Number = 0;
    n10Number = static_cast<int>( m_GameResult.m_cKilled / 10 );
    if( n10Number > 9 )
        n10Number %= 10;
    n1Number = static_cast<int>( m_GameResult.m_cKilled % 10 );
    DrawNumber( n100Number, n10Number, n1Number, m_ptKilled );
}

//-----------------------------------------------------------------------------------------------------
// CRYLBattleGroundResultDlg::Update()
HRESULT CRYLBattleGroundResultDlg::Update( BOOL& bClick, BOOL& bEdge )
{
    return S_OK;
}

//-----------------------------------------------------------------------------------------------------
// CRYLBattleGroundResultDlg::DlgClick()
VOID CRYLBattleGroundResultDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y )
{
}

//-----------------------------------------------------------------------------------------------------
// CRYLBattleGroundResultDlg::DrawNumber()
VOID CRYLBattleGroundResultDlg::DrawNumber( INT n100Number, INT n10Number, INT n1Number,  POINT ptPos )
{
    m_pWhiteNumberImage[ n100Number ]->Draw( ptPos.x, ptPos.y );
    m_pWhiteNumberImage[ n10Number ]->Draw( ptPos.x + 32, ptPos.y );
    m_pWhiteNumberImage[ n1Number ]->Draw( ptPos.x + 64, ptPos.y );
}
