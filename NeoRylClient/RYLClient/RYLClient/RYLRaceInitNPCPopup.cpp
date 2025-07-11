
#include <Creature/EnemyCheck.h>

#include "RYLGameData.h"
#include "RYLNetworkData.h"

#include "RYLSceneManager.h"
#include "RYLGameScene.h"

#include "GUITextEdit.h"
#include "RYLStringTable.h"

#include "RYLGuildFrameDlg.h"
#include "RYLAuthoritySettingDlg.h"
#include "RYLBlacksmithDlg.h"
#include "RYLStatusAndQuestFrameDlg.h"
#include "RYLCompleteQuestPanel.h"

#include "RYLHumanInventoryFrameDlg.h"
#include "RYLAbilityPanel.h"


#include "RYLRaceBaseDefine.h"
#include "RYLRaceBase.h"

#include "RYLGameTimeManager.h"

#include <Network/Packet/PacketStruct/ServerInfo.h>
#include "GMMemory.h"

VOID CRYLRaceBase::InitPopupPC( PopupMenu* pmAddMenu )
{
	CRYLGameData*	pGame = CRYLGameData::Instance();

	RYLCreature*    pTempCreature = RYLCreatureManager::Instance()->GetCreature( m_dwPopupSelectChrID );
	if ( !pTempCreature )
		return;

	// 자신이 병기에 탑승한 경우 Skip.
	RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
	
	if(pSelfCreature)
	{
		if(EnemyCheck::IsSiegeArms(pSelfCreature->GetObjectType()) || EnemyCheck::IsCastleArms(pSelfCreature->GetObjectType()))		
		{
			return;
		}
	}

	if ( pTempCreature )
	{   
		if(pTempCreature->GetRideEnable())
			return;

		CharDataNode*   pTempData = RYLCharacterDataManager::Instance()->GetCharData( pTempCreature->GetCharID() );
		if ( !pTempData )
			return;

		unsigned long dwEnemyType = pGame->GetClientEnemyType(pTempCreature->GetCharID());

	//	if (EnemyCheck::EC_FRIEND == dwEnemyType)   original line
		if (EnemyCheck::EC_FRIEND == dwEnemyType || EnemyCheck::EC_NEUTRAL == dwEnemyType) // trade human akkan available now
		{
			pmAddMenu->m_dwPopupKind = POPUPMENU_EXCHANGE;
			strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[14]);
			m_lstPopupMenu.push_back(*pmAddMenu);
		}

		if (EnemyCheck::EC_FRIEND == dwEnemyType || EnemyCheck::EC_NEUTRAL == dwEnemyType)
		{
			pmAddMenu->m_dwPopupKind = POPUPMENU_DUEL;
			strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[509]);
			m_lstPopupMenu.push_back(*pmAddMenu);
		}

		// 상대가 파티에 속하지 않은 경우
		if (0 == pTempData->m_ulPartyID)
		{	
			if (EnemyCheck::EC_FRIEND == dwEnemyType)
			{
				pmAddMenu->m_dwPopupKind = POPUPMENU_PARTYINVATE;
				strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[0]);
				m_lstPopupMenu.push_back(*pmAddMenu);
			}
		} 
		else
		{
			// 상대가 같은 파티원
			if ( pTempData->m_ulPartyID == pGame->m_csStatus.m_PartyInfo.m_dwPartyID )
			{
				// 자신이 리더인 경우
				if (pGame->m_csStatus.m_PartyInfo.m_dwLeaderID == CRYLNetworkData::Instance()->m_dwMyChrID)
				{	
#ifndef __NEWNPCSCRIPT
					m_dwKindPopup = POPUP_LEADER;
#endif
					pmAddMenu->m_dwPopupKind = POPUPMENU_LEADER;
					strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[1]);
					m_lstPopupMenu.push_back(*pmAddMenu);
					pmAddMenu->m_dwPopupKind = POPUPMENU_VANISH;
					strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[2]);
					m_lstPopupMenu.push_back(*pmAddMenu);
				} 
				else
				{
#ifndef __NEWNPCSCRIPT
					m_dwKindPopup = POPUP_MEMBER;
#endif
				}
			} 
			// 상대가 다른 파티원
			else
			{
#ifndef __NEWNPCSCRIPT
				m_dwKindPopup = POPUP_MEMBER;
#endif
				if (EnemyCheck::EC_FRIEND == dwEnemyType || EnemyCheck::EC_NEUTRAL == dwEnemyType)
				{
					if (pGame->m_csStatus.m_PartyInfo.m_dwLeaderID == CRYLNetworkData::Instance()->m_dwMyChrID)
					{
						if(CRYLNetworkData::Instance()->m_dwPresentZone == SERVER_ID::CAPITAL)
						{
							// 길드전 시간이 아닐때와 길드전 시간중에 길드 미가입자만 가능
							if (!CRYLGameTimeManager::Instance()->IsGuildWarTime())
							{
								pmAddMenu->m_dwPopupKind = POPUPMENU_PARTYDUEL;
								strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[511]);
								m_lstPopupMenu.push_back(*pmAddMenu);
							}
							else if (0 == pTempData->m_ulGuildID)
							{
								pmAddMenu->m_dwPopupKind = POPUPMENU_PARTYDUEL;
								strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[511]);
								m_lstPopupMenu.push_back(*pmAddMenu);
							}
						}
						else //if (CRYLNetworkData::Instance()->m_dwPresentZone == SERVER_ID::ZONE12)
						{
							// edith 2008.01.28 팀배틀 신청 팝업메뉴 ZONE12 제거로직
							pmAddMenu->m_dwPopupKind = POPUPMENU_PARTYDUEL;
							strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[511]);
							m_lstPopupMenu.push_back(*pmAddMenu);
						}
					}
				}
			}
		}

		// edith 2010.01.23 만약 개인상점이 열려있으면 개인상점을 열수 있는 팝업 띄우기
		if ( !pTempData->m_strStreetStallName.empty() )
		{
			pmAddMenu->m_dwPopupKind = POPUPMENU_STREETSTALL;
			strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[510]);
			m_lstPopupMenu.push_back(*pmAddMenu);
		}

		// 길드 초대
		if (pGame->m_csStatus.m_Info.GID != 0 && pTempData->m_ulGuildID == 0 &&
			!CRYLGameTimeManager::Instance()->IsGuildWarTime() &&
			EnemyCheck::EC_FRIEND == dwEnemyType)
		{
			CRYLGameScene*		 pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );
			CRYLGuildFrameDlg*	 pDlg   = ( CRYLGuildFrameDlg* )pScene->GetGuildFrameDlg();
			CRYLAuthoritySettingDlg* pASDlg = ( CRYLAuthoritySettingDlg* )pDlg->GetAuthoritySettingDlg();

			GuildRight* pGuild = pASDlg->GetGuildRight();
			UCHAR ucTitle = pASDlg->GetTitle();

			if ( ucTitle == Guild::MASTER )
			{
				pmAddMenu->m_dwPopupKind = POPUPMENU_GUILDINVITE ;
				strcpy( pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[ 968 ] );
				m_lstPopupMenu.push_back(*pmAddMenu);
			}
			if ( ucTitle == Guild::MIDDLE_ADMIN )
			{
				if ( ( pGuild->m_aryRight[ GuildRight::INVITE_MEMBER ] == Guild::COMMON ) ||
					( pGuild->m_aryRight[ GuildRight::INVITE_MEMBER ] == Guild::MIDDLE_ADMIN ) )
				{
					pmAddMenu->m_dwPopupKind = POPUPMENU_GUILDINVITE ;
					strcpy( pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[ 968 ] );
					m_lstPopupMenu.push_back(*pmAddMenu);
				}
			}	
			if ( ucTitle == Guild::COMMON )
			{
				if ( pGuild->m_aryRight[ GuildRight::INVITE_MEMBER ] == Guild::COMMON ) 
				{
					pmAddMenu->m_dwPopupKind = POPUPMENU_GUILDINVITE ;
					strcpy( pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[ 968 ] );
					m_lstPopupMenu.push_back(*pmAddMenu);
				}
			}
		}

		// 길드 축출
		if (pGame->m_csStatus.m_Info.GID != 0 && pGame->m_csStatus.m_Info.GID == pTempData->m_ulGuildID && 
			EnemyCheck::EC_FRIEND == dwEnemyType)
		{
			CRYLGameScene*		 pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );
			CRYLGuildFrameDlg*	 pDlg   = ( CRYLGuildFrameDlg* )pScene->GetGuildFrameDlg();
			CRYLAuthoritySettingDlg* pASDlg = ( CRYLAuthoritySettingDlg* )pDlg->GetAuthoritySettingDlg();

			GuildRight* pGuild = pASDlg->GetGuildRight();
			UCHAR ucTitle = pASDlg->GetTitle();

			if ( ucTitle == Guild::MASTER )
			{
				pmAddMenu->m_dwPopupKind = POPUPMENU_GUILDKICK ;
				strcpy( pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[ 4044 ] );
				m_lstPopupMenu.push_back(*pmAddMenu);
			}
			if ( ucTitle == Guild::MIDDLE_ADMIN )
			{
				if ( ( pGuild->m_aryRight[ GuildRight::KICK_MEMBER ] == Guild::COMMON ) ||
					( pGuild->m_aryRight[ GuildRight::KICK_MEMBER ] == Guild::MIDDLE_ADMIN ) )
				{
					pmAddMenu->m_dwPopupKind = POPUPMENU_GUILDKICK ;
					strcpy( pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[ 4044 ] );
					m_lstPopupMenu.push_back(*pmAddMenu);
				}
			}	
			if ( ucTitle == Guild::COMMON )
			{
				if ( pGuild->m_aryRight[ GuildRight::KICK_MEMBER ] == Guild::COMMON ) 
				{
					pmAddMenu->m_dwPopupKind = POPUPMENU_GUILDKICK ;
					strcpy( pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[ 4044 ] );
					m_lstPopupMenu.push_back(*pmAddMenu);
				}
			}
		}


		if (EnemyCheck::EC_FRIEND == dwEnemyType)
		{
			if (FALSE == pGame->m_csStatus.GetIsFriend(pTempCreature->GetCharID()) && pGame->m_csStatus.m_lstFriendList.size() < 100)
			{
				pmAddMenu->m_dwPopupKind = POPUPMENU_FRIEND;
				strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[517]);
				m_lstPopupMenu.push_back(*pmAddMenu);
			}
		}
		if (EnemyCheck::EC_FRIEND == dwEnemyType || EnemyCheck::EC_NEUTRAL == dwEnemyType)
		{
			if (FALSE == pGame->m_csStatus.GetIsBan(pTempCreature->GetCharID()) && pGame->m_csStatus.m_lstBanList.size() < 100)
			{
				// 같은 국적일때만 거부하기가 가능하도록 변경!!
				if ( pGame->m_cAccountNation == pTempData->m_cNation )
				{
					pmAddMenu->m_dwPopupKind = POPUPMENU_BAN;
					strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[518]);
					m_lstPopupMenu.push_back(*pmAddMenu);
				}
			}
		}

		if (EnemyCheck::EC_FRIEND == dwEnemyType)
		{
			pmAddMenu->m_dwPopupKind = POPUPMENU_FOLLOW;
			strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[952]);
			m_lstPopupMenu.push_back(*pmAddMenu);
		}
		

		// 용병관련 팝업 //
		pmAddMenu->m_dwPopupKind = POPUPMENU_TACTICS;
		strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[3593]);
		m_lstPopupMenu.push_back(*pmAddMenu);		
