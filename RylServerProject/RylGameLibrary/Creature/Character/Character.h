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


// ���� ����
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

    // DB�߰迡�� ���̺�� �о TempInven���� �־��� �������� CreationID�� �����Ѵ�.
    typedef std::vector<unsigned long>  GivenItemList;

	enum Const
	{
		LOGOUT_PULSE    			= 5,	
		LOGOUT_COUNT    			= 30,//hz was 5 if cause any problems 14 seconds stay if dc or char select

		DBUPDATE_PULSE  			= 10,	
		DBUPDATE_COUNT  			= 180,

		BATTLE_GROUND_PULSE 		= 600,

		RESPAWN_PULSE				= 100,	// 10 ��

		MELEE_ATTACK_RANGE			= 10,	// ĳ������ ���� ���� ���� �Ÿ�
		RANGE_ATTACK_RANGE			= 50	// ĳ������ ���Ÿ� ���� ���� �Ÿ�
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
		// edith �������� ������ ������
		WEAPON_DURABILITY_DECRESE_PERCENTAGE = 100,	// 200
		ARMOUR_DURABILITY_DECRESE_PERCENTAGE = 25,	// 50
		SHIELD_DURABILITY_DECRESE_PERCENTAGE = 10,	// 20

		STARTAXDURABILITY = 10,				// �ִ� ������ ������ ���۰�
		DOWNMAXDURABILITY = 10				// �ִ� ������ ������ 
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
	
    bool Initialize(CGameClientDispatch* lpGameClientDispatch);		// �ʱ�ȭ

	// --------------------------------------------------------------------------------------------
	// ĳ���� �α��ν� ó���� �κе� --------------------------------------------------------------
	
    void PrepareLogin(void); 
	bool Login(void);	        // ĳ���� ������ ĳ���� ���̺� �ε��(�α�����)	

	bool IsLogined(void) { return IsOperationFlagSet(CHAR_INFO_LOADED); }
		
	// --------------------------------------------------------------------------------------------
	// ĳ���� �α׾ƿ��� ó���� �κе� ------------------------------------------------------------
	
	inline void ResetLogoutCount(void)	{ m_nLogoutCount = LOGOUT_COUNT; }
	inline bool StillAlive(void)		{ return --m_nLogoutCount >= 0; }
	inline void SetDBUpdateCount(LONG nDBUpdateCount) { m_nDBUpdateCount = nDBUpdateCount; }
	
    // ĳ���͸� �ʿ��� �α׾ƿ���Ű��, �����͸� DB�� ������.
    bool Logout(DBUpdateData::UpdateType eUpdateType = DBUpdateData::LOGOUT);

	
	// --------------------------------------------------------------------------------------------
	// ĳ���� ������ ������Ʈ - CharacterDataUpdate.cpp -------------------------------------------

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
	// ������, �� �̵� �� ��� ���� �޼ҵ� - CharacterItem.cpp ------------------------------------

	inline unsigned long GetGold(void) const { return m_DBData.m_Info.Gold; }
    unsigned long GetGold(unsigned char cPos);
	bool AddGold(unsigned long dwGold, bool bNotice);
    bool DeductGold(unsigned long dwGold, bool bNotice);
    bool MoveGold(unsigned long dwGold, unsigned char cSrcPos, 
        unsigned char cDstPos, unsigned short& usError);            // �� �ű��

	bool DeductMileage(unsigned long dwMileage, bool bNotice);

	bool MoveItem(const TakeType takeType, bool bChk = true);		// ������ �̵� �� ����
	bool SwapItem(const TakeType SrcType, const TakeType DstType);	// ������ ��ġ ����
	Item::CItem* SplitItem(const TakeType takeType);				// ������ ���� ������

	bool Pickup(Item::CItem* lpItem, Item::ItemPos dstPos);			// ������ ����
	Item::CItem* Drop(Item::ItemPos SrcPos, unsigned char cNum);	// ������ ������

	// �������� �κ��丮�� �־��ش�. (�ڸ��� ������ �ӽ� �κ��丮�� �ִ´�.)
	bool GiveItem(Item::CItem* lpItem);
	bool TestItem(Item::CItem* lpItem);	// �������� ���� �� �ִ��� üũ�Ѵ�.

	bool UseLottery(unsigned short usItemID);
	bool UseCashItem(unsigned long dwSender, unsigned long dwReceiver, Item::ItemPos itemPos, unsigned short wCashType, unsigned short& wError);
	bool UseStartKit(unsigned short wObjectType, unsigned short& wError);

	bool CheckItemRuneSkill(Item::ItemPos SrcPos, Item::ItemPos DstPos, bool bChk);		// ������ �̵��� �� �Ӽ� üũ

	const Broadcast2nd::CNetworkEquipGrade GetNetworkEquipGrade(void);					// ����Ʈ�� ���� ��� �׷��̵� ����

	// --------------------------------------------------------------------------------------------
	// ĳ���� �κ��丮 ����(������ Get, Set, Reset) - CharacterInventoryItem.cpp ------------------

public:

	Item::CItem* GetItem(Item::ItemPos SrcPos);
	bool SetItem(Item::ItemPos SrcPos, Item::CItem* lpItemBase);
	bool RemoveItem(Item::ItemPos SrcPos);


	// --------------------------------------------------------------------------------------------
	// ������ �Ȱ� ��� �����ϴ� �޼��� - CharacterTradeItem.cpp ----------------------------------

	unsigned long RepairItem(const unsigned long dwNPCID, Item::ItemPos itemPos, unsigned short& wError);

	unsigned long RepairAllItem(const unsigned long dwNPCID, unsigned short& wError);

	Item::CItem* SellToCharacter(CCharacter *lpCustomer, unsigned short wKindItem, TakeType takeType, 
		Item::CItem* lpRequestItem, unsigned long &dwPrice, unsigned short wCouponID, unsigned short &usError);


	// --------------------------------------------------------------------------------------------
	// �������ͽ� ���� �޼ҵ� - CharacterStatus.cpp -----------------------------------------------

	// �ɷ�ġ �� �������ͽ� �����͸� DBData���� �о ����. HP, MP�� Full�� �ȴ�.
	bool CalculateStatusData(bool bFullHPandMP);				
	bool CalculateAbility(const SKILL& skill);	// �����Ƽ�� ������ �����Ѵ�.

	bool ChangeClass(unsigned char cClassType);					// CClass ����
	bool IncrementExp(unsigned long dwExp);						// ������ ��Ȳ�� �Ǹ� �������Ѵ�.
	bool GetHuntingExp(CAggresiveCreature* lpDeadCreature, unsigned long dwExp, unsigned char cMemberNum);			

	unsigned short AddState(unsigned char Type_In, ChState& ChState_Out);
	bool StateRedistribution(ChState& State);					// ������ �ش� Ŭ������ ó�� ���·� ������ �Լ�
	bool StatusRetrain(ChState& State, Item::ItemPos InvenPos);	// ������ ������ �����޴� ó���� �ϴ� �Լ� (������ �� ���)

	bool ChangeWeaponAndShield(unsigned char cSelect);			// ���� �ٲٱ�
	bool ChangeRide(unsigned char cSelect);			// �� Ÿ�� ������

	bool CalculateEquipDurability(unsigned short wAttackType);
	unsigned char GetCurrentDurability(unsigned char cEquipmentIndex);
	bool CalculateAllEquipDurability(unsigned char cDecreasePersent);

    // HP�� MP�� Regen�Ѵ�. �߰� ���� 0�� ���� �⺻ ������ Regen�Ѵ�.
	virtual bool RegenHPAndMP(unsigned short usAdditionalHP, unsigned short usAdditionalMP, bool bAddDefaultRegenValue);

	// Admin ������� ������ �ٿ� ��ų�� ����ϴ� �Լ� (���� 1�� �ʱ� Ŭ������ ����� ������.)
	bool InitLevel1Char(unsigned char cClassType);

	bool CheckRenameWarrant(Item::ItemPos InvenPos, bool bItemAccept);

	// --------------------------------------------------------------------------------------------
	// ���� ���� �޼ҵ� - CharacterFight.cpp ------------------------------------------------------

	virtual const int CalculateFixLevelGap(CAggresiveCreature *pDefender);

	void Casting(AtType attackType, AtNode& attackNode);

	bool AttackCID(AtType attackType, AtNode& attackNode, unsigned short& wError);
	bool Attack(AtType attackType, unsigned char cDefenderNum, CAggresiveCreature** pDefenders, unsigned char* cDefenserJudges, unsigned short* wDefenderMPHeal);
	bool AttackUsingBow(unsigned short wType);
	bool UseAmmo(Item::CUseItem* pUseItem);
    
	virtual bool Dead(CAggresiveCreature* pOffencer);
	bool Respawn(unsigned char cType = 0, const Position& Pos = Position::Position());
	bool AutoRespawn(void);		// ��Ʋ �׶��忡�� �ʷ� ���� ��ġ�� �ڵ� ������

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
	// ��ų ���� �޼ҵ� - CharacterSkill.cpp ------------------------------------------------------

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
	// ������ ���� �޼ҵ� - CharacterQuickSlot.cpp ------------------------------------------------

	void UpdateQuickSlotSkill(SKILLSLOT Slot);
	bool MoveQuickSlot(const TakeType takeType, const unsigned short usSkillID, unsigned char cLockCount, unsigned char cSkillLevel);


	// --------------------------------------------------------------------------------------------
	// ĳ���� ��Ʈ�� ���� �޼ҵ� - CharacterControl.cpp -------------------------------------------

	bool DropItem(unsigned short usProtoTypeID, unsigned char cNum);

	bool MovePos(Position Pos, char cZone, const bool bSitDown);
	bool MoveZone(POS NewPos, char cZone, char Channel);
	bool Kill(CAggresiveCreature* lpAttacker);

	bool NotifyInfo(unsigned long dwAdminCID);

	bool DuelInit(unsigned char cCmd);

	// �����, �������� �ش��ϴ� ������ �̵�
	void MoveToGuildWarZone();
	void MoveToRealmWarZone();

	// --------------------------------------------------------------------------------------------
	// ĳ���� ����Ʈ ���� �޼ҵ� - CharacterQuest.cpp ---------------------------------------------

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
	// ģ�� ���� ������Ʈ

	void FriendInfoUpdate(unsigned long dwUID, unsigned long dwCID, unsigned long dwGID, unsigned short wClass, char cLevel, unsigned long dwServerID);
		
private:

	Quest::ErrorCode ExecuteEvent(Quest::ExecutingQuest executingQuest, const Quest::TriggerNode* triggerNode, Position Pos);
	void EventEnd(unsigned short wQuestID, bool bSave);
	void EventAward(unsigned long dwExp, unsigned long dwGold, unsigned long dwFame, unsigned long dwMileage);

public:

	// ��ų ���� //
	void SkillClear();	

	bool ClearGarbage(vector<Item::ItemGarbage>& vecItemGarbage, unsigned char cCmd);

	bool HasExecutingQuest(unsigned short wQuestID);
	bool HasHistoryQuest(unsigned short wQuestID);
	bool InsertHistoryQuest(unsigned short wQuestID);
	bool DeleteHistoryQuest(unsigned short wQuestID);

	virtual void CalculateStatusByQuest(void);


	// --------------------------------------------------------------------------------------------
	// ����  ���� �޼ҵ�  -------------------------------------------------------------------------

	CGameClientDispatch* GetDispatcher(void) { return m_lpGameClientDispatch; }
    void SetDispatcher(CGameClientDispatch* lpGameClientDispatch);


	// --------------------------------------------------------------------------------------------
	// ��Ÿ �޼ҵ�  -------------------------------------------------------------------------------
	
	bool GetCharacterView(CHAR_VIEW& charView);

	// TODO : AggresiveCreature�� �ø��� GetName���� �����սô�. (Creature�� �÷��� ����...)
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

	bool BindPositionToNPC(const unsigned long dwNPCID);					// ������ ��ġ�� NPC��ġ�� ���ε�
	
	bool ControlOption(const RejectOption Reject, bool bLogin);				// ���� �ɼ� ����

	void UpgradeRespawnSpeedByEmblem(unsigned char cUpgradeType, unsigned char cUpgradeStep);
	void DegradeRespawnSpeedByEmblem();

	virtual unsigned long GetUID(void) const	{ return m_dwUID;					}

	char GetConsumeMPCount(void)				{ return m_cConsumeMPCount;			}
	unsigned short GetClass(void)				{ return m_DBData.m_Info.Class;		}
    unsigned char GetLevel(void)            	{ return m_DBData.m_Info.Level;		}

	CClass::RaceType GetRace(void)				{ return static_cast<CClass::RaceType>(m_DBData.m_Info.Race);	}

	unsigned char GetNation(void) const;
	// WORK_LIST 2.4 ���� ������ ���Ӽ����� ĳ���Ͱ� �������� ����
	void SetNation(unsigned char cNation)		{ m_cAccountNation = cNation;		}
	unsigned long GetPID(void) const			{ return m_DBData.m_Info.PID;		}
	void SetPID(unsigned long dwPID);
	unsigned long GetGID(void) const			{ return m_DBData.m_Info.GID;		}
	void SetGID(unsigned long dwGID);

    unsigned char GetHair(void)  				{ return m_DBData.m_Info.Hair; 		}
    unsigned char GetFace(void)  				{ return m_DBData.m_Info.Face; 		}
    unsigned char GetSex(void)   				{ return m_DBData.m_Info.Sex;		}			// 0 : ����, 1 : ����
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

    // DB�߰輭���� ��û���� �������� �ӽ��κ��� �־��ش�.
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

	// ���� ���� �������� ����Ʈ.

	virtual unsigned char GetRealmPoint()			{ return m_cRealmPoint;			}

	void SetRealmPoint(unsigned char cRealmPoint)	{ m_cRealmPoint = cRealmPoint;	}


	int GetMagicChancePoint()
	{
		// ���� ����Ʈ ����ϴ°�
		int iMCPoint = 0;

		// edtih 2009.06.20  �鿡���� ���������� �߰�
		iMCPoint = (int)m_EquipStatus.m_fLuckResistRate;
		
//		if(GetRealmPoint() > 0)
//			iMCPoint = GetRealmPoint()*5;	// ��������Ʈ�� 1���� 5�ۼ�Ʈ��

		// ������ ��ƿ�ڵ尡 ��ġ�� ����.
		if(GetEnchantInfo().GetFlag(Skill::SpellID::StealHand))
		{
			// 3-0���� 1������ 1�ۼ�Ʈ�� ������� �߰�
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
		// �����̾� Ÿ�� ���� �ð�
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
		// �׽�Ʈ�� ���� �÷���Ÿ���� �ణ �ٲ��ش�.
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

	// ��ȯ ���� �޼ҵ�.

	unsigned long GetExchangeID() const { return m_dwExchangeID; }
	void SetExchangeID(unsigned long dwExchangeID) { m_dwExchangeID = dwExchangeID; }

	SPELL GetSpell() { return m_DBData.m_Spell; }
	void SetSpell(SPELL spell) { m_DBData.m_Spell = spell; }

	void SaveSpell(BOOL bDead = FALSE);


	// ��� ���� �޼ҵ�
	void SetGuildSafe(char cGuildSafe) { m_cGuildSafe = cGuildSafe; }
	char GetGuildSafe() { return m_cGuildSafe; }

	void SetDead(BOOL bDead) { m_bDead = bDead; }
	BOOL IsDead() { return m_bDead; }

	void SetRealmWarBuffer(BOOL bEnable) { m_bRealmWarBuffer = bEnable; }
	BOOL IsRealmWarBuffer() { return m_bRealmWarBuffer; }

	// ������ Ÿ�� ���� ���̵�
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
		MAX_USING_MASTERY	= 2,	// ���ÿ� �ִ�� ���� ������ �����͸���
		PAD_BYTE			= 3		// byte alignment �� ���߱� ���� �迭�� ũ��
	};

	CharacterFightInfo		m_FightInfo;				// ��� ����
//	PeaceModeInfo			m_PeaceMode;				// ���� ��� ����

    Item::CInventoryContainer      	m_Inventory;				// �κ��丮
    Item::CEquipmentsContainer  	m_Equipments;				// ���
    Item::CListContainer        	m_ExtraSpace;				// �߰� ����
    Item::CExchangeContainer    	m_Exchange;		    		// ��ȯâ
    Item::CDepositContainer     	m_Deposit;          		// â��
	Item::CCharacterShopContainer	m_Stall;					// ������
	Item::CTempInvenContainer		m_TempInven;				// �ӽ� �κ��丮

    GivenItemList           m_GivenItemList;            // DB�߰�κ��� ���� �����۵��� CreationID

    CFriendList             m_friendList;       		// ģ�� ����Ʈ
    CBanList                m_banList;          		// �ź� ����Ʈ

	unsigned short			m_wHistoryQuest[Quest::MAX_HISTORY_QUEST];		// ���� �Ϸ��� ����Ʈ
	Quest::ExecutingQuest	m_ExecutingQuest[Quest::MAX_EXECUTING_QUEST];	// �������� ����Ʈ
  
	CharacterDBData		    m_DBData;					// DB���� �о� ���� ������    
	RejectOption		    m_RejectOption;				// ���� �ź� �ɼ�	 

	FightStatus				m_EtcTypeStatus[Creature::MAX_STATUS_TYPE];		// Ư���� ��쿡 ���Ǵ� ���ȵ�
    
	unsigned long           m_dwUID;					        // ���� ID
    unsigned long           m_dwSessionID;                      // ���� ID - DB�� ������Ʈ�� �� ���δ�.
    unsigned long           m_dwLastShoutTime;                  // ���������� ��ģ �ð�
    unsigned long           m_dwLastSendPartyAttackInfoTime;    // ���������� ��Ƽ������ ���ݴ��ߴٴ� ������ ���� �ð�
	int						m_nLogoutCount;				        // �α׾ƿ��ñ��� ���� ī��Ʈ �� ���� (0 ���ϰ� �� �� �����Ƿ� �ݵ�� signed��!)
	int						m_nDBUpdateCount;			        // DB Update������ ���� ī��Ʈ �� ���� (0 ���ϰ� �� �� �����Ƿ� �ݵ�� signed��!)

	unsigned long			m_dwExchangeID;						// ��ȯ�� ���� ID.

	unsigned char			m_cGMModelFlag;						// GM �𵨷� ���� üũ.
    
    CGameClientDispatch*    m_lpGameClientDispatch;

	CMonster*				m_lpSummonee;
    unsigned long           m_dwLastUpdateExTime;   	// ������ �̵� ������Ʈ �ð�
	unsigned long			m_dwRideArmsCID;			// Ÿ�� �ִ� �������� ������ CID
	unsigned long			m_dwProtectGateCID;			// �������⸦ �ϰ� �ִ� ������ CID
	unsigned long			m_dwRespawnSpeed;			// ������ �ӵ� (pulse ���)

    LAST_DEAD_TYPE          m_eLastDeadType;            // �������� ��� �׾����� üũ

    Broadcast2nd::CSerializeCharacterData   m_SerializeCharacterData;     // ĳ���� ������ ���� �� ����� ������ ����

    char            m_szAccountName[CHAR_INFOST::MAX_ACCOUNT_LEN];
	int			m_KillSpreeLevel;						//hz added for killspreelevel.
	char 			m_cUsingMastery[MAX_USING_MASTERY];		// �������� �����͸� ����
	char 			m_cConsumeMPCount;						// MP �Ҹ���� ���� ī��Ʈ �� ����
    char 			m_cOperationFlags;						// ���� ó���� �Ϸ�Ǿ������� �˻��ϴ� ��ƾ
    char 			m_cHandPos;								// HandPos
	char			m_cRidePos;								// Ride Pos
    
    char 			m_cMoveUpdateExCount;					// �̵� ������Ʈ Count
	bool			m_bChatBan;								// ä�� ����    
    unsigned char   m_cNameChangeCount;                     // �̸� �ٲ� �� �ִ� ȸ��

	unsigned char	m_cGuildWarFlag;						// ��� ���� ���� �÷���
	unsigned char	m_cRealmWarFlag;						// ���� ��ì ���� �÷���
	unsigned char	m_cTactics;								// �뺴 �÷���.

	char			m_cGuildSafe;							// edith 2008.03.15 ��� ����� ���� �÷���

	unsigned char	m_cRealmPoint;							// ���� ���� �������� ����Ʈ.

	unsigned int	m_dwPlayTime;							// �߱����� ���� ���� ���� �÷��̽ð�.
	CTime			m_StartTime;

	long			m_lPremiumTime;							// �����̾� ����
	int				m_iPremiumType;
	CTime			m_StartPremiumTime;

	BOOL			m_bDead;
	BOOL			m_bRealmWarBuffer;

	unsigned long	m_dwRespawnTownID;

	// WORK_LIST 2.4 ���� ������ ���Ӽ����� ĳ���Ͱ� �������� ����
	unsigned char	m_cAccountNation;						// ���� ����

	char			m_bPadding[PAD_BYTE];					// 4Byte Alignment�� ���߱� ���� ����

	// edith 2009.10.22 �����Ƽ �۾���..
	int				m_iAbilityPoint;
	int				m_iUseAbilityPoint;
	int				m_iAdminAbilityPoint;		// ������ �ӽ������� �ִ� ����Ʈ (�⺻ 0, ���� ������� ����)

	// edith 2009.11.10 �����Ƽ �ɷ�ġ ����
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