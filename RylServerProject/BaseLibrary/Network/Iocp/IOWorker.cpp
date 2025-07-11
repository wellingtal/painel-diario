#include "stdafx.h"
#include "IOWorker.h"
#include "Overlapped.h"

#include "../IOCP/CompletionHandler.h"
#include "../Session/Session.h"
#include "../Listener/Listener.h"

#include <Log/ServerLog.h>

CIOWorker::CIOWorker(CCompletionHandler& SocketHandler)
:   m_SocketHandler(SocketHandler)
{

}

CIOWorker::~CIOWorker()
{

}

unsigned int CIOWorker::Run()
{
    COverlapped* lpOverlapped = 0;

    while(true)
    {
        unsigned long   dwProcessedBytes = 0;
        ULONG_PTR       lpSessionKey = 0;
        OVERLAPPED*     lpOverlappedStruct = 0;

        BOOL bResult = m_SocketHandler.GetHanderStatus(&dwProcessedBytes, 
            &lpSessionKey, &lpOverlappedStruct);

        if(0 == lpSessionKey)
        {
            DETLOG4(g_SessionLog, "SP:0x%p/lpOverlapped:0x%p/bResult:%s/Thread 0x%p Completed", 
                lpSessionKey, lpOverlappedStruct, bResult ? "T" : "F", GetCurrentThreadId());
            break;
        }

        if(!bResult && 0 == lpOverlappedStruct)
        {
            // �� �� ���� ����, Ȥ�� Ÿ�Ӿƿ�.
            DETLOG4(g_SessionLog, "SP:0x%p/lpOverlapped:0x%p/bResult:%s/GetQueuedCompletionStatus error : %u", 
                lpSessionKey, lpOverlappedStruct, bResult ? "T" : "F", WSAGetLastError());
        }
        else		
        {
            // Recv, Send, Accept�� ��.
            lpOverlapped = static_cast<COverlapped*>(lpOverlappedStruct);
            lpOverlapped->Dispatch(bResult, lpSessionKey, dwProcessedBytes);
        }
    }

    return 0;
}


BOOL CIOWorker::End()
{
    m_SocketHandler.PostToHandler(0, 0, 0);	
	return true;
}
