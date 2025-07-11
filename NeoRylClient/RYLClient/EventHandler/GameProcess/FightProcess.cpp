//--------------------------------------------------------------------------------------------------------------------------
// File Name: FightProcess.cpp
//
// Programmer: Zergra( Park Jongtae ) in GamaSoft corp.
//
// File Desciption: Fight Process
//
// Date: 2004. 4. 26. ( Mon )
//--------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------------
// Headers
//--------------------------------------------------------------------------------------------------------------------------

#include "FightProcess.h"
#include "../GameHandler.h"

#include "RYLClientMain.h"
#include "RYLRaceBase.h"
#include "RYLRaceBaseDefine.h"
#include "RYLMessageBox.h"

#include "../../HumanStoneStatueRespawn.h"
#include "../../SkillSystem.h"
#include "../../datatable.h"

#include <Network/Packet/PacketStruct/ServerInfo.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/ClientSocket/Parse/ParseFight.h>
#include <Network/ClientSocket/Parse/ParseCommunity.h>
#include <Network/ClientSocket/ClientConstants.h>
#include <Network/ClientSocket/Send/SendAttack.h>

#include <Creature/EnemyCheck.h>
#include <Creature/Monster/MonsterMgr.h>
#include <Creature/Character/CharacterClass.h>

#include <Skill/SkillMgr.h>

//------------------------------------------------------------------------------

#include "SoundMgr.h"

#include "RYLStringTable.h"
#include "RYLNetWorkData.h"
#include "RYLGameData.h"
#include "RYLCommunityData.h"
#include "RYLChattingDlg.h"

#include "RYLSceneManager.h"
#include "RYLGameScene.h"
#include "RYLRankingDlg.h"
#include "RYLBattleGroundDlg.h"
#include "RYLLoadingScene.h"

#include "BroadcastProcess.h"

#include "RYLGameScene.h"
#include "RYLSceneManager.h"
#include "RYLLargeMapFrameDlg.h"
#include "RYLTabButton.h"
#include "RYLBattleWaitDlg.h"
#include "RYLCampChangeDlg.h"
#include "RYLCampUpgradeDlg.h"
#include "RYLWorldWeaponLoadDlg.h"


#include "RYLCreatureManager.h"
#include "RYLCharacterDataManager.h"

#include "RYLMessageBoxManager.h"
#include "GMMemory.h"

#include "RYLJoystickConfigureDlg.h"
#include "RYLJoystickConfigurePanel.h"

GAME_EVENT_FUNC(ParseCharAttack);
GAME_EVENT_FUNC(ParseCharAttacked);
GAME_EVENT_FUNC(ParseCharAttackInfo);
GAME_EVENT_FUNC(ParseCharEquipDurability);
GAME_EVENT_FUNC(ParseCharRespawn);
GAME_EVENT_FUNC(ParseCharBattleGroundRespawn);
GAME_EVENT_FUNC(ParseCharRespawnWaitQueue);
GAME_EVENT_FUNC(ParseCharDuelCmd);
GAME_EVENT_FUNC(ParseCharTeamBattleInfo);
GAME_EVENT_FUNC(ParseCharPeaceMode);
GAME_EVENT_FUNC(ParseCharRankingInfo);
GAME_EVENT_FUNC(ParseCharAward);
GAME_EVENT_FUNC(ParseCharStatueInfo);
GAME_EVENT_FUNC(ParseCharCameraScript);
GAME_EVENT_FUNC(ParseCharRespawnInfo);
GAME_EVENT_FUNC(ParseCharRespawnAreaInfo);

struct CoMPareRanking
{
	int operator() ( const RankingNode* node1, const RankingNode* node2 ) 
	{ 
		return ( node1->m_dwPoint > node2->m_dwPoint ); 
	}
};

//--------------------------------------------------------------------------------------------------------------------------
// Event Function
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
// GameProcess::RegisterFightHandler()
bool GameProcess::RegisterFightHandler(CGameEventHandler& GameEventHandler)
{
	unsigned long dwErrorCount = 0;
	unsigned long dwIgnoreNonPlayPacket = !(BIT(CLIENT_GAME) | BIT(CLIENT_GAMELOADING));

	dwErrorCount += GameEventHandler.AddHandler(CmdCharAttack, ParseCharAttack, dwIgnoreNonPlayPacket) ? 0 : 1;
	dwErrorCount += GameEventHandler.AddHandler(CmdCharAttacked, ParseCharAttacked, dwIgnoreNonPlayPacket) ? 0 : 1;
	dwErrorCount += GameEventHandler.AddHandler(CmdCharAttackInfo, ParseCharAttackInfo, dwIgnoreNonPlayPacket) ? 0 : 1;
	dwErrorCount += GameEventHandler.AddHandler(CmdCharEquipDurability, ParseCharEquipDurability) ? 0 : 1;
	dwErrorCount += GameEventHandler.AddHandler(CmdCharRespawn, ParseCharRespawn, dwIgnoreNonPlayPacket) ? 0 : 1;
	dwErrorCount += GameEventHandler.AddHandler(CmdCharBattleGroundRespawn, ParseCharBattleGroundRespawn) ? 0 : 1;
	dwErrorCount += GameEventHandler.AddHandler(CmdCharRespawnWaitQueue, ParseCharRespawnWaitQueue) ? 0 : 1;
	dwErrorCount += GameEventHandler.AddHandler(CmdCharDuelCmd, ParseCharDuelCmd) ? 0 : 1;
	dwErrorCount += GameEventHandler.AddHandler(CmdCharTeamBattleInfo, ParseCharTeamBattleInfo) ? 0 : 1;
	dwErrorCount += GameEventHandler.AddHandler(CmdCharPeaceMode, ParseCharPeaceMode) ? 0 : 1;
	dwErrorCount += GameEventHandler.AddHandler(CmdRankingInfo, ParseCharRankingInfo) ? 0 : 1;
	dwErrorCount += GameEventHandler.AddHandler(CmdCharAward, ParseCharAward, dwIgnoreNonPlayPacket) ? 0 : 1;
	dwErrorCount += GameEventHandler.AddHandler(CmdStatueInfo, ParseCharStatueInfo, dwIgnoreNonPlayPacket) ? 0 : 1;
	dwErrorCount += GameEventHandler.AddHandler(CmdCameraScript, ParseCharCameraScript) ? 0 : 1;
	dwErrorCount += GameEventHandler.AddHandler(CmdCharRespawnInfo, ParseCharRespawnInfo) ? 0 : 1 ;
	dwErrorCount += GameEventHandler.AddHandler(CmdCharRespawnAreaInfo, ParseCharRespawnAreaInfo ) ? 0 : 1 ;

	return 0 == dwErrorCount;
}

