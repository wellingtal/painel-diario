#include "stdafx.h"
#include "SendLoginout.h"

#include <Network/Stream/SendStream.h>
#include <Network/Packet/PacketStruct/CharLoginOutPacket.h>
#include <Network/Packet/PacketStruct/CharLoginOutPacketStruct.h>
#include <Network/Packet/PacketStruct/CharItemPacket.h>
#include <Network/Packet/PacketStruct/CharQuestPacket.h>
#include <Network/Packet/PacketStruct/CharConfigPacket.h>
#include <Network/Packet/PacketStruct/AuthServerToDBAgentServer.h>

#include <DataStorage/StoreData.h>
#include <DataStorage/CharacterData.h>
#include <DataStorage/SessionData.h>

#include <Log/ServerLog.h>
#include <Network/Packet/PacketCommand.h>

#include <Community/Party/PartyDBInfo.h>
#include <Community/Party/PartyDBMgr.h>

#include <Community/Guild/GuildDB.h>
#include <Community/Guild/GuildDBMgr.h>

#include <DB/DBComponent.h>
#include <DB/GameDBComponent.h>

#include <Utility/Setup/ServerSetup.h>

#include <DataStorage/SessionDataMgr.h>

#include <GameTime/GameTimeConstants.h>
#include <GameTime/GameTimeDBMgr.h>

#include <Network/Packet/PacketStruct/CharCommunityPacket.h>

