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
    // 캐릭터 업데이트 데이터 파싱
 
    bool Parse(CSendStream& SendStream, unsigned long dwServerID, 
        Item::CItemSerialMgr& itemSerialMgr, PktDBUpdate* lpPktDBUpdate);


    // ------------------------------------------------------------------------
    // 데이터 업데이트 메서드

    bool UpdateDeposit(CSendStream& SendStream, PktBase* lpPktBase);
    bool UpdateDepositDB(CSendStream& SendStream, PktBase* lpPktBase);
    bool UpdateFriendDB(CSendStream& SendStream, PktBase* lpPktBase);
    bool UpdateQuestDB(CSendStream& SendStream, PktBase* lpPktBase);
    bool UpdateConfigInfoDB(CSendStream& SendStream, PktBase* lpPktBase);	
	
	
	//--------------------------------------------------------------------------
	// 듀얼 관련 정보 저장.

	bool SaveEnemy(CSendStream& SendStream, PktBase* lpPktBase);

	//--------------------------------------------------------------------------
	// 조이스틱 키 관련.
	bool UpdateKeyInfo(CSendStream& SendStream, PktBase* lpPktBase);


	// 캐쉬아이템 사용
	bool UseCashItem(CSendStream& SendStream, PktBase* lpPktBase);

	// 엑스트라 이벤트
	bool ExtraEvent(CSendStream& SendStream, PktBase* lpPktBase);
}

namespace ParseCharAdmin
{
	bool AdminCommandLog(CSendStream& SendStream, PktBase* lpPktBase);	
}

}

#endif