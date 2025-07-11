///////////////////////////////////////////////////////////////////////////////////////////////
//
// LoginSocket Class
//
// Last Update : 2002. 8.28
//
///////////////////////////////////////////////////////////////////////////////////////////////
#ifndef _GAMA_CLIENT_LOGIN_SOCKET_H_
#define _GAMA_CLIENT_LOGIN_SOCKET_H_

#include <cstdio>


#include <Network/ClientSocket/Send/SendPacket.h>


// forward decl
struct PktBase;
struct SERVER_LIST;

namespace ClientNet
{
    class CClientEventHandler;
    class CClientEventHandlerMgr;
}


const unsigned short PatcherModel = 1;

// class decl
class LoginSocket
{
public:
	
    enum AuthFlag	
    {
        AuthFlag_RegDB	= 0x8000,
        AuthFlag_Email	= 0x4000
    };

    enum ConnectPort
    {
        DEFAULT_LOGIN_PORT = 10101  // Client Login TCP Port    
    };

    LoginSocket();
	virtual ~LoginSocket();

    // �α��� ������ �����Ѵ�.
    bool ConnectToLoginServer(ClientNet::CClientEventHandler* lpEventHandler, 
        const char* Address_In, unsigned short port = DEFAULT_LOGIN_PORT);

    // �̺�Ʈ�� ó���Ѵ�.
    void HandleEvents(unsigned long dwProcessPerCall = 30);

    // ������ ���´�.
	void Disconnect();
    void Destroy();
    bool IsConnected();
    
	bool ServerList(unsigned long ClientVersion_In);    // ��������Ʈ ��� ��û

    void SetHandler(ClientNet::CClientEventHandler* lpHandler) { m_lpHandler = lpHandler; }
    void ResetHandler(ClientNet::CClientEventHandler* lpHandler) { if(m_lpHandler == lpHandler) { m_lpHandler = 0; } }

    // ��������Ʈ�� �޴´�.
	static unsigned long HandleServerList(PktBase* lpPktBase, 
        unsigned long* ClientVer_Out, char* PatchAddress_Out, 
        SERVER_LIST* lpServerList_Out);

private:

    ClientNet::CClientEventHandlerMgr*  m_lpHandlerMgr;
    ClientNet::CClientEventHandler*     m_lpHandler;
};


#endif
