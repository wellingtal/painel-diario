//--------------------------------------------------------------------------------------------------------------------------
// File Name: MoveProcess.cpp
//
// Programmer: Zergra( Park Jongtae ) in GamaSoft corp.
//
// File Desciption: MoveProcess
//
// Date: 2004. 4. 26. ( Mon )
//--------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------------
// Headers
//--------------------------------------------------------------------------------------------------------------------------

#include "../LogFunctions.h"
#include "../GameHandler.h"
#include "../../GUITextEdit.h"

#include "RYLProfile.h"
#include "RYLClientMain.h"

#include <Network/Packet/PacketCommand.h>
#include <Network/ClientSocket/Parse/ParseMove.h>
#include <Network/Broadcast/BroadcastCharacterData.h>

#include <Creature/Monster/MonsterMgr.h>
#include <Item/ItemMgr.h>

#include "RYLGameData.h"
#include "RYLNetworkData.h"
#include "RYLSceneManager.h"
#include "RYLGameScene.h"

#include "RYLChattingDlg.h"
#include "RYLGuildFrameDlg.h"
#include "RYLGuildInfoPanel.h"

#include "BroadcastProcess.h"
#include "MoveProcess.h"
#include "GMMemory.h"

GAME_EVENT_FUNC(ParseCharMove);
GAME_EVENT_FUNC(ParseMonMove);

//--------------------------------------------------------------------------------------------------------------------------
// Event Function
//--------------------------------------------------------------------------------------------------------------------------
bool GameProcess::RegisterMoveHandler(CGameEventHandler& GameEventHandler)
{
	unsigned long dwErrorCount = 0;

	dwErrorCount += GameEventHandler.AddHandler(CmdCharMove, ParseCharMove) ? 0 : 1;
	dwErrorCount += GameEventHandler.AddHandler(CmdMonMove, ParseMonMove) ? 0 : 1;

	return (0 == dwErrorCount);
}

//--------------------------------------------------------------------------------------------------------------------------
// GAME_EVENT_FUNC(ParseCharMove)
GAME_EVENT_FUNC(ParseCharMove)
{
	unsigned long dwChrID;
	float   fDir;
	POS     pos;

	unsigned char cUpperAction, cLowerAction;	

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleCharMoveEx( lpPktBase, 
		&dwChrID, 
		&pos, 
		&fDir, 
		&cUpperAction, 
		&cLowerAction );

	if(cUpperAction > 100)
	{
		int a=0;
		a++;
	}


	if( 0 == CRYLNetworkData::Instance()->m_dwNetworkError )
	{
		vector3 vecDst;

		vecDst.x = pos.fPointX * 100.0f;
		vecDst.y = pos.fPointY * 100.0f + 15.0f;
		vecDst.z = pos.fPointZ * 100.0f;

		CRYLGameData* lpGame = CRYLGameData::Instance() ;
		RYLCreatureManager* lpCreatureMgr = RYLCreatureManager::Instance();

		RYLCreature* lpTempCreature = lpCreatureMgr->GetCreature(dwChrID) ;


		if(lpTempCreature && !lpTempCreature->GetIsDead())
		{
			lpCreatureMgr->AddAction( dwChrID, cUpperAction, cLowerAction, fDir, vecDst );
		}
		else if(CA_RESPAWN == cLowerAction)
		{
			lpCreatureMgr->AddAction( dwChrID, cUpperAction, cLowerAction, fDir, vecDst );
		}
		
		/*if ( lpTempCreature && Siege::SHORT_RANGE_SIEGE_ARMS == lpTempCreature->GetObjectType())
		{
			if ( CA_WALK == cLowerAction || CA_RUN == cLowerAction ) cLowerAction = CA_FORWARD ;
			if ( CA_WALKBACK == cLowerAction ) cLowerAction = CA_BACKWARD ;
		}*/

		if (lpGame->GetClientIdentity(dwChrID) == Creature::CT_SIEGE_OBJECT)
		{
			RYLCreature* lpTempCreature = lpCreatureMgr->GetCreature(dwChrID) ;
			RYLCreature* lpRideCreature = NULL ;

			if (lpTempCreature)
			{
				for (int i = 0; i < Siege::AIRSHIP_RIDER_NUM; ++i)
				{
					if (0 != lpTempCreature->GetRiderID(i))
					{
						lpRideCreature = lpCreatureMgr->GetCreature(lpTempCreature->GetRiderID(i)) ;
						if (lpRideCreature)
						{
							lpRideCreature->SetPosition(vecDst);
						}
					}
				}
			}
		}

		return TRUE;
	}

	return FALSE;
}

