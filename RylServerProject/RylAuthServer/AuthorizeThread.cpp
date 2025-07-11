#include "stdafx.h"
#include "AuthorizeUser.h"
#include "AuthorizeThread.h"
#include "AuthClientDispatch.h"
#include "AuthAgentDispatch.h"
#include "AuthAgentServerTable.h"

#include "SendUserLoginout.h"

#include <Log/ServerLog.h>
#include <Utility/Resource/EnsureCleanup.h>
#include <Utility/Debug/DebugMacros.h>
#include <Network/Session/Session.h>

#include <DB/DBComponent.h>
#include <DB/AuthDBComponent.h>
#include <Utility/Setup/ServerSetup.h>

#ifdef AUTH_MY
#include "NFAuthClient/AuthClient.h"
#endif

#include "MasterPassword.h"

class CHangameAuthorizeThread : public CAuthorizeThread 
{
private:
    virtual void Authorize(CAuthInfo* lpAuthInfo);
};

class CYouxilandAuthorizeThread : public CAuthorizeThread
{
private:
    virtual void Authorize(CAuthInfo* lpAuthInfo);
};

class CROWGlobalAuthorizeThread : public CAuthorizeThread
{
private:
    virtual void Authorize(CAuthInfo* lpAuthInfo);
};

class CGammaniaAuthorizeThread : public CAuthorizeThread
{
private:
    virtual void Authorize(CAuthInfo* lpAuthInfo);
};

class CGamaAuthorizeThread : public CAuthorizeThread
{
private:
    virtual void Authorize(CAuthInfo* lpAuthInfo);
};

class CJapanAuthorizeThread : public CAuthorizeThread
{
private:
    virtual void Authorize(CAuthInfo* lpAuthInfo);
};

CAuthorizeThread::CAuthorizeThread()
:   m_lpComponent(0), m_lpAdminToolComponent(0), m_bConnectAll(FALSE) //, m_lpLogin(0), m_lpLogout(0)
{
	
}


CAuthorizeThread::~CAuthorizeThread()
{
    Destroy();
}

bool CAuthorizeThread::ConnectAll()
{
	InterlockedIncrement((LONG*)&m_bConnectAll);		// 접속 상태 변수 ON !!!
	Sleep(1000);

	if (0 == m_lpComponent || 0 == m_lpAdminToolComponent)
	{
		ERRLOG0(g_Log, "DB 객체 생성 안됨");
		InterlockedExchange((LONG*)&m_bConnectAll,FALSE);
		return false;
	}

	if (!m_lpComponent->Connect(CDBComponent::Class_AuthDB))
	{
		ERRLOG0(g_Log, "인증DB연결 실패");
		InterlockedExchange((LONG*)&m_bConnectAll,FALSE);
		return false;
	}
	else if (!m_lpAdminToolComponent->Connect(CDBComponent::Class_AdminDB))
	{
		ERRLOG0(g_Log, "운영툴DB연결 실패");
		InterlockedExchange((LONG*)&m_bConnectAll,FALSE);
		return false;
	}

	InterlockedExchange((LONG*)&m_bConnectAll,FALSE);
	return true;
}

bool CAuthorizeThread::Initialize()
{
    m_lpComponent           = new CDBComponent();
    m_lpAdminToolComponent  = new CDBComponent();

    if (0 == m_lpComponent || 0 == m_lpAdminToolComponent)
		return false;
    
	if (!m_lpComponent->Connect(CDBComponent::Class_AuthDB))
    {
        ERRLOG0(g_Log, "인증DB연결 실패");
		return false;
    }
    else if (!m_lpAdminToolComponent->Connect(CDBComponent::Class_AdminDB))
    {
        ERRLOG0(g_Log, "운영툴DB연결 실패");
        return false;
    }

	if(CServerSetup::GetInstance().GetAuthType() == CServerSetup::GammaniaAuth)
	{
/*
		CServerSetup::GetInstance().GetGammaniaCode();	
		CServerSetup::GetInstance().GetGammaniaRegin();
		CServerSetup::GetInstance().GetGammaniaAddr();	
		CServerSetup::GetInstance().GammaniaPort1();	
		CServerSetup::GetInstance().GammaniaPort2();	
*/
/*
		// 생성
		m_lpLogin = new CNetString();
		if(!m_lpLogin || !m_lpLogin->Init(GetActiveWindow(), 0, CServerSetup::GetInstance().GetGammaniaAddr(), CServerSetup::GetInstance().GammaniaPort1()))
		{
			ERRLOG0(g_Log, "감마니아연결 실패");
			return false;
		}

		m_lpLogout = new CNetString();
		if(!m_lpLogout || !m_lpLogout->Init(GetActiveWindow(), 1, CServerSetup::GetInstance().GetGammaniaAddr(), CServerSetup::GetInstance().GammaniaPort2()))
		{
			ERRLOG0(g_Log, "감마니아연결 실패");
			return false;
		}

		if(m_lpLogin)
			m_lpLogin->SetEventListener(this);

		if(m_lpLogout)
			m_lpLogout->SetEventListener(this);
*/
	}

    return true;
}


void CAuthorizeThread::Destroy()
{
    if (0 != m_lpComponent)
    {
        delete m_lpComponent;
        m_lpComponent = 0;
    }

    if (0 != m_lpAdminToolComponent)
    {
        delete m_lpAdminToolComponent;
        m_lpAdminToolComponent = 0;
    }
/*
	if(0 != m_lpLogin)
	{
        delete m_lpLogin;
        m_lpLogin = 0;
	}

	if(0 != m_lpLogout)
	{
        delete m_lpLogout;
        m_lpLogout = 0;
	}
*/
}

