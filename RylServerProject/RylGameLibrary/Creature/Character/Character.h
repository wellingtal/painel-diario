#ifndef _CCHARACTER_H_
#define _CCHARACTER_H_

#include <Item/Container/ItemContainer.h>
#include <Item/Container/EquipmentsContainer.h>
#include <Item/Container/ExchangeContainer.h>
#include <Item/Container/DepositContainer.h>
#include <Item/Container/StallContainer.h>
#include <Item/Container/TempInvenContainer.h>
#include <Item/Container/InventoryContainer.h>

#include <Network/Packet/PacketStruct/CharLoginOutPacketStruct.h>
#include <Network/Packet/PacketStruct/CharItemPacketStruct.h>
#include <Network/Packet/PacketStruct/CharCommunityPacketStruct.h>
#include <Network/Packet/PacketStruct/CharStatusPacketStruct.h>
#include <Network/Packet/PacketStruct/CharConfigPacketStruct.h>

#include <Creature/AggresiveCreature.h>
#include <Creature/CreatureManager.h>

#include <Community/FriendList.h>
#include <Community/BanList.h>

#include <Quest/QuestStruct.h>
#include <Utility/Setup/ServerSetup.h>

#include "CharacterClass.h"

#include <Network/Broadcast/SerializeCharacterData.h>

#include <atltime.h>
#include <GameTime/GameTimeMgr.h>
#include <GameTime/GameTimeConstants.h>


// 전방 참조
class CChatPacket;
class CGameClientDispatch;
struct GiveItemInfo;

namespace Broadcast2nd
{
    class CSerializeCharacterData;
};

namespace Item
{
    class CItem;
	class CEquipment;
	class CUseItem;

	struct ItemPos;
};

namespace BattleInclination
{
	void SetCharData(CCreature& creature, BattleInclination::CharData& charData, bool bSetRideArmsInfo = false);
	void SetRelationData(CCreature& ownerCreature, CCreature& targetCreature, BattleInclination::RelationData& relationData);
};

namespace RealmSkill
{
	void RealmInchantAdd(CCharacter* lpCharacter);
	void RealmInchantRemove(CCharacter* lpCharacter);	
};

class CCharacter : public CAggresiveCreature
{
public:

    // DB중계에서 테이블로 읽어서 TempInven으로 주어진 아이템의 CreationID를 저장한다.
    typedef std::vector<unsigned long>  GivenItemList;

	enum Const
	{
		LOGOUT_PULSE    			= 5,	
		LOGOUT_COUNT    			= 30,//hz was 5 if cause any problems 14 seconds stay if dc or char select

		DBUPDATE_PULSE  			= 10,	
		DBUPDATE_COUNT  			= 180,

		BATTLE_GROUND_PULSE 		= 600,

		RESPAWN_PULSE				= 100,	// 10 초

		MELEE_ATTACK_RANGE			= 10,	// 캐릭터의 근접 공격 제한 거리
		RANGE_ATTACK_RANGE			= 50	// 캐릭터의 원거리 공격 제한 거리
	};

    enum OperationFlag
    {
        CHAR_INFO_LOADED        = ( 1 << 0),
        CHAR_ZONE_MOVED         = ( 1 << 1),
        MOVEZONE_PROCESSED      = ( 1 << 2),
        SERVERZONE_PROCESSED    = ( 1 << 3)        
    };

	enum DURABILITY_DECRESE_PERCENTAGE
	{	
		// edith 아이템의 내구도 랜덤값
		WEAPON_DURABILITY_DECRESE_PERCENTAGE = 100,	// 200
		ARMOUR_DURABILITY_DECRESE_PERCENTAGE = 25,	// 50
		SHIELD_DURABILITY_DECRESE_PERCENTAGE = 10,	// 20

		STARTAXDURABILITY = 10,				// 최대 내구도 감소폭 시작값
		DOWNMAXDURABILITY = 10				// 최대 내구도 감소폭 
	};

	enum DURABILITY_DECRESE_PERSENT
	{
		DURABILITY_DECREASE_PERSENT_BY_MONSTER = 20,
		DURABILITY_DECREASE_PERSENT_BY_SUICIDE = 3
	};

    enum LAST_DEAD_TYPE
    {
        DEAD_BY_NONE,
        DEAD_BY_SUICIDE,
        DEAD_BY_MONSTER,
        DEAD_BY_CHARACTER
    };

	enum RESPAWN_HP_MP_RECOVERY_PERCENTAGE
	{
		TOWN_RECOVERY_PERCENTAGE			= 25,
		FORTRESS_RECOVERY_PERCENTAGE		= 50,
		RESURRECTION_RECOVERY_PERCENTAGE	= 75
	};
	
    bool Initialize(CGameClientDispatch* lpGameClientDispatch);		// 초기화

	// --------------------------------------------------------------------------------------------
	// 캐릭터 로그인시 처리될 부분들 --------------------------------------------------------------
	
    void PrepareLogin(void); 
	bool Login(void);	        // 캐릭터 정보가 캐릭터 테이블에 로드됨(로그인함)	

