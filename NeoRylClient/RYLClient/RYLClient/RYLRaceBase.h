//=================================================================
// Name : RYLRaceBase.h
// Data : 2003. 11. 25
// Desc : ���� ���̽� Ŭ����
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
		MAX_RANKING_SPRITE	= 30,		// ��� ��������Ʈ ����
		MAX_STATUE_SPRITE	= 20		// ���� �� ���� ����� ��������Ʈ ����.
	};

	// ------------------------------------------------------------------------------------------
	// �ʱ�ȭ �޼ҵ� - RYLRaceInit.cpp

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
	// NPC ���� �˾�â �ʱ�ȭ �޼ҵ� - RYLRaceInitNPCPopup.cpp

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
	// ���� ���� �˾�â �ʱ�ȭ �޼ҵ� - RYLRaceInitSiegePopup.cpp

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
	// ������Ʈ �޼ҵ� - RYLRaceUpdate.cpp

public:

	void UpdateChrSelect(void) ;
	void UpdateGame(BOOL &bClick, BOOL &bEdge);

	// ------------------------------------------------------------------------------------------
	// ���� ���� ������Ʈ �޼ҵ� - RYLRaceUpdateFight.cpp

	void UpdateGameStillCasting( CHAR* lpszFileName ) ;
	void UpdateGameCoolDownTimeCheck( void ) ;
	void UpdateGameSiegeCoolDownTimeCheck( void ) ;

	// ------------------------------------------------------------------------------------------
	// ����Ű ���� ������Ʈ �޼ҵ� - RYLRaceUpdateAccelerator.cpp

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
	// UI ���� ������Ʈ �޼ҵ� - RYLRaceUpdateUI.cpp

	void UpdateGameEtc( float fCreatureUpdate ) ;
	void UpdateGameMsgBox( BOOL& bClick, BOOL& bEdge ) ;
	void UpdateGameInterface( BOOL& bClick, BOOL& bEdge ) ;
	void UpdateGameItem( BOOL& bClick, BOOL& bEdge ) ;
	void UpdateGameNPCPanel( void );


	// ------------------------------------------------------------------------------------------
	// ������ �޼ҵ� - RYLRaceRender.cpp

public:

	void RenderChrSelect(LPDIRECT3DDEVICE8 lpD3DDevice) ;
	void RenderGame(LPDIRECT3DDEVICE8 lpD3DDevice);
	void FinalRenderGame(LPDIRECT3DDEVICE8 lpD3DDevice);

protected: 

	void SetRenderState( LPDIRECT3DDEVICE8 lpD3DDevice ) ;
	void RenderScene( LPDIRECT3DDEVICE8 lpD3DDevice ) ;

	// ------------------------------------------------------------------------------------------
	// ���� ���� ������ �޼ҵ� - RYLRaceRenderFight.cpp

	void RenderCrossHair(LPDIRECT3DDEVICE8 lpD3DDevice, unsigned long dwMode);
	void RenderCrossHair(LPDIRECT3DDEVICE8 lpD3DDevice);
	void RenderNormalTarget( void ) ;
	void RenderSkillTarget( void ) ; 
	void RenderSkillGauge(LPDIRECT3DDEVICE8 lpD3DDevice);

	// ------------------------------------------------------------------------------------------
	// ũ���� ���� ������ �޼ҵ� - RYLRaceRenderCreature.cpp

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
	// UI ���� ������ �޼ҵ� - RYLRaceRenderUI.cpp

	void RenderStoneRespawn( LPDIRECT3DDEVICE8 lpD3DDevice ) ;
	void RenderInterface( LPDIRECT3DDEVICE8 lpD3DDevice ) ;
	void RenderMsgBox( LPDIRECT3DDEVICE8 lpD3DDevice ) ;
	void RenderEtcInterface( LPDIRECT3DDEVICE8 lpD3DDevice ) ;

	// ------------------------------------------------------------------------------------------
	// NPC ���� ��Ÿ �޼ҵ� - RYLRaceNPC.cpp

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
	// ���� ���� ��Ÿ �޼ҵ� - RYLRaceSiege.cpp

