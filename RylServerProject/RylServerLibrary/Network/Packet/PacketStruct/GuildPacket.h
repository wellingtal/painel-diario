#ifndef _GUILD_PACKET_H_
#define _GUILD_PACKET_H_

#include <Network/Packet/PacketBase.h>
#include <Creature/Character/CharacterClass.h>
#include <Creature/CreatureStructure.h>

#include <Community/Guild/GuildConstants.h>
#include <Community/Guild/GuildStructure.h>

#pragma pack(1)

using namespace Guild;

// ��� ����
struct PktCreateGuild : public PktBase
{
	enum PktCreateGuildErr
	{
		FAIL_ALREADY_NAME	= 2,	// �̹� �����ϴ� �̸�
		FAIL_DISSOLVE_GUILD	= 3,    // ��� ��ü
        FAIL_INVALID_NAME   = 4     // �������� �̸�
	};

    enum DissolveReason
    {
        NONE_LOGIN                                      = 1,    // �������� �ƹ��� �α������� ����
        MIN_MEMBER_TIMEOUT                              = 2,    // ������ �ּ� �ο� �̸��� ä ���� �ð� ���
        NONE_NEXT_GUILDMASTER_BY_NONE_LOGIN             = 3,    // �����Ⱓ ��帶���Ͱ� �α������� �ʾ�, ���� �̾��Ϸ� �� �ÿ� �渶�� ���� ���
        NONE_NEXT_GUILDMASTER_BY_GUILDMASTER_OUT        = 4,    // ��帶���� Ż��ÿ� ���� �̾��Ϸ� �� �ÿ� �渶�� ���� ���
        NONE_NEXT_GUILDMASTER_BY_GUILDMASTER_DELETED    = 5     // ��帶���� �����ÿ� ���� �̾��Ϸ� �� �ÿ� �渶�� ���� ���
    };

	unsigned long	m_dwCID;
	unsigned long	m_dwGID;
	unsigned char	m_cInclination;
	char			m_szGuildName[MAX_GUILD_NAME_LEN];
};

// ��� ����Ʈ (���� ����) + ��� ���� 
struct PktGuildList : public PktBase
{
	enum { MAX_NUM_PER_PAGE = 10 };

	unsigned long	m_dwCID;			// ĳ���� ���̵�

	unsigned char	m_cSortCmd;			// ���
	unsigned char	m_cPage;			// ������

	unsigned char	m_cSmallNodeNum;	// ��� �� (C -> S : üũ�� ���, S -> C : ���� ���)
	unsigned char	m_cLargeNodeNum;	// ��� �� (��ü ���� ���)
};

// ��� ���� (üũ��)
struct GuildCheckSumNode
{
	unsigned long	m_dwGID;			// ��� ���̵�
	unsigned long	m_dwCheckSum;		// üũ��
};

// ��� ���� (���� �������� ��Ȥ ��û�Ǵ� ������ �͵�)
struct GuildSmallInfoNode
{
	// TODO : ����, ����, ��� �̸��� GuildLargeInfoNode�� ���� �� �� �մϴ�.
	//	(������ ��� �̸��� ������� �ʽ��ϴ�. ������ ��ũ�� ����ó�� ����ø��� ��ε�ĳ�����ϸ� �˴ϴ�.)
	//	���Ҿ� ��� ������ �����Ǵ� ����� GuildLargeInfoNode�� ��ε�ĳ�����ؾ� �մϴ�.
	//	(�� �� ������ ���� ��� ���� ��Ŷ ���� ��ÿ� �����Ͱ� �������� ���� �����̹Ƿ� ������ �̸��� ������ �־���� �մϴ�.)

	unsigned long	m_dwGID;				// ��� ���̵�
	unsigned char	m_cIndex;				// ����Ʈ���� �ε���
	unsigned char	m_cInclination;			// ��� ����

	unsigned short	m_wRank;				// ����
	unsigned long	m_dwFame;				// ��
	unsigned char	m_cLevel;				// ����
	unsigned char	m_cCurrentMemberNum;	// �ο���

