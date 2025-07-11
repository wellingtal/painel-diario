#ifndef _SEND_CHAR_ETC_H_
#define _SEND_CHAR_ETC_H_

#include <Network/Packet/PacketBase.h>


// 전방 참조
class   CSendStream;
class	CCharacter;

namespace Item
{
	struct ItemPos;
}

namespace GameClientSendPacket
{
	// 캐릭터 스킬 커맨드
    bool SendCharSkillCommand(CSendStream& SendStream, unsigned long dwCharID, 
		PktBase::CMDType Cmd, unsigned char cIndex, unsigned short usSkillID, unsigned short usError, unsigned char cSkillLevel = 0, Item::ItemPos* lpItem = NULL);
};

#endif