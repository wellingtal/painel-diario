// CharStatus.h: interface for the CCharStatus class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHARSTATUS_H__E6A8A3B4_7211_4E9F_85BE_2C83B5D9BC51__INCLUDED_)
#define AFX_CHARSTATUS_H__E6A8A3B4_7211_4E9F_85BE_2C83B5D9BC51__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Network/Packet/PacketStruct/CharAttackPacketStruct.h>
#include <Network/Packet/PacketStruct/CharItemPacket.h>
#include <Network/Packet/PacketStruct/CharStatusPacket.h>
#include <Network/Packet/PacketStruct/FriendPacket.h>
#include <Network/Packet/PacketStruct/RankingPacket.h>

#include <windows.h>
#include <vector>
#include <list>

#include <DB/DBdefine.h>
#include "NPC/NPCList.h"
#include <Skill/Spell/SpellKind.h>
#include <Skill/SkillStructure.h>

#include "ItemInstance.h"


using namespace std;

#define ITEM_FAILED			0xFFFFFFFFFFFFFFFF	// 이동할 수 없음
#define ITEM_EMPTY			0xFEFFFFFFFFFFFFFF	// 비어있는 자리
#define ITEM_QUEST_TYPE		0xFDFFFFFFFFFFFFFF	// 퀘스트 타입 아이템 (퀘스트 인벤토리에서만 이동 가능)
#define ITEM_NOT_QUEST_TYPE	0xFCFFFFFFFFFFFFFF	// 퀘스트 타입이 아닌 아이템 (퀘스트 인벤토리에 들어갈 수 없음)

#define STATUS_STR 0
#define STATUS_DEX 1
#define STATUS_CON 2
#define STATUS_INT 3
#define STATUS_WIS 4

#define	STATUS_UPDATE_BASE			0
#define	STATUS_UPDATE_EQUIP			1
#define	STATUS_UPDATE_ENCHANT		2


typedef struct _FieldInstance : public FieldObject 
{
	unsigned long m_dwInstanceID;
	long m_lLifeTick;
	vector3 m_vecPos;
} FieldInstance, *LPFieldInstance;

typedef struct QuestInstance
{
	enum Const
	{
		MAX_HISTORY_QUEST	= 200,				// 히스토리 최대 목록
		MAX_EXECUTING_QUEST	= 20,				// 동시 수행 가능한 최대 퀘스트 수
		MAX_REQUEST_QUEST	= 15,				// 클라이언트에서 퀘스트 수락 버튼을 클릭해서 얻을 수 있는 최대 퀘스트 수
												// 자동 부여 퀘스트 누락 방지를 위해 5개의 퀘스트의 차이를 벌려놓음. (by hackermz)
		MAX_TRIGGER			= 10,				// 한 페이즈당 최대 트리거 수

		DISABLE_CANCEL_QUEST_BIT	= 0xF000	// 삭제 불가능한 강제 퀘스트
	};

	unsigned long m_dwQuestID;
	unsigned long m_dwPresentPhase;
	vector<unsigned long> m_lstTriggerCount;
	LPQuestNode m_lpQuestScript;
	LPPhaseNode m_lpPhaseScript;
	CRYLSprite *m_lpQuestIcon;

	BOOL StartPhase(unsigned long dwPhase);
	BOOL StartTrigger(LPTriggerNode lpTrigger, BOOL bSuccess);

	BOOL EventPhase(unsigned long dwPhase);
	BOOL EventMsgBox(char *strWord);
} QuestInstance, *LPQuestInstance;

typedef struct PotionCoolTime
{
	unsigned short m_wPotionSkillID;
	long m_lPotionCoolDownTime;
} PotionCoolTime, *LPPotionCoolTime;

// 인벤토리에 아이템을 넣을 수 있는지 체크할때 사용하는 아이템 정보 구조체
typedef struct TestItemInfo
{
	unsigned short	wItemID;
	unsigned char	cNowNum;
} TestItemInfo, *LPTestItemInfo;

