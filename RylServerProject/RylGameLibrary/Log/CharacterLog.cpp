#include "stdafx.h"
#include "CharacterLog.h"


void GAMELOG::LogCharLoginOut(const unsigned long dwUID, CCharacter* lpCharacter_In, 
                              const SOCKADDR_IN* lpSockAddr, const char* lpCharacterInfo, 
                              const int nTotalSize, const unsigned short* usUpdates, 
                              const unsigned char cCMD, const unsigned short eError)
{
    CGameLog& gameLog = CGameLog::GetInstance();
    
    static const unsigned long MAX_DEPOSIT_SIZE = 16000;

	//	Login, Logout Log
	sCharLoginOut* lpCharLog = reinterpret_cast<sCharLoginOut*>(gameLog.ReserveBuffer(
		static_cast<unsigned short>(sizeof(sCharLoginOut) + nTotalSize + MAX_DEPOSIT_SIZE)));

	unsigned long		dwCID = 0;
	Position	CurrentPos;

	if(NULL != lpCharLog && NULL != lpCharacter_In)
	{        
		char* lpBuffer  = reinterpret_cast<char*>(lpCharLog) + sizeof(sCharLoginOut);
        
        lpCharLog->InitLogBase(dwUID, lpCharacter_In->GetCID(), lpCharacter_In->GetCurrentPos(), 
            gameLog.GetLogTime(), cCMD, static_cast<ERRType>(eError));

		memcpy(lpBuffer, lpCharacterInfo, nTotalSize);
        
        unsigned long dwDepositSize = MAX_DEPOSIT_SIZE;
        lpCharacter_In->GetDeposit().LogUpdate(lpBuffer + nTotalSize, dwDepositSize);

        lpCharLog->InitCharLog(lpSockAddr, lpCharacter_In->IsAdmin(), usUpdates, 
            static_cast<unsigned short>(dwDepositSize), lpCharacter_In->GetDeposit().GetGold());

		gameLog.UseComplete(static_cast<unsigned short>(sizeof(sCharLoginOut) + nTotalSize + dwDepositSize));
	}
	else
	{
		char* szBuffer = NULL;
		switch(cCMD)
		{
		case CMD::CHAR_LOGIN:		szBuffer = "로그인";		    break;
		case CMD::CHAR_LOGOUT:		szBuffer = "로그아웃";	        break;
		case CMD::CHAR_DBUPDATE:	szBuffer = "DBUpdate";	        break;
		default:					szBuffer = "알 수 없는 커맨드";	break;
		}

        ERRLOG4(g_Log, "CID:0x%08x(0x%p) 의 %s:0x%02x 로그를 남길 수 없습니다. : 버퍼를 할당할 수 없습니다.", 
			dwCID, lpCharacter_In, szBuffer, cCMD);
	}
}


void GAMELOG::LogCharCreateDelete(const unsigned long dwUID, const SOCKADDR_IN* lpSockAddr, const unsigned long dwCID,
                                  const unsigned char cCMD, const unsigned short eError)
{
    CGameLog& gameLog = CGameLog::GetInstance();

	// 캐릭터 생성/삭제 로그 남기기
	sCharCreateDelete* lpCharLog = reinterpret_cast<sCharCreateDelete*>(gameLog.ReserveBuffer(
		static_cast<unsigned short>(sizeof(sCharCreateDelete))));

	if(NULL != lpCharLog)
	{
		lpCharLog->InitLogBase(dwUID, dwCID, Position(), gameLog.GetLogTime(), cCMD, static_cast<ERRType>(eError));
		lpCharLog->InitCharLog(lpSockAddr);
		gameLog.UseComplete(static_cast<unsigned short>(sizeof(sCharCreateDelete)));
	}
	else
	{
		char* szBuffer = NULL;
		switch(cCMD)
		{
		case CMD::CHAR_CREATE:	szBuffer = "캐릭터 생성";	    break;
		case CMD::CHAR_DELETE:	szBuffer = "캐릭터 삭제";	    break;
		default:				szBuffer = "알 수 없는 커맨드"; break;
		}

        ERRLOG4(g_Log, "UID:0x%08x CID:0x%08x 의 %s:%2d로그를 남길 수 없습니다.", dwUID, dwCID, szBuffer, cCMD);
	}
}