	char	m_szName[MAX_GUILD_NAME_LEN];			// ��� �̸�
	char	m_szMasterName[MAX_MEMBER_NAME_LEN];	// ������ �̸�

	GuildSmallInfoNode()
		:	m_dwGID(0), m_cIndex(0), m_cInclination(0), m_wRank(0), m_dwFame(0), m_cLevel(0), m_cCurrentMemberNum(0)
	{
		std::fill_n(m_szName, int(MAX_GUILD_NAME_LEN), 0);
		std::fill_n(m_szMasterName, int(MAX_MEMBER_NAME_LEN), 0);
	}

	GuildSmallInfoNode(unsigned long dwGID, unsigned char cIndex, unsigned char cInclination, 
		unsigned short wRank, unsigned long dwFame, unsigned char cLevel, 
		unsigned char cCurrentMemberNum, const char* szGuildName, const char* szMasterName)
		:	m_dwGID(dwGID), m_cIndex(cIndex), m_cInclination(cInclination), 
			m_wRank(wRank), m_dwFame(dwFame), m_cLevel(cLevel), m_cCurrentMemberNum(cCurrentMemberNum)
	{
		if (NULL != szGuildName)
		{
			strncpy(m_szName, szGuildName, MAX_GUILD_NAME_LEN);
		}

		if (NULL != szMasterName)
		{
			strncpy(m_szMasterName, szMasterName, MAX_MEMBER_NAME_LEN);
		}
	}
};

// ��� ���� (���)
struct GuildLargeInfoNode : public GuildSmallInfoNode
{
	char			m_szMark[MAX_MARK_SIZE];		// ��ũ
	unsigned char	m_cRelationByMine;				// Ŭ���̾�Ʈ(����)�� �� ��忡 ���� ��� ����
	unsigned char	m_cRelationByTarget;			// �� ��尡 Ŭ���̾�Ʈ(����)���� ���� ��� ����

	GuildLargeInfoNode()
		:	GuildSmallInfoNode(), m_cRelationByMine(0), m_cRelationByTarget(0)
	{
		std::fill_n(m_szMark, int(MAX_MARK_SIZE), 0);
	}

	GuildLargeInfoNode(unsigned long dwGID, unsigned char cIndex, unsigned char cInclination, 
		unsigned short wRank, unsigned long dwFame, unsigned char cLevel, 
		unsigned char cCurrentMemberNum, const char* szMasterName, const char* szGuildName, 
		const char* szMark, unsigned char cRelationByMine, unsigned char cRelationByTarget)
		:	GuildSmallInfoNode(dwGID, cIndex, cInclination, wRank, dwFame, cLevel, cCurrentMemberNum, szGuildName, szMasterName), 
			m_cRelationByMine(cRelationByMine), m_cRelationByTarget(cRelationByTarget)
	{
		if (NULL != szMark)
		{
			memcpy(m_szMark, szMark, MAX_MARK_SIZE);
		}
	}
};

// ��� ��� ���� ���
struct PktGuildCmd : public PktBase
{
	enum GuildCmd
	{ 
		GC_INVITE			= 0,			// �ʴ�
		GC_JOIN				= 1,			// ���� (�ʴ뿡 ����)
		GC_REFUSE			= 2,			// ���� (�ʴ뿡 ����)
		GC_ASK_ENTER		= 3,			// ���� ��û
		GC_ASK_LEAVE		= 4,			// Ż�� ��û
		GC_KICK				= 5,			// ���� Ż��
		GC_SETUP_TITLE		= 6,			// ���� ����
		GC_LOGINOUT			= 7,			// �α���/�ƿ�
		GC_RANK_UP			= 8,			// ���� ����
		GC_RANK_DOWN		= 9,			// ���� �Ʒ���
		GC_TACTICS			= 10,			// �뺴 ���� ��û.
		GC_TACTICS_JOIN		= 11,			// �뺴 ���� �ʴ�.
		GC_TACTICS_TITLE	= 12,			// �뺴 �㰡.
		GC_TACTICS_KICK		= 13,			// �뺴 ����.
		GC_TACTICS_LEAVE	= 14,			// �뺴 Ż��.
		GC_TACTICS_REQUEST	= 15,			// ��� ���� �� �뺴 �㰡 �� ĳ���� Ȯ�� ��û.
	};

