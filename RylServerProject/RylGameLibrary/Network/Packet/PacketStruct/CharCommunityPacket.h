#ifndef _CHAR_COMMUNITY_PACKET_H_
#define _CHAR_COMMUNITY_PACKET_H_

// CharCommunityPacket.h
// DBDefine�ʿ�.

#include <Network/Packet/PacketBase.h>
#include <Network/Packet/PacketStruct/DataPacket.h>
#include <Network/Packet/PacketStruct/CharItemPacketStruct.h>

#include "AddressPacket.h"
#include "CharCommunityPacketStruct.h"

#pragma pack(1)

// ��Ƽ ����
struct PktPI : public PktBase
{
	PARTY_EX	m_Party;
};


// ��Ƽ ���
//
//  ��� : 
//	0 = �ʴ�,	1 = ����,	2 = ����,
//	3 = ����,	4 = Ż��,	5 = �絵,
//	6 = �ı�,	7 = �α���,	8 = �α׾ƿ�,
//	9 = ����(ĳ����),	10 = ��α���,
//	11 = ��Ƽã��,		12 = ���ã��,
//	13 = ������� �ѱ�,	14 = ������� ����
//	15 = ��Ƽ ��� �� �̵�
//	16 = ��Ƽ ��� ���
//
//  ���� : 
//	1 = ���� ����,	2 = �̹� ��Ƽ�� ���ԵǾ� �ִ� ���, 3 = ���� ����
//	4 = ���� ����,	5 = Ż�� ����,	6 = �絵 ����, 7 = ��Ƽ �ο� �ʰ�,
//	8 = �ʴ� ����,	9 = �ź� �ɼ��� �� ����
//
struct PktPC : public PktBase
{
	enum PartyCmd 
	{ 
		PC_INVITE		= 0,	PC_ACCEPT			= 1,	PC_REFUSE			= 2, 
		PC_BANISH		= 3,	PC_SECESSION		= 4,	PC_TRANSFER			= 5,
		PC_DESTROY		= 6,	PC_LOGIN			= 7,	PC_LOGOUT			= 8,
		PC_DELETE		= 9,	PC_RELOGIN			= 10,	PC_FINDPARTY		= 11,
		PC_FINDMEMBER	= 12,	PC_AUTOROUTING_ON	= 13,	PC_AUTOROUTING_OFF	= 14,
		PC_MOVEZONE		= 15
	};

	enum PktPCError
	{	
		SERVER_ERROR	= 1,	CHAR_JOINED		= 2,	FAIL_ACCEPT		= 3,
		FAIL_BANISH		= 4,	FAIL_SECESSION	= 5,	FAIL_TRANSFER	= 6,
		OVER_MEMBER		= 7,	FAIL_INVITE		= 8,	FAIL_REJECT		= 9,

		ALREADY_PARTY	= 10,	FAIL_NOT_PARTY	= 11,	FAIL_NOT_LEADER	= 12
	};

	unsigned long	m_dwPartyID;        // ��Ƽ ���̵�
	unsigned long	m_dwSenderID;	    // ĳ���� ���̵�(��ü)
	unsigned long	m_dwReferenceID;	// ���� ���̵�(��� ĳ����/���� ���̵�)
   	unsigned short	m_wCmd;             // ���
};


// ��Ƽ ��� Ack
struct PktPCAck : public PktBase
{
	AddressInfo		m_SenderAddressInfo;        // Sender�� �ּ�
	char			m_strSenderName[CHAR_INFOST::MAX_NAME_LEN];  // Sender�� �̸�

	unsigned long	m_dwPartyID;                // ��Ƽ ���̵�
	unsigned long	m_dwSenderID;               // ĳ���� ���̵�(��ü)
	unsigned long	m_dwReferenceID;	        // ���� ���̵�(��� ĳ����/���� ���̵�)

	unsigned long	m_dwReferenceGID;			// ��Ƽ�� �ӽ� ��� ���̵�. 

    unsigned char	m_cCmd;                     // ���
};


// ��Ƽ ����
struct PktPCInfo : public PktBase 
{
	unsigned long	m_dwPartyID;                // ��Ƽ ID
	unsigned long	m_dwMemberID;	            // ��� ID
	unsigned char   m_cCmd;                     // ���
};

