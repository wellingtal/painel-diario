#ifndef _MINING_CAMP_H_
#define _MINING_CAMP_H_

#pragma once

#include <map>
#include <Creature/Siege/Camp.h>
#include <Network/Packet/PacketStruct/CastlePacket.h>

using namespace Siege;

class CSendStream;

class CMiningCamp : public CCamp
{
public:
	virtual ~CMiningCamp();

	typedef std::map<unsigned short, unsigned short>	MineralInfoMap;			// <wMineralID, wAmount>

	void	ProcessMining(unsigned long dwProcessType);

	bool	Destroy(unsigned long dwOffencerGID = 0);		// 채굴기 파괴 완료시 처리

	bool	OnOff(unsigned char cSubState);
	bool	GainMineral(unsigned short wMineralID, unsigned short wAmount);

	bool	AddMineral(unsigned char cFlag, const CampMineralInfo& campMineralInfo);
	bool	AddMineral(unsigned char cFlag, unsigned short wMineralID, unsigned short wAmount);

	unsigned short	GetMineralNum(unsigned char cFlag, unsigned short wMineralID) const;
	unsigned short	GetMineralTypeNum(unsigned char cFlag) const;

	// 광물 정보 전송
	bool	SendMineralInfo(CSendStream& SendStream, unsigned char cMineralType=Siege::ACCUMULATED_MINERAL);

protected:
	
	CMiningCamp(MonsterCreateInfo& MonsterCreate, unsigned long dwCampID, unsigned long dwGID, 
				unsigned long dwHP, unsigned short wObjectType, unsigned char cState, unsigned char cSubState,
				unsigned char cUpgradeStep, unsigned char cMaterial, unsigned char cSiegeCount,
				const CampRight& campRight, bool bFullHP);


	MineralInfoMap		m_AccumulatedMineralMap;
	MineralInfoMap		m_TemporaryMineralMap;

	friend	class	CSiegeObjectMgr;
};

#endif	// _MINING_CAMP_H_