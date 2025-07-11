#ifndef _VIRTUAL_AREA_MONSTER_MANAGER_H_
#define _VIRTUAL_AREA_MONSTER_MANAGER_H_

#pragma once
#pragma warning(disable:4800)

#include <map>
#include <functional>
#include <boost/pool/pool.hpp>
#include <boost/pool/pool_alloc.hpp>


// 전방 참조
class CCreature;
class CAggresiveCreature;
class CCharacter;
class CMonster;
struct Position;


// Singleton 아님!!
class CVirtualMonsterMgr
{
public:
	CVirtualMonsterMgr();
	~CVirtualMonsterMgr();

	enum eAdminMonster
	{
		NO_BLANK_UID	= 0x8FFF,
		INIT_UID		= 0x8FFE
	};

	bool		AddMonster(CMonster* lpMonster);

	CCreature*	GetCreature(unsigned long dwCID);
	CAggresiveCreature*	GetAggresiveCreature(unsigned long dwCID);
	CMonster*	GetMonster(unsigned long dwCID);

	bool		IsSummonee(unsigned long dwCID);
	bool		SummonMonster(int nKID, Position Pos, CCharacter* lpMaster);
	bool		AdminSummonMonster(int nKID, Position Pos, unsigned short wMapIndex);

	void		ProcessAllMonster();
	void		ProcessMonsterRegenHPAndMP();
	void		ProcessSummonMonsterDead(void);

	unsigned short	GetAvailableMonsterUID(unsigned short wKindID);
	unsigned short	GetMonsterNum(void)			{ return static_cast<unsigned short>(m_MonsterMap.size()); }

	// Type 정의
	typedef std::map<unsigned long, CMonster*, std::less<unsigned long>,
		boost::fast_pool_allocator<std::pair<unsigned long, CMonster*> > >		MonsterMap;

	typedef std::map<unsigned short, unsigned short>							AdminMonsterUIDMap;	// < KindID, NextUID(상위16비트) >

	MonsterMap&		GetMonsterMap()				{ return m_MonsterMap;	}

private:
	void	DestroyMonsterList();

	MonsterMap			m_MonsterMap;
	AdminMonsterUIDMap	m_AdminMonsterUIDMap;

	// 룬 오프 소환 몬스터 카운터 수
	unsigned short	m_usSummonCount;
};


#endif	// _VIRTUAL_AREA_MONSTER_MANAGER_H_