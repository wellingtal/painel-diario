#ifndef _RYL_DBAGENT_STORE_DATA_MGR_H_
#define _RYL_DBAGENT_STORE_DATA_MGR_H_

#include "DBDataMgr.h"
#include "StoreData.h"

namespace DBAgent
{
namespace DataStorage
{
    // 첫번째 : UID
    // 두번째 : 서버군 번호
    class CStoreDataMgr : public CDBDataMgr<UnifiedStoreKey, CStoreData>
    {
    public:

        static CStoreDataMgr& GetInstance();

    private:

        CStoreDataMgr();
        ~CStoreDataMgr();

        virtual void Log(LOG_TYPE eLogType, const char* pRtn, const char* pFileName, int nLine,
            const CSessionData* lpSessionData, const UnifiedStoreKey& dataKey, const char* szErrorMsg);        
    };
}
}

#endif