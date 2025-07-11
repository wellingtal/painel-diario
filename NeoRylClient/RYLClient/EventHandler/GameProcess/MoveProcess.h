#ifndef _RYL_CLIENT_MOVE_PROCESS_H_
#define _RYL_CLIENT_MOVE_PROCESS_H_

// forward decl
class CGameEventHandler;

namespace GameProcess
{
    bool RegisterMoveHandler(CGameEventHandler& GameEventHandler);
}

#endif