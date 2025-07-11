#include <winsock2.h>

#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>
#include <Network/Packet/PacketStruct/CastlePacket.h>
#include <Network/ClientSocket/Parse/ParseCastle.h>
#include <Network/ClientSocket/Send/SendEtc.h>

#include <Creature/Monster/MonsterMgr.h>
#include <Creature/Siege/BroadCastSiegeObjectData.h>
#include <Creature/EnemyCheck.h>
#include <Castle/CastleConstants.h>
#include <Castle/CastleBlessMgr.h>

#include <Community/Guild/GuildConstants.h>

#include <Item/ItemFactory.h>

#include "../GameHandler.h"

#include "RYLMessageBox.h"
#include "RYLStringTable.h"

#include "RYLCastleManager.h"
#include "RYLCastle.h"
#include "RYLCampManager.h"
#include "RYLCamp.h"

#include "RYLGameTimeManager.h"

#include "RYLRaceBase.h"
#include "RYLNetworkData.h"
#include "RYLCommunityData.h"
#include "RYLGameData.h"
#include "RYLSceneManager.h"
#include "RYLGameScene.h"
#include "RYLClientMain.h"

#include "RYLChattingDlg.h"
#include "RYLNoticeManager.h"
#include "RYLCastleDoorblockDlg.h"
#include "RYLGuildFrameDlg.h"
#include "RYLGuildInfoPanel.h"
#include "RYLGuildPropertyPanel.h"
#include "RYLGuildMemberPanel.h"
#include "RYLGuildTacticsPanel.h"
#include "RYLMiningCampDlg.h"
#include "RYLCampShopDlg.h"
#include "RYLCastleMineralDlg.h"
#include "RYLSiegeTimeDlg.h"
#include "RYLArmsHPDlg.h"

#include "RYLWorldWeaponLaunchDlg.h"
#include "RYLWorldWeaponLoadDlg.h"

#include "RYLSiegeTimeDlg.h"

#include "BroadcastProcess.h"
#include "CastleProcess.h"

#include "GUITooltipItem.h"

#include "SoundMgr.h"
#include "BgmManager.h"

#include "RYLLargeMapFrameDlg.h"
#include "RYLLargeMapDlg.h"
#include "RYLDialogControlDlg.h"

#include "RylGuildWarPenaltyDlg.h"


//#include "WinInput.h"
#include "GMMemory.h"

GAME_EVENT_FUNC(ParseCharCreateCastle);
GAME_EVENT_FUNC(ParseCharCreateCamp);
GAME_EVENT_FUNC(ParseCharCharCreateSiegeArms);
GAME_EVENT_FUNC(ParseCharCastleCmd);
GAME_EVENT_FUNC(ParseCharCampCmd);
GAME_EVENT_FUNC(ParseCharCharSiegeArmsCmd);
GAME_EVENT_FUNC(ParseCharSiegeBroadCast);
GAME_EVENT_FUNC(ParseCharSiegeBroadCast2nd);
GAME_EVENT_FUNC(ParseCharNewSiegeBroadCast);
GAME_EVENT_FUNC(ParseCastleRight);
GAME_EVENT_FUNC(ParseCampRight);
GAME_EVENT_FUNC(ParseGameTimeInfo);
GAME_EVENT_FUNC(ParseStealthInfo);
GAME_EVENT_FUNC(ParseCampMessage);
GAME_EVENT_FUNC(ParseMiningCampMineralInfo);
GAME_EVENT_FUNC(ParseTakeMaterial);
GAME_EVENT_FUNC(ParseCastleSiegeCount);
GAME_EVENT_FUNC(ParseCastleTaxMove);
GAME_EVENT_FUNC(ParseTakeCastleJewel);
GAME_EVENT_FUNC(ParseCastleMineralInfo);
GAME_EVENT_FUNC(ParseWarOnOff);
GAME_EVENT_FUNC(ParseRealmPoint);
GAME_EVENT_FUNC(ParseStatueDisplayInfo);



bool GameProcess::RegisterCastleHandler(CGameEventHandler& GameEventHandler)
{
	unsigned long dwErrorCount = 0;

    dwErrorCount += GameEventHandler.AddHandler(CmdCreateCastle, ParseCharCreateCastle) ? 0 : 1;
    dwErrorCount += GameEventHandler.AddHandler(CmdCreateCamp, ParseCharCreateCamp) ? 0 : 1;
    dwErrorCount += GameEventHandler.AddHandler(CmdCreateSiegeArms, ParseCharCharCreateSiegeArms) ? 0 : 1;
    dwErrorCount += GameEventHandler.AddHandler(CmdCastleCmd, ParseCharCastleCmd) ? 0 : 1;
    dwErrorCount += GameEventHandler.AddHandler(CmdCampCmd, ParseCharCampCmd) ? 0 : 1;
    dwErrorCount += GameEventHandler.AddHandler(CmdSiegeArmsCmd, ParseCharCharSiegeArmsCmd) ? 0 : 1;
	dwErrorCount += GameEventHandler.AddHandler(CmdSiegeBroadCast, ParseCharSiegeBroadCast) ? 0 : 1;
	dwErrorCount += GameEventHandler.AddHandler(CmdSiegeBroadCast2nd, ParseCharSiegeBroadCast2nd) ? 0 : 1;
	dwErrorCount += GameEventHandler.AddHandler(CmdNewSiegeBroadCast, ParseCharNewSiegeBroadCast) ? 0 : 1;
	dwErrorCount += GameEventHandler.AddHandler(CmdCastleRight,	ParseCastleRight) ? 0 : 1 ;
	dwErrorCount += GameEventHandler.AddHandler(CmdCampRight, ParseCampRight) ? 0 : 1 ;
	dwErrorCount += GameEventHandler.AddHandler(CmdGameTimeInfo, ParseGameTimeInfo) ? 0 : 1 ;
	dwErrorCount += GameEventHandler.AddHandler(CmdStealthInfo, ParseStealthInfo) ? 0 : 1 ;
	dwErrorCount += GameEventHandler.AddHandler(CmdCampMessage, ParseCampMessage) ? 0 : 1 ;
	dwErrorCount += GameEventHandler.AddHandler(CmdMiningCampMineralInfo, ParseMiningCampMineralInfo) ? 0 : 1 ;
	dwErrorCount += GameEventHandler.AddHandler(CmdTakeMaterial, ParseTakeMaterial) ? 0 : 1 ;
	dwErrorCount += GameEventHandler.AddHandler(CmdCastleSiegeCount, ParseCastleSiegeCount) ? 0 : 1 ;
	dwErrorCount += GameEventHandler.AddHandler(CmdCastleTaxMove, ParseCastleTaxMove) ? 0 : 1 ;
	dwErrorCount += GameEventHandler.AddHandler(CmdTakeCastleJewel, ParseTakeCastleJewel) ? 0 : 1 ;
	dwErrorCount += GameEventHandler.AddHandler(CmdCastleMineralInfo, ParseCastleMineralInfo) ? 0 : 1 ;
	dwErrorCount += GameEventHandler.AddHandler(CmdWarOnOff, ParseWarOnOff) ? 0 : 1 ;
	dwErrorCount += GameEventHandler.AddHandler(CmdRealmPoint, ParseRealmPoint) ? 0 : 1 ;
	dwErrorCount += GameEventHandler.AddHandler(CmdStatueCmd, ParseStatueDisplayInfo) ? 0 : 1 ;

	
    return 0 == dwErrorCount;
}

GAME_EVENT_FUNC(ParseCharCreateCastle)
{
	unsigned long	dwCastleID ;							// CastleID
	unsigned char	cNation ;								// 소유 국가
	unsigned char	cZone ;									// 존 번호
	unsigned char	cNameID ;								// 성 이름 ID
	unsigned char	cUpgradeStep ;							// 성 상징물 업그레이드 단계
	unsigned char	cInvincibleCount ;						// 무적 횟수
	unsigned short	wTotalSiegeCount ;						// 누적 공성 횟수
	unsigned long	dwTotalTaxMoney ;						// 누적 세금 회수량
	float			fPosX, fPosY, fPosZ ;					// 성의 위치
	unsigned short	wItemID;								// 업그레이드에 사용할 보석 아이템 ID
	unsigned char	cItemNum[ Castle::EMBLEM_UPGRADE_JEWEL_POS_NUM ] ;	// 업그레이드에 사용할 보석 아이템 갯수
	CastleRight		Right ;									// 길드원들의 성 관리 권한
	CastleTaxInfo	CastleTax[ Castle::MAX_TAX_TYPE ] ;		// 성 세율/세금 정보
	
	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharCreateCastle( lpPktBase, 
																						 &dwCastleID, 
																						 &cNation, 
																						 &cZone,
																						 &cNameID,
																						 &cUpgradeStep,
																						 &cInvincibleCount,
																						 &wTotalSiegeCount,
																						 &dwTotalTaxMoney,
																						 &fPosX, &fPosY, &fPosZ,
																						 &wItemID,
																						 cItemNum,
																						 &Right,
																						 CastleTax ) ;
	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		CRYLCastle* pCastle = new CRYLCastle ;

		pCastle->SetCastleID( dwCastleID ) ;
		pCastle->SetNation( cNation ) ;
		pCastle->SetZone( cZone ) ;
		pCastle->SetCastleName( cNameID ) ;
		pCastle->SetUpgradeStep( cUpgradeStep ) ;
		pCastle->SetInvincibleCount( cInvincibleCount ) ;
		pCastle->SetTotalSiegeCount( wTotalSiegeCount ) ;
		pCastle->SetTotalTaxMoney( dwTotalTaxMoney ) ;
		pCastle->SetPos( fPosX, fPosY, fPosZ ) ;
		pCastle->SetCastleRight( Right ) ;
		pCastle->SetCastleTaxInfo( CastleTax ) ;
		pCastle->SetUpgradeItem( wItemID, cItemNum ) ;
		
		CRYLCastleManager::Instance()->InsertCastle( pCastle ) ;

		// 국기 변경.
		if(CRYLNetworkData::Instance()->m_dwPresentZone == SERVER_ID::CAPITAL)
		{			
			CSceneManager::SetNationFlag(cNation);
			CSceneManager::m_HeightField.SetNationFlag(cNation);
		}
	}
	else
	{
		// Error
	}

	return FALSE ;
}

GAME_EVENT_FUNC(ParseCharCreateCamp)
{
	unsigned long		dwActorCID ; 
	unsigned long		dwCID ;		
	unsigned long		dwCampID ;		
	unsigned long		dwGID ;		
	unsigned long		dwHP ;	
	unsigned short		wObjectType ;
	unsigned char		cState ;	
	unsigned char		cSubState ;
	unsigned char		cUpgradeStep ;
	unsigned char		cNation ;
	float				fPointX ;				
	float				fPointY ; 
	float				fPointZ ;
	unsigned char		cMaterial ;
	CampRight			campRight ;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharCreateCamp( lpPktBase, 
																					   &dwActorCID,		// 요새 생성을 요청한 캐릭터의 CID
																					   &dwCID,			// CID
																					   &dwCampID,		// 캠프 ID
																					   &dwGID,			// 캠프의 GUI
																					   &dwHP,			// 캠프 HP
																					   &wObjectType,	// 요새 타입
																					   &cState,			// 캠프 State
																					   &cSubState,		// 캠프 SubState
																					   &cUpgradeStep,	// 캠프 UpgradeStep
																					   &cNation,
																					   &campRight,		// 캠프 권한
																					   &fPointX,		// x,y,z
																					   &fPointY, 
																					   &fPointZ,
																					   &cMaterial ) ;

	if(0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		// large map data
		CRYLCamp* pCamp = new CRYLCamp ;

		pCamp->SetCID( dwCID )	;
		pCamp->SetCampID( dwCampID ) ;
		pCamp->SetGID( dwGID )	;			
		pCamp->SetHP( dwHP ) ;					
		pCamp->SetState( cState ) ;
		pCamp->SetUpgradeStep( cUpgradeStep ) ;
		pCamp->SetCampNation( cNation ) ;
		pCamp->SetPointX( fPointX ) ;
		pCamp->SetPointY( fPointY ) ;
		pCamp->SetPointZ( fPointZ ) ;
		pCamp->SetCampRight( campRight ) ;
		pCamp->UpdateMaterial( cMaterial ) ;
		
		CRYLCampManager::Instance()->InsertCamp( pCamp ) ;

		//-------------------------------------------------
		WORD wRealObjType = wObjectType
						    + ( cState  * Siege::UPGRADE_KIND_NUM )
						    + cUpgradeStep ; 
		
		const CMonsterMgr::MonsterProtoType *lpMonster = g_MonsterMgr.GetMonsterProtoType( wRealObjType ) ;
		
		if ( lpMonster )
		{
			vector3 vecPos;
			vecPos.x = fPointX * 100.0f ;
			vecPos.y = fPointY * 100.0f ;
			vecPos.z = fPointZ * 100.0f ; 

			GameProcess::LogCharLoad(dwCID, LOG_RFL); 
			RYLCreature* pBuildEnable = RYLCreatureManager::Instance()->GetCreature( dwCID );
			int iHouseType ;

			switch ( cState )
			{ 
				case Siege::DEVELOPING :	iHouseType = 2 ;  break ;
				case Siege::COMPLETE :		iHouseType = 0 ;  break ;
				default :					iHouseType = -1 ; break ;
			}

			if ( wObjectType == Siege::KARTERANT_WEAPON )
			{
				iHouseType = 3;
			}
			else if ( wObjectType == Siege::MERKADIA_WEAPON )
			{
				iHouseType = 4;
			}
			else if ( wObjectType == Siege::MINING_CAMP )
			{
				iHouseType = 5;
			}
			else if ( wObjectType == Siege::CAMP_SHOP )
			{
				iHouseType = 6;
			}
			
			RYLCreatureManager::Instance()->AddCreature( dwCID, 
										                 lpMonster->m_MonsterInfo.m_strName,
														 (char *)lpMonster->m_MonsterInfo.m_strModelingFlag, 
										                 vecPos, 
										                 0, 
										                 lpMonster->m_MonsterInfo.m_bCollision,
														 0,
										                 -1, NULL, 
										                 true, iHouseType, D3DXVECTOR3( vecPos.x, vecPos.y, vecPos.z ) ) ;
			
            RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( dwCID );
            if( !pTempCreature )
                return FALSE;

            CZ3DGeneralChrModel* pTempModel = pTempCreature->GetCharModel();
            if( !pTempModel )
                return FALSE;

            CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( dwCID );
            if( !pTempData )
                return FALSE;

            pTempCreature->SetScale( 1.0f );
			pTempModel->SetScale(1.0f);
            pTempModel->SetPosition( *( pTempCreature->GetPosition() ) );

            pTempData->m_ulGuildID  = dwGID;
            pTempData->m_lCurrHP    = dwHP;
            pTempData->m_ulMaxHP    = lpMonster->m_CreatureStatus.m_StatusInfo.m_nMaxHP;

			pTempCreature->SetObjectType( wObjectType ) ;
			pTempCreature->SetState( cState ) ;
			pTempCreature->SetSubState( cSubState );
			pTempCreature->SetUpgradeType( 0 );
			pTempCreature->SetUpgradeStep( cUpgradeStep ) ;
			pTempCreature->SetCampOrCastleID( dwCampID ) ;
			
			pTempCreature->SetAction( CA_WAIT, CA_WAIT );

			// 추가된 캐릭터들의 길드 정보 요청용 (길드의 중복 요청을 피하기 위해 자료 구조는 set)
			CRYLGameScene*		 pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
			CRYLGuildFrameDlg*	 pDlg   = ( CRYLGuildFrameDlg* )pScene->GetGuildFrameDlg() ;
			CRYLGuildInfoPanel*  pPanel = ( CRYLGuildInfoPanel* )pDlg->GetGuildInfoPanel() ;

			std::set<unsigned long>	setGID;

			if (0 != dwGID)
			{
				if (NULL == pPanel->FindGuild(dwGID & ~Guild::MEMBER_WAIT_BIT))
				{
					setGID.insert(dwGID & ~Guild::MEMBER_WAIT_BIT);

					pPanel->RequestNewGuild(setGID) ;
					setGID.clear() ;
				}
			}

			// 자신이 요청한 경우라면, 자신의 캐릭터 위치를 변경시켜준다.
			if (0 != dwActorCID && dwActorCID == CRYLNetworkData::Instance()->m_dwMyChrID)
			{
				RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
				if ( !pSelfCreature )
					return FALSE;

				// 요새를 바라보도록 방향 설정
				pSelfCreature->SetDirection( FLOAT_PHI );

				// 위치 이동
				vector3 vecPos = vector3(fPointX, 0.0f, fPointZ - 20);
				vecPos.y = CRYLGameData::Instance()->m_lpSceneManager->m_HeightField.GetHeight(vecPos) + 15.0f;
				POS psPos = { vecPos.x, vecPos.y / 100.0f, vecPos.z };
				CRYLNetworkData::Instance()->SetNetworkFlag( NF_BINDPOS ) ; 
				SendPacket::CharBindPos(g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, 1, &psPos, 0);

				// 마우스 모드라면 카메라도 갱신해준다.
				if (CRYLGameData::Instance()->m_bMouseMode)
				{
					g_CharacterData.SetCameraToCharDir();
				}
			}
		}			
	}
	else
	{
		// Error
	}

	return FALSE ;
}

GAME_EVENT_FUNC(ParseCharCharCreateSiegeArms)  
{
	unsigned long		dwCID ; 
	unsigned long		dwOwnerID ; 
	unsigned char		cNation ;
	unsigned long		dwHP ;
	unsigned short		wObjectType ;
	unsigned char		cState ;
	unsigned char		cUpgradeStep ;
	float				fPointX ;
	float				fPointY ;
	float				fPointZ ;


	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharCreateSiegeArms( lpPktBase, 
																							&dwCID, 
																							&dwOwnerID, 
																							&cNation,
																							&dwHP, 
																							&wObjectType, 
																							&cState, 
																							&cUpgradeStep,
																							&fPointX, 
																							&fPointY, 
																							&fPointZ);
	if(0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
        RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
        if( !pSelfCreature )
            return FALSE;

        CharDataNode* pSelfData = RYLCharacterDataManager::Instance()->GetSelfData();
        if( !pSelfData )
            return FALSE;

        if ( dwOwnerID == pSelfCreature->GetCharID() ) 
		{
			CRYLGameData* pGame = CRYLGameData::Instance() ;
			
			delete pGame->m_csStatus.m_lpSiegeArmsMaterial ;
			pGame->m_csStatus.m_lpSiegeArmsMaterial = NULL ;
		}
		
		vector3 vecPos;
		vecPos.x = fPointX * 100.0f ;
		vecPos.y = fPointY * 100.0f ;
		vecPos.z = fPointZ * 100.0f ; 

		switch ( wObjectType )
		{
		case SHORT_RANGE_SIEGE_ARMS	:		// 근거리 공성병기
			{
				g_CharacterData.ChangeSiegeModel( dwCID, 
                                                  pSelfData->m_strName.c_str(),
												  vecPos, 0.0f, 
												  "ShortSiegeWeapon" ) ;
			}
			break ;
		case LONG_RANGE_SIEGE_ARMS :		// 원거리 공성병기
			{
				g_CharacterData.ChangeSiegeModel( dwCID, 
												  pSelfData->m_strName.c_str(),
												  vecPos, 0.0f, 
												  "LongSeigeWeapon" ) ;
			}
			break ;
			 
		case AIRSHIP :
			{ 
				g_CharacterData.ChangeSiegeModel( dwCID, 
												  pSelfData->m_strName.c_str(),
												  vecPos, 0.0f, 
												  "DropShip" ) ;
			}
			break ;
		}

        RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( dwCID );
		
		if ( pTempCreature )
		{
            CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( dwCID );
            if( !pTempData )
                return FALSE;

            pTempData->m_cNation	= cNation;
            pTempData->m_lCurrHP	= dwHP;
            //pTempData->m_ulMaxHP	= dwMaxHP;

            pTempCreature->SetOwnerID( dwOwnerID )	;
			pTempCreature->SetObjectType( wObjectType ) ;							
			pTempCreature->SetState( cState ) ;
			//pTempCreature->SetSubState( cSubState ) ;
			pTempCreature->SetUpgradeStep( cUpgradeStep ) ;
			//pTempCreature->SetUpgradeType( cUpgradeType ) ;

			pTempCreature->SetAction( CA_WAIT, CA_WAIT );

			switch ( wObjectType )
			{
			case SHORT_RANGE_SIEGE_ARMS	:		// 근거리 공성병기
			case LONG_RANGE_SIEGE_ARMS :		// 원거리 공성병기
				if ( dwOwnerID == pSelfCreature->GetCharID() ) 
				{
					CSceneManager::CharacterCollisionDetectType( pTempCreature->GetCharModel(), CDT_FULL );
					pTempCreature->SetCDTType( CDT_FULL );
				}
				else
				{
					CSceneManager::CharacterCollisionDetectType( pTempCreature->GetCharModel(), CDT_NONE );
					pTempCreature->SetCDTType( CDT_NONE );
				}
				
				break ;
			}
		}
	}
	else
	{
		// Error
	}

	return FALSE ;
}

