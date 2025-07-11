#include "StdAfx.h"
#include ".\guildmanager.h"
#include "GuildInfo.h"
#include "GuildSafe.h"
#include "GuildMember.h"
#include "GMMemory.h"

CString		CGuildManager::m_strClass[ 25 ] ;

CGuildManager::CGuildManager(void)
{
	m_strClass[ 0 ] = "None" ;				// 450
	m_strClass[ 1 ] = "파이터" ;			// 451
	m_strClass[ 2 ] = "로그" ;				// 452
	m_strClass[ 3 ] = "메이지" ;			// 453
	m_strClass[ 4 ] = "어콜라이트" ;		// 454
	m_strClass[ 5 ] = "디펜더" ;			// 455
	m_strClass[ 6 ] = "워리어" ;			// 456
	m_strClass[ 7 ] = "어쎄신" ;			// 457
	m_strClass[ 8 ] = "아처" ;				// 458
	m_strClass[ 9 ] = "소서러" ;			// 459
	m_strClass[ 10 ] = "엔찬터" ;			// 460
	m_strClass[ 11 ] = "프리스트" ;			// 461
	m_strClass[ 12 ] = "클레릭" ;			// 462
	m_strClass[ 13 ] = "" ;					// 463
	m_strClass[ 14 ] = "" ;					// 464
	m_strClass[ 15 ] = "" ;					// 465
	m_strClass[ 16 ] = "" ;					// 466
	m_strClass[ 17 ] = "컴배턴트" ;			// 467
	m_strClass[ 18 ] = "오피세이터" ;		// 468
	m_strClass[ 19 ] = "템플러" ;			// 469
	m_strClass[ 20 ] = "어태커" ;			// 470
	m_strClass[ 21 ] = "거너" ;				// 471
	m_strClass[ 22 ] = "룬오프" ;			// 472
	m_strClass[ 23 ] = "라이프오프" ;		// 473
	m_strClass[ 24 ] = "쉐도우오프" ;		// 474

}

CGuildManager::~CGuildManager(void)
{
}

CString		CGuildManager::GetClassName( unsigned short wClass )
{
	if (1 <= wClass && wClass <= 24)
		return m_strClass[ wClass ] ;
	else
		return m_strClass[ 0 ];
}

void		CGuildManager::Reflush() 
{
}

void		CGuildManager::UpdateGuildMessageBox() 
{
}

void		CGuildManager::CheckGuildStipulation() 
{
}