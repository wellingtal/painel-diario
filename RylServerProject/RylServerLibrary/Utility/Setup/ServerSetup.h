#ifndef _CGAMESERVER_ADDRESS_H_
#define _CGAMESERVER_ADDRESS_H_

#include <winsock2.h>
#include <windows.h>

#include <vector>
#include <DB/OleDB.h>

#include <Network/Address/INET_Addr.h>
#include <Network/Packet/PacketStruct/UnifiedCharPacket.h>

#include <Utility/Setup/GlobalConstants.h>


class CServerSetup
{
public:

	static CServerSetup& GetInstance();

	enum DefaultPorts
    {   
        // Ruleset 
        // 10000 ~ 11000 port : �ܺ� ����
        // 11001 ~ 12000 port : �系���� ����
        // 12001 ~ 13000 port : ���� ������ ����θ� ��� (���γ����� ����)

        // Ŭ���̾�Ʈ -> ���� (�ܺ� -> ����, �����ϸ� �ȵ�(Ŭ���̾�Ʈ�� �ϵ��ڵ� �Ǿ� ����))
		LoginServerLauncherListen       = 10101,	// ��ó -> �α��μ���
        AuthServerClientListen          = 10210,    // Ŭ���̾�Ʈ -> ��������
        
        // Ŭ���̾�Ʈ -> ���� (�ܺ� -> ����, �����ص� ��� ����.. ��Ʈ2�� ��� ����)
        P1ChatServerClientListen        = 10111,

        // Ŭ���̾�Ʈ -> ���Ӽ��� (�ܺ� -> ����, ��Ģ�� ���� ������, ���� �������� �ٲ㵵 ������.)
        // ������ TCP��Ʈ. GameServerBaseTCPPort + ä�� ��ȣ(0~4) * 20 + �� ��ȣ(1~20)
        GameServerClientBaseListen      = 10400,
        MAX_ZONE                        = 20,
        MAX_CHANNEL                     = 5,

        // Ŭ���̾�Ʈ -> ���Ӽ��� (�ܺ� -> ����, ��Ģ�� ���� ������, ���� �������� �ٲ㵵 ������.)
        // ��Ʈ 1 �����̴�. ��Ʈ 2�� UDP�� ������� �ʴ´�.
        P1GameServerClientUDPListen     = 10500,

        // ���� ��Ʈ��, �ܺο����� ���� ip�� ���ؼ��� ���� �����ϵ��� �Ѵ�.
        // 11001 ~ 11100 ��Ʈ�� PCAnywhereȤ�� Remote Desktop���� ���� ���� ���ؼ� ��� �д�.
        P1AdminToolServerClientListen   = 11121,    // ��Ʈ 1 ��� Ŭ���̾�Ʈ -> ��Ʈ 1 ��� ����
        P2AdminToolServerClientListen   = 11122,    // ��Ʈ 2 ��� Ŭ���̾�Ʈ -> ��Ʈ 2 ��� ����
        LogServerLogClientListen        = 11123,    // �α� Ŭ���̾�Ʈ(�α� ����͸� ��) -> �α� ����
		ManageServerManageToolListen    = 11124,	// ���� �� -> ���� ����
        ChatServerMonitoringToolListen  = 11125,    // ä�� ����͸� �� -> ä�ü���
        StatServerManageServerListen    = 11126,    // ���� ���� -> ��� ����

        // �� ������ ���� (�ܺο��� ���� ���� ���ƾ� �Ѵ�, ���ΰ� ���Ḹ ����Ѵ�)
        UIDServerDBAgentServerListen    = 12531,	// DB�߰輭�� -> UID����
		LoginServerDBAgentServerListen  = 12532,	// DB�߰輭�� -> �α��μ���
		
        DBAgentServerGameServerListen   = 12533,	// ���Ӽ��� -> DB�߰輭��				
		DBAgentServerAuthServerListen   = 12534,	// �������� -> DB�߰輭��
        DBAgentServerChatServerListen   = 12535,    // ä�ü��� -> DB�߰輭��
        DBAgentAdminToolServerListen    = 12536,    // ��� ���� -> DB�߰輭��

        ChatServerGameServerListen      = 12537,	// ���Ӽ��� -> ä�ü���
        ManageServerManageClientListen  = 12538,	// ���� Ŭ���̾�Ʈ -> ���� ����
        LogServerOtherServerListen      = 12539     // �ٸ� ������ -> �α� ����
	};
	
	enum ServerType
    { 
        LoginServer     = 0,
        AuthServer      = 1,
        GameServer      = 2, 
        AgentServer     = 3, 
        UIDServer       = 4, 
        AdminToolServer = 5,
        LogServer       = 6,
        ChatServer      = 7,
        MaxType         = 8 
    };

    enum BillingType
    {
        ROWGlobalBilling    = 0,    // ROW��������
        GamaBilling         = 1,    // �ѱ� �Ϲݰ���(���� ���ݹ��)
        GamaUnitedBilling   = 2,    // �ѱ� ���հ���
        YouxiLandBilling    = 3,    // �븸�� ����
        JapanBilling		= 4,    // �Ϻ��� ����
        GammaniaBilling     = 5,    // �����Ͼƿ� ����
        MaxBillingType      = 6
    };
    
    enum AuthType
    {
        ROWGlobalAuth       = 0,    // ROW��������
        HangameAuth         = 1,    // �Ѱ��ӿ� ����
        GamaAuth            = 2,    // ��������Ʈ�� ����
        YouxiLandAuth       = 3,    // �븸�� ����
        JapanAuth           = 4,    // �Ϻ��� ����
		GammaniaAuth		= 5,	// �����Ͼ� ����
        MaxAuthType         = 6
    };

    struct DBInfo
	{
		enum Const
		{
			MAX_BUFFER = 128
		};

        OleDB::ConnType m_ConnType;

		char m_szDBAddr[MAX_BUFFER];
		char m_szDBName[MAX_BUFFER];
		char m_szDBUser[MAX_BUFFER];
		char m_szDBPass[MAX_BUFFER];
	};

protected:

	unsigned long	m_ServerID;					// ���� ���̵�
	
	unsigned long	m_dwLimitVer;				// �ּ� ���� ����
	unsigned long	m_dwClientVer;				// Ŭ���̾�Ʈ ����
	char			m_chPatchAddr[MAX_PATH];	// ��ġ ���� �ּ� ���ڿ�

    unsigned long   m_dwBillingType;
    unsigned long   m_dwAuthType;

	bool			m_bPingCheck;				// �� üũ
	bool			m_bAlone;					// ȥ�� ���
	bool			m_bHanCheck;				// �ѱ� üũ
	bool			m_bHackCheck;				// �� üũ
	bool			m_bAdminIPCheck;			// ���� IP üũ
	bool			m_bDuelModeCheck;			// ��� ��� üũ
	bool			m_bLotteryEvent;			// ���� �̺�Ʈ üũ
	bool			m_bLevelUpEvent;			// ������ �̺�Ʈ üũ
	bool			m_bDeathPenaltyEvent;		// ����� �г�Ƽ �̺�Ʈ üũ

	bool			m_bFreeCheck;				// ���� üũ
	bool			m_bIgnoreFlag;				// �÷��� ����

	bool            m_bBattleAuth;				// ��Ʋ�׶��� ���� �����ΰ�?
	bool            m_bBattleGame;				// ��Ʋ�׶��� ���� �����ΰ�?
    bool            m_bBattleAgent;				// ��Ʋ�׶��� �߰� �����ΰ�?

    bool            m_bSupressCharCreate;       // ĳ���� ������ �����ϴ°�?
    bool            m_bSupressCharDelete;       // ĳ���� ������ �����ϴ°�?
	bool			m_bTestGroupAuth;			// ���� ������ �������� �׼����� ����
	bool			m_bChatToolIPCheck;			// ����͸� ���� IP üũ ����

    unsigned char   m_cRestrictedPart1ToPart2Level;     // Part1���� Part2�� ĳ���� ������ �ʿ��� �ּ� ĳ���� ����
    unsigned char   m_cMaxTransferPart1ToPart2Count;    // Part1���� Part2�� ĳ���� ���� ������ ȸ��

	unsigned long	m_dwUserLimit;				// ���� ���� ����
	unsigned long	m_dwCheckSum;				// üũ��

