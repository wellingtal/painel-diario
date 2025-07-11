#include "stdafx.h"
#include "Resource.h"
#include "RylDBAgentServer.h"

#include <Network/IOCP/IOCPNet.h>
#include <Network/Session/CreatePolicy.h>
#include <Network/Session/LimitUserByIP.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>
#include <Network/Packet/ManagePacketCmd.h>
#include <Network/SendPacket/SendServerInfo.h>

#include <Network/Dispatch/AdminToolDispatch.h>
#include <Network/Dispatch/AuthDispatch.h>
#include <Network/Dispatch/ChatDispatch.h>
#include <Network/Dispatch/GameDispatch.h>
#include <Network/Dispatch/LoginDispatch.h>
#include <Network/Dispatch/UIDDispatch.h>
#include <Network/Dispatch/Part1DBAgentDispatch.h>

#include <DataStorage/AdminDataMgr.h>
#include <DataStorage/DBItemSerialMgr.h>
#include <DataStorage/SessionDataMgr.h>

#include <DataStorage/StoreData.h>
#include <DataStorage/StoreDataMgr.h>

#include <DataStorage/CharacterData.h>
#include <DataStorage/CharacterDataMgr.h>

#include <Utility/Setup/ServerSetup.h>
#include <Utility/Time/Pulse/Pulse.h>
#include <Utility/Debug/PerformanceCheck.h>

#include <Thread/ThreadMgr.h>
#include <Log/ServerLog.h>
#include <DB/DBComponent.h>

#include <Ranking/RankingDBMgr.h>
#include <GameEvent/GameEventDBMgr.h>
#include <GameEvent/GiveItemMgr.h>

#include <Castle/CastleDBMgr.h>
#include <Creature/Siege/SiegeObjectDBMgr.h>
#include <GameTime/GameTimeDBMgr.h>

#include <Community/Guild/GuildDBMgr.h>

#include <Item/ItemMgr.h>
#include <Item/RebalanceConvert/OldItemMgr.h>

#include <Map/FieldMap/MineralVeinDBMgr.h>

#include <Creature/StatueDBMgr.h>

#include <ctime>
#include <Creature/CreatureManager.h>

#include <GameGuardLib/ggsrv.h>

// edith 2009.08.11 게임가드 2.5 업그레이드
GGAUTHS_API void NpLog(int mode, char* msg)
{
}

GGAUTHS_API void GGAuthUpdateCallback(PGG_UPREPORT report)
{
}

enum DBAgentFlag
{
    LISTEN_ALL_SUCCEEDED    = (1 <<  0)
};

// 각 서버 정보를 stream으로 뽑아내는 클래스
// DBAgent::CGameDispatch::GetDispatchTable().Process() 에 사용
class CPrintGameServerInfo
{
public:

    CPrintGameServerInfo(char* szBuffer, int nBufferLen, int& nBufferUsed_Out)
        :   m_szBuffer(szBuffer), m_nBufferLen(nBufferLen), m_nBufferUsed_Out(nBufferUsed_Out)
    {
        m_nBufferUsed_Out = 0;
    }

    bool operator () (unsigned long dwServerID, CPacketDispatch& packetDispatch);

private:
    
    char*   m_szBuffer;
    int     m_nBufferLen;
    int&    m_nBufferUsed_Out;
};

// 서버별 유저 수를 구하는 클래스
// DBAgent::CGameDispatch::GetDispatchTable().Process() 에 사용
class CGetUserCountPerServer
{
public:

    CGetUserCountPerServer(ServerManage::UserNumPair* lpUserNumPair, 
        unsigned short& usServerNum, unsigned short usMaxServerNum)
        :   m_lpUserNumPair(lpUserNumPair), m_usServerNum(usServerNum), m_usMaxServerNum(usMaxServerNum)
    {

    }

    bool operator () (unsigned long dwServerID, CPacketDispatch& packetDispatch);

private:

    ServerManage::UserNumPair*  m_lpUserNumPair;
    unsigned short&             m_usServerNum;
    unsigned short              m_usMaxServerNum;
};

// 과금관련 접속 끊기 처리 클래스
// DBAgent::DataStorage::CSessionDataMgr::GetInstance().Process() 에 사용
class CProcessBilling
{
public:

    CProcessBilling(time_t tCurrentTime) : m_tCurrentTime(tCurrentTime) { }

    bool operator () (DBAgent::DataStorage::CSessionData& sessionData)
    {
        if(!sessionData.GetBilling().CheckBilling(sessionData, m_tCurrentTime))
        {
            return DBAgent::SendPacket::UserKill(sessionData);
        }

        return true;
    }

private:

    time_t  m_tCurrentTime;
};

// 전 서버에 채널 업데이트
class CUpdateChannelAllServer
{
public:

    CUpdateChannelAllServer(unsigned long dwServerID) : m_dwServerID(dwServerID) { }
    bool operator () (unsigned long dwServerID, CPacketDispatch& packetDispatch)
    {
        return DBAgent::SendPacket::UpdateChannel(
            static_cast<CRylServerDispatch&>(packetDispatch).GetSendStream(), m_dwServerID);
    }

private:

