#ifndef _CASTLE_PACKET_H_
#define _CASTLE_PACKET_H_

#include <algorithm>

#include <Network/Packet/PacketBase.h>
#include <Network/Packet/PacketStruct/CharMovePacket.h>

#include <DB/DBDefine.h>
#include <Item/ItemStructure.h>
#include <Castle/CastleConstants.h>
#include <Creature/Siege/SiegeConstants.h>
#include <Community/Guild/GuildConstants.h>

#pragma pack(1)

using namespace Castle;
using namespace Siege;

// �� ���� ����
struct CastleRight
{
	enum Index
	{
		CONTROL_CASTLE_EMBLEM	= 0,		// �� ��¡�� ����
		CONTROL_CASTLE_GATE		= 1,		// ���� ����
		USE_CASTLE_GATE			= 2,		// ���� �̿�
		CONTROL_CASTLE_ARMS		= 3,		// ���� ����
		USE_CASTLE_ARMS			= 4,		// ���� �̿�

		MAX_RIGHT_NUM			= 10
	};

	unsigned char	m_aryCastleRight[MAX_RIGHT_NUM];

	CastleRight()
	{
		std::fill_n(&m_aryCastleRight[0], int(MAX_RIGHT_NUM), Guild::COMMON);
	};

	bool Check(unsigned char cRightType, unsigned char cTitle)
	{
		return (m_aryCastleRight[cRightType] >= cTitle);
	}
};

// ��� ��� ���� ����
struct CampRight
{
	enum Index
	{
		CAMP_CONTROL		= 0,	// ��� ��� ���� ����

		MAX_RIGHT_NUM		= 10
	};

	unsigned char	m_aryCampRight[MAX_RIGHT_NUM];

	CampRight()
	{
		std::fill_n(&m_aryCampRight[0], int(MAX_RIGHT_NUM), Guild::MASTER);
	}

	bool Check(unsigned char cRightType, unsigned char cTitle)
	{
        return (m_aryCampRight[cRightType] >= cTitle);
	}
};


// �� ���� DB ������
struct CastleInfoDB
{
	unsigned long	m_dwCastleID;							// Castle ID
	unsigned char	m_cNation;								// ���� ����
	unsigned char	m_cZone;								// �� ��ȣ
	unsigned char	m_cChannel;								// ä�� ��ȣ
	unsigned char	m_cNameID;								// �� �̸� ID
	unsigned char	m_cSiegeCount;							// ���� Ƚ�� (�ӽ� ���� -> ���� �������� �ű涧 ���)
	unsigned char	m_cInvincibleCount;						// ���� Ƚ��
	unsigned short	m_wTotalSiegeCount;						// ���� ���� Ƚ��
	unsigned long	m_dwTotalTaxMoney;						// ���� ���� ȸ����
	char			m_szRight[CastleRight::MAX_RIGHT_NUM];	// �������� �� ���� ����
	POS				m_InPos;								// �޹� ���� �������� �̵��� ��ġ
	POS				m_OutPos;								// �޹� ���� �������� �̵��� ��ġ
	unsigned short	m_wItemID;								// ���׷��̵忡 ����� ���� ������ ���̵�
	unsigned char	m_cItemNum;								// ���׷��̵忡 ����� ���� ������ ����
};

// ���� ���� �� ���� DB ������
struct CastleTaxInfoDB
{
	unsigned long	m_dwCastleID;							// Castle ID
	unsigned char	m_cTaxType;								// ���� (����/�Ǹ�, ����, ���, ��� Gold, ��� ����)
	unsigned char	m_cTax;									// ����
	unsigned long	m_dwTempTaxMoney;						// �ӽ� ����
	unsigned long	m_dwTaxMoney;							// ����
	unsigned char	m_cTaxChangable;						// ���� ���� ���� ����
};

// ���� ���� ���� ������
struct CastleMineralInfoDB
{
	unsigned long	m_dwCastleID;							// Castle ID
	unsigned short	m_wMineralID;							// ���� Item ID
	unsigned short	m_wAmount;								// ��
	unsigned char	m_cFlag;								// ���� or �ӽ� üũ �÷���
};

// �� ���� ������Ʈ DB ������
struct CastleObjectInfoDB
{
	unsigned long	m_dwCID;
	unsigned long	m_dwCastleID;							// CastleID : �� ��¡��, ����, �޹�, ����, �ٰŸ�/���Ÿ� ��������
	unsigned long	m_dwOwnerID;							// �ٰŸ�/���Ÿ� ���������� ������ CID
	unsigned long	m_dwHP;									// HP
	unsigned short	m_wObjectType;							// �� ������Ʈ Ÿ�� ( �� ��¡��, ����, �޹�, ����, �ٰŸ�/���Ÿ� ��������)
	float			m_fDefaultDir;							// �� ������Ʈ�� �⺻ ����
	unsigned char	m_cState;								// ���� ������Ʈ�� ����
	unsigned char	m_cSubState;							// ���� ������Ʈ�� ���� ����		(Use EMBLEM & GATE)
	unsigned char	m_cUpgradeStep;							// ���� ������Ʈ�� ���׷��̵� �ܰ�	(Except GUARD)
	unsigned char	m_cUpgradeType;							// �� ��¡���� ���׷��̵� Ÿ��		(Only EMBLEM)
	TIME			m_RemainUpTime;							// ���� ���� �ð� ���� ����
	TIME			m_LastUseTime;							// ������ ��� �ð�
	POS				m_Pos;									// ��ġ
};


// ��� ������Ʈ DB ������
struct CampInfoDB
{
	unsigned long	m_dwCampID;								// CampID
	unsigned long	m_dwGID;								// ���� Guild ID
	unsigned long	m_dwHP;									// HP
	unsigned char	m_cZone;								// ��
	unsigned char	m_cChannel;								// ä�� ����
	unsigned short	m_wObjectType;							// ��� ������Ʈ Ÿ�� ( ���, ä����, ����, ������� )
	unsigned char	m_cState;								// ��� ��� ����
	unsigned char	m_cSubState;							// ��� ��� ���� ���� ( ä������ On/Off/Ready ���� )
	unsigned char	m_cUpgradeStep;							// ���׷��̵� �ܰ�
	TIME			m_TimeValue;							// ����ñ��� ���� �ð�
	TIME			m_LastUseTime;							// ���������� ����� �ð�
	char			m_szRight[CampRight::MAX_RIGHT_NUM];	// �������� ��� ��� ���� ����
	POS				m_Pos;									// ��ġ
	unsigned char	m_cMaterial;							// ������ ���� ����
	unsigned char	m_cSiegeCount;							// ������ ������ Ƚ��
};

// ä������ ���� ���� DB ������
struct CampMineralInfoDB
{
	unsigned long	m_dwCampID;								// CampID
	unsigned short	m_wMineralID;							// ���� Item ID
	unsigned short	m_wAmount;								// ��
	unsigned char	m_cFlag;								// ���� or �ӽ� üũ �÷���
};

