
#ifndef _Message_H_
#define _Message_H_

///////////////////////////////////////////////////////////////////////////////////
//

typedef void (*MESSAGE_FUNCTION)( const char * ErrMsg );
typedef void (*MESSAGE_FUNCTION2)( const char *, ... );

void CompilerMessage( const char * );
void CompilerMessage2( const char *, ... );
void SetCompilerMessageFunction( MESSAGE_FUNCTION );
void ErrorMessage( int line, const char * );
void ErrorMessage( const char * );
void ErrorMessage2( int line, const char *, ... );
void ErrorMessage2( const char *, ... );
void WarningMessage( int line, const char * );
void WarningMessage( const char * );
void WarningMessage2( int line, const char *, ... );
void WarningMessage2( const char *, ... );
void ScriptSystemError( const char * );

///////////////////////////////////////////////////////////////////////////////////
//

#endif