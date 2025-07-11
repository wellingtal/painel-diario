#ifndef _GAMA_TEST_CLIENT_AUTH_HANDLER_H_
#define _GAMA_TEST_CLIENT_AUTH_HANDLER_H_

#include <Network/ClientNetwork/ClientEventHandler.h>

// forward decl.
namespace ClientNet
{
    class CSession;
    class CSessionMgr;
}


// ���� ���� �̺�Ʈ �ڵ鷯
class CAuthEventHandler : public ClientNet::CClientEventHandler
{
public:

    CAuthEventHandler();
    virtual ~CAuthEventHandler();

protected:

    virtual int OnDispatch(PktBase* lpPktBase, INET_Addr& peerAddr);
    virtual int OnOpen(int iErrorCode);
    virtual int OnClose();
};


#endif