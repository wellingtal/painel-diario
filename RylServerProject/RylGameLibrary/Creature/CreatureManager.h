#ifndef _CCREAUTRE_MANAGER_H_
#define _CCREAUTRE_MANAGER_H_

#pragma once
#pragma warning(disable:4800)

#include <boost/pool/pool.hpp>
#include <boost/pool/pool_alloc.hpp>

#include <map>
#include <list>
#include <functional>

#include <Thread/Lock.h>
#include <DB/DBdefine.h>

#include <Creature/Character/CharacterClass.h>

#include <Network/Stream/SendStream.h>

#include "CreatureStructure.h"


// ���� ����
class CCreature;
class CAggresiveCreature;
class CCharacter;
class CMonster;
class CNPC;
class CSiegeObject;

namespace Item
{
	class CShopContainer;
};


// SingleThread Ŭ����
class CCreatureManager
{
private:

	// Map���� second ����� �Լ��� ����
	template<typename FnSecondProcess, typename PairType>
	class CProcessSecond : public std::unary_function<FnSecondProcess, bool> 
    {
	public:
		CProcessSecond(FnSecondProcess& fnSecondProcess) : m_fnSecondProcess(fnSecondProcess) { }
        bool operator() (PairType& Pair) { m_fnSecondProcess(Pair.second); return true; }
	protected:
		FnSecondProcess& m_fnSecondProcess;
	};

public:

    typedef std::map<unsigned long, CCharacter*, std::less<unsigned long>,
        boost::fast_pool_allocator<std::pair<unsigned long, CCharacter*> > >	CharacterMap;

    typedef std::map<unsigned long, CMonster*, std::less<unsigned long>,
        boost::fast_pool_allocator<std::pair<unsigned long, CMonster*> > >		MonsterMap;

    typedef std::map<unsigned short, unsigned short, std::less<unsigned short>,
        boost::fast_pool_allocator<std::pair<unsigned short, unsigned short> > >    AdminMonsterUIDMap;	// < KindID, NextUID(����16��Ʈ) >

    typedef std::map<unsigned long, CNPC*, std::less<unsigned long>,
        boost::fast_pool_allocator<std::pair<unsigned long, CNPC*> > >			NPCMap;

    // �̰� boost::pool���� �� ��.
    typedef std::list<CCharacter*>	CharacterList;

    typedef std::multimap<unsigned long, CCharacter*, std::less<unsigned long>,
        boost::fast_pool_allocator<std::pair<unsigned long, CCharacter*> > >	CharacterMultimap;

    typedef std::map<unsigned long, CSiegeObject*, std::less<unsigned long>,
        boost::fast_pool_allocator<std::pair<unsigned long, CSiegeObject*> > >	SiegeObjectMap;

	static CCreatureManager& GetInstance();

    enum eAdminMonster
	{
		NO_BLANK_UID	= 0x8FFF,
		INIT_UID		= 0x8FFE
	};

	// --------------------------------------------------------------------------------------------
	// interface
	
    // ĳ���� ���� �� ������ ���� Wrapper.
    CCharacter* CreateCharacter(unsigned long dwCID, unsigned long dwSessionID);
    void DeleteCharacter(CCharacter* lpCharacter);

    // Creature�� �Ŵ����� ��� �� ����.
	bool AddCreature(CCreature* lpCreature);
	bool DeleteCreature(unsigned long dwCID);

    CCreature*	GetCreature(unsigned long dwCID);
    CAggresiveCreature* GetAggresiveCreature(unsigned long dwCID);

	CNPC*			GetNPC(unsigned long dwCID);
	CMonster*		GetMonster(unsigned long dwCID);
	CSiegeObject*	GetSiegeObject(unsigned long dwCID);
	CCharacter*		GetCharacter(unsigned long dwCID);	
	CCharacter*		GetCharacter(const char* szCharacterName);

	void DestoryCharacterList();
	void DestoryMonsterList();
	void DestorySiegeObjectList();
	void DestoryNPCList();

    void DestroyAll();

    void EnqueueLogout(CCharacter* lpCharacter);
	bool CancelLogout(CCharacter* lpCharacter);
	bool ProcessCharacterLogout(void);
	void ProcessSummonMonsterDead(void);
	void SetNationToCastleNPC(unsigned long dwCastleID, unsigned char cNation);

