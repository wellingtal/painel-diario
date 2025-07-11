
#include "RYLGameData.h"
#include "RYLNetworkData.h"

#include <Creature/EnemyCheck.h>
#include <Creature/Siege/SiegeConstants.h>

#include <Network/Packet/PacketStruct/CastlePacket.h>

#include "RYLObjectControl.h"
#include "RYLCharacterControl.h"
#include "RYLShortAttackWeaponControl.h"
#include "RYLLongAttackWeaponControl.h"
#include "RYLShortDefenceWeaponControl.h"
#include "RYLLongDefenceWeaponControl.h"
#include "RYLDropshipControl.h"

#include "RYLCreatureManager.h"
#include "RYLCreature.h"

#include "RYLSceneManager.h"
#include "RYLGameScene.h"

#include "RYLCastleManager.h"
#include "RYLCastle.h"

#include "RYLAuthoritySettingDlg.h"
#include "RYLGuildFrameDlg.h"
#include "RYLGuildInfoPanel.h"

#include "RYLRaceBase.h"
#include "GMMemory.h"

void CRYLRaceBase::ArmsRide( unsigned short wObjType, unsigned long dwCID, unsigned long dwArmsID ) 
{
	RYLCreature* pChrCreature   = RYLCreatureManager::Instance()->GetCreature( dwCID ) ;
	RYLCreature* pArmsCreature  = RYLCreatureManager::Instance()->GetCreature( dwArmsID ) ;

	if ( dwCID == CRYLNetworkData::Instance()->m_dwMyChrID )
	{
		CRYLNetworkData::Instance()->m_dwMyChrID = dwArmsID ;

		// 병기에 탄 케릭 ID
		pChrCreature->SetRideEnable( true ) ;
		pArmsCreature->AddRiderID( dwCID ) ;

		g_CharacterData.SwapElement( pChrCreature, pArmsCreature ) ;

		switch ( wObjType )
		{
			case SHORT_RANGE_CASTLE_ARMS :
				g_CharacterData.m_pRYLShortDefenceWeaponControl->SetCreature( pArmsCreature ) ;
				g_CharacterData.m_pRYLShortDefenceWeaponControl->SetCharacterDataNode( CSceneManager::GetCharacterDataNode( pArmsCreature->GetCharModel() ) ) ;
				CSceneManager::UnallocListChr( *g_CharacterData.m_pRYLShortDefenceWeaponControl->GetCharacterDataNode(), TRUE ) ;
				g_CharacterData.SetWeaponControlKind( SHORT_DEFENCEWEAPON_CONTROL ) ;
				break ;

			case LONG_RANGE_CASTLE_ARMS :
				g_CharacterData.m_pRYLLongDefenceWeaponControl->SetCreature( pArmsCreature ) ;
				g_CharacterData.m_pRYLLongDefenceWeaponControl->SetCharacterDataNode(  CSceneManager::GetCharacterDataNode( pArmsCreature->GetCharModel() ) ) ;
				CSceneManager::UnallocListChr( *g_CharacterData.m_pRYLLongDefenceWeaponControl->GetCharacterDataNode(), TRUE ) ;
				g_CharacterData.SetWeaponControlKind( LONG_DEFENCEWEAPON_CONTROL ) ;
				break ;

			case SHORT_RANGE_SIEGE_ARMS :
				g_CharacterData.m_pRYLShortAttackWeaponControl->SetCreature( pArmsCreature ) ;
				g_CharacterData.m_pRYLShortAttackWeaponControl->SetCharacterDataNode(  CSceneManager::GetCharacterDataNode( pArmsCreature->GetCharModel() ) ) ;
				CSceneManager::UnallocListChr( *g_CharacterData.m_pRYLShortAttackWeaponControl->GetCharacterDataNode(), TRUE ) ;
				g_CharacterData.SetWeaponControlKind( SHORT_ATTACKWEAPON_CONTROL ) ;
				break ;

			case LONG_RANGE_SIEGE_ARMS :
				g_CharacterData.m_pRYLLongAttackWeaponControl->SetCreature( pArmsCreature ) ;
				g_CharacterData.m_pRYLLongAttackWeaponControl->SetCharacterDataNode(  CSceneManager::GetCharacterDataNode( pArmsCreature->GetCharModel() ) ) ;
				CSceneManager::UnallocListChr( *g_CharacterData.m_pRYLLongAttackWeaponControl->GetCharacterDataNode(), TRUE ) ;
				g_CharacterData.SetWeaponControlKind( LONG_ATTACKWEAPON_CONTROL ) ;
				break ;

			case AIRSHIP :
				g_CharacterData.m_pRYLDropshipControl->SetCreature( pArmsCreature ) ;
				g_CharacterData.m_pRYLDropshipControl->SetCharacterDataNode(  CSceneManager::GetCharacterDataNode( pArmsCreature->GetCharModel() ) ) ;
				CSceneManager::UnallocListChr( *g_CharacterData.m_pRYLDropshipControl->GetCharacterDataNode(), TRUE ) ;
				g_CharacterData.SetWeaponControlKind( DROPSHIP_CONTROL ) ;
				break ;
		}

		g_CharacterData.m_pRYLCharacterControl->SetCharacterDataNode( CSceneManager::GetCharacterDataNode( pChrCreature->GetCharModel() ) ) ;
		//CSceneManager::m_CharacterManager.DeleteElement( pChrCreature->GetCharModel() ) ;

		ChangeKeyboardMode() ; 

		RYLCreatureManager::Instance()->SetSelfCreature( pArmsCreature ) ;
	}
	else
	{
		// 병기에 탄 케릭 ID
		if ( pChrCreature )
		{
			pChrCreature->SetRideEnable( true ) ;
		}
		
		RYLCreatureManager::Instance()->AddArmsCreature(dwCID, dwArmsID);
		
		if ( pArmsCreature ) pArmsCreature->AddRiderID( dwCID ) ;
	}
}