	unsigned long	m_dwExp;					// ����ġ(Percentage)
	unsigned long	m_dwDrop;					// �����(Percentage)
	unsigned long	m_dwFame;					// ��ġ(Percentage)
	unsigned long	m_dwRefine;					// ���ü�����(Percentage)
	unsigned long	m_dwMileage;				// ���ϸ���(Percentage)
	unsigned long	m_dwEquipCorr;
    
	unsigned short	m_wBattleLimit;				// ��Ʋ�׶��� ���� �ο�//
	unsigned short	m_wBattleLimitPer;			// ��Ʋ�׶��� ���� �ۼ�Ʈ


	// �����Ͼ� ���� ����
	char			m_szGammaniaCode[32];
	char			m_szGammaniaRegin[32];
	char			m_szGammaniaAddr[32];				// �����Ͼ� �����ּ�
	int				m_GammaniaPort1;
	int				m_GammaniaPort2;


	INET_Addr       m_ServerAddress[MaxType];   // ���� �ּҵ�
    INET_Addr       m_GameServerUDPAddr;
    INET_Addr       m_HanUnitedBillingAddr;     // �Ѱ��� ���պ��� ���� �ּ�(UID������ ������)
	INET_Addr		m_Part1UnifiedAgentAddr;	// ��Ʈ1 ���ձ��� �߰輭�� �ּ�
    INET_Addr       m_P1GameServerUDPAddr;      // ��Ʈ1 ���Ӽ����� UDP�ּ�

    DBInfo			m_stPart1UnifiedDBInfo;		// ��Ʈ1 ���ձ��� DB ����
	DBInfo			m_stAdminToolDBInfo;		// � DB ����
    DBInfo          m_stKeeperDBInfo;           // ���� DB ����
    DBInfo          m_stAuthDBInfo;             // ���� DB ����
    DBInfo          m_stGameDBInfo;             // ���� DB ����
    DBInfo          m_stLogDBInfo;              // �α� DB ����

	unsigned long	        m_dwGameContentsFlag;	// ����� �������� �����ϴ� �÷���
    GameRYL::ServiceNation	m_eNationType;		    // ���� Ÿ�� (������ ���� ���� ���� �Ϻΰ� �ٸ��� �����)

    std::vector<unsigned long>	    m_vecAdminUID;	                // ������ UID ����Ʈ
    UnifiedConst::AgentServerType   m_eAgentServerType;		        // �߰� ���� Ÿ�� (��Ʈ1, ��Ʈ2, ��Ʈ1����...)
    unsigned long                   m_dwSelectableUnifiedServerNum; // ���� ���ս� ������ ���� ���

public:

	static const char* GetServerWindowName(ServerType eServerType);
    static const char* GetManageClientWindowName();

    static unsigned short GetGameServerTCPPort(unsigned long dwServerID);    
    static unsigned short GetP1GameServerUDPPort(unsigned long dwServerID);

    static char GetZoneFromCmdLine();
    static char GetChannelFromCmdLine();

    CServerSetup();
	virtual ~CServerSetup();

	bool Initialize(ServerType eServerType);

	bool InitLoginServer(void);
	bool InitAuthServer(void);	
    bool InitGameServer(void);
	bool InitAgentServer(void);
	bool InitUIDServer(void);
    bool InitLogServer(void);
    bool InitChatServer(void);

	unsigned long 	GetServerID(void);
	char			GetServerGroup(void);
	char			GetServerZone(void);
    char            GetServerChannel(void);

	unsigned long	GetLimitVer(void)		{ return m_dwLimitVer;			}
	unsigned long	GetClientVer(void)		{ return m_dwClientVer;			}

	char*			GetPatchAddress(void)	{ return m_chPatchAddr;			}
	
    unsigned long   GetBillingType(void)    { return m_dwBillingType;       }
    unsigned long   GetAuthType(void)       { return m_dwAuthType;          }

	GameRYL::ServiceNation	GetNationType(void)	{ return m_eNationType;		}

    UnifiedConst::AgentServerType GetAgentServerType()	{ return m_eAgentServerType; }
    unsigned long   GetSelectableUnifiedServerNum(void) { return m_dwSelectableUnifiedServerNum; }

    bool    IsSupressCharCreate()   { return m_bSupressCharCreate; }
    bool    IsSupressCharDelete()   { return m_bSupressCharDelete; }
	bool	IsTestGroupAuth(void)	{ return m_bTestGroupAuth; }

