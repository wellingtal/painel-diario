#include "stdafx.h"
#include "DBComponent.h"
#include "GameDBComponent.h"

#include <Utility/Math/Math.h>
#include <Utility/Debug/PerformanceCheck.h>
#include <Log/ServerLog.h>

#include <atltime.h>

namespace DBComponent
{
namespace GameDB
{

class CGameDBParams
{
public:

    static CGameDBParams& GetInstance();

    const OleDB::PARAM_INFO& GetCharCreateParam() const { return m_CharCreateParam; }
    const OleDB::PARAM_INFO& GetCharChangeNameParam() const { return m_CharChangeNameParam; }
    
private:

    OleDB::PARAM_INFO   m_CharCreateParam;
    OleDB::PARAM_INFO   m_CharChangeNameParam;

    CGameDBParams();
};


CGameDBParams& CGameDBParams::GetInstance()
{
    static CGameDBParams gameDBParams;
    return gameDBParams;
}

CGameDBParams::CGameDBParams()
{
    // 캐릭터 생성 파라미터 시작
    memset(&m_CharCreateParam, 0, sizeof(OleDB::PARAM_INFO));
    m_CharCreateParam.ColNum    = 21;

    // @user_uid	    AS INT,				// 유저 고유 아이디			
    m_CharCreateParam.ColSize[0]    = sizeof(unsigned long);
    m_CharCreateParam.ColType[0]    = DBTYPE_I4;
    m_CharCreateParam.eParamIO[0]   = DBPARAMIO_INPUT;

    // @user_pos	    AS INT,				// 캐릭터 위치 (3개의 슬롯)		
    m_CharCreateParam.ColSize[1]    = sizeof(unsigned long);
    m_CharCreateParam.ColType[1]    = DBTYPE_I4;
    m_CharCreateParam.eParamIO[1]   = DBPARAMIO_INPUT;

    // @user_name	    AS VARCHAR(16),		// 캐릭터 이름				
    m_CharCreateParam.ColSize[2]    = sizeof(char) * 16;
    m_CharCreateParam.ColType[2]    = DBTYPE_STR;
    m_CharCreateParam.eParamIO[2]   = DBPARAMIO_INPUT;

    // @user_Sex	    AS TINYINT,			// 캐릭터 성				
    m_CharCreateParam.ColSize[3]    = sizeof(unsigned char);
    m_CharCreateParam.ColType[3]    = DBTYPE_UI1;
    m_CharCreateParam.eParamIO[3]   = DBPARAMIO_INPUT;

    // @user_Hair	    AS TINYINT,			// 캐릭터 머리 모양			
    m_CharCreateParam.ColSize[4]    = sizeof(unsigned char);
    m_CharCreateParam.ColType[4]    = DBTYPE_UI1;
    m_CharCreateParam.eParamIO[4]   = DBPARAMIO_INPUT;

    // @user_Face	    AS TINYINT,			// 캐릭터 얼굴 모양			
    m_CharCreateParam.ColSize[5]    = sizeof(unsigned char);
    m_CharCreateParam.ColType[5]    = DBTYPE_UI1;
    m_CharCreateParam.eParamIO[5]   = DBPARAMIO_INPUT;

    // @user_Race	AS TINYINT,			// 캐릭터 종족
    m_CharCreateParam.ColSize[6]    = sizeof(unsigned char);
    m_CharCreateParam.ColType[6]    = DBTYPE_UI1;
    m_CharCreateParam.eParamIO[6]   = DBPARAMIO_INPUT;

    // @user_Class	    AS SMALLINT,		// 캐릭터 클래스			
    m_CharCreateParam.ColSize[7]    = sizeof(short);
    m_CharCreateParam.ColType[7]    = DBTYPE_I2;
    m_CharCreateParam.eParamIO[7]   = DBPARAMIO_INPUT;

    // @C_Gold	        AS INT,			    // 캐릭터 소지금								
    m_CharCreateParam.ColSize[8]    = sizeof(unsigned long);
    m_CharCreateParam.ColType[8]    = DBTYPE_I4;
    m_CharCreateParam.eParamIO[8]   = DBPARAMIO_INPUT;

    // @C_STR		    AS SMALLINT,		// 캐릭터 STR				
    m_CharCreateParam.ColSize[9]    = sizeof(short);
    m_CharCreateParam.ColType[9]    = DBTYPE_I2;
    m_CharCreateParam.eParamIO[9]   = DBPARAMIO_INPUT;

    // @C_DEX		    AS SMALLINT,		// 캐릭터 DEX				
    m_CharCreateParam.ColSize[10]    = sizeof(short);
    m_CharCreateParam.ColType[10]    = DBTYPE_I2;
    m_CharCreateParam.eParamIO[10]   = DBPARAMIO_INPUT;

    // @C_CON		    AS SMALLINT,		// 캐릭터 CON				
    m_CharCreateParam.ColSize[11]    = sizeof(short);
    m_CharCreateParam.ColType[11]    = DBTYPE_I2;
    m_CharCreateParam.eParamIO[11]   = DBPARAMIO_INPUT;

    // @C_INT		    AS SMALLINT,		// 캐릭터 INT				
    m_CharCreateParam.ColSize[12]    = sizeof(short);
    m_CharCreateParam.ColType[12]    = DBTYPE_I2;
    m_CharCreateParam.eParamIO[12]   = DBPARAMIO_INPUT;

    // @C_WIS		    AS SMALLINT,		// 캐릭터 WIS				
    m_CharCreateParam.ColSize[13]    = sizeof(short);
    m_CharCreateParam.ColType[13]    = DBTYPE_I2;
    m_CharCreateParam.eParamIO[13]   = DBPARAMIO_INPUT;

    // @PointX		    AS REAL,			// 캐릭터 Point X 좌표			
    m_CharCreateParam.ColSize[14]    = sizeof(float);
    m_CharCreateParam.ColType[14]    = DBTYPE_R4;
    m_CharCreateParam.eParamIO[14]   = DBPARAMIO_INPUT;

    // @PointY		    AS REAL,			// 캐릭터 Point Y 좌표			
    m_CharCreateParam.ColSize[15]    = sizeof(float);
    m_CharCreateParam.ColType[15]    = DBTYPE_R4;
    m_CharCreateParam.eParamIO[15]   = DBPARAMIO_INPUT;

    // @PointZ		    AS REAL,			// 캐릭터 Point Z 좌표			
    m_CharCreateParam.ColSize[16]    = sizeof(float);
    m_CharCreateParam.ColType[16]    = DBTYPE_R4;
    m_CharCreateParam.eParamIO[16]   = DBPARAMIO_INPUT;

    // @C_Equip	    AS varBINARY(30),	    // 장비 (2 * 15 = 30)			
    m_CharCreateParam.ColSize[17]    = sizeof(unsigned short) * 15;
    m_CharCreateParam.ColType[17]    = DBTYPE_BYTES;
    m_CharCreateParam.eParamIO[17]   = DBPARAMIO_INPUT;

    // @Skill		    AS varBinary(164),   // 스킬
    m_CharCreateParam.ColSize[18]    = sizeof(char) * 164;
    m_CharCreateParam.ColType[18]    = DBTYPE_BYTES;
    m_CharCreateParam.eParamIO[18]   = DBPARAMIO_INPUT;

    // @servergroupid	TINYINT,            // 서버군 ID
    m_CharCreateParam.ColSize[19]    = sizeof(unsigned char);
    m_CharCreateParam.ColType[19]    = DBTYPE_UI1;
    m_CharCreateParam.eParamIO[19]   = DBPARAMIO_INPUT;

    // @UID		    INT	OUTPUT              // 실패값 혹은 CID
    m_CharCreateParam.ColSize[20]    = sizeof(unsigned long);
    m_CharCreateParam.ColType[20]    = DBTYPE_I4;
    m_CharCreateParam.eParamIO[20]   = DBPARAMIO_OUTPUT;    

    // 캐릭터 생성 파라미터 끝
    memset(&m_CharChangeNameParam, 0, sizeof(OleDB::PARAM_INFO));

    m_CharChangeNameParam.ColNum        = 2;
    
    m_CharChangeNameParam.ColSize[0]    = sizeof(char) * CHAR_INFOST::MAX_NAME_LEN;
    m_CharChangeNameParam.ColType[0]    = DBTYPE_STR;
    m_CharChangeNameParam.eParamIO[0]   = DBPARAMIO_INPUT;

    m_CharChangeNameParam.ColSize[1]    = sizeof(int);
    m_CharChangeNameParam.ColType[1]    = DBTYPE_I4;
    m_CharChangeNameParam.eParamIO[1]   = DBPARAMIO_INPUT;
}


///////////////////////////////////////////////////////////////////////////////////////////////
//
// InsertUser [ public ]
// - 유저 추가
//
// Parameter :
//	1st : 추가 테이블[In]
//
// Return :
//	생성 성공 시 true, 실패시 false
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool InsertUser(CDBComponent& DBComponent, unsigned long UserID_In, 
                unsigned char cOldServerGroupID, bool bSupressLog)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    char szQuery[CDBComponent::QUERY_BUFFER_LEN];

    if (_snprintf(szQuery, CDBComponent::QUERY_BUFFER_LEN - 1, "InsertUser %d, %d", 
        UserID_In, cOldServerGroupID))
    {
        if (DBComponent.ExecuteQuery(szQuery)) 
        {
            return true;
        }
    }

    if(!bSupressLog)
    {
        szQuery[CDBComponent::QUERY_BUFFER_LEN - 1] = 0;
        ERRLOG3(g_Log, "UID:%10u / %s : %s", UserID_In, szQuery, DBComponent.GetErrorString());	
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
// UpdateUser [ public ]
// - 유저 업데이트
//
// Parameter :
//	1st : 수정 테이블[In]
//
// Return :
//	생성 성공 시 true, 실패시 false
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool UpdateUser(CDBComponent& DBComponent, unsigned long dwUID, USER_INFO* lpUserInfo)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    char szQuery[CDBComponent::QUERY_BUFFER_LEN];

    // WORK_LIST 3.2 정섭 슬롯 5개로 늘리기
    // WORK_LIST 2.1 계정 국적 추가
    if (0 < _snprintf(szQuery, CDBComponent::QUERY_BUFFER_LEN - 1, 
        "dbo.UpdateUserInfo %d, %d, %d, %d, %d, %d, %d, %d", 
        dwUID, lpUserInfo->OldServerGroupID, lpUserInfo->Nation, 
        lpUserInfo->CharID[0], lpUserInfo->CharID[1], 
        lpUserInfo->CharID[2], lpUserInfo->CharID[3], lpUserInfo->CharID[4]))
    {
        if (DBComponent.ExecuteQuery(szQuery))
        {
            return true;
        }
    }

    szQuery[CDBComponent::QUERY_BUFFER_LEN - 1] = 0;
    ERRLOG3(g_Log, "UID:%10u / %s : %s", dwUID, szQuery, DBComponent.GetErrorString());	
    return false;
}

bool UpdateUserFirstLogin(CDBComponent& DBComponent, unsigned long dwUID, 
                          unsigned char cFirstLoginType)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    char szQuery[CDBComponent::QUERY_BUFFER_LEN];

    if (0 < _snprintf(szQuery, CDBComponent::QUERY_BUFFER_LEN - 1,
        "UpdateUserFirstLogin %d, %d", dwUID, cFirstLoginType))
    {
        if (DBComponent.ExecuteQuery(szQuery))
        {
            return true;
        }
    }

    szQuery[CDBComponent::QUERY_BUFFER_LEN - 1] = 0;
    ERRLOG3(g_Log, "UID:%10u / %s : %s", dwUID, szQuery, DBComponent.GetErrorString());	
    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
// UpdateCharZone [ public ]
// - 유저 업데이트
//
// Parameter :
//	1st : 수정 테이블[In]
//
// Return :
//	생성 성공 시 true, 실패시 false
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool UpdateCharZone(CDBComponent& DBComponent, unsigned long UserID_In, unsigned long Zone_In)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    sprintf(DBComponent.GetQueryBuffer(), "UpdateLastZone %d, %d", UserID_In, Zone_In);
    if(DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer()))
        return true;	

    return false;
}