void CRYLRaceBase::ArmsGetOff( unsigned long dwCID, unsigned long dwArmsID ) 
{
	// 크리쳐 정보를 얻는다.
	RYLCreature* pChrCreature   = RYLCreatureManager::Instance()->GetCreature( dwCID ) ;
	RYLCreature* pArmsCreature  = RYLCreatureManager::Instance()->GetCreature( dwArmsID ) ;

	if ( dwArmsID == CRYLNetworkData::Instance()->m_dwMyChrID )
	{
		CRYLNetworkData::Instance()->m_dwMyChrID = dwCID ; 

		// 병기에 탄 케릭 ID
		pChrCreature->SetRideEnable( false ) ;
		pArmsCreature->DeleteRiderID( dwCID ) ;

		CSceneManager::UnallocListChr( *g_CharacterData.m_pRYLCharacterControl->GetCharacterDataNode(), TRUE ) ;

		g_CharacterData.SetWeaponControlKind( CHARACTER_CONTROL ) ;

		RYLCreatureManager::Instance()->SetSelfCreature( pChrCreature ) ;
	}
	else
	{
		// 병기에 탄 케릭 ID
		if ( pChrCreature ) pChrCreature->SetRideEnable( false ) ;
		if ( pArmsCreature ) pArmsCreature->DeleteRiderID( dwCID ) ;
	}

	RYLCreatureManager::Instance()->DeleteArmsCreature(dwCID);
}

void CRYLRaceBase::DestroyArms( unsigned long dwArmsID ) 
{
	RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( dwArmsID );

	if ( pTempCreature )
	{
		CSceneManager::DeleteCharacter( pTempCreature->GetCharModel() );
	}

	RYLCreatureManager::Instance()->DeleteCreatureList( dwArmsID );
}

BOOL CRYLRaceBase::CheckUsableSiegePopup( unsigned short wObjectType,
										  unsigned long dwMyCID, unsigned long dwOwnerCID,
										  unsigned long dwMyGID, unsigned long dwObjectGID,
										  unsigned char cMyNation, unsigned char cObjectNation )
{
	switch ( wObjectType )
	{
		case Siege::EMBLEM :
		case Siege::GATE :
			return FALSE ;

		case Siege::CASTLE_ARMS_NPC :
		{
			if ( Creature::STATELESS != cObjectNation && cMyNation == cObjectNation )
			{
				return TRUE ;
			}
			break ;
		}

		case Siege::GUARD :
		case Siege::SHORT_RANGE_CASTLE_ARMS :
		case Siege::LONG_RANGE_CASTLE_ARMS :
		{
			if ( Creature::STATELESS != cObjectNation &&
				 cMyNation == cObjectNation &&
				 dwMyCID == dwOwnerCID )
			{
				return TRUE ;
			}
			break ;
		}

		case Siege::CAMP :
		case Siege::MINING_CAMP :
		case Siege::CAMP_SHOP :
		case Siege::KARTERANT_WEAPON :
		case Siege::MERKADIA_WEAPON :
		{
			if ( 0 != dwMyGID && dwMyGID == dwObjectGID )
			{
				CRYLGameScene*		 pScene = ( CRYLGameScene * )CRYLSceneManager::Instance()->FindScene( GAME_SCENE ) ;
				CRYLGuildFrameDlg*	 pDlg   = ( CRYLGuildFrameDlg* )pScene->GetGuildFrameDlg() ;
				CRYLAuthoritySettingDlg* pASDlg = ( CRYLAuthoritySettingDlg* )pDlg->GetAuthoritySettingDlg() ;

				if ( Guild::JOIN_WAIT != pASDlg->GetTitle() )
				{
					return TRUE ;
				}
			}
			break ;
		}

		case Siege::AIRSHIP:
		{
			if ( 0 != dwMyGID && dwMyGID == dwObjectGID )
			{
				return TRUE ;
			}
			break ;
		}

		case Siege::BACKDOOR:
		{
			if ( Creature::STATELESS != cObjectNation && cMyNation == cObjectNation )
			{
				return TRUE ;
			}
			break ;
		}

		case Siege::SHORT_RANGE_SIEGE_ARMS :
		case Siege::LONG_RANGE_SIEGE_ARMS :
			return TRUE ;
	}

	return FALSE ;
}