// ���� ���� DB ������
struct FertilityInfoDB
{
	unsigned long	m_dwVeinColor;							// ���� ����
	unsigned long	m_dwFertility;							// ���� ����
	unsigned char	m_cZone;								// �� ��ȣ
	unsigned char	m_cChannel;								// ä�� ��ȣ
};

// ���� �ð� DB ������
struct GameTimeDB
{
	unsigned char	m_cGuildTime;							// ��� ���� �ð� ����
	unsigned char	m_cRealmTime;							// ���� ���� �ð� ����
	unsigned char	m_cSiegeTime;							// ������ �ð� ����
	unsigned char	m_cMiningTime;							// �ӽ� ä���� �ð� ����
	unsigned char	m_cEnterTime;							// �׼��� �ű��� �������� �÷���
	unsigned char	m_cEventTime;							// �̺�Ʈ Ÿ��
};

// ��� ��� ���� ���� DB ������
struct CampShopInfoDB
{
	enum
	{
		MAX_CONTAINER_SIZE		= 7995						// 	m_dwTotalSize + m_cItemNum + m_aryItemContainer = 8000
	};

	unsigned long	m_dwCampID;								// ��� ��� ID
	unsigned long	m_dwTotalSize;							// ������ ������ (������ ���� + ����)
	unsigned char	m_cItemNum;								// ������ ����
	unsigned char	m_aryItemContainer[MAX_CONTAINER_SIZE];	// �Ǹ����� ������ ���
	unsigned long	m_dwTempSafe;							// �ӽ� �ݰ�
	unsigned char	m_cTax;									// ����
};

// ���� ���� ���� DB ������
struct WorldWeaponInfoDB
{
	unsigned char	m_cZone;								// �� ��ȣ
	unsigned char	m_cChannel;								// ä�� ��ȣ
	unsigned char	m_cKarRemainSiegeTime;					// ī���׶�Ʈ ���� ������ �μ����� �� �������� ���� �ð�
	unsigned char	m_cMerRemainSiegeTime;					// �޸�ī��� ���� ������ �μ����� �� �������� ���� �ð�
};

// ���� ���� ���� ���� DB ������
struct StatueInfoDB
{
	unsigned long	m_dwIndex;								// ���� �ε��� (���� ��ġ�� ���ڸ�ŭ ����)
	unsigned char	m_cChannel;								// ä��
	unsigned short	m_wKID;									// ���� ������ ���� (KID �� ���� �Ǻ�)
	unsigned long	m_dwLostHP;								// ������ ���� HP ����
};


// �� ���� (DBAgentServer -> GameServer)
// ���� ���� : + CastleInfoDB
//			   + CastleTaxInfo * Castle::MAX_TAX_TYPE
//			   + CastleMineralInfo + CastleMineral * ( m_cAccumulatedNum + m_cTemporaryNum )
//			   + CastleObjectInfo * m_cCastleObjectNum
struct PktCastleInfo : public PktBase
{
	bool			m_bStartFlag;							// ó�� �����ΰ�?
	unsigned char	m_cCastleObjectNum;						// �� ���� ������Ʈ ��
	unsigned short	m_wSize;								// �� ���� ũ��
};

struct CastleTaxInfo
{
	unsigned char	m_cTaxType;								// ���� (����/�Ǹ�, ����, ���, ��� Gold, ��� ����)
	unsigned char	m_cTax;									// ����
	unsigned long	m_dwTempTaxMoney;						// �ӽ� ����
	unsigned long	m_dwTaxMoney;							// ����
	unsigned char	m_cTaxChangable;						// ���� ���� ���� ����
};

struct CastleMineral
{
	unsigned short	m_wMineralID;							// ���� Item ID
	unsigned short	m_wAmount;								// ���� ����
};

struct CastleMineralInfo
{
	unsigned char	m_cAccumulatedNum;						// ���� ������ ������ ����
	unsigned char	m_cTemporaryNum;						// �ӽ� ������ ������ ����
	unsigned short	m_wSize;								// ���� ���� ũ��
};

struct CastleObjectInfo
{
	unsigned long	m_dwCID;								// CID
	unsigned long	m_dwCastleID;							// CastleID : �� ��¡��, ����, �޹�, ����, �ٰŸ�/���Ÿ� ��������
	unsigned long	m_dwOwnerID;							// �ٰŸ�/���Ÿ� ���� ������ ������ CID
	unsigned char	m_cNation;								// ����
	unsigned long	m_dwHP;									// HP
	unsigned short	m_wObjectType;							// �� ������Ʈ Ÿ�� ( �� ��¡��, ����, �޹�, ����, �ٰŸ�/���Ÿ� ��������)
	float			m_fDefaultDir;							// �� ������Ʈ�� �⺻ ����
	unsigned char	m_cState;								// ���� ������Ʈ�� ����
	unsigned char	m_cSubState;							// ���� ������Ʈ�� ���� ����		(Use EMBLEM & GATE)
	unsigned char	m_cUpgradeStep;							// ���� ������Ʈ�� ���׷��̵� �ܰ�	(Except GUARD)
	unsigned char	m_cUpgradeType;							// �� ��¡���� ���׷��̵� Ÿ��		(Only EMBLEM)
	POS				m_Pos;									// ��ġ
};

// ���� �̵� ���� ��Ŷ
// DBAgentServer -> GameServer -> Client	: �ӽ� ���� -> ���� �������� �̵�
// GameServer -> DBAgentServer -> DB		: �ӽ� ���� ���� ������Ʈ
typedef struct PktCastleTaxMove* LPPktCastleTaxMove;
struct PktCastleTaxMove : public PktBase
{
	unsigned long	m_dwCastleID;								// �� ID
	unsigned long	m_dwMoveTempMoney[ Castle::MAX_TAX_TYPE ];	// ���� �������� �̵��� �ӽ� ���� �׼�, ������Ʈ �� �ӽ� ���� ����
	unsigned long	m_dwResultTaxMoney[ Castle::MAX_TAX_TYPE ];	// �ӽ� ������ �̵��� ���� ���� ���� �׼�
};

// ���� ���� ���� ���� ��Ŷ
// ���� ����  : + CastleMineral * m_cNum
typedef struct PktCastleMineralInfo* LPPktCastleMineralInfo;
struct PktCastleMineralInfo : public PktBase
{
	unsigned long	m_dwCastleID;							// �� ID
	unsigned char	m_cMineralType;							// ���� Ÿ�� (����, �ӽ�)
	unsigned char	m_cNum;									// ���� ��
	unsigned short	m_wSize;								// ���� ���� ũ��
};

// ���� ���� Ƚ�� ���� ��Ŷ
typedef struct PktCastleSiegeCount* LPPktCastleSiegeCount;
struct PktCastleSiegeCount : public PktBase
{
	unsigned long	m_dwCastleID;							// �� ID
	unsigned char	m_cSiegeCount;							// ���� Ƚ�� (�ӽ� ���� -> ���� ����)
	unsigned char	m_cInvincibleCount;						// ���� ���� ���� Ƚ��
	unsigned short	m_wTotalSiegeCount;						// ���� ���� Ƚ��
};


// ���� ���� ���� ���� ����ü
struct StatueInfo
{
	unsigned long	m_dwIndex;
	unsigned short	m_wKID;
	unsigned long	m_dwLostHP;
};

