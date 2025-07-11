#ifndef _DBAGENT_REQUEST_H_
#define _DBAGENT_REQUEST_H_

// 전방 참조
class CDBRequest;
class CSendStream;
class CPacketDispatch;
class CGameClientDispatch;

class CDBRequest
{
public:

	typedef void (*TimeoutRequest)(CPacketDispatch* lpSrcDispatch);

    CDBRequest(CGameClientDispatch& GameClientDispatch, 
		unsigned long dwDurationSec, TimeoutRequest lpTimeoutRequest = 0);

    bool IsValid() { return (0 != m_lpDBAgentSendStream) && (0 != m_dwRequestKey); }
    CSendStream&  GetSendStream() { return *m_lpDBAgentSendStream; }
    unsigned long GetRequestKey() { return m_dwRequestKey; }

	void CancelRequest();

private:
    
    unsigned long   m_dwRequestKey;
    CSendStream*    m_lpDBAgentSendStream;
};


#endif