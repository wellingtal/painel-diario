#ifndef _RYL_GAME_DB_COMPONENT_H_
#define _RYL_GAME_DB_COMPONENT_H_

// forward decl.
class CDBComponent;

namespace DBComponent
{
    namespace GameDB
    {
        // ���� ����
        bool InsertAdmin(CDBComponent& DBComponent, unsigned long UserID_In);
        bool DeleteAdmin(CDBComponent& DBComponent, unsigned long UserID_In);
        bool IsAdmin(CDBComponent& DBComponent, unsigned long UserID_In);

        bool InsertUser(CDBComponent& DBComponent, unsigned long UserID_In, unsigned char cOldServerGroupID, bool bSupressLog = false);
        bool UpdateUser(CDBComponent& DBComponent, unsigned long dwUID, USER_INFO* lpUserInfo);
        bool UpdateUserFirstLogin(CDBComponent& DBComponent, unsigned long dwUID, unsigned char cFirstLoginType);
        bool UpdateCharZone(CDBComponent& DBComponent, unsigned long UserID_In, unsigned long Zone_In);
		
        // WORK_LIST 2.1 ���� ���� �߰�
		bool UpdateAccountNation(CDBComponent& DBComponent, unsigned long UserID_In, 
            unsigned char AccountNation_In, unsigned char cOldServerGroupID);

        bool GetUserInfo(CDBComponent& DBComponent, unsigned long UserID_In, USER_INFO& UserInfo_Out, unsigned char cOldServerGroupID, bool bSupressLog = false);
        bool GetCharView(CDBComponent& DBComponent, unsigned long CharID_In, CHAR_VIEW* lpCharLogin_Out);

        bool GetCharInfoEx(CDBComponent& DBComponent, unsigned long CharID_In, CHAR_INFOEX* lpCharInfoEx_Out);
        bool UpdateCharInfoEx(CDBComponent& DBComponent, unsigned long CharID_In, CHAR_INFOEX* lpCharInfoEx_In);

        bool InsertItemUID(CDBComponent& DBComponent, unsigned long ServerID_In, DWORD64 dwItemUID);
        bool GetItemUID(CDBComponent& DBComponent, unsigned long ServerID_In, DWORD64 *lpItemID_Out, bool bSupressLog = false);
        bool SetItemUID(CDBComponent& DBComponent, unsigned long ServerID_In, DWORD64 ItemID_In);

        bool InsertEventItem(CDBComponent& DBComponent, unsigned short wItemID, unsigned short wMaxNum, const tm& StartTime, const tm& EndTime);
        bool UpdateEventItem(CDBComponent& DBComponent, unsigned short wItemID, unsigned short wMaxNum, const tm& StartTime, const tm& EndTime);
        bool UpdateEventItem(CDBComponent& DBComponent, unsigned short wItemID, unsigned short wNowNum);
        bool DeleteEventItem(CDBComponent& DBComponent, unsigned short wItemID);

        // â�����
        bool GetUnifiedItemStore1(CDBComponent& DBComponent, unsigned long UserID_In, unsigned char cOldServerGroupID, LPSTORE lpStore_Out);
        bool GetUnifiedItemStore2(CDBComponent& DBComponent, unsigned long UserID_In, unsigned char cOldServerGroupID, LPSTORE lpStore_Out);
        
        bool UpdateUnifiedItemStore1(CDBComponent& DBComponent, unsigned long UserID_In, unsigned char cOldServerGroupID, LPSTORE lpStore_In);
        bool UpdateUnifiedItemStore2(CDBComponent& DBComponent, unsigned long UserID_In, unsigned char cOldServerGroupID, LPSTORE lpStore_In);
        
        bool GetUnifiedItemStoreInfo(CDBComponent& DBComponent, unsigned long UserID_In, unsigned char cOldServerGroupID, STORE_INFO* lpStoreInfo, bool bSupressLog = false);
        bool GetUnifiedItemStoreInfo(CDBComponent& DBComponent, unsigned long UserID_In, UnifiedStoreInfo* lpUnifiedStoreInfo_Out, int& nStoreNum_InOut);
        bool SetUnifiedItemStoreInfo(CDBComponent& DBComponent, unsigned long UserID_In, unsigned char cOldServerGroupID, LPSTORE_INFO lpStoreInfo_In);
        
        // cOldServerGroupID�� â�� �����͸� cChangedServerGroupID�� ���� �����,
        // cOldServerGroupID�� â�� �����͸� NULL�� �����.
        bool ChangeUnifiedItemStoreGroup(CDBComponent& DBComponent, unsigned long dwUID, 
            unsigned char cOldServerGroupID, unsigned char cChangedServerGroupID);

