#ifndef _GM_NETWORK_CREATE_POLICY_
#define _GM_NETWORK_CREATE_POLICY_

#include <Network/Session/SessionPolicy.h>
#include <Network/Winsock/SocketFactory.h>
#include <Network/Dispatch/Dispatch.h>
#include <Network/Dispatch/PoolDispatchFactory.h>
#include <Network/IOCP/Overlapped.h>
#include <Stream/Buffer/BufferFactory.h>


namespace SessionPolicy
{
	template<typename DispatchType>
	CSessionPolicy* CreateTCPPolicy()
	{
		return CSessionPolicy::Create<CTCPFactory, CPoolBufferFactory, 
			CPoolDispatchFactory<DispatchType>, CStreamOverlappedFactory>();
	}

	template<typename DispatchType>
	CSessionPolicy* CreateUDPPolicy()
	{
		return CSessionPolicy::Create<CUDPFactory, CPoolBufferFactory,
			CPoolDispatchFactory<DispatchType>, CDatagramOverlappedFactory>();
	}
}


#endif