// WORK_LIST 2.1 계정 국적 추가
///////////////////////////////////////////////////////////////////////////////////////////////
//
// UpdateCharZone [ public ]
// - 유저 업데이트
//
// Parameter :
//	1st : 수정 테이블[In]
//
// Return :
//	생성 성공 시 true, 실패시 false
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool UpdateAccountNation(CDBComponent& DBComponent, unsigned long UserID_In, 
                         unsigned char AccountNation_In, unsigned char cOldServerGroupID)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	sprintf(DBComponent.GetQueryBuffer(), "UpdateUserInfoNation %d, %d, %d", 
        UserID_In, AccountNation_In, cOldServerGroupID);

	if (DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer()))
		return true;

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
// InsertAdmin [ public ]
// - 운영자 추가
//
// Parameter :
//
// Return :
//	생성 성공 시 true, 실패시 false
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool InsertAdmin(CDBComponent& DBComponent, unsigned long UserID_In)
{
    char Query[CDBComponent::QUERY_BUFFER_LEN];
    _snprintf(Query, CDBComponent::QUERY_BUFFER_LEN, "InsertAdmin %d", UserID_In );

    if (DBComponent.ExecuteQuery(Query)) {
        return true;
    }

    SERLOG2(g_Log, "UID:%10u %s", UserID_In, DBComponent.GetErrorString());	// cppis 가 플러쉬없이 바로 로그하기위해서 추가함
    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
// DeleteAdmin [ public ]
// - 운영자 삭제
//
// Parameter :
//
// Return :
//	생성 성공 시 true, 실패시 false
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool DeleteAdmin(CDBComponent& DBComponent, unsigned long UserID_In)
{
    char Query[CDBComponent::QUERY_BUFFER_LEN];
    _snprintf(Query, CDBComponent::QUERY_BUFFER_LEN, "DeleteAdmin %d", UserID_In);

    if (DBComponent.ExecuteQuery(Query)) {
        return true;
    }

    SERLOG2(g_Log, "UID:%10u %s", UserID_In, DBComponent.GetErrorString());	// cppis 가 플러쉬없이 바로 로그하기위해서 추가함
    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
// IsAdmin [ public ]
// - 운영자 확인
//
// Parameter :
//
// Return :
//	생성 성공 시 true, 실패시 false
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool IsAdmin(CDBComponent& DBComponent, unsigned long UserID_In)
{
    unsigned long Result = 0;

    char szQuery[CDBComponent::MAX_QUERY_LENGTH];
    _snprintf(szQuery, CDBComponent::MAX_QUERY_LENGTH, "IsAdmin %d", UserID_In);

    if (DBComponent.ExecuteQueryGetData(szQuery, (void *)&Result)) {
        if (1 == Result) 
        {
            return true;
        }

        return false;
    }

    SERLOG3(g_Log, "UID:%10u / %s : %s", UserID_In, szQuery, DBComponent.GetErrorString());	
    return false;
}


///////////////////////////////////////////////////////////////////////////////////////////////
//
// GetUserInfo [ public ]
// - 유저 정보 얻기
//
// Parameter :
//	1st : 유저 아이디[In]
//	2st : 유저 정보 테이블[Out]
//
// Return :
//	생성 성공 시 true, 실패시 false
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool GetUserInfo(CDBComponent& DBComponent, unsigned long UserID_In, 
                 USER_INFO& UserInfo_Out, unsigned char cOldServerGroupID, bool bSupressLog)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    memset(&UserInfo_Out, 0, sizeof(USER_INFO));

    char szQuery[CDBComponent::QUERY_BUFFER_LEN];
    if (0 < _snprintf(szQuery, CDBComponent::QUERY_BUFFER_LEN - 1, 
        "dbo.GetUserInfo %d, %d", UserID_In, cOldServerGroupID))
    {
        if (DBComponent.ExecuteQueryGetData(szQuery, &UserInfo_Out)) 
        {
            return true;
        }
    }

    if (!bSupressLog)
    {
        szQuery[CDBComponent::QUERY_BUFFER_LEN - 1] = 0;
        SERLOG4(g_Log, "UID:%10u / OldServerGroup:%d / %s : %s", 
            UserID_In, cOldServerGroupID, szQuery, DBComponent.GetErrorString());	
    }

    return false;
}


///////////////////////////////////////////////////////////////////////////////////////////////
//
// GetCharView [ public ]
// - 캐릭터 뷰 얻기
//
// Parameter :
//	1st : 캐릭터 아이디[In]
//	2st : 캐릭터 로그인 테이블[Out]
//
// Return :
//	생성 성공 시 true, 실패시 false
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool GetCharView(CDBComponent& DBComponent, unsigned long CharID_In, CHAR_VIEW* lpCharLogin_Out)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    memset(lpCharLogin_Out, 0, sizeof(CHAR_VIEW));

    _snprintf(DBComponent.GetQueryBuffer(), CDBComponent::MAX_QUERY_LENGTH, "dbo.GetCharView %d", CharID_In);

    if (DBComponent.ExecuteQueryGetData(DBComponent.GetQueryBuffer(), (void *)lpCharLogin_Out)) {
        return true;
    }

    SERLOG2(g_Log, "CID:%10u %s", CharID_In, DBComponent.GetErrorString());	// cppis 가 플러쉬없이 바로 로그하기위해서 추가함
    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
// GetCharInfoEx [ public ]
// - 캐릭터 추가 정보 얻기
//
// Parameter :
//	1st : 캐릭터 아이디[In]
//	2st : 캐릭터 정보 테이블[Out]
//
// Return :
//	생성 성공 시 true, 실패시 false
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool GetCharInfoEx(CDBComponent& DBComponent, unsigned long CharID_In, CHAR_INFOEX* lpCharInfoEx_Out)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    memset(lpCharInfoEx_Out, 0, sizeof(CHAR_INFOEX));

    char Query[CDBComponent::QUERY_BUFFER_LEN];
    _snprintf(Query, CDBComponent::QUERY_BUFFER_LEN, 
        "SELECT Total, ServerID, NameChangeCount, AdminFlag, GuildWarFlag, RealmWarFlag, RealmPoint, RealmCheckPoint, RealmMinute, LogoutTime FROM CharInfoEx WHERE CID = %d", CharID_In);

    if (DBComponent.ExecuteQueryGetData(Query, (void *)lpCharInfoEx_Out))
    {
        return true;
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
// UpdateCharInfoEx [ public ]
// - 캐릭터 추가 정보 업데이트
//
// Parameter :
//	1st : 캐릭터 아이디[In]
//	2st : 캐릭터 정보 테이블[Out]
//
// Return :
//	생성 성공 시 true, 실패시 false
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool UpdateCharInfoEx(CDBComponent& DBComponent, unsigned long CharID_In, LPCHAR_INFOEX lpCharInfoEx_In)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	char szInfo[0xff] = {0,};	

	sprintf(szInfo, "cast('%d-%d-%d %d:%d:%d' as smalldatetime)", lpCharInfoEx_In->LoggoutTime.Year, lpCharInfoEx_In->LoggoutTime.Month, lpCharInfoEx_In->LoggoutTime.Day,
		lpCharInfoEx_In->LoggoutTime.Hour, lpCharInfoEx_In->LoggoutTime.Minute, lpCharInfoEx_In->LoggoutTime.Second, lpCharInfoEx_In->LoggoutTime.MSecond);

    _snprintf(DBComponent.GetQueryBuffer(), CDBComponent::MAX_QUERY_LENGTH,         
        "UPDATE CharInfoEx SET Total=%d, ServerID=%d, NameChangeCount=%d, AdminFlag=%d, GuildWarFlag=%d, RealmWarFlag=%d, RealmPoint=%d, RealmCheckPoint=%d, RealmMinute=%d, LogoutTime=%s WHERE CID=%d",
        lpCharInfoEx_In->Total, lpCharInfoEx_In->ServerID, lpCharInfoEx_In->cNameChangeCount, lpCharInfoEx_In->cAdminFlag,
		lpCharInfoEx_In->GuildWarFlag, lpCharInfoEx_In->RealmWarFlag, lpCharInfoEx_In->RealmPoint, lpCharInfoEx_In->RealmCheckPoint, lpCharInfoEx_In->RealmMinute, szInfo, CharID_In);


    if (DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer())) {
        return true;
    }

    ERRLOG3(g_Log, "CID:%10u %s %s", CharID_In, DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
// InsertItemUID [ public ]
// - 아이템 UID 추가
//
// Parameter :
//	1st : 서버 아이디[In]
//
// Return :
//	생성 성공 시 true, 실패시 false
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool InsertItemUID(CDBComponent& DBComponent, unsigned long ServerID_In, DWORD64 dwItemUID)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    _snprintf(DBComponent.GetQueryBuffer(), CDBComponent::MAX_QUERY_LENGTH, 
        "INSERT INTO ItemUID (Server, Item) VALUES (%d, %I64d)", 
        ServerID_In, dwItemUID);

    if (DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer())) {
        return true;
    }

    SERLOG2(g_Log, "SID:%10u %s", ServerID_In, DBComponent.GetErrorString());	// cppis 가 플러쉬없이 바로 로그하기위해서 추가함
    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
// GetItemUID [ public ]
// - 아이템 UID 얻기
//
// Parameter :
//	1st : 서버 아이디[In]
//
// Return :
//	생성 성공 시 true, 실패시 false
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool GetItemUID(CDBComponent& DBComponent, unsigned long ServerID_In, DWORD64 *lpItemID_Out, bool bSupressLog)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    memset(lpItemID_Out, 0, sizeof(DWORD64));

    _snprintf(DBComponent.GetQueryBuffer(), CDBComponent::MAX_QUERY_LENGTH, "SELECT Item FROM ItemUID WHERE Server = %d", ServerID_In );
    if (DBComponent.ExecuteQueryGetData(DBComponent.GetQueryBuffer(), (void *)lpItemID_Out))
    {
        return true;
    }

    if(!bSupressLog)
    {
        SERLOG2(g_Log, "SID:0x%08X %s", ServerID_In, DBComponent.GetErrorString());	// cppis 가 플러쉬없이 바로 로그하기위해서 추가함
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
// SetItemUID [ public ]
// - 캐릭터 뷰 얻기
//
// Parameter :
//	1st : 서버 아이디[In]
//
// Return :
//	생성 성공 시 true, 실패시 false
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool SetItemUID(CDBComponent& DBComponent, unsigned long ServerID_In, DWORD64 ItemID_In)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    char Text[100] = "";
    Math::Convert::Hex64ToStr(Text, ItemID_In);

    _snprintf(DBComponent.GetQueryBuffer(), CDBComponent::MAX_QUERY_LENGTH, "UPDATE ItemUID SET Item = 0x%s WHERE Server = %d", Text, ServerID_In );
    if (DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer())) {
        return true;
    }

    SERLOG2(g_Log, "SID:%10u %s", ServerID_In, DBComponent.GetErrorString());	// cppis 가 플러쉬없이 바로 로그하기위해서 추가함
    return false;
}


bool InsertEventItem(CDBComponent& DBComponent, unsigned short wItemID, unsigned short wMaxNum, const tm& StartTime, const tm& EndTime)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    char strStartTime[20] = "";
    char strEndTime[20] = "";

    sprintf(strStartTime, "%d-%d-%d %d:%d:%d", StartTime.tm_year + 1900, StartTime.tm_mon + 1, StartTime.tm_mday, 
        StartTime.tm_hour, StartTime.tm_min, StartTime.tm_sec);
    sprintf(strEndTime, "%d-%d-%d %d:%d:%d", EndTime.tm_year + 1900, EndTime.tm_mon + 1, EndTime.tm_mday, 
        EndTime.tm_hour, EndTime.tm_min, EndTime.tm_sec);

    _snprintf(DBComponent.GetQueryBuffer(), CDBComponent::MAX_QUERY_LENGTH, 
        "INSERT INTO TblEventItem (nItemIndex, nItemAmount, nMaxAmount, StartTime, EndTime) "
        "VALUES (%d, 0, %d, cast('%s' as smalldatetime), cast('%s' as smalldatetime))", 
        wItemID, wMaxNum, strStartTime, strEndTime);

    if (true == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer())) 
    {
        return true;
    }

    SERLOG1(g_Log, "%s", DBComponent.GetErrorString());
    return false;
}

bool UpdateEventItem(CDBComponent& DBComponent, unsigned short wItemID, unsigned short wMaxNum, const tm& StartTime, const tm& EndTime)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    char strStartTime[20] = "";
    char strEndTime[20] = "";

    sprintf(strStartTime, "%d-%d-%d %d:%d:%d", StartTime.tm_year + 1900, StartTime.tm_mon + 1, StartTime.tm_mday, 
        StartTime.tm_hour, StartTime.tm_min, StartTime.tm_sec);
    sprintf(strEndTime, "%d-%d-%d %d:%d:%d", EndTime.tm_year + 1900, EndTime.tm_mon + 1, EndTime.tm_mday, 
        EndTime.tm_hour, EndTime.tm_min, EndTime.tm_sec);

    _snprintf(DBComponent.GetQueryBuffer(), CDBComponent::MAX_QUERY_LENGTH, "UPDATE TblEventItem "
        "SET nMaxAmount = %d, StartTime = cast('%s' as smalldatetime), EndTime = cast('%s' as smalldatetime) "
        "WHERE nItemIndex = %d", wMaxNum, strStartTime, strEndTime, wItemID);

    if (DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer())) 
    {
        return true;
    }

    SERLOG1(g_Log, "UpdateEventItem : %s", DBComponent.GetErrorString());
    return false;
}

