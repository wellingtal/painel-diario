#include <winsock2.h>
#include "RYLClientMain.h"
#include "RYLGuildFrameDlg.h"
#include "GMUIObject.h"
#include "BaseDataDefine.h"
#include "RYLTabButton.h"
#include "RYLImageManager.h"
#include "RYLImage.h"

#include "RYLGuildInfoPanel.h"
#include "RYLGuildPropertyPanel.h"
#include "RYLGuildMemberPanel.h"
#include "RYLGuildTacticsPanel.h"

#include "RYLAuthoritySettingDlg.h"
#include "RYLGuildMarkDlg.h"
#include "RYLHostilityDlg.h"

#include "RYLSpriteList.h"
#include "GMImageDib.h"
#include "WinInput.h"

#include "RYLGameData.h"
#include "RYLNetworkData.h"
#include "RYLStringTable.h"
#include "RYLMessageBox.h"

#include "..\\StringFilter.h"
#include "BaseGraphicsLayer.h"
#include <Network/ClientSocket/Send/SendCommunity.h>
#include <RenderOption.h>
#include "GMMemory.h"
CRYLGuildFrameDlg*	g_pGuildDlg = NULL ;

CRYLGuildFrameDlg::CRYLGuildFrameDlg() 
{
	m_dwCresteGuildResult = 0L ;
	m_szCreateGuildName[ 0 ] = '\0' ;
	m_dwResCheckGuildName = 0L ;
	m_pTabButton	= NULL ;
	
	RECT rt ;
	CGMImageDib*	pImageDIB = NULL ;
	CRYLImage*		pImage	  = NULL ;

	CRYLSpriteList::Instance()->CreateGMImage( 328, 15, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 0, 256, 15 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_GUILD_256x256 ) ) ;
	SetRect( &rt, 0, 53, 72, 68 ) ;
	pImageDIB->DrawRect( &rt, 256, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_GUILD_256x256 ) ) ;
	
	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	AttachCaptionTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;

	CRYLSpriteList::Instance()->CreateGMImage( 328, 398, &pImageDIB ) ;
	pImageDIB->ClearbyColor( 0x00000000 ) ;
	SetRect( &rt, 0, 15, 256, 37 ) ;
	pImageDIB->DrawRect( &rt, 0, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_GUILD_256x256 ) ) ;
	SetRect( &rt, 0, 68, 72, 90 ) ;
	pImageDIB->DrawRect( &rt, 256, 0, CRYLSpriteList::Instance()->FindSprite( ERYLSPRITENAME::RYL_GUILD_256x256 ) ) ;
	
	pImageDIB->DrawFrameRect( 128, 0, 197, 21, 0xc0000000 ) ;
	pImageDIB->DrawFrameRect( 128, 20, 197, 1, 0xffa88a60 ) ;

	pImage = CRYLImageManager::Instance()->CreateRYLImage() ;
	pImage->LoadFromImage( pImageDIB ) ;
	CRYLImageManager::s_pDefaultImageCabinet->AddRYLImage( pImage ) ;
	
	AttachWindowTexture( ( CGMTexture*)pImage ) ;

	GM_DELETE( pImageDIB ) ;
	
	SetWidth( 328 ) ;
	SetHeight( 413 ) ;

	SetTop( 135 ) ;
	SetLeft( 256 ) ;                                

	SetFormType( RYLDLG_FRAME_TYPE_CLOSEBUTTON ) ;
	
	InitCtrl() ;

	g_pGuildDlg = this ;	
}

CRYLGuildFrameDlg::~CRYLGuildFrameDlg() 
{
	FinalCtrl() ;
}

