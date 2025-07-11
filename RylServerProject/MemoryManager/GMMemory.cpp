
#include "GMMemory.h"
#include "GMFilename.h"

void TempLogFunction( const char* szFormat, ... )
{
}

#ifdef GM_USE_MEMORY

GMMemoryManager*	GMMemoryManager::Instance()
{
	static GMMemoryManager ms_this ;
	return ( &ms_this ) ;
}

GMMemoryManager::GMMemoryManager()
{
	Initialize() ;
}

GMMemoryManager::~GMMemoryManager()
{
	Shutdown() ;
}

// ------------------------------------------------------------------------
bool	GMMemoryManager::Initialize()
{
	m_iCallsNew						= 0 ;			// 메모리 할당 횟수
	m_iCallsDelete					= 0 ;			// 메모리 해제 횟수

	m_bUseTracker					= true ;		// 메모리 트래커 사용 여부
	m_bDumpFreedTracker				= false ;		// 메모리 해제 정보 로그 여부
	m_bDumpAllocFrees				= false ;		// 메모리 할당 / 해제시 정보 표시 여부

	m_dwNextID						= 0 ;			// 메모리 할당시 부여되는 ID

	m_dwBytesAllocated				= 0 ;			// 할당된 메모리 바이트 수
	m_dwBytesMaxAllocated			= 0 ;			// 최대로 할당된 메모리 바이트 수
	m_dwBytesMaxChunkAllocated		= 0 ;			// 한번에 할당된 메모리 청크의 최대 바이트 수
	m_dwBytesRunningTotalAllocated	= 0 ;			// 실행중에 할당된 메모리 바이트 총수
	m_dwBytesFreed					= 0 ;			// 해제된 메모리 바이트 수

	m_fTimeInit						= GetCurrentTimeInSec() ;	// 초기 시간 값
	m_fTimeMaxAllocated				= 0.0f ;		// 최대로 할당된 때의 시간

	m_iTrackerAllocated				= 0 ;			// 메모리 트래커가 할당된 수

	m_pAvailableTrackerList			= NULL ;		// 사용 가능한 메모리 트래커 리스트
	m_pAllocatedTrackerList			= NULL ;		// 할당된 메모리의 메모리 트래커 리스트
	m_pDeletedTrackerList			= NULL ;		// 해제된 메모리 트래커 리스트

	m_iTimestampAllocated			= 0 ;			// Timestamp 가 할당된 수

	m_pAvailableTimestampList		= NULL ;		// 사용 가능한 Timestamp 리스트
	m_pTimestampList				= NULL ;		// 할당된 Timestamp 리스트
	m_pTimestampEnd					= NULL ;		// 할당된 Timestamp 리스트 마지막 노드

	m_pLogFile						= NULL ;		// 메모리 로그 파일
	m_bLogFlushOnWrite				= false ;		// 메모리 로그 파일에 쓸때마다 플러쉬 여부
	m_bCommitToDisk					= false ;
    m_szLogName[ 0 ]				= '\0' ;		// 메모리 로그 파일명
	m_szLogBuffer[ 0 ]				= '\0' ;		// 메모리 로그 버퍼
	m_WriteSize						= 0 ;			// 현재 로그 파일에 쓴 크기
	m_iLogFileCounter				= 0 ;			// 로그 파일 인덱스 카운터

	AllocateTrackerBlock() ;
	AllocateTimestampBlock() ;

	CreateDirectory( "MemoryLog", NULL ) ;
	return OpenLogFile( "MemoryLog/RylMemory.log", true, false ) ;
}

// ------------------------------------------------------------------------
void	GMMemoryManager::Shutdown()
{
	GM_MEMORY_LOG( "\n"
				   "##################################### \n"
				   "### Gama Memory Manager Shutting Down... \n"
				   "##################################### \n\n" ) ;

	DumpMemoryStats() ;
	DumpMemory() ;

	DeleteTrackerBlock() ;
	DeleteTimestampBlock() ;

	CloseLogFile() ;
}

// ------------------------------------------------------------------------
float	GMMemoryManager::GetCurrentTimeInSec()
{
	static bool bFirst = true ;
	static LARGE_INTEGER freq ;
	static LARGE_INTEGER initial ;

	if ( bFirst )
	{
		QueryPerformanceFrequency( &freq ) ;
		QueryPerformanceCounter( &initial ) ;
		bFirst = false ;
	}

	LARGE_INTEGER counter ;
	QueryPerformanceCounter( &counter ) ;

	return (float)( (long double)(counter.QuadPart - initial.QuadPart) / (long double)freq.QuadPart ) ;
}

