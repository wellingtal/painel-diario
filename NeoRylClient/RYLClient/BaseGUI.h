// BaseGUI.h: interface for the CBaseGUI class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BASEGUI_H__3E9B914A_483F_4DBB_A374_227D1CAAC19F__INCLUDED_)
#define AFX_BASEGUI_H__3E9B914A_483F_4DBB_A374_227D1CAAC19F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include <list>
#include <Texture.h>
#include "GUIWindow.h"

using namespace std;

// 각 NPC들의 직업ID
#define POPUP_NORMAL 1
#define POPUP_GUARD 2
#define POPUP_NOBIND 3

#define POPUP_WEAPONMERCHANT 11
#define POPUP_ARMOURMERCHANT 12
#define POPUP_ETCMERCHANT 13

#define POPUP_WEAPONNARMOURMERCHANT 14

#define POPUP_DARKWEAPONMERCHANT 16
#define POPUP_DARKARMOURMERCHANT 17
#define POPUP_DARKETCMERCHANT 18

#define POPUP_BLACKSMITH 21

#define POPUP_COUNSELLER 26
#define POPUP_MEDALSHOP 27

#define POPUP_ADVENTURETRAINER 31
#define POPUP_FIGHTERTRAINER 32
#define POPUP_ROGUETRAINER 33
#define POPUP_MAGETRAINER 34
#define POPUP_ACOLYTETRAINER 35

//================================
// by Ichabod
// desc : 휴먼 길드 생성 NPC
#define POPUP_GUILDMAKER     36

#define POPUP_TELEPORTER 41
#define POPUP_PEACEGUILD 42
#define POPUP_CONTAINER 43

#define POPUP_STATUSCLEANER 44
#define POPUP_CLASSCLEANER 45

#define POPUP_A_NORMAL 51
#define POPUP_A_GUARD 52
#define POPUP_A_NOBIND 53

#define POPUP_A_WEAPONMERCHANT 61
#define POPUP_A_ARMOURMERCHANT 62
#define POPUP_A_ETCMERCHANT 63

#define POPUP_A_DARKWEAPONMERCHANT 66
#define POPUP_A_DARKARMOURMERCHANT 67
#define POPUP_A_DARKETCMERCHANT 68

#define POPUP_A_BLACKSMITH 71

#define POPUP_A_COUNSELLER 76
#define POPUP_A_MEDALSHOP 77

#define POPUP_A_BEGINNERTRAINER 81
#define POPUP_A_COMBATANTTRAINER 82
#define POPUP_A_OPPICATERTRAINER 83
//================================
// by Ichabod
// desc : 아칸 길드 생성 NPC
#define POPUP_A_GUILDMAKER     86

#define POPUP_A_TELEPORTER 91
#define POPUP_A_PEACEGUILD 92
#define POPUP_A_CONTAINER 93

#define POPUP_A_STATUSCLEANER 94
#define POPUP_A_CLASSCLEANER 95

#define POPUP_PC 100
#define POPUP_LEADER 101
#define POPUP_MEMBER 102


// 팝업 메뉴의 종류
#define POPUPMENU_EXCHANGE			0
#define POPUPMENU_DUEL				1
#define POPUPMENU_PARTYINVATE		2
#define POPUPMENU_STREETSTALL		3
#define POPUPMENU_LEADER			4
#define POPUPMENU_VANISH			5
#define POPUPMENU_DIALOG			6
#define POPUPMENU_SAVEPOS			7
#define POPUPMENU_TRADE				8
#define POPUPMENU_UPGRADE			9
#define POPUPMENU_UNLOCK			10
#define POPUPMENU_JOBCHANGE			11
#define POPUPMENU_ZONEMOVE			12
#define POPUPMENU_DIALOG2			13
#define POPUPMENU_DEPOSIT			14
#define POPUPMENU_PARTYDUEL			15
#define POPUPMENU_QUEST				16
#define POPUPMENU_FRIEND			17
#define POPUPMENU_BAN				18
#define POPUPMENU_PEACE				19
#define POPUPMENU_FOLLOW			20
#define POPUPMENU_STATUSCLEAN		21
#define POPUPMENU_CLASSCLEAN		22
// by Ichabod
// 길드 관련
#define POPUPMENU_GUILDINVITE		23
#define POPUPMENU_MAKEGUILD			24

