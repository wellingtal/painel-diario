#ifndef _RYL_DBAGENT_SIEGE_OBJECT_DB_H_
#define _RYL_DBAGENT_SIEGE_OBJECT_DB_H_

#pragma once

#include <Network/Packet/PacketStruct/CastlePacket.h>
#include <Creature/CreatureStructure.h>

using namespace Siege;

// forward decl.
class CDBComponent;

class CSiegeObjectDB
{
public:

	virtual ~CSiegeObjectDB();

	// Process 함수
	virtual Siege::eReturn	Process()		{ return Siege::RET_OK;		}

	// Cmd 처리 가상 함수
	virtual bool	CancelBuild()																											{ return false;	}
	virtual bool	Cancel()																												{ return false;	}
	virtual bool	Build(unsigned short wType, unsigned char cDevelopTime, unsigned long dwDevelopGold = 0, unsigned long dwOwnerID = 0)	{ return false;	}
	virtual bool	Upgrade(unsigned char cUpgradeTime, unsigned long dwUpgradeGold = 0, unsigned char cUpgradeType = Siege::NO_JEWEL)		{ return false;	}
	virtual bool	Repair(unsigned long dwRepairHP, unsigned long dwRepairGold = 0)														{ return false;	}
	virtual bool	Restore(unsigned char cRestoreTime, unsigned long dwRestoreGold = 0)													{ return false;	}
	virtual bool	Destroy(unsigned long dwEnemyNation = 0, bool bDeductFame = false)														{ return false;	}
	virtual bool	ChangeType(unsigned short wType)																						{ return false;	}

	virtual void	UpdateRewardFame()					{						}

	virtual void	UpdateLastUseTime(bool bInit=false);
	virtual void	UpdateHP(unsigned long dwNowHP)		{ m_dwHP = dwNowHP;		}
	virtual	bool	ToStartKit()						{ return false;			}

	// To GameServer 
	void	SendCampCmd(unsigned char cSubCmd, unsigned long dwValue1=0, unsigned long dwValue2=0);
	void	SendCastleCmd(unsigned long dwValue, unsigned char cSubCmd);
	void	SendSiegeArmsCmd(unsigned long dwValue, unsigned char cSubCmd);

	// Get / Set 함수
	bool IsEmblem() const		{ return (Siege::EMBLEM == m_wObjectType);	}
	bool IsCamp() const;
	bool IsGate() const			{ return (Siege::GATE == m_wObjectType);	}
	bool IsGuard() const		{ return (Siege::GUARD == m_wObjectType);	}
	bool IsCastleObject() const;
	bool IsCastleArms() const;
	bool IsSiegeArms() const;
	bool IsRidable() const;
	bool IsWorldWeapon() const;

	unsigned long	GetCID() const				{ return m_dwCID;			}
	unsigned long	GetGID() const				{ return m_dwGID;			}	// 주의!! 요새 및 월드웨폰만 가능
	unsigned long	GetNation() const			{ return m_cNation;			}	// 주의!! 공성 및 수성 관련 오브젝트들만 가능
	unsigned long	GetNowHP()	const			{ return m_dwHP;			}
	unsigned char	GetState() const			{ return m_cState;			}
	unsigned char	GetSubState() const			{ return m_cSubState;		}
	unsigned char	GetUpgradeStep() const		{ return m_cUpgradeStep;	}
	unsigned char	GetZone() const				{ return m_cZone;			}
	unsigned char	GetChannel() const			{ return m_cChannel;		}
	const Position&	GetPosition() const			{ return m_Pos;				}
	unsigned char	GetMaterialNum() const		{ return m_cMaterial;		}
	unsigned char	GetSiegeCount() const		{ return m_cSiegeCount;		}
	unsigned short	GetObjectType() const		{ return m_wObjectType;		}
	unsigned char	GetDevelopTime() const		{ return m_cDevelopTime;	}
	unsigned char	GetUpgradeTime() const		{ return m_cUpgradeTime;	}
	unsigned char	GetRepairTime() const		{ return m_cRepairTime;		}
	unsigned char	GetChangeTime() const		{ return m_cChangeTime;		}

	const TIME&		GetTimeValue() const		{ return m_TimeValue;		}
	const TIME&		GetLastUseTime() const		{ return m_LastUseTime;		}
        
