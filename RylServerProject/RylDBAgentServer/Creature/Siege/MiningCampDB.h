#ifndef _RYL_DBAGENT_MINING_CAMP_H_
#define _RYL_DBAGENT_MINING_CAMP_H_

#pragma once

#include "CampDB.h"
#include <map>
#include <Network/Packet/PacketStruct/CastlePacket.h>

using namespace Siege;


class CMiningCampDB : public CCampDB
{
public:
	virtual ~CMiningCampDB();

	typedef std::map<unsigned short, unsigned short>	MineralInfoMap;			// <wMineralID, wAmount>
	
	bool	OnOff(unsigned long dwSubState);
	bool	GainMineral(unsigned short wMineralID, unsigned short wAmount);		// 누적 광물 획득
	
	// 주의 AddMineral 함수는 insert만 담당함!! (이미 있는 녀석의 Add 는 하지 않음)
	bool	AddMineral(unsigned char cFlag, const CampMineralInfo& campMineralInfo);
	bool	UpdateMineralInfo(unsigned char cFlag, unsigned char cNum, CampMineralInfo* lpMineralInfo);

	bool	SerializeOut(char* szBuffer_Out, unsigned short& usBufferSize_Out, unsigned long& dwCID_Out);

private:
	CMiningCampDB(CDBComponent& DBComponent, const CampInfoDB& CampInfo);

	MineralInfoMap	m_AccumulatedMineralMap;
	MineralInfoMap	m_TemporaryMineralMap;

	friend	class	CSiegeObjectDBMgr;
};

#endif	// _RYL_DBAGENT_MINING_CAMP_H_