#define POPUPMENU_STATUSRETRAIN		25

// 캐릭터 선택시의 상태
#define CHRSELECT_SELECT		0
#define CHRSELECT_START			1
#define CHRSELECT_CREATE		2
#define CHRSELECT_INITSELECT	3
#define CHRSELECT_INITSTART		4
#define CHRSELECT_INITCREATE	5

// 게임의 상태
#define GAME_LOADING			6
#define GAME_PLAYING			7
#define GAME_END				8

#define CHRSELECT_SELECTCHANNEL 9

const unsigned long WINDOW_MINISTATUS = 0;
const unsigned long WINDOW_CHAT = 1;
const unsigned long WINDOW_SHORTCUT = 2;
const unsigned long WINDOW_STATUS = 3;
const unsigned long WINDOW_SKILL = 4;
const unsigned long WINDOW_INVENTORY = 5;
const unsigned long WINDOW_SOCEITY = 6;
const unsigned long WINDOW_OPTION = 7;
const unsigned long WINDOW_SHOP = 8;
const unsigned long WINDOW_MINIMAP = 9;
const unsigned long WINDOW_QUICKSLOT = 10;
const unsigned long WINDOW_SIMPLEMODE = 11;
const unsigned long WINDOW_MINIPARTY = 12;
const unsigned long WINDOW_UPGRADE = 13;
const unsigned long WINDOW_JOBCHANGE = 14;
const unsigned long WINDOW_SIMPLECHAT = 15;
const unsigned long WINDOW_EXCHANGE = 16;
const unsigned long WINDOW_CHANNEL = 17;
const unsigned long WINDOW_RESPAWN = 18;
const unsigned long WINDOW_VAULT = 19;
const unsigned long WINDOW_STREETSTALL = 20;
const unsigned long WINDOW_QUESTDESC = 21;
const unsigned long WINDOW_QUEST = 22;
const unsigned long WINDOW_RANK = 23;
const unsigned long WINDOW_STATUSINIT = 24;
// by ichabod
const unsigned long WINDOW_STONERESPAWN			= 25 ;
const unsigned long WINDOW_STONESTATUE			= 26 ;
const unsigned long WINDOW_GUILDMAIN			= 27 ;
const unsigned long WINDOW_GUILDINFO			= 28 ;
const unsigned long WINDOW_GUILDBOARD			= 29 ;
const unsigned long WINDOW_GUILDPROPERTY		= 30 ;
const unsigned long WINDOW_GUILDMEMBERS			= 31 ;
const unsigned long WINDOW_GUILDCASHBOX			= 32 ;
const unsigned long WINDOW_GUILDSTOREHOUSE		= 33 ;
const unsigned long WINDOW_GUILDAUTHORITY		= 34 ;
const unsigned long WINDOW_GUILDMARK			= 35 ;
const unsigned long WINDOW_GENERALGUILDPOPUP	= 36 ;
const unsigned long WINDOW_PROPOSALGUILDPOPUP	= 37 ;
// by chajaeil
const unsigned long WINDOW_STATUSRETRAIN		= 38;

typedef struct PopupMenu
{
	PopupMenu(void)
	{
		m_dwPopupKind = 0;
		m_dwQuestID = 0;
		m_dwColor = D3DCOLOR_RGBA(255, 255, 255, 255);
	}

	unsigned long m_dwPopupKind;
	char m_strPopupStr[MAX_PATH];
	unsigned long m_dwQuestID;
	unsigned long m_dwColor;
} PopupMenu;

class CCreature ;
class CItemInstance ;