GAME_EVENT_FUNC(ParseCharCastleCmd)
{
	g_pClientSocket->SetStatusFlag(NS_NOWAITING);

	unsigned long dwCastleID ; 
	unsigned long dwCastleObjectID ; 
	unsigned long dwValue1 ;
	unsigned long dwValue2 ; 
	unsigned char cState ; 
	unsigned char cSubCmd ;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharCastleCmd( lpPktBase, 
																					  &dwCastleID, 
																					  &dwCastleObjectID, 
																					  &dwValue1,
																					  &dwValue2, 
																					  &cState, 
																					  &cSubCmd ) ;
	if(0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		CRYLChattingDlg* pChat = CRYLChattingDlg::Instance() ;
		CRYLGameData* pGame = CRYLGameData::Instance() ;
		switch( cSubCmd )
		{
			// 상징물 관련 명령																																																			
			case PktCastleCmd::CASTLE_SET_TAX :					// 세율 조정				(dwValue1 : 세율 타입, dwValue2 : 세율)
			{
// CASTLE_TODO : 성이 길드 소유가 아니므로 세율을 설정할 주체가 없다.

//				CRYLCastle* pCastle = CRYLCastleManager::Instance()->FindCastle( dwCastleID ) ;
//				
//				if ( pCastle )
//				{
//					pCastle->SetTax( dwValue1, dwValue2 ) ;
//
//					// 메세지 출력
//					char szBuffer[ MAX_PATH ] ;
//					switch ( dwValue1 )
//					{
//						case Castle::TRADE_TAX :			sprintf( szBuffer, CRYLStringTable::m_strString[ 2569 ], CRYLStringTable::m_strString[ 3825 ], dwValue2 ) ;			break ;
//						case Castle::REPAIR_TAX :			sprintf( szBuffer, CRYLStringTable::m_strString[ 2569 ], CRYLStringTable::m_strString[ 3068 ], dwValue2 ) ;				break ;
//						case Castle::RESMELT_TAX :			sprintf( szBuffer, CRYLStringTable::m_strString[ 2569 ], CRYLStringTable::m_strString[ 2089 ], dwValue2 ) ;					break ;
//						case Castle::CAMP_GOLD_TAX :		sprintf( szBuffer, CRYLStringTable::m_strString[ 2569 ], CRYLStringTable::m_strString[ 3826 ], dwValue2 ) ;	break ;
//						case Castle::CAMP_MINERAL_TAX :		sprintf( szBuffer, CRYLStringTable::m_strString[ 2569 ], CRYLStringTable::m_strString[ 3827 ], dwValue2 ) ;			break ;
//					}
//					pChat->AddMessage( szBuffer, CRYLNetworkData::Instance()->m_dwMyChrID, 128 ) ;
//
//					// 공지 출력
//					CRYLGameScene*	pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
//					CRYLGuildFrameDlg* pDlg = ( CRYLGuildFrameDlg* )pScene->GetGuildFrameDlg() ;
//					CRYLGuildInfoPanel* pPanel = ( CRYLGuildInfoPanel* )pDlg->GetGuildInfoPanel() ;
//					LPGUILDLARGEINFOEX lpGuildInfo = pPanel->FindGuild( pCastle->GetGuildID() ) ;
//					unsigned char cTitle = g_CastleBlessMgr.GetTitle( pCastle->GetTotalGainTaxCount(), pCastle->GetUpgradeStep() ) ;
//
//					sprintf( szBuffer, CRYLStringTable::m_strString[ 3828 ], 
//						lpGuildInfo->m_szName, pCastle->GetTitleName( cTitle ), pCastle->GetCastleName() ) ;
//					CRYLNoticeManager::Instance()->AddNotice( szBuffer, NOTICECOLOR::WAR, NOTICETIME::NORMAL, TRUE ) ;
//				}
			}
			break ;

			case PktCastleCmd::CASTLE_UPGRADE_EMBLEM :			// 상징물 업그레이드		(dwValue1 : 업그레이드 타입)
			{
// CASTLE_TODO : 성이 길드 소유가 아니므로 업그레이드를 할 주체가 없다.

//				CRYLCastle* pCastle = CRYLCastleManager::Instance()->FindCastle( dwCastleID ) ;
//				if ( pCastle )
//				{
//					pCastle->SetUpgradeItem( 0, 0 ) ;
//				}
//
//                RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( dwCastleObjectID ) ;
//				if ( pTempCreature )
//				{
//					pTempCreature->SetUpgradeType( dwValue1 ) ;
//				}
//
//				pChat->AddMessage( CRYLStringTable::m_strString[ 2570 ], CRYLNetworkData::Instance()->m_dwMyChrID, 128 ) ;
			}
			break ;

			case PktCastleCmd::CASTLE_UPGRADE_EMBLEM_COMPLETE :	// 상징물 업그레이드 완료	(dwValue1 : 업그레이드 단계)
			{
// CASTLE_TODO : 성이 길드 소유가 아니므로 업그레이드를 할 주체가 없다.

//				CRYLCastle* pCastle = CRYLCastleManager::Instance()->FindCastle( dwCastleID ) ;
//				if ( pCastle )
//				{
//					pCastle->SetUpgradeStep( dwValue1 ) ;
//				}
//
//				RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( dwCastleObjectID ) ;
//				if ( pTempCreature )
//				{
//					pTempCreature->SetUpgradeStep( dwValue1 ) ;
//				}
//
//				if ( pCastle && pTempCreature )
//				{
//					char szBuffer[ MAX_PATH ] ;
//					switch ( pTempCreature->GetUpgradeType() )
//					{
//						case Siege::RUBY :
//							sprintf( szBuffer, CRYLStringTable::m_strString[ 2571 ], pCastle->GetCastleName(), CRYLStringTable::m_strString[ 2551 ], dwValue1 ) ;
//							break ;
//
//						case Siege::EMERALD :
//							sprintf( szBuffer, CRYLStringTable::m_strString[ 2571 ], pCastle->GetCastleName(), CRYLStringTable::m_strString[ 2552 ], dwValue1 ) ;
//							break ;
//
//						case Siege::SAPPHIRE :
//							sprintf( szBuffer, CRYLStringTable::m_strString[ 2571 ], pCastle->GetCastleName(), CRYLStringTable::m_strString[ 2553 ], dwValue1 ) ;
//							break ;
//
//						case Siege::DIAMOND :
//							sprintf( szBuffer, CRYLStringTable::m_strString[ 2571 ], pCastle->GetCastleName(), CRYLStringTable::m_strString[ 2554 ], dwValue1 ) ;
//							break ;
//
//						case Siege::BLACKMOON :
//							sprintf( szBuffer, CRYLStringTable::m_strString[ 2571 ], pCastle->GetCastleName(), CRYLStringTable::m_strString[ 2555 ], dwValue1 ) ;
//							break ;
//					}
//					
//					pChat->AddMessage( szBuffer, CRYLNetworkData::Instance()->m_dwMyChrID, 128 ) ;
//				}
			}
			break ;

			case PktCastleCmd::CASTLE_UPDATE_EMBLEM :			// 상징물 부서짐			(dwValue1 : 상징물의 소유 국가)
			{
                RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( dwCastleObjectID ) ;
                CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( dwCastleObjectID );

				CRYLCastle* pCastle = CRYLCastleManager::Instance()->FindCastle( dwCastleID ) ;
				if ( pCastle )
				{
					pCastle->SetUpgradeStep( 0 ) ;

					if ( pTempCreature && pTempData )
					{
						if ( Creature::STATELESS != pTempData->m_cNation && Creature::STATELESS != dwValue1 )
						{
							char szBuffer[ MAX_PATH ] ;
							
							// 파괴시 모두 출력.
							switch(pCastle->GetNation())
							{
							case Creature::KARTERANT:
								sprintf( szBuffer, CRYLStringTable::m_strString[4109], CRYLStringTable::m_strString[2991], pCastle->GetCastleName() ) ;
								break;
							case Creature::MERKADIA:
								sprintf( szBuffer, CRYLStringTable::m_strString[4109], CRYLStringTable::m_strString[2264], pCastle->GetCastleName() ) ;
								break;
							default:
								sprintf( szBuffer, CRYLStringTable::m_strString[ 4102 ], pCastle->GetCastleName() ) ;
								break;
							}									
							
							pChat->AddMessage( szBuffer, CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;

							// 구축
							switch(dwValue1)
							{
							case Creature::KARTERANT:
								sprintf( szBuffer, CRYLStringTable::m_strString[4104], CRYLStringTable::m_strString[2991], pCastle->GetCastleName() ) ;
								break;
							case Creature::MERKADIA:
								sprintf( szBuffer, CRYLStringTable::m_strString[4104], CRYLStringTable::m_strString[2264], pCastle->GetCastleName() ) ;
								break;
							default:
								sprintf( szBuffer, CRYLStringTable::m_strString[ 4103 ], pCastle->GetCastleName() ) ;
								break;
							}	

							if ( dwValue1 == pGame->m_cAccountNation )
							{
								pChat->AddMessage( szBuffer, CRYLNetworkData::Instance()->m_dwMyChrID, 128 ) ;
							}
							else
							{
								pChat->AddMessage( szBuffer, CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
							}							
						}

						pTempCreature->SetState( cState ) ;
						pTempCreature->SetUpgradeStep( 0 ) ;
						pTempCreature->SetUpgradeType( Siege::NO_JEWEL ) ;
						pTempData->m_cNation = static_cast<unsigned char>( dwValue1 ) ;
/*
						g_CharacterData.SwitchModel( pTempCreature->GetCharID(),
													pTempCreature->GetObjectType(),
													pTempCreature->GetState(),
													pTempCreature->GetUpgradeType(),
													pTempCreature->GetUpgradeStep() ) ; 
*/
					}
				}
			}
			break ;

			case PktCastleCmd::CASTLE_CHANGE_MASTER :			// 성주 변경				(dwValue1 : 새 성주 국가)
			{
				CRYLCastle* pCastle = CRYLCastleManager::Instance()->FindCastle( dwCastleID ) ;
				if ( pCastle )
				{
					pCastle->ChangeMaster( static_cast<unsigned char>( dwValue1 ) ) ;

					char szBuffer[ MAX_PATH ];

					// 국기 변경.
					if(CRYLNetworkData::Instance()->m_dwPresentZone == SERVER_ID::CAPITAL)
					{						
						CSceneManager::SetNationFlag(dwValue1);
						CSceneManager::m_HeightField.SetNationFlag(static_cast<unsigned char>( dwValue1 ));
					}
					
					switch(pCastle->GetNation())
					{
					case Creature::KARTERANT:
						sprintf( szBuffer, CRYLStringTable::m_strString[4112], CRYLStringTable::m_strString[2991], pCastle->GetCastleName()) ;							
						break;
					case Creature::MERKADIA:
						sprintf( szBuffer, CRYLStringTable::m_strString[4112], CRYLStringTable::m_strString[2264], pCastle->GetCastleName()) ;							
						break;
					default:
						sprintf( szBuffer, CRYLStringTable::m_strString[4111], pCastle->GetCastleName());							
						break;
					}	

					if ( pCastle->GetNation() == pGame->m_cAccountNation )
					{
						pChat->AddMessage(szBuffer, CRYLNetworkData::Instance()->m_dwMyChrID, 128 ) ;
					}
					else
					{
						pChat->AddMessage(szBuffer, CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
					}					

					// 성에 속해있는 NPC 의 국가 변경
					g_CharacterData.SetNationToCastleNPC( pCastle->GetCastleID(), static_cast<unsigned char>( dwValue1 ) ) ;
				}
			}
			break ;

			case PktCastleCmd::CASTLE_TAKE_TAXMONEY :			// 세금 걷기				(dwValue1 : 세금 타입, dwValue2 : 회수한 세금)
			{
// CASTLE_TODO : 성이 길드 소유가 아니므로 세금을 걷을 수 없다.

				// 길드 금고로 회수
//				CRYLCastle* pCastle = CRYLCastleManager::Instance()->FindCastle( dwCastleID ) ;
//				if ( pCastle )
//				{
//					if ( Castle::CAMP_MINERAL_TAX == dwValue1 )
//					{
//						pCastle->SetTotalTaxMoney( pCastle->GetTotalTaxMoney() + dwValue2 ) ;
//					}
//					else
//					{
//						pCastle->SetTaxMoney( dwValue1, pCastle->GetTaxMoney( dwValue1 ) - dwValue2 ) ;
//						pCastle->SetTotalTaxMoney( pCastle->GetTotalTaxMoney() + dwValue2 ) ;
//
//						CRYLGameScene*				pScene		= ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
//						CRYLGuildFrameDlg*			pDlg		= ( CRYLGuildFrameDlg* )pScene->GetGuildFrameDlg() ;
//						CRYLGuildPropertyPanel*		pProPanel	= ( CRYLGuildPropertyPanel* )pDlg->GetGuildPropertyPanel() ;
//						unsigned long dwCurrentGold = pProPanel->GetGuildGold() ;
//						
//						char szBuffer[ MAX_PATH ] = { 0, };
//						char szType[ MAX_PATH ] = { 0, };
//
//						switch ( dwValue1 )
//						{
//							case Castle::TRADE_TAX :		strncpy(szType, CRYLStringTable::m_strString[ 3825 ], MAX_PATH);	break;
//							case Castle::REPAIR_TAX :		strncpy(szType, CRYLStringTable::m_strString[ 3068 ], MAX_PATH);	break;
//							case Castle::RESMELT_TAX :		strncpy(szType, CRYLStringTable::m_strString[ 2089 ], MAX_PATH);	break;
//							case Castle::CAMP_GOLD_TAX :	strncpy(szType, CRYLStringTable::m_strString[ 3826 ], MAX_PATH);	break;
//						}
//
//						sprintf( szBuffer, CRYLStringTable::m_strString[ 3829 ], szType, dwValue2, dwCurrentGold ) ;
//						pChat->AddMessage( szBuffer, CRYLNetworkData::Instance()->m_dwMyChrID, 128 ) ;
//					}
//				}
			}
			break ;

			case PktCastleCmd::CASTLE_LOSE_OWNERSHIP :		// 성주 없어짐 (성 중립화)
			{
				CRYLCastle* pCastle = CRYLCastleManager::Instance()->FindCastle( dwCastleID ) ;
				if ( pCastle )
				{
					pCastle->ChangeMaster( Creature::STATELESS ) ;

					// 국기 변경.
					if(CRYLNetworkData::Instance()->m_dwPresentZone == SERVER_ID::CAPITAL)
					{						
						CSceneManager::SetNationFlag(dwValue1);
						CSceneManager::m_HeightField.SetNationFlag(Creature::STATELESS);
					}

					// 성에 속해있는 NPC 의 국적 변경
					g_CharacterData.SetNationToCastleNPC( pCastle->GetCastleID(), Creature::STATELESS ) ;
				}
			}
			break ;

            case PktCastleCmd::CASTLE_GAIN_MINERAL :	// 광물 세금 회수	(dwValue1 : 광물ID, dwValue2 : 회수한 광물 갯수)
			{
// CASTLE_TODO : 성이 길드 소유가 아니므로 광물 세금을 획득할 수 없다.

//				CRYLGameScene*			pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
//				CRYLCastleMineralDlg*	pDlg   = ( CRYLCastleMineralDlg* )pScene->GetCastleMineralDlg() ;
//				pDlg->RemoveItem( dwValue1, dwValue2 ) ; 
//
//				const Item::ItemInfo* lpItemInfo = g_ItemMgr.GetItemInfo( static_cast<unsigned short>(dwValue1) ) ;
//				if ( NULL != lpItemInfo )
//				{
//					char szBuffer[ MAX_PATH ] ;
//					sprintf( szBuffer, CRYLStringTable::m_strString[ 3830 ], lpItemInfo->m_SpriteData.m_szName, dwValue2 ) ;
//					pChat->AddMessage( szBuffer, CRYLNetworkData::Instance()->m_dwMyChrID, 128 ) ;
//				}
			}
			break ;
																																																									
																																																									
			// 성문/뒷문 관련 명령																																																		
			case PktCastleCmd::CASTLE_GATE_OPEN :				// 성문 열기				(dwValue1 : SubState(OPENED))
            {
                RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( dwCastleObjectID ) ;
				CZ3DGeneralChrModel* pTempModel = pTempCreature->GetCharModel();

				if ( pTempCreature && pTempModel )
				{
					vector3 vecPos = *( pTempCreature->GetPosition() ) ;
					float fDir = pTempModel->GetDirection() ;

					RYLCreatureManager::Instance()->FreeModel( pTempCreature->GetCharModel(), TRUE ) ;
                    pTempModel = RYLCreatureManager::Instance()->AllocModel( pTempCreature, true, 7, D3DXVECTOR3( vecPos.x, vecPos.y, vecPos.z ) ) ;
					if ( pTempModel )
					{
						pTempCreature->SetPosition( vecPos ) ;
						pTempModel->SetDirection( fDir ) ;

						pTempCreature->SetState( cState ) ;
						pTempCreature->SetSubState( dwValue1 ) ;
						pTempCreature->SetAction( "open", "open" ) ;
					}
				}
            }
            break;

			case PktCastleCmd::CASTLE_GATE_CLOSE	:			// 성문 닫기				(dwValue1 : SubState(CLOSED))
			{
                RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( dwCastleObjectID ) ;
				CZ3DGeneralChrModel* pTempModel = pTempCreature->GetCharModel();

				if ( pTempCreature && pTempModel )
				{
					vector3 vecPos = *( pTempCreature->GetPosition() ) ;
					float fDir = pTempModel->GetDirection() ;

					RYLCreatureManager::Instance()->FreeModel( pTempCreature->GetCharModel(), TRUE ) ;
					pTempModel = RYLCreatureManager::Instance()->AllocModel( pTempCreature, true, 1, D3DXVECTOR3( vecPos.x, vecPos.y, vecPos.z ) ) ;
					if ( pTempModel )
					{
						pTempCreature->SetPosition( vecPos ) ;
						pTempModel->SetDirection( fDir ) ;

						pTempCreature->SetState( cState ) ;
						pTempCreature->SetSubState( dwValue1 ) ;
						pTempCreature->SetAction( "close", "close" );
					}
				}
			}
			break ;

			case PktCastleCmd::CASTLE_GATE_PROTECT :			// 성문 막기				
			{
// CASTLE_TODO : 성문 막기 기능은 막아둔다.

//				CRYLGameScene* pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
//				pScene->GetCastleDoorblockDlg()->SetVisible( TRUE ) ;				
			}
			break ;

			case PktCastleCmd::CASTLE_GATE_PROTECT_CANCEL :		// 성문 막기 해제			
			{
// CASTLE_TODO : 성문 막기 기능은 막아둔다.

//				CRYLGameScene* pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
//				pScene->GetCastleDoorblockDlg()->SetVisible( FALSE ) ;
			}
			break ;

			case PktCastleCmd::CASTLE_GATE_PROTECT_INFO :		// 성문 막기 정보			(dwValue1 : 감소율, dwValue2 : 성문 HP)
			{
// CASTLE_TODO : 성문 막기 기능은 막아둔다.

//				CRYLCommunityData::Instance()->m_dwCastleDoorID = dwCastleObjectID ;			
//				RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( dwCastleObjectID ) ;
//				if ( pTempCreature )
//				{
//                    CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( dwCastleObjectID );
//                    if( pTempData )
//                    {
//					    pTempCreature->SetDoorBlockrate( dwValue1 ) ;
//
//                        pTempData->m_lCurrHP = dwValue2;
//                    }
//
//					CRYLGameScene* pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
//					CRYLCastleDoorblockDlg* pDlg = ( CRYLCastleDoorblockDlg* )pScene->GetCastleDoorblockDlg() ;
//					pDlg->SetData( dwValue1, dwValue2 ) ;
//					pDlg->SetVisible( TRUE ) ;
//				}
			}
			break ;

			case PktCastleCmd::CASTLE_UPGRADE_GATE :			// 성문 업그레이드
				break ;

			case PktCastleCmd::CASTLE_UPGRADE_GATE_COMPLETE :	// 성문 업그레이드 완료		(dwValue1 : 업그레이드 단계)
			{
// CASTLE_TODO : 성문을 업그레이드할 주체가 없다.

//				RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( dwCastleObjectID ) ;
//				if ( pTempCreature )
//				{
//					pTempCreature->SetUpgradeStep( dwValue1 ) ;
//				}
			}
			break ;

			case PktCastleCmd::CASTLE_REPAIR_GATE :				// 성문 수리				
				break ;
	
			case PktCastleCmd::CASTLE_REPAIR_GATE_COMPLETE :	// 성문 수리 완료			(dwValue1 : 수리된 HP)
			case PktCastleCmd::CASTLE_RESTORE_GATE_COMPLETE :	// 성문 복구 완료			(dwValue1 : 복구된 HP)
			{
                CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( dwCastleObjectID ) ;
				if ( pTempData )
				{
                    pTempData->m_lCurrHP = dwValue1;
				}
			}
			break ;

			case PktCastleCmd::CASTLE_RESTORE_GATE :			// 성문 복구				
				break ;

			case PktCastleCmd::CASTLE_DESTROY_GATE :			// 성문 파괴
			{
				CRYLCastle* pCastle = CRYLCastleManager::Instance()->FindCastle( dwCastleID ) ;
				if ( pCastle )
				{
					char szBuffer[ MAX_PATH ] ;
					sprintf( szBuffer, CRYLStringTable::m_strString[ 4105 ], pCastle->GetCastleName() ) ;
					pChat->AddMessage( szBuffer, CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
				}
			}
			break ;

			case PktCastleCmd::CASTLE_BACKDOOR :				// 뒷문 사용				
				break ;

													
			// 수성 병기 관련 명령																																																	
			case PktCastleCmd::CASTLE_CREATE_ARMS :				// 수성 병기 개발	
			{
			}
			break ;

			case PktCastleCmd::CASTLE_CREATE_ARMS_COMPLETE :	// 수성 병기 개발 완료		(dwValue1 : 병기 Type)
			{
				RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( dwCastleObjectID ) ;
				if ( pTempCreature && pTempCreature->GetOwnerID() == CRYLNetworkData::Instance()->m_dwMyChrID )
				{
                    char szBuff[ 512 ] ;
					sprintf( szBuff, CRYLStringTable::m_strString[ 4079 ], Siege::CASTLE_ARMS_LEAST_USE_TIME ) ;
					CRYLChattingDlg::Instance()->AddMessage( szBuff, CRYLNetworkData::Instance()->m_dwMyChrID, 128 ) ;
				}
			}
			break ;

			case PktCastleCmd::CASTLE_RIDE_ARMS :				// 수성 병기 탑승			
			{
				RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( dwCastleObjectID ) ;
				CRYLGameData*	pGame = CRYLGameData::Instance() ;

				pGame->m_lpInterface->ArmsRide( pTempCreature->GetObjectType(), dwValue1, dwCastleObjectID ) ;	// 근거리 공성병기 탑승

				if ( dwCastleObjectID == CRYLNetworkData::Instance()->m_dwMyChrID )
				{
					switch( pTempCreature->GetObjectType() )
					{
						case SHORT_RANGE_CASTLE_ARMS	:		// 근거리 수성병기
						{
							// 퀵슬롯에 근거리공성병기 스킬 등록
							pGame->m_csStatus.m_lpSiegeArmsSkill = new CItemInstance ;
							pGame->m_csStatus.m_lpSiegeArmsSkill->SetSkillInfo( 0x9A03, 0, 1, TakeType::TS_QSLOT, 0, 0 ) ;
							
							CItemInstance* lpQuickSkill = NULL ;
							lpQuickSkill = pGame->m_csStatus.m_lpQuickSelected ;
							pGame->m_csStatus.m_lpQuickSelected = pGame->m_csStatus.m_lpSiegeArmsSkill ;
							pGame->m_csStatus.m_lpTmpQuickSkill = lpQuickSkill ;
						}
						break ;

						case LONG_RANGE_CASTLE_ARMS :			// 원거리 수성병기
						{
							// 퀵슬롯에 근거리공성병기 스킬 등록
							pGame->m_csStatus.m_lpSiegeArmsSkill = new CItemInstance ;
							pGame->m_csStatus.m_lpSiegeArmsSkill->SetSkillInfo( 0x9A04, 0, 1, TakeType::TS_QSLOT, 0, 0 ) ;
							
							CItemInstance* lpQuickSkill = NULL ;
							lpQuickSkill = pGame->m_csStatus.m_lpQuickSelected ;
							pGame->m_csStatus.m_lpQuickSelected = pGame->m_csStatus.m_lpSiegeArmsSkill ;
							pGame->m_csStatus.m_lpTmpQuickSkill = lpQuickSkill ;
						}
						break ;
					}

					CRYLGameScene*	pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
					CRYLArmsHPDlg*	pDlg   = ( CRYLArmsHPDlg* )pScene->GetArmsHPDlg() ;
					pDlg->SetVisible( TRUE ) ;

					pGame->m_lpInterface->CloseInterface();
					pGame->SetEnableMove( TRUE ) ;
				}
			}
			break ;

			case PktCastleCmd::CASTLE_GETOFF_ARMS :				// 수성 병기 내림	
			{
				RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( dwCastleObjectID ) ;
				CRYLGameData*	pGame = CRYLGameData::Instance() ;

				pGame->m_lpInterface->ArmsGetOff( dwValue1, dwCastleObjectID ) ;

				if ( dwValue1 == CRYLNetworkData::Instance()->m_dwMyChrID )
				{
					pGame->m_csStatus.m_lpQuickSelected = pGame->m_csStatus.m_lpTmpQuickSkill ;
					GM_DELETE( pGame->m_csStatus.m_lpSiegeArmsSkill ) ;

					CRYLGameScene*	pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
					CRYLArmsHPDlg*	pDlg   = ( CRYLArmsHPDlg* )pScene->GetArmsHPDlg() ;
					pDlg->SetVisible( FALSE ) ;

					// 자신이 요청한 경우라면, 자신의 캐릭터 위치를 변경시켜준다.
					RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
					if ( !pSelfCreature )
						return FALSE;

					// 위치 이동
					vector3 vecPos = *pTempCreature->GetPosition() ;
					POS psPos = { vecPos.x / 100.0f - 1.5f, (vecPos.y+100) / 100.0f, vecPos.z / 100.0f - 1.5f } ;
					CRYLNetworkData::Instance()->SetNetworkFlag( NF_BINDPOS ) ; 
					SendPacket::CharBindPos( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, 1, &psPos, 0 ) ;
				}
				else
				{
					RYLCreature* pRideCreature = RYLCreatureManager::Instance()->GetCreature( dwValue1 ) ;

					if(pRideCreature)
					{
						pRideCreature->SetRideEnable(false);
					}

					RYLCreatureManager::Instance()->DeleteArmsCreature(dwValue1);					
				}
			}
			break ;

			case PktCastleCmd::CASTLE_UPGRADE_ARMS :			// 수성 병기 업그레이드		
			{
// CASTLE_TODO : 업그레이드는 막아둔다.

//				RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( dwCastleObjectID ) ;
//				switch( pTempCreature->GetObjectType() )
//				{
//					case SHORT_RANGE_CASTLE_ARMS	:		// 근거리 수성병기
//						pChat->AddMessage( CRYLStringTable::m_strString[ 2572 ], CRYLNetworkData::Instance()->m_dwMyChrID, 128 ) ;
//						break ;
//					case LONG_RANGE_CASTLE_ARMS :			// 원거리 수성병기
//						pChat->AddMessage( CRYLStringTable::m_strString[ 2573 ], CRYLNetworkData::Instance()->m_dwMyChrID, 128 ) ;
//						break ;
//				}
			}
			break ;

			case PktCastleCmd::CASTLE_UPGRADE_ARMS_COMPLETE :	// 수성 병기 업그레이드 완료(dwValue1 : 업그레이드 단계)
			{
// CASTLE_TODO : 업그레이드는 막아둔다.

//				RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( dwCastleObjectID ) ;
//				switch( pTempCreature->GetObjectType() )
//				{
//					case SHORT_RANGE_CASTLE_ARMS	:		// 근거리 수성병기
//						pChat->AddMessage( CRYLStringTable::m_strString[ 2574 ], CRYLNetworkData::Instance()->m_dwMyChrID, 128 ) ;
//						break ;
//					case LONG_RANGE_CASTLE_ARMS :			// 원거리 수성병기
//						pChat->AddMessage( CRYLStringTable::m_strString[ 2575 ], CRYLNetworkData::Instance()->m_dwMyChrID, 128 ) ;
//						break ;
//				}
			}
			break ;

			case PktCastleCmd::CASTLE_REPAIR_ARMS :				// 수성 병기 수리			
			{
				RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( dwCastleObjectID ) ;

				if ( pTempCreature )
				{
					if(CRYLNetworkData::Instance()->m_dwMyChrID==pTempCreature->GetOwnerID())
					{
						CharDataNode* pTempData	= RYLCharacterDataManager::Instance()->GetCharData( dwCastleObjectID ) ;
						if ( pTempData )
						{
							char szBuff[ 128 ] ;
							if ( pTempCreature->GetRepairFullHP() )
							{
								sprintf( szBuff, CRYLStringTable::m_strString[ 4091 ], CRYLStringTable::MakePrintGold( dwValue1 ) ) ;
								pChat->AddMessage( szBuff, CRYLNetworkData::Instance()->m_dwMyChrID, 128 ) ;
							}
							else
							{
								unsigned long dwRepairPercent = static_cast<unsigned long>( (float)dwValue2 / (float)pTempData->m_ulMaxHP * 100.0f ) ;
								sprintf( szBuff, CRYLStringTable::m_strString[ 4092 ], dwRepairPercent, CRYLStringTable::MakePrintGold( dwValue1 ) ) ;
								pChat->AddMessage( szBuff, CRYLNetworkData::Instance()->m_dwMyChrID, 128 ) ;
							}
						}
					}
				}
			}
			break ;

			case PktCastleCmd::CASTLE_REPAIR_ARMS_COMPLETE :	// 수성 병기 수리 완료		(dwValue1 : 수리된 HP, dwValue2 : 수리된 최종 HP)
			{
				RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( dwCastleObjectID ) ;

				if ( pTempCreature )
				{
					if(CRYLNetworkData::Instance()->m_dwMyChrID==pTempCreature->GetOwnerID())
					{
						CharDataNode* pTempData	= RYLCharacterDataManager::Instance()->GetCharData( dwCastleObjectID ) ;

						if ( pTempData )
						{
							char szBuff[ 128 ] ;
							unsigned long dwRepairPercent = static_cast<unsigned long>( (float)dwValue1 / (float)pTempData->m_ulMaxHP * 100.0f ) ;
							sprintf( szBuff, CRYLStringTable::m_strString[ 4093 ], dwRepairPercent ) ;
							pChat->AddMessage( szBuff, CRYLNetworkData::Instance()->m_dwMyChrID, 128 ) ;
						}
					}
				}
			}
			break ;

			case PktCastleCmd::CASTLE_DESTROY_ARMS :			// 수성 병기 파괴			(dwValue1 : 병기 Type(NPC))
			{
				RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( dwCastleObjectID ) ;
				switch( pTempCreature->GetObjectType() )
				{
					case SHORT_RANGE_CASTLE_ARMS	:		// 근거리 수성병기
						pChat->AddMessage( CRYLStringTable::m_strString[ 2580 ], CRYLNetworkData::Instance()->m_dwMyChrID, 128 ) ;
						pTempCreature->SetOwnerID( 0 ) ;
						break ;

					case LONG_RANGE_CASTLE_ARMS :			// 원거리 수성병기
						pChat->AddMessage( CRYLStringTable::m_strString[ 2581 ], CRYLNetworkData::Instance()->m_dwMyChrID, 128 ) ;
						pTempCreature->SetOwnerID( 0 ) ;
						break ;
				}
			}
			break ;

			case PktCastleCmd::CASTLE_ARMS_CHANGE_MASTER :		// 수성 병기 성주 변경
				break ;


			// HP 업데이트
			case PktCastleCmd::CASTLE_UPDATE_HP :				// HP 를 게임서버의 값으로 업데이트 (GameServer --> DBAgentServer)
				break ;

			case PktCastleCmd::CASTLE_EMBLEM_ATTACKED :			// 성 상징물 공격 받음
			{
				RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( dwCastleObjectID ) ;

				if ( pTempCreature )
				{
					CharDataNode* pTempData	= RYLCharacterDataManager::Instance()->GetCharData( dwCastleObjectID ) ;
					if ( pTempData && Creature::STATELESS != pTempData->m_cNation &&
						 pTempData->m_cNation == CRYLGameData::Instance()->m_cAccountNation )
					{										
						CRYLCastle* pCastle = CRYLCastleManager::Instance()->FindCastle( dwCastleID ) ;
						
						if ( pCastle )
						{
							char szBuffer[ MAX_PATH ] ;
							
							switch(pTempData->m_cNation)
							{
							case Creature::KARTERANT:
								sprintf( szBuffer, CRYLStringTable::m_strString[4110], CRYLStringTable::m_strString[2991], pCastle->GetCastleName()) ;								
								break;
							case Creature::MERKADIA:
								sprintf( szBuffer, CRYLStringTable::m_strString[4110], CRYLStringTable::m_strString[2264], pCastle->GetCastleName()) ;								
								break;
							default:
								sprintf( szBuffer, CRYLStringTable::m_strString[4108], pCastle->GetCastleName()) ;																		
								break;
							}	

							pChat->AddMessage(szBuffer, CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
						}						
					}
				}
			}
			break ;
		}
	}
	else
	{
		CRYLMessageBox *lpMessageBox;
		switch (CRYLNetworkData::Instance()->m_dwNetworkError)
		{
			case PktCastleCmd::FAIL_NOT_GUILD_MEMBER :		// 길드에 가입하지 않았습니다.
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 2582 ]);
				break;
			}

			case PktCastleCmd::FAIL_NOT_EXIST_CASTLE :		// 존재 하지 않는 성
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 2583 ]);
				break;
			}

			case PktCastleCmd::FAIL_DIFFERENT_GUILD	:		// 길드 아이디가 다름
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 2584 ]);
				break;
			}

			case PktCastleCmd::FAIL_NOT_EXIST_GUILD	:		// 존재 하지 않는 길드
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 2585 ]);
				break;
			}

			case PktCastleCmd::FAIL_CASTLE_RIGHT :			// 성 관련 권한이 없습니다.
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 1240 ]);
				break;
			}

			case PktCastleCmd::FAIL_MAX_UPGRADE	:			// 최대 업그레이드 상태임
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 2586 ]);
				break;
			}

			case PktCastleCmd::FAIL_NOT_ENOUGH_JEWEL :		// 보석 갯수 부족
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 2587 ]);
				break;
			}

			case PktCastleCmd::FAIL_JEWEL_KIND :			// 적절하지 않은 보석 종류
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 2588 ]);
				break;
			}

			case PktCastleCmd::FAIL_CASTLE_OBJECT_STATE :	// 성 관련 오브젝트의 상태가 적절치 못할 경우
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 2589 ]);
				break;
			}

			case PktCastleCmd::FAIL_DIFFERENT_GOLD :		// 클라이언트와 서버의 돈이 다름
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 2590 ]);
				break;
			}

			case PktCastleCmd::FAIL_NOT_ENOUGH_GOLD	:		// 길드에 돈이 부족 -> 개인 인벤에 돈이 부족으로 변경
			{
				lpMessageBox = new CRYLMessageBox;
				//lpMessageBox->Create(CRYLStringTable::m_strString[ 2591 ]);
				lpMessageBox->Create(CRYLStringTable::m_strString[ 4078 ]);
				break;
			}

			case PktCastleCmd::FAIL_ALREADY_RIDE :			// 병기에 탑승해 있음
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 2592 ]);
				break;
			}

			case PktCastleCmd::FAIL_NOT_RIDE :				// 병기를 타고 있지 않음
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 2593 ]);
				break;
			}

			case PktCastleCmd::FAIL_CHANGE_TAX :			// 세율을 변경할수 없음
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create( CRYLStringTable::m_strString[ 3831 ] );
				break;
			}

			case PktCastleCmd::FAIL_NOT_GUILD_MASTER :		// 길드 마스터가 아니다.
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create( CRYLStringTable::m_strString[ 3540 ] );
				break;
			}

			case PktCastleCmd::FAIL_REQUEST_TOO_MUCH_GOLD :	// 너무 많은 돈을 요청했습니다.
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create( CRYLStringTable::m_strString[ 3832 ] );
				break;
			}

			case PktCastleCmd::FAIL_NOT_ENOUGH_MINERAL :	// 너무 많은 수의 광물을 요청했습니다.
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create( CRYLStringTable::m_strString[ 3833 ] );
				break;
			}

			case PktCastleCmd::FAIL_HAS_ANOTHER_CASTLE_ARMS :	// 이미 다른 수성 병기를 가지고 있습니다.
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create( CRYLStringTable::m_strString[ 4080 ] );
				break;
			}
			case PktCastleCmd::FAIL_HAS_ANOTHER_SIEGE_ARMS :	// 이미 다른 공성 병기를 가지고 있습니다.
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create( CRYLStringTable::m_strString[ 4114 ] );
				break;
			}			
			case PktCastleCmd::FAIL_NOT_MY_CASTLE_ARMS :	// 자신의 수성 병기가 아닙니다.
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create( CRYLStringTable::m_strString[ 4085 ] );
				break;
			}

			case PktCastleCmd::FAIL_NOT_SIEGE_TIME :	// 수성 병기를 생성할수 있는 시간이 아닙니다.
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create( CRYLStringTable::m_strString[ 4064 ] );
				break;
			}
			case PktCastleCmd::NOT_LIMIT_LEVEL :	// 80 레벨 미만일 경우.
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create( CRYLStringTable::m_strString[ 4118 ] );
				break;
			}
		}
	} 

	return FALSE ;
}