void GAMELOG::LogCharLevelUp(const CCharacter& character, const unsigned short usIP, 
                             const unsigned char cLevel, const unsigned short eError)
{
    CGameLog& gameLog = CGameLog::GetInstance();

	sCharLevelUpLog* pCharLevelUpLog = reinterpret_cast<sCharLevelUpLog*>(
		gameLog.ReserveBuffer(sizeof(sCharLevelUpLog)));

	if(NULL != pCharLevelUpLog)
	{
		pCharLevelUpLog->InitLogBase(character.GetUID(), character.GetCID(), character.GetCurrentPos(), 
			gameLog.GetLogTime(), GAMELOG::CMD::CHAR_LEVELUP, static_cast<ERRType>(eError));

		pCharLevelUpLog->InitCharLevelUpLog(usIP, cLevel);
		gameLog.UseComplete(sizeof(sCharLevelUpLog));
	}
	else
	{
		ERRLOG1(g_Log, "CID:0x%08x 캐릭터 레벨업 로그를 남길 수 없습니다. 버퍼를 할당할 수 없습니다.", 
			character.GetCID());
	}
}


void GAMELOG::LogCharBindPos(const CCharacter& character, const unsigned long dwNPCID, const unsigned short eError)
{
    CGameLog& gameLog = CGameLog::GetInstance();

	sCharBindPosLog* pCharBindPosLog = reinterpret_cast<sCharBindPosLog*>(
		gameLog.ReserveBuffer(sizeof(sCharBindPosLog)));
	
	if(NULL != pCharBindPosLog)
	{
		pCharBindPosLog->InitLogBase(character.GetUID(), character.GetCID(), character.GetCurrentPos(),
			gameLog.GetLogTime(), GAMELOG::CMD::CHAR_BIND_POS, static_cast<ERRType>(eError));

		pCharBindPosLog->InitCharBindPosLog(dwNPCID);

		gameLog.UseComplete(sizeof(sCharBindPosLog));
	}
	else
	{
		ERRLOG1(g_Log, "CID:0x%08x 위치 저장 로그를 남길 수 없습니다. 버퍼를 할당할 수 없습니다.", 
			character.GetCID());
	}
}


void GAMELOG::LogCharDead(const CCharacter& character, const DWORD64 dwPrevExp, const DWORD64 dwNextExp, 
                          const CAggresiveCreature* lpOffencer_In, const unsigned char cLevel, const unsigned short eError)
{
    CGameLog& gameLog = CGameLog::GetInstance();

	sCharDeadLog* pCharDeadLog = reinterpret_cast<sCharDeadLog*>(
        gameLog.ReserveBuffer(sizeof(sCharDeadLog)));

	const unsigned long dwOffencerCID = (0 != lpOffencer_In) ? lpOffencer_In->GetCID() : 0;

	if(NULL != pCharDeadLog)
	{
		pCharDeadLog->InitLogBase(character.GetUID(), character.GetCID(), character.GetCurrentPos(),
			gameLog.GetLogTime(), GAMELOG::CMD::CHAR_DEAD, static_cast<ERRType>(eError));

		pCharDeadLog->InitCharDeadLog(dwPrevExp, dwNextExp, dwOffencerCID, cLevel);

		gameLog.UseComplete(sizeof(sCharDeadLog));
	}
	else
	{
		ERRLOG1(g_Log, "CID:0x%08x 캐릭터 사망 로그를 남길 수 없습니다. 버퍼를 할당할 수 없습니다.",
			character.GetCID());
	}
}



