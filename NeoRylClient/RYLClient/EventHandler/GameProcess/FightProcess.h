#ifndef _RYL_CLIENT_FIGHT_PROCESS_H_
#define _RYL_CLIENT_FIGHT_PROCESS_H_

// forward decl
class CGameEventHandler;  
class RYLCreature;

struct AtType;

namespace GameProcess
{
    bool RegisterFightHandler(CGameEventHandler& GameEventHandler);
    void SetHitEffect( AtType& refAtType, RYLCreature* pCreature, RYLCreature* pUseCreature, int nDIndex );
}

#endif