#ifndef _CHAR_LOG_H_
#define _CHAR_LOG_H_

#include <winsock2.h>
#include <windows.h>

#include "GameLog.h"
#include "LogStruct.h"
#include "LogCommands.h"
#include <RylGameLibrary/Log/GameLog.h>
#include <Creature/AggresiveCreature.h>
#include <Creature/Character/Character.h>
#include <Item/Container/DepositCOntainer.h>
#include <Community/Party/Party.h>


// 전방 참조
class CCharacter;
class CCharacterParty;
class CAggresiveCreature;

namespace GAMELOG
{
    //-------------------------------------------------------------------------------------------
	// 캐릭터 관련 로그들.

	// CHAR_LOGIN, CHAR_LOGOUT, CHAR_DBUPDATE
	void LogCharLoginOut(const unsigned long dwUID, CCharacter* lpCharacter_In, const SOCKADDR_IN* lpSockAddr, 
        const char* lpCharacterInfo, const int nTotalSize, const unsigned short* usUpdates, 
        const unsigned char cCMD, const unsigned short eError);

	// CHAR_CREATE, CHAR_DELETE
	void LogCharCreateDelete(const unsigned long dwUID, const SOCKADDR_IN* lpSockAddr, const unsigned long dwCID,
        const unsigned char cCMD, const unsigned short eError);

	// CHAR_LEVELUP
    void LogCharLevelUp(const CCharacter& character, const unsigned short usIP, 
        const unsigned char cLevel, const unsigned short eError);

	// CHAR_BINDPOS
	void LogCharBindPos(const CCharacter& character, const unsigned long dwNPCID, const unsigned short eError);

	// CHAR_DEAD
	void LogCharDead(const CCharacter& character, const DWORD64 dwPrevExp, const DWORD64 dwNextExp, 
        const CAggresiveCreature* lpOffencer_In, const unsigned char cLevel, const unsigned short eError);

	// CHAR_RESPAWN
	void LogCharRespawn(const CCharacter& character, const DWORD64 dwPrevExp, 
        const DWORD64 dwNextExp, const unsigned short eError);

    // ZONE_MOVE
    void LogZoneMove(const CCharacter& character, unsigned char cZone, 
        unsigned char cChannel, unsigned short usError);

    // MONSTER_DEAD
    void LogMonsterDead(const CCharacter& character, unsigned long dwMonsterCID, 
        unsigned char cMonsterLevel, unsigned long* dwItemKind, unsigned long dwItemNum);

	// FAME_GET_BATTLE
	void LogFameGetBattle(const CCharacter& character, const CAggresiveCreature* lpEnemy_In, CCharacterParty* lpOurParty,
		const unsigned long dwPrevFame, const unsigned long dwNextFame, const unsigned long dwPrevMileage, const unsigned long dwNextMileage, const unsigned short eError);
    	
	// FAME_LOSE_BATTLE
	void LogFameLoseBattle(const CCharacter& character, const CAggresiveCreature* lpEnemy_In, CCharacterParty* lpOurParty,
		const unsigned long dwPrevFame, const unsigned long dwNextFame, const unsigned short eError);

	// FAME_GET_CAMP
	void LogFameGetCamp(const CCharacter& character, const unsigned long dwEnemyGID, 
		const unsigned long dwPrevFame, const unsigned long dwNextFame, const unsigned short eError);

	// FAME_LOSE_BATTLE
	void LogFameLoseCamp(const CCharacter& character, const unsigned long dwEnemyGID, 
		const unsigned long dwPrevFame, const unsigned long dwNextFame, const unsigned short eError);

	// QUEST_GET_REWARD
	void LogQuestGetReward(const CCharacter& character, const unsigned short wQuestID, const Item::CItem* lpItem,
		const unsigned long dwExp, const unsigned long dwGold, const unsigned long dwFame, const unsigned long dwMileage, const unsigned short eError);
};

#endif