//--------------------------------------------------------------------------------------------------------------------------
// GAME_EVENT_FUNC(ParseCharAttack)
GAME_EVENT_FUNC(ParseCharAttack)
{
	unsigned long   ulCharID = 0;
	unsigned char   ucDefenserNum = 0;
	unsigned short  usHP = 0;
	unsigned short  usMP = 0;
	unsigned short	usMPHeal = 0;
	DefenserNode*   pDefNode = NULL;
	AtType			pAtType;
	char			cJudge = 0;
	CRYLGameData*	pGame = CRYLGameData::Instance() ;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharAttack( lpPktBase, 
		&ulCharID, 
		&pAtType, 
		&usHP, 
		&usMP, 
		&usMPHeal,
		&cJudge, 
		&ucDefenserNum, 
		&pDefNode);

	CRYLChattingDlg* pChat = CRYLChattingDlg::Instance() ;


	if ( CRYLNetworkData::Instance()->m_dwNetworkError == 0 )
	{
		RYLCreature*    pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
		if ( !pSelfCreature )
			return false;

		CharDataNode*   pSelfData = RYLCharacterDataManager::Instance()->GetCharData( pSelfCreature->GetCharID() );
		if ( !pSelfData )
			return false;

		RYLCreature* pAttackCreature = RYLCreatureManager::Instance()->GetCreature( ulCharID );
		if ( !pAttackCreature )
			return true;

		CharDataNode* pAttackData = RYLCharacterDataManager::Instance()->GetCharData( ulCharID );
		if ( !pAttackData )
			return true;

		// GM 캐릭터가 공격했는지 체크.
		if (pAttackCreature->GetGMModelFlag())
		{
			CEffScript* pInputEffect;
			pInputEffect = new CEffScript;

			pInputEffect->GetScriptBinData("adminattack.esf");

			pInputEffect->SetPlayer(pAttackCreature, 0);

			pInputEffect->SetChr(pAttackCreature->GetCharModel(), pAttackCreature->GetCharModel());

			float fChrDir = pAttackCreature->GetCharModel()->GetDirection();
			fChrDir = -fChrDir;
			fChrDir -= FLOAT_PHI / 2.0f;

			vector3 vecChrToward;
			vecChrToward.x = cosf(fChrDir);
			vecChrToward.z = sinf(fChrDir);	
			vecChrToward.y = 0.0f;
			vecChrToward.Normalize();
			pInputEffect->SetChrFwd(-vecChrToward.x, -vecChrToward.y, -vecChrToward.z);

			pInputEffect->SetStartPos(pAttackCreature->GetPosition()->x, pAttackCreature->GetPosition()->y, pAttackCreature->GetPosition()->z);
			pInputEffect->SetEndPos(pAttackCreature->GetPosition()->x, pAttackCreature->GetPosition()->y, pAttackCreature->GetPosition()->z);

			pInputEffect->SetMine( true );
			CSceneManager::m_EffectManager.AddEffScript( pInputEffect );
		}

        /*
            FIXED : Removed UDP Packets

		list<unsigned long>     SendCharacterList;
		unsigned long   ulDefID[ 10 ];
		ZeroMemory( ulDefID, sizeof( ulDefID ) );

		for ( int i = 0 ; i < ucDefenserNum ; ++i )
		{
			ulDefID[ i ] = pDefNode[ i ].m_dwCharID;
		}
		RYLCreatureManager::Instance()->GetSendCharacterAroundList( SendCharacterList, ulDefID );

		for ( list<unsigned long>::iterator iterSend = SendCharacterList.begin() ; 
			iterSend != SendCharacterList.end() ; 
			++iterSend )
		{
			g_pClientSocket->RegisterInstanceUDPList( (*iterSend) );
		}

		SendPacket::CharAttackInfo( *g_pClientSocket, ulCharID, pAtType, ucDefenserNum, pDefNode );

        */ 

		if ( ulCharID == pGame->m_csStatus.m_dwSummonID )
		{
			if ( pAttackCreature && pAttackData )
			{
				pAttackData->m_ulTargetCharID = pDefNode[ 0 ].m_dwCharID;

				CZ3DGeneralChrModel* pAttackModel = pAttackCreature->GetCharModel();

				if ( pAttackModel )
				{	
					pAttackModel->SetAnimationSpeed( 1.2f );

					if ( pAttackCreature->GetLowerActionID() == CA_RUN || pAttackCreature->GetLowerActionID() == CA_WALK )
					{
						pAttackModel->AddMotion( "ATTACK", 3, 1.0f );
					} 
					else
					{
						pAttackCreature->SetAction( CA_ATTACK, CA_ATTACK );
					}

					RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( pDefNode[ 0 ].m_dwCharID );

					if ( pTempCreature )
					{
						CZ3DGeneralChrModel* pTeMPModel = pTempCreature->GetCharModel();

						pTeMPModel->SetDirection( pTempCreature->GetDirection() + FLOAT_PHI );
					}
				}
			}
		}

		bool bShow = false;

		RYLDamageNode* pDamageNode  = new RYLDamageNode;
		pDamageNode->fScale         = 1.0f;
		pDamageNode->sDamageLifeTime= 1500;
		pDamageNode->usDamageType   = cJudge;

		switch ( pDamageNode->usDamageType )
		{
		case ClientConstants::Judge_Heal:
			{
				if ( pAttackCreature )
				{
					bShow = true;

					pDamageNode->lDamageValue = usHP - pAttackData->m_lCurrHP;

					//////////////////////////////////////////////////////////////////////////////////////////////////////////////
					//////////////////////////////////////////////////////////////////////////////////////////////////////////////
					// By Belldandy (2005.02.15)
					if (usMPHeal > 0)
					{
						pDamageNode->dwAlign			= RYLDamageNode::DN_Left;
						pAttackCreature->GetDamageEffect()->AddDamage( pDamageNode );

						pDamageNode						= new RYLDamageNode;
						pDamageNode->sDamageLifeTime	= 1500;
						pDamageNode->lDamageValue		= usMPHeal;
						pDamageNode->usDamageType		= ClientConstants::Judge_ManaHeal;
						pDamageNode->fScale				= 1.0f;
						pDamageNode->dwAlign			= RYLDamageNode::DN_Right;
					}
					else if (usMP - pAttackData->m_lCurrMP > 0)
					{
						pDamageNode->dwAlign			= RYLDamageNode::DN_Left;
						pAttackCreature->GetDamageEffect()->AddDamage( pDamageNode );

						pDamageNode						= new RYLDamageNode;
						pDamageNode->sDamageLifeTime	= 1500;
						pDamageNode->lDamageValue		= usMP - pAttackData->m_lCurrMP;
						pDamageNode->usDamageType		= ClientConstants::Judge_ManaHeal;
						pDamageNode->fScale				= 1.0f;
						pDamageNode->dwAlign			= RYLDamageNode::DN_Right;
					}
					//////////////////////////////////////////////////////////////////////////////////////////////////////////////
					//////////////////////////////////////////////////////////////////////////////////////////////////////////////

					pAttackCreature->GetDamageEffect()->AddDamage( pDamageNode );
				}
				else
				{
					GM_DELETE( pDamageNode ) ;
				}
			}
			break;

		case ClientConstants::Judge_ManaHeal:
			{
				if ( pAttackCreature )
				{
					bShow = true;
					pDamageNode->lDamageValue = usMP - pAttackData->m_lCurrMP;
					
					if (usHP - pAttackData->m_lCurrHP > 0)
					{
						pDamageNode->dwAlign			= RYLDamageNode::DN_Right;
						pAttackCreature->GetDamageEffect()->AddDamage( pDamageNode );

						pDamageNode						= new RYLDamageNode;
						pDamageNode->sDamageLifeTime	= 1500;
						pDamageNode->lDamageValue		= usHP - pAttackData->m_lCurrHP;
						pDamageNode->usDamageType		= ClientConstants::Judge_Heal;
						pDamageNode->fScale				= 1.0f;
						pDamageNode->dwAlign			= RYLDamageNode::DN_Left;
					}

					pAttackCreature->GetDamageEffect()->AddDamage( pDamageNode );
				}
				else
				{
					GM_DELETE( pDamageNode ) ;
				}
			}
			break;

		case ClientConstants::Judge_Gather:
		case ClientConstants::Judge_Social:
		case ClientConstants::Judge_Chant:
		case ClientConstants::Judge_Enchant:
		case ClientConstants::Judge_Disenchant:
		case ClientConstants::Judge_NonShow:
			{
				GM_DELETE( pDamageNode ) ;
			}
			break;

		case ClientConstants::Judge_Evade:
		case ClientConstants::Judge_Resist:
			{
				GM_DELETE( pDamageNode ) ;

				bShow = true;
			}
			break;

		case ClientConstants::Judge_FireCracker:
		case ClientConstants::Judge_ChinaFireCracker:
			{
				GM_DELETE( pDamageNode ) ;
			}
			break;

		default:
			{
				if ( pAttackCreature )
				{
					if ( pAttackData->m_lCurrHP > usHP )
					{
						pDamageNode->lDamageValue = pAttackData->m_lCurrHP - usHP;
						pAttackCreature->GetDamageEffect()->AddDamage( pDamageNode );

						bShow = true;
					}
					else
					{
						GM_DELETE( pDamageNode ) ;
					}
				}
				else
				{
					GM_DELETE( pDamageNode ) ;
				}
			}
			break;
        }

        /* FIXED : REMOVED UDP PACKETS
        if ( bShow )
		{
			DefenserNode DNode;
			DNode.m_dwCharID= CRYLNetworkData::Instance()->m_dwMyChrID;
			DNode.m_wDamage = pDamageNode->lDamageValue;
			DNode.m_wMaxHP  = pAttackData->m_ulMaxHP;
			DNode.m_sCurrHP = usHP;
			DNode.m_wMaxMP  = pAttackData->m_ulMaxMP;
			DNode.m_sCurrMP = usMP;
			DNode.m_cJudge  = pDamageNode->usDamageType;

			list<unsigned long> SendCharacterList;
			RYLCreatureManager::Instance()->GetSendCharacterAroundList( SendCharacterList );

			for ( list<unsigned long>::iterator iterSend = SendCharacterList.begin() ;
				iterSend != SendCharacterList.end() ; 
				++iterSend )
			{
				g_pClientSocket->RegisterInstanceUDPList( (*iterSend) );
			}

			SendPacket::CharAttackInfo( *g_pClientSocket, ulCharID, pAtType, 1, &DNode );
		}
        */

		using namespace EnemyCheck;

		if ( pAttackData )
		{
			unsigned short wObjectID = pAttackCreature->GetObjectType();

			if(!IsCastleArms(wObjectID) && !IsSiegeArms(wObjectID))
			{
				pAttackData->m_lCurrHP = usHP;
				pAttackData->m_lCurrMP = usMP;
			}					
		}

		if ( ulCharID == CRYLNetworkData::Instance()->m_dwMyChrID )
		{
			unsigned short wObjectID = pAttackCreature->GetObjectType();

			if(!IsCastleArms(wObjectID) && !IsSiegeArms(wObjectID))
			{
				pGame->m_csStatus.m_Info.HP = usHP;
				pGame->m_csStatus.m_Info.MP = usMP;
			}

			if ( usHP == 0 )
			{
				// 자살
				pGame->m_csStatus.m_Info.HP = 0;
				g_CharacterData.InitGlobalAction();

				pSelfData->m_lCurrHP = 0;

				pSelfCreature->Dead();

				pGame->m_lpInterface->HideInterfaceforDead();
				pGame->m_csStatus.ChantDisable();

				CRYLGameData::SetAutoRun(false);

				g_CharacterData.m_bAutoFollowMode = false;
				g_CharacterData.m_dwAutoFollowChrID = 0xFFFFFFFF;

				CRYLNetworkData::SendMovingPacket( pSelfCreature->GetUpperActionID(), pSelfCreature->GetLowerActionID(),
					pSelfCreature->GetDirection(), *(pSelfCreature->GetPosition()) );

				if (/*SERVER_ID::STONE_WAR1*/SERVER_ID::CAPITAL <= CRYLNetworkData::Instance()->m_dwPresentZone && CRYLNetworkData::Instance()->m_dwPresentZone <= SERVER_ID::STONE_WAR3)
				{
					pGame->SetRespawnCounter(60000);
				}
			}

		}
		else
		{
			if ( usHP == 0 )
			{
				// 자살
				pAttackCreature->Dead();
			}
		}

		if ( ucDefenserNum )
		{
			RYLCreature*    pTempCreature = NULL;
			CharDataNode*   pTempCharData = NULL;

			for ( int i = 0; i < ucDefenserNum; i++ )
			{
				if ( pDefNode[ i ].m_dwCharID == CRYLNetworkData::Instance()->m_dwMyChrID ) 
					continue;

				pTempCreature = RYLCreatureManager::Instance()->GetCreature( pDefNode[ i ].m_dwCharID );
				pTempCharData = RYLCharacterDataManager::Instance()->GetCharData( pDefNode[ i ].m_dwCharID );
				
				if ( pTempCreature && pTempCharData )
				{
					long lSaveHp = pTempCharData->m_lCurrHP;
					pTempCharData->m_ulMaxHP = pDefNode[ i ].m_wMaxHP;
					pTempCharData->m_ulMaxMP = pDefNode[ i ].m_wMaxMP;
					pTempCharData->m_lCurrHP = pDefNode[ i ].m_sCurrHP;
					pTempCharData->m_lCurrMP = pDefNode[ i ].m_sCurrMP;

					// 모션 블로우 처리
					if ( pTempCreature->GetCharID() & Creature::MONSTER_BIT )
					{
						const CMonsterMgr::MonsterProtoType *lpMonster = g_MonsterMgr.GetMonsterProtoType( pTempCreature->GetCharID() & 0x0000FFFF );

						if ( lpMonster && lpMonster->m_MonsterInfo.m_cSkillPattern == MonsterInfo::MonsterPattern::PATTERN_STRUCTURE )
						{
							float fNousHP;

							if ( pTempCharData->m_ulMaxHP ) 
							{
								fNousHP = ( float )pTempCharData->m_lCurrHP / ( float )pTempCharData->m_ulMaxHP; 
							}
							else 
							{   
								fNousHP = 0.0f;
							}

							if ( fNousHP < 0.34f )
							{
								if ( pTempCreature->GetUpperActionID() == CA_WAITH1 ) 
								{
									pTempCreature->SetAction( CA_WAITH2, CA_WAITH2 );
								}
							} 
							else if ( fNousHP < 0.67f )
							{
								if ( pTempCreature->GetUpperActionID() == CA_WAIT ) 
								{
									pTempCreature->SetAction( CA_WAITH1, CA_WAITH1 );
								}
							}
						}
					}

					if ( pTempCreature->GetCharID() == CRYLNetworkData::Instance()->m_dwMyChrID )
					{
						pGame->m_csStatus.m_Info.HP = pDefNode[ i ].m_sCurrHP;
						pGame->m_csStatus.m_Info.MP = pDefNode[ i ].m_sCurrMP;

						if ( pDefNode[ i ].m_sCurrHP <= 0 )
						{
							pGame->m_csStatus.m_Info.HP = 0;
							g_CharacterData.m_bAutoTargetAttack = false;
							g_CharacterData.m_dwAutoTargetID = 0xFFFFFFFF;
						}
					} 
					else
					{
						// 스킬 2차 이펙트를 붙인다.
						if ( ( pAtType.m_wType & Skill::SKILL_MASK ) && ( pAtType.m_cSkillLevel || pAtType.m_cSkillLockCount ) )
						{
							if (pAtType.m_wType != 0x8708 || i == 0)
							{
								RYLCreature* pAttackCreature = RYLCreatureManager::Instance()->GetCreature( ulCharID );

								GameProcess::SetHitEffect( pAtType, pTempCreature, ( pAttackCreature ? pAttackCreature : NULL ), i );
							}
						}
					}

					RYLDamageNode* pDamageNode  = new RYLDamageNode;
					pDamageNode->sDamageLifeTime	= 1500;
					pDamageNode->usDamageType		= pDefNode[ i ].m_cJudge;
					pDamageNode->fScale				= 1.0f;

					switch ( pDamageNode->usDamageType )
					{
					case ClientConstants::Judge_ManaShell:
						{
							pTempCreature->GetEffect()->AddBodyEffect("Chant_ManaShell.esf", (unsigned long)0, true);

							pDamageNode->usDamageType = ClientConstants::Judge_Guard;
							pTempCreature->GetDamageEffect()->AddDamage( pDamageNode );
						}
						break;

					case ClientConstants::Judge_Guard:
						{
							pDamageNode->lDamageValue   = pDefNode[i].m_wDamage;
							pTempCreature->GetDamageEffect()->AddDamage( pDamageNode );

							{
								CEffScript *test_script = new CEffScript;
								CSceneManager::AddEffectScript(test_script);
								test_script->GetScriptBinData("guard_blocking.esf");

								test_script->SetStartPos( pTempCreature->GetPosition()->x, pTempCreature->GetPosition()->y, pTempCreature->GetPosition()->z );
								test_script->SetEndPos( pTempCreature->GetPosition()->x, pTempCreature->GetPosition()->y, pTempCreature->GetPosition()->z );

								// 스킬 이펙트 사이즈 설정 - Vincent
								if (CRYLGameData::GetClientIdentity( pTempCreature->GetCharID() ) == Creature::CT_MONSTER )
								{
									const CMonsterMgr::MonsterProtoType *pMonster = g_MonsterMgr.GetMonsterProtoType( pTempCreature->GetCharID() & 0x0000FFFF );
									if ( pMonster )
									{
										test_script->SetScale( pTempCreature->GetCharModel()->GetEffectScale() * pMonster->m_MonsterInfo.m_fSkillEffectSize );
									}
								}
								// ---------------------------------
							}
						}
						break;

					case ClientConstants::Judge_Critical:
						{
							pDamageNode->lDamageValue   = pDefNode[i].m_wDamage;
							pTempCreature->GetDamageEffect()->AddDamage( pDamageNode );

							{
								CEffScript *test_script = new CEffScript;
								CSceneManager::AddEffectScript( test_script );			
								test_script->GetScriptBinData("guard_critical.esf");

								test_script->SetStartPos(pTempCreature->GetPosition()->x, pTempCreature->GetPosition()->y, pTempCreature->GetPosition()->z);
								test_script->SetEndPos(pTempCreature->GetPosition()->x, pTempCreature->GetPosition()->y, pTempCreature->GetPosition()->z);

								// 스킬 이펙트 사이즈 설정 - Vincent
								if (CRYLGameData::GetClientIdentity( pTempCreature->GetCharID() ) == Creature::CT_MONSTER)
								{
									const CMonsterMgr::MonsterProtoType *pMonster = g_MonsterMgr.GetMonsterProtoType( pTempCreature->GetCharID() & 0x0000FFFF);
									if ( pMonster )
									{
										test_script->SetScale( pTempCreature->GetCharModel()->GetEffectScale() * pMonster->m_MonsterInfo.m_fSkillEffectSize );
									}
								}
								// ---------------------------------
							}
						}
						break;

					case ClientConstants::Judge_Gather:
					case ClientConstants::Judge_Social:
					case ClientConstants::Judge_Chant:
					case ClientConstants::Judge_Disenchant:
					case ClientConstants::Judge_NonShow:
					case ClientConstants::Judge_FireCracker:
					case ClientConstants::Judge_ChinaFireCracker:
						{
							GM_DELETE( pDamageNode ) ;
						}
						break;

					case ClientConstants::Judge_Enchant:
						{
							if ( pDefNode[i].m_wDamage > 0 )
							{
								pDamageNode->lDamageValue   = pDefNode[i].m_wDamage;
								pTempCreature->GetDamageEffect()->AddDamage( pDamageNode );
							}
							else
							{
								GM_DELETE( pDamageNode ) ;
							}
						}
						break;

					case ClientConstants::Judge_Evade:
						{
							pDamageNode->lDamageValue   = pDefNode[i].m_wDamage;
							pTempCreature->GetDamageEffect()->AddDamage( pDamageNode );

						}
						break;

					case ClientConstants::Judge_Resist:
						{
							pDamageNode->lDamageValue   = pDefNode[i].m_wDamage;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// By Belldandy (2005.02.15)
							if (lSaveHp - pDefNode[i].m_sCurrHP)
							{
								pDamageNode->dwAlign		= RYLDamageNode::DN_Right;

								pTempCreature->GetDamageEffect()->AddDamage( pDamageNode );

								pDamageNode = new RYLDamageNode;
								pDamageNode->lDamageValue = lSaveHp - pDefNode[i].m_sCurrHP;
								if (pDamageNode->lDamageValue > 0)
									pDamageNode->usDamageType	= ClientConstants::Judge_Front;
								else
									pDamageNode->usDamageType	= ClientConstants::Judge_Heal;
								pDamageNode->sDamageLifeTime= 1500;
								pDamageNode->fScale         = 1.0f;
								pDamageNode->dwAlign		= RYLDamageNode::DN_Left;
							}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

							pTempCreature->GetDamageEffect()->AddDamage( pDamageNode );
						}
						break;

					default:
						{
							pDamageNode->lDamageValue   = pDefNode[i].m_wDamage;
							pTempCreature->GetDamageEffect()->AddDamage( pDamageNode );

						}
						break;
					}

					if ( ulCharID != CRYLNetworkData::Instance()->m_dwMyChrID )
					{
						g_CharacterData.RegistAttacked( ulCharID, pDefNode[ i ].m_dwCharID, pAtType.m_wType );
					}

					if (ulCharID == CRYLNetworkData::Instance()->m_dwMyChrID && pDamageNode)
					{
						if (pDefNode[ i ].m_wDamage > 0)
						{
							CRYLGameScene*				pScene			= (CRYLGameScene*)CRYLSceneManager::Instance()->FindScene(GAME_SCENE);
							CRYLJoystickConfigureDlg*	pJoystickDlg	= (CRYLJoystickConfigureDlg*)pScene->GetJoystickConfigureDlg() ;
							CRYLJoystickConfigurePanel* pJoystickPanel	= (CRYLJoystickConfigurePanel*)pJoystickDlg->GetRYLJoystickPanel();

							if (pJoystickPanel->GetForcefeedback() && ClientConstants::Judge_Critical == pDamageNode->usDamageType)
							{
								// edith 조이스틱 틸트 이팩트
								// ForceFeedback : 일반 공격으로 크리티컬 데미지
								KeyControl::StartForceFeedback(1000, 1000, 200);
							}
							else if (pJoystickPanel->GetForcefeedback() && pAtType.m_wType & Skill::SKILL_MASK)
							{
								// ForceFeedback : 인스턴스 스킬 공격 데미지
								KeyControl::StartForceFeedback(1500, 1500, 500);
							}
						}
					}

					if ( pDefNode[ i ].m_sCurrHP <= 0 && cJudge != ClientConstants::Judge_NonShow )
					{
						pTempCharData->m_ulMaxHP = 0;
						pTempCharData->m_ulMaxMP = 0;
						pTempCharData->m_lCurrHP = -1;
						pTempCharData->m_lCurrMP = -1;

						if ( CRYLGameData::GetClientIdentity( pTempCreature->GetCharID() ) == Creature::CT_PC )
						{
							if ( ulCharID == CRYLNetworkData::Instance()->m_dwMyChrID )
							{
								char strText[ MAX_PATH ];
								char* strName = const_cast<char*>( pTempCharData->m_strName.c_str() );
								unsigned long ulCreatureType = CRYLGameData::GetClientIdentity( pDefNode[ i ].m_dwCharID );
								if (strName)
								{
									if ( ulCreatureType == Creature::CT_PC )
									{
/*
										// 상대 국적 캐릭터의 이름은 클래스 이름으로 대체한다.
										if (pGame->m_cAccountNation != pTempCharData->m_cNation &&
											!CRYLNetworkData::Instance()->UseContents(GameRYL::SHOW_ENEMYNAME))
										{
											sprintf( strText, CRYLStringTable::m_strString[ 372 ], CRYLStringTable::GetJobName(pTempCharData->m_usClass) );
											pChat->AddMessage( strText, CRYLNetworkData::Instance()->m_dwMyChrID, 128 );
										}
										else
										{
											sprintf( strText, CRYLStringTable::m_strString[ 372 ], strName );
											pChat->AddMessage( strText, CRYLNetworkData::Instance()->m_dwMyChrID, 128 );
										}
*/
										sprintf( strText, CRYLStringTable::m_strString[ 372 ], strName );
										pChat->AddMessage( strText, CRYLNetworkData::Instance()->m_dwMyChrID, 128 );
									}
								} 
								else
								{
									pChat->AddMessage( CRYLStringTable::m_strString[ 373 ], CRYLNetworkData::Instance()->m_dwMyChrID, 128 );
								}
							}

							// 성문은 죽지 않는다.
							if ( Siege::GATE != pTempCreature->GetObjectType() )
							{
								pTempCreature->Dead();
							}

							if ( pTempCreature->GetCharID() == CRYLNetworkData::Instance()->m_dwMyChrID )
							{
								CRYLNetworkData::SendMovingPacket( pTempCreature->GetUpperActionID(), pTempCreature->GetLowerActionID(),
									pTempCreature->GetDirection(), *( pTempCreature->GetPosition() ) );
							}
						} 
						else
						{
							unsigned long ulMonsterID = pTempCreature->GetCharID();
							const CMonsterMgr::MonsterProtoType *lpMonster = g_MonsterMgr.GetMonsterProtoType( ulMonsterID & 0x0000FFFF );

							// 석상은 Dead 함수를 호출하지 않고, 바로 삭제한다.
							if ( lpMonster && lpMonster->m_MonsterInfo.m_cSkillPattern == MonsterInfo::MonsterPattern::PATTERN_STRUCTURE )
							{
								GameProcess::LogCharUnload(pTempCreature->GetCharID(), LOG_RFL);
								RYLCreatureManager::Instance()->DeleteCreature( pTempCreature->GetCharID() );
							}
							else
							{
                                /*
                                    몬스터 사망 로그
                                vector3* vec3 = pTempCreature->GetPosition();
                                const CNetworkPos nowNetPos(vec3->x / 100.0f, vec3->y / 100.0f, vec3->z / 100.0f);

                                const int MAX_BUFFER = 256;
                                char szBuffer[MAX_BUFFER];

                                if (0 < _snprintf(szBuffer, MAX_BUFFER - 1, 
                                    "PktAttack : CID:0x%08x : 몬스터 사망하셨다. (%f, %f, %f)\n", 
                                    pTempCreature->GetCharID(), nowNetPos.GetXPos(), nowNetPos.GetYPos(), nowNetPos.GetZPos()))
                                {
                                    szBuffer[MAX_BUFFER - 1] = 0;
                                    OutputDebugString(szBuffer);
                                } 
                                */

								// 성문은 죽지 않는다.
								if ( Siege::GATE != pTempCreature->GetObjectType() )
								{
									pTempCreature->Dead();
								}
							}
						}
					}
				}
			}
		}

		return true;
	}
	else
	{
		switch ( CRYLNetworkData::Instance()->m_dwNetworkError )
		{
		case 3:
			{
				pChat->AddMessage( CRYLStringTable::m_strString[ 418 ], CRYLNetworkData::Instance()->m_dwMyChrID, 129 );
			}
			break;

		case 4:
			{
				pChat->AddMessage( CRYLStringTable::m_strString[ 419 ], CRYLNetworkData::Instance()->m_dwMyChrID, 129 );
			}
			break;

		case 5:
			{
				pChat->AddMessage(CRYLStringTable::m_strString[1580], CRYLNetworkData::Instance()->m_dwMyChrID, 129);
			}
			break;

		case 6 :
			{
				pChat->AddMessage(CRYLStringTable::m_strString[1582], CRYLNetworkData::Instance()->m_dwMyChrID, 129);
			}
			break;

		default:
			{
				/*
				if ( pAtType.m_wType & Skill::SKILL_MASK )
				{
				const Skill::ProtoType *lpSkillProtoType = g_SkillMgr.GetSkillProtoType( pAtType.m_wType );

				if ( lpSkillProtoType && lpSkillProtoType->m_cEndCoolDown )
				{
				pGame->SetSkillCoolDown( pAtType.m_wType );
				}
				}
				*/
			}
			break;
		}

		if ( pAtType.m_wType & Skill::SKILL_MASK )
		{
			const Skill::ProtoType *lpSkillProtoType = g_SkillMgr.GetSkillProtoType( pAtType.m_wType );

			if ( lpSkillProtoType && lpSkillProtoType->m_cEndCoolDown )
			{
				pGame->SetSkillCoolDown( pAtType.m_wType );
			}
		}
	}

	return false;
}

