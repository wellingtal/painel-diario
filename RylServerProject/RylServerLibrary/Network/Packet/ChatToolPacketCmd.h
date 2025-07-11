#ifndef _CHAT_TOOL_PACKET_COMMAND_H_
#define _CHAT_TOOL_PACKET_COMMAND_H_

#include <Network/Packet/PacketBase.h>
#include <Network/Packet/PacketCommand.h>

#include <DB/DBDefine.h>

//! ä�� ����͸� �� ��Ŷ �� ��Ŷ Ŀ�ǵ带 �����ϴ� �����̴�.

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
        unsigned long   m_dwServerID;		// ���� ID�� 0�� ���� ����͸� ������ ���������̴�.
											// �̶��� UID/CID�� ������ ���� �޽����� ���� ����� UID/CID���� �ȴ�.

        char            m_szSenderName[CHAR_INFOST::MAX_NAME_LEN];
        char            m_szTargetName[CHAR_INFOST::MAX_NAME_LEN];

        unsigned short  m_usXPos;
        unsigned short  m_usZPos;

		unsigned short  m_usLang;

        unsigned char   m_cChatType;
        unsigned char   m_cChatMsgLen;

		unsigned char	m_cRace;		// ������ �ӳ�
    };

    //! ChatTool -> ChatServer  : ä���� ���� �� ���
    struct ChatRequest : public PktBase
    {
        enum ERR
        {
            PACKET_ERROR   = 1,
            NONE_CHARACTER = 2,             // ĳ���� ����

        };

        char            m_szAdminName[MAX_CHAR_NAME];   // ������ ��� �̸�.
        char            m_szTargetName[MAX_CHAR_NAME];  // �ӼӸ��� ������ ���, �������� ������ ����. (ĳ���� Ȥ�� ������)

		unsigned long   m_dwUID;                        // ��� ĳ���� UID (TargetName�� ���� ��� ���)
        unsigned long   m_dwCID;                        // ��� ĳ���� CID
        unsigned long   m_dwMessageID;                  // �޽��� ID. �״�� �����ش�.
        
        unsigned char   m_cTargetType;                  // UID / CharName / AccountName �� �ϳ�

        unsigned char   m_cChatType;
        unsigned char   m_cChatMsgLen;
    };

    //! Request : fill m_cChatOption
    //! Result  : none
    struct ChatOption : public PktBase
    {
        enum Options
        {
            DO_LISTEN           = 0,        // �����Ÿ� ������ 1�� ����.
            LISTEN_NORMAL_CHAT	= 1,
		    LISTEN_PARTY_CHAT   = 2,
            LISTEN_FRIEND_CHAT	= 3,
		    LISTEN_GUILD_CHAT   = 4,
		    
		    LISTEN_STALL_CHAT	= 5,
		    LISTEN_SHOUT_CHAT   = 6,

            LISTEN_ADMIN_NORMAL_CHAT = 7,   // ��� ��� ä��     (Ack�θ� ���)
            LISTEN_ADMIN_SHOUT       = 8,   // ��� ��ġ��        (Ack�θ� ���)

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