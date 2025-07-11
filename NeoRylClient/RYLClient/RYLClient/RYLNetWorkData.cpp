
#include "list.h"

#include <Item/ItemMgr.h>
#include <Skill/SkillMgr.h>

#include <Network/ClientSocket/Send/SendMove.h>
#include <Network/ClientSocket/Send/SendAttack.h>
#include <Network/ClientSocket/Send/SendSkill.h>
#include <Network/ClientSocket/Send/SendCommunity.h>
#include <Network/ClientSocket/Send/SendAuthServer.h>
#include <Network/ClientSocket/Send/SendItem.h>
#include <Network/ClientSocket/Send/SendCastle.h>
#include <Network/Packet/PacketStruct/CharCommunityPacket.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>
#include <Network/Packet/PacketStruct/UnifiedCharPacket.h>

#include "ChristmasParticle.h"
#include "ChristmasParticleManager.h"

#include "../HumanStoneStatueRespawn.h"
#include "../GUITextEdit.h"
#include "../EventHandler/gameprocess/BroadcastProcess.h"

#include "FrameTimer.h"

#include "RYLCharacterDataManager.h"
#include "RYLCreatureManager.h"

#include "RYLNetworkData.h"
#include "RYLGameData.h"
#include "RYLRaceBase.h"
#include "RYLCommunityData.h"

#include "RYLMessageBox.h"
#include "RYLStringTable.h"
#include "RYLTimer.h"
#include "RYLProfile.h"

#include "RYLSceneManager.h"
#include "RYLGameScene.h"
#include "RYLClientMain.h"

#include "RYLBattleGroundDlg.h"
#include "RYLChattingDlg.h"
#include "RYLLargeMapFrameDlg.h"
#include "RYLStreetStallDlg.h"
#include "RYLCampShopDlg.h"
#include "GMMemory.h"

ClientSocket* g_pClientSocket; // 클라이언트 소켓 클래스 포인터

CRYLNetworkData* CRYLNetworkData::s_pInstance = NULL ;
long		CRYLNetworkData::m_lGMReportCounter		= 0L ;
long		CRYLNetworkData::m_lShoutCounter		= 0L ;
long		CRYLNetworkData::m_lFameInfoCounter		= 0L ;
long		CRYLNetworkData::m_lPartyFindCounter	= 0L ;
long		CRYLNetworkData::m_lPartyFindCounter2	= 60000L ;
long		CRYLNetworkData::m_lPartyCmdCounter		= 0L ;

char		CRYLNetworkData::m_strIP[MAX_PATH];
unsigned long	CRYLNetworkData::m_dwServerID = 0L ; 

CRYLNetworkData::CRYLNetworkData(GameRYL::ServiceNation eInternationalCode, 
								 GameRYL::ServerType eServerType, 
                                 const char* szGameGuardString) 
:   m_eInternationalCode(eInternationalCode),
    m_eServerType(eServerType)
{
#ifndef NO_GAMEGUARD
	m_lpNPGameLib = NULL;

	if (0 != szGameGuardString && 0 != strlen(szGameGuardString))
    {
		// edith 2009.07.29 게임가드 테스트
		//MessageBox(NULL, szGameGuardString, "GG", MB_OK);
        m_lpNPGameLib = new CNPGameLib(szGameGuardString);
    }
#endif

	m_dwPresentZone					= 0 ;
	m_dwNetworkError				= 0 ;
	m_nsNetworkStateforCastObject	= NS_NOWAITING ;

	s_pInstance	= this ;

	memset( m_aryChannel, 0, sizeof( unsigned short ) * 5 ) ;
	m_aryChannelNum = 0 ;

	m_dwNetworkFlag = NF_NOWAITING ;
    m_cLoginStatusFlag = 0;
	m_cAgentServerType = UnifiedConst::ROW;

	std::fill_n(m_strLoginID, 24, 0);
	std::fill_n(m_strLoginPass, 36, 0);
	m_bChinaBillingTime = false ;

	// 이 부분은 서버에서 값을 받아서 처리하도록 추후 수정이 필요합니다.
	switch (m_eInternationalCode)
	{
		case GameRYL::KOREA:		m_dwGameContentsFlag = GameRYL::FIFTHSKILL_LOCKITEM | GameRYL::LEVEL_LIMIT_100 | GameRYL::SIEGE | GameRYL::ACCESSORY | GameRYL::JUDGE_PROTOTYPE | GameRYL::STONE_BATTLE;																break;
		// 김재연 S그레이드 검출되게
//		case GameRYL::GLOBAL:		m_dwGameContentsFlag = GameRYL::REBALANCE_OVERITEM | GameRYL::FIFTHSKILL_LOCKITEM | GameRYL::LEVEL_LIMIT_90 | GameRYL::SIEGE | GameRYL::ACCESSORY | GameRYL::JUDGE_PROTOTYPE | GameRYL::STONE_BATTLE;																break;
		// 아래는 원본
		case GameRYL::GLOBAL:		m_dwGameContentsFlag = GameRYL::FIFTHSKILL_LOCKITEM | GameRYL::LEVEL_LIMIT_100 | GameRYL::SIEGE | GameRYL::ACCESSORY | GameRYL::JUDGE_PROTOTYPE | GameRYL::STONE_BATTLE;																break;
		case GameRYL::CHINA:		m_dwGameContentsFlag = GameRYL::FIFTHSKILL_LOCKITEM | GameRYL::LEVEL_LIMIT_100 | GameRYL::SIEGE | GameRYL::ACCESSORY | GameRYL::JUDGE_PROTOTYPE | GameRYL::STONE_BATTLE;																break;
		case GameRYL::THAILAND:		m_dwGameContentsFlag = GameRYL::FIFTHSKILL_LOCKITEM | GameRYL::LEVEL_LIMIT_100 | GameRYL::SIEGE | GameRYL::ACCESSORY | GameRYL::JUDGE_PROTOTYPE | GameRYL::STONE_BATTLE;																break;
		case GameRYL::JAPAN:		m_dwGameContentsFlag = GameRYL::FIFTHSKILL_LOCKITEM | GameRYL::LEVEL_LIMIT_100 | GameRYL::SIEGE | GameRYL::ACCESSORY | GameRYL::JUDGE_PROTOTYPE | GameRYL::STONE_BATTLE;																break;
	}

#ifdef _PACKET_PROFILE_
	m_lpPacketProfileMgr = new CPacketProfileMgr;
#else
	m_lpPacketProfileMgr = NULL;
#endif
}

CRYLNetworkData::~CRYLNetworkData() 
{
	if (NULL != m_lpPacketProfileMgr)
	{
		delete m_lpPacketProfileMgr;
		m_lpPacketProfileMgr = NULL;
	}

#ifndef NO_GAMEGUARD
	if ( NULL != m_lpNPGameLib )
	{
		delete m_lpNPGameLib ;
		m_lpNPGameLib = NULL ;
	}
#endif

#ifdef ON_HACKSHIELD
	StopHShield();
	UnInstHShield();
#endif

}

VOID	CRYLNetworkData::LinkCharacterData() 
{
	g_CharacterData.SendMovingPacket		= &CRYLNetworkData::SendMovingPacket ;
	g_CharacterData.SendCharRespawn			= &CRYLNetworkData::SendCharRespawn ;
	g_CharacterData.SendMoveUpdatePacket	= &CRYLNetworkData::SendMoveUpdatePacket ;
	g_CharacterData.SendCharAttack			= &CRYLNetworkData::SendCharAttack ;
	g_CharacterData.SendSummonAttack		= &CRYLNetworkData::SendSummonAttack ;
}

BOOL	CRYLNetworkData::SendMovingPacket( unsigned long dwUpperAni, unsigned long dwLowerAni, float fDir, vector3 &vecMove )
{
    RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
    if ( !pSelfCreature )
        return FALSE;

    CNetworkPos newPos;
    newPos.Initialize(vecMove.x / 100, vecMove.y / 100, vecMove.z / 100, fDir, 0);

    unsigned char cUAct = static_cast<unsigned char>(dwUpperAni);
    unsigned char cLAct = static_cast<unsigned char>(dwLowerAni);

    CRYLNetworkData* lpNetData = CRYLNetworkData::Instance();

    unsigned long dwCurrentTime = timeGetTime();

	// edith 2008.03.06 패킷을 1초에 1번 보내게 되어있다.
    if (lpNetData->m_LastNetPos != newPos || 
        lpNetData->m_cLastUAct != cUAct || 
        lpNetData->m_cLastLAct != cLAct ||
        1000 < dwCurrentTime - lpNetData->m_dwLastSentTime)
    {
        // 위치/방향이 변경되었거나 Act가 변경되면 전송한다.
        SendPacket::CharMoveEx(*g_pClientSocket, 
            lpNetData->m_dwMyChrID, newPos, cUAct, cLAct);

        lpNetData->m_LastNetPos     = newPos;
        lpNetData->m_cLastUAct      = cUAct;
        lpNetData->m_cLastLAct      = cLAct;
        lpNetData->m_dwLastSentTime = dwCurrentTime;
    }

    return TRUE ;
}

