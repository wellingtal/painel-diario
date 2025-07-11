
#ifndef __GAMA_MEMORY_MANAGER_H__
#define __GAMA_MEMORY_MANAGER_H__

#pragma once

#include <windows.h>
#include <cstdio>

void TempLogFunction( const char* szFormat, ... ) ;

#if defined( GM_USE_MEMORY )

void*	operator new( size_t size ) ;
void*	operator new[]( size_t size ) ;
void	operator delete( void* pMem ) ;
void	operator delete[]( void* pMem ) ;

// GM_USE_MEMORY ���� new �� ���ϸ�, �Լ���, ���ι�ȣ�� �޴� new �� ȣ���Ѵ�.
// delete �� ���ϸ�, �Լ���, ���ι�ȣ �Լ��� ȣ����� �ʴ´�.
void*	operator new( size_t size, const char* szFile, const char* szFunc, int iLine ) ;
void*	operator new[]( size_t size, const char* szFile, const char* szFunc, int iLine ) ;
void	operator delete( void* pMem, const char* szFile, const char* szFunc, int iLine ) ; 
void	operator delete[]( void* pMem, const char* szFile, const char* szFunc, int iLine ) ;
void*	GMMalloc( size_t size, const char* szFile, const char* szFunc, int iLine ) ;
void*	GMRealloc( void* pMem, size_t size, const char* szFile, const char* szFunc, int iLine ) ;
void	GMFree( void* pMem, const char* szFile, const char* szFunc, int iLine ) ;

#define GM_NEW					new( __FILE__, __FUNCTION__, __LINE__ )
//#define GM_DELETE				delete( __FILE__, __FUNCTION__, __LINE__ )
#define GM_MALLOC( a )			GMMalloc( a, __FILE__, __FUNCTION__, __LINE__ )
#define GM_FREE( a )			GMFree( a, __FILE__, __FUNCTION__, __LINE__ )
#define GM_REALLOC( a, b )		GMRealloc( a, b, __FILE__, __FUNCTION__, __LINE__ )

// delete �� ���� ������ �Ҽ� ����.
#define new						GM_NEW
//#define malloc( a )				GM_MALLOC( a )
//#define realloc( a, b )			GM_REALLOC( a, b )
//#define free( a )				GM_FREE( a )


// ---------------------------------------------------------------------------------- //
//																					  //
// new / delete / malloc / realloc / free �� �޸� ��������� �����ϴ� STATIC Ŭ���� //
//																					  //
// ---------------------------------------------------------------------------------- //

class GMMemoryManager
{
public :
	
	~GMMemoryManager() ;

	static GMMemoryManager* Instance() ;
	
private :

	GMMemoryManager() ;

	// Non exist function
	GMMemoryManager( const GMMemoryManager& mgr ) ;
	GMMemoryManager& operator = ( const GMMemoryManager& mgr ) ;

	bool	Initialize() ;						// �ʱ�ȭ
	void	Shutdown() ;						// ����

	float	GetCurrentTimeInSec() ;				// �ð��� ������ �Լ�

	// ---------------------------------------------------------------
	// For memory manage
public :

	enum EConst
	{
		MEMORY_MULT_KB  = (1024),
		MEMORY_MULT_MB  = (1024 * 1024)
	} ;

	void	DumpMemoryStats() ;					// �޸� ��� ��� ���� ���
	void	DumpMemory() ;						// �޸� �Ҵ� / ���� ���� ���

	void	CallNew() ;							// New ȣ��
	void	CallDelete() ;						// Delete ȣ��

	bool	UseTracker() ;						// �޸� Ʈ��Ŀ ��� ���� Ȯ��
	void	SetUseTracker( bool bUse ) ;		// �޸� Ʈ��Ŀ ��� ���� ����
	void	SetDumpFreedTracker( bool bSet ) ;	// �޸� ���� ���� �α� ���� ����
	void	SetDumpAllocFrees( bool bSet ) ;	// �޸� �Ҵ� / ������ �α� ���� ����

