#include "stdafx.h"
#include "UserIDTable.h"
#include "UIDAgentDispatch.h"
#include "SendAgentPacket.h"

#include <Log/ServerLog.h>
#include <Network/Session/Session.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/WrapPacket.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>
#include <Network/Packet/PacketStruct/ServerPacket.h>

#include <DB/DBComponent.h>
#include <Utility/Setup/ServerSetup.h>


enum UIDServerConst
{
	MAX_PACKET_DISPATCH_PER_PULSE = 30
};


CUIDAgentDispatch::CUIDAgentDispatch(CSession& Session) 
:   CRylServerDispatch(Session, MAX_PACKET_DISPATCH_PER_PULSE),
    m_Group(-1), m_bDisconnected(true)
{
	memset(&m_Address, 0, sizeof(IN_ADDR));
	
	InitUnitNum();
	InitFailNum();
	InitSucceedNum();
}


bool CUIDAgentDispatch::DispatchPacket(PktBase* lpPktBase)
{
    bool bResult = false;

  	switch(lpPktBase->GetCmd())
	{
	case CmdSysServerLogin:		
        bResult = ParseServerLogin(static_cast<PktSL*>(lpPktBase));
        break;

    case CmdSysServerLogout:
        bResult = ParseServerLogout(lpPktBase);
        break;

	case CmdUpdateUIDTable:		
        bResult = ParseUpdateUIDTable(static_cast<PktUUT*>(lpPktBase));
        break;

	default:
		// ��Ŷ Command�� invaild���� ����. invalid�� ���� �ٽ� �˻��ؼ� vaild�� ��ġ�� ã�Ƴ���.		
		LogErrorPacket("��Ŷ ����� ��Ŷ Ŀ�ǵ尡 �߸��Ǿ����ϴ�.", lpPktBase->GetCmd());
		break;
	}

    return true;
}

void CUIDAgentDispatch::Connected()
{
	DETLOG2(g_Log, "DP:0x%p/IP:%s/Servergroup connected", this, 
		GetRemoteAddr().get_addr_string());
}

void CUIDAgentDispatch::Disconnected()
{
	if(!m_bDisconnected)
	{
        bool bResult = false;

        switch(CServerSetup::GetInstance().GetBillingType())
        {
        case CServerSetup::GamaBilling:
            // �Ѱ���
            bResult = ProcessDisconnectHan();
            break;

        case CServerSetup::YouxiLandBilling:
            // ��÷���
            bResult = ProcessDisconnectYouxiLand();
            break;

        case CServerSetup::ROWGlobalBilling:
            // ROW��������
            bResult = ProcessDisconnectROWGlobal();
            break;

		case CServerSetup::JapanBilling:
            // �Ϻ�
            bResult = ProcessDisconnectJapan();
            break;

        case CServerSetup::GammaniaBilling:
            // �����Ͼ�
//            bResult = ProcessDisconnectGammania();
            break;

        case CServerSetup::GamaUnitedBilling:
            // �Ѱ��� ���պ���
            bResult = ProcessDisconnectHanUnited();
            break;

        default:
            ERRLOG1(g_Log, "Unknown Server Type!!! %d", CServerSetup::GetInstance().GetBillingType());
            break;
        };    

        CUserIDTable::GetInstance().RemoveUserOfCurrentAgent(m_Group);
        m_bDisconnected = true;
	}

    if (-1 != m_Group)
    {
        GetDispatchTable().RemoveDispatch(m_Group);
    }

	DETLOG2(g_Log, "DP:0x%p/IP:%s/Servergroup disconnected", this,
        GetRemoteAddr().get_addr_string());
}


bool CUIDAgentDispatch::ParseServerLogin(PktSL* lpPktSL)
{
    if(lpPktSL->GetLen() != sizeof(PktSL))
    {
        // ��Ŷ ���� �ٸ��� ����!
        return false;
    }

	SERVER_ID ServerID = {0,};
	ServerID.dwID = lpPktSL->m_dwServerID;

	m_Group		= ServerID.sID.Group;
	m_Address	= lpPktSL->m_Address;

	if(!GetDispatchTable().SetDispatch(m_Group, this))
	{
		ERRLOG3(g_Log, "DP:0x%p/IP:%s/Duplicated servergroup login : %d",
			this, GetRemoteAddr().get_addr_string(), m_Group);
	
        return false;
	}

	m_bDisconnected = false;

	// Ű�� DB �ʱ�ȭ
/*	if(CServerSetup::GetInstance().IsHangame())
	{
		unsigned long Result = 0;
		if(false == DBComponent::BillingDB::USPDisConnectLogOut(CDBSingleObject::GetInstance(), m_Group, &Result))
		{
			SERLOG2(g_Log, "%d �׷� �α׾ƿ� ����. �����:0x%08x", m_Group, Result);
			return false;
		}

		SERLOG1(g_Log, "%d �׷� USPDisConnectLogOut ȣ�� ����", m_Group);

		if(false == DBComponent::BillingDB::USPServer_Start(CDBSingleObject::GetInstance(), m_Group, &Result))
		{
			SERLOG2(g_Log, "%d �׷� �α׾ƿ� ����. �����:0x%08x", m_Group, Result);
			return false;
		}

		SERLOG1(g_Log, "%d �׷� USPServer_Start ȣ�� ����", m_Group);
	}
*/

	DETLOG3(g_Log, "DP:0x%p/IP:%s/Servergroup login : %d",
		this, GetRemoteAddr().get_addr_string(), m_Group);

	return true;
}


bool CUIDAgentDispatch::ParseServerLogout(PktBase* lpPktBase)
{
    // ���� �α׾ƿ� ��Ŷ�� �޾Ҵ�. �׳� �����ֱ⸸ �ϸ� �ȴ�
    char* lpBuffer = m_SendStream.GetBuffer(sizeof(PktBase));

    if(0 != lpBuffer)
    {
        m_SendStream.WrapHeader(sizeof(PktBase), CmdSysServerLogout, 0, 0);
    }

    return true;
}


bool CUIDAgentDispatch::ParseUpdateUIDTable(PktUUT* lpPktUUT)
{
    bool bResult = false;

    switch(CServerSetup::GetInstance().GetBillingType())
    {
    case CServerSetup::GamaBilling:
        // �Ѱ���
        bResult = ProcessBillingHan(lpPktUUT);
        break;

    case CServerSetup::YouxiLandBilling:
        // ��÷���
        bResult = ProcessBillingYouxiLand(lpPktUUT);
        break;

    case CServerSetup::ROWGlobalBilling:
        // ROW��������
        bResult = ProcessBillingROWGlobal(lpPktUUT);
        break;

    case CServerSetup::JapanBilling:
        // �Ϻ�
        bResult = ProcessBillingJapan(lpPktUUT);
        break;

    case CServerSetup::GammaniaBilling:
        // �����Ͼ�
//        bResult = ProcessBillingGammania(lpPktUUT);
        break;

    case CServerSetup::GamaUnitedBilling:
        // �Ѱ��� ���պ���
        bResult = ProcessBillingHanUnited(lpPktUUT);
        break;

    default:
        ERRLOG1(g_Log, "Unknown Server Type!!! %d", CServerSetup::GetInstance().GetBillingType());
        break;
    };    

    return true;
}

