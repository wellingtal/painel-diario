#include "stdafx.h"
#include "VirtualAreaStructure.h"


VirtualArea::MapTypeMatching::MapTypeMatching()
{
	m_matchMap.clear();
	m_matchMap.insert(make_pair("BG_FRAG", VirtualArea::FRAG));
	m_matchMap.insert(make_pair("BG_STATUE", VirtualArea::STATUE));
	m_matchMap.insert(make_pair("DUEL_FRAG", VirtualArea::FRAG));
	m_matchMap.insert(make_pair("DUEL_STATUE", VirtualArea::STATUE));
}



VirtualArea::MapInfo::MapInfo()
:  m_cMapType(0), m_cMaxCharNumOfNation(0), m_cRemainPlayMin(0),
	m_cRemainRestMin(0), m_wTargetScore(0), m_cLimitMin(0), m_cRestMin(0)
{
	std::fill_n(m_cCurrentCharNum, int(CClass::MAX_RACE), 0);
	std::fill_n(m_wScore, int(CClass::MAX_RACE), 0);

	m_PersonalInfoMap.clear();
}

VirtualArea::MapInfo::MapInfo(unsigned char cMapType)
: m_cMapType(cMapType), m_cMaxCharNumOfNation(DefaultMaxCharacterNumOfNation[cMapType]),
  m_cRemainPlayMin(DefaultLimitMin[cMapType]), m_cRemainRestMin(DefaultRestMin[cMapType]),
  m_wTargetScore(DefaultTargetScore[cMapType]),
  m_cLimitMin(DefaultLimitMin[cMapType]), m_cRestMin(DefaultRestMin[cMapType])
{
	std::fill_n(m_cCurrentCharNum, int(CClass::MAX_RACE), 0);
	std::fill_n(m_wScore, int(CClass::MAX_RACE), 0);

	m_PersonalInfoMap.clear();
}

void VirtualArea::MapInfo::Initialize()
{
	m_cRemainPlayMin = m_cLimitMin;
	m_cRemainRestMin = m_cRestMin;

	std::fill_n(m_cCurrentCharNum, int(CClass::MAX_RACE), 0);
	std::fill_n(m_wScore, int(CClass::MAX_RACE), 0);

	m_PersonalInfoMap.clear();
}


VirtualArea::ResultInfo::ResultInfo()
{
	Initialize();
}

void VirtualArea::ResultInfo::Initialize()
{
	m_cWinRace = CClass::MAX_RACE;
	std::fill_n(m_wScore, int(CClass::MAX_RACE), 0);
}

