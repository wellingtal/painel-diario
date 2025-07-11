
#ifdef GM_USE_MEMORY

#include <cassert>

//------------------------------------------------------------------------------
inline GMMemoryManager::SMemoryTrack*	GMMemoryManager::GetFreeTracker()
{
	if ( NULL == m_pAvailableTrackerList )
	{
		AllocateTrackerBlock() ;
	}

	assert( NULL != m_pAvailableTrackerList && "Allocate tracker block failed !!" ) ;

	SMemoryTrack*	pTrack = m_pAvailableTrackerList ;
	m_pAvailableTrackerList = m_pAvailableTrackerList->m_pNext ;
	return pTrack ;
}

//------------------------------------------------------------------------------
inline GMMemoryManager::STimestamp*		GMMemoryManager::GetFreeTimestamp()
{
	if ( NULL == m_pAvailableTimestampList )
	{
		AllocateTimestampBlock() ;
	}

	assert( NULL != m_pAvailableTimestampList && "Allocate timestamp block failed !!" ) ;

	STimestamp* pStamp = m_pAvailableTimestampList ;
	m_pAvailableTimestampList = m_pAvailableTimestampList->m_pNext ;
	return pStamp ;
}

//------------------------------------------------------------------------------

#endif	// GM_USE_MEMORY
