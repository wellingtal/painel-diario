#ifndef _RYL_DBAGENT_USER_DATA_H_
#define _RYL_DBAGENT_USER_DATA_H_

#include <DB/DBdefine.h>

// forward decl.
class CDBComponent;

namespace DBAgent
{
namespace DataStorage
{
    typedef std::pair<unsigned long, unsigned char> UnifiedStoreKey;

    class CStoreData
    {
    public:

        CStoreData();
        ~CStoreData();
    
        UnifiedStoreKey GetKey() const { return UnifiedStoreKey(m_dwUID, m_cOldServerGroupID); }
        unsigned long GetUID() const { return m_dwUID; }

        const STORE_INFO& GetStoreInfo() const  { return m_StoreInfo; }
        const STORE& GetStore1() const          { return m_Store1; }
        const STORE& GetStore2() const          { return m_Store2; }

        void SetStoreInfo(const STORE_INFO& storeInfo);
        bool SetStore1(const char* lpData, unsigned long dwDataSize);
        bool SetStore2(const char* lpData, unsigned long dwDataSize);

        bool IsVaildData() const
        {
            return 0 != m_dwUID && 
                0 != strncmp("", m_StoreInfo.Password, STORE_INFO::MAX_PASS_LEN);
        }

        bool IsDataChanged() const { return (0 != m_dwUpdateData); }

        void ClearData();

        //! DB에 창고 데이터를 기록한다.
        bool UpdateDBData(CDBComponent& DBComponent);
        
        //! DB에 창고 데이터를 강제로 업데이트한다.
        bool ForceUpdateDBAllData(CDBComponent& DBComponent)
        {
            m_dwUpdateData = 0xFFFFFFFF;
            return UpdateDBData(DBComponent);
        }

        unsigned long GetLastDBUpdateTime() const { return m_dwLastDBUpdateTime; }

        //! DB에서 창고 데이터를 얻어온다.
        bool GetFromDB(CDBComponent& DBComponent, UnifiedStoreKey storeKey); 

        bool WriteDataToDumpFile(const char* szFileName);
        bool ReadDataFromDumpFile(const char* szFileName);

        //! 아이템 컨버팅 관련
        bool ConvertToPart2Item(bool bConvertSkillToTicket);

    private:

        unsigned long   m_dwUID;
        unsigned long   m_dwUpdateData;
        unsigned long   m_dwLastDBUpdateTime;

        STORE_INFO	    m_StoreInfo;
        STORE		    m_Store1;
        STORE		    m_Store2;

        unsigned char   m_cOldServerGroupID;
    };



}
}

#endif