	enum PktGuildCmdErr
	{
		FAIL_NOT_MEMBER			= 2,	// ������ �ƴ�
		FAIL_NOT_RIGHT			= 3,	// ������ ����
		FAIL_ALREADY_MEMBER		= 4,	// �̹� ������
		FAIL_OVER_MEMBER		= 5,	// ��� ���� �ʰ�
		FAIL_NATION				= 6,	// �ٸ� ������ ���

		FAIL_DIFFERENCE_TITLE	= 7,	// �ٸ� �������� ���� ����
		FAIL_OVER_MEMBER_NUM	= 8,	// ���� �� ������ �ʰ��ϴ� ���� ����

		FAIL_GUILDWAR_TIME_KICK	= 9,	// ������� �ð��� Ż�� �õ��ϴ� ���

		FAIL_TACTICS_LIMITLEVEL			= 10,	// �뺴 ��û ������ �ȵǴ� ĳ����.
		FAIL_TACTICS_NATION				= 11,	// ������ �ٸ� ĳ����.
		FAIL_TACTICS_GUILDJOIN			= 12,	// ��峪 �뺴�� ���ԵǾ��ְų� ��û���� ĳ����.
		FAIL_TACTICS_OVER_MEMBER		= 13,	// ��� ���� �ʰ�.

		FAIL_TACTICS_LOGOUT				= 14	// �뺴 �㰡 �� �α׾ƿ� �����϶�.
	};

	unsigned long	m_dwGID;			// ��� ���̵�

	unsigned long	m_dwSenderID;	    // ĳ���� ���̵� (��ü)
	unsigned long	m_dwReferenceID;	// ���� ���̵� (��� ĳ����/���� ���̵�/���� ��Ʈ)

	char			m_szGuildName[MAX_GUILD_NAME_LEN];		// ��� �̸�
	char			m_szSenderName[MAX_MEMBER_NAME_LEN];	// ĳ���� �̸� (��ü)

	unsigned short	m_wCmd;             // ���
};

// ��� ����Ʈ (���� ����) + ��� ���� 
struct PktGuildMemberList : public PktBase
{
	enum { MAX_NUM_PER_PAGE = 10 };

	unsigned long	m_dwCID;			// ĳ���� ���̵�

	unsigned char	m_cMemberType;		// �������Ʈ, �뺴����Ʈ
	unsigned char	m_cSortCmd;			// ��� (Guild::MemberSort)
	unsigned char	m_cPage;			// ������

	unsigned char	m_cTotalMemberNum;	// �� ��� ��
	unsigned char	m_cNodeNum;			// ��� �� (��� ���)
};

// ��� ����
struct GuildMemberInfoNode
{
	unsigned long	m_dwCID;

	char			m_szName[MAX_MEMBER_NAME_LEN];
	unsigned char	m_cTactics;

	unsigned long	m_dwFame;
	unsigned long	m_dwGold;
	unsigned long	m_dwServerID;

	unsigned char	m_cRank;
    unsigned char	m_cTitle;
	unsigned char	m_cLevel;
	unsigned char	m_cClass;
	unsigned char	m_cGuildWarFlag;
};

// ��� ���� ������Ʈ (Game <-> DBAgent)
struct PktGuildMemberInfoUpdate : public PktBase
{
	unsigned long		m_dwGID;
	unsigned long		m_dwCID;

	MemberListInfo		m_MemberListInfo;
	MemberDetailInfo	m_MemberDetailInfo;
};

// ��� �� ������Ʈ
struct PktGuildMemberFameUpdate : public PktBase
{
	unsigned long		m_dwOurGID;
	unsigned long		m_dwEnemyGID;
	unsigned long		m_dwCID;
	unsigned char		m_cType;
	int					m_nAddFame;
};

// ��� �� ������Ʈ
struct PktGuildMemberGoldUpdate : public PktBase
{
	unsigned long		m_dwGID;
	unsigned long		m_dwCID;
	unsigned long		m_dwGold;
};

