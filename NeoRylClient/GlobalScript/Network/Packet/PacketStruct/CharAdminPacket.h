#ifndef _CHAR_ADMIN_PACKET_H_
#define _CHAR_ADMIN_PACKET_H_

// CharAdminPacket.h
#include <winsock2.h>
#include <windows.h>
#include <Network/Packet/PacketBase.h>
#include <DB/DBdefine.h>
#include <Item/ItemStructure.h>

#pragma pack(1)

typedef struct PktAdmin* LPPktAdmin;
struct PktAdmin : public PktBase 
{
	enum {	MAX_NAME_LEN = 16 };
	enum CMD 
	{	
		NONE					= 0,	// ��� ���

		MOVE_ZONE_CHAR			= 1,	// ĳ���� �� �̵�
		MOVE_ZONE_PARTY			= 2,	// ��Ƽ �� �̵�
		MOVE_CHAR				= 3,	// ĳ���� �̵�
		MOVE_PARTY				= 4,	// ��Ƽ �̵�

		RESPAWN_CHAR			= 5,	// ĳ���� ������
		RESPAWN_PARTY			= 6,	// ��Ƽ ������
		KILL_CHAR				= 7,	// ĳ���� ų
		KILL_PARTY				= 8,	// ��Ƽ ų

		DUEL_INIT				= 9,	// ��� �ʱ�ȭ

		CREATE_ITEM				= 10,	// ������ ���
		CREATE_MON				= 11,	// ���� ����

		INFO_CHAR				= 12,	// ĳ���� ����

		GET_EXP					= 13,	// ����ġ ���
		GET_GOLD				= 14,	// �� ���
		LEVEL_UP				= 15,	// ������

		PEACE_MODE				= 16,	// ������ü ����
		WAR_MODE				= 17,	// ������ü Ż��

		GET_FAME				= 18,	// �� ���
		GET_MILEAGE				= 19,	// ���ϸ��� ���

		TRACE_CHAR				= 20,	// ĳ���� ����
		HIDE_CHAR				= 21,	// ĳ���� ����
		SHOW_CHAR				= 22,	// ĳ���� ���� ����
		INVINCIBLE_CHAR			= 23,	// ĳ���� ����
		NORMAL_CHAR				= 24,	// ĳ���� ���� ����
		LOGOUT_CHAR				= 25,	// ĳ���� ���� �α׾ƿ�

		APOCALYPSE				= 26,	// ���� ����ü ��� ų

		CREATE_EQUIP			= 27,	// ���� �ɼ��� �ٴ� ��� ���

		ELITE_BONUS				= 28,	// ����Ʈ ���ʽ� ����

		STATUE_INIT				= 29,	// ���� ���� �ʱ�ȭ
		RESPAWN_ALL				= 30,	// ��� ĳ���� ������

		CHANGE_CLASS			= 31,	// ���� �ϱ�
		LEARN_SKILL				= 32,	// �ش� ��ų ����
		ERASE_SKILL				= 33,	// �ش� ��ų �����
		LEVEL_DOWN				= 34,	// ���� �ٿ�

		GET_QUEST				= 35,	// ����Ʈ ����
		ADD_QUEST				= 36,	// �Ϸ� ����Ʈ�� �߰�
		DELETE_QUEST			= 37,	// ����/�Ϸ� ����Ʈ�� ����

										// BATTLEGROUND_SERVER ��� ���
		FINISH_GAME				= 38,	// �ش� ���� ��� ���� (���� ����)
		DRAW_GAME				= 39,	// �ش� ���� ��� ���� (���º�)
		IGNORE_GAME				= 40,	// �ش� ���� ��� ���� (���� ����)
		SET_MAX					= 41,	// �ش� ���� �ִ� �ο��� ����
		SET_SCORE				= 42,	// �ش� ���� ��ǥ ���� ����
		SET_LIMIT_TIME			= 43,	// �ش� ���� �ð� ���� ����
		SET_REST_TIME			= 44,	// �ش� ���� ���� �ð� ����

