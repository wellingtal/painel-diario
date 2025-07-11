#ifndef _GAMA_SERVER_NETWORK_POOL_DISPATCH_FACTORY_H_
#define _GAMA_SERVER_NETWORK_POOL_DISPATCH_FACTORY_H_

#include "Dispatch.h"
#include <boost/pool/pool.hpp>

template<typename DispatchType>
class CPoolDispatchFactory : public CDispatchFactory
{
public:

    CPoolDispatchFactory() : m_DispatchPool(sizeof(DispatchType)) { }
    virtual ~CPoolDispatchFactory() { }

    virtual CPacketDispatch* CreateDispatch(CSession& Session) 
    {
        void* ptr = m_DispatchPool.malloc();
        return new (ptr) DispatchType(Session);
    }

    virtual void DeleteDispatch(CPacketDispatch* lpDispatch) 
    {
        lpDispatch->~CPacketDispatch();
        m_DispatchPool.free(lpDispatch);
    }

protected:

    typedef boost::pool<> DispatchPool;
    
    DispatchPool m_DispatchPool;
};
  
#endif