// ------------------------------------------------------------------------
void	GMMemoryManager::DumpMemoryStats()
{
	GM_MEMORY_LOG( "######################################## \n"
				   "### Gama Memory Statistics... \n"
				   "######################################## \n\n"
				   "CALLS : \n" ) ;

	// 메모리 할당 / 해제 호출 횟수
	GM_MEMORY_LOG( "new                                    , %9d\n", m_iCallsNew ) ;
	GM_MEMORY_LOG( "delete                                 , %9d\n\n", m_iCallsDelete ) ;
	GM_MEMORY_LOG( "Allocate tracker                       , %9d\n", m_iTrackerAllocated ) ;
	GM_MEMORY_LOG( "Allocate timestamp                     , %9d\n", m_iTimestampAllocated ) ;

	unsigned long dwBytesAverageChunkSize = 0 ;
	if ( m_iCallsNew > 0 )
	{
		dwBytesAverageChunkSize = m_dwBytesRunningTotalAllocated / m_iCallsNew ;
	}

	// 메모리 할당 / 해제 바이트 정보
	GM_MEMORY_LOG( "\n\nBYTES : \n" ) ;
	GM_MEMORY_LOG( "Currently allocated                    , %9d , (%8d KB)\n",	m_dwBytesAllocated, m_dwBytesAllocated / MEMORY_MULT_KB ) ;
	GM_MEMORY_LOG( "Maximum allocated (high water mark)    , %9d , (%8d KB)\n",	m_dwBytesMaxAllocated, m_dwBytesMaxAllocated / MEMORY_MULT_KB ) ;
	GM_MEMORY_LOG( "Largest chunk allocated                , %9d , (%8d KB)\n",	m_dwBytesMaxChunkAllocated, m_dwBytesMaxChunkAllocated / MEMORY_MULT_KB ) ;
	GM_MEMORY_LOG( "Sum of all allocations                 , %9d , (%8d KB)\n",	m_dwBytesRunningTotalAllocated, m_dwBytesRunningTotalAllocated / MEMORY_MULT_KB ) ;
	GM_MEMORY_LOG( "Sum af all frees                       , %9d , (%8d KB)\n",	m_dwBytesFreed, m_dwBytesFreed / MEMORY_MULT_KB ) ;
	GM_MEMORY_LOG( "Average chunk size                     , %9d , (%8d KB)\n",	dwBytesAverageChunkSize, dwBytesAverageChunkSize / MEMORY_MULT_KB ) ;

	// 타임스탬프 정보
	GM_MEMORY_LOG( "\n\nTIMES (seconds after application init) : \n" ) ;
	GM_MEMORY_LOG( "High water mark                        , %9.5f \n\n", m_fTimeMaxAllocated - m_fTimeInit ) ;

	STimestamp* pStamp = m_pTimestampList ;
	while ( pStamp )
	{
		GM_MEMORY_LOG( "%-39.39s, %9.5f\n", pStamp->m_szString, pStamp->m_fTime ) ;
		pStamp = pStamp->m_pNext ;
	}
}

