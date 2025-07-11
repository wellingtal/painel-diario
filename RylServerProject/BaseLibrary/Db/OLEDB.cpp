///////////////////////////////////////////////////////////////////////////////////////////////
//
// OleDB
//
//////////////////////////////////////////////////////////////////////////////////////////////
#define DBINITCONSTANTS
#define INITGUID

#define _WIN32_WINNT 0x0500

#include "stdafx.h"

#include <winsock2.h>
#include <windows.h>

#include <crtdbg.h>
#include <objbase.h>

#include "oledb.h"

#include <msdasc.h>
#include <msdasql.h>
#include <msdaora.h>
#include <sqloledb.h>

#include "../Log/ServerLog.h"
#include "../Utility/Debug/PerformanceCheck.h"

#define SAFE_UNINIT(p)			{ if (p) { (p)->Uninitialize(); (p)->Release(); (p) = NULL; } }
#define SAFE_RELEASE(p)			{ if (p) { (p)->Release(); (p) = NULL; } }
#define SAFE_DELETE(p)			{ if (p) { delete (p); (p) = NULL; } }
#define SAFE_FREE(p)			{ if (p) { free(p); (p) = NULL; } }

//Interface////////////////////////////////////////////////////////////////////////////////////
//
// OleDB 생성자 [ public ]     
// - COM 초기화
// - 멤버 변수 초기화
//
///////////////////////////////////////////////////////////////////////////////////////////////
OleDB::OleDB(void) :
m_pIDBInit(NULL), m_pIDBCreateSession(NULL), m_pIDBCreateCommand(NULL),
m_pIRowset(NULL), m_pIRowsetChange(NULL), m_dwLastError(0), m_dwQueryErrorCount(0)
{
	
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	ZeroMemory(&m_ColInfo, sizeof(COL_INFO));
	ZeroMemory(m_ErrorString, sizeof(char) * MaxErrorLen);
}