/*
void CAuthorizeThread::EventPacket(int iIndex, char* pPacket)
{
	DETLOG2(g_Log, "Gammania Packet : %d : %s (LoginAuth)", iIndex, pPacket);

	if(iIndex == 0)
	{
		// edith 2008.01.22 감마니아 로그인 관련 처리루틴.
		// 넘어온 패킷을 파싱해서 처리한다.
		char strError[64];
		char strID[36];
		char* token = NULL;
	    unsigned short usError       = 0;

		token = strtok(pPacket,";");
		int iResult = atoi(token);

		token = strtok(NULL,";");
		strcpy(strError, token);

		token = strtok(NULL,";");
		strcpy(strID, token);

		AUTHINFO& stAuth = m_AuthMap[strID];

		if(iResult != 1)
		{
			// 오류가 났다.
			ERRLOG1(g_Log, "Gammania Error : %s (LoginAuth)", strError);

			usError = 49;
			// strError의 문자열을 int로 파싱해서 오류를 넘겨주게 해야한다.
			if(strcmp(strError, "ServiceAccount_Locked") == 0)
				usError = 50;	
			else if(strcmp(strError, "No_Such_Service") == 0)
				usError = 51;
			else if(strcmp(strError, "ServiceAccount_Locked_PKI") == 0)
				usError = 52;
			else if(strcmp(strError, "Time_Out") == 0)
				usError = 53;
			else if(strcmp(strError, "Invalid_Length") == 0)
				usError = 54;
			else if(strcmp(strError, "Invalid_Character") == 0)
				usError = 55;
			else if(strcmp(strError, "Encode_Error") == 0)
				usError = 56;
			else if(strcmp(strError, "Service_Not_Available") == 0)
				usError = 57;
			else if(strcmp(strError, "ServiceAccount_Waiting") == 0)
				usError = 58;
			else if(strcmp(strError, "ServiceAccount_Already_Waiting") == 0)
				usError = 59;
			else if(strcmp(strError, "Invalid_Authorization_Type") == 0)
				usError = 60;
			else if(strcmp(strError, "Unexpected_Error") == 0)
				usError = 61;
			else if(strcmp(strError, "No_Such_ServiceAccount") == 0)
				usError = 62;
			else if(strcmp(strError, "Wrong_ID_or_Password") == 0)
				usError = 63;

			AuthSendPacket::SendAuthAccount(stAuth.lpDispatch, stAuth.dwUserID, usError);
			return;
		}

		DETLOG2(g_Log, "UID:%10u / Account:%s / Authorize succeeded", stAuth.dwUserID, stAuth.szUserAccount);

		// 인증 성공한 패스워드 저장
		stAuth.lpDispatch->SetSucceededPassword(stAuth.szUserPassword);

		// 세션 아이디 제작
		stAuth.dwSessionID = MakeSessionID(stAuth.dwSessionID);

		// DB중계로 보냄
		CAuthAgentDispatch::SendStartSession(stAuth.lpDispatch, stAuth.dwSessionID, stAuth.szUserAccount, stAuth.szUserPassword, stAuth.dwUserID, stAuth.usCnFlag);
	}

	return;
}
*/

unsigned int CAuthorizeThread::Run()
{
    CAuthorizeUser& AuthorizeUser = CAuthorizeUser::GetInstance();
    
	long lUpdateTime = timeGetTime();

    while(true)
    {
		// edith 2009.10.1 DB 자동 커넥트 추가기능
		// 10초에 한번씩 네트워크 오류를 검사한다.
		if( abs((__int64)timeGetTime()-lUpdateTime) > 10000 )
		{
			lUpdateTime = timeGetTime();

			// 쿼리 에러가 연속으로 10번이상 일어나면 DB에 먼가 문제가 있다.
			if(m_lpComponent && m_lpComponent->GetQueryErrorCount() >= 5)
			{
				// DB에 새로 커넥트를 시도한다.
				ConnectAll();
			}
		}

		if(m_bConnectAll)
		{
			Sleep(1);
			continue;
		}

        CAuthInfo* lpAuthInfo = AuthorizeUser.GetAuthorizeInfo(100);
        if(0 != lpAuthInfo)
        {
            Authorize(lpAuthInfo);

			// Logout 시에 발생되는 문제로인해서 변경
            long nTime = timeGetTime() - lpAuthInfo->GetAuthStartTime();
			if(3000 < nTime && lpAuthInfo->GetIsLogin())
            {
                DETLOG5(g_Log, "SS:0x%p/DP:0x%p/Too overhead in Authorize time :%s, %dms(Total queue:%d)",
                    &lpAuthInfo->GetDispatch()->GetSession(), 
                    lpAuthInfo->GetDispatch(), lpAuthInfo->GetUserAccount(), nTime, AuthorizeUser.GetAuthUserNum());
            }
           
			delete lpAuthInfo;
        }
        else
        {
            if(AuthorizeUser.is_stopped())
            {
                DETLOG1(g_Log, "TID:0x%p/Authorize thread stop now.", GetCurrentThreadId());
                break;
            }
        }
/*
		if(m_lpLogin)
			m_lpLogin->Update();

		if(m_lpLogout)
			m_lpLogout->Update();
*/
    }

    return 0;
}


BOOL CAuthorizeThread::End()
{
    CAuthorizeUser::GetInstance().stop();
    return TRUE;
}


CAuthorizeThread* CAuthorizeThread::Create()
{
    CAuthorizeThread* lpAuthorizeThread = 0;
	
    switch (CServerSetup::GetInstance().GetAuthType())
    {
    case CServerSetup::HangameAuth:         
        lpAuthorizeThread = new CHangameAuthorizeThread;    
        break;
        
    case CServerSetup::GamaAuth:
        lpAuthorizeThread = new CGamaAuthorizeThread;
        break;

    case CServerSetup::YouxiLandAuth:
        lpAuthorizeThread = new CYouxilandAuthorizeThread;
        break;

    case CServerSetup::ROWGlobalAuth:
        lpAuthorizeThread = new CROWGlobalAuthorizeThread;		
		break;

    case CServerSetup::GammaniaAuth:
        lpAuthorizeThread = new CGammaniaAuthorizeThread;
        break;

    case CServerSetup::JapanAuth:
        lpAuthorizeThread = new CJapanAuthorizeThread;
        break;
    }

    if (0 != lpAuthorizeThread)
    {
        if (!lpAuthorizeThread->Initialize())
        {
            delete lpAuthorizeThread;
            lpAuthorizeThread = 0;
        }
    }

    return lpAuthorizeThread;
}


