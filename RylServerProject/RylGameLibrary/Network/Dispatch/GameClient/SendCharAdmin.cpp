#include "stdafx.h"

#include "SendCharAdmin.h"

#include <Network/Stream/SendStream.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/CharAdminPacket.h>
#include <Network/Packet/PacketStruct/CharItemPacketStruct.h>


bool GameClientSendPacket::SendCharAdminCmdToDBAgent(CSendStream& AgentSendStream, PktBase* lpPktBase)
{
	if (NULL == lpPktBase)
	{
		ERRLOG0(g_Log, "패킷이 NULL 입니다.");
		return false;
	}
    
	char* lpBuffer = AgentSendStream.GetBuffer(sizeof(PktAdmin));
	if (NULL != lpBuffer)
	{
		PktAdmin* lpPktAdmin = reinterpret_cast<PktAdmin*>(lpBuffer);

		lpPktAdmin->m_usCmd			= static_cast<PktAdmin*>(lpPktBase)->m_usCmd;
		strncpy(lpPktAdmin->m_stName, static_cast<PktAdmin*>(lpPktBase)->m_stName, PktAdmin::MAX_NAME_LEN);
		lpPktAdmin->m_usProtoTypeID	= static_cast<PktAdmin*>(lpPktBase)->m_usProtoTypeID;
		lpPktAdmin->m_Position		= static_cast<PktAdmin*>(lpPktBase)->m_Position;
		lpPktAdmin->m_dwAmount		= static_cast<PktAdmin*>(lpPktBase)->m_dwAmount;
		lpPktAdmin->m_dwAdminCID	= static_cast<PktAdmin*>(lpPktBase)->m_dwAdminCID;

		return AgentSendStream.WrapHeader(sizeof(PktAdmin), CmdCharAdminCmd, 0, PktBase::NO_SERVER_ERR);
	}

	return false;
}

bool GameClientSendPacket::SendCharNameChange(CSendStream& SendStream, unsigned long dwUID, unsigned long dwCID,
                                              const char* szChangeName, unsigned char cNameChangeCount, Item::ItemPos* lpItem_In, unsigned short usError)
{
    char* lpBuffer = SendStream.GetBuffer(sizeof(PktCharNameChange));

    if(0 != lpBuffer)
    {
        PktCharNameChange* lpPktCharNameChange = 
            reinterpret_cast<PktCharNameChange*>(lpBuffer);

        lpPktCharNameChange->m_dwUID = dwUID;
        lpPktCharNameChange->m_dwCID = dwCID;

        strncpy(lpPktCharNameChange->m_szCharName, szChangeName, CHAR_INFOST::MAX_NAME_LEN);
        lpPktCharNameChange->m_szCharName[CHAR_INFOST::MAX_NAME_LEN -1] = 0;

        lpPktCharNameChange->m_cNameChangeCount = cNameChangeCount;

		if(lpItem_In != NULL)
		{
			lpPktCharNameChange->m_ItemPos = *lpItem_In;
		}
		else
		{
			// 어드민 명령어.
			lpPktCharNameChange->m_ItemPos.m_cPos = TakeType::TS_ADMIN;
		}


        return SendStream.WrapCrypt(sizeof(PktCharNameChange), CmdCharNameChange, 0, usError);
    }

    return false;
}