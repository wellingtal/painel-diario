#pragma once

class CGuildInfo ;
class CGuildSafe ;
class CGuildMember ;

class CGuildManager
{
public :
	//======================================
	// * ���Ѽ����� ��ũ ������ ����
	//======================================
	CGuildInfo*			m_pGuildInfo ;
	CGuildSafe*			m_pGuildSafe ;
	CGuildMember*		m_pGuildMember ;

	DWORD		m_dwCresteGuildResult ;
	CHAR		m_szCreateGuildName[ 128 ] ;
	DWORD		m_dwResCheckGuildName ;
	
	static CString		m_strClass[ 25 ] ;
	
public:
	CGuildManager(void);
	~CGuildManager(void);
	
	void		Reflush() ;
	void		UpdateGuildMessageBox() ;
	void		CheckGuildStipulation() ;
	static CString		GetClassName( unsigned short wClass ) ;
};
