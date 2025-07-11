#ifndef _SIEGE_OBJECT_MANAGER_H_
#define _SIEGE_OBJECT_MANAGER_H_

#pragma once

#include <map>
#include <Creature/Siege/SiegeConstants.h>

#include <Network/Stream/SendStream.h>
#include <Network/Packet/PacketStruct/CastlePacket.h>


// 전방 참조
class CSiegeObject;
class CCamp;

class CSiegeObjectMgr
{
public:
	~CSiegeObjectMgr();

	static CSiegeObjectMgr&	GetInstance();

	// 타입 정의
	typedef	std::map<unsigned long, CSiegeObject*>	SiegeObjectMap;		//	<CID, CSiegeObject*>
	typedef std::map<unsigned long, CCamp*>			CampMap;			//	<CampID, CCamp*>

	void			Destroy();
	void			DestroyAllCamp();

	// DBAgent Server 접속시 DB 에서 얻어온 정보로 객체 생성함수
	CSiegeObject*	CreateCastleObject(CastleObjectInfo& CastleObject);

	// 클라이언트의 요청으로 DBAgentServer 를 거쳐서 객체를 생성하는 함수
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

	bool			HasSiegeArms(unsigned long dwCID);		// 해당 CID 가 공성 병기 소유 여부.

	unsigned long	GetSiegeObjectCount(void);
	
	// ------------------------------------------------------------------------------------------------------
	// 월드 웨폰 관련 함수
	void			SetWorldWeaponInfo(unsigned char cKarRemainSiegeTime, unsigned char cMerRemainSiegeTime);
	void			UpdateWorldWeaponInfo(unsigned char cNation, unsigned char cRemainSiegeTime);
	bool			EnableCreateWorldWeapon(unsigned char cNation, unsigned short& wError) const;
	void			DecreaseWeaponRemainSiegeTime();
	CCamp*			GetWorldWeapon() const						{ return m_lpWorldWeapon;					}
	void			RequestWorldWeaponCreation()				{ m_bRequestWorldWeaponCreation = true;		}
	void			ReleaseWorldWeaponCreation()				{ m_bRequestWorldWeaponCreation = false;	}
	// ------------------------------------------------------------------------------------------------------


	// ------------------------------------------------------------------------------------------------------
	// Process 함수
	void			ProcessAllSiegeObject();
	void			ProcessCampRegenHPAndMP();

	void			ProcessMiningCamp(unsigned long dwProcessType);
	void			ProcessCampShopUpdate(bool bForce);
	void			ProcessWorldWeaponFire();
	// ------------------------------------------------------------------------------------------------------
		
	// DBAgentServer -> GameServer 요새 정보 전송
	bool			SerializeIn(char* lpBuffer, unsigned short wSize, unsigned long dwCID);

	// Client 에게 길드 요새 정보 전송
	bool			SendCampInfo(CSendStream& SendStream);

	// 해당 성의 모든 오브젝트의 주인이 바뀌었다는것을 병기의 반경 5셀에 있는 캐릭터에게 알려준다.
	bool			SendChangeMaster(unsigned long dwCastleID, unsigned char cNation);
	bool			SendLoseOwnership(unsigned long dwCastleID);

	// 모든 공성관련 오브젝트들을 반경 5셀에 있는 캐릭터들에게 알려준다.
	// Vincent - 브로드 캐스트 테스트 코드
	void			PrepareBroadCast();
	void			BroadCast();

	
private:
	CSiegeObjectMgr();

	// DBAgent Server 접속시 DB 에서 얻어온 정보로 객체 생성함수
	CSiegeObject*	CreateCamp(char* lpBuffer, unsigned short wSize, unsigned long dwCID);
	CSiegeObject*	CreateMiningCamp(char* lpBuffer, unsigned short wSize, unsigned long dwCID);
	CSiegeObject*	CreateCampShop(char* lpBuffer, unsigned short wSize, unsigned long dwCID);
	CSiegeObject*	CreateWorldWeapon(char* lpBuffer, unsigned short wSize, unsigned long dwCID, unsigned char cNation);

	SiegeObjectMap	m_SiegeObjectMap;
	CampMap			m_CampMap;

	// ------------------------------------------------------------------------------------------------------
	// 월드 웨폰 관련
	CCamp*			m_lpWorldWeapon;				// 월드 웨폰 (하나 뿐이므로 따로 관리)
	unsigned char	m_cKarRemainSiegeTime;			// 카르테란트 월드 웨폰이 부서진후 지나야할 공성 시간 횟수
	unsigned char	m_cMerRemainSiegeTime;			// 카르테란트 월드 웨폰이 부서진후 지나야할 공성 시간 횟수
	bool			m_bRequestWorldWeaponCreation;	// 월드 웨폰 생성을 DBAgentServer 에 요청한 상태인가
	// ------------------------------------------------------------------------------------------------------
};

#endif	// _SIEGE_OBJECT_MANAGER_H_
