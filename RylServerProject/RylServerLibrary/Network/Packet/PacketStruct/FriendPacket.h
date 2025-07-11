#ifndef _CHAR_FRIEND_PACKET_H_
#define _CHAR_FRIEND_PACKET_H_

// CharFriendPacket.h
#include <Network/Packet/PacketBase.h>
#include <Network/Packet/PacketStruct/DataPacket.h>

#pragma pack(1)

/*
    struct PktFriendAddReq : public PktBase         CMD : 0x64
    struct PktFriendRemoveReq : public PktBase      CMD : 0x65
    struct PktFriendEtc : public PktBase            CMD : 0x66
    struct PktFriendAck : public PktBase            CMD : 0x67
    struct PktFriendDB : public PktBase             CMD : 0x68

    ģ�� ���� Operations 

    <Client->GameServer>

    ģ�� ��� : PktFriendAddReq
    �ź� ��� : PktFriendAddReq
    ģ�� ���� : PktFriendRemoveReq
    �ź� ���� : PktFriendRemoveReq
    �׷� ���� : PktFriendEtc(Ack����)
    
    <GameServer->Client>

    ģ�� ����Ʈ �ֱ� : PktFriendDB
    �ź� ����Ʈ �߱� : PktFriendDB

    ģ�� ��� Ack : PktFriendAck
    ģ�� ���� Ack : PktFriendAck
    �ź� ��� Ack : PktFriendAck
    �ź� ���� Ack : PktFriendAck
    
    ģ�� �α���   : PktFriendAck
    ģ�� �α׾ƿ� : PktFriendAck
    ģ�� ��ϴ��� : PktFriendAddReq
    

    <GameServer->DBAgent>

    ģ�� ��� : PktFriendDB
    �ź� ��� : PktFriendDB
    ģ�� ���� : PktFriendDB
    �ź� ���� : PktFriendDB
    �׷� ���� : PktFriendDB


    <DBAgent->GameServer>

    ģ�� ����Ʈ �ֱ� : PktFriendDB
    �ź� ����Ʈ �ֱ� : PktFriendDB
*/

struct DBFriend
{
	enum { MAX_NAME = 16 };

	unsigned long   m_dwCID;
	unsigned long   m_dwStatusFlag;
	char            m_szName[MAX_NAME];	
};

struct DBFriendInfo
{
	unsigned long	m_dwGID;				// �������.
	unsigned short	m_wClass;				// Ŭ��������.
	char			m_cLevel;				// ��������.
};

struct FriendInfo
{
private:

    enum StatusMask
    {
        IS_LOGINED  = 0x00000001,
        GROUP       = 0x000000F0,		
    };

public:

    enum
	{ 
		MAX_NAME	= 16,
		MOVE_ZONE	= 0x00000100,
	};
    
    unsigned long   m_dwCID;
    unsigned long   m_dwStatusFlag;
    char            m_szName[MAX_NAME];

	unsigned long	m_dwGID;				// �������.
	unsigned short	m_wClass;				// Ŭ��������.
	char			m_cLevel;				// ��������.

	unsigned long	m_dwServerID;			// ���� ����.

    FriendInfo() : m_dwCID(0), m_dwStatusFlag(0), m_dwGID(0), m_wClass(0), m_cLevel(0), m_dwServerID(0)	{ }

    bool IsLogined() { return (0 != (m_dwStatusFlag & IS_LOGINED)); }

	void SetLoginStatus(bool bLogined)
	{ 
		unsigned long dwMoveZone = 0;

		if(!bLogined)
		{
			if(m_dwStatusFlag&MOVE_ZONE )
				dwMoveZone = MOVE_ZONE;
		}

		m_dwStatusFlag = bLogined ? (m_dwStatusFlag | IS_LOGINED) : (m_dwStatusFlag & (~IS_LOGINED));
		m_dwStatusFlag = (m_dwStatusFlag - dwMoveZone);
	}

    unsigned long GetGroup() { return ((m_dwStatusFlag & GROUP) >> 4); }

	bool IsMoveZone() { if(m_dwStatusFlag & MOVE_ZONE) return true; return false; }

	void SetMoveZone(unsigned long dwMoveZone)  { m_dwStatusFlag = (m_dwStatusFlag | MOVE_ZONE); }

