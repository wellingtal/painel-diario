///////////////////////////////////////////////////////////////////////////////////////////////
//
// DB Struct Define
//
//////////////////////////////////////////////////////////////////////////////////////////////
#ifndef _DBDefine
#define _DBDefine

#include <algorithm>

#pragma pack(1)	// 1byte order�� ��ŷ

///////////////////////////////////////////////////////////////////////////////////////////////
//
// ĳ���� ���� DB Table
//
//////////////////////////////////////////////////////////////////////////////////////////////
struct CHAR_INFOST
{
    enum 
    { 
		// edith 2008.03.17 ID,PASS ��������
        MAX_ACCOUNT_LEN = 24,
        MAX_NAME_LEN    = 16 
    };

    unsigned long		CID;

    char				Name[MAX_NAME_LEN];	// ĳ���� �̸�
    char				Sex;				// ĳ���� ��
    char				Hair;				// ĳ���� �Ӹ� ���
    char 				Face;				// ĳ���� �� ���
    char				Race;				// ĳ���� ����
    unsigned short		Class;				// ĳ���� Ŭ����

    unsigned long		Fame;				// ��		
    unsigned long		Mileage;			// ���ϸ���			
    unsigned long		GID;				// ĳ���� ���
    unsigned long		PID;				// ĳ���� ��Ƽ
    char				Level;				// ĳ���� ����

    unsigned long		Gold;				// �� 

    unsigned short		IP;					// ���� ����Ʈ
    unsigned short		STR;				// ĳ���� STR
    unsigned short		DEX;				// ĳ���� DEX
    unsigned short		CON;				// ĳ���� CON
    unsigned short		INT;				// ĳ���� INT
    unsigned short		WIS;				// ĳ���� WIS

    short				HP;					// ĳ���� HP
    short				MP;					// ĳ���� MP	
    __int64				Exp;				// ĳ���� ������

    char				Chance;				// ĳ���� ����
};

typedef CHAR_INFOST* LPCHAR_INFOST;

///////////////////////////////////////////////////////////////////////////////////////////////
//
// �ð�
//
//////////////////////////////////////////////////////////////////////////////////////////////
struct TIME
{
	unsigned short	Year;					// ����
	unsigned short	Month;					// �� 
	unsigned short	Day;					// ��¥
	unsigned short	Hour;					// �ð�
	unsigned short	Minute;					// ��
	unsigned short	Second;					// ��
	unsigned long	MSecond;				// �и���

};
typedef TIME* LPTIME;

///////////////////////////////////////////////////////////////////////////////////////////////
//
// ��÷��� ����
//
//////////////////////////////////////////////////////////////////////////////////////////////
struct AUTH
{
	unsigned long			Errcode;			// ���� �ڵ�
	unsigned long			UID;				// ���� ���̵�
	union COMMENT
	{
		unsigned long		Point;				// ����Ʈ ��
		TIME				Time;				// �ð�
	} Comment;

	unsigned long			Dumy[4];			// �԰ݿ� ���� ���� �������� ������ ���
};

struct PAY_AUTH
{
	// edith 2008.05.29 ��ī��Ʈ ���̰� 20->40���� �ٲ���.
	enum { AccountLen = 40 };

	unsigned long			Errcode;			    // ���� �ڵ�
	char					Account[AccountLen];    // ���� �̸�
	unsigned long			UID;				    // ���� ���̵�
	unsigned long			Time;				    // �ð� (��)
    int                     WarningCode;            // 0 : �븸 ����� / 1 : �߱� ������ / 2 : �߱� �����
};

///////////////////////////////////////////////////////////////////////////////////////////////
//
// �Ѱ��� ����
//
//////////////////////////////////////////////////////////////////////////////////////////////
typedef struct _HAN_UID
{
	unsigned long			hUID;				// ���� ���̵�
	unsigned long			lUID;				// ���� ���̵�
}HAN_UID, *LPHAN_UID;

typedef struct _HAN_AUTH
{
	unsigned char			Num;				// �ڸ���
	unsigned short			Extra;				// ���ϴ� ���� ��
	unsigned long			UID;				// ���� ���̵�
	unsigned char			Dump;				// �����
}HAN_AUTH, *LPHAN_AUTH;

