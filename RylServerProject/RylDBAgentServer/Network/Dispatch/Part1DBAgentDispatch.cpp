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
        memset(&m_CharInfo, 0, sizeof(CHAR_INFOST));	// �⺻ ����
        memset(&m_Skill, 0, sizeof(SKILL));		        // ��ų ����

        memset(&m_Equip, 0, sizeof(EQUIP));             // ���
        memset(&m_Inven, 0, sizeof(INVEN));             // �κ�
        memset(&m_Extra, 0, sizeof(EXTRA));     		// ����
        memset(&m_Exchange, 0, sizeof(EXCHANGE));		// ��ȯ
        memset(&m_TempInven, 0, sizeof(TEMPINVEN));	    // �ӽ� �κ��丮

        memset(&m_Quest, 0, sizeof(QUEST));             // ����Ʈ
        memset(&m_History, 0, sizeof(HISTORY));         // �����丮
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

    CHAR_INFOST	    m_CharInfo;		// �⺻ ����
    SKILL		    m_Skill;		// ��ų ����

    EQUIP		    m_Equip;		// ���
    INVEN		    m_Inven;		// �κ�
    EXTRA		    m_Extra;		// ����
    EXCHANGE	    m_Exchange;		// ��ȯ
    TEMPINVEN	    m_TempInven;	// �ӽ� �κ��丮

    QUEST           m_Quest;        // ����Ʈ
    HISTORY         m_History;      // History

    unsigned char   m_cOldServerGroupID;    // ���� �����׷� ID
    bool            m_bBackupData;          // ������ ����Ǿ����� ����

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
        // �ش� UID�� ����.
        ERRLOG2(g_Log, "UID:%10u / Part2Unified ĳ���� ���� ���� - Part1 DB�߰輭������ %d������ �����߽��ϴ�.",
            dwUID, lpPktBase->GetError());

        usError = lpPktBase->GetError();
    }
    else if(0 == (lpSessionData = CSessionDataMgr::GetInstance().GetOpenSession(dwUID)))
    {
        // �ش� UID�� ����.
        ERRLOG1(g_Log, "UID:%10u / Part2Unified ĳ���� ���� ���� - ������ ���� �ֽ��ϴ�", dwUID);

        usError = PktUnifiedCharSelectAck::SERVER_ERROR;
    }
    else
    {
        // â�� ������ �� ����
        if (!(lpSessionData->GetFirstLogin() & UnifiedConst::SELECTED_PART2))
        {
            // Ȥ�� ĳ���� loginDB�� �����Ͱ� ����ִ��� ���ɴ�. ������ ���� �����Ѵ�.
            if(storeDataMgr.IsDataLoginDB(srcStoreKey))
            {
                ERRLOG2(g_Log, "UID:%10u / SrcServerGroupID:%d / Part2Unified â�� ���� ���� - �ش� â�� �α����� �ֽ��ϴ�.", 
                    srcStoreKey.first, srcStoreKey.second);

                usError = PktUnifiedCharSelectAck::SERVER_ERROR;
            }
            else if(storeDataMgr.IsDataLoginDB(dstStoreKey))
            {
                ERRLOG2(g_Log, "UID:%10u / DstServerGroupID:%d / Part2Unified â�� ���� ���� - �ش� â�� �α����� �ֽ��ϴ�.", 
                    dstStoreKey.first, dstStoreKey.second);

                usError = PktUnifiedCharSelectAck::SERVER_ERROR;
            }
            else
            {
                // �ϴ� ĳ������ �����͸� ������.
                if(storeDataMgr.RemoveLogoutData(srcStoreKey))
                {
                    ERRLOG2(g_Log, "UID:%10u / SrcServerGroupID:%d / "
                        "Part2Unified â�� ���� �̻� - �ش� �����Ͱ� ĳ���� ��� �־� ������ �����ϴ�.", 
                        srcStoreKey.first, srcStoreKey.second);
                }

                if(storeDataMgr.RemoveLogoutData(dstStoreKey))
                {
                    ERRLOG2(g_Log, "UID:%10u / DstServerGroupID:%d / "
                        "Part2Unified â�� ���� �̻� - �ش� �����Ͱ� ĳ���� ��� �־� ������ �����ϴ�.", 
                        dstStoreKey.first, dstStoreKey.second);
                }

                // ������ ������ â�� �� �����׷� â��� �����Ѵ�!
                if (!lpSessionData->ChangeUnifiedStoreInfoGroup(
                    dbComponent, cSelectedServerGroupID, cAgentServerType))
                {
                    usError = PktUnifiedCharSelectAck::UNIFIED_STORE_READ_ERROR;
                }
                // ���� �� ������ ĳ���� �ε��ؼ� �������� �Ѵ�.
                else 
                {
                    if(0 == (lpStoreData = storeDataMgr.GetLogoutData(dstStoreKey)))
                    {                    
                        ERRLOG2(g_Log, "UID:%10u / ServerGroupID:%d / ���ռ��� ���� ���� : â�� ĳ���� ������ �ε� ����", 
                            dwUID, cSelectedServerGroupID);

                        usError = PktUnifiedCharSelectAck::UNIFIED_STORE_READ_ERROR;
                    }
                    // â�� �����͸� Part2���������� �������Ѵ�.
                    else if (!lpStoreData->ConvertToPart2Item(true))
                    {
                        ERRLOG2(g_Log, "UID:%10u / ServerGroupID:%d / ���ռ��� ���� ���� : â�� ��Ʈ2 ���������� ������ ������ ����", 
                            dwUID, cSelectedServerGroupID);

                        usError = PktUnifiedCharSelectAck::UNIFIED_STORE_WRITE_ERROR;
                    }
                    // â�� �����͸� �����Ѵ�.
                    else if(!lpStoreData->ForceUpdateDBAllData(dbComponent))
                    {
                        ERRLOG2(g_Log, "UID:%10u / ServerGroupID:%d / ���ռ��� ���� ���� : �����õ� â�� �����͸� ���� ����", 
                            dwUID, cSelectedServerGroupID);

                        usError = PktUnifiedCharSelectAck::UNIFIED_STORE_WRITE_ERROR;
                    }
                    else if(!DBComponent::GameDB::UpdateUserFirstLogin(dbComponent, dwUID, 
                        lpSessionData->GetFirstLogin() | UnifiedConst::SELECTED_PART2))
                    {
                        ERRLOG2(g_Log, "UID:%10u / ServerGroupID:%d / ���ռ��� ���� ���� : â�� ���������� ��� ����",  
                            dwUID, cSelectedServerGroupID);
                    }

                    if(0 != usError)
                    {
                        // ������ �� �� â�� �����͸� ������ �� �߱� �����̴�,
                        // �׳� ĳ������ ������ �ٽ� �����ϸ� �ȴ�.
                        storeDataMgr.RemoveLogoutData(dstStoreKey);

                        if(!lpSessionData->ChangeUnifiedStoreInfoGroup(
                            dbComponent, cAgentServerType, cSelectedServerGroupID))
                        {
                            ERRLOG3(g_Log, "UID:%10u / ServerGroupID:%d -> %d / ���ռ��� ���� ���� : â�� ������ �ѹ� ����", 
                                dwUID, cAgentServerType, cSelectedServerGroupID);
                        }
                    }
                    else
                    {
                        lpSessionData->SetFirstLogin(
                            lpSessionData->GetFirstLogin() | UnifiedConst::SELECTED_PART2);

                        INFLOG2(g_Log, "UID:%10u / ServerGroupID:%d / ���ռ��� ���� ����", 
                            dwUID, cSelectedServerGroupID);
                    }
                }
            }
        }

        // ĳ���� ���� ���� ó���Ѵ�.

        // 1. ĳ���͸� ������, �ϴ� UserInfo�� �ε��� �Ŀ�, 
        //    �ùٸ� ���Կ� ���� �ߺ����� �ʴ� ĳ���͸� ã�´�.

        // 2. ������ ������ Part1 DB�߰輭����, �ش� ĳ���͸� ĳ������ ��ε��ϰ�
        //    ��� / ��Ƽ�� Ż���Ű��� ��û�Ѵ�.

        // 3. ��û ����� �������� ���ƿ���, �ش� ĳ���͸� DB���� �ε��ϰ� 
        //    �����͸� ������ �� �Ŀ� �����Ѵ�. �������̳� ������ �����ϸ� �����͸� �ѹ��Ѵ�.

        // 4. ��� �۾��� ������ ������, DB�� UserInfo�� �����Ѵ�.

        CClass::RaceType eRace = 
            lpSessionData->CheckUnifiedCharRace(lpPktUnifiedCharSelectReq->dwCID);

        USER_INFO userInfo = lpSessionData->GetUserInfo();

        // ���� ���ε� �˻縦 �ѹ� �� �Ѵ�.
        unsigned long   dwSelectedCID[USER_INFO::MAX_CHAR_NUM]; 
        unsigned long*  lpdwRequestedCID = lpPktUnifiedCharSelectReq->dwCID;

        int nSelectedCount = 0;

        std::fill_n(dwSelectedCID, size_t(USER_INFO::MAX_CHAR_NUM), 0);

        // �����͸� ����� ��� �Դ�. 
        // ���� ���ε� �� ĳ�������� �ƴ��� �Ǵ��Ѵ�.
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
            // �ѹ��� ���鼭 ������ ������ / ��ų�� ������ ���� �����Ѵ�.
            CCharBackupItemData backupData[USER_INFO::MAX_CHAR_NUM];
            
            for(int nCount = 0; nCount < USER_INFO::MAX_CHAR_NUM; ++nCount)
            {
                if(0 != dwSelectedCID[nCount])
                {
                    if(0 == (lpCharacterData = charDataMgr.GetLogoutData(dwSelectedCID[nCount])))
                    {
                        ERRLOG3(g_Log, "UID:%10u / CID:%10u / ServerGroupID:%d / ���ռ��� ���� ���� : ĳ���� ĳ���� ������ �ε� ����", 
                            dwUID, dwSelectedCID[nCount], cSelectedServerGroupID);

                        usError = PktUnifiedCharSelectAck::UNIFIED_CHAR_READ_ERROR;
                    }
                    else
                    {
                        // �����͸� ����Ѵ�.
                        backupData[nCount].BackupData(*lpSessionData, *lpCharacterData);

                        // Part2 ���������� �������Ѵ�.
                        if (!lpCharacterData->ConvertToPart2Item(true))
                        {
                            ERRLOG3(g_Log, "UID:%10u / CID:%10u / ServerGroupID:%d / ���ռ��� ���� ���� : ĳ���� Part2���������� ������ ����", 
                                dwUID, dwSelectedCID[nCount], cSelectedServerGroupID);

                            usError = PktUnifiedCharSelectAck::UNIFIED_CHAR_WRITE_ERROR;
                        }
                        // UnifiedCharInfo����, ĳ���� ������ ��ȣ�� �����Ѵ�.
                        else if (!lpSessionData->ChangeUnifiedCharServerGroupID(
                            dbComponent, dwUID, dwSelectedCID[nCount], cAgentServerType))
                        {
                            usError = PktUnifiedCharSelectAck::UNIFIED_CHAR_WRITE_ERROR;
                        }
                        else if (!lpCharacterData->ForceUpdateDBAllData(dbComponent))
                        {
                            ERRLOG3(g_Log, "UID:%10u / CID:%10u / ServerGroupID:%d / ���ռ��� ���� ���� : ĳ���� ������ ���� ����", 
                                dwUID, dwSelectedCID[nCount], cSelectedServerGroupID);

                            usError = PktUnifiedCharSelectAck::UNIFIED_CHAR_WRITE_ERROR;
                        }

                        if(0 != usError)
                        {
                            // ������ ������ �ٷ� �ѹ�;;
                            break;
                        }

						// �籹ü���� ���� //
						if(lpSessionData->GetAccountNation()==Creature::ALMIGHTY_PIRATE)
						{
							/* ������ ���� �������̸鼭, ������ �޸��� ������� ������ ī���׶�Ʈ�� �ٲ��ش�. */
							if(lpCharacterData->GetRace()==CClass::HUMAN)
							{
								lpSessionData->SetAccountNation(Creature::KARTERANT);
							}
							/* ������ ���� �������̸鼭, ������ ��ĭ�� ������� ������ �޸�ī��Ʒ� �ٲ��ش�. */
							else if(lpCharacterData->GetRace()==CClass::AKHAN)
							{
								lpSessionData->SetAccountNation(Creature::MERKADIA);
							}
						}

						// ģ������Ʈ, �ź� ����Ʈ ���� //
						/*unsigned long dwMemberCID[CFriendList::MAX_FRIENDS_NUM] = {0,};
						unsigned long dwDeleteCID[CFriendList::MAX_FRIENDS_NUM] = {0,};
						
						// ģ������Ʈ�� CID �� ������ �´�. //
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

							// �ٸ� ������ ��� ���� //
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

							// �ٸ� ������ ��� ���� //
							if(dwDeleteCID[cIndex])
							{
								banList.Remove(dwDeleteCID[cIndex]);
							}
						}*/
                    }
                }
            }

            // ������ ������ �����͸� ���ε��Ѵ�.
            if(0 == usError)
            {
                // ���õ� ĳ���Ͱ� �Ѹ����� ��������, ������ �����Ѵ�.
                if(0 == userInfo.GetCharacterNum())
                {
                    unsigned char cOldAccountNation = lpSessionData->GetAccountNation();

                    // ������ ������ ������ �´��� üũ�� �� �����Ѵ�.
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
                        INFLOG3(g_Log, "UID:%10u / OldAccountNation:%s / AccountNation:%s / ���� ����",
                            dwUID, Creature::GetShortNationName(cOldAccountNation), 
                            Creature::GetShortNationName(lpSessionData->GetAccountNation()));
                    }
                }

                if(!lpSessionData->UpdateCharacterBinding(dbComponent, dwSelectedCID))
                {
                    ERRLOG2(g_Log, "UID:%10u / ServerGroupID:%d / ���ռ��� ���� ���� : ���� ĳ���� ���ε��� �����߽��ϴ�",
                        dwUID, cSelectedServerGroupID);

                    usError = PktUnifiedCharSelectAck::UNIFIED_CHAR_WRITE_ERROR;
                }
                else
                {
                    // â��/ĳ���� ������ �����ߴ�!
                    lpSessionData->SetOldServerGroupID(cAgentServerType);
                }
            }
            
            if(0 != usError)
            {
                // ������ �����. �����͸� ���� Rollback�Ѵ�.
                for(int nCount = 0; nCount < USER_INFO::MAX_CHAR_NUM; ++nCount)
                {
                    if(0 != dwSelectedCID[nCount] && backupData[nCount].IsBackupedData())
                    {
                        if(0 == (lpCharacterData = charDataMgr.GetLogoutData(dwSelectedCID[nCount])))
                        {
                            ERRLOG3(g_Log, "UID:%10u / CID:%10u / ServerGroupID:%d / ���ռ��� ���� ���� - ������ ���� ���� : ĳ���� ĳ���� ������ �ε� ����", 
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
            INFLOG2(g_Log, "UID:%10u / ServerGroupID:%d / ĳ���� �� â�� ���� ����", 
                dwUID, cSelectedServerGroupID);
        }
    }
    else
    {
        ERRLOG2(g_Log, "UID:%10u / ServerGroupID:%d / ĳ���� ���� ���� : "
            "���������� ������ ����ų� ���ۿ� �����߽��ϴ�.", dwUID, cSelectedServerGroupID);
    }
}






}


