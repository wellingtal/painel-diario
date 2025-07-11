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

	// Process �Լ�
	virtual Siege::eReturn	Process()		{ return Siege::RET_OK;		}

	// Cmd ó�� ���� �Լ�
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

	// Get / Set �Լ�
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
	unsigned long	GetGID() const				{ return m_dwGID;			}	// ����!! ��� �� ��������� ����
	unsigned long	GetNation() const			{ return m_cNation;			}	// ����!! ���� �� ���� ���� ������Ʈ�鸸 ����
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
        
	unsigned long	GetCastleID() const			{ return m_dwCampOrCastleID;	}	// �� ��¡��, ����/�޹�, ���� ����
	unsigned long	GetCampID()	const			{ return m_dwCampOrCastleID;	}	// ��� ���
	unsigned long	GetOwnerID() const			{ return m_dwOwnerID;			}	// �ٰŸ�/���Ÿ� ��������, �ٰŸ�/���Ÿ� ���� ����, �����

	void			SetGID(unsigned long dwGID);								// ����!! ��� �� ��������� ����
	void			SetNation(unsigned char cNation);							// ����!! ���� �� �������� ������Ʈ�鸸 ����
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

	unsigned long	m_dwCID;				// CID		: SIEGE_OBJECT_BIT �� ������ CreatureID
	unsigned long	m_dwCampOrCastleID;		// CampID	: ������ ��� ��� �� ���� ����
											// CastleID : �� ��¡��, ����, �޹�, ����, �ٰŸ�/���Ÿ� ���������� ��ID
	unsigned long	m_dwOwnerID;			// CID      : �ٰŸ�/���Ÿ� ��������, ������� ����, �ٰŸ�/���Ÿ� ��������
	
	unsigned long	m_dwGID;				// GID		: ��� ��� ���� ����� ���ID (��� �� ��������� �����)
	unsigned char	m_cNation;				// �� ��¡��, ����, �޹�, ����, �ٰŸ�/���Ÿ� ���������� ���� (������ ������Ʈ�鸸 ���)
	unsigned long	m_dwHP;					// HP
	
	unsigned short	m_wObjectType;			// ���� ���� ������Ʈ Ÿ��
	unsigned char	m_cState;				// ���� ������Ʈ�� ����
	unsigned char	m_cSubState;			// ���� ������Ʈ�� ���� ����		(Use EMBLEM & GATE)
	unsigned char	m_cUpgradeStep;			// ���� ������Ʈ�� ���׷��̵� �ܰ�
	unsigned char	m_cUpgradeType;			// �� ��¡���� ���׷��̵� Ÿ��		(Only EMBLEM)

	unsigned char	m_cDevelopTime;			// ����(����)�� �ɸ��� �ð� (��)
	unsigned char	m_cUpgradeTime;			// ���׷��̵忡 �ɸ��� �ð� (��)
	unsigned char	m_cRepairTime;			// ������ �ɸ��� �ð� (��)
	unsigned char	m_cChangeTime;			// Ÿ�� ������ �ɸ��� �ð� (��)
	
	unsigned char	m_cZone;				// ��								(Only CAMP)
	unsigned char	m_cChannel;				// ä��								(Only CAMP)
	TIME			m_TimeValue;			// ���� �ð� ���� ����
	TIME			m_LastUseTime;			// ������ ������ ��� �ð�			(Use CAMP & SIEGE_ARMS & CASTLE_ARMS)
	Position		m_Pos;					// ��ġ
	unsigned char	m_cMaterial;			// ������ ���� ����
	unsigned char	m_cSiegeCount;			// ���� ���� Ƚ��

	unsigned long	m_dwRepairHP;			// ���� �Ϸ�� ȸ���� HP ��
	float			m_fDefaultDir;			// ���� ������ �⺻ ����

	CDBComponent&   m_DBComponent;          // DB

	friend	class	CSiegeObjectDBMgr;
};

#endif	// _RYL_DBAGENT_SIEGE_OBJECT_DB_H_