	bool IsLogined(void) { return IsOperationFlagSet(CHAR_INFO_LOADED); }
		
	// --------------------------------------------------------------------------------------------
	// 캐릭터 로그아웃시 처리될 부분들 ------------------------------------------------------------
	
	inline void ResetLogoutCount(void)	{ m_nLogoutCount = LOGOUT_COUNT; }
	inline bool StillAlive(void)		{ return --m_nLogoutCount >= 0; }
	inline void SetDBUpdateCount(LONG nDBUpdateCount) { m_nDBUpdateCount = nDBUpdateCount; }
	
    // 캐릭터를 맵에서 로그아웃시키고, 데이터를 DB로 보낸다.
    bool Logout(DBUpdateData::UpdateType eUpdateType = DBUpdateData::LOGOUT);

	
	// --------------------------------------------------------------------------------------------
	// 캐릭터 데이터 업데이트 - CharacterDataUpdate.cpp -------------------------------------------

	bool GetCharacterInfo(char* pBuffer, int* nBufferSize_InOut, unsigned short* lpUpdateLen);
	bool SetCharacterInfo(char* pBuffer, unsigned short usUpdateLen[DBUpdateData::MAX_UPDATE_DB]);
	bool DBUpdate(DBUpdateData::UpdateType eUpdateType);
	bool DBUpdateForce(DBUpdateData::UpdateType eUpdateType) 
    { 
        m_nDBUpdateCount = -1;
        return DBUpdate(DBUpdateData::UPDATE); 
    }

    bool MoveZoneProcess(unsigned long dwServerID);
	bool ItemDump(char* pBuffer, int* nBufferSize_InOut) const;


	// --------------------------------------------------------------------------------------------
	// 아이템, 돈 이동 및 사용 관련 메소드 - CharacterItem.cpp ------------------------------------

	inline unsigned long GetGold(void) const { return m_DBData.m_Info.Gold; }
    unsigned long GetGold(unsigned char cPos);
	bool AddGold(unsigned long dwGold, bool bNotice);
    bool DeductGold(unsigned long dwGold, bool bNotice);
    bool MoveGold(unsigned long dwGold, unsigned char cSrcPos, 
        unsigned char cDstPos, unsigned short& usError);            // 돈 옮기기

	bool DeductMileage(unsigned long dwMileage, bool bNotice);

	bool MoveItem(const TakeType takeType, bool bChk = true);		// 아이템 이동 및 스택
	bool SwapItem(const TakeType SrcType, const TakeType DstType);	// 아이템 위치 스왑
	Item::CItem* SplitItem(const TakeType takeType);				// 아이템 개수 나누기

	bool Pickup(Item::CItem* lpItem, Item::ItemPos dstPos);			// 아이템 집기
	Item::CItem* Drop(Item::ItemPos SrcPos, unsigned char cNum);	// 아이템 버리기

	// 아이템을 인벤토리에 넣어준다. (자리가 없으면 임시 인벤토리에 넣는다.)
	bool GiveItem(Item::CItem* lpItem);
	bool TestItem(Item::CItem* lpItem);	// 아이템을 넣을 수 있는지 체크한다.

	bool UseLottery(unsigned short usItemID);
	bool UseCashItem(unsigned long dwSender, unsigned long dwReceiver, Item::ItemPos itemPos, unsigned short wCashType, unsigned short& wError);
	bool UseStartKit(unsigned short wObjectType, unsigned short& wError);

	bool CheckItemRuneSkill(Item::ItemPos SrcPos, Item::ItemPos DstPos, bool bChk);		// 아이템 이동시 룬 속성 체크

	const Broadcast2nd::CNetworkEquipGrade GetNetworkEquipGrade(void);					// 이펙트를 위한 장비 그레이드 정보

	// --------------------------------------------------------------------------------------------
	// 캐릭터 인벤토리 관련(아이템 Get, Set, Reset) - CharacterInventoryItem.cpp ------------------

public:

	Item::CItem* GetItem(Item::ItemPos SrcPos);
	bool SetItem(Item::ItemPos SrcPos, Item::CItem* lpItemBase);
	bool RemoveItem(Item::ItemPos SrcPos);


	// --------------------------------------------------------------------------------------------
	// 아이템 팔고 사고 수리하는 메서드 - CharacterTradeItem.cpp ----------------------------------

	unsigned long RepairItem(const unsigned long dwNPCID, Item::ItemPos itemPos, unsigned short& wError);

	unsigned long RepairAllItem(const unsigned long dwNPCID, unsigned short& wError);

	Item::CItem* SellToCharacter(CCharacter *lpCustomer, unsigned short wKindItem, TakeType takeType, 
		Item::CItem* lpRequestItem, unsigned long &dwPrice, unsigned short wCouponID, unsigned short &usError);


	// --------------------------------------------------------------------------------------------
	// 스테이터스 관련 메소드 - CharacterStatus.cpp -----------------------------------------------

	// 능력치 및 스테이터스 데이터를 DBData에서 읽어서 재계산. HP, MP가 Full이 된다.
	bool CalculateStatusData(bool bFullHPandMP);				
	bool CalculateAbility(const SKILL& skill);	// 어빌리티의 정보를 갱신한다.

