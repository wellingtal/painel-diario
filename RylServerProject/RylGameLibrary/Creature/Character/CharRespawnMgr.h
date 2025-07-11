#ifndef _CHARACTER_RESPAWN_MANAGER_H_
#define _CHARACTER_RESPAWN_MANAGER_H_

#pragma once

#include <Creature/CreatureStructure.h>

class CCharRespawnMgr
{
public:
	enum Const
	{
		RESPAWN_DELAY_SEC	= 5,

		SEC_PER_HOUR		= 3600,
		SEC_PER_MIN			= 60,
		MIN_PER_HOUR		= 60
	};

	enum eReturn
	{
		RET_FALSE			= 0,
		RET_TRUE			= 1,
		RET_TOWN			= 2
	};

	enum RespawnType
	{
		RST_TOWN			= 0,	// 죽거나 포탈 이용으로 마을에서 부활 (일반적인 리스폰)
		RST_FORTRESS		= 1,	// 죽거나 포탈 이용으로 요새나 성 및 다크 카나번 석상에서 부활
		RST_RESURRECTION	= 2,	// 레저렉션으로 부활
		RST_BATTLE			= 3		// 배틀 그라운드나 배틀 로한에서 리스폰 시
	};

	struct RespawnProtoType
	{
		unsigned long			m_dwTownID;			// 마을, 길드 요새, 성 ID, 다크 카나번 석상 ID(0x800x0000)
		unsigned long			m_dwGID;			// 길드 요새
		unsigned char			m_cNation;			// 국적 (휴먼, 아칸, 제3세력)
		unsigned char			m_cZone;			// 존
		std::vector<Position>	m_RespawnPosList;	// 리스폰 위치
	};

	struct RespawnCharInfo
	{
		unsigned long	m_dwCID;
		unsigned long	m_dwRemainSec;

		RespawnCharInfo(unsigned long dwCID, unsigned long dwSec)
			: m_dwCID(dwCID), m_dwRemainSec(dwSec) { }
	};

	typedef std::list<RespawnCharInfo>					RespawnCharList;
	typedef std::map<unsigned long, RespawnProtoType>	RespawnProtoTypeMap;
	typedef std::map<unsigned long, RespawnCharList>	RespawnCharMap;
	
	~CCharRespawnMgr();

	static CCharRespawnMgr&	GetInstance();

	// ----------------------------------------------------------------------------
	// 리스폰 스크립트 (RespawnScript.txt)
	// 스크립트의 0x40001000 은 Castle::CASTLE_BIT | (성의 NameID << Castle::CASTLE_NAME_BIT_SHIFT) 입니다.
	// 이후에 성 정보를 DB중계 서버로부터 받아와서 Map 정보에 제대로된 CastleID 가 key 로 들어갑니다.
	bool LoadRespawnFromFile(const char* szFileName = 0);

	
	// 요새 리스폰 포인트 사용
	bool AddCampRespawnPoint(unsigned long dwCampID, unsigned long dwGID, const Position& RespawnPos);
	bool DeleteCampRespawnPoint(unsigned long dwCampID);


	// 다크 카나번 석상 리스폰 포인트 사용
	// TownID 로 0x80010000 처럼 Castle::STATUE_BIT | (석상 Index) << 16) 입니다.
	bool AddRealmStatueRespawnPoint(unsigned long dwIndex, unsigned char cNation, const Position& RespawnPos);
	bool DeleteRealmStatueRespawnPoint(unsigned long dwIndex);


	// 성 리스폰 포인트 사용
	// 수성측 성 리스폰 포인트 키   : CASTLE_BIT | (NameID << Castle::CASTLE_NAME_BIT_SHIFT)
	// 공성측 임시 리스폰 포인트 키 : CASTLE_BIT | SIEGE_BIT | (NameID << Castle::CASTLE_NAME_BIT_SHIFT) | ID(갯수별)
	bool SetCastleRespawnPointNation(unsigned long dwCastleNameID, unsigned char cNation);

	
	eReturn AddCharacter(unsigned long dwTownID, unsigned long dwCID);
//	void DelCharacter(unsigned long dwTownID, unsigned long dwCID);

	void ProcessRespawn();

	const Position GetDefaultRespawnPos(unsigned char cNation);
	const Position GetRespawnPos(unsigned long dwTownID);
	const Position GetTownRespawnPos(unsigned char cZone, unsigned char cNation);
	
	// ----------------------------------------------------------------------------
	// Send 함수
	bool SendRespawnInfo(unsigned long dwCID);
	bool SendRespawnAreaInfo(unsigned long dwCID, unsigned long dwTownID, bool bCount=false);
	
	
private:
	CCharRespawnMgr();

	RespawnProtoTypeMap		m_ProtoTypeList;	// 현재 존의 리스폰 포인트 정보
	RespawnProtoTypeMap		m_AllZoneTownList;	// 전 존의 마을 리스폰 포인트 정보
	RespawnCharMap			m_CharList;
    
	static const char*		ms_szRespawnScriptFileName;
};

#endif
