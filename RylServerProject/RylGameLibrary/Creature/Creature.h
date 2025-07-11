#ifndef _CCREATURE_H_
#define _CCREATURE_H_

#include "stdafx.h"

#include <DB/DBdefine.h>
#include <Network/Packet/PacketStruct/CharItemPacketStruct.h>

#include "EnemyCheck.h"
#include "CreatureStructure.h"


class CCell;
class CCharacter;
class CParty;
namespace Item
{
	class CItem;
};

class CCreature
{

public:

	enum MoveInfo 
    {
		S_CELL_CHANGED		= 1,
		S_CELL_UNCHANGED	= 2,
		F_CELL_CANNOTCHANGE	= 3,
		F_TOO_FAST			= 4
	};

	CCreature(unsigned long dwCID) : m_dwCID(dwCID)	{ }
	virtual ~CCreature() { }

	const Position& GetCurrentPos(void)	const   		{ return m_CurrentPos;		}
	const Position& GetCastingPos(void) const			{ return m_CastingPos;		}
	void SetCurrentPos(const Position& pos)				{ m_CurrentPos = pos;		}
	void SetCurrentPos(float fX, float fY, float fZ)	{ m_CurrentPos.m_fPointX = fX; m_CurrentPos.m_fPointY = fY; m_CurrentPos.m_fPointZ = fZ; }
	void SetCastingPos()								{ m_CastingPos = m_CurrentPos; }
	void SetCastingPos(const Position& pos)				{ m_CastingPos = pos;		}
	void SetCastingPos(float fX, float fY, float fZ)	{ m_CastingPos.m_fPointX = fX; m_CastingPos.m_fPointY = fY; m_CastingPos.m_fPointZ = fZ; }

	const MotionInfo& GetMotionInfo(void) const { return m_MotionInfo;				}
	void SetDirection(float fDir)				{ m_MotionInfo.m_fDirection = fDir;	}

	EnchantInfo& GetEnchantInfo(void)			{ return m_EnchantInfo;				}

	unsigned long GetCID(void) const			{ return m_dwCID;					}
	void SetCIDMe(unsigned long InCID)			{ m_dwCID = InCID;					}

	virtual unsigned long RepairItem(Item::CEquipment* pEquipment, unsigned long& dwCurrentGold)	{ return 0;	}

	virtual Item::CItem* SellToCharacter(CCharacter *lpCustomer, unsigned short wKindItem, TakeType takeType, 
		Item::CItem* lpRequestItem, unsigned long &dwPrice, unsigned short wCouponID, unsigned short &usError) = 0;

	virtual EnemyCheck::EnemyType IsEnemy(CCreature* lpTarget, unsigned char* cResult = NULL) = 0;
	virtual unsigned char GetNation() const = 0;
	virtual CParty* GetParty() = 0;
	virtual bool IsPeaceMode() = 0;
	virtual unsigned long GetGID() const = 0;
	virtual bool IsRideArms() const = 0;


protected:

	Position		m_CurrentPos;	// 현재 위치
	Position		m_CastingPos;	// 스킬 캐스팅 위치
	MotionInfo		m_MotionInfo;	// 모션 정보
	EnchantInfo		m_EnchantInfo;	// 챈트/인챈트 정보

	unsigned long	m_dwCID;					// 캐릭터 CID.
};

#endif