	bool ChangeClass(unsigned char cClassType);					// CClass 참조
	bool IncrementExp(unsigned long dwExp);						// 레벨업 상황이 되면 레벨업한다.
	bool GetHuntingExp(CAggresiveCreature* lpDeadCreature, unsigned long dwExp, unsigned char cMemberNum);			

	unsigned short AddState(unsigned char Type_In, ChState& ChState_Out);
	bool StateRedistribution(ChState& State);					// 스탯을 해당 클래스의 처음 상태로 돌리는 함수
	bool StatusRetrain(ChState& State, Item::ItemPos InvenPos);	// 스탯을 일정량 돌려받는 처리를 하는 함수 (망각의 돌 사용)

	bool ChangeWeaponAndShield(unsigned char cSelect);			// 무기 바꾸기
	bool ChangeRide(unsigned char cSelect);			// 말 타기 내리기

	bool CalculateEquipDurability(unsigned short wAttackType);
	unsigned char GetCurrentDurability(unsigned char cEquipmentIndex);
	bool CalculateAllEquipDurability(unsigned char cDecreasePersent);

    // HP와 MP를 Regen한다. 추가 값이 0인 경우는 기본 값으로 Regen한다.
	virtual bool RegenHPAndMP(unsigned short usAdditionalHP, unsigned short usAdditionalMP, bool bAddDefaultRegenValue);

	// Admin 명령으로 레벨을 다운 시킬때 사용하는 함수 (레벨 1의 초기 클래스로 만들어 버린다.)
	bool InitLevel1Char(unsigned char cClassType);

	bool CheckRenameWarrant(Item::ItemPos InvenPos, bool bItemAccept);

	// --------------------------------------------------------------------------------------------
	// 전투 관련 메소드 - CharacterFight.cpp ------------------------------------------------------

	virtual const int CalculateFixLevelGap(CAggresiveCreature *pDefender);

	void Casting(AtType attackType, AtNode& attackNode);

	bool AttackCID(AtType attackType, AtNode& attackNode, unsigned short& wError);
	bool Attack(AtType attackType, unsigned char cDefenderNum, CAggresiveCreature** pDefenders, unsigned char* cDefenserJudges, unsigned short* wDefenderMPHeal);
	bool AttackUsingBow(unsigned short wType);
	bool UseAmmo(Item::CUseItem* pUseItem);
    
	virtual bool Dead(CAggresiveCreature* pOffencer);
	bool Respawn(unsigned char cType = 0, const Position& Pos = Position::Position());
	bool AutoRespawn(void);		// 배틀 그라운드에서 초렙 존의 위치로 자동 리스폰

	bool IsPeaceMode(void);
	EnemyCheck::EnemyType IsEnemy(CCreature* lpTarget, unsigned char* lpResult = NULL);

    void SendAttackedToParty(unsigned long dwAttackerID, AtType atType, 
        unsigned short wDamage, unsigned char cDefenserJudge, unsigned short wMPHeal);

	char GetEliteBonus(void);
	char GetCaste(void);

	void CalculateFame(CCharacter* lpWinnerCharacter);

	unsigned long CCharacter::KillSpreeMultiplier(CCharacter* lpWinnerCharacter,CCharacter* lpLoserCharacter);
	unsigned long	m_dwLastKillTime/*=timeGetTime()*/; 
	unsigned long	m_dwKilingCount ; 
	unsigned long	m_dwLastKilledID ;

	//hz added for food use cooldown fix
	unsigned long	LastUsePotionTime[12];
	unsigned long	LastUseSkillTime[130];
	bool IsCast(unsigned long skillID);
	int GetPositionOfSkillInArr(unsigned long skillID);
	int GetPositionOfSkillInBigArr(unsigned long skillID);
	//hz end

	/*edited*/
	int getKillSpreeLevel(void) { return m_KillSpreeLevel; }
	void setKillSpreeLevel(int newLevel=0) { m_KillSpreeLevel = newLevel; }//hz added for killspreelevel

	unsigned long	RightNow/*=timeGetTime()*/;

	// --------------------------------------------------------------------------------------------
	// 스킬 관련 메소드 - CharacterSkill.cpp ------------------------------------------------------

	unsigned short ReadSkill(SKILLSLOT SkillSlot, unsigned short wSkillID, unsigned short wSkillLockCount);
	bool RedistributionSkill(void);
	bool CalculateMaxSkillSlot(void);

	bool SkillCreate(Item::CUseItem* pUseItem);
	bool SkillErase(unsigned char Index_In, Item::ItemPos InvenPos);
	bool SkillLock(unsigned char Index_In);
	bool SkillUnLock(unsigned char Index_In, Item::ItemPos* InvenPos, bool b_SkillFifthUnlock = false);
	bool SkillFifthLock(unsigned short wSKilID);
	bool SkillFifthUnlock(unsigned short wSKilID);
	
