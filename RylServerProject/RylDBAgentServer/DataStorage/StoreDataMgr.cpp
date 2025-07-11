#include "stdafx.h"
#include "StoreDataMgr.h"
#include <Log/ServerLog.h>

namespace DBAgent
{
namespace DataStorage
{

CStoreDataMgr& CStoreDataMgr::GetInstance()
{
    static CStoreDataMgr storeDataMgr;
    return storeDataMgr;
}

CStoreDataMgr::CStoreDataMgr()
{
    SetManagerName("Ã¢°í");
}


CStoreDataMgr::~CStoreDataMgr()
{

}

void CStoreDataMgr::Log(LOG_TYPE eLogType, const char* pRtn, const char* pFileName, int nLine,
                        const CSessionData* lpSessionData, const UnifiedStoreKey& dataKey, const char* szErrorMsg)
{
    switch(eLogType)
    {
    case LOG_NORMAL:
    case LOG_RULE:
    case LOG_INFO:

        if(0 != lpSessionData)
        {
            g_Log.SimpleLog(eLogType, "UID:%10u / CID:%10u / KeyUID:%10u / KeyServerGroupID:%d / %s %s",
                lpSessionData->GetUID(), lpSessionData->GetCID(), 
                dataKey.first, dataKey.second, GetManagerName(), szErrorMsg);
        }
        else
        {
            g_Log.SimpleLog(eLogType, "KeyUID:%10u / KeyServerGroupID:%d / %s %s",
                dataKey.first, dataKey.second, GetManagerName(), szErrorMsg);
        }
        break;

    case LOG_DETAIL:
    case LOG_SYSERR:
    case LOG_ERROR:
    case LOG_DEBUG:
    case LOG_WARN:

        if(0 != lpSessionData)
        {
            g_Log.DetailLog(eLogType, pRtn, pFileName, nLine, 
                "UID:%10u / CID:%10u / KeyUID:%10u / KeyServerGroupID:%d / %s %s",
                lpSessionData->GetUID(), lpSessionData->GetCID(), 
                dataKey.first, dataKey.second, GetManagerName(), szErrorMsg);
        }
        else
        {
            g_Log.DetailLog(eLogType, pRtn, pFileName, nLine, 
                "KeyUID:%10u / KeyServerGroupID:%d / %s %s",
                dataKey.first, dataKey.second, GetManagerName(), szErrorMsg);
        }
        break;
    }
}



}
}