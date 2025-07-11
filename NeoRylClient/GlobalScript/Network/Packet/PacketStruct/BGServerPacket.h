#ifndef _BATTLEGROUND_SERVER_PACKET_H_
#define _BATTLEGROUND_SERVER_PACKET_H_

#include <Network/Packet/PacketBase.h>
#include <Creature/Character/CharacterClass.h>

#pragma pack(1)


// �� ����Ʈ (���� ����) + �� ����
typedef struct PktBGServerMapList* LPPktBGServerMapList;
struct PktBGServerMapList : public PktBase
{
	enum Const
	{
		MAX_MAP_NUM = 10
	};

	bool			m_bAll;				// true �̸� ��� �� ����Ʈ, false �̸� �ڽ��� �� ����
	unsigned char	m_cMapInfoNodeNum;	// ��� ��
};


// �� ����
struct BGServerMapInfoNode
{
	enum MapType
	{
		FRAG			= 0,		// ���ھ� ���� ���� ��
		STATUE			= 1,		// ���� ���� ���� ��

		MAX_MAP_TYPE
	};

	// ��Ʋ �׶��� �ϳ��� ���� ���� ������ �ϴ� ����
	bool			m_bPlaying;								// ���� ������ ����
    unsigned char	m_cMapType;								// ���� ���
	unsigned char	m_cMaxCharNumOfNation;					// �� ���� �ִ� �ο�
	unsigned char	m_cRemainMin;							// ���� �ð� (���ð� Ȥ�� ���½ð�) (��)
	unsigned short	m_wTargetScore;							// ��ǥ ����
	unsigned char	m_cCurrentCharNum[CClass::MAX_RACE];	// ���� �ο�
	unsigned short	m_wScore[CClass::MAX_RACE];				// ����
};


// �� ��� ����Ʈ (���� ����) + �� ��� ����
typedef struct PktBGServerResultList* LPPktBGServerResultList;
struct PktBGServerResultList : public PktBase
{
	enum Const
	{
		MAX_MAP_NUM = 10
	};

	bool			m_bAll;					// true �̸� ��� �� ����Ʈ, false �̸� �ڽ��� �� ����
	unsigned char	m_cResultInfoNodeNum;	// ��� ��
};


// �� ��� ����
struct BGServerResultInfoNode
{
	unsigned char	m_cWinRace;						// ���� (CClass::MAX_RACE �̸� ���º�)
	unsigned char	m_cPlayMin;						// ���� �÷����� �ð�
	unsigned char	m_cKill;						// ������ ���� �� 
	unsigned char	m_cKilled;						// ���� ��
	unsigned short	m_wAward;						// �������� ���� ���ϸ���
	unsigned short	m_wScore[CClass::MAX_RACE];	// ����
};


// BATTLEGROUND_SERVER �� �̵�
typedef struct PktBGServerMoveZone* LPPktBGServerMoveZone;
struct PktBGServerMoveZone : public PktBase
{
	enum MoveType
	{
		TYPE_PLAYER		= 0,	// ���� ������
		TYPE_SPECTATOR	= 1,	// ���� ������

		MAX_TYPE_NUM
	};

	enum Error
	{
		FAIL_FULL_MAP	= 2,	// �� ���� �ʰ�
		FAIL_FIX_RATE	= 3		// �� ���� ������ ���߱� ���� ���� ����
	};

	unsigned short	m_wMapNumber;	// �ش� �� ��ȣ (0�� ����)
	unsigned char	m_cMoveType;	// �濡 ���� Ÿ��
	unsigned char	m_cZone;		// �ش� ���� �� ��ȣ (Ack)
};


// ���� ĳ���� ����
typedef struct PktBGServerCharSlot* LPPktBGServerCharSlot;
struct PktBGServerCharSlot : public PktBase
{
	unsigned long	m_dwCID;		// ĳ���� ���̵� (Client <-> Game)
	unsigned long	m_dwUID;		// ���� ���̵�   (Game <-> DBAgent)
	unsigned char	m_cGroup;		// ���� �׷�
};

// ���� ĳ���� ���� Ack
typedef struct PktBGServerCharSlotAck* LPPktBGServerCharSlotAck;
struct PktBGServerCharSlotAck : public PktBase
{
    enum PktCharSlotAckErr
    {
        FAIL_NOT_USER	= 2,		// DB���� ���� ���� ��� ����
        FAIL_NOT_CHAR	= 3			// DB���� ĳ���� ���� ��� ����
    };

	unsigned long	m_dwCID;		// ĳ���� ���̵�
	unsigned char	m_cGroup;		// ���� �׷�

    unsigned long	m_dwSlotCID[USER_INFO::MAX_CHAR_NUM];								// ���� ĳ���� ID
	char            m_szSlotName[USER_INFO::MAX_CHAR_NUM][CHAR_INFOST::MAX_NAME_LEN];	// ���� ĳ���� �̸�
};


// ȯ���� �̿�
typedef struct PktBGServerMileageChange* LPPktBGServerMileageChange;
struct PktBGServerMileageChange : public PktBase
{
	enum PktBGSMCCmd
	{
		MC_REQUEST		= 0,	// ���� ��û (������ ������...)
		MC_INFO			= 1,	// ���� Ack
		MC_CHANGE		= 2,	// ��ȯ ��û (BG�� ����޴� -> ������ ��)
		MC_RESULT		= 3		// ��ȯ Ack
	};

    enum PktErr
    {
        SERVER_ERROR        = 1,
        NOT_USED_STORE      = 2,    // â�� ��������� ���� ��� - MC_RESULT (�޽��� ó�� �ʿ�)
        NOT_ENOUGH_MEDAL    = 3,    // ���� �޴� ���� - MC_RESULT (�޽��� ó�� �ʿ�)
        MONEY_OVERFLOW      = 4,    // �� �����÷ο� �߻�(�Ա� �Ұ�) - MC_RESULT ( �޽��� ó�� �ʿ�)
        DATA_REQUESTING     = 5,    // ���� ������ ��û���� - MC_RESULT (�޽��� ó�� �ʿ� ����)
        NONE_CHARACTER      = 6     // ĳ���Ͱ� ���� ��� - MC_INFO (�޽��� ó�� �ʿ�)
    };

	unsigned long	m_dwCID;		// ĳ���� ID
	unsigned char	m_cGroup;		// ���� �׷�

	unsigned char	m_cCmd;			// ���

	unsigned long	m_dwGold;		// ��
	unsigned long	m_dwMileage;	// ���� �޴�
};

#pragma pack()


#endif