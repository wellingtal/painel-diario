//================================================================
//
//	Name : RYLGameData.h 
//	Desc : 게임 관련 데이타
//			- Client 메세지 루프에 직접 관여하는 데이터들
//  Date : 2003. 11. 16
//
//================================================================

#ifndef __RYL_GAMEDATA_H__
#define __RYL_GAMEDATA_H__

#include <Network/Packet/PacketStruct/CharAttackPacket.h>
#include <Network/Packet/PacketStruct/CharConfigPacketStruct.h>
#include <Skill/SkillStructure.h>

#include "../CharStatus.h"

#include "SceneManager.h"
#include "RYLObjectControl.h"

#include <windows.h>
#include <vector>
#include <atltime.h>

#define CURSOR_NORMAL				0
#define CURSOR_CLICK				1
#define CURSOR_PICKUP				2
#define CURSOR_ATTACK				3
#define CURSOR_DIALOG				4
#define CURSOR_REPAIR				5
#define CURSOR_TOOLTIP				6
#define CURSOR_POPUP				7

#define CLIENT_LOGO							0
#define CLIENT_MAIN							1
#define CLIENT_NATIONSELECT					2
#define CLIENT_CHARACTERSELECT				3
#define CLIENT_GAME							4
#define CLIENT_END							5
#define CLIENT_CHARACTERSELECTLOADING		6
#define CLIENT_GAMELOADING					7
#define CLIENT_ZONEMOVE						8

#define FADE_IN				0
#define FADE_OUT			1
#define FADE_NONE			0xFFFFFFFF

#define FADE_START			0
#define FADE_END			200

#define MAX_CHAR_SLOT		5

#define VIEW_ALL_NAME		0
#define VIEW_PC_NAME		1
#define VIEW_OTHER_NAME		2
#define VIEW_NO_NAME		3

namespace BattleInclination
{
	void SetCharData(RYLCreature* pCreature, CharDataNode* pDataNode, 
		BattleInclination::CharData& charData, bool bSetRideArmsInfo = false);
	void SetRelationData(RYLCreature* pOwnerCreature, RYLCreature* pTargetCreature, 
		BattleInclination::RelationData& relationData);
};

struct ClientOption
{
	BOOL 			m_bTipBroad;
	BOOL 			m_bChat;

	unsigned long 	m_dwCharName;

	unsigned long 	m_dwSkillEffect;
	unsigned long 	m_dwChantEffect;
	unsigned long 	m_dwEquipEffect;

	unsigned long 	m_dwMasterSound;
	unsigned long 	m_dwEffectSound;
	unsigned long 	m_dwBGMSound; 

	float 			m_fMasterVolume;
	float 			m_fAmbVolume;
	float 			m_fBgmVolume;
};

class CTexture ;
class CRYLRaceBase ;

struct GUILDLARGEINFOEX ;

class CRYLGameData
{
public :

	enum RELOGIN
	{		
		CHAR_SELECT	= 1,
		LOGIN		= 2
	};

	CSceneManager*		m_lpSceneManager ;					// SceneManager Class
	CRYLRaceBase*		m_lpInterface ;	

	static char			m_strSheet[ MAX_PATH ] ;
	long				m_lQuickChatTimer ;
	ClientOption		m_coOption ;
	RejectOption		m_roOption;

	long				m_lTipBroadCounterSeed;				// 화면에 뿌려지는 팁의 위한 변수
	long				m_lTipBroadCounter;					// 화면에 뿌려지는 팁의 위한 변수
	vector<char *>		m_lstTipBroad;						// 화면에 뿌려지는 팁의 위한 변수

	bool				m_bEveryEffect ;					// 옵션 (다른사람의 이펙트)
	bool				m_bPartyEffect ;					// 옵션 (파티의 이펙트)
	bool				m_bSelfEffect ;						// 옵션 (자신의 이펙트)
	bool				m_bFriendEffect ;					// 옵션 (아군의 이펙트)
	bool				m_bNeutralEffect ;					// 옵션 (중립의 이펙트)
	bool				m_bEnemyEffect ;					// 옵션 (적군의 이펙트)	

