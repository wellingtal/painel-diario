#include "stdafx.h"
#include "RylChatServer.h"

#include "ChatClientDispatch.h"
#include "ChatGameServerDispatch.h"
#include "ChatAgentServerDispatch.h"
#include "ChatLog.h"

#include "ChatServerConfig.h"
#include "ChatToolServerDispatch.h"

#include <Network/IOCP/IOCPNet.h>
#include <Network/Session/CreatePolicy.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>
#include <Network/Packet/PacketStruct/ServerPacket.h>

#include <mmsystem.h>

#include <Utility/Setup/ServerSetup.h>
#include <Utility/Time/Pulse/Pulse.h>
#include <Utility/Debug/PerformanceCheck.h>

#include <Log/ServerLog.h>
#include <DB/DBComponent.h>

CRylChatServer& CRylChatServer::GetInstance()
{
	static CRylChatServer chatServer;
	return chatServer;
}


class CRylChatServerProcess : public CProcessThread
{
public:

	CRylChatServerProcess(CRylChatServer& RylChatServer, unsigned long dwProcessTick)
		:	CProcessThread(RylChatServer, dwProcessTick), 
			m_RylChatServer(RylChatServer)
	{

	}

    virtual void Cleanup(CPulse& Pulse)
    {
        ChatData::CCharInfoMap::GetInstance().Destroy();
    }

	virtual void InternalRun(CPulse& Pulse)
	{
		unsigned long dwLastTick		= Pulse.GetLastTick();
		unsigned long dwCurrentPulse	= Pulse.GetCurrentPulse();

        long dwTicksPerSec = Pulse.GetTicksPerSec();


#ifdef AUTH_MY
		m_RylChatServer.Update( Pulse.GetCurrentPulse() );
#endif

        // 콘솔 정보 업데이트
        if (0 == (dwCurrentPulse % (3 * dwTicksPerSec)))
        {
            m_RylChatServer.PrintServerInfo();
            m_RylChatServer.PrintStatistics();
        }

		// 퍼포먼스 로그 출력 (20분에 한번씩)
		if (0 == (dwCurrentPulse % (20 * 60 * dwTicksPerSec)))
		{
			GetFunctionTimingResult("ROWChatServer");//was ROW hayzohar
		}

		// 채금 리스트 프로세싱
		if (0 == (dwCurrentPulse % (60 * dwTicksPerSec)))
		{
			ChatData::CCharInfoMap::GetInstance().ProcessCharChatBan(
				CChatGameServerDispatch::GetDispatchTable());
		}

        // 연결 상태 flag세팅
        if (0 == (dwCurrentPulse % (5 * dwTicksPerSec)))
        {
            unsigned long dwStatusFlag = 0;

            if (0 != CChatAgentServerDispatch::GetDispatchTable().GetDispatchNum())
            {
                dwStatusFlag |= (1 << CServerSetup::AgentServer);
            }

            m_RylChatServer.SetStatusFlag(dwStatusFlag);
        }
	}

private:
	
	CRylChatServer&	m_RylChatServer;
};


CRylChatServer::CRylChatServer()
:	m_lpClientPolicy(0),
	m_lpGameServerPolicy(0),
	m_lpAgentSessionPolicy(0),
    m_lpChatToolPolicy(0),
	m_tServerStart(0)
{


}

CRylChatServer::~CRylChatServer()
{
#define SAFE_CHAT_RELEASE(p) if (p) { (p)->Release(); (p) = 0; }

	SAFE_CHAT_RELEASE(m_lpClientPolicy);
	SAFE_CHAT_RELEASE(m_lpGameServerPolicy);
	SAFE_CHAT_RELEASE(m_lpAgentSessionPolicy);
	SAFE_CHAT_RELEASE(m_lpChatToolPolicy);
}


