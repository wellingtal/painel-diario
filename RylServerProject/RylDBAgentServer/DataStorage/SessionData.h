#ifndef _RYL_DBAGENT_SESSION_DATA_H_
#define _RYL_DBAGENT_SESSION_DATA_H_

#include <DB/DBDefine.h>
#include <Creature/Character/CharacterClass.h>
#include <Network/Packet/PacketStruct/UnifiedCharPacket.h>

#include "Billing.h"

#include <deque>

//! forward decl.
class CDBComponent;

namespace DBAgent
{
namespace DataStorage
{
    //! forward decl.
    class CCharacterData;
    class CStoreData;

    struct RequestData
    {
		enum 
		{ 
			// edith 2008.03.17 ID,PASS 길이조정
			MaxName = 24,
			MaxPass = 36,
		};

        unsigned long   m_dwRequestKey;
        unsigned long   m_dwSelectedCID;
        unsigned long   m_dwServerID;
        unsigned long   m_dwSessionID;

        IN_ADDR         m_PeerAddress;
        char            m_szAccount[MaxName];

		// edith 2008.01.15 패스워드 추가작업
        char            m_szPassword[MaxPass];

        RequestData();

		// edith 2008.01.15 패스워드 추가작업
        RequestData(unsigned long dwRequestKey, unsigned long dwSelectedCID, 
            unsigned long dwServerID, unsigned long dwSessionID,
            IN_ADDR peerAddr, const char* szAccountName, const char* szPassword);
    };

    class CSessionData
    {
    public:

        enum SessionState
        {
            SE_NONE             = 0,
            SE_USER_ENABLED     = 1,
            SE_USER_DISABLED    = 2,
            SE_MOVE             = 3,
            SE_CHAR_ENABLED     = 4,
            SE_CHAR_DISABLED    = 5,
            SE_MAX_STATE        = 6
        };

        enum
        {
			MAX_REALM_POINT					= 5,					// 마땅히 넣을곳이 없어서 ㅠ,.ㅠ by Minbobo.

			REALM_POINT_TIME				= 60,					// 60 분

			FAME_PANELTY_VALUE_1ST			= 5000,
			FAME_PANELTY_VALUE_2ND			= 10000,
        };

		struct SaveEnemy
		{
			enum
			{
				MAX_SAVING_ENEMY			= 5,		// 기억하는 최대 개체 수
			};
			

			unsigned long	m_dwCID;
			unsigned long	m_dwTick;
		};

        typedef std::deque<RequestData>     RequestQueue;

        CSessionData();
        ~CSessionData();


        //! 세션 기본 데이터.
        unsigned long GetUID() const { return m_dwUID; }
        unsigned long GetCID() const { return m_dwCID; }
        unsigned long GetSessionID() const { return m_dwSessionID; }
        unsigned long GetServerID() const  { return m_dwServerID;  }
        IN_ADDR GetRemoteAddress() const { return m_RemoteAddress; }
        const char* GetAccountName() const { return m_szAccount; }
        unsigned char GetAdminLevel() const { return m_cAdminLevel; }
		// WORK_LIST 2.1 계정 국적 추가
		unsigned char GetAccountNation() const { return m_UserInfo.Nation;	}
		void SetAccountNation(unsigned char cNation) { m_UserInfo.Nation = cNation; }

        const USER_INFO& GetUserInfo() const { return m_UserInfo; }
        const CHAR_VIEW* GetCharView(unsigned char cIndex) const 
        { return (cIndex < USER_INFO::MAX_CHAR_NUM) ? m_CharView + cIndex : 0; }

        bool GetCharView(unsigned long dwCID, CHAR_VIEW& charView_Out);
        
		// WORK_LIST 2.3 계정 국적 변경 기능 구현

		bool SetCharViewGID(unsigned long dwCID, unsigned long dwGID);
		bool SetCharViewFame(unsigned long dwCID, unsigned long dwFame);

        CBilling& GetBilling() { return m_Billing; }

        bool HasCharacter(unsigned long dwCID) const; 

        //! 세션 상태 얻어오기 및, 세션 상태 전환 함수
        SessionState GetSessionState() const { return m_eSessionState; }
        void SetSessionState(SessionState eSessionState) { m_eSessionState = eSessionState; }

