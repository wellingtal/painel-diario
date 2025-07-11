#ifndef __RYL_DBAGENT_MINERAL_VEIN_MANAGER_H__
#define __RYL_DBAGENT_MINERAL_VEIN_MANAGER_H__

#pragma once

#include <map>

// ��������
class CSendStream;
class CDBComponent;

// ���� ���� Ŭ���� (�ٸ� ������ ����, ������ ���� ������ �Ѵ�.)
class CMineralVeinDBMgr
{
public:

	static CMineralVeinDBMgr&		GetInstance();

	typedef std::map<unsigned long, unsigned long>		FertilityInfoMap;	// < VeinColor, Fertility >,		Fertility Info Per Zone
	typedef std::map<unsigned long, FertilityInfoMap>	FertilityZoneMap;	// < ZoneChannel, FertilityInfo >,	Fertility Info Of All Zone, Channel
																			// ZoneChannel : Zone (16bit), Channel (16bit)

	bool	Initialize(CDBComponent& DBComponent);
	void	Destroy();

	bool	SendFertilityInfo(CSendStream& SendStream, unsigned char cZone, unsigned char cChannel);
	bool	UpdateFertilityInfo(unsigned char cZone, unsigned char cChannel, unsigned short wNum, unsigned long* lpFertilityInfo);

private:

	CMineralVeinDBMgr();
	~CMineralVeinDBMgr();

	bool	ReadFertilityDB();

	FertilityZoneMap	m_mapFertility;
	CDBComponent*		m_lpDBComponent;
};

#endif	// __RYL_DBAGENT_MINERAL_VEIN_MANAGER_H__
