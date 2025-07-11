#include "stdafx.h"
#include "SerializeSiegeObjectData.h"
#include <Creature/Siege/SiegeObject.h>


BroadCastSiege::CSerializeSiegeObjectData::CSerializeSiegeObjectData() :
	m_wBroadCastDataLen(0), m_wDeltaBroadCastDataLen(0)
{
}


BroadCastSiege::CSerializeSiegeObjectData::~CSerializeSiegeObjectData()
{
}

void
BroadCastSiege::CSerializeSiegeObjectData::Initialize( const CSiegeObject& siegeObject )
{
	// Data ������
	sOwnerShipInfo ownerShipInfo;
	ownerShipInfo.m_dwCampOrCastleID	= siegeObject.GetCampID();
	ownerShipInfo.m_dwOwnerCID			= siegeObject.GetOwnerID();
	ownerShipInfo.m_dwGID				= siegeObject.GetGID();
	ownerShipInfo.m_cNation				= siegeObject.GetNation();

	sStateInfo stateInfo;
	stateInfo.m_wObjectType		= siegeObject.GetObjectType();
	stateInfo.m_cState			= siegeObject.GetState();
	stateInfo.m_cSubState		= siegeObject.GetSubState();
	stateInfo.m_cUpgradeStep	= siegeObject.GetUpgradeStep();
	stateInfo.m_cUpgradeType	= siegeObject.GetUpgradeType();

	sHPInfo hpInfo;
	hpInfo.m_dwNowHP	= siegeObject.GetNowHP();
	hpInfo.m_dwMaxHP	= siegeObject.GetMaxHP();

	sPosInfo posInfo;
	const Position& pos		= siegeObject.GetPosition();
	const MotionInfo& motionInfo = siegeObject.GetMotionInfo();
	posInfo.m_fDefaultDir	= siegeObject.GetDefaultDir();
	posInfo.m_NetworkPos	= CNetworkPos( pos.m_fPointX, pos.m_fPointY, pos.m_fPointZ, motionInfo.m_fDirection,
										   (0 == motionInfo.m_dwFrame) ? 0.0f : motionInfo.m_fVelocity / motionInfo.m_dwFrame );

	sMaterialInfo materialInfo;
	materialInfo.m_cMaterial = siegeObject.GetMaterialNum();

	sRiderInfo riderInfo;
	std::fill_n(&riderInfo.m_dwRiderID[0], int(Siege::AIRSHIP_RIDER_NUM), 0);
	siegeObject.GetRiders( riderInfo.m_dwRiderID );

	// ���� Data �� �ʱ�ȭ
	m_LastSiegeObjectData.SetOwnerShipInfo( ownerShipInfo );
	m_LastSiegeObjectData.SetStateInfo( stateInfo );
	m_LastSiegeObjectData.SetHPInfo( hpInfo );
	m_LastSiegeObjectData.SetPosInfo( posInfo );
	m_LastSiegeObjectData.SetMaterialInfo( materialInfo );
	m_LastSiegeObjectData.SetRiderInfo( riderInfo );

	// BroadCast ���� ����
	PrepareData( siegeObject );
}


// ==================================================================================
// Data �� ����
void
BroadCastSiege::CSerializeSiegeObjectData::PrepareData( const CSiegeObject& siegeObject )
{
	PrepareBroadCastData( siegeObject );
	PrepareDeltaData( siegeObject );
}

