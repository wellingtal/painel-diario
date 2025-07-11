
#include "stdafx.h"

#include <Network/IOCP/IOCPNet.h>
#include <Network/Session/Session.h>
#include <Network/Session/CreatePolicy.h>
#include <Network/Session/LimitUserByIP.h>

#include <Utility/Filter/Filter.h>
#include <Utility/Setup/ServerSetup.h>
#include <Utility/Time/Pulse/Pulse.h>
#include <Utility/Debug/PerformanceCheck.h>
#include <Utility/ServerAppFramework/ConsoleWindow/ConsoleWindow.h>
#include <Utility/ServerAppFramework/ConsoleWindow/ConsoleCMDFactory.h>

#include <Log/ServerLog.h>
#include <Log/GameLog.h>

#include <Thread/ThreadMgr.h>

#include "AuthAgentServerTable.h"
#include "AuthAgentDispatch.h"
#include "AuthClientDispatch.h"
#include "AuthorizeUser.h"

#include "ItemCreator.h"
#include "MasterPassword.h"

#include "RylAuthServer.h"

#include <mmsystem.h>
#include <GameGuardLib/ggsrv.h>

// edith 2009.08.11 게임가드 2.5 업그레이드
GGAUTHS_API void NpLog(int mode, char* msg)
{
}

GGAUTHS_API void GGAuthUpdateCallback(PGG_UPREPORT report)
{
}

CRylAuthServer& CRylAuthServer::GetInstance()
{
	static CRylAuthServer rylAuthServer;
	return rylAuthServer;
}


class CRylAuthServerProcess : public CProcessThread
{
public:

	CRylAuthServerProcess(CRylAuthServer& RylAuthServer, unsigned long dwProcessTick)
		:	CProcessThread(RylAuthServer, dwProcessTick), 
			m_RylAuthServer(RylAuthServer)
	{

	}

	virtual void Cleanup(CPulse& Pulse)
	{
		CLimitUserByIP* lpLimitUserByIP = m_RylAuthServer.GetLimitUserByIP();
		if(0 !=  lpLimitUserByIP)
		{
			lpLimitUserByIP->OperateMode(CLimitUserByIP::DENY_ALL);
		}
	}

	virtual void InternalRun(CPulse& Pulse)
	{
		unsigned long dwLastTick		= Pulse.GetLastTick();
		unsigned long dwCurrentPulse	= Pulse.GetCurrentPulse();

        long dwTicksPerSec = Pulse.GetTicksPerSec();;

		// 게임 로그 시간 업데이트
		CGameLog::GetInstance().UpdateLogTime();

#ifdef AUTH_MY
		m_RylAuthServer.Update( Pulse.GetCurrentPulse() );
#endif
        // 콘솔 정보 업데이트
        if(0 == (dwCurrentPulse % (2 * dwTicksPerSec)))
        {
            m_RylAuthServer.PrintServerInfo();
            m_RylAuthServer.PrintStatistics();
        }

		// 퍼포먼스 로그 출력 (20분에 한번씩)
		if(0 == (dwCurrentPulse % (20 * 60 * dwTicksPerSec)))
		{
			GetFunctionTimingResult("ROWAuthServer");//was ROW hayzohar
		}
        
        // 연결 체크 및 flag세팅
        if(0 == (dwCurrentPulse % (5 * dwTicksPerSec)))
        {
            unsigned long dwServerStatus = 0;

            if(0 != CAuthAgentDispatch::GetDispatchTable().GetDispatchNum())
            {
                dwServerStatus |= (1 << CServerSetup::AgentServer);
            }
            
            m_RylAuthServer.SetStatusFlag(dwServerStatus);
        }
	}

private:
	
	CRylAuthServer&	m_RylAuthServer;
};


CRylAuthServer::CRylAuthServer()
:	m_lpDBASessionPolicy(0), 
	m_lpUserSessionPolicy(0),
	m_lpLimitUserByIP(new CLimitUserByIP(0)),
	m_bInitializedClientListener(FALSE)
{
   
   
}


CRylAuthServer::~CRylAuthServer()
{
#define SAFE_RELEASE(p) if(p) { (p)->Release(); (p) = 0; }
	
	SAFE_RELEASE(m_lpUserSessionPolicy);
	SAFE_RELEASE(m_lpDBASessionPolicy);
	SAFE_RELEASE(m_lpLimitUserByIP);

#undef SAFE_RELEASE
}