public:

	void ArmsRide( unsigned short wObjType, unsigned long dwCID, unsigned long dwArmsID ) ;		// ���� ž��
	void ArmsGetOff( unsigned long dwCID, unsigned long dwArmsID ) ;							// ���� ����
	void DestroyArms( unsigned long dwArmsID ) ;												// ���� �ı�

protected:

	BOOL CheckUsableSiegePopup( unsigned short wObjectType,
							 	unsigned long dwMyCID, unsigned long dwOwnerCID,
								unsigned long dwMyGID, unsigned long dwObjectGID,
								unsigned char cMyNation, unsigned char cObjectNation ) ;


	// ------------------------------------------------------------------------------------------
	// ��Ÿ �޼ҵ�

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

	unsigned long 		m_dwKindMarket;		// ���� �� NPC������ ����
	unsigned long 		m_dwKindPos;		// �˾��� ���� 0 = ĳ���� ���� �ٴ� �˾� 1 = ���콺 Ŀ���� �ٴ� �˾�
	unsigned long 		m_dwPopupSelect;	// ������ �˾� �޴� ���ؽ�

	vector<PopupMenu>	m_lstPopupMenu;		// �˾� ��� ����Ʈ
	int 				m_nPopupWindowX;	// �˾��� X��ǥ
	int 				m_nPopupWindowY;	// �˾��� Y��ǥ

	unsigned long 		m_dwNormalTargetChrID;			// ���� �븻 Ÿ��(�Ϲ� ����)
	unsigned long 		m_dwSpecialTargetChrID;			// ���� ��ų Ÿ��
	unsigned long 		m_dwRangedWeaponTargetChrID;	// ��Ÿ� Ÿ��

	unsigned long 		m_dwSpecialTargetChrIDTemp;			// ���� ��ų Ÿ��

	unsigned long		m_dwPopupWindowLength;			// �˾��� ���� ����
	BOOL				m_bPopupShow;					// �˾��� ǥ�� ����
	unsigned long 		m_dwPopupSelectChrID;			// �˾��� ��� ĳ����
	unsigned long 		m_dwSiegeArmsSelectID ;			// �������� �������� Ÿ�� ID
	unsigned long 		m_dwKindPopup;

	vector<CTexture *>		m_lstTexture;		// �������̽����� ���̴� �ؽ���
	vector<CGUIWindow *>	m_lstWindow;		// �������̽����� ���̴� ������

	BOOL 	m_bQuickRideExchange;				// ��Ÿ�� ��ȯ ���� ���� ����

	BOOL 	m_bQuickWeaponExchange;				// ���� ��ȯ ���� ���� ����
	BOOL 	m_bWeaponExchageAble;				// ���� ��ȯ ���� ���� ����
	float	m_fWeaponExchageAbleTimeCounter;	// ���� ��ȯ ���� ���� ����

	BOOL			m_bExchange;				// �ŷ� ����
	unsigned long 	m_dwExchangeCmdResult;		// �ŷ� ����
	unsigned long 	m_dwExchangeCmdSenderID;	// �ŷ� ����
	unsigned long 	m_dwExchangeCmdType;		// �ŷ� ����

	unsigned long 	m_dwDuelCmdResult;			// ��� ����
	unsigned long 	m_dwDuelCmdSenderID;		// ��� ����
	unsigned long 	m_dwDuelCmdType;			// ��� ����

	unsigned long 	m_dwPartyDuelCmdResult;		// ��Ƽ��� ����
	unsigned long 	m_dwPartyDuelCmdSenderID;	// ��Ƽ��� ����
	unsigned long 	m_dwPartyDuelCmdType;		// ��Ƽ��� ����

	unsigned long 	m_dwPartyCmdResult;			// ��Ƽ ����
	unsigned long 	m_dwPartyCmdType;			// ��Ƽ ����
	unsigned long 	m_dwPartyCmdSenderID;		// ��Ƽ ����

	unsigned long 	m_dwTacticsReq;				// �뺴 ����.

	unsigned long	m_dwLevelUp;

	int				m_iGateW;					// ĳ���� ������ ��ġ
	int				m_iGateH;

	CRYLSprite*		m_lpSkillGaugeFrame;			// ĳ��Ʈ ��ų ������ ������
	CRYLSprite*		m_lpSkillGauge[5];				// ĳ��Ʈ ��ų ������(������)
	CRYLSprite*		m_lpTarget[2];
	CRYLSprite*		m_lpHPGauge;					// ĳ���� �Ӹ����� �ߴ� HP������
	CRYLSprite*		m_lpQuiver;						// ȭ�� ���ϴܿ� ������ ȭ�� ���� ǥ��
	CRYLSprite*		m_lpQuiverD;					// ȭ�� ���ϴܿ� ������ ȭ�� ���� ǥ��
	CRYLSprite*		m_lpMark[MAX_RANKING_SPRITE];	// �� ������ �����

	CRYLSprite*		m_lpStatue[MAX_STATUE_SPRITE];	// ���� �� ��������� �̹���.

	CTexture*		m_lpRealmPointTexture;			// �������� �������� �ؽ���.
	CRYLSprite*		m_lpRealmPoint;					// �������� ��������.

	CTexture*		m_lpTacticsMarkTexture;			// �뺴 ��ũ �ؽ���.
	CRYLSprite*		m_lpTacticsMark;				// �뺴 ��ũ.

	CTexture*		m_lpChrButtonTexture;			// ĳ���� �̸��� �ǳ�
	CTexture*		m_lpPopupTexture;				// �˾� �ǳ�
	CTexture*		m_lpSocket;						// ������ ������ ��� ������ ������ ������
	CTexture*		m_lpCommonWindow;
	CTexture*		m_lpMarkIcon;					// ����� ������ �ؽ���

	unsigned long	m_dwSpendTime[6];

	BOOL 			m_bClickButton;					// ĳ���� Ŭ�� ����
	BOOL 			m_bShowHelp;					// ���� ǥ�� ����
	BOOL 			m_bShowSkillError;				// ��ų ������ ���� ǥ�� ���� (���콺 ��带 ����)

	// ------------------------------------------------------------------------------------------
	// �������� �޽����ڽ�

	DWORD		m_dwCastleDoorUpgradeResult ;	// ���� ���׷��̵�
	DWORD		m_dwCastleDoorRepairResult ;	// ���� ����
	DWORD		m_dwCastleDoorRecoveryResult ;	// ���� ����

	DWORD		m_dwArmsType ;					// ���� �Ϸ��� �������� Ÿ��
	DWORD		m_dwCastleArmsDevelopeResult ;	// �������� ����
	DWORD		m_dwCastleArmsRideResult ;		// �������� ž��
	DWORD		m_dwCastleArmsRepairResult ;	// �������� ����
	DWORD		m_dwCastleArmsUpgradeResult;	// �������� ���׷��̵�
	DWORD		m_dwCastleArmsDestroyResult ;	// �������� �ı�

	DWORD		m_dwCampCreateResult ;			// ���� ����
	DWORD		m_dwCampCareResult ;			// ���� �����ϱ�
	DWORD		m_dwCampRepairResult ;			// ���� ����
	DWORD		m_dwCampDestoryResult ;			// ���� �ı�
	DWORD		m_dwWorldWeaponCreateResult ;	// ���� ���� ����
	DWORD		m_dwWorldWeaponDestroyResult ;	// ���� ���� �ı�
				
	DWORD		m_dwSiegeArmsRideResult;				// ���� ���� ž��
	DWORD		m_dwSiegeArmsRepair;					// ���� ���� ����
	DWORD		m_dwSiegeArmsStartKitRecoveryResult ;	// �������� ��Ÿ��Ŷ���� ����
	DWORD		m_dwCampRepairPrice ;					// ���� �������

	// WORK_LIST 2.3 ���� ���� ���� ��� ����
	DWORD		m_dwChangeNationResult ;		// ���� ���� ���� ����
	
	long		m_nPreSelectItem;	

	std::map<unsigned long,int>	m_mapQuestExclamation;
	std::map<unsigned long,int>	m_mapQuestQuestion;

	CEffScript*	m_pCastingEffect;
};

#endif //__RYL_RACEBASE_H__