// ------------------------------------------------------------------------
void	GMMemoryManager::DumpMemory()
{
	if ( m_bUseTracker )
	{
		// 아직 해제되지 않고 할당되어 있는 메모리 정보
		GM_MEMORY_LOG( "\n"
					   "################################################# \n"
					   "### Gama Memory Currently Allocated ... \n"
					   "################################################# \n\n" ) ;

		GM_MEMORY_LOG( "       ID ,    Pointer ,      Size ,    NewTime ,   Lifetime , AllocedAtNew ,   Line ,    Function,    File \n\n" ) ;
		float fCurrentTime = GetCurrentTimeInSec() ;

		SMemoryTrack* pTrack = m_pAllocatedTrackerList ;
		while ( pTrack )
		{
			GM_MEMORY_LOG( "% 9d , 0x%08x , % 9ld , %10.5f , %10.5f ,    % 9ld , % 6d , %s,   %s\n",
				pTrack->m_dwID, reinterpret_cast<unsigned int>(pTrack->m_pMem), pTrack->m_dwSize,
				pTrack->m_fTimeNew - m_fTimeInit, fCurrentTime - pTrack->m_fTimeNew,
				pTrack->m_dwBytesAllocated, pTrack->m_iLine, pTrack->m_szFunc, pTrack->m_szFile ) ;

			pTrack = pTrack->m_pNext ;
		}
		GM_MEMORY_LOG( "\n" ) ;
	}

	if ( m_bUseTracker && m_bDumpFreedTracker )
	{
		// 해제된 메모리 정보
		GM_MEMORY_LOG( "################################### \n"
					   "### Gama Freed Memory ... \n"
					   "################################### \n\n" ) ;

		GM_MEMORY_LOG( "       ID ,    Pointer ,      Size ,    NewTime ,   Lifetime , AllocedAtNew ,   Line ,    Function,    File \n\n" ) ;

		SMemoryTrack* pTrack = m_pDeletedTrackerList ;
		while ( pTrack )
		{
			GM_MEMORY_LOG( "% 9d , 0x%08x , % 9ld , %10.5f , %10.5f ,    % 9ld , % 6d , %s,   %s\n",
				pTrack->m_dwID, reinterpret_cast<unsigned int>(pTrack->m_pMem), pTrack->m_dwSize,
				pTrack->m_fTimeNew - m_fTimeInit, pTrack->m_fTimeDelete - pTrack->m_fTimeNew,
				pTrack->m_dwBytesAllocated, pTrack->m_iLine, pTrack->m_szFunc, pTrack->m_szFile ) ;

			pTrack = pTrack->m_pNext ;
		}
		GM_MEMORY_LOG( "\n");
	}
}

// ------------------------------------------------------------------------
void	GMMemoryManager::CallNew()
{
	++m_iCallsNew ;
}

// ------------------------------------------------------------------------
void	GMMemoryManager::CallDelete()
{
	++m_iCallsDelete ;
}

// ------------------------------------------------------------------------
bool	GMMemoryManager::UseTracker()
{
	return ( m_bUseTracker ) ;
}

// ------------------------------------------------------------------------
void	GMMemoryManager::SetUseTracker( bool bUse )
{
	m_bUseTracker = bUse ;
}

// ------------------------------------------------------------------------
void	GMMemoryManager::SetDumpFreedTracker( bool bSet )
{
	m_bDumpFreedTracker = bSet ;
}

// ------------------------------------------------------------------------
void	GMMemoryManager::SetDumpAllocFrees( bool bSet )
{
	m_bDumpAllocFrees = bSet ;
}

// ------------------------------------------------------------------------
void	GMMemoryManager::AllocateBytes( unsigned long dwSize )
{
	m_dwBytesAllocated += dwSize ;
	m_dwBytesRunningTotalAllocated += dwSize ;

	if ( m_dwBytesAllocated > m_dwBytesMaxAllocated )
	{
		m_dwBytesMaxAllocated = m_dwBytesAllocated ;
		m_fTimeMaxAllocated = GetCurrentTimeInSec() ;
	}

	if ( dwSize > m_dwBytesMaxChunkAllocated )
	{
		m_dwBytesMaxChunkAllocated = dwSize ;
	}
}

// ------------------------------------------------------------------------
void	GMMemoryManager::AllocateBytes( void* pMem, unsigned long dwSize, const char* szFile, const char* szFunc, int iLine )
{
	m_dwBytesAllocated += dwSize ;
	m_dwBytesRunningTotalAllocated += dwSize ;

	float fNewTime = GetCurrentTimeInSec() ;
	if ( m_dwBytesAllocated > m_dwBytesMaxAllocated )
	{
		m_dwBytesMaxAllocated = m_dwBytesAllocated ;
		m_fTimeMaxAllocated = fNewTime ;
	}

	if ( dwSize > m_dwBytesMaxChunkAllocated )
	{
		m_dwBytesMaxChunkAllocated = dwSize ;
	}

	if ( m_bDumpAllocFrees )
	{
		GM_MEMORY_LOG( "Allocate	%9d - %9ld - %6d - %s - %s\n", m_dwNextID, dwSize, iLine, szFunc, szFile ) ;
	}

	SMemoryTrack* pNewTrack = GetFreeTracker() ;
	if ( pNewTrack )
	{
		pNewTrack->m_dwID				= m_dwNextID++ ;
		pNewTrack->m_pMem				= pMem ;
		pNewTrack->m_dwSize				= dwSize ;
		pNewTrack->m_fTimeNew			= fNewTime ;
		pNewTrack->m_dwBytesAllocated	= m_dwBytesAllocated ;

		if ( szFile )
		{
			GMFilename Filename( szFile ) ;
			strncpy( pNewTrack->m_szFile, Filename.GetFilename(), PATH_LEN ) ;
			strncat( pNewTrack->m_szFile, Filename.GetExt(), PATH_LEN ) ;
		}
		else
		{
			pNewTrack->m_szFile[ 0 ] = '\0' ;
		}

		if ( szFunc )
		{
			strncpy( pNewTrack->m_szFunc, szFunc, FUNCTION_LEN ) ;
		}
		else
		{
			pNewTrack->m_szFunc[ 0 ] = '\0' ;
		}

		pNewTrack->m_iLine	= iLine ;
		pNewTrack->m_pNext	= m_pAllocatedTrackerList ;
		pNewTrack->m_pPrev	= NULL ;

		if ( m_pAllocatedTrackerList )
		{
			m_pAllocatedTrackerList->m_pPrev = pNewTrack ;
		}

		m_pAllocatedTrackerList = pNewTrack ;
	}
	else
	{
		assert( !"GMMemoryManager::AllocateBytes() - Get free tracker failed !!" ) ;
	}
}

