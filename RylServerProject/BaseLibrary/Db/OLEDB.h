///////////////////////////////////////////////////////////////////////////////////////////////
//
// OleDB
//
//////////////////////////////////////////////////////////////////////////////////////////////
#ifndef _RYL_BASE_OleDB
#define _RYL_BASE_OleDB

#include <oledb.h>
#include <oledberr.h>

//#define _CHECK_OLEDB_PERFORMANCE		// DB 퍼포먼스 체크
#ifdef _CHECK_OLEDB_PERFORMANCE
	#define DB_PERFORMANCE_CHECK(x) x
#else 
	#define DB_PERFORMANCE_CHECK(x) (void*)0;
#endif

//////////////////////////////////////////////////////////////////////////////////////////////
//
// 클래스 정의
//
///////////////////////////////////////////////////////////////////////////////////////////////
class OleDB
{
public:

	const static unsigned long	MaxRowNum		= 2000;					// 최대 열 숫자
	const static unsigned long	MaxErrorLen		= 512;					// 최대 에러 길이
	const static unsigned long	MaxQueryTextLen	= 8192;					// 최대 쿼리 문자열 길이

protected:

	typedef struct _COL_INFO
	{
		const static unsigned short	MaxColNameLen = 100;				// 최대 컬럼 이름 길이

		char				ColName[MaxColNameLen];						// 컬럼 이름 
		unsigned long		ColSize;									// 컬럼 사이즈
	}COL_INFO, *LPCOL_INFO;

	typedef struct _RESULT_COLS
	{
		unsigned long		ColNum;
		DBCOLUMNINFO*		lpDBColumnInfo;
		WCHAR*				lpStringsBuffer;
	}RESULT_COLS, *LPRESULT_COLS;	

	WCHAR		m_QueryText[MaxQueryTextLen];							// 쿼리 문자열
	COL_INFO	m_ColInfo;

public:

	enum ConnType	{ ConnType_ODBC = 0, ConnType_MSSQL = 1, ConnType_ORACLE };
	enum Rowset		{ Rowset_Get = 0, Rowset_Update = 1 };				        // 쿼리 실행후 로우셋

	typedef struct _PARAM_INFO
	{
		const static unsigned short	MaxColNum = 30;					    // 최대 컬럼 이름 길이
	
		unsigned long	ColNum;											// 컬럼 숫자		
        DBPARAMIO       eParamIO[MaxColNum];                            // 컬럼 파라미터 타입
		unsigned long	ColSize[MaxColNum];								// 컬럼 사이즈
        unsigned short	ColType[MaxColNum];                             // 컬럼 타입
	}
    PARAM_INFO, *LPPARAM_INFO;

	/*typedef struct _DB_SP_INFO
	{
		char			szCatalog[0xff];		// 소유 DB		
		char			szType[0xff];			// 소유자.
		char			szSPName[0xff];			// 스토어 프로시저 이름.

	}DB_SP_INFO, *LP_DB_SP_INFO;*/

	typedef struct _SET_BINARY
	{
		unsigned long Size;
	}SET_BINARY, *LPSET_BINARY;

protected:

	IDBInitialize*		m_pIDBInit;
	IDBCreateSession*	m_pIDBCreateSession;
	IDBCreateCommand*	m_pIDBCreateCommand;
	IRowset*			m_pIRowset;
	IRowsetChange*		m_pIRowsetChange;

    HRESULT             m_dwLastError;
	char				m_ErrorString[MaxErrorLen];

	unsigned int		m_dwQueryErrorCount;

public:
	OleDB(void);
	virtual ~OleDB(void);

	inline const char* GetErrorString(void) const { return m_ErrorString; }
    inline HRESULT GetLastError(void)   const { return m_dwLastError;  }
	inline const unsigned int GetQueryErrorCount() const { return m_dwQueryErrorCount; }

	bool ConnectDataSourcePrompt(HWND hWnd_In);
	bool ConnectSQLServer(LPCTSTR ServerName_In, LPCTSTR DataBaseName_In, LPCTSTR UserID_In, LPCTSTR UserPass_In, ConnType ConnType_In);
	bool DisconnectDataSource(void);

	bool ExecuteQuery(LPCTSTR Query_In, Rowset Rowset_In = Rowset_Get);
	bool ExecuteQueryWithParams(LPCTSTR Query_In, char *Data_In, const PARAM_INFO& ColInfo_In);

	bool ExecuteQueryGetData(LPCTSTR Query_In, void *Buffer_Out);

	bool GetData(void *Buffer_Out);
	bool GetData(void **Buffer_Out, int RowSize_In, int Row_In, int *pGetRow_Out);

	bool SetBinaryData(int ColNum_In, LPSET_BINARY lpSetBinary);

	COL_INFO& GetColInfo(void)		{	return m_ColInfo;	}

	//bool GetUserStoredProcedureInfo(DB_SP_INFO* pSpInfo, DWORD dwMaxSPCnt);

private:

	bool HandleError(int ErrorLine_In, HRESULT hResult_In, char *Buffer_In);

	bool CreateSession(void);
	bool DBCreateCommand(void);

	bool AllocResultCols(IUnknown* lpIUnknown_In, RESULT_COLS &Rsult_Cols);
	bool ReleaseResultCols(IUnknown* lpIUnknown_In, RESULT_COLS &Rsult_Cols);

	bool SetConnectionProperties(LPCTSTR ServerName_In, LPCTSTR DataBaseName_In, LPCTSTR UserID_In, LPCTSTR UserPass_In);
	DBBINDING* AllocBindGetData(int ColsNum_In, DBCOLUMNINFO* pDBColumnInfo_In);
	DBBINDING* AllocBindParamInputData(const PARAM_INFO &ColInfo_In);
};

#endif