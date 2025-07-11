//=================================================================
// Name : RYLStringTable.h
// Data : 2003. 11. 13
// Desc : RYL String Table 
//			- Client���� ���ڿ� �迭
//=================================================================

#ifndef __RYL_STRINGTABLE_H__
#define __RYL_STRINGTABLE_H__

#include <Utility/Setup/GlobalConstants.h>
#include <windows.h>

class CRYLStringTable
{
public :

    enum
    {
        MAX_GAMEGUARD_CODE_LEN = 64
    };

	static unsigned long	m_dwStringCount ;
	static char				m_strString[ 8192 ][ 1024 ] ;
	static unsigned long	m_dwTextSize[ 129 ] ;

	static GameRYL::ServiceNation	m_eInternationalCode;						// ���� �ڵ�
    static GameRYL::ServerType		m_eServerType;								// ���� Ÿ��
    static char             		m_szGameGuardCode[MAX_GAMEGUARD_CODE_LEN];  // ���� ���� �ʱ�ȭ �ڵ�

private :

	CRYLStringTable()	{} ;
	virtual ~CRYLStringTable()  {} ;

public :

	static bool		Load(const char *strScriptFile, const char *strMacroFile) ;

	static char*	MakeWordBreak( const char *strString, unsigned long dwWidth ) ;
	static char*	MakePrintGold( unsigned long dwGold ) ;
	static char*	MakePrintGoldAbleMinus( long lGold ) ;

	static VOID		CreateTextSize() ;
	static INT		GetStringWidth( LPCTSTR lpStr ) ;

    // ���ڿ��� ���̰� dwWidth �̻����� üũ�ؼ� �Ѿ�� �Ѿ �κп� ���ؼ� ... ó���Ѵ�. 
	static char*	CheckStringWidth( LPCTSTR lpStr, unsigned long dwWidth ) ;	

	static DWORD	GetChatColor( DWORD dwCommand ) ;

	static char*	GetJobName( unsigned long dwClass ) ;

	// ���� : strResult �� ũ�Ⱑ strSource ���� ũ�ų� ���ƾ� �Ѵ�!!
	static VOID		EraseToken( char *strResult, char *strSource, char *szToken ) ;
	static VOID		GetFirstStringByToken( char *strResult, char *strSource, char *szToken ) ;
} ;

#endif // __RYL_STRINGTABLE_H__
