/* ==========================================================================
  File : ExceptionReport.h
  Vers.: 0.91
  Plat.: Windows 98 or above
  Desc.: ExceptionHandle routine (report)
  Req. : DbgHelp v5.1 or above

  Adapted from MSDN March 2002 - Under the Hood

  first made by cpascal.
  modified by Sparrowhawk.

========================================================================== */

// #define _WIN32_WINDOWS	0x0410		// Windows 98
// #define _WIN32_WINNT		0x0500		// Windows 2000


#include "stdafx.h"
#include "ExceptionReport.h"


#define CHECK_LENGTH_RETURN(length, total_length, retval) \
	if(length < 0) { return retval; } else { total_length += length; }


struct SymEnumSymbols_UserData
{
	STACKFRAME* m_lpsf;
	char*		m_szBuffer_Out;
	int			m_nBufferSize_In;
	int			m_nWritten_Out;
};


// stolen from dia sdk 2.0 (cvconst.h)
enum SymTagEnum
{
    SymTagNull,
    SymTagExe,
    SymTagCompiland,
    SymTagCompilandDetails,
    SymTagCompilandEnv,
    SymTagFunction,
    SymTagBlock,
    SymTagData,
    SymTagAnnotation,
    SymTagLabel,
    SymTagPublicSymbol,
    SymTagUDT,
    SymTagEnum,
    SymTagFunctionType,
    SymTagPointerType,
    SymTagArrayType,
    SymTagBaseType,
    SymTagTypedef,
    SymTagBaseClass,
    SymTagFriend,
    SymTagFunctionArgType,
    SymTagFuncDebugStart,
    SymTagFuncDebugEnd,
    SymTagUsingNamespace,
    SymTagVTableShape,
    SymTagVTable,
    SymTagCustom,
    SymTagThunk,
    SymTagCustomType,
    SymTagManagedType,
    SymTagDimension,
    SymTagMax
};


CExceptionReport& CExceptionReport::GetInstance()
{
	static CExceptionReport exceptionReport;
	return exceptionReport;
}


CExceptionReport::CExceptionReport()
:	m_dwFeaturesFlag(0),
	m_OldFilter(0),
	m_lpUserFunc(0),	
	m_logFile(0),
    m_eMiniDumpType(MiniDumpNormal)
{
	SetProgramName(m_szLogPrefixName, MAX_PATH, 0);
	Enable(CATCH_EXCEPTION | USE_MINIDUMP | USE_REPORT);

	m_DBGHELP.Init("DBGHELP.DLL");
}


CExceptionReport::~CExceptionReport()
{
	m_DBGHELP.Release();

	Disable(CATCH_EXCEPTION | USE_MINIDUMP | USE_REPORT);
	CloseLogFile();
}

// 헤더 참조를 줄이기 위해서(갖다 쓰기 좋게 하기 위해서) 이곳에 넣음.
void CExceptionReport::SetProgramName(TCHAR* pszOutBuffer, int nBufferSize, TCHAR* pszProgramName)
{
	if(0 == pszProgramName)
	{
		TCHAR szDrive[MAX_PATH], szDir[MAX_PATH], szFilename[MAX_PATH], szExt[MAX_PATH];

		// Figure out what the report file will be named, and store it away
	    GetModuleFileName(0, pszOutBuffer, nBufferSize);

		PTSTR pszDot = pszOutBuffer;
	    
		// Look for the '.' before the "EXE" extension. Replace '.' to '\0'
		if((pszDot = _tcsrchr( pszDot, _T('.')))) 
		{
			*pszDot = 0;
		}

		_tsplitpath(pszOutBuffer, szDrive, szDir, szFilename, szExt);
		_tcsncpy(pszOutBuffer, szFilename, nBufferSize);
	}
	else
	{
		_tcsncpy(pszOutBuffer, pszProgramName, nBufferSize);
	}
}

void CExceptionReport::CloseLogFile()
{
	if(m_logFile) 
	{
		fprintf(m_logFile, "Exception occured AGAIN! EXITING NOW!\r\n");
		fclose(m_logFile);
		m_logFile = 0;
	}
}

void CExceptionReport::Enable(unsigned long dwEnableFeature)
{
	if(0 == m_OldFilter && (dwEnableFeature & CATCH_EXCEPTION))
	{
        m_OldFilter = SetUnhandledExceptionFilter(UnhandledExceptionFilter);
	}

	m_dwFeaturesFlag |= dwEnableFeature;
}

void CExceptionReport::Disable(unsigned long dwDisableFeature)
{
	if(0 != m_OldFilter && (dwDisableFeature & CATCH_EXCEPTION))
	{
		SetUnhandledExceptionFilter(m_OldFilter);
		m_OldFilter = 0;
	}

	m_dwFeaturesFlag &= ~dwDisableFeature;
}


LONG CExceptionReport::ProcessSecondException(PEXCEPTION_POINTERS lpExceptionInfo)
{
	CloseLogFile();
    return (0 != m_OldFilter) ? m_OldFilter(lpExceptionInfo) : EXCEPTION_CONTINUE_SEARCH;
}


