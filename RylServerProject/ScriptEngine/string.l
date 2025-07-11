%{

/* ------------------------------------------------------------------
		Initial code (copied verbatim to the output file)
   ------------------------------------------------------------------ */
#include <io.h>					// isatty

#define  _LEX_CPP_				// make sure our variables get created
#include "lex.h"
#include "lexsymb.h"
#include "SymbolTable.h"
#include <string>

#ifdef MSVC
#define  isatty _isatty			// for some reason isatty is called _isatty in VC..
#endif

extern "C" int yywrap ();		// the yywrap function is declared by the caller


void EatCComment();
void EatCppComment();

extern CSymbolTable *	g_pSymbolTable;

%}

/* ------------------------------------------------------------------
		Some macros (standard regular expressions)
   ------------------------------------------------------------------ */

LETTER				[a-zA-Z_]
DIGIT				[0-9]
HEXLETTER			[a-fA-F0-9]

HEXVALUE			0[xX]{HEXLETTER}+
INTEGERVALUE		{DIGIT}+
FLOATVALUE			{DIGIT}+[.]{DIGIT}+
BOOLVALUE			true|false
STRINGVALUE			\"[^\"]*\"

ID					{LETTER}({LETTER}|{DIGIT})*

WSPACE				[ \t]+

INCLUDE				"#include"{WSPACE}*{STRINGVALUE}
DEFINEoptPARAM		{WSPACE}*","{WSPACE}*{ID}
DEFINEopt			{WSPACE}*"("{WSPACE}*{ID}{DEFINEoptPARAM}*{WSPACE}*")"
DEFINEtokenstring	{WSPACE}+{ID}
DEFINE				"#define"{WSPACE}+{ID}{DEFINEopt}?{DEFINEtokenstring}?










/* ------------------------------------------------------------------
		The lexer rules
   ------------------------------------------------------------------ */

%%

{INCLUDE}		{ return TOKEN_INCLUDE; }
{DEFINE}		{ return TOKEN_DEFINE; }

{HEXVALUE}		{ strcpy( yylval.szConstant, yytext );
					return TOKEN_INTEGERVALUE; }
{INTEGERVALUE}	{ strcpy( yylval.szConstant, yytext );
					return TOKEN_INTEGERVALUE; }
{FLOATVALUE}	{ strcpy( yylval.szConstant, yytext );
					return TOKEN_FLOATVALUE; }
{BOOLVALUE}		{ strcpy( yylval.szConstant, yytext );
					return TOKEN_BOOLVALUE; }
{STRINGVALUE}	{ strcpy( yylval.szConstant, yytext + 1 );
					yylval.szConstant[ strlen( yylval.szConstant ) - 1 ] = '\0';
					return TOKEN_STRINGVALUE; }

"void"			{ return TOKEN_VOID; }
"int"			{ return TOKEN_INT; }
"float"			{ return TOKEN_FLOAT; }
"bool"			{ return TOKEN_BOOL; }
"string"		{ return TOKEN_STRING; }

"for"			{ return TOKEN_FOR; }
"while"			{ return TOKEN_WHILE; }
"break"			{ return TOKEN_BREAK; }
"if"			{ return TOKEN_IF; }
"else"			{ return TOKEN_ELSE; }
"switch"		{ return TOKEN_SWITCH; }
"case"			{ return TOKEN_CASE; }
"default"		{ return TOKEN_DEFAULT; }
"continue"		{ return TOKEN_CONTINUE; }
"return"		{ return TOKEN_RETURN; }

{ID}			{ strcpy( yylval.szIdentifier, yytext );
					return TOKEN_ID; }

"+"				{ return TOKEN_ADDITION; }
"-"				{ return TOKEN_SUBTRACTION; }
"*"				{ return TOKEN_MULTIPLICATION; }
"/"				{ return TOKEN_DIVISION; }
"%"				{ return TOKEN_REMINDER; }
"="				{ return TOKEN_ASSIGNMENT; }
"+="			{ return TOKEN_COMPOUNDADDITION; }
"-="			{ return TOKEN_COMPOUNDSUBTRACTION; }
"*="			{ return TOKEN_COMPOUNDMULTIPLICATION; }
"/="			{ return TOKEN_COMPOUNDDIVISION; }
"%="			{ return TOKEN_COMPOUNDREMINDER; }



"!"				{ return TOKEN_NOT; }
"<"				{ return TOKEN_LESSTHAN; }
"<="			{ return TOKEN_LESSTHANOREQUAL; }
">"				{ return TOKEN_MORETHAN; }
">="			{ return TOKEN_MORETHANOREQUAL; }
"=="			{ return TOKEN_EQUALITY; }
"!="			{ return TOKEN_NOTEQUAL; }
"++"			{ return TOKEN_INCREMENT; }
"--"			{ return TOKEN_DECREMENT; }
";"				{ return TOKEN_ENDSTATEMENT; }
"("				{ return TOKEN_LEFTPARENTHESIS; }
")"				{ return TOKEN_RIGHTPARENTHESIS; }
"["				{ return TOKEN_LEFTBRACE; }
"]"				{ return TOKEN_RIGHTBRACE; }
"{"				{ return TOKEN_LEFTBRACKET; }
"}"				{ return TOKEN_RIGHTBRACKET; }
","				{ return TOKEN_COMMA; }
":"				{ return TOKEN_COLON; }

"||"			{ return TOKEN_OR; }
"&&"			{ return TOKEN_AND; }


"//"			{ EatCppComment(); }
"/*"			{ EatCComment(); }
\n				{ yylloc.first_line = ++lineno; }
{WSPACE}		{}									//무시해버림

.				{ return TOKEN_ERROR; }				//그 외의 것들은 모두 에러!

%%










/* ------------------------------------------------------------------
		Additional code (again copied verbatim to the output file)
   ------------------------------------------------------------------ */

void EatCppComment()  
{
	char c;

	while( (c = yyinput()) != '\n' && c != 0 );
	
	yylloc.first_line = ++lineno;
}

void EatCComment()
{
	char c, c0 = ' ';

	while( true )
	{
		c = yyinput();

		if( c0 == '*' && c == '/' ) 
			break;

		if( c == '\n' )
			yylloc.first_line = ++lineno;

		c0 = c;
	}
}
