#ifndef _SEND_CHAR_ETC_H_
#define _SEND_CHAR_ETC_H_

#include <Network/Packet/PacketBase.h>


// ���� ����
class   CSendStream;
class	CCharacter;

namespace Item
{
	struct ItemPos;
}

namespace GameClientSendPacket
{
	// ĳ���� ��ų Ŀ�ǵ�
    bool SendCharSkillCommand(CSendStream& SendStream, unsigned long dwCharID, 
		PktBase::CMDType Cmd, unsigned char cIndex, unsigned short usSkillID, unsigned short usError, unsigned char cSkillLevel = 0, Item::ItemPos* lpItem = NULL);
};

#endif