    unsigned char   GetRestrictedPart1ToPart2Level() { return m_cRestrictedPart1ToPart2Level; }
    unsigned char   GetMaxTransferPart1ToPart2Count() { return m_cMaxTransferPart1ToPart2Count; }

	bool	GetPingCheck(void)			{ return m_bPingCheck;			}
	bool	GetAlone(void)				{ return m_bAlone;				}
	bool	GetHanCheck(void)			{ return m_bHanCheck;			}
	bool	GetHackCheck(void)			{ return m_bHackCheck;			}
	bool	GetAdminIPCheck(void)		{ return m_bAdminIPCheck;		}
	bool	GetDuelModeCheck(void)		{ return m_bDuelModeCheck;		}
	bool	GetLotteryEvent(void)		{ return m_bLotteryEvent;		}
	bool	GetLevelUpEvent(void)		{ return m_bLevelUpEvent;		}
	bool	GetDeathPenaltyEvent(void)	{ return m_bDeathPenaltyEvent;	}
	bool	GetChatToolIPCheck(void)	{ return m_bChatToolIPCheck;	}

	bool	GetFreeCheck(void)		{ return m_bFreeCheck;		}
	bool	GetIgnoreFlag(void)		{ return m_bIgnoreFlag;		}

	unsigned long	GetUserLimit(void)		{ return m_dwUserLimit;		}
	unsigned long	GetCheckSum(void)		{ return m_dwCheckSum;		}

	unsigned long	GetExpDefault(void)		{ return m_dwExp;			}
	unsigned long	GetDropDefault(void)	{ return m_dwDrop;			}
	unsigned long	GetFameDefault(void)		{ return m_dwFame;			}
	unsigned long	GetRefineDefault(void)		{ return m_dwRefine;			}
	unsigned long	GetMileageDefault(void)		{ return m_dwMileage;		}
	unsigned long	GetEquipCorrDefault(void)	{ return m_dwEquipCorr;		}

	unsigned short	GetBattleLimit(void)	{ return m_wBattleLimit;	}
	unsigned short	GetBattleLimitPer(void)	{ return m_wBattleLimitPer;	}

    INET_Addr& GetServerAddress(ServerType eServerType) { return m_ServerAddress[eServerType]; }
    INET_Addr& GetP1GameServerUDPAddress()  { return m_P1GameServerUDPAddr;   }
    INET_Addr& GetHanUnitedBillingAddr() 	{ return m_HanUnitedBillingAddr;  }
	INET_Addr& GetPart1UnifiedAgentAddr()	{ return m_Part1UnifiedAgentAddr; }

	unsigned long SetClientVer(unsigned long ClientVer_In)	{ return m_dwClientVer = ClientVer_In;	}
	unsigned long SetCheckSum(unsigned long CheckSum_In)	{ return m_dwCheckSum = CheckSum_In;	}

	bool IsBattleAuthServer(void) const { return m_bBattleAuth;	}
	bool IsBattleGameServer(void) const { return m_bBattleGame;	}
	bool IsBattleAgentServer(void) const { return m_bBattleAgent; }

	DBInfo& GetPart1UnifiedDBInfo() { return m_stPart1UnifiedDBInfo; }
	DBInfo&	GetAdminToolDBInfo()	{ return m_stAdminToolDBInfo;	 }
    DBInfo& GetKeeperDBInfo()       { return m_stKeeperDBInfo;       }
    DBInfo& GetAuthDBInfo()         { return m_stAuthDBInfo;         }
    DBInfo& GetGameDBInfo()         { return m_stGameDBInfo;         }
    DBInfo& GetLogDBInfo()          { return m_stLogDBInfo;          }

	char* GetGammaniaCode()			{ return m_szGammaniaCode;		}
	char* GetGammaniaRegin()		{ return m_szGammaniaRegin;		}
	char* GetGammaniaAddr()			{ return m_szGammaniaAddr;		}
	int   GammaniaPort1()			{ return m_GammaniaPort1;		}
	int   GammaniaPort2()			{ return m_GammaniaPort2;		}

	bool UseContents(unsigned long dwContents)	{ return (m_dwGameContentsFlag & dwContents) == dwContents;	}
};

#endif