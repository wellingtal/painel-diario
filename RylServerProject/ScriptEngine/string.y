%{

/* ------------------------------------------------------------------
   Initial code (copied verbatim to the output file)
   ------------------------------------------------------------------ */

#include <malloc.h>  // _alloca is used by the parser
#include <string>  // strcpy
#include <assert.h>
#include <stdlib.h>
#include "lex.h"     // the lexer
#include "SyntaxTree.h"
#include "SymbolTable.h"
#include "BaseDef.h"
#include "Message.h"

// Some yacc (bison) defines
#define YYDEBUG 1	      // Generate debug code; needed for YYERROR_VERBOSE
#define YYERROR_VERBOSE // Give a more specific parse error message 

// Forward references
void yyerror (char *msg);

extern CSyntaxTree *	g_pSynTree;
extern CSymbolTable *	g_pSymbolTable;

SFuncType	GetFunctionType( int nodeID );
SFuncType	GetFunctionType( int nodeID1, int nodeID2 );
SFuncType	GetFuncCallType( int nodeID );
int			CountArrayElements( int nodeID );
int			GetConstantValue( int nodeID );


%}


















/* ------------------------------------------------------------------
   Yacc declarations
   ------------------------------------------------------------------ */
%union {
   int		symbolID;	// entry from symbol table
   int		nodeID;		// node in the syntax tree
   char 	szIdentifier[4096];
   char		szConstant[4096];
}

%type <symbolID> variable new_variable
%type <nodeID> program statement_list statement compound_statement expression_statement selection_statement
%type <nodeID> cases case_one default iteration_statement for_expression for_init_statement optional_expression
%type <nodeID> jump_statement declaration decl_specifiers  declarator_list init_declarator
%type <nodeID> array_initializer initializer_list argument_declaration_list argument_declaration function_definition
%type <nodeID> constant_expression expression assignment_expression logical_or_expression logical_and_expression
%type <nodeID> equality_expression relational_expression additive_expression multiplicative_expression unary_expression
%type <nodeID> postfix_expression primary_expression
%type <szIdentifier>	function_name
%type <nodeID> function_decl_end

%token <szIdentifier> TOKEN_ID
%token <szConstant> TOKEN_INTEGERVALUE TOKEN_FLOATVALUE TOKEN_BOOLVALUE TOKEN_STRINGVALUE
%token TOKEN_INT TOKEN_FLOAT TOKEN_BOOL TOKEN_STRING TOKEN_VOID
%token TOKEN_FOR TOKEN_WHILE TOKEN_IF TOKEN_ELSE TOKEN_SWITCH TOKEN_CASE 
%token TOKEN_DEFAULT TOKEN_CONTINUE TOKEN_BREAK TOKEN_RETURN
%token TOKEN_ENDSTATEMENT TOKEN_LEFTPARENTHESIS TOKEN_RIGHTPARENTHESIS
%token TOKEN_LEFTBRACE TOKEN_RIGHTBRACE TOKEN_LEFTBRACKET TOKEN_RIGHTBRACKET 
%token TOKEN_ERROR

/*밑에 있을수록 연산자 우선순위가 높음.*/
%left  TOKEN_COMMA TOKEN_COLON
%right TOKEN_ASSIGNMENT TOKEN_COMPOUNDADDITION TOKEN_COMPOUNDSUBTRACTION TOKEN_COMPOUNDMULTIPLICATION TOKEN_COMPOUNDDIVISION TOKEN_COMPOUNDREMINDER
%left  TOKEN_AND TOKEN_OR
%left  TOKEN_LESSTHAN TOKEN_LESSTHANOREQUAL TOKEN_MORETHAN TOKEN_MORETHANOREQUAL TOKEN_EQUALITY TOKEN_NOTEQUAL
%left  TOKEN_ADDITION TOKEN_SUBTRACTION 
%right TOKEN_MULTIPLICATION TOKEN_DIVISION TOKEN_REMINDER
%right TOKEN_NOT
%left  PREFIXINCREMENT PREFIXDECREMENT		/*Context-Dependent Precedence(가짜 심볼)*/
%left  TOKEN_INCREMENT TOKEN_DECREMENT
%right TOKEN_LEFTPARENTHESIS
%left  TOKEN_RIGHTPARENTHESIS

