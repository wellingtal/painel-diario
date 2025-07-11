#ifndef _ANTIHACK_H_
#define _ANTIHACK_H_

#define MAX_HACKCOUNT	5

#define HACK0			0
#define HACK1			0
#define HACK2			0

#define XCHG( dwAddress )	(((DWORD) dwAddress) << 16) | (((DWORD) dwAddress) >> 16)

#pragma comment( lib, "version" )

#define DECLARE_ANTIHACK()		CAntiHack g_AntiHack
#define USE_ANTIHACK1()			g_AntiHack.AntiDebug1();
#define USE_ANTIHACK2(Func)		g_AntiHack.AntiDebug2( XCHG( Func ) );
#define USE_ANTIHACK3(Inst)		g_AntiHack.AntiDebug3( Inst );

class CAntiHack
{
private:
	char m_szTmp[ MAX_PATH ];
	BOOL (WINAPI *pCall)( HINSTANCE hInstance );
	BOOL m_bAntiHack[ MAX_HACKCOUNT ];
	BOOL m_bWinNT;

public:
	CAntiHack()
	{
		for( int i=0; i < MAX_HACKCOUNT; i++ )
			m_bAntiHack[ i ] = TRUE;

		OSVERSIONINFOEX osv;

		osv.dwOSVersionInfoSize = sizeof( osv );
		GetVersionEx( (LPOSVERSIONINFO) &osv );
		m_bWinNT = (bool) ( osv.dwPlatformId == VER_PLATFORM_WIN32_NT );

		HMODULE hKernel;

		if( NULL == (hKernel = LoadLibrary( DecryptText( "KDPKAG-+&;BA" ))) ) // LoadLibrary( "KERNEL32.DLL" );
			return;
		BOOL (WINAPI *pIsDebuggerPresent)( void );
		pIsDebuggerPresent = GetProcAddress( (HMODULE) hKernel, DecryptText( "IrBb^pa`]iFgYfW_d" ) ); // GetProcAddress( "IsDebuggerPresent" );
		m_bAntiHack[HACK0] = pIsDebuggerPresent();
		FreeLibrary( hKernel );
	}

	~CAntiHack()
	{
	}

	void AntiDebug1( void )
	{
#if USE_ANTIHACK == 1
		if( m_bAntiHack[HACK1] ) {
			MessageBox( NULL, "Please turn your Debugger off!", "ERROR", MB_OK );
			ExitProcess( 0 );
		}
		m_bAntiHack[ HACK1 ] = (CreateFileA( "\\[,YOD=>", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 ) != INVALID_HANDLE_VALUE ); // \\.\SICE
		m_bAntiHack[ HACK2 ] = (CreateFileA( "\\[,YJOC<=", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 ) != INVALID_HANDLE_VALUE ); // \\.\NTICE
#endif
	}

	void AntiDebug2( DWORD dwAddress )
	{
#if USE_ANTIHACK == 1
		if( m_bAntiHack[ HACK2 ] || m_bAntiHack[ HACK1 ] ) {
			WinExit( EWX_SHUTDOWN | EWX_FORCE );
		}
#endif
		pCall = (BOOL (WINAPI *)( HINSTANCE )) (XCHG( dwAddress ));
	}

	BOOL AntiDebug3( HINSTANCE hInstance )
	{
		return pCall( hInstance );
	}

protected:
	char *DecryptText( char *lpData )
	{
		strcpy( m_szTmp, lpData );
		Decrypt( (unsigned char *) m_szTmp, (DWORD) strlen( m_szTmp ) );
		return m_szTmp;
	}
	bool Encrypt( unsigned char *lpData, DWORD dwLength )
	{
		for( int i=0; i < (int) dwLength; i++ )
			*(lpData + i) -= i;
		return true;
	}
	bool Decrypt( unsigned char *lpData, DWORD dwLength )
	{
		for( int i=0; i < (int) dwLength; i++ )
			*(lpData + i) += i;
		return true;
	}
	BOOL SetPrivilege( unsigned char *name, BOOL bEnable )
	{
		BOOL bResult = FALSE;
		HANDLE hToken;
		DWORD dwRetLen;
		TOKEN_PRIVILEGES tpToken, tpPrev;
		
		OpenProcessToken( GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken );
		tpToken.PrivilegeCount = 1;
		if( LookupPrivilegeValue( NULL, (LPCSTR) name, &(tpToken.Privileges[0].Luid) ) == TRUE ) {
			if( bEnable )
				tpToken.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			else
				tpToken.Privileges[0].Attributes = 0;
			dwRetLen = 0;
			bResult = AdjustTokenPrivileges( hToken, FALSE, &tpToken, sizeof( tpPrev ), &tpPrev, &dwRetLen );
		}
		CloseHandle( hToken );
		return bResult;
	}

	bool WinExit( DWORD dwFlags )
	{
		if( SetPrivilege( (unsigned char *) "SeShutdownPrivilege", TRUE ) ) {
			ExitWindowsEx( dwFlags, 0 );
			SetPrivilege( (unsigned char *) "SeShutdownPrivilege", FALSE );
		}
		return true;
	}
};

#endif
