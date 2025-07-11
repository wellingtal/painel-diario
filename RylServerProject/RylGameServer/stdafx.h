// stdafx.h : �� ������� �ʰ� ���� ����ϴ�
// ǥ�� �ý��� ���� ���� �� ������Ʈ ���� ���� ������ 
// ��� �ִ� ���� �����Դϴ�.
//

#pragma once

#define WIN32_LEAN_AND_MEAN		// ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
#pragma warning(disable:4800)

#include <winsock2.h>
#include <atltime.h>        // �̰� ����� �ȳ�����, time ���� �Լ� ȣ��� �����忡�� ������.

// Windows ��� �����Դϴ�.
#include <windows.h>
// C�� ��Ÿ�� ��� �����Դϴ�.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// TODO: ���α׷��� �ʿ��� �߰� ����� ���⿡�� �����մϴ�.
#ifdef _DEBUG
	#define PERFORMANCE_CHECK(x) x
#else 
	#define PERFORMANCE_CHECK(x) x
#endif

// ��Ÿ
#include <Log/ServerLog.h>
#include <Utility/Debug/DebugMacros.h>
#include <Utility/Debug/ExceptionReport.h>
#include <Utility/Debug/PerformanceCheck.h>