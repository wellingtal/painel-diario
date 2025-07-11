
#ifndef __GAMA_FILENAME_H__
#define __GAMA_FILENAME_H__

#pragma once

class GMFilename
{
public:

	GMFilename( const char* szFullPath ) ;

	void	SetDir( const char* szDir ) ;
	void	SetDrive( const char* szDrive ) ;
	void	SetExt( const char* szExt ) ;
	void	SetFilename( const char* szFilename ) ;

	const char*	GetDir() const ;
	const char*	GetDrive() const ;
	const char*	GetExt() const ;
	const char* GetFilename() const ;

	void	GetFullPath( char* pszFullPath ) const ;

private:

	void	SplitPath( const char* pszStr ) ;
	void	MakePath( char* pszStr ) const ;

private:

	char	m_szDir[ _MAX_DIR ] ;
	char	m_szDrive[ _MAX_DRIVE ] ;
	char	m_szExt[ _MAX_EXT ] ;
	char	m_szFilename[ _MAX_FNAME ] ;

} ;


//---------------------------------------------------------------------------
inline void GMFilename::SetDir( const char* szDir )
{
	strcpy( m_szDir, szDir ) ;
}

//---------------------------------------------------------------------------
inline void GMFilename::SetDrive( const char* szDrive )
{
	strcpy( m_szDrive, szDrive ) ;
}

//---------------------------------------------------------------------------
inline void GMFilename::SetExt( const char* szExt )
{
	strcpy( m_szExt, szExt ) ;
}

//---------------------------------------------------------------------------
inline void GMFilename::SetFilename( const char* szFilename )
{
	strcpy( m_szFilename, szFilename ) ;
}

//---------------------------------------------------------------------------
inline const char* GMFilename::GetDir() const
{
	return m_szDir ;
}

//---------------------------------------------------------------------------
inline const char* GMFilename::GetDrive() const
{
	return m_szDrive ;
}

//---------------------------------------------------------------------------
inline const char* GMFilename::GetExt() const
{
	return m_szExt ;
}

//---------------------------------------------------------------------------
inline const char* GMFilename::GetFilename() const
{
	return m_szFilename ;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
inline GMFilename::GMFilename( const char* szFullPath )
{
	SplitPath( szFullPath ) ;
}

//---------------------------------------------------------------------------
inline void GMFilename::GetFullPath( char* pszFullPath ) const
{
	MakePath( pszFullPath ) ;
}

//---------------------------------------------------------------------------
// Wrapper/replacement for _splitpath. The char arrays should be allocated
// prior to calling this function.
// This function _does not_ handle multibyte characters.

inline void GMFilename::SplitPath( const char* pszStr )
{
	_splitpath( pszStr, m_szDrive, m_szDir, m_szFilename, m_szExt ) ;
}

//---------------------------------------------------------------------------
// Wrapper/replacement for _makepath.  pStr should already be allocated prior
// to calling this function

inline void GMFilename::MakePath( char* pszStr ) const
{
	_makepath( pszStr, m_szDrive, m_szDir, m_szFilename, m_szExt ) ;
}

#endif	// __GAMA_FILENAME_H__