class CCharStatus  
{
public:

	enum ItemContainer
	{
		INVENTORY_TAB		= 4,
		INVENTORY_HEIGHT	= 13,
		INVENTORY_WIDTH		= 8,
	};

	enum CheckField
	{
		REMOVE_FIELD		= 0,
		SET_FIELD			= 1,
	};

	CCharStatus();
	~CCharStatus();

	bool CalculateAbility(const SKILL& skill);

	void StatusUpdate( int type, bool bRight, bool bPassive );

	__int64 IsCheckItem(CItemInstance* lpItem, unsigned long dwPos, unsigned long dwInvenX, unsigned long dwInvenY = 0, unsigned long dwInvenTab = 0);
	bool GetAbleInven(LPFieldInstance lpFieldInstance, Item::ItemPos &pItemIndex, BOOL bStack = TRUE);
	bool GetAbleInven(unsigned short wItemID, unsigned char cNum, Item::ItemPos &pItemIndex, BOOL bStack = TRUE);

	VOID	InitTestItemInfo() ;
	BOOL	CheckInvenMoney(unsigned long dwAddGold, unsigned long dwDeductGold=0) ;
	BOOL	CheckAbleInven(unsigned short wItemID, unsigned char cNum) ;
	BOOL	CheckQuestInvenSpace(QuestNode* lpQuest) ;

	
	long GetPotionCoolTime(unsigned short wPotionSkillID);
	void AddCompleteQuest(LPQuestInstance lpDeleteQuest);
	void SetStatusInit(unsigned short wClass, ChState &pState);
	void CheckUseSkillforStatus(unsigned long &dwLock, unsigned long &dwLevel);
	void CheckUseSkillforMP(unsigned long &dwLock, unsigned long &dwLevel);
	
	LPTriggerNode GetTriggertoGeton(LPQuestInstance &lpQuest, unsigned long *dwCount, vector3 vecPos);
	LPTriggerNode GetTriggertoTalk(LPQuestInstance &lpQuest, unsigned long *dwCount, unsigned long dwChrID);
	LPTriggerNode GetTriggertoTalkInQuest(LPQuestInstance lpQuest, unsigned long *dwCount, unsigned long dwChrID);
	LPTriggerNode GetTriggertoLevelTalk(LPQuestInstance &lpQuest, unsigned long *dwCount, unsigned long dwChrID);
	LPTriggerNode GetTriggertoLevelTalkInQuest(LPQuestInstance lpQuest, unsigned long *dwCount, unsigned long dwChrID);
	LPTriggerNode GetTriggertoFameTalk(LPQuestInstance &lpQuest, unsigned long *dwCount, unsigned long dwChrID);
	LPTriggerNode GetTriggertoFameTalkInQuest(LPQuestInstance lpQuest, unsigned long *dwCount, unsigned long dwChrID);

	LPTriggerNode GetTriggertoAbilityTalk(LPQuestInstance &lpQuest, unsigned long *dwCount, unsigned long dwChrID);
	LPTriggerNode GetTriggertoAbilityTalkInQuest(LPQuestInstance lpQuest, unsigned long *dwCount, unsigned long dwChrID);


	LPQuestNode	GetQuestToGeton(vector3 vecPos);
	LPQuestInstance GetProcessQuest(unsigned short wQuestID);
	
	BOOL GetCorrectClass(unsigned long dwClassFlag);
	void ChantDisable(void);
	void PayDepositPrice(unsigned short wSrcPos, unsigned short wDstPos);
	void Destroy(void);
	BOOL CheckStatus(void);
	void SetActivateSkill(CItemInstance *lpUseSkill);
	unsigned long CheckUseSkill(void);
	unsigned long CheckSkillCooldown(unsigned short wSkill);

	BOOL UseSocialAction(unsigned short wSkill, unsigned short wGrade);

