#ifndef _RYL_CLIENT_SKILL_PROCESS_H_
#define _RYL_CLIENT_SKILL_PROCESS_H_

// forward decl
class CGameEventHandler;
class RYLCreature;

struct EnchantInfo;

namespace GameProcess
{
    bool RegisterSkillHandler(CGameEventHandler& GameEventHandler);    
}

#endif