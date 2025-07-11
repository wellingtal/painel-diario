#ifndef _GAMA_CHAT_SERVER_CONFIGURATIONS_H_
#define _GAMA_CHAT_SERVER_CONFIGURATIONS_H_

#include <string>
#include <map>

#include <Network/Session/LimitUserByIP.h>

class CChatServerConfig
{
public:

    static CChatServerConfig& GetInstance();

    //! ���Ϸκ��� �¾��� �ε��Ѵ�. (������ ������ ���� ��������.)
    bool LoadSetup(const char* szFileName = ms_szDefaultSetupFileName);

    //! ���� ID�� �н����带 �޾Ƽ�, �¾� ���Ͽ� �����ϴ� ����ID/Password���� Ȯ���Ѵ�.
    //! �ߺ� �α��� üũ�� ���Ѵ�.
    bool Authorize(const char* szUserID, const char* szPassword);
    void Logout(const char* szUserID);

    //! IP������ �Ŵ� Ŭ������ �ѱ��.
    CLimitUserByIP* GetAllowIP() { return m_lpLimitUserByIP; }

private:

    CChatServerConfig();
    ~CChatServerConfig();

    static const char* ms_szDefaultSetupFileName;
    
    typedef std::map<std::string, std::pair<std::string, unsigned long> >   ChatToolAuthMap;

    ChatToolAuthMap     m_ChatToolAuthMap;
    CLimitUserByIP*     m_lpLimitUserByIP;
};


#endif