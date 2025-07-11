//=================================================================
// Name : RYLRaceBase.h
// Data : 2003. 11. 25
// Desc : 종족 베이스 클래스
//================================================================= 
#ifndef __RYL_RACEBASE_H__
#define __RYL_RACEBASE_H__

#include <vector>
#include <list>
#include <Texture.h>
#include <VECTOR.h>
#include "..\\GUIWindow.h"

#include <map>

using namespace std;

#ifdef __NEWNPCSCRIPT
	struct JobNode;

	struct PopupMenu
	{
		PopupMenu(void)
		{
			m_dwPopupKind = 0;
			m_dwQuestID = 0;
			m_dwColor = D3DCOLOR_RGBA(255, 255, 255, 255);
			m_dwLimitLevel = 0;
			m_dwLimitFame = 0;
			m_dwLimitQuest = 0;
		}

		unsigned long	m_dwPopupKind;
		char			m_strPopupStr[MAX_PATH];
		unsigned long	m_dwQuestID;
		unsigned long	m_dwColor;
		unsigned long	m_dwLimitLevel;
		unsigned long	m_dwLimitFame;
		unsigned long	m_dwLimitQuest;

		JobNode *m_lpJobNode;
	};
#else
	struct PopupMenu
	{
		PopupMenu(void)
		{
			m_dwPopupKind = 0;
			m_dwQuestID = 0;
			m_dwColor = D3DCOLOR_RGBA(255, 255, 255, 255);
		}

		unsigned long	m_dwPopupKind;
		char			m_strPopupStr[MAX_PATH];
		unsigned long	m_dwQuestID;
		unsigned long	m_dwColor;
		unsigned short  m_usCapacity;
	};
#endif

struct CharDataNode;
struct NPCNode;

class RYLCreature;
class CItemInstance;
class CEffScript ;

class CRYLRaceBase
{
public:

	enum Const
	{
		MAX_RANKING_SPRITE	= 30,		// 계급 스프라이트 갯수
		MAX_STATUE_SPRITE	= 20		// 석상 및 생명 추출기 스프라이트 갯수.
	};

	// ------------------------------------------------------------------------------------------
	// 초기화 메소드 - RYLRaceInit.cpp

public:

	unsigned long  CheckOCTREE();
	//a method that checks the version of the client and loads all of the gsf and mcf files, and loads everything that the zone needs(music, loading screen and etc..).
	BOOL InitZone(unsigned long dwZoneNumber);
	BOOL InitChrSelect(void);
	//a method that handle the character load to the game[load the look,information,last status(dead or alive,position) and etc]
	BOOL InitGame(void) ;

	void InitPos(CItemInstance *lpItem) ;
	void InitStreetStall(unsigned long dwChrID, char *lpstrStreetName) ;
#ifdef __NEWNPCSCRIPT
	void InitTrade( unsigned long ulNPCID, unsigned long dwContainerID );
#else
	void InitTrade( unsigned long ulNPCID, unsigned char ucTradeDlgIndex, unsigned short usCapacity );
#endif

protected:

	void InitQuest(unsigned long dwChrID, unsigned long dwQuestID) ;
	void InitItemUpgrade(unsigned long dwChrID) ;
	void InitItemComposition(unsigned long dwChrID) ;
	void InitCompensation(void) ;
	void InitDeposit(void) ;
	void InitStatusRetrain(unsigned char cState) ;

	VOID InitClassChange(ULONG dwNPCID) ;

	// ------------------------------------------------------------------------------------------
	// NPC 관련 팝업창 초기화 메소드 - RYLRaceInitNPCPopup.cpp

