#include "stdafx.h"
#include "GuildLog.h"
#include "LogCommands.h"

#include "GameLog.h"
#include "LogStruct.h"


// 기본 함수. 내부에서 호출됨
void LogGuild(unsigned char cCommand, unsigned char cType,
    unsigned long dwGID, unsigned long dwSrcCID, unsigned long dwDstCID, unsigned char cCmd,   
    const char* szExtraData, unsigned short usExtraDataSize, unsigned short usErrorCode)
{
    CGameLog& gameLog = CGameLog::GetInstance();

	//	MoveItem Log
    GAMELOG::sGuildLog* lpGuildLog = reinterpret_cast<GAMELOG::sGuildLog*>(
        gameLog.ReserveBuffer(sizeof(GAMELOG::sGuildLog) + usExtraDataSize));

	if (NULL != lpGuildLog)
	{
		unsigned short nUsage = static_cast<unsigned short>(sizeof(GAMELOG::sGuildLog));

		lpGuildLog->InitLogBase(0, dwSrcCID, Position(), 
            gameLog.GetLogTime(), cCommand, static_cast<GAMELOG::ERRType>(usErrorCode));

        lpGuildLog->m_dwGID     = dwGID;
        lpGuildLog->m_dwSrcCID  = dwSrcCID;
        lpGuildLog->m_dwDstCID  = dwDstCID;

        lpGuildLog->m_cType     = cType;   // Request or Result
		lpGuildLog->m_cCmd		= cCmd;
        lpGuildLog->m_cData     = 0;

        lpGuildLog->m_usExtraDataSize = 0;
        
        if(0 != usExtraDataSize && 0 != szExtraData)
        {
            memcpy(lpGuildLog + 1, szExtraData, usExtraDataSize);
            nUsage += usExtraDataSize;

            lpGuildLog->m_usExtraDataSize = usExtraDataSize;
        }
		
		gameLog.UseComplete(nUsage);
	}
	else
	{
        ERRLOG5(g_Log, "GID:0x%08x/Command:0x%02x/Type:%d/SrcCID:0x%08x/DstCID:0x%08x/"
            "길드 로그를 남길 수 없습니다 : 버퍼를 할당할 수 없습니다.", 
            dwGID, cCommand, cType, dwSrcCID, dwDstCID);
	}
}


// 길드 생성
void GAMELOG::LogGuildCreate(unsigned char cType, unsigned long dwGID, 
                             unsigned long dwCreateCID, unsigned long dwCreateGold, 
                             unsigned short usErrorCode)
{
    LogGuild(CMD::GUILD_CREATE, cType, dwGID, dwCreateCID, 0, 0,
        reinterpret_cast<const char*>(&dwCreateGold), sizeof(unsigned long), usErrorCode);
}

// 길드 가입 신청
void GAMELOG::LogGuildJoin(unsigned char cType, unsigned long dwGID, 
                           unsigned long dwJoinCID, unsigned long dwFirstTitle, unsigned short usErrorCode)
{
    LogGuild(CMD::GUILD_JOIN, cType, dwGID, 
        dwJoinCID, 0,  0, reinterpret_cast<const char*>(&dwFirstTitle), sizeof(unsigned long), usErrorCode);
}

// 길드 탈퇴
void GAMELOG::LogGuildLeave(unsigned char cType, unsigned long dwGID, unsigned long dwPermitterCID, 
                            unsigned long dwLeaveCID, unsigned short usErrorCode)
{
    LogGuild(CMD::GUILD_LEAVE, cType, dwGID,
        dwPermitterCID, dwLeaveCID, 0, 0, 0, usErrorCode);
}

// 길드원 등급 변경
void GAMELOG::LogGuildMemberLevelAdjust(unsigned char cType, unsigned long dwGID,
                                        unsigned long dwPermitterCID, unsigned long dwMemberCID, 
                                        unsigned long dwGuildMemberLevel, unsigned short usErrorCode)
{
    LogGuild(CMD::GUILD_MEMBER_LEVEL, cType, dwGID, dwPermitterCID, dwMemberCID, 0,
        reinterpret_cast<const char*>(&dwGuildMemberLevel), sizeof(unsigned long), usErrorCode);
}

// 길드원 권한 설정
void GAMELOG::LogGuildRightsChange(unsigned char cType, unsigned long dwGID,
                                   unsigned long dwPermitterCID, const char* cGuildRights, 
                                   unsigned short usGuildRightsSize, unsigned short usErrorCode)
{
    LogGuild(CMD::GUILD_RIGHTS_CHANGE, cType, dwGID, dwPermitterCID, 0, 0,
        cGuildRights, usGuildRightsSize, usErrorCode);
}

