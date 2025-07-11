#ifndef _COVERLAPPED_H_
#define _COVERLAPPED_H_

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "../../Thread/Lock.h"

#include <winsock2.h>
#include <windows.h>
#include <boost/pool/poolfwd.hpp>

// 전방 참조
class CBuffer;
class CBufferFactory;
class CSession;
class CListener;
class COverlappedFactory;

class COverlapped : public OVERLAPPED
{
public:

    COverlapped(COverlappedFactory& ovlFactory, CBuffer* lpBuffer);
    virtual ~COverlapped();

    virtual void Dispatch(BOOL bResult, ULONG_PTR lpSessionKey, 
		unsigned long dwProcessedBytes) = 0;

    void SetBuffer(CBuffer* lpBuffer) { m_lpBuffer = lpBuffer; }
    void Release();

protected:
    
	COverlappedFactory&	m_ovlFactory;
    CBuffer*			m_lpBuffer;	
};


// ---------------------------------------------------------------------------
// COverlappedFactory : 인터페이스 팩토리

class COverlappedFactory
{
public:
	
	virtual ~COverlappedFactory() { }
	
	virtual COverlapped* CreateSend(CSession* lpSession, CBuffer* lpMsgBlock) = 0;
	virtual COverlapped* CreateRecv(CSession* lpSession, CBuffer* lpMsgBlock) = 0;
	virtual COverlapped* CreateAccept(CListener* lpListener, SOCKET hSocket, CBuffer* lpMsgBlock) = 0;

    virtual void DeleteOverlapped(COverlapped* lpOverlapped) = 0;
};


// ---------------------------------------------------------------------------
// CStreamOverlappedFactory : TCP, SPX통신에 사용한다.

class CStreamOverlappedFactory : public COverlappedFactory
{
public:

    CStreamOverlappedFactory();
    virtual ~CStreamOverlappedFactory();

	virtual COverlapped* CreateSend(CSession* lpSession, CBuffer* lpMsgBlock);
	virtual COverlapped* CreateRecv(CSession* lpSession, CBuffer* lpMsgBlock);
	virtual COverlapped* CreateAccept(CListener* lpListener, SOCKET hSocket, CBuffer* lpMsgBlock);

    virtual void DeleteOverlapped(COverlapped* lpOverlapped);

private:

	typedef CCSLock	FactoryLock;

	FactoryLock		m_Lock;
	CACHE_PAD(FactoryLockPad, sizeof(FactoryLock));

    boost::pool<>*  m_lpOverlappedPool;
};


// ---------------------------------------------------------------------------
// CDatagramOverlappedFactory  : UDP, IPX통신에 사용한다.

class CDatagramOverlappedFactory : public COverlappedFactory
{
public:

    CDatagramOverlappedFactory();
    virtual ~CDatagramOverlappedFactory();

	virtual COverlapped* CreateSend(CSession* lpSession, CBuffer* lpMsgBlock);
	virtual COverlapped* CreateRecv(CSession* lpSession, CBuffer* lpMsgBlock);
	virtual COverlapped* CreateAccept(CListener* lpListener, SOCKET hSocket, CBuffer* lpMsgBlock) { return 0; }

    virtual void DeleteOverlapped(COverlapped* lpOverlapped);

private:

	typedef CCSLock	FactoryLock;

	FactoryLock		m_Lock;
	CACHE_PAD(FactoryLockPad, sizeof(FactoryLock));

    boost::pool<>*  m_lpOverlappedPool;
};


#endif