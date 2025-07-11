#include "stdafx.h"
#include "resource.h"
#include "DummyClient.h"
#include "DummyAuthHandler.h"
#include "CharacterData.h"
#include "LoginDlg.h"

#include <Network/ClientNetwork/SessionMgr.h>
#include <Network/ClientNetwork/TCPSession.h>
#include <Network/ClientNetwork/UDPSession.h>
#include <Network/ClientSocket/Send/SendEtc.h>
#include <Utility/Math/Math.h>
#include "GMMemory.h"


CCharacterData::CCharacterData()
{
    memset(m_szAccount, 0, sizeof(char) * MAX_ACCOUNT_LEN);
    memset(m_szPassword, 0, sizeof(char) * MAX_PASSWORD_LEN);
    memset(m_szCharName, 0, sizeof(char) * CHAR_INFOST::MAX_NAME_LEN);

    m_dwCheckSum = 0;
    m_dwClientVersion = 0;

    m_dwUID = 0;
    m_dwCID = 0;
    m_dwSessionID = 0;
    m_dwServerID = 0;

    m_cAgentServerType = 0;
    m_cFirstLogin = 0;
}

CCharacterData::~CCharacterData()
{

}

bool CCharacterData::Login(const char* szAuthServerIP, const char* szAccount, const char* szPassword, 
                           unsigned long dwCheckSum, unsigned long dwClientVersion)

{
    if(!m_ClientSocket.IsConnected(ClientSocket::AuthEventHandler))
    {
        // 데이터를 로컬에 세팅한 후, 연결을 시도
        m_dwCheckSum = dwCheckSum;
        m_dwClientVersion = dwClientVersion;
        
        strncpy(m_szAccount, szAccount, MAX_ACCOUNT_LEN);
        m_szAccount[MAX_ACCOUNT_LEN - 1] = 0;

        strncpy(m_szPassword, szPassword, MAX_PASSWORD_LEN);
        m_szPassword[MAX_PASSWORD_LEN - 1] = 0;

        return m_ClientSocket.Open(szAuthServerIP, ClientSocket::DEFAULT_AUTH_TCP_PORT, new CDummyAuthHandler(*this));
    }

    return false;
}

bool CCharacterData::Login(CWnd* lpParentWnd)
{
    if(!m_ClientSocket.IsConnected(ClientSocket::AuthEventHandler))
    {
        CLoginDlg loginDlg(lpParentWnd);

        if(IDOK == loginDlg.DoModal())
        {
            // 데이터를 로컬에 세팅한 후, 연결을 시도
            m_dwCheckSum = loginDlg.GetCheckSum();
            m_dwClientVersion = loginDlg.GetClientVersion();

            strncpy(m_szAccount, loginDlg.GetAccount(), MAX_ACCOUNT_LEN);
            m_szAccount[MAX_ACCOUNT_LEN - 1] = 0;

            strncpy(m_szPassword, loginDlg.GetPassword(), MAX_PASSWORD_LEN);
            m_szPassword[MAX_PASSWORD_LEN - 1] = 0;

            return m_ClientSocket.Open(loginDlg.GetAuthServerIP(),  
                ClientSocket::DEFAULT_AUTH_TCP_PORT, new CDummyAuthHandler(*this));
        }
    }

    return false;
}

void CCharacterData::SetCharView(CHAR_VIEW* lpCharView, unsigned int nViewNum)
{
    if(0 != lpCharView)
    {
		// WORK_LIST 3.2 정섭 슬롯 5개로 늘리기
        memcpy(m_charView, lpCharView, sizeof(CHAR_VIEW) * min(nViewNum, MAX_CHAR_VIEW));
    }
}

// WORK_LIST 3.1 RcaeBase 파일 합치기 및 캐릭터 선택화면 변경
void CCharacterData::SetCharGuildData(sGuildData* lpGuildData, unsigned int nNum)
{
	if(0 != lpGuildData)
	{
		memcpy(m_charGuildData, lpGuildData, sizeof(sGuildData) * min(nNum, MAX_CHAR_VIEW));
	}
}


CCharacterDataMgr::CCharacterDataMgr()
{
    m_CharDataVector.reserve(100);
}

