#include "stdafx.h"
#include "MemoryPool.h"

#include <winsock2.h>
#include <windows.h>

#include <new>
#include <iostream>
#include <fstream>

const char* const CFixedPool::ms_strErrLogFileName = "FixedMemoryPool_ErrLog.txt";


struct CFixedPool::ChunkNode
{
	ChunkNode*			m_pNext;
};

struct CFixedPool::AllocateInfo
{
	AllocateInfo*		m_pNext;

	static const int	ms_nPaddingSize;
	static const char*	ms_strAllocatedPadding;
	static const char*	ms_strDeallocatedPadding;
	static const char*	ms_strNotUsedPadding;
};

const char*	CFixedPool::AllocateInfo::ms_strAllocatedPadding	= "Aloc";
const char*	CFixedPool::AllocateInfo::ms_strDeallocatedPadding	= "Free";
const char* CFixedPool::AllocateInfo::ms_strNotUsedPadding		= "Nuse";


#ifdef MEMPOOL_DBGMODE
const int CFixedPool::AllocateInfo::ms_nPaddingSize	= 4;
#else
const int CFixedPool::AllocateInfo::ms_nPaddingSize	= 0;
#endif



DEFINE_ALLOCATOR(CFixedPool)
{
	if(nSize != m_nOriginalSize)
	{
		return operator new(nSize);
	}

	m_PoolLock.Lock();

	if(NULL == m_pNodeHead)
	{
		if(!AllocateChunks())
		{
			return NULL;
		}
	}

	AllocateInfo* pNode = m_pNodeHead;
	m_pNodeHead = m_pNodeHead->m_pNext;	
	++m_nTotalInUse;

	m_PoolLock.Unlock();

	char* pResult = reinterpret_cast<char*>(pNode);

#ifdef MEMPOOL_DBGMODE
	memcpy(pResult + m_nPerAllocateSize - AllocateInfo::ms_nPaddingSize,
		AllocateInfo::ms_strAllocatedPadding, AllocateInfo::ms_nPaddingSize);
#endif

	return pResult + sizeof(AllocateInfo);
}


DEFINE_DEALLOCATOR(CFixedPool)
{
	if(NULL == pDelete)
	{
		return;
	}

	if(nSize != m_nOriginalSize)
	{
		operator delete(pDelete);
		return;
	}

	char* pszDelete = reinterpret_cast<char*>(pDelete) - sizeof(AllocateInfo);
	AllocateInfo* pNode = reinterpret_cast<AllocateInfo*>(pszDelete);

#ifdef MEMPOOL_DBGMODE

	//	버퍼 오버플로우 혹은 메모리를 여러번 해제.
	const char* pszPadding = pszDelete + m_nPerAllocateSize - AllocateInfo::ms_nPaddingSize;
	if(0 != memcmp(pszPadding, AllocateInfo::ms_strAllocatedPadding, AllocateInfo::ms_nPaddingSize))
	{
        LogBufferOverflow(pszDelete);
	}
	
#endif
	
	m_PoolLock.Lock();

    pNode->m_pNext = m_pNodeHead;
	m_pNodeHead = pNode;
	--m_nTotalInUse;

	m_PoolLock.Unlock();
}



bool CFixedPool::AllocateChunks()
{
	// Allocation and make list
	ChunkNode* pChunkNode = reinterpret_cast<ChunkNode*>(
		::operator new(sizeof(ChunkNode) + m_nPerAllocateSize * m_nPerAllocateNum));

	if(0 == pChunkNode)
	{
		// Chunk 메모리 할당 에러.
		return false;
	}

	pChunkNode->m_pNext = m_pChunkHead;
	m_pChunkHead = pChunkNode;	
	m_pNodeHead = reinterpret_cast<AllocateInfo*>(m_pChunkHead + 1);

	m_nTotalAllocated += m_nPerAllocateNum;		// 전체 개수 증가.
	
	char*  pMakeList = reinterpret_cast<char*>(m_pNodeHead);
	size_t nAllocated = m_nPerAllocateNum - 1;

	while(0 < nAllocated--)
	{
		reinterpret_cast<AllocateInfo*>(pMakeList)->m_pNext = 
			reinterpret_cast<AllocateInfo*>(pMakeList + m_nPerAllocateSize);
		pMakeList += m_nPerAllocateSize;

#ifdef MEMPOOL_DBGMODE
		//	메모리가 사용되지 않았음을 나타내는 패딩을 넣음.
		memcpy(pMakeList - AllocateInfo::ms_nPaddingSize,
			AllocateInfo::ms_strNotUsedPadding, AllocateInfo::ms_nPaddingSize);
#endif
	}

	reinterpret_cast<AllocateInfo*>(pMakeList)->m_pNext = 0;
	return true;
}



CFixedPool::CFixedPool()
:	m_pChunkHead(NULL), m_pNodeHead(NULL),
	m_nOriginalSize(0), m_nPerAllocateNum(0), m_nPerAllocateSize(0),
	m_nTotalAllocated(0), m_nTotalInUse(0)
{


}

