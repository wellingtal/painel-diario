//================================================================
//
//	Name : GMList.h 
//	Desc : Gama Soft File Stream Class
//  Date : 2003. 09. 18
//
//================================================================
#ifndef __GMLIST_H__
#define __GMLIST_H__

#include "GMObject.h"
#include "GMFileUtil.h"
#include <fcntl.h>

//------------------------------------------------------------------------------
// File Pointer Position
//------------------------------------------------------------------------------
#define		FP_BEGIN		0
#define		FP_CURRENT		1
#define		FP_END			2

//------------------------------------------------------------------------------
// OpenMode
//------------------------------------------------------------------------------
#define		O_LOAD ( O_RDONLY|O_BINARY )
#define		O_SAVE ( O_CREAT|O_RDWR )

class CGMStream : public CGMObject
{
private:
	INT		m_hFileHandle ;
	INT		m_iFilePostion ;

public:
	CGMStream() ;
	virtual ~CGMStream() ;

	virtual INT		Create( LPSTR pFileName, INT OpenMode ) ;
	virtual INT		ReadBuffer( VOID* pbuffer, INT size ) ;
	virtual INT		WriteBuffer( VOID* pbuffer, INT size ) ;
	virtual INT		SeekBuffer( INT Offset, INT Origin ) ;

	INT		GetPostion ();
	INT		GetSize ();
};

class CGMFileStream : public CGMStream
{
private :
	CGMString		m_FileName ;
	CGMString		m_FileFullName ;

public :
	CGMFileStream() ;
	virtual ~CGMFileStream() ;

	virtual INT		Create( LPSTR pFileName, INT OpenMode ) ;

	LPSTR	GetFileName() ;
	LPSTR	GetFileFullName() ;
};

class CGMMemoryStream : public CGMObject
{
private:
	VOID*		m_pMemory ;
	PBYTE		m_pBeginning ;
	PBYTE		m_pCurrent ;
	PBYTE		m_pEnd ;

	HGLOBAL		m_hMem ;
	DWORD		m_dwPostion ;
	INT			m_iSize ;
	INT			m_iCurrentPos ;

public:	
	CGMMemoryStream ();
	virtual ~CGMMemoryStream ();

	VOID		Create() ;
	INT			ReadBuffer( VOID* pbuffer, INT size ) ;
	INT			WriteBuffer( VOID* pbuffer, INT size ) ;
	INT			SeekBuffer( INT Offset, INT Origin ) ;
	INT			GetPostion() ;

	BOOL		SetSize( INT Size ) ;
	INT			GetSize() ;

	VOID*		GetMemory( INT Origin = FP_BEGIN ) ;

	VOID		LoadFromFile( LPSTR pFileName ) ;
	VOID		LoadFormStream( CGMStream* pStream ) ;
	VOID		SaveToFile( LPSTR pFileName ) ;
	VOID		SaveToStream( CGMStream* pStream ) ;
};

#endif