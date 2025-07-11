//-----------------------------------------------------------------------------------------------------
// File Name: BattleGroundGameStatusDlg.cpp
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
#include "RYLLoadingScene.h"

#include "RYLPicture.h"
        
#include <WinInput.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>

#include "SoundMgr.h"

#include "BattleGroundGameStatusDlg.h"
#include "GMMemory.h"

//-----------------------------------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------------------------------
VOID StartBattleGroundEffect();

///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
// class CRYLBattleGroundGameStatusDlg
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
CRYLBattleGroundGameStatusDlg*  g_pBattleGroundGameStatus = NULL;
//-----------------------------------------------------------------------------------------------------
// constructor, destructor
//-----------------------------------------------------------------------------------------------------
CRYLBattleGroundGameStatusDlg::CRYLBattleGroundGameStatusDlg()
{
    CGMImageDib*	pImageDIB           = NULL;
    CRYLImage*		pImage	            = NULL;
	RECT			rt;

    //-- create number images ( 0 ~ 9 ) : 각 이미지를 8등분 --//
    for( int i = 0 ; i < 80 ; ++i )
    {
        CRYLSpriteList::Instance()->CreateGMImage( 32, 5, &pImageDIB );
        pImageDIB->ClearbyColor( 0x00000000 );
        if( i < 64 )
            SetRect( &rt, ( i / 8 ) * 32, 146 + ( i % 8 ) * 5, ( i / 8 + 1 ) * 32, 146 + ( i % 8 + 1 ) * 5 );
        else
            SetRect( &rt, ( i / 8  - 4 ) * 32, 92 + ( i % 8 ) * 5 , ( i / 8  - 3 ) * 32, 92 + ( i % 8 + 1 ) * 5 );
        pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BATTLEGROUND_GAME_STATUS_256x256 ) );
        m_pWhiteNumberImage[ i ] = CRYLImageManager::Instance()->CreateRYLImage();
        m_pWhiteNumberImage[ i ]->LoadFromImage( pImageDIB );
        GM_DELETE( pImageDIB );

        CRYLSpriteList::Instance()->CreateGMImage( 32, 5, &pImageDIB );
        pImageDIB->ClearbyColor( 0x00000000 );
        if( i < 64 )
            SetRect( &rt, ( i / 8 ) * 32, 192 + ( i % 8 ) * 5 , ( i / 8 + 1 ) * 32, 192 + ( i % 8 + 1 ) * 5 );
        else
            SetRect( &rt, ( i / 8 - 2 ) * 32, 92 + ( i % 8 ) * 5 , ( i / 8 - 1 ) * 32, 92 + ( i % 8 + 1 ) * 5 );
        pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BATTLEGROUND_GAME_STATUS_256x256 ) );
        m_pRedNumberImage[ i ] = CRYLImageManager::Instance()->CreateRYLImage();
        m_pRedNumberImage[ i ]->LoadFromImage( pImageDIB );
        GM_DELETE( pImageDIB );
    }
	
    //--  initialize dialog image and controls --//
    CRYLSpriteList::Instance()->CreateGMImage( 1024, 175, &pImageDIB ); // was 517, 89,
	pImageDIB->ClearbyColor( 0x00000000 );
    // Human face
//	SetRect( &rt,  0,  0,128, 68 );
//  pImageDIB->DrawRect( &rt,  0,  8, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BATTLEGROUND_GAME_STATUS_256x256 ) );
     // clock
    SetRect( &rt,  0, 0,1024, 175 ); // was  SetRect( &rt,  0, 68,128,146 );
    pImageDIB->DrawRect( &rt,0,  0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_LOHAN_256x256 ) );
    // Akan face
