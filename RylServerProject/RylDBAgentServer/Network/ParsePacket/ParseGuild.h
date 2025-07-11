#ifndef _DBAGENT_SERVER_PARSE_GUILD_H_
#define _DBAGENT_SERVER_PARSE_GUILD_H_

// forward delc.
class CSendStream;
struct PktBase;

namespace DBAgent
{
    namespace ParseGuild
    {
        bool CreateGuild(CSendStream& SendStream, PktBase* lpPktBase);
        bool GuildCmd(CSendStream& SendStream, PktBase* lpPktBase);
        bool GuildMark(CSendStream& SendStream, PktBase* lpPktBase);
        bool GuildLevel(CSendStream& SendStream, PktBase* lpPktBase);
        bool GuildRelation(CSendStream& SendStream, PktBase* lpPktBase);
        bool GuildInclination(CSendStream& SendStream, PktBase* lpPktBase);
        bool SetGuildRight(CSendStream& SendStream, PktBase* lpPktBase);
        bool GuildSafe(CSendStream& SendStream, PktBase* lpPktBase);
        bool GuildMemberInfoUpdate(CSendStream& SendStream, PktBase* lpPktBase);
    }
}

#endif