GAME_EVENT_FUNC(ParseCharCampCmd)
{
	unsigned long dwCID ; 
	unsigned long dwCampID ;
	unsigned char cState ; 
	unsigned long dwValue1 ;
	unsigned long dwValue2 ;
	unsigned char cSubCmd ;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharCampCmd( lpPktBase, 
																					&dwCID, 
																					&dwCampID,
																					&cState, 
																					&dwValue1, 
																					&dwValue2,
																					&cSubCmd ) ;

	if(0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		switch( cSubCmd )
		{
			case PktCampCmd::CAMP_BUILD_COMPLETE :		// 진지 구축 완료
			{
				CRYLCamp* pCamp = CRYLCampManager::Instance()->FindCamp( dwCampID ) ;
				if ( pCamp )
				{
					pCamp->SetState( cState ) ;
				}

                RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( dwCID ) ;
				if ( pTempCreature )
				{
					vector3 vecPos = *pTempCreature->GetPosition();

					pTempCreature->SetState( cState ) ;
/*					
					CSceneManager::DeleteCharacter( pTempCreature->GetCharModel() ) ;
					CZ3DGeneralChrModel* pTempModel = 
						RYLCreatureManager::Instance()->AllocModel( pTempCreature, true, 0, D3DXVECTOR3( vecPos.x, vecPos.y, vecPos.z ) );
					pTempCreature->SetCharModel( pTempModel ) ;
*/
				}

				// CharDataNode 의 이름만 변경해준다.
				CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( dwCID );
				if ( pTempCreature && pTempData )
				{
					WORD wRealObjType = pTempCreature->GetObjectType() + ( pTempCreature->GetState()  * Siege::UPGRADE_KIND_NUM ) + pTempCreature->GetUpgradeStep() ; 
					const CMonsterMgr::MonsterProtoType *lpMonster = g_MonsterMgr.GetMonsterProtoType( wRealObjType ) ;
					if ( lpMonster )
					{
						pTempData->m_strName = lpMonster->m_MonsterInfo.m_strName ;
					}
				}
			}
			break ;

			/*case PktCampCmd::CAMP_NOT_REBUILDTIME:		// 길드 요새 파괴 후 재 구축 할수 있는 시간이 지나지 않았을 경우.
			{
				CRYLChattingDlg::Instance()->AddMessage(CRYLStringTable::m_strString[0], CRYLNetworkData::Instance()->m_dwMyChrID, 128);					
			}
			break;*/

			case PktCampCmd::WORLDWEAPON_CHARGE:				// 무기 장전
			{
				RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( dwCID ) ;
				if( pTempCreature )
				{
					pTempCreature->SetSubState( Siege::WEAPON_CHARGE );
				}

				// 자재 아이템 지우기
				CRYLCamp* pCamp = CRYLCampManager::Instance()->FindCamp( dwCampID ) ;
				if ( pCamp )
				{
					pCamp->UpdateMaterial( dwValue1 ) ;
				}
/*				
				if ( pCamp )
				{
					if ( pCamp->GetMaterial() )
					{
						int nCnt = Siege::WEAPON_CHARGE_MATERIAL_NUM;
						nCnt = pCamp->GetMaterial()->GetNowDurability() - nCnt ;
						if( nCnt > 0 )
						{
							pCamp->GetMaterial()->SetNowDurability( nCnt );
						}
						else
						{
							delete pCamp->GetMaterial() ;
							pCamp->SetMaterial( NULL ) ;
						}
					}
				}
*/
			}
			break;

			case PktCampCmd::WORLDWEAPON_CHARGE_COMPLETE:		// 무기 장전 완료
			{
				RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( dwCID ) ;
				if( pTempCreature )
				{
					pTempCreature->SetSubState( Siege::WEAPON_READY );
				}
			}
			break;

			case PktCampCmd::WORLDWEAPON_FIRE:					// 무기 발사
			{
				RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( dwCID ) ;
				if( pTempCreature )
				{
					pTempCreature->SetSubState( Siege::WEAPON_FIRE );
				}
			}
			break;
	
			case PktCampCmd::WORLDWEAPON_FIRE_COMPLETE:			// 무기 발사 완료
			{
				CRYLGameScene*				pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
				CRYLWorldWeaponLaunchDlg*	pDlg   = ( CRYLWorldWeaponLaunchDlg* )pScene->GetWorldWeaponLaunchDlg(); 

				CRYLCampManager::Instance()->DeleteCamp( dwCampID );
				RYLCreatureManager::Instance()->DeleteCreature( dwCID, TRUE );
			}
			break;
	
			case PktCampCmd::CAMP_CANCEL :				// 요새 구축 해제
			case PktCampCmd::CAMP_TO_STARTKIT :			// 스타트킷으로
			case PktCampCmd::CAMP_UPGRADE :				// 요새 업그레이드		dwValue1[In] : 업그레이드 단계	dwValue2[In] : 요청한 캐릭터 CID
			case PktCampCmd::CAMP_REPAIR :				// 요새 수리			dwValue1[In] : 수리로 회복될 HP	dwValue2[In] : 요청한 캐릭터 CID
			case PktCampCmd::CAMP_UPGRADE_COMPLETE :	// 요새 업그레이드 완료	dwValue1[In] : 업그레이드 단계
			case PktCampCmd::CAMP_CHANGE_TYPE :			// 요새 변형			dwValue1[In] : 변형할 타입		dwValue2[In] : 요청한 캐릭터 CID
			{
				CRYLCamp* pCamp = CRYLCampManager::Instance()->FindCamp( dwCampID ) ;
				if ( pCamp )
				{
					pCamp->SetState( cState ) ;
				}

				RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( dwCID ) ;
				if ( pTempCreature )
				{
					pTempCreature->SetState( cState ) ;
				}

				if ( cSubCmd == PktCampCmd::CAMP_UPGRADE_COMPLETE )
				{
					if ( pCamp ) pCamp->SetUpgradeStep( dwValue1 ) ;
					if ( pTempCreature ) pTempCreature->SetUpgradeStep( dwValue1 ) ;
				}

				if ( cSubCmd == PktCampCmd::CAMP_UPGRADE )
				{
					if ( pCamp ) pCamp->SetUpgradeStep( dwValue1 ) ;
					if ( pTempCreature ) pTempCreature->SetUpgradeStep( dwValue1 ) ;

					// 자재 아이템 지우기
					if ( pCamp )
					{
						pCamp->UpdateMaterial( dwValue2 ) ;
					}
/*
					if ( pCamp && pCamp->GetMaterial() )
					{
						int nCnt = 0;
						nCnt = Siege::GetUpgradeMaterialNum( pTempCreature->GetObjectType() );
						nCnt = pCamp->GetMaterial()->GetNowDurability() - nCnt ;
						if( nCnt > 0 )
						{
							pCamp->GetMaterial()->SetNowDurability( nCnt );
						}
						else
						{
							delete pCamp->GetMaterial() ;
							pCamp->SetMaterial( NULL ) ;
						}
					}
*/
				}
				else if ( cSubCmd == PktCampCmd::CAMP_REPAIR )
				{
					// 자재 아이템 지우기
					if ( pCamp )
					{
						pCamp->UpdateMaterial( dwValue2 ) ;
					}
/*
					if ( pCamp && pCamp->GetMaterial() )
					{
						int nCnt = dwValue1 / Siege::CAMP_REPAIR_HP_PER_MATERIAL;
						if (0 != (dwValue1 % Siege::CAMP_REPAIR_HP_PER_MATERIAL)) ++nCnt;

						nCnt = pCamp->GetMaterial()->GetNowDurability() - nCnt ;
						if( nCnt > 0 )
						{
							pCamp->GetMaterial()->SetNowDurability( nCnt );
						}
						else
						{
							delete pCamp->GetMaterial() ;
							pCamp->SetMaterial( NULL ) ;
						}
					}
*/
				}
				else if( cSubCmd == PktCampCmd::CAMP_CHANGE_TYPE )
				{
					// 자재 아이템 지우기
					if ( pCamp )
					{
						pCamp->UpdateMaterial( dwValue2 ) ;
					}
/*
					if ( pCamp && pCamp->GetMaterial() )
					{
						int nCnt = 0;
						nCnt = Siege::GetChangingTypeMaterialNum( pTempCreature->GetObjectType(), dwValue1, pTempCreature->GetUpgradeStep() );
						nCnt = pCamp->GetMaterial()->GetNowDurability() - nCnt ;
						if( nCnt > 0 )
						{
							pCamp->GetMaterial()->SetNowDurability( nCnt );
						}
						else
						{
							delete pCamp->GetMaterial() ;
							pCamp->SetMaterial( NULL ) ;
						}
					}
*/
				}
			}
			break ;

			case PktCampCmd::CAMP_CANCEL_COMPLETE :			// 진지 구축 해제 완료
			case PktCampCmd::CAMP_TO_STARTKIT_COMPLETE :	// 스타트킷으로 완료
			case PktCampCmd::CAMP_DESTROY :					// 진지 파괴시 (7일간 캠프 미사용시 파괴)
			{
				CRYLCamp* pCamp = CRYLCampManager::Instance()->FindCamp( dwCampID ) ;
				RYLCreature* lpTempCreature = RYLCreatureManager::Instance()->GetCreature( dwCID ) ;
				
				GameProcess::LogCharUnload(dwCID, LOG_RFL);
                RYLCreatureManager::Instance()->DeleteCreature( dwCID, TRUE ) ;

				CRYLCampManager::Instance()->DeleteCamp( dwCampID ) ;				

				if ( cSubCmd == PktCampCmd::CAMP_CANCEL_COMPLETE )
				{
					CRYLChattingDlg::Instance()->AddMessage( 
						CRYLStringTable::m_strString[ 2595 ], CRYLNetworkData::Instance()->m_dwMyChrID, 128 );
				}
				else if ( cSubCmd == PktCampCmd::CAMP_TO_STARTKIT_COMPLETE ) 
				{
					CRYLChattingDlg::Instance()->AddMessage( 
						CRYLStringTable::m_strString[ 2596 ], CRYLNetworkData::Instance()->m_dwMyChrID, 128 );
				}
			}
			break ;
				
			case PktCampCmd::CAMP_REPAIR_COMPLETE :		// 진지 수리 완료
			{
				// cState값, dwValue1에 현재 HP
				CRYLCamp* pCamp = CRYLCampManager::Instance()->FindCamp( dwCampID ) ;
				if ( pCamp )
				{
					pCamp->SetState( cState ) ;
					pCamp->SetHP( dwValue1 ) ;
				}
				
				RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( dwCID ) ;
				if ( pTempCreature )
				{
					pTempCreature->SetState( cState ) ;

                    CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( dwCID );
                    if( pTempData )
                    {
                        pTempData->m_lCurrHP = dwValue1;
                    }
                }
			}		
			break ;
		
			case PktCampCmd::CAMP_CARE :				// 손질하기
			{
				CRYLMessageBox* lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create( CRYLStringTable::m_strString[ 2597 ],  MB_CONFIRM | MB_EXIT );
			}
			break ;
	
			case PktCampCmd::MINING_CAMP_ON_OFF :
			{
				CRYLGameScene*		 pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
				CRYLMiningCampDlg*	 pDlg   = ( CRYLMiningCampDlg* )pScene->GetMiningCampDlg(); 
				pDlg->SetCampOnOff( dwCampID, dwValue1 );
				 
				//dwValue2

			}
			break;

			case PktCampCmd::MINING_CAMP_GAIN_MINERAL :
			{
				CRYLGameScene*		 pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
				CRYLMiningCampDlg*	 pDlg   = ( CRYLMiningCampDlg* )pScene->GetMiningCampDlg(); 
				pDlg->RemoveItem( dwValue1, dwValue2 );

			}
			break;

			case PktCampCmd::CAMP_SHOP_CHANGE_TAX:		// 세율 변경 (길드 요새 상점)
			{
				CRYLGameScene*		 pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
				CRYLCampShopDlg*	 pDlg   = ( CRYLCampShopDlg* )pScene->GetCampShopDlg(); 

				pDlg->ChangeTax(dwValue1);
			}
			break;
		}
	}
	else
	{
		CRYLMessageBox* lpMessageBox = NULL;
		switch (CRYLNetworkData::Instance()->m_dwNetworkError)
		{
			case PktCampCmd::FAIL_NOT_EXIST_GUILD :			// 존재 하지 않는 길드
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 2583 ]);
				break;
			}

			case PktCampCmd::FAIL_NOT_EXIST_CAMP :			// 존재 하지 않는 성
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 2584 ]);
				break;
			}

			case PktCampCmd::FAIL_NOT_CAMP :				// 진지가 아님
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 2598 ]);
				break;
			}

			case PktCampCmd::FAIL_CAMP_STATE :				// 진지 상태가 이상함
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 2599 ]);
				break;
			}

			case PktCampCmd::FAIL_DIFFERENT_GUILD :			// 길드 아이디가 다름
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 2600 ]);
				break;
			}

			case PktCampCmd::FAIL_NOT_MASTER :				// 길드 마스터가 아님
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 2601 ]);
				break;
			}

			case PktCampCmd::FAIL_CAMP_RIGHT :				// 진지 권한 없음
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 1240 ]);
				break;
			}

			case PktCampCmd::FAIL_NOT_ENOUGH_MATERIAL :		// 자재 부족
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 2602 ]);
				break;
			}

			case PktCampCmd::FAIL_DIFFERENT_GOLD :			// 클라이언트와 서버의 돈이 다름
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 2590 ]);
				break;
			}

			case PktCampCmd::FAIL_NOT_ENOUGH_GOLD :			// 길드에 돈이 부족
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 2591 ]);
				break;
			}

			case PktCampCmd::FAIL_NOT_MINING_CAMP :			// 채굴기가 아님 (채굴기로 캐스팅 실패)
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 3034 ]);
				break;
			}

			case PktCampCmd::FAIL_NOT_COMPLETE_STATE :		// 완료상태(보통상태)가 아닙니다.
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 3035 ]);
				break;
			}

			case PktCampCmd::FAIL_NOW_MINING :				// 채굴중입니다.
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 3036 ]);
				break;
			}

			case PktCampCmd::FAIL_UPGRADE_STEP :			// 업그레이드 단계가 부족합니다.
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 3037 ]);
				break;
			}

			case PktCampCmd::FAIL_NOT_ENOUGH_MINERAL :		// 광물수가 부족합니다.
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 3038 ]);
				break;
			}

			case PktCampCmd::FAIL_NOT_CAMP_SHOP :			// 길드 요새 상점이 아닙니다.
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 3031 ]);
				break;
			}

			case PktCampCmd::FAIL_NOT_ZERO_CUSTOMER :		// 입장객이 있을 때는 세율을 변경할 수 없습니다.
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 3032 ]);
				break;
			}

			case PktCampCmd::FAIL_TOO_MANY_TAX :			// 세율이 최대치를 초과하였습니다.
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 3033 ]);
				break;
			}

			case PktCampCmd::FAIL_NOT_WORLDWEAPON :			// 월드 웨폰이 아닙니다.
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 3039 ]);
				break;
			}

			case PktCampCmd::FAIL_NOT_EMPTY_WEAPON :		// 월드 웨폰의 무기가 비어 있지 않습니다. (장전중, 장전됨, 발사중)
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 3040 ]);
				break;
			}

			case PktCampCmd::FAIL_NOT_READY_WEAPON :		// 월드 웨폰의 무기가 준비 되어 있지 않습니다. (비어있음, 장전중, 발사중)
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 3041 ]);
				break;
			}

			case PktCampCmd::FAIL_NOW_CHARGING :			// 월드 웨폰 무기 장전중입니다.
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 3042 ]);
				break;
			}

			case PktCampCmd::FAIL_NOW_FIRING :				// 월드 웨폰 무기 발사중입니다.
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 3043 ]);
				break;
			}

			case PktCampCmd::FAIL_MAX_UPGRADE :				// 업그레이드 Max 입니다.
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 3044 ]);
				break;
			}

			case PktCampCmd::FAIL_HAS_MINERAL :				// 채굴기 타입을 변형하려 할때 광물이 남아 있을 경우
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 3045 ]);
				break;
			}

			case PktCampCmd::FAIL_HAS_ITEM :				// 상점 타입을 변형하려 할때 아이템이 남아 있을 경우
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 3046 ]);
				break;
			}
		}
	}

	return FALSE ;
}

GAME_EVENT_FUNC(ParseCharCharSiegeArmsCmd)
{
	g_pClientSocket->SetStatusFlag(NS_NOWAITING);
	unsigned long		dwCID ; 
	unsigned long		dwArmsID ; 
	unsigned long		dwValue ; 
	unsigned char		cState ; 
	unsigned char		cSubCmd ;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharSiegeArmsCmd( lpPktBase, 
																						 &dwCID, 
																						 &dwArmsID,
																						 &dwValue, 
																						 &cState, 
																						 &cSubCmd ) ;

	if(0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		CRYLGameData*	pGame = CRYLGameData::Instance() ;
		CRYLChattingDlg* pChat = CRYLChattingDlg::Instance() ;

		switch ( cSubCmd )
		{
		case PktSiegeArmsCmd::SIEGE_CREATE_ARMS_COMPLETE :		// 공성 병기 생성 완료
			{
				RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( dwArmsID ) ;
				if ( pTempCreature && pTempCreature->GetOwnerID() == CRYLNetworkData::Instance()->m_dwMyChrID )
				{
					pChat->AddMessage( CRYLStringTable::m_strString[ 2603 ], CRYLNetworkData::Instance()->m_dwMyChrID, 128 ) ;
				}
			}
			break ;
		case PktSiegeArmsCmd::SIEGE_CANCEL				 :		// 공성 병기 생성 취소 (바로됨)
			{
				RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( dwArmsID ) ;
				if ( pTempCreature && pTempCreature->GetOwnerID() == CRYLNetworkData::Instance()->m_dwMyChrID )
				{
					pChat->AddMessage( CRYLStringTable::m_strString[ 2604 ], CRYLNetworkData::Instance()->m_dwMyChrID, 128 ) ;
				}
			}
			break ;
		case PktSiegeArmsCmd::SIEGE_RIDE_ARMS			 :		// 공성 병기 탑승
			{
				RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( dwArmsID ) ;

				pGame->m_lpInterface->ArmsRide( pTempCreature->GetObjectType(), dwCID, dwArmsID ) ;	// 근거리 공성병기 탑승

				if ( dwArmsID == CRYLNetworkData::Instance()->m_dwMyChrID )
				{
					switch( pTempCreature->GetObjectType() )
					{
						case SHORT_RANGE_SIEGE_ARMS	:		// 근거리 공성병기
							{
								// 퀵슬롯에 근거리공성병기 스킬 등록
								pGame->m_csStatus.m_lpSiegeArmsSkill = new CItemInstance ;
								pGame->m_csStatus.m_lpSiegeArmsSkill->SetSkillInfo( 0x9A01, 0, 1, TakeType::TS_QSLOT, 0, 0 ) ;
								
								CItemInstance* lpQuickSkill = NULL ;
								lpQuickSkill = pGame->m_csStatus.m_lpQuickSelected ;
								pGame->m_csStatus.m_lpQuickSelected = pGame->m_csStatus.m_lpSiegeArmsSkill ;
								pGame->m_csStatus.m_lpTmpQuickSkill = lpQuickSkill ;
							}
							break ;

						case LONG_RANGE_SIEGE_ARMS :		// 원거리 공성병기
							{
								// 퀵슬롯에 원거리공성병기 스킬 등록
								pGame->m_csStatus.m_lpSiegeArmsSkill = new CItemInstance ;
								pGame->m_csStatus.m_lpSiegeArmsSkill->SetSkillInfo( 0x9A02, 0, 1, TakeType::TS_QSLOT, 0, 0 ) ;
								
								CItemInstance* lpQuickSkill = NULL ;
								lpQuickSkill = pGame->m_csStatus.m_lpQuickSelected ;
								pGame->m_csStatus.m_lpQuickSelected = pGame->m_csStatus.m_lpSiegeArmsSkill ;
								pGame->m_csStatus.m_lpTmpQuickSkill = lpQuickSkill ;
							}
							break ;

						case AIRSHIP :
							{
								pGame->m_csStatus.m_lpQuickSelected = NULL ;
								pGame->m_csStatus.m_lpTmpQuickSkill = NULL ;
							}
							break ;
					}

					CRYLGameScene*	pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
					CRYLArmsHPDlg*	pDlg   = ( CRYLArmsHPDlg* )pScene->GetArmsHPDlg() ;
					pDlg->SetVisible( TRUE ) ;
					
					pGame->m_lpInterface->CloseInterface();					
					pGame->SetEnableMove( TRUE ) ;
				}
			}
			break ;

		case PktSiegeArmsCmd::SIEGE_GETOFF_ARMS			 :		// 공성 병기 내림
			{
				RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( dwArmsID ) ;
				pGame->m_lpInterface->ArmsGetOff( dwCID, dwArmsID ) ;

				if ( dwCID == CRYLNetworkData::Instance()->m_dwMyChrID )
				{
					pGame->m_csStatus.m_lpQuickSelected = pGame->m_csStatus.m_lpTmpQuickSkill ;
					GM_DELETE( pGame->m_csStatus.m_lpSiegeArmsSkill ) 

					CRYLGameScene*	pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
					CRYLArmsHPDlg*	pDlg   = ( CRYLArmsHPDlg* )pScene->GetArmsHPDlg() ;
					pDlg->SetVisible( FALSE ) ;

					// 자신이 요청한 경우라면, 자신의 캐릭터 위치를 변경시켜준다.
					RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
					if ( !pSelfCreature )
						return FALSE;

					// 위치 이동
					vector3 vecPos = *pTempCreature->GetPosition() ;
					POS psPos = { vecPos.x / 100.0f - 3.0f, vecPos.y / 100.0f, vecPos.z / 100.0f - 3.0f } ;
					CRYLNetworkData::Instance()->SetNetworkFlag( NF_BINDPOS ) ; 
					SendPacket::CharBindPos( g_GameSession, CRYLNetworkData::Instance()->m_dwMyChrID, 1, &psPos, 0 ) ;
				}
				else
				{
					RYLCreature* pRideCreature = RYLCreatureManager::Instance()->GetCreature( dwCID ) ;

					if(pRideCreature)
					{
						pRideCreature->SetRideEnable(false);
					}

					RYLCreatureManager::Instance()->DeleteArmsCreature(dwCID);
				}
			}
			break ;

		case PktSiegeArmsCmd::SIEGE_REPAIR_ARMS :				// 공성 병기 수리 [In] dwValue : 수리할 HP
			{
				RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( dwArmsID ) ;
				if ( pTempCreature && pTempCreature->GetOwnerID() == CRYLNetworkData::Instance()->m_dwMyChrID )
				{
					CharDataNode* pData = RYLCharacterDataManager::Instance()->GetCharData( dwArmsID ) ;
					if ( pData )
					{
						char szBuff[ 128 ] ;
						float fMaterialNum = (float)(dwValue) / (float)pData->m_ulMaxHP * 10.0f ;
						fMaterialNum = ceilf( fMaterialNum ) ;
						unsigned long dwNum = (unsigned long)( fMaterialNum ) ;
												
						if ( pData->m_lCurrHP + dwValue == pData->m_ulMaxHP )
						{
							sprintf( szBuff, CRYLStringTable::m_strString[ 4100 ], dwNum ) ;
							pChat->AddMessage( szBuff, CRYLNetworkData::Instance()->m_dwMyChrID, 128 ) ;
						}
						else
						{
							unsigned long dwRepairPercent = static_cast<unsigned long>( (float)dwValue / (float)pData->m_ulMaxHP * 100.0f ) ;
							sprintf( szBuff, CRYLStringTable::m_strString[ 4101 ], dwRepairPercent, dwNum ) ;
							pChat->AddMessage( szBuff, CRYLNetworkData::Instance()->m_dwMyChrID, 128 ) ;
						}
					}
				}
			}
			break ;

		case PktSiegeArmsCmd::SIEGE_REPAIR_ARMS_COMPLETE :		// 공성 병기 수리 완료
			{
				RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( dwArmsID ) ;
				if ( pTempCreature && pTempCreature->GetOwnerID() == CRYLNetworkData::Instance()->m_dwMyChrID )
				{
					switch( pTempCreature->GetObjectType() )
					{
					case SHORT_RANGE_SIEGE_ARMS	:		// 근거리 수성병기
						pChat->AddMessage( CRYLStringTable::m_strString[ 4086 ], CRYLNetworkData::Instance()->m_dwMyChrID, 128 ) ;
						break ;
					case LONG_RANGE_SIEGE_ARMS :			// 원거리 수성병기
						pChat->AddMessage( CRYLStringTable::m_strString[ 4087 ], CRYLNetworkData::Instance()->m_dwMyChrID, 128 ) ;
						break ;
					}
				}
			}
			break ;
		case PktSiegeArmsCmd::SIEGE_TO_STARTKIT_COMPLETE :		// 공성 병기를 스타트킷으로 완료
			break ;
		case PktSiegeArmsCmd::SIEGE_DESTROY_ARMS		 :		// 공성 병기 파괴
			{
				RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( dwArmsID ) ;
				
				if ( pTempCreature )
				{
					for (int i=0; i<Siege::AIRSHIP_RIDER_NUM; ++i)
					{
						if ( 0 != pTempCreature->GetRiderID(i) )
						{
							unsigned long dwRiderID = pTempCreature->GetRiderID(i) ;
							pGame->m_lpInterface->ArmsGetOff( dwRiderID, dwArmsID ) ;

							if ( dwRiderID == CRYLNetworkData::Instance()->m_dwMyChrID )
							{
								CRYLGameScene*	pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
								CRYLArmsHPDlg*	pDlg   = ( CRYLArmsHPDlg* )pScene->GetArmsHPDlg() ;
								pDlg->SetVisible( FALSE ) ;
							}
						}
					}
				}

				pGame->m_lpInterface->DestroyArms( dwArmsID ) ;
			}
			break ;
		}
	}
	else
	{
		CRYLMessageBox *lpMessageBox;
		switch (CRYLNetworkData::Instance()->m_dwNetworkError)
		{
			case PktSiegeArmsCmd::FAIL_NOT_EXIST_GUILD :			// 존재 하지 않는 길드
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 2582 ]);
				break;
			}

			case PktSiegeArmsCmd::FAIL_NOT_EXIST_SIEGE_ARMS :		// 존재 하지 않는 공성 병기
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 2605 ]);
				break;
			}

			case PktSiegeArmsCmd::FAIL_NOT_SIEGE_ARMS :				// 공성 병기가 아님
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 2606 ]);
				break;
			}

			case PktSiegeArmsCmd::FAIL_NOT_OWNER :					// 병기 소유자가 아님
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 2607 ]);
				break;
			}

			case PktSiegeArmsCmd::FAIL_SIEGE_ARMS_STATE :			// 공성 병기의 상태가 적절치 않음
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 2608 ]);
				break;
			}

			case PktSiegeArmsCmd::FAIL_ALREADY_RIDE :				// 병기에 탑승해 있음
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 2592 ]);
				break;
			}

			case PktSiegeArmsCmd::FAIL_NOT_RIDE :					// 병기를 타고 있지 않음
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 2593 ]);
				break;
			}

			case PktSiegeArmsCmd::FAIL_RIDE_SIEGE_ARMS :			// 병기 탑승에 실패
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 2609 ]);
				break;
			}

			case PktSiegeArmsCmd::FAIL_GET_OFF_SIEGE_ARMS :			// 병기 내리기 실패
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 2610 ]);
				break;
			}

			case PktSiegeArmsCmd::FAIL_NOT_ENOUGH_MATERIAL :		// 자재 부족
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 2602 ]);
				break;
			}
		}
	}

	return FALSE ;
}