		SET_ITEM_TEMP_INVEN		= 45,	// �ӽ� �κ��丮�� ������ �ֱ�

		ADD_SPELL				= 46,	// �ش� ������ �ο�
		DELETE_SPELL			= 47,	// �ο��� �ش� ������ �ı�

		STOP					= 48,	// �ش� ĳ���� ����α�
		STOP_OFF				= 49,	// ����� �ش� ĳ���͸� Ǯ���ֱ�

		CHAT_BAN				= 50,	// ä�� ����
		CHAT_OK					= 51,	// ä�� ����

		NEO						= 52,	// �׿�
		SMITH					= 53,	// ���̽�

		RELATION				= 55,	// ���Ʊ� �ĺ�.

		GM_FORM					= 56,	// PC ���� GM ����.
		PC_FORM					= 57,	// GM ���� PC ��.

		REALM_POINT				= 58,	// ���� ���� ���� ����.
		DBAGENT_KILL			= 59,	// �߰� ���� ���� ����

		REQUEST_TRACE_POS		= 100	// DBAgent �� ���� �������� ������ ĳ������ ��ġ�� ��û�ϴ� ���
										// �ش� ���� ������ DBAgent ���� ������ ĳ������ ��ǥ�� �Ѱ��ٶ��� ���
	};

	enum eError
	{
		E_LOGOUT_CHAR			= 2,	// �α׾ƿ��� ĳ���� �Դϴ�.
	};
	
	unsigned short	m_usCmd;					// ��� ���

	char			m_stName[MAX_NAME_LEN];		// �̸�

	union
	{
		struct ZoneInfo
		{
			char		m_cZone;				// �� ��ȣ
			char		m_cChannel;				// ä�� ��ȣ
		} m_ZoneInfo;

		struct EliteBonusInfo
		{
			char		m_cNation;				// ����
			char		m_cLevel;				// ����
		} m_EliteBonusInfo;

		unsigned short	m_usProtoTypeID;		// Ÿ�� ���̵�
	};
	
	POS				m_Position;					// ��ġ
	
	unsigned long	m_dwAmount;					// ��
	unsigned long	m_dwAdminCID;				// Action �� ��ü�� �Ǵ� Admin ĳ������ CID
	unsigned long	m_dwAmount2;				// ��2
	unsigned long	m_dwAmount3;				// ��3
};


// ���� ���� ������ ������Ʈ�ϱ�.
struct PktAdminToolGetData : public PktBase
{
    enum GetDataType
    {
        // DB�߰� -> �����
        CHAR_BASIC_DATA		= 10,			// ĳ���� ����. ���� �������� ��. 

        // CHAR_INFOST	ĳ���� �⺻ ����
        // CHAR_POS		ĳ���� ��ġ ����
        // SKILL		ĳ���� ��ų ����
        // QUICK		ĳ���� �� ����
        // EQUIP		ĳ���� ���
        // INVEN		ĳ���� �κ�
        // EXTRA		ĳ���� ����
        // EXCHANGE		ĳ���� ��ȯ

        CHAR_EXTRA_DATA		= 11,			// ĳ���� �߰� ������. ���� �������� ��.

        // CHAR_INFOEX	ĳ���� �߰� ����
        // QUEST		����Ʈ
        // HISTORY		�����丮	
        // CONFIG		����
        // STORE_INFO	â�� ����

        CHAR_FRIEND_DATA	= 12,	// ĳ���� ģ�� ����
        CHAR_BAN_DATA		= 13,	// ĳ���� �ź� ����

        STORE_12_DATA		= 14,	// â�� 1, 2�� ������
        STORE_34_DATA		= 15,	// â�� 3, 4�� ������

        FINISH_GET_DATA		= 19,	// DB�߰迡�� ������� �����͸� ���� ������.