/*
// ��Ƽ ��� ������ (+ �������� ������)
struct PktMD : public PktBase 
{
	unsigned long	m_dwCharID;			// ĳ���� ���̵�
	unsigned short	m_wCmd;             // ���� ���
	unsigned short	m_wLen;             // ����
};


// ��Ƽ ��� ����
struct PktPM : public PktBase 
{
	SOCKADDR_IN		m_PublicAddress;
	SOCKADDR_IN		m_PrivateAddress;

	unsigned long	m_dwCharID;			// ĳ���� ���̵�
	unsigned long	m_dwPartyID;	    // ��Ƽ ���̵�

	unsigned long	m_dwChant;	        // æƮ ����
	unsigned long	m_dwEnchant;        // ��æƮ ����

    unsigned short	m_wMaxHP;	        // �ִ� HP
	unsigned short	m_sCurrHP;		    // ���� HP
	unsigned short	m_wMaxMP;		    // �ִ� MP
	unsigned short	m_sCurrMP;		    // ���� MP

	unsigned char	m_cClass;           // ĳ���� Ŭ����
	unsigned char	m_cLevel;           // ĳ���� ����
	POS				m_Position;			// ���� ��ǥ
};

// ��Ƽ ��� �ּ� Update
struct PktPMA : public PktBase
{
	SOCKADDR_IN		m_PublicAddress;
	SOCKADDR_IN		m_PrivateAddress;

	unsigned long	m_dwCharID;
	unsigned long	m_dwPartyID;
};
*/

// ��Ƽ ã��
struct PktPF : public PktBase 
{
	unsigned long m_dwCharID;			
};

// ��Ƽ ã�� Ack
struct PktPFAck : public PktBase 
{
	enum { MAX_LIST = 30, MAX_DIFF_LEVEL_VIEW = 14 };

	unsigned long m_dwCharID;			    // ĳ���� ���̵�
	unsigned char m_cMemberFindPartyNum;    // ��Ƽ ã�� ����Ʈ ��
	unsigned char m_cPartyFindMemberNum;    // ��Ƽ�� ã�� ����Ʈ ��
};

// ĳ���� ��(���嵵) ����
struct PktNameCheck : public PktBase //hz added for new packet
{
	char name[16];         // ĳ���� ���̵�
	unsigned long charID;
	int check;
};


// ĳ���� ��(���嵵) ����
struct PktFI : public PktBase 
{
	unsigned long	m_dwCharID;         // ĳ���� ���̵�
};


// ĳ���� ��(���嵵) ���� Ack
struct PktFIAck : public PktBase 
{
	enum 
	{	
		MAX_NAME_LEN = 16 
	};

	enum FIAckCmd
	{
		FAME_INFO					= 0,	// �Ϲ� ���� ��û

		FAME_LOSE_DUEL				= 1,	// �ڽ��� ��� ��뿡�� ���� ���� ���
		FAME_LOSE_CONTINUE			= 2,	// �ڽ��� ���� ���� ���� ���
		FAME_LOSE					= 3,	// �ڽ��� ���� ���ؼ� ���� �Ҵ� ���

		FAME_LOSE_MEMBER			= 4,	// ��Ƽ���� ���� ���� ���

		FAME_WIN_DUEL				= 5,	// �ڽ��� ��� ��븦 ������ ���
		FAME_WIN_LARGE_GAP			= 6,	// �ڽ��� ���������� ������ ū ���
		FAME_WIN_CONTINUE			= 7,	// �ڽ��� ���� ������ ���
		FAME_WIN					= 8,	// �ڽ��� �����Ͽ� ���� ���ϸ����� ��� ���

		FAME_WIN_MEMBER_DUEL		= 9,	// ��Ƽ���� ��� ��븦 ������ ���
		FAME_WIN_MEMBER_LARGE_GAP	= 10,	// ��Ƽ���� ���������� ������ ū ���
		FAME_WIN_MEMBER_CONTINUE	= 11,	// ��Ƽ���� ���� ������ ���
		FAME_WIN_MEMBER				= 12,	// ��Ƽ���� �����Ͽ� ���� ���ϸ����� ��� ���

		FAME_LOSE_LEVELMIN			= 13,	// �ڽ��� ������ ���� �������� ���� ���� ���
		FAME_WIN_LEVELMIN			= 14,	// �ڽ��� ������ ���� ������ ���ΰ��
		FAME_WIN_MEMBER_LEVELMIN	= 15	// ��Ƽ���� ������ ���� ������ ���ΰ��.
	};

	unsigned long	m_dwCharID;         // ĳ���� ���̵� (��û�� ���)

	unsigned long	m_cOtherNation;		// ���� ĳ���� ����
	unsigned char	m_cOtherClass;		// ���� ĳ���� Ŭ����

	unsigned long	m_dwFame;			// ���� ��
	unsigned long	m_dwMileage;		// ���� ���ϸ���

	unsigned char	m_cRankingByRace;   // ������ ����
	unsigned char	m_cRankingByClass;  // Ŭ������ ����

	unsigned char	m_cCmd;				// ���
	unsigned char   m_KillCount; //hayzohar added for killspree
	int				m_KillspreeLevel;
	char	m_szWinCharName[MAX_NAME_LEN];		// ������ ĳ���� �̸�
	char	m_szLoseCharName[MAX_NAME_LEN];		// ���ش��� ĳ���� �̸�
};

// Ŀ�ǵ� / ������ ���� / ������, ������, ������... / ä�� �޽���(�ִ� PktChatMaxSize)
struct PktChat : public PktBase 
{
    enum PktChatCmd
    {
        NORMAL		        = 0,    // ���� ĳ���� ��ǥ�� ������ �Ǵ�.
        PARTY		        = 1,    // ��Ƽ ä��
        FRIEND		        = 2,    // ģ�� ä�� (�̸� char[16] * m_cNum - MAX 100��)
        GUILD	            = 3,    // ��� ä��

        CLIENT_LOG	        = 5,
        STALL		        = 6,    // ������ ä�� (�̸� char[16] * m_cNum - MAX 10��)	
        SHOUT               = 7,    // ��ġ��

        ADMIN_NORMAL_CHAT   = 8,    // ��� ��� ä��     (Ack�θ� ���)
        ADMIN_SHOUT         = 9,    // ��� ��ġ��        (Ack�θ� ���)

        WHISPER             = 10,   // �� ��ȣ�� ������ �ʴ´�.Whisper�� ������ ��Ŷ�� ����ϱ� �����̴�. ��, �α׽ÿ� ���δ�.
        TRADE               = 11,   // ��ġ��� ���� (�� ���п����� �ٸ� Ŀ�ǵ�� ����)

		CAMP_SHOP			= 12,	// ��� ��� ���� ä�� (�̸� char[16] * m_cNum - MAX 10��)	

		NOTIFY_CHAR_INFO	= 13,	// ��� ��� hp

		FIND_PARTY			= 14,	// ��Ƽ ã��

		ENEMY_CHECK			= 15,	// ���Ʊ� �ĺ�.

		DICE				= 16,	// �ֻ��� ä�� (��Ƽä�ð� ����)

		GMREPORT			= 17,	// ���� ä��

        NOTICE		        = 255,

        PktChatMaxSize      = 180
    };

    enum ERR
    {
        SHOUT_TIMEOUT   = 2,		// ��ħ �ð� ���� �ɸ�
		CHAT_BAN		= 3			// ä�� ����
    };

    unsigned long   m_dwSenderCID;  // ä�� ������ CID
	unsigned short	m_usLang;		// �ý��� ������
    unsigned char	m_cCmd;         // Ŀ�ǵ�(Ŀ�ǵ忡 ���� ������ ũ�Ⱑ �ٸ�)
    unsigned char   m_cNum;	        // ������ ����
};


// ���Ӽ������� ä�ü����� ä�� ��û�� �� �� ���̴� ��Ŷ�̴�.
struct PktChatRequest : public PktBase 
{
    unsigned long   m_dwUID;    // ���� ID      (������ ��ſ����� ����)
    unsigned long   m_dwCID;    // ĳ���� ID    

	unsigned short	m_usLang;		// �ý��� ������

    unsigned short  m_usXPos;   // ĳ���� ��ǥ  (������ ��ſ����� ����)
    unsigned short  m_usYPos;   // ĳ���� ��ǥ  (������ ��ſ����� ����)
    unsigned short  m_usZPos;   // ĳ���� ��ǥ  (������ ��ſ����� ����)

	char			m_cZone;		// ĳ���� ��
    unsigned char	m_cCmd;         // Ŀ�ǵ�(Ŀ�ǵ忡 ���� ������ ũ�Ⱑ �ٸ�)
    unsigned char   m_cNum;	        // ������ ����
};

struct PktChatBanInfo : public PktBase 
{	
	unsigned long   m_dwCID;		// ĳ���� ID    	
};

// ĳ���� �ӻ��� ��Ŷ
struct PktWh : public PktBase 
{
	enum
    {	        
        FAIL_CHAR_LOGOUT	= 2,	// �α׾ƿ��� ĳ����
        FAIL_REJECT		    = 3,	// �Ӹ� �ź�
		FAIL_ENEMY			= 4,	// �������Դ� �Ӹ��� ���� �� ����.
		FAIL_CHAT_BAN		= 5,	// ä�� ����

