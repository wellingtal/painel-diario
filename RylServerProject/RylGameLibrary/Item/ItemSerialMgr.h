#ifndef _RYL_DB_ITEM_SERIAL_MGR_H_
#define _RYL_DB_ITEM_SERIAL_MGR_H_

// forward delc.
class CDBComponent;

namespace Item
{
    class CItemSerialMgr
    {
    public:

        CItemSerialMgr();
        ~CItemSerialMgr();

        unsigned __int64 GetItemSerial() const { return m_dwItemSerial; }
        unsigned __int64 GetNewItemSerial() { return ++m_dwItemSerial; }

        bool SetItemSerial(unsigned __int64 dwItemSerial);

        bool LoadItemSerial(CDBComponent& DBComponent, unsigned long dwServerID);
        bool SaveItemSerial(CDBComponent& DBComponent, unsigned long dwServerID);

    protected:

        unsigned __int64    m_dwItemSerial;
        unsigned long       m_dwServerID;
    };
}

#endif