#ifndef _CNPC_H_
#define _CNPC_H_

#include <Item/Item.h>
#include <ScriptEngine/ScriptEngine.h>
#include <Creature/Creature.h>


// 전방 참조
namespace Quest 
{
	struct QuestNode;
};


class CNPC : public CCreature
{
public:

	enum Const
	{
		MAX_CONTACT_RANGE	= 15,			
		DISPLAY_CYCLE		= 1800000		// 30분
	};

	struct GoodInfo
	{
		unsigned short	m_wItemID;
		unsigned char	m_cRace;
		unsigned char	m_cTabPage;

		Item::EquipType::Grade	m_ePreGrade;
		Item::EquipType::Grade	m_eCurrentGrade;

		GoodInfo();
		GoodInfo(unsigned short wItemID, unsigned char cRace, unsigned char cTabPage);
		bool operator < (const GoodInfo& rhs) { return (m_wItemID < rhs.m_wItemID); }
	};


	CNPC(unsigned long dwCID, int nZone, int nTownID, bool bBelongToCastle);
	virtual ~CNPC();

	// 기본은 스크립트에서 읽어오되, 공성의 Guild 소속에 따라 변경될 수 있습니다.
	virtual unsigned char GetNation(void) const;
	void SetNation(unsigned char cNation) { m_cNation = cNation; }

	virtual EnemyCheck::EnemyType IsEnemy(CCreature* lpTarget, unsigned char* cResult = NULL ) { return EnemyCheck::EC_NEUTRAL; }
	virtual CParty* GetParty(void) { return 0;    }
    virtual bool IsPeaceMode(void) { return true; }
	virtual bool IsRideArms(void) const	{ return false; }
    virtual unsigned long GetGID(void) const { return m_dwGID; }

    bool IsBelongToCastle(void)	const; 

    int	GetZone(void) { return m_nZone;	}	
	unsigned long GetTownID(void) const			{ return m_dwTownOrNameID;	}
	unsigned long GetCastleNameID(void) const	{ return m_dwTownOrNameID;	}
	unsigned long GetDisplayTime(void) const	{ return m_dwDisplayTime;	}

	void SetGID(unsigned long dwGID)	{ m_dwGID = dwGID; }

	bool GetQuest(CCharacter* lpCharacter, unsigned short wQuestID, Quest::QuestNode** lppQuestNode);
    virtual unsigned long RepairItem(Item::CEquipment* pEquipment, unsigned long& dwCurrentGold);

    Item::CItem* SellToCharacter(CCharacter* lpCustomer, unsigned short wKindItem, TakeType takeType, 
		Item::CItem* lpRequestItem, unsigned long& dwPrice, unsigned short wCouponID, unsigned short &usError);

	unsigned char GetEquipShopInfo(unsigned char cRace, unsigned char cTabPage, 
		unsigned short *aryItemID, unsigned char *aryItemGrade);

	static bool LoadNPCInfo(void);

    // -- internal use for load NPC objects

    void SortGoods(void)	{ std::sort(m_Goods.begin(), m_Goods.end());   }
	void SortQuests(void)	{ std::sort(m_Quests.begin(), m_Quests.end()); }

    void SetFlags(unsigned long dwFlags) { m_dwFlags |= dwFlags; }   // 상인 속성 추가로 세팅
    void AddGoodsToNPC(GoodInfo goodInfo)           { m_Goods.push_back(goodInfo);	}
	void AddQuestsToNPC(unsigned short wQuestID)	{ m_Quests.push_back(wQuestID); }

	void SetItemDropGrade(unsigned char cDropType, unsigned char cGradeF, unsigned char cGradeD, 
		unsigned char cGradeC, unsigned char cGradeB, unsigned char cGradeA);
	void SetItemDropBaseNum(unsigned char cDropType, unsigned char cGradeF, unsigned char cGradeD, 
		unsigned char cGradeC, unsigned char cGradeB, unsigned char cGradeA);

	void SetRandomGrade(void);
	void SetNPCPosition(Position& position) { m_CurrentPos = position; }

private:

    std::vector<GoodInfo>		m_Goods;
	std::vector<unsigned short> m_Quests;

	int		        m_nZone;

	unsigned long	m_dwTownOrNameID;
	unsigned long	m_dwGID;        // Vincent : GID 를 Creature 로 옮길까?
    unsigned long   m_dwFlags;
	unsigned long	m_dwDisplayTime;

    unsigned char	m_cGradeRate[Item::EquipType::MAX_OPTION_TYPE][Item::EquipType::MAX_GRADE];
	unsigned char	m_cBaseNum[Item::EquipType::MAX_OPTION_TYPE][Item::EquipType::MAX_GRADE];
    unsigned char	m_cNation;

};


#endif