CCharacterDataMgr::~CCharacterDataMgr()
{
    CharDataVector::iterator pos = m_CharDataVector.begin();
    CharDataVector::iterator end = m_CharDataVector.end();

    for(; pos != end; ++pos)
    {
        delete (*pos);
    }
}

// 계정명으로 추가
bool CCharacterDataMgr::AddByAccount(CCharacterData* lpCharacterData)
{
    if(0 != lpCharacterData && 0 == GetByAccount(lpCharacterData->GetAccount()))
    {
        m_CharDataVector.push_back(lpCharacterData);
        return true;
    }
    
    return false;
}

// 계정명으로 얻어옴
CCharacterData* CCharacterDataMgr::GetByAccount(const char* szAccount)
{
    CharDataVector::iterator pos = m_CharDataVector.begin();
    CharDataVector::iterator end = m_CharDataVector.end();

    for(; pos != end; ++pos)
    {
        CCharacterData* lpCharacterData = (*pos);

        if(0 == strcmp(szAccount, lpCharacterData->GetAccount()))
        {
            return *pos;
        }
    }

    return 0;
}


// UID로 얻어옴
CCharacterData* CCharacterDataMgr::GetByUID(unsigned long dwUID)
{
    CharDataVector::iterator pos = m_CharDataVector.begin();
    CharDataVector::iterator end = m_CharDataVector.end();

    for(; pos != end; ++pos)
    {
        CCharacterData* lpCharacterData = (*pos);

        if(dwUID == lpCharacterData->GetUID())
        {
            return lpCharacterData;
        }
    }

    return 0;
}

CCharacterData* CCharacterDataMgr::GetByCharName(const char* szCharName)
{
    CharDataVector::iterator pos = m_CharDataVector.begin();
    CharDataVector::iterator end = m_CharDataVector.end();

    for(; pos != end; ++pos)
    {
        CCharacterData* lpCharacterData = (*pos);

        if(0 == strcmp(lpCharacterData->GetCharacterName(), szCharName))
        {
            return lpCharacterData;
        }
    }

    return 0;
}

// 로그인한 CID로 얻어옴
CCharacterData* CCharacterDataMgr::GetByCID(unsigned long dwCID)
{
    CharDataVector::iterator pos = m_CharDataVector.begin();
    CharDataVector::iterator end = m_CharDataVector.end();

    for(; pos != end; ++pos)
    {
        CCharacterData* lpCharacterData = (*pos);

        if(dwCID == lpCharacterData->GetCID())
        {
            return lpCharacterData;
        }
    }

    return 0;
}

bool CCharacterDataMgr::RemoveByAccount(const char* szAccount)
{
    CharDataVector::iterator pos = m_CharDataVector.begin();
    CharDataVector::iterator end = m_CharDataVector.end();

    for(; pos != end; ++pos)
    {
        CCharacterData* lpCharacterData = (*pos);

        if(0 == strcmp(szAccount, lpCharacterData->GetAccount()))
        {
            m_CharDataVector.erase(pos);
            return true;
        }
    }

    return false;
}

bool CCharacterDataMgr::RemoveByUID(unsigned long dwUID)
{
    CharDataVector::iterator pos = m_CharDataVector.begin();
    CharDataVector::iterator end = m_CharDataVector.end();

    for(; pos != end; ++pos)
    {
        CCharacterData* lpCharacterData = (*pos);

        if(dwUID == lpCharacterData->GetUID())
        {
            m_CharDataVector.erase(pos);
            return true;
        }
    }

    return false;
}

bool CCharacterDataMgr::RemoveByCID(unsigned long dwCID)
{
    CharDataVector::iterator pos = m_CharDataVector.begin();
    CharDataVector::iterator end = m_CharDataVector.end();

    for(; pos != end; ++pos)
    {
        CCharacterData* lpCharacterData = (*pos);

        if(dwCID == lpCharacterData->GetCID())
        {
            m_CharDataVector.erase(pos);
            return true;
        }
    }

    return false;
}

void CCharacterDataMgr::HandleEvents(unsigned long dwProcessPerCall)
{
    CharDataVector::iterator pos = m_CharDataVector.begin();
    CharDataVector::iterator end = m_CharDataVector.end();

    for(; pos != end; ++pos)
    {
        (*pos)->GetClientSocket().HandleEvents(dwProcessPerCall);
    }
}
