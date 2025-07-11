#ifndef _GAMA_CHAT_SERVER_CONFIGURATIONS_H_
#define _GAMA_CHAT_SERVER_CONFIGURATIONS_H_

#include <string>
#include <map>

#include <Network/Session/LimitUserByIP.h>

class CChatServerConfig
{
public:

    static CChatServerConfig& GetInstance();

    //! 파일로부터 셋업을 로드한다. (이전의 내용은 전부 지워진다.)
    bool LoadSetup(const char* szFileName = ms_szDefaultSetupFileName);

    //! 유저 ID와 패스워드를 받아서, 셋업 파일에 존재하는 유저ID/Password인지 확인한다.
    //! 중복 로그인 체크도 겸한다.
    bool Authorize(const char* szUserID, const char* szPassword);
    void Logout(const char* szUserID);

    //! IP제한을 거는 클래스를 넘긴다.
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