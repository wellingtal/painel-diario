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


// 전방 참조
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


// SingleThread 클래스
class CCreatureManager
{
private:

	// Map에서 second 멤버로 함수자 실행
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
        boost::fast_pool_allocator<std::pair<unsigned short, unsigned short> > >    AdminMonsterUIDMap;	// < KindID, NextUID(상위16비트) >

    typedef std::map<unsigned long, CNPC*, std::less<unsigned long>,
        boost::fast_pool_allocator<std::pair<unsigned long, CNPC*> > >			NPCMap;

    // 이건 boost::pool쓰지 말 것.
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
	
    // 캐릭터 생성 및 삭제를 위한 Wrapper.
    CCharacter* CreateCharacter(unsigned long dwCID, unsigned long dwSessionID);
    void DeleteCharacter(CCharacter* lpCharacter);

    // Creature를 매니저에 등록 및 삭제.
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

	unsigned short	GetAvailableMonsterUID(unsigned short wKindID);			// 몬스터를 생성할때 CID가 겹치지 않게 하기 위해서 호출하는 함수

	bool IsSummonee(unsigned long dwCID);
	Item::CShopContainer* GetShopContainer(unsigned long dwCID);

	void SendAllCharacter(const char* szBuffer, unsigned long dwLength, 
        unsigned char cCMD_In, bool bSendAllMap = true);	

	void SendFindPartyMsg(unsigned long dwSenderCID, const char* szCompressedPacket, unsigned long dwPacketSize);

    // bAll 이 true 이면 모두,
    // false 이면 MapIndex 가 0 인 Character 들만...
    bool ChangeCharacterName(unsigned long dwCID, const char* szChangedName);

	// 길드전과 국가전 전쟁 시간시에 존이동 처리 함수
	void MoveCharacterToGuildWarZone(unsigned char cType, bool bOn = true);
	void MoveCharacterToRealmWarZone(unsigned char cType, bool bOn = true);

	// 길드전과 국가전 전쟁이 끝난후 플래그 끄기
	void ClearGuildWarInstanceFlag();
	void ClearRealmWarInstanceFlag();

// --------------------------------------------------------------------------------------------
// 국가 전쟁 석상 관련

public:

	// 국가전 석상 초기값 셋팅 및 상태 초기화 함수
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
// 공성전 보상 관련
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

	// CSiegeObjectMgr 가 대부분의 관리를 하고,
	// CCreatureMgr 은 리스트만 가지고 있음
	SiegeObjectMap		m_SiegeObjectMap;

	CharacterList	    m_LogoutWaitList;

	boost::pool<>		m_CharacterPool;

// --------------------------------------------------------------------------------------------
// 엘리트 보너스 관련

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
// 월드 웨폰 관련
public:

	void AddWorldWeaponEnchant(CAggresiveCreature* lpWeapon, unsigned char cNation);
	void ClearWorldWeaponEnchant();
	void ProcessWorldWeaponDamage(int iX, int iZ, unsigned char cUpgradeStep);

// --------------------------------------------------------------------------------------------
// 배틀 그라운드 관련

public:

	void PushRespawnQueue(CCharacter* lpCharacter, unsigned char cPointNumber);
	void PopRespawnQueue(CCharacter* lpCharacter);
	void ProcessRespawnQueue(void);
	bool SendRespawnQueue(unsigned long dwCID);

	unsigned char GetBonusTurn(const unsigned short wMapIndex);

	void ProcessBattleGround();
	
	enum StatueConst
	{
		STATUE_REST_TIME_1ST_START	= 25,		// 서버 시간으로 25분과 55분부터 5분간 석상이 없애고,
		STATUE_REST_TIME_1ST_END	= 29,		// 모든 캐릭터들을 초렙존으로 리스폰 시킨다.
		STATUE_REST_TIME_2ND_START	= 55,
		STATUE_REST_TIME_2ND_END	= 59,

		STATUE_RESPAWN_TIME_1ST	= 0,			// 서버 시간으로 0분과 30분에는 석상이 초기화 상태로 소환되어야 한다.
		STATUE_RESPAWN_TIME_2ND	= 30,
	};

private:

	bool m_bRest;								// 석상이 하나도 없는 휴식시간인가?


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