	virtual bool HasSkill(unsigned short usSkillType, unsigned char cLockCount, unsigned char cLevel);
	short GetSkillLockCount(unsigned short usSkillType);
	short GetSkillLevel(unsigned short usSkillType);
	short GetSkillSlotIndex(unsigned short usSkillType);

    bool AddWorldWeaponEnchant(CAggresiveCreature* lpWeapon, unsigned char cNation);
	bool ClearWorldWeaponEnchant(void);

	void CheckSkillVaild(void);


	bool AbilityCreate(Item::CUseItem* pUseItem);
	bool AbilityErase(unsigned char Index_In, Item::ItemPos InvenPos);



	// --------------------------------------------------------------------------------------------
	// 퀵슬롯 관련 메소드 - CharacterQuickSlot.cpp ------------------------------------------------

	void UpdateQuickSlotSkill(SKILLSLOT Slot);
	bool MoveQuickSlot(const TakeType takeType, const unsigned short usSkillID, unsigned char cLockCount, unsigned char cSkillLevel);


	// --------------------------------------------------------------------------------------------
	// 캐릭터 컨트롤 관련 메소드 - CharacterControl.cpp -------------------------------------------

	bool DropItem(unsigned short usProtoTypeID, unsigned char cNum);

	bool MovePos(Position Pos, char cZone, const bool bSitDown);
	bool MoveZone(POS NewPos, char cZone, char Channel);
	bool Kill(CAggresiveCreature* lpAttacker);

	bool NotifyInfo(unsigned long dwAdminCID);

	bool DuelInit(unsigned char cCmd);

	// 길드전, 국가전에 해당하는 존으로 이동
	void MoveToGuildWarZone();
	void MoveToRealmWarZone();

	// --------------------------------------------------------------------------------------------
	// 캐릭터 퀘스트 관련 메소드 - CharacterQuest.cpp ---------------------------------------------

	bool CheckQuest(Quest::QuestNode* lpQuestNode, unsigned short& wError);
	bool HasQuest(unsigned short wQuestID);
	bool GiveQuest(Quest::QuestNode* lpQuestNode);
	bool StartPhase(unsigned short wQuestID, unsigned char cPhase);
	bool CancelQuest(unsigned short wQuestID, bool bFinished = false);

	void CheckTrigger(unsigned char cTriggerKind, unsigned long dwReferenceID, Position Pos, short wCount);
	
	unsigned short OperateTrigger(unsigned short wQuestID, unsigned char cPhase, unsigned char cTrigger, 
		unsigned char cCount, Position Pos, unsigned char cType = 0);

	void PendingQuest(Quest::TriggerMsg* pTriggerMsg);
	
	//----------------------------------------------------------------------------------------------
	// 친구 정보 업데이트

	void FriendInfoUpdate(unsigned long dwUID, unsigned long dwCID, unsigned long dwGID, unsigned short wClass, char cLevel, unsigned long dwServerID);
		
private:

	Quest::ErrorCode ExecuteEvent(Quest::ExecutingQuest executingQuest, const Quest::TriggerNode* triggerNode, Position Pos);
	void EventEnd(unsigned short wQuestID, bool bSave);
	void EventAward(unsigned long dwExp, unsigned long dwGold, unsigned long dwFame, unsigned long dwMileage);

public:

	// 스킬 관련 //
	void SkillClear();	

	bool ClearGarbage(vector<Item::ItemGarbage>& vecItemGarbage, unsigned char cCmd);

	bool HasExecutingQuest(unsigned short wQuestID);
	bool HasHistoryQuest(unsigned short wQuestID);
	bool InsertHistoryQuest(unsigned short wQuestID);
	bool DeleteHistoryQuest(unsigned short wQuestID);

	virtual void CalculateStatusByQuest(void);


	// --------------------------------------------------------------------------------------------
	// 세션  관련 메소드  -------------------------------------------------------------------------

	CGameClientDispatch* GetDispatcher(void) { return m_lpGameClientDispatch; }
    void SetDispatcher(CGameClientDispatch* lpGameClientDispatch);


	// --------------------------------------------------------------------------------------------
	// 기타 메소드  -------------------------------------------------------------------------------
	
	bool GetCharacterView(CHAR_VIEW& charView);

	// TODO : AggresiveCreature로 올리고 GetName으로 수정합시다. (Creature로 올려도 좋고...)
	const char* GetCharacterName() const { return m_DBData.m_Info.Name; }
    void SetCharacterName(const char* szName) 
    {
        strncpy(m_DBData.m_Info.Name, szName, CHAR_INFOST::MAX_NAME_LEN);
        m_DBData.m_Info.Name[CHAR_INFOST::MAX_NAME_LEN - 1] = 0;
    }
	
    const char* GetAccountName() const { return m_szAccountName; }
    void SetAccountName(const char* szAccountName)
    {
        strncpy(m_szAccountName, szAccountName, CHAR_INFOST::MAX_ACCOUNT_LEN);
        m_szAccountName[CHAR_INFOST::MAX_ACCOUNT_LEN - 1] = 0;
    }

	bool BindPositionToNPC(const unsigned long dwNPCID);					// 리스폰 위치를 NPC위치에 바인딩
	
