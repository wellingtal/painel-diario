#include "stdafx.h"
#include "DBComponent.h"
#include "AuthDBComponent.h"


#include <Utility/Math/Math.h>
#include <Utility/Debug/PerformanceCheck.h>
#include <Log/ServerLog.h>

#pragma pack(1)

struct HanLoginData
{
    enum
    {
        ID_LEN = 16,
        PASS_LEN = 32
    };    

    char            m_szAccount[ID_LEN];
    char            m_szPassword[PASS_LEN];
    HAN_UID         m_nUID;        
};

struct GamaLoginData
{
    enum
    {
        ID_LEN = 16,
        PASS_LEN = 32
    };    

    char            m_szAccount[ID_LEN];
    char            m_szPassword[PASS_LEN];
    unsigned long   m_dwUID;
};

struct PCCheckAgreement
{
	enum
	{
		// edith 2008.03.17 ID,PASS 길이조정
		ID_LEN = 24,
	};    

	char            m_szAccount[ID_LEN];
	unsigned long	m_dwRet;
};

struct GamaGetAccountByUID
{
    enum
    {
        ID_LEN = 20
    };

    unsigned long   m_dwUID;
    char            m_szAccount[ID_LEN];
};

struct GamaGetUIDByAccount
{
    enum
    {
        ID_LEN = 12
    };

    char            m_szAccount[ID_LEN];
    unsigned long   m_dwUID;
};


#pragma pack()


class CAuthorizeParam
{
public:

    static CAuthorizeParam& GetInstance();

    OleDB::PARAM_INFO m_HanAuthParam;		// 한게임 인증 파라미터 정보
    OleDB::PARAM_INFO m_HanUpdateParam;	    // 업데이트 한
    OleDB::PARAM_INFO m_HanRegParam;		
	OleDB::PARAM_INFO m_HanAuthCheckParam;	// 이관 신청 여부 확인 정보.

    OleDB::PARAM_INFO m_GamaAuthParam;          // 인증 파라미터
    OleDB::PARAM_INFO m_GamaGetAccountByUID;    // UID로 계정명 얻기
    OleDB::PARAM_INFO m_GamaGetUIDByAccount;    // 계정명으로 UID얻기

private:

    CAuthorizeParam();
};

CAuthorizeParam& CAuthorizeParam::GetInstance()
{
    static CAuthorizeParam authParam;
    return authParam;
}

