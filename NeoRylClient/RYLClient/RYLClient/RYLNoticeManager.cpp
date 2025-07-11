#include <winsock2.h>
#include "RYLNoticeManager.h"
#include "RYLStringTable.h"
#include "RYLClientMain.h"

#include "SoundMgr.h"
#include "BgmManager.h"

#include <algorithm>
#include "GMMemory.h"


CRYLNoticeManager* CRYLNoticeManager::Instance()
{
	static CRYLNoticeManager ms_this ;
	return &ms_this ;
}

CRYLNoticeManager::CRYLNoticeManager()
: m_bNewNotice(FALSE)
{
}

CRYLNoticeManager::~CRYLNoticeManager()
{
}

VOID	CRYLNoticeManager::AddNotice( const char* szNotice, DWORD dwColor, FLOAT fTime, BOOL bForce )
{
	if (NULL == szNotice)	return;

	CHAR strMessage[ MAX_PATH ] ;
	INT	i, nLen = strlen(szNotice), nCount = 0, nWidth = 0, nStart = 0 ;

	if ( !IsEmpty() && bForce )
	{
		RemoveUselessNotice() ;
	}

	NoticeInfo	noticeInfo ;
	noticeInfo.m_dwIndex	= GetLastIndex() + 1 ;
	noticeInfo.m_dwColor	= dwColor ;
	noticeInfo.m_fTime		= fTime ;
	noticeInfo.m_bForce		= bForce ;
			
	bool bInsert ;
	for ( i = 0 ; i < nLen ; ++i )
	{
		bInsert = false ;
		if ( szNotice[ i ] & 0x80 )
		{
			nWidth += CRYLStringTable::m_dwTextSize[ 128 ] ;
			nCount  += 2 ;
			++i ;

			if ( nWidth >= 471 ) // 483 - 12 하나의 최고 폭
			{
				bInsert = true ;
			}
		} 
		else if ( szNotice[ i ] == '\\' )
		{
			bInsert = true ;
			++i ;	// \\ 이므로 \를 2개 사용한다.
		}
		else
		{
			nWidth += CRYLStringTable::m_dwTextSize[ szNotice[ i ] ] ;
			++nCount ;

			if ( nWidth >= 475 ) // 483 - 8 하나의 최고 폭
			{
				bInsert = true ;
			}
		}

		if ( bInsert )
		{
			if (noticeInfo.m_lstMsg.size() >= MAX_NOTICE_LINE)
			{
				if ( IsEmpty() )
				{
					m_bNewNotice = TRUE ;
				}

				m_NoticeTable.insert( std::make_pair(noticeInfo.m_dwIndex, noticeInfo) ).second ;
				noticeInfo.m_dwIndex = noticeInfo.m_dwIndex + 1 ;
				noticeInfo.m_lstMsg.clear() ;
			}

			memcpy( strMessage, &szNotice[ nStart ], nCount ) ;
			strMessage[ nCount ] = '\0' ;
			noticeInfo.m_lstMsg.push_back( strMessage ) ;

			nWidth = nCount = 0 ;
			nStart  = i + 1 ;
		}
	}

	if ( nStart <= nLen )
	{
		if (noticeInfo.m_lstMsg.size() >= MAX_NOTICE_LINE)
		{
			m_NoticeTable.insert( std::make_pair(noticeInfo.m_dwIndex, noticeInfo) ).second ;
			noticeInfo.m_dwIndex = noticeInfo.m_dwIndex + 1 ;
			noticeInfo.m_lstMsg.clear() ;
		}

		memcpy( strMessage, &szNotice[ nStart ], nCount ) ;
		strMessage[ nCount ] = '\0' ;
		noticeInfo.m_lstMsg.push_back( strMessage ) ;
	}

	if ( !noticeInfo.m_lstMsg.empty() )
	{
		if ( IsEmpty() )
		{
			m_bNewNotice = TRUE ;
		}

		m_NoticeTable.insert( std::make_pair(noticeInfo.m_dwIndex, noticeInfo) ).second ;
	}
}

