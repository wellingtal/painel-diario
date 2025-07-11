#include "stdafx.h"
#include "AuthDispatch.h"
#include "Part1DBAgentDispatch.h"

#include <Log/ServerLog.h>
#include <Network/Address/INET_Addr.h>
#include <Network/SendPacket/SendServerInfo.h>
#include <Network/SendPacket/SendCharManage.h>

#include <Network/Packet/PacketCommand.h>
#include <Network/Packet/PacketStruct/UnifiedCharPacket.h>

#include <Utility/Setup/ServerSetup.h>

#include <DataStorage/SessionDataMgr.h>
#include <DataStorage/StoreDataMgr.h>
#include <DataStorage/CharacterDataMgr.h>

#include <Creature/Creature.h>
#include <Creature/Character/CharacterClass.h>

#include <DB/DBComponent.h>
#include <DB/GameDBComponent.h>

#include <Network/Packet/PacketStruct/CharQuestPacket.h>

namespace DBAgent
{

class CCharBackupItemData
{
public:

    CCharBackupItemData() 
        :   m_bBackupData(false), m_cOldServerGroupID(0)
    {
        memset(&m_CharInfo, 0, sizeof(CHAR_INFOST));	// 기본 정보
        memset(&m_Skill, 0, sizeof(SKILL));		        // 스킬 정보

        memset(&m_Equip, 0, sizeof(EQUIP));             // 장비
        memset(&m_Inven, 0, sizeof(INVEN));             // 인벤
        memset(&m_Extra, 0, sizeof(EXTRA));     		// 여분
        memset(&m_Exchange, 0, sizeof(EXCHANGE));		// 교환
        memset(&m_TempInven, 0, sizeof(TEMPINVEN));	    // 임시 인벤토리

        memset(&m_Quest, 0, sizeof(QUEST));             // 퀘스트
        memset(&m_History, 0, sizeof(HISTORY));         // 히스토리
    }

    bool IsBackupedData() const { return m_bBackupData; }
    unsigned char GetOldServerGroupID() const { return m_cOldServerGroupID; }

    void BackupData(DataStorage::CSessionData& sessionData, DataStorage::CCharacterData& charData)
    {
        m_CharInfo  = charData.GetInfo();
        m_Skill     = charData.GetSkill();
        m_Equip     = charData.GetEquip();
        m_Inven     = charData.GetInven();
        m_Extra     = charData.GetExtra();
        m_Exchange  = charData.GetExchange();
        m_TempInven = charData.GetTempInven();

        m_Quest     = charData.GetQuest();
        m_History   = charData.GetHistory();

        const UnifiedCharData* lpUnifiedCharData = 
            sessionData.GetUnifiedCharData(charData.GetCID());

        if(0 != lpUnifiedCharData)
        {
            m_cOldServerGroupID = lpUnifiedCharData->cOldServerGroupID;
        }

        m_bBackupData = true;
    }

    void RestoreData(DataStorage::CCharacterData& charData)
    {
        if (m_bBackupData)
        {
            charData.SetInfo(m_CharInfo);
            charData.SetSkill(m_Skill);
            charData.SetEquip(m_Equip.Data, m_Equip.dwSize);
            charData.SetInven(m_Inven.Data, m_Inven.dwSize);
            charData.SetExtra(m_Extra.Data, m_Extra.dwSize);
            charData.SetExchange(m_Exchange.Data, m_Exchange.dwSize);
            charData.SetTempInven(m_TempInven.Data, m_TempInven.dwSize);

            charData.SetQuest(m_Quest);
            charData.SetHistory(m_History);
        }
    }

private:

    CHAR_INFOST	    m_CharInfo;		// 기본 정보
    SKILL		    m_Skill;		// 스킬 정보

    EQUIP		    m_Equip;		// 장비
    INVEN		    m_Inven;		// 인벤
    EXTRA		    m_Extra;		// 여분
    EXCHANGE	    m_Exchange;		// 교환
    TEMPINVEN	    m_TempInven;	// 임시 인벤토리