BOOL	CRYLNetworkData::SendCharRespawn( unsigned long dwChrID )
{
	CRYLGameData*	pGame = CRYLGameData::Instance() ;

	if (CRYLNetworkData::Instance()->m_dwPresentZone == SERVER_ID::ZONE3 || 
		CRYLNetworkData::Instance()->m_dwPresentZone == SERVER_ID::BATTLE_ZONE) 
	{
		// 전투대기 창이 떻있는 상태에서 프로그래스바가 진행중일때.
		CRYLGameScene*			pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
		CRYLBattleGroundDlg*	pBattleDlg = ( CRYLBattleGroundDlg* )pScene->GetBattleGroundDlg() ;
		CRYLLargeMapFrameDlg*	pMapDlg = ( CRYLLargeMapFrameDlg* )pScene->GetLargeMapDlg() ;

		BOOL bRespawnWait   = pBattleDlg->GetRespawnWait() ;
		BOOL bRespawnWindow = CRYLCommunityData::Instance()->GetStoneStatueRespawn() ;

		if ( !bRespawnWindow && !bRespawnWait && !pMapDlg->GetShowLargeMap() )
		{
			SendPacket::CharRespawnWaitQueue( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID ) ;
		}
		return FALSE ;
	} 
/*
	else if ( CRYLNetworkData::Instance()->m_dwPresentZone == SERVER_ID::ZONE4)
	{
		CRYLMessageBox *lpMessageBox ;
		lpMessageBox = new CRYLMessageBox ;
		lpMessageBox->SetWordBreak( TRUE ) ;
		lpMessageBox->Create( CRYLStringTable::m_strString[405], MB_YES | MB_NO | MB_EXIT ) ;
		lpMessageBox->SetResult( &pGame->m_dwRespawnResult ) ;
		pGame->m_dwRespawnResult = 0 ;

		return FALSE ;
	} 
*/
	else if ( CRYLNetworkData::Instance()->m_dwPresentZone == SERVER_ID::FRAG_ZONE || 
		CRYLNetworkData::Instance()->m_dwPresentZone == SERVER_ID::STATUE_ZONE )
    {
		CRYLNetworkData::Instance()->SetNetworkFlag( NF_RESPAWN ) ; 
        g_pClientSocket->SetStatusFlag( NS_RESPAWN ) ;
		POS psPos = { 0.0f, 0.0f, 0.0f } ;

        if ( SendPacket::CharRespawn( g_GameSession, dwChrID, ClientConstants::RespawnCmd_BGServer_Random, psPos, 0 ) )
			return TRUE ;
		else
			return FALSE ;
    }
	else
	{
		CRYLNetworkData::Instance()->SetNetworkFlag( NF_RESPAWN ) ; 
		g_pClientSocket->SetStatusFlag( NS_RESPAWN ) ;
		POS psPos = { 0.0f, 0.0f, 0.0f } ;

		if ( SendPacket::CharRespawn( g_GameSession, dwChrID, ClientConstants::RespawnCmd_Normal, psPos, 0 ) )
			return TRUE ;
		else
			return FALSE ;
	}
}

BOOL	CRYLNetworkData::SendMoveUpdatePacket( float fDir, vector3 &vecPosition )
{
	// FIXED : REMOVE UDP PACKETS
    RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
    if ( !pSelfCreature )
        return FALSE;

/*
    if ( pSelfCreature->GetSitMode() )
	{
		SendPacket::CharMoveUpdate( g_ChatSession, CRYLNetworkData::Instance()->m_dwMyChrID, true, ( LPPOS )&vecPosition, fDir ) ;
	} 
	else
	{
		SendPacket::CharMoveUpdate( g_ChatSession, CRYLNetworkData::Instance()->m_dwMyChrID, false, ( LPPOS )&vecPosition, fDir ) ;
	}
*/

	return TRUE ;
}

BOOL	CRYLNetworkData::SendCharAttack( vector3 &vecPos, float fDir, unsigned short wType, bool bCasting, 
										unsigned char cSkillLock, unsigned char cSkillLevel, unsigned char cAtCount, 
										unsigned short wDefenserNum, unsigned long dwDefenser[ 10 ], 
										bool bMissileAttack, vector3 &vecDstPos )
{
	AtNode AtList ;
	AtList.m_wDefenserNum = wDefenserNum ;
	for ( int i = 0 ; i < 10 ; i ++ )
	{
		AtList.m_dwDefenser[ i ] = dwDefenser[ i ] ; 
	}

	AtType pAtType = { 0, };

	pAtType.m_cCasting			= static_cast<unsigned char>(bCasting);
	pAtType.m_cAtCount			= cAtCount ;
	pAtType.m_cSkillLevel		= cSkillLevel ;
	pAtType.m_cSkillLockCount	= cSkillLock ;
	pAtType.m_wType				= wType ;
	pAtType.m_cMissileAttack	= static_cast<unsigned char>(bMissileAttack) ;	
	pAtType.m_DstPos.fPointX	= vecDstPos.x / 100.0f ;
	pAtType.m_DstPos.fPointY	= vecDstPos.y / 100.0f ;
	pAtType.m_DstPos.fPointZ	= vecDstPos.z / 100.0f ;

	if (pAtType.m_wType & Skill::SKILL_MASK)
	{
		if (0 == pAtType.m_cSkillLockCount && 0 == pAtType.m_cSkillLevel && Skill::Type::CHANT != CRYLGameData::Instance()->m_csStatus.m_lpQuickSelected->GetSkillType() )
		{
			// 쨉캐스팅 보정 (전투룰 밸런스를 위해서...)
			pAtType.m_cSkillLevel = 1;
		}
	}

	POS psPos ;
	psPos.fPointX = vecPos.x / 100.0f ;
	psPos.fPointY = vecPos.y / 100.0f ;
	psPos.fPointZ = vecPos.z / 100.0f ;

	if ( SendPacket::CharAttack( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, &psPos, fDir, pAtType, AtList ) )
		return TRUE ;
	else
		return FALSE ;
}

BOOL	CRYLNetworkData::SendSummonAttack( unsigned long dwTargetID )
{
	if ( g_CharacterData.m_bSummon )
	{
		SendPacket::CharSummonCmd( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, ClientConstants::SummonCmd_Attack, dwTargetID ) ;
	}

	g_CharacterData.m_bSummonCmd = FALSE ;

	return TRUE ;
}