typedef union _HAN_CONVERT
{
	unsigned _int64			iValue;				// 64 Bit ��
	HAN_AUTH				sValue;				// ����ü
}HAN_CONVERT, *LPHAN_CONVERT;

///////////////////////////////////////////////////////////////////////////////////////////////
//
// ��ġ
//
//////////////////////////////////////////////////////////////////////////////////////////////
struct POS
{
	float			fPointX;				// Point X ��ǥ
	float			fPointY;				// Point Y ��ǥ
	float			fPointZ;				// Point Z ��ǥ
};

///////////////////////////////////////////////////////////////////////////////////////////////
//
// ��ų ����	// modified 2002/10/3 11:20 by sparrowhawk
//
//////////////////////////////////////////////////////////////////////////////////////////////
union SKILLSLOT
{
	struct 
	{
		unsigned short	wSkill;				// ��ų ���̵�
		char			cLockCount;			// ��ų �� ī��Ʈ	( 0~4 )
		char			cSkillLevel;		// ��ų ����		( -127 ~ 128 )
	}SKILLINFO;
	
	unsigned long		dwSkillSlot;

	SKILLSLOT() : dwSkillSlot(0) { }
	SKILLSLOT(unsigned short skill, char lockcount, char skillLevel)
		{ SKILLINFO.wSkill = skill; SKILLINFO.cLockCount = lockcount; SKILLINFO.cSkillLevel = skillLevel; }
};
typedef SKILLSLOT* LPSKILLSLOT;

///////////////////////////////////////////////////////////////////////////////////////////////
//
// ��ų			
//
//////////////////////////////////////////////////////////////////////////////////////////////
struct SKILL
{
    enum Const
    { 
        MAX_SKILL_LEVEL	= 6,
        MAX_SLOT_NUM	= 40 
    };

    unsigned short	wSkillNum;				// ��ų �� (TODO : ������� �ʽ��ϴ�.)
    unsigned short	wSlotNum;				// ���� ��

    SKILLSLOT		SSlot[MAX_SLOT_NUM];	// ��ų

    SKILL() : wSkillNum(0), wSlotNum(0) 
    { 
        std::fill_n(&SSlot[0], int(MAX_SLOT_NUM), SKILLSLOT());
    }

    SKILL(unsigned short usSkillNum, unsigned short usSlotNum)
        : wSkillNum(usSkillNum), wSlotNum(usSlotNum) 
    { 
        std::fill_n(&SSlot[0], int(MAX_SLOT_NUM), SKILLSLOT());
    }

    unsigned short GetSkillNum(void)
    {
        unsigned short wCount = 0;
        for (int Index = 0; Index < wSlotNum; ++Index)
        {
			// ��ų���̵� 0x8000 ���� ������ �Ҽ����, �����Ƽ �̴�.
			if(SSlot[Index].SKILLINFO.wSkill < 0x8000)
				continue;

			//if((SSlot[Index].SKILLINFO.wSkill == 0x9E04	)||(SSlot[Index].SKILLINFO.wSkill == 0x9E03	))//hz wasnt commented out
			//	continue;

            wCount += (SSlot[Index].SKILLINFO.cLockCount * MAX_SKILL_LEVEL + 
                SSlot[Index].SKILLINFO.cSkillLevel);
			
        }

        return wCount;
    }

    unsigned short GetAbilityNum(void)
    {
        unsigned short wCount = 0;
        for (int Index = 0; Index < wSlotNum; ++Index)
        {
			if(SSlot[Index].SKILLINFO.wSkill >= 0x5000)
				continue;
			
			wCount += (SSlot[Index].SKILLINFO.cLockCount + 1);
				//wCount += SSlot[Index].SKILLINFO.cSkillLevel;
        }

        return wCount;
    }

};
typedef SKILL* LPSKILL;


///////////////////////////////////////////////////////////////////////////////////////////////
//
// Ÿ��Ʋ
//
//////////////////////////////////////////////////////////////////////////////////////////////
typedef struct _TITLE
{
	__int64			dlTitle;				// Ÿ��Ʋ ���̵�
	unsigned short	wScore;					// Ÿ��Ʋ ����
}TITLE, *LPTITLE;

///////////////////////////////////////////////////////////////////////////////////////////////
//
// ���
//
//////////////////////////////////////////////////////////////////////////////////////////////
typedef struct _EQUIP
{
	enum { MAX_EQUIP_SIZE = 1436 }; // + 4 = 1440

	unsigned long	dwSize;
	char			Data[MAX_EQUIP_SIZE];
} EQUIP, *LPEQUIP;