	CCharStatus			m_csStatus;							// 캐릭터 정보
	
	long				m_lPortalCounter;					// 포탈시의 카운트다운 타이머
	long				m_lKillCounter;						// 자살시의 카운트다운 타이머
	long				m_lExitCounter;						// 클라이언트 종료시의 카운트다운 타이머
	long				m_lLogoutCounter ;					// 캐릭터 로그아웃 타이머
	long				m_lLogoutTimeResult ;				// Login Counter ( n초 남았습니다. )<--이거
	bool				m_bLogoutEnable ;
	long				m_lSkillUseCounter;
	long				m_lBillingTimer;	
	int					m_nPlayTime;						// BillingType이 정액이나 무료인 경우는 남은 날짜. 정량인 경우는 남은 시간(분)
	
	long				m_lRespawnCounter;					// 사망 후 1분이 지나면 자동으로 리스폰


	CItemInstance*		m_lpPortalScroll;					// 현재 쓴 포탈 스크롤 아이템
	CItemInstance*		m_lpPickItem;						// 현재 손에 쥐고 있는 아이템
	CItemInstance*		m_lpStartKit;						// 현재 사용한 스타트 킷
	CItemInstance*		m_lpCampStartKit ;					// Camp에 사용한 스타트 킷
	CItemInstance*		m_lpWorldWeaponKit ;				// 월드웨폰에 사용한 스타터 킷

	// 계명확인서 아이템
	CItemInstance*		m_lpRenameWarrantItem;

	BOOL				m_bStallRegFlag;					// 개인상점 플래그전용
	BOOL				m_bPullDownFlag;					// 아이템을 버렸으면 플래그
	BOOL				m_bUseCashFlag;						// 캐쉬 아이템 사용플래그

	BOOL				m_bSkillFlag;						// 스킬사용중 중복으로 스킬을 사용못하게 제한.

	unsigned long		m_dwClientMode;						// 클라이언트 현재 상태
	BOOL				m_bUsedDeposit;

	BOOL				m_bCursorToggle;
	
	list<LPFieldInstance>	m_lstFieldItem;					// 바닥에 떨어져 있는 아이템

	CEffScript*			m_ClassEffectPtr;					// 전직 이팩트 스크립트 pointer 
	int					m_ClassEffectAlpha;					// 전직 이팩트시 나오는 글자의 alpha value

	CEffScript*			m_lpItemUpgrade;					// 아이템 업그레이드 이펙트 포인터
	CEffScript*			m_lpItemComposition;				// 아이템 이식 이펙트 포인터
	CEffScript*			m_lpItemResultEft;					// 아이템 이식 결과 이팩트포인트

	
	CEffScript*			m_pEnemySelectEff;					// 적 선택시 화살표
	CEffScript*			m_pEnemySkillSelectEff;				// 전 선택시 바닥에 보이는 동그라미
	CEffScript*			m_pPlayerSelectEff;					// 플레이어 선택시 화살표
	CEffScript*			m_pPlayerSkillSelectEff;			// 플레이어 선택시 바닥에 보이는 동그라미

	CEffScript*			m_QuestCompleteEffectPtr;			// 퀘스트 완료시 이팩트 스크립트 pointer 
	BOOL				m_bQuestComplete;					// 퀘스트 완료인지를 체크하는 변수

	unsigned long		m_dwStartQuickSlot;
	vector3				m_vecStartPosition;					// 로긴인시에 나오는 처음 좌표