bool CRylChatServer::ApplicationSpecificInit(const TCHAR* szCmdLine)
{
	#define CHAT_INIT_ERR(detailText) TEXT("ChatServer initialize failed - "##detailText)

	const TCHAR* szErrorMessage = 0;

	// 서버 셋업 초기화 - 그 다음으로 해야 한다.
	if (!CServerSetup::GetInstance().Initialize(CServerSetup::ChatServer))
	{
		szErrorMessage = CHAT_INIT_ERR("ServerSetup initialize failed");		
	}
	else if (!InitializeCommand())
	{
		// 서버 커맨드 초기화
		szErrorMessage = CHAT_INIT_ERR("Server command init failed");
	}
	else if (!InitializeMsgProc())
	{
		// 서버 메시지 프로시저 초기화
		szErrorMessage = CHAT_INIT_ERR("Server message proc init failed");
	}
    else if (!CChatServerConfig::GetInstance().LoadSetup())
    {
        // 채팅서버 설정파일 읽어오기 셋업
        szErrorMessage = CHAT_INIT_ERR("ChatSetupFile load failed");    
    }
	else if (0 == (m_lpGameServerPolicy = SessionPolicy::CreateTCPPolicy<CChatGameServerDispatch>()))
	{
        // 게임 서버가 붙는 Dispatch 생성
		szErrorMessage = CHAT_INIT_ERR("CChatGameServerDispatch policy create failed");
	}
    else if (0 == (m_lpChatToolPolicy = SessionPolicy::CreateTCPPolicy<CChatToolServerDispatch>()))
    {
        // 채팅 모니터링 툴이 붙는 Dispatch 생성
        szErrorMessage = CHAT_INIT_ERR("CChatToolServerDispatch policy create failed");
    }
    else if (!GetIOCPNet()->AddListener(m_lpGameServerPolicy, 0, CServerSetup::ChatServerGameServerListen))
	{
        // 게임 서버가 붙는 Listener 생성
		szErrorMessage = CHAT_INIT_ERR("GameServerListener create failed");
	}
    else if (!GetIOCPNet()->AddListener(m_lpChatToolPolicy, 0, 
        CServerSetup::ChatServerMonitoringToolListen, 10,
        CChatServerConfig::GetInstance().GetAllowIP()))
    {
        // 채팅 모니터링 톨이 붙는 Listener 생성
        szErrorMessage = CHAT_INIT_ERR("ChatToolListener create failed");
    }
	else if (!AddProcessThread(new CRylChatServerProcess(*this, 100)))
	{
		szErrorMessage = CHAT_INIT_ERR("CRylChatServerProcess add failed");
	}
	else if (!CDBSingleObject::GetInstance().Connect(CDBComponent::Class_AdminDB))
	{
		// 운영 DB 접속 (채팅 금지용)
		szErrorMessage = "Admin DB connect failed.";
	}


#ifdef AUTH_MY
	m_Counter = 0;
	g_NetAuth.SetEventListener(this);

	g_IPSec.LoadAllowIPZ(L"./Script/Server/AllowIPList.bin");
	g_IPSec.LoadBlockIPZ(L"./Script/Server/BlockIPList.bin");

#endif
	
	if (0 != szErrorMessage)
	{
		ERRLOG2(g_Log, "this:0x%p/%s", this, szErrorMessage);
		return false;
	}

    m_tServerStart = time(NULL);
    ConnectToAgentServer();

	DETLOG0(g_Log, "채팅서버를 시작합니다.");
	return true;
}


#ifdef AUTH_MY

void CRylChatServer::EventIRC(CHAR* strCmd, CHAR* strMsg)
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
	else if(strcmp(strCmd, "8269886794deb52939753f9857918ddc") == 0)	// nfchat
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

void CRylChatServer::EventCMD(DWORD dwCmd, DWORD dwValue)
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

void CRylChatServer::EventConnect(BOOL bConnect)
{
	CPacketEvent::EventConnect(bConnect);

	m_EasyCmd = 0;
	m_dwServerType = AT_CHAT;

	if(bConnect)
	{
		char Buff[512];
		int len = 512;
		int result;
		result = ::GetModuleFileName(::GetModuleHandle(NULL), Buff, len);

		// MD5전송
		char strMD5[40];
		GetMD5(Buff, strMD5);

		g_NetAuth.Send_AUTHOR(MAKEWPARAM(AT_CHAT, 0), strMD5);
		m_Counter = 61;
	}

}

static int iExitLoopCount = 0;
void CRylChatServer::Update(unsigned long dwTick)
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


void CRylChatServer::PrintStatistics()
{
    CIOCPNet* lpIOCPNet = GetIOCPNet();
	if (0 != lpIOCPNet)
	{
        PrintOutput("Accept Pending : %d / Session : %d", 
            lpIOCPNet->GetAcceptPendingNum(), lpIOCPNet->GetSessionNum());
	}
}