///////////////////////////////////////////////////////////////////////////////////////////////
//
// �κ�
//
//////////////////////////////////////////////////////////////////////////////////////////////
typedef struct _INVEN
{
	enum { MAX_INVEN_SIZE = 5116 }; // + 4 = 5120

	unsigned long	dwSize;
	char			Data[MAX_INVEN_SIZE];
} INVEN, *LPINVEN;

///////////////////////////////////////////////////////////////////////////////////////////////
//
// Extra
//
//////////////////////////////////////////////////////////////////////////////////////////////
typedef struct _EXTRA
{
	enum { MAX_EXTRA_SIZE = 508 }; // + 4 = 512

	unsigned long	dwSize;
	char			Data[MAX_EXTRA_SIZE];
} EXTRA, *LPEXTRA;

///////////////////////////////////////////////////////////////////////////////////////////////
//
// Exchange
//
//////////////////////////////////////////////////////////////////////////////////////////////
typedef struct _EXCHANGE
{
	enum { MAX_EXCHANGE_SIZE = 1280 }; // + 4 = 1284

	unsigned long	dwSize;
	char			Data[MAX_EXCHANGE_SIZE];
} EXCHANGE, *LPEXCHANGE;

///////////////////////////////////////////////////////////////////////////////////////////////
//
// �ӽ� �κ��丮
//
//////////////////////////////////////////////////////////////////////////////////////////////
struct TEMPINVEN
{
	enum { MAX_TEMPINVEN_SIZE = 1436 }; // + 4 = 1440

	unsigned long	dwSize;
	char			Data[MAX_TEMPINVEN_SIZE];
};

///////////////////////////////////////////////////////////////////////////////////////////////
//
// â��
//
//////////////////////////////////////////////////////////////////////////////////////////////
typedef struct _STORE
{
	enum { MAX_STORE_SIZE = 7296 }; // + 4 = 7300

	unsigned long	dwSize;
	char			Data[MAX_STORE_SIZE];
} STORE, *LPSTORE;

///////////////////////////////////////////////////////////////////////////////////////////////
//
// �� ����
//
//////////////////////////////////////////////////////////////////////////////////////////////
struct QUICKSLOT {

	enum Type { NONE = 0, SKILL = 1, ITEM = 2 };
	char nType;
	char nSkillLockCount;
	char nSkillLevel;
	unsigned short wID;			// Skill_ID �Ǵ� ������ ProtoType ID
	
	QUICKSLOT() : nType(NONE), nSkillLockCount(0), nSkillLevel(0), wID(0) { }
	QUICKSLOT(char type, char lockCount, char skillLevel, unsigned short wid)
		:	nType(type), nSkillLockCount(lockCount), nSkillLevel(skillLevel), wID(wid) { }
};
typedef QUICKSLOT* LPQUICKSLOT;

struct QUICK
{
	enum { MAX_QUICK_NUM = 20 };
	QUICKSLOT Slots[MAX_QUICK_NUM];

	QUICK()
	{
		std::fill_n(&Slots[0], int(MAX_QUICK_NUM), QUICKSLOT());
	}

};
typedef QUICK *LPQUICK;

///////////////////////////////////////////////////////////////////////////////////////////////
//
// ĳ���� ���� ����ü
//
//////////////////////////////////////////////////////////////////////////////////////////////
struct CHAR_CREATE
{
	enum RaceType
	{ 
		HUMAN	= 0,
		AKHAN	= 1
	};

    char				Name[CHAR_INFOST::MAX_NAME_LEN];	// ĳ���� �̸�
	char				Sex;				// ĳ���� ��
	char				Hair;				// ĳ���� �Ӹ� ���
	char 				Face;				// ĳ���� �� ���
	char				Race;				// ĳ���� ����
	unsigned short		Class;				// ĳ���� Ŭ����

	unsigned short		Equip[15];			// ���

	unsigned short		STR;				// ĳ���� STR
	unsigned short		DEX;				// ĳ���� DEX
	unsigned short		CON;				// ĳ���� CON
	unsigned short		INT;				// ĳ���� INT
	unsigned short		WIS;				// ĳ���� WIS
};

