#ifndef _RYL_CHAT_SERVER_H_
#define _RYL_CHAT_SERVER_H_

#include <Utility/ServerAppFramework/ServerWindowFramework.h>

#ifdef AUTH_MY
#include "NFAuthClient/AuthClient.h"
#endif

// ���� ����
class CSessionPolicy;

class CRylChatServer : public CServerWindowFramework
#ifdef AUTH_MY
						, public CPacketEvent
#endif
{
public:

	static CRylChatServer& GetInstance();

    // Desc : �߰輭���� �����Ѵ�.
    void ConnectToAgentServer();

	// Desc : RylChatCommands���� ȣ���ϴ� �Լ���.
	void PrintStatistics();
	void PrintServerInfo();
	void ReloadSetup();

    int MakeChatLog(char* szBuffer, size_t nBufferLength);

#ifdef AUTH_MY

public:
	virtual void EventConnect(BOOL bConnect);

	virtual void EventIRC(CHAR* strCmd, CHAR* strMsg);
	virtual void EventCMD(DWORD dwCmd, DWORD dwValue);

	void Update(unsigned long dwTick);

#endif

private:

	virtual bool ApplicationSpecificInit(const TCHAR* szCmdLine);
	bool InitializeCommand();
	bool InitializeMsgProc();

	CRylChatServer();
	virtual ~CRylChatServer();

	CSessionPolicy*	m_lpClientPolicy;
	CSessionPolicy*	m_lpGameServerPolicy;
	CSessionPolicy*	m_lpAgentSessionPolicy;
    CSessionPolicy* m_lpChatToolPolicy;
	time_t			m_tServerStart;
};

#endif