#ifndef _CLOCK_H_
#define _CLOCK_H_

#include <winsock2.h>
#include <windows.h>


// 다중 CPU환경에서는 Cache-Line에 대한 고려를 해야만 할 것.
#define CACHE_ALIGN		32
#define CACHE_PAD(Name, BytesSoFar) \
		BYTE Name[CACHE_ALIGN - ((BytesSoFar) % CACHE_ALIGN)]


template<class DerivedLockClass>
class CLock	
{
public:
	class Syncronize
	{
	public:
		Syncronize(DerivedLockClass* pLockClass) : m_Lock(*pLockClass) { m_Lock.Lock(); }
		Syncronize(DerivedLockClass& LockClass)  : m_Lock(LockClass) { m_Lock.Lock(); }
		~Syncronize() { m_Lock.Unlock();}
		
	protected:
		DerivedLockClass&	m_Lock;
	};
};


// 이 Lock클래스들은 더이상 상속을 받아서는 안 된다.

// ----------------------------------------------------------------------------
// DummyLock ------------------------------------------------------------------
class CDummyLock : public CLock<CDummyLock>
{
public:
	void Lock()   { }
	void Unlock() { }
	bool IsVaildLock() { return true; }
};


// ----------------------------------------------------------------------------
// SpinLock -------------------------------------------------------------------
class CSpinLock : public CLock<CSpinLock>
{
public:
	CSpinLock() : m_bLocked(FALSE) { }
	~CSpinLock() { }
	void Lock()   { while(TRUE == InterlockedExchange(&m_bLocked, TRUE)) { Sleep(0); } }
	void Unlock() { InterlockedExchange(&m_bLocked, FALSE); }
	
	bool IsVaildLock() { return true; }

private:
	volatile LONG m_bLocked;
};



// ----------------------------------------------------------------------------
// Critical Section -----------------------------------------------------------

class CCSLock : public CLock<CCSLock>
{
public:
	CCSLock()     { InitializeCriticalSection(&m_CSLock); }
	~CCSLock()    { DeleteCriticalSection(&m_CSLock);     }
	void Lock()   { EnterCriticalSection(&m_CSLock);      }
	void Unlock() { LeaveCriticalSection(&m_CSLock);      }
	bool IsVaildLock() { return true; }

private:
	CRITICAL_SECTION	m_CSLock;
};


// ----------------------------------------------------------------------------
// Mutex Lock -----------------------------------------------------------------

class CMutex : public CLock<CMutex>
{
public:
	CMutex() : m_hMutex(CreateMutex(NULL, FALSE, NULL)) { }
	~CMutex()      { if(0 != m_hMutex) { CloseHandle(m_hMutex); }    }
	unsigned long Lock()   { return WaitForSingleObject(m_hMutex, INFINITE); }
	unsigned long Unlock() { return ReleaseMutex(m_hMutex); }
	bool IsVaildLock() { return (NULL != m_hMutex); }

private:
	HANDLE	m_hMutex;
};



class CNamedMutex : public CLock<CNamedMutex>
{
public:
	CNamedMutex(const TCHAR* szMutexName, BOOL bInitialOwner)
		:	m_hMutex(CreateMutex(NULL, bInitialOwner, szMutexName)) { }
	~CNamedMutex() { if(0 != m_hMutex) { CloseHandle(m_hMutex); } }
	unsigned long Lock()   { return WaitForSingleObject(m_hMutex, INFINITE); }
	unsigned long Unlock() { return ReleaseMutex(m_hMutex); }
	bool IsVaildLock() { return (NULL != m_hMutex); }

private:
	HANDLE m_hMutex;
};


/*
class CFairRWLock : private CCSLock
{
private:
  Semaphore     access_lock;     // used as a one-at-a-time release valve
  CCSLock       read_barrier;    // used to block/wakeup readers
  unsigned int  is_write_lock;   // nonzero if locked for writing
  unsigned int  writer_count;    // # of writers waiting for or holding the lock
  unsigned int  reader_count;    // # of readers holding the lock
  unsigned int  readers_waiting; // # of readers waiting for the lock

public:
  ReadLock()
      : access_lock(1), is_write_lock(0), 
        writers_waiting(0), reader_count(0), readers_waiting(0)
  {
    
  }

  void ReadLock() 
  {
    Mutex::Lock();

    // if there is at least one writer using the lock or waiting for it,
    // we need to wait for access
    if (writer_count > 0)) 
    {
      if (readers_waiting++ == 0) // if we're the first reader in line
        Mutex::Unlock();
        access_lock.Wait();       // get the access lock
        Mutex::Lock();
        if (readers_waiting > 1)  // then if there are other readers
          read_barrier.Open();    // let them go
      }
      else 
      {
        Mutex::Unlock();
        read_barrier.Wait();      // otherwise wait until someone lets us go
        Mutex::Lock();
      }
      readers_waiting--;
    }
    reader_count++;
    Mutex::Unlock();
  }

  void WriteLock() 
  {
    Mutex::Lock();
    writer_count++;       // one more writer
    Mutex::Unlock();
    access_lock.Wait();   // wait until the access lock is available
    Mutex::Lock();
    is_write_lock = 1;    // lock is a write lock
    read_barrier.Close(); // give readers something to wait for
    Mutex::Unlock();
  }

  void Unlock() 
  {
    Mutex::Lock();
    if (is_write_lock)
    {          // if this is a write lock
      is_write_lock = 0;          // let it go
      writer_count--;             // one less writer
      access_lock.Post();         // now let someone else have a chance
    }
    else if (--reader_count == 0)
    {
        // if we're the last reader
        access_lock.Post();         // release the access lock
    }
    Mutex::Unlock();
  }
};
*/



#endif