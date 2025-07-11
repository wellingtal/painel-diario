#ifndef _DBAGENT_SERVER_SEND_MOVE_ZONE_H_
#define _DBAGENT_SERVER_SEND_MOVE_ZONE_H_


// forward decl.
class CSendStream;

namespace DBAgent
{
namespace SendPacket
{
    bool CharMoveZone(CSendStream& SendStream, unsigned long dwRequestKey, 
        unsigned char cZone, unsigned short usError);

    bool ServerZone(CSendStream& SendStream, unsigned long dwRequestKey, 
        unsigned char cZone, unsigned char cChannel, unsigned short usError = 0);

    bool ServerZoneEnd(CSendStream& SendStream, unsigned long dwRequestKey);

}
}


#endif