VOID	CRYLGuildFrameDlg::InitCtrl() 
{	
	m_pTabButton = new CRYLTabButton( this ) ;
	m_pTabButton->SetLeft( 1 ) ;
	m_pTabButton->SetTop( 15 ) ;
	m_pTabButton->SetWidth( 252 ) ;
	m_pTabButton->SetHeight( 23 ) ;
	m_pTabButton->SetInstance( RYLGUILDDLG::TAB_BUTTON ) ;
	m_pTabButton->OnClickEvent = DlgClick ;
	m_pTabButton->AttachTabButton( CRYLStringTable::m_strString[ 1985 ], 4 ) ;
	m_pTabButton->AttachTabButton( CRYLStringTable::m_strString[ 1986 ], 4 ) ;
	m_pTabButton->AttachTabButton( CRYLStringTable::m_strString[ 1987 ], 4 ) ;
	m_pTabButton->AttachTabButton( CRYLStringTable::m_strString[ 3944 ], 4 ) ;

	/* -----------------------------------------------------------------------
	By Vincent - 길드 게시판 탭 메뉴를 구현이 될때까지 빼 놓는다.
	m_pTabButton->AttachTabButton( CRYLStringTable::m_strString[ 1988 ], 3 ) ;
	----------------------------------------------------------------------- */

	AttachGMUIControl( m_pTabButton ) ;	;

	LPTABBUTTONINFO		lpTabButton = NULL ;
	lpTabButton = m_pTabButton->GetTabButtonInfo( 0 ) ;
	lpTabButton->m_pTabButton->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 1851 ], 1851, TOOLTIP_TAB ) ;
	lpTabButton = m_pTabButton->GetTabButtonInfo( 1 ) ;
	lpTabButton->m_pTabButton->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 1852 ], 1852, TOOLTIP_TAB ) ;
	lpTabButton = m_pTabButton->GetTabButtonInfo( 2 ) ;
	lpTabButton->m_pTabButton->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 1853 ], 1853, TOOLTIP_TAB ) ;
	lpTabButton = m_pTabButton->GetTabButtonInfo( 3 ) ;
	lpTabButton->m_pTabButton->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 1853 ], 1853, TOOLTIP_TAB ) ;

	/* --------------------------------------------------------------------------------------------------------------------
	By Vincent - 길드 게시판 탭 메뉴를 구현이 될때까지 빼 놓는다.
	lpTabButton = m_pTabButton->GetTabButtonInfo( 3 ) ;
	lpTabButton->m_pTabButton->SetTooltip( &m_TooltipManager, CRYLStringTable::m_strString[ 1854 ], 1854, TOOLTIP_TAB ) ;
	-------------------------------------------------------------------------------------------------------------------- */

	m_AuthoritySettingDlg	= new CRYLAuthoritySettingDlg ;
	m_pGuildMarkDlg			= new CRYLGuildMarkDlg ;
	m_pHostilityDlg			= new CRYLHostilityDlg ;

	m_pGuildInfoPanel = new CRYLGuildInfoPanel( this ) ;
	m_pGuildInfoPanel->SetLeft( 0 ) ;
	m_pGuildInfoPanel->SetTop( 36 ) ;
	m_pGuildInfoPanel->SetWidth( 328 ) ;
	m_pGuildInfoPanel->SetHeight( 375 ) ;
	( ( CRYLGuildInfoPanel* )m_pGuildInfoPanel )->InitCtrl() ;
	m_pGuildInfoPanel->OnClickEvent = DlgClick ;	
	m_pGuildInfoPanel->SetInstance( RYLGUILDDLG::GUILDINFO_PANEL ) ;	
	m_pGuildInfoPanel->SetVisible( TRUE ) ;
	AttachGMUIControl( m_pGuildInfoPanel ) ;	

	m_pGuildPropertyPanel = new CRYLGuildPropertyPanel( this ) ;
	m_pGuildPropertyPanel->SetLeft( 0 ) ;
	m_pGuildPropertyPanel->SetTop( 36 ) ;
	m_pGuildPropertyPanel->SetWidth( 328 ) ;
	m_pGuildPropertyPanel->SetHeight( 375 ) ;
	( ( CRYLGuildPropertyPanel* )m_pGuildPropertyPanel )->InitCtrl() ;
	m_pGuildPropertyPanel->OnClickEvent = DlgClick ;
	m_pGuildPropertyPanel->SetInstance( RYLGUILDDLG::GUILDPROPERTY_PANEL ) ;
	m_pGuildPropertyPanel->SetVisible( FALSE ) ;
	AttachGMUIControl( m_pGuildPropertyPanel ) ;

	m_pGuildMemberPanel = new CRYLGuildMemberPanel( this ) ;
	m_pGuildMemberPanel->SetLeft( 0 ) ;
	m_pGuildMemberPanel->SetTop( 36 ) ;
	m_pGuildMemberPanel->SetWidth( 328 ) ;
	m_pGuildMemberPanel->SetHeight( 375 ) ;
	( ( CRYLGuildMemberPanel* )m_pGuildMemberPanel )->InitCtrl() ;
	m_pGuildMemberPanel->OnClickEvent = DlgClick ;
	m_pGuildMemberPanel->SetInstance( RYLGUILDDLG::GUILDMEMBER_PANEL ) ;
	m_pGuildMemberPanel->SetVisible( FALSE ) ;
	AttachGMUIControl( m_pGuildMemberPanel ) ;

	m_pGuildTacticsPanel = new CRYLGuildTacticsPanel( this ) ;
	m_pGuildTacticsPanel->SetLeft( 0 ) ;
	m_pGuildTacticsPanel->SetTop( 36 ) ;
	m_pGuildTacticsPanel->SetWidth( 328 ) ;
	m_pGuildTacticsPanel->SetHeight( 375 ) ;
	( ( CRYLGuildTacticsPanel* )m_pGuildTacticsPanel )->InitCtrl() ;
	m_pGuildTacticsPanel->OnClickEvent = DlgClick ;
	m_pGuildTacticsPanel->SetInstance( RYLGUILDDLG::GUILDTACTICS_PANEL ) ;
	m_pGuildTacticsPanel->SetVisible( FALSE ) ;
	AttachGMUIControl( m_pGuildTacticsPanel ) ;	
}