%expect 1						/* shift/reduce conflict: dangling ELSE */
								/*    declaration */

%%















/* ------------------------------------------------------------------
   Yacc grammar rules
   ------------------------------------------------------------------ */

program
		: statement_list									{ assert( g_pSynTree != NULL );
																g_pSynTree->SetRoot( $1 ); }
		;

statement_list
		: statement_list statement							{ $$ = g_pSynTree->Insert( @1.first_line, TYPE_STATEMENT_LIST, $1, $2 ); }
		| /* empty */										{ $$ = g_pSynTree->Insert( @1.first_line, TYPE_EMPTY_STATEMENT ); }
		;

statement
		: compound_statement								{ $$ = $1; }
		| expression_statement								{ $$ = $1; }
		| selection_statement								{ $$ = $1; }
		| iteration_statement								{ $$ = $1; }
		| jump_statement									{ $$ = $1; }
		| error TOKEN_ENDSTATEMENT							{ $$ = g_pSynTree->Insert( @1.first_line, TYPE_ERROR_STATEMENT ); }
		| declaration										{ $$ = $1; }
		| function_definition								{ $$ = $1; }
		;

compound_statement
		: TOKEN_LEFTBRACKET statement_list TOKEN_RIGHTBRACKET
															{ $$ = $2; }
		;

expression_statement
		: TOKEN_ENDSTATEMENT								{ $$ = g_pSynTree->Insert( @1.first_line, TYPE_EMPTY_STATEMENT ); }
		| expression TOKEN_ENDSTATEMENT						{ $$ = g_pSynTree->Insert( @1.first_line, TYPE_EXPRESSION, $1 ); }
		;

selection_statement
		: TOKEN_IF TOKEN_LEFTPARENTHESIS expression TOKEN_RIGHTPARENTHESIS statement
															{ $$ = g_pSynTree->Insert( @1.first_line, TYPE_IF_STATEMENT, $3, $5 ); }
		| TOKEN_IF TOKEN_LEFTPARENTHESIS expression TOKEN_RIGHTPARENTHESIS statement TOKEN_ELSE statement
															{ $$ = g_pSynTree->Insert( @1.first_line, TYPE_IF_ELSE_STATEMENT, $3, $5, $7 ); }
		| TOKEN_SWITCH TOKEN_LEFTPARENTHESIS expression TOKEN_RIGHTPARENTHESIS
			TOKEN_LEFTBRACKET cases default TOKEN_RIGHTBRACKET
															{ $$ = g_pSynTree->Insert( @1.first_line, TYPE_SWITCH_STATEMENT, $3, $6, $7 ); }
		;

cases
		: case_one cases 									{ $$ = g_pSynTree->Insert( @1.first_line, TYPE_SWITCH_CASES, $1, $2 ); }
		| case_one											{ $$ = $1; }
		;

case_one
		: TOKEN_CASE constant_expression TOKEN_COLON statement_list
															{ $$ = g_pSynTree->Insert( @1.first_line, TYPE_CASE_ONE, $2, $4 ); }
		;

default
		: TOKEN_DEFAULT TOKEN_COLON statement_list			{ $$ = g_pSynTree->Insert( @1.first_line, TYPE_DEFAULT, $3 ); }
		|													{ $$ = 0; }
		;

iteration_statement
		: TOKEN_FOR TOKEN_LEFTPARENTHESIS for_expression TOKEN_RIGHTPARENTHESIS statement
															{ $$ = g_pSynTree->Insert( @1.first_line, TYPE_FOR_STATEMENT, $3, $5 ); }
		| TOKEN_WHILE TOKEN_LEFTPARENTHESIS expression TOKEN_RIGHTPARENTHESIS statement
															{ $$ = g_pSynTree->Insert( @1.first_line, TYPE_WHILE_STATEMENT, $3, $5 ); }
		;

