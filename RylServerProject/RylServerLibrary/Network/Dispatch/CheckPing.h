#ifndef _CHECK_PING_H_
#define _CHECK_PING_H_

#include <Thread/Lock.h>

class CCheckPing
{
public:

    CCheckPing();
        
    enum { PING_INTERVAL = 10000 }; // 10√ 

    bool CheckPing(unsigned long dwCurrentTime);
    void SetLastPingRecvTime(unsigned long dwCurrentTime);

    unsigned long GetLastPingRecvTime();
    unsigned long GetPingCount();

    void GetPingData(unsigned long& dwPingCount, 
        unsigned long& dwLastPingRecvTime, unsigned long& dwFirstCheckTime);

private:
    
    typedef CCSLock CheckPingLock;
    CheckPingLock   m_PingLock;
    CACHE_PAD(CheckPingLockPad, sizeof(CheckPingLock));

    unsigned long   m_dwPingCount;
    unsigned long   m_dwLastPingRecvTime;
    unsigned long   m_dwFirstCheckTime;    
};

#endif