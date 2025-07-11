#include "stdafx.h"
#include "FieldServerClientDispatch.h"
#include <Network/Packet/PacketCommand.h>
#include <Network/Dispatch/GameClient/ParseCharLoginOut.h>


class CFieldGameClientDispatchTable : public CGameClientDispatchTable
{
public:

    static CFieldGameClientDispatchTable& GetInstance();

private:

    CFieldGameClientDispatchTable();
};


CFieldGameClientDispatchTable& CFieldGameClientDispatchTable::GetInstance()
{
	static CFieldGameClientDispatchTable fieldGameClientDispatchTable;
	return fieldGameClientDispatchTable;
}


CFieldGameClientDispatchTable::CFieldGameClientDispatchTable()
{    
    using namespace GameClientParsePacket;

    AddDispatch(CmdCharLogin,       ParseCharLogin);		// ĳ���� �α���
    AddDispatch(CmdCharLogout,      ParseCharLogout);		// ĳ���� �α׾ƿ�
    AddDispatch(CmdCharMoveZone,    ParseCharMoveZone);		// ĳ���� �� �̵�
    AddDispatch(CmdServerZone,      ParseServerZone);		// ĳ���� �� �̵�
	AddDispatch(CmdCSAuth,          ParseCSAuth);			// ���� �ڵ� (���� ����)    
}


CFieldGameClientDispatch::CFieldGameClientDispatch(CSession& Session)
:	CGameClientDispatch(Session, CFieldGameClientDispatchTable::GetInstance())
{


}