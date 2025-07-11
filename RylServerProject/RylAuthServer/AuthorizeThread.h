#ifndef _CAUTHORIZE_THREAD_H_
#define _CAUTHORIZE_THREAD_H_

#include <Thread/Thread.h>
//#include <map>

// ���� ����.
class CDBComponent;
class CAuthInfo;
class CAuthorizeThread;

//#include "NetString.h"

class CAuthorizeThread : public CThread
//						,public CNetString::IEventListener
{
public:
    // Desc    : Ÿ�Կ� ���� �˸��� ���� ������ ���� (�Ѱ���/�븸)
    static CAuthorizeThread* Create(); 

    // Desc    : Ÿ�Կ� ���� �˸��� �̸�/�н����� �˻� (�Ѱ���/�븸)
    static bool CheckUserIDAndPass(const char* szUserAccount_In, const char* szUserPW_In);	

    virtual ~CAuthorizeThread();

    bool Initialize();  // Desc    : DBObject���� �����Ѵ�.
    void Destroy();     // Desc    : DBObject���� �Ҹ��Ų��.

//	void EventPacket(int iIndex, char* pPacket);

	bool ConnectAll();

private:
    unsigned int Run();
    virtual BOOL End();

protected:

    virtual void Authorize(CAuthInfo* lpAuthInfo) = 0;    // Desc    : ���� ���.

    CAuthorizeThread(); // Desc    : Create �Լ��� ����ؼ��� ������ �� �ְ� ��.

    CDBComponent* m_lpComponent;
    CDBComponent* m_lpAdminToolComponent;

	BOOL	m_bConnectAll;

	// NetString�� �����Ѵ�.
//	CNetString*	m_lpLogin;
//	CNetString*	m_lpLogout;

//	std::map<std::string, AUTHINFO>	m_AuthMap;

private:
	unsigned long m_AuthCount;

public:

	unsigned long MakeSessionID(unsigned long SessionID)
	{
		unsigned long dwSessionID = (SessionID ^ ++m_AuthCount);
        return 0 == dwSessionID ? dwSessionID + rand() : dwSessionID;
	}
};


#endif