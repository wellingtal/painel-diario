#ifndef _RYL_AUTH_DB_COMPONENT_H_
#define _RYL_AUTH_DB_COMPONENT_H_

// forward decl.
class CDBComponent;

namespace DBComponent
{
    namespace AuthDB
    {
        // ---------------------------------------------------------------------------------
        // ���� ����
        /*
        unsigned long GetID(HAN_UID Auth_In);
        */

        bool LoginAuthMyth(CDBComponent& DBComponent, const char *Account_In, 
            const char *Password_In, const char *szAddress, AUTH* lpAuthUser_Out);

		// �Ϻ� ����
		bool LoginAuthJapan(CDBComponent& DBComponent, const char *Account_In, 
			const char *Password_In, const char *szAddress, AUTH* lpAuthUser_Out);	

        bool LoginAuthHan(CDBComponent& DBComponent, const char* szAccount, const char* szPassword, HAN_UID& hanUID);
        bool RegRylGameHan(CDBComponent& DBComponent, char *Account_In, char *Mail_In);

        bool GetAccountByUID(CDBComponent& DBComponent, unsigned long UID_In, char *Account_Out);
        bool GetUIDByAccount(CDBComponent& DBComponent, char *Account_In, char *UID_Out);

        bool GetMythAccountByUID(CDBComponent& DBComponent, unsigned long UID_In, char *Account_Out);
        bool GetMythUIDByAccount(CDBComponent& DBComponent, const char *Account_In, unsigned long *UID_Out);

        bool CheckBlockedUser(CDBComponent& DBComponent, unsigned long UID_In, unsigned char cServerID, LPRE_CMS_CheckUserBlock lpData_Out);

        // ��Ʋ���� ���� ����
        bool LoginBattleAuthHan(CDBComponent& DBComponent, const char* szAccount, const char* szPassword, HAN_UID& hanUID);
        bool RegBattleRylGameHan(CDBComponent& DBComponent, char* szAccount_In, char* szMail_In);

        bool GetBattleAccountByUID(CDBComponent& DBComponent, unsigned long UID_In, char *Account_Out);
        bool GetBattleUIDByAccount(CDBComponent& DBComponent, char *Account_In, char *UID_Out);

        // ��Ʋ������ ���� ���Ӱ� UID�� �ٸ���. �׷��� ���� ���� UID�� ��Ʋ���� UID�� ���ų�
        // ��Ʋ���� UID�� ���� ���� UID�� �� �ʿ䰡 �ִ�.
        bool GetUIDFromBattleUID(CDBComponent& DBComponent, unsigned long dwBattleUID, unsigned long* lpUID);
        bool GetBattleUIDFromUID(CDBComponent& DBComponent, unsigned long dwUID, unsigned long* lpBattleUID);

		// �̰� ��û ���� Ȯ��.
		bool GetPCCheckAgreement(CDBComponent& DBComponent, const char* szAccount_In, unsigned long* dwNumber_Out);

        // ���� ���� ���� ���� ����
        bool LoginAuthGama(CDBComponent& DBComponent, const char* szAccount, const char* szPassword, unsigned long& dwUID);
        bool GetGamaAccountByUID(CDBComponent& DBComponent, unsigned long dwUID, char* szAccount_Out, int nAccountBufferLen);
        bool GetGamaUIDByAccount(CDBComponent& DBComponent, const char* szAccount, unsigned long& dwUID);
    };
};

#endif 