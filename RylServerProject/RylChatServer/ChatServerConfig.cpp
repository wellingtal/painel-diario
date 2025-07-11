#include "stdafx.h"
#include "ChatServerConfig.h"
#include <Utility/Setup/ServerSetup.h>

const char* CChatServerConfig::ms_szDefaultSetupFileName = "./Script/Server/ChatServerConfig.txt";

CChatServerConfig& CChatServerConfig::GetInstance()
{
    static CChatServerConfig chatServerConfig;
    return chatServerConfig;
}

CChatServerConfig::CChatServerConfig()
:   m_lpLimitUserByIP(new CLimitUserByIP(0))
{


}

CChatServerConfig::~CChatServerConfig()
{
    if(m_lpLimitUserByIP)
    {
        delete m_lpLimitUserByIP;
        m_lpLimitUserByIP = 0;
    }
}

//! 파일로부터 셋업을 로드한다. (이전의 내용은 전부 지워진다.)
bool CChatServerConfig::LoadSetup(const char* szFileName)
{
    if(0 != m_lpLimitUserByIP)
    {
        m_lpLimitUserByIP->ClearIPList();
        m_lpLimitUserByIP->OperateMode(CLimitUserByIP::DENY_ALL);
        m_lpLimitUserByIP->ReserveAllowIP(256);
        m_ChatToolAuthMap.clear();

        FILE* lpFile = fopen(szFileName, "rt");

        if(lpFile)
        {
            const int MAX_BUFFER = 4096;
            char  szBuffer[MAX_BUFFER];
            char* szToken = 0;

            enum SetupMode
            {
                NONE,
                USER_PASSWORD,
                ALLOW_IP
            };

            SetupMode eSetupMode = NONE;            
            unsigned long dwAddress = 0;

            std::string szUserID;
            std::string szPassword;

            while(fgets(szBuffer, MAX_BUFFER - 1, lpFile))
            {
                szBuffer[MAX_BUFFER - 1] = 0;

                szToken = strtok(szBuffer, "\r\n\t= ");
                if(0 != szToken)
                {
                    char szTokenBuffer[MAX_BUFFER];
                    strcpy(szTokenBuffer, szToken);

                    _strupr(szTokenBuffer);
                    if(0 == strcmp(szTokenBuffer, "//"))
                    {
                        // 주석처리...
                        continue;
                    }
                    else if(0 == strcmp(szTokenBuffer, "[USER]"))
                    {
                        eSetupMode = USER_PASSWORD;
                    }
                    else if(0 == strcmp(szTokenBuffer, "[ALLOW_IP]"))
                    {
                        eSetupMode = ALLOW_IP;
						
						if (true == CServerSetup::GetInstance().GetChatToolIPCheck())
						{
							// 허가받은 사람들만 들어올 수 있도록 설정.
							m_lpLimitUserByIP->OperateMode(CLimitUserByIP::ALLOW_SOME);
						}
						else
						{
							m_lpLimitUserByIP->OperateMode(CLimitUserByIP::ALLOW_ALL);	
						}
                    }
                    else
                    {
                        switch(eSetupMode)
                        {
                        case USER_PASSWORD:
                            szUserID.assign(szToken);
                            szToken = strtok(0, "\r\n\t= ");
                            if(0 != szToken)
                            {
                                szPassword.assign(szToken);
                                m_ChatToolAuthMap.insert(
                                    std::make_pair(szUserID, std::make_pair(szPassword, 0)));
                            }
                            break;

                        case ALLOW_IP:
                            dwAddress = inet_addr(szToken);
                            if(0 != dwAddress)
                            {
                                m_lpLimitUserByIP->AddAllowIP(dwAddress);
                            }
                            break;
                        }
                    }
                }
            };
            
            fclose(lpFile);
            return true;
        }
    }

    return false;
}

//! 유저 ID와 패스워드를 받아서, 셋업 파일에 존재하는 유저ID/Password인지 확인한다.
bool CChatServerConfig::Authorize(const char* szUserID, const char* szPassword)
{
    ChatToolAuthMap::iterator pos = m_ChatToolAuthMap.find(szUserID);
    ChatToolAuthMap::iterator end = m_ChatToolAuthMap.end();

   if (pos != end && 0 == pos->second.second && 
        0 == pos->second.first.compare(szPassword))
    {
        // 로그인 상태로 전환
        pos->second.second = 1;
        return true;
    }

    return false;
}

void CChatServerConfig::Logout(const char* szUserID)
{
    ChatToolAuthMap::iterator pos = m_ChatToolAuthMap.find(szUserID);
    ChatToolAuthMap::iterator end = m_ChatToolAuthMap.end();

    if (pos != end && 0 != pos->second.second)
    {
        // 로그아웃 상태로 전환
        pos->second.second = 0;        
    }
}