CAuthorizeParam::CAuthorizeParam()
{
    // 한게임 인증 파라미터
    memset(&m_HanAuthParam, 0, sizeof(OleDB::PARAM_INFO));
    m_HanAuthParam.ColNum		= 3;
    m_HanAuthParam.ColSize[0]	= sizeof(char) * 16;
    m_HanAuthParam.ColType[0]	= DBTYPE_STR;
    m_HanAuthParam.eParamIO[0]	= DBPARAMIO_INPUT;

    m_HanAuthParam.ColSize[1]	= sizeof(char) * 32;
    m_HanAuthParam.ColType[1]	= DBTYPE_STR;
    m_HanAuthParam.eParamIO[1]	= DBPARAMIO_INPUT;

    m_HanAuthParam.ColSize[2]   = sizeof(INT64);
    m_HanAuthParam.ColType[2]   = DBTYPE_I8;
    m_HanAuthParam.eParamIO[2]  = DBPARAMIO_OUTPUT;

    // 한게임 등록 파라미터
    memset(&m_HanRegParam, 0, sizeof(OleDB::PARAM_INFO));
    m_HanRegParam.ColNum		= 2;
    m_HanRegParam.ColSize[0]	= sizeof(char) * 16;
    m_HanRegParam.ColType[0]	= DBTYPE_STR;
    m_HanRegParam.eParamIO[0]   = DBPARAMIO_INPUT;

    m_HanRegParam.ColSize[1]	= sizeof(char) * 3;
    m_HanRegParam.ColType[1]	= DBTYPE_STR;
    m_HanRegParam.eParamIO[1]   = DBPARAMIO_INPUT;

    // 한게임 계정이전 파라미터
	memset(&m_HanAuthCheckParam, 0, sizeof(OleDB::PARAM_INFO));

	m_HanAuthCheckParam.ColNum			= 2;
	m_HanAuthCheckParam.ColSize[0]		= sizeof(char) * 16;
	m_HanAuthCheckParam.ColType[0]		= DBTYPE_STR;
	m_HanAuthCheckParam.eParamIO[0]		= DBPARAMIO_INPUT;

	m_HanAuthCheckParam.ColSize[1]		= sizeof(unsigned long);
	m_HanAuthCheckParam.ColType[1]		= DBTYPE_I4;
	m_HanAuthCheckParam.eParamIO[1]		= DBPARAMIO_OUTPUT;

    // 가마 인증 파라미터
    memset(&m_GamaAuthParam, 0, sizeof(OleDB::PARAM_INFO));
    m_GamaAuthParam.ColNum = 3;

    m_GamaAuthParam.ColSize[0]  = sizeof(char) * GamaLoginData::ID_LEN;
    m_GamaAuthParam.ColType[0]  = DBTYPE_STR;
    m_GamaAuthParam.eParamIO[0] = DBPARAMIO_INPUT;

    m_GamaAuthParam.ColSize[1]  = sizeof(char) * GamaLoginData::PASS_LEN;
    m_GamaAuthParam.ColType[1]  = DBTYPE_STR;
    m_GamaAuthParam.eParamIO[1] = DBPARAMIO_INPUT;

    m_GamaAuthParam.ColSize[2]  = sizeof(unsigned long);
    m_GamaAuthParam.ColType[2]  = DBTYPE_I4;
    m_GamaAuthParam.eParamIO[2] = DBPARAMIO_OUTPUT;

    // UID로 계정명 얻기
    memset(&m_GamaGetAccountByUID, 0, sizeof(OleDB::PARAM_INFO));
    m_GamaGetAccountByUID.ColNum = 2;

    m_GamaGetAccountByUID.ColSize[0]  = sizeof(unsigned long);
    m_GamaGetAccountByUID.ColType[0]  = DBTYPE_I4;
    m_GamaGetAccountByUID.eParamIO[0] = DBPARAMIO_INPUT;
    
    m_GamaGetAccountByUID.ColSize[1]  = sizeof(char) * GamaGetAccountByUID::ID_LEN;
    m_GamaGetAccountByUID.ColType[1]  = DBTYPE_STR;
    m_GamaGetAccountByUID.eParamIO[1] = DBPARAMIO_OUTPUT;

    // 계정명으로 UID얻기
    memset(&m_GamaGetUIDByAccount, 0, sizeof(OleDB::PARAM_INFO));
    m_GamaGetUIDByAccount.ColNum = 2;

    m_GamaGetUIDByAccount.ColSize[0]  = sizeof(char) * GamaGetUIDByAccount::ID_LEN;
    m_GamaGetUIDByAccount.ColType[0]  = DBTYPE_STR;
    m_GamaGetUIDByAccount.eParamIO[0] = DBPARAMIO_INPUT;

    m_GamaGetUIDByAccount.ColSize[1]  = sizeof(unsigned long);
    m_GamaGetUIDByAccount.ColType[1]  = DBTYPE_I4;
    m_GamaGetUIDByAccount.eParamIO[1] = DBPARAMIO_OUTPUT;
}


//Interface/////////////////////////////////////////////////////////////////////////////////////
//
// 일본 인증
//
// Parameter :
//
// Do :
//
// Return :
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool DBComponent::AuthDB::LoginAuthJapan(CDBComponent& DBComponent, const char *Account_In, 
					const char *Password_In, const char *szAddress, AUTH* lpAuthUser_Out)
{
	char Query[CDBComponent::QUERY_BUFFER_LEN];
	_snprintf(Query, CDBComponent::QUERY_BUFFER_LEN, "LoginUserStatus '%s', '%s', '%s'", 
		Account_In, Password_In, szAddress);

	if (DBComponent.ExecuteQueryGetData(Query, (void *)lpAuthUser_Out)) {
		return true;
	}

	return false;
}

