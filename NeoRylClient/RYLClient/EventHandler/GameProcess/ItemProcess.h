#ifndef _RYL_CLIENT_ITEM_PROCESS_H_
#define _RYL_CLIENT_ITEM_PROCESS_H_

// forward decl
class CGameEventHandler;

namespace GameProcess
{
    void LogInvenData(unsigned __int64 nObjectID);
    bool RegisterItemHandler(CGameEventHandler& GameEventHandler);
}

#endif