BOOL	CRYLNetworkData::SendChatMessage(const char *strName, const char *strMessage, unsigned long dwCommand, unsigned short shLang)
{
	char			strNameMsg[MAX_PATH];
	CRYLGameData*	pGame = CRYLGameData::Instance() ;

	// 운영자는 외치기 시간 제한을 체크하지 않는다.
	if (ADMIN_NONE == g_ClientMain.m_wAdminMode)
	{
		if ((dwCommand == PktChat::SHOUT || dwCommand == PktChat::TRADE) && CRYLNetworkData::m_lShoutCounter > 0) 
		{
			return FALSE;
		}
		if (dwCommand == PktChat::GMREPORT && CRYLNetworkData::m_lGMReportCounter > 0) 
		{
			return FALSE;
		}
	}

	switch (dwCommand)
	{
		case PktChat::WHISPER:
			sprintf(strNameMsg, "(to. %s) %s", strName, strMessage);
			break;

		case PktChat::STALL:
		case PktChat::CAMP_SHOP:
			{
				char strName[16];
				memcpy(strName, pGame->m_csStatus.m_Info.Name, 16);
				strName[15] = 0;
				sprintf(strNameMsg, "%s : %s", strName, strMessage);
			}
			break;

		case PktChat::DICE:
			{
                RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
                if ( !pSelfCreature )
                    return false;

                CharDataNode* pSelfData = RYLCharacterDataManager::Instance()->GetCharData( pSelfCreature->GetCharID() );
				if ( pSelfData )
				{
					char strName[16];
					memcpy(strName, pGame->m_csStatus.m_Info.Name, 16);
					strName[15] = 0;
					sprintf(strNameMsg, "%s %s %s", strName, strMessage, strMessage);
				}
			}
			break;

		default:
			{
                RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
                if ( !pSelfCreature )
                    return false;

                CharDataNode* pSelfData = RYLCharacterDataManager::Instance()->GetCharData( pSelfCreature->GetCharID() );
				if ( pSelfData )
				{
					char strName[16];
					memcpy(strName, pGame->m_csStatus.m_Info.Name, 16);
					strName[15] = 0;
					sprintf(strNameMsg, "%s : %s", strName, strMessage);

					if (PktChat::FIND_PARTY != dwCommand)
					{
						pSelfData->m_strChatMessage = strNameMsg;
						pSelfCreature->m_fChatTimer = 2000.0f;
					}
				}
			}
			break;
	}

	CRYLChattingDlg* pChat = CRYLChattingDlg::Instance() ;
	switch (dwCommand)
	{
		case PktChat::DICE:
			if (!SendPacket::CharChat(g_GameSession, ClientConstants::ChatType_Dice, strNameMsg, strlen(strNameMsg), 0)) 
				return FALSE;
			break;

		case PktChat::NORMAL:
			pChat->AddChatMessage( strNameMsg, CRYLNetworkData::Instance()->m_dwMyChrID, dwCommand ) ;
			pChat->SaveText( strNameMsg, dwCommand ) ;
			if (!SendPacket::CharChat(g_GameSession, ClientConstants::ChatType_Normal, strNameMsg, strlen(strNameMsg), shLang)) 
				return FALSE;
			break;

		case PktChat::PARTY:
			pChat->AddChatMessage( strNameMsg, CRYLNetworkData::Instance()->m_dwMyChrID, dwCommand ) ;
			pChat->SaveText( strNameMsg, dwCommand ) ;
			if (!SendPacket::CharChat(g_GameSession, ClientConstants::ChatType_Party, strNameMsg, strlen(strNameMsg), 0)) 
				return FALSE;
			break;

		case PktChat::GUILD:
			pChat->AddChatMessage( strNameMsg, CRYLNetworkData::Instance()->m_dwMyChrID, dwCommand ) ;
			pChat->SaveText( strNameMsg, dwCommand ) ;
			if (!SendPacket::CharChat(g_GameSession, ClientConstants::ChatType_Guild, strNameMsg, strlen(strNameMsg), 0)) 
				return FALSE;
			break;

		case PktChat::TRADE :
			SendPacket::CharChat(g_GameSession, PktChat::TRADE, strNameMsg, strlen(strNameMsg), shLang);
			m_lShoutCounter = 15000;
			break ;

		case PktChat::SHOUT:
			SendPacket::CharChat(g_GameSession, ClientConstants::ChatType_Shout, strNameMsg, strlen(strNameMsg), shLang);
			m_lShoutCounter = 15000;
			break;

		case PktChat::GMREPORT:
			SendPacket::CharChat(g_GameSession, ClientConstants::ChatType_GMReport, strNameMsg, strlen(strNameMsg), shLang);
			m_lGMReportCounter = 15000;
			break;
			
		case PktChat::ADMIN_SHOUT:
			SendPacket::CharChat(g_GameSession, PktChat::ADMIN_SHOUT, strNameMsg, strlen(strNameMsg), shLang);
			break;

		case PktChat::STALL:
		{
			CRYLGameScene*		 pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
			CRYLStreetStallDlg*  pDlg   = ( CRYLStreetStallDlg* )pScene->GetStreetStallDlg() ;

			if (false == pDlg->SendChat(strNameMsg))
			{
				return false;
			}
			break;
		}

		case PktChat::CAMP_SHOP:
		{
			CRYLGameScene*		pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
			CRYLCampShopDlg*	pDlg   = ( CRYLCampShopDlg* )pScene->GetCampShopDlg() ;

			if (false == pDlg->SendChat(strNameMsg))
			{
				return false;
			}
			break;
		}

		case PktChat::WHISPER:	
		{
			char strMyName[16];
			memcpy(strMyName, pGame->m_csStatus.m_Info.Name, 15);
			strMyName[15] = 0;

			pChat->AddChatMessage( strNameMsg, CRYLNetworkData::Instance()->m_dwMyChrID, dwCommand ) ;
			pChat->SaveText( strNameMsg, dwCommand ) ;

			if (false == SendPacket::CharWhisper(g_GameSession, 
				(char *)strMyName, (char *)strName, (char *)strMessage, strlen(strMessage))) 
			{
				return FALSE;
			}

			break;
		}

		case PktChat::FIND_PARTY:
		{
			pChat->AddChatMessage( strNameMsg, CRYLNetworkData::Instance()->m_dwMyChrID, dwCommand ) ;
			pChat->SaveText( strNameMsg, dwCommand ) ;
			if (!SendPacket::CharChat(g_GameSession, ClientConstants::ChatType_FindParty, strNameMsg, strlen(strNameMsg), 0)) 
				return FALSE;
			break;
		}
	}

	return TRUE;
}


BOOL	CRYLNetworkData::SendCreateChar(unsigned long dwSlot, CHAR_CREATE &pCreateChar)
{
	g_pClientSocket->SetStatusFlag(NS_CREATECHAR);

	if (SendPacket::CharCreate(g_AuthSession, s_pInstance->m_dwUserID, dwSlot, pCreateChar))
		return TRUE;
	else
		return FALSE;
}

BOOL	CRYLNetworkData::SendDeleteChar(unsigned long dwSlot, unsigned long dwUID, char *strPassword)
{
	g_pClientSocket->SetStatusFlag(NS_DELETECHAR);

	if (SendPacket::CharDelete(g_AuthSession, s_pInstance->m_dwUserID, dwUID, dwSlot, strPassword))
		return TRUE;
	else
		return FALSE;
}

// WORK_LIST 2.1 계정 국적 추가
BOOL	CRYLNetworkData::SendSelectAccountNation(unsigned char cType, unsigned char cAccountNation)
{
	g_pClientSocket->SetStatusFlag(NS_SELECTACCOUNTNATION);

	if (SendPacket::SelectAccountNation(g_AuthSession, s_pInstance->m_dwUserID, cType, cAccountNation))
		return TRUE;
	else
		return FALSE;
}


BOOL	CRYLNetworkData::SendFieldObjectPickUp(unsigned __int64 nFieldID, Item::ItemPos &pItemIndex)
{
	//g_pClientSocket->SetStatusFlag(NS_TAKEITEM);

	if (SendPacket::CharPickUp(g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, nFieldID, pItemIndex))
		return TRUE;
	else
		return FALSE;
}

BOOL	CRYLNetworkData::SendTakeItem(CItemInstance *lpTakeItem, Item::ItemPos *lpTakeIndex, unsigned char cNum)
{
	CRYLGameData*	pGame = CRYLGameData::Instance() ;

	if (lpTakeItem->m_lpItemBase->GetPos().m_cPos != TakeType::TS_DEPOSIT && lpTakeIndex->m_cPos == TakeType::TS_DEPOSIT)
	{
		if (pGame->m_csStatus.m_Info.Gold < (unsigned long)(pGame->m_csStatus.m_Info.Level * 8))//hz was 20 deposit price
		{
			CRYLMessageBox *lpMessageBox = new CRYLMessageBox;
			lpMessageBox->Create(CRYLStringTable::m_strString[401]);
			lpMessageBox->SetTimer(1000);
			return FALSE;
		}
	}

	//g_pClientSocket->SetStatusFlag(NS_TAKEITEM);

	TakeType pTakeType;
	pTakeType.m_srcPos = lpTakeItem->m_lpItemBase->GetPos();
	pTakeType.m_dstPos = *lpTakeIndex;
	pTakeType.m_cNum = cNum;

	CPacketProfileMgr* lpPacketProfileMgr = CRYLNetworkData::Instance()->m_lpPacketProfileMgr;
	if (NULL != lpPacketProfileMgr)
	{
		lpPacketProfileMgr->Send("PktTI");
	}

	if (SendPacket::CharTakeItem(g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, pTakeType))
		return TRUE;
	else
		return FALSE;
}