LONG CExceptionReport::ProcessException(PEXCEPTION_POINTERS lpExceptionInfo)
{
    SetUnhandledExceptionFilter(UnhandledSecondExceptionFilter);

	GetLocalTime(&m_tempSystemTime);

	_tcscpy(m_szTempBuffer, m_szLogPrefixName);
	wsprintf(m_szTempBuffer + strlen(m_szTempBuffer), " %04d-%02d-%02d %02d,%02d,%02d.dmp", 
		m_tempSystemTime.wYear, m_tempSystemTime.wMonth, m_tempSystemTime.wDay,
		m_tempSystemTime.wHour, m_tempSystemTime.wMinute, m_tempSystemTime.wSecond );

	HANDLE hDumpFile = CreateFile(m_szTempBuffer, GENERIC_WRITE, FILE_SHARE_READ,
		0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0 );

	if(m_DBGHELP.MiniDumpWriteDump && INVALID_HANDLE_VALUE != hDumpFile)
	{
		memset(&m_miniDumpInfo, 0, sizeof(MINIDUMP_EXCEPTION_INFORMATION));

		m_miniDumpInfo.ThreadId = ::GetCurrentThreadId();
		m_miniDumpInfo.ExceptionPointers = lpExceptionInfo;
		m_miniDumpInfo.ClientPointers = 0;

/*
		MiniDump Options 

		MiniDumpWithDataSegs
		MiniDumpWithFullMemory
*/

		m_DBGHELP.MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), 
			hDumpFile, m_eMiniDumpType, &m_miniDumpInfo, 0, 0);

		CloseHandle(hDumpFile);
	}

	_tcscpy(m_szTempBuffer, m_szLogPrefixName);
	wsprintf(m_szTempBuffer + strlen(m_szTempBuffer), " %04d-%02d-%02d %02d,%02d,%02d.txt", 
		m_tempSystemTime.wYear, m_tempSystemTime.wMonth, m_tempSystemTime.wDay,
		m_tempSystemTime.wHour, m_tempSystemTime.wMinute, m_tempSystemTime.wSecond );

	m_logFile = fopen(m_szTempBuffer, "wt");
    	
	if(0 != m_logFile)
	{		
		WriteExceptionReport(lpExceptionInfo);
		
		if(m_lpUserFunc)
		{
			fprintf(m_logFile, 
                _T( "------------------------------------------------------------------------------\r\n")
				_T( "    Application-specific log\r\n" )
				_T( "------------------------------------------------------------------------------\r\n")
				_T( "\r\n" ));
            
			fflush(m_logFile);

			__try
			{
                int nLength = m_lpUserFunc(m_szLogBuffer, MAX_LOG_BUFFER);
				if(0 < nLength)
				{
					m_szLogBuffer[MAX_LOG_BUFFER - 1] = 0;
					fprintf(m_logFile, "%s", m_szLogBuffer);
				}
			}
			__except(EXCEPTION_EXECUTE_HANDLER)
			{
				fprintf(m_logFile, "Exception occured by UserFunc : ExceptionCode : %d - %s",
					GetExceptionCode(), GetExceptionString(GetExceptionCode()));
			}
			
			fprintf(m_logFile, _T("\r\n\r\n")
				_T( "==============================================================================\r\n"));

			CloseLogFile();
		}
	}

	return EXCEPTION_EXECUTE_HANDLER;
}


//
// Write informations to Report file. Called by UnhandledExceptionFilter
//
void CExceptionReport::WriteExceptionReport(PEXCEPTION_POINTERS pExceptionInfo)
{
	// Start out with a banner
	if(0 == m_logFile)
	{
		return;
	}

	fprintf(m_logFile, 
		_T( "==============================================================================\r\n\r\n" ));
	
	if(WriteBasicInfo(m_szLogBuffer, MAX_LOG_BUFFER, 
		pExceptionInfo->ExceptionRecord) < 0)
	{
		_sntprintf(m_szLogBuffer, MAX_LOG_BUFFER, 
			"Failed to write basic info.\r\n\r\n");
	}

	_fputts(m_szLogBuffer, m_logFile);
	fflush(m_logFile);
	
	if(WriteRegistersInfo(m_szLogBuffer, MAX_LOG_BUFFER,		
		pExceptionInfo->ContextRecord) < 0)
	{
		_sntprintf(m_szLogBuffer, MAX_LOG_BUFFER,
			"Failed to write registers info.\r\n\r\n");
	}

	_fputts(m_szLogBuffer, m_logFile);
	fflush(m_logFile);

    if(!m_DBGHELP.SymInitialize || 
		!m_DBGHELP.SymInitialize(GetCurrentProcess(), 0, TRUE))
	{        
		return;
	}

    m_tempContext = *(pExceptionInfo->ContextRecord);
    
	BOOL bHasSymbol = FALSE;

	if(WriteStackDetails(m_szLogBuffer, MAX_LOG_BUFFER,
		&m_tempContext, FALSE, bHasSymbol) < 0)
	{
		_sntprintf(m_szLogBuffer, MAX_LOG_BUFFER,
			"Failed to write stack details.\r\n\r\n");
	}

	_fputts(m_szLogBuffer, m_logFile);
	fflush(m_logFile);

	if(bHasSymbol)
	{
/*		m_tempContext = *(pExceptionInfo->ContextRecord);

		if(WriteStackDetails(m_szLogBuffer, MAX_LOG_BUFFER,
			&m_tempContext, FALSE, bHasSymbol) < 0)
		{
			_sntprintf(m_szLogBuffer, MAX_LOG_BUFFER,
				"Failed to write stack details.\r\n\r\n");
		}

		_fputts(m_szLogBuffer, m_logFile);
		fflush(m_logFile);


		전역변수를 로그로 남기는 루틴. 단, 용량이 꽤 되므로, 지금은 남기기 않는다.
		_fputts(
			_T( "------------------------------------------------------------------------------\r\n" )
            _T( "    Global Variables\r\n" )
			_T( "------------------------------------------------------------------------------\r\n" )
			_T( "\r\n" ));

		if(m_DBGHELP.SymEnumSymbols)
		{
			m_DBGHELP.SymEnumSymbols( GetCurrentProcess(), (DWORD64) GetModuleHandle( m_szModuleName ), 0, EnumerateSymbolsCallback, 0 );
		}
*/
	}

	if(m_DBGHELP.SymCleanup)
	{
		m_DBGHELP.SymCleanup(GetCurrentProcess());
	}

	_fputts(_T("\r\n\r\n\r\n"), m_logFile);

	if(WriteMemoryDump(m_szLogBuffer, MAX_LOG_BUFFER,
		pExceptionInfo->ContextRecord) < 0)
	{
		_sntprintf(m_szLogBuffer, MAX_LOG_BUFFER,
			"Failed to write memory dump.\r\n\r\n");
	}

	_fputts(m_szLogBuffer, m_logFile);
	fflush(m_logFile);
	return;
}