class CBaseGUI  
{
	public:
		void RenderChant(LPDIRECT3DDEVICE8 lpD3DDevice);
		BOOL InitZone(unsigned long dwZoneNumber);
		void ProcessSkill(void);
		void RenderCrossHair(LPDIRECT3DDEVICE8 lpD3DDevice, unsigned long dwMode);
		
		static void PickItem(DWORD nSelectItem);
		void RenderChrName(LPDIRECT3DDEVICE8 lpD3DDevice, unsigned long dwUID, short wPosX, short wPosY);
		void RenderSkillGauge(LPDIRECT3DDEVICE8 lpD3DDevice);
		BOOL SelectNPC(unsigned long dwTargetID, BOOL bButton = FALSE);
		void NPCSpawn(void);
		BOOL UpdatePopup(void);

		unsigned long m_dwKindMarket;		// 현재 들어간 NPC상점의 종류
		unsigned long m_dwKindPos;			// 팝업의 종류 0 = 캐릭터 몸에 붙는 팝업 1 = 마우스 커서에 붙는 팝업
		unsigned long m_dwPopupSelect;		// 선택한 팝업 메뉴 인텍스

		vector<PopupMenu> m_lstPopupMenu;	// 팝업 명령 리스트
//		char m_strPopupMenu[6][MAX_PATH];
		int m_nPopupWindowX;				// 팝업의 X좌표
		int m_nPopupWindowY;				// 팝업의 Y좌표

		unsigned long m_dwNormalTargetChrID;		// 현재 노말 타겟(일반 공격)
		unsigned long m_dwSpecialTargetChrID;		// 현재 스킬 타겟
		unsigned long m_dwRangedWeaponTargetChrID;	// 장거리 타겟

		unsigned long m_dwPopupWindowLength;		// 팝업의 가로 길이
		BOOL m_bPopupShow;							// 팝업의 표시 여부
		unsigned long m_dwPopupSelectChrID;			// 팝업의 대상 캐릭터
		unsigned long m_dwKindPopup;
		vector<CTexture *> m_lstTexture;			// 인터페이스에서 쓰이는 텍스쳐
		vector<CGUIWindow *> m_lstWindow;			// 인터페이스에서 쓰이는 윈도우

		BOOL m_bQuickWeaponExchange;				// 무기 전환 가능 유무 관련
		BOOL m_bWeaponExchageAble;					// 무기 전환 가능 유무 관련
		float m_fWeaponExchageAbleTimeCounter;		// 무기 전환 가능 유무 관련

		BOOL m_bExchange;							// 거래 관련
		unsigned long m_dwExchangeCmdResult;		// 거래 관련
		unsigned long m_dwExchangeCmdSenderID;		// 거래 관련
		unsigned long m_dwExchangeCmdType;			// 거래 관련

		unsigned long m_dwDuelCmdResult;			// 듀얼 관련
		unsigned long m_dwDuelCmdSenderID;			// 듀얼 관련
		unsigned long m_dwDuelCmdType;				// 듀얼 관련

		unsigned long m_dwPartyDuelCmdResult;		// 파티듀얼 관련
		unsigned long m_dwPartyDuelCmdSenderID;		// 파티듀얼 관련
		unsigned long m_dwPartyDuelCmdType;			// 파티듀얼 관련

		unsigned long m_dwPartyCmdResult;			// 파티 관련
		unsigned long m_dwPartyCmdType;				// 파티 관련
		unsigned long m_dwPartyCmdSenderID;			// 파티 관련

		CSprite *m_lpSkillGaugeFrame;				// 캐스트 스킬 게이지 프레임
		CSprite *m_lpSkillGauge[5];					// 캐스트 스킬 게이지(레벨별)
		CSprite *m_lpTarget[2];
		CSprite *m_lpHPGauge;						// 캐릭터 머리위에 뜨는 HP게이지
		CSprite *m_lpQuiver;						// 화면 우하단에 나오는 화살 부족 표시
		CSprite *m_lpChant[44];						// 화면 좌하단에 나오는 챈트 아이콘
		CSprite *m_lpMark[10];						// 각 종족의 계급장

