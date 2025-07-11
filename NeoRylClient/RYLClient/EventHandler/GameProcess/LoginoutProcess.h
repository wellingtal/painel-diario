#ifndef _RYL_CLIENT_LOGINOUT_PROCESS_H_
#define _RYL_CLIENT_LOGINOUT_PROCESS_H_

// forward decl
class CGameEventHandler;

namespace GameProcess
{
    bool RegisterLoginoutHandler(CGameEventHandler& GameEventHandler);
}

#endif