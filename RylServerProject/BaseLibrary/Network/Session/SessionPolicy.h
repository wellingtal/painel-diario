#ifndef _GAMA_NETWORK_LIBRARY_SESSION_POLICY_H_
#define _GAMA_NETWORK_LIBRARY_SESSION_POLICY_H_

// forward decl.
class CSocketFactory;
class CBufferFactory;
class CDispatchFactory;
class COverlappedFactory;

class CSessionPolicy
{
public:

    LONG AddRef();
    LONG Release();

    bool IsValid() 
    { 
        return 0 != m_lpBufferFactory && 0 != m_lpSocketFactory &&
			0 != m_lpDispatchFactory && 0 != m_lpOverlappedFactory; 
    }

	CSocketFactory&		GetSocketFactory()	    { return *m_lpSocketFactory;	 }
    CBufferFactory&     GetBufferFactory()      { return *m_lpBufferFactory;     }
    CDispatchFactory&   GetDispatchFactory()    { return *m_lpDispatchFactory;   }
    COverlappedFactory& GetOverlappedFactory()  { return *m_lpOverlappedFactory; }

    // Creation Factory
template <class SocketFactory, class BufferFactory, class DispatchFactory, class OverlappedFactory>        
        static CSessionPolicy* Create()
    {
        CSessionPolicy* lpSessionPolicy = new (std::nothrow) CSessionPolicy(            
			new SocketFactory,
            new BufferFactory,
            new DispatchFactory,
            new OverlappedFactory);

        if(0 == lpSessionPolicy || !lpSessionPolicy->IsValid())
        {
            delete lpSessionPolicy;
            lpSessionPolicy = 0;            
        }

        return lpSessionPolicy;
    }

	// edith 2009.08.14 어드민툴 메모리 누스 수정을 위해 Private 타입에서 Public 타입으로 변경.
	~CSessionPolicy();

private:
	
    CSessionPolicy(
		CSocketFactory*	lpSocketFactory,
		CBufferFactory* lpBufferFactory, 
        CDispatchFactory* lpDispatchFactory, 
		COverlappedFactory* lpOverlappedFactory);


	CSocketFactory*		m_lpSocketFactory;
    CBufferFactory*     m_lpBufferFactory;
    CDispatchFactory*   m_lpDispatchFactory;
    COverlappedFactory* m_lpOverlappedFactory;
    LONG                m_nRefCount;
};

#endif