//--------------------------------------------------------------------------------------------------------------------------
// GAME_EVENT_FUNC(ParseCharAttacked)
GAME_EVENT_FUNC(ParseCharAttacked)
{
	unsigned long	ulAttCharID, ulDefCharID;
	unsigned short	usJudge;
	float			fDir;
	POS				psPos;
	AtType			pAtType;
	unsigned short	usHp, usMp, usMpHeal;
	CRYLGameData*	pGame = CRYLGameData::Instance();

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharAttacked( lpPktBase, 
		&ulAttCharID, 
		&ulDefCharID, 
		&pAtType, 
		&psPos, 
		&fDir, 
		&usJudge, 
		&usHp, 
		&usMp,
		&usMpHeal );

	CRYLChattingDlg* pChat = CRYLChattingDlg::Instance();

	if (CRYLNetworkData::Instance()->m_dwNetworkError == 0)
	{
		RYLCreature*    pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
		if ( !pSelfCreature )
			return false;

		CharDataNode*   pSelfData = RYLCharacterDataManager::Instance()->GetCharData( pSelfCreature->GetCharID() );
		if ( !pSelfData )
			return false;

		RYLCreature* pDefCreature = RYLCreatureManager::Instance()->GetCreature( ulDefCharID );
		if ( !pDefCreature )
			return true;

		CharDataNode* pDefData = RYLCharacterDataManager::Instance()->GetCharData( ulDefCharID );
		if ( !pDefData )
			return true;

		/*if (pSelfCreature->GetGMModelFlag() && pSelfCreature!=pDefCreature)
		{
			CEffScript* pInputEffect;
			pInputEffect = new CEffScript;

			pInputEffect->GetScriptBinData("adminattack.esf");
			pInputEffect->SetStartPos(pSelfCreature->GetPosition()->x, pSelfCreature->GetPosition()->y, pSelfCreature->GetPosition()->z);
			pInputEffect->SetEndPos(pSelfCreature->GetPosition()->x, pSelfCreature->GetPosition()->y, pSelfCreature->GetPosition()->z);
			pInputEffect->SetPlayer(pSelfCreature, 0);

			pInputEffect->SetChr(pSelfCreature->GetCharModel(), pSelfCreature->GetCharModel());

			float fChrDir = pSelfCreature->GetDirection();
			fChrDir = -fChrDir;
			fChrDir -= FLOAT_PHI / 2.0f;

			vector3 vecChrToward;
			vecChrToward.x = cosf(fChrDir);
			vecChrToward.z = sinf(fChrDir);	
			vecChrToward.y = 0.0f;
			vecChrToward.Normalize();
			pInputEffect->SetChrFwd(-vecChrToward.x, -vecChrToward.y, -vecChrToward.z);

			pInputEffect->SetMine( true );
			CSceneManager::m_EffectManager.AddEffScript( pInputEffect );
		}*/

		RYLDamageNode* pDamageNode		= new RYLDamageNode;
		pDamageNode->sDamageLifeTime	= 1500;
		pDamageNode->usDamageType		= usJudge;
		pDamageNode->fScale				= 1.0f;
		unsigned short usDamageType		= usJudge;

		switch ( pDamageNode->usDamageType )
		{
			case ClientConstants::Judge_ManaShell:
			{
				usDamageType = pDamageNode->usDamageType = ClientConstants::Judge_Guard;

				if ( pDefCreature )
				{
					pDefCreature->GetEffect()->AddBodyEffect( "Chant_ManaShell.esf", (unsigned long)0, true );
					pDefCreature->GetDamageEffect()->AddDamage( pDamageNode );
				}
				else
				{
					GM_DELETE( pDamageNode ) ;
				}
				break;
			}
	
			case ClientConstants::Judge_Heal:
			{
				if ( pDefCreature )
				{
					pDamageNode->lDamageValue = usHp - pDefData->m_lCurrHP;

					if ( !pDamageNode->lDamageValue )
					{
						pChat->AddMessage( CRYLStringTable::m_strString[ 275 ], CRYLNetworkData::Instance()->m_dwMyChrID, 129 );
					}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// By Belldandy (2005.02.15)
					if (usMpHeal > 0)
					{
						pDamageNode->dwAlign			= RYLDamageNode::DN_Left;
						pDefCreature->GetDamageEffect()->AddDamage( pDamageNode );

						pDamageNode						= new RYLDamageNode;
						pDamageNode->sDamageLifeTime	= 1500;
						pDamageNode->lDamageValue		= usMpHeal;
						usDamageType = pDamageNode->usDamageType = ClientConstants::Judge_ManaHeal;
						pDamageNode->fScale				= 1.0f;
						pDamageNode->dwAlign			= RYLDamageNode::DN_Right;
					}
					else if (usMp - pDefData->m_lCurrMP > 0)
					{
						pDamageNode->dwAlign			= RYLDamageNode::DN_Left;
						pDefCreature->GetDamageEffect()->AddDamage( pDamageNode );

						pDamageNode						= new RYLDamageNode;
						pDamageNode->sDamageLifeTime	= 1500;
						pDamageNode->lDamageValue		= usMp - pDefData->m_lCurrMP;
						usDamageType = pDamageNode->usDamageType = ClientConstants::Judge_ManaHeal;
						pDamageNode->fScale				= 1.0f;
						pDamageNode->dwAlign			= RYLDamageNode::DN_Right;
					}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

					pDefCreature->GetDamageEffect()->AddDamage( pDamageNode );
				}
				else
				{
					GM_DELETE( pDamageNode ) ;
				}
				break;
			}

			case ClientConstants::Judge_ManaHeal:
			{
				if ( pDefCreature )
				{
					pDamageNode->lDamageValue = usMp - pDefData->m_lCurrMP;

					if ( !pDamageNode->lDamageValue )
					{
						pChat->AddMessage( CRYLStringTable::m_strString[ 275 ], CRYLNetworkData::Instance()->m_dwMyChrID, 129 );
					}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// By Belldandy (2005.02.15)
					if (usHp - pDefData->m_lCurrHP)
					{
						pDamageNode->dwAlign			= RYLDamageNode::DN_Right;
						pDefCreature->GetDamageEffect()->AddDamage( pDamageNode );

						pDamageNode						= new RYLDamageNode;
						pDamageNode->sDamageLifeTime	= 1500;
						pDamageNode->lDamageValue		= usHp - pDefData->m_lCurrHP;
						if (pDamageNode->lDamageValue > 0)
							usDamageType = pDamageNode->usDamageType = ClientConstants::Judge_Heal;
						else
							usDamageType = pDamageNode->usDamageType = ClientConstants::Judge_Front;
						pDamageNode->fScale				= 1.0f;
						pDamageNode->dwAlign			= RYLDamageNode::DN_Left;
					}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

					pDefCreature->GetDamageEffect()->AddDamage( pDamageNode );
				}
				else
				{
					GM_DELETE( pDamageNode ) ;
				}
				break;
			}

			case ClientConstants::Judge_Gather:
			case ClientConstants::Judge_Social:
			case ClientConstants::Judge_Chant:
			case ClientConstants::Judge_NonShow:
			case ClientConstants::Judge_Disenchant:
				{
					GM_DELETE( pDamageNode ) ;
				}
				break;

			case ClientConstants::Judge_Enchant:
				{
					if ( pDefCreature && pDefData->m_lCurrHP - usHp > 0 )
					{
						pDamageNode->lDamageValue = pDefData->m_lCurrHP - usHp;
						pDefCreature->GetDamageEffect()->AddDamage( pDamageNode );
					}
					else
					{
						GM_DELETE( pDamageNode ) ;
					}

					// edith 2009.07.29 점프 스킬 시전 버그 수정부분
					if ( ulDefCharID == CRYLNetworkData::Instance()->m_dwMyChrID )
						pSelfCreature->SetFlying(false);
				}
				break;

			case ClientConstants::Judge_Evade:
			{
				if ( pDefCreature )
				{
					pDefCreature->GetDamageEffect()->AddDamage( pDamageNode );
				}
				else
				{
					GM_DELETE( pDamageNode ) ;
				}
				break;
			}

			case ClientConstants::Judge_Resist:
			{
				if ( pDefCreature )
				{
					if (usHp - pDefData->m_lCurrHP)
					{
						pDamageNode->dwAlign		= RYLDamageNode::DN_Right;
						pDefCreature->GetDamageEffect()->AddDamage( pDamageNode );

						pDamageNode = new RYLDamageNode;
						pDamageNode->lDamageValue = usHp - pDefData->m_lCurrHP;
						if (pDamageNode->lDamageValue > 0)
							usDamageType = pDamageNode->usDamageType = ClientConstants::Judge_Heal;
						else
							usDamageType = pDamageNode->usDamageType = ClientConstants::Judge_Front;
						pDamageNode->sDamageLifeTime= 1500;
						pDamageNode->fScale         = 1.0f;
						pDamageNode->dwAlign		= RYLDamageNode::DN_Left;
					}

					pDefCreature->GetDamageEffect()->AddDamage( pDamageNode );
				}
				else
				{
					GM_DELETE( pDamageNode ) ;
				}
				break;
			}

			case ClientConstants::Judge_FireCracker:
				{
					CRYLNetworkData::Instance()->ChristmasParticle( ulDefCharID, pAtType.m_wType, pAtType.m_cSkillLockCount ) ;
					GM_DELETE( pDamageNode ) ;
				}
				break;

			case ClientConstants::Judge_ChinaFireCracker:
				{
					char Temp[MAX_PATH];
					int nLevel[3] = { 100, 300, 500 };
					sprintf(Temp, "%s\\Sound\\ChinaCracker_%d.wav", g_ClientMain.m_strClientPath, nLevel[pAtType.m_cSkillLockCount]);
					CSound* pSound; 
					CSoundMgr::_GetInstance()->GetSound( pSound, Temp ); 
					pSound->Play(); 

					GM_DELETE( pDamageNode ) ;
				}
				break;

			case ClientConstants::Judge_Counter:
			{
				RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();

				g_CharacterData.m_bSkillExecuted = FALSE;
				pSelfCreature->CancelSkill();

				const Skill::ProtoType *pSkillType = g_SkillMgr.GetSkillProtoType(0x8606);

				if (strcmp(pSkillType->m_szCastingFileName, "NULL"))
				{
					pSelfCreature->EndSkill((char *)pSkillType->m_szCastingFileName);
				}

				pSelfCreature->m_ulSkillComboCount = 0;
				pSelfCreature->m_bSkillCombo = false;

				CSkillNode* pSkillNode = new CSkillNode;
				pSkillNode->Create(CRYLNetworkData::Instance()->m_dwMyChrID, 0x8606, 0, 0, NULL);

				pSelfCreature->SetStillCasting( false );

				CRYLGameData::Instance()->m_lpInterface->m_bShowSkillError = TRUE;

				GM_DELETE( pDamageNode ) ;
				break;
			}

			default:
			{		
				if ( pDefCreature )
				{
					pDamageNode->lDamageValue = pDefData->m_lCurrHP - usHp;
					pDefCreature->GetDamageEffect()->AddDamage( pDamageNode );
				}
				else
				{
					GM_DELETE( pDamageNode ) ;
				}
				break;
			}
		}

		if ( ulDefCharID == CRYLNetworkData::Instance()->m_dwMyChrID )
		{
			bool bRestCancel = true;
			bool bCastCancel = true;

			if ( pSelfCreature->GetEffect()->GetChantStatus( Skill::SpellID::ManaShell ) || 
				pSelfCreature->GetEffect()->GetChantStatus( Skill::SpellID::Flexibility ) )
			{
				if ( pAtType.m_wType & Skill::SKILL_MASK )
				{
					const Skill::ProtoType* lpSkillType = g_SkillMgr.GetSkillProtoType( pAtType.m_wType );
					if ( lpSkillType )
					{
						if ( true == lpSkillType->m_bProtection )
						{
							bCastCancel = false;
						}
					}
				} 
				else
				{
					bCastCancel = false;
				}
			}

			if ( pAtType.m_wType & Skill::SKILL_MASK )
			{
				const Skill::ProtoType* lpSkillType = g_SkillMgr.GetSkillProtoType( pAtType.m_wType );
				if ( lpSkillType )
				{
					if ( false == lpSkillType->m_bInterrupt )
					{
						bCastCancel = false;
						bRestCancel = false;
					}
				}
			}

			if ( pGame->m_csStatus.m_aryEnchantLevel[ Skill::SpellID::Concentration ] )		// 매직 포션 사용시
			{
				bCastCancel = false;
			}

			if ( true == bRestCancel )
			{
				// 특별한 경우가 아니면 공격 당했을 때 자동으로 일어선다.
				if ( pSelfCreature->GetUpperActionID() == CA_REST && pSelfCreature->GetLowerActionID() == CA_REST )
				{
					// edith 2009.07.16 공격당한 후에는 점프 플래그 끄기
					pSelfCreature->SetFlying(false);
					pSelfCreature->SetRunFactor( 0.0f );
					pSelfCreature->SetAction( CA_WAIT, CA_WAIT );
					pSelfCreature->SetSitMode( false );
				}
			}

			


			if ( true == bCastCancel )
			{
				// 스킬 캐스팅 실패
				if ( pSelfCreature->GetStillCasting() &&
					!( usJudge == ClientConstants::Judge_Heal || usJudge == ClientConstants::Judge_ManaHeal ) )
				{
					float fTime = CSceneManager::GetWeatherTime();

					if ( ( fTime < 16.0f ) && ( fTime > 9.0f ) )
					{
						CCameraControl::SetFadeOut(1.0f, 1.0f, 30.0f);
					}

					// 캐스팅 중이라면 그 스킬의 ID
					const Skill::ProtoType* lpSkillTypeTemp = g_SkillMgr.GetSkillProtoType( pGame->m_csStatus.m_lpQuickSelected->GetSkillID() );	
					const Skill::ProtoType* lpSkillType = &lpSkillTypeTemp[ g_CharacterData.m_dwCastingSkillGrade ];

					unsigned long cLevel;
					if ( Skill::Type::INSTANCE == lpSkillType->m_eSkillType )
					{
						cLevel = ( unsigned char )g_CharacterData.m_dwCastingSkillTick;
					} 
					else if ( Skill::Type::GATHER == lpSkillType->m_eSkillType )
					{
						cLevel = ( unsigned char )g_CharacterData.m_dwCastingSkillTick;
					} 
					else
					{
						unsigned long dwLevelTick = lpSkillType->m_LevelTick;
						cLevel = g_CharacterData.m_dwCastingSkillTick / dwLevelTick;
						if ( cLevel > CSkillMgr::MAX_SKILL_LEVEL )
						{
							cLevel = CSkillMgr::MAX_SKILL_LEVEL;
						}
					}

					unsigned long pulDefenser[ 10 ];
					ZeroMemory( pulDefenser, sizeof( unsigned long ) * 10 );

					CRYLNetworkData::SendCharAttack( *(pSelfCreature->GetPosition()), 
						pSelfCreature->GetDirection() - FLOAT_PHI / 2.0f,
						pGame->m_csStatus.m_lpQuickSelected->GetSkillID(),     // 캐스팅중이라면 그 스킬ID
						false, 
						g_CharacterData.m_dwCastingSkillGrade, 
						cLevel, 
						0, 
						0, 
						pulDefenser );

					pGame->SetSkillCoolDown(g_CharacterData.m_dwCastingSkillGrade, 0);
					g_CharacterData.m_bSkillExecuted = false;

					pSelfCreature->CancelSkill();
					pSelfCreature->SetStillCasting( false );
				}
			}
		}

		switch ( usDamageType )
		{
		case ClientConstants::Judge_Resist:
		case ClientConstants::Judge_Guard:  
		case ClientConstants::Judge_Critical:
			{
				if (pDefCreature)
				{
					CEffScript *test_script = new CEffScript;
					CSceneManager::AddEffectScript( test_script );			
					if ( usDamageType == ClientConstants::Judge_Guard 
						|| usDamageType == ClientConstants::Judge_Resist)
					{
						test_script->GetScriptBinData( "guard_blocking.esf" );
					}
					else if ( usDamageType == ClientConstants::Judge_Critical )
					{
						test_script->GetScriptBinData( "guard_critical.esf" );
					}

					test_script->SetStartPos( pDefCreature->GetPosition()->x, pDefCreature->GetPosition()->y, pDefCreature->GetPosition()->z );
					test_script->SetEndPos( pDefCreature->GetPosition()->x, pDefCreature->GetPosition()->y, pDefCreature->GetPosition()->z );

					// 스킬 이펙트 사이즈 설정 - Vincent
					if ( CRYLGameData::GetClientIdentity( pDefCreature->GetCharID() ) == Creature::CT_MONSTER )
					{
						const CMonsterMgr::MonsterProtoType *pMonster = g_MonsterMgr.GetMonsterProtoType( pDefCreature->GetCharID() & 0x0000FFFF );
						if (NULL != pMonster)
						{
							test_script->SetScale( pDefCreature->GetCharModel()->GetEffectScale() * pMonster->m_MonsterInfo.m_fSkillEffectSize );
						}
					}
					// ---------------------------------
				}
			}
			break;
		}

		if ( CRYLGameData::GetClientIdentity( ulAttCharID ) == Creature::CT_MONSTER )
		{
			RYLCreature *pAttCreature = RYLCreatureManager::Instance()->GetCreature( ulAttCharID );

			if ( pAttCreature )
			{
				CharDataNode* pAttData = RYLCharacterDataManager::Instance()->GetCharData( ulAttCharID );
				if ( !pAttData )
					return true;

				pAttData->m_ulTargetCharID = ulDefCharID;

				CZ3DGeneralChrModel* pAttModel = pAttCreature->GetCharModel();
				if ( !pAttModel )
					return true;

				if ( pAttModel )
				{
					pAttModel->SetAnimationSpeed( 1.2f );

					if ( pAttCreature->GetLowerActionID() == CA_RUN || pAttCreature->GetUpperActionID() == CA_WALK )
					{
						// edith 2009.08.06 스킬공격시 액션 상태를 Action로 설정
//						pAttCreature->SetAction(CA_WAIT, CA_WAIT);
						pAttModel->AddMotion( "ATTACK", 3, 1.0f );
					}
					else
					{
						// edith 2009.08.06 스킬공격시 액션 상태를 Action로 설정
//						pAttCreature->SetAction(CA_WAIT, CA_WAIT);
						pAttCreature->SetAction( CA_ATTACK, CA_ATTACK );
					}

					pAttCreature->SetDirection( -(fDir) - FLOAT_PHI / 2.0f );
				}
			} 
			else
			{
				const CMonsterMgr::MonsterProtoType* pMonster = g_MonsterMgr.GetMonsterProtoType( ulAttCharID & 0x0000FFFF );

				if ( pMonster )
				{
					GameProcess::LogCharLoad(ulAttCharID, LOG_RFL);

					vector3 vecPos = vector3( psPos.fPointX * 100.0f, psPos.fPointY * 100.0f, psPos.fPointZ * 100.0f );

					RYLCreatureManager::Instance()->AddCreature( ulAttCharID, pMonster->m_MonsterInfo.m_strName, (char *)pMonster->m_MonsterInfo.m_strModelingFlag, 
						vecPos, -(fDir) - FLOAT_PHI / 2.0f, pMonster->m_MonsterInfo.m_bCollision, pMonster->m_MonsterInfo.m_cNation );

					pAttCreature = RYLCreatureManager::Instance()->GetCreature( ulAttCharID );

					CZ3DGeneralChrModel* pAttModel = pAttCreature->GetCharModel();
					if ( !pAttModel )
						return true;

					CharDataNode* pAttData = RYLCharacterDataManager::Instance()->GetCharData( ulAttCharID );

					if ( pAttCreature && pAttModel && pAttData )
					{
						pAttModel->SetAnimationSpeed( 1.2f );
						pAttData->m_ulTargetCharID = CRYLNetworkData::Instance()->m_dwMyChrID;

						if ( pAttCreature->GetLowerActionID() == CA_RUN || pAttCreature->GetLowerActionID() == CA_WALK )
						{
							// edith 2009.08.06 스킬공격시 액션 상태를 Action로 설정
//							pAttCreature->SetAction(CA_WAIT, CA_WAIT);
							pAttModel->AddMotion( "ATTACK", 3, 1.0f );
						}
						else
						{
							// edith 2009.08.06 스킬공격시 액션 상태를 Action로 설정
//							pAttCreature->SetAction(CA_WAIT, CA_WAIT);
							pAttCreature->SetAction( CA_ATTACK, CA_ATTACK );
						}
					}
				}
			}
		}
		else if ( CRYLGameData::GetClientIdentity( ulAttCharID ) == Creature::CT_PC &&
			CRYLNetworkData::Instance()->m_dwMyChrID != ulAttCharID)
		{
			CRYLGameScene*				pScene			= (CRYLGameScene*)CRYLSceneManager::Instance()->FindScene(GAME_SCENE);
			CRYLJoystickConfigureDlg*	pJoystickDlg	= (CRYLJoystickConfigureDlg*)pScene->GetJoystickConfigureDlg() ;
			CRYLJoystickConfigurePanel* pJoystickPanel	= (CRYLJoystickConfigurePanel*)pJoystickDlg->GetRYLJoystickPanel();

			if (pJoystickPanel->GetForcefeedback())
			{
				if ( pAtType.m_wType & Skill::SKILL_MASK )
				{
					// ForceFeedback : 적대 캐릭터에게 스킬로 공격 받음
					KeyControl::StartForceFeedback(3000, 3000, 1000);
				}
				else
				{
					// ForceFeedback : 적대 캐릭터에게 일반 공격 받음
					KeyControl::StartForceFeedback(1500, 1500, 500);
				}
			}

		}

		g_CharacterData.RegistAttacked( ulAttCharID, ulDefCharID, pAtType.m_wType );

		// 스킬 2차 이펙트를 붙인다.
		if ( pDefCreature && ( pAtType.m_wType & Skill::SKILL_MASK ) && ( pAtType.m_cSkillLevel || pAtType.m_cSkillLockCount ) )
		{
			RYLCreature* pAttackCreature = RYLCreatureManager::Instance()->GetCreature( ulAttCharID );

			if ( pAttackCreature )
			{
				if ( pDefCreature == pAttackCreature )	// 마법을 쏜사람이 자신일때
				{
					GameProcess::SetHitEffect( pAtType, pDefCreature, pAttackCreature, 0 );
				}
				else
				{
					GameProcess::SetHitEffect( pAtType, pDefCreature, pAttackCreature, 1 );
				}
			}
			else
			{
				GameProcess::SetHitEffect( pAtType, pDefCreature, NULL, 1 );
			}
		}

		if ( usHp > 0 )
		{
			if ( pDefData ) 
			{
				pDefData->m_lCurrHP = usHp;
			}

			if ( ulDefCharID == CRYLNetworkData::Instance()->m_dwMyChrID ) 
			{
				pGame->m_csStatus.m_Info.HP = usHp;
			}
		}
		else if ( usJudge != ClientConstants::Judge_NonShow )
		{
			if ( pDefCreature ) 
			{
				pDefData->m_lCurrHP = 0;
			}

			if ( ulDefCharID == CRYLNetworkData::Instance()->m_dwMyChrID )
			{
				pGame->m_csStatus.m_Info.HP = 0;

				// 리콜창이 떠 있던 상태에서 죽었을 경우 창이 사라지면서, 설정되어 있던 변수도 초기화 해준다.
				pGame->m_dwAuthorizeResult = 0;
				pGame->m_dwAuthorizeCasterID = 0;
				pGame->m_cAuthorizeCmd = 0;

				g_CharacterData.InitGlobalAction();

				char strText[ MAX_PATH ];

				unsigned long dwCreatureType = CRYLGameData::GetClientIdentity( ulAttCharID );

				CharDataNode* pAttData = RYLCharacterDataManager::Instance()->GetCharData( ulAttCharID );
				if ( pAttData && pAttData->m_strName.c_str() )
				{
					if ( usJudge == ClientConstants::Judge_Poisoned )
					{
						// 독 데미지로 죽은 경우
						pChat->AddMessage( CRYLStringTable::m_strString[2772], CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
					}
					else if ( usJudge == ClientConstants::Judge_Fired )
					{
						// 화상 데미지로 죽은 경우
						pChat->AddMessage( CRYLStringTable::m_strString[4230], CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
					}
					else
					{		
						if ( dwCreatureType == Creature::CT_PC )
						{
/*
							// 상대 국적 캐릭터의 이름은 클래스 이름으로 대체한다.
							if (pGame->m_cAccountNation != pAttData->m_cNation &&
								!CRYLNetworkData::Instance()->UseContents(GameRYL::SHOW_ENEMYNAME))
							{
								sprintf( strText, CRYLStringTable::m_strString[381], CRYLStringTable::GetJobName(pAttData->m_usClass) );
							}
							else
							{
								sprintf( strText, CRYLStringTable::m_strString[381], pAttData->m_strName.c_str() );
							}
*/
							sprintf( strText, CRYLStringTable::m_strString[381], pAttData->m_strName.c_str() );
						} 
						else
						{
							sprintf( strText, CRYLStringTable::m_strString[382], pAttData->m_strName.c_str() );
						}

						pChat->AddMessage( strText, CRYLNetworkData::Instance()->m_dwMyChrID, 129 );
					}
				} 
				else
				{
					if ( usJudge == ClientConstants::Judge_Poisoned )
					{
						// 독 데미지로 죽은 경우
						pChat->AddMessage( CRYLStringTable::m_strString[2772], CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
					}
					else if ( usJudge == ClientConstants::Judge_Fired ) 
					{
						// 화상 데미지로 죽은 경우
						pChat->AddMessage( CRYLStringTable::m_strString[4230], CRYLNetworkData::Instance()->m_dwMyChrID, 129 ) ;
					}
					else
					{
						if ( dwCreatureType == Creature::CT_PC )
						{
							pChat->AddMessage( CRYLStringTable::m_strString[383], CRYLNetworkData::Instance()->m_dwMyChrID, 129 );
						}
						else if ( dwCreatureType == Creature::CT_SIEGE_OBJECT )
						{
							pChat->AddMessage( CRYLStringTable::m_strString[2799], CRYLNetworkData::Instance()->m_dwMyChrID, 129 );
						}
					}
				}

				pSelfCreature->Dead();

				pGame->m_lpInterface->HideInterfaceforDead();
				pGame->m_csStatus.ChantDisable();
				CRYLGameData::SetAutoRun( false );
				g_CharacterData.m_bAutoFollowMode = false;
				g_CharacterData.m_dwAutoFollowChrID = 0xFFFFFFFF;

				CRYLNetworkData::SendMovingPacket( pSelfCreature->GetUpperActionID(), pSelfCreature->GetLowerActionID(), 
					pSelfCreature->GetDirection(), *(pSelfCreature->GetPosition()) );

				// 길드 요새에서 자재를 올려 놓는 요새 창이 열려있다면 모두 닫아준다.
				// 변형, 업그레이드, 월드웨폰 장전
				CRYLGameScene*	 pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );
				CRYLCampChangeDlg* pChangeDlg = ( CRYLCampChangeDlg* )pScene->GetCampChangeDlg();
				CRYLCampUpgradeDlg* pUpgradeDlg = ( CRYLCampUpgradeDlg* )pScene->GetCampUpgradeDlg();
				CRYLWorldWeaponLoadDlg* pLoadDlg = ( CRYLWorldWeaponLoadDlg* )pScene->GetWorldWeaponLoadDlg();

				if ( pChangeDlg && pChangeDlg->GetVisible() )
				{
					pChangeDlg->SetVisible( FALSE ) ;
				}

				if ( pUpgradeDlg && pUpgradeDlg->GetVisible() )
				{
					pUpgradeDlg->SetVisible( FALSE ) ;
				}

				if ( pLoadDlg && pLoadDlg->GetVisible() )
				{
					pLoadDlg->SetVisible( FALSE ) ;
				}

				if (/*SERVER_ID::STONE_WAR1*/SERVER_ID::CAPITAL <= CRYLNetworkData::Instance()->m_dwPresentZone && CRYLNetworkData::Instance()->m_dwPresentZone <= SERVER_ID::STONE_WAR3)
				{
					pGame->SetRespawnCounter(60000);
				}
			} 
			else
			{
				if ( pDefCreature )
				{
					// 성문은 죽지 않는다.
					if ( Siege::GATE != pDefCreature->GetObjectType() )
					{
						pDefCreature->Dead();
					}
				}
			}
		}

		if ( pDefData ) 
		{
			pDefData->m_lCurrMP = usMp;
		}

		if ( ulDefCharID == CRYLNetworkData::Instance()->m_dwMyChrID )
		{ 
			pGame->m_csStatus.m_Info.MP = usMp;			
		}

		return true;
	}

	return false;
}

//--------------------------------------------------------------------------------------------------------------------------
// GAME_EVENT_FUNC(ParseCharAttackInfo)
GAME_EVENT_FUNC(ParseCharAttackInfo)
{
	unsigned long	ulCharID;
	unsigned char	cDefenserNum;
	DefenserNode*	pDefNode;
	AtType			pAtType;
	CRYLGameData*	pGame = CRYLGameData::Instance() ;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharAttackInfo( lpPktBase, &ulCharID, 
		&pAtType, &cDefenserNum, &pDefNode );

	if ( CRYLNetworkData::Instance()->m_dwNetworkError == 0 )
	{
		if ( 0 == cDefenserNum )
		{
			return true;
		}

		if ( pDefNode[ 0 ].m_cJudge != ClientConstants::Judge_Poisoned && pDefNode[ 0 ].m_cJudge != ClientConstants::Judge_Fired && 
			ulCharID == CRYLNetworkData::Instance()->m_dwMyChrID ) 
		{
			if ( pDefNode[ 0 ].m_cJudge != ClientConstants::Judge_FireCracker &&
				pDefNode[ 0 ].m_cJudge != ClientConstants::Judge_ChinaFireCracker &&
				pDefNode[ 0 ].m_cJudge != ClientConstants::Judge_Social &&
				pDefNode[ 0 ].m_cJudge != ClientConstants::Judge_Gather	)
			{
				// 나에게 온 거고, 폭죽이 아니면 리턴해버리는데, 그래도 MiniPartyDlg에는 표시해줬으면 한다.
				for ( int i = 0 ; i < cDefenserNum ; ++i )
				{
					pGame->m_csStatus.AttackPartyPlayer(pDefNode[ i ], pAtType);
				}

				return true;
			}
		}

		RYLCreature*    pCreature = RYLCreatureManager::Instance()->GetCreature( ulCharID );
		CharDataNode*   pData = RYLCharacterDataManager::Instance()->GetCharData( ulCharID );

		if (NULL != pCreature)
		{
			// GM 캐릭터가 공격했는지 체크
			if (pCreature->GetGMModelFlag())
			{
				CEffScript* pInputEffect;
				pInputEffect = new CEffScript;

				pInputEffect->GetScriptBinData("adminattack.esf");
				pInputEffect->SetStartPos(pCreature->GetPosition()->x, pCreature->GetPosition()->y, pCreature->GetPosition()->z);
				pInputEffect->SetEndPos(pCreature->GetPosition()->x, pCreature->GetPosition()->y, pCreature->GetPosition()->z);
				pInputEffect->SetPlayer(pCreature, 0);

				pInputEffect->SetChr(pCreature->GetCharModel(), pCreature->GetCharModel());

				float fChrDir = pCreature->GetDirection();
				fChrDir = -fChrDir;
				fChrDir -= FLOAT_PHI / 2.0f;

				vector3 vecChrToward;
				vecChrToward.x = cosf(fChrDir);
				vecChrToward.z = sinf(fChrDir);	
				vecChrToward.y = 0.0f;
				vecChrToward.Normalize();
				pInputEffect->SetChrFwd(-vecChrToward.x, -vecChrToward.y, -vecChrToward.z);

				pInputEffect->SetMine( true );
				CSceneManager::m_EffectManager.AddEffScript( pInputEffect );
			}
		}

		// 몬스터가 공격한 경우, 몬스터의 동작을 공격 동작으로 변경 ( 독 공격의 경우 제외 )
		if ( CRYLGameData::GetClientIdentity( ulCharID ) == Creature::CT_MONSTER &&
			ClientConstants::Judge_Poisoned != pDefNode[ 0 ].m_cJudge && ClientConstants::Judge_Fired != pDefNode[ 0 ].m_cJudge )
		{
			if ( pCreature && pData )
			{
				pData->m_ulTargetCharID = pDefNode[ 0 ].m_dwCharID;

				if ( ( pAtType.m_wType & AtType::SKILL_BIT ) == 0 ) 
					pCreature->Attack( CA_ATTACK, CA_ATTACK );
			}
		}

		if ( cDefenserNum )
		{   
			for ( int i = 0 ; i < cDefenserNum ; ++i )
			{
				if ( pDefNode[ i ].m_cJudge != ClientConstants::Judge_FireCracker &&
					pDefNode[ i ].m_cJudge != ClientConstants::Judge_ChinaFireCracker &&
					pDefNode[ i ].m_cJudge != ClientConstants::Judge_Social &&
					pDefNode[ i ].m_cJudge != ClientConstants::Judge_Gather )
				{
					pGame->m_csStatus.AttackPartyPlayer( pDefNode[ i ], pAtType );
				}

				// 나한테 오는 Info는 걸러낸다.
				if ( pDefNode[ i ].m_dwCharID == CRYLNetworkData::Instance()->m_dwMyChrID ) 
				{
					continue;
				}

				RYLCreature*    pTempCreature = RYLCreatureManager::Instance()->GetCreature( pDefNode[ i ].m_dwCharID );
				CharDataNode*   pTempData = RYLCharacterDataManager::Instance()->GetCharData( pDefNode[ i ].m_dwCharID );

				if ( pTempCreature && pTempData )
				{
					long lSavedHp = pTempData->m_lCurrHP;
					long lSavedMp = pTempData->m_lCurrMP;
					pTempData->m_ulMaxHP = pDefNode[i].m_wMaxHP;
					pTempData->m_lCurrHP = pDefNode[i].m_sCurrHP;
					pTempData->m_ulMaxMP = pDefNode[i].m_wMaxMP;
					pTempData->m_lCurrMP = pDefNode[i].m_sCurrMP;
					//					pTempData->m_sDamage = Node[i].m_wDamage;

					if ( pTempCreature->GetCharID() & Creature::MONSTER_BIT )
					{
						const CMonsterMgr::MonsterProtoType *pMonster = g_MonsterMgr.GetMonsterProtoType( pTempCreature->GetCharID() & 0x0000FFFF );

						if ( pMonster && pMonster->m_MonsterInfo.m_cSkillPattern == MonsterInfo::MonsterPattern::PATTERN_STRUCTURE )
						{
							float fNowHp;
							if ( pTempData->m_ulMaxHP ) 
							{
								fNowHp = ( float )pTempData->m_lCurrHP / ( float )pTempData->m_ulMaxHP; 
							}
							else
							{
								fNowHp = 0.0f;
							}

							if ( fNowHp < 0.34f )
							{
								if ( pTempCreature->GetLowerActionID() == CA_WAITH1 ) 
								{
									pTempCreature->SetAction( CA_WAITH2, CA_WAITH2 );
								}
							} 
							else if ( fNowHp < 0.67f )
							{
								if ( pTempCreature->GetUpperActionID() == CA_WAITH1 ) 
									pTempCreature->SetAction( CA_WAITH1, CA_WAITH1 );
							}
						}
					}

					RYLDamageNode* pTempDamage  = new RYLDamageNode;      
					pTempDamage->sDamageLifeTime= 1500;
					pTempDamage->lDamageValue   = pDefNode[ i ].m_wDamage;
					pTempDamage->usDamageType   = pDefNode[ i ].m_cJudge;
					pTempDamage->fScale         = 0.5f;

					switch ( pTempDamage->usDamageType )
					{
					case ClientConstants::Judge_ManaShell:
						{
							pTempCreature->GetEffect()->AddBodyEffect( "Chant_ManaShell.esf", (unsigned long)0, true );
							pTempDamage->usDamageType = ClientConstants::Judge_Guard;
							pTempCreature->GetDamageEffect()->AddDamage( pTempDamage );
						}
						break;

					case ClientConstants::Judge_ManaHeal:
						{
							//////////////////////////////////////////////////////////////////////////////////////////////////////////////
							//////////////////////////////////////////////////////////////////////////////////////////////////////////////
							// By Belldandy (2005.02.15)
							if (pTempData->m_lCurrHP - lSavedHp)
							{
								pTempDamage->dwAlign			= RYLDamageNode::DN_Right;
								pTempCreature->GetDamageEffect()->AddDamage( pTempDamage );

								pTempDamage						= new RYLDamageNode;
								pTempDamage->sDamageLifeTime	= 1500;
								pTempDamage->lDamageValue		= pTempData->m_lCurrHP - lSavedHp;
								if (pTempDamage->lDamageValue > 0)
									pTempDamage->usDamageType	= ClientConstants::Judge_Heal;
								else
									pTempDamage->usDamageType	= ClientConstants::Judge_Front;
								pTempDamage->fScale		        = 0.5f;
								pTempDamage->dwAlign			= RYLDamageNode::DN_Left;
							}
							//////////////////////////////////////////////////////////////////////////////////////////////////////////////
							//////////////////////////////////////////////////////////////////////////////////////////////////////////////
							pTempCreature->GetDamageEffect()->AddDamage( pTempDamage );
						}
						break;

					case ClientConstants::Judge_Heal:
						{
							//////////////////////////////////////////////////////////////////////////////////////////////////////////////
							//////////////////////////////////////////////////////////////////////////////////////////////////////////////
							// By Belldandy (2005.02.15)
							if (pTempData->m_lCurrMP - lSavedMp > 0)
							{
								pTempDamage->dwAlign			= RYLDamageNode::DN_Left;
								pTempCreature->GetDamageEffect()->AddDamage( pTempDamage );

								pTempDamage						= new RYLDamageNode;
								pTempDamage->sDamageLifeTime	= 1500;
								pTempDamage->lDamageValue		= pTempData->m_lCurrMP - lSavedMp;
								pTempDamage->usDamageType		= ClientConstants::Judge_ManaHeal;
								pTempDamage->fScale		        = 0.5f;
								pTempDamage->dwAlign			= RYLDamageNode::DN_Right;
							}
							else if (pDefNode[ i ].m_wMPHeal > 0)
							{
								pTempDamage->dwAlign			= RYLDamageNode::DN_Left;
								pTempCreature->GetDamageEffect()->AddDamage( pTempDamage );

								pTempDamage						= new RYLDamageNode;
								pTempDamage->sDamageLifeTime	= 1500;
								pTempDamage->lDamageValue		= pDefNode[ i ].m_wMPHeal;
								pTempDamage->usDamageType		= ClientConstants::Judge_ManaHeal;
								pTempDamage->fScale		        = 0.5f;
								pTempDamage->dwAlign			= RYLDamageNode::DN_Right;
							}
							//////////////////////////////////////////////////////////////////////////////////////////////////////////////
							//////////////////////////////////////////////////////////////////////////////////////////////////////////////
							pTempCreature->GetDamageEffect()->AddDamage( pTempDamage );
						}
						break;

					case ClientConstants::Judge_Gather:
					case ClientConstants::Judge_Social:
					case ClientConstants::Judge_Chant:
					case ClientConstants::Judge_Disenchant:
					case ClientConstants::Judge_NonShow:
						{
							GM_DELETE( pTempDamage ) ;
						}
						break;

					case ClientConstants::Judge_Enchant:
						{
							if ( pTempDamage->lDamageValue > 0 )
							{
								pTempCreature->GetDamageEffect()->AddDamage( pTempDamage );
							}
							else
							{
								GM_DELETE( pTempDamage ) ;
							}
						}
						break;

					case ClientConstants::Judge_FireCracker:
						{
							CRYLNetworkData::Instance()->ChristmasParticle( ulCharID, pAtType.m_wType, pAtType.m_cSkillLockCount ) ;
							GM_DELETE( pTempDamage ) ;
						}
						break;

					case ClientConstants::Judge_ChinaFireCracker:
						{
							char Temp[MAX_PATH];
							int nLevel[3] = { 100, 300, 500 };
							sprintf(Temp, "%s\\Sound\\ChinaCracker_%d.wav", g_ClientMain.m_strClientPath, nLevel[pAtType.m_cSkillLockCount]);
							CSound* pSound; 
							CSoundMgr::_GetInstance()->GetSound( pSound, Temp ); 
							pSound->Play(); 

							GM_DELETE( pTempDamage ) ;
						}
						break;

					case ClientConstants::Judge_Evade:
						{
							pTempCreature->GetDamageEffect()->AddDamage( pTempDamage );
						}
						break;

					case ClientConstants::Judge_Resist:
						{
							//////////////////////////////////////////////////////////////////////////////////////////////////////////////
							//////////////////////////////////////////////////////////////////////////////////////////////////////////////
							// By Belldandy (2005.02.15)
							if (pTempData->m_lCurrHP - lSavedHp)
							{
								pTempDamage->dwAlign		= RYLDamageNode::DN_Right;
								pTempCreature->GetDamageEffect()->AddDamage( pTempDamage );

								pTempDamage = new RYLDamageNode;
								pTempDamage->lDamageValue = pTempData->m_lCurrHP - lSavedHp;
								if (pTempDamage->lDamageValue > 0)
									pTempDamage->usDamageType	= ClientConstants::Judge_Heal;
								else
									pTempDamage->usDamageType	= ClientConstants::Judge_Front;
								pTempDamage->sDamageLifeTime= 1500;
								pTempDamage->fScale         = 0.5f;
								pTempDamage->dwAlign		= RYLDamageNode::DN_Left;
							}
							//////////////////////////////////////////////////////////////////////////////////////////////////////////////
							//////////////////////////////////////////////////////////////////////////////////////////////////////////////
							pTempCreature->GetDamageEffect()->AddDamage( pTempDamage );
						}
						break;

					default:
						{
							g_CharacterData.RegistAttacked( ulCharID, pDefNode[ i ].m_dwCharID, pAtType.m_wType );

							pTempCreature->GetDamageEffect()->AddDamage( pTempDamage );
						}
						break;
					}

					// 스킬 2차 이펙트를 붙인다.
					if ( ( pAtType.m_wType & Skill::SKILL_MASK ) && ( pAtType.m_cSkillLevel || pAtType.m_cSkillLockCount ) )
					{
						if (pAtType.m_wType != 0x8708 || i == 0)
						{
							RYLCreature* pAttCreature = RYLCreatureManager::Instance()->GetCreature( ulCharID );

							if ( pAttCreature ) 
							{
								GameProcess::SetHitEffect( pAtType, pTempCreature, pAttCreature, i );
							}
							else 
							{
								GameProcess::SetHitEffect( pAtType, pTempCreature, NULL, i );
							}
						}
					}

					if (pDefNode[i].m_sCurrHP <= 0 && pDefNode[i].m_cJudge != ClientConstants::Judge_NonShow)
					{
						pTempData->m_ulMaxHP = 0;
						pTempData->m_lCurrHP = -1;
						pTempData->m_ulMaxMP = 0;
						pTempData->m_lCurrMP = -1;

						if ( CRYLGameData::GetClientIdentity( pTempCreature->GetCharID() ) == Creature::CT_PC )
						{
							pTempCreature->Dead();

							char    pstrText[MAX_PATH];
							char*   pstrName;

							// 상대 국적 캐릭터의 이름은 클래스 이름으로 대체한다.
							if (pGame->m_cAccountNation != pTempData->m_cNation &&
								!CRYLNetworkData::Instance()->UseContents(GameRYL::SHOW_ENEMYNAME))
							{
								pstrName = const_cast<char*>( pTempData->m_strName.c_str() );
//								pstrName = CRYLStringTable::GetJobName( pTempData->m_usClass ) ;
							}
							else
							{
								pstrName = const_cast<char*>( pTempData->m_strName.c_str() );
							}

							// 파티 멤버인 경우에는 다른 방법으로 사망 메시지가 간다. 따라서 처리할 필요 없다.
							if ( pstrName && !pGame->m_csStatus.IsPartyMember(pTempCreature->GetCharID()))
							{
								const char*		pstrKillerName = ( pData ) ? pData->m_strName.c_str() : NULL;
								unsigned long   ulKillerType = CRYLGameData::GetClientIdentity( ulCharID );

								char pstrTemp[50];

								// 독 데미지로 죽은 경우
								if ( ClientConstants::Judge_Poisoned == pDefNode[i].m_cJudge )
								{
									sprintf( pstrText, CRYLStringTable::m_strString[ 2773 ], pstrName );
								}
								else if ( ClientConstants::Judge_Fired == pDefNode[i].m_cJudge )
								{
									sprintf( pstrText, CRYLStringTable::m_strString[ 4230 ], pstrName );
								}
								else
								{
									if ( ulKillerType == Creature::CT_PC )
									{
										if ( pTempCreature->GetCharID() != ulCharID )
										{
											if ( pstrKillerName )
											{
/*
												// 상대 국적 캐릭터의 이름은 클래스 이름으로 대체한다.
												if (pGame->m_cAccountNation != pData->m_cNation &&
													!CRYLNetworkData::Instance()->UseContents(GameRYL::SHOW_ENEMYNAME))
												{
													strcpy( pstrTemp, CRYLStringTable::GetJobName(pData->m_usClass) );
												}
												else
												{
													strcpy( pstrTemp, pstrKillerName );
												}
*/
												strcpy( pstrTemp, pstrKillerName );

												strcat( pstrTemp, CRYLStringTable::m_strString[ 374 ] );
											} 
											else
											{
												strcpy( pstrTemp, CRYLStringTable::m_strString[ 375 ] );
											}

											sprintf( pstrText, CRYLStringTable::m_strString[ 376 ], pstrTemp, pstrName );
										} 
										else
										{
											if ( pstrKillerName )
											{
/*
												// 상대 국적 캐릭터의 이름은 클래스 이름으로 대체한다.
												if (pGame->m_cAccountNation != pData->m_cNation &&
													!CRYLNetworkData::Instance()->UseContents(GameRYL::SHOW_ENEMYNAME))
												{
													strcpy( pstrTemp, CRYLStringTable::GetJobName(pData->m_usClass) );
												}
												else
												{
													strcpy( pstrTemp, pstrKillerName );
												}
*/
												strcpy( pstrTemp, pstrKillerName );

												strcat( pstrTemp, CRYLStringTable::m_strString[ 374 ] );
											} 
											else
											{
												strcpy( pstrTemp, CRYLStringTable::m_strString[ 375 ] );
											}

											sprintf( pstrText, CRYLStringTable::m_strString[ 377 ], pstrTemp );
										}
									} 
									else
									{
										if ( pstrKillerName )
										{
											strcpy( pstrTemp, pstrKillerName );
											strcat( pstrTemp, CRYLStringTable::m_strString[ 378 ] );
										}
										else
										{
											strcpy( pstrTemp, CRYLStringTable::m_strString[ 379 ] );
										}

										sprintf( pstrText, CRYLStringTable::m_strString[ 380 ], pstrName, pstrTemp );
									}
								}

								if ( CRYLGameData::GetClientIdentity( pTempCreature->GetCharID() ) == Creature::CT_PC && 
									CRYLGameData::GetClientEnemyType( pTempCreature->GetCharID() ) == EnemyCheck::EC_FRIEND )
								{
									CRYLChattingDlg::Instance()->AddMessage( pstrText, CRYLNetworkData::Instance()->m_dwMyChrID, 129 );
								} 
								else
								{
									CRYLChattingDlg::Instance()->AddMessage( pstrText, CRYLNetworkData::Instance()->m_dwMyChrID, 128 );
								}
							}
						}
						else
						{
							unsigned long ulMonsterID = pTempCreature->GetCharID();
							const CMonsterMgr::MonsterProtoType *pMonster = g_MonsterMgr.GetMonsterProtoType( ulMonsterID & 0x0000FFFF );

							// 석상은 Dead 함수를 호출하지 않고, 바로 삭제한다.
							if ( pMonster && pMonster->m_MonsterInfo.m_cSkillPattern == MonsterInfo::MonsterPattern::PATTERN_STRUCTURE )
							{
								GameProcess::LogCharUnload(pTempCreature->GetCharID(), LOG_RFL);
								RYLCreatureManager::Instance()->DeleteCreature( pTempCreature->GetCharID() );
							}
							else
							{
								// 성문은 죽지 않는다.
								if ( Siege::GATE != pTempCreature->GetObjectType() )
								{
									pTempCreature->Dead();
								}
							}
						}
					}
				}
			}
		}
	}

	return false;
}

//--------------------------------------------------------------------------------------------------------------------------
// GAME_EVENT_FUNC(ParseCharEquipDurability) 
GAME_EVENT_FUNC(ParseCharEquipDurability) 
{
	unsigned long	ulCharID;
	unsigned char	ucIndex, ucValue, ucValueMax;
	CRYLGameData*	pGame = CRYLGameData::Instance();

	if(!pGame)
		return true;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharEquipDurability( lpPktBase,
		&ulCharID,
		&ucIndex,
		&ucValue,
		&ucValueMax );

	CRYLChattingDlg* pChat = CRYLChattingDlg::Instance();

	if(!pChat)
		return true;

	// edith 2008.01.16 장비의 내구도 변경. (최대내구도 감소패킷 추가)
	// 장비 내구도 달기
	if ( ucIndex < Item::EquipmentPos::MAX_EQUPMENT_POS && pGame->m_csStatus.m_lstEquipment[ ucIndex ] )
	{
		CItemInstance* pItem = pGame->m_csStatus.m_lstEquipment[ ucIndex ];
		if(!pItem)
			return true;
		
		pItem->SetNowDurability( ucValue );

		// 최대 내구도가 0보다 크면 최대내구도의 설정이 변경되었음을 말한다.
		if ( ucValueMax > 0)
		{
			pItem->SetMaxDurability(ucValueMax);
		}

		if ( ucValue == 0 )
		{
			unsigned long dwType = pItem->GetItemType();

			// 볼트 계열이면 아이템을 뽀갠다.
			if ( dwType == Item::ItemType::ARROW || dwType == Item::ItemType::BOLT )
			{
				delete pGame->m_csStatus.m_lstEquipment[ ucIndex ];
				pGame->m_csStatus.m_lstEquipment[ ucIndex ] = NULL;

				switch ( dwType )
				{
				case Item::ItemType::ARROW :
					{
						pChat->AddMessage( CRYLStringTable::m_strString[356], CRYLNetworkData::Instance()->m_dwMyChrID, 129 );
					}
					break;

				case Item::ItemType::BOLT :
					{
						pChat->AddMessage( CRYLStringTable::m_strString[357], CRYLNetworkData::Instance()->m_dwMyChrID, 129 );
					}
					break;
				}
			} 
			else
			{
				pChat->AddMessage( CRYLStringTable::m_strString[271], CRYLNetworkData::Instance()->m_dwMyChrID, 129 );
			}
		}
	}

	return false;
}

//--------------------------------------------------------------------------------------------------------------------------
// GAME_EVENT_FUNC(ParseCharRespawn)
GAME_EVENT_FUNC(ParseCharRespawn)
{
	CRYLNetworkData::Instance()->SetNetworkFlag( NF_NOWAITING ) ;
	g_pClientSocket->SetStatusFlag( NS_NOWAITING );

	unsigned long	ulCharID, ulGold;
	unsigned short	usHp, usMp;
	POS				Pos;
	__int64			ddExp;
	CRYLGameData*	pGame = CRYLGameData::Instance();

	// edith 2009.06.20 부활후 리스폰 카운트 삭제 
	CRYLGameData::SetRespawnCounter( 0 ) ;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharRespawn( lpPktBase, &ulCharID, &usHp, &usMp, &Pos, &ddExp, &ulGold );

	if ( CRYLNetworkData::Instance()->m_dwNetworkError == 0 )
	{
		if ( ulCharID == CRYLNetworkData::Instance()->m_dwMyChrID )
		{
			if ( CRYLNetworkData::Instance()->m_dwPresentZone == 100 )
			{
				CRYLNetworkData::Instance()->m_dwPresentZone = CRYLNetworkData::Instance()->m_dwNextZone;
				pGame->m_lpInterface->InitZone( CRYLNetworkData::Instance()->m_dwPresentZone );

				CRYLGameScene*			 pScene      = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );
				CRYLLargeMapFrameDlg*	 pDlg		 = ( CRYLLargeMapFrameDlg* )pScene->GetLargeMapDlg();
				pDlg->LoadLargeMapTexture();

				pGame->m_dwClientState = GAME_PLAYING;
			}

			// 다시 움직일수 있게 한다.
			CRYLGameData::Instance()->SetEnableMove( TRUE ) ;

			// 레저렉션 인증창이 떠 있었다면 없애준다.
			if ( PktAP::AP_RESURRECTION == pGame->m_cAuthorizeCmd )
			{
				g_MessageBoxManager.DestroyList();

				pGame->m_dwAuthorizeResult = 0;
				pGame->m_dwAuthorizeCasterID = 0;
				pGame->m_cAuthorizeCmd = 0;
			}

			pGame->m_csStatus.m_Info.HP = usHp;
			pGame->m_csStatus.m_Info.MP = usMp;
			pGame->m_csStatus.m_Info.Exp = ddExp;
			pGame->m_csStatus.m_Info.Gold = ulGold;

			RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
			if ( !pSelfCreature )
				return true;

			CharDataNode* pSelfData = RYLCharacterDataManager::Instance()->GetCharData( pSelfCreature->GetCharID() );
			if ( !pSelfData )
				return true;

			pSelfData->m_lCurrHP = usHp;
			pSelfData->m_lCurrMP = usMp;

			RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( ulCharID );
			pTempCreature->Respawn();

			vector3 vecPos = vector3( Pos.fPointX * 100.0f, Pos.fPointY * 100.0f, Pos.fPointZ * 100.0f );
			CZ3DGeneralChrModel* pTempModel = pTempCreature->GetCharModel();
			if ( pTempCreature && pTempModel )
			{
				pTempCreature->SetPosition( vecPos );
				pTempModel->SetPosition( vecPos );
			}

			CEffScript *pRespawnEff = new CEffScript;
			CSceneManager::AddEffectScript( pRespawnEff );

			pRespawnEff->GetScriptBinData( "respawn.esf", pGame->m_bSelfEffect );
			pRespawnEff->SetStartPos( Pos.fPointX * 100.0f, Pos.fPointY * 100.0f, Pos.fPointZ * 100.0f );
			pRespawnEff->SetEndPos( ( D3DXVECTOR3 * )pSelfCreature->GetPosition(), 1 );

			// edith 2009.07.16  리스폰 할때 플라잉 값을 끈다.
			pSelfCreature->SetFlying(false);

/*
			// 죽었는데 쿨다운을 초기화 하면 안된다.
			int i;
			for ( i = 0; i < SKILL::MAX_SLOT_NUM; i++)
			{
				if (pGame->m_csStatus.m_lstSkillSlot[i])
				{
					pGame->m_csStatus.m_lstSkillSlot[i]->ClearCoolDown();
				}
			}

			for ( i = 0; i < QUICK::MAX_QUICK_NUM; i++)
			{
				if (pGame->m_csStatus.m_lstQuickSlot[i])
				{
					pGame->m_csStatus.m_lstQuickSlot[i]->ClearCoolDown();
				}
			}
*/
			CRYLGameScene* pScene = static_cast<CRYLGameScene*>(CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) );	
			CRYLLargeMapFrameDlg* pMapDlg = ( CRYLLargeMapFrameDlg* )pScene->GetLargeMapDlg() ;
			CRYLBattleGroundDlg* pBattleDlg = ( CRYLBattleGroundDlg* )pScene->GetBattleGroundDlg() ;

			if (CRYLNetworkData::Instance()->m_dwPresentZone == SERVER_ID::ZONE3 || 
				CRYLNetworkData::Instance()->m_dwPresentZone == SERVER_ID::BATTLE_ZONE)
			{
				CRYLCommunityData::Instance()->SetStoneStatueRespawn( false ) ;
				pBattleDlg->SetVisible( FALSE ) ;
				pMapDlg->HideControl() ;
			}

			pScene->GetRespawnDlg()->SetVisible( FALSE ) ;
			pScene->GetBattleWaitDlg()->SetVisible( FALSE ) ;

			pMapDlg->ClearRespawnButton() ;

			CRYLNetworkData::SendMovingPacket(CA_RESPAWN, CA_RESPAWN, pSelfCreature->GetDirection(), *(pSelfCreature->GetPosition()));
		}
		return true;
	}
	else
	{
		CRYLGameScene* pScene = static_cast<CRYLGameScene*>(CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) );	
		CRYLLargeMapFrameDlg* pMapDlg = ( CRYLLargeMapFrameDlg* )pScene->GetLargeMapDlg() ;

		pScene->GetRespawnDlg()->SetVisible( FALSE ) ;
		pScene->GetBattleWaitDlg()->SetVisible( FALSE ) ;
		pMapDlg->ClearRespawnButton() ;


		CRYLMessageBox *lpMessageBox;
		switch (CRYLNetworkData::Instance()->m_dwNetworkError)
		{
		case PktRsAck::ERR_NOT_EXIST_CAMP:
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create(CRYLStringTable::m_strString[ 3458 ]);
				break;
			}
		}
	}

	return false;
}

