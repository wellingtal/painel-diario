//================================================================
//
//	Name : GMObject.h 
//	Desc : Gama Soft UI¹×Texture°ü·Ã Base Class
//  Date : 2003. 09. 16
//
//================================================================
#ifndef __GMOBJECT_H__
#define __GMOBJECT_H__

#include "GMBase.h"
#include "GMString.h"

class CGMObject
{
private :
	INT			m_hInstance ;
	CGMString	m_ClassName ;

public :
	CGMObject()									{ m_hInstance = -1 ; }
	virtual ~CGMObject()						{ m_hInstance = -1 ; } 

	VOID	SetInstance( INT hInstance )		{ m_hInstance = hInstance; } 
	INT		GetInstance()						{ return m_hInstance ;} 

	VOID	SetClassName( LPCTSTR lpClassName ) { m_ClassName = lpClassName ; } 
	LPSTR	GetClassName()						{ return m_ClassName ; } 
} ;

#endif //__GMOBJECT_H__