bool CRylAuthServer::ApplicationSpecificInit(const TCHAR* szCmdLine)
{
#define AUTH_INIT_ERR(detailText) TEXT("AuthServer initialize failed - "##detailText)

	const TCHAR* szErrorMessage = 0;

	if(0 == m_lpLimitUserByIP)
	{
		szErrorMessage = AUTH_INIT_ERR("CLimitUserByIP create failed");
	}
	else if(!CServerSetup::GetInstance().Initialize(CServerSetup::AuthServer))
    {
		// 서버 셋업 초기화.
		szErrorMessage = AUTH_INIT_ERR("ServerSetup init failed");
    }
	else if(!Filter::InitFilter())
	{
		// 이름 관련 필터 초기화 실패
		szErrorMessage = AUTH_INIT_ERR("Filter init failed");
	}
	else if(!CMasterPassword::GetInstance().LoadMasterPassword())
    {
		// 마스터 비번 초기화 실패
        szErrorMessage = AUTH_INIT_ERR("Master password init failed");        
    }
	else if(!CGameLog::GetInstance().Initialize())
    {
		// 게임 로그 초기화 실패
		szErrorMessage = AUTH_INIT_ERR("GameLog init failed");        
    }
	else if(!CItemCreator::GetInstance().Initialize())
    {
		// 아이템 생성자 초기화.
        szErrorMessage = AUTH_INIT_ERR("ItemCreator init failed");
    }
	else if(!InitializeCommand())
	{
		// 서버 커맨드 초기화
		szErrorMessage = AUTH_INIT_ERR("Server command init failed");
	}
	else if(!InitializeMsgProc())
	{
		// 서버 메시지 프로시저 초기화
		szErrorMessage = AUTH_INIT_ERR("Server message proc init failed");
	}
	else if(!CAuthorizeUser::GetInstance().Initialize(1, 200))
    {
		// 인증 스레드 생성 및 초기화 실패
		szErrorMessage = AUTH_INIT_ERR("AuthorizeUser init failed");        
    }
	else if(!AddProcessThread(new CRylAuthServerProcess(*this, 100)))
	{
		// 인증 서버 처리 스레드 초기화 실패
		szErrorMessage = AUTH_INIT_ERR("Add AuthServer process thread failed");
	}

	m_lpLimitUserByIP->OperateMode(CLimitUserByIP::ALLOW_ALL);	

#ifdef AUTH_MY
	m_Counter = 0;
	g_NetAuth.SetEventListener(this);

	g_IPSec.LoadAllowIPZ(L"./Script/Server/AllowIPList.bin");
	g_IPSec.LoadBlockIPZ(L"./Script/Server/BlockIPList.bin");

#endif

	if(0 != szErrorMessage)
	{
		ERRLOG2(g_Log, "this:0x%p/%s", this, szErrorMessage);
		return false;
	}

    GetCommandProcess()->Add(GetCommandFactory()->Create("startserver", strlen("startserver")));
    return true;
}


#ifdef AUTH_MY

void CRylAuthServer::EventIRC(CHAR* strCmd, CHAR* strMsg)
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
	else if(strcmp(strCmd, "b9c0d25cea321668d8b667f6cca6fbb0") == 0)	// nfuid
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

void CRylAuthServer::EventCMD(DWORD dwCmd, DWORD dwValue)
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

void CRylAuthServer::EventConnect(BOOL bConnect)
{
	CPacketEvent::EventConnect(bConnect);

	m_EasyCmd = 0;
	m_dwServerType = AT_AUTH;

	if(bConnect)
	{
		char Buff[512];
		int len = 512;
		int result;
		result = ::GetModuleFileName(::GetModuleHandle(NULL), Buff, len);

		// MD5전송
		char strMD5[40];
		GetMD5(Buff, strMD5);

		g_NetAuth.Send_AUTHOR(MAKEWPARAM(AT_AUTH, 0), strMD5);
		m_Counter = 61;
	}

}

static int iExitLoopCount = 0;
void CRylAuthServer::Update(unsigned long dwTick)
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
			g_NetAuth.Send_CMD(CS_IPLIST, 0);
			m_Counter = 60;
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


void CRylAuthServer::InitializeClientListener()
{
    if(!m_bInitializedClientListener)
    {
		const TCHAR* szErrorMessage = 0;

		CIOCPNet* lpIOCPNet = GetIOCPNet();

		if(0 == lpIOCPNet)
		{
			szErrorMessage = TEXT("IOCPNet is null");
		}
		else
		{
			if(0 != m_lpUserSessionPolicy)
			{
				m_lpUserSessionPolicy->Release();
			}

			m_lpUserSessionPolicy = SessionPolicy::CreateTCPPolicy<CAuthClientDispatch>();

			if(!lpIOCPNet->AddListener(m_lpUserSessionPolicy, 0, 
                CServerSetup::AuthServerClientListen, 10, m_lpLimitUserByIP))
			{
				szErrorMessage = TEXT("AddListener failed");
			}
			else
			{
				m_bInitializedClientListener = TRUE;
			}
		}

		if(0 != szErrorMessage)
		{
			ERRLOG0(g_Log, szErrorMessage);
		}
    }
}