VOID	CRYLNoticeManager::UpdateNotice( FLOAT fDeltaTime )
{
	if ( IsEmpty() ) return ;

	NoticeTable::iterator itr = m_NoticeTable.begin() ;
	if ( itr == m_NoticeTable.end() ) return ;
	NoticeInfo& Info = itr->second ;

	if ( m_bNewNotice )
	{
		m_bNewNotice = FALSE ;

		switch ( Info.m_dwColor )
		{
			case NOTICECOLOR::NORMAL:
			{
				char Temp[MAX_PATH]; 
				sprintf( Temp, "%s\\Sound\\NoticeNormal.mnd", g_ClientMain.m_strClientPath ) ;
				CSound* pSound; 
				CSoundMgr::_GetInstance()->GetSound( pSound, Temp ); 
				pSound->Play(); 
			}
			break;

			case NOTICECOLOR::WAR:
			{
				char Temp[MAX_PATH]; 
				sprintf( Temp, "%s\\Sound\\NoticeWar.mnd", g_ClientMain.m_strClientPath ) ;
				CSound* pSound; 
				CSoundMgr::_GetInstance()->GetSound( pSound, Temp ); 
				pSound->Play(); 
			}
			break;

			case NOTICECOLOR::TIP:
			{
				// Vincent - 툴팁 효과음 제거
				//char Temp[MAX_PATH]; 
				//sprintf( Temp, "%s\\Sound\\NoticeTip.mnd", g_ClientMain.m_strClientPath ) ;
				//CSound* pSound; 
				//CSoundMgr::_GetInstance()->GetSound( pSound, Temp ); 
				//pSound->Play(); 
			}
			break;
		}
	}

	Info.m_fTime -= fDeltaTime ;
	if ( Info.m_fTime <= 0.0f )
	{
		itr = m_NoticeTable.erase( itr ) ;

		if ( itr != m_NoticeTable.end() )
		{
			switch ( itr->second.m_dwColor )
			{
				case NOTICECOLOR::NORMAL:
				{
					char Temp[MAX_PATH]; 
					sprintf( Temp, "%s\\Sound\\NoticeNormal.mnd", g_ClientMain.m_strClientPath ) ;
					CSound* pSound; 
					CSoundMgr::_GetInstance()->GetSound( pSound, Temp ); 
					pSound->Play(); 
				}
				break;

				case NOTICECOLOR::WAR:
				{
					char Temp[MAX_PATH]; 
					sprintf( Temp, "%s\\Sound\\NoticeWar.mnd", g_ClientMain.m_strClientPath ) ;
					CSound* pSound; 
					CSoundMgr::_GetInstance()->GetSound( pSound, Temp ); 
					pSound->Play(); 
				}
				break;

				case NOTICECOLOR::TIP:
				{
					// Vincent - 툴팁 효과음 제거
					//char Temp[MAX_PATH]; 
					//sprintf( Temp, "%s\\Sound\\NoticeTip.mnd", g_ClientMain.m_strClientPath ) ;
					//CSound* pSound; 
					//CSoundMgr::_GetInstance()->GetSound( pSound, Temp ); 
					//pSound->Play(); 
				}
				break;
			}
		}
	}
}

VOID	CRYLNoticeManager::RemoveUselessNotice()
{
	if ( IsEmpty() ) return ;

	unsigned long dwOldIndex = 0, dwNowIndex = 0 ;
	unsigned long dwColor ;
	NoticeTable::iterator itr = m_NoticeTable.begin() ;

	if ( itr != m_NoticeTable.end() )
	{
		dwOldIndex = dwNowIndex = itr->second.m_dwIndex ;
	}

	while ( itr != m_NoticeTable.end() )
	{
		NoticeInfo& Info = itr->second ;
		if ( FALSE == Info.m_bForce )
		{
			itr = m_NoticeTable.erase( itr ) ;

			if ( Info.m_dwIndex == dwNowIndex )
			{
				if ( itr != m_NoticeTable.end() )
				{
					dwNowIndex = itr->second.m_dwIndex ;
					dwColor = itr->second.m_dwColor ;
				}
			}
		}
		else
		{
			++itr ;
		}
	}

	// 현재 나가야할 공지가 바뀐 경우
	if ( !IsEmpty() && 0 != dwNowIndex && dwOldIndex != dwNowIndex )
	{
		m_bNewNotice = TRUE ;
	}
}

DWORD	CRYLNoticeManager::GetLastIndex() const
{
	if ( IsEmpty() ) return 0 ;

	NoticeTable::const_reverse_iterator itr = m_NoticeTable.rbegin() ;
	const NoticeInfo& lastInfo = itr->second ;
	return lastInfo.m_dwIndex ;
}

BOOL	CRYLNoticeManager::IsEmpty() const
{
	return m_NoticeTable.empty() ;
}

BOOL	CRYLNoticeManager::GetNowNotice( StringList& lstNotice, DWORD& dwColor ) const
{
	if ( IsEmpty() ) return FALSE ;

	NoticeTable::const_iterator itr = m_NoticeTable.begin() ;
	if ( itr == m_NoticeTable.end() )
	{
		return FALSE ;
	}

	lstNotice = itr->second.m_lstMsg ;
	dwColor = itr->second.m_dwColor ;
	return TRUE ;
}
