#include "stdAfx.h"
#include <Log/ServerLog.h>
#include "UserIDTable.h"

CUserNode::CUserNode(unsigned long dwUserID, unsigned long dwSessionID, 
					 unsigned long dwCharID, unsigned char cGroup, unsigned char cState)
					 :	m_dwUserID(dwUserID), m_dwSessionID(dwSessionID), 
						m_dwCharID(dwCharID), m_cAgentGroup(cGroup), m_cState(cState) 
{


}

CUserNode::~CUserNode()
{

}

void CUserNode::InitUserNode(unsigned long dwUserID, unsigned long dwSessionID, 
							 unsigned long dwCharID, unsigned char cGroup, unsigned char cState)
{
	m_cAgentGroup	= cGroup;
    m_cState        = cState;
	m_dwSessionID	= dwSessionID;
	m_dwUserID		= dwUserID;
	m_dwCharID		= dwCharID;
}


CUserIDTable& CUserIDTable::GetInstance()
{
	static CUserIDTable userIDTable;
	return userIDTable;
}


CUserIDTable::CUserIDTable()
{

}

CUserIDTable::~CUserIDTable()
{
	m_UIDTable.clear();
}


BOOL CUserIDTable::Insert(unsigned long dwUserID, const CUserNode& Node)
{
	if(m_UIDTable.insert(std::make_pair(dwUserID, Node)).second)
	{
		return TRUE;
	}
	
	SERLOG1(g_Log, "유저(0x%08x)를 테이블에 넣는데 실패", dwUserID);
	return FALSE;
}


BOOL CUserIDTable::Erase(unsigned long dwUserID)
{
    UIDTable::iterator itr = m_UIDTable.find(dwUserID);
    if(itr != m_UIDTable.end())
    {        
        m_UIDTable.erase(itr);
        return TRUE;
    }

	return FALSE;
}


CUserNode* CUserIDTable::Find(unsigned long dwUserID)
{
    UIDTable::iterator itr = m_UIDTable.find(dwUserID);
    return (itr != m_UIDTable.end()) ? (&itr->second) : 0;
}


int CUserIDTable::RemoveUserOfCurrentAgent(char Group)
{
	int nNum = 0;
	INFLOG1(g_Log, "그룹(%d)의 사용자 삭제 시작", Group);

	UIDTable::iterator pos = m_UIDTable.begin();
	UIDTable::iterator end = m_UIDTable.end();

	for(; pos != end; ) 
	{
		CUserNode& node = pos->second;

		if(node.GetAgentGroup() == Group)
		{                
			pos = m_UIDTable.erase(pos);
		}
		else
		{
			++pos;
		}
	}

	INFLOG2(g_Log, "그룹(%d)의 사용자 삭제 종료 %d", Group, nNum);
	return nNum;
}