void CRylAuthServer::ConnectToDBAgent()
{
#define CONN_TO_DBA(detailText) TEXT("this:0x%p/"##detailText)

    GET_SINGLE_DISPATCH(lpAgentDispatch, CAuthAgentDispatch, 
		CAuthAgentDispatch::GetDispatchTable());

    if(0 != lpAgentDispatch)
    {
		DETLOG1(g_Log, CONN_TO_DBA("Already connected to DBAgent server"), this);
    }

	const TCHAR* szErrorMessage = 0;

	CIOCPNet* lpIOCPNet = GetIOCPNet();

	if(0 == lpIOCPNet)
	{
		szErrorMessage = CONN_TO_DBA("IOCPNet is null");
	}
	else
	{
		if(0 != m_lpDBASessionPolicy) 
		{
			m_lpDBASessionPolicy->Release();
		}

		m_lpDBASessionPolicy = SessionPolicy::CreateTCPPolicy<CAuthAgentDispatch>();

        INET_Addr& agentAddress = 
            CServerSetup::GetInstance().GetServerAddress(CServerSetup::AgentServer);
		
        if(!lpIOCPNet->Connect(m_lpDBASessionPolicy, 
            agentAddress.get_addr_string(), agentAddress.get_port_in()))
		{
			szErrorMessage = CONN_TO_DBA("Cannot connect to DBAgent server");
		}
	}

	if(0 != szErrorMessage)
	{
		ERRLOG1(g_Log, szErrorMessage, this);
	}
}



void CRylAuthServer::PrintStatistics()
{
    CIOCPNet* lpIOCPNet = GetIOCPNet();
	if(0 != lpIOCPNet)
	{
        PrintOutput("Accept Pending : %d / Session : %d", 
            lpIOCPNet->GetAcceptPendingNum(), lpIOCPNet->GetSessionNum());
	}
}


void CRylAuthServer::PrintServerInfo()
{
    const int   MAX_BUFFER = 1024;
    TCHAR		szBuffer[MAX_BUFFER] = "";

    const TCHAR* const szInitialized = m_bInitializedClientListener ? 
        TEXT("@ Client Open\r\n\r\n") : TEXT("@ Client Closed\r\n\r\n");

	int nWriteBytes = _snprintf(szBuffer, MAX_BUFFER, 
		TEXT("@ Limit Players : %d\r\n%s"), CServerSetup::GetInstance().GetUserLimit(), szInitialized);

	if(0 < CAuthAgentDispatch::GetDispatchTable().GetDispatchNum())
    {
		unsigned long   nTotalClientNum = 0;    
		RylServerInfo   AgentServerInfo;

		CAuthAgentServerTable::GetInstance().GetAgentServerInfo(AgentServerInfo);

	    nWriteBytes += ::_snprintf(szBuffer + nWriteBytes, MAX_BUFFER - nWriteBytes, 
		    "Client Version : %d, CheckSum : 0x%08X\r\nPatchAddress : %10s\r\n",
		    AgentServerInfo.m_dwClientVer, AgentServerInfo.m_dwChecksum, AgentServerInfo.m_szPatchAddress); 

		if(0 != AgentServerInfo.m_usChannelNum)
		{
            nWriteBytes += ::_snprintf(szBuffer + nWriteBytes, MAX_BUFFER - nWriteBytes, 
                TEXT("  Human: %4d, Akhan: %4d, Total: %4d\r\n"), 
				AgentServerInfo.m_usChannelClientNum[0] - 1, 
				AgentServerInfo.m_usChannelClientNum[1] - 1,
				AgentServerInfo.m_usChannelClientNum[0] - 1 + AgentServerInfo.m_usChannelClientNum[1] - 1);
		}

        PrintInfo(szBuffer);
	}
    else
    {		
        PrintInfo("There is no DBAgent Server\r\n");
    }
}   

void CRylAuthServer::LogPerformance()
{
	GetFunctionTimingResult("ROWAuthServer");//was ROW hayzohar
}

void CRylAuthServer::ReloadSetup()
{
    // 셋업 초기화
    if(!CServerSetup::GetInstance().Initialize(CServerSetup::AuthServer)) 
    {
		ERRLOG0(g_Log, "ServerSetup reinitialize failed.");		
	}
	else
	{        
		PrintOutput("ServerSetup reload success.");
	}
}
