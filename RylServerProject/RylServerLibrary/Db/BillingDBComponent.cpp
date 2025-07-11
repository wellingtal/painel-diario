#include "stdafx.h"
#include "DBComponent.h"
#include "BillingDBComponent.h"


#include <Utility/Math/Math.h>
#include <Utility/Debug/PerformanceCheck.h>
#include <Log/ServerLog.h>


bool DBComponent::BillingDB::InitUserList(CDBComponent& DBComponent)
{
	char Query[CDBComponent::QUERY_BUFFER_LEN];
	_snprintf(Query, CDBComponent::QUERY_BUFFER_LEN, "DELETE FROM TblCurrentUser");

	if (DBComponent.ExecuteQuery(Query)) {
		return true;
	}

	return false;
}

// --------------------------------------------------------------------------------------------
// 빌링 관련 (ROW 빌링)

//
// USPCheckUser_Login [ public ]
// - 빌링 로그인
//
// Parameter :
//
// Return :
//	생성 성공 시 true, 실패시 false
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool DBComponent::BillingDB::USPCheckUser_Login(CDBComponent& DBComponent, char *Account, unsigned long UserID, 
                                                 char Check, char *ClientIP, char Group, 
                                                 RE_USPCheckBilling_Login* lpData_Out){
    char Query[CDBComponent::QUERY_BUFFER_LEN];
    _snprintf(Query, CDBComponent::QUERY_BUFFER_LEN, "dbo.USPCheckUser_Login '%s', %d, %d, '%s', %d", 
        Account, UserID, Check, ClientIP, Group);

    if (DBComponent.ExecuteQueryGetData(Query, (void *)lpData_Out)) {
        return true;
    }

    return false;
}

bool DBComponent::BillingDB::USPCheckUser_CharIDLogin(CDBComponent& DBComponent, char *Account, 
                                                       unsigned long UserID, 
                                                       char Check, char *ClientIP, char Group, 
                                                       RE_USPCheckBilling_Login* lpData_Out)
{
    char Query[CDBComponent::QUERY_BUFFER_LEN];
    _snprintf(Query, CDBComponent::QUERY_BUFFER_LEN, "dbo.USPCheckUser_CharIDLogin '%s', %d, %d, '%s', %d", 
        Account, UserID, Check, ClientIP, Group);

    if (DBComponent.ExecuteQueryGetData(Query, (void *)lpData_Out)) {
        return true;
    }

    return false;
}

bool DBComponent::BillingDB::USPCheckUser_LogOut(CDBComponent& DBComponent, 
                                                  char *Account, unsigned long* Return_Out)
{
    char Query[CDBComponent::QUERY_BUFFER_LEN];
    _snprintf(Query, CDBComponent::QUERY_BUFFER_LEN, "dbo.USPCheckUser_LogOut '%s'", Account);

    if (DBComponent.ExecuteQueryGetData(Query, (void *)Return_Out)) {
        return true;
    }

    return false;
}

// --------------------------------------------------------------------------------------------
// 빌링 관련 (기존 빌링)

//
// USPCheckBilling_Login [ public ]
// - 빌링 로그인
//
// Parameter :
//
// Return :
//	생성 성공 시 true, 실패시 false
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool DBComponent::BillingDB::USPCheckBilling_Login(CDBComponent& DBComponent, char *Account, unsigned long UserID, 
                                                 char Check, char *ClientIP, char Group, 
                                                 RE_USPCheckBilling_Login* lpData_Out){
    char Query[CDBComponent::QUERY_BUFFER_LEN];
    _snprintf(Query, CDBComponent::QUERY_BUFFER_LEN, "dbo.USPCheckBilling_Login '%s', %d, %d, '%s', %d", 
        Account, UserID, Check, ClientIP, Group);

    if (DBComponent.ExecuteQueryGetData(Query, (void *)lpData_Out)) {
        return true;
    }

    return false;
}

bool DBComponent::BillingDB::USPCheckBilling_CharIDLogin(CDBComponent& DBComponent, char *Account, 
                                                       unsigned long UserID, 
                                                       char Check, char *ClientIP, char Group, 
                                                       RE_USPCheckBilling_Login* lpData_Out)
{
    char Query[CDBComponent::QUERY_BUFFER_LEN];
    _snprintf(Query, CDBComponent::QUERY_BUFFER_LEN, "dbo.USPCheckBilling_CharIDLogin '%s', %d, %d, '%s', %d", 
        Account, UserID, Check, ClientIP, Group);

    if (DBComponent.ExecuteQueryGetData(Query, (void *)lpData_Out)) {
        return true;
    }

    return false;
}

