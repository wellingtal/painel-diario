#ifndef _RYL_AUTH_DB_COMPONENT_H_
#define _RYL_AUTH_DB_COMPONENT_H_

// forward decl.
class CDBComponent;

namespace DBComponent
{
    namespace AuthDB
    {
        // ---------------------------------------------------------------------------------
        // 인증 관련
        /*
        unsigned long GetID(HAN_UID Auth_In);
        */

        bool LoginAuthMyth(CDBComponent& DBComponent, const char *Account_In, 
            const char *Password_In, const char *szAddress, AUTH* lpAuthUser_Out);

		// 일본 인증
		bool LoginAuthJapan(CDBComponent& DBComponent, const char *Account_In, 
			const char *Password_In, const char *szAddress, AUTH* lpAuthUser_Out);	

        bool LoginAuthHan(CDBComponent& DBComponent, const char* szAccount, const char* szPassword, HAN_UID& hanUID);
        bool RegRylGameHan(CDBComponent& DBComponent, char *Account_In, char *Mail_In);

        bool GetAccountByUID(CDBComponent& DBComponent, unsigned long UID_In, char *Account_Out);
        bool GetUIDByAccount(CDBComponent& DBComponent, char *Account_In, char *UID_Out);

        bool GetMythAccountByUID(CDBComponent& DBComponent, unsigned long UID_In, char *Account_Out);
        bool GetMythUIDByAccount(CDBComponent& DBComponent, const char *Account_In, unsigned long *UID_Out);

        bool CheckBlockedUser(CDBComponent& DBComponent, unsigned long UID_In, unsigned char cServerID, LPRE_CMS_CheckUserBlock lpData_Out);

        // 배틀로한 인증 관련
        bool LoginBattleAuthHan(CDBComponent& DBComponent, const char* szAccount, const char* szPassword, HAN_UID& hanUID);
        bool RegBattleRylGameHan(CDBComponent& DBComponent, char* szAccount_In, char* szMail_In);

        bool GetBattleAccountByUID(CDBComponent& DBComponent, unsigned long UID_In, char *Account_Out);
        bool GetBattleUIDByAccount(CDBComponent& DBComponent, char *Account_In, char *UID_Out);

        // 배틀로한은 기존 게임과 UID가 다르다. 그래서 기존 게임 UID로 배틀로한 UID를 얻어내거나
        // 배틀로한 UID로 기존 게임 UID를 얻어낼 필요가 있다.
        bool GetUIDFromBattleUID(CDBComponent& DBComponent, unsigned long dwBattleUID, unsigned long* lpUID);
        bool GetBattleUIDFromUID(CDBComponent& DBComponent, unsigned long dwUID, unsigned long* lpBattleUID);

		// 이관 신청 여부 확인.
		bool GetPCCheckAgreement(CDBComponent& DBComponent, const char* szAccount_In, unsigned long* dwNumber_Out);

        // 가마 독자 서비스 관련 인증
        bool LoginAuthGama(CDBComponent& DBComponent, const char* szAccount, const char* szPassword, unsigned long& dwUID);
        bool GetGamaAccountByUID(CDBComponent& DBComponent, unsigned long dwUID, char* szAccount_Out, int nAccountBufferLen);
        bool GetGamaUIDByAccount(CDBComponent& DBComponent, const char* szAccount, unsigned long& dwUID);
    };
};

#endif 