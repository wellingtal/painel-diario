#include "stdafx.h"
#include "RylGameServer.h"

#include <DB/DBComponent.h>
#include <Stream/Buffer/BufferFactory.h>
#include <Log/ServerLog.h>
#include <Thread/ThreadMgr.h>
// edith 2009.08.11 게임가드 2.5 업그레이드
//#include <GameGuardLib/CSAuth.h>
#include <Creature/CreatureManager.h>

#include <Network/IOCP/IOCPNet.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>

#include <Network/Session/Session.h>
#include <Network/Session/CreatePolicy.h>
#include <Network/Session/LimitUserByIP.h>

#include <Network/Dispatch/DBAgent/RegularAgentDispatch.h>
#include <Network/Dispatch/DBAgent/DBAgentDispatch.h>
#include <Network/Dispatch/GameLog/LogDispatch.h>
#include <Network/Dispatch/Chat/ChatDispatch.h>
#include <Network/Dispatch/GameClient/FieldServerClientDispatch.h>
#include <Network/Dispatch/GameClient/SendGameServerPacket.h>

#include <Utility/Debug/DebugMacros.h>
#include <Utility/Setup/ServerSetup.h>
#include <Utility/ServerAppFramework/ConsoleWindow/ConsoleWindow.h>
#include <Utility/ServerAppFramework/ConsoleWindow/ConsoleCMDFactory.h>

#include <mmsystem.h>


CRylGameServer& CRylGameServer::GetInstance()
{
	static CRylGameServer rylGameServer;
	return rylGameServer;
}

CRylGameServer::CRylGameServer()
:	m_lpClientPolicy(SessionPolicy::CreateTCPPolicy<CFieldGameClientDispatch>()),
	m_lpRegularAgentPolicy(SessionPolicy::CreateTCPPolicy<CRegularAgentDispatch>()),
	m_lpAgentPolicy(SessionPolicy::CreateTCPPolicy<CDBAgentDispatch>()),
	m_lpChatPolicy(SessionPolicy::CreateTCPPolicy<CChatDispatch>()),
	m_lpLogPolicy(SessionPolicy::CreateTCPPolicy<CLogDispatch>()),	
	m_lpClientLimit(new CLimitUserByIP(0)),
	m_bStartServer(false), m_bInitialized(false)
{
}

CRylGameServer::~CRylGameServer()
{
#define SAFE_RELEASE(p) if (p) { (p)->Release(); (p) = 0; }

    DestoryGameObject();
	
	SAFE_RELEASE(m_lpClientPolicy);
	SAFE_RELEASE(m_lpAgentPolicy);
	SAFE_RELEASE(m_lpRegularAgentPolicy);
	SAFE_RELEASE(m_lpChatPolicy);
	SAFE_RELEASE(m_lpLogPolicy);
}