        // ����� -> DB�߰�
        GET_CHAR_DATA		= 20,	// ĳ���� ������ ��û ��Ŷ

        CHANGE_CHAR_NAME    = 30    // ĳ���� �̸� �ٲٱ�
    };

    enum GetError
    {
        UNKNOWN_ERROR				= 1,	// �� �� ���� ����
        INVALID_REQUEST				= 2,	// �߸��� ������ ��Ŷ

        GET_UIDCID_QUERY_FAILED		= 3,	// DB���� �̸����� UID, CID�� ������ �� �����ߴ�.
        INVALID_UID_CID_DATA		= 4,	// DB���� �̸����� ���� UID, CID�����Ͱ� �̻��ϴ�.

        GET_STORE_QUERY_FAILED		= 5,	// DB���� â�� �����͸� ������ �� �����ߴ�.
        GET_CHAR_DATA_QUERY_FAILED	= 6		// DB���� ĳ���� �����͸� ������ �� �����ߴ�.
    };

    char            m_szName[CHAR_INFOST::MAX_NAME_LEN];
    unsigned long   m_dwRequestKey;
	unsigned long   m_dwUID;
    unsigned long   m_dwCID;
    unsigned char   m_cType;    
	unsigned char	m_cOldServerGroupID;
};

struct PktAdminToolGetDataAck : public PktBase
{
    unsigned long   m_dwRequestKey;
    unsigned long   m_dwUID;
    unsigned long   m_dwCID;
    unsigned char   m_cType;
	unsigned char	m_cOldServerGroupID;
};


// ���� ������ ������ ������Ʈ�ϱ�. (�ڿ� �����͸� ����)
struct PktAdminToolSetData : public PktBase
{
    enum SetDataType
    {
        CHAR_BASIC_DATA		= 10,			// ĳ���� ����. ���� �������� ��. 

        // CHAR_INFOST	ĳ���� �⺻ ����
        // CHAR_POS		ĳ���� ��ġ ����
        // SKILL		ĳ���� ��ų ����
        // QUICK		ĳ���� �� ����
        // EQUIP		ĳ���� ���
        // INVEN		ĳ���� �κ�
        // EXTRA		ĳ���� ����
        // EXCHANGE		ĳ���� ��ȯ

        CHAR_EXTRA_DATA		= 11,			// ĳ���� �߰� ������. ���� �������� ��.

        // CHAR_INFOEX	ĳ���� �߰� ����
        // QUEST		����Ʈ
        // HISTORY		�����丮	
        // CONFIG		����
        // STORE_INFO	â�� ����

        CHAR_FRIEND_DATA	= 12,	// ĳ���� ģ�� ����
        CHAR_BAN_DATA		= 13,	// ĳ���� �ź� ����

        STORE_12_DATA		= 14,	// â�� 1, 2�� ������
        STORE_34_DATA		= 15,	// â�� 3, 4�� ������

        OPEN_UPDATE_DATA	= 20,	// ������ ���� DB�� ������Ʈ�Ϸ� �õ��Ѵ�.
        FINISH_UPDATE_DATA	= 21,	// �����͸� DB�� ������Ʈ�ϰ� ������ �ݴ´�.		

        RELOAD_ADMINLIST    = 30
    };

    enum SetError
    {
        UNKNOWN_ERROR				= 1,	// �� �� ���� ����.
        INVALID_UID_CID_ERROR		= 2,	// �� �� ���� UID/CID
        LOGIN_ANOTHER_GAMESERVER	= 3,	// �ٸ� ���� ������ �α��� �� �־ ������Ʈ �Ұ�.

        SESSION_OPEN_FAILED					= 4,	// ���� ���� ����
        ALREADY_LOGIN_USER_SESSION_OPEN		= 5,	// �̹� �α��� �Ǿ� �ִ� ���� ������ ������ ����
        ANOTHER_CHAR_LOAD_FAILED			= 6,	// ĳ���� ������ DB���� �о���� �� ����
        SETTING_DATA_FAILED					= 7,	// ������ ���� ���� (������ ���̵��� ���� ����)
        SESSION_START_FAILED				= 8		// ���� ���� ����.
    };

