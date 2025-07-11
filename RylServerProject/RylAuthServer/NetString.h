#pragma once

#include "IOBuffer.h"
#include <list>
#include <string>

// sock
#include<winsock2.h>											// win32 socket 
#pragma comment(lib,"ws2_32.lib")								

#include<Mswsock.h>												// extension socket library 
#pragma comment(lib,"mswsock.lib")


#define WM_CONNECT				WM_APP + 0x1001
#define WM_RECV_MSG				WM_APP + 0x1002
#define WM_SOCK_CLOSE			WM_APP + 0x1003

class CNetString
{
public:
	class IEventListener
	{
	public:
		virtual ~IEventListener() {}
		virtual void EventPacket(int iIndex, char* pPacket)=0;
	};

	void SetEventListener(IEventListener* event)
	{
		m_event = event;
	}

	IEventListener*		m_event;

	HWND				m_hWnd;				// 부모 윈도우 핸들 
	int					m_iIndex;

	SOCKET				m_hSocket;			// 클라이언트 소켓 
	UINT				m_nPort;			// 포트 
	CHAR				m_strIPAddr[20];	// Server IP저장 

	BOOL				m_bConnect;			// 접속 상태 플래그 
	BOOL				m_bClose;

	CPacketIOBuffer		m_RecvIO;
	char				m_PacketBuffer[MAX_PACKETSIZE];

	WSAEVENT			m_hEvent;			// 네트워크 이벤트 핸들러 

	HANDLE				m_hEventThread;		// Recv 스레드 핸들 

	std::list<std::string>	m_SendQue;
	std::list<std::string>	m_RecvQue;

private:
	static DWORD WINAPI EventThreadProc(LPVOID lParam);	// Main Thread 

	void	OnSendPacketData();
	void	OnReadPacketData();

	//////////////////
	// MessageHandling
	BOOL OnConnect();					// On Connect 신호시	
	BOOL OnClose();						// On Close   신호시

	/////////////////
	// 내부 처리 함수 
	BOOL Connect();										// C-S 연결 
	BOOL	WinSockInit();

protected:
	virtual void	OnSocketEvent(DWORD dID, DWORD dEvent) {};

	BOOL NetworkEventHanlder(LONG lEvent);						// 메세지 분기 함수 
	void ProcessPacket(char* Packet, int PacketLen);

	int GetQueCnt()
	{
		return (int)m_RecvQue.size();
	}

	int GetPacket(char* pPacket)
	{
		strcpy(pPacket, m_RecvQue.begin()->c_str());
		return (int)strlen(pPacket);
	}

	void PopPacket()
	{
		m_RecvQue.pop_front();
	}

public:
	void	GetLocalIP(char* LocalIP);

	BOOL	IsConnect() { return m_bConnect; }

	virtual void	SendPost(char* Packet);

	// 시작 종료함수.
	BOOL	Init(HWND hWnd, int iIndex, char* szIP, int nPort);	// 초기화 
	virtual void	Disconnect();

	virtual void	Update();

	void	Stop();									// 클라이언트 정지 
	void	CloseAll();
	void	SetParent(HWND hWnd);

public:
	CNetString(void);
	~CNetString(void);
};
