// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#pragma warning(disable:4800)

// ���� ���˿� ���ǵ�. ������� ���� ���� (void*)0 �� ����� ��.
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
// ���� ���� �����
#include <winsock2.h>
#include <mswsock.h>
#include <iphlpapi.h>	// for IP Help functions, that gets local IP address
#include <ws2tcpip.h>   // tcp/ip specific options
#include <wsipx.h>		// for IPX/SPX
#include <wsnwlink.h>

// DB���� �����
#include <msdasc.h>				// OLE DB Service Component header
#include <msdaguid.h>			// OLE DB Root Enumerator 
#include <msdasql.h>			// MSDASQL - Default provider
#include <sqloledb.h>			// MS SQL

// �⺻ �����
#include <windows.h>
#include <process.h>
#include <shellapi.h>
#include <tchar.h>

// C �Լ� �����
#include <cassert>
#include <ctime>
#include <cmath>
#include <cstdio>
#include <cctype>
#include <cstring>
#include <cstdarg>
#include <cstddef>
#include <cstdlib>

// STL �����
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