	BOOL				m_bMouseMode;						// 마우스 모드 / 키보드 모드
	BOOL				m_bAutoRunMode;						// 자동 달리기 모드
	BOOL				m_bSimpleMode;						// 단순 모드
	BOOL				m_bEnableMove;						// 움직일수 있는가
	unsigned long		m_dwTradingNpcID;					// 현재 거래하고 있는 NPC ID
	BOOL				m_bHardwareCursor;					// 하드웨어 / 소프트웨어 유무
	BOOL				m_bShowCursor;						// 커서 표시 유무
	BOOL				m_bSystemConsoleShow;				// 콘솔창
	unsigned long		m_dwKindCursor;						// 커서 종류, 종류는 define되 있음
	unsigned long		m_dwMessageBoxResult;				// 메세지 박스에서 돌려지는 결과값
	
	BOOL				m_bTargetMouseMode;					// Alt + G 효과 할려고 만든 함수
	BOOL				m_bChangeWeapon;					// 이 변수가 TRUE가 되면 무기전환을 할 수 있다.
	BOOL				m_bInterfaceAlreadyLoad;			// 한번만 인터페이스를 로딩하기 위해
	BOOL				m_bStreetStallEnter;				// 노점상에 들어가있는지 체크
	BOOL				m_bAdminObserver;

	unsigned short		m_wSkillUseID;						// 현재 쓰고 있는 스킬의 ID
	unsigned long		m_dwRespawnResult;					// 리스폰시의 메세지창 결과값

	unsigned long		m_dwClassCleanResult;				// 클래스 초기화시의 메세지창 결과값

	unsigned long		m_dwDropPickResult;					// 손에든 아이템을 버릴때
	unsigned long		m_dwTradePickResult;				// 손에든 아이템을 팔때

	unsigned long		m_dwTempInvenResult;				// 임시 인벤에서 물건을 집을때의 메세지창 결과값
	unsigned char		m_cTempInvenItemNum;				// 임시 인벤에서 물건을 집을때의 집은 물건의 갯수

	unsigned long		m_dwAuthorizeResult;				// 리콜이나 부활시 메세지창 결과값
	unsigned long		m_dwAuthorizeCasterID;				// 마법을 시전한 사람
	unsigned char		m_cAuthorizeCmd;					// 리콜 / 부활 명령

	char				m_cEliteBonus;						// 엘리트 보너스
	unsigned long		m_dwClientRace ;					// 현재 선택한 종족
	unsigned long		m_dwClientSex ;					// 현재 선택한 종족

	unsigned long		m_dwClientLoginResult ;				// 중복 로긴시 끊고 들어가기 메세지 박스의 결과값
	unsigned long		m_dwClientExitConfirm ;				// 인증시 클라이언트 강제 종료 메세지 박스의 확인값
	unsigned long		m_dwExecuteExplorerConfirm ;		// 인증 3회 실패시 익스플로러 실행 메세지 박스의 확인값

	unsigned long		m_dwGuildWarPerTime;				// 길드 전쟁 남은 시간.
	unsigned long		m_dwNationWarPerTime;				// 국가 전쟁 남은 시간.

	unsigned long		m_dwRealmWarJoinResult ;			// 국가전 참여 결과값 (국가 전쟁 시간 10분전에 출력되는 인증창 결과값)

	unsigned long		m_dwGuildWarJoinLeaveResult ;		// 길드전 참여 결과값 (길드 멤버탭의 참여 선언)

	unsigned long		m_dwRequestResult ;					// 이미 적대 선언이 된 길드와 평화선언을 하고 다른 길드와 적대 선언을 할지 결과값
	unsigned long		m_dwCounterHostilityResult ;		// 카운터 적대 선언 결과값
	unsigned long		m_dwTargetGID ;						// 적대 신청을 하려고 하는 대상 길드 GID / 카운터 적대 신청 대상 길드 GID
	unsigned long		m_dwAlreadyHostilityGID ;			// 이미 적대 관계인 길드 GID
	
	BOOL				m_bUpdateMove ;						// 업데이트 패킷의 토글 변수

	BOOL				m_bGuildSafeResult;					// 금고 패킷응답결과변수