// ���� ���� ���� ���÷��� ���� ����ü
struct StatueDisplayInfo
{
	unsigned long	m_dwIndex;
	unsigned long	m_dwKID;
	unsigned char	m_cNation;
	bool			m_bSummoning;
	bool			m_bAttacked;
	POS				m_Pos;
};

// ���� ���� ���� ���� ��Ŷ (DBAgentServer <-> GameServer / GameServer -> Client)
// ���� ���� :
//		wSubCmd : CMD_INFO				+ StatueInfo * ���� ��
//				: CMD_UPDATE_LOST_HP	+ StatueInfo
//				: CMD_UPDATE_KID		+ StatueInfo
//				: CMD_DISPLAY_INFO		+ StatueDisplayInfo * ���� ��	(Ŭ���̾�Ʈ ó�� �α��ν�)
//										+ StatueDisplayInfo				(���� ���� �����)

typedef struct PktStatueCmd* LPPktStatueCmd;
struct PktStatueCmd : public PktBase
{
	enum SubCmd
	{
		CMD_INFO			= 0,	// DBAgentServer	->	GameServer
		CMD_UPDATE_LOST_HP	= 1,	// GameServer		->	DBAgentServer
		CMD_UPDATE_KID		= 2,	// GameServer		->	DBAgentServer
		CMD_DISPLAY_INFO	= 3		// GameServer		->	Client
	};

	enum Const
	{
		MAX_STATUE_NUM		= 16
	};

	unsigned short	m_wSubCmd;
	unsigned short	m_wSize;		// INFO : ���� ���� ������  /  LOST_HP : sizeof(StatueInfo)	/  KID : sizeof(StatueInfo)	/	DISPLAY_INFO : ���� ���� ������
	unsigned long	m_dwValue;		// INFO : ���� ��			/  LOST_HP : Channel			/  KID : Channel			/	DISPLAY_INFO : ���� ��
};


// ��� ��� ���� (DBAgentServer -> GameServer)
// ���� ���� : ä����   - CampInfoDB + MiningCampInfo + CampMineralInfo * (m_cAccumulatedNum + m_cTemporaryNum)
//             ����     - CampInfoDB + CampShopInfo + ������ ����
//             ������� -
struct PktCampInfo : public PktBase
{
	bool			m_bStartFlag;							// ó�� �����ΰ�?
	unsigned long	m_dwCID;								// ����� CID
	unsigned short	m_wSize;								// ��� ���� ũ��
};

struct MiningCampInfo
{
	unsigned char	m_cAccumulatedNum;						// ���� ������ ������ ����
	unsigned char	m_cTemporaryNum;						// �ӽ� ������ ������ ����
	unsigned short	m_wSize;								// ���� ���� ũ��
};

struct CampMineralInfo
{
	unsigned short	m_wMineralID;							// ���� Item ID
	unsigned short	m_wAmount;								// ���� ����
};

struct CampShopInfo
{
	unsigned long	m_dwTempSafe;							// �ӽ� �ݰ�
	unsigned char	m_cTax;									// ����
	unsigned long	m_dwBufferSize;							// ������ ���� ������
	unsigned char	m_cItemNum;								// ������ ����
};

// ���� ���� ���� ���� (DBAgentServer -> GameServer)
struct PktWorldWeaponInfo : public PktBase
{
	unsigned char	m_cKarRemainSiegeTime;					// ī���׶�Ʈ ������ ���� ������ �μ����� �������� ���� �ð�
	unsigned char	m_cMerRemainSiegeTime;					// �޸�ī��� ������ ���� ������ �μ����� �������� ���� �ð�
};

// ���� ����
// ���� ���� (DBAgentServer -> GameServer) : + (dwVeinColor + dwFertility) * m_wNum
// ���� ���� (GameServer -> DBAgentServer) : + ZoneChannelInfo + (dwVeinColor + dwFertility) * m_wNum
struct PktFertilityInfo : public PktBase
{
	unsigned short	m_wNum;									// ���� ����
	unsigned short	m_wSize;								// ���� ���� ũ��
};

// ��� ��� ���� ���� (+ ������ ����)
struct PktCampShopInfo : public PktBase
{
	unsigned long	m_dwCampID;								// ��� ��� ID
	CampShopInfo	m_CampShopInfo;							// ��� ��� ���� ����
};

struct ZoneChannelInfo
{
	unsigned char	m_cZone;
	unsigned char	m_cChannel;
};

// ���� ������ ä���� ������ Ȯ�� ��Ŷ
// ���� ����  : + CampMineralInfo * m_cNum
typedef struct PktMiningCampMineralInfo* LPPktMiningCampMineralInfo;
struct PktMiningCampMineralInfo : public PktBase
{
	unsigned long	m_dwCampID;								// ä���� ��� ID
	unsigned long	m_dwRemainTime;							// ���� �ð� (����: ��)
	unsigned char	m_cState;								// ���� ����
	unsigned char	m_cMineralType;							// ���� Ÿ�� (����, �ӽ�)
	unsigned char	m_cNum;									// ���� ��
	unsigned short	m_wSize;								// ���� ���� ũ��
};

// ä���� ���μ��� ���� ��Ŷ
typedef struct PktProcessMining* LPPktProcessMining;
struct PktProcessMining : public PktBase
{
	enum
	{
		TEMPORARY_PROCESS	= 1,
		ACCUMULATED_PROCESS	= 2
	};

	unsigned long	m_dwProcessType;						// �ӽ� ä�� ���μ�������, ���� ä�� ���μ�������
};


// �� ���� (GameServer -> Client)
// ���� ���� ������ �ʿ��Ҷ� �������� �����ش�.
typedef struct PktCreateCastle*	LPPktCreateCastle;
struct PktCreateCastle : public PktBase
{
	unsigned long	m_dwCastleID;							// CastleID
	unsigned char	m_cNation;								// ���� ����
	unsigned char	m_cZone;								// �� ��ȣ
	unsigned char	m_cNameID;								// �� �̸� ID
	unsigned char	m_cUpgradeStep;							// �� ��¡�� ���׷��̵� �ܰ�
	unsigned char	m_cInvincibleCount;						// ���� Ƚ��
    unsigned short	m_wTotalSiegeCount;						// ���� ���� Ƚ��
	unsigned long	m_dwTotalTaxMoney;						// ���� ���� ȸ����
	float			m_fPosX ;								// ���� ��ġ
	float			m_fPosY ;
	float			m_fPosZ ;
	unsigned short	m_wItemID;								// ���׷��̵忡 ����� ���� ������ ID
	unsigned char	m_cItemNum[Castle::EMBLEM_UPGRADE_JEWEL_POS_NUM];	// ���׷��̵忡 ����� ���� ������ ����
	CastleRight		m_CastleRight;							// �������� �� ���� ����
	CastleTaxInfo	m_CastleTax[Castle::MAX_TAX_TYPE];		// �� ����/���� ����
};