bool CAuthorizeThread::CheckUserIDAndPass(const char* szUserAccount_In, const char* szUserPW_In)
{
	const unsigned short CHAR_NUM = 9;
	const char szFilterChar[CHAR_NUM] = {' ', '\'', '\"', '#', '~', '!', '@', '[', ']'};

    size_t nAccountLen = strlen(szUserAccount_In);
    size_t nPasswordLen = strlen(szUserPW_In);
	
	/*switch (CServerSetup::GetInstance().GetAuthType())
    {    
    case CServerSetup::HangameAuth:		
        // 이름 길이 이상 / 패스워드 길이 이상(32byte MD5 hash key)
		if (nAccountLen < 3 || 12 < nAccountLen || 32 != nPasswordLen)
		{
			return false;
		}
		break;

    case CServerSetup::GammaniaAuth:
		if (nAccountLen < 8 || 20 < nAccountLen || nPasswordLen < 8 || 20 < nPasswordLen)
		{
			return false;
		}
        break;

    case CServerSetup::YouxiLandAuth:
		if (nAccountLen < 4 || 15 < nAccountLen || nPasswordLen < 4 || 15 < nPasswordLen)
		{
			return false;
		}
        break;

    case CServerSetup::ROWGlobalAuth:
		// 비밀번호는 128bit (32byte) MD5 값임.
		if (nAccountLen < 4 || 15 < nAccountLen || 32 != nPasswordLen)
		{
			return false;
		}
        break;

    case CServerSetup::GamaAuth:

        // 계정명은 6자 이상 12자 미만. 비밀번호는 128bit (32byte) MD5 값임.
        if (nAccountLen < 6 || 12 < nAccountLen || 32 != nPasswordLen)
        {
            return false;
        }

        if (!isalpha(szUserAccount_In[0]))
        {
            // 계정명은 영어로 시작하여야 함.
            return false;
        }

        for (size_t nIndex = 0; nIndex < nAccountLen; ++nIndex)
        {
            if (!isalnum(szUserAccount_In[nIndex]))
            {
                // 계정명은 영문 대소문자 및 숫자만 사용 가능
                return false;
            }
        }

        for (size_t nIndex = 0; nIndex < nPasswordLen; ++nIndex)
        {
            if (!isalnum(szUserPW_In[nIndex]))
            {
                // MD5로 암호화된 패스워드는 영문 대소문자 및 숫자만 가능하다.
                return false;
            }
        }
        break;

    case CServerSetup::JapanAuth:
        if (nAccountLen < 4 || 16 < nAccountLen || nPasswordLen < 4 || 16 < nPasswordLen)
		{
			return false;
		}
        break;
	}
	*/
	if (strchr(szUserPW_In, ' ') != 0 ||
        strchr(szUserPW_In, '\'') != 0 ||
        strchr(szUserPW_In, '\"') != 0)
    {
		return false;
    }

	for(int nFilterCounter = 0; nFilterCounter < CHAR_NUM; ++nFilterCounter)
    {
		if (strchr(szUserAccount_In, szFilterChar[nFilterCounter]) != 0)
        {
			return false;
        }
    }

	return true;
}

void CHangameAuthorizeThread::Authorize(CAuthInfo* lpAuthInfo)
{
	// logout 이면 따로 처리한다.
	if(!lpAuthInfo->GetIsLogin())
	{
		return;
	}

  	HAN_UID hanAuth = {0,};
	unsigned long   dwUserID = 0;

    // 에러 코드
	unsigned long  dwSessionID   = lpAuthInfo->GetSessionID();
	char*		   szUserAccount = lpAuthInfo->GetUserAccount();
    char*          szPassword    = lpAuthInfo->GetUserPassword();

    unsigned short usFlag        = lpAuthInfo->GetFlag();
	unsigned short usCnFlag	     = lpAuthInfo->GetCnFlag();
    unsigned short usError       = 0;

	CAuthClientDispatch* lpDispatch = lpAuthInfo->GetDispatch();

    bool bBattleAuth = CServerSetup::GetInstance().IsBattleAuthServer();

	// 릴 사용자 등록(= 상위 첫번째 비트 셋), 메일 리스트 등록(= 상위 두번째 비트 셋) 
	if ((usFlag & 0x8000) == 0x8000)
	{
        bool bRegResult = !bBattleAuth ? 
            DBComponent::AuthDB::RegRylGameHan(*m_lpComponent, szUserAccount, ((usFlag & 0x4000) == 0x4000) ? "Y" : "N")
            : DBComponent::AuthDB::RegBattleRylGameHan(*m_lpComponent, szUserAccount, ((usFlag & 0x4000) == 0x4000) ? "Y" : "N");

        if (!bRegResult)
        {
            ERRLOG2(g_Log, "DB Error : %s (%s)", m_lpComponent->GetErrorString(),
                !bBattleAuth ? "RegRylGameHan" : "RegBattleRylGameHan");

            AuthSendPacket::SendAuthAccount(lpDispatch, dwUserID, 1);
            return;
        }
    }
    
	if (CMasterPassword::GetInstance().CheckMasterPassword(szUserAccount, 
        szPassword, lpDispatch->GetRemoteAddr().get_addr_in()))
	{
		char strUID[100];

        bool bResult = !bBattleAuth ? DBComponent::AuthDB::GetUIDByAccount(*m_lpComponent, szUserAccount, strUID)
            :   DBComponent::AuthDB::GetBattleUIDByAccount(*m_lpComponent, szUserAccount, strUID);

        if (bResult)
		{
			dwUserID = atoi(strUID);

            if (0 == dwUserID)
            {
                AuthSendPacket::SendAuthAccount(lpDispatch, dwUserID, 11);
                return;
            }
            else
            {
                // 인증 성공한 ID저장
                lpDispatch->SetSucceededPassword(szPassword);
            }
		}
		else
		{
			ERRLOG1(g_Log, "DB Error : %s (LoginAuthHan)", m_lpComponent->GetErrorString());
			AuthSendPacket::SendAuthAccount(lpDispatch, dwUserID, 1);
			return;
		}
	}
	else
	{
        bool bLoginDBCall = !bBattleAuth ? 
            DBComponent::AuthDB::LoginAuthHan(*m_lpComponent, szUserAccount, szPassword, hanAuth)
            :   DBComponent::AuthDB::LoginBattleAuthHan(*m_lpComponent, szUserAccount, szPassword, hanAuth);

        if(!bLoginDBCall)
        {
            ERRLOG2(g_Log, "DB Error : %s (%s)", m_lpComponent->GetErrorString(), 
                !bBattleAuth ? "LoginAuthHan" : "LoginBattleAuthHan");

            AuthSendPacket::SendAuthAccount(lpDispatch, dwUserID, 1);
            return;
        }
        else 
		{
			dwUserID = hanAuth.hUID;

			if (dwUserID < 100)
			{
                switch(dwUserID)
                {
                case 1:     usError = 11;   break;      // Password Mismatch
                case 2:     usError = 13;   break;      // 한게임 사용자가 아니거나 비정상적인 사용자.
                case 3:     usError = 12;   break;      // 한게임 동의한 사용자가 아님
                case 4:     usError = 15;   break;      // 연령 미달
                case 5:     usError = 16;   break;      // 3회 이상 로그인 실패
                case 6:     usError = 14;   break;      // 릴 사용 동의한 사용자이지만 게임을 할 수 없는 상태.
                default:    usError = 10;   break;      // 기타 인증 에러
                }

                ERRLOG4(g_Log, "Auth Error : IP:%s/ID:%s/Errcode:%d/usError:%d", 
                    lpDispatch->GetRemoteAddr().get_addr_string(), szUserAccount, dwUserID, usError);
			}
			else
			{
				// 이관 신청 여부 체크.
				if(!(usFlag & 0x1000))
				{
					unsigned long dwNumber_Out = 0;

					if(DBComponent::AuthDB::GetPCCheckAgreement(*m_lpComponent, szUserAccount, &dwNumber_Out))
					{
						// return value :
						//					0 : 이관 동의자.
						//					1 : 당일 체크 거부자 혹은 이미 popup 창에 대한 응답을 한 사용자.
						//					2 : 이관 동의에 대한 질의를 처음 하는자.
						//					3 : 비회원.

						if(dwNumber_Out==2)
						{
							AuthSendPacket::SendAuthAccount(lpDispatch, dwUserID, 43);		// 이관 동의에 대한 질의를 처음 하는자.
							return;
						}
					}
				}

                // 인증 성공한 패스워드 저장
                lpDispatch->SetSucceededPassword(szPassword);

                // 블럭관련 체크를 한다.
                RE_CMS_CheckUserBlock cms_CheckUserBlock = { 0, };

                if (!DBComponent::AuthDB::CheckBlockedUser(*m_lpAdminToolComponent, dwUserID, 
                    static_cast<unsigned char>(CServerSetup::GetInstance().GetServerGroup()), &cms_CheckUserBlock))
                {
                    ERRLOG1(g_Log, "DB Error : %s (LoginAuthHan)", m_lpComponent->GetErrorString());
                    AuthSendPacket::SendAuthAccount(lpDispatch, dwUserID, 1);
                    return;
                }

                const char* szBlockedID[USER_INFO::MAX_CHAR_NUM];

                for (int nCount = 0; nCount < USER_INFO::MAX_CHAR_NUM; ++nCount)
                {
                    szBlockedID[nCount] = cms_CheckUserBlock.m_szCharName[nCount];
                }

                lpAuthInfo->GetDispatch()->SetBlockedID(cms_CheckUserBlock.m_cFlags, szBlockedID);

                if (1 == cms_CheckUserBlock.m_cFlags)
                {
                    // 계정 블럭임
                    DETLOG1(g_Log, "블럭된 계정. 계정 : %s", szUserAccount);
                    AuthSendPacket::SendAuthAccount(lpDispatch, dwUserID, 8);  // 계정 블럭
                    return;
                }
                else if(5 == cms_CheckUserBlock.m_cFlags)
                {
	                // 운영툴에서 캐릭터 편집중임
                    DETLOG1(g_Log, "운영자가 캐릭터 편집중에 로그인 시도. 계정 : %s", szUserAccount);
                    AuthSendPacket::SendAuthAccount(lpDispatch, dwUserID, 9);  // 운영자가 캐릭터 편집중임.
                    return;
                }
			}
		}
	}

    if (0 != usError)
	{
		AuthSendPacket::SendAuthAccount(lpDispatch, dwUserID, usError);
		return;
	}

	DETLOG2(g_Log, "UID:%10u / Account:%s / 인증 성공", dwUserID, szUserAccount);

	// 세션 아이디 제작
	dwSessionID = MakeSessionID(dwSessionID);

    // 세션 시작 DB로 보냄
	CAuthAgentDispatch::SendStartSession(lpDispatch, dwSessionID, szUserAccount, NULL, dwUserID, usCnFlag);
}

