#ifndef _IO_WORKER_H_
#define _IO_WORKER_H_

#include "../../Thread/Thread.h"

// ���� ����
class CCompletionHandler;

class CIOWorker : public CThread 
{

public:

	CIOWorker(CCompletionHandler& SocketHandler);
	virtual ~CIOWorker();

private:

	virtual unsigned int Run();
	virtual BOOL End();

    CCompletionHandler& m_SocketHandler;

};


#endif