#ifndef _RYL_DBAGENT_DB_ADMIN_DATA_H_
#define _RYL_DBAGENT_DB_ADMIN_DATA_H_

#include <winsock2.h>
#include <windows.h>

#include <map>
#include <boost/pool/pool_alloc.hpp>

// forward decl.
class CDBComponent;


namespace DBAgent
{
namespace DataStorage
{

    struct AdminData
    {
        unsigned long   m_dwUID;
        unsigned char   m_cLevel;

        IN_ADDR         m_startIP;
        IN_ADDR         m_endIP;
    };

    class CAdminDataMgr
    {
    public:

        static CAdminDataMgr& GetInstance();

        bool LoadAdminList(CDBComponent& DBComponent);
        unsigned char GetAdminLevel(unsigned long dwUID, IN_ADDR address);

    private:

        typedef std::map<unsigned long, AdminData, std::less<unsigned long>, 
            boost::fast_pool_allocator<std::pair<unsigned long, AdminData> > > AdminMap;

        AdminMap    m_AdminMap;
    };

}
}

#endif