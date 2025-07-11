#include "stdafx.h"
#include "StoreData.h"
#include "SessionData.h"
#include "DBDataMgr.h"
#include "DBItemSerialMgr.h"

#include <DB/DBComponent.h>
#include <DB/GameDBComponent.h>

#include <Log/ServerLog.h>
#include <Utility/Debug/PerformanceCheck.h>

#include <Item/ItemMgr.h>
#include <Item/Container/ContainerConstant.h>
#include <Item/RebalanceConvert/OldItemMgr.h>
#include <Item/RebalanceConvert/RebalanceItem.h>
#include <Item/RebalanceConvert/ContainerChecker.h>

#include <mmsystem.h>

namespace DBAgent
{
namespace DataStorage
{

enum CHANGED_STORE_DATA
{
    CHANGED_STORE_INFO  = (1 <<  0),
    CHANGED_STORE_1     = (1 <<  1),
    CHANGED_STORE_2     = (1 <<  2)
};


CStoreData::CStoreData()
:   m_dwUID(0), m_dwUpdateData(0), m_dwLastDBUpdateTime(0)
{
    ClearData();
}

CStoreData::~CStoreData()
{

}

void CStoreData::SetStoreInfo(const STORE_INFO& storeInfo)
{
    if(0 != memcmp(&m_StoreInfo, &storeInfo, sizeof(STORE_INFO)))
    {
        m_StoreInfo = storeInfo;
        m_dwUpdateData |= CHANGED_STORE_INFO;
    }
}

bool CStoreData::SetStore1(const char* lpData, unsigned long dwDataSize)
{
    if(STORE::MAX_STORE_SIZE < dwDataSize)
    {
        SERLOG3(g_Log, "UID:%10u / 창고 데이터 업데이트 에러 : Store1 (%u/%u)",
            GetUID(), STORE::MAX_STORE_SIZE, dwDataSize);

        return false;
    }
    else if(m_Store1.dwSize != dwDataSize || 0 != memcmp(m_Store1.Data, lpData, dwDataSize))
    {
        memcpy(m_Store1.Data, lpData, dwDataSize);
        m_Store1.dwSize = dwDataSize;

        m_dwUpdateData |= CHANGED_STORE_1;
    }

    return true;
}

bool CStoreData::SetStore2(const char* lpData, unsigned long dwDataSize)
{
    if(STORE::MAX_STORE_SIZE < dwDataSize)
    {
        SERLOG3(g_Log, "UID:%10u / 창고 데이터 업데이트 에러 : Store2 (%u/%u)",
            GetUID(), STORE::MAX_STORE_SIZE, dwDataSize);

        return false;
    }
    else if(m_Store2.dwSize != dwDataSize || 0 != memcmp(m_Store2.Data, lpData, dwDataSize))
    {
        memcpy(m_Store2.Data, lpData, dwDataSize);
        m_Store2.dwSize = dwDataSize;

        m_dwUpdateData |= CHANGED_STORE_2;
    }

    return true;
}

void CStoreData::ClearData()
{
    m_dwUID = 0;
    m_dwUpdateData = 0;
    m_cOldServerGroupID = 0;

    memset(&m_StoreInfo, 0, sizeof(STORE_INFO));
    memset(&m_Store1, 0, sizeof(STORE));
    memset(&m_Store2, 0, sizeof(STORE));
}


bool CStoreData::GetFromDB(CDBComponent& DBComponent, UnifiedStoreKey storeKey) 
{
    PERFORMANCE_CHECK(FunctionTimingCheck);

    ClearData();

    using namespace DBComponent;

    // 창고 정보
    if (!GameDB::GetUnifiedItemStoreInfo(DBComponent, 
        storeKey.first, storeKey.second, &m_StoreInfo, true)) 
    {
        if (!GameDB::InsertUser(DBComponent, storeKey.first, storeKey.second, 0)) 
        {
            SERLOG2(g_Log, "UID:%10u / ServerGroupID:%d / 창고 데이터 로드 실패 : InsertUser 호출 실패", 
                storeKey.first, storeKey.second);
            return false;
        }

        if (!GameDB::GetUnifiedItemStoreInfo(DBComponent, 
            storeKey.first, storeKey.second, &m_StoreInfo)) 
        {
            SERLOG2(g_Log, "UID:%10u / ServerGroupID:%d / 창고 데이터 로드 실패 : GetUnifiedItemStoreInfo 호출 실패", 
                storeKey.first, storeKey.second);
            return false;
        }
    }

    // 창고 1
    if (!GameDB::GetUnifiedItemStore1(DBComponent, storeKey.first, storeKey.second, &m_Store1)) 
    {
        SERLOG2(g_Log, "UID:%10u / ServerGroupID:%d / 창고 데이터 로드 실패 : GetItemStore1 호출 실패", 
            storeKey.first, storeKey.second);
        return false;
    }

    // 창고 2
    if (!GameDB::GetUnifiedItemStore2(DBComponent, storeKey.first, storeKey.second, &m_Store2)) 
    {
        SERLOG2(g_Log, "UID:%10u / ServerGroupID:%d / 창고 데이터 로드 실패 : GetItemStore2 호출 실패", 
            storeKey.first, storeKey.second);
        return false;
    }

    m_dwUID = storeKey.first;
    m_cOldServerGroupID = storeKey.second;

    INFLOG2(g_Log, "UID:%10u / ServerGroupID:%d / 창고 데이터 로드 성공 : CStoreData::GetFromDB", storeKey.first, storeKey.second);
    return true;
}


bool CStoreData::UpdateDBData(CDBComponent& DBComponent)
{
    PERFORMANCE_CHECK(FunctionTimingCheck);

    // 무결성 검사
    if (!IsVaildData())
    {
        SERLOG1(g_Log, "UID:%10u / 창고 데이터 저장 실패 : UpdateStoreToDB 에서 무결성 검사 실패", m_dwUID);        
        return false;
    }
    else if(IsDataChanged())
    {
        using namespace DBComponent;

        if ((m_dwUpdateData & CHANGED_STORE_INFO) && 
            !GameDB::SetUnifiedItemStoreInfo(DBComponent, m_dwUID, m_cOldServerGroupID, &m_StoreInfo))
        {
            // 창고 정보 업데이트
            SERLOG2(g_Log, "UID:%10u / ServerGroupID:%d / 창고 데이터 저장 실패 : SetUnifiedItemStoreInfo 호출 실패",
                m_dwUID, m_cOldServerGroupID);
            return false;
        }
        else if((m_dwUpdateData & CHANGED_STORE_1) && 
            !GameDB::UpdateUnifiedItemStore1(DBComponent, m_dwUID, m_cOldServerGroupID, &m_Store1))
        {
            // 창고1 업데이트
            SERLOG2(g_Log, "UID:%10u / ServerGroupID:%d / 창고 데이터 저장 실패 : UpdateUnifiedItemStore1 호출 실패", 
                m_dwUID, m_cOldServerGroupID);
            return false;
        }
        else if((m_dwUpdateData & CHANGED_STORE_2) && 
            !GameDB::UpdateUnifiedItemStore2(DBComponent, m_dwUID, m_cOldServerGroupID, &m_Store2))
        {
            // 창고2 업데이트
            SERLOG2(g_Log, "UID:%10u / ServerGroupID:%d / 창고 데이터 저장 실패 : UpdateUnifiedItemStore2 호출 실패", 
                m_dwUID, m_cOldServerGroupID);
            return false;
        }

        m_dwUpdateData = 0;
        INFLOG2(g_Log, "UID:%10u / ServerGroupID:%d / 창고 데이터 저장 성공 : CStoreData::UpdateDBData", 
            m_dwUID, m_cOldServerGroupID);
    }
    else
    {
        INFLOG2(g_Log, "UID:%10u / ServerGroupID:%d / 창고 데이터 저장 성공 : 변경된 데이터가 없어서 업데이트하지 않았습니다.", 
            m_dwUID, m_cOldServerGroupID);
    }

    m_dwLastDBUpdateTime = timeGetTime();
    return true;
}

//! 아이템 컨버팅 관련
bool CStoreData::ConvertToPart2Item(bool bConvertSkillToTicket)
{
    RebalanceLib::CItemArrayChecker<
        ContainerConstant::DEPOSIT_WIDTH, 
        ContainerConstant::DEPOSIT_HEIGHT, 
        ContainerConstant::MAX_DEPOSIT_TAB> depositChecker(TakeType::TS_DEPOSIT);

    RebalanceLib::SkillBookInfo skillBookInfo;

    STORE store1;
    STORE store2;
    unsigned long dwGold = m_StoreInfo.Gold;

    CDBAgentItemSerialMgr& dbItemSerialMgr = 
        CDBAgentItemSerialMgr::GetInstance();

    unsigned __int64 dwItemSerial = dbItemSerialMgr.GetItemSerial();

    memset(&skillBookInfo, 0, sizeof(RebalanceLib::SkillBookInfo));
    memset(&store1, 0, sizeof(STORE));
    memset(&store2, 0, sizeof(STORE));

    store1.dwSize = STORE::MAX_STORE_SIZE;
    store2.dwSize = STORE::MAX_STORE_SIZE;

    if(!RebalanceLib::ConvertContainer(m_dwUID, 0, &depositChecker, 
        bConvertSkillToTicket ? &skillBookInfo : 0, 
        m_Store1.Data, m_Store1.dwSize, store1.Data, store1.dwSize))
    {
        ERRLOG2(g_Log, "UID:%10u / ServerGroupID:%d / 창고1 컨버팅에 실패했습니다", m_dwUID, m_cOldServerGroupID);
    }
    else if(!RebalanceLib::ConvertContainer(m_dwUID, 0, &depositChecker, 
        bConvertSkillToTicket ? &skillBookInfo : 0, 
        m_Store2.Data, m_Store2.dwSize, store2.Data, store2.dwSize))
    {
        ERRLOG2(g_Log, "UID:%10u / ServerGroupID:%d / 창고2 컨버팅에 실패했습니다", m_dwUID, m_cOldServerGroupID);
    }
    else 
    {
        for(int nCount = 0; nCount < CSkillMgr::MAX_SKILL_LOCKCOUNT; ++nCount)
        {
            // 3배로 보상한다.
            skillBookInfo.m_dwBookNum[nCount] *= 3;
        }

        if(bConvertSkillToTicket && 
            !RebalanceLib::AddSkillBook(m_dwUID, 0, "Store1", dbItemSerialMgr, dwGold,
            store1.Data, store1.dwSize, STORE::MAX_STORE_SIZE, depositChecker, skillBookInfo))
        {
            ERRLOG2(g_Log, "UID:%10u / ServerGroupID:%d / 창고1 스킬북 넣어주기에 실패했습니다", m_dwUID, m_cOldServerGroupID);
        }
        else if(bConvertSkillToTicket && !skillBookInfo.IsEmpty() && 
            !RebalanceLib::AddSkillBook(m_dwUID, 0, "Store2", dbItemSerialMgr, dwGold,
            store2.Data, store2.dwSize, STORE::MAX_STORE_SIZE, depositChecker, skillBookInfo))
        {
            ERRLOG2(g_Log, "UID:%10u / ServerGroupID:%d / 창고2 스킬북 넣어주기에 실패했습니다", m_dwUID, m_cOldServerGroupID);
        }
        else
        {
            if(dwItemSerial < dbItemSerialMgr.GetItemSerial())
            {
                dbItemSerialMgr.SaveItemSerial(
                    CDBSingleObject::GetInstance(), dbItemSerialMgr.GetServerID());
            }

            m_StoreInfo.Gold = dwGold;
            m_Store1 = store1;
            m_Store2 = store2;            
        }

        if(!skillBookInfo.IsEmpty())
        {
            for(int nCount = 0; nCount < CSkillMgr::MAX_SKILL_LOCKCOUNT; ++nCount)
            {
                if(0 != skillBookInfo.m_dwBookNum[nCount])
                {
                    ERRLOG3(g_Log, "UID:%10u / LockCount:%d / Num:%d / 스킬북을 넣어주지 못했습니다",
                        m_dwUID, nCount + 1, skillBookInfo.m_dwBookNum[nCount]);
                }
            }
        }

        return true;
    }

    return false;
}

bool CStoreData::WriteDataToDumpFile(const char* szFileName)
{

    return false;
}

bool CStoreData::ReadDataFromDumpFile(const char* szFileName)
{

    return false;
}

}
}