for_expression
		: for_init_statement optional_expression TOKEN_ENDSTATEMENT optional_expression
															{ $$ = g_pSynTree->Insert( @1.first_line, TYPE_FOR_EXPRESSION, $1, $2, $4 ); }
		;

for_init_statement
		: expression_statement								{ $$ = $1; }
		| declaration										{ $$ = $1; }
		;

optional_expression
		: expression										{ $$ = $1; }
		|													{ $$ = 0; }
		;

jump_statement
		: TOKEN_BREAK TOKEN_ENDSTATEMENT					{ $$ = g_pSynTree->Insert( @1.first_line, TYPE_BREAK_STATEMENT ); }
		| TOKEN_CONTINUE TOKEN_ENDSTATEMENT					{ $$ = g_pSynTree->Insert( @1.first_line, TYPE_CONTINUE_STATEMENT ); }
		| TOKEN_RETURN optional_expression TOKEN_ENDSTATEMENT
															{ $$ = g_pSynTree->Insert( @1.first_line, TYPE_RETURN_STATEMENT, $2 ); }
		;

declaration
		: decl_specifiers declarator_list TOKEN_ENDSTATEMENT
															{ $$ = g_pSynTree->Insert( @1.first_line, TYPE_DECLARATION, $1, $2 ); 
																g_pSymbolTable->SetCurrentType( T_VOID ); }
		;

decl_specifiers
		: TOKEN_VOID										{ $$ = g_pSynTree->Insert( @1.first_line, TYPE_SPECIFIER_VOID ); }
		| TOKEN_INT											{ g_pSymbolTable->SetCurrentType( T_INT );
																$$ = g_pSynTree->Insert( @1.first_line, TYPE_SPECIFIER_INT ); }
		| TOKEN_FLOAT										{ g_pSymbolTable->SetCurrentType( T_FLOAT );
																$$ = g_pSynTree->Insert( @1.first_line, TYPE_SPECIFIER_FLOAT ); }
		| TOKEN_BOOL										{ g_pSymbolTable->SetCurrentType( T_BOOL );
																$$ = g_pSynTree->Insert( @1.first_line, TYPE_SPECIFIER_BOOL ); }
		| TOKEN_STRING										{ g_pSymbolTable->SetCurrentType( T_STRING );
																$$ = g_pSynTree->Insert( @1.first_line, TYPE_SPECIFIER_STRING ); }
		;

variable
		: TOKEN_ID											{ 
																$$ = g_pSymbolTable->FindVar( $1 ); 
																if( $$ == 0 )
																	ErrorMessage2( @1.first_line, "undefined symbol : %s", $1 );
															}
		;

new_variable
		: TOKEN_ID											{
																$$ = g_pSymbolTable->AddVar( $1 ); 
																if( $$ == 0 )
																	ErrorMessage2( @1.first_line, "이미 정의 되어 있는 심볼입니다. : %s", $1 );
															}
		;

declarator_list
		: init_declarator									{ $$ = $1; }
		| declarator_list TOKEN_COMMA init_declarator		{ $$ = g_pSynTree->Insert( @1.first_line, TYPE_DECLARATOR_LIST, $1, $3 ); }
		;