	BOOL GetExistExchangeItem(void);
	BOOL CheckItemByStatus(CItemInstance *lpItem);
	void SetQuickSlot(CItemInstance *lpItem);
	BOOL SetQuickSlotItemNumber(CItemInstance *lpItem);

	void UpdateQuickSlot(unsigned short wSkill, unsigned short wGrade);
	void DelQuickSlot(unsigned short wSkill);
	
	unsigned long IsCheckClassforSkill(unsigned long dwClassLimit);

	bool CheckUsePotion(CItemInstance *lpItem);
	unsigned long CheckUseSkillBook(CItemInstance *lpItem);
	
	long GetUseIP(unsigned long dwKindStatus);
	
	BOOL IsCheckStatusforSkill(unsigned short wSkill);
	BOOL CheckPartyMember(unsigned long dwChrID);
	BOOL GetIsInstallSocket(CItemInstance *lpEquipItem, CItemInstance *lpSocketItem);
	BOOL GetIsRuneSocket(CItemInstance *lpExistItem, CItemInstance *lpRuneItem, int &nPosition);

	SKILLSLOT *GetSkillSlot(unsigned short wSkill);

	//hz added this for skill bug fix
	int GetSkillSlotIndex(unsigned short wSkill);
	//hz end
	// 아이템 합성  
	// 인자 : ( 인벤토리에 있는 아이템, 손에 들려져 있는 아이템, 결과 값 리턴 )		
	//		함수의 결과는 합성가능하면 TRUE 리턴
	BOOL GetIsChemicalitem(CItemInstance *lpExistItem, CItemInstance *lpPickItem, unsigned char &result);		
	
	char *LogoutPartyPlayer(unsigned long dwChrID);
	char *LoginPartyPlayer(unsigned long dwChrID, unsigned long dwServerID);
	void AddPartyPlayer(unsigned long dwChrID, unsigned long dwServerID, char *strName);
	void AttackPartyPlayer(DefenserNode& defNode, AtType attackType);
	void DeletePartyPlayer(unsigned long dwChrID);
	void DestroyParty(void);
	char *GetPartyName(unsigned long dwChrID);
    bool IsPartyMember(unsigned long dwChrID);
    bool IsPartyMember(const char* szName);

	CItemInstance* GetItem(Item::ItemPos srcPos);
	void SetItem(Item::ItemPos dstPos, CItemInstance* lpItem, BOOL bSound = TRUE, BOOL bSelf = TRUE);
	void RemoveItem(Item::ItemPos dstPos);

	void AddEquipItem(unsigned long dwPos, CItemInstance* lpItem, BOOL bSound = TRUE);
	void AddInventoryItem(CItemInstance* lpItem, BOOL bSound = TRUE);
	void AddDepositItem(CItemInstance* lpItem, BOOL bSound = TRUE);
	void AddExchangeItem(CItemInstance* lpItem, BOOL bSelf = TRUE, BOOL bSound = TRUE);

	void DeleteInventoryItem(CItemInstance *lpDelete);
	void DeleteDepositItem(CItemInstance *lpDelete);
	void DeleteExchangeItem(CItemInstance *lpDelete, BOOL bSelf = TRUE);
	void DeleteStreetTradeItem(CItemInstance *lpDelete);

	void CheckInvenField(unsigned long dwCommand, CItemInstance *lpItem);
	void CheckDepositField(unsigned long dwCommand, CItemInstance *lpItem);
	void CheckExchangeField(unsigned long dwCommand, CItemInstance *lpItem, BOOL bSelf = TRUE);
	void CheckStreetTradeField(unsigned long dwCommand, CItemInstance *lpItem);

	BOOL GetIsInventoryItem(CItemInstance *lpItem);

