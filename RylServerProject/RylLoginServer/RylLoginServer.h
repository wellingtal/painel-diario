#ifndef _RYL_LOGIN_SERVER_H_
#define _RYL_LOGIN_SERVER_H_

#include <Utility/ServerAppFramework/ServerWindowFramework.h>

#ifdef AUTH_MY
#include "NFAuthClient/AuthClient.h"
#endif


class CLimitUserByIP;
class CSessionPolicy;
class CProcessThread;

class CRylLoginServer : public CServerWindowFramework
#ifdef AUTH_MY
						, public CPacketEvent
#endif
{
public:

	static CRylLoginServer& GetInstance();

    // Desc : RylLoginCommands에서 호출하는 함수들.    
    void PrintStatistics();
    void PrintServerInfo();
    void ReloadSetup();
    
	static const TCHAR* GetDefaultLimitIPFileName();

#ifdef AUTH_MY

public:
	virtual void EventConnect(BOOL bConnect);

	virtual void EventIRC(CHAR* strCmd, CHAR* strMsg);
	virtual void EventCMD(DWORD dwCmd, DWORD dwValue);

	void Update(unsigned long dwTick);

#endif

protected:

	virtual bool ApplicationSpecificInit(const TCHAR* szCmdLine);

	bool InitializeCommand();
	bool InitializeMsgProc();

private:

    CRylLoginServer();
    virtual ~CRylLoginServer();
    
	CSessionPolicy*	m_lpUserSessionPolicy;
	CSessionPolicy*	m_lpAgentServerPolicy;	
};

#endif