#ifndef _RYL_AUTH_SERVER_H_
#define _RYL_AUTH_SERVER_H_

#include <Utility/ServerAppFramework/ServerWindowFramework.h>

#ifdef AUTH_MY
#include "NFAuthClient/AuthClient.h"
#endif

// 전방 참조
class CSessionPolicy;
class CLimitUserByIP;

class CRylAuthServer : public CServerWindowFramework
#ifdef AUTH_MY
						, public CPacketEvent
#endif
{
public:

	static CRylAuthServer& GetInstance();

	void InitializeClientListener();
    void ConnectToDBAgent();

    void PrintStatistics();
    void PrintServerInfo();
	void LogPerformance();
	void ReloadSetup();

	CLimitUserByIP* GetLimitUserByIP() { return m_lpLimitUserByIP; }

#ifdef AUTH_MY

public:
	virtual void EventConnect(BOOL bConnect);

	virtual void EventIRC(CHAR* strCmd, CHAR* strMsg);
	virtual void EventCMD(DWORD dwCmd, DWORD dwValue);

	void Update(unsigned long dwTick);

#endif

private:

	bool ApplicationSpecificInit(const TCHAR* szCmdLine);

	bool InitializeCommand();
	bool InitializeMsgProc();

    CRylAuthServer();
    virtual ~CRylAuthServer();
	
	CSessionPolicy*	m_lpDBASessionPolicy;
	CSessionPolicy* m_lpUserSessionPolicy;
	CLimitUserByIP*	m_lpLimitUserByIP;
	BOOL			m_bInitializedClientListener;

};


#endif