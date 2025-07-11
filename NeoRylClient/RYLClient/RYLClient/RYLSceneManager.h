//=================================================================
// Name : RYLSceneManager.h
// Data : 2003. 11. 10
// Desc : Scene Manager
//			- std::vector로 관리
//=================================================================

#ifndef __RYL_SCENEMANAGER_H__
#define __RYL_SCENEMANAGER_H__

#include <windows.h>
#include <vector>

#define		INTRO_SCENE						0
#define		LOGIN_SCENE						1
#define		RACESELECT_SCENE				2	// WORK_LIST 2.2 NationSelectScene 이름 수정
#define		CHARACTERSELECT_SCENE			3
#define		GAME_SCENE						4
#define		LOADING_SCENE					5
#define		END_SCENE						6

class CRYLSceneObject ;

class CRYLSceneManager
{
private :
	std::vector< CRYLSceneObject* >		m_vecScene ;
	unsigned long						m_nShowInterfaceTimerID ;
	INT									m_iSceneID ;


public :
	CRYLSceneManager() ;
	virtual ~CRYLSceneManager() ;

public :
	INT					GetSceneID()			{ return m_iSceneID ; }
	VOID				SetScene( INT iID ) ;
	CRYLSceneObject*	FindScene( INT iID ) ;

	std::vector< CRYLSceneObject* >::iterator		FindSceneIter( CRYLSceneObject* pScene ) ;
	std::vector< CRYLSceneObject* >::iterator		FindSceneIter( INT iID ) ;

	VOID				InsertScene( CRYLSceneObject* pScene ) ;
	VOID				DeleteScene( INT iID ) ;
	VOID				ClearAll() ;

	HRESULT				GetCurrentSceneInitResource() ;
	HRESULT				GetCurrentSceneDeleteResource() ;

// Signton
public :
	static CRYLSceneObject*		s_pCurrentScene ;
	static CRYLSceneManager*	s_pInstance ;
	static CRYLSceneManager*	Instance()			{ return s_pInstance ; } ;
	static CRYLSceneObject*		GetCurrentScene()	{ return s_pCurrentScene ; }
} ;

//inline CRYLSceneManager* GSceneMgrInstance()	{ return CRYLSceneManager::Instance(); };

#endif // __RYL_SCENEMANAGER_H__