//
// Write basic informations(user, computer name, error type)
//
int CExceptionReport::WriteBasicInfo(TCHAR* szBuffer_Out, const int nBufferSize, 
									  PEXCEPTION_RECORD pExceptionRecord)
{
	TCHAR szProgramName[MAX_PATH];
	TCHAR szFileName[MAX_PATH];
	TCHAR szUserName[MAX_PATH];
	TCHAR szComputerName[MAX_PATH];
	TCHAR szModuleName[MAX_PATH];

	unsigned long dwUserLen = MAX_PATH;
	unsigned long dwComputerLen = MAX_PATH;
	unsigned long dwSection = 0;
	unsigned long dwOffset = 0;
	
	SetProgramName(szProgramName, MAX_PATH, 0);
    GetModuleFileName((HMODULE)0, szFileName, MAX_PATH);	
	GetUserName(szUserName, &dwUserLen);
	GetComputerName(szComputerName, &dwComputerLen);

	// Print information about the type of fault and where the fault occured    
    GetLogicalAddress(pExceptionRecord->ExceptionAddress, 
		szModuleName, sizeof(szModuleName), dwSection, dwOffset);

	return _sntprintf(szBuffer_Out, nBufferSize, 
		_T("------------------------------------------------------------------------------\r\n")
        _T("    Basic Information\r\n")
        _T("------------------------------------------------------------------------------\r\n")
        _T("\r\n")
		_T("Program Name : %s\r\n")
		_T("EXE          : %s\r\n")
		_T("User         : %s\r\n")
		_T("Computer     : %s\r\n")
		_T("\r\n")
		_T("Program      : %s\r\n")
		_T("Exception    : %08x (%s)\r\n")
		_T("Fault Address: %08x %02X:%08x\r\n\r\n\r\n"), 

		szProgramName,
		szFileName,
		szUserName,
		szComputerName,
		szModuleName,
		pExceptionRecord->ExceptionCode,
		GetExceptionString(pExceptionRecord->ExceptionCode),
		pExceptionRecord->ExceptionAddress,
		dwSection,
		dwOffset);
}

//
// Show the registers
//
int CExceptionReport::WriteRegistersInfo(TCHAR* szBuffer_Out, const int nBufferSize, PCONTEXT pContext)
{
	return _sntprintf(szBuffer_Out, nBufferSize,
        
		_T("------------------------------------------------------------------------------\r\n")
        _T("    x86 Registers\r\n")
		_T("------------------------------------------------------------------------------\r\n")
		_T("\r\n")
		_T("EAX=%08x  EBX=%08x  ECX=%08x  EDX=%08x\r\n")
		_T("ESI=%08x  EDI=%08x  EBP=%08x\r\n")
		_T("DS =%04X      ES=%04X       FS=%04X       GS:%04X\r\n")
		_T("CS:EIP=%04X:%08x\r\n")
		_T("SS:ESP=%04X:%08x\r\n")
		_T("Flags=%08x\r\n")
		_T("\r\n\r\n"),

		pContext->Eax, pContext->Ebx, pContext->Ecx, pContext->Edx,
		pContext->Esi, pContext->Edi, pContext->Ebp, 
		pContext->SegDs, pContext->SegEs, pContext->SegFs, pContext->SegGs,
		pContext->SegCs, pContext->Eip,
		pContext->SegSs, pContext->Esp,
		pContext->EFlags);
}

//
// Show the Memory
//
int CExceptionReport::WriteMemoryDump(TCHAR* szBuffer_Out, 
									  const int nBufferSize, 
									  PCONTEXT pContext,
									  unsigned int nMaxIPDump,
									  unsigned int nMaxStackDump)
{
	int nTotalLength = 0;
	int nLength = _sntprintf(szBuffer_Out, nBufferSize,
        
		_T("------------------------------------------------------------------------------\r\n")
		_T("    Memory Dump\r\n")
		_T("------------------------------------------------------------------------------\r\n")
		_T("\r\n" )
		_T("Code: %d bytes starting at (EIP = %08lX)\r\n"), nMaxIPDump, pContext->Eip);

	CHECK_LENGTH_RETURN(nLength, nTotalLength, -1);

	nLength = Dump(szBuffer_Out + nTotalLength, 
		nBufferSize - nTotalLength, 
		pContext->Eip, nMaxIPDump, FALSE);

	CHECK_LENGTH_RETURN(nLength, nTotalLength, -1);
	
	
	nLength = _snprintf(szBuffer_Out + nTotalLength, nBufferSize - nTotalLength,
		_T("\r\nStack: %d bytes starting at (ESP = %08lX)\r\n"), nMaxStackDump, pContext->Esp);

	CHECK_LENGTH_RETURN(nLength, nTotalLength, -1);

	
	nLength = Dump(szBuffer_Out + nTotalLength, 
		nBufferSize - nTotalLength, 
		pContext->Esp, nMaxStackDump, TRUE);

	CHECK_LENGTH_RETURN(nLength, nTotalLength, -1);

	
	nLength = _snprintf(szBuffer_Out + nTotalLength, nBufferSize - nTotalLength, _T("\r\n\r\n"));
	CHECK_LENGTH_RETURN(nLength, nTotalLength, -1);

	return nTotalLength;
}