	CItemInstance* GetInventoryItemfromID(unsigned short wProtoTypeID);
	CItemInstance* GetInventoryItem(Item::ItemPos &pIndex);
	CItemInstance* GetInventoryItem(unsigned short wIndex);
	CItemInstance* GetInventoryItem(__int64 ddItemUID);
	CItemInstance* GetExchangeItemfromID(unsigned short wProtoTypeID, BOOL bSelf = TRUE);
	CItemInstance* GetExchangeItem(Item::ItemPos &pIndex, BOOL bSelf = TRUE);
	CItemInstance* GetExchangeItem(unsigned short wIndex, BOOL bSelf = TRUE);
	CItemInstance* GetExchangeItem(__int64 ddItemUID, BOOL bSelf = TRUE);
	CItemInstance* GetDepositItemfromID(unsigned short wProtoTypeID);
	CItemInstance* GetDepositItem(Item::ItemPos &pIndex);
	CItemInstance* GetDepositItem(unsigned short wIndex);
	CItemInstance* GetDepositItem(__int64 ddItemUID);
	CItemInstance* GetStreetTradeItemfromID(unsigned short wProtoTypeID);
	CItemInstance* GetStreetTradeItem(Item::ItemPos &pIndex);
	CItemInstance* GetStreetTradeItem(unsigned short wIndex);
	CItemInstance* GetStreetTradeItem(__int64 ddItemUID);
	
	BOOL AddFriend(unsigned long dwChrID, char *strName, unsigned long dwGID, unsigned short wClass,
					char cLevel, unsigned long dwServerID);

	BOOL AddBan(unsigned long dwChrID, char *strName, unsigned long dwGID, unsigned short wClass, char cLevel, unsigned long dwServerID);
	BOOL DeleteFriend(unsigned long dwChrID);
	BOOL DeleteBan(unsigned long dwChrID);
	BOOL GetIsFriend(unsigned long dwChrID);
	BOOL GetIsBan(unsigned long dwChrID);
	FriendInfo *GetFriend(unsigned long dwChrID);
	BanInfo	*GetBan(unsigned long dwChrID);
	FriendInfo *GetFriendtoNum(unsigned long dwNum);
	BanInfo *GetBantoNum(unsigned long dwNum);

	unsigned short GetSkillIDbyChantID(unsigned char cChant);
	void SetClassSkill(void);
	void CheckGratherQuickSlot();

	INT DeleteQuest(LPQuestInstance lpDeleteQuest);

	BOOL			GetIsUpgrading()					{ return m_bUpgrading;			}
	VOID			SetUpgrading( BOOL bUpgrading )		{ m_bUpgrading = bUpgrading;	}
	VOID			SetUseStealth( BOOL bUseStealth)	{ m_bUseStealth = bUseStealth;	}

	short			GetCurrentHP()					{ return m_Info.HP; 	};
	short			GetCurrentMP()					{ return m_Info.MP; 	};
	__int64			GetCurrentExp()					{ return m_Info.Exp;	};					// 캐릭터 경험점

	short			GetSTR()						{ return m_Info.STR; 	};
	short			GetDEX()						{ return m_Info.DEX; 	};	
	short			GetINT()						{ return m_Info.INT; 	};
	short			GetCON()						{ return m_Info.CON; 	};
	short			GetWIS()						{ return m_Info.WIS; 	};

	unsigned long	GetUID()						{ return m_Info.CID;			};
	const char*		GetName()						{ return m_Info.Name;			};			// 이름
	char			GetSex()						{ return m_Info.Sex;			};			// 성별
	char			GetHair()						{ return m_Info.Hair;			};			// 머리 모양
	char 			GetFace()						{ return m_Info.Face;			};			// 얼굴 모양
	char			GetRace()						{ return m_Info.Race;			};			// 종족
	unsigned short	GetClass()						{ return m_Info.Class;			};			// 클래스
	unsigned short	GetIP()							{ return m_Info.IP;				};			// 증가 포인트

