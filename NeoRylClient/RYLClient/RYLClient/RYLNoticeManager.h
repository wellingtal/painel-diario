//=================================================================
// Name : RYLNoticeManager.h
// Data : 2005. 07. 11
// Desc : Notice Manager
//=================================================================
#ifndef __RYL_NOTICE_MANAGER_H__
#define __RYL_NOTICE_MANAGER_H__

#pragma once
#include <list>
#include <map>
#include <string>

#include <d3d8types.h>	// for D3DCOLOR_RGBA

// StringList 타입 정의
typedef std::list<std::string>	StringList ;

namespace NOTICECOLOR
{
	const DWORD	NORMAL	= D3DCOLOR_RGBA( 208, 216, 116, 255 ) ;
	const DWORD	TIP		= D3DCOLOR_RGBA( 129, 255, 129, 255 ) ;
	const DWORD WAR		= D3DCOLOR_RGBA( 209, 217, 117, 255 ) ;
} ;

namespace NOTICETIME
{
	const FLOAT	SHORT	= 5000 ;
	const FLOAT NORMAL	= 10000 ;
	const FLOAT	LONG	= 15000 ;
} ;

#define MAX_NOTICE_LINE	3

class CRYLNoticeManager
{
private:
	CRYLNoticeManager() ;
	virtual ~CRYLNoticeManager() ;

	struct NoticeInfo
	{
		DWORD		m_dwIndex ;		// Notice 순서 index
		DWORD		m_dwColor ;		// 색상
		FLOAT		m_fTime ;		// 보여질 시간
		BOOL		m_bForce ;		// 꼭 보여져야하는 공지인가?
		StringList	m_lstMsg ;		// 공지 메세지 리스트
	} ;

	typedef std::map<DWORD, NoticeInfo>	NoticeTable;

private:
	DWORD	GetLastIndex() const ;
	VOID	RemoveUselessNotice() ;

public:

	VOID	AddNotice( const char* szNotice, DWORD dwColor, FLOAT fTime, BOOL bForce = FALSE ) ;
	VOID	UpdateNotice( FLOAT fDeltaTime ) ;

	BOOL	IsEmpty() const ;
	BOOL	GetNowNotice( StringList& lstNotice, DWORD& dwColor ) const ;

	// Signton
	static CRYLNoticeManager* Instance() ;

private:

	BOOL			m_bNewNotice ;
	NoticeTable		m_NoticeTable ;
};

#endif	//__RYL_NOTICE_MANAGER_H__