GAME_EVENT_FUNC(ParseCharSiegeBroadCast)
{
// CASTLE_TODO : 사용하지 않는 옛날 공성 오브젝트 브로드캐스트

//	unsigned long			dwCID ;		
//	unsigned long			dwOwnerID ;	
//	unsigned long			dwGID ;		
//	unsigned short			wObjectType ;
//	unsigned long			dwNowHP ;		
//	unsigned long			dwMaxHP ;		
//	unsigned char			cState ;		
//	unsigned char			cSubState ;	
//	unsigned char			cUpgradeStep ;
//	unsigned char			cUpgradeType ;
//	float					fDefaultDir ;
//	unsigned long			dwRiderCID[Siege::AIRSHIP_RIDER_NUM];
//	CNetworkPos				netPos ;
//
//	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharSiegeBroadCast( lpPktBase, 
//																						   &dwCID,			// 공성관련 오브젝트 ID
//																						   &dwOwnerID,		// 진지 = 진지 ID, 성 = 성ID, 공성병기 = 주인 CID,
//																						   &dwGID,			// GID
//																						   &wObjectType,	// Siege::SiegeObjectType 참고pe, 
//																						   &dwNowHP,		// NowHP
//																						   &dwMaxHP,		// MaxHP	
//																						   &cState,			// Siege::State
//																						   &cSubState,		// Siege::State,
//																						   &cUpgradeStep,	// 업그레이드 단계tep, 
//																						   &cUpgradeType,	// 상징물만 해당( 보석에의한 업그레이드 상태 )ype, 
//																						   &fDefaultDir,	// 기본 방향
//																						   dwRiderCID,		// 타고 있는 캐릭터 CID
//																						   &netPos ) ;		//위치, x, y, z
//																						   
//
//	if(0 == CRYLNetworkData::Instance()->m_dwNetworkError)
//	{
//        RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
//        if( !pSelfCreature )
//            return FALSE;
//
//        RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( dwCID ) ;
//
//		if ( pTempCreature )
//		{
//			vector3 vecPos;
//			vecPos.x = netPos.GetXPos() * 100.0f ;
//			vecPos.y = netPos.GetYPos() * 100.0f ;   
//			vecPos.z = netPos.GetZPos() * 100.0f ;
//
//			// KID             => SiegeObjectType
//			//					  + (State * UPGRADE_KIND_NUM)
//			//					  + UpgradeStep
//
//			// KID (성 상징물) => SiegeObjectType
//			//					  + (State * JEWEL_TYPE_NUM * UPGRADE_KIND_NUM)
//			//					  +	(JewelType * UPGRADE_KIND_NUM)
//			//					  + UpgradeStep
//			WORD wRealObjType = wObjectType ;
//			
//			if ( wObjectType == EMBLEM )
//			{
//				wRealObjType = wObjectType + ( cState  * Siege::JEWEL_TYPE_NUM * Siege::UPGRADE_KIND_NUM ) 
//								+ ( cUpgradeType * Siege::UPGRADE_KIND_NUM ) 
//								+ cUpgradeStep ;
//
//                // Castle에 상징물 갱신
//				CRYLCastle* pCastle = CRYLCastleManager::Instance()->FindCastle( dwOwnerID ) ;
//				if ( pCastle )
//				{
//					pCastle->SetEmblem( pTempCreature ) ;
//				}
//			}
//			else 
//			{
//				wRealObjType = wObjectType 
//								+ ( cState  * Siege::UPGRADE_KIND_NUM )
//								+ cUpgradeStep ; 
//			}
//
//			if ( pTempCreature->GetObjectType() != wObjectType || 
//				 pTempCreature->GetState() != cState ||
//				 pTempCreature->GetSubState() != cSubState )
//			{
//				const CMonsterMgr::MonsterProtoType *lpMonster = g_MonsterMgr.GetMonsterProtoType( wRealObjType ) ;
//
//				if ( lpMonster )
//				{
//					CZ3DGeneralChrModel* pAddChrmodel = CSceneManager::AddCharacter( (char*)lpMonster->m_MonsterInfo.m_strModelingFlag, 
//																					 "FACE01", "HAIR01" ) ;
//                    CZ3DGeneralChrModel* pTempModel = pTempCreature->GetCharModel();
//
//					if ( pAddChrmodel && pTempModel )
//					{
//						if ( Siege::LONG_RANGE_SIEGE_ARMS == wObjectType || Siege::SHORT_RANGE_SIEGE_ARMS == wObjectType )
//						{
//							CSceneManager::CharacterCollisionDetectType( pAddChrmodel, CDT_ONLYTERRAIN );	// 추가..
//						}
//
//						pAddChrmodel->SetPosition( pTempModel->GetPosition() ) ;
//						pAddChrmodel->SetDirection( pTempModel->GetDirection() ) ;
//						CSceneManager::DeleteCharacter( pTempModel ) ;
//                        pTempCreature->SetCharModel( pAddChrmodel );
//					}
//				}
//			}
//
//            RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
//            if( !pSelfCreature )
//                return FALSE;
//
//            CZ3DGeneralChrModel* pTempModel = pTempCreature->GetCharModel();
//
//            if( pTempModel && dwCID != pSelfCreature->GetCharID() )
//			{
//                pTempModel->SetDirection( netPos.GetDirection() );
//			}
//
//            CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( pTempCreature->GetCharID() );
//            if( !pTempData )
//                return FALSE;
//			
//            pTempData->m_ulGuildID  = dwGID;
//            pTempData->m_lCurrHP    = dwNowHP;
//            pTempData->m_ulMaxHP    = dwMaxHP;
//            
//			pTempCreature->SetOwnerID( dwOwnerID )	;
//			pTempCreature->SetObjectType( wObjectType ) ;
//			pTempCreature->SetState( cState ) ;
//			pTempCreature->SetSubState( cSubState ) ;
//			pTempCreature->SetUpgradeStep( cUpgradeStep ) ;
//			pTempCreature->SetUpgradeType( cUpgradeType ) ;
//			pTempCreature->SetRiders( dwRiderCID ) ;
//
//			if ( Siege::CAMP == wObjectType || Siege::MINING_CAMP == wObjectType ||
//				 Siege::CAMP_SHOP == wObjectType ||
//				 Siege::KARTERANT_WEAPON == wObjectType ||
//				 Siege::MERKADIA_WEAPON  == wObjectType )
//			{
//				CRYLCamp* pCamp = CRYLCampManager::Instance()->FindCamp( dwOwnerID );
//				if ( pCamp )
//				{
//					pCamp->SetState( cState ) ;
//				}
//			}
//		}
//		else
//		{
//			WORD wRealObjType = wObjectType ;
//				
//			if ( wObjectType == EMBLEM )
//			{
//				wRealObjType = wObjectType + ( cState  * Siege::JEWEL_TYPE_NUM * Siege::UPGRADE_KIND_NUM ) 
//								+ ( cUpgradeType * Siege::UPGRADE_KIND_NUM ) 
//								+ cUpgradeStep ;
//			}
//			else 
//			{
//				wRealObjType = wObjectType 
//								+ ( cState  * Siege::UPGRADE_KIND_NUM )
//								+ cUpgradeStep ; 
//			}
//
//			const CMonsterMgr::MonsterProtoType *lpMonster = g_MonsterMgr.GetMonsterProtoType( wRealObjType ) ;
//			
//			if ( lpMonster )
//			{
//				vector3 vecPos;
//				vecPos.x = netPos.GetXPos() * 100.0f ;
//				vecPos.y = netPos.GetYPos() * 100.0f ;
//				vecPos.z = netPos.GetZPos() * 100.0f ; 
//
//				switch ( wObjectType )
//				{
//				case SHORT_RANGE_CASTLE_ARMS :		// 근거리 수성병기
//					{
//						g_CharacterData.ChangeSiegeModel( dwCID, 
//														  lpMonster->m_MonsterInfo.m_strName,
//														  vecPos, 0.0f, 
//														  "ShortCastleWeapon" ) ;
//					}
//					break ;		
//				case LONG_RANGE_CASTLE_ARMS	:		// 원거리 수성병기
//					{
//						g_CharacterData.ChangeSiegeModel( dwCID, 
//														  lpMonster->m_MonsterInfo.m_strName,
//														  vecPos, 0.0f, 
//														  "LongCastleWeapon" ) ;
//					}
//					break ;
//				case SHORT_RANGE_SIEGE_ARMS	:		// 근거리 공성병기
//					{
//						g_CharacterData.ChangeSiegeModel( dwCID, 
//														  lpMonster->m_MonsterInfo.m_strName,
//														  vecPos, 0.0f, 
//														  "ShortSiegeWeapon" ) ;
//					}
//					break ;
//				case LONG_RANGE_SIEGE_ARMS :		// 원거리 공성병기
//					{
//						g_CharacterData.ChangeSiegeModel( dwCID, 
//														  lpMonster->m_MonsterInfo.m_strName,
//														  vecPos, 0.0f, 
//														  "LongSeigeWeapon" ) ;
//					}
//					break ;
//					 
//				case AIRSHIP :
//					{
//						g_CharacterData.ChangeSiegeModel( dwCID, 
//														  lpMonster->m_MonsterInfo.m_strName,
//														  vecPos, 0.0f, 
//														  "DropShip" ) ;
//					}
//					break ;
//
//				default :
//					{
//						GameProcess::LogCharLoad(dwCID, LOG_RFL);
//
//						if ( wObjectType == GATE )
//						{
//                            RYLCreatureManager::Instance()->AddCreature( dwCID,
//																		 lpMonster->m_MonsterInfo.m_strName,
//																		 (char*)lpMonster->m_MonsterInfo.m_strModelingFlag,
//																		 vecPos,
//																		 0,
//																		 true,
//																		 0,
//																		 -1, NULL,
//																		 true, 1, D3DXVECTOR3( vecPos.x, vecPos.y, vecPos.z ) ) ;
//						}
//						else
//						{
//							RYLCreatureManager::Instance()->AddCreature( dwCID,
//							  											 lpMonster->m_MonsterInfo.m_strName,
//																		 (char*)lpMonster->m_MonsterInfo.m_strModelingFlag,
//																		 vecPos, 0,
//																		 lpMonster->m_MonsterInfo.m_bCollision ) ;
//						}
//					}
//					break ;
//				}
//
//                pTempCreature = RYLCreatureManager::Instance()->GetCreature( dwCID ) ;
//                if( !pTempCreature )
//                    return FALSE;
//
//                CZ3DGeneralChrModel* pTempModel = pTempCreature->GetCharModel();
//                if( !pTempModel )
//                    return FALSE;
//
//                pTempCreature->SetScale( lpMonster->m_MonsterInfo.m_fSize );
//				pTempModel->SetScale( lpMonster->m_MonsterInfo.m_fSize );
//				pTempModel->SetDirection( fDefaultDir );
//
//				if ( Siege::LONG_RANGE_SIEGE_ARMS == wObjectType || Siege::SHORT_RANGE_SIEGE_ARMS == wObjectType )
//				{
//					CSceneManager::CharacterCollisionDetectType( pTempModel, CDT_ONLYTERRAIN );	// 추가..
//					pTempCreature->SetCDTType( CDT_ONLYTERRAIN );	// 추가..
//				}
//
//                CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( pTempCreature->GetCharID() );
//                if( !pTempData )
//                    return FALSE;
//				
//                pTempData->m_ulGuildID  = dwGID;
//                pTempData->m_lCurrHP    = dwNowHP;
//                pTempData->m_ulMaxHP    = dwMaxHP;
//
//				pTempCreature->SetOwnerID( dwOwnerID )	;
//				pTempCreature->SetObjectType( wObjectType ) ;
//				pTempCreature->SetState( cState ) ;
//				pTempCreature->SetSubState( cSubState ) ;
//				pTempCreature->SetUpgradeStep( cUpgradeStep ) ;
//				pTempCreature->SetUpgradeType( cUpgradeType ) ;
//				pTempCreature->SetRiders( dwRiderCID ) ;
//
//				pTempCreature->SetAction( CA_WAIT, CA_WAIT );
//
//                if ( wObjectType == EMBLEM )
//				{
//					// Castle에 상징물 갱신
//					CRYLCastle* pCastle = CRYLCastleManager::Instance()->FindCastle( dwOwnerID ) ;
//					if ( pCastle )
//					{
//						pCastle->SetEmblem( pTempCreature ) ;
//					}
//				}
//
//				if ( Siege::CAMP == wObjectType || Siege::MINING_CAMP == wObjectType ||
//					 Siege::CAMP_SHOP == wObjectType ||
//					 Siege::KARTERANT_WEAPON == wObjectType ||
//					 Siege::MERKADIA_WEAPON  == wObjectType )
//				{
//					CRYLCamp* pCamp = CRYLCampManager::Instance()->FindCamp( dwOwnerID );
//					if ( pCamp )
//					{
//						pCamp->SetState( cState ) ;
//					}
//				}
//			}			
//		}
//
//		// 추가된 캐릭터들의 길드 정보 요청용 (길드의 중복 요청을 피하기 위해 자료 구조는 set)
//		CRYLGameScene*		 pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
//		CRYLGuildFrameDlg*	 pDlg   = ( CRYLGuildFrameDlg* )pScene->GetGuildFrameDlg() ;
//		CRYLGuildInfoPanel*  pPanel = ( CRYLGuildInfoPanel* )pDlg->GetGuildInfoPanel() ;
//
//		std::set<unsigned long>	setGID;
//
//		if (0 != dwGID)
//		{
//			if (NULL == pPanel->FindGuild(dwGID & ~Guild::MEMBER_WAIT_BIT))
//			{
//				setGID.insert(dwGID & ~Guild::MEMBER_WAIT_BIT);
//
//				pPanel->RequestNewGuild(setGID) ;
//				setGID.clear() ;
//			}
//		}
//	}
//	else
//	{
//		// Error
//	}

	return FALSE ;
}

GAME_EVENT_FUNC(ParseCharSiegeBroadCast2nd)
{
// CASTLE_TODO : 사용하지 않는 공성 오브젝트 브로드 캐스트 세컨드 버젼

//	const int nMaxObjectNum = 255;
//	PktSiegeBroadCast siegeObjectArray[nMaxObjectNum];
//	unsigned long dwObjectCount = 0;
//
//	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharSiegeBroadCast2nd( lpPktBase,
//																							  &dwObjectCount,
//																							  siegeObjectArray ) ;
//
//	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError && dwObjectCount > 0)
//	{
//		CRYLGameScene*		 pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
//		CRYLGuildFrameDlg*	 pDlg   = ( CRYLGuildFrameDlg* )pScene->GetGuildFrameDlg() ;
//		CRYLGuildInfoPanel*  pPanel = ( CRYLGuildInfoPanel* )pDlg->GetGuildInfoPanel() ;
//
//		// 추가된 캐릭터들의 길드 정보 요청용 (길드의 중복 요청을 피하기 위해 자료 구조는 set)
//		std::set<unsigned long>	setGID;
//
//		for (unsigned long i = 0; i < dwObjectCount; ++i)
//		{
//            RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( siegeObjectArray[i].m_dwCID ) ;
//
//			if ( pTempCreature )
//			{
//				vector3 vecPos;
//				vecPos.x = siegeObjectArray[i].m_NetworkPos.GetXPos() * 100.0f ;
//				vecPos.y = siegeObjectArray[i].m_NetworkPos.GetYPos() * 100.0f ;   
//				vecPos.z = siegeObjectArray[i].m_NetworkPos.GetZPos() * 100.0f ;
//
//				// KID             => SiegeObjectType
//				//					  + (State * UPGRADE_KIND_NUM)
//				//					  + UpgradeStep
//
//				// KID (성 상징물) => SiegeObjectType
//				//					  + (State * JEWEL_TYPE_NUM * UPGRADE_KIND_NUM)
//				//					  +	(JewelType * UPGRADE_KIND_NUM)
//				//					  + UpgradeStep
//				WORD wRealObjType = siegeObjectArray[i].m_wObjectType ;
//
//				if ( siegeObjectArray[i].m_wObjectType == EMBLEM )
//				{
//					wRealObjType = siegeObjectArray[i].m_wObjectType
//									+ ( siegeObjectArray[i].m_cState  * Siege::JEWEL_TYPE_NUM * Siege::UPGRADE_KIND_NUM ) 
//									+ ( siegeObjectArray[i].m_cUpgradeType * Siege::UPGRADE_KIND_NUM ) 
//									+ siegeObjectArray[i].m_cUpgradeStep ;
//
//					// Castle 에 상징물 설정
//					CRYLCastle* pCastle = CRYLCastleManager::Instance()->FindCastle( siegeObjectArray[i].m_dwOwnerID ) ;
//					if ( pCastle )
//					{
//						pCastle->SetEmblem( pTempCreature ) ;
//					}
//				}
//				else 
//				{
//					wRealObjType = siegeObjectArray[i].m_wObjectType 
//									+ ( siegeObjectArray[i].m_cState  * Siege::UPGRADE_KIND_NUM )
//									+ siegeObjectArray[i].m_cUpgradeStep ; 
//				}
//
//				if ( pTempCreature->GetObjectType() != siegeObjectArray[i].m_wObjectType || 
//					 pTempCreature->GetState() != siegeObjectArray[i].m_cState ||
//					 pTempCreature->GetSubState() != siegeObjectArray[i].m_cSubState )
//				{
//					const CMonsterMgr::MonsterProtoType *lpMonster = g_MonsterMgr.GetMonsterProtoType( wRealObjType ) ;
//
//                    if ( lpMonster && ( pTempCreature->GetObjectType() != Siege::CAMP ) )
//				    {
//					    CZ3DGeneralChrModel* pAddChrmodel = CSceneManager::AddCharacter( (char*)lpMonster->m_MonsterInfo.m_strModelingFlag, 
//																					    "FACE01", "HAIR01" ) ;
//                        CZ3DGeneralChrModel* pTempModel = pTempCreature->GetCharModel();
//
//					    if ( pAddChrmodel && pTempModel )
//					    {
//						    if ( Siege::LONG_RANGE_SIEGE_ARMS == siegeObjectArray[i].m_wObjectType ||
//								 Siege::SHORT_RANGE_SIEGE_ARMS == siegeObjectArray[i].m_wObjectType )
//						    {
//							    CSceneManager::CharacterCollisionDetectType( pAddChrmodel, CDT_ONLYTERRAIN );	// 추가..
//						    }
//
//						    pAddChrmodel->SetPosition( pTempModel->GetPosition() ) ;
//						    pAddChrmodel->SetDirection( pTempModel->GetDirection() ) ;
//						    pTempCreature->SetCharModel( pAddChrmodel );
//
//							CSceneManager::DeleteCharacter( pTempModel ) ;
//					    }
//				    }
//
//				}
//
//                RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
//                if( !pSelfCreature )
//                    return FALSE;
//
//                CZ3DGeneralChrModel* pTempModel = pTempCreature->GetCharModel();
//
//                if( pTempModel && siegeObjectArray[i].m_dwCID != pSelfCreature->GetCharID() )
//			    {
//                    pTempModel->SetDirection( siegeObjectArray[i].m_NetworkPos.GetDirection() );
//			    }
//
//                CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( pTempCreature->GetCharID() );
//                if( !pTempData )
//				{
//					continue;
//				}
//
//				pTempData->m_ulGuildID  = siegeObjectArray[i].m_dwGID;
//                pTempData->m_lCurrHP    = siegeObjectArray[i].m_dwNowHP;
//				pTempData->m_ulMaxHP    = siegeObjectArray[i].m_dwMaxHP;
//
//				pTempCreature->SetOwnerID( siegeObjectArray[i].m_dwOwnerID );
//				pTempCreature->SetObjectType( siegeObjectArray[i].m_wObjectType );
//				pTempCreature->SetState( siegeObjectArray[i].m_cState );
//				pTempCreature->SetSubState( siegeObjectArray[i].m_cSubState );
//				pTempCreature->SetUpgradeStep( siegeObjectArray[i].m_cUpgradeStep );
//				pTempCreature->SetUpgradeType( siegeObjectArray[i].m_cUpgradeType );
//				pTempCreature->SetRiders( siegeObjectArray[i].m_dwRiderID ) ;
//
//				if ( Siege::CAMP == siegeObjectArray[i].m_wObjectType || 
//					 Siege::MINING_CAMP == siegeObjectArray[i].m_wObjectType ||
//					 Siege::CAMP_SHOP == siegeObjectArray[i].m_wObjectType || 
//					 Siege::KARTERANT_WEAPON == siegeObjectArray[i].m_wObjectType ||
//					Siege::MERKADIA_WEAPON  == siegeObjectArray[i].m_wObjectType )
//				{
//					CRYLCamp* pCamp = CRYLCampManager::Instance()->FindCamp( siegeObjectArray[i].m_dwOwnerID );
//					if ( pCamp )
//					{
//						pCamp->SetState( siegeObjectArray[i].m_cState ) ;
//					}
//				}
//			}
//			else
//			{
//				WORD wRealObjType = siegeObjectArray[i].m_wObjectType ;
//
//				if ( siegeObjectArray[i].m_wObjectType == EMBLEM )
//				{
//					wRealObjType = siegeObjectArray[i].m_wObjectType
//									+ ( siegeObjectArray[i].m_cState  * Siege::JEWEL_TYPE_NUM * Siege::UPGRADE_KIND_NUM ) 
//									+ ( siegeObjectArray[i].m_cUpgradeType * Siege::UPGRADE_KIND_NUM ) 
//									+ siegeObjectArray[i].m_cUpgradeStep ;
//				}
//				else 
//				{
//					wRealObjType = siegeObjectArray[i].m_wObjectType 
//									+ ( siegeObjectArray[i].m_cState  * Siege::UPGRADE_KIND_NUM )
//									+ siegeObjectArray[i].m_cUpgradeStep ; 
//				}
//
//				const CMonsterMgr::MonsterProtoType *lpMonster = g_MonsterMgr.GetMonsterProtoType( wRealObjType ) ;
//
//				if ( lpMonster )
//				{
//					vector3 vecPos;
//					vecPos.x = siegeObjectArray[i].m_NetworkPos.GetXPos() * 100.0f ;
//					vecPos.y = siegeObjectArray[i].m_NetworkPos.GetYPos() * 100.0f ;
//					vecPos.z = siegeObjectArray[i].m_NetworkPos.GetZPos() * 100.0f ; 
//
//					switch ( siegeObjectArray[i].m_wObjectType )
//					{
//					case SHORT_RANGE_CASTLE_ARMS :		// 근거리 수성병기
//						{
//							g_CharacterData.ChangeSiegeModel( siegeObjectArray[i].m_dwCID, 
//								                              lpMonster->m_MonsterInfo.m_strName,
//								                              vecPos, 0.0f, 
//								                              "ShortCastleWeapon" ) ;
//						}
//						break ;		
//					case LONG_RANGE_CASTLE_ARMS	:		// 원거리 수성병기
//						{
//							g_CharacterData.ChangeSiegeModel( siegeObjectArray[i].m_dwCID, 
//								                              lpMonster->m_MonsterInfo.m_strName,
//								                              vecPos, 0.0f, 
//								                              "LongCastleWeapon" ) ;
//						}
//						break ;
//					case SHORT_RANGE_SIEGE_ARMS	:		// 근거리 공성병기
//						{
//							g_CharacterData.ChangeSiegeModel( siegeObjectArray[i].m_dwCID, 
//								                              lpMonster->m_MonsterInfo.m_strName,
//								                              vecPos, 0.0f, 
//								                              "ShortSiegeWeapon" ) ;
//						}
//						break ;
//					case LONG_RANGE_SIEGE_ARMS :		// 원거리 공성병기
//						{
//							g_CharacterData.ChangeSiegeModel( siegeObjectArray[i].m_dwCID, 
//								                              lpMonster->m_MonsterInfo.m_strName,
//								                              vecPos, 0.0f, 
//								                              "LongSeigeWeapon" ) ;
//						}
//						break ;
//
//					case AIRSHIP :
//						{
//							g_CharacterData.ChangeSiegeModel( siegeObjectArray[i].m_dwCID, 
//								                              lpMonster->m_MonsterInfo.m_strName,
//								                              vecPos, 0.0f, 
//								                              "DropShip" ) ;
//						}
//						break ;
//
//					default :
//						{
//							GameProcess::LogCharLoad( siegeObjectArray[i].m_dwCID, LOG_RFL );
//
//							switch( siegeObjectArray[i].m_wObjectType )
//							{
//							case Siege::GATE :
//								{
//									RYLCreatureManager::Instance()->AddCreature( siegeObjectArray[i].m_dwCID,
//																				lpMonster->m_MonsterInfo.m_strName,
//																				(char*)lpMonster->m_MonsterInfo.m_strModelingFlag,
//																				vecPos,
//																				0,
//																				true,
//																				0,
//																				-1, NULL,
//																				true, 1, D3DXVECTOR3( vecPos.x, vecPos.y, vecPos.z ) ) ;
//								}
//								break ;
//							/*
//							case Siege::CAMP :
//								{
//									RYLCreatureManager::Instance()->AddCreature( siegeObjectArray[i].m_dwCID, 
//																				lpMonster->m_MonsterInfo.m_strName,
//																				(char *)lpMonster->m_MonsterInfo.m_strModelingFlag, 
//																				vecPos, 
//																				0, 
//																				true,
//																				-1, NULL, 
//																				true, 0, D3DXVECTOR3( vecPos.x, vecPos.y + 50.0f, vecPos.z ) ) ;
//
//								}
//								break ;
//							*/
//							default :
//								{
//									RYLCreatureManager::Instance()->AddCreature( siegeObjectArray[i].m_dwCID,
//																				lpMonster->m_MonsterInfo.m_strName,
//																				(char*)lpMonster->m_MonsterInfo.m_strModelingFlag,
//																				vecPos, 0,
//																				lpMonster->m_MonsterInfo.m_bCollision ) ;
//								}
//								break ;
//							}
//						}
//						break ;
//					}
//
//                    pTempCreature = RYLCreatureManager::Instance()->GetCreature( siegeObjectArray[i].m_dwCID ) ;
//                    if( !pTempCreature )
//					{
//						continue;
//					}
//
//                    CZ3DGeneralChrModel* pTempModel = pTempCreature->GetCharModel();
//                    if( !pTempModel )
//					{
//						continue;
//					}
//
//                    pTempCreature->SetScale( lpMonster->m_MonsterInfo.m_fSize );
//					pTempModel->SetScale( lpMonster->m_MonsterInfo.m_fSize );
//					pTempModel->SetDirection( siegeObjectArray[i].m_fDefaultDir );
//
//					if ( Siege::LONG_RANGE_SIEGE_ARMS == siegeObjectArray[i].m_wObjectType ||
//						 Siege::SHORT_RANGE_SIEGE_ARMS == siegeObjectArray[i].m_wObjectType )
//					{
//						CSceneManager::CharacterCollisionDetectType( pTempModel, CDT_ONLYTERRAIN );	// 추가..
//						pTempCreature->SetCDTType( CDT_ONLYTERRAIN );	// 추가..
//					}
//                                                                                 
//					CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( pTempCreature->GetCharID() );
//                    if( !pTempData )
//					{
//						continue;
//					}
//
//				    pTempData->m_ulGuildID  = siegeObjectArray[i].m_dwGID;
//                    pTempData->m_lCurrHP    = siegeObjectArray[i].m_dwNowHP;
//				    pTempData->m_ulMaxHP    = siegeObjectArray[i].m_dwMaxHP;
//
//				    pTempCreature->SetOwnerID( siegeObjectArray[i].m_dwOwnerID );
//				    pTempCreature->SetObjectType( siegeObjectArray[i].m_wObjectType );
//				    pTempCreature->SetState( siegeObjectArray[i].m_cState );
//				    pTempCreature->SetSubState( siegeObjectArray[i].m_cSubState );
//				    pTempCreature->SetUpgradeStep( siegeObjectArray[i].m_cUpgradeStep );
//				    pTempCreature->SetUpgradeType( siegeObjectArray[i].m_cUpgradeType );
//					pTempCreature->SetRiders( siegeObjectArray[i].m_dwRiderID ) ;
//
//					pTempCreature->SetAction( CA_WAIT, CA_WAIT );
//
//					// Castle 에 상징물 갱신
//					if ( siegeObjectArray[i].m_wObjectType == Siege::EMBLEM )
//					{
//						CRYLCastle* pCastle = CRYLCastleManager::Instance()->FindCastle( siegeObjectArray[i].m_dwOwnerID ) ;
//						if ( pCastle )
//						{
//							pCastle->SetEmblem( pTempCreature ) ;
//						}
//					}
//
//					if ( Siege::CAMP == siegeObjectArray[i].m_wObjectType || 
//						 Siege::MINING_CAMP == siegeObjectArray[i].m_wObjectType ||
//						 Siege::CAMP_SHOP == siegeObjectArray[i].m_wObjectType || 
//						 Siege::KARTERANT_WEAPON == siegeObjectArray[i].m_wObjectType ||
//						 Siege::MERKADIA_WEAPON  == siegeObjectArray[i].m_wObjectType )
//					{
//						CRYLCamp* pCamp = CRYLCampManager::Instance()->FindCamp( siegeObjectArray[i].m_dwOwnerID );
//						if ( pCamp )
//						{
//							pCamp->SetState( siegeObjectArray[i].m_cState ) ;
//						}
//					}
//				}			
//			}
//
//			if (0 != siegeObjectArray[i].m_dwGID )
//			{
//				if (NULL == pPanel->FindGuild(siegeObjectArray[i].m_dwGID & ~Guild::MEMBER_WAIT_BIT))
//				{
//					setGID.insert(siegeObjectArray[i].m_dwGID & ~Guild::MEMBER_WAIT_BIT);
//
//					// 한번에 요청할 수 있는 길드 정보의 최대 갯수는 10개
//					if (setGID.size() >= 10)
//					{
//						pPanel->RequestNewGuild(setGID) ;
//						setGID.clear();
//					}
//				}
//			}
//		}
//
//		// 정보를 가지고 있지않은 길드들은 서버에 요청
//		if (false == setGID.empty())
//		{
//			pPanel->RequestNewGuild(setGID);
//			setGID.clear();
//		}
//	}
//	else
//	{
//		// Error
//	}

	return FALSE;
}

