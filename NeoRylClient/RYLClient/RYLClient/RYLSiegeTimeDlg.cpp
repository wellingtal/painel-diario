
#include <winsock2.h>
#include "RYLClientMain.h"
#include "RYLRaceBase.h"
#include "RYLNetworkData.h"
#include "RYLGameScene.h"
#include "RYLSceneManager.h"

#include "WinInput.h"
#include "BaseGraphicsLayer.h"
#include "GMImageDib.h"

#include "RYLSiegeTimeDlg.h"

#include "RYLButton.h"
#include "RYLImageManager.h"
#include "RYLImageCabinet.h"
#include "RYLImage.h"
#include "RYLSpriteList.h"
#include "RYLEdit.h"
#include "RYLStringTable.h"
#include "RYLLabel.h"

#include <GameTime/GameTimeConstants.h>
#include "RYLStringTable.h"
#include "RYLChattingDlg.h"
#include "RYLGameData.h"
#include "RYLMessageBox.h"
#include "RYLGameTimeManager.h"
#include <Network/Packet/PacketStruct/CastlePacket.h>
#include <Network/ClientSocket/Send/SendCastle.h>

#include <Network/Packet/PacketStruct/ServerInfo.h>
#include "GMMemory.h"

#include "../EventHandler/gameprocess/BroadcastProcess.h"

using namespace GameTime;

CRYLSiegeTimeDlg* g_SiegeTimeDlg = NULL;

CRYLSiegeTimeDlg::CRYLSiegeTimeDlg(INT nLeft, INT nTop) : CRYLDialog()
{
	m_pGuildLabel				= NULL;
	m_pGuildButton				= NULL;
	m_pRealmLabel				= NULL;
	m_pRealmButton				= NULL;
	m_pCaption					= NULL;
	m_pGuildTimeLabel			= NULL;	
	m_pRealmTimeLabel			= NULL;		
			
	SetWidth(256);
	SetHeight(180);
	
	SetLeft(nLeft);
	SetTop(nTop);
	
	InitCtrl();

	m_dwGuildResult	= 0;
	m_dwRealmResult	= 0;
	g_SiegeTimeDlg	= this;
}