BOOL	CRYLNetworkData::SendTakeMaterial( unsigned long dwCampID, unsigned char cSubCmd, unsigned char cNum, Item::ItemPos TakeIndex )
{
	CPacketProfileMgr* lpPacketProfileMgr = CRYLNetworkData::Instance()->m_lpPacketProfileMgr;
	if (NULL != lpPacketProfileMgr)
	{
		lpPacketProfileMgr->Send("PktTakeMaterial");
	}

	if (SendPacket::CharTakeMaterial(g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, dwCampID, cSubCmd, cNum, TakeIndex))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL	CRYLNetworkData::SendTakeCastleJewel( unsigned long dwCastleID, unsigned char cSubCmd, unsigned short wItemID,
											  unsigned char cIndex, unsigned char cNum, Item::ItemPos TakeIndex )
{
	CPacketProfileMgr* lpPacketProfileMgr = CRYLNetworkData::Instance()->m_lpPacketProfileMgr;
	if (NULL != lpPacketProfileMgr)
	{
		lpPacketProfileMgr->Send("PktTakeCastleJewel");
	}

	if (SendPacket::CharTakeCastleJewel(g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, dwCastleID, cSubCmd, wItemID, cIndex, cNum, TakeIndex))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL	CRYLNetworkData::SendSplitItem(CItemInstance *lpTakeItem, Item::ItemPos *lpTakeIndex, unsigned char cNum)
{
	CRYLGameData*	pGame = CRYLGameData::Instance() ;

	if (lpTakeItem->m_lpItemBase->GetPos().m_cPos != TakeType::TS_DEPOSIT && lpTakeIndex->m_cPos == TakeType::TS_DEPOSIT)
	{
		if (pGame->m_csStatus.m_Info.Gold < (unsigned long)(pGame->m_csStatus.m_Info.Level * 20))
		{
			CRYLMessageBox *lpMessageBox = new CRYLMessageBox;
			lpMessageBox->Create(CRYLStringTable::m_strString[401]);
			lpMessageBox->SetTimer(1000);
			return FALSE;
		}
	}

	//g_pClientSocket->SetStatusFlag(NS_TAKEITEM);

	TakeType pTakeType;
	pTakeType.m_srcPos = lpTakeItem->m_lpItemBase->GetPos();
	pTakeType.m_dstPos = *lpTakeIndex;
	pTakeType.m_cNum = cNum;

	if (SendPacket::CharSplitItem(g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, pTakeType))
		return TRUE;
	else
		return FALSE;
}

BOOL	CRYLNetworkData::SendQuickSlotMove(CItemInstance *lpTakeItem, unsigned char cPos, unsigned char cIndex, unsigned char cNum)
{
	//g_pClientSocket->SetStatusFlag(NS_TAKEITEM);
	if(lpTakeItem == NULL)
		return FALSE;

	TakeType pTakeType;
	pTakeType.m_srcPos = lpTakeItem->m_lpItemBase->GetPos();
	pTakeType.m_dstPos.m_cPos = cPos;
	pTakeType.m_dstPos.m_cIndex = cIndex;
	pTakeType.m_cNum = cNum;

	unsigned short wSkill = lpTakeItem->m_lpItemBase->GetPrototypeID();
	unsigned char cLockCount = 0;
	unsigned char cSkillLevel = 0;

	// 스킬일 경우
	if(lpTakeItem->m_bSkill)
	{
		// 소셜모션일때 (소설모션을 따로 처리해아함)
		if(0x4000 <= wSkill && wSkill < 0x5000)
		{
			cLockCount = ((Item::CSkillItem*)lpTakeItem->m_lpItemBase)->GetMaxNumOrDurability();
			cSkillLevel = lpTakeItem->m_lpItemBase->GetNumOrDurability();
		}
	}

	if (SendPacket::CharQuickSlotMove(g_GameSession, pTakeType, wSkill, cLockCount, cSkillLevel))
		return TRUE;
	else
		return FALSE;
}

BOOL	CRYLNetworkData::SendFieldObjectPullDown(unsigned long dwNum, Item::ItemPos &pItemIndex)
{
	//g_pClientSocket->SetStatusFlag(NS_TAKEITEM);

	if (SendPacket::CharPullDown(g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, pItemIndex, dwNum))
		return TRUE;
	else
		return FALSE;
}

BOOL	CRYLNetworkData::SendSwapItem(CItemInstance *lpSrcItem, Item::ItemPos *lpSrcIndex, CItemInstance *lpDstItem)
{
	CRYLGameData*	pGame = CRYLGameData::Instance() ;

	if (lpSrcItem->m_lpItemBase->GetPos().m_cPos != TakeType::TS_DEPOSIT && lpSrcIndex->m_cPos == TakeType::TS_DEPOSIT)
	{
		if (pGame->m_csStatus.m_Info.Gold < (unsigned long)(pGame->m_csStatus.m_Info.Level * 20))
		{
			CRYLMessageBox *lpMessageBox = new CRYLMessageBox;
			lpMessageBox->Create(CRYLStringTable::m_strString[401]);
			lpMessageBox->SetTimer(1000);
			return FALSE;
		}
	}

	TakeType SrcTakeType, DstTakeType;

	if (lpSrcItem)
	{
		if (lpSrcItem->m_lpItemBase) SrcTakeType.m_srcPos = lpSrcItem->m_lpItemBase->GetPos();

		SrcTakeType.m_dstPos = *lpSrcIndex;

		if (lpSrcItem->GetIsEnableStack())
			SrcTakeType.m_cNum = lpSrcItem->GetNowDurability();
		else
			SrcTakeType.m_cNum = 1;
	} 
	else
	{
		return FALSE;
	}

	if (lpDstItem)
	{
		if (lpDstItem->m_lpItemBase) DstTakeType.m_srcPos = lpDstItem->m_lpItemBase->GetPos();

		DstTakeType.m_dstPos.m_cPos = TakeType::TS_TEMP;
		DstTakeType.m_dstPos.m_cIndex = 0;

		if (lpDstItem->GetIsEnableStack())
			DstTakeType.m_cNum = lpDstItem->GetNowDurability();
		else
			DstTakeType.m_cNum = 1;
	} 
	else
	{
		return FALSE;
	}

	if (SendPacket::CharSwapItem(g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, SrcTakeType, DstTakeType))
	{
		return TRUE;
	} 
	else
	{
		return FALSE;
	}
}

BOOL	CRYLNetworkData::SendTradeItem(unsigned char cCmd, unsigned long dwNPCID, unsigned short wTypeID, 
									   TakeType takeType, Item::ItemPos CouponPos, Item::CItem* lpItem)
{
	if (false == SendPacket::CharTradeItem(g_GameSession, 
		CRYLNetworkData::Instance()->m_dwMyChrID, dwNPCID, cCmd, wTypeID, takeType, CouponPos, lpItem))
	{
		return FALSE;
	}

	return TRUE;
}

BOOL	CRYLNetworkData::SendCharShape(unsigned long dwChrID)
{
    RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
    if ( NULL != pSelfCreature )
	{
		CRYLGameData*	pGame = CRYLGameData::Instance() ;

		pSelfCreature->SetWeaponPos( pGame->m_csStatus.m_cWeaponPos );
		pSelfCreature->SetRideFlag( pGame->m_csStatus.m_cRidePos );
		
		const char* strShape[ Item::EquipmentPos::MAX_EQUPMENT_POS ] = { 0, };
		for ( int i = 0; i < Item::EquipmentPos::MAX_EQUPMENT_POS; ++i )
		{
			if (NULL != pGame->m_csStatus.m_lstEquipment[i])
			{
				strShape[i] = g_ItemMgr.GetEquipName(pGame->m_csStatus.m_lstEquipment[i]->GetProtoTypeID());
			}
		}
	    
		pSelfCreature->SetShape( strShape, pGame->GetNetworkEquipGrade() );

		return TRUE;
	}

	return FALSE;
}

BOOL	CRYLNetworkData::SendCharSwitchHand(void)
{
    CRYLGameData*	pGame = CRYLGameData::Instance() ;

	if (SendPacket::CharSwitchHand(g_GameSession, pGame->m_csStatus.m_cWeaponPos))
		return TRUE;
	else
		return FALSE;
}

BOOL	CRYLNetworkData::SendCharSwitchRide(void)
{
    CRYLGameData*	pGame = CRYLGameData::Instance() ;

	if (SendPacket::CharSwitchRide(g_GameSession, pGame->m_csStatus.m_cRidePos))
		return TRUE;
	else
		return FALSE;
}

BOOL	CRYLNetworkData::SendFameInfo(char* name,int check)
{

	if (CRYLNetworkData::m_lFameInfoCounter <= 0)
	{
		SendPacket::CharFameInfo(g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, name,check);
	}

	return TRUE;
}


long	CRYLNetworkData::SendPartyFind(void)
{
	long lCounter = m_lPartyFindCounter;

	if (CRYLNetworkData::m_lPartyFindCounter <= 0)
	{
		SendPacket::CharPartyFind(g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID);
		m_lPartyFindCounter = 10000;
	}

	return lCounter;
}

long	CRYLNetworkData::SendPartyCmd(unsigned long dwReferenceID, unsigned short wCmd)
{
	long			lCounter = m_lPartyCmdCounter;
	CRYLGameData*	pGame = CRYLGameData::Instance() ;

	if (wCmd == ClientConstants::PartyCmd_FindParty || wCmd == ClientConstants::PartyCmd_FindMember)
	{
		SendPacket::CharPartyCmd(g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, dwReferenceID, pGame->m_csStatus.m_PartyInfo.m_dwPartyID, wCmd);
	}
	else
	{
		SendPacket::CharPartyCmd(g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, dwReferenceID, pGame->m_csStatus.m_PartyInfo.m_dwPartyID, wCmd);
		lCounter = m_lPartyFindCounter = 0;
	}

	return lCounter;
}

void	CRYLNetworkData::SendChant(CItemInstance *lpChantSkill)		// lpChantSkill이 부모스킬만 들어와야된다.
{
	CRYLGameData*	pGame = CRYLGameData::Instance() ;

    RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
    if ( !pSelfCreature )
        return;

	if (lpChantSkill)
	{
		if (Skill::Type::CHANT == lpChantSkill->GetSkillType())
		{
            if ( pSelfCreature->GetEffect()->GetChantStatus( Skill::SpellID::Hide ) )
                return;
            
			unsigned long dwDefenser[10];
			ZeroMemory(&dwDefenser, sizeof(unsigned long) * 10);

			dwDefenser[0] = CRYLNetworkData::Instance()->m_dwMyChrID;

			CRYLNetworkData::SendCharAttack( *( pSelfCreature->GetPosition() ),
                                             pSelfCreature->GetDirection() - FLOAT_PHI / 2.0f,
				                             lpChantSkill->m_lpItemBase->GetPrototypeID(), false, 
				                             lpChantSkill->GetMaxDurability(), lpChantSkill->GetNowDurability(), 1, 1, dwDefenser);

			pGame->m_csStatus.m_wUseChantSkillID = lpChantSkill->m_lpItemBase->GetPrototypeID();
		}
	} 
	else
	{
		if (0 != pGame->m_csStatus.m_wUseChantSkillID)
		{
			if (Skill::Type::CHANT == pGame->m_csStatus.m_lpQuickSelected->GetSkillType())
			{
				if ( pSelfCreature->GetEffect()->GetChantStatus( Skill::SpellID::Hide ) )
					return;

				unsigned long dwDefenser[10];
				ZeroMemory(&dwDefenser, sizeof(unsigned long) * 10);
				dwDefenser[0] = CRYLNetworkData::Instance()->m_dwMyChrID;

				CRYLNetworkData::SendCharAttack( *( pSelfCreature->GetPosition() ),
												pSelfCreature->GetDirection() - FLOAT_PHI / 2.0f,
												pGame->m_csStatus.m_wUseChantSkillID, false, 
                              					0, 0, 1, 1, dwDefenser);

				
				pGame->m_csStatus.m_wUseChantSkillID = 0;
			}
		}
	}
}

BOOL	CRYLNetworkData::SendExchangeCmd(unsigned long dwSendID, unsigned short wCmd)
{
	if (SendPacket::CharExchangeCmd(g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, dwSendID, wCmd))
		return TRUE;

	return FALSE;
}

void	CRYLNetworkData::SetChant(CItemInstance *lpChantSkill)
{
	CRYLGameData*	pGame = CRYLGameData::Instance() ;

	if ( lpChantSkill && lpChantSkill->GetSkillType() == Skill::Type::CHANT )
	{
		unsigned short wUseChantSkillID = pGame->m_csStatus.m_wUseChantSkillID ;
		if ( 0 != wUseChantSkillID )
		{
			SendChant(NULL) ;
		}

		if ( lpChantSkill->m_lpItemBase->GetPrototypeID() != wUseChantSkillID )
		{
			SendChant( pGame->m_csStatus.m_lpQuickSelected ) ;
		}
	}
	else
	{
		if ( 0 != pGame->m_csStatus.m_wUseChantSkillID )
		{
			SendChant(NULL) ;
		}
	}
}

void	CRYLNetworkData::SendClientLog(unsigned short wCmd, char *strString, char *strString2)
{
	char strText[MAX_PATH];

	static BOOL bBadStatus = TRUE;

	switch (wCmd)
	{
		case 0:				// 스테이터스 이상
			if (bBadStatus)
			{
				strcpy(strText, "[Hack Log] ");
				strcat(strText, CRYLStringTable::m_strString[391]);
				SendPacket::CharChat(g_GameSession, ClientConstants::ChatType_ClientLog, strText, strlen(strText), 0);
				bBadStatus = FALSE;
			}
			break;

		case 1:				// 욕설
			char Temp[MAX_PATH];
			strcpy(Temp, "None");
			if (!strString)
			{
				strString2 = Temp;
			}
			sprintf(strText, "[Abuse Log] ''%s'', ''%s''", strString, strString2);
			SendPacket::CharChat(g_GameSession, ClientConstants::ChatType_ClientLog, strText, strlen(strText), 0);
			break;
	}
}

BOOL	CRYLNetworkData::SendStallEnter(unsigned long dwStallChrID)
{
	CRYLGameData*	pGame = CRYLGameData::Instance() ;

	if (!pGame->m_bStreetStallEnter)
	{
		SendPacket::CharStallEnter(g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, dwStallChrID);

		return TRUE;
	} 
	else
	{
		if (pGame->m_lpInterface && pGame->m_lpInterface->GetIsTrade())
		{
			CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
			lpMessageBox->Create(CRYLStringTable::m_strString[406]);
		}
	}

	return FALSE;
}

BOOL	CRYLNetworkData::SendStallLeave(void)
{
	CRYLGameData*	pGame = CRYLGameData::Instance() ;

	if (pGame->m_bStreetStallEnter)
	{
		SendPacket::CharStallEnter(g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, 0);

		pGame->m_bStreetStallEnter = FALSE;

		return TRUE;
	}

	return FALSE;
}

BOOL	CRYLNetworkData::SendSummonCmd(unsigned char cCmd, unsigned long dwTargetID)
{
	if (g_CharacterData.m_bSummon)
	{
		SendPacket::CharSummonCmd(g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, cCmd, dwTargetID);
	}

	g_CharacterData.m_bSummonCmd = FALSE;

	return TRUE;
}

BOOL	CRYLNetworkData::ChristmasParticle( DWORD dwAttChrID, WORD wSkillID, CHAR SkillLockCount )
{
	RYLCreature *pAttCreature = RYLCreatureManager::Instance()->GetCreature( dwAttChrID ) ;
	if ( NULL != pAttCreature )
	{
		CChristmasParticle* pChristmasParticle = new CChristmasParticle;

		const Skill::ProtoType *pSkillType = g_SkillMgr.GetSkillProtoType(wSkillID);
		if (pSkillType)
		{
			pChristmasParticle->Init( 900, (char *)pSkillType[SkillLockCount].m_szEffectFileName ) ;

			CSceneManager::m_pChristmasParticleManager->AddParticle( pChristmasParticle, 
				pAttCreature->GetPosition()->x, pAttCreature->GetPosition()->y, pAttCreature->GetPosition()->z ) ;
		} 
		else
		{
			return FALSE;
		}
		return TRUE ;
	}

	return FALSE ;
}

VOID	CRYLNetworkData::ProcessCounter( FLOAT fUpdate ) 
{
	if ( m_lShoutCounter )
	{
		m_lShoutCounter -= fUpdate * 2 ;
		if ( m_lShoutCounter < 0 )  
			m_lShoutCounter = 0 ;
	}

	if ( m_lGMReportCounter )
	{
		m_lGMReportCounter -= fUpdate * 2 ;
		if ( m_lGMReportCounter < 0 )  
			m_lGMReportCounter = 0 ;
	}
	

	if ( m_lFameInfoCounter )
	{
		m_lFameInfoCounter -= fUpdate * 2 ;
		if ( m_lFameInfoCounter < 0 ) 
			m_lFameInfoCounter = 0 ;
	}

	if ( m_lPartyFindCounter )
	{
		m_lPartyFindCounter -= fUpdate * 2 ;
		if ( m_lPartyFindCounter < 0 ) 
			m_lPartyFindCounter = 0 ;
	}

	if ( m_lPartyFindCounter2 )
	{
		m_lPartyFindCounter2 -= fUpdate * 2 ;
		if ( m_lPartyFindCounter2 < 0 ) 
		{
			m_lPartyFindCounter2 = 0;
/*			// 임시로 막아두기
			if (0 == SendPartyFind())
			{
				m_lPartyFindCounter2 = 60000 ;
			}
*/
		}
	}

	if ( m_lPartyCmdCounter )
	{
		m_lPartyCmdCounter -= fUpdate * 2 ;
		if ( m_lPartyCmdCounter < 0 )
			m_lPartyCmdCounter = 0 ;
	}
}

void	CRYLNetworkData::ChecknProtect( HWND hWnd )
{
    DWORD dwResult = 0;

#ifndef NO_GAMEGUARD
    if (0 != m_lpNPGameLib && 
        NPGAMEMON_SUCCESS != (dwResult = m_lpNPGameLib->Check()))
    {        
		// 게임가드 체크를 실패했다. 적절한 종료 메시지 출력
		TCHAR msg[128];
		wsprintf(msg, CRYLStringTable::m_strString[1300], dwResult);
		MessageBox(NULL, msg, CRYLStringTable::m_strString[21], MB_OK);
		
        // 게임을 종료시킨다.
        PostMessage(hWnd, WM_DESTROY, 0, 0);
		CRYLGameData* pGame = CRYLGameData::Instance();
        if (pGame) { pGame->m_dwClientMode = CLIENT_END; }
	}	
#endif

#ifdef ON_HACKSHIELD
	// edith 2009.12.26 핵쉴드 시간체크 루틴
#endif
}

void	CRYLNetworkData::ClosenProtect()
{

}

BOOL	CRYLNetworkData::StartnProtect(void)
{
	return TRUE;
}

BOOL	CRYLNetworkData::Initnpfindsh()
{
	return TRUE;
}

void	CRYLNetworkData::SendLoginID(char *strLoginID)
{
    // 배틀로한이나, 개발자가 아닌 경우 전송
	if (GameRYL::SERVER_BATTLE_LOHAN != CRYLNetworkData::Instance()->m_eServerType && ADMIN_L3 != g_ClientMain.m_wAdminMode)
	{
#ifndef NO_GAMEGUARD
		if (NULL != m_lpNPGameLib)
		{
			// 로그인 창에서 캐릭터 로그인을 했을때 게임가드를 인증시킨다. 
			// 이 결과는 게임서버에 접속한뒤 3분후에 게임서버에서 결과를 보낸다. ParseCSAuth 에서 결과를 받는다.
			m_lpNPGameLib->Send(strLoginID);
		}
#endif
	}
}

#ifdef ON_HACKSHIELD
int __stdcall HS_CallbackProc ( long lCode, long lParamSize, void* pParam )
{
	switch(lCode)
	{
		//Engine Callback
		case AHNHS_ENGINE_DETECT_GAME_HACK:
		{
			TCHAR szMsg[255];
			_stprintf(szMsg, _T("다음 위치에서 해킹툴이 발견되어 프로그램을 종료시켰습니다.\n%s"), (char*)pParam);
			MessageBox(NULL, szMsg, "Error", MB_OK);
			//PostThreadMessage(g_dwMainThreadID, WM_QUIT, 0, 0);
			break;
		}
		//창모드 해킹툴 감지
		case AHNHS_ENGINE_DETECT_WINDOWED_HACK:
		{
			TCHAR szMsg[255];
			_stprintf(szMsg, _T("프로그램에 대하여 창모드 해킹툴이 감지되었습니다."));
			MessageBox(NULL, szMsg, "Error", MB_OK);
			//PostThreadMessage(g_dwMainThreadID, WM_QUIT, 0, 0);
			break;
		}
		//Speed 관련
		case AHNHS_ACTAPC_DETECT_SPEEDHACK:
		{
			MessageBox(NULL, _T("현재 이 PC에서 SpeedHack으로 의심되는 동작이 감지되었습니다."), "Error", MB_OK);
			//PostThreadMessage(g_dwMainThreadID, WM_QUIT, 0, 0);
			break;
		}

		//디버깅 방지 
		case AHNHS_ACTAPC_DETECT_KDTRACE:	
		case AHNHS_ACTAPC_DETECT_KDTRACE_CHANGED:
		{
			TCHAR szMsg[255];
			_stprintf(szMsg, _T("프로그램에 대하여 디버깅 시도가 발생하였습니다. (Code = %x)\n프로그램을 종료합니다."), lCode);
			MessageBox(NULL, szMsg, "Error", MB_OK);
			//PostThreadMessage(g_dwMainThreadID, WM_QUIT, 0, 0);
			break;
		}
		
		case AHNHS_ACTAPC_DETECT_AUTOMACRO:
		{
			TCHAR szMsg[255];
			_stprintf(szMsg, _T("매크로 기능으로 의심되는 동작이 감지되었습니다.. (Code = %x)\n프로그램을 종료합니다."), lCode);
			MessageBox(NULL, szMsg, "Error", MB_OK);
			//PostThreadMessage(g_dwMainThreadID, WM_QUIT, 0, 0);
			break;
		}

		// 코드 패치 감지
		case AHNHS_ACTAPC_DETECT_ABNORMAL_MEMORY_ACCESS:
		{
			TCHAR szMsg[255];
			_stprintf(szMsg, _T("다음 위치에서 메모리 접근이 감지되어 프로그램을 종료시켰습니다.\n%s"), (char*)pParam);
			MessageBox(NULL, szMsg, "Error", MB_OK);
			//PostThreadMessage(g_dwMainThreadID, WM_QUIT, 0, 0);
			break;
		}

		//그외 해킹 방지 기능 이상 
		case AHNHS_ACTAPC_DETECT_AUTOMOUSE:
		case AHNHS_ACTAPC_DETECT_DRIVERFAILED:
		case AHNHS_ACTAPC_DETECT_HOOKFUNCTION:
		case AHNHS_ACTAPC_DETECT_MESSAGEHOOK:
		case AHNHS_ACTAPC_DETECT_MODULE_CHANGE:
		case AHNHS_ACTAPC_DETECT_ENGINEFAILED:
		case AHNHS_ACTAPC_DETECT_CODEMISMATCH:
		case AHNHS_ACTAPC_DETECT_PROTECTSCREENFAILED:
		{
			TCHAR szMsg[255];
			_stprintf(szMsg, _T("해킹 방어 기능에 이상이 발생하였습니다. (Code = %x)\n프로그램을 종료합니다."), lCode);
			MessageBox(NULL, szMsg, "Error", MB_OK);
			//PostThreadMessage(g_dwMainThreadID, WM_QUIT, 0, 0);
			break;
		}
	}
	return 1;
}
#endif

BOOL CRYLNetworkData::UpdateHShield()
{
#ifdef ON_HACKSHIELD
	HKEY hMPClientReg;
	unsigned long dwReadType = 0, dwReadLens = 256;
	char strClientPath[MAX_PATH];

	LONG lResult = 0;
	char strReg[MAX_PATH];
	sprintf(strReg, "Software\\	GamaSoft\\MP-Client(MY)"); // was ROW hayzohar

	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, strReg, 0, KEY_READ, &hMPClientReg))
	{
		lResult = RegQueryValueEx(hMPClientReg, "Folder", 0, &dwReadType, (LPBYTE)strClientPath, &dwReadLens);
		RegCloseKey(hMPClientReg);
	}

	DWORD dwRet = 0; 
	CHAR szFullFilePath[MAX_PATH]; // 핵쉴드 폴더 위치를 지정합니다. 
	
	sprintf(szFullFilePath, "%s\\HShield", strClientPath);

	AHNHS_EXT_ERRORINFOA HsExtError; 
	ZeroMemory(&HsExtError, sizeof(AHNHS_EXT_ERRORINFOA)); // Monitoring 정보를 지정합니다. 

	// _AhnHS_HSUpdate 함수 호출
	dwRet = _AhnHS_HSUpdateEx( szFullFilePath,		// 핵쉴드 폴더 경로 
								1000 * 600,			// 업데이트 전체 타임 아웃 
								5750,				// 게임 코드 
								AHNHSUPDATE_CHKOPT_HOSTFILE| AHNHSUPDATE_CHKOPT_GAMECODE, 
								HsExtError, 
								1000* 20 );			// 서버 연결 타임아웃 
	
	// Ex 함수를 사용하실때는 반드시 HSUpSetEnv.exe 설정 툴로 env 파일에 
	// 게임 코드를 입력하셔야 합니다. 
	
	if ( dwRet != ERROR_SUCCESS) 
	{
		TCHAR szTitle[128];
		// 에러 처리 
		switch ( dwRet ) 
		{ 
		case HSERROR_ENVFILE_NOTREAD: 
			strcpy(szTitle, "HSERROR_ENVFILE_NOTREAD");
			break;
		case HSERROR_ENVFILE_NOTWRITE: 
			strcpy(szTitle, "HSERROR_ENVFILE_NOTWRITE");
			break;
		case HSERROR_NETWORK_CONNECT_FAIL: 
			strcpy(szTitle, "HSERROR_NETWORK_CONNECT_FAIL");
			break;
		case HSERROR_HSUPDATE_TIMEOUT: 
			strcpy(szTitle, "HSERROR_HSUPDATE_TIMEOUT");
			break;
		case HSERROR_MISMATCH_ENVFILE: 
			strcpy(szTitle, "HSERROR_MISMATCH_ENVFILE");
			break;
		case HSERROR_HOSTFILE_MODIFICATION: 
			strcpy(szTitle, "HSERROR_HOSTFILE_MODIFICATION");
			break;
		} 

		TCHAR szBuff[256];
		sprintf(szBuff, "업데이트를 실행하지 못했습니다\nError ID : %s", szTitle);

		MessageBox(NULL, szBuff, "Error", MB_OK);
		return FALSE;
	}

#endif
	return TRUE;
}

