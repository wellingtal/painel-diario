#include "stdafx.h"
#include "CharacterDataMgr.h"
#include "SessionDataMgr.h"

#include <Log/ServerLog.h>

namespace DBAgent
{
namespace DataStorage
{

CCharacterDataMgr& CCharacterDataMgr::GetInstance()
{
    static CCharacterDataMgr characterDataMgr;
    return characterDataMgr;
}

CCharacterDataMgr::CCharacterDataMgr()
{
    SetManagerName("Ä³¸¯ÅÍ");
}


CCharacterDataMgr::~CCharacterDataMgr()
{

}

DBDataError CCharacterDataMgr::LoadData(CSessionData& SessionData, unsigned long dwKey, 
                                        CCharacterData*& lpLoadData_Out)
{
    DBDataError eDataError = CCharaterDataMgrBase::LoadData(SessionData, dwKey, lpLoadData_Out);

    if(DATA_SUCCEEDED == eDataError)
    {
        CSessionDataMgr::GetInstance().AddCharLoadedSession(dwKey, SessionData);
    }

    return eDataError;
}

DBDataError CCharacterDataMgr::UnloadData(const CSessionData& SessionData, CCharacterData* lpUnloadData)
{
    DBDataError eDataError = CCharaterDataMgrBase::UnloadData(SessionData, lpUnloadData);

    if(DATA_SUCCEEDED == eDataError)
    {
        CSessionDataMgr::GetInstance().RemoveCharLoadedSession(SessionData.GetCID());
    }

    return eDataError;
}

void CCharacterDataMgr::Log(LOG_TYPE eLogType, const char* pRtn, const char* pFileName, int nLine,
                            const CSessionData* lpSessionData, const unsigned long& dwKey, const char* szErrorMsg)
{
    switch(eLogType)
    {
    case LOG_NORMAL:
    case LOG_RULE:
    case LOG_INFO:

        if(0 != lpSessionData)
        {
            g_Log.SimpleLog(eLogType, "UID:%10u / CID:%10u / Key:%10u / %s %s",
                lpSessionData->GetUID(), lpSessionData->GetCID(), dwKey, GetManagerName(), szErrorMsg);
        }
        else
        {
            g_Log.SimpleLog(eLogType, "Key:%10u / %s %s",
                dwKey, GetManagerName(), szErrorMsg);
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
                "UID:%10u / CID:%10u / Key:%10u / %s %s",
                lpSessionData->GetUID(), lpSessionData->GetCID(), dwKey, GetManagerName(), szErrorMsg);
        }
        else
        {
            g_Log.DetailLog(eLogType, pRtn, pFileName, nLine, "Key:%10u / %s %s",
                dwKey, GetManagerName(), szErrorMsg);
        }
        break;
    }
}



}
}