GAME_EVENT_FUNC(ParseCharNewSiegeBroadCast)
{
	unsigned long dwCID;
	unsigned char cBroadCastType;
	int iDataSize = 0 ;
	char* pData = NULL ;

	PktNewSiegeBroadCast* lpBroadCast = reinterpret_cast<PktNewSiegeBroadCast*>( lpPktBase ) ;
	
   	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharNewSiegeBroadCast( lpPktBase, &dwCID, &cBroadCastType ) ;
	
	if ( 0 == CRYLNetworkData::Instance()->m_dwNetworkError )
	{
		unsigned long dwUpdateFalg;
		if ( BroadCastSiege::BROADCAST_DELETE_DATA != cBroadCastType )
		{
			iDataSize = lpBroadCast->GetLen() - sizeof(PktNewSiegeBroadCast) ;
			pData = reinterpret_cast<char*>( lpBroadCast + 1 ) ;

			if ( iDataSize < sizeof( unsigned long ) )
			{
				return FALSE;
			}

			// UpdateFlag 를 얻어온다.
			BroadCastSiege::COPY_AND_ADVANCE_SRC( &dwUpdateFalg, pData, sizeof(unsigned long) );
		}

		BroadCastSiege::sOwnerShipInfo ownerShipInfo;
		BroadCastSiege::sStateInfo stateInfo;
		BroadCastSiege::sHPInfo HPInfo;
		BroadCastSiege::sPosInfo posInfo;
		BroadCastSiege::sMaterialInfo materialInfo;
		BroadCastSiege::sRiderInfo riderInfo;

		switch ( cBroadCastType )
		{
			// ======================================================================================================================
			// 처음 데이터를 받아서 객체를 생성할 때
			case BroadCastSiege::BROADCAST_ALL_DATA:
			{
				// 순서주의!!! 
				BroadCastSiege::COPY_AND_ADVANCE_SRC( &ownerShipInfo, pData, sizeof(BroadCastSiege::sOwnerShipInfo) );
				BroadCastSiege::COPY_AND_ADVANCE_SRC( &stateInfo, pData, sizeof(BroadCastSiege::sStateInfo) );
				BroadCastSiege::COPY_AND_ADVANCE_SRC( &HPInfo, pData, sizeof(BroadCastSiege::sHPInfo) );
				BroadCastSiege::COPY_AND_ADVANCE_SRC( &posInfo, pData, sizeof(BroadCastSiege::sPosInfo) );
				BroadCastSiege::COPY_AND_ADVANCE_SRC( &materialInfo, pData, sizeof(BroadCastSiege::sMaterialInfo) );
				BroadCastSiege::COPY_AND_ADVANCE_SRC( &riderInfo, pData, sizeof(BroadCastSiege::sRiderInfo) );
				
				RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature() ;
				if( !pSelfCreature )
					return FALSE;

				RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( dwCID ) ;
				if ( pTempCreature )
				{
					vector3 vecPos;
					vecPos.x = posInfo.m_NetworkPos.GetXPos() * 100.0f ;
					vecPos.y = posInfo.m_NetworkPos.GetYPos() * 100.0f + 15.0f;   
					vecPos.z = posInfo.m_NetworkPos.GetZPos() * 100.0f ;

					if(SHORT_RANGE_CASTLE_ARMS == stateInfo.m_wObjectType || SHORT_RANGE_SIEGE_ARMS == stateInfo.m_wObjectType || CASTLE_ARMS_NPC== stateInfo.m_wObjectType )
					{
						pTempCreature->SetPosition(vecPos);						
					}

					// KID             => SiegeObjectType
					//					  + (State * UPGRADE_KIND_NUM)
					//					  + UpgradeStep

					// KID (성 상징물) => SiegeObjectType
					//					  + (State * JEWEL_TYPE_NUM * UPGRADE_KIND_NUM)
					//					  +	(JewelType * UPGRADE_KIND_NUM)
					//					  + UpgradeStep

					WORD wRealObjType = stateInfo.m_wObjectType ;

					if ( stateInfo.m_wObjectType == EMBLEM )
					{
						wRealObjType = stateInfo.m_wObjectType + ( stateInfo.m_cState  * Siege::JEWEL_TYPE_NUM * Siege::UPGRADE_KIND_NUM ) 
															   + ( stateInfo.m_cUpgradeType * Siege::UPGRADE_KIND_NUM ) 
															   + stateInfo.m_cUpgradeStep ;

						// Castle에 상징물 갱신
						CRYLCastle* pCastle = CRYLCastleManager::Instance()->FindCastle( ownerShipInfo.m_dwCampOrCastleID ) ;
						if ( pCastle )
						{
							pCastle->SetEmblem( pTempCreature ) ;
						}
					}
					else 
					{
						wRealObjType = stateInfo.m_wObjectType + ( stateInfo.m_cState  * Siege::UPGRADE_KIND_NUM ) + stateInfo.m_cUpgradeStep ; 
					}

					const CMonsterMgr::MonsterProtoType *lpMonster = g_MonsterMgr.GetMonsterProtoType( wRealObjType ) ;

					if ( lpMonster )
					{						
						if ( stricmp( (char*)lpMonster->m_MonsterInfo.m_strModelingFlag, pTempCreature->GetModelFileName()->c_str() ) )
						{
							vector3 vecPos = *pTempCreature->GetPosition();
							pTempCreature->SetModelFileName( lpMonster->m_MonsterInfo.m_strModelingFlag ) ;
							
							CZ3DGeneralChrModel* pTempModel = NULL ;
							bool bHouse = false ;
							int  iHouseType = -1 ;
							if ( stateInfo.m_wObjectType == Siege::CAMP )
							{
								bHouse = true ;

								switch ( stateInfo.m_cState )
								{ 
									case Siege::DEVELOPING :	iHouseType = 2 ;  break ;
									case Siege::COMPLETE :		iHouseType = 0 ;  break ;
									default :					iHouseType = -1 ; break ;
								}
							}
							else if ( stateInfo.m_wObjectType == Siege::KARTERANT_WEAPON )
							{
								bHouse = true ;
								iHouseType = 3;
							}
							else if ( stateInfo.m_wObjectType == Siege::MERKADIA_WEAPON )
							{
								bHouse = true ;
								iHouseType = 4;
							}
							else if ( stateInfo.m_wObjectType == Siege::MINING_CAMP )
							{
								bHouse = true ;
								iHouseType = 5;
							}
							else if ( stateInfo.m_wObjectType == Siege::CAMP_SHOP )
							{
								bHouse = true ;
								iHouseType = 6;
							}

							RYLCreatureManager::Instance()->FreeModel( pTempCreature->GetCharModel(), TRUE ) ;
							pTempModel = RYLCreatureManager::Instance()->AllocModel( pTempCreature, bHouse, iHouseType, D3DXVECTOR3( vecPos.x, vecPos.y, vecPos.z ) ) ;
							pTempCreature->SetPosition( vecPos ) ;
						}						
					}

					CZ3DGeneralChrModel* pTempModel = pTempCreature->GetCharModel();
					if( pTempModel && dwCID != pSelfCreature->GetCharID() )
					{
						pTempModel->SetDirection( posInfo.m_NetworkPos.GetDirection() );
					}		

					CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( pTempCreature->GetCharID() );
					if( !pTempData )
						return FALSE;

					// 성문 열고 닫는 애니메이션 때문에 체크
					bool bSubStateChanged = ( stateInfo.m_cSubState != pTempCreature->GetSubState() ) ;

					pTempData->m_ulGuildID  = ownerShipInfo.m_dwGID;
					pTempData->m_cNation	= ownerShipInfo.m_cNation;
					pTempData->m_lCurrHP    = HPInfo.m_dwNowHP;
					pTempData->m_ulMaxHP    = HPInfo.m_dwMaxHP;

                    pTempCreature->SetCampOrCastleID( ownerShipInfo.m_dwCampOrCastleID ) ;
					pTempCreature->SetOwnerID( ownerShipInfo.m_dwOwnerCID )	;
					pTempCreature->SetObjectType( stateInfo.m_wObjectType ) ;
					pTempCreature->SetState( stateInfo.m_cState ) ;
					pTempCreature->SetSubState( stateInfo.m_cSubState ) ;
					pTempCreature->SetUpgradeStep( stateInfo.m_cUpgradeStep ) ;
					pTempCreature->SetUpgradeType( stateInfo.m_cUpgradeType ) ;
					pTempCreature->SetRiders( riderInfo.m_dwRiderID ) ;


					if ( Siege::CAMP == stateInfo.m_wObjectType || 
						 Siege::MINING_CAMP == stateInfo.m_wObjectType ||
						 Siege::CAMP_SHOP == stateInfo.m_wObjectType || 
						 Siege::KARTERANT_WEAPON == stateInfo.m_wObjectType ||
						 Siege::MERKADIA_WEAPON  == stateInfo.m_wObjectType )
					{
						CRYLCamp* pCamp = CRYLCampManager::Instance()->FindCamp( ownerShipInfo.m_dwCampOrCastleID );
						if ( pCamp )
						{
							pCamp->SetState( stateInfo.m_cState ) ;
                            pCamp->UpdateMaterial( materialInfo.m_cMaterial ) ;
						}
					}

					// 성문 열리고 닫히는 처리
					if ( Siege::GATE == stateInfo.m_wObjectType )
					{
						float fDir = pTempModel->GetDirection() ;
						
						if ( Siege::DESTROYED == stateInfo.m_cState )
						{
							RYLCreatureManager::Instance()->FreeModel( pTempCreature->GetCharModel(), TRUE ) ;
							pTempModel = RYLCreatureManager::Instance()->AllocModel( pTempCreature, true, 7, D3DXVECTOR3( vecPos.x, vecPos.y, vecPos.z ) ) ;
							if ( pTempModel )
							{
								pTempCreature->SetPosition( vecPos ) ;
								pTempModel->SetDirection( fDir ) ;

								CCharacterLightShadowManager::CCharacterDataNode CheckNode = 
									CSceneManager::GetCharacterDataNode( pTempModel ) ;

								CheckNode.m_bHide = true ;
								CSceneManager::SetCharacterDataNode( CheckNode ) ;

								pTempCreature->SetAction( CA_WAIT, CA_WAIT ) ;
							}
						}
						else
						{
							if ( bSubStateChanged && Siege::OPENED == stateInfo.m_cSubState )
							{
								RYLCreatureManager::Instance()->FreeModel( pTempCreature->GetCharModel(), TRUE ) ;
								pTempModel = RYLCreatureManager::Instance()->AllocModel( pTempCreature, true, 7, D3DXVECTOR3( vecPos.x, vecPos.y, vecPos.z ) ) ;
								if ( pTempModel )
								{
									pTempCreature->SetPosition( vecPos ) ;
									pTempModel->SetDirection( fDir ) ;

									CCharacterLightShadowManager::CCharacterDataNode CheckNode = 
										CSceneManager::GetCharacterDataNode( pTempModel ) ;

									if ( CheckNode.m_bHide )
									{
										CheckNode.m_bHide = false ;
										CSceneManager::SetCharacterDataNode( CheckNode ) ;	
									}

									pTempCreature->SetAction( "open", "open" ) ;
								}
							}
							else if ( bSubStateChanged && Siege::CLOSED == stateInfo.m_cSubState )
							{
								RYLCreatureManager::Instance()->FreeModel( pTempCreature->GetCharModel(), TRUE ) ;
								pTempModel = RYLCreatureManager::Instance()->AllocModel( pTempCreature, true, 1, D3DXVECTOR3( vecPos.x, vecPos.y, vecPos.z ) ) ;
								if ( pTempModel )
								{
									pTempCreature->SetPosition( vecPos ) ;
									pTempModel->SetDirection( fDir ) ;

									CCharacterLightShadowManager::CCharacterDataNode CheckNode = 
										CSceneManager::GetCharacterDataNode( pTempModel ) ;

									if ( CheckNode.m_bHide )
									{
										CheckNode.m_bHide = false ;
										CSceneManager::SetCharacterDataNode( CheckNode ) ;	
									}
								}
							}
						}
					}
				}
				else
				{
					WORD wRealObjType = stateInfo.m_wObjectType ;

					if ( stateInfo.m_wObjectType == EMBLEM )
					{
						wRealObjType = stateInfo.m_wObjectType + ( stateInfo.m_cState  * Siege::JEWEL_TYPE_NUM * Siege::UPGRADE_KIND_NUM ) 
															   + ( stateInfo.m_cUpgradeType * Siege::UPGRADE_KIND_NUM ) 
															   + stateInfo.m_cUpgradeStep ;
					}
					else 
					{
						wRealObjType = stateInfo.m_wObjectType + ( stateInfo.m_cState  * Siege::UPGRADE_KIND_NUM ) + stateInfo.m_cUpgradeStep ; 
					}

					const CMonsterMgr::MonsterProtoType *lpMonster = g_MonsterMgr.GetMonsterProtoType( wRealObjType ) ;

					if ( lpMonster )
					{
						vector3 vecPos;
						vecPos.x = posInfo.m_NetworkPos.GetXPos() * 100.0f ;
						vecPos.y = posInfo.m_NetworkPos.GetYPos() * 100.0f + 15.0f;
						vecPos.z = posInfo.m_NetworkPos.GetZPos() * 100.0f ; 

						switch ( stateInfo.m_wObjectType )
						{
							case SHORT_RANGE_CASTLE_ARMS :		// 근거리 수성병기
							{
								g_CharacterData.ChangeSiegeModel( dwCID, 
																  lpMonster->m_MonsterInfo.m_strName,
																  vecPos, 0.0f, 
																  "ShortCastleWeapon" ) ;
							}
							break ;		
							
							case LONG_RANGE_CASTLE_ARMS	:		// 원거리 수성병기
							{
								g_CharacterData.ChangeSiegeModel( dwCID, 
																  lpMonster->m_MonsterInfo.m_strName,
																  vecPos, 0.0f, 
																  "LongCastleWeapon" ) ;
							}
							break ;
							
							case SHORT_RANGE_SIEGE_ARMS	:		// 근거리 공성병기
							{								
								g_CharacterData.ChangeSiegeModel( dwCID, 
																  lpMonster->m_MonsterInfo.m_strName,
																  vecPos, 0.0f, 
																  "ShortSiegeWeapon" ) ;
							}
							break ;
							
							case LONG_RANGE_SIEGE_ARMS :		// 원거리 공성병기
							{
								g_CharacterData.ChangeSiegeModel( dwCID, 
																  lpMonster->m_MonsterInfo.m_strName,
																  vecPos, 0.0f, 
																  "LongSeigeWeapon" ) ;
							}
							break ;

							case AIRSHIP :
							{
								g_CharacterData.ChangeSiegeModel( dwCID, 
																  lpMonster->m_MonsterInfo.m_strName,
																  vecPos, 0.0f, 
																  "DropShip" ) ;
							}
							break ;

							default :
							{
								GameProcess::LogCharLoad(dwCID, LOG_RFL);

								if ( stateInfo.m_wObjectType == GATE )
								{
									RYLCreatureManager::Instance()->AddCreature( dwCID,
																				 lpMonster->m_MonsterInfo.m_strName,
																				 (char*)lpMonster->m_MonsterInfo.m_strModelingFlag,
																				 vecPos, 0.0f ,
																				 true,
																				 0,
																				 -1, NULL,
																				 true, 1, D3DXVECTOR3( vecPos.x, vecPos.y, vecPos.z ) ) ;
								}
								else
								{
									RYLCreatureManager::Instance()->AddCreature( dwCID,
																				 lpMonster->m_MonsterInfo.m_strName,
																				 (char*)lpMonster->m_MonsterInfo.m_strModelingFlag,
																				 vecPos, 0,
																				 lpMonster->m_MonsterInfo.m_bCollision ) ;
								}
							}
							break ;
						}

						pTempCreature = RYLCreatureManager::Instance()->GetCreature( dwCID ) ;
						if( !pTempCreature )
							return FALSE;

						CZ3DGeneralChrModel* pTempModel = pTempCreature->GetCharModel();
						if( !pTempModel )
							return FALSE;

						pTempCreature->SetScale( lpMonster->m_MonsterInfo.m_fSize );
						pTempModel->SetScale( lpMonster->m_MonsterInfo.m_fSize );
						pTempModel->SetDirection( posInfo.m_fDefaultDir );

						CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( pTempCreature->GetCharID() );
						if( !pTempData )
							return FALSE;

						pTempData->m_cNation	= ownerShipInfo.m_cNation;
						pTempData->m_ulGuildID  = ownerShipInfo.m_dwGID;
						pTempData->m_lCurrHP    = HPInfo.m_dwNowHP;
						pTempData->m_ulMaxHP    = HPInfo.m_dwMaxHP;

						pTempCreature->SetCampOrCastleID( ownerShipInfo.m_dwCampOrCastleID ) ;
						pTempCreature->SetOwnerID( ownerShipInfo.m_dwOwnerCID )	;
						pTempCreature->SetObjectType( stateInfo.m_wObjectType ) ;
						pTempCreature->SetState( stateInfo.m_cState ) ;
						pTempCreature->SetSubState( stateInfo.m_cSubState ) ;
						pTempCreature->SetUpgradeStep( stateInfo.m_cUpgradeStep ) ;
						pTempCreature->SetUpgradeType( stateInfo.m_cUpgradeType ) ;
						pTempCreature->SetRiders( riderInfo.m_dwRiderID ) ;

						pTempCreature->SetAction( CA_WAIT, CA_WAIT );

						if(SHORT_RANGE_CASTLE_ARMS == stateInfo.m_wObjectType || SHORT_RANGE_SIEGE_ARMS == stateInfo.m_wObjectType || CASTLE_ARMS_NPC== stateInfo.m_wObjectType )
						{
							pTempCreature->SetPosition(vecPos);							
						}

                        if ( stateInfo.m_wObjectType == EMBLEM )
						{
							// Castle에 상징물 갱신
							CRYLCastle* pCastle = CRYLCastleManager::Instance()->FindCastle( ownerShipInfo.m_dwCampOrCastleID ) ;
							if ( pCastle )
							{
								pCastle->SetEmblem( pTempCreature ) ;
							}
						}

						if ( Siege::CAMP == stateInfo.m_wObjectType || 
							 Siege::MINING_CAMP == stateInfo.m_wObjectType ||
							 Siege::CAMP_SHOP == stateInfo.m_wObjectType || 
							 Siege::KARTERANT_WEAPON == stateInfo.m_wObjectType ||
							 Siege::MERKADIA_WEAPON  == stateInfo.m_wObjectType )
						{
							CRYLCamp* pCamp = CRYLCampManager::Instance()->FindCamp( ownerShipInfo.m_dwCampOrCastleID );
							if ( pCamp )
							{
								pCamp->SetState( stateInfo.m_cState ) ;
								pCamp->UpdateMaterial( materialInfo.m_cMaterial ) ;
							}
						}

						// 성문 열리고 닫히는 처리
						if ( Siege::GATE == stateInfo.m_wObjectType )
						{
							float fDir = pTempModel->GetDirection() ;

							if ( Siege::DESTROYED == stateInfo.m_cState )
							{
								RYLCreatureManager::Instance()->FreeModel( pTempCreature->GetCharModel(), TRUE ) ;
								pTempModel = RYLCreatureManager::Instance()->AllocModel( pTempCreature, true, 7, D3DXVECTOR3( vecPos.x, vecPos.y, vecPos.z ) ) ;
								if ( pTempModel )
								{
									pTempCreature->SetPosition( vecPos ) ;
									pTempModel->SetDirection( fDir ) ;

									CCharacterLightShadowManager::CCharacterDataNode CheckNode = 
										CSceneManager::GetCharacterDataNode( pTempModel ) ;

									CheckNode.m_bHide = true ;
									CSceneManager::SetCharacterDataNode( CheckNode ) ;

									pTempCreature->SetAction( CA_WAIT, CA_WAIT ) ;
								}
							}
							else
							{
								if ( Siege::OPENED == stateInfo.m_cSubState )
								{
									RYLCreatureManager::Instance()->FreeModel( pTempCreature->GetCharModel(), TRUE ) ;
									pTempModel = RYLCreatureManager::Instance()->AllocModel( pTempCreature, true, 7, D3DXVECTOR3( vecPos.x, vecPos.y, vecPos.z ) ) ;
									if ( pTempModel )
									{
										pTempCreature->SetPosition( vecPos ) ;
										pTempModel->SetDirection( fDir ) ;

										CCharacterLightShadowManager::CCharacterDataNode CheckNode = 
											CSceneManager::GetCharacterDataNode( pTempModel ) ;

										if ( CheckNode.m_bHide )
										{
											CheckNode.m_bHide = false ;
											CSceneManager::SetCharacterDataNode( CheckNode ) ;
										}

										pTempCreature->SetAction( "open", "open" ) ;
									}
								}
								else if ( Siege::CLOSED == stateInfo.m_cSubState )
								{
									CCharacterLightShadowManager::CCharacterDataNode CheckNode = 
										CSceneManager::GetCharacterDataNode( pTempModel ) ;

									if ( CheckNode.m_bHide )
									{
										CheckNode.m_bHide = false ;
										CSceneManager::SetCharacterDataNode( CheckNode ) ;
									}
								}
							}
						}
					}			
				}

				// 병기에 충돌처리 세팅(자기 자신에 충차에 대해서만 충돌처리).
				if(EnemyCheck::IsSiegeArms(pTempCreature->GetObjectType()) || EnemyCheck::IsCastleArms(pTempCreature->GetObjectType()))
				{
					CZ3DGeneralChrModel* pTempModel = pTempCreature->GetCharModel();

					if (Siege::SHORT_RANGE_SIEGE_ARMS == stateInfo.m_wObjectType ) 						
					{
						if(pSelfCreature->GetCharID()!=ownerShipInfo.m_dwOwnerCID)
						{
							CSceneManager::CharacterCollisionDetectType( pTempModel, CDT_NONE );
							pTempCreature->SetCDTType( CDT_NONE );
						}
						else
						{
							CSceneManager::CharacterCollisionDetectType( pTempModel, CDT_FULL );
							pTempCreature->SetCDTType( CDT_FULL );
						}
					}					
					else
					{
						CSceneManager::CharacterCollisionDetectType( pTempCreature->GetCharModel(), CDT_NONE );
						pTempCreature->SetCDTType( CDT_NONE );
					}
				}		

				// 추가된 캐릭터들의 길드 정보 요청용 (길드의 중복 요청을 피하기 위해 자료 구조는 set)
				CRYLGameScene*		 pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
				CRYLGuildFrameDlg*	 pDlg   = ( CRYLGuildFrameDlg* )pScene->GetGuildFrameDlg() ;
				CRYLGuildInfoPanel*  pPanel = ( CRYLGuildInfoPanel* )pDlg->GetGuildInfoPanel() ;

				std::set<unsigned long>	setGID;

				if (0 != ownerShipInfo.m_dwGID)
				{
					if (NULL == pPanel->FindGuild(ownerShipInfo.m_dwGID & ~Guild::MEMBER_WAIT_BIT))
					{
						setGID.insert(ownerShipInfo.m_dwGID & ~Guild::MEMBER_WAIT_BIT);

						pPanel->RequestNewGuild(setGID) ;
						setGID.clear() ;
					}
				}

				if(EnemyCheck::IsCastleArms(stateInfo.m_wObjectType) || EnemyCheck::IsSiegeArms(stateInfo.m_wObjectType))
				{
					for(unsigned char cIndex = 0; cIndex < Siege::AIRSHIP_RIDER_NUM; cIndex++)
					{
						if(riderInfo.m_dwRiderID[cIndex])
						{
							RYLCreature* pOwenCreature = RYLCreatureManager::Instance()->GetCreature( riderInfo.m_dwRiderID[cIndex] ) ;

							if(pOwenCreature)
							{
								pOwenCreature->SetRideEnable(true);
							}

							RYLCreatureManager::Instance()->AddArmsCreature(riderInfo.m_dwRiderID[cIndex], dwCID);							
						}
					}					
				}
			}
			break;
			// ======================================================================================================================


			// ======================================================================================================================
			// 변경된 정보를 받아서 갱신해 줄 때
			case BroadCastSiege::BROADCAST_DELTA_DATA:
			{
				RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature() ;
				if( !pSelfCreature )
					return FALSE;

				RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( dwCID ) ;
				if ( pTempCreature )
				{
					// 순서에 주의!!!!!
					// 1. 소유권 정보 체크!!
					if ( dwUpdateFalg & BroadCastSiege::DELTA_OWNERSHIP )
					{
						BroadCastSiege::COPY_AND_ADVANCE_SRC( &ownerShipInfo, pData, sizeof(BroadCastSiege::sOwnerShipInfo) );

						// CampOrCastleID 설정
						pTempCreature->SetCampOrCastleID( ownerShipInfo.m_dwCampOrCastleID ) ;
						if ( EMBLEM == pTempCreature->GetObjectType() )
						{
							// Castle에 상징물 갱신
							CRYLCastle* pCastle = CRYLCastleManager::Instance()->FindCastle( ownerShipInfo.m_dwCampOrCastleID ) ;
							if ( pCastle )
							{
								pCastle->SetEmblem( pTempCreature ) ;
							}
						}

						// OwnerID 설정
						pTempCreature->SetOwnerID( ownerShipInfo.m_dwOwnerCID )	;

						// Nation 설정
						CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( pTempCreature->GetCharID() );
						if( !pTempData )
							return FALSE;

						pTempData->m_cNation = ownerShipInfo.m_cNation ;

						// GID 변경
						pTempData->m_ulGuildID  = ownerShipInfo.m_dwGID;

						// 추가된 캐릭터들의 길드 정보 요청용 (길드의 중복 요청을 피하기 위해 자료 구조는 set)
						CRYLGameScene*		 pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
						CRYLGuildFrameDlg*	 pDlg   = ( CRYLGuildFrameDlg* )pScene->GetGuildFrameDlg() ;
						CRYLGuildInfoPanel*  pPanel = ( CRYLGuildInfoPanel* )pDlg->GetGuildInfoPanel() ;

						std::set<unsigned long>	setGID;

						if (0 != ownerShipInfo.m_dwGID)
						{
							if (NULL == pPanel->FindGuild(ownerShipInfo.m_dwGID & ~Guild::MEMBER_WAIT_BIT))
							{
								setGID.insert(ownerShipInfo.m_dwGID & ~Guild::MEMBER_WAIT_BIT);

								pPanel->RequestNewGuild(setGID) ;
								setGID.clear() ;
							}
						}
					}

					// 2. 상태 정보 체크!!
					if ( dwUpdateFalg & BroadCastSiege::DELTA_STATE )
					{
						BroadCastSiege::COPY_AND_ADVANCE_SRC( &stateInfo, pData, sizeof(BroadCastSiege::sStateInfo) );

						WORD wRealObjType = stateInfo.m_wObjectType ;
						if ( stateInfo.m_wObjectType == EMBLEM )
						{
							wRealObjType = stateInfo.m_wObjectType + ( stateInfo.m_cState  * Siege::JEWEL_TYPE_NUM * Siege::UPGRADE_KIND_NUM ) 
																   + ( stateInfo.m_cUpgradeType * Siege::UPGRADE_KIND_NUM ) 
																   + stateInfo.m_cUpgradeStep ;
						}
						else 
						{
							wRealObjType = stateInfo.m_wObjectType + ( stateInfo.m_cState  * Siege::UPGRADE_KIND_NUM ) + stateInfo.m_cUpgradeStep ; 
						}

						const CMonsterMgr::MonsterProtoType *lpMonster = g_MonsterMgr.GetMonsterProtoType( wRealObjType ) ;

						if ( lpMonster )
						{
							/*
							CZ3DGeneralChrModel* pAddChrmodel = CSceneManager::AddCharacter( (char*)lpMonster->m_MonsterInfo.m_strModelingFlag, 
																							 "FACE01", "HAIR01" ) ;
							CZ3DGeneralChrModel* pTempModel = pTempCreature->GetCharModel();

							if ( pAddChrmodel && pTempModel )
							{
								if ( Siege::LONG_RANGE_SIEGE_ARMS == stateInfo.m_wObjectType ||
									 Siege::SHORT_RANGE_SIEGE_ARMS == stateInfo.m_wObjectType )
								{
									CSceneManager::CharacterCollisionDetectType( pAddChrmodel, CDT_ONLYTERRAIN );	// 추가..
								}

								pAddChrmodel->SetPosition( pTempModel->GetPosition() ) ;
								pAddChrmodel->SetDirection( pTempModel->GetDirection() ) ;
								CSceneManager::DeleteCharacter( pTempModel ) ;
								pTempCreature->SetCharModel( pAddChrmodel );
							}
							*/

							// CharDataNode 의 이름만 변경해준다.
							CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( dwCID );
							if ( pTempData )
							{
								pTempData->m_strName = lpMonster->m_MonsterInfo.m_strName ;
							}
							
							if ( stricmp( (char*)lpMonster->m_MonsterInfo.m_strModelingFlag, pTempCreature->GetModelFileName()->c_str() ) )
							{
								vector3 vecPos = *pTempCreature->GetPosition();
								
								pTempCreature->SetModelFileName( lpMonster->m_MonsterInfo.m_strModelingFlag ) ;
								
								CZ3DGeneralChrModel* pTempModel = NULL ;
								bool bHouse = false ;
								int  iHouseType = -1 ;

								if ( pTempCreature->GetObjectType() == Siege::CAMP )
								{
									bHouse = true ;

									switch ( pTempCreature->GetState() )
									{ 
										case Siege::DEVELOPING :	iHouseType = 2 ;  break ;
										case Siege::COMPLETE :		iHouseType = 0 ;  break ;
										default :					iHouseType = -1 ; break ;
									}

									CSceneManager::CharacterCollisionDetectType( pTempModel, CDT_NONE );
									pTempCreature->SetCDTType( CDT_NONE );
								}
								else if ( pTempCreature->GetObjectType() == Siege::KARTERANT_WEAPON )
								{
									bHouse = true;
									iHouseType = 3;

									CSceneManager::CharacterCollisionDetectType( pTempModel, CDT_NONE );
									pTempCreature->SetCDTType( CDT_NONE );
								}
								else if ( pTempCreature->GetObjectType() == Siege::MERKADIA_WEAPON )
								{
									bHouse = true;
									iHouseType = 4;

									CSceneManager::CharacterCollisionDetectType( pTempModel, CDT_NONE );
									pTempCreature->SetCDTType( CDT_NONE );
								}
								else if ( pTempCreature->GetObjectType() == Siege::MINING_CAMP )
								{
									bHouse = true;
									iHouseType = 5;

									CSceneManager::CharacterCollisionDetectType( pTempModel, CDT_NONE );
									pTempCreature->SetCDTType( CDT_NONE );
								}
								else if ( pTempCreature->GetObjectType() == Siege::CAMP_SHOP )
								{
									bHouse = true;
									iHouseType = 6;

									CSceneManager::CharacterCollisionDetectType( pTempModel, CDT_NONE );
									pTempCreature->SetCDTType( CDT_NONE );
								}
								
								RYLCreatureManager::Instance()->FreeModel( pTempCreature->GetCharModel(), TRUE ) ;
								pTempModel    = RYLCreatureManager::Instance()->AllocModel( pTempCreature, bHouse, iHouseType, D3DXVECTOR3( vecPos.x, vecPos.y, vecPos.z ) ) ;
								pTempCreature->SetPosition( vecPos ) ;
							}
						}

						// 성문 애니메이션 처리 때문에 체크
						bool bSubStateChanged = ( stateInfo.m_cSubState != pTempCreature->GetSubState() ) ;

						pTempCreature->SetObjectType( stateInfo.m_wObjectType ) ;
						pTempCreature->SetState( stateInfo.m_cState ) ;
						pTempCreature->SetSubState( stateInfo.m_cSubState ) ;
						pTempCreature->SetUpgradeStep( stateInfo.m_cUpgradeStep ) ;
						pTempCreature->SetUpgradeType( stateInfo.m_cUpgradeType ) ;

						if ( Siege::CAMP == stateInfo.m_wObjectType || 
							 Siege::MINING_CAMP == stateInfo.m_wObjectType ||
							 Siege::CAMP_SHOP == stateInfo.m_wObjectType || 
							 Siege::KARTERANT_WEAPON == stateInfo.m_wObjectType ||
							 Siege::MERKADIA_WEAPON  == stateInfo.m_wObjectType )
						{
							CRYLCamp* pCamp = CRYLCampManager::Instance()->FindCamp( pTempCreature->GetCampID() );
							if ( pCamp )
							{
								pCamp->SetState( stateInfo.m_cState ) ;
							}
						}

						// 성문 열리고 닫히는 처리
						if ( Siege::GATE == stateInfo.m_wObjectType )
						{
							vector3 vecPos = *( pTempCreature->GetPosition() ) ;
							CZ3DGeneralChrModel* pTempModel = pTempCreature->GetCharModel() ;
							if ( pTempModel )
							{
								float fDir = pTempModel->GetDirection() ;
								
								if ( Siege::DESTROYED == stateInfo.m_cState )
								{
									RYLCreatureManager::Instance()->FreeModel( pTempCreature->GetCharModel(), TRUE ) ;
									pTempModel = RYLCreatureManager::Instance()->AllocModel( pTempCreature, true, 7, D3DXVECTOR3( vecPos.x, vecPos.y, vecPos.z ) ) ;
									if ( pTempModel )
									{
										pTempCreature->SetPosition( vecPos ) ;
										pTempModel->SetDirection( fDir ) ;

										CCharacterLightShadowManager::CCharacterDataNode CheckNode = 
											CSceneManager::GetCharacterDataNode( pTempModel ) ;

										CheckNode.m_bHide = true ;
										CSceneManager::SetCharacterDataNode( CheckNode ) ;

										pTempCreature->SetAction( CA_WAIT, CA_WAIT ) ;
									}
								}
								else
								{
									if ( bSubStateChanged && Siege::OPENED == stateInfo.m_cSubState )
									{
										RYLCreatureManager::Instance()->FreeModel( pTempCreature->GetCharModel(), TRUE ) ;
										pTempModel = RYLCreatureManager::Instance()->AllocModel( pTempCreature, true, 7, D3DXVECTOR3( vecPos.x, vecPos.y, vecPos.z ) ) ;
										if ( pTempModel )
										{
											pTempCreature->SetPosition( vecPos ) ;
											pTempModel->SetDirection( fDir ) ;

											CCharacterLightShadowManager::CCharacterDataNode CheckNode = 
												CSceneManager::GetCharacterDataNode( pTempModel ) ;

											if ( CheckNode.m_bHide )
											{
												CheckNode.m_bHide = false ;
												CSceneManager::SetCharacterDataNode( CheckNode ) ;
											}

											pTempCreature->SetAction( "open", "open" ) ;
										}
									}
									else if ( bSubStateChanged && Siege::CLOSED == stateInfo.m_cSubState )
									{
										RYLCreatureManager::Instance()->FreeModel( pTempCreature->GetCharModel(), TRUE ) ;
										pTempModel = RYLCreatureManager::Instance()->AllocModel( pTempCreature, true, 1, D3DXVECTOR3( vecPos.x, vecPos.y, vecPos.z ) ) ;
										if ( pTempModel )
										{
											pTempCreature->SetPosition( vecPos ) ;
											pTempModel->SetDirection( fDir ) ;

											CCharacterLightShadowManager::CCharacterDataNode CheckNode = 
												CSceneManager::GetCharacterDataNode( pTempModel ) ;

											if ( CheckNode.m_bHide )
											{
												CheckNode.m_bHide = false ;
												CSceneManager::SetCharacterDataNode( CheckNode ) ;
											}
										}
									}
								}
							}
						}
					}

					// 3. HP 정보 체크!!
					if ( dwUpdateFalg & BroadCastSiege::DELTA_HP )
					{
						BroadCastSiege::COPY_AND_ADVANCE_SRC( &HPInfo, pData, sizeof(BroadCastSiege::sHPInfo) );

						CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( pTempCreature->GetCharID() );
						if( !pTempData )
							return FALSE;

						pTempData->m_lCurrHP    = HPInfo.m_dwNowHP;
						pTempData->m_ulMaxHP    = HPInfo.m_dwMaxHP;
					}

					// 4. 위치 정보 체크!!
					if ( dwUpdateFalg & BroadCastSiege::DELTA_POS )
					{
						BroadCastSiege::COPY_AND_ADVANCE_SRC( &posInfo, pData, sizeof(BroadCastSiege::sPosInfo) );

						CZ3DGeneralChrModel* pTempModel = pTempCreature->GetCharModel();
						if ( pTempModel && dwCID != pSelfCreature->GetCharID() )
						{
							pTempModel->SetDirection( posInfo.m_NetworkPos.GetDirection() );

							/*vector3 vecPos;
							vecPos.x = posInfo.m_NetworkPos.GetXPos() * 100.0f ;
							vecPos.y = posInfo.m_NetworkPos.GetYPos() * 100.0f ;
							vecPos.z = posInfo.m_NetworkPos.GetZPos() * 100.0f ; 


							pTempCreature->SetPosition( vecPos ) ;*/
						}
					}

					// 5. 자재 정보 체크!!
					if ( dwUpdateFalg & BroadCastSiege::DELTA_MATERIAL )
					{
						BroadCastSiege::COPY_AND_ADVANCE_SRC( &materialInfo, pData, sizeof(BroadCastSiege::sMaterialInfo) );
						
						switch ( pTempCreature->GetObjectType() )
						{
							case Siege::CAMP :
							case Siege::MINING_CAMP :
							case Siege::CAMP_SHOP :
							case Siege::KARTERANT_WEAPON :
							case Siege::MERKADIA_WEAPON :
							{
								CRYLCamp* pCamp = CRYLCampManager::Instance()->FindCamp( pTempCreature->GetCampID() );
								if ( pCamp )
								{
									pCamp->UpdateMaterial( materialInfo.m_cMaterial ) ;
								}
							}
						}
					}

					// 6. 탑승자 정보 체크!!
					if ( dwUpdateFalg & BroadCastSiege::DELTA_RIDER )
					{
						BroadCastSiege::COPY_AND_ADVANCE_SRC( &riderInfo, pData, sizeof(BroadCastSiege::sRiderInfo) );

						pTempCreature->SetRiders( riderInfo.m_dwRiderID ) ;

						for(unsigned char cIndex = 0; cIndex < Siege::AIRSHIP_RIDER_NUM; cIndex++)
						{
							if(riderInfo.m_dwRiderID[cIndex])
							{																
								RYLCreature* pOwenCreature = RYLCreatureManager::Instance()->GetCreature( riderInfo.m_dwRiderID[cIndex] ) ;

								if(pOwenCreature)
								{
									pOwenCreature->SetRideEnable(true);
								}
								
								RYLCreatureManager::Instance()->AddArmsCreature(riderInfo.m_dwRiderID[cIndex], dwCID);								
							}						
						}						
					}
				}
			}
			break;
			// ======================================================================================================================

			// ======================================================================================================================
			// 변경된 정보를 받아서 갱신해 줄 때
			case BroadCastSiege::BROADCAST_DELETE_DATA:
			{
				RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature() ;
				if( !pSelfCreature )
					return FALSE;

				RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( dwCID ) ;
				if ( pTempCreature )
				{
					for(unsigned char cIndex = 0; cIndex < Siege::AIRSHIP_RIDER_NUM; cIndex++)
					{
						unsigned long dwRideCID = pTempCreature->GetRiderID(cIndex);

						if(dwRideCID)
						{																
							RYLCreature* pOwenCreature = RYLCreatureManager::Instance()->GetCreature(dwRideCID) ;

							if(pOwenCreature)
							{
								pOwenCreature->SetRideEnable(false);
							}
							
							RYLCreatureManager::Instance()->DeleteArmsCreature(dwRideCID);							
						}						
					}						

					GameProcess::LogCharUnload( dwCID, LOG_RFL ) ;
					RYLCreatureManager::Instance()->DeleteCreature( dwCID, TRUE ) ;
				}
			}
			break;
			// ======================================================================================================================
		}
	}

	return FALSE;
}