BOOL	CRYLNetworkData::InitHShield(HWND hWnd)
{
	// edith 2009.12.25 핵쉴드를 추가합니다.
    // 배틀로한이나, 개발자가 아닌 경우 처리
	if (GameRYL::SERVER_BATTLE_LOHAN != CRYLNetworkData::Instance()->m_eServerType && ADMIN_L3 != g_ClientMain.m_wAdminMode)
	{
#ifdef ON_HACKSHIELD
		int nRet = HS_ERR_OK; 

		CHAR szFullFilePath[MAX_PATH]; 
		CHAR szMsg[MAX_PATH]; 
		DWORD dwOption = 0; 
		
		// ① 핵쉴드 폴더의 EhSvc.dll 위치를 지정합니다. 
		sprintf(szFullFilePath, "%s\\HShield\\EhSvc.dll", g_ClientMain.m_strClientPath);

		// ② _AhnHS_Initialize 함수 호출에 쓰일 옵션 플래그를 정의합니다 
		dwOption = AHNHS_CHKOPT_ALL ; 
		
		// ③ _AhnHS_Initialize 함수를 호출하여 핵쉴드 서비스를 초기화 합니다. 
		nRet = _AhnHS_Initialize ( szFullFilePath, 
									HS_CallbackProc,			// 콜백함수 
									5750,						// 게임 코드 
									"15E4BF63F549784BECC7FC70", // 라이선스 키 
									dwOption,					// 옵션 플래그
									AHNHS_SPEEDHACK_SENSING_RATIO_NORMAL ); 
		
		// ④ _AhnHS_Initialize 함수의 리턴 값을 검사하여 에러 처리합니다. 
		if ( nRet != HS_ERR_OK ) 
		{ 
			switch( nRet ) 
			{ 
				case HS_ERR_ANOTHER_SERVICE_RUNNING:
				{
					MessageBox(NULL, _T("다른 게임이 실행중입니다.\n프로그램을 종료합니다."), "Error", MB_OK);
					break;
				}
				case HS_ERR_INVALID_FILES:
				{
					MessageBox(NULL, _T("잘못된 파일 설치되었습니다.\n프로그램을 재설치하시기 바랍니다."), "Error", MB_OK);
					break;
				}
				case HS_ERR_DEBUGGER_DETECT:
				{
					MessageBox(NULL, _T("컴퓨터에서 디버거 실행이 감지되었습니다.\n디버거의 실행을 중지시킨 뒤에 다시 실행시켜주시기바랍니다."), "Error", MB_OK);
					break;
				}
				case HS_ERR_NEED_ADMIN_RIGHTS:
				{
					MessageBox(NULL, _T("Admin 권한으로 실행되어야 합니다.\n프로그램을 종료합니다."), "Error", MB_OK);
					break;
				}
				case HS_ERR_COMPATIBILITY_MODE_RUNNING:
				{
					MessageBox(NULL, _T("호환성 모드로 프로그램이 실행중입니다.\n프로그램을 종료합니다."), "Error", MB_OK);
					break;				
				}
				default:
				{
					_stprintf(szMsg, _T("해킹방지 기능에 문제가 발생하였습니다.(Error Code = %x)\n프로그램을 종료합니다."), nRet);
					MessageBox(NULL, szMsg, "Error", MB_OK);
					break;
				}
			}
			return FALSE; 
		}

		SetTimer(hWnd, 255, 10000, NULL);
#endif
	}
	return TRUE;
}