// ------------------------------------------------------------------------
void	GMMemoryManager::DeleteBytes( void* pMem, const char* szFile, const char* szFunc, int iLine )
{
	assert( NULL != m_pAllocatedTrackerList ) ;

	// 할당된 메모리 트래커 리스트에서 해제할 트래커를 찾는다.
	bool bFound = false ;
	SMemoryTrack* pTrack = m_pAllocatedTrackerList ;
	float fDeleteTime = GetCurrentTimeInSec() ;

	while ( !bFound )
	{
		if ( pTrack->m_pMem == pMem )
		{
			if ( m_bDumpAllocFrees )
			{
				GM_MEMORY_LOG( "Free		%9d - %9ld - %6d - %s - %s\n",
					pTrack->m_dwID, pTrack->m_dwSize, pTrack->m_iLine, pTrack->m_szFunc, pTrack->m_szFile ) ;
			}

			m_dwBytesAllocated -= pTrack->m_dwSize ;
			m_dwBytesFreed += pTrack->m_dwSize ;
			pTrack->m_fTimeDelete = fDeleteTime ;

			// 할당된 메모리 트래커 리스트에서 제거
			if ( pTrack->m_pNext )
			{
				pTrack->m_pNext->m_pPrev = pTrack->m_pPrev ;
			}

			if ( pTrack->m_pPrev )
			{
				pTrack->m_pPrev->m_pNext = pTrack->m_pNext ;
			}

			if ( m_pAllocatedTrackerList == pTrack )
			{
				m_pAllocatedTrackerList = pTrack->m_pNext ;
			}


			// 해제된 메모리 트래커 리스트에 추가
			pTrack->m_pNext = m_pDeletedTrackerList ;
			pTrack->m_pPrev = NULL ;

			if ( m_pDeletedTrackerList )
			{
				m_pDeletedTrackerList->m_pPrev = pTrack ;
			}

			m_pDeletedTrackerList = pTrack ;

			bFound = true ;
		}
		else
		{
			pTrack = pTrack->m_pNext ;
			if ( NULL == pTrack )
			{
				if ( m_bDumpAllocFrees )
				{
					GM_MEMORY_LOG( "Free		UNKNOWN!\n" ) ;
				}

				// 메모리 트래커 체인을 잃어버린 상황..
				// 발생하면 안된다!!
				bFound = true ;
			}
		}
	}
}

// ------------------------------------------------------------------------
void	GMMemoryManager::Timestamp( const char* szString )
{
	if ( NULL != szString )
	{
		STimestamp* pNewStamp = GetFreeTimestamp() ;
		strncpy( pNewStamp->m_szString, szString, MAX_STRING_LEN ) ;
		pNewStamp->m_szString[ MAX_STRING_LEN - 1 ] = '\0' ;
		pNewStamp->m_fTime = GetCurrentTimeInSec() ;

		pNewStamp->m_pPrev = m_pTimestampEnd ;
		pNewStamp->m_pNext = NULL ;

		if ( NULL == m_pTimestampList )
		{
			m_pTimestampList = pNewStamp ;
		}

		if ( m_pTimestampEnd )
		{
			m_pTimestampEnd->m_pNext = pNewStamp ;
		}
		m_pTimestampEnd = pNewStamp ;
	}
}