        //! 인증서버 ID로 세팅
        void SetAuthServerID();
        void SetAccountName(const char* szAccountName);

        //! 유저 Enable/Disable/Move
        bool UserEnable(CDBComponent& DBComponent, unsigned long dwUID);
        bool UserDisable();
        bool UserMove();

        //! 캐릭터 Enable/Disable/Move
        bool LoadData(unsigned long dwCID);
        void UnloadData();

        bool CharEnable(unsigned long dwCID, unsigned long dwServerID);
        bool CharDisable(unsigned long dwCID, unsigned long dwServerID);
        bool CharMove(unsigned long dwCID, unsigned long dwServerID);
               
        //! 세션 연동된 데이터. (캐릭터)
        CCharacterData* GetCharacterData()	{ return m_lpCharacterData; }
        CStoreData*     GetStoreData()		{ return m_lpStoreData; }

		SaveEnemy*		GetSaveEnemy()		{ return m_SaveEnemy; }

        //! 세션 연동된 데이터. (창고)
        void SetCharacterData(CCharacterData* lpCharacterData) { m_lpCharacterData = lpCharacterData; }
        void SetStoreData(CStoreData* lpStoreData) { m_lpStoreData = lpStoreData; }

        //! 존 이동 관련 데이터        
        bool SetMoveZone(unsigned char cZone, const POS& Pos);
        void UpdateMoveZone();			
        bool CheckCharZone(unsigned char cZone);

        unsigned char GetCharStartZone(char cRace);
        
		bool GetPlayTimeFromDB(CDBComponent& DBComponent);
		bool GetPremiumServiceFromDB(CDBComponent& DBComponent);

		bool GetUserInfoFromDB(CDBComponent& DBComponent);      //! DB로부터 유저 정보 및 뷰를 얻어온다.
        bool GetUnifiedDataFromDB(CDBComponent& DBComponent);   //! DB로부터 통합 유저 정보를 얻어온다.
        
        //! 데이터 로그 관련 메소드
        void LogUserData(const char* szMessage);
        void LogCharData(const char* szMessage);
        void LogUserDataError(const char* szMessage, const char* szRoutine, const char* szFile, int nLine);
        void LogCharDataError(const char* szMessage, const char* szRoutine, const char* szFile, int nLine);

        //! 데이터 세팅 관련 메소드
        void SetUID(unsigned long dwUID) { m_dwUID = dwUID; }
        void SetAddress(IN_ADDR addr) { m_RemoteAddress = addr; }

        //! 요청 큐 관련 (dwRequestKey가 0이면 에러)
        void PushRequest(const RequestData& reqData);
        RequestData PopRequest();

        void SetRequestData(const RequestData& reqData);

        //! 서버통합 관련 데이터
        unsigned char GetOldServerGroupID() const { return m_cOldServerGroupID; }
        void SetOldServerGroupID(unsigned char cOldServerGroupID) { m_cOldServerGroupID = cOldServerGroupID; }

        const UnifiedStoreInfo* GetUnifiedStoreInfo() const { return m_UnifiedStoreInfo; }
        const UnifiedCharData*  GetUnifiedCharData() const  { return m_UnifiedCharData; }
        const UnifiedCharData*  GetUnifiedCharData(unsigned long dwCID) const;

        int GetUnifiedStoreInfoNum() const { return m_nUnifiedStoreInfoNum; }
        int GetUnifiedCharDataNum() const { return m_nUnifiedCharDataNum; }

        // 창고 선택 / 처음 로그인 여부 등 정보를 담고 있는 데이터
        unsigned char GetFirstLogin() const { return m_UserInfo.FirstLogin; }
        void SetFirstLogin(unsigned char cFirstLogin) { m_UserInfo.FirstLogin = cFirstLogin; }

        // 해당하는 서버군 번호의 창고가 있는지 살핀다.
        bool HasUnifiedStoreInfo(unsigned char cOldServerGroupID);

        // 해당하는 서버군 번호의 창고의 서버군 번호를 바꾼다.
        bool ChangeUnifiedStoreInfoGroup(CDBComponent& DBComponent, 
            unsigned char cOldServerGroupID, unsigned char cNewServerGroupID);