/*
		// edith 2009.08.05 용병신청 권유는 막아버린다.
		if(pGame->m_csStatus.GetGuild())
		{
			pmAddMenu->m_dwPopupKind = POPUPMENU_TACTICS_JOIN;
			strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[3594]);
			m_lstPopupMenu.push_back(*pmAddMenu);
		}
*/
	}
}
#ifndef __NEWNPCSCRIPT
	VOID CRYLRaceBase::InitPopupGuard( PopupMenu* pmAddMenu )
	{
		pmAddMenu->m_dwPopupKind = POPUPMENU_DIALOG;
		strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[5]);
		m_lstPopupMenu.push_back(*pmAddMenu);

		pmAddMenu->m_dwPopupKind = POPUPMENU_SAVEPOS;
		strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[6]);
		m_lstPopupMenu.push_back(*pmAddMenu);
	}

	VOID CRYLRaceBase::InitPopupNormal( PopupMenu* pmAddMenu )
	{
		pmAddMenu->m_dwPopupKind = POPUPMENU_DIALOG;
		strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[5]);
		m_lstPopupMenu.push_back(*pmAddMenu);

		pmAddMenu->m_dwPopupKind = POPUPMENU_SAVEPOS;
		strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[6]);
		m_lstPopupMenu.push_back(*pmAddMenu);
	}

	VOID CRYLRaceBase::InitPopupNoBind( PopupMenu* pmAddMenu )
	{
		pmAddMenu->m_dwPopupKind = POPUPMENU_DIALOG;
		strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[5]);
		m_lstPopupMenu.push_back(*pmAddMenu);
	}

	VOID CRYLRaceBase::InitPopupMerchant( PopupMenu* pmAddMenu )
	{
		pmAddMenu->m_dwPopupKind = POPUPMENU_DIALOG;
		strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[5]);
		m_lstPopupMenu.push_back(*pmAddMenu);

		pmAddMenu->m_dwPopupKind = POPUPMENU_TRADE0;
		strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[7]);
		m_lstPopupMenu.push_back(*pmAddMenu);
	}

	VOID CRYLRaceBase::InitPopup2KindMerchant( PopupMenu* pmAddMenu )
	{
		pmAddMenu->m_dwPopupKind = POPUPMENU_DIALOG;
		strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[5]);
		m_lstPopupMenu.push_back(*pmAddMenu);

		NPCNode* pNode = g_NPCList.GetNPCNode( m_dwPopupSelectChrID ) ;
		if ( pNode )
		{
			if ( true == pNode->GetHasItem( CClass::RaceType::HUMAN ) )
			{
				pmAddMenu->m_dwPopupKind = POPUPMENU_TRADE1;
				strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[2725]);
				m_lstPopupMenu.push_back(*pmAddMenu);
			}

			if ( true == pNode->GetHasItem( CClass::RaceType::AKHAN ) )
			{
				pmAddMenu->m_dwPopupKind = POPUPMENU_TRADE2;
				strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[2726]);
				m_lstPopupMenu.push_back(*pmAddMenu);
			}
		}
	}

	VOID CRYLRaceBase::InitPopupCounseller( PopupMenu* pmAddMenu )
	{
		pmAddMenu->m_dwPopupKind = POPUPMENU_DIALOG;
		strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[5]);
		m_lstPopupMenu.push_back(*pmAddMenu);

		pmAddMenu->m_dwPopupKind = POPUPMENU_SAVEPOS;
		strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[6]);
		m_lstPopupMenu.push_back(*pmAddMenu);
	}

	VOID CRYLRaceBase::InitPopupBlackSmith( PopupMenu* pmAddMenu )
	{
		CRYLGameData*	pGame = CRYLGameData::Instance() ;

		pmAddMenu->m_dwColor = D3DCOLOR_RGBA(255, 255, 255, 255);
		pmAddMenu->m_dwPopupKind = POPUPMENU_DIALOG2;
		strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[953]);
		m_lstPopupMenu.push_back(*pmAddMenu);

		if (CRYLBlacksmithDlg::UPGRADE_LIMIT_LEVEL > pGame->m_csStatus.m_Info.Level)
		{
			pmAddMenu->m_dwColor = D3DCOLOR_RGBA(128, 128, 128, 255);
		}
		else
		{
			pmAddMenu->m_dwColor = D3DCOLOR_RGBA(255, 255, 255, 255);
		}
		pmAddMenu->m_dwPopupKind = POPUPMENU_UPGRADE;
		strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[8]);
		m_lstPopupMenu.push_back(*pmAddMenu);

		if (CRYLBlacksmithDlg::GRAFT_LIMIT_LEVEL > pGame->m_csStatus.m_Info.Level)
		{
			pmAddMenu->m_dwColor = D3DCOLOR_RGBA(128, 128, 128, 255);
		}
		else
		{
			pmAddMenu->m_dwColor = D3DCOLOR_RGBA(255, 255, 255, 255);
		}
		pmAddMenu->m_dwPopupKind = POPUPMENU_OPTIONCOMPOSITION;
		strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[2288]);
		m_lstPopupMenu.push_back(*pmAddMenu);

		pmAddMenu->m_dwColor = D3DCOLOR_RGBA(255, 255, 255, 255);
		pmAddMenu->m_dwPopupKind = POPUPMENU_DIALOG;
		strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[5]);
		m_lstPopupMenu.push_back(*pmAddMenu);
	}

	VOID CRYLRaceBase::InitPopupAdventureTrainer( PopupMenu* pmAddMenu )
	{
		pmAddMenu->m_dwPopupKind = POPUPMENU_DIALOG;
		strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[5]);
		m_lstPopupMenu.push_back(*pmAddMenu);

		pmAddMenu->m_dwPopupKind = POPUPMENU_TRADE0;
		strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[7]);
		m_lstPopupMenu.push_back(*pmAddMenu);

		pmAddMenu->m_dwPopupKind = POPUPMENU_STATUSRETRAIN;
		strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[155]);
		m_lstPopupMenu.push_back(*pmAddMenu);
	}

	VOID CRYLRaceBase::InitPopupFigtherTrainer( PopupMenu* pmAddMenu )
	{
		pmAddMenu->m_dwPopupKind = POPUPMENU_DIALOG;
		strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[5]);
		m_lstPopupMenu.push_back(*pmAddMenu);

		pmAddMenu->m_dwPopupKind = POPUPMENU_TRADE0;
		strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[7]);
		m_lstPopupMenu.push_back(*pmAddMenu);
	}

	VOID CRYLRaceBase::InitPopupRogueTrainer( PopupMenu* pmAddMenu )
	{
		pmAddMenu->m_dwPopupKind = POPUPMENU_DIALOG;
		strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[5]);
		m_lstPopupMenu.push_back(*pmAddMenu);

		pmAddMenu->m_dwPopupKind = POPUPMENU_TRADE0;
		strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[7]);
		m_lstPopupMenu.push_back(*pmAddMenu);
	}

	VOID CRYLRaceBase::InitPopupMageTrainer( PopupMenu* pmAddMenu )
	{
		CRYLGameData*	pGame = CRYLGameData::Instance() ;

		pmAddMenu->m_dwPopupKind = POPUPMENU_DIALOG;
		strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[5]);
		m_lstPopupMenu.push_back(*pmAddMenu);

		pmAddMenu->m_dwPopupKind = POPUPMENU_TRADE0;
		strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[7]);
		m_lstPopupMenu.push_back(*pmAddMenu);

		if (pGame->m_csStatus.m_Info.Level >= 1 && pGame->m_csStatus.m_Info.Class == CClass::Mage)
		{
			pmAddMenu->m_dwPopupKind = POPUPMENU_JOBCHANGE;
			strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[9]);
			m_lstPopupMenu.push_back(*pmAddMenu);
		}
	}

	VOID CRYLRaceBase::InitPopupAcolyteTrainer( PopupMenu* pmAddMenu )
	{
		CRYLGameData*	pGame = CRYLGameData::Instance() ;

		pmAddMenu->m_dwPopupKind = POPUPMENU_DIALOG;
		strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[5]);
		m_lstPopupMenu.push_back(*pmAddMenu);

		pmAddMenu->m_dwPopupKind = POPUPMENU_TRADE0;
		strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[7]);
		m_lstPopupMenu.push_back(*pmAddMenu);

		if (pGame->m_csStatus.m_Info.Level >= 1 && pGame->m_csStatus.m_Info.Class == CClass::Acolyte)
		{
			pmAddMenu->m_dwPopupKind = POPUPMENU_JOBCHANGE;
			strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[9]);
			m_lstPopupMenu.push_back(*pmAddMenu);
		}
	}

	VOID CRYLRaceBase::InitPopupGuildMaker( PopupMenu* pmAddMenu ) 
	{
		pmAddMenu->m_dwPopupKind = POPUPMENU_DIALOG;
		strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[5]);
		m_lstPopupMenu.push_back(*pmAddMenu);

		pmAddMenu->m_dwPopupKind = POPUPMENU_MAKEGUILD;
		strcpy( pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[ 969 ] );
		m_lstPopupMenu.push_back( *pmAddMenu );
	}

	VOID CRYLRaceBase::InitPopupTeleporter( PopupMenu* pmAddMenu, DWORD dwUID )
	{
		pmAddMenu->m_dwPopupKind = POPUPMENU_DIALOG;
		strcpy( pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[5] );
		m_lstPopupMenu.push_back( *pmAddMenu );

		NPCNode *pNode = g_NPCList.GetNPCNode(dwUID);
		if (pNode)
		{
			WarpNode* pZone = pNode->GetWarpNode(0);
			if (pZone)
			{
				pmAddMenu->m_dwPopupKind = POPUPMENU_ZONEMOVE;
				switch (pZone->m_wZoneNumber)
				{
					case 1:		strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[156]);	break;
					case 2:		strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[157]);	break;
					case 3:		strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[158]);	break;
					case 4:		strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[512]);	break;
					case 5:		strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[513]);	break;
					case 6:		strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[514]);	break;
					case 8:		strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[954]);	break;
					case 12:	strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[2754]);	break;
					default:	strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[159]);	break;
				}

				m_lstPopupMenu.push_back(*pmAddMenu);
				return;
			} 
		}

		m_dwKindPopup = 0;
		m_dwPopupSelectChrID = 0;
		m_bPopupShow = FALSE;
		m_dwKindPos = 0;
	}

	VOID CRYLRaceBase::InitPopupContainer( PopupMenu* pmAddMenu )
	{
		pmAddMenu->m_dwPopupKind = POPUPMENU_DIALOG2;
		strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[515]);
		m_lstPopupMenu.push_back(*pmAddMenu);

		pmAddMenu->m_dwPopupKind = POPUPMENU_DEPOSIT;
		strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[516]);
		m_lstPopupMenu.push_back(*pmAddMenu);

		pmAddMenu->m_dwPopupKind = POPUPMENU_DIALOG;
		strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[5]);
		m_lstPopupMenu.push_back(*pmAddMenu);
	}

	VOID CRYLRaceBase::InitPopupPeaceGuild( PopupMenu* pmAddMenu )
	{
		pmAddMenu->m_dwPopupKind = POPUPMENU_DIALOG;
		strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[5]);
		m_lstPopupMenu.push_back(*pmAddMenu);

		RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
		if ( !pSelfCreature )
			return;

		CharDataNode* pSelfData = RYLCharacterDataManager::Instance()->GetCharData( pSelfCreature->GetCharID() );
		if ( !pSelfData )
			return;

		pmAddMenu->m_dwPopupKind = POPUPMENU_PEACE;
		if (pSelfData->m_bPeaceMode)
			strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[519]);
		else
			strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[580]);
		m_lstPopupMenu.push_back(*pmAddMenu);
	}

	VOID CRYLRaceBase::InitPopupStatuscleaner( PopupMenu* pmAddMenu )
	{
		pmAddMenu->m_dwPopupKind = POPUPMENU_DIALOG;
		strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[5]);
		m_lstPopupMenu.push_back(*pmAddMenu);

		pmAddMenu->m_dwPopupKind = POPUPMENU_STATUSCLEAN;
		strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[955]);
		m_lstPopupMenu.push_back(*pmAddMenu);
	}

	VOID CRYLRaceBase::InitPopupClasscleaner( PopupMenu* pmAddMenu )
	{
		pmAddMenu->m_dwPopupKind = POPUPMENU_DIALOG;
		strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[5]);
		m_lstPopupMenu.push_back(*pmAddMenu);

		pmAddMenu->m_dwPopupKind = POPUPMENU_CLASSCLEAN;
		strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[956]);
		m_lstPopupMenu.push_back(*pmAddMenu);
	}

	VOID CRYLRaceBase::InitPopupCompensation( PopupMenu* pmAddMenu )
	{
		pmAddMenu->m_dwPopupKind = POPUPMENU_DIALOG;
		strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[5]);
		m_lstPopupMenu.push_back(*pmAddMenu);

		pmAddMenu->m_dwPopupKind = POPUPMENU_COMPENSATION;
		strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[970]);
		m_lstPopupMenu.push_back(*pmAddMenu);
	}

	VOID CRYLRaceBase::InitPopupSkillMerchant( PopupMenu* pmAddMenu )
	{
		pmAddMenu->m_dwPopupKind = POPUPMENU_DIALOG;
		strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[5]);
		m_lstPopupMenu.push_back(*pmAddMenu);

		pmAddMenu->m_dwPopupKind = POPUPMENU_TRADE0;
		strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[2781]);
		m_lstPopupMenu.push_back(*pmAddMenu);

		pmAddMenu->m_dwPopupKind = POPUPMENU_TRADE0;
		strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[2782]);
		m_lstPopupMenu.push_back(*pmAddMenu);

		pmAddMenu->m_dwPopupKind = POPUPMENU_TRADE0;
		strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[2783]);
		m_lstPopupMenu.push_back(*pmAddMenu);
	}

	VOID CRYLRaceBase::InitPopupBattleGroundManager( PopupMenu* pmAddMenu )
	{
		pmAddMenu->m_dwPopupKind = POPUPMENU_DIALOG;
		strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[5]);
		m_lstPopupMenu.push_back(*pmAddMenu);

		pmAddMenu->m_dwPopupKind = POPUPMENU_BATTLEGROUND_MANAGE;
		strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[2343]);
		m_lstPopupMenu.push_back(*pmAddMenu);

		pmAddMenu->m_dwPopupKind = POPUPMENU_BATTLEGROUNDPREV_RESULT;
		strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[2344]);
		m_lstPopupMenu.push_back(*pmAddMenu);
	}

	VOID CRYLRaceBase::InitPopupMoneyChanger( PopupMenu* pmAddMenu )
	{
		pmAddMenu->m_dwPopupKind = POPUPMENU_DIALOG;
		strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[5]);
		m_lstPopupMenu.push_back(*pmAddMenu);

		pmAddMenu->m_dwPopupKind = POPUPMENU_MONEY_CHANGE;
		strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[2345]);
		m_lstPopupMenu.push_back(*pmAddMenu);
	}

	VOID CRYLRaceBase::InitPopupBattleGroundExiter( PopupMenu* pmAddMenu )
	{
		pmAddMenu->m_dwPopupKind = POPUPMENU_DIALOG;
		strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[5]);
		m_lstPopupMenu.push_back(*pmAddMenu);

		pmAddMenu->m_dwPopupKind = POPUPMENU_BATTLEGROUND_EXIT;
		strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[2346]);
		m_lstPopupMenu.push_back(*pmAddMenu);
	}

	VOID CRYLRaceBase::InitPopupChangeName( PopupMenu* pmAddMenu )
	{
		pmAddMenu->m_dwPopupKind = POPUPMENU_DIALOG;
		strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[5]);
		m_lstPopupMenu.push_back(*pmAddMenu);

		pmAddMenu->m_dwPopupKind = POPUPMENU_CHANGENAME;
		strcpy(pmAddMenu->m_strPopupStr, CRYLStringTable::m_strString[2952]);
		m_lstPopupMenu.push_back(*pmAddMenu);
	}