	VOID InitPopupPC( PopupMenu* pmAddMenu ) ;
#ifndef __NEWNPCSCRIPT
	VOID InitPopupGuard( PopupMenu* pmAddMenu ) ;
	VOID InitPopupNormal( PopupMenu* pmAddMenu ) ;
	VOID InitPopupNoBind( PopupMenu* pmAddMenu ) ;
	VOID InitPopupMerchant( PopupMenu* pmAddMenu ) ;
	VOID InitPopup2KindMerchant( PopupMenu* pmAddMenu ) ;
	VOID InitPopupCounseller( PopupMenu* pmAddMenu ) ;
	VOID InitPopupBlackSmith( PopupMenu* pmAddMenu ) ;
	VOID InitPopupAdventureTrainer( PopupMenu* pmAddMenu ) ;
	VOID InitPopupFigtherTrainer( PopupMenu* pmAddMenu ) ;
	VOID InitPopupRogueTrainer( PopupMenu* pmAddMenu ) ;
	VOID InitPopupMageTrainer( PopupMenu* pmAddMenu ) ;
	VOID InitPopupAcolyteTrainer( PopupMenu* pmAddMenu ) ;
	VOID InitPopupGuildMaker( PopupMenu* pmAddMenu ) ;
	VOID InitPopupTeleporter( PopupMenu* pmAddMenu, DWORD dwUID ) ;
	VOID InitPopupContainer( PopupMenu* pmAddMenu ) ;
	VOID InitPopupPeaceGuild( PopupMenu* pmAddMenu ) ;
	VOID InitPopupStatuscleaner( PopupMenu* pmAddMenu ) ;
	VOID InitPopupClasscleaner( PopupMenu* pmAddMenu ) ;
	VOID InitPopupCompensation( PopupMenu* pmAddMenu ) ;
	VOID InitPopupSkillMerchant( PopupMenu* pmAddMenu ) ;	
	VOID InitPopupBattleGroundManager( PopupMenu* pmAddMenu );
	VOID InitPopupMoneyChanger( PopupMenu* pmAddMenu );
	VOID InitPopupBattleGroundExiter( PopupMenu* pmAddMenu );
	VOID InitPopupChangeName( PopupMenu* pmAddMenu );
#endif
	VOID InitPopupQuest( PopupMenu* pmAddMenu, DWORD dwUID ) ;

	//-- 2004. 9. 8. Zergra From. --//
	BOOL InitNPCPopup( NPCNode* pNpc, unsigned long dwTargetID );
	BOOL ProcessPopup( NPCNode* pNpc, PopupMenu* pMenu );
	//-- Zergra To. --// 

	// ------------------------------------------------------------------------------------------
	// 공성 관련 팝업창 초기화 메소드 - RYLRaceInitSiegePopup.cpp

	VOID InitPopupEmblem( PopupMenu* pmAddMenu ) ;
	VOID InitPopupGate( PopupMenu* pmAddMenu ) ;		
	VOID InitPopupBackDoor( PopupMenu* pmAddMenu ) ;
	VOID InitPopupCamp( PopupMenu* pmAddMenu ) ;
	VOID InitPopupCastleArmsNPC( PopupMenu* pmAddMenu ) ;
	VOID InitPopupCastleGuard( PopupMenu* pmAddMenu ) ;
	VOID InitPopupShortRangeCastleArms( PopupMenu* pmAddMenu ) ;
	VOID InitPopupLongRangeCastleArms( PopupMenu* pmAddMenu ) ;
	VOID InitPopupShortRangeSiegeArms( PopupMenu* pmAddMenu ) ;
	VOID InitPopupLongRangeSiegeArms( PopupMenu* pmAddMenu ) ;
	VOID InitPopupAirShip( PopupMenu* pmAddMenu ) ;
	VOID InitPopupCampMining( PopupMenu* pmAddMenu ) ;
	VOID InitPopupCampShop( PopupMenu* pmAddMenu ) ;
	VOID InitPopupWorldWeapon( PopupMenu* pmAddMenu ) ;


	// ------------------------------------------------------------------------------------------
	// 업데이트 메소드 - RYLRaceUpdate.cpp

public:

	void UpdateChrSelect(void) ;
	void UpdateGame(BOOL &bClick, BOOL &bEdge);

	// ------------------------------------------------------------------------------------------
	// 전투 관련 업데이트 메소드 - RYLRaceUpdateFight.cpp

	void UpdateGameStillCasting( CHAR* lpszFileName ) ;
	void UpdateGameCoolDownTimeCheck( void ) ;
	void UpdateGameSiegeCoolDownTimeCheck( void ) ;

	// ------------------------------------------------------------------------------------------
	// 단축키 관련 업데이트 메소드 - RYLRaceUpdateAccelerator.cpp

	void UpdateGameAccelerator(void) ;
	void UpdateGame_W_KeyDown(void) ;
	//void UpdateGame_AltTab_KeyDown(void) ;//hz added this for Alt+Tab disable
	void UpdateGame_NUMPAD_KeyDown(void) ;
	void UpdateGame_TAB_KeyDown( BOOL &bClick, BOOL &bEdge ) ;
	void UpdateGame_F_KeyDown(void) ;
	void UpdateGame_R_KeyDown(void) ;
	void UpdateGameAltnG( BOOL& bEdge ) ;
	void UpdateGame_ForMiniMap_KeyHold(void) ;
	void UpdateGameHelp( float fCreatureUpdate ) ;


	void CloseInterface(void);

