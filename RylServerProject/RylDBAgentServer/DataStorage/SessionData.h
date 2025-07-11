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
			// edith 2008.03.17 ID,PASS ��������
			MaxName = 24,
			MaxPass = 36,
		};

        unsigned long   m_dwRequestKey;
        unsigned long   m_dwSelectedCID;
        unsigned long   m_dwServerID;
        unsigned long   m_dwSessionID;

        IN_ADDR         m_PeerAddress;
        char            m_szAccount[MaxName];

		// edith 2008.01.15 �н����� �߰��۾�
        char            m_szPassword[MaxPass];

        RequestData();

		// edith 2008.01.15 �н����� �߰��۾�
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
			MAX_REALM_POINT					= 5,					// ������ �������� ��� ��,.�� by Minbobo.

			REALM_POINT_TIME				= 60,					// 60 ��

			FAME_PANELTY_VALUE_1ST			= 5000,
			FAME_PANELTY_VALUE_2ND			= 10000,
        };

		struct SaveEnemy
		{
			enum
			{
				MAX_SAVING_ENEMY			= 5,		// ����ϴ� �ִ� ��ü ��
			};
			

			unsigned long	m_dwCID;
			unsigned long	m_dwTick;
		};

        typedef std::deque<RequestData>     RequestQueue;

        CSessionData();
        ~CSessionData();


        //! ���� �⺻ ������.
        unsigned long GetUID() const { return m_dwUID; }
        unsigned long GetCID() const { return m_dwCID; }
        unsigned long GetSessionID() const { return m_dwSessionID; }
        unsigned long GetServerID() const  { return m_dwServerID;  }
        IN_ADDR GetRemoteAddress() const { return m_RemoteAddress; }
        const char* GetAccountName() const { return m_szAccount; }
        unsigned char GetAdminLevel() const { return m_cAdminLevel; }
		// WORK_LIST 2.1 ���� ���� �߰�
		unsigned char GetAccountNation() const { return m_UserInfo.Nation;	}
		void SetAccountNation(unsigned char cNation) { m_UserInfo.Nation = cNation; }

        const USER_INFO& GetUserInfo() const { return m_UserInfo; }
        const CHAR_VIEW* GetCharView(unsigned char cIndex) const 
        { return (cIndex < USER_INFO::MAX_CHAR_NUM) ? m_CharView + cIndex : 0; }

        bool GetCharView(unsigned long dwCID, CHAR_VIEW& charView_Out);
        
		// WORK_LIST 2.3 ���� ���� ���� ��� ����

		bool SetCharViewGID(unsigned long dwCID, unsigned long dwGID);
		bool SetCharViewFame(unsigned long dwCID, unsigned long dwFame);

        CBilling& GetBilling() { return m_Billing; }

        bool HasCharacter(unsigned long dwCID) const; 

        //! ���� ���� ������ ��, ���� ���� ��ȯ �Լ�
        SessionState GetSessionState() const { return m_eSessionState; }
        void SetSessionState(SessionState eSessionState) { m_eSessionState = eSessionState; }

        //! �������� ID�� ����
        void SetAuthServerID();
        void SetAccountName(const char* szAccountName);

        //! ���� Enable/Disable/Move
        bool UserEnable(CDBComponent& DBComponent, unsigned long dwUID);
        bool UserDisable();
        bool UserMove();

        //! ĳ���� Enable/Disable/Move
        bool LoadData(unsigned long dwCID);
        void UnloadData();

        bool CharEnable(unsigned long dwCID, unsigned long dwServerID);
        bool CharDisable(unsigned long dwCID, unsigned long dwServerID);
        bool CharMove(unsigned long dwCID, unsigned long dwServerID);
               
        //! ���� ������ ������. (ĳ����)
        CCharacterData* GetCharacterData()	{ return m_lpCharacterData; }
        CStoreData*     GetStoreData()		{ return m_lpStoreData; }

		SaveEnemy*		GetSaveEnemy()		{ return m_SaveEnemy; }

        //! ���� ������ ������. (â��)
        void SetCharacterData(CCharacterData* lpCharacterData) { m_lpCharacterData = lpCharacterData; }
        void SetStoreData(CStoreData* lpStoreData) { m_lpStoreData = lpStoreData; }

        //! �� �̵� ���� ������        
        bool SetMoveZone(unsigned char cZone, const POS& Pos);
        void UpdateMoveZone();			
        bool CheckCharZone(unsigned char cZone);

        unsigned char GetCharStartZone(char cRace);
        
		bool GetPlayTimeFromDB(CDBComponent& DBComponent);
		bool GetPremiumServiceFromDB(CDBComponent& DBComponent);

		bool GetUserInfoFromDB(CDBComponent& DBComponent);      //! DB�κ��� ���� ���� �� �並 ���´�.
        bool GetUnifiedDataFromDB(CDBComponent& DBComponent);   //! DB�κ��� ���� ���� ������ ���´�.
        
        //! ������ �α� ���� �޼ҵ�
        void LogUserData(const char* szMessage);
        void LogCharData(const char* szMessage);
        void LogUserDataError(const char* szMessage, const char* szRoutine, const char* szFile, int nLine);
        void LogCharDataError(const char* szMessage, const char* szRoutine, const char* szFile, int nLine);

        //! ������ ���� ���� �޼ҵ�
        void SetUID(unsigned long dwUID) { m_dwUID = dwUID; }
        void SetAddress(IN_ADDR addr) { m_RemoteAddress = addr; }

        //! ��û ť ���� (dwRequestKey�� 0�̸� ����)
        void PushRequest(const RequestData& reqData);
        RequestData PopRequest();

        void SetRequestData(const RequestData& reqData);

        //! �������� ���� ������
        unsigned char GetOldServerGroupID() const { return m_cOldServerGroupID; }
        void SetOldServerGroupID(unsigned char cOldServerGroupID) { m_cOldServerGroupID = cOldServerGroupID; }

        const UnifiedStoreInfo* GetUnifiedStoreInfo() const { return m_UnifiedStoreInfo; }
        const UnifiedCharData*  GetUnifiedCharData() const  { return m_UnifiedCharData; }
        const UnifiedCharData*  GetUnifiedCharData(unsigned long dwCID) const;

        int GetUnifiedStoreInfoNum() const { return m_nUnifiedStoreInfoNum; }
        int GetUnifiedCharDataNum() const { return m_nUnifiedCharDataNum; }

        // â�� ���� / ó�� �α��� ���� �� ������ ��� �ִ� ������
        unsigned char GetFirstLogin() const { return m_UserInfo.FirstLogin; }
        void SetFirstLogin(unsigned char cFirstLogin) { m_UserInfo.FirstLogin = cFirstLogin; }

        // �ش��ϴ� ������ ��ȣ�� â�� �ִ��� ���ɴ�.
        bool HasUnifiedStoreInfo(unsigned char cOldServerGroupID);

        // �ش��ϴ� ������ ��ȣ�� â���� ������ ��ȣ�� �ٲ۴�.
        bool ChangeUnifiedStoreInfoGroup(CDBComponent& DBComponent, 
            unsigned char cOldServerGroupID, unsigned char cNewServerGroupID);

        // UID/CID�� ��ġ�ϴ� ĳ������ �����׷�ID�� �ٲ۴�.
        bool ChangeUnifiedCharServerGroupID(CDBComponent& DBComponent,
            unsigned long dwUID, unsigned long dwCID, unsigned char cOldServerGroupID);

        // �ش��ϴ� ���ռ��� ĳ���� ������ �����Ѵ�(ĳ���� ������ ���)
        bool DelUnifiedCharData(unsigned long dwCID);

        // ĳ���� ���ε��� �����Ѵ�.
        bool UpdateCharacterBinding(CDBComponent& DBComponent, 
            unsigned long dwSelectedCID[USER_INFO::MAX_CHAR_NUM]);      

        // ĳ���� CID�� �ָ� ���õ� ������ �����ش�.
        CClass::RaceType CheckUnifiedCharRace(unsigned long dwSelectedCID[USER_INFO::MAX_CHAR_NUM]);

        // Part1 -> Part2 ĳ���� ���� ȸ���� �����Ѵ�.
        unsigned char GetTransferedCharCount(CDBComponent& DBComponent);        
        bool UpdateTransferedCharCount(CDBComponent& DBComponent);

		// ĳ���� â�� ����.
		bool GetUnifiedInfo(CDBComponent& DBComponent, unsigned long UserID_In);


		// ĳ���� �������� ����Ʈ üũ.
		bool GetRealmPointCheck();

		// ���̽�ƽ ������ ���´�.
		KeyInfo* GetKeyInfo(void) { return &m_KeyInfo; }


		unsigned int	GetPlayTime() { return m_dwPlayTime; }
		long			GetPremiumTime() { return m_PremiumService.lPremiumTime; }
		int				GetPremiumType() { return m_PremiumService.iPremiumType; }

    private:

        USER_INFO			m_UserInfo;         					    //! ���� ����
        CHAR_VIEW			m_CharView[USER_INFO::MAX_CHAR_NUM];	    //! ĳ���� ��

        UnifiedStoreInfo    m_UnifiedStoreInfo[PktUnifiedCharInfo::MAX_STORE_INFO]; // ���� â�� ����
        UnifiedCharData     m_UnifiedCharData[PktUnifiedCharInfo::MAX_CHAR_DATA];   // ���� ĳ���� ����

        int                 m_nUnifiedStoreInfoNum;
        int                 m_nUnifiedCharDataNum;

        char				m_szAccount[CHAR_INFOST::MAX_ACCOUNT_LEN];   //! ������

        IN_ADDR				m_RemoteAddress;                //! ���� ������ �ּ�

        unsigned long       m_dwUID;                        //! ���� ���� UID
        unsigned long       m_dwCID;                        //! ���� ���� CID
        unsigned long		m_dwSessionID;                  //! ���� ���� SessionID
        unsigned long       m_dwServerID;                   //! ���� ���� ServerID
        
        SessionState        m_eSessionState;                //! ���� ������ ���� 
        unsigned long       m_dwFlags;                      //! ���� Flag��

        CBilling            m_Billing;                      //! ���ݰ��� ���� �� ó��
        RequestQueue        m_RequestQueue;                 //! ��û ť

        CCharacterData*     m_lpCharacterData;              //! ĳ���� ������
        CStoreData*         m_lpStoreData;                  //! â��   ������

        unsigned char       m_cAdminLevel;                  //! ��� ����
        unsigned char       m_cOldServerGroupID;            //! ���� ������ ID : 1~10 / �������� CServerSetup::AgentServerType �� ����.
        unsigned char       m_cTransferedCharCount;         //! ĳ���� ���� ȸ��. �ʱⰪ�� UCHAR_MAX �̰�, 
                                                            //! GetTransferedCharCount�� ȣ��ÿ�, �ʱⰪ �״���̸� DB���� �ε��Ѵ�.

        char	            m_cNewZone;                     //! �� �̵��� �̵��� �� ǥ��
        POS		            m_NewPos;                       //! �� �̵��� �̵��� ��ǥ ǥ��

		SaveEnemy			m_SaveEnemy[SaveEnemy::MAX_SAVING_ENEMY];

		KeyInfo				m_KeyInfo;

		unsigned int		m_dwPlayTime;

		PREMIUMSERVICE		m_PremiumService;
    };

    extern const char* g_szSessionStateString[CSessionData::SE_MAX_STATE];
}
}


#endif