#endif

VOID CRYLRaceBase::InitPopupQuest( PopupMenu* pmAddMenu, DWORD dwUID ) 
{
	if (m_bPopupShow)
	{
		NPCNode*		pNode       = g_NPCList.GetNPCNode(dwUID);
		CRYLGameData*	pGame       = CRYLGameData::Instance();
		CharDataNode*   pSelfData   = RYLCharacterDataManager::Instance()->GetSelfData();

		if ( pNode )
		{
			list<LPQuestInstance>::iterator itQuest;
			for (itQuest = pGame->m_csStatus.m_lstQuest.begin(); itQuest != pGame->m_csStatus.m_lstQuest.end(); itQuest++)
			{
				unsigned long dwTriggerCount = 0;
				TriggerNode* pTrigger = pGame->m_csStatus.GetTriggertoTalkInQuest((*itQuest), &dwTriggerCount, dwUID);
				if (pTrigger)
				{
					pmAddMenu->m_dwPopupKind = POPUPMENU_QUESTTRIGGER ;
					strcpy(pmAddMenu->m_strPopupStr, (*itQuest)->m_lpQuestScript->m_strQuestTitle) ;
					pmAddMenu->m_dwQuestID = (*itQuest)->m_lpQuestScript->m_wQuestID ;
					pmAddMenu->m_dwColor = D3DCOLOR_RGBA(0, 255, 0, 255) ;
#ifdef __NEWNPCSCRIPT
					pmAddMenu->m_dwLimitLevel = 0;
					pmAddMenu->m_dwLimitFame = 0;
					pmAddMenu->m_dwLimitQuest = 0;
#endif
					m_lstPopupMenu.push_back(*pmAddMenu);
				}

				pTrigger = pGame->m_csStatus.GetTriggertoLevelTalkInQuest((*itQuest), &dwTriggerCount, dwUID);
				if (pTrigger)
				{
					pmAddMenu->m_dwPopupKind = POPUPMENU_QUESTTRIGGER ;
					strcpy(pmAddMenu->m_strPopupStr, (*itQuest)->m_lpQuestScript->m_strQuestTitle) ;
					pmAddMenu->m_dwQuestID = (*itQuest)->m_lpQuestScript->m_wQuestID ;
					pmAddMenu->m_dwColor = D3DCOLOR_RGBA(0, 255, 0, 255) ;
#ifdef __NEWNPCSCRIPT
					pmAddMenu->m_dwLimitLevel = (*itQuest)->m_lpPhaseScript->m_lstTrigger[dwTriggerCount]->m_dwLevel;
					pmAddMenu->m_dwLimitFame = 0;
					pmAddMenu->m_dwLimitQuest = 0;
#endif
					m_lstPopupMenu.push_back(*pmAddMenu);
				}

				pTrigger = pGame->m_csStatus.GetTriggertoFameTalkInQuest((*itQuest), &dwTriggerCount, dwUID);
				if (pTrigger)
				{
					pmAddMenu->m_dwPopupKind = POPUPMENU_QUESTTRIGGER ;
					strcpy(pmAddMenu->m_strPopupStr, (*itQuest)->m_lpQuestScript->m_strQuestTitle) ;
					pmAddMenu->m_dwQuestID = (*itQuest)->m_lpQuestScript->m_wQuestID ;
					pmAddMenu->m_dwColor = D3DCOLOR_RGBA(0, 255, 0, 255) ;
#ifdef __NEWNPCSCRIPT
					pmAddMenu->m_dwLimitLevel = 0;
					pmAddMenu->m_dwLimitFame = (*itQuest)->m_lpPhaseScript->m_lstTrigger[dwTriggerCount]->m_dwLevel;
					pmAddMenu->m_dwLimitQuest = 0;
#endif
					m_lstPopupMenu.push_back(*pmAddMenu);
				}

				pTrigger = pGame->m_csStatus.GetTriggertoAbilityTalkInQuest((*itQuest), &dwTriggerCount, dwUID);
				if (pTrigger)
				{
					pmAddMenu->m_dwPopupKind = POPUPMENU_QUESTTRIGGER ;
					strcpy(pmAddMenu->m_strPopupStr, (*itQuest)->m_lpQuestScript->m_strQuestTitle) ;
					pmAddMenu->m_dwQuestID = (*itQuest)->m_lpQuestScript->m_wQuestID ;
					pmAddMenu->m_dwColor = D3DCOLOR_RGBA(0, 255, 0, 255) ;
#ifdef __NEWNPCSCRIPT
					pmAddMenu->m_dwLimitLevel = 0;
					pmAddMenu->m_dwLimitFame = (*itQuest)->m_lpPhaseScript->m_lstTrigger[dwTriggerCount]->m_dwLevel;
					pmAddMenu->m_dwLimitQuest = 0;
#endif
					m_lstPopupMenu.push_back(*pmAddMenu);
				}

			}
		}

#ifndef __NEWNPCSCRIPT
		// 퀘스트를 팝업에 집어넣는 루틴
		if (pNode && pNode->m_lstQuest.size())
		{
			unsigned short aryComplete[1000], aryQuest[QuestInstance::MAX_EXECUTING_QUEST];
			unsigned long i;
			ZeroMemory(aryComplete, sizeof(aryComplete));
			ZeroMemory(aryQuest, sizeof(aryQuest));
			list<QuestInstance *>::iterator it;
			for (it = pGame->m_csStatus.m_lstQuest.begin(), i = 0; it != pGame->m_csStatus.m_lstQuest.end(); it++, i++)
			{
				aryQuest[i] = (*it)->m_dwQuestID;
			}

			for (it = pGame->m_csStatus.m_lstCompleteQuest.begin(), i = 0; it != pGame->m_csStatus.m_lstCompleteQuest.end(); it++, i++)
			{
				aryComplete[i] = (*it)->m_dwQuestID;
			}

			unsigned long dwResult;
			for (i = 0; i < pNode->m_lstQuest.size(); i++)
			{
				QuestNode* pQuest = pNode->m_lstQuest[i]->m_lpQuest;
				if (pQuest)
				{
					if ( pQuest->m_bInvisible )
						continue;

					dwResult = pQuest->CheckQuest( pGame->m_csStatus.m_Info.Level,		// dwLevel
						pGame->m_csStatus.m_Info.Fame,									// dwFame
						pGame->m_csStatus.m_Info.Class,									// dwClass
						pGame->m_cAccountNation,										// dwNation
						aryComplete,													// lstCompleted	
						(unsigned short)pGame->m_csStatus.m_lstCompleteQuest.size(),	// wNumCompleted
						aryQuest );														// lstQuestList

					switch ( dwResult )
					{
						case QuestNode::RES_SUCCESS :					// 성공
						{
							pmAddMenu->m_dwPopupKind = POPUPMENU_QUEST ;
							strcpy(pmAddMenu->m_strPopupStr, pQuest->m_strQuestTitle) ;
							pmAddMenu->m_dwQuestID = pQuest->m_wQuestID ;
							pmAddMenu->m_dwColor = D3DCOLOR_RGBA(0, 255, 0, 255) ;
							m_lstPopupMenu.push_back(*pmAddMenu);
							break;
						}

						case QuestNode::RES_LOW_LEVEL :					// 수행할 수 있는 레벨보다 조금 낮다.
						{
							pmAddMenu->m_dwPopupKind = POPUPMENU_QUEST ;
							strcpy(pmAddMenu->m_strPopupStr, pQuest->m_strQuestTitle) ;
							pmAddMenu->m_dwQuestID = pQuest->m_wQuestID ;
							pmAddMenu->m_dwColor = D3DCOLOR_RGBA(128, 128, 128, 255) ;
							m_lstPopupMenu.push_back(*pmAddMenu);
							break;
						}

						case QuestNode::RES_MISMATCH_CLASS :			// 클래스가 맞지 않음
						case QuestNode::RES_ALREADY_TAKEN :				// 이미 받은 퀘스트
						case QuestNode::RES_NOT_COMPLETE_PRIOR_QUEST :	// 선결 퀘스트를 완료하지 않았음
						case QuestNode::RES_HIGH_THAN_MAX_LEVEL :		// 수행할 수 있는 최고 레벨보다 높다.
						case QuestNode::RES_10_LOW_THAN_MIN_LEVEL :		// 수행할 수 있는 최저 레벨보다 10 레벨 낮다.
						case QuestNode::RES_MISMATCH_NATION :			// 수행할 수 있는 국적이 아니다.
						case QuestNode::RES_LOW_FAME :					// 명성부족
						case QuestNode::RES_HIGH_THAN_MAX_FAME :		// 명성 높다
							break;
					}
				}
			}
		}
#endif
	}
}

