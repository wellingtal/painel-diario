#ifndef _RYL_CASTLE__PROCESS_H_
#define _RYL_CASTLE__PROCESS_H_

// forward decl
class CGameEventHandler;

namespace GameProcess
{
    bool RegisterCastleHandler(CGameEventHandler& GameEventHandler);
}

#endif