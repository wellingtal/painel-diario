#ifndef _RYL_DBAGENT_DB_DATA_H_
#define _RYL_DBAGENT_DB_DATA_H_

//forward decl.
class CDBComponent;

namespace DBAgent
{
namespace DataStorage
{
    // forward decl.
    class CSessionData;
    class CDBDataMgr;

    class IDBData
    {
    public:

        virtual ~IDBData() { }

        virtual unsigned long GetKey() const = 0;                   //! Key가 되는 값을 얻어 온다.
        virtual unsigned long GetLastDBUpdateTime() const = 0;      //! 마지막으로 DB에 업데이트 한 시간을 얻어온다.

        virtual bool UpdateDBData(CDBComponent& DBComponent) = 0;   //! DB에 데이터를 업데이트한다.
        virtual bool IsDataChanged() const = 0;                     //! 데이터가 변경되었는지 여부를 리턴
        
        //! 가지고 있는 데이터를 파일로 덤프를 남기거나, 파일에서 데이터를 불러 온다.
        virtual bool WriteDataToDumpFile(const char* szFileName) = 0;
        virtual bool ReadDataFromDumpFile(const char* szFileName) = 0;

    protected:

        //! DB에서 데이터를 불러온다.
        virtual bool GetFromDB(CDBComponent& DBComponent, unsigned long dwKey) = 0; 

        friend class CDBDataMgr;
    };
}
}

#endif