VOID CRYLSiegeTimeDlg::InitCtrl()
{	
	RECT rt;

	// 라벨.

	m_pCaption = new CRYLLabel( this ) ;
	m_pCaption->SetLeft(12);
	m_pCaption->SetTop(18);
	m_pCaption->SetFontColor (0xffffffff);
	m_pCaption->SetAutoSize( TRUE ) ;
	m_pCaption->SetCaption(CRYLStringTable::m_strString[3819]);	
	AttachGMUIControl(m_pCaption);	

	m_pGuildLabel = new CRYLLabel( this ) ;
	m_pGuildLabel->SetLeft(12);
	m_pGuildLabel->SetTop(56);
	m_pGuildLabel->SetFontColor (0xffffffff);
	m_pGuildLabel->SetAutoSize( TRUE ) ;
	m_pGuildLabel->SetCaption(CRYLStringTable::m_strString[3820]);	
	AttachGMUIControl(m_pGuildLabel);

	m_pRealmLabel = new CRYLLabel( this ) ;
	m_pRealmLabel->SetLeft(12);
	m_pRealmLabel->SetTop(108);
	m_pRealmLabel->SetFontColor(0xffffffff);
	m_pRealmLabel->SetAutoSize(TRUE) ;
	m_pRealmLabel->SetCaption(CRYLStringTable::m_strString[3821]);
	AttachGMUIControl(m_pRealmLabel);

	m_pGuildTimeLabel = new CRYLLabel( this ) ;
	m_pGuildTimeLabel->SetLeft(12);
	m_pGuildTimeLabel->SetTop(81);
	m_pGuildTimeLabel->SetFontColor(0xffffffff);
	SetRect( &rt, 12, 81, 160, 93) ;
	m_pGuildTimeLabel->SetAlignRect( rt ) ;
	m_pGuildTimeLabel->SetAlign(CENTER);	
	m_pGuildTimeLabel->SetCaption("00:00");
	m_pGuildTimeLabel->SetAutoSize(TRUE) ;	
	AttachGMUIControl(m_pGuildTimeLabel);

	m_pRealmTimeLabel = new CRYLLabel( this ) ;
	m_pRealmTimeLabel->SetLeft(12);
	m_pRealmTimeLabel->SetTop(133);
	m_pRealmTimeLabel->SetFontColor(0xffffffff);
	SetRect( &rt, 12, 133, 160, 145) ;
	m_pRealmTimeLabel->SetAlignRect( rt ) ;
	m_pRealmTimeLabel->SetAlign(CENTER);	
	m_pRealmTimeLabel->SetCaption("00:00");
	m_pRealmTimeLabel->SetAutoSize(TRUE) ;	
	AttachGMUIControl(m_pRealmTimeLabel);

	// 버튼.

	CRYLSpriteList* pRYLButtonRect	= CRYLSpriteList::Instance() ;
	CGMImageDib*	pSprite			= CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_BUTTON_256x256 ) ;	

	m_pGuildButton = new CRYLButton(this) ;
	m_pGuildButton->SetLeft(174) ;
	m_pGuildButton->SetTop(72);
	m_pGuildButton->OnClickEvent = DlgClick ;
	m_pGuildButton->SetInstance(RYLSIEGETIMEDLG::GUILDWAR_JOIN_BUTTON) ;
	m_pGuildButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pGuildButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pGuildButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pGuildButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pGuildButton ) ;
	m_pGuildButton->SetCaption(CRYLStringTable::m_strString[3586]);	

	m_pRealmButton = new CRYLButton(this) ;
	m_pRealmButton->SetLeft(174) ;
	m_pRealmButton->SetTop(124);
	m_pRealmButton->OnClickEvent = DlgClick ;
	m_pRealmButton->SetInstance(RYLSIEGETIMEDLG::REALMWAR_JOIN_BUTTON) ;
	m_pRealmButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonNormalRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_NORMAL ) ;
	m_pRealmButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDownRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DOWN ) ;
	m_pRealmButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonUpRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_UP ) ;
	m_pRealmButton->AttachButtonImage( pSprite, pRYLButtonRect->GetDefaultButtonDisableRect( 4 ), CRYLImageCabinet::EDCT_BUTTON_DISIBLE ) ;
	AttachGMUIControl( m_pRealmButton);
	m_pRealmButton->SetCaption(CRYLStringTable::m_strString[3586]);

	// 다이얼 로그 이미지.
	
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

	CRYLSpriteList::Instance()->CreateGMImage( 256, ( 30 * 5 ) + 15 , &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;		
	int i = 0;
	for (  ; i < 30 ; i ++ )
	{
		SetRect( &rt, 0, 38, 256, 43 ) ;
		pImageDIB->DrawRect( &rt, 0, (i * 5), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;
	}	

	SetRect( &rt, 0, 42, 256, 52 ) ;
	pImageDIB->DrawRect( &rt, 0, (i * 5), CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_COMPONENT01_256x256 ) ) ;			

	pImageDIB->DrawFrameRect( 2, 0, 252, 21, 0xc0000000 ) ;
	pImageDIB->MakeFrameRect( 2, 20, 252, 1, 0xffB99E6B, 0x80000000,  1 ) ;

	// 라인그리기.

	pImageDIB->MakeFrameRect( 10, 61, 150, 20, 0xffB99E6B, 0x80000000,  1 ) ;
	pImageDIB->DrawFrameRect( 11, 62, 148, 18, 0xc0000000 ) ;

	pImageDIB->MakeFrameRect( 10, 113, 150, 20, 0xffB99E6B, 0x80000000,  1 ) ;
	pImageDIB->DrawFrameRect( 11, 114, 148, 18, 0xc0000000 ) ;
	
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;

	AttachWindowTexture((CGMTexture*)pImage);		

	GM_DELETE( pImageDIB ) ;

	SetFormType( RYLDLG_TYPE_CLOSEBUTTON ) ;	
}

CRYLSiegeTimeDlg::~CRYLSiegeTimeDlg()
{
	FinalCtrl() ;
}

