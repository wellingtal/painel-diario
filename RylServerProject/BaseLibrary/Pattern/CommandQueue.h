#ifndef _SYNCRONIZED_COMMAND_QUEUE_
#define _SYNCRONIZED_COMMAND_QUEUE_

#include <winsock2.h>
#include <windows.h>
#include <Thread/Lock.h>
#include <Thread/Thread.h>

#include <list>

// forward decl.
class CCommand;


// 메시지 큐 스레드.
class CCommandQueueThread : public CThread
{
public:

	CCommandQueueThread(long nMaxQueueSize = LONG_MAX);
	virtual ~CCommandQueueThread();

	bool Add(CCommand* lpCommand);
	bool IsValid();

protected:

	enum Const
	{
		StopperIndex, 
		SemaphoreIndex, 
		MaxIndex
	};

private:

	virtual unsigned int Run();
	virtual BOOL End();

	typedef CCSLock					MsgQueueLock;
	typedef std::list<CCommand*>	CommandList;

	HANDLE			m_hHandles[MaxIndex];
	MsgQueueLock	m_Lock;
	CACHE_PAD(MsgQueueLockPad, sizeof(CCSLock));

	CommandList		m_CommandList;
};

#endif