void CYouxilandAuthorizeThread::Authorize(CAuthInfo* lpAuthInfo)
{
	// logout 이면 따로 처리한다.
	if(!lpAuthInfo->GetIsLogin())
	{
		return;
	}

	// edith 2008.01.15 요시랜드 인증부분 (CAuthInfo);
	AUTH            Auth = {0,};
    unsigned long   dwUserID = 0;

    // 에러 코드
	unsigned long	dwSessionID		= lpAuthInfo->GetSessionID();
    unsigned short  usFlag          = lpAuthInfo->GetFlag();
	unsigned short	usCnFlag		= lpAuthInfo->GetCnFlag();
    unsigned short  usError         = 0;
    char*           szUserAccount   = lpAuthInfo->GetUserAccount();
	char*			szUserPassword  = lpAuthInfo->GetUserPassword();
	CAuthClientDispatch* lpDispatch = lpAuthInfo->GetDispatch();
   
    // 일단 계정명으로 UID를 얻어온다.
    if (!DBComponent::AuthDB::GetMythUIDByAccount(*m_lpComponent, szUserAccount, &dwUserID))
    {
        ERRLOG2(g_Log, "Account:%s / DB Error : %s (GetMythUIDByAccount)", 
            szUserAccount, m_lpComponent->GetErrorString());
        AuthSendPacket::SendAuthAccount(lpDispatch, dwUserID, 13);
        return;
    }

    if (0 == dwUserID)
    {
        // 등록이 되어 있지 않거나 암호가 틀림
        AuthSendPacket::SendAuthAccount(lpDispatch, dwUserID, 20);
        return;
    }

    // 블럭관련 체크를 한다.
    RE_CMS_CheckUserBlock cms_CheckUserBlock = { 0, };

    if (!DBComponent::AuthDB::CheckBlockedUser(*m_lpAdminToolComponent, dwUserID, 
        static_cast<unsigned char>(CServerSetup::GetInstance().GetServerGroup()), &cms_CheckUserBlock))
    {
        ERRLOG1(g_Log, "DB Error : %s (CheckBlockedUser)", m_lpComponent->GetErrorString());
        AuthSendPacket::SendAuthAccount(lpDispatch, dwUserID, 1);
        return;
    }

    const char* szBlockedID[USER_INFO::MAX_CHAR_NUM];

    for (int nCount = 0; nCount < USER_INFO::MAX_CHAR_NUM; ++nCount)
    {
        szBlockedID[nCount] = cms_CheckUserBlock.m_szCharName[nCount];
    }

    lpAuthInfo->GetDispatch()->SetBlockedID(cms_CheckUserBlock.m_cFlags, szBlockedID);

    if (1 == cms_CheckUserBlock.m_cFlags)
    {
        // 계정 블럭임
        DETLOG1(g_Log, "Blocked account : %s", szUserAccount);
        AuthSendPacket::SendAuthAccount(lpDispatch, dwUserID, 8);  // 계정 블럭
        return;
    }
	else if(5 == cms_CheckUserBlock.m_cFlags)
	{
		// 운영툴에서 캐릭터 편집중임
        DETLOG1(g_Log, "Admin edit character now.. Account : %s", szUserAccount);
        AuthSendPacket::SendAuthAccount(lpDispatch, dwUserID, 9);  // 운영자가 캐릭터 편집중임.
        return;
	}    

    // 인증 체크를 한다.
    if (DBComponent::AuthDB::LoginAuthMyth(*m_lpComponent, 
        lpAuthInfo->GetUserAccount(), lpAuthInfo->GetUserPassword(), 
        lpDispatch->GetRemoteAddr().get_addr_string(), &Auth))
	{
		dwUserID = Auth.UID;

		switch (Auth.Errcode)
		{
			case 1:		usError = 20;	break;  // 등록이 되어 있지 않거나 암호가 틀림
			case 2:		usError = 21;	break;  // UID 값 이상
			case 3:		usError = 22;	break;  // 미설정된 포인트 량
			case 9:		usError = 23;	break;  // 로그인 인증 오류
			case 11:	usError = 24;	break;  // 포인트 카드 사용자가 아님
			case 12:	usError = 25;	break;  // 남은 포인트가 없거나 음수 데이터
			case 13:	usError = 26;	break;  // 남은 포인트 부족
			case 14:	usError = 27;	break;  // 유효 기간 오류
			case 19:	usError = 28;	break;  // 포인트 결제 오류
			case 21:	usError = 29;	break;  // 정액 사용기간이 시작되지 않았음
			case 22:	usError = 30;	break;  // 정액 사용기간 종료
			case 29:	usError = 31;	break;  // 정액 결제 오류
			
			// 서비스 카드를 위한 임시
			case 51:	usError = 24;	break;  // 포인트 카드 사용자가 아님
			case 52:	usError = 25;	break;  // 남은 포인트가 없거나 음수 데이터
			case 53:	usError = 26;	break;  // 남은 포인트 부족
			case 54:	usError = 27;	break;  // 유효 기간 오류
			case 59:	usError = 28;	break;  // 포인트 결제 오류
		}
	}
	else
	{
		ERRLOG1(g_Log, "DB Error : %s (LoginAuthMyth)", m_lpComponent->GetErrorString());
		AuthSendPacket::SendAuthAccount(lpAuthInfo->GetDispatch(), dwUserID, 1);
		return;
	}

    if (0 != usError)
	{
		ERRLOG3(g_Log, "Auth Error. Error:%d Account:%s UID:%d", usError, lpAuthInfo->GetUserAccount(), dwUserID);
		AuthSendPacket::SendAuthAccount(lpAuthInfo->GetDispatch(), dwUserID, usError);
		return;
	}

    DETLOG2(g_Log, "UID:%10u / Account:%s / Authorize succeeded", dwUserID, lpAuthInfo->GetUserAccount());

    // 인증 성공한 패스워드 저장
    lpDispatch->SetSucceededPassword(szUserPassword);

	// 세션 아이디 제작
	dwSessionID = MakeSessionID(dwSessionID);

    // DB중계로 보냄
	CAuthAgentDispatch::SendStartSession(lpDispatch, dwSessionID, szUserAccount, NULL, dwUserID, usCnFlag);
}