	template<typename FnCharacterProcess>
        inline void ProcessAllCharacter(FnCharacterProcess fnCharacterProcess) 
    {
		std::for_each(m_CharacterMap.begin(), m_CharacterMap.end(), 
            CProcessSecond<FnCharacterProcess, CharacterMap::value_type>(fnCharacterProcess));
	}

	template<typename FnMonsterProcess>
        inline void ProcessAllMonster(FnMonsterProcess fnMonsterProcess) 
    {
		std::for_each(m_MonsterMap.begin(), m_MonsterMap.end(), 
			CProcessSecond<FnMonsterProcess, MonsterMap::value_type>(fnMonsterProcess));
	}
	
    template<typename FnNPCProcess>
        inline void ProcessAllNPC(FnNPCProcess fnNPCProcess) 
    {
		std::for_each(m_NPCMap.begin(), m_NPCMap.end(), 
			CProcessSecond<FnNPCProcess, NPCMap::value_type>(fnNPCProcess));
	}

	unsigned short	GetCharacterNum(unsigned char cNation)	{ return m_wCharacterNum[cNation]; }
	unsigned short	GetCharacterNum(void)		{ return static_cast<unsigned short>(m_CharacterMap.size());	}
	unsigned short	GetMonsterNum(void)			{ return static_cast<unsigned short>(m_MonsterMap.size());		}
	unsigned short	GetSiegeObjectNum(void)		{ return static_cast<unsigned short>(m_SiegeObjectMap.size());	}

	unsigned short	GetAvailableMonsterUID(unsigned short wKindID);			// ���͸� �����Ҷ� CID�� ��ġ�� �ʰ� �ϱ� ���ؼ� ȣ���ϴ� �Լ�

	bool IsSummonee(unsigned long dwCID);
	Item::CShopContainer* GetShopContainer(unsigned long dwCID);

	void SendAllCharacter(const char* szBuffer, unsigned long dwLength, 
        unsigned char cCMD_In, bool bSendAllMap = true);	

	void SendFindPartyMsg(unsigned long dwSenderCID, const char* szCompressedPacket, unsigned long dwPacketSize);

    // bAll �� true �̸� ���,
    // false �̸� MapIndex �� 0 �� Character �鸸...
    bool ChangeCharacterName(unsigned long dwCID, const char* szChangedName);

	// ������� ������ ���� �ð��ÿ� ���̵� ó�� �Լ�
	void MoveCharacterToGuildWarZone(unsigned char cType, bool bOn = true);
	void MoveCharacterToRealmWarZone(unsigned char cType, bool bOn = true);

	// ������� ������ ������ ������ �÷��� ����
	void ClearGuildWarInstanceFlag();
	void ClearRealmWarInstanceFlag();

// --------------------------------------------------------------------------------------------
// ���� ���� ���� ����

public:

	// ������ ���� �ʱⰪ ���� �� ���� �ʱ�ȭ �Լ�
	void SetRealmStatueInfo(unsigned long dwIndex, unsigned short wKID, unsigned long dwLostHP);
	void InitRealmStatue();
	void InitRealmLoadingStatue();
	bool SendRealmStatueDisplayInfo(CSendStream& SendStream);
	bool SendRealmStatueDisplayInfo(unsigned long dwIndex = 0, bool bAttacked = false);

	void CalculateRealmStatueEnchantInfo();
	void AddRealmStatueEnchant(CCharacter* lpCharacter);
	void AddRealmStatueEnchant();
	void ClearRealmStatueEnchant();

private:

	int	GetRealmStatueNum();

	struct sRealmEnchant
	{
		unsigned char	m_cHPPercent;
		unsigned char	m_cMPPercent;
		unsigned char	m_cExpPercent;
		unsigned char	m_cDropRate;
	};

	sRealmEnchant	m_RealmEnchantInfo[CClass::MAX_RACE];
// --------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------
// ������ ���� ����
private:

	void AddCastleBonus();
// --------------------------------------------------------------------------------------------

private:

    CCreatureManager();
	~CCreatureManager();