LPTSTR CExceptionReport::GetExceptionString(unsigned long dwCode)
{
	#define EXCEPTION( x ) case EXCEPTION_##x: return _T(#x)
    
	switch(dwCode) 
	{
        EXCEPTION(ACCESS_VIOLATION);
        EXCEPTION(DATATYPE_MISALIGNMENT);
        EXCEPTION(BREAKPOINT);
        EXCEPTION(SINGLE_STEP);
        EXCEPTION(ARRAY_BOUNDS_EXCEEDED);
        EXCEPTION(FLT_DENORMAL_OPERAND);
        EXCEPTION(FLT_DIVIDE_BY_ZERO);
        EXCEPTION(FLT_INEXACT_RESULT);
        EXCEPTION(FLT_INVALID_OPERATION);
        EXCEPTION(FLT_OVERFLOW);
        EXCEPTION(FLT_STACK_CHECK);
        EXCEPTION(FLT_UNDERFLOW);
        EXCEPTION(INT_DIVIDE_BY_ZERO);
        EXCEPTION(INT_OVERFLOW);
        EXCEPTION(PRIV_INSTRUCTION);
        EXCEPTION(IN_PAGE_ERROR);
        EXCEPTION(ILLEGAL_INSTRUCTION);
        EXCEPTION(NONCONTINUABLE_EXCEPTION);
        EXCEPTION(STACK_OVERFLOW);
        EXCEPTION(INVALID_DISPOSITION);
        EXCEPTION(GUARD_PAGE);
        EXCEPTION(INVALID_HANDLE);
    }

	#undef EXCEPTION

    // If not one of the "known" exceptions, try to get the string
    // from NTDLL.DLL's message table.

    __declspec(thread) static TCHAR s_szBuffer[MAX_PATH] = { 0 };

    FormatMessage( FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_HMODULE,
                   GetModuleHandle( _T("NTDLL.DLL") ),
                   dwCode, 0, s_szBuffer, MAX_PATH, 0 );

    return s_szBuffer;
}


//                                                                             
// Given a linear address, locates the module, section, and offset containing that address.                                                               
// Note: the szModule paramater buffer is an output buffer of length specified by the len parameter (in TCHARacters!)                                       
//                                                                             
BOOL CExceptionReport::GetLogicalAddress(PVOID addr, PTSTR szModule, 
										 unsigned long len, 
										 unsigned long& section, 
										 unsigned long& offset )
{
    MEMORY_BASIC_INFORMATION mbi;

	if(!VirtualQuery(addr, &mbi, sizeof(mbi)))
	{
        return FALSE;
	}

    PVOID hMod = mbi.AllocationBase;
	if(NULL == hMod) 
	{
		return FALSE;
	}

	if(!GetModuleFileName((HMODULE) hMod, szModule, len))
	{
		return FALSE;
	}

    // Point to the DOS header in memory
    PIMAGE_DOS_HEADER pDosHdr = (PIMAGE_DOS_HEADER) hMod;

    // From the DOS header, find the NT (PE) header
    PIMAGE_NT_HEADERS pNtHdr = (PIMAGE_NT_HEADERS)(((DWORD64) hMod) + pDosHdr->e_lfanew);

    PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION( pNtHdr );

    DWORD64 rva = (DWORD64)addr - (DWORD64)hMod; // RVA is offset from module load address

    // Iterate through the section table, looking for the one that encompasses
    // the linear address.
    for( unsigned i = 0; i < pNtHdr->FileHeader.NumberOfSections; i++, pSection++ ) 
	{
        unsigned long sectionStart = pSection->VirtualAddress;
        unsigned long sectionEnd = sectionStart + max(pSection->SizeOfRawData, pSection->Misc.VirtualSize);

        // Is the address in this section???
        if ( (rva >= sectionStart) && (rva <= sectionEnd) ) 
		{
            // Yes, address is in the section.  Calculate section and offset,
            // and store in the "section" & "offset" params, which were
            // passed by reference.
            section = i+1;
            offset = (unsigned long) (rva - sectionStart);
            return TRUE;
        }
    }

    return FALSE;   // Should never get here!
}


