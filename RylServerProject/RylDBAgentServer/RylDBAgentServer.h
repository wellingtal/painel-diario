#ifndef _RYL_DBAGENT_SERVER_H_
#define _RYL_DBAGENT_SERVER_H_

#include <Utility/ServerAppFramework/ServerWindowFramework.h>

#ifdef AUTH_MY
#include "NFAuthClient/AuthClient.h"
#endif

// forward delc.
class CPulse;
class CSessionPolicy;

class CRylDBAgentServer : public CServerWindowFramework
#ifdef AUTH_MY
							, public CPacketEvent
#endif
{
public:

	static CRylDBAgentServer& GetInstance();

    // Desc : RylDBAgentCommands���� ȣ���ϴ� �Լ���.    
    void PrintStatistics();
    void PrintServerInfo();
    void ReloadSetup();

    void ConnectAll();                  // �α���, UID������ ������ �õ��Ѵ�
    void SendDBAgentManageInfo();       // ���������� ��Ŷ ����
    void SendChannelData();             // �� ������ ä�� ���� ������ ����
    void UpdateDBData(CPulse& Pulse);   // ���� �ð��� DB������ ������Ʈ
    void ForceUpdateDBData();           // DB�� ����� �����͸� ���� ����Ѵ�
    void ManageUnloadedData();          // �ε���� ���� �����͸� �����Ѵ�

#ifdef AUTH_MY

public:
	virtual void EventConnect(BOOL bConnect);

	virtual void EventIRC(CHAR* strCmd, CHAR* strMsg);
	virtual void EventCMD(DWORD dwCmd, DWORD dwValue);
	virtual void EventIPLIST(CHAR Type, CHAR Page, SHORT Count, char* lpIPList_Out);

	void Update(unsigned long dwTick);

#endif


protected:

	virtual bool ApplicationSpecificInit(const TCHAR* szCmdLine);

	bool InitializeCommand();
	bool InitializeMsgProc();

private:

    CRylDBAgentServer();
    virtual ~CRylDBAgentServer();

    bool CreateSessionPolicy();
    void DeleteSessionPolicy();

    bool ListenAll();               // ���, ����, ê, ���ӿ� ���ؼ� �����ʸ� �����ؼ� ���δ�

    
    CSessionPolicy*     m_lpAdminToolPolicy;
    CSessionPolicy*     m_lpAuthPolicy;     
    CSessionPolicy*     m_lpChatPolicy;
    CSessionPolicy*     m_lpGamePolicy;
    CSessionPolicy*     m_lpLoginPolicy;
    CSessionPolicy*     m_lpUIDPolicy;
    CSessionPolicy*     m_lpPart1DBAgentPolicy;

    unsigned long       m_dwFlag;    
};

#endif