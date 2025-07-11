#ifndef _DBAGENT_SERVER_SEND_GUILD_H_
#define _DBAGENT_SERVER_SEND_GUILD_H_

// forward decl.
class CSendStream;

namespace DBAgent
{
    namespace SendPacket
    {        
        bool GuildCmd(CSendStream* lpSendStream, 
            unsigned long dwGID, unsigned long dwSenderID, unsigned long dwReferenceID, 
            unsigned short wCmd, const char* szSenderName, const char* szGuildName, unsigned short wError);

        bool GuildMemberFameUpdate(unsigned long dwCID, unsigned long dwOurGID,
			unsigned char cType, int nAddFame, unsigned char cZone, unsigned long dwEnemyGID = 0);

		bool GuildMemberGoldUpdate(unsigned long dwCID,
			unsigned long dwGID, unsigned long dwGold, unsigned char cZone);
    }
}

#endif