HRESULT		CRYLGuildFrameDlg::Update( BOOL &bClick, BOOL &bEdge ) 
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
	
	if ( m_pGuildInfoPanel->GetVisible() )
		( ( CRYLGuildInfoPanel * )m_pGuildInfoPanel )->Update( bClick, bEdge ) ;

	if ( m_pGuildPropertyPanel->GetVisible() )
		( ( CRYLGuildPropertyPanel * )m_pGuildPropertyPanel )->Update( bClick, bEdge ) ;

	if ( m_pGuildMemberPanel->GetVisible() )
		( ( CRYLGuildMemberPanel * )m_pGuildMemberPanel )->Update( bClick, bEdge ) ;

	if ( m_pGuildTacticsPanel->GetVisible() )
		( ( CRYLGuildTacticsPanel * )m_pGuildTacticsPanel )->Update( bClick, bEdge ) ;

	if ( m_AuthoritySettingDlg->GetVisible() )
	{
		( ( CRYLAuthoritySettingDlg* )m_AuthoritySettingDlg )->Update( bClick, bEdge ) ;
	}

	if ( m_pGuildMarkDlg->GetVisible() )
	{
		( ( CRYLGuildMarkDlg* )m_pGuildMarkDlg )->Update( bClick, bEdge ) ;
	}

	if ( m_pHostilityDlg->GetVisible() )
	{
		( ( CRYLHostilityDlg* )m_pHostilityDlg )->Update( bClick, bEdge ) ;
	}

	return S_OK ;
}

HRESULT		CRYLGuildFrameDlg::Render( LPDIRECT3DDEVICE8 lpD3DDevice )  
{
	return S_OK ;
}

HRESULT		CRYLGuildFrameDlg::FinalRender( LPDIRECT3DDEVICE8 lpD3DDevice ) 
{
	return S_OK ;
}

VOID	CRYLGuildFrameDlg::FinalCtrl() 
{
	GM_DELETE( m_pTabButton ) ;
	GM_DELETE( m_pGuildInfoPanel ) ;
	GM_DELETE( m_pGuildPropertyPanel ) ;
	GM_DELETE( m_pGuildMemberPanel ) ;
	GM_DELETE( m_pGuildTacticsPanel ) ;

	GM_DELETE( m_AuthoritySettingDlg ) ;
	GM_DELETE( m_pGuildMarkDlg ) ;
	GM_DELETE( m_pHostilityDlg ) ;
}

VOID	CRYLGuildFrameDlg::Show( CGMUIObject* Sender ) 
{
	if ( m_pGuildInfoPanel && ( GetTabButton()->GetFocusTabButton() == 0 ))
	{
		( ( CRYLGuildInfoPanel* )m_pGuildInfoPanel )->UpdateGuildScreen() ;
	}

	if ( m_pGuildMemberPanel->GetVisible() )
	{
		( ( CRYLGuildMemberPanel* )m_pGuildMemberPanel )->UpdateGuildMemberPanel() ;
	}

	if ( m_pGuildTacticsPanel->GetVisible() )
	{
		( ( CRYLGuildTacticsPanel* )m_pGuildTacticsPanel )->UpdateGuildTacticsPanel() ;
	}
}