	CHAR_VIEW			m_scLoginChar[ MAX_CHAR_SLOT ] ;	// 로그인 캐릭터 정보(5명)
	sGuildData			m_scLoginCharGuildData[ MAX_CHAR_SLOT ] ;
	vector3				m_vecCharSelectPos[ MAX_CHAR_SLOT ] ;
	float				m_fCharSelectDir[ MAX_CHAR_SLOT ] ;

	unsigned char		m_cChangeNameCount;
	// WORK_LIST 2.1 계정 국적 추가
	unsigned char		m_cAccountNation;					// 계정 국적
	unsigned long		m_dwSelectChr ;						// 현재 선택한 캐릭터
	unsigned long		m_dwClientState ;					// 클라이언트 모드내에서 클라이언트 상태
	BOOL				m_bCreateChar;
	BOOL				m_bDeleteChar;
	BOOL				m_bSelectChar;
	BOOL				m_bGameStartState[ 2 ] ;

	unsigned long		m_dwFunction ;						// 스킬 사용시 쓰이는 변수

	DWORD				m_dwSoundTickCount ;

	BOOL 				m_bShowInfo;						// 화면에 정보 표시 유무
	BOOL				m_bSimpleMouse ;					// 심플 마우스 모드 (키보드 모드에서 마우스 커서 보이기)
	BOOL				m_bSaveID ;
	BOOL				m_bFPS ;							// FPS 정보 표시 유무

	bool 				m_bMovieVersion;					// 무비 모드
	unsigned long		m_dwViewNameType;					// 게임 화면 내의 캐릭터 이름 감추기 옵션(전체)

	// 중국판에서 게임 진행 시간을 알려주기 위한 방법
	unsigned int		m_dwPlayTime;
	unsigned int		m_dwNewTime;
	CTime				m_StartTime;
	long				m_lPlayTimer;	

	// 프리미엄 서비스를 위해 시간을 알려주기 위한 값
	long				m_lPremiumTime;
	int					m_iPremiumType;
	long				m_lNewPremiumTime;
	CTime				m_StartPremiumTime;
	long				m_lPlayPremiumTimer;	

	BOOL				m_bRYLTradeMode;		// 아이템 트리에드 모드일때 TRUE가 된다.
	BOOL				m_bRYLAutoTrade;		// 자동으로 아이템을 팔기위한 타입
	CItemInstance*		m_lpTradeItem;			// 현재 손에 쥐고 있는 아이템
	int					m_iRYLAutoTradeNum;		// 판매개수


//	BOOL				m_bRYLVaultClick ;
//	BOOL				m_bRYLBlacksmithClick ;
//	BOOL				m_bRYLTradeDlgClick ;
    BOOL				m_bRYLStreetStallDlgClick ;
	BOOL				m_bRYLHumanInventoryFrameDlgClick ;
	BOOL				m_bRYLExchangeDlgClick ;

	unsigned char		m_cTempAdminFlag;

	unsigned char		m_cTempGuildWarFlag;				// 로그인시에 자기 크리쳐 객체가 생성되지 않아서,
	unsigned char		m_cTempRealmWarFlag;				// 임시로 값을 가지고 있는 변수이다. 코드내에서 사용하지 않도록...
	unsigned char		m_cTempRealmPoint;					// 임시 국가 전쟁 공헌포인트.
	unsigned char		m_cTempTacticsFlag;					// 임시 용병 플래그.

	unsigned char		m_cReLogin;	

	// Old CommonInterface Method	
	long				m_lScreenFade ;
	long				m_lItemFade ;
	unsigned long		m_dwFadeMode ;
	CTexture*			m_lpNumber ;
	CTexture*			m_lpStackNumber ;
	CTexture*			m_lpCursor ;

	DWORD				m_dwRideTickCount;

//	DWORD				m_dwFireingTick;

private:

   unsigned char	m_cEnemyTypeResult;					// 최근 적아군식별의 근거


public :

	CRYLGameData() ;
	~CRYLGameData() ;

	VOID		LinkCharacterData() ;
	
