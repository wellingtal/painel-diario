#pragma once

#include <dbghelp.h>
#include <cstdio>
#include "DLLModule.h"

#pragma comment(lib, "dbghelp.lib")

class CDBGFuncClass : public CDLLModule
{
public :

	DECLARE_DLL_FUNCTION( BOOL,   WINAPI, MiniDumpWriteDump, ( HANDLE, unsigned long, HANDLE, MINIDUMP_TYPE, CONST PMINIDUMP_EXCEPTION_INFORMATION, CONST PMINIDUMP_USER_STREAM_INFORMATION, CONST PMINIDUMP_CALLBACK_INFORMATION ) )
	DECLARE_DLL_FUNCTION( BOOL,   WINAPI, SymEnumSymbols, ( HANDLE, ULONG64, PCSTR, PSYM_ENUMERATESYMBOLS_CALLBACK, PVOID ) )
	DECLARE_DLL_FUNCTION( ULONG,  WINAPI, SymSetContext, ( HANDLE, PIMAGEHLP_STACK_FRAME, PIMAGEHLP_CONTEXT ) )
	DECLARE_DLL_FUNCTION( BOOL,   WINAPI, SymFromAddr, ( HANDLE, DWORD64, PDWORD64, PSYMBOL_INFO ) )
	DECLARE_DLL_FUNCTION( BOOL,   WINAPI, StackWalk, ( unsigned long, HANDLE, HANDLE, LPSTACKFRAME, PVOID, PREAD_PROCESS_MEMORY_ROUTINE, PFUNCTION_TABLE_ACCESS_ROUTINE, PGET_MODULE_BASE_ROUTINE, PTRANSLATE_ADDRESS_ROUTINE ) )
//	DECLARE_DLL_FUNCTION( BOOL,   WINAPI, StackWalk64, ( unsigned long, HANDLE, HANDLE, LPSTACKFRAME64, PVOID, PREAD_PROCESS_MEMORY_ROUTINE64, PFUNCTION_TABLE_ACCESS_ROUTINE64, PGET_MODULE_BASE_ROUTINE64, PTRANSLATE_ADDRESS_ROUTINE64 ) )
 	DECLARE_DLL_FUNCTION( BOOL,   WINAPI, SymGetLineFromAddr, ( HANDLE, unsigned long, PDWORD, PIMAGEHLP_LINE ) )		// ImageHlp
	DECLARE_DLL_FUNCTION( BOOL,   WINAPI, SymGetLineFromAddr64, ( HANDLE, DWORD64, PDWORD, PIMAGEHLP_LINE64 ) )
	DECLARE_DLL_FUNCTION( PVOID,  WINAPI, SymFunctionTableAccess, ( HANDLE, unsigned long ) )								// ImageHlp
//	DECLARE_DLL_FUNCTION( PVOID,  WINAPI, SymFunctionTableAccess64, ( HANDLE, DWORD64 ) )
	DECLARE_DLL_FUNCTION( DWORD64,WINAPI, SymGetModuleBase64, ( HANDLE, DWORD64 ) )
	DECLARE_DLL_FUNCTION( BOOL,   WINAPI, SymInitialize, ( HANDLE, PSTR, BOOL ) )
	DECLARE_DLL_FUNCTION( unsigned long,  WINAPI, SymSetOptions, ( unsigned long ) )
	DECLARE_DLL_FUNCTION( BOOL,   WINAPI, SymCleanup, ( HANDLE ) )
	DECLARE_DLL_FUNCTION( unsigned long,  WINAPI, SymGetModuleBase, ( HANDLE, unsigned long ) )
//	DECLARE_DLL_FUNCTION( DWORD64,WINAPI, SymGetModuleBase64, ( HANDLE, DWORD64 ) )
	DECLARE_DLL_FUNCTION( BOOL,   WINAPI, SymGetTypeInfo, ( HANDLE, DWORD64, ULONG, IMAGEHLP_SYMBOL_TYPE_INFO, PVOID ) )

public:

	BEGIN_DLL_INIT()
	INIT_DLL_FUNCTION( BOOL,   WINAPI, MiniDumpWriteDump, ( HANDLE, unsigned long, HANDLE, MINIDUMP_TYPE, CONST PMINIDUMP_EXCEPTION_INFORMATION, CONST PMINIDUMP_USER_STREAM_INFORMATION, CONST PMINIDUMP_CALLBACK_INFORMATION ) )
	INIT_DLL_FUNCTION( BOOL,   WINAPI, SymEnumSymbols, ( HANDLE, ULONG64, PCSTR, PSYM_ENUMERATESYMBOLS_CALLBACK, PVOID ) )
	INIT_DLL_FUNCTION( ULONG,  WINAPI, SymSetContext, ( HANDLE, PIMAGEHLP_STACK_FRAME, PIMAGEHLP_CONTEXT ) )
	INIT_DLL_FUNCTION( BOOL,   WINAPI, SymFromAddr, ( HANDLE, DWORD64, PDWORD64, PSYMBOL_INFO ) )
	INIT_DLL_FUNCTION( BOOL,   WINAPI, StackWalk, ( unsigned long, HANDLE, HANDLE, LPSTACKFRAME, PVOID, PREAD_PROCESS_MEMORY_ROUTINE, PFUNCTION_TABLE_ACCESS_ROUTINE, PGET_MODULE_BASE_ROUTINE, PTRANSLATE_ADDRESS_ROUTINE ) )
//	INIT_DLL_FUNCTION( BOOL,   WINAPI, StackWalk64, ( unsigned long, HANDLE, HANDLE, LPSTACKFRAME64, PVOID, PREAD_PROCESS_MEMORY_ROUTINE64, PFUNCTION_TABLE_ACCESS_ROUTINE64, PGET_MODULE_BASE_ROUTINE64, PTRANSLATE_ADDRESS_ROUTINE64 ) )
 	INIT_DLL_FUNCTION( BOOL,   WINAPI, SymGetLineFromAddr, ( HANDLE, unsigned long, PDWORD, PIMAGEHLP_LINE ) )		// ImageHlp
	INIT_DLL_FUNCTION( BOOL,   WINAPI, SymGetLineFromAddr64, ( HANDLE, DWORD64, PDWORD, PIMAGEHLP_LINE64 ) )
	INIT_DLL_FUNCTION( PVOID,  WINAPI, SymFunctionTableAccess, ( HANDLE, unsigned long ) )
//	INIT_DLL_FUNCTION( PVOID,  WINAPI, SymFunctionTableAccess64, ( HANDLE, DWORD64 ) )
	INIT_DLL_FUNCTION( DWORD64,WINAPI, SymGetModuleBase64, ( HANDLE, DWORD64 ) )
	INIT_DLL_FUNCTION( BOOL,   WINAPI, SymInitialize, ( HANDLE, PSTR, BOOL ) )
	INIT_DLL_FUNCTION( unsigned long,  WINAPI, SymSetOptions, ( unsigned long ) )
	INIT_DLL_FUNCTION( BOOL,   WINAPI, SymCleanup, ( HANDLE ) )
	INIT_DLL_FUNCTION( unsigned long,  WINAPI, SymGetModuleBase, ( HANDLE, unsigned long ) )
//	INIT_DLL_FUNCTION( DWORD64,WINAPI, SymGetModuleBase64, ( HANDLE, DWORD64 ) )
	INIT_DLL_FUNCTION( BOOL,   WINAPI, SymGetTypeInfo, ( HANDLE, DWORD64, ULONG, IMAGEHLP_SYMBOL_TYPE_INFO, PVOID ) )
	END_DLL_INIT()
};

enum BasicType  // Stolen from CVCONST.H in the DIA 2.0 SDK
{
    btNoType	= 0,
    btVoid		= 1,
    btChar		= 2,
    btWChar		= 3,
    btInt		= 6,
    btUInt		= 7,
    btFloat		= 8,
    btBCD		= 9,
    btBool		= 10,
    btLong		= 13,
    btULong		= 14,
    btCurrency	= 25,
    btDate		= 26,
    btVariant	= 27,
    btComplex	= 28,
    btBit		= 29,
    btBSTR		= 30,
    btHresult	= 31
};

class CExceptionReport
{
public:

	static CExceptionReport& GetInstance();

	typedef int (WINAPI *UserFunc)(TCHAR* szBuffer, const int nBufferSize);

	enum Features
	{
		CATCH_EXCEPTION	= (1 << 0),
		USE_MINIDUMP	= (1 << 1),
		USE_REPORT		= (1 << 2)
	};
		
