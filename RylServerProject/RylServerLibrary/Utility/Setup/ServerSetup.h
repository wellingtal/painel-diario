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
        // 10000 ~ 11000 port : 외부 오픈
        // 11001 ~ 12000 port : 사내에만 오픈
        // 12001 ~ 13000 port : 내부 서버간 연결로만 사용 (내부끼리만 오픈)

        // 클라이언트 -> 서버 (외부 -> 내부, 변경하면 안됨(클라이언트에 하드코드 되어 있음))
		LoginServerLauncherListen       = 10101,	// 런처 -> 로그인서버
        AuthServerClientListen          = 10210,    // 클라이언트 -> 인증서버
        
        // 클라이언트 -> 서버 (외부 -> 내부, 변경해도 상관 없음.. 파트2는 사용 안함)
        P1ChatServerClientListen        = 10111,

        // 클라이언트 -> 게임서버 (외부 -> 내부, 규칙에 따라 변동됨, 범위 내에서는 바꿔도 괜찮다.)
        // 게임쪽 TCP포트. GameServerBaseTCPPort + 채널 번호(0~4) * 20 + 존 번호(1~20)
        GameServerClientBaseListen      = 10400,
        MAX_ZONE                        = 20,
        MAX_CHANNEL                     = 5,

        // 클라이언트 -> 게임서버 (외부 -> 내부, 규칙에 따라 변동됨, 범위 내에서는 바꿔도 괜찮다.)
        // 파트 1 전용이다. 파트 2는 UDP를 사용하지 않는다.
        P1GameServerClientUDPListen     = 10500,

        // 다음 포트는, 외부에서는 허용된 ip에 한해서만 접근 가능하도록 한다.
        // 11001 ~ 11100 포트는 PCAnywhere혹은 Remote Desktop등을 열어 놓기 위해서 비워 둔다.
        P1AdminToolServerClientListen   = 11121,    // 파트 1 운영툴 클라이언트 -> 파트 1 운영툴 서버
        P2AdminToolServerClientListen   = 11122,    // 파트 2 운영툴 클라이언트 -> 파트 2 운영툴 서버
        LogServerLogClientListen        = 11123,    // 로그 클라이언트(로그 모니터링 툴) -> 로그 서버
		ManageServerManageToolListen    = 11124,	// 관리 툴 -> 관리 서버
        ChatServerMonitoringToolListen  = 11125,    // 채팅 모니터링 툴 -> 채팅서버
        StatServerManageServerListen    = 11126,    // 관리 서버 -> 통계 서버

        // 각 서버간 연결 (외부에는 전부 막아 놓아야 한다, 내부간 연결만 허용한다)
        UIDServerDBAgentServerListen    = 12531,	// DB중계서버 -> UID서버
		LoginServerDBAgentServerListen  = 12532,	// DB중계서버 -> 로그인서버
		
        DBAgentServerGameServerListen   = 12533,	// 게임서버 -> DB중계서버				
		DBAgentServerAuthServerListen   = 12534,	// 인증서버 -> DB중계서버
        DBAgentServerChatServerListen   = 12535,    // 채팅서버 -> DB중계서버
        DBAgentAdminToolServerListen    = 12536,    // 운영툴 서버 -> DB중계서버

        ChatServerGameServerListen      = 12537,	// 게임서버 -> 채팅서버
        ManageServerManageClientListen  = 12538,	// 관리 클라이언트 -> 관리 서버
        LogServerOtherServerListen      = 12539     // 다른 서버들 -> 로그 서버
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
        ROWGlobalBilling    = 0,    // ROW고유인증
        GamaBilling         = 1,    // 한국 일반과금(예전 과금방식)
        GamaUnitedBilling   = 2,    // 한국 통합과금
        YouxiLandBilling    = 3,    // 대만용 과금
        JapanBilling		= 4,    // 일본용 과금
        GammaniaBilling     = 5,    // 감마니아용 과금
        MaxBillingType      = 6
    };
    
    enum AuthType
    {
        ROWGlobalAuth       = 0,    // ROW고유인증
        HangameAuth         = 1,    // 한게임용 인증
        GamaAuth            = 2,    // 가마소프트용 인증
        YouxiLandAuth       = 3,    // 대만용 인증
        JapanAuth           = 4,    // 일본용 인증
		GammaniaAuth		= 5,	// 감마니아 인증
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

	unsigned long	m_ServerID;					// 서버 아이디
	
	unsigned long	m_dwLimitVer;				// 최소 제한 버젼
	unsigned long	m_dwClientVer;				// 클라이언트 버젼
	char			m_chPatchAddr[MAX_PATH];	// 패치 서버 주소 문자열

    unsigned long   m_dwBillingType;
    unsigned long   m_dwAuthType;

	bool			m_bPingCheck;				// 핑 체크
	bool			m_bAlone;					// 혼자 모드
	bool			m_bHanCheck;				// 한글 체크
	bool			m_bHackCheck;				// 핵 체크
	bool			m_bAdminIPCheck;			// 영자 IP 체크
	bool			m_bDuelModeCheck;			// 듀얼 모드 체크
	bool			m_bLotteryEvent;			// 복권 이벤트 체크
	bool			m_bLevelUpEvent;			// 레벨업 이벤트 체크
	bool			m_bDeathPenaltyEvent;		// 사망시 패널티 이벤트 체크

	bool			m_bFreeCheck;				// 프리 체크
	bool			m_bIgnoreFlag;				// 플래그 무시

	bool            m_bBattleAuth;				// 배틀그라운드 인증 서버인가?
	bool            m_bBattleGame;				// 배틀그라운드 게임 서버인가?
    bool            m_bBattleAgent;				// 배틀그라운드 중계 서버인가?

    bool            m_bSupressCharCreate;       // 캐릭터 생성을 제한하는가?
    bool            m_bSupressCharDelete;       // 캐릭터 삭제를 제한하는가?
	bool			m_bTestGroupAuth;			// 인증 서버가 정섭인지 테섭인지 여부
	bool			m_bChatToolIPCheck;			// 모니터링 툴의 IP 체크 여부

    unsigned char   m_cRestrictedPart1ToPart2Level;     // Part1에서 Part2로 캐릭터 이전시 필요한 최소 캐릭터 레벨
    unsigned char   m_cMaxTransferPart1ToPart2Count;    // Part1에서 Part2로 캐릭터 이전 가능한 회수

	unsigned long	m_dwUserLimit;				// 유저 제한 숫자
	unsigned long	m_dwCheckSum;				// 체크썸

	unsigned long	m_dwExp;					// 경험치(Percentage)
	unsigned long	m_dwDrop;					// 드랍률(Percentage)
	unsigned long	m_dwFame;					// 명성치(Percentage)
	unsigned long	m_dwRefine;					// 제련성공율(Percentage)
	unsigned long	m_dwMileage;				// 마일리지(Percentage)
	unsigned long	m_dwEquipCorr;
    
	unsigned short	m_wBattleLimit;				// 배틀그라운드 제한 인원//
	unsigned short	m_wBattleLimitPer;			// 배틀그라운드 제한 퍼센트


	// 감마니아 전용 정보
	char			m_szGammaniaCode[32];
	char			m_szGammaniaRegin[32];
	char			m_szGammaniaAddr[32];				// 감마니아 서버주소
	int				m_GammaniaPort1;
	int				m_GammaniaPort2;


	INET_Addr       m_ServerAddress[MaxType];   // 서버 주소들
    INET_Addr       m_GameServerUDPAddr;
    INET_Addr       m_HanUnitedBillingAddr;     // 한게임 통합빌링 서버 주소(UID서버만 세팅함)
	INET_Addr		m_Part1UnifiedAgentAddr;	// 파트1 통합군의 중계서버 주소
    INET_Addr       m_P1GameServerUDPAddr;      // 파트1 게임서버의 UDP주소

    DBInfo			m_stPart1UnifiedDBInfo;		// 파트1 통합군의 DB 정보
	DBInfo			m_stAdminToolDBInfo;		// 운영 DB 정보
    DBInfo          m_stKeeperDBInfo;           // 과금 DB 정보
    DBInfo          m_stAuthDBInfo;             // 인증 DB 정보
    DBInfo          m_stGameDBInfo;             // 게임 DB 정보
    DBInfo          m_stLogDBInfo;              // 로그 DB 정보

	unsigned long	        m_dwGameContentsFlag;	// 적용된 컨텐츠를 관리하는 플래그
    GameRYL::ServiceNation	m_eNationType;		    // 국가 타입 (국가에 따라 게임 룰의 일부가 다르게 적용됨)

    std::vector<unsigned long>	    m_vecAdminUID;	                // 디버깅용 UID 리스트
    UnifiedConst::AgentServerType   m_eAgentServerType;		        // 중계 서버 타입 (파트1, 파트2, 파트1통합...)
    unsigned long                   m_dwSelectableUnifiedServerNum; // 서버 통합시 통합한 서버 대수

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