void
BroadCastSiege::CSerializeSiegeObjectData::PrepareBroadCastData( const CSiegeObject& siegeObject )
{
	assert( CSiegeObjectData::EstimateBufferSize(0xFFFFFFFF) < MAX_SIEGEOBJECT_DATA_SIZE && "MAX_SIEGEOBJECT_DATA_SIZE �� �÷��ּ���!" );

	m_wBroadCastDataLen = 0;
	char* szDataPos = m_aryBroadCastData;

	// UpdateDataFlag ���� (��� Data �� ������.)
	unsigned long dwUpdateFlag = 0xFFFFFFFF;
	COPY_AND_ADVANCE_DST( szDataPos, &dwUpdateFlag, sizeof(unsigned long) );

	// ������ ���� ����
	sOwnerShipInfo ownerShipInfo;
	ownerShipInfo.m_dwCampOrCastleID	= siegeObject.GetCampID();
	ownerShipInfo.m_dwOwnerCID			= siegeObject.GetOwnerID();
	ownerShipInfo.m_dwGID				= siegeObject.GetGID();
	ownerShipInfo.m_cNation				= siegeObject.GetNation();
	COPY_AND_ADVANCE_DST( szDataPos, &ownerShipInfo, sizeof(sOwnerShipInfo) );

	// ���� ���� ����
	sStateInfo stateInfo;
	stateInfo.m_wObjectType		= siegeObject.GetObjectType();
	stateInfo.m_cState			= siegeObject.GetState();
	stateInfo.m_cSubState		= siegeObject.GetSubState();
	stateInfo.m_cUpgradeStep	= siegeObject.GetUpgradeStep();
	stateInfo.m_cUpgradeType	= siegeObject.GetUpgradeType();
	COPY_AND_ADVANCE_DST( szDataPos, &stateInfo, sizeof(sStateInfo) );

	// HP ���� ����
	sHPInfo hpInfo;
	hpInfo.m_dwNowHP	= siegeObject.GetNowHP();
	hpInfo.m_dwMaxHP	= siegeObject.GetMaxHP();
	COPY_AND_ADVANCE_DST( szDataPos, &hpInfo, sizeof(sHPInfo) );

	// ��ġ ���� ����
	sPosInfo posInfo;
	const Position& pos	= siegeObject.GetPosition();
	const MotionInfo& motionInfo = siegeObject.GetMotionInfo();
	
	/*float fY = pos.m_fPointY;
	if ( siegeObject.IsSiegeArms() && Siege::AIRSHIP != siegeObject.GetObjectType() )
	{
		fY = 0.0f;
	}*/

	posInfo.m_fDefaultDir	= siegeObject.GetDefaultDir();
	posInfo.m_NetworkPos	= CNetworkPos( pos.m_fPointX, pos.m_fPointY, pos.m_fPointZ, motionInfo.m_fDirection,
										   (0 == motionInfo.m_dwFrame) ? 0.0f : motionInfo.m_fVelocity / motionInfo.m_dwFrame );
    COPY_AND_ADVANCE_DST( szDataPos, &posInfo, sizeof(sPosInfo) );

	// ���� ���� ����
	sMaterialInfo materialInfo;
	materialInfo.m_cMaterial	= siegeObject.GetMaterialNum();
	COPY_AND_ADVANCE_DST( szDataPos, &materialInfo, sizeof(sMaterialInfo) );

	// ž���� ���� ����
	sRiderInfo riderInfo;
	std::fill_n(&riderInfo.m_dwRiderID[0], int(Siege::AIRSHIP_RIDER_NUM), 0);
	siegeObject.GetRiders( riderInfo.m_dwRiderID );
	COPY_AND_ADVANCE_DST( szDataPos, &riderInfo, sizeof(sRiderInfo) );

	// ������ ���̸� ���Ѵ�.
	m_wBroadCastDataLen = static_cast<unsigned short>( szDataPos - m_aryBroadCastData );
}

