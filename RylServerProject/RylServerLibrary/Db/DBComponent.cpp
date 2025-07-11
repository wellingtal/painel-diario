///////////////////////////////////////////////////////////////////////////////////////////////
//
// CDBComponent
//
//////////////////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <Log/ServerLog.h>

#include <Utility/Setup/ServerSetup.h>
#include <Utility/Math/Math.h>
#include <Utility/Debug/PerformanceCheck.h>	

#include "DBComponent.h"

CDBSingleObject& CDBSingleObject::GetInstance()
{
    static CDBSingleObject dbSingleObject;
    return dbSingleObject;
}

CDBComponent::CDBComponent(void)
{

}

CDBComponent::~CDBComponent()
{

}

bool CDBComponent::Connect(DBClass DBClass_In)
{
    CServerSetup& serverSetup = CServerSetup::GetInstance();

    CServerSetup::DBInfo* lpDBInfo = 0;

    switch (DBClass_In)
    {
    case Class_AuthDB:      lpDBInfo = &serverSetup.GetAuthDBInfo();        break;
    case Class_GameDB:      lpDBInfo = &serverSetup.GetGameDBInfo();        break;
    case Class_KeeperDB:    lpDBInfo = &serverSetup.GetKeeperDBInfo();      break;
    case Class_AdminDB:     lpDBInfo = &serverSetup.GetAdminToolDBInfo();   break;
    }

    if (0 == lpDBInfo)
    {	
        ERRLOG0(g_Log, "DB Connect failed : Unknown dbType");
    }
    else if (!ConnectSQLServer(lpDBInfo->m_szDBAddr, lpDBInfo->m_szDBName, 
        lpDBInfo->m_szDBUser, lpDBInfo->m_szDBPass, lpDBInfo->m_ConnType)) 
	{
        ERRLOG1(g_Log, "DB Connect failed : %s", GetErrorString());
	}
    else
    {
        return true;
    }        

    return false;
}

bool CDBComponent::Connect(const TCHAR* szDBServerName, const TCHAR* szDBName, 
                           const TCHAR* szDBAccount, const TCHAR* szDBPass,
                           OleDB::ConnType connType)
{
//    DETLOG1(g_Log, "DB에 로그인합니다 : DB : %s", szDBName);

//    DETLOG4(g_Log, "DB에 로그인합니다 : %s, DB : %s, Account : %s, Password : %s", 
//      szDBServerName, szDBName, szDBAccount, szDBPass);

    if (!ConnectSQLServer(szDBServerName, szDBName, szDBAccount, szDBPass, connType)) 
    {
        ERRLOG1(g_Log, "DB Connect failed : %s", GetErrorString());
        return false;
    }

    return true;
}


bool CDBComponent::Select(const TCHAR *Query_In, void** lpLoginTable_Out, 
                          int Size_In, int StartNum_In, int RowNum_In, int *GetRowNum_Out)
{
	if (StartNum_In == 0 && !ExecuteQuery(Query_In))
	{
        ERRLOG1(g_Log, "Execute DB Query failed : %s", GetErrorString());
    }
    else if (!GetData((void **)lpLoginTable_Out, Size_In, RowNum_In, GetRowNum_Out))
    {
        ERRLOG1(g_Log, "Get DBdata failed : %s", GetErrorString());
	}
    else 
    {
        return true;
    }

	return false;
}