//--------------------------------------------------------------------------------------------------------------------------
// CRYLRaceBase::InitNPCPopup()
BOOL CRYLRaceBase::InitNPCPopup( NPCNode* pNpc, unsigned long dwTargetID )
{
	CRYLGameScene*				pScene              = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
	CRYLGameData*				pGame               = CRYLGameData::Instance() ;
	CRYLStatusAndQuestFrameDlg* pDlg				= ( CRYLStatusAndQuestFrameDlg* )pScene->GetStatusAndQuestFrameDlg() ;
	CRYLCompleteQuestPanel*		pCompleteQuestPanel	= ( CRYLCompleteQuestPanel* )pDlg->GetCompleteQuestPanel() ;

	CRYLHumanInventoryFrameDlg* pInvenDlg = ( CRYLHumanInventoryFrameDlg* )pScene->GetHumanInventoryFrameDlg() ;
	CRYLAbilityPanel* pAbilityPanel		 = ( CRYLAbilityPanel* )pInvenDlg->GetAbilityPanel() ;

	if ( pScene->GetVaultDlg()->GetVisible() || 
		pScene->GetBlacksmithDlg()->GetVisible() || 
		pScene->GetNewTradeDlg()->GetVisible() || 
		pScene->GetStreetStallDlg()->GetVisible() || 
		pScene->GetEmblemUpgradeDlg()->GetVisible() )
	{
		return FALSE;
	}

	const unsigned long dwEnemyType = CRYLGameData::GetClientEnemyType(dwTargetID);
	
	if ( EnemyCheck::EC_ENEMY == dwEnemyType || EnemyCheck::EC_NEUTRAL == dwEnemyType )
	{
		m_dwKindPopup = 0;
		m_dwPopupSelectChrID = 0;
		m_bPopupShow = FALSE;
		m_dwKindPos = 0;

		return FALSE;
	} 
	else
	{
		m_dwKindPopup = 0;
		m_dwPopupSelectChrID = dwTargetID;
		m_bPopupShow = TRUE;
		m_dwKindPos = 0;
	}

	PopupMenu	pmAddMenu;
	m_lstPopupMenu.clear();

	unsigned long unDialogOrder = 1;
    unsigned long unTradeOrder  = 0;
#ifdef __NEWNPCSCRIPT
	BOOL bAddMenu = TRUE;
#endif

#ifdef __NEWNPCSCRIPT
	unsigned short aryComplete[1000], aryQuest[QuestInstance::MAX_EXECUTING_QUEST];
	
	if (pNpc && pNpc->IsExistQuest())
	{
		unsigned long i;
		ZeroMemory(aryComplete, sizeof(aryComplete));
		ZeroMemory(aryQuest, sizeof(aryQuest));
		list<QuestInstance *>::iterator it;
		for (it = pGame->m_csStatus.m_lstQuest.begin(), i = 0; it != pGame->m_csStatus.m_lstQuest.end(); it++, i++)
		{
			aryQuest[i] = (*it)->m_dwQuestID;
		}

		for (it = pGame->m_csStatus.m_lstCompleteQuest.begin(), i = 0; it != pGame->m_csStatus.m_lstCompleteQuest.end(); it++, i++)
		{
			aryComplete[i] = (*it)->m_dwQuestID;
		}
	}
#endif

#ifdef __NEWNPCSCRIPT
	for ( vector<PopupNode*>::iterator iterPopup = pNpc->m_lstPopup.begin() ;
         iterPopup != pNpc->m_lstPopup.end() ;
         ++iterPopup )
    {
#else
	for ( vector<PopupNode*>::iterator iterPopup = pNpc->m_vecPopup.begin() ;
         iterPopup != pNpc->m_vecPopup.end() ;
         ++iterPopup )
    {
		PopupNode* pPopup = (*iterPopup);
		bool bPopup	= false;	

		// 선행 퀘스트 체크
		if ( (*iterPopup)->m_ulQuestID != 0 )
        {
			if ( !pCompleteQuestPanel->GetIsCompleteQuest( (*iterPopup)->m_ulQuestID ) )
				continue;
        }

		// 선행 어빌리티 체크
		if ( (*iterPopup)->m_ulLimitAbilityID != 0 )
        {
			if ( !pAbilityPanel->CheckAbility( (*iterPopup)->m_ulLimitAbilityID, (*iterPopup)->m_ulLimitAbilityLV ) )
				continue;
        }

#endif
        CharDataNode* pSelfData = RYLCharacterDataManager::Instance()->GetSelfData();
        if ( !pSelfData )
		{
			m_dwKindPopup = 0;
			m_dwPopupSelectChrID = 0;
			m_bPopupShow = FALSE;
			m_dwKindPos = 0;

            return FALSE;
		}

#ifdef __NEWNPCSCRIPT
		// 클래스 제한
		if ( 0 == ((1 << (pSelfData->m_usClass - 1)) & (*iterPopup)->m_dwLimitClass) )
			continue;
		
#else
		// 레벨 제한
		if ( pSelfData->m_cLevel < (*iterPopup)->m_usLimitLevel )
		{			
			bPopup = true;
		}

		// 명성 제한
		if ( pSelfData->m_ulFame < (*iterPopup)->m_ulLimitFame ) 		
		{
			bPopup = true;
		}

		// 클래스 제한
		if ( 0 == ((1 << (pSelfData->m_usClass - 1)) & (*iterPopup)->m_ulLimitClass) )
			continue;


		// 선행 정보 제한. (예를들면 길드가입여부라든지)
		if((*iterPopup)->m_usLimitOther != 0)
		{
			// edith 2010.01.23 길드가입 제한 처리하기
			if((*iterPopup)->m_usLimitOther == 1 && pSelfData->m_ulGuildID == 0)	// 길드 가입 제한인데 길드 가입 안되어있음
				continue;
		}

#endif

        pmAddMenu.m_dwColor     = D3DCOLOR_RGBA( 255, 255, 255, 255 );
        pmAddMenu.m_dwPopupKind = (*iterPopup)->m_dwPopupKind;
        strcpy( pmAddMenu.m_strPopupStr, (*iterPopup)->m_strPopupString.c_str() );
#ifdef __NEWNPCSCRIPT
		pmAddMenu.m_lpJobNode = (*iterPopup)->m_lpJobNode;
		pmAddMenu.m_dwLimitLevel = (*iterPopup)->m_dwLimitLevel;
		pmAddMenu.m_dwLimitFame	 = (*iterPopup)->m_dwLimitFame;
		pmAddMenu.m_dwLimitQuest = (*iterPopup)->m_dwLimitQuest;

		bAddMenu = TRUE;

		// 레벨 제한
		if ( pSelfData->m_cLevel < static_cast<char>(pmAddMenu.m_dwLimitLevel) )
            pmAddMenu.m_dwColor = D3DCOLOR_RGBA(128, 128, 128, 255);

		// 명성 제한
		if ( pSelfData->m_ulFame < static_cast<unsigned long>(pmAddMenu.m_dwLimitFame) )
            pmAddMenu.m_dwColor = D3DCOLOR_RGBA(128, 128, 128, 255);

		// 선행 퀘스트 체크 
		if ( pmAddMenu.m_dwLimitQuest != 0 )
        {
			if ( !pCompleteQuestPanel->GetIsCompleteQuest( pmAddMenu.m_dwLimitQuest ) )
	            pmAddMenu.m_dwColor = D3DCOLOR_RGBA(128, 128, 128, 255);
        }

#endif
        
#ifdef __NEWNPCSCRIPT
		switch(pmAddMenu.m_dwPopupKind)
		{
			case POPUPMENU_DIALOG:
				// 다이얼로그의 갯수를 퀘스트ID에 저장한다. (대략 좋지 않음)
				pmAddMenu.m_dwQuestID = unDialogOrder;
				++unDialogOrder;
				break;

			case POPUPMENU_QUEST:
				if(pmAddMenu.m_lpJobNode)
				{
					QuestNode* pQuest = static_cast<LPQuestJobNode>(pmAddMenu.m_lpJobNode)->m_lpQuest;
					if (pQuest)
					{
						if ( pQuest->m_bInvisible )							// 선행 퀘스트가 0xFFFF면 나오지 않는다.
							continue;

						unsigned long dwResult = pQuest->CheckQuest(
							pGame->m_csStatus.m_Info.Level,									// dwLevel
							pGame->m_csStatus.m_Info.Fame,									// dwFame
							pGame->m_csStatus.m_Info.Class,									// dwClass
							pGame->m_cAccountNation,										// dwNation
							aryComplete,													// lstCompleted	
							(unsigned short)pGame->m_csStatus.m_lstCompleteQuest.size(),	// wNumCompleted
							aryQuest );														// lstQuestList

						switch ( dwResult )
						{
							case QuestNode::RES_SUCCESS :					// 성공
								pmAddMenu.m_dwQuestID = pQuest->m_wQuestID ;
								pmAddMenu.m_dwColor = D3DCOLOR_RGBA(0, 255, 0, 255) ;
								break;

							case QuestNode::RES_LOW_LEVEL :					// 수행할 수 있는 레벨보다 조금 낮다.
							case QuestNode::RES_NOT_COMPLETE_PRIOR_QUEST :	// 선결 퀘스트를 완료하지 않았음
								pmAddMenu.m_dwQuestID = pQuest->m_wQuestID ;
								pmAddMenu.m_dwColor = D3DCOLOR_RGBA(128, 128, 128, 255) ;
								break;

							case QuestNode::RES_MISMATCH_CLASS :			// 클래스가 맞지 않음
							case QuestNode::RES_ALREADY_TAKEN :				// 이미 받은 퀘스트
							case QuestNode::RES_HIGH_THAN_MAX_LEVEL :		// 수행할 수 있는 최고 레벨보다 높다.
							case QuestNode::RES_10_LOW_THAN_MIN_LEVEL :		// 수행할 수 있는 최저 레벨보다 10 레벨 낮다.
							case QuestNode::RES_MISMATCH_NATION :			// 수행할 수 있는 국적이 아니다.
							case QuestNode::RES_LOW_FAME :					// 명성부족
							case QuestNode::RES_HIGH_THAN_MAX_FAME :		// 명성 높다
								bAddMenu = FALSE;
								break;
						}
					}
				}
				break;
		}

		if(bAddMenu)
		{ 
			m_lstPopupMenu.push_back( pmAddMenu );

			if(bPopup)
			{
				DisablePopupMenuKind(pmAddMenu->m_dwPopupKind, pmAddMenu->m_dwPopupKind);
			}
		}
#else
		if ( ProcessPopup( pNpc, &pmAddMenu ) )
        {
			if ( pmAddMenu.m_dwPopupKind == POPUPMENU_DIALOG )
            {
                pmAddMenu.m_dwQuestID = unDialogOrder;
                ++unDialogOrder;
            }

            if ( pmAddMenu.m_dwPopupKind == POPUPMENU_TRADE )
            {
                pmAddMenu.m_dwQuestID = unTradeOrder;
                pmAddMenu.m_usCapacity= (*iterPopup)->m_usCapacity;
                ++unTradeOrder;
            }

			m_lstPopupMenu.push_back( pmAddMenu );

			if(bPopup)
			{
				DisablePopupMenuKind(pmAddMenu.m_dwPopupKind, pmAddMenu.m_dwPopupKind);
			}
        }
#endif
    }

    InitPopupQuest( &pmAddMenu, dwTargetID );

	if(m_lstPopupMenu.empty())
	{
		m_dwKindPopup = 0;
		m_dwPopupSelectChrID = 0;
		m_bPopupShow = FALSE;
		m_dwKindPos = 0;
		return FALSE;
	}

	m_dwPopupWindowLength = 15;
	for (unsigned long i = 0; i < m_lstPopupMenu.size(); i++)
	{
		unsigned long lengthtemp = g_TextEdit.GetStringLength(m_lstPopupMenu[i].m_strPopupStr);
		if (m_dwPopupWindowLength < lengthtemp) 
			m_dwPopupWindowLength = lengthtemp;
	}

	InitPopupPosition();

	// Simple Mouse Mode 로 변경s
	if ( pGame && !pGame->m_bMouseMode && m_bPopupShow )
	{
		pGame->m_bSimpleMouse = TRUE ;
	}

	return TRUE;
}

//--------------------------------------------------------------------------------------------------------------------------
// CRYLRaceBase::ProcessPopup()
BOOL CRYLRaceBase::ProcessPopup( NPCNode* pNpc, PopupMenu* pMenu )
{
	CRYLGameData*	pGame = CRYLGameData::Instance() ;

	// 팝업 메뉴에 대해서 특별한 처리를 해줘야 하는 것들에 대해서....
	switch ( pMenu->m_dwPopupKind )
	{
		// 대장간 
		case POPUPMENU_UPGRADE:
		{
			PopupNode* pPopupNode = pNpc->GetPopupNode( POPUPMENU_UPGRADE ) ;
			if ( pPopupNode )
			{
#ifdef __NEWNPCSCRIPT
				if ( static_cast<char>(pPopupNode->m_dwLimitLevel) > pGame->m_csStatus.m_Info.Level )
#else
				if ( pPopupNode->m_usLimitLevel > pGame->m_csStatus.m_Info.Level )
#endif
				{
					pMenu->m_dwColor = D3DCOLOR_RGBA(128, 128, 128, 255);
				}
				else
				{
					pMenu->m_dwColor = D3DCOLOR_RGBA(255, 255, 255, 255);
				}
			}
			break;
		}
		
		case POPUPMENU_OPTIONCOMPOSITION:
		{
			PopupNode* pPopupNode = pNpc->GetPopupNode( POPUPMENU_OPTIONCOMPOSITION ) ;
			if ( pPopupNode )
			{
#ifdef __NEWNPCSCRIPT
				if ( static_cast<char>(pPopupNode->m_dwLimitLevel) > pGame->m_csStatus.m_Info.Level )
#else
				if ( pPopupNode->m_usLimitLevel > pGame->m_csStatus.m_Info.Level )
#endif
				{
					pMenu->m_dwColor = D3DCOLOR_RGBA(128, 128, 128, 255);
				}
				else
				{
					pMenu->m_dwColor = D3DCOLOR_RGBA(255, 255, 255, 255);
				}
			}
			break;
		}

		case POPUPMENU_JOBCHANGE:
		{
			if ( pGame->m_csStatus.m_Info.Level < 10)
			{
				pMenu->m_dwColor = D3DCOLOR_RGBA(128, 128, 128, 255);
				return TRUE;
			}			
			else if(pGame->m_csStatus.m_Info.Class == CClass::Fighter	|| 
					pGame->m_csStatus.m_Info.Class == CClass::Rogue		|| 
					pGame->m_csStatus.m_Info.Class == CClass::Mage		|| 
					pGame->m_csStatus.m_Info.Class == CClass::Acolyte	|| 
					pGame->m_csStatus.m_Info.Class == CClass::Combatant	|| 
					pGame->m_csStatus.m_Info.Class == CClass::Officiator)
			{												
				return TRUE;
			}
			else
			{				
				return FALSE;
			}
			break;
		}

		case POPUPMENU_PEACE:
		{
			CharDataNode* pSelfData = RYLCharacterDataManager::Instance()->GetSelfData();
			if ( pSelfData )
			{
				if ( pSelfData->m_bPeaceMode )
				{
					strcpy( pMenu->m_strPopupStr, CRYLStringTable::m_strString[ 519 ] );
				}
				else
				{
					strcpy( pMenu->m_strPopupStr, CRYLStringTable::m_strString[ 580 ] );
				}
			}
			break;
		}		
	}

	return TRUE;
}
