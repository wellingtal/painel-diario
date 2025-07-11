#ifndef _RYL_DUMMYCLIENT_CHARACTER_DATA_H_
#define _RYL_DUMMYCLIENT_CHARACTER_DATA_H_

#pragma once

#include <DB/DBdefine.h>
#include <Network/ClientSocket/ClientSocket.h>

namespace ClientNet
{
    class CSessionMgr;
}

class CDummyClientSocket : public ClientSocket
{
public:

    CDummyClientSocket() { }
    virtual ~CDummyClientSocket() { }
};

class CCharacterData
{
public:

    enum
    {
        MAX_ACCOUNT_LEN     = 16,
        MAX_PASSWORD_LEN    = 33,

		MAX_CHAR_VIEW		= 5			// WORK_LIST 3.2 정섭 슬롯 5개로 늘리기
    };

    enum SessionType
    {
        AUTH_SESSION    = 0,
        GAME_SESSION    = 1,
        CHAT_SESSION    = 2,
        UDP_SESSION     = 3,
        MAX_SESSION     = 4    
    };

    CCharacterData();
    ~CCharacterData();

    bool Login(const char* szAuthServerIP, const char* szAccount, const char* szPassword, 
        unsigned long dwCheckSum, unsigned long dwClientVersion);

    bool Login(CWnd* lpParentWnd);

    ClientSocket& GetClientSocket() { return m_ClientSocket; }
    
    char* GetAccount()  { return m_szAccount;  }
    char* GetPassword() { return m_szPassword; }
    
    unsigned long GetCheckSum() const       { return m_dwCheckSum;      }
    unsigned long GetClientVersion() const  { return m_dwClientVersion; }

    unsigned long GetUID() const { return m_dwUID; }
    unsigned long GetCID() const { return m_dwCID; }
    unsigned long GetSessionID() const { return m_dwSessionID; }
    unsigned long GetServerID() const  { return m_dwServerID;  }
    const char*   GetCharacterName() const { return m_szCharName; }

    void SetUID(unsigned long dwUID) { m_dwUID = dwUID; }
    void SetCID(unsigned long dwCID) { m_dwCID = dwCID; }   // SendPacket::CharSelect전에, CID세팅 필요.
    void SetCharView(CHAR_VIEW* lpCharView, unsigned int nViewNum);
	// WORK_LIST 3.1 RcaeBase 파일 합치기 및 캐릭터 선택화면 변경
	void SetCharGuildData(sGuildData* lpGuildData, unsigned int nNum);
    void SetSessionID(unsigned long dwSessionID) { m_dwSessionID = dwSessionID; }
    void SetServerID(unsigned long dwServerID)   { m_dwServerID  = dwServerID;  }
    void SetCharacterName(const char* szCharName)
    {
        strncpy(m_szCharName, szCharName, CHAR_INFOST::MAX_NAME_LEN);
        m_szCharName[CHAR_INFOST::MAX_NAME_LEN - 1] = 0;
    }

    unsigned char GetAgentServerType() const { return m_cAgentServerType; }
    unsigned char GetFirstLogin() const { return m_cFirstLogin; }

    void SetUnifiedLoginInfo(unsigned char cAgentServerType, unsigned char cFirstLogin)
    {
        m_cAgentServerType = cAgentServerType;
        m_cFirstLogin = cFirstLogin;
    }

private:

    char                m_szAccount[MAX_ACCOUNT_LEN];
    char                m_szPassword[MAX_PASSWORD_LEN];
    char                m_szCharName[CHAR_INFOST::MAX_NAME_LEN];

    unsigned long       m_dwCheckSum;
    unsigned long       m_dwClientVersion;

    unsigned long       m_dwUID;
    unsigned long       m_dwCID;
    unsigned long       m_dwSessionID;
    unsigned long       m_dwServerID;

    

	// WORK_LIST 3.1 RcaeBase 파일 합치기 및 캐릭터 선택화면 변경
	// WORK_LIST 3.2 정섭 슬롯 5개로 늘리기
    CHAR_VIEW           m_charView[MAX_CHAR_VIEW];
	sGuildData		    m_charGuildData[MAX_CHAR_VIEW];
    CDummyClientSocket  m_ClientSocket;

    unsigned char       m_cAgentServerType;
    unsigned char       m_cFirstLogin;
};


class CCharacterDataMgr
{
public:

    CCharacterDataMgr();
    ~CCharacterDataMgr();

    bool AddByAccount(CCharacterData* lpCharacterData);      // 추가
    CCharacterData* GetByAccount(const char* szAccount);     // 계정명으로 얻어옴
    CCharacterData* GetByCharName(const char* szCharName);   // 캐릭터 이름으로 얻어옴
    CCharacterData* GetByUID(unsigned long dwUID);  // UID로 얻어옴
    CCharacterData* GetByCID(unsigned long dwCID);  // 로그인한 CID로 얻어옴

    bool RemoveByAccount(const char* szAccount);
    bool RemoveByUID(unsigned long dwUID);
    bool RemoveByCID(unsigned long dwCID);

    void HandleEvents(unsigned long dwProcessPerCall);

private:
    
    typedef std::vector<CCharacterData*>    CharDataVector;
    CharDataVector m_CharDataVector;
};

#endif