// ��� ��ũ
struct PktGuildMark : public PktBase
{
	enum eError
	{
		FAIL_INVALID_GUILD		= 2,		// ���������ʴ� ���
		FAIL_INVALID_MARK		= 3,		// ������ ��ũ�� ����
		FAIL_INVALID_CHARACTER	= 4,		// ���������ʴ� ĳ����
		FAIL_NOT_MASTER			= 5,		// �����Ͱ� �ƴ�
		FAIL_NOT_ENOUGH_GOLD	= 6,		// �ݾ� ����
	};

	unsigned long	m_dwCID;					// ĳ���� ���̵�
	unsigned long	m_dwGID;					// ��� ���̵�

	char			m_szMark[MAX_MARK_SIZE];	// ��ũ
	unsigned long	m_dwGold;					// �ݰ� �ܾ�
};

// ��� ����
struct PktGuildLevel : public PktBase
{
	unsigned long	m_dwUID;			// ���̵� (C -> S : ĳ���� ���̵�, S -> C : ��� ���̵�)
	unsigned char	m_cLevel;			// ����
	unsigned long	m_dwGold;			// �ݰ� �ܾ�
};

// ��� ����
struct PktGuildRelation : public PktBase
{
	enum eSubCmd
	{
		GR_HOSTILITY			= 0,	// ���� ����
		GR_PEACE				= 1,	// ��ȭ ����
		GR_ALERT				= 2,	// ��� ���� ����
		GR_COUNTER				= 3,	// ī���� ���� ����
		GR_REQUEST				= 4,	// �̹� ������ ���� ��ȭ ������ ��� ���� ���� ����
										// ī���� ���� ������ ���� ����� ��ȭâ
		GR_HOSTILITYDEL			= 5		// A->B�� ���뼱���� ���� A->B���� ��ȭ������ �ص� B�� Target�� �����ϴ�
										// ���뼱�� ����Ʈ�� ������� �ʴ´�. B�� ���� 5�� Ŀ�ǵ�� �������߸� 
										// ��Ұ� �ȴ�.
	};

	enum eError
	{
		FAIL_NOT_MEMBER					= 2,	// �ش� ����� ����� �ƴ�
		FAIL_NOT_HOSTILITY_TIME			= 3,	// ���� ������ �Ҽ� ���� �ð�
		FAIL_NOT_ALERT_TIME				= 4,	// ��� ���� ������ �Ҽ� ���� �ð�
		FAIL_NOT_COUNTER_TIME			= 5,	// ī���� ���� ������ �Ҽ� ���� �ð�
		FAIL_NOT_MASTER					= 6,	// ����� �����Ͱ� �ƴ�
        FAIL_NOT_EXIST_GUILD			= 7,	// ��� ID�� 0 �̰ų� ��尡 �������� ����
		FAIL_SELF_GUILD					= 8,	// �ڽ��� ��忡�� ����/��ȭ/���/ī���� ����
		FAIL_ALREADY_HOSTILITY			= 9,	// ��� ���� �̹� ���� �����Դϴ�.
		FAIL_ALREADY_COUNTER_HOSTILITY	= 10,	// ��� ���� �̹� ī���� ���� �����Դϴ�.
		FAIL_ALREADY_ALERT_HOSTILITY	= 11,	// ���� ���� �̹� ��� ���� �����Դϴ�.
		FAIL_ALREADY_NEUTRALITY			= 12,	// ��� ���� �̹� �߸� �����Դϴ�.
		FAIL_MAX_ALERT_NUM				= 13,	// ��� ���� ����� �ʰ�
		FAIL_NOT_HAVE_DEVELOPING_CAMP	= 14,	// �������� ��� ����� ����
		FAIL_NOT_HAVE_ALERT				= 15,	// ��� ���� ��û�� ���� �ʰ� ī���� ���� ��û�� �Ϸ���
		FAIL_TARGET_HOSTILITY_FULL		= 16,	// ���� ���� ���� �� �ִ� �Ѱ�ġ �ʰ�

		FAIL_REQUEST_WINDOW				= 100,	// ���� ����� �ʰ��� ���� ��ȭ ������ ���� ������ �ϵ��� ����
		FAIL_REQUEST_COUNTER			= 101,	// ī���� ���� ������ �ϵ��� ����
	};