bool UpdateEventItem(CDBComponent& DBComponent, unsigned short wItemID, unsigned short wNowNum)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    _snprintf(DBComponent.GetQueryBuffer(), CDBComponent::MAX_QUERY_LENGTH, 
        "UPDATE TblEventItem SET nItemAmount = %d WHERE nItemIndex = %d", wNowNum, wItemID);

    if (true == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer())) 
    {
        return true;
    }

    SERLOG1(g_Log, "%s", DBComponent.GetErrorString());
    return false;
}

bool DeleteEventItem(CDBComponent& DBComponent, unsigned short wItemID)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    _snprintf(DBComponent.GetQueryBuffer(), CDBComponent::MAX_QUERY_LENGTH, 
        "DELETE FROM TblEventItem WHERE nItemIndex = %d", wItemID);

    if (true == DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer())) 
    {
        return true;
    }

    SERLOG1(g_Log, "%s", DBComponent.GetErrorString());
    return false;

}

bool GetUnifiedItemStore1(CDBComponent& DBComponent, unsigned long UserID_In,
                          unsigned char cOldServerGroupID, LPSTORE lpStore_Out)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    memset(lpStore_Out, 0, sizeof(STORE));

    char szQuery[CDBComponent::QUERY_BUFFER_LEN];

    if (0 < _snprintf(szQuery, CDBComponent::QUERY_BUFFER_LEN - 1, 
        "SELECT Store FROM TblUnifiedItemStore1 WHERE UID = %d AND OldServerGroupID = %d",
        UserID_In, cOldServerGroupID))
    {
        if (DBComponent.ExecuteQueryGetData(szQuery, lpStore_Out)) 
        {
            if (sizeof(unsigned long) <= lpStore_Out->dwSize)
            {
                lpStore_Out->dwSize -= sizeof(unsigned long);
            }
            else
            {
                lpStore_Out->dwSize = 0;
            }

            return true;
        }
    }

    szQuery[CDBComponent::QUERY_BUFFER_LEN - 1] = 0;
    ERRLOG3(g_Log, "UID:%10u / %s : %s", UserID_In, DBComponent.GetErrorString(), szQuery);
    return false;
}

bool GetUnifiedItemStore2(CDBComponent& DBComponent, unsigned long UserID_In, 
                          unsigned char cOldServerGroupID, LPSTORE lpStore_Out)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    memset(lpStore_Out, 0, sizeof(STORE));

    char szQuery[CDBComponent::QUERY_BUFFER_LEN];

    if (0 < _snprintf(szQuery, CDBComponent::QUERY_BUFFER_LEN - 1, 
        "SELECT Store FROM TblUnifiedItemStore2 WHERE UID = %d AND OldServerGroupID = %d", 
        UserID_In, cOldServerGroupID))
    {
        if (DBComponent.ExecuteQueryGetData(szQuery, lpStore_Out)) 
        {
            if (sizeof(unsigned long) <= lpStore_Out->dwSize)
            {
                lpStore_Out->dwSize -= sizeof(unsigned long);
            }
            else
            {
                lpStore_Out->dwSize = 0;
            }

            return true;
        }
    }

    szQuery[CDBComponent::QUERY_BUFFER_LEN - 1] = 0;
    ERRLOG3(g_Log, "UID:%10u / %s : %s", UserID_In, DBComponent.GetErrorString(), szQuery);
    return false;
}

bool UpdateUnifiedItemStore1(CDBComponent& DBComponent, unsigned long UserID_In, 
                             unsigned char cOldServerGroupID, LPSTORE lpStore_In)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    char szQuery[CDBComponent::QUERY_BUFFER_LEN];

    if (0 < lpStore_In->dwSize)
    {
        if (0 < _snprintf(szQuery, CDBComponent::QUERY_BUFFER_LEN - 1, 
            "SELECT Store FROM TblUnifiedItemStore1 WHERE UID = %d AND OldServerGroupID = %d", 
            UserID_In, cOldServerGroupID))
        {
            lpStore_In->dwSize += sizeof(unsigned long);
            if (DBComponent.ExecuteQuery(szQuery, OleDB::Rowset_Update))
            {
                if (DBComponent.SetBinaryData(1, (OleDB::LPSET_BINARY)lpStore_In)) 
                {
                    lpStore_In->dwSize -= sizeof(unsigned long);
                    return true;
                }
            }

            lpStore_In->dwSize -= sizeof(unsigned long);
        }
    }
    else
    {
        if (0 < _snprintf(szQuery, CDBComponent::QUERY_BUFFER_LEN - 1, 
            "UPDATE TblUnifiedItemStore1 SET Store = NULL WHERE UID = %d AND OldServerGroupID = %d", 
            UserID_In, cOldServerGroupID))
        {
            if (DBComponent.ExecuteQuery(szQuery))
            {
                return true;
            }
        }
    }

    szQuery[CDBComponent::QUERY_BUFFER_LEN - 1] = 0;
    ERRLOG3(g_Log, "UID:%10u / %s : %s", UserID_In, DBComponent.GetErrorString(), szQuery);
    return false;
}

bool UpdateUnifiedItemStore2(CDBComponent& DBComponent, unsigned long UserID_In, 
                             unsigned char cOldServerGroupID, LPSTORE lpStore_In)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    char szQuery[CDBComponent::QUERY_BUFFER_LEN];

    if(0 < lpStore_In->dwSize)
    {
        _snprintf(szQuery, CDBComponent::QUERY_BUFFER_LEN - 1, 
            "SELECT Store FROM TblUnifiedItemStore2 WHERE UID = %d AND OldServerGroupID = %d", 
            UserID_In, cOldServerGroupID);

        lpStore_In->dwSize += sizeof(unsigned long);
        if (DBComponent.ExecuteQuery(szQuery, OleDB::Rowset_Update)) 
        {
            if (DBComponent.SetBinaryData(1, (OleDB::LPSET_BINARY)lpStore_In)) 
            {
                lpStore_In->dwSize -= sizeof(unsigned long);
                return true;
            }
        }

        lpStore_In->dwSize -= sizeof(unsigned long);
    }
    else
    {
        if (0 < _snprintf(szQuery, CDBComponent::QUERY_BUFFER_LEN - 1, 
            "UPDATE TblUnifiedItemStore2 SET Store = NULL WHERE UID = %d AND OldServerGroupID = %d", 
            UserID_In, cOldServerGroupID))
        {
            if (DBComponent.ExecuteQuery(szQuery))
            {
                return true;
            }
        }
    }

    szQuery[CDBComponent::QUERY_BUFFER_LEN - 1] = 0;
    ERRLOG3(g_Log, "UID:%10u / %s : %s", UserID_In, DBComponent.GetErrorString(), szQuery);
    return false;
}

bool GetUnifiedItemStoreInfo(CDBComponent& DBComponent, unsigned long UserID_In, 
                             unsigned char cOldServerGroupID, STORE_INFO* lpStoreInfo, bool bSupressLog)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    memset(lpStoreInfo, 0, sizeof(STORE_INFO));

    char szQuery[CDBComponent::QUERY_BUFFER_LEN];

    _snprintf(szQuery, CDBComponent::QUERY_BUFFER_LEN - 1, 
        "SELECT Password, Frag, Gold FROM TblUnifiedItemStore1 WHERE UID = %d AND OldServerGroupID = %d", 
        UserID_In, cOldServerGroupID);

    if (DBComponent.ExecuteQueryGetData(szQuery, lpStoreInfo))
    {
        return true;
    }

    if(!bSupressLog)
    {
        szQuery[CDBComponent::QUERY_BUFFER_LEN - 1] = 0;
        ERRLOG3(g_Log, "UID:%10u / %s %s", UserID_In, DBComponent.GetErrorString(), szQuery);
    }

    return false;
}

bool GetUnifiedItemStoreInfo(CDBComponent& DBComponent, unsigned long UserID_In, 
                             UnifiedStoreInfo* lpUnifiedStoreInfo_Out, int& nStoreNum_InOut)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    char szQuery[CDBComponent::QUERY_BUFFER_LEN];

    _snprintf(szQuery, CDBComponent::QUERY_BUFFER_LEN - 1, 
        "SELECT OldServerGroupID, Password, Frag, Gold FROM TblUnifiedItemStore1 WHERE UID = %d", UserID_In);

    if (DBComponent.ExecuteQuery(szQuery))
    {
        int nGetRows = 0;

        if(DBComponent.GetData((void**)lpUnifiedStoreInfo_Out, 
            sizeof(UnifiedStoreInfo), nStoreNum_InOut, &nGetRows))
        {
            nStoreNum_InOut = nGetRows;
            return true;
        }
    }

    szQuery[CDBComponent::QUERY_BUFFER_LEN - 1] = 0;
    ERRLOG3(g_Log, "UID:%10u / %s %s", UserID_In, DBComponent.GetErrorString(), szQuery);
    return false;
}

bool SetUnifiedItemStoreInfo(CDBComponent& DBComponent, unsigned long UserID_In, 
                             unsigned char cOldServerGroupID, LPSTORE_INFO lpStoreInfo_In)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    lpStoreInfo_In->Password[STORE_INFO::MAX_PASS_LEN - 1] = 0;

    char szQuery[CDBComponent::QUERY_BUFFER_LEN];

    if (0 < _snprintf(szQuery, CDBComponent::QUERY_BUFFER_LEN - 1, 
        "UPDATE TblUnifiedItemStore1 SET Password = '%s', Frag = %d, Gold = %d WHERE UID = %d AND OldServerGroupID = %d",
        lpStoreInfo_In->Password, lpStoreInfo_In->Flag, lpStoreInfo_In->Gold, UserID_In, cOldServerGroupID))
    {
        if (DBComponent.ExecuteQuery(szQuery)) 
        {
            return true;
        }
    }

    szQuery[CDBComponent::QUERY_BUFFER_LEN - 1] = 0;
    ERRLOG3(g_Log, "UID:%10u / %s %s", UserID_In, DBComponent.GetErrorString(), szQuery);
    return false;
}

bool DeleteUnifiedStore(CDBComponent& DBComponent, unsigned long dwUID, unsigned char cOldStoreServerGroup)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    bool bStore1Result = false;
    bool bStore2Result = false;

    char szQuery[CDBComponent::QUERY_BUFFER_LEN];

    if (0 < _snprintf(szQuery, CDBComponent::QUERY_BUFFER_LEN - 1, 
        "DELETE FROM TblUnifiedItemStore1 WHERE UID = %d AND OldServerGroupID = %d",
        dwUID, cOldStoreServerGroup))
    {
        if (DBComponent.ExecuteQuery(szQuery))
        {            
            bStore1Result = true;
        }
    }

    if (!bStore1Result)
    {
        szQuery[CDBComponent::QUERY_BUFFER_LEN - 1] = 0;
        ERRLOG3(g_Log, "UID:%10u / %s : %s", dwUID, DBComponent.GetErrorString(), szQuery);        
    }
    
    if (0 < _snprintf(szQuery, CDBComponent::QUERY_BUFFER_LEN - 1, 
        "DELETE FROM TblUnifiedItemStore2 WHERE UID = %d AND OldServerGroupID = %d",
        dwUID, cOldStoreServerGroup))
    {
        if (DBComponent.ExecuteQuery(szQuery))
        {            
            bStore2Result = true;
        }
    }

    if (!bStore2Result)
    {
        szQuery[CDBComponent::QUERY_BUFFER_LEN - 1] = 0;
        ERRLOG3(g_Log, "UID:%10u / %s : %s", dwUID, DBComponent.GetErrorString(), szQuery);
    }
    
    return (bStore1Result && bStore2Result);
}

bool ChangeUnifiedItemStoreGroup(CDBComponent& DBComponent, unsigned long dwUID, 
                                 unsigned char cOldServerGroupID, unsigned char cChangedServerGroupID)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);
        
    char szQuery[CDBComponent::QUERY_BUFFER_LEN];

    if (0 < _snprintf(szQuery, CDBComponent::QUERY_BUFFER_LEN - 1, 
        "dbo.TransferStore %d, %d, %d", dwUID, cOldServerGroupID, cChangedServerGroupID))
    {
        if (DBComponent.ExecuteQuery(szQuery))
        {            
            return true;
        }
    }

    szQuery[CDBComponent::QUERY_BUFFER_LEN - 1] = 0;
    ERRLOG3(g_Log, "UID:%10u / %s : %s", dwUID, DBComponent.GetErrorString(), szQuery);
    return false;
}



