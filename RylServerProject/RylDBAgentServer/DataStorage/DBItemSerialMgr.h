#ifndef _RYL_DBAGENT_DB_ITEM_SERIAL_MGR_H_
#define _RYL_DBAGENT_DB_ITEM_SERIAL_MGR_H_

#include <Item/ItemSerialMgr.h>

namespace DBAgent
{
namespace DataStorage
{
    class CDBAgentItemSerialMgr : public Item::CItemSerialMgr
    {
    public:

        static CDBAgentItemSerialMgr& GetInstance();
        
        unsigned long GetServerID() const { return m_dwServerID; }
        void SetServerID(unsigned long dwServerID) { m_dwServerID = dwServerID; }
        
        void SaveChangedSerial(CDBComponent& DBComponent);

    private:

        CDBAgentItemSerialMgr() : m_dwLastChangedItemSerial(0) { }
        ~CDBAgentItemSerialMgr() { }

        unsigned __int64 m_dwLastChangedItemSerial;

    };
}
}

#endif