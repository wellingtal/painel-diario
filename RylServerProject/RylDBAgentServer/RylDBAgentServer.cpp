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

// edith 2009.08.11 ���Ӱ��� 2.5 ���׷��̵�
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

// �� ���� ������ stream���� �̾Ƴ��� Ŭ����
// DBAgent::CGameDispatch::GetDispatchTable().Process() �� ���
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

// ������ ���� ���� ���ϴ� Ŭ����
// DBAgent::CGameDispatch::GetDispatchTable().Process() �� ���
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

// ���ݰ��� ���� ���� ó�� Ŭ����
// DBAgent::DataStorage::CSessionDataMgr::GetInstance().Process() �� ���
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

// �� ������ ä�� ������Ʈ
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
// DB�߰輭�� ���� �Լ���.

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
        // DB�� ������Ʈ
        m_RylDBAgentServer.ForceUpdateDBData();

        DBAgent::DataStorage::CDBAgentItemSerialMgr& dbItemSerialMgr = 
            DBAgent::DataStorage::CDBAgentItemSerialMgr::GetInstance();

        dbItemSerialMgr.SaveItemSerial(
            CDBSingleObject::GetInstance(), dbItemSerialMgr.GetServerID());
        
        DBAgent::DataStorage::CStoreDataMgr::GetInstance().Destroy();
        DBAgent::DataStorage::CCharacterDataMgr::GetInstance().Destroy();

        SERLOG1(g_Log, "this:0x%p / DB�� �����͸� ���� ������Ʈ�߽��ϴ�. ������ �����մϴ�.", this);
	}

	virtual void InternalRun(CPulse& Pulse)	
    {        
        // 3�ʸ��� �ѹ��� ó��
        if(Pulse.ProcessBySecond(3))
        {            
            m_RylDBAgentServer.PrintServerInfo();       // ���� ���� ���
            m_RylDBAgentServer.PrintStatistics();       // ��Ÿ ���� ���
            m_RylDBAgentServer.SendDBAgentManageInfo(); // �������� ������ ����

            CGiveItemMgr::GetInstance().ReadGiveInfo(); // �ӽ��κ����� ������ ������ ������ �д´�.
        }

#ifdef AUTH_MY
		m_RylDBAgentServer.Update( Pulse.GetCurrentPulse() );
#endif

		// 10�ʸ��� �ѹ��� ó��
		if(Pulse.ProcessBySecond(10))
		{            
			// DB�׽�Ʈ
			// edith 2009.10.1 DB �ڵ� Ŀ��Ʈ �߰����
			// 10�ʿ� �ѹ��� ��Ʈ��ũ ������ �˻��Ѵ�.
			// ���� ������ �������� 10���̻� �Ͼ�� DB�� �հ� ������ �ִ�.
			if(CDBSingleObject::GetInstance().GetQueryErrorCount() >= 5)
			{
				// DB�� ���� Ŀ��Ʈ�� �õ��Ѵ�.
				CDBSingleObject::GetInstance().Connect(CDBSingleObject::Class_GameDB);
			}
		}


        // 20�ʸ��� �ѹ��� ó��
        if(Pulse.ProcessBySecond(20))
        {            
            using namespace DBAgent::DataStorage;        

            time_t currentTime = 0;
            time(&currentTime);

            CSessionDataMgr::GetInstance().Process(CProcessBilling(currentTime));   // ���ݰ��� ó���� �Ѵ�
            m_RylDBAgentServer.SendChannelData();                                   // ä�� �����͸� ������
		}

        // 1�и��� �ѹ��� ó��
        if(Pulse.ProcessByMinute(1))
        {            
            CGameEventDBMgr::GetInstance().CheckGameEvent();    // ���� �̺�Ʈ ó��
                                
            Guild::CGuildDBMgr::GetInstance().ProcessGuild();
            m_RylDBAgentServer.ManageUnloadedData();
        }
                
        // 20�и��� �ѹ��� ó��
        if(Pulse.ProcessByMinute(20))
        {
            // �����ս� �α� ���;
            GetFunctionTimingResult("DBAgentLog");
        }

		// 1�ʸ��� �ѹ��� ó��
		if(Pulse.ProcessBySecond(1))
		{
			CGameTimeDBMgr::GetInstance().Process();			// ���� �ð� ���μ��� ó��
			CSiegeObjectDBMgr::GetInstance().Process();         // ���� ���� ó��
		}

        // �ӽ��κ����� �� �������� ������ �ش�.
        CGiveItemMgr::GetInstance().CheckGiveItem();
    
        // �ð� ������ �����ð����� ĳ�� DB������Ʈ
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

	// Agent���� ���̺� �ʱ�ȭ
	// Session Policy ����
	// Listener �ʱ�ȭ
	// ���μ��� ������ �߰�

    if(!CServerSetup::GetInstance().Initialize(CServerSetup::AgentServer))
    {
		// ���� �¾� �ε� ����
		szErrorString = APP_SPECIFIC_INIT_ERR("ServerSetup initialize failed");
    }
    else if(!CDBSingleObject::GetInstance().Connect(CDBComponent::Class_GameDB))
    {
        // DB�ʱ�ȭ ����
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

            // DB �ν��Ͻ� ����
            CStoreDataMgr::GetInstance().SetDBComponent(&CDBSingleObject::GetInstance());
            CCharacterDataMgr::GetInstance().SetDBComponent(&CDBSingleObject::GetInstance());
            CGiveItemMgr::GetInstance().SetDBComponent(CDBSingleObject::GetInstance());
            
            // ������ ID�� ���� ID�� Zone(ID) / Channel ���� ���谡 �ִ�.
            // Zone�� ��� ������, Channel�� 32������ �Ѵ�(����� ���� �׸� ���� ���� ���� �������;;)
            // ����� ��� ä���� 
            // 0 : �� ���(�񺣷� ���� RCMS_TOOL)
            // 1 : �� ���
            // 2 ~ 7 : ��Ʋ ���ѿ��� ȯ���� ���� ����
            // ���̴�.

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
                // Session Policy �ۼ� ����
                szErrorString = APP_SPECIFIC_INIT_ERR("DB�߰輭�� ������ �ø��� �о���� ����");
            }
            if(!CreateSessionPolicy())
	        {
		        // Session Policy �ۼ� ����
		        szErrorString = APP_SPECIFIC_INIT_ERR("SessionPolicy create failed");
	        }
	        else if(!ListenAll())
            {
		        // Listen ����
		        szErrorString = APP_SPECIFIC_INIT_ERR("Listener initialize failed");
            }
            else if (!CRankingDBMgr::GetInstance().Initialize())
            {
                // ��ŷ �ʱ�ȭ ����
                szErrorString = APP_SPECIFIC_INIT_ERR("RankingDBMgr Initialize failed");
            }
            else if (!CAdminDataMgr::GetInstance().LoadAdminList(CDBSingleObject::GetInstance()))
            {
                // ��� DB�ʱ�ȭ ����
                szErrorString = APP_SPECIFIC_INIT_ERR("AdminDataMgr Initialize failed");
            }
			else if (!CGameTimeDBMgr::GetInstance().Initialize(CDBSingleObject::GetInstance()))
			{
				// ���� �ð� �ʱⰪ �о���� ���� (������ ���� �о���Ѵ�. ���� ���� ���� üũ������...)
				szErrorString = APP_SPECIFIC_INIT_ERR("CGameTimeDBMgr Read DB time value failed");
			}
            else if (!Guild::CGuildDBMgr::GetInstance().Initialize(CDBSingleObject::GetInstance()))
            {
                // ��� �ʱ�ȭ ���� 
                szErrorString = APP_SPECIFIC_INIT_ERR("GuildDBMgr Initialize failed");        
            }
            else if (!Castle::CCastleDBMgr::GetInstance().Initialize(CDBSingleObject::GetInstance()))
            {
                // �� �ʱ�ȭ ����
                szErrorString = APP_SPECIFIC_INIT_ERR("CastleDBMgr Initialize failed");
            }
            else if (!CSiegeObjectDBMgr::GetInstance().Initialize(CDBSingleObject::GetInstance()))
            {
                // ķ�� �ʱ�ȭ ����
                szErrorString = APP_SPECIFIC_INIT_ERR("SiegeObjectDBMgr Initialize failed");
            }
			else if (!CMineralVeinDBMgr::GetInstance().Initialize(CDBSingleObject::GetInstance()))
			{
				// ���� DB �б� ����
				szErrorString = APP_SPECIFIC_INIT_ERR("CMineralVeinDBMgr Initialize failed");
			}
            else if (!CGameEventDBMgr::GetInstance().Initialize(CDBSingleObject::GetInstance()))
            {
                // ���� �̺�Ʈ �ʱ�ȭ ����
                szErrorString = APP_SPECIFIC_INIT_ERR("CGameEventDBMgr Initialize failed");
            }
			else if (!CStatueDBMgr::GetInstance().Initialize(CDBSingleObject::GetInstance()))
			{
				// ���� �ʱⰪ �о���� ����
				szErrorString = APP_SPECIFIC_INIT_ERR("CStatueDBMgr Initialize failed");
			}
	        else if(!InitializeCommand())
	        {
		        // Ŀ�ǵ� �ʱ�ȭ ����
		        szErrorString = APP_SPECIFIC_INIT_ERR("Server command create failed");
	        }
	        else if(!InitializeMsgProc())
	        {
		        // �޽��� ���μ��� �ʱ�ȭ ����
		        szErrorString = APP_SPECIFIC_INIT_ERR("Server message proc create failed");
	        }
	        else if(!AddProcessThread(new CRylDBAgentServerProcess(*this, 100)))
	        {
		        // ��Ŷ ó�� ������ ���� ����
		        szErrorString = APP_SPECIFIC_INIT_ERR("Packet process thread failed");
	        }
			else
			{
				CCreatureManager::GetInstance().InitRealmStatue();

				// ������ ǥ�� ���� ������
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
	// ���⼭ ����� ���� ���Ӽ����� �����ϴ��� ��Ÿ �ٸ� �ൿ�� �ϴ��� �Ѵ�.
	CPacketEvent::EventCMD(dwCmd, dwValue);

	switch(dwCmd)
	{
	case SC_IPLISTEND:
		m_Counter = 62;
		break;

	case SC_SHUTDOWN:	// �����Ѵ�.
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

		// MD5����
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

		// 1�ʿ� �ѹ���
		if(0 == dwTick % (5 * 10))
		{
			if(!g_NetAuth.IsConnect())
			{
				g_NetAuth.Init("nf.returnofwarrior.com", 14050);
				//g_NetAuth.Init("192.168.0.7", 14050);
				iConnectTick++;

				// 10�� ���ӽõ��ؼ� ������ ������
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

			// ���ӿ� ���������� IPList�� ��û�Ѵ�.
			g_NetAuth.Send_CMD(CS_IPLIST, 0);
			m_Counter = 60;
			return;
		}

		if(m_Counter == 62)
		{
			// �� �������� Ư���� �ൿ�� �Ѵ�.
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

	// 60�ʿ� �ѹ���
	if(0 == dwTick % (60 * 10))
	{
		// 1�п� 1�� ���� 
		m_Counter++;

		if(m_Counter > 100)
			PostMessage(GetWnd(), WM_QUIT, 0, 0);
	}
}

#endif

void CRylDBAgentServer::ReloadSetup()
{
    // �¾� �ʱ�ȭ
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
            // �α��� ���� ������. �����Ѵ�.            
            INET_Addr& loginServerAddr = serverSetup.GetServerAddress(CServerSetup::LoginServer);

            lpIOCPNet->Connect(m_lpLoginPolicy, loginServerAddr.get_addr_string(), 
                loginServerAddr.get_port_in());
        }

        GET_SINGLE_DISPATCH(lpUIDDispatch, DBAgent::CUIDDispatch,
            DBAgent::CUIDDispatch::GetDispatchTable());

        if(0 == lpUIDDispatch)
        {
            // UID ���� ������. �����Ѵ�.
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
                // Part1 DB�߰輭������ ���� ������. �����Ѵ�.
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

    // �α��� ���� ���� ���� ���
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


    // UID ���� ���� ���� ���
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

    // ���� ���� ���� ���� ���
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


    // ä�� ���� ���� ���� ���
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

    // ���Ӽ��� ���� ���� ���

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

    // ĳ�� �� ĳ���� ���� ����
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
    // �� �������� ���� ���� �ֱ�
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
            // �α��� ���� ������¸� �ش�.
            dwStatusFlag |= (1 << CServerSetup::LoginServer);
        }

        if (!DBAgent::CUIDDispatch::GetDispatchTable().IsEmpty())
        {
            // UID ���� ������¸� �ش�.
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

    // �α��μ��� ���� ������
    GET_SINGLE_DISPATCH(lpLoginDispatch, CLoginDispatch, 
        CLoginDispatch::GetDispatchTable());    

    if(0 != lpLoginDispatch)
    {
        // �α��� ������ ä�� ���� ������Ʈ
        SendPacket::UpdateChannel(lpLoginDispatch->GetSendStream(), dwServerID);
    }

    // ���� ���� ���� ������
    GET_SINGLE_DISPATCH(lpAuthDispatch, CAuthDispatch,
        CAuthDispatch::GetDispatchTable());

    if(0 != lpAuthDispatch)
    {
        // ���� ������ ä�� ���� ������Ʈ
        SendPacket::UpdateChannel(lpAuthDispatch->GetSendStream(), dwServerID);
    }

    // �� ���Ӽ����� ���� ä�� ���� ������Ʈ�ϱ�
    CGameDispatch::GetDispatchTable().Process(CUpdateChannelAllServer(dwServerID));
}


// ���� �ð��� DB������ ������Ʈ
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

// �ε���� ���� �����͸� ������ �����Ѵ�. 
// (�α׾ƿ����� �ð��� ������ �����ͺ��� �޸𸮿��� ����)
void CRylDBAgentServer::ManageUnloadedData()
{
    using namespace DBAgent::DataStorage;

    CStoreDataMgr&      storeDataMgr        = CStoreDataMgr::GetInstance();
    CCharacterDataMgr&  characterDataMgr    = CCharacterDataMgr::GetInstance();

    const unsigned long MAX_MANAGE_DATA_NUM = 3000;

    storeDataMgr.ManageUnloadedData(MAX_MANAGE_DATA_NUM);
    characterDataMgr.ManageUnloadedData(MAX_MANAGE_DATA_NUM);
}
