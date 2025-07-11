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

// WORK_LIST 2.3 계정 국적 변경 기능 구현
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
        SERLOG1(g_Log, "UID:%10u / 캐릭터 데이터 로드 실패 : GetPlayTime 호출 실패", m_dwUID);
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
//      SERLOG1(g_Log, "UID:%10u / 프리미엄 유저 아님", m_dwUID);
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

    // 유저 정보 얻기
    bool bFirstLoadUserInfoResult = 
        GameDB::GetUserInfo(DBComponent, m_dwUID, m_UserInfo, m_cOldServerGroupID, true);

    if (!bFirstLoadUserInfoResult) 
    {
        if(!GameDB::InsertUser(DBComponent, m_dwUID, m_cOldServerGroupID))
        {
            SERLOG3(g_Log, "UID:%10u / ServerGroupID:%d / 유저 삽입 실패 : %s",
                m_dwUID, m_cOldServerGroupID, DBComponent.GetErrorString());

            return false;
        }        
    }

    if (!bFirstLoadUserInfoResult && 
        !GameDB::GetUserInfo(DBComponent, m_dwUID, m_UserInfo, m_cOldServerGroupID)) 
    {
        SERLOG3(g_Log, "UID:%10u / ServerGroupID:%d / 유저 정보 얻기 실패 : %s",
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
                SERLOG5(g_Log, "UID:%10u / ServerGroupID:%d / CID:%10u / Slot:%d / 캐릭터 뷰 얻기 실패 : %s", 
                    m_dwUID, m_cOldServerGroupID, m_UserInfo.CharID[nCount], nCount, DBComponent.GetErrorString());
/*
				// edith 2010.01.03 제너럴 네트워크로 DB연결이 끊어졌을때 GetCharView를 실패하니
				// GameDB의 UserInfo의 CID를 0으로 초기화 해벼러 나중에 DB에 CID가 0인 정보가 업데이트 되어
				// 캐릭터가 삭제되는 버그개 됐다.

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
            INFLOG2(g_Log, "UID:%10u / ServerGroupID:%d / 유저 정보 얻기 성공", 
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
    // UnifiedStoreInfo / UnifiedCharData 를 읽어서 세션에 저장한다.
    memset(m_UnifiedStoreInfo, 0, sizeof(UnifiedStoreInfo) * PktUnifiedCharInfo::MAX_STORE_INFO);
    memset(m_UnifiedCharData, 0, sizeof(UnifiedCharData) * PktUnifiedCharInfo::MAX_CHAR_DATA);

    m_nUnifiedStoreInfoNum = PktUnifiedCharInfo::MAX_STORE_INFO;
    m_nUnifiedCharDataNum = PktUnifiedCharInfo::MAX_CHAR_DATA;

    using namespace DBComponent;

    // 통합 정보 읽기.
    if(!GameDB::GetUnifiedItemStoreInfo(DBComponent, m_dwUID, m_UnifiedStoreInfo, m_nUnifiedStoreInfoNum))
    {
        ERRLOG2(g_Log, "UID:%10u / 통합 창고 정보 읽어오기 실패 : %s", m_dwUID, DBComponent.GetErrorString());
    }
    else if(!GameDB::GetUnifiedCharList(DBComponent, m_dwUID, m_UnifiedCharData, m_nUnifiedCharDataNum))
    {
        ERRLOG2(g_Log, "UID:%10u / 통합 캐릭터 정보 읽어오기 실패 : %s", m_dwUID, DBComponent.GetErrorString());    
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
                ERRLOG2(g_Log, "UID:%10u / 통합 캐릭터 유저 생성 실패 : %s", 
                    m_dwUID, DBComponent.GetErrorString());
            }
            else
            {
                unsigned char cAgentServerType = 
                    static_cast<unsigned char>(CServerSetup::GetInstance().GetAgentServerType());

                if (m_UserInfo.FirstLogin & UnifiedConst::SELECTED_PART2)
                {
                    // 창고를 이미 선택한 경우는 창고 정보를 줄 필요가 없다.
                    memset(m_UnifiedStoreInfo, 0, sizeof(UnifiedStoreInfo) * PktUnifiedCharInfo::MAX_STORE_INFO);
                    m_nUnifiedStoreInfoNum = 0;
                }
                else
                {
                    // 서버그룹 ID가 현재 중계서버타입보다 작은 값만 추린다.
                    UnifiedStoreInfo* lpStorePos = m_UnifiedStoreInfo;
                    UnifiedStoreInfo* lpStoreEnd = m_UnifiedStoreInfo + m_nUnifiedStoreInfoNum;

                    for(; lpStorePos < lpStoreEnd; )
                    {
                        // 현재 보유중인 창고는 보내 줄 필요 없다.
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

                    // 창고가 없거나, 창고가 있어도 자기 창고만 있는 경우에는, 
                    // 창고를 이미 선택한 것 처럼 해 준다.
                    if (0 == m_nUnifiedStoreInfoNum ||
                       (1 == m_nUnifiedStoreInfoNum && m_UnifiedStoreInfo->cOldServerGroupID == cAgentServerType))
                    {
                        m_UserInfo.FirstLogin |= UnifiedConst::SELECTED_PART2;
                        DBComponent::GameDB::UpdateUserFirstLogin(DBComponent, m_dwUID, m_UserInfo.FirstLogin);
                    }
                }
                
                // 서버그룹 ID가 현재 중계서버타입보다 작은 값만 추린다.
                UnifiedCharData* lpCharPos = m_UnifiedCharData;
                UnifiedCharData* lpCharEnd = m_UnifiedCharData+ m_nUnifiedCharDataNum;

                for(; lpCharPos < lpCharEnd; )
                {
                    // 현재 보유중인 캐릭터는 보내 줘야 한다.
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

                INFLOG4(g_Log, "UID:%10u / FirstLogin:%d / ReadStoreInfo:%d / ReadCharData:%d / 서버통합 정보를 읽는데 성공했습니다", 
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

            INFLOG3(g_Log, "UID:%10u / ReadStoreInfo:%d / ReadCharData:%d / 서버통합 정보를 읽는데 성공했습니다",
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
        LogUserDataError("유저 활성화 시도 실패", LOG_FFL);
    }
    // DB에서 유저 정보 및 뷰를 얻어온다.
    else if (GetUserInfoFromDB(DBComponent))
    {
        // 유저 활성화.
        SetSessionState(SE_USER_ENABLED);
        INFLOG1(g_Log, "UID:%10u / 유저 활성화", m_dwUID);

        // 운영자 레벨 설정.
        m_cAdminLevel = CAdminDataMgr::GetInstance().GetAdminLevel(m_dwUID, m_RemoteAddress);

        // 유저 수 늘림.
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
        // 유저 비활성화 상태 전환
        SetSessionState(SE_USER_DISABLED);

        // 유저 비활성화 전송
        SendPacket::UpdateUIDTable(PktUUT::UpdateUIDTableUserLogout,
            m_szAccount, NULL, m_dwUID, m_dwCID, m_dwSessionID, m_dwServerID, m_RemoteAddress);

        // 유저 수 줄임
        GET_SINGLE_DISPATCH(lpAuthDispatch, 
            CAuthDispatch, CAuthDispatch::GetDispatchTable());

        if(0 != lpAuthDispatch)
        {
            lpAuthDispatch->DecUserNum();
        }

        // 혹시 데이터가 로드되어 있으면 언로드한다.
        UnloadData();
        GetBilling().ClearBilling();
    }
    else
    {
        LogUserDataError("유저 비활성화 시도 실패", LOG_FFL);
        return false;
    }
    
    return true;
}

bool CSessionData::UserMove()
{
    if (SE_USER_ENABLED == m_eSessionState)
    {
        // 캐릭터 이동
        SetSessionState(SE_MOVE);
        SendPacket::UpdateUIDTable(PktUUT::UpdateUIDTableUserMove,
            m_szAccount, NULL, m_dwUID, m_dwCID, m_dwSessionID, m_dwServerID, m_RemoteAddress);

        // 유저 수 줄임
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
        LogUserDataError("유저 이동 시도 실패", LOG_FFL);
        return false;
    }

    return true;
}

bool CSessionData::LoadData(unsigned long dwCID)
{
    DBDataError eStoreDataError     = DATA_SUCCEEDED;
    DBDataError eCharacterDataError = DATA_SUCCEEDED;

    // 창고 데이터 로드 (창고가 비었거나, 다른 UID데이터를 가진 경우에만 로드)
    if(0 == m_lpStoreData || m_dwUID != m_lpStoreData->GetUID())
    {            
        if(0 != m_lpStoreData)
        {
            // 데이터 반납
            CStoreDataMgr::GetInstance().UnloadData(*this, m_lpStoreData);
            m_lpStoreData = 0;
        }

        // 데이터 로드
        eStoreDataError = CStoreDataMgr::GetInstance().LoadData(*this, 
            UnifiedStoreKey(m_dwUID, m_cOldServerGroupID), m_lpStoreData);
    }

    // 캐릭터 데이터 로드 (캐릭터 데이터가 비었거나, 다른 캐릭터 데이터를 가진 경우에만 로드)
    if(0 == m_lpCharacterData || dwCID != m_lpCharacterData->GetCID())
    {
        if(0 != m_lpCharacterData)
        {
            // 데이터 반납
            CCharacterDataMgr::GetInstance().UnloadData(*this, m_lpCharacterData);
            m_lpCharacterData = 0;
        }

	    // 데이터 로드
        eCharacterDataError = 
            CCharacterDataMgr::GetInstance().LoadData(*this, dwCID, m_lpCharacterData);        
    }

    // 결과 처리
    if (eStoreDataError != DATA_SUCCEEDED || 
        eCharacterDataError != DATA_SUCCEEDED)
    {
        // 에러처리. 데이터를 Unload하고 로그를 찍는다.
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

        ERRLOG5(g_Log, "UID:%10u / ServerGroupID:%d / CID:%10u / Store:%s / Character:%s / 창고, 캐릭터 데이터 로드 실패.", 
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
            // 이동 존 업데이트
            UpdateMoveZone();

			// edith 2008.12.4 로그인 타임갱신기능 추가
			// 캐릭터가 Enable 됀 상태면 현재서버에 로그인된 시간을 나타낸다.
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

            // 기타 정보 세팅 (데이터 세팅 후 함수 호출 필요)
            m_dwServerID = dwServerID;
            m_lpCharacterData->SetServerID(dwServerID);

            // 캐릭터 활성화
            SetSessionState(SE_CHAR_ENABLED);
            LogCharData("캐릭터 활성화");
            
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
        SERLOG4(g_Log, "UID:%10u / CID:%10u / ServerID:0x%08X / ST:%s / 캐릭터 활성화 시도 실패 : "
            "세션 상태가 이상합니다.", m_dwUID, dwCID, dwServerID, g_szSessionStateString[m_eSessionState]);
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
                "캐릭터 비활성화 실패 : 다른 캐릭터로, 혹은 다른 서버에서 비활성화 시도",
                m_dwUID, m_dwCID, m_dwServerID, dwCID, dwServerID);
        }
        else
        {
            int nRace = (0 != m_lpCharacterData) ? m_lpCharacterData->GetRace() : -1;

            // 캐릭터 비활성화 전송
            SendPacket::UpdateUIDTable(PktUUT::UpdateUIDTableCharLogout,
                m_szAccount, NULL, m_dwUID, m_dwCID, m_dwSessionID, m_dwServerID, m_RemoteAddress);
            
            // 데이터 반납. 성공하던 실패하던, 소유권은 잃어버리게 된다.
            UnloadData();

            // 캐릭터 비활성화 (함수 호출 후에 데이터 리셋)
            SetSessionState(SE_CHAR_DISABLED);

            GET_MULTI_DISPATCH(lpGameDispatch, dwServerID, 
                CGameDispatch, CGameDispatch::GetDispatchTable());

            if(0 != lpGameDispatch)
            {
                lpGameDispatch->DecCharNum(nRace);
            }

            // 순서 주의! 맨 마지막에 0으로 클리어!
            m_dwCID = 0;
            m_dwServerID = 0;
            m_cNewZone = 0;
            GetBilling().ClearBilling();
            return true;
        }
    }
    else
    {
        LogCharDataError("캐릭터 비활성화 시도 실패", LOG_FFL);
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
                "캐릭터 이동 실패 : 다른 캐릭터로, 혹은 다른 서버에서 비활성화 시도",
                m_dwUID, m_dwCID, m_dwServerID, dwCID, dwServerID);
        }
        else
        {
            int nRace = (0 != m_lpCharacterData) ? m_lpCharacterData->GetRace() : -1;

            // 캐릭터 이동 전송
            SendPacket::UpdateUIDTable(PktUUT::UpdateUIDTableCharLogout,
                m_szAccount, NULL, m_dwUID, m_dwCID, m_dwSessionID, m_dwServerID, m_RemoteAddress);

            LogCharData("캐릭터 이동");

            // 데이터 반납. 성공하던 실패하던, 소유권은 잃어버리게 된다.
            UnloadData();

            // 캐릭터 이동(함수 호출 뒤에 데이터 리셋
            SetSessionState(SE_MOVE);

            GET_MULTI_DISPATCH(lpGameDispatch, dwServerID, 
                CGameDispatch, CGameDispatch::GetDispatchTable());

            if(0 != lpGameDispatch)
            {
                lpGameDispatch->DecCharNum(nRace);
            }

            // 순서 주의! 맨 마지막에 0으로 클리어!
            m_dwCID = 0;
            m_dwServerID = 0;
            GetBilling().ClearBilling();
            return true;
        }
    }
    else
    {
        LogCharDataError("캐릭터 이동 시도 실패", LOG_FFL);
    }

    return false;
}


// ---------------------------------------------------------------------------
// 존 이동 관련 부분

// 존 이동 설정
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

// 존 이동 업데이트
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

// 캐릭터 존 체크
bool CSessionData::CheckCharZone(unsigned char cZone)
{
	if(this->m_cAdminLevel)
	{
		return true;
	}

	bool bResult = false;

	// edith 2009.07.30 종족별로 갈수없는 존을 제한한다.
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
		// 파트1에서 2로 추가된 기본서버
		bResult = true;
	}
	else if (cZone == SERVER_ID::CAPITAL || cZone == SERVER_ID::ZONE12 || cZone == SERVER_ID::PRISON)
	{
		// 파트2에서 사용했던 로직
		bResult = true;
	}
	else if (cZone == /*SERVER_ID::STONE_WAR1*/SERVER_ID::CAPITAL || cZone == SERVER_ID::STONE_WAR2 || cZone == SERVER_ID::STONE_WAR3)
	{
		// 다크카나반 제한
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
		// 신규존 제한
		if (true == CServerSetup::GetInstance().UseContents(GameRYL::NEWZONE_ZONE9))
		{
			bResult = true;
		}
	}
*/

	return bResult;
}