    bool SetGroup(unsigned long dwGroup) 
    {
        if(dwGroup <= 0xF) { m_dwStatusFlag = (m_dwStatusFlag & (~GROUP)) | (dwGroup << 4); return true; }
        return false;
    }
};

struct DBBan
{
	enum { MAX_NAME = 16 };

	unsigned long   m_dwCID;	
	char            m_szName[MAX_NAME];	
};

struct DBBanInfo
{
	unsigned long	m_dwGID;				// �������.
	unsigned short	m_wClass;				// Ŭ��������.
	char			m_cLevel;				// ��������.
};

struct BanInfo
{
    enum { MAX_NAME = 16 };
    unsigned long   m_dwCID;
    char            m_szName[MAX_NAME];

	unsigned long	m_dwGID;
	unsigned short	m_wClass;
	char			m_cLevel;

	unsigned long	m_dwServerID;	

    BanInfo() : m_dwCID(0) { }
};

// �߰� ��Ŷ
struct PktFriendAddReq : public PktBase
{
    enum { MAX_NAME = 16 };

    enum CMD
    {
        ADD_FRIEND_REQ      = 0, // ģ�� ���      (Client->GameServer), m_szName�� ����� ����� �̸�.
        BAN_FRIEND_REQ      = 1, // �ź� ���      (Client->GameServer), m_szName�� ����� ����� �̸�.
        ADDED_INFO          = 2  // ģ���� ��ϵ�  (GameServer->Client), m_szName�� ���� ����� ����� �̸�.
    };
   
    char            m_szName[MAX_NAME]; // ����� ��� �̸�.
    unsigned long   m_dwCID;            // ADDED_INFO�϶��� CID�� �־� �ش�.
    unsigned char   m_cCmd;
};


// ���� ��Ŷ
struct PktFriendRemoveReq : public PktBase
{
	enum { MAX_NAME = 16 };

    enum CMD
    {
        REMOVE_FRIEND_REQ   = 0, // ģ�� ����      (Client->GameServer), m_dwCID�� ������ ����� CID.
        REMOVE_BAN_REQ      = 1, // �ź� ��� ���� (Client->GameServer), m_dwCID�� ������ ����� CID.
		NOFRIEND_REQ		= 2, // ���ģ�� ����Ʈ���� �ڽ��� ����
    };
    
	char            m_szName[MAX_NAME]; // ������ ��� �̸�.
    unsigned long   m_dwCID;       // ����� CID;
    unsigned char   m_cCmd;
};


// ��Ÿ ��Ŷ
struct PktFriendEtc : public PktBase
{
    enum CMD
    {
        SETGROUP            = 0, // �׷� ���     (Client->GameServer), 
                                 // m_dwCID�� �׷� ������ ����� CID, m_dwData�� �ٲ� �׷� ��ȣ
        
    };

    unsigned long   m_dwCID;
    unsigned long   m_dwData;
    unsigned char   m_cCmd;
};


// ģ�� ���� ��Ŷ ( Client <--> GameServer )
struct PktFriendAck : public PktBase
{
    enum CMD
    {
        UNKNOWN_ERROR_ACK   = 0, // �� �� ���� Ŀ�ǵ� ���� (Ack)

        ADD_FRIEND_ACK      = 1, // ģ�� ��� Ack (GameServer->Client)
                                 // ������ ���� : SERVER_ERROR (���� ����)
                                 //               CLIENT_ERROR (Ŭ���̾�Ʈ�� �߸��� Ŀ�ǵ带 �ִ� ���)
                                 //               NOT_LOGINED  (�α��ε��� ���� ĳ���͸� ����Ϸ� ��)
                                 //               LIST_FULL    (ģ�� ����Ʈ�� �� �� ����)
                                 //               REJECTED     (����� �ź� ����Ʈ�� ���� ����)
                                 // m_dwCID : ��� ������ : ����� ����� CID, ���н� : 0

        MOVE_BAN_TO_FRIEND  = 2, // ģ�� ��� Ack (GameServer->Client) �ź� ����Ʈ���� �����ϰ� ģ���� �߰���.
                                 // ������ ���� : SERVER_ERROR (���� ����)
                                 //               CLIENT_ERROR (Ŭ���̾�Ʈ�� �߸��� Ŀ�ǵ带 �ִ� ���)
                                 //               NOT_LOGINED  (�α��ε��� ���� ĳ���͸� ����Ϸ� ��)
                                 //               LIST_FULL    (ģ�� ����Ʈ�� �� �� ����)
                                 //               REJECTED     (����� �ź� ����Ʈ�� ���� ����)
                                 // m_dwCID : ��� ������ : ����� ����� CID, ���н� : 0