	void Enable(unsigned long dwEnableFeature);
	void Disable(unsigned long dwDisableFeature);
    void SetDumpLevel(MINIDUMP_TYPE eMiniDumpType) { m_eMiniDumpType = eMiniDumpType; }
	
	void SetUserFunc(UserFunc lpUserFunc) { m_lpUserFunc = lpUserFunc; }
	void WriteExceptionReport(PEXCEPTION_POINTERS pExceptionInfo);

	static void SetProgramName(TCHAR* pszOutBuffer, int nBufferSize, TCHAR* pszProgramName);

	static int WriteBasicInfo(TCHAR* szBuffer_Out, const int nBufferSize, PEXCEPTION_RECORD pExceptionRecord);	
	static int WriteRegistersInfo(TCHAR* szBuffer_Out, const int nBufferSize, PCONTEXT pContext);
	static int WriteMemoryDump(TCHAR* szBuffer_Out, const int nBufferSize, PCONTEXT pContext, 
		unsigned int nMaxIPDump = 16, unsigned int nMaxStackDump = 1024);

    static int WriteStackDetails(TCHAR* szBuffer_Out, const int nBufferSize, 
		PCONTEXT pContext, BOOL bWriteVariables, BOOL& bHasSymbol_Out, const int nStackDepth = 256);

	static int Dump(TCHAR* szBuffer_Out, const int nBufferSize, 
		DWORD64 pData, unsigned long dwSize, BOOL bAlign);

	CDBGFuncClass& GetDBGFuncClass() { return m_DBGHELP; }

private:

	CExceptionReport();
	~CExceptionReport();

	void CloseLogFile();

	LONG ProcessException(PEXCEPTION_POINTERS lpExceptionInfo);
	LONG ProcessSecondException(PEXCEPTION_POINTERS lpExceptionInfo);

	static LONG WINAPI UnhandledExceptionFilter(PEXCEPTION_POINTERS lpExceptionInfo) { return CExceptionReport::GetInstance().ProcessException(lpExceptionInfo); }
	static LONG WINAPI UnhandledSecondExceptionFilter(PEXCEPTION_POINTERS lpExceptionInfo) { return CExceptionReport::GetInstance().ProcessSecondException(lpExceptionInfo); }

	static void StoreCoreDump(void);
	
    static LPTSTR GetExceptionString( unsigned long dwCode );
    static BOOL GetLogicalAddress(PVOID addr, PTSTR szModule, unsigned long len, unsigned long& section, unsigned long& offset );
    
	static BOOL CALLBACK EnumerateSymbolsCallback(PSYMBOL_INFO,ULONG, PVOID);
    static BOOL FormatSymbolValue(PSYMBOL_INFO, STACKFRAME *, TCHAR * pszBuffer, unsigned nBufferSize);

    static int DumpTypeIndex(TCHAR* szBuffer_Out, 
		const int nBufferSize, DWORD64 modBase, unsigned long dwTypeIndex, 
		unsigned int nestingLevel, DWORD_PTR offset, BOOL & bHandled);

    static int FormatOutputValue(TCHAR* pszCurrBuffer, 
		const int nBufferSize, BasicType basicType, DWORD64 length, PVOID pAddress);

    static BasicType GetBasicType(unsigned long typeIndex, DWORD64 modBase);
	
	// ---------------------------------------------------------------------
	// Data

	enum 
	{
		SHIFT_NUM = 4,				
		BYTES_PER_LINE = 16, 	
		MAX_TEMP_VALUE = 5,
		MAX_LOG_BUFFER = 65535		// 64-kb log buffer
	};

	CDBGFuncClass	m_DBGHELP;
	UserFunc		m_lpUserFunc;

	LPTOP_LEVEL_EXCEPTION_FILTER m_OldFilter;

	TCHAR	m_szLogPrefixName[MAX_PATH];
	TCHAR	m_szModuleName[MAX_PATH];
	TCHAR	m_szTempBuffer[MAX_PATH];
	TCHAR	m_szLogBuffer[MAX_LOG_BUFFER];

	CONTEXT		m_tempContext;
	SYSTEMTIME	m_tempSystemTime;

	MINIDUMP_EXCEPTION_INFORMATION	m_miniDumpInfo;

	FILE*			m_logFile;	
	BOOL			m_bHasSymbol;
	unsigned long	m_dwFeaturesFlag;	
    MINIDUMP_TYPE   m_eMiniDumpType;
};