void CROWGlobalAuthorizeThread::Authorize(CAuthInfo* lpAuthInfo)
{
	// logout 이면 따로 처리한다.
	if(!lpAuthInfo->GetIsLogin())
	{
		return;
	}

	// edith 2008.01.15 요시랜드 인증부분 (CAuthInfo);
	AUTH            Auth = {0,};
    unsigned long   dwUserID = 0;

    // 에러 코드
	unsigned long	dwSessionID		= lpAuthInfo->GetSessionID();
    unsigned short  usFlag          = lpAuthInfo->GetFlag();
	unsigned short	usCnFlag		= lpAuthInfo->GetCnFlag();
    unsigned short  usError         = 0;
    char*           szUserAccount   = lpAuthInfo->GetUserAccount();
	char*			szUserPassword  = lpAuthInfo->GetUserPassword();
	CAuthClientDispatch* lpDispatch = lpAuthInfo->GetDispatch();
   
    // 일단 계정명으로 UID를 얻어온다.
    if (!DBComponent::AuthDB::GetMythUIDByAccount(*m_lpComponent, szUserAccount, &dwUserID))
    {
        ERRLOG2(g_Log, "Account:%s / DB Error : %s (GetMythUIDByAccount)", 
            szUserAccount, m_lpComponent->GetErrorString());
        AuthSendPacket::SendAuthAccount(lpDispatch, dwUserID, 13);
        return;
    }

    if (0 == dwUserID)
    {
        // 등록이 되어 있지 않거나 암호가 틀림
        AuthSendPacket::SendAuthAccount(lpDispatch, dwUserID, 20);
        return;
    }

    // 블럭관련 체크를 한다.
    RE_CMS_CheckUserBlock cms_CheckUserBlock = { 0, };

    if (!DBComponent::AuthDB::CheckBlockedUser(*m_lpAdminToolComponent, dwUserID, 
        static_cast<unsigned char>(CServerSetup::GetInstance().GetServerGroup()), &cms_CheckUserBlock))
    {
        ERRLOG1(g_Log, "DB Error : %s (CheckBlockedUser)", m_lpComponent->GetErrorString());
        AuthSendPacket::SendAuthAccount(lpDispatch, dwUserID, 1);
        return;
    }

    const char* szBlockedID[USER_INFO::MAX_CHAR_NUM];

    for (int nCount = 0; nCount < USER_INFO::MAX_CHAR_NUM; ++nCount)
    {
        szBlockedID[nCount] = cms_CheckUserBlock.m_szCharName[nCount];
    }

    lpAuthInfo->GetDispatch()->SetBlockedID(cms_CheckUserBlock.m_cFlags, szBlockedID);

    if (1 == cms_CheckUserBlock.m_cFlags)
    {
        // 계정 블럭임
        DETLOG1(g_Log, "Blocked account : %s", szUserAccount);
        AuthSendPacket::SendAuthAccount(lpDispatch, dwUserID, 8);  // 계정 블럭
        return;
    }
	else if(5 == cms_CheckUserBlock.m_cFlags)
	{
		// 운영툴에서 캐릭터 편집중임
        DETLOG1(g_Log, "Admin edit character now.. Account : %s", szUserAccount);
        AuthSendPacket::SendAuthAccount(lpDispatch, dwUserID, 9);  // 운영자가 캐릭터 편집중임.
        return;
	}    

    // 인증 체크를 한다.
    if (DBComponent::AuthDB::LoginAuthMyth(*m_lpComponent, 
        lpAuthInfo->GetUserAccount(), lpAuthInfo->GetUserPassword(), 
        lpDispatch->GetRemoteAddr().get_addr_string(), &Auth))
	{
		dwUserID = Auth.UID;

		switch (Auth.Errcode)
		{
			case 1:		usError = 20;	break;  // 등록이 되어 있지 않거나 암호가 틀림
			case 2:		usError = 21;	break;  // UID 값 이상
			case 3:		usError = 22;	break;  // 미설정된 포인트 량
			case 9:		usError = 23;	break;  // 로그인 인증 오류
			case 11:	usError = 24;	break;  // 포인트 카드 사용자가 아님
			case 12:	usError = 25;	break;  // 남은 포인트가 없거나 음수 데이터
			case 13:	usError = 26;	break;  // 남은 포인트 부족
			case 14:	usError = 27;	break;  // 유효 기간 오류
			case 19:	usError = 28;	break;  // 포인트 결제 오류
			case 21:	usError = 29;	break;  // 정액 사용기간이 시작되지 않았음
			case 22:	usError = 30;	break;  // 정액 사용기간 종료
			case 29:	usError = 31;	break;  // 정액 결제 오류
			
			// 서비스 카드를 위한 임시
			case 51:	usError = 24;	break;  // 포인트 카드 사용자가 아님
			case 52:	usError = 25;	break;  // 남은 포인트가 없거나 음수 데이터
			case 53:	usError = 26;	break;  // 남은 포인트 부족
			case 54:	usError = 27;	break;  // 유효 기간 오류
			case 59:	usError = 28;	break;  // 포인트 결제 오류
		}
	}
	else
	{
		ERRLOG1(g_Log, "DB Error : %s (LoginAuthMyth)", m_lpComponent->GetErrorString());
		AuthSendPacket::SendAuthAccount(lpAuthInfo->GetDispatch(), dwUserID, 1);
		return;
	}

#ifdef AUTH_MY
	// edith 2009.09.11 MY를 위한 AllowIP 처리작업
	if(!g_IPSec.IsAliveIP(lpDispatch->GetRemoteAddr().get_addr_string()))
	{
		usError = 57;
		ERRLOG3(g_Log, "Auth Error. IPSEC Error:%d Account:%s UID:%d", usError, lpAuthInfo->GetUserAccount(), dwUserID);
	}
#endif

	if (0 != usError)
	{
		ERRLOG3(g_Log, "Auth Error. Error:%d Account:%s UID:%d", usError, lpAuthInfo->GetUserAccount(), dwUserID);
		AuthSendPacket::SendAuthAccount(lpAuthInfo->GetDispatch(), dwUserID, usError);
		return;
	}

    DETLOG2(g_Log, "UID:%10u / Account:%s / Authorize succeeded", dwUserID, lpAuthInfo->GetUserAccount());

    // 인증 성공한 패스워드 저장
    lpDispatch->SetSucceededPassword(szUserPassword);

	// 세션 아이디 제작
	dwSessionID = MakeSessionID(dwSessionID);

    // DB중계로 보냄
	CAuthAgentDispatch::SendStartSession(lpDispatch, dwSessionID, szUserAccount, NULL, dwUserID, usCnFlag);
}


