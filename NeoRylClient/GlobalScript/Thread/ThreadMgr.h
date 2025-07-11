#ifndef _CTHREADMGR_H_
#define _CTHREADMGR_H_

#ifndef WINDOWS_LEAN_AND_MEAN
#define WINDOWS_LEAN_AND_MEAN
#endif

#include <winsock2.h>
#include <windows.h>
#include <process.h>

#include "Lock.h"

// forward decl
class CThread;

class CThreadMgr
{
public:

	enum { MAX_THREAD_NUM = 63 };

	CThreadMgr();
	virtual ~CThreadMgr();
	
	bool RegisterAndRun(CThread* lpThread);	// �����带 ����Ѵ�. �ִ� 63������ ����� �� �ִ�.
	bool JoinThread();						// ��� �����带 �����Ų��.
	
	// �Ŵ����� ������� �ʰ�, �׳� ���� ��Ų��.
	static HANDLE	Run(CThread* lpThread);
	static bool		Stop(CThread* lpThread, unsigned long dwTimeout = INFINITE);

private:

	typedef CCSLock	ThreadLock;	

	CThread*		m_lpThreads[MAX_THREAD_NUM];
	HANDLE			m_hThreads[MAX_THREAD_NUM];
	
	ThreadLock		m_ThreadLock;

	unsigned int	m_nThreadNum;
	unsigned int	m_bJoinStarted;
};

#endif