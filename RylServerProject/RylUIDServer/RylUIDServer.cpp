#include "stdafx.h"
#include "RylUIDServer.h"

#include "UIDAgentDispatch.h"
#include "SendAgentPacket.h"

#include "HanUnitedBilling.h"
#include "HanUnitedDisconnID.h"

#include <Log/ServerLog.h>
#include <Thread/Thread.h>
#include <Thread/ThreadMgr.h>
#include <Network/IOCP/IOCPNet.h>
#include <Network/Winsock/SocketFactory.h>
#include <Utility/Debug/DebugMacros.h>
#include <Utility/Time/Pulse/Pulse.h>

#include <Network/Session/Session.h>
#include <Network/Session/CreatePolicy.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/WrapPacket.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>
#include <Network/Packet/PacketStruct/ServerPacket.h>

#include <DB/DBComponent.h>
#include <DB/BillingDBComponent.h>
#include <Utility/Setup/ServerSetup.h>

#include <mmsystem.h>
#include <Utility/Debug/PerformanceCheck.h>

CRylUIDServer& CRylUIDServer::GetInstance()
{
	static CRylUIDServer rylUIDServer;
	return rylUIDServer;
}


class CUIDServerProcessThread : public CProcessThread
{
public:

	enum
	{		
		UIDSERVER_PROCESS_TICK	= 100,
		PULSE_PER_SEC			= 1000 / UIDSERVER_PROCESS_TICK
	};

	CUIDServerProcessThread(CRylUIDServer& rylUIDServer)
		:	m_rylUIDServer(rylUIDServer), CProcessThread(rylUIDServer, UIDSERVER_PROCESS_TICK)
	{

	}

    virtual void Cleanup(CPulse& Pulse)	
	{        
        GetFunctionTimingResult("UIDPerfCheck");
	}

	virtual void InternalRun(CPulse& pulse)
	{
        if (0 == pulse.GetCurrentPulse() % (2 * PULSE_PER_SEC))
        {
			// 2��
            m_rylUIDServer.PrintServerInfo();
        }        

        // ���� Ÿ���� ���´�.
        unsigned long dwBillingType = CServerSetup::GetInstance().GetBillingType();

		if (0 == pulse.GetCurrentPulse() % (60 * PULSE_PER_SEC) &&
            (CServerSetup::GamaBilling == dwBillingType ||
            CServerSetup::GamaUnitedBilling == dwBillingType))
		{
			// 60��
			m_rylUIDServer.CheckCurrentUser();
		}

		m_rylUIDServer.SetStatusFlag(0);

#ifdef AUTH_MY
		m_rylUIDServer.Update( pulse.GetCurrentPulse() );
#endif
        if ((0 == pulse.GetCurrentPulse() % (5 * PULSE_PER_SEC)) 
            && CServerSetup::GamaUnitedBilling == dwBillingType)
        {
            // 5�ʸ��� ���պ��� ���ؼ� üũ �� Callback Timerüũ
            GET_SINGLE_DISPATCH(lpHanUnitedBilling, CHanUnitedDispatch,
                CHanUnitedDispatch::GetDispatchTable());

            if (0 == lpHanUnitedBilling || !lpHanUnitedBilling->GetSession().IsConnected())
            {
                // ������ ������ �����̴�. ������ �õ��Ѵ�.
                CHanUnitedDispatch::ConnectToHanUnited(*m_rylUIDServer.GetIOCPNet());

                // �̵���� ���պ��������� ���� ����.
                m_rylUIDServer.SetStatusFlag(0x00000001);
            }
            else
            {
                // �α׾ƿ��� ���� ���� �� ������ ��Ƶξ��� �����Ѵ�.
                CHanUnitedLogout::GetInstance().Retransmission();

                // �̹� ���ӵǾ� �ִ�. ������ timeoutó���� �Ѵ�.
                lpHanUnitedBilling->ProcessTimeout();
            }
        }

		if(pulse.ProcessBySecond(10))
		{
			// DB�׽�Ʈ
			// edith 2009.10.1 DB �ڵ� Ŀ��Ʈ �߰����
			// 10�ʿ� �ѹ��� ��Ʈ��ũ ������ �˻��Ѵ�.
			// ���� ������ �������� 10���̻� �Ͼ�� DB�� �հ� ������ �ִ�.
			if(CDBSingleObject::GetInstance().GetQueryErrorCount() >= 5)
			{
				// DB�� ���� Ŀ��Ʈ�� �õ��Ѵ�.
				CDBSingleObject::GetInstance().Connect(CDBSingleObject::Class_KeeperDB);
			}
		}


        if ( (0 == pulse.GetCurrentPulse() % (10 * PULSE_PER_SEC))  && 
			CServerSetup::GamaUnitedBilling == dwBillingType )
		{			
			// 10�ʸ��� ������ üũ�ؼ�, ����� ����� ������ ���ų�, ��� ������.
			CHanUnitedDisconnectID::GetInstance().CheckDisconnect();
        }
	}

private:

