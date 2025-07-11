#ifndef _RYL_BILLING_DB_COMPONENTS_H_
#define _RYL_BILLING_DB_COMPONENTS_H_

#include <DB/DBDefine.h>

// forward decl;
class CDBComponent;

namespace DBComponent
{
    namespace BillingDB
    {
		bool InitUserList(CDBComponent& DBComponent);

        // ---------------------------------------------------------------------------------
        // 기존 빌링
        bool USPCheckUser_Login(CDBComponent& DBComponent, char *Account, unsigned long UserID, 
            char Check, char *ClientIP, char Group, RE_USPCheckBilling_Login* lpData_Out);

        bool USPCheckUser_CharIDLogin(CDBComponent& DBComponent, char *Account, unsigned long UserID, 
            char Check, char *ClientIP, char Group, RE_USPCheckBilling_Login* lpData_Out);

        bool USPCheckUser_LogOut(CDBComponent& DBComponent, char *Account, unsigned long* Return_Out);


        // ---------------------------------------------------------------------------------
        // 기존 빌링
        bool USPCheckBilling_Login(CDBComponent& DBComponent, char *Account, unsigned long UserID, 
            char Check, char *ClientIP, char Group, RE_USPCheckBilling_Login* lpData_Out);

        bool USPCheckBilling_CharIDLogin(CDBComponent& DBComponent, char *Account, unsigned long UserID, 
            char Check, char *ClientIP, char Group, RE_USPCheckBilling_Login* lpData_Out);

        bool USPCheckBilling_LogOut(CDBComponent& DBComponent, char *Account, unsigned long* Return_Out);

        bool USPServer_Start(CDBComponent& DBComponent, char Group, unsigned long* Return_Out);
        bool USPServer_End(CDBComponent& DBComponent, char Group, unsigned long* Return_Out);
        bool USPDisConnectLogOut(CDBComponent& DBComponent, char Group, unsigned long* Return_Out);

        bool USPGRoomCurrent_DisConn(CDBComponent& DBComponent, char *Account);

        // ---------------------------------------------------------------------------------
        // 통합빌링
        bool USPCheckBilling_Login_Post(CDBComponent& DBComponent, char *Account, unsigned long UserID, 
            char Check, char *ClientIP, char Group, RE_USPCheckBilling_Login* lpData_Out);

        bool USPCheckBilling_CharIDLogin_Post(CDBComponent& DBComponent, char *Account, unsigned long UserID, char Check, 
            char *ClientIP, char Group, RE_USPCheckBilling_Login* lpData_Out);

        bool USPCheckBilling_LogOut_Post(CDBComponent& DBComponent, char *Account, unsigned long* Return_Out);

        bool USPCheckBilling_InsertCurrentUser_Post(CDBComponent& DBComponent, int nTableType, char* Account, unsigned long UserID,
            char *ClientIP, char Group, char cBillingType, char cCheck, unsigned long* Return_Out);

        // 다음 두 쿼리는, 실행한 후 데이터를 얻어와서 처리할 필요가 있음.
        bool USPDisConnectLogOut_Post(CDBComponent& DBComponent, char Group);
        bool USPCheckTimeoutUser(CDBComponent& DBComponent);

        // ---------------------------------------------------------------------------------
        // 대만빌링
        bool PayAuthMyth(CDBComponent& DBComponent, 
            const char *Account_In, const char* szAddress, PAY_AUTH* lpPayAuth_Out);

		// ---------------------------------------------------------------------------------
		// 일본 오픈베타
		bool USPJapanUserLoginToZone(CDBComponent& DBComponent, char *Account, unsigned long UserID, 
			char Check, char *ClientIP, char Group, unsigned long* dwReturnedFlag);

		bool USPJapanUserLoginToAuth(CDBComponent& DBComponent, char *Account, unsigned long UserID, 
			char Check, char *ClientIP, char Group, unsigned long* dwReturnedFlag);

		bool USPJapanUserLogOut(CDBComponent& DBComponent, char *Account, unsigned long* dwReturnedFlag);
    };
};

#endif