	unsigned long	GetFame()						{ return m_Info.Fame;		};				// 명성		
	unsigned long	GetMileage()					{ return m_Info.Mileage;	};				// 마일리지			
	unsigned long	GetGuild()						{ return m_Info.GID; 		};				// 길드
	unsigned long	GetParty()						{ return m_Info.PID; 		};				// 파티
	char			GetLevel()						{ return m_Info.Level; 		};				// 레벨
	unsigned long	GetGold()						{ return m_Info.Gold;		};				// 돈 

	void			SetKillStreak(int New = 0)		{ m_Killstreak = New;	}//hz added for Killstreak panel show
	void			SetSTR( short val )				{ m_Info.STR = val; 	};
	void			SetDEX( short val )				{ m_Info.DEX = val; 	};
	void			SetINT( short val )				{ m_Info.INT = val; 	};
	void			SetCON( short val )				{ m_Info.CON = val; 	};
	void			SetWIS( short val )				{ m_Info.WIS = val; 	};
	void			SetClass( unsigned short val )	{ m_Info.Class = val;	};					// 클래스
	void			SetIP( unsigned short val )		{ m_Info.IP = val;		};					// 증가 포인트
	void			SetGold( unsigned long val )	{ m_Info.Gold = val;	};
	void			SetGuild( unsigned long val )	{ m_Info.GID = val;	    };					// 길드
	void			SetFame( unsigned long val )	{ m_Info.Fame = val;	};					// 명성		
	void			SetMileage( unsigned long val )	{ m_Info.Mileage = val; };					// 마일리지	
	void			SetParty( unsigned long val )	{ m_Info.PID = val;	    };					// 파티
	void			SetLevel( char val )			{ m_Info.Level = val;	};					// 레벨
	void			SetCurrentHP( short val )		{ m_Info.HP = val; 		};
	void			SetCurrentMP( short val )		{ m_Info.MP = val; 		};
	void			SetCurrentExp( __int64 val )	{ m_Info.Exp = val;		};					// 경험점

	void			AddTempInventory( CItemInstance* pItem );
	CItemInstance*	RemoveTopTempInventory();

	long			GetMinDamage()					{ return m_statInfoEnchant.m_lMinDamage;		};
	long			GetBaseMinDamage()				{ return m_statInfoEquip.m_lMinDamage;			};
	long			GetMaxDamage()					{ return m_statInfoEnchant.m_lMaxDamage;		};
	long			GetBaseMaxDamage()				{ return m_statInfoEquip.m_lMaxDamage;			};
	short			GetArmor()						{ return m_statInfoEnchant.m_wArmor;			};
	short			GetBaseArmor()					{ return m_statInfoEquip.m_wArmor;				};
	short			GetHitRate()					{ return m_statInfoEnchant.m_wHitRate;			};
	short			GetBaseHitRate()				{ return m_statInfoEquip.m_wHitRate; 			};
	short			GetEvade()						{ return m_statInfoEnchant.m_wEvade; 			};
	short			GetBaseEvade()					{ return m_statInfoEquip.m_wEvade;				};
	unsigned short	GetMaxHP()						{ return m_statInfoEnchant.m_nMaxHP;			};
	unsigned short	GetBaseMaxHP()					{ return m_statInfoEquip.m_nMaxHP;				};
	unsigned short	GetMaxMP()						{ return m_statInfoEnchant.m_nMaxMP;			};
	unsigned short	GetBaseMaxMP()					{ return m_statInfoEquip.m_nMaxMP;				};
	short			GetHPRegen()					{ return m_statInfoEnchant.m_wHPRegen;			};
	short			GetBaseHPRegen()				{ return m_statInfoEquip.m_wHPRegen;			};
	short			GetMPRegen()					{ return m_statInfoEnchant.m_wMPRegen;			};
	short			GetBaseMPRegen()				{ return m_statInfoEquip.m_wMPRegen;			};
	short			GetCritical()					{ return m_statInfoEnchant.m_wCritical;			};			// 크리티컬
	short			GetBaseCritical()				{ return m_statInfoEquip.m_wCritical;			};			// 크리티컬
	short			GetBlock()						{ return m_statInfoEnchant.m_wBlock;			};			// 블록
	short			GetBaseBlock()					{ return m_statInfoEquip.m_wBlock;				};			// 블록
	short			GetSpeed()						{ return m_statInfoEnchant.m_wSpeed;			};			// 속도
	short			GetBaseSpeed()					{ return m_statInfoEquip.m_wSpeed;				};			// 속도
	short			GetCoolDownRevision()			{ return m_statInfoEnchant.m_wCoolDownRevision;	};			// 쿨다운보정
	short			GetBaseCoolDownRevision()		{ return m_statInfoEquip.m_wCoolDownRevision;	};			// 쿨다운보정
	short			GetSkillPoint()					{ return m_statInfoEnchant.m_wSkillPoint;		};			// 스킬포인트
	short			GetMagicPower()					{ return m_statInfoEnchant.m_wMagicPower;		};			// 마법력
	short			GetBaseMagicPower()				{ return m_statInfoEquip.m_wMagicPower;			};			// 마법력
	short			GetMagicResist()				{ return m_statInfoEnchant.m_wMagicResist;		};			// 마법저항
	short			GetBaseMagicResist()			{ return m_statInfoEquip.m_wMagicResist;		};			// 마법저항