//Interface/////////////////////////////////////////////////////////////////////////////////////
//
// 요시랜드 인증
//
// Parameter :
//
// Do :
//
// Return :
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool DBComponent::AuthDB::LoginAuthMyth(CDBComponent& DBComponent, 
                                        const char *Account_In, const char *Password_In, 
                                        const char *szAddress, AUTH* lpAuthUser_Out)
{
    char Query[CDBComponent::QUERY_BUFFER_LEN];
    _snprintf(Query, CDBComponent::QUERY_BUFFER_LEN, "dbo.spLoginAuthMyth '%s', '%s', '%s'", 
        Account_In, Password_In, szAddress);

    if (DBComponent.ExecuteQueryGetData(Query, (void *)lpAuthUser_Out)) {
        return true;
    }

    return false;
}

bool DBComponent::AuthDB::GetPCCheckAgreement(CDBComponent& DBComponent, const char* szAccount_In, unsigned long* dwNumber_Out)
{
	PCCheckAgreement pcCheckAgreement;
	memset(&pcCheckAgreement, 0, sizeof(PCCheckAgreement));

	strncpy(pcCheckAgreement.m_szAccount, szAccount_In, PCCheckAgreement::ID_LEN);	

	if(DBComponent.ExecuteQueryWithParams("{call PC_CHECK_AGREEMENT(?, ?)}", reinterpret_cast<char*>(&pcCheckAgreement), CAuthorizeParam::GetInstance().m_HanAuthCheckParam))
	{
		*dwNumber_Out = pcCheckAgreement.m_dwRet;
		return true;
	}

	return false;
}

//Interface/////////////////////////////////////////////////////////////////////////////////////
//
// 한게임 인증
//
// Parameter :
//
// Do :
//
// Return :
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool DBComponent::AuthDB::LoginAuthHan(CDBComponent& DBComponent, 
                                       const char* szAccount, const char* szPassword, HAN_UID& hanUID)   
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    HanLoginData loginData;
    memset(&loginData, 0, sizeof(HanLoginData));

    strncpy(loginData.m_szAccount, szAccount, HanLoginData::ID_LEN);
    strncpy(loginData.m_szPassword, szPassword, HanLoginData::PASS_LEN);

    if( DBComponent.ExecuteQueryWithParams("{call pc_ryl_login(?, ?, ?)}", 
        reinterpret_cast<char*>(&loginData), CAuthorizeParam::GetInstance().m_HanAuthParam))
    {   
        hanUID = loginData.m_nUID;
        return true;
    }

    return false;
}

//Interface/////////////////////////////////////////////////////////////////////////////////////
//
// 릴 게임 등록
//
// Parameter :
//
// Do :
//
// Return :
//
///////////////////////////////////////////////////////////////////////////////////////////////
typedef struct _REG_HAN
{
    char Name[16];
    char Reg[3];
}REG_HAN, *LPREG_HAN;

bool DBComponent::AuthDB::RegRylGameHan(CDBComponent& DBComponent, char *Account_In, char *Mail_In)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    char Query[CDBComponent::QUERY_BUFFER_LEN];

    REG_HAN Reg = {0,};
    strcpy(Reg.Name, Account_In);
    strcpy(Reg.Reg, Mail_In);
    _snprintf(Query, CDBComponent::QUERY_BUFFER_LEN, "{call pc_ryl_login2(?, ?)}");

    if( DBComponent.ExecuteQueryWithParams(Query, (char *)&Reg, CAuthorizeParam::GetInstance().m_HanRegParam))
    {
        return true;
    }

    return false;
}

bool DBComponent::AuthDB::GetAccountByUID(CDBComponent& DBComponent, unsigned long UID_In, char *Account_Out)
{
    char Query[CDBComponent::QUERY_BUFFER_LEN];

    sprintf(Query, "select FC_GETMEMBERID_BYUID(%d) from dual", UID_In);

    if(DBComponent.ExecuteQueryGetData(Query, Account_Out))
        return true;

    return false;
}

bool DBComponent::AuthDB::GetUIDByAccount(CDBComponent& DBComponent, char *Account_In, char *UID_Out)
{
    char Query[CDBComponent::QUERY_BUFFER_LEN];

    sprintf(Query, "select FC_GETUID_BYMEMBERID('%s') from dual", Account_In);

    if(DBComponent.ExecuteQueryGetData(Query, UID_Out))
        return true;

    return false;
}