	unsigned long	m_dwCID;			// ĳ���� ���̵�
	unsigned long	m_dwGID;			// �ڽ��� GID
	unsigned long	m_dwTargetGID;		// ��� ��� ���̵�
	unsigned long	m_dwValue;			// ���� �������� GID ( GR_REQUEST �϶���... )
    char			m_cSubCmd;			// ��� ���� ���� Ŀ�ǵ�

	char			m_szGuildName[MAX_GUILD_NAME_LEN];			// ��� ���� / ���� ����
	char			m_szTempGuildName[MAX_GUILD_NAME_LEN];		// �̹� ���� �������� ��� �̸�
};

// ��� ����
struct PktGuildInclination : public PktBase
{
	enum PktGIErr
	{
		FAIL_SO_FAST				= 2,	// ���� �ð������� ������ �� ����.
		FAIL_WRONG_NATION_KARTERANT	= 3,	// ī���׶�Ʈ ��忡�� �ΰ��� ���� �� �ִ�.
		FAIL_WRONG_NATION_MERKADIA	= 4		// �޸�ī��� ��忡�� ��ĭ�� ���� �� �ִ�.
	};

	unsigned long	m_dwUID;			// ���̵� (C -> S : ĳ���� ���̵�, S -> C : ��� ���̵�)
	unsigned char	m_cInclination;		// ���� (Guild::Inclination ����)
};

// ��� ���� ����
struct GuildRight
{
	enum Index
	{
		PUT_STOREHOUSE		= 0,
		GET_STOREHOUSE		= 1,
		USE_SAFE			= 2,
		INVITE_MEMBER		= 3,
		PERMIT_JOIN			= 4,
		CHANGE_PASSWORD		= 5,

		REGULATE_TEX		= 6,
		KICK_MEMBER			= 7,
		BOARD_ADMIN			= 8,

		SETUP_MARK_N_LEVEL	= 9,
		SETUP_MIDDLE		= 10,
		DIVIDE_GOLD			= 11,
		SETUP_POLICY		= 12,

		MAX_USING_RIGHT		= 13
	};

	unsigned char m_aryRight[MAX_RIGHT_SIZE];

	GuildRight()
	{
		Init(false);
	}

	GuildRight(bool bMaxRight)
	{ 
		Init(bMaxRight);
	}

	void Init(bool bMaxRight)
	{
		std::fill_n(m_aryRight, int(MAX_RIGHT_SIZE), Guild::NONE);

		m_aryRight[CHANGE_PASSWORD] = (true == bMaxRight) ? Guild::COMMON : Guild::MASTER;
		m_aryRight[PUT_STOREHOUSE] = (true == bMaxRight) ? Guild::COMMON : Guild::MASTER;		
		m_aryRight[GET_STOREHOUSE] = (true == bMaxRight) ? Guild::COMMON : Guild::MASTER;
		m_aryRight[USE_SAFE] = (true == bMaxRight) ? Guild::COMMON : Guild::MASTER;
		m_aryRight[INVITE_MEMBER] = (true == bMaxRight) ? Guild::COMMON : Guild::MASTER;	
		m_aryRight[PERMIT_JOIN] = (true == bMaxRight) ? Guild::COMMON : Guild::MASTER;

		m_aryRight[REGULATE_TEX] = (true == bMaxRight) ? Guild::MIDDLE_ADMIN : Guild::MASTER;	
		m_aryRight[KICK_MEMBER] = (true == bMaxRight) ? Guild::MIDDLE_ADMIN : Guild::MASTER;
		m_aryRight[BOARD_ADMIN] = (true == bMaxRight) ? Guild::MIDDLE_ADMIN : Guild::MASTER;

		m_aryRight[SETUP_MARK_N_LEVEL] = (true == bMaxRight) ? Guild::MASTER : Guild::MASTER;
		m_aryRight[SETUP_MIDDLE] = (true == bMaxRight) ? Guild::MASTER : Guild::MASTER;
		m_aryRight[DIVIDE_GOLD] = (true == bMaxRight) ? Guild::MASTER : Guild::MASTER;
		m_aryRight[SETUP_POLICY] = (true == bMaxRight) ? Guild::MASTER : Guild::MASTER;
	}

