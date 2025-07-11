#ifndef _DBAGENT_SERVER_PARSE_CHAR_MANAGE_H_
#define _DBAGENT_SERVER_PARSE_CHAR_MANAGE_H_

// forward decl;
class CSendStream;
struct PktDD;
struct PktBase;

namespace DBAgent
{

namespace DataStorage
{
    // forward decl.
    class CItemSerialMgr;
}


namespace ParseCharManage
{
    // --------------------------------------------------------------------
    // 캐릭터 관리 및 세션 메소드
    
    bool Parse(CSendStream& SendStream, unsigned long dwServerID, 
        Item::CItemSerialMgr& itemSerialMgr, PktDD* lpPktDD);

    // --------------------------------------------------------------------
    // 배틀그라운드 서버군 관련 메소드. 
    // 배섭 게임서버가 중계서버로 캐릭터 슬롯 정보 요청.

    bool BGServerCharSlot(CSendStream& SendStream, PktBase* lpPktBase); 
    
    // 서버통합 관련 메소드
    bool UnifiedCharSelect(CSendStream& SendStream, PktBase* lpPktBase);
}
}


#endif