BOOL	CRYLNetworkData::UnInstHShield(  )
{
	if (GameRYL::SERVER_BATTLE_LOHAN != CRYLNetworkData::Instance()->m_eServerType && ADMIN_L3 != g_ClientMain.m_wAdminMode)
	{
#ifdef ON_HACKSHIELD
		int nRet = HS_ERR_OK; 
		
		// ① _AhnHS_Uninitialize 함수를 호출하여 핵쉴드 서비스를 종료합니다. 
		nRet = _AhnHS_Uninitialize(); 
		
		if ( nRet != HS_ERR_OK ) 
		{ 
			return FALSE; 
		} 
#endif
	}
	return TRUE;
}

BOOL	CRYLNetworkData::StartHShield(HWND hWnd)
{
	if (GameRYL::SERVER_BATTLE_LOHAN != CRYLNetworkData::Instance()->m_eServerType && ADMIN_L3 != g_ClientMain.m_wAdminMode)
	{
#ifdef ON_HACKSHIELD
		int nRet = HS_ERR_OK; 
		CHAR szMsg[MAX_PATH]; 
		
		// ① _AhnHS_StartService 함수를 호출하여 핵쉴드 서비스를 시작합니다. 
		nRet = _AhnHS_StartService(); 
		
		// ② _AhnHS_StartService 함수의 리턴 값을 검사하여 에러 처리합니다. 
		if ( nRet != HS_ERR_OK ) 
		{ 
			switch ( nRet ) 
			{ 
			case HS_ERR_START_ENGINE_FAILED: 
			case HS_ERR_DRV_FILE_CREATE_FAILED: 
			case HS_ERR_REG_DRV_FILE_FAILED: 
			case HS_ERR_START_DRV_FAILED: 
			default: 
				wsprintf ( szMsg, "해킹 방지 기능에 문제가 발생 하였습니다.(%x)", nRet );
				break; 
			} 
			MessageBox( NULL, szMsg, "Error", MB_OK ); 
			return FALSE; 
		}
#endif
	}
	return TRUE;
}