VOID	CRYLGuildFrameDlg::Hide( CGMUIObject* Sender ) 
{
	if( m_pGuildInfoPanel )
		m_pGuildInfoPanel->Hide( Sender );
	if( m_pGuildPropertyPanel )
		m_pGuildPropertyPanel->Hide( Sender ) ;
	if( m_pGuildMemberPanel )
		m_pGuildMemberPanel->Hide( Sender ) ;
	if( m_pGuildTacticsPanel )
		m_pGuildTacticsPanel->Hide( Sender ) ;
}

VOID	CRYLGuildFrameDlg::GMUIPaint( INT xx, INT yy ) 
{
	LPDIRECT3DDEVICE8   lpD3DDevice = BaseGraphicsLayer::GetDevice() ;

	CRYLDialog::GMUIPaint( xx, yy ) ;

	if ( m_pGuildInfoPanel->GetVisible() )
	{
		( ( CRYLGuildInfoPanel * )m_pGuildInfoPanel )->FinalRender( lpD3DDevice ) ;
	}

	if ( m_pGuildMarkDlg->GetVisible() )
	{
		( ( CRYLGuildMarkDlg* )m_pGuildMarkDlg )->FinalRender( lpD3DDevice ) ;
	}

	if ( m_pHostilityDlg->GetVisible() )
	{
		( ( CRYLHostilityDlg* )m_pHostilityDlg )->FinalRender( lpD3DDevice ) ;
	}
}

VOID	CRYLGuildFrameDlg::UpdateGuildMessageBox()
{
	if ( m_dwCresteGuildResult & MB_YES )
	{
		char szTempName[ 256 ];
		strncpy( szTempName, m_szCreateGuildName, 256 );

		if ( strtok( szTempName, " \t\r\n" ) == NULL )
		{
			CRYLMessageBox *lpMessageBox ;
			lpMessageBox = new CRYLMessageBox ;
			lpMessageBox->Create( CRYLStringTable::m_strString[ 1989 ] ) ;
			m_dwCresteGuildResult = 0L ;
			return ;
		}

		if ( strlen( m_szCreateGuildName ) > 10 )
		{
			CRYLMessageBox *lpMessageBox ;
			lpMessageBox = new CRYLMessageBox ;
			lpMessageBox->Create( CRYLStringTable::m_strString[1113] ) ;
			m_dwCresteGuildResult = 0L ;
			return ;
		}

		int Len = strlen( m_szCreateGuildName );
		BOOL bCheck2Byte = FALSE;

		static const unsigned short ALLOW_LETTER_NUM	= 2;
		static char AllowLetters[ALLOW_LETTER_NUM] =
		{
			'-', '_'
		};

		// 2바이트 문자열 제한
		for(unsigned short LCount = 0; LCount < Len; LCount++)
		{
			//2바이트는 허가안된다.
			if((szTempName[LCount] & 0x80) == 0x80)
			{
				bCheck2Byte = TRUE;
				break;
			}
			else
			{
				// 영문자 허용	
				if((szTempName[LCount] >= 'A' && szTempName[LCount] <= 'Z') || (szTempName[LCount] >= 'a' && szTempName[LCount] <= 'z'))
					continue;

				// 숫자 허용
				if(szTempName[LCount] >= '0' && szTempName[LCount] <= '9')
					continue;

				// 특수 기호 부분 허용
				int ACount = 0;
				for(; ACount < ALLOW_LETTER_NUM; ACount++)
				{
					if(szTempName[LCount] == AllowLetters[ACount])
						break;
				}

				if(ACount == ALLOW_LETTER_NUM)
				{
					bCheck2Byte = TRUE;
					break;
				}
			}
		}

		if(bCheck2Byte)
		{
			CRYLMessageBox *lpMessageBox ;
			lpMessageBox = new CRYLMessageBox ;
			lpMessageBox->Create( CRYLStringTable::m_strString[1113] ) ;
			m_dwCresteGuildResult = 0L ;
			return;
		}

		if ( g_StringFilter.Filter( m_szCreateGuildName ) )
		{
			CRYLMessageBox *lpMessageBox ;
			lpMessageBox = new CRYLMessageBox ;
			CHAR Buff[ 256 ] ;
			sprintf( Buff, CRYLStringTable::m_strString[1114], m_szCreateGuildName ) ;
			lpMessageBox->Create( Buff, MB_YES | MB_NO | MB_EXIT ) ;
			lpMessageBox->SetResult( &m_dwResCheckGuildName ) ;
			m_dwCresteGuildResult = 0L ;
		} 
		else
		{
			CRYLMessageBox *lpMessageBox ;
			lpMessageBox = new CRYLMessageBox ;
			lpMessageBox->Create( CRYLStringTable::m_strString[116] ) ;
			m_dwCresteGuildResult = 0L ;
			return ;
		}
	}	

	if ( m_dwResCheckGuildName & MB_YES )
	{	
		SendPacket::CharCreateGuild( g_GameSession, 
            CRYLNetworkData::Instance()->m_dwMyChrID, m_szCreateGuildName );

		m_dwResCheckGuildName = 0L ;
	}
}