	unsigned long	GetCastleID() const			{ return m_dwCampOrCastleID;	}	// 성 상징물, 성문/뒷문, 수성 병기
	unsigned long	GetCampID()	const			{ return m_dwCampOrCastleID;	}	// 길드 요새
	unsigned long	GetOwnerID() const			{ return m_dwOwnerID;			}	// 근거리/원거리 수성병기, 근거리/원거리 공성 병기, 비공정

	void			SetGID(unsigned long dwGID);								// 주의!! 요새 및 월드웨폰만 가능
	void			SetNation(unsigned char cNation);							// 주의!! 공성 및 수성관련 오브젝트들만 가능
	void			SetZoneChannel(unsigned char cZone, unsigned char cChannel);
	virtual void	SetMaterialNum(unsigned char cMaterial);
	void			SetSiegeCount(unsigned char cSiegeCount)	{ m_cSiegeCount = cSiegeCount;	}
	
protected:

	CSiegeObjectDB(CDBComponent& DBComponent, const CastleObjectInfoDB& CastleObject);
	CSiegeObjectDB(CDBComponent& DBComponent, const CampInfoDB& CampInfo);

	CSiegeObjectDB(CDBComponent& DBComponent, unsigned long dwCampID, 
        unsigned long dwGID, unsigned char cZone, unsigned char cChannel, Position Pos);

	CSiegeObjectDB(CDBComponent& DBComponent, unsigned long dwOwnerID, 
        unsigned char cNation, unsigned short wObjectType, unsigned char cUpgradeStep, 
        unsigned char cZone, unsigned char cChannel, Position Pos);

	void	SetCID(unsigned long dwCID)			{ m_dwCID = dwCID;			}

	unsigned long	m_dwCID;				// CID		: SIEGE_OBJECT_BIT 이 설정된 CreatureID
	unsigned long	m_dwCampOrCastleID;		// CampID	: 진지의 길드 요새 및 월드 웨폰
											// CastleID : 성 상징물, 성문, 뒷문, 가드, 근거리/원거리 수성병기의 성ID
	unsigned long	m_dwOwnerID;			// CID      : 근거리/원거리 공성병기, 비공정의 주인, 근거리/원거리 수성병기
	
	unsigned long	m_dwGID;				// GID		: 길드 요새 소유 길드의 길드ID (요새 및 월드웨폰만 사용함)
	unsigned char	m_cNation;				// 성 상징물, 성문, 뒷문, 가드, 근거리/원거리 수성병기의 국가 (공성전 오브젝트들만 사용)
	unsigned long	m_dwHP;					// HP
	
	unsigned short	m_wObjectType;			// 공성 관련 오브젝트 타입
	unsigned char	m_cState;				// 현재 오브젝트의 상태
	unsigned char	m_cSubState;			// 현재 오브젝트의 서브 상태		(Use EMBLEM & GATE)
	unsigned char	m_cUpgradeStep;			// 현재 오브젝트의 업그레이드 단계
	unsigned char	m_cUpgradeType;			// 성 상징물의 업그레이드 타입		(Only EMBLEM)

	unsigned char	m_cDevelopTime;			// 개발(복구)에 걸리는 시간 (분)
	unsigned char	m_cUpgradeTime;			// 업그레이드에 걸리는 시간 (분)
	unsigned char	m_cRepairTime;			// 수리에 걸리는 시간 (초)
	unsigned char	m_cChangeTime;			// 타입 변형에 걸리는 시간 (분)
	
	unsigned char	m_cZone;				// 존								(Only CAMP)
	unsigned char	m_cChannel;				// 채널								(Only CAMP)
	TIME			m_TimeValue;			// 각종 시간 저장 변수
	TIME			m_LastUseTime;			// 진지의 마지막 사용 시간			(Use CAMP & SIEGE_ARMS & CASTLE_ARMS)
	Position		m_Pos;					// 위치
	unsigned char	m_cMaterial;			// 공성용 자재 갯수
	unsigned char	m_cSiegeCount;			// 공성 지난 횟수

	unsigned long	m_dwRepairHP;			// 수리 완료시 회복할 HP 양
	float			m_fDefaultDir;			// 수성 병기의 기본 방향

	CDBComponent&   m_DBComponent;          // DB

	friend	class	CSiegeObjectDBMgr;
};

#endif	// _RYL_DBAGENT_SIEGE_OBJECT_DB_H_