GAME_EVENT_FUNC(ParseCastleRight)
{
	unsigned long	dwCastleID ;
	CastleRight		castleRight ; 

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharCastleRight( lpPktBase, 
																						&dwCastleID, 
																						&castleRight ) ;
	if(0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		CRYLCastle* lpCastle = CRYLCastleManager::Instance()->FindCastle( dwCastleID ) ;
		if ( lpCastle )
		{
			lpCastle->SetCastleRight( castleRight ) ;

			CRYLChattingDlg::Instance()->AddMessage( CRYLStringTable::m_strString[ 2611 ], CRYLNetworkData::Instance()->m_dwMyChrID, 128 ) ;
		}
	}
	else
	{
		CRYLMessageBox *lpMessageBox;
		switch (CRYLNetworkData::Instance()->m_dwNetworkError)
		{
			case PktCastleRight::FAIL_NOT_EXIST_GUILD :		// 존재 하지 않는 길드
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 2582 ]);
				break;
			}

			case PktCastleRight::FAIL_NOT_EXIST_CASTLE :	// 존재 하지 않는 성
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 2583 ]);
				break;
			}

			case PktCastleRight::FAIL_DIFFERENT_GUILD :		// 길드 아이디가 다름
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 2584 ]);
				break;
			}

			case PktCastleRight::FAIL_NOT_MASTER :			// 길드 마스터가 아님
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 2601 ]);
				break;
			}
		}
	}

	return FALSE ;
}

GAME_EVENT_FUNC(ParseCampRight)
{
	unsigned long dwCID ;
	unsigned long dwCampID ; 
	CampRight	  campRight ;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharCampRight( lpPktBase, 
																					  &dwCID,
																					  &dwCampID, 
																					  &campRight);
	if(0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		CRYLCamp* lpCamp = CRYLCampManager::Instance()->FindCamp( dwCampID ) ;
		if ( lpCamp )
		{
			lpCamp->SetCampRight( campRight ) ;

			// 공성 관련 팝업창이 떠 있었다면 닫아준다.
			CRYLGameData* pGame = CRYLGameData::Instance();
			if ( Creature::CT_SIEGE_OBJECT == pGame->GetClientIdentity(pGame->m_lpInterface->m_dwPopupSelectChrID) )
			{
				pGame->m_lpInterface->m_bPopupShow = FALSE;
				pGame->m_lpInterface->m_dwPopupSelect = 0xFFFFFFFF;
			}
		}
	}
	else
	{
		CRYLMessageBox *lpMessageBox;
		switch (CRYLNetworkData::Instance()->m_dwNetworkError)
		{
			case PktCampRight::FAIL_NOT_EXIST_GUILD :		// 존재 하지 않는 길드
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 2582 ]);
				break;
			}

			case PktCampRight::FAIL_NOT_EXIST_CAMP :		// 존재 하지 않는 진지
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 2612 ]);
				break;
			}

			case PktCampRight::FAIL_NOT_CAMP :				// 캠프가 아님
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 2598 ]);
				break;
			}

			case PktCampRight::FAIL_DIFFERENT_GUILD :		// 길드 아이디가 다름
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 2600 ]);
				break;
			}

			case PktCampRight::FAIL_CAMP_STATE :			// 진지 상태가 이상함
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 2599 ]);
				break;
			}

			case PktCampRight::FAIL_NOT_MASTER :			// 길드 마스터가 아님
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 2601 ]);
				break;
			}
		}
	}

	return FALSE ;
}

GAME_EVENT_FUNC(ParseGameTimeInfo)
{
	GuildTimeInfo guildTimeInfo ;
	RealmTimeInfo realmTimeInfo ;
	SiegeTimeInfo siegeTimeInfo ;
	EnterTimeInfo enterTimeInfo ;
	EventTimeInfo eventTimeInfo ;
    CastleSiegeInfo castleSiegeInfo[ Castle::MAX_CASTLE_NUM ] ;
	unsigned char cType = 0 ;
	unsigned char cCastleNum = 0 ;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharGameTimeInfo( lpPktBase, &guildTimeInfo, &realmTimeInfo, &siegeTimeInfo, &enterTimeInfo, &eventTimeInfo,
																						 &cType, &cCastleNum, castleSiegeInfo) ;

	if ( 0 == CRYLNetworkData::Instance()->m_dwNetworkError )
	{
		unsigned long dwOldGuildTime = CRYLGameTimeManager::Instance()->GetNowGuildTime();
		unsigned long dwOldRealmTime = CRYLGameTimeManager::Instance()->GetNowRealmTime();
		unsigned long dwOldSiegeTime = CRYLGameTimeManager::Instance()->GetNowSiegeTime();
		unsigned long dwOldEnterTime = CRYLGameTimeManager::Instance()->GetNowEnterTime();
		unsigned long dwOldEventTime = CRYLGameTimeManager::Instance()->GetNowEventTime();

		CRYLGameTimeManager::Instance()->SetTimeValue( cType, guildTimeInfo, realmTimeInfo, siegeTimeInfo, enterTimeInfo, eventTimeInfo) ;

		for (int i=0; i<cCastleNum; ++i)
		{
			CRYLCastle* pCastle = CRYLCastleManager::Instance()->FindCastle( castleSiegeInfo[i].m_dwCastleID ) ;
			if ( pCastle )
			{
				pCastle->SetEnableSiege( castleSiegeInfo[i].m_bEnableSiege ) ;

                // 공성 시간이 끝나면, 세율 변경 가능 정보를 초기화한다.
				if ( GameTime::TYPE_CHANGE_GAME_TIME == cType && 
					 CRYLGameTimeManager::Instance()->IsSiegeWarTime(dwOldSiegeTime) &&
					 !CRYLGameTimeManager::Instance()->IsSiegeWarTime())
				{
					pCastle->EnableTaxChange() ;
				}
			}
		}

		// 피아식별 룰 셋팅
		switch ( CRYLNetworkData::Instance()->m_dwPresentZone )
		{
			case SERVER_ID::CAPITAL :
		//	case 100/*SERVER_ID::CAPITAL*/ :
			{
				// 컨텐츠 : 공성전
				if (true == CRYLNetworkData::Instance()->UseContents(GameRYL::SIEGE)) 

				{
					if (CRYLGameTimeManager::Instance()->IsGuildWarTime())
					{
						EnemyCheck::CCheckTable::GetInstance().SetEnemyCheckRule( EnemyCheck::RULE_GUILD_WAR ) ;
					}
					else if (CRYLGameTimeManager::Instance()->IsSiegeWarTime())
					{
						EnemyCheck::CCheckTable::GetInstance().SetEnemyCheckRule( EnemyCheck::RULE_SIEGE_WAR ) ;
					}

					else
					{
						EnemyCheck::CCheckTable::GetInstance().SetEnemyCheckRule( EnemyCheck::RULE_REALM_WAR ) ;
					}
				}
				else
				{
					if (CRYLGameTimeManager::Instance()->IsGuildWarTime())
					{
						EnemyCheck::CCheckTable::GetInstance().SetEnemyCheckRule( EnemyCheck::RULE_GUILD_WAR ) ;
					}
					else
					{
						EnemyCheck::CCheckTable::GetInstance().SetEnemyCheckRule( EnemyCheck::RULE_REALM_WAR ) ;
					}
				}

				{
				
				if (true == CRYLNetworkData::Instance()->UseContents(GameRYL::STONE_BATTLE))
				{
					if (CRYLGameTimeManager::Instance()->IsRealmWarTime())
					{
						EnemyCheck::CCheckTable::GetInstance().SetEnemyCheckRule( EnemyCheck::RULE_STATUE_WAR ) ;
					}
					else
					{
						EnemyCheck::CCheckTable::GetInstance().SetEnemyCheckRule( EnemyCheck::RULE_REALM_WAR ) ;
					}
				}
				else
				{
					EnemyCheck::CCheckTable::GetInstance().SetEnemyCheckRule( EnemyCheck::RULE_REALM_WAR ) ;
				}
			}
		}
			break ;





	//		case SERVER_ID::CAPITAL : // was war1
			case SERVER_ID::STONE_WAR1 :
			case SERVER_ID::STONE_WAR2 :
			case SERVER_ID::STONE_WAR3 :
			{
				
				if (true == CRYLNetworkData::Instance()->UseContents(GameRYL::STONE_BATTLE))
				{
					if (CRYLGameTimeManager::Instance()->IsRealmWarTime())
					{
						EnemyCheck::CCheckTable::GetInstance().SetEnemyCheckRule( EnemyCheck::RULE_STATUE_WAR ) ;
					}
					else
					{
						EnemyCheck::CCheckTable::GetInstance().SetEnemyCheckRule( EnemyCheck::RULE_REALM_WAR ) ;
					}
				}
				else
				{
					EnemyCheck::CCheckTable::GetInstance().SetEnemyCheckRule( EnemyCheck::RULE_REALM_WAR ) ;
				}
			}
			break ;




				
			case SERVER_ID::ZONE12 :
			{
				EnemyCheck::CCheckTable::GetInstance().SetEnemyCheckRule( EnemyCheck::RULE_REALM_WAR ) ;
			}
			break ;

			case SERVER_ID::ZONE1:
			case SERVER_ID::ZONE2:
			case SERVER_ID::ZONE4:
			case SERVER_ID::ZONE5:
			{
				EnemyCheck::CCheckTable::GetInstance().SetEnemyCheckRule( EnemyCheck::RULE_HUNTING ) ;
			}
			break ;

			default :
			{
				EnemyCheck::CCheckTable::GetInstance().SetEnemyCheckRule( EnemyCheck::RULE_REALM_WAR ) ;
//				EnemyCheck::CCheckTable::GetInstance().SetEnemyCheckRule( EnemyCheck::RULE_HUNTING ) ;
			}
			break ;
		}

		// 길드창이 열려 있으면 길드 탈퇴 버튼을 활성화 혹은 비활성화 시킨다.

		if (GameTime::TYPE_CHARACTER_LOGIN == cType || GameTime::TYPE_CHANGE_GAME_TIME == cType)
		{
			CRYLGameScene*			pScene		= ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
			CRYLGuildFrameDlg*		pFrameDlg	= ( CRYLGuildFrameDlg* )pScene->GetGuildFrameDlg() ;
			CRYLGuildMemberPanel*	pMemPanel	= ( CRYLGuildMemberPanel* )pFrameDlg->GetGuildMemberPanel() ;
			CRYLGuildTacticsPanel*	pTacPanel	= ( CRYLGuildTacticsPanel* )pFrameDlg->GetGuildTacticsPanel() ;

			if ( pMemPanel->GetVisible() )
			{
				pMemPanel->UpdateGuildMemberPanel() ;
			}

			if ( pTacPanel->GetVisible() )
			{
				pTacPanel->UpdateGuildTacticsPanel() ;
			}
		}

		// 전상 상태 확인창 값 변경
		if (GameTime::TYPE_CHARACTER_LOGIN == cType || GameTime::TYPE_CHANGE_GAME_TIME == cType)
		{
			CRYLGameScene*			pScene		= ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
			CRYLSiegeTimeDlg*		pDlg		= ( CRYLSiegeTimeDlg* )pScene->GetSiegeTimeDlg() ;
			CRYLDialogControlDlg*	pControlDlg	= (CRYLDialogControlDlg*)pScene->GetDialogControlDlg();

			pDlg->SetGuildType(guildTimeInfo.m_cNowGuildTime);
			pDlg->SetRealmType(realmTimeInfo.m_cNowRealmTime);

			if (CRYLGameTimeManager::Instance()->IsGuildWarTime(guildTimeInfo.m_cNowGuildTime))
			{
				pControlDlg->SetWarTypeButton(CRYLDialogControlDlg::GUILD_WAR_TYPE);
			}
			else if (CRYLGameTimeManager::Instance()->IsRealmWarTime(realmTimeInfo.m_cNowRealmTime))
			{
				pControlDlg->SetWarTypeButton(CRYLDialogControlDlg::REALM_WAR_TYPE);
			}			
			else
			{
				pControlDlg->SetWarTypeButton(CRYLDialogControlDlg::NONE_TYPE);
			}
		}

		
		// 공지 메세지 출력
		// -----------------------------------------------------------------------------------
		// !! 공지 메세지를 컨텐츠 별로 묶어서 관리해야 함..!! 길드전 추가 이후 수정해야 함!!
		// -----------------------------------------------------------------------------------
		if (GameTime::TYPE_CHANGE_GAME_TIME == cType)
		{
			char szNotice[ MAX_PATH ] = {0, };

			if (guildTimeInfo.m_bUpdated)
			{
				unsigned long dwRemainTime = guildTimeInfo.m_TimeInfo.m_dwTime / 60 ;
				if ((guildTimeInfo.m_TimeInfo.m_dwTime % 60) > 0) ++dwRemainTime;

				switch (guildTimeInfo.m_cNowGuildTime)
				{
					case GameTime::GT_NORMAL:
					{
						if ( CRYLNetworkData::Instance()->m_dwPresentZone == SERVER_ID::CAPITAL )
						{
							sprintf(szNotice, CRYLStringTable::m_strString[ 3834 ]);
							CRYLNoticeManager::Instance()->AddNotice( szNotice, NOTICECOLOR::NORMAL, NOTICETIME::NORMAL, TRUE );

							// 길드 전쟁 종료 효과음
/*							char Temp[MAX_PATH]; 
							sprintf( Temp, "%s\\Sound\\gong.mnd", g_ClientMain.m_strClientPath ) ;
							CSound* pSound; 
							CSoundMgr::_GetInstance()->GetSound( pSound, Temp ); 
							pSound->Play(); 
*/
						}
					}
					break;

					case GameTime::GT_PREPARE_START_30:
					{
						sprintf(szNotice, CRYLStringTable::m_strString[ 3835 ], dwRemainTime);
						CRYLNoticeManager::Instance()->AddNotice( szNotice, NOTICECOLOR::NORMAL, NOTICETIME::NORMAL, TRUE );
						
						sprintf(szNotice, CRYLStringTable::m_strString[ 3836 ]);
						CRYLNoticeManager::Instance()->AddNotice( szNotice, NOTICECOLOR::NORMAL, NOTICETIME::NORMAL, TRUE );
					}
					break;

					case GameTime::GT_PREPARE_START_5:
					{
						sprintf(szNotice, CRYLStringTable::m_strString[ 3837 ], dwRemainTime, dwRemainTime);
						CRYLNoticeManager::Instance()->AddNotice( szNotice, NOTICECOLOR::NORMAL, NOTICETIME::NORMAL, TRUE );

						sprintf(szNotice, CRYLStringTable::m_strString[ 3838 ]);
						CRYLNoticeManager::Instance()->AddNotice( szNotice, NOTICECOLOR::NORMAL, NOTICETIME::NORMAL, TRUE );
					}
					break;

					case GameTime::GT_GUILD_WAR:
					{
						if ( CRYLNetworkData::Instance()->m_dwPresentZone == SERVER_ID::CAPITAL )
						{
							sprintf(szNotice, CRYLStringTable::m_strString[ 3839 ]);
							CRYLNoticeManager::Instance()->AddNotice( szNotice, NOTICECOLOR::NORMAL, NOTICETIME::NORMAL, TRUE );

							// 길드 전쟁 시작 효과음
/*							char Temp[MAX_PATH]; 
							sprintf( Temp, "%s\\Sound\\gong.mnd", g_ClientMain.m_strClientPath ) ;
							CSound* pSound; 
							CSoundMgr::_GetInstance()->GetSound( pSound, Temp ); 
							pSound->Play(); 
*/
						}
					}
					break;

					case GameTime::GT_PREPARE_END_30:
					{
						sprintf(szNotice, CRYLStringTable::m_strString[ 3840 ], dwRemainTime );
						CRYLNoticeManager::Instance()->AddNotice( szNotice, NOTICECOLOR::NORMAL, NOTICETIME::NORMAL, TRUE );
					}
					break;
				}
			}

			// 컨텐츠 : 다크 카나번 국가 전쟁
			if (true == CRYLNetworkData::Instance()->UseContents(GameRYL::STONE_BATTLE))
			{
				if (realmTimeInfo.m_bUpdated)
				{
					unsigned long dwRemainTime = realmTimeInfo.m_TimeInfo.m_dwTime / 60 ;
					if ((realmTimeInfo.m_TimeInfo.m_dwTime % 60) > 0) ++dwRemainTime;

					switch (realmTimeInfo.m_cNowRealmTime)
					{
						case GameTime::RT_NORMAL:
						{
							sprintf(szNotice, CRYLStringTable::m_strString[ 3841 ]);
							CRYLNoticeManager::Instance()->AddNotice( szNotice, NOTICECOLOR::NORMAL, NOTICETIME::NORMAL, TRUE );

							if (/*SERVER_ID::STONE_WAR1*/SERVER_ID::CAPITAL <= CRYLNetworkData::Instance()->m_dwPresentZone && CRYLNetworkData::Instance()->m_dwPresentZone <= SERVER_ID::STONE_WAR3)
							{
								// 국가 전쟁 종료 효과음
								char Temp[MAX_PATH]; 
								sprintf( Temp, "%s\\Sound\\WarEnd.mnd", g_ClientMain.m_strClientPath ) ;
								CSound* pSound; 
								CSoundMgr::_GetInstance()->GetSound( pSound, Temp ); 
								pSound->Play(); 
							}
						}
						break;

						case GameTime::RT_PREPARE_START_30:
						{
							sprintf(szNotice, CRYLStringTable::m_strString[ 3842 ], dwRemainTime);
							CRYLNoticeManager::Instance()->AddNotice( szNotice, NOTICECOLOR::NORMAL, NOTICETIME::NORMAL, TRUE );
						}
						break;

						case GameTime::RT_PREPARE_START_15:
						{
							sprintf(szNotice, CRYLStringTable::m_strString[ 3843 ], dwRemainTime, dwRemainTime - 10, dwRemainTime);
							CRYLNoticeManager::Instance()->AddNotice( szNotice, NOTICECOLOR::NORMAL, NOTICETIME::NORMAL, TRUE );
						}
						break;

						case GameTime::RT_PREPARE_START_10:
						break;

						case GameTime::RT_PREPARE_START_5:
						{
							if ( /*SERVER_ID::STONE_WAR1*/SERVER_ID::CAPITAL <= CRYLNetworkData::Instance()->m_dwPresentZone && CRYLNetworkData::Instance()->m_dwPresentZone <= SERVER_ID::STONE_WAR3 )
							{
								sprintf(szNotice, CRYLStringTable::m_strString[ 3844 ], dwRemainTime);
								CRYLNoticeManager::Instance()->AddNotice( szNotice, NOTICECOLOR::NORMAL, NOTICETIME::NORMAL, TRUE );

								// bgm 재생
								char Temp[MAX_PATH]; 
								sprintf( Temp, "%s\\Sound\\bgm\\WarReady.ogg", g_ClientMain.m_strClientPath ) ;
								CSceneManager::ms_pSceneEventMgr->PlayEventMusic( Temp, false ) ;
							}
						}
						break;

						case GameTime::RT_REALM_WAR:
						{
							if ( /*SERVER_ID::STONE_WAR1*/SERVER_ID::CAPITAL <= CRYLNetworkData::Instance()->m_dwPresentZone && CRYLNetworkData::Instance()->m_dwPresentZone <= SERVER_ID::STONE_WAR3 )
							{
								sprintf(szNotice, CRYLStringTable::m_strString[ 3845 ]);
								CRYLNoticeManager::Instance()->AddNotice( szNotice, NOTICECOLOR::NORMAL, NOTICETIME::NORMAL, TRUE );

								// 국가 전쟁 시작 효과음
								char Temp[MAX_PATH]; 
								sprintf( Temp, "%s\\Sound\\WarStart.mnd", g_ClientMain.m_strClientPath ) ;
								CSound* pSound; 
								CSoundMgr::_GetInstance()->GetSound( pSound, Temp ); 
								pSound->Play(); 
							}
						}
						break;
					}
				}
			}


			// 컨텐츠 : 공성전
			if (true == CRYLNetworkData::Instance()->UseContents(GameRYL::SIEGE))
			{
				if (siegeTimeInfo.m_bUpdated)
				{
					unsigned long dwRemainTime = siegeTimeInfo.m_TimeInfo.m_dwTime / 60 ;
					if ((siegeTimeInfo.m_TimeInfo.m_dwTime % 60) > 0) ++dwRemainTime;

					switch (siegeTimeInfo.m_cNowSiegeTime)
					{
						case GameTime::ST_NORMAL:
						{
							sprintf(szNotice, CRYLStringTable::m_strString[ 4065 ]);
							CRYLNoticeManager::Instance()->AddNotice( szNotice, NOTICECOLOR::NORMAL, NOTICETIME::NORMAL, TRUE );

							for ( int i = 0; i < cCastleNum; ++i )
							{
								CRYLCastle* pCastle = CRYLCastleManager::Instance()->FindCastle( castleSiegeInfo[ i ].m_dwCastleID ) ;
								if ( pCastle && Creature::STATELESS != pCastle->GetNation() )
								{
									switch ( pCastle->GetNation() )
									{
										case Creature::KARTERANT :
										{
											sprintf( szNotice, CRYLStringTable::m_strString[ 4096 ], CRYLStringTable::m_strString[ 3100 ], pCastle->GetCastleName() );
											CRYLNoticeManager::Instance()->AddNotice( szNotice, NOTICECOLOR::NORMAL, NOTICETIME::NORMAL, TRUE );
											break ;
										}

										case Creature::MERKADIA :
										{
											sprintf( szNotice, CRYLStringTable::m_strString[ 4096 ], CRYLStringTable::m_strString[ 3101 ], pCastle->GetCastleName() );
											CRYLNoticeManager::Instance()->AddNotice( szNotice, NOTICECOLOR::NORMAL, NOTICETIME::NORMAL, TRUE );
											break ;
										}
									}
								}
							}

							sprintf(szNotice, CRYLStringTable::m_strString[ 4099 ]);
							CRYLNoticeManager::Instance()->AddNotice( szNotice, NOTICECOLOR::NORMAL, NOTICETIME::NORMAL, TRUE );
						}
						break;

						case GameTime::ST_PREPARE_START_5:
						{
							sprintf(szNotice, CRYLStringTable::m_strString[ 4097 ], dwRemainTime);
							CRYLNoticeManager::Instance()->AddNotice( szNotice, NOTICECOLOR::NORMAL, NOTICETIME::NORMAL, TRUE );
							
							sprintf(szNotice, CRYLStringTable::m_strString[ 4106 ], dwRemainTime);
							CRYLNoticeManager::Instance()->AddNotice( szNotice, NOTICECOLOR::NORMAL, NOTICETIME::NORMAL, TRUE );
						}
						break;
						
						case GameTime::ST_SIEGE_WAR:
						{
							sprintf(szNotice, CRYLStringTable::m_strString[ 4066 ]);
							CRYLNoticeManager::Instance()->AddNotice( szNotice, NOTICECOLOR::NORMAL, NOTICETIME::NORMAL, TRUE );
						}
						break;

						case GameTime::ST_PREPARE_END_5:
						{
							sprintf(szNotice, CRYLStringTable::m_strString[ 4098 ], dwRemainTime);
							CRYLNoticeManager::Instance()->AddNotice( szNotice, NOTICECOLOR::NORMAL, NOTICETIME::NORMAL, TRUE );

							sprintf(szNotice, CRYLStringTable::m_strString[ 4107 ], dwRemainTime);
							CRYLNoticeManager::Instance()->AddNotice( szNotice, NOTICECOLOR::NORMAL, NOTICETIME::NORMAL, TRUE );
						}
						break;
					}
				}
			}


			// 컨텐츠 : 이벤트
			if (eventTimeInfo.m_bUpdated)
			{
				unsigned long dwRemainTime = eventTimeInfo.m_TimeInfo.m_dwTime / 60 ;
				if ((eventTimeInfo.m_TimeInfo.m_dwTime % 60) > 0) ++dwRemainTime;

				switch (eventTimeInfo.m_cNowEventTime)
				{
				case GameTime::EVENT_NORMAL:
					{
						// 이벤트가 종료되었습니다. 메시지 출력
						sprintf(szNotice, CRYLStringTable::m_strString[ 4401 ]);
						CRYLNoticeManager::Instance()->AddNotice( szNotice, NOTICECOLOR::NORMAL, NOTICETIME::NORMAL, TRUE );
					}
					break;

				case GameTime::EVENT_All20:
					{
						sprintf(szNotice, CRYLStringTable::m_strString[ 4402 ], dwRemainTime);
						CRYLNoticeManager::Instance()->AddNotice( szNotice, NOTICECOLOR::NORMAL, NOTICETIME::NORMAL, TRUE );
					}
					break;

				case GameTime::EVENT_All50:
					{
						sprintf(szNotice, CRYLStringTable::m_strString[ 4403 ]);
						CRYLNoticeManager::Instance()->AddNotice( szNotice, NOTICECOLOR::NORMAL, NOTICETIME::NORMAL, TRUE );
					}
					break;
				}
			}


		}
	}
	else
	{
		;// Error
	}

	return FALSE ;
}

