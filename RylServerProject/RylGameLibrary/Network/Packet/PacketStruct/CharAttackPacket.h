#ifndef _CHAR_ATTACK_PACKET_H_
#define _CHAR_ATTACK_PACKET_H_

// CharAttackPacket.h
#include <DB/DBDefine.h>
#include <Network/Packet/PacketBase.h>
#include <Network/Packet/PacketStruct/DataPacket.h>

#include "CharAttackPacketStruct.h"
#include <Community/Guild/GuildConstants.h>

#pragma pack(1)


// ĳ���� ���� ��Ŷ
typedef struct PktAt* LPPktAt;
struct PktAt : public PktBase
{
	POS				m_Postion;      // ������ ĳ���� ��ġ
	float			m_fDir;         // ������ ����

	unsigned long	m_dwCharID;	    // ������ ĳ���� ���̵�

	AtType			m_AtType;       // ���� Ÿ��
	AtNode			m_AtNode;       // ����� ����
};

// ĳ���� ���� ��Ŷ Ack (���� ����) + ����� ���
typedef struct PktAtAck* LPPktAtAck;
struct PktAtAck : public PktBase 
{
    enum PktAtAckErr
	{
		FAIL_ALREADY_DEAD		= 2,	// �̹� ���� ũ���Ŀ��� ����
		FAIL_TOO_FAR			= 3,	// �Ÿ��� �־����� ��ų ��� �Ұ�
		FAIL_SUICIDE			= 4,	// ��ų�� ����ϸ� �ڻ��ϰ� �ȴ�.
		FAIL_PEACE_TO_WAR		= 5,	// ���� ��� ĳ���Ͱ� ���� ��� ĳ���Ϳ��� �������� ��æƮ �ο� �õ�
		FAIL_SPELL				= 6,	// ���� ��� ����
		FAIL_TO_SIEGE_OBJECT	= 7,	// ���� ������Ʈ���� �Ҽ� ���� ���� �õ�
		FAIL_TO_MONSTER			= 8,	// ���Ϳ��� �Ҽ� ���� ���� �õ�
		FAIL_FRIENDLY_ATTACK	= 9,	// �Ʊ����� �����Ҽ� ����
		FAIL_NOT_SIEGE_ATTACK	= 10,	// ���������� ������ �߸��Ǿ���.
		FAIL_NOT_STEALTH		= 11,	// ���ڽ��� ����� �� ����.
	};

	unsigned long	m_dwCharID;     // ������ ĳ���� ���̵�
	AtType			m_AtType;       // ���� Ÿ��

	unsigned short	m_wHP;			// ������ HP
	unsigned short	m_wMP;			// ������ MP
	unsigned short	m_wMPHeal;		// ������ MP ȸ����

	unsigned char	m_cJudge;       // �÷���
	unsigned char	m_cDefenserNum; // ����� ��
};


// ĳ���� ���� ���� (���� ����) + ����� ���
typedef struct PktAtInfo* LPPktAtInfo;
struct PktAtInfo : public PktBase 
{
	unsigned long	m_dwCharID;     // ĳ���� ���̵�
	AtType			m_AtType;       // ���� Ÿ��
	unsigned char	m_cDefenserNum; // ����� ��
};


// ĳ���� ���� ���� ��Ŷ
typedef struct PktAted* LPPktAted;
struct PktAted : public PktBase
{
	POS				m_Postion;      // ������ ��ġ
	float			m_fDir;         // ������ ����

	unsigned long	m_dwCharID;     // ������ ���̵�
	unsigned long	m_dwMyID;		// ����� ���̵�

	AtType			m_AtType;       // ���� Ÿ��

	unsigned short	m_wMyHP;		// ����� HP
	unsigned short	m_wMyMP;		// ����� MP
	unsigned short	m_wMyMPHeal;	// ����� MP ȸ����

   	unsigned char	m_cJudge;       // ���� ��
};


// ĳ���� ��� ������
typedef struct PktEquipDurability* LPPktEquipDurability;
struct PktEquipDurability : public PktBase
{
	unsigned long	m_dwCharID;		// ĳ���� ���̵�

	unsigned char	m_cIndex;		// ��� ��ġ
	unsigned char	m_cValue;		// ���� ������
	unsigned char	m_cValueMax;	// ���� �ִ볻����
};