init_declarator
		: new_variable TOKEN_ASSIGNMENT assignment_expression { $$ = g_pSynTree->Insert( @1.first_line, $1, TYPE_INIT_DECLARATION, $3 ); }
		| new_variable										{ $$ = g_pSynTree->Insert( @1.first_line, $1, TYPE_NORMAL_DECLARATION ); }
		| TOKEN_ID TOKEN_LEFTBRACE TOKEN_RIGHTBRACE array_initializer				
															{ $$ = g_pSynTree->Insert( @1.first_line, g_pSymbolTable->AddArrVar( $1, CountArrayElements( $4 ) ), TYPE_ARRAY_INITIALIZE, $4 ); }
		| TOKEN_ID TOKEN_LEFTBRACE constant_expression TOKEN_RIGHTBRACE array_initializer
															{ $$ = g_pSynTree->Insert( @1.first_line, g_pSymbolTable->AddArrVar( $1, GetConstantValue( $3 ) ), TYPE_ARRAY_INITIALIZE2, $3, $5 ); }
		| TOKEN_ID TOKEN_LEFTBRACE constant_expression TOKEN_RIGHTBRACE
															{ $$ = g_pSynTree->Insert( @1.first_line, g_pSymbolTable->AddArrVar( $1, GetConstantValue( $3 ) ), TYPE_ARRAY_DECLARATION, $3 ); }
		;

array_initializer
		: TOKEN_ASSIGNMENT TOKEN_LEFTBRACKET initializer_list TOKEN_RIGHTBRACKET
															{ $$ = $3; }
		;

initializer_list
		: assignment_expression								{ $$ = $1; }
		| assignment_expression TOKEN_COMMA initializer_list
															{ $$ = g_pSynTree->Insert( @1.first_line, TYPE_INITIALIZER_LIST, $1, $3 ); }
		;

argument_declaration_list
		: argument_declaration								{ $$ = $1; }
		| argument_declaration TOKEN_COMMA argument_declaration_list
															{ $$ = g_pSynTree->Insert( @1.first_line, TYPE_ARGUMENT_DECLARATION_LIST, $1, $3 ); }
		|													{ $$ = 0; }
		;

argument_declaration
		: decl_specifiers new_variable						{ $$ = g_pSynTree->Insert( @1.first_line, $2, TYPE_ARGUMENT_DECLARATION, $1 ); }
		| decl_specifiers									{ $$ = $1; }
		;

function_name
		: TOKEN_ID											{ strcpy( $$, $1 ); 
																g_pSymbolTable->BeginLocalNameSpace();	}
		;

function_decl_end
		: TOKEN_ENDSTATEMENT								{ g_pSymbolTable->EndLocalNameSpace( 0 ); }
		;

function_def_start
		: TOKEN_LEFTBRACKET									{ g_pSymbolTable->EndArgument(); }						
		;

function_definition
		: decl_specifiers function_name TOKEN_LEFTPARENTHESIS argument_declaration_list TOKEN_RIGHTPARENTHESIS function_def_start statement_list TOKEN_RIGHTBRACKET
															{ 
																int symbID = g_pSymbolTable->AddFunc( true, $2, GetFunctionType( $1, $4 ) );
																if( symbID == 0 )
																	ErrorMessage( @1.first_line, "같은 함수를 중복하여 정의했습니다." );
																$$ = g_pSynTree->Insert( @1.first_line, symbID, TYPE_FUNCTION_DEFINITION, $4, $7 );
																	g_pSymbolTable->SetCurrentType( T_VOID );
																g_pSymbolTable->EndLocalNameSpace( symbID );
															}
		| decl_specifiers function_name TOKEN_LEFTPARENTHESIS argument_declaration_list TOKEN_RIGHTPARENTHESIS function_decl_end
															{ $$ = g_pSynTree->Insert( @1.first_line, g_pSymbolTable->AddFunc( false, $2, GetFunctionType( $1, $4 ) ), TYPE_FUNCTION_DECLARATION, $4 );
																	g_pSymbolTable->SetCurrentType( T_VOID ); }
		;

