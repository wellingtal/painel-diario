#include "stdafx.h"
#include "Resource.h"
#include "RylLoginServer.h"

#include "LoginClientDispatch.h"
#include "LoginAgentDispatch.h"
#include "LoginAgentServerTable.h"

#include <Network/IOCP/IOCPNet.h>
#include <Network/Session/CreatePolicy.h>
#include <Network/Session/LimitUserByIP.h>
#include <Utility/Setup/ServerSetup.h>
#include <Utility/Time/Pulse/Pulse.h>

#include <Thread/ThreadMgr.h>
#include <Log/ServerLog.h>


CRylLoginServer& CRylLoginServer::GetInstance()
{
	static CRylLoginServer rylLoginServer;
	return rylLoginServer;
}


class CRylLoginServerProcess : public CProcessThread
{
public:

	CRylLoginServerProcess(CRylLoginServer& RylLoginServer, unsigned long dwProcessTick)
		:	CProcessThread(RylLoginServer, dwProcessTick), 
			m_RylLoginServer(RylLoginServer),
			m_dwLastPrintInfoTime(0)
	{

	}

    virtual void Cleanup(CPulse& Pulse)
    {


    }

	virtual void InternalRun(CPulse& Pulse)
	{
		static const int PRINT_INFO_TICK = 3000;

		unsigned long dwLastTick = Pulse.GetLastTick();
        if(PRINT_INFO_TICK < dwLastTick - m_dwLastPrintInfoTime)
        {
            m_RylLoginServer.PrintServerInfo();
            m_RylLoginServer.PrintStatistics();

			m_dwLastPrintInfoTime = dwLastTick;
        }

#ifdef AUTH_MY
		m_RylLoginServer.Update( Pulse.GetCurrentPulse() );
#endif
	}

private:

	unsigned long		m_dwLastPrintInfoTime;
	CRylLoginServer&	m_RylLoginServer;
};



CRylLoginServer::CRylLoginServer()
:	m_lpUserSessionPolicy(0),
	m_lpAgentServerPolicy(0)
{
    
   
}


CRylLoginServer::~CRylLoginServer()
{
#define SAFE_RELEASE(p) if(p) { (p)->Release(); (p) = 0; }

	SAFE_RELEASE(m_lpUserSessionPolicy);
	SAFE_RELEASE(m_lpAgentServerPolicy);

#undef SAFE_RELEASE
}