        PktWhMaxSize        = PktChat::PktChatMaxSize
	};    

	char    m_SenderName[CHAR_INFOST::MAX_NAME_LEN];
	char	m_RecverName[CHAR_INFOST::MAX_NAME_LEN];
};


// ���Ӽ��� -> ä�ü����� ������ ����
struct PktChatData : public PktBase
{
    enum
    {
        LOGIN   = 0,
        LOGOUT  = 1,
        DELTA   = 2    
    };

    unsigned long 	m_dwUID;
    unsigned long 	m_dwCID;    
    unsigned char 	m_cType;
};


// ä�� ����
struct PktChatBan : public PktBase
{
	unsigned long 	m_dwAdminCID;	// ä���� �� ��� CID
	unsigned long	m_dwTargetCID;	// ä�� ����� CID
	unsigned long	m_dwMinutes;	// ä�� �ð� ( 0�̸� ���� )
};


// ĳ���� ������ ���� ��Ŷ
struct PktStO : public PktBase 
{
	enum { MAX_STALL_NAME_LEN = 32 };

	unsigned long	m_dwCharID;
	char			m_StallName[MAX_STALL_NAME_LEN];
};

// ĳ���� ������ ���� ��� ��Ŷ
struct PktStRI : public PktBase
{
	enum PktStRICmd 
	{
		SC_REGISTER			= 0,	// ���� ������ ��ǰ ���
		SC_CANCEL			= 1,	// ���� ������ ��ǰ ��� ���

		SC_CAMP_REGISTER	= 2,	// ��� ��� ���� ��ǰ ���
		SC_CAMP_CANCEL		= 3,	// ��� ��� ���� ��ǰ ��� ���
		SC_CAMP_SELL		= 4		// ��� ��� ������ ��ǰ�� �ǸŵǾ� �����
	};

	enum PktStRIEErr
	{
		FAIL_NOT_ITEM	= 2,		// �߸��� �������Դϴ�.
		FAIL_NOT_SELL	= 3			// �ǸŰ� �Ұ����մϴ�.
	};

	unsigned long	m_dwCharID;		// ���� ���̵�
	unsigned long	m_dwShopID;		// ������ ���̵� (���� �������� ��� ������� �ʴ´�.)

	TakeType		m_TakeType;		// ������ �̵� ����ü
	unsigned long	m_dwPrice;		// ��� ����

	unsigned char	m_cCmd;			// ���

	char			m_BuyUser[CHAR_INFOST::MAX_NAME_LEN];
};

// ĳ���� ������ ���� ��Ŷ
struct PktStE : public PktBase
{
	enum PktStEErr
	{
		SUCCESS			= 0,
		SERVER_ERROR	= 1,
		FAIL_FULL_STALL	= 2,
		FAIL_ENTER		= 3,
		FAIL_LEAVE		= 4
	};

	unsigned long	m_dwCustomerID;
	unsigned long	m_dwOwnerID;
};

// ĳ���� ������ ������ ���� ��Ŷ (+ ������ ���� + ������ ����)
struct PktStIInfo : public PktBase
{
	enum 
	{ 
		MAX_CUSTOMER_NUM = 10 
	};

	unsigned long	m_dwOwnerID;						// ���� ID
	unsigned long	m_dwCustomerID[MAX_CUSTOMER_NUM];	// ���尴 ID

	unsigned long	m_dwTempSafe;						// �ӽ� �ݰ� (��� ��� ������)
	unsigned char	m_cTax;								// ���� (��� ��� ������)

	unsigned long	m_dwItemSize;						// ��ϵ� ������ ũ��
	unsigned char	m_cItemNum;							// ��ϵ� ������ ����
};

// ���� �ǳڿ� ���� ����
struct PktAP : public PktBase
{
	enum PktAPCmd
	{ 
		AP_RESURRECTION		= 1,
		AP_RECALL			= 2
	};

	unsigned long	m_dwAffectedID;
	unsigned long	m_dwCasterID;
	unsigned char	m_cCmd;
};

// ĳ���� ���� �ǳ� 
struct PktAPAck : public PktBase
{
	unsigned long	m_dwCasterID;
	char			m_szCasterName[CHAR_INFOST::MAX_NAME_LEN];
	unsigned char	m_cCmd;
};

// ĳ���� ���� ���� ���� ��Ŷ.
struct PktSaveEnemy : public PktBase
{
	unsigned long	m_dwCID;	
	unsigned long	m_dwAttackedCID;
	unsigned long	m_dwTick;
};

#pragma pack()

#endif