	static char*		 GetMotionSheet( char *strWeaponName, char *strShieldName, unsigned short wClass ) ;
	static unsigned long GetWeaponType( char *strWeaponName ) ;

	static void			 CheckTargetLength( unsigned long &dwTargetID) ;
	static void			 CheckTargetforSkill( unsigned long &dwTargetID, BOOL bCheckLength ) ;

	static BOOL			 CheckSkillStart( BOOL bDown ) ;
	static BOOL			 CheckStillCasting( unsigned long &dwFunction ) ;
	static void			 SetKillCounter( long lKillCounter ) ;
	static void			 SetRespawnCounter( long lKillCounter ) ;
	static void			 SetClientExitCounter( long lKillCounter ) ;
	static void			 SetLogoutCounter(long lLogoutCounter) ;
	static void			 SetPortalCounter( long lPortalCounter ) ;
	static void			 SetSkillUseCounter( long lSkillUseCounter ) ;
	static void			 SetAutoRun( BOOL bAutoRun );
	static BOOL			 GetAutoRun( void ) ; 
	static void			 ChangeWeapon( void ) ;
	static void			 SetChangeWeapon( BOOL bChangeWeapon ) ; 

	static void			 ChangeRide(void);

	static BOOL			 CheckAttackable( void ) ;
	static void			 UpdateCamp( RYLCreature* pCreature ) ;
	static vector3		 GetMonsterMinBox( unsigned long dwChrID ) ;
	static vector3		 GetMonsterMaxBox( unsigned long dwChrID ) ;
	static unsigned long GetClientIdentity( unsigned long dwChrID ) ;											// PC인지 NPC인지 MONSTER인지 소환물 인지 건물 인지
	static unsigned long GetClientEnemyType( unsigned long dwChrID ) ;											// 피아식별( friend/neutral/enemy )

	static unsigned short GetEnchantLevel( unsigned long lID ) ;
	static unsigned long GetEnchantTime( unsigned long lID ) ;
	static unsigned short GetSkillIDbyChantID(unsigned char cChant);
	static unsigned long  GetClientRace();												// 현재 선택한 종족

	const unsigned char	GetEnemyTypeResult(void)					{ return m_cEnemyTypeResult;	}
	void				SetEnemyTypeResult(unsigned char cResult)	{ m_cEnemyTypeResult = cResult;	}

	unsigned long GetScreenPosChr(POINT &Point, int nMode, float fLength);
	BOOL	GetChangeWeapon(void);
	BOOL	CreateRaceInterface( unsigned long dwRace,unsigned long dwSex );

	VOID	UpdateEnchantTime();		// 인첸트 타임을 업데이트 한다.

	VOID	ClearGameDlgData() ;
	VOID	MoveCharSelectScene() ;
	void	EnterClient(void);

	VOID	UpdateQuickChatTimer( FLOAT fUpdate ) ;
	VOID	UpdateTipBroadCounter( FLOAT fUpdate ) ;
	VOID	UpdatePartyAttackTimer( FLOAT fUpdate ) ;
	VOID	UpdateSkillUseCounter( FLOAT fUpdate ) ;
	VOID	UpdateLogoutCounter( FLOAT fUpdate ) ;
	VOID	UpdatePortalCounter( FLOAT fUpdate ) ;
	VOID	UpdateKillCounter( FLOAT fUpdate ) ;
	VOID	UpdateRespawnCounter( FLOAT fUpdate ) ;
	
	VOID	UpdateClientExitCounter( FLOAT fUpdate ) ;
	VOID	UpdateBillingTimer( FLOAT fUpdate ) ;

	VOID	UpdateDeadCounter( FLOAT fUpdate ) ;
	
	VOID	UpdateGameTimer( FLOAT fUpdate ) ;
	VOID	UpdatePremiumTimer( FLOAT fUpdate ) ;

	VOID	UpdateFrameTimer() ;
	VOID	UpdateInterfaceEffect() ;
	