// ------------------------------------------------------------------------
void	GMMemoryManager::Log( const char* szFormat, ... )
{
	assert( NULL != szFormat ) ;
	assert( NULL != m_pLogFile ) ;

	va_list v_list ;
	va_start( v_list, szFormat ) ;

	_vsnprintf( m_szLogBuffer, MAX_MEMORY_LOG_OUTPUT_LENGTH, szFormat, v_list ) ;
	m_szLogBuffer[ MAX_MEMORY_LOG_OUTPUT_LENGTH - 1 ] = '\0' ;

	va_end( v_list ) ;

	m_WriteSize += fwrite( m_szLogBuffer, sizeof(char), strlen(m_szLogBuffer), m_pLogFile ) ;
	if ( m_bLogFlushOnWrite )
	{
		fflush( m_pLogFile ) ;
	}

	if ( m_WriteSize >= MAX_LOG_FILE_SIZE )
	{
        m_WriteSize = 0 ;
        CloseLogFile() ;
		OpenNextLogFile() ;
	}
}

// ------------------------------------------------------------------------
bool	GMMemoryManager::OpenLogFile( const char* szName, bool bFlushOnWrite, bool bCommitToDisk )
{
	assert( NULL != szName ) ;

	strcpy( m_szLogName, szName ) ;
	m_bLogFlushOnWrite = bFlushOnWrite ;
	m_bCommitToDisk = bCommitToDisk ;

	char szOpenMode[ 5 ] = "wt" ;
	if ( bCommitToDisk )
	{
		strcat( szOpenMode, "c" ) ;
	}

	m_pLogFile = fopen( m_szLogName, szOpenMode ) ;
	if ( NULL == m_pLogFile )
	{
		assert( !"GMMemoryManager::OpenLogFile() - Failed to open file!" ) ;
		return false ;
	}

	char szMsg[ MAX_MEMORY_LOG_OUTPUT_LENGTH ] ;
	SYSTEMTIME sysTime ;

	Log( "/*****************************************************\n" ) ;
	sprintf( szMsg, "* %s - Log File Opened\n", m_szLogName ) ;
	Log( szMsg ) ;
	GetSystemTime( &sysTime ) ;
	sprintf( szMsg, "* %04d/%02d/%02d - %02d:%02d:%02d\n",
			 sysTime.wYear, sysTime.wMonth, sysTime.wDay,
			 sysTime.wHour, sysTime.wMinute, sysTime.wSecond ) ;
	Log( szMsg ) ;
	Log( "*****************************************************/\n" ) ;

	return true ;
}

// ------------------------------------------------------------------------
bool	GMMemoryManager::OpenNextLogFile()
{
	assert( NULL == m_pLogFile ) ;
	
	// 생성할 로그 파일 이름을 만든다.
	char szNextLogName[ MAX_LOG_PATH ] ;
	GMFilename filename( m_szLogName ) ;
//	sprintf( szNextLogName, "%s%s%02d%s", filename.GetDir(), filename.GetFilename(), ++m_iLogFileCounter, filename.GetExt() ) ;
	sprintf( szNextLogName, "%s%02d%s", filename.GetFilename(), ++m_iLogFileCounter, filename.GetExt() ) ;

	char szOpenMode[ 5 ] = "wt" ;
	if ( m_bCommitToDisk )
	{
		strcat( szOpenMode, "c" ) ;
	}

	m_pLogFile = fopen( szNextLogName, szOpenMode ) ;
	if ( NULL == m_pLogFile )
	{
		assert( !"GMMemoryManager::OpenLogFile() - Failed to open file!" ) ;
		return false ;
	}

	char szMsg[ MAX_MEMORY_LOG_OUTPUT_LENGTH ] ;
	SYSTEMTIME sysTime ;

	Log( "/*****************************************************\n" ) ;
	sprintf( szMsg, "* %s - Log File Opened\n", szNextLogName ) ;
	Log( szMsg ) ;
	GetSystemTime( &sysTime ) ;
	sprintf( szMsg, "* %04d/%02d/%02d - %02d:%02d:%02d\n",
			 sysTime.wYear, sysTime.wMonth, sysTime.wDay,
			 sysTime.wHour, sysTime.wMinute, sysTime.wSecond ) ;
	Log( szMsg ) ;
	Log( "*****************************************************/\n" ) ;

	return true ;
}