GAME_EVENT_FUNC(ParseStealthInfo)
{
	bool bUseStealth ;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharStealthInfo( lpPktBase, 
																					    &bUseStealth ) ;

	if(0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		CRYLGameData::Instance()->m_csStatus.SetUseStealth( bUseStealth ) ;
	}
	else
	{
		;// Error
	}

	return FALSE ;
}


GAME_EVENT_FUNC(ParseCampMessage)
{
	char					szGuildName[ Guild::MAX_GUILD_NAME_LEN ] ;
	char					szMasterName[ PktCampMessage::MAX_NAME_LEN ] ;
	char					szPCName[ PktCampMessage::MAX_NAME_LEN ] ;
	unsigned long			dwCampID;
	unsigned char			cMsgCmd;
	unsigned char			cZone;
	unsigned char			cRemainTime;
	unsigned char			cNation;
	PktCampMessage::sRight	Right;
	bool					bNotify;
	int						nValue1;
	int						nValue2;
	unsigned char			cTimeChk;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharCampMessage( lpPktBase,
																						szGuildName,
																						szMasterName,
																						szPCName,
																						&cMsgCmd,
																						&dwCampID,
																						&cZone,
																						&cRemainTime,
																						&cNation,
																						&Right,
																						&bNotify,
																						&nValue1,
																						&nValue2,
																						&cTimeChk) ;
	
	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		char strMessage[ MAX_PATH ] = "" ;
		CRYLChattingDlg* pChat	= CRYLChattingDlg::Instance() ;

		char szZoneName[ SERVER_ID::MAX_ZONE_NUM ][ MAX_PATH ] = { 0, };

		strncpy(szZoneName[SERVER_ID::ZONE1], CRYLStringTable::m_strString[ 1154 ], MAX_PATH);
		strncpy(szZoneName[SERVER_ID::ZONE2], CRYLStringTable::m_strString[ 1155 ], MAX_PATH);
		strncpy(szZoneName[SERVER_ID::ZONE4], CRYLStringTable::m_strString[ 1157 ], MAX_PATH);
		strncpy(szZoneName[SERVER_ID::ZONE5], CRYLStringTable::m_strString[ 1158 ], MAX_PATH);
		strncpy(szZoneName[SERVER_ID::CAPITAL], CRYLStringTable::m_strString[ 1160 ], MAX_PATH);
		strncpy(szZoneName[SERVER_ID::ZONE9], CRYLStringTable::m_strString[ 1166 ], MAX_PATH);
		strncpy(szZoneName[SERVER_ID::ZONE12], CRYLStringTable::m_strString[ 2734 ], MAX_PATH);
		strncpy(szZoneName[/*SERVER_ID::STONE_WAR1*/SERVER_ID::CAPITAL], CRYLStringTable::m_strString[ 1156 ], MAX_PATH);
		strncpy(szZoneName[SERVER_ID::STONE_WAR2], CRYLStringTable::m_strString[ 1156 ], MAX_PATH);
		strncpy(szZoneName[SERVER_ID::STONE_WAR3], CRYLStringTable::m_strString[ 1156 ], MAX_PATH);

		switch (cMsgCmd)
		{
			case PktCampMessage::MSGCMD_ENCAMP_START :			// 길드 요새 구축 시작
			{
				sprintf( strMessage, CRYLStringTable::m_strString[ 3000 ], szMasterName, szZoneName[cZone], cRemainTime ) ;
				pChat->AddMessage( strMessage, CRYLNetworkData::Instance()->m_dwMyChrID, 300 ) ;
			}
			break ;

			case PktCampMessage::MSGCMD_ENCAMPING :				// 길드 요새 구축 중
			{
				sprintf( strMessage, CRYLStringTable::m_strString[ 3001 ], szZoneName[cZone], cRemainTime ) ;
				pChat->AddMessage( strMessage, CRYLNetworkData::Instance()->m_dwMyChrID, 300 ) ;
			}
			break ;
	
			case PktCampMessage::MSGCMD_ENCAMP_END :			// 길드 요새 구축 완료
			{
				if ( bNotify )
				{
					sprintf( strMessage, CRYLStringTable::m_strString[ 3002 ], szZoneName[cZone], szGuildName ) ;
					CRYLNoticeManager::Instance()->AddNotice( strMessage, NOTICECOLOR::WAR, NOTICETIME::NORMAL, TRUE ) ;
				}
				else
				{
					sprintf( strMessage, CRYLStringTable::m_strString[ 3003 ], szZoneName[cZone] ) ;
					pChat->AddMessage( strMessage, CRYLNetworkData::Instance()->m_dwMyChrID, 300 ) ;
				}
			}
			break ;

			case PktCampMessage::MSGCMD_CANCEL_START :			// 길드 요새 구축 취소
			{
				sprintf( strMessage, CRYLStringTable::m_strString[ 3004 ], szMasterName, szZoneName[cZone], cRemainTime ) ;
				pChat->AddMessage( strMessage, CRYLNetworkData::Instance()->m_dwMyChrID, 300 ) ;
			}
			break ;

			case PktCampMessage::MSGCMD_CANCEL_END :			// 길드 요새 구축 취소 완료
			{
				sprintf( strMessage, CRYLStringTable::m_strString[ 3005 ], szZoneName[cZone] ) ;
				pChat->AddMessage( strMessage, CRYLNetworkData::Instance()->m_dwMyChrID, 300 ) ;
			}
			break ;

			case PktCampMessage::MSGCMD_REPAIR_START :			// 길드 요새 수리
			{
				sprintf( strMessage, CRYLStringTable::m_strString[ 3006 ], szPCName, szZoneName[cZone], cRemainTime ) ;
				pChat->AddMessage( strMessage, CRYLNetworkData::Instance()->m_dwMyChrID, 300 ) ;
			}
			break ;

			case PktCampMessage::MSGCMD_REPAIR_END :			// 길드 요새 수리 완료
			{
				sprintf( strMessage, CRYLStringTable::m_strString[ 3007 ], szZoneName[cZone] ) ;
				pChat->AddMessage( strMessage, CRYLNetworkData::Instance()->m_dwMyChrID, 300 ) ;
			}
			break ;
	
			case PktCampMessage::MSGCMD_UPGRADE_START :			// 길드 요새 업그레이드
			{
				sprintf( strMessage, CRYLStringTable::m_strString[ 3008 ], szPCName, szZoneName[cZone], cRemainTime ) ;
				pChat->AddMessage( strMessage, CRYLNetworkData::Instance()->m_dwMyChrID, 300 ) ;
			}
			break ;

			case PktCampMessage::MSGCMD_UPGRADE_END :			// 길드 요새 업그레이드 완료
			{
				sprintf( strMessage, CRYLStringTable::m_strString[ 3009 ], szZoneName[cZone] ) ;
				pChat->AddMessage( strMessage, CRYLNetworkData::Instance()->m_dwMyChrID, 300 ) ;
			}
			break ;

			case PktCampMessage::MSGCMD_SET_RIGHT :				// 길드 요새 관리 권한 변경
			{
				if ( 0 != Right.m_cCampControl )
				{
					switch ( Right.m_cCampControl )
					{
						case 1 :
							sprintf( strMessage, CRYLStringTable::m_strString[ 3010 ], szZoneName[cZone] ) ;
							break ;

						case 2 :
							sprintf( strMessage, CRYLStringTable::m_strString[ 3011 ], szZoneName[cZone] ) ;
							break ;

						case 3 :
							sprintf( strMessage, CRYLStringTable::m_strString[ 3012 ], szZoneName[cZone] ) ;
							break ;

						default :
							return FALSE;
					}
					pChat->AddMessage( strMessage, CRYLNetworkData::Instance()->m_dwMyChrID, 300 ) ;
				}
			}
			break ;

			case PktCampMessage::MSGCMD_SELF_DESTROY_START :	// 길드 요새 스스로 파괴 시작
			{
				sprintf( strMessage, CRYLStringTable::m_strString[ 3015 ], szPCName, szZoneName[cZone], cRemainTime ) ;
				pChat->AddMessage( strMessage, CRYLNetworkData::Instance()->m_dwMyChrID, 300 ) ;
			}
			break ;

			case PktCampMessage::MSGCMD_SELF_DESTROY_END :		// 길드 요새 스스로 파괴 완료
			{
				if ( bNotify )
				{
					sprintf( strMessage, CRYLStringTable::m_strString[ 3016 ], szGuildName, szZoneName[cZone] ) ;
					CRYLNoticeManager::Instance()->AddNotice( strMessage, NOTICECOLOR::WAR, NOTICETIME::NORMAL, TRUE ) ;
				}
				else
				{
					sprintf( strMessage, CRYLStringTable::m_strString[ 3017 ], CRYLStringTable::MakePrintGold(nValue2), nValue1 ) ;
					pChat->AddMessage( strMessage, CRYLNetworkData::Instance()->m_dwMyChrID, 300 ) ;
				}
			}
			break ;

			case PktCampMessage::MSGCMD_CHANGE_TYPE_START :		// 길드 요새 타입 변형 시작
			{
				char szTypeName[ MAX_PATH ] = "", szOldTypeName[ MAX_PATH ] = "";
				switch ( nValue1 )
				{
					case Siege::CAMP :			strcpy(szTypeName, CRYLStringTable::m_strString[ 3429 ]) ;		break ;
					case Siege::MINING_CAMP :	strcpy(szTypeName, CRYLStringTable::m_strString[ 3430 ]) ;		break ;
					case Siege::CAMP_SHOP :		strcpy(szTypeName, CRYLStringTable::m_strString[ 3431 ]) ;		break ;
				}
				switch ( nValue2 )
				{
					case Siege::CAMP :			strcpy(szOldTypeName, CRYLStringTable::m_strString[ 3429 ]);	break ;
					case Siege::MINING_CAMP :	strcpy(szOldTypeName, CRYLStringTable::m_strString[ 3430 ]);	break ;
					case Siege::CAMP_SHOP :		strcpy(szOldTypeName, CRYLStringTable::m_strString[ 3431 ]);	break ;
				}

				sprintf( strMessage, CRYLStringTable::m_strString[ 3432 ], 
					szPCName, szZoneName[cZone], szOldTypeName, szTypeName, Siege::CAMP_CHANGING_TIME ) ;
				pChat->AddMessage( strMessage, CRYLNetworkData::Instance()->m_dwMyChrID, 300 ) ;
			}
			break ;

			case PktCampMessage::MSGCMD_CHANGE_TYPE_END :		// 길드 요새 타입 변형 완료
			{
				char szTypeName[ MAX_PATH ] = "";
				switch ( nValue1 )
				{
					case Siege::CAMP :			strcpy(szTypeName, CRYLStringTable::m_strString[ 3429 ]) ;		break ;
					case Siege::MINING_CAMP :	strcpy(szTypeName, CRYLStringTable::m_strString[ 3430 ]) ;		break ;
					case Siege::CAMP_SHOP :		strcpy(szTypeName, CRYLStringTable::m_strString[ 3431 ]) ;		break ;
				}

				sprintf( strMessage, CRYLStringTable::m_strString[ 3433 ], szZoneName[cZone], szTypeName ) ;
				pChat->AddMessage( strMessage, CRYLNetworkData::Instance()->m_dwMyChrID, 300 ) ;
			}
			break ;

			case PktCampMessage::MSGCMD_ATTACKED :				// 길드 요새 공격 당함
			{
				sprintf( strMessage, CRYLStringTable::m_strString[ 3018 ], szZoneName[cZone] ) ;
				pChat->AddMessage( strMessage, CRYLNetworkData::Instance()->m_dwMyChrID, 300 ) ;

				CRYLCamp* pCamp = CRYLCampManager::Instance()->FindCamp( dwCampID ) ;
				if ( pCamp )
				{
					pCamp->SetAttacked( TRUE ) ;
				}
			}
			break ;

			case PktCampMessage::MSGCMD_DESTROYED :				// 길드 요새 파괴됨 (파괴 당함)
			{
				if ( bNotify )
				{
					// 국가 이름 얻기
					char szNation[ MAX_PATH ] = "";
					switch ( cNation )
					{
						case Creature::KARTERANT :			strcpy( szNation, CRYLStringTable::m_strString[ 2233 ] ) ;	break ;
						case Creature::MERKADIA :			strcpy( szNation, CRYLStringTable::m_strString[ 2264 ] ) ;	break ;
						case Creature::ALMIGHTY_PIRATE :	strcpy( szNation, CRYLStringTable::m_strString[ 2268 ] ) ;	break ;
					}

					sprintf( strMessage, CRYLStringTable::m_strString[ 3019 ], szNation, szPCName, szGuildName, szZoneName[cZone] ) ;
					CRYLNoticeManager::Instance()->AddNotice( strMessage, NOTICECOLOR::WAR, NOTICETIME::NORMAL, TRUE ) ;
				}
				else
				{
					sprintf( strMessage, CRYLStringTable::m_strString[ 3020 ], szZoneName[cZone], nValue1 ) ;
					pChat->AddMessage( strMessage, CRYLNetworkData::Instance()->m_dwMyChrID, 300 ) ;
				}
			}
			break ;

			case PktCampMessage::MSGCMD_DESTROY :				// 길드 요새 파괴함 (파괴 함)
			{
				// 국가 이름 얻기
				char szNation[ MAX_PATH ] = "";
				switch ( cNation )
				{
					case Creature::KARTERANT :			strcpy( szNation, CRYLStringTable::m_strString[ 2233 ] ) ;	break ;
					case Creature::MERKADIA :			strcpy( szNation, CRYLStringTable::m_strString[ 2264 ] ) ;	break ;
					case Creature::ALMIGHTY_PIRATE :	strcpy( szNation, CRYLStringTable::m_strString[ 2268 ] ) ;	break ;
				}

				sprintf( strMessage, CRYLStringTable::m_strString[ 3021 ], szZoneName[cZone], szNation, szGuildName, nValue1 ) ;
				pChat->AddMessage( strMessage, CRYLNetworkData::Instance()->m_dwMyChrID, 300 ) ;

				// 길드전 재정비 관련 메세지
				if(!cTimeChk)
				{
					sprintf( strMessage, CRYLStringTable::m_strString[ 3846 ], Guild::TIME_GUILD_POINT);
					pChat->AddMessage( strMessage, CRYLNetworkData::Instance()->m_dwMyChrID, 300 );
				}
				else
				{
					sprintf(strMessage, CRYLStringTable::m_strString[ 3847 ], cTimeChk);
					pChat->AddMessage(strMessage, CRYLNetworkData::Instance()->m_dwMyChrID, 300);
				}
			}
			break ;
			
			case PktCampMessage::MSGCMD_GUILD_TIME_COMPLETE:
			{
				// 길드 재정비 시간에 길드요새를 파괴 했을 경우
				sprintf(strMessage, CRYLStringTable::m_strString[ 3848 ]);
				pChat->AddMessage(strMessage, CRYLNetworkData::Instance()->m_dwMyChrID, 300);
			}
			break;

			case PktCampMessage::MSGCMD_MINING_END :			// 채굴기 채굴 완료
			{
				sprintf( strMessage, CRYLStringTable::m_strString[ 3434 ], szZoneName[cZone] ) ;
				pChat->AddMessage( strMessage, CRYLNetworkData::Instance()->m_dwMyChrID, 300 ) ;
			}
			break ;

			case PktCampMessage::MSGCMD_SHOP_GAIN_TAX :			// 상점 수익금 획득
			{
				sprintf( strMessage, CRYLStringTable::m_strString[ 3435 ], 
					szZoneName[cZone], CRYLStringTable::MakePrintGold( nValue1 ) ) ;
				pChat->AddMessage( strMessage, CRYLNetworkData::Instance()->m_dwMyChrID, 300 ) ;
			}
			break ;

			case PktCampMessage::MSGCMD_WEAPON_ENCAMP_START :	// 월드 웨폰 구축 시작
			{
				sprintf( strMessage, CRYLStringTable::m_strString[ 3436 ], szGuildName, szMasterName, szZoneName[cZone], cRemainTime ) ;
				CRYLNoticeManager::Instance()->AddNotice( strMessage, NOTICECOLOR::WAR, NOTICETIME::NORMAL, TRUE ) ;
			}
			break ;

			case PktCampMessage::MSGCMD_WEAPON_ENCAMPING :		// 월드 웨폰 구축 중
			{
				sprintf( strMessage, CRYLStringTable::m_strString[ 3437 ], szZoneName[cZone], cRemainTime ) ;
				CRYLNoticeManager::Instance()->AddNotice( strMessage, NOTICECOLOR::WAR, NOTICETIME::NORMAL, TRUE ) ;
			}
			break ;

			case PktCampMessage::MSGCMD_WEAPON_ENCAMP_END :		// 월드 웨폰 구축 완료
			{
				// 국가 이름 얻기
				char szNation[ MAX_PATH ] = "";
				switch ( cNation )
				{
					case Creature::KARTERANT :			strcpy( szNation, CRYLStringTable::m_strString[ 2233 ] ) ;	break ;
					case Creature::MERKADIA :			strcpy( szNation, CRYLStringTable::m_strString[ 2264 ] ) ;	break ;
					case Creature::ALMIGHTY_PIRATE :	strcpy( szNation, CRYLStringTable::m_strString[ 2268 ] ) ;	break ;
				}

				sprintf( strMessage, CRYLStringTable::m_strString[ 3438 ], szZoneName[cZone], szNation ) ;
				CRYLNoticeManager::Instance()->AddNotice( strMessage, NOTICECOLOR::WAR, NOTICETIME::NORMAL, TRUE ) ;
			}
			break ;

			case PktCampMessage::MSGCMD_WEAPON_CANCEL_START :	// 월드 웨폰 구축 취소
			{
				sprintf( strMessage, CRYLStringTable::m_strString[ 3439 ], szGuildName, szMasterName, szZoneName[cZone], cRemainTime ) ;
				CRYLNoticeManager::Instance()->AddNotice( strMessage, NOTICECOLOR::WAR, NOTICETIME::NORMAL, TRUE ) ;
			}
			break ;

			case PktCampMessage::MSGCMD_WEAPON_CANCEL_END :		// 월드 웨폰 구축 취소 완료
			{
				sprintf( strMessage, CRYLStringTable::m_strString[ 3440 ], szZoneName[cZone] ) ;
				CRYLNoticeManager::Instance()->AddNotice( strMessage, NOTICECOLOR::WAR, NOTICETIME::NORMAL, TRUE ) ;
			}
			break ;

			case PktCampMessage::MSGCMD_WEAPON_REPAIR_START :	// 월드 웨폰 수리
			{
				sprintf( strMessage, CRYLStringTable::m_strString[ 3441 ], szPCName, szZoneName[cZone], cRemainTime );
				pChat->AddMessage( strMessage, CRYLNetworkData::Instance()->m_dwMyChrID, 300 ) ;
			}
			break ;

			case PktCampMessage::MSGCMD_WEAPON_REPAIR_END :		// 월드 웨폰 수리 완료
			{
				sprintf( strMessage, CRYLStringTable::m_strString[ 3442 ], szZoneName[cZone] );
				pChat->AddMessage( strMessage, CRYLNetworkData::Instance()->m_dwMyChrID, 300 ) ;
			}
			break ;

			case PktCampMessage::MSGCMD_WEAPON_UPGRADE_START :	// 월드 웨폰 업그레이드
			{
				sprintf( strMessage, CRYLStringTable::m_strString[ 3443 ], szPCName, szZoneName[cZone], cRemainTime );
				pChat->AddMessage( strMessage, CRYLNetworkData::Instance()->m_dwMyChrID, 300 ) ;
			}
			break ;

			case PktCampMessage::MSGCMD_WEAPON_UPGRADE_END :	// 월드 웨폰 업그레이드 완료
			{
				sprintf( strMessage, CRYLStringTable::m_strString[ 3444 ], szZoneName[cZone] );
				pChat->AddMessage( strMessage, CRYLNetworkData::Instance()->m_dwMyChrID, 300 ) ;
			}
			break ;

			case PktCampMessage::MSGCMD_WEAPON_SET_RIGHT :		// 월드 웨폰 관리 권한 변경
			{
				if ( 0 != Right.m_cCampControl )
				{
					switch ( Right.m_cCampControl )
					{
						case 1 :
							sprintf( strMessage, CRYLStringTable::m_strString[ 3445 ], szZoneName[cZone] ) ;
							break ;

						case 2 :
							sprintf( strMessage, CRYLStringTable::m_strString[ 3446 ], szZoneName[cZone] ) ;
							break ;

						case 3 :
							sprintf( strMessage, CRYLStringTable::m_strString[ 3447 ], szZoneName[cZone] ) ;
							break ;

						default :
							return FALSE;
					}
					pChat->AddMessage( strMessage, CRYLNetworkData::Instance()->m_dwMyChrID, 300 ) ;
				}
			}
			break ;

			case PktCampMessage::MSGCMD_WEAPON_SELF_DESTROY_START :			// 월드 웨폰 스스로 파괴 시작
			{
				sprintf( strMessage, CRYLStringTable::m_strString[ 3450 ], szPCName, szZoneName[cZone], cRemainTime ) ;
				CRYLNoticeManager::Instance()->AddNotice( strMessage, NOTICECOLOR::WAR, NOTICETIME::NORMAL, TRUE ) ;
			}
			break ;

			case PktCampMessage::MSGCMD_WEAPON_SELF_DESTROY_END :			// 월드 웨폰 스스로 파괴 완료
			{
				// 국가 이름 얻기
				char szNation[ MAX_PATH ] = "";
				switch ( cNation )
				{
					case Creature::KARTERANT :			strcpy( szNation, CRYLStringTable::m_strString[ 2233 ] ) ;	break ;
					case Creature::MERKADIA :			strcpy( szNation, CRYLStringTable::m_strString[ 2264 ] ) ;	break ;
					case Creature::ALMIGHTY_PIRATE :	strcpy( szNation, CRYLStringTable::m_strString[ 2268 ] ) ;	break ;
				}

				sprintf( strMessage, CRYLStringTable::m_strString[ 3451 ], szGuildName, szZoneName[cZone], szNation ) ;
				CRYLNoticeManager::Instance()->AddNotice( strMessage, NOTICECOLOR::WAR, NOTICETIME::NORMAL, TRUE ) ;
			}
			break ;

			case PktCampMessage::MSGCMD_WEAPON_CHARGE_START :				// 월드 웨폰 무기 장전 시작
			{
				// 국가 이름 얻기
				char szNation[ MAX_PATH ] = "";
				switch ( cNation )
				{
					case Creature::KARTERANT :			strcpy( szNation, CRYLStringTable::m_strString[ 2233 ] ) ;	break ;
					case Creature::MERKADIA :			strcpy( szNation, CRYLStringTable::m_strString[ 2264 ] ) ;	break ;
					case Creature::ALMIGHTY_PIRATE :	strcpy( szNation, CRYLStringTable::m_strString[ 2268 ] ) ;	break ;
				}
				sprintf( strMessage, CRYLStringTable::m_strString[ 3452 ], szZoneName[cZone], szNation ) ;

				pChat->AddMessage( strMessage, CRYLNetworkData::Instance()->m_dwMyChrID, 300 ) ;
			}
			break ;

			case PktCampMessage::MSGCMD_WEAPON_CHARGE_END :					// 월드 웨폰 무기 장전 완료
			{
				// 국가 이름 얻기
				char szNation[ MAX_PATH ] = "";
				switch ( cNation )
				{
					case Creature::KARTERANT :			strcpy( szNation, CRYLStringTable::m_strString[ 2233 ] ) ;	break ;
					case Creature::MERKADIA :			strcpy( szNation, CRYLStringTable::m_strString[ 2264 ] ) ;	break ;
					case Creature::ALMIGHTY_PIRATE :	strcpy( szNation, CRYLStringTable::m_strString[ 2268 ] ) ;	break ;
				}
				sprintf( strMessage, CRYLStringTable::m_strString[ 3453 ], szZoneName[cZone], szNation ) ;

				CRYLNoticeManager::Instance()->AddNotice( strMessage, NOTICECOLOR::WAR, NOTICETIME::NORMAL, TRUE ) ;
			}
			break ;

			case PktCampMessage::MSGCMD_WEAPON_ATTACKED :					// 월드 웨폰 공격 당함
			{
				sprintf( strMessage, CRYLStringTable::m_strString[ 3454 ], szZoneName[cZone] ) ;
				pChat->AddMessage( strMessage, CRYLNetworkData::Instance()->m_dwMyChrID, 300 ) ;

				CRYLCamp* pCamp = CRYLCampManager::Instance()->FindCamp( dwCampID ) ;
				if ( pCamp )
				{
					pCamp->SetAttacked( TRUE ) ;
				}
			}
			break ;

			case PktCampMessage::MSGCMD_WEAPON_DESTROYED :					// 월드 웨폰 파괴됨
			{
				// 국가 이름 얻기
				char szNation[ MAX_PATH ] = "";
				char szNation2[ MAX_PATH ] = "";
				switch ( cNation )
				{
					case Creature::KARTERANT :			strcpy( szNation, CRYLStringTable::m_strString[ 2233 ] ) ;	break ;
					case Creature::MERKADIA :			strcpy( szNation, CRYLStringTable::m_strString[ 2264 ] ) ;	break ;
					case Creature::ALMIGHTY_PIRATE :	strcpy( szNation, CRYLStringTable::m_strString[ 2268 ] ) ;	break ;
				}
				switch ( nValue1 )
				{
					case Creature::KARTERANT :			strcpy( szNation2, CRYLStringTable::m_strString[ 2233 ] ) ;	break ;
					case Creature::MERKADIA :			strcpy( szNation2, CRYLStringTable::m_strString[ 2264 ] ) ;	break ;
					case Creature::ALMIGHTY_PIRATE :	strcpy( szNation2, CRYLStringTable::m_strString[ 2268 ] ) ;	break ;
				}


				sprintf( strMessage, CRYLStringTable::m_strString[ 3455 ], szNation, szPCName, szGuildName, szNation2 ) ;
				CRYLNoticeManager::Instance()->AddNotice( strMessage, NOTICECOLOR::WAR, NOTICETIME::NORMAL, TRUE ) ;
			}
			break ;

			case PktCampMessage::MSGCMD_WEAPON_FIRE_WARNING :				// 월드 웨폰 발사 경고
			{
				// 국가 이름 얻기
				char szNation[ MAX_PATH ] = "";
				switch ( cNation )
				{
					case Creature::KARTERANT :			strcpy( szNation, CRYLStringTable::m_strString[ 2233 ] ) ;	break ;
					case Creature::MERKADIA :			strcpy( szNation, CRYLStringTable::m_strString[ 2264 ] ) ;	break ;
					case Creature::ALMIGHTY_PIRATE :	strcpy( szNation, CRYLStringTable::m_strString[ 2268 ] ) ;	break ;
				}

				sprintf( strMessage, CRYLStringTable::m_strString[ 3456 ], szNation, szGuildName, nValue1, nValue2, cRemainTime ) ;
				CRYLNoticeManager::Instance()->AddNotice( strMessage, NOTICECOLOR::WAR, NOTICETIME::NORMAL, TRUE ) ;

				// 이펙트를 발동 시킨다.
				if ( ( 7 == cRemainTime && Creature::KARTERANT == cNation ) ||		// Karterant
					 ( 5 == cRemainTime && Creature::MERKADIA == cNation ) )		// Merkadia
				{
					CRYLGameScene*				pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
					CRYLWorldWeaponLaunchDlg*	pDlg   = ( CRYLWorldWeaponLaunchDlg* )pScene->GetWorldWeaponLaunchDlg(); 
					pDlg->SetCamp( dwCampID ) ;
					pDlg->Effect( nValue1 * 100, nValue2 * 100 );
				}
			}
			break ;

			case PktCampMessage::MSGCMD_TAKE_IN_MATERIAL :					// 길드 요새에 자재 넣기
			{

				CRYLCamp* pCamp = CRYLCampManager::Instance()->FindCamp( dwCampID ) ;
				RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( pCamp->GetCID() );

				if(Siege::KARTERANT_WEAPON != pTempCreature->GetObjectType() && 
				   Siege::MERKADIA_WEAPON != pTempCreature->GetObjectType() )
				{
					sprintf( strMessage, CRYLStringTable::m_strString[ 3072 ],
						szPCName, szZoneName[cZone], nValue1 ) ;
				}
				else
				{
					sprintf( strMessage, CRYLStringTable::m_strString[ 3079 ], szPCName, szZoneName[cZone], nValue1 ) ;
				}

				pChat->AddMessage( strMessage, CRYLNetworkData::Instance()->m_dwMyChrID, 300 ) ;
			}
			break ;

			case PktCampMessage::MSGCMD_TAKE_OUT_MATERIAL :					// 길드 요새에 자재 꺼내기
			{
				CRYLCamp* pCamp = CRYLCampManager::Instance()->FindCamp( dwCampID ) ;
				RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( pCamp->GetCID() );

				if(Siege::KARTERANT_WEAPON != pTempCreature->GetObjectType() && 
				   Siege::MERKADIA_WEAPON != pTempCreature->GetObjectType() )
				{
					sprintf( strMessage, CRYLStringTable::m_strString[ 3073 ],
						szPCName, szZoneName[cZone], nValue1 ) ;
				}
				else
				{
					sprintf( strMessage, CRYLStringTable::m_strString[ 3080 ], szPCName, szZoneName[cZone], nValue1 ) ;
				}

				pChat->AddMessage( strMessage, CRYLNetworkData::Instance()->m_dwMyChrID, 300 ) ;
			}
			break ;
		}
	}
	else
	{
		;// Error
	}

	return FALSE ;
}


