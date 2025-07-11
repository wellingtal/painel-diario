#include "stdafx.h"
#include "NetString.h"

CNetString::CNetString(void)
{
	m_iIndex = 0;
	m_hWnd = NULL;				// 부모 윈도우 핸들 

	m_hSocket = NULL;			// 클라이언트 소켓 
	m_nPort = 0;				// 포트 
	ZeroMemory(m_strIPAddr, sizeof(char)*20);	// Server IP저장 

	m_bConnect = FALSE;			// 접속 상태 플래그 
	m_bClose = FALSE;

	m_RecvIO.NewIOBuf(0);	// 0으로 하면 기본적으로 DefaultPacketSize * 1024

	m_hEvent = NULL;	// 네트워크 이벤트 핸들러 

	ZeroMemory(m_PacketBuffer, sizeof(char)*MAX_PACKETSIZE);	// Server IP저장 

	WinSockInit();
}

CNetString::~CNetString(void)
{
	OnClose();
	Stop();								// 종료 함수 호출 

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
// [1]DESCRIPTION : 클래스 초기화 					 			//
// [2]PARAMETER : strIPAddr - 연결IP주소, nPort - 포트번호,		//
//				  hWnd - 부모 윈도우 핸들						//
// [3]RETURN :	정상 - TRUE, 실패 - FALSE						//
// [4]DATE : 2000년 9월 11일									//
//////////////////////////////////////////////////////////////////
BOOL CNetString::Init( HWND hWnd, int iIndex, char* szIP, int nPort )
{	
	m_iIndex = iIndex;
	m_nPort = nPort;											// 포트 번호 
	unsigned long dwThreadId = 0;								// 스레드 생성을 위한 변수 

	// ip 어드레스 
	strcpy(m_strIPAddr,szIP);
	m_hWnd = hWnd;												// 부모 핸들 

	// 연결을 시킨다. 서버로.. 만약 서버연결에 실패한다면
	// Netword Event 에서 FW_CLOSE가 발생해 소켓이 Close된다.
	if(!Connect())
	{
		// 실패했을경우 종료한다.
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
// [1]DESCRIPTION : Main Thread, 네트워크 이벤트 처리			//
// [2]PARAMETER : void											//
// [3]RETURN :	void											//
// [4]DATE : 2000년 9월 10일									//
//////////////////////////////////////////////////////////////////
DWORD WINAPI CNetString::EventThreadProc(LPVOID lParam)
{
	// 클래스를 변수로 받음
	CNetString* pThis = reinterpret_cast<CNetString*>(lParam);

	WSANETWORKEVENTS events;		// 네트워크 이벤트 변수 
	BOOL bThreadRun = TRUE;			// 무한 루프 변수 

	// 스레드 무한 루프 
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
			// 종료
			bThreadRun = FALSE;
			break;
		}
		// Figure out what happened
		int nRet = WSAEnumNetworkEvents(pThis->m_hSocket,
								pThis->m_hEvent,
								&events);
		
		// 소켓 에러라면,
		if (nRet == SOCKET_ERROR)	
		{
			bThreadRun = FALSE;
			break;
		}

		///////////////////
		// Handle events //
		bThreadRun = pThis->NetworkEventHanlder(events.lNetworkEvents);
	}
	// 이리로 스레드가 종료 되면 Server에 의한 클라이언트 종료!!! <비정상 종료>
	// 스레드 초기화는 위에서 해주기 때문에 여기서 하지는 않는다.
//		pThis->CloseAll();
	return 0;
}

void CNetString::Disconnect()
{
	// 이건 무조건 Close해줘야하기 때문에  CloseAll() 호출하지 않고 직접 끈다.
	OnClose();
	Stop();								// 종료 함수 호출 
}

void CNetString::Stop()
{
	if (m_hSocket)
	{
		struct linger li = {0, 0};	// Default: SO_DONTLINGER

		li.l_onoff = 1; // SO_LINGER, timeout = 0
		shutdown(m_hSocket, SD_BOTH );						// 오잉? 이게 뭐지? ^^;; 담에 찾아보자 
															// 2001년 9월 6일 

		// 클로즈 소켓 전에 큐된 데이타를 보낼지 말지 결정하는 옵션
		setsockopt(m_hSocket, SOL_SOCKET, SO_LINGER, (CHAR *)&li, sizeof(li));
		closesocket(m_hSocket);								// 소켓 닫기 
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
		Stop();								// 종료 함수 호출 
	}
}

BOOL CNetString::OnClose()
{
	m_bClose = TRUE;
	m_bConnect = FALSE;											// Connect 변수 변경 

	OnSocketEvent(WM_SOCK_CLOSE, 0);

	return FALSE;
}

//////////////////////////////////////////////////////////////////
// [1]DESCRIPTION : 서버 연결을 위한 함수 						//
// [2]PARAMETER : void											//
// [3]RETURN :	정상 - TRUE, 실패 - FALSE						//
// [4]DATE : 2000년 9월 11일									//
//////////////////////////////////////////////////////////////////
BOOL CNetString::Connect()
{	
	// 연결중이라면 
	if(m_bConnect) return TRUE;

	// 소켓이 남아 있다면 
	Stop();

	m_RecvIO.InitIOBuf();

	// 소켓 생성 
	m_hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	// 소켓 생성 검사 
	if (m_hSocket == INVALID_SOCKET)
	{
		OnSocketEvent(WM_CONNECT, FALSE);
		return FALSE;
	}

	// 네트워크 이벤트 핸들 생성 
	m_hEvent = WSACreateEvent();
	if (m_hEvent == WSA_INVALID_EVENT)
	{
		Stop();
		return FALSE;
	}

	// Request async notification
	int nRet = WSAEventSelect(m_hSocket,
							m_hEvent,
							FD_CLOSE | FD_CONNECT);	// 신호를 선별하여 받게 한다 
	
	// 에러라면 
	if (nRet == SOCKET_ERROR)
	{
		Stop();
		return FALSE;
	}

	// 비동기 방식
	unsigned long ul = 1;
	nRet = ioctlsocket(m_hSocket, FIONBIO, (unsigned long*)&ul);
	// 소켓 생성 검사 
	if (m_hSocket == SOCKET_ERROR)
	{
		OnSocketEvent(WM_CONNECT, FALSE);
		return FALSE;
	}

	/////////////////////////////////
	// 소켓의 성능 최적화를 위한 세팅 
	int zero = 0;
	int err = 0;

	// Send Buffer에 대한 세팅
	if( (err = setsockopt( m_hSocket, SOL_SOCKET, SO_SNDBUF, (const char*)&zero, sizeof(zero))) == SOCKET_ERROR)
	{
		closesocket(m_hSocket);
		m_hSocket = NULL;
		return FALSE;
	}

	// Receive Buffer에 대한 세팅 
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

	// 서버와 Connect
	nRet = connect(m_hSocket,(sockaddr*)&saServer, sizeof(saServer));
	// 소켓 에러이거나 블럭킹이 되었다면 
	if (nRet == SOCKET_ERROR &&	WSAGetLastError() != WSAEWOULDBLOCK)
	{
		Stop();
		return FALSE;
	}
/*
	이렇게 Connect를 하면 서버는 AccetpEx가 성공하여 ClientSceesion은 
	Recv대기상태로 들어간다. 
	소켓 이벤트는 Connect가 성공하면 FD_CONNECT를 발생시킨다.
*/
	return TRUE;
}

//////////////////////////////////////////////////////////////////
// [1]DESCRIPTION : 네트워크 메세지를 핸들링 하여 분기			//
// [2]PARAMETER : lEvent - 이벤트					 			//
// [3]RETURN :	void											//
// [4]DATE : 2000년 9월 14일									//
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
	// 서버로 Send 하기..
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
		// 10004 : WSACancelBlockingCall를 호출하여 차단 작업이 중단되었습니다. 
		// 10058 : 해당 소켓이 종료되었으므로 데이터 보내거나 받을 수 없습니다. 
		// 10038 : 연결이 끊어진 소켓을 사용할려고 할때 난다.
		if((rc = send(m_hSocket, &send_buf[idx], size, 0)) == SOCKET_ERROR)
		{
			// 블럭킹 에러라면 
			if (GetLastError() != WSAEWOULDBLOCK) // 블럭킹 에러가 아니라면 
			{				
				break;
			}
		}
		else
		{
			// 에러가 없다면 
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
	int Ret = recv(m_hSocket, m_PacketBuffer, MAX_PACKETSIZE, 0);	// 데이타 Receive
	if(Ret == 0)        // Graceful close
	{
		CloseAll();
		return;
	}
	else if (Ret == SOCKET_ERROR &&	WSAGetLastError() != WSAEWOULDBLOCK ) // 블럭킹 에러가 아니라면 
	{
		m_bConnect = FALSE;
		Stop();												// 프로그램 종료 
		return;							
	}

	if(Ret > 0)
	{
		if(m_RecvIO.Append(m_PacketBuffer, Ret) == -1)
		{
			;
		}
	}

	// 받은 패킷은 IOBuffer에 넣어 처리한다.
	int iLen = m_RecvIO.GetPacket(m_PacketBuffer);
	if(iLen > 0)
	{
		// 여기서 한패킷 처리 루틴 호출
		m_RecvQue.push_back(m_PacketBuffer);

		// Message Type 일때 이걸로 보낸다. 
		// 만약 Update 이벤트 호출이면 이 루틴을 주석처리 해준다.
		OnSocketEvent(WM_RECV_MSG, iLen);
	}
}

//////////////////////////////////////////////////////////////////
// [1]DESCRIPTION : 이벤트 처리 (On Connect)					//
// [2]PARAMETER : void											//
// [3]RETURN :	false 반환										//
// [4]DATE : 2000년 9월 11일									//
//////////////////////////////////////////////////////////////////
BOOL CNetString::OnConnect()
{
	m_bConnect = TRUE;						// 연결 변수 설정 ON
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