	bool ControlOption(const RejectOption Reject, bool bLogin);				// 각종 옵션 조정

	void UpgradeRespawnSpeedByEmblem(unsigned char cUpgradeType, unsigned char cUpgradeStep);
	void DegradeRespawnSpeedByEmblem();

	virtual unsigned long GetUID(void) const	{ return m_dwUID;					}

	char GetConsumeMPCount(void)				{ return m_cConsumeMPCount;			}
	unsigned short GetClass(void)				{ return m_DBData.m_Info.Class;		}
    unsigned char GetLevel(void)            	{ return m_DBData.m_Info.Level;		}

	CClass::RaceType GetRace(void)				{ return static_cast<CClass::RaceType>(m_DBData.m_Info.Race);	}

	unsigned char GetNation(void) const;
	// WORK_LIST 2.4 계정 국적을 게임서버의 캐릭터가 가지도록 구현
	void SetNation(unsigned char cNation)		{ m_cAccountNation = cNation;		}
	unsigned long GetPID(void) const			{ return m_DBData.m_Info.PID;		}
	void SetPID(unsigned long dwPID);
	unsigned long GetGID(void) const			{ return m_DBData.m_Info.GID;		}
	void SetGID(unsigned long dwGID);

    unsigned char GetHair(void)  				{ return m_DBData.m_Info.Hair; 		}
    unsigned char GetFace(void)  				{ return m_DBData.m_Info.Face; 		}
    unsigned char GetSex(void)   				{ return m_DBData.m_Info.Sex;		}			// 0 : 남자, 1 : 여자
    unsigned char GetHand(void)  				{ return m_cHandPos;				}
    unsigned char GetRide(void)  				{ return m_cRidePos;				}
	

	ChState GetState(void);
	const SKILL& GetSkill(void)					{ return m_DBData.m_Skill;			}
    
	const unsigned long GetFame(void)				{ return m_DBData.m_Info.Fame;			}
	void SetFame(unsigned long dwFame);
	const unsigned long GetMileage(void)			{ return m_DBData.m_Info.Mileage;		}
	void SetMileage(unsigned long dwMileage)		{ m_DBData.m_Info.Mileage = dwMileage;	}
    
	bool IsAdmin(void) const		                { return (0 < m_DBData.m_cAdminLevel);  }
    unsigned char GetAdminLevel() const             { return m_DBData.m_cAdminLevel;        }
	void SetAdminLevel(unsigned char cAdminLevel)	{ m_DBData.m_cAdminLevel = cAdminLevel;	}
    
	bool IsRideArms(void) const						{ return (0 != m_dwRideArmsCID);		}
	unsigned long GetRideArmsCID(void) const		{ return m_dwRideArmsCID;				}
	void Ride(unsigned long dwArmsID)				{ m_dwRideArmsCID = dwArmsID;			}
	void GetOff(void)								{ m_dwRideArmsCID = 0;					}

	void SetProtectGateCID(unsigned long dwCID)		{ m_dwProtectGateCID = dwCID;			}

	const RejectOption& GetRejectOption(void)		{ return m_RejectOption;				}

	CAggresiveCreature* GetDuelOpponent(void)		{ return m_FightInfo.m_pDuelOpponent;	}
	bool SetDuelOpponent(CCharacter* pCharacter);

	CMonster* GetSummonee(void)				{ return m_lpSummonee;			}
	void SetSummonee(CMonster* lpSummonee)	{ m_lpSummonee = lpSummonee;	}

    bool CanShout(void) const;
    void Shouted(void);

	unsigned long GetLastTime()	const	{ return m_dwLastTime;	}
    
	CFriendList& GetFriendList(void) 	{ return m_friendList;	}
    CBanList&    GetBanList(void)    	{ return m_banList;		}    

	inline void SaveToDBData();
    inline Item::CItemContainer* GetItemContainer(unsigned char cPos);

	Item::CInventoryContainer&		GetInventory(void)		{ return m_Inventory;	}
    Item::CEquipmentsContainer&		GetEquipments(void)		{ return m_Equipments;  }
    Item::CListContainer&			GetExtra(void)			{ return m_ExtraSpace;  }
    Item::CExchangeContainer&		GetExchange(void)		{ return m_Exchange;    }
    Item::CDepositContainer&		GetDeposit(void)		{ return m_Deposit;     }
	Item::CCharacterShopContainer&	GetStall(void)			{ return m_Stall;		}
	Item::CTempInvenContainer&		GetTempInven(void)		{ return m_TempInven;	}

    // DB중계서버의 요청으로 아이템을 임시인벤에 넣어준다.
    bool GiveItemByDBAgentRequest(GiveItemInfo& giveItemInfo);

	inline unsigned short*			GetHistoryQuest(void)	{ return m_wHistoryQuest;	}
	inline Quest::ExecutingQuest*	GetExecutingQuest(void)	{ return m_ExecutingQuest;	}

	const FightStatus& GetEtcTypeStatus(Creature::StatusType eType)	{ return m_EtcTypeStatus[eType];	}

	Broadcast2nd::CSerializeCharacterData&  GetSerializeData(void)  { return m_SerializeCharacterData;	}