bool GetUnifiedCharList(CDBComponent& DBComponent, unsigned long dwUID, 
                        UnifiedCharData* lpUnifiedCharData, int& nUnifiedCharData_InOut)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    memset(lpUnifiedCharData, 0, sizeof(UnifiedCharData) * nUnifiedCharData_InOut);

    char szQuery[CDBComponent::QUERY_BUFFER_LEN];

    if (0 < _snprintf(szQuery, CDBComponent::QUERY_BUFFER_LEN - 1, 
        "dbo.GetUnifiedCharList %d", dwUID))
    {
        if (DBComponent.ExecuteQuery(szQuery))
        {
            int nGetRows = 0;

            if (DBComponent.GetData((void**)lpUnifiedCharData, 
                sizeof(UnifiedCharData), nUnifiedCharData_InOut, &nGetRows))
            {
                nUnifiedCharData_InOut = nGetRows;
                return true;
            }
        }
    }

    szQuery[CDBComponent::QUERY_BUFFER_LEN - 1] = 0;
    ERRLOG3(g_Log, "UID:%10u / %s : %s", dwUID, DBComponent.GetErrorString(), szQuery);
    return false;
}

bool UpdateUnifiedCharServerGroupID(CDBComponent& DBComponent, unsigned long dwUID, 
                                    unsigned long dwCID, unsigned char cOldServerGroupID)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    char szQuery[CDBComponent::QUERY_BUFFER_LEN];

    if (0 < _snprintf(szQuery, CDBComponent::QUERY_BUFFER_LEN - 1, 
        "UPDATE TblUnifiedCharList SET OldServerGroupID = %d WHERE UID = %d AND NewCID = %d", 
        cOldServerGroupID, dwUID, dwCID))
    {
        if (DBComponent.ExecuteQuery(szQuery))
        {
            return true;
        }
    }

    szQuery[CDBComponent::QUERY_BUFFER_LEN - 1] = 0;
    ERRLOG5(g_Log, "UID:%10u / CID:%10u / ServerGroupID:%d / %s : %s", 
        dwUID, dwCID, cOldServerGroupID, DBComponent.GetErrorString(), szQuery);
    return false;
}


///////////////////////////////////////////////////////////////////////////////////////////////
//
// 프랜드 관련
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool GetFriend(CDBComponent& DBComponent, unsigned long CharID_In, LPFRIEND lpFriend_Out)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    memset(lpFriend_Out, 0, sizeof(FRIEND));

    _snprintf(DBComponent.GetQueryBuffer(), 
        CDBComponent::MAX_QUERY_LENGTH, "SELECT Friend, FriendInfo FROM Friend WHERE CID = %d", CharID_In);

    if (DBComponent.ExecuteQueryGetData(DBComponent.GetQueryBuffer(), lpFriend_Out)) 
    {        
        if(sizeof(unsigned long) < lpFriend_Out->dwSize)
        {
            lpFriend_Out->dwSize -= sizeof(unsigned long);
        }
        else
        {
            lpFriend_Out->dwSize = 0;
        }

        return true;
    }

    ERRLOG3(g_Log, "CID:%08d %s %s", CharID_In, 
        DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());

    return false;
}

bool GetBan(CDBComponent& DBComponent, unsigned long CharID_In, LPBAN lpBan_Out)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    memset(lpBan_Out, 0, sizeof(BAN));

    _snprintf(DBComponent.GetQueryBuffer(), 
        CDBComponent::MAX_QUERY_LENGTH, "SELECT Ban, BanInfo FROM Friend WHERE CID = %d", CharID_In);

    if (DBComponent.ExecuteQueryGetData(DBComponent.GetQueryBuffer(), lpBan_Out)) 
    {
        if(sizeof(unsigned long) < lpBan_Out->dwSize)
        {
            lpBan_Out->dwSize -= sizeof(unsigned long);
        }
        else
        {
            lpBan_Out->dwSize = 0;
        }

        return true;
    }

    ERRLOG3(g_Log, "CID:%08d %s %s", CharID_In, 
        DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());

    return false;
}

bool UpdateFriend(CDBComponent& DBComponent, unsigned long CharID_In, LPFRIEND lpFriend_In)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    int	Counter = 0;
    char* pDest = NULL;

    if(0 < lpFriend_In->dwSize)
    {
        _snprintf(DBComponent.GetQueryBuffer(), 
            CDBComponent::MAX_QUERY_LENGTH, "SELECT Friend FROM Friend WHERE CID = %d", CharID_In );

        lpFriend_In->dwSize += sizeof(unsigned long);

        if(DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer(), OleDB::Rowset_Update))
        {
            if (DBComponent.SetBinaryData(1, (OleDB::LPSET_BINARY)lpFriend_In)) 
            {
				_snprintf(DBComponent.GetQueryBuffer(), 
					CDBComponent::MAX_QUERY_LENGTH, "SELECT FriendInfo FROM Friend WHERE CID = %d", CharID_In);

				lpFriend_In->dwInfoSize += sizeof(unsigned long);

				if(DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer(), OleDB::Rowset_Update))
				{
					if (DBComponent.SetBinaryData(1, (OleDB::LPSET_BINARY)&lpFriend_In->dwInfoSize)) 
					{
						lpFriend_In->dwInfoSize -= sizeof(unsigned long);
						lpFriend_In->dwSize		-= sizeof(unsigned long);

						return true;
					}
				}

				lpFriend_In->dwInfoSize -= sizeof(unsigned long);
            }
        }		

        lpFriend_In->dwSize -= sizeof(unsigned long);
    }
    else
    {
        _snprintf(DBComponent.GetQueryBuffer(), 
            CDBComponent::MAX_QUERY_LENGTH, "UPDATE Friend SET Friend = NULL, FriendInfo = NULL WHERE CID = %d", CharID_In );

        if (DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer()))
        {
            return true;
        }
    }

    ERRLOG3(g_Log, "CID:%08d %s %s", CharID_In, 
        DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
    
    return false;
}

bool UpdateBan(CDBComponent& DBComponent, unsigned long CharID_In, LPBAN lpBan_In)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    int	Counter = 0;
    char* pDest = NULL;

    if(0 < lpBan_In->dwSize)
    {
        _snprintf( DBComponent.GetQueryBuffer(), CDBComponent::MAX_QUERY_LENGTH, 
            "SELECT Ban FROM Friend WHERE CID = %d", CharID_In );

        lpBan_In->dwSize += sizeof(unsigned long);
        
		if (DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer(), OleDB::Rowset_Update)) 
        {
            if (DBComponent.SetBinaryData(1, (OleDB::LPSET_BINARY)lpBan_In)) 
            {
				_snprintf(DBComponent.GetQueryBuffer(), 
					CDBComponent::MAX_QUERY_LENGTH, "SELECT BanInfo FROM Friend WHERE CID = %d", CharID_In);

				lpBan_In->dwInfoSize += sizeof(unsigned long);

				if(DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer(), OleDB::Rowset_Update))
				{
					if (DBComponent.SetBinaryData(1, (OleDB::LPSET_BINARY)&lpBan_In->dwInfoSize)) 
					{
						lpBan_In->dwInfoSize	-= sizeof(unsigned long);
						lpBan_In->dwSize		-= sizeof(unsigned long);

						return true;
					}
				}

				lpBan_In->dwInfoSize -= sizeof(unsigned long);                                
            }
        }

        lpBan_In->dwSize -= sizeof(unsigned long);
    }
    else
    {
        _snprintf( DBComponent.GetQueryBuffer(), CDBComponent::MAX_QUERY_LENGTH, 
            "UPDATE Friend SET Ban = NULL, BanInfo = NULL WHERE CID = %d", CharID_In );
        
        if (DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer())) 
        {
            return true;
        }
    }

    ERRLOG3(g_Log, "CID:%08d %s %s", CharID_In, 
        DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
    
    return false;
}


// 퀘스트 관련
bool GetQuest(CDBComponent& DBComponent, unsigned long CharID_In, LPQUEST lpQuest_Out)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    memset(lpQuest_Out, 0, sizeof(QUEST));

    _snprintf(DBComponent.GetQueryBuffer(), CDBComponent::MAX_QUERY_LENGTH, 
        "SELECT Quest FROM Quest WHERE CID = %d", CharID_In);

    if (DBComponent.ExecuteQueryGetData(DBComponent.GetQueryBuffer(), lpQuest_Out)) {
        if (lpQuest_Out->dwSize != 0)
            lpQuest_Out->dwSize -= sizeof(unsigned long);

        return true;
    }

    ERRLOG3(g_Log, "CID:%08d %s %s", CharID_In, DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
    return false;
}

bool GetHistory(CDBComponent& DBComponent, unsigned long CharID_In, LPHISTORY lpHistory_Out)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    memset(lpHistory_Out, 0, sizeof(HISTORY));

    _snprintf(DBComponent.GetQueryBuffer(), CDBComponent::MAX_QUERY_LENGTH, 
        "SELECT History FROM Quest WHERE CID = %d", CharID_In);

    if (DBComponent.ExecuteQueryGetData(DBComponent.GetQueryBuffer(), lpHistory_Out)) {
        if (lpHistory_Out->dwSize != 0)
            lpHistory_Out->dwSize -= sizeof(unsigned long);

        return true;
    }

    ERRLOG3(g_Log, "CID:%08d %s %s", CharID_In, DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
    return false;
}

bool UpdateQuest(CDBComponent& DBComponent, unsigned long CharID_In, LPQUEST lpQuest_In)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    int	Counter = 0;
    char* pDest = NULL;

    _snprintf(DBComponent.GetQueryBuffer(), CDBComponent::MAX_QUERY_LENGTH, "SELECT Quest FROM Quest WHERE CID = %d", CharID_In );

    lpQuest_In->dwSize += sizeof(unsigned long);
    if (DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer(), OleDB::Rowset_Update)) {
        if (DBComponent.SetBinaryData(1, (OleDB::LPSET_BINARY)lpQuest_In)) {
            lpQuest_In->dwSize -= sizeof(unsigned long);
            return true;
        }
    }

    ERRLOG3(g_Log, "CID:%08d %s %s", CharID_In, DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());

    lpQuest_In->dwSize -= sizeof(unsigned long);
    return false;
}

bool UpdateHistory(CDBComponent& DBComponent, unsigned long CharID_In, LPHISTORY lpHistory_In)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    int	Counter = 0;
    char* pDest = NULL;

    _snprintf(DBComponent.GetQueryBuffer(), CDBComponent::MAX_QUERY_LENGTH, "SELECT History FROM Quest WHERE CID = %d", CharID_In );

    lpHistory_In->dwSize += sizeof(unsigned long);
    if (DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer(), OleDB::Rowset_Update)) {
        if (DBComponent.SetBinaryData(1, (OleDB::LPSET_BINARY)lpHistory_In)) {
            lpHistory_In->dwSize -= sizeof(unsigned long);
            return true;
        }
    }

    ERRLOG3(g_Log, "CID:%08d %s %s", CharID_In, DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());

    lpHistory_In->dwSize -= sizeof(unsigned long);
    return false;
}

// 설정 관련
bool GetConfig(CDBComponent& DBComponent, unsigned long CharID_In, LPCONFIG lpConfig_Out)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    memset(lpConfig_Out, 0, sizeof(CONFIG));

    _snprintf(DBComponent.GetQueryBuffer(), CDBComponent::MAX_QUERY_LENGTH, 
        "SELECT Config FROM CharInfoEx WHERE CID = %d", CharID_In);

    if (DBComponent.ExecuteQueryGetData(DBComponent.GetQueryBuffer(), lpConfig_Out)) {
        if (lpConfig_Out->dwSize != 0)
            lpConfig_Out->dwSize -= sizeof(unsigned long);

        return true;
    }

    ERRLOG3(g_Log, "CID:%08d %s %s", CharID_In, DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
    return false;
}

bool UpdateConfig(CDBComponent& DBComponent, unsigned long CharID_In, LPCONFIG lpConfig_In)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    int	Counter = 0;
    char* pDest = NULL;

    _snprintf(DBComponent.GetQueryBuffer(), CDBComponent::MAX_QUERY_LENGTH, "SELECT Config FROM CharInfoEx WHERE CID = %d", CharID_In);

    lpConfig_In->dwSize += sizeof(unsigned long);
    if (DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer(), OleDB::Rowset_Update)) {
        if (DBComponent.SetBinaryData(1, (OleDB::LPSET_BINARY)lpConfig_In)) {
            lpConfig_In->dwSize -= sizeof(unsigned long);
            return true;
        }
    }

    ERRLOG3(g_Log, "CID:%08d %s %s", CharID_In, DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());

    lpConfig_In->dwSize -= sizeof(unsigned long);
    return false;
}