VOID CRYLSiegeTimeDlg::FinalCtrl ()
{
	GM_DELETE(m_pGuildLabel);
	GM_DELETE(m_pGuildButton);
	
	GM_DELETE(m_pRealmLabel);
	GM_DELETE(m_pRealmButton);

	GM_DELETE(m_pGuildTimeLabel);
	GM_DELETE(m_pRealmTimeLabel);

	GM_DELETE(m_pCaption);
}

VOID CRYLSiegeTimeDlg::Show( CGMUIObject* Sender )
{	
}
	
VOID CRYLSiegeTimeDlg::Hide( CGMUIObject* Sender )
{	
	// Visible Flag Setting
	CRYLGameScene* pScene = static_cast<CRYLGameScene*>( CRYLSceneManager::Instance()->GetCurrentScene() ) ;
	if ( pScene ) pScene->SetVisibleInfo( RYLMAINWINDOW::SIEGETIME_WINDOW, FALSE ) ;
}

VOID CRYLSiegeTimeDlg::GMUIPaint( INT xx, INT yy )
{
	CRYLDialog::GMUIPaint( xx, yy ) ;	
}

HRESULT CRYLSiegeTimeDlg::Update( BOOL &bClick, BOOL &bEdge ) 
{
	if (FALSE == GetVisible() )
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

	CRYLGameData* pGame = CRYLGameData::Instance();

	unsigned long dwGuildTime	= CRYLGameTimeManager::Instance()->GetGuildRemainSec()/1000;
	unsigned long dwRealmTime	= CRYLGameTimeManager::Instance()->GetRealmRemainSec()/1000;

	// edith 2009.07.17 전쟁시간 표시 방법 변경
//	if (GameTime::GT_NORMAL != m_cGuildType)
	if (GameTime::GT_GUILD_WAR <= m_cGuildType)
	{
		char szInfo[0xff] = {0, };
		unsigned long dwHours = ( dwGuildTime / 3600 ) ;
		unsigned long dwMinutes = ( dwGuildTime / 60 ) % 60 ;
		
		if ( 0 != ( dwGuildTime % 60 ) )
		{
			++dwMinutes ;
		}

		if ( 60 == dwMinutes )
		{
			dwMinutes = 0 ;
			++dwHours ;
		}

		sprintf(szInfo, CRYLStringTable::m_strString[3585], dwHours, dwMinutes );
		m_pGuildTimeLabel->SetCaption(szInfo);			
	}

	// 컨텐츠 : 석상전
	if (true == CRYLNetworkData::Instance()->UseContents(GameRYL::STONE_BATTLE))
	{
		// edith 2009.07.17 전쟁시간 표시 방법 변경
//		if (GameTime::RT_NORMAL != m_cRealmType)
		if (GameTime::RT_REALM_WAR <= m_cRealmType)
		{
			char szInfo[0xff] = {0, };
			unsigned long dwHours = ( dwRealmTime / 3600 ) ;
			unsigned long dwMinutes = ( dwRealmTime / 60 ) % 60 ;

			if ( 0 != ( dwRealmTime % 60 ) )
			{
				++dwMinutes ;
			}

			if ( 60 == dwMinutes )
			{
				dwMinutes = 0 ;
				++dwHours ;
			}

			sprintf(szInfo, CRYLStringTable::m_strString[3585], dwHours, dwMinutes );
			m_pRealmTimeLabel->SetCaption(szInfo);			
		}
	}
	else
	{
		m_pRealmLabel->SetCaption( CRYLStringTable::m_strString[3863] ) ;
		m_pRealmTimeLabel->SetCaption( CRYLStringTable::m_strString[3864] ) ;
		m_pRealmButton->SetVisible( FALSE ) ;
		m_pRealmButton->SetEnable( FALSE ) ;
	}

	// 참여 버튼 활성화 여부 체크
	RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature() ;
	if ( pSelfCreature )
	{
		if (Creature::WAR_OFF == pSelfCreature->GetGuildWarFlag())
		{
			m_pGuildButton->SetEnable( TRUE ) ;
		}

		if (Creature::WAR_OFF == pSelfCreature->GetRealmWarFlag())
		{
			m_pRealmButton->SetEnable( TRUE ) ;
		}
	}
	else
	{
		m_pGuildButton->SetEnable( FALSE ) ;
		m_pRealmButton->SetEnable( FALSE ) ;
	}

	if ( m_dwRealmResult || m_dwGuildResult )
	{
		if ( m_dwRealmResult & MB_YES )
		{
			// 패킷 전송					
			SendPacket::WarOnOff(g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, Creature::WAR_INSTANCE, GameTime::REALM);
			m_dwRealmResult = 0;
		}

		if ( m_dwGuildResult & MB_YES )
		{
			SendPacket::WarOnOff(g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, Creature::WAR_INSTANCE, GameTime::GUILD);
			m_dwGuildResult = 0;
		}
	}
	
	return S_OK ;
}

