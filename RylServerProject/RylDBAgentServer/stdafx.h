// stdafx.h : �� ������� �ʰ� ���� ����ϴ�
// ǥ�� �ý��� ���� ���� �� ������Ʈ ���� ���� ������ 
// ��� �ִ� ���� �����Դϴ�.
//

#pragma once

#define WIN32_LEAN_AND_MEAN		// ���� ������ �ʴ� ������ Windows ������� �����մϴ�.

// TODO: ���α׷��� �ʿ��� �߰� ����� ���⿡�� �����մϴ�.

#pragma warning(disable:4800)

#include <winsock2.h>
#include <windows.h>
#include <atltime.h>        // �̰� ����� �ȳ�����, time ���� �Լ� ȣ��� �����忡�� ������.

#ifdef _DEBUG
#define PERFORMANCE_CHECK(x) x
#else 
#define PERFORMANCE_CHECK(x) x
#endif

#include <boost/pool/pool.hpp>
#include <boost/pool/object_pool.hpp>
#include <boost/pool/pool_alloc.hpp>
#include <Network/Dispatch/PoolDispatchFactory.h>

#include <map>
#include <vector>
#include <list>
#include <malloc.h>

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
