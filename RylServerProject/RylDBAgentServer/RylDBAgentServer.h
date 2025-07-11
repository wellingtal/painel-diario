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

    // Desc : RylDBAgentCommands에서 호출하는 함수들.    
    void PrintStatistics();
    void PrintServerInfo();
    void ReloadSetup();

    void ConnectAll();                  // 로그인, UID서버로 연결을 시도한다
    void SendDBAgentManageInfo();       // 관리서버로 패킷 보냄
    void SendChannelData();             // 각 서버로 채널 관련 데이터 보냄
    void UpdateDBData(CPulse& Pulse);   // 남은 시간에 DB데이터 업데이트
    void ForceUpdateDBData();           // DB에 변경된 데이터를 전부 기록한다
    void ManageUnloadedData();          // 로드되지 않은 데이터를 관리한다

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

    bool ListenAll();               // 운영툴, 인증, 챗, 게임에 대해서 리스너를 생성해서 붙인다

    
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