	// ------------------------------------------------------------------------------------------
	// UI 관련 업데이트 메소드 - RYLRaceUpdateUI.cpp

	void UpdateGameEtc( float fCreatureUpdate ) ;
	void UpdateGameMsgBox( BOOL& bClick, BOOL& bEdge ) ;
	void UpdateGameInterface( BOOL& bClick, BOOL& bEdge ) ;
	void UpdateGameItem( BOOL& bClick, BOOL& bEdge ) ;
	void UpdateGameNPCPanel( void );


	// ------------------------------------------------------------------------------------------
	// 렌더링 메소드 - RYLRaceRender.cpp

public:

	void RenderChrSelect(LPDIRECT3DDEVICE8 lpD3DDevice) ;
	void RenderGame(LPDIRECT3DDEVICE8 lpD3DDevice);
	void FinalRenderGame(LPDIRECT3DDEVICE8 lpD3DDevice);

protected: 

	void SetRenderState( LPDIRECT3DDEVICE8 lpD3DDevice ) ;
	void RenderScene( LPDIRECT3DDEVICE8 lpD3DDevice ) ;

	// ------------------------------------------------------------------------------------------
	// 전투 관련 렌더링 메소드 - RYLRaceRenderFight.cpp

	void RenderCrossHair(LPDIRECT3DDEVICE8 lpD3DDevice, unsigned long dwMode);
	void RenderCrossHair(LPDIRECT3DDEVICE8 lpD3DDevice);
	void RenderNormalTarget( void ) ;
	void RenderSkillTarget( void ) ; 
	void RenderSkillGauge(LPDIRECT3DDEVICE8 lpD3DDevice);

	// ------------------------------------------------------------------------------------------
	// 크리쳐 관련 렌더링 메소드 - RYLRaceRenderCreature.cpp

	void RenderStreetStallButton(LPDIRECT3DDEVICE8 lpD3DDevice, RYLCreature* pCreature, char *strText, short wPosX, short wPosY);
	void RenderChrButton(LPDIRECT3DDEVICE8 lpD3DDevice, RYLCreature* pCreature, char *strText, short wPosX, short wPosY);
	void RenderChrButton(LPDIRECT3DDEVICE8 lpD3DDevice, RYLCreature* pCreature, unsigned long dwUID, short wPosX, short wPosY, unsigned long dwColor);
	void RenderPopUp(LPDIRECT3DDEVICE8 lpD3DDevice );
	void RenderHPBar(LPDIRECT3DDEVICE8 lpD3DDevice, CharDataNode* pData, short wPosX, short wPosY, unsigned long ulBackColor, unsigned long ulHPColor );
	void RenderChrName(LPDIRECT3DDEVICE8 lpD3DDevice, RYLCreature* pCreature, short wPosX, short wPosY);
	void RenderHead( LPDIRECT3DDEVICE8 lpD3DDevice ) ;
	void RenderPC( LPDIRECT3DDEVICE8 lpD3DDevice, RYLCreature* pCreature, vector3 vecResult, vector3 vecDamageResult ) ;
	void RenderNPC( LPDIRECT3DDEVICE8 lpD3DDevice, RYLCreature* pCreature, vector3 vecResult, vector3 vecDamageResult ) ; 
	void RenderMonster( LPDIRECT3DDEVICE8 lpD3DDevice, RYLCreature* pCreature, vector3 vecResult, vector3 vecDamageResult ) ;
	void RenderSiege( LPDIRECT3DDEVICE8 lpD3DDevice, RYLCreature* pCreature, vector3 vecResult, vector3 vecDamageResult ) ;

	// ------------------------------------------------------------------------------------------
	// UI 관련 렌더링 메소드 - RYLRaceRenderUI.cpp

	void RenderStoneRespawn( LPDIRECT3DDEVICE8 lpD3DDevice ) ;
	void RenderInterface( LPDIRECT3DDEVICE8 lpD3DDevice ) ;
	void RenderMsgBox( LPDIRECT3DDEVICE8 lpD3DDevice ) ;
	void RenderEtcInterface( LPDIRECT3DDEVICE8 lpD3DDevice ) ;

	// ------------------------------------------------------------------------------------------
	// NPC 관련 기타 메소드 - RYLRaceNPC.cpp

protected: 

#ifdef __NEWNPCSCRIPT
	BOOL InitPopup(unsigned long dwUID, unsigned long dwKindPos, unsigned long dwKindPopup = 1);
#else
	BOOL InitPopup(unsigned long dwKindPopup, unsigned long dwUID, unsigned long dwKindPos);
#endif
	void InitPopupPosition(void) ;