	BOOL	LoadClientOption(const char *strFileName) ;
	BOOL	SaveClientOption(const char *strFileName, ClientOption *coOption) ;
	void	SetTipBroadTimer(long lTimer) ;
	VOID	InitTipBroad( char* pszFilePath ) ;
	VOID	DestroyTipBroad() ;
	BOOL	GetSpeedHack() ;

	void	SetSkillCoolDown( unsigned short wSkill ) ;
	void	SetSkillCoolDown( unsigned short wSkill, unsigned short wGrade, unsigned char cEndCooldown ) ;
	void	SetSkillCoolDown( unsigned short wGrade, unsigned char cEndCooldown ) ;
	void	SetPotionCoolDown( unsigned short wPotionSkillID, unsigned int iCurCooldown = 0) ;

	void	ResetTerrain() ;
	void	AddFieldObject( FieldObject* lpObject ) ;
	void	DeleteFieldObject( unsigned __int64 nFieldInstanceID ) ;
	void	DeleteAllFieldItem() ;
	LPFieldInstance GetFieldObject( unsigned long dwFieldInstanceID ) ;

	VOID	RenderClassEffect() ;
	VOID	LoadTargetCursor() ;
	VOID	RenderQuestCompleteEffect() ;

	void	SetPickItem( CItemInstance *lpItem, BOOL bSound = TRUE ) ;
	void	PlayClickSound(void) ;

	bool 	IsInSafetyZone( RYLCreature* pCreature );
	bool 	CanUseSkill( RYLCreature* pCreature, Skill::Target::Type eTargetType );

	Skill::Type::SkillType	GetAlterSkillType(void);
	bool					GetAlterSkillEnable(void);

	void	SetEnableMove( BOOL bEnableMove ) ;
	BOOL	GetEnableMove( void ) ;

	void			ChangeViewNameMode() ;
	unsigned long	GetViewNameMode() const							{ return m_dwViewNameType ;	}

	const Broadcast2nd::CNetworkEquipGrade	GetNetworkEquipGrade(void);


protected:
    
	bool IsPeaceMode(const GUILDLARGEINFOEX* pGuildInfo, unsigned char& ucNation);

	// 피아식별
	static unsigned long GetClientEnemyTypePC( RYLCreature* pRivalCreature, CharDataNode* pRivalData) ;


public:

	// Old CommonInterface Mothod
	void	RenderNumber(LPDIRECT3DDEVICE8 lpD3DDevice, short sPosX, short sPosY, unsigned char cAlpha, unsigned short wMode, long lValue, float fRate) ;
	void	RenderRect(LPDIRECT3DDEVICE8 lpD3DDevice, unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2, unsigned long dwColor, unsigned char cAlpha) ;
	char*	GetClassName(unsigned short wClass) ;
	void	RenderStackNumber(LPDIRECT3DDEVICE8 lpD3DDevice, short sPosX, short sPosY, long lValue) ;
	void	RenderCursor(LPDIRECT3DDEVICE8 lpD3DDevice, unsigned short wPosX, unsigned short wPosY, unsigned long dwKindCursor, unsigned char cAlpha = 0xFF) ;
	BOOL	EncodeJPGFile(LPTSTR lpstrFilename, LPDIRECT3DSURFACE8 lpSurface) ;

	void FadeIn(void)
	{				
		m_dwFadeMode = FADE_IN;
		m_lScreenFade = FADE_START;
		m_lItemFade = FADE_START;	
	}
	void FadeOut(void)
	{		
		m_dwFadeMode = FADE_OUT;
		m_lScreenFade = FADE_END;
		m_lItemFade = FADE_END;
	}

	RespawnArea			m_RespawnArea[ 100 ] ;
	unsigned char		m_iRespawnNum ;
 

public :

	static CRYLGameData*	s_pInstance ;
	static CRYLGameData*	Instance()	{ return s_pInstance ; }
} ;

extern CCharacterControl	g_CharacterData ;

#endif //__RYL_GAMEDATA_H__

