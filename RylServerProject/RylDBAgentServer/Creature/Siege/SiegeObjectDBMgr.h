#ifndef _SIEGE_OBJECT_MANAGER_H_
#define _SIEGE_OBJECT_MANAGER_H_

#pragma once

#include <map>
#include <list>
#include <boost/pool/pool.hpp>
#include <boost/pool/pool_alloc.hpp>
#include <Creature/Siege/SiegeConstants.h>
#include <Network/Packet/PacketStruct/CastlePacket.h>

// ���� ����
class CSendStream;
class CDBComponent;
class CSiegeObjectDB;
class CCampDB;

class CSiegeObjectDBMgr
{
public:

    static CSiegeObjectDBMgr& GetInstance();

	// Ÿ�� ����
	typedef	std::map<unsigned long, CSiegeObjectDB*>	SiegeObjectMap;		// < CID, CSiegeObjectDB* >
	typedef std::map<unsigned long, CCampDB*>			CampMap;			// < CampID, CCampDB* >
    
	bool	Initialize(CDBComponent& DBComponent);
	void	Process();
	void	UpdateSiegeCount();											// ���� �ð� ���� Ƚ�� ���� �� ������Ʈ

	void	UpdateRewardFame();											// ���� �����Ѵ�.

	// ���� ���� �ı������� ���� �ð� ���� Ƚ�� ����
	void	DecreaseWeaponRemainSiegeTime();							// ���� �ð��� ������ ������ ȣ��ȴ�.

	// DB ���� �о���� ������ �����ϴ� �Լ�
	CSiegeObjectDB*		CreateCastleObject(CastleObjectInfoDB& CastleObject);
	CSiegeObjectDB*		CreateCamp(CampInfoDB& CampInfo, unsigned long dwCID=0);
	CSiegeObjectDB*		CreateMiningCamp(CampInfoDB& CampInfo, unsigned long dwCID=0);
	CSiegeObjectDB*		CreateCampShop(CampInfoDB& CampInfo, unsigned long dwCID=0);
	CSiegeObjectDB*		CreateWorldWeapon(CampInfoDB& CampInfo, unsigned long dwCID=0);

	// ���Ӽ����� ��û���� �����ؼ� DB �� �߰��ϴ� �Լ�
	CSiegeObjectDB*		CreateCamp(unsigned long dwGID, unsigned char cZone, unsigned char cChannel, POS Pos);
	CSiegeObjectDB*		CreateWorldWeapon(unsigned long dwGID, unsigned short wObjectType, unsigned char cZone, unsigned char cChannel, POS Pos);
	CSiegeObjectDB*		CreateSiegeArms(unsigned long dwOwnerID, unsigned char cNation, unsigned char cZone, unsigned char cChannel,
										unsigned short wObjectType,	unsigned char cUpgradeStep, POS Pos);

	// Get �Լ�
	CCampDB*			GetCamp(unsigned long dwCampID);
	CSiegeObjectDB*		GetSiegeObject(unsigned long dwObjectID);
	unsigned char		GetWorldWeaponNation(unsigned char cZone, unsigned char cChannel) const;	// ���� ���� ���� ���� ���
	void				DeleteWorldWeaponNation(unsigned char cZone, unsigned char cChannel);		// ���� ���� ���� ���� �ʱ�ȭ
	void				UpdateWorldWeaponInfo(unsigned char cZone, unsigned char cChannel, unsigned char cOwenrNation,
											  unsigned char cDestroyedWeaponNation, unsigned char cRemainSiegeTime);
	bool				HasCampByGID(unsigned long dwGID, unsigned long dwExceptCampID=0);
	
	// ��ü ���� �Լ�
	bool	DeleteSiegeObject(unsigned long dwCID);
	bool	DeleteSiegeObject(CSiegeObjectDB* lpObject);
	CampMap::iterator	DeleteCamp(unsigned long dwCampID);

