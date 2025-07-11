//================================================================
//
//	Name : GMFileUtil.h 
//	Desc : Gama Soft File Stream Class
//  Date : 2003. 09. 18
//
//================================================================
#ifndef __GMFILEUTIL_H__
#define __GMFILEUTIL_H__
 
#include "GMBase.h"
#include "GMString.h"

INT		FileAge( LPCTSTR lpFileName ) ;
BOOL	FileExists( LPCTSTR lpFileName ) ;
LPTSTR	ExtractFileExt( LPCTSTR pFileName ) ;
VOID	ExtractFilePath( LPCTSTR pFileName, CGMString& StrDir ) ;


#endif