        // UID/CID가 일치하는 캐릭터의 서버그룹ID를 바꾼다.
        bool ChangeUnifiedCharServerGroupID(CDBComponent& DBComponent,
            unsigned long dwUID, unsigned long dwCID, unsigned char cOldServerGroupID);

        // 해당하는 통합서버 캐릭터 정보를 제거한다(캐릭터 삭제시 사용)
        bool DelUnifiedCharData(unsigned long dwCID);

        // 캐릭터 바인딩을 수정한다.
        bool UpdateCharacterBinding(CDBComponent& DBComponent, 
            unsigned long dwSelectedCID[USER_INFO::MAX_CHAR_NUM]);      

        // 캐릭터 CID를 주면 선택된 종족을 돌려준다.
        CClass::RaceType CheckUnifiedCharRace(unsigned long dwSelectedCID[USER_INFO::MAX_CHAR_NUM]);

        // Part1 -> Part2 캐릭터 이전 회수를 제어한다.
        unsigned char GetTransferedCharCount(CDBComponent& DBComponent);        
        bool UpdateTransferedCharCount(CDBComponent& DBComponent);

		// 캐릭터 창고 정보.
		bool GetUnifiedInfo(CDBComponent& DBComponent, unsigned long UserID_In);


		// 캐릭터 국가전쟁 포인트 체크.
		bool GetRealmPointCheck();

		// 조이스틱 정보를 얻어온다.
		KeyInfo* GetKeyInfo(void) { return &m_KeyInfo; }


		unsigned int	GetPlayTime() { return m_dwPlayTime; }
		long			GetPremiumTime() { return m_PremiumService.lPremiumTime; }
		int				GetPremiumType() { return m_PremiumService.iPremiumType; }

    private:

        USER_INFO			m_UserInfo;         					    //! 유저 정보
        CHAR_VIEW			m_CharView[USER_INFO::MAX_CHAR_NUM];	    //! 캐릭터 뷰

        UnifiedStoreInfo    m_UnifiedStoreInfo[PktUnifiedCharInfo::MAX_STORE_INFO]; // 통합 창고 정보
        UnifiedCharData     m_UnifiedCharData[PktUnifiedCharInfo::MAX_CHAR_DATA];   // 통합 캐릭터 정보

        int                 m_nUnifiedStoreInfoNum;
        int                 m_nUnifiedCharDataNum;

        char				m_szAccount[CHAR_INFOST::MAX_ACCOUNT_LEN];   //! 계정명

        IN_ADDR				m_RemoteAddress;                //! 접속 유저의 주소

        unsigned long       m_dwUID;                        //! 접속 유저 UID
        unsigned long       m_dwCID;                        //! 접속 유저 CID
        unsigned long		m_dwSessionID;                  //! 접속 유저 SessionID
        unsigned long       m_dwServerID;                   //! 접속 유저 ServerID
        
        SessionState        m_eSessionState;                //! 현재 세션의 상태 
        unsigned long       m_dwFlags;                      //! 각종 Flag들

        CBilling            m_Billing;                      //! 과금관련 정보 및 처리
        RequestQueue        m_RequestQueue;                 //! 요청 큐

        CCharacterData*     m_lpCharacterData;              //! 캐릭터 데이터
        CStoreData*         m_lpStoreData;                  //! 창고   데이터

        unsigned char       m_cAdminLevel;                  //! 운영자 레벨
        unsigned char       m_cOldServerGroupID;            //! 예전 서버군 ID : 1~10 / 나머지는 CServerSetup::AgentServerType 을 참고.
        unsigned char       m_cTransferedCharCount;         //! 캐릭터 이전 회수. 초기값은 UCHAR_MAX 이고, 
                                                            //! GetTransferedCharCount를 호출시에, 초기값 그대로이면 DB에서 로드한다.

        char	            m_cNewZone;                     //! 존 이동시 이동할 존 표시
        POS		            m_NewPos;                       //! 존 이동시 이동할 좌표 표시

		SaveEnemy			m_SaveEnemy[SaveEnemy::MAX_SAVING_ENEMY];

		KeyInfo				m_KeyInfo;

		unsigned int		m_dwPlayTime;

		PREMIUMSERVICE		m_PremiumService;
    };

    extern const char* g_szSessionStateString[CSessionData::SE_MAX_STATE];
}
}


#endif