	int		DestroyCamp(unsigned long dwGID);				// �ش� ����� ������ ��� �ı�, �ı��� �������� ����
	void	DestroySiegeArms(unsigned long dwOnwerCID);		// ��� Ż��� �ش� ĳ������ �������⸦ ���������� �����ֱ�
	void	DestoryAllSiegeArms();							// ���� �ð��� �������� ���������� �����ֱ�

	// ��� ��ü ���� �Լ�
	bool	ChangeCampType(unsigned long dwCampID, unsigned short wChangeType);

	// GameServer ���� Send �Լ�
	void	SendWorldWeaponInfo(CSendStream& SendStream, unsigned char cZone, unsigned char cChannel);
	void	SendCampInfo(CSendStream& SendStream, unsigned char cZone, unsigned char cChannel);
	void	SendSiegeArmsInfo(CSendStream& SendStream, unsigned char cZone, unsigned char cChannel);
	void	SendProcessMining(unsigned long dwProcessType);

	bool	SerializeOutCastleObject(CSiegeObjectDB* lpObject_In, char* lpBuffer_Out, unsigned short& wBufferSize_Out);

private:

    CSiegeObjectDBMgr();
    ~CSiegeObjectDBMgr();

	// CSiegeObjectDBMgr �������� ����ϴ� Ÿ�� ����
	typedef struct _WorldWeaponInfo
	{
		unsigned char	m_cWorldWeaponNation;		// ���� �ִ� ���� ������ ���� ����
		unsigned char	m_cKarRemainSiegeTime;		// ī���׶�Ʈ ���� ������ �μ����� �� �������� �����ð�
		unsigned char	m_cMerRemainSiegeTime;		// �޸�ī��� ���� ������ �μ����� �� �������� �����ð�
	} WorldWeaponInfo;

	typedef	std::map<unsigned long, WorldWeaponInfo>	WorldWeaponInfoMap;	// < dwZoneChannel, WorldWeaponInfo >
																			// dwZoneChannel : Zone(16bit) | Channel(16bit)
	bool	ReadCastleObjectInfoDB();
	bool	ReadWorldWeaponInfoDB();
	bool	ReadCampInfoDB();
	
	unsigned long	GetBlankIndex();
	
	void	CreateDummyCamp();

	// ������ ��ü�� �����ϴ� �Լ�
    CSiegeObjectDB*		CreateCastleObjectDB(const CastleObjectInfoDB& CastleObject);
    CSiegeObjectDB*		CreateCampDB(const CampInfoDB& CampInfo);
	CSiegeObjectDB*		CreateMiningCampDB(const CampInfoDB& CampInfo);
	CSiegeObjectDB*		CreateCampShopDB(const CampInfoDB& CampInfo);
	CSiegeObjectDB*		CreateWorldWeaponDB(const CampInfoDB& CampInfo);

    CSiegeObjectDB*		CreateCampDB(unsigned long dwCampID, unsigned long dwGID, 
									 unsigned char cZone, unsigned char cChannel, Position Pos);
	CSiegeObjectDB*		CreateWorldWeaponDB(unsigned long dwCampID, unsigned long dwGID, unsigned short wObjectType,
											unsigned char cZone, unsigned char cChannel, Position Pos);
    CSiegeObjectDB*		CreateSiegeArmsDB(unsigned long dwOwnerID, 
											unsigned char cNation, unsigned short wObjectType, unsigned char cUpgradeStep, 
											unsigned char cZone, unsigned char cChannel, Position Pos);

    void	DeleteSiegeObjectDB(CSiegeObjectDB* lpSiegeObjectDB);

	SiegeObjectMap		m_SiegeObjectMap;
	CampMap				m_CampMap;
	WorldWeaponInfoMap	m_WorldWeaponInfoMap;
    CDBComponent*		m_lpDBComponent;
};

#endif	// _SIEGE_OBJECT_MANAGER_H_