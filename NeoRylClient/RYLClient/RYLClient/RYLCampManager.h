//=================================================================
// Name : RYLCampManager.h
// Data : 2004. 06. 01
// Desc : Camp Manager Class
//=================================================================
#ifndef __RYL_CAMP_MANAGER_H__
#define __RYL_CAMP_MANAGER_H__

#include <map>
#include <windows.h>

class CRYLCamp ;

class CRYLCampManager
{
private : 
	std::map< unsigned long, CRYLCamp* >		m_mapCamp ;			// map::< CampID, CRYLCamp* >

public :
	CRYLCampManager() ;
	virtual ~CRYLCampManager() ;

	VOID		ClearAllCamp() ;

	CRYLCamp*	FindCamp( unsigned long dwCampID ) ;
	HRESULT		InsertCamp( CRYLCamp* pCastle ) ;
	HRESULT		DeleteCamp( unsigned long dwCampID ) ;

	void		HasCampByGID( unsigned long dwGID, bool& bHasDevelopingCamp, bool& bHasCompleteCamp ) ;
	std::map< unsigned long, CRYLCamp* > GetNode()		{ return m_mapCamp ; }

// Signton
public :
	static CRYLCampManager*	s_pInstance ;
	static CRYLCampManager*	Instance()			{ return s_pInstance ; } ;
} ;

#endif //__RYL_CAMP_MANAGER_H__