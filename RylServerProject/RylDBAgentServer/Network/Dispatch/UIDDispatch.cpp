#include "stdafx.h"
#include "UIDDispatch.h"

#include "AuthDispatch.h"
#include "GameDispatch.h"
#include "ChatDispatch.h"

#include <DB/DBComponent.h>
#include <DB/GameDBComponent.h>

#include <Log/ServerLog.h>
#include <Network/Address/INET_Addr.h>
#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>
#include <Network/Packet/PacketStruct/ServerPacket.h>
#include <Network/Packet/PacketStruct/CharLoginOutPacket.h>
#include <Network/Packet/PacketStruct/CharLoginOutPacketStruct.h>
#include <Network/SendPacket/SendServerInfo.h>
#include <Network/SendPacket/SendLoginout.h>
#include <Network/SendPacket/SendCharManage.h>

#include <DataStorage/Billing.h>
#include <DataStorage/SessionData.h>
#include <DataStorage/SessionDataMgr.h>

#include <Utility/Setup/ServerSetup.h>

namespace DBAgent
{

const char* g_szPktUUTString[PktUUT::MAX_UPDATE_TYPE] =
{
    "UpdateUIDTableNone",
    "UpdateUIDTableUserLogin",
    "UpdateUIDTableUserLogout",
    "UpdateUIDTableUserMove",
    "UpdateUIDTableCharLogin",
    "UpdateUIDTableCharLogout",
    "UpdateUIDTableCharMove",
    "UpdateUIDTableBillingCheck",
};



// UpdateUIDTableó���� ���� �α��� ó���� �Ѵ�.
// ���ϰ��� ��� ���� ���ڿ��̴�. NULL�̸� �����̴�.
const char* ProcessUpdateTableUserLogin(DataStorage::CSessionData& sessionData, 
                                        DataStorage::RequestData& requestData, 
                                        unsigned long dwFlag, int nPlayTime, 
                                        unsigned long dwCRMIndex1,
                                        unsigned char cCmd, char cBillingType, 
                                        unsigned short usUUKAckError);

// UpdateUIDTableó���� ĳ���� �α��� ó���� �Ѵ�.
// ���ϰ��� ��� ���� ���ڿ��̴�. NULL�̸� �����̴�.
const char* ProcessUpdateTableCharLogin(DataStorage::CSessionData& sessionData, 
                                        DataStorage::RequestData& requestData, 
                                        unsigned long dwFlag, int nPlayTime, 
                                        unsigned long dwCRMIndex1, 
                                        unsigned char cCmd, char cBillingType);

CSingleDispatch& CUIDDispatch::GetDispatchTable()
{
    static CSingleDispatch uidDispatch;
    return uidDispatch;
}

CUIDDispatch::CUIDDispatch(CSession& Session)
    :   CRylServerDispatch(Session, MAX_PACKET_DISPATCH_PER_PULSE)
{


}

CUIDDispatch::~CUIDDispatch()
{


}

void CUIDDispatch::Connected()
{
    DETLOG3(g_Log, "SS:0x%08x/DP:0x%08x/IP:%15s/UIDServer Connected",
        &GetSession(), this, GetRemoteAddr().get_addr_string());

    GetDispatchTable().SetDispatch(this);

    SendPacket::ServerLogin(GetSendStream(), 
        CServerSetup::GetInstance().GetServerID());
}

void CUIDDispatch::Disconnected()
{
    DETLOG3(g_Log, "SS:0x%08x/DP:0x%08x/IP:%15s/UIDServer Disconnected",
        &GetSession(), this, GetRemoteAddr().get_addr_string());

    GetDispatchTable().RemoveDispatch(this);
}

bool CUIDDispatch::DispatchPacket(PktBase* lpPktBase)
{
    PktBase::CMDType cCmd = lpPktBase->GetCmd();

    bool bResult = false;

    switch(cCmd)
    {
    case CmdSysServerLogout:        bResult = ParseServerLogoutAck(lpPktBase);                              break;
    case CmdUserKill:               bResult = ParseUserKill(static_cast<PktUK*>(lpPktBase));                break;
    case CmdBillingTimeoutNotify:   bResult = ParseBillingTimeNotify(static_cast<PktBTN*>(lpPktBase));      break;
    case CmdBillingTimeCheckNotify: bResult = ParseBillingTimeCheckNotify(static_cast<PktBTN*>(lpPktBase)); break;
    case CmdHanBTNWarning:          bResult = ParseHanBTNWarning(static_cast<PktHanBTN*>(lpPktBase));       break;
    case CmdHanBTNUserKill:         bResult = ParseHanBTNUserKill(static_cast<PktHanUserKill*>(lpPktBase)); break;
    case CmdUpdateUIDTable:         bResult = ParseUpdateUIDTable(static_cast<PktUUTAck*>(lpPktBase));      break;

    default:

        ERRLOG4(g_Log, "SS:0x%08x/DP:0x%08x/IP:%15s/Cmd:0x%02X/ UID���� ��Ŷ ó�� ���� : ���� Ŀ�ǵ��Դϴ�",
            &GetSession(), this, GetRemoteAddr().get_addr_string(), cCmd);

        bResult = true;
        break;
    }

    if(!bResult)
    {
        ERRLOG4(g_Log, "SS:0x%08x/DP:0x%08x/IP:%15s/Cmd:0x%02X/ UID���� ��Ŷ ó�� ���� : ó���� �����߽��ϴ�",
            &GetSession(), this, GetRemoteAddr().get_addr_string(), cCmd);
    }

    return true;
}


bool CUIDDispatch::ParseServerLogoutAck(PktBase* lpPktBase)
{
    INFLOG3(g_Log, "SS:0x%08x/DP:0x%08x/IP:%15s/Cmd:0x%02X/ UID���� ���� ���� : ���� �α׾ƿ� ��Ŷ�� �޾ҽ��ϴ�.",
        &GetSession(), this, GetRemoteAddr().get_addr_string());

    CloseSession();
    return true;
}


bool CUIDDispatch::ParseUpdateUIDTable(PktUUTAck* lpPktUUTAck)
{    
	// edith 2008.01.15 UID���� ���� UpdateUIDTable ����� ó����ƾ

    using namespace DBAgent::DataStorage;

    unsigned char   cCmd		        = lpPktUUTAck->m_cCmd;
    unsigned long   dwUID               = lpPktUUTAck->m_dwUserID;
    unsigned long   dwFlag		        = lpPktUUTAck->m_dwFlag;
    int             nPlayTime	        = lpPktUUTAck->m_nPlayTime;
    unsigned long   dwCRMIndex1         = lpPktUUTAck->m_dwCRMIndex1;       // �ǽù� ID (0�̸� ��������)
    char            cBillingType	    = lpPktUUTAck->m_cstrBillingType;

    CBilling::WarningType eOldWarnMsgType = CBilling::WARN_BEFORE_1SEC;
    CBilling::WarningType eNewWarnMsgType = static_cast<CBilling::WarningType>(lpPktUUTAck->GetState());

    const char*     szErrorReason       = 0;

    RequestData requestData;
    memset(&requestData, 0, sizeof(RequestData));

    CSessionData* lpSessionData = CSessionDataMgr::GetInstance().GetOpenSession(dwUID);

    if(PktUUT::MAX_UPDATE_TYPE <= cCmd)
    {
        szErrorReason = "�α��� ���� - �� �� ���� Ŀ�ǵ��Դϴ�";
    }
    else if(0 == lpSessionData)
    {
        szErrorReason = "�α��� ���� - ������ �����ϴ�";
    }
    else
    {
        switch(cCmd)
        {
        case PktUUT::UpdateUIDTableUserLogin:

            requestData = lpSessionData->PopRequest();
            
            if(0 == requestData.m_dwRequestKey)
            {
                szErrorReason = "�α��� ���� - RequestKey�� �̻��մϴ�";
            }
            else
            {
                szErrorReason = ProcessUpdateTableUserLogin(*lpSessionData, requestData, dwFlag, 
                    nPlayTime, dwCRMIndex1, cCmd, cBillingType, lpPktUUTAck->GetError());

                eOldWarnMsgType = lpSessionData->GetBilling().GetWarnMsgType();
                lpSessionData->GetBilling().SetWarnMsgType(eNewWarnMsgType);

                if(CBilling::WARN_EVERY_MINUTE == eNewWarnMsgType)
                {
                    // ��� ���·� ����. �źи��� �޽����� �������� �Ѵ�.
                    SendPacket::BillingCheckNotify(*lpSessionData, 5, cBillingType);
                }
            }
            break;

        case PktUUT::UpdateUIDTableCharLogin:

            requestData = lpSessionData->PopRequest();
            
            if(0 == requestData.m_dwRequestKey)
            {
                szErrorReason = "�α��� ���� - RequestKey�� �̻��մϴ�";
            }
            else
            {
                szErrorReason = ProcessUpdateTableCharLogin(*lpSessionData, requestData, 
                    dwFlag, nPlayTime, dwCRMIndex1, cCmd, cBillingType);
            }
            break;

        case PktUUT::UpdateUIDTableBillingCheck:
            
            eOldWarnMsgType = lpSessionData->GetBilling().GetWarnMsgType();

            if(0 == dwFlag)
            {
                lpSessionData->GetBilling().SetWarnMsgType(eNewWarnMsgType);

                if (CBilling::NO_WARNING == eOldWarnMsgType && 
                    CBilling::WARN_EVERY_MINUTE == eNewWarnMsgType)
                {
                    // ����� ���¿��� ��� ���·� ����. �źи��� �޽����� �������� �Ѵ�.
                    SendPacket::BillingCheckNotify(*lpSessionData, 5, cBillingType);
                }
                else 
                    if (CBilling::WARN_EVERY_MINUTE == eOldWarnMsgType && 
                        CBilling::NO_WARNING == eNewWarnMsgType)
                    {
                        // ��� ���¿��� ����� ���·� ����. �޽����� �׸� �������� �Ѵ�.
                        SendPacket::BillingCheckNotify(*lpSessionData, 20, cBillingType);
                    }
            }

            lpSessionData->GetBilling().AddYouxiLandPoint(nPlayTime, cBillingType);

            DETLOG4(g_Log, "UID:%10u / CID:%10u / nPlayTime:%d / cBillingType:%c / UpdateUIDTableBillingCheck ����",
                lpSessionData->GetUID(), lpSessionData->GetCID(), nPlayTime, cBillingType);

            if (0 != dwFlag)
            {
                // ���� ų ó�� - Ư���� ���� �ʴ´�. ���߿� ����üũ���� ���� ���̴�.
                // KillUser(lpDataSession);
            }

            break;
        }

    }

    if(0 != szErrorReason)
    {
        ERRLOG10(g_Log, "UID:%10u / Cmd:%s / Flag:%u / PlayTime:%d / CRMIndex:%u / BillingType:%c / "
            " RequestKey:%10u / SelectedCID:%10u / ServerID:0x%08X / ParseUIDTable ���� : %s", 
            dwUID, g_szPktUUTString[cCmd], dwFlag, nPlayTime, dwCRMIndex1, cBillingType, 
            requestData.m_dwRequestKey, requestData.m_dwSelectedCID, 
            requestData.m_dwServerID, szErrorReason);

        if(0 == dwFlag)
        {
            PktUUT::UpdateType eUpdateType = PktUUT::UpdateUIDTableNone;

            switch(cCmd)
            {
            case PktUUT::UpdateUIDTableUserLogin:
                eUpdateType = PktUUT::UpdateUIDTableUserLogout;
                break;

            case PktUUT::UpdateUIDTableCharLogin:
                eUpdateType = PktUUT::UpdateUIDTableCharLogout;
                break;
            }

			// edith 2008.01.15 UID�� �α��� �޽��� ����
            if(0 == lpSessionData)
            {
                ERRLOG1(g_Log, "UID:%10u / UID���� ���� ���� : ������ �����ϴ�.", dwUID);
            }
            else if(eUpdateType != PktUUT::UpdateUIDTableNone &&
				!SendPacket::UpdateUIDTable(eUpdateType, requestData.m_szAccount, requestData.m_szPassword,
                dwUID, requestData.m_dwSelectedCID, requestData.m_dwSessionID, 
                requestData.m_dwServerID, requestData.m_PeerAddress))
            {
                ERRLOG1(g_Log, "UID:%10u / UID���� ���� ���� : ���ۿ� �����߽��ϴ�.", dwUID);
            }
        }
    }


    return true;
}

const char* ProcessUpdateTableUserLogin(DataStorage::CSessionData& sessionData, 
                                        DataStorage::RequestData& requestData, 
                                        unsigned long dwFlag, int nPlayTime, unsigned long dwCRMIndex1,
                                        unsigned char cCmd, char cBillingType, unsigned short usUUKAckError)
{
	// edith 2008.01.15 UpdateTableUserLogin�� ���õ� ���μ��� ����
    const char* szErrorReason = 0; 
    unsigned long dwUID = sessionData.GetUID();

    GET_SINGLE_DISPATCH(lpAuthDispatch, 
        CAuthDispatch, CAuthDispatch::GetDispatchTable());

    if(0 == lpAuthDispatch)
    {
        szErrorReason = "���� �α��� ���� - �������� ���� ����";
    }
    else
    {
        unsigned short usError = 1;  // Default : Server Error(Unknown Error)

        if (0 == dwFlag)
        {
            // ���ռ��� ������ �� �����ش�.
            unsigned char cAgentServerType = static_cast<unsigned char>(
                CServerSetup::GetInstance().GetAgentServerType());

            unsigned char cFirstLogin = 0;

            switch(cAgentServerType)
            {
            case UnifiedConst::Part1:
            case UnifiedConst::Part1Unified:
                // ���� ���� �߸� ����. ���߿� UserLogin
                szErrorReason = "���� �α��� ���� - ���� �߸� ����";
                break;

            case UnifiedConst::ROW:                
                sessionData.SetOldServerGroupID(UnifiedConst::ROW);
                break;

            case UnifiedConst::Part2Unified:
            case UnifiedConst::Part2Selectable:

                if(!sessionData.GetUnifiedDataFromDB(CDBSingleObject::GetInstance()))
                {                 
                    szErrorReason = "���� �α��� ���� - ���ռ��� ������ ��� ����";
                }
                break;
            }

            cFirstLogin = sessionData.GetFirstLogin();

            if(0 != szErrorReason)
            {
                // ������ �о���� ���� �� ����̴�. UserLogin�� ����Ѵ�.
            }
            else if(!sessionData.UserEnable(CDBSingleObject::GetInstance(), dwUID))
            {
                szErrorReason = "���� �α��� ���� - ���� Ȱ��ȭ ����";
            }
            else if(!SendPacket::StartSession(lpAuthDispatch->GetSendStream(),
                requestData.m_dwRequestKey, dwUID, requestData.m_dwSessionID, cFirstLogin, 0))
            {
                szErrorReason = "���� �α��� ���� - ���� �α��� ���� ���� ����";
            }
            else
            {
                // ���� ����(���� ���� ����. ���� �Ѱ��� ���ݰ� ��÷��� ���ݸ� �����).
                // �������������� ���� ����. ���� ���� ǥ�ø� ���ؼ� ������.
                sessionData.GetBilling().StartBilling(nPlayTime, dwCRMIndex1, cBillingType);
                
				// edith 2008.01.15 �α��ο� ���������� ���ǿ� ����Ѵ�.
                // �α��� ������������ ���� ID���� �����͸� �����Ѵ�.                
                sessionData.SetRequestData(requestData);

                // ���� ���� ����
                usError = 0;
                
                // StartSession�� �޾��� ��, ������ 0�̰�, �߰輭�� Ÿ���� ���ռ����̸�,
                // ���ռ��� �����͸� ��ٸ��Ƿ�, �����͸� ���⼭ ������ �ش�.

                switch(cAgentServerType)
                {
                case UnifiedConst::Part2Unified:
                case UnifiedConst::Part2Selectable:

                    // �ű� �������� �ƴ��� ������ �ʿ�� ����. �ϴ� ���� �ְ� ����
                    // ������ ĳ���� ������ �˾Ƽ� �׳� �α����ϱ� �����̴�.
                    if (!SendPacket::UnifiedCharInfo(lpAuthDispatch->GetSendStream(), 
                        dwUID, sessionData.GetTransferedCharCount(CDBSingleObject::GetInstance()),
                        sessionData.GetUserInfo(),                         
                        sessionData.GetUnifiedStoreInfo(), sessionData.GetUnifiedStoreInfoNum(),
                        sessionData.GetUnifiedCharData(), sessionData.GetUnifiedCharDataNum()))
                    {
                        // ���� ���ۿ� ���������� �� �ϴ� �� ����;; �˾Ƽ� ������ ��ٸ��� ����������;;
                        ERRLOG1(g_Log, "UID:%10u / ���� �α��� ���� - ���ռ��� ���� ���� ����", dwUID);
                    }
                    break;
                }
            }
        }		
        else if (1 == dwFlag) 
        {
            switch(usUUKAckError)
            {
            default:
                usError = 41; // �ߺ��α���
                break;

            case PktUUT::DISCONNECT_USER:
                usError = 42; // Disconnect�� ������ ��õ��� ��..
                break;
            }			
        }
        else if (2 == dwFlag) { usError = 24; } // ����,�ǽù� ���� ���� ������ ����(�Ѱ���)
        else if (4 == dwFlag) { usError = 1;  } // ���� ���� (DB Query ����)
        else if (11 == dwFlag || 13 == dwFlag || 19 == dwFlag)
        {
            // ����,�ǽù� ���� ���� ������ ����(�븸)
            usError = 24;   
        }
        else if (20 == dwFlag) { usError = 40; } // �븸, �Ϻ� �ߺ� �α��� ����
		// edith 2009.09.11 MY�� ���� AllowIP ó���۾�
		else if (57 == dwFlag) { usError = 57; }	// ROW���� AllowIP ���� �ɸ�
		else if (100 >= dwFlag) { usError = (unsigned short)(dwFlag-100); }; // �����Ͼ� ����

        if(0 != usError)
        {
            SendPacket::StartSession(lpAuthDispatch->GetSendStream(),
                requestData.m_dwRequestKey, dwUID, sessionData.GetSessionID(), 0, usError);
        }
    }

    return szErrorReason;
}



const char* ProcessUpdateTableCharLogin(DataStorage::CSessionData& sessionData, 
                                        DataStorage::RequestData& requestData, 
                                        unsigned long dwFlag, int nPlayTime, 
                                        unsigned long dwCRMIndex1, 
                                        unsigned char cCmd, char cBillingType)
{
    const char* szErrorReason = 0;
    unsigned long dwUID = sessionData.GetUID();

    GET_MULTI_DISPATCH(lpGameDispatch, requestData.m_dwServerID, 
        CGameDispatch, CGameDispatch::GetDispatchTable());

    if(0 == lpGameDispatch)
    {
        szErrorReason = "ĳ���� �α��� ���� - ���Ӽ��� ���� ����";
    }
    else if(!sessionData.HasCharacter(requestData.m_dwSelectedCID))
    {
        szErrorReason = "ĳ���� �α��� ���� - �������� ���� ĳ���� Ȱ��ȭ";

        const USER_INFO& userInfo = sessionData.GetUserInfo();

		// WORK_LIST 2.1 ���� ���� �߰�
		SERLOG9(g_Log, "UID:%10u / SelectedCID:%10u / Char1:%10u / Char2:%10u / Char3:%10u / Char4:%10u / Char5:%10u / Nation:%d %s", 
            dwUID, requestData.m_dwSelectedCID, 
            userInfo.CharID[0], userInfo.CharID[1], userInfo.CharID[2], 
			userInfo.CharID[3], userInfo.CharID[4], userInfo.Nation, szErrorReason);
    }
    else
    {
        // �⺻���� ����. ���� ����.
        unsigned short usError = 1;

        if (0 == dwFlag)
        {
            // ĳ���� Ȱ��ȭ			
            if (!sessionData.CharEnable(requestData.m_dwSelectedCID, requestData.m_dwServerID))
            {
                szErrorReason = "ĳ���� �α��� ���� - ���� Ȱ��ȭ ����";
            }
            // ĳ���� ������ ����
            else if (!SendPacket::CharLogin(lpGameDispatch->GetSendStream(), 
                sessionData, requestData.m_dwRequestKey))
            {
                szErrorReason = "ĳ���� �α��� ���� - ĳ���� ������ ���� ����";

                // Ȱ��ȭ�� ĳ���͸� Disable��.
                sessionData.CharDisable(requestData.m_dwSelectedCID, requestData.m_dwServerID);
            }
            else 
            {
                // ���� ����
                switch(CServerSetup::GetInstance().GetBillingType())
                {
                case CServerSetup::GamaBilling:
                case CServerSetup::GamaUnitedBilling:                
                    sessionData.GetBilling().StartBilling(nPlayTime, dwCRMIndex1, cBillingType);
                    break;
                }

                // �α��� ������������ ���� ID���� �����͸� �����Ѵ�.
                sessionData.SetRequestData(requestData);

                // ���� ����!
                usError = 0;    
            }
        }
        else if (1 == dwFlag) { usError = 41; }  // �ߺ��α���
        else if (2 == dwFlag) { usError = 24; }  // ����,�ǽù� ���� ���� ������ ����
        else if (4 == dwFlag) { usError = 1;  }  // ���� ���� (DB Query ����)
        else if (11 == dwFlag || 13 == dwFlag || 19 == dwFlag)
        {
            // ����,�ǽù� ���� ���� ������ ����(�븸)
            usError = 24;    
        }
        else if (20 == dwFlag) { usError = 40; }  // �븸, �Ϻ� �ߺ� �α��� ����
		// edith 2009.09.11 MY�� ���� AllowIP ó���۾�
		else if (57 == dwFlag) { usError = 57; }	// ROW���� AllowIP ���� �ɸ�
		else if (100 >= dwFlag) { usError = (unsigned short)(dwFlag-100); }; // �����Ͼ� ����

        if (0 != usError)
        {            
            SendPacket::CharLoginError(lpGameDispatch->GetSendStream(), 
                requestData.m_dwRequestKey, DBUpdateData::LOGIN, 
                dwUID, requestData.m_dwSelectedCID, usError);
        }
    }

    return szErrorReason;
}


bool CUIDDispatch::ParseBillingTimeNotify(PktBTN* lpPktBTN)
{
    using namespace DataStorage;

    CSessionData* lpSessionData = CSessionDataMgr::GetInstance().GetOpenSession(lpPktBTN->m_dwUserID);
    if(0 == lpSessionData)
    {
        ERRLOG2(g_Log, "UID:%10u / cRemainMin:%u / �ð� ���� ������ ������ ���� : ������ �����ϴ�",
            lpPktBTN->m_dwUserID, lpPktBTN->m_cRemainMinute);
    }
    else if(CSessionData::SE_CHAR_ENABLED != lpSessionData->GetSessionState())
    {
        ERRLOG2(g_Log, "UID:%10u / ST:%s / �ð� ���� ������ ������ ���� : ĳ���Ͱ� ��Ȱ��ȭ�Ǿ� �ֽ��ϴ�",
            lpPktBTN->m_dwUserID, g_szSessionStateString[lpSessionData->GetSessionState()]);
    }
    else
    {
        // �ð��� �� �������� ��� ����.
        if(!SendPacket::BillingNotify(*lpSessionData, 
            lpPktBTN->m_cRemainMinute, lpPktBTN->m_cBillingType))
        {
            INFLOG4(g_Log, "UID:%10u / ServerID:0x%08X / cRemainMin:%u / cBillingType:%c / �ð� ���� ���� ������ ����",
                lpPktBTN->m_dwUserID, lpSessionData->GetServerID(), lpPktBTN->m_cRemainMinute, lpPktBTN->m_cBillingType);
        }
        else
        {
            ERRLOG2(g_Log, "UID:%10u / ServerID:0x%08X / �ð� ���� ������ ������ ���� : ���Ӽ������� ������ ������ �ֽ��ϴ�",
                lpPktBTN->m_dwUserID, lpSessionData->GetServerID());
        }
    }

    return true;
}

bool CUIDDispatch::ParseBillingTimeCheckNotify(PktBTN* lpPktBTN)
{
    using namespace DataStorage;

    CSessionData* lpSessionData = CSessionDataMgr::GetInstance().GetOpenSession(lpPktBTN->m_dwUserID);
    if(0 == lpSessionData)
    {
        ERRLOG2(g_Log, "UID:%10u / cRemainMin:%u / �ð� �ٽ� üũ(�븸) ������ ������ ���� : ������ �����ϴ�",
            lpPktBTN->m_dwUserID, lpPktBTN->m_cRemainMinute);
    }
    else if(CSessionData::SE_CHAR_ENABLED != lpSessionData->GetSessionState())
    {
        ERRLOG2(g_Log, "UID:%10u / ST:%s / �ð� �ٽ� üũ(�븸) ������ ������ ���� : ĳ���Ͱ� ��Ȱ��ȭ�Ǿ� �ֽ��ϴ�",
            lpPktBTN->m_dwUserID, g_szSessionStateString[lpSessionData->GetSessionState()]);
    }
    else
    {
        // �ð��� �� �������� ��� ����.
        if(!SendPacket::BillingCheckNotify(*lpSessionData, 
            lpPktBTN->m_cRemainMinute, lpPktBTN->m_cBillingType))
        {
            INFLOG4(g_Log, "UID:%10u / ServerID:0x%08X / cRemainMin:%u / cBillingType:%c / �ð� �ٽ� üũ(�븸) ���� ������ ����",
                lpPktBTN->m_dwUserID, lpSessionData->GetServerID(), lpPktBTN->m_cRemainMinute, lpPktBTN->m_cBillingType);
        }
        else
        {
            ERRLOG2(g_Log, "UID:%10u / ServerID:0x%08X / �ð� �ٽ� üũ ����(�븸)�� ������ ���� : ���Ӽ������� ������ ������ �ֽ��ϴ�",
                lpPktBTN->m_dwUserID, lpSessionData->GetServerID());
        }
    }

    return true;
}


bool CUIDDispatch::ParseHanBTNWarning(PktHanBTN* lpPktHanBTN)
{
    // ä�ü����� �޽����� Relay�Ѵ�.
    using namespace DataStorage;

    IN_ADDR peerAddress;
    peerAddress.S_un.S_addr = lpPktHanBTN->m_dwIP;

    bool bCheckAddress = (0xFFFFFFFF != peerAddress.S_un.S_addr);

    CSessionData* lpSessionData = CSessionDataMgr::GetInstance().GetOpenSession(
        lpPktHanBTN->m_szAccount, peerAddress, bCheckAddress);
    
    if(0 == lpSessionData)
    {
        ERRLOG2(g_Log, "AccountName:%16s / IP:%15s / ���� ���� �޽��� ���� ���� : ������ �����ϴ�",
            lpPktHanBTN->m_szAccount, inet_ntoa(peerAddress));
    }
    else if(CSessionData::SE_CHAR_ENABLED != lpSessionData->GetSessionState())
    {
        ERRLOG4(g_Log, "UID:%10u / AccountName:%16s / IP:%15s / ST:%s / ���� ���� �޽��� ���� ���� : ĳ���Ͱ� ��Ȱ��ȭ�Ǿ� �ֽ��ϴ�",
            lpSessionData->GetUID(), lpPktHanBTN->m_szAccount, inet_ntoa(peerAddress), 
            g_szSessionStateString[lpSessionData->GetSessionState()]);
    }
    else 
    {
        GET_SINGLE_DISPATCH(lpChatDispatch, 
            CChatDispatch, CChatDispatch::GetDispatchTable());

        if(0 != lpChatDispatch)
        {
            char* lpBuffer = lpChatDispatch->GetSendStream().GetBuffer(sizeof(PktHanBTN));

            if(0 != lpBuffer)
            {
                PktHanBTN* lpPktChatBTN = reinterpret_cast<PktHanBTN*>(lpBuffer);

                *lpPktChatBTN = *lpPktHanBTN;

                lpPktChatBTN->m_dwUID = lpSessionData->GetUID();
                lpPktChatBTN->m_dwCID = lpSessionData->GetCID();

                return lpChatDispatch->GetSendStream().WrapHeader(
                    sizeof(PktHanBTN), CmdHanBTNWarning, 0, 0);
            }
        }

        ERRLOG5(g_Log, "UID:%10u / AccountName:%16s / IP:%15s / ST:%s / ChatDispatch:%p / ���� ���� �޽��� ���� ���� : ä�ü����� ������ ����ų�, ���� �Ҵ� ����",
            lpSessionData->GetUID(), lpPktHanBTN->m_szAccount, inet_ntoa(peerAddress), 
            g_szSessionStateString[lpSessionData->GetSessionState()], lpChatDispatch);
    }

    return true;
}

bool CUIDDispatch::ParseHanBTNUserKill(PktHanUserKill* lpPktHanUserKill)
{
    using namespace DataStorage;

    IN_ADDR peerAddress;
    peerAddress.S_un.S_addr = lpPktHanUserKill->m_dwIP;

    // ���� DB�� ���� ������, ������ IP�� ��ġ�ϴ� ������ �߰ſ� ���� ���� �ش�.
    bool bCheckAddress = (peerAddress.S_un.S_addr != 0xFFFFFFFF);

    CSessionData* lpSessionData = CSessionDataMgr::GetInstance().GetOpenSession(
        lpPktHanUserKill->m_szAccount, peerAddress, bCheckAddress);

    if(0 == lpSessionData)
    {
        ERRLOG2(g_Log, "AccountName:%16s / IP:%15s / ���� ���� ���� ���� ���� : ������ �����ϴ�",
            lpPktHanUserKill->m_szAccount, inet_ntoa(peerAddress));
    }
    else 
	{
		SERVER_ID serverID;
		serverID.dwID = lpSessionData->GetServerID();
		CSessionData::SessionState eSessionState = lpSessionData->GetSessionState();

		if ((serverID.GetType() == CServerSetup::AuthServer && CSessionData::SE_USER_ENABLED != eSessionState) ||
			(serverID.GetType() == CServerSetup::GameServer && CSessionData::SE_CHAR_ENABLED != eSessionState))
		{
			ERRLOG3(g_Log, "UID:%10u / ServerID:0x%08X / ST:%s / ���� ���� ���� ���� ���� : ������ ĳ���Ͱ� ��Ȱ��ȭ�Ǿ� �ֽ��ϴ�", 
				lpSessionData->GetUID(), serverID.dwID, g_szSessionStateString[lpSessionData->GetSessionState()]);
		}
		else if(!SendPacket::UserKill(*lpSessionData))
		{
			ERRLOG3(g_Log, "UID:%10u / ServerID:0x%08X / ST:%s / ���� ���� ���� ���� ���� : ���� ���� ��Ŷ ������ ����", 
				lpSessionData->GetUID(), serverID.dwID, g_szSessionStateString[lpSessionData->GetSessionState()]);
		}
	}

    return true;
}

bool CUIDDispatch::ParseUserKill(PktUK* lpPktUK)
{
    using namespace DataStorage;

    // ���� DB�� ���� ������, ������ IP�� ��ġ�ϴ� ������ �߰ſ� ���� ���� �ش�.
    CSessionData* lpSessionData = 
		CSessionDataMgr::GetInstance().GetOpenSession(lpPktUK->m_dwUserID);

    if(0 == lpSessionData)
    {
        ERRLOG1(g_Log, "UID:%10u / UID ���� ���� ���� ���� ���� : ������ �����ϴ�", lpPktUK->m_dwUserID);
    }
    else 
	{
		SERVER_ID serverID;
		serverID.dwID = lpSessionData->GetServerID();
		CSessionData::SessionState eSessionState = lpSessionData->GetSessionState();

		if ((serverID.GetType() == CServerSetup::AuthServer && CSessionData::SE_USER_ENABLED != eSessionState) ||
			(serverID.GetType() == CServerSetup::GameServer && CSessionData::SE_CHAR_ENABLED != eSessionState))
		{
			ERRLOG3(g_Log, "UID:%10u / ServerID:0x%08X / ST:%s / UID ���� ���� ���� ���� ���� : ������ ĳ���Ͱ� ��Ȱ��ȭ�Ǿ� �ֽ��ϴ�", 
				lpPktUK->m_dwUserID, serverID.dwID, g_szSessionStateString[lpSessionData->GetSessionState()]);
		}
		else if(!SendPacket::UserKill(*lpSessionData))
		{
			ERRLOG3(g_Log, "UID:%10u / ServerID:0x%08X / ST:%s / UID ���� ���� ���� ���� ���� : ���� ���� ��Ŷ ������ ����", 
				lpPktUK->m_dwUserID, serverID.dwID, g_szSessionStateString[lpSessionData->GetSessionState()]);
		}
	}

    return true;
}


}


