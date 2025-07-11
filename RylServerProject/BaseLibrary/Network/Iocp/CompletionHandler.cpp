#include "stdafx.h"
#include "CompletionHandler.h"

CCompletionHandler::CCompletionHandler()
:	m_hIOCP(INVALID_HANDLE_VALUE), m_nThread(0), m_nTimeOutMS(0)
{
	
}


CCompletionHandler::~CCompletionHandler()
{
	Destroy();	
}


bool CCompletionHandler::Initialize(unsigned long nThread, unsigned long nTimeOutMS)
{
	if(INVALID_HANDLE_VALUE != m_hIOCP)
	{		
		return false;
	}

	m_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, nThread);
	if(0 == m_hIOCP)
	{		
		return false;
	}

	m_nThread = nThread;
	m_nTimeOutMS = nTimeOutMS;
    return true;
}


bool CCompletionHandler::Destroy()
{
	if(INVALID_HANDLE_VALUE != m_hIOCP)
	{
		if(!CloseHandle(m_hIOCP))
		{
			return false;
		}
	}

	m_hIOCP = INVALID_HANDLE_VALUE;
	return true;
}