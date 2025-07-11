#ifndef _RYL_GAME_DB_COMPONENT_H_
#define _RYL_GAME_DB_COMPONENT_H_

// forward decl.
class CDBComponent;

namespace DBComponent
{
    namespace GameDB
    {
        // 게임 관련
        bool InsertAdmin(CDBComponent& DBComponent, unsigned long UserID_In);
        bool DeleteAdmin(CDBComponent& DBComponent, unsigned long UserID_In);
        bool IsAdmin(CDBComponent& DBComponent, unsigned long UserID_In);

        bool InsertUser(CDBComponent& DBComponent, unsigned long UserID_In, unsigned char cOldServerGroupID, bool bSupressLog = false);
        bool UpdateUser(CDBComponent& DBComponent, unsigned long dwUID, USER_INFO* lpUserInfo);
        bool UpdateUserFirstLogin(CDBComponent& DBComponent, unsigned long dwUID, unsigned char cFirstLoginType);
        bool UpdateCharZone(CDBComponent& DBComponent, unsigned long UserID_In, unsigned long Zone_In);
		
        // WORK_LIST 2.1 계정 국적 추가
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

        // 창고관련
        bool GetUnifiedItemStore1(CDBComponent& DBComponent, unsigned long UserID_In, unsigned char cOldServerGroupID, LPSTORE lpStore_Out);
        bool GetUnifiedItemStore2(CDBComponent& DBComponent, unsigned long UserID_In, unsigned char cOldServerGroupID, LPSTORE lpStore_Out);
        
        bool UpdateUnifiedItemStore1(CDBComponent& DBComponent, unsigned long UserID_In, unsigned char cOldServerGroupID, LPSTORE lpStore_In);
        bool UpdateUnifiedItemStore2(CDBComponent& DBComponent, unsigned long UserID_In, unsigned char cOldServerGroupID, LPSTORE lpStore_In);
        
        bool GetUnifiedItemStoreInfo(CDBComponent& DBComponent, unsigned long UserID_In, unsigned char cOldServerGroupID, STORE_INFO* lpStoreInfo, bool bSupressLog = false);
        bool GetUnifiedItemStoreInfo(CDBComponent& DBComponent, unsigned long UserID_In, UnifiedStoreInfo* lpUnifiedStoreInfo_Out, int& nStoreNum_InOut);
        bool SetUnifiedItemStoreInfo(CDBComponent& DBComponent, unsigned long UserID_In, unsigned char cOldServerGroupID, LPSTORE_INFO lpStoreInfo_In);
        
        // cOldServerGroupID의 창고 데이터를 cChangedServerGroupID로 덮어 씌우고,
        // cOldServerGroupID의 창고 데이터를 NULL로 만든다.
        bool ChangeUnifiedItemStoreGroup(CDBComponent& DBComponent, unsigned long dwUID, 
            unsigned char cOldServerGroupID, unsigned char cChangedServerGroupID);

        bool DeleteUnifiedStore(CDBComponent& DBComponent, unsigned long dwUID, unsigned char cOldStoreServerGroup);

        // 통합서버관련
        bool GetUnifiedCharList(CDBComponent& DBComponent, unsigned long dwUID, UnifiedCharData* lpUnifiedCharData, int& nUnifiedCharData_InOut);
        bool UpdateUnifiedCharServerGroupID(CDBComponent& DBComponent, unsigned long dwUID, unsigned long dwCID, unsigned char cOldServerGroupID);

        // 친구/거부 관련
        bool GetFriend(CDBComponent& DBComponent, unsigned long CharID_In, FRIEND* lpFriend_Out);
        bool GetBan(CDBComponent& DBComponent, unsigned long CharID_In, BAN* lpBan_Out);

        bool UpdateFriend(CDBComponent& DBComponent, unsigned long CharID_In, FRIEND* lpFriend_In);
        bool UpdateBan(CDBComponent& DBComponent, unsigned long CharID_In, BAN* lpBan_Out);

        // 퀘스트 관련
        bool GetQuest(CDBComponent& DBComponent, unsigned long CharID_In, LPQUEST lpQuest_Out);
        bool GetHistory(CDBComponent& DBComponent, unsigned long CharID_In, LPHISTORY lpHistory_Out);

        bool UpdateQuest(CDBComponent& DBComponent, unsigned long CharID_In, LPQUEST lpQuest_In);
        bool UpdateHistory(CDBComponent& DBComponent, unsigned long CharID_In, LPHISTORY lpHistory_In);