VOID	CRYLGuildFrameDlg::DlgClick( CGMUIObject* Sender, INT X, INT Y )
{
	if ( !g_pGuildDlg )
		return ;

	switch ( Sender->GetInstance() )
	{
		case RYLGUILDDLG::TAB_BUTTON :
		{
			INT		iFocus = g_pGuildDlg->GetTabButton()->GetFocusTabButton() ;
			switch ( iFocus )
			{
				case 0 :	// 길드 정보 탭
				{
					( ( CRYLGuildInfoPanel* )g_pGuildDlg->GetGuildInfoPanel() )->SetVisible( TRUE ) ;
					( ( CRYLGuildPropertyPanel* )g_pGuildDlg->GetGuildPropertyPanel() )->SetVisible( FALSE ) ;	
					( ( CRYLGuildMemberPanel* )g_pGuildDlg->GetGuildMemberPanel() )->SetVisible( FALSE ) ;
					( ( CRYLGuildTacticsPanel* )g_pGuildDlg->GetGuildTacticsPanel() )->SetVisible( FALSE ) ;

					( ( CRYLGuildInfoPanel* )g_pGuildDlg->GetGuildInfoPanel() )->RequestGuildList();
					break ;
				}

				case 1 :	// 길드 관리 탭
				{
					if ( CRYLGameData::Instance()->m_csStatus.m_Info.GID == 0 ||
						CRYLGameData::Instance()->m_csStatus.m_Info.GID & Guild::MEMBER_WAIT_BIT )
					{
						CRYLMessageBox *lpMessageBox ;
						lpMessageBox = new CRYLMessageBox ;
						lpMessageBox->Create( CRYLStringTable::m_strString[4060] ) ;
						return ;
					}

					RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();

					if(pSelfCreature)
					{
						if(pSelfCreature->GetTactics())
						{
							CRYLMessageBox *lpMessageBox ;
							lpMessageBox = new CRYLMessageBox ;
							lpMessageBox->Create( CRYLStringTable::m_strString[4061] ) ;
							return ;
						}
					}

					( ( CRYLGuildInfoPanel* )g_pGuildDlg->GetGuildInfoPanel() )->SetVisible( FALSE ) ;
					( ( CRYLGuildPropertyPanel* )g_pGuildDlg->GetGuildPropertyPanel() )->SetVisible( TRUE ) ;
					( ( CRYLGuildMemberPanel* )g_pGuildDlg->GetGuildMemberPanel() )->SetVisible( FALSE ) ;	
					( ( CRYLGuildTacticsPanel* )g_pGuildDlg->GetGuildTacticsPanel() )->SetVisible( FALSE ) ;
					
					( ( CRYLGuildPropertyPanel* )g_pGuildDlg->GetGuildPropertyPanel() )->CheckRightButton() ;
					break ;
				}

				case 2 :	// 길드 멤버 탭
				{
					if ( CRYLGameData::Instance()->m_csStatus.m_Info.GID == 0 ||
						 CRYLGameData::Instance()->m_csStatus.m_Info.GID & Guild::MEMBER_WAIT_BIT )
					{
						CRYLMessageBox *lpMessageBox ;
						lpMessageBox = new CRYLMessageBox ;
						lpMessageBox->Create( CRYLStringTable::m_strString[1112] ) ;
						return ;
					}

					( ( CRYLGuildInfoPanel* )g_pGuildDlg->GetGuildInfoPanel() )->SetVisible( FALSE ) ;
					( ( CRYLGuildPropertyPanel* )g_pGuildDlg->GetGuildPropertyPanel() )->SetVisible( FALSE ) ;	
					( ( CRYLGuildMemberPanel* )g_pGuildDlg->GetGuildMemberPanel() )->SetVisible( TRUE ) ;
					( ( CRYLGuildTacticsPanel* )g_pGuildDlg->GetGuildTacticsPanel() )->SetVisible( FALSE ) ;

					( ( CRYLGuildMemberPanel* )g_pGuildDlg->GetGuildMemberPanel() )->UpdateGuildMemberPanel() ;
					break ;
				}

				case 3 :	// 길드 용병 탭
				{
					if ( CRYLGameData::Instance()->m_csStatus.m_Info.GID == 0 ||
						 CRYLGameData::Instance()->m_csStatus.m_Info.GID & Guild::MEMBER_WAIT_BIT )
					{
						CRYLMessageBox *lpMessageBox ;
						lpMessageBox = new CRYLMessageBox ;
						lpMessageBox->Create( CRYLStringTable::m_strString[1112] ) ;
						return ;
					}

					( ( CRYLGuildInfoPanel* )g_pGuildDlg->GetGuildInfoPanel() )->SetVisible( FALSE ) ;
					( ( CRYLGuildPropertyPanel* )g_pGuildDlg->GetGuildPropertyPanel() )->SetVisible( FALSE ) ;	
					( ( CRYLGuildMemberPanel* )g_pGuildDlg->GetGuildMemberPanel() )->SetVisible( FALSE ) ;
					( ( CRYLGuildTacticsPanel* )g_pGuildDlg->GetGuildTacticsPanel() )->SetVisible( TRUE ) ;

					( ( CRYLGuildTacticsPanel* )g_pGuildDlg->GetGuildTacticsPanel() )->UpdateGuildTacticsPanel() ;
					break ;
				}

				/* -------------------------------------------------------------------------------------------------
				By Vincent - 길드 게시판 탭 메뉴를 구현이 될때까지 빼 놓는다.
				case 3 : 
				{
					CRYLMessageBox *lpMessageBox ;
					lpMessageBox = new CRYLMessageBox ;
					lpMessageBox->Create( CRYLStringTable::m_strString[1110] ) ;
					g_pGuildDlg->GetTabButton()->SetFocusTabButton( 0 ) ;
					( ( CRYLGuildInfoPanel* )g_pGuildDlg->GetGuildInfoPanel() )->SetVisible( TRUE ) ;
					( ( CRYLGuildPropertyPanel* )g_pGuildDlg->GetGuildPropertyPanel() )->SetVisible( FALSE ) ;	
					( ( CRYLGuildMemberPanel* )g_pGuildDlg->GetGuildMemberPanel() )->SetVisible( FALSE ) ;	
					break ;
				}
				------------------------------------------------------------------------------------------------- */
			}
			break ;
		}
	}
}