	friend class Skill::CFunctions;

    void SetOperationFlag(OperationFlag eOperationFlag)		{ m_cOperationFlags |= eOperationFlag;				}
	void ResetOperationFlag(OperationFlag eOperationFlag)	{ m_cOperationFlags &= ~eOperationFlag;				}
    bool IsOperationFlagSet(OperationFlag eOperationFlag)	{ return 0 != (m_cOperationFlags & eOperationFlag); }

    unsigned long GetLastMoveUpdateTime(void) const					{ return m_dwLastUpdateExTime;				}
    void SetLastMoveUpdateTime(unsigned long dwLastUpdateTime)		{ m_dwLastUpdateExTime = dwLastUpdateTime;	}
    
    unsigned char GetMoveUpdateCount(void) const					{ return m_cMoveUpdateExCount;				}
    void IncrementMoveUpdateCount(void)								{ ++m_cMoveUpdateExCount;					}

	bool IsChatBan(void) const			{ return m_bChatBan;	}
	void SetChatBan(bool bBan)			{ m_bChatBan = bBan;	}

	void SetGMModelFlag(unsigned char cGMModelFlag)		{ m_cGMModelFlag = cGMModelFlag;	}
	unsigned char GetGMModelFlag()						{ return m_cGMModelFlag;			}

    unsigned char GetNameChangeCount() const { return m_cNameChangeCount; }
    void SetNameChangeCount(unsigned char cNameChangeCount) { m_cNameChangeCount = cNameChangeCount; }

	unsigned char GetRealmWarFlag() const			{ return m_cRealmWarFlag;	}
	unsigned char GetGuildWarFlag() const			{ return m_cGuildWarFlag;	}
	void	SetRealmWarFlag(unsigned char cFlag)	{ m_cRealmWarFlag = cFlag;	}
	void	SetGuildWarFlag(unsigned char cFlag);

	unsigned char GetTacticsFlag() const			{ return m_cTactics;		}
	void SetTacticsFlag(unsigned char cTactics)		{ m_cTactics = cTactics;	}

	// 국가 전쟁 공헌훈장 포인트.

	virtual unsigned char GetRealmPoint()			{ return m_cRealmPoint;			}

	void SetRealmPoint(unsigned char cRealmPoint)	{ m_cRealmPoint = cRealmPoint;	}


	int GetMagicChancePoint()
	{
		// 매찬 포인트 계상하는곳
		int iMCPoint = 0;

		// edtih 2009.06.20  룬에의한 매직찬스값 추가
		iMCPoint = (int)m_EquipStatus.m_fLuckResistRate;
		
//		if(GetRealmPoint() > 0)
//			iMCPoint = GetRealmPoint()*5;	// 랠름포인트는 1개당 5퍼센트씩

		// 매찬에 스틸핸드가 미치는 영향.
		if(GetEnchantInfo().GetFlag(Skill::SpellID::StealHand))
		{
			// 3-0부터 1래벨당 1퍼센트씩 매찬기능 추가
			int iLevel = GetEnchantLevel(Skill::SpellID::StealHand)-12;

			if(iLevel > 0)
				iMCPoint += iLevel;
		}

		return iMCPoint;
	}


	void SetPlayTime(unsigned int dwPlayTime) 
	{ 
		m_StartTime = CTime::GetCurrentTime();
		m_dwPlayTime = dwPlayTime; 
	}

	unsigned int GetPlayTime() { return m_dwPlayTime; }

	void SetPremiumService(long lPremiumTime, int iPremiumType) 
	{ 
		// 프리미엄 타임 남은 시간
		m_StartPremiumTime = CTime::GetCurrentTime();		
		m_lPremiumTime = lPremiumTime; 
		m_iPremiumType = iPremiumType;
	}

	long GetPremiumTime() { return m_lPremiumTime; }
	int GetPremiumType() { return m_iPremiumType; }

	float GetPremiumPt()
	{
		if(m_iPremiumType == 1)
			return 0.5f;
		else if(m_iPremiumType == 2)
			return 0.3f;

		return 0.0f;
	}

	bool CheckPremiumTime()
	{
		if(m_lPremiumTime == 0)
			return false;

		CTime		nowTime = CTime::GetCurrentTime();
		CTimeSpan	tm(m_StartPremiumTime.GetDay(), m_StartPremiumTime.GetHour(), m_StartPremiumTime.GetMinute(), m_StartPremiumTime.GetSecond());    
		CTime		tmPrev = nowTime-tm;

		int			iTime = (tmPrev.GetHour()*60)+tmPrev.GetMinute();

		// 
		iTime = m_lPremiumTime-iTime;

		if(iTime <= 0)
		{
			m_iPremiumType = 0;
			m_lPremiumTime = 0;
			return false;
		}

		return true;
	}

