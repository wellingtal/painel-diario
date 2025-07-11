
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

	bool	Initialize(unsigned long nThread = 0, unsigned long nTimeOutMS = INFINITE);	// Completion Handler�� �ʱ�ȭ�Ѵ�.
	bool	Destroy();	// Completion Hander�� �Ҹ��Ų��.

	bool	AttachToHander(HANDLE hAttach, ULONG_PTR pCompletionKey);	// Completion Handler�� �� �ڵ��� �߰��Ѵ�.

	BOOL	GetHanderStatus(LPDWORD lpNumOfBytes, 
		PULONG_PTR lpCompletionKey, LPOVERLAPPED *lpOverlapped);	// �۾��� ���� �ڵ��� ��� �´�. �׵����� block�ȴ�.

	BOOL	PostToHandler(unsigned long dwNumOfBytesTransfered, 
		ULONG_PTR dwCompletionKey, LPOVERLAPPED lpOverlapped);	// Handler�� �۾��� ������ �߰��Ѵ�.
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