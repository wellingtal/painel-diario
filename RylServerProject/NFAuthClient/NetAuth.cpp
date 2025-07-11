#include "NetAuth.h"

using namespace NaveNet;

CNetAuth::CNetAuth(void) : m_event(0)
{
	// �Լ��� Map���� �����Ѵ�.
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
	// ���Ͽ��� �Ͼ �̺�Ʈ�� ó���ϱ� ���Ѱ�.
	if(dID == WM_CONNECT)			// ���� �̺�Ʈ
	{
		if(m_event)
			m_event->EventConnect(dEvent);
	}
	else if(dID == WM_SOCK_CLOSE)	// ���� ���� �̺�Ʈ
	{
		if(m_event)
			m_event->EventSocketClose();
	}
	else if(dID == WM_RECV_MSG)		// ��Ŷ �Ѱ� ���� �̺�Ʈ
	{
		// Test�Ҷ��� ���ȴ� ���߿� �������� ���� ������.
//			::PostMessage(m_hWnd, dID, dEvent, NULL); // ������ Packet Data return
	}
}

void CNetAuth::Update()
{
	// Socket Update;
	NFNetClientS::Update();


	// ��Ÿó��
}

void CNetAuth::ProcessPacket(NFPacket* pPacket, int PacketLen)
{
	// �Լ� ȣ��
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