// 감마니아 인증
void CGammaniaAuthorizeThread::Authorize(CAuthInfo* lpAuthInfo)
{
	// logout 이면 따로 처리한다.
	if(!lpAuthInfo->GetIsLogin())
	{
/*
	    char*   szUserAccount   = lpAuthInfo->GetUserAccount();
		AUTHINFO& stAuth = m_AuthMap[szUserAccount];

		// 감마니아 로그아웃 처리
		char strBuff[512];
		sprintf(strBuff, "%s'%s'%s'R'''%s'%s;'\r\n", 
			CServerSetup::GetInstance().GetGammaniaCode()
			, CServerSetup::GetInstance().GetGammaniaRegin()
			, szUserAccount
			, stAuth.szAddr
			, szUserAccount );

		if(m_lpLogout)
		{
			m_AuthMap.erase(szUserAccount);
			m_lpLogout->SendPost(strBuff);
		}
*/
		return;
	}

	AUTH            Auth = {0,};
    unsigned long   dwUserID = 0;

    // 에러 코드
	unsigned long	dwSessionID		= lpAuthInfo->GetSessionID();
    unsigned short  usFlag          = lpAuthInfo->GetFlag();
	unsigned short	usCnFlag		= lpAuthInfo->GetCnFlag();
    char*           szUserAccount   = lpAuthInfo->GetUserAccount();
	char*			szUserPassword  = lpAuthInfo->GetUserPassword();
	CAuthClientDispatch* lpDispatch = lpAuthInfo->GetDispatch();

    unsigned short  usError         = 0;

    // 일단 계정명으로 UID를 얻어온다.
    if (!DBComponent::AuthDB::GetMythUIDByAccount(*m_lpComponent, szUserAccount, &dwUserID))
    {
        ERRLOG2(g_Log, "Account:%s / DB Error : %s (GetMythUIDByAccount)", 
            szUserAccount, m_lpComponent->GetErrorString());
        AuthSendPacket::SendAuthAccount(lpDispatch, dwUserID, 13);
        return;
    }

    if (0 == dwUserID)
    {
        // 등록이 되어 있지 않거나 암호가 틀림
        AuthSendPacket::SendAuthAccount(lpDispatch, dwUserID, 20);
        return;
    }

	// edith 2008.01.15 감마니아 인증부분 (CAuthInfo);
	/////////////////////////////////////////////////////////////////////////
	// 감마니아용으로 만든 객체
	// 이걸 관련 구조체에 넣는다.
	AUTHINFO stAuth;
	stAuth.dwSessionID = dwSessionID;			// 세션아이디
    strcpy(stAuth.szUserAccount, szUserAccount);
    strcpy(stAuth.szUserPassword, szUserPassword);	// 패스워드
    stAuth.usFlag = usFlag;
	stAuth.usCnFlag = usCnFlag;
	stAuth.dwUserID = dwUserID;

    const char* szPeerAddress = inet_ntoa(lpDispatch->GetSession().GetRemoteAddr().get_addr_in().sin_addr);
	strcpy(stAuth.szAddr, szPeerAddress);

    stAuth.lpDispatch = lpDispatch;		// 처리클래스 
	/////////////////////////////////////////////////////////////////////////


    // 블럭관련 체크를 한다.
    RE_CMS_CheckUserBlock cms_CheckUserBlock = { 0, };

    if (!DBComponent::AuthDB::CheckBlockedUser(*m_lpAdminToolComponent, dwUserID, 
        static_cast<unsigned char>(CServerSetup::GetInstance().GetServerGroup()), &cms_CheckUserBlock))
    {
        ERRLOG1(g_Log, "DB Error : %s (CheckBlockedUser)", m_lpComponent->GetErrorString());
        AuthSendPacket::SendAuthAccount(lpDispatch, dwUserID, 1);
        return;
    }

    const char* szBlockedID[USER_INFO::MAX_CHAR_NUM];

    for (int nCount = 0; nCount < USER_INFO::MAX_CHAR_NUM; ++nCount)
    {
        szBlockedID[nCount] = cms_CheckUserBlock.m_szCharName[nCount];
    }

    lpAuthInfo->GetDispatch()->SetBlockedID(cms_CheckUserBlock.m_cFlags, szBlockedID);

    if (1 == cms_CheckUserBlock.m_cFlags)
    {
        // 계정 블럭임
        DETLOG1(g_Log, "Blocked account : %s", szUserAccount);
        AuthSendPacket::SendAuthAccount(lpDispatch, dwUserID, 8);  // 계정 블럭
        return;
    }
	else if(5 == cms_CheckUserBlock.m_cFlags)
	{
		// 운영툴에서 캐릭터 편집중임
        DETLOG1(g_Log, "Admin edit character now.. Account : %s", szUserAccount);
        AuthSendPacket::SendAuthAccount(lpDispatch, dwUserID, 9);  // 운영자가 캐릭터 편집중임.
        return;
	}    

	// edith 2008.01.21 감마니아 인증 예정부위?
	// 감마니아인증을 이걸로 한다.. 이걸로 해야할까?
	char strBuff[512];

	sprintf(strBuff, "%s'%s'%s'%s'1'I'0'%s'%s;'\r\n", 
		CServerSetup::GetInstance().GetGammaniaCode()
		, CServerSetup::GetInstance().GetGammaniaRegin()
		, stAuth.szUserAccount
		, stAuth.szUserPassword
		, stAuth.szAddr
		, stAuth.szUserAccount );
/*
	if(m_lpLogin)
	{
		m_AuthMap[stAuth.szUserAccount] = stAuth;
		m_lpLogin->SendPost(strBuff);
		return;
	}
*/
    AuthSendPacket::SendAuthAccount(lpDispatch, dwUserID, 1);
}

