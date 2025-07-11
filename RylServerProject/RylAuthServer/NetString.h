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

	HWND				m_hWnd;				// �θ� ������ �ڵ� 
	int					m_iIndex;

	SOCKET				m_hSocket;			// Ŭ���̾�Ʈ ���� 
	UINT				m_nPort;			// ��Ʈ 
	CHAR				m_strIPAddr[20];	// Server IP���� 

	BOOL				m_bConnect;			// ���� ���� �÷��� 
	BOOL				m_bClose;

	CPacketIOBuffer		m_RecvIO;
	char				m_PacketBuffer[MAX_PACKETSIZE];

	WSAEVENT			m_hEvent;			// ��Ʈ��ũ �̺�Ʈ �ڵ鷯 

	HANDLE				m_hEventThread;		// Recv ������ �ڵ� 

	std::list<std::string>	m_SendQue;
	std::list<std::string>	m_RecvQue;

private:
	static DWORD WINAPI EventThreadProc(LPVOID lParam);	// Main Thread 

	void	OnSendPacketData();
	void	OnReadPacketData();

	//////////////////
	// MessageHandling
	BOOL OnConnect();					// On Connect ��ȣ��	
	BOOL OnClose();						// On Close   ��ȣ��

	/////////////////
	// ���� ó�� �Լ� 
	BOOL Connect();										// C-S ���� 
	BOOL	WinSockInit();

protected:
	virtual void	OnSocketEvent(DWORD dID, DWORD dEvent) {};

	BOOL NetworkEventHanlder(LONG lEvent);						// �޼��� �б� �Լ� 
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

	// ���� �����Լ�.
	BOOL	Init(HWND hWnd, int iIndex, char* szIP, int nPort);	// �ʱ�ȭ 
	virtual void	Disconnect();

	virtual void	Update();

	void	Stop();									// Ŭ���̾�Ʈ ���� 
	void	CloseAll();
	void	SetParent(HWND hWnd);

public:
	CNetString(void);
	~CNetString(void);
};