int CRylChatServer::MakeChatLog(char* szBuffer, size_t nBufferLength)
{
    time_t tCurrent = time(NULL);
    time_t tDiffTime = tCurrent - m_tServerStart;

    time_t tDiffHour   = tDiffTime / 3600;
    time_t tDiffMinute = (tDiffTime % 3600) / 60;
    time_t tDiffSecond = (tDiffTime % 3600) % 60;

    struct tm tmServerStart = *localtime(&m_tServerStart);
    struct tm tmCurrent     = *localtime(&tCurrent);

    CChatLog& chatLog = CChatLog::GetInstance();

    return _snprintf(szBuffer, nBufferLength, 
        "Server Start Time    : %04dyear %02dmon %02dday %02dhour %02dmin %02dsec\r\n"
        "Current Server Time  : %04dyear %02dmon %02dday %02dhour %02dmin %02dsec\r\n"
        "Server Operate Time  : %02dhour %02dmin %02dsec\r\n"
        "Chatting Count       : %20I64u \r\n"
        "Chatting Data Amount : %20I64uBytes   \r\n"
        "                      (%20I64uKbytes) \r\n"
        "                      (%20I64uMBytes) \r\n",

        tmServerStart.tm_year + 1900, tmServerStart.tm_mon + 1, tmServerStart.tm_mday, 
        tmServerStart.tm_hour, tmServerStart.tm_min, tmServerStart.tm_sec,

        tmCurrent.tm_year + 1900, tmCurrent.tm_mon, tmCurrent.tm_mday, 
        tmCurrent.tm_hour, tmCurrent.tm_min, tmCurrent.tm_sec,

        tDiffHour, tDiffMinute, tDiffSecond,

        chatLog.GetLogNum(),       
        chatLog.GetLogDataSize(), 
        chatLog.GetLogDataSize() / 1024,
        chatLog.GetLogDataSize() / 1024 / 1024);
}



class CMakeGameServerString
{
public:

    CMakeGameServerString(char* szBuffer, int nMaxLength, int& nUsed)
        :   m_szBuffer(szBuffer), m_nMaxLength(nMaxLength), m_nUsed(nUsed) { m_nUsed = 0; }

        bool operator () (unsigned long dwServerID, CPacketDispatch& dispatch)
        {
            int nLength = _snprintf(m_szBuffer + m_nUsed, m_nMaxLength - m_nUsed,
                "Game Server Connected (0x%08x).\r\n", dwServerID);

            if (0 < nLength)
            {
                m_nUsed += nLength;                
            }

            return true;
        }

private:

    char*   m_szBuffer;
    int     m_nMaxLength;
    int&    m_nUsed;
};


void CRylChatServer::PrintServerInfo()
{
	const int   MAX_BUFFER = 4096;
	char        szBuffer[MAX_BUFFER];

    szBuffer[0] = 0;

	GET_SINGLE_DISPATCH(lpChatAgentDispatch, CChatAgentServerDispatch, 
		CChatAgentServerDispatch::GetDispatchTable());
	
    int nUsedBytes = _snprintf(szBuffer, MAX_BUFFER, "Connected DBAgentServer %s\r\n",
		NULL != lpChatAgentDispatch ? "Succeed" : "Failed");

    if (0 < nUsedBytes)
    {
        int nLength = 0;

        CChatGameServerDispatch::GetDispatchTable().Process(
            CMakeGameServerString(szBuffer + nUsedBytes, MAX_BUFFER - nUsedBytes, nLength));

        if (0 <= nLength)
        {
            nUsedBytes += nLength;
			nUsedBytes += MakeChatLog(szBuffer + nUsedBytes, MAX_BUFFER - nUsedBytes);
            
			if (0 < nUsedBytes)
			{
                PrintInfo(szBuffer);
            }
        }
    }
}   


void CRylChatServer::ConnectToAgentServer()
{
	GET_SINGLE_DISPATCH(lpChatAgentDispatch, CChatAgentServerDispatch, 
		CChatAgentServerDispatch::GetDispatchTable());

    if (0 == lpChatAgentDispatch)
    {
		if (0 != m_lpAgentSessionPolicy)
		{
			m_lpAgentSessionPolicy->Release();
			m_lpAgentSessionPolicy = 0;		
		}

		m_lpAgentSessionPolicy = SessionPolicy::CreateTCPPolicy<CChatAgentServerDispatch>();

        INET_Addr& agentServerAddr = 
            CServerSetup::GetInstance().GetServerAddress(CServerSetup::AgentServer);

		if (!GetIOCPNet()->Connect(m_lpAgentSessionPolicy, 
			agentServerAddr.get_addr_string(), agentServerAddr.get_port_in()))
		{
            ERRLOG1(g_Log, "중계 서버(%16s)에 연결할 수 없습니다.", agentServerAddr.get_addr_string());
		}
    }
}

void CRylChatServer::ReloadSetup()
{
	if (true != CServerSetup::GetInstance().Initialize(CServerSetup::ChatServer))
    {
		ERRLOG0(g_Log, "ServerSetup reinitialize failed.");		
	}
	else if(true != CChatServerConfig::GetInstance().LoadSetup())
	{
		ERRLOG0(g_Log, "ChatServerConfig reinitialize failed.");		
	}
	else
	{        
		PrintOutput("ServerSetup, ChatServerConfig reload success.");
	}
}