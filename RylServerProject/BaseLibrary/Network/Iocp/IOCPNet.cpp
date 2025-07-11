#include "stdafx.h"

#include "IOCPNet.h"
#include "IOWorker.h"

#include "CompletionHandler.h"
#include "../../Thread/ThreadMgr.h"
#include "../../Stream/Buffer/BufferFactory.h"


#include "../Session/Session.h"
#include "../Session/SessionMgr.h"
#include "../Listener/Listener.h"

#include "../Dispatch/Dispatch.h"
#include "../Winsock/SocketFactory.h"
#include "../../Utility/Resource/EnsureCleanup.h"
#include "../../Utility/Debug/DebugUtils.h"

#include "../../Log/ServerLog.h"

#include <mmsystem.h>


enum IOCPInternalFlag
{
    INITIALIZED_IOCP    = (1 << 0),
    DESTROYED_IOCP      = (1 << 1)
};


unsigned long GetOptimalThreadNum()
{
	unsigned long dwThreadCount;
	SYSTEM_INFO	systemInfo;

	GetSystemInfo(&systemInfo);
	dwThreadCount = systemInfo.dwNumberOfProcessors * 2;

	return dwThreadCount;
}


CIOCPNet::CIOCPNet()
:   m_dwFlags(0),
    m_lpSocketHandler(new (std::nothrow) CCompletionHandler),
    m_lpThreadMgr(new (std::nothrow) CThreadMgr),
	m_lpSessionMgr(new (std::nothrow) CSessionMgr)
{
    WSADATA wsaData;
    WSAStartup(0x0202, &wsaData);
}


CIOCPNet::~CIOCPNet()
{
    Destroy();
    WSACleanup();
}


// Desc : 초기화 및 스레드 생성.
bool CIOCPNet::Initialize()
{
    {
        SessionLock::Syncronize sync(m_IOCPLock);

        // 초기화는 한번만 할 수 있음.
        if(INITIALIZED_IOCP & m_dwFlags)
        {
            return false;
        }        
        
        m_dwFlags |= INITIALIZED_IOCP;        
    }

    if(0 == m_lpSocketHandler || 
		0 == m_lpThreadMgr ||
		0 == m_lpSessionMgr ||
		!m_lpSocketHandler->Initialize())
    {
        return false;
    }

    // IOCP 스레드 생성.
	unsigned long dwThreadNum = GetOptimalThreadNum();
	while(0 != dwThreadNum--) 
	{
		if(!m_lpThreadMgr->RegisterAndRun(
            new (std::nothrow) CIOWorker(*m_lpSocketHandler))) 
		{			
			return false;
		}
	}
    
    return true;
}


// Desc : Listener를 생성한다.
bool CIOCPNet::AddListener(CSessionPolicy* lpSessionPolicy, 						   
                           const char* lpListenAddress, unsigned short usPort,                            
                           unsigned long dwMaxListenPeding,
						   CValidateConnection* lpValidateConnection)
{
    if(0 == lpSessionPolicy || !lpSessionPolicy->IsValid() || 
		0 == m_lpSocketHandler || 0 == m_lpSessionMgr)
    {
        return false;
    }

    CListener* lpListener = new (std::nothrow) CListener(*m_lpSocketHandler, 
		*lpSessionPolicy, *m_lpSessionMgr, lpValidateConnection); 

    if(0 == lpListener)
    {
        return false;
    }
	
    if(!lpListener->Initialize(lpListenAddress, usPort, dwMaxListenPeding))
    {
        delete lpListener;
        return false;
    }

	lpSessionPolicy->AddRef();

    SessionLock::Syncronize sync(m_IOCPLock);
    m_ListenerList.push_back(lpListener);
	++m_SessionPolicyMap[lpSessionPolicy];
	return true;
}


// Desc : 연결 세션을 생성한다.
bool CIOCPNet::Connect(CSessionPolicy* lpSessionPolicy, 
					   const char* lpConnectAddress, unsigned short usPort)
{
    if(0 == lpSessionPolicy || !lpSessionPolicy->IsValid() || 
		0 == m_lpSocketHandler || 0 == m_lpSessionMgr)
    {
        return false;
    }

	SOCKET hConnectedSocket = lpSessionPolicy->GetSocketFactory().CreateConnectedSocket(lpConnectAddress, usPort);
	if(INVALID_SOCKET == hConnectedSocket)
	{
		ERRLOG3(g_Log, "Connected socket creation error : %d, destination : %s:%d",
			WSAGetLastError(), lpConnectAddress, usPort);
	}
	else
	{
		CSession* lpSession = m_lpSessionMgr->CreateSession(*lpSessionPolicy);

		if(0 != lpSession)
		{
			if(m_lpSocketHandler->AttachToHander(
				reinterpret_cast<HANDLE>(hConnectedSocket), 
				reinterpret_cast<ULONG_PTR>(lpSession)))
			{
				lpSession->Socket(hConnectedSocket);
				lpSession->SetAddress(INET_Addr(lpConnectAddress, usPort), INET_Addr());

				DETLOG3(g_SessionLog, "SP:0x%p/DP:0x%p/IP:%15s/ Connect Success.", 
					lpSession, lpSession->GetDispatch(), lpConnectAddress);

				lpSession->InternalRecv();

				m_lpSessionMgr->Add(lpSession);

				lpSessionPolicy->AddRef();

				SessionLock::Syncronize sync(m_IOCPLock);
				++m_SessionPolicyMap[lpSessionPolicy];
				return true;
			}

			m_lpSessionMgr->DeleteSession(lpSession);
		}
		
		closesocket(hConnectedSocket);	
	}
	
    return false;
}