//	SetRect( &rt,128,  0,256, 68 );
//	pImageDIB->DrawRect( &rt,390,  8, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BATTLEGROUND_GAME_STATUS_256x256 ) );
	
	pImage = CRYLImageManager::Instance()->CreateRYLImage();
	pImage->LoadFromImage( pImageDIB );
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage );
	GM_DELETE( pImageDIB );

	AttachWindowTexture( ( CGMTexture*)pImage );

    // initialize dialog first position
    SetLeft( ( CGMUIScreen::GetInstance()->GetWidth() - GetWidth() ) / 2 - 520 );     // was / 2 - 260                            
	SetTop( 15 ); // was 15
    SetWidth( 1024 ); // was 516
	SetHeight( 175 ); // was 78

    SetFormType( RYLDLG_TYPE_NONE );
    SetDlgMoving( FALSE );

    m_ptHumanScore.x= GetLeft() + 242; // was 43
    m_ptHumanScore.y= GetTop() + 35; // was 33

    m_ptTime.x      = GetLeft() + 476; // was 212
    m_ptTime.y      = GetTop() + 90; // was 26

    m_ptAkhanScore.x= GetLeft() + 665; // was 370
    m_ptAkhanScore.y= GetTop() + 35; // was 33

    for(int i = 0 ; i < 3 ; ++i ) // was 3
    {
        m_nTargetHumanScoreNumber[ i ] = 0; // was 
        m_nTargetAkhanScoreNumber[ i ] = 0; // was 

        m_queTargetHumanScoreNumber[ i ].reserve( 9 ); // was 
        m_queTargetHumanScoreNumber[ i ].resize( 9 ); // was 
        m_queTargetAkhanScoreNumber[ i ].reserve( 9 ); // was 
        m_queTargetAkhanScoreNumber[ i ].resize( 9 ); // was 
    }

    m_nTargetTimeNumber[ 0 ] = 0;
    m_nTargetTimeNumber[ 1 ] = 0;

    m_queTargetTimeNumber[ 0 ].reserve( 9 );
    m_queTargetTimeNumber[ 0 ].resize( 9 );
    m_queTargetTimeNumber[ 1 ].reserve( 9 );
    m_queTargetTimeNumber[ 1 ].resize( 9 );

    g_pBattleGroundGameStatus = this;	

	InitCtrl();
}

CRYLBattleGroundGameStatusDlg::~CRYLBattleGroundGameStatusDlg()
{
    for( int i = 0 ; i < 80 ; ++i )
    {        
        CRYLImageManager::Instance()->DeleteRYLImage( m_pWhiteNumberImage[ i ] );
        CRYLImageManager::Instance()->DeleteRYLImage( m_pRedNumberImage[ i ] );
    }

    FinalCtrl();
}

//-----------------------------------------------------------------------------------------------------
// Member Functions
//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------
// CRYLBattleGroundGameStatusDlg::InitCtrl()
VOID CRYLBattleGroundGameStatusDlg::InitCtrl()
{
}

//-----------------------------------------------------------------------------------------------------
// CRYLBattleGroundGameStatusDlg::InitCtrl()
VOID CRYLBattleGroundGameStatusDlg::FinalCtrl()
{
}

//-----------------------------------------------------------------------------------------------------
// CRYLBattleGroundGameStatusDlg::Show()
VOID CRYLBattleGroundGameStatusDlg::Show( CGMUIObject* Sender )
{
}

//-----------------------------------------------------------------------------------------------------
// CRYLBattleGroundGameStatusDlg::Hide()
VOID CRYLBattleGroundGameStatusDlg::Hide( CGMUIObject* Sender )
{
    m_GameInfo.m_cMapType              = 0;
    m_GameInfo.m_cRemainMin            = 0;			
    m_GameInfo.m_wTargetScore          = 0;
    ZeroMemory( m_GameInfo.m_cCurrentCharNum, sizeof( unsigned char ) * GameInfo::MAX_RACE );
    ZeroMemory( m_GameInfo.m_wScore, sizeof( unsigned short ) * GameInfo::MAX_RACE );
}

//-----------------------------------------------------------------------------------------------------
// CRYLBattleGroundGameStatusDlg::GMUIPaint()
VOID CRYLBattleGroundGameStatusDlg::GMUIPaint( INT nPosX, INT nPosY )
{
    CRYLDialog::GMUIPaint( nPosX, nPosY );

    DrawNumber();

    if( CRYLLoadingScene::Instance()->GetFadeOutFinish() )
    {
        StartBattleGroundEffect();
    }
}

//-----------------------------------------------------------------------------------------------------
// CRYLBattleGroundGameStatusDlg::Update()
HRESULT	CRYLBattleGroundGameStatusDlg::Update( BOOL &bClick, BOOL &bEdge )
{
    return S_OK;
}

//-----------------------------------------------------------------------------------------------------
// CRYLBattleGroundGameStatusDlg::DlgClick()
VOID CRYLBattleGroundGameStatusDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y )
{
}