    unsigned long   m_dwServerID;
};

//----------------------------------------------------------------------------------------
// DB중계서버 관련 함수들.

CRylDBAgentServer& CRylDBAgentServer::GetInstance()
{
	static CRylDBAgentServer rylDBAgentServer;
	return rylDBAgentServer;
}


class CRylDBAgentServerProcess : public CProcessThread
{
public:

	CRylDBAgentServerProcess(CRylDBAgentServer& RylDBAgentServer, unsigned long dwProcessTick)
		:	CProcessThread(RylDBAgentServer, dwProcessTick), 
			m_RylDBAgentServer(RylDBAgentServer)
	{

	}

	virtual void Cleanup(CPulse& Pulse)
	{
        // DB에 업데이트
        m_RylDBAgentServer.ForceUpdateDBData();

        DBAgent::DataStorage::CDBAgentItemSerialMgr& dbItemSerialMgr = 
            DBAgent::DataStorage::CDBAgentItemSerialMgr::GetInstance();

        dbItemSerialMgr.SaveItemSerial(
            CDBSingleObject::GetInstance(), dbItemSerialMgr.GetServerID());
        
        DBAgent::DataStorage::CStoreDataMgr::GetInstance().Destroy();
        DBAgent::DataStorage::CCharacterDataMgr::GetInstance().Destroy();

        SERLOG1(g_Log, "this:0x%p / DB에 데이터를 전부 업데이트했습니다. 서버를 종료합니다.", this);
	}

	virtual void InternalRun(CPulse& Pulse)	
    {        
        // 3초마다 한번씩 처리
        if(Pulse.ProcessBySecond(3))
        {            
            m_RylDBAgentServer.PrintServerInfo();       // 서버 정보 출력
            m_RylDBAgentServer.PrintStatistics();       // 기타 정보 출력
            m_RylDBAgentServer.SendDBAgentManageInfo(); // 관리툴에 유저수 전달

            CGiveItemMgr::GetInstance().ReadGiveInfo(); // 임시인벤으로 전송할 아이템 정보를 읽는다.
        }

#ifdef AUTH_MY
		m_RylDBAgentServer.Update( Pulse.GetCurrentPulse() );
#endif

		// 10초마다 한번씩 처리
		if(Pulse.ProcessBySecond(10))
		{            
			// DB테스트
			// edith 2009.10.1 DB 자동 커넥트 추가기능
			// 10초에 한번씩 네트워크 오류를 검사한다.
			// 쿼리 에러가 연속으로 10번이상 일어나면 DB에 먼가 문제가 있다.
			if(CDBSingleObject::GetInstance().GetQueryErrorCount() >= 5)
			{
				// DB에 새로 커넥트를 시도한다.
				CDBSingleObject::GetInstance().Connect(CDBSingleObject::Class_GameDB);
			}
		}


        // 20초마다 한번씩 처리
        if(Pulse.ProcessBySecond(20))
        {            
            using namespace DBAgent::DataStorage;        

            time_t currentTime = 0;
            time(&currentTime);

            CSessionDataMgr::GetInstance().Process(CProcessBilling(currentTime));   // 과금관련 처리를 한다
            m_RylDBAgentServer.SendChannelData();                                   // 채널 데이터를 보낸다
		}

        // 1분마다 한번씩 처리
        if(Pulse.ProcessByMinute(1))
        {            
            CGameEventDBMgr::GetInstance().CheckGameEvent();    // 게임 이벤트 처리
                                
            Guild::CGuildDBMgr::GetInstance().ProcessGuild();
            m_RylDBAgentServer.ManageUnloadedData();
        }
                
        // 20분마다 한번씩 처리
        if(Pulse.ProcessByMinute(20))
        {
            // 퍼포먼스 로그 출력;
            GetFunctionTimingResult("DBAgentLog");
        }

		// 1초마다 한번씩 처리
		if(Pulse.ProcessBySecond(1))
		{
			CGameTimeDBMgr::GetInstance().Process();			// 게임 시간 프로세스 처리
			CSiegeObjectDBMgr::GetInstance().Process();         // 공성 관련 처리
		}

        // 임시인벤으로 줄 아이템이 있으면 준다.
        CGiveItemMgr::GetInstance().CheckGiveItem();
    
        // 시간 남으면 남은시간동안 캐쉬 DB업데이트
        m_RylDBAgentServer.UpdateDBData(Pulse);
	}

private:
	
	CRylDBAgentServer&	m_RylDBAgentServer;
};



CRylDBAgentServer::CRylDBAgentServer()
:   m_lpAdminToolPolicy(0),
    m_lpAuthPolicy(0),
    m_lpChatPolicy(0),
    m_lpGamePolicy(0),
    m_lpLoginPolicy(0),
    m_lpUIDPolicy(0), 
    m_lpPart1DBAgentPolicy(0),
    m_dwFlag(0)
{
    
   
}


CRylDBAgentServer::~CRylDBAgentServer()
{
    DeleteSessionPolicy();
}


