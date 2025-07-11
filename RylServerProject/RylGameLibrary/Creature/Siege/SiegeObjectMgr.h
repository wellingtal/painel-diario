#ifndef _SIEGE_OBJECT_MANAGER_H_
#define _SIEGE_OBJECT_MANAGER_H_

#pragma once

#include <map>
#include <Creature/Siege/SiegeConstants.h>

#include <Network/Stream/SendStream.h>
#include <Network/Packet/PacketStruct/CastlePacket.h>


// ���� ����
class CSiegeObject;
class CCamp;

class CSiegeObjectMgr
{
public:
	~CSiegeObjectMgr();

	static CSiegeObjectMgr&	GetInstance();

	// Ÿ�� ����
	typedef	std::map<unsigned long, CSiegeObject*>	SiegeObjectMap;		//	<CID, CSiegeObject*>
	typedef std::map<unsigned long, CCamp*>			CampMap;			//	<CampID, CCamp*>

	void			Destroy();
	void			DestroyAllCamp();

	// DBAgent Server ���ӽ� DB ���� ���� ������ ��ü �����Լ�
	CSiegeObject*	CreateCastleObject(CastleObjectInfo& CastleObject);

	// Ŭ���̾�Ʈ�� ��û���� DBAgentServer �� ���ļ� ��ü�� �����ϴ� �Լ�
	CSiegeObject*	CreateCamp(unsigned long dwCID, unsigned long dwCampID, unsigned long dwGID, unsigned long dwHP,
							  unsigned char cState, unsigned char cUpgradeStep, const CampRight& campRight,
							  Position Pos, unsigned char cMaterial, bool bFullHP);

	CSiegeObject*	CreateWorldWeapon(unsigned long dwCID, unsigned long dwCampID, unsigned long dwGID,	unsigned long dwHP,
									unsigned char cState, unsigned char cUpgradeStep, const CampRight& campRight,
									Position Pos, unsigned char cMaterial, bool bFullHP, unsigned short wObjectType);
							
	CSiegeObject*	CreateSiegeArms(unsigned long dwCID, unsigned long dwOwnerID, unsigned char cNation, unsigned long dwHP,
									unsigned short wObjectType, unsigned char cState, unsigned char cUpgradeStep,
									Position Pos);


	CSiegeObject*	ChangeCampType(unsigned long dwCampID, unsigned short wChangeType);

	CCamp*			GetCamp(unsigned long dwCampID);
	CSiegeObject*	GetSiegeObject(unsigned long dwObjectID);

	bool			DeleteSiegeObject(unsigned long dwCID);
	bool			DeleteSiegeObject(CSiegeObject* lpObject);
	bool			DeleteCamp(unsigned long dwCampID);

	bool			ExistBuildingOrDestroyingCamp(unsigned long dwGID);
	bool			ExistCampInRadius(const Position& Pos);
	bool			ExistCompleteCampInRadius(const Position& Pos, unsigned long dwGID);
	void			HasCampByGID(unsigned long dwGID, bool& bHasDevelopingCamp, bool& bHasCompleteCamp);

	bool			HasSiegeArms(unsigned long dwCID);		// �ش� CID �� ���� ���� ���� ����.

	unsigned long	GetSiegeObjectCount(void);
	
	// ------------------------------------------------------------------------------------------------------
	// ���� ���� ���� �Լ�
	void			SetWorldWeaponInfo(unsigned char cKarRemainSiegeTime, unsigned char cMerRemainSiegeTime);
	void			UpdateWorldWeaponInfo(unsigned char cNation, unsigned char cRemainSiegeTime);
	bool			EnableCreateWorldWeapon(unsigned char cNation, unsigned short& wError) const;
	void			DecreaseWeaponRemainSiegeTime();
	CCamp*			GetWorldWeapon() const						{ return m_lpWorldWeapon;					}
	void			RequestWorldWeaponCreation()				{ m_bRequestWorldWeaponCreation = true;		}
	void			ReleaseWorldWeaponCreation()				{ m_bRequestWorldWeaponCreation = false;	}
	// ------------------------------------------------------------------------------------------------------


	// ------------------------------------------------------------------------------------------------------
	// Process �Լ�
	void			ProcessAllSiegeObject();
	void			ProcessCampRegenHPAndMP();

	void			ProcessMiningCamp(unsigned long dwProcessType);
	void			ProcessCampShopUpdate(bool bForce);
	void			ProcessWorldWeaponFire();
	// ------------------------------------------------------------------------------------------------------
		
	// DBAgentServer -> GameServer ��� ���� ����
	bool			SerializeIn(char* lpBuffer, unsigned short wSize, unsigned long dwCID);

	// Client ���� ��� ��� ���� ����
	bool			SendCampInfo(CSendStream& SendStream);

	// �ش� ���� ��� ������Ʈ�� ������ �ٲ���ٴ°��� ������ �ݰ� 5���� �ִ� ĳ���Ϳ��� �˷��ش�.
	bool			SendChangeMaster(unsigned long dwCastleID, unsigned char cNation);
	bool			SendLoseOwnership(unsigned long dwCastleID);

	// ��� �������� ������Ʈ���� �ݰ� 5���� �ִ� ĳ���͵鿡�� �˷��ش�.
	// Vincent - ��ε� ĳ��Ʈ �׽�Ʈ �ڵ�
	void			PrepareBroadCast();
	void			BroadCast();

	
private:
	CSiegeObjectMgr();

	// DBAgent Server ���ӽ� DB ���� ���� ������ ��ü �����Լ�
	CSiegeObject*	CreateCamp(char* lpBuffer, unsigned short wSize, unsigned long dwCID);
	CSiegeObject*	CreateMiningCamp(char* lpBuffer, unsigned short wSize, unsigned long dwCID);
	CSiegeObject*	CreateCampShop(char* lpBuffer, unsigned short wSize, unsigned long dwCID);
	CSiegeObject*	CreateWorldWeapon(char* lpBuffer, unsigned short wSize, unsigned long dwCID, unsigned char cNation);

	SiegeObjectMap	m_SiegeObjectMap;
	CampMap			m_CampMap;

	// ------------------------------------------------------------------------------------------------------
	// ���� ���� ����
	CCamp*			m_lpWorldWeapon;				// ���� ���� (�ϳ� ���̹Ƿ� ���� ����)
	unsigned char	m_cKarRemainSiegeTime;			// ī���׶�Ʈ ���� ������ �μ����� �������� ���� �ð� Ƚ��
	unsigned char	m_cMerRemainSiegeTime;			// ī���׶�Ʈ ���� ������ �μ����� �������� ���� �ð� Ƚ��
	bool			m_bRequestWorldWeaponCreation;	// ���� ���� ������ DBAgentServer �� ��û�� �����ΰ�
	// ------------------------------------------------------------------------------------------------------
};

#endif	// _SIEGE_OBJECT_MANAGER_H_