void GAMELOG::LogCharRespawn(const CCharacter& character, const DWORD64 dwPrevExp, 
                             const DWORD64 dwNextExp, const unsigned short eError)
{
    CGameLog& gameLog = CGameLog::GetInstance();

	sCharRespawnLog* pCharRespawnLog = reinterpret_cast<sCharRespawnLog*>(
        gameLog.ReserveBuffer(sizeof(sCharRespawnLog)));

	if(NULL != pCharRespawnLog)
	{
		pCharRespawnLog->InitLogBase(character.GetUID(), character.GetCID(), character.GetCurrentPos(), 
			gameLog.GetLogTime(), GAMELOG::CMD::CHAR_RESPAWN, static_cast<ERRType>(eError));

		pCharRespawnLog->InitCharRespawnLog(dwPrevExp, dwNextExp);

		gameLog.UseComplete(sizeof(sCharRespawnLog));
	}
	else
	{
		ERRLOG1(g_Log, "CID:0x%08x 캐릭터 리스폰 로그를 남길 수 없습니다. 버퍼 용량이 부족합니다.", 
			character.GetCID());
	}
}


void GAMELOG::LogZoneMove(const CCharacter& character, 
                          unsigned char cZone, unsigned char cChannel, unsigned short usError)
{
    CGameLog& gameLog = CGameLog::GetInstance();

	sMoveZoneLog* lpZoneMoveLog = reinterpret_cast<sMoveZoneLog*>(
        gameLog.ReserveBuffer(sizeof(sMoveZoneLog)));

	if(NULL != lpZoneMoveLog)
	{
		lpZoneMoveLog->InitLogBase(character.GetUID(), character.GetCID(), character.GetCurrentPos(), 
            gameLog.GetLogTime(), GAMELOG::CMD::ZONE_MOVE, static_cast<ERRType>(usError));

        lpZoneMoveLog->m_cDstZone       = cZone;
		lpZoneMoveLog->m_cDstChannel    = cChannel;

		gameLog.UseComplete(sizeof(sMoveZoneLog));
	}
	else
	{
		ERRLOG1(g_Log, "CID:0x%08x 캐릭터 존 이동 로그를 남길 수 없습니다. 버퍼 용량이 부족합니다.", 
			character.GetCID());
	}
}

// MONSTER_DEAD
void GAMELOG::LogMonsterDead(const CCharacter& character, unsigned long dwMonsterCID, 
                             unsigned char cMonsterLevel, unsigned long* dwItemKind, unsigned long dwItemNum)
{
    CGameLog& gameLog = CGameLog::GetInstance();

    sMonsterDeadLog* lpMonsterDeadLog = reinterpret_cast<sMonsterDeadLog*>(
        gameLog.ReserveBuffer(static_cast<unsigned short>(sizeof(sMonsterDeadLog) + dwItemNum)));

    if(NULL != lpMonsterDeadLog)
    {
        lpMonsterDeadLog->InitLogBase(character.GetUID(), character.GetCID(), character.GetCurrentPos(), 
            gameLog.GetLogTime(), GAMELOG::CMD::MONSTER_DEAD, 0);

        lpMonsterDeadLog->m_dwMonsterCID    = dwMonsterCID;
        lpMonsterDeadLog->m_cMonsterLevel   = cMonsterLevel;
        lpMonsterDeadLog->m_cDropItemNum    = 0;
                
        unsigned long* dwDstItemKind = reinterpret_cast<unsigned long*>(lpMonsterDeadLog + 1);

        for(unsigned long dwCount = 0; dwCount < dwItemNum; ++dwCount)
        {
            if(0 != dwItemKind[dwCount])
            {
                *dwDstItemKind = dwItemKind[dwCount];
                ++dwDstItemKind;
                ++lpMonsterDeadLog->m_cDropItemNum;
            }
        }
        
        gameLog.UseComplete(sizeof(sMonsterDeadLog) + 
            sizeof(unsigned long) * lpMonsterDeadLog->m_cDropItemNum);
    }
    else
    {
        ERRLOG1(g_Log, "CID:0x%08x 캐릭터 존 이동 로그를 남길 수 없습니다. 버퍼 용량이 부족합니다.", 
            character.GetCID());
    }
}