///////////////////////////////////////////////////////////////////////////////////////////////
//
// ĳ���� ��� ����ü
//
//////////////////////////////////////////////////////////////////////////////////////////////
struct CHAR_VIEW
{
	enum RaceType
	{ 
		HUMAN	= 0,
		AKHAN	= 1
	};

	unsigned long		CID;

	char				Name[CHAR_INFOST::MAX_NAME_LEN];	// ĳ���� �̸�
	char				Sex;				// ĳ���� ��
	char				Hair;				// ĳ���� �Ӹ� ���
	char 				Face;				// ĳ���� �� ���
	char				Race;				// ĳ���� ����
	unsigned short		Class;				// ĳ���� Ŭ����

	unsigned long		Fame;				// ��		
	unsigned long		Mileage;			// ���ϸ���			
	unsigned long		GID;				// ĳ���� ���
	unsigned long		PID;				// ĳ���� ��Ƽ
	char				Level;				// ĳ���� ����

	unsigned short		Equip[15];			// ���(�Ѻ���)
};

// DB �� ���������� ������ CHAR_VIEW �� ���õ� �߰� �����̴�.
// ��� �� ���� ����
struct sGuildData
{
	unsigned char	m_cNation;
	char			m_szName[CHAR_INFOST::MAX_NAME_LEN];
};

typedef struct _PREMIUMSERVICE
{
	long			lPremiumTime;
	int				iPremiumType;
}	PREMIUMSERVICE, *LPPREMIUMSERVICE;


///////////////////////////////////////////////////////////////////////////////////////////////
//
// ĳ���� ���� DB Table
//
//////////////////////////////////////////////////////////////////////////////////////////////
struct USER_INFO
{
    enum
    {
        MAX_CHAR_NUM = 5
    };
    
    unsigned long       CharID[MAX_CHAR_NUM];   // ĳ���� 

    // WORK_LIST 2.1 ���� ���� �߰�
    unsigned char		Nation;                 // ���� ����

    // �������� - by sphawk
    unsigned char       OldServerGroupID;       // ������ �����׷�ID (0�̸� ���� �������� �������� ��Ÿ��)
    unsigned char       FirstLogin;             

    bool HasCharacter(unsigned long dwCID) const 
    {
        for(int nCount = 0; nCount < MAX_CHAR_NUM; ++nCount)
        {
            if(CharID[nCount] == dwCID) { return true; }
        }

        return false;
    }

    unsigned char GetCharacterNum() const
    {
        unsigned char cSelectedCharacter = 0;
        for(int nCount = 0; nCount < MAX_CHAR_NUM; ++nCount)
        {
            if(0 != CharID[nCount]) { ++cSelectedCharacter; }
        }

        return cSelectedCharacter;
    }

};
typedef USER_INFO *LPUSER_INFO;



///////////////////////////////////////////////////////////////////////////////////////////////
//
// ĳ���� �߰� ����
//
//////////////////////////////////////////////////////////////////////////////////////////////
typedef struct _CHAR_INFOEX
{
	unsigned long		Total;				// �� �̿� �ð�(��)
	unsigned long		ServerID;			// ��
    unsigned char       cNameChangeCount;   // �̸� �ٲ� �� �ִ� Ƚ��
	unsigned char		cAdminFlag;			// Admin �� ����.
	unsigned char		GuildWarFlag;		// ��� ���� ���� �÷���
	unsigned char		RealmWarFlag;		// ���� ���� ���� �÷���

	unsigned char		RealmPoint;			// ���� ���� �������� ����Ʈ.

	unsigned char		RealmCheckPoint;	// ���� ���� �߿� �α׾ƿ��ؼ� ����Ʈ�� ����� ���.

	unsigned char		RealmMinute;		// ���� ���� �÷��� Ÿ�� ���.
	
	TIME				LoggoutTime;		// �α׾ƿ��� �ð�.

}	CHAR_INFOEX, *LPCHAR_INFOEX;

///////////////////////////////////////////////////////////////////////////////////////////////
//
// ĳ���� ��ġ ���� DB Table
//
//////////////////////////////////////////////////////////////////////////////////////////////
typedef struct _CHAR_POS
{
	POS	LastPoint;							// ������ ��ġ
	POS	SavePoint;							// ���̺� ��ġ
}	CHAR_POS, *LPCHAR_POS;

