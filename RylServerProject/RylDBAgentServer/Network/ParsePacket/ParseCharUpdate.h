#ifndef _DBAGENT_SERVER_PARSE_CHAR_UPDATE_H_
#define _DBAGENT_SERVER_PARSE_CHAR_UPDATE_H_

#include <Network/Packet/PacketStruct/CharLoginOutPacket.h>

// forward delc.
class CSendStream;


namespace Item
{
    // forward decl.
    class CItemSerialMgr;
};

namespace DBAgent
{

namespace ParseCharUpdate
{
    // ------------------------------------------------------------------------
    // ĳ���� ������Ʈ ������ �Ľ�
 
    bool Parse(CSendStream& SendStream, unsigned long dwServerID, 
        Item::CItemSerialMgr& itemSerialMgr, PktDBUpdate* lpPktDBUpdate);


    // ------------------------------------------------------------------------
    // ������ ������Ʈ �޼���

    bool UpdateDeposit(CSendStream& SendStream, PktBase* lpPktBase);
    bool UpdateDepositDB(CSendStream& SendStream, PktBase* lpPktBase);
    bool UpdateFriendDB(CSendStream& SendStream, PktBase* lpPktBase);
    bool UpdateQuestDB(CSendStream& SendStream, PktBase* lpPktBase);
    bool UpdateConfigInfoDB(CSendStream& SendStream, PktBase* lpPktBase);	
	
	
	//--------------------------------------------------------------------------
	// ��� ���� ���� ����.

	bool SaveEnemy(CSendStream& SendStream, PktBase* lpPktBase);

	//--------------------------------------------------------------------------
	// ���̽�ƽ Ű ����.
	bool UpdateKeyInfo(CSendStream& SendStream, PktBase* lpPktBase);


	// ĳ�������� ���
	bool UseCashItem(CSendStream& SendStream, PktBase* lpPktBase);

	// ����Ʈ�� �̺�Ʈ
	bool ExtraEvent(CSendStream& SendStream, PktBase* lpPktBase);
}

namespace ParseCharAdmin
{
	bool AdminCommandLog(CSendStream& SendStream, PktBase* lpPktBase);	
}

}

#endif