// ��� ��� ���� (GameServer -> Client, GameServer -> DBAgentServer)
typedef struct PktCreateCamp*	LPPktCreateCamp;
struct PktCreateCamp : public PktBase
{
	enum Error
	{
		ERROR_EXIST_ANOTHER_WEAPON	= 2,						// �̹� ���� ������ ������ ���
	};

	unsigned long	m_dwActorCID;							// ��� ������ ��û�� ĳ������ CID
	unsigned long	m_dwCID;								// CID
	unsigned long	m_dwCampID;								// CampID
	unsigned long	m_dwGID;								// ���� Guild ID
	unsigned long	m_dwHP;									// HP
	unsigned short	m_wObjectType;							// ��� ��� Ÿ��
	unsigned char	m_cZone;								// �� ��ȣ (GameServer -> DBAgentServer ���� ���)
	unsigned char	m_cChannel;								// ä�� ��ȣ (GameServer -> DBAgentServer ���� ���)
	unsigned char	m_cState;								// ��� ��� ����
	unsigned char	m_cSubState;							// ��� ��� ���� ����
	unsigned char	m_cUpgradeStep;							// ���׷��̵� �ܰ�
	unsigned char	m_cNation;								// ķ�� ���� (��� ���� : Ŭ���̾�Ʈ�� ��� ������ �������� ���ؼ�)
	CampRight		m_CampRight;							// ��� ��� ���� ����
	POS				m_Pos;									// ��ġ
	unsigned char	m_cMaterial;							// ������ ���� ����
};


// ���� ���� ���� (GameServer -> Client, GameServer -> DBAgentServer)
typedef struct PktCreateSiegeArms*	LPPktCreateSiegeArms;
struct PktCreateSiegeArms : public PktBase
{
	unsigned long	m_dwCID;								// CID (���� ���� ID)
	unsigned long	m_dwOwnerID;							// ���� ���� ���� CID
	unsigned short	m_wObjectType;							// ���� ���� Ÿ��
	unsigned char	m_cNation;								// ���� ���� �������� ����
	unsigned long	m_dwHP;									// HP
	unsigned char	m_cZone;								// �� ��ȣ (GameServer -> DBAgentServer ���� ���)
	unsigned char	m_cChannel;								// ä�� ��ȣ (GameServer -> DBAgentServer ���� ���)
	unsigned char	m_cState;								// ���� ���� ����
	unsigned char	m_cUpgradeStep;							// ���� ���� ���׷��̵� �ܰ�
	POS				m_Pos;									// ��ġ
};


// ��� ��� ���� ��� ��Ŷ ( Client <-> GameServer <-> DBAgentServer )
typedef struct PktCampCmd*	LPPktCampCmd;
struct PktCampCmd : public PktBase
{
	enum SubCmd
	{
		CAMP_NO_CMD					= 0,

		// ��� ��� �⺻ ���
		CAMP_BUILD					= 1,	// ��� ��� ���� (��� ����, ��ŸƮŶ ���������� ���)
		CAMP_BUILD_COMPLETE			= 2,	// ��� ��� ���� �Ϸ�
		CAMP_CANCEL					= 3,	// ��� ��� ���� ����
		CAMP_CANCEL_COMPLETE		= 4,	// ��� ��� ���� ���� �Ϸ�
		CAMP_UPGRADE				= 5,	// ��� ��� ���׷��̵�
		CAMP_UPGRADE_COMPLETE		= 6,	// ��� ��� ���׷��̵� �Ϸ�
		CAMP_REPAIR					= 7,	// ��� ��� ���� 
		CAMP_REPAIR_COMPLETE		= 8,	// ���� ���� �Ϸ�
		CAMP_TO_STARTKIT			= 9,	// ��ŸƮŶ����
		CAMP_TO_STARTKIT_COMPLETE	= 10,	// ��ŸƮŶ���� �Ϸ�
		CAMP_CHANGE_TYPE			= 11,	// ��� ��� Ÿ�� ����
		CAMP_CHANGE_COMPLETE		= 12,	// ��� ��� Ÿ�� ���� �Ϸ�
		CAMP_DESTROY				= 13,	// ��� ��� �ı��� / 7�ϰ� ķ�� �̻��� �ı�
		CAMP_CARE					= 14,	// �����ϱ�

		CAMP_UPDATE_HP				= 15,	// HP ������ ������Ʈ (���� ������ HP ������ ������Ʈ)

		// ä���� ���� ���
		MINING_CAMP_SHOW_MINERAL	= 16,	// ���� ���� Ȯ��
		MINING_CAMP_ON_OFF			= 17,	// ä���� �ѱ� / ����	( dwValue1 : On / Off )
		MINING_CAMP_GAIN_MINERAL	= 18,	// ���� ȹ��			( dwValue1 : ���� Index, dwValue2 : ȹ�淮 )

		// ���� ���� ���
		CAMP_SHOP_CHANGE_TAX		= 19,	// ���� ����
		CAMP_SHOP_GAIN_TAX			= 20,	// ���ͱ� ȹ��

		// ������� ���� ���
		WORLDWEAPON_CHARGE			= 21,	// ���� ����
		WORLDWEAPON_CHARGE_COMPLETE	= 22,	// ���� ���� �Ϸ�
		WORLDWEAPON_FIRE			= 23,	// ���� �߻�
		WORLDWEAPON_FIRE_COMPLETE	= 24,	// ���� �߻� �Ϸ�

		CAMP_NOT_REBUILDTIME		= 25	// ��� ��� �ı� �� �ٽ� ������ �� �ִ� �ð��� ������ �ʾ��� ���.
	};

	enum Error
	{
		FAIL_NOT_EXIST_GUILD		= 2,		// ���� ���� �ʴ� ���
		FAIL_NOT_EXIST_CAMP			= 3,		// ���� ���� �ʴ� ��
		FAIL_NOT_CAMP				= 4,		// ������ �ƴ�
		FAIL_CAMP_STATE				= 5,		// ��� ��� ���°� �̻���
		FAIL_DIFFERENT_GUILD		= 6,		// ��� ���̵� �ٸ�
		FAIL_NOT_MASTER				= 7,		// ��� �����Ͱ� �ƴ�
		FAIL_CAMP_RIGHT				= 8,		// ��� ��� ���� ����
		FAIL_NOT_ENOUGH_MATERIAL	= 9,		// ���� ����
		FAIL_DIFFERENT_GOLD			= 10,		// Ŭ���̾�Ʈ�� ������ ���� �ٸ�
		FAIL_NOT_ENOUGH_GOLD		= 11,		// ��忡 ���� ����
		FAIL_NOT_MINING_CAMP		= 12,		// ä���Ⱑ �ƴ� (ä����� ĳ���� ����)
		FAIL_NOT_COMPLETE_STATE		= 13,		// �Ϸ����(�������)�� �ƴմϴ�.
		FAIL_NOW_MINING				= 14,		// ä�����Դϴ�.
		FAIL_UPGRADE_STEP			= 15,		// ���׷��̵� �ܰ谡 �����մϴ�.
		FAIL_NOT_ENOUGH_MINERAL		= 16,		// �������� �����մϴ�.
		FAIL_NOT_CAMP_SHOP			= 17,		// ��� ��� ������ �ƴմϴ�.
		FAIL_NOT_ZERO_CUSTOMER		= 18,		// ���尴�� ���� ���� ������ ������ �� �����ϴ�.
		FAIL_TOO_MANY_TAX			= 19,		// ������ �ִ�ġ�� �ʰ��Ͽ����ϴ�.
		FAIL_NOT_WORLDWEAPON		= 20,		// ���� ������ �ƴմϴ�.
		FAIL_NOT_EMPTY_WEAPON		= 21,		// ���� ������ ���Ⱑ ��� ���� �ʽ��ϴ�. (������, ������, �߻���)
		FAIL_NOT_READY_WEAPON		= 22,		// ���� ������ ���Ⱑ �غ� �Ǿ� ���� �ʽ��ϴ�. (�������, ������, �߻���)
		FAIL_NOW_CHARGING			= 23,		// ���� ���� ���� �������Դϴ�.
		FAIL_NOW_FIRING				= 24,		// ���� ���� ���� �߻����Դϴ�.
		FAIL_MAX_UPGRADE			= 25,		// ���׷��̵� Max �Դϴ�.
		FAIL_HAS_MINERAL			= 26,		// ä���� Ÿ���� �����Ϸ� �Ҷ� ������ ���� ���� ���
		FAIL_HAS_ITEM				= 27		// ���� Ÿ���� �����Ϸ� �Ҷ� �������� ���� ���� ���
	};