// ��� ���
//
//  ��� :
//	0 = ����,	1 = ����,	2 = ����,	3 = �α׾ƿ�,	4 = ���
//
//  ���� :
//	0 = ����,	1 = ���� ����,	2 = �̹� ������� ĳ���Ϳ��� ��� ��û,
//	3 = �ź� �ɼ��� ���� ����
typedef struct PktDuC* LPPktDuC;
struct PktDuC : public PktBase 
{
	enum DuelCmd 
	{ 
		DUC_PROPOSE		= 0,	DUC_READY		= 1,			DUC_REFUSE			= 2, 
		DUC_LOGOUT		= 3,	DUC_DEAD		= 4,			DUC_PARTY_PROPOSE	= 5,
		DUC_PARTY_READY	= 6,	DUC_REQUEST_PARTY_INFO	= 7,	DUC_CANCEL			= 8
	};

	enum PktDuCError
	{
		NO_SERVER_ERR	= 0,	SERVER_ERROR	= 1,	FAIL_DUELING_CHAR	= 2,
		FAIL_REJECT		= 3,	FAIL_NOT_LEADER	= 4,	FAIL_NOT_PARTY		= 5
	};

	unsigned long	m_dwSenderID;	// ������ ���̵�
	unsigned long	m_dwRecverID;	// �޴��� ���̵�
   	unsigned char   m_cCmd;         // ���
};


// ����Ʋ ����
typedef struct PktTBInfo* LPPktTBInfo;
struct PktTBInfo : public PktBase
{
	enum DuelCmd 
	{ 
		DUC_PROPOSE		= 0,	DUC_READY		= 1,	DUC_REFUSE			= 2, 
		DUC_LOGOUT		= 3,	DUC_DEAD		= 4,	DUC_PARTY_PROPOSE	= 5,
		DUC_PARTY_READY	= 6,	DUC_REQUEST_PARTY_INFO	= 7
	};

	unsigned long m_dwCharID;
	char m_strChallengerName[CHAR_INFOST::MAX_NAME_LEN];
    unsigned char m_cCmd;

	unsigned char m_cMemberNum;
};


// ĳ���� æƮ&��æƮ ����
typedef struct PktSpInfo* LPPktSpInfo;
struct PktSpInfo : public PktBase 
{
	unsigned long	m_dwCharID;         // ĳ���� ���̵�
	unsigned short	m_nEnchantLevel;    // �ֹ� ����
	unsigned char	m_cSpellType;       // �ֹ� Ÿ�� ( namespace Skill::SpellType���� )
	unsigned long	m_dwEnchantTime;    // �ֹ� �����ð�
	bool			m_bOnOff;           // ����/����	
};


// ĳ���� ������
typedef struct PktRs* LPPktRs;
struct PktRs : public PktBase
{
	enum RespawnCmd
	{
		RS_NORMAL				= 0,		// �Ϲ����� ��� (�⺻ ������ ��ҿ��� ������)
		RS_SELECT				= 1,		// ������ ��� ���� Ȯ��
		RS_FORMER_PLACE			= 2,		// ���ڸ� ������ (��ų�� ����)

		RS_BG_HUMAN1			= 3,		// �޸� ����Ʈ1 (��Ʋ�׶���)
		RS_BG_HUMAN2			= 4,		// �޸� ����Ʈ2 (��Ʋ�׶���)
		RS_BG_HUMAN3			= 5,		// �޸� ����Ʈ3 (��Ʋ�׶���)
		RS_BG_AKHAN1			= 6,		// ��ĭ ����Ʈ1 (��Ʋ�׶���)
		RS_BG_AKHAN2			= 7,		// ��ĭ ����Ʈ2 (��Ʋ�׶���)
		RS_BG_AKHAN3			= 8,		// ��ĭ ����Ʈ3 (��Ʋ�׶���)

		RS_BGSERVER_RANDOM		= 9,		// ��Ʋ �׶��� ������
		
		RS_BG_EXIT				= 10		// ��Ʋ�׶��� Ż��
	};

	unsigned long	m_dwCharID;
	unsigned char	m_cCmd;
	unsigned long	m_dwTownID;
	POS				m_Position;
};

// ĳ���� ������ Ack
typedef struct PktRsAck* LPPktRsAck;
struct PktRsAck : public PktBase 
{
	enum Error
	{
		ERR_NOT_EXIST_CAMP	= 2,	// �������� �ʴ� ���
	};