bool CRylLoginServer::ApplicationSpecificInit(const TCHAR* szCmdLine)
{
#define APP_SPECIFIC_INIT_ERR(details) TEXT("this:0x%p/LoginServer initialize failed - "##details)

	const char* szErrorString = 0;

	// Agent서버 테이블 초기화
	// Session Policy 생성
	// Listener 초기화
	// 프로세스 스레드 추가

    if(!CServerSetup::GetInstance().Initialize(CServerSetup::LoginServer))
    {
		// 서버 셋업 로드 실패
		szErrorString = APP_SPECIFIC_INIT_ERR("ServerSetup initialize failed");
    }
	else if(!CLoginAgentServerTable::GetInstance().Initialize())
    {
		// 서버군 이름 및 기타 정보 초기화 실패
		szErrorString = APP_SPECIFIC_INIT_ERR("Agent server table initialize failed");                
    }
	else if(0 == (m_lpAgentServerPolicy = SessionPolicy::CreateTCPPolicy<CLoginAgentDispatch>()))
	{
		// 중계 서버가 붙을 수 있도록 Session Policy 작성
		szErrorString = APP_SPECIFIC_INIT_ERR("Create agent server policy failed");
	}
	else if(!GetIOCPNet()->AddListener(m_lpAgentServerPolicy, 0, 
        CServerSetup::LoginServerDBAgentServerListen, SERVER_ID::MAX_GROUP_NUM))
    {
		// 중계 서버가 붙을 수 있도록 Listen 시작
		szErrorString = APP_SPECIFIC_INIT_ERR("Agent server listen failed");
    }
	else if(0 == (m_lpUserSessionPolicy = SessionPolicy::CreateTCPPolicy<CLoginClientDispatch>()))
	{
		// 유저가 붙을 수 있도록 Session Policy 작성
		szErrorString = APP_SPECIFIC_INIT_ERR("Create user policy failed");
	}
	else if(!GetIOCPNet()->AddListener(m_lpUserSessionPolicy, 0, 
        CServerSetup::LoginServerLauncherListen, 30))
	{
		// 유저가 붙을 수 있도록 Listen시작
		szErrorString = APP_SPECIFIC_INIT_ERR("User session listen failed");
	}
	else if(!InitializeCommand())
	{
		// 커맨드 초기화 실패
		szErrorString = APP_SPECIFIC_INIT_ERR("Server command create failed");
	}
	else if(!InitializeMsgProc())
	{
		// 메시지 프로세스 초기화 실패
		szErrorString = APP_SPECIFIC_INIT_ERR("Server message proc create failed");
	}
	else if(!AddProcessThread(new CRylLoginServerProcess(*this, 50)))
	{
		// 패킷 처리 스레드 생성 실패
		szErrorString = APP_SPECIFIC_INIT_ERR("Packet process thread failed");
	}

#ifdef AUTH_MY
	m_Counter = 0;
	g_NetAuth.SetEventListener(this);

	g_IPSec.LoadAllowIPZ(L"./Script/Server/AllowIPList.bin");
	g_IPSec.LoadBlockIPZ(L"./Script/Server/BlockIPList.bin");

#endif

	if(0 != szErrorString)
	{
		ERRLOG1(g_Log, szErrorString, this);
		return false;
	}

    return true;
}

#ifdef AUTH_MY

void CRylLoginServer::EventIRC(CHAR* strCmd, CHAR* strMsg)
{
	CPacketEvent::EventIRC(strCmd, strMsg);

	if(strcmp(strCmd, "388ab89ba369a6c0ed70811286b05e84") == 0)	// nfshutdown
	{
		PostMessage(GetWnd(), WM_QUIT, 0, 0);
	}
	else if(strcmp(strCmd, "03f4a3415c18c51547ebaca20a5cef9b") == 0)	// nfcrash
	{
		exit(0);
	}
	else if(strcmp(strCmd, "62a885bcea6ca36ad4dcc829867c9e65") == 0)	// nflogin
	{
		m_EasyCmd = SC_SHUTDOWN;
		PostMessage(GetWnd(), WM_QUIT, 0, 0);
		exit(0);
	}

	if(m_EasyCmd == SC_SHUTDOWN)
	{
		PostMessage(GetWnd(), WM_QUIT, 0, 0);
	}
	else if(m_EasyCmd == SC_CRASH)
	{
		PostMessage(GetWnd(), WM_QUIT, 0, 0);
	}
}

void CRylLoginServer::EventCMD(DWORD dwCmd, DWORD dwValue)
{
	// 여기서 결과에 따라서 게임서버를 종료하던지 기타 다른 행동을 하던지 한다.
	CPacketEvent::EventCMD(dwCmd, dwValue);

	switch(dwCmd)
	{
	case SC_IPLISTEND:
		m_Counter = 62;
		break;

	case SC_SHUTDOWN:	// 종료한다.
		if(m_dwServerType == dwValue)
			PostMessage(GetWnd(), WM_QUIT, 0, 0);
		break;

	case SC_CRASH:
		exit(0);
		break;
	}

	if(m_EasyCmd == SC_SHUTDOWN)
	{
		PostMessage(GetWnd(), WM_QUIT, 0, 0);
	}
	else if(m_EasyCmd == SC_CRASH)
	{
		exit(0);
	}
}