	unsigned long	m_dwCID;			// Client -> GameServer : ���� ��� CID		GameServer -> DBAgentServer : ���� ��� CID Ȥ�� 0
										// Client <- GameServer : ��� ��� CID		GameServer <- DBAgentServer : ��� ��� CID
	unsigned long	m_dwCampID;
	unsigned char	m_cState;

	unsigned long	m_dwValue1;
	unsigned long	m_dwValue2;

	unsigned char	m_cSubCmd;
};



// �� ���� ��� ��Ŷ  ( Client <-> GameServer <-> DBAgentServer )
typedef struct PktCastleCmd*	LPPktCastleCmd;
struct PktCastleCmd : public PktBase
{
	enum SubCmd
	{
		CASTLE_NO_CMD					=  0,
		
		// ��¡�� ���� ���																																																			
		CASTLE_SET_TAX					=  1,		// ���� ����
//		CASTLE_SET_RIGHT				=  2,		// ���� ���� (���� ��Ŷ ���� ����)
		CASTLE_UPGRADE_EMBLEM			=  3,		// ��¡�� ���׷��̵�
		CASTLE_UPGRADE_EMBLEM_COMPLETE	=  4,		// ��¡�� ���׷��̵� �Ϸ�
		CASTLE_UPGRADE_ITEM_INFO		=  5,		// �� ��¡�� ���׷��̵� ���� ����	(GameServer --> DBAgentServer)
		CASTLE_UPDATE_EMBLEM			=  6,		// ��¡�� �μ���
		CASTLE_SUMMON_EMBLEM_COMPLETE	=  7,		// ��¡�� ��ȯ �Ϸ�
		CASTLE_CHANGE_MASTER			=  8,		// ���� ����
		CASTLE_TAKE_TAXMONEY			=  9,		// ���� ȸ��
		CASTLE_LOSE_OWNERSHIP			= 10,		// ��� �ı������� ���� ������
		CASTLE_SHOW_MINERAL				= 11,		// ���� ���� ���� ���� ��û
		CASTLE_GAIN_MINERAL				= 12,		// ���� ȹ�� ( dwValue1 : ����ID, dwValue2 : ȹ�淮 )
																																																											
		// ����/�޹� ���� ���																																																		
		CASTLE_GATE_OPEN				= 13,		// ���� ����
		CASTLE_GATE_CLOSE				= 14,		// ���� �ݱ�
		CASTLE_GATE_PROTECT				= 15,		// ���� ����
		CASTLE_GATE_PROTECT_CANCEL		= 16,		// ���� ���� ����																																								
		CASTLE_GATE_PROTECT_INFO		= 17,		// ���� ���� ����
		CASTLE_UPGRADE_GATE				= 18,		// ���� ���׷��̵�
		CASTLE_UPGRADE_GATE_COMPLETE	= 19,		// ���� ���׷��̵� �Ϸ�
		CASTLE_REPAIR_GATE				= 20,		// ���� ����
		CASTLE_REPAIR_GATE_COMPLETE		= 21,		// ���� ���� �Ϸ�
		CASTLE_RESTORE_GATE				= 22,		// ���� ����
		CASTLE_RESTORE_GATE_COMPLETE	= 23,		// ���� ���� �Ϸ�
		CASTLE_DESTROY_GATE				= 24,		// ���� �ı�
		CASTLE_BACKDOOR					= 25,		// �޹� ���

		// ���� ���� ���� ���																																																	
		CASTLE_CREATE_ARMS				= 26,		// ���� ���� ����
		CASTLE_CREATE_ARMS_COMPLETE		= 27,		// ���� ���� ���� �Ϸ�
		CASTLE_RIDE_ARMS				= 28,		// ���� ���� ž��
		CASTLE_GETOFF_ARMS				= 29,		// ���� ���� ����
		CASTLE_UPGRADE_ARMS				= 30,		// ���� ���� ���׷��̵�
		CASTLE_UPGRADE_ARMS_COMPLETE	= 31,		// ���� ���� ���׷��̵� �Ϸ�
		CASTLE_REPAIR_ARMS				= 32,		// ���� ���� ����
		CASTLE_REPAIR_ARMS_COMPLETE		= 33,		// ���� ���� ���� �Ϸ�
		CASTLE_DESTROY_ARMS				= 34,		// ���� ���� �ı�
		CASTLE_ARMS_CHANGE_MASTER		= 35,		// ���� ���� ���� ����

		// HP ������Ʈ
		CASTLE_UPDATE_HP				= 36,		// HP �� ���Ӽ����� ������ ������Ʈ (GameServer --> DBAgentServer)

		// ���� �߰� ��� ( DBAgentServer -> GameServer )
		CASTLE_GATE_FORCE_OPEN			= 37,		// �߰� ������ ������ ������ ���� ���·� �����.
		CASTLE_GATE_FORCE_CLOSE			= 38,		// �߰� ������ ������ ������ ���� ���·� �����.

		// �� ��¡�� ���� �ް� �ִٴ� �޼��� ���
		CASTLE_EMBLEM_ATTACKED			= 39,		// �� ��¡�� ���� ����
	};

	enum Const
	{
		DESTROY		= 0,		// ���� ���� �ı�
		TO_NPC		= 1,		// ���� ���⸦ ���� ���� NPC �� ( ���� ��� ������ ������ )
	};

