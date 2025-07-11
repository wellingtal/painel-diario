#ifndef _CAUTHRORIZE_USER_H_
#define _CAUTHRORIZE_USER_H_

#include <list>
#include <Network/Packet/PacketStruct/ClientToAuthServer.h>
#include <BaseLibrary/Thread/Lock.h>
#include <BaseLibrary/Thread/ThreadMgr.h>
#include <BaseLibrary/Pattern/Singleton.h>

// 전방 참조
class CAuthClientDispatch;

typedef struct AUTHINFO
{
	unsigned long	dwSessionID;			// 세션아이디
    char            szUserAccount[40];		// 아이디
    char            szUserPassword[40];		// 패스워드
    char            szAddr[32];					// 아아피

	unsigned long   dwUserID;
    unsigned short  usFlag;
	unsigned short	usCnFlag;
    CAuthClientDispatch* lpDispatch;		// 처리클래스 
}*LPAUTHINFO;

class CAuthInfo
{
public:

	// login 용
    CAuthInfo(CAuthClientDispatch* lpDispatch, unsigned long dwSessionID, 
        const char* szUserAccount, const char* szUserPassword, 
        size_t nMaxUserAccountSize, size_t nMaxUserPasswordSize, 
        unsigned short usFlag, unsigned short usCnFlag);

	// logout용
    CAuthInfo(CAuthClientDispatch* lpDispatch, const char* szUserAccount,
		size_t nMaxUserAccountSize);

    ~CAuthInfo();

	BOOL GetIsLogin()			{ return m_bLogin; }
	unsigned long GetSessionID() { return m_dwSessionID; }
    char* GetUserAccount()		 { return m_szUserAccount;  }
    char* GetUserPassword()		 { return m_szUserPassword; }
	unsigned short GetFlag()	 { return m_usFlag;		}
    unsigned short GetCnFlag()	 { return m_usCnFlag;	}
    CAuthClientDispatch* GetDispatch()	{ return m_lpDispatch;	}
    
    unsigned long GetAuthStartTime() { return m_dwAuthStartTime; }

protected:

	unsigned long	m_dwSessionID;			// 세션아이디
    unsigned long   m_dwAuthStartTime;		// 인증 시작시간
    
    char            m_szUserAccount[PktAU::ID_LEN];		// 아이디
    char            m_szUserPassword[PktAU::PASS_LEN];	// 패스워드

	BOOL			m_bLogin;				// 로그인 패킷

    unsigned short  m_usFlag;
	unsigned short	m_usCnFlag;
    CAuthClientDispatch* m_lpDispatch;		// 처리클래스 
};


class CSyncronizedObjectQueue
{
public:

	CSyncronizedObjectQueue();
	~CSyncronizedObjectQueue();

	bool enqueue(CAuthInfo* lpCommand, bool bPendHead = false);
	CAuthInfo* dequeue(unsigned long dwMilliSecond = INFINITE);

    void remove(const char* szUserAccount);

    void set_max_size(size_t nSize) { m_nMaxSize = nSize; }
    size_t get_size() { return m_CommandQueue.size(); }
    void stop();
    bool is_stopped();
    void clear();

protected:

	enum Const
	{
		StopperIndex, 
		SemaphoreIndex, 
		MaxIndex
	};

private:

	typedef std::list<CAuthInfo*> CommandQueue;

	CCSLock			m_DataLock;
	HANDLE			m_hHandles[MaxIndex];
	CommandQueue	m_CommandQueue;
    size_t          m_nMaxSize;
    bool            m_bStop;
};


class CAuthorizeUser
{
public:

	static CAuthorizeUser& GetInstance();

    typedef std::list<CAuthInfo*>   AuthorizeList;
    
    // Desc   : 인증 스레드 생성 및 처리 시작.   
    bool Initialize(unsigned long nThreadNum, unsigned long dwMaxAuthorizeListSize);
    
    // Desc   : 인증 스레스 소멸 및 처리 종료.
    void Destroy();
    
    // Desc   : 인증 리스트에 새 인증 요청을 집어 넣음.
    // Input  : CAuthInfo. 인증에 필요한 정보들
    // Output : 리스트에 삽입 성공 : true,   리스트가 일정 개수 이상임 : false;
    bool AddAuthorizeUser(CAuthInfo* lpAuthInfo);

    // Desc   : 현재 가지고 있는 인증 정보를 리스트에 넣어 줌. 데이터가 올 때까지 Wait상태임.    
    // Input  : AuthorizeList& LocalAuthorizeList 인증 정보 리스트.
    // Output : 데이터를 얻어 옴 : true,  얻어올 수 없음(종료시) : false
    CAuthInfo* GetAuthorizeInfo(unsigned long dwMilliSecond = INFINITE);

    size_t GetAuthUserNum() { return m_AuthInfoQueue.get_size(); }

    void stop() { m_AuthInfoQueue.stop(); }
    bool is_stopped() { return m_AuthInfoQueue.is_stopped(); }

    void RemovePending(const char* szUserAccount);

	void ConnectAll();

private:

    CAuthorizeUser();
    ~CAuthorizeUser();

    enum { DEFAULT_AUTHORIZE_LIST = 10000 };
    
    typedef CCSLock AuthorizeLock;

    CSyncronizedObjectQueue m_AuthInfoQueue;
    CThreadMgr              m_AuthorizeThreadMgr;
};

#endif