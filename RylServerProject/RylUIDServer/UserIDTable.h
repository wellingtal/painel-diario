#ifndef _UID_USER_ID_TABLE_H_
#define _UID_USER_ID_TABLE_H_

/*
나중에 DB로 다 뺄 부분이다. 이곳은 임시로나마 하자.
*/

class CUserNode
{
private:

	unsigned long	m_dwSessionID;
	unsigned long	m_dwUserID;
	unsigned long	m_dwCharID;

    unsigned char   m_cAgentGroup;
    unsigned char   m_cState;

public:

	CUserNode(unsigned long dwUserID, unsigned long dwSessionID, 
		unsigned long dwCharID, unsigned char cGroup, unsigned char cState);	
	~CUserNode();

	void InitUserNode(unsigned long dwUserID, unsigned long dwSessionID, 
		unsigned long dwCharID, unsigned char cGroup, unsigned char cState);

	unsigned char   GetAgentGroup(void) const   { return m_cAgentGroup;	}
    unsigned char   GetState(void) const        { return m_cState;      }
    
	unsigned long	GetSessionID(void) const	{ return m_dwSessionID;	}
	unsigned long	GetUID(void) const			{ return m_dwUserID;	}
	unsigned long	GetCID(void) const			{ return m_dwCharID;	}
};


class CUserIDTable
{
public:

	static CUserIDTable& GetInstance();

	// interface
	BOOL		Insert(unsigned long dwUserID, const CUserNode& Node);
	BOOL		Erase(unsigned long dwUserID);
	CUserNode*	Find(unsigned long dwUserID);

	int			RemoveUserOfCurrentAgent(char Group);

private:

	CUserIDTable();
	~CUserIDTable();

	typedef std::map<unsigned long, CUserNode> UIDTable;
    UIDTable m_UIDTable;
};


#endif