	float			GetDefenceRate()				{ return m_statInfoEnchant.m_fDefenceRate;				};		// 방어 %
	float			GetBaseDefenceRate()			{ return m_statInfoEquip.m_fDefenceRate;				};		// 방어 %
	float			GetCriticalMagnification()		{ return m_statInfoEnchant.m_fCriticalMagnification;	};		// 크리티컬배율
	float			GetBaseCriticalMagnification()	{ return m_statInfoEquip.m_fCriticalMagnification;		};		// 크리티컬배율
	float			GetCriticalRate()				{ return m_statInfoEnchant.m_fCriticalRate;				};		// 크리티컬%
	float			GetBaseCriticalRate()			{ return m_statInfoEquip.m_fCriticalRate;				};		// 크리티컬%
	float			GetBlockRate()					{ return m_statInfoEnchant.m_fBlockRate;				};		// 블록률%
	float			GetBaseBlockRate()				{ return m_statInfoEquip.m_fBlockRate;					};		// 블록률%
	float			GetSpeedRate()					{ return m_statInfoEnchant.m_fSpeedRate;				};		// 속도%
	float			GetBaseSpeedRate()				{ return m_statInfoEquip.m_fSpeedRate;					};		// 속도%
	float			GetCoolDownRevisionRate()		{ return m_statInfoEnchant.m_fCoolDownRevisionRate;		};		// 쿨다운보정%
	float			GetBaseCoolDownRevisionRate()	{ return m_statInfoEquip.m_fCoolDownRevisionRate;		};		// 쿨다운보정%
	float			GetMagicResistRate()			{ return m_statInfoEnchant.m_fMagicResistRate;			};		// 마법저항%
	float			GetBaseMagicResistRate()		{ return m_statInfoEquip.m_fMagicResistRate;			};		// 마법저항%

	int				GetAbilityPoint()							{ return m_iAbilityPoint;					};		// 어빌리티포인트
	void			SetAbilityPoint(int iAbilityPoint)			{ m_iAbilityPoint = iAbilityPoint;			};		// 어빌리티포인트
	int				GetUseAbilityPoint()						{ return m_iUseAbilityPoint;				};
	void			SetUseAbilityPoint(int iUseAbilityPoint)	{ m_iUseAbilityPoint = iUseAbilityPoint;	};

	unsigned short	GetAbilityValue(int nAbilityType) 
	{
		if(nAbilityType >= Skill::Type::MAX_ABILITY_TYPE )
			return 0;

		return m_AbilityValue[nAbilityType]; 
	}