int CExceptionReport::WriteStackDetails(TCHAR* szBuffer_Out, const int nBufferSize, 
										PCONTEXT pContext, 
										BOOL bWriteVariables, /* TRUE if local/params should be output */
										BOOL& bHasSymbol_Out,
										const int nStackDepth)  
{
	int nTotalLength = 0;
	int nLength = _sntprintf(szBuffer_Out, nBufferSize, 
		_T("------------------------------------------------------------------------------\r\n")
		_T("    Call Stack (%s)\r\n")
		_T("------------------------------------------------------------------------------\r\n")
		_T("\r\n" )
		_T("Address   Frame     Function                        SourceFile\r\n"), 
		bWriteVariables ? "Detail" : "Short");

	CHECK_LENGTH_RETURN(nLength, nTotalLength, -1);

    unsigned long dwMachineType = 0;
    
	// Could use SymSetOptions here to add the SYMOPT_DEFERRED_LOADS flag
    STACKFRAME sf;
	memset( &sf, 0, sizeof(sf) );

    // Initialize the STACKFRAME structure for the first call.  This is only
    // necessary for Intel CPUs, and isn't mentioned in the documentation.
    sf.AddrPC.Offset       = pContext->Eip;
    sf.AddrPC.Mode         = AddrModeFlat;
    sf.AddrStack.Offset    = pContext->Esp;
    sf.AddrStack.Mode      = AddrModeFlat;
    sf.AddrFrame.Offset    = pContext->Ebp;
    sf.AddrFrame.Mode      = AddrModeFlat;

    dwMachineType = IMAGE_FILE_MACHINE_I386;

	const int MAX_SYMBOL_NAME_LEN = 2048;
	BYTE symbolBuffer[sizeof(SYMBOL_INFO) + MAX_SYMBOL_NAME_LEN];
	TCHAR szModule[MAX_PATH] = _T("");
	IMAGEHLP_LINE lineInfo;
	IMAGEHLP_STACK_FRAME imagehlpStackFrame;

	SymEnumSymbols_UserData symbol_userdata;

	CDBGFuncClass& dbgFuncClass = CExceptionReport::GetInstance().GetDBGFuncClass();

	if(dbgFuncClass.StackWalk)
	{
		for(int nCount = 0; nCount < nStackDepth; ++nCount)
		{
			if(0 == sf.AddrFrame.Offset || 
				!dbgFuncClass.StackWalk(dwMachineType, GetCurrentProcess(), GetCurrentThread(), &sf, pContext, 0, 
				dbgFuncClass.SymFunctionTableAccess, dbgFuncClass.SymGetModuleBase, 0))
			{
				break;
			}

			nLength = _sntprintf(szBuffer_Out + nTotalLength, nBufferSize - nTotalLength, 
				_T("%08x  %08x  "), sf.AddrPC.Offset, sf.AddrFrame.Offset);

			CHECK_LENGTH_RETURN(nLength, nTotalLength, -1);

			// Get the name of the function for this stack frame entry
			PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)symbolBuffer;
			pSymbol->SizeOfStruct = sizeof(symbolBuffer);
			pSymbol->MaxNameLen = MAX_SYMBOL_NAME_LEN;
	                        
			DWORD64 symDisplacement = 0;    // Displacement of the input address,
											// relative to the start of the symbol
			if(dbgFuncClass.SymFromAddr && 
				dbgFuncClass.SymFromAddr(GetCurrentProcess(), sf.AddrPC.Offset, &symDisplacement, pSymbol))
			{
				wsprintf(szModule, _T("%-s+0x%p"), pSymbol->Name, (void*)symDisplacement);

				nLength = _sntprintf(szBuffer_Out + nTotalLength, 
					nBufferSize - nTotalLength, 
                    _T( "%-30s" ), szModule);				
			} 
			else 
			{
				// No symbol found.  Print out the logical address instead.				
				unsigned long section = 0, offset = 0;

				GetLogicalAddress((PVOID)(DWORD64)sf.AddrPC.Offset, szModule, sizeof(szModule), section, offset);
				
				nLength = _sntprintf(szBuffer_Out + nTotalLength, nBufferSize - nTotalLength, 
					_T("%04X:%08x                   %s"), section, offset, szModule);
			}

			CHECK_LENGTH_RETURN(nLength, nTotalLength, -1);

			// Get the source line for this stack frame entry

			memset(&lineInfo, 0, sizeof(IMAGEHLP_LINE));
			lineInfo.SizeOfStruct = sizeof(IMAGEHLP_LINE);

			unsigned long dwLineDisplacement = 0;
			if(dbgFuncClass.SymGetLineFromAddr && 
				(dbgFuncClass.SymGetLineFromAddr(GetCurrentProcess(), sf.AddrPC.Offset, &dwLineDisplacement, &lineInfo)))
			{
				nLength = _sntprintf(szBuffer_Out + nTotalLength, nBufferSize - nTotalLength, 
					_T("  %s line %u"),lineInfo.FileName,lineInfo.LineNumber); 
				
				CHECK_LENGTH_RETURN(nLength, nTotalLength, -1);
				
				if(bHasSymbol_Out == FALSE)
				{
					bHasSymbol_Out = TRUE;
				}
			}

			nLength = _sntprintf(szBuffer_Out + nTotalLength, nBufferSize - nTotalLength, _T("\r\n")); 
			CHECK_LENGTH_RETURN(nLength, nTotalLength, -1);

			// Write out the variables, if desired
			if(bWriteVariables) 
			{
				// Use SymSetContext to get just the locals/params for this frame				
				memset(&imagehlpStackFrame, 0, sizeof(IMAGEHLP_STACK_FRAME));

				imagehlpStackFrame.InstructionOffset = sf.AddrPC.Offset;

				if(dbgFuncClass.SymSetContext)
				{
					dbgFuncClass.SymSetContext(GetCurrentProcess(), 
						&imagehlpStackFrame, 0);
				}

				// Enumerate the locals/parameters
				if(dbgFuncClass.SymEnumSymbols)
				{
					symbol_userdata.m_lpsf			= &sf;
					symbol_userdata.m_szBuffer_Out	= szBuffer_Out + nTotalLength;
					symbol_userdata.m_nWritten_Out	= 0;
					symbol_userdata.m_nBufferSize_In = nBufferSize - nTotalLength;

					dbgFuncClass.SymEnumSymbols(GetCurrentProcess(), 
						0, 0, EnumerateSymbolsCallback, &symbol_userdata);

					CHECK_LENGTH_RETURN(symbol_userdata.m_nWritten_Out, nTotalLength, -1);					
				}

				nLength = _sntprintf(szBuffer_Out + nTotalLength, nBufferSize - nTotalLength, _T("\r\n")); 
				CHECK_LENGTH_RETURN(nLength, nTotalLength, -1);
			}
		}
	}

	nLength = _sntprintf(szBuffer_Out + nTotalLength, nBufferSize - nTotalLength, _T("\r\n\r\n"));
	CHECK_LENGTH_RETURN(nLength, nTotalLength, -1);

	return nTotalLength;
}


