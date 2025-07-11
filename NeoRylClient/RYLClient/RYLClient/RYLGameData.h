//================================================================
//
//	Name : RYLGameData.h 
//	Desc : ���� ���� ����Ÿ
//			- Client �޼��� ������ ���� �����ϴ� �����͵�
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

	long				m_lTipBroadCounterSeed;				// ȭ�鿡 �ѷ����� ���� ���� ����
	long				m_lTipBroadCounter;					// ȭ�鿡 �ѷ����� ���� ���� ����
	vector<char *>		m_lstTipBroad;						// ȭ�鿡 �ѷ����� ���� ���� ����

	bool				m_bEveryEffect ;					// �ɼ� (�ٸ������ ����Ʈ)
	bool				m_bPartyEffect ;					// �ɼ� (��Ƽ�� ����Ʈ)
	bool				m_bSelfEffect ;						// �ɼ� (�ڽ��� ����Ʈ)
	bool				m_bFriendEffect ;					// �ɼ� (�Ʊ��� ����Ʈ)
	bool				m_bNeutralEffect ;					// �ɼ� (�߸��� ����Ʈ)
	bool				m_bEnemyEffect ;					// �ɼ� (������ ����Ʈ)	

	CCharStatus			m_csStatus;							// ĳ���� ����
	
	long				m_lPortalCounter;					// ��Ż���� ī��Ʈ�ٿ� Ÿ�̸�
	long				m_lKillCounter;						// �ڻ���� ī��Ʈ�ٿ� Ÿ�̸�
	long				m_lExitCounter;						// Ŭ���̾�Ʈ ������� ī��Ʈ�ٿ� Ÿ�̸�
	long				m_lLogoutCounter ;					// ĳ���� �α׾ƿ� Ÿ�̸�
	long				m_lLogoutTimeResult ;				// Login Counter ( n�� ���ҽ��ϴ�. )<--�̰�
	bool				m_bLogoutEnable ;
	long				m_lSkillUseCounter;
	long				m_lBillingTimer;	
	int					m_nPlayTime;						// BillingType�� �����̳� ������ ���� ���� ��¥. ������ ���� ���� �ð�(��)
	
	long				m_lRespawnCounter;					// ��� �� 1���� ������ �ڵ����� ������


	CItemInstance*		m_lpPortalScroll;					// ���� �� ��Ż ��ũ�� ������
	CItemInstance*		m_lpPickItem;						// ���� �տ� ��� �ִ� ������
	CItemInstance*		m_lpStartKit;						// ���� ����� ��ŸƮ Ŷ
	CItemInstance*		m_lpCampStartKit ;					// Camp�� ����� ��ŸƮ Ŷ
	CItemInstance*		m_lpWorldWeaponKit ;				// ��������� ����� ��Ÿ�� Ŷ

	// ���Ȯ�μ� ������
	CItemInstance*		m_lpRenameWarrantItem;

	BOOL				m_bStallRegFlag;					// ���λ��� �÷�������
	BOOL				m_bPullDownFlag;					// �������� �������� �÷���
	BOOL				m_bUseCashFlag;						// ĳ�� ������ ����÷���

	BOOL				m_bSkillFlag;						// ��ų����� �ߺ����� ��ų�� �����ϰ� ����.

	unsigned long		m_dwClientMode;						// Ŭ���̾�Ʈ ���� ����
	BOOL				m_bUsedDeposit;

	BOOL				m_bCursorToggle;
	
	list<LPFieldInstance>	m_lstFieldItem;					// �ٴڿ� ������ �ִ� ������

	CEffScript*			m_ClassEffectPtr;					// ���� ����Ʈ ��ũ��Ʈ pointer 
	int					m_ClassEffectAlpha;					// ���� ����Ʈ�� ������ ������ alpha value

	CEffScript*			m_lpItemUpgrade;					// ������ ���׷��̵� ����Ʈ ������
	CEffScript*			m_lpItemComposition;				// ������ �̽� ����Ʈ ������
	CEffScript*			m_lpItemResultEft;					// ������ �̽� ��� ����Ʈ����Ʈ

	
	CEffScript*			m_pEnemySelectEff;					// �� ���ý� ȭ��ǥ
	CEffScript*			m_pEnemySkillSelectEff;				// �� ���ý� �ٴڿ� ���̴� ���׶��
	CEffScript*			m_pPlayerSelectEff;					// �÷��̾� ���ý� ȭ��ǥ
	CEffScript*			m_pPlayerSkillSelectEff;			// �÷��̾� ���ý� �ٴڿ� ���̴� ���׶��

	CEffScript*			m_QuestCompleteEffectPtr;			// ����Ʈ �Ϸ�� ����Ʈ ��ũ��Ʈ pointer 
	BOOL				m_bQuestComplete;					// ����Ʈ �Ϸ������� üũ�ϴ� ����

	unsigned long		m_dwStartQuickSlot;
	vector3				m_vecStartPosition;					// �α��νÿ� ������ ó�� ��ǥ

	BOOL				m_bMouseMode;						// ���콺 ��� / Ű���� ���
	BOOL				m_bAutoRunMode;						// �ڵ� �޸��� ���
	BOOL				m_bSimpleMode;						// �ܼ� ���
	BOOL				m_bEnableMove;						// �����ϼ� �ִ°�
	unsigned long		m_dwTradingNpcID;					// ���� �ŷ��ϰ� �ִ� NPC ID
	BOOL				m_bHardwareCursor;					// �ϵ���� / ����Ʈ���� ����
	BOOL				m_bShowCursor;						// Ŀ�� ǥ�� ����
	BOOL				m_bSystemConsoleShow;				// �ܼ�â
	unsigned long		m_dwKindCursor;						// Ŀ�� ����, ������ define�� ����
	unsigned long		m_dwMessageBoxResult;				// �޼��� �ڽ����� �������� �����
	
	BOOL				m_bTargetMouseMode;					// Alt + G ȿ�� �ҷ��� ���� �Լ�
	BOOL				m_bChangeWeapon;					// �� ������ TRUE�� �Ǹ� ������ȯ�� �� �� �ִ�.
	BOOL				m_bInterfaceAlreadyLoad;			// �ѹ��� �������̽��� �ε��ϱ� ����
	BOOL				m_bStreetStallEnter;				// ������ ���ִ��� üũ
	BOOL				m_bAdminObserver;

	unsigned short		m_wSkillUseID;						// ���� ���� �ִ� ��ų�� ID
	unsigned long		m_dwRespawnResult;					// ���������� �޼���â �����

	unsigned long		m_dwClassCleanResult;				// Ŭ���� �ʱ�ȭ���� �޼���â �����

	unsigned long		m_dwDropPickResult;					// �տ��� �������� ������
	unsigned long		m_dwTradePickResult;				// �տ��� �������� �ȶ�

	unsigned long		m_dwTempInvenResult;				// �ӽ� �κ����� ������ �������� �޼���â �����
	unsigned char		m_cTempInvenItemNum;				// �ӽ� �κ����� ������ �������� ���� ������ ����

	unsigned long		m_dwAuthorizeResult;				// �����̳� ��Ȱ�� �޼���â �����
	unsigned long		m_dwAuthorizeCasterID;				// ������ ������ ���
	unsigned char		m_cAuthorizeCmd;					// ���� / ��Ȱ ���

	char				m_cEliteBonus;						// ����Ʈ ���ʽ�
	unsigned long		m_dwClientRace ;					// ���� ������ ����
	unsigned long		m_dwClientSex ;					// ���� ������ ����

	unsigned long		m_dwClientLoginResult ;				// �ߺ� �α�� ���� ���� �޼��� �ڽ��� �����
	unsigned long		m_dwClientExitConfirm ;				// ������ Ŭ���̾�Ʈ ���� ���� �޼��� �ڽ��� Ȯ�ΰ�
	unsigned long		m_dwExecuteExplorerConfirm ;		// ���� 3ȸ ���н� �ͽ��÷η� ���� �޼��� �ڽ��� Ȯ�ΰ�

	unsigned long		m_dwGuildWarPerTime;				// ��� ���� ���� �ð�.
	unsigned long		m_dwNationWarPerTime;				// ���� ���� ���� �ð�.

	unsigned long		m_dwRealmWarJoinResult ;			// ������ ���� ����� (���� ���� �ð� 10������ ��µǴ� ����â �����)

	unsigned long		m_dwGuildWarJoinLeaveResult ;		// ����� ���� ����� (��� ������� ���� ����)

	unsigned long		m_dwRequestResult ;					// �̹� ���� ������ �� ���� ��ȭ������ �ϰ� �ٸ� ���� ���� ������ ���� �����
	unsigned long		m_dwCounterHostilityResult ;		// ī���� ���� ���� �����
	unsigned long		m_dwTargetGID ;						// ���� ��û�� �Ϸ��� �ϴ� ��� ��� GID / ī���� ���� ��û ��� ��� GID
	unsigned long		m_dwAlreadyHostilityGID ;			// �̹� ���� ������ ��� GID
	
	BOOL				m_bUpdateMove ;						// ������Ʈ ��Ŷ�� ��� ����

	BOOL				m_bGuildSafeResult;					// �ݰ� ��Ŷ����������


	CHAR_VIEW			m_scLoginChar[ MAX_CHAR_SLOT ] ;	// �α��� ĳ���� ����(5��)
	sGuildData			m_scLoginCharGuildData[ MAX_CHAR_SLOT ] ;
	vector3				m_vecCharSelectPos[ MAX_CHAR_SLOT ] ;
	float				m_fCharSelectDir[ MAX_CHAR_SLOT ] ;

	unsigned char		m_cChangeNameCount;
	// WORK_LIST 2.1 ���� ���� �߰�
	unsigned char		m_cAccountNation;					// ���� ����
	unsigned long		m_dwSelectChr ;						// ���� ������ ĳ����
	unsigned long		m_dwClientState ;					// Ŭ���̾�Ʈ ��峻���� Ŭ���̾�Ʈ ����
	BOOL				m_bCreateChar;
	BOOL				m_bDeleteChar;
	BOOL				m_bSelectChar;
	BOOL				m_bGameStartState[ 2 ] ;

	unsigned long		m_dwFunction ;						// ��ų ���� ���̴� ����

	DWORD				m_dwSoundTickCount ;

	BOOL 				m_bShowInfo;						// ȭ�鿡 ���� ǥ�� ����
	BOOL				m_bSimpleMouse ;					// ���� ���콺 ��� (Ű���� ��忡�� ���콺 Ŀ�� ���̱�)
	BOOL				m_bSaveID ;
	BOOL				m_bFPS ;							// FPS ���� ǥ�� ����

	bool 				m_bMovieVersion;					// ���� ���
	unsigned long		m_dwViewNameType;					// ���� ȭ�� ���� ĳ���� �̸� ���߱� �ɼ�(��ü)

	// �߱��ǿ��� ���� ���� �ð��� �˷��ֱ� ���� ���
	unsigned int		m_dwPlayTime;
	unsigned int		m_dwNewTime;
	CTime				m_StartTime;
	long				m_lPlayTimer;	

	// �����̾� ���񽺸� ���� �ð��� �˷��ֱ� ���� ��
	long				m_lPremiumTime;
	int					m_iPremiumType;
	long				m_lNewPremiumTime;
	CTime				m_StartPremiumTime;
	long				m_lPlayPremiumTimer;	

	BOOL				m_bRYLTradeMode;		// ������ Ʈ������ ����϶� TRUE�� �ȴ�.
	BOOL				m_bRYLAutoTrade;		// �ڵ����� �������� �ȱ����� Ÿ��
	CItemInstance*		m_lpTradeItem;			// ���� �տ� ��� �ִ� ������
	int					m_iRYLAutoTradeNum;		// �ǸŰ���


//	BOOL				m_bRYLVaultClick ;
//	BOOL				m_bRYLBlacksmithClick ;
//	BOOL				m_bRYLTradeDlgClick ;
    BOOL				m_bRYLStreetStallDlgClick ;
	BOOL				m_bRYLHumanInventoryFrameDlgClick ;
	BOOL				m_bRYLExchangeDlgClick ;

	unsigned char		m_cTempAdminFlag;

	unsigned char		m_cTempGuildWarFlag;				// �α��νÿ� �ڱ� ũ���� ��ü�� �������� �ʾƼ�,
	unsigned char		m_cTempRealmWarFlag;				// �ӽ÷� ���� ������ �ִ� �����̴�. �ڵ峻���� ������� �ʵ���...
	unsigned char		m_cTempRealmPoint;					// �ӽ� ���� ���� ��������Ʈ.
	unsigned char		m_cTempTacticsFlag;					// �ӽ� �뺴 �÷���.

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

   unsigned char	m_cEnemyTypeResult;					// �ֱ� ���Ʊ��ĺ��� �ٰ�


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
	static unsigned long GetClientIdentity( unsigned long dwChrID ) ;											// PC���� NPC���� MONSTER���� ��ȯ�� ���� �ǹ� ����
	static unsigned long GetClientEnemyType( unsigned long dwChrID ) ;											// �Ǿƽĺ�( friend/neutral/enemy )

	static unsigned short GetEnchantLevel( unsigned long lID ) ;
	static unsigned long GetEnchantTime( unsigned long lID ) ;
	static unsigned short GetSkillIDbyChantID(unsigned char cChant);
	static unsigned long  GetClientRace();												// ���� ������ ����

	const unsigned char	GetEnemyTypeResult(void)					{ return m_cEnemyTypeResult;	}
	void				SetEnemyTypeResult(unsigned char cResult)	{ m_cEnemyTypeResult = cResult;	}

	unsigned long GetScreenPosChr(POINT &Point, int nMode, float fLength);
	BOOL	GetChangeWeapon(void);
	BOOL	CreateRaceInterface( unsigned long dwRace,unsigned long dwSex );

	VOID	UpdateEnchantTime();		// ��þƮ Ÿ���� ������Ʈ �Ѵ�.

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

	// �Ǿƽĺ�
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