bool DBComponent::BillingDB::USPCheckBilling_LogOut(CDBComponent& DBComponent, 
                                                  char *Account, unsigned long* Return_Out)
{
    char Query[CDBComponent::QUERY_BUFFER_LEN];
    _snprintf(Query, CDBComponent::QUERY_BUFFER_LEN, "dbo.USPCheckBilling_LogOut '%s'", Account);

    if (DBComponent.ExecuteQueryGetData(Query, (void *)Return_Out)) {
        return true;
    }

    return false;
}

bool DBComponent::BillingDB::USPServer_Start(CDBComponent& DBComponent, char Group, unsigned long* Return_Out)
{
    char Query[CDBComponent::QUERY_BUFFER_LEN];
    _snprintf(Query, CDBComponent::QUERY_BUFFER_LEN, "dbo.USPServer_Start %d", Group);

    if (DBComponent.ExecuteQueryGetData(Query, (void *)Return_Out)) {
        return true;
    }

    return false;
}

bool DBComponent::BillingDB::USPServer_End(CDBComponent& DBComponent, char Group, unsigned long* Return_Out)
{
    char Query[CDBComponent::QUERY_BUFFER_LEN];
    _snprintf(Query, CDBComponent::QUERY_BUFFER_LEN, "dbo.USPServer_End %d", Group);

    if (DBComponent.ExecuteQueryGetData(Query, (void *)Return_Out)) {
        return true;
    }

    return false;
}

bool DBComponent::BillingDB::USPDisConnectLogOut(CDBComponent& DBComponent, char Group, unsigned long* Return_Out)
{
    char Query[CDBComponent::QUERY_BUFFER_LEN];
    _snprintf(Query, CDBComponent::QUERY_BUFFER_LEN, "dbo.USPDisConnectLogOut %d", Group);

    if (DBComponent.ExecuteQueryGetData(Query, (void *)Return_Out)) {
        return true;
    }

    return false;
}

bool DBComponent::BillingDB::USPGRoomCurrent_DisConn(CDBComponent& DBComponent, char *Account)
{
    char Query[CDBComponent::QUERY_BUFFER_LEN];
    _snprintf(Query, CDBComponent::QUERY_BUFFER_LEN, "dbo.USPGRoomCurrent_DisConn '%s'", Account);

    if (DBComponent.ExecuteQuery(Query)) {
        return true;
    }

    return false;
}

// --------------------------------------------------------------------------------------------
// 빌링 관련 (통합 빌링)

bool DBComponent::BillingDB::USPCheckBilling_Login_Post(CDBComponent& DBComponent, char *Account, 
                                                      unsigned long UserID, char Check, 
                                                      char *ClientIP, char Group, RE_USPCheckBilling_Login* lpData_Out)
{
    char Query[CDBComponent::QUERY_BUFFER_LEN];
    _snprintf(Query, CDBComponent::QUERY_BUFFER_LEN, "dbo.USPCheckBilling_Login_Post '%s', %d, %d, '%s', %d", 
        Account, UserID, Check, ClientIP, Group);

    if (DBComponent.ExecuteQueryGetData(Query, (void *)lpData_Out)) {
        return true;
    }

    return false;
}


bool DBComponent::BillingDB::USPCheckBilling_CharIDLogin_Post(CDBComponent& DBComponent, char *Account, 
                                                            unsigned long UserID, char Check, 
                                                            char *ClientIP, char Group, RE_USPCheckBilling_Login* lpData_Out)
{
    char Query[CDBComponent::QUERY_BUFFER_LEN];
    _snprintf(Query, CDBComponent::QUERY_BUFFER_LEN, "dbo.USPCheckBilling_CharIDLogin_Post '%s', %d, %d, '%s', %d", 
        Account, UserID, Check, ClientIP, Group);

    return DBComponent.ExecuteQueryGetData(Query, (void *)lpData_Out);
}

