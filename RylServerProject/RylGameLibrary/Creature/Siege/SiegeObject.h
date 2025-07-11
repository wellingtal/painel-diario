#ifndef _SIEGE_OBJECT_H_
#define _SIEGE_OBJECT_H_

#pragma once

#include <list>
#include <map>
#include <Creature/Monster/PatternMonster.h>
#include <Creature/Siege/SiegeConstants.h>
#include <Creature/Character/SphereTree/CharSphereTree.h>
#include <Creature/Siege/SerializeSiegeObjectData.h>
#include <Network/Packet/PacketStruct/CastlePacket.h>

using namespace Siege;

// ���� ����
class CCharacter;

namespace BroadCastSiege
{
	class CSerializeSiegeObjectData;
}

class CSiegeObject : public CSkillMonster, public ICharSphereTreeCallBack
{
public:

	typedef std::set<unsigned long>		BroadCastSet;

	virtual ~CSiegeObject();

	// ---------------------------------------------------------------------
	// CSkillMonster �κ��� ��ӹ��� �Լ�

			// ���Ʊ� �ĺ�
	virtual	EnemyCheck::EnemyType	IsEnemy(CCreature* lpTarget, unsigned char* cResult = NULL);

			// ��ü ���� �ɷ� ���� �Լ�
	virtual void	NormalBehavior(unsigned long dwTick)		{				}
	virtual void	AttackBehavior(unsigned long dwTick)		{				}
	virtual void	SearchPlayer(void)							{				}
			bool	SkillAttack(void);

	virtual bool	Dead(CAggresiveCreature* pOffencer)			{ return false;	}
	virtual void	MoveTo(const Position& NewPosition);
	// ---------------------------------------------------------------------


	// ---------------------------------------------------------------------
	// ĳ���Ͱ� �����ؼ� ������ ��� Attack Packet ó�� �Լ�

	virtual bool	AttackCID(CCharacter* lpRideChar, AtType attackType, AtNode& attackNode, unsigned short& wError)							{ return false;	}
	virtual bool	Attack(AtType attackType, unsigned char cDefenderNum, CAggresiveCreature** ppDefenders,
							unsigned char* cDefenserJudges, unsigned short* wDefenserMPHeal)													{ return false;	}
	virtual bool	MissileAttack(AtType attackType, float fDir, float nRange, float fAngle, char cTargetType)									{ return false;	}

	// ---------------------------------------------------------------------
	
	
	// ---------------------------------------------------------------------
	// SiegeObject ó�� �Լ�

	virtual bool	Build(unsigned char cUpgradeStep = 0)		{ return false;	}		// ���� �Ϸ�� ó��
	virtual bool	Cancel()									{ return false;	}		// ���� ��� �Ϸ�
	virtual bool	Upgrade(unsigned char cUpgradeStep)			{ return false;	}		// ���׷��̵� �Ϸ�� ó��
    virtual bool	Repair(unsigned short wRepairHP)			{ return false;	}		// ���� �Ϸ�� ó��
	virtual bool	Restore()									{ return false;	}		// ���� �Ϸ�� ó��
	virtual bool	Destroy(unsigned char cOffencerNation, bool bTakeGold)	{ return false;	}		// �ı� �Ϸ�� ó�� (��ü �ı�, �ı���)
	virtual bool	Destroy(unsigned long dwOffencerGID)		{ return false;	}		// ��� ��� �ı� �Ϸ�� ó�� (��ü �ı�, �ı���)
	virtual bool	Update(unsigned char cState, unsigned long dwValue1, unsigned long dwValue2, unsigned long dwOwnerID, unsigned char cSubCmd)	{ return false;	}	// ���� ������Ʈ
	virtual bool	ChangeType(unsigned short wChangeType)		{ return false;	}		// ���� �Ϸ�� ó��
	
	// ��Ÿ��Ŷ ���������� ��ȯ (bFullMaterial �� true �̸� ������ 90%�� �����ش�.)
	virtual bool	ToStarterKit(bool bFullMaterial=false)		{ return false;	}

	virtual unsigned long	GetRepairGold() const				{ return Siege::MAX_REPAIR_GOLD; }
			unsigned long	GetRepairHP();

	// Rider ���� ����
	virtual unsigned char	IsRider(unsigned long dwCID) const;								// �ش� ĳ���Ͱ� ���⿡ ž���� �ִ°�?
	virtual bool			Ride(unsigned long dwCID)			{ return false;	}			// ���� ž��
	virtual bool			GetOff(unsigned long dwCID)			{ return false;	}			// ���� ����
	virtual void			AllGetOff()							{ GetOff(m_dwRiderCID);	}	// ���⿡�� ��� ����

	// �� ������Ʈ ���õ� �Լ� (��¡�� ���׷��̵忡 ���� ȿ��)
			void	UpgradeByEmblem(unsigned char cUpgradeType, unsigned char cUpgradeStep);
			void	DegradeByEmblem(unsigned char cUpgradeType, unsigned char cUpgradeStep);
			unsigned short GetDefaultNowHP();			// ��¡���� ���׷��̵� ȿ���� ���� ���� HP ���

	// ---------------------------------------------------------------------

	// �ݰ� 5���� ��Ŷ�� �����ϴ� �Լ�
			void	SendHPUpdateToDBAgent(bool bForce=false);

// ���� ��ε�ĳ��Ʈ �Լ�
//			void	SendToRadiusCell(const char* szPacket, unsigned long dwPacketSize, unsigned char cCMD_In);
//			bool	SerializeOut(char* szBuffer_In, unsigned short& wDataLen);

			void	RangeTest();
	virtual void	RangeTestCallBack( const Position& centerPos, float fDistance, CCharSphereNode* pNode );
			void	SendToRange( float fRadius, const char* szPacket, unsigned long dwPacketSize, unsigned char cCMD_In );

	BroadCastSiege::CSerializeSiegeObjectData&  GetSerializeData(void)  { return m_SerializeSiegeObjectData;	}



	// Get / Set �Լ�
			bool	IsEmblem() const		{ return (Siege::EMBLEM == m_wObjectType);	}
			bool	IsCamp() const;
			bool	IsGate() const			{ return (Siege::GATE == m_wObjectType);	}
			bool	IsGuard() const			{ return (Siege::GUARD == m_wObjectType);	}
			bool	IsCastleObject() const;
			bool	IsCastleArms() const;
			bool	IsSiegeArms() const;
			bool	IsRidable() const;		// Ż�� �ִ� ������Ʈ�ΰ�?
			bool	IsCastleNPC() const		{ return (m_wObjectType == Siege::BACKDOOR || m_wObjectType == Siege::CASTLE_ARMS_NPC); }
			bool	IsMaxHP() const			{ return (m_CreatureStatus.m_nNowHP == m_CreatureStatus.m_StatusInfo.m_nMaxHP);	}
			bool	IsPeaceMode();
			bool	IsRideArms(void) const { return false; }
			bool	IsWorldWeapon() const;

			unsigned short	GetObjectType() const	{ return m_wObjectType;					}

			unsigned long	GetCID() const			{ return m_dwCID;						}
			unsigned long	GetNowHP()	const		{ return m_CreatureStatus.m_nNowHP;		}
			unsigned long	GetMaxHP() const		{ return m_CreatureStatus.m_StatusInfo.m_nMaxHP;	}
			unsigned char	GetState() const		{ return m_cState;						}
			unsigned char	GetSubState() const		{ return m_cSubState;					}
			unsigned char	GetUpgradeStep() const	{ return m_cUpgradeStep;				}
			unsigned char	GetUpgradeType() const	{ return m_cUpgradeType;				}
			const Position&	GetPosition() const		{ return m_CurrentPos;					}
			unsigned char	GetMaterialNum() const	{ return m_cMaterial;					}
			unsigned char	GetSiegeCount() const	{ return m_cSiegeCount;					}
			float			GetDefaultDir()	const	{ return m_fDefaultDir;					}
			unsigned long	GetRiderCID() const		{ return m_dwRiderCID;					}
	virtual	void			GetRiders( unsigned long* pRiders ) const;
	virtual unsigned char	GetRiderNum() const;

			unsigned long	GetDevelopGold(unsigned short wDefaultObjectType=Siege::DEFAULT_TYPE) const;
			unsigned char	GetDevelopSpeed(unsigned short wDefaultObjectType=Siege::DEFAULT_TYPE) const;
			unsigned long	GetUpgradeGold() const;
			unsigned char	GetUpgradeSpeed(unsigned char cDefaultUpgradeType=Siege::NO_JEWEL) const;
			float			GetBonusRate() const;
			
			int				GetKID() const;
			unsigned char	GetNation() const;
			unsigned long	GetGID() const;
			CAggresiveCreature*	GetDuelOpponent(void) const;

			char			GetConsumeMPCount(void)	{ return m_cConsumeMPCount;				}

