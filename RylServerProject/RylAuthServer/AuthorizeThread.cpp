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
	InterlockedIncrement((LONG*)&m_bConnectAll);		// ���� ���� ���� ON !!!
	Sleep(1000);

	if (0 == m_lpComponent || 0 == m_lpAdminToolComponent)
	{
		ERRLOG0(g_Log, "DB ��ü ���� �ȵ�");
		InterlockedExchange((LONG*)&m_bConnectAll,FALSE);
		return false;
	}

	if (!m_lpComponent->Connect(CDBComponent::Class_AuthDB))
	{
		ERRLOG0(g_Log, "����DB���� ����");
		InterlockedExchange((LONG*)&m_bConnectAll,FALSE);
		return false;
	}
	else if (!m_lpAdminToolComponent->Connect(CDBComponent::Class_AdminDB))
	{
		ERRLOG0(g_Log, "���DB���� ����");
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
        ERRLOG0(g_Log, "����DB���� ����");
		return false;
    }
    else if (!m_lpAdminToolComponent->Connect(CDBComponent::Class_AdminDB))
    {
        ERRLOG0(g_Log, "���DB���� ����");
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
		// ����
		m_lpLogin = new CNetString();
		if(!m_lpLogin || !m_lpLogin->Init(GetActiveWindow(), 0, CServerSetup::GetInstance().GetGammaniaAddr(), CServerSetup::GetInstance().GammaniaPort1()))
		{
			ERRLOG0(g_Log, "�����Ͼƿ��� ����");
			return false;
		}

		m_lpLogout = new CNetString();
		if(!m_lpLogout || !m_lpLogout->Init(GetActiveWindow(), 1, CServerSetup::GetInstance().GetGammaniaAddr(), CServerSetup::GetInstance().GammaniaPort2()))
		{
			ERRLOG0(g_Log, "�����Ͼƿ��� ����");
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
		// edith 2008.01.22 �����Ͼ� �α��� ���� ó����ƾ.
		// �Ѿ�� ��Ŷ�� �Ľ��ؼ� ó���Ѵ�.
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
			// ������ ����.
			ERRLOG1(g_Log, "Gammania Error : %s (LoginAuth)", strError);

			usError = 49;
			// strError�� ���ڿ��� int�� �Ľ��ؼ� ������ �Ѱ��ְ� �ؾ��Ѵ�.
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

		// ���� ������ �н����� ����
		stAuth.lpDispatch->SetSucceededPassword(stAuth.szUserPassword);

		// ���� ���̵� ����
		stAuth.dwSessionID = MakeSessionID(stAuth.dwSessionID);

		// DB�߰�� ����
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
		// edith 2009.10.1 DB �ڵ� Ŀ��Ʈ �߰����
		// 10�ʿ� �ѹ��� ��Ʈ��ũ ������ �˻��Ѵ�.
		if( abs((__int64)timeGetTime()-lUpdateTime) > 10000 )
		{
			lUpdateTime = timeGetTime();

			// ���� ������ �������� 10���̻� �Ͼ�� DB�� �հ� ������ �ִ�.
			if(m_lpComponent && m_lpComponent->GetQueryErrorCount() >= 5)
			{
				// DB�� ���� Ŀ��Ʈ�� �õ��Ѵ�.
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

			// Logout �ÿ� �߻��Ǵ� ���������ؼ� ����
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
        // �̸� ���� �̻� / �н����� ���� �̻�(32byte MD5 hash key)
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
		// ��й�ȣ�� 128bit (32byte) MD5 ����.
		if (nAccountLen < 4 || 15 < nAccountLen || 32 != nPasswordLen)
		{
			return false;
		}
        break;

    case CServerSetup::GamaAuth:

        // �������� 6�� �̻� 12�� �̸�. ��й�ȣ�� 128bit (32byte) MD5 ����.
        if (nAccountLen < 6 || 12 < nAccountLen || 32 != nPasswordLen)
        {
            return false;
        }

        if (!isalpha(szUserAccount_In[0]))
        {
            // �������� ����� �����Ͽ��� ��.
            return false;
        }

        for (size_t nIndex = 0; nIndex < nAccountLen; ++nIndex)
        {
            if (!isalnum(szUserAccount_In[nIndex]))
            {
                // �������� ���� ��ҹ��� �� ���ڸ� ��� ����
                return false;
            }
        }

        for (size_t nIndex = 0; nIndex < nPasswordLen; ++nIndex)
        {
            if (!isalnum(szUserPW_In[nIndex]))
            {
                // MD5�� ��ȣȭ�� �н������ ���� ��ҹ��� �� ���ڸ� �����ϴ�.
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
	// logout �̸� ���� ó���Ѵ�.
	if(!lpAuthInfo->GetIsLogin())
	{
		return;
	}

  	HAN_UID hanAuth = {0,};
	unsigned long   dwUserID = 0;

    // ���� �ڵ�
	unsigned long  dwSessionID   = lpAuthInfo->GetSessionID();
	char*		   szUserAccount = lpAuthInfo->GetUserAccount();
    char*          szPassword    = lpAuthInfo->GetUserPassword();

    unsigned short usFlag        = lpAuthInfo->GetFlag();
	unsigned short usCnFlag	     = lpAuthInfo->GetCnFlag();
    unsigned short usError       = 0;

	CAuthClientDispatch* lpDispatch = lpAuthInfo->GetDispatch();

    bool bBattleAuth = CServerSetup::GetInstance().IsBattleAuthServer();

	// �� ����� ���(= ���� ù��° ��Ʈ ��), ���� ����Ʈ ���(= ���� �ι�° ��Ʈ ��) 
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
                // ���� ������ ID����
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
                case 2:     usError = 13;   break;      // �Ѱ��� ����ڰ� �ƴϰų� ���������� �����.
                case 3:     usError = 12;   break;      // �Ѱ��� ������ ����ڰ� �ƴ�
                case 4:     usError = 15;   break;      // ���� �̴�
                case 5:     usError = 16;   break;      // 3ȸ �̻� �α��� ����
                case 6:     usError = 14;   break;      // �� ��� ������ ����������� ������ �� �� ���� ����.
                default:    usError = 10;   break;      // ��Ÿ ���� ����
                }

                ERRLOG4(g_Log, "Auth Error : IP:%s/ID:%s/Errcode:%d/usError:%d", 
                    lpDispatch->GetRemoteAddr().get_addr_string(), szUserAccount, dwUserID, usError);
			}
			else
			{
				// �̰� ��û ���� üũ.
				if(!(usFlag & 0x1000))
				{
					unsigned long dwNumber_Out = 0;

					if(DBComponent::AuthDB::GetPCCheckAgreement(*m_lpComponent, szUserAccount, &dwNumber_Out))
					{
						// return value :
						//					0 : �̰� ������.
						//					1 : ���� üũ �ź��� Ȥ�� �̹� popup â�� ���� ������ �� �����.
						//					2 : �̰� ���ǿ� ���� ���Ǹ� ó�� �ϴ���.
						//					3 : ��ȸ��.

						if(dwNumber_Out==2)
						{
							AuthSendPacket::SendAuthAccount(lpDispatch, dwUserID, 43);		// �̰� ���ǿ� ���� ���Ǹ� ó�� �ϴ���.
							return;
						}
					}
				}

                // ���� ������ �н����� ����
                lpDispatch->SetSucceededPassword(szPassword);

                // ������ üũ�� �Ѵ�.
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
                    // ���� ����
                    DETLOG1(g_Log, "���� ����. ���� : %s", szUserAccount);
                    AuthSendPacket::SendAuthAccount(lpDispatch, dwUserID, 8);  // ���� ��
                    return;
                }
                else if(5 == cms_CheckUserBlock.m_cFlags)
                {
	                // ������� ĳ���� ��������
                    DETLOG1(g_Log, "��ڰ� ĳ���� �����߿� �α��� �õ�. ���� : %s", szUserAccount);
                    AuthSendPacket::SendAuthAccount(lpDispatch, dwUserID, 9);  // ��ڰ� ĳ���� ��������.
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

	DETLOG2(g_Log, "UID:%10u / Account:%s / ���� ����", dwUserID, szUserAccount);

	// ���� ���̵� ����
	dwSessionID = MakeSessionID(dwSessionID);

    // ���� ���� DB�� ����
	CAuthAgentDispatch::SendStartSession(lpDispatch, dwSessionID, szUserAccount, NULL, dwUserID, usCnFlag);
}

void CYouxilandAuthorizeThread::Authorize(CAuthInfo* lpAuthInfo)
{
	// logout �̸� ���� ó���Ѵ�.
	if(!lpAuthInfo->GetIsLogin())
	{
		return;
	}

	// edith 2008.01.15 ��÷��� �����κ� (CAuthInfo);
	AUTH            Auth = {0,};
    unsigned long   dwUserID = 0;

    // ���� �ڵ�
	unsigned long	dwSessionID		= lpAuthInfo->GetSessionID();
    unsigned short  usFlag          = lpAuthInfo->GetFlag();
	unsigned short	usCnFlag		= lpAuthInfo->GetCnFlag();
    unsigned short  usError         = 0;
    char*           szUserAccount   = lpAuthInfo->GetUserAccount();
	char*			szUserPassword  = lpAuthInfo->GetUserPassword();
	CAuthClientDispatch* lpDispatch = lpAuthInfo->GetDispatch();
   
    // �ϴ� ���������� UID�� ���´�.
    if (!DBComponent::AuthDB::GetMythUIDByAccount(*m_lpComponent, szUserAccount, &dwUserID))
    {
        ERRLOG2(g_Log, "Account:%s / DB Error : %s (GetMythUIDByAccount)", 
            szUserAccount, m_lpComponent->GetErrorString());
        AuthSendPacket::SendAuthAccount(lpDispatch, dwUserID, 13);
        return;
    }

    if (0 == dwUserID)
    {
        // ����� �Ǿ� ���� �ʰų� ��ȣ�� Ʋ��
        AuthSendPacket::SendAuthAccount(lpDispatch, dwUserID, 20);
        return;
    }

    // ������ üũ�� �Ѵ�.
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
        // ���� ����
        DETLOG1(g_Log, "Blocked account : %s", szUserAccount);
        AuthSendPacket::SendAuthAccount(lpDispatch, dwUserID, 8);  // ���� ��
        return;
    }
	else if(5 == cms_CheckUserBlock.m_cFlags)
	{
		// ������� ĳ���� ��������
        DETLOG1(g_Log, "Admin edit character now.. Account : %s", szUserAccount);
        AuthSendPacket::SendAuthAccount(lpDispatch, dwUserID, 9);  // ��ڰ� ĳ���� ��������.
        return;
	}    

    // ���� üũ�� �Ѵ�.
    if (DBComponent::AuthDB::LoginAuthMyth(*m_lpComponent, 
        lpAuthInfo->GetUserAccount(), lpAuthInfo->GetUserPassword(), 
        lpDispatch->GetRemoteAddr().get_addr_string(), &Auth))
	{
		dwUserID = Auth.UID;

		switch (Auth.Errcode)
		{
			case 1:		usError = 20;	break;  // ����� �Ǿ� ���� �ʰų� ��ȣ�� Ʋ��
			case 2:		usError = 21;	break;  // UID �� �̻�
			case 3:		usError = 22;	break;  // �̼����� ����Ʈ ��
			case 9:		usError = 23;	break;  // �α��� ���� ����
			case 11:	usError = 24;	break;  // ����Ʈ ī�� ����ڰ� �ƴ�
			case 12:	usError = 25;	break;  // ���� ����Ʈ�� ���ų� ���� ������
			case 13:	usError = 26;	break;  // ���� ����Ʈ ����
			case 14:	usError = 27;	break;  // ��ȿ �Ⱓ ����
			case 19:	usError = 28;	break;  // ����Ʈ ���� ����
			case 21:	usError = 29;	break;  // ���� ���Ⱓ�� ���۵��� �ʾ���
			case 22:	usError = 30;	break;  // ���� ���Ⱓ ����
			case 29:	usError = 31;	break;  // ���� ���� ����
			
			// ���� ī�带 ���� �ӽ�
			case 51:	usError = 24;	break;  // ����Ʈ ī�� ����ڰ� �ƴ�
			case 52:	usError = 25;	break;  // ���� ����Ʈ�� ���ų� ���� ������
			case 53:	usError = 26;	break;  // ���� ����Ʈ ����
			case 54:	usError = 27;	break;  // ��ȿ �Ⱓ ����
			case 59:	usError = 28;	break;  // ����Ʈ ���� ����
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

    // ���� ������ �н����� ����
    lpDispatch->SetSucceededPassword(szUserPassword);

	// ���� ���̵� ����
	dwSessionID = MakeSessionID(dwSessionID);

    // DB�߰�� ����
	CAuthAgentDispatch::SendStartSession(lpDispatch, dwSessionID, szUserAccount, NULL, dwUserID, usCnFlag);
}

void CROWGlobalAuthorizeThread::Authorize(CAuthInfo* lpAuthInfo)
{
	// logout �̸� ���� ó���Ѵ�.
	if(!lpAuthInfo->GetIsLogin())
	{
		return;
	}

	// edith 2008.01.15 ��÷��� �����κ� (CAuthInfo);
	AUTH            Auth = {0,};
    unsigned long   dwUserID = 0;

    // ���� �ڵ�
	unsigned long	dwSessionID		= lpAuthInfo->GetSessionID();
    unsigned short  usFlag          = lpAuthInfo->GetFlag();
	unsigned short	usCnFlag		= lpAuthInfo->GetCnFlag();
    unsigned short  usError         = 0;
    char*           szUserAccount   = lpAuthInfo->GetUserAccount();
	char*			szUserPassword  = lpAuthInfo->GetUserPassword();
	CAuthClientDispatch* lpDispatch = lpAuthInfo->GetDispatch();
   
    // �ϴ� ���������� UID�� ���´�.
    if (!DBComponent::AuthDB::GetMythUIDByAccount(*m_lpComponent, szUserAccount, &dwUserID))
    {
        ERRLOG2(g_Log, "Account:%s / DB Error : %s (GetMythUIDByAccount)", 
            szUserAccount, m_lpComponent->GetErrorString());
        AuthSendPacket::SendAuthAccount(lpDispatch, dwUserID, 13);
        return;
    }

    if (0 == dwUserID)
    {
        // ����� �Ǿ� ���� �ʰų� ��ȣ�� Ʋ��
        AuthSendPacket::SendAuthAccount(lpDispatch, dwUserID, 20);
        return;
    }

    // ������ üũ�� �Ѵ�.
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
        // ���� ����
        DETLOG1(g_Log, "Blocked account : %s", szUserAccount);
        AuthSendPacket::SendAuthAccount(lpDispatch, dwUserID, 8);  // ���� ��
        return;
    }
	else if(5 == cms_CheckUserBlock.m_cFlags)
	{
		// ������� ĳ���� ��������
        DETLOG1(g_Log, "Admin edit character now.. Account : %s", szUserAccount);
        AuthSendPacket::SendAuthAccount(lpDispatch, dwUserID, 9);  // ��ڰ� ĳ���� ��������.
        return;
	}    

    // ���� üũ�� �Ѵ�.
    if (DBComponent::AuthDB::LoginAuthMyth(*m_lpComponent, 
        lpAuthInfo->GetUserAccount(), lpAuthInfo->GetUserPassword(), 
        lpDispatch->GetRemoteAddr().get_addr_string(), &Auth))
	{
		dwUserID = Auth.UID;

		switch (Auth.Errcode)
		{
			case 1:		usError = 20;	break;  // ����� �Ǿ� ���� �ʰų� ��ȣ�� Ʋ��
			case 2:		usError = 21;	break;  // UID �� �̻�
			case 3:		usError = 22;	break;  // �̼����� ����Ʈ ��
			case 9:		usError = 23;	break;  // �α��� ���� ����
			case 11:	usError = 24;	break;  // ����Ʈ ī�� ����ڰ� �ƴ�
			case 12:	usError = 25;	break;  // ���� ����Ʈ�� ���ų� ���� ������
			case 13:	usError = 26;	break;  // ���� ����Ʈ ����
			case 14:	usError = 27;	break;  // ��ȿ �Ⱓ ����
			case 19:	usError = 28;	break;  // ����Ʈ ���� ����
			case 21:	usError = 29;	break;  // ���� ���Ⱓ�� ���۵��� �ʾ���
			case 22:	usError = 30;	break;  // ���� ���Ⱓ ����
			case 29:	usError = 31;	break;  // ���� ���� ����
			
			// ���� ī�带 ���� �ӽ�
			case 51:	usError = 24;	break;  // ����Ʈ ī�� ����ڰ� �ƴ�
			case 52:	usError = 25;	break;  // ���� ����Ʈ�� ���ų� ���� ������
			case 53:	usError = 26;	break;  // ���� ����Ʈ ����
			case 54:	usError = 27;	break;  // ��ȿ �Ⱓ ����
			case 59:	usError = 28;	break;  // ����Ʈ ���� ����
		}
	}
	else
	{
		ERRLOG1(g_Log, "DB Error : %s (LoginAuthMyth)", m_lpComponent->GetErrorString());
		AuthSendPacket::SendAuthAccount(lpAuthInfo->GetDispatch(), dwUserID, 1);
		return;
	}

#ifdef AUTH_MY
	// edith 2009.09.11 MY�� ���� AllowIP ó���۾�
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

    // ���� ������ �н����� ����
    lpDispatch->SetSucceededPassword(szUserPassword);

	// ���� ���̵� ����
	dwSessionID = MakeSessionID(dwSessionID);

    // DB�߰�� ����
	CAuthAgentDispatch::SendStartSession(lpDispatch, dwSessionID, szUserAccount, NULL, dwUserID, usCnFlag);
}


// �����Ͼ� ����
void CGammaniaAuthorizeThread::Authorize(CAuthInfo* lpAuthInfo)
{
	// logout �̸� ���� ó���Ѵ�.
	if(!lpAuthInfo->GetIsLogin())
	{
/*
	    char*   szUserAccount   = lpAuthInfo->GetUserAccount();
		AUTHINFO& stAuth = m_AuthMap[szUserAccount];

		// �����Ͼ� �α׾ƿ� ó��
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

    // ���� �ڵ�
	unsigned long	dwSessionID		= lpAuthInfo->GetSessionID();
    unsigned short  usFlag          = lpAuthInfo->GetFlag();
	unsigned short	usCnFlag		= lpAuthInfo->GetCnFlag();
    char*           szUserAccount   = lpAuthInfo->GetUserAccount();
	char*			szUserPassword  = lpAuthInfo->GetUserPassword();
	CAuthClientDispatch* lpDispatch = lpAuthInfo->GetDispatch();

    unsigned short  usError         = 0;

    // �ϴ� ���������� UID�� ���´�.
    if (!DBComponent::AuthDB::GetMythUIDByAccount(*m_lpComponent, szUserAccount, &dwUserID))
    {
        ERRLOG2(g_Log, "Account:%s / DB Error : %s (GetMythUIDByAccount)", 
            szUserAccount, m_lpComponent->GetErrorString());
        AuthSendPacket::SendAuthAccount(lpDispatch, dwUserID, 13);
        return;
    }

    if (0 == dwUserID)
    {
        // ����� �Ǿ� ���� �ʰų� ��ȣ�� Ʋ��
        AuthSendPacket::SendAuthAccount(lpDispatch, dwUserID, 20);
        return;
    }

	// edith 2008.01.15 �����Ͼ� �����κ� (CAuthInfo);
	/////////////////////////////////////////////////////////////////////////
	// �����Ͼƿ����� ���� ��ü
	// �̰� ���� ����ü�� �ִ´�.
	AUTHINFO stAuth;
	stAuth.dwSessionID = dwSessionID;			// ���Ǿ��̵�
    strcpy(stAuth.szUserAccount, szUserAccount);
    strcpy(stAuth.szUserPassword, szUserPassword);	// �н�����
    stAuth.usFlag = usFlag;
	stAuth.usCnFlag = usCnFlag;
	stAuth.dwUserID = dwUserID;

    const char* szPeerAddress = inet_ntoa(lpDispatch->GetSession().GetRemoteAddr().get_addr_in().sin_addr);
	strcpy(stAuth.szAddr, szPeerAddress);

    stAuth.lpDispatch = lpDispatch;		// ó��Ŭ���� 
	/////////////////////////////////////////////////////////////////////////


    // ������ üũ�� �Ѵ�.
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
        // ���� ����
        DETLOG1(g_Log, "Blocked account : %s", szUserAccount);
        AuthSendPacket::SendAuthAccount(lpDispatch, dwUserID, 8);  // ���� ��
        return;
    }
	else if(5 == cms_CheckUserBlock.m_cFlags)
	{
		// ������� ĳ���� ��������
        DETLOG1(g_Log, "Admin edit character now.. Account : %s", szUserAccount);
        AuthSendPacket::SendAuthAccount(lpDispatch, dwUserID, 9);  // ��ڰ� ĳ���� ��������.
        return;
	}    

	// edith 2008.01.21 �����Ͼ� ���� ��������?
	// �����Ͼ������� �̰ɷ� �Ѵ�.. �̰ɷ� �ؾ��ұ�?
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

// ���� ���� ���� ����
void CGamaAuthorizeThread::Authorize(CAuthInfo* lpAuthInfo)
{  	
	// logout �̸� ���� ó���Ѵ�.
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

    // ������ ��� �˻�.
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
                // ���� ������ ID����
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
            case 2:     usError = 13;   break;  // ���¶��� ����ڰ� �ƴϰų� ���������� �����
            case 5:     usError = 16;   break;  // 3ȸ �̻� �α��� ����
            default:    usError = 10;   break;  // ��Ÿ ���� ����
            }

            ERRLOG4(g_Log, "Auth Error : IP:%s/ID:%s/Errcode:%d/usError:%d", 
                lpDispatch->GetRemoteAddr().get_addr_string(), szUserAccount, dwUserID, usError);
		}
		else
		{
            // ���� ������ �н����� ����
            lpDispatch->SetSucceededPassword(szPassword);

            // ������ üũ�� �Ѵ�.
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
                // ���� ����
                DETLOG1(g_Log, "���� ����. ���� : %s", szUserAccount);
                AuthSendPacket::SendAuthAccount(lpDispatch, dwUserID, 8);  // ���� ��
                return;
            }
            else if(5 == cms_CheckUserBlock.m_cFlags)
            {
	            // ������� ĳ���� ��������
                DETLOG1(g_Log, "��ڰ� ĳ���� �����߿� �α��� �õ�. ���� : %s", szUserAccount);
                AuthSendPacket::SendAuthAccount(lpDispatch, dwUserID, 9);  // ��ڰ� ĳ���� ��������.
                return;
            }
		}
	}

    if (0 != usError)
	{
		AuthSendPacket::SendAuthAccount(lpDispatch, dwUserID, usError);
		return;
	}

    DETLOG2(g_Log, "UID:%10u / Account:%s / ���� ����", dwUserID, lpAuthInfo->GetUserAccount());

	// ���� ���̵� ����
	dwSessionID = MakeSessionID(dwSessionID);

    // ���� ���� DB�� ����
	CAuthAgentDispatch::SendStartSession(lpDispatch, dwSessionID, szUserAccount, NULL, dwUserID, usCnFlag);
}


// �Ϻ��� ����
void CJapanAuthorizeThread::Authorize(CAuthInfo* lpAuthInfo)
{
	// logout �̸� ���� ó���Ѵ�.
	if(!lpAuthInfo->GetIsLogin())
		return;

    AUTH            Auth = {0,};
    unsigned long   dwUserID = 0;

    // ���� �ڵ�
	unsigned long	dwSessionID		= lpAuthInfo->GetSessionID();
    unsigned short  usFlag          = lpAuthInfo->GetFlag();
	unsigned short	usCnFlag		= lpAuthInfo->GetCnFlag();
    unsigned short  usError         = 0;
    char*           szUserAccount   = lpAuthInfo->GetUserAccount();
	CAuthClientDispatch* lpDispatch = lpAuthInfo->GetDispatch();
   
    // �ϴ� ���������� UID�� ���´�.
    if (!DBComponent::AuthDB::GetMythUIDByAccount(*m_lpComponent, szUserAccount, &dwUserID))
    {
        ERRLOG2(g_Log, "Account:%s / DB Error : %s (GetMythUIDByAccount)", 
            szUserAccount, m_lpComponent->GetErrorString());
        AuthSendPacket::SendAuthAccount(lpDispatch, dwUserID, 13);
        return;
    }

    if (0 == dwUserID)
    {
        // ����� �Ǿ� ���� �ʰų� ��ȣ�� Ʋ��
        AuthSendPacket::SendAuthAccount(lpDispatch, dwUserID, 20);
        return;
    }

    // ������ üũ�� �Ѵ�.
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
        // ���� ����
        DETLOG1(g_Log, "Blocked account : %s", szUserAccount);
        AuthSendPacket::SendAuthAccount(lpDispatch, dwUserID, 8);  // ���� ��
        return;
    }
	else if(5 == cms_CheckUserBlock.m_cFlags)
	{
		// ������� ĳ���� ��������
        DETLOG1(g_Log, "Admin edit character now.. Account : %s", szUserAccount);
        AuthSendPacket::SendAuthAccount(lpDispatch, dwUserID, 9);  // ��ڰ� ĳ���� ��������.
        return;
	}    

    // ���� üũ�� �Ѵ�.
    if (DBComponent::AuthDB::LoginAuthJapan(*m_lpComponent, 
        lpAuthInfo->GetUserAccount(), lpAuthInfo->GetUserPassword(), 
        lpDispatch->GetRemoteAddr().get_addr_string(), &Auth))
	{
		dwUserID = Auth.UID;

		switch (Auth.Errcode)
		{
			case 1:		usError	= 20;	break;	// UID �� �������� �ʴ´�.
			case 2:		usError = 20;	break;	// �н����尡 �ٸ���.
			case 3:		usError = 32;	break;	// ��ȿ�Ⱓ ����.
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

    // ���� ������ �н����� ����
    lpDispatch->SetSucceededPassword(lpAuthInfo->GetUserPassword());

	// ���� ���̵� ����
	dwSessionID = MakeSessionID(dwSessionID);

    // DB�߰�� ����
	CAuthAgentDispatch::SendStartSession(lpDispatch, dwSessionID, szUserAccount, NULL, dwUserID, usCnFlag);
}