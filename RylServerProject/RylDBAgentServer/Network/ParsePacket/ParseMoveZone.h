#ifndef _DBAGENT_SERVER_PARSE_MOVE_ZONE_H_
#define _DBAGENT_SERVER_PARSE_MOVE_ZONE_H_

// forward delc.
class CSendStream;
struct PktBase;

namespace DBAgent
{
namespace ParseMoveZone
{
    bool Parse(CSendStream& SendStream, PktBase* lpPktBase);
}
}


#endif