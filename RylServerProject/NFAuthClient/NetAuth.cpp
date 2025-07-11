#include "NetAuth.h"

using namespace NaveNet;

CNetAuth::CNetAuth(void) : m_event(0)
{
	// 함수를 Map으로 선언한다.
	OnMsgMap[CMD] = &CNetAuth::Parsed_CMD;
	OnMsgMap[AUTHOR] = &CNetAuth::Parsed_Dummy;
	OnMsgMap[IPLIST] = &CNetAuth::Parsed_IPLIST;
	
	OnMsgMap[IRC] = &CNetAuth::Parsed_IRC;
}

CNetAuth::~CNetAuth(void)
{
	m_event = NULL;
	Disconnect();
}

void CNetAuth::Disconnect()
{
	NFNetClientS::Disconnect();

	m_SendPacket.Init();
}

void CNetAuth::OnSocketEvent(DWORD dID, DWORD dEvent)
{
	// 소켓에서 일어난 이벤트를 처리하기 위한것.
	if(dID == WM_CONNECT)			// 접속 이벤트
	{
		if(m_event)
			m_event->EventConnect(dEvent);
	}
	else if(dID == WM_SOCK_CLOSE)	// 소켓 종료 이벤트
	{
		if(m_event)
			m_event->EventSocketClose();
	}
	else if(dID == WM_RECV_MSG)		// 패킷 한개 받은 이벤트
	{
		// Test할때만 사용된다 나중에 정식으로 들어가면 빠진다.
//			::PostMessage(m_hWnd, dID, dEvent, NULL); // 완전한 Packet Data return
	}
}

void CNetAuth::Update()
{
	// Socket Update;
	NFNetClientS::Update();


	// 기타처리
}

void CNetAuth::ProcessPacket(NFPacket* pPacket, int PacketLen)
{
	// 함수 호출
	(this->*OnMsgMap[pPacket->GetCommand()])(pPacket->m_Packet, pPacket->GetSize());
}

void CNetAuth::Parsed_IRC(CHAR* Packet, int Len)
{
	LPPKIRC pkIRC = (LPPKIRC)Packet;

	if(m_event)
		m_event->EventIRC(pkIRC->Key, pkIRC->Message);
}

void CNetAuth::Parsed_Dummy(CHAR* Packet, int Len)
{
}

void CNetAuth::Parsed_CMD(CHAR* Packet, int Len)
{
	LPPKCMD pkPar = (LPPKCMD)Packet;

	if(m_event)
		m_event->EventCMD(pkPar->dwCmd, pkPar->dwValue);
}

void CNetAuth::Parsed_IPLIST(CHAR* Packet, int Len)
{
	LPPKIPLIST lpPk = (LPPKIPLIST)Packet;

	char* Buffer_Out = (char*)Packet+sizeof(PKIPLIST);

	if(m_event)
		m_event->EventIPLIST(lpPk->Type, lpPk->Page, lpPk->Count, Buffer_Out);
}

void CNetAuth::Send_IRC(const char* strCommand, const char* strMsg)
{
//	Sync::CLive CL(&m_Cs);

	m_SendPacket.SetCommand(IRC);
	m_SendPacket.SetSize(sizeof(PKIRC));

	LPPKIRC lpIRC = (LPPKIRC)m_SendPacket.m_Packet;

	strcpy(lpIRC->Key, strCommand);
	strcpy(lpIRC->Message, strMsg);

	SendPost(m_SendPacket);
}

void CNetAuth::Send_AUTHOR(DWORD dwType, const char* strKey)
{
	m_SendPacket.SetCommand(AUTHOR);
	m_SendPacket.SetSize(sizeof(PKAUTHOR));

	LPPKAUTHOR lpPk = (LPPKAUTHOR)m_SendPacket.m_Packet;

	lpPk->Type = dwType;
	memcpy(lpPk->Key, strKey, sizeof(lpPk->Key));

	SendPost(m_SendPacket);
}

void CNetAuth::Send_CMD(DWORD dwCmd, DWORD dwValue)
{
	m_SendPacket.SetCommand(CMD);
	m_SendPacket.SetSize(sizeof(PKCMD));

	LPPKCMD lpPk = (LPPKCMD)m_SendPacket.m_Packet;

	lpPk->dwCmd = dwCmd;
	lpPk->dwValue = dwValue;

	SendPost(m_SendPacket);
}

void CNetAuth::Send_BUYITEMINFO(SHORT Count, const char* lpBuffer_In, SHORT Len)
{
	m_SendPacket.SetCommand(BUYITEMINFO);

	LPPKBUYITEMINFO lpPk = (LPPKBUYITEMINFO)m_SendPacket.m_Packet;

	lpPk->Count = Count;
	lpPk->Len = Len;
	char* pBuffer = (char*)m_SendPacket.m_Packet+sizeof(PKBUYITEMINFO);
	memcpy(pBuffer, lpBuffer_In, Len);

	m_SendPacket.SetSize(sizeof(PKBUYITEMINFO)+Len);

	SendPost(m_SendPacket);
}