// FAME_GET_BATTLE
void GAMELOG::LogFameGetBattle(const CCharacter& character, const CAggresiveCreature* lpEnemy_In, CCharacterParty* lpOurParty,
				const unsigned long dwPrevFame, const unsigned long dwNextFame, const unsigned long dwPrevMileage, const unsigned long dwNextMileage, const unsigned short eError)
{
	CGameLog& gameLog = CGameLog::GetInstance();

	GAMELOG::sFameGetBattleLog* pFameGetBattleLog = reinterpret_cast<sFameGetBattleLog*>(
		gameLog.ReserveBuffer(sizeof(sFameGetBattleLog)));

	const unsigned long dwEnemyCID = (0 != lpEnemy_In) ? lpEnemy_In->GetCID() : 0;
	const unsigned long dwOurPartyUID = (0 != lpOurParty) ? lpOurParty->GetUID() : 0;

	if(NULL != pFameGetBattleLog)
	{
		pFameGetBattleLog->InitLogBase(character.GetUID(), character.GetCID(), character.GetCurrentPos(),
			gameLog.GetLogTime(), GAMELOG::CMD::FAME_GET_BATTLE, static_cast<ERRType>(eError));

		pFameGetBattleLog->InitFameGetBattleLog(dwPrevFame, dwNextFame, dwPrevMileage, dwNextMileage, dwEnemyCID, dwOurPartyUID);

		gameLog.UseComplete(sizeof(sFameGetBattleLog));
	}
	else
	{
		ERRLOG1(g_Log, "CID:0x%08x 전투로 얻은 명성 로그를 남길 수 없습니다. 버퍼를 할당할 수 없습니다.",
			character.GetCID());
	}
}

// FAME_LOSE_BATTLE
void GAMELOG::LogFameLoseBattle(const CCharacter& character, const CAggresiveCreature* lpEnemy_In, CCharacterParty* lpOurParty,
				 const unsigned long dwPrevFame, const unsigned long dwNextFame, const unsigned short eError)
{
	CGameLog& gameLog = CGameLog::GetInstance();

	GAMELOG::sFameLoseBattleLog* pFameLoseBattleLog = reinterpret_cast<sFameLoseBattleLog*>(
		gameLog.ReserveBuffer(sizeof(sFameLoseBattleLog)));

	const unsigned long dwEnemyCID = (0 != lpEnemy_In) ? lpEnemy_In->GetCID() : 0;
	const unsigned long dwOurPartyUID = (0 != lpOurParty) ? lpOurParty->GetUID() : 0;

	if(NULL != pFameLoseBattleLog)
	{
		pFameLoseBattleLog->InitLogBase(character.GetUID(), character.GetCID(), character.GetCurrentPos(),
			gameLog.GetLogTime(), GAMELOG::CMD::FAME_LOSE_BATTLE, static_cast<ERRType>(eError));

		pFameLoseBattleLog->InitFameLoseBattleLog(dwPrevFame, dwNextFame, dwEnemyCID, dwOurPartyUID);

		gameLog.UseComplete(sizeof(sFameLoseBattleLog));
	}
	else
	{
		ERRLOG1(g_Log, "CID:0x%08x 전투로 잃은 명성 로그를 남길 수 없습니다. 버퍼를 할당할 수 없습니다.",
			character.GetCID());
	}
}

