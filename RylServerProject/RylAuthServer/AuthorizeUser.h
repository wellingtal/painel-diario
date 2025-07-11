#ifndef _CAUTHRORIZE_USER_H_
#define _CAUTHRORIZE_USER_H_

#include <list>
#include <Network/Packet/PacketStruct/ClientToAuthServer.h>
#include <BaseLibrary/Thread/Lock.h>
#include <BaseLibrary/Thread/ThreadMgr.h>
#include <BaseLibrary/Pattern/Singleton.h>

// ���� ����
class CAuthClientDispatch;

typedef struct AUTHINFO
{
	unsigned long	dwSessionID;			// ���Ǿ��̵�
    char            szUserAccount[40];		// ���̵�
    char            szUserPassword[40];		// �н�����
    char            szAddr[32];					// �ƾ���

	unsigned long   dwUserID;
    unsigned short  usFlag;
	unsigned short	usCnFlag;
    CAuthClientDispatch* lpDispatch;		// ó��Ŭ���� 
}*LPAUTHINFO;

class CAuthInfo
{
public:

	// login ��
    CAuthInfo(CAuthClientDispatch* lpDispatch, unsigned long dwSessionID, 
        const char* szUserAccount, const char* szUserPassword, 
        size_t nMaxUserAccountSize, size_t nMaxUserPasswordSize, 
        unsigned short usFlag, unsigned short usCnFlag);

	// logout��
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

	unsigned long	m_dwSessionID;			// ���Ǿ��̵�
    unsigned long   m_dwAuthStartTime;		// ���� ���۽ð�
    
    char            m_szUserAccount[PktAU::ID_LEN];		// ���̵�
    char            m_szUserPassword[PktAU::PASS_LEN];	// �н�����

	BOOL			m_bLogin;				// �α��� ��Ŷ

    unsigned short  m_usFlag;
	unsigned short	m_usCnFlag;
    CAuthClientDispatch* m_lpDispatch;		// ó��Ŭ���� 
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
    
    // Desc   : ���� ������ ���� �� ó�� ����.   
    bool Initialize(unsigned long nThreadNum, unsigned long dwMaxAuthorizeListSize);
    
    // Desc   : ���� ������ �Ҹ� �� ó�� ����.
    void Destroy();
    
    // Desc   : ���� ����Ʈ�� �� ���� ��û�� ���� ����.
    // Input  : CAuthInfo. ������ �ʿ��� ������
    // Output : ����Ʈ�� ���� ���� : true,   ����Ʈ�� ���� ���� �̻��� : false;
    bool AddAuthorizeUser(CAuthInfo* lpAuthInfo);

    // Desc   : ���� ������ �ִ� ���� ������ ����Ʈ�� �־� ��. �����Ͱ� �� ������ Wait������.    
    // Input  : AuthorizeList& LocalAuthorizeList ���� ���� ����Ʈ.
    // Output : �����͸� ��� �� : true,  ���� �� ����(�����) : false
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