	int IsValid(void)
	{
		GuildRight MaxRight(true);

		int nIndex = 0;
		for (; nIndex < MAX_RIGHT_SIZE; ++nIndex)
		{
			if (m_aryRight[nIndex] > MaxRight.m_aryRight[nIndex])
			{
				return nIndex;
			}
		}

		return nIndex;
	}
};

// ��� ����
struct PktGuildRight : public PktBase
{
	unsigned long	m_dwUID;			// ���̵� (C -> S : ĳ���� ���̵�, S -> C : ��� ���̵�)
	GuildRight		m_GuildRight;		// ��� ����
};

// �ڱ� ��� ����
struct PktMyGuildInfo : public PktBase
{
	unsigned long	m_dwGold;			// ��� �ݰ�

	GuildRight		m_GuildRight;		// ��� ����
	unsigned char	m_cTitle;			// ����
};

// ���� ��Ȳ ����Ʈ (���� ����) + ���� ��Ȳ ���� 
struct PktGuildHostilityList : public PktBase
{
	enum { MAX_NUM_PER_PAGE = 5 };

	enum PageState
	{
		PAGE_CURRENT	= 0,
		PAGE_PREV		= 1,
		PAGE_NEXT		= 2
	};

	unsigned long	m_dwCID;			// ĳ���� ���̵�

	unsigned char	m_cCurrentPage;		// ���� ������
	unsigned char	m_cPageState;		// ��û�� �������� ����
	unsigned char	m_cNodeNum;			// ���� ����
	unsigned short	m_wTotalItemNum;	// ���� ��Ȳ�� �� ����Ʈ ��

	char	m_szHostility[MAX_GUILD_NAME_LEN];	// ���� ���� ���� ����
	char	m_szAlert[MAX_GUILD_NAME_LEN];		// ��� ���� �������� ����
};

// ��� ���� ��Ȳ ����
struct GuildHostilityInfoNode
{
	char	m_szName[MAX_GUILD_NAME_LEN];	// ���� ���� ��� �̸�
	unsigned char m_cRelationType;			// ���� ���� Ÿ�� (Guild::Relation)
};

// �ڽ��� ����� ���� ����Ʈ ��û ��Ŷ (���� ����) + ���� ����
struct PktGuildRelationInfo : public PktBase
{
	unsigned long	m_dwCID;			// ��û ĳ����
	unsigned long	m_dwGID;			// �ڽ��� ��� ID
	unsigned short	m_wNodeNum;			// ���� ����
};

// ��� ���� ����
struct GuildRelationInfoNode
{
	// GuildLargeInfo �� ����� ����
	unsigned long	m_dwGID;
	unsigned char	m_cRelationByMine;
	unsigned char	m_cRelationByTarget;
};

// ��� �ݰ�
struct PktGuildSafe : public PktBase
{
	enum PktGuildSafeErr
	{
		FAIL_NOT_ENOUGH_MONEY	= 2,		// �ݰ� ���� ���ġ ����
		FAIL_NOT_CHAR_MONEY     = 3,		// ĳ���Ϳ� ���� ���ġ ����
		FAIL_NOT_GUILD_DEPOSIT  = 4,		// �ݰ� �Ա��ϴµ� �����߽��ϴ�.
		FAIL_NOT_GUILD_UPDATE	= 5,		// �ݰ� ������ ������Ʈ �ϴµ� �����߽��ϴ�.
		FAIL_REQUEST_DATA	    = 6,		// ������ ��û���Դϴ�.
	};

	enum SafeCmd
	{
		SC_WITHDRAWAL	= 0,			// ���
		SC_DEPOSIT		= 1,			// �Ա�
		SC_RELEASE		= 2,			// �������� ����
		SC_REFER		= 3,			// ��� ���� ��ȸ
		SC_CAMPSHOP		= 4,			// ��� ��� ������ ���ͱ�
		SC_BONUS		= 5,			// ���ʽ� �Ա�
	};