//-----------------------------------------------------------------------------------------------------
// CRYLBattleGroundGameStatusDlg::ChangeGameStatus()
VOID CRYLBattleGroundGameStatusDlg::ChangeGameStatus( GameInfo* pGameInfo )
{
    m_GameInfo = *pGameInfo;

    // save Human Socre
    // 100
    m_nTargetHumanScoreNumber[ 0 ] = static_cast<int>( m_GameInfo.m_wScore[ 0 ] / 100 );
    if( m_nTargetHumanScoreNumber[ 0 ] > 9 )
        m_nTargetHumanScoreNumber[ 0 ] = 0;
    // 10
    m_nTargetHumanScoreNumber[ 1 ] = static_cast<int>( m_GameInfo.m_wScore[ 0 ] / 10 );
    if( m_nTargetHumanScoreNumber[ 1 ] > 9 )
        m_nTargetHumanScoreNumber[ 1 ] %= 10;
    // 1
    m_nTargetHumanScoreNumber[ 2 ] = static_cast<int>( m_GameInfo.m_wScore[ 0 ] % 10 );

    // save Akhan Score
    // 100
    m_nTargetAkhanScoreNumber[ 0 ] = static_cast<int>( m_GameInfo.m_wScore[ 1 ] / 100 );
    if( m_nTargetAkhanScoreNumber[ 0 ] > 9 )
        m_nTargetAkhanScoreNumber[ 0 ] = 0;
    // 10
    m_nTargetAkhanScoreNumber[ 1 ] = static_cast<int>( m_GameInfo.m_wScore[ 1 ] / 10 );
    if( m_nTargetAkhanScoreNumber[ 1 ] > 9 )
        m_nTargetAkhanScoreNumber[ 1 ] %= 10;
    // 1
    m_nTargetAkhanScoreNumber[ 2 ] = static_cast<int>( m_GameInfo.m_wScore[ 1 ] % 10 );

    // save time
    // 10
    m_nTargetTimeNumber[ 0 ] = static_cast<int>( m_GameInfo.m_cRemainMin / 10 );
    if( m_nTargetTimeNumber[ 0 ] > 9 )
        m_nTargetTimeNumber[ 0 ] %= 10;

    // 1
    m_nTargetTimeNumber[ 1 ] = static_cast<int>( m_GameInfo.m_cRemainMin % 10 );

    if( m_GameInfo.m_cRemainMin <= 5 )
    {
        char    Temp[128] = { 0, };

        CStreamingSound* pPrevSound; 
        if( CRYLNetworkData::Instance()->m_dwPresentZone == SERVER_ID::FRAG_ZONE )
        {
	        sprintf(Temp, "%s\\sound\\am\\BG Client\\BG Client - Death match.ogg", g_ClientMain.m_strClientPath);
        }
        else if( CRYLNetworkData::Instance()->m_dwPresentZone == SERVER_ID::STATUE_ZONE )
        {
            sprintf(Temp, "%s\\sound\\am\\BG Client\\BG Client - Conquer.ogg", g_ClientMain.m_strClientPath);
        }
	    CSoundMgr::_GetInstance()->GetStreamingSound( pPrevSound, Temp ); 
        pPrevSound->Stop();

        CStreamingSound* pCurrentSound; 
	    sprintf(Temp, "%s\\sound\\am\\BG Client\\BG Client - 5 minutes remain.ogg", g_ClientMain.m_strClientPath);
	    CSoundMgr::_GetInstance()->GetStreamingSound( pCurrentSound, Temp ); 
        pCurrentSound->SetLooping( true );
        pCurrentSound->Play();
    }
}

