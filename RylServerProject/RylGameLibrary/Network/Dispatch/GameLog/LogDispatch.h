#ifndef _GAME_SERVER_LOG_DISPATCH_H_
#define _GAME_SERVER_LOG_DISPATCH_H_

#include <Network/Stream/SendStream.h>
#include <Network/Dispatch/RylServerDispatch.h>
#include <Network/Dispatch/SingleDispatchStorage.h>

class CLogDispatch : public CRylServerDispatch
{
public:

	static CSingleDispatch& GetDispatchTable()
	{
		static CSingleDispatch singleDispatch;
		return singleDispatch;
	}

    enum Const
    {
        MAX_PACKET_DISPATCH_PER_PULSE   = 10
    };

    CLogDispatch(CSession& Session);
    virtual ~CLogDispatch();

	virtual void Connected();   
    virtual void Disconnected();
    virtual bool DispatchPacket(PktBase* lpPktBase);

private:

    bool ProcessServerLoginAck(PktBase* lpPktBase);
};

#endif