// ------------------------------------------------------------------------
void	GMMemoryManager::CloseLogFile()
{
	if ( NULL != m_pLogFile )
	{
		char szMsg[ MAX_MEMORY_LOG_OUTPUT_LENGTH ] ;
		SYSTEMTIME sysTime ;

		Log( "\n" ) ;
		Log( "/*****************************************************\n" ) ;
		sprintf( szMsg, "* %s - Log File Closed\n", m_szLogName ) ;
		Log( szMsg ) ;
		GetSystemTime( &sysTime ) ;
		sprintf( szMsg, "* %04d/%02d/%02d - %02d:%02d:%02d\n",
				 sysTime.wYear, sysTime.wMonth, sysTime.wDay,
				 sysTime.wHour, sysTime.wMinute, sysTime.wSecond ) ;
		Log( szMsg ) ;
		Log( "*****************************************************/\n" ) ;

		fclose( m_pLogFile ) ;
	}

	m_pLogFile = NULL ;
}


//-------------------------------
//-------------------------------

#undef new
#undef malloc
#undef realloc
#undef free

//-------------------------------
//-------------------------------

// ------------------------------------------------------------------------
void	GMMemoryManager::AllocateTrackerBlock()
{
	SMemoryTrack* pTracker = NULL ;
	SMemoryTrack* pLastTracker = NULL ;

	pTracker = (SMemoryTrack*)malloc( sizeof(SMemoryTrack) ) ;
	assert( NULL != pTracker ) ;

	pTracker->m_dwID = 0xFFFFFFFF ;
	pTracker->m_pMem = NULL ;
	pTracker->m_dwSize = 0 ;
	pTracker->m_fTimeNew = 0.0f ;
	pTracker->m_fTimeDelete = 0.0f ;
	pTracker->m_dwBytesAllocated = 0 ;
	pTracker->m_iLine = 0 ;
	memset( pTracker->m_szFunc, 0, FUNCTION_LEN ) ;
	memset( pTracker->m_szFile, 0, PATH_LEN ) ;

	pTracker->m_pNext = m_pAvailableTrackerList ;
	if ( m_pAvailableTrackerList )
	{
		m_pAvailableTrackerList->m_pPrev = pTracker ;
	}
	pTracker->m_pPrev = NULL ;

	pLastTracker = pTracker ;

	for ( int i=0; i < TRACKER_ALLOCATE_NUM; ++i )
	{
		pTracker = (SMemoryTrack*)malloc( sizeof(SMemoryTrack) ) ;
		assert( NULL != pTracker ) ;

		pTracker->m_dwID = 0xFFFFFFFF ;
		pTracker->m_pMem = NULL ;
		pTracker->m_dwSize = 0 ;
		pTracker->m_fTimeNew = 0.0f ;
		pTracker->m_fTimeDelete = 0.0f ;
		pTracker->m_dwBytesAllocated = 0 ;
		pTracker->m_iLine = 0 ;
		memset( pTracker->m_szFunc, 0, FUNCTION_LEN ) ;
		memset( pTracker->m_szFile, 0, PATH_LEN ) ;

		pTracker->m_pNext = pLastTracker ;
		if ( pLastTracker )
		{
			pLastTracker->m_pPrev = pTracker ;
		}
		pTracker->m_pPrev = NULL ;

		pLastTracker = pTracker ;
		++m_iTrackerAllocated ;
	}

	m_pAvailableTrackerList = pLastTracker ;
}

// ------------------------------------------------------------------------
void	GMMemoryManager::DeleteTrackerBlock()
{
	SMemoryTrack* pDelete = NULL ;

	while ( m_pAvailableTrackerList )
	{
		pDelete = m_pAvailableTrackerList ;
		m_pAvailableTrackerList = m_pAvailableTrackerList->m_pNext ;
		free( pDelete ) ;
	}

	while ( m_pAllocatedTrackerList )
	{
		pDelete = m_pAllocatedTrackerList ;
		m_pAllocatedTrackerList = m_pAllocatedTrackerList->m_pNext ;
		free( pDelete ) ;
	}

	while ( m_pDeletedTrackerList )
	{
		pDelete = m_pDeletedTrackerList ;
		m_pDeletedTrackerList = m_pDeletedTrackerList->m_pNext ;
		free( pDelete ) ;
	}

	m_pAvailableTrackerList = NULL ;
	m_pAllocatedTrackerList = NULL ;
	m_pDeletedTrackerList = NULL ;
}

