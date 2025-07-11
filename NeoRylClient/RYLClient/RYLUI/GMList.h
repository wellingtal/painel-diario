//================================================================
//
//	Name : GMList.h 
//	Desc : Gama Soft List 자료구조
//			- 현재로서는 GMList를 사용, 후에 GMKeeper사용 요망.
//  Date : 2003. 09. 17
//
//================================================================
#ifndef __GMLIST_H__
#define __GMLIST_H__

#include "GMBase.h"
#include <malloc.h>

#define MAX_LISTSIZE			134217727

typedef VOID **PTPoINTerList ;

class CGMList
{
private :
	PTPoINTerList	m_pList ;
	INT				m_iCount ;
	INT				m_iCapacity ;

protected :
	VOID	Grow() ;
	INT		Put( INT Index, LPVOID p ) ;
	INT		SetCapacity( INT NewCapacity ) ;
	INT		SetCount( INT NewCount ) ;

public :
	CGMList() ;
	virtual ~CGMList() ;

	VOID	Clear() ;

	INT		Add( LPVOID p ) ;
	VOID	Get( INT Index, VOID **p ) ;

	INT		Delete(INT Index);
	INT		Exchange( INT Index1, INT Index2 ) ;
	INT		First( LPVOID p ) ;
	INT		Last( LPVOID p ) ;
	INT		IndexOf( LPVOID p ) ;
	INT		Insert( INT Index, LPVOID p ) ;
	INT		Move( INT CurIndex, INT NewIndex ) ;
	INT		Remove( LPVOID p ) ;
	VOID	Pack() ;
	INT Count()		{ return m_iCount ; } ;
} ;

VOID GMListBufferMove( VOID **sour, VOID **dest, INT Count ) ;

#endif // __GMLIST_H__