#include "stdafx.h"
#include <windows.h>
#include <crtdbg.h>
#include <malloc.h> 
#include <time.h>
#include <tlhelp32.h>
#include <excpt.h>

#pragma comment (lib, "dbghelp")

#include "DbgHelp.h"
#include "Exception.h"
#include <sstream>

#pragma warning(push)
#pragma warning(disable:4311)
#pragma warning(disable:4312)
#pragma warning(disable:4127)
#pragma warning(disable:4535) // must be compiled with: /EHa

/*
// �⺻ �����޽��� �ڽ�â�� ����� �ʰ� �ϱ� ����â.
SetErrorMode(UINT uMode)

SEM_FAILCRITICALERRORS
���������α׷� ��������� �ϴ� ȸ��â(-_-)�� �������� �ʴ´�.

SEM_NOGPFAULTERRORBOX
NOGPFAULT ����. NO General Protection FAULT �� �ٿ��������̴�. �Ϲ� ��ȣ������ �߻��ص� ȸ��â�� �������� �ʴ´�.

SEM_NOOPENFILEERRORBOX
Ȥ�� ������ �ִ��� �𸣰�����, ������ ������ �� ���� ������ �����ϸ� ������ �߻��Ѵ�. ȸ��������(-_-) ���������α׷� ������ ���� â�� �ƴ� �޽��� �ڽ��� ���(���� ���Ͽ� �������� �� �����ϴ�.�������? �� ����� �޽����� ������� �� ����). �װ� �Ⱥ��̰� ���ش�.
*/


/*
	Release ��忡���� ���ܰ� �۵����� �ʴ´�.
	���ܸ� �۵��ϱ� ���ؼ� �ɼ��� ����ȭ ��带 /Od �� ������ �Ѵ�.
	/Od�� �ƴ� ����Ʈ�� /O2 ������ ���ܰ� ������ ����� ã��.
*/

Exception::MiniDumpTypes Exception::MiniDumpType = Exception::NormalDump;
bool Exception::ExceptionSaveDump = false;
char Exception::DefaultDumpFileName[512]="";
LPTOP_LEVEL_EXCEPTION_FILTER Exception::lpPreviousFilter = NULL;


bool IsNT()
{	
	OSVERSIONINFO vi = { sizeof(vi)};	
	::GetVersionEx(&vi);
	return vi.dwPlatformId == VER_PLATFORM_WIN32_NT;
}

HANDLE SymGetProcessHandle()
{
	if (IsNT())
		return GetCurrentProcess();
	else
		return (HANDLE)GetCurrentProcessId();
}

void MiniDump(const char* filename, PEXCEPTION_POINTERS ep)
{
	HANDLE process=SymGetProcessHandle();
	DWORD processId=GetCurrentProcessId();

	MINIDUMP_EXCEPTION_INFORMATION ei;
	ei.ClientPointers=FALSE;
	ei.ThreadId=GetCurrentThreadId();
	ei.ExceptionPointers=ep;

	HANDLE file=CreateFile(filename, GENERIC_WRITE, FILE_SHARE_READ
		, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file==INVALID_HANDLE_VALUE) return;

	_MINIDUMP_TYPE type=MiniDumpNormal;
	switch(Exception::MiniDumpType)
	{
		case Exception::HeapDump: type=MiniDumpWithFullMemory; break;
	};

	MiniDumpWriteDump(process, processId, file, type, &ei, NULL, NULL);

	CloseHandle(file);

}

char* GetExceptionString(DWORD Code)
{
	switch(Code)
	{
	case EXCEPTION_ACCESS_VIOLATION:		 return "EXCEPTION_ACCESS_VIOLATION";
	case EXCEPTION_BREAKPOINT:				 return "EXCEPTION_BREAKPOINT";
	case EXCEPTION_DATATYPE_MISALIGNMENT:	 return "EXCEPTION_DATATYPE_MISALIGNMENT";
	case EXCEPTION_SINGLE_STEP:				 return "EXCEPTION_SINGLE_STEP";
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:	 return "EXCEPTION_ARRAY_BOUNDS_EXCEEDED";
	case EXCEPTION_FLT_DENORMAL_OPERAND:	 return "EXCEPTION_FLT_DENORMAL_OPERAND";
	case EXCEPTION_FLT_DIVIDE_BY_ZERO:		 return "EXCEPTION_FLT_DIVIDE_BY_ZERO";
	case EXCEPTION_FLT_INEXACT_RESULT:		 return "EXCEPTION_FLT_INEXACT_RESULT";
	case EXCEPTION_FLT_INVALID_OPERATION:	 return "EXCEPTION_FLT_INVALID_OPERATION";
	case EXCEPTION_FLT_OVERFLOW:			 return "EXCEPTION_FLT_OVERFLOW";
	case EXCEPTION_FLT_STACK_CHECK:			 return "EXCEPTION_FLT_STACK_CHECK";
	case EXCEPTION_FLT_UNDERFLOW:			 return "EXCEPTION_FLT_UNDERFLOW";
	case EXCEPTION_INT_DIVIDE_BY_ZERO:		 return "EXCEPTION_INT_DIVIDE_BY_ZERO";
	case EXCEPTION_INT_OVERFLOW:			 return "EXCEPTION_INT_OVERFLOW";
	case EXCEPTION_PRIV_INSTRUCTION:		 return "EXCEPTION_PRIV_INSTRUCTION";
	case EXCEPTION_NONCONTINUABLE_EXCEPTION: return "EXCEPTION_NONCONTINUABLE_EXCEPTION";
	case EXCEPTION_STACK_OVERFLOW:			 return "EXCEPTION_STACK_OVERFLOW";
	}
	return "INVALID";
}