	CRylUIDServer&	m_rylUIDServer;
};


CRylUIDServer::CRylUIDServer()
:	m_lpAgentSessionPolicy(SessionPolicy::CreateTCPPolicy<CUIDAgentDispatch>())
{
}

CRylUIDServer::~CRylUIDServer()
{
	if(m_lpAgentSessionPolicy)
	{
		m_lpAgentSessionPolicy->Release();
		m_lpAgentSessionPolicy = 0;
	}
}


bool CRylUIDServer::ApplicationSpecificInit(const TCHAR* szCmdLine)
{
	const TCHAR* szErrorMessage = 0; 

	// ���� �¾� �ʱ�ȭ - �� �������� �ؾ� ��.
	if(!CServerSetup::GetInstance().Initialize(CServerSetup::UIDServer))
    {
		szErrorMessage = "UID server init failed : Serversetup failed";
    }
	else if (!CDBSingleObject::GetInstance().Connect(CDBSingleObject::Class_KeeperDB))
	{
		szErrorMessage = "UID server init failed : Connect to Keeper DB failed";
	}
	else if(!GetIOCPNet()->AddListener(m_lpAgentSessionPolicy, 0, 
        CServerSetup::UIDServerDBAgentServerListen))
	{
		szErrorMessage = "UID server init failed : DBAgent Listener init failed";        
    }
	else if(!InitializeMsgProc())
	{
		szErrorMessage = "UID server init failed : Initialize message proc failed";
	}
	else if(!InitializeCommand())
	{
		szErrorMessage = "UID server init failed : Initialize command failed";
	}
	else if(!AddProcessThread(new CUIDServerProcessThread(*this)))
	{
		szErrorMessage = "UID server init failed : Add process thread failed";
	}

	if(0 != szErrorMessage)
	{
		SERLOG2(g_Log, "this:0x%p/%s", this, szErrorMessage);
		return false;
	}

#ifdef AUTH_MY
	m_Counter = 0;
	g_NetAuth.SetEventListener(this);

	g_IPSec.LoadAllowIPZ(L"./Script/Server/AllowIPList.bin");
	g_IPSec.LoadBlockIPZ(L"./Script/Server/BlockIPList.bin");

#endif

	// edith 2009.09.07 TblCurrentUser ���� 1�� ����.
	// TblCurrentUser ������ �����Ѵ�. (������ �������� ����Ʈ������ ����ȵǴ� ���׸� �����ϱ� ����.)
	DBComponent::BillingDB::InitUserList(CDBSingleObject::GetInstance());


    DETLOG0(g_Log, "UID server start.");
    return true;
}

#ifdef AUTH_MY

void CRylUIDServer::EventIRC(CHAR* strCmd, CHAR* strMsg)
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

void CRylUIDServer::EventCMD(DWORD dwCmd, DWORD dwValue)
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

void CRylUIDServer::EventConnect(BOOL bConnect)
{
	CPacketEvent::EventConnect(bConnect);

	m_EasyCmd = 0;
	m_dwServerType = AT_UID;

	if(bConnect)
	{
		char Buff[512];
		int len = 512;
		int result;
		result = ::GetModuleFileName(::GetModuleHandle(NULL), Buff, len);

		// MD5����
		char strMD5[40];
		GetMD5(Buff, strMD5);

		g_NetAuth.Send_AUTHOR(MAKEWPARAM(AT_UID, 0), strMD5);
		m_Counter = 61;
	}

}

static int iExitLoopCount = 0;
void CRylUIDServer::Update(unsigned long dwTick)
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

void CRylUIDServer::PrintStatistics()
{
	CIOCPNet* lpIOCPNet = GetIOCPNet();
	if(0 != lpIOCPNet)
	{
        PrintOutput("Accept Pending : %d / SessionNum : %d", 
            lpIOCPNet->GetAcceptPendingNum(), lpIOCPNet->GetSessionNum());
	}
}


class CPrintDispatchInfo
{
public:

