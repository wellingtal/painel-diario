#include "stdafx.h"

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include "Overlapped.h"
#include "../Session/Session.h"
#include "../Listener/Listener.h"
#include "../../Stream/Buffer/Buffer.h"
#include "../../Stream/Buffer/BufferFactory.h"

#include <boost/pool/pool.hpp>


COverlapped::COverlapped(COverlappedFactory& ovlFactory, CBuffer* lpBuffer)
:   m_lpBuffer(lpBuffer), m_ovlFactory(ovlFactory)
{
    Internal = InternalHigh = Offset = OffsetHigh = 0;
    hEvent   = 0;
}


COverlapped::~COverlapped()
{
	SAFE_RELEASE_BUFFER(m_lpBuffer);
}

void COverlapped::Release()
{
    m_ovlFactory.DeleteOverlapped(this);
}

class CSendOverlapped : public COverlapped
{
public:

    CSendOverlapped(COverlappedFactory& ovlFactory, CBuffer* lpSendBuffer) 
		: COverlapped(ovlFactory, lpSendBuffer) { }
	virtual ~CSendOverlapped() { }

    virtual void Dispatch(BOOL bResult, ULONG_PTR lpSessionKey, unsigned long dwProcessedBytes)
	{
		CSession& Session = *reinterpret_cast<CSession*>(lpSessionKey);
		
		Session.SendCompleted(bResult, dwProcessedBytes);
		m_ovlFactory.DeleteOverlapped(this);
	}
};


class CStreamRecvOverlapped : public COverlapped
{
public:
    
	CStreamRecvOverlapped(COverlappedFactory& ovlFactory) : COverlapped(ovlFactory, 0) { }
    virtual ~CStreamRecvOverlapped() { }

    virtual void Dispatch(BOOL bResult, ULONG_PTR lpSessionKey, unsigned long dwProcessedBytes)
	{
		CSession& Session = *reinterpret_cast<CSession*>(lpSessionKey);
		
		if(Session.Dispatch(dwProcessedBytes))
		{
			Session.Recv();
		}
	    
		m_ovlFactory.DeleteOverlapped(this);
	}
};


class CDatagramRecvOverlapped : public COverlapped
{
public:

    CDatagramRecvOverlapped(COverlappedFactory& ovlFactory) : COverlapped(ovlFactory, 0) { }
    virtual ~CDatagramRecvOverlapped() { }

    virtual void Dispatch(BOOL bResult, ULONG_PTR lpSessionKey, unsigned long dwProcessedBytes)
	{
		CSession& Session = *reinterpret_cast<CSession*>(lpSessionKey);
		
		if(Session.Dispatch(dwProcessedBytes))
		{
			Session.RecvFrom();
		}
	    
		m_ovlFactory.DeleteOverlapped(this);
	}
};


class CAcceptOverlapped : public COverlapped
{
public:
    
    CAcceptOverlapped(COverlappedFactory& ovlFactory, CListener& Listener, SOCKET hSocket, CBuffer* lpAddrBuffer) 
        :   COverlapped(ovlFactory, lpAddrBuffer), m_Listener(Listener), m_hSocket(hSocket)
	{ 
	
	}

    virtual ~CAcceptOverlapped() { }
    
    virtual void Dispatch(BOOL bResult, ULONG_PTR lpSessionKey, unsigned long dwProcessedBytes)
	{
		CListener& Listener = *reinterpret_cast<CListener*>(lpSessionKey);
		
		Listener.ProcessAccept(bResult, m_hSocket, m_lpBuffer, dwProcessedBytes);
		m_ovlFactory.DeleteOverlapped(this);
	}

private:

	CListener&	m_Listener;
	SOCKET		m_hSocket;
};


// ----------------------------------------------------------------------
// StreamOverlapped

CStreamOverlappedFactory::CStreamOverlappedFactory()
{
	FactoryLock::Syncronize sync(m_Lock);

    size_t nMaxPoolSize = std::max(sizeof(CAcceptOverlapped),
        std::max(sizeof(CSendOverlapped), sizeof(CStreamRecvOverlapped)));

    m_lpOverlappedPool = new (std::nothrow) boost::pool<>(nMaxPoolSize);
}


CStreamOverlappedFactory::~CStreamOverlappedFactory()
{
	FactoryLock::Syncronize sync(m_Lock);
    
	delete m_lpOverlappedPool;
	m_lpOverlappedPool = 0;
}