// Desc : Listener에 딸린 세션들과, ConnectedSession의 처리를 한다.
void CIOCPNet::Process()
{
	if(0 != m_lpSessionMgr)
	{
		m_lpSessionMgr->Process();
	}    
}


// Desc : 모든 Listener와 세션을 종료한다.
bool CIOCPNet::Destroy()
{
	m_IOCPLock.Lock();

    if(!(m_dwFlags & DESTROYED_IOCP))
    {
		CThreadMgr*			lpThreadMgr = 0;
		CCompletionHandler* lpSocketHandler = 0;
		CSessionMgr*		lpSessionMgr = 0;

		SessionPolicyMap tempMap(m_SessionPolicyMap);
		m_SessionPolicyMap.clear();

		std::swap(m_lpThreadMgr, lpThreadMgr);
		std::swap(m_lpSocketHandler, lpSocketHandler);
		std::swap(m_lpSessionMgr, lpSessionMgr);

		m_dwFlags |= DESTROYED_IOCP;
	
		m_IOCPLock.Unlock();

        DestroyListener();
        
		delete lpSessionMgr;
        delete lpThreadMgr;
        delete lpSocketHandler;

		SessionPolicyMap::iterator pos = tempMap.begin();
		SessionPolicyMap::iterator end = tempMap.end();

		for(; pos != end; ++pos)
		{
			for(unsigned int nReleaseCount = pos->second; 
				0 != nReleaseCount; --nReleaseCount)
			{
                pos->first->Release();
			}
		}

		return true;
    }

	m_IOCPLock.Unlock();
	return false;
}


void CIOCPNet::DestroyListener()
{
	ListenerList deleteList;

	{
        SessionLock::Syncronize sync(m_IOCPLock);
		deleteList.splice(deleteList.end(), m_ListenerList);
	}	

	ListenerList::iterator pos = deleteList.begin();
	ListenerList::iterator end = deleteList.end();

    for(; pos != end; ++pos)
    {        
        delete (*pos);
    }
}


// Desc : 현재 Accept를 pending하고 있는 세션의 수를 리턴.
unsigned long CIOCPNet::GetAcceptPendingNum()
{
    unsigned long dwPendingNum = 0;    

    SessionLock::Syncronize sync(m_IOCPLock);

    for(ListenerList::iterator itr = m_ListenerList.begin();
        itr != m_ListenerList.end(); ++itr)
    {    
        dwPendingNum += (*itr)->GetPendingAcceptNum();
    }

    return dwPendingNum;
}

unsigned long CIOCPNet::GetSessionNum()
{
	if(0 != m_lpSessionMgr)
	{
		return m_lpSessionMgr->GetSessionNum();
	}

	return 0;
}

/*
void CIOCPNet::LogListeners()
{
    SessionLock::Syncronize sync(m_IOCPLock);

    std::for_each(m_ListenerList.begin(), m_ListenerList.end(), 
        std::mem_fun(CListener::LogListenerStatus));
}


void CIOCPNet::LogConnected()
{
    using namespace std;

    unsigned int nStateNum[CSession::MAX_SESSION_STATE];
    std::fill_n(nStateNum, unsigned int(CSession::MAX_SESSION_STATE), 0);

    const char* szDelimiter = "// ------------------------------------------------------------------------------- ";

    char szProgramName[MAX_PATH];
    char szFileName[MAX_PATH];
    char szTime[MAX_PATH];

    SYSTEMTIME  systemTime;
    GetLocalTime(&systemTime);

    DbgUtils::SetProgramName(szProgramName, MAX_PATH);

    _snprintf(szTime, MAX_PATH, "%04d%02d%02d-%02d%02d%02d", 
        systemTime.wYear, systemTime.wMonth, systemTime.wDay, 
        systemTime.wHour, systemTime.wMinute, systemTime.wSecond);

    _snprintf(szFileName, MAX_PATH, "%s/ConnectedLog-%s.txt", szProgramName, szTime);

    fstream             logfile(szFileName, ios::out | ios::app);
    fstream::char_type  endline = logfile.widen('\n');

    logfile << szDelimiter << endline;

    m_IOCPLock.Lock();

    unsigned int nConnectedNum = 0;
    ConnectedArray::iterator first = m_ConnectedArray.begin();
    ConnectedArray::iterator last  = m_ConnectedArray.end();

    for(; first != last; ++first)
    {
        logfile << **first << endline;
        ++nStateNum[(*first)->m_cCurrentStatus];
        ++nConnectedNum;
    }

    m_IOCPLock.Unlock();

    logfile << endline << szDelimiter << endline
        << "Time           : " << szTime << endline << endline
        << "nUNINITIALIZED : " << nStateNum[CSession::UNINITIALIZED]  << endline
        << "ACCEPT_PENDING : " << nStateNum[CSession::ACCEPT_PENDING] << endline
        << "CONNECTED      : " << nStateNum[CSession::CONNECTED]      << endline
        << "DISCONNECTED   : " << nStateNum[CSession::DISCONNECTED]   << endline
        << "Total          : " << nConnectedNum << endl;
}

*/