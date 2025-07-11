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

    AddDispatch(CmdCharLogin,       ParseCharLogin);		// 캐릭터 로그인
    AddDispatch(CmdCharLogout,      ParseCharLogout);		// 캐릭터 로그아웃
    AddDispatch(CmdCharMoveZone,    ParseCharMoveZone);		// 캐릭터 존 이동
    AddDispatch(CmdServerZone,      ParseServerZone);		// 캐릭터 존 이동
	AddDispatch(CmdCSAuth,          ParseCSAuth);			// 인증 코드 (게임 가드)    
}


CFieldGameClientDispatch::CFieldGameClientDispatch(CSession& Session)
:	CGameClientDispatch(Session, CFieldGameClientDispatchTable::GetInstance())
{


}