			unsigned long	GetCastleID() const		{ return m_dwCampOrCastleID;	}
			unsigned long	GetCampID()	const		{ return m_dwCampOrCastleID;	}
			unsigned long	GetOwnerID() const		{ return m_dwOwnerID;			}

			void			SetGID(unsigned long dwGID)					{ m_dwGID = dwGID; }
			void			SetFullMP()									{ m_CreatureStatus.m_nNowMP = m_CreatureStatus.m_StatusInfo.m_nMaxMP; }
			void			SetState(unsigned char cState)				{ m_cState = cState; }
			void			SetSubState(unsigned char cSubState)		{ m_cSubState = cSubState; }
			void			SetMaterialNum(unsigned char cMaterial);
			void			SetSiegeCount(unsigned char cSiegeCount)	{ m_cSiegeCount = cSiegeCount;	}
			void			SetNation(unsigned char cNation);
				
protected:
	
	CSiegeObject(MonsterCreateInfo& MonsterCreate, const CastleObjectInfo& CastleObject);
	CSiegeObject(MonsterCreateInfo& MonsterCreate, unsigned long dwCampID, unsigned long dwGID, 
		unsigned long dwHP, unsigned short wObjectType, unsigned char cState, unsigned char cSubState,
		unsigned char cUpgradeStep, unsigned char cMaterial, unsigned char cSiegeCount, bool bFullHP);
	CSiegeObject(MonsterCreateInfo& MonsterCreate, unsigned long dwOwnerID, unsigned char cNation,
		unsigned long dwHP, unsigned short wObjectType, unsigned char cState, unsigned char cUpgradeStep);

			void	UpdateObjectInfo(unsigned char cHPType=Siege::NOW_HP, unsigned short wRepairHP=0);	// ����, ���׷��̵� ��ȭ�ÿ� ���ΰ��� ����

	unsigned long	m_dwCampOrCastleID;		// CastleID : �� ��¡��, ����, �޹�, ����, �ٰŸ�/���Ÿ� ��������
											// CampID   : ��� ��� ��¡��, ���
	unsigned long	m_dwOwnerID;			// CID      : �ٰŸ�/���Ÿ� ��������, �ٰŸ�/���Ÿ� ��������, �����

	unsigned long	m_dwGID;				// GID		: ��� ��� �� ���� ������ ���� ����� ���ID
											// ����!! ������ ���� ������Ʈ�� GID �� ������� �ʴ´�!!
	unsigned char	m_cNation;				// ������ ���� ������Ʈ�� ����ϴ� ����

	unsigned short	m_wObjectType;			// ���� ���� ������Ʈ Ÿ��
	unsigned char	m_cState;				// ���� ������Ʈ�� ����
	unsigned char	m_cSubState;			// ���� ������Ʈ�� ���� ����		(Use EMBLEM & GATE)
	unsigned char	m_cUpgradeStep;			// ���� ������Ʈ�� ���׷��̵� �ܰ�
	unsigned char	m_cUpgradeType;			// �� ��¡���� ���׷��̵� Ÿ��		(Only EMBLEM)
	unsigned char	m_cMaterial;			// ���� ���� ����
	unsigned char	m_cSiegeCount;			// ���� ���� Ƚ��
	float			m_fDefaultDir;			// ���� ������ �⺻ ����
	unsigned long	m_dwRiderCID;			// ���� ����, ���� ���⿡ ž���� ĳ������ CID
	
	unsigned char	m_cHPUpdateCount;		// HP ���� ������Ʈ ī��Ʈ

	// Rider ������ �ʿ��� ����
	// 1. ����̳� Ÿ�� �ִ��� �˼��� ����.
	// 2. �������� ������ ��� ���� ���ȴ��� �˼��� ����.
	//
	// * ������� Ÿ�� ������ ������üũ�� �ϸ� �ȵȴ�.

	// ----------------------------------------------------------------------------------------
	// New BroadCast Data
	BroadCastSiege::CSerializeSiegeObjectData   m_SerializeSiegeObjectData;     // ���� ������Ʈ ������ ���� �� ����� ������ ����

	char			m_szBroadCastBuffer[ BroadCastSiege::MAX_SIEGEBROADCAST_BUFFER_SIZE ];
	char			m_szDeltaBuffer[ BroadCastSiege::MAX_SIEGEBROADCAST_BUFFER_SIZE ];
	unsigned long	m_dwBroadCastSize;
	unsigned long	m_dwDeltaSize;

	BroadCastSet	m_BroadCastSet;
	// ----------------------------------------------------------------------------------------

	friend	class	CSiegeObjectMgr;
};

#endif	// _SIEGE_OBJECT_H_