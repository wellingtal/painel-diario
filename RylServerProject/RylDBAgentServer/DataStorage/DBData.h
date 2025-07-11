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

        virtual unsigned long GetKey() const = 0;                   //! Key�� �Ǵ� ���� ��� �´�.
        virtual unsigned long GetLastDBUpdateTime() const = 0;      //! ���������� DB�� ������Ʈ �� �ð��� ���´�.

        virtual bool UpdateDBData(CDBComponent& DBComponent) = 0;   //! DB�� �����͸� ������Ʈ�Ѵ�.
        virtual bool IsDataChanged() const = 0;                     //! �����Ͱ� ����Ǿ����� ���θ� ����
        
        //! ������ �ִ� �����͸� ���Ϸ� ������ ����ų�, ���Ͽ��� �����͸� �ҷ� �´�.
        virtual bool WriteDataToDumpFile(const char* szFileName) = 0;
        virtual bool ReadDataFromDumpFile(const char* szFileName) = 0;

    protected:

        //! DB���� �����͸� �ҷ��´�.
        virtual bool GetFromDB(CDBComponent& DBComponent, unsigned long dwKey) = 0; 

        friend class CDBDataMgr;
    };
}
}

#endif