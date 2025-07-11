#ifndef _MULTITHREAD_PKT_DISPATCH_H_
#define _MULTITHREAD_PKT_DISPATCH_H_

#include "RylServerDispatch.h"

// 전방 참조
class CSession;
struct PktBase;

// 패킷 처리를 여러 스레드에서 한다. 
// 단, 한 세션을 한번에 한 스레드만 접근해서 처리한다.
class CRylServerMultiThreadDispatch : public CRylServerDispatch
{
public:
    
    virtual bool ParsePacket(char* const lpStream_In, unsigned long* dwStreamSize_InOut);    

protected:
    
    CRylServerMultiThreadDispatch(CSession& Session, unsigned long dwMaxProcessPacketPerPulse);
    virtual ~CRylServerMultiThreadDispatch();
};

#endif