	int				GetLuck()						{ return m_iLuck;		};			// 럭키찬스
	float			GetLuckResist()					{ return m_fLuckResistRate;		};	// 럭키찬스

	CItemInstance*	GetTempInventory()				{ if(m_lstTempInventoryItem.size() > 0) return (*m_lstTempInventoryItem.begin() ); return NULL; };
	unsigned long	GetTempInventoryTime()			{ return m_dwTempInventoryTime;			};
	unsigned long	GetTempInventorySize()			{ return m_lstTempInventoryItem.size(); };

	unsigned char	GetWeaponType()
	{
		Item::CEquipment* pItem = NULL ;
		
		if( m_cWeaponPos == 0  )
		{
			if ( m_lstEquipment[ Item::EquipmentPos::WEAPON_HAND1 ] )
				return m_lstEquipment[ Item::EquipmentPos::WEAPON_HAND1 ]->GetItemType();
		}
		else
		{
			if ( m_lstEquipment[ Item::EquipmentPos::WEAPON_HAND2	] )
				return m_lstEquipment[ Item::EquipmentPos::WEAPON_HAND2	]->GetItemType();
		}

		return 0;
	}

    struct PartyExtraInfo
    {
		EnchantInfo		m_EnchantInfo;

        long            m_lPartyAttackTimer;

        unsigned short  m_wAttackType;
        unsigned char   m_cSkillLockcount;
        unsigned char   m_cSkillLevel;

        unsigned short  m_usMaxHP;
        unsigned short  m_usMaxMP;
        unsigned short  m_usCurrHP;
        unsigned short  m_usCurrMP;

        unsigned char   m_cSkipLoginOutNum;

		PartyExtraInfo()
		:	m_lPartyAttackTimer(0), m_wAttackType(0), m_cSkillLockcount(0), m_cSkillLevel(0), 
			m_usMaxHP(0), m_usMaxMP(0), m_usCurrHP(0), m_usCurrMP(0), m_cSkipLoginOutNum(0)
		{ }
    };

	// 전직시 이전의 스테이스 저장
	CHAR_INFOST 	m_BeforeInfo;
	CHAR_INFOST 	m_Info;

	int				m_Killstreak;
	int				m_LastKillstreak;

	SKILL			m_Skill;

	PARTY_EX		m_PartyInfo;
    PartyExtraInfo  m_aryPartyExtraInfo[PARTY::MAX_MEM];

	unsigned short	m_wUseChantSkillID;
	unsigned short	m_aryEnchantLevel[ Skill::SpellID::MAX_SPELL_TYPE ];
	unsigned short	m_aryEnchantTime[ Skill::SpellID::MAX_SPELL_TYPE ];

	BOOL 			m_bSummonGuard;
	unsigned long	m_dwSummonID;

	BOOL 			m_bRangedWeapon;
	char			m_cWeaponPos;
	char			m_cRidePos;
	__int64			m_llExpMax;

	__int64			m_aryDeposit[4][12][8];
	unsigned long	m_dwDepositGold;
	char			m_cSavePass; 
	char			m_cUsedDeposit;
	unsigned long	m_dwDepositFlag;
	unsigned long	m_dwMaxTab;

	vector<PotionCoolTime>	m_lstPotionCoolTime;

	BOOL			m_bQuickSlotLength[QUICK::MAX_QUICK_NUM];

	CItemInstance*	m_lpQuickSelected;
	CItemInstance*	m_lstQuickSlot[QUICK::MAX_QUICK_NUM];
	CItemInstance*	m_lstSkillSlot[SKILL::MAX_SLOT_NUM];

	std::vector<CItemInstance*>	m_lstSocialSlot;

	CItemInstance*	m_lstEquipment[Item::EquipmentPos::MAX_EQUPMENT_POS];
	CItemInstance*	m_lpUpgrade[2];							// 제련 관련 슬롯