COverlapped* CStreamOverlappedFactory::CreateSend(CSession* lpSession, CBuffer* lpMsgBlock)
{
    COverlapped* lpOverlapped = 0;

	FactoryLock::Syncronize sync(m_Lock);

    if(0 != lpSession && 0 != lpMsgBlock && 0 != m_lpOverlappedPool)
	{
		void* lpBuffer = m_lpOverlappedPool->malloc();

		if(0 != lpBuffer)
		{
            lpOverlapped = new (lpBuffer) CSendOverlapped(*this, lpMsgBlock);
		}
	}

	return lpOverlapped;
}

COverlapped* CStreamOverlappedFactory::CreateRecv(CSession* lpSession, CBuffer* lpMsgBlock)
{
    COverlapped* lpOverlapped = 0;

	FactoryLock::Syncronize sync(m_Lock);

    if(0 != lpSession && 0 != lpMsgBlock && 0 != m_lpOverlappedPool)
	{
		void* lpBuffer = m_lpOverlappedPool->malloc();
		
		if(0 != lpBuffer)
		{
            lpOverlapped = new (lpBuffer) CStreamRecvOverlapped(*this); 
		}
	}

	return lpOverlapped;
}

COverlapped* CStreamOverlappedFactory::CreateAccept(CListener* lpListener, SOCKET hSocket, CBuffer* lpMsgBlock)
{
    COverlapped* lpOverlapped = 0;

	FactoryLock::Syncronize sync(m_Lock);

    if(0 != lpListener && 0 != lpMsgBlock && 0 != m_lpOverlappedPool)
	{
		void* lpBuffer = m_lpOverlappedPool->malloc();

		if(0 != lpBuffer)
		{
            lpOverlapped = new (lpBuffer) CAcceptOverlapped(*this, *lpListener, hSocket, lpMsgBlock); 
		}
	}

	return lpOverlapped;
}

void CStreamOverlappedFactory::DeleteOverlapped(COverlapped* lpOverlapped)
{
	FactoryLock::Syncronize sync(m_Lock);

    if(0 != lpOverlapped && 0 != m_lpOverlappedPool)
    {
        lpOverlapped->~COverlapped();
        m_lpOverlappedPool->free(lpOverlapped);
    }
}

// ----------------------------------------------------------------------
// DatagramOverlapped


CDatagramOverlappedFactory::CDatagramOverlappedFactory()
{
	FactoryLock::Syncronize sync(m_Lock);

    size_t nMaxPoolSize = std::max(sizeof(CAcceptOverlapped),
        std::max(sizeof(CSendOverlapped), sizeof(CDatagramRecvOverlapped)));

    m_lpOverlappedPool = new (std::nothrow) boost::pool<>(nMaxPoolSize);
}


CDatagramOverlappedFactory::~CDatagramOverlappedFactory()
{
    FactoryLock::Syncronize sync(m_Lock);

    delete m_lpOverlappedPool;
	m_lpOverlappedPool = 0;
}



COverlapped* CDatagramOverlappedFactory::CreateSend(CSession* lpSession, CBuffer* lpMsgBlock)
{
    COverlapped* lpOverlapped = 0;

	FactoryLock::Syncronize sync(m_Lock);

    if(0 != lpSession && 0 != lpMsgBlock && 0 != m_lpOverlappedPool)
	{
		void* lpBuffer = m_lpOverlappedPool->malloc();
		
		if(0 != lpBuffer)
		{
            lpOverlapped = new (lpBuffer) CSendOverlapped(*this, lpMsgBlock); 
		}
	}

	return lpOverlapped;
}

COverlapped* CDatagramOverlappedFactory::CreateRecv(CSession* lpSession, CBuffer* lpMsgBlock)
{
    COverlapped* lpOverlapped = 0;

	FactoryLock::Syncronize sync(m_Lock);

    if(0 != lpSession && 0 != lpMsgBlock && 0 != m_lpOverlappedPool)
	{
		void* lpBuffer = m_lpOverlappedPool->malloc();
		
		if(0 != lpBuffer)
		{
			lpOverlapped = new (lpBuffer) CDatagramRecvOverlapped(*this); 
		}
	}

	return lpOverlapped;
}

void CDatagramOverlappedFactory::DeleteOverlapped(COverlapped* lpOverlapped)
{
	FactoryLock::Syncronize sync(m_Lock);

    if(0 != lpOverlapped && 0 != m_lpOverlappedPool)
    {
        lpOverlapped->~COverlapped();
        m_lpOverlappedPool->free(lpOverlapped);
    }
}
