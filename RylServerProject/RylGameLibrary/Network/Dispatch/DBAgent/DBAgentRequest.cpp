#include "stdafx.h"
#include "DBAgentRequest.h"
#include "DBAgentDispatch.h"
#include "../GameClient/GameClientDispatch.h"

#include <Network/Session/Session.h>
#include <Network/Dispatch/ServerRequest.h>


CDBRequest::CDBRequest(CGameClientDispatch& GameClientDispatch, 
					   unsigned long dwDurationSec, TimeoutRequest lpTimeoutRequest)
:	m_dwRequestKey(0), 
	m_lpDBAgentSendStream(0)
{
	GET_SINGLE_DISPATCH(lpDBAgentDispatch, 
		CDBAgentDispatch, CDBAgentDispatch::GetDispatchTable());

    if(0 != lpDBAgentDispatch)
    {
        m_lpDBAgentSendStream = &lpDBAgentDispatch->GetSendStream();
        if(0 != m_lpDBAgentSendStream)
        {
            m_dwRequestKey = CServerRequest::GetInstance().AddRequest(&GameClientDispatch, 
				lpDBAgentDispatch, dwDurationSec, lpTimeoutRequest);

            if(0 == m_dwRequestKey) 
			{
				m_lpDBAgentSendStream = 0;
			}
        }
    }
}

void CDBRequest::CancelRequest()
{
	if(0 != m_dwRequestKey)
	{
        CServerRequest::GetInstance().RemoveRequest(m_dwRequestKey);
	}
}