//Interface/////////////////////////////////////////////////////////////////////////////////////////////
//
// OleDB 소멸자 [ public ]
// - 인터페이스 릴리즈
// - 컴 릴리즈
//
///////////////////////////////////////////////////////////////////////////////////////////////
OleDB::~OleDB(void)
{
	SAFE_UNINIT(m_pIDBInit);

	SAFE_RELEASE(m_pIDBCreateSession);
	SAFE_RELEASE(m_pIDBCreateCommand);
	SAFE_RELEASE(m_pIRowset);
	SAFE_RELEASE(m_pIRowsetChange);

	CoUninitialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
// ConnectDataSourcePrompt [ public ]
// - 데이터 소스 프롬프트로 부터 데이터 소스 얻기
//
// Parameter :
//	1st : 부모 윈도우 핸들
//
// Return :
//	생성 성공 시 true, 실패시 false
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool OleDB::ConnectDataSourcePrompt(HWND hWnd_In)
{
	HRESULT hResult = NULL;

	if(hWnd_In == NULL)
		return false;

	// COM 초기화
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	IDBPromptInitialize* pIDBPromptInitialize = NULL;
	if(FAILED(CoCreateInstance(CLSID_DataLinks, NULL, CLSCTX_INPROC_SERVER, IID_IDBPromptInitialize, (void**)&pIDBPromptInitialize)))
		return false;
	
	SAFE_RELEASE(m_pIDBInit);

	// 프롬프트 데이터 소스 열기
	if(FAILED(hResult = pIDBPromptInitialize->PromptDataSource(NULL, hWnd_In, DBPROMPTOPTIONS_PROPERTYSHEET, 0, NULL, NULL, IID_IDBInitialize, (IUnknown**)&m_pIDBInit)))
	{
		HandleError(__LINE__, hResult, "IDBPromptInitialize::PromptDataSource");

		SAFE_RELEASE(pIDBPromptInitialize);
		return false;
	}

	// 데이터 소스 오브젝트 초기화
	if(FAILED(hResult = m_pIDBInit->Initialize()))
   	{
		HandleError(__LINE__, hResult, "IDBInitialize::Initialize");

		SAFE_RELEASE(m_pIDBInit);
		return false;
	}

	return CreateSession();
}

EXTERN_C const CLSID CLSID_OraOLEDB = \
    {0x3F63C36E,0x51A3,0x11D2,{0xBB,0x7D,0x00,0xC0,0x4F,0xA3,0x00,0x80}};

///////////////////////////////////////////////////////////////////////////////////////////////
//
// ConnectSQLServer [ public ]
// - SQL에 연결 하여 데이터 소스를 얻기
//
// Parameter :
//	1st	: DB 서버 이름/데이터 소스 이름[In]
//	2st : 유저 아이디[In]
//	3st : 유저 패스워드[In]
//	4st : DB 이름[In]
//
// Return :
//	생성 성공 시 true, 실패시 false
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool OleDB::ConnectSQLServer(LPCTSTR ServerName_In, LPCTSTR DataBaseName_In, LPCTSTR UserID_In, LPCTSTR UserPass_In, ConnType ConnType_In)
{
	CLSID ProviderCLSID = CLSID_MSDASQL;
	switch(ConnType_In)
	{
	case ConnType_ODBC:		ProviderCLSID = CLSID_MSDASQL;	DataBaseName_In = _T("");	break;
	case ConnType_MSSQL:	ProviderCLSID = CLSID_SQLOLEDB;	break;
	case ConnType_ORACLE:	ProviderCLSID = CLSID_MSDAORA;	break;
	}

	HRESULT hResult;

	if(ServerName_In == NULL || UserID_In == NULL || UserPass_In == NULL)
		return false;

	SAFE_RELEASE(m_pIDBInit);

	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if(FAILED(CoCreateInstance(ProviderCLSID, NULL, CLSCTX_INPROC_SERVER, IID_IDBInitialize, (void **)&m_pIDBInit)))
		return false;

	SetConnectionProperties(ServerName_In, DataBaseName_In, UserID_In, UserPass_In);

	// 데이터 소스 오브젝트 초기화
	if(FAILED(hResult = m_pIDBInit->Initialize()))
   	{
		HandleError(__LINE__, hResult, "IDBInitialize::Initialize");

		SAFE_RELEASE(m_pIDBInit);
		return false;
	}

	return CreateSession();
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
// DisconnectDataSource [ public ]
// - 데이터 소스로 부터 연결 헤제
//
// Parameter :
//
// Return :
//	생성 성공 시 true, 실패시 false
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool OleDB::DisconnectDataSource(void)
{
	SAFE_RELEASE(m_pIDBCreateSession);

    m_dwLastError = 0;
	m_dwQueryErrorCount = 0;
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
// ExecuteQuery [ public ]
// - 쿼리를 실행한다.
//
// Parameter :
//	1st	: 쿼리 문[In]
//	2st : 로우 셋[In]
//
// Return :
//	생성 성공 시 true, 실패시 false
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool OleDB::ExecuteQuery(LPCTSTR Query_In, Rowset Rowset_In)
{
	HRESULT hResult = NULL;

	if (m_pIDBCreateSession == NULL || Query_In == NULL)
	{
		HandleError(__LINE__, 0, "세션이 초기화 되지 않았거나 쿼리가 없습니다.");
		return false;
	}

	// 체크
	DB_PERFORMANCE_CHECK(
		DeclareBlockTimingCheck("Check Excute CreateSession", CreateSession);
		DeclareBlockTimingCheck("Check Excute CreateCommand", CreateCommand);
		DeclareBlockTimingCheck("Check Excute DataBind", DataBind);
		DeclareBlockTimingCheck("Check Excute Execute", Execute);
	)

	DB_PERFORMANCE_CHECK(BlockTimingCheckStart(CreateSession);)

	// DB 커맨드 생성
	if(NULL == m_pIDBCreateCommand)
		DBCreateCommand();

	// 체크
	DB_PERFORMANCE_CHECK(BlockTimingCheckStop(CreateSession);)	
	DB_PERFORMANCE_CHECK(BlockTimingCheckStart(CreateCommand);)

	// 쿼리 제작
	ICommandText* pICommandText = NULL;
	if (FAILED(hResult = m_pIDBCreateCommand->CreateCommand(NULL, IID_ICommandText, (IUnknown**)&pICommandText)))
	{
		SERLOG2(g_Log, "CreateCommand 을 %d 의 에러로 실패했습니다.", ::GetLastError(), hResult);
		HandleError(__LINE__, hResult, "IDBCreateCommand::CreateCommand");

		SAFE_RELEASE(pICommandText);
		return false;
	}

	if(NULL == pICommandText)
	{
		SAFE_RELEASE(pICommandText);
		return false;
	}

	MultiByteToWideChar(CP_ACP, 0, Query_In, -1, m_QueryText, MaxQueryTextLen/sizeof(WCHAR));
	if (FAILED(hResult = pICommandText->SetCommandText(DBGUID_DBSQL, m_QueryText)))
	{
		HandleError(__LINE__, hResult, "ICommandText::SetCommandText");

		SAFE_RELEASE(pICommandText);
		return false;
	}

	// 체크
	DB_PERFORMANCE_CHECK(BlockTimingCheckStop(CreateCommand);)
	DB_PERFORMANCE_CHECK(BlockTimingCheckStart(Execute);)

	SAFE_RELEASE(m_pIRowset);
	SAFE_RELEASE(m_pIRowsetChange);

	// 쿼리 실행
	if (OleDB::Rowset_Get == Rowset_In)
	{
		hResult = pICommandText->Execute(NULL, IID_IRowset, NULL, NULL, (IUnknown **)&m_pIRowset);
	}
	else if (OleDB::Rowset_Update == Rowset_In)
	{
		hResult = pICommandText->Execute(NULL, IID_IRowsetChange, NULL, NULL, (IUnknown **)&m_pIRowsetChange);
	}

	if (FAILED(hResult))
	{
		HandleError(__LINE__, hResult, "IRowset::Execute 또는 IRowsetChange::Execute");

		SAFE_RELEASE(pICommandText);
		return false;
	}
	
	SAFE_RELEASE(pICommandText);
    m_dwLastError = 0;
	m_dwQueryErrorCount = 0;
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
// ExecuteQueryWithParams [ public ]
// - 쿼리를 실행한다.
//
// Parameter :
//	1st	: 쿼리 문[In]
//	2st : 로우 셋[In]
//
// Return :
//	생성 성공 시 true, 실패시 false
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool OleDB::ExecuteQueryWithParams(LPCTSTR Query_In, char *Data_In, const PARAM_INFO& ParamInfo_In)
{
	HRESULT hResult = NULL;

	if (m_pIDBCreateSession == NULL || Query_In == NULL)
	{
		HandleError(__LINE__, 0, "세션이 초기화 되지 않았거나 쿼리가 없습니다.");
		return false;
	}

	// 체크
	DB_PERFORMANCE_CHECK(
		DeclareBlockTimingCheck("Check ExcuteParam CreateSession", CreateSession);
		DeclareBlockTimingCheck("Check ExcuteParam CreateCommand", CreateCommand);
		DeclareBlockTimingCheck("Check ExcuteParam DataBind", DataBind);
		DeclareBlockTimingCheck("Check ExcuteParam Execute", Execute);
	)

	DB_PERFORMANCE_CHECK(BlockTimingCheckStart(CreateSession);)

	// DB 커맨드 생성
	if(NULL == m_pIDBCreateCommand)
		DBCreateCommand();

	// 체크
	DB_PERFORMANCE_CHECK(BlockTimingCheckStop(CreateSession);)	
	DB_PERFORMANCE_CHECK(BlockTimingCheckStart(CreateCommand);)

	// 쿼리 제작
	ICommandText* pICommandText = NULL;
	if (FAILED(hResult = m_pIDBCreateCommand->CreateCommand(NULL, IID_ICommandText, (IUnknown**)&pICommandText)))
	{
		SERLOG2(g_Log, "CreateCommand 을 %d 의 에러로 실패했습니다.", ::GetLastError(), hResult);
		HandleError(__LINE__, hResult, "IDBCreateCommand::CreateCommand");

		SAFE_RELEASE(pICommandText);
		return false;
	}

	if(NULL == pICommandText)	
	{
		SAFE_RELEASE(pICommandText);
		return false;
	}

	MultiByteToWideChar(CP_ACP, 0, Query_In, -1, m_QueryText, MaxQueryTextLen/sizeof(WCHAR));
	if (FAILED(hResult = pICommandText->SetCommandText(DBGUID_DBSQL, m_QueryText)))
	{
		HandleError(__LINE__, hResult, "ICommandText::SetCommandText");

		SAFE_RELEASE(pICommandText);
		return false;
	}

	// 체크
	DB_PERFORMANCE_CHECK(BlockTimingCheckStop(CreateCommand);)
	DB_PERFORMANCE_CHECK(BlockTimingCheckStart(DataBind);)

	// 접근 인터페이스 바인드
	IAccessor* pIAccessor = NULL;
	if (FAILED(hResult = pICommandText->QueryInterface(IID_IAccessor, (void **) &pIAccessor))) 
	{
		SERLOG1(g_Log, "QueryInterface 를 %d 의 에러로 실패했습니다.", ::GetLastError());
		HandleError(__LINE__, hResult, "ICommandText::QueryInterface");

		SAFE_RELEASE(pICommandText);
		SAFE_RELEASE(pIAccessor);
		return false;
	}

	if(NULL == pIAccessor)	
	{	
		SAFE_RELEASE(pICommandText);
		SAFE_RELEASE(pIAccessor);
		return false;	
	}

	// 바인드 정보 데이터 할당
	DBBINDING *pBindings = AllocBindParamInputData(ParamInfo_In);
	HACCESSOR hAccessor = NULL;
	if (FAILED(hResult = pIAccessor->CreateAccessor(DBACCESSOR_PARAMETERDATA, ParamInfo_In.ColNum, pBindings, NULL, &hAccessor, NULL)))
	{
		SERLOG2(g_Log, "CreateAccessor 를 %d 의 에러로 실패했습니다. %d", ::GetLastError(), hResult);
		HandleError(__LINE__, hResult, "IAccessor::CreateAccessor");

		SAFE_RELEASE(pICommandText);
		SAFE_RELEASE(pIAccessor);
		SAFE_FREE(pBindings);
		return false;
	}

	SAFE_FREE(pBindings);

	// 체크
	DB_PERFORMANCE_CHECK(BlockTimingCheckStop(DataBind);)
	DB_PERFORMANCE_CHECK(BlockTimingCheckStart(Execute);)

	// 파라미터 데이터 설정
	DBPARAMS DBParams = {0,};
	DBParams.cParamSets	= 1;
	DBParams.hAccessor	= hAccessor;
	DBParams.pData		= Data_In;

	// 쿼리 실행
	SAFE_RELEASE(m_pIRowset);
	if (FAILED(hResult = pICommandText->Execute(NULL, IID_IRowset, &DBParams, NULL, (IUnknown **)&m_pIRowset)))
	{
		HandleError(__LINE__, hResult, "IRowset::Execute 또는 IRowsetChange::Execute");

		pIAccessor->ReleaseAccessor(hAccessor, NULL);
		SAFE_RELEASE(pIAccessor);
		SAFE_RELEASE(pICommandText);
		return false;
	}

	pIAccessor->ReleaseAccessor(hAccessor, NULL);
	SAFE_RELEASE(pIAccessor);

	SAFE_RELEASE(pICommandText);
    m_dwLastError = 0;
	m_dwQueryErrorCount = 0;
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
// ExecuteQueryGetData [ public ]
// - 쿼리를 실행 후 데이터를 얻는다
//
// Parameter :
//	1st	: 쿼리 문[In]
//	2st : 로우 셋[In]
//
// Return :
//	생성 성공 시 true, 실패시 false
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool OleDB::ExecuteQueryGetData(LPCTSTR Query_In, void *Buffer_Out)
{
	HRESULT hResult = NULL;

	if (m_pIDBCreateSession == NULL || Query_In == NULL)
	{
		HandleError(__LINE__, 0, "세션이 초기화 되지 않았거나 쿼리가 없습니다.");
		return false;
	}

	// 체크
	DB_PERFORMANCE_CHECK(
		DeclareBlockTimingCheck("Check ExcuteGetData CreateSession", CreateSession);
		DeclareBlockTimingCheck("Check ExcuteGetData CreateCommand", CreateCommand);
		DeclareBlockTimingCheck("Check ExcuteGetData Execute", Execute);
		DeclareBlockTimingCheck("Check ExcuteGetData DataBind", DataBind);
		DeclareBlockTimingCheck("Check ExcuteGetData GetData", GetData);
	)

	DB_PERFORMANCE_CHECK(BlockTimingCheckStart(CreateSession);)

	// DB 커맨드 생성
	if(NULL == m_pIDBCreateCommand)
		DBCreateCommand();

	// 체크
	DB_PERFORMANCE_CHECK(BlockTimingCheckStop(CreateSession);)	
	DB_PERFORMANCE_CHECK(BlockTimingCheckStart(CreateCommand);)

	// 쿼리 제작
	ICommandText* pICommandText = NULL;
	if (FAILED(hResult = m_pIDBCreateCommand->CreateCommand(NULL, IID_ICommandText, (IUnknown**)&pICommandText)))
	{
		HandleError(__LINE__, hResult, "IDBCreateCommand::CreateCommand");

		SAFE_RELEASE(pICommandText);
		return false;
	}

	MultiByteToWideChar(CP_ACP, 0, Query_In, -1, m_QueryText, MaxQueryTextLen/sizeof(WCHAR));
	if (FAILED(hResult = pICommandText->SetCommandText(DBGUID_DBSQL, m_QueryText)))
	{
		HandleError(__LINE__, hResult, "ICommandText::SetCommandText");

		SAFE_RELEASE(pICommandText);
		return false;
	}

	// 체크
	DB_PERFORMANCE_CHECK(BlockTimingCheckStop(CreateCommand);)
	DB_PERFORMANCE_CHECK(BlockTimingCheckStart(Execute);)

	IRowset *pIRowset = NULL;
	hResult = pICommandText->Execute(NULL, IID_IRowset, NULL, NULL, (IUnknown **)&pIRowset);
	if(FAILED(hResult))
	{
		HandleError(__LINE__, hResult, "ICommandText::Execute");

		SAFE_RELEASE(pICommandText);
		return false;
	}
	
	SAFE_RELEASE(pICommandText);

	if(pIRowset == NULL)
	{
		HandleError(__LINE__, hResult, "로우셋 얻기 실패");
		return false;
	}

	DB_PERFORMANCE_CHECK(BlockTimingCheckStop(Execute);)
	DB_PERFORMANCE_CHECK(BlockTimingCheckStart(DataBind);)

	// 컬럼 정보 얻음
	RESULT_COLS ResultCols = {0,};
	if(!AllocResultCols(pIRowset, ResultCols)) 
	{
		HandleError(__LINE__, 0, " 컬럼수를 구할수 없습니다.");

		SAFE_RELEASE(pIRowset);
		return false;
	}

	// 접근 인터페이스 바인드
	IAccessor* pIAccessor = NULL;
	if (FAILED(hResult = pIRowset->QueryInterface(IID_IAccessor, (void **) &pIAccessor))) 
	{
		HandleError(__LINE__, hResult, "IRowset::QueryInterface");

		SAFE_RELEASE(pIAccessor);

		SAFE_RELEASE(pIRowset);
		return false;
	}

	// 바인드 정보 데이터 할당
	DBBINDING *pBindings = AllocBindGetData(ResultCols.ColNum, ResultCols.lpDBColumnInfo);
	HACCESSOR hAccessor = NULL;

	if (FAILED(hResult = pIAccessor->CreateAccessor(DBACCESSOR_ROWDATA, ResultCols.ColNum, pBindings, 0, &hAccessor, NULL)))
	{
		HandleError(__LINE__, hResult, "IAccessor::CreateAccessor");

		SAFE_FREE(pBindings);
		SAFE_RELEASE(pIAccessor);

		SAFE_RELEASE(pIRowset);
		return false;
	}

	SAFE_FREE(pBindings);

	// 체크
	DB_PERFORMANCE_CHECK(BlockTimingCheckStop(DataBind);)
	DB_PERFORMANCE_CHECK(BlockTimingCheckStart(GetData);)

	// 결과 로우 얻기
	HROW *pRows = new HROW;
	unsigned long RowNums = 0;
	if (FAILED(hResult = pIRowset->GetNextRows(NULL, 0, 1, &RowNums, &pRows)))
	{
		HandleError(__LINE__, hResult, "IRowset::GetNextRows");

		pIAccessor->ReleaseAccessor(hAccessor, NULL);
		SAFE_RELEASE(pIAccessor);

		pIRowset->ReleaseRows(RowNums, pRows, NULL, NULL, NULL);
		SAFE_DELETE(pRows);
		
		ReleaseResultCols(pIRowset, ResultCols);
		SAFE_RELEASE(pIRowset);
		return false;
	}

	if (RowNums <= 0)
	{
		HandleError(__LINE__, 0, "결과 로우셋이 없습니다.");

		pIAccessor->ReleaseAccessor(hAccessor, NULL);
		SAFE_RELEASE(pIAccessor);

		pIRowset->ReleaseRows(RowNums, pRows, NULL, NULL, NULL);
		SAFE_DELETE(pRows);

		ReleaseResultCols(pIRowset, ResultCols);
		SAFE_RELEASE(pIRowset);
		return false;
	}

	// 데이터 얻기
	if (FAILED(hResult = pIRowset->GetData(*pRows, hAccessor, Buffer_Out)))
	{
		HandleError(__LINE__, hResult, "IRowset::GetData");

		pIAccessor->ReleaseAccessor(hAccessor, NULL);
		SAFE_RELEASE(pIAccessor);

		pIRowset->ReleaseRows(RowNums, pRows, NULL, NULL, NULL);
		SAFE_DELETE(pRows);
		
		ReleaseResultCols(pIRowset, ResultCols);
		SAFE_RELEASE(pIRowset);
		return false;
	}

	pIAccessor->ReleaseAccessor(hAccessor, NULL);
	SAFE_RELEASE(pIAccessor);

	pIRowset->ReleaseRows(RowNums, pRows, NULL, NULL, NULL);
	SAFE_DELETE(pRows);
	
	ReleaseResultCols(pIRowset, ResultCols);
	SAFE_RELEASE(pIRowset);

    m_dwLastError = 0;
	m_dwQueryErrorCount = 0;
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
// GetData [ public ]
// - 쿼리 결과 데이터 얻기
//
// Parameter :
//	1st	: 버퍼 포인터[Out]
//
// Return :
//	생성 성공 시 true, 실패시 false
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool OleDB::GetData(void *Buffer_Out)
{
	HRESULT hResult;

	if (m_pIRowset == NULL || Buffer_Out == NULL)
	{
		HandleError(__LINE__, 0, "로우셋이 없거나 버퍼가 없습니다");
		return false;
	}

	// 체크
	DB_PERFORMANCE_CHECK(
		DeclareBlockTimingCheck("Check GetData DataBind", DataBind);
		DeclareBlockTimingCheck("Check GetData GetData", GetData);
	)

	DB_PERFORMANCE_CHECK(BlockTimingCheckStart(DataBind);)

	// 컬럼 정보 얻음
	RESULT_COLS ResultCols = {0,};
	if(!AllocResultCols(m_pIRowset, ResultCols)) 
	{
		HandleError(__LINE__, 0, " 컬럼수를 구할수 없습니다.");
		return false;
	}

	// 접근 인터페이스 바인드
	IAccessor* pIAccessor = NULL;
	if (FAILED(hResult = m_pIRowset->QueryInterface(IID_IAccessor, (void **) &pIAccessor))) 
	{
		SERLOG1(g_Log, "QueryInterface 를 %d 의 에러로 실패했습니다.", ::GetLastError());
		HandleError(__LINE__, hResult, "IRowset::QueryInterface");

		SAFE_RELEASE(pIAccessor);

		ReleaseResultCols(m_pIRowset, ResultCols);
		return false;
	}

	// 바인드 정보 데이터 할당
	DBBINDING *pBindings = AllocBindGetData(ResultCols.ColNum, ResultCols.lpDBColumnInfo);
	HACCESSOR hAccessor = NULL;
	if (FAILED(hResult = pIAccessor->CreateAccessor(DBACCESSOR_ROWDATA, ResultCols.ColNum, pBindings, 0, &hAccessor, NULL)))
	{
		SERLOG1(g_Log, "CreateAccessor 를 %d 의 에러로 실패했습니다.", ::GetLastError());
		HandleError(__LINE__, hResult, "IAccessor::CreateAccessor");

		SAFE_FREE(pBindings);
		SAFE_RELEASE(pIAccessor);

		ReleaseResultCols(m_pIRowset, ResultCols);
		return false;
	}

	SAFE_FREE(pBindings);

	// 체크
	DB_PERFORMANCE_CHECK(BlockTimingCheckStop(DataBind);)
	DB_PERFORMANCE_CHECK(BlockTimingCheckStart(GetData);)

	// 결과 로우 얻기
	HROW *pRows = new HROW;
	unsigned long RowNums = 0;
	if (FAILED(hResult = m_pIRowset->GetNextRows(NULL, 0, 1, &RowNums, &pRows)))
	{
		HandleError(__LINE__, hResult, "IRowset::GetNextRows");

		pIAccessor->ReleaseAccessor(hAccessor, NULL);
		SAFE_RELEASE(pIAccessor);

		m_pIRowset->ReleaseRows(RowNums, pRows, NULL, NULL, NULL);
		SAFE_DELETE(pRows);
		
		ReleaseResultCols(m_pIRowset, ResultCols);
		return false;
	}

	if (RowNums <= 0)
	{
		HandleError(__LINE__, 0, "결과 로우셋이 없습니다.");

		pIAccessor->ReleaseAccessor(hAccessor, NULL);
		SAFE_RELEASE(pIAccessor);

		m_pIRowset->ReleaseRows(RowNums, pRows, NULL, NULL, NULL);
		SAFE_DELETE(pRows);

		ReleaseResultCols(m_pIRowset, ResultCols);
		return false;
	}

	// 데이터 얻기
	if (FAILED(hResult = m_pIRowset->GetData(*pRows, hAccessor, Buffer_Out)))
	{
		HandleError(__LINE__, hResult, "IRowset::GetData");

		pIAccessor->ReleaseAccessor(hAccessor, NULL);
		SAFE_RELEASE(pIAccessor);

		m_pIRowset->ReleaseRows(RowNums, pRows, NULL, NULL, NULL);
		SAFE_DELETE(pRows);
		
		ReleaseResultCols(m_pIRowset, ResultCols);
		return false;
	}

	pIAccessor->ReleaseAccessor(hAccessor, NULL);
	SAFE_RELEASE(pIAccessor);

	m_pIRowset->ReleaseRows(RowNums, pRows, NULL, NULL, NULL);
	SAFE_DELETE(pRows);
	
	ReleaseResultCols(m_pIRowset, ResultCols);
    m_dwLastError = 0;
	m_dwQueryErrorCount = 0;
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
// GetData [ public ]
// - 쿼리 결과 데이터 열 얻기
//
// Parameter :
//	1st	: 버퍼 포인터 배열[Out]
//	2st : 열 사이즈{In]
//	3st : 얻을 열 수[In]
//	4st : 성공한 열 계수[Out]
//
// Return :
//	생성 성공 시 true, 실패시 false
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool OleDB::GetData(void **Buffer_Out, int RowSize_In, int Row_In, int *pGetRow_Out)
{
	HRESULT hResult = NULL;

	if (m_pIRowset == NULL || Buffer_Out == NULL || RowSize_In == 0 || Row_In == 0 || pGetRow_Out == NULL)
	{
		HandleError(__LINE__, 0, "로우셋이 없거나 버퍼가 없습니다");

		return false;
	}

	// 컬럼 정보 얻음
	RESULT_COLS ResultCols = {0,};
	if(!AllocResultCols(m_pIRowset, ResultCols)) 
	{
		HandleError(__LINE__, 0, " 컬럼수를 구할수 없습니다.");
		return false;
	}

	// 접근 인터페이스 바인드
	IAccessor* pIAccessor = NULL;
	if (FAILED(hResult = m_pIRowset->QueryInterface(IID_IAccessor, (void **) &pIAccessor))) 
	{
		SERLOG1(g_Log, "QueryInterface 를 %d 의 에러로 실패했습니다.", ::GetLastError());
		HandleError(__LINE__, hResult, "IRowset::QueryInterface");

		SAFE_RELEASE(pIAccessor);

		ReleaseResultCols(m_pIRowset, ResultCols);
		return false;
	}

	// 바인드 정보 데이터 할당
	DBBINDING *pBindings = AllocBindGetData(ResultCols.ColNum, ResultCols.lpDBColumnInfo);
	HACCESSOR hAccessor = NULL;
	if (FAILED(hResult = pIAccessor->CreateAccessor(DBACCESSOR_ROWDATA, ResultCols.ColNum, pBindings, 0, &hAccessor, NULL)))
	{
		SERLOG1(g_Log, "CreateAccessor 를 %d 의 에러로 실패했습니다.", ::GetLastError());
		HandleError(__LINE__, hResult, "IAccessor::CreateAccessor");

		SAFE_FREE(pBindings);
		SAFE_RELEASE(pIAccessor);

		ReleaseResultCols(m_pIRowset, ResultCols);
		return false;
	}

	SAFE_FREE(pBindings);

	// 결과 로우 얻기
	HROW* pRow = new HROW[Row_In];
	unsigned long RowNums = 0;
	if (FAILED(hResult = m_pIRowset->GetNextRows(NULL, 0, Row_In, &RowNums, (HROW **)&pRow)))
	{
		HandleError(__LINE__, hResult, "IRowset::GetNextRows");

		m_pIRowset->ReleaseRows(RowNums, pRow, NULL, NULL, NULL);
		*pGetRow_Out = RowNums;
        delete [] pRow;
		
		pIAccessor->ReleaseAccessor(hAccessor, NULL);
		SAFE_RELEASE(pIAccessor);

		ReleaseResultCols(m_pIRowset, ResultCols);
		return false;
	}

	for (unsigned long Counter = 0; Counter < RowNums; Counter++)
	{
		if (FAILED(hResult = m_pIRowset->GetData(pRow[Counter], hAccessor, (char *)Buffer_Out + RowSize_In * Counter)))
		{
			HandleError(__LINE__, hResult, "IRowset::GetData");

			m_pIRowset->ReleaseRows(RowNums, pRow, NULL, NULL, NULL);
			*pGetRow_Out = RowNums;
            delete [] pRow;
			
			pIAccessor->ReleaseAccessor(hAccessor, NULL);
			SAFE_RELEASE(pIAccessor);

			ReleaseResultCols(m_pIRowset, ResultCols);
			return false;
		}
	}

	m_pIRowset->ReleaseRows(RowNums, pRow, NULL, NULL, NULL);
	*pGetRow_Out = RowNums;
    delete [] pRow;
	
	pIAccessor->ReleaseAccessor(hAccessor, NULL);
	SAFE_RELEASE(pIAccessor);

	ReleaseResultCols(m_pIRowset, ResultCols);
    m_dwLastError = 0;
	m_dwQueryErrorCount = 0;
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
// SetData [ public ]
// - 바이너리 데이터 업데이트
//
// Parameter :
//	버퍼 포인터 배열[Out]
//
// Return :
//	생성 성공 시 true, 실패시 false
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool OleDB::SetBinaryData(int ColNum_In, LPSET_BINARY lpSetBinary)
{
	HRESULT hResult = NULL;

	if(m_pIRowsetChange == NULL || lpSetBinary == NULL)
	{
		HandleError(__LINE__, 0, "로우셋이 없거나 데이터가 없습니다.");
		return false;
	}

	// 접근 인터페이스 바인드
	IAccessor* pIAccessor = NULL;
	if(FAILED(hResult = m_pIRowsetChange->QueryInterface(IID_IAccessor, (void **) &pIAccessor))) 
	{
		HandleError(__LINE__, hResult, "IRowsetChange::QueryInterface");

		SAFE_RELEASE(pIAccessor);
		return false;
	}

	// DB 바인드 배열 생성
	DBBINDING *pBindings = NULL;
	pBindings = (DBBINDING*)malloc(1 * sizeof(DBBINDING));

	pBindings[0].iOrdinal	= ColNum_In;
	pBindings[0].pTypeInfo	= NULL;
	pBindings[0].pObject	= NULL;
	pBindings[0].pBindExt	= NULL;

	pBindings[0].dwPart		= DBPART_VALUE | DBPART_LENGTH;			
	pBindings[0].obLength	= 0;
//	pBindings[0].obValue	= 0 + sizeof(unsigned long);
	pBindings[0].obValue	= 0;

	pBindings[0].dwMemOwner = DBMEMOWNER_CLIENTOWNED;
	pBindings[0].eParamIO	= DBPARAMIO_NOTPARAM;
	pBindings[0].cbMaxLen	= lpSetBinary->Size;

	pBindings[0].dwFlags	= 0;
	pBindings[0].wType		= DBTYPE_BYTES;

	HACCESSOR hAccessor = NULL;
	if(FAILED(hResult = pIAccessor->CreateAccessor(DBACCESSOR_ROWDATA, 1, pBindings, 0, &hAccessor, NULL)))
	{
		HandleError(__LINE__, hResult, "IAccessor::CreateAccessor");

		SAFE_FREE(pBindings);
		SAFE_RELEASE(pIAccessor);
		return false;
	}

	SAFE_FREE(pBindings);

	IRowset* pIRowset = NULL;
	if(FAILED(hResult = m_pIRowsetChange->QueryInterface(IID_IRowset, (void **) &pIRowset))) 
	{
		HandleError(__LINE__, hResult, "IRowsetChange::QueryInterface");

		pIAccessor->ReleaseAccessor(hAccessor, NULL);
		SAFE_RELEASE(pIAccessor);

		SAFE_RELEASE(pIRowset);
		return false;
	}

	HROW *pRows = new HROW;
	unsigned long RowNums = 0;
	if(FAILED(hResult = pIRowset->GetNextRows(NULL, 0, 1, &RowNums, &pRows)))
	{
		HandleError(__LINE__, hResult, "IRowset::GetNextRows");

		SAFE_DELETE(pRows);

		pIAccessor->ReleaseAccessor(hAccessor, NULL);
		SAFE_RELEASE(pIAccessor);

		SAFE_RELEASE(pIRowset);
		return false;
	}

	if(RowNums <= 0)
	{
		HandleError(__LINE__, hResult, "결과 로우셋이 없습니다.");

		pIAccessor->ReleaseAccessor(hAccessor, NULL);
		SAFE_RELEASE(pIAccessor);

		pIRowset->ReleaseRows(RowNums, pRows, NULL, NULL, NULL);
		SAFE_DELETE(pRows);

		SAFE_RELEASE(pIRowset);
		return false;
	}

	if(FAILED(hResult = m_pIRowsetChange->SetData(*pRows, hAccessor, lpSetBinary)))
	{
		HandleError(__LINE__, hResult, "IRowsetChange::SetData");

		pIAccessor->ReleaseAccessor(hAccessor, NULL);
		SAFE_RELEASE(pIAccessor);

		pIRowset->ReleaseRows(RowNums, pRows, NULL, NULL, NULL);
		SAFE_DELETE(pRows);

		SAFE_RELEASE(pIRowset);
		return false;
	}

	pIAccessor->ReleaseAccessor(hAccessor, NULL);
	SAFE_RELEASE(pIAccessor);

	pIRowset->ReleaseRows(RowNums, pRows, NULL, NULL, NULL);
	SAFE_DELETE(pRows);

	SAFE_RELEASE(pIRowset);
    m_dwLastError = 0;
	m_dwQueryErrorCount = 0;
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
// HandleError [ private ]
// - 에러 처리
//
// Parameter :
//
// Return :
//	생성 성공 시 true, 실패시 false
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool OleDB::HandleError(int ErrorLine_In, HRESULT hResult_In, char *Buffer_In)
{
	IErrorInfo* pIErrorInfo = NULL;
	BSTR ErrorString = NULL;
	BOOL Bool;

    m_dwLastError = hResult_In;
	m_dwQueryErrorCount++;

	HRESULT hResult;

	if(FAILED(hResult = GetErrorInfo(0, &pIErrorInfo)))
	{
		SAFE_RELEASE(pIErrorInfo);
		_snprintf(m_ErrorString, MaxErrorLen, "[%3d:0x%5x] [M:%s]", ErrorLine_In, hResult_In, (NULL != Buffer_In) ? Buffer_In : "");
		return false;
	}

	if(pIErrorInfo == NULL)
	{
		SAFE_RELEASE(pIErrorInfo);
		_snprintf(m_ErrorString, MaxErrorLen, "[%3d:0x%5x] [M:%s]", ErrorLine_In, hResult_In, (NULL != Buffer_In) ? Buffer_In : "");
		return false;
	}
	
	if(FAILED(hResult = pIErrorInfo->GetDescription(&ErrorString)))
	{
		SAFE_RELEASE(pIErrorInfo);
		_snprintf(m_ErrorString, MaxErrorLen, "[%3d:0x%5x] [M:%s]", ErrorLine_In, hResult_In, (NULL != Buffer_In) ? Buffer_In : "");
		return false;
	}
	
	char Buffer[MaxErrorLen];
	WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)ErrorString, -1, (char *)Buffer, MaxErrorLen - 50, NULL, &Bool);
	_snprintf(m_ErrorString, MaxErrorLen, "[%3d:0x%5x] %s  [M:%s]", ErrorLine_In, hResult_In, Buffer, (NULL != Buffer_In) ? Buffer_In : "");

	SAFE_RELEASE(pIErrorInfo);    
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
// CreateSession [ private ]
// - 세션 열기
//
// Parameter :
//
// Return :
//	생성 성공 시 컬럼 수, 실패시 -1
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool OleDB::CreateSession(void)
{
	HRESULT hResult;

	if (m_pIDBInit == NULL)
	{
		SERLOG0(g_Log, "IDBInitialize 오브젝트가 초기화되지 않았습니다.");
		return false;
	}

	SAFE_RELEASE(m_pIDBCreateCommand);
	SAFE_RELEASE(m_pIDBCreateSession);

	// 세션 열기
    if (FAILED(hResult = m_pIDBInit->QueryInterface(IID_IDBCreateSession, (void**)&m_pIDBCreateSession)))
	{
		SERLOG0(g_Log, "IDBInitialize 오브젝트의 QueryInterface를 실행하는데 실패했습니다..");
		HandleError(__LINE__, hResult, "IDBInitialize::QueryInterface");
		return false;
	}

    m_dwLastError = 0;
	m_dwQueryErrorCount = 0;
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
// DBCreateCommand [ private ]
// - 세션 열기
//
// Parameter :
//
// Return :
//	생성 성공 시 컬럼 수, 실패시 -1
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool OleDB::DBCreateCommand(void)
{
	HRESULT hResult;

	if(NULL == m_pIDBCreateSession)
	{
		if(!CreateSession())
			return false;
	}

	SAFE_RELEASE(m_pIDBCreateCommand);

	if(FAILED(hResult = m_pIDBCreateSession->CreateSession(NULL, IID_IDBCreateCommand, (IUnknown**)&m_pIDBCreateCommand)))
	{
		HandleError(__LINE__, hResult, "IDBCreateCommand::CreateSession");
		if(!CreateSession()) 
			return false;

		if(FAILED(hResult = m_pIDBCreateSession->CreateSession(NULL, IID_IDBCreateCommand, (IUnknown**)&m_pIDBCreateCommand)))
		{
			HandleError(__LINE__, hResult, "IDBCreateCommand::CreateSession");
			SERLOG2(g_Log, "CreateSession 을 %d 의 에러로 실패했습니다. %d", ::GetLastError(), hResult);

			return false;
		}
	}

	if(NULL == m_pIDBCreateSession)
		return false;

    m_dwLastError = 0;
	m_dwQueryErrorCount = 0;
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
// AllocResultCols [ private ]
// - 컬럼 정보 할당
//
// Parameter :
//	1st	: 컬럼 정보 포인터[Out]
//
// Return :
//	생성 성공 시 컬럼 수, 실패시 -1
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool OleDB::AllocResultCols(IUnknown* lpIUnknown_In, RESULT_COLS &Rsult_Cols)
{
	IColumnsInfo* pIColumnsInfo = NULL;
	if(FAILED(lpIUnknown_In->QueryInterface(IID_IColumnsInfo, (void **)&pIColumnsInfo)))
	{
		SAFE_RELEASE(pIColumnsInfo);
		return false;
	}

	// 컬럼 정보 인터페이스 얻기
	if(FAILED(pIColumnsInfo->GetColumnInfo(&Rsult_Cols.ColNum, &(Rsult_Cols.lpDBColumnInfo), &(Rsult_Cols.lpStringsBuffer))))
	{
		SAFE_RELEASE(pIColumnsInfo);
		return false;
    }

	SAFE_RELEASE(pIColumnsInfo);
    m_dwLastError = 0;
	m_dwQueryErrorCount = 0;
	return true;
}

bool OleDB::ReleaseResultCols(IUnknown* lpIUnknown_In, RESULT_COLS &Rsult_Cols)
{
	IMalloc* pMalloc = NULL;
    HRESULT hr;
	
	if(0 == Rsult_Cols.ColNum)
		return false;

	if( (hr = CoGetMalloc( MEMCTX_TASK, &pMalloc )) != S_OK ) {
		// ERROR 처리
		return false;
	}

	if(Rsult_Cols.lpDBColumnInfo)
	{
		pMalloc->Free(Rsult_Cols.lpDBColumnInfo);
		Rsult_Cols.lpDBColumnInfo = NULL;
	}

	if(Rsult_Cols.lpStringsBuffer)
	{
		pMalloc->Free(Rsult_Cols.lpStringsBuffer);
		Rsult_Cols.lpStringsBuffer = NULL;
	}

	Rsult_Cols.ColNum = 0;

	SAFE_RELEASE(pMalloc);
    m_dwLastError = 0;
	m_dwQueryErrorCount = 0;
	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////
//
// SetConnectionProperties [ private ]
// - 연결 특성 설정
//
// Parameter :
//	1st	: 서버 이름[In]				# (or 데이터 소스 이름)
//	2st : 데이터 베이스 이름[In]	# 데이터 소스일 경우 NULL이어도 상관 없음.
//	3st : 계정 이름[In]				# (or 데이터 소스 인증)
//	4st : 계정 패스[In]				# (or 데이터 소스 인증)
//
// Return :
//	생성 성공 시 컬럼 수, 실패시 -1
//
///////////////////////////////////////////////////////////////////////////////////////////////
bool OleDB::SetConnectionProperties(LPCTSTR ServerName_In, LPCTSTR DataBaseName_In, LPCTSTR UserID_In, LPCTSTR UserPass_In)
{
	WCHAR ServerName[100] = L"", DataBaseName[100] = L"";
	WCHAR UserID[100] = L"", UserPass[100] = L"";

	IDBProperties* pIDBProperties;	

	DBPROPSET dbPropSet[2];		
	DBPROP DataSourceProp[4];		
	DBPROP RowsetProp[1];

	HRESULT hResult;
	int Counter;

	if(m_pIDBInit == NULL)
		return false;

	// 유니 코드 문자로 메핑
	MultiByteToWideChar(CP_ACP, 0, ServerName_In, -1, ServerName, 100);
	MultiByteToWideChar(CP_ACP, 0, DataBaseName_In, -1, DataBaseName, 100);
	MultiByteToWideChar(CP_ACP, 0, UserID_In, -1, UserID, 100);
	MultiByteToWideChar(CP_ACP, 0, UserPass_In, -1, UserPass, 100);

	// DB 초기화 특성 : 데이터 소스 이름, DB 이름, 계정, 패스워드 설정
	ZeroMemory(&dbPropSet[0], sizeof(DBPROPSET));
	for(Counter = 0; Counter < 4; Counter++)
	{
		ZeroMemory(&DataSourceProp[Counter], sizeof(DBPROP));
		DataSourceProp[Counter].vValue.vt = VT_BSTR;
		DataSourceProp[Counter].dwOptions = DBPROPOPTIONS_REQUIRED;
		DataSourceProp[Counter].colid = DB_NULLID;
	}

	DataSourceProp[0].dwPropertyID  = (wcslen(ServerName) != 0) ? DBPROP_INIT_DATASOURCE : 0;
    DataSourceProp[0].vValue.bstrVal= SysAllocString(ServerName);	
 
    DataSourceProp[1].dwPropertyID  = (wcslen(DataBaseName) != 0) ? DBPROP_INIT_CATALOG : 0;
    DataSourceProp[1].vValue.bstrVal= SysAllocString(DataBaseName);
	
    DataSourceProp[2].dwPropertyID  = (wcslen(UserID) != 0) ? DBPROP_AUTH_USERID : 0; 
    DataSourceProp[2].vValue.bstrVal= SysAllocString(UserID);
    
    DataSourceProp[3].dwPropertyID  = (wcslen(UserPass) != 0) ? DBPROP_AUTH_PASSWORD : 0;
    DataSourceProp[3].vValue.bstrVal= SysAllocString(UserPass);

	dbPropSet[0].guidPropertySet = DBPROPSET_DBINIT;
    dbPropSet[0].cProperties    = 4;
    dbPropSet[0].rgProperties   = DataSourceProp;

	// DB 로우셋 특성 : 업데이트 가능 설정
	RowsetProp[0].dwOptions = DBPROPOPTIONS_REQUIRED;
	RowsetProp[0].colid = DB_NULLID;

	RowsetProp[0].dwPropertyID  = DBPROP_UPDATABILITY;
	RowsetProp[0].vValue.vt		= VT_I4;
	RowsetProp[0].vValue.intVal	= VARIANT_TRUE;	

	dbPropSet[1].guidPropertySet = DBPROPSET_ROWSET;
	dbPropSet[1].cProperties    = 1;
	dbPropSet[1].rgProperties   = RowsetProp;

	if(FAILED(hResult = m_pIDBInit->QueryInterface(IID_IDBProperties, (void **)&pIDBProperties)))
	{
		HandleError(__LINE__, hResult, "IDBInitialize::QueryInterface");

		SysFreeString(DataSourceProp[0].vValue.bstrVal);
		SysFreeString(DataSourceProp[1].vValue.bstrVal);
		SysFreeString(DataSourceProp[2].vValue.bstrVal);
		SysFreeString(DataSourceProp[3].vValue.bstrVal);

		SAFE_RELEASE(pIDBProperties);
		return false;
	}

 	// 특성 설정
   if(FAILED(hResult = pIDBProperties->SetProperties(2, dbPropSet)))
   	{
		HandleError(__LINE__, hResult, "IDBProperties::SetProperties");

		SysFreeString(DataSourceProp[0].vValue.bstrVal);
		SysFreeString(DataSourceProp[1].vValue.bstrVal);
		SysFreeString(DataSourceProp[2].vValue.bstrVal);
		SysFreeString(DataSourceProp[3].vValue.bstrVal);

		SAFE_RELEASE(pIDBProperties);
		return false;
	}
	
	// 데이터 릴리즈
	SysFreeString(DataSourceProp[0].vValue.bstrVal);
	SysFreeString(DataSourceProp[1].vValue.bstrVal);
	SysFreeString(DataSourceProp[2].vValue.bstrVal);
	SysFreeString(DataSourceProp[3].vValue.bstrVal);

	SAFE_RELEASE(pIDBProperties);
    m_dwLastError = 0;
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
// AllocBindGetData [ private ]
// - 데이터 바인드
//
// Parameter :
//	1st	: 컬럼 수[In]				
//	2st : 컬럼 정보 구조체[In]		
//
// Return :
//	생성 성공 시 컬럼 수, 실패시 -1
//
///////////////////////////////////////////////////////////////////////////////////////////////
DBBINDING* OleDB::AllocBindGetData(int ColsNum_In, DBCOLUMNINFO* pDBColumnInfo_In)
{
	DBBINDING* pBindings = (DBBINDING*)malloc(ColsNum_In * sizeof(DBBINDING));

	int ColCounter;
	unsigned long ConsumerBufColOffset = 0;

	for(ColCounter = 0; ColCounter < ColsNum_In; ColCounter++)
	{
		pBindings[ColCounter].iOrdinal		= pDBColumnInfo_In[ColCounter].iOrdinal;

		pBindings[ColCounter].dwPart		= DBPART_VALUE;
		pBindings[ColCounter].obValue		= ConsumerBufColOffset;

		pBindings[ColCounter].pTypeInfo		= NULL;
		pBindings[ColCounter].pObject		= NULL;
		pBindings[ColCounter].pBindExt		= NULL;

		pBindings[ColCounter].dwMemOwner	= DBMEMOWNER_CLIENTOWNED;
		pBindings[ColCounter].eParamIO		= DBPARAMIO_NOTPARAM;
		pBindings[ColCounter].cbMaxLen		= pDBColumnInfo_In[ColCounter].ulColumnSize;

		pBindings[ColCounter].dwFlags		= 0;
		pBindings[ColCounter].wType			= pDBColumnInfo_In[ColCounter].wType;

		// 다음 버퍼셋 위치 계산
		ConsumerBufColOffset = ConsumerBufColOffset + pDBColumnInfo_In[ColCounter].ulColumnSize;
	}

	return pBindings;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
// AllocBindParamData [ private ]
// - 데이터 바인드
//
// Parameter :
//	1st	: 컬럼 정보[In]				
//
// Return :
//	생성 성공 시 컬럼 수, 실패시 -1
//
///////////////////////////////////////////////////////////////////////////////////////////////
DBBINDING* OleDB::AllocBindParamInputData(const PARAM_INFO &ParamInfo_In)
{
	DBBINDING* pBindings = (DBBINDING*)malloc(ParamInfo_In.ColNum * sizeof(DBBINDING));

	unsigned long ColCounter;
	unsigned long ConsumerBufColOffset = 0;

	for(ColCounter = 0; ColCounter < ParamInfo_In.ColNum; ColCounter++)
	{
		pBindings[ColCounter].iOrdinal		= ColCounter + 1;

		pBindings[ColCounter].dwPart		= DBPART_VALUE;
		pBindings[ColCounter].obValue		= ConsumerBufColOffset;

		pBindings[ColCounter].pTypeInfo		= NULL;
		pBindings[ColCounter].pObject		= NULL;
		pBindings[ColCounter].pBindExt		= NULL;

		pBindings[ColCounter].dwMemOwner	= DBMEMOWNER_CLIENTOWNED;
		pBindings[ColCounter].eParamIO		= ParamInfo_In.eParamIO[ColCounter];
		pBindings[ColCounter].cbMaxLen		= ParamInfo_In.ColSize[ColCounter];

		pBindings[ColCounter].dwFlags		= 0;
		pBindings[ColCounter].wType			= ParamInfo_In.ColType[ColCounter];

		// 다음 버퍼셋 위치 계산
		ConsumerBufColOffset = ConsumerBufColOffset + ParamInfo_In.ColSize[ColCounter];
	}

	return pBindings;
}

/*bool OleDB::GetUserStoredProcedureInfo(DB_SP_INFO* pSpInfo, DWORD dwMaxSPCnt)
{
	HROW* hRows = NULL;

	hRows = new HROW[5000];
	memset(hRows, 0, sizeof(HROW)*5000);

	HRESULT						hr					= NULL;	
	HROW*				        pRows				= &hRows[0];

	IDBSchemaRowset				*pIDBSchemaRowset	= NULL;
	IRowset*					pIRowset			= NULL;
	IAccessor*					pIAccessor			= NULL;
	HACCESSOR					hAccessor			= 0;	

	WCHAR						wBuffer[ 0xff ]		= {0, };
	char						szBuffer[ 1024 ]	= {0, };	

	const static unsigned char	cBindingCnt			= 1;	

	DBBINDING rgBindings[cBindingCnt] = 
	{
		1, 0, 0, 0,	NULL, NULL, NULL, DBPART_VALUE, DBMEMOWNER_CLIENTOWNED, DBPARAMIO_NOTPARAM, 0xff, 0, DBTYPE_WSTR, 0, 0,						
	};

	if(!m_pIDBCreateCommand)		
	{
		if(!DBCreateCommand())
			return false;
	}

	hr = m_pIDBCreateCommand->QueryInterface(IID_IDBSchemaRowset, (void**)&pIDBSchemaRowset);

	if(FAILED(hr))
		return false;

	VARIANT	rgRestrictions[3];
	ULONG	cRestrictions = 3;

	for(ULONG j=0; j < 3; j++)
	VariantInit( &rgRestrictions[j]);

	for(unsigned char cCnt = 0; cCnt < 3; cCnt++)
	{
		unsigned long	dwRet		= 0;		
		unsigned short	wIndex		= 0;

		hr = pIDBSchemaRowset->GetRowset(NULL, DBSCHEMA_PROCEDURES, 1, rgRestrictions, IID_IRowset, 0, NULL, (IUnknown**)&pIRowset);	

		if(FAILED(hr))
		return false;

		hr = pIRowset->QueryInterface(IID_IAccessor, (void **) &pIAccessor);

		if(FAILED(hr))
		return false;

		switch(cCnt)
		{
		case 1:	rgBindings[0].iOrdinal	= 2;	break;
		case 2:	rgBindings[0].iOrdinal	= 3;	break;		
		}

		hr = pIAccessor->CreateAccessor(DBACCESSOR_ROWDATA, 1, rgBindings, 0, &hAccessor, NULL);

		if(FAILED(hr))
			return false;

		hr = pIRowset->GetNextRows(DB_NULL_HCHAPTER, 0, dwMaxSPCnt, &dwRet, &pRows);

		if(FAILED(hr))
		return false;

		while(dwRet > 0) 
		{		
			if(wIndex >= dwMaxSPCnt)
			{
				pIRowset->ReleaseRows(dwRet, hRows, NULL, NULL, NULL);					
				break;
			}

			for(int nIndex = 0; nIndex < dwRet; nIndex++)
			{
				wBuffer[0] = L'\0';
				pIRowset->GetData(hRows[nIndex], hAccessor, wBuffer);

				wsprintf((LPTSTR)szBuffer, (LPCTSTR)"%S", wBuffer);

				if(lstrlen(szBuffer))
				{
					switch(cCnt)
					{
					case 0:
						memcpy(pSpInfo[wIndex].szCatalog, szBuffer, lstrlen(szBuffer));
						break;
					case 1:
						memcpy(pSpInfo[wIndex].szType, szBuffer, lstrlen(szBuffer));
						break;
					case 2:
						{
							char	szSeps[]	= ";";
							char*	szToken		= NULL;

							szToken = strtok(szBuffer, szSeps) ;

							memcpy(pSpInfo[wIndex].szSPName, szBuffer, lstrlen(szBuffer));
						}						
						break;
					}					
				}
			
				wIndex++;				
			}		
			pIRowset->ReleaseRows(dwRet, hRows, NULL, NULL, NULL);
			pIRowset->GetNextRows(NULL, 0, dwMaxSPCnt, &dwRet, &pRows);			
		}		
	}

	pIDBSchemaRowset->Release();
	pIAccessor->ReleaseAccessor(hAccessor, NULL);
	pIAccessor->Release();
	pIRowset->Release();

	if(hRows)
	{
		delete [] hRows;
		hRows = NULL;
	}

	return true;	
}*/