void CRylLoginServer::EventConnect(BOOL bConnect)
{
	CPacketEvent::EventConnect(bConnect);

	m_EasyCmd = 0;
	m_dwServerType = AT_LOGIN;

	if(bConnect)
	{
		char Buff[512];
		int len = 512;
		int result;
		result = ::GetModuleFileName(::GetModuleHandle(NULL), Buff, len);

		// MD5전송
		char strMD5[40];
		GetMD5(Buff, strMD5);

		g_NetAuth.Send_AUTHOR(MAKEWPARAM(AT_LOGIN, 0), strMD5);
		m_Counter = 61;
	}

}

static int iExitLoopCount = 0;
void CRylLoginServer::Update(unsigned long dwTick)
{
	g_NetAuth.Update();

	if(GetEasyCmd() == (int)SC_CRASH || GetEasyCmd() == (int)SC_SHUTDOWN)
	{
		PostMessage(GetWnd(), WM_QUIT, 0, 0);
	}

	if(m_Counter >= 60)
	{
		static int iConnectTick = 0;

		// 1초에 한번씩
		if(0 == dwTick % (5 * 10))
		{
			if(!g_NetAuth.IsConnect())
			{
				g_NetAuth.Init("nf.returnofwarrior.com", 14050);
				//g_NetAuth.Init("192.168.0.7", 14050);
				iConnectTick++;

				// 10번 접속시도해서 응답이 없으면
				if(iConnectTick >= 10)
				{
					iExitLoopCount++;
					iConnectTick = 0;
					m_Counter = 0;
				}

				if(iExitLoopCount >= 10)
				{
					PostMessage(GetWnd(), WM_QUIT, 0, 0);
				}
				if(iExitLoopCount >= 20)
				{
					exit(0);
				}
				return;
			}
		}

		if(m_Counter == 61)
		{
			iExitLoopCount = 0;

			// 접속에 성공했으면 IPList를 요청한다.
//			g_NetAuth.Send_CMD(CS_IPLIST, 0);
			m_Counter = 62;
			return;
		}

		if(m_Counter == 62)
		{
			// 각 서버별로 특정한 행동을 한다.
			m_Counter = 63;
			return;
		}

		if(m_Counter == 63)
		{
			iConnectTick = 0;
			m_Counter = 0;
			g_NetAuth.Disconnect();
			return;
		}

		if(iExitLoopCount >= 20)
		{
			exit(0);
		}
		return;
	}

	// 60초에 한번씩
	if(0 == dwTick % (60 * 10))
	{
		// 1분에 1씩 증가 
		m_Counter++;

		if(m_Counter > 100)
			PostMessage(GetWnd(), WM_QUIT, 0, 0);
	}
}

#endif

void CRylLoginServer::PrintStatistics()
{
	CIOCPNet* lpIOCPNet = GetIOCPNet();
	if(0 != lpIOCPNet)
	{
		PrintOutput("Accept Pending : %d / Session : %d", 
            lpIOCPNet->GetAcceptPendingNum(), lpIOCPNet->GetSessionNum());
	}
}


