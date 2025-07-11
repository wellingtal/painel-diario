#ifndef _CAUTHORIZE_THREAD_H_
#define _CAUTHORIZE_THREAD_H_

#include <Thread/Thread.h>
//#include <map>

// 전방 참조.
class CDBComponent;
class CAuthInfo;
class CAuthorizeThread;

//#include "NetString.h"

class CAuthorizeThread : public CThread
//						,public CNetString::IEventListener
{
public:
    // Desc    : 타입에 따라 알맞은 인증 스레드 생성 (한게임/대만)
    static CAuthorizeThread* Create(); 

    // Desc    : 타입에 따라 알맞은 이름/패스워드 검사 (한게임/대만)
    static bool CheckUserIDAndPass(const char* szUserAccount_In, const char* szUserPW_In);	

    virtual ~CAuthorizeThread();

    bool Initialize();  // Desc    : DBObject등을 생성한다.
    void Destroy();     // Desc    : DBObject등을 소멸시킨다.

//	void EventPacket(int iIndex, char* pPacket);

	bool ConnectAll();

private:
    unsigned int Run();
    virtual BOOL End();

protected:

    virtual void Authorize(CAuthInfo* lpAuthInfo) = 0;    // Desc    : 인증 방법.

    CAuthorizeThread(); // Desc    : Create 함수를 사용해서만 생성할 수 있게 함.

    CDBComponent* m_lpComponent;
    CDBComponent* m_lpAdminToolComponent;

	BOOL	m_bConnectAll;

	// NetString을 생성한다.
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