	float GetAwardPer()
	{
		float fEventAdd = 0.0f;
		if (CGameTimeMgr::GetInstance().GetCurrentEventTime() == GameTime::EVENT_All20)
			fEventAdd = 0.2f;
		else if (CGameTimeMgr::GetInstance().GetCurrentEventTime() == GameTime::EVENT_All50)
			fEventAdd = 0.5f;

		if(GameRYL::CHINA != CServerSetup::GetInstance().GetNationType())
			return 1.0f+fEventAdd;

		if(m_dwPlayTime == 0)
			return 1.0f+fEventAdd;

		CTime		nowTime = CTime::GetCurrentTime();
		CTimeSpan	tm(m_StartTime.GetDay(), m_StartTime.GetHour(), m_StartTime.GetMinute(), m_StartTime.GetSecond());    
		CTime		tmPrev = nowTime-tm;

		int			iTime = (tmPrev.GetHour()*60)+tmPrev.GetMinute();

		iTime += m_dwPlayTime;
/*
		// 테스트를 위해 플레이타음을 약간 바꿔준다.
		if(iTime <= 2)
		{
			return 1.0f;
		}
		else if(2 < iTime && iTime <= 4)
		{
			return 0.5f;
		}
*/
		if(iTime <= 180)
		{
			return 1.0f;
		}
		else if(180 < iTime && iTime <= 300)
		{
			return 0.5f;
		}

		return 0.0f;
	}

	// 교환 관련 메소드.

	unsigned long GetExchangeID() const { return m_dwExchangeID; }
	void SetExchangeID(unsigned long dwExchangeID) { m_dwExchangeID = dwExchangeID; }

	SPELL GetSpell() { return m_DBData.m_Spell; }
	void SetSpell(SPELL spell) { m_DBData.m_Spell = spell; }

	void SaveSpell(BOOL bDead = FALSE);


	// 길드 관련 메소드
	void SetGuildSafe(char cGuildSafe) { m_cGuildSafe = cGuildSafe; }
	char GetGuildSafe() { return m_cGuildSafe; }

	void SetDead(BOOL bDead) { m_bDead = bDead; }
	BOOL IsDead() { return m_bDead; }

	void SetRealmWarBuffer(BOOL bEnable) { m_bRealmWarBuffer = bEnable; }
	BOOL IsRealmWarBuffer() { return m_bRealmWarBuffer; }

	// 리스폰 타운 정보 아이디
	void SetRespawnTownID(unsigned long dwTownID) { m_dwRespawnTownID = dwTownID; }
	unsigned long GetRespawnTownID() const { return m_dwRespawnTownID; }

	void SetAdminAbilityPoint(int point) { m_iAdminAbilityPoint = point; }
	int GetAbilityPoint();
	int GetUseAbilityPoint() { return m_iUseAbilityPoint; }
	void UpdateUseAbilityPoint();

	unsigned short GetAbilityValue(int nAbilityType) 
	{
		if(nAbilityType >= Skill::Type::MAX_ABILITY_TYPE )
			return 0;

		return m_AbilityValue[nAbilityType]; 
	}


    CCharacter(unsigned long dwCID, unsigned long dwSessionID);
    virtual ~CCharacter();

protected:

	enum 
	{
		MAX_USING_MASTERY	= 2,	// 동시에 최대로 적용 가능한 마스터리수
		PAD_BYTE			= 3		// byte alignment 를 맞추기 위한 배열의 크기
	};

	CharacterFightInfo		m_FightInfo;				// 듀얼 정보
//	PeaceModeInfo			m_PeaceMode;				// 반전 모드 정보

    Item::CInventoryContainer      	m_Inventory;				// 인벤토리
    Item::CEquipmentsContainer  	m_Equipments;				// 장비
    Item::CListContainer        	m_ExtraSpace;				// 추가 공간
    Item::CExchangeContainer    	m_Exchange;		    		// 교환창
    Item::CDepositContainer     	m_Deposit;          		// 창고
	Item::CCharacterShopContainer	m_Stall;					// 노점상
	Item::CTempInvenContainer		m_TempInven;				// 임시 인벤토리

    GivenItemList           m_GivenItemList;            // DB중계로부터 받은 아이템들의 CreationID

    CFriendList             m_friendList;       		// 친구 리스트
    CBanList                m_banList;          		// 거부 리스트

	unsigned short			m_wHistoryQuest[Quest::MAX_HISTORY_QUEST];		// 수행 완료한 퀘스트
	Quest::ExecutingQuest	m_ExecutingQuest[Quest::MAX_EXECUTING_QUEST];	// 수행중인 퀘스트
  
	CharacterDBData		    m_DBData;					// DB에서 읽어 오는 데이터    
	RejectOption		    m_RejectOption;				// 각종 거부 옵션	 

	FightStatus				m_EtcTypeStatus[Creature::MAX_STATUS_TYPE];		// 특수한 경우에 사용되는 스탯들
    
	unsigned long           m_dwUID;					        // 유저 ID
    unsigned long           m_dwSessionID;                      // 세션 ID - DB에 업데이트할 때 쓰인다.
    unsigned long           m_dwLastShoutTime;                  // 마지막으로 외친 시간
    unsigned long           m_dwLastSendPartyAttackInfoTime;    // 마지막으로 파티원에게 공격당했다는 정보를 보낸 시간
	int						m_nLogoutCount;				        // 로그아웃시까지 남은 카운트 수 저장 (0 이하가 될 수 있으므로 반드시 signed로!)
	int						m_nDBUpdateCount;			        // DB Update때까지 남은 카운트 수 저장 (0 이하가 될 수 있으므로 반드시 signed로!)

