#include "stdafx.h"
#include "SessionData.h"
#include "SessionDataMgr.h"

#include "StoreData.h"
#include "CharacterData.h"

#include "AdminDataMgr.h"

#include "StoreDataMgr.h"
#include "CharacterDataMgr.h"


#include <Network/Dispatch/LoginDispatch.h>
#include <Network/Dispatch/GameDispatch.h>
#include <Network/Dispatch/AuthDispatch.h>
#include <Network/Dispatch/UIDDispatch.h>

#include <Network/SendPacket/SendServerInfo.h>

#include <GameTime/GameTimeDBMgr.h>

#include <Community/Guild/GuildDB.h>
#include <Community/Guild/GuildDBMgr.h>

#include <DB/DBComponent.h>
#include <DB/GameDBComponent.h>
#include <Log/ServerLog.h>
#include <Network/Packet/PacketStruct/ServerInfo.h>

#include <Utility/Debug/PerformanceCheck.h>
#include <Utility/Setup/ServerSetup.h>

namespace DBAgent
{
namespace DataStorage
{

const char* g_szSessionStateString[CSessionData::SE_MAX_STATE] =
{
    "NONE",
    "USER_ENABLED",
    "USER_DISABLED",
    "MOVE",
    "CHAR_ENABLED",
    "CHAR_DISABLED"
};

RequestData::RequestData()
{
    m_dwRequestKey = m_dwSelectedCID = m_dwServerID = m_dwSessionID = 0;
    m_PeerAddress.S_un.S_addr = 0;
    
    memset(m_szAccount, 0, sizeof(char) * MaxName);
    memset(m_szPassword, 0, sizeof(char) * MaxPass);
}

RequestData::RequestData(unsigned long dwRequestKey, unsigned long dwSelectedCID, 
                         unsigned long dwServerID, unsigned long dwSessionID,
                         IN_ADDR peerAddr, const char* szAccountName, const char* szPassword)
                         :   m_dwRequestKey(dwRequestKey), m_dwSelectedCID(dwSelectedCID),
                             m_dwServerID(dwServerID), m_dwSessionID(dwSessionID),
                             m_PeerAddress(peerAddr)
{
    if(0 != szAccountName && strlen(szAccountName) > 0)
    {
        strcpy(m_szAccount, szAccountName);
    }
    else
    {
        memset(m_szAccount, 0, sizeof(char) * MaxName);
    }

    if(0 != szPassword && strlen(szPassword) > 0)
    {
        strcpy(m_szPassword, szPassword);
    }
    else
    {
        memset(m_szPassword, 0, sizeof(char) * MaxPass);
    }
}


CSessionData::CSessionData()
:   m_nUnifiedStoreInfoNum(0),
    m_nUnifiedCharDataNum(0),
    m_dwUID(0), 
    m_dwCID(0), 
    m_dwSessionID(0), 
    m_dwServerID(0),
    m_eSessionState(SE_NONE),
    m_dwFlags(0),
    m_lpCharacterData(0),
    m_lpStoreData(0),
    m_cAdminLevel(0),
    m_cOldServerGroupID(0),
    m_cTransferedCharCount(UCHAR_MAX),
    m_cNewZone(0), 
	m_dwPlayTime(0) 
{
    memset(&m_PremiumService, 0, sizeof(PREMIUMSERVICE));
    memset(&m_UserInfo, 0, sizeof(USER_INFO));
    memset(m_CharView, 0, sizeof(CHAR_VIEW) * USER_INFO::MAX_CHAR_NUM);
    memset(m_szAccount, 0, sizeof(char) * CHAR_INFOST::MAX_ACCOUNT_LEN);
    memset(&m_NewPos, 0, sizeof(POS));

    memset(m_UnifiedStoreInfo, 0, sizeof(UnifiedStoreInfo) * PktUnifiedCharInfo::MAX_STORE_INFO);
    memset(m_UnifiedCharData, 0, sizeof(UnifiedCharData) * PktUnifiedCharInfo::MAX_CHAR_DATA);

	memset(m_SaveEnemy, 0, sizeof(SaveEnemy)*SaveEnemy::MAX_SAVING_ENEMY);

    m_RemoteAddress.S_un.S_addr = 0;
}

CSessionData::~CSessionData()
{
	m_RequestQueue.clear();
}


bool CSessionData::GetCharView(unsigned long dwCID, CHAR_VIEW& charView_Out)
{
    for(unsigned int nCount = 0; nCount < USER_INFO::MAX_CHAR_NUM; ++nCount)
    {
        if(m_CharView[nCount].CID == dwCID)
        {
            charView_Out = m_CharView[nCount];
            return true;
        }
    }

    return false;
}

// WORK_LIST 2.3 ���� ���� ���� ��� ����
bool CSessionData::SetCharViewGID(unsigned long dwCID, unsigned long dwGID)
{
	for(unsigned int nCount = 0; nCount < USER_INFO::MAX_CHAR_NUM; ++nCount)
	{
		if(m_CharView[nCount].CID == dwCID)
		{
			m_CharView[nCount].GID = dwGID;
			return true;
		}
	}

	return false;
}

bool CSessionData::SetCharViewFame(unsigned long dwCID, unsigned long dwFame)
{
	for(unsigned int nCount = 0; nCount < USER_INFO::MAX_CHAR_NUM; ++nCount)
	{
		if(m_CharView[nCount].CID == dwCID)
		{
			m_CharView[nCount].Fame = dwFame;
			return true;
		}
	}

	return false;
}

bool CSessionData::HasCharacter(unsigned long dwCID) const 
{ 
    if(0 != dwCID)
    {
        for (int nCount = 0; nCount < USER_INFO::MAX_CHAR_NUM; ++nCount)
        {
            if (dwCID == m_UserInfo.CharID[nCount])
            {
                return true;
            }
        }
    }

    return false;
}

bool CSessionData::GetPlayTimeFromDB(CDBComponent& DBComponent)
{
	if (GameRYL::CHINA != CServerSetup::GetInstance().GetNationType())
	{
		m_dwPlayTime = 0;
		return true;
	}

	using namespace DBComponent;

    if (!GameDB::GetPlayTime(DBComponent, m_dwUID, &m_dwPlayTime)) 
    {
        SERLOG1(g_Log, "UID:%10u / ĳ���� ������ �ε� ���� : GetPlayTime ȣ�� ����", m_dwUID);
		return false;
    }

	return true;
}

bool CSessionData::GetPremiumServiceFromDB(CDBComponent& DBComponent)
{
    memset(&m_PremiumService, 0, sizeof(PREMIUMSERVICE));

	using namespace DBComponent;

    if (!GameDB::GetPremiumService(DBComponent, m_dwUID, m_PremiumService)) 
    {
//      SERLOG1(g_Log, "UID:%10u / �����̾� ���� �ƴ�", m_dwUID);
		return false;
    }

	if(m_PremiumService.lPremiumTime == 0)
		m_PremiumService.iPremiumType = 0;

	return true;
}

bool CSessionData::GetUserInfoFromDB(CDBComponent& DBComponent)
{
    PERFORMANCE_CHECK(FunctionTimingCheck);

    memset(&m_UserInfo, 0, sizeof(USER_INFO));
    memset(m_CharView, 0, sizeof(CHAR_VIEW) * USER_INFO::MAX_CHAR_NUM);

    using namespace DBComponent;

    // ���� ���� ���
    bool bFirstLoadUserInfoResult = 
        GameDB::GetUserInfo(DBComponent, m_dwUID, m_UserInfo, m_cOldServerGroupID, true);

    if (!bFirstLoadUserInfoResult) 
    {
        if(!GameDB::InsertUser(DBComponent, m_dwUID, m_cOldServerGroupID))
        {
            SERLOG3(g_Log, "UID:%10u / ServerGroupID:%d / ���� ���� ���� : %s",
                m_dwUID, m_cOldServerGroupID, DBComponent.GetErrorString());

            return false;
        }        
    }

    if (!bFirstLoadUserInfoResult && 
        !GameDB::GetUserInfo(DBComponent, m_dwUID, m_UserInfo, m_cOldServerGroupID)) 
    {
        SERLOG3(g_Log, "UID:%10u / ServerGroupID:%d / ���� ���� ��� ���� : %s",
            m_dwUID, m_cOldServerGroupID, DBComponent.GetErrorString());        
    }
    else
    {
        bool bGetViewResult = true;

        for (int nCount = 0; nCount < USER_INFO::MAX_CHAR_NUM; ++nCount)
        {
            if (0 != m_UserInfo.CharID[nCount] && 
                !GameDB::GetCharView(DBComponent, m_UserInfo.CharID[nCount], &m_CharView[nCount]))
            {
                SERLOG5(g_Log, "UID:%10u / ServerGroupID:%d / CID:%10u / Slot:%d / ĳ���� �� ��� ���� : %s", 
                    m_dwUID, m_cOldServerGroupID, m_UserInfo.CharID[nCount], nCount, DBComponent.GetErrorString());
/*
				// edith 2010.01.03 ���ʷ� ��Ʈ��ũ�� DB������ ���������� GetCharView�� �����ϴ�
				// GameDB�� UserInfo�� CID�� 0���� �ʱ�ȭ �غ��� ���߿� DB�� CID�� 0�� ������ ������Ʈ �Ǿ�
				// ĳ���Ͱ� �����Ǵ� ���װ� �ƴ�.

				m_UserInfo.CharID[nCount] = 0;
				if (!GameDB::UpdateUser(DBComponent, m_dwUID, &m_UserInfo))
				{
					bGetViewResult = false;
				}
*/
					bGetViewResult = false;
            }
        }

        if(bGetViewResult)
        {
            INFLOG2(g_Log, "UID:%10u / ServerGroupID:%d / ���� ���� ��� ����", 
                m_dwUID, m_cOldServerGroupID);
            return true;
        }
    }

    return false;
}

bool CSessionData::GetUnifiedInfo(CDBComponent& DBComponent, unsigned long UserID_In)
{
	using namespace DBComponent;

	memset(m_UnifiedStoreInfo, 0, sizeof(UnifiedStoreInfo) * PktUnifiedCharInfo::MAX_STORE_INFO);

	m_nUnifiedStoreInfoNum = PktUnifiedCharInfo::MAX_STORE_INFO;

	return GameDB::GetUnifiedItemStoreInfo(DBComponent, UserID_In, m_UnifiedStoreInfo, m_nUnifiedStoreInfoNum);		
}

bool CSessionData::GetUnifiedDataFromDB(CDBComponent& DBComponent)
{
    // UnifiedStoreInfo / UnifiedCharData �� �о ���ǿ� �����Ѵ�.
    memset(m_UnifiedStoreInfo, 0, sizeof(UnifiedStoreInfo) * PktUnifiedCharInfo::MAX_STORE_INFO);
    memset(m_UnifiedCharData, 0, sizeof(UnifiedCharData) * PktUnifiedCharInfo::MAX_CHAR_DATA);

    m_nUnifiedStoreInfoNum = PktUnifiedCharInfo::MAX_STORE_INFO;
    m_nUnifiedCharDataNum = PktUnifiedCharInfo::MAX_CHAR_DATA;

    using namespace DBComponent;

    // ���� ���� �б�.
    if(!GameDB::GetUnifiedItemStoreInfo(DBComponent, m_dwUID, m_UnifiedStoreInfo, m_nUnifiedStoreInfoNum))
    {
        ERRLOG2(g_Log, "UID:%10u / ���� â�� ���� �о���� ���� : %s", m_dwUID, DBComponent.GetErrorString());
    }
    else if(!GameDB::GetUnifiedCharList(DBComponent, m_dwUID, m_UnifiedCharData, m_nUnifiedCharDataNum))
    {
        ERRLOG2(g_Log, "UID:%10u / ���� ĳ���� ���� �о���� ���� : %s", m_dwUID, DBComponent.GetErrorString());    
    }
    else
    {
        unsigned char cAgentServerType = static_cast<unsigned char>(
            CServerSetup::GetInstance().GetAgentServerType());

        bool bHasUnifiedStoreInfo = HasUnifiedStoreInfo(cAgentServerType);

        switch(cAgentServerType)
        {
        case UnifiedConst::Part2Unified:
            
            m_cOldServerGroupID = UnifiedConst::Part2Unified;
            
            if(!GetUserInfoFromDB(DBComponent))
            {
                ERRLOG2(g_Log, "UID:%10u / ���� ĳ���� ���� ���� ���� : %s", 
                    m_dwUID, DBComponent.GetErrorString());
            }
            else
            {
                unsigned char cAgentServerType = 
                    static_cast<unsigned char>(CServerSetup::GetInstance().GetAgentServerType());

                if (m_UserInfo.FirstLogin & UnifiedConst::SELECTED_PART2)
                {
                    // â�� �̹� ������ ���� â�� ������ �� �ʿ䰡 ����.
                    memset(m_UnifiedStoreInfo, 0, sizeof(UnifiedStoreInfo) * PktUnifiedCharInfo::MAX_STORE_INFO);
                    m_nUnifiedStoreInfoNum = 0;
                }
                else
                {
                    // �����׷� ID�� ���� �߰輭��Ÿ�Ժ��� ���� ���� �߸���.
                    UnifiedStoreInfo* lpStorePos = m_UnifiedStoreInfo;
                    UnifiedStoreInfo* lpStoreEnd = m_UnifiedStoreInfo + m_nUnifiedStoreInfoNum;

                    for(; lpStorePos < lpStoreEnd; )
                    {
                        // ���� �������� â��� ���� �� �ʿ� ����.
                        if(cAgentServerType <= lpStorePos->cOldServerGroupID)
                        {
                            memmove(lpStorePos, lpStorePos + 1, sizeof(UnifiedStoreInfo) * (lpStoreEnd - lpStorePos - 1));
                            --m_nUnifiedStoreInfoNum;

                            lpStoreEnd = m_UnifiedStoreInfo + m_nUnifiedStoreInfoNum;
                        }
                        else
                        {
                            ++lpStorePos;
                        }
                    }

                    // â�� ���ų�, â�� �־ �ڱ� â�� �ִ� ��쿡��, 
                    // â�� �̹� ������ �� ó�� �� �ش�.
                    if (0 == m_nUnifiedStoreInfoNum ||
                       (1 == m_nUnifiedStoreInfoNum && m_UnifiedStoreInfo->cOldServerGroupID == cAgentServerType))
                    {
                        m_UserInfo.FirstLogin |= UnifiedConst::SELECTED_PART2;
                        DBComponent::GameDB::UpdateUserFirstLogin(DBComponent, m_dwUID, m_UserInfo.FirstLogin);
                    }
                }
                
                // �����׷� ID�� ���� �߰輭��Ÿ�Ժ��� ���� ���� �߸���.
                UnifiedCharData* lpCharPos = m_UnifiedCharData;
                UnifiedCharData* lpCharEnd = m_UnifiedCharData+ m_nUnifiedCharDataNum;

                for(; lpCharPos < lpCharEnd; )
                {
                    // ���� �������� ĳ���ʹ� ���� ��� �Ѵ�.
                    if(cAgentServerType < lpCharPos->cOldServerGroupID)
                    {
                        memmove(lpCharPos, lpCharPos + 1, sizeof(UnifiedStoreInfo) * (lpCharEnd - lpCharPos - 1));
                        --m_nUnifiedCharDataNum;

                        lpCharEnd = m_UnifiedCharData+ m_nUnifiedCharDataNum;
                    }
                    else
                    {
                        ++lpCharPos;
                    }
                }

                INFLOG4(g_Log, "UID:%10u / FirstLogin:%d / ReadStoreInfo:%d / ReadCharData:%d / �������� ������ �дµ� �����߽��ϴ�", 
                    m_dwUID, m_UserInfo.FirstLogin, m_nUnifiedStoreInfoNum, m_nUnifiedCharDataNum);

                return true;
            }
            break;
        
        case UnifiedConst::Part2Selectable:
                                            
            if (0 == m_nUnifiedStoreInfoNum || 
                (1 == m_nUnifiedStoreInfoNum && bHasUnifiedStoreInfo))
            {
                m_cOldServerGroupID = UnifiedConst::Part2Selectable;

                memset(m_UnifiedStoreInfo, 0, sizeof(UnifiedStoreInfo) * PktUnifiedCharInfo::MAX_STORE_INFO);
                m_nUnifiedStoreInfoNum = 0;
            }
            else
            {
                m_cOldServerGroupID = 1;
            }

            INFLOG3(g_Log, "UID:%10u / ReadStoreInfo:%d / ReadCharData:%d / �������� ������ �дµ� �����߽��ϴ�",
                m_dwUID, m_nUnifiedStoreInfoNum, m_nUnifiedCharDataNum);

            return true;            
        }
    }
    
    m_nUnifiedStoreInfoNum = 0;
    m_nUnifiedCharDataNum = 0;

    m_cOldServerGroupID = 0;    
    return false;
}

void CSessionData::SetRequestData(const RequestData& reqData)
{
    m_dwSessionID   = reqData.m_dwSessionID;
    m_dwServerID    = reqData.m_dwServerID;
    m_RemoteAddress = reqData.m_PeerAddress;

    strcpy(m_szAccount, reqData.m_szAccount);
}


bool CSessionData::UserEnable(CDBComponent& DBComponent, unsigned long dwUID)
{
    if (SE_USER_ENABLED == m_eSessionState || SE_CHAR_ENABLED == m_eSessionState)
    {
        LogUserDataError("���� Ȱ��ȭ �õ� ����", LOG_FFL);
    }
    // DB���� ���� ���� �� �並 ���´�.
    else if (GetUserInfoFromDB(DBComponent))
    {
        // ���� Ȱ��ȭ.
        SetSessionState(SE_USER_ENABLED);
        INFLOG1(g_Log, "UID:%10u / ���� Ȱ��ȭ", m_dwUID);

        // ��� ���� ����.
        m_cAdminLevel = CAdminDataMgr::GetInstance().GetAdminLevel(m_dwUID, m_RemoteAddress);

        // ���� �� �ø�.
        GET_SINGLE_DISPATCH(lpAuthDispatch, 
            CAuthDispatch, CAuthDispatch::GetDispatchTable());

        if(0 != lpAuthDispatch)
        {
            lpAuthDispatch->IncUserNum();
        }
        
        return true;
    }

    return false;
}

bool CSessionData::UserDisable()
{
    if (SE_USER_ENABLED == m_eSessionState || SE_MOVE == m_eSessionState)
    {
        // ���� ��Ȱ��ȭ ���� ��ȯ
        SetSessionState(SE_USER_DISABLED);

        // ���� ��Ȱ��ȭ ����
        SendPacket::UpdateUIDTable(PktUUT::UpdateUIDTableUserLogout,
            m_szAccount, NULL, m_dwUID, m_dwCID, m_dwSessionID, m_dwServerID, m_RemoteAddress);

        // ���� �� ����
        GET_SINGLE_DISPATCH(lpAuthDispatch, 
            CAuthDispatch, CAuthDispatch::GetDispatchTable());

        if(0 != lpAuthDispatch)
        {
            lpAuthDispatch->DecUserNum();
        }

        // Ȥ�� �����Ͱ� �ε�Ǿ� ������ ��ε��Ѵ�.
        UnloadData();
        GetBilling().ClearBilling();
    }
    else
    {
        LogUserDataError("���� ��Ȱ��ȭ �õ� ����", LOG_FFL);
        return false;
    }
    
    return true;
}

bool CSessionData::UserMove()
{
    if (SE_USER_ENABLED == m_eSessionState)
    {
        // ĳ���� �̵�
        SetSessionState(SE_MOVE);
        SendPacket::UpdateUIDTable(PktUUT::UpdateUIDTableUserMove,
            m_szAccount, NULL, m_dwUID, m_dwCID, m_dwSessionID, m_dwServerID, m_RemoteAddress);

        // ���� �� ����
        GET_SINGLE_DISPATCH(lpAuthDispatch, 
            CAuthDispatch, CAuthDispatch::GetDispatchTable());

        if(0 != lpAuthDispatch)
        {
            lpAuthDispatch->DecUserNum();
        }

        UnloadData();        
    }
    else
    {
        LogUserDataError("���� �̵� �õ� ����", LOG_FFL);
        return false;
    }

    return true;
}

bool CSessionData::LoadData(unsigned long dwCID)
{
    DBDataError eStoreDataError     = DATA_SUCCEEDED;
    DBDataError eCharacterDataError = DATA_SUCCEEDED;

    // â�� ������ �ε� (â�� ����ų�, �ٸ� UID�����͸� ���� ��쿡�� �ε�)
    if(0 == m_lpStoreData || m_dwUID != m_lpStoreData->GetUID())
    {            
        if(0 != m_lpStoreData)
        {
            // ������ �ݳ�
            CStoreDataMgr::GetInstance().UnloadData(*this, m_lpStoreData);
            m_lpStoreData = 0;
        }

        // ������ �ε�
        eStoreDataError = CStoreDataMgr::GetInstance().LoadData(*this, 
            UnifiedStoreKey(m_dwUID, m_cOldServerGroupID), m_lpStoreData);
    }

    // ĳ���� ������ �ε� (ĳ���� �����Ͱ� ����ų�, �ٸ� ĳ���� �����͸� ���� ��쿡�� �ε�)
    if(0 == m_lpCharacterData || dwCID != m_lpCharacterData->GetCID())
    {
        if(0 != m_lpCharacterData)
        {
            // ������ �ݳ�
            CCharacterDataMgr::GetInstance().UnloadData(*this, m_lpCharacterData);
            m_lpCharacterData = 0;
        }

	    // ������ �ε�
        eCharacterDataError = 
            CCharacterDataMgr::GetInstance().LoadData(*this, dwCID, m_lpCharacterData);        
    }

    // ��� ó��
    if (eStoreDataError != DATA_SUCCEEDED || 
        eCharacterDataError != DATA_SUCCEEDED)
    {
        // ����ó��. �����͸� Unload�ϰ� �α׸� ��´�.
        if(0 != m_lpStoreData && eStoreDataError == DATA_SUCCEEDED)
        {
            CStoreDataMgr::GetInstance().UnloadData(*this, m_lpStoreData);
            m_lpStoreData = 0;
        }

        if(0 != m_lpCharacterData && eCharacterDataError == DATA_SUCCEEDED)
        {
            CCharacterDataMgr::GetInstance().UnloadData(*this, m_lpCharacterData);
            m_lpCharacterData = 0;
        }

        ERRLOG5(g_Log, "UID:%10u / ServerGroupID:%d / CID:%10u / Store:%s / Character:%s / â��, ĳ���� ������ �ε� ����.", 
            m_dwUID, m_cOldServerGroupID, dwCID,
            g_szDBDataErrorString[eStoreDataError],
            g_szDBDataErrorString[eCharacterDataError]);

        return false;
    }

    m_dwCID = dwCID;
    return true;
}

void CSessionData::UnloadData()
{
    if(0 != m_lpStoreData)
    {
        CStoreDataMgr::GetInstance().UnloadData(*this, m_lpStoreData);
        m_lpStoreData = 0;
    }

    if(0 != m_lpCharacterData)    
    {
        CCharacterDataMgr::GetInstance().UnloadData(*this, m_lpCharacterData);
        m_lpCharacterData = 0;
    }
    
    m_dwCID = 0;	
}

bool CSessionData::CharEnable(unsigned long dwCID, unsigned long dwServerID)
{
    if (SE_USER_ENABLED != m_eSessionState && SE_CHAR_ENABLED != m_eSessionState)
    {
        if(LoadData(dwCID))
        {
            // �̵� �� ������Ʈ
            UpdateMoveZone();

			// edith 2008.12.4 �α��� Ÿ�Ӱ��ű�� �߰�
			// ĳ���Ͱ� Enable �� ���¸� ���缭���� �α��ε� �ð��� ��Ÿ����.
			CTime timeLogin = CTime::GetCurrentTime();
			TIME* pLoginTime = m_lpCharacterData->GetLoginTime();
			if(pLoginTime)
			{
				pLoginTime->Year		= timeLogin.GetYear();
				pLoginTime->Month		= timeLogin.GetMonth();
				pLoginTime->Day			= timeLogin.GetDay();			
				pLoginTime->Hour		= timeLogin.GetHour();
				pLoginTime->Minute		= timeLogin.GetMinute();
				pLoginTime->Second		= timeLogin.GetSecond();
			}

            // ��Ÿ ���� ���� (������ ���� �� �Լ� ȣ�� �ʿ�)
            m_dwServerID = dwServerID;
            m_lpCharacterData->SetServerID(dwServerID);

            // ĳ���� Ȱ��ȭ
            SetSessionState(SE_CHAR_ENABLED);
            LogCharData("ĳ���� Ȱ��ȭ");
            
            GET_MULTI_DISPATCH(lpGameDispatch, dwServerID, 
                CGameDispatch, CGameDispatch::GetDispatchTable());

            if(0 != lpGameDispatch)
            {
                lpGameDispatch->IncCharNum(m_lpCharacterData->GetRace());
            }

            return true;
        }
    }
    else
    {
        SERLOG4(g_Log, "UID:%10u / CID:%10u / ServerID:0x%08X / ST:%s / ĳ���� Ȱ��ȭ �õ� ���� : "
            "���� ���°� �̻��մϴ�.", m_dwUID, dwCID, dwServerID, g_szSessionStateString[m_eSessionState]);
    }

    return false;
}

bool CSessionData::CharDisable(unsigned long dwCID, unsigned long dwServerID)
{
    if (SE_CHAR_ENABLED == m_eSessionState)
    {        
        if (m_dwCID != dwCID || m_dwServerID != dwServerID)
        {
            SERLOG5(g_Log, "UID:%10u / CID:%10u / ServerID:0x%08X / DisableCID:%10u / DisableServerID:0x%08X / "
                "ĳ���� ��Ȱ��ȭ ���� : �ٸ� ĳ���ͷ�, Ȥ�� �ٸ� �������� ��Ȱ��ȭ �õ�",
                m_dwUID, m_dwCID, m_dwServerID, dwCID, dwServerID);
        }
        else
        {
            int nRace = (0 != m_lpCharacterData) ? m_lpCharacterData->GetRace() : -1;

            // ĳ���� ��Ȱ��ȭ ����
            SendPacket::UpdateUIDTable(PktUUT::UpdateUIDTableCharLogout,
                m_szAccount, NULL, m_dwUID, m_dwCID, m_dwSessionID, m_dwServerID, m_RemoteAddress);
            
            // ������ �ݳ�. �����ϴ� �����ϴ�, �������� �Ҿ������ �ȴ�.
            UnloadData();

            // ĳ���� ��Ȱ��ȭ (�Լ� ȣ�� �Ŀ� ������ ����)
            SetSessionState(SE_CHAR_DISABLED);

            GET_MULTI_DISPATCH(lpGameDispatch, dwServerID, 
                CGameDispatch, CGameDispatch::GetDispatchTable());

            if(0 != lpGameDispatch)
            {
                lpGameDispatch->DecCharNum(nRace);
            }

            // ���� ����! �� �������� 0���� Ŭ����!
            m_dwCID = 0;
            m_dwServerID = 0;
            m_cNewZone = 0;
            GetBilling().ClearBilling();
            return true;
        }
    }
    else
    {
        LogCharDataError("ĳ���� ��Ȱ��ȭ �õ� ����", LOG_FFL);
    }
    
    return false;
}

bool CSessionData::CharMove(unsigned long dwCID, unsigned long dwServerID)
{
    if (SE_CHAR_ENABLED == m_eSessionState)
    {
        if (m_dwCID != dwCID || m_dwServerID != dwServerID)
        {
            SERLOG5(g_Log, "UID:%10u / CID:%10u / ServerID:0x%08X / DisableCID:%10u / DisableServerID:0x%08X / "
                "ĳ���� �̵� ���� : �ٸ� ĳ���ͷ�, Ȥ�� �ٸ� �������� ��Ȱ��ȭ �õ�",
                m_dwUID, m_dwCID, m_dwServerID, dwCID, dwServerID);
        }
        else
        {
            int nRace = (0 != m_lpCharacterData) ? m_lpCharacterData->GetRace() : -1;

            // ĳ���� �̵� ����
            SendPacket::UpdateUIDTable(PktUUT::UpdateUIDTableCharLogout,
                m_szAccount, NULL, m_dwUID, m_dwCID, m_dwSessionID, m_dwServerID, m_RemoteAddress);

            LogCharData("ĳ���� �̵�");

            // ������ �ݳ�. �����ϴ� �����ϴ�, �������� �Ҿ������ �ȴ�.
            UnloadData();

            // ĳ���� �̵�(�Լ� ȣ�� �ڿ� ������ ����
            SetSessionState(SE_MOVE);

            GET_MULTI_DISPATCH(lpGameDispatch, dwServerID, 
                CGameDispatch, CGameDispatch::GetDispatchTable());

            if(0 != lpGameDispatch)
            {
                lpGameDispatch->DecCharNum(nRace);
            }

            // ���� ����! �� �������� 0���� Ŭ����!
            m_dwCID = 0;
            m_dwServerID = 0;
            GetBilling().ClearBilling();
            return true;
        }
    }
    else
    {
        LogCharDataError("ĳ���� �̵� �õ� ����", LOG_FFL);
    }

    return false;
}


// ---------------------------------------------------------------------------
// �� �̵� ���� �κ�

// �� �̵� ����
bool CSessionData::SetMoveZone(unsigned char cZone, const POS& Pos)		
{	
    if (!CheckCharZone(cZone))
    {
        return false;
    }

    m_cNewZone = cZone;	
    m_NewPos = Pos;		

    return true;
}

// �� �̵� ������Ʈ
void CSessionData::UpdateMoveZone()
{	
    if (0 != m_cNewZone && 0 != m_lpCharacterData)
    {
        CHAR_POS charPos;

        charPos.LastPoint = m_NewPos;
        charPos.SavePoint = m_NewPos;

        m_lpCharacterData->SetPos(charPos);
    }

    m_cNewZone = 0;
}

// ĳ���� �� üũ
bool CSessionData::CheckCharZone(unsigned char cZone)
{
	if(this->m_cAdminLevel)
	{
		return true;
	}

	bool bResult = false;

	// edith 2009.07.30 �������� �������� ���� �����Ѵ�.
	int nRace = (0 != m_lpCharacterData) ? m_lpCharacterData->GetRace() : -1;
	if(nRace == CClass::HUMAN)
	{
		if(cZone == SERVER_ID::ZONE2 || cZone == SERVER_ID::ZONE5)
			return true;//was false
	}
	else if(nRace == CClass::AKHAN)
	{
		if(cZone == SERVER_ID::ZONE1 || cZone == SERVER_ID::ZONE4)
			return true;//was false
	} // hayzohar wasnt commented to fix zone move bug

	if (cZone == SERVER_ID::ZONE1 || cZone == SERVER_ID::ZONE2 || cZone == SERVER_ID::ZONE4 || cZone == SERVER_ID::ZONE5)
	{
		// ��Ʈ1���� 2�� �߰��� �⺻����
		bResult = true;
	}
	else if (cZone == SERVER_ID::CAPITAL || cZone == SERVER_ID::ZONE12 || cZone == SERVER_ID::PRISON)
	{
		// ��Ʈ2���� ����ߴ� ����
		bResult = true;
	}
	else if (cZone == /*SERVER_ID::STONE_WAR1*/SERVER_ID::CAPITAL || cZone == SERVER_ID::STONE_WAR2 || cZone == SERVER_ID::STONE_WAR3)
	{
		// ��ũī���� ����
		//if (true == CServerSetup::GetInstance().UseContents(GameRYL::STONE_BATTLE))//hayzohar statue war fix
		//{
			bResult = true;
		//}
	}
	else if (cZone == SERVER_ID::ZONE9)
		bResult = true;

	

/*
	else if (cZone == SERVER_ID::ZONE9)
	{
		// �ű��� ����
		if (true == CServerSetup::GetInstance().UseContents(GameRYL::NEWZONE_ZONE9))
		{
			bResult = true;
		}
	}
*/

	return bResult;
}

// edith 2008.02.28 ĳ���� ���� ��
unsigned char CSessionData::GetCharStartZone(char cRace)
{
	/*if(cRace == CHAR_CREATE::HUMAN)
		return SERVER_ID::ZONE12;*/


//	return SERVER_ID::ZONE1;
	return SERVER_ID::CAPITAL;
}

void CSessionData::LogUserData(const char* szMessage)
{
    RULLOG5(g_Log, "UID:%10u / ServerID:0x%08X / ST:%s / IP:%15s / %s",
        m_dwUID, m_dwServerID, g_szSessionStateString[m_eSessionState],
        inet_ntoa(m_RemoteAddress), szMessage);
}

void CSessionData::LogCharData(const char* szMessage)
{
    if(0 != m_lpCharacterData)
    {
        RULLOG9(g_Log, "UID:%10u / CID:%10u / ServerID:0x%08X / ST:%s / "
            "IP:%15s / DataCID:%10u / Name:%-16s / Lev:%3d / %s",
            m_dwUID, m_dwCID, m_dwServerID, g_szSessionStateString[m_eSessionState], 
            inet_ntoa(m_RemoteAddress), m_lpCharacterData->GetCID(), 
            m_lpCharacterData->GetName(), m_lpCharacterData->GetInfo().Level, szMessage);
    }
    else
    {
        LogUserData(szMessage);
    }
}

void CSessionData::LogUserDataError(const char* szMessage, const char* szRoutine, 
                                    const char* szFile, int nLine)
{
    g_Log.DetailLog(LOG_ERROR, szRoutine, szFile, nLine, 
        "UID:%10u / ServerID:0x%08X / ST:%s / IP:%15s / %s",
        m_dwUID, m_dwServerID, g_szSessionStateString[m_eSessionState],
        inet_ntoa(m_RemoteAddress), szMessage);
}

void CSessionData::LogCharDataError(const char* szMessage, const char* szRoutine, 
                                    const char* szFile, int nLine)
{
    if(0 != m_lpCharacterData)
    {
        g_Log.DetailLog(LOG_ERROR, szRoutine, szFile, nLine, 
            "UID:%10u / CID:%10u / ServerID:0x%08X / ST:%s / "
            "IP:%15s / DataCID:%10u / Name:%-16s / Lev:%3d / %s",
            m_dwUID, m_dwCID, m_dwServerID, g_szSessionStateString[m_eSessionState], 
            inet_ntoa(m_RemoteAddress), m_lpCharacterData->GetCID(), 
            m_lpCharacterData->GetName(), m_lpCharacterData->GetInfo().Level, szMessage);
    }
    else
    {
        LogUserDataError(szMessage, szRoutine, szFile, nLine);
    }
}

//! ��û ť ���� (dwRequestKey�� 0�̸� ����)
void CSessionData::PushRequest(const RequestData& reqData)
{
    m_RequestQueue.push_back(reqData);
}

RequestData CSessionData::PopRequest()
{
    RequestData data;

    if(m_RequestQueue.empty())
    {
        data.m_dwRequestKey = 0;
        data.m_dwServerID = 0;
        data.m_dwSelectedCID = 0;
    }
    else
    {
        data = m_RequestQueue.front();
        m_RequestQueue.pop_front();
    }    

    return data;
}

const UnifiedCharData* CSessionData::GetUnifiedCharData(unsigned long dwCID) const
{
    const UnifiedCharData* lpPos = m_UnifiedCharData;
    const UnifiedCharData* lpEnd = m_UnifiedCharData + m_nUnifiedCharDataNum;

    for(; lpPos != lpEnd; ++lpPos)
    {
        if (lpPos->dwNewCID == dwCID)
        {
            return lpPos;
        }
    }

    return 0;
}

bool CSessionData::HasUnifiedStoreInfo(unsigned char cOldServerGroupID)
{
    const UnifiedStoreInfo* lpPos = m_UnifiedStoreInfo;
    const UnifiedStoreInfo* lpEnd = m_UnifiedStoreInfo + m_nUnifiedStoreInfoNum;

    for(; lpPos != lpEnd; ++lpPos)
    {
        if(lpPos->cOldServerGroupID == cOldServerGroupID)
        {
            return true;
        }
    }

    return false;
}

bool CSessionData::ChangeUnifiedStoreInfoGroup(CDBComponent& DBComponent, 
                                               unsigned char cOldServerGroupID, 
                                               unsigned char cNewServerGroupID)
{
    CStoreDataMgr&  storeDataMgr = CStoreDataMgr::GetInstance();
    UnifiedStoreKey storeKey(m_dwUID, cOldServerGroupID);    

    if (HasUnifiedStoreInfo(cOldServerGroupID) &&
        !storeDataMgr.IsDataLoginDB(storeKey) &&
        !storeDataMgr.IsDataLogoutDB(storeKey))
    {
        // â�� ������ �� ����, �����ϴ� â���̰�, ĳ������ ���� ����ϰ� �ִ°��� 
        // Ȯ���� ����, ������ �����Ѵ�.
       
        // cOldServerGroupID�� â�� �����͸� cChangedServerGroupID�� ���� �����,
        // cOldServerGroupID�� â�� �����͸� NULL�� �����.
        if(!DBComponent::GameDB::ChangeUnifiedItemStoreGroup(
            DBComponent, storeKey.first, cOldServerGroupID, cNewServerGroupID))
        {
            // â�� �׷� ��ȣ ���� ����
            ERRLOG2(g_Log, "UID:%10u / SelectedServerGroupID:%d / ���ռ��� ���� ���� : â�� �׷� ��ȣ ���� ����",
                storeKey.first, cOldServerGroupID);            
        }
        else
        {
            // 3. �޸��� UnifiedStoreInfo�� ���缭 �����Ѵ�.
            UnifiedStoreInfo* lpPos = m_UnifiedStoreInfo;
            UnifiedStoreInfo* lpEnd = m_UnifiedStoreInfo + m_nUnifiedStoreInfoNum;

            for(; lpPos != lpEnd; ++lpPos)
            {
                if (lpPos->cOldServerGroupID == cOldServerGroupID)
                {
                    lpPos->cOldServerGroupID = cNewServerGroupID;
                    break;
                }
            }

            return true;
        }
    }
    else
    {
        ERRLOG2(g_Log, "UID:%10u / SelectedServerGroupID:%d / ���ռ��� ���� ���� : �߸��� �����׷� ����",
            storeKey.first, storeKey.second);
    }

    return false;
}

bool CSessionData::ChangeUnifiedCharServerGroupID(CDBComponent& DBComponent, unsigned long dwUID, 
                                                  unsigned long dwCID, unsigned char cOldServerGroupID)
{
    if (0 != GetUnifiedCharData(dwCID))
    {
        if (!DBComponent::GameDB::UpdateUnifiedCharServerGroupID(
            DBComponent, dwUID, dwCID, cOldServerGroupID))
        {
            // ���� �α� ����
            ERRLOG3(g_Log, "UID:%10u / CID:%10u / ServerGroupID:%d / ���ռ��� ���� ���� : ĳ���� �����׷� ID���� ����",
                dwUID, dwCID, cOldServerGroupID);
        }
        else
        {
            UnifiedCharData* lpPos = m_UnifiedCharData;
            UnifiedCharData* lpEnd = m_UnifiedCharData + m_nUnifiedCharDataNum;

            for(; lpPos != lpEnd; ++lpPos)
            {
                if (lpPos->dwNewCID == dwCID)
                {
                    // �޸� �����͸� �����Ѵ�.
                    lpPos->cOldServerGroupID = cOldServerGroupID;
                    break;
                }
            } 

            return true;
        }
    }
    else
    {
        ERRLOG3(g_Log, "UID:%10u / CID:%10u / ServerGroupID:%d / ���ռ��� ���� ���� : �߸��� ĳ���� ����",
            dwUID, dwCID, cOldServerGroupID);
    }

    return false;
}


bool CSessionData::DelUnifiedCharData(unsigned long dwCID)
{
    UnifiedCharData* lpPos = m_UnifiedCharData;
    UnifiedCharData* lpEnd = m_UnifiedCharData + m_nUnifiedCharDataNum;

    for(; lpPos != lpEnd; ++lpPos)
    {
        if (lpPos->dwNewCID == dwCID)
        {
            memmove(lpPos, lpPos + 1, lpEnd - lpPos - 1);
            return true;
        }
    }

    return false;
}

bool CSessionData::UpdateCharacterBinding(CDBComponent& DBComponent, 
                                          unsigned long dwSelectedCID[USER_INFO::MAX_CHAR_NUM])
{
    // UserInfo�� �����Ѵ�.
    USER_INFO userInfo = m_UserInfo;

    unsigned char cSelectedCount = 0;

    for(int nCount = 0; nCount < USER_INFO::MAX_CHAR_NUM; ++nCount)
    {
        if(0 == userInfo.CharID[nCount] && 0 != dwSelectedCID[nCount])
        {
            userInfo.CharID[nCount] = dwSelectedCID[nCount];
            ++cSelectedCount;
        }
    }

    // UserInfo�� DB�� ������Ʈ�Ѵ�.
    if (0 != memcmp(&m_UserInfo, &userInfo, sizeof(USER_INFO)))
    {
        if (!DBComponent::GameDB::UpdateUser(DBComponent, m_dwUID, &userInfo))
        {
            // DB���� ����!
            return false;
        }
        else
        {
            m_UserInfo = userInfo;
        }
    }

    // ĳ���� ���� ȸ���� DB�� ����Ѵ�.
    m_cTransferedCharCount += cSelectedCount;
    UpdateTransferedCharCount(DBComponent);    

    // ������ ĳ���� �並 �ٽ� ���´�.
    bool bGetViewResult = true;

    for (int nCount = 0; nCount < USER_INFO::MAX_CHAR_NUM; ++nCount)
    {
        if (0 != m_UserInfo.CharID[nCount] && 
            !DBComponent::GameDB::GetCharView(DBComponent, m_UserInfo.CharID[nCount], &m_CharView[nCount]))
        {
            SERLOG5(g_Log, "UID:%10u / ServerGroupID:%d / CID:%10u / Slot:%d / ĳ���� �� ��� ���� : %s", 
                m_dwUID, m_cOldServerGroupID, m_UserInfo.CharID[nCount], nCount, DBComponent.GetErrorString());

            bGetViewResult = false;
        }
    }

    if(bGetViewResult)
    {
        INFLOG2(g_Log, "UID:%10u / ServerGroupID:%d / ���� ���� ��� ����", 
            m_dwUID, m_cOldServerGroupID);
        return true;
    }

    return false;
}

CClass::RaceType CSessionData::CheckUnifiedCharRace(unsigned long dwSelectedCID[USER_INFO::MAX_CHAR_NUM])
{
    CClass::RaceType eLastRace = CClass::MAX_RACE;
    CClass::RaceType eCharRace = CClass::MAX_RACE;

    unsigned long dwCheckCID[USER_INFO::MAX_CHAR_NUM];
    memset(dwCheckCID, 0, sizeof(unsigned long) * USER_INFO::MAX_CHAR_NUM);

    for(int nCount = 0; nCount < USER_INFO::MAX_CHAR_NUM; ++nCount)
    {
        if(0 != m_UserInfo.CharID[nCount])
        {
            dwCheckCID[nCount] = m_UserInfo.CharID[nCount];
        }
        else if(0 != dwSelectedCID[nCount])
        {
            dwCheckCID[nCount] = dwSelectedCID[nCount];
        }
    }

    for(int nCount = 0; nCount < USER_INFO::MAX_CHAR_NUM; ++nCount)
    {
        if(0 != dwCheckCID[nCount])
        {
            UnifiedCharData* lpPos = m_UnifiedCharData;
            UnifiedCharData* lpEnd = m_UnifiedCharData + m_nUnifiedCharDataNum;

            for(; lpPos != lpEnd; ++lpPos)
            {
                if (lpPos->dwNewCID == dwCheckCID[nCount])
                {
                    eCharRace = CClass::GetRace(static_cast<unsigned char>(lpPos->sClass));

                    if(eLastRace == CClass::MAX_RACE)
                    {
                        eLastRace = eCharRace;
                    }
                    else if(eLastRace != eCharRace)
                    {
                        // ĳ���Ͱ� ������ �ٸ� �� ���� ������ �ȵȴ�.
                        return CClass::MAX_RACE;
                    }
                }
            }
        }
    }

    return eLastRace;
}


unsigned char CSessionData::GetTransferedCharCount(CDBComponent& DBComponent)
{
    if (UCHAR_MAX == m_cTransferedCharCount)
    {
        OleDB::PARAM_INFO paramInfo;
        memset(&paramInfo, 0, sizeof(OleDB::PARAM_INFO));

        paramInfo.ColNum = 1;
        paramInfo.eParamIO[0]   = DBPARAMIO_INPUT;
        paramInfo.ColType[0]    = DBTYPE_I4;
		paramInfo.ColSize[0]    = sizeof(unsigned long);
        
        // DB���� �ε�
        if (!DBComponent.ExecuteQueryWithParams(
            "SELECT TransferCharCount FROM UserFirstLoginInfo WHERE UID=?", 
            reinterpret_cast<char*>(&m_dwUID), paramInfo))
        {
            ERRLOG2(g_Log, "UID:%10u / ĳ���� ���� ȸ�� �ε� ���� : DB���� ���� : %s", 
                m_dwUID, DBComponent.GetErrorString());
        }
        else if (!DBComponent.GetData(&m_cTransferedCharCount))
        {
            ERRLOG2(g_Log, "UID:%10u / ĳ���� ���� ȸ�� �ε� ���� : DB GetData ���� : %s", 
                m_dwUID, DBComponent.GetErrorString());
        }
    }

    return m_cTransferedCharCount;
}

bool CSessionData::UpdateTransferedCharCount(CDBComponent& DBComponent)
{
    // DB�� ����Ѵ�.
    OleDB::PARAM_INFO paramInfo;
    memset(&paramInfo, 0, sizeof(OleDB::PARAM_INFO));

    paramInfo.ColNum = 2;
    paramInfo.eParamIO[0]   = DBPARAMIO_INPUT;
    paramInfo.ColType[0]    = DBTYPE_UI1;
	paramInfo.ColSize[0]    = sizeof(unsigned char);
    
    paramInfo.eParamIO[1]   = DBPARAMIO_INPUT;
    paramInfo.ColType[1]    = DBTYPE_I4;
	paramInfo.ColSize[1]    = sizeof(unsigned long);

#pragma pack(1)

    struct UpdateTransferCharCountParam
    {
        unsigned char   m_cTransferedCharCount;
        unsigned long   m_dwUID;
    };

#pragma pack()

    UpdateTransferCharCountParam paramData;

    paramData.m_cTransferedCharCount = m_cTransferedCharCount;
    paramData.m_dwUID = m_dwUID;

    // DB���� �ε�
    if (!DBComponent.ExecuteQueryWithParams(
        "UPDATE UserFirstLoginInfo SET TransferCharCount=? WHERE UID=?", 
        reinterpret_cast<char*>(&paramData), paramInfo))
    {
        ERRLOG2(g_Log, "UID:%10u / ĳ���� ���� ȸ�� ������Ʈ ���� : DB���� ���� : %s", 
            m_dwUID, DBComponent.GetErrorString());

        return false;
    }

    return true;
}


bool CSessionData::GetRealmPointCheck()
{
	DBAgent::DataStorage::CCharacterData* lpCharacterData = GetCharacterData();

	// ���Լ��� ĳ���Ͱ� �α׾ƿ� �ɶ� ���´�.

	if(lpCharacterData)
	{
		unsigned char cRealmPoint = lpCharacterData->GetRealmPoint();

		// ���� �ð��� �������̰�.
		if(CGameTimeDBMgr::GetInstance().GetCurrentGameTime(GameTime::REALM)==GameTime::RT_REALM_WAR
			|| CGameTimeDBMgr::GetInstance().GetCurrentGameTime(GameTime::REALM)==GameTime::RT_REALM_WAR_END_5)
		{
			// ĳ���Ϳ� �ֿ������� �ӽ� ��������Ʈ�� ������.
			if(!lpCharacterData->GetRealmCheckPoint())
			{
				// 1����Ʈ�� ��������Ʈ�� �ش�. ���⼭ AddRealmPoint�� �ش� ������
				// �α����� �ð����� ������/2�� �ð��� �����߸� �ش�.
/*
				// edith 2010.01.10 �ð����� ���� ����Ʈ �ִ� �κ� ����
				// �ð��� ���� ��������Ʈ�� �ִ� ������ ���ŵư�. ������ ���� ���� ���� �ְ� �����ƴ�.
				RealmPoint::AddRealmPoint(this, 1);
*/
				// ���� ��������Ʈ�� ���� ��ũ��.
				// �̷����� AddRealmPoint �Լ��� ȣ���ϱ����� 1�̰� �α׾ƿ��� ȣ���ؼ� 
				// ��������Ʈ���� 2�̸� �ٷ� �־����� �ʰ� 
				// ����üũ ����Ʈ�� 1����Ʈ�� �༭ ���� �α��ν� ������ �ְ� �ϱ� ���ѷ���
				// �� �α׾ƿ��� AddRealmPoint �Լ��� ������ ��������Ʈ�� �������ִ� ��Ȳ�̳� �ƴϳĸ� �˻�
				// �ϱ� ���� �Լ��� ���ǰ� ����. 
				if(cRealmPoint<lpCharacterData->GetRealmPoint())
				{
					lpCharacterData->SetRealmCheckPoint(1);
					lpCharacterData->SetRealmPoint(cRealmPoint);
					lpCharacterData->SetRealmMinute(0);					
				}
				else
				{
					// ���� ����Ʈ�� ���ų� ������ �̸��� ����Ʈ�� ����Ȱ� ����.
					// �׷� ������ ������ �ð��� �������ش�.
					// edith 2008.12.4 �α��� Ÿ������ ó��
					TIME* pTime = lpCharacterData->GetLoginTime();
//					TIME* pTime = lpCharacterData->GetLogoutTime();

					if(pTime)
					{
						CTime timeCurrent = CTime::GetCurrentTime();

						CTime timeLogin(pTime->Year, pTime->Month, pTime->Day, pTime->Hour, pTime->Minute, pTime->Second, pTime->MSecond);

						CTimeSpan ts = timeCurrent - timeLogin;

						unsigned char cMinute = lpCharacterData->GetRealmMinute();

						lpCharacterData->SetRealmMinute((unsigned char)(ts.GetTotalMinutes()+cMinute));
					}					
				}
			}			
		}		

		// edith 2010.01.02 ��������Ʈ�� ���� ������ ���� ó�����ִ� �Լ�.
		unsigned int GID = lpCharacterData->GetGID();
		Guild::CGuildDB* lpGuild = static_cast<Guild::CGuildDB*>(
            Guild::CGuildDBMgr::GetInstance().GetGuild( GID ));

		if(lpGuild)
		{
			unsigned char MaxNum = lpGuild->GetMaxMemberNum();
			unsigned char CurNum = lpGuild->GetCurrentMemberNum();
			lpCharacterData->SetRealmPoint(MaxNum/10);
		}
		else 
			lpCharacterData->SetRealmPoint(0);


		CTime timeLogin = CTime::GetCurrentTime();

		TIME* pLoggoutTime = lpCharacterData->GetLogoutTime();

		if(pLoggoutTime)
		{
			pLoggoutTime->Year		= timeLogin.GetYear();
			pLoggoutTime->Month		= timeLogin.GetMonth();
			pLoggoutTime->Day		= timeLogin.GetDay();			
			pLoggoutTime->Hour		= timeLogin.GetHour();
			pLoggoutTime->Minute	= timeLogin.GetMinute();
			pLoggoutTime->Second	= timeLogin.GetSecond();
		}
	}	
	return true;
}


}
}