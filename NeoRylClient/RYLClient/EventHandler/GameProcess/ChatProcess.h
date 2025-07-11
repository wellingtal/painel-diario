#ifndef _RYL_CLIENT_CHAT_PROCESS_H_
#define _RYL_CLIENT_CHAT_PROCESS_H_

// forward decl
class CGameEventHandler;

namespace BroadcastInfo
{
	class CObjectInfoMgr;
};


namespace GameProcess
{
    bool RegisterChatHandler(CGameEventHandler& GameEventHandler);
}

#endif