// ------------------------------------------------------------------------
void	GMMemoryManager::AllocateTimestampBlock()
{
	STimestamp*	pStamp = NULL ;
	STimestamp*	pLastStamp = NULL ;

	pStamp = (STimestamp*)malloc( sizeof(STimestamp) ) ;
	assert( NULL != pStamp ) ;

	pStamp->m_szString[ 0 ] = '\0' ;
	pStamp->m_fTime = 0.0f ;

	pStamp->m_pNext = m_pAvailableTimestampList ;
	if ( m_pAvailableTimestampList )
	{
		m_pAvailableTimestampList->m_pPrev = pStamp ;
	}
	pStamp->m_pPrev = NULL ;

	pLastStamp = pStamp ;
	for ( int i = 0; i < TIMESTAMP_ALLOCATE_NUM; ++i )
	{
		pStamp = (STimestamp*)malloc( sizeof(STimestamp) ) ;
		assert( NULL != pStamp ) ;

		pStamp->m_szString[ 0 ] = '\0' ;
		pStamp->m_fTime = 0.0f ;

		pStamp->m_pNext = pLastStamp ;
		if ( pLastStamp )
		{
			pLastStamp->m_pPrev = pStamp ;
		}
		pStamp->m_pPrev = NULL ;

		pLastStamp = pStamp ;
		++m_iTimestampAllocated ;
	}

	m_pAvailableTimestampList = pLastStamp ;
}

// ------------------------------------------------------------------------
void	GMMemoryManager::DeleteTimestampBlock()
{
	STimestamp* pStamp = NULL ;

	while ( m_pAvailableTimestampList )
	{
		pStamp = m_pAvailableTimestampList ;
		m_pAvailableTimestampList = m_pAvailableTimestampList->m_pNext ;
		free( pStamp ) ;
	}

	while ( m_pTimestampList )
	{
		pStamp = m_pTimestampList ;
		m_pTimestampList = m_pTimestampList->m_pNext ;
		free( pStamp ) ;
	}

	m_pAvailableTimestampList = NULL ;
	m_pTimestampList = NULL ;
	m_pTimestampEnd = NULL ;
}


//---------------------------------------
//---------------------------------------
//  Overriding global new and delete...

// ------------------------------------------------------------------------
void*	operator new( size_t size )
{
	// GM_USE_MEMORY 를 사용할때 호출되어서는 안된다.
//	GM_MEMORY_LOG( "GMMemoryManager: new called instead of new(__FILE__, __FUNCTION__, __LINE__)\n" ) ;

	GMMemoryManager::Instance()->CallNew() ;
	void* pMem = malloc( size ) ;

	if ( GMMemoryManager::Instance()->UseTracker() )
	{
		GMMemoryManager::Instance()->AllocateBytes( pMem, static_cast<unsigned long>(size) ) ;
	}
	else
	{
		GMMemoryManager::Instance()->AllocateBytes( static_cast<unsigned long>(size) ) ;
	}

	return pMem ;
}

//---------------------------------------------------------------------------
void*	operator new( size_t size, const char* szFile, const char* szFunc, int iLine )
{
	GMMemoryManager::Instance()->CallNew() ;
	void* pMem = malloc( size ) ;

	if ( GMMemoryManager::Instance()->UseTracker() )
	{
		GMMemoryManager::Instance()->AllocateBytes( pMem, static_cast<unsigned long>(size), szFile, szFunc,  iLine ) ;
	}
	else
	{
		GMMemoryManager::Instance()->AllocateBytes( static_cast<unsigned long>(size) ) ;
	}

	return pMem ;
}

//---------------------------------------------------------------------------
void*	operator new[]( size_t size )
{
	// GM_USE_MEMORY 를 사용할때 호출되어서는 안된다.
//	GM_MEMORY_LOG( "GMMemoryManager: new called instead of new(__FILE__, __FUNCTION__, __LINE__)\n" ) ;

	GMMemoryManager::Instance()->CallNew() ;
	void* pMem = malloc( size ) ;

	if ( GMMemoryManager::Instance()->UseTracker() )
	{
		GMMemoryManager::Instance()->AllocateBytes( pMem, static_cast<unsigned long>(size) ) ;
	}
	else
	{
		GMMemoryManager::Instance()->AllocateBytes( static_cast<unsigned long>(size) ) ;
	}

	return pMem ;
}

