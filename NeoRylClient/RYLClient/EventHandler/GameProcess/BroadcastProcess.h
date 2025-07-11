#ifndef _RYL_CLIENT_BROADCAST_PROCESS_H_
#define _RYL_CLIENT_BROADCAST_PROCESS_H_

#include <Network/Broadcast/ClientCellMgr.h>

// forward decl
class CGameEventHandler;


// 캐릭터 로드        - CID를 찍어준다.
// 캐릭터 언로드      - CID를 찍어준다.
// 캐릭터 셀 로그인   - CID/이름을 찍어준다.
// 캐릭터 셀 로그아웃 - CID를 찍어준다.
// 캐릭터 정보 갱신   - CID/이름을 찍어준다.

#define LOG_RFL __FUNCTION__, __FILE__, __LINE__

namespace GameProcess
{
    void LogCharLoad(unsigned long dwCID, const char* szRoutine, const char* szFile, const int nLine);
    void LogCharUnload(unsigned long dwCID, const char* szRoutine, const char* szFile, const int nLine);
    
    bool RegisterBroadcastHandler(CGameEventHandler& GameEventHandler);
}

extern Broadcast2nd::CClientCellMgr g_ClientCellMgr;
extern const bool g_bLogCharBroadcast;

#endif