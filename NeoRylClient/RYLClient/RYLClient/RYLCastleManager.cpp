
#include <winsock2.h>

#include <Network/Packet/PacketStruct/ServerInfo.h>
#include <Castle/CastleBlessMgr.h>
#include "RYLCastleManager.h"
#include "RYLCastle.h"
#include "RYLStringTable.h"
#include "RYLNetworkData.h"
#include "GMMemory.h"

CRYLCastleManager*	CRYLCastleManager::s_pInstance = NULL ;

CRYLCastleManager::CRYLCastleManager() 
{
	s_pInstance = this ;
}

CRYLCastleManager::~CRYLCastleManager() 
{
	ClearAllCastle() ;
}

VOID		CRYLCastleManager::ClearAllCastle() 
{
	if ( m_mapCastle.size() == 0 ) return ;

	std::map< DWORD, CRYLCastle* >::iterator		iter = m_mapCastle.begin() ;

	while ( iter != m_mapCastle.end() )
	{
		delete iter->second ;
		++ iter ;
	}

	m_mapCastle.clear() ;
}

HRESULT		CRYLCastleManager::InsertCastle( CRYLCastle* pCastle ) 
{
	std::map< DWORD, CRYLCastle* >::iterator		iter = m_mapCastle.find( pCastle->GetCastleID() ) ;

	if ( iter != m_mapCastle.end() )
	{
		// 성 정보 갱신
		return E_FAIL ;
	}

	m_mapCastle.insert( std::pair< unsigned long, CRYLCastle* >( pCastle->GetCastleID(), pCastle ) ) ;
	
	return S_OK ;	
}

CRYLCastle*		CRYLCastleManager::FindCastle( unsigned long dwCastleID ) 
{
	std::map< DWORD, CRYLCastle* >::iterator		iter = m_mapCastle.find( dwCastleID ) ;


	if ( iter != m_mapCastle.end() )
	{
		m_pCurrentCastle = iter->second ;
		return iter->second ;
	}

	return NULL ;
}

CRYLCastle*		CRYLCastleManager::FindCastleByNameID( unsigned long dwNameID )
{
	if ( dwNameID < Castle::MAX_CASTLE_NAME_NUM )
	{
		CRYLCastle* pCastle = NULL ;
		std::map< DWORD, CRYLCastle* >::iterator		iter = m_mapCastle.begin() ;
		for (; iter != m_mapCastle.end(); ++iter)
		{
			pCastle = iter->second ;
			if ( pCastle && pCastle->GetNameID() == dwNameID )
			{
				return iter->second ;
			}
		}
	}

	return NULL ;
}

CRYLCastle*		CRYLCastleManager::FindCastleInBlessArea( const vector3& vecPos )
{
	CRYLCastle* pCastle = NULL ;
	std::map< DWORD, CRYLCastle* >::iterator		iter = m_mapCastle.begin() ;
	for (; iter != m_mapCastle.end(); ++iter)
	{
		pCastle = iter->second ;
		if ( pCastle )
		{
			float fBlessArea = g_CastleBlessMgr.GetBlessArea( pCastle->GetTotalGainTaxCount(), pCastle->GetUpgradeStep() ) ;
			
			const float fDX = (vecPos.x / 100.0f) - pCastle->GetPos().x ;
			const float fDZ = (vecPos.z / 100.0f) - pCastle->GetPos().z ;
			const float fDistance = sqrtf( (fDX * fDX) + (fDZ * fDZ) ) ;

			if ( fDistance <= fBlessArea )
			{
				return pCastle ;
			}
		}
	}

	return NULL ;
}