    QUEST           m_Quest;        // 퀘스트
    HISTORY         m_History;      // History

    unsigned char   m_cOldServerGroupID;    // 예전 서버그룹 ID
    bool            m_bBackupData;          // 데이터 백업되었는지 여부

};

void ProcessUnifiedCharSelect(CSendStream& SendStream, PktBase* lpPktBase);


CSingleDispatch& CPart1DBAgentDispatch::GetDispatchTable()
{
    static CSingleDispatch part1DBAgentDispatch;
    return part1DBAgentDispatch;
}

CPart1DBAgentDispatch::CPart1DBAgentDispatch(CSession& Session)
    :   CRylServerDispatch(Session, MAX_PACKET_DISPATCH_PER_PULSE)
{


}

CPart1DBAgentDispatch::~CPart1DBAgentDispatch()
{


}

void CPart1DBAgentDispatch::Connected()
{
    DETLOG3(g_Log, "SS:0x%08x/DP:0x%08x/IP:%15s/Part1 DBAgentServer Connected",
        &GetSession(), this, GetRemoteAddr().get_addr_string());

    GetDispatchTable().SetDispatch(this);

    SendPacket::ServerLogin(GetSendStream(), 
        CServerSetup::GetInstance().GetServerID());
}


void CPart1DBAgentDispatch::Disconnected()
{
    DETLOG3(g_Log, "SS:0x%08x/DP:0x%08x/IP:%15s/Part1 DBAgentServer Disconnected",
        &GetSession(), this, GetRemoteAddr().get_addr_string());

    GetDispatchTable().RemoveDispatch(this);
}


bool CPart1DBAgentDispatch::DispatchPacket(PktBase* lpPktBase)
{
    switch(lpPktBase->GetCmd())
    {
    case CmdUnifiedCharSelect:
        ProcessUnifiedCharSelect(GetSendStream(), lpPktBase);
        break;
    }

    return true;
}

bool CPart1DBAgentDispatch::TransferCharPart1ToPart2(CSendStream& SendStream, unsigned long dwUID, 
                                                     unsigned char cSelectedServerGroup,
                                                     unsigned char cSelectedNation,                                                     
                                                     unsigned long* lpdwSelectedCID, 
                                                     unsigned char cSelectedCharNum)
{
    char* lpBuffer = SendStream.GetBuffer(sizeof(PktUnifiedCharSelectReq));

    if(0 != lpBuffer)
    {
        PktUnifiedCharSelectReq* lpPktUnifiedCharSelectReq = 
            reinterpret_cast<PktUnifiedCharSelectReq*>(lpBuffer);

        memset(lpPktUnifiedCharSelectReq->szPassword, 0, 
            sizeof(char) * PktUnifiedCharSelectReq::MAX_PASSWORD_LEN);

        lpPktUnifiedCharSelectReq->cSelectedNation          = cSelectedNation;
        lpPktUnifiedCharSelectReq->cSelectedServerGroupID   = cSelectedServerGroup;
        lpPktUnifiedCharSelectReq->dwRequestKey             = 0;

        lpPktUnifiedCharSelectReq->dwUID = dwUID;
        memcpy(lpPktUnifiedCharSelectReq->dwCID, lpdwSelectedCID, 
            sizeof(unsigned long) * std::min(int(cSelectedCharNum), int(USER_INFO::MAX_CHAR_NUM)));

        return SendStream.WrapHeader(sizeof(PktUnifiedCharSelectReq), CmdUnifiedCharSelect, 0, 0);
    }

    return false;
}


void ProcessUnifiedCharSelect(CSendStream& SendStream, PktBase* lpPktBase)
{
    PktUnifiedCharSelectReq* lpPktUnifiedCharSelectReq = 
        reinterpret_cast<PktUnifiedCharSelectReq*>(lpPktBase);

    unsigned long       dwUID                   = lpPktUnifiedCharSelectReq->dwUID;    
    unsigned long       dwRequestKey            = lpPktUnifiedCharSelectReq->dwRequestKey;
    unsigned char       cSelectedServerGroupID  = lpPktUnifiedCharSelectReq->cSelectedServerGroupID;
    unsigned char       cSelectedNation         = lpPktUnifiedCharSelectReq->cSelectedNation;

    unsigned short      usError                 = 0;

    using namespace DBAgent::DataStorage;

    unsigned char cAgentServerType = static_cast<unsigned char>(
        CServerSetup::GetInstance().GetAgentServerType());

    CStoreDataMgr&      storeDataMgr    = CStoreDataMgr::GetInstance();
    CCharacterDataMgr&  charDataMgr     = CCharacterDataMgr::GetInstance();
    CDBComponent&       dbComponent     = CDBSingleObject::GetInstance();

    UnifiedStoreKey     srcStoreKey(dwUID, cSelectedServerGroupID);
    UnifiedStoreKey     dstStoreKey(dwUID, cAgentServerType);

    CSessionData*       lpSessionData   = 0;
    CStoreData*         lpStoreData     = 0;
    CCharacterData*     lpCharacterData = 0;

    if(0 != lpPktBase->GetError())
    {
        // 해당 UID가 없다.
        ERRLOG2(g_Log, "UID:%10u / Part2Unified 캐릭터 선택 실패 - Part1 DB중계서버에서 %d에러를 전송했습니다.",
            dwUID, lpPktBase->GetError());

        usError = lpPktBase->GetError();
    }
    else if(0 == (lpSessionData = CSessionDataMgr::GetInstance().GetOpenSession(dwUID)))
    {
        // 해당 UID가 없다.
        ERRLOG1(g_Log, "UID:%10u / Part2Unified 캐릭터 선택 실패 - 세션이 닫혀 있습니다", dwUID);

        usError = PktUnifiedCharSelectAck::SERVER_ERROR;
    }
    else
    {
        // 창고 선택한 적 없음
        if (!(lpSessionData->GetFirstLogin() & UnifiedConst::SELECTED_PART2))
        {
            // 혹시 캐쉬나 loginDB에 데이터가 들어있는지 살핀다. 있으면 전부 제거한다.
            if(storeDataMgr.IsDataLoginDB(srcStoreKey))
            {
                ERRLOG2(g_Log, "UID:%10u / SrcServerGroupID:%d / Part2Unified 창고 선택 실패 - 해당 창고가 로그인해 있습니다.", 
                    srcStoreKey.first, srcStoreKey.second);

                usError = PktUnifiedCharSelectAck::SERVER_ERROR;
            }
            else if(storeDataMgr.IsDataLoginDB(dstStoreKey))
            {
                ERRLOG2(g_Log, "UID:%10u / DstServerGroupID:%d / Part2Unified 창고 선택 실패 - 해당 창고가 로그인해 있습니다.", 
                    dstStoreKey.first, dstStoreKey.second);

                usError = PktUnifiedCharSelectAck::SERVER_ERROR;
            }
            else
            {
                // 일단 캐쉬에서 데이터를 내린다.
                if(storeDataMgr.RemoveLogoutData(srcStoreKey))
                {
                    ERRLOG2(g_Log, "UID:%10u / SrcServerGroupID:%d / "
                        "Part2Unified 창고 선택 이상 - 해당 데이터가 캐쉬에 들어 있어 강제로 내립니다.", 
                        srcStoreKey.first, srcStoreKey.second);
                }

                if(storeDataMgr.RemoveLogoutData(dstStoreKey))
                {
                    ERRLOG2(g_Log, "UID:%10u / DstServerGroupID:%d / "
                        "Part2Unified 창고 선택 이상 - 해당 데이터가 캐쉬에 들어 있어 강제로 내립니다.", 
                        dstStoreKey.first, dstStoreKey.second);
                }

                // 선택한 서버군 창고를 내 서버그룹 창고로 복사한다!
                if (!lpSessionData->ChangeUnifiedStoreInfoGroup(
                    dbComponent, cSelectedServerGroupID, cAgentServerType))
                {
                    usError = PktUnifiedCharSelectAck::UNIFIED_STORE_READ_ERROR;
                }
                // 복사 잘 됐으면 캐쉬로 로드해서 컨버팅을 한다.
                else 
                {
                    if(0 == (lpStoreData = storeDataMgr.GetLogoutData(dstStoreKey)))
                    {                    
                        ERRLOG2(g_Log, "UID:%10u / ServerGroupID:%d / 통합서버 선택 실패 : 창고 캐쉬로 데이터 로드 실패", 
                            dwUID, cSelectedServerGroupID);

                        usError = PktUnifiedCharSelectAck::UNIFIED_STORE_READ_ERROR;
                    }
                    // 창고 데이터를 Part2아이템으로 컨버팅한다.
                    else if (!lpStoreData->ConvertToPart2Item(true))
                    {
                        ERRLOG2(g_Log, "UID:%10u / ServerGroupID:%d / 통합서버 선택 실패 : 창고 파트2 아이템으로 아이템 컨버팅 실패", 
                            dwUID, cSelectedServerGroupID);

                        usError = PktUnifiedCharSelectAck::UNIFIED_STORE_WRITE_ERROR;
                    }
                    // 창고 데이터를 저장한다.
                    else if(!lpStoreData->ForceUpdateDBAllData(dbComponent))
                    {
                        ERRLOG2(g_Log, "UID:%10u / ServerGroupID:%d / 통합서버 선택 실패 : 컨버팅된 창고 데이터를 저장 실패", 
                            dwUID, cSelectedServerGroupID);

                        usError = PktUnifiedCharSelectAck::UNIFIED_STORE_WRITE_ERROR;
                    }
                    else if(!DBComponent::GameDB::UpdateUserFirstLogin(dbComponent, dwUID, 
                        lpSessionData->GetFirstLogin() | UnifiedConst::SELECTED_PART2))
                    {
                        ERRLOG2(g_Log, "UID:%10u / ServerGroupID:%d / 통합서버 선택 실패 : 창고 가져왔음을 기록 실패",  
                            dwUID, cSelectedServerGroupID);
                    }

                    if(0 != usError)
                    {
                        // 에러가 난 건 창고 데이터를 저장을 못 했기 때문이니,
                        // 그냥 캐쉬에서 날리고 다시 세팅하면 된다.
                        storeDataMgr.RemoveLogoutData(dstStoreKey);

                        if(!lpSessionData->ChangeUnifiedStoreInfoGroup(
                            dbComponent, cAgentServerType, cSelectedServerGroupID))
                        {
                            ERRLOG3(g_Log, "UID:%10u / ServerGroupID:%d -> %d / 통합서버 선택 실패 : 창고 데이터 롤백 실패", 
                                dwUID, cAgentServerType, cSelectedServerGroupID);
                        }
                    }
                    else
                    {
                        lpSessionData->SetFirstLogin(
                            lpSessionData->GetFirstLogin() | UnifiedConst::SELECTED_PART2);

                        INFLOG2(g_Log, "UID:%10u / ServerGroupID:%d / 통합서버 선택 성공", 
                            dwUID, cSelectedServerGroupID);
                    }
                }
            }
        }

        // 캐릭터 선택 쪽을 처리한다.

        // 1. 캐릭터를 받으면, 일단 UserInfo를 로드한 후에, 
        //    올바른 슬롯에 넣은 중복되지 않는 캐릭터를 찾는다.

        // 2. 정리가 끝나면 Part1 DB중계서버에, 해당 캐릭터를 캐쉬에서 언로드하고
        //    길드 / 파티를 탈퇴시키라고 요청한다.

        // 3. 요청 결과가 성공으로 돌아오면, 해당 캐릭터를 DB에서 로드하고 
        //    데이터를 컨버팅 한 후에 저장한다. 컨버팅이나 저장이 실패하면 데이터를 롤백한다.

        // 4. 모든 작업이 무사히 끝나면, DB의 UserInfo를 갱신한다.

        CClass::RaceType eRace = 
            lpSessionData->CheckUnifiedCharRace(lpPktUnifiedCharSelectReq->dwCID);

        USER_INFO userInfo = lpSessionData->GetUserInfo();

        // 유저 바인딩 검사를 한번 더 한다.
        unsigned long   dwSelectedCID[USER_INFO::MAX_CHAR_NUM]; 
        unsigned long*  lpdwRequestedCID = lpPktUnifiedCharSelectReq->dwCID;

        int nSelectedCount = 0;

        std::fill_n(dwSelectedCID, size_t(USER_INFO::MAX_CHAR_NUM), 0);

        // 데이터를 제대로 얻어 왔다. 
        // 현재 바인딩 된 캐릭터인지 아닌지 판단한다.
        unsigned char cRestrictedPart1ToPart2Level = 
            CServerSetup::GetInstance().GetRestrictedPart1ToPart2Level();

        for(int nCount = 0; nCount < USER_INFO::MAX_CHAR_NUM; ++nCount)
        {
            unsigned long dwBindCID = lpdwRequestedCID[nCount];
            const UnifiedCharData* lpUnifiedCharData = 0;

            if (0 == userInfo.CharID[nCount] && 
                0 != dwBindCID && !userInfo.HasCharacter(dwBindCID) &&
                0 != (lpUnifiedCharData = lpSessionData->GetUnifiedCharData(dwBindCID)) &&
                (lpUnifiedCharData->cOldServerGroupID < UnifiedConst::Part1 ||
                (lpUnifiedCharData->cOldServerGroupID == UnifiedConst::Part1Unified &&
                cRestrictedPart1ToPart2Level <= lpUnifiedCharData->cLevel)))
            {
                dwSelectedCID[nCount] = dwBindCID;
                ++nSelectedCount;
            }
        }

        if (0 < nSelectedCount && CClass::MAX_RACE == eRace)
        {
            usError = PktUnifiedCharSelectAck::PACKET_ERROR;
        }
        else if(0 == usError)
        {
            // 한바퀴 돌면서 아이템 컨버팅 / 스킬북 컨버팅 등을 수행한다.
            CCharBackupItemData backupData[USER_INFO::MAX_CHAR_NUM];
            
            for(int nCount = 0; nCount < USER_INFO::MAX_CHAR_NUM; ++nCount)
            {
                if(0 != dwSelectedCID[nCount])
                {
                    if(0 == (lpCharacterData = charDataMgr.GetLogoutData(dwSelectedCID[nCount])))
                    {
                        ERRLOG3(g_Log, "UID:%10u / CID:%10u / ServerGroupID:%d / 통합서버 선택 실패 : 캐릭터 캐쉬로 데이터 로드 실패", 
                            dwUID, dwSelectedCID[nCount], cSelectedServerGroupID);

                        usError = PktUnifiedCharSelectAck::UNIFIED_CHAR_READ_ERROR;
                    }
                    else
                    {
                        // 데이터를 백업한다.
                        backupData[nCount].BackupData(*lpSessionData, *lpCharacterData);

                        // Part2 아이템으로 컨버팅한다.
                        if (!lpCharacterData->ConvertToPart2Item(true))
                        {
                            ERRLOG3(g_Log, "UID:%10u / CID:%10u / ServerGroupID:%d / 통합서버 선택 실패 : 캐릭터 Part2아이템으로 컨버팅 실패", 
                                dwUID, dwSelectedCID[nCount], cSelectedServerGroupID);

                            usError = PktUnifiedCharSelectAck::UNIFIED_CHAR_WRITE_ERROR;
                        }
                        // UnifiedCharInfo에서, 캐릭터 서버군 번호를 수정한다.
                        else if (!lpSessionData->ChangeUnifiedCharServerGroupID(
                            dbComponent, dwUID, dwSelectedCID[nCount], cAgentServerType))
                        {
                            usError = PktUnifiedCharSelectAck::UNIFIED_CHAR_WRITE_ERROR;
                        }
                        else if (!lpCharacterData->ForceUpdateDBAllData(dbComponent))
                        {
                            ERRLOG3(g_Log, "UID:%10u / CID:%10u / ServerGroupID:%d / 통합서버 선택 실패 : 캐릭터 아이템 저장 실패", 
                                dwUID, dwSelectedCID[nCount], cSelectedServerGroupID);

                            usError = PktUnifiedCharSelectAck::UNIFIED_CHAR_WRITE_ERROR;
                        }

                        if(0 != usError)
                        {
                            // 에러가 있으면 바로 롤백;;
                            break;
                        }

						// 양국체제로 변경 //
						if(lpSessionData->GetAccountNation()==Creature::ALMIGHTY_PIRATE)
						{
							/* 국적이 신의 해적단이면서, 종족이 휴먼인 사람들의 국적을 카르테란트로 바꿔준다. */
							if(lpCharacterData->GetRace()==CClass::HUMAN)
							{
								lpSessionData->SetAccountNation(Creature::KARTERANT);
							}
							/* 국적이 신의 해적단이면서, 종족이 아칸인 사람들의 국적을 메르카디아로 바꿔준다. */
							else if(lpCharacterData->GetRace()==CClass::AKHAN)
							{
								lpSessionData->SetAccountNation(Creature::MERKADIA);
							}
						}

						// 친구리스트, 거부 리스트 삭제 //
						/*unsigned long dwMemberCID[CFriendList::MAX_FRIENDS_NUM] = {0,};
						unsigned long dwDeleteCID[CFriendList::MAX_FRIENDS_NUM] = {0,};
						
						// 친구리스트에 CID 만 가지고 온다. //
						CFriendList	friendList = lpCharacterData->GetFriendList();

						friendList.GetCIDList(dwMemberCID);

						for(unsigned char cIndex = 0; cIndex < friendList.GetFriendNum(); cIndex++)
						{							
							CFriendList::Rebind* lpRebind = friendList.GetFriend(dwMemberCID[cIndex]);

							if(lpCharacterData->GetRace()==CClass::HUMAN)
							{
								if(lpRebind->GetClass()>=CClass::Combatant)
								{
									dwDeleteCID[cIndex] = dwMemberCID[cIndex];
								}
							}
							else if(lpCharacterData->GetRace()==CClass::AKHAN)
							{
								if(lpRebind->GetClass()<CClass::Combatant)
								{
									dwDeleteCID[cIndex] = dwMemberCID[cIndex];
								}
							}

							// 다른 종족일 경우 삭제 //
							if(dwDeleteCID[cIndex])
							{
								friendList.Remove(dwDeleteCID[cIndex]);
							}
						}
						
						memset(dwMemberCID, 0, sizeof(unsigned long) * CFriendList::MAX_FRIENDS_NUM);
						memset(dwDeleteCID, 0, sizeof(unsigned long) * CFriendList::MAX_FRIENDS_NUM);

						CBanList banList = lpCharacterData->GetBanList();						

						banList.GetCIDList(dwMemberCID);

						for(unsigned char cIndex = 0; cIndex < banList.GetBanNum(); cIndex++)
						{
							CBanList::Rebind* lpRebind = banList.GetBan(dwMemberCID[cIndex]);

							if(lpCharacterData->GetRace()==CClass::HUMAN)
							{
								if(lpRebind->GetClass()>=CClass::Combatant)
								{
									dwDeleteCID[cIndex] = dwMemberCID[cIndex];
								}
							}
							else if(lpCharacterData->GetRace()==CClass::AKHAN)
							{
								if(lpRebind->GetClass()<CClass::Combatant)
								{
									dwDeleteCID[cIndex] = dwMemberCID[cIndex];
								}
							}

							// 다른 종족일 경우 삭제 //
							if(dwDeleteCID[cIndex])
							{
								banList.Remove(dwDeleteCID[cIndex]);
							}
						}*/
                    }
                }
            }

            // 에러가 없으면 데이터를 바인딩한다.
            if(0 == usError)
            {
                // 선택된 캐릭터가 한마리도 없었으면, 국적을 선택한다.
                if(0 == userInfo.GetCharacterNum())
                {
                    unsigned char cOldAccountNation = lpSessionData->GetAccountNation();

                    // 선택한 국가가 종족과 맞는지 체크한 후 세팅한다.
                    if(Creature::ALMIGHTY_PIRATE == cSelectedNation)
                    {
                        lpSessionData->SetAccountNation(Creature::ALMIGHTY_PIRATE);
                    }
                    else
                    {
                        lpSessionData->SetAccountNation(
                            CClass::HUMAN == eRace ? Creature::KARTERANT : Creature::MERKADIA);
                    }

                    if (cOldAccountNation != lpSessionData->GetAccountNation())
                    {
                        INFLOG3(g_Log, "UID:%10u / OldAccountNation:%s / AccountNation:%s / 국적 변경",
                            dwUID, Creature::GetShortNationName(cOldAccountNation), 
                            Creature::GetShortNationName(lpSessionData->GetAccountNation()));
                    }
                }

                if(!lpSessionData->UpdateCharacterBinding(dbComponent, dwSelectedCID))
                {
                    ERRLOG2(g_Log, "UID:%10u / ServerGroupID:%d / 통합서버 선택 실패 : 최종 캐릭터 바인딩에 실패했습니다",
                        dwUID, cSelectedServerGroupID);

                    usError = PktUnifiedCharSelectAck::UNIFIED_CHAR_WRITE_ERROR;
                }
                else
                {
                    // 창고/캐릭터 이전에 성공했다!
                    lpSessionData->SetOldServerGroupID(cAgentServerType);
                }
            }
            
            if(0 != usError)
            {
                // 문제가 생겼다. 데이터를 전부 Rollback한다.
                for(int nCount = 0; nCount < USER_INFO::MAX_CHAR_NUM; ++nCount)
                {
                    if(0 != dwSelectedCID[nCount] && backupData[nCount].IsBackupedData())
                    {
                        if(0 == (lpCharacterData = charDataMgr.GetLogoutData(dwSelectedCID[nCount])))
                        {
                            ERRLOG3(g_Log, "UID:%10u / CID:%10u / ServerGroupID:%d / 통합서버 선택 실패 - 데이터 복구 실패 : 캐릭터 캐쉬로 데이터 로드 실패", 
                                dwUID, dwSelectedCID[nCount], cSelectedServerGroupID);
                        }
                        else
                        {
                            backupData[nCount].RestoreData(*lpCharacterData);

                            lpCharacterData->ForceUpdateDBAllData(dbComponent);
                            lpSessionData->ChangeUnifiedCharServerGroupID(dbComponent,
                                dwUID, dwSelectedCID[nCount], backupData[nCount].GetOldServerGroupID());
                        }
                    }
                }
            }
        }
    }

    GET_SINGLE_DISPATCH(lpAuthDispatch, CAuthDispatch, 
        CAuthDispatch::GetDispatchTable());

    if(0 != lpAuthDispatch && SendPacket::UnifiedCharSelectAck(
        lpAuthDispatch->GetSendStream(), dwUID, dwRequestKey, usError))
    {
        if(0 == usError)
        {
            INFLOG2(g_Log, "UID:%10u / ServerGroupID:%d / 캐릭터 및 창고 선택 성공", 
                dwUID, cSelectedServerGroupID);
        }
    }
    else
    {
        ERRLOG2(g_Log, "UID:%10u / ServerGroupID:%d / 캐릭터 선택 실패 : "
            "인증서버와 접속이 끊기거나 전송에 실패했습니다.", dwUID, cSelectedServerGroupID);
    }
}






}