        // 캐릭터 생성
        bool InsertChar(CDBComponent& DBComponent, unsigned long dwUID, unsigned long dwSlot, 
            CHAR_CREATE& char_In, unsigned long dwGold, POS& pos, SKILL& skill, 
            unsigned char cOldServerGroupID, unsigned long& dwCID_Out);

        // 캐릭터 이름 변경 
        // dwResult : 0 - 캐릭터 이름 변경 성공
        // dwResult : 1 - 캐릭터 이름 변경 실패 (겹치는 이름 있음)
        bool ChangeCharName(CDBComponent& DBComponent, unsigned long dwCID, 
            const char* szCharName, unsigned long& dwResult);

        // 캐릭터 삭제
        bool DeleteChar(CDBComponent& DBComponent, unsigned long dwUID, unsigned long dwCID, 
            unsigned char cSlot, unsigned char cCurrentServerGroupID, unsigned char cOldServerGroupID);

        bool DeleteCharBattleServer(CDBComponent& DBComponent, unsigned long dwUID, unsigned long dwCID, 
            unsigned char cSlot, unsigned char cCurrentServerGroupID, unsigned char cOldServerGroupID);

        // 설정 관련
        bool GetConfig(CDBComponent& DBComponent, unsigned long CharID_In, LPCONFIG lpConfig_Out);
        bool UpdateConfig(CDBComponent& DBComponent, unsigned long CharID_In, LPCONFIG lpConfig_In);

		// 게임타임 관련
        bool GetPlayTime(CDBComponent& DBComponent, unsigned long UserID_In, unsigned int* PlayTime_Out);
        bool UpdatePlayTime(CDBComponent& DBComponent, unsigned long UserID_In, int Flag_In);

		// 프리미엄 타임 관련
		bool GetPremiumService(CDBComponent& DBComponent, unsigned long UserID_In, PREMIUMSERVICE& Premium_Out);
        bool UpdatePremiumService(CDBComponent& DBComponent, unsigned long UserID_In, int Flag_In);
		bool AddPremiumService(CDBComponent& DBComponent, unsigned long UserID_In, int ServiceType_In, int AddDay_In);

		// 엑스트라 이벤트 관련
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

        // 파티 관련
        bool InsertParty(CDBComponent& DBComponent, LPPARTY_DB_INFO lpParty_Info_In, unsigned long *PartyID_Out);
        bool DeleteParty(CDBComponent& DBComponent, LPPARTY_DB_INFO lpParty_Info_In, unsigned long PartyID_In);

        bool GetPartyInfo(CDBComponent& DBComponent, unsigned long PartyID_In, LPPARTY_DB_INFO lpPartyInfo_Out); 
        bool UpdatePartyInfo(CDBComponent& DBComponent, unsigned long PartyID_In, LPPARTY_DB_INFO lpPartyInfo_In);

        bool InsertPartyMember(CDBComponent& DBComponent, unsigned long PartyID_In, LPPARTY_DB_INFO lpPartyInfo_In, unsigned long CharID_In);
        bool DeletePartyMember(CDBComponent& DBComponent, unsigned long PartyID_In, LPPARTY_DB_INFO lpPartyInfo_In, unsigned long CharID_In);		

        // 로그 관련
        bool InsertItemDuplicatedLog(CDBComponent& DBComponent, unsigned __int64 dwItemSerial, 
            const char* szName, unsigned long dwUID, unsigned long dwCID, unsigned long dwQty);

		// 핵로그를 남긴다
        bool InsertHackLog(CDBComponent& DBComponent, unsigned long dwUID, unsigned long dwCID, const char* szName);


		// 전쟁 플래그 관련
		bool UpdateGuildWarFlag(CDBComponent& DBComponent, unsigned long dwCID, unsigned char cFlag);
		bool UpdateRealmWarFlag(CDBComponent& DBComponent, unsigned long dwCID, unsigned char cFlag);


		// 운영자 캐릭터 명령어 로그 관련.
		bool InsertAdminCommandLog(CDBComponent& DBComponent, unsigned long dwServerGroupID, unsigned long dwAdminCID, unsigned long dwCID, char* szCommand);

//		bool GetKeyInfo(CDBComponent& DBComponent, KeyInfo* pKeyInfo, unsigned long dwUID);
//		bool UpdateKeyInfo(CDBComponent& DBComponent, KeyInfo* pKeyInfo, unsigned long dwUID);

		// 스펠 관련
		bool GetSpell(CDBComponent& DBComponent, unsigned long CharID_In, SPELL* lpSpell_Out);
		bool UpdateSpell(CDBComponent& DBComponent, unsigned long CharID_In, SPELL* lpSpell_In);

    }
}


#endif