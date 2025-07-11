#ifndef _DBAGENT_SERVER_PARSE_PARTY_H_
#define _DBAGENT_SERVER_PARSE_PARTY_H_

// forward delc.
class CSendStream;
struct PktBase;

namespace DBAgent
{
namespace ParseParty
{
    bool Parse(CSendStream& SendStream, PktBase* lpPktBase);
}
}


#endif