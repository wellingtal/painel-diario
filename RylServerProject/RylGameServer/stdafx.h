// stdafx.h : 잘 변경되지 않고 자주 사용하는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이 
// 들어 있는 포함 파일입니다.
//

#pragma once

#define WIN32_LEAN_AND_MEAN		// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#pragma warning(disable:4800)

#include <winsock2.h>
#include <atltime.h>        // 이거 여기다 안넣으면, time 관련 함수 호출시 스레드에서 에러남.

// Windows 헤더 파일입니다.
#include <windows.h>
// C의 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
#ifdef _DEBUG
	#define PERFORMANCE_CHECK(x) x
#else 
	#define PERFORMANCE_CHECK(x) x
#endif

// 기타
#include <Log/ServerLog.h>
#include <Utility/Debug/DebugMacros.h>
#include <Utility/Debug/ExceptionReport.h>
#include <Utility/Debug/PerformanceCheck.h>