constant_expression
		: TOKEN_INTEGERVALUE								{ $$ = g_pSynTree->Insert( @1.first_line, g_pSymbolTable->AddConst( $1, T_INT ), TYPE_CONSTANT_EXPRESSION ); }
		| TOKEN_FLOATVALUE									{ $$ = g_pSynTree->Insert( @1.first_line, g_pSymbolTable->AddConst( $1, T_FLOAT ), TYPE_CONSTANT_EXPRESSION ); }
		| TOKEN_BOOLVALUE									{ $$ = g_pSynTree->Insert( @1.first_line, g_pSymbolTable->AddConst( $1, T_BOOL ), TYPE_CONSTANT_EXPRESSION ); }
		| TOKEN_STRINGVALUE									{ $$ = g_pSynTree->Insert( @1.first_line, g_pSymbolTable->AddConst( $1, T_STRING ), TYPE_CONSTANT_EXPRESSION ); }
		;

expression
		: assignment_expression								{ $$ = $1; }
		| assignment_expression TOKEN_COMMA expression		{ $$ = g_pSynTree->Insert( @1.first_line, TYPE_EXPRESSION_LIST, $1, $3 ); }
		;

assignment_expression
		: logical_or_expression								{ $$ = $1; }
		| variable TOKEN_ASSIGNMENT assignment_expression
															{ $$ = g_pSynTree->Insert( @1.first_line, $1, TYPE_ASSIGNMENT_EXPRESSION, $3 ); }
		| variable TOKEN_COMPOUNDADDITION assignment_expression
															{ $$ = g_pSynTree->Insert( @1.first_line, $1, TYPE_COMPOUND_ADDITION, $3 ); }
		| variable TOKEN_COMPOUNDSUBTRACTION assignment_expression
															{ $$ = g_pSynTree->Insert( @1.first_line, $1, TYPE_COMPOUND_SUBTRACTION, $3 ); }
		| variable TOKEN_COMPOUNDMULTIPLICATION assignment_expression
															{ $$ = g_pSynTree->Insert( @1.first_line, $1, TYPE_COMPOUND_MULTIPLICATION, $3 ); }
		| variable TOKEN_COMPOUNDDIVISION assignment_expression
															{ $$ = g_pSynTree->Insert( @1.first_line, $1, TYPE_COMPOUND_DIVISION, $3 ); }
		| variable TOKEN_COMPOUNDREMINDER assignment_expression
															{ $$ = g_pSynTree->Insert( @1.first_line, $1, TYPE_COMPOUND_REMINDER, $3 ); }
		;

logical_or_expression
		: logical_and_expression							{ $$ = $1; }
		| logical_or_expression TOKEN_OR logical_and_expression		
															{ $$ = g_pSynTree->Insert( @1.first_line, TYPE_OR_EXPRESSION, $1, $3 ); }
		;

logical_and_expression
		: equality_expression								{ $$ = $1; }
		| logical_and_expression TOKEN_AND equality_expression
															{ $$ = g_pSynTree->Insert( @1.first_line, TYPE_AND_EXPRESSION, $1, $3 ); }
		;

equality_expression
		: relational_expression								{ $$ = $1; }
		| equality_expression TOKEN_EQUALITY relational_expression
															{ $$ = g_pSynTree->Insert( @1.first_line, TYPE_EQUALITY_EXPRESSION, $1, $3 ); }
		| equality_expression TOKEN_NOTEQUAL relational_expression
															{ $$ = g_pSynTree->Insert( @1.first_line, TYPE_NOTEQAUL_EXPRESSION, $1, $3 ); }
		;

relational_expression
		: additive_expression								{ $$ = $1; }
		| relational_expression TOKEN_LESSTHAN additive_expression
															{ $$ = g_pSynTree->Insert( @1.first_line, TYPE_LESSTHAN_EXPRESSION, $1, $3 ); }
		| relational_expression TOKEN_MORETHAN additive_expression
															{ $$ = g_pSynTree->Insert( @1.first_line, TYPE_MORETHAN_EXPRESSION, $1, $3 ); }
		| relational_expression TOKEN_LESSTHANOREQUAL additive_expression
															{ $$ = g_pSynTree->Insert( @1.first_line, TYPE_LESSTHANOREQUAL_EXPRESSION, $1, $3 ); }
		| relational_expression TOKEN_MORETHANOREQUAL additive_expression
															{ $$ = g_pSynTree->Insert( @1.first_line, TYPE_MORETHANOREQUAL_EXPRESSION, $1, $3 ); }
		;