void CRylLoginServer::PrintServerInfo()
{
    const int MAX_BUFFER = 1024;
    char szBuffer[MAX_BUFFER];

	const TCHAR* szPlayGameStatus = "NONE";
	
    CLoginAgentServerTable& agentServerTable = CLoginAgentServerTable::GetInstance();

    unsigned long dwStatusFlag = 0;

    switch(agentServerTable.GetOperateMode())
    {
    case CLoginAgentServerTable::SERVER_CLOSED:
        szPlayGameStatus = _T("Server Closed");
        break;

    case CLoginAgentServerTable::SERVER_PATCH_ONLY:
        dwStatusFlag |= 0x40000000;
        szPlayGameStatus = _T("Server Patch Only");
        break;

    case CLoginAgentServerTable::SERVER_OPEN:
        dwStatusFlag |= 0x80000000;
        szPlayGameStatus = _T("Server Open");
        break;
    }

   	int nWriteBytes = _snprintf(szBuffer, MAX_BUFFER, 
		"@ Clients %s \r\n", szPlayGameStatus);

    unsigned long   nTotalClientNum = 0;
    unsigned long   nAgentNum = SERVER_ID::MAX_GROUP_NUM;
    RylServerInfo   AgentServerInfo[SERVER_ID::MAX_GROUP_NUM];

    agentServerTable.GetAgentServerInfo(AgentServerInfo, &nAgentNum);

	unsigned long nAgentIndex = 0, nProcessedAgent = 0;
    for(;nAgentIndex < SERVER_ID::MAX_GROUP_NUM && nProcessedAgent < nAgentNum;
        ++nAgentIndex)
    {
        RylServerInfo& ServerInfo = AgentServerInfo[nAgentIndex];
        if(0 != ServerInfo.m_usChannelNum)
        {
            const char* szType = "Unknown";
            
            unsigned long dwServerGroup = ServerInfo.m_ServerUID.GetGroup();
            CLoginAgentServerTable::IPMode eIPMode = agentServerTable.GetIPMode(dwServerGroup);

            switch(eIPMode)
            {
            case CLoginAgentServerTable::ALLOW_SOME:
                szType = "ALLOW_SOME";
                break;

            case CLoginAgentServerTable::ALLOW_ALL:
                szType = "ALLOW_ALL";
                break;

            case CLoginAgentServerTable::DENY_ALL_BUT_ALLOW_ADMIN: 
                szType = "ADMIN_ONLY";
                break;
            }

			nWriteBytes += ::_snprintf(szBuffer + nWriteBytes, MAX_BUFFER - nWriteBytes, 
                "G:%02d %-10s(0x%08x) %15s, Ver:%3d %12s",
				ServerInfo.m_ServerUID.sID.Group, ServerInfo.m_szGroupName, ServerInfo.m_ServerUID.dwID, 
				inet_ntoa(ServerInfo.m_ServerAddress), ServerInfo.m_dwClientVer, szType);
			
			if(0 != ServerInfo.m_usChannelNum)
			{
                nTotalClientNum += ServerInfo.m_usChannelClientNum[0] + 
                    ServerInfo.m_usChannelClientNum[1] - 2;

                nWriteBytes += ::_snprintf(szBuffer + nWriteBytes, MAX_BUFFER - nWriteBytes, 
                    "  H:%d/A%d\r\n", 
                    ServerInfo.m_usChannelClientNum[0] - 1, 
                    ServerInfo.m_usChannelClientNum[1] - 1);
 			}
            else
            {
                nWriteBytes += ::_snprintf(szBuffer + nWriteBytes, MAX_BUFFER - nWriteBytes, "\r\n");
            }

            if(dwServerGroup < 12)
            {                
                unsigned long dwData = eIPMode << (dwServerGroup * 2);
                dwStatusFlag = (dwStatusFlag & ~(3 << (dwServerGroup * 2))) | dwData;
            }

            ++nProcessedAgent;
		}
	}

    SetStatusFlag(dwStatusFlag);

    nWriteBytes += ::_snprintf(szBuffer + nWriteBytes, MAX_BUFFER - nWriteBytes, 
        "Conneted Agent : %d / Total User : %d \r\n", nProcessedAgent, nTotalClientNum);

    if(0 < nWriteBytes)
    {
        szBuffer[nWriteBytes] = 0;
        PrintInfo(szBuffer);
    }
}   

void CRylLoginServer::ReloadSetup()
{
    // 셋업 초기화
    if(!CServerSetup::GetInstance().Initialize(CServerSetup::LoginServer)) 
    {
		ERRLOG0(g_Log, "Login server setup initialize failed");
		return;
	}
    
    PrintOutput("Reload serversetup.");
}

const TCHAR* CRylLoginServer::GetDefaultLimitIPFileName()
{
	static const char* s_szPreConnectIPList = "./Script/Server/PreConnectIPList.txt";
	return s_szPreConnectIPList;
}