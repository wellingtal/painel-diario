#include "stdafx.h"
#include "Buffer.h"
#include "BufferFactory.h"

#include <boost/pool/pool.hpp>
#include <boost/pool/object_pool.hpp>

#include <Log/ServerLog.h>


CBuffer* CBufferFactory::CreateTracking(const char* szRoutine, const char* szFileName, const int nLine, size_t size)
{
	CBuffer* lpBuffer = Create(size);

	g_Log.DetailLog(LOG_DETAIL, szRoutine, szFileName, nLine, 
		"this:%p/BufferAllocate   : (0x%p/%d)", this, lpBuffer, size);

	return lpBuffer;
};

void CBufferFactory::ReleaseTracking(const char* szRoutine, const char* szFileName, const int nLine, CBuffer* lpBuffer)
{
	if(0 != lpBuffer)
	{
        g_Log.DetailLog(LOG_DETAIL, szRoutine, szFileName, nLine, 
			"this:%p/BufferDeallocate : (0x%p/%d)", this, lpBuffer, lpBuffer->capacity());

		Release(lpBuffer);
	}
}


CBuffer* CDefaultBufferFactory::Create(size_t size)
{
	char*	 szBuffer = new (std::nothrow) char[size];
	CBuffer* lpBuffer = new (std::nothrow) CBuffer(*this);

	if(0 != szBuffer && 0 != lpBuffer)
	{
        lpBuffer->init(szBuffer, size);
		return lpBuffer;
	}

	delete lpBuffer;
	delete [] szBuffer;
	return 0;
}

void CDefaultBufferFactory::Release(CBuffer* lpBuffer)
{
	if(0 != lpBuffer)
	{
		if(0 != lpBuffer->begin())
		{
            delete [] lpBuffer->begin();
		}

		delete lpBuffer;
	}
}



// ----------------------------------------------------------------------------------------
// PoolBufferFactory

CPoolBufferFactory::CPoolBufferFactory()
:	m_lpBufferPool(0)
{	
	Initialize();
}

CPoolBufferFactory::~CPoolBufferFactory()
{	
	Destroy();
}


bool CPoolBufferFactory::Initialize()
{
	Destroy();

	BufferLock::Syncronize sync(m_BufferLock);
	
    m_lpBufferPool = new MemoryPool(sizeof(CBuffer));

    if(0 == m_lpBufferPool)
    {
        return false;
    }

    const unsigned long MAX_SIZE_ARRAY = 7;
    unsigned long dwSizeArray[MAX_SIZE_ARRAY] = 
    {
        1024, 2048, 4096, 8192, 16384, 32768, 65535
    };

    for(unsigned long dwCount = 0; dwCount < MAX_SIZE_ARRAY; ++dwCount)
    {
        MemoryPool* lpMemoryPool = new MemoryPool(dwSizeArray[dwCount]);

        if(0 == lpMemoryPool)
        {
            return false;
        }

        m_PoolArray.push_back(lpMemoryPool);
    }

    return true;
}


CBuffer* CPoolBufferFactory::Create(size_t size)
{
	BufferLock::Syncronize sync(m_BufferLock);

	if(m_lpBufferPool == 0)
		return NULL;

	void*		ptr = m_lpBufferPool->malloc();
	CBuffer*	lpBuffer = 0;

    if(0 != ptr)
    {
		lpBuffer = new (ptr) CBuffer(*this);

        PoolArray::iterator pos = m_PoolArray.begin();
        PoolArray::iterator end = m_PoolArray.end();

        MemoryPool* lpMemoryPool = 0;
        char*       szBuffer = 0; 

        for(; pos != end; ++pos)
        {
            lpMemoryPool = (*pos);
            size_t requested_size = lpMemoryPool->get_requested_size();

            if(size <= requested_size)
            {
                szBuffer = reinterpret_cast<char*>(lpMemoryPool->malloc());
                break;
            }
        }

        if(0 == szBuffer)
        {
            szBuffer = new char[size];
        }

        if(0 != szBuffer)
        {
            lpBuffer->init(szBuffer, static_cast<unsigned long>(size));
        }
        else
        {
            m_lpBufferPool->free(lpBuffer);
            lpBuffer = 0;
        }
    }

    return lpBuffer;
}