///////////////////////////////////////////////////////////////////////////////////////////////
//
// ��Ƽ ���� DB Table
//
//////////////////////////////////////////////////////////////////////////////////////////////
typedef struct _PARTY_INFO
{
	enum { MAX_MEM = 10 };//hayzohar was 10 // avalansa was 12 by hayzohar

	unsigned long	MemberCID[MAX_MEM];							// ��� CID
	char			Name[MAX_MEM][CHAR_INFOST::MAX_NAME_LEN];	// ��� �̸� 	

}	PARTY_INFO, *LPPARTY_INFO;

typedef struct _PARTY_USER_INFO
{
	enum { MAX_MEM = 10 };//hayzohar was 10  // avalansa was 12 by hayzohar

	unsigned short	m_wClass[MAX_MEM];							// ��� Ŭ���� 
	char			m_cLevel[MAX_MEM];							// ��� ����
	unsigned long	m_dwGID[MAX_MEM];							// ��� ��� ���̵�

}	PARTY_USER_INFO, *LPPARTY_USER_INFO;

typedef struct _PARTY_DB_INFO									// ��񿡼� ���ö� ���.
{
	PARTY_INFO			PartyInfo;
	PARTY_USER_INFO		PartyUserInfo;

} PARTY_DB_INFO, *LPPARTY_DB_INFO;
///////////////////////////////////////////////////////////////////////////////////////////////
//
// ��Ƽ
//
//////////////////////////////////////////////////////////////////////////////////////////////
struct PARTY
{
	enum { MAX_MEM = 10 };//hayzohar was 10 // avalansa was 12 by hayzohar

	unsigned long	m_dwPartyID;
	unsigned long	m_dwLeaderID;	

	unsigned char	m_cMemberNum;

	char			Name[MAX_MEM][CHAR_INFOST::MAX_NAME_LEN];
	unsigned long	MemberCID[MAX_MEM];			
	unsigned long	ServerID[MAX_MEM];

	unsigned short	m_wClass[MAX_MEM];					// ��� Ŭ���� 
	char			m_cLevel[MAX_MEM];					// ��� ����
	unsigned long	m_dwGID[MAX_MEM];					// ��� ��� ���̵�

	PARTY() : m_dwPartyID(0), m_dwLeaderID(0), m_cMemberNum(0)
	{
		std::fill_n(&MemberCID[0], int(MAX_MEM), 0);
		std::fill_n(&ServerID[0], int(MAX_MEM), 0);

		std::fill_n(&m_wClass[0], int(MAX_MEM), 0);
		std::fill_n(&m_cLevel[0], int(MAX_MEM), 0);	
		std::fill_n(&m_dwGID[0], int(MAX_MEM), 0);	
	}
};

struct PARTY_USERINFO
{
	unsigned long	m_dwGID[PARTY::MAX_MEM];
};

// ��Ƽ ����(������� �߰�) //

struct PARTY_EX : public PARTY
{		
    bool			m_bAutoRouting[PARTY::MAX_MEM];
	POS				m_Position[PARTY::MAX_MEM];

	PARTY_EX()
	{				
        InitMemberAutoRouting();
        InitMemberPos();
    }

	PARTY_EX(const PARTY& party)
	{
		m_dwPartyID		= party.m_dwPartyID;
		m_dwLeaderID	= party.m_dwLeaderID;
		m_cMemberNum	= party.m_cMemberNum;		

		for (int nIndex = 0; nIndex < PARTY::MAX_MEM; ++nIndex)
		{
			::memcpy(Name[nIndex], party.Name[nIndex], CHAR_INFOST::MAX_NAME_LEN);
			
			MemberCID[nIndex]	= party.MemberCID[nIndex];
			ServerID[nIndex]	= party.ServerID[nIndex];
			m_wClass[nIndex]	= party.m_wClass[nIndex];
			m_cLevel[nIndex]	= party.m_cLevel[nIndex];
			m_dwGID[nIndex]		= party.m_dwGID[nIndex];
		}

        InitMemberAutoRouting();
        InitMemberPos();
	}

    inline void InitMemberAutoRouting()
    {
        std::fill_n(&m_bAutoRouting[0], int(PARTY::MAX_MEM), false);
    }