//--------------------------------------------------------------------------------------------------------------------------
// GAME_EVENT_FUNC(ParseCharBattleGroundRespawn)
GAME_EVENT_FUNC(ParseCharBattleGroundRespawn)
{
	unsigned long ulCharID;

	STONESTATUERESPAWNINFOEX	StoneStatueRespawn;
	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharBattleGroundRespawn( lpPktBase, &ulCharID, 
		&StoneStatueRespawn.usTurn_Out,
		&StoneStatueRespawn.usWaitNum_Out,
		&StoneStatueRespawn.usLeftTime_Out,
		&StoneStatueRespawn.usHumanNum_Out,
		&StoneStatueRespawn.usAkhanNum_Out );

	if ( 0 == CRYLNetworkData::Instance()->m_dwNetworkError )
	{
		CRYLGameScene*		 pScene     = (CRYLGameScene*)CRYLSceneManager::Instance()->FindScene( GAME_SCENE );
		CRYLBattleGroundDlg* pBattleDlg = (CRYLBattleGroundDlg*)pScene->GetBattleGroundDlg();

		pBattleDlg->SetStoneStatueRespawnInfo( &StoneStatueRespawn );
		pBattleDlg->SetVisible( TRUE );
		return true;
	} 
	else
	{
		CRYLMessageBox *lpMessageBox;
		switch ( CRYLNetworkData::Instance()->m_dwNetworkError )
		{
		case 2:
			{
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->Create( CRYLStringTable::m_strString[1581] );

				CRYLGameScene*          pScene      = (CRYLGameScene*)CRYLSceneManager::Instance()->FindScene( GAME_SCENE );
				CRYLBattleGroundDlg*    pBattleDlg  = (CRYLBattleGroundDlg*)pScene->GetBattleGroundDlg();
				pBattleDlg->SetRespawnWait( FALSE );
			}
			break;
		}
	}

	return false;
}