        bool DeleteUnifiedStore(CDBComponent& DBComponent, unsigned long dwUID, unsigned char cOldStoreServerGroup);

        // ���ռ�������
        bool GetUnifiedCharList(CDBComponent& DBComponent, unsigned long dwUID, UnifiedCharData* lpUnifiedCharData, int& nUnifiedCharData_InOut);
        bool UpdateUnifiedCharServerGroupID(CDBComponent& DBComponent, unsigned long dwUID, unsigned long dwCID, unsigned char cOldServerGroupID);

        // ģ��/�ź� ����
        bool GetFriend(CDBComponent& DBComponent, unsigned long CharID_In, FRIEND* lpFriend_Out);
        bool GetBan(CDBComponent& DBComponent, unsigned long CharID_In, BAN* lpBan_Out);

        bool UpdateFriend(CDBComponent& DBComponent, unsigned long CharID_In, FRIEND* lpFriend_In);
        bool UpdateBan(CDBComponent& DBComponent, unsigned long CharID_In, BAN* lpBan_Out);

        // ����Ʈ ����
        bool GetQuest(CDBComponent& DBComponent, unsigned long CharID_In, LPQUEST lpQuest_Out);
        bool GetHistory(CDBComponent& DBComponent, unsigned long CharID_In, LPHISTORY lpHistory_Out);

        bool UpdateQuest(CDBComponent& DBComponent, unsigned long CharID_In, LPQUEST lpQuest_In);
        bool UpdateHistory(CDBComponent& DBComponent, unsigned long CharID_In, LPHISTORY lpHistory_In);

        // ĳ���� ����
        bool InsertChar(CDBComponent& DBComponent, unsigned long dwUID, unsigned long dwSlot, 
            CHAR_CREATE& char_In, unsigned long dwGold, POS& pos, SKILL& skill, 
            unsigned char cOldServerGroupID, unsigned long& dwCID_Out);

        // ĳ���� �̸� ���� 
        // dwResult : 0 - ĳ���� �̸� ���� ����
        // dwResult : 1 - ĳ���� �̸� ���� ���� (��ġ�� �̸� ����)
        bool ChangeCharName(CDBComponent& DBComponent, unsigned long dwCID, 
            const char* szCharName, unsigned long& dwResult);

        // ĳ���� ����
        bool DeleteChar(CDBComponent& DBComponent, unsigned long dwUID, unsigned long dwCID, 
            unsigned char cSlot, unsigned char cCurrentServerGroupID, unsigned char cOldServerGroupID);

        bool DeleteCharBattleServer(CDBComponent& DBComponent, unsigned long dwUID, unsigned long dwCID, 
            unsigned char cSlot, unsigned char cCurrentServerGroupID, unsigned char cOldServerGroupID);

        // ���� ����
        bool GetConfig(CDBComponent& DBComponent, unsigned long CharID_In, LPCONFIG lpConfig_Out);
        bool UpdateConfig(CDBComponent& DBComponent, unsigned long CharID_In, LPCONFIG lpConfig_In);

		// ����Ÿ�� ����
        bool GetPlayTime(CDBComponent& DBComponent, unsigned long UserID_In, unsigned int* PlayTime_Out);
        bool UpdatePlayTime(CDBComponent& DBComponent, unsigned long UserID_In, int Flag_In);

		// �����̾� Ÿ�� ����
		bool GetPremiumService(CDBComponent& DBComponent, unsigned long UserID_In, PREMIUMSERVICE& Premium_Out);
        bool UpdatePremiumService(CDBComponent& DBComponent, unsigned long UserID_In, int Flag_In);
		bool AddPremiumService(CDBComponent& DBComponent, unsigned long UserID_In, int ServiceType_In, int AddDay_In);

		// ����Ʈ�� �̺�Ʈ ����
		bool UpdateExtraEvent(CDBComponent& DBComponent, unsigned long UserID_In, unsigned long EventType_In, unsigned long EventNum_In, unsigned long EventValue_In, unsigned int* Result_Out);
	
        bool GetEquip(CDBComponent& DBComponent, unsigned long CharID_In, EQUIP* lpEquip_Out);
        bool GetInven(CDBComponent& DBComponent, unsigned long CharID_In, INVEN* lpInven_Out);
        bool GetQuick(CDBComponent& DBComponent, unsigned long CharID_In, QUICK* lpQuick_Out);
        bool GetExtra(CDBComponent& DBComponent, unsigned long CharID_In, EXTRA* lpExtra_Out);
        bool GetExchange(CDBComponent& DBComponent, unsigned long CharID_In, EXCHANGE* lpExchange_Out);
        bool GetTempInven(CDBComponent& DBComponent, unsigned long CharID_In, TEMPINVEN* lpTempInven_Out);