// 길드 레벨 변경
void GAMELOG::LogGuildLevel(unsigned char cType, unsigned long dwGID, unsigned long dwPermitterCID, unsigned long dwGuildLevel, 
                            unsigned long dwPreChangeGold, unsigned long dwPostChangeGold, unsigned short usErrorCode)
{
    char szData[sizeof(unsigned long) * 3];
    char* szDataCopyPtr = szData;
    
    memcpy(szDataCopyPtr, &dwGuildLevel, sizeof(unsigned long));
    szDataCopyPtr += sizeof(unsigned long);

    memcpy(szDataCopyPtr, &dwPreChangeGold, sizeof(unsigned long));
    szDataCopyPtr += sizeof(unsigned long);

    memcpy(szDataCopyPtr, &dwPostChangeGold, sizeof(unsigned long));
    szDataCopyPtr += sizeof(unsigned long);

    LogGuild(CMD::GUILD_LEVEL_ADJUST, cType, dwGID, dwPermitterCID, 0, 0, 
        szData, sizeof(unsigned long) * 3, usErrorCode);
}


// 길드마크 등록/변경
void GAMELOG::LogGuildMarkChange(unsigned char cType, unsigned long dwGID, unsigned long dwChangerCID, 
                                 unsigned long dwPreChangeGold, unsigned long dwPostChangeGold,
                                 const char* szGuildMarkData, unsigned short usGuildMarkDataSize, 
                                 unsigned short usErrorCode)
{
    const int MAX_DATA = 4096;
    char szData[MAX_DATA];

    char* szDataCopyPtr = szData;
    memcpy(szDataCopyPtr, &dwPreChangeGold, sizeof(unsigned long));
    szDataCopyPtr += sizeof(unsigned long);

    memcpy(szDataCopyPtr, &dwPostChangeGold, sizeof(unsigned long));
    szDataCopyPtr += sizeof(unsigned long);

    if(0 != szGuildMarkData && usGuildMarkDataSize + (sizeof(unsigned long) * 2) < MAX_DATA)
    {
        memcpy(szDataCopyPtr, szGuildMarkData, usGuildMarkDataSize);
        szDataCopyPtr += usGuildMarkDataSize;
    }

    LogGuild(CMD::GUILD_MARK_ADJUST, cType, dwGID, dwChangerCID, 0, 0, szData,
        static_cast<unsigned short>(szDataCopyPtr - szData), usErrorCode);
}

// 길드 금고 변경
void GAMELOG::LogGuildStoreGoldChange(unsigned char cType, unsigned long dwGID, unsigned long dwChangerCID, 
                                      unsigned char cCmd, unsigned long dwPreChangeGold, unsigned long dwPostChangeGold, 
                                      unsigned short usErrorCode)
{
    const int MAX_DATA = sizeof(unsigned long) * 2;
    char szData[MAX_DATA];

    char* szDataCopyPtr = szData;
    memcpy(szDataCopyPtr, &dwPreChangeGold, sizeof(unsigned long));
    szDataCopyPtr += sizeof(unsigned long);

    memcpy(szDataCopyPtr, &dwPostChangeGold, sizeof(unsigned long));
    szDataCopyPtr += sizeof(unsigned long);

    LogGuild(CMD::GUILD_GOLD_CHANGE, cType, dwGID, dwChangerCID, 0, cCmd, szData,
        static_cast<unsigned short>(szDataCopyPtr - szData), usErrorCode);
}


// 길드 소멸됨
void GAMELOG::LogGuildDispose(unsigned char cType, unsigned long dwGID, const char* szDissolveReason,
                              const char* szGuildDestroyFileName, int nGuildDestroyLine, 
                              unsigned short usErrorCode)
{
    const int MAX_BUFFER = MAX_PATH * 3;
    char szData[MAX_BUFFER];
    memset(szData, 0, sizeof(char) * MAX_BUFFER);
    
    char* szDataPos = szData;
    memcpy(szDataPos, &nGuildDestroyLine, sizeof(int));
    szDataPos += sizeof(int);

    char szFileName[MAX_PATH] = "";
    char szExt[MAX_PATH] = "";

    if(0 == szDissolveReason)
    {
        szDissolveReason = "Unknown reason";
    }

    if(0 != szGuildDestroyFileName)
    {
        _splitpath(szGuildDestroyFileName, 0, 0, szFileName, szExt);
    }
    
    const size_t nBufferSize = MAX_BUFFER - (szDataPos - szData) - 1;
    _snprintf(szDataPos, nBufferSize, "%s : %s%s", szDissolveReason, szFileName, szExt);
    szDataPos[nBufferSize] = 0;
    szDataPos += nBufferSize + 1;
    
    LogGuild(CMD::GUILD_DISSOLVE, cType, dwGID, 0, 0, 0, szData, 
        static_cast<unsigned short>(szDataPos - szData), usErrorCode);
}