// 프리미엄 타임 관련 2008.6.09 일
bool GetPremiumService(CDBComponent& DBComponent, unsigned long UserID_In, PREMIUMSERVICE& Premium_Out)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    memset(&Premium_Out, 0, sizeof(PREMIUMSERVICE));

	char szQuery[CDBComponent::QUERY_BUFFER_LEN];

    if (0 < _snprintf(szQuery, CDBComponent::QUERY_BUFFER_LEN - 1,
        "GetPremiumTime %d", UserID_In))
    {
	    if (DBComponent.ExecuteQueryGetData(szQuery, &Premium_Out)) 
            return true;
    }

    ERRLOG3(g_Log, "UID:%08d %s %s", UserID_In, DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
    return false;
}

bool UpdatePremiumService(CDBComponent& DBComponent, unsigned long UserID_In, int Flag_In)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    char szQuery[CDBComponent::QUERY_BUFFER_LEN];

    if (0 < _snprintf(szQuery, CDBComponent::QUERY_BUFFER_LEN - 1,
        "UpdatePremiumTime %d, %d", UserID_In, Flag_In))
    {
        if (DBComponent.ExecuteQuery(szQuery))
            return true;
    }

    szQuery[CDBComponent::QUERY_BUFFER_LEN - 1] = 0;
    ERRLOG3(g_Log, "UID:%10u / %s : %s", UserID_In, szQuery, DBComponent.GetErrorString());	

	return false;
}

bool AddPremiumService(CDBComponent& DBComponent, unsigned long UserID_In, int ServiceType_In, int AddDay_In)
{
   DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	char szQuery[CDBComponent::QUERY_BUFFER_LEN];

	int iRet = 0;

    if (0 < _snprintf(szQuery, CDBComponent::QUERY_BUFFER_LEN - 1,
        "AddPremiumTime %d, %d, %d", UserID_In, ServiceType_In, AddDay_In))
    {
	    if (DBComponent.ExecuteQueryGetData(szQuery, &iRet)) 
		{
			if(iRet == 1)
				return true;	

			return false;
		}
    }

    ERRLOG3(g_Log, "UID:%08d %s %s", UserID_In, DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
    return false;
}

bool UpdateExtraEvent(CDBComponent& DBComponent, unsigned long UserID_In, unsigned long EventType_In, unsigned long EventNum_In, unsigned long EventValue_In, unsigned int* Result_Out)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	char szQuery[CDBComponent::QUERY_BUFFER_LEN];

	if (0 < _snprintf(szQuery, CDBComponent::QUERY_BUFFER_LEN - 1,
		"UpdateExtraEvent %d, %d, %d, %d", UserID_In, EventType_In, EventNum_In, EventValue_In))
	{
		if (DBComponent.ExecuteQueryGetData(szQuery, Result_Out)) 
		{
			return true;
		}
	}

	ERRLOG3(g_Log, "UID:%08d %s %s", UserID_In, DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
	return false;
}

// 게임 타임 관련 2007.9.10 일 중국 타임문제로 추가됨.
bool GetPlayTime(CDBComponent& DBComponent, unsigned long UserID_In, unsigned int* PlayTime_Out)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);
/*
    _snprintf(DBComponent.GetQueryBuffer(), CDBComponent::MAX_QUERY_LENGTH, 
        "SELECT cumtime FROM youxiuser.dbo.UserTime WHERE uid = %d", UserID_In);

    if (DBComponent.ExecuteQueryGetData(DBComponent.GetQueryBuffer(), PlayTime_Out)) 
        return true;
*/
    char szQuery[CDBComponent::QUERY_BUFFER_LEN];

    if (0 < _snprintf(szQuery, CDBComponent::QUERY_BUFFER_LEN - 1,
        "GetTime %d", UserID_In))
    {
	    if (DBComponent.ExecuteQueryGetData(szQuery, PlayTime_Out)) 
            return true;
    }

    ERRLOG3(g_Log, "UID:%08d %s %s", UserID_In, DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
    return false;
}

bool UpdatePlayTime(CDBComponent& DBComponent, unsigned long UserID_In, int Flag_In)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    char szQuery[CDBComponent::QUERY_BUFFER_LEN];

    if (0 < _snprintf(szQuery, CDBComponent::QUERY_BUFFER_LEN - 1,
        "UpdateTime %d, %d", UserID_In, Flag_In))
    {
        if (DBComponent.ExecuteQuery(szQuery))
            return true;
    }

    szQuery[CDBComponent::QUERY_BUFFER_LEN - 1] = 0;
    ERRLOG3(g_Log, "UID:%10u / %s : %s", UserID_In, szQuery, DBComponent.GetErrorString());	

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
// InsertChar [ public ]
// - 캐릭터 업데이트
//
// Parameter :
//	1st : 유저 아이디[In]
//	2st : 슬롯 위치[In]
//	3st : 생성 테이블[In]
//	4st : 위치[In]
//
// Return :
//	생성 성공 시 true, 실패시 false
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool InsertChar(CDBComponent& DBComponent, unsigned long dwUID, unsigned long dwSlot, 
                CHAR_CREATE& char_In, unsigned long dwGold, POS& pos, SKILL& skill, 
                unsigned char cOldServerGroupID, unsigned long& dwCID_Out)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    CHAR_CREATE_DB charCreateDB;

    charCreateDB.dwUID          = dwUID;         
    charCreateDB.dwSlotPos      = dwSlot;
    memcpy(charCreateDB.szName, char_In.Name, sizeof(char) * CHAR_INFOST::MAX_NAME_LEN);
    charCreateDB.cSex           = char_In.Sex;
    charCreateDB.cHair          = char_In.Hair;
    charCreateDB.cFace          = char_In.Face;
    charCreateDB.cRace          = char_In.Race;
    charCreateDB.sClass         = char_In.Class;
    charCreateDB.dwGold         = dwGold;
    charCreateDB.sSTR           = char_In.STR;
    charCreateDB.sDEX           = char_In.DEX;
    charCreateDB.sCON           = char_In.CON;
    charCreateDB.sINT           = char_In.INT;
    charCreateDB.sWIS           = char_In.WIS;
    charCreateDB.fPointX        = pos.fPointX;
    charCreateDB.fPointY        = pos.fPointY;
    charCreateDB.fPointZ        = pos.fPointZ;
    memcpy(charCreateDB.Equip, char_In.Equip, sizeof(char) * 30);

    charCreateDB.skill          = skill;
    charCreateDB.cServerGroupID = cOldServerGroupID;
    charCreateDB.dwCID_Out      = 0;

    if (DBComponent.ExecuteQueryWithParams(
        "dbo.InsertChar ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ? OUTPUT", 
        reinterpret_cast<char*>(&charCreateDB), CGameDBParams::GetInstance().GetCharCreateParam()))
    {
        dwCID_Out = charCreateDB.dwCID_Out;
        return true;
    }

    SERLOG3(g_Log, "UID:%10u / Slot : %d / %s", dwUID, dwSlot, DBComponent.GetErrorString());	
    return false;

}


///////////////////////////////////////////////////////////////////////////////////////////////
//
// DeleteChar [ public ]
// - 캐릭터 삭제
//
// Parameter :
//	1st : 유저 아이디[In]
//	2st : 슬롯[In]
//	3st : 캐릭터 아이디[In]
//
// Return :
//	생성 성공 시 true, 실패시 false
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool DeleteChar(CDBComponent& DBComponent, unsigned long dwUID, unsigned long dwCID, unsigned char cSlot, 
                unsigned char cCurrentServerGroupID, unsigned char cOldServerGroupID)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    char szQuery[CDBComponent::QUERY_BUFFER_LEN];

    if (0 < _snprintf(szQuery, CDBComponent::QUERY_BUFFER_LEN - 1, 
        "DeleteChar %d, %d, %d, %d, %d", dwUID, cSlot, dwCID, cCurrentServerGroupID, cOldServerGroupID))
    {
        if (DBComponent.ExecuteQuery(szQuery)) 
        {
            return true;
        }
    }

    szQuery[CDBComponent::QUERY_BUFFER_LEN - 1] = 0;
    ERRLOG5(g_Log, "UID:%10u / CID:%10u / Slot:%d / %s : %s", 
        dwUID, dwCID, cSlot, DBComponent.GetErrorString(), szQuery);

    return false;
}

