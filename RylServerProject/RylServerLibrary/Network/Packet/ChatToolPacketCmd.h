#ifndef _CHAT_TOOL_PACKET_COMMAND_H_
#define _CHAT_TOOL_PACKET_COMMAND_H_

#include <Network/Packet/PacketBase.h>
#include <Network/Packet/PacketCommand.h>

#include <DB/DBDefine.h>

//! 채팅 모니터링 툴 패킷 및 패킷 커맨드를 정의하는 파일이다.

namespace ChatToolCMD
{
    enum CMD
    {
        Authorize       = 1,
        ChangeOption    = 2,
        ChatSend        = 3,
        ChatRequest     = 4,
        ChatAdminStatus = 5,
        ChatPing        = CmdSysPing
    };
};


#pragma pack(1)

namespace ChatToolPkt
{
    enum Const
    {
        MAX_USER_ID     = 16,
        MAX_PASSWORD    = 16,

        MAX_CHAR_NAME   = 16,

        TARGET_UID          = 0,
        TARGET_CHARNAME     = 1,
        TARGET_ACCOUNTNAME  = 2
    };

    //! Request : fill UserID/Passsword
    //! Result  : fill m_cResult( 1 : success / 0 : failed )
    struct Authorize : public PktBase
    {
        char m_szUserID[MAX_USER_ID];
        char m_szPassword[MAX_PASSWORD];
        unsigned char m_cResult;
    };

    //! ChatServer -> ChatTool   : fill all data + chatting message;
    struct ChatDataSend : public PktBase
    {
        unsigned long   m_dwUID;
        unsigned long   m_dwCID;
        unsigned long   m_dwServerID;		// 서버 ID가 0인 경우는 모니터링 툴에서 보냈을때이다.
											// 이때는 UID/CID가 툴에서 보낸 메시지를 받은 사람의 UID/CID값이 된다.

        char            m_szSenderName[CHAR_INFOST::MAX_NAME_LEN];
        char            m_szTargetName[CHAR_INFOST::MAX_NAME_LEN];

        unsigned short  m_usXPos;
        unsigned short  m_usZPos;

		unsigned short  m_usLang;

        unsigned char   m_cChatType;
        unsigned char   m_cChatMsgLen;

		unsigned char	m_cRace;		// 종족이 머냐
    };

    //! ChatTool -> ChatServer  : 채팅을 보낼 때 사용
    struct ChatRequest : public PktBase
    {
        enum ERR
        {
            PACKET_ERROR   = 1,
            NONE_CHARACTER = 2,             // 캐릭터 없음

        };

        char            m_szAdminName[MAX_CHAR_NAME];   // 보내는 운영자 이름.
        char            m_szTargetName[MAX_CHAR_NAME];  // 귓속말을 보내는 경우, 누구에게 보낼지 세팅. (캐릭명 혹은 계정명)

		unsigned long   m_dwUID;                        // 대상 캐릭터 UID (TargetName이 없을 경우 사용)
        unsigned long   m_dwCID;                        // 대상 캐릭터 CID
        unsigned long   m_dwMessageID;                  // 메시지 ID. 그대로 돌려준다.
        
        unsigned char   m_cTargetType;                  // UID / CharName / AccountName 중 하나

        unsigned char   m_cChatType;
        unsigned char   m_cChatMsgLen;
    };

    //! Request : fill m_cChatOption
    //! Result  : none
    struct ChatOption : public PktBase
    {
        enum Options
        {
            DO_LISTEN           = 0,        // 들을거면 무조건 1로 세팅.
            LISTEN_NORMAL_CHAT	= 1,
		    LISTEN_PARTY_CHAT   = 2,
            LISTEN_FRIEND_CHAT	= 3,
		    LISTEN_GUILD_CHAT   = 4,
		    
		    LISTEN_STALL_CHAT	= 5,
		    LISTEN_SHOUT_CHAT   = 6,

            LISTEN_ADMIN_NORMAL_CHAT = 7,   // 운영자 통상 채팅     (Ack로만 사용)
            LISTEN_ADMIN_SHOUT       = 8,   // 운영자 외치기        (Ack로만 사용)

            LISTEN_WHISPER_CHAT = 9,

		    LISTEN_NOTICE       = 10,
            LISTEN_TRADE_CHAT   = 11,

			LISTEN_DICE_CHAT	= 12,

            MAX_OPTION          = 20
        };

        unsigned char m_cChatOption[MAX_OPTION];
    };

    struct ChatAdminStatus : public PktBase
    {
        enum Status
        {
            LOGIN   = 0,
            LOGOUT  = 1
        };

        char            m_szChatAdminName[MAX_USER_ID];
        unsigned char   m_cChangeStatus;
    };
};
#pragma pack()

#endif