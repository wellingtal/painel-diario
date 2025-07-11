#ifndef _RYL_DBAGENT_CHARACTER_DATA_MGR_H_
#define _RYL_DBAGENT_CHARACTER_DATA_MGR_H_

#include "DBDataMgr.h"
#include "CharacterData.h"

namespace DBAgent
{
namespace DataStorage
{
    typedef CDBDataMgr<unsigned long, CCharacterData> CCharaterDataMgrBase;

    class CCharacterDataMgr : public CCharaterDataMgrBase
    {
    public:

        static CCharacterDataMgr& GetInstance();

        // 데이터 로드/언로드
        virtual DBDataError LoadData(CSessionData& SessionData, 
            unsigned long dwKey, CCharacterData*& lpLoadData_Out);

        virtual DBDataError UnloadData(const CSessionData& SessionData, CCharacterData* lpUnloadData);

    private:

        CCharacterDataMgr();
        ~CCharacterDataMgr();

        virtual void Log(LOG_TYPE eLogType, const char* pRtn, const char* pFileName, int nLine,
            const CSessionData* lpSessionData, const unsigned long& dataKey, const char* szErrorMsg);
    };
}
}

#endif