//-----------------------------------------------------------------------------------------------------
// CRYLBattleGroundGameStatusDlg::DrawNumber()
VOID CRYLBattleGroundGameStatusDlg::DrawNumber()
{
    // 점수 각 자리 숫자 계산
    for( int i = 0 ; i < 3 ; ++i )
    {
        char cTemp = m_nTargetHumanScoreNumber[ i ];
        m_queTargetHumanScoreNumber[ i ].insert( ( std::string::size_type)0, 1, cTemp );
        m_queTargetHumanScoreNumber[ i ].resize( 8 );
        
        cTemp = m_nTargetAkhanScoreNumber[ i ];
        m_queTargetAkhanScoreNumber[ i ].insert( ( std::string::size_type)0, 1, cTemp );
        m_queTargetAkhanScoreNumber[ i ].resize( 8 );
    }

    int nSameImageCheck1 = 0;
    int nSameImageCheck2 = 0;

    for( int i = 0 ; i < 3 ; ++i )
    {
        for( int j = 0 ; j < 8 ; ++j )
        {
            // draw human score
            if( j == 0 || m_queTargetHumanScoreNumber[ i ][ j - 1 ] != m_queTargetHumanScoreNumber[ i ][ j ] )
            {
                nSameImageCheck1 = 0;
            }
            else if( m_queTargetHumanScoreNumber[ i ][ j - 1 ] == m_queTargetHumanScoreNumber[ i ][ j ] )
            {
                ++nSameImageCheck1;
            }
            m_pRedNumberImage[ m_queTargetHumanScoreNumber[ i ][ j ] * 8 + ( 7 - nSameImageCheck1 ) ]->Draw( m_ptHumanScore.x + i * 32, m_ptHumanScore.y + ( 7 - j ) * 5 );

            // draw akhan score
            if( j == 0 || m_queTargetAkhanScoreNumber[ i ][ j - 1 ] != m_queTargetAkhanScoreNumber[ i ][ j ] )
            {
                nSameImageCheck2 = 0;
            }
            else if( m_queTargetAkhanScoreNumber[ i ][ j - 1 ] == m_queTargetAkhanScoreNumber[ i ][ j ] )
            {
                ++nSameImageCheck2;
            }
            m_pRedNumberImage[ m_queTargetAkhanScoreNumber[ i ][ j ] * 8 + ( 7 - nSameImageCheck2 ) ]->Draw( m_ptAkhanScore.x + i * 32, m_ptAkhanScore.y + ( 7 - j ) * 5 );
        }
    }

    // 시간 각 자리 숫자 계산
    for( int i = 0 ; i < 2 ; ++i )
    {
        char cTemp = m_nTargetTimeNumber[ i ];
        m_queTargetTimeNumber[ i ].insert( ( std::string::size_type)0, 1, cTemp );
        m_queTargetTimeNumber[ i ].resize( 8 );
    }

    // draw time
    for( int i = 0 ; i < 2 ; ++i )
    {
        for( int j = 0 ; j < 8 ; ++j )
        {
            if( j == 0 || m_queTargetTimeNumber[ i ][ j - 1 ] != m_queTargetTimeNumber[ i ][ j ] )
            {
                nSameImageCheck1 = 0;
            }
            else if( m_queTargetTimeNumber[ i ][ j - 1 ] == m_queTargetTimeNumber[ i ][ j ] )
            {
                ++nSameImageCheck1;
            }

            
            m_pWhiteNumberImage[ m_queTargetTimeNumber[ i ][ j ] * 8 + ( 7 - nSameImageCheck1 ) ]->Draw( m_ptTime.x + i * 32, m_ptTime.y + ( 7 - j ) * 5 );
        }
    }
}

//-----------------------------------------------------------------------------------------------------
// CRYLBattleGroundGameStatusDlg::
 
//-----------------------------------------------------------------------------------------------------
// StartBattleGroundEffect()
VOID StartBattleGroundEffect()
{
    static CEffScript*  s_effBattleStart = NULL;

    if( s_effBattleStart )
    {
        CSceneManager::m_EffectManager.DeleteInterfaceScript( s_effBattleStart );
        s_effBattleStart = NULL;
    }

    CEffScript* effBattleStart = new CEffScript;
    effBattleStart->SetInterfaceSet(true);
    effBattleStart->GetScriptBinData("inter_duel.esf");	

    float fX = effBattleStart->m_SubScript[0].m_InterfacePos[0] / 800.0f;
	float fY = effBattleStart->m_SubScript[0].m_InterfacePos[1] / 600.0f;

	effBattleStart->m_SubScript[0].m_InterfacePos[0] = fX * CGMUIScreen::GetInstance()->GetWidth();
	effBattleStart->m_SubScript[0].m_InterfacePos[1] = fY * CGMUIScreen::GetInstance()->GetHeight();				

	fX = effBattleStart->m_SubScript[1].m_InterfacePos[0] / 800.0f;
	fY = effBattleStart->m_SubScript[1].m_InterfacePos[1] / 600.0f;

	effBattleStart->m_SubScript[1].m_InterfacePos[0] = fX * CGMUIScreen::GetInstance()->GetWidth();
	effBattleStart->m_SubScript[1].m_InterfacePos[1] = fY * CGMUIScreen::GetInstance()->GetHeight();				

    s_effBattleStart = CSceneManager::m_EffectManager.AddInterfaceScript( effBattleStart );

  //  CRYLGameScene*		            pScene          = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );	    
  //  CRYLBattleGroundStatusDlg*      pStatusDlg      = ( CRYLBattleGroundStatusDlg* )pScene->GetBattleGroundStatusDlg();

  //  if( !pStatusDlg->GetPlayerState() )
  //  {
  //      // "/hide"
  //      /*POS psPos = { 0.0f, 0.0f, 0.0f };
  //      SendPacket::CharAdmin(g_GameSession, ClientConstants::HideChar, CRYLGameData::Instance()->m_csStatus.m_Info.Name, 0, 0, 0, psPos, 0);*/
  //      // "neo"
  //      g_CharacterData.m_bFreelook = TRUE;
		//g_CharacterData.m_vecCameraPos = g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->m_vecPos;
		//g_CharacterData.m_fCameraDir = g_CharacterData.m_lstCharData[g_CharacterData.m_nFocusCharacter]->m_fDirection;
  //  }
}
