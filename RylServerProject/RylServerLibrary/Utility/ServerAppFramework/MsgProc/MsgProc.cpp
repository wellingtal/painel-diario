#include "stdafx.h"
#include "MsgProc.h"

CMsgProcessMgr::CMsgProcessMgr()
{

}

CMsgProcessMgr::~CMsgProcessMgr()
{
	Clear();
}


bool CMsgProcessMgr::Register(UINT uMsg, CMsgProc* lpMsgProcessProc)
{
	return m_MessageProcessMap.insert(
		MessageProcessMap::value_type(uMsg, lpMsgProcessProc)).second;	
}


bool CMsgProcessMgr::Remove(UINT uMsg)
{
	return (0 != m_MessageProcessMap.erase(uMsg));
}


CMsgProc* CMsgProcessMgr::FindMsgProc(UINT uMsg)
{
	MessageProcessMap::iterator find = m_MessageProcessMap.find(uMsg);
	MessageProcessMap::iterator end  = m_MessageProcessMap.end();

	return (find != end) ? find->second : 0;
}


void CMsgProcessMgr::Clear()
{
	MessageProcessMap::iterator pos = m_MessageProcessMap.begin();
	MessageProcessMap::iterator end = m_MessageProcessMap.end();

	for(; pos != end; ++pos)
	{
		delete pos->second;
	}

	m_MessageProcessMap.clear();
}