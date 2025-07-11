#include <winsock2.h>
#include "GMBase.h"
#include "RYLCampManager.h"
#include "RYLCamp.h"
#include "GMMemory.h"

CRYLCampManager*	CRYLCampManager::s_pInstance = NULL ;

CRYLCampManager::CRYLCampManager() 
{
	s_pInstance = this ;
}

CRYLCampManager::~CRYLCampManager() 
{
	ClearAllCamp() ;
}

VOID		CRYLCampManager::ClearAllCamp() 
{
	if ( m_mapCamp.size() == 0 ) return ;

	std::map< DWORD, CRYLCamp* >::iterator		iter = m_mapCamp.begin() ;

	while ( iter != m_mapCamp.end() )
	{
		delete iter->second ;
		++ iter ;
	}

	m_mapCamp.clear() ;
}

HRESULT		CRYLCampManager::InsertCamp( CRYLCamp* pCamp ) 
{
	std::map< DWORD, CRYLCamp* >::iterator		iter = m_mapCamp.find( pCamp->GetCampID() ) ;

	if ( iter != m_mapCamp.end() )
	{
		// 진지 정보 갱신
		return E_FAIL ;
	}

	m_mapCamp.insert( std::pair< unsigned long, CRYLCamp* >( pCamp->GetCampID(), pCamp ) ) ;
	
	return S_OK ;	
}

HRESULT		CRYLCampManager::DeleteCamp( unsigned long dwCampID )
{
	std::map< DWORD, CRYLCamp* >::iterator		iter = m_mapCamp.find( dwCampID ) ;

	if ( iter != m_mapCamp.end() )
	{
		GM_DELETE( iter->second ) ;
		m_mapCamp.erase( iter ) ;
		return S_OK ;
	}

	return	E_FAIL ;
}

CRYLCamp*	CRYLCampManager::FindCamp( unsigned long dwCampID ) 
{
	std::map< DWORD, CRYLCamp* >::iterator		iter = m_mapCamp.find( dwCampID ) ;


	if ( iter != m_mapCamp.end() )
	{
		return iter->second ;
	}

	return NULL ;
}

void	CRYLCampManager::HasCampByGID( unsigned long dwGID, bool& bHasDevelopingCamp, bool& bHasCompleteCamp )
{
	bHasDevelopingCamp = bHasCompleteCamp = false ;

	std::map< DWORD, CRYLCamp* >::iterator		iter = m_mapCamp.begin() ;
	while ( iter != m_mapCamp.end() )
	{
		CRYLCamp* pCamp = iter->second ;
		if ( pCamp && pCamp->GetGID() == dwGID )
		{
			if ( Siege::DEVELOPING == pCamp->GetState() )
			{
				bHasDevelopingCamp = true ;
			}
			else
			{
				bHasCompleteCamp = true ;
			}
		}

		++ iter ;
	}
}