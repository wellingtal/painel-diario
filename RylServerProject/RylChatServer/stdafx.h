// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#pragma warning(disable:4800)

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

// Windows Header Files:
#include <windows.h>
// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// TODO: reference additional headers your program requires here
// 소켓 관련 헤더들
#include <winsock2.h>
#include <mswsock.h>
#include <iphlpapi.h>	// for IP Help functions, that gets local IP address
#include <ws2tcpip.h>   // tcp/ip specific options
#include <wsipx.h>		// for IPX/SPX
#include <wsnwlink.h>

// DB관련 헤더들
#include <msdasc.h>				// OLE DB Service Component header
#include <msdaguid.h>			// OLE DB Root Enumerator 
#include <msdasql.h>			// MSDASQL - Default provider
#include <sqloledb.h>			// MS SQL

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