GAME_EVENT_FUNC(ParseMiningCampMineralInfo)
{
	unsigned long		dwCampID ;		
	unsigned long		dwRemainTime ;			
	unsigned char		cMineralType ;		
	unsigned char		cNum ;
	unsigned char		cState ;
	CampMineralInfo		lpMineralList[ 255 ];


	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharMiningCampMineralInfo( lpPktBase,
																								  &dwCampID,
																								  &cState,
																								  &dwRemainTime,
																								  &cMineralType,
																								  &cNum,
																								  lpMineralList );
	if(0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		CRYLGameScene*		 pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
		CRYLMiningCampDlg*	 pDlg   = ( CRYLMiningCampDlg* )pScene->GetMiningCampDlg();
		pDlg->SetMineral( dwCampID, cState, dwRemainTime, cNum, lpMineralList );
		
	}
	else
	{
		;// Error
	}

	return FALSE ;
}


GAME_EVENT_FUNC(ParseTakeMaterial)
{
	unsigned long		dwCID ;
	unsigned long		dwCampID ;
	unsigned char		cSubCmd ;
	unsigned char		cNum ;
	Item::ItemPos		itemPos ;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharTakeMaterial( lpPktBase, &dwCID, &dwCampID, &cSubCmd, &cNum, &itemPos );

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		CRYLGameData* pGame = CRYLGameData::Instance();

		switch ( cSubCmd )
		{
			case PktTakeMaterial::SC_TAKE_IN :	// 넣기
			{
				CRYLCamp* pCamp = CRYLCampManager::Instance()->FindCamp( dwCampID ) ;
				if ( pCamp )
				{
					if ( pCamp->GetMaterial() )
					{
						pCamp->UpdateMaterial( pCamp->GetMaterial()->GetNowDurability() + cNum ) ;
					}
					else
					{
						pCamp->UpdateMaterial( cNum ) ;
					}
				}

				if ( CRYLNetworkData::Instance()->m_dwMyChrID == dwCID )
				{
					if ( pGame->m_lpPickItem )
					{
						if ( cNum < pGame->m_lpPickItem->GetNowDurability() )
						{
							pGame->m_lpPickItem->SetNowDurability( pGame->m_lpPickItem->GetNowDurability() - cNum ) ;
						}
						else if ( cNum == pGame->m_lpPickItem->GetNowDurability() )
						{
							delete pGame->m_lpPickItem;
							pGame->SetPickItem( NULL ) ;
						}
					}
				}
			}
			break;

			case PktTakeMaterial::SC_TAKE_OUT :	// 꺼내기
			{
				CRYLCamp* pCamp = CRYLCampManager::Instance()->FindCamp( dwCampID ) ;
				if ( pCamp )
				{
					pCamp->UpdateMaterial( 0 ) ;
				}

				if ( CRYLNetworkData::Instance()->m_dwMyChrID == dwCID )
				{
					if ( !pGame->m_lpPickItem )
					{
						pGame->m_lpPickItem = new CItemInstance ;
						pGame->m_lpPickItem->m_lpItemBase = Item::CItemFactory::GetInstance().CreateItem( Item::EtcItemID::SIEGE_MATERIAL_ID );
						pGame->m_lpPickItem->m_lpItemBase->MoveItem(itemPos);
						pGame->m_lpPickItem->SetNowDurability(cNum);

						pGame->m_lpPickItem->SetItemPos(TakeType::TS_TEMP);
						pGame->m_lpPickItem->m_dwItemUID = 0;
						pGame->m_lpPickItem->m_lCoolDownTimeCount = 0;
						pGame->m_lpPickItem->m_lpSprite = new CRYLSprite ;
						pGame->m_lpPickItem->m_lpSprite->Create( 0, 0, 
								pGame->m_lpPickItem->GetItemInfo().m_SpriteData.m_nSpriteMinX,
								pGame->m_lpPickItem->GetItemInfo().m_SpriteData.m_nSpriteMinY,
								pGame->m_lpPickItem->GetItemInfo().m_SpriteData.m_nSpriteMaxX,
								pGame->m_lpPickItem->GetItemInfo().m_SpriteData.m_nSpriteMaxY, NULL);
						pGame->m_lpPickItem->m_lpTooltip = new CGUITooltipItem;
						((CGUITooltipItem *)pGame->m_lpPickItem->m_lpTooltip)->SetItem(pGame->m_lpPickItem);
					}
				}
			}
			break;
		}
	}
	else
	{
		CRYLMessageBox* lpMessageBox = NULL;
		switch (CRYLNetworkData::Instance()->m_dwNetworkError)
		{
			case PktTakeMaterial::FAIL_ITEM_NUMBER:			// 갯수가 이상함
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 3074 ]);
				break;
			}

			case PktTakeMaterial::FAIL_NOT_EXIST_ITEMINFO:	// 존재하지 않는 아이템 정보 (자재 아이템 아이디 이상)
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 3075 ]);
				break;
			}

			case PktTakeMaterial::FAIL_OVER_STACK:			// 스택 갯수 초과
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 3076 ]);
				break;
			}

			case PktTakeMaterial::FAIL_NOT_EXIST_CAMP:		// 해당 요새가 존재 하지 않음
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 2612 ]);
				break;
			}

			case PktTakeMaterial::FAIL_CREATE_ITEM:			// 공성용 자재 생성 실패
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 3077 ]);
				break;
			}

			case PktTakeMaterial::FAIL_NOT_EXIST_ITEM:		// 공성용 자재가 없음
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 3078 ]);
				break;
			}

			case PktTakeMaterial::FAIL_CAMP_RIGHT:			// 권한 없음
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 1240 ]);
				break;
			}
		}
	}

	return FALSE ;
}

GAME_EVENT_FUNC(ParseCastleSiegeCount)
{
	unsigned long	dwCastleID ;			// 성 ID
	unsigned char	cSiegeCount ;			// 공성 횟수 (임시 세금 -> 누적 세금)
	unsigned char	cInvincibleCount ;		// 남은 무적 공성 횟수
	unsigned short	wTotalSiegeCount ;		// 누적 공성 횟수

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharCastleSiegeCount( lpPktBase, &dwCastleID, &cSiegeCount,
																							 &cInvincibleCount, &wTotalSiegeCount );

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		CRYLCastle* lpCastle = CRYLCastleManager::Instance()->FindCastle( dwCastleID ) ;
		if ( lpCastle )
		{
			lpCastle->SetInvincibleCount( cInvincibleCount ) ;
			lpCastle->SetTotalSiegeCount( wTotalSiegeCount ) ;
		}
	}

	return FALSE ;
}

GAME_EVENT_FUNC(ParseCastleTaxMove)
{
	unsigned long	dwCastleID ;			// 성 ID
	unsigned long	dwMoveTempMoney[ Castle::MAX_TAX_TYPE ] ;	// 임시 세금 -> 누적 세금으로 이동한 양
	unsigned long	dwResultTaxMoney[ Castle::MAX_TAX_TYPE ] ;	// 최종 누적 세금

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharCastleTaxMove( lpPktBase, &dwCastleID,
																						  dwMoveTempMoney, dwResultTaxMoney ) ;

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		CRYLCastle* lpCastle = CRYLCastleManager::Instance()->FindCastle( dwCastleID ) ;
		if ( lpCastle )
		{
			for (int i=0; i<Castle::MAX_TAX_TYPE; ++i)
			{
				lpCastle->SetTaxMoney( i, dwResultTaxMoney[ i ] ) ;
			}
		}
	}

	return FALSE ;
}

GAME_EVENT_FUNC(ParseTakeCastleJewel)
{
	unsigned long		dwCID ;
	unsigned long		dwCastleID ;
	unsigned char		cSubCmd ;
	unsigned short		wItemID ;
	unsigned char		cIndex ;
	unsigned char		cNum ;
	Item::ItemPos		itemPos ;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharTakeCastleJewel( lpPktBase, &dwCID, &dwCastleID, &cSubCmd,
																							&wItemID, &cIndex, &cNum, &itemPos );

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		CRYLGameData* pGame = CRYLGameData::Instance();

		switch ( cSubCmd )
		{
			case PktTakeCastleJewel::SC_TAKE_IN :	// 넣기
			{
				CRYLCastle* pCastle = CRYLCastleManager::Instance()->FindCastle( dwCastleID ) ;
				if ( pCastle )
				{
					pCastle->AddUpgradeItem( wItemID, cIndex, cNum ) ;
				}

				if ( CRYLNetworkData::Instance()->m_dwMyChrID == dwCID )
				{
					if ( pGame->m_lpPickItem )
					{
						if ( cNum < pGame->m_lpPickItem->GetNowDurability() )
						{
							pGame->m_lpPickItem->SetNowDurability( pGame->m_lpPickItem->GetNowDurability() - cNum ) ;
						}
						else
						{
							delete pGame->m_lpPickItem ;
							pGame->SetPickItem( NULL ) ;
						}
					}
				}
			}
			break;

			case PktTakeCastleJewel::SC_TAKE_OUT :	// 꺼내기
			{
				CRYLCastle* pCastle = CRYLCastleManager::Instance()->FindCastle( dwCastleID ) ;
				if ( pCastle )
				{
					pCastle->DeleteUpgradeItem( wItemID, cIndex, cNum ) ;
				}

				if ( CRYLNetworkData::Instance()->m_dwMyChrID == dwCID )
				{
					if ( !pGame->m_lpPickItem )
					{
						pGame->m_lpPickItem = new CItemInstance ;
						pGame->m_lpPickItem->m_lpItemBase = Item::CItemFactory::GetInstance().CreateItem( wItemID ) ;
						pGame->m_lpPickItem->m_lpItemBase->MoveItem( itemPos ) ;
						pGame->m_lpPickItem->SetNowDurability( cNum ) ;

						pGame->m_lpPickItem->SetItemPos( TakeType::TS_TEMP ) ;
						pGame->m_lpPickItem->m_dwItemUID = 0 ;
					pGame->m_lpPickItem->m_lCoolDownTimeCount = 0 ;
						pGame->m_lpPickItem->m_lpSprite = new CRYLSprite ;
						pGame->m_lpPickItem->m_lpSprite->Create( 0, 0, 
							pGame->m_lpPickItem->GetItemInfo().m_SpriteData.m_nSpriteMinX,
							pGame->m_lpPickItem->GetItemInfo().m_SpriteData.m_nSpriteMinY,
							pGame->m_lpPickItem->GetItemInfo().m_SpriteData.m_nSpriteMaxX,
							pGame->m_lpPickItem->GetItemInfo().m_SpriteData.m_nSpriteMaxY, NULL ) ;
						pGame->m_lpPickItem->m_lpTooltip = new CGUITooltipItem ;
						((CGUITooltipItem *)pGame->m_lpPickItem->m_lpTooltip)->SetItem( pGame->m_lpPickItem ) ;
					}
				}
			}
			break;
		}
	}
	else
	{
		CRYLMessageBox* lpMessageBox = NULL;
		switch (CRYLNetworkData::Instance()->m_dwNetworkError)
		{
			case PktTakeCastleJewel::FAIL_NOT_EXIST_CASTLE:		// 존재 하지 않는 성
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 2583 ]);
				break;
			}

			case PktTakeCastleJewel::FAIL_NOT_EXIST_EMBLEM:		// 존재 하지 않는 성 상징물
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 3849 ]);
				break;
			}

			case PktTakeCastleJewel::FAIL_WRONG_JEWEL:		// 적절하지 못한 보석
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 3850 ]);
				break;
			}

			case PktTakeCastleJewel::FAIL_DIFFERENT_JEWEL:		// 다른 종류의 보석
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 3851 ]);
				break;
			}

			case PktTakeCastleJewel::FAIL_NOT_EXIST_ITEM_PROTOTYPE:		// 아이템 프로토 타입이 존재하지 않음
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 3852 ]);
				break;
			}

			case PktTakeCastleJewel::FAIL_OVER_STACK:		// 스택 오버 플러우
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 3853 ]);
				break;
			}

			case PktTakeCastleJewel::FAIL_NOT_EXIST_ITEM:		// 해당 위치에 보석 아이템이 존재하지 않음
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 3854 ]);
				break;
			}

			case PktTakeCastleJewel::FAIL_ITEM_NUMBER:		// 아이템 갯수가 이상함
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 3855 ]);
				break;
			}

			case PktTakeCastleJewel::FAIL_CREATE_ITEM:		// 보석 아이템 생성 실패
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 3856 ]);
				break;
			}
		}
	}

	return FALSE ;
}

GAME_EVENT_FUNC(ParseCastleMineralInfo)
{
	unsigned long		dwCastleID ;
	unsigned char		cNum ;
	CastleMineral		lpMineralList[ 255 ];


	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharCastleMineralInfo( lpPktBase, &dwCastleID, &cNum, lpMineralList );
	if(0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		CRYLGameScene*			pScene	= ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
		CRYLCastleMineralDlg*	pDlg	= ( CRYLCastleMineralDlg* )pScene->GetCastleMineralDlg() ;
		pDlg->SetMineral( dwCastleID, cNum, lpMineralList ) ;
	}
	else
	{
		;// Error
	}

	return FALSE ;
}

GAME_EVENT_FUNC(ParseWarOnOff)
{
	unsigned long dwCID = 0;
	unsigned char cFlag	= 0;
	unsigned char cType	= 0;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharWarOnOff( lpPktBase, &dwCID, &cFlag, &cType);

	if(!CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		if ( cType==GameTime::REALM )
		{
			RYLCreature* pSelfCreature	= RYLCreatureManager::Instance()->GetSelfCreature() ;
			if ( pSelfCreature )
			{
				// 국가 전쟁 플래그.
				pSelfCreature->SetRealmWarFlag( cFlag ) ;
			}

			switch( cFlag )
			{
				case Creature::WAR_INSTANCE :
				{
					CRYLGameScene*		pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );
					CRYLChattingDlg*	pChat  = CRYLChattingDlg::Instance() ;
					pChat->AddMessage(CRYLStringTable::m_strString[3823], CRYLNetworkData::Instance()->m_dwMyChrID, 129);

					break ;
				}				

				case Creature::WAR_ON :
				{
					break ;
				}
				
				case Creature::WAR_OFF :
				{
					break ;
				}
			}
		}
		else if ( cType == GameTime::GUILD )
		{
			RYLCreature* pSelfCreature	= RYLCreatureManager::Instance()->GetSelfCreature() ;
			if ( pSelfCreature )
			{
				// 길드 전쟁 플래그.
				pSelfCreature->SetGuildWarFlag( cFlag ) ;
			}

			switch ( cFlag )
			{
				case Creature::WAR_INSTANCE :
				{
					CRYLGameScene*		pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );
					CRYLChattingDlg*	pChat  = CRYLChattingDlg::Instance() ;
					pChat->AddMessage( CRYLStringTable::m_strString[3913], CRYLNetworkData::Instance()->m_dwMyChrID, 128 ) ;
					
					break ;
				}

				case Creature::WAR_ON :
				{
					CRYLGameScene*		pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );
					CRYLChattingDlg*	pChat  = CRYLChattingDlg::Instance() ;
					pChat->AddMessage( CRYLStringTable::m_strString[3914], CRYLNetworkData::Instance()->m_dwMyChrID, 128 ) ;

					CRYLGuildFrameDlg*		pDlg	= ( CRYLGuildFrameDlg* )pScene->GetGuildFrameDlg() ;
					CRYLGuildMemberPanel*	pMemberPanel = ( CRYLGuildMemberPanel* )pDlg->GetGuildMemberPanel() ;
					if ( pMemberPanel->GetVisible() )
					{
						pMemberPanel->UpdateGuildWarJoinFlag( dwCID, cFlag ) ;
					}

					break ;
				}

				case Creature::WAR_OFF :
				{
					CRYLGameScene*		pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );
					CRYLChattingDlg*	pChat  = CRYLChattingDlg::Instance() ;
					pChat->AddMessage( CRYLStringTable::m_strString[3915], CRYLNetworkData::Instance()->m_dwMyChrID, 128 ) ;

					CRYLGuildFrameDlg*		pDlg	= ( CRYLGuildFrameDlg* )pScene->GetGuildFrameDlg() ;
					CRYLGuildMemberPanel*	pMemberPanel = ( CRYLGuildMemberPanel* )pDlg->GetGuildMemberPanel() ;
					if ( pMemberPanel->GetVisible() )
					{
						pMemberPanel->UpdateGuildWarJoinFlag( dwCID, cFlag ) ;
					}

					break ;
				}
			}

			// WAR_TODO : 무방비 패널티 적용 루틴 필요!!					
			if(cFlag!=Creature::WAR_OFF)
			{
				if ( CRYLNetworkData::Instance()->m_dwPresentZone == SERVER_ID::CAPITAL )
				{
					if(CRYLGameTimeManager::Instance()->IsGuildWarTime())
					{						
						CRYLGameScene*		pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );
						CRYLGuildFrameDlg*	pDlg	= ( CRYLGuildFrameDlg* )pScene->GetGuildFrameDlg() ;
						CRYLGuildInfoPanel* pPanel	= ( CRYLGuildInfoPanel* )pDlg->GetGuildInfoPanel() ;												
						LPGUILDLARGEINFOEX	lpGuild = pPanel->FindGuild( CRYLGameData::Instance()->m_csStatus.m_Info.GID ) ;
						CRYLGuildWarPenaltyDlg* pGuildWarPenaltyDlg = (CRYLGuildWarPenaltyDlg*)pScene->GetGuildWarPenaltyDlg();

						if(lpGuild)
						{								
							pGuildWarPenaltyDlg->SetPenalty(lpGuild->m_szName);
							pGuildWarPenaltyDlg->SetVisible(TRUE);
							CRYLGameData::SetAutoRun(false);
						}
					}					
				}
				
			}			
		}

		return true;
	}
	else
	{
		CRYLMessageBox* lpMessageBox = NULL;
		
		switch (CRYLNetworkData::Instance()->m_dwNetworkError)
		{
			case PktWarOnOff::E_WAR_OFF:
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 3857 ]);
				break;
			}

			case PktWarOnOff::E_WAR_ON:
			case PktWarOnOff::E_WAR_INSTANCE:
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 3858 ]);
				break;
			}

			case PktWarOnOff::E_INVALID_TYPE:
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 3859 ]);
				break;
			}
		}		
	}

	return true;
}

GAME_EVENT_FUNC(ParseRealmPoint)
{
	// 공헌 훈장 포인트 세팅 //

	unsigned long	dwCID			= 0;
	unsigned char	cRealmPoint		= 0;
	unsigned char	cType			= 0;
	unsigned long	dwColor			= 0;
	char			szInfo[0xff]	= {0,};

	unsigned char cBonus[2][6] =
	{
		0, 5, 5, 10, 10, 15,
		0, 0, 5, 5, 10, 10
	};

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharRealmPoint(lpPktBase, &dwCID, &cRealmPoint, &cType);

	if(!CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		RYLCreature* pSelfCreature	= RYLCreatureManager::Instance()->GetSelfCreature();

		if(pSelfCreature)
		{
			// edith 2009.06.20 공훈보상 메시지 삭제
/*
			if(cType==PktRealmPoint::REALM_JOIN)
			{
				if(cRealmPoint==5)
				{
					sprintf(szInfo, CRYLStringTable::m_strString[3587], cBonus[0][cRealmPoint], cBonus[1][cRealmPoint]);
				}
				else
				{
					sprintf(szInfo, CRYLStringTable::m_strString[3588], cBonus[0][cRealmPoint], cBonus[1][cRealmPoint]);
				}
				dwColor = 128;
			}
			else if(cType==PktRealmPoint::REALM_OUT)
			{
				dwColor = 129;
				sprintf(szInfo, CRYLStringTable::m_strString[3589], cBonus[0][cRealmPoint], cBonus[1][cRealmPoint]);
			}

			CRYLGameScene*		pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );
			CRYLChattingDlg*	pChat  = CRYLChattingDlg::Instance() ;
			pChat->AddMessage(szInfo, CRYLNetworkData::Instance()->m_dwMyChrID, dwColor);
*/
			pSelfCreature->SetRealmPoint(cRealmPoint);
		}
	}

	return true;
}

GAME_EVENT_FUNC(ParseStatueDisplayInfo)
{
	unsigned long		dwStatueNum	= 0;
	unsigned short		wSubCmd		= 0;
	StatueDisplayInfo	statueInfo[PktStatueCmd::MAX_STATUE_NUM];

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharRealmStatueDisplayInfo(lpPktBase, &wSubCmd, &dwStatueNum, statueInfo);
	if (!CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		CRYLGameScene*		    pScene          = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );	    
		CRYLLargeMapFrameDlg*	pLargeMapDlg	= ( CRYLLargeMapFrameDlg* )pScene->GetLargeMapDlg();

		for(unsigned long dwIndex = 0; dwIndex < dwStatueNum; dwIndex++)
		{						
			pLargeMapDlg->Update(statueInfo[dwIndex].m_dwIndex, &statueInfo[dwIndex]);

			if (statueInfo[dwIndex].m_bAttacked &&
				((statueInfo[dwIndex].m_cNation == CRYLGameData::Instance()->m_cAccountNation) || (Creature::STATELESS == statueInfo[dwIndex].m_cNation)))
			{
				unsigned long dwTick = timeGetTime();
				unsigned char cIndex = (statueInfo[dwIndex].m_cNation==Creature::KARTERANT) ? 0 : 1;

				if(dwTick-pLargeMapDlg->GetStatueTextTick(cIndex)>CRYLLargeMapFrameDlg::ATTACK_TEXT_TIME)
				{
					pLargeMapDlg->SetStatueTextTick(cIndex, dwTick);

					char szInfo[0xff] = {0, };

					switch(statueInfo[dwIndex].m_dwKID)
					{
						// 1레벨
						case MonsterInfo::STATUE_NEUTRALITY1:		
						case MonsterInfo::POWER_STATUE_NEUTRALITY1:		
						case MonsterInfo::INT_STATUE_NEUTRALITY1:		
						case MonsterInfo::EXP_STATUE_NEUTRALITY1:		
						case MonsterInfo::WEALTH_STATUE_NEUTRALITY1:		return true;
						// 2레벨
						case MonsterInfo::STATUE_NEUTRALITY2:		
						case MonsterInfo::POWER_STATUE_NEUTRALITY2:		
						case MonsterInfo::INT_STATUE_NEUTRALITY2:		
						case MonsterInfo::EXP_STATUE_NEUTRALITY2:		
						case MonsterInfo::WEALTH_STATUE_NEUTRALITY2:		return true;

						case MonsterInfo::STATUE_HUMAN_LOADING1:	
						case MonsterInfo::POWER_STATUE_HUMAN_LOADING1:	
						case MonsterInfo::INT_STATUE_HUMAN_LOADING1:
						case MonsterInfo::EXP_STATUE_HUMAN_LOADING1:
						case MonsterInfo::WEALTH_STATUE_HUMAN_LOADING1:	sprintf(szInfo, CRYLStringTable::m_strString[3591], CRYLStringTable::m_strString[2233]); break;

						case MonsterInfo::STATUE_HUMAN_LOADING2:	
						case MonsterInfo::POWER_STATUE_HUMAN_LOADING2:	
						case MonsterInfo::INT_STATUE_HUMAN_LOADING2:
						case MonsterInfo::EXP_STATUE_HUMAN_LOADING2:
						case MonsterInfo::WEALTH_STATUE_HUMAN_LOADING2:	sprintf(szInfo, CRYLStringTable::m_strString[3591], CRYLStringTable::m_strString[2233]); break;

						case MonsterInfo::STATUE_HUMAN_COMPLETE1:
						case MonsterInfo::POWER_STATUE_HUMAN_COMPLETE1:
						case MonsterInfo::INT_STATUE_HUMAN_COMPLETE1:
						case MonsterInfo::EXP_STATUE_HUMAN_COMPLETE1:
						case MonsterInfo::WEALTH_STATUE_HUMAN_COMPLETE1:	sprintf(szInfo, CRYLStringTable::m_strString[3592], CRYLStringTable::m_strString[2233]); break;

						case MonsterInfo::STATUE_HUMAN_COMPLETE2:
						case MonsterInfo::POWER_STATUE_HUMAN_COMPLETE2:
						case MonsterInfo::INT_STATUE_HUMAN_COMPLETE2:
						case MonsterInfo::EXP_STATUE_HUMAN_COMPLETE2:
						case MonsterInfo::WEALTH_STATUE_HUMAN_COMPLETE2:	sprintf(szInfo, CRYLStringTable::m_strString[3592], CRYLStringTable::m_strString[2233]); break;

						case MonsterInfo::STATUE_AKHAN_LOADING1:
						case MonsterInfo::POWER_STATUE_AKHAN_LOADING1:
						case MonsterInfo::INT_STATUE_AKHAN_LOADING1:
						case MonsterInfo::EXP_STATUE_AKHAN_LOADING1:
						case MonsterInfo::WEALTH_STATUE_AKHAN_LOADING1:	sprintf(szInfo, CRYLStringTable::m_strString[3591], CRYLStringTable::m_strString[2264]); break;

						case MonsterInfo::STATUE_AKHAN_LOADING2:
						case MonsterInfo::POWER_STATUE_AKHAN_LOADING2:
						case MonsterInfo::INT_STATUE_AKHAN_LOADING2:
						case MonsterInfo::EXP_STATUE_AKHAN_LOADING2:
						case MonsterInfo::WEALTH_STATUE_AKHAN_LOADING2:	sprintf(szInfo, CRYLStringTable::m_strString[3591], CRYLStringTable::m_strString[2264]); break;

						case MonsterInfo::STATUE_AKHAN_COMPLETE1:
						case MonsterInfo::POWER_STATUE_AKHAN_COMPLETE1:
						case MonsterInfo::INT_STATUE_AKHAN_COMPLETE1:					
						case MonsterInfo::EXP_STATUE_AKHAN_COMPLETE1:
						case MonsterInfo::WEALTH_STATUE_AKHAN_COMPLETE1:	sprintf(szInfo, CRYLStringTable::m_strString[3592], CRYLStringTable::m_strString[2264]); break;

						case MonsterInfo::STATUE_AKHAN_COMPLETE2:
						case MonsterInfo::POWER_STATUE_AKHAN_COMPLETE2:
						case MonsterInfo::INT_STATUE_AKHAN_COMPLETE2:					
						case MonsterInfo::EXP_STATUE_AKHAN_COMPLETE2:
						case MonsterInfo::WEALTH_STATUE_AKHAN_COMPLETE2:	sprintf(szInfo, CRYLStringTable::m_strString[3592], CRYLStringTable::m_strString[2264]); break;

						case MonsterInfo::LIFE_EXTRACT_NEUTRALITY1:		return true;
						case MonsterInfo::LIFE_EXTRACT_HUMAN_LOADING1:	sprintf(szInfo, CRYLStringTable::m_strString[4232], CRYLStringTable::m_strString[2233]); break;
						case MonsterInfo::LIFE_EXTRACT_HUMAN_COMPLETE1:	sprintf(szInfo, CRYLStringTable::m_strString[4233], CRYLStringTable::m_strString[2233]); break;
						case MonsterInfo::LIFE_EXTRACT_AKHAN_LOADING1:	sprintf(szInfo, CRYLStringTable::m_strString[4232], CRYLStringTable::m_strString[2264]); break;
						case MonsterInfo::LIFE_EXTRACT_AKHAN_COMPLETE1:	sprintf(szInfo, CRYLStringTable::m_strString[4233], CRYLStringTable::m_strString[2264]); break;

						case MonsterInfo::LIFE_EXTRACT_NEUTRALITY2:		return true;
						case MonsterInfo::LIFE_EXTRACT_HUMAN_LOADING2:	sprintf(szInfo, CRYLStringTable::m_strString[4232], CRYLStringTable::m_strString[2233]); break;
						case MonsterInfo::LIFE_EXTRACT_HUMAN_COMPLETE2:	sprintf(szInfo, CRYLStringTable::m_strString[4233], CRYLStringTable::m_strString[2233]); break;
						case MonsterInfo::LIFE_EXTRACT_AKHAN_LOADING2:	sprintf(szInfo, CRYLStringTable::m_strString[4232], CRYLStringTable::m_strString[2264]); break;
						case MonsterInfo::LIFE_EXTRACT_AKHAN_COMPLETE2:	sprintf(szInfo, CRYLStringTable::m_strString[4233], CRYLStringTable::m_strString[2264]); break;

						default:										return true;	
					}

					CRYLChattingDlg::Instance()->AddMessage(szInfo, CRYLNetworkData::Instance()->m_dwMyChrID, 128);					
				}
			}	
			//HZ WASNT HERE:
			
		}		
	}

	return true;
}

