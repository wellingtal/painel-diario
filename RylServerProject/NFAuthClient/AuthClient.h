#pragma once

#include "NetAuth.h"
#include "NFIPSec.h"

extern CNetAuth	g_NetAuth;
extern NFIPSec  g_IPSec;

void GetMD5(char* file, char* outMD5);

class CPacketEvent : public CNetAuth::IEventListener
{
public:
	CPacketEvent();
	~CPacketEvent();

public:
	virtual void EventIRC(CHAR* strCmd, CHAR* strMsg);
	virtual void EventCMD(DWORD dwCmd, DWORD dwValue);
	virtual void EventIPLIST(CHAR Type, CHAR Page, SHORT Count, char* lpIPList_Out);

	virtual void EventConnect(BOOL bConnect);
	virtual void EventSocketClose();

	inline int	GetEasyCmd() { return m_EasyCmd; }

protected:
	int		m_Counter;
	int		m_EasyCmd;

	DWORD	m_dwServerType;
};