bool DeleteCharBattleServer(CDBComponent& DBComponent, unsigned long dwUID, unsigned long dwCID, 
                            unsigned char cSlot, unsigned char cCurrentServerGroupID, unsigned char cOldServerGroupID)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    char szQuery[CDBComponent::QUERY_BUFFER_LEN];

    if (0 < _snprintf(szQuery, CDBComponent::QUERY_BUFFER_LEN - 1, 
        "DeleteChar_BattleServer %d, %d, %d, %d, %d", 
        dwUID, cSlot, dwCID, cCurrentServerGroupID, cOldServerGroupID))
    {
        if (DBComponent.ExecuteQuery(szQuery)) 
        {
            return true;
        }
    }

    szQuery[CDBComponent::QUERY_BUFFER_LEN - 1] = 0;
    ERRLOG5(g_Log, "UID:%10u / CID:%10u / Slot:%d / %s %s", 
        dwUID, dwCID, cSlot, DBComponent.GetErrorString(), szQuery);

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
// GetEquip [ public ]
// - 장비 얻기
//
// Parameter :
//	1st : 캐릭터 아이디[In]
//	2st : 장비 데이터[Out]
//
// Return :
//	생성 성공 시 true, 실패시 false
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool GetEquip(CDBComponent& DBComponent, unsigned long CharID_In, LPEQUIP lpEquip_Out)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    memset(lpEquip_Out, 0, sizeof(EQUIP));

    _snprintf(DBComponent.GetQueryBuffer(), CDBComponent::MAX_QUERY_LENGTH, 
        "SELECT Equip FROM CharItem WHERE CID = %d", CharID_In );

    if (DBComponent.ExecuteQueryGetData(DBComponent.GetQueryBuffer(), lpEquip_Out)) 
    {
        if (sizeof(unsigned long) < lpEquip_Out->dwSize)
        {
            lpEquip_Out->dwSize -= sizeof(unsigned long);
        }
        else
        {
            lpEquip_Out->dwSize = 0;
        }

        return true;
    }

    ERRLOG3(g_Log, "CID:%10u %s %s", CharID_In, 
        DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
// GetInven [ public ]
// - 인벤 얻기
//
// Parameter :
//	1st : 캐릭터 아이디[In]
//	2st : 인벤 데이터[Out]
//
// Return :
//	생성 성공 시 true, 실패시 false
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool GetInven(CDBComponent& DBComponent, unsigned long CharID_In, LPINVEN lpInven_Out)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    memset(lpInven_Out, 0, sizeof(INVEN));

    _snprintf(DBComponent.GetQueryBuffer(), 
        CDBComponent::MAX_QUERY_LENGTH, "SELECT Inventory FROM CharItem WHERE CID = %d", CharID_In );

    if (DBComponent.ExecuteQueryGetData(DBComponent.GetQueryBuffer(), lpInven_Out)) 
    {
        if (sizeof(unsigned long) < lpInven_Out->dwSize)
        {
            lpInven_Out->dwSize -= sizeof(unsigned long);
        }
        else
        {
            lpInven_Out->dwSize = 0;
        }

        return true;
    }

    ERRLOG3(g_Log, "CID:%10u %s %s", CharID_In, 
        DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
// GetExtra [ public ]
// - 임시
//
// Parameter :
//	1st : 캐릭터 아이디[In]
//	2st : 임시 데이터[Out]
//
// Return :
//	생성 성공 시 true, 실패시 false
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool GetExtra(CDBComponent& DBComponent, unsigned long CharID_In, LPEXTRA lpExtra_Out)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    memset(lpExtra_Out, 0, sizeof(EXTRA));

    _snprintf(DBComponent.GetQueryBuffer(), 
        CDBComponent::MAX_QUERY_LENGTH, "SELECT Extra FROM CharItem WHERE CID = %d", CharID_In );

    if (DBComponent.ExecuteQueryGetData(DBComponent.GetQueryBuffer(), lpExtra_Out)) 
    {
        if (sizeof(unsigned long) < lpExtra_Out->dwSize)
        {
            lpExtra_Out->dwSize -= sizeof(unsigned long);
        }
        else
        {
            lpExtra_Out->dwSize = 0;
        }

        return true;
    }

    ERRLOG3(g_Log, "CID:%10u %s %s", CharID_In, 
        DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
// Exchange [ public ]
// - 교환
//
// Parameter :
//	1st : 캐릭터 아이디[In]
//	2st : 교환 데이터[Out]
//
// Return :
//	생성 성공 시 true, 실패시 false
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool GetExchange(CDBComponent& DBComponent, unsigned long CharID_In, 
                                      LPEXCHANGE lpExchange_Out)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    memset(lpExchange_Out, 0, sizeof(EXCHANGE));

    _snprintf(DBComponent.GetQueryBuffer(), 
        CDBComponent::MAX_QUERY_LENGTH, "SELECT Exchange FROM CharItemEx WHERE CID = %d", CharID_In );

    if (DBComponent.ExecuteQueryGetData(DBComponent.GetQueryBuffer(), lpExchange_Out))
    {
        if (sizeof(unsigned long) < lpExchange_Out->dwSize)
        {
            lpExchange_Out->dwSize -= sizeof(unsigned long);
        }
        else
        {
            lpExchange_Out->dwSize = 0;
        }

        return true;
    }

    ERRLOG3(g_Log, "CID:%10u %s %s", CharID_In, 
        DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
// GetQuick [ public ]
// - 퀵 얻기
//
// Parameter :
//	1st : 캐릭터 아이디[In]
//	2st : 퀵 데이터[Out]
//
// Return :
//	생성 성공 시 true, 실패시 false
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool GetQuick(CDBComponent& DBComponent, unsigned long CharID_In, LPQUICK lpQuick_Out)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    memset(lpQuick_Out, 0, sizeof(QUICK));

    _snprintf(DBComponent.GetQueryBuffer(), CDBComponent::MAX_QUERY_LENGTH, 
        "SELECT Quick FROM CharItem WHERE CID = %d", CharID_In );

    if (DBComponent.ExecuteQueryGetData(DBComponent.GetQueryBuffer(), lpQuick_Out)) 
    {
        return true;
    }

    ERRLOG3(g_Log, "CID:%10u %s %s", CharID_In, DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
// GetSpell [ public ]
// - 스펠 얻기
//
// Parameter :
//	1st : 캐릭터 아이디[In]
//	2st : 스펠 데이터[Out]
//
// Return :
//	생성 성공 시 true, 실패시 false
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool GetSpell(CDBComponent& DBComponent, unsigned long CharID_In, LPSPELL lpSpell_Out)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	memset(lpSpell_Out, 0, sizeof(SPELL));

	_snprintf(DBComponent.GetQueryBuffer(), CDBComponent::MAX_QUERY_LENGTH, 
		"SELECT Spell FROM CharSpell WHERE CID = %d", CharID_In );

	if (DBComponent.ExecuteQueryGetData(DBComponent.GetQueryBuffer(), lpSpell_Out)) 
	{
		return true;
	}

	ERRLOG3(g_Log, "CID:%10u %s %s", CharID_In, DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
// TempInven [ public ]
// - 임시 인벤토리
//
// Parameter :
//	1st : 캐릭터 아이디[In]
//	2st : 임시 인벤토리 데이터[Out]
//
// Return :
//	생성 성공 시 true, 실패시 false
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool GetTempInven(CDBComponent& DBComponent, unsigned long CharID_In, TEMPINVEN* lpTempInven_Out)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    memset(lpTempInven_Out, 0, sizeof(TEMPINVEN));

    _snprintf(DBComponent.GetQueryBuffer(), CDBComponent::MAX_QUERY_LENGTH, 
        "SELECT TempInven FROM CharItemEx WHERE CID = %d", CharID_In);

    if (DBComponent.ExecuteQueryGetData(DBComponent.GetQueryBuffer(), lpTempInven_Out)) 
    {
        if (sizeof(unsigned long) < lpTempInven_Out->dwSize)
        {
            lpTempInven_Out->dwSize -= sizeof(unsigned long);
        }
        else
        {
            lpTempInven_Out->dwSize = 0;
        }

        return true;
    }

    ERRLOG3(g_Log, "CID:%10u %s %s", CharID_In, 
        DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
// UpdateEquip [ public ]
// - 장비 업데이트
//
// Parameter :
//	1st : 캐릭터 아이디[In]
//	2st : 장비 데이터[Out]
//
// Return :
//	생성 성공 시 true, 실패시 false
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool UpdateEquip(CDBComponent& DBComponent, unsigned long CharID_In, LPEQUIP lpEquip_In)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    int	Counter = 0;
    char* pDest = NULL;

    if(0 < lpEquip_In->dwSize)
    {
        _snprintf(DBComponent.GetQueryBuffer(), 
            CDBComponent::MAX_QUERY_LENGTH, "SELECT Equip FROM CharItem WHERE CID = %d", CharID_In);

        lpEquip_In->dwSize += sizeof(unsigned long);
        if (DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer(), OleDB::Rowset_Update)) 
        {
            if (DBComponent.SetBinaryData(1, (OleDB::LPSET_BINARY)lpEquip_In)) 
            {
                lpEquip_In->dwSize -= sizeof(unsigned long);
                return true;
            }
        }

        lpEquip_In->dwSize -= sizeof(unsigned long);
    }
    else
    {
        _snprintf(DBComponent.GetQueryBuffer(), 
            CDBComponent::MAX_QUERY_LENGTH, "UPDATE CharItem SET Equip = NULL WHERE CID = %d", CharID_In);

        if (DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer())) 
        {
            return true;
        }
    }

    ERRLOG3(g_Log, "CID:%10u %s %s", CharID_In, 
        DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
    
    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
// UpdateInven [ public ]
// - 인벤 업데이트
//
// Parameter :
//	1st : 캐릭터 아이디[In]
//	2st : 인벤 데이터[Out]
//
// Return :
//	생성 성공 시 true, 실패시 false
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool UpdateInven(CDBComponent& DBComponent, unsigned long CharID_In, LPINVEN lpInven_In)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    int Counter = 0;
    char* pDest = NULL;

    if (0 < lpInven_In->dwSize)
    {
        _snprintf(DBComponent.GetQueryBuffer(), 
            CDBComponent::MAX_QUERY_LENGTH, "SELECT Inventory FROM CharItem WHERE CID = %d", CharID_In );

        lpInven_In->dwSize += sizeof(unsigned long);
        if (DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer(), OleDB::Rowset_Update)) 
        {
            if (DBComponent.SetBinaryData(1, (OleDB::LPSET_BINARY)lpInven_In)) 
            {
                lpInven_In->dwSize -= sizeof(unsigned long);
                return true;
            }
        }

        lpInven_In->dwSize -= sizeof(unsigned long);
    }
    else
    {
        _snprintf(DBComponent.GetQueryBuffer(), 
            CDBComponent::MAX_QUERY_LENGTH, "UPDATE CharItem SET Inventory = NULL WHERE CID = %d", CharID_In);
        
        if (DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer())) 
        {
            return true;
        }
    }

    SERLOG3(g_Log, "CID:%10u %s %s", CharID_In, 
        DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
    
    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
// UpdateExtra [ public ]
// - 인벤 퀵 슬롯
//
// Parameter :
//	1st : 캐릭터 아이디[In]
//	2st : 퀵[Out]
//
// Return :
//	생성 성공 시 true, 실패시 false
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool UpdateExtra(CDBComponent& DBComponent, unsigned long CharID_In, LPEXTRA lpExtra_In)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    if(0 < lpExtra_In->dwSize)
    {
        _snprintf(DBComponent.GetQueryBuffer(), 
            CDBComponent::MAX_QUERY_LENGTH, "SELECT Extra FROM CharItem WHERE CID = %d", CharID_In );

        lpExtra_In->dwSize += sizeof(unsigned long);
        if (DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer(), OleDB::Rowset_Update)) 
        {
            if (DBComponent.SetBinaryData(1, (OleDB::LPSET_BINARY)lpExtra_In)) 
            {
                lpExtra_In->dwSize -= sizeof(unsigned long);
                return true;
            }
        }

        lpExtra_In->dwSize -= sizeof(unsigned long);
    }
    else
    {
        _snprintf(DBComponent.GetQueryBuffer(), 
            CDBComponent::MAX_QUERY_LENGTH, "UPDATE CharItem SET Extra = NULL WHERE CID = %d", CharID_In);

        if (DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer())) 
        {
            return true;
        }
    }

    ERRLOG3(g_Log, "CID:%10u %s %s", CharID_In, 
        DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
    
    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
// UpdateExchange [ public ]
// - 교환 업데이트
//
// Parameter :
//	1st : 캐릭터 아이디[In]
//	2st : 교환[In]
//
// Return :
//	생성 성공 시 true, 실패시 false
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool UpdateExchange(CDBComponent& DBComponent, unsigned long CharID_In, 
                                         LPEXCHANGE lpExchange_In)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    if (0 < lpExchange_In->dwSize)
    {
        _snprintf(DBComponent.GetQueryBuffer(), 
            CDBComponent::MAX_QUERY_LENGTH, "SELECT Exchange FROM CharItemEx WHERE CID = %d", CharID_In );

        lpExchange_In->dwSize += sizeof(unsigned long);
        if (DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer(), OleDB::Rowset_Update)) 
        {
            if (DBComponent.SetBinaryData(1, (OleDB::LPSET_BINARY)lpExchange_In)) 
            {
                lpExchange_In->dwSize -= sizeof(unsigned long);
                return true;
            }
        }

        lpExchange_In->dwSize -= sizeof(unsigned long);
    }
    else 
    {
        _snprintf(DBComponent.GetQueryBuffer(), 
            CDBComponent::MAX_QUERY_LENGTH, "UPDATE CharItemEx SET Exchange = NULL WHERE CID = %d", CharID_In);
        
        if (DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer())) 
        {
            return true;
        }
    }

    ERRLOG3(g_Log, "CID:%10u %s %s", CharID_In, 
        DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
    
    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
// UpdateTempInven [ public ]
// - 임시 인벤토리 업데이트
//
// Parameter :
//	1st : 캐릭터 아이디[In]
//	2st : 임시 인벤토리[In]
//
// Return :
//	생성 성공 시 true, 실패시 false
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool UpdateTempInven(CDBComponent& DBComponent, unsigned long CharID_In,
                                          TEMPINVEN* lpTempInven_In)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    if (0 < lpTempInven_In->dwSize)
    {
        _snprintf(DBComponent.GetQueryBuffer(), CDBComponent::MAX_QUERY_LENGTH, 
            "SELECT TempInven FROM CharItemEx WHERE CID = %d", CharID_In );

        lpTempInven_In->dwSize += sizeof(unsigned long);
        if (DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer(), OleDB::Rowset_Update)) 
        {
            if (DBComponent.SetBinaryData(1, (OleDB::LPSET_BINARY)lpTempInven_In)) 
            {
                lpTempInven_In->dwSize -= sizeof(unsigned long);
                return true;
            }
        }

        lpTempInven_In->dwSize -= sizeof(unsigned long);
    }
    else
    {
        _snprintf(DBComponent.GetQueryBuffer(), CDBComponent::MAX_QUERY_LENGTH, 
            "UPDATE CharItemEx SET TempInven = NULL WHERE CID = %d", CharID_In );
        
        if (DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer())) 
        {
            return true;
        }
    }

    ERRLOG3(g_Log, "CID:%10u %s %s", CharID_In, 
        DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
    
    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
// UpdateQuick [ public ]
// - 인벤 퀵 슬롯
//
// Parameter :
//	1st : 캐릭터 아이디[In]
//	2st : 퀵[Out]
//
// Return :
//	생성 성공 시 true, 실패시 false
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool UpdateQuick(CDBComponent& DBComponent, unsigned long CharID_In, LPQUICK lpQuick_In)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    int Counter = 0;
    char* pDest = NULL;

    _snprintf(DBComponent.GetQueryBuffer(), CDBComponent::MAX_QUERY_LENGTH, "UpdateCharItemQuick %d, 0x", CharID_In);
    pDest = DBComponent.GetQueryBuffer() + strlen(DBComponent.GetQueryBuffer());

    BYTE *lpPos = (BYTE*)lpQuick_In;
    for (Counter = 0; Counter < sizeof(QUICK); ++Counter, ++lpPos, pDest += 2) {
        Math::Convert::Hex08ToStr(pDest, *lpPos);
    }

    if (DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer())) {
        return true;
    }

    ERRLOG3(g_Log, "CID:%10u %s %s", CharID_In, DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
// UpdateSpell [ public ]
// - 스펠 업데이트
//
// Parameter :
//	1st : 캐릭터 아이디[In]
//	2st : 스펠 데이터[Out]
//
// Return :
//	생성 성공 시 true, 실패시 false
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool UpdateSpell(CDBComponent& DBComponent, unsigned long CharID_In, LPSPELL lpSpell_In)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	int Counter = 0;
	char* pDest = NULL;

	_snprintf(DBComponent.GetQueryBuffer(), CDBComponent::MAX_QUERY_LENGTH, "UpdateCharSpell %d, 0x", CharID_In);
	pDest = DBComponent.GetQueryBuffer() + strlen(DBComponent.GetQueryBuffer());

	BYTE *lpPos = (BYTE*)lpSpell_In;
	for (Counter = 0; Counter < sizeof(SPELL); ++Counter, ++lpPos, pDest += 2) {
		Math::Convert::Hex08ToStr(pDest, *lpPos);
	}

	if (DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer())) {
		return true;
	}

	ERRLOG3(g_Log, "CID:%10u %s %s", CharID_In, DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
// GetCharInfo [ public ]
// - 캐릭터 정보 얻기
//
// Parameter :
//	1st : 캐릭터 아이디[In]
//	2st : 캐릭터 정보 구조체[Out]
//
// Return :
//	생성 성공 시 true, 실패시 false
//
///////////////////////////////////////////////////////////////////////////////////////////////

bool GetCharInfo(CDBComponent& DBComponent, unsigned long CharID_In, 
				 LPCHAR_INFOST lpCharInfo_In, unsigned short* EquiView_In, const int nEquipView)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	_snprintf( DBComponent.GetQueryBuffer(), 
		CDBComponent::MAX_QUERY_LENGTH, "GetCharInfo %d", CharID_In );

	const int CHARINFO_SIZE = sizeof(CHAR_INFOST);
	const int EQUIPVIEW_NUM  = 15;
	const int EQUIPVIEW_SIZE = sizeof(unsigned short) * EQUIPVIEW_NUM;
	const int MAX_BUFFER =  (CHARINFO_SIZE + EQUIPVIEW_SIZE) * 2;
	char szBuffer[MAX_BUFFER] = { 0, };

	memset(lpCharInfo_In, 0, sizeof(CHAR_INFOST));
	memset(EquiView_In, 0, min(EQUIPVIEW_SIZE, sizeof(unsigned short) * nEquipView));

	if (DBComponent.ExecuteQueryGetData(DBComponent.GetQueryBuffer(), szBuffer)) 
	{
		memcpy(lpCharInfo_In, szBuffer, CHARINFO_SIZE);
		memcpy(EquiView_In, szBuffer + CHARINFO_SIZE, 
			min(EQUIPVIEW_SIZE, sizeof(unsigned short) * nEquipView));

		return true;
	}

	ERRLOG3(g_Log, "CID:%10u %s %s", CharID_In, DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
// GetCharPos [ public ]
// - 캐릭터 위치 정보 얻기
//
// Parameter :
//	1st : 캐릭터 아이디[In]
//	2st : 캐릭터 위치 구조체[Out]
//
// Return :
//	생성 성공 시 true, 실패시 false
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool GetCharPos(CDBComponent& DBComponent, unsigned long CharID_In, LPCHAR_POS lpCharPos_Out)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    memset(lpCharPos_Out, 0, sizeof(CHAR_POS));

    _snprintf(DBComponent.GetQueryBuffer(), 
        CDBComponent::MAX_QUERY_LENGTH, "dbo.GetCharPos %d", CharID_In);

    if (DBComponent.ExecuteQueryGetData(DBComponent.GetQueryBuffer(), lpCharPos_Out)) {
        return true;
    }

    ERRLOG3(g_Log, "CID:%10u %s %s", CharID_In, DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
// GetCharSkill [ public ]
// - 캐릭터 스킬 얻기
//
// Parameter :
//	1st : 캐릭터 아이디[In]
//	2st : 캐릭터 스킬 구조체[In]
//
// Return :
//	생성 성공 시 true, 실패시 false
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool GetCharSkill(CDBComponent& DBComponent, unsigned long CharID_In, LPSKILL lpSkill_Out)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    memset(lpSkill_Out, 0, sizeof(SKILL));

    _snprintf(DBComponent.GetQueryBuffer(), 
        CDBComponent::MAX_QUERY_LENGTH, "GetCharSkill %d", CharID_In);

    if (DBComponent.ExecuteQueryGetData(DBComponent.GetQueryBuffer(), lpSkill_Out)) {
        return true;
    }

    ERRLOG3(g_Log, "CID:%10u %s %s", CharID_In, DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
// UpdateCharInfo [ public ]
// - 캐릭터 정보 업데이트
//
// Parameter :
//	1st : 캐릭터 아이디[In]
//	2st : 캐릭터 정보 구조체[In]
//
// Return :
//	생성 성공 시 true, 실패시 false
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool UpdateCharInfo(CDBComponent& DBComponent, unsigned long CharID_In, 
                                         LPCHAR_INFOST lpCharInfo_In, unsigned short *EquipData_In,
                                         const int nEquipView)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);
    
    char* pDest = NULL;

    char Text[100] = "";
    Math::Convert::Hex64ToStr(Text, lpCharInfo_In->Exp);
    _snprintf(DBComponent.GetQueryBuffer(), CDBComponent::MAX_QUERY_LENGTH, "UpdateCharInfo %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, 0x%s, %d, 0x", 
        CharID_In, lpCharInfo_In->Class, lpCharInfo_In->Fame, lpCharInfo_In->Mileage, lpCharInfo_In->Level, lpCharInfo_In->Gold, lpCharInfo_In->IP, 
        lpCharInfo_In->STR, lpCharInfo_In->DEX, lpCharInfo_In->CON, lpCharInfo_In->INT, lpCharInfo_In->WIS, lpCharInfo_In->HP, lpCharInfo_In->MP,
        Text, lpCharInfo_In->Chance);
    pDest = DBComponent.GetQueryBuffer() + strlen(DBComponent.GetQueryBuffer());

    BYTE *lpPos = (BYTE*)EquipData_In;
    for (unsigned int Counter = 0; Counter < sizeof(unsigned short) * nEquipView; ++Counter, ++lpPos, pDest += 2) {
        Math::Convert::Hex08ToStr(pDest, *lpPos);
    }

    if (DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer())) {
        return true;
    }

    ERRLOG3(g_Log, "CID:%10u %s %s", CharID_In, DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());

    return false;
}


///////////////////////////////////////////////////////////////////////////////////////////////
//
// UpdateCharPos [ public ]
// - 캐릭터 위치 정보 업데이트
//
// Parameter :
//	1st : 캐릭터 아이디[In]
//	2st : 캐릭터 위치 정보 구조체[In]
//
// Return :
//	생성 성공 시 true, 실패시 false
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool UpdateCharPos(CDBComponent& DBComponent, unsigned long CharID_In, LPCHAR_POS lpCharPos_In)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    // Real값이 부정확해서 빠지는 것을 막기 위해 정수로 캐스팅함.
    _snprintf(DBComponent.GetQueryBuffer(), CDBComponent::MAX_QUERY_LENGTH, 
        "UpdateCharPos %d, %d, %d, %d, %d, %d, %d", CharID_In, 
        static_cast<int>(lpCharPos_In->LastPoint.fPointX), 
        static_cast<int>(lpCharPos_In->LastPoint.fPointY), 
        static_cast<int>(lpCharPos_In->LastPoint.fPointZ), 		
		// edith 2009.07.24 SavePoint 값을 LastPoint 값으로 변경.
		static_cast<int>(lpCharPos_In->LastPoint.fPointX), 
		static_cast<int>(lpCharPos_In->LastPoint.fPointY), 
		static_cast<int>(lpCharPos_In->LastPoint.fPointZ));
//        static_cast<int>(lpCharPos_In->SavePoint.fPointX), 
//        static_cast<int>(lpCharPos_In->SavePoint.fPointY), 
//        static_cast<int>(lpCharPos_In->SavePoint.fPointZ));

    if (DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer())) 
    {
        return true;
    }

    ERRLOG3(g_Log, "CID:%10u %s %s", CharID_In, DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());
    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
// UpdateCharSkill [ public ]
// - 캐릭터 캐릭터 스킬 업데이트
//
// Parameter :
//	1st : 캐릭터 아이디[In]
//	2st : 캐릭터 스킬 정보 구조체[In]
//
// Return :
//	생성 성공 시 true, 실패시 false
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool UpdateCharSkill(CDBComponent& DBComponent, unsigned long CharID_In, LPSKILL lpSkill_In)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    int Counter = 0;
    char* pDest = NULL;

    _snprintf(DBComponent.GetQueryBuffer(), CDBComponent::MAX_QUERY_LENGTH, "UpdateCharSkill %d, 0x", CharID_In);
    pDest = DBComponent.GetQueryBuffer() + strlen(DBComponent.GetQueryBuffer());

    BYTE *lpPos = (BYTE*)lpSkill_In;
    for (Counter = 0; Counter < sizeof(SKILL); ++Counter, ++lpPos, pDest += 2) {
        Math::Convert::Hex08ToStr(pDest, *lpPos);
    }

    if (DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer())) 
    {
        return true;
    }

    ERRLOG3(g_Log, "CID:%10u %s %s", CharID_In, 
        DBComponent.GetErrorString(), DBComponent.GetQueryBuffer());

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
// UpdateCharParty [ public ]
// - 파티 업데이트
//
// Parameter :
//	1st : 파티 아이디[Out]
//
// Return :
//	생성 성공 시 true, 실패시 false
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool UpdateCharParty(CDBComponent& DBComponent, unsigned long CharID_In, unsigned long PartyID_In)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    char Query[100];

    _snprintf( Query, 100, "dbo.UpdateCharParty %d, %d", CharID_In, PartyID_In );

    if (DBComponent.ExecuteQuery(Query)) {
        return true;
    }

    SERLOG3(g_Log, "CID:%10u PID:%10u %s", CharID_In, PartyID_In, DBComponent.GetErrorString());	// cppis 가 플러쉬없이 바로 로그하기위해서 추가함
    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
