#ifndef _DBAGENT_SERVER_PART1_DBAGENT_DISPATCH_H_
#define _DBAGENT_SERVER_PART1_DBAGENT_DISPATCH_H_

#include <Network/Dispatch/RylServerDispatch.h>
#include <Network/Dispatch/SingleDispatchStorage.h>
#include <Community/FriendList.h>
#include <Community/BanList.h>

namespace DBAgent
{
    // Part1 DBAgent서버에 접속한다. 

    class CPart1DBAgentDispatch : public CRylServerDispatch
    {
    public:

        static CSingleDispatch& GetDispatchTable();

        enum Const
        {
            MAX_PACKET_DISPATCH_PER_PULSE   = 100,
            MAX_STREAM_BUFFER_SIZE          = 16000
        };

        CPart1DBAgentDispatch(CSession& Session);
        virtual ~CPart1DBAgentDispatch();

        virtual void Connected();
        virtual void Disconnected();
        virtual bool DispatchPacket(PktBase* lpPktBase);
        
        static bool TransferCharPart1ToPart2(CSendStream& SendStream, unsigned long dwUID, 
            unsigned char cSelectedServerGroup, unsigned char cSelectedNation,
            unsigned long* lpdwSelectedCID, unsigned char cSelectedCharNum);

    private:
        
    };	

	typedef std::map<unsigned short, unsigned short>	QuestDelete;
	typedef std::map<unsigned short, unsigned short>	QuestChange;
}

#endif