//
// The function invoked by SymEnumSymbols
//
BOOL CALLBACK CExceptionReport::EnumerateSymbolsCallback(PSYMBOL_INFO pSymInfo, ULONG SymbolSize, PVOID UserContext)
{
	const int MAX_NAME_BUFFER = 3072;
    TCHAR szBuffer[MAX_NAME_BUFFER];

	SymEnumSymbols_UserData* lpSymbol_UserData = 
		reinterpret_cast<SymEnumSymbols_UserData*>(UserContext);

    __try 
	{
        if(0 < FormatSymbolValue(pSymInfo, lpSymbol_UserData->m_lpsf, szBuffer, MAX_NAME_BUFFER))
		{
            lpSymbol_UserData->m_nWritten_Out = _sntprintf(
				lpSymbol_UserData->m_szBuffer_Out, 
				lpSymbol_UserData->m_nBufferSize_In,
				_T("\t%s\r\n"), szBuffer);
		}
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
	{
        lpSymbol_UserData->m_nWritten_Out = _sntprintf(
			lpSymbol_UserData->m_szBuffer_Out, 
			lpSymbol_UserData->m_nBufferSize_In,			
			_T("punting on symbol %s\r\n"), pSymInfo->Name);
    }

    return TRUE;
}


//
// Given a SYMBOL_INFO representing a particular variable, displays its contents.
// If it's a user defined type, display the members and their values.
//
int CExceptionReport::FormatSymbolValue(PSYMBOL_INFO pSym, STACKFRAME * sf,
										TCHAR* pszBuffer, unsigned int nBufferSize)
{
	int nLength = 0;
	int nTotalLength = 0;

    // If it's a function, don't do anything.
    if(pSym->Tag == SymTagFunction)
	{
        return 0;
	}

    // Indicate if the variable is a local or parameter
    if(pSym->Flags & IMAGEHLP_SYMBOL_INFO_PARAMETER)
	{
		nLength = _sntprintf(pszBuffer, nBufferSize, "Parameter ");
		CHECK_LENGTH_RETURN(nLength, nTotalLength, -1);
	}
    else if(pSym->Flags & IMAGEHLP_SYMBOL_INFO_LOCAL)
	{
		nLength = _sntprintf(pszBuffer, nBufferSize, "Local ");
		CHECK_LENGTH_RETURN(nLength, nTotalLength, -1);
	}

    // Emit the variable name
	nLength = _sntprintf(pszBuffer + nTotalLength, nBufferSize - nTotalLength, "\'%s\'", pSym->Name);
	CHECK_LENGTH_RETURN(nLength, nTotalLength, -1);

    
	DWORD_PTR pVariable = 0;    // Will point to the variable's data in memory

    if(pSym->Flags & IMAGEHLP_SYMBOL_INFO_REGRELATIVE) 
	{
        pVariable = sf->AddrFrame.Offset;
        pVariable += (DWORD_PTR)pSym->Address;
    }
    else if (pSym->Flags & IMAGEHLP_SYMBOL_INFO_REGISTER) 
	{
		// Don't try to report register variable
        return 0;
    }
	else 
	{
		// It must be a global variable
        pVariable = (DWORD_PTR)pSym->Address;
    }

    // Determine if the variable is a user defined type (UDT).  IF so, bHandled
    // will return TRUE.
    BOOL bHandled;

	nLength = DumpTypeIndex(pszBuffer + nTotalLength, nBufferSize - nTotalLength,
		pSym->ModBase, pSym->TypeIndex, 0, pVariable, bHandled);

	CHECK_LENGTH_RETURN(nLength, nTotalLength, -1);
    
	if(!bHandled) 
	{
        // The symbol wasn't a UDT, so do basic, stupid formatting of the
        // variable.  Based on the size, we're assuming it's a TCHAR, WORD, or
        // unsigned long.
        BasicType basicType = GetBasicType(pSym->TypeIndex, pSym->ModBase);

		nLength = FormatOutputValue(pszBuffer + nTotalLength, nBufferSize - nTotalLength,
			basicType, pSym->Size, (PVOID) pVariable);

        CHECK_LENGTH_RETURN(nLength, nTotalLength, -1);
    }

    return nTotalLength;
}

//////////////////////////////////////////////////////////////////////////////
// If it's a user defined type (UDT), recurse through its members until we're
// at fundamental types.  When he hit fundamental types, return
// bHandled = FALSE, so that FormatSymbolValue() will format them.
//////////////////////////////////////////////////////////////////////////////
int CExceptionReport::DumpTypeIndex(TCHAR* szBuffer_Out, 
									const int nBufferSize,
									DWORD64 modBase, 
									unsigned long dwTypeIndex, 
									unsigned int nestingLevel, 
									DWORD_PTR offset, 
									BOOL & bHandled)
{
    bHandled = FALSE;

	int nTotalLength = 0;
	int nLength = 0;

	CDBGFuncClass& dbgFuncClass = CExceptionReport::GetInstance().GetDBGFuncClass();

	if(!dbgFuncClass.SymGetTypeInfo)
	{
		return 0;
	}

    // Get the name of the symbol.  This will either be a Type name (if a UDT), or the structure member name.
    WCHAR* pwszTypeName = 0;

    if(dbgFuncClass.SymGetTypeInfo(GetCurrentProcess(), 
		modBase, dwTypeIndex, TI_GET_SYMNAME, &pwszTypeName))
	{
		nLength += _snprintf(szBuffer_Out, nBufferSize, " %ls", pwszTypeName);
        LocalFree(pwszTypeName);

		CHECK_LENGTH_RETURN(nLength, nTotalLength, -1);
    }

    // Determine how many children this type has.
    unsigned long dwChildrenCount = 0;
    dbgFuncClass.SymGetTypeInfo(GetCurrentProcess(), modBase, 
		dwTypeIndex, TI_GET_CHILDRENCOUNT, &dwChildrenCount);

    if(!dwChildrenCount)     // If no children, we're done
	{
        return nTotalLength;
	}

    // Prepare to get an array of "TypeIds", representing each of the children.
    // SymGetTypeInfo(TI_FINDCHILDREN) expects more memory than just a
    // TI_FINDCHILDREN_PARAMS struct has.  Use derivation to accomplish this.
    struct FINDCHILDREN : TI_FINDCHILDREN_PARAMS 
	{
        ULONG   MoreChildIds[1024];
		FINDCHILDREN() { Count = sizeof(MoreChildIds) / sizeof(MoreChildIds[0]); }
    } children;

    children.Count = dwChildrenCount;
    children.Start = 0;

    // Get the array of TypeIds, one for each child type
    if(!dbgFuncClass.SymGetTypeInfo(GetCurrentProcess(), 
		modBase, dwTypeIndex, TI_FINDCHILDREN, &children))
	{
        return nTotalLength;
    }

    // Append a line feed
	nLength += _snprintf(szBuffer_Out + nTotalLength, nBufferSize - nTotalLength, "\r\n");
	CHECK_LENGTH_RETURN(nLength, nTotalLength, -1);

    // Iterate through each of the children
    for(unsigned int nChildrenCount = 0; nChildrenCount < dwChildrenCount; ++nChildrenCount) 
	{
        // Add appropriate indentation level (since this routine is recursive)
        for( unsigned int j = 0; j <= nestingLevel+1; j++ )
		{
            szBuffer_Out += sprintf( szBuffer_Out, "\t" );
		}

        // Recurse for each of the child types
        BOOL bHandled2;

        nLength = DumpTypeIndex(szBuffer_Out + nTotalLength, nBufferSize - nTotalLength, 
			modBase, children.ChildId[nChildrenCount], nestingLevel+1, offset, bHandled2);
		CHECK_LENGTH_RETURN(nLength, nTotalLength, -1);

        // If the child wasn't a UDT, format it appropriately
        if(!bHandled2)
		{
            // Get the offset of the child member, relative to its parent
            unsigned long dwMemberOffset = 0;
            dbgFuncClass.SymGetTypeInfo( GetCurrentProcess(), 
				modBase, children.ChildId[nChildrenCount], TI_GET_OFFSET, &dwMemberOffset);

            // Get the real "TypeId" of the child.  We need this for the SymGetTypeInfo( TI_GET_TYPEID ) call below.
            unsigned long typeId = 0;
            dbgFuncClass.SymGetTypeInfo( GetCurrentProcess(), 
				modBase, children.ChildId[nChildrenCount], TI_GET_TYPEID, &typeId);

            // Get the size of the child member
            ULONG64 length = 0;
            dbgFuncClass.SymGetTypeInfo(GetCurrentProcess(), modBase, 
				typeId, TI_GET_LENGTH, &length);

            // Calculate the address of the member
            DWORD_PTR dwFinalOffset = offset + dwMemberOffset;

            BasicType basicType = GetBasicType(children.ChildId[nChildrenCount], modBase );

			nLength = FormatOutputValue(szBuffer_Out + nTotalLength, 
				nBufferSize - nTotalLength, basicType, length, (PVOID)dwFinalOffset);
			CHECK_LENGTH_RETURN(nLength, nTotalLength, -1);

			nLength = _sntprintf(szBuffer_Out + nTotalLength, 
				nBufferSize - nTotalLength, "\r\n");

			CHECK_LENGTH_RETURN(nLength, nTotalLength, -1);
        }
    }

    bHandled = TRUE;
    return nTotalLength;
}

int CExceptionReport::FormatOutputValue(TCHAR* szBuffer_Out, 
										const int nBufferSize,
										BasicType basicType, 
										DWORD64 length, PVOID pAddress)
{
	int nLength = 0;
	int nTotalLength = 0;

    // Format appropriately (assuming it's a 1, 2, or 4 bytes (!!!)
    if(length == 1)
	{
		nLength = _sntprintf(szBuffer_Out, nBufferSize, " = %X", *(PBYTE)pAddress);
	}
    else if(length == 2)
	{
        nLength = _sntprintf(szBuffer_Out, nBufferSize, " = %X", *(PWORD)pAddress);
	}
    else if(length == 4) 
	{
        if(basicType == btFloat) 
		{
            nLength = _sntprintf(szBuffer_Out, nBufferSize, " = %f", *(PFLOAT)pAddress);
        }
		else if(basicType == btChar) 
		{
            if(!IsBadStringPtr(*(PSTR*)pAddress, 32)) 
			{
                nLength = _sntprintf(szBuffer_Out, nBufferSize, " = \"%.31s\"", *(PDWORD)pAddress);
			}
			else
			{
                nLength = _sntprintf(szBuffer_Out, nBufferSize, " = %X", *(PDWORD)pAddress);
			}
        } 
		else
		{
            nLength = _sntprintf(szBuffer_Out, nBufferSize, " = %X", *(PDWORD)pAddress);
		}
    }
	else if (length == 8) 
	{
        if(basicType == btFloat)
		{
            nLength = _sntprintf(szBuffer_Out, nBufferSize, " = %lf", *(double *)pAddress );
		}
		else
		{	
			TCHAR szAddress[64] = "";
            _ui64toa(*(DWORD64*)pAddress, szAddress, 16);

            nLength = _sntprintf(szBuffer_Out, nBufferSize, " = 0x%s", szAddress);
		}
    }

	CHECK_LENGTH_RETURN(nLength, nTotalLength, -1);
    return nTotalLength;
}


BasicType CExceptionReport::GetBasicType(unsigned long typeIndex, DWORD64 modBase)
{
	CDBGFuncClass& dbgFuncClass = CExceptionReport::GetInstance().GetDBGFuncClass();

    BasicType basicType;

	if(!dbgFuncClass.SymGetTypeInfo)
		return btNoType;

    if(dbgFuncClass.SymGetTypeInfo(GetCurrentProcess(), 
		modBase, typeIndex, TI_GET_BASETYPE, &basicType))
	{
        return basicType;
    }

    // Get the real "TypeId" of the child.  We need this for the
    // SymGetTypeInfo( TI_GET_TYPEID ) call below.
    unsigned long typeId = 0;

    if(dbgFuncClass.SymGetTypeInfo(GetCurrentProcess(), 
		modBase, typeIndex, TI_GET_TYPEID, &typeId)) 
	{
        if(dbgFuncClass.SymGetTypeInfo( GetCurrentProcess(), 
			modBase, typeId, TI_GET_BASETYPE, &basicType))
		{
            return basicType;
        }
    }

    return btNoType;
}


//
// Memory Dump function
//
int CExceptionReport::Dump(TCHAR* szBuffer_Out, const int nBufferSize, 
						   DWORD64 dw64Offset, unsigned long dwSize, BOOL bAlign )
{
	int nTotalLength = 0;
	int nLength = 0;

	__try
	{
		unsigned long dwLoc = 0, dwILoc = 0, dwX = 0;
		
		LPBYTE pOut = (LPBYTE) dw64Offset;
		
		if(bAlign == TRUE)
		{
			pOut = (LPBYTE) ((dw64Offset >> SHIFT_NUM) << SHIFT_NUM);
		}

		for( dwLoc = 0; dwLoc < dwSize; dwLoc += 16, pOut += BYTES_PER_LINE ) 
		{
			LPBYTE pLine = pOut;
			
			nLength = _sntprintf(szBuffer_Out + nTotalLength, 
				nBufferSize - nTotalLength,
				_T( "%08lX: " ), (DWORD64) pOut);

			CHECK_LENGTH_RETURN(nLength, nTotalLength, -1);

			for(dwX = 0, dwILoc = dwLoc; dwX < BYTES_PER_LINE; dwX++)
			{
				if(dwX == (BYTES_PER_LINE / 2))
				{
					nLength = _sntprintf(szBuffer_Out + nTotalLength, 
						nBufferSize - nTotalLength, _T("   "));
				}
				if( dwILoc++ > dwSize ) 
				{
					nLength = _sntprintf(szBuffer_Out + nTotalLength, 
						nBufferSize - nTotalLength, _T("?? "));
				}
				else 
				{
					nLength = _sntprintf(szBuffer_Out + nTotalLength, 
						nBufferSize - nTotalLength, _T("%02X "), *(pLine++));
				}

				CHECK_LENGTH_RETURN(nLength, nTotalLength, -1);
			}

			pLine = pOut;

			nLength = _sntprintf(szBuffer_Out + nTotalLength, 
				nBufferSize - nTotalLength, _T(" "));
			CHECK_LENGTH_RETURN(nLength, nTotalLength, -1);

			for( dwX = 0, dwILoc = dwLoc; dwX < BYTES_PER_LINE; dwX++ )
			{
				if( dwILoc++ > dwSize )
				{
					nLength = _sntprintf(szBuffer_Out + nTotalLength, 
						nBufferSize - nTotalLength, _T(" "));				
				} 
				else
				{
					nLength = _sntprintf(szBuffer_Out + nTotalLength, 
						nBufferSize - nTotalLength, _T( "%c" ), isprint(*pLine) ? *pLine : '.');

					pLine++;
				}

				CHECK_LENGTH_RETURN(nLength, nTotalLength, -1);
			}

			nLength = _sntprintf(szBuffer_Out + nTotalLength, 
				nBufferSize - nTotalLength, _T("\r\n"));

			CHECK_LENGTH_RETURN(nLength, nTotalLength, -1);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		nLength = _sntprintf(szBuffer_Out + nTotalLength, nBufferSize - nTotalLength, 
			_T("\r\n Dump failed by Exception : %d - %s\r\n"),
			GetExceptionCode(), GetExceptionString(GetExceptionCode()));

		CHECK_LENGTH_RETURN(nLength, nTotalLength, -1);
	}

	return nTotalLength;
}

