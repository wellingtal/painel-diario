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

// 전방 참조
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
	// CSkillMonster 로부터 상속받은 함수

			// 적아군 식별
	virtual	EnemyCheck::EnemyType	IsEnemy(CCreature* lpTarget, unsigned char* cResult = NULL);

			// 자체 공격 능력 구현 함수
	virtual void	NormalBehavior(unsigned long dwTick)		{				}
	virtual void	AttackBehavior(unsigned long dwTick)		{				}
	virtual void	SearchPlayer(void)							{				}
			bool	SkillAttack(void);

	virtual bool	Dead(CAggresiveCreature* pOffencer)			{ return false;	}
	virtual void	MoveTo(const Position& NewPosition);
	// ---------------------------------------------------------------------


	// ---------------------------------------------------------------------
	// 캐릭터가 조종해서 공격할 경우 Attack Packet 처리 함수

	virtual bool	AttackCID(CCharacter* lpRideChar, AtType attackType, AtNode& attackNode, unsigned short& wError)							{ return false;	}
	virtual bool	Attack(AtType attackType, unsigned char cDefenderNum, CAggresiveCreature** ppDefenders,
							unsigned char* cDefenserJudges, unsigned short* wDefenserMPHeal)													{ return false;	}
	virtual bool	MissileAttack(AtType attackType, float fDir, float nRange, float fAngle, char cTargetType)									{ return false;	}

	// ---------------------------------------------------------------------
	
	
	// ---------------------------------------------------------------------
	// SiegeObject 처리 함수

	virtual bool	Build(unsigned char cUpgradeStep = 0)		{ return false;	}		// 구축 완료시 처리
	virtual bool	Cancel()									{ return false;	}		// 구축 취소 완료
	virtual bool	Upgrade(unsigned char cUpgradeStep)			{ return false;	}		// 업그레이드 완료시 처리
    virtual bool	Repair(unsigned short wRepairHP)			{ return false;	}		// 수리 완료시 처리
	virtual bool	Restore()									{ return false;	}		// 복구 완료시 처리
	virtual bool	Destroy(unsigned char cOffencerNation, bool bTakeGold)	{ return false;	}		// 파괴 완료시 처리 (자체 파괴, 파괴됨)
	virtual bool	Destroy(unsigned long dwOffencerGID)		{ return false;	}		// 길드 요새 파괴 완료시 처리 (자체 파괴, 파괴됨)
	virtual bool	Update(unsigned char cState, unsigned long dwValue1, unsigned long dwValue2, unsigned long dwOwnerID, unsigned char cSubCmd)	{ return false;	}	// 정보 업데이트
	virtual bool	ChangeType(unsigned short wChangeType)		{ return false;	}		// 변형 완료시 처리
	
	// 스타터킷 아이템으로 전환 (bFullMaterial 이 true 이면 자재의 90%를 돌려준다.)
	virtual bool	ToStarterKit(bool bFullMaterial=false)		{ return false;	}

	virtual unsigned long	GetRepairGold() const				{ return Siege::MAX_REPAIR_GOLD; }
			unsigned long	GetRepairHP();

	// Rider 관련 정보
	virtual unsigned char	IsRider(unsigned long dwCID) const;								// 해당 캐릭터가 병기에 탑승해 있는가?
	virtual bool			Ride(unsigned long dwCID)			{ return false;	}			// 병기 탑승
	virtual bool			GetOff(unsigned long dwCID)			{ return false;	}			// 병기 내림
	virtual void			AllGetOff()							{ GetOff(m_dwRiderCID);	}	// 병기에서 모두 내림

	// 성 오브젝트 관련된 함수 (상징물 업그레이드에 따른 효과)
			void	UpgradeByEmblem(unsigned char cUpgradeType, unsigned char cUpgradeStep);
			void	DegradeByEmblem(unsigned char cUpgradeType, unsigned char cUpgradeStep);
			unsigned short GetDefaultNowHP();			// 상징물의 업그레이드 효과가 없는 현재 HP 얻기

	// ---------------------------------------------------------------------

	// 반경 5셀에 패킷을 전송하는 함수
			void	SendHPUpdateToDBAgent(bool bForce=false);