VOID CRYLSiegeTimeDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y )
{
	if ( !g_SiegeTimeDlg )
		return ;

	RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature() ;
	if ( !pSelfCreature )
		return ;

	CharDataNode* pSelfData = RYLCharacterDataManager::Instance()->GetSelfData();
    if( !pSelfData )
        return;

	switch ( Sender->GetInstance() )
	{
		// 길드 전쟁 참여 신청
		case RYLSIEGETIMEDLG::GUILDWAR_JOIN_BUTTON :
		{	
			if ( CRYLGameTimeManager::Instance()->IsGuildWarTime( g_SiegeTimeDlg->m_cGuildType ) )
			{
				if ( pSelfCreature->GetGuildWarFlag() == Creature::WAR_OFF )
				{
					if ( !CRYLGameData::Instance()->m_csStatus.m_Info.GID )
					{
						CRYLChattingDlg* pChat = CRYLChattingDlg::Instance() ;
						pChat->AddMessage( CRYLStringTable::m_strString[3886], CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
					}
					else
					{
						// 참여.
						CRYLMessageBox* lpMessageBox = new CRYLMessageBox ;

						if ( CRYLNetworkData::Instance()->m_dwPresentZone == SERVER_ID::CAPITAL )
						{
							lpMessageBox->SetWordBreak( TRUE ) ;
							lpMessageBox->Create( CRYLStringTable::m_strString[3888], MB_YES | MB_NO | MB_EXIT ) ;
						}
						else
						{
							lpMessageBox->SetWordBreak( TRUE ) ;
							lpMessageBox->Create( CRYLStringTable::m_strString[3889], MB_YES | MB_NO | MB_EXIT ) ;
						}

						lpMessageBox->SetResult( &g_SiegeTimeDlg->m_dwGuildResult ) ;
						g_SiegeTimeDlg->m_dwGuildResult = 0 ;
					}					
				}
				else
				{
					// 참여중.
					CRYLChattingDlg* pChat = CRYLChattingDlg::Instance() ;
					pChat->AddMessage( CRYLStringTable::m_strString[3887], CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
				}
			}
			else
			{
				CRYLChattingDlg* pChat = CRYLChattingDlg::Instance() ;
				pChat->AddMessage( CRYLStringTable::m_strString[3885], CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
			}
		}
		break ;


		// 국가 전쟁 참가 신청.			
		case RYLSIEGETIMEDLG::REALMWAR_JOIN_BUTTON :
		{
			if ( CRYLGameTimeManager::Instance()->IsRealmWarReadyTime(g_SiegeTimeDlg->m_cRealmType) ||
				(g_SiegeTimeDlg->m_cRealmType == GameTime::RT_REALM_WAR || g_SiegeTimeDlg->m_cRealmType == GameTime::RT_REALM_WAR_END_5 ) 	)
			{
				if (CRYLNetworkData::Instance()->m_dwPresentZone >= /*SERVER_ID::STONE_WAR1*/SERVER_ID::CAPITAL && 
					CRYLNetworkData::Instance()->m_dwPresentZone <= SERVER_ID::STONE_WAR3)
				{
					// 참여중.
					CRYLChattingDlg* pChat = CRYLChattingDlg::Instance() ;
					pChat->AddMessage( CRYLStringTable::m_strString[3582], CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
				}
				else if(pSelfData->m_cLevel <= 60)
				{
					// edith 2010.01.16 레벨이 부족하면 참가 불가 40랩이하
					CRYLMessageBox *lpMessageBox = new CRYLMessageBox;
					lpMessageBox->Create(CRYLStringTable::m_strString[4413]);
					lpMessageBox->SetModal(FALSE);
				}
				else
				{
					// 참여.
					CRYLMessageBox *lpMessageBox = new CRYLMessageBox ;					
					lpMessageBox->SetWordBreak( TRUE ) ;
					lpMessageBox->Create( CRYLStringTable::m_strString[3581], MB_YES | MB_NO | MB_EXIT ) ;
					lpMessageBox->SetResult( &g_SiegeTimeDlg->m_dwRealmResult ) ;
					g_SiegeTimeDlg->m_dwRealmResult = 0 ;

				}
/*
				if ( pSelfCreature->GetRealmWarFlag() == Creature::WAR_OFF )
				{
					// 참여.
					CRYLMessageBox *lpMessageBox = new CRYLMessageBox ;					
					lpMessageBox->SetWordBreak( TRUE ) ;
					lpMessageBox->Create( CRYLStringTable::m_strString[3581], MB_YES | MB_NO | MB_EXIT ) ;
					lpMessageBox->SetResult( &g_SiegeTimeDlg->m_dwRealmResult ) ;
					g_SiegeTimeDlg->m_dwRealmResult = 0 ;
				}
				else
				{
					// 참여중.
					CRYLChattingDlg* pChat = CRYLChattingDlg::Instance() ;
					pChat->AddMessage( CRYLStringTable::m_strString[3582], CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
				}
*/
			}
			else
			{
				CRYLChattingDlg* pChat = CRYLChattingDlg::Instance() ;
				pChat->AddMessage( CRYLStringTable::m_strString[3583], CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
			}
		}
		break ;	
	}
}
/*
HRESULT	CRYLSiegeTimeDlg::Render( LPDIRECT3DDEVICE8 lpD3DDevice )
{ 
	return S_OK;
}

HRESULT	CRYLSiegeTimeDlg::FinalRender(LPDIRECT3DDEVICE8 lpD3DDevice) 
{
	return S_OK ;
}

*/
void CRYLSiegeTimeDlg::SetGuildType(unsigned char cType)									   
{		
	m_cGuildType = cType;

	// edith 2009.07.17 전쟁시간 표시 방법 변경
//	if(cType==GT_NORMAL)
	if(cType<GT_GUILD_WAR)
	{
		CRYLGameTimeManager::sDetailTimeInfo* pTimeInfo = CRYLGameTimeManager::Instance()->GetGuildDetailTimeInfo();

		if(pTimeInfo)
		{			
			char szInfo[0xff] = {0, };
			sprintf(szInfo, CRYLStringTable::m_strString[3584], pTimeInfo->m_cMonth, pTimeInfo->m_cDay, pTimeInfo->m_cHour, pTimeInfo->m_cMinute);
			m_pGuildTimeLabel->SetCaption(szInfo);
		}		
	}
}

void CRYLSiegeTimeDlg::SetRealmType(unsigned char cType)
{
	m_cRealmType = cType;

	// 컨텐츠 : 다크 카나번 국가 전쟁
	if (true == CRYLNetworkData::Instance()->UseContents(GameRYL::STONE_BATTLE))
	{
		// edith 2009.07.17 전쟁시간 표시 방법 변경
//		if(cType==RT_NORMAL)
		if(cType<RT_REALM_WAR)
		{
			CRYLGameTimeManager::sDetailTimeInfo* pTimeInfo = CRYLGameTimeManager::Instance()->GetRealmDetailTimeInfo();

			if(pTimeInfo)
			{
				char szInfo[0xff] = {0, };
				sprintf(szInfo, CRYLStringTable::m_strString[3584], pTimeInfo->m_cMonth, pTimeInfo->m_cDay, pTimeInfo->m_cHour, pTimeInfo->m_cMinute);
				m_pRealmTimeLabel->SetCaption(szInfo);
			}		
		}
	}
}