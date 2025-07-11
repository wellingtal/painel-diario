#include "stdafx.h"
#include "Commands.h"
#include <Creature/CreatureManager.h>
#include <Creature/Character/Character.h>
#include <Network/Dispatch/GameClient/GameClientDispatch.h>
#include <Network/Dispatch/Chat/ChatDispatch.h>

#include <Network/Packet/ChatPacket.h>
#include <Network/Packet/WrapPacket.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/CharCommunityPacket.h>
#include <Network/Stream/SendStream.h>


CConsoleCommand* CCMDNotify::Clone(const char* szCommand, size_t nCommandLength)
{
    size_t nIndex = 0;

    // 첫번째 whitespace인 곳 까지.
    for (; nIndex < nCommandLength; ++nIndex)
    {
        if (0 == szCommand[nIndex] || ' ' == szCommand[nIndex] || 
			'\t' == szCommand[nIndex] || '\n' == szCommand[nIndex])
        {
            break;
        }
    }

    ++nIndex;

    // whitespace가 끝나는 곳 까지
    for (; nIndex < nCommandLength; ++nIndex)
    {
        if (0 != szCommand[nIndex] || ' ' != szCommand[nIndex] || 
			'\t' != szCommand[nIndex] || '\n' != szCommand[nIndex])
        {
            break;
        }
    }

    CCMDNotify* lpNotify = NULL;

    if(nIndex < nCommandLength)
    {
        lpNotify = new CCMDNotify;
       
        if(NULL != lpNotify)
        {			
			if (GameRYL::KOREA == CServerSetup::GetInstance().GetNationType())
			{
				lpNotify->m_nLength = _snprintf(lpNotify->m_szBuffer, 
					PktChat::PktChatMaxSize - 1, "[운영자 공지] : %s", szCommand + nIndex);
			}
			else
			{
				lpNotify->m_nLength = _snprintf(lpNotify->m_szBuffer, 
					PktChat::PktChatMaxSize - 1, "%s", szCommand + nIndex);
			}

            lpNotify->m_szBuffer[PktChat::PktChatMaxSize - 1] = 0;
		}	
    }

    return lpNotify;    
}

bool CCMDNotify::DoProcess()
{
	if(0 < m_nLength)
	{        
        CChatPacket chatPacket(m_szBuffer, 0, PktChat::NOTICE, 0);

        if(chatPacket.IsValid())
        {
            CCreatureManager::GetInstance().SendAllCharacter(chatPacket.GetCompressedPacket(),
                chatPacket.GetCompressedSize(), CmdCharChat);
        }
	}

	return true;
}