//--------------------------------------------------------------------------------------------------------------------------
// GAME_EVENT_FUNC(ParseCharRespawnWaitQueue)
GAME_EVENT_FUNC(ParseCharRespawnWaitQueue)
{
	unsigned long ulCharID;
	unsigned short usWaitNum_Out[ 3 ];
	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharRespawnWaitQueue( lpPktBase, &ulCharID, usWaitNum_Out );

	if ( 0 == CRYLNetworkData::Instance()->m_dwNetworkError )
	{
		CRYLGameScene*			pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE );
		CRYLLargeMapFrameDlg*	pMapDlg = ( CRYLLargeMapFrameDlg* )pScene->GetLargeMapDlg();
		CRYLBattleGroundDlg*	pBattleDlg = ( CRYLBattleGroundDlg* )pScene->GetBattleGroundDlg();

		pMapDlg->ShowLargeMap( *usWaitNum_Out );
		pMapDlg->SetVisible( TRUE );
		return true;
	}

	return false;
}

//--------------------------------------------------------------------------------------------------------------------------
// GAME_EVENT_FUNC(ParseCharDuelCmd)
GAME_EVENT_FUNC(ParseCharDuelCmd)
{
	unsigned long	ulSendID, ulRecvID;
	unsigned char	ucCmd;
	CRYLGameData*	    pGame = CRYLGameData::Instance();
	CRYLChattingDlg*    pChat = CRYLChattingDlg::Instance();

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharDuelCmd( lpPktBase, &ulSendID, &ulRecvID, &ucCmd );

	if ( 0 == CRYLNetworkData::Instance()->m_dwNetworkError )
	{
		switch ( ucCmd )
		{
		case PktDuC::DUC_PROPOSE:
			{
				if ( pGame->m_lpInterface->GetBusy() )
				{
					SendPacket::CharDuelCmd( lpHandler, CRYLNetworkData::Instance()->m_dwMyChrID, ulSendID, PktDuC::DUC_REFUSE );
				}
				else if ( pGame->m_lpInterface )
				{
					pGame->m_lpInterface->m_dwDuelCmdResult   = 0;
					pGame->m_lpInterface->m_dwDuelCmdType     = PktDuC::DUC_PROPOSE;
					pGame->m_lpInterface->m_dwDuelCmdSenderID = ulSendID;

					char pstrText[ 200 ];

					CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( ulSendID );

					if ( pTempData )
					{
/*
						// 상대 국적 캐릭터의 이름은 클래스 이름으로 대체한다.
						if (0 != pTempData->m_cNation && pGame->m_cAccountNation != pTempData->m_cNation &&
							!CRYLNetworkData::Instance()->UseContents(GameRYL::SHOW_ENEMYNAME))
						{
							sprintf( pstrText, CRYLStringTable::m_strString[359], pGame->GetClassName( pTempData->m_usClass ),
								pTempData->m_cLevel, pGame->GetClassName( pTempData->m_usClass ) );
						}
						else
						{
*/							sprintf( pstrText, CRYLStringTable::m_strString[359], pTempData->m_strName.c_str(),
								pTempData->m_cLevel, pGame->GetClassName( pTempData->m_usClass ) );
//						}

						CRYLMessageBox *lpMessageBox;
						lpMessageBox = new CRYLMessageBox;
						lpMessageBox->Create( pstrText, MB_EXIT | MB_YES | MB_NO );
						lpMessageBox->SetResult( &pGame->m_lpInterface->m_dwDuelCmdResult );
						lpMessageBox->SetEnableEnter( false );
						lpMessageBox->SetModal(FALSE);
					}
				}
				break;
			}

		case PktDuC::DUC_REFUSE:
			{
				GRYLCommunityDataInstance()->AddRefuseList( ulSendID );

				char pstrText[ 200 ];

				CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( ulSendID );

				if ( pTempData )
				{
/*					// 상대 국적 캐릭터의 이름은 클래스 이름으로 대체한다.
					if (0 != pTempData->m_cNation && pGame->m_cAccountNation != pTempData->m_cNation &&
						!CRYLNetworkData::Instance()->UseContents(GameRYL::SHOW_ENEMYNAME))
					{
						sprintf( pstrText, CRYLStringTable::m_strString[ 360 ], pGame->GetClassName( pTempData->m_usClass ) );
					}
					else
					{
*/						sprintf( pstrText, CRYLStringTable::m_strString[ 360 ], pTempData->m_strName.c_str() );
//					}

					pChat->AddMessage( pstrText, CRYLNetworkData::Instance()->m_dwMyChrID, 129 );
				}
				break;
			}

		case PktDuC::DUC_READY:
			// 듀얼 시작
			{
				g_CharacterData.InitDuelTarget();

				if ( ulSendID == CRYLNetworkData::Instance()->m_dwMyChrID )
				{
					g_CharacterData.SetPreDuelTarget( ulRecvID, BattleInclination::VS_CHARACTER );
				}
				else
				{
					g_CharacterData.SetPreDuelTarget( ulSendID, BattleInclination::VS_CHARACTER );
				}

				if ( g_CharacterData.m_lpDuelReady )
				{
					CSceneManager::m_EffectManager.DeleteInterfaceScript( g_CharacterData.m_lpDuelReady );
					g_CharacterData.m_lpDuelReady = NULL;
				}

				g_CharacterData.m_lpDuelReady = new CEffScript;
				g_CharacterData.m_lpDuelReady->SetInterfaceSet( true );
				g_CharacterData.m_lpDuelReady->GetScriptBinData( "inter_duel.esf" );	

				float fX = g_CharacterData.m_lpDuelReady->m_SubScript[0].m_InterfacePos[0] / 800.0f;
				float fY = g_CharacterData.m_lpDuelReady->m_SubScript[0].m_InterfacePos[1] / 600.0f;

				g_CharacterData.m_lpDuelReady->m_SubScript[0].m_InterfacePos[0] = fX * g_ClientMain.m_iScreenWidth;
				g_CharacterData.m_lpDuelReady->m_SubScript[0].m_InterfacePos[1] = fY * g_ClientMain.m_iScreenHeight;				

				fX = g_CharacterData.m_lpDuelReady->m_SubScript[1].m_InterfacePos[0] / 800.0f;
				fY = g_CharacterData.m_lpDuelReady->m_SubScript[1].m_InterfacePos[1] / 600.0f;
				g_CharacterData.m_lpDuelReady->m_SubScript[1].m_InterfacePos[0] = fX * g_ClientMain.m_iScreenWidth;
				g_CharacterData.m_lpDuelReady->m_SubScript[1].m_InterfacePos[1] = fY * g_ClientMain.m_iScreenHeight;				

				CSceneManager::m_EffectManager.AddInterfaceScript( g_CharacterData.m_lpDuelReady );
				break;
			}

		case PktDuC::DUC_LOGOUT:
			{
				if ( ulSendID == CRYLNetworkData::Instance()->m_dwMyChrID )
				{
					g_CharacterData.InitDuelTarget();
				}
				else
				{
					g_CharacterData.DeleteDuelTarget( ulSendID );
				}

				char pstrText[ 200 ];

				CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( ulSendID );

				if ( pTempData )
				{               
/*
					// 상대 국적 캐릭터의 이름은 클래스 이름으로 대체한다.
					if (0 != pTempData->m_cNation && pGame->m_cAccountNation != pTempData->m_cNation &&
						!CRYLNetworkData::Instance()->UseContents(GameRYL::SHOW_ENEMYNAME))
					{
						sprintf( pstrText, CRYLStringTable::m_strString[ 361 ], pGame->GetClassName( pTempData->m_usClass ) );
					}
					else
					{
*/						sprintf( pstrText, CRYLStringTable::m_strString[ 361 ], pTempData->m_strName.c_str() );
//					}

					pChat->AddMessage( pstrText, CRYLNetworkData::Instance()->m_dwMyChrID, 129 );
				}
				break;
			}

		case PktDuC::DUC_DEAD:
			{
				if ( ulSendID == CRYLNetworkData::Instance()->m_dwMyChrID )
				{
					g_CharacterData.InitDuelTarget();
				}
				else
				{
					g_CharacterData.DeleteDuelTarget( ulSendID );
				}

				if ( ulSendID != CRYLNetworkData::Instance()->m_dwMyChrID )
				{
					char pstrText[ 200 ];

					CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( ulSendID );

					if ( pTempData )
					{               
/*
						// 상대 국적 캐릭터의 이름은 클래스 이름으로 대체한다.
						if (0 != pTempData->m_cNation && pGame->m_cAccountNation != pTempData->m_cNation &&
							!CRYLNetworkData::Instance()->UseContents(GameRYL::SHOW_ENEMYNAME))
						{
							sprintf( pstrText, CRYLStringTable::m_strString[ 362 ], pGame->GetClassName( pTempData->m_usClass ) );
						}
						else
						{
*/							sprintf( pstrText, CRYLStringTable::m_strString[ 362 ], pTempData->m_strName.c_str() );
//						}

						pChat->AddMessage( pstrText, CRYLNetworkData::Instance()->m_dwMyChrID, 129 );
					}
				}
				break;
			}

		case PktDuC::DUC_CANCEL:
			{
				g_CharacterData.InitDuelTarget();
				break;
			}
		}

		return true;
	} 
	else
	{
		switch ( CRYLNetworkData::Instance()->m_dwNetworkError )
		{
		case 3:					// 3 = 듀얼 거부
			{
				pChat->AddMessage( CRYLStringTable::m_strString[ 368 ], CRYLNetworkData::Instance()->m_dwMyChrID, 129 );
				break;
			}
		}
	}

	return false;
}

