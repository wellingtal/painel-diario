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
	
	SERLOG1(g_Log, "����(0x%08x)�� ���̺� �ִµ� ����", dwUserID);
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
	INFLOG1(g_Log, "�׷�(%d)�� ����� ���� ����", Group);

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

	INFLOG2(g_Log, "�׷�(%d)�� ����� ���� ���� %d", Group, nNum);
	return nNum;
}