CFixedPool::CFixedPool(size_t nPerAllocateSize, size_t nPerAllocateNum, 
					   const char* strMaxPoolName, size_t nNameLen)
                       :    m_pChunkHead(NULL), m_pNodeHead(NULL),
                            m_nOriginalSize(0), m_nPerAllocateNum(0), m_nPerAllocateSize(0),
                            m_nTotalAllocated(0), m_nTotalInUse(0)
{
	Initialize(nPerAllocateSize, nPerAllocateNum, strMaxPoolName, nNameLen);
}


CFixedPool::~CFixedPool()
{
	Destroy();
}

bool CFixedPool::Initialize(size_t nPerAllocateSize, size_t nPerAllocateNum, 
							const char* strMaxPoolName, size_t nNameLen)
{
	Destroy();

	m_nOriginalSize    = nPerAllocateSize;
	m_nPerAllocateNum  = nPerAllocateNum;
	m_nPerAllocateSize = nPerAllocateSize + GetLeastAllocateSize();        

    const size_t nLastPos = min(nNameLen, MAX_POOL_NAME - 1);
	strncpy(m_pszPoolName, strMaxPoolName, nLastPos);
	m_pszPoolName[nLastPos] = 0;
	return true;
}


void CFixedPool::LogBufferOverflow(const char* lpBuffer)
{
	FILE* pLogFile = fopen(ms_strErrLogFileName, "ab");
	if(NULL != pLogFile)
	{
        SYSTEMTIME sysTime;
        GetLocalTime(&sysTime);

        __try
        {
            fprintf(pLogFile, "\n[%04d-%02d-%02d %02d:%02d:%02d] %s 메모리 풀에서 버퍼 오버플로우 또는, "
                "메모리 해제가 여러 번 있었습니다. PerAllocateSize = %d(0x%08x)\n<",
                sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond, m_pszPoolName,
                m_nPerAllocateSize, m_nPerAllocateSize);

            fwrite(lpBuffer, m_nPerAllocateSize * 2, 1, pLogFile);
        }
        __finally
        {
            fprintf(pLogFile, ">");
    	    fclose(pLogFile);
        }
	}
}


void CFixedPool::Destroy()
{
	PoolLock::Syncronize sync(m_PoolLock);

	if(0 != m_nTotalInUse)
	{
		//	메모리 관련 로그를 남긴다. (Chunk를 전부 뒤져서 해제가 되지 않은 메모리를 찾아낸다.)
		FILE* pLogFile = fopen(ms_strErrLogFileName, "ab");		
		if(NULL != pLogFile)
		{
            SYSTEMTIME sysTime;
            GetLocalTime(&sysTime);
		
            fprintf(pLogFile, "[%d-%d-%d %d:%d:%d] %s 메모리 풀에서 %d개가 해제되지 않았습니다. 로그로 기록합니다.\r\n",
                sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond,
                m_pszPoolName, m_nTotalInUse);

			fclose(pLogFile);
		}	
	}

	for(ChunkNode* pChunkNode = m_pChunkHead; NULL != pChunkNode;)
	{
		ChunkNode* pDelNode = pChunkNode;
		pChunkNode = pChunkNode->m_pNext;
		delete pDelNode;
	}

	m_nTotalAllocated = m_nTotalInUse = 0;
	m_pChunkHead = NULL;
	m_pNodeHead  = NULL;
}


size_t CFixedPool::GetLeastAllocateSize()
{
	return sizeof(AllocateInfo) + AllocateInfo::ms_nPaddingSize;
}

const char* CFixedPool::GetErrFileName()
{
	return ms_strErrLogFileName;
}



void CFixedPoolTest::DoTest()
{
	const char* const strErrLogFileName = CFixedPool::GetErrFileName();

	printf("%s Test Started.\n", __FUNCTION__);
	remove(strErrLogFileName);

	CFixedPool	pool;

	const char* pName = "테스트풀";

	const int nSize = 10;
	const int nPerAllocate = 100;

	const int MAX_ALLOC = nPerAllocate * 10;

	void* pAlloc[MAX_ALLOC];
	pool.Initialize(nSize, nPerAllocate, pName, strlen(pName));


	for(int i = 0; i < MAX_ALLOC; ++i)
	{
        pAlloc[i] = pool.ALLOC(nSize);
	}

	for(int i = 0; i < MAX_ALLOC; ++i)
	{
		pool.FREE(pAlloc[i], nSize);
	}

	pool.Destroy();

	FILE* pFile = fopen(strErrLogFileName, "rb");
	if(NULL == pFile)
	{
		printf("\tTest Successed!\n");
	}
	else
	{
		fclose(pFile);

#ifdef WIN32
		char szPath[MAX_PATH];
		char szFileNameWithPath[MAX_PATH];

		UINT nResult = GetWindowsDirectory(szPath, MAX_PATH);
		if(0 != nResult && nResult <= MAX_PATH)
		{
			_snprintf(szFileNameWithPath, MAX_PATH, "%s\\NotePad.exe %s", szPath, strErrLogFileName);
			WinExec(szFileNameWithPath, SW_SHOW);
		}
#endif

		printf("\tMemory Leak or Curruption Detected!\n\tPlease See %s File\n", strErrLogFileName);
	}

	printf("%s Test Completed.\n\n", __FUNCTION__);
}