bool DBComponent::BillingDB::USPCheckBilling_LogOut_Post(CDBComponent& DBComponent, char *Account, unsigned long* Return_Out)
{
    char Query[CDBComponent::QUERY_BUFFER_LEN];
    _snprintf(Query, CDBComponent::QUERY_BUFFER_LEN, "dbo.USPCheckBilling_LogOut_Post '%s'", Account);

    return DBComponent.ExecuteQueryGetData(Query, (void *)Return_Out);
}

bool DBComponent::BillingDB::USPDisConnectLogOut_Post(CDBComponent& DBComponent, char Group)
{
    char Query[CDBComponent::QUERY_BUFFER_LEN];
    _snprintf(Query, CDBComponent::QUERY_BUFFER_LEN, "dbo.USPDisConnectLogOut %d", Group);

    return DBComponent.ExecuteQuery(Query);   
}

bool DBComponent::BillingDB::USPCheckTimeoutUser(CDBComponent& DBComponent)
{
    return DBComponent.ExecuteQuery("dbo.USPCheckTimeOutUser");
}

bool DBComponent::BillingDB::USPCheckBilling_InsertCurrentUser_Post(CDBComponent& DBComponent, int nTableType, 
                                                                  char* Account, unsigned long UserID,
                                                                  char *ClientIP, char Group, char cBillingType, 
                                                                  char cCheck, unsigned long* Return_Out)
{
    char Query[CDBComponent::QUERY_BUFFER_LEN];
    _snprintf(Query, CDBComponent::QUERY_BUFFER_LEN, 
        "dbo.USPCheckBilling_InsertCurrentUser_Post %d, '%s', %u, '%s', %d, '%c', %d", 
        nTableType, Account, UserID, ClientIP, Group, cBillingType, cCheck);

    return DBComponent.ExecuteQueryGetData(Query, Return_Out);
}



//Interface/////////////////////////////////////////////////////////////////////////////////////
//
// 요시랜드 인증 ( 포인트 처리 )
//
// Parameter :
//
// Do :
//
// Return :
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool DBComponent::BillingDB::PayAuthMyth(CDBComponent& DBComponent, 
                                         const char *Account_In, const char* szAddress, PAY_AUTH* lpPayAuth_Out)
{
    char Query[CDBComponent::QUERY_BUFFER_LEN];
    _snprintf(Query, CDBComponent::QUERY_BUFFER_LEN, "dbo.spPayAuthMyth '%s', '%s'", 
        Account_In, szAddress);

    if (DBComponent.ExecuteQueryGetData(Query, (void *)lpPayAuth_Out)) {
        return true;
    }

    return false;
}

// 일본 인증 (오픈베타용)
bool DBComponent::BillingDB::USPJapanUserLoginToZone(CDBComponent& DBComponent, char *Account,
													 unsigned long UserID,char Check, char *ClientIP,
													 char Group, unsigned long* dwReturnedFlag)
{
	char Query[CDBComponent::QUERY_BUFFER_LEN];
    _snprintf(Query, CDBComponent::QUERY_BUFFER_LEN, "dbo.USPJapanUserLoginToZone '%s', %d, %d, '%s', %d", 
        Account, UserID, Check, ClientIP, Group);

    return DBComponent.ExecuteQueryGetData(Query, (void*)dwReturnedFlag);
}

bool DBComponent::BillingDB::USPJapanUserLoginToAuth(CDBComponent& DBComponent, char *Account,
													 unsigned long UserID, char Check, char *ClientIP,
													 char Group, unsigned long* dwReturnedFlag)
{
	char Query[CDBComponent::QUERY_BUFFER_LEN];
    _snprintf(Query, CDBComponent::QUERY_BUFFER_LEN, "dbo.USPJapanUserLoginToAuth '%s', %d, %d, '%s', %d", 
        Account, UserID, Check, ClientIP, Group);

    return DBComponent.ExecuteQueryGetData(Query, (void*)dwReturnedFlag);
}

bool DBComponent::BillingDB::USPJapanUserLogOut(CDBComponent& DBComponent, char *Account,
												unsigned long* dwReturnedFlag)
{
	char Query[CDBComponent::QUERY_BUFFER_LEN];
    _snprintf(Query, CDBComponent::QUERY_BUFFER_LEN, "dbo.USPJapanUserLogOut '%s'", Account);

    return DBComponent.ExecuteQueryGetData(Query, (void*)dwReturnedFlag);
}