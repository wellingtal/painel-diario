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
    // ĳ���� ���� �� ���� �޼ҵ�
    
    bool Parse(CSendStream& SendStream, unsigned long dwServerID, 
        Item::CItemSerialMgr& itemSerialMgr, PktDD* lpPktDD);

    // --------------------------------------------------------------------
    // ��Ʋ�׶��� ������ ���� �޼ҵ�. 
    // �輷 ���Ӽ����� �߰輭���� ĳ���� ���� ���� ��û.

    bool BGServerCharSlot(CSendStream& SendStream, PktBase* lpPktBase); 
    
    // �������� ���� �޼ҵ�
    bool UnifiedCharSelect(CSendStream& SendStream, PktBase* lpPktBase);
}
}


#endif