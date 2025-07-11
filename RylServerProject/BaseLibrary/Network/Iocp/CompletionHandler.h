
#ifndef _CCOMPLETIONHANDLER_H_
#define _CCOMPLETIONHANDLER_H_

#include <winsock2.h>
#include <windows.h>

//-----------------------------------------------------------------------------
// CCompletionHandler Class
//-----------------------------------------------------------------------------
class CCompletionHandler 
{
private:
	
	HANDLE	m_hIOCP;
	unsigned long	m_nThread;
	unsigned long	m_nTimeOutMS;

public:

	CCompletionHandler();
	virtual ~CCompletionHandler();

	bool	Initialize(unsigned long nThread = 0, unsigned long nTimeOutMS = INFINITE);	// Completion Handler를 초기화한다.
	bool	Destroy();	// Completion Hander를 소멸시킨다.

	bool	AttachToHander(HANDLE hAttach, ULONG_PTR pCompletionKey);	// Completion Handler에 새 핸들을 추가한다.

	BOOL	GetHanderStatus(LPDWORD lpNumOfBytes, 
		PULONG_PTR lpCompletionKey, LPOVERLAPPED *lpOverlapped);	// 작업이 끝난 핸들을 얻어 온다. 그동안은 block된다.

	BOOL	PostToHandler(unsigned long dwNumOfBytesTransfered, 
		ULONG_PTR dwCompletionKey, LPOVERLAPPED lpOverlapped);	// Handler에 작업을 강제로 추가한다.
};

//-----------------------------------------------------------------------------
// Inline Implementation ------------------------------------------------------

inline bool CCompletionHandler::AttachToHander(HANDLE hAttach, ULONG_PTR pCompletionKey)
{
	return (0 != CreateIoCompletionPort(hAttach, m_hIOCP, pCompletionKey, m_nThread));
}


inline BOOL CCompletionHandler::GetHanderStatus(LPDWORD lpNumOfBytes, 
										 PULONG_PTR lpCompletionKey, LPOVERLAPPED *lpOverlapped)
{
	return GetQueuedCompletionStatus(m_hIOCP, lpNumOfBytes, lpCompletionKey, lpOverlapped, m_nTimeOutMS);
}


inline BOOL CCompletionHandler::PostToHandler(unsigned long dwNumOfBytesTransfered, 
									  ULONG_PTR dwCompletionKey, LPOVERLAPPED lpOverlapped)
{
	return PostQueuedCompletionStatus(m_hIOCP, dwNumOfBytesTransfered, dwCompletionKey, lpOverlapped);
}


#endif