#ifndef _MULTITHREAD_PKT_DISPATCH_H_
#define _MULTITHREAD_PKT_DISPATCH_H_

#include "RylServerDispatch.h"

// ���� ����
class CSession;
struct PktBase;

// ��Ŷ ó���� ���� �����忡�� �Ѵ�. 
// ��, �� ������ �ѹ��� �� �����常 �����ؼ� ó���Ѵ�.
class CRylServerMultiThreadDispatch : public CRylServerDispatch
{
public:
    
    virtual bool ParsePacket(char* const lpStream_In, unsigned long* dwStreamSize_InOut);    

protected:
    
    CRylServerMultiThreadDispatch(CSession& Session, unsigned long dwMaxProcessPacketPerPulse);
    virtual ~CRylServerMultiThreadDispatch();
};

#endif