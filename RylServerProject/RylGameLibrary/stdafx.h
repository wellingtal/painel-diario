// stdafx.h : 잘 변경되지 않고 자주 사용하는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이 
// 들어 있는 포함 파일입니다.
//

#pragma once

#pragma warning(disable:4800)

#define WIN32_LEAN_AND_MEAN		// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
#include <crtdbg.h>
#define _CRTDBG_MAP_ALLOC


// 각종 점검용 정의들. 사용하지 않을 경우는 (void*)0 로 대신할 것.
#ifdef _DEBUG
	#define PERFORMANCE_CHECK(x) x
#else 
	#define PERFORMANCE_CHECK(x) x
#endif

#ifdef _DEBUG
	#define DEBUG_CRT_MEMORY(x) x
#else
	#define DEBUG_CRT_MEMORY(x) (void*)0
#endif


// 로그
#ifdef _DEBUG
	#define LOG_INOUT(x)	x
#else
	#define LOG_INOUT(x)	(void*)0
#endif

/*
// 소켓 관련 헤더들
#include <winsock2.h>
#include <mswsock.h>
#include <iphlpapi.h>	// for IP Help functions, that gets local IP address
#include <ws2tcpip.h>   // tcp/ip specific options
#include <wsipx.h>		// for IPX/SPX
#include <wsnwlink.h>

// DB관련 헤더들
#include <msdasc.h>		// OLE DB Service Component header
#include <msdaguid.h>	// OLE DB Root Enumerator 
#include <msdasql.h>	// MSDASQL - Default provider
#include <sqloledb.h>	// MS SQL

// 기본 헤더들
#include <windows.h>
#include <process.h>
#include <shellapi.h>
#include <tchar.h>

// C 함수 헤더들
#include <cassert>
#include <ctime>
#include <cmath>
#include <cstdio>
#include <cctype>
#include <cstring>
#include <cstdarg>
#include <cstddef>
#include <cstdlib>
*/

#include <atltime.h>	// CTime

// STL 헤더들
#include <new>
#include <set>
#include <map>
#include <hash_map>
#include <vector>
#include <list>
#include <string>
#include <limits>
#include <bitset>
#include <complex>

#include <algorithm>
#include <numeric>
#include <utility>
#include <functional>

// 기타
#include <Log/ServerLog.h>
#include <Utility/Debug/DebugMacros.h>
#include <Utility/Debug/ExceptionReport.h>
#include <Utility/Debug/PerformanceCheck.h>
