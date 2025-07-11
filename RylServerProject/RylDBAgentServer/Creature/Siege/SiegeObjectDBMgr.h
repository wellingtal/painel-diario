#ifndef _SIEGE_OBJECT_MANAGER_H_
#define _SIEGE_OBJECT_MANAGER_H_

#pragma once

#include <map>
#include <list>
#include <boost/pool/pool.hpp>
#include <boost/pool/pool_alloc.hpp>
#include <Creature/Siege/SiegeConstants.h>
#include <Network/Packet/PacketStruct/CastlePacket.h>

// 전방 참조
class CSendStream;
class CDBComponent;
class CSiegeObjectDB;
class CCampDB;

class CSiegeObjectDBMgr
{
public:

    static CSiegeObjectDBMgr& GetInstance();

	// 타입 정의
	typedef	std::map<unsigned long, CSiegeObjectDB*>	SiegeObjectMap;		// < CID, CSiegeObjectDB* >
	typedef std::map<unsigned long, CCampDB*>			CampMap;			// < CampID, CCampDB* >
    
	bool	Initialize(CDBComponent& DBComponent);
	void	Process();
	void	UpdateSiegeCount();											// 공성 시간 지난 횟수 증가 및 업데이트

	void	UpdateRewardFame();											// 명성을 보상한다.

	// 월드 웨폰 파괴정보의 공성 시간 지난 횟수 증가
	void	DecreaseWeaponRemainSiegeTime();							// 공성 시간이 끝나는 시점에 호출된다.

	// DB 에서 읽어들인 값으로 생성하는 함수
	CSiegeObjectDB*		CreateCastleObject(CastleObjectInfoDB& CastleObject);
	CSiegeObjectDB*		CreateCamp(CampInfoDB& CampInfo, unsigned long dwCID=0);
	CSiegeObjectDB*		CreateMiningCamp(CampInfoDB& CampInfo, unsigned long dwCID=0);
	CSiegeObjectDB*		CreateCampShop(CampInfoDB& CampInfo, unsigned long dwCID=0);
	CSiegeObjectDB*		CreateWorldWeapon(CampInfoDB& CampInfo, unsigned long dwCID=0);

	// 게임서버의 요청으로 생성해서 DB 에 추가하는 함수
	CSiegeObjectDB*		CreateCamp(unsigned long dwGID, unsigned char cZone, unsigned char cChannel, POS Pos);
	CSiegeObjectDB*		CreateWorldWeapon(unsigned long dwGID, unsigned short wObjectType, unsigned char cZone, unsigned char cChannel, POS Pos);
	CSiegeObjectDB*		CreateSiegeArms(unsigned long dwOwnerID, unsigned char cNation, unsigned char cZone, unsigned char cChannel,
										unsigned short wObjectType,	unsigned char cUpgradeStep, POS Pos);

	// Get 함수
	CCampDB*			GetCamp(unsigned long dwCampID);
	CSiegeObjectDB*		GetSiegeObject(unsigned long dwObjectID);
	unsigned char		GetWorldWeaponNation(unsigned char cZone, unsigned char cChannel) const;	// 월드 웨폰 소유 국가 얻기
	void				DeleteWorldWeaponNation(unsigned char cZone, unsigned char cChannel);		// 월드 웨폰 소유 국가 초기화
	void				UpdateWorldWeaponInfo(unsigned char cZone, unsigned char cChannel, unsigned char cOwenrNation,
											  unsigned char cDestroyedWeaponNation, unsigned char cRemainSiegeTime);
	bool				HasCampByGID(unsigned long dwGID, unsigned long dwExceptCampID=0);
	
	// 객체 삭제 함수
	bool	DeleteSiegeObject(unsigned long dwCID);
	bool	DeleteSiegeObject(CSiegeObjectDB* lpObject);
	CampMap::iterator	DeleteCamp(unsigned long dwCampID);

	int		DestroyCamp(unsigned long dwGID);				// 해당 길드의 진지를 모두 파괴, 파괴한 진지수를 리턴
	void	DestroySiegeArms(unsigned long dwOnwerCID);		// 길드 탈퇴시 해당 캐릭터의 공성병기를 아이템으로 돌려주기
	void	DestoryAllSiegeArms();							// 공성 시간이 끝났을때 아이템으로 돌려주기

	// 요새 객체 변형 함수
	bool	ChangeCampType(unsigned long dwCampID, unsigned short wChangeType);

	// GameServer 로의 Send 함수
	void	SendWorldWeaponInfo(CSendStream& SendStream, unsigned char cZone, unsigned char cChannel);
	void	SendCampInfo(CSendStream& SendStream, unsigned char cZone, unsigned char cChannel);
	void	SendSiegeArmsInfo(CSendStream& SendStream, unsigned char cZone, unsigned char cChannel);
	void	SendProcessMining(unsigned long dwProcessType);

	bool	SerializeOutCastleObject(CSiegeObjectDB* lpObject_In, char* lpBuffer_Out, unsigned short& wBufferSize_Out);

private:

    CSiegeObjectDBMgr();
    ~CSiegeObjectDBMgr();

	// CSiegeObjectDBMgr 내에서만 사용하는 타입 정의
	typedef struct _WorldWeaponInfo
	{
		unsigned char	m_cWorldWeaponNation;		// 현재 있는 월드 웨폰의 소유 국가
		unsigned char	m_cKarRemainSiegeTime;		// 카르테란트 월드 웨폰이 부서지고난 후 지나야할 공성시간
		unsigned char	m_cMerRemainSiegeTime;		// 메르카디아 월드 웨폰이 부서지고난 후 지나야할 공성시간
	} WorldWeaponInfo;

	typedef	std::map<unsigned long, WorldWeaponInfo>	WorldWeaponInfoMap;	// < dwZoneChannel, WorldWeaponInfo >
																			// dwZoneChannel : Zone(16bit) | Channel(16bit)
	bool	ReadCastleObjectInfoDB();
	bool	ReadWorldWeaponInfoDB();
	bool	ReadCampInfoDB();
	
	unsigned long	GetBlankIndex();
	
	void	CreateDummyCamp();

	// 실제로 객체를 생성하는 함수
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