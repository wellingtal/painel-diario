#ifndef _RYL_GAME_SERVER_H_
#define _RYL_GAME_SERVER_H_

#include <Utility/ServerAppFramework/ServerWindowFramework.h>

#ifdef AUTH_MY
#include "NFAuthClient/AuthClient.h"
#endif


class CSessionPolicy;
class CRylGameServer;
class CLimitUserByIP;

class CRylGameServer : public CServerWindowFramework
#ifdef AUTH_MY
						, public CPacketEvent
#endif
{
public:

	static CRylGameServer& GetInstance();

    // Desc : RylGameCommands에서 호출하는 함수들.
    void StartServer();

	void ConnectToRegularAgent(void);
    void ConnectToAgent(void);
    void ConnectToLogServer(void);
    void ConnectToChatServer(void);

    void PrintStatistics(void);
    void PrintServerInfo(void);
    void ReloadSetup(void);
       
	CLimitUserByIP* GetClientLimit(void)	{ return m_lpClientLimit;	}

#ifdef AUTH_MY

public:
	virtual void EventConnect(BOOL bConnect);

	virtual void EventIRC(CHAR* strCmd, CHAR* strMsg);
	virtual void EventCMD(DWORD dwCmd, DWORD dwValue);

	void Update(unsigned long dwTick);

#endif


private:

	virtual bool ApplicationSpecificInit(const TCHAR* szCmdLine);
	bool InitializeMsgProc(void);
	bool InitializeCommand(void);
	bool AddGameProcessThread(void);

    CRylGameServer();
    ~CRylGameServer();

    bool InitializeGameObject(void);
    void DestoryGameObject(void);
    
	CSessionPolicy*	m_lpClientPolicy;
	CSessionPolicy*	m_lpRegularAgentPolicy;
	CSessionPolicy* m_lpAgentPolicy;
	CSessionPolicy* m_lpChatPolicy;
	CSessionPolicy* m_lpLogPolicy;
	CLimitUserByIP*	m_lpClientLimit;

    bool m_bStartServer;
    bool m_bInitialized;
};

#endif