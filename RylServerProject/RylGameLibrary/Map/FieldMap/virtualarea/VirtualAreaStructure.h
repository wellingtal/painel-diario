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

	// ��ũ��Ʈ�� szMapType �� cMapType �� ��Ī�����ֱ� ����
	// BGServerMap �� ��ũ��Ʈȭ ���� �ʰ�, �ϵ��ڵ��� �ϱ� ������ ��Ī�� �ʿ��ϴ�.
	struct MapTypeMatching
	{
		std::map<std::string, unsigned char> m_matchMap;

		MapTypeMatching();
	};

	
	// �� ����
	struct MapInfo
	{
		struct PersonalInfo
		{
			unsigned char m_cEnteringMin;	// ���� ������ ���� ��� �ð�
			unsigned char m_cKill;			// ���� ���� ���� ��
			unsigned char m_cKilled;		// ���� ���� ��

			PersonalInfo(unsigned char cEnteringMin) : m_cEnteringMin(cEnteringMin), m_cKill(0), m_cKilled(0)
			{
			}
		};

		// ��Ʋ �׶��� �ϳ��� ��(���� ��)�� ������ �ϴ� ����
		unsigned char	m_cMapType;
		unsigned char	m_cMaxCharNumOfNation;		// �� ���� �ִ� �ο�
		unsigned char	m_cRemainPlayMin;			// ���� ��� �ð� (�� ����)
		unsigned char	m_cRemainRestMin;			// ���� ���� �ð� (�� ����)
		unsigned short	m_wTargetScore;
		unsigned char	m_cCurrentCharNum[CClass::MAX_RACE];
		unsigned short	m_wScore[CClass::MAX_RACE];
		unsigned char	m_cLimitMin;				// �ð� ���� (�� ����)
		unsigned char	m_cRestMin;					// ���� �ð� (�� ����)

		// ���ӿ� ���� ĳ������ ���� ����
		typedef std::map<unsigned long, PersonalInfo> PersonalInfoMap;
		PersonalInfoMap	m_PersonalInfoMap;

		MapInfo();
		MapInfo(unsigned char cMapType);

		void Initialize();
	};



	// ���� ��� ����
	struct ResultInfo
	{
		unsigned short	m_wScore[CClass::MAX_RACE];
		unsigned char	m_cWinRace;

		ResultInfo();

		void Initialize();
	};
}

#endif	// __VIRTUAL_AREA_STRUCTURE_H__