	unsigned long	m_dwCID;			// ĳ���� ���̵�
	unsigned long	m_dwGold;			// �Ա�/���/�����ϴ� �ݾ�
	unsigned char	m_cCmd;				// ���
};

// ��� �ݰ� Ack
struct PktGuildSafeAck : public PktBase
{
	unsigned long	m_dwCID;			// ĳ���� ���̵�
	unsigned long	m_dwGID;			// ��� ���̵�

	unsigned long	m_dwSafeGold;		// �ݰ� �ܾ�
	unsigned long	m_dwCharGold;		// ĳ���� �ܾ� (C -> S : �Ա�/���/�����ϴ� �ݾ�)

	unsigned char	m_cCmd;				// ���

	char			m_szCharName[Guild::MAX_MEMBER_NAME_LEN];		// ĳ���� �̸�
};

// ��� DB (���� ����) + ��� ����
struct PktGuildDB : public PktBase
{
	bool			m_bStartFlag;			// ���� ��Ŷ�ΰ�? (PktGuildDB�� ��� ����ŭ �����Ƿ� ù��° ��带 �޾��� �� ����Ʈ�� �ʱ�ȭ ���־�� �Ѵ�.)

	unsigned long	m_dwGID;				// ��� ���̵�

	unsigned char	m_cTotalMemberNum;		// ���� ��
	unsigned char	m_cRelationByMineNum;	// ���� ���� ��� �����
	unsigned char	m_cRelationByTargetNum;	// �ٸ� ��尡 ���� ��� �����

	unsigned short	m_wSize;				// ��� ���� ũ��
};

// ��� ���� (DB)
struct GuildInfoDB
{
	unsigned long	m_dwGID;
	char			m_strName[MAX_GUILD_NAME_FOR_DB];
	unsigned char	m_cLevel;
	unsigned long	m_dwFame;
	unsigned long	m_dwGold;
	TIME			m_tmLastLogout;
	TIME			m_tmCheckMember;
	TIME			m_tmGMLastLogout;
	char			m_szMark[MAX_MARK_SIZE];
	char			m_szRight[MAX_RIGHT_SIZE];
	unsigned char	m_cInclination;
	TIME			m_tmChangeInclination;
};

// ��� ��� (DB)
struct GuildMemberDB
{
	unsigned long	m_dwGID;
	unsigned long	m_dwCID;
	unsigned long	m_dwRank;
	char			m_strName[MAX_MEMBER_NAME_LEN];
	unsigned long	m_dwTitle;
	unsigned char	m_cLevel;
	unsigned short	m_wClass;
	unsigned long	m_dwFame;
	unsigned long	m_dwGold;
	unsigned char	m_cGuildWarFlag;
	TIME			m_LeaveGuildTime;

	unsigned char	m_cTactics;
};

// ��� ���� (DB)
struct GuildRelationDB
{
	unsigned long	m_dwGID;
	unsigned long	m_dwTargetGID;
	unsigned char	m_cRelation;
};

// ��� ������ ���� ��Ŷ.
struct PktGuildPosition : public PktBase
{
	enum PktErr
	{
		FAIL_GUILD			= 1,		// ��尡 ������.
		FAIL_CONNECT_CHAR	= 2,		// ĳ���Ͱ� ���� ���϶�.
		FAIL_DB_UPDATE		= 3,		// DB ������Ʈ ����.
		FAIL_MEMBER_DELETE	= 4			// ��� ��� ���� ����.
	};

	enum Const
	{
		TYPE_POSITION		= 0,		// ������ �����.
		TYPE_MEMBERDELETE	= 1			// ��� ��� ������.
	};

	unsigned long	m_dwGID;
	unsigned long	m_dwCID;
	unsigned char	m_cPosition;
	unsigned char	m_cType;
};

// ��ġ�� ��� �ӽ÷�(���̽�ƽ Ű ���� ����).
struct PktKeyInfo : public PktBase
{	
	enum Const
	{
		MAX_KEY_INFO	= 34	
	};

	unsigned long	m_dwCID;
	unsigned long	m_dwKeyInfo[MAX_KEY_INFO];
};

#pragma pack()

#endif