//--------------------------------------------------------------------------------------------------------------------------
// GAME_EVENT_FUNC(ParseCharTeamBattleInfo)
GAME_EVENT_FUNC(ParseCharTeamBattleInfo)
{
	unsigned long	aryMemberID[ 10 ];//was 10 hayzohar notsure
	unsigned long   ulCharID;
	unsigned char   aryMemberLevel[ 10 ];//was 10 hayzohar notsure
	unsigned char	ucMember, ucCmd;
	char			pstrName[ 16 ];
	CRYLGameData*	pGame = CRYLGameData::Instance();

	ZeroMemory( pstrName, sizeof( pstrName ) );
	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharTeamBattleInfo( lpPktBase, &ulCharID, pstrName, &ucCmd, &ucMember, aryMemberID, aryMemberLevel );

	if ( 0 == CRYLNetworkData::Instance()->m_dwNetworkError )
	{
		if ( !pGame->m_lpInterface ) return true;

		switch ( ucCmd )
		{
		case PktDuC::DUC_REQUEST_PARTY_INFO:
			{
				pGame->m_lpInterface->m_dwPartyDuelCmdResult      = 0;
				pGame->m_lpInterface->m_dwPartyDuelCmdType        = PktDuC::DUC_REQUEST_PARTY_INFO;
				pGame->m_lpInterface->m_dwPartyDuelCmdSenderID    = ulCharID;

				char strText[ MAX_PATH ], strTemp[ MAX_PATH ];
				sprintf( strText, CRYLStringTable::m_strString[ 396 ], pstrName, ucMember, aryMemberLevel[ 0 ] );
				for ( unsigned long i = 1; i < ucMember; i++ )
				{
					sprintf( strTemp, ",%d", aryMemberLevel[ i ] );
					strcat( strText, strTemp );
				}
				strcat( strText, CRYLStringTable::m_strString[ 397 ] );

				CRYLMessageBox *lpMessageBox;
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->SetWordBreak( true );
				lpMessageBox->Create( strText, MB_EXIT | MB_YES | MB_NO );
				lpMessageBox->SetResult( &pGame->m_lpInterface->m_dwPartyDuelCmdResult );
			}
			break;

		case PktDuC::DUC_PARTY_PROPOSE:
			{
				pGame->m_lpInterface->m_dwPartyDuelCmdResult      = 0;
				pGame->m_lpInterface->m_dwPartyDuelCmdType        = PktDuC::DUC_PARTY_PROPOSE;
				pGame->m_lpInterface->m_dwPartyDuelCmdSenderID    = ulCharID;

				char strText[ MAX_PATH ], strTemp[ MAX_PATH ];
				sprintf( strText, CRYLStringTable::m_strString[ 396 ], pstrName, ucMember, aryMemberLevel[ 0 ] );
				for ( unsigned long i = 1; i < ucMember; i++ )
				{
					sprintf( strTemp, ",%d", aryMemberLevel[ i ] );
					strcat( strText, strTemp );
				}
				strcat( strText, CRYLStringTable::m_strString[ 398 ] );

				CRYLMessageBox *lpMessageBox;
				lpMessageBox = new CRYLMessageBox;
				lpMessageBox->SetWordBreak( true );
				lpMessageBox->Create( strText, MB_EXIT | MB_YES | MB_NO );
				lpMessageBox->SetResult( &pGame->m_lpInterface->m_dwPartyDuelCmdResult );
			}
			break;

		case PktDuC::DUC_PARTY_READY:
			// 팀 배틀 시작
			{
				g_CharacterData.InitDuelTarget();
				for ( unsigned long i = 0; i < ucMember; i++ )
				{
					g_CharacterData.SetPreDuelTarget( aryMemberID[ i ], BattleInclination::VS_PARTY );
				}

				if ( g_CharacterData.m_lpDuelReady )
				{
					CSceneManager::m_EffectManager.DeleteInterfaceScript( g_CharacterData.m_lpDuelReady );
					g_CharacterData.m_lpDuelReady = NULL;
				}

				g_CharacterData.m_lpDuelReady = new CEffScript;
				g_CharacterData.m_lpDuelReady->SetInterfaceSet( true );
				g_CharacterData.m_lpDuelReady->GetScriptBinData( "inter_duel.esf" );	

				float fX = g_CharacterData.m_lpDuelReady->m_SubScript[ 0 ].m_InterfacePos[ 0 ] /800.0f;
				float fY = g_CharacterData.m_lpDuelReady->m_SubScript[ 0 ].m_InterfacePos[ 1 ] /600.0f;

				g_CharacterData.m_lpDuelReady->m_SubScript[ 0 ].m_InterfacePos[ 0 ] = fX * g_ClientMain.m_iScreenWidth;
				g_CharacterData.m_lpDuelReady->m_SubScript[ 0 ].m_InterfacePos[ 1 ] = fY * g_ClientMain.m_iScreenHeight;				

				fX = g_CharacterData.m_lpDuelReady->m_SubScript[ 1 ].m_InterfacePos[ 0 ] /800.0f;
				fY = g_CharacterData.m_lpDuelReady->m_SubScript[ 1 ].m_InterfacePos[ 1 ] /600.0f;

				g_CharacterData.m_lpDuelReady->m_SubScript[ 1 ].m_InterfacePos[ 0 ] = fX * g_ClientMain.m_iScreenWidth;
				g_CharacterData.m_lpDuelReady->m_SubScript[ 1 ].m_InterfacePos[ 1 ] = fY * g_ClientMain.m_iScreenHeight;				

				CSceneManager::m_EffectManager.AddInterfaceScript( g_CharacterData.m_lpDuelReady );
			}
			break;
		}

		return true;
	}

	return false;
}