// FAME_GET_CAMP
void GAMELOG::LogFameGetCamp(const CCharacter& character, const unsigned long dwEnemyGID, 
					const unsigned long dwPrevFame, const unsigned long dwNextFame, const unsigned short eError)
{
	CGameLog& gameLog = CGameLog::GetInstance();

	GAMELOG::sFameGetCampLog* pFameGetCampLog = reinterpret_cast<sFameGetCampLog*>(
		gameLog.ReserveBuffer(sizeof(sFameGetCampLog)));


	if(NULL != pFameGetCampLog)
	{
		pFameGetCampLog->InitLogBase(character.GetUID(), character.GetCID(), character.GetCurrentPos(),
			gameLog.GetLogTime(), GAMELOG::CMD::FAME_GET_CAMP, static_cast<ERRType>(eError));

		pFameGetCampLog->InitFameGetCampLog(dwPrevFame, dwNextFame, dwEnemyGID);

		gameLog.UseComplete(sizeof(sFameGetCampLog));
	}
	else
	{
		ERRLOG1(g_Log, "CID:0x%08x 길드 요새로 얻은 명성 로그를 남길 수 없습니다. 버퍼를 할당할 수 없습니다.",
			character.GetCID());
	}
}

// FAME_LOSE_CAMP
void GAMELOG::LogFameLoseCamp(const CCharacter& character, const unsigned long dwEnemyGID, 
					 const unsigned long dwPrevFame, const unsigned long dwNextFame, const unsigned short eError)
{
	CGameLog& gameLog = CGameLog::GetInstance();

	GAMELOG::sFameLoseCampLog* pLogFameLoseCamp = reinterpret_cast<sFameLoseCampLog*>(
		gameLog.ReserveBuffer(sizeof(sFameLoseCampLog)));

	if(NULL != pLogFameLoseCamp)
	{
		pLogFameLoseCamp->InitLogBase(character.GetUID(), character.GetCID(), character.GetCurrentPos(),
			gameLog.GetLogTime(), GAMELOG::CMD::FAME_LOSE_CAMP, static_cast<ERRType>(eError));

		pLogFameLoseCamp->InitFameLoseCampLog(dwPrevFame, dwNextFame, dwEnemyGID);

		gameLog.UseComplete(sizeof(sFameGetCampLog));
	}
	else
	{
		ERRLOG1(g_Log, "CID:0x%08x 길드 요새로 잃은 명성 로그를 남길 수 없습니다. 버퍼를 할당할 수 없습니다.",
			character.GetCID());
	}
}

// QUEST_GET_REWARD
void GAMELOG::LogQuestGetReward(const CCharacter& character, const unsigned short wQuestID, const Item::CItem* lpItem,
								const unsigned long dwExp, const unsigned long dwGold, const unsigned long dwFame, const unsigned long dwMileage, const unsigned short eError)
{
	CGameLog& gameLog = CGameLog::GetInstance();

	GAMELOG::sQuestGetRewardLog* pQuestGetRewardLog = reinterpret_cast<sQuestGetRewardLog*>(
		gameLog.ReserveBuffer(sizeof(sQuestGetRewardLog)));

	const DWORD64 dwItemUID = (0 != lpItem) ? lpItem->GetUID() : 0;

	if(NULL != pQuestGetRewardLog)
	{
		pQuestGetRewardLog->InitLogBase(character.GetUID(), character.GetCID(), character.GetCurrentPos(),
			gameLog.GetLogTime(), GAMELOG::CMD::QUEST_GET_REWARD, static_cast<ERRType>(eError));

		pQuestGetRewardLog->InitQuestGetRewardLog(wQuestID, dwItemUID, dwExp, dwGold, dwFame, dwMileage);

		gameLog.UseComplete(sizeof(sQuestGetRewardLog));
	}
	else
	{
		ERRLOG1(g_Log, "CID:0x%08x 퀘스트 수행으로 얻은 보상 로그를 남길 수 없습니다.. 버퍼를 할당할 수 없습니다.",
			character.GetCID());
	}
}

