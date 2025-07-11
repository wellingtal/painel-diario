//-----------------------------------------------------------------------------------------------------
// File Name: BattleGroundClientProcess.h
//
// Programmer: Zergra( Park Jongtae ) in GamaSoft corp.
//
// Date: 2004. 2. 5. Tuesday
//-----------------------------------------------------------------------------------------------------

#ifndef _RYL_BATTLE_GROUND_CLIENT_PROCESS_H_
#define _RYL_BATTLE_GROUND_CLIENT_PROCESS_H_

// forward 
class CGameEventHandler;

namespace GameProcess
{
    bool RegisterBattleGroundClientHandler(CGameEventHandler& GameEventHandler);
}

#endif //_RYL_BATTLE_GROUND_CLIENT_PROCESS_H_