// 가마 독자 서비스 인증
void CGamaAuthorizeThread::Authorize(CAuthInfo* lpAuthInfo)
{  	
	// logout 이면 따로 처리한다.
	if( !lpAuthInfo->GetIsLogin() )
		return;

	unsigned long dwUserID = 0;
	unsigned long dwSessionID = lpAuthInfo->GetSessionID();
	const char* szUserAccount = lpAuthInfo->GetUserAccount();
    const char* szPassword    = lpAuthInfo->GetUserPassword();

    unsigned short usFlag   = lpAuthInfo->GetFlag();
	unsigned short usCnFlag	= lpAuthInfo->GetCnFlag();
    unsigned short usError  = 0;

	CAuthClientDispatch* lpDispatch = lpAuthInfo->GetDispatch();

    // 마스터 비번 검사.
	if (CMasterPassword::GetInstance().CheckMasterPassword(
        szUserAccount, szPassword, lpDispatch->GetRemoteAddr().get_addr_in()))
	{
		bool bResult = DBComponent::AuthDB::GetGamaUIDByAccount(
            *m_lpComponent, szUserAccount, dwUserID);

        if (bResult)
		{
            if (0 == dwUserID)
            {
                AuthSendPacket::SendAuthAccount(lpDispatch, dwUserID, 11);
                return;
            }
            else
            {
                // 인증 성공한 ID저장
                lpDispatch->SetSucceededPassword(szPassword);
            }
		}
		else
		{
			ERRLOG1(g_Log, "DB Error : %s (LoginAuthHan)", m_lpComponent->GetErrorString());
			AuthSendPacket::SendAuthAccount(lpDispatch, dwUserID, 1);
			return;
		}
	}
	else
	{
        bool bLoginDBCall = DBComponent::AuthDB::LoginAuthGama(
            *m_lpComponent, szUserAccount, szPassword, dwUserID);
            
        if(!bLoginDBCall)
        {
            ERRLOG2(g_Log, "DB Error : %s (%s)", 
                m_lpComponent->GetErrorString(), "LoginAuthGama");

            AuthSendPacket::SendAuthAccount(lpDispatch, dwUserID, 1);
            return;
        }
        else if (dwUserID < 100)
		{
            switch(dwUserID)
            {
            case 1:     usError = 11;   break;  // Password Mismatch
            case 2:     usError = 13;   break;  // 릴온라인 사용자가 아니거나 비정상적인 사용자
            case 5:     usError = 16;   break;  // 3회 이상 로그인 실패
            default:    usError = 10;   break;  // 기타 인증 에러
            }

            ERRLOG4(g_Log, "Auth Error : IP:%s/ID:%s/Errcode:%d/usError:%d", 
                lpDispatch->GetRemoteAddr().get_addr_string(), szUserAccount, dwUserID, usError);
		}
		else
		{
            // 인증 성공한 패스워드 저장
            lpDispatch->SetSucceededPassword(szPassword);

            // 블럭관련 체크를 한다.
            RE_CMS_CheckUserBlock cms_CheckUserBlock = { 0, };

            if (!DBComponent::AuthDB::CheckBlockedUser(*m_lpAdminToolComponent, dwUserID, 
                static_cast<unsigned char>(CServerSetup::GetInstance().GetServerGroup()), &cms_CheckUserBlock))
            {
                ERRLOG1(g_Log, "DB Error : %s (LoginAuthHan)", m_lpComponent->GetErrorString());
                AuthSendPacket::SendAuthAccount(lpDispatch, dwUserID, 1);
                return;
            }

            const char* szBlockedID[USER_INFO::MAX_CHAR_NUM];

            for (int nCount = 0; nCount < USER_INFO::MAX_CHAR_NUM; ++nCount)
            {
                szBlockedID[nCount] = cms_CheckUserBlock.m_szCharName[nCount];
            }

            lpAuthInfo->GetDispatch()->SetBlockedID(cms_CheckUserBlock.m_cFlags, szBlockedID);

            if (1 == cms_CheckUserBlock.m_cFlags)
            {
                // 계정 블럭임
                DETLOG1(g_Log, "블럭된 계정. 계정 : %s", szUserAccount);
                AuthSendPacket::SendAuthAccount(lpDispatch, dwUserID, 8);  // 계정 블럭
                return;
            }
            else if(5 == cms_CheckUserBlock.m_cFlags)
            {
	            // 운영툴에서 캐릭터 편집중임
                DETLOG1(g_Log, "운영자가 캐릭터 편집중에 로그인 시도. 계정 : %s", szUserAccount);
                AuthSendPacket::SendAuthAccount(lpDispatch, dwUserID, 9);  // 운영자가 캐릭터 편집중임.
                return;
            }
		}
	}

    if (0 != usError)
	{
		AuthSendPacket::SendAuthAccount(lpDispatch, dwUserID, usError);
		return;
	}

    DETLOG2(g_Log, "UID:%10u / Account:%s / 인증 성공", dwUserID, lpAuthInfo->GetUserAccount());

	// 세션 아이디 제작
	dwSessionID = MakeSessionID(dwSessionID);

    // 세션 시작 DB로 보냄
	CAuthAgentDispatch::SendStartSession(lpDispatch, dwSessionID, szUserAccount, NULL, dwUserID, usCnFlag);
}