// 이전 브로드캐스트 함수
//			void	SendToRadiusCell(const char* szPacket, unsigned long dwPacketSize, unsigned char cCMD_In);
//			bool	SerializeOut(char* szBuffer_In, unsigned short& wDataLen);

			void	RangeTest();
	virtual void	RangeTestCallBack( const Position& centerPos, float fDistance, CCharSphereNode* pNode );
			void	SendToRange( float fRadius, const char* szPacket, unsigned long dwPacketSize, unsigned char cCMD_In );

	BroadCastSiege::CSerializeSiegeObjectData&  GetSerializeData(void)  { return m_SerializeSiegeObjectData;	}



	// Get / Set 함수
			bool	IsEmblem() const		{ return (Siege::EMBLEM == m_wObjectType);	}
			bool	IsCamp() const;
			bool	IsGate() const			{ return (Siege::GATE == m_wObjectType);	}
			bool	IsGuard() const			{ return (Siege::GUARD == m_wObjectType);	}
			bool	IsCastleObject() const;
			bool	IsCastleArms() const;
			bool	IsSiegeArms() const;
			bool	IsRidable() const;		// 탈수 있는 오브젝트인가?
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

			void	UpdateObjectInfo(unsigned char cHPType=Siege::NOW_HP, unsigned short wRepairHP=0);	// 상태, 업그레이드 변화시에 내부값들 변경

	unsigned long	m_dwCampOrCastleID;		// CastleID : 성 상징물, 성문, 뒷문, 가드, 근거리/원거리 수성병기
											// CampID   : 길드 요새 상징물, 경비병
	unsigned long	m_dwOwnerID;			// CID      : 근거리/원거리 수성병기, 근거리/원거리 공성병기, 비공정

	unsigned long	m_dwGID;				// GID		: 길드 요새 및 월드 웨폰의 소유 길드의 길드ID
											// 주의!! 공성전 관련 오브젝트는 GID 를 사용하지 않는다!!
	unsigned char	m_cNation;				// 공성전 관련 오브젝트가 사용하는 국가

	unsigned short	m_wObjectType;			// 공성 관련 오브젝트 타입
	unsigned char	m_cState;				// 현재 오브젝트의 상태
	unsigned char	m_cSubState;			// 현재 오브젝트의 서브 상태		(Use EMBLEM & GATE)
	unsigned char	m_cUpgradeStep;			// 현재 오브젝트의 업그레이드 단계
	unsigned char	m_cUpgradeType;			// 성 상징물의 업그레이드 타입		(Only EMBLEM)
	unsigned char	m_cMaterial;			// 공성 자재 갯수
	unsigned char	m_cSiegeCount;			// 공성 지난 횟수
	float			m_fDefaultDir;			// 수성 병기의 기본 방향
	unsigned long	m_dwRiderCID;			// 수성 병기, 공성 병기에 탑승한 캐릭터의 CID
	
	unsigned char	m_cHPUpdateCount;		// HP 정보 업데이트 카운트

	// Rider 정보가 필요한 이유
	// 1. 몇명이나 타고 있는지 알수가 없다.
	// 2. 누군가가 내렸을 경우 누가 내렸는지 알수가 없다.
	//
	// * 드랍쉽에 타고 내릴때 소유주체크를 하면 안된다.

	// ----------------------------------------------------------------------------------------
	// New BroadCast Data
	BroadCastSiege::CSerializeSiegeObjectData   m_SerializeSiegeObjectData;     // 공성 오브젝트 데이터 보관 및 변경된 데이터 보관

	char			m_szBroadCastBuffer[ BroadCastSiege::MAX_SIEGEBROADCAST_BUFFER_SIZE ];
	char			m_szDeltaBuffer[ BroadCastSiege::MAX_SIEGEBROADCAST_BUFFER_SIZE ];
	unsigned long	m_dwBroadCastSize;
	unsigned long	m_dwDeltaSize;

	BroadCastSet	m_BroadCastSet;
	// ----------------------------------------------------------------------------------------

	friend	class	CSiegeObjectMgr;
};

#endif	// _SIEGE_OBJECT_H_