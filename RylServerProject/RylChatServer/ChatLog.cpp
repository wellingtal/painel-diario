#include "stdafx.h"
#include "ChatLog.h"
#include "ChatToolServerDispatch.h"
#include <Log/ServerLog.h>

#include <Utility/Setup/ServerSetup.h>


CServerLog g_ChattingLog("ChattingLog", 0);

CChatLog& CChatLog::GetInstance()
{
    static CChatLog chatLog;
    return chatLog;
}

CChatLog::CChatLog()
:   m_dwLogNum(0LL)
{

}

CChatLog::~CChatLog()
{

}


unsigned __int64 CChatLog::GetLogDataSize() const 
{
    return g_ChattingLog.GetTotalLoggedSize();
}

static char szGroupName[20][20] = { "Ichman", "Ponpon", "Other", };

bool CChatLog::Log(ChatData::CCharInfo& charInfo, const char cZone, const char cChannel, const char* szChatMsg, PktChat::PktChatCmd eChatType, 
                   unsigned short usXPos, unsigned short usYPos, unsigned short usZPos, unsigned short usLang)
{
    SYSTEMTIME sysTime;
    GetLocalTime(&sysTime);

	int GroupID = CServerSetup::GetInstance().GetServerGroup() > 2 ? 2 : CServerSetup::GetInstance().GetServerGroup();

    g_ChattingLog.Log("[%04d-%02d-%02d %02d:%02d:%02d][ServerID:0x%08x][Group:%-15s]"
		"[Zone:%2d][CH:%2d][X:%4d][Z:%4d][UID:%10d][CID:%10d][Type:%2d][S:%-15s][T:%-15s][%s]\r\n",
        sysTime.wYear, sysTime.wMonth, sysTime.wDay, 
        sysTime.wHour, sysTime.wMinute, sysTime.wSecond,
        charInfo.GetServerID(), szGroupName[GroupID], cZone, cChannel, usXPos, usZPos,  
		charInfo.GetUID(), charInfo.GetCID(), eChatType + 1, charInfo.GetName(), "", szChatMsg);

    ++m_dwLogNum;
    
    CChatToolServerDispatch::GetDispatchTable().Process(
        CSendAllDispatch(charInfo, szChatMsg, NULL, usXPos, usZPos, eChatType, usLang, false));

    return true;
}

bool CChatLog::WhisperLog(ChatData::CCharInfo& charInfo, const char cZone, const char cChannel, const char* szChatMsg, const char* szTargetName,
                          unsigned short usXPos, unsigned short usYPos, unsigned short usZPos, unsigned short usLang, bool bIsChatToolAdmin)
{
    SYSTEMTIME sysTime;
    GetLocalTime(&sysTime);

	int GroupID = CServerSetup::GetInstance().GetServerGroup() > 2 ? 2 : CServerSetup::GetInstance().GetServerGroup();

    g_ChattingLog.Log("[%04d-%02d-%02d %02d:%02d:%02d][ServerID:0x%08x][Group:%-15s]"
		"[Zone:%2d][CH:%2d][X:%4d][Z:%4d][UID:%10d][CID:%10d][Type:%2d][S:%-15s][T:%-15s][%s]\r\n",
        sysTime.wYear, sysTime.wMonth, sysTime.wDay, 
        sysTime.wHour, sysTime.wMinute, sysTime.wSecond,
        charInfo.GetServerID(), szGroupName[GroupID], cZone, cChannel, usXPos, usZPos,  
		charInfo.GetUID(), charInfo.GetCID(), PktChat::WHISPER + 1, charInfo.GetName(), szTargetName, szChatMsg);

    ++m_dwLogNum;
    
    CChatToolServerDispatch::GetDispatchTable().Process(
        CSendAllDispatch(charInfo, szChatMsg, szTargetName,
        usXPos, usZPos, PktChat::WHISPER, usLang, bIsChatToolAdmin));

    return true;
}

bool CChatLog::AdminLog(unsigned long dwTargetUID, unsigned long dwTargetCID, 
						const char* szAdminID, const char* szSenderName, const char* szTargetName,
                        const char* szChatMsg, unsigned char cChatType, unsigned short usLang)
{
    if(0 != szAdminID && 0 != szChatMsg && 0 != szSenderName)
    {
        SYSTEMTIME  sysTime;
        GetLocalTime(&sysTime);

        if(0 != szTargetName)
        {
			g_ChattingLog.Log("[%04d-%02d-%02d %02d:%02d:%02d][ServerID:0x%08x][Group:%-15s]"
				"[Zone:%2d][CH:%2d][X:%4d][Z:%4d][UID:%10d][CID:%10d][Type:%2d][S:%-15s][T:%-15s][%s]\r\n",
                sysTime.wYear, sysTime.wMonth, sysTime.wDay, 
                sysTime.wHour, sysTime.wMinute, sysTime.wSecond,
				0, "Tool", 0, 0, 0, 0, 0, 0, cChatType + 1, szSenderName, szTargetName, szChatMsg);
        }
        else
        {
			g_ChattingLog.Log("[%04d-%02d-%02d %02d:%02d:%02d][ServerID:0x%08x][Group:%-15s]"
				"[Zone:%2d][CH:%2d][X:%4d][Z:%4d][UID:%10d][CID:%10d][Type:%2d][S:%-15s][T:%-15s][%s]\r\n",
                sysTime.wYear, sysTime.wMonth, sysTime.wDay, 
                sysTime.wHour, sysTime.wMinute, sysTime.wSecond,
				0, "Tool", 0, 0, 0, 0, 0, 0, cChatType + 1, szSenderName, "NOTICE", szChatMsg);
        }

        ++m_dwLogNum;
        
        CChatToolServerDispatch::GetDispatchTable().Process(
            CSendAdminDispatch(dwTargetUID, dwTargetCID, szSenderName, szTargetName, szChatMsg, cChatType, usLang));
    }

    return true;
}


void CChatLog::Flush()
{
    SERLOG0(g_ChattingLog, "로그를 Flush합니다");
}