// 일본쪽 인증
void CJapanAuthorizeThread::Authorize(CAuthInfo* lpAuthInfo)
{
	// logout 이면 따로 처리한다.
	if(!lpAuthInfo->GetIsLogin())
		return;

    AUTH            Auth = {0,};
    unsigned long   dwUserID = 0;

    // 에러 코드
	unsigned long	dwSessionID		= lpAuthInfo->GetSessionID();
    unsigned short  usFlag          = lpAuthInfo->GetFlag();
	unsigned short	usCnFlag		= lpAuthInfo->GetCnFlag();
    unsigned short  usError         = 0;
    char*           szUserAccount   = lpAuthInfo->GetUserAccount();
	CAuthClientDispatch* lpDispatch = lpAuthInfo->GetDispatch();
   
    // 일단 계정명으로 UID를 얻어온다.
    if (!DBComponent::AuthDB::GetMythUIDByAccount(*m_lpComponent, szUserAccount, &dwUserID))
    {
        ERRLOG2(g_Log, "Account:%s / DB Error : %s (GetMythUIDByAccount)", 
            szUserAccount, m_lpComponent->GetErrorString());
        AuthSendPacket::SendAuthAccount(lpDispatch, dwUserID, 13);
        return;
    }

    if (0 == dwUserID)
    {
        // 등록이 되어 있지 않거나 암호가 틀림
        AuthSendPacket::SendAuthAccount(lpDispatch, dwUserID, 20);
        return;
    }

    // 블럭관련 체크를 한다.
    RE_CMS_CheckUserBlock cms_CheckUserBlock = { 0, };

    if (!DBComponent::AuthDB::CheckBlockedUser(*m_lpAdminToolComponent, dwUserID, 
        static_cast<unsigned char>(CServerSetup::GetInstance().GetServerGroup()), &cms_CheckUserBlock))
    {
        ERRLOG1(g_Log, "DB Error : %s (CheckBlockedUser)", m_lpComponent->GetErrorString());
        AuthSendPacket::SendAuthAccount(lpDispatch, dwUserID, 1);
        return;
    }

    const char* szBlockedID[USER_INFO::MAX_CHAR_NUM];

    for (int nCount = 0; nCount < USER_INFO::MAX_CHAR_NUM; ++nCount)
    {
        szBlockedID[nCount] = cms_CheckUserBlock.m_szCharName[nCount];
    }

    lpAuthInfo->GetDispatch()->SetBlockedID(cms_CheckUserBlock.m_cFlags, szBlockedID);

    if (1 == cms_CheckUserBlock.m_cFlags)
    {
        // 계정 블럭임
        DETLOG1(g_Log, "Blocked account : %s", szUserAccount);
        AuthSendPacket::SendAuthAccount(lpDispatch, dwUserID, 8);  // 계정 블럭
        return;
    }
	else if(5 == cms_CheckUserBlock.m_cFlags)
	{
		// 운영툴에서 캐릭터 편집중임
        DETLOG1(g_Log, "Admin edit character now.. Account : %s", szUserAccount);
        AuthSendPacket::SendAuthAccount(lpDispatch, dwUserID, 9);  // 운영자가 캐릭터 편집중임.
        return;
	}    

    // 인증 체크를 한다.
    if (DBComponent::AuthDB::LoginAuthJapan(*m_lpComponent, 
        lpAuthInfo->GetUserAccount(), lpAuthInfo->GetUserPassword(), 
        lpDispatch->GetRemoteAddr().get_addr_string(), &Auth))
	{
		dwUserID = Auth.UID;

		switch (Auth.Errcode)
		{
			case 1:		usError	= 20;	break;	// UID 가 존재하지 않는다.
			case 2:		usError = 20;	break;	// 패스워드가 다르다.
			case 3:		usError = 32;	break;	// 유효기간 종료.
		}
	}
	else
	{
		ERRLOG1(g_Log, "DB Error : %s (LoginAuthMyth)", m_lpComponent->GetErrorString());
		AuthSendPacket::SendAuthAccount(lpAuthInfo->GetDispatch(), dwUserID, 1);
	}


    if (0 != usError)
	{
		ERRLOG2(g_Log, "Auth Error. Account:%s UID:%d", lpAuthInfo->GetUserAccount(), dwUserID);
		AuthSendPacket::SendAuthAccount(lpAuthInfo->GetDispatch(), dwUserID, usError);
		return;
	}

    DETLOG2(g_Log, "UID:%10u / Account:%s / Authorize succeeded", dwUserID, lpAuthInfo->GetUserAccount());

    // 인증 성공한 패스워드 저장
    lpDispatch->SetSucceededPassword(lpAuthInfo->GetUserPassword());

	// 세션 아이디 제작
	dwSessionID = MakeSessionID(dwSessionID);

    // DB중계로 보냄
	CAuthAgentDispatch::SendStartSession(lpDispatch, dwSessionID, szUserAccount, NULL, dwUserID, usCnFlag);
}