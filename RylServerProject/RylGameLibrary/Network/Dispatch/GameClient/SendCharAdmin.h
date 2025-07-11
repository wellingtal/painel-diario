#ifndef _SEND_CHAR_ADMIN_H_
#define _SEND_CHAR_ADMIN_H_

#include <DB/DBdefine.h>
#include <Item/ItemStructure.h>


// 전방 참조
struct  PktBase;
class   CSendStream;


namespace GameClientSendPacket
{
	bool SendCharAdminCmdToDBAgent(CSendStream& AgentSendStream, PktBase* lpPktBase);

    bool SendCharNameChange(CSendStream& SendStream, unsigned long dwUID, unsigned long dwCID,
        const char* szChangeName, unsigned char cNameChangeCount, Item::ItemPos* lpItem_In, unsigned short usError);
};

#endif