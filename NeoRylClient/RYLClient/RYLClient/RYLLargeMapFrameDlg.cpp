#include <winsock2.h>

#include "GMUIObject.h"
#include "GMImageDib.h"
#include "WinInput.h"
#include "BaseDataDefine.h"
#include "BaseGraphicsLayer.h"

#include <Network/ClientSocket/Send/SendAttack.h>
#include <Network/ClientSocket/Send/SendCommunity.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>

#include <RenderOption.h>

#include "RYLLargeMapFrameDlg.h"
#include "RYLTabButton.h"
#include "RYLImageManager.h"
#include "RYLImage.h"
#include "RYLSpriteEx.h"
#include "RYLSprite.h"
#include "RYLSpriteList.h"
#include "RYLStringTable.h"
#include "RYLMessageBox.h"

#include "../GUITextEdit.h"
#include "../CharStatus.h"

#include "RYLClientMain.h"
#include "RYLGameData.h"
#include "RYLNetworkData.h"
#include "RYLGameScene.h"
#include "RYLSceneManager.h"

#include "RYLMiniMapDlg.h"
#include "RYLLargeMapDlg.h"
#include "RYLLargeMapMobDistributeDlg.h"
#include "RYLLargeMapCampDistributeDlg.h"

#include "RYLRaceBase.h"
#include "GMMemory.h"

#include <Creature/Monster/MonsterMgr.h>

CRYLLargeMapFrameDlg*	g_pLargeMapDlg = NULL ;