    inline void InitMemberPos()
    {
		for (int i = 0; i < PARTY::MAX_MEM; ++i)
		{
			m_Position[i].fPointX = 0.0f;
			m_Position[i].fPointY = 0.0f;
			m_Position[i].fPointZ = 0.0f;
		}
    }

};

///////////////////////////////////////////////////////////////////////////////////////////////
//
// â��
//
//////////////////////////////////////////////////////////////////////////////////////////////
typedef struct _STORE_INFO
{
	enum { MAX_PASS_LEN = 5 };

	char			Password[MAX_PASS_LEN];			// �н����� 
	unsigned long	Flag;							// �÷���
	unsigned long	Gold;							// �÷���

}	STORE_INFO, *LPSTORE_INFO;

///////////////////////////////////////////////////////////////////////////////////////////////
//
// ���
//
//////////////////////////////////////////////////////////////////////////////////////////////
typedef struct _ADMIN
{
	enum	{	MAX_IP_LEN = 16	};

	unsigned long	UID;
	unsigned short	Level;
	char			IP_S[MAX_IP_LEN];
	char			IP_E[MAX_IP_LEN];
}	ADMIN, *LPADMIN;

///////////////////////////////////////////////////////////////////////////////////////////////
//
// ������
//
//////////////////////////////////////////////////////////////////////////////////////////////
typedef struct _FRIEND
{
	enum
	{ 
		MAX_FRIEND_SIZE		= 2400,		// + 4 = 2404
		MAX_FRIENDINFO_SIZE	= 700,		// + 4 = 704 (ĳ���� ���� �߰�.)
	};

	unsigned long	dwSize;
	char			Data[MAX_FRIEND_SIZE];	

	unsigned long	dwInfoSize;						//
	char			Info[MAX_FRIENDINFO_SIZE];		

} FRIEND, *LPFRIEND;

typedef struct _BAN
{
	enum
	{ 
		MAX_BAN_SIZE		= 2000,			// + 4 = 2004
		MAX_BANINFO_SIZE	= 700,			// + 4 = 704 (ĳ���� ���� �߰�.)
	};	

	unsigned long	dwSize;
	char			Data[MAX_BAN_SIZE];

	unsigned long	dwInfoSize;
	char			Info[MAX_BANINFO_SIZE];

} BAN, *LPBAN;

///////////////////////////////////////////////////////////////////////////////////////////////
//
// ���� ����
//
//////////////////////////////////////////////////////////////////////////////////////////////
typedef struct _RE_USPCheckBilling_Login
{
	unsigned long   Flag;						// Success(=0)
	char            strBillingType[2];			// D, T(N: �������)
	int             PlayTime;					// ���� ���� �ð� (��, ��)
	unsigned long   intCRMIndex1;				// �ǽù� �������̵�
} RE_USPCheckBilling_Login, *LPRE_USPCheckBilling_Login;

typedef struct _RE_USPGRoomCurrent_DisConn
{
	enum	{ ClientIDLen = 20 };

	unsigned char			ClientID[ClientIDLen];		// ����
	unsigned long			UID;						// ���� ���̵�
	char					ServerID;					// ���� ���̵�
}RE_USPGRoomCurrent_DisConn, *LPRE_USPGRoomCurrent_DisConn;


///////////////////////////////////////////////////////////////////////////////////////////////
//
// ���� ��� ����
//
//////////////////////////////////////////////////////////////////////////////////////////////
typedef struct _RE_CMS_CheckUserBlock
{
	unsigned char   m_cFlags;           //  0 : �� ����, 1 : ���� ��, 2 : ĳ���� ��  
    char            m_szCharName[20][USER_INFO::MAX_CHAR_NUM];  //  ���� ĳ���� �̸�

} RE_CMS_CheckUserBlock, *LPRE_CMS_CheckUserBlock;

///////////////////////////////////////////////////////////////////////////////////////////////
//
// ����Ʈ
//
//////////////////////////////////////////////////////////////////////////////////////////////
typedef struct _QUEST
{
	enum { MAX_QUEST_SIZE = 260 }; // + 4 = 264

	unsigned long	dwSize;
	char			Data[MAX_QUEST_SIZE];
} QUEST, *LPQUEST;