bool DBComponent::AuthDB::GetMythAccountByUID(CDBComponent& DBComponent, unsigned long UID_In, char *Account_Out)
{
    char szQuery[CDBComponent::QUERY_BUFFER_LEN];
    char szResults[CDBComponent::QUERY_BUFFER_LEN];
    memset(szResults, 0, sizeof(szResults));

    if (0 < _snprintf(szQuery, CDBComponent::QUERY_BUFFER_LEN - 1, 
        "dbo.sp_SearchUserAccount %d, NULL", UID_In) &&        
        DBComponent.ExecuteQueryGetData(szQuery, szResults))
    {
        memcpy(Account_Out, szResults + sizeof(unsigned long), 
            sizeof(char) * HanLoginData::ID_LEN);

        return true;
    }

    return false;
}

bool DBComponent::AuthDB::GetMythUIDByAccount(CDBComponent& DBComponent, const char *Account_In, unsigned long *UID_Out)
{
    char szQuery[CDBComponent::QUERY_BUFFER_LEN];
    char szResults[CDBComponent::QUERY_BUFFER_LEN];
    memset(szResults, 0, sizeof(szResults));

    if (0 < _snprintf(szQuery, CDBComponent::QUERY_BUFFER_LEN - 1,
        "dbo.sp_SearchUserAccount NULL, '%s'", Account_In) &&
        DBComponent.ExecuteQueryGetData(szQuery, szResults))
    {		
        memcpy(UID_Out, szResults, sizeof(unsigned long));
        return true;
    }

    return false;
}


//Interface/////////////////////////////////////////////////////////////////////////////////////
//
// 블럭된 계정인지 확인
//
// Parameter :
//
// Do :
//
// Return :
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool DBComponent::AuthDB::CheckBlockedUser(CDBComponent& DBComponent, unsigned long UID_In, 
                                         unsigned char cServerID, LPRE_CMS_CheckUserBlock lpData_Out)
{
    char Query[CDBComponent::QUERY_BUFFER_LEN];

    if(0 < _snprintf(Query, CDBComponent::QUERY_BUFFER_LEN, "dbo.USPCheckRestraintChar '%d', '%d'", cServerID, UID_In))
    {
        if(DBComponent.ExecuteQueryGetData(Query, lpData_Out))
        {
            return true;
        }
    }

    return false;
}


bool DBComponent::AuthDB::LoginBattleAuthHan(CDBComponent& DBComponent, 
                                      const char* szAccount, const char* szPassword, HAN_UID& hanUID)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    HanLoginData loginData;
    memset(&loginData, 0, sizeof(HanLoginData));

    strncpy(loginData.m_szAccount, szAccount, HanLoginData::ID_LEN);
    strncpy(loginData.m_szPassword, szPassword, HanLoginData::PASS_LEN);

    if (DBComponent.ExecuteQueryWithParams("{call pc_rylbl_login(?, ?, ?)}", 
        reinterpret_cast<char*>(&loginData), CAuthorizeParam::GetInstance().m_HanAuthParam))
    {   
        hanUID = loginData.m_nUID;
        return true;
    }

    return false;
}

bool DBComponent::AuthDB::RegBattleRylGameHan(CDBComponent& DBComponent, char* szAccount_In, char* szMail_In)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    REG_HAN Reg = {0,};
    strncpy(Reg.Name, szAccount_In, 16);
    strncpy(Reg.Reg, szMail_In, 3);

    return DBComponent.ExecuteQueryWithParams("{call pc_rylbl_login2(?, ?)}", (char *)&Reg, 
        CAuthorizeParam::GetInstance().m_HanRegParam);
}


bool DBComponent::AuthDB::GetBattleAccountByUID(CDBComponent& DBComponent, unsigned long UID_In, char *Account_Out)
{
    char Query[CDBComponent::QUERY_BUFFER_LEN];
    sprintf(Query, "select FC_RYLBL_GETMEMBERID_BYUID(%d) from dual", UID_In);

    return DBComponent.ExecuteQueryGetData(Query, Account_Out);
}