	POS				m_Position;     // ���� ��ġ
    __int64			m_dlExp;        // ���� ������
	unsigned long	m_dwGold;		// ���� ���� �ݾ�

	unsigned long	m_dwCharID;     // ĳ���� ���̵�
	unsigned short	m_wHP;          // HP
	unsigned short	m_wMP;          // MP
};

// ������ Ack (��Ʋ�׶��� �����)
typedef struct PktBGRsAck* LPPktBGRsAck;
struct PktBGRsAck : public PktBase
{
	enum PktBGRsAckErr
	{
		FAIL_PERSONNEL_OVER	= 2
	};

	unsigned long	m_dwCharID;		// ĳ���� ���̵�

	unsigned short	m_wTurn;		// ĳ���� ����
	unsigned short	m_wWaitNum;		// ��ü ����� ��

	unsigned short	m_wLeftTime;	// ���� �ð�

	unsigned short	m_wHumanNum;	// ��ü �޸� ��
	unsigned short	m_wAkhanNum;	// ��ü ��ĭ ��
};

// ������ ť ����� ���� (��Ʋ�׶����)
typedef struct PktRsWaitQueue* LPPktRsWaitQueue;
struct PktRsWaitQueue : public PktBase
{
	unsigned long	m_dwCharID;		// ĳ���� ���̵�
};

// ������ ť ����� ���� Ack (��Ʋ�׶����)
typedef struct PktRsWaitQueueAck* LPPktRsWaitQueueAck;
struct PktRsWaitQueueAck : public PktBase
{
	enum { MAX_POINT_NUM_PER_RACE = 3};

	unsigned long	m_dwCharID;								// ĳ���� ���̵�
	unsigned short	m_wWaitNum[MAX_POINT_NUM_PER_RACE];		// �� ������ ��ġ�� ����� ��
};

// ĳ���� ������ ���� (������ ��ġ��)
// �߰� ������ �ڿ� ���� �پ ����.
typedef struct PktRsInfo* LPPktRsInfo;
struct PktRsInfo : public PktBase
{
	unsigned long m_dwCharID;
};

// ĳ���� ������ ���� Ack (������ ��ġ��)
// �߰� ������ �ڿ� ���� �پ ����.
typedef struct PktRsInfoAck* LPPktRsInfoAck;
struct PktRsInfoAck : public PktBase
{
	enum Const
	{
		MAX_RESPAWN_POINT	= 50
	};

	unsigned long	m_dwCharID;
	unsigned char	m_cRsAreaNum;
	unsigned short	m_wSize;
};

struct RespawnArea
{
	unsigned long	m_dwTownID;
	POS				m_RespawnPos;
};

// ĳ���� ������ ���� ���� ����
typedef struct PktRsAreaInfo* LPPktRsAreaInfo;
struct PktRsAreaInfo : public PktBase
{
	unsigned long m_dwCharID;
	unsigned long m_dwTownID;	// ����, ��� ���, �� ID
};

// ĳ���� ������ ���� ���� ���� Ack
typedef struct PktRsAreaInfoAck* LPPktRsAreaInfoAck;
struct PktRsAreaInfoAck : public PktBase
{
	unsigned long	m_dwCharID;
	unsigned long	m_dwTownID;
	unsigned short	m_nWaitOrder;	// ������
	unsigned char	m_cRemainHour;	// ���� �ð� (��)
	unsigned char	m_cRemainMin;	// ���� �ð� (��)
	unsigned char	m_cRemainSec;	// ���� �ð� (��)
	bool			m_bCount;		// �ð� ī���� ���ۿ���
};

// ����� ó��
typedef struct PktAw* LPPktAw;
struct PktAw : public PktBase
{
	unsigned long	m_dwCharID;     // ĳ���� ���̵�
	unsigned long	m_dwExp;        // ����ġ
};


// ����/������ ��ȯ
typedef struct PktPeace* LPPktPeace;
struct PktPeace : public PktBase
{
	enum PktPeaceErr
	{
		FAIL_WRONG_SYNC		= 2,	// Ŭ���̾�Ʈ/���� ���� ��ũ�� ���� ����
		FAIL_BATTLE_GROUND	= 3,	// ��Ʋ�׶��忡���� ��� ���� �Ұ���
		FAIL_GUILD_MEMBER	= 4		// ������ ��� ���� �Ұ���
	};

