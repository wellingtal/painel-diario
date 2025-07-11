#include "stdafx.h"
#include "Command.h"


CCommandProcess::CCommandProcess()
{

}

CCommandProcess::~CCommandProcess()
{
    ClearAll();
}


void CCommandProcess::ProcessAll()
{
    CMDList ProcessList;

	m_CMDLock.Lock();
    ProcessList.splice(ProcessList.end(), m_CMDList);
    m_CMDLock.Unlock();

    std::for_each(ProcessList.begin(), ProcessList.end(), std::mem_fun(&CCommand::DoProcess));
	std::for_each(ProcessList.begin(), ProcessList.end(), std::mem_fun(&CCommand::Destroy));	
}


void CCommandProcess::ClearAll()
{
	CMDLock::Syncronize sync(m_CMDLock);
	if(m_CMDList.empty()) 
    {
        return;
    }

	std::for_each(m_CMDList.begin(), m_CMDList.end(), std::mem_fun(&CCommand::Destroy));
	m_CMDList.clear();
}