	unsigned long			m_dwExchangeID;						// 교환할 유저 ID.

	unsigned char			m_cGMModelFlag;						// GM 모델로 변경 체크.
    
    CGameClientDispatch*    m_lpGameClientDispatch;

	CMonster*				m_lpSummonee;
    unsigned long           m_dwLastUpdateExTime;   	// 마지막 이동 업데이트 시간
	unsigned long			m_dwRideArmsCID;			// 타고 있는 공성관련 병기의 CID
	unsigned long			m_dwProtectGateCID;			// 성문막기를 하고 있는 성문의 CID
	unsigned long			m_dwRespawnSpeed;			// 리스폰 속도 (pulse 사용)

    LAST_DEAD_TYPE          m_eLastDeadType;            // 마지막에 어떻게 죽었는지 체크

    Broadcast2nd::CSerializeCharacterData   m_SerializeCharacterData;     // 캐릭터 데이터 보관 및 변경된 데이터 보관

    char            m_szAccountName[CHAR_INFOST::MAX_ACCOUNT_LEN];
	int			m_KillSpreeLevel;						//hz added for killspreelevel.
	char 			m_cUsingMastery[MAX_USING_MASTERY];		// 적용중인 마스터리 종류
	char 			m_cConsumeMPCount;						// MP 소모까지 남은 카운트 수 저장
    char 			m_cOperationFlags;						// 각종 처리가 완료되었는지를 검사하는 루틴
    char 			m_cHandPos;								// HandPos
	char			m_cRidePos;								// Ride Pos
    
    char 			m_cMoveUpdateExCount;					// 이동 업데이트 Count
	bool			m_bChatBan;								// 채팅 금지    
    unsigned char   m_cNameChangeCount;                     // 이름 바꿀 수 있는 회수

	unsigned char	m_cGuildWarFlag;						// 길드 전쟁 참여 플래그
	unsigned char	m_cRealmWarFlag;						// 국가 전챙 참여 플래그
	unsigned char	m_cTactics;								// 용병 플레그.

	char			m_cGuildSafe;							// edith 2008.03.15 길드 입출금 관련 플래스

	unsigned char	m_cRealmPoint;							// 국가 전쟁 공헌훈장 포인트.

	unsigned int	m_dwPlayTime;							// 중국판을 위한 게임 연속 플레이시간.
	CTime			m_StartTime;

	long			m_lPremiumTime;							// 프리미엄 서비스
	int				m_iPremiumType;
	CTime			m_StartPremiumTime;

	BOOL			m_bDead;
	BOOL			m_bRealmWarBuffer;

	unsigned long	m_dwRespawnTownID;

	// WORK_LIST 2.4 계정 국적을 게임서버의 캐릭터가 가지도록 구현
	unsigned char	m_cAccountNation;						// 계정 국적

	char			m_bPadding[PAD_BYTE];					// 4Byte Alignment를 맞추기 위한 도구

	// edith 2009.10.22 어빌리티 작업중..
	int				m_iAbilityPoint;
	int				m_iUseAbilityPoint;
	int				m_iAdminAbilityPoint;		// 어드민이 임시적으로 주는 포인트 (기본 0, 값은 저장되지 않음)

	// edith 2009.11.10 어빌리티 능력치 제어
	unsigned short	m_AbilityValue[Skill::Type::MAX_ABILITY_TYPE];
};

inline void CCharacter::SaveToDBData()
{
	m_DBData.m_Info.Exp		= m_CreatureStatus.m_nExp;
	m_DBData.m_Info.Level	= m_CreatureStatus.m_nLevel;
	m_DBData.m_Info.HP		= m_CreatureStatus.m_nNowHP;
	m_DBData.m_Info.MP		= m_CreatureStatus.m_nNowMP;

	m_DBData.m_Pos.LastPoint.fPointX = m_CurrentPos.m_fPointX;
	m_DBData.m_Pos.LastPoint.fPointY = m_CurrentPos.m_fPointY;
	m_DBData.m_Pos.LastPoint.fPointZ = m_CurrentPos.m_fPointZ;
}


inline Item::CItemContainer* CCharacter::GetItemContainer(unsigned char cPos)
{
    switch(cPos)
    {
		case TakeType::TS_EQUIP:		return &m_Equipments;
		case TakeType::TS_INVEN:		return &m_Inventory;
		case TakeType::TS_TEMP:			return &m_ExtraSpace;
		case TakeType::TS_EXTRA:		return &m_ExtraSpace;
		case TakeType::TS_EXCHANGE:		return &m_Exchange;
		case TakeType::TS_DEPOSIT:		return &m_Deposit;
		case TakeType::TS_STALL:		return &m_Stall;
		case TakeType::TS_TEMPINVEN:	return &m_TempInven;
	}

    return NULL;
}

#endif