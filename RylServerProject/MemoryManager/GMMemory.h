
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

// GM_USE_MEMORY 사용시 new 를 파일명, 함수명, 라인번호를 받는 new 로 호출한다.
// delete 는 파일명, 함수명, 라인번호 함수가 호출되지 않는다.
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

// delete 는 쉽게 재정의 할수 없다.
#define new						GM_NEW
//#define malloc( a )				GM_MALLOC( a )
//#define realloc( a, b )			GM_REALLOC( a, b )
//#define free( a )				GM_FREE( a )


// ---------------------------------------------------------------------------------- //
//																					  //
// new / delete / malloc / realloc / free 의 메모리 사용정보를 관리하는 STATIC 클래스 //
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

	bool	Initialize() ;						// 초기화
	void	Shutdown() ;						// 종료

	float	GetCurrentTimeInSec() ;				// 시간을 얻어오는 함수

	// ---------------------------------------------------------------
	// For memory manage
public :

	enum EConst
	{
		MEMORY_MULT_KB  = (1024),
		MEMORY_MULT_MB  = (1024 * 1024)
	} ;

	void	DumpMemoryStats() ;					// 메모리 사용 통계 정보 출력
	void	DumpMemory() ;						// 메모리 할당 / 해제 정보 출력

	void	CallNew() ;							// New 호출
	void	CallDelete() ;						// Delete 호출

	bool	UseTracker() ;						// 메모리 트래커 사용 여부 확인
	void	SetUseTracker( bool bUse ) ;		// 메모리 트래커 사용 여부 설정
	void	SetDumpFreedTracker( bool bSet ) ;	// 메모리 해제 정보 로그 여부 설정
	void	SetDumpAllocFrees( bool bSet ) ;	// 메모리 할당 / 해제시 로깅 여부 설정

	void	AllocateBytes( unsigned long dwSize ) ;
	void	AllocateBytes( void* pMem, unsigned long dwSize, const char* szFile = 0, const char* szFunc = 0, int iLine = 0 ) ;
	void	DeleteBytes( void* pMem, const char* szFile = 0, const char* szFunc = 0, int iLine = 0 ) ;

	void	Timestamp( const char* szString ) ;					// Timestamp() 대신 GM_TIMESTAMP 매크로 사용!!

private :
	
	int		m_iCallsNew ;								// 메모리 할당 횟수
	int		m_iCallsDelete ;							// 메모리 해제 횟수

	bool	m_bUseTracker ;								// 메모리 트래커 사용 여부
	bool	m_bDumpFreedTracker ;						// 메모리 해제 정보 로그 여부
	bool	m_bDumpAllocFrees ;							// 메모리 할당 / 해제시 정보 표시 여부
	
	unsigned long	m_dwNextID ;						// 메모리 할당시 부여되는 ID

	unsigned long	m_dwBytesAllocated ;				// 할당된 메모리 바이트 수
	unsigned long	m_dwBytesMaxAllocated ;				// 최대로 할당된 메모리 바이트 수
	unsigned long	m_dwBytesMaxChunkAllocated ;		// 한번에 할당된 메모리 청크의 최대 바이트 수
	unsigned long	m_dwBytesRunningTotalAllocated ;	// 실행중에 할당된 메모리 바이트 총수
	unsigned long	m_dwBytesFreed ;					// 해제된 메모리 바이트 수

	float	m_fTimeInit ;								// 초기 시간 값
	float	m_fTimeMaxAllocated ;						// 최대로 할당된 때의 시간

	// ---------------------------------------------------------------


	// ---------------------------------------------------------------
	// For memory tracker
public :

	enum ETrackConst
	{
		PATH_LEN				= 64,
		FUNCTION_LEN			= 64,
		TRACKER_ALLOCATE_NUM	= 128				// 기본 메모리 트래커 할당 수
	} ;

	struct SMemoryTrack
	{
		unsigned long	m_dwID ;					// 메모리 트래커 ID
		void*			m_pMem ;					// 메모리 주소
		
		unsigned long	m_dwSize ;					// 할당된 크기
		char			m_szFile[ PATH_LEN ] ;		// 메모리 할당을 한 파일명
		char			m_szFunc[ FUNCTION_LEN ] ;	// 메모리 할당을 한 함수명
		int				m_iLine ;					// 메모리 할당을 한 라인

		SMemoryTrack*	m_pPrev ;					// Prev link
		SMemoryTrack*	m_pNext ;					// Next link

		float			m_fTimeNew ;				// 할당된 시간
		float			m_fTimeDelete ;				// 해제된 시간

		unsigned long	m_dwBytesAllocated ;		// 해당 메모리가 할당된 시점에서의 할당된 메모리 총량
	} ;

	void				AllocateTrackerBlock() ;	// 메모리 트래커들을 할당
	void				DeleteTrackerBlock() ;		// 메모리 트래커들을 해제

	SMemoryTrack*		GetFreeTracker() ;			// 사용 가능한 메모리 트래커를 반환하는 함수
	

private :

	int		m_iTrackerAllocated ;					// 메모리 트래커가 할당된 수

	SMemoryTrack*		m_pAvailableTrackerList ;	// 사용 가능한 메모리 트래커 리스트
	SMemoryTrack*		m_pAllocatedTrackerList ;	// 할당된 메모리의 메모리 트래커 리스트
	SMemoryTrack*		m_pDeletedTrackerList ;		// 해제된 메모리 트래커 리스트

	// ---------------------------------------------------------------


	// ---------------------------------------------------------------
	// For timestamp
public :

	enum ETimestampConst
	{
		MAX_STRING_LEN			= 256,					// Timestamp 문자열 최대 길이
		TIMESTAMP_ALLOCATE_NUM	= 32					// 기본 Timestamp 노드 할당 수
	} ;

	struct STimestamp
	{
		char			m_szString[ MAX_STRING_LEN ] ;	// Timestamp 문자열
		float			m_fTime ;						// 시간
		
		STimestamp*		m_pPrev ;						// Prev link
		STimestamp*		m_pNext ;						// Next link
	} ;

	void			AllocateTimestampBlock() ;			// Timestamp node를 할당
	void			DeleteTimestampBlock() ;			// Timestamp node를 해제

	STimestamp*		GetFreeTimestamp() ;				// 사용 가능한 Timestamp node를 반환하는 함수

private :

	int		m_iTimestampAllocated ;						// Timestamp 가 할당된 수

	STimestamp*		m_pAvailableTimestampList ;			// 사용 가능한 Timestamp 리스트
	STimestamp*		m_pTimestampList ;					// 할당된 Timestamp 리스트
	STimestamp*		m_pTimestampEnd ;					// 할당된 Timestamp 리스트 마지막 노드

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

	FILE*	m_pLogFile ;										// 메모리 로그 파일
	bool	m_bLogFlushOnWrite ;								// 메모리 로그 파일에 쓸때마다 플러쉬 여부
	bool	m_bCommitToDisk ;
	char	m_szLogName[ MAX_LOG_PATH ] ;						// 메모리 로그 파일명
	char	m_szLogBuffer[ MAX_MEMORY_LOG_OUTPUT_LENGTH ] ;		// 메모리 로그 버퍼
	size_t	m_WriteSize ;										// 현재 로그 파일에 쓴 크기
	int		m_iLogFileCounter ;									// 로그 파일 인덱스 카운터

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