    NPCMap				m_NPCMap;
	MonsterMap			m_MonsterMap;
	AdminMonsterUIDMap	m_AdminMonsterUIDMap;
	CharacterMap		m_CharacterMap;	
	CharacterMultimap	m_CharacterNameMap;

	// CSiegeObjectMgr �� ��κ��� ������ �ϰ�,
	// CCreatureMgr �� ����Ʈ�� ������ ����
	SiegeObjectMap		m_SiegeObjectMap;

	CharacterList	    m_LogoutWaitList;

	boost::pool<>		m_CharacterPool;

// --------------------------------------------------------------------------------------------
// ����Ʈ ���ʽ� ����

public:

	enum FightResult
	{
		WIN		= 0, 
		LOSE	= 1
	};

	void CalculateEliteBonus(unsigned short *usPeopleNum);
	
	void SetEliteBonus(EliteBonus::EliteBonusData eliteBonus);
	const EliteBonus::EliteBonusData& GetEliteBonus(void)	{ return m_EliteBonus;	}
	
	void SetAutoBalance(bool bAutoBalance)	{ m_bAutoBalance = bAutoBalance;		}
	bool GetAutoBalance(void)				{ return m_bAutoBalance;				}

	float GetFameBonus(CClass::RaceType eRace, FightResult eResult);


private: 

	EliteBonus::EliteBonusData	m_EliteBonus;
	bool						m_bAutoBalance;


// --------------------------------------------------------------------------------------------
// ���� ���� ����
public:

	void AddWorldWeaponEnchant(CAggresiveCreature* lpWeapon, unsigned char cNation);
	void ClearWorldWeaponEnchant();
	void ProcessWorldWeaponDamage(int iX, int iZ, unsigned char cUpgradeStep);

// --------------------------------------------------------------------------------------------
// ��Ʋ �׶��� ����

public:

	void PushRespawnQueue(CCharacter* lpCharacter, unsigned char cPointNumber);
	void PopRespawnQueue(CCharacter* lpCharacter);
	void ProcessRespawnQueue(void);
	bool SendRespawnQueue(unsigned long dwCID);

	unsigned char GetBonusTurn(const unsigned short wMapIndex);

	void ProcessBattleGround();
	
	enum StatueConst
	{
		STATUE_REST_TIME_1ST_START	= 25,		// ���� �ð����� 25�а� 55�к��� 5�а� ������ ���ְ�,
		STATUE_REST_TIME_1ST_END	= 29,		// ��� ĳ���͵��� �ʷ������� ������ ��Ų��.
		STATUE_REST_TIME_2ND_START	= 55,
		STATUE_REST_TIME_2ND_END	= 59,

		STATUE_RESPAWN_TIME_1ST	= 0,			// ���� �ð����� 0�а� 30�п��� ������ �ʱ�ȭ ���·� ��ȯ�Ǿ�� �Ѵ�.
		STATUE_RESPAWN_TIME_2ND	= 30,
	};

private:

	bool m_bRest;								// ������ �ϳ��� ���� �޽Ľð��ΰ�?


private:

	enum RespawnPoint
	{
		POINT_HUMAN1	= 0,
		POINT_HUMAN2	= 1,
		POINT_HUMAN3	= 2,
		POINT_AKHAN1	= 3,
		POINT_AKHAN2	= 4,
		POINT_AKHAN3	= 5,
		MAX_POINT_NUM	= 6
	};

	struct BattleGroundRespawnInfo
	{
		CCharacter*		m_lpCharacter;
		Position		m_RespawnPos;
		long			m_lLeftTime;

		BattleGroundRespawnInfo()
			:	m_lpCharacter(NULL), m_lLeftTime(0)
		{ }

		BattleGroundRespawnInfo(CCharacter* lpCharacter, Position Pos, long lTime)
			:	m_lpCharacter(lpCharacter), m_RespawnPos(Pos), m_lLeftTime(lTime)
		{ }
	};

	typedef std::list<BattleGroundRespawnInfo>		RespawnQueue;

	RespawnQueue		m_lstRespawn[MAX_POINT_NUM];
	Position			m_RespawnPoint[MAX_POINT_NUM];

	unsigned long		m_dwLastUpdateTime;

	unsigned short		m_wCharacterNum[CClass::MAX_RACE];
};


#endif