#include "stdafx.h"
#include "Character.h"
#include <Creature/CreatureManager.h>
#include <Network/Dispatch/GameClient/GameClientDispatch.h>

#include <Network/Packet/ChatPacket.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/CharCommunityPacket.h>

#include <mmsystem.h>


bool CCharacter::CanShout() const 
{
    const long MAX_SHOUT_DELAY = 15000;
    unsigned long dwCurrentTime = timeGetTime();
    
    return (0 == m_dwLastShoutTime || MAX_SHOUT_DELAY < dwCurrentTime - m_dwLastShoutTime);
}

void CCharacter::Shouted()
{
    m_dwLastShoutTime = timeGetTime();

    if(0 == m_dwLastShoutTime)
    {
        ++m_dwLastShoutTime;
    }
}


