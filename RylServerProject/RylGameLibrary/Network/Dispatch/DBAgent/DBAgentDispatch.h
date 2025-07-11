#ifndef _DBAGENT_DISPATCH_H_
#define _DBAGENT_DISPATCH_H_

#include <map>
#include <Network/Stream/SendStream.h>
#include <Network/Dispatch/RylServerDispatch.h>
#include <Network/Dispatch/GameClient/GameClientDispatch.h>
#include <Network/Dispatch/SingleDispatchStorage.h>


class CDBAgentDispatch : public CRylServerDispatch
{
public:

	static CSingleDispatch& GetDispatchTable();

	CDBAgentDispatch(CSession& Session);
	virtual ~CDBAgentDispatch();

    virtual void Connected();
    virtual void Disconnected();
    virtual bool DispatchPacket(PktBase* lpPktBase);
};


#endif