#include "RYLSceneManager.h"
#include "RYLSceneObject.h"
#include "GMBase.h"
#include "FrameTimer.h"
#include "SceneManager.h"
#include "GMMemory.h"

#ifdef _DEBUG
#define new DEBUG_NORMALBLOCK
#endif


CRYLSceneManager*	CRYLSceneManager::s_pInstance = NULL ;
CRYLSceneObject*	CRYLSceneManager::s_pCurrentScene = NULL;

//------------------------------------------------------------------------------
//					CRYLSceneManager
//------------------------------------------------------------------------------
CRYLSceneManager::CRYLSceneManager()
{
	m_iSceneID				= INTRO_SCENE ;
	s_pInstance				 = this ;
}

CRYLSceneManager::~CRYLSceneManager()
{
	std::vector< CRYLSceneObject* >::iterator	iter = m_vecScene.begin() ;

	while( iter != m_vecScene.end() )
	{
        GM_DELETE( *iter ) ;
		++ iter ;
	}
	m_vecScene.clear() ;

	s_pCurrentScene = NULL;
}

VOID	CRYLSceneManager::SetScene( INT iID )				
{ 
	// 캐릭터 선택화면에서는 기본으로 토치를 켜준다.
	CSceneManager::m_bCharacterLightOp = true;//( CHARACTERSELECT_SCENE == iID ) ? 1 : 0 ;

	if ( s_pCurrentScene )
		s_pCurrentScene->SceneHide() ;

	s_pCurrentScene = m_vecScene[ iID ] ;
	s_pCurrentScene->SceneShow() ;

	m_iSceneID = iID ;
}

CRYLSceneObject*		CRYLSceneManager::FindScene( INT iID ) 
{
	std::vector< CRYLSceneObject* >::iterator	iter = m_vecScene.begin() ;

	while( iter != m_vecScene.end() )
	{
		if ( ( *iter )->GetSceneID() == iID )
			return ( *iter ) ;

		++ iter ;
	}

	return NULL ;
}

std::vector< CRYLSceneObject* >::iterator		CRYLSceneManager::FindSceneIter( CRYLSceneObject* pScene ) 
{
	std::vector< CRYLSceneObject* >::iterator	iter = m_vecScene.begin() ;

	while( iter != m_vecScene.end() )
	{
		if ( ( *iter ) == pScene )
		{
			return iter ;
		}
		++iter ;
	}

	return m_vecScene.end() ;
}

std::vector< CRYLSceneObject* >::iterator		CRYLSceneManager::FindSceneIter( INT iID ) 
{
	std::vector< CRYLSceneObject* >::iterator	iter = m_vecScene.begin() ;

	while( iter != m_vecScene.end() )
	{
		if ( ( *iter )->GetSceneID() == iID )
		{
			return iter ;
		}
		++iter ;
	}

	return m_vecScene.end() ;
}

VOID	CRYLSceneManager::InsertScene( CRYLSceneObject* pScene ) 
{
	GMASSERT( CRYLSceneManager::s_pInstance != NULL, "CSceneManager::s_pInstance is NULL!") ;
	GMASSERT( pScene != NULL, "pScene is NULL!" ) ;

	std::vector< CRYLSceneObject* >::iterator	iter = FindSceneIter( pScene ) ;

	if ( iter != m_vecScene.end() )
	{
		return ;
	}
	else
	{
		m_vecScene.push_back( pScene ) ;
	}
}

VOID		CRYLSceneManager::DeleteScene( INT iID ) 
{
	std::vector< CRYLSceneObject* >::iterator	iter = FindSceneIter( iID ) ;

	delete *iter ;
	m_vecScene.erase( iter ) ;
}

VOID		CRYLSceneManager::ClearAll() 
{
	s_pCurrentScene = NULL;
	std::vector< CRYLSceneObject* >::iterator	iter = m_vecScene.begin() ;

	while( iter != m_vecScene.end() )
	{
		delete ( *iter ) ;
		++ iter ;
	}

	m_vecScene.clear() ;
}

HRESULT		CRYLSceneManager::GetCurrentSceneInitResource() 
{
	HRESULT		hr = E_FAIL ;

	if ( s_pCurrentScene )
	{
		hr = s_pCurrentScene->InitResourceObject() ;
	}

	return hr ;
}

HRESULT		CRYLSceneManager::GetCurrentSceneDeleteResource() 
{
	HRESULT		hr = E_FAIL ;

	if ( s_pCurrentScene )
	{
		hr = s_pCurrentScene->DeleteResourceObject() ;
	}

	return hr ;
}
