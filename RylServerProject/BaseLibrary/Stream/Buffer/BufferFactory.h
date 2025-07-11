#ifndef _CBUFFER_FACTORY_H_
#define _CBUFFER_FACTORY_H_

#include <vector>
#include <boost/pool/poolfwd.hpp>

#include "../../Thread/Lock.h"

//#define DEBUG_BUFFERFACTORY

#ifdef DEBUG_BUFFERFACTORY
    #define BUFFER_ARGUMENT __FUNCTION__, __FILE__, __LINE__, 
	#define CREATE_METHOD	CreateTracking
	#define RELEASE_METHOD  ReleaseTracking
#else
	#define BUFFER_ARGUMENT
	#define CREATE_METHOD	Create
	#define RELEASE_METHOD  Release
#endif

#define CREATE_BUFFER(instance, size)	(instance).CREATE_METHOD(BUFFER_ARGUMENT size)
#define SAFE_RELEASE_BUFFER(ptr)		if(ptr) { (ptr)->GetBufferFactory().RELEASE_METHOD(BUFFER_ARGUMENT (ptr)); (ptr) = 0; }


//	전방 참조
class CBuffer;

class CBufferFactory
{
public:

	virtual ~CBufferFactory() { }

	virtual CBuffer* Create(size_t size) = 0;
	virtual void Release(CBuffer* lpBuffer) = 0;

	CBuffer* CreateTracking(const char* szRoutine, const char* szFileName, const int nLine, size_t size);
	void ReleaseTracking(const char* szRoutine, const char* szFileName, const int nLine, CBuffer* lpBuffer);
};


class CDefaultBufferFactory : public CBufferFactory
{
public:

	virtual CBuffer* Create(size_t size);
	virtual void Release(CBuffer* lpBuffer);
};


class CPoolBufferFactory : public CBufferFactory
{
public:
    
    CPoolBufferFactory();
    virtual ~CPoolBufferFactory();
    
	virtual CBuffer* Create(size_t size);
	virtual void Release(CBuffer* lpBuffer);

	void Destroy();

protected:

	bool Initialize();
    
    typedef boost::pool<>               MemoryPool;

    typedef std::vector<MemoryPool*>    PoolArray;
	typedef CCSLock						BufferLock;

	BufferLock	m_BufferLock;
    MemoryPool* m_lpBufferPool;
    PoolArray   m_PoolArray;
};


class CBufferFactoryTest
{
public:
	void DoTest();
};

#endif