// UpdateCharEquip [ public ]
// - 캐릭터 보기 장비 업데이터
//
// Parameter :
//	1st : 장비[In]
//
// Return :
//	생성 성공 시 true, 실패시 false
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool UpdateCharEquip(CDBComponent& DBComponent, unsigned long CharID_In, unsigned short *lpEquip_In)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    int Counter = 0;
    char* pDest = NULL;

    _snprintf(DBComponent.GetQueryBuffer(), CDBComponent::MAX_QUERY_LENGTH, "dbo.UpdateCharEquip %d, 0x", CharID_In);
    pDest = DBComponent.GetQueryBuffer() + strlen(DBComponent.GetQueryBuffer());

    BYTE *lpPos = (BYTE*)lpEquip_In;
    for (Counter = 0; Counter < sizeof(unsigned short) * 15; ++Counter, ++lpPos, pDest += 2) {
        Math::Convert::Hex08ToStr(pDest, *lpPos);
    }

    if (DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer())) {
        return true;
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
// InsertParty [ public ]
// - 파티 추가
//
// Parameter :
//	1st : 파티 아이디[Out]
//
// Return :
//	생성 성공 시 true, 실패시 false
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool InsertParty(CDBComponent& DBComponent, LPPARTY_DB_INFO lpParty_Info_In, unsigned long *PartyID_Out)
{
	char szInfo[0xff] = {0,};
	char *pDest;

	int Counter;
	int i;

	_snprintf(DBComponent.GetQueryBuffer(), CDBComponent::MAX_QUERY_LENGTH, "dbo.InsertParty_Part2 0x");

	pDest = DBComponent.GetQueryBuffer() + strlen( DBComponent.GetQueryBuffer() );
		
	BYTE *lpPos = (BYTE*)&lpParty_Info_In->PartyInfo;

	for(Counter = 0; Counter < sizeof(PARTY_INFO); ++Counter, ++lpPos, pDest += 2)
	{
		Math::Convert::Hex08ToStr(pDest, *lpPos);
	}

	strcat(DBComponent.GetQueryBuffer(), ", 0x");

	pDest = DBComponent.GetQueryBuffer() + strlen( DBComponent.GetQueryBuffer() );
	
	lpPos = (BYTE*)&lpParty_Info_In->PartyUserInfo;

	for(Counter = 0; Counter < sizeof(PARTY_USER_INFO); ++Counter, ++lpPos, pDest += 2)
	{
		Math::Convert::Hex08ToStr(pDest, *lpPos);
	}	

	if (DBComponent.ExecuteQueryGetData(DBComponent.GetQueryBuffer(), (void *)PartyID_Out))
	{
		for (i = 0; i < 10; i++)//hayzohar was 10 // avalansa was 12 by hayzohar
		{
			if (lpParty_Info_In->PartyInfo.MemberCID[i] != 0)
			{
				bool bRet = UpdateCharParty(DBComponent, lpParty_Info_In->PartyInfo.MemberCID[i], *PartyID_Out);

				if (!bRet)
				{
					ERRLOG1(g_Log, "UpdateCharParty 를 실패했습니다. %s", DBComponent.GetErrorString());
				}
			}
		}

		return true;
	}

	SERLOG1(g_Log, "%s", DBComponent.GetErrorString());	// cppis 가 플러쉬없이 바로 로그하기위해서 추가함
	return false;	
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
// DeleteParty [ public ]
// - 파티 제거
//
// Parameter :
//	1st : 파티 구조체[In]
//	1st : 캐릭터 아이디[In]
//
// Return :
//	생성 성공 시 true, 실패시 false
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool DeleteParty(CDBComponent& DBComponent, LPPARTY_DB_INFO lpParty_Info_In, unsigned long PartyID_In)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    char Query[CDBComponent::QUERY_BUFFER_LEN];
    int Counter;

    _snprintf(Query, CDBComponent::QUERY_BUFFER_LEN,  "dbo.DeleteParty %d", PartyID_In);

    if (DBComponent.ExecuteQuery(Query))
	{
        for(Counter = 0; Counter < 10; Counter++)//hayzohar was 10 // avalansa was 12 by hayzohar
		{
            if(lpParty_Info_In->PartyInfo.MemberCID[Counter] != 0)
			{
                bool bRet = UpdateCharParty(DBComponent, lpParty_Info_In->PartyInfo.MemberCID[Counter], 0);
                
				if (!bRet)
				{
                    ERRLOG1(g_Log, "UpdateCharParty 를 실패했습니다. %s", DBComponent.GetErrorString());
                }
            }
        }

        return true;
    }

    SERLOG2(g_Log, "PID:%10u %s", PartyID_In, DBComponent.GetErrorString());	// cppis 가 플러쉬없이 바로 로그하기위해서 추가함
    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
// GetPartyInfo [ public ]
// - 파티 정보 얻기
//
// Parameter :
//	1st : 파티 아이디[In]
//	2st : 정보 포인터[Out]
//
// Return :
//	생성 성공 시 true, 실패시 false
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool GetPartyInfo(CDBComponent& DBComponent, unsigned long PartyID_In, LPPARTY_DB_INFO lpPartyInfo_Out)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    memset(lpPartyInfo_Out, 0, sizeof(PARTY_DB_INFO));

    _snprintf(DBComponent.GetQueryBuffer(), 
        CDBComponent::QUERY_BUFFER_LEN, "dbo.GetPartyInfo_Part2 %d", PartyID_In);

    if (DBComponent.ExecuteQueryGetData(DBComponent.GetQueryBuffer(), (void *)lpPartyInfo_Out)) {
        return true;
    }

    SERLOG2(g_Log, "PID:%10u %s", PartyID_In, DBComponent.GetErrorString());	// cppis 가 플러쉬없이 바로 로그하기위해서 추가함
    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
// UpdatePartyInfo [ public ]
// - 파티 정보 얻기
//
// Parameter :
//	1st : 파티 아이디[In]
//	2st : 정보 포인터[In]
//
// Return :
//	생성 성공 시 true, 실패시 false
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool UpdatePartyInfo(CDBComponent& DBComponent, unsigned long PartyID_In, LPPARTY_DB_INFO lpPartyInfo_In)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	char szInfo[0xff] = {0,};
	char *pDest;

    int Counter;

    _snprintf(DBComponent.GetQueryBuffer(), CDBComponent::MAX_QUERY_LENGTH, "dbo.UpdatePartyInfo_Part2 %d, 0x", PartyID_In);
    
	pDest = DBComponent.GetQueryBuffer() + strlen( DBComponent.GetQueryBuffer() );

	BYTE *lpPos = (BYTE*)&lpPartyInfo_In->PartyInfo;

	for(Counter = 0; Counter < sizeof(PARTY_INFO); ++Counter, ++lpPos, pDest += 2)
	{
		Math::Convert::Hex08ToStr(pDest, *lpPos);
	}

	strcat(DBComponent.GetQueryBuffer(), ", 0x");

	pDest = DBComponent.GetQueryBuffer() + strlen( DBComponent.GetQueryBuffer() );

	lpPos = (BYTE*)&lpPartyInfo_In->PartyUserInfo;

	for(Counter = 0; Counter < sizeof(PARTY_USER_INFO); ++Counter, ++lpPos, pDest += 2)
	{
		Math::Convert::Hex08ToStr(pDest, *lpPos);
	}

    if (DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer())) {
        return true;
    }	

    SERLOG2(g_Log, "PID:%10u %s", PartyID_In, DBComponent.GetErrorString());	// cppis 가 플러쉬없이 바로 로그하기위해서 추가함
    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
// InsertPartyMember [ public ]
// - 파티 맴버 추가
//
// Parameter :
//	1st : 파티 아이디[In]
//	2st : 정보 포인터[In]
//	3st : 캐릭터 아이디[In]
//
// Return :
//	생성 성공 시 true, 실패시 false
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool InsertPartyMember(CDBComponent& DBComponent, unsigned long PartyID_In, LPPARTY_DB_INFO lpPartyInfo_In, unsigned long CharID_In)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    if (UpdatePartyInfo(DBComponent, PartyID_In, lpPartyInfo_In))
	{
        if (UpdateCharParty(DBComponent, CharID_In, PartyID_In))
		{
            return true;
        }
    }

    SERLOG2(g_Log, "PID:%10u %s", PartyID_In, DBComponent.GetErrorString());	// cppis 가 플러쉬없이 바로 로그하기위해서 추가함
    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
// DeletePartyMember [ public ]
// - 파티 맴버 제거
//
// Parameter :
//	1st : 파티 아이디[In]
//	2st : 정보 포인터[In]
//	3st : 캐릭터 아이디[In]
//
// Return :
//	생성 성공 시 true, 실패시 false
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool DeletePartyMember(CDBComponent& DBComponent, unsigned long PartyID_In, LPPARTY_DB_INFO lpPartyInfo_In, unsigned long CharID_In)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    if (UpdatePartyInfo(DBComponent, PartyID_In, lpPartyInfo_In))
	{
        if (UpdateCharParty(DBComponent, CharID_In, 0))
		{
            return true;
        }
    }

    SERLOG2(g_Log, "PID:%10u %s", PartyID_In, DBComponent.GetErrorString());	// cppis 가 플러쉬없이 바로 로그하기위해서 추가함
    return false;
}