	CItemInstance*	m_lpCompositionOrigin;					// 아이템 옵션 이식 원본 슬롯
	CItemInstance*	m_lpCompositionOffering;				// 아이템 옵션 이식 제물 슬롯
	CItemInstance*	m_lpCompositionResult;					// 아이템 옵션 이식 결과 슬롯
	int				m_iCompositionResult;					// 아이템 옵션이식 결과 변수
	CItemInstance*	m_lpCompositionTempResult;

	CItemInstance*	m_lpCompensation;						// 아이템 보상 판매 대상 슬롯
	CItemInstance*	m_lpCompensationResult;					// 아이템 보상 판매 결과 슬롯

	// 공성 관련 아이템
	CItemInstance*	m_lpSiegeArmsSkill ;
	CItemInstance*	m_lpTmpQuickSkill ;
	CItemInstance*	m_lpSiegeArmsMaterial ;
	
	vector<RankingNode *>	m_lstRanking;
	unsigned long 			m_dwMyTotalRanking;
	unsigned long 			m_dwMyClassRanking;
	SYSTEMTIME				m_stRankingTime;

	list<FriendInfo *>		m_lstFriendList;
	list<BanInfo *>			m_lstBanList;

	vector<CItemInstance *>	m_lstClassSkill;

	list<CItemInstance *>	m_lstInventory;
    __int64			        m_aryInven[INVENTORY_TAB][INVENTORY_HEIGHT][INVENTORY_WIDTH];
	
    list<CItemInstance *>	m_lstDeposit;

	vector<CItemInstance *>	m_lstTempInventoryItem;
	unsigned long			m_dwTempInventoryTime;

	list<CItemInstance *> 	m_lstExchange;
	list<CItemInstance *> 	m_lstExchange2;
	__int64 				m_aryExchange[4][8];
	__int64 				m_aryExchange2[4][8];
	unsigned long 			m_dwExchangeGold;
	unsigned long 			m_dwExchange2Gold;

	list<CItemInstance *>	m_lstStreetTrade;
	__int64					m_aryStreetTrade[8][10];

	list<QuestInstance *> 	m_lstQuest;
	list<QuestInstance *> 	m_lstCompleteQuest;

	int 			m_MaxHP;
	int 			m_MaxMP;

	long 			m_lAddMaxHP;
	long 			m_lAddMaxMP;

	// 인벤토리에 아이템 리스트를 넣을수 있는지 체크하기위해서 이전에 있는 인벤토리의 아이템 배열을 복사해서 사용할 임시 공간
	__int64									m_aryTestInven[INVENTORY_TAB][INVENTORY_HEIGHT][INVENTORY_WIDTH] ;
	std::map<unsigned long, TestItemInfo>	m_mapTestItem ;	// 인벤토리로 새로 들어간 물품 정보를 기억해두는 임시 공간

private:

	CHAR_POS		m_InfoPos;

	BOOL			m_bUpgrading;			// Upgrade중이라서 아이템을 바꾸면 안될때!
	BOOL			m_bUseStealth;			// 스텔스를 사용할수 있는가 여부

	unsigned long	m_dwSelectQuickslot;

	int				m_iLuck;				// 럭키찬스
	float			m_fLuckResistRate;		// 럭키찬스 레이트
	
	int				m_iAbilityPoint;		// 어빌리티 포인트
	int				m_iUseAbilityPoint;		// 사용된 어빌리티 포인트
    
	// edith 2009.11.10 어빌리티 능력치 제어
	unsigned short	m_AbilityValue[Skill::Type::MAX_ABILITY_TYPE];

	FightStatus		m_statInfoBase;
	FightStatus		m_statInfoEquip;
	FightStatus		m_statInfoEnchant;
};

#endif // !defined(AFX_CHARSTATUS_H__E6A8A3B4_7211_4E9F_85BE_2C83B5D9BC51__INCLUDED_)
