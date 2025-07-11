#pragma once

#include "NaveNet/NFNet.h"
#include "NaveNet/NFNetClientS.h"

#include "Packet.h"

class CNetAuth : public NaveNet::NFNetClientS
{
public:
	CNetAuth(void);
	~CNetAuth(void);

public:
	//////////////////////////////////////////////////////////////////////
	// Socket -> Client�� ��Ŷ�� �����ϱ� ���� �����ϴ� Listener�̴�.
	//////////////////////////////////////////////////////////////////////
	class IEventListener
	{
	public:
		virtual ~IEventListener() {}
		virtual void EventIRC(CHAR* strCmd, CHAR* strMsg)=0;
		virtual void EventCMD(DWORD dwCmd, DWORD dwValue)=0;		
		virtual void EventIPLIST(CHAR Type, CHAR Page, SHORT Count, char* lpIPList_Out)=0;		
		virtual void EventConnect(BOOL bConnect)=0;
		virtual void EventSocketClose()=0;
	};

	void	SetEventListener(IEventListener* event)
	{
		m_event = event;
	}

public:
	virtual void	Update();

	//////////////////////////////////////////////////////////////////////
	// ToServer�� ���ǵ� ��Ŷ���
	// ���޹��� ��Ŷ�� ó���� ��Ŷ�Լ� ������ Recv_��Ŷ�̸�(CHAR* Packet, int Len) �̵ȴ�.
	//////////////////////////////////////////////////////////////////////
	void	Send_IRC(const char* strCommand, const  char* strMsg);
	void	Send_AUTHOR(DWORD dwType, const char* strKey);
	void	Send_CMD(DWORD dwCmd, DWORD dwValue);
	void	Send_BUYITEMINFO(SHORT Count, const char* lpBuffer_In, SHORT Len);

	void	Disconnect();

private:
	//////////////////////////////////////////////////////////////////////
	// CNetAuth���� ���Ǵ� ��������
	//////////////////////////////////////////////////////////////////////
	//	Nave::Sync::CSync					m_Cs;

	// Send�� �ϱ����� ��Ŷ�� ���鶧 ���ȴ�.
	NaveNet::NFPacket					m_SendPacket;

	IEventListener*						m_event;

private:
	void	(CNetAuth::*OnMsgMap[MaxPacket])(CHAR* Packet, int Len);

	// ���� �κп��� �������� �̺�Ʈ�� ó���Ҷ�
	void	OnSocketEvent(DWORD dID, DWORD dEvent);

	//////////////////////////////////////////////////////////////////////
	// FromServer�� ���ǵ� ��Ŷ��� (Ŭ���̾�Ʈ�� �������.)
	// ���޹��� ��Ŷ�� ó���� ��Ŷ�Լ� ������ Recv_��Ŷ�̸�(CHAR* Packet, int Len) �̵ȴ�.
	//////////////////////////////////////////////////////////////////////

	// IRC Packet ó���Լ�
	void	Parsed_CMD(CHAR* Packet, int Len);
	void	Parsed_IPLIST(CHAR* Packet, int Len);

	void	Parsed_Dummy(CHAR* Packet, int Len);
	void	Parsed_IRC(CHAR* Packet, int Len);

	void	ProcessPacket(NaveNet::NFPacket* pPacket, int PacketLen);

};

