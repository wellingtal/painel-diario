#include "stdafx.h"
#include "MultiThreadDispatch.h"

#include <Network/Session/Session.h>
#include <Stream/Buffer/Buffer.h>
#include <Stream/Buffer/BufferFactory.h>

#include <mmsystem.h>


CRylServerMultiThreadDispatch::CRylServerMultiThreadDispatch(
    CSession& Session, unsigned long dwMaxProcessPacketPerPulse)
:   CRylServerDispatch(Session, dwMaxProcessPacketPerPulse)
{

}

CRylServerMultiThreadDispatch::~CRylServerMultiThreadDispatch()
{

}


bool CRylServerMultiThreadDispatch::ParsePacket(char* const lpStream_In, 
                                                unsigned long* dwStreamSize_InOut)
{
    return CRylServerDispatch::ParsePacket(lpStream_In, dwStreamSize_InOut) ? 
        Dispatch() : false;
}

