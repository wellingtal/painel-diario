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
		// 패킷 Command가 invaild한지 본다. invalid한 경우는 다시 검색해서 vaild한 위치를 찾아낸다.		
		LogErrorPacket("패킷 헤더의 패킷 커맨드가 잘못되었습니다.", lpPktBase->GetCmd());
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
            // 한게임
            bResult = ProcessDisconnectHan();
            break;

        case CServerSetup::YouxiLandBilling:
            // 요시랜드
            bResult = ProcessDisconnectYouxiLand();
            break;

        case CServerSetup::ROWGlobalBilling:
            // ROW고유인증
            bResult = ProcessDisconnectROWGlobal();
            break;

		case CServerSetup::JapanBilling:
            // 일본
            bResult = ProcessDisconnectJapan();
            break;

        case CServerSetup::GammaniaBilling:
            // 감마니아
//            bResult = ProcessDisconnectGammania();
            break;

        case CServerSetup::GamaUnitedBilling:
            // 한게임 통합빌링
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
        // 패킷 길이 다르면 무시!
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

	// 키퍼 DB 초기화
/*	if(CServerSetup::GetInstance().IsHangame())
	{
		unsigned long Result = 0;
		if(false == DBComponent::BillingDB::USPDisConnectLogOut(CDBSingleObject::GetInstance(), m_Group, &Result))
		{
			SERLOG2(g_Log, "%d 그룹 로그아웃 실패. 결과값:0x%08x", m_Group, Result);
			return false;
		}

		SERLOG1(g_Log, "%d 그룹 USPDisConnectLogOut 호출 성공", m_Group);

		if(false == DBComponent::BillingDB::USPServer_Start(CDBSingleObject::GetInstance(), m_Group, &Result))
		{
			SERLOG2(g_Log, "%d 그룹 로그아웃 실패. 결과값:0x%08x", m_Group, Result);
			return false;
		}

		SERLOG1(g_Log, "%d 그룹 USPServer_Start 호출 성공", m_Group);
	}
*/

	DETLOG3(g_Log, "DP:0x%p/IP:%s/Servergroup login : %d",
		this, GetRemoteAddr().get_addr_string(), m_Group);

	return true;
}


bool CUIDAgentDispatch::ParseServerLogout(PktBase* lpPktBase)
{
    // 서버 로그아웃 패킷을 받았다. 그냥 돌려주기만 하면 된다
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
        // 한게임
        bResult = ProcessBillingHan(lpPktUUT);
        break;

    case CServerSetup::YouxiLandBilling:
        // 요시랜드
        bResult = ProcessBillingYouxiLand(lpPktUUT);
        break;

    case CServerSetup::ROWGlobalBilling:
        // ROW고유인증
        bResult = ProcessBillingROWGlobal(lpPktUUT);
        break;

    case CServerSetup::JapanBilling:
        // 일본
        bResult = ProcessBillingJapan(lpPktUUT);
        break;

    case CServerSetup::GammaniaBilling:
        // 감마니아
//        bResult = ProcessBillingGammania(lpPktUUT);
        break;

    case CServerSetup::GamaUnitedBilling:
        // 한게임 통합빌링
        bResult = ProcessBillingHanUnited(lpPktUUT);
        break;

    default:
        ERRLOG1(g_Log, "Unknown Server Type!!! %d", CServerSetup::GetInstance().GetBillingType());
        break;
    };    

    return true;
}