BOOL	CRYLNetworkData::StopHShield(  )
{
	if (GameRYL::SERVER_BATTLE_LOHAN != CRYLNetworkData::Instance()->m_eServerType && ADMIN_L3 != g_ClientMain.m_wAdminMode)
	{
#ifdef ON_HACKSHIELD
		int nRet = HS_ERR_OK; 
		
		// ① _AhnHS_StopService 함수를 호출하여 핵쉴드 서비스를 정지합니다. 
		nRet = _AhnHS_StopService(); 
		
		if ( nRet != HS_ERR_OK ) 
		{
			return FALSE;
		}
#endif
	}
	return TRUE;
}


BOOL	CRYLNetworkData::InitNPGameLib(HWND hWnd)
{
	// edith 2009.08.11 게임가드 2.5 업그레이드
//	InitPacketProtect("RYLPART2");

    // 배틀로한이나, 개발자가 아닌 경우 처리
	if (GameRYL::SERVER_BATTLE_LOHAN != CRYLNetworkData::Instance()->m_eServerType && ADMIN_L3 != g_ClientMain.m_wAdminMode)
	{
#ifndef NO_GAMEGUARD
		if (NULL != m_lpNPGameLib)
		{
			DWORD dwResult = NPGAMEMON_SUCCESS ;
			dwResult = m_lpNPGameLib->Init() ;
			m_lpNPGameLib->SetHwnd(hWnd) ;
			
			TCHAR	msg[256] ;
			LPCSTR	lpszMsg ;

			// edith 2009.07.29 게임가드 테스트
//			sprintf(msg, "GG초기화 : %x", dwResult);
//			MessageBox(NULL, msg, "GG", MB_OK);

			switch (dwResult)
			{
				//------------------------------------------------------------------------------------------
				// Error Code  110 - 300
				//------------------------------------------------------------------------------------------
				case NPGAMEMON_ERROR_CREATE :                  // GameGuard Directory Create Error
					lpszMsg = CRYLStringTable::m_strString[ 2826 ] ;
					break ;
				
				case NPGAMEMON_ERROR_THREAD :                  // CreateThread Error
					lpszMsg = CRYLStringTable::m_strString[ 2827 ] ;
					break ;

				case NPGAMEMON_ERROR_AUTH_NPGMUP :             // npgmup.des Authentication Fail
					lpszMsg = CRYLStringTable::m_strString[ 2828 ] ;
					break ;

				case NPGAMEMON_ERROR_AUTH_GAMEMON :            // GameMon.des Authentication Fail
					lpszMsg = CRYLStringTable::m_strString[ 2829 ] ;
					break ;

				case NPGAMEMON_ERROR_AUTH_NEWUP :              // npgmup.des.new Auth Fail
					lpszMsg = CRYLStringTable::m_strString[ 2830 ] ;
					break ;

				case NPGAMEMON_ERROR_DECRYPT :                 // .ini File Decryption Fail
					lpszMsg = CRYLStringTable::m_strString[ 2831 ] ;
					break ;

				case NPGAMEMON_ERROR_CORRUPT_INI :             // Corrupt ini file Error
					lpszMsg = CRYLStringTable::m_strString[ 2832 ] ;
					break ;

				case NPGAMEMON_ERROR_CORRUPT_INI2 :            // Not match GameName in ini file Error
					lpszMsg = CRYLStringTable::m_strString[ 2833 ] ;
					break ;

				case NPGAMEMON_ERROR_NFOUND_NPGMUP :           // npgmup.des not found
					lpszMsg = CRYLStringTable::m_strString[ 2834 ] ;
					break ;

				case NPGAMEMON_ERROR_NFOUND_NEWUP :            // npgmup.des.new not found
					lpszMsg = CRYLStringTable::m_strString[ 2835 ] ;
					break ;

				case NPGAMEMON_ERROR_NFOUND_GM :               // GameMon.des not found
					lpszMsg = CRYLStringTable::m_strString[ 2836 ] ;
					break ;

				case NPGAMEMON_ERROR_COMM :                    // Communication Init Error
					lpszMsg = CRYLStringTable::m_strString[ 2837 ] ;
					break ;

				case NPGAMEMON_ERROR_EVENT :                   // GameMon Event Create Error
					lpszMsg = CRYLStringTable::m_strString[ 2838 ] ;
					break ;

				case NPGAMEMON_ERROR_NPGMUP :                  // npgmup.dll Error
					lpszMsg = CRYLStringTable::m_strString[ 2839 ] ;
					break ;

				case NPGAMEMON_ERROR_MOVE_INI :                // Move ini Error
					lpszMsg = CRYLStringTable::m_strString[ 2840 ] ;
					break ;

				case NPGAMEMON_ERROR_MOVE_NEWUP :              // Move npgmup.des.new Error
					lpszMsg = CRYLStringTable::m_strString[ 2841 ] ;
					break ;

				case NPGAMEMON_ERROR_GAMEMON :                 // GameMon Init Error
					lpszMsg = CRYLStringTable::m_strString[ 2842 ] ;
					break ;

				case NPGAMEMON_ERROR_SPEEDCHECK :              // SpeedCheck Init Error
					lpszMsg = CRYLStringTable::m_strString[ 2843 ] ;
					break ;

				case NPGAMEMON_ERROR_GAMEGUARD :               // GameGuard Init Error
					lpszMsg = CRYLStringTable::m_strString[ 2844 ] ;
					break ;

				case NPGAMEMON_ERROR_EXIST :                   // GameMon Already Exist
					lpszMsg = CRYLStringTable::m_strString[ 2213 ] ;
					break;
				
				case NPGAMEMON_ERROR_GAME_EXIST :              // Game Instance Already Exist
					lpszMsg = CRYLStringTable::m_strString[ 2214 ] ;
					break;
				
				case NPGAMEMON_ERROR_INIT :                    // GameMon Initialize Error
					lpszMsg = CRYLStringTable::m_strString[ 2215 ] ;
					break;
				
				case NPGAMEMON_ERROR_AUTH_GAMEGUARD :          // GameGuard.des Authentication Fail
				case NPGAMEMON_ERROR_NFOUND_GG :               // GameGuard.des not found
				case NPGAMEMON_ERROR_AUTH_INI :                // .ini Authentication Fail
				case NPGAMEMON_ERROR_NFOUND_INI :              // ini File not Found
					lpszMsg = CRYLStringTable::m_strString[ 2216 ] ;
					break;

				case NPGAMEMON_ERROR_CRYPTOAPI :               // rsabase.dll is corrupted
					lpszMsg = CRYLStringTable::m_strString[ 2217 ] ;
					break;
				
				case NPGAMEMON_ERROR_EXECUTE :                 // GameMon Execute Error
					lpszMsg = CRYLStringTable::m_strString[ 2218 ] ;
					break;
				
				case NPGAMEMON_ERROR_ILLEGAL_PRG :             // Detected a illegal program
					lpszMsg = CRYLStringTable::m_strString[ 2219 ] ;
					break;

				case NPGAMEMON_ERROR_NPSCAN :                  // npscan.des Error
					lpszMsg = CRYLStringTable::m_strString[ 2845 ] ;
					break ;
				

				//------------------------------------------------------------------------------------------
				// Error Code  310 - 400
				//------------------------------------------------------------------------------------------
				// Update Error Code
				case NPGMUP_ERROR_AUTH_INI :                   // .ini Authentication Error
				case NPGMUP_ERROR_DECRYPT :                    // .ini Decrypt Error
				case NPGMUP_ERROR_INI :                        // .ini Corrupted
					break ;

				case NPGMUP_ERROR_ABORT :                      // Canceled by User
					lpszMsg = CRYLStringTable::m_strString[ 2220 ] ;
					break;

				case NPGMUP_ERROR_CONNECT :                    // Connect to update server Fail
				case NPGMUP_ERROR_DOWNCFG :                    // Download Error
					lpszMsg = CRYLStringTable::m_strString[ 2221 ] ;
					break;

				case NPGMUP_ERROR_AUTH :                       // File Authentication Error
					lpszMsg = CRYLStringTable::m_strString[ 2222 ] ;
					break;

				default:
					// 적절한 종료 메시지 출력, UI 초기화 후라면 MessageBox 대신 게임 UI를 사용해도 됨
					wsprintf( msg, CRYLStringTable::m_strString[ 2846 ] );
					lpszMsg = msg ;
					break;
			}

			if (dwResult != NPGAMEMON_SUCCESS)
			{
				char szBuff[ 64 ] ;
				sprintf( szBuff, "GameGuard Error[ Error Code : %ld ]", dwResult ) ;
				MessageBox( hWnd, lpszMsg, szBuff, MB_OK ) ; // 첫번째 인자를 NULL로 하면 안 됨
				return FALSE ;
			}
			
			SetTimer(hWnd, 255, 10000, NULL);
		}
#endif
	}
    return TRUE ;
} 

