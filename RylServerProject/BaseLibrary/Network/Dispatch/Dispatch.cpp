#include "stdafx.h"
#include "Dispatch.h"

#include "../Session/Session.h"
#include "../../Stream/Buffer/Buffer.h"
#include "../../Stream/Buffer/BufferFactory.h"

#include "PoolDispatchFactory.h"

#include "../../Log/ServerLog.h"

void CEchoDispatch::Connected()
{
	DETLOG1(g_Log, "this:0x%p Connected", this);
}

void CEchoDispatch::Disconnected()
{
	DETLOG1(g_Log, "this:0x%p Disconnected", this);
}

bool CEchoDispatch::ParsePacket (char* const lpStream_In, unsigned long* dwStreamSIze_InOut)    
{    
    unsigned long dwReceived = *dwStreamSIze_InOut;

    CBuffer* lpEchoBuffer = CREATE_BUFFER(m_Session.GetPolicy().GetBufferFactory(), dwReceived);
    memcpy(lpEchoBuffer->wr_ptr(), lpStream_In, dwReceived);
    lpEchoBuffer->wr_ptr(dwReceived);

    return m_Session.SendPending(lpEchoBuffer);
}