LONG __stdcall UserUnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo) 
{
	if(Exception::ExceptionSaveDump)
	{
		// ���ܸ� ȣ���ϱ� ���� �̴� ���������� �����Ѵ�.
		char dumpfile[512];

		if(strlen(Exception::DefaultDumpFileName) == 0)
		{
			SYSTEMTIME	st;
			GetLocalTime(&st);

			sprintf(dumpfile, ".\\Errors\\%04d%02d%02d-%02d%02d%02d.dmp",
				st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		}
		else 
			strcpy(dumpfile, Exception::DefaultDumpFileName);

		// �� ���� ������ƾ�� Execption ���� �ɼ����� �����Ҽ� �ֵ��� ����.
		// �̴ϴ��� ����.
		MiniDump(dumpfile, pExceptionInfo);
	}

	MessageBox(GetActiveWindow(), "Ryl Tides of War ERR::655.", "Client Error", MB_OK);//was ROW hayzohar

	return EXCEPTION_EXECUTE_HANDLER;
}

void Exception::SetDumpType(MiniDumpTypes type)
{
	Exception::MiniDumpType=type;
}

void Exception::EnableSaveDump(bool enable)
{
	Exception::ExceptionSaveDump = enable;
}

void Exception::EnableUnhandledExceptioinFilterEx(LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter, bool enable)
{
	if(lpPreviousFilter)
	{
		::SetUnhandledExceptionFilter(lpPreviousFilter);
		lpPreviousFilter = NULL;
	}

	// try~catch �� ���� ���� ���� ���ܿ����� �˻��ϱ� ���� 
	// ����ó����ƾ�̴�.
	if(enable)
		lpPreviousFilter = ::SetUnhandledExceptionFilter(lpTopLevelExceptionFilter);
	else
	{
		::SetUnhandledExceptionFilter(lpPreviousFilter);
		lpPreviousFilter = NULL;
	}
}

void Exception::EnableUnhandledExceptioinFilter(bool enable)
{
	if(lpPreviousFilter)
	{
		::SetUnhandledExceptionFilter(lpPreviousFilter);
		lpPreviousFilter = NULL;
	}

	// try~catch �� ���� ���� ���� ���ܿ����� �˻��ϱ� ���� 
	// ����ó����ƾ�̴�.
	if(enable)
		lpPreviousFilter = ::SetUnhandledExceptionFilter(UserUnhandledExceptionFilter);
	else
	{
		::SetUnhandledExceptionFilter(lpPreviousFilter);
		lpPreviousFilter = NULL;
	}
}

// �̹���� ������ ���������� ����� �ִ�.
void Exception::MiniDump(const char* filename)
{
	HANDLE process=SymGetProcessHandle();
	DWORD processId=GetCurrentProcessId();
/*		MINIDUMP_EXCEPTION_INFORMATION ei;
	PEXCEPTION_POINTERS ep = GetExceptionInformation();
	ei.ClientPointers=false;
	ei.ThreadId=GetCurrentThreadId();
	ei.ExceptionPointers=ep;
*/
	HANDLE file=CreateFile(filename, GENERIC_WRITE, FILE_SHARE_READ
		, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file==INVALID_HANDLE_VALUE) return;

	_MINIDUMP_TYPE type=MiniDumpNormal;
	switch(Exception::MiniDumpType)
	{
//		case Exception::DataSegs: type=MiniDumpWithDataSegs; break;
		case Exception::HeapDump: type=MiniDumpWithFullMemory; break;
	};

	MiniDumpWriteDump(process, processId, file, type, NULL, NULL, NULL);
	CloseHandle(file);
}

void Exception::set(const char* message) 
{
	strcpy(buffer, message);
}

Exception::Exception(const char* message) {
	set(message);
}

Exception::Exception(const std::string& message) {
	set(message.c_str());
}

const char* Exception::What() const {
	return buffer;
}

Exception::~Exception(void)
{
}

#pragma warning(pop)