// edith 2008.02.28 캐릭터 시작 존
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

//! 요청 큐 관련 (dwRequestKey가 0이면 에러)
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
        // 창고를 선택한 적 없고, 존재하는 창고이고, 캐쉬에서 누가 사용하고 있는가를 
        // 확인한 다음, 다음을 수행한다.
       
        // cOldServerGroupID의 창고 데이터를 cChangedServerGroupID로 덮어 씌우고,
        // cOldServerGroupID의 창고 데이터를 NULL로 만든다.
        if(!DBComponent::GameDB::ChangeUnifiedItemStoreGroup(
            DBComponent, storeKey.first, cOldServerGroupID, cNewServerGroupID))
        {
            // 창고 그룹 번호 변경 실패
            ERRLOG2(g_Log, "UID:%10u / SelectedServerGroupID:%d / 통합서버 선택 실패 : 창고 그룹 번호 변경 실패",
                storeKey.first, cOldServerGroupID);            
        }
        else
        {
            // 3. 메모리의 UnifiedStoreInfo를 맞춰서 변경한다.
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
        ERRLOG2(g_Log, "UID:%10u / SelectedServerGroupID:%d / 통합서버 선택 실패 : 잘못된 서버그룹 선택",
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
            // 에러 로그 남김
            ERRLOG3(g_Log, "UID:%10u / CID:%10u / ServerGroupID:%d / 통합서버 선택 실패 : 캐릭터 서버그룹 ID변경 실패",
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
                    // 메모리 데이터를 수정한다.
                    lpPos->cOldServerGroupID = cOldServerGroupID;
                    break;
                }
            } 

            return true;
        }
    }
    else
    {
        ERRLOG3(g_Log, "UID:%10u / CID:%10u / ServerGroupID:%d / 통합서버 선택 실패 : 잘못된 캐릭터 선택",
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
    // UserInfo를 갱신한다.
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

    // UserInfo를 DB에 업데이트한다.
    if (0 != memcmp(&m_UserInfo, &userInfo, sizeof(USER_INFO)))
    {
        if (!DBComponent::GameDB::UpdateUser(DBComponent, m_dwUID, &userInfo))
        {
            // DB쿼리 실패!
            return false;
        }
        else
        {
            m_UserInfo = userInfo;
        }
    }

    // 캐릭터 이전 회수를 DB에 기록한다.
    m_cTransferedCharCount += cSelectedCount;
    UpdateTransferedCharCount(DBComponent);    

    // 선택한 캐릭터 뷰를 다시 얻어온다.
    bool bGetViewResult = true;

    for (int nCount = 0; nCount < USER_INFO::MAX_CHAR_NUM; ++nCount)
    {
        if (0 != m_UserInfo.CharID[nCount] && 
            !DBComponent::GameDB::GetCharView(DBComponent, m_UserInfo.CharID[nCount], &m_CharView[nCount]))
        {
            SERLOG5(g_Log, "UID:%10u / ServerGroupID:%d / CID:%10u / Slot:%d / 캐릭터 뷰 얻기 실패 : %s", 
                m_dwUID, m_cOldServerGroupID, m_UserInfo.CharID[nCount], nCount, DBComponent.GetErrorString());

            bGetViewResult = false;
        }
    }

    if(bGetViewResult)
    {
        INFLOG2(g_Log, "UID:%10u / ServerGroupID:%d / 유저 정보 얻기 성공", 
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
                        // 캐릭터가 종족이 다른 게 섞여 있으면 안된다.
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
        
        // DB에서 로드
        if (!DBComponent.ExecuteQueryWithParams(
            "SELECT TransferCharCount FROM UserFirstLoginInfo WHERE UID=?", 
            reinterpret_cast<char*>(&m_dwUID), paramInfo))
        {
            ERRLOG2(g_Log, "UID:%10u / 캐릭터 이전 회수 로드 실패 : DB쿼리 실패 : %s", 
                m_dwUID, DBComponent.GetErrorString());
        }
        else if (!DBComponent.GetData(&m_cTransferedCharCount))
        {
            ERRLOG2(g_Log, "UID:%10u / 캐릭터 이전 회수 로드 실패 : DB GetData 실패 : %s", 
                m_dwUID, DBComponent.GetErrorString());
        }
    }

    return m_cTransferedCharCount;
}

bool CSessionData::UpdateTransferedCharCount(CDBComponent& DBComponent)
{
    // DB에 기록한다.
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

    // DB에서 로드
    if (!DBComponent.ExecuteQueryWithParams(
        "UPDATE UserFirstLoginInfo SET TransferCharCount=? WHERE UID=?", 
        reinterpret_cast<char*>(&paramData), paramInfo))
    {
        ERRLOG2(g_Log, "UID:%10u / 캐릭터 이전 회수 업데이트 실패 : DB쿼리 실패 : %s", 
            m_dwUID, DBComponent.GetErrorString());

        return false;
    }

    return true;
}


bool CSessionData::GetRealmPointCheck()
{
	DBAgent::DataStorage::CCharacterData* lpCharacterData = GetCharacterData();

	// 이함수는 캐릭터가 로그아웃 될때 들어온다.

	if(lpCharacterData)
	{
		unsigned char cRealmPoint = lpCharacterData->GetRealmPoint();

		// 현재 시간이 전쟁중이고.
		if(CGameTimeDBMgr::GetInstance().GetCurrentGameTime(GameTime::REALM)==GameTime::RT_REALM_WAR
			|| CGameTimeDBMgr::GetInstance().GetCurrentGameTime(GameTime::REALM)==GameTime::RT_REALM_WAR_END_5)
		{
			// 캐릭터에 주워져야할 임시 랠름포인트가 없으면.
			if(!lpCharacterData->GetRealmCheckPoint())
			{
				// 1포인트의 렐름포인트를 준다. 여기서 AddRealmPoint는 해당 서버에
				// 로그인한 시간부터 랠름전/2의 시간이 지나야만 준다.
/*
				// edith 2010.01.10 시간별로 렐름 포인트 주는 부분 삭제
				// 시간에 따라 렐름포인트를 주는 로직이 제거됐고. 길드원의 수에 따라 별을 주게 수정됐다.
				RealmPoint::AddRealmPoint(this, 1);
*/
				// 만약 렐름포인트의 값이 더크면.
				// 이로직은 AddRealmPoint 함수를 호출하기전에 1이고 로그아웃시 호출해서 
				// 랠름포인트값이 2이면 바로 주어지지 않고 
				// 랠름체크 포인트에 1포인트를 줘서 다음 로그인시 받을수 있게 하기 위한로직
				// 즉 로그아웃시 AddRealmPoint 함수는 유저가 랠름포인트를 받을수있는 상황이냐 아니냐를 검사
				// 하기 위한 함수로 사용되고 있음. 
				if(cRealmPoint<lpCharacterData->GetRealmPoint())
				{
					lpCharacterData->SetRealmCheckPoint(1);
					lpCharacterData->SetRealmPoint(cRealmPoint);
					lpCharacterData->SetRealmMinute(0);					
				}
				else
				{
					// 만약 포인트가 같거나 작으면 이말은 포인트가 변경된게 없다.
					// 그럼 유저의 랠름전 시간을 갱신해준다.
					// edith 2008.12.4 로그인 타임으로 처리
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

		// edith 2010.01.02 공헌포인트를 길드원 정보에 따라 처리해주는 함수.
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