        bool UpdateEquip(CDBComponent& DBComponent, unsigned long CharID_In, EQUIP* lpEquip_In);
        bool UpdateInven(CDBComponent& DBComponent, unsigned long CharID_In, INVEN* lpInven_In);
        bool UpdateQuick(CDBComponent& DBComponent, unsigned long CharID_In, QUICK* lpQuick_In);
        bool UpdateExtra(CDBComponent& DBComponent, unsigned long CharID_In, EXTRA* lpExtra_In);
        bool UpdateExchange(CDBComponent& DBComponent, unsigned long CharID_In, EXCHANGE* lpExchange_In);
        bool UpdateTempInven(CDBComponent& DBComponent, unsigned long CharID_In, TEMPINVEN* lpTempInven_In);

        bool GetCharInfo(CDBComponent& DBComponent, unsigned long CharID_In, LPCHAR_INFOST lpCharInfo_In, unsigned short* EquiView_In, const int nEquipView);
        bool GetCharPos(CDBComponent& DBComponent, unsigned long CharID_In, LPCHAR_POS lpCharPos_Out);
        bool GetCharSkill(CDBComponent& DBComponent, unsigned long CharID_In, LPSKILL lpSkill_Out);

        bool UpdateCharInfo(CDBComponent& DBComponent, unsigned long CharID_In, LPCHAR_INFOST lpCharInfo_In, unsigned short *EquipData_In, const int nEquipView);
        bool UpdateCharPos(CDBComponent& DBComponent, unsigned long CharID_In, LPCHAR_POS lpCharPos_In);
        bool UpdateCharSkill(CDBComponent& DBComponent, unsigned long CharID_In, LPSKILL lpSkill_In);

        bool UpdateCharParty(CDBComponent& DBComponent, unsigned long CharID_In, unsigned long PartyID_In);
        bool UpdateCharEquip(CDBComponent& DBComponent, unsigned long CharID_In, unsigned short *lpEquip_In); 

        // ��Ƽ ����
        bool InsertParty(CDBComponent& DBComponent, LPPARTY_DB_INFO lpParty_Info_In, unsigned long *PartyID_Out);
        bool DeleteParty(CDBComponent& DBComponent, LPPARTY_DB_INFO lpParty_Info_In, unsigned long PartyID_In);

        bool GetPartyInfo(CDBComponent& DBComponent, unsigned long PartyID_In, LPPARTY_DB_INFO lpPartyInfo_Out); 
        bool UpdatePartyInfo(CDBComponent& DBComponent, unsigned long PartyID_In, LPPARTY_DB_INFO lpPartyInfo_In);

        bool InsertPartyMember(CDBComponent& DBComponent, unsigned long PartyID_In, LPPARTY_DB_INFO lpPartyInfo_In, unsigned long CharID_In);
        bool DeletePartyMember(CDBComponent& DBComponent, unsigned long PartyID_In, LPPARTY_DB_INFO lpPartyInfo_In, unsigned long CharID_In);		

        // �α� ����
        bool InsertItemDuplicatedLog(CDBComponent& DBComponent, unsigned __int64 dwItemSerial, 
            const char* szName, unsigned long dwUID, unsigned long dwCID, unsigned long dwQty);

		// �ٷα׸� �����
        bool InsertHackLog(CDBComponent& DBComponent, unsigned long dwUID, unsigned long dwCID, const char* szName);


		// ���� �÷��� ����
		bool UpdateGuildWarFlag(CDBComponent& DBComponent, unsigned long dwCID, unsigned char cFlag);
		bool UpdateRealmWarFlag(CDBComponent& DBComponent, unsigned long dwCID, unsigned char cFlag);


		// ��� ĳ���� ��ɾ� �α� ����.
		bool InsertAdminCommandLog(CDBComponent& DBComponent, unsigned long dwServerGroupID, unsigned long dwAdminCID, unsigned long dwCID, char* szCommand);

//		bool GetKeyInfo(CDBComponent& DBComponent, KeyInfo* pKeyInfo, unsigned long dwUID);
//		bool UpdateKeyInfo(CDBComponent& DBComponent, KeyInfo* pKeyInfo, unsigned long dwUID);

		// ���� ����
		bool GetSpell(CDBComponent& DBComponent, unsigned long CharID_In, SPELL* lpSpell_Out);
		bool UpdateSpell(CDBComponent& DBComponent, unsigned long CharID_In, SPELL* lpSpell_In);

    }
}


#endif