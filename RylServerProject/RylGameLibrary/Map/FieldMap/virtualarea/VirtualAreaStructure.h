#ifndef __VIRTUAL_AREA_STRUCTURE_H__
#define __VIRTUAL_AREA_STRUCTURE_H__

#pragma once

#include <map>
#include <Map/FieldMap/VirtualArea/VirtualAreaConstants.h>

using namespace std;

namespace VirtualArea
{
	// VirtualArea Info
	struct ProtoType
	{
		unsigned long	m_dwVID;
		unsigned char	m_cMapType;
		unsigned char	m_cZone;
		unsigned short	m_wStartX;
		unsigned short	m_wStartZ;
		unsigned short	m_wWidth;
		unsigned short	m_wHeight;
		unsigned char	m_cMaxRespawnPos;
		char			m_szArrangementFileName[MAX_FILE_NAME];
		char			m_szMapType[MAX_MAP_TYPE_NAME];
		Position		m_StartPos[CClass::MAX_RACE];
		Position		m_RespawnPos[CClass::MAX_RACE][MAX_VIRTUAL_AREA_RESPAWN_POINT];

		inline bool operator < (ProtoType& rhs)
		{ return m_dwVID < rhs.m_dwVID; }
	};

	// 스크립트의 szMapType 과 cMapType 을 매칭시켜주기 위함
	// BGServerMap 을 스크립트화 하지 않고, 하드코딩을 하기 때문에 매칭이 필요하다.
	struct MapTypeMatching
	{
		std::map<std::string, unsigned char> m_matchMap;

		MapTypeMatching();
	};

	
	// 방 정보
	struct MapInfo
	{
		struct PersonalInfo
		{
			unsigned char m_cEnteringMin;	// 들어온 시점의 남은 경기 시간
			unsigned char m_cKill;			// 내가 죽인 유저 수
			unsigned char m_cKilled;		// 내가 죽은 수

			PersonalInfo(unsigned char cEnteringMin) : m_cEnteringMin(cEnteringMin), m_cKill(0), m_cKilled(0)
			{
			}
		};

		// 배틀 그라운드 하나의 방(가상 맵)이 가져야 하는 변수
		unsigned char	m_cMapType;
		unsigned char	m_cMaxCharNumOfNation;		// 한 진영 최대 인원
		unsigned char	m_cRemainPlayMin;			// 남은 경기 시간 (분 단위)
		unsigned char	m_cRemainRestMin;			// 남은 쉬는 시간 (분 단위)
		unsigned short	m_wTargetScore;
		unsigned char	m_cCurrentCharNum[CClass::MAX_RACE];
		unsigned short	m_wScore[CClass::MAX_RACE];
		unsigned char	m_cLimitMin;				// 시간 제한 (분 단위)
		unsigned char	m_cRestMin;					// 쉬는 시간 (분 단위)

		// 게임에 들어온 캐릭터의 개인 정보
		typedef std::map<unsigned long, PersonalInfo> PersonalInfoMap;
		PersonalInfoMap	m_PersonalInfoMap;

		MapInfo();
		MapInfo(unsigned char cMapType);

		void Initialize();
	};



	// 방의 결과 정보
	struct ResultInfo
	{
		unsigned short	m_wScore[CClass::MAX_RACE];
		unsigned char	m_cWinRace;

		ResultInfo();

		void Initialize();
	};
}

#endif	// __VIRTUAL_AREA_STRUCTURE_H__
