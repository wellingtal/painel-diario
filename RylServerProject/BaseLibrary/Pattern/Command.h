#ifndef _PATTERN_COMMAND_H_
#define _PATTERN_COMMAND_H_

#include <list>
#include <algorithm>
#include <functional>

#include "../Thread/Lock.h"


class CCommand
{
public:

	virtual bool DoProcess() = 0;    
	virtual bool Destroy() = 0;		// ��ü�� ���� ����� �����Ѵ�. 
									// stack��� ��ü�� �ƹ� �ϵ� ���� �ʰ�, Heap��� ��ü�� delete this�� �� �� ��.    
};


class CCommandProcess
{
public:

	typedef CCSLock 				CMDLock;
	typedef std::list<CCommand*>	CMDList;

	CCommandProcess();
	~CCommandProcess();

	inline void Add(CCommand* lpNewCMD);
    void ProcessAll();
	
protected:

    void ClearAll();

	CMDLock m_CMDLock;
	CMDList	m_CMDList;
};


inline void CCommandProcess::Add(CCommand* lpNewCMD)
{
    if(NULL != lpNewCMD)
    {
        CMDLock::Syncronize sync(m_CMDLock);
	    m_CMDList.push_back(lpNewCMD);
    }
}


#endif