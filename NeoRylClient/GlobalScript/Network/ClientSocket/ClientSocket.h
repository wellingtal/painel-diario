///////////////////////////////////////////////////////////////////////////////////////////////
//
// ClientSocket Class
//
// Last Update : 2003-01-10
//
///////////////////////////////////////////////////////////////////////////////////////////////
#ifndef _GAMA_CLIENT_SOCKET_H_
#define _GAMA_CLIENT_SOCKET_H_

#include <winsock2.h>
#include <windows.h>

#include <Network/Packet/PacketCommand.h>
#include <DB/DBDefine.h>
#include <vector>
#include <bitset>

namespace ClientNet
{
    // forward decl.
    class CNetworkMsgBlock;
    class CClientEventHandler;
    class CClientEventHandlerMgr;
};


class ClientSocket
{
protected:

    ClientSocket();
    virtual ~ClientSocket();
	static ClientSocket* s_pInstance;

public:

	static ClientSocket* CreateInstance();
	
    static void DestroyInstance() { if(s_pInstance) { delete s_pInstance; s_pInstance = 0; } };
	static ClientSocket* Instance() { return s_pInstance; };

    enum Const
    {
        MAX_CHAT_SERVER_ADDR				= 16,
		MAX_GAME_GUARD_CRYPT_HEADER_SIZE	= 32
    };

    enum ConnectPort
    {
        DEFAULT_AUTH_TCP_PORT   = 10210,//hz was on dedi 10222,//hz was 10222,//was 10210 
        DEFAULT_CLIENT_UDP_PORT = 20002    
    };

    enum HandlerType
    {
        AuthEventHandler    = 0,
        GameEventHandler    = 1,                
        MAX_HANDLER         = 2
    };

    enum AddressType
    {
        PublicAddr      = 0,
        PrivateAddr     = 1,
        MoveZoneAddr    = 2,
        MAX_ADDRESS     = 3
    };

    // ------------------------------------------------------------------------------------------------
    // ��Ʈ��ũ ���� �÷���
	// by Hades Kang
    void SetStatusFlag(unsigned long dwStatusFlag);
	unsigned long GetStatusFlag();

	static bool IsNSFlagChecked(unsigned char ucCmdFlag);			//�ش� �÷��װ� üũ�Ǿ����� ����
	static bool IsNSFlagChainChecked(unsigned char ucCmdFlag);		//�ش� �÷��� ü���� üũ�Ǿ����� ����
	static void SetNSFlagOn(unsigned char ucCmdFlag);				//�ش� �÷��׸� Ȱ��ȭ ��Ŵ - Send �ÿ� ����
	static void SetNSFlagOff(unsigned char ucCmdFlag);				//�ش� �÷��׿� ���� �� �͵��� ��Ȱ��ȭ ��Ŵ - Receive �ÿ� ����
	static int	CheckFlag( int* arrFlag );          				//������

    // ------------------------------------------------------------------------------------------------
    // Management Address
    SOCKADDR_IN& GetAddress(AddressType eAddrType) { return m_Address[eAddrType]; }
    const char* GetAddressString(AddressType eAddrType) { return inet_ntoa(m_Address[eAddrType].sin_addr); }
    unsigned short GetAddressPort(AddressType eAddrType) { return ntohs(m_Address[eAddrType].sin_port);    } 
    
    void SetAddress(AddressType eAddrType, SOCKADDR_IN& Address) { m_Address[eAddrType] = Address; }
    void SetAddress(AddressType eAddrType, const char* szAddress, unsigned port);
    void ClearAddress(AddressType eAddrType) { memset(m_Address + eAddrType, 0, sizeof(SOCKADDR_IN)); }

    ClientNet::CClientEventHandler& GetHandler(HandlerType eHandlerType) 
    { return (0 == m_lpEventHandler[eHandlerType]) ? *m_lpNullEventHandler : *m_lpEventHandler[eHandlerType]; }

    void SetHandler(ClientNet::CClientEventHandler* lpHandler, HandlerType eHandlerType);
    void ResetHandler(ClientNet::CClientEventHandler* lpHandler, HandlerType eHandlerType);

    // ������ �����ϰ�, ������ �����ϰų� ������ �ּҿ� ���ε��� �Ѵ�.
    bool Open(const char* address, unsigned short usport, 
        ClientNet::CClientEventHandler* lpEventHandler);

    // ����Ǿ��ִ��� ���θ� ���ɴ�.
    bool IsConnected(HandlerType eHandlerType);

    // MAX_SESSION�� ������ ���� ������ ���´�.
    void Disconnect(HandlerType eHandlerType);

    // ���� �� ��Ŷ�� ó���Ѵ�.
    void HandleEvents(DWORD dwProcessPerCall);

private:

	void InitNSCheckList();         // Network Status ���� Check List �ʱ�ȭ ( Send, Receive )
	void InitSendCheckList();       
	void InitReceiveCheckList();
    
    SOCKADDR_IN                         m_Address[MAX_ADDRESS + 1];
    ClientNet::CClientEventHandler*     m_lpEventHandler[MAX_HANDLER + 1];
    ClientNet::CClientEventHandler*     m_lpNullEventHandler;

    ClientNet::CClientEventHandlerMgr*  m_lpEventHandlerMgr;
        
    unsigned long                       m_dwStatusFlag;
	std::vector<unsigned char>	        m_SendCheckList[CmdFinalPacketNum];		//������ üũ �ؾ��ϴ� Ŀ�ǵ� ����Ʈ ( �ϳ��� true �� ��� ������ �ʴ´�! )
	std::vector<unsigned char>	        m_ReceiveCheckList[CmdFinalPacketNum];  //������ üũ �ؾ��ϴ� Ŀ�ǵ� ����Ʈ ( ��� false�� ó���Ѵ�. )
    std::bitset<CmdFinalPacketNum>      m_bStatusFlag;
};


#endif