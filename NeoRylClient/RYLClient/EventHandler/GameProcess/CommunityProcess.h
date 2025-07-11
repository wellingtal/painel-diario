#ifndef _RYL_CLIENT_COMMUNITY_PROCESS_H_
#define _RYL_CLIENT_COMMUNITY_PROCESS_H_

// forward decl
static int killCount=0;//hayzohar
class CGameEventHandler;

namespace GameProcess
{
    bool RegisterCommunityHandler(CGameEventHandler& GameEventHandler);
}

#endif