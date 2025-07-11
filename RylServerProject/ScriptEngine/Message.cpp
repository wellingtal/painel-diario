
#include "Message.h"

#include <iostream>
#include <stdarg.h>

using namespace std;

void DefaultMessageFunction( const char * ErrMsg )
{
	cout << ErrMsg << endl;
}


MESSAGE_FUNCTION	g_pfuncCompilerMessage = DefaultMessageFunction;

void CompilerMessage( const char * szStr )
{
	(*g_pfuncCompilerMessage)( szStr );
}

void CompilerMessage2( const char * szStr, ... )
{
	va_list args;

	va_start( args, szStr );

	char szBuffer[4096];
	vsprintf( szBuffer, szStr, args );

	CompilerMessage( szBuffer );
}

void SetCompilerMessageFunction( MESSAGE_FUNCTION func )
{
	g_pfuncCompilerMessage = func;
}

void ErrorMessage( int line, const char * Msg )
{
	CompilerMessage2( "Error (line %d) : %s", line, Msg );
	exit(0);
}

void ErrorMessage( const char * Msg )
{
	CompilerMessage2( "Error : %s", Msg );
	exit(0);
}

void ErrorMessage2( int line, const char * Msg, ... )
{
	va_list args;

	va_start( args, Msg );

	char szBuffer[4096];
	vsprintf( szBuffer, Msg, args );
	CompilerMessage2( "Error (line %d) : %s", line, szBuffer );
	exit(0);
}

void ErrorMessage2( const char * Msg, ... )
{
	va_list args;

	va_start( args, Msg );

	char szBuffer[4096];
	vsprintf( szBuffer, Msg, args );
	CompilerMessage2( "Error : %s", szBuffer );
	exit(0);
}

void WarningMessage( int line, const char * Msg )
{
	CompilerMessage2( "Warning (line %d) : %s", line, Msg );
}

void WarningMessage( const char * Msg )
{
	CompilerMessage2( "Warning : %s", Msg );
}

void WarningMessage2( const char * Msg, ... )
{
	va_list args;

	va_start( args, Msg );

	char szBuffer[4096];
	vsprintf( szBuffer, Msg, args );
	CompilerMessage2( "Warning : %s", szBuffer );
}

void WarningMessage2( int line, const char * Msg, ... )
{
	va_list args;

	va_start( args, Msg );

	char szBuffer[4096];
	vsprintf( szBuffer, Msg, args );
	CompilerMessage2( "Warning (line %d) : %s", line, szBuffer );	
}

void ScriptSystemError( const char * Msg )
{
	CompilerMessage2( "Script System Error : %s", Msg );
}