	BOOL SelectNPC(unsigned long dwTargetID, BOOL bButton = FALSE);
	BOOL UpdatePopup(void);
	void NPCSpawn(void);

	void DisablePopupMenuKind(unsigned long dwSrcKind, unsigned long dwResultKind);


	// ------------------------------------------------------------------------------------------
	// 공성 관련 기타 메소드 - RYLRaceSiege.cpp

public:

	void ArmsRide( unsigned short wObjType, unsigned long dwCID, unsigned long dwArmsID ) ;		// 병기 탑승
	void ArmsGetOff( unsigned long dwCID, unsigned long dwArmsID ) ;							// 병기 내림
	void DestroyArms( unsigned long dwArmsID ) ;												// 병기 파괴

protected:

	BOOL CheckUsableSiegePopup( unsigned short wObjectType,
							 	unsigned long dwMyCID, unsigned long dwOwnerCID,
								unsigned long dwMyGID, unsigned long dwObjectGID,
								unsigned char cMyNation, unsigned char cObjectNation ) ;


	// ------------------------------------------------------------------------------------------
	// 기타 메소드

public:

	CRYLRaceBase();
	virtual ~CRYLRaceBase();

	BOOL GetIsTrade(void) ;
	BOOL GetBusy(void) ;

	void ProcessExchangeCmd(unsigned long dwSenderID, unsigned short wCmd) ;
	void HideInterfaceforDead(void) ;

	static bool PickItem(DWORD nSelectItem);

	BOOL CheckSocialAction(char* token);

protected:

	void DestroyAllObject(void);
	void ProcessSkill(void);
	void ChangeKeyboardMode(void);

	BOOL GetIsExchange(void);

	// ------------------------------------------------------------------------------------------


public:

	unsigned long 		m_dwKindMarket;		// 현재 들어간 NPC상점의 종류
	unsigned long 		m_dwKindPos;		// 팝업의 종류 0 = 캐릭터 몸에 붙는 팝업 1 = 마우스 커서에 붙는 팝업
	unsigned long 		m_dwPopupSelect;	// 선택한 팝업 메뉴 인텍스

	vector<PopupMenu>	m_lstPopupMenu;		// 팝업 명령 리스트
	int 				m_nPopupWindowX;	// 팝업의 X좌표
	int 				m_nPopupWindowY;	// 팝업의 Y좌표

	unsigned long 		m_dwNormalTargetChrID;			// 현재 노말 타겟(일반 공격)
	unsigned long 		m_dwSpecialTargetChrID;			// 현재 스킬 타겟
	unsigned long 		m_dwRangedWeaponTargetChrID;	// 장거리 타겟

	unsigned long 		m_dwSpecialTargetChrIDTemp;			// 현재 스킬 타겟

	unsigned long		m_dwPopupWindowLength;			// 팝업의 가로 길이
	BOOL				m_bPopupShow;					// 팝업의 표시 여부
	unsigned long 		m_dwPopupSelectChrID;			// 팝업의 대상 캐릭터
	unsigned long 		m_dwSiegeArmsSelectID ;			// 공성병기 구축해제 타겟 ID
	unsigned long 		m_dwKindPopup;

	vector<CTexture *>		m_lstTexture;		// 인터페이스에서 쓰이는 텍스쳐
	vector<CGUIWindow *>	m_lstWindow;		// 인터페이스에서 쓰이는 윈도우

	BOOL 	m_bQuickRideExchange;				// 말타기 전환 가능 유무 관련

	BOOL 	m_bQuickWeaponExchange;				// 무기 전환 가능 유무 관련
	BOOL 	m_bWeaponExchageAble;				// 무기 전환 가능 유무 관련
	float	m_fWeaponExchageAbleTimeCounter;	// 무기 전환 가능 유무 관련

	BOOL			m_bExchange;				// 거래 관련
	unsigned long 	m_dwExchangeCmdResult;		// 거래 관련
	unsigned long 	m_dwExchangeCmdSenderID;	// 거래 관련
	unsigned long 	m_dwExchangeCmdType;		// 거래 관련

	unsigned long 	m_dwDuelCmdResult;			// 듀얼 관련
	unsigned long 	m_dwDuelCmdSenderID;		// 듀얼 관련
	unsigned long 	m_dwDuelCmdType;			// 듀얼 관련

	unsigned long 	m_dwPartyDuelCmdResult;		// 파티듀얼 관련
	unsigned long 	m_dwPartyDuelCmdSenderID;	// 파티듀얼 관련
	unsigned long 	m_dwPartyDuelCmdType;		// 파티듀얼 관련