VOID	CRYLGuildFrameDlg::CheckGuildStipulation()
{
	if ( CRYLGameData::Instance()->m_csStatus.m_Info.GID == 0 )
	{
		unsigned char cCreateLevel = CREATE_LEVEL;
		unsigned long dwCreateGold = Guild::ms_arySetLevelGold[0];
/*
		if (GameRYL::CHINA == CRYLNetworkData::Instance()->m_eInternationalCode)
		{
			cCreateLevel = CREATE_LEVEL_FOR_CHINA;
			dwCreateGold = Guild::ms_arySetLevelGoldForChina[0];
		}
*/
/*
		// edith 2008.02.27 길드생성 레벨 제외
		if ( CRYLGameData::Instance()->m_csStatus.m_Info.Level < cCreateLevel )
		{
			CRYLMessageBox* lpMessageBox = new CRYLMessageBox ;
			lpMessageBox->Create( CRYLStringTable::m_strString[1115] ) ;
			return ;
		}
*/			
		if ( CRYLGameData::Instance()->m_csStatus.m_Info.Gold < dwCreateGold )
		{
			CRYLMessageBox* lpMessageBox = new CRYLMessageBox ;
			lpMessageBox->Create( CRYLStringTable::m_strString[1116] ) ;
			return ;
		}

		CRYLMessageBox* lpMessageBox = new CRYLMessageBox ;
		lpMessageBox->Create( CRYLStringTable::m_strString[1117], MB_STRING | MB_YES | MB_NO | MB_EXIT ) ;
		lpMessageBox->SetResult( &m_dwCresteGuildResult ) ;
		lpMessageBox->SetValue( (char *)&m_szCreateGuildName  ) ;
	}
	else
	{
		CRYLMessageBox *lpMessageBox = new CRYLMessageBox ;
		lpMessageBox->Create( CRYLStringTable::m_strString[1118] ) ;
	}
}	