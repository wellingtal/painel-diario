#ifndef _AUTH_CLIENT_DISPATCH_H_
#define _AUTH_CLIENT_DISPATCH_H_

#include <mmsystem.h>
#include <algorithm>

#include <DB/DBDefine.h>
#include <Network/Dispatch/CheckPing.h>
#include <Network/Dispatch/MultiThreadDispatch.h>
#include <Network/Dispatch/MultiDispatchStorage.h>
#include <Network/Address/INET_Addr.h>

// 전방 참조
struct PktAU;
struct PktULi;
struct PktSPI;

struct PktCS;
struct PktCC;
struct PktCD;
// WORK_LIST 2.1 계정 국적 추가
struct PktSelectAccountNation;

struct PktUnifiedCharSelectReq;

struct PktSZ;
struct PktJPAU;

struct CHAR_VIEW;

// 1유저당 CAuthClientDispatch 1클래스 배치
class CAuthClientDispatch : public CRylServerDispatch
{
public:

    enum Const
    {   
		// edith 2008.03.17 ID,PASS 길이조정
        MAX_PASSWORD_LEN    = 36,

	    PENDED_CHAR_SELECT      = (1 << 0),
        CHAR_SELECT_SUCCESS     = (1 << 1),
        
	    PENDED_SERVER_ZONE      = (1 << 2),
        SERVER_ZONE_SUCCESS     = (1 << 3),

        AUTHORIZE_PENDED        = (1 << 4),
        AUTHORIZED_SUCCESS      = (1 << 5),
        PENDED_CHAR_CREATE      = (1 << 6),
        PENDED_CHAR_DELETE      = (1 << 7)
    };

	// Do not make inline function : 인라인 함수로 만들지 마시오. 대략 낭패.
	static CMultiDispatch& GetDispatchTable();

	CAuthClientDispatch(CSession& Session);
	virtual ~CAuthClientDispatch();

	virtual void Connected();
    virtual void Disconnected();

    virtual bool Dispatch();

	CHAR_VIEW&	GetCharacterView(unsigned long Slot_In)	{ return m_CharView[Slot_In]; }
    void		SetCharacterView(CHAR_VIEW* lpCharView) { memcpy(m_CharView, lpCharView, sizeof(CHAR_VIEW) * USER_INFO::MAX_CHAR_NUM); }

	unsigned long GetUID() const		{ return m_dwUID;		}
	unsigned long GetSessionID() const	{ return m_dwSessionID;	}

    unsigned char GetAgentServerType() const { return m_cAgentServerType; }
    unsigned char GetFirstLogin() const { return m_cFirstLogin; }

	void UserLogin(unsigned long dwSessionID, unsigned long dwUID);
	void UserLogout(void);
	void UserMove(void);

    void SetBlockedID(unsigned char cBlockedFlag, const char* szBlockedChar[USER_INFO::MAX_CHAR_NUM]);
    void SetSucceededPassword(const char* szPassword) 
    {
		ZeroMemory(m_szSucceededPassword, sizeof(char) * MAX_PASSWORD_LEN);

		if(0 != szPassword && strlen(szPassword) > 0)
		{
			strcpy(m_szSucceededPassword, szPassword);
		}
    }

    void SetUnifiedInfo(unsigned char cAgentServerType, unsigned char cFirstLogin)
    {
        m_cAgentServerType = cAgentServerType;
        m_cFirstLogin = cFirstLogin;
    }

	void SetCnFlag(unsigned short CnFlag)	{ m_CnFlag = CnFlag; }
	unsigned short GetCnFlag(void)			{ return m_CnFlag;   }

    void SetPacketProcessFlag(unsigned short usFlags) { m_usPacketProcessFlag |= usFlags; }
    void ResetPacketProcessFlag(unsigned short usFlags) { m_usPacketProcessFlag &= ~usFlags; }
    bool IsSetPacketProcessFlag(unsigned short usFlags) { return (usFlags == (m_usPacketProcessFlag & usFlags)); }


    void SetMoveAddress(unsigned long dwMoveServerID, const INET_Addr& moveAddress) 
    {
        m_dwMoveServerID = dwMoveServerID;
        m_MoveAddress = moveAddress;
    }

    INET_Addr& GetMoveAddress() { return m_MoveAddress; }
    unsigned long GetMoveServerID() { return m_dwMoveServerID; }


     // Send Functions
	static bool SendCharSelect(CSendStream& SendStream,  unsigned char Zone, 
		unsigned short *lpChannelNum, unsigned short usError_In);

    static bool SendCharCreate(CSendStream& SendStream, unsigned long CharID_In, unsigned long Slot_In, 
		CHAR_VIEW* lpCharView_In, unsigned short usError_In);

    static bool SendCharDelete(CSendStream& SendStream, unsigned long UserID_In, unsigned long Slot_In, 
        unsigned short usError_In);

	// WORK_LIST 2.1 계정 국적 추가
	static bool SendSelectAccountNation(CSendStream& SendStream, unsigned long UserID_In,
		unsigned char cType, unsigned char cAccountNation_In, unsigned short usError_In);

	// WORK_LIST 2.3 계정 국적 변경 기능 구현
	static bool SendNationChangeResult(CSendStream& SendStream, unsigned long UserID_In,
		unsigned long* dwGID, unsigned long* dwFame, unsigned short usError_In);

	inline STORE_INFO* GetStoreInfo() { return &m_sStoreInfo; }

private:

    // Dispatch Functions
    virtual bool DispatchPacket(PktBase* lpPktBase);
        
    bool ParseAuthAccount(PktAU* lpPktAU);
    bool ParseUserLogin(PktULi* lpPktULi);
    bool ParseSysPatchAddress(PktSPI* lpPktSPI);    // 패치 서버 주소를 클라이언트에게 보내준다.

    bool ParseCharSelect(PktCS* lpPktCS);
    bool ParseCharCreate(PktCC* lpPktCC);
    bool ParseCharDelete(PktCD* lpPktCD);

	// WORK_LIST 2.1 계정 국적 추가
	bool ParseSelectAccountNation(PktSelectAccountNation* lpPktSAN_In);

	bool ParseServerZone(PktSZ* lpPktSZ);

    bool ParseJapanAuthAccount(PktJPAU* lpPktAU);

    bool ParseUnifiedCharSelect(PktUnifiedCharSelectReq* lpPktUnifiedCharSelectReq);


	// --------------------------------------------------------------------------------------------------
	// 캐릭터 포인터 Get, Set 

    CCheckPing  m_CheckPing;
    CHAR_VIEW	m_CharView[USER_INFO::MAX_CHAR_NUM];

	// --------------------------------------------------------------------------------------------------
	// 유저 데이터

    unsigned long	m_dwSessionID;
	unsigned long	m_dwUID;

    char            m_szBlockedID[USER_INFO::MAX_CHAR_NUM][CHAR_INFOST::MAX_NAME_LEN];
    char            m_szAccountName[CHAR_INFOST::MAX_ACCOUNT_LEN];
    char            m_szSucceededPassword[MAX_PASSWORD_LEN];
    unsigned char   m_cBlockedFlag;			// 0 : 블록x, 1 : 계정 블록, 2 : 캐릭터 블록
	unsigned short	m_usPacketProcessFlag;
    unsigned short	m_CnFlag;

    unsigned char   m_cAgentServerType;
    unsigned char   m_cFirstLogin;    

    INET_Addr       m_MoveAddress;
    unsigned long   m_dwMoveServerID;

	
	STORE_INFO		m_sStoreInfo;
};

#endif