	unsigned long 	m_dwPartyCmdResult;			// 파티 관련
	unsigned long 	m_dwPartyCmdType;			// 파티 관련
	unsigned long 	m_dwPartyCmdSenderID;		// 파티 관련

	unsigned long 	m_dwTacticsReq;				// 용병 관련.

	unsigned long	m_dwLevelUp;

	int				m_iGateW;					// 캐스팅 게이지 위치
	int				m_iGateH;

	CRYLSprite*		m_lpSkillGaugeFrame;			// 캐스트 스킬 게이지 프레임
	CRYLSprite*		m_lpSkillGauge[5];				// 캐스트 스킬 게이지(레벨별)
	CRYLSprite*		m_lpTarget[2];
	CRYLSprite*		m_lpHPGauge;					// 캐릭터 머리위에 뜨는 HP게이지
	CRYLSprite*		m_lpQuiver;						// 화면 우하단에 나오는 화살 부족 표시
	CRYLSprite*		m_lpQuiverD;					// 화면 우하단에 나오는 화살 부족 표시
	CRYLSprite*		m_lpMark[MAX_RANKING_SPRITE];	// 각 종족의 계급장

	CRYLSprite*		m_lpStatue[MAX_STATUE_SPRITE];	// 석상 및 생명추출기 이미지.

	CTexture*		m_lpRealmPointTexture;			// 국가전쟁 공헌훈장 텍스쳐.
	CRYLSprite*		m_lpRealmPoint;					// 국가전쟁 공헌훈장.

	CTexture*		m_lpTacticsMarkTexture;			// 용병 마크 텍스쳐.
	CRYLSprite*		m_lpTacticsMark;				// 용병 마크.

	CTexture*		m_lpChrButtonTexture;			// 캐릭터 이름의 판넬
	CTexture*		m_lpPopupTexture;				// 팝업 판넬
	CTexture*		m_lpSocket;						// 툴팁에 나오는 장비에 박히는 소켓의 프레임
	CTexture*		m_lpCommonWindow;
	CTexture*		m_lpMarkIcon;					// 계급장 아이콘 텍스쳐

	unsigned long	m_dwSpendTime[6];

	BOOL 			m_bClickButton;					// 캐릭터 클릭 유무
	BOOL 			m_bShowHelp;					// 도움말 표시 여부
	BOOL 			m_bShowSkillError;				// 스킬 시전시 에러 표시 유무 (마우스 모드를 위해)

	// ------------------------------------------------------------------------------------------
	// 공성관련 메시지박스

	DWORD		m_dwCastleDoorUpgradeResult ;	// 성문 업그레이드
	DWORD		m_dwCastleDoorRepairResult ;	// 성문 수리
	DWORD		m_dwCastleDoorRecoveryResult ;	// 성문 복구

	DWORD		m_dwArmsType ;					// 개발 하려는 수성병기 타입
	DWORD		m_dwCastleArmsDevelopeResult ;	// 수성병기 개발
	DWORD		m_dwCastleArmsRideResult ;		// 수성병기 탑승
	DWORD		m_dwCastleArmsRepairResult ;	// 수성병기 수리
	DWORD		m_dwCastleArmsUpgradeResult;	// 수성병기 업그레이드
	DWORD		m_dwCastleArmsDestroyResult ;	// 수성병기 파괴

	DWORD		m_dwCampCreateResult ;			// 진지 생성
	DWORD		m_dwCampCareResult ;			// 진지 손질하기
	DWORD		m_dwCampRepairResult ;			// 진지 수리
	DWORD		m_dwCampDestoryResult ;			// 진지 파괴
	DWORD		m_dwWorldWeaponCreateResult ;	// 월드 웨폰 생성
	DWORD		m_dwWorldWeaponDestroyResult ;	// 월드 웨폰 파괴
				
	DWORD		m_dwSiegeArmsRideResult;				// 공성 병기 탑승
	DWORD		m_dwSiegeArmsRepair;					// 공성 병기 수리
	DWORD		m_dwSiegeArmsStartKitRecoveryResult ;	// 공성병기 스타터킷으로 복구
	DWORD		m_dwCampRepairPrice ;					// 진지 수리비용

	// WORK_LIST 2.3 계정 국적 변경 기능 구현
	DWORD		m_dwChangeNationResult ;		// 계정 국적 변경 관련
	
	long		m_nPreSelectItem;	

	std::map<unsigned long,int>	m_mapQuestExclamation;
	std::map<unsigned long,int>	m_mapQuestQuestion;

	CEffScript*	m_pCastingEffect;
};

#endif //__RYL_RACEBASE_H__