        REMOVE_FRIEND_ACK   = 3, // ģ�� ���� Ack (GameServer->Client)
                                 // ������ ���� : SERVER_ERROR (���� ����)
                                 //               CLIENT_ERROR (Ŭ���̾�Ʈ�� �߸��� Ŀ�ǵ带 �ִ� ���)
                                 //               NOT_IN_LIST  (��û�ϴ� ĳ���Ͱ� ����Ʈ�� ��� ������ �� ����)
                                 // m_dwCID : 0

        ADD_BAN_ACK         = 4, // �ź� ��� Ack (GameServer->Client)
                                 // ������ ���� : SERVER_ERROR (���� ����)
                                 //               CLIENT_ERROR (Ŭ���̾�Ʈ�� �߸��� Ŀ�ǵ带 �ִ� ���)
                                 //               NOT_LOGINED  (�α��ε��� ���� ĳ���͸� ����Ϸ� ��)
                                 //               LIST_FULL    (ģ�� ����Ʈ�� �� �� ����)
                                 // m_dwCID : ��� ������ : ����� ����� CID, ���н� : 0

        MOVE_FRIEND_TO_BAN  = 5, // ģ�� �շ� Ack (GameServer->Client) ģ�� ����Ʈ���� �����ϰ� �źθ� �߰���.
                                 // ������ ���� : SERVER_ERROR (���� ����)
                                 //               CLIENT_ERROR (Ŭ���̾�Ʈ�� �߸��� Ŀ�ǵ带 �ִ� ���)
                                 //               NOT_LOGINED  (�α��ε��� ���� ĳ���͸� ����Ϸ� ��)
                                 //               LIST_FULL    (ģ�� ����Ʈ�� �� �� ����)
                                 //               REJECTED     (����� �ź� ����Ʈ�� ���� ����)
                                 // m_dwCID : ��� ������ : ����� ����� CID, ���н� : 0

        REMOVE_BAN_ACK      = 6, // �ź� ���� Ack (GameServer->Client)
                                 // ������ ���� : SERVER_ERROR (���� ����)
                                 //               CLIENT_ERROR (Ŭ���̾�Ʈ�� �߸��� Ŀ�ǵ带 �ִ� ���)
                                 //               NOT_IN_LIST  (��û�ϴ� ĳ���Ͱ� ����Ʈ�� ��� ������ �� ����)
                                 // m_dwCID : 0

        FRIEND_LOGIN        = 7, // ģ�� �α���   (GameServer->Client), m_dwCID�� �α����� ģ���� CID, m_szName�� �α����� ģ���� �̸�
        FRIEND_LOGOUT       = 8, // ģ�� �α׾ƿ� (GameServer->Client), m_dwCID�� �α׾ƿ��� ģ���� CID, m_szName�� �α����� ģ���� �̸�

        FRIEND_LOGIN_TO_GAME    = 9,    // ģ�� �α���      (Chat->GameServer)
                                        // m_dwCID�� �α����� ģ���� CID, m_szName�� �α����� ģ���� �̸�
                                        // ��Ŷ �ڿ� ĳ���� CID����(2byte unsigned long)
                                        // ĳ���� CID�� CID������ŭ ����´�. �ش� ĳ���͵鿡�� ��Ŷ�� �����ϸ� �ȴ�.

        FRIEND_LOGOUT_TO_GAME   = 10,   // ģ�� �α׾ƿ�    (Chat->GameServer)
                                        // m_dwCID�� �α׾ƿ��� ģ���� CID, m_szName�� �α����� ģ���� �̸�
                                        // ��Ŷ �ڿ� ĳ���� CID����(2byte unsigned long)
                                        // ĳ���� CID�� CID������ŭ ����´�. �ش� ĳ���͵鿡�� ��Ŷ�� �����ϸ� �ȴ�.

		FRIEND_INFO_UPDATE		= 11,	// ģ�� ���� ������Ʈ.

		BAN_INFO_UPDATE			= 12,	// �ź� ���� ������Ʈ.

		BAN_LOGIN				= 13,	// �ź� �α���.	(GameServer->Client)
		BAN_LOGOUT				= 14,	// �ź� �α׾ƿ�. (GameServer->Client)