void CPoolBufferFactory::Release(CBuffer* buffer)
{	    
    if(0 != buffer)
    {
		BufferLock::Syncronize sync(m_BufferLock);

        PoolArray::iterator pos = m_PoolArray.begin();
        PoolArray::iterator end = m_PoolArray.end();

        MemoryPool* lpMemoryPool    = 0;        
        size_t      buffer_size     = buffer->capacity();

        for(; pos != end; ++pos)
        {
            lpMemoryPool = (*pos);
            size_t requested_size = lpMemoryPool->get_requested_size();

            if(buffer_size <= requested_size)
            {
                lpMemoryPool->free(buffer->begin());
                break;
            }
        }

        if(pos == end)
        {
            delete [] buffer->begin();
        }

		buffer->~CBuffer();
        m_lpBufferPool->free(buffer);
    }
}

void CPoolBufferFactory::Destroy()
{
	BufferLock::Syncronize sync(m_BufferLock);

    delete m_lpBufferPool;
	m_lpBufferPool = 0;

    PoolArray::iterator pos = m_PoolArray.begin();
    PoolArray::iterator end = m_PoolArray.end();

    for(; pos != end; ++pos)
    {
        delete *pos;
    }

	m_PoolArray.clear();
}



//----------------------------------------------------------------------------------------


void CBufferFactoryTest::DoTest()
{
	#define RealClock(Large_Integer_In) \
		{ __asm rdtsc __asm mov Large_Integer_In.HighPart, edx __asm mov Large_Integer_In.LowPart, eax }

	const char* const strErrLogFileName = "./PoolErrorLog.txt";

	FILE* lpFile = freopen(strErrLogFileName, "wt", stderr);
	if(0 == lpFile)
	{
		return;
	}

	fprintf(stderr, "%s Test Started.\n", __FUNCTION__);
	    
    LARGE_INTEGER start, stop;
    double fTime = 0;

    CDefaultBufferFactory bufferFactory;

    const int MAX_LOOP = 10000;
  
    for(int nBytes = 36; nBytes < 60000; nBytes *= 2)
    {
        char* pcBuffer = new char[nBytes];
        for(int i = 0; i < nBytes; ++i)
        {
            pcBuffer[i] = (char)i;
        }
		
        CBuffer* pTemp = CREATE_BUFFER(bufferFactory, nBytes);

        fTime = 0;
        for(int i = 0; i < MAX_LOOP; ++i)
        {
            RealClock(start);
        
            CBuffer* pBuffer = CREATE_BUFFER(bufferFactory, nBytes);                
            
            memcpy(pBuffer->wr_ptr(), pcBuffer, nBytes);
            pBuffer->wr_ptr(nBytes);            

            SAFE_RELEASE_BUFFER(pBuffer);    
        
            RealClock(stop);

            fTime += (double)(stop.QuadPart - start.QuadPart);
        }
        fprintf(stderr, "Buffer속도 테스트. %dBytes : %f\n", nBytes, fTime/MAX_LOOP);
        
        fTime = 0;
        for(int i = 0; i < MAX_LOOP; ++i)
        {
            RealClock(start);

            char* pChar = new char[nBytes];
            memcpy(pChar, pcBuffer, nBytes);
            delete [] pChar;

            RealClock(stop);

            fTime += (double)(stop.QuadPart - start.QuadPart);
        }        
        fprintf(stderr, "NewAndDelete %dBytes : %f\n\n", nBytes, fTime/MAX_LOOP);

		SAFE_RELEASE_BUFFER(pTemp);
        delete [] pcBuffer;
    }

	fprintf(stderr, "%s Test Completed.\n\n", __FUNCTION__);

	fclose(lpFile);

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
}