	enum Error
	{
		FAIL_NOT_GUILD_MEMBER		= 2,	// ��忡 �������� �ʾҽ��ϴ�.
		FAIL_NOT_EXIST_CASTLE		= 3,	// ���� ���� �ʴ� ��
		FAIL_DIFFERENT_GUILD		= 4,	// ��� ���̵� �ٸ�
		FAIL_NOT_EXIST_GUILD		= 5,	// ���� ���� �ʴ� ���
		FAIL_CASTLE_RIGHT			= 6,	// �� ���� ������ �����ϴ�.
		FAIL_MAX_UPGRADE			= 7,	// �ִ� ���׷��̵� ������
		FAIL_NOT_ENOUGH_JEWEL		= 8,	// ���� ���� ����
		FAIL_JEWEL_KIND				= 9,	// �������� ���� ���� ����
		FAIL_CASTLE_OBJECT_STATE	= 10,	// �� ���� ������Ʈ�� ���°� ����ġ ���� ���
		FAIL_DIFFERENT_GOLD			= 11,	// Ŭ���̾�Ʈ�� ������ ���� �ٸ�
		FAIL_NOT_ENOUGH_GOLD		= 12,	// �ݾ��� ������ ���
		FAIL_ALREADY_RIDE			= 13,	// ���⿡ ž���� ����
		FAIL_NOT_RIDE				= 14,	// ���⸦ Ÿ�� ���� ����
		FAIL_CHANGE_TAX				= 15,	// ������ �����Ҽ� ����
		FAIL_NOT_GUILD_MASTER		= 16,	// ��� �����Ͱ� �ƴϴ�.
		FAIL_REQUEST_TOO_MUCH_GOLD	= 17,	// �ʹ� ���� ���� ��û�߽��ϴ�.
		FAIL_NOT_ENOUGH_MINERAL		= 18,	// �ʹ� ���� ���� ������ ��û�߽��ϴ�.
		FAIL_HAS_ANOTHER_CASTLE_ARMS = 19,	// �̹� �ٸ� ���� ���⸦ ������ �ֽ��ϴ�.
		FAIL_NOT_MY_CASTLE_ARMS		= 20,	// �ڽ��� ���� ���Ⱑ �ƴմϴ�.
		FAIL_NOT_SIEGE_TIME			= 21,	// ���� ���⸦ ����� �ִ� �ð��� �ƴմϴ�.
		FAIL_HAS_ANOTHER_SIEGE_ARMS	= 22,	// �̹� �ٸ� ���� ���⸦ ������ �ֽ��ϴ�.
		NOT_LIMIT_LEVEL				= 23,	// 80 ���� �̸��� ���.
	};

	unsigned long	m_dwCID;						// ���� ��� CID
	unsigned long	m_dwCastleID;					// �� ID
	unsigned long	m_dwCastleObjectID;				// �� ������Ʈ ID

	unsigned long	m_dwValue1;
	unsigned long	m_dwValue2;
	unsigned char	m_cState;

	unsigned char	m_cSubCmd;
};

// �� ���� ����
typedef struct PktCastleRight* LPPktCastleRight;
struct PktCastleRight : public PktBase
{
	enum eError
	{
		FAIL_NOT_EXIST_GUILD		= 2,	// ���� ���� �ʴ� ���
		FAIL_NOT_EXIST_CASTLE		= 3,	// ���� ���� �ʴ� ��
		FAIL_DIFFERENT_GUILD		= 4,	// ��� ���̵� �ٸ�
		FAIL_NOT_MASTER				= 5		// ��� �����Ͱ� �ƴ�
	};

	unsigned long	m_dwCID;			// ���̵� (C -> S : CID)
	unsigned long	m_dwCastleID;
	CastleRight		m_CastleRight;		// �� ���� ����
};


// ��� ��� ���� ����
typedef struct PktCampRight* LPPktCampRight;
struct PktCampRight : public PktBase
{
	enum eError
	{
		FAIL_NOT_EXIST_GUILD		= 2,	// ���� ���� �ʴ� ���
		FAIL_NOT_EXIST_CAMP			= 3,	// ���� ���� �ʴ� ��� ���
		FAIL_NOT_CAMP				= 4,	// ķ���� �ƴ�
		FAIL_DIFFERENT_GUILD		= 5,	// ��� ���̵� �ٸ�
		FAIL_CAMP_STATE				= 6,	// ��� ��� ���°� �̻���
		FAIL_NOT_MASTER				= 7		// ��� �����Ͱ� �ƴ�
	};

	unsigned long	m_dwCID;			// ���̵� (C -> S : CID)
	unsigned long	m_dwCampID;
	CampRight		m_CampRight;		// ��� ��� ���� ����
};

// ���� ������Ʈ ���� �̵� ��Ŷ
typedef struct PktTakeMaterial* LPPktTakeMaterial;
struct PktTakeMaterial : public PktBase
{
	enum SubCmd
	{
		SC_TAKE_IN	= 1,				// ��� ����� ���� �ֱ�
		SC_TAKE_OUT	= 2					// ��� ������� ���� ������
	};

	enum eError
	{
		FAIL_ITEM_NUMBER		= 2,	// ������ �̻���
		FAIL_NOT_EXIST_ITEMINFO	= 3,	// �������� �ʴ� ������ ���� (���� ������ ���̵� �̻�)
		FAIL_OVER_STACK			= 4,	// ���� ���� �ʰ�
		FAIL_NOT_EXIST_CAMP		= 5,	// �ش� ����� ���� ���� ����
		FAIL_CREATE_ITEM		= 6,	// ������ ���� ���� ����
		FAIL_NOT_EXIST_ITEM		= 7,	// ������ ���簡 ����
		FAIL_CAMP_RIGHT			= 8		// ���� ����
	};

	unsigned long	m_dwCID;			// ������
	unsigned long	m_dwCampID;			// ��� ���̵�
	unsigned char	m_cSubCmd;			// �ֱ�/���� Ŀ�ǵ�
	unsigned char	m_cNum;				// ����
	Item::ItemPos	m_Pos;				// �������� ��ġ
};

// �� ���׷��̵� ���� ������ �̵� ��Ŷ
typedef struct PktTakeCastleJewel* LPPktTakeCastleJewel;
struct PktTakeCastleJewel : public PktBase
{
	enum SubCmd
	{
		SC_TAKE_IN	= 1,				// �� ��¡�� ���׷��̵忡 ���� �ֱ�
		SC_TAKE_OUT	= 2					// �� ��¡�� ���׷��̵忡�� ���� ������
	};

	enum eError
	{
		FAIL_NOT_EXIST_CASTLE			= 2,	// ���� ���� �ʴ� ��
		FAIL_NOT_EXIST_EMBLEM			= 3,	// ���� ���� �ʴ� �� ��¡��
		FAIL_WRONG_JEWEL				= 4,	// �������� ���� ����
		FAIL_DIFFERENT_JEWEL			= 5,	// �ٸ� ������ ����
		FAIL_NOT_EXIST_ITEM_PROTOTYPE	= 6,	// ������ ������ Ÿ���� �������� ����
		FAIL_OVER_STACK					= 7,	// ���� ���� �÷���
		FAIL_NOT_EXIST_ITEM				= 8,	// �ش� ��ġ�� ���� �������� �������� ����
		FAIL_ITEM_NUMBER				= 9,	// ������ ������ �̻���
		FAIL_CREATE_ITEM				= 10,	// ���� ������ ���� ����
	};