	CPrintDispatchInfo(char* szBuffer, int nBufferSize, int& nWriteBytes_Out)
		:	m_szBuffer(szBuffer), m_nBufferSize(nBufferSize), m_nWriteBytes(nWriteBytes_Out)
	{

	}

	bool operator () (unsigned long dwGroupNum, CPacketDispatch& packetDispatch)
	{
		CUIDAgentDispatch& uidAgentDispatch = static_cast<CUIDAgentDispatch&>(packetDispatch);

        int nWriteBytes = _snprintf(m_szBuffer + m_nWriteBytes, m_nBufferSize - m_nWriteBytes, 
            "[%2dGroup] User : %d, Succeed : %d, Failed : %d\r\n",
			dwGroupNum, uidAgentDispatch.GetUnitNum(), uidAgentDispatch.GetSucceedNum(), uidAgentDispatch.GetFailNum());		
    
        if(0 < nWriteBytes) { m_nWriteBytes += nWriteBytes; }

		return true;
	}

private:

	char*	m_szBuffer;
	int		m_nBufferSize;
	int&	m_nWriteBytes;	
};

void CRylUIDServer::PrintServerInfo()
{
	const int MAX_BUFFER = 4096;
    char      szBuffer[MAX_BUFFER];
	
	// �߰輭�� ����
	int AgentNum = CUIDAgentDispatch::GetDispatchTable().GetDispatchNum();

    int nWriteBytes = 0;
    int nTotalWriteBytes = 0;

    if(CServerSetup::GamaUnitedBilling == CServerSetup::GetInstance().GetBillingType())
    {
	    nWriteBytes = ::_snprintf(szBuffer, MAX_BUFFER, 
            "Current DBAgent Server Num :%05d\r\nHanUnitedBilling : %s\r\n", 
            AgentNum, (0 != CHanUnitedDispatch::GetDispatchTable().GetDispatchNum()) ? "Connected" : "Disconnected");
    }
    else
    {
        nWriteBytes = ::_snprintf(szBuffer, MAX_BUFFER, 
            "Current DBAgent Server Num :%05d\r\n", AgentNum);
    }

    if(0 < nWriteBytes) { nTotalWriteBytes += nWriteBytes; }

	CUIDAgentDispatch::GetDispatchTable().Process(
		CPrintDispatchInfo(szBuffer + nTotalWriteBytes, MAX_BUFFER - nTotalWriteBytes, nTotalWriteBytes));

    if(0 < nTotalWriteBytes)
    {
        szBuffer[nTotalWriteBytes] = 0;
        PrintInfo(szBuffer);
    }    
}   

void CRylUIDServer::CheckCurrentUser()
{
	char Query[256] = "";
	int Rows = 0;

	RE_USPGRoomCurrent_DisConn Data[CDBSingleObject::MaxRowNum] = {0,};

	sprintf(Query, "select strClientID, UID, ServerID from TblCurrentUser_Disconn"); 
	
    for(int StartRows = 0;; StartRows += Rows)
	{
		memset(&Data, 0, sizeof(RE_USPGRoomCurrent_DisConn) * CDBSingleObject::MaxRowNum);

		if(!CDBSingleObject::GetInstance().Select(Query, (void**)&Data, 
            sizeof(RE_USPGRoomCurrent_DisConn), StartRows, CDBSingleObject::MaxRowNum, &Rows))
		{
			ERRLOG0(g_Log, "TblCurrentUser_Disconn ���̺� ��� ����.");
			break;
		}

		for(int Count = 0; Count < Rows; ++Count)
		{
            DBComponent::BillingDB::USPGRoomCurrent_DisConn(CDBSingleObject::GetInstance(), (char *)Data[Count].ClientID);
			AgentSendPacket::SendUserKill(Data[Count].ServerID, Data[Count].UID);
		}

		if(Rows != CDBSingleObject::MaxRowNum)
		{
			break;
		}
	}
}

void CRylUIDServer::SendPost(int iIndex, char* pPacket)
{
}

LPUIDINFO CRylUIDServer::AddAuth(UIDINFO stInfo)
{
	m_AuthMap[stInfo.AccountName] = stInfo;

	return &m_AuthMap[stInfo.AccountName];
}

LPUIDINFO CRylUIDServer::FindAuth(char* strAccount)
{
	std::map<std::string, UIDINFO>::iterator obj = m_AuthMap.find(strAccount);
	if(obj == m_AuthMap.end())
		return NULL;

	return &obj->second;
}
