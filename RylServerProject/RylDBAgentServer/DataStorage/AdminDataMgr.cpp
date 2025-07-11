#include "stdafx.h"
#include "AdminDataMgr.h"

#include <DB/DBdefine.h>
#include <DB/DBComponent.h>

#include <Log/ServerLog.h>
#include <Utility/Setup/ServerSetup.h>

namespace DBAgent
{
namespace DataStorage
{


CAdminDataMgr& CAdminDataMgr::GetInstance()
{
    static CAdminDataMgr adminDataMgr;
    return adminDataMgr;
}

bool CAdminDataMgr::LoadAdminList(CDBComponent& DBComponent)
{
    const int MAX_LIST = 100;

    ADMIN   AdminList[MAX_LIST];
    int		AdminNum = 0;

    memset(&AdminList, 0, sizeof(ADMIN) * MAX_LIST);

    if(!DBComponent.ExecuteQuery("SELECT * FROM AdminList"))
    {
        ERRLOG1(g_Log, "Operators Failed to get DB information. : %s", 
            DBComponent.GetErrorString());

        return false;
    }

    m_AdminMap.clear();

    while(DBComponent.GetData((void **)AdminList, sizeof(ADMIN), MAX_LIST, &AdminNum))
    {
        if(0 == AdminNum)
        {
            break;
        }

        ADMIN* lpAdminPos = AdminList;
        ADMIN* lpAdminEnd = AdminList + AdminNum;

        for(;lpAdminPos != lpAdminEnd; ++lpAdminPos)
        {
            AdminData adminData;
            adminData.m_dwUID  = lpAdminPos->UID;
            adminData.m_cLevel = static_cast<unsigned char>(lpAdminPos->Level);

            adminData.m_startIP.S_un.S_addr = inet_addr(lpAdminPos->IP_S);
            adminData.m_endIP.S_un.S_addr   = inet_addr(lpAdminPos->IP_E);

            m_AdminMap.insert(AdminMap::value_type(adminData.m_dwUID, adminData));
        }

        memset(&AdminList, 0, sizeof(ADMIN) * MAX_LIST);
    }

    return true;
}

unsigned char CAdminDataMgr::GetAdminLevel(unsigned long dwUID, IN_ADDR address)
{
    AdminMap::iterator pos = m_AdminMap.find(dwUID);
    if(pos != m_AdminMap.end())
    {
        AdminData& admin = pos->second;

        if(admin.m_dwUID == dwUID)
        {
            if(CServerSetup::GetInstance().GetAdminIPCheck())
            {
                if(admin.m_startIP.S_un.S_un_b.s_b1 == address.S_un.S_un_b.s_b1 && 
                    admin.m_startIP.S_un.S_un_b.s_b2 == address.S_un.S_un_b.s_b2 &&
                    admin.m_startIP.S_un.S_un_b.s_b3 == address.S_un.S_un_b.s_b3 && 
                    admin.m_startIP.S_un.S_un_b.s_b4 <= address.S_un.S_un_b.s_b4 &&
                    address.S_un.S_un_b.s_b4 <= admin.m_endIP.S_un.S_un_b.s_b4)
                {
                    return admin.m_cLevel;
                }
            }
            else
            {
                return admin.m_cLevel;
            }
        }
    }

    return 0;
}

}
}
