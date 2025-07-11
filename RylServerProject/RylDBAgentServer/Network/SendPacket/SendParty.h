#ifndef _DBAGENT_SERVER_SEND_PARTY_H_
#define _DBAGENT_SERVER_SEND_PARTY_H_

// forward decl.
class CSendStream;
class CPartyDBInfo;

namespace DBAgent
{
    namespace SendPacket
    {        
        bool DeleteParty(CPartyDBInfo* lpParty);

		bool PartyCmd(CSendStream& SendStream, unsigned short cSubCmd, unsigned long dwPID, unsigned long dwSenderID, unsigned long dwGID, 
						unsigned short wClass, unsigned long dwServerID, char cLevel, const char* szSenderName, unsigned short usError);

		bool PartyCmd(unsigned short cSubCmd, unsigned long dwPID, unsigned long dwSenderID, unsigned long dwGID, 
						unsigned short wClass, unsigned long dwServerID, char cLevel, const char* szSenderName);
    }
}


#endif