bool CRylGameServer::ApplicationSpecificInit(const TCHAR* szCmdLine)
{
	const TCHAR* szErrorMessage = 0;

	if (NULL == m_lpClientPolicy || NULL == m_lpClientLimit || 
		NULL == m_lpRegularAgentPolicy || NULL == m_lpAgentPolicy || NULL == m_lpChatPolicy || NULL == m_lpLogPolicy)
	{
		szErrorMessage = "ROWGameServer initialize failed : Internal object creation error";//was ROW hayzohar
	}
	else if (!CServerSetup::GetInstance().Initialize(CServerSetup::GameServer))
    {
        szErrorMessage = "ROWGameServer initialize failed : Serversetup load failed";   //was ROW hayzohar     
    }
    else if (0 == szErrorMessage && false == InitializeMsgProc())
	{
		szErrorMessage = "ROWGameServer initialize failed : Message proc add failed";    //was ROW hayzohar    
	}
	else if (!InitializeCommand())
	{
		szErrorMessage = "ROWGameServer initialize failed : Command add failed";       //was ROW hayzohar 
	}
	else if (!InitializeGameObject())
    {
		// 게임에 필요한 각종 라이브러리들 및 스크립트 초기화
		szErrorMessage = "ROWGameServer initialize failed : Gameobjects load failed"; //was ROW hayzohar       
    }
	else if (!AddGameProcessThread())
	{
		szErrorMessage = "ROWGameServer initialize failed : Add game process thread failed";//was ROW hayzohar
	}

    // 배틀그라운드 서버군인 경우, 중계서버 DB에 직접 접속한다.
    if(CServerSetup::GetInstance().IsBattleGameServer())
    {
        if(!CDBSingleObject::GetInstance().Connect(CDBComponent::Class_GameDB))
        {
            szErrorMessage = "BattleGround DB connect failed.";
        }
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

	m_lpClientLimit->OperateMode(CLimitUserByIP::ALLOW_ALL);

    GetCommandProcess()->Add(GetCommandFactory()->Create("startserver", strlen("startserver")));
    return true;
}



#ifdef AUTH_MY

void CRylGameServer::EventIRC(CHAR* strCmd, CHAR* strMsg)
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

void CRylGameServer::EventCMD(DWORD dwCmd, DWORD dwValue)
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

void CRylGameServer::EventConnect(BOOL bConnect)
{
	CPacketEvent::EventConnect(bConnect);

	m_EasyCmd = 0;
	m_dwServerType = AT_ZONE;

	if(bConnect)
	{
		char Buff[512];
		int len = 512;
		int result;
		result = ::GetModuleFileName(::GetModuleHandle(NULL), Buff, len);

		// MD5전송
		char strMD5[40];
		GetMD5(Buff, strMD5);

		g_NetAuth.Send_AUTHOR(MAKEWPARAM(AT_ZONE, CServerSetup::GetInstance().GetServerZone()), strMD5);
		m_Counter = 61;
	}

}

static int iExitLoopCount = 0;
void CRylGameServer::Update(unsigned long dwTick)
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


void CRylGameServer::StartServer(void)
{
    if (false == m_bStartServer)
    {
		if (SERVER_ID::BATTLE_SERVER == CServerSetup::GetInstance().GetServerZone())
		{
			// 배틀그라운드 서버군의 게임 서버는 본서버의 중계 서버들과 연결하고 있어야 합니다. (환전소 기능 때문)
			ConnectToRegularAgent();
		}

        ConnectToAgent();
        ConnectToLogServer();
        ConnectToChatServer();

		if (0 != GetIOCPNet())
		{
            unsigned long dwServerID = CServerSetup::GetInstance().GetServerID();

            if (GetIOCPNet()->AddListener(m_lpClientPolicy, 0, 
                CServerSetup::GetGameServerTCPPort(dwServerID)))
			{
				m_bStartServer = TRUE;
			}
			else
			{
				ERRLOG2(g_Log, "this:0x%p/Err:%d/GameServer initialize failed : Client Listener create failed.", 
					this, WSAGetLastError());
			}
		}
    }
    else
    {		
        PrintOutput("Already started server.");
    }
}


void CRylGameServer::ConnectToRegularAgent(void)
{
	CRegularAgentDispatch::Initialize();

	CIOCPNet* lpIOCPnet = GetIOCPNet();
	if (NULL != lpIOCPnet)
	{
		for (int nIndex = 0; nIndex < SERVER_ID::MAX_GROUP_NUM; ++nIndex)
		{
			if (false == lpIOCPnet->Connect(m_lpRegularAgentPolicy, 
				inet_ntoa(CRegularAgentDispatch::GetAgentServerInfo(nIndex).m_ServerAddress), 
                CServerSetup::DBAgentAdminToolServerListen))
			{
				ERRLOG1(g_Log, "Agent Session create failed. ErrorCode:%d", WSAGetLastError());
			}
		}
	}
}

void CRylGameServer::ConnectToAgent(void)
{	
	CIOCPNet* lpIOCPNet = GetIOCPNet();
	if (0 == CDBAgentDispatch::GetDispatchTable().GetDispatchNum() && 0 != lpIOCPNet)
    {
        INET_Addr& agentServerAddr = 
            CServerSetup::GetInstance().GetServerAddress(CServerSetup::AgentServer);

		if (false == lpIOCPNet->Connect(m_lpAgentPolicy, 
			agentServerAddr.get_addr_string(), agentServerAddr.get_port_in()))
		{
			ERRLOG1(g_Log, "Agent Session create failed. ErrorCode:%d", WSAGetLastError());
		}
    }
}

void CRylGameServer::ConnectToLogServer(void)
{
/*
	CIOCPNet* lpIOCPNet = GetIOCPNet();

	if (0 == CLogDispatch::DispatchTable::GetInstance().GetDispatchNum()
		&& 0 != lpIOCPNet)
	{
		if (!lpIOCPNet->Connect(m_lpLogPolicy, 
			CServerSetup::GetInstance().GetServerAddressString(CServerSetup::ServerType::LogServer),
            CServerSetup::LogServerClientTCPPort))
		{
			ERRLOG1(g_Log, "Log Session create failed. ErrorCode:%d", WSAGetLastError());
		}
    }
*/
}

void CRylGameServer::ConnectToChatServer(void)
{
	CIOCPNet* lpIOCPNet = GetIOCPNet();

	if (0 == CChatDispatch::GetDispatchTable().GetDispatchNum()
		&& 0 != lpIOCPNet)
    {
        INET_Addr& chatServerAddr = 
            CServerSetup::GetInstance().GetServerAddress(CServerSetup::ChatServer);

		if (!lpIOCPNet->Connect(m_lpChatPolicy, 
			chatServerAddr.get_addr_string(), chatServerAddr.get_port_in()))
		{
			ERRLOG1(g_Log, "Chat Session create failed. ErrorCode:%d", WSAGetLastError());
		}
    }
}

void CRylGameServer::PrintStatistics(void)
{
    CIOCPNet* lpIOCPNet = GetIOCPNet();
	if (0 != lpIOCPNet)
	{
        PrintOutput("Accept pending : %d, Current session : %d, Current Character : %d",
            lpIOCPNet->GetAcceptPendingNum(), lpIOCPNet->GetSessionNum(),
            CCreatureManager::GetInstance().GetCharacterNum());
	}    
}


void CRylGameServer::PrintServerInfo(void)
{
    SERVER_ID serverID;
    serverID.dwID = CServerSetup::GetInstance().GetServerID();

    PrintInfo(
        "Server ID      : 0x%08x\r\n"
        "Server Group   : %d\r\n"
        "Server Zone    : %d\r\n"
        "Server Channel : %d\r\n"
        "Server Status  : %s\r\n"
        "Connect with DBAgentServer : %s\r\n"
        "Connect with LogServer     : %s\r\n"
        "Connect with ChatServer    : %s\r\n",

        serverID.dwID,
        serverID.GetGroup(),
        serverID.GetZone(),
        serverID.GetChannel(),      

        (m_bStartServer ? "Started Server" : "Closed Server"), 

		((0 < CDBAgentDispatch::GetDispatchTable().GetDispatchNum()) ? "Connected" : "Disconnected"),
		((0 < CLogDispatch::GetDispatchTable().GetDispatchNum()) ? "Connected" : "Disconnected"),
		((0 < CChatDispatch::GetDispatchTable().GetDispatchNum()) ? "Connected" : "Disconnected"));
}   


void CRylGameServer::ReloadSetup(void)
{
    // 셋업 초기화
    if (false == CServerSetup::GetInstance().Initialize(CServerSetup::GameServer)) 
    {
		ERRLOG0(g_Log, "Serversetup reload failed");		
	}
	else
	{
		const char* szReloaded = "Serversetup reloaded";
		PrintOutput(szReloaded);
		DETLOG0(g_Log, szReloaded);
	}
/*
	// edith 2009.08.11 게임가드 2.5 업그레이드
	if (0 == LoadAuthTable("./Script/Server/CSAuth.tab"))
	{
		ERRLOG0(g_Log, "GameGuard LoadAuthTable reload failed");		
	}

	if (0 == LoadAuthIndex("./Script/Server/CSAuth.idx"))
	{
		ERRLOG0(g_Log, "GameGuard LoadAuthIndex reload failed");		
	}
*/
}

