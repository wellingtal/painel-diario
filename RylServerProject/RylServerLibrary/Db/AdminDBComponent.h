#ifndef _RYL_ADMIN_DB_COMPONENT_H_
#define _RYL_ADMIN_DB_COMPONENT_H_

// forward decl.
class CDBComponent;

namespace DBComponent
{
    namespace AdminDB
    {
        bool StartCharChatBan(CDBComponent& DBComponent, unsigned long dwAdminCID, unsigned long dwTargetCID, unsigned long dwMinutes);
        bool GetCharChatBan(CDBComponent& DBComponent, unsigned long dwCID, unsigned long& dwMinutes);
        bool EndCharChatBan(CDBComponent& DBComponent, unsigned long dwCID);
		bool UpdateCharChatBanTime(CDBComponent& DBComponent, unsigned long dwCID, unsigned long dwMinutes);
    };
};

#endif 