#include "stdafx.h"
#include "SessionPolicy.h"

#include "../IOCP/Overlapped.h"
#include "../Dispatch/Dispatch.h"
#include "../Winsock/SocketFactory.h"
#include "../../Stream/Buffer/BufferFactory.h"

CSessionPolicy::CSessionPolicy(CSocketFactory* lpSocketFactory,
							   CBufferFactory* lpBufferFactory, 
							   CDispatchFactory* lpDispatchFactory, 
							   COverlappedFactory* lpOverlappedFactory)
							   :	m_lpSocketFactory(lpSocketFactory),
									m_lpBufferFactory(lpBufferFactory),
									m_lpDispatchFactory(lpDispatchFactory),
									m_lpOverlappedFactory(lpOverlappedFactory),
									m_nRefCount(1)
{
    
}

CSessionPolicy::~CSessionPolicy() 
{   
	delete m_lpSocketFactory;
    delete m_lpBufferFactory;
    delete m_lpDispatchFactory;
    delete m_lpOverlappedFactory;
}


LONG CSessionPolicy::AddRef()
{
    return InterlockedIncrement(&m_nRefCount);
}

LONG CSessionPolicy::Release()
{
    LONG nRefCount = InterlockedDecrement(&m_nRefCount);
    if(0 == nRefCount)
    {
        delete this;
    }

    return nRefCount;
}