const char*		CRYLNetworkData::GetOldServerName( int iServerIdx )
{
	switch( iServerIdx )
	{
		case 1 : return (char*)&CRYLStringTable::m_strString[ 3382 ] ;
		case 2 : return (char*)&CRYLStringTable::m_strString[ 3383 ] ;
		case 3 : return (char*)&CRYLStringTable::m_strString[ 3384 ] ;
		case 4 : return (char*)&CRYLStringTable::m_strString[ 3385 ] ;
		case 5 : return (char*)&CRYLStringTable::m_strString[ 3386 ] ;
		case 6 : return (char*)&CRYLStringTable::m_strString[ 3387 ] ;
		default : return (char*)&CRYLStringTable::m_strString[ 3388 ] ;
	}
	return NULL ;
}

const char*		CRYLNetworkData::GetZoneName( char cZone )
{
	switch (cZone)
	{
		case SERVER_ID::ZONE1:              return CRYLStringTable::m_strString[1154];		// 그랜드 코스트
		case SERVER_ID::ZONE2:              return CRYLStringTable::m_strString[1155];    	// 레한 스팟
		case SERVER_ID::ZONE3:				return CRYLStringTable::m_strString[1156];    	// 배틀 그라운드
		case SERVER_ID::ZONE4:              return CRYLStringTable::m_strString[1157];    	// 사이너 아일랜드
		case SERVER_ID::ZONE5:              return CRYLStringTable::m_strString[1158];    	// 아일랜드 오브 페인
		case SERVER_ID::BATTLE_ZONE:        return CRYLStringTable::m_strString[1159];    	// 배틀 존
//		case SERVER_ID::ADMIN_TOOL_ZONE:    return CRYLStringTable::m_strString[1154];    	// (클라이언트 사정으로 쓰이지 않음) - 운영툴이 사용하겠소.
		case SERVER_ID::CAPITAL:            return CRYLStringTable::m_strString[1160];    	// 카나번
		case SERVER_ID::ZONE9:				return CRYLStringTable::m_strString[1166];    	// 다크 카나번
		case SERVER_ID::FRAG_ZONE:          return CRYLStringTable::m_strString[2325];    	// 배틀 그라운드 전용 서버의 가상 존
		case SERVER_ID::BATTLE_SERVER:      return CRYLStringTable::m_strString[1163];    	// 배틀 그라운드 전용 서버
		case SERVER_ID::ZONE12:             return CRYLStringTable::m_strString[2734];    	// 신의 대지
		case SERVER_ID::STATUE_ZONE:        return CRYLStringTable::m_strString[2326];    	// 배틀 그라운드 전용 서버의 가상 존
		case SERVER_ID::STONE_WAR1:			return CRYLStringTable::m_strString[1156];		// 다크 카나번
		case SERVER_ID::STONE_WAR2:			return CRYLStringTable::m_strString[1156];		// 다크 카나번
		case SERVER_ID::STONE_WAR3:			return CRYLStringTable::m_strString[1156];		// 다크 카나번
	}

	return "Unknown";
}