bool InsertItemDuplicatedLog(CDBComponent& DBComponent, unsigned __int64 dwItemSerial, 
                                                  const char* szName, unsigned long dwUID, 
                                                  unsigned long dwCID, unsigned long dwQty)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    _snprintf(DBComponent.GetQueryBuffer(), CDBComponent::QUERY_BUFFER_LEN, 
        "dbo.USP_InsertItemLog '%u', '%u', '%I64d', '%s', '%u'", dwUID, dwCID, dwItemSerial, szName, dwQty);

    if (DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer()))
    {
        return true;
    }

    SERLOG2(g_Log, "%s : %s", DBComponent.GetQueryBuffer(), 
        DBComponent.GetErrorString());	// cppis 가 플러쉬없이 바로 로그하기위해서 추가함
    return false;
}

bool InsertHackLog(CDBComponent& DBComponent, unsigned long dwUID, unsigned long dwCID, const char* szName)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    _snprintf(DBComponent.GetQueryBuffer(), CDBComponent::QUERY_BUFFER_LEN, 
        "dbo.InsertHackLog '%d', '%d', '%s'", dwUID, dwCID, szName);

    if (DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer()))
    {
        return true;
    }

	return false;
}

// 전쟁 플래그 관련
bool UpdateGuildWarFlag(CDBComponent& DBComponent, unsigned long dwCID, unsigned char cFlag)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	_snprintf(DBComponent.GetQueryBuffer(), CDBComponent::QUERY_BUFFER_LEN, 
		"UPDATE CharInfoEx SET GuildWarFlag=%d WHERE CID=%d", cFlag, dwCID);

	if (DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer()))
	{
		return true;
	}

	SERLOG2(g_Log, "%s : %s", DBComponent.GetQueryBuffer(), DBComponent.GetErrorString());	// cppis 가 플러쉬없이 바로 로그하기위해서 추가함
	return false;
}

bool UpdateRealmWarFlag(CDBComponent& DBComponent, unsigned long dwCID, unsigned char cFlag)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	_snprintf(DBComponent.GetQueryBuffer(), CDBComponent::QUERY_BUFFER_LEN, 
		"UPDATE CharInfoEx SET RealmWarFlag=%d WHERE CID=%d", cFlag, dwCID);

	if (DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer()))
	{
		return true;
	}

	SERLOG2(g_Log, "%s : %s", DBComponent.GetQueryBuffer(), DBComponent.GetErrorString());	// cppis 가 플러쉬없이 바로 로그하기위해서 추가함
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
// InsertAdminCommandLog [ public ]
// - 파티 맴버 제거
//
// Parameter :
//	1st : 운영자 캐릭터 CID
//	2st : 해당 캐릭터 CID
//	3st : 명령어
//
// Return :
//	생성 성공 시 true, 실패시 false
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool InsertAdminCommandLog(CDBComponent& DBComponent, unsigned long dwServerGroupID, unsigned long dwAdminCID, unsigned long dwCID, char* szCommand)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	_snprintf(DBComponent.GetQueryBuffer(), CDBComponent::QUERY_BUFFER_LEN, 
		"Insert Into AdminCommandLog(ServerGroupID, AdminCID, CID, Type) Values(%d, %d, %d, '%s')", dwServerGroupID, dwAdminCID, dwCID, szCommand);
	
	if (DBComponent.ExecuteQuery(DBComponent.GetQueryBuffer()))
	{
		return true;
	}

	return false;
}

#pragma pack(1)

struct ChangeCharNameInfo
{    
    char            m_szName[CHAR_INFOST::MAX_NAME_LEN];
    unsigned long   m_dwCID;
};

#pragma pack()

// 캐릭터 이름 변경
bool ChangeCharName(CDBComponent& DBComponent, unsigned long dwCID, const char* szCharName,
                    unsigned long& dwResult)
{
    DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

    ChangeCharNameInfo changeCharNameInfo;
    memset(&changeCharNameInfo, 0, sizeof(ChangeCharNameInfo));

    changeCharNameInfo.m_dwCID = dwCID;
    strncpy(changeCharNameInfo.m_szName, szCharName, sizeof(char) * CHAR_INFOST::MAX_NAME_LEN);
    changeCharNameInfo.m_szName[CHAR_INFOST::MAX_NAME_LEN - 1] = 0;

    dwResult = 0;
    const char* szQuery = "UPDATE dbo.CharInfo SET Name=? WHERE CID=?";

    if (DBComponent.ExecuteQueryWithParams(szQuery, 
        reinterpret_cast<char*>(&changeCharNameInfo),
        CGameDBParams::GetInstance().GetCharChangeNameParam()))
    {
        dwResult = 0;
        return true;
    }
    else if(DB_E_INTEGRITYVIOLATION == DBComponent.GetLastError())
    {
        dwResult = 1;
        return true;
    }

    SERLOG5(g_Log, "CID:%10u / %s / %s : (0x%08X)%s", 
        dwCID, szCharName, szQuery, DBComponent.GetLastError(), DBComponent.GetErrorString());

    return false;
}

/*
bool GetKeyInfo(CDBComponent& DBComponent, KeyInfo* pKeyInfo, unsigned long dwUID)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	_snprintf(DBComponent.GetQueryBuffer(), CDBComponent::QUERY_BUFFER_LEN, "Select KeyInfo From UserInfo Where UID = %u", dwUID);

	if (DBComponent.ExecuteQueryGetData(DBComponent.GetQueryBuffer(), (void*)pKeyInfo))
	{
		return true;
	}
	
	return false;
}

bool UpdateKeyInfo(CDBComponent& DBComponent, KeyInfo* pKeyInfo, unsigned long dwUID)
{
	DBOBJECT_PERFORMANCE_CHECK(FunctionTimingCheck);

	char szQuery[CDBComponent::QUERY_BUFFER_LEN];

	_snprintf(szQuery, CDBComponent::QUERY_BUFFER_LEN - 1, "Update UserInfo Set KeyInfo = ? Where UID = %u", dwUID);

	OleDB::PARAM_INFO paramInfo;
	memset(&paramInfo, 0, sizeof(OleDB::PARAM_INFO));

	paramInfo.ColNum		= 1;
	paramInfo.eParamIO[0]   = DBPARAMIO_INPUT;
	paramInfo.ColType[0]    = DBTYPE_BYTES;
	paramInfo.ColSize[0]    = sizeof(KeyInfo);
	
	if (!DBComponent.ExecuteQueryWithParams(szQuery, reinterpret_cast<char*>(pKeyInfo), paramInfo))
	{
		return false;
	}

	return true;
}
*/
}
}