    unsigned __int64    m_dwSerial;
    unsigned long       m_dwRequestKey;
    unsigned long       m_dwUID;
    unsigned long       m_dwCID;
    unsigned char       m_cType;
	unsigned char		m_cOldServerGroupID;
};

struct PktAdminToolSetDataAck : public PktBase
{
    unsigned long       m_dwRequestKey;
    unsigned char       m_cType;
};


struct PktNewAdminToolSetDataAck : public PktBase
{
    unsigned long   m_dwRequestKey;
    unsigned long   m_dwUID;
    unsigned long   m_dwCID;
    unsigned char   m_cType;
};


struct GiveItemInfo
{
    enum GiveItemType
    {
        MIN_DAMAGE      = 0,
        MAX_DAMAGE      = 1,
        ARMOR           = 2,
        HIT_RATE        = 3,
        EVADE           = 4,
        MAX_HP          = 5,
        HP_REGEN        = 6,
        MAX_MP          = 7,
        MP_REGEN        = 8,
        CRITICAL        = 9,
        BLOCK           = 10,
        SPEED           = 11,
        MAGIC_POWER     = 12,
        MAGIC_RESIST    = 13,
        MAX_GIVEITEM_ATTRIBUTE = 14
    };

    unsigned long       m_dwCreateID;
    unsigned long       m_dwUID;
    unsigned long       m_dwCID;
    unsigned short      m_usPrototypeID;
    unsigned char       m_cAmount;
    unsigned char       m_cAttribute[MAX_GIVEITEM_ATTRIBUTE];
    unsigned __int64    m_dwItemUID;
};

struct UpdateItemUID
{
    unsigned __int64    m_dwItemUID;
    unsigned long       m_dwCreateID;
};

struct PktGiveItemToTempInven : public PktBase
{
    enum Const
    {
        MAX_GIVE_ITEM = 10
    };

    GiveItemInfo        m_GiveItemInfo[MAX_GIVE_ITEM];
    unsigned char       m_cGiveItemNum;
};

struct PktCharNameChange : public PktBase
{
    enum Error
    {
        ERR_SERVER                  = 1,    // ���� ����
        ERR_CANNOT_FIND_CHARACTER   = 2,    // ĳ���� ã�� �� ����(�α��ε� ĳ���Ͱ� �ƴ�)
        ERR_NAMECHANGE_ITEM         = 3,    // �̸� ���� 
        ERR_INVALID_NAME            = 4,    // ���� �� ���� �̸�(Ư�����ڵ��� �� �̻��� �̸�)
        ERR_CANNOT_USE_NAME         = 5,    // ����� �� ���� �̸�
        ERR_ALREADY_USE_NAME        = 6     // �̹� ����ϰ� �ִ� �̸�
       
    };

    char                m_szCharName[CHAR_INFOST::MAX_NAME_LEN];    // �ٲ� �̸�
    unsigned long       m_dwUID;                                    // �ڱ� ĳ���� UID
    unsigned long       m_dwCID;                                    // �ڱ� ĳ���� CID    
    unsigned char       m_cNameChangeCount;                         // ���ŵ� �̸� ���� ī��Ʈ ����.
	Item::ItemPos		m_ItemPos;		// ����㰡�� (�߰��۾���)
};

struct PktAdminCommandLog : public PktBase 
{
	enum Const
	{
		MAX_COMMAND_SIZE	= 128
	};

	unsigned long		m_dwAdminCID;					// �ش� GM UID
	unsigned long		m_dwCID;						// �ش� ĳ���� CID.
	char				m_szCommand[MAX_COMMAND_SIZE];	// �ش� ��ɾ� �ؽ�Ʈ.
};

#pragma pack()

#endif