//--------------------------------------------------------------------------------------------------------------------------
// GAME_EVENT_FUNC(ParseMonMove)
GAME_EVENT_FUNC(ParseMonMove)
{
	unsigned long   ulMonID;
	unsigned short  usAction;
	unsigned short  usAniNum;
	float           fDir;
	float           fVec;
	POS             Pos;

	CRYLNetworkData::Instance()->m_dwNetworkError = ParsePacket::HandleMonMove(
        lpPktBase, &ulMonID, &Pos, &fDir, &fVec, &usAction, &usAniNum );

	if( CRYLNetworkData::Instance()->m_dwNetworkError == 0 )
	{
		fDir += FLOAT_PHI / 2.0f;
		fDir = -fDir;

		Pos.fPointX *= 100.0f;
		Pos.fPointY *= 100.0f;
		Pos.fPointZ *= 100.0f;

		unsigned long ulFrame = 0;
		const CMonsterMgr::MonsterProtoType *pMonster = g_MonsterMgr.GetMonsterProtoType( ulMonID & 0x0000FFFF );
		if( pMonster )
		{
			if( usAction == CA_WALK )
			{
				ulFrame = pMonster->m_MonsterInfo.m_MonsterMotions[ 0 ].m_dwFrame;
			} 
			else if( usAction == CA_RUN )
			{
				ulFrame = pMonster->m_MonsterInfo.m_MonsterMotions[ 1 ].m_dwFrame;
			} 
			else if( usAction == CA_CASTING )
			{
				ulFrame = pMonster->m_MonsterInfo.m_MonsterMotions[ 3 ].m_dwFrame;
			}
		} 
		else
		{
			ulFrame = 20;
		}

		if( !RYLCreatureManager::Instance()->GetCreature( ulMonID ) )
		{
			if( pMonster )
			{
				RYLCreatureManager::Instance()->AddCreature( ulMonID, pMonster->m_MonsterInfo.m_strName, (char*)pMonster->m_MonsterInfo.m_strModelingFlag,
					( vector3 & )Pos, fDir, pMonster->m_MonsterInfo.m_bCollision, pMonster->m_MonsterInfo.m_cNation );

				RYLCreature* pTempCreature = RYLCreatureManager::Instance()->GetCreature( ulMonID );

				if( pTempCreature )
				{
					CharDataNode* pTempData = RYLCharacterDataManager::Instance()->GetCharData( ulMonID );

					if( pTempData && CRYLGameData::GetClientIdentity( ulMonID ) == Creature::CT_SUMMON )
					{
						pTempData->m_ulMaxHP = pMonster->m_CreatureStatus.m_StatusInfo.m_nMaxHP;
					}

					pTempCreature->SetScale( pMonster->m_MonsterInfo.m_fSize );

					CZ3DGeneralChrModel* pTempModel = pTempCreature->GetCharModel();

					if( pTempModel )
					{
						pTempModel->SetScale( pMonster->m_MonsterInfo.m_fSize );
					}
				}
			}
		}
        /*
        else
        {
            // TODO : REMOVE HERE
            RYLCreature* lpMon = RYLCreatureManager::Instance()->GetCreature( ulMonID );

            vector3* vec3 = lpMon->GetPosition();
            const CNetworkPos oldNetPos(vec3->x / 100.0f, vec3->y / 100.0f, vec3->z / 100.0f);
            const CNetworkPos newNetPos(Pos.fPointX / 100.0f, oldNetPos.GetYPos(), Pos.fPointZ / 100.0f);
            
            if (49 < oldNetPos.GetSquaredRange(CNetworkPos(0, 0, 0)) &&
                49 < oldNetPos.GetSquaredRange(newNetPos))
            {
                const int MAX_BUFFER = 256;
                char szBuffer[MAX_BUFFER];

                if (0 < _snprintf(szBuffer, MAX_BUFFER - 1, 
                    "PktMM : CID:0x%08x : 순간이동하나?.. %f거리를 눈앞에서 이동했다! (%f,%f,%f)->(%f,%f,%f)\n", 
                    lpMon->GetCharID(), oldNetPos.GetSquaredRange(newNetPos), 
                    oldNetPos.GetXPos(), oldNetPos.GetYPos(), oldNetPos.GetZPos(),
                    newNetPos.GetXPos(), newNetPos.GetYPos(), newNetPos.GetZPos()))
                {
                    szBuffer[MAX_BUFFER - 1] = 0;
                    OutputDebugString(szBuffer);
                }        
            }
        }
        */

		RYLCreatureManager::Instance()->AddAction( ulMonID, usAction, usAction, 
            fDir, (vector3 &)Pos, fVec, usAniNum, ulFrame );

		return TRUE;
	}

	return FALSE;
}