namespace DBAgent
{
namespace SendPacket
{

// 친구 / 거부 데이터 전송
bool Friend(CSendStream& SendStream, unsigned char cCmd, unsigned long dwRequestKey, unsigned long dwUID, unsigned long dwCID, 
            unsigned long dwGID, unsigned short wClass, char cLevel, unsigned long dwServerID, const char* lpData, unsigned long dwDataLen);

// 퀘스트 / 히스토리 데이터 전송
bool Quest(CSendStream& SendStream, unsigned long dwRequestKey, 
           unsigned long dwUID, unsigned long dwCID, 
           const char* lpQuestBuffer, unsigned long dwQuestSize, 
           const char* lpHistoryBuffer, unsigned long dwHistorySize);

// 설정 정보 전송
bool ConfigInfo(CSendStream& SendStream, unsigned long dwRequestKey, 
                unsigned long dwUID, unsigned long dwCID, 
                const char* lpConfigBuffer, unsigned long dwConfigSize);

// 창고 정보 전송
bool DepositData(CSendStream& SendStream, unsigned char cCmd, 
                 unsigned long dwRequestKey, unsigned long dwCID, const STORE_INFO& storeInfo);

// 창고 데이터 전송
bool DepositDBUpdate(CSendStream& SendStream, unsigned long dwRequestKey,
                     unsigned long dwUID, unsigned long dwCID,
                     unsigned char cTabNum, bool bComplete,
                     const STORE_INFO& storeInfo, const STORE& store);


// 세션 시작
bool StartSession(CSendStream& SendStream, unsigned long dwRequestKey, 
                  unsigned long dwUID, unsigned long dwSessionID, 
                  unsigned char cFirstLogin, unsigned short usError)
{
    char* lpBuffer = SendStream.GetBuffer(sizeof(PktSSD));
    if(0 != lpBuffer)
    {
        PktSSD* lpPktSSDAck = reinterpret_cast<PktSSD*>(lpBuffer);

        lpPktSSDAck->m_dwRequestKey    = dwRequestKey;
        lpPktSSDAck->m_wCmd			    = PktDD::SCmdStartSession;

        lpPktSSDAck->m_dwUserID		    = dwUID;
        lpPktSSDAck->m_dwSessionID		= dwSessionID;

        lpPktSSDAck->m_cAgentServerType = 
            static_cast<unsigned char>(CServerSetup::GetInstance().GetAgentServerType());

        lpPktSSDAck->m_cFirstLogin      = 
            (UnifiedConst::Part2Selectable != lpPktSSDAck->m_cAgentServerType)
            ? cFirstLogin : static_cast<unsigned char>(CServerSetup::GetInstance().GetSelectableUnifiedServerNum());

		ZeroMemory(lpPktSSDAck->m_AccountName, sizeof(char)*PktSSD::MaxName);
		ZeroMemory(lpPktSSDAck->m_Password, sizeof(char)*PktSSD::MaxPass);

        return SendStream.WrapHeader(sizeof(PktSSD), CmdDBGetData, 0, usError);
    }

    return false;
}


// 캐릭터 로그인
bool CharLogin(CSendStream& SendStream, DataStorage::CSessionData& SessionData, 
               unsigned long dwRequestKey)
{
	// DBAgent에 캐릭터 로그인 결과가 날아왔다.
	// DB에서 정보를 읽어와 DataStorage::CCharacterData 에 정보를 저정한다.


    const unsigned long MAX_BUFFER_SIZE = sizeof(PktDBUpdate) + DBUpdateData::MAX_DBUPDATE_SIZE;

    char szBuffer[MAX_BUFFER_SIZE] = {0,};
    unsigned long dwBufferSize = MAX_BUFFER_SIZE;

    DataStorage::CCharacterData* lpCharacterData = SessionData.GetCharacterData();
    DataStorage::CStoreData*     lpStoreData     = SessionData.GetStoreData();

    if(0 == lpCharacterData || 0 == lpStoreData)
    {
        ERRLOG4(g_Log, "UID:%10u / CID:%10u / CCharacterData:%p / CStoreData:%p / "
            "캐릭터 로그인 전송 실패 : 데이터가 없습니다.",
            SessionData.GetUID(), SessionData.GetCID(), lpCharacterData, lpStoreData);
    }
    else
    {
        unsigned long dwUID = SessionData.GetUID();
        unsigned long dwCID = lpCharacterData->GetCID();

        PktDBUpdate* lpPktDBUpdate = reinterpret_cast<PktDBUpdate*>(szBuffer);
        char* lpDataPos = szBuffer + sizeof(PktDBUpdate);

        unsigned long dwDataSize = MAX_BUFFER_SIZE - sizeof(PktDBUpdate);

        memset(lpPktDBUpdate, 0, sizeof(PktDBUpdate));

        if(!lpCharacterData->SerializeOut(lpDataPos, lpPktDBUpdate->m_usUpdate, 
            dwDataSize, DBUpdateData::MAX_UPDATE_DB))
        {
            ERRLOG2(g_Log, "UID:%10u / CID:%10u / 캐릭터 로그인 전송 실패 : 캐릭터 데이터 SerializeOut에 실패했습니다.",
                SessionData.GetUID(), SessionData.GetCID());
        }
        else
        {
			// 플레이어 타음을 업데이트 해준다.
			// 로그인 한다.
			if (GameRYL::CHINA == CServerSetup::GetInstance().GetNationType())
			{
				DBComponent::GameDB::UpdatePlayTime(CDBSingleObject::GetInstance(), dwUID, 0);
				SessionData.GetPlayTimeFromDB(CDBSingleObject::GetInstance());
			}

			// 프리미엄 서비스 타임 설정
			DBComponent::GameDB::UpdatePremiumService(CDBSingleObject::GetInstance(), dwUID, 0);
			SessionData.GetPremiumServiceFromDB(CDBSingleObject::GetInstance());
			

            unsigned char cAdminLevel = SessionData.GetAdminLevel();
            DBUpdateData::UpdateType eUpdateType = 
                (0 != cAdminLevel) ? DBUpdateData::ADMIN_LOGIN : DBUpdateData::LOGIN;

			// 용병 정보.
			CGuildDB* lpGuild = static_cast<CGuildDB*>(
				CGuildDBMgr::GetInstance().GetGuild(lpCharacterData->GetGID()));

			unsigned char cTacticsFlag = 0;

			if(lpGuild)
			{
				cTacticsFlag = lpGuild->GetTacticsMember(dwCID);
			}

            lpPktDBUpdate->m_dlItemSerial		= 0;
            lpPktDBUpdate->m_dwSessionID		= SessionData.GetSessionID();
            lpPktDBUpdate->m_dwUserID			= dwUID;
            lpPktDBUpdate->m_dwCharID			= dwCID;
            lpPktDBUpdate->m_dwRequestKey		= dwRequestKey;
            lpPktDBUpdate->m_Address			= SessionData.GetRemoteAddress();
            lpPktDBUpdate->m_cAdminLevel		= cAdminLevel;
			lpPktDBUpdate->m_cAdminFlag			= lpCharacterData->GetAdminFlag();			
			lpPktDBUpdate->m_cTacticsFlag		= cTacticsFlag;

            strncpy(lpPktDBUpdate->m_szAccountName, 
                SessionData.GetAccountName(), CHAR_INFOST::MAX_ACCOUNT_LEN);

            lpPktDBUpdate->m_szAccountName[CHAR_INFOST::MAX_ACCOUNT_LEN - 1] = 0;
			
            // WORK_LIST 2.4 계정 국적을 게임서버의 캐릭터가 가지도록 구현            
			lpPktDBUpdate->m_cAccountNation		= SessionData.GetAccountNation();
            lpPktDBUpdate->m_cNameChangeCount	= lpCharacterData->GetInfoEx().cNameChangeCount;

			// 인스턴스 전쟁 참여 플래그인데 전쟁 시간이 아니면 OFF 로 바꿔준다.
			if (lpCharacterData->GetInfoEx().GuildWarFlag == Creature::WAR_INSTANCE &&
				!CGameTimeDBMgr::GetInstance().IsGuildWarTime())
			{
				lpCharacterData->SetGuildWarFlag(Creature::WAR_OFF);
			}

			if (lpCharacterData->GetInfoEx().RealmWarFlag == Creature::WAR_INSTANCE &&
				!CGameTimeDBMgr::GetInstance().IsRealmWarReadyTime() &&
				!CGameTimeDBMgr::GetInstance().IsRealmWarTime())
			{
				lpCharacterData->SetRealmWarFlag(Creature::WAR_OFF);
			}


			lpPktDBUpdate->m_cGuildWarFlag	= lpCharacterData->GetInfoEx().GuildWarFlag;
			lpPktDBUpdate->m_cRealmWarFlag	= lpCharacterData->GetInfoEx().RealmWarFlag;
			lpPktDBUpdate->m_cRealmPoint	= lpCharacterData->GetInfoEx().RealmPoint;

            lpPktDBUpdate->m_TypeCode       = static_cast<unsigned short>(eUpdateType);
			lpPktDBUpdate->m_PlayTime		= SessionData.GetPlayTime();
			lpPktDBUpdate->m_PremiumTime	= SessionData.GetPremiumTime();
			lpPktDBUpdate->m_PremiumType	= SessionData.GetPremiumType();

            if(!SendStream.WrapCompress(szBuffer, 
                static_cast<unsigned short>(sizeof(PktDBUpdate) + dwDataSize), CmdDBUpdateData, 0, 0))
            {
                // 캐릭터 로그인 전송 실패
                return false;
            }

            // 일단 캐릭터 로그인이 전송 완료된 이후는, 

			unsigned long	dwGID	= lpCharacterData->GetGID();
			unsigned short	wClass	= lpCharacterData->GetClass();
			char			cLevel	= lpCharacterData->GetLevel();

			// Friend 정보 설정 (시간이 생기면 수정하겠음) //
			unsigned long dwMemberCID[CFriendList::MAX_FRIENDS_NUM] = {0,};

			// 친구리스트에 CID 만 가지고 온다. //
			CFriendList	friendList = lpCharacterData->GetFriendList();
			
			friendList.GetCIDList(dwMemberCID);

			for(unsigned char cIndex = 0; cIndex < friendList.GetFriendNum(); cIndex++)
			{
				DataStorage::CSessionData* lpTargetSessionData = DataStorage::CSessionDataMgr::GetInstance().GetCharLoadedSession(dwMemberCID[cIndex]);

				CFriendList::Rebind* lpRebind = friendList.GetFriend(dwMemberCID[cIndex]);

				if(lpRebind)
				{
					if(lpTargetSessionData)
					{
						DataStorage::CCharacterData* lpTargetCharacterData = lpTargetSessionData->GetCharacterData();

						if(lpTargetCharacterData)
						{
							if (DataStorage::CSessionData::SE_USER_ENABLED != lpTargetSessionData->GetSessionState() &&
								DataStorage::CSessionData::SE_CHAR_ENABLED != lpTargetSessionData->GetSessionState())
							{
								lpRebind->UpdateServerID(0);
							}
							else
							{
								// Frind 초기화 //
								lpRebind->InitializeFriendInfo(lpTargetCharacterData->GetServerID(), lpTargetCharacterData->GetGID(),
									lpTargetCharacterData->GetClass(), lpTargetCharacterData->GetLevel());
							}							
						}
						else
						{
							lpRebind->UpdateServerID(0);
						}
					}
					else
					{
						lpRebind->UpdateServerID(0);
					}
				}
			}
            
            // 친구 리스트 전송
            dwBufferSize = MAX_BUFFER_SIZE;
            if(friendList.SerializeOut(szBuffer, dwBufferSize))
            {
                SendPacket::Friend(SendStream, PktFriendDB::CMD::FRIEND_LIST, dwRequestKey, dwUID, dwCID, 
                    dwGID, wClass, cLevel, SessionData.GetServerID(), szBuffer, dwBufferSize);				
            }

			// Ban 정보 설정 (시간이 생기면 여기도 수정하겠음) //

			CBanList banList = lpCharacterData->GetBanList();

			// 거부리스트에 CID 만 가지고 온다. //
			banList.GetCIDList(dwMemberCID);

			for(unsigned char cIndex = 0; cIndex < banList.GetBanNum(); cIndex++)
			{
				DataStorage::CSessionData* lpTargetSessionData = DataStorage::CSessionDataMgr::GetInstance().GetCharLoadedSession(dwMemberCID[cIndex]);

				CBanList::Rebind* lpRebind	= banList.GetBan(dwMemberCID[cIndex]);

				if(lpRebind)				
				{					
					if(lpTargetSessionData)
					{
						DataStorage::CCharacterData* lpTargetCharacterData = lpTargetSessionData->GetCharacterData();					

						if(lpTargetCharacterData)
						{
							if (DataStorage::CSessionData::SE_USER_ENABLED != lpTargetSessionData->GetSessionState() &&
								DataStorage::CSessionData::SE_CHAR_ENABLED != lpTargetSessionData->GetSessionState())
							{
								lpRebind->UpdateServerID(0);
							}
							else
							{
								// Frind 초기화 //
								lpRebind->InitializeBanInfo(lpTargetCharacterData->GetServerID(), lpTargetCharacterData->GetGID(),
									lpTargetCharacterData->GetClass(), lpTargetCharacterData->GetLevel());
							}
							
						}
						else
						{
							lpRebind->UpdateServerID(0);
						}
					}
					else
					{
						lpRebind->UpdateServerID(0);
					}
				}
			}

            // 거부 리스트 전송
            dwBufferSize = MAX_BUFFER_SIZE;
            if(banList.SerializeOut(szBuffer, dwBufferSize))
            {				
                SendPacket::Friend(SendStream, PktFriendDB::CMD::BAN_LIST, dwRequestKey, dwUID, dwCID, 
                    dwGID, wClass, cLevel, SessionData.GetServerID(), szBuffer, dwBufferSize);
            }

            // 퀘스트 전송
            const QUEST& quest = lpCharacterData->GetQuest();
            const HISTORY& history = lpCharacterData->GetHistory();

            SendPacket::Quest(SendStream, dwRequestKey, dwUID, dwCID, 
                quest.Data, quest.dwSize, history.Data, history.dwSize);

            // 설정 전송
            const CONFIG& config = lpCharacterData->GetConfig();
            SendPacket::ConfigInfo(SendStream, dwRequestKey, dwUID, dwCID, config.Data, config.dwSize);

            const STORE_INFO& storeInfo = lpStoreData->GetStoreInfo();

            // 창고 패스워드 및 창고 금액 전송. 반드시 DepositDBUpdate보다 먼저 보내야 한다.
            SendPacket::DepositData(SendStream, PktDeposit::CMD::PASSWORD, dwRequestKey, dwCID, storeInfo); 
            SendPacket::DepositData(SendStream, PktDeposit::CMD::GOLD,     dwRequestKey, dwCID, storeInfo); 

            // 창고 전송
            const STORE& store1 = lpStoreData->GetStore1();
            const STORE& store2 = lpStoreData->GetStore2();

            if (0 == store2.dwSize)
            {
                SendPacket::DepositDBUpdate(SendStream, dwRequestKey, dwUID, dwCID, 
                    PktDepositUpdateDB::TabNum::TAB_12, true, storeInfo, store1); 
            }
            else
            {
                SendPacket::DepositDBUpdate(SendStream, dwRequestKey, dwUID, dwCID, 
                    PktDepositUpdateDB::TabNum::TAB_12, false, storeInfo, store1); 

                SendPacket::DepositDBUpdate(SendStream, dwRequestKey, dwUID, dwCID, 
                    PktDepositUpdateDB::TabNum::TAB_34, true, storeInfo, store2); 
            }

            // 파티 로그인 처리
            CPartyDBInfo* lpParty = CPartyDBMgr::GetInstance().OpenParty(
                CDBSingleObject::GetInstance(), lpCharacterData->GetPID());

            if (0 != lpParty)
            {				
                CPartyDBMgr::GetInstance().LoginPartyMember(
                    lpParty, dwUID, dwCID, dwGID, wClass, SessionData.GetServerID(), cLevel);
            }

            // 길드 로그인 처리
            if (0 != lpGuild)
            {
                lpGuild->SendLoginOutMember(dwCID, SessionData.GetServerID());
            }
            
			// 듀얼시 연사 방지 패킷.
			DataStorage::CSessionData::SaveEnemy* lpSaveEnemyInfo = SessionData.GetSaveEnemy();

			unsigned long dwSaveEnemySize = sizeof(PktSaveEnemy) + sizeof(DataStorage::CSessionData::SaveEnemy)*4;

			char* szSaveEnemyBuffer = SendStream.GetBuffer(dwSaveEnemySize);
					
			PktSaveEnemy* lpPktSaveEnemy = reinterpret_cast<PktSaveEnemy*>(szSaveEnemyBuffer);
			lpPktSaveEnemy->m_dwCID	= SessionData.GetCID();

			DataStorage::CSessionData::SaveEnemy* lpPktSaveEnemyInfo = NULL;

			unsigned long dwTick = ::GetTickCount();
						
			for(unsigned char cIndex = 0; cIndex < DataStorage::CSessionData::SaveEnemy::MAX_SAVING_ENEMY; cIndex++)
			{
				if(!cIndex)
				{
					lpPktSaveEnemy->m_dwAttackedCID	= lpSaveEnemyInfo[cIndex].m_dwCID;
					lpPktSaveEnemy->m_dwTick		= dwTick - lpSaveEnemyInfo[cIndex].m_dwTick;
					lpPktSaveEnemyInfo				= reinterpret_cast<DataStorage::CSessionData::SaveEnemy*>(szSaveEnemyBuffer+sizeof(PktSaveEnemy));
				}
				else
				{
					lpPktSaveEnemyInfo[cIndex-1].m_dwCID	= lpSaveEnemyInfo[cIndex].m_dwCID;
					lpPktSaveEnemyInfo[cIndex-1].m_dwTick	= 0;

					if(lpSaveEnemyInfo[cIndex].m_dwCID)
					{						
						lpPktSaveEnemyInfo[cIndex-1].m_dwTick	= dwTick - lpSaveEnemyInfo[cIndex].m_dwTick;
					}										
				}								
			}

			if(!SendStream.WrapHeader((unsigned short)dwSaveEnemySize, CmdSaveEnemy, 0, 0))			
			{
				return false;
			}

			// 조이스틱 키 정보.
			KeyInfo* pKeyInfo = SessionData.GetKeyInfo();

			if (pKeyInfo && pKeyInfo->m_cUsed)
			{
				char* lpBuffer = SendStream.GetBuffer(sizeof(PktKeyInfo));

				if(0 != lpBuffer)
				{
					PktKeyInfo* pPktKeyInfo = reinterpret_cast<PktKeyInfo*>(lpBuffer);;
					pPktKeyInfo->m_dwCID = dwCID;
					memcpy(pPktKeyInfo->m_dwKeyInfo, pKeyInfo->m_dwKeyInfo, sizeof(unsigned long)*PktKeyInfo::MAX_KEY_INFO);

					SendStream.WrapCrypt(sizeof(PktKeyInfo), CmdKeyInfo, 0, 0);
				}				
			}			

            // 로그 출력
            SessionData.LogCharData("캐릭터 로그인");
        }
    }

    return true;
}


// 캐릭터 로그인 실패.
bool CharLoginError(CSendStream& SendStream, unsigned long dwRequestKey,
                    DBUpdateData::UpdateType eUpdateType,  
                    unsigned long dwUID, unsigned long dwCID, unsigned short usError)
{
    if(0 != usError)
    {
        char* lpBuffer = SendStream.GetBuffer(sizeof(PktDBUpdate));

        if(0 != lpBuffer)
        {
            PktDBUpdate* lpPktDBUpdate = reinterpret_cast<PktDBUpdate*>(lpBuffer);
            memset(lpPktDBUpdate, 0, sizeof(PktDBUpdate)); 

            lpPktDBUpdate->m_dlItemSerial   = 0;
            lpPktDBUpdate->m_dwSessionID    = 0;
            lpPktDBUpdate->m_dwUserID       = dwUID;
            lpPktDBUpdate->m_dwCharID       = dwCID;
            lpPktDBUpdate->m_dwRequestKey   = dwRequestKey;

            lpPktDBUpdate->m_Address.S_un.S_addr = 0;

            lpPktDBUpdate->m_cAdminLevel        = 0;

			// WORK_LIST 2.4 계정 국적을 게임서버의 캐릭터가 가지도록 구현
			lpPktDBUpdate->m_cAccountNation     = 0;
            lpPktDBUpdate->m_cNameChangeCount   = 0;
			lpPktDBUpdate->m_cGuildWarFlag		= 0;
			lpPktDBUpdate->m_cRealmWarFlag		= 0;			
			lpPktDBUpdate->m_cTacticsFlag		= 0;

            lpPktDBUpdate->m_TypeCode           = static_cast<unsigned short>(eUpdateType);

            memset(lpPktDBUpdate->m_usUpdate, 0, 
                sizeof(unsigned short) * DBUpdateData::MAX_UPDATE_DB);

            return SendStream.WrapCrypt(sizeof(PktDBUpdate), CmdDBUpdateData, 0, usError);
        }
    }

    return false;
}





bool Friend(CSendStream& SendStream, unsigned char cCmd, unsigned long dwRequestKey, unsigned long dwUID, unsigned long dwCID, 
            unsigned long dwGID, unsigned short wClass, char cLevel, unsigned long dwServerID, const char* lpData, unsigned long dwDataLen)
{
    char* lpBuffer = SendStream.GetBuffer(sizeof(PktFriendDB) + dwDataLen);
    if(0 != lpBuffer)
    {
        PktFriendDB* lpFriendDB = reinterpret_cast<PktFriendDB*>(lpBuffer);

        lpFriendDB->m_cCmd				= cCmd;
        lpFriendDB->m_dwRequestKey		= dwRequestKey;
        lpFriendDB->m_wCmd				= 0;

        lpFriendDB->m_dwOwnerUID		= dwUID;
        lpFriendDB->m_dwOwnerCID		= dwCID;
        lpFriendDB->m_dwReferenceCID	= 0;
        lpFriendDB->m_dwData			= dwDataLen;
		
		lpFriendDB->m_dwGID				= dwGID;
		lpFriendDB->m_wClass			= wClass;
		lpFriendDB->m_cLevel			= cLevel;
		lpFriendDB->m_dwServerID		= dwServerID;

        memcpy(lpFriendDB + 1, lpData, dwDataLen);

        return SendStream.WrapHeader(
            static_cast<unsigned short>(sizeof(PktFriendDB) + dwDataLen), CmdFriendDB, 0, 0);
    }

    return false;
}

bool Quest(CSendStream& SendStream, unsigned long dwRequestKey, 
           unsigned long dwUID, unsigned long dwCID, 
           const char* lpQuestBuffer, unsigned long dwQuestSize, 
           const char* lpHistoryBuffer, unsigned long dwHistorySize)
{
    char* lpBuffer = SendStream.GetBuffer(sizeof(PktQuestDB) + dwQuestSize + dwHistorySize);

    if(0 != lpBuffer)
    {
        PktQuestDB* lpPktQuestDB = reinterpret_cast<PktQuestDB*>(lpBuffer);

        lpPktQuestDB->m_dwRequestKey = dwRequestKey;
        lpPktQuestDB->m_wCmd   = 0;

        lpPktQuestDB->m_dwUID	= dwUID;
        lpPktQuestDB->m_dwCID	= dwCID;

        lpPktQuestDB->m_wExecuteQuestSize = static_cast<unsigned short>(dwQuestSize);	
        lpPktQuestDB->m_wHistoryQuestSize = static_cast<unsigned short>(dwHistorySize);	
         
        memcpy(lpBuffer + sizeof(PktQuestDB), lpQuestBuffer, dwQuestSize); 
        memcpy(lpBuffer + sizeof(PktQuestDB) + dwQuestSize, lpHistoryBuffer, dwHistorySize);

        return SendStream.WrapHeader(
            static_cast<unsigned short>(sizeof(PktQuestDB) + dwQuestSize + dwHistorySize),
            CmdQuestDB, 0, 0);
    }

    return false;
}

bool ConfigInfo(CSendStream& SendStream, unsigned long dwRequestKey, 
                unsigned long dwUID, unsigned long dwCID, 
                const char* lpConfigBuffer, unsigned long dwConfigSize)
{
    char* lpBuffer = SendStream.GetBuffer(sizeof(PktConfigInfo) + dwConfigSize);

    if(0 != lpBuffer)
    {
        PktConfigInfo* lpConfigInfo = reinterpret_cast<PktConfigInfo*>(lpBuffer);

        lpConfigInfo->m_dwRequestKey = dwRequestKey;
        lpConfigInfo->m_wCmd = 0;

        lpConfigInfo->m_dwUserID = dwUID;
        lpConfigInfo->m_dwCharID = dwCID;
        lpConfigInfo->m_wSize = static_cast<unsigned short>(dwConfigSize);

        memcpy(lpBuffer + sizeof(PktConfigInfo), lpConfigBuffer, dwConfigSize);

        return SendStream.WrapHeader(
            static_cast<unsigned short>(sizeof(PktConfigInfo) + dwConfigSize), CmdConfigInfoDB, 0, 0);
    }

    return false;
}

bool DepositData(CSendStream& SendStream, unsigned char cCmd, unsigned long dwRequestKey, 
                 unsigned long dwCID, const STORE_INFO& storeInfo)
{
    char* lpBuffer = SendStream.GetBuffer(sizeof(PktDeposit));

    if(0 != lpBuffer)
    {
        PktDeposit* lpPktDeposit = reinterpret_cast<PktDeposit*>(lpBuffer);

        lpPktDeposit->m_cCmd = cCmd;
        lpPktDeposit->m_wCmd = 0;
        lpPktDeposit->m_dwRequestKey = dwRequestKey;

        if (PktDeposit::CMD::PASSWORD == cCmd)
        {
            memcpy(&lpPktDeposit->m_szData[0], &dwCID, sizeof(unsigned long)); 
            memcpy(&lpPktDeposit->m_szData[4], storeInfo.Password, Deposit::PASSWORD_LENGTH); 
        }
        else if (PktDeposit::CMD::GOLD == cCmd)
        {
            memcpy(&lpPktDeposit->m_szData[0], &dwCID, sizeof(unsigned long)); 
            memcpy(&lpPktDeposit->m_szData[4], &storeInfo.Gold, sizeof(unsigned long)); 
        }

        return SendStream.WrapHeader(sizeof(PktDeposit), CmdDeposit, 0, 0);
    }

    return false;
}

bool DepositDBUpdate(CSendStream& SendStream, unsigned long dwRequestKey,
                     unsigned long dwUID, unsigned long dwCID,
                     unsigned char cTabNum, bool bComplete,
                     const STORE_INFO& storeInfo, const STORE& store)
{
    char* lpBuffer = SendStream.GetBuffer(sizeof(PktDepositUpdateDB) + store.dwSize);

    if(0 != lpBuffer)
    {
        PktDepositUpdateDB* lpPktDepositUpdateDB = 
            reinterpret_cast<PktDepositUpdateDB*>(lpBuffer);

        lpPktDepositUpdateDB->m_dwRequestKey    = dwRequestKey;
        lpPktDepositUpdateDB->m_wCmd            = 0;

        lpPktDepositUpdateDB->m_dwUID			= dwUID;
        lpPktDepositUpdateDB->m_dwCID			= dwCID;
        lpPktDepositUpdateDB->m_dwTabFlag		= storeInfo.Flag;
        lpPktDepositUpdateDB->m_usDataSize	    = static_cast<unsigned short>(store.dwSize);
        lpPktDepositUpdateDB->m_cTabNum		    = cTabNum;
        lpPktDepositUpdateDB->m_bUpdateComplete	= bComplete;

        memcpy(lpBuffer + sizeof(PktDepositUpdateDB), store.Data, store.dwSize); 

        return SendStream.WrapHeader(
            static_cast<unsigned short>(sizeof(PktDepositUpdateDB) + store.dwSize), CmdDepositUpdate, 0, 0);
    }

    return false;
}






}
}