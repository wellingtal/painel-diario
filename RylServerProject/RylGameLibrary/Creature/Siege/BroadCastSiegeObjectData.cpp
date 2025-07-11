#include "stdafx.h"
#include "BroadCastSiegeObjectData.h"


BroadCastSiege::CSiegeObjectData::CSiegeObjectData() :
	m_dwCID(0), m_dwUpdateDataFlag(0)
{
	memset( &m_OwershipInfo, 0, sizeof( sOwnerShipInfo ) );
	memset( &m_StateInfo, 0, sizeof( sStateInfo ) );
	memset( &m_HPInfo, 0, sizeof( sHPInfo ) );
	memset( &m_PosInfo, 0, sizeof( sPosInfo ) );
	memset( &m_MaterialInfo, 0, sizeof( sMaterialInfo ) );
	memset( &m_RiderInfo, 0, sizeof( sRiderInfo ) );

	m_PosInfo.m_NetworkPos.Initialize( 0, 0, 0, 0, 0 );
}

BroadCastSiege::CSiegeObjectData::~CSiegeObjectData()
{
}


const void
BroadCastSiege::CSiegeObjectData::GetRiders( unsigned long* pRiders ) const
{
	std::copy( &m_RiderInfo.m_dwRiderID[0], &m_RiderInfo.m_dwRiderID[Siege::AIRSHIP_RIDER_NUM ], pRiders );
}

const bool
BroadCastSiege::CSiegeObjectData::IsSameRiders( unsigned long* pRiders ) const
{
	for ( int i=0; i<Siege::AIRSHIP_RIDER_NUM; ++i )
	{
		if ( m_RiderInfo.m_dwRiderID[i] != pRiders[i] )
		{
			return false;
		}
	}

	return true;
}

void
BroadCastSiege::CSiegeObjectData::SetRiders( unsigned long* pRiders )
{
	std::copy( &pRiders[0], &pRiders[ Siege::AIRSHIP_RIDER_NUM ], m_RiderInfo.m_dwRiderID );
}

void
BroadCastSiege::CSiegeObjectData::ClearRiders()
{
	std::fill_n( &m_RiderInfo.m_dwRiderID[0], int( Siege::AIRSHIP_RIDER_NUM ), 0 );
}

// ===================================================================================
// Data ó�� �Լ�
void
BroadCastSiege::CSiegeObjectData::ClearData()
{
	memset( &m_OwershipInfo, 0, sizeof( sOwnerShipInfo ) );
	memset( &m_StateInfo, 0, sizeof( sStateInfo ) );
	memset( &m_HPInfo, 0, sizeof( sHPInfo ) );
	memset( &m_PosInfo, 0, sizeof( sPosInfo ) );
	memset( &m_MaterialInfo, 0, sizeof( sMaterialInfo ) );
	memset( &m_RiderInfo, 0, sizeof( sRiderInfo ) );

	m_PosInfo.m_NetworkPos.Initialize( 0, 0, 0, 0, 0 );
    m_dwCID = m_dwUpdateDataFlag = 0;
}

//! ���ۿ� ���� ���̸� ���ڷ� �޾Ƽ� �����͸� ������Ʈ�Ѵ�.
//! ���Ͻÿ� ���� ���̿�, ����� ���� ���̸� �־� �ش�.
//! ���ϰ��� ����/���� ���θ� �����Ѵ�.
bool
BroadCastSiege::CSiegeObjectData::UpdateData( unsigned long dwCID, const char* szData, int& iBufferSize_InOut )
{
	// ������ ������ ������ ����.
	// UpdateFlag( 4 byte )
	// �⺻ ���� ���� ( 3 * 4 + 1 byte = 13 byte )
	// ���� ���� ( 6 byte )
	// HP ���� ( 2 * 4 byte = 8 byte )
	// ��ġ ���� ( 12 byte )
	// ���� ���� ( 1 byte )
	// ž���� ���� ( 10 * 4 byte = 40 byte )
	// Total -> 84 byte

	unsigned long dwUpdateFlag = 0;

	const char* szDataPos = szData;
	const int   iBufferSize = iBufferSize_InOut;
	if ( iBufferSize < sizeof(unsigned long) )
	{
		return false;
	}

	// Update Flag �о����
	COPY_AND_ADVANCE_SRC( &dwUpdateFlag, szDataPos, sizeof(unsigned long) );

	// �о�� UpdateFlag��, �ʿ��� �������� ���� �߻��Ѵ�. ������ ���� �����ϸ� ������ ��´�.
	int iEstimateBufferSize = EstimateBufferSize( dwUpdateFlag );

	if ( iBufferSize < iEstimateBufferSize )
	{
		// ���� ũ�Ⱑ �߻��� ���̺��� ���� ���� ����.        
		return false;
	}

	// ������ ����. ���� �ٲ�� �״´�!!.
	m_dwCID = dwCID;

	if ( dwUpdateFlag & DELTA_OWNERSHIP )	{ COPY_AND_ADVANCE_SRC( &m_OwershipInfo, szDataPos, sizeof( sOwnerShipInfo ) );	}
	if ( dwUpdateFlag & DELTA_STATE )		{ COPY_AND_ADVANCE_SRC( &m_StateInfo, szDataPos, sizeof( sStateInfo ) );		}
	if ( dwUpdateFlag & DELTA_HP )			{ COPY_AND_ADVANCE_SRC( &m_HPInfo, szDataPos, sizeof( sHPInfo ) );				}
	if ( dwUpdateFlag & DELTA_POS )			{ COPY_AND_ADVANCE_SRC( &m_PosInfo, szDataPos, sizeof( sPosInfo ) );			}
	if ( dwUpdateFlag & DELTA_MATERIAL )	{ COPY_AND_ADVANCE_SRC( &m_MaterialInfo, szDataPos, sizeof( sMaterialInfo ) );	}
	if ( dwUpdateFlag & DELTA_RIDER )		{ COPY_AND_ADVANCE_SRC( &m_RiderInfo, szDataPos, sizeof( sRiderInfo ) );		}

	iBufferSize_InOut = static_cast<int>( szDataPos - szData );
//	SetDataFlag(Broadcast2nd::CHARDATA_CHANGED);
	return true;
}

int
BroadCastSiege::CSiegeObjectData::EstimateBufferSize( unsigned long dwUpdateFlag )
{
	int iEstimateBufferSize = 0;

	if ( dwUpdateFlag & DELTA_OWNERSHIP )	{ iEstimateBufferSize += sizeof( sOwnerShipInfo );	}
	if ( dwUpdateFlag & DELTA_STATE )		{ iEstimateBufferSize += sizeof( sStateInfo );		}
	if ( dwUpdateFlag & DELTA_HP )			{ iEstimateBufferSize += sizeof( sHPInfo );			}
	if ( dwUpdateFlag & DELTA_POS )			{ iEstimateBufferSize += sizeof( sPosInfo );		}
	if ( dwUpdateFlag & DELTA_MATERIAL )	{ iEstimateBufferSize += sizeof( sMaterialInfo );	}
	if ( dwUpdateFlag & DELTA_RIDER )		{ iEstimateBufferSize += sizeof( sRiderInfo );		}

	return iEstimateBufferSize;
}