additive_expression
		: multiplicative_expression							{ $$ = $1; }
		| additive_expression TOKEN_ADDITION multiplicative_expression
															{ $$ = g_pSynTree->Insert( @1.first_line, TYPE_ADDITION_EXPRESSION, $1, $3 ); }
		| additive_expression TOKEN_SUBTRACTION multiplicative_expression
															{ $$ = g_pSynTree->Insert( @1.first_line, TYPE_SUBTRACTION_EXPRESSION, $1, $3 ); }
		;

multiplicative_expression
		: unary_expression									{ $$ = $1; }
		| multiplicative_expression TOKEN_MULTIPLICATION unary_expression
															{ $$ = g_pSynTree->Insert( @1.first_line, TYPE_MULTIPLICATION_EXPRESSION, $1, $3 ); }
		| multiplicative_expression TOKEN_DIVISION unary_expression
															{ $$ = g_pSynTree->Insert( @1.first_line, TYPE_DIVISION_EXPRESSION, $1, $3 ); }
		| multiplicative_expression TOKEN_REMINDER unary_expression
															{ $$ = g_pSynTree->Insert( @1.first_line, TYPE_REMINDER_EXPRESSION, $1, $3 ); }
		;

unary_expression
		: postfix_expression								{ $$ = $1; }
		| TOKEN_INCREMENT variable %prec PREFIXINCREMENT
															{ $$ = g_pSynTree->Insert( @1.first_line, $2, TYPE_PREFIXINCREMENT ); }
		| TOKEN_DECREMENT variable %prec PREFIXDECREMENT
															{ $$ = g_pSynTree->Insert( @1.first_line, $2, TYPE_PREFIXDECREMENT ); }
		| TOKEN_NOT unary_expression						{ $$ = g_pSynTree->Insert( @1.first_line, TYPE_NOT_EXPRESSION, $2 ); }
		;

postfix_expression
		: primary_expression								{ $$ = $1; }
		| variable TOKEN_LEFTBRACE expression TOKEN_RIGHTBRACE
															{ $$ = g_pSynTree->Insert( @1.first_line, $1, TYPE_ARRAY_INDEXING, $3 ); }
		| TOKEN_ID TOKEN_LEFTPARENTHESIS expression TOKEN_RIGHTPARENTHESIS
															{ 
																int symbolID = g_pSymbolTable->FindFunc( $1, GetFuncCallType( $3 ) );
																if( symbolID == 0 )
																	ErrorMessage2( "Error(line %d) : 지정한 함수를 찾을 수 없습니다.\n", @1.first_line );
																$$ = g_pSynTree->Insert( @1.first_line, symbolID, TYPE_FUNCTION_CALL, $3 ); 
															}
		| TOKEN_ID TOKEN_LEFTPARENTHESIS TOKEN_RIGHTPARENTHESIS
															{ 
																int symbolID = g_pSymbolTable->FindFunc( $1, 0 );
																if( symbolID == 0 )
																	ErrorMessage2( "Error(line %d) : 지정한 함수를 찾을 수 없습니다.\n", @1.first_line );
																$$ = g_pSynTree->Insert( @1.first_line, symbolID, TYPE_FUNCTION_CALL ); 
															}
		| variable TOKEN_INCREMENT							{ $$ = g_pSynTree->Insert( @1.first_line, $1, TYPE_POSTFIXINCREMENT ); }
		| variable TOKEN_DECREMENT							{ $$ = g_pSynTree->Insert( @1.first_line, $1, TYPE_POSTFIXDECREMENT ); }
		;

primary_expression
		: constant_expression								{ $$ = $1; }
		| TOKEN_LEFTPARENTHESIS expression TOKEN_RIGHTPARENTHESIS
															{ $$ = g_pSynTree->Insert( @1.first_line, TYPE_EXPRESSION, $2 ); }
		| variable											{ $$ = g_pSynTree->Insert( @1.first_line, $1, TYPE_VARIABLE ); }
		;





















%%
/* ------------------------------------------------------------------
   Additional code (again copied verbatim to the output file)
   ------------------------------------------------------------------ */

eDataType	GetType( SNode * pNode )
{
	switch( pNode->m_eType )
	{
	case TYPE_ARGUMENT_DECLARATION :
			
			return GetType( pNode->m_ArrPtrChilds[0] );

	case TYPE_SPECIFIER_INT :
			
			return T_INT;

	case TYPE_SPECIFIER_FLOAT :
			
			return T_FLOAT;

	case TYPE_SPECIFIER_BOOL :

			return T_BOOL;

	case TYPE_SPECIFIER_STRING :

			return T_STRING;
	}
	return T_VOID;
}

SFuncType	GetFunctionType( eDataType retType, int nodeID )
{
	SNode * pNode = (SNode*)nodeID;

	SFuncType FuncType;
	
	int i = 0;

	while( pNode != NULL )
	{
		if( pNode->m_eType == TYPE_ARGUMENT_DECLARATION_LIST )
		{
			FuncType.SetArgType( i++, GetType( pNode->m_ArrPtrChilds[0] ) );
			pNode = pNode->m_ArrPtrChilds[1];
		}
		else																//TYPE_ARGUMENT_DECLARATION나 TYPE_SPECIFIER_XXX 계열일 때
		{																	
			FuncType.SetArgType( i++, GetType( pNode ) );
			break;
		}
	}

	assert( i <= 8 );

	FuncType.SetReturnType( retType );

	return FuncType;
}

SFuncType	GetFunctionType( int nodeID )
{
	return GetFunctionType( g_pSymbolTable->GetCurrentType(), nodeID );
}

SFuncType	GetFunctionType( int nodeID1, int nodeID2 )
{
	SNode * pNode = (SNode*)nodeID1;
	if( pNode != NULL )
	{
		return GetFunctionType( GetType( pNode ), nodeID2 );
	}
	return 0;
}

SFuncType	GetFuncCallType( int nodeID )
{
	SNode * pNode = (SNode*)nodeID;

	SFuncType FuncType;

	int i = 0;

	while( pNode != NULL )
	{
		if( pNode->m_eType == TYPE_EXPRESSION_LIST )
		{
			FuncType.SetArgType( i++, pNode->m_ArrPtrChilds[0]->m_eReturnType );
			pNode = pNode->m_ArrPtrChilds[1];
		}
		else
		{
			FuncType.SetArgType( i++, pNode->m_eReturnType );
			break;	
		}
	}

	assert( i <= 8 );

	FuncType.SetReturnType( T_VOID );

	return FuncType;
}

int			CountArrayElements( int nodeID )
{
	if( nodeID == 0 )
		return 0;

	SNode * pNode = (SNode*)nodeID;
	//initializer_list

	int i = 0;
	
	while( pNode != NULL )
	{
		if( pNode->m_eType == TYPE_INITIALIZER_LIST )
		{
			i++;
			pNode = pNode->m_ArrPtrChilds[1];
		}
		else
		{
			i++;
			break;
		}	
	}

	return i;
}

int			GetConstantValue( int nodeID )
{
	//constant expression
	SNode * pNode = (SNode*)nodeID;

	if( pNode->m_eType == TYPE_CONSTANT_EXPRESSION )
	{
		if( g_pSymbolTable->GetTypeOfConst( pNode->m_SymbolID ) == T_INT )
		{
			const char * pStr = g_pSymbolTable->GetNameOfConst( pNode->m_SymbolID );
			return strtol( pStr, NULL, 0 );
		}
	}

	return 0;
}
