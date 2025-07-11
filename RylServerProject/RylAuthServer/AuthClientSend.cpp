#include "stdafx.h"
#include "AuthClientDispatch.h"
#include "AuthAgentServerTable.h"

#include <Network/Session/Session.h>
#include <Network/Packet/WrapPacket.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/ClientToAuthServer.h>
#include <Network/Packet/PacketStruct/ServerPacket.h>

#include <Stream/Buffer/BufferFactory.h>

bool CAuthClientDispatch::SendCharSelect(CSendStream& SendStream,  unsigned char Zone, 
                                         unsigned short *lpChannelNum, unsigned short usError_In)
{
    char* lpBuffer = SendStream.GetBuffer(sizeof(PktCSAck));
    if(NULL != lpBuffer)
    {
        PktCSAck* lpPktCSAck = reinterpret_cast<PktCSAck*>(lpBuffer);
	    
		lpPktCSAck->m_cZone = Zone;
		if(NULL != lpChannelNum)
		{
			memcpy(lpPktCSAck->m_wChannelNum, lpChannelNum, sizeof(unsigned short) * PktCSAck::MAX_CHANNEL_NUM);
		}

		return SendStream.WrapCrypt(sizeof(PktCSAck), CmdCharSelect, 0, usError_In);
    }

    return false;
}


bool CAuthClientDispatch::SendCharCreate(CSendStream& SendStream, unsigned long CharID_In, unsigned long Slot_In, 
                                         CHAR_VIEW* lpCharView_In, unsigned short usError_In)
{
    char* lpBuffer = SendStream.GetBuffer(sizeof(PktCCAck));
    if(NULL != lpBuffer)
    {
        PktCCAck* lpPktCCAck = reinterpret_cast<PktCCAck*>(lpBuffer);

        if(0 == usError_In)
        {
            lpPktCCAck->m_dwCharID  = CharID_In;
            lpPktCCAck->m_dwSlotNum = Slot_In;
            lpPktCCAck->m_CharView  = *lpCharView_In;
        }
        else
	    {
		    lpPktCCAck->m_dwCharID  = CharID_In;
		    lpPktCCAck->m_dwSlotNum = Slot_In;
		    memset(&lpPktCCAck->m_CharView, 0, sizeof(CHAR_VIEW));
	    }

        return SendStream.WrapCrypt(sizeof(PktCCAck), CmdCharCreate, 0, usError_In);			
    }

    return false;
}


bool CAuthClientDispatch::SendCharDelete(CSendStream& SendStream, unsigned long UserID_In, unsigned long Slot_In, unsigned short usError_In)
{
    char* lpBuffer = SendStream.GetBuffer(sizeof(PktCDAck));
    if(NULL != lpBuffer)
    {	    
	    PktCDAck* lpPktCDAck = reinterpret_cast<PktCDAck*>(lpBuffer);

        if(0 == usError_In)
	    {
		    lpPktCDAck->m_dwUserID	= UserID_In;
		    lpPktCDAck->m_dwSlotNum	= Slot_In;
	    }
	    else
	    {
		    lpPktCDAck->m_dwUserID  = 0;
		    lpPktCDAck->m_dwSlotNum = 0;
	    }

        return SendStream.WrapCrypt(sizeof(PktCDAck), CmdCharDelete, 0, usError_In);
    }

    return false;
}

// WORK_LIST 2.1 계정 국적 추가
bool CAuthClientDispatch::SendSelectAccountNation(CSendStream& SendStream, unsigned long UserID_In,
												  unsigned char cType, unsigned char cAccountNation_In, unsigned short usError_In)
{
	char* lpBuffer = SendStream.GetBuffer(sizeof(PktSelectAccountNation));
	if (NULL != lpBuffer)
	{	    
		PktSelectAccountNation* lpPktSAN = reinterpret_cast<PktSelectAccountNation*>(lpBuffer);

		if (0 == usError_In)
		{
			lpPktSAN->m_dwUserID		= UserID_In;
			lpPktSAN->m_cType			= cType;
			lpPktSAN->m_cAccountNation	= cAccountNation_In;
		}
		else
		{
			lpPktSAN->m_dwUserID		= 0;
			lpPktSAN->m_cType			= 0;
			lpPktSAN->m_cAccountNation	= 0;
		}

		return SendStream.WrapCrypt(sizeof(PktSelectAccountNation), CmdSelectAccountNation, 0, usError_In);
	}

	return false;
}

// WORK_LIST 2.3 계정 국적 변경 기능 구현
bool CAuthClientDispatch::SendNationChangeResult(CSendStream& SendStream, unsigned long UserID_In,
				 							     unsigned long* dwGID, unsigned long* dwFame, unsigned short usError_In)
{
	char* lpBuffer = SendStream.GetBuffer(sizeof(PktNationChangeResult));
	if (NULL != lpBuffer)
	{	    
		PktNationChangeResult* lpPktANC = reinterpret_cast<PktNationChangeResult*>(lpBuffer);

		if (0 == usError_In)
		{
			lpPktANC->m_dwUserID = UserID_In;
			::memcpy(lpPktANC->m_dwGID, dwGID, sizeof(unsigned long) * PktNationChangeResult::MAX_CHAR_VIEW);
			::memcpy(lpPktANC->m_dwFame, dwFame, sizeof(unsigned long) * PktNationChangeResult::MAX_CHAR_VIEW);
		}
		else
		{
			lpPktANC->m_dwUserID = 0;
			::memset(lpPktANC->m_dwGID, 0, sizeof(unsigned long) * PktNationChangeResult::MAX_CHAR_VIEW);
			::memset(lpPktANC->m_dwFame, 0, sizeof(unsigned long) * PktNationChangeResult::MAX_CHAR_VIEW);
		}

		return SendStream.WrapCrypt(sizeof(PktNationChangeResult), CmdNationChangeResult, 0, usError_In);
	}

	return false;
}

