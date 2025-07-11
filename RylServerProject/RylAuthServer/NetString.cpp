#include "stdafx.h"
#include "NetString.h"

CNetString::CNetString(void)
{
	m_iIndex = 0;
	m_hWnd = NULL;				// �θ� ������ �ڵ� 

	m_hSocket = NULL;			// Ŭ���̾�Ʈ ���� 
	m_nPort = 0;				// ��Ʈ 
	ZeroMemory(m_strIPAddr, sizeof(char)*20);	// Server IP���� 

	m_bConnect = FALSE;			// ���� ���� �÷��� 
	m_bClose = FALSE;

	m_RecvIO.NewIOBuf(0);	// 0���� �ϸ� �⺻������ DefaultPacketSize * 1024

	m_hEvent = NULL;	// ��Ʈ��ũ �̺�Ʈ �ڵ鷯 

	ZeroMemory(m_PacketBuffer, sizeof(char)*MAX_PACKETSIZE);	// Server IP���� 

	WinSockInit();
}

CNetString::~CNetString(void)
{
	OnClose();
	Stop();								// ���� �Լ� ȣ�� 

	m_RecvIO.DeleteIOBuf();

	WSACleanup();
}

BOOL CNetString::WinSockInit()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD( 2, 2 );
 
	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) {
		WSACleanup();
		// Tell the user that we could not find a usable 
		// WinSock DLL.                                  
		return FALSE;
	}
 
	if ( LOBYTE( wsaData.wVersion ) != 2 ||
			HIBYTE( wsaData.wVersion ) != 2 ) {
		// Tell the user that we could not find a usable //
		// WinSock DLL.   //
		WSACleanup( );
		return FALSE; 
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////
// [1]DESCRIPTION : Ŭ���� �ʱ�ȭ 					 			//
// [2]PARAMETER : strIPAddr - ����IP�ּ�, nPort - ��Ʈ��ȣ,		//
//				  hWnd - �θ� ������ �ڵ�						//
// [3]RETURN :	���� - TRUE, ���� - FALSE						//
// [4]DATE : 2000�� 9�� 11��									//
//////////////////////////////////////////////////////////////////
BOOL CNetString::Init( HWND hWnd, int iIndex, char* szIP, int nPort )
{	
	m_iIndex = iIndex;
	m_nPort = nPort;											// ��Ʈ ��ȣ 
	unsigned long dwThreadId = 0;								// ������ ������ ���� ���� 

	// ip ��巹�� 
	strcpy(m_strIPAddr,szIP);
	m_hWnd = hWnd;												// �θ� �ڵ� 

	// ������ ��Ų��. ������.. ���� �������ῡ �����Ѵٸ�
	// Netword Event ���� FW_CLOSE�� �߻��� ������ Close�ȴ�.
	if(!Connect())
	{
		// ����������� �����Ѵ�.
		Sleep(100);													// Sleep...
		OnClose();	
		return FALSE;
	}

	m_bClose = FALSE;

	m_hEventThread =
			(HANDLE)CreateThread(NULL,							// Security
				0,												// Stack size - use default
				EventThreadProc,     							// Thread fn entry point
				(void*) this,	      
				0,												// Init flag
				&dwThreadId);									// Thread address

	return TRUE;
}

//////////////////////////////////////////////////////////////////
// [1]DESCRIPTION : Main Thread, ��Ʈ��ũ �̺�Ʈ ó��			//
// [2]PARAMETER : void											//
// [3]RETURN :	void											//
// [4]DATE : 2000�� 9�� 10��									//
//////////////////////////////////////////////////////////////////
DWORD WINAPI CNetString::EventThreadProc(LPVOID lParam)
{
	// Ŭ������ ������ ����
	CNetString* pThis = reinterpret_cast<CNetString*>(lParam);

	WSANETWORKEVENTS events;		// ��Ʈ��ũ �̺�Ʈ ���� 
	BOOL bThreadRun = TRUE;			// ���� ���� ���� 

	// ������ ���� ���� 
	while(bThreadRun)
	{
		if(pThis->m_bClose)
		{
			bThreadRun = FALSE;
			break;
		}

		DWORD dwRet;
		dwRet = WSAWaitForMultipleEvents(1,
									&pThis->m_hEvent,
									FALSE,
									INFINITE,
									FALSE);
 
 		if(!pThis->m_hSocket)
		{	
			// ����
			bThreadRun = FALSE;
			break;
		}
		// Figure out what happened
		int nRet = WSAEnumNetworkEvents(pThis->m_hSocket,
								pThis->m_hEvent,
								&events);
		
		// ���� �������,
		if (nRet == SOCKET_ERROR)	
		{
			bThreadRun = FALSE;
			break;
		}

		///////////////////
		// Handle events //
		bThreadRun = pThis->NetworkEventHanlder(events.lNetworkEvents);
	}
	// �̸��� �����尡 ���� �Ǹ� Server�� ���� Ŭ���̾�Ʈ ����!!! <������ ����>
	// ������ �ʱ�ȭ�� ������ ���ֱ� ������ ���⼭ ������ �ʴ´�.
//		pThis->CloseAll();
	return 0;
}

void CNetString::Disconnect()
{
	// �̰� ������ Close������ϱ� ������  CloseAll() ȣ������ �ʰ� ���� ����.
	OnClose();
	Stop();								// ���� �Լ� ȣ�� 
}

void CNetString::Stop()
{
	if (m_hSocket)
	{
		struct linger li = {0, 0};	// Default: SO_DONTLINGER

		li.l_onoff = 1; // SO_LINGER, timeout = 0
		shutdown(m_hSocket, SD_BOTH );						// ����? �̰� ����? ^^;; �㿡 ã�ƺ��� 
															// 2001�� 9�� 6�� 

		// Ŭ���� ���� ���� ť�� ����Ÿ�� ������ ���� �����ϴ� �ɼ�
		setsockopt(m_hSocket, SOL_SOCKET, SO_LINGER, (CHAR *)&li, sizeof(li));
		closesocket(m_hSocket);								// ���� �ݱ� 
		m_hSocket = NULL;
	}

	if(m_hEvent) WSACloseEvent(m_hEvent);
	m_hEvent = NULL;
}

void CNetString::CloseAll()
{
	if(m_bConnect)
	{
		OnClose();
		Stop();								// ���� �Լ� ȣ�� 
	}
}

BOOL CNetString::OnClose()
{
	m_bClose = TRUE;
	m_bConnect = FALSE;											// Connect ���� ���� 

	OnSocketEvent(WM_SOCK_CLOSE, 0);

	return FALSE;
}

//////////////////////////////////////////////////////////////////
// [1]DESCRIPTION : ���� ������ ���� �Լ� 						//
// [2]PARAMETER : void											//
// [3]RETURN :	���� - TRUE, ���� - FALSE						//
// [4]DATE : 2000�� 9�� 11��									//
//////////////////////////////////////////////////////////////////
BOOL CNetString::Connect()
{	
	// �������̶�� 
	if(m_bConnect) return TRUE;

	// ������ ���� �ִٸ� 
	Stop();

	m_RecvIO.InitIOBuf();

	// ���� ���� 
	m_hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	// ���� ���� �˻� 
	if (m_hSocket == INVALID_SOCKET)
	{
		OnSocketEvent(WM_CONNECT, FALSE);
		return FALSE;
	}

	// ��Ʈ��ũ �̺�Ʈ �ڵ� ���� 
	m_hEvent = WSACreateEvent();
	if (m_hEvent == WSA_INVALID_EVENT)
	{
		Stop();
		return FALSE;
	}

	// Request async notification
	int nRet = WSAEventSelect(m_hSocket,
							m_hEvent,
							FD_CLOSE | FD_CONNECT);	// ��ȣ�� �����Ͽ� �ް� �Ѵ� 
	
	// ������� 
	if (nRet == SOCKET_ERROR)
	{
		Stop();
		return FALSE;
	}

	// �񵿱� ���
	unsigned long ul = 1;
	nRet = ioctlsocket(m_hSocket, FIONBIO, (unsigned long*)&ul);
	// ���� ���� �˻� 
	if (m_hSocket == SOCKET_ERROR)
	{
		OnSocketEvent(WM_CONNECT, FALSE);
		return FALSE;
	}

	/////////////////////////////////
	// ������ ���� ����ȭ�� ���� ���� 
	int zero = 0;
	int err = 0;

	// Send Buffer�� ���� ����
	if( (err = setsockopt( m_hSocket, SOL_SOCKET, SO_SNDBUF, (const char*)&zero, sizeof(zero))) == SOCKET_ERROR)
	{
		closesocket(m_hSocket);
		m_hSocket = NULL;
		return FALSE;
	}

	// Receive Buffer�� ���� ���� 
	if((err = setsockopt( m_hSocket, SOL_SOCKET, SO_RCVBUF, (const char*)&zero, sizeof(zero) )) == SOCKET_ERROR)
	{
		closesocket(m_hSocket);
		m_hSocket = NULL;
		return FALSE;
	}

	SOCKADDR_IN		saServer;		

	memset(&saServer,0,sizeof(saServer));

	saServer.sin_family = AF_INET;
	saServer.sin_addr.s_addr = inet_addr(m_strIPAddr);
	saServer.sin_port = htons(m_nPort);

	// ������ Connect
	nRet = connect(m_hSocket,(sockaddr*)&saServer, sizeof(saServer));
	// ���� �����̰ų� ��ŷ�� �Ǿ��ٸ� 
	if (nRet == SOCKET_ERROR &&	WSAGetLastError() != WSAEWOULDBLOCK)
	{
		Stop();
		return FALSE;
	}
/*
	�̷��� Connect�� �ϸ� ������ AccetpEx�� �����Ͽ� ClientSceesion�� 
	Recv�����·� ����. 
	���� �̺�Ʈ�� Connect�� �����ϸ� FD_CONNECT�� �߻���Ų��.
*/
	return TRUE;
}

//////////////////////////////////////////////////////////////////
// [1]DESCRIPTION : ��Ʈ��ũ �޼����� �ڵ鸵 �Ͽ� �б�			//
// [2]PARAMETER : lEvent - �̺�Ʈ					 			//
// [3]RETURN :	void											//
// [4]DATE : 2000�� 9�� 14��									//
//////////////////////////////////////////////////////////////////
BOOL CNetString::NetworkEventHanlder(LONG lEvent)
{
	BOOL bFlag = TRUE;

	if(lEvent & FD_CLOSE)
	{
		bFlag = FALSE;
	}
	if(lEvent & FD_CONNECT)
	{
		bFlag = OnConnect();
	}

	return bFlag;
}

void CNetString::Update()
{
	if(!IsConnect())
		return;

	OnSendPacketData();
	OnReadPacketData();

	while(GetQueCnt() != 0)
	{
		int iCnt = GetPacket(m_PacketBuffer);

		ProcessPacket(m_PacketBuffer, iCnt);

//		printf(m_PacketBuffer);

		PopPacket();
	}
}

void CNetString::ProcessPacket(char* Packet, int PacketLen)
{
	if(m_event)
		m_event->EventPacket(m_iIndex, Packet);
}

void CNetString::SendPost(char* Packet)
{
	// ������ Send �ϱ�..
	m_SendQue.push_back(Packet);
}

void CNetString::OnSendPacketData()
{
	if(m_SendQue.empty())
		return;

	//////////////////////////////////////////////////////////////////////////////
	// Send
	int rc = 0;
	int idx = 0,size = 0;
	char send_buf[MAX_PACKETSIZE];

	strcpy(send_buf, m_SendQue.begin()->c_str());
	size = (int)strlen(send_buf);

	while(size > 0)
	{
		// 10004 : WSACancelBlockingCall�� ȣ���Ͽ� ���� �۾��� �ߴܵǾ����ϴ�. 
		// 10058 : �ش� ������ ����Ǿ����Ƿ� ������ �����ų� ���� �� �����ϴ�. 
		// 10038 : ������ ������ ������ ����ҷ��� �Ҷ� ����.
		if((rc = send(m_hSocket, &send_buf[idx], size, 0)) == SOCKET_ERROR)
		{
			// ��ŷ ������� 
			if (GetLastError() != WSAEWOULDBLOCK) // ��ŷ ������ �ƴ϶�� 
			{				
				break;
			}
		}
		else
		{
			// ������ ���ٸ� 
			size -= rc;
			idx += rc;
		}

		m_SendQue.pop_front();
	}
}

void CNetString::OnReadPacketData()
{
	//////////////////////////////////////////////////////////////////////////////
	// Recv
	int Ret = recv(m_hSocket, m_PacketBuffer, MAX_PACKETSIZE, 0);	// ����Ÿ Receive
	if(Ret == 0)        // Graceful close
	{
		CloseAll();
		return;
	}
	else if (Ret == SOCKET_ERROR &&	WSAGetLastError() != WSAEWOULDBLOCK ) // ��ŷ ������ �ƴ϶�� 
	{
		m_bConnect = FALSE;
		Stop();												// ���α׷� ���� 
		return;							
	}

	if(Ret > 0)
	{
		if(m_RecvIO.Append(m_PacketBuffer, Ret) == -1)
		{
			;
		}
	}

	// ���� ��Ŷ�� IOBuffer�� �־� ó���Ѵ�.
	int iLen = m_RecvIO.GetPacket(m_PacketBuffer);
	if(iLen > 0)
	{
		// ���⼭ ����Ŷ ó�� ��ƾ ȣ��
		m_RecvQue.push_back(m_PacketBuffer);

		// Message Type �϶� �̰ɷ� ������. 
		// ���� Update �̺�Ʈ ȣ���̸� �� ��ƾ�� �ּ�ó�� ���ش�.
		OnSocketEvent(WM_RECV_MSG, iLen);
	}
}

//////////////////////////////////////////////////////////////////
// [1]DESCRIPTION : �̺�Ʈ ó�� (On Connect)					//
// [2]PARAMETER : void											//
// [3]RETURN :	false ��ȯ										//
// [4]DATE : 2000�� 9�� 11��									//
//////////////////////////////////////////////////////////////////
BOOL CNetString::OnConnect()
{
	m_bConnect = TRUE;						// ���� ���� ���� ON
	OnSocketEvent(WM_CONNECT, m_bConnect);

	return m_bConnect;
}

void CNetString::SetParent(HWND hWnd)
{
	m_hWnd = hWnd;
}

void CNetString::GetLocalIP(char* LocalIP)
{
	char name[256];
	char* TempIp;
	PHOSTENT hostinfo;

	WinSockInit();

	if( gethostname ( name, sizeof(name)) == 0)
	{
		if((hostinfo = gethostbyname(name)) != NULL)
		{
			TempIp = inet_ntoa (*(struct in_addr *)*hostinfo->h_addr_list);
			strcpy(LocalIP, TempIp);
		}
	}
}

