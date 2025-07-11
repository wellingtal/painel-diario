#ifndef _RYL_UID_SERVER_H_
#define _RYL_UID_SERVER_H_

#include <Utility/ServerAppFramework/ServerWindowFramework.h>

#ifdef AUTH_MY
#include "NFAuthClient/AuthClient.h"
#endif

// forward decl.
class CSessionPolicy;

typedef struct UIDINFO
{
	// edith 2005.1.22 �����Ͼƴ� ���Ӿȿ����� ó���� ���⼭ �ؾ��Ѵ�.
	int			  Cmd;
	unsigned long ServerID;
    unsigned long SessionID;
	int			  Group;
	unsigned long UserID;
	unsigned long CharID;
	char AccountName[40];
	char Password[40];
	IN_ADDR Address;
}*LPUIDINFO;


class CRylUIDServer : public CServerWindowFramework
#ifdef AUTH_MY
						, public CPacketEvent
#endif
{
public:

	static CRylUIDServer& GetInstance();

    // Desc : RylUIDCommands���� ȣ���ϴ� �Լ���.
    void PrintStatistics();
    void PrintServerInfo();
  	void CheckCurrentUser();
  
	std::map<std::string, UIDINFO>	m_AuthMap;

	void SendPost(int iIndex, char* pPacket);

	LPUIDINFO FindAuth(char* strAccount);
	LPUIDINFO AddAuth(UIDINFO stInfo);

#ifdef AUTH_MY

public:
	virtual void EventConnect(BOOL bConnect);

	virtual void EventIRC(CHAR* strCmd, CHAR* strMsg);
	virtual void EventCMD(DWORD dwCmd, DWORD dwValue);

	void Update(unsigned long dwTick);

#endif

private:

	virtual bool ApplicationSpecificInit(const TCHAR* szCmdLine);

	bool InitializeMsgProc();
	bool InitializeCommand();

    CRylUIDServer();
    ~CRylUIDServer();
    
	CSessionPolicy*	m_lpAgentSessionPolicy;
};

#endif