#ifndef _CHAT_LOG_H_
#define _CHAT_LOG_H_

#include <Network/Packet/PacketStruct/CharCommunityPacket.h>
#include <Network/Broadcast/ChatCharData.h>
#include <Log/ServerLog.h>

extern CServerLog g_ChattingLog;

class CChatLog
{
public:

    static CChatLog& GetInstance();
    
    bool Log(ChatData::CCharInfo& charInfo, const char cZone, const char cChannel, const char* szChatMsg, PktChat::PktChatCmd eChatType, 
        unsigned short usXPos, unsigned short usYPos, unsigned short usZPos, unsigned short usLang);

    bool WhisperLog(ChatData::CCharInfo& charInfo, const char cZone, const char cChannel, const char* szChatMsg, const char* szTargetName,
        unsigned short usXPos, unsigned short usYPos, unsigned short usZPos, unsigned short usLang, bool bIsChatToolAdmin);

    bool AdminLog(unsigned long dwTargetUID, unsigned long dwTargetCID,
		const char* szAdminID, const char* szSendName, const char* szReceiveName,
		const char* szChatMsg, unsigned char cChatType, unsigned short usLang);
    
    void Flush();

    unsigned __int64 GetLogDataSize() const;
    unsigned __int64 GetLogNum() const { return m_dwLogNum; } 

private:

    CChatLog();
    ~CChatLog();

    unsigned __int64 m_dwLogNum;
};

#endif