		BAN_LOGIN_TO_GAME		= 15,	// �ź� �α���.	 (Chat->GameServer)
		BAN_LOGOUT_TO_GAME		= 16,   // �ź� �α׾ƿ�. (Chat->GameServer)

		// edith 2009.08.28 NoFriend �߰�
		NOFRIEND_ACK			= 17,	// ģ�� ���� Ack (GameServer->Client)
										// ������ ���� : SERVER_ERROR (���� ����)
										//               CLIENT_ERROR (Ŭ���̾�Ʈ�� �߸��� Ŀ�ǵ带 �ִ� ���)
										//               NOT_IN_LIST  (��û�ϴ� ĳ���Ͱ� ����Ʈ�� ��� ������ �� ����)
										// m_dwCID : 0
	};

    enum FriendERROR
    {
		SERVER_ERROR        = 1,    // ���� ����
		CLIENT_ERROR        = 2,    // Ŭ���̾�Ʈ�� �߸��� Ŀ�ǵ带 �ִ� ���
		NOT_LOGINED         = 3,    // �α������� �ʾ���
		LIST_FULL           = 4,    // ����Ʈ�� �� á��
		REJECTED            = 5,    // �źδ�����
		NOT_IN_LIST         = 6,	// ��û�ϴ� �۾��� ����Ʈ�� ����
		FAIL_ENEMY			= 7		// ������ �߰� �Ұ���
    };

    enum { MAX_NAME = 16 };
        
    unsigned long   m_dwCID;
    char            m_szName[MAX_NAME];

	unsigned long	m_dwGID;		// ��� ���̵�.
	unsigned short	m_wClass;		// Ŭ����.
	char			m_cLevel;		// ����.

	unsigned long	m_dwServerID;	// ���� ���̵�.

    unsigned char   m_cCmd;
};


// ģ�� ���� ��Ŷ
struct PktFriendDB : public PktDD
{
    enum CMD
    {
        ADD_FRIEND          = 0, // ģ�� ���     (GameServer->DBAgent) Owner�� Reference�� ���.
        REMOVE_FRIEND       = 1, // ģ�� ����     (GameServer->DBAgent) Owner�� Reference�� ����. �̶��� ReferenceUID = 0;
        ADD_BAN             = 2, // �ź� ���     (GameServer->DBAgent) Owner�� Reference�� ���. 
        REMOVE_BAN          = 3, // �ź� ����     (GameServer->DBAgent) Owner�� Reference�� ����. �̶��� ReferenceUID = 0;

        SETGROUP            = 4, // �׷� ����     (GameServer->DBAgent) 
                                 // Owner�� Reference�� m_dwData�� ��� �ִ� �׷����� ����. �̶��� ReferenceUID = 0;

        FRIEND_LIST         = 5, // ģ�� ����Ʈ   (DBAgent->GameServer, GameServer->Client)
                                 // FriendList�� �迭�� �ٴ´�.
                                 // Reference�� �ǹ� ����. m_dwData�� �ڿ� �ٴ� �������� ũ��.

        BAN_LIST            = 6, // �ź� ����Ʈ   (DBAgent->GameServer, GameServer->Client)
                                 // BanInfo�� �迭�� �ٴ´�.
                                 // Reference�� �ǹ� ����. m_dwData�� �ڿ� �ٴ� �������� ũ��.
		
		FRIEND_INFO_UPDATE	= 7, // ���� ������Ʈ.

		FRIEND_INFO			= 8, // ���� ������Ʈ.
		BAN_INFO			= 9, // ���� ������Ʈ.
    };

    unsigned long   m_dwOwnerUID;       // ��� �ϴ� ��� UID
    unsigned long   m_dwOwnerCID;       // ��� �ϴ� ��� CID
    unsigned long   m_dwReferenceUID;   // ��� �Ǵ� ��� UID
    unsigned long   m_dwReferenceCID;   // ��� �Ǵ� ��� CID
    unsigned long   m_dwData;           // Data

	unsigned long	m_dwGID;			// ��� �Ǵ� ��� ��� ����.
	unsigned short	m_wClass;			// ��� �Ǵ� ��� Ŭ���� ����.
	char			m_cLevel;			// ��� �Ǵ� ��� ���� ����.
	unsigned long	m_dwServerID;		// ��� �Ǵ� ��� ���� ����.

    unsigned char   m_cCmd;
};

#pragma pack()

#endif