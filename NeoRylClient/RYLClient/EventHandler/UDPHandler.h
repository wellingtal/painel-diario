#ifndef _GAMA_TEST_UDP_HANDLER_H_
#define _GAMA_TEST_UDP_HANDLER_H_

#include <Network/ClientNetwork/ClientEventHandler.h>

// forward decl.
namespace ClientNet
{
    class CSession;
    class CSessionMgr;
}

// UDP 이벤트 핸들러
class CUDPEventHandler : public ClientNet::CClientEventHandler
{
public:

    CUDPEventHandler(ClientNet::CSessionMgr& SessionMgr);
    virtual ~CUDPEventHandler();

    virtual int OnOpen(ClientNet::CSession& OpenSession, int iErrorCode);
    virtual int OnClose(ClientNet::CSession& CloseSession);

protected:

    virtual int OnDispatch(ClientNet::CSession& ReceivedSession, PktBase* lpPktBase, INET_Addr& peerAddr);
};



#endif