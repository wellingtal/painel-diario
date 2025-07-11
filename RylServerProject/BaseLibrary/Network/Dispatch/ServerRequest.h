#ifndef _SERVER_REQUEST_H_
#define _SERVER_REQUEST_H_

#include <map>
#include <vector>
#include <utility>
#include <Thread/Lock.h>

class CSession;
class CPacketDispatch;

class CServerRequest
{
public:

	typedef void (*TimeoutRequest)(CPacketDispatch* lpSrcDispatch);

	static CServerRequest& GetInstance();
			
	// return value : RequestKey
	unsigned long AddRequest(CPacketDispatch* lpSrcDispatch, CPacketDispatch* lpDstDispatch, 
		unsigned long dwDurationSec, TimeoutRequest lpTimeoutRequest = 0);

	void RemoveRequest(unsigned long dwRequestKey);
	void RemoveRequest(CPacketDispatch* lpDispatch);
	
	void RemoveTimeoutRequest();
	void RemoveAllRequest();

	void RequestOn();
	void RequestOff();
	
    class Result
    {
    public:

        explicit Result(unsigned long dwRequestKey, bool bRemove = true);
        ~Result();

        CPacketDispatch* GetDispatch() { return m_lpSrcDispatch; }
        CSession*        GetSession();

    private:

		// new 나 delete로 생성하지 말 것.
		void* operator new (size_t size);
		void operator delete (void* ptr);

        CPacketDispatch* m_lpSrcDispatch;
		bool			 m_bRemove;
    };

private:

	CServerRequest();
	~CServerRequest();

	// return value : SrcRequest
	CPacketDispatch* GetRequest(unsigned long dwRequestKey);
	CPacketDispatch* PopRequest(unsigned long dwRequestKey);

    struct RequestInfo
    {
		CPacketDispatch*	m_lpSrcDispatch;
        CPacketDispatch*	m_lpDstDispatch;
        unsigned long		m_dwDurationSec;
		TimeoutRequest		m_lpTimeoutRequest;
        
		RequestInfo()
			:	m_lpSrcDispatch(0), 
				m_lpDstDispatch(0), 
				m_dwDurationSec(0), 
				m_lpTimeoutRequest(0)
		{

		}
        
        RequestInfo(CPacketDispatch* lpSrcDispatch, CPacketDispatch* lpDstDispatch, 
			unsigned long dwDurationSec, TimeoutRequest lpTimeoutRequest)
			:	m_lpSrcDispatch(lpSrcDispatch), 
				m_lpDstDispatch(lpDstDispatch), 
				m_dwDurationSec(dwDurationSec), 
				m_lpTimeoutRequest(lpTimeoutRequest)
		{

		}

		void ProcessRemove();
    };
    
	typedef CCSLock									LockType;	
    typedef std::map<unsigned long, RequestInfo>    RequestMap;

    LockType      m_RequestLock;
    CACHE_PAD(RequestLock, sizeof(LockType));

    RequestMap    m_RequestMap;
    unsigned long m_dwRequestCounter;
	unsigned long m_dwRequestFlags;
};


#endif