typedef struct _HISTORY
{
	enum { MAX_HISTORY_SIZE = 400 };	// + 4 = 404

	unsigned long	dwSize;
	char			Data[MAX_HISTORY_SIZE];
} HISTORY, *LPHISTORY;

///////////////////////////////////////////////////////////////////////////////////////////////
//
// ����
//
//////////////////////////////////////////////////////////////////////////////////////////////
typedef struct _CONFIG
{
	enum { MAX_CONFIG_SIZE = 46 }; // + 4 = 50

	unsigned long	dwSize;
	char			Data[MAX_CONFIG_SIZE];
} CONFIG, *LPCONFIG;


///////////////////////////////////////////////////////////////////////////////////////////////
//
// Ư�� �Ҽ���� 
//
//////////////////////////////////////////////////////////////////////////////////////////////
typedef struct _SOCIALEX
{
	enum { MAX_SOCIAL_SIZE = 20 }; // + 4 = 24

	unsigned long	dwSize;
	char			Data[MAX_SOCIAL_SIZE];
} SOCIALEX, *LPSOCIALEX;

///////////////////////////////////////////////////////////////////////////////////////////////
//
// �̺�Ʈ ������
//
//////////////////////////////////////////////////////////////////////////////////////////////
typedef struct _EVENTITEM
{
	enum { MAX_ITEM_NAME = 32 };

	unsigned long	nItemIndex;
	char			strItemName[MAX_ITEM_NAME];
	unsigned long	nItemAmount;
	TIME			UpdateTime;

} EVENTITEM, *LPEVENTITEM;


// �������� - ĳ���� ������ �Ķ���ͷ� DB�� ȣ��.
struct CHAR_CREATE_DB
{
    unsigned long   dwUID;
    unsigned long   dwSlotPos;
    char            szName[CHAR_INFOST::MAX_NAME_LEN];
    unsigned char   cSex;
    unsigned char   cHair;
    unsigned char   cFace;
    unsigned char   cRace;
    short           sClass;
    unsigned long   dwGold;
    short           sSTR;
    short           sDEX;
    short           sCON;
    short           sINT;
    short           sWIS;
    float           fPointX;
    float           fPointY;
    float           fPointZ;
    unsigned short  Equip[15];
    SKILL           skill;
    unsigned char   cServerGroupID;
    int             dwCID_Out;
};

// �������� - ���յ� ĳ���� ������ ���´�.
struct UnifiedCharData
{
    char            szName[16];
    char            szBeforeCharName[20];
    unsigned long   dwNewCID;
    unsigned long   dwBeforeCID;
    short           sClass;
    unsigned char   cLevel;
    unsigned char   cOldServerGroupID;
};

struct UnifiedStoreInfo
{
    unsigned char   cOldServerGroupID;     // �� �����׷�

    char			Password[STORE_INFO::MAX_PASS_LEN];	// �н����� 
    unsigned long	Flag;					            // �÷���
    unsigned long	Gold;					            // �ݾ�
};

struct KeyInfo
{
	enum Const
	{
		MAX_KEY_INFO	= 34
	};

	unsigned char	m_cUsed;

	unsigned long	m_dwKeyInfo[MAX_KEY_INFO];
};

///////////////////////////////////////////////////////////////////////////////////////////////
//
// ����
//
//////////////////////////////////////////////////////////////////////////////////////////////
struct SPELLINSTANCE {
	unsigned long	dwDurationSec;
	unsigned short	wSpellID;
	unsigned short	wEnchantLevel;
	unsigned char	cSpellType; 

	SPELLINSTANCE() : dwDurationSec(0), wSpellID(0), wEnchantLevel(0), cSpellType(0) { }
	SPELLINSTANCE(unsigned long dwDurationSec, unsigned short wSpellID, unsigned short wEnchantLevel, unsigned char cSpellType)
		:	dwDurationSec(dwDurationSec), wSpellID(wSpellID), wEnchantLevel(wEnchantLevel), cSpellType(cSpellType) { }
};
typedef SPELLINSTANCE* LPSPELLINSTANCE;

struct SPELL
{
	enum { MAX_SPELL_NUM = 30 };
	SPELLINSTANCE Spells[MAX_SPELL_NUM];

	SPELL()
	{
		std::fill_n(&Spells[0], int(MAX_SPELL_NUM), SPELLINSTANCE());
	}
};
typedef SPELL* LPSPELL;

#pragma pack()

#endif