		CTexture *m_lpChrButtonTexture;				// 캐릭터 이름의 판넬
		CTexture *m_lpPopupTexture;					// 팝업 판넬
		CTexture *m_lpSocket;						// 툴팁에 나오는 장비에 박히는 소켓의 프레임
		CTexture *m_lpCommonWindow;
		CTexture *m_lpChantIcon;					// 챈트 아이콘 텍스쳐
		CTexture *m_lpMarkIcon;						// 계급장 아이콘 텍스쳐

		list<unsigned long> m_lstDeleteChr;
		unsigned long m_dwSpendTime[6];

		BOOL m_bClickButton;						// 캐릭터 클릭 유무
		BOOL m_bShowHelp;							// 도움말 표시 여부
		BOOL m_bShowSkillError;						// 스킬 시전시 에러 표시 유무 (마우스 모드를 위해)

		void RenderChrButton(LPDIRECT3DDEVICE8 lpD3DDevice, char *strText, short wPosX, short wPosY);
		void RenderChrButton(LPDIRECT3DDEVICE8 lpD3DDevice, unsigned long dwUID, short wPosX, short wPosY, unsigned long dwColor = D3DCOLOR_RGBA(255, 202, 132, 255));
		void RenderPopUp(LPDIRECT3DDEVICE8 lpD3DDevice);
		void RenderChrName(LPDIRECT3DDEVICE8 lpD3DDevice, CCreature *lpCreature, short wPosX, short wPosY);

		void DestroyAllObject(void);

		CBaseGUI();
		virtual ~CBaseGUI();

		virtual BOOL InitChrSelect(void) = 0;
		virtual void UpdateChrSelect(void) = 0;
		virtual void RenderChrSelect(LPDIRECT3DDEVICE8 lpD3DDevice) = 0;
		virtual BOOL InitGame(void) = 0;
		virtual void UpdateGame(void) = 0;
		virtual void RenderGame(LPDIRECT3DDEVICE8 lpD3DDevice) = 0;
		virtual void InitTrade(unsigned long dwKindMarket, unsigned long dwNPCID) = 0;
		virtual BOOL GetIsTrade(void) = 0;
		virtual void InitClassChange(unsigned long dwNPCID) = 0;
		virtual void ProcessExchangeCmd(unsigned long dwSenderID, unsigned short wCmd) = 0;
		virtual void InitPos(CItemInstance *lpItem) = 0;
		virtual BOOL GetIsExchange(void) = 0;
//		virtual BOOL StartSkill(void);
		virtual BOOL InitPopup(unsigned long dwKindPopup, unsigned long dwUID, unsigned long dwKindPos) = 0;
		virtual void InitSkillUnLock(unsigned long dwNPCID) = 0;
		virtual void InitChannel(unsigned char cChannelNum, unsigned short *lpChannel) = 0;
		virtual void InitRespawn(unsigned char cZoneNum) = 0;
		virtual BOOL GetIsRespawn(void) = 0;
		virtual void DestroyRespawn(void) = 0;
		virtual BOOL GetEnableChat(void) = 0;
		virtual void InitDeposit(void) = 0;
		virtual void ShowWindow(unsigned long dwWindowNum) = 0;
		virtual BOOL GetBusy(void) = 0;
		virtual BOOL GetIsBlackMarket(void) = 0;
		virtual void InitStreetStall(unsigned long dwChrID, char *lpstrStreetName) = 0;
		virtual void InitQuest(unsigned long dwChrID, unsigned long dwQuestID) = 0;
		virtual void InitItemUpgrade(unsigned long dwChrID) = 0;
		virtual void InitRank(void) = 0;
		virtual void InitStatusInit(void) = 0;
		virtual void InitStatusRetrain(void) = 0;
		virtual void HideInterfaceforDead(void) = 0;
};

#endif // !defined(AFX_BASEGUI_H__3E9B914A_483F_4DBB_A374_227D1CAAC19F__INCLUDED_)