//---------------------------------------------------------------------------
void*	operator new[]( size_t size, const char* szFile, const char* szFunc, int iLine )
{
	GMMemoryManager::Instance()->CallNew() ;
	void* pMem = malloc( size ) ;

	if ( GMMemoryManager::Instance()->UseTracker() )
	{
		GMMemoryManager::Instance()->AllocateBytes( pMem, static_cast<unsigned long>(size), szFile, szFunc, iLine ) ;
	}
	else
	{
		GMMemoryManager::Instance()->AllocateBytes( static_cast<unsigned long>(size) ) ;
	}

	return pMem ;
}

//---------------------------------------------------------------------------
void	operator delete( void* pMem )
{
	if ( pMem )
	{
		GMMemoryManager::Instance()->CallDelete() ;

		if ( GMMemoryManager::Instance()->UseTracker() )
		{
			GMMemoryManager::Instance()->DeleteBytes( pMem ) ;
		}

		free( pMem ) ;
	}
}

//---------------------------------------------------------------------------
void	operator delete( void* pMem, const char* szFile, const char* szFunc, int iLine )
{
	if ( pMem )
	{
		GMMemoryManager::Instance()->CallDelete() ;

		if ( GMMemoryManager::Instance()->UseTracker() )
		{
			GMMemoryManager::Instance()->DeleteBytes( pMem, szFile, szFunc, iLine ) ;
		}

		free( pMem ) ;
	}
}

//---------------------------------------------------------------------------
void	operator delete[]( void* pMem )
{
	if ( pMem )
	{
		GMMemoryManager::Instance()->CallDelete() ;

		if ( GMMemoryManager::Instance()->UseTracker() )
		{
			GMMemoryManager::Instance()->DeleteBytes( pMem ) ;
		}

		free( pMem ) ;
	}
}

//---------------------------------------------------------------------------
void	operator delete[]( void* pMem, const char* szFile, const char* szFunc, int iLine )
{
	if ( pMem )
	{
		GMMemoryManager::Instance()->CallDelete() ;

		if ( GMMemoryManager::Instance()->UseTracker() )
		{
			GMMemoryManager::Instance()->DeleteBytes( pMem, szFile, szFunc, iLine ) ;
		}

		free( pMem ) ;
	}
}

//---------------------------------------------------------------------------
void*	WrgMalloc( size_t size, const char* szFile, const char* szFunc, int iLine )
{
	GMMemoryManager::Instance()->CallNew() ;

	void* pMem = malloc( size ) ;
	if ( GMMemoryManager::Instance()->UseTracker() )
	{
		GMMemoryManager::Instance()->AllocateBytes( pMem, static_cast<unsigned long>(size), szFile, szFunc, iLine ) ;
	}
	else
	{
		GMMemoryManager::Instance()->AllocateBytes( static_cast<unsigned long>(size) ) ;
	}

	return pMem ;
}

//---------------------------------------------------------------------------
void*	WrgRealloc( void* pMem, size_t size, const char* szFile, const char* szFunc, int iLine )
{
	GMMemoryManager::Instance()->CallNew() ;

	pMem = realloc( pMem, size ) ;
	if ( GMMemoryManager::Instance()->UseTracker() )
	{
		GMMemoryManager::Instance()->AllocateBytes( pMem, static_cast<unsigned long>(size), szFile, szFunc, iLine ) ;
	}
	else
	{
		GMMemoryManager::Instance()->AllocateBytes( static_cast<unsigned long>(size) ) ;
	}

	return pMem ;
}

//---------------------------------------------------------------------------
void	WrgFree( void* pMem, const char* szFile, const char* szFunc, int iLine )
{
	if ( pMem )
	{
		GMMemoryManager::Instance()->CallDelete() ;
		if ( GMMemoryManager::Instance()->UseTracker() )
		{
			GMMemoryManager::Instance()->DeleteBytes( pMem, szFile, szFunc, iLine ) ;
		}

		free( pMem ) ;
	}
}

//---------------------------------------------------------------------------

// delete 는 쉽게 재정의 할수 없다.
#define new					GM_NEW
#define malloc( a )			GM_MALLOC( a )
#define realloc( a, b )		GM_REALLOC( a, b )
#define free( a )			GM_FREE( a )
//-----------------------------------------------------------------------------------------------------

#endif	// GM_USE_MEMORY