void
BroadCastSiege::CSerializeSiegeObjectData::PrepareDeltaData( const CSiegeObject& siegeObject )
{
	assert( CSiegeObjectData::EstimateBufferSize(0xFFFFFFFF) < MAX_SIEGEOBJECT_DATA_SIZE && "MAX_SIEGEOBJECT_DATA_SIZE �� �÷��ּ���!" );

	// �����͸� ���ؼ� Delta�� ���Ѵ�.    
	m_wDeltaBroadCastDataLen = 0;
	char* szDataPos = m_aryDeltaBroadCastData;

	// UpdateFlag �� ������ ��ġ�� ����صΰ�, ���������� ����Ѵ�.
	unsigned long dwDeltaUpdateFlag = 0;
	char* szUpdateFlagCopyPos = szDataPos;
	szDataPos += sizeof(unsigned long);

	// ������ ���� �� �� ����
	if ( m_LastSiegeObjectData.GetCampID() != siegeObject.GetCampID() ||
		 m_LastSiegeObjectData.GetOwnerCID() != siegeObject.GetOwnerID() ||
		 m_LastSiegeObjectData.GetGID() != siegeObject.GetGID() ||
		 m_LastSiegeObjectData.GetNation() != siegeObject.GetNation() )
	{
		sOwnerShipInfo ownerShipInfo;
		ownerShipInfo.m_dwCampOrCastleID	= siegeObject.GetCampID();
		ownerShipInfo.m_dwOwnerCID			= siegeObject.GetOwnerID();
		ownerShipInfo.m_dwGID				= siegeObject.GetGID();
		ownerShipInfo.m_cNation				= siegeObject.GetNation();

		COPY_AND_ADVANCE_DST( szDataPos, &ownerShipInfo, sizeof(sOwnerShipInfo) );
		dwDeltaUpdateFlag |= DELTA_OWNERSHIP;

		m_LastSiegeObjectData.SetOwnerShipInfo( ownerShipInfo );
	}

	// ���� ���� �� �� ����
	if ( m_LastSiegeObjectData.GetObjectType() != siegeObject.GetObjectType() ||
		 m_LastSiegeObjectData.GetState() != siegeObject.GetState() ||
		 m_LastSiegeObjectData.GetSubState() != siegeObject.GetSubState() ||
		 m_LastSiegeObjectData.GetUpgradeStep() != siegeObject.GetUpgradeStep() ||
		 m_LastSiegeObjectData.GetUpgradeType() != siegeObject.GetUpgradeType() )
	{
		sStateInfo stateInfo;
		stateInfo.m_wObjectType		= siegeObject.GetObjectType();
		stateInfo.m_cState			= siegeObject.GetState();
		stateInfo.m_cSubState		= siegeObject.GetSubState();
		stateInfo.m_cUpgradeStep	= siegeObject.GetUpgradeStep();
		stateInfo.m_cUpgradeType	= siegeObject.GetUpgradeType();

		COPY_AND_ADVANCE_DST( szDataPos, &stateInfo, sizeof(sStateInfo) );
		dwDeltaUpdateFlag |= DELTA_STATE;

		m_LastSiegeObjectData.SetStateInfo( stateInfo );
	}

	// HP ���� �� �� ����
	if ( m_LastSiegeObjectData.GetNowHP() != siegeObject.GetNowHP() ||
		 m_LastSiegeObjectData.GetMaxHP() != siegeObject.GetMaxHP() )
	{
		sHPInfo hpInfo;
		hpInfo.m_dwNowHP	= siegeObject.GetNowHP();
		hpInfo.m_dwMaxHP	= siegeObject.GetMaxHP();
		
		COPY_AND_ADVANCE_DST( szDataPos, &hpInfo, sizeof(sHPInfo) );
		dwDeltaUpdateFlag |= DELTA_HP;

		m_LastSiegeObjectData.SetHPInfo( hpInfo );
	}

	// ��ġ ���� �� �� ����
	const MotionInfo& motionInfo = siegeObject.GetMotionInfo();
	CNetworkPos netPos = CNetworkPos( siegeObject.GetCurrentPos().m_fPointX, siegeObject.GetCurrentPos().m_fPointY,
									  siegeObject.GetCurrentPos().m_fPointZ, motionInfo.m_fDirection,
									  (0 == motionInfo.m_dwFrame) ? 0.0f : motionInfo.m_fVelocity / motionInfo.m_dwFrame );

	// Code by By Minbobo 
	if( (netPos!=m_LastSiegeObjectData.GetNetworkPos()) ||
		(m_LastSiegeObjectData.GetDefaultDir()!=siegeObject.GetDefaultDir()) )
	{		 
		sPosInfo posInfo;
		const Position& pos	= siegeObject.GetPosition();
		float fY = pos.m_fPointY;
		
		if ( siegeObject.IsSiegeArms() && Siege::AIRSHIP != siegeObject.GetObjectType() )
		{
			fY = 0.0f;
		}

		posInfo.m_fDefaultDir	= siegeObject.GetDefaultDir();
		posInfo.m_NetworkPos	= CNetworkPos( pos.m_fPointX, fY, pos.m_fPointZ, motionInfo.m_fDirection,
			(0 == motionInfo.m_dwFrame) ? 0.0f : motionInfo.m_fVelocity / motionInfo.m_dwFrame );

		COPY_AND_ADVANCE_DST( szDataPos, &posInfo, sizeof(sPosInfo) );
		dwDeltaUpdateFlag |= DELTA_POS;

		posInfo.m_NetworkPos = netPos;
		m_LastSiegeObjectData.SetPosInfo( posInfo );
	}

	/*if ( (m_LastSiegeObjectData.GetDefaultDir() - siegeObject.GetDefaultDir()) > FLOAT_EPSILON ||
		 (m_LastSiegeObjectData.GetNetworkPos().GetXPos() - netPos.GetXPos()) > FLOAT_EPSILON ||
		 (m_LastSiegeObjectData.GetNetworkPos().GetYPos() - netPos.GetYPos()) > FLOAT_EPSILON ||
		 (m_LastSiegeObjectData.GetNetworkPos().GetZPos() - netPos.GetZPos()) > FLOAT_EPSILON )
	{
		sPosInfo posInfo;
		const Position& pos	= siegeObject.GetPosition();
		float fY = pos.m_fPointY;
		if ( siegeObject.IsSiegeArms() && Siege::AIRSHIP != siegeObject.GetObjectType() )
		{
			fY = 0.0f;
		}
		posInfo.m_fDefaultDir	= siegeObject.GetDefaultDir();
		posInfo.m_NetworkPos	= CNetworkPos( pos.m_fPointX, fY, pos.m_fPointZ, motionInfo.m_fDirection,
											   (0 == motionInfo.m_dwFrame) ? 0.0f : motionInfo.m_fVelocity / motionInfo.m_dwFrame );

		COPY_AND_ADVANCE_DST( szDataPos, &posInfo, sizeof(sPosInfo) );
		dwDeltaUpdateFlag |= DELTA_POS;

		posInfo.m_NetworkPos = netPos;
		m_LastSiegeObjectData.SetPosInfo( posInfo );
	}*/

	// ���� ���� �� �� ����
	if ( m_LastSiegeObjectData.GetMaterialNum() != siegeObject.GetMaterialNum() )
	{
		sMaterialInfo materialInfo;
		materialInfo.m_cMaterial	= siegeObject.GetMaterialNum();

		COPY_AND_ADVANCE_DST( szDataPos, &materialInfo, sizeof(sMaterialInfo) );
		dwDeltaUpdateFlag |= DELTA_MATERIAL;

		m_LastSiegeObjectData.SetMaterialInfo( materialInfo );
	}

	// ž���� ���� �� �� ����
	unsigned long dwRiders[ Siege::AIRSHIP_RIDER_NUM ];
	std::fill_n(dwRiders, int(Siege::AIRSHIP_RIDER_NUM), 0);
	siegeObject.GetRiders( dwRiders );
	if ( !m_LastSiegeObjectData.IsSameRiders( dwRiders ) )
	{
		sRiderInfo riderInfo;
		std::fill_n(&riderInfo.m_dwRiderID[0], int(Siege::AIRSHIP_RIDER_NUM), 0);
		siegeObject.GetRiders( riderInfo.m_dwRiderID );

		COPY_AND_ADVANCE_DST( szDataPos, &riderInfo, sizeof(sRiderInfo) );
		dwDeltaUpdateFlag |= DELTA_RIDER;

		m_LastSiegeObjectData.SetRiders( riderInfo.m_dwRiderID );
	}

    // ���� �Ϸ� �� ������ ���� ���.
	memcpy( szUpdateFlagCopyPos, &dwDeltaUpdateFlag, sizeof(unsigned long) );
	m_wDeltaBroadCastDataLen = static_cast<unsigned short>( szDataPos - m_aryDeltaBroadCastData);

	// ����Ȱ��� ���ٸ�,
	if ( 0 == dwDeltaUpdateFlag )
	{
		m_wDeltaBroadCastDataLen = 0;
	}
}