CRYLLargeMapFrameDlg::CRYLLargeMapFrameDlg() 
{
	m_pTabButton	= NULL ;
	m_pLargeMapPanel = NULL ;
	m_pLargeMapCampDistributePanel = NULL ;

//	m_pLargeMapMobDistributePanel = NULL;   ///////////////

	m_pMyPosSprite	= NULL ;

	for(int i = 0; i < 4; ++i)
		m_pQuestSprite[i]	= NULL ;

	m_pPartySprite	= NULL ;

	m_lpQuestTarget	= NULL ;

	for ( int i = 0 ; i < RESPAWNPOS_MAX ; i ++ )
	{
		m_pSpawnButton[ i ] = NULL ;
	}

	m_bPortal		= FALSE;
	m_bClearPortal	= FALSE;
	m_bStatue		= TRUE;


	RECT rt ;
	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;

	CRYLSpriteList::Instance()->CreateGMImage( 512, 34, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 0, 50, 13 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	SetRect( &rt, 50, 0, 150, 13 ) ;
	pImageDIB->DrawRect( &rt, 50,  0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;			
	pImageDIB->DrawRect( &rt, 150, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	pImageDIB->DrawRect( &rt, 250, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	pImageDIB->DrawRect( &rt, 350, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	SetRect( &rt, 194, 0, 256, 13 ) ;
	pImageDIB->DrawRect( &rt, 450, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;	

	pImageDIB->DrawFrameRect( 40, 13, 472, 21, 0xc0000000 ) ;
	pImageDIB->DrawFrameRect( 0, 13, 1, 21, 0xff5b4c35 ) ;
	pImageDIB->DrawFrameRect( 1, 13, 1, 21, 0xffe1c995 ) ;
	pImageDIB->DrawFrameRect( 42, 33, 468, 1, 0xff9f835b ) ;
	pImageDIB->DrawFrameRect( 510, 13, 1, 21, 0xffe1c995 ) ;
	pImageDIB->DrawFrameRect( 511, 13, 1, 21, 0xff5b4c35 ) ;
	
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	AttachCaptionTexture( ( CGMTexture*)pImage ) ;
	GM_DELETE( pImageDIB ) ;

	SetWidth( 512 ) ;
	SetHeight( 546 ) ;
	SetLeft( ( CGMUIScreen::GetInstance()->GetWidth() - GetWidth() ) / 2 ) ;
	SetTop( ( CGMUIScreen::GetInstance()->GetHeight() - GetHeight() ) / 2 ) ;

	SetFormType( RYLDLG_FRAME_TYPE_CLOSEBUTTON ) ;
	InitCtrl() ;
	g_pLargeMapDlg = this ;
}

CRYLLargeMapFrameDlg::~CRYLLargeMapFrameDlg() 
{
	FinalCtrl() ;
}

VOID	CRYLLargeMapFrameDlg::InitCtrl() 
{
	m_pTabButton = new CRYLTabButton( this ) ;
	m_pTabButton->SetLeft( 1 ) ;
	m_pTabButton->SetTop( 13 ) ;
	m_pTabButton->SetWidth( 511 ) ;
	m_pTabButton->SetHeight( 23 ) ;
	m_pTabButton->SetInstance( RYLLARGEMAPFRAMEDLG::TAB_BUTTON ) ;
	m_pTabButton->OnClickEvent = DlgClick ;
	m_pTabButton->AttachTabButton( CRYLStringTable::m_strString[ 2242 ], 2 ) ;
/*
	if (true == CRYLNetworkData::Instance()->UseContents(GameRYL::CAMP))
	{
		m_pTabButton->AttachTabButton( CRYLStringTable::m_strString[ 2527 ], 5 ) ;
	}

	m_pTabButton->AttachTabButton( CRYLStringTable::m_strString[ 2244 ], 5 ) ;
/*/
	AttachGMUIControl( m_pTabButton ) ;

	LPTABBUTTONINFO		lpTabButton = NULL ;
	lpTabButton = m_pTabButton->GetTabButtonInfo( 0 ) ;
	lpTabButton->m_pTabButton->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 2243 ], 1851, TOOLTIP_TAB ) ;

//	lpTabButton = m_pTabButton->GetTabButtonInfo( 2 ) ; //////////////////
//	lpTabButton->m_pTabButton->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 5100 ], 5100, TOOLTIP_TAB ) ; //////////////////

	if ( !m_pLargeMapPanel )
		m_pLargeMapPanel = new CRYLLargeMapDlg( this ) ;
	m_pLargeMapPanel->SetLeft( 0 ) ;
	m_pLargeMapPanel->SetTop( 34 ) ;
	m_pLargeMapPanel->SetWidth( 512 ) ;
	m_pLargeMapPanel->SetHeight( 512 ) ;
	m_pLargeMapPanel->SetVisible( TRUE ) ;
	AttachGMUIControl( m_pLargeMapPanel ) ;

	if ( !m_pLargeMapCampDistributePanel )
		m_pLargeMapCampDistributePanel = new CRYLCampDistributePanel( this );
	m_pLargeMapCampDistributePanel->SetLeft( 0 ) ;
	m_pLargeMapCampDistributePanel->SetTop( 34 ) ;
	m_pLargeMapCampDistributePanel->SetWidth( 512 ) ;
	m_pLargeMapCampDistributePanel->SetHeight( 512 ) ;
	m_pLargeMapCampDistributePanel->SetVisible( FALSE ) ;
	AttachGMUIControl( m_pLargeMapCampDistributePanel ) ;


/*
	if ( !m_pLargeMapMobDistributePanel )
		m_pLargeMapMobDistributePanel = new CRYLMobDistributeDlg( this );
	m_pLargeMapMobDistributePanel->SetLeft( 0 ) ;
	m_pLargeMapMobDistributePanel->SetTop( 34 ) ;
	m_pLargeMapMobDistributePanel->SetWidth( 512 ) ;
	m_pLargeMapMobDistributePanel->SetHeight( 512 ) ;
	m_pLargeMapMobDistributePanel->SetVisible( FALSE ) ;
	AttachGMUIControl( m_pLargeMapMobDistributePanel ) ;
*/

	CRYLGameScene*		 pScene      = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
	CRYLMIniMapDlg*		 pDlg		 = ( CRYLMIniMapDlg* )pScene->GetMiniMapDlg() ;

	m_pMyPosSprite = new CRYLSpriteEx( m_pGMUIParentDlg ) ;
	m_pMyPosSprite->m_pSprite = new CRYLSprite ;
	m_pMyPosSprite->m_pTexture = new CTexture ;
	m_pMyPosSprite->SetLeft( 0 ) ;
	m_pMyPosSprite->SetTop( 0 ) ;
	m_pMyPosSprite->SetWidth( 26 ) ;
	m_pMyPosSprite->SetHeight( 56 ) ;
	m_pMyPosSprite->m_pSprite->Create( 0, 0, 126, 17, 152, 73, pDlg->GetMapTexture() ) ;
	m_pMyPosSprite->SetVisible( FALSE ) ;
	AttachGMUIControl( m_pMyPosSprite ) ;


	for(int i = 0; i < 4; ++i)
	{
		m_pQuestSprite[i] = new CRYLSpriteEx( m_pGMUIParentDlg ) ;
		m_pQuestSprite[i]->m_pSprite = new CRYLSprite ;
		m_pQuestSprite[i]->m_pTexture = new CTexture ;
		m_pQuestSprite[i]->SetLeft( 0 ) ;
		m_pQuestSprite[i]->SetTop( 0 ) ;
		m_pQuestSprite[i]->SetWidth( 25 ) ;
		m_pQuestSprite[i]->SetHeight( 25 ) ;
		m_pQuestSprite[i]->SetVisible( FALSE ) ;

		if(i == 0)
			m_pQuestSprite[0]->m_pSprite->Create( 0, 0, 152, 35, 176, 59, pDlg->GetMapTexture() ) ;
		else if(i == 1)
			m_pQuestSprite[1]->m_pSprite->Create( 0, 0, 177, 35, 201, 59, pDlg->GetMapTexture() ) ;
		else if(i == 2)
			m_pQuestSprite[2]->m_pSprite->Create( 0, 0, 177, 10, 201, 34, pDlg->GetMapTexture() ) ;
		else if(i == 3)
			m_pQuestSprite[3]->m_pSprite->Create( 0, 0, 152, 60, 176, 84, pDlg->GetMapTexture() ) ;

		AttachGMUIControl( m_pQuestSprite[i] ) ;
	}


	m_pPartySprite = new CRYLSprite;		// 파티원 표시 : 8
	m_pPartySprite->Create(0, 0, 178, 62, 205, 85, pDlg->GetMapTexture() ) ;

	SetStatue(TRUE);
}

HRESULT		CRYLLargeMapFrameDlg::Update( BOOL &bClick, BOOL &bEdge ) 
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
	
	return S_OK ;
}

HRESULT		CRYLLargeMapFrameDlg::Render( LPDIRECT3DDEVICE8 lpD3DDevice )  
{
	return S_OK ;
}

HRESULT		CRYLLargeMapFrameDlg::FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) 
{
	return S_OK ;
}

VOID	CRYLLargeMapFrameDlg::FinalCtrl() 
{
	GM_DELETE( m_pTabButton ) ;
	GM_DELETE( m_pLargeMapPanel );
	GM_DELETE( m_pLargeMapCampDistributePanel ) ;
//	GM_DELETE( m_pLargeMapMobDistributePanel );   //////////////

	GM_DELETE( m_pMyPosSprite ) ;

	for(int i = 0; i < 4; ++i)
		GM_DELETE( m_pQuestSprite[i] ) ;

	GM_DELETE( m_pPartySprite ) ;

	for ( int i = 0 ; i < RESPAWNPOS_MAX ; i ++ )
	{
		if ( m_pSpawnButton[ i ] )
		{
			GM_DELETE( m_pSpawnButton[ i ] ) ;
		}
	}

	// 석상 정보 삭제
	StetueMap::iterator begin	= m_mapStatue.begin();
	StetueMap::iterator end		= m_mapStatue.end();

	for(StetueMap::iterator itr = begin; itr!=end; itr++)
	{
		if(itr->second)
		{ 
			delete itr->second;
		}
	}

	m_mapStatue.clear();
}

VOID	CRYLLargeMapFrameDlg::Show( CGMUIObject* Sender ) 
{
    if ( CRYLNetworkData::Instance()->m_dwPresentZone == SERVER_ID::BATTLE_SERVER || 
		CRYLNetworkData::Instance()->m_dwPresentZone == SERVER_ID::PRISON )
    {
        SetVisible( FALSE );		
    }	

	if(/*SERVER_ID::STONE_WAR1*/SERVER_ID::CAPITAL <= CRYLNetworkData::Instance()->m_dwPresentZone && CRYLNetworkData::Instance()->m_dwPresentZone <= SERVER_ID::STONE_WAR3)
	{
		SetTabButtonName(TRUE);
	}
	else
	{		
		SetTabButtonName(FALSE);		
	}

	if(CRYLNetworkData::Instance()->m_dwPresentZone == SERVER_ID::CAPITAL)
	{
		m_pTabButton->SetTabVisible(1, TRUE);
	}
	else
	{
		m_pTabButton->SetTabVisible(1, FALSE);
	}

	g_pLargeMapDlg->SetFocusPanel( 0 ) ;
}

VOID	CRYLLargeMapFrameDlg::Hide( CGMUIObject* Sender ) 
{
	if (GetPortal())
	{
		CRYLGameData* pGame = CRYLGameData::Instance();

		if ( pGame->GetAutoRun() )
			pGame->SetAutoRun( TRUE ) ;

		pGame->SetEnableMove( TRUE ) ;	
		SetPortal(FALSE);

		if (!GetClearPortal())
		{			
			ClearRespawnButton() ;			
		}
		else
		{
			SetClearPortal(FALSE);
			SetDisableRespawn();
		}
	}
}

VOID	CRYLLargeMapFrameDlg::SetTabButtonName(BOOL bStatue)
{
/*
	if (true == CRYLNetworkData::Instance()->UseContents(GameRYL::CAMP))
	{
		if(!bStatue)
		{
			m_pTabButton->RestoreTabButton( 1, CRYLStringTable::m_strString[ 2527 ], 5 );
		}
		else
		{
			m_pTabButton->RestoreTabButton( 1, CRYLStringTable::m_strString[ 3590 ], 5 ) ;
		}
	}
*/
}

VOID	CRYLLargeMapFrameDlg::GMUIPaint( INT xx, INT yy ) //stone war map information hayzohar
{
	LPDIRECT3DDEVICE8   lpD3DDevice = BaseGraphicsLayer::GetDevice() ;

	CRYLDialog::GMUIPaint( xx, yy ) ;

	CRYLGameData*	    pGame		= CRYLGameData::Instance() ;
	CRYLNetworkData*	pNetwork	= CRYLNetworkData::Instance() ;

	INT left = GetLeft() ;
	INT top  = GetTop() + 34 ;

	if (NULL != m_lpQuestTarget)
	{
		PhaseNode* lpPhase = m_lpQuestTarget->m_lpPhaseScript;
		if (NULL != lpPhase)
		{
			if (lpPhase->m_dwZoneID == pNetwork->m_dwPresentZone)
			{
				int nType = 0;
				long nPosX, nPosY;
				for (unsigned long i = 0; i < lpPhase->m_lstTargetPos.size(); ++i)
				{
					nType = lpPhase->m_lstTargetPos[i].m_iType;
					nPosX = static_cast<long>(lpPhase->m_lstTargetPos[i].m_dwPosX) ;
					nPosY = static_cast<long>(lpPhase->m_lstTargetPos[i].m_dwPosY) ;

					switch ( pNetwork->m_dwPresentZone)
					{
						case SERVER_ID::ZONE1:
						//	m_pQuestSprite[nType]->m_pSprite->m_nPutX = (long)( nPosX / 3465.0f * 512) - 48;
						//	m_pQuestSprite[nType]->m_pSprite->m_nPutY = (long)( 512 - nPosY / 3465.0f * 512) - 46 ;

						//	m_pQuestSprite[nType]->m_pSprite->m_nPutX += left - 12 ;
						//	m_pQuestSprite[nType]->m_pSprite->m_nPutY += top - 12 ;

							m_pQuestSprite[nType]->m_pSprite->m_nPutX = (long)( (nPosX - 315) * 512 / 3465.0f ) ;
							m_pQuestSprite[nType]->m_pSprite->m_nPutY = (long)( 512 - (nPosY - 315) * 512 / 3465.0f ) ;

							m_pQuestSprite[nType]->m_pSprite->m_nPutX += left - 12 ;
							m_pQuestSprite[nType]->m_pSprite->m_nPutY += top - 12 ;
							break;

						case SERVER_ID::ZONE2:
							m_pQuestSprite[nType]->m_pSprite->m_nPutX = (long)(nPosX / 3422.0f * 512) - 52;
							m_pQuestSprite[nType]->m_pSprite->m_nPutY = (long)( 512 - nPosY / 3438.0f * 512) + 52;

							m_pQuestSprite[nType]->m_pSprite->m_nPutX += left - 12 ;
							m_pQuestSprite[nType]->m_pSprite->m_nPutY += top - 12 ;
							break;
/*
						case SERVER_ID::ZONE3:
							m_pQuestSprite[nType]->m_pSprite->m_nPutX = (long)( (nPosX - 400) * 512 / 2970 ) ;
							m_pQuestSprite[nType]->m_pSprite->m_nPutY = (long)( 512 - (nPosY - 400) * 512 / 2970 ) ;

							m_pQuestSprite[nType]->m_pSprite->m_nPutX += left - 12 ;
							m_pQuestSprite[nType]->m_pSprite->m_nPutY += top - 12 ;
							break;
*/

						case SERVER_ID::ZONE4:
							m_pQuestSprite[nType]->m_pSprite->m_nPutX = (long)( nPosX  / 2463.0f * 512) - 68;
							m_pQuestSprite[nType]->m_pSprite->m_nPutY = (long)( 512 - nPosY / 2383.0f * 512) + 83;

							m_pQuestSprite[nType]->m_pSprite->m_nPutX += left - 12 ;
							m_pQuestSprite[nType]->m_pSprite->m_nPutY += top - 12 ;
							break;

						case SERVER_ID::ZONE5:
							m_pQuestSprite[nType]->m_pSprite->m_nPutX = (long)( nPosX / 3120.0f * 512) - 53;
							m_pQuestSprite[nType]->m_pSprite->m_nPutY = (long)( 512 - nPosY / 3104.0f * 512) + 55;

							m_pQuestSprite[nType]->m_pSprite->m_nPutX += left - 12 ;
							m_pQuestSprite[nType]->m_pSprite->m_nPutY += top - 12 ;
							break;

						case SERVER_ID::ZONE9:
							m_pQuestSprite[nType]->m_pSprite->m_nPutX = (long)( (nPosX - 630) * 512 / 3150.0f ) ;
							m_pQuestSprite[nType]->m_pSprite->m_nPutY = (long)( 512 - (nPosY - 630) * 512 / 3150.0f ) ;

							m_pQuestSprite[nType]->m_pSprite->m_nPutX += left - 12 ;
							m_pQuestSprite[nType]->m_pSprite->m_nPutY += top - 12 ;
							break;

						case SERVER_ID::CAPITAL:
							m_pQuestSprite[nType]->m_pSprite->m_nPutX = (long)( (nPosX - 315) * 512 / 3465.0f ) ;
							m_pQuestSprite[nType]->m_pSprite->m_nPutY = (long)( 512 - (nPosY - 315) * 512 / 3465.0f ) ;

							m_pQuestSprite[nType]->m_pSprite->m_nPutX += left - 12 ;
							m_pQuestSprite[nType]->m_pSprite->m_nPutY += top - 12 ;
							break;

						case SERVER_ID::FRAG_ZONE:
							m_pQuestSprite[nType]->m_pSprite->m_nPutX = 138 + (long)((nPosX - 1000) * 1.09f);
							m_pQuestSprite[nType]->m_pSprite->m_nPutY = 372 + (long)((1000 - nPosY) * 1.09f);

							m_pQuestSprite[nType]->m_pSprite->m_nPutX += left - 12 ;
							m_pQuestSprite[nType]->m_pSprite->m_nPutY += top - 12 ;
							break;

						case SERVER_ID::ZONE12:
							m_pQuestSprite[nType]->m_pSprite->m_nPutX = (long)( (nPosX - 315) * 512 / 3465.0f ) ;
							m_pQuestSprite[nType]->m_pSprite->m_nPutY = (long)( 512 - (nPosY - 315) * 512 / 3465.0f ) ;

							m_pQuestSprite[nType]->m_pSprite->m_nPutX += left - 12 ;
							m_pQuestSprite[nType]->m_pSprite->m_nPutY += top - 12 ;
							break;

						case SERVER_ID::STATUE_ZONE:
							m_pQuestSprite[nType]->m_pSprite->m_nPutX =  92 + (long)((nPosX - 1000) * 1.61f);
							m_pQuestSprite[nType]->m_pSprite->m_nPutY = 421 + (long)((1000 - nPosY) * 1.61f);

							m_pQuestSprite[nType]->m_pSprite->m_nPutX += left - 12;
							m_pQuestSprite[nType]->m_pSprite->m_nPutY += top - 12 ;
							break;
					}

					m_pQuestSprite[nType]->m_pSprite->Render(lpD3DDevice, 0xFF, 0, 0 ) ;
				}
			}
		}
	}

	for ( INT i = 0 ; i < pGame->m_csStatus.m_PartyInfo.m_cMemberNum ; ++i )
	{
		if ( pGame->m_csStatus.m_PartyInfo.MemberCID[i] )
		{
			if ( pGame->m_csStatus.m_PartyInfo.MemberCID[i] == pNetwork->m_dwMyChrID ) 
			{
				continue;
			}

			SERVER_ID serverID;
			serverID.dwID = pGame->m_csStatus.m_PartyInfo.ServerID[i];

			if (serverID.GetZone() == pNetwork->m_dwPresentZone &&
				serverID.GetChannel() == pNetwork->m_dwChannelID)
			{
				if (true == pGame->m_csStatus.m_aryPartyExtraInfo[i].m_EnchantInfo.GetFlag(Skill::SpellID::Hide))
				{
					continue;   // Hide 캐릭터 표시 안함
				}

				INT nPosX, nPosY ;
				RECT rcRect ;

				switch (pNetwork->m_dwPresentZone)
				{
					case SERVER_ID::ZONE1:
					//	nPosX = (long)((pGame->m_csStatus.m_PartyInfo.m_Position[i].fPointX ) / 3465.0f * 512) - 48;
					//	nPosY = (long)( 512 - pGame->m_csStatus.m_PartyInfo.m_Position[i].fPointZ / 3465.0f * 512) - 46;
						nPosX = (long)( (long)(pGame->m_csStatus.m_PartyInfo.m_Position[i].fPointX - 315) * 512 / 3465.0f ) ;
						nPosY = (long)( 512 - (long)(pGame->m_csStatus.m_PartyInfo.m_Position[i].fPointZ - 315) * 512 / 3465.0f ) ;
						break;

					case SERVER_ID::ZONE2:
						nPosX = (long)((pGame->m_csStatus.m_PartyInfo.m_Position[i].fPointX ) / 3422.0f * 512) - 52;
						nPosY = (long)( 512 - pGame->m_csStatus.m_PartyInfo.m_Position[i].fPointZ / 3438.0f * 512) + 52;
						break;

					case SERVER_ID::ZONE3:
					case SERVER_ID::STONE_WAR1:
					case SERVER_ID::STONE_WAR2:
					case SERVER_ID::STONE_WAR3:
						nPosX = (long)((pGame->m_csStatus.m_PartyInfo.m_Position[i].fPointX ) / 1116.0f * 512) - 395;
						nPosY = (long)( 512 - pGame->m_csStatus.m_PartyInfo.m_Position[i].fPointZ / 1111.0f * 512) + 405;
						break;

					case SERVER_ID::ZONE4:
						nPosX = (long)((pGame->m_csStatus.m_PartyInfo.m_Position[i].fPointX ) / 2463.0f * 512) - 68;
						nPosY = (long)( 512 - pGame->m_csStatus.m_PartyInfo.m_Position[i].fPointZ / 2383.0f * 512) + 83;
						break;

					case SERVER_ID::ZONE5:
						nPosX = (long)((pGame->m_csStatus.m_PartyInfo.m_Position[i].fPointX ) / 3120.0f * 512) - 53;
						nPosY = (long)( 512 - pGame->m_csStatus.m_PartyInfo.m_Position[i].fPointZ / 3104.0f * 512) + 55;
						break;

					case SERVER_ID::ZONE9:
						nPosX = (long)( (long)(pGame->m_csStatus.m_PartyInfo.m_Position[i].fPointX - 630) * 512 / 3150.0f ) ;
						nPosY = (long)( 512 - (long)(pGame->m_csStatus.m_PartyInfo.m_Position[i].fPointZ - 630) * 512 / 3150.0f ) ;
						break;
	
					case SERVER_ID::CAPITAL:
						nPosX = (long)( (long)(pGame->m_csStatus.m_PartyInfo.m_Position[i].fPointX - 315) * 512 / 3465.0f ) ;
						nPosY = (long)( 512 - (long)(pGame->m_csStatus.m_PartyInfo.m_Position[i].fPointZ - 315) * 512 / 3465.0f ) ;
						break;

					case SERVER_ID::FRAG_ZONE:
						nPosX = 138 + (long)(((long)( pGame->m_csStatus.m_PartyInfo.m_Position[i].fPointX ) - 1000) * 1.0923f);
						nPosY = 372 + (long)((1000 - (long)( pGame->m_csStatus.m_PartyInfo.m_Position[i].fPointZ )) * 1.09f);
						break;

					case SERVER_ID::ZONE12:
						nPosX = (long)( (long)(pGame->m_csStatus.m_PartyInfo.m_Position[i].fPointX - 315) * 512 / 3465.0f ) ;
						nPosY = (long)( 512 - (long)(pGame->m_csStatus.m_PartyInfo.m_Position[i].fPointZ - 315) * 512 / 3465.0f ) ;
						break;

					case SERVER_ID::STATUE_ZONE:
						nPosX =  92 + (long)(((long)( pGame->m_csStatus.m_PartyInfo.m_Position[i].fPointX ) - 1000) * 1.611f);
						nPosY = 421 + (long)((1000 - (long)( pGame->m_csStatus.m_PartyInfo.m_Position[i].fPointZ )) * 1.61f);
						break;
				}

				nPosX += left ;
				nPosY += top ;

				m_pPartySprite->m_nPutX = nPosX - 13;
				m_pPartySprite->m_nPutY = nPosY - 11;

				if ( left < m_pPartySprite->m_nPutX )
				{
					m_pPartySprite->Render( lpD3DDevice, 0x80, 0, 0 );

					// 외곽선 그리기
					SetRect(&rcRect, m_pPartySprite->m_nPutX - 1, m_pPartySprite->m_nPutY - 19, 
						m_pPartySprite->m_nPutX + 149, m_pPartySprite->m_nPutY - 1 );

					g_TextEdit.DrawText( pGame->m_csStatus.GetPartyName( pGame->m_csStatus.m_PartyInfo.MemberCID[ i ] ), 
						&rcRect, DT_LEFT | DT_VCENTER, D3DCOLOR_RGBA(0, 0, 0, 255));

					rcRect.left += 2;	rcRect.right += 2;
					g_TextEdit.DrawText( pGame->m_csStatus.GetPartyName( pGame->m_csStatus.m_PartyInfo.MemberCID[ i ] ), 
						&rcRect, DT_LEFT | DT_VCENTER, D3DCOLOR_RGBA(0, 0, 0, 255));

					rcRect.top += 2;	rcRect.bottom += 2;
					g_TextEdit.DrawText( pGame->m_csStatus.GetPartyName( pGame->m_csStatus.m_PartyInfo.MemberCID[ i ] ), 
						&rcRect, DT_LEFT | DT_VCENTER, D3DCOLOR_RGBA(0, 0, 0, 255));

					rcRect.left -= 2;	rcRect.right -= 2;
					g_TextEdit.DrawText( pGame->m_csStatus.GetPartyName( pGame->m_csStatus.m_PartyInfo.MemberCID[ i ] ), 
						&rcRect, DT_LEFT | DT_VCENTER, D3DCOLOR_RGBA(0, 0, 0, 255));

					// 실제 파티원 이름 그리기
					++rcRect.left;
					++rcRect.right;
					--rcRect.top;
					--rcRect.bottom;
					g_TextEdit.DrawText( pGame->m_csStatus.GetPartyName( pGame->m_csStatus.m_PartyInfo.MemberCID[ i ] ), 
						&rcRect, DT_LEFT | DT_VCENTER, D3DCOLOR_RGBA(0, 213, 255, 255));
				}
			}
		}
	}

	RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
	if ( !pSelfCreature )
		return;

	switch ( pNetwork->m_dwPresentZone)
	{
		case SERVER_ID::ZONE1:
		//	m_pMyPosSprite->m_pSprite->m_nPutX = (long)((pSelfCreature->GetPosition()->x / 100.0f) / 3465.0f * 512) - 48;
		//	m_pMyPosSprite->m_pSprite->m_nPutY = (long)( 512 - (pSelfCreature->GetPosition()->z / 100.0f) / 3465.0f * 512) - 46 ;
			m_pMyPosSprite->m_pSprite->m_nPutX = (long)( (long)(pSelfCreature->GetPosition()->x / 100.0f - 315) * 512 / 3465.0f ) ;
			m_pMyPosSprite->m_pSprite->m_nPutY = (long)( 512 - (long)(pSelfCreature->GetPosition()->z / 100.0f - 315) * 512 / 3465.0f ) ;
			break;

		case SERVER_ID::ZONE2:
			m_pMyPosSprite->m_pSprite->m_nPutX = (long)((pSelfCreature->GetPosition()->x / 100.0f) / 3422.0f * 512) - 52;
			m_pMyPosSprite->m_pSprite->m_nPutY = (long)( 512 - (pSelfCreature->GetPosition()->z / 100.0f) / 3438.0f * 512) + 52;
			break;

		
		case SERVER_ID::ZONE3:
		case SERVER_ID::STONE_WAR1:
		case SERVER_ID::STONE_WAR2:
		case SERVER_ID::STONE_WAR3:
			m_pMyPosSprite->m_pSprite->m_nPutX = (long)((pSelfCreature->GetPosition()->x / 100.0f) / 1116.0f * 512) - 395;
			m_pMyPosSprite->m_pSprite->m_nPutY = (long)( 512 - (pSelfCreature->GetPosition()->z / 100.0f) / 1111.0f * 512) + 405;
			break;

		case SERVER_ID::ZONE4:
			m_pMyPosSprite->m_pSprite->m_nPutX = (long)((pSelfCreature->GetPosition()->x / 100.0f) / 2463.0f * 512) - 68;
			m_pMyPosSprite->m_pSprite->m_nPutY = (long)( 512 - (pSelfCreature->GetPosition()->z / 100.0f) / 2383.0f * 512) + 83;
			break;

		case SERVER_ID::ZONE5:
			m_pMyPosSprite->m_pSprite->m_nPutX = (long)((pSelfCreature->GetPosition()->x / 100.0f) / 3120.0f * 512) - 53;
			m_pMyPosSprite->m_pSprite->m_nPutY = (long)( 512 - (pSelfCreature->GetPosition()->z / 100.0f) / 3104.0f * 512) + 55;
			break;

		case SERVER_ID::ZONE9:
			// 계산공식은 ZONE9번은 미니맵에 보이는 좌표가 2~11번섹터까지, 1섹터당 315이니 630,630~3780,3780이다 계산공식은 현재위치에서 630을 배고 3780-630 한 값으로 계산한다.
			m_pMyPosSprite->m_pSprite->m_nPutX = (long)( (long)(pSelfCreature->GetPosition()->x / 100.0f - 630) * 512 / 3150.0f ) ;
			m_pMyPosSprite->m_pSprite->m_nPutY = (long)( 512 - (long)(pSelfCreature->GetPosition()->z / 100.0f - 630) * 512 / 3150.0f ) ;
			break;

		case SERVER_ID::CAPITAL:
			m_pMyPosSprite->m_pSprite->m_nPutX = (long)( (long)(pSelfCreature->GetPosition()->x / 100.0f - 315) * 512 / 3465.0f ) ;
			m_pMyPosSprite->m_pSprite->m_nPutY = (long)( 512 - (long)(pSelfCreature->GetPosition()->z / 100.0f - 315) * 512 / 3465.0f ) ;
			break;

		case SERVER_ID::FRAG_ZONE:
			m_pMyPosSprite->m_pSprite->m_nPutX = 138 + (long)(((long)(pSelfCreature->GetPosition()->x / 100.0f) - 1000) * 1.0923f);
			m_pMyPosSprite->m_pSprite->m_nPutY = 372 + (long)((1000 - (long)(pSelfCreature->GetPosition()->z / 100.0f)) * 1.09f);
			break;

		case SERVER_ID::ZONE12:
			m_pMyPosSprite->m_pSprite->m_nPutX = (long)( (long)(pSelfCreature->GetPosition()->x / 100.0f - 315) * 512 / 3465.0f ) ;
			m_pMyPosSprite->m_pSprite->m_nPutY = (long)( 512 - (long)(pSelfCreature->GetPosition()->z / 100.0f - 315) * 512 / 3465.0f ) ;
			break;

		case SERVER_ID::STATUE_ZONE:
			m_pMyPosSprite->m_pSprite->m_nPutX = 182 + (long)(((long)(pSelfCreature->GetPosition()->x / 100.0f) - 1000) * 0.655f);
			m_pMyPosSprite->m_pSprite->m_nPutY = 328 + (long)((1000 - (long)(pSelfCreature->GetPosition()->z / 100.0f)) * 0.65f);
			break;
	}

	m_pMyPosSprite->m_pSprite->m_nPutX += left;
	m_pMyPosSprite->m_pSprite->m_nPutY += top;

	m_pMyPosSprite->m_pSprite->Render(lpD3DDevice, pSelfCreature->GetDirection() + FLOAT_PHI, 0x80);//paint myself on the map
	
	// 컨텐츠 : 석상전
	if (true == CRYLNetworkData::Instance()->UseContents(GameRYL::STONE_BATTLE))
	{
		// 석상전 존일때는 석상
		if( (/*SERVER_ID::STONE_WAR1*/SERVER_ID::CAPITAL <= CRYLNetworkData::Instance()->m_dwPresentZone && CRYLNetworkData::Instance()->m_dwPresentZone <= SERVER_ID::STONE_WAR3) && GetStatue())
		{
			CRYLGameData* pGame = CRYLGameData::Instance();
			StetueMap::iterator end		= m_mapStatue.end();
			StetueMap::iterator begin	= m_mapStatue.begin();
			for(StetueMap::iterator itr = begin; itr!=end; itr++)//hayzohar statue bug? was for(StetueMap::iterator itr = begin; itr!=end; itr++) 茵 鎭壽
			{
				if(itr->second)
				{
					if( // 1레벨
						itr->second->m_sStatue.m_dwKID != MonsterInfo::LIFE_EXTRACT_AKHAN_COMPLETE1 &&
						itr->second->m_sStatue.m_dwKID != MonsterInfo::LIFE_EXTRACT_AKHAN_LOADING1 &&
						itr->second->m_sStatue.m_dwKID != MonsterInfo::LIFE_EXTRACT_HUMAN_LOADING1 &&
						itr->second->m_sStatue.m_dwKID != MonsterInfo::LIFE_EXTRACT_HUMAN_COMPLETE1 &&
						itr->second->m_sStatue.m_dwKID != MonsterInfo::LIFE_EXTRACT_NEUTRALITY1 &&
						// 2레벨
						itr->second->m_sStatue.m_dwKID != MonsterInfo::LIFE_EXTRACT_AKHAN_COMPLETE2 &&
						itr->second->m_sStatue.m_dwKID != MonsterInfo::LIFE_EXTRACT_AKHAN_LOADING2 &&
						itr->second->m_sStatue.m_dwKID != MonsterInfo::LIFE_EXTRACT_HUMAN_LOADING2 &&
						itr->second->m_sStatue.m_dwKID != MonsterInfo::LIFE_EXTRACT_HUMAN_COMPLETE2 &&
						itr->second->m_sStatue.m_dwKID != MonsterInfo::LIFE_EXTRACT_NEUTRALITY2 
						) 
					{
						CRYLSprite* pSprite = NULL;

						int iKID = itr->second->m_sStatue.m_dwKID;

						bool bAlpha = false;

						// 중립 석상.
						if(itr->second->m_sStatue.m_cNation==Creature::STATELESS)
						{
							if(itr->second->m_sStatue.m_bAttacked)
							{
								pSprite = pGame->m_lpInterface->m_lpStatue[1];
								bAlpha = true;
							}
							else
							{
								pSprite = pGame->m_lpInterface->m_lpStatue[0];
							}
						}
						// 아칸 석상
						else if(itr->second->m_sStatue.m_cNation==Creature::MERKADIA)
						{
							// 소환중
							if(itr->second->m_sStatue.m_bSummoning)
							{
								if(itr->second->m_sStatue.m_bAttacked)
								{
									pSprite = pGame->m_lpInterface->m_lpStatue[5];
								}
								else
								{
									pSprite = pGame->m_lpInterface->m_lpStatue[4];
								}
								bAlpha = true;
							}
							else
							{
								if(itr->second->m_sStatue.m_bAttacked)
								{
									pSprite = pGame->m_lpInterface->m_lpStatue[3];
								}
								else
								{
									pSprite = pGame->m_lpInterface->m_lpStatue[2];
								}
							}
						}
						// 인간 석상
						else if(itr->second->m_sStatue.m_cNation==Creature::KARTERANT)
						{
							// 소환중
							if(itr->second->m_sStatue.m_bSummoning)
							{
								if(itr->second->m_sStatue.m_bAttacked)
								{
									pSprite = pGame->m_lpInterface->m_lpStatue[9];
								}
								else
								{
									pSprite = pGame->m_lpInterface->m_lpStatue[8];
								}
								bAlpha = true;
							}
							else
							{
								if(itr->second->m_sStatue.m_bAttacked)
								{
									pSprite = pGame->m_lpInterface->m_lpStatue[7];
								}
								else
								{
									pSprite = pGame->m_lpInterface->m_lpStatue[6];
								}
							}
						}

						// 석상 스프라이트 출력
						if(pSprite)
						{				
							/*
							m_pMyPosSprite->m_pSprite->m_nPutX = (long)((pSelfCreature->GetPosition()->x / 100.0f) / 1116.0f * 512) - 395;
							m_pMyPosSprite->m_pSprite->m_nPutY = (long)( 512 - (pSelfCreature->GetPosition()->z / 100.0f) / 1111.0f * 512) + 405;
							break;
							long ix = itr->second->m_sStatue.m_Pos.fPointX;
							long iy = itr->second->m_sStatue.m_Pos.fPointZ;
							long dwX = (long)(ix / 1116.0f * 512) - 395;
							long dwY = (long)( 512 - (long)(iy / 1111.0f * 512)) + 405;

							dwX -= 30;
							dwY	-= 76;

							dwX += left;
							dwY += top;
							*/
							
							long ix = itr->second->m_sStatue.m_Pos.fPointX;
							long iy = itr->second->m_sStatue.m_Pos.fPointZ;
							long dwX = (long)(ix / 3465.0f * 512) - 315;
							long dwY = (long)( 512 - (long)(iy / 3465.0f * 512)) - 315;

							dwX += 240;
							dwY	+= 285;

							dwX += left;
							dwY += top;
							
							unsigned long dwTick = timeGetTime();

							if(dwTick-itr->second->m_dwTick>300)
							{
								itr->second->m_dwTick = dwTick;

								if(itr->second->m_sStatue.m_bAttacked)
								{
									if(itr->second->m_bAttackFlag<MAX_ATTACK_FLAG)
									{
										itr->second->m_bAttackFlag++;
									}
									else
									{
										itr->second->m_sStatue.m_bAttacked = false;
									}
								}

								if(bAlpha)
								{
									(itr->second->m_cAlpha == 0xff) ? itr->second->m_cAlpha = 0x80 : itr->second->m_cAlpha = 0xff;
								}
							}					

							pSprite->Render(lpD3DDevice, itr->second->m_cAlpha, dwX, dwY);
							
						}
					}
					
				}
			}		
		}
	}
/*
	// 컨텐츠 : 신규존
	if(true == CRYLNetworkData::Instance()->UseContents(GameRYL::NEWZONE_ZONE9))
	{
		// 신규존일때 생명축출기 출력
		if(CRYLNetworkData::Instance()->m_dwPresentZone == SERVER_ID::ZONE9 && GetStatue())
		{
			CRYLGameData* pGame = CRYLGameData::Instance();

			StetueMap::iterator begin	= m_mapStatue.begin();
			StetueMap::iterator end		= m_mapStatue.end();

			for(StetueMap::iterator itr = begin; itr!=end; itr++)
			{
				if(itr->second)
				{
					if( // 1레벨
						itr->second->m_sStatue.m_dwKID == MonsterInfo::LIFE_EXTRACT_AKHAN_COMPLETE1 ||
						itr->second->m_sStatue.m_dwKID == MonsterInfo::LIFE_EXTRACT_AKHAN_LOADING1 ||
						itr->second->m_sStatue.m_dwKID == MonsterInfo::LIFE_EXTRACT_HUMAN_LOADING1 ||
						itr->second->m_sStatue.m_dwKID == MonsterInfo::LIFE_EXTRACT_HUMAN_COMPLETE1 ||
						itr->second->m_sStatue.m_dwKID == MonsterInfo::LIFE_EXTRACT_NEUTRALITY1 ||
						// 2레벨
						itr->second->m_sStatue.m_dwKID == MonsterInfo::LIFE_EXTRACT_AKHAN_COMPLETE2 ||
						itr->second->m_sStatue.m_dwKID == MonsterInfo::LIFE_EXTRACT_AKHAN_LOADING2 ||
						itr->second->m_sStatue.m_dwKID == MonsterInfo::LIFE_EXTRACT_HUMAN_LOADING2 ||
						itr->second->m_sStatue.m_dwKID == MonsterInfo::LIFE_EXTRACT_HUMAN_COMPLETE2 ||
						itr->second->m_sStatue.m_dwKID == MonsterInfo::LIFE_EXTRACT_NEUTRALITY2 
						)
					{
						CRYLSprite* pSprite = NULL;

						bool bAlpha = false;

						// 중립 생명 추출기
						if(itr->second->m_sStatue.m_cNation==Creature::STATELESS)
						{
							if(itr->second->m_sStatue.m_bAttacked)
							{
								pSprite = pGame->m_lpInterface->m_lpStatue[11];
								bAlpha = true;
							}
							else
							{
								pSprite = pGame->m_lpInterface->m_lpStatue[10];
							}
						}
						// 아칸 생명 추출기
						else if(itr->second->m_sStatue.m_cNation==Creature::MERKADIA)
						{
							// 소환중
							if(itr->second->m_sStatue.m_bSummoning)
							{
								if(itr->second->m_sStatue.m_bAttacked)
								{
									pSprite = pGame->m_lpInterface->m_lpStatue[15];
								}
								else
								{
									pSprite = pGame->m_lpInterface->m_lpStatue[14];
								}
								bAlpha = true;
							}
							else
							{
								if(itr->second->m_sStatue.m_bAttacked)
								{
									pSprite = pGame->m_lpInterface->m_lpStatue[13];
								}
								else
								{
									pSprite = pGame->m_lpInterface->m_lpStatue[12];
								}
							}
						}
						// 인간 생명 추출기
						else if(itr->second->m_sStatue.m_cNation==Creature::KARTERANT)
						{
							// 소환중
							if(itr->second->m_sStatue.m_bSummoning)
							{
								if(itr->second->m_sStatue.m_bAttacked)
								{
									pSprite = pGame->m_lpInterface->m_lpStatue[19];
								}
								else
								{
									pSprite = pGame->m_lpInterface->m_lpStatue[18];
								}
								bAlpha = true;
							}
							else
							{
								if(itr->second->m_sStatue.m_bAttacked)
								{
									pSprite = pGame->m_lpInterface->m_lpStatue[17];
								}
								else
								{
									pSprite = pGame->m_lpInterface->m_lpStatue[16];
								}
							}
						}

						// 생명 추출기 스프라이트 출력.
						if(pSprite)
						{		
							long ix = itr->second->m_sStatue.m_Pos.fPointX;
							long iy = itr->second->m_sStatue.m_Pos.fPointZ;
							long dwX = (long)(ix / 1116.0f * 512) - 395;
							long dwY = (long)( 512 - (long)(iy / 1111.0f * 512)) + 405;

							dwX -= 30;
							dwY	-= 76;

							dwX += left;
							dwY += top;

							if(bAlpha)
							{
								unsigned long dwTick = timeGetTime();

								if(dwTick-itr->second->m_dwTick>300)
								{
									itr->second->m_dwTick = dwTick;
									(itr->second->m_cAlpha == 0xff) ? itr->second->m_cAlpha = 0x80 : itr->second->m_cAlpha = 0xff;

									if(itr->second->m_sStatue.m_bAttacked)
									{
										if(itr->second->m_bAttackFlag<MAX_ATTACK_FLAG)
										{
											itr->second->m_bAttackFlag++;
										}
										else
										{
											itr->second->m_sStatue.m_bAttacked = false;
										}
									}
								}
							}					

							pSprite->Render(lpD3DDevice, itr->second->m_cAlpha, dwX, dwY);
						}
					}

				}
			}
////////////////
			g_pLargeMapDlg->m_pLargeMapPanel->SetVisible( TRUE ) ;
			g_pLargeMapDlg->m_pLargeMapCampDistributePanel->SetVisible( FALSE ) ;
//			g_pLargeMapDlg->m_pLargeMapMobDistributePanel->SetVisible( FALSE ) ;
/////////////
		}
	}
*/

	// 리스폰 버튼을 제일 위에 그리기 위해 다시 그린다. (한번 더 그리는 셈이 된다.)
	for ( int i = 0 ; i < RESPAWNPOS_MAX ; i ++ )
	{
		if ( m_pSpawnButton[ i ] && m_pSpawnButton[ i ]->GetVisible() )
		{
			GetDlgPosition( m_pSpawnButton[ i ], &xx, &yy ) ;
			m_pSpawnButton[ i ]->GMUIPaint( xx, yy ) ;
		}
	}
}

VOID	CRYLLargeMapFrameDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y )
{
	if ( !g_pLargeMapDlg )
		return ;

	CRYLGameData* pGame = CRYLGameData::Instance() ;
	CRYLNetworkData* pNetwork = CRYLNetworkData::Instance() ;

	switch( Sender->GetInstance() )
	{
		case RYLLARGEMAPFRAMEDLG::TAB_BUTTON :
		{
			INT		iFocus = g_pLargeMapDlg->GetTabButton()->GetFocusTabButton() ;
			g_pLargeMapDlg->SetFocusPanel( iFocus ) ;
			break ;
		}

		default :
		{
			if ( Sender->GetInstance() >= RYLLARGEMAPFRAMEDLG::RESPAWN_BUTTON )
			{
				int nIndex = Sender->GetInstance() - RYLLARGEMAPFRAMEDLG::RESPAWN_BUTTON ;
				SendPacket::CharRespawnAreaInfo( g_GameSession, pNetwork->m_dwMyChrID, pGame->m_RespawnArea[ nIndex ].m_dwTownID ) ;
			}
			break ;
		}
	}
}

VOID	CRYLLargeMapFrameDlg::SetFocusPanel( int iFocus )
{
	SetStatue(FALSE);
/*
	if(true == CRYLNetworkData::Instance()->UseContents(GameRYL::NEWZONE_ZONE9))
	{
		// 신규존일때 생명축출기 출력
		if(CRYLNetworkData::Instance()->m_dwPresentZone == SERVER_ID::ZONE9)
		{
			SetStatue(TRUE);
		}
	}
*/
	g_pLargeMapDlg->m_pLargeMapPanel->SetVisible( TRUE ) ;

	if (true == CRYLNetworkData::Instance()->UseContents(GameRYL::STONE_BATTLE))
	{
		// 다크 카나번 존일때는 석상
		if(/*SERVER_ID::STONE_WAR1*/SERVER_ID::CAPITAL <= CRYLNetworkData::Instance()->m_dwPresentZone && CRYLNetworkData::Instance()->m_dwPresentZone <= SERVER_ID::STONE_WAR3)
		{
			SetStatue(TRUE);
		}
	}

	switch( iFocus )
	{
		case 0 : 
			g_pLargeMapDlg->m_pLargeMapPanel->SetVisible( TRUE ) ;
			g_pLargeMapDlg->m_pLargeMapCampDistributePanel->SetVisible( FALSE ) ;
			break ;
		case 1 :
			g_pLargeMapDlg->m_pLargeMapPanel->SetVisible( FALSE ) ;
			g_pLargeMapDlg->m_pLargeMapCampDistributePanel->SetVisible( TRUE ) ;
			break ;
	}

/*
	switch( iFocus )
	{
		case 0 : 
			SetStatue(FALSE);
			if(true == CRYLNetworkData::Instance()->UseContents(GameRYL::NEWZONE_ZONE9))
			{
				// 신규존일때 생명축출기 출력
				if(CRYLNetworkData::Instance()->m_dwPresentZone == SERVER_ID::ZONE9)
				{
					SetStatue(TRUE);
				}
			}

			g_pLargeMapDlg->m_pLargeMapPanel->SetVisible( TRUE ) ;
			g_pLargeMapDlg->m_pLargeMapMobDistributePanel->SetVisible( FALSE ) ;
			g_pLargeMapDlg->m_pLargeMapCampDistributePanel->SetVisible( FALSE ) ;
			break ;
		case 1 :
			SetStatue(FALSE);

			g_pLargeMapDlg->m_pLargeMapPanel->SetVisible( TRUE ) ;
			g_pLargeMapDlg->m_pLargeMapMobDistributePanel->SetVisible( TRUE ) ;
			g_pLargeMapDlg->m_pLargeMapCampDistributePanel->SetVisible( FALSE ) ;
			break ;
		case 2 :
			SetStatue(TRUE);

			g_pLargeMapDlg->m_pLargeMapPanel->SetVisible( FALSE ) ;
			g_pLargeMapDlg->m_pLargeMapMobDistributePanel->SetVisible( FALSE ) ;
			g_pLargeMapDlg->m_pLargeMapCampDistributePanel->SetVisible( TRUE ) ;
			break ;
	}
*/
}

VOID				
CRYLLargeMapFrameDlg::SetShowLargeMap( BOOL bShowLargeMap )
{
	if ( m_pLargeMapPanel )
		((CRYLLargeMapDlg*)m_pLargeMapPanel)->SetShowLargeMap( bShowLargeMap );
}

VOID				
CRYLLargeMapFrameDlg::LoadLargeMapTexture() 
{
	if (SERVER_ID::PRISON != CRYLNetworkData::Instance()->m_dwPresentZone &&
		100 != CRYLNetworkData::Instance()->m_dwPresentZone)
	{
		if ( m_pLargeMapPanel )
		{
			static_cast<CRYLLargeMapDlg*>(m_pLargeMapPanel)->LoadLargeMapTexture();
		}
		if ( m_pLargeMapCampDistributePanel )
		{
			static_cast<CRYLCampDistributePanel*>(m_pLargeMapCampDistributePanel)->LoadCampEnableBuildMapTexture();
		}

/////////////
//		if ( m_pLargeMapMobDistributePanel )
	//	{
//			static_cast<CRYLMobDistributeDlg*>(m_pLargeMapMobDistributePanel)->LoadMobDistributeTexture();
//		}

//////////////
	}
}

VOID	
CRYLLargeMapFrameDlg::HideControl()
{
	if ( m_pLargeMapPanel )
		((CRYLLargeMapDlg*)m_pLargeMapPanel)->HideControl();
}

VOID				
CRYLLargeMapFrameDlg::ShowLargeMap( USHORT& pWaitNum_Out )
{
	if ( m_pLargeMapPanel )
		((CRYLLargeMapDlg*)m_pLargeMapPanel)->ShowLargeMap( pWaitNum_Out );
}

VOID                
CRYLLargeMapFrameDlg::SetStoneState( unsigned char cIndex, unsigned wKID, unsigned char cState )
{
	if ( m_pLargeMapPanel )
		((CRYLLargeMapDlg*)m_pLargeMapPanel)->SetStoneState( cIndex, wKID, cState );
}

BOOL				
CRYLLargeMapFrameDlg::GetShowLargeMap()
{
	if ( m_pLargeMapPanel )
		return ((CRYLLargeMapDlg*)m_pLargeMapPanel)->GetShowLargeMap();
	return FALSE;
}

BOOL				
CRYLLargeMapFrameDlg::GetRespawnEnable()
{
	if ( m_pLargeMapPanel )
		((CRYLLargeMapDlg*)m_pLargeMapPanel)->GetRespawnEnable();
	return FALSE;
}

VOID	
CRYLLargeMapFrameDlg::ShowQuestTarget( QuestInstance* lpQuest )
{
	m_lpQuestTarget = lpQuest ;
}

VOID	
CRYLLargeMapFrameDlg::PlayerLevelChanged()
{
//	if ( m_pLargeMapMobDistributePanel )   //////////////
//		((CRYLMobDistributeDlg*)m_pLargeMapMobDistributePanel)->PlayerLevelChanged();   /////////////
}

VOID CRYLLargeMapFrameDlg::SetDisableRespawn()
{
	for ( int i = 0 ; i < RESPAWNPOS_MAX ; i ++ )
	{
		if ( m_pSpawnButton[ i ] )
		{
			DetachGMUIControl( m_pSpawnButton[ i ] ) ;
			GM_DELETE( m_pSpawnButton[ i ] ) ;
		}
	}
}

VOID	CRYLLargeMapFrameDlg::ClearRespawnButton() 
{
	for ( int i = 0 ; i < RESPAWNPOS_MAX ; i ++ )
	{
		if ( m_pSpawnButton[ i ] )
		{
			DetachGMUIControl( m_pSpawnButton[ i ] ) ;
			GM_DELETE( m_pSpawnButton[ i ] ) ;
		}
	}

	CRYLGameData* pGame = CRYLGameData::Instance() ;
	pGame->m_iRespawnNum = 0 ;
	memset( pGame->m_RespawnArea, 0, sizeof( RespawnArea ) * 100 ) ;
}

VOID	CRYLLargeMapFrameDlg::MakeRespawnButton() 
{
	CRYLGameData* pGame = CRYLGameData::Instance() ;
	CGMImageDib* pSprite = CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ;
	RECT rt ;
	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;

	for ( int i = 0 ; i < RESPAWNPOS_MAX ; i ++ )
	{
		if ( m_pSpawnButton[ i ] )
		{
			DetachGMUIControl( m_pSpawnButton[ i ] ) ;
			GM_DELETE( m_pSpawnButton[ i ] ) ;
		}
	}

	for ( int i = 0 ; i < pGame->m_iRespawnNum ; i ++ )
	{
		m_pSpawnButton[ i ] = new CRYLButton( this ) ;		// 휴먼 리스폰 버튼
		m_pSpawnButton[ i ]->SetLeft( 0 ) ;
		m_pSpawnButton[ i ]->SetTop( 0 ) ;
		m_pSpawnButton[ i ]->OnClickEvent = DlgClick ;
		m_pSpawnButton[ i ]->SetInstance( RYLLARGEMAPFRAMEDLG::RESPAWN_BUTTON + i ) ;
		SetRect( &rt, 80, 84, 93, 97) ;
		m_pSpawnButton[ i ]->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
		m_pSpawnButton[ i ]->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
		m_pSpawnButton[ i ]->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_UP ) ;
		m_pSpawnButton[ i ]->AttachButtonImage( pSprite, &rt, CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
		m_pSpawnButton[ i ]->SetVisible( TRUE ) ;
		AddAlwaysTopObject( m_pSpawnButton[ i ] ) ;

		// edith 2008.01.23 죽었을때 리스폰에서 부활 버튼 클릭시 다운되는 버그.
		long lX, lY;

		switch (CRYLNetworkData::Instance()->m_dwPresentZone)
		{
		case SERVER_ID::ZONE1:
		//	lX = (long)((pGame->m_RespawnArea[ i ].m_RespawnPos.fPointX ) / 3465.0f * 512) - 48;
		//	lY = (long)( 512 - (pGame->m_RespawnArea[ i ].m_RespawnPos.fPointZ ) / 3465.0f * 512) - 46 ;
			lX = (long)( (pGame->m_RespawnArea[ i ].m_RespawnPos.fPointX  - 315) * 512 / 3465 ) ;
			lY = (long)( 512 - (pGame->m_RespawnArea[ i ].m_RespawnPos.fPointZ  - 315) * 512 / 3465 ) ;
			break;

		case SERVER_ID::ZONE2:
			lX = (long)((pGame->m_RespawnArea[ i ].m_RespawnPos.fPointX ) / 3422.0f * 512) - 52;
			lY = (long)( 512 - (pGame->m_RespawnArea[ i ].m_RespawnPos.fPointZ ) / 3438.0f * 512) + 52;
			break;

		case SERVER_ID::ZONE3:
		case SERVER_ID::STONE_WAR1:
		case SERVER_ID::STONE_WAR2:
		case SERVER_ID::STONE_WAR3:
			lX = (long)( (pGame->m_RespawnArea[ i ].m_RespawnPos.fPointX / 1116.0f * 512) ) - 395;
			lY = (long)( 512 - (pGame->m_RespawnArea[ i ].m_RespawnPos.fPointZ / 1111.0f * 512) ) + 405;
			break;

		case SERVER_ID::ZONE4:
			lX = (long)((pGame->m_RespawnArea[ i ].m_RespawnPos.fPointX ) / 2463.0f * 512) - 68;
			lY = (long)( 512 - (pGame->m_RespawnArea[ i ].m_RespawnPos.fPointZ ) / 2383.0f * 512) + 83;
			break;

		case SERVER_ID::ZONE5:
			lX = (long)((pGame->m_RespawnArea[ i ].m_RespawnPos.fPointX ) / 3120.0f * 512) - 53;
			lY = (long)( 512 - (pGame->m_RespawnArea[ i ].m_RespawnPos.fPointZ ) / 3104 * 512) + 55;
			break;

		case SERVER_ID::ZONE9:
			lX = (long)( (long)(pGame->m_RespawnArea[ i ].m_RespawnPos.fPointX  - 630) * 512 / 3150.0f ) ;
			lY = (long)( 512 - (long)(pGame->m_RespawnArea[ i ].m_RespawnPos.fPointZ  - 630) * 512 / 3150.0f ) ;
			break;

		case SERVER_ID::CAPITAL:
			lX = (long)( (long)(pGame->m_RespawnArea[ i ].m_RespawnPos.fPointX  - 315) * 512 / 3465.0f ) ;
			lY = (long)( 512 - (long)(pGame->m_RespawnArea[ i ].m_RespawnPos.fPointZ  - 315) * 512 / 3465.0f ) ;
			break;

		case SERVER_ID::FRAG_ZONE:
			lX = 138 + (long)(((long)(pGame->m_RespawnArea[ i ].m_RespawnPos.fPointX ) - 1000) * 1.0923f);
			lY = 372 + (long)((1000 - (long)(pGame->m_RespawnArea[ i ].m_RespawnPos.fPointZ )) * 1.09f);
			break;

		case SERVER_ID::ZONE12:
			lX = (long)( (pGame->m_RespawnArea[ i ].m_RespawnPos.fPointX  - 315) * 512 / 3465 ) ;
			lY = (long)( 512 - (pGame->m_RespawnArea[ i ].m_RespawnPos.fPointZ  - 315) * 512 / 3465 ) ;
			break;

		case SERVER_ID::STATUE_ZONE:
			lX = 182 + (long)(((long)(pGame->m_RespawnArea[ i ].m_RespawnPos.fPointX ) - 1000) * 0.655f);
			lY = 328 + (long)((1000 - (long)(pGame->m_RespawnArea[ i ].m_RespawnPos.fPointZ )) * 0.65f);
			break;
		}

		m_pSpawnButton[ i ]->SetLeft( lX - 7 ) ;
		m_pSpawnButton[ i ]->SetTop( lY + 27 ) ;
	}
}

BOOL CRYLLargeMapFrameDlg::GetClearPortal()
{
	return m_bClearPortal;
}

void CRYLLargeMapFrameDlg::SetClearPortal(BOOL bClearPortal)
{
	m_bClearPortal = bClearPortal;
}

void CRYLLargeMapFrameDlg::SetPortal(BOOL bPortal)
{ 
	m_bPortal = bPortal;
}

BOOL CRYLLargeMapFrameDlg::GetPortal()
{ 
	return m_bPortal;
}

const StatueDisplayInfo* CRYLLargeMapFrameDlg::GetStatue(unsigned long dwIndex)
{
	StetueMap::iterator itr = m_mapStatue.find(dwIndex);

	if(itr==m_mapStatue.end())
	{ 
		return NULL;
	}

	return &itr->second->m_sStatue;
}

void CRYLLargeMapFrameDlg::Update(unsigned long dwIndex, StatueDisplayInfo* sStatueInfo)
{
	StetueMap::iterator itr = m_mapStatue.find(dwIndex);

	StatueMapInfo* pStatueMapInfo = NULL;

	if(itr==m_mapStatue.end())
	{
		// new
		pStatueMapInfo = new StatueMapInfo;
		
		if(pStatueMapInfo)
		{
			bool bInsert = m_mapStatue.insert(std::make_pair(sStatueInfo->m_dwIndex, pStatueMapInfo)).second;	

			if(!bInsert)
			{
				delete pStatueMapInfo;				
				return;
			}
		}
	}
	else
	{
		pStatueMapInfo = itr->second;
	}

	memcpy(&pStatueMapInfo->m_sStatue, sStatueInfo, sizeof(StatueDisplayInfo));
	pStatueMapInfo->m_cAlpha		= 0xff;
	pStatueMapInfo->m_bAttackFlag	= 0;
	pStatueMapInfo->m_dwTick		= timeGetTime();	
}