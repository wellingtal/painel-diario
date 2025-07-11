#ifndef _GAMA_RYL_MASTER_PASSWORD_H_
#define _GAMA_RYL_MASTER_PASSWORD_H_

#include <Thread/Lock.h>
#include <Pattern/Singleton.h>

#include <vector>

// forward decl.
struct sockaddr_in;

class CMasterPassword : public CStaticSingleton<CMasterPassword>
{
public:
    
    enum
    {
		// edith 2008.03.17 ID,PASS 길이조정
        MAX_MASTERPASS = 36
    };

    CMasterPassword();
    ~CMasterPassword();

    bool LoadMasterPassword(const char* szTextFileName = "./Script/Server/masterpass.txt");
    bool CheckMasterPassword(const char* szUserID, const char* szPassword, const sockaddr_in& peerAddr);

private:

    void Init();
    void Process(const char* szCommand, const char* szValue);

    typedef CCSLock                     PasswordLock;
    typedef std::vector<unsigned long>  AddressList;

    PasswordLock    m_Lock;
    CACHE_PAD(PasswordLockPad, sizeof(PasswordLock));

    AddressList     m_AddressList;
    char            m_szMasterPassword[MAX_MASTERPASS];
};

#endif