bool DBComponent::AuthDB::GetBattleUIDByAccount(CDBComponent& DBComponent, char *Account_In, char *UID_Out)
{
    char Query[CDBComponent::QUERY_BUFFER_LEN];
    sprintf(Query, "select FC_RYLBL_GETUID_BYMEMBERID('%s') from dual", Account_In);

    return DBComponent.ExecuteQueryGetData(Query, UID_Out);
}

bool DBComponent::AuthDB::GetUIDFromBattleUID(CDBComponent& DBComponent, unsigned long dwBattleUID, unsigned long* lpUID)
{
    char Query[CDBComponent::QUERY_BUFFER_LEN];
    sprintf(Query, "USPGetUID %d", dwBattleUID);

    return DBComponent.ExecuteQueryGetData(Query, lpUID);
}


bool DBComponent::AuthDB::GetBattleUIDFromUID(CDBComponent& DBComponent, unsigned long dwUID, unsigned long* lpBattleUID)
{
    char Query[CDBComponent::QUERY_BUFFER_LEN];
    sprintf(Query, "USPGetUIDBattle %d", dwUID);

    return DBComponent.ExecuteQueryGetData(Query, lpBattleUID);
}


// 가마 독자 서비스 관련 인증
bool DBComponent::AuthDB::LoginAuthGama(CDBComponent& DBComponent, 
                                        const char* szAccount, const char* szPassword, unsigned long& dwUID)
{
    GamaLoginData gamaLoginData;
    memset(&gamaLoginData, 0, sizeof(gamaLoginData));

    strncpy(gamaLoginData.m_szAccount, szAccount, GamaLoginData::ID_LEN);
    strncpy(gamaLoginData.m_szPassword, szPassword, GamaLoginData::PASS_LEN);

    if (DBComponent.ExecuteQueryWithParams("Client_RYL_Login ?, ?, ? OUTPUT", 
        reinterpret_cast<char*>(&gamaLoginData), CAuthorizeParam::GetInstance().m_GamaAuthParam))
    {
        dwUID = gamaLoginData.m_dwUID;
        return true;
    }

    ERRLOG1(g_Log, "DB에러  : 로그인 프로시저 호출 실패. %s", DBComponent.GetErrorString());
    return false;
}

bool DBComponent::AuthDB::GetGamaAccountByUID(CDBComponent& DBComponent, 
                                              unsigned long dwUID, char* szAccount_Out, int nAccountBufferLen)
{
    GamaGetAccountByUID gamaGetAccountByUID;
    memset(&gamaGetAccountByUID, 0, sizeof(gamaGetAccountByUID));

    gamaGetAccountByUID.m_dwUID = dwUID;

    if (DBComponent.ExecuteQueryWithParams("Client_RYL_GETMIDBYUID ?, ? OUTPUT",
        reinterpret_cast<char*>(&gamaGetAccountByUID), CAuthorizeParam::GetInstance().m_GamaGetAccountByUID))
    {
        _snprintf(szAccount_Out, nAccountBufferLen, gamaGetAccountByUID.m_szAccount);
        szAccount_Out[nAccountBufferLen - 1] = 0;
        return true;
    }

    ERRLOG1(g_Log, "DB에러  : UID로 계정명 얻어오는 프로시저 호출 실패. %s", DBComponent.GetErrorString());
    return false;
}

bool DBComponent::AuthDB::GetGamaUIDByAccount(CDBComponent& DBComponent, 
                                              const char* szAccount, unsigned long& dwUID)
{
    GamaGetUIDByAccount gamaGetUIDByAccount;
    memset(&gamaGetUIDByAccount, 0, sizeof(gamaGetUIDByAccount));

    strncpy(gamaGetUIDByAccount.m_szAccount, szAccount, GamaGetUIDByAccount::ID_LEN);

    if (DBComponent.ExecuteQueryWithParams("Client_RYL_GETUIDBYMID ?, ? OUTPUT",
        reinterpret_cast<char*>(&gamaGetUIDByAccount), CAuthorizeParam::GetInstance().m_GamaGetUIDByAccount))
    {
        dwUID = gamaGetUIDByAccount.m_dwUID;
        return true;
    }

    ERRLOG1(g_Log, "DB에러  : 계정명으로 UID 얻어오는 프로시저 호출 실패. %s", DBComponent.GetErrorString());
    return false;
}