//--------------------------------------------------------------------------------------------------------------------------
// GAME_EVENT_FUNC( ParseCharPeaceMode )
GAME_EVENT_FUNC( ParseCharPeaceMode )
{
	unsigned long ulCharID;
	unsigned char ucHour;
	bool bPeace;
	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharPeaceMode( lpPktBase, &ulCharID, &ucHour, &bPeace );

	CRYLChattingDlg* pChat = CRYLChattingDlg::Instance() ;

	if ( 0 == CRYLNetworkData::Instance()->m_dwNetworkError )
	{
		if ( ulCharID == CRYLNetworkData::Instance()->m_dwMyChrID )
		{
			CRYLCommunityData::Instance()->m_bPeaceMode = bPeace;

			if ( ucHour )
			{
				char pstrMessage[ MAX_PATH ];
				if ( bPeace )
				{
					sprintf( pstrMessage, CRYLStringTable::m_strString[ 415 ], ucHour );
				} 
				else
				{
					sprintf( pstrMessage, CRYLStringTable::m_strString[ 416 ], ucHour );
				}
				pChat->AddMessage( pstrMessage, CRYLNetworkData::Instance()->m_dwMyChrID, 129 );
			} 
			else
			{
				RYLCreature* pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
				if ( !pSelfCreature )
					return true;

				CharDataNode*   pSelfData = RYLCharacterDataManager::Instance()->GetCharData( pSelfCreature->GetCharID() );
				if ( pSelfData )
				{
					pSelfData->m_bPeaceMode = bPeace;
				}
			}
		}

		return true;
	} 
	else
	{
		switch ( CRYLNetworkData::Instance()->m_dwNetworkError )
		{
		case 2:
			{
				pChat->AddMessage( CRYLStringTable::m_strString[ 417 ], CRYLNetworkData::Instance()->m_dwMyChrID, 129 );

				RYLCreature*    pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
				if ( !pSelfCreature )
					return false;

				CharDataNode*   pSelfData = RYLCharacterDataManager::Instance()->GetCharData( pSelfCreature->GetCharID() );
				if ( pSelfData )
				{
					pSelfData->m_bPeaceMode = !pSelfData->m_bPeaceMode;
				}
			}
			break;
		}
	}

	return false;
}

//--------------------------------------------------------------------------------------------------------------------------
// GAME_EVENT_FUNC(ParseCharRankingInfo)
GAME_EVENT_FUNC(ParseCharRankingInfo)
{
	unsigned long		ulCharID_Out ; 
	unsigned char		ucClass_Out ;
	unsigned char		ucPage_Out ;
	unsigned short		usNum_Out ;
	RankingNode*		pRankingNode_Out ;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharRankingInfo( lpPktBase, 
		&ulCharID_Out, 
		&ucClass_Out, 
		&ucPage_Out, 
		&usNum_Out, 
		&pRankingNode_Out ) ;


	if ( 0 == CRYLNetworkData::Instance()->m_dwNetworkError )
	{
		CRYLGameScene*		pScene      = (CRYLGameScene*)CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
		CRYLRankingDlg*		pDlg		= (CRYLRankingDlg*)pScene->GetRankingDlg() ;

		pDlg->InsertRanking( ucClass_Out, ucPage_Out, usNum_Out, pRankingNode_Out ) ;
		pDlg->InsertDataListbox( ucClass_Out ) ;
		pDlg->SetVisible( TRUE ) ;

		return true;
	}
/*	else
	{
		CRYLChattingDlg* pChat = CRYLChattingDlg::Instance() ;
	
		char pstrText[ 100 ];
		sprintf( pstrText, "랭킹 오류 %d",  CRYLNetworkData::Instance()->m_dwNetworkError);
		pChat->AddMessage( pstrText, CRYLNetworkData::Instance()->m_dwMyChrID, 128 );
	}
*/
	return false;
}

//--------------------------------------------------------------------------------------------------------------------------
// GAME_EVENT_FUNC(ParseCharAward)
GAME_EVENT_FUNC(ParseCharAward)
{
	unsigned long	ulCharID, ulExp;
	CRYLGameData*	pGame = CRYLGameData::Instance() ;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharAward( lpPktBase, &ulCharID, &ulExp );

	CRYLChattingDlg* pChat = CRYLChattingDlg::Instance() ;

	if ( 0 == CRYLNetworkData::Instance()->m_dwNetworkError )
	{
		if ( ulCharID == CRYLNetworkData::Instance()->m_dwMyChrID && ulExp > 0 )
		{
			pGame->m_csStatus.m_Info.Exp += ulExp;
			float percent = pGame->m_csStatus.m_llExpMax;
			percent= (ulExp/percent);
			percent*=100;
			char ahuz='%';
			char pstrText[ 100 ];
			sprintf( pstrText, CRYLStringTable::m_strString[ 290 ], ulExp, percent, ahuz );//was without the percent.
			pChat->AddMessage( pstrText, CRYLNetworkData::Instance()->m_dwMyChrID, 128 );

			if ( pGame->m_csStatus.m_Info.Exp >= g_dtDateTable.MAXEXPTABLE[ pGame->m_csStatus.m_Info.Level ] )
			{
				__int64 OverExp = pGame->m_csStatus.m_Info.Exp - g_dtDateTable.MAXEXPTABLE[ pGame->m_csStatus.m_Info.Level ] ;

				// 한번에 2레벨을 레벨업 할려고 할 경우
				if ( OverExp >= g_dtDateTable.MAXEXPTABLE[ pGame->m_csStatus.m_Info.Level + 1 ] )
				{
					// 99.9% 가 되도록 (1 모자라도록) 해준다.
					pGame->m_csStatus.m_Info.Exp = g_dtDateTable.MAXEXPTABLE[ pGame->m_csStatus.m_Info.Level + 1 ] - 1 ;
				}
				else
				{
					// 레벨 업후 남은 경험치를 준다.
					pGame->m_csStatus.m_Info.Exp = OverExp ;
				}
			}
		}

		return true;
	}

	return false;
}

//--------------------------------------------------------------------------------------------------------------------------
// GAME_EVENT_FUNC(ParseCharStatueInfo)
GAME_EVENT_FUNC(ParseCharStatueInfo)
{
	unsigned short  usKID;
	unsigned char   ucState;
	unsigned long   ulMileage;
	unsigned char   ucIndex;
	unsigned short  usNowHP;
	unsigned short  usHumanNum;
	unsigned short  usAkhanNum;
	CRYLGameData*   pGame = CRYLGameData::Instance();

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleStatueInfo(lpPktBase, &ucIndex, &usKID, &ucState, &usNowHP, &ulMileage, &usHumanNum, &usAkhanNum );
	// 상태 1 = 노말, 2 = 맞고있는 상태

	if ( 0 == CRYLNetworkData::Instance()->m_dwNetworkError )
	{
		if ( pGame->m_lpInterface->m_lstWindow.size() > WINDOW_STONESTATUE ) 
		{
			if ( CRYLNetworkData::Instance()->m_dwPresentZone == SERVER_ID::CAPITAL )//hayzohar was  STATUE_ZONE
			{
				//-- 2004. 3. 12. Zergra From. --//
				// 무슨 이유인지는 모르겠는데 SetStoneStatueState() 함수를 호출하면 StoneStatue 객체가 소멸될때 뻑이 남.
				// 일반 배틀 그라운드에서는 그런 현상이 없는듯한데 왜인지 모르겠는데 배틀 그라운드 서버군용 클라이언트에서만 이런 일이....
				// 그래서 일단 배틀 그라운드 서버군에서는 아래 루틴 처리 없이 바로 return
				CRYLGameScene*			 pScene      = (CRYLGameScene*)CRYLSceneManager::Instance()->FindScene( GAME_SCENE );
				CRYLLargeMapFrameDlg*	 pLargeMapDlg= (CRYLLargeMapFrameDlg*)pScene->GetLargeMapDlg();
				pLargeMapDlg->SetStoneState(ucIndex, usKID, ucState);

				return true; 
				//-- Zergra To. --//
			}

			if (/*SERVER_ID::STONE_WAR1*/SERVER_ID::CAPITAL <= CRYLNetworkData::Instance()->m_dwPresentZone && CRYLNetworkData::Instance()->m_dwPresentZone <= SERVER_ID::STONE_WAR3)//hayzohar statue bug? medal shower?
			{
				// 마일리지
				if ( ulMileage != 0 )
				{
					RYLCreature*    pSelfCreature = RYLCreatureManager::Instance()->GetSelfCreature();
					if( !pSelfCreature )
						return true;

					CharDataNode*   pSelfData = RYLCharacterDataManager::Instance()->GetCharData( pSelfCreature->GetCharID() );
					if( !pSelfData )
						return true;

					pSelfData->m_ulMileage = CRYLGameData::Instance()->m_csStatus.m_Info.Mileage = ulMileage;
			        
					// 128 연두색
					// 129 빨간색
					// pChat->AddMessage( CRYLStringTable::m_strString[ 1210 ], CRYLNetworkData::Instance()->m_dwMyChrID, 128 ) ;
				}
			}
			
			if (( CRYLNetworkData::Instance()->m_dwPresentZone == SERVER_ID::ZONE3 ) )//hayzohar statue bug? medal shower? was ZONE3
			{
				( ( CHumanStoneStatue* )pGame->m_lpInterface->m_lstWindow[ WINDOW_STONESTATUE ] )->SetStoneStatueState( ucIndex, 
					usKID, 
					ucState, 
					usNowHP, 
					ulMileage, 
					usHumanNum, 
					usAkhanNum );
			}
		}
		return true;
	}

	return false;
}