	unsigned long	m_dwCharID;		// ĳ���� ���̵�
	unsigned char	m_cLeftTime;	// ���� �ð�
	bool			m_bPeace;		// ���� ���

};


// ��ȯ�� ���
typedef struct PktSummonCmd* LPPktSummonCmd;
struct PktSummonCmd : public PktBase
{
	enum SummonCmd
	{
		SUC_ATTACK		= 0,
		SUC_RETURN		= 1,
		SUC_DIE			= 2,
		SUC_GUARD_ON	= 3,
		SUC_GUARD_OFF	= 4
	};

	unsigned long	m_dwCharID;		// ĳ���� ���̵�
	unsigned long	m_dwTargetID;	// ��� ���̵�
	unsigned char	m_cCmd;			// ���
};


// ���� ����
typedef struct PktStatueInfo* LPPktStatueInfo;
struct PktStatueInfo : public PktBase
{
	unsigned long	m_dwMileage;	// ���ʽ��� �߰��� ����޴� (0�̸� ���ʽ� ����)

	unsigned short	m_wHumanNum;	// ��ü �޸� ��
	unsigned short	m_wAkhanNum;	// ��ü ��ĭ ��

	unsigned short	m_wNowHP;		// ������ ���� HP
	unsigned short	m_wKID;			// ���� ���� ���̵�
	unsigned char	m_cState;		// ���� ���� (���� State�� ����)
	unsigned char	m_cIndex;		// ���� �ε���
};


// ī�޶� ��ũ��Ʈ
typedef struct PktCameraScript* LPPktCameraScript;
struct PktCameraScript : public PktBase
{
	unsigned long	m_dwDeleteObject;	// ������� ������Ʈ ���̵�
	unsigned long	m_dwNewObject;		// �����Ǵ� ������Ʈ ���̵�
	POS				m_NewPos;			// �����Ǵ� ������Ʈ ��ġ
};

// ��� �� ��Ȱ ����
struct PktDeadInfo : public PktDD 
{
    enum TypeCode
    {
        DEAD        = 0,
        RESPAWN     = 1,
        RESURRECT   = 2
    };

    /*
    typecode:00 HP/MP ������CID / �������̸� / ������CID / �������̸� / �����ڱ���
    typecode:00 HP/MP ������CID / �������̸� / ������CID / �������̸� /

    'XXXXX���� ���� YYYYY�� ���� ����ϼ̽��ϴ�.'
    'XXXXX���� YYYYY����� ZZZZZ�Կ� ���� ����ϼ̽��ϴ�.'		
    'XXXXX���� ZZZZZ�Կ� ���� ����ϼ̽��ϴ�.'

    typecode:01 HP/MP ������CID / �������̸� /
    'XXXXX���� �Ϲ� ��Ȱ�� �����ϼ̽��ϴ�. �����̳� ������� �������ϰ� �˴ϴ�.'

    typecode:02 HP/MP ������CID / �������̸� / �츰��CID / �츰���̸� / �츰�ѱ���
    typecode:02 HP/MP ������CID / �������̸� / 
    'XXXXX�Կ� ���� YYYYY���� ��Ȱ �ϼ̽��ϴ�.'
    '�������� ���� YYYYY���� ��Ȱ �ϼ̽��ϴ�.'
    */

    unsigned char       m_cTypeCode;            // Ÿ���ڵ�
    unsigned short      m_usDeadMemCurrHP;      // ������ HP
    unsigned short      m_usDeadMemCurrMP;      // ������ MP

    unsigned long       m_dwDeadCID;            // ������ CID
    unsigned long       m_dwDeadPID;            // ������ PID
    unsigned long       m_dwReferenceID;        // ���̰ų� �츰�� CID

	unsigned char		m_cDeadNation;			// ������ ����
	unsigned char		m_cDeadClass;			// ������ Ŭ����
	unsigned char		m_cRefNation;			// ���̰ų� �츰�� ����
	unsigned char		m_cRefClass;			// ���̰ų� �츰�� Ŭ����

    char                m_szDeadName[CHAR_INFOST::MAX_NAME_LEN];
    char                m_szRefName[CHAR_INFOST::MAX_NAME_LEN];
    char                m_szRefGuildName[Guild::MAX_GUILD_NAME_LEN];
};


#pragma pack()

#endif