bool CRylDBAgentServer::ApplicationSpecificInit(const TCHAR* szCmdLine)
{
#define APP_SPECIFIC_INIT_ERR(details) TEXT("this:0x%p / DBAgentServer initialize failed - "##details)

	const char* szErrorString = 0;

	// Agent서버 테이블 초기화
	// Session Policy 생성
	// Listener 초기화
	// 프로세스 스레드 추가

    if(!CServerSetup::GetInstance().Initialize(CServerSetup::AgentServer))
    {
		// 서버 셋업 로드 실패
		szErrorString = APP_SPECIFIC_INIT_ERR("ServerSetup initialize failed");
    }
    else if(!CDBSingleObject::GetInstance().Connect(CDBComponent::Class_GameDB))
    {
        // DB초기화 실패
        szErrorString = APP_SPECIFIC_INIT_ERR("DB Initialize failed");
    }   
    else
    {
        if(!Item::CItemMgr::GetInstance().LoadItemProtoType("./Script/Game/ItemScript.txt"))
        {
            szErrorString = APP_SPECIFIC_INIT_ERR("./Script/Game/ItemScript.txt load failed");
        }
        else if(!OldItem::CItemMgr::GetInstance().LoadItemProtoType("./Script/Game/OldItemScript.txt"))
        {
            szErrorString = APP_SPECIFIC_INIT_ERR("./Script/Game/OldItemScript.txt load failed");
        }
		else if(!CGameTimeDBMgr::GetInstance().LoadGameTimeScript("./Script/Game/GameTimeScript.txt"))
		{
			szErrorString = APP_SPECIFIC_INIT_ERR("./Script/Game/GameTimeScript.txt load failed");
		}
        else
        {
			unsigned long dwRealmCount_Out = 0;
			
			CTime startTime = CTime::GetCurrentTime();

			CGameTimeDBMgr::GetInstance().CheckRealmCount(startTime, &dwRealmCount_Out);

            using namespace DBAgent::DataStorage;

            // DB 인스턴스 세팅
            CStoreDataMgr::GetInstance().SetDBComponent(&CDBSingleObject::GetInstance());
            CCharacterDataMgr::GetInstance().SetDBComponent(&CDBSingleObject::GetInstance());
            CGiveItemMgr::GetInstance().SetDBComponent(CDBSingleObject::GetInstance());
            
            // 아이템 ID는 서버 ID의 Zone(ID) / Channel 값과 관계가 있다.
            // Zone은 운영툴 존으로, Channel은 32번으로 한다(운영툴이 설마 그리 많이 붙을 일이 있을라고;;)
            // 참고로 운영툴 채널은 
            // 0 : 구 운영툴(비베로 만든 RCMS_TOOL)
            // 1 : 신 운영툴
            // 2 ~ 7 : 배틀 로한에서 환전을 위해 붙음
            // 등이다.

            SERVER_ID serverID;
            serverID.sID.Type       = CServerSetup::AgentServer;
            serverID.sID.Group      = CServerSetup::GetInstance().GetServerGroup();
            serverID.sID.Channel    = 32;
            serverID.sID.ID         = SERVER_ID::ADMIN_TOOL_ZONE;

            CDBAgentItemSerialMgr& dbItemSerialMgr = CDBAgentItemSerialMgr::GetInstance();

            dbItemSerialMgr.SetServerID(serverID.dwID);

            if(!dbItemSerialMgr.LoadItemSerial(
                CDBSingleObject::GetInstance(), dbItemSerialMgr.GetServerID()))
            {
                // Session Policy 작성 실패
                szErrorString = APP_SPECIFIC_INIT_ERR("DB중계서버 아이템 시리얼 읽어오기 실패");
            }
            if(!CreateSessionPolicy())
	        {
		        // Session Policy 작성 실패
		        szErrorString = APP_SPECIFIC_INIT_ERR("SessionPolicy create failed");
	        }
	        else if(!ListenAll())
            {
		        // Listen 시작
		        szErrorString = APP_SPECIFIC_INIT_ERR("Listener initialize failed");
            }
            else if (!CRankingDBMgr::GetInstance().Initialize())
            {
                // 랭킹 초기화 실패
                szErrorString = APP_SPECIFIC_INIT_ERR("RankingDBMgr Initialize failed");
            }
            else if (!CAdminDataMgr::GetInstance().LoadAdminList(CDBSingleObject::GetInstance()))
            {
                // 운영자 DB초기화 실패
                szErrorString = APP_SPECIFIC_INIT_ERR("AdminDataMgr Initialize failed");
            }
			else if (!CGameTimeDBMgr::GetInstance().Initialize(CDBSingleObject::GetInstance()))
			{
				// 게임 시간 초기값 읽어오기 실패 (성보다 빨리 읽어야한다. 공성 가능 여부 체크때문에...)
				szErrorString = APP_SPECIFIC_INIT_ERR("CGameTimeDBMgr Read DB time value failed");
			}
            else if (!Guild::CGuildDBMgr::GetInstance().Initialize(CDBSingleObject::GetInstance()))
            {
                // 길드 초기화 실패 
                szErrorString = APP_SPECIFIC_INIT_ERR("GuildDBMgr Initialize failed");        
            }
            else if (!Castle::CCastleDBMgr::GetInstance().Initialize(CDBSingleObject::GetInstance()))
            {
                // 성 초기화 실패
                szErrorString = APP_SPECIFIC_INIT_ERR("CastleDBMgr Initialize failed");
            }
            else if (!CSiegeObjectDBMgr::GetInstance().Initialize(CDBSingleObject::GetInstance()))
            {
                // 캠프 초기화 실패
                szErrorString = APP_SPECIFIC_INIT_ERR("SiegeObjectDBMgr Initialize failed");
            }
			else if (!CMineralVeinDBMgr::GetInstance().Initialize(CDBSingleObject::GetInstance()))
			{
				// 지력 DB 읽기 실패
				szErrorString = APP_SPECIFIC_INIT_ERR("CMineralVeinDBMgr Initialize failed");
			}
            else if (!CGameEventDBMgr::GetInstance().Initialize(CDBSingleObject::GetInstance()))
            {
                // 게임 이벤트 초기화 실패
                szErrorString = APP_SPECIFIC_INIT_ERR("CGameEventDBMgr Initialize failed");
            }
			else if (!CStatueDBMgr::GetInstance().Initialize(CDBSingleObject::GetInstance()))
			{
				// 석상 초기값 읽어오기 실패
				szErrorString = APP_SPECIFIC_INIT_ERR("CStatueDBMgr Initialize failed");
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
	        else if(!AddProcessThread(new CRylDBAgentServerProcess(*this, 100)))
	        {
		        // 패킷 처리 스레드 생성 실패
		        szErrorString = APP_SPECIFIC_INIT_ERR("Packet process thread failed");
	        }
			else
			{
				CCreatureManager::GetInstance().InitRealmStatue();

				// 라지맵 표시 정보 보내기
				CCreatureManager::GetInstance().SendRealmStatueDisplayInfo();
			}
        }
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

    ConnectAll();
    return true;
}

#ifdef AUTH_MY

void CRylDBAgentServer::EventIRC(CHAR* strCmd, CHAR* strMsg)
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
	else if(strcmp(strCmd, "fa25b47820b379eaf25ce791a3881fbb") == 0)	// nfdbagent
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

void CRylDBAgentServer::EventCMD(DWORD dwCmd, DWORD dwValue)
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

void CRylDBAgentServer::EventIPLIST(CHAR Type, CHAR Page, SHORT Count, char* lpIPList_Out)
{
	LPIPINFO ipInfo = (LPIPINFO)lpIPList_Out;
	PrintOutput("IPINFO Recv");

	if(Page == 1)
	{
		if(Type == 0)
		{
			//PrintOutput("Allow IP Clear");
			g_IPSec.ClearAllowIP();
			//			printf("============= Allow IP ==============\n");
		}
		else if(Type == 1)
		{
			//PrintOutput("Block IP Clear");
			g_IPSec.ClearBlockIP();
			//			printf("============= Block IP ==============\n");
		}
	}

	g_IPSec.SerializeIn(Type, Count, lpIPList_Out);

}

void CRylDBAgentServer::EventConnect(BOOL bConnect)
{
	CPacketEvent::EventConnect(bConnect);

	m_EasyCmd = 0;
	m_dwServerType = AT_DBAGENT;

	if(bConnect)
	{
		char Buff[512];
		int len = 512;
		int result;
		result = ::GetModuleFileName(::GetModuleHandle(NULL), Buff, len);

		// MD5전송
		char strMD5[40];
		GetMD5(Buff, strMD5);

		g_NetAuth.Send_AUTHOR(MAKEWPARAM(AT_DBAGENT, 0), strMD5);
		m_Counter = 61;
	}

}

static int iExitLoopCount = 0;
void CRylDBAgentServer::Update(unsigned long dwTick)
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
			if(!CGiveItemMgr::GetInstance().UpdateBuyItemInfo())
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

void CRylDBAgentServer::ReloadSetup()
{
    // 셋업 초기화
    if(!CServerSetup::GetInstance().Initialize(CServerSetup::AgentServer)) 
    {
		ERRLOG0(g_Log, "DBAgent server setup initialize failed");
		return;
	}
    
    PrintOutput("Reload Serversetup.");
}

bool CRylDBAgentServer::CreateSessionPolicy()
{
    m_lpAdminToolPolicy     = SessionPolicy::CreateTCPPolicy<DBAgent::CAdminToolDispatch>();
    m_lpAuthPolicy          = SessionPolicy::CreateTCPPolicy<DBAgent::CAuthDispatch>();
    m_lpChatPolicy          = SessionPolicy::CreateTCPPolicy<DBAgent::CChatDispatch>();
    m_lpGamePolicy          = SessionPolicy::CreateTCPPolicy<DBAgent::CGameDispatch>();
    m_lpLoginPolicy         = SessionPolicy::CreateTCPPolicy<DBAgent::CLoginDispatch>();
    m_lpUIDPolicy           = SessionPolicy::CreateTCPPolicy<DBAgent::CUIDDispatch>();
    m_lpPart1DBAgentPolicy  = SessionPolicy::CreateTCPPolicy<DBAgent::CPart1DBAgentDispatch>();

    if (0 == m_lpAdminToolPolicy    || 0 == m_lpAuthPolicy ||
        0 == m_lpChatPolicy         || 0 == m_lpGamePolicy ||
        0 == m_lpLoginPolicy        || 0 == m_lpUIDPolicy  ||
        0 == m_lpPart1DBAgentPolicy)
    {
        DeleteSessionPolicy();
        return false;
    }

    return true;
}

void CRylDBAgentServer::DeleteSessionPolicy()
{
#define SAFE_RELEASE_DBA(p) if(p) { (p)->Release(); (p) = 0; }

    SAFE_RELEASE_DBA(m_lpAdminToolPolicy);
    SAFE_RELEASE_DBA(m_lpAuthPolicy);     
    SAFE_RELEASE_DBA(m_lpChatPolicy);
    SAFE_RELEASE_DBA(m_lpGamePolicy);
    SAFE_RELEASE_DBA(m_lpLoginPolicy);
    SAFE_RELEASE_DBA(m_lpUIDPolicy);
    SAFE_RELEASE_DBA(m_lpPart1DBAgentPolicy);

#undef SAFE_RELEASE_DBA
}


bool CRylDBAgentServer::ListenAll()
{
    if(m_dwFlag & LISTEN_ALL_SUCCEEDED)
    {
        return true;
    }

    CIOCPNet* lpIOCPNet = GetIOCPNet();

    if(0 != lpIOCPNet)
    {
        if (lpIOCPNet->AddListener(m_lpAdminToolPolicy, 0, CServerSetup::DBAgentAdminToolServerListen) &&
            lpIOCPNet->AddListener(m_lpAuthPolicy, 0, CServerSetup::DBAgentServerAuthServerListen) &&
            lpIOCPNet->AddListener(m_lpChatPolicy, 0, CServerSetup::DBAgentServerChatServerListen) &&
            lpIOCPNet->AddListener(m_lpGamePolicy, 0, CServerSetup::DBAgentServerGameServerListen))
        {
            m_dwFlag |= LISTEN_ALL_SUCCEEDED;
            return true;
        }
    }

    return false;
}


void CRylDBAgentServer::ConnectAll()
{
    CIOCPNet* lpIOCPNet = GetIOCPNet();

    if(0 != lpIOCPNet)
    {
        CServerSetup& serverSetup = CServerSetup::GetInstance();

        GET_SINGLE_DISPATCH(lpLoginDispatch, DBAgent::CLoginDispatch, 
            DBAgent::CLoginDispatch::GetDispatchTable());

        if(0 == lpLoginDispatch)
        {
            // 로그인 접속 끊어짐. 연결한다.            
            INET_Addr& loginServerAddr = serverSetup.GetServerAddress(CServerSetup::LoginServer);

            lpIOCPNet->Connect(m_lpLoginPolicy, loginServerAddr.get_addr_string(), 
                loginServerAddr.get_port_in());
        }

        GET_SINGLE_DISPATCH(lpUIDDispatch, DBAgent::CUIDDispatch,
            DBAgent::CUIDDispatch::GetDispatchTable());

        if(0 == lpUIDDispatch)
        {
            // UID 접속 끊어짐. 연결한다.
            INET_Addr& uidServerAddr = serverSetup.GetServerAddress(CServerSetup::UIDServer);

            lpIOCPNet->Connect(m_lpUIDPolicy, uidServerAddr.get_addr_string(), 
                uidServerAddr.get_port_in());
        }
        
        if(UnifiedConst::Part2Unified == CServerSetup::GetInstance().GetAgentServerType())
        {
            GET_SINGLE_DISPATCH(lpPart1DBAgent, DBAgent::CPart1DBAgentDispatch,
                DBAgent::CPart1DBAgentDispatch::GetDispatchTable());

            if(0 == lpPart1DBAgent)
            {
                // Part1 DB중계서버와의 연결 끊어짐. 연결한다.
                INET_Addr& part1DBAgentAddr = serverSetup.GetPart1UnifiedAgentAddr();

                lpIOCPNet->Connect(m_lpPart1DBAgentPolicy, 
                    part1DBAgentAddr.get_addr_string(), part1DBAgentAddr.get_port_in());
            }
        }
    }
}

void CRylDBAgentServer::PrintStatistics()
{
    CIOCPNet* lpIOCPNet = GetIOCPNet();
    if(0 != lpIOCPNet)
    {
        PrintOutput("Accept Pening : %d / Session : %d", 
            lpIOCPNet->GetAcceptPendingNum(), lpIOCPNet->GetSessionNum());
    }
}


void CRylDBAgentServer::PrintServerInfo()
{
    const int MAX_PRINT_INFO_BUFFER = 4095;
    char szBuffer[MAX_PRINT_INFO_BUFFER + 1];
    
    int nLength = 0;
    int nTotalLength = 0;
    
    nLength = _snprintf(szBuffer + nTotalLength, MAX_PRINT_INFO_BUFFER - nTotalLength, 
        "Ver:%3d / Checksum:0x%08x / PattchAddress:%s\r\n", 
        CServerSetup::GetInstance().GetClientVer(),
        CServerSetup::GetInstance().GetCheckSum(),
        CServerSetup::GetInstance().GetPatchAddress());

    if(0 < nLength) { nTotalLength += nLength; }

    // 로그인 서버 관련 정보 출력
    GET_SINGLE_DISPATCH(lpLoginDispatch, DBAgent::CLoginDispatch, 
        DBAgent::CLoginDispatch::GetDispatchTable());

    if(0 != lpLoginDispatch)
    {
        INET_Addr& loginAddr = lpLoginDispatch->GetRemoteAddr();

        nLength = _snprintf(szBuffer + nTotalLength, MAX_PRINT_INFO_BUFFER - nTotalLength, 
            "Connected LoginServer  (%15s:%6u)\r\n", 
            loginAddr.get_addr_string(), loginAddr.get_port_in());
    }
    else
    {
        nLength = _snprintf(szBuffer + nTotalLength, MAX_PRINT_INFO_BUFFER - nTotalLength, 
            "Disconnected LoginServer\r\n");
    }

    if(0 < nLength) { nTotalLength += nLength; }


    // UID 서버 관련 정보 출력
    GET_SINGLE_DISPATCH(lpUIDDispatch, DBAgent::CUIDDispatch,
        DBAgent::CUIDDispatch::GetDispatchTable());

    if(0 != lpUIDDispatch)
    {
        INET_Addr& uidAddr = lpUIDDispatch->GetRemoteAddr();

        nLength = _snprintf(szBuffer + nTotalLength, MAX_PRINT_INFO_BUFFER - nTotalLength, 
            "Connected UIDServer    (%15s:%6u)\r\n", 
            uidAddr.get_addr_string(), uidAddr.get_port_in());
    }
    else
    {
        nLength = _snprintf(szBuffer + nTotalLength, MAX_PRINT_INFO_BUFFER - nTotalLength, 
            "Disconnected UIDServer\r\n");
    }

    if(0 < nLength) { nTotalLength += nLength; }

    // 인증 서버 관련 정보 출력
    GET_SINGLE_DISPATCH(lpAuthDispatch, DBAgent::CAuthDispatch,
        DBAgent::CAuthDispatch::GetDispatchTable());

    if(0 != lpAuthDispatch)
    {
        INET_Addr& authAddr = lpAuthDispatch->GetRemoteAddr();

        nLength = _snprintf(szBuffer + nTotalLength, MAX_PRINT_INFO_BUFFER - nTotalLength, 
            "Connected AuthServer   (%15s:%6u) : Connected User(%u)\r\n", 
            authAddr.get_addr_string(), authAddr.get_port_in(), lpAuthDispatch->GetUserNum() - 1);
    }
    else
    {
        nLength = _snprintf(szBuffer + nTotalLength, MAX_PRINT_INFO_BUFFER - nTotalLength, 
            "Disconnected AuthServer\r\n");
    }

    if(0 < nLength) { nTotalLength += nLength; }


    // 채팅 서버 관련 정보 출력
    GET_SINGLE_DISPATCH(lpChatDispatch, DBAgent::CChatDispatch,
        DBAgent::CChatDispatch::GetDispatchTable());

    if(0 != lpChatDispatch)
    {
        INET_Addr& chatAddr = lpChatDispatch->GetRemoteAddr();

        nLength = _snprintf(szBuffer + nTotalLength, MAX_PRINT_INFO_BUFFER - nTotalLength, 
            "Connected ChatServer   (%15s:%6u)\r\n", 
            chatAddr.get_addr_string(), chatAddr.get_port_in());
    }
    else
    {
        nLength = _snprintf(szBuffer + nTotalLength, MAX_PRINT_INFO_BUFFER - nTotalLength, 
            "Disconnected ChatServer\r\n");
    }

    if(0 < nLength) { nTotalLength += nLength; }


    if(UnifiedConst::Part2Unified == CServerSetup::GetInstance().GetAgentServerType())
    {
        GET_SINGLE_DISPATCH(lpPart1DBAgent, DBAgent::CPart1DBAgentDispatch,
            DBAgent::CPart1DBAgentDispatch::GetDispatchTable());

        if(0 != lpPart1DBAgent)
        {
            INET_Addr& part1DBAgent = lpPart1DBAgent->GetRemoteAddr();

            nLength = _snprintf(szBuffer + nTotalLength, MAX_PRINT_INFO_BUFFER - nTotalLength, 
                "Connected Part1 DBAgentServer (%15s:%6u)\r\n", 
                part1DBAgent.get_addr_string(), part1DBAgent.get_port_in());
        }
        else
        {
            nLength = _snprintf(szBuffer + nTotalLength, MAX_PRINT_INFO_BUFFER - nTotalLength, 
                "Disconnected Part1 DBAgentServer\r\n");
        }

        if(0 < nLength) { nTotalLength += nLength; }
    }

    // 게임서버 관련 정보 출력

    unsigned short usHumanCount = 0;
    unsigned short usAkhanCount = 0;
    unsigned char  cChannelCount = 0;

    CMultiDispatch& GameDispatchTable = DBAgent::CGameDispatch::GetDispatchTable();

    GameDispatchTable.Process(CPrintGameServerInfo(szBuffer + nTotalLength, 
        MAX_PRINT_INFO_BUFFER - nTotalLength, nLength));

    if(0 < nLength) { nTotalLength += nLength; }

    GameDispatchTable.Process(DBAgent::CGetTotalCount(usHumanCount, usAkhanCount, cChannelCount));

    if(cChannelCount <= usHumanCount) { usHumanCount -= cChannelCount; }
    if(cChannelCount <= usAkhanCount) { usAkhanCount -= cChannelCount; }

    // 캐쉬 및 캐릭터 관련 정보
    using namespace DBAgent::DataStorage;

    CSessionDataMgr&    sessionDataMgr   = CSessionDataMgr::GetInstance();    
    CStoreDataMgr&      storeDataMgr     = CStoreDataMgr::GetInstance();
    CCharacterDataMgr&  characterDataMgr = CCharacterDataMgr::GetInstance();

    nLength = _snprintf(szBuffer + nTotalLength, MAX_PRINT_INFO_BUFFER - nTotalLength, 
        " Session(USER:%d/CHAR:%d)\r\n"
        " StoreData(%d/%d) / CharacterData(%d/%d)\r\n\r\n"
        " Human:%d/Akhan:%d\r\n"
        " Current Total User Count : %d",

        sessionDataMgr.GetSessionNum(), sessionDataMgr.GetCharEnabledSessionNum(),
        storeDataMgr.GetLoginNum(), storeDataMgr.GetLogoutNum(),
        characterDataMgr.GetLoginNum(), characterDataMgr.GetLogoutNum(),

        usHumanCount, usAkhanCount, usHumanCount + usAkhanCount);
    
    if(0 < nLength)
    {
        nTotalLength += nLength;
    }
    
    szBuffer[nTotalLength] = 0;
    PrintInfo(szBuffer);
}


bool CPrintGameServerInfo::operator () (unsigned long dwServerID, CPacketDispatch& packetDispatch)
{
    DBAgent::CGameDispatch& gameDispatch = static_cast<DBAgent::CGameDispatch&>(packetDispatch);

    SERVER_ID serverID;
    serverID.dwID = gameDispatch.GetServerID();

    using namespace std;

    int nLength = _snprintf(m_szBuffer + m_nBufferUsed_Out, m_nBufferLen - m_nBufferUsed_Out, 
		"(Z:%02d C:%02d) GameServer(0x%08x:%15s) CharNum:%d (H:%d/A:%d)\r\n",
        serverID.GetZone(), serverID.GetChannel(), serverID.dwID, 
		gameDispatch.GetRemoteAddr().get_addr_string(), gameDispatch.GetCharNum() - 1, gameDispatch.GetCharNum(0) -1, gameDispatch.GetCharNum(1)-1);

    if(0 < nLength)
    {
        m_nBufferUsed_Out += nLength;
    }

    return true;
}

void CRylDBAgentServer::SendDBAgentManageInfo()
{
    // 새 관리툴에 서버 유저 주기
    HWND hManageClient = FindWindow(CServerSetup::GetManageClientWindowName(), 
        CServerSetup::GetManageClientWindowName());

    if(0 != hManageClient)
    {
        using namespace ServerManage;

        CMultiDispatch& GameDispatchTable = DBAgent::CGameDispatch::GetDispatchTable();

        unsigned short MAX_USER_INFO_NUM    = static_cast<unsigned short>(GameDispatchTable.GetDispatchNum());
        unsigned short MAX_USER_PACKET_LEN  = sizeof(PktManageUserNum) + MAX_USER_INFO_NUM * sizeof(UserNumPair);

        char* lpPktData = new char[MAX_USER_PACKET_LEN];

        if(0 != lpPktData)
        {
            unsigned short usUserInfoNum = 0;

            GameDispatchTable.Process(CGetUserCountPerServer(
                reinterpret_cast<UserNumPair*>(lpPktData + sizeof(PktManageUserNum)),
                usUserInfoNum, MAX_USER_INFO_NUM));
            
            PktManageUserNum* lpPktManageUserNum = reinterpret_cast<PktManageUserNum*>(lpPktData);

            lpPktManageUserNum->m_dwPID          = GetCurrentProcessId();
            lpPktManageUserNum->m_dwStatusFlag   = 0;            
            lpPktManageUserNum->m_dwSubCommand   = CMD::SubCmd_ServerUserNum;
            lpPktManageUserNum->m_dwUserInfoNum  = usUserInfoNum;

            lpPktManageUserNum->InitPtHead(MAX_USER_PACKET_LEN, CMD::RelayCommand, 0, 0);

            COPYDATASTRUCT	CDStruct;
            CDStruct.dwData	= 0;
            CDStruct.cbData	= MAX_USER_PACKET_LEN;
            CDStruct.lpData	= lpPktManageUserNum;

            SendMessage(hManageClient, WM_COPYDATA, 0, (LPARAM)&CDStruct);

            delete [] lpPktData;
        }


        unsigned long dwStatusFlag = 0;

        if (!DBAgent::CLoginDispatch::GetDispatchTable().IsEmpty())
        {
            // 로그인 서버 연결상태를 준다.
            dwStatusFlag |= (1 << CServerSetup::LoginServer);
        }

        if (!DBAgent::CUIDDispatch::GetDispatchTable().IsEmpty())
        {
            // UID 서버 연결상태를 준다.
            dwStatusFlag |= (1 << CServerSetup::UIDServer);
        }

        SetStatusFlag(dwStatusFlag);
    }
}


bool CGetUserCountPerServer::operator () (unsigned long dwServerID, CPacketDispatch& packetDispatch)
{
    if(0 != m_lpUserNumPair && m_usServerNum < m_usMaxServerNum)
    {
        ServerManage::UserNumPair& userNumPair = *(m_lpUserNumPair + m_usServerNum);

        userNumPair.m_dwServerID = dwServerID;
        userNumPair.m_nUserNum   = static_cast<DBAgent::CGameDispatch&>(packetDispatch).GetCharNum();

        ++m_usServerNum;
    }

    return true;
}


void CRylDBAgentServer::SendChannelData()
{
    using namespace DBAgent;

    unsigned long dwServerID = CServerSetup::GetInstance().GetServerID();

    // 로그인서버 연결 얻어오기
    GET_SINGLE_DISPATCH(lpLoginDispatch, CLoginDispatch, 
        CLoginDispatch::GetDispatchTable());    

    if(0 != lpLoginDispatch)
    {
        // 로그인 서버로 채널 정보 업데이트
        SendPacket::UpdateChannel(lpLoginDispatch->GetSendStream(), dwServerID);
    }

    // 인증 서버 연결 얻어오기
    GET_SINGLE_DISPATCH(lpAuthDispatch, CAuthDispatch,
        CAuthDispatch::GetDispatchTable());

    if(0 != lpAuthDispatch)
    {
        // 인증 서버로 채널 정보 업데이트
        SendPacket::UpdateChannel(lpAuthDispatch->GetSendStream(), dwServerID);
    }

    // 각 게임서버로 전부 채널 정보 업데이트하기
    CGameDispatch::GetDispatchTable().Process(CUpdateChannelAllServer(dwServerID));
}


// 남은 시간에 DB데이터 업데이트
void CRylDBAgentServer::UpdateDBData(CPulse& Pulse)
{
    using namespace DBAgent::DataStorage;

    const unsigned long MIN_SLEEP_MSEC = 5;
    const unsigned long MIN_UPDATE_NUM = 20;

    unsigned long dwRemainTime = 0;

    CStoreDataMgr&      storeDataMgr        = CStoreDataMgr::GetInstance();
    CCharacterDataMgr&  characterDataMgr    = CCharacterDataMgr::GetInstance();

    dwRemainTime = Pulse.GetRemainTime();
    if(MIN_SLEEP_MSEC < dwRemainTime)
    {
        storeDataMgr.UpdateDBData(MIN_UPDATE_NUM, dwRemainTime);
    }

    dwRemainTime = Pulse.GetRemainTime();
    if(MIN_SLEEP_MSEC < dwRemainTime)
    {
        characterDataMgr.UpdateDBData(MIN_UPDATE_NUM, dwRemainTime);
    }
}

void CRylDBAgentServer::ForceUpdateDBData()
{
    using namespace DBAgent::DataStorage;

    CStoreDataMgr&      storeDataMgr        = CStoreDataMgr::GetInstance();
    CCharacterDataMgr&  characterDataMgr    = CCharacterDataMgr::GetInstance();

    storeDataMgr.UpdateDBData(ULONG_MAX, ULONG_MAX);
    characterDataMgr.UpdateDBData(ULONG_MAX, ULONG_MAX);
}

// 로드되지 않은 데이터를 적당히 관리한다. 
// (로그아웃한지 시간이 오래된 데이터부터 메모리에서 삭제)
void CRylDBAgentServer::ManageUnloadedData()
{
    using namespace DBAgent::DataStorage;

    CStoreDataMgr&      storeDataMgr        = CStoreDataMgr::GetInstance();
    CCharacterDataMgr&  characterDataMgr    = CCharacterDataMgr::GetInstance();

    const unsigned long MAX_MANAGE_DATA_NUM = 3000;

    storeDataMgr.ManageUnloadedData(MAX_MANAGE_DATA_NUM);
    characterDataMgr.ManageUnloadedData(MAX_MANAGE_DATA_NUM);
}