	void	AllocateBytes( unsigned long dwSize ) ;
	void	AllocateBytes( void* pMem, unsigned long dwSize, const char* szFile = 0, const char* szFunc = 0, int iLine = 0 ) ;
	void	DeleteBytes( void* pMem, const char* szFile = 0, const char* szFunc = 0, int iLine = 0 ) ;

	void	Timestamp( const char* szString ) ;					// Timestamp() ��� GM_TIMESTAMP ��ũ�� ���!!

private :
	
	int		m_iCallsNew ;								// �޸� �Ҵ� Ƚ��
	int		m_iCallsDelete ;							// �޸� ���� Ƚ��

	bool	m_bUseTracker ;								// �޸� Ʈ��Ŀ ��� ����
	bool	m_bDumpFreedTracker ;						// �޸� ���� ���� �α� ����
	bool	m_bDumpAllocFrees ;							// �޸� �Ҵ� / ������ ���� ǥ�� ����
	
	unsigned long	m_dwNextID ;						// �޸� �Ҵ�� �ο��Ǵ� ID

	unsigned long	m_dwBytesAllocated ;				// �Ҵ�� �޸� ����Ʈ ��
	unsigned long	m_dwBytesMaxAllocated ;				// �ִ�� �Ҵ�� �޸� ����Ʈ ��
	unsigned long	m_dwBytesMaxChunkAllocated ;		// �ѹ��� �Ҵ�� �޸� ûũ�� �ִ� ����Ʈ ��
	unsigned long	m_dwBytesRunningTotalAllocated ;	// �����߿� �Ҵ�� �޸� ����Ʈ �Ѽ�
	unsigned long	m_dwBytesFreed ;					// ������ �޸� ����Ʈ ��

	float	m_fTimeInit ;								// �ʱ� �ð� ��
	float	m_fTimeMaxAllocated ;						// �ִ�� �Ҵ�� ���� �ð�

	// ---------------------------------------------------------------


	// ---------------------------------------------------------------
	// For memory tracker
public :

	enum ETrackConst
	{
		PATH_LEN				= 64,
		FUNCTION_LEN			= 64,
		TRACKER_ALLOCATE_NUM	= 128				// �⺻ �޸� Ʈ��Ŀ �Ҵ� ��
	} ;

	struct SMemoryTrack
	{
		unsigned long	m_dwID ;					// �޸� Ʈ��Ŀ ID
		void*			m_pMem ;					// �޸� �ּ�
		
		unsigned long	m_dwSize ;					// �Ҵ�� ũ��
		char			m_szFile[ PATH_LEN ] ;		// �޸� �Ҵ��� �� ���ϸ�
		char			m_szFunc[ FUNCTION_LEN ] ;	// �޸� �Ҵ��� �� �Լ���
		int				m_iLine ;					// �޸� �Ҵ��� �� ����

		SMemoryTrack*	m_pPrev ;					// Prev link
		SMemoryTrack*	m_pNext ;					// Next link

		float			m_fTimeNew ;				// �Ҵ�� �ð�
		float			m_fTimeDelete ;				// ������ �ð�

		unsigned long	m_dwBytesAllocated ;		// �ش� �޸𸮰� �Ҵ�� ���������� �Ҵ�� �޸� �ѷ�
	} ;

	void				AllocateTrackerBlock() ;	// �޸� Ʈ��Ŀ���� �Ҵ�
	void				DeleteTrackerBlock() ;		// �޸� Ʈ��Ŀ���� ����

	SMemoryTrack*		GetFreeTracker() ;			// ��� ������ �޸� Ʈ��Ŀ�� ��ȯ�ϴ� �Լ�
	

private :

	int		m_iTrackerAllocated ;					// �޸� Ʈ��Ŀ�� �Ҵ�� ��

	SMemoryTrack*		m_pAvailableTrackerList ;	// ��� ������ �޸� Ʈ��Ŀ ����Ʈ
	SMemoryTrack*		m_pAllocatedTrackerList ;	// �Ҵ�� �޸��� �޸� Ʈ��Ŀ ����Ʈ
	SMemoryTrack*		m_pDeletedTrackerList ;		// ������ �޸� Ʈ��Ŀ ����Ʈ

	// ---------------------------------------------------------------


	// ---------------------------------------------------------------
	// For timestamp
public :

	enum ETimestampConst
	{
		MAX_STRING_LEN			= 256,					// Timestamp ���ڿ� �ִ� ����
		TIMESTAMP_ALLOCATE_NUM	= 32					// �⺻ Timestamp ��� �Ҵ� ��
	} ;

	struct STimestamp
	{
		char			m_szString[ MAX_STRING_LEN ] ;	// Timestamp ���ڿ�
		float			m_fTime ;						// �ð�
		
		STimestamp*		m_pPrev ;						// Prev link
		STimestamp*		m_pNext ;						// Next link
	} ;

	void			AllocateTimestampBlock() ;			// Timestamp node�� �Ҵ�
	void			DeleteTimestampBlock() ;			// Timestamp node�� ����

	STimestamp*		GetFreeTimestamp() ;				// ��� ������ Timestamp node�� ��ȯ�ϴ� �Լ�

private :

	int		m_iTimestampAllocated ;						// Timestamp �� �Ҵ�� ��

	STimestamp*		m_pAvailableTimestampList ;			// ��� ������ Timestamp ����Ʈ
	STimestamp*		m_pTimestampList ;					// �Ҵ�� Timestamp ����Ʈ
	STimestamp*		m_pTimestampEnd ;					// �Ҵ�� Timestamp ����Ʈ ������ ���

	// ---------------------------------------------------------------


	// ---------------------------------------------------------------
	// For log
public :

	enum ELogConst
	{
		MAX_LOG_PATH					= 255,
		MAX_MEMORY_LOG_OUTPUT_LENGTH	= 512,
		MAX_LOG_FILE_SIZE				= 10 * 1024 * 1024
	} ;

	void	Log( const char* szFormat, ... ) ;

private :

	bool	OpenLogFile( const char* szName="RowMemory.log", bool bFlushOnWrite=false, bool bCommitToDisk=false ) ;// was RowMemory hayzohar
	bool	OpenNextLogFile() ;
	void	CloseLogFile() ;

	FILE*	m_pLogFile ;										// �޸� �α� ����
	bool	m_bLogFlushOnWrite ;								// �޸� �α� ���Ͽ� �������� �÷��� ����
	bool	m_bCommitToDisk ;
	char	m_szLogName[ MAX_LOG_PATH ] ;						// �޸� �α� ���ϸ�
	char	m_szLogBuffer[ MAX_MEMORY_LOG_OUTPUT_LENGTH ] ;		// �޸� �α� ����
	size_t	m_WriteSize ;										// ���� �α� ���Ͽ� �� ũ��
	int		m_iLogFileCounter ;									// �α� ���� �ε��� ī����

	// ---------------------------------------------------------------

} ;

#else

#define GM_NEW					new
//#define GM_DELETE				delete
#define GM_MALLOC				malloc
#define GM_FREE					free
#define GM_REALLOC				realloc

#endif	// GM_USE_MEMORY

// Macro define
#if defined( GM_USE_MEMORY )
	#define GM_MEMORY_LOG		GMMemoryManager::Instance()->Log
	#define GM_TIMESTAMP		GMMemoryManager::Instance()->Timestamp
#else
	#define GM_MEMORY_LOG		TempLogFunction
	#define GM_TIMESTAMP( str )
#endif	//	GM_USE_MEMORY

#include "GMMemory.inl"

#endif	// __GAMA_MEMORY_MANAGER_H__