	unsigned long	m_dwCID;			// ������
	unsigned long	m_dwCastleID;		// �� ���̵�
	unsigned char	m_cSubCmd;			// �ֱ�/���� Ŀ�ǵ�
	unsigned short	m_wItemID;			// ���� ���̵�
	unsigned char	m_cIndex;			// ������ ���� UI ���� ��ġ (0 ~ 9)
	unsigned char	m_cNum;				// ����
	Item::ItemPos	m_Pos;				// �������� ��ġ
};

// ���� ���� ���� ��� ��Ŷ ( Client <-> GameServer <-> DBAgentServer )
typedef struct PktSiegeArmsCmd*	LPPktSiegeArmsCmd;
struct PktSiegeArmsCmd : public PktBase
{
	enum SubCmd
	{
		SIEGE_NO_CMD				= 0,

		SIEGE_CREATE_ARMS			= 1,		// ���� ���� ���� (��� ����, ��ŸƮŶ ���������� ���)
		SIEGE_CREATE_ARMS_COMPLETE	= 2,		// ���� ���� ���� �Ϸ�
		SIEGE_CANCEL				= 3,		// ���� ���� ���� ��� (�ٷε�)
		SIEGE_RIDE_ARMS				= 4,		// ���� ���� ž��
		SIEGE_GETOFF_ARMS			= 5,		// ���� ���� ����
		SIEGE_REPAIR_ARMS			= 6,		// ���� ���� ����
		SIEGE_REPAIR_ARMS_COMPLETE	= 7,		// ���� ���� ���� �Ϸ�
		SIEGE_TO_STARTKIT			= 8,		// ���� ���⸦ ��ŸƮŶ����
		SIEGE_TO_STARTKIT_COMPLETE	= 9,		// ���� ���⸦ ��ŸƮŶ���� �Ϸ�
		SIEGE_DESTROY_ARMS			= 10,		// ���� ���� �ı�

		// HP ������Ʈ
		SIEGE_UPDATE_HP				= 12,		// HP �� ���Ӽ����� ������ ������Ʈ (GameServer --> DBAgentServer)
	};

	enum Error
	{
		FAIL_NOT_EXIST_GUILD		= 2,		// ���� ���� �ʴ� ���
		FAIL_NOT_EXIST_SIEGE_ARMS	= 3,		// ���� ���� �ʴ� ���� ����
		FAIL_NOT_SIEGE_ARMS			= 4,		// ���� ���Ⱑ �ƴ�
		FAIL_NOT_OWNER				= 5,		// ���� �����ڰ� �ƴ�
		FAIL_SIEGE_ARMS_STATE		= 6,		// ���� ������ ���°� ����ġ ����
		FAIL_ALREADY_RIDE			= 7,		// ���⿡ ž���� ����
		FAIL_NOT_RIDE				= 8,		// ���⸦ Ÿ�� ���� ����
		FAIL_RIDE_SIEGE_ARMS		= 9,		// ���� ž�¿� ����
		FAIL_GET_OFF_SIEGE_ARMS		= 10,		// ���� ������ ����
		FAIL_NOT_ENOUGH_MATERIAL	= 11		// ���� ����
	};

	unsigned long	m_dwCID;					// OwnerID ��� ����ȴ�.
	unsigned long	m_dwArmsID;

	unsigned char	m_cState;
	unsigned long	m_dwValue;					// ž���� ĳ���� CID, ���� ĳ���� CID, ������ ��ǥ HP, ���縦 ��� �����ִ°�?
	
	unsigned char	m_cSubCmd;
};


// ���� ������Ʈ ��ε� ĳ��Ʈ
typedef struct PktSiegeBroadCast* LPPktSiegeBroadCast;
struct PktSiegeBroadCast : public PktBase
{
// CASTLE_TODO : ������� �ʴ� ���� ���� ������Ʈ ��ε� ĳ��Ʈ ��Ŷ

	unsigned long	m_dwCID;
	unsigned long	m_dwOwnerID;	// ��, ��� ���, ����ĳ���� ID
	unsigned long	m_dwGID;		// ��� ID
	unsigned long	m_dwNowHP;
	unsigned long	m_dwMaxHP;
	unsigned long	m_dwRiderID[Siege::AIRSHIP_RIDER_NUM];
	unsigned short	m_wObjectType;
	unsigned char	m_cState;
	unsigned char	m_cSubState;
	unsigned char	m_cUpgradeStep;
	unsigned char	m_cUpgradeType;
	float			m_fDefaultDir;
	CNetworkPos		m_NetworkPos;
};

typedef struct PktSiegeBroadCast2nd* LPPktSiegeBroadCast2nd;
struct PktSiegeBroadCast2nd : public PktBase
{
// CASTLE_TODO : ������� �ʴ� ���� ���� ������Ʈ ��ε� ĳ��Ʈ ��Ŷ
};

typedef struct PktNewSiegeBroadCast* LPPktNewSiegeBroadCast;
struct PktNewSiegeBroadCast : public PktBase
{
	unsigned long	m_dwCID;
	unsigned char	m_cType;
};

// ���� �ð� ��Ŷ
struct TimeInfo
{
	unsigned char	m_cMonth;
	unsigned char	m_cDay;
	unsigned char	m_cHour;
	unsigned char	m_cMinute;
	unsigned long	m_dwTime;	// DBAgentServer -> GameServer	: Duration Minutes
								// GameServer -> Client			: Remain Seconds
};

struct GuildTimeInfo
{
	bool			m_bUpdated;
	unsigned char	m_cNowGuildTime;
	TimeInfo		m_TimeInfo;
};

struct RealmTimeInfo
{
	bool			m_bUpdated;
	unsigned char	m_cNowRealmTime;
	TimeInfo		m_TimeInfo;
};

struct SiegeTimeInfo
{
	bool			m_bUpdated;
	unsigned char	m_cNowSiegeTime;
	TimeInfo		m_TimeInfo;
};

struct EnterTimeInfo
{
	bool			m_bUpdated;
	unsigned char	m_cNowEnterTime;
	TimeInfo		m_TimeInfo;
};

struct EventTimeInfo
{
	bool			m_bUpdated;
	unsigned char	m_cNowEventTime;
	TimeInfo		m_TimeInfo;
};

struct CastleSiegeInfo
{
	unsigned long	m_dwCastleID;
	bool			m_bEnableSiege;				// ���� ������ �����Ѱ�?
};

// �������� : + CastleSiegeInfo * SiegeTimeInfo::m_cCastleNum
typedef struct PktGameTimeInfo* LPPktGameTimeInfo;
struct PktGameTimeInfo : public PktBase
{
	unsigned char	m_cType;
	unsigned char	m_cCastleNum;
	unsigned short	m_wSize;

	GuildTimeInfo	m_GuildTimeInfo;
	RealmTimeInfo	m_RealmTimeInfo;
	SiegeTimeInfo	m_SiegeTimeInfo;
	EnterTimeInfo	m_EnterTimeInfo;
	EventTimeInfo	m_EventTimeInfo;
};

// ���ڽ� ��� ���� ���� ��Ŷ
typedef struct PktStealthInfo* LPPktStealthInfo;
struct PktStealthInfo : public PktBase
{
	bool	m_bUseStealth;			// ���ڽ��� ����Ҽ� �ִ°�?
};