//--------------------------------------------------------------------------------------------------------------------------
// GAME_EVENT_FUNC(ParseCharCameraScript)
GAME_EVENT_FUNC(ParseCharCameraScript)
{
	unsigned long	ulDeleteObjectID, ulNewObjectID;
	POS				posNewPos;
	CRYLGameData*	pGame = CRYLGameData::Instance() ;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCameraScript( lpPktBase, &ulDeleteObjectID, &ulNewObjectID, &posNewPos );

	if ( 0 == CRYLNetworkData::Instance()->m_dwNetworkError )
	{
		GameProcess::LogCharUnload(ulDeleteObjectID, LOG_RFL);

		RYLCreatureManager::Instance()->DeleteCreature( ulDeleteObjectID );

		const CMonsterMgr::MonsterProtoType *pMonster = g_MonsterMgr.GetMonsterProtoType( ulNewObjectID & 0x0000FFFF );

		if ( pMonster && pMonster->m_MonsterInfo.m_cSkillPattern == MonsterInfo::MonsterPattern::PATTERN_STRUCTURE )
		{
			RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( ulNewObjectID );

			if ( !pTempCreature )
			{
				vector3 vecPos( posNewPos.fPointX * 100.0f, posNewPos.fPointY * 100.0f + 15.0f, posNewPos.fPointZ * 100.0f );

				GameProcess::LogCharLoad(ulNewObjectID, LOG_RFL);
				RYLCreatureManager::Instance()->AddCreature( ulNewObjectID, pMonster->m_MonsterInfo.m_strName, (char*)pMonster->m_MonsterInfo.m_strModelingFlag, 
					vecPos, 0.0f, pMonster->m_MonsterInfo.m_bCollision, pMonster->m_MonsterInfo.m_cNation );

				pTempCreature = RYLCreatureManager::Instance()->GetCreature( ulNewObjectID );

				if ( pTempCreature )
				{
					CZ3DGeneralChrModel* pTempModel = pTempCreature->GetCharModel();

					if ( pTempModel )
					{
						pTempModel->SetScale( pMonster->m_MonsterInfo.m_fSize );
					}

					CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( ulNewObjectID );
					if ( pTempData )
					{
						pTempData->m_nChrPattern= MonsterInfo::MonsterPattern::PATTERN_STRUCTURE;
						pTempData->m_cNation    = pTempData->m_cRace = pMonster->m_MonsterInfo.m_cNation;
						pTempData->m_ulMaxHP    = pMonster->m_CreatureStatus.m_StatusInfo.m_nMaxHP;
						pTempData->m_ulMaxMP    = pMonster->m_CreatureStatus.m_StatusInfo.m_nMaxMP;
						pTempData->m_lCurrHP    = pTempData->m_ulMaxHP;
					}

					CEffScript *pRespawnEff = new CEffScript;
					CSceneManager::AddEffectScript( pRespawnEff );

					//by Hades Kang - option dialog < skill effect >
					if ( pTempData->m_cRace == CClass::RaceType::AKHAN )
					{
						pRespawnEff->GetScriptBinData( "stone_a_re_effect11.esf", pGame->m_bFriendEffect );
					} 
					else if ( pTempData->m_cRace == CClass::RaceType::HUMAN )
					{
						pRespawnEff->GetScriptBinData( "stone_h_re_effect11.esf", pGame->m_bFriendEffect );
					} 
					else
					{
						pRespawnEff->GetScriptBinData( "stone_n_re_effect11.esf", pGame->m_bFriendEffect );
					}
					/*
					if ( pTempData->m_cRace == CClass::RaceType::AKHAN )
					{
					pRespawnEff->GetScriptBinData( "stone_a_re_effect11.esf", pGame->m_bEveryEffect );
					} 
					else if ( pTempData->m_cRace == CClass::RaceType::HUMAN )
					{
					pRespawnEff->GetScriptBinData( "stone_h_re_effect11.esf", pGame->m_bEveryEffect );
					} 
					else
					{
					pRespawnEff->GetScriptBinData( "stone_n_re_effect11.esf", pGame->m_bEveryEffect );
					}
					*/

					pRespawnEff->SetStartPos( pTempCreature->GetPosition()->x, pTempCreature->GetPosition()->y, pTempCreature->GetPosition()->z );
					pRespawnEff->SetEndPos( (D3DXVECTOR3*)pTempCreature->GetPosition(), 1 );
				}
			}
		}

		return true;
	}

	return false;
}

//--------------------------------------------------------------------------------------------------------------------------
// GameProcess::SetHitEffect()
void GameProcess::SetHitEffect( AtType& refAtType, RYLCreature* pCreature, RYLCreature* pUseCreature, int nDIndex )
{
	CRYLGameData* pGame = CRYLGameData::Instance();
	const Skill::ProtoType *pSkillTypeTemp = g_SkillMgr.GetSkillProtoType( refAtType.m_wType );

	if ( pUseCreature && pUseCreature->GetEffect()->GetChantStatus( Skill::SpellID::Hide ) ) // Hide 한 캐릭터에게는 이펙트 표시 안함
		return;

	if ( pSkillTypeTemp )
	{
		const Skill::ProtoType *pSkillType = &pSkillTypeTemp[ refAtType.m_cSkillLockCount ];
		if ( pSkillType )
		{
			if ( pSkillType->m_eSkillType == Skill::Type::CHANT )
			{
				if ( strcmp( pSkillType->m_szEffectFileName, "" ) )
				{
					// Chant 시전 effect 
					float fChrDir = pCreature->GetDirection();
					fChrDir = -fChrDir;
					fChrDir -= FLOAT_PHI / 2.0f;

					vector3 vecChrToward( cosf( fChrDir ), 0.0f, sinf( fChrDir ) );
					vecChrToward.Normalize( );

					CEffScript *test_script = new CEffScript;
					unsigned long ulEnemyType = CRYLGameData::GetClientEnemyType( pCreature->GetCharID() );
					if ( EnemyCheck::EC_ENEMY == ulEnemyType && pCreature->GetEffect()->GetChantStatus( Skill::SpellID::Stealth ) )
					{
						test_script->GetScriptBinData( (char*)pSkillType->m_szEffectFileName, false );
					} 
					else
					{
						// by Hades Kang - option dialog < skill effect >
						switch ( ulEnemyType )
						{
							case EnemyCheck::EC_ENEMY:
								test_script->GetScriptBinData( (char*)pSkillType->m_szEffectFileName, pGame->m_bEnemyEffect );
								break;

							case EnemyCheck::EC_FRIEND:
								test_script->GetScriptBinData( (char*)pSkillType->m_szEffectFileName, pGame->m_bFriendEffect );							
								break;

							case EnemyCheck::EC_NEUTRAL:
								test_script->GetScriptBinData( (char*)pSkillType->m_szEffectFileName, pGame->m_bNeutralEffect );							
								break;
						}
					}
					test_script->SetStartPos( pCreature->GetPosition()->x, pCreature->GetPosition()->y, pCreature->GetPosition()->z );
					test_script->SetEndPos( (D3DXVECTOR3*)pCreature->GetPosition(), 1 );

					// 스킬 이펙트 사이즈 설정 - Vincent
					if ( CRYLGameData::GetClientIdentity( pCreature->GetCharID() ) == Creature::CT_MONSTER )
					{
						const CMonsterMgr::MonsterProtoType *pMonster = g_MonsterMgr.GetMonsterProtoType( pCreature->GetCharID() & 0x0000FFFF );
						if (NULL != pMonster)
						{
							test_script->SetScale( pCreature->GetCharModel()->GetEffectScale() * pMonster->m_MonsterInfo.m_fSkillEffectSize );
						}
					}
					// ---------------------------------

					test_script->SetChant( true );	
					test_script->SetPlayer( pCreature, pCreature->GetCharID() );
					test_script->SetChr( pCreature->GetCharModel(), pCreature->GetCharModel() );
					test_script->SetChrFwd( -vecChrToward.x, -vecChrToward.y, -vecChrToward.z );

					if ( pGame->m_csStatus.CheckPartyMember( pCreature->GetCharID() ) || 
						pCreature->GetCharID() == CRYLNetworkData::Instance( )->m_dwMyChrID )
					{
						test_script->SetMine( true );
					} 
					else
					{
						test_script->SetMine( false );
					}

					CSceneManager::m_EffectManager.AddEffScript( test_script );
				}
			} 
			// 2차 Skill 이팩트 				
			else		
			{	
				if ( strcmp( pSkillType->m_szHitFileName, "" ) )
				{
					CEffScript *test_script = new CEffScript;
					unsigned long ulEnemyType = CRYLGameData::GetClientEnemyType( pCreature->GetCharID() );
					if ( EnemyCheck::EC_ENEMY == ulEnemyType && pCreature->GetEffect()->GetChantStatus( Skill::SpellID::Stealth ) )
					{
						test_script->GetScriptBinData( (char*)pSkillType->m_szHitFileName, false );
					} 
					else
					{
						if ( pUseCreature == RYLCreatureManager::Instance()->GetSelfCreature() )
						{
							test_script->GetScriptBinData( (char*)pSkillType->m_szHitFileName, pGame->m_bSelfEffect );
						}
						else
						{
							if ( pUseCreature != NULL )
							{
								unsigned long ulUseType = CRYLGameData::GetClientEnemyType( pUseCreature->GetCharID() );
								//by Hades Kang - option dialog < skill effect >
								switch ( ulUseType )
								{
									case EnemyCheck::EC_ENEMY:
										test_script->GetScriptBinData( (char*)pSkillType->m_szHitFileName, pGame->m_bEnemyEffect );
										break;

									case EnemyCheck::EC_FRIEND:
										test_script->GetScriptBinData( (char*)pSkillType->m_szHitFileName, pGame->m_bFriendEffect );							
										break;

									case EnemyCheck::EC_NEUTRAL:
										test_script->GetScriptBinData( (char*)pSkillType->m_szHitFileName, pGame->m_bNeutralEffect );							
										break;
								}
							}
						}
					}

					if ( pUseCreature != NULL && 
						( ( strstr( pSkillType->m_szHitFileName,"E_Lightning" ) != NULL ) ||	// lightning
						( strstr( pSkillType->m_szHitFileName,"E_Grease" ) != NULL ) ||			// 그리스
						( strstr( pSkillType->m_szHitFileName,"E_Entangle" ) != NULL ) ||	    // 인텡글
						( strstr( pSkillType->m_szHitFileName,"E_LSTRENGTH" ) != NULL ) ||		// 로우어 스트렝스
						( strstr( pSkillType->m_szHitFileName,"E_A_Rot" ) != NULL ) ||			// 랏 
						( strstr( pSkillType->m_szHitFileName,"E_A_Shock" ) != NULL ) ||		// 쇼크
						( strstr( pSkillType->m_szHitFileName,"E_A_Root" ) != NULL ) ||			// 루트
						( strstr( pSkillType->m_szHitFileName,"E_CureWounds" ) != NULL ) ||		// 큐어운즈
						( strstr( pSkillType->m_szHitFileName,"E_purification" ) != NULL ) ||	// 퓨리피 케이션
						( strstr( pSkillType->m_szHitFileName,"E_Dispel" ) != NULL ) 		    // 디스펠
						) ) 
					{ 
						// 광역 effect ( 쏜사람 존재할때 )
						float fChrDir = pUseCreature->GetDirection();
						fChrDir = -fChrDir;
						fChrDir -= FLOAT_PHI / 2.0f;

						vector3 vecChrToward( cosf( fChrDir ), 0.0f, sinf( fChrDir ) );
						vecChrToward.Normalize( );

						test_script->SetStartPos( pUseCreature->GetPosition()->x,pUseCreature->GetPosition()->y,pUseCreature->GetPosition()->z );
						test_script->SetPlayer( pUseCreature, pUseCreature->GetCharID() );
						test_script->SetChr( pUseCreature->GetCharModel(), pCreature->GetCharModel() );
						test_script->SetChrFwd( -vecChrToward.x, -vecChrToward.y, -vecChrToward.z );

						// 스킬 이펙트 사이즈 설정 - Vincent
						if ( CRYLGameData::GetClientIdentity( pUseCreature->GetCharID() ) == Creature::CT_MONSTER )
						{
							const CMonsterMgr::MonsterProtoType *pMonster = g_MonsterMgr.GetMonsterProtoType( pUseCreature->GetCharID() & 0x0000FFFF );
							if (NULL != pMonster)
							{
								test_script->SetScale( pUseCreature->GetCharModel()->GetEffectScale() * pMonster->m_MonsterInfo.m_fSkillEffectSize );
							}
						}
						// ---------------------------------

						if ( pUseCreature->GetCharID() == CRYLNetworkData::Instance( )->m_dwMyChrID )
						{
							EffectNode* pEffect     = new EffectNode;
							pEffect->m_lpEffect     = test_script;
							pEffect->m_dwTargetID   = pCreature->GetCharID();

							g_SkillSystem.m_lstEffect.push_back( pEffect );
						}
					} 
					else
					{
						float fChrDir = pCreature->GetDirection();
						fChrDir = -fChrDir;
						fChrDir -= FLOAT_PHI / 2.0f;

						vector3 vecChrToward( cosf( fChrDir ), 0.0f, sinf( fChrDir ) );
						vecChrToward.Normalize( );

						test_script->SetStartPos( pCreature->GetPosition()->x, pCreature->GetPosition()->y, pCreature->GetPosition()->z );
						test_script->SetPlayer( pCreature,pCreature->GetCharID() );
						test_script->SetChr( pCreature->GetCharModel() , pCreature->GetCharModel() );
						test_script->SetChrFwd( -vecChrToward.x, -vecChrToward.y, -vecChrToward.z );

						// 스킬 이펙트 사이즈 설정 - Vincent
						if ( CRYLGameData::GetClientIdentity( pCreature->GetCharID() ) == Creature::CT_MONSTER )
						{
							const CMonsterMgr::MonsterProtoType *pMonster = g_MonsterMgr.GetMonsterProtoType( pCreature->GetCharID() & 0x0000FFFF );
							if (NULL != pMonster)
							{
								test_script->SetScale( pCreature->GetCharModel()->GetEffectScale() * pMonster->m_MonsterInfo.m_fSkillEffectSize );
							}
						}
						// ---------------------------------
					}

					test_script->SetEndPos( pCreature->GetPosition()->x,pCreature->GetPosition()->y,pCreature->GetPosition()->z );

					if ( pGame->m_csStatus.CheckPartyMember( pCreature->GetCharID() ) || pCreature->GetCharID() == CRYLNetworkData::Instance( )->m_dwMyChrID )
					{
						test_script->SetMine( true );
					} 
					else
					{
						test_script->SetMine( false );
					}

					CSceneManager::m_EffectManager.AddEffScript( test_script );
				} // if ( strcmp( pSkillType->m_szHitFileName, "" ) )
			}
		}
	}
}


GAME_EVENT_FUNC(ParseCharRespawnInfo)
{
	unsigned long CharID_Out ; 
	unsigned short wSize ; 

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharRespawnInfo( lpPktBase, 
		&CharID_Out,									// 내 id
		&CRYLGameData::Instance()->m_iRespawnNum,		// 리스폰이 가능한 장소 개수
		&wSize,											// Buff size
		CRYLGameData::Instance()->m_RespawnArea ) ;	// 

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		CRYLGameScene* pScene = static_cast<CRYLGameScene*>(CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) );	
		pScene->GetRespawnDlg()->SetVisible( FALSE ) ;

		CRYLLargeMapFrameDlg* pDlg = static_cast<CRYLLargeMapFrameDlg*>( pScene->GetLargeMapDlg() ) ;

		pDlg->MakeRespawnButton() ;
		pDlg->SetVisible( TRUE ) ;
		pDlg->GetTabButton()->SetFocusTabButton( 0 ) ;
		pDlg->SetFocusPanel( 0 ) ;
		pDlg->SetPortal(TRUE);

		CRYLGameData*	pGame = CRYLGameData::Instance();

		// 못움직이게 한다.
		pGame->SetAutoRun( FALSE ) ;
		pGame->SetEnableMove( FALSE ) ;

		// Simple Mouse Mode 로 변경
		if ( pGame && !pGame->m_bMouseMode )
		{
			if ( !pScene->GetStatusAndQuestFrameDlg()->GetVisible() && !pScene->GetSocietyFrameDlg()->GetVisible() &&
				!pScene->GetGuildFrameDlg()->GetVisible() && !pScene->GetLargeMapDlg()->GetVisible() &&
				!pScene->GetHumanInventoryFrameDlg()->GetVisible() && !pScene->GetSystemDlg()->GetVisible() &&
				!pScene->GetBlacksmithDlg()->GetVisible() && !pScene->GetExchangeDlg()->GetVisible() &&
				!pScene->GetJobChangeDlg()->GetVisible() && !pScene->GetOptionDlg()->GetVisible() &&
				!pScene->GetQuestDetailDataDlg()->GetVisible() && !pScene->GetQuestOpeningDlg()->GetVisible() &&
				!pScene->GetRankingDlg()->GetVisible() && !pScene->GetStatusRetrainDlg()->GetVisible() &&
				!pScene->GetStreetStallDlg()->GetVisible() && !pScene->GetNewTradeDlg()->GetVisible() &&
				!pScene->GetVaultCertificationdDlg()->GetVisible() && !pScene->GetVaultDlg()->GetVisible() &&
				!pScene->GetVaultPasswordDlg()->GetVisible() && !pScene->GetEmblemUpgradeDlg()->GetVisible() )
			{
				pGame->m_bSimpleMouse = FALSE;
			}
			else 
			{
				pGame->m_bSimpleMouse = TRUE;
			}
		}
	}
	else
	{
	}

	return false ;
}

GAME_EVENT_FUNC(ParseCharRespawnAreaInfo)
{
	unsigned long	CharID_Out ; 
	unsigned long	dwTownID ;
	unsigned short	wWaitOrder ; 
	unsigned char	cHour ;
	unsigned char	cMin ;
	unsigned char	cSec ;
	bool			bCount ;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharRespawnAreaInfo( lpPktBase, 
		&CharID_Out, // 내 ID
		&dwTownID,	 // TownID
		&wWaitOrder, // 대기수
		&cHour,		 // 시간
		&cMin,		 // 분
		&cSec,		 // 초
		&bCount) ;	 // true면 진행상황

	if (0 == CRYLNetworkData::Instance()->m_dwNetworkError)
	{
		CRYLGameScene* pScene = static_cast<CRYLGameScene*>(CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) );	
		CRYLLargeMapFrameDlg* pDlg = static_cast<CRYLLargeMapFrameDlg*>( pScene->GetLargeMapDlg() ) ;
		pDlg->SetVisible( FALSE ) ;
		pDlg->SetClearPortal(TRUE);

		CRYLBattleWaitDlg* pWaitDlg = static_cast<CRYLBattleWaitDlg*>(pScene->GetBattleWaitDlg()) ;
		pWaitDlg->SetData( dwTownID, wWaitOrder, cHour, cMin, cSec, bCount ) ;	
		pWaitDlg->InitDlg( bCount ) ;
		pWaitDlg->SetVisible( TRUE ) ;			
		pWaitDlg->SetPortal(TRUE);

		CRYLGameData* pGame = CRYLGameData::Instance();

		pGame->SetAutoRun( FALSE ) ;
		pGame->SetEnableMove( FALSE ) ;	
	}
	else
	{
	}

	return false;
}
	