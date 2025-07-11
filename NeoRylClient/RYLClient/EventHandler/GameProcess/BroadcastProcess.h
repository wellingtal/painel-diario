#ifndef _RYL_CLIENT_BROADCAST_PROCESS_H_
#define _RYL_CLIENT_BROADCAST_PROCESS_H_

#include <Network/Broadcast/ClientCellMgr.h>

// forward decl
class CGameEventHandler;


// ĳ���� �ε�        - CID�� ����ش�.
// ĳ���� ��ε�      - CID�� ����ش�.
// ĳ���� �� �α���   - CID/�̸��� ����ش�.
// ĳ���� �� �α׾ƿ� - CID�� ����ش�.
// ĳ���� ���� ����   - CID/�̸��� ����ش�.

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