// �� ���� ������Ʈ ( DBAgentServer --> GameServer --> Client )
typedef struct PktCastleUpdate* LPPktCastleUpdate;
struct PktCastleUpdate : public PktBase
{
	unsigned long	m_dwCastleID;							// CastleID
	unsigned long	m_dwGID;								// ���� Guild ID
	unsigned short	m_wTax;									// ����
	unsigned long	m_dwTaxMoney;							// ���� ���Ա�
	unsigned char	m_cInvincibleDay;						// ���� ���� Ƚ��
	unsigned char	m_cTaxMoneyRemainDay;					// ���Ա��� �ޱ���� ���� �� (GameServer -> Client)

	CastleRight		m_CastleRight;							// �������� �� ���� ����
//	char			m_szCastleName[MAX_CASTLE_NAME_LEN];	// �� �̸�
};

// ��� ��� ���� �޼��� ��Ŷ
typedef struct PktCampMessage* LPPktCampMessage;
struct PktCampMessage : public PktBase
{
	enum { MAX_NAME_LEN	= 16 };

	enum
	{
		MSGCMD_ENCAMP_START					= 0,	// ��� ��� ���� ����
		MSGCMD_ENCAMPING,							// ��� ��� ���� ��
		MSGCMD_ENCAMP_END,							// ��� ��� ���� �Ϸ�
		
		MSGCMD_CANCEL_START,						// ��� ��� ���� ���
		MSGCMD_CANCEL_END,							// ��� ��� ���� ��� �Ϸ�
		
		MSGCMD_REPAIR_START,						// ��� ��� ����
		MSGCMD_REPAIR_END,							// ��� ��� ���� �Ϸ�
		
		MSGCMD_UPGRADE_START,						// ��� ��� ���׷��̵�
		MSGCMD_UPGRADE_END,							// ��� ��� ���׷��̵� �Ϸ�

		MSGCMD_SET_RIGHT,							// ��� ��� ���� ���� ����

		MSGCMD_SELF_DESTROY_START,					// ��� ��� ������ �ı� ����
		MSGCMD_SELF_DESTROY_END,					// ��� ��� ������ �ı� �Ϸ�

		MSGCMD_CHANGE_TYPE_START,					// ��� ��� Ÿ�� ���� ����
		MSGCMD_CHANGE_TYPE_END,						// ��� ��� Ÿ�� ���� �Ϸ�

		MSGCMD_ATTACKED,							// ��� ��� ���� ����
		MSGCMD_DESTROYED,							// ��� ��� �ı��� (�ı� ����)
		MSGCMD_DESTROY,								// ��� ��� �ı��� (�ı� ��)

		MSGCMD_MINING_END,							// ä���� ä�� �Ϸ�

		MSGCMD_SHOP_GAIN_TAX,						// ��� ��� ���� ���� ���ͱ� ȹ��

		MSGCMD_WEAPON_ENCAMP_START,					// ���� ���� ���� ����
		MSGCMD_WEAPON_ENCAMPING,					// ���� ���� ���� ��
        MSGCMD_WEAPON_ENCAMP_END,					// ���� ���� ���� �Ϸ�

		MSGCMD_WEAPON_CANCEL_START,					// ���� ���� ���� ���
		MSGCMD_WEAPON_CANCEL_END,					// ���� ���� ���� ��� �Ϸ�

		MSGCMD_WEAPON_REPAIR_START,					// ���� ���� ����
		MSGCMD_WEAPON_REPAIR_END,					// ���� ���� ���� �Ϸ�

		MSGCMD_WEAPON_UPGRADE_START,				// ���� ���� ���׷��̵�
		MSGCMD_WEAPON_UPGRADE_END,					// ���� ���� ���׷��̵� �Ϸ�

		MSGCMD_WEAPON_SET_RIGHT,					// ���� ���� ���� ���� ����
		
		MSGCMD_WEAPON_SELF_DESTROY_START,			// ���� ���� ������ �ı� ����
		MSGCMD_WEAPON_SELF_DESTROY_END,				// ���� ���� ������ �ı� �Ϸ�

		MSGCMD_WEAPON_CHARGE_START,					// ���� ���� ���� ���� ����
		MSGCMD_WEAPON_CHARGE_END,					// ���� ���� ���� ���� �Ϸ�

		MSGCMD_WEAPON_ATTACKED,						// ���� ���� ���� ����
		MSGCMD_WEAPON_DESTROYED,					// ���� ���� �ı���

		MSGCMD_WEAPON_FIRE_WARNING,					// ���� ���� �߻� ���

		MSGCMD_TAKE_IN_MATERIAL,					// ��� ����� ���� �ֱ�
		MSGCMD_TAKE_OUT_MATERIAL,					// ��� ������� ���� ����

		MSGCMD_GUILD_TIME_COMPLETE,					// ��� ������ �ð� �Ϸ�.

		MSGCMD_MAX_CMD
	};

	struct sRight
	{
		unsigned char m_cCampControl;
	};

	char			m_szMasterName[MAX_NAME_LEN];
	char			m_szPCName[MAX_NAME_LEN];
	char			m_szGuildName[Guild::MAX_GUILD_NAME_LEN];
	unsigned long	m_dwCampID;
	unsigned long	m_dwGID;
	unsigned char	m_cZone;
	unsigned char	m_cMsgCmd;
	unsigned char	m_cRemainTime;
	unsigned char	m_cNation;
	sRight			m_Right;
	bool			m_bNotify;
	int				m_nValue1;	// ��ġ, ������ Ÿ��, ���� ���� �߻� X��ǥ, ��������� �ı����� ����, ���� ���� �̵� ����
	int				m_nValue2;	// ���� Ÿ��, ���� ���� �߻� Z��ǥ, ���� ���� ���� ����

	unsigned char	m_cTimeChk;		// �ִ� 256 ��.
};

struct PktWarOnOff : public PktBase
{
	enum Error
	{
		E_WAR_OFF		= 2,	// ���� �÷��׸� Off ��ų�� ����
		E_WAR_ON		= 3,	// ���� �÷��׸� On ��ų�� ����
		E_WAR_INSTANCE	= 4,	// ���� �÷��׸� Instance ��ų�� ����
		E_INVALID_TYPE	= 5		// �������� �ʴ� ���� Ÿ��
	};

	unsigned long	m_dwCID;	
	unsigned char	m_cType;	// GameTime::eTimeType
	unsigned char	m_cFlag;	// Creature::WarFlag
};

// �������� �������� ����Ʈ.
struct PktRealmPoint : public PktBase
{
	enum
	{
		REALM_JOIN		= 1,				// ���� ���￡ ���� ���� ���.
		REALM_OUT		= 2					// ���� ���￡ ������ ���� ���.
	};

	unsigned char	m_cRealmPoint;			// �������� ����Ʈ.
	unsigned long	m_dwCID;				// ĳ���� CID.
	unsigned char	m_cType;				// ����, ������.
};

#pragma pack()

#endif	// _CASTLE_PACKET_H_