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
	m_strClass[ 1 ] = "������" ;			// 451
	m_strClass[ 2 ] = "�α�" ;				// 452
	m_strClass[ 3 ] = "������" ;			// 453
	m_strClass[ 4 ] = "���ݶ���Ʈ" ;		// 454
	m_strClass[ 5 ] = "�����" ;			// 455
	m_strClass[ 6 ] = "������" ;			// 456
	m_strClass[ 7 ] = "����" ;			// 457
	m_strClass[ 8 ] = "��ó" ;				// 458
	m_strClass[ 9 ] = "�Ҽ���" ;			// 459
	m_strClass[ 10 ] = "������" ;			// 460
	m_strClass[ 11 ] = "������Ʈ" ;			// 461
	m_strClass[ 12 ] = "Ŭ����" ;			// 462
	m_strClass[ 13 ] = "" ;					// 463
	m_strClass[ 14 ] = "" ;					// 464
	m_strClass[ 15 ] = "" ;					// 465
	m_strClass[ 16 ] = "" ;					// 466
	m_strClass[ 17 ] = "�Ĺ���Ʈ" ;			// 467
	m_strClass[ 18 ] = "���Ǽ�����" ;		// 468
	m_strClass[ 19 ] = "���÷�" ;			// 469
	m_strClass[ 20 ] = "����Ŀ" ;			// 470
	m_strClass[ 21 ] = "�ų�" ;				// 471
	m_strClass[ 22 ] = "